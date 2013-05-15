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
* npd_mirror.c
*
*
* CREATOR:
*		chenb@autelan.com
*
* DESCRIPTION:
*		APIs used in NPD for logging process.
*
* DATE:
*		02/21/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.48 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>

#include "util/npd_list.h"
#include "sysdef/npd_sysdef.h"
#include "dbus/npd/npd_dbus_def.h"
#include "npd/nam/npd_amapi.h"
#include "npd/nbm/npd_bmapi.h"
#include "npd_log.h"
#include "npd_product.h"
#include "npd_eth_port.h"
#include "npd_c_slot.h"
#include "npd_acl_drv.h"
#include "npd_vlan.h"		/* sw data structure in npd*/
#include "npd_trunk.h"
#include "npd_fdb.h"
#include "npd_mirror.h"
#include "sysdef/returncode.h"

struct mirror_struct_s     **g_mirror = NULL;
unsigned int* source_port_for_bcm = NULL;

extern AclRuleList         **g_acl_rule;

/**********************************************************************************
 *  npd_mirror_init
 *
 *	DESCRIPTION:
 * 		this routine initialize all mirror mode
 *
 *	INPUT:
 *		NULL
 *		
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_mirror_init(void){

	int i =0;

	g_mirror = (struct mirror_struct_s**)malloc(sizeof(struct mirror_struct_s*)*MAX_MIRROR_PROFILE);
	if(NULL == g_mirror) {
		syslog_ax_mirror_err("mirror init alloc memory fail!!\n");
		return;		
	}

	for(;i<MAX_MIRROR_PROFILE;i++) {

		g_mirror[i] = NULL;
	}
	/*if(PRODUCT_ID_AX5K_E == PRODUCT_ID || PRODUCT_ID_AX5608== PRODUCT_ID){/*5612e not support*/
	/*	return;
	}*/
	if(!((productinfo.capbmp) & FUNCTION_MIRROR_VLAUE)){
		npd_syslog_dbg("do not support mirror initial!\n");
		return;
	}
	else if(((productinfo.capbmp) & FUNCTION_MIRROR_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_MIRROR_VLAUE)){
		npd_syslog_dbg("do not support mirror initial!\n");
		return;
	}
	else {
		nam_mirror_init();
	}
	return ;
}

/**********************************************************************************
 *  npd_mirror_profile_create
 *
 *	DESCRIPTION:
 * 		This method create mirror profile with global mirror profile id
 *
 *	INPUT:
 *		profileId - global mirror profile id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		MIRROR_RETURN_CODE_PROFILE_ID_OUTOFRANGE - mirror profile id is out of range
 *		MIRROR_RETURN_CODE_PROFILE_NODE_CREATED - found mirror profile has been created before
 *		MIRROR_RETURN_CODE_MALLOC_FAIL - memory allocation fail when create new mirror profile
 *		MIRROR_RETURN_CODE_SUCCESS - create mirror profile successfully
 *
 **********************************************************************************/
unsigned int npd_mirror_profile_create
(
	unsigned int profileId
)
{

	struct mirror_struct_s * mirrorNode = NULL;

	if(profileId >= MAX_MIRROR_PROFILE) {
		syslog_ax_mirror_err("create mirror profile %d error: profile id out of range\n",profileId);
		return MIRROR_RETURN_CODE_PROFILE_ID_OUTOFRANGE;
	}
	else if(NULL != (mirrorNode = g_mirror[profileId])) {
		syslog_ax_mirror_err("mirror profile %d exists when new creation!\n",profileId);
		return MIRROR_RETURN_CODE_PROFILE_NODE_CREATED;
	}
	
	mirrorNode = (struct mirror_struct_s*)malloc(sizeof(struct mirror_struct_s));
	if(NULL == mirrorNode) {
		syslog_ax_mirror_err("mirror profile %d creation memory fail!",profileId);
		return MIRROR_RETURN_CODE_MALLOC_FAIL;		
	}

	memset(mirrorNode,0,sizeof(struct mirror_struct_s));
	mirrorNode->in_eth_index = MIRROR_DEST_INPORT_DEFAULT;
	mirrorNode->eg_eth_index = MIRROR_DEST_EGPORT_DEFAULT;
	
	g_mirror[profileId] = mirrorNode;
	
	return MIRROR_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 *  npd_mirror_get_profile_node
 *
 *	DESCRIPTION:
 * 		this routine get destination port node
 *
 *	INPUT:
 *		profile			-- mirror profile
 *		
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NULL
 *		destNode -- struct mirror_struct_s*
 *		
 *
 **********************************************************************************/
struct mirror_struct_s* npd_mirror_get_profile_node
(
	unsigned int profile
)
{
	struct mirror_struct_s* destNode = NULL;

	if(profile >= MAX_MIRROR_PROFILE)
		return NULL;
	
	return (destNode = g_mirror[profile]);
	
}


/**********************************************************************************
 *  npd_mirror_destination_port_check
 *
 *	DESCRIPTION:
 * 		this routine check destination port node
 *
 *	INPUT:
 *		profile -- mirror profile
 *		eth_g_index -- destination port index
 *		direct			--  ingress or egress	
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_FALSE
 *		NPD_TRUE
 *		
 *
 **********************************************************************************/
int npd_mirror_destination_port_direct_check
(
	unsigned int profile,
	unsigned int direct
)
{
	struct mirror_struct_s* destNode = NULL;

	if(profile > MAX_MIRROR_PROFILE){
		 return NPD_FALSE;
	}
    if(direct > MIRROR_DIRECT_DEL_E){
		 return NPD_FALSE;
	}
	
	destNode = g_mirror[profile];
	
	if(NULL == destNode){
		return NPD_FALSE;
	}
    else {
        if( (destNode->in_eth_index == destNode->eg_eth_index)&&(MIRROR_DEST_INPORT_DEFAULT != destNode->eg_eth_index )){
             return NPD_TRUE;
		}
         else if((MIRROR_INGRESS_E == direct)&&(MIRROR_DEST_INPORT_DEFAULT != destNode->in_eth_index)){
            return NPD_TRUE;
		 }
		 else if((MIRROR_EGRESS_E == direct)&&(MIRROR_DEST_EGPORT_DEFAULT != destNode->eg_eth_index)){
            return NPD_TRUE;
		 }
		 else if((MIRROR_BIDIRECTION_E == direct)&&(MIRROR_DEST_INPORT_DEFAULT != destNode->in_eth_index)&&(MIRROR_DEST_EGPORT_DEFAULT != destNode->eg_eth_index)){
			 return NPD_TRUE;
		 }
		 else {
            return NPD_FALSE;
		 }
	}

}

/**********************************************************************************
 *  npd_mirror_destination_port_set
 *
 *	DESCRIPTION:
 * 		this routine set destination port node
 *
 *	INPUT:
 *		profile			-- mirror profile
 *		eth_g_index -- destination port index
 *		direct			--  ingress or egress	
 *
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		MIRROR_RETURN_CODE_DESTINATION_NODE_EXIST
 *		MIRROR_RETURN_CODE_ERROR
 *		MIRROR_RETURN_CODE_SUCCESS
 *		
 *
 **********************************************************************************/
int npd_mirror_destination_port_set
(
	unsigned int profile,
	unsigned int eth_g_index,
	MIRROR_DIRECTION_TYPE direct
)
{
   struct mirror_struct_s* destNode = NULL;

	if(profile > MAX_MIRROR_PROFILE) {
		return MIRROR_RETURN_CODE_ERROR;
	}

	if(eth_g_index > MAX_ETH_GLOBAL_INDEX) {
		return MIRROR_RETURN_CODE_ERROR;
	}

	if(SYSTEM_TYPE == IS_DISTRIBUTED)
	{
		
		destNode = g_mirror[profile];
		if(NULL != destNode) {
			/*change eth_g_index direct*/	
			if(MIRROR_INGRESS_E == direct)
			{
	           destNode->in_eth_index = eth_g_index;
			}
			else if(MIRROR_EGRESS_E == direct)
			{
	            destNode->eg_eth_index = eth_g_index;
			}
			else if(MIRROR_BIDIRECTION_E == direct){
				destNode->in_eth_index = eth_g_index;
				destNode->eg_eth_index = eth_g_index;
			}
			else 
			{
	            npd_syslog_err("Error direction input!\n");
				return MIRROR_RETURN_CODE_ERROR;
			}
			return MIRROR_RETURN_CODE_SUCCESS;
		}
		else 
		{
	         npd_syslog_err("Error!No destination node yet!\n");
			 return MIRROR_RETURN_CODE_DESTINATION_NODE_NOTEXIST;
		}
	}
	else
	{
		destNode = g_mirror[profile];
		if(NULL != destNode) {
			/*change eth_g_index direct*/	
			if(MIRROR_INGRESS_E == direct){
	           destNode->in_eth_index = eth_g_index;
			   if(eth_g_index == destNode->eg_eth_index)
			       destNode->eg_eth_index = MIRROR_DEST_EGPORT_DEFAULT;
			}
			else if(MIRROR_EGRESS_E == direct){
	            destNode->eg_eth_index = eth_g_index;
				if(eth_g_index == destNode->in_eth_index)
				   destNode->in_eth_index = MIRROR_DEST_INPORT_DEFAULT;
			}
			else if(MIRROR_BIDIRECTION_E == direct){
				destNode->in_eth_index = eth_g_index;
				destNode->eg_eth_index = eth_g_index;
			}
			else {
	            npd_syslog_err("Error direction input!\n");
				return MIRROR_RETURN_CODE_ERROR;
			}
			return MIRROR_RETURN_CODE_SUCCESS;
		}
		else {
	         npd_syslog_err("Error!No destination node yet!\n");
			 return MIRROR_RETURN_CODE_DESTINATION_NODE_NOTEXIST;
		}
	}
}

/**********************************************************************************
 *  npd_mirror_destination_node_member_check
 *
 *	DESCRIPTION:
 * 		this routine check destination node members
 *
 *	INPUT:
 *		profile			-- mirror profile
 *		direct			--  ingress or egress	
 *
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		MIRROR_RETURN_CODE_DESTINATION_NODE_NOTEXIST
 *		NPD_FALSE
 *		NPD_TRUE
 *		
 *
 **********************************************************************************/
int npd_mirror_destination_node_member_check
(
	unsigned int profile,
	unsigned int direct
)
{
	struct mirror_struct_s* destNode = NULL;
	mirror_src_port_list_s *srcPortList = NULL;
	struct mirror_src_port_node_s *srcPortNode = NULL;
	struct list_head *pos = NULL, *list = NULL;
	int i = 0;

	destNode = npd_mirror_get_profile_node(profile);
	if(NULL == destNode) {
		return MIRROR_RETURN_CODE_DESTINATION_NODE_NOTEXIST;
	}
	else {

		if(MIRROR_INGRESS_E == direct)
		{
		   for(i = 0; i < MIRROR_FUNC_MAX; i++) 
		   {
			 if(destNode->func[i]!= NULL)
			 {
                if(MIRROR_FUNC_PORT == i)
				{
                   srcPortList = destNode->func[i];
				   if((NULL != srcPortList)&&(srcPortList->count > 0))
				   {
						list = &(srcPortList->list);
						__list_for_each(pos,list) 
						{
							srcPortNode = list_entry(pos,struct mirror_src_port_node_s,list);
							if((MIRROR_INGRESS_E == srcPortNode->direct) || (MIRROR_BIDIRECTION_E == srcPortNode->direct))
							{
	                            return NPD_TRUE;
							}
						}
				   	}
				}
				else
				{
			         return NPD_TRUE;					
				}				
			 }  
		   } 
		}
		else if(MIRROR_EGRESS_E == direct)
		{
             srcPortList = destNode->func[MIRROR_FUNC_PORT];
 		     if((NULL != srcPortList)&&(srcPortList->count > 0)) 
			 {
 				list = &(srcPortList->list);
 				__list_for_each(pos,list) 
				{
 					srcPortNode = list_entry(pos,struct mirror_src_port_node_s,list);
 					if((MIRROR_EGRESS_E == srcPortNode->direct) || (MIRROR_BIDIRECTION_E == srcPortNode->direct))
					{
                         return NPD_TRUE;
 					}
 				}
 		   	}
		}
		else if(MIRROR_BIDIRECTION_E == direct)
		{
			for(i = 0; i < MIRROR_FUNC_MAX; i++) 
			{
				if(destNode->func[i]!= NULL)
				{
					return NPD_TRUE;
				}
		    } 
	    }
	}	
	return NPD_FALSE;
}



/**********************************************************************************
 *  npd_mirror_destination_node_port_get
 *
 *	DESCRIPTION:
 * 		this routine check destination node members
 *
 *	INPUT:
 *		profile			-- mirror profile
 *		direct			--  ingress or egress	
 *
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		MIRROR_RETURN_CODE_DESTINATION_NODE_NOTEXIST
 *		NPD_FALSE
 *		NPD_TRUE
 *		
 *
 **********************************************************************************/
int npd_mirror_destination_node_port_get
(
	unsigned int profile,
	unsigned int direct,
	unsigned int *eth_g_index
)
{
	struct mirror_struct_s* destNode = NULL;

	destNode = npd_mirror_get_profile_node(profile);
	if(NULL == destNode) {
		return MIRROR_RETURN_CODE_DESTINATION_NODE_NOTEXIST;
	}
	else {
        if(MIRROR_BIDIRECTION_E == direct){
           if((destNode->in_eth_index == destNode->eg_eth_index)&&(MIRROR_DEST_INPORT_DEFAULT != destNode->in_eth_index)){
              *eth_g_index  = destNode->in_eth_index;
		   }
		   else {
               return MIRROR_RETURN_CODE_ERROR;
		   }
		}
		else if(MIRROR_INGRESS_E == direct){
           if(MIRROR_DEST_INPORT_DEFAULT != destNode->in_eth_index){
               *eth_g_index  = destNode->in_eth_index;
		   }
		  else {
               return MIRROR_RETURN_CODE_ERROR;
		   }
		}
		else if(MIRROR_EGRESS_E == direct){
           if(MIRROR_DEST_INPORT_DEFAULT != destNode->eg_eth_index){
               *eth_g_index  = destNode->eg_eth_index;
		   }
		  else {
               return MIRROR_RETURN_CODE_ERROR;
		   }
		}
	}
	return MIRROR_RETURN_CODE_SUCCESS;
}


/**********************************************************************************
 *  npd_mirror_profile_config_save
 *
 *	DESCRIPTION:
 * 		this routine save destination port node
 *
 *	INPUT:
 *		showStr -- buf
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		MIRROR_RETURN_CODE_DESTINATION_NODE_NOTEXIST
 *		MIRROR_RETURN_CODE_ERROR
 *		MIRROR_RETURN_CODE_SUCCESS
 *		
 *
 **********************************************************************************/
void npd_mirror_profile_config_save
(
	char* showStr
)
{
	char* pos = showStr;
	unsigned int totalLen = 0;
	int i = 0;
	unsigned char enter_node = 0;/* need enter mirror config node or not*/
	char showBuf[2048] = {0},*curPos = NULL;
	unsigned int curLen = 0;

	if(NULL == showStr)
		return ;

	for (i = 0 ; i < MAX_MIRROR_PROFILE; i++ ) {
			unsigned int slot_no = 0, slot_index= 0, port_no = 0,port_index =0;
			struct mirror_struct_s* destNode = NULL;
			mirror_src_port_list_s* srcPortList = NULL;
			mirror_src_fdb_list_s* srcFdbList = NULL;
			mirror_src_vlan_list_s* srcVlanList = NULL;
			mirror_src_policy_list_s* srcPolicyList = NULL;
			
			destNode = g_mirror[i];
			curPos = showBuf;
			curLen = 0;
			/*next time to enter,reset enter_node */
			enter_node = 0;
			if(NULL != destNode) {
				/* bi-directional destination port, Assure destination port is legal!!!*/
				if((destNode->in_eth_index == destNode->eg_eth_index) && 
					(MIRROR_DEST_INPORT_DEFAULT != destNode->in_eth_index)) {
					slot_index		= SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(destNode->in_eth_index);
					slot_no 		= CHASSIS_SLOT_INDEX2NO(slot_index);
					if(CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
						enter_node = 1;
						port_index		= ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(destNode->in_eth_index);
						port_no 	= ETH_LOCAL_INDEX2NO(slot_index,port_index);
						
						curLen += sprintf(curPos," destination-port %d/%d %s\n",slot_no,port_no, "bidirection");
						curPos = showBuf + curLen;
					}
					else {
						syslog_ax_mirror_err("mirror show running bidirect dest has illegal slot %d\n",slot_no);
					}

				}
				else {/* uni-directional destination port*/
					if(MIRROR_DEST_INPORT_DEFAULT != destNode->in_eth_index){
						slot_index		= SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(destNode->in_eth_index);
						slot_no 		= CHASSIS_SLOT_INDEX2NO(slot_index);
						if(CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
							enter_node = 1;
							port_index	= ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(destNode->in_eth_index);
							port_no 	= ETH_LOCAL_INDEX2NO(slot_index,port_index);
							
							curLen += sprintf(curPos," destination-port %d/%d %s\n",slot_no,port_no, "ingress");
							curPos = showBuf + curLen;
						}
						else {
							syslog_ax_mirror_err("mirror show running ingress dest has illegal slot %d\n",slot_no);
						}	
					}
					if(MIRROR_DEST_EGPORT_DEFAULT != destNode->eg_eth_index){
						enter_node = 1;					
						slot_index	= SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(destNode->eg_eth_index);
						slot_no 	= CHASSIS_SLOT_INDEX2NO(slot_index);
						if(CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
							port_index	= ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(destNode->eg_eth_index);
							port_no 	= ETH_LOCAL_INDEX2NO(slot_index,port_index);

							curLen += sprintf(curPos," destination-port %d/%d %s\n",slot_no,port_no, "egress");
							curPos = showBuf + curLen;
						}				
						else {
							syslog_ax_mirror_err("mirror show running egress dest has illegal slot %d\n",slot_no);
						}	
					}
				}
				
				if(enter_node) {
					if(0 == i){
						totalLen += sprintf(pos,"config mirror-profile \n");
						pos = showStr + totalLen;
					}
					else{
						totalLen += sprintf(pos,"config mirror-profile %d\n",i);
						pos = showStr + totalLen;
					}
					totalLen += sprintf(pos,"%s",showBuf);
					pos = showStr + totalLen;
				}
				
				if(NULL != (srcPortList = destNode->func[MIRROR_FUNC_PORT])) {
					npd_mirror_src_port_cfg_save(srcPortList,&pos,&totalLen,&enter_node);
				}
				
				if(NULL != (srcVlanList = destNode->func[MIRROR_FUNC_VLAN])) {
					npd_mirror_src_vlan_cfg_save(srcVlanList,&pos,&totalLen,&enter_node);
				}
				
				if(NULL != (srcFdbList = destNode->func[MIRROR_FUNC_FDB])) {
					npd_mirror_src_fdb_cfg_save(srcFdbList,&pos,&totalLen,&enter_node);
				}
				
				if(NULL != (srcPolicyList = destNode->func[MIRROR_FUNC_POLICY])) {
					npd_mirror_src_policy_cfg_save(srcPolicyList,&pos,&totalLen,&enter_node);
				}

				if(enter_node) {
					totalLen += sprintf(pos,"exit\n");
					pos = showStr + totalLen;
				}
			}
	}
	return ;
}

/**********************************************************************************
 *  npd_mirror_src_port_check
 *
 *	DESCRIPTION:
 * 		this routine check src port node
 *
 *	INPUT:
 *		profile					--  mirror profile
 *		src_eth_g-index  --  source port index
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_FALSE
 *		NPD_TRUE
 *		
 *
 **********************************************************************************/
int npd_mirror_src_port_check
(
	unsigned int profile,
	unsigned int direct,
	unsigned int src_eth_g_index
)
{
	struct mirror_struct_s* destNode = NULL;
	mirror_src_port_list_s* srcPortList = NULL;
	struct mirror_src_port_node_s* srcPortNode = NULL;
	struct list_head	*pos = NULL,*list = NULL;

	destNode = npd_mirror_get_profile_node(profile);
	if(NULL == destNode) {
		return MIRROR_RETURN_CODE_DESTINATION_NODE_NOTEXIST;
	}
	srcPortList = destNode->func[MIRROR_FUNC_PORT];
	if(NULL == srcPortList) {
	   return NPD_FALSE;
	}
	if(srcPortList->count > 0) {
		list = &(srcPortList->list);
		__list_for_each(pos,list) {
			srcPortNode = list_entry(pos,struct mirror_src_port_node_s,list);
			if(src_eth_g_index == srcPortNode->eth_g_index) {
				#if 0
				if((direct == srcPortNode->direct)||(MIRROR_BIDIRECTION_E == srcPortNode->direct)){
				   return NPD_TRUE;
				}
				#endif
				if(direct == srcPortNode->direct)
				{
				   return NPD_TRUE;
				}
			}
		}
	}

	return NPD_FALSE;
}

/**********************************************************************************
 *  npd_mirror_src_port_get
 *
 *	DESCRIPTION:
 * 		this routine check src port node
 *
 *	INPUT:
 *		profile					--  mirror profile
 *		src_eth_g-index  --  source port index
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_FALSE
 *		NPD_TRUE
 *		
 *
 **********************************************************************************/
int npd_mirror_src_port_get
(
	unsigned int profile,
	unsigned int direct,
	unsigned int* src_eth_g_index,
	unsigned int* src_count
)
{
	struct mirror_struct_s* destNode = NULL;
	mirror_src_port_list_s* srcPortList = NULL;
	struct mirror_src_port_node_s* srcPortNode = NULL;
	struct list_head	*pos = NULL,*list = NULL;
	unsigned int i =0;
	memset(src_eth_g_index,0,(*src_count)*sizeof(unsigned int));
    *src_count = 0;
	destNode = npd_mirror_get_profile_node(profile);
	if(NULL == destNode) 
	{
		return MIRROR_RETURN_CODE_DESTINATION_NODE_NOTEXIST;
	}
	srcPortList = destNode->func[MIRROR_FUNC_PORT];
	if(NULL == srcPortList) 
	{
	   return MIRROR_RETURN_CODE_ERROR;
	}
	if(srcPortList->count > 0) 
	{
		list = &(srcPortList->list);
		__list_for_each(pos,list) 
		{
			srcPortNode = list_entry(pos,struct mirror_src_port_node_s,list);
			if(direct == srcPortNode->direct)
			{
				src_eth_g_index[i++] = srcPortNode->eth_g_index;
				*src_count++;
			}
		}
	}

	return MIRROR_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 *  npd_mirror_src_port_create
 *
 *	DESCRIPTION:
 * 		this routine create src port node
 *
 *	INPUT:
 *		profile					--  mirror profile
 *		src_eth_g-index  --  source port index
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		MIRROR_RETURN_CODE_ERROR
 *		MIRROR_RETURN_CODE_SUCCESS
 *		
 *
 **********************************************************************************/
int npd_mirror_src_port_create
(
	unsigned int profile,
	unsigned int direct,
	unsigned int src_eth_g_index
)
{
	struct mirror_struct_s* destNode = NULL;
	mirror_src_port_list_s* srcPortList = NULL;
	struct mirror_src_port_node_s* srcPortNode = NULL;
	struct list_head	*pos = NULL,*list = NULL;

	destNode = npd_mirror_get_profile_node(profile);
	if(NULL == destNode) {
		 return MIRROR_RETURN_CODE_ERROR;
	}
    srcPortList = destNode->func[MIRROR_FUNC_PORT];
    if(NULL == srcPortList) {
 	   srcPortList = (mirror_src_port_list_s*)malloc(sizeof(mirror_src_port_list_s)); 
 	   if(NULL == srcPortList) {
 		   syslog_ax_mirror_err("mirror port-source alloc list memory fail\n");
 		   return MIRROR_RETURN_CODE_ERROR;
 	   }
 	   memset(srcPortList,0,sizeof(mirror_src_port_list_s));
 	   INIT_LIST_HEAD(&(srcPortList->list));
 	   destNode->func[MIRROR_FUNC_PORT] = srcPortList;
    } 
	
    if(NPD_TRUE == npd_mirror_src_port_check(profile,direct,src_eth_g_index)) {
 	   return MIRROR_RETURN_CODE_SRC_PORT_EXIST;	/*if has already exist, do nothing:)*/
    }
	/*check if the same eth port exists*/
    if(srcPortList->count > 0) {
		list = &(srcPortList->list);
		__list_for_each(pos,list) {
			srcPortNode = list_entry(pos,struct mirror_src_port_node_s,list);
			if((src_eth_g_index == srcPortNode->eth_g_index)) {/*if the eth_g_index change the direct directly*/
                srcPortNode->direct = direct;
				return MIRROR_RETURN_CODE_SUCCESS;
			}
		}
	}
    srcPortNode = (struct mirror_src_port_node_s *)malloc(sizeof(struct mirror_src_port_node_s));
    if(NULL == srcPortNode) {
 	   syslog_ax_mirror_err("mirror port-source alloc node memory fail\n");
 	   return MIRROR_RETURN_CODE_ERROR;
    }
    
    srcPortNode->eth_g_index = src_eth_g_index;
	srcPortNode->direct = direct;
    list_add(&(srcPortNode->list), &(srcPortList->list));
    srcPortList->count++;
 	destNode->func[MIRROR_FUNC_PORT] = srcPortList;
    return MIRROR_RETURN_CODE_SUCCESS;

	
}

/**********************************************************************************
 *  npd_mirror_src_port_remove
 *
 *	DESCRIPTION:
 * 		this routine remov src port node
 *
 *	INPUT:
 *		profile					--  mirror profile
 *		src_eth_g-index  --  source port index
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		MIRROR_RETURN_CODE_ERROR
 *		MIRROR_RETURN_CODE_SUCCESS
 *		
 *
 **********************************************************************************/
int npd_mirror_src_port_remove
(
	unsigned int profile,
	unsigned int direct,
	unsigned int src_eth_g_index
)
{
	  struct mirror_struct_s* destNode = NULL;
	   mirror_src_port_list_s* srcPortList = NULL;
	   struct mirror_src_port_node_s* srcPortNode = NULL;
	   struct list_head    *pos = NULL,*list = NULL;
   
	   destNode = npd_mirror_get_profile_node(profile);
	   if(NULL == destNode) {
		   return MIRROR_RETURN_CODE_ERROR;
	   }
	   srcPortList = destNode->func[MIRROR_FUNC_PORT];
	   if(NULL == srcPortList) {
		  return NPD_FALSE;
	   }
	   if(srcPortList->count > 0) {
		   list = &(srcPortList->list);
		   __list_for_each(pos,list) {
			   srcPortNode = list_entry(pos,struct mirror_src_port_node_s,list);
			   if(NULL != srcPortNode) {
				   if(src_eth_g_index == srcPortNode->eth_g_index) {
				   	 npd_syslog_dbg("src_eth_g_index %d,direct %d\n",srcPortNode->eth_g_index,srcPortNode->direct);
				   	  if(srcPortNode->direct == direct){/*if direct is the same,delete the node*/ 
						  __list_del(srcPortNode->list.prev,srcPortNode->list.next);
						  free(srcPortNode);
						  if(srcPortList->count > 0) {
							  srcPortList->count--;
						  }
					  }
					  else if((MIRROR_BIDIRECTION_E == srcPortNode->direct)&&(MIRROR_BIDIRECTION_E != direct)){
					  	  /*if the node direct is bidirection, set the node direct to the opposite direction */
                          srcPortNode->direct = (MIRROR_INGRESS_E == direct) ? MIRROR_EGRESS_E : MIRROR_INGRESS_E ;
					  }
				   }
			   }
		   }
	   }
   
	   if(0 == srcPortList->count) {
		   free(srcPortList);
		   destNode->func[MIRROR_FUNC_PORT] = NULL;
	   }
   
	   return MIRROR_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 *  npd_mirror_src_port_cfg_save
 *
 *	DESCRIPTION:
 * 		this routine save all src port configuration
 *
 *	INPUT:
 *		 srcPortList -- Port list head
 *		currentPos -- first no used char addr
 *		totalLen      -- used buf len
 *		entered  - need to enter mirror node or not( 1 - no need re-enter, 0 - nee enter)
 *
 *	OUTPUT:
 *		entered - does mirror node entered or not
 *
 * 	RETURN:
 *		NPD_FALSE
 *		NPD_TRUE
 *		
 *
 **********************************************************************************/
void npd_mirror_src_port_cfg_save
(
	mirror_src_port_list_s* srcPortList,
	char** currentPos,
	unsigned int *totalLen,
	unsigned char *entered
)
{
	char* buf = *currentPos;
	struct mirror_src_port_node_s* srcPortNode = NULL;
	struct list_head *pos = NULL, *list = NULL;
	unsigned int curLen = 0;
	unsigned int eth_g_index = 0;
	MIRROR_DIRECTION_TYPE direct = MIRROR_DIRECT_DEL_E;
	unsigned char enter_node = 0;
	
	if((NULL == srcPortList) || (NULL == *currentPos)) {
		return ;
	}

	if(srcPortList->count > 0) {
		list = &(srcPortList->list);
		__list_for_each(pos,list) {
			srcPortNode = list_entry(pos,struct mirror_src_port_node_s,list);
			if((*totalLen + curLen + 30) > MIRROR_SAVE_CFG_MEM) {
				*currentPos = buf;
				*totalLen += curLen;
				return ;
			}
			
			if(NULL != srcPortNode) {
				eth_g_index = srcPortNode->eth_g_index;
				direct = srcPortNode->direct;
				if((MIRROR_SRC_PORT_INVALID == eth_g_index) ||
					(MIRROR_DIRECT_DEL_E == direct)) {
					syslog_ax_mirror_err("mirror port source with index %#x direction %d error\n",	\
							eth_g_index, direct);
				}
				else {
					unsigned char slot_no = 0, local_port_no = 0, slot_index = 0,port_index = 0;
					slot_index 		= SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
					slot_no			= CHASSIS_SLOT_INDEX2NO(slot_index);
					if(CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
						port_index		= ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
						local_port_no	= ETH_LOCAL_INDEX2NO(slot_index,port_index);
					
						if((0 == *entered) && !enter_node) {
							curLen += sprintf(buf, "config mirror-profile\n");
							buf = (*currentPos) + curLen;
							enter_node = 1;
						}
						curLen += sprintf(buf," mirror port-source %d/%d %s\n",	\
									slot_no,local_port_no, (MIRROR_BIDIRECTION_E == direct) ? "bidirection": \
									(MIRROR_EGRESS_E == direct) ? "egress":"ingress");
						buf = (*currentPos) + curLen;
					}				
					else {
						syslog_ax_mirror_err("mirror show running port source has illegal slot %d\n",slot_no);
					}	
				}
			}
		}		
	}

	*currentPos = buf;
	*totalLen += curLen;
	if(0 == *entered) { /* not entered before, mark current enter state( current may or may not enter)*/
		*entered = enter_node;
	}
	
	return ;
}

/**********************************************************************************
 *  npd_mirror_src_vlan_check
 *
 *	DESCRIPTION:
 * 		this routine check src vlan node
 *
 *	INPUT:
 *		profile					--  mirror profile
 *		vid  --  vlan id
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_FALSE
 *		NPD_TRUE
 *		
 *
 **********************************************************************************/
int npd_mirror_src_vlan_check
(
	unsigned int profile,
	unsigned short vid
)
{
	struct mirror_struct_s* destNode = NULL;
	mirror_src_vlan_list_s* srcVlanList = NULL;
	struct mirror_src_vlan_node_s* srcVlanNode = NULL;
	struct list_head	*pos = NULL,*list = NULL;

	destNode = npd_mirror_get_profile_node(profile);
	if(NULL == destNode) {
		return MIRROR_RETURN_CODE_DESTINATION_NODE_NOTEXIST;
	}

	srcVlanList = destNode->func[MIRROR_FUNC_VLAN];
	if(NULL == srcVlanList) {
		return NPD_FALSE;
	}

	if(srcVlanList->count > 0) {
		list = &(srcVlanList->list);
		__list_for_each(pos,list) {
			srcVlanNode = list_entry(pos,struct mirror_src_vlan_node_s,list);
			if(vid == srcVlanNode->vid) {
				return NPD_TRUE;
			}
		}	
	}

	return NPD_FALSE;
}

/**********************************************************************************
 *  npd_mirror_src_vlan_create
 *
 *	DESCRIPTION:
 * 		this routine create src vlan node
 *
 *	INPUT:
 *		profile					--  mirror profile
 *		vid 						--  vlan id 
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		MIRROR_RETURN_CODE_ERROR
 *		MIRROR_RETURN_CODE_SUCCESS
 *		
 *
 **********************************************************************************/
int npd_mirror_src_vlan_create
(
	unsigned int profile,
	unsigned short vid
)
{
	struct mirror_struct_s* destNode = NULL;
	mirror_src_vlan_list_s* srcVlanList = NULL;
	struct mirror_src_vlan_node_s* srcVlanNode = NULL;

	destNode = npd_mirror_get_profile_node(profile);
	if(NULL == destNode) {
		return MIRROR_RETURN_CODE_ERROR;
	}

	srcVlanList = destNode->func[MIRROR_FUNC_VLAN];
	if(NULL == srcVlanList) {
		srcVlanList = (mirror_src_vlan_list_s*)malloc(sizeof(mirror_src_vlan_list_s)); 
		if(NULL == srcVlanList) {
			syslog_ax_mirror_err("mirror vlan-source alloc list memory fail\n");
			return MIRROR_RETURN_CODE_ERROR;
		}
		memset(srcVlanList,0,sizeof(mirror_src_vlan_list_s));
		INIT_LIST_HEAD(&(srcVlanList->list));
		destNode->func[MIRROR_FUNC_VLAN] = srcVlanList;
	} 

	if(NPD_TRUE == npd_mirror_src_vlan_check(profile,vid)) {
		return MIRROR_RETURN_CODE_ERROR;
	}
	
	srcVlanNode = (struct mirror_src_vlan_node_s *)malloc(sizeof(struct mirror_src_vlan_node_s));
	if(NULL == srcVlanNode) {
		syslog_ax_mirror_err("mirror vlan-source alloc node memory fail\n");
		return MIRROR_RETURN_CODE_ERROR;
	}
	
	srcVlanNode->vid = vid;
	list_add(&(srcVlanNode->list), &(srcVlanList->list));
	srcVlanList->count++;
	destNode->func[MIRROR_FUNC_VLAN] = srcVlanList;

	return MIRROR_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 *  npd_mirror_src_port_remove
 *
 *	DESCRIPTION:
 * 		this routine remove srv vlan node
 *
 *	INPUT:
 *		profile					--  mirror profile
 *		vid  --  vlan id
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_FALSE
 *		NPD_TRUE
 *		
 *
 **********************************************************************************/
int npd_mirror_src_vlan_remove
(
	unsigned int profile,
	unsigned int vid
)
{
	struct mirror_struct_s* destNode = NULL;
	mirror_src_vlan_list_s* srcVlanList = NULL;
	struct mirror_src_vlan_node_s* srcVlanNode = NULL;
	struct list_head	*pos = NULL,*list = NULL;

	destNode = npd_mirror_get_profile_node(profile);
	if(NULL == destNode) {
		return MIRROR_RETURN_CODE_ERROR;
	}

	srcVlanList = destNode->func[MIRROR_FUNC_VLAN];
	if(NULL == srcVlanList) {
		return MIRROR_RETURN_CODE_ERROR;
	} 

	if(srcVlanList->count > 0) {
		list = &(srcVlanList->list);
		__list_for_each(pos,list) {
			srcVlanNode = list_entry(pos,struct mirror_src_vlan_node_s,list);
			if(NULL != srcVlanNode) {
				if(vid == srcVlanNode->vid) {
					__list_del(srcVlanNode->list.prev,srcVlanNode->list.next);
					free(srcVlanNode);
					if(srcVlanList->count > 0) {
						srcVlanList->count--;
					}
				}
			}
		}
	}

	if(0 == srcVlanList->count) {
		free(srcVlanList);
		destNode->func[MIRROR_FUNC_VLAN] = NULL;
	}

	return MIRROR_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 *  npd_mirror_src_all_vlan_destroy
 *
 *	DESCRIPTION:
 * 		this routine destroy all src vlan node
 *
 *	INPUT:
 *		profile					--  mirror profile
 *		
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_FALSE
 *		NPD_TRUE
 *		
 *
 **********************************************************************************/
int npd_mirror_src_all_vlan_destroy
(
	unsigned int profile
)
{
	struct mirror_struct_s* destNode = NULL;
	mirror_src_vlan_list_s* srcVlanList = NULL;
	struct mirror_src_vlan_node_s* srcVlanNode = NULL;
	struct list_head	*pos = NULL,*list = NULL;

	destNode = npd_mirror_get_profile_node(profile);
	if(NULL == destNode) {
		return MIRROR_RETURN_CODE_ERROR;
	}

	srcVlanList = destNode->func[MIRROR_FUNC_VLAN];
	if(NULL == srcVlanList) {
		return MIRROR_RETURN_CODE_ERROR;
	} 

	if(srcVlanList->count > 0) {
		list = &(srcVlanList->list);
		__list_for_each(pos,list) {
			srcVlanNode = list_entry(pos,struct mirror_src_vlan_node_s,list);
			if(NULL != srcVlanNode) {
				__list_del(srcVlanNode->list.prev,srcVlanNode->list.next);
				free(srcVlanNode);
				if(srcVlanList->count > 0) {
					srcVlanList->count--;
				}
			}
		}
	}

	if(0 == srcVlanList->count) {
		free(srcVlanList);
		destNode->func[MIRROR_FUNC_VLAN] = NULL;
	}

	return MIRROR_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 *  npd_mirror_src_vlans_cfg_save
 *
 *	DESCRIPTION:
 * 		this routine save all src vlan configuration
 *
 *	INPUT:
 *		srcVlanList -- vlan list head
 *		currentPos -- first no used char addr
 *		totalLen      -- used buf len
 *		entered  - need to enter mirror node or not( 1 - no need re-enter, 0 - nee enter)
 *
 *	OUTPUT:
 *		entered - does mirror node entered or not
 *		
 *
 * 	RETURN:
 *		NPD_FALSE
 *		NPD_TRUE
 *		
 *
 **********************************************************************************/
void npd_mirror_src_vlan_cfg_save
(
	mirror_src_vlan_list_s* srcVlanList,
	char** currentPos,
	unsigned int* totalLen,
	unsigned char *entered
)
{
	char* buf = *currentPos;
	struct mirror_src_vlan_node_s* srcVlanNode = NULL;
	struct list_head *pos = NULL, *list = NULL;
	unsigned int curLen = 0;
	unsigned char enter_node = 0;
	
	if((NULL == srcVlanList) || (NULL == *currentPos))
		return ;

	if(srcVlanList->count > 0) {
		list = &(srcVlanList->list);
		__list_for_each(pos,list) {
			srcVlanNode = list_entry(pos,struct mirror_src_vlan_node_s,list);
			if((*totalLen + curLen + 30) > MIRROR_SAVE_CFG_MEM) {
				*currentPos = buf;
				*totalLen += curLen; 
				return ;
			}
			
			if(NULL != srcVlanNode) {
				if((0 == *entered) && (!enter_node)) {
					curLen += sprintf(buf,"config mirror-profile\n");
					buf = (*currentPos) + curLen;
					enter_node = 1;
				}
				
				curLen += sprintf(buf," mirror vlan-source %d\n",srcVlanNode->vid);
				buf = (*currentPos) + curLen;
			}
		}			
	}

	*currentPos = buf;
	*totalLen += curLen;

	if(0 == *entered) { /* not entered before, mark current enter state( current may or may not enter)*/
		*entered = enter_node;
	}
	
	return ;
}

/**********************************************************************************
 *  npd_mirror_src_fdb_check
 *
 *	DESCRIPTION:
 * 		this routine check src fdb
 *
 *	INPUT:
 *		profile					--  mirror profile
 *		vid  --  vlan id
 *		mac -- ether address
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_FALSE
 *		NPD_TRUE
 *		
 *
 **********************************************************************************/
int npd_mirror_src_fdb_check
(
	unsigned int profile,
	unsigned short vid,
	unsigned char* mac,
	unsigned char slot_no,
	unsigned char port_no
)
{
	struct mirror_struct_s* destNode = NULL;
	mirror_src_fdb_list_s* srcFdbList = NULL;
	struct mirror_src_fdb_node_s* srcFdbNode = NULL;
	struct list_head	*pos = NULL,*list = NULL;

	destNode = npd_mirror_get_profile_node(profile);
	if(NULL == destNode) {
		return MIRROR_RETURN_CODE_DESTINATION_NODE_NOTEXIST;
	}

	srcFdbList = destNode->func[MIRROR_FUNC_FDB];
	if(NULL == srcFdbList) {
		return NPD_FALSE;
	}

	if(srcFdbList->count > 0) {
		list = &(srcFdbList->list);
		__list_for_each(pos,list) {
			srcFdbNode = list_entry(pos,struct mirror_src_fdb_node_s,list);
			if((vid == srcFdbNode->vid) && (0 == memcmp(srcFdbNode->mac,mac,MAC_ADDRESS_LEN))) {

				return NPD_TRUE;
			}
		}	
	}

	return NPD_FALSE;
}

/**********************************************************************************
 *  npd_mirror_src_fdb_create
 *
 *	DESCRIPTION:
 * 		this routine create src fdc  node
 *
 *	INPUT:
 *		profile					--  mirror profile
 *		vid 						--  vlan id 
 *		mac 						--  ether address
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		MIRROR_RETURN_CODE_ERROR
 *		MIRROR_RETURN_CODE_SUCCESS
 *		
 *
 **********************************************************************************/
int npd_mirror_src_fdb_create
(
	unsigned int profile,
	unsigned short vid,
	unsigned char* mac,
	unsigned char slot_no,
	unsigned char port_no
)
{
	struct mirror_struct_s* destNode = NULL;
	mirror_src_fdb_list_s* srcFdbList = NULL;
	struct mirror_src_fdb_node_s* srcFdbNode = NULL;

	destNode = npd_mirror_get_profile_node(profile);
	if(NULL == destNode) {
		return MIRROR_RETURN_CODE_ERROR;
	}

	srcFdbList = destNode->func[MIRROR_FUNC_FDB];
	if(NULL == srcFdbList) {
		srcFdbList = (mirror_src_fdb_list_s*)malloc(sizeof(mirror_src_fdb_list_s)); 
		if(NULL == srcFdbList) {
			syslog_ax_mirror_err("mirror fdb-source alloc list memory fail\n");
			return MIRROR_RETURN_CODE_ERROR;
		}
		memset(srcFdbList,0,sizeof(mirror_src_fdb_list_s));
		INIT_LIST_HEAD(&(srcFdbList->list));
		destNode->func[MIRROR_FUNC_FDB] = srcFdbList;
	} 

	if(NPD_TRUE == npd_mirror_src_fdb_check(profile,vid,mac,slot_no,port_no)) {
		return MIRROR_RETURN_CODE_ERROR;
	}
	
	srcFdbNode = (struct mirror_src_fdb_node_s *)malloc(sizeof(struct mirror_src_fdb_node_s));
	if(NULL == srcFdbNode) {
		syslog_ax_mirror_err("mirror fdb-source alloc node memory fail\n");
		return MIRROR_RETURN_CODE_ERROR;
	}
	
	srcFdbNode->vid = vid;
	memcpy(srcFdbNode->mac,mac,MAC_ADDRESS_LEN);
	srcFdbNode->slot_no = slot_no;
	srcFdbNode->port_no = port_no;
	list_add(&(srcFdbNode->list), &(srcFdbList->list));
	srcFdbList->count++;
	destNode->func[MIRROR_FUNC_FDB] = srcFdbList;
	
	return MIRROR_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 *  npd_mirror_src_fdb_remove
 *
 *	DESCRIPTION:
 * 		this routine remove src fdb node
 *
 *	INPUT:
 *		profile					--  mirror profile
 *		vid  --  vlan id
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_FALSE
 *		NPD_TRUE
 *		
 *
 **********************************************************************************/
int npd_mirror_src_fdb_remove
(
	unsigned int profile,
	unsigned short  vid,
	unsigned char*  mac,
	unsigned char slot_no,
	unsigned char port_no
)
{
	struct mirror_struct_s* destNode = NULL;
	mirror_src_fdb_list_s* srcFdbList = NULL;
	struct mirror_src_fdb_node_s* srcFdbNode = NULL;
	struct list_head	*pos = NULL,*list = NULL;

	destNode = npd_mirror_get_profile_node(profile);
	if(NULL == destNode) {
		return MIRROR_RETURN_CODE_ERROR;
	}

	srcFdbList = destNode->func[MIRROR_FUNC_FDB];
	if(NULL == srcFdbList) {
		return MIRROR_RETURN_CODE_ERROR;
	} 

	if(srcFdbList->count > 0) {
		list = &(srcFdbList->list);
		__list_for_each(pos,list) {
			srcFdbNode = list_entry(pos,struct mirror_src_fdb_node_s,list);
			if(NULL != srcFdbNode) {
				if((vid == srcFdbNode->vid) && (0 == memcmp(srcFdbNode->mac,mac,MAC_ADDRESS_LEN))&&(slot_no == srcFdbNode->slot_no)&&(port_no == srcFdbNode->port_no)) {
					__list_del(srcFdbNode->list.prev,srcFdbNode->list.next);
					free(srcFdbNode);
					if(srcFdbList->count > 0) {
						srcFdbList->count--;
					}
				}
			}
		}
	}

	if(0 == srcFdbList->count) {
		free(srcFdbList);
		destNode->func[MIRROR_FUNC_FDB] = NULL;
	}

	return MIRROR_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 *  npd_mirror_src_fdb_cfg_save
 *
 *	DESCRIPTION:
 * 		this routine save src fdb configuration node
 *
 *	INPUT:
 *		srcFdbList -- fdb list head
 *		currentPos -- first no used char addr
 *		totalLen      -- used buf len
 *		entered  - need to enter mirror node or not( 1 - no need re-enter, 0 - nee enter)
 *
 *	OUTPUT:
 *		entered - does mirror node entered or not
 *
 * 	RETURN:
 *		NPD_FALSE
 *		NPD_TRUE
 *
 **********************************************************************************/
void npd_mirror_src_fdb_cfg_save
(
	mirror_src_fdb_list_s* srcFdbList,
	char** currentPos,
	unsigned int* totalLen,
	unsigned char *entered
)
{
	char* buf = *currentPos;
	struct mirror_src_fdb_node_s* srcFdbNode = NULL;
	struct list_head *pos = NULL, *list = NULL;
	unsigned int curLen = 0;
	unsigned char enter_node = 0;
	
	if((NULL == srcFdbList) || (NULL == *currentPos))
		return ;

	if(srcFdbList->count > 0) {
		list = &(srcFdbList->list);
		__list_for_each(pos,list) {
			srcFdbNode = list_entry(pos,struct mirror_src_fdb_node_s,list);
			if((*totalLen + curLen + 50) > MIRROR_SAVE_CFG_MEM) {
				*currentPos = buf;
				*totalLen += curLen;
				return ;
			}
			
			if(NULL != srcFdbNode) {
				if((0 == *entered) && (!enter_node)) {
					curLen += sprintf(buf,"config mirror-profile\n");
					buf = (*currentPos) + curLen;
					enter_node = 1;
				}
				
				curLen += sprintf(buf," mirror fdb-source %02x:%02x:%02x:%02x:%02x:%02x %d %d/%d\n",		\
					srcFdbNode->mac[0],srcFdbNode->mac[1],		\
					srcFdbNode->mac[2],srcFdbNode->mac[3],		\
					srcFdbNode->mac[4],srcFdbNode->mac[5],       \
					srcFdbNode->vid,srcFdbNode->slot_no,srcFdbNode->port_no);
				buf = *currentPos + curLen;
			}
		}			
	}

	*currentPos = buf;
	*totalLen += curLen;

	if(0 == *entered) { /* not entered before, mark current enter state( current may or may not enter)*/
		*entered = enter_node;
	}

	return ;
}
/**********************************************************************************
 *  npd_mirror_acl_action_check
 *
 *	DESCRIPTION:
 * 		this routine check acl action
 *
 *	INPUT:
 *		ruleIndex
 *		
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
unsigned int npd_mirror_acl_action_check
(
	unsigned int ruleIndex
)
{
	unsigned int  actionT=0;

	if(NULL==g_acl_rule[ruleIndex])
		return MIRROR_RETURN_CODE_ERROR;
	else{
		actionT = g_acl_rule[ruleIndex]->ActionPtr.actionType;
		if(ACL_ACTION_TYPE_PERMIT==actionT)
			return MIRROR_RETURN_CODE_SUCCESS;
		else
			return MIRROR_RETURN_CODE_ERROR;
	}
	
	return MIRROR_RETURN_CODE_ERROR;

}

/**********************************************************************************
 *  npd_mirror_src_policy_cfg_save
 *
 *	DESCRIPTION:
 * 		this routine save src policy configuration node
 *
 *	INPUT:
 *		srcPolicyList - policy list head
 *		currentPos - first no used char addr
 *		totalLen      - used buf len
 *		entered  - need to enter mirror node or not( 1 - no need re-enter, 0 - nee enter)
 *
 *	OUTPUT:
 *		entered - does mirror node entered or not
 *
 * 	RETURN:
 *		NPD_FALSE
 *		NPD_TRUE
 *
 **********************************************************************************/
void npd_mirror_src_policy_cfg_save
(
	mirror_src_policy_list_s* srcPolicyList,
	char** currentPos,
	unsigned int* totalLen,
	unsigned char *entered
)
{
	char* buf = *currentPos;
	struct mirror_src_policy_node_s* srcPolicyNode = NULL;
	struct list_head *pos = NULL, *list = NULL;
	unsigned int curLen = 0;
	unsigned char enter_node = 0;
	
	if((NULL == srcPolicyList) || (NULL == *currentPos))
		return ;

	if(srcPolicyList->count > 0) {
		list = &(srcPolicyList->list);
		__list_for_each(pos,list) {
			srcPolicyNode = list_entry(pos,struct mirror_src_policy_node_s,list);
			if((*totalLen + curLen + 30) > MIRROR_SAVE_CFG_MEM) {
				*currentPos = buf;
				*totalLen += curLen;
				return ;
			}
			
			if(NULL != srcPolicyNode) {
				if((0 == *entered) && !enter_node) {
					curLen += sprintf(buf,"config mirror-profile\n");
					buf = (*currentPos) + curLen;
					enter_node = 1;
				}
				curLen += sprintf(buf," mirror policy-source %d\n",1+srcPolicyNode->ruleIndex);
				buf = *currentPos + curLen;
			}
		}			
	}

	*currentPos = buf;
	*totalLen += curLen;

	if(0 == *entered) { /* not entered before, mark current enter state( current may or may not enter)*/
		*entered = enter_node;
	}

	return ;
}


/**********************************************************************************
 *  npd_mirror_src_acl_check
 *
 *	DESCRIPTION:
 * 		this routine check src acl node
 *
 *	INPUT:
 *		profile					--  mirror profile
 *		ruleindex  --  
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_FALSE
 *		NPD_TRUE
 *		
 *
 **********************************************************************************/
int npd_mirror_src_acl_check
(
	unsigned int profile,
	unsigned int  ruleIndex
)
{
    struct mirror_struct_s* destNode = NULL;
	mirror_src_policy_list_s* srcAclList = NULL;
    struct mirror_src_policy_node_s  *srvAclNode=NULL;
	struct list_head	*pos = NULL,*list = NULL;

	destNode = npd_mirror_get_profile_node(profile);
	if(NULL == destNode) {
		return MIRROR_RETURN_CODE_DESTINATION_NODE_NOTEXIST;
	}
    /*policy only ingress node*/
	srcAclList = destNode->func[MIRROR_FUNC_POLICY];
	if(NULL == srcAclList) {
		return NPD_FALSE;
	}

	if(srcAclList->count > 0) {
		list = &(srcAclList->list);
		__list_for_each(pos,list) {
			srvAclNode = list_entry(pos,struct mirror_src_policy_node_s,list);
			if(ruleIndex == srvAclNode->ruleIndex) {
				return NPD_TRUE;
			}
		}	
	}

	return NPD_FALSE;
}
/**********************************************************************************
 *  npd_mirror_src_acl_create
 *
 *	DESCRIPTION:
 * 		 create src acl node
 *
 *	INPUT:
 *		profile					--  mirror profile
 *		ruleindex 				--  acl rule index 
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		MIRROR_RETURN_CODE_ERROR
 *		MIRROR_RETURN_CODE_SUCCESS
 *		
 *
 **********************************************************************************/
int npd_mirror_src_acl_create
(
	unsigned int profile,
	unsigned int ruleIndex
)
{
	struct mirror_struct_s* destNode = NULL;
	mirror_src_policy_list_s* srcAclList = NULL;
    struct mirror_src_policy_node_s  *srvAclNode=NULL;


	destNode = npd_mirror_get_profile_node(profile);
	if(NULL == destNode) {
		return MIRROR_RETURN_CODE_ERROR;
	}

	srcAclList = destNode->func[MIRROR_FUNC_POLICY];
	if(NULL == srcAclList) {
		srcAclList = (mirror_src_policy_list_s*)malloc(sizeof(mirror_src_policy_list_s)); 
		if(NULL == srcAclList) {
			syslog_ax_mirror_err("npd mirror src acl malloc error \n");
			return MIRROR_RETURN_CODE_ERROR;
		}
		memset(srcAclList,0,sizeof(mirror_src_policy_list_s));
		INIT_LIST_HEAD(&(srcAclList->list));
		destNode->func[MIRROR_FUNC_POLICY] = srcAclList;
	} 

	if(NPD_TRUE == npd_mirror_src_acl_check(profile,ruleIndex)) {
		return MIRROR_RETURN_CODE_ERROR;
	}
	
	srvAclNode = (struct mirror_src_policy_node_s *)malloc(sizeof(struct mirror_src_policy_node_s));
	if(NULL == srvAclNode) {
		syslog_ax_mirror_err("npd mirror src acl node malloc error \n");
		return MIRROR_RETURN_CODE_ERROR;
	}
	
	srvAclNode->ruleIndex = ruleIndex;
	list_add(&(srvAclNode->list), &(srcAclList->list));
	srcAclList->count++;
	destNode->func[MIRROR_FUNC_POLICY] = srcAclList;

	return MIRROR_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 *  npd_mirror_src_acl_remove
 *
 *	DESCRIPTION:
 * 		this routine remove srv acl node
 *
 *	INPUT:
 *		profile					--  mirror profile
 *		ruleIndex 				 --  
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_FALSE
 *		NPD_TRUE
 *		
 *
 **********************************************************************************/
int npd_mirror_src_acl_remove
(
	unsigned int profile,
	unsigned int ruleIndex
)
{
	
	struct mirror_struct_s* destNode = NULL;
	mirror_src_policy_list_s* srcAclList = NULL;
	struct mirror_src_policy_node_s  *srvAclNode=NULL;
	struct list_head	*pos = NULL,*list = NULL;

	destNode = npd_mirror_get_profile_node(profile);
	if(NULL == destNode) {
		return MIRROR_RETURN_CODE_ERROR;
	}

	srcAclList = destNode->func[MIRROR_FUNC_POLICY];
	if(NULL == srcAclList) {
		return MIRROR_RETURN_CODE_ERROR;
	} 

	if(srcAclList->count > 0) {
		list = &(srcAclList->list);
		__list_for_each(pos,list) {
			srvAclNode = list_entry(pos,struct mirror_src_policy_node_s,list);
			if(NULL != srvAclNode) {
				if(ruleIndex == srvAclNode->ruleIndex) {
					__list_del(srvAclNode->list.prev,srvAclNode->list.next);
					free(srvAclNode);
					if(srcAclList->count > 0) {
						srcAclList->count--;
					}
				}
			}
		}
	}

	if(0 == srcAclList->count) {
		free(srcAclList);
		destNode->func[MIRROR_FUNC_POLICY] = NULL;
	}

	return MIRROR_RETURN_CODE_SUCCESS;
}


DBusMessage * npd_dbus_mirror_config(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter= {0};
	DBusError			err;   		
	unsigned int	    ret = 0;
	unsigned int 	profile = DEFAULT_MIRROR_PROFILE;

	if(!((productinfo.capbmp) & FUNCTION_MIRROR_VLAUE))
	{
		npd_syslog_dbg("do not support mirror!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_MIRROR_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_MIRROR_VLAUE))
	{
		npd_syslog_dbg("do not support mirror!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if((BOARD_TYPE == BOARD_TYPE_AX71_CRSMU) || (BOARD_TYPE == BOARD_TYPE_AX81_SMU))
	{
		npd_syslog_dbg("do not support mirror!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else 
	{
		dbus_error_init(&err);

		if (!(dbus_message_get_args ( msg, &err,
									 DBUS_TYPE_UINT32,&profile,
									 DBUS_TYPE_INVALID))) 
		{
			syslog_ax_mirror_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) 
			{
				syslog_ax_mirror_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}	
		if(MIRROR_RETURN_CODE_SUCCESS != profile_check(profile))
		{
	       syslog_ax_mirror_err("profile check error not supported value %d!\n",profile);
		   ret = MIRROR_RETURN_CODE_NOT_SUPPORT_PROFILE;
		}
		else
		{
			ret = MIRROR_RETURN_CODE_BAD_PARAM;
			if(NULL != npd_mirror_get_profile_node(profile))
			{/*if the node has already exist*/
		       ret = MIRROR_RETURN_CODE_SUCCESS;
			}
		    else 
			{/*else if not exist ,create first!*/
				ret = npd_mirror_profile_create(profile);
			}
		}
	}
	
	dbus_error_init(&err);
	
	reply = dbus_message_new_method_return(msg);	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&profile);
	return reply;
}

DBusMessage * npd_dbus_mirror_create_dest_port(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter= {0};
	DBusError			err;   		
	unsigned char       slot_index = 0,port_index = 0,slot_no = 0,port_no = 0,devNum = 0,portNum = 0,virPortId = 0;
	unsigned int	    eth_g_index=0, ret = MIRROR_RETURN_CODE_SUCCESS,profile = DEFAULT_MIRROR_PROFILE;	 
	MIRROR_DIRECTION_TYPE direct = MIRROR_INGRESS_E;
	unsigned char analyzerrxDev = 0,analyzerrxPort = 0,analyzertxDev = 0,analyzertxPort = 0;
    mirror_src_port_list_s* srcPortList = NULL;
	mirror_src_vlan_list_s* srcVlanList = NULL;
	mirror_src_policy_list_s* srcPolicyList = NULL;
	mirror_src_fdb_list_s*   srcFdbList = NULL;
	struct mirror_src_port_node_s* srcPortNode = NULL;
	struct mirror_src_vlan_node_s* srcVlanNode = NULL;
	struct mirror_src_policy_node_s* srcPolicyNode = NULL;
	struct mirror_src_fdb_node_s*  srcFdbNode = NULL;
	struct list_head	*pos = NULL,*list = NULL;
    unsigned int groupNum = 0,exist_dir = 0;
	AclRuleList  *rulePtr=NULL;
	ETHERADDR macAddr;
	unsigned char *mac = NULL;

	if(!((productinfo.capbmp) & FUNCTION_MIRROR_VLAUE))
	{
		npd_syslog_dbg("do not support mirror!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_MIRROR_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_MIRROR_VLAUE))
	{
		npd_syslog_dbg("do not support mirror!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else 
	{
		memset(&macAddr,0,sizeof(ETHERADDR));
		dbus_error_init(&err);

		if (!(dbus_message_get_args ( msg, &err,
									 DBUS_TYPE_BYTE,&slot_no,
									 DBUS_TYPE_BYTE,&port_no,  
									 DBUS_TYPE_UINT32,&direct,
									 DBUS_TYPE_UINT32,&profile,
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

		ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
		if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) 
		{
			if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,port_no)) 
			{
				eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
				ret = MIRROR_RETURN_CODE_SUCCESS;
			}
		}

		if(MIRROR_RETURN_CODE_SUCCESS == ret) 
		{
			if(0 != npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortId))
			{
				ret = MIRROR_RETURN_CODE_ERROR;
			}
			else if(0 != (ret = nam_mirror_analyzer_port_get(profile,&analyzerrxDev,&analyzerrxPort,&analyzertxDev,&analyzertxPort)))
			{
	            npd_syslog_warning("Error to get the destination port!\n");
			}
			else if(0 != (ret = nam_mirror_analyzer_port_set(profile,devNum,virPortId,direct))) 
			{
				npd_syslog_err("Error to set the destination port!\n");
				ret = MIRROR_RETURN_CODE_DESTINATION_NODE_CREATE_FAIL;
			}
			else
			{
				ret = npd_mirror_nam_dest_port_set_for_bcm(profile,devNum,virPortId,direct);
				if(ret != MIRROR_RETURN_CODE_SUCCESS)
				{
					npd_syslog_err("Error to set the destination port!\n");
	                ret = MIRROR_RETURN_CODE_DESTINATION_NODE_CREATE_FAIL;
				}
			}
	        if(0 == ret)
			{
				if(SYSTEM_TYPE == IS_DISTRIBUTED)
				{
					if(0 != (ret = npd_mirror_destination_port_set(profile,eth_g_index,direct)))
					{
					   npd_syslog_err("Set the destination port failed!\n");
					   ret = MIRROR_RETURN_CODE_DESTINATION_NODE_CREATE_FAIL;
					}
				}
				else
				{
					if(0 != (ret = npd_mirror_destination_port_set(profile,eth_g_index,direct)))
					{
						   npd_syslog_err("Set the destination port failed!\n");
						   nam_mirror_analyzer_port_set(profile,analyzerrxDev,analyzerrxPort,MIRROR_INGRESS_E);
						   nam_mirror_analyzer_port_set(profile,analyzertxDev,analyzertxPort,MIRROR_EGRESS_E);
						   ret = MIRROR_RETURN_CODE_DESTINATION_NODE_CREATE_FAIL;
					}
					else 
					{
						/*check if any source port has no destination port, and delete them*/
						struct mirror_struct_s* destNode = NULL;
					    destNode = g_mirror[profile];
						if(MIRROR_DEST_EGPORT_DEFAULT == destNode->eg_eth_index)
						{
							if((NULL !=(srcPortList = destNode->func[MIRROR_FUNC_PORT]))&&(srcPortList->count > 0))
							{
								list = &(srcPortList->list);
							    __list_for_each(pos,list) 
								{
									srcPortNode = list_entry(pos,struct mirror_src_port_node_s,list);
									if((MIRROR_EGRESS_E == srcPortNode->direct)||(MIRROR_BIDIRECTION_E == srcPortNode->direct))
									{
										slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(srcPortNode->eth_g_index);
										slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
										port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(srcPortNode->eth_g_index);
										port_no = ETH_LOCAL_INDEX2NO(slot_index,srcPortNode->eth_g_index);
							            npd_get_devport_by_global_index(srcPortNode->eth_g_index, &devNum, &portNum);
					                    if(0 != nam_mirror_port_set(devNum,portNum,0,MIRROR_EGRESS_E,profile)) 
										{
										    npd_syslog_err("Delete src port error! devNum %d,portNum %d,direct %d\n",devNum,portNum,MIRROR_EGRESS_E);
											ret = MIRROR_RETURN_CODE_ERROR;
										}
										else if(0 != npd_mirror_nam_mirror_port_del_for_bcm(profile,MIRROR_EGRESS_E,srcPortNode->eth_g_index))
										{
											npd_syslog_err("Delete src port error! devNum %d,portNum %d,direct %d\n",devNum,portNum,MIRROR_EGRESS_E);
											ret = MIRROR_RETURN_CODE_ERROR;
										}
										else 
										{
											ret = npd_mirror_src_port_remove(profile,MIRROR_EGRESS_E,srcPortNode->eth_g_index);
										}
									}
								}
				             }
						} 
						if(MIRROR_DEST_INPORT_DEFAULT == destNode->in_eth_index)
						{
						  	 /* FOR PORT SOURCE*/
				          if((NULL !=(srcPortList = destNode->func[MIRROR_FUNC_PORT]))&&(srcPortList->count > 0))
						  {
							 	list = &(srcPortList->list);
							    __list_for_each(pos,list) 
								{
									srcPortNode = list_entry(pos,struct mirror_src_port_node_s,list);
									if((MIRROR_INGRESS_E == srcPortNode->direct)||(MIRROR_BIDIRECTION_E == srcPortNode->direct)){
										slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(srcPortNode->eth_g_index);
										slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
										port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(srcPortNode->eth_g_index);
										port_no = ETH_LOCAL_INDEX2NO(slot_index,srcPortNode->eth_g_index);
							            npd_get_devport_by_global_index(srcPortNode->eth_g_index, &devNum, &portNum);
					                    if(0 != nam_mirror_port_set(devNum,portNum,0,MIRROR_INGRESS_E,profile)) {
										    npd_syslog_err("Delete src port error! devNum %d,portNum %d,direct %d\n",devNum,portNum,MIRROR_INGRESS_E);
											ret = MIRROR_RETURN_CODE_ERROR;
										}								
										else if(0 != npd_mirror_nam_mirror_port_del_for_bcm(profile,MIRROR_INGRESS_E,srcPortNode->eth_g_index)){
											npd_syslog_err("Delete src port error! devNum %d,portNum %d,direct %d\n",devNum,portNum,MIRROR_INGRESS_E);
											ret = MIRROR_RETURN_CODE_ERROR;
										}
										else {
											ret = npd_mirror_src_port_remove(profile,MIRROR_INGRESS_E,srcPortNode->eth_g_index);
										}
									}
								}
				          }
						  if((NULL !=(srcVlanList = destNode->func[MIRROR_FUNC_VLAN]))&&(srcVlanList->count > 0))
						  {
			                 list = &(srcVlanList->list);
						     __list_for_each(pos,list) {
							     srcVlanNode = list_entry(pos,struct mirror_src_vlan_node_s,list);
								 npd_syslog_dbg("srcVlanNode vid %d\n",srcVlanNode->vid);
			                     if(0 != nam_mirror_vlan_set(srcVlanNode->vid,0)) {
								 	 npd_syslog_dbg("Delete src vlan error! vid %d\n",srcVlanNode->vid);
									 ret = MIRROR_RETURN_CODE_ERROR;
								 }
								 else {
									npd_mirror_src_vlan_remove(profile,srcVlanNode->vid);
								 }
						     }
						  }
						  if((NULL !=(srcPolicyList = destNode->func[MIRROR_FUNC_POLICY]))&&(srcPolicyList->count > 0))
						  {
			                 list = &(srcPolicyList->list);
						     __list_for_each(pos,list) {
							    srcPolicyNode = list_entry(pos,struct mirror_src_policy_node_s,list);
								npd_syslog_dbg("srcPolicyNode ruleIndex %d\n",srcPolicyNode->ruleIndex); 		
								if(ACL_GROUP_RULE_EXISTED == search_acl_group_rule_index_exist(srcPolicyNode->ruleIndex,&groupNum,&exist_dir)){
									/*update HW action*/
									ret = nam_acl_mirror_action_update(srcPolicyNode->ruleIndex,0,0);
									if(ret!=0)
										syslog_ax_mirror_err("del policy-source %d action update fail %d!\n",srcPolicyNode->ruleIndex,ret);				
								}	
								rulePtr = g_acl_rule[srcPolicyNode->ruleIndex];
								rulePtr->appendInfo.appendType =0;
								rulePtr->ActionPtr.actionType=ACL_ACTION_TYPE_PERMIT;
								rulePtr->ActionPtr.actionInfo.mirror.portNum =0;	
								ret = npd_mirror_src_acl_remove(profile,srcPolicyNode->ruleIndex);
						     }
						  }
						  if((NULL !=(srcFdbList = destNode->func[MIRROR_FUNC_FDB]))&&(srcFdbList->count > 0))
						  {
			                 list = &(srcFdbList->list);
						     __list_for_each(pos,list) {
								    srcFdbNode = list_entry(pos,struct mirror_src_fdb_node_s,list);
									memcpy(macAddr.arEther,srcFdbNode->mac,6);
									mac = macAddr.arEther;
									npd_syslog_dbg("srcFdbNode vid %d,mac %#02x:%#02x:%#02x:%#02x:%#02x:%#02x slot_no %d,port_no %d\n",srcFdbNode->vid,   \
										                      srcFdbNode->mac[0],srcFdbNode->mac[1],srcFdbNode->mac[2],srcFdbNode->mac[3],srcFdbNode->mac[4],srcFdbNode->mac[5],\
								                              srcFdbNode->slot_no,srcFdbNode->port_no);
									eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(srcFdbNode->slot_no,srcFdbNode->port_no);
									if(0 ==( ret = nam_mirror_fdb_entry_set(srcFdbNode->vid,eth_g_index,&macAddr,0))) {
									   if(0 != npd_fdb_static_mirror_entry_del(mac,srcFdbNode->vid)) {
			                               ret = MIRROR_RETURN_CODE_ERROR;
									   }
									   else {
						                  npd_mirror_src_fdb_remove(profile,srcFdbNode->vid,mac,srcFdbNode->slot_no,srcFdbNode->port_no);
									  }
						           }
					          }	
			   	            }
				  		} 
			   		}
	        	}
				if(MIRROR_RETURN_CODE_SUCCESS == ret)
				{
					npd_syslog_dbg("Set destination port success!profile %d,vriportId %d,direct %s\n",profile,virPortId,(MIRROR_INGRESS_E == direct)? "ingress":(MIRROR_INGRESS_E == direct)? "egress":"bidirection");
				}
			}	
		}
	}

	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,
								   DBUS_TYPE_UINT32,
								   &ret);
	dbus_message_iter_append_basic (&iter,
								   DBUS_TYPE_UINT32,
								   &eth_g_index);
	return reply;
}


DBusMessage * npd_dbus_debug_mirror_create_dest_port(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter= {0};
	DBusError			err;   		
	unsigned char       slot_index = 0,port_index = 0,slot_no = 0,port_no = 0,devNum = 0,portNum = 0,virPortId = 0;
	unsigned int	    eth_g_index=0, ret = MIRROR_RETURN_CODE_SUCCESS,profile = DEFAULT_MIRROR_PROFILE;


	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
								 DBUS_TYPE_BYTE,&slot_no,
								 DBUS_TYPE_BYTE,&port_no,  
								 DBUS_TYPE_UINT32,&profile,
								 DBUS_TYPE_INVALID))) 
	{
		npd_syslog_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			npd_syslog_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}	

	ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
	if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
		if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,port_no)) {
			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
			ret = MIRROR_RETURN_CODE_SUCCESS;
		}
	}

	if(MIRROR_RETURN_CODE_SUCCESS == ret) 
	{
		if(0 != npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum))
		{
			ret = MIRROR_RETURN_CODE_ERROR;
		}
		npd_syslog_dbg("devNum = %d,portNum = %d\n",devNum,portNum);
		ret = nam_mirror_analyzer_port_set(profile,devNum,portNum,0);
		if(ret != MIRROR_RETURN_CODE_SUCCESS)
		{
			npd_syslog_err("Error to set the destination port!\n");
            ret = MIRROR_RETURN_CODE_DESTINATION_NODE_CREATE_FAIL;
		}
	}
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,
								   DBUS_TYPE_UINT32,
								   &ret);
	return reply;
}

unsigned int npd_mirror_nam_dest_port_set_for_bcm
(
	unsigned int profile,
	unsigned char devNum,
	unsigned char virPortId,
	unsigned int direct
)
{

	unsigned int src_eth_g_index[64] = {0};
	unsigned int srcCount = 64;
	unsigned int i = 0;
	unsigned int ret = 0;
	unsigned char srcDevNum = 0;
	unsigned char srcPort = 0;
	unsigned int step = 1;
	MIRROR_DIRECTION_TYPE initMod = MIRROR_INGRESS_E;
	MIRROR_DIRECTION_TYPE mirrorMod = MIRROR_INGRESS_E;
	MIRROR_DIRECTION_TYPE setMod = MIRROR_INGRESS_E;
	
	if(MIRROR_INGRESS_E == direct){
		step = 2;
	}
	if(MIRROR_EGRESS_E == direct){
		initMod = MIRROR_EGRESS_E;
	}
	for(mirrorMod = initMod; mirrorMod <= MIRROR_BIDIRECTION_E; mirrorMod += step){
		srcCount = 64;
		if((MIRROR_RETURN_CODE_SUCCESS == npd_mirror_src_port_get(profile,mirrorMod,\
			src_eth_g_index,&srcCount))&&(srcCount > 0)){
			for(i = 0;i<srcCount;i++){
				npd_get_devport_by_global_index(src_eth_g_index[i],&srcDevNum,&srcPort);
				if(direct != MIRROR_BIDIRECTION_E){
                    setMod = direct;
				}
				else {
					setMod = mirrorMod;
				}
				ret = nam_mirror_analyzer_port_set_for_bcm(srcDevNum,srcPort,\
					devNum,virPortId,setMod);
				if(0 != ret){
                    npd_syslog_dbg("set nam mirror analyzer port for bcm error! mirror mode %d\n",setMod);
					return -MIRROR_RETURN_CODE_ERROR;
				}
			}
		}
	}
	return MIRROR_RETURN_CODE_SUCCESS;
}

DBusMessage * npd_dbus_mirror_del_dest_port(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter= {0};
	DBusError			err;   		
	unsigned char       slot_no = 0,port_no = 0,devNum = 0,virPortId = 0;
	unsigned int	    eth_g_index=0, ret = MIRROR_RETURN_CODE_SUCCESS,profile = DEFAULT_MIRROR_PROFILE;	
	MIRROR_DIRECTION_TYPE direct = MIRROR_INGRESS_E;
	struct mirror_struct_s *destNode = NULL;

	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
								 DBUS_TYPE_BYTE,&slot_no,
								 DBUS_TYPE_BYTE,&port_no, 
								 DBUS_TYPE_UINT32,&profile,
								 DBUS_TYPE_UINT32,&direct,
								 DBUS_TYPE_INVALID))) 
	{
		syslog_ax_mirror_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_mirror_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}	
	/*del dest port means that recover eth ingress/egress destination port to default port */
	ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
	if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
		if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,port_no)) {
			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
			/*printf("index for %d/%d is %#0x\n",slot_no,port_no,eth_g_index);*/
			ret = MIRROR_RETURN_CODE_SUCCESS;
		}
	}

	if(MIRROR_RETURN_CODE_SUCCESS == ret) 
	{
		if(0 != npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortId))
		{
			ret = MIRROR_RETURN_CODE_ERROR;
		}
		if(NULL ==(destNode = npd_mirror_get_profile_node(profile)))
		{/*check if the node exist*/
            ret = MIRROR_RETURN_CODE_PROFILE_NOT_CREATED;
	    }
		else if(NPD_TRUE == npd_mirror_destination_node_member_check(profile,direct))
		{/*check if has members*/
            ret = MIRROR_RETURN_CODE_DESTINATION_NODE_MEMBER_EXIST;
		}
		else
		{/*set to default value*/
           if(MIRROR_INGRESS_E == direct)
		   {
			  if(destNode->in_eth_index == eth_g_index)
			  {
                  destNode->in_eth_index = MIRROR_DEST_INPORT_DEFAULT;
				  /*cancel the mirror port in hw*/
				  nam_mirror_analyzer_port_set(profile,0,0,MIRROR_INGRESS_E);
				  npd_mirror_nam_analyer_port_del_for_bcm(profile,direct,destNode->eg_eth_index);
			  }
			  else 
			  {
                   ret = MIRROR_RETURN_CODE_BAD_PARAM;
			  }
			}
			else if(MIRROR_EGRESS_E == direct)
			{
				if(destNode->eg_eth_index == eth_g_index)
				{
				    destNode->eg_eth_index =  MIRROR_DEST_INPORT_DEFAULT;
					/*cancel the mirror port in hw*/
				    nam_mirror_analyzer_port_set(profile,0,0,MIRROR_EGRESS_E);
				    npd_mirror_nam_analyer_port_del_for_bcm(profile,direct,destNode->in_eth_index);
				}
				else 
				{
                   ret = MIRROR_RETURN_CODE_BAD_PARAM;
			    }
			}
			else if(MIRROR_BIDIRECTION_E == direct)
			{
				if((destNode->eg_eth_index == eth_g_index)&&(destNode->in_eth_index == eth_g_index))
				{
				    destNode->in_eth_index = MIRROR_DEST_INPORT_DEFAULT;
				    destNode->eg_eth_index =  MIRROR_DEST_INPORT_DEFAULT;
					/*cancel the mirror port in hw*/
				    nam_mirror_analyzer_port_set(profile,0,0,MIRROR_BIDIRECTION_E);
				    npd_mirror_nam_analyer_port_del_for_bcm(profile,direct,0);
				}
				else 
				{
                   ret = MIRROR_RETURN_CODE_BAD_PARAM;
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

unsigned int npd_mirror_nam_analyer_port_del_for_bcm
(
    unsigned int profile,
    MIRROR_DIRECTION_TYPE direct,
    unsigned int dest_eth_g_index
)
{
	unsigned int src_eth_g_index[64] = {0};
	unsigned int srcCount = 64;
	unsigned int i = 0;
	unsigned char srcDevNum = 0;
	unsigned char srcPort = 0;
	unsigned int ret = 0;
	MIRROR_DIRECTION_TYPE setMod = direct;	
    unsigned char destDevNum = 0;
    unsigned char destPort = 0;
	do{
	    srcCount = 64;
		if((MIRROR_RETURN_CODE_SUCCESS == npd_mirror_src_port_get(profile,setMod,\
			src_eth_g_index,&srcCount))&&(srcCount > 0)){
			for(i = 0;i<srcCount;i++){
				npd_get_devport_by_global_index(src_eth_g_index[i],&srcDevNum,&srcPort);

				ret = nam_mirror_analyzer_port_set_for_bcm(srcDevNum,srcPort,\
					0,0,setMod);
				if(0 != ret){
	                npd_syslog_dbg("set nam mirror analyzer port for bcm error! mirror mode %d\n",setMod);
					return -MIRROR_RETURN_CODE_ERROR;
				}
			}
		}
	}while((direct == MIRROR_BIDIRECTION_E)&&(setMod-- > 0));
	
    if((MIRROR_INGRESS_E == direct)||(MIRROR_EGRESS_E == direct)){
	    srcCount = 64;
		if((MIRROR_RETURN_CODE_SUCCESS == npd_mirror_src_port_get(profile,MIRROR_BIDIRECTION_E,src_eth_g_index,&srcCount))&&(srcCount > 0)){
			for(i = 0;i<srcCount;i++){
				npd_get_devport_by_global_index(src_eth_g_index[i],&srcDevNum,&srcPort);
				npd_get_devport_by_global_index(dest_eth_g_index,&destDevNum,&destPort);
				ret = nam_mirror_analyzer_port_set_for_bcm(srcDevNum,srcPort,\
					destDevNum,destPort,1-direct);
				if(0 != ret){
	                npd_syslog_dbg("set nam mirror analyzer port for bcm error! mirror mode %d\n",1-direct);
					return -MIRROR_RETURN_CODE_ERROR;
				}
			}
		}
	}
	return MIRROR_RETURN_CODE_SUCCESS;

}

DBusMessage * npd_dbus_mirror_create_source_acl(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter= {0};
	DBusError			err;   		 
	unsigned int        ruleIndex = 0;
	int ret = 0;
	unsigned int profile = DEFAULT_MIRROR_PROFILE;
	unsigned int groupNum,exist_dir,ethIndex,rs;
	AclRuleList  *rulePtr=NULL;
	struct mirror_struct_s *destNode = NULL;
	unsigned char devNum,portNum;

	if(!((productinfo.capbmp) & FUNCTION_MIRROR_VLAUE)){
		npd_syslog_dbg("do not support mirror!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_MIRROR_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_MIRROR_VLAUE)){
		npd_syslog_dbg("do not support mirror!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		dbus_error_init(&err);

		if (!(dbus_message_get_args ( msg, &err,
									 DBUS_TYPE_UINT32,&profile,
									 DBUS_TYPE_UINT32,&ruleIndex,
									 DBUS_TYPE_INVALID))) 
		{
			syslog_ax_mirror_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				syslog_ax_mirror_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}	

		if(NULL==g_acl_rule[ruleIndex])
		{
			ret= MIRROR_RETURN_CODE_ACL_GLOBAL_NOT_EXISTED;
		}
		#if 0
		else if(NULL == npd_mirror_get_profile_node(profile)) {
			ret = npd_mirror_destination_port_create(profile,0, MIRROR_INGRESS_E);
			printf("create dest 0!\n");
		}
		#endif
		else if(NULL == (destNode = npd_mirror_get_profile_node(profile))) {
			npd_syslog_err("No profile node!\n");
			ret = MIRROR_RETURN_CODE_PROFILE_NOT_CREATED;
		}
		else if(NPD_FALSE == npd_mirror_destination_port_direct_check(profile,MIRROR_INGRESS_E)){
	       	ret = MIRROR_RETURN_CODE_DESTINATION_NODE_NOTEXIST;
		}
		else if(NPD_TRUE == npd_mirror_src_acl_check(profile,ruleIndex)) {
			npd_syslog_err("No acl rule index\n");
			ret = MIRROR_RETURN_CODE_SRC_ACL_EXIST;
		}
		else if(MIRROR_RETURN_CODE_ERROR == npd_mirror_acl_action_check(ruleIndex)){
	 	      ret = MIRROR_RETURN_CODE_ERROR;
		}
		else if ( 1 != g_acl_rule[ruleIndex]->ruleType) {
			ret = MIRROR_RETURN_CODE_EXT_RULE_OLNY;
		}
		else {
			/*check if acl in group ,if yes,update action in HW	*/
			ethIndex = destNode->in_eth_index;
			npd_get_devport_by_global_index(ethIndex, &devNum, &portNum);
			rulePtr=g_acl_rule[ruleIndex];
			rulePtr->appendInfo.appendType =8;
			rulePtr->ActionPtr.actionType=ACL_ACTION_TYPE_MIRROR_TO_ANALYZER;
			rulePtr->ActionPtr.actionInfo.mirror.portNum =portNum;

			rs = search_acl_group_rule_index_exist(ruleIndex,&groupNum,&exist_dir);
			/*if(ACL_GROUP_RULE_EXISTED == rs) { */
			if(ACL_RETURN_CODE_GROUP_RULE_EXISTED == rs) {
				/*update HW action*/
				ret = nam_acl_mirror_action_update(ruleIndex,1,0);
				if(ret != 0) {
					syslog_ax_mirror_err("add policy-source %d mirror update fail %d!\n",ruleIndex,ret);
				}
				npd_syslog_dbg("set acl mirror for hw ret is %d \n", ret);
			}	
			ret = npd_mirror_src_acl_create(profile,ruleIndex);
		}
	}
  	
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,
								   DBUS_TYPE_UINT32,
								   &ret);
	return reply;
}


DBusMessage * npd_dbus_mirror_delete_source_acl(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter= {0};
	DBusError			err;   		
	unsigned int 		ruleIndex = 0;
	int ret = 0;
	unsigned int profile = DEFAULT_MIRROR_PROFILE;
	unsigned int groupNum = 0,exist_dir = 0,rs = 0;
	AclRuleList  *rulePtr=NULL;
	
	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
								 DBUS_TYPE_UINT32,&profile,
								 DBUS_TYPE_UINT32,&ruleIndex,  
								 DBUS_TYPE_INVALID))) 
	{
		syslog_ax_mirror_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_mirror_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}	

	if(NULL==g_acl_rule[ruleIndex])
	{
		ret= MIRROR_RETURN_CODE_ACL_GLOBAL_NOT_EXISTED;
	}
		
	else if(NPD_TRUE != npd_mirror_src_acl_check(profile,ruleIndex)) {
		ret = MIRROR_RETURN_CODE_SRC_ACL_NOTEXIST;		
	}
	
	else{		
		rs=search_acl_group_rule_index_exist(ruleIndex,&groupNum,&exist_dir);			
		if(ACL_GROUP_RULE_EXISTED == rs)
		{
			/*update HW action*/
			ret = nam_acl_mirror_action_update(ruleIndex,0,0);
			if(ret!=0)
				syslog_ax_mirror_err("del policy-source %d action update fail %d!\n",ruleIndex,ret);		      	
		}	
		rulePtr=g_acl_rule[ruleIndex];
		rulePtr->appendInfo.appendType =0;
		rulePtr->ActionPtr.actionType=ACL_ACTION_TYPE_PERMIT;
		rulePtr->ActionPtr.actionInfo.mirror.portNum =0;	
		ret = npd_mirror_src_acl_remove(profile,ruleIndex);
	}

	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,
								   DBUS_TYPE_UINT32,
								   &ret);
	return reply;
}

DBusMessage * npd_dbus_mirror_create_source_port(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter= {0};
	DBusError			err;   		
	unsigned char	    slot = 0,port = 0,devNum = 0,portNum = 0;	
	unsigned int	    src_eth_g_index = 0;	 
	int ret = 0;
	MIRROR_DIRECTION_TYPE direct = MIRROR_INGRESS_E;
	unsigned int profile = DEFAULT_MIRROR_PROFILE;

	if(!((productinfo.capbmp) & FUNCTION_MIRROR_VLAUE)){
		npd_syslog_dbg("do not support mirror!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_MIRROR_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_MIRROR_VLAUE)){
		npd_syslog_dbg("do not support mirror!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		dbus_error_init(&err);

		if (!(dbus_message_get_args ( msg, &err,
									 DBUS_TYPE_UINT32,&profile,
									 DBUS_TYPE_UINT32,&direct,
									 DBUS_TYPE_BYTE,&slot,  
									 DBUS_TYPE_BYTE,&port,
									 DBUS_TYPE_INVALID))) 
		{
			syslog_ax_mirror_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				syslog_ax_mirror_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}	

		ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
		if (CHASSIS_SLOTNO_ISLEGAL(slot)) {
			if (ETH_LOCAL_PORTNO_ISLEGAL(slot,port)) {
				src_eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot,port);
				npd_get_devport_by_global_index(src_eth_g_index, &devNum, &portNum);
				/*printf("index for %d/%d is %#0x\n",slot,port,src_eth_g_index);*/
				ret = MIRROR_RETURN_CODE_SUCCESS;
			}
		}

		if(MIRROR_RETURN_CODE_SUCCESS == ret) {
			if(NULL == npd_mirror_get_profile_node(profile)) {
				ret = MIRROR_RETURN_CODE_PROFILE_NOT_CREATED;
			}
			else if(NPD_FALSE == npd_mirror_destination_port_direct_check(profile,direct)){
				ret = MIRROR_RETURN_CODE_DESTINATION_NODE_NOTEXIST;
			}
			else if(NPD_TRUE == npd_mirror_src_port_check(profile,direct,src_eth_g_index)) {
				ret = MIRROR_RETURN_CODE_SRC_PORT_EXIST;
			}
			else if(0 != nam_mirror_port_set(devNum,portNum,1,direct,profile)) {
				ret = MIRROR_RETURN_CODE_ERROR;
			}
			else if(0 != npd_mirror_nam_mirror_port_set_for_bcm(profile,direct,src_eth_g_index)){
				ret = MIRROR_RETURN_CODE_ERROR;
			}
			else {
				ret = npd_mirror_src_port_create(profile,direct,src_eth_g_index);
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

DBusMessage * npd_dbus_debug_mirror_create_source_port(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter= {0};
	DBusError			err;   		
	unsigned char	    slot = 0,port = 0;	 
	int ret = 0;
	MIRROR_DIRECTION_TYPE direct = MIRROR_INGRESS_E;
	unsigned int profile = DEFAULT_MIRROR_PROFILE;

	
	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
								 DBUS_TYPE_UINT32,&profile,
								 DBUS_TYPE_UINT32,&direct,
								 DBUS_TYPE_BYTE,&slot,  
								 DBUS_TYPE_BYTE,&port,
								 DBUS_TYPE_INVALID))) 
	{
		syslog_ax_mirror_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_mirror_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}	
	slot = 0;
	ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
	npd_syslog_dbg("dev = %d,port = %d\n",slot,port);
	ret = nam_mirror_port_set(slot,port,1,direct,profile);
	if(ret != MIRROR_RETURN_CODE_SUCCESS)
	{
		npd_syslog_err("Error to set the destination port!\n");
        ret = MIRROR_RETURN_CODE_DESTINATION_NODE_CREATE_FAIL;
	}
	else
	{
		npd_syslog_dbg("set port %d mirror successfully !\n",port);
	}
			
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,
								   DBUS_TYPE_UINT32,
								   &ret);
	return reply;
}


unsigned int npd_mirror_nam_mirror_port_set_for_bcm
(
	unsigned int profile,
	MIRROR_DIRECTION_TYPE direct,
	unsigned int src_eth_g_index
	
)
{
	unsigned int eth_g_index = 0;
	unsigned char srcDev = 0;
	unsigned char srcPort = 0;
	unsigned char destDev = 0;
	unsigned char destPort = 0;
	unsigned int destDirect = direct;
	unsigned int ret = 0;
	
    ret = npd_mirror_destination_node_port_get(profile,destDirect,&eth_g_index);
	if(MIRROR_RETURN_CODE_SUCCESS == ret){
		npd_get_devport_by_global_index(src_eth_g_index,&srcDev,&srcPort);
	    npd_get_devport_by_global_index(eth_g_index,&destDev,&destPort);
		ret = nam_mirror_analyzer_port_set_for_bcm(srcDev,srcPort,destDev,destPort,direct);
		if(MIRROR_RETURN_CODE_SUCCESS != ret){
			syslog_ax_mirror_err("mirror port set error for bcm, ret %d",ret);
	        return -MIRROR_RETURN_CODE_ERROR;
		}		
		return MIRROR_RETURN_CODE_SUCCESS;
	}	
	return -MIRROR_RETURN_CODE_ERROR;
}


DBusMessage * npd_dbus_mirror_del_source_port(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter= {0};
	DBusError			err;   		
	unsigned char	    slot = 0,port = 0,devNum = 0, portNum = 0;	
	unsigned int	    src_eth_g_index = 0;	 
	int ret = 0;
	MIRROR_DIRECTION_TYPE direct = MIRROR_INGRESS_E;
	unsigned int profile = DEFAULT_MIRROR_PROFILE;

	
	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
								 DBUS_TYPE_UINT32,&profile,
								  DBUS_TYPE_UINT32,&direct,
								 DBUS_TYPE_BYTE,&slot,  
								 DBUS_TYPE_BYTE,&port,
								 DBUS_TYPE_INVALID))) 
	{
		syslog_ax_mirror_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_mirror_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}	

	ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
	if (CHASSIS_SLOTNO_ISLEGAL(slot)) 
	{
		if (ETH_LOCAL_PORTNO_ISLEGAL(slot,port)) 
		{
			src_eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot,port);
			npd_get_devport_by_global_index(src_eth_g_index, &devNum, &portNum);
			/*printf("index for %d/%d is %#0x\n",slot,port,src_eth_g_index);*/
			ret = MIRROR_RETURN_CODE_SUCCESS;
		}
	}

	if(MIRROR_RETURN_CODE_SUCCESS == ret) {
		if(NULL == npd_mirror_get_profile_node(profile)) {
			ret = MIRROR_RETURN_CODE_PROFILE_NOT_CREATED;
		}
		else if(NPD_FALSE == npd_mirror_src_port_check(profile,direct,src_eth_g_index)) {
			ret = MIRROR_RETURN_CODE_SRC_PORT_NOTEXIST;
		}
		else if(0 != nam_mirror_port_set(devNum,portNum,0,direct,profile)) {
			ret = MIRROR_RETURN_CODE_ERROR;
		}
		else if(0 != npd_mirror_nam_mirror_port_del_for_bcm(profile,direct,src_eth_g_index)){
			ret = MIRROR_RETURN_CODE_ERROR;
		}
		else {
			ret = npd_mirror_src_port_remove(profile,direct,src_eth_g_index);
		}
	}

		
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,
								   DBUS_TYPE_UINT32,
								   &ret);
	return reply;
}

DBusMessage * npd_dbus_debug_mirror_del_source_port(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter= {0};
	DBusError			err;   		
	unsigned char	    slot = 0,port = 0,devNum = 0, portNum = 0;	
	unsigned int	    src_eth_g_index = 0;	 
	int ret = 0;
	MIRROR_DIRECTION_TYPE direct = MIRROR_INGRESS_E;
	unsigned int profile = DEFAULT_MIRROR_PROFILE;

	
	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
								 DBUS_TYPE_UINT32,&profile,
								  DBUS_TYPE_UINT32,&direct,
								 DBUS_TYPE_BYTE,&slot,  
								 DBUS_TYPE_BYTE,&port,
								 DBUS_TYPE_INVALID))) 
	{
		syslog_ax_mirror_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_mirror_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}	
	slot = 0;

	ret = nam_mirror_port_set(slot,port,0,direct,profile);
	if(ret != MIRROR_RETURN_CODE_SUCCESS)
	{
		npd_syslog_err("Error to set the destination port!\n");
		ret = MIRROR_RETURN_CODE_DESTINATION_NODE_CREATE_FAIL;
	}
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,
								   DBUS_TYPE_UINT32,
								   &ret);
	return reply;
}


unsigned int npd_mirror_nam_mirror_port_del_for_bcm
(
	unsigned int profile,
	MIRROR_DIRECTION_TYPE direct,
	unsigned int src_eth_g_index
	
)
{
	unsigned int eth_g_index = 0;
	unsigned char srcDev = 0;
	unsigned char srcPort = 0;
	unsigned char destDev = 0;
	unsigned char destPort = 0;
	unsigned int destDirect = direct;
	unsigned int ret = 0;
	
    ret = npd_mirror_destination_node_port_get(profile,destDirect,&eth_g_index);
	if(MIRROR_RETURN_CODE_SUCCESS == ret){
		npd_get_devport_by_global_index(src_eth_g_index,&srcDev,&srcPort);
	    npd_get_devport_by_global_index(eth_g_index,&destDev,&destPort);
		ret = nam_mirror_analyzer_port_set_for_bcm(srcDev,srcPort,destDev,destPort,MIRROR_DIRECT_DEL_E);
		if(MIRROR_RETURN_CODE_SUCCESS != ret){
			syslog_ax_mirror_err("mirror port set error for bcm, ret %d",ret);
	        return -MIRROR_RETURN_CODE_ERROR;
		}		
		return MIRROR_RETURN_CODE_SUCCESS;
	}	
	return -MIRROR_RETURN_CODE_ERROR;
}

DBusMessage * npd_dbus_mirror_create_source_vlan(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter= {0};
	DBusError			err;   		 
	unsigned short vid = 0;
	int ret = 0;
	unsigned int profile = DEFAULT_MIRROR_PROFILE;
	struct mirror_struct_s *destNode = NULL;
	unsigned char destDev = 0;
	unsigned char destPort = 0;

	if(!((productinfo.capbmp) & FUNCTION_MIRROR_VLAUE)){
		npd_syslog_dbg("do not support mirror!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_MIRROR_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_MIRROR_VLAUE)){
		npd_syslog_dbg("do not support mirror!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		dbus_error_init(&err);

		if (!(dbus_message_get_args ( msg, &err,
									 DBUS_TYPE_UINT32,&profile,
									 DBUS_TYPE_UINT16,&vid,  
									 DBUS_TYPE_INVALID))) 
		{
			syslog_ax_mirror_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				syslog_ax_mirror_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}	

		printf("vid %d\n",vid);
		if(VLAN_RETURN_CODE_VLAN_EXISTS != npd_check_vlan_exist(vid)) {
			ret =MIRROR_RETURN_CODE_VLAN_NOT_EXIST;
		}
		else if(NULL == (destNode = npd_mirror_get_profile_node(profile))) {
			ret = MIRROR_RETURN_CODE_PROFILE_NOT_CREATED;
		}
		else if(NPD_FALSE == npd_mirror_destination_port_direct_check(profile,MIRROR_INGRESS_E)){
				ret = MIRROR_RETURN_CODE_DESTINATION_NODE_NOTEXIST;
		}
		else if(NPD_TRUE == npd_mirror_src_vlan_check(profile,vid)) {
			ret = MIRROR_RETURN_CODE_SRC_VLAN_EXIST;
		}
		else if(0 != nam_mirror_vlan_set(vid,1)) {
			ret = MIRROR_RETURN_CODE_ERROR;
		}
		else if(MIRROR_DEST_INPORT_DEFAULT != destNode->in_eth_index){
		     npd_get_devport_by_global_index(destNode->in_eth_index,&destDev,&destPort);
			 ret = nam_mirror_vlan_set_for_bcm(vid,destDev,destPort,0);
			 if(0 == ret){
			 	 if(MIRROR_DEST_INPORT_DEFAULT != destNode->eg_eth_index){
				     npd_get_devport_by_global_index(destNode->eg_eth_index,&destDev,&destPort);
					 ret = nam_mirror_vlan_set_for_bcm(vid,destDev,destPort,0);
					 if(0 != ret){
		                 npd_syslog_dbg("Delete src vlan for ingress error! vid %d\n",vid);
				 	     ret = MIRROR_RETURN_CODE_ERROR;
					 }
			 	 }
			 }
			 else{
			 	npd_syslog_dbg("Delete src vlan for egress error! vid %d\n",vid);
			 	ret = MIRROR_RETURN_CODE_ERROR;
			 }
		}
		if(0 == ret){
			ret = npd_mirror_src_vlan_create(profile,vid);
		}
	}

	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,
								   DBUS_TYPE_UINT32,
								   &ret);
	return reply;
}


DBusMessage * npd_dbus_mirror_del_source_vlan(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter= {0};
	DBusError			err;   			 
	unsigned short vid = 0;
	int ret = 0;
	unsigned int profile = DEFAULT_MIRROR_PROFILE;
	struct mirror_struct_s *destNode = NULL;
	unsigned char destDev = 0;
	unsigned char destPort = 0;

	
	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
								 DBUS_TYPE_UINT32,&profile,
								 DBUS_TYPE_UINT16,&vid,  
								 DBUS_TYPE_INVALID))) 
	{
		syslog_ax_mirror_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_mirror_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}	

	/*printf("vid %d\n",vid);*/
	if(VLAN_RETURN_CODE_VLAN_EXISTS != npd_check_vlan_exist(vid)) {
		ret =MIRROR_RETURN_CODE_VLAN_NOT_EXIST;
	}
	else if(NULL == (destNode = npd_mirror_get_profile_node(profile))) {
		ret = MIRROR_RETURN_CODE_PROFILE_NOT_CREATED;
	}
	else if(NPD_FALSE == npd_mirror_src_vlan_check(profile,vid)) {
		ret = MIRROR_RETURN_CODE_SRC_VLAN_NOTEXIST;
	}
	else if(0 != nam_mirror_vlan_set(vid,0)) {
		ret = MIRROR_RETURN_CODE_ERROR;
	}
	else{
		 if(MIRROR_DEST_INPORT_DEFAULT != destNode->in_eth_index){
		     npd_get_devport_by_global_index(destNode->in_eth_index,&destDev,&destPort);
			 ret = nam_mirror_vlan_set_for_bcm(vid,destDev,destPort,0);
			 if(0 == ret){
			 	 if(MIRROR_DEST_INPORT_DEFAULT != destNode->eg_eth_index){
				     npd_get_devport_by_global_index(destNode->eg_eth_index,&destDev,&destPort);
					 ret = nam_mirror_vlan_set_for_bcm(vid,destDev,destPort,0);
					 if(0 != ret){
		                 npd_syslog_dbg("Delete src vlan for ingress error! vid %d\n",vid);
				 	     ret = MIRROR_RETURN_CODE_ERROR;
					 }
			 	 }
			 }
			 else{
			 	npd_syslog_dbg("Delete src vlan for egress error! vid %d\n",vid);
			 	ret = MIRROR_RETURN_CODE_ERROR;
			 }
		 }
	}
	if(0 == ret) {
		ret = npd_mirror_src_vlan_remove(profile,vid);
	}

	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,
								   DBUS_TYPE_UINT32,
								   &ret);
	return reply;
}

DBusMessage * npd_dbus_mirror_fdb_mac_vlanid_port_set(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter= {0};
	DBusError			err;   		
	unsigned char	    slot_no = 0,port_no = 0,slot = 0,port = 0;;	
	unsigned short      vlanid = 0;
	unsigned int       global_index = 0;
	int	   ret = MIRROR_RETURN_CODE_SUCCESS;
    ETHERADDR macAddr; 
	char macComp[13] = {0};
	unsigned char *mac = NULL;
    memset(&macAddr,0,sizeof(ETHERADDR));
	unsigned int profile = DEFAULT_MIRROR_PROFILE;

	if(!((productinfo.capbmp) & FUNCTION_MIRROR_VLAUE))
	{
		npd_syslog_dbg("do not support mirror!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_MIRROR_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_MIRROR_VLAUE)){
		npd_syslog_dbg("do not support mirror!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else 
	{
		dbus_error_init(&err);

		if (!(dbus_message_get_args ( msg, &err,
									 DBUS_TYPE_UINT16,&vlanid,
									 DBUS_TYPE_BYTE,&slot_no,
									 DBUS_TYPE_BYTE,&port_no,
									 DBUS_TYPE_BYTE,&(macAddr.arEther[0]),
									 DBUS_TYPE_BYTE,&(macAddr.arEther[1]),
									 DBUS_TYPE_BYTE,&(macAddr.arEther[2]),
									 DBUS_TYPE_BYTE,&(macAddr.arEther[3]),
									 DBUS_TYPE_BYTE,&(macAddr.arEther[4]),
									 DBUS_TYPE_BYTE,&(macAddr.arEther[5]),
									 DBUS_TYPE_UINT32,&profile,
									 DBUS_TYPE_INVALID))) 
		{
			syslog_ax_mirror_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) 
			{
				syslog_ax_mirror_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}	
		mac = macAddr.arEther;
	   /*check if sys mac*/
		sprintf(macComp,"%02x%02x%02x%02x%02x%02x",	\
				macAddr.arEther[0],macAddr.arEther[1],macAddr.arEther[2],macAddr.arEther[3],macAddr.arEther[4],macAddr.arEther[5]);
	 	if(nam_is_bcm_check())
		{
	        ret = MIRROR_RETURN_CODE_FDB_NOT_SUPPORT;
		}
		else if(0 != npd_system_verify_basemac(macComp))
		{
	        ret = MIRROR_RETURN_CODE_FDB_MAC_BE_SYSMAC;
		}
	   /*check vlan if exist*/
		else if(VLAN_RETURN_CODE_VLAN_EXISTS != npd_check_vlan_exist(vlanid)) 
		{
			ret =MIRROR_RETURN_CODE_VLAN_NOT_EXIST;
		}
		/*check static item exist*/
		else if((NPD_FDB_ERR_NODE_EXIST == npd_fdb_check_static_entry_exist(mac,vlanid,&slot,&port))&&((slot_no != slot)||(port_no != port)))
		{
	             ret = MIRROR_RETURN_CODE_SRC_PORT_CONFLICT;
		}
		else if(NULL == npd_mirror_get_profile_node(profile)) 
		{
			ret = MIRROR_RETURN_CODE_PROFILE_NOT_CREATED;
		}
		else if(NPD_FALSE == npd_mirror_destination_port_direct_check(profile,MIRROR_INGRESS_E))
		{
			ret = MIRROR_RETURN_CODE_DESTINATION_NODE_NOTEXIST;
		}
		else if(NPD_TRUE == npd_mirror_src_fdb_check(profile,vlanid,mac,slot_no,port_no)) 
		{
			ret = MIRROR_RETURN_CODE_SRC_FDB_EXIST;
		}
		else
		{
			global_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
			if(0 == (ret = nam_mirror_fdb_entry_set(vlanid,global_index,&macAddr,1)))
			{	
				if(0 != npd_fdb_static_entry_add(mac,vlanid,slot_no, port_no,FDB_STATIC_ISMIRROR)) 
				{
					nam_mirror_fdb_entry_set(vlanid,global_index,&macAddr,0);
				}
				else if(0 != (ret = npd_mirror_src_fdb_create(profile,vlanid,mac,slot_no,port_no)))
				{
					nam_mirror_fdb_entry_set(vlanid,global_index,&macAddr,0);
					npd_fdb_static_entry_del(mac,vlanid);
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



DBusMessage * npd_dbus_mirror_fdb_mac_vlanid_port_cancel(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter= {0};
	DBusError			err;   		
	unsigned char	    slot_no,port_no;	
	unsigned short      vlanid = 0;
	unsigned int       global_index = 0;
	int	     ret = MIRROR_RETURN_CODE_SUCCESS;
    ETHERADDR macAddr;
	unsigned char *mac = NULL;
    memset(&macAddr,0,sizeof(ETHERADDR));
	unsigned int profile = DEFAULT_MIRROR_PROFILE;

	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
								 DBUS_TYPE_UINT16,&vlanid,
								 DBUS_TYPE_BYTE,&slot_no,
								 DBUS_TYPE_BYTE,&port_no,
								 DBUS_TYPE_BYTE,&(macAddr.arEther[0]),
								 DBUS_TYPE_BYTE,&(macAddr.arEther[1]),
								 DBUS_TYPE_BYTE,&(macAddr.arEther[2]),
								 DBUS_TYPE_BYTE,&(macAddr.arEther[3]),
								 DBUS_TYPE_BYTE,&(macAddr.arEther[4]),
								 DBUS_TYPE_BYTE,&(macAddr.arEther[5]),
								 DBUS_TYPE_UINT32,&profile,
								 DBUS_TYPE_INVALID))) 
	{
		syslog_ax_mirror_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_mirror_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}	
    mac = macAddr.arEther;
	if(VLAN_RETURN_CODE_VLAN_EXISTS != npd_check_vlan_exist(vlanid)) {
		ret =MIRROR_RETURN_CODE_VLAN_NOT_EXIST;
	}
	else if(NULL == npd_mirror_get_profile_node(profile)) {
		ret = MIRROR_RETURN_CODE_PROFILE_NOT_CREATED;
	}
	else if(NPD_FALSE == npd_mirror_src_fdb_check(profile,vlanid,mac,slot_no,port_no)) {
		ret = MIRROR_RETURN_CODE_SRC_FDB_NOTEXIST;
	}
	else {
		global_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
		if(0 ==( ret = nam_mirror_fdb_entry_set(vlanid,global_index,&macAddr,0))) {
			 if(0 != npd_fdb_static_mirror_entry_del(mac,vlanid)) {
				 npd_fdb_static_entry_add(mac,vlanid,slot_no, port_no,FDB_STATIC_ISMIRROR);
			}
			else if(0 != (ret = npd_mirror_src_fdb_remove(profile,vlanid,mac,slot_no,port_no))){
				npd_fdb_static_entry_add(mac,vlanid,slot_no, port_no,FDB_STATIC_ISMIRROR);
				nam_mirror_fdb_entry_set(vlanid,global_index,&macAddr,1);
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

DBusMessage * npd_dbus_mirror_show(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter= {0};
	DBusError			err;   		
	unsigned char slot_index = 0,port_index = 0,slot_no = 0,port_no=0,ingress_slot_no = 0,ingress_port_no = 0,egress_slot_no = 0,egress_port_no = 0;
	unsigned int ingress_eth_index=0, egress_eth_index = 0;
	unsigned int bi_flag = 0,in_flag = 0,eg_flag = 0;
	unsigned int ret = MIRROR_RETURN_CODE_SUCCESS,rc = MIRROR_RETURN_CODE_SUCCESS,profile = DEFAULT_MIRROR_PROFILE;	 

	unsigned int port_count = 0,vlan_count = 0,policy_count =0,fdb_count = 0;
	struct mirror_struct_s *destNode = NULL;
    mirror_src_port_list_s* srcPortList = NULL;
    mirror_src_vlan_list_s* srcVlanList = NULL;
	mirror_src_policy_list_s* srcPolicyList = NULL;
	mirror_src_fdb_list_s*   srcFdbList = NULL;
	struct mirror_src_port_node_s* srcPortNode = NULL;
	struct mirror_src_vlan_node_s* srcVlanNode = NULL;
	struct mirror_src_policy_node_s* srcPolicyNode = NULL;
	struct mirror_src_fdb_node_s*  srcFdbNode = NULL;
	struct list_head *pos = NULL,*list = NULL;
	

	dbus_error_init(&err);

	if(!((productinfo.capbmp) & FUNCTION_MIRROR_VLAUE)){
		npd_syslog_dbg("do not support mirror!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_MIRROR_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_MIRROR_VLAUE)){
		npd_syslog_dbg("do not support mirror!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		if (!(dbus_message_get_args ( msg, &err,
									 DBUS_TYPE_UINT32,&profile,
									 DBUS_TYPE_INVALID))) 
		{
			syslog_ax_mirror_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				syslog_ax_mirror_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}
	    syslog_ax_mirror_dbg("show mirror profile %d\n",profile);
		destNode = npd_mirror_get_profile_node(profile);
	    if(NULL == destNode) {
			ret = MIRROR_RETURN_CODE_DESTINATION_NODE_NOTEXIST;
		}
		else {
			/* bi-directional mirror destination port*/
			if((destNode->in_eth_index == destNode->eg_eth_index) && 
				(MIRROR_DEST_INPORT_DEFAULT != destNode->in_eth_index)) {
				bi_flag = 1;
				ingress_eth_index = destNode->in_eth_index;
				slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(ingress_eth_index);
				slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
				port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(ingress_eth_index);
				port_no = ETH_LOCAL_INDEX2NO(slot_index,port_index);
				syslog_ax_mirror_dbg("bidirection mirror target:eth-port(%d) %d/%d\n",	\
							ingress_eth_index,slot_no,port_no);
				ingress_slot_no = ingress_port_no = (unsigned char)MIRROR_DEST_INPORT_DEFAULT;
				egress_slot_no = egress_port_no = (unsigned char)MIRROR_DEST_INPORT_DEFAULT;
			}
			else { /* uni-directional mirror destination port*/
				if(MIRROR_DEST_INPORT_DEFAULT != destNode->in_eth_index){
					in_flag = 1;
		            ingress_eth_index = destNode->in_eth_index;
					slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(ingress_eth_index);
					ingress_slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
					port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(ingress_eth_index);
					ingress_port_no = ETH_LOCAL_INDEX2NO(slot_index,port_index);
					syslog_ax_mirror_dbg("ingress mirror target:eth-port(%d) %d/%d\n",	\
								ingress_eth_index,ingress_slot_no,ingress_port_no);
					if(MIRROR_DEST_EGPORT_DEFAULT == destNode->eg_eth_index){
						egress_slot_no = egress_port_no = (unsigned char)MIRROR_DEST_INPORT_DEFAULT;
						slot_no = port_no = (unsigned char)MIRROR_DEST_INPORT_DEFAULT;
					}
				}
			   if(MIRROR_DEST_EGPORT_DEFAULT != destNode->eg_eth_index){
			   	    eg_flag = 1;
		            egress_eth_index = destNode->eg_eth_index;
					slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(egress_eth_index);
					egress_slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
					port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(egress_eth_index);
					egress_port_no = ETH_LOCAL_INDEX2NO(slot_index,port_index);
					syslog_ax_mirror_dbg("egress mirror target:eth-port(%d) %d/%d\n",	\
								egress_eth_index,egress_slot_no,egress_port_no);
					if(MIRROR_DEST_INPORT_DEFAULT == destNode->in_eth_index){
						ingress_slot_no = ingress_port_no = (unsigned char)MIRROR_DEST_INPORT_DEFAULT;
						slot_no = port_no = (unsigned char)MIRROR_DEST_INPORT_DEFAULT;
					}
				}
			   if((destNode->in_eth_index == destNode->eg_eth_index) && 
				(MIRROR_DEST_INPORT_DEFAULT == destNode->in_eth_index)) {
				    ret = MIRROR_RETURN_CODE_DESTINATION_NODE_NOTEXIST;
			   	}
		    }
		     if((NULL !=(srcPortList = destNode->func[MIRROR_FUNC_PORT]))&&(srcPortList->count > 0)){
		         port_count = srcPortList->count;
			 }
			 if((NULL !=(srcVlanList = destNode->func[MIRROR_FUNC_VLAN]))&&(srcVlanList->count > 0)){
		         vlan_count = srcVlanList->count;
			 }
			 if((NULL !=(srcPolicyList = destNode->func[MIRROR_FUNC_POLICY]))&&(srcPolicyList->count > 0)){
		         policy_count = srcPolicyList->count;
			 }
			 if((NULL !=(srcFdbList = destNode->func[MIRROR_FUNC_FDB]))&&(srcFdbList->count > 0)){
		         fdb_count = srcFdbList->count;
			 }
		}
			
	    syslog_ax_mirror_dbg("port_count %d,vlan_count %d,policy_count %d,fdb_count %d\n",port_count,vlan_count,policy_count,fdb_count);
	}

		reply = dbus_message_new_method_return(msg);	
		dbus_message_iter_init_append (reply, &iter);
		dbus_message_iter_append_basic (&iter,
										 DBUS_TYPE_UINT32,
										 &ret);
		dbus_message_iter_append_basic (&iter,
										 DBUS_TYPE_UINT32,
										 &bi_flag);
		dbus_message_iter_append_basic (&iter,
										 DBUS_TYPE_UINT32,
										 &in_flag);
		dbus_message_iter_append_basic (&iter,
										 DBUS_TYPE_UINT32,
										 &eg_flag);
		if(bi_flag){
		   dbus_message_iter_append_basic (&iter,
											 DBUS_TYPE_BYTE,
											 &slot_no);
			dbus_message_iter_append_basic (&iter,
											 DBUS_TYPE_BYTE,
											  &port_no);

		}	
        if(in_flag){
			dbus_message_iter_append_basic (&iter,
											 DBUS_TYPE_BYTE,
											 &ingress_slot_no);
				
	        dbus_message_iter_append_basic (&iter,
											 DBUS_TYPE_BYTE,
											 &ingress_port_no);
        }
		if(eg_flag){
	        dbus_message_iter_append_basic (&iter,
											 DBUS_TYPE_BYTE,
											 &egress_slot_no);
				
	        dbus_message_iter_append_basic (&iter,
											 DBUS_TYPE_BYTE,
											 &egress_port_no);
		}
		dbus_message_iter_append_basic (&iter,
										 DBUS_TYPE_UINT32,
										 &port_count);

		dbus_message_iter_append_basic (&iter,
										 DBUS_TYPE_UINT32,
										 &vlan_count);	
		dbus_message_iter_append_basic (&iter,
										 DBUS_TYPE_UINT32,
										 &policy_count);
		dbus_message_iter_append_basic (&iter,
										 DBUS_TYPE_UINT32,
										 &fdb_count);
		syslog_ax_mirror_dbg("ret %d,rc %d,port_count %d,vlan_count %d,policy_count %d,fdb_count %d\n",ret,rc,port_count,vlan_count,policy_count,fdb_count);

		if(port_count > 0){
			DBusMessageIter		iter_array;
			/*open outer container*/
			dbus_message_iter_open_container (&iter,
								   DBUS_TYPE_ARRAY,
									  DBUS_STRUCT_BEGIN_CHAR_AS_STRING /*begin*/
											DBUS_TYPE_BYTE_AS_STRING 
											DBUS_TYPE_BYTE_AS_STRING 
											DBUS_TYPE_UINT32_AS_STRING
									  DBUS_STRUCT_END_CHAR_AS_STRING, /*end*/
								   &iter_array);
			list = &(srcPortList->list);
			__list_for_each(pos,list) {
					srcPortNode = list_entry(pos,struct mirror_src_port_node_s,list);
					DBusMessageIter iter_struct;
					dbus_message_iter_open_container (&iter_array,
													   DBUS_TYPE_STRUCT,
													   NULL,
													   &iter_struct);
					syslog_ax_mirror_dbg("srcPortNode index %d\n",srcPortNode->eth_g_index);
					slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(srcPortNode->eth_g_index);
					slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
					port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(srcPortNode->eth_g_index);
					port_no = ETH_LOCAL_INDEX2NO(slot_index,port_index);
					dbus_message_iter_append_basic
							(&iter_struct,
							  DBUS_TYPE_BYTE,
							  &slot_no);
					dbus_message_iter_append_basic
							(&iter_struct,
							  DBUS_TYPE_BYTE,
							  &port_no);
					dbus_message_iter_append_basic
							(&iter_struct,
							  DBUS_TYPE_UINT32,
							  &srcPortNode->direct);
					dbus_message_iter_close_container (&iter_array, &iter_struct);
				}
			dbus_message_iter_close_container (&iter, &iter_array);

		}

        if(vlan_count > 0){
			DBusMessageIter		iter_array;
			/*open outer container*/
			dbus_message_iter_open_container (&iter,
									   DBUS_TYPE_ARRAY,
										  DBUS_STRUCT_BEGIN_CHAR_AS_STRING /*begin*/
												DBUS_TYPE_UINT16_AS_STRING 
										  DBUS_STRUCT_END_CHAR_AS_STRING, /*end*/
									   &iter_array);

			list = &(srcVlanList->list);
			__list_for_each(pos,list) {
			    srcVlanNode = list_entry(pos,struct mirror_src_vlan_node_s,list);
				DBusMessageIter iter_struct;
				dbus_message_iter_open_container (&iter_array,
												   DBUS_TYPE_STRUCT,
												   NULL,
												   &iter_struct);
				syslog_ax_mirror_dbg("srcVlanNode vid %d\n",srcVlanNode->vid);
				
				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_UINT16,
						  &(srcVlanNode->vid));
				dbus_message_iter_close_container (&iter_array, &iter_struct);
			}
		 dbus_message_iter_close_container (&iter, &iter_array);
      }
	  if(policy_count > 0){
		  	DBusMessageIter		iter_array;
		  	/*open outer container*/
			dbus_message_iter_open_container (&iter,
								   DBUS_TYPE_ARRAY,
									  DBUS_STRUCT_BEGIN_CHAR_AS_STRING /*begin*/
											DBUS_TYPE_UINT32_AS_STRING 
									  DBUS_STRUCT_END_CHAR_AS_STRING, /*end*/
								   &iter_array);
	  	    list = &(srcPolicyList->list);
			__list_for_each(pos,list) {
			    srcPolicyNode = list_entry(pos,struct mirror_src_policy_node_s,list);
				DBusMessageIter iter_struct;
				dbus_message_iter_open_container (&iter_array,
												   DBUS_TYPE_STRUCT,
												   NULL,
												   &iter_struct);
				syslog_ax_mirror_dbg("srcPolicyNode ruleIndex %d\n",srcPolicyNode->ruleIndex);
				
				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_UINT32,
						  &(srcPolicyNode->ruleIndex));
				dbus_message_iter_close_container (&iter_array, &iter_struct);
			}
		dbus_message_iter_close_container (&iter, &iter_array);

	  }

	  if(fdb_count > 0){
		  	DBusMessageIter		iter_array;
		  	/*open outer container*/
		    dbus_message_iter_open_container (&iter,
								   DBUS_TYPE_ARRAY,
									  DBUS_STRUCT_BEGIN_CHAR_AS_STRING /*begin*/
											DBUS_TYPE_UINT16_AS_STRING /*vidx*/
											DBUS_TYPE_BYTE_AS_STRING /*mac[0]*/
										   	DBUS_TYPE_BYTE_AS_STRING /*mac[1]*/
										   	DBUS_TYPE_BYTE_AS_STRING /*mac[2]*/
										   	DBUS_TYPE_BYTE_AS_STRING /*mac[3]*/
										   	DBUS_TYPE_BYTE_AS_STRING /*mac[4]*/
										   	DBUS_TYPE_BYTE_AS_STRING /*mac[5]*/
										    DBUS_TYPE_BYTE_AS_STRING /*slot*/
										   	DBUS_TYPE_BYTE_AS_STRING /*port*/
									  DBUS_STRUCT_END_CHAR_AS_STRING, /*end*/
								   &iter_array);
	  	   list = &(srcFdbList->list);
			__list_for_each(pos,list) {
			    srcFdbNode = list_entry(pos,struct mirror_src_fdb_node_s,list);

				DBusMessageIter iter_struct;
				dbus_message_iter_open_container (&iter_array,
												   DBUS_TYPE_STRUCT,
												   NULL,
												   &iter_struct);
				syslog_ax_mirror_dbg("srcFdbNode vid %d,mac %02x:%02x:%02x:%02x:%02x:%02x slot_no %d,port_no %d\n",srcFdbNode->vid,   \
					                      srcFdbNode->mac[0],srcFdbNode->mac[1],srcFdbNode->mac[2],srcFdbNode->mac[3],srcFdbNode->mac[4],srcFdbNode->mac[5],\
					                      srcFdbNode->slot_no,srcFdbNode->port_no);
				
				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_UINT16,
						  &(srcFdbNode->vid));
				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_BYTE,
						  &(srcFdbNode->mac[0]));
				dbus_message_iter_append_basic
						(&iter_struct,
						   DBUS_TYPE_BYTE,
						  &(srcFdbNode->mac[1]));
				dbus_message_iter_append_basic
						(&iter_struct,
						   DBUS_TYPE_BYTE,
						  &(srcFdbNode->mac[2]));
				dbus_message_iter_append_basic
						(&iter_struct,
						   DBUS_TYPE_BYTE,
						  &(srcFdbNode->mac[3]));
				dbus_message_iter_append_basic
						(&iter_struct,
						   DBUS_TYPE_BYTE,
						  &(srcFdbNode->mac[4]));
				dbus_message_iter_append_basic
						(&iter_struct,
						   DBUS_TYPE_BYTE,
						  &(srcFdbNode->mac[5]));
				dbus_message_iter_append_basic
						(&iter_struct,
						   DBUS_TYPE_BYTE,
						  &(srcFdbNode->slot_no));
				dbus_message_iter_append_basic
						(&iter_struct,
						   DBUS_TYPE_BYTE,
						  &(srcFdbNode->port_no));
				dbus_message_iter_close_container (&iter_array, &iter_struct);
			}
         dbus_message_iter_close_container (&iter, &iter_array);
	  }
	return reply;
}

DBusMessage * npd_dbus_mirror_delete(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter= {0};
	DBusError			err;   	

	unsigned char       slot_index = 0,port_index = 0,slot_no = 0,port_no = 0;
	unsigned char devNum = 0,portNum = 0;
	unsigned int	    eth_g_index=0, ret = MIRROR_RETURN_CODE_SUCCESS,profile = DEFAULT_MIRROR_PROFILE;	 
	ETHERADDR macAddr;
	unsigned char *mac = NULL;
	struct mirror_struct_s *destNode = NULL;
    mirror_src_port_list_s* srcPortList = NULL;
    mirror_src_vlan_list_s* srcVlanList = NULL;
	mirror_src_policy_list_s* srcPolicyList = NULL;
	mirror_src_fdb_list_s*   srcFdbList = NULL;
	struct mirror_src_port_node_s* srcPortNode = NULL;
	struct mirror_src_vlan_node_s* srcVlanNode = NULL;
	struct mirror_src_policy_node_s* srcPolicyNode = NULL;
	struct mirror_src_fdb_node_s*  srcFdbNode = NULL;
	struct list_head *pos = NULL,*list = NULL;
	unsigned int groupNum = 0,exist_dir = 0;
	AclRuleList  *rulePtr=NULL;
	unsigned int destDev = 0;
	unsigned int destPort = 0;

    memset(&macAddr,0,sizeof(ETHERADDR));
	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
								 DBUS_TYPE_UINT32,&profile,
								 DBUS_TYPE_INVALID))) 
	{
		npd_syslog_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			npd_syslog_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}return NULL;
	}	


#if 1
  /* for(profile = 0;profile < MAX_MIRROR_PROFILE;profile++){*/
		destNode = npd_mirror_get_profile_node(profile);
	    if(NULL == destNode) {
			ret = MIRROR_RETURN_CODE_DESTINATION_NODE_NOTEXIST;
		}
		else if((MIRROR_DEST_INPORT_DEFAULT == destNode->in_eth_index)&& \
			(MIRROR_DEST_EGPORT_DEFAULT == destNode->eg_eth_index)){
            ret = MIRROR_RETURN_CODE_DESTINATION_NODE_NOTEXIST;
		}
		else if(MIRROR_DEST_INPORT_DEFAULT != destNode->in_eth_index){
			eth_g_index = destNode->in_eth_index;
			slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
			slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
			port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
			port_no = ETH_LOCAL_INDEX2NO(slot_index,eth_g_index);
			
			 npd_syslog_dbg("eth_g_index %d,slot_no %d,port_no %d\n",eth_g_index,slot_no,port_no);
			 
             if((NULL !=(srcPortList = destNode->func[MIRROR_FUNC_PORT]))&&(srcPortList->count > 0)){
			 	list = &(srcPortList->list);
			    __list_for_each(pos,list) {
					srcPortNode = list_entry(pos,struct mirror_src_port_node_s,list);
					npd_syslog_dbg("srcPortNode index %d\n",srcPortNode->eth_g_index);
					slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(srcPortNode->eth_g_index);
					slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
					port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(srcPortNode->eth_g_index);
					port_no = ETH_LOCAL_INDEX2NO(slot_index,srcPortNode->eth_g_index);
		            npd_get_devport_by_global_index(srcPortNode->eth_g_index, &devNum, &portNum);
                    if(0 != nam_mirror_port_set(devNum,portNum,0,srcPortNode->direct,profile)) {
					    npd_syslog_err("Delete src port error! devNum %d,portNum %d,direct %d\n",devNum,portNum,srcPortNode->direct);
						ret =  MIRROR_RETURN_CODE_ERROR;
					}								
					else if(0 != npd_mirror_nam_mirror_port_del_for_bcm(profile,srcPortNode->direct,srcPortNode->eth_g_index)){
						npd_syslog_err("Delete src port error! devNum %d,portNum %d,direct %d\n",devNum,portNum,srcPortNode->direct);
						ret = MIRROR_RETURN_CODE_ERROR;
					}
					else {
						ret = npd_mirror_src_port_remove(profile,srcPortNode->direct,srcPortNode->eth_g_index);
					}
				}
             }
           
			 if((NULL !=(srcVlanList = destNode->func[MIRROR_FUNC_VLAN]))&&(srcVlanList->count > 0)){
                 list = &(srcVlanList->list);
			     __list_for_each(pos,list) {
				     srcVlanNode = list_entry(pos,struct mirror_src_vlan_node_s,list);
					 npd_syslog_dbg("srcVlanNode vid %d\n",srcVlanNode->vid);
                     if(0 != nam_mirror_vlan_set(srcVlanNode->vid,0)) {
					 	 npd_syslog_dbg("Delete src vlan error! vid %d\n",srcVlanNode->vid);
						 ret = MIRROR_RETURN_CODE_ERROR;
					 }
					 else{
					 	 if(MIRROR_DEST_INPORT_DEFAULT != destNode->in_eth_index){
						     npd_get_devport_by_global_index(destNode->in_eth_index,&destDev,&destPort);
							 ret = nam_mirror_vlan_set_for_bcm(srcVlanNode->vid,destDev,destPort,0);
							 if(0 == ret){
							 	if(MIRROR_DEST_INPORT_DEFAULT != destNode->in_eth_index){
								     npd_get_devport_by_global_index(destNode->eg_eth_index,&destDev,&destPort);
									 ret = nam_mirror_vlan_set_for_bcm(srcVlanNode->vid,destDev,destPort,0);
									 if(0 != ret){
		                                 npd_syslog_dbg("Delete src vlan for ingress error! vid %d\n",srcVlanNode->vid);
								 	     ret = MIRROR_RETURN_CODE_ERROR;
									 }
							 	}
							 }
							 else{
							 	npd_syslog_dbg("Delete src vlan for egress error! vid %d\n",srcVlanNode->vid);
							 	ret = MIRROR_RETURN_CODE_ERROR;
							 }
						 }
					 }
					 if(0 == ret){
						npd_mirror_src_vlan_remove(profile,srcVlanNode->vid);
					 }
			     }
			 }
			 if((NULL !=(srcPolicyList = destNode->func[MIRROR_FUNC_POLICY]))&&(srcPolicyList->count > 0)){
                 list = &(srcPolicyList->list);
			     __list_for_each(pos,list) {
				    srcPolicyNode = list_entry(pos,struct mirror_src_policy_node_s,list);
					npd_syslog_dbg("srcPolicyNode ruleIndex %d\n",srcPolicyNode->ruleIndex); 		
					if(ACL_GROUP_RULE_EXISTED == search_acl_group_rule_index_exist(srcPolicyNode->ruleIndex,&groupNum,&exist_dir)){
						/*update HW action*/
						ret = nam_acl_mirror_action_update(srcPolicyNode->ruleIndex,0,0);
						if(ret!=0)
							syslog_ax_mirror_err("del policy-source %d action update fail %d!\n",srcPolicyNode->ruleIndex,ret);				
					}	
					rulePtr = g_acl_rule[srcPolicyNode->ruleIndex];
					rulePtr->appendInfo.appendType =0;
					rulePtr->ActionPtr.actionType=ACL_ACTION_TYPE_PERMIT;
					rulePtr->ActionPtr.actionInfo.mirror.portNum =0;	
					ret = npd_mirror_src_acl_remove(profile,srcPolicyNode->ruleIndex);
			     }
			 }
			 if((NULL !=(srcFdbList = destNode->func[MIRROR_FUNC_FDB]))&&(srcFdbList->count > 0)){
                 list = &(srcFdbList->list);
			     __list_for_each(pos,list) {
					    srcFdbNode = list_entry(pos,struct mirror_src_fdb_node_s,list);
						memcpy(macAddr.arEther,srcFdbNode->mac,6);
						mac = macAddr.arEther;
						npd_syslog_dbg("srcFdbNode vid %d,mac %#02x:%#02x:%#02x:%#02x:%#02x:%#02x slot_no %d,port_no %d\n",srcFdbNode->vid,   \
							                      srcFdbNode->mac[0],srcFdbNode->mac[1],srcFdbNode->mac[2],srcFdbNode->mac[3],srcFdbNode->mac[4],srcFdbNode->mac[5],\
					                              srcFdbNode->slot_no,srcFdbNode->port_no);
						eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(srcFdbNode->slot_no,srcFdbNode->port_no);
						if(0 ==( ret = nam_mirror_fdb_entry_set(srcFdbNode->vid,eth_g_index,&macAddr,0))) {
						   if(0 != npd_fdb_static_mirror_entry_del(mac,srcFdbNode->vid)) {
                               ret = MIRROR_RETURN_CODE_ERROR;
						   }
						   else {
			                  npd_mirror_src_fdb_remove(profile,srcFdbNode->vid,mac,srcFdbNode->slot_no,srcFdbNode->port_no);
						  }
			           }
			     }
		     }	
   	     }
		 else if(MIRROR_DEST_EGPORT_DEFAULT != destNode->eg_eth_index){/*if ingress node exist ,do not care this */
			eth_g_index = destNode->eg_eth_index;
			slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
			slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
			port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
			port_no = ETH_LOCAL_INDEX2NO(slot_index,eth_g_index);
			
			 npd_syslog_dbg("eth_g_index %d,slot_no %d,port_no %d\n",eth_g_index,slot_no,port_no);
			 
             if((NULL !=(srcPortList = destNode->func[MIRROR_FUNC_PORT]))&&(srcPortList->count > 0)){
			 	list = &(srcPortList->list);
			    __list_for_each(pos,list) {
					srcPortNode = list_entry(pos,struct mirror_src_port_node_s,list);
					npd_syslog_dbg("srcPortNode index %d\n",srcPortNode->eth_g_index);
					slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(srcPortNode->eth_g_index);
					slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
					port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(srcPortNode->eth_g_index);
					port_no = ETH_LOCAL_INDEX2NO(slot_index,srcPortNode->eth_g_index);
		            npd_get_devport_by_global_index(srcPortNode->eth_g_index, &devNum, &portNum);
                    if(0 != nam_mirror_port_set(devNum,portNum,0,srcPortNode->direct,profile)) {
					    npd_syslog_err("Delete src port error! devNum %d,portNum %d,direct %d\n",devNum,portNum,srcPortNode->direct);
						ret =  MIRROR_RETURN_CODE_ERROR;
					}								
					else if(0 != npd_mirror_nam_mirror_port_del_for_bcm(profile,srcPortNode->direct,srcPortNode->eth_g_index)){
						npd_syslog_err("Delete src port error! devNum %d,portNum %d,direct %d\n",devNum,portNum,srcPortNode->direct);
						ret = MIRROR_RETURN_CODE_ERROR;
					}
					else {
						ret = npd_mirror_src_port_remove(profile,srcPortNode->direct,srcPortNode->eth_g_index);
					}
				}
             }
		 }
   /* }*/
#endif
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,
								   DBUS_TYPE_UINT32,
								   &ret);
	return reply;
}

DBusMessage * npd_dbus_mirror_show_running_cfg(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter= {0};
	DBusError			err;   		
	char *strShow = NULL;
	strShow = (char*)malloc(MIRROR_SAVE_CFG_MEM);
	if(NULL == strShow) {
		syslog_ax_mirror_err("alloc memory fail when mirror show running-config\n");
		return NULL;
	}
	memset(strShow,0,MIRROR_SAVE_CFG_MEM);

	dbus_error_init(&err);

	npd_mirror_profile_config_save(strShow);

	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,
								   DBUS_TYPE_STRING,
								   &strShow);

	free(strShow);
	strShow = NULL;
	return reply;
}

#ifdef __cplusplus
}
#endif 
