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
* npd_qos.c
*
*
* CREATOR:
*		sulong@autelan.com
*
* DESCRIPTION:
*		API used in NPD dbus process for QOS  module
*
* DATE:
*		02/21/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.80 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sysdef/npd_sysdef.h"
#include "util/npd_list.h"
#include "npd/nam/npd_amapi.h"
#include "npd/nbm/npd_bmapi.h"
#include "npd_c_slot.h"
#include "npd_eth_port.h"
#include "npd_log.h"
#include "npd_product.h"
#include "npd_qos.h"
#include "npd_acl_drv.h"
#include "npd_cpss_acl_def.h"
#include "dbus/npd/npd_dbus_def.h"
#include "sysdef/returncode.h"

QOS_PROFILE_STC 		   **g_qos_profile=NULL,**g_tmp_qos=NULL;
QOS_UP_PROFILE_MAP_STC     **g_up_profile = NULL;
QOS_POLICER_STC				**g_policer = NULL;
QOS_POLICER_ALIAS			**alias_policer=NULL;	
QOS_DSCP_PROFILE_MAP_STC   **g_dscp_profile = NULL;
QOS_DSCP_DSCP_REMAP_STC    **g_dscp_dscp = NULL;
QOS_PORT_POLICY_MAP_ATTRIBUTE_STC **g_policy_map = NULL;
QOS_GLOBAL_POLICER_STC		 g_g_policer;
QOS_COUNTER_STC				**g_counter=NULL;	
QOS_WRR_TX_WEIGHT_E	*g_queue=NULL,*g_hybrid=NULL,*g_queue_showrunn=NULL,*g_hybrid_showrunn=NULL;				
unsigned int				 g_queue_type =QOS_PORT_TX_DEFAULT_WRR_ARB_E;
QOS_MODE qos_mode = QOS_MODE_DEFAULT;

void npd_qos_init(void) {
	int n=0,h=0,p=0;
	unsigned int ret = QOS_RETURN_CODE_SUCCESS;
		
	g_qos_profile = (QOS_PROFILE_STC**)malloc(sizeof(QOS_PROFILE_STC*)*MAX_PROFILE_NUMBER);
	if(NULL == g_qos_profile) {
		npd_syslog_dbg("memory alloc error for qos init!!");
		goto errorRet;		
	}
	g_tmp_qos = (QOS_PROFILE_STC**)malloc(sizeof(QOS_PROFILE_STC*)*MAX_PROFILE_NUMBER);
	if(NULL == g_tmp_qos) {
		npd_syslog_dbg("memory alloc error for qos init!!");
		goto errorRet;		
	}
	g_up_profile = (QOS_UP_PROFILE_MAP_STC**)malloc(sizeof(QOS_UP_PROFILE_MAP_STC*)*MAX_UP_PROFILE_NUM);
	if(NULL == g_up_profile) {
		npd_syslog_dbg("memory alloc error for qos init!!");
		goto errorRet;	
	}	
	g_dscp_profile =(QOS_DSCP_PROFILE_MAP_STC**) malloc(sizeof(QOS_DSCP_PROFILE_MAP_STC*)*MAX_DSCP_PROFILE_NUM);
	if(NULL == g_dscp_profile) {
		npd_syslog_dbg("memory alloc error for qos init!!");
		goto errorRet;
	}	
	g_dscp_dscp = (QOS_DSCP_DSCP_REMAP_STC**)malloc(sizeof(QOS_DSCP_DSCP_REMAP_STC*)*MAX_DSCP_DSCP_NUM);
	if(NULL == g_dscp_dscp) {
		npd_syslog_dbg("memory alloc error for qos init!!");
		goto errorRet;
	}	
	g_policy_map = (QOS_PORT_POLICY_MAP_ATTRIBUTE_STC**)malloc(sizeof(QOS_PORT_POLICY_MAP_ATTRIBUTE_STC*)*MAX_POLICY_MAP_NUM);
	if(NULL == g_policy_map) {
		npd_syslog_dbg("memory alloc error for qos init!!");
		goto errorRet;
	}
	g_policer = (QOS_POLICER_STC**)malloc(sizeof(QOS_POLICER_STC*)*MAX_POLICER_NUM);
	if(NULL == g_policer) {
		npd_syslog_dbg("memory alloc error for qos init!!");
		goto errorRet;
	}
	alias_policer = (QOS_POLICER_ALIAS**)malloc(sizeof(QOS_POLICER_ALIAS*)*MAX_POLICER_NUM);
	if(NULL == alias_policer) {
		npd_syslog_dbg("memory alloc error for qos init!!");
		goto errorRet;
	}
	g_counter = (QOS_COUNTER_STC**)malloc(sizeof(QOS_COUNTER_STC*)*MAX_COUNTER_NUM);
	if(NULL == g_counter) {
		npd_syslog_dbg("memory alloc error for qos init!!");
		goto errorRet;
	}
	g_queue = (QOS_WRR_TX_WEIGHT_E*)malloc(sizeof(QOS_WRR_TX_WEIGHT_E)*8);
	if(NULL == g_queue) {
		npd_syslog_dbg("memory alloc error for qos init!!");
		goto errorRet;
	}
	g_queue_showrunn= (QOS_WRR_TX_WEIGHT_E*)malloc(sizeof(QOS_WRR_TX_WEIGHT_E)*8);
	if(NULL == g_queue_showrunn) {
		npd_syslog_dbg("memory alloc error for qos init!!");
		goto errorRet;
	}
	g_hybrid = (QOS_WRR_TX_WEIGHT_E*)malloc(sizeof(QOS_WRR_TX_WEIGHT_E)*8);
	if(NULL == g_hybrid) {
		npd_syslog_dbg("memory alloc error for qos init!!");
		goto errorRet;
	}
	g_hybrid_showrunn= (QOS_WRR_TX_WEIGHT_E*)malloc(sizeof(QOS_WRR_TX_WEIGHT_E)*8);
	if(NULL == g_hybrid_showrunn) {
		npd_syslog_dbg("memory alloc error for qos init!!");
		goto errorRet;
	}
	
	ret = npd_qos_mode_def();
	if (QOS_RETURN_CODE_SUCCESS != ret) {
		npd_syslog_dbg("npd_qos_mode_def for qos init!!");
	}
	else
	{
		npd_syslog_dbg("npd_qos_mode_def for qos init OK!\n");		
	}
	/*
	for(i=0;i<MAX_PROFILE_NUMBER;i++) {		
		g_qos_profile[i] = NULL;
		if(i<MAX_PROFILE_NUMBER - 64)
		{
			
			g_tmp_qos[i]=(QOS_PROFILE_STC*)malloc(sizeof(QOS_PROFILE_STC);
			if(NULL==g_tmp_qos[i]){
				npd_syslog_dbg("memory alloc error for qos init!!");
				return;
			}
			else{
				memset(g_tmp_qos[i],0,sizeof(g_tmp_qos[i]);
				g_tmp_qos[i]->dscp =i;
				if(i>7){
					g_tmp_qos[i]->userPriority = 0;
					g_tmp_qos[i]->trafficClass = 0;
				}
				else if(i<8){
					g_tmp_qos[i]->trafficClass = i;
					g_tmp_qos[i]->userPriority = i;
				}
			}
			profilePtr = g_tmp_qos[i];			
			ret=nam_qos_profile_entry_set(i,profilePtr);
	          if(ret!=QOS_RETURN_CODE_SUCCESS)
	             npd_syslog_dbg("qos profile entry set fail"); 
			}
	}
	for(;j<MAX_UP_PROFILE_NUM;j++) {
		g_up_profile[j] = NULL;
		ret = nam_qos_up_to_profile_entry(j,j);
		if(ret!=QOS_RETURN_CODE_SUCCESS)
			npd_syslog_dbg("fail to set up to qos profile map table entry "); 
	}
	for(;k<MAX_DSCP_PROFILE_NUM;k++) {
		g_dscp_profile[k] = NULL;
		
		ret = nam_qos_dscp_to_profile_entry(k,k);
		if(ret!=QOS_RETURN_CODE_SUCCESS)
			npd_syslog_dbg("fail to set dscp to profile map table entry ");		
	}
	
	for(i=0;i<MAX_PROFILE_NUMBER;i++) {		
		g_qos_profile[i] = NULL;
	}	
	for(;l<MAX_DSCP_DSCP_NUM;l++) {
		g_dscp_dscp[l] = NULL;
		ret = nam_qos_dscp_to_dscp_entry(l,l);
    	if(ret!=QOS_RETURN_CODE_SUCCESS)		 
           npd_syslog_dbg("fail to set dscp to dscp map table entry "); 
 
	}
	
	for(;m<MAX_POLICY_MAP_NUM;m++) {
		g_policy_map[m] = NULL;
	}
	*/
	for(;n<MAX_POLICER_NUM;n++) {
		g_policer[n] = NULL;
		alias_policer[n] = NULL;
	}
	for(;h<MAX_COUNTER_NUM;h++) {
		g_counter[h] = NULL;
	}
	for(;p<8;p++) {
		g_queue[p].groupFlag=QOS_PORT_TX_WRR_ARB_GROUP0_E;
		g_queue[p].weight = p+1;
		g_queue_showrunn[p].groupFlag=QOS_PORT_TX_WRR_ARB_GROUP0_E;
		g_queue_showrunn[p].weight =0;
		g_hybrid[p].groupFlag = QOS_PORT_TX_SP_WRR_ARB_E;
		g_hybrid[p].weight = 0;
		g_hybrid_showrunn[p].groupFlag = QOS_PORT_TX_SP_WRR_ARB_E;
		g_hybrid_showrunn[p].weight = 0;
	}

	memset(&g_g_policer,0,sizeof(QOS_GLOBAL_POLICER_STC));
	g_g_policer.policing=POLICER_PACKET_SIZE_L1_INCLUDE_E;

	/*if(PRODUCT_ID_AX5K_E == PRODUCT_ID || PRODUCT_ID_AX5608== PRODUCT_ID){/*5612e not support*/
	/*	return;
	}*/
	if(!((productinfo.capbmp) & FUNCTION_QOS_VLAUE)){
		npd_syslog_dbg("do not support qos initial!\n");
		return;
	}
	else if(((productinfo.capbmp) & FUNCTION_QOS_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_QOS_VLAUE)){
		npd_syslog_dbg("do not support qos initial!\n");
		return;
	}
	else{
		ret = nam_qos_queue_init();
		if(QOS_RETURN_CODE_SUCCESS != ret) {
			npd_syslog_err("nam_qos_queue_init fail!");
		}
    	else
    	{
    		npd_syslog_dbg("nam_qos_queue_init() init OK!\n");		
    	}
		
		ret = nam_qos_policer_init();
		if(QOS_RETURN_CODE_SUCCESS != ret) {
			npd_syslog_err("policer init error!");
		}
		else
		{
		    npd_syslog_dbg("nam_qos_policer_init() init OK! \n");
		}
		return ret;
		
		errorRet:
		if(g_qos_profile) {
			free(g_qos_profile);
			g_qos_profile = NULL;
		}
		if(g_tmp_qos) {
			free(g_tmp_qos);
			g_tmp_qos = NULL;
		}
		if(g_up_profile)	{
			free(g_up_profile);
			g_up_profile = NULL;
		}	
		if(g_dscp_profile) 	{
			free(g_dscp_profile);
			g_dscp_profile = NULL;
		}
		if(g_dscp_dscp) {
			free(g_dscp_dscp);
			g_dscp_dscp = NULL;
		}	
		if(g_policy_map)	{
			free(g_policy_map);
			g_policy_map = NULL;
		}	
		if(g_policer)	{
			free(g_policer);
			g_policer = NULL;
		}	
		if(g_counter)	{
			free(g_counter);
			g_counter = NULL;
		}
		if(g_queue) {
			free(g_queue);
			g_queue = NULL;
		}	
		if(g_queue_showrunn)	{
			free(g_queue_showrunn);
			g_queue_showrunn = NULL;
		}
		if(g_hybrid) {
			free(g_hybrid);
			g_hybrid = NULL;
		}
		if(g_hybrid_showrunn)	{
			free(g_hybrid_showrunn);
			g_hybrid_showrunn = NULL;
		}	
	}
}

unsigned int npd_qos_port_unbind_opt(unsigned int eth_g_index,unsigned int policyMapIndex)
{
	struct eth_port_s				   *portInfo = NULL;
	QOS_PORT_POLICY_MAP_INFO_STC	   *eth_policy=NULL;
	unsigned int					   ret = QOS_RETURN_CODE_ERROR;
	QOS_PORT_POLICY_MAP_ATTRIBUTE_STC  *poMap=NULL;
	QOS_POLICY_MAP_ETH_INDEX_STC		*ruleInfo =NULL;
	struct list_head					*Ptr=NULL;
	/*unsigned int 						count=0;*/
		
	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) {
		ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
	}
	else{ 
		 eth_policy=portInfo->funcs.func_data[ETH_PORT_FUNC_QoS_PROFILE];
		 if(eth_policy!=NULL){
			if(policyMapIndex==eth_policy->policyMapId){
				/*clear map info about eth*/
				if ((NULL != g_policy_map) && (NULL != g_policy_map[policyMapIndex])){
					
					poMap = g_policy_map[policyMapIndex];

					/*npd_syslog_dbg("eth port count %d\n",poMap->eth_count);*/
					__list_for_each(Ptr,&(poMap->list)){
						/*npd_syslog_dbg(" pre count %d ptr %p\n",count,Ptr);*/
						ruleInfo = list_entry(Ptr,QOS_POLICY_MAP_ETH_INDEX_STC,list);
						/*npd_syslog_dbg("ruleInfo node %p\n",ruleInfo);*/
						if(NULL!=ruleInfo){			
							if(eth_g_index == ruleInfo->ethIndex){
								__list_del(ruleInfo->list.prev,ruleInfo->list.next);
								free(ruleInfo);
								ruleInfo=NULL;
								poMap->eth_count-=1;
								ret =QOS_RETURN_CODE_SUCCESS;
								/*count++;*/						
							}						
						}
					  } /*__list_for_each*/
					  if(ret==QOS_RETURN_CODE_SUCCESS){
							portInfo->funcs.func_data[ETH_PORT_FUNC_QoS_PROFILE]=NULL;
							free(eth_policy);
					}					
				}			
			}						
		 }				
	}
	
	return ret;
}

unsigned int npd_qos_unbind_remap_port(void)
{
	unsigned int	i = 0, j = 0, portnum = 0;
	unsigned char	   devNum=0,virPort=0;
	unsigned int	   bindId = 0,ret = QOS_RETURN_CODE_SUCCESS, portInfo=NPD_DBUS_ERROR_NO_SUCH_PORT;

	for (i = 0; i < CHASSIS_SLOT_COUNT; i++ ) {
		syslog_ax_eth_port_dbg("creating eth-port on slot %d module %s",	\
				i,module_id_str(MODULE_TYPE_ON_SLOT_INDEX(i)));
		for (j = 0; j < ETH_LOCAL_PORT_COUNT(i); j++) {
			portnum = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(i, j);
			portInfo = npd_get_devport_by_global_index(portnum,&devNum,&virPort);
			if(portInfo!=QOS_RETURN_CODE_SUCCESS) {
				/*ret = NPD_DBUS_ERROR_NO_SUCH_PORT;*/
				continue;
			}			
			ret = npd_qos_port_bind_check(portnum,&bindId);
			npd_syslog_dbg("end for npd_qos_port_bind_check bindId %d\n", bindId);
			if(ret == NPD_DBUS_ERROR_NO_SUCH_PORT) {
				npd_syslog_dbg("wrong eth_g_index \n");
			}
			else if(ret==QOS_RETURN_CODE_SUCCESS) {
				/*ret = QOS_RETURN_CODE_POLICY_NOT_EXISTED; no policy on port*/
				npd_syslog_dbg("no policy map on port!\n");
			}
			else if(ret==QOS_RETURN_CODE_POLICY_MAP_BIND){
				npd_syslog_dbg("policy map %d on port \n",bindId);
				ret = nam_qos_port_unbind_policy_map(devNum,virPort,bindId);
				if(ret!=QOS_RETURN_CODE_SUCCESS) {
					npd_syslog_dbg("hw clear register fail \n"); 					
				}
				else{
					ret = npd_qos_port_unbind_opt(portnum,bindId);
					if(ret != QOS_RETURN_CODE_SUCCESS) {
						ret = QOS_RETURN_CODE_ERROR;
					}
				}	
			}
		}
	}
	
	return ret;
}

unsigned int npd_qos_mode_def(void)
{
	int i = 0, j = 0, k = 0, l = 0, m = 0, ret = QOS_RETURN_CODE_SUCCESS;
	QOS_PROFILE_STC  *profilePtr = NULL;
	
	for(i= 1;i < MAX_PROFILE_NUMBER; i++) 
	{		
		g_qos_profile[i] = NULL;
		if (i <= MAX_DSCP_PROFILE_NUM) 
		{
			g_tmp_qos[i]=(QOS_PROFILE_STC*)malloc(sizeof(QOS_PROFILE_STC));
			if(NULL == g_tmp_qos[i]) 
			{
				npd_syslog_dbg("memory alloc error for qos init!!");
				return QOS_RETURN_CODE_ERROR;
			}
			else 
			{
				/*code optimize: Wrong sizeof argument
				zhangcl@autelan.com 2013-1-17*/
				memset(g_tmp_qos[i],0,sizeof(QOS_PROFILE_STC));
				g_tmp_qos[i]->dscp = i-1;
				if (i > MAX_UP_PROFILE_NUM) 
				{
					g_tmp_qos[i]->userPriority = 0;
					g_tmp_qos[i]->trafficClass = 0;
				}
				else if (i <= MAX_UP_PROFILE_NUM) 
				{
					g_tmp_qos[i]->trafficClass = i-1;
					g_tmp_qos[i]->userPriority = i-1;
				}
			}
			profilePtr = g_tmp_qos[i];			
			ret=nam_qos_profile_entry_set(i,profilePtr);
			if( QOS_RETURN_CODE_SUCCESS != ret) 
			{
				npd_syslog_dbg("qos profile entry set fail");
			}
		}
	}
	for(;j<MAX_UP_PROFILE_NUM;j++) 
	{
		g_up_profile[j] = NULL;
		ret = nam_qos_up_to_profile_entry(j,j+1,0,ACL_ANY_PORT_CHAR);
		if(ret!=QOS_RETURN_CODE_SUCCESS)
			npd_syslog_dbg("fail to set up to qos profile map table entry "); 
	}
	for(;k<MAX_DSCP_PROFILE_NUM;k++) 
	{
		g_dscp_profile[k] = NULL;
		
		ret = nam_qos_dscp_to_profile_entry(k,k+1,0,ACL_ANY_PORT_CHAR);
		if(ret!=QOS_RETURN_CODE_SUCCESS)
			npd_syslog_dbg("fail to set dscp to profile map table entry ");		
	}	
	for(;l<MAX_DSCP_DSCP_NUM;l++) 
	{
		g_dscp_dscp[l] = NULL;
		ret = nam_qos_dscp_to_dscp_entry(l,l);
    		if(ret!=QOS_RETURN_CODE_SUCCESS)		 
          		 npd_syslog_dbg("fail to set dscp to dscp map table entry "); 
	}
	for(;m<MAX_POLICY_MAP_NUM;m++) 
	{
		g_policy_map[m] = NULL;
	}
	
	return ret;
}

unsigned int npd_qos_mode_init(unsigned int qosmode)
{
	unsigned int	i = 0, k = 0, j = 0, m = 0, l = 0;
	unsigned int	ret = QOS_RETURN_CODE_SUCCESS;
	QOS_PROFILE_STC  *profilePtr = NULL;
	
	if((qosmode == QOS_MODE_PORT)&&(qos_mode != QOS_MODE_PORT) \
		||(qosmode == QOS_MODE_HYBRID)&&(qos_mode != QOS_MODE_HYBRID) \
		||(qosmode == QOS_MODE_FLOW)&&(qos_mode != QOS_MODE_FLOW)) 
	{
		
		if(qos_mode != QOS_MODE_FLOW) 
		{
			ret = npd_qos_unbind_remap_port();
			if (QOS_RETURN_CODE_SUCCESS != ret) 
			{
				npd_syslog_dbg("npd_qos_unbind_remap_port() fail!!");
				return QOS_RETURN_CODE_ERROR;
			}
		}
		for(i=1; i < MAX_PROFILE_NUMBER; i++) 
		{
			if (NULL != g_qos_profile[i]) 
			{
				free( g_qos_profile[i]);
			}
			g_qos_profile[i] = NULL;
			if(i < (MAX_DSCP_PROFILE_NUM + MAX_UP_PROFILE_NUM)) 
			{				
				g_tmp_qos[i]=(QOS_PROFILE_STC*)malloc(sizeof(QOS_PROFILE_STC));
				if(NULL==g_tmp_qos[i]) 
				{
					npd_syslog_dbg("memory alloc error for qos init!!");
					return QOS_RETURN_CODE_ERROR;
				}
				else
				{
					/*code optimize: Wrong sizeof argument
					zhangcl@autelan.com 2013-1-17*/
					memset(g_tmp_qos[i],0,sizeof(QOS_PROFILE_STC));
					if(i > MAX_DSCP_PROFILE_NUM) 
					{
						g_tmp_qos[i]->userPriority = i - (MAX_DSCP_PROFILE_NUM + 1);
						g_tmp_qos[i]->trafficClass = 0;
						g_tmp_qos[i]->dscp = 0;
					}
					else if( i < (MAX_DSCP_PROFILE_NUM+1)) 
					{
						g_tmp_qos[i]->trafficClass = 0;
						g_tmp_qos[i]->userPriority = 0;
						g_tmp_qos[i]->dscp =i - 1;
					}
				}
				profilePtr = g_tmp_qos[i];			
				ret=nam_qos_profile_entry_set(i,profilePtr);
		             if(ret!=QOS_RETURN_CODE_SUCCESS) 
					 {
		             	npd_syslog_dbg("qos profile entry set fail"); 
		             }
			}	
		}
		for(j=0; j<MAX_UP_PROFILE_NUM; j++) 
		{
			g_up_profile[j] = NULL;
			ret = nam_qos_up_to_profile_entry(j, (j+MAX_DSCP_PROFILE_NUM+1),0,ACL_ANY_PORT_CHAR);
			if(ret != QOS_RETURN_CODE_SUCCESS) 
			{
				npd_syslog_dbg("fail to set up to qos profile map table entry "); 
			}
		}
		for(k=0; k<MAX_DSCP_PROFILE_NUM; k++) 
		{
			g_dscp_profile[k] = NULL;
			ret = nam_qos_dscp_to_profile_entry(k, (k+1),0,ACL_ANY_PORT_CHAR);
			if(ret != QOS_RETURN_CODE_SUCCESS) 
			{
				npd_syslog_dbg("fail to set dscp to profile map table entry ");	
			}
		}
		for(l =0;l<MAX_DSCP_DSCP_NUM;l++) 
		{
			g_dscp_dscp[l] = NULL;
		}
		for(m=0;m<MAX_POLICY_MAP_NUM;m++) 
		{
			g_policy_map[m] = NULL;
		}
		if (qos_mode != QOS_MODE_PORT) 
		{
			ret = npd_acl_flow_to_port();
			if(ret != QOS_RETURN_CODE_SUCCESS) 
			{
				npd_syslog_err("npd_acl_flow_to_port failed !");
			}
		}	
	}
	else 
	{
		return QOS_RETURN_CODE_ERROR;
	}
	
	return ret;
}

unsigned int npd_qos_port_bind_check(unsigned int eth_g_index,unsigned int *ID)
{
	struct eth_port_s				*portInfo = NULL;
	QOS_PORT_POLICY_MAP_INFO_STC	*eth_policy=NULL;
	unsigned int					 ret = QOS_RETURN_CODE_ERROR;

	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) {
		ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
      }
	else{
		 eth_policy=portInfo->funcs.func_data[ETH_PORT_FUNC_QoS_PROFILE];
		 if(eth_policy!=NULL){
			npd_syslog_dbg("port has bind another policy map");
			ret = QOS_RETURN_CODE_POLICY_MAP_BIND;
			*ID=eth_policy->policyMapId;			
		 }
		 else{
		 	ret = QOS_RETURN_CODE_SUCCESS;
			/*npd_syslog_dbg("port has no policy map infos");*/
		 }
	}
	
	return ret;
}
unsigned int npd_qos_profile_index_check(unsigned int profileIndex){

	if ((NULL != g_qos_profile) && (NULL != g_qos_profile[profileIndex])) {
		return QOS_RETURN_CODE_SUCCESS;
	}
	else {
		return QOS_RETURN_CODE_PROFILE_NOT_EXISTED;
	}
}

unsigned int npd_qos_port_bind_opt(unsigned int eth_g_index,unsigned int policyMapIndex)
{
	struct eth_port_s				   *portInfo = NULL;
	QOS_PORT_POLICY_MAP_INFO_STC	   *eth_policy=NULL;
	unsigned int					   ret = QOS_RETURN_CODE_ERROR;
	QOS_PORT_POLICY_MAP_ATTRIBUTE_STC  *poMap=NULL;
	QOS_POLICY_MAP_ETH_INDEX_STC		*poEthId =NULL;
	enum product_id_e productId = PRODUCT_ID; 
	int isBoard = NPD_FALSE;/* 1 -if main board port, 0 - slave board port*/ 
	
	if(((PRODUCT_ID_AX7K_I == productId) || (PRODUCT_ID_AX7K == productId))
		&& (eth_g_index < 4)) {
		isBoard = NPD_TRUE;
		return NPD_FALSE;
	}
	else if((PRODUCT_ID_AX5K_I == productId) && (eth_g_index >= 8) && (eth_g_index <= 11)) {
		isBoard = NPD_TRUE;
		return NPD_FALSE;
	}

	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) {
		ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
      }
	else{ 
		 eth_policy=portInfo->funcs.func_data[ETH_PORT_FUNC_QoS_PROFILE];
		 if ((NULL != eth_policy) ||( NULL == g_policy_map)){
			npd_syslog_dbg("port has bind another policy map");
			ret = QOS_RETURN_CODE_POLICY_MAP_BIND;
		 }
		 else{
			poMap = g_policy_map[policyMapIndex];
			if(poMap==NULL){
				ret = QOS_RETURN_CODE_POLICY_NOT_EXISTED;
			}
			else{
				eth_policy =(QOS_PORT_POLICY_MAP_INFO_STC*)malloc(sizeof(QOS_PORT_POLICY_MAP_INFO_STC));
				if(eth_policy==NULL){
					npd_syslog_dbg("malloc failure");
				}
				else{
					memset(eth_policy,0,sizeof(QOS_PORT_POLICY_MAP_INFO_STC));
					/*port maintain policy map index*/
					eth_policy->policyMapId=policyMapIndex;
					npd_syslog_dbg("eth_policy->policyMapId %d",eth_policy->policyMapId);
					/*this step must notice*/
					portInfo->funcs.func_data[ETH_PORT_FUNC_QoS_PROFILE] = eth_policy;
					npd_syslog_dbg(" *portInfo->qos %p\n",portInfo->funcs.func_data[ETH_PORT_FUNC_QoS_PROFILE]);
					/*policy map maintain eth port index*/					
					poEthId = (QOS_POLICY_MAP_ETH_INDEX_STC*)malloc(sizeof(QOS_POLICY_MAP_ETH_INDEX_STC));
					if(poEthId==NULL){
						npd_syslog_dbg("malloc failure");
					}
					else{
						memset(poEthId,0,sizeof(QOS_POLICY_MAP_ETH_INDEX_STC));

						/*maintain port index with policymap,policymap eth_count+1*/
						poEthId->ethIndex = eth_g_index;
						list_add_tail(&(poEthId->list),&(poMap->list));
						poMap->eth_count+=1;
						npd_syslog_dbg("poMap %p poEth %p eth_count %d\n",poMap,poEthId,poMap->eth_count);
						/*g_policy_map[policyMapIndex]=poMap;*/
						ret = QOS_RETURN_CODE_SUCCESS;
					}					
				}
			}						
		 }		 	
	}
	
	return ret;
}

unsigned int npd_qos_policy_bind_check(unsigned int policyIndex)
{
	QOS_PORT_POLICY_MAP_ATTRIBUTE_STC	*policy=NULL;
	
	policy = g_policy_map[policyIndex];
	if (policy->eth_count==0) {
		return QOS_RETURN_CODE_SUCCESS;
	}
	else {
		return QOS_RETURN_CODE_POLICY_MAP_BIND;
	}
}

unsigned int npd_qos_policer_enable_check(QOS_POLICER_STC *node)
{
	unsigned int ret;	
       ret= node->policerEnable;
	npd_syslog_dbg("policer enable %d,",node->policerEnable);
	
  	return ret;
}
#if 0
unsigned int npd_qos_profile_in_policymap_check(unsigned int profileIndex)
{
	unsigned int i=0,num=0,ret=QOS_RETURN_CODE_SUCCESS;

	for(i=0;i<MAX_POLICY_MAP_NUM;i++){
		if(NULL!=g_policy_map[i]){
			if(profileIndex==(g_policy_map[i]->defaultProfileIndex)
				ret= QOS_RETURN_CODE_ERROR;
		}		
	}

	return ret;
}
#endif

unsigned int npd_qos_profile_relate_check(unsigned int profileIndex)
{
	unsigned int ret=QOS_RETURN_CODE_SUCCESS;
	unsigned int i=0,j=0;
	
	/*note:need remap check	*/
	for(i = 0; i < MAX_UP_PROFILE_NUM; i++) {
		if(NULL != g_up_profile[i]) {
			if(profileIndex==g_up_profile[i]->profileIndex) {
				npd_syslog_dbg("QOS profile %d in up profile use\n",profileIndex);
				return QOS_RETURN_CODE_ERROR;
			}
		}
	}
	for(j = 0; j < MAX_DSCP_PROFILE_NUM; j++) {
		if(NULL != g_dscp_profile[j]) {
			if(profileIndex==g_dscp_profile[j]->profileIndex) {
				npd_syslog_dbg("QOS profile %d in dscp profile use\n",profileIndex);
				return QOS_RETURN_CODE_ERROR;
			}
		}
	}
	ret = npd_qos_profile_in_acl_check(profileIndex);
	if(ret!=QOS_RETURN_CODE_SUCCESS){
		npd_syslog_dbg("QOS profile %d in acl use\n",profileIndex);
		return QOS_RETURN_CODE_ERROR;
	}

	return ret;
}

unsigned int npd_qos_clear_eth_policymap_if_hwfail(unsigned int eth_g_index,unsigned int policyMapIndex)
{
	struct eth_port_s				   *portInfo = NULL;
	QOS_PORT_POLICY_MAP_INFO_STC	   *eth_policy=NULL;
	unsigned int					   ret = QOS_RETURN_CODE_ERROR;
	QOS_PORT_POLICY_MAP_ATTRIBUTE_STC  *poMap=NULL;
	QOS_POLICY_MAP_ETH_INDEX_STC		*poEthId =NULL;
	struct list_head					*ptr=NULL;

	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) {
		ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
	}
	else{ 
		 eth_policy=portInfo->funcs.func_data[ETH_PORT_FUNC_QoS_PROFILE];
		 if(eth_policy!=NULL){
			npd_syslog_dbg("port has bind another policy map");
			if(eth_policy->policyMapId==policyMapIndex){
				free(eth_policy);
				portInfo->funcs.func_data[ETH_PORT_FUNC_QoS_PROFILE]=NULL;
				/*policy map*/
				poMap = g_policy_map[policyMapIndex];
				 if(poMap!=NULL){
					__list_for_each(ptr,&(poMap->list)) {
						poEthId=list_entry(ptr,QOS_POLICY_MAP_ETH_INDEX_STC,list);
						if(poEthId!=NULL) {
							if(poEthId->ethIndex==eth_g_index) {
								list_del(&(poEthId->list));
							}
							ret = QOS_RETURN_CODE_SUCCESS;
							npd_syslog_dbg("successfully delete ethInfo on policyMap");					
						}		
				       }			
				}
			} 			
		 }		 
	}		
	
	return ret;
}

int npd_qos_traffic_shape_check
(
	unsigned int 				g_eth_index,
	QOS_TRAFFIC_SHAPE_PORT_STC  *shapeData
)
{	
	struct eth_port_s	 		*portInfo = NULL;
	QOS_TRAFFIC_SHAPE_PORT_STC	*shapePtr=NULL;
	unsigned int				ret = QOS_RETURN_CODE_SUCCESS;

	portInfo = npd_get_port_by_index(g_eth_index);
	if(NULL == portInfo) {
		ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
		npd_syslog_dbg("NPD_DBUS_ERROR_NO_SUCH_PORT\n");
	}
	else{
		shapePtr=portInfo->funcs.func_data[ETH_PORT_FUNC_QoS_SHAPE];
		if(NULL == shapePtr) {
				ret=QOS_RETURN_CODE_ERROR;
				/*npd_syslog_dbg("no infos on port!\n");*/
		}
		else {
			memcpy(shapeData,shapePtr,sizeof(QOS_TRAFFIC_SHAPE_PORT_STC));
			ret=QOS_RETURN_CODE_SUCCESS;
			/*npd_syslog_dbg("shape infos on port!\n");*/
		}
	}
	
	return ret;
}

DBusMessage * npd_dbus_config_qos_profile(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;    
	DBusMessageIter		iter;
	DBusError			err;          
	unsigned int        profileIndex =0;
	unsigned int        ret = QOS_RETURN_CODE_SUCCESS;
	QOS_PROFILE_STC     *profilePtr =NULL;

	if(!((productinfo.capbmp) & FUNCTION_QOS_VLAUE))
	{
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_QOS_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_QOS_VLAUE))
	{
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		dbus_error_init(&err);
		if (!(dbus_message_get_args ( msg, &err,  
		DBUS_TYPE_UINT32,&profileIndex,
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
		npd_syslog_dbg("output profileIndex %d",profileIndex);


		if(NULL==g_qos_profile[profileIndex])
		{
			profilePtr = (QOS_PROFILE_STC *)malloc(sizeof(QOS_PROFILE_STC));
			if(NULL == profilePtr) 
			{	
				npd_syslog_err("memory alloc error for QOS_PROFILE_STC !!");
				ret = QOS_RETURN_CODE_ERROR;
			}
			else
			{
				/*code optimize: Dereference after null check
				zhangcl@autelan.com 2013-1-17*/
				memset(profilePtr,0,sizeof(QOS_PROFILE_STC));
				g_qos_profile[profileIndex]=profilePtr;
			}
		}
	}
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	
	return reply;	    
}

DBusMessage * npd_dbus_config_qos_mode(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;    
	DBusMessageIter		iter;
	DBusError			err;          
	unsigned int        qosmode =0;
	unsigned int        ret = QOS_RETURN_CODE_SUCCESS;

	if(!((productinfo.capbmp) & FUNCTION_QOS_VLAUE)){
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_QOS_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_QOS_VLAUE)){
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else{
		dbus_error_init(&err);
		if (!(dbus_message_get_args ( msg, &err,  
		DBUS_TYPE_UINT32,&qosmode,
		DBUS_TYPE_INVALID))) {
			npd_syslog_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				npd_syslog_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}   
		npd_syslog_dbg("output profileIndex %d",qosmode);

		if (qosmode != qos_mode) {
			if (QOS_MODE_DEFAULT == qosmode) {
				ret = npd_qos_unbind_remap_port();
				if (ret != QOS_RETURN_CODE_SUCCESS) {
					npd_syslog_err("npd qos mode init failed !");
				}
				ret = npd_qos_mode_def();
				if (ret != QOS_RETURN_CODE_SUCCESS) {
					npd_syslog_err("npd_qos_mode_def failed !");
				}
				ret = npd_acl_flow_to_port();
				if(ret != QOS_RETURN_CODE_SUCCESS) {
					npd_syslog_err("npd_acl_flow_to_port failed !");
				}			
			}
			else {
				ret = npd_qos_mode_init(qosmode);
				if (ret != QOS_RETURN_CODE_SUCCESS) {
					npd_syslog_err("npd qos mode init failed !");
				}	
			}
			qos_mode = qosmode;
		}
		else {
			if( QOS_MODE_PORT == qos_mode) {
				ret = ACL_RETURN_CODE_ALREADY_PORT;
			}
			else if( QOS_MODE_FLOW == qos_mode) {
				ret = ACL_RETURN_CODE_ALREADY_FLOW;
			}
			else if( QOS_MODE_HYBRID == qos_mode) {
				ret = ACL_RETURN_CODE_ALREADY_HYBRID;	
			}
		}	  
	}

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	
	return reply;	    
}

DBusMessage * npd_dbus_show_qos_mode(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;    
	DBusMessageIter		iter;
	DBusError			err;          
	unsigned int        qosmode =0;
	
 	dbus_error_init(&err);
	qosmode = qos_mode;
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&qosmode);
	
	return reply;   
}

DBusMessage * npd_dbus_qos_profile_attributes(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*	reply;    
	DBusMessageIter	iter;
	DBusError		err;      

	QOS_PROFILE_STC   *profilePtr =NULL;
	unsigned int       profileIndex =0;
	unsigned int       dp=0,up=0,tc=0,dscp=0;
	unsigned int       ret = QOS_RETURN_CODE_ERROR;

	if(!((productinfo.capbmp) & FUNCTION_QOS_VLAUE)){
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_QOS_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_QOS_VLAUE)){
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else{
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,  
	DBUS_TYPE_UINT32,&profileIndex,
	DBUS_TYPE_UINT32,&dp,
	DBUS_TYPE_UINT32,&up,
	DBUS_TYPE_UINT32,&tc,
	DBUS_TYPE_UINT32,&dscp,
	DBUS_TYPE_INVALID))) {
		npd_syslog_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			npd_syslog_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}   
	npd_syslog_dbg("output profileIndex %d ,dp %d,tc %d,up %d ,dscp %d",profileIndex,dp,tc,up,dscp);


	if(NULL!=g_qos_profile[profileIndex]){

		profilePtr = g_qos_profile[profileIndex];

		if(dp==0) {
			profilePtr->dropPrecedence =QOS_DP_GREEN_E;
		}
		else if(dp==1) {
			profilePtr->dropPrecedence =QOS_DP_RED_E;
		}

		profilePtr->userPriority =up;
		profilePtr->trafficClass =tc;
		profilePtr->dscp =dscp; 

		ret=nam_qos_profile_entry_set(profileIndex,profilePtr);
		if(ret!=QOS_RETURN_CODE_SUCCESS) {
			npd_syslog_dbg("qos profile entry set fail");
		}
	}
	else if(NULL==g_qos_profile[profileIndex]) {
		ret = QOS_RETURN_CODE_PROFILE_NOT_EXISTED;
	}
	/* npd_syslog_dbg("ret %d\n",ret);*/
	}

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	
	return reply;	    
}

DBusMessage * npd_dbus_dscp_to_qos_profile_table(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*	reply;    
	DBusMessageIter	iter;
	DBusError		err;      

	unsigned int       profileIndex =0,dscp=0;
	unsigned int       ret = QOS_RETURN_CODE_SUCCESS;
	QOS_DSCP_PROFILE_MAP_STC *dscpProfile=NULL;
	unsigned int    eth_g_index = 0;
   	unsigned int	i = 0;
	unsigned char	   devNum=0,virPort=0;
	QOS_PORT_POLICY_MAP_ATTRIBUTE_STC  *poMap=NULL;
	QOS_POLICY_MAP_ETH_INDEX_STC		*ruleInfo =NULL;
	struct list_head					*Ptr=NULL;

	if(!((productinfo.capbmp) & FUNCTION_QOS_VLAUE))
	{
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_QOS_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_QOS_VLAUE))
	{
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		dbus_error_init(&err);
		if (!(dbus_message_get_args ( msg, &err,  
			DBUS_TYPE_UINT32,&dscp,
			DBUS_TYPE_UINT32,&profileIndex,
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
		npd_syslog_dbg("output profileIndex %d,dscp %d",profileIndex,dscp);

		if((qos_mode == QOS_MODE_HYBRID)&&(profileIndex > MAX_HYBRID_DSCP))
		{
			ret = ACL_RETURN_CODE_HYBRID_DSCP;
		}
		else 
		{
			if (NULL!=g_qos_profile[profileIndex]) 
			{
				if(NULL==g_dscp_profile[dscp]) 
				{
					dscpProfile = (QOS_DSCP_PROFILE_MAP_STC *)malloc(sizeof(QOS_DSCP_PROFILE_MAP_STC));
					if(NULL == dscpProfile)
					{
						npd_syslog_err("memory alloc error for QOS_DSCP_PROFILE_MAP_STC !!");
						ret = QOS_RETURN_CODE_ERROR;	
					}
					else
					{
						/*code optimize: Dereference after null check
						zhangcl@autelan.com 2013-1-17*/
						memset(dscpProfile,0,sizeof(QOS_DSCP_PROFILE_MAP_STC));		
						dscpProfile->profileIndex = profileIndex;
						g_dscp_profile[dscp]=dscpProfile;
					}
				}
				else 
				{
					dscpProfile=g_dscp_profile[dscp];
					dscpProfile->profileIndex = profileIndex;
				}

				ret = bcm_nam_qos_support_check();
				if (ACL_RETURN_CODE_BCM_DEVICE_NOT_SUPPORT == ret) 
				{											
					for (i = 0; i < MAX_POLICY_MAP_NUM; i++) 
					{
						poMap = g_policy_map[i];
						if(NULL != poMap) 
						{
							__list_for_each(Ptr,&(poMap->list)) 
							{
								npd_syslog_dbg(" del dscp %d to prifile\n", dscp);
								ruleInfo = list_entry(Ptr,QOS_POLICY_MAP_ETH_INDEX_STC,list);
								/*npd_syslog_dbg("ruleInfo node %p\n",ruleInfo);*/
								if (NULL != ruleInfo) 
								{		
									eth_g_index = ruleInfo->ethIndex;
									ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPort);
									if (ret != QOS_RETURN_CODE_SUCCESS) 
									{
										/*ret = NPD_DBUS_ERROR_NO_SUCH_PORT;*/
									}
								}
							}
							ret = nam_qos_dscp_to_profile_entry(dscp, profileIndex, devNum, virPort);
							if (QOS_RETURN_CODE_SUCCESS != ret) 
							{
								npd_syslog_dbg("nam_acl_port_acl_enable fail ! \n");
							}												
						}
						else 
						{
							ret = QOS_RETURN_CODE_SUCCESS;
						}
					}
				}
				else 
				{
					npd_syslog_dbg("dscp is %d, profileindex is %d", dscp, profileIndex);
					ret = nam_qos_dscp_to_profile_entry(dscp, profileIndex, devNum, virPort);
					if (QOS_RETURN_CODE_SUCCESS != ret) 
					{
						npd_syslog_dbg("nam_acl_port_acl_enable fail ! \n");
					}				
				}
			}
			else
			{
				npd_syslog_dbg("qos profile not existed");
				ret = QOS_RETURN_CODE_PROFILE_NOT_EXISTED;
			}
		}	

	}

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	
	return reply;	    
}

DBusMessage * npd_dbus_dscp_to_dscp_table(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*	reply;    
	DBusMessageIter	iter;
	DBusError		err;      

	unsigned int       oldDscp =0,newDscp=0;
	unsigned int       ret = QOS_RETURN_CODE_ERROR;
	QOS_DSCP_DSCP_REMAP_STC *dscpDscp=NULL;

	if(!((productinfo.capbmp) & FUNCTION_QOS_VLAUE))
	{
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_QOS_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_QOS_VLAUE))
	{
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		dbus_error_init(&err);
		if (!(dbus_message_get_args ( msg, &err,  
			DBUS_TYPE_UINT32,&oldDscp,
			DBUS_TYPE_UINT32,&newDscp,
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
		npd_syslog_dbg("output old %d,new %d",oldDscp,newDscp);

		if(NULL==g_dscp_dscp[oldDscp])
		{
			dscpDscp = (QOS_DSCP_DSCP_REMAP_STC *)malloc(sizeof(QOS_DSCP_DSCP_REMAP_STC));
			if(NULL == dscpDscp) 
			{
				npd_syslog_err("memory alloc error for QOS_DSCP_DSCP_REMAP_STC !!");
				ret = QOS_RETURN_CODE_ERROR;	
			}
			else
			{
				/*code optimize: Dereference after null check
				zhangcl@autelan.com 2013-1-17*/
				memset(dscpDscp,0,sizeof(QOS_DSCP_DSCP_REMAP_STC)); 	
				dscpDscp->NewDscp = newDscp;
				g_dscp_dscp[oldDscp]=dscpDscp;
			}
		}
		else 
		{
			dscpDscp=g_dscp_dscp[oldDscp];
			dscpDscp->NewDscp = newDscp;
		}

		ret = nam_qos_dscp_to_dscp_entry(oldDscp,newDscp);
		if(ACL_RETURN_CODE_BCM_DEVICE_NOT_SUPPORT == ret) 
		{
			npd_syslog_dbg("ACL_RETURN_CODE_BCM_DEVICE_NOT_SUPPORT \n ");
			free(g_dscp_dscp[oldDscp]);
			g_dscp_dscp[oldDscp]=NULL;
		}
		else if (QOS_RETURN_CODE_SUCCESS != ret) 
		{
			npd_syslog_dbg("nam_qos_dscp_to_dscp_entry \n ");
		}
	}
	
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	
	return reply;	    
}

DBusMessage *npd_dbus_up_to_qos_profile(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*	reply;    
	DBusMessageIter	iter;
	DBusError		err;      
	   
	unsigned int    up = 0, profileIndex = 0;
	unsigned int    ret = QOS_RETURN_CODE_SUCCESS;
	QOS_UP_PROFILE_MAP_STC *upProfile = NULL;
	unsigned int    eth_g_index = 0;
   	unsigned int	i = 0;
	unsigned char	   devNum=0,virPort=0;
	QOS_PORT_POLICY_MAP_ATTRIBUTE_STC  *poMap = NULL;
	QOS_POLICY_MAP_ETH_INDEX_STC		*ruleInfo = NULL;
	struct list_head					*Ptr = NULL;
	
	if(!((productinfo.capbmp) & FUNCTION_QOS_VLAUE))
	{
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_QOS_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_QOS_VLAUE))
	{
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		dbus_error_init(&err);
		if (!(dbus_message_get_args ( msg, &err,  
											DBUS_TYPE_UINT32,&up,
											DBUS_TYPE_UINT32,&profileIndex,
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
		npd_syslog_dbg("output up %d,profileIndex %d",up,profileIndex);

		if((qos_mode == QOS_MODE_HYBRID)&&((profileIndex > (MAX_HYBRID_UP + 1))||(profileIndex <= MAX_HYBRID_DSCP))) 
		{
			ret = ACL_RETURN_CODE_HYBRID_UP;
		}	
		else 
		{
			if(NULL != g_qos_profile[profileIndex]) 
			{
				if(NULL == g_up_profile[up]) 
				{
					upProfile = (QOS_UP_PROFILE_MAP_STC *)malloc(sizeof(QOS_UP_PROFILE_MAP_STC));
					if(NULL == upProfile) 
					{	
						npd_syslog_err("memory alloc error for QOS_UP_PROFILE_MAP_STC !!");
						ret = QOS_RETURN_CODE_ERROR;
					}
					else
					{
						/*code optimize: Dereference after null check
						zhangcl@autelan.com 2013-1-17*/
						memset(upProfile,0,sizeof(QOS_UP_PROFILE_MAP_STC)); 	
						upProfile->profileIndex = profileIndex;
						g_up_profile[up] = upProfile;
					}
				}
				else 
				{
					upProfile = g_up_profile[up];
					upProfile->profileIndex = profileIndex;
				}
				ret = bcm_nam_qos_support_check();
				if (ACL_RETURN_CODE_BCM_DEVICE_NOT_SUPPORT == ret) 
				{
					for (i = 0; i < MAX_POLICY_MAP_NUM; i++) 
					{
						poMap = g_policy_map[i];
						if(NULL != poMap) 
						{
							__list_for_each(Ptr,&(poMap->list))
							{
								npd_syslog_dbg("del up %d to profile\n", up);
								ruleInfo = list_entry(Ptr,QOS_POLICY_MAP_ETH_INDEX_STC,list);
								/*npd_syslog_dbg("ruleInfo node %p\n",ruleInfo);*/
								if(NULL!=ruleInfo)
								{ 		
								     eth_g_index = ruleInfo->ethIndex;
									ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPort);
									if(ret != QOS_RETURN_CODE_SUCCESS) 
									{
										/*ret = NPD_DBUS_ERROR_NO_SUCH_PORT;*/
									}
								}
							}
							ret = nam_qos_up_to_profile_entry(up, profileIndex, devNum, virPort);
							if (QOS_RETURN_CODE_SUCCESS != ret) 
							{
								npd_syslog_dbg("nam_acl_port_acl_enable fail ! \n");
							}											
						} 
						else 
						{
							ret = QOS_RETURN_CODE_SUCCESS;
						}
						/*
						if(ACL_RETURN_CODE_BCM_DEVICE_NOT_SUPPORT == ret) {
							npd_syslog_dbg("BCM_DEICE_NOT_SUPPORT \n "); 
						}
						else if (QOS_RETURN_CODE_SUCCESS != ret) {
							npd_syslog_dbg("ERROR ! nam_qos_up_to_profile_entry\n");				
							ret = nam_qos_up_to_profile_entry(up, profileIndex, devNum, virPort);
							if (QOS_RETURN_CODE_SUCCESS != ret) {
								npd_syslog_dbg("nam_acl_port_acl_enable fail ! \n");
							}											
						}
						*/
					}
					
				}
				else 
				{
					npd_syslog_dbg("up is %d, profileindex is %d", up, profileIndex);
					ret = nam_qos_up_to_profile_entry(up, profileIndex, devNum, virPort);
					if (QOS_RETURN_CODE_SUCCESS != ret) 
					{
						npd_syslog_dbg("nam_acl_port_acl_enable fail ! \n");
					}
				}
			}
			else
			{
				npd_syslog_dbg("no qos profile existed");
				ret = QOS_RETURN_CODE_PROFILE_NOT_EXISTED;

			}
		}
	}
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	
	return reply;	    
}

DBusMessage * npd_dbus_create_policy_map(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*	reply;	  
	DBusMessageIter iter;
	DBusError		err;
	unsigned int    policyIndex =0;
	unsigned int    ret = QOS_RETURN_CODE_ERROR;
	QOS_PORT_POLICY_MAP_ATTRIBUTE_STC *policyPtr=NULL;

	if(!((productinfo.capbmp) & FUNCTION_QOS_VLAUE))
	{
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_QOS_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_QOS_VLAUE))
	{
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		dbus_error_init(&err);
	   
		if (!(dbus_message_get_args ( msg, &err,  
		   DBUS_TYPE_UINT32,&policyIndex,
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

		if(NULL != g_policy_map[policyIndex]) 
		{
			ret = QOS_RETURN_CODE_POLICY_EXISTED;
			npd_syslog_err("existed this policy map\n");
		}
		else 
		{
			policyPtr = (QOS_PORT_POLICY_MAP_ATTRIBUTE_STC *)malloc(sizeof(QOS_PORT_POLICY_MAP_ATTRIBUTE_STC));
			if(NULL == policyPtr) 
			{
				npd_syslog_err("memory alloc error for QOS_PORT_POLICY_MAP_ATTRIBUTE_STC !!");
				ret = QOS_RETURN_CODE_ERROR; 	
			}
			else
			{
				memset(policyPtr,0,sizeof(QOS_PORT_POLICY_MAP_ATTRIBUTE_STC));
				policyPtr->modifyDscp = DISABLE_E;
				policyPtr->modifyUp   = DISABLE_E;
				INIT_LIST_HEAD(&(policyPtr->list));/*add port index*/
				g_policy_map[policyIndex] = policyPtr;
				ret = QOS_RETURN_CODE_SUCCESS;
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

DBusMessage * npd_dbus_config_policy_map(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*	  reply;	
	DBusMessageIter	  iter;
	DBusError		  err;	  
	unsigned int      policyIndex=0;
	unsigned int	  ret =QOS_RETURN_CODE_ERROR;

	dbus_error_init(&err);	   
	if (!(dbus_message_get_args ( msg, &err,  
	   DBUS_TYPE_UINT32,&policyIndex,
	   DBUS_TYPE_INVALID))) {
	   npd_syslog_err("Unable to get input args ");
	   if (dbus_error_is_set(&err)) {
		   npd_syslog_err("%s raised: %s",err.name,err.message);
		   dbus_error_free(&err);
	   }
	   return NULL;
	}
	if(NULL == g_policy_map[policyIndex]) {
		ret = QOS_RETURN_CODE_POLICY_NOT_EXISTED;
	}
	else if(0 != (g_policy_map[policyIndex]->eth_count)) {
		ret = QOS_RETURN_CODE_POLICY_MAP_BIND;
	}
	else if((NULL != g_policy_map[policyIndex])&&(0 == g_policy_map[policyIndex]->eth_count)) {
		ret = QOS_RETURN_CODE_POLICY_EXISTED;
	}
	reply = dbus_message_new_method_return(msg);	   
	dbus_message_iter_init_append (reply, &iter);	   
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,
									&ret);	
	return reply;
}
DBusMessage * npd_dbus_policy_modify_qos(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*	   reply;    
	DBusMessageIter	   iter;
	DBusError		   err;          
	unsigned int       IsEnable =0,policyMapIndex=0;
	unsigned int       ret = QOS_RETURN_CODE_ERROR;
	QOS_PORT_POLICY_MAP_ATTRIBUTE_STC *policyPtr=NULL;
	
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,  
		DBUS_TYPE_UINT32,&IsEnable,
		DBUS_TYPE_UINT32,&policyMapIndex,
		DBUS_TYPE_INVALID))) {
		npd_syslog_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			npd_syslog_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}   
	npd_syslog_dbg("output IsEnable %d,policyMapIndex %d",IsEnable,policyMapIndex);

	policyPtr = g_policy_map[policyMapIndex];
	if(NULL != policyPtr) {
		policyPtr->assignPrecedence = (QOS_ATTRIBUTE_ASSIGN_PRECEDENCE_E)IsEnable;
		ret = QOS_RETURN_CODE_SUCCESS;
	}

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	
	return reply;	    
}

#if 0
DBusMessage * npd_dbus_policy_default_up(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
    DBusMessage*	   reply;    
	DBusMessageIter	   iter= {0};
	DBusError		   err;          
    unsigned int       up =0,policyMapIndex=0;
    unsigned int       ret = QOS_RETURN_CODE_ERROR;
    QOS_PORT_POLICY_MAP_ATTRIBUTE_STC *policyPtr=NULL;
	
    dbus_error_init(&err);
    if (!(dbus_message_get_args ( msg, &err,  
	   DBUS_TYPE_UINT32,&up,
	   DBUS_TYPE_UINT32,&policyMapIndex,
	   DBUS_TYPE_INVALID))) {
	   npd_syslog_err("Unable to get input args ");
	   if (dbus_error_is_set(&err)) {
		   npd_syslog_err("%s raised: %s",err.name,err.message);
		   dbus_error_free(&err);
	   }
	   return NULL;
    }   
    npd_syslog_dbg("output up %d,policyMapIndex %d",up,policyMapIndex);

	policyPtr = g_policy_map[policyMapIndex];
	if(policyPtr!=NULL){
		policyPtr->defaultUp = up;
		ret =QOS_RETURN_CODE_SUCCESS;
	}
   
   reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	return reply;	    
}
DBusMessage * npd_dbus_policy_default_qos_profile(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*	   reply;	 
	DBusMessageIter    iter= {0};
	DBusError		   err; 		 
	unsigned int	   ProIndex =0,policyMapIndex=0;
	unsigned int	   ret = QOS_RETURN_CODE_ERROR;
	QOS_PORT_POLICY_MAP_ATTRIBUTE_STC *policyPtr=NULL;
	
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,  
	   DBUS_TYPE_UINT32,&ProIndex,
	   DBUS_TYPE_UINT32,&policyMapIndex,
	   DBUS_TYPE_INVALID))) {
	   npd_syslog_err("Unable to get input args ");
	   if (dbus_error_is_set(&err)) {
		   npd_syslog_err("%s raised: %s",err.name,err.message);
		   dbus_error_free(&err);
	   }
	   return NULL;
	}	
	npd_syslog_dbg("output ProIndex %d,policyMapIndex %d",ProIndex,policyMapIndex);
	  
	policyPtr = g_policy_map[policyMapIndex];
	if(policyPtr!=NULL){
		if(NULL!=g_qos_profile[ProIndex]){
			policyPtr->defaultProfileIndex = ProIndex;
			ret =QOS_RETURN_CODE_SUCCESS;
		}
		else{
			npd_syslog_dbg("default qos peofile has not configured!");
			ret =QOS_RETURN_CODE_PROFILE_NOT_EXISTED;
		}
	}
	
   reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	return reply;		
}
#endif

DBusMessage * npd_dbus_policy_trust_mode_l2_set(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*	   reply;	 
	DBusMessageIter    iter;
	DBusError		   err; 		 
	unsigned int	   upEnable =1,policyMapIndex=0;
	unsigned int	   ret = QOS_RETURN_CODE_ERROR;
	QOS_PORT_POLICY_MAP_ATTRIBUTE_STC *policyPtr=NULL;
	
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,  
	   DBUS_TYPE_UINT32,&upEnable,
	   DBUS_TYPE_UINT32,&policyMapIndex,
	   DBUS_TYPE_INVALID))) {
	   npd_syslog_err("Unable to get input args ");
	   if (dbus_error_is_set(&err)) {
		   npd_syslog_err("%s raised: %s",err.name,err.message);
		   dbus_error_free(&err);
	   }
	   return NULL;
	}	
	npd_syslog_dbg("output upEnable %d,policyMapIndex %d",upEnable,policyMapIndex);

	policyPtr = g_policy_map[policyMapIndex];
	if(NULL != policyPtr) {
		policyPtr->trustFlag = PORT_L2_TRUST_E;
		policyPtr->modifyUp  = (QOS_NORMAL_ENABLE_E)upEnable;
		policyPtr->modifyDscp = DISABLE_E;
		policyPtr->remapDscp = 0;
		ret = QOS_RETURN_CODE_SUCCESS;
	}
	/*ret = bcm_nam_qos_support_check();*/

      reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	
	return reply;		
}

DBusMessage * npd_dbus_policy_trust_mode_l3_set(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*	   reply;	 
	DBusMessageIter    iter;
	DBusError		   err; 		 
	unsigned int	   dscpEnable =1,dscpRemap=1,policyMapIndex=0;
	unsigned int	   ret = QOS_RETURN_CODE_ERROR;
	QOS_PORT_POLICY_MAP_ATTRIBUTE_STC *policyPtr=NULL;
	
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,  
	   DBUS_TYPE_UINT32,&dscpEnable,
	   DBUS_TYPE_UINT32,&dscpRemap,
	   DBUS_TYPE_UINT32,&policyMapIndex,
	   DBUS_TYPE_INVALID))) {
	   npd_syslog_err("Unable to get input args ");
	   if (dbus_error_is_set(&err)) {
		   npd_syslog_err("%s raised: %s",err.name,err.message);
		   dbus_error_free(&err);
	   }
	   return NULL;
	}	
	npd_syslog_dbg("output dscpEnable %d,dscpRemap %d,policyMapIndex %d",dscpEnable,dscpRemap,policyMapIndex);

	policyPtr = g_policy_map[policyMapIndex];
	if(NULL != policyPtr) {
		ret = bcm_nam_qos_support_check();
		if ((QOS_SUCCESS == ret) || (0 == dscpRemap)) {	
			policyPtr->trustFlag = PORT_L3_TRUST_E;
			policyPtr->modifyDscp= (QOS_NORMAL_ENABLE_E)dscpEnable;
			policyPtr->remapDscp = dscpRemap;
			policyPtr->modifyUp =DISABLE_E;
			ret = QOS_RETURN_CODE_SUCCESS;
		}
	}
       reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	
	return reply;		
}

DBusMessage * npd_dbus_policy_trust_mode_l2_l3_set(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*	   reply;	 
	DBusMessageIter    iter;
	DBusError		   err; 		 
	unsigned int	   upEnable=1,dscpEnable =1,dscpRemap=1,policyMapIndex=0;
	unsigned int	   ret = QOS_RETURN_CODE_ERROR;
	QOS_PORT_POLICY_MAP_ATTRIBUTE_STC *policyPtr=NULL;
	
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,  
	DBUS_TYPE_UINT32,&upEnable,
	DBUS_TYPE_UINT32,&dscpEnable,
	DBUS_TYPE_UINT32,&dscpRemap,
	DBUS_TYPE_UINT32,&policyMapIndex,
	DBUS_TYPE_INVALID))) {
		npd_syslog_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			npd_syslog_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}	
	npd_syslog_dbg("output upEnable %d,dscpEnable %d,dscpRemap %d,policyMapIndex %d",upEnable,dscpEnable,dscpRemap,policyMapIndex);

	ret = bcm_nam_qos_support_check();
	if (QOS_RETURN_CODE_SUCCESS == ret) {
		policyPtr = g_policy_map[policyMapIndex];
		if(NULL != policyPtr) {
			policyPtr->trustFlag = PORT_L2_L3_TRUST_E;
			policyPtr->modifyUp = (QOS_NORMAL_ENABLE_E)upEnable;
			policyPtr->modifyDscp = (QOS_NORMAL_ENABLE_E)dscpEnable;
			policyPtr->remapDscp =  dscpRemap;
			ret = QOS_RETURN_CODE_SUCCESS;
		}
	}
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	
	return reply;		
}

#if 0
DBusMessage * npd_dbus_policy_trust_mode_untrust_set(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*	   reply;	 
	DBusMessageIter    iter= {0};
	DBusError		   err; 		 
	unsigned int	   policyMapIndex=0;
	unsigned int	   ret = QOS_RETURN_CODE_ERROR;
	QOS_PORT_POLICY_MAP_ATTRIBUTE_STC *policyPtr=NULL;
	
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,  
	   DBUS_TYPE_UINT32,&policyMapIndex,
	   DBUS_TYPE_INVALID))) {
	   npd_syslog_err("Unable to get input args ");
	   if (dbus_error_is_set(&err)) {
		   npd_syslog_err("%s raised: %s",err.name,err.message);
		   dbus_error_free(&err);
	   }
	   return NULL;
	}	
	npd_syslog_dbg("output policyMapIndex %d",policyMapIndex);
	
	policyPtr = g_policy_map[policyMapIndex];
	if(policyPtr!=NULL){
		policyPtr->trustFlag = PORT_UNTRUST_E;	
		policyPtr->modifyUp= DISABLE_E;
		policyPtr->modifyDscp= DISABLE_E;
		policyPtr->remapDscp = 0;
		ret = QOS_RETURN_CODE_SUCCESS;
	}
    reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	return reply;		
}
#endif

DBusMessage * npd_dbus_delete_qos_profile(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*   reply;	 
	DBusMessageIter iter;
	DBusError	   err; 	 

	unsigned int 	  profileIndex =0;
	unsigned int 	  ret = QOS_RETURN_CODE_ERROR;

	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,  
	DBUS_TYPE_UINT32,&profileIndex,
	DBUS_TYPE_INVALID))) {
		npd_syslog_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			npd_syslog_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}   
	npd_syslog_dbg("output profileIndex %d",profileIndex);

	ret =npd_qos_profile_index_check(profileIndex);
	if(QOS_RETURN_CODE_SUCCESS == ret) {   
		ret= npd_qos_profile_relate_check(profileIndex);
		if(QOS_RETURN_CODE_SUCCESS != ret) {
			ret = QOS_RETURN_CODE_PROFILE_IN_USE;
			npd_syslog_dbg("in use!\n");
		}
		else	 {		/*exists,not in use*/
			free(g_qos_profile[profileIndex]);
			g_qos_profile[profileIndex] = NULL;
			ret = QOS_RETURN_CODE_SUCCESS; 
		}
	}    

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	
	return reply;	
}

DBusMessage * npd_dbus_delete_dscp_to_qos_profile_table(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*	reply;    
	DBusMessageIter	iter;
	DBusError		err;      

	unsigned int       dscp=0;
	unsigned int    eth_g_index = 0;
   	unsigned int	i = 0;
	unsigned char	   devNum=0,virPort=0;
	unsigned int	   ret = QOS_RETURN_CODE_SUCCESS;
	QOS_PORT_POLICY_MAP_ATTRIBUTE_STC  *poMap=NULL;
	QOS_POLICY_MAP_ETH_INDEX_STC		*ruleInfo =NULL;
	struct list_head					*Ptr=NULL;
	/*unsigned int 						count=0;*/

	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,  
	DBUS_TYPE_UINT32,&dscp,	  
	DBUS_TYPE_INVALID))) {
		npd_syslog_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			npd_syslog_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}  

	if(NULL != g_dscp_profile[dscp]){
		free(g_dscp_profile[dscp]);
		g_dscp_profile[dscp]=NULL;
		for (i = 0; i < MAX_POLICY_MAP_NUM; i++) {
			poMap = g_policy_map[i];
			if(NULL != poMap) {
				__list_for_each(Ptr,&(poMap->list)) {
					npd_syslog_dbg(" del dscp %d to prifile\n", dscp);
					ruleInfo = list_entry(Ptr,QOS_POLICY_MAP_ETH_INDEX_STC,list);
					/*npd_syslog_dbg("ruleInfo node %p\n",ruleInfo);*/
					if (NULL != ruleInfo) {		
						eth_g_index = ruleInfo->ethIndex;
						ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPort);
						if (ret != QOS_RETURN_CODE_SUCCESS) {
							/*ret = NPD_DBUS_ERROR_NO_SUCH_PORT;*/
						}
						ret = nam_qos_dscp_to_profile_entry(dscp, 0, devNum, virPort);
						if (QOS_RETURN_CODE_SUCCESS != ret) {
							npd_syslog_dbg("nam_acl_port_acl_enable fail ! \n");
						}												
					}
				} /*__list_for_each*/
			}
		}
	}
	else {
		ret =QOS_RETURN_CODE_NO_MAPPED;
	}

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);

	return reply;	    
}

DBusMessage * npd_dbus_delete_dscp_to_dscp_table(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
    DBusMessage*	reply;    
	DBusMessageIter	iter;
	DBusError		err;      
    
    unsigned int       oldDscp =0;
    unsigned int       ret = QOS_RETURN_CODE_SUCCESS;
   
    dbus_error_init(&err);
    if (!(dbus_message_get_args ( msg, &err,  
       DBUS_TYPE_UINT32,&oldDscp,
	   DBUS_TYPE_INVALID))) {
	   npd_syslog_err("Unable to get input args ");
	   if (dbus_error_is_set(&err)) {
		   npd_syslog_err("%s raised: %s",err.name,err.message);
		   dbus_error_free(&err);
	   }
	   return NULL;
    }   

	if(NULL != g_dscp_dscp[oldDscp]) {		
		free(g_dscp_dscp[oldDscp]);
		g_dscp_dscp[oldDscp] = NULL;
		#if 0
		ret = nam_qos_dscp_to_dscp_entry(oldDscp,0);
		#else
		/* for bug when delete dscp-to-dscp remap, make dscp remap to default */
		/* zhangdi@autelan.com 2011-10-28 */
		ret = nam_qos_dscp_to_dscp_entry(oldDscp,oldDscp);		
		#endif
		if(QOS_RETURN_CODE_SUCCESS != ret) {
			npd_syslog_dbg("fail to delete dscp to dscp map table entry ");
		}
	}
	else {
		ret = QOS_RETURN_CODE_NO_MAPPED;
	} 
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	
	return reply;	    
}

DBusMessage *npd_dbus_delete_up_to_qos_profile(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*	reply;    
	DBusMessageIter	iter;
	DBusError		err;      
       
	unsigned int    up = 0;
	unsigned int       ret = QOS_RETURN_CODE_SUCCESS;
	unsigned int    eth_g_index = 0;
   	unsigned int	i = 0;
	unsigned char	   devNum=0,virPort=0;
	QOS_PORT_POLICY_MAP_ATTRIBUTE_STC  *poMap=NULL;
	QOS_POLICY_MAP_ETH_INDEX_STC		*ruleInfo =NULL;
	struct list_head					*Ptr=NULL;

	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,  
	   DBUS_TYPE_UINT32,&up,
	   DBUS_TYPE_INVALID))) {
	   npd_syslog_err("Unable to get input args ");
	   if (dbus_error_is_set(&err)) {
		   npd_syslog_err("%s raised: %s",err.name,err.message);
		   dbus_error_free(&err);
	   }
	   return NULL;
	}   

	if(NULL!=g_up_profile[up]){
		free(g_up_profile[up]);
		g_up_profile[up]=NULL;
		for (i = 0; i < MAX_POLICY_MAP_NUM; i++) {
			poMap = g_policy_map[i];
			if(NULL != poMap) {
				__list_for_each(Ptr,&(poMap->list)){
					npd_syslog_dbg("del up %d to profile\n", up);
					ruleInfo = list_entry(Ptr,QOS_POLICY_MAP_ETH_INDEX_STC,list);
					/*npd_syslog_dbg("ruleInfo node %p\n",ruleInfo);*/
					if(NULL!=ruleInfo){ 		
					      eth_g_index = ruleInfo->ethIndex;
						ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPort);
						if(QOS_RETURN_CODE_SUCCESS != ret) {
							/*ret = NPD_DBUS_ERROR_NO_SUCH_PORT;*/
						}
						ret = nam_qos_up_to_profile_entry(up, 0, devNum, virPort);
						if (QOS_RETURN_CODE_SUCCESS != ret) {
							npd_syslog_dbg("nam_acl_port_acl_enable fail ! \n");
						}											
					}
				} /*__list_for_each*/
			}
		}
	}
	else {
		ret =QOS_RETURN_CODE_NO_MAPPED;
	}

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	
	return reply;	    
}

DBusMessage * npd_dbus_delete_policy_map(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*	reply;	  
	DBusMessageIter iter;
	DBusError		err;
	unsigned int    policyIndex =0;
	unsigned int    ret = QOS_RETURN_CODE_ERROR;
	
	dbus_error_init(&err);
   
	if (!(dbus_message_get_args ( msg, &err,  
	   DBUS_TYPE_UINT32,&policyIndex,
	   DBUS_TYPE_INVALID))) {
	   npd_syslog_err("Unable to get input args ");
	   if (dbus_error_is_set(&err)) {
		   npd_syslog_err("%s raised: %s",err.name,err.message);
		   dbus_error_free(&err);
	   }
	   return NULL;
	}

	if(NULL == g_policy_map[policyIndex]) {
		ret = QOS_RETURN_CODE_POLICY_NOT_EXISTED;
		npd_syslog_err("not existed this policy map\n");
	}
	else {
		ret = npd_qos_policy_bind_check(policyIndex);
		if(QOS_RETURN_CODE_POLICY_MAP_BIND == ret) {
			npd_syslog_err("since bind to some port ,can not delete");
		}
		else if(QOS_RETURN_CODE_SUCCESS == ret) {
			/*free directly*/
			free(g_policy_map[policyIndex]);
			g_policy_map[policyIndex] = NULL;
			ret = QOS_RETURN_CODE_SUCCESS;
		}
	}		 
   reply = dbus_message_new_method_return(msg);
   
   dbus_message_iter_init_append (reply, &iter);
   
   dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,
									&ret);

   return reply;
}

DBusMessage * npd_dbus_ethport_unbind_policy_map(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*	   reply;	 
	DBusMessageIter    iter;
	DBusError		   err; 	

	unsigned int	   g_eth_index=0,policyIndex=0;
	unsigned char	   devNum=0,virPort=0;
	unsigned int	   ret=QOS_RETURN_CODE_ERROR,portInfo=NPD_DBUS_ERROR_NO_SUCH_PORT;
	unsigned int		hwRet=QOS_RETURN_CODE_ERROR;
	unsigned int 		bindId=0;
	
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,  
	   DBUS_TYPE_UINT32,&g_eth_index,
	   DBUS_TYPE_UINT32,&policyIndex,
	   DBUS_TYPE_INVALID))) {
	   npd_syslog_err("Unable to get input args ");
	   if (dbus_error_is_set(&err)) {
		   npd_syslog_err("%s raised: %s",err.name,err.message);
		   dbus_error_free(&err);
	   }
	   return NULL;
	}	
	npd_syslog_dbg("output g_eth_index %d,policyIndex %d",g_eth_index,policyIndex);
	portInfo = npd_get_devport_by_global_index(g_eth_index,&devNum,&virPort);
	if(portInfo!=QOS_RETURN_CODE_SUCCESS)
		ret= NPD_DBUS_ERROR_NO_SUCH_PORT;
	else{
		/*check if bind policymap*/
		ret=npd_qos_port_bind_check(g_eth_index,&bindId);
		if(ret == NPD_DBUS_ERROR_NO_SUCH_PORT)
			npd_syslog_dbg("wrong eth_g_index");
		else if(ret==QOS_RETURN_CODE_POLICY_MAP_BIND){
			npd_syslog_dbg("policy map %d on port",bindId);
			if(bindId!=policyIndex){
				ret = QOS_RETURN_CODE_POLICY_MAP_PORT_WRONG;
				npd_syslog_dbg("wrong policy index");
			}
			else{
				/*HW set relation registers */
				hwRet=nam_qos_port_unbind_policy_map(devNum,virPort,policyIndex);
				if(hwRet!=QOS_RETURN_CODE_SUCCESS){
					npd_syslog_dbg("hw clear register fail");						
				}
				else{
					npd_syslog_dbg("success clear hw data!\n");
					ret = npd_qos_port_unbind_opt(g_eth_index,policyIndex);
					if(ret!=QOS_RETURN_CODE_SUCCESS){
						npd_syslog_dbg("sw fail!\n");
						ret = QOS_RETURN_CODE_ERROR;
					}
					else
						npd_syslog_dbg("success clear sw data!\n");
					
				}
							
			}				
		}			
		else if(ret==QOS_RETURN_CODE_SUCCESS){
			ret = QOS_RETURN_CODE_POLICY_NOT_EXISTED; /*no policy on port*/
			npd_syslog_dbg("no policy map on port!");
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&hwRet);
	
	return reply;
}

DBusMessage * npd_dbus_ethport_bind_policy_map(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*	   reply;	 
	DBusMessageIter    iter;
	DBusError		   err; 	

	unsigned int	   g_eth_index=0,policyIndex=0;
	unsigned char	   devNum=0,virPort=0;
	unsigned int	   ret=QOS_RETURN_CODE_ERROR,portInfo=NPD_DBUS_ERROR_NO_SUCH_PORT;
	unsigned int		hwRet=QOS_RETURN_CODE_ERROR;
	unsigned int        bindId=0, rem = 0, groupNo = 0, devret = 0;
	enum product_id_e productId = PRODUCT_ID; 
	int isBoard = NPD_FALSE;/* 1 -if main board port, 0 - slave board port*/ 

	if(!((productinfo.capbmp) & FUNCTION_QOS_VLAUE)){
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_QOS_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_QOS_VLAUE)){
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else{
		dbus_error_init(&err);
		if (!(dbus_message_get_args ( msg, &err,  
		   DBUS_TYPE_UINT32,&g_eth_index,
		   DBUS_TYPE_UINT32,&policyIndex,
		   DBUS_TYPE_INVALID))) {
		   npd_syslog_err("Unable to get input args ");
		   if (dbus_error_is_set(&err)) {
			   npd_syslog_err("%s raised: %s",err.name,err.message);
			   dbus_error_free(&err);
		   }
		   return NULL;
		}	
		rem = npd_port_bind_group_check(rem, &groupNo, ACL_DIRECTION_INGRESS);
		 if(ACL_RETURN_CODE_GROUP_PORT_BINDED == rem) {
			  npd_syslog_dbg("this interface has binded acl group %d\n",groupNo);
		}
		devret = bcm_nam_qos_support_check();
		
		npd_syslog_dbg("output g_eth_index %d,policyIndex %d",g_eth_index,policyIndex);
		portInfo = npd_get_devport_by_global_index(g_eth_index,&devNum,&virPort);
		if(portInfo!=QOS_RETURN_CODE_SUCCESS) {
			if(((PRODUCT_ID_AX7K_I == productId) || (PRODUCT_ID_AX7K == productId))
				&& (g_eth_index < 4)) {
				isBoard = NPD_TRUE;
				ret = ACL_PORT_NOT_SUPPORT_BINDED;
			}
			else if((PRODUCT_ID_AX5K_I == productId) && ((g_eth_index >= 8) && (g_eth_index <= 11))) {
				isBoard = NPD_TRUE;
				ret = ACL_PORT_NOT_SUPPORT_BINDED;
			}
			else {
				ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
			}
		}
		else if (qos_mode == QOS_MODE_FLOW) {
			ret = ACL_RETURN_CODE_ALREADY_FLOW;
		}
		else if (qos_mode == QOS_MODE_DEFAULT) {
			ret = ACL_RETURN_CODE_NO_QOS_MODE;	
		}
		else {
			if ((ACL_RETURN_CODE_GROUP_PORT_BINDED != rem) || (QOS_SUCCESS == devret)){
				/*check if bind policymap*/
				ret=npd_qos_port_bind_check(g_eth_index,&bindId);
				if(ret==NPD_DBUS_ERROR_NO_SUCH_PORT) {
					npd_syslog_dbg("wrong eth_g_index");
				}
				else if(ret==QOS_RETURN_CODE_POLICY_MAP_BIND) {
					npd_syslog_dbg("port has bind another policy map %d",bindId);
				}
				else if(ret==QOS_RETURN_CODE_SUCCESS) {
					ret = npd_qos_port_bind_opt(g_eth_index,policyIndex);
					npd_syslog_dbg("ret %d\n",ret);
					if(ret==QOS_RETURN_CODE_POLICY_NOT_EXISTED) {
						npd_syslog_dbg(" policy map should config firstly");
					}
					else if(ret==QOS_RETURN_CODE_SUCCESS){
						/*HW set relation registers */
						npd_syslog_dbg("hw write\n");
						npd_syslog_dbg("nam_qos_port_bind_policy_map devNum %d, virPort %d, policyIndex %d \n",
							devNum, virPort, policyIndex);
						hwRet=nam_qos_port_bind_policy_map(devNum,virPort,policyIndex);
						if(hwRet!=QOS_RETURN_CODE_SUCCESS){
							npd_syslog_dbg("hw set register fail");
							/*SW clear*/
							ret=npd_qos_clear_eth_policymap_if_hwfail(g_eth_index,policyIndex);
							if(ret!=QOS_RETURN_CODE_SUCCESS) {
								npd_syslog_dbg("sw clear fail");
							}
						}
					}
					else {
						ret = QOS_RETURN_CODE_ERROR;
						/* nothing */
					}
				}
			}
			else {
				ret = ACL_RETURN_CODE_BCM_DEVICE_NOT_SUPPORT;
			}
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&hwRet);
	
	return reply;
}

DBusMessage * npd_dbus_show_policy_map(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*			 reply = NULL;
	DBusMessageIter 		 iter, iter_array;		 
	DBusError				 err;	
	unsigned int			 null_num=0,i=1,j=0,count=0,m=0,k=0,ret =QOS_RETURN_CODE_SUCCESS;
	unsigned char			 devNum=0,portNum=0;
	static POLICY_MAP_SHOW_STC	 	 *showProfile[MAX_POLICY_MAP_NUM];
	QOS_PORT_POLICY_MAP_ATTRIBUTE_STC 		 *profile=NULL;
	GROUP_ETH_INDEX_STC	     *pmethindex[MAX_PORT_NUMBER];
	QOS_POLICY_MAP_ETH_INDEX_STC 		 *pmethinfo=NULL;
	struct list_head		 *ptr=NULL;
	unsigned int			 slot_no=0,local_port_no=0,pmcount=0,pmort=0;

	dbus_error_init(&err);
	for(i=0;i<MAX_POLICY_MAP_NUM;i++)
	{
		if(g_policy_map[i]!=NULL)
		{
			profile = g_policy_map[i];
			showProfile[count]=(POLICY_MAP_SHOW_STC*)malloc(sizeof(POLICY_MAP_SHOW_STC));
			if(NULL==showProfile[count]) 
				return NULL;
			memset(showProfile[count],0,sizeof(POLICY_MAP_SHOW_STC));
			showProfile[count]->policyIndex =i;
			/*showProfile[count]->defaultUp=profile->defaultUp;*/
			/*showProfile[count]->defaultProfileIndex= profile->defaultProfileIndex;*/
			showProfile[count]->assignPrecedence= profile->assignPrecedence;
			showProfile[count]->trustFlag= profile->trustFlag;
			showProfile[count]->modifyDscp= profile->modifyDscp;
			showProfile[count]->modifyUp=profile->modifyUp;
			showProfile[count]->remapDscp=profile->remapDscp;
			showProfile[count]->bindport_num=profile->eth_count;
			__list_for_each(ptr,&(profile->list)) 
			{
				pmethinfo = list_entry(ptr,QOS_POLICY_MAP_ETH_INDEX_STC,list);
				if(NULL!=pmethinfo)
				{
					pmethindex[m] = (GROUP_ETH_INDEX_STC*)malloc(sizeof(GROUP_ETH_INDEX_STC));
					memset(pmethindex[m],0,sizeof(GROUP_ETH_INDEX_STC));
					pmethindex[m]->ethindex = 1 + (pmethinfo->ethIndex);
					/*code optimize: Unchecked return value
					zhangcl@autelan.com 2013-1-17*/
					ret = npd_get_devport_by_global_index(pmethinfo->ethIndex,&devNum,&portNum);
					if(ret != QOS_RETURN_CODE_SUCCESS)
					{
						free(pmethindex[m]);
						continue;
					}
					ret = nam_get_slotport_by_devport(devNum, portNum, PRODUCT_ID, &(slot_no), &(local_port_no));
					if(ret != QOS_RETURN_CODE_SUCCESS)
					{
						free(pmethindex[m]);
						continue;
					}
					npd_syslog_dbg("bind by port %d/%d",slot_no,local_port_no);
					npd_syslog_dbg("pmethindex[%d]:%d",m,pmethindex[m]->ethindex);
					pmethindex[m]->slot = slot_no;
					pmethindex[m]->port = local_port_no;
					m++;
				}
				else
				{
					syslog_ax_acl_dbg("no policy-map is bound to any port");
				}
			}
			count++;
		}
		else 
		{
			null_num++;
		}
	}
	if(null_num==MAX_POLICY_MAP_NUM)
	{
		ret = QOS_RETURN_CODE_POLICY_NOT_EXISTED;
		npd_syslog_dbg("policy map not existed");
	}
	npd_syslog_dbg("ret %d",ret);
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret); 
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &count);
	dbus_message_iter_open_container (&iter,
								   DBUS_TYPE_ARRAY,
									DBUS_STRUCT_BEGIN_CHAR_AS_STRING	 /*begin*/
									   DBUS_TYPE_UINT32_AS_STRING	 /*policyIndex*/
									   DBUS_TYPE_UINT32_AS_STRING	 /*assignPrecedence*/
									   DBUS_TYPE_UINT32_AS_STRING	 /*trustFlag*/
									   DBUS_TYPE_UINT32_AS_STRING    /*modifyDscp*/
									   DBUS_TYPE_UINT32_AS_STRING    /*modifyUp*/
									   DBUS_TYPE_UINT32_AS_STRING    /*remapDscp*/
									   DBUS_TYPE_UINT32_AS_STRING    /*bind port count*/
								     DBUS_TYPE_ARRAY_AS_STRING
									  DBUS_STRUCT_BEGIN_CHAR_AS_STRING
									   DBUS_TYPE_UINT32_AS_STRING    /*slot_no*/
									   DBUS_TYPE_UINT32_AS_STRING    /*port_no*/
									  DBUS_STRUCT_END_CHAR_AS_STRING
									DBUS_STRUCT_END_CHAR_AS_STRING, 	/*end*/
									&iter_array);
	for (j = 0; j < count; j++ ) 
	{
		DBusMessageIter iter_struct;
		DBusMessageIter iter_sub_array;
		dbus_message_iter_open_container (&iter_array,
										   DBUS_TYPE_STRUCT,
										   NULL,
										   &iter_struct);
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(showProfile[j]->policyIndex));
		
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(showProfile[j]->assignPrecedence));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(showProfile[j]->trustFlag)); 
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(showProfile[j]->modifyUp)); 
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(showProfile[j]->modifyDscp));	
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(showProfile[j]->remapDscp)); 
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(showProfile[j]->bindport_num));

		pmcount = showProfile[j]->bindport_num;
		syslog_ax_acl_dbg("pmethindex[%d].pmcount=%d",j,pmcount);
		dbus_message_iter_open_container (&iter_struct,
							   DBUS_TYPE_ARRAY,
							   DBUS_STRUCT_BEGIN_CHAR_AS_STRING  
							   DBUS_TYPE_UINT32_AS_STRING	 /*slot_no*/
							   DBUS_TYPE_UINT32_AS_STRING	/*port_no*/
							   DBUS_STRUCT_END_CHAR_AS_STRING, 
							   &iter_sub_array);
		for(k=0;k<pmcount;k++)
		{
			DBusMessageIter iter_sub_struct;
			dbus_message_iter_open_container (&iter_sub_array,
													DBUS_TYPE_STRUCT,
													NULL,
												  &iter_sub_struct);
			dbus_message_iter_append_basic 
						(&iter_sub_struct,
						 DBUS_TYPE_UINT32,
						 &(pmethindex[k+pmort]->slot));
			dbus_message_iter_append_basic 
						(&iter_sub_struct,
						 DBUS_TYPE_UINT32,
						 &(pmethindex[k+pmort]->port));
			syslog_ax_acl_dbg("slot num :%d\n",pmethindex[k+pmort]->slot);
			syslog_ax_acl_dbg("port num :%d\n",pmethindex[k+pmort]->port);
			dbus_message_iter_close_container (&iter_sub_array, &iter_sub_struct);
				free(pmethindex[k+pmort]);
				pmethindex[k+pmort] = NULL;
		}
		dbus_message_iter_close_container (&iter_struct, &iter_sub_array);
		pmort += pmcount;
		dbus_message_iter_close_container (&iter_array, &iter_struct);
		free(showProfile[j]);
		showProfile[j]=NULL;
	}
	dbus_message_iter_close_container (&iter, &iter_array);

	return reply;	
}

DBusMessage * npd_dbus_show_qos_profile(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*			 reply = NULL;
	DBusMessageIter 		 iter, iter_array;		 
	DBusError				 err;	
	unsigned int 			 null_num=0,i=0,j=0,count=0,ret =QOS_RETURN_CODE_SUCCESS;
	QOS_PROFILE_SHOW_STC     *showProfile[MAX_PROFILE_NUMBER];
	QOS_PROFILE_STC			 *profile=NULL;

	dbus_error_init(&err);

	for(i=0;i<MAX_PROFILE_NUMBER;i++){
		if(g_qos_profile[i]!=NULL){
			profile = g_qos_profile[i];
			showProfile[count]=(QOS_PROFILE_SHOW_STC*)malloc(sizeof(QOS_PROFILE_SHOW_STC));
			if(showProfile[count]==NULL) return NULL;
			memset(showProfile[count],0,sizeof(QOS_PROFILE_SHOW_STC));
			showProfile[count]->profileIndex = i;
			showProfile[count]->DP = profile->dropPrecedence;
			showProfile[count]->TC = profile->trafficClass;
			showProfile[count]->DSCP= profile->dscp;
			showProfile[count]->UP= profile->userPriority;
			count++;
		}
		else {
			null_num++;
		}
	}
	if(null_num==MAX_PROFILE_NUMBER)
		ret = QOS_RETURN_CODE_PROFILE_NOT_EXISTED;
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &count);
	dbus_message_iter_open_container (&iter,
								   DBUS_TYPE_ARRAY,
								    DBUS_STRUCT_BEGIN_CHAR_AS_STRING     /*begin*/
								       DBUS_TYPE_UINT32_AS_STRING    /*profileIndex*/
									   DBUS_TYPE_UINT32_AS_STRING    /*TC*/
                                       DBUS_TYPE_UINT32_AS_STRING    /*DP*/
									   DBUS_TYPE_UINT32_AS_STRING    /*UP*/
									   DBUS_TYPE_UINT32_AS_STRING    /*dscp*/
									DBUS_STRUCT_END_CHAR_AS_STRING,     /*end*/
								    &iter_array);
	for (j = 0; j < count; j++ ) {
		DBusMessageIter iter_struct;
		dbus_message_iter_open_container (&iter_array,
										   DBUS_TYPE_STRUCT,
										   NULL,
										   &iter_struct);
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(showProfile[j]->profileIndex));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(showProfile[j]->TC));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(showProfile[j]->DP));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(showProfile[j]->UP));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(showProfile[j]->DSCP));			
		dbus_message_iter_close_container (&iter_array, &iter_struct);
		free(showProfile[j]);
		showProfile[j]=NULL;
	}
	dbus_message_iter_close_container (&iter, &iter_array);

	return reply;	
}

DBusMessage * npd_dbus_ethport_show_policy_map(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*			reply = NULL;
	DBusMessageIter 		iter;
	DBusError				err;
	unsigned int    		slot_no=0,local_port_no=0,g_index=0;
	QOS_PORT_POLICY_MAP_INFO_STC	   *eth_policy=NULL;
	unsigned int 			poliyIndex=0,ret=QOS_RETURN_CODE_ERROR;
	struct eth_port_s				*portInfo = NULL;
	unsigned char			devNum=0,portNum=0;
	
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
			DBUS_TYPE_UINT32,&g_index,
			DBUS_TYPE_INVALID))) {
			npd_syslog_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				npd_syslog_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}		

	npd_get_devport_by_global_index(g_index,&devNum,&portNum);
	nam_get_slotport_by_devport(devNum,portNum,PRODUCT_ID,&(slot_no),&(local_port_no));
	npd_syslog_dbg("slot/port %d/%d",slot_no,local_port_no);

	portInfo = npd_get_port_by_index(g_index);
	if(NULL == portInfo) {
		ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
	}
	else {
		if(NULL==(g_eth_ports[g_index]->funcs.func_data[ETH_PORT_FUNC_QoS_PROFILE])){
			npd_syslog_dbg("interface no policy map infomation!\n");
			ret=QOS_RETURN_CODE_POLICY_NOT_EXISTED;
		}
		else {
			ret =QOS_RETURN_CODE_POLICY_MAP_BIND;
			eth_policy=g_eth_ports[g_index]->funcs.func_data[ETH_PORT_FUNC_QoS_PROFILE];
			npd_syslog_dbg("eth_policy %p\n",eth_policy);
			poliyIndex=eth_policy->policyMapId;
			npd_syslog_dbg("policyIndex %d\n",poliyIndex);
		}
	}
   	
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&slot_no);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&local_port_no);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&poliyIndex);
	
	return reply;	
}

DBusMessage * npd_dbus_show_remap_table(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*			 reply = NULL;
	DBusMessageIter 		 iter,iter_array;		 
	DBusError				 err;	
	unsigned int 			 i=0,j=0,k=0,m=0;
	unsigned int 			 upCount=0,dscpCount=0,dscpReCount=0,countVal=0;
	QOS_REMAP_BACK_VALUE_SHOW	*rebackPtr[150];
	
	dbus_error_init(&err);

	for(i=0;i<MAX_UP_PROFILE_NUM;i++){
		if(g_up_profile[i]!=NULL){
			rebackPtr[countVal] = (QOS_REMAP_BACK_VALUE_SHOW*)malloc(sizeof(QOS_REMAP_BACK_VALUE_SHOW));
			if(rebackPtr[countVal]==NULL) return NULL;
			memset(rebackPtr[countVal],0,sizeof(QOS_REMAP_BACK_VALUE_SHOW));
			rebackPtr[countVal]->specflag = i;
			rebackPtr[countVal]->profileIndex= g_up_profile[i]->profileIndex;
			countVal ++;
			/*npd_syslog_dbg("**upCount %d",upCount);*/
		}
	}
	upCount =countVal;
	npd_syslog_dbg("upCount %d",upCount);
	for(j=0;j<MAX_DSCP_PROFILE_NUM;j++){
		if(g_dscp_profile[j]!=NULL){			
			rebackPtr[countVal]=(QOS_REMAP_BACK_VALUE_SHOW*)malloc(sizeof(QOS_REMAP_BACK_VALUE_SHOW));
			if(rebackPtr[countVal]==NULL) return NULL;
			memset(rebackPtr[countVal],0,sizeof(QOS_REMAP_BACK_VALUE_SHOW));
			rebackPtr[countVal]->specflag =j;
			rebackPtr[countVal]->profileIndex = g_dscp_profile[j]->profileIndex;
			countVal ++;
			/*npd_syslog_dbg("**dscpCount %d",dscpCount);*/
		}
	}
	dscpCount=countVal-upCount;
	npd_syslog_dbg("dscpCount %d",dscpCount);
	for(k=0;k<MAX_DSCP_PROFILE_NUM;k++){
		if(g_dscp_dscp[k]!=NULL){	
			rebackPtr[countVal]=(QOS_REMAP_BACK_VALUE_SHOW*)malloc(sizeof(QOS_REMAP_BACK_VALUE_SHOW));
			if(rebackPtr[countVal]==NULL) return NULL;
			memset(rebackPtr[countVal],0,sizeof(QOS_REMAP_BACK_VALUE_SHOW));
			rebackPtr[countVal]->specflag =k;
			rebackPtr[countVal]->profileIndex = g_dscp_dscp[k]->NewDscp;
			countVal ++;
			/*npd_syslog_dbg("**dscpReCount %d",dscpReCount);*/
		}
	}
	
	dscpReCount= countVal - upCount-dscpCount;
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &upCount);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &dscpCount);
								 
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,
									&dscpReCount);
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,
									&countVal);
								
	dbus_message_iter_open_container (&iter,
								   DBUS_TYPE_ARRAY,
								    DBUS_STRUCT_BEGIN_CHAR_AS_STRING     /*begin*/
								       DBUS_TYPE_UINT32_AS_STRING    /*specflag*/
								       DBUS_TYPE_UINT32_AS_STRING    /*profileIndex		*/			 
									DBUS_STRUCT_END_CHAR_AS_STRING,     /*end*/
								    &iter_array);
	for (m = 0; m < countVal; m++ ) {
		DBusMessageIter iter_struct;
		dbus_message_iter_open_container (&iter_array,
										   DBUS_TYPE_STRUCT,
										   NULL,
										   &iter_struct);
		dbus_message_iter_append_basic
				  (&iter_struct,
				    DBUS_TYPE_UINT32,
				    &(rebackPtr[m]->specflag));
		dbus_message_iter_append_basic
				  (&iter_struct,
				    DBUS_TYPE_UINT32,
				    &(rebackPtr[m]->profileIndex));
		npd_syslog_dbg("***reback  %d ---->> %d\n",rebackPtr[m]->specflag,rebackPtr[m]->profileIndex);
		dbus_message_iter_close_container (&iter_array, &iter_struct);
		free(rebackPtr[m]);
		rebackPtr[m]=NULL;
	}	
	dbus_message_iter_close_container (&iter, &iter_array);

	return reply;	
}

DBusMessage * npd_dbus_policer_set(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter;
	DBusError			err;
	unsigned int    	policerIndex =0;
       unsigned int    	ret = QOS_RETURN_CODE_ERROR;
	QOS_POLICER_STC 	*policerPtr=NULL;

	if(!((productinfo.capbmp) & FUNCTION_QOS_VLAUE))
	{
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_QOS_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_QOS_VLAUE))
	{
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		dbus_error_init(&err);
	   
		if (!(dbus_message_get_args ( msg, &err,  
		   DBUS_TYPE_UINT32,&policerIndex,
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
		npd_syslog_dbg("policer index %d",policerIndex);
	/*	if(g_policer[policerIndex]!=NULL){
			
			npd_syslog_dbg("existed this policer\n");	
			api disable
			ret = npd_qos_policer_enable_check(g_policer[policerIndex]);
			if(ret==QOS_POLICER_ENABLE) success enable
				npd_syslog_dbg("you shoule disable policer before config policer");
			else if(ret==QOS_POLICER_DISABLE) 
				npd_syslog_dbg("you can config policer");
		}*/
		/*else {*/
		if(g_policer[policerIndex]==NULL)
		{
			policerPtr = (QOS_POLICER_STC *)malloc(sizeof(QOS_POLICER_STC));
			if(NULL == policerPtr) 
			{	
				npd_syslog_err("memory alloc error for QOS_POLICER_STC !!");
				ret = QOS_RETURN_CODE_ERROR; 	
			}
			else
			{
				memset(policerPtr,0,sizeof(QOS_POLICER_STC));

				policerPtr->policerEnable=QOS_POLICER_ENABLE;
				g_policer[policerIndex]=policerPtr;
				ret = QOS_RETURN_CODE_SUCCESS;

				ret = nam_qos_policer_init();
				if(ret != QOS_RETURN_CODE_SUCCESS) 
				{
					npd_syslog_err("policer init error!");
				}
			}
		}
		else 
		{
			g_policer[policerIndex]->policerEnable = QOS_POLICER_ENABLE;
			ret = QOS_RETURN_CODE_SUCCESS;
		}
	}
   reply = dbus_message_new_method_return(msg);
   dbus_message_iter_init_append (reply, &iter);   
   dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,
									&ret);

   return reply;

}

DBusMessage * npd_dbus_policer_set_range(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter;
	DBusError			err;
	unsigned int policerIndex= 0, startPid = 0, endPid = 0, indexcheck = 0;
	unsigned int 	ret = QOS_RETURN_CODE_ERROR, i = 0, j = 0, aliasindex = 0, range = 0, enrange = 0;
	unsigned long		cir=0,cbs=0;
	QOS_POLICER_STC 	*policerPtr=NULL;

	if(!((productinfo.capbmp) & FUNCTION_QOS_VLAUE)){
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_QOS_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_QOS_VLAUE)){
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else{
		dbus_error_init(&err);
	   
		if (!(dbus_message_get_args ( msg, &err,  
									DBUS_TYPE_UINT32, &policerIndex,
									DBUS_TYPE_UINT32, &startPid,
									DBUS_TYPE_UINT32, &endPid,
									DBUS_TYPE_INVALID))) {
		   npd_syslog_err("Unable to get input args ");
		   if (dbus_error_is_set(&err)) {
			   npd_syslog_err("%s raised: %s",err.name,err.message);
			   dbus_error_free(&err);
		   }
		   return NULL;
		}
		npd_syslog_dbg("policer index %d", policerIndex);
		npd_syslog_dbg("policerIndex is %d, startpid is %d, endpid is %d !\n", policerIndex, startPid, endPid);
		npd_syslog_dbg("check if policerIndex is in anther policer-range, this is not permit !!!\n");
		for (i = 0; i < MAX_POLICER_NUM; i++) {
			if (NULL != alias_policer[i]) {
				if ((policerIndex >= (alias_policer[i]->startpolicer)) 
					&& (policerIndex <= (alias_policer[i]->endpolicer))) {
					indexcheck = 1;
					break;
				}
			}
		}
		for (i = 0; i < MAX_POLICER_NUM; i++) {
			if (NULL != alias_policer[i]) {
				range++;
				if (((startPid < alias_policer[i]->startpolicer) && (endPid <  alias_policer[i]->endpolicer)) ||
					((startPid > alias_policer[i]->startpolicer) && (endPid >  alias_policer[i]->endpolicer))) {
					enrange++;
				}
			}
		}
		if ((NULL != g_policer[policerIndex]) 
			&& ((policerIndex > endPid) || (policerIndex < startPid))
			&& (0 == indexcheck) && (range == enrange)) {
			for (i = 0; i < MAX_POLICER_NUM; i++) {
				if (NULL == alias_policer[i]) {
					alias_policer[i] = (QOS_POLICER_ALIAS *)malloc(sizeof(QOS_POLICER_ALIAS));
					if(NULL == alias_policer[i]) {
						npd_syslog_err("memory alloc error for QOS_POLICER_ALIAS !!");
						ret = QOS_RETURN_CODE_ERROR; 	
					}
					memset(alias_policer[i], 0, sizeof(QOS_POLICER_ALIAS));
					alias_policer[i]->aliaspolicer = policerIndex;
					alias_policer[i]->startpolicer = startPid;
					alias_policer[i]->endpolicer = endPid;
					aliasindex = i;
					break;
				}
			}
			policerPtr = g_policer[policerIndex];
			for (i = 0; i < (endPid - startPid + 1); i++) {
				if(NULL == g_policer[startPid+i]) {
					g_policer[startPid+i] = (QOS_POLICER_STC *)malloc(sizeof(QOS_POLICER_STC));
					if(NULL == g_policer[startPid+i]) {
						npd_syslog_err("memory alloc error for QOS_POLICER_STC !!");
						ret = QOS_RETURN_CODE_ERROR; 	
					}
					memset(g_policer[startPid+i], 0, sizeof(QOS_POLICER_STC));

					memcpy(g_policer[startPid+i], policerPtr, sizeof(QOS_POLICER_STC));
					g_policer[startPid+i]->rangepolicer = 1;
					ret = nam_qos_policer_entry_set(startPid+i, policerPtr, &cir, &cbs);
					if(ret != QOS_RETURN_CODE_SUCCESS) {
						npd_syslog_err("policer set fail\n");
						/*if fail free the set g_policer*/
					}
					ret = QOS_RETURN_CODE_SUCCESS;
				}
				else {
					for (j = 0; j < i; j ++) {
						npd_syslog_dbg("set g_policer fail free g_policer");
						nam_qos_policer_invalid(startPid+j);
						free(g_policer[startPid+j]);
						g_policer[startPid+j] = NULL;
					}
					free(alias_policer[aliasindex]);
					alias_policer[aliasindex] = NULL;
					ret = QOS_RETURN_CODE_ERROR;
					break;
				}
			}
		}
		else {
			ret = QOS_RETURN_CODE_ERROR;
			npd_syslog_dbg("g_policer[%d] is not exist !", policerIndex);
		}
	}
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);   
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,
									&ret);
	return reply;

}

DBusMessage * npd_dbus_policer_cir_cbs(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*	reply;    
	DBusMessageIter	iter;
	DBusError		err;      

	QOS_POLICER_STC   *policerPtr =NULL;
	unsigned int       policerIndex =0;
	unsigned long      cir=0,cbs=0;
	unsigned int       ret = QOS_RETURN_CODE_ERROR;
	
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,  
	      DBUS_TYPE_UINT32,&policerIndex,
	      DBUS_TYPE_UINT32,&cir,
	      DBUS_TYPE_UINT32,&cbs,
	      DBUS_TYPE_INVALID))) {
	      npd_syslog_err("Unable to get input args ");
	      if (dbus_error_is_set(&err)) {
	         npd_syslog_err("%s raised: %s",err.name,err.message);
	         dbus_error_free(&err);
	        }
	        return NULL;
	}   
	npd_syslog_dbg("output profileIndex %d ,cir %ld,cbs %ld",policerIndex,cir,cbs);

	policerPtr=g_policer[policerIndex];	

	if(policerPtr==NULL) {
		ret = QOS_RETURN_CODE_BAD_PTR;
		npd_syslog_err("illegal policer index");
	}
	else{
		ret = nam_qos_policer_cir_cbs_check(cir,cbs);
		if(ret==QOS_RETURN_CODE_SUCCESS) {
			policerPtr->cir = cir;
			policerPtr->cbs = cbs;
			ret=nam_qos_policer_entry_set(policerIndex,policerPtr,&cir,&cbs);
			if(ret!=QOS_RETURN_CODE_SUCCESS) {
			       npd_syslog_err("policer set fail\n");
			}
			else {
				ret = QOS_RETURN_CODE_SUCCESS;
			}
		}
	}

	reply = dbus_message_new_method_return(msg);	
	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	return reply;	    
}

DBusMessage * npd_dbus_policer_set_out_profile(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter;
	DBusError			err;   	
	unsigned int	    policerIndex =0;
	unsigned int	    ret = QOS_RETURN_CODE_ERROR;
	QOS_POLICER_STC   *policerPtr =NULL;
	 
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,  
		  DBUS_TYPE_UINT32,&policerIndex,
		  DBUS_TYPE_INVALID))) {
		  npd_syslog_err("Unable to get input args ");
		  if (dbus_error_is_set(&err)) {
			 npd_syslog_err("%s raised: %s",err.name,err.message);
			 dbus_error_free(&err);
			}
			return NULL;
	}	
	npd_syslog_dbg("output policerIndex %d",policerIndex);

	policerPtr = g_policer[policerIndex];
	if(policerPtr!=NULL) {ret = QOS_RETURN_CODE_SUCCESS;}
	
	reply = dbus_message_new_method_return(msg);	
	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	return reply;
}

DBusMessage * npd_dbus_policer_out_profile_cmd_keep_drop(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*	  reply;	  
	DBusMessageIter   iter;
	DBusError		  err;   
	QOS_POLICER_STC   *policerPtr =NULL;
	unsigned int	   policerIndex =0,action = 0;
	unsigned int	   ret = QOS_RETURN_CODE_ERROR;
	unsigned long		cir=0,cbs=0;
	
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,  
		  DBUS_TYPE_UINT32,&policerIndex,
		  DBUS_TYPE_UINT32,&action,
		  DBUS_TYPE_INVALID))) {
		  npd_syslog_err("Unable to get input args ");
		  if (dbus_error_is_set(&err)) {
			 npd_syslog_err("%s raised: %s",err.name,err.message);
			 dbus_error_free(&err);
			}
			return NULL;
	}	
	npd_syslog_dbg("output profileIndex %d,action %d",policerIndex,action);

	policerPtr = g_policer[policerIndex];
	if(policerPtr==NULL) {
		ret = QOS_RETURN_CODE_BAD_PTR;
		npd_syslog_err("illegal policer index");
	}
	else{
	       policerPtr->cmd=(QOS_PROFILE_OUT_PROFILE_ACTION_E)action;
		ret=nam_qos_policer_entry_set(policerIndex,policerPtr,&cir,&cbs);
		if(ret!=QOS_RETURN_CODE_SUCCESS) {
		       npd_syslog_err("policer set fail\n");
		}
		else {
			ret = QOS_RETURN_CODE_SUCCESS;
		}
	}

	reply = dbus_message_new_method_return(msg);	
	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	return reply;
}

DBusMessage * npd_dbus_policer_out_profile_cmd_remap(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*	  reply;	  
	DBusMessageIter   iter;
	DBusError		  err;   
	QOS_POLICER_STC   *policerPtr =NULL;
	unsigned int	   policerIndex =0,profileIndex=0;
	unsigned int	   ret = QOS_RETURN_CODE_ERROR;
	unsigned long		cir=0,cbs=0;
	
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,  
		  DBUS_TYPE_UINT32,&policerIndex,
		  DBUS_TYPE_UINT32,&profileIndex,
		  DBUS_TYPE_INVALID))) {
		  npd_syslog_err("Unable to get input args ");
		  if (dbus_error_is_set(&err)) {
			 npd_syslog_err("%s raised: %s",err.name,err.message);
			 dbus_error_free(&err);
			}
			return NULL;
	}	
	npd_syslog_dbg("output policerIndex %d,profileIndex %d",policerIndex,profileIndex);

	policerPtr = g_policer[policerIndex];
	if(policerPtr==NULL) {
		ret = QOS_RETURN_CODE_BAD_PTR;
		npd_syslog_err("illegal policer index");
	}
	else {
		ret = nam_bcm_acl_support_check();
		if (QOS_RETURN_CODE_SUCCESS == ret) {
			if ((qos_mode != QOS_MODE_FLOW)&&(qos_mode != QOS_MODE_HYBRID)) {
				if (qos_mode == QOS_MODE_DEFAULT) {
					ret = ACL_RETURN_CODE_NO_QOS_MODE;
				}
				else {
					ret = ACL_RETURN_CODE_ALREADY_PORT;
				}	
			}
			else if ((qos_mode == QOS_MODE_HYBRID)&&(profileIndex < (MAX_HYBRID_UP+1))) {
				ret = ACL_RETURN_CODE_HYBRID_FLOW;
			}
			else {
				if(NULL==g_qos_profile[profileIndex]) {
					npd_syslog_err("not existed!");
					ret =QOS_RETURN_CODE_PROFILE_NOT_EXISTED;
				}
				else {
					policerPtr->cmd=OUT_PROFILE_REMAP_ENTRY;
					policerPtr->qosProfile = profileIndex;
					policerPtr->modifyDscp=ENABLE_E;
					policerPtr->modifyUp=ENABLE_E;
					ret=nam_qos_policer_entry_set(policerIndex,policerPtr,&cir,&cbs);
					if(ret!=QOS_RETURN_CODE_SUCCESS) {
					       npd_syslog_err("policer set fail\n");
					}
					else {
						ret = QOS_RETURN_CODE_SUCCESS;
					}
				}
				
			}
		}
	}
	reply = dbus_message_new_method_return(msg);	
	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	return reply;
}

DBusMessage * npd_dbus_policer_color(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter;
	DBusError			err;
	unsigned int    	policerIndex =0,color=0;
       unsigned int    	ret = QOS_RETURN_CODE_ERROR;
	QOS_POLICER_STC 	*policerPtr=NULL;
	unsigned long		cir=0,cbs=0;
	
	dbus_error_init(&err);
   
	if (!(dbus_message_get_args ( msg, &err,  
	   DBUS_TYPE_UINT32,&policerIndex,
	   DBUS_TYPE_UINT32,&color,
	   DBUS_TYPE_INVALID))) {
	   npd_syslog_err("Unable to get input args ");
	   if (dbus_error_is_set(&err)) {
		   npd_syslog_err("%s raised: %s",err.name,err.message);
		   dbus_error_free(&err);
	   }
	   return NULL;
	}
	
	npd_syslog_dbg("output profileIndex %d,color %d",policerIndex,color);

	policerPtr = g_policer[policerIndex];
	if(policerPtr==NULL) {
		ret = QOS_RETURN_CODE_BAD_PTR;
		npd_syslog_err("illegal policer index");
	}
	else{		
		policerPtr->meterColorMode = (QOS_POLICER_COLOR_MODE_ENT)color;
		ret = nam_qos_policer_entry_set(policerIndex,policerPtr,&cir,&cbs);
		if(ret!=QOS_RETURN_CODE_SUCCESS) {
		       npd_syslog_err("policer set fail\n");
		}
		else {
			ret = QOS_RETURN_CODE_SUCCESS;
		}
	}
	reply = dbus_message_new_method_return(msg);	
	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	return reply;	
}

DBusMessage * npd_dbus_policer_counter(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter;
	DBusError			err;
	unsigned int		policerIndex=0,countIndex=0,ret=QOS_RETURN_CODE_ERROR,IsEnable=QOS_POLICER_DISABLE;
	QOS_POLICER_STC     *policerPtr =NULL;
	unsigned long		cir=0,cbs=0;
	dbus_error_init(&err);

	if(!((productinfo.capbmp) & FUNCTION_QOS_VLAUE)){
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_QOS_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_QOS_VLAUE)){
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else{
		if (!(dbus_message_get_args ( msg, &err,  
		   DBUS_TYPE_UINT32,&policerIndex, 
		   DBUS_TYPE_UINT32,&countIndex,
		   DBUS_TYPE_UINT32,&IsEnable,
		   DBUS_TYPE_INVALID))) {
		   npd_syslog_err("Unable to get input args ");
		   if (dbus_error_is_set(&err)) {
			   npd_syslog_err("%s raised: %s",err.name,err.message);
			   dbus_error_free(&err);
		   }
		   return NULL;
		}
		
		npd_syslog_dbg("output policerIndex %d,countIndex %d",policerIndex,countIndex);

		policerPtr=g_policer[policerIndex];	

		if(policerPtr==NULL) {
			ret = QOS_RETURN_CODE_BAD_PTR;
			npd_syslog_err("illegal policer index");
		}
		else{
			/*api*/
			if(NULL==g_counter[countIndex]){
				npd_syslog_err("counter not existed");
				ret = QOS_RETURN_CODE_COUNTER_NOT_EXISTED;
			}
			else{
				policerPtr->counterEnable   = IsEnable;
				policerPtr->counterSetIndex = countIndex;
				ret = nam_qos_policer_entry_set(policerIndex,policerPtr,&cir,&cbs);
				if(ret!=QOS_RETURN_CODE_SUCCESS) {
				       npd_syslog_err("policer set fail\n");
				}
				else {
					ret = QOS_RETURN_CODE_SUCCESS;
				}
			}
		}
	}
	reply = dbus_message_new_method_return(msg);	
	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	return reply;

}

DBusMessage * npd_dbus_policer_share_mode(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter;
	DBusError			err;
	unsigned int		policerIndex=0,countIndex=0,num=0,ret=QOS_RETURN_CODE_ERROR,IsShare=QOS_POLICER_DISABLE;
	QOS_POLICER_STC     *policerPtr =NULL;
	unsigned long		cir=0,cbs=0,tmpcir=0,tmpcbs=0;
	dbus_error_init(&err);
		   
	if (!(dbus_message_get_args ( msg, &err,  
	   DBUS_TYPE_UINT32,&policerIndex, 
	   DBUS_TYPE_UINT32,&countIndex,
	   DBUS_TYPE_UINT32,&IsShare,
	   DBUS_TYPE_INVALID))) {
	   npd_syslog_err("Unable to get input args ");
	   if (dbus_error_is_set(&err)) {
		   npd_syslog_err("%s raised: %s",err.name,err.message);
		   dbus_error_free(&err);
	   }
	   return NULL;
	}
	
	npd_syslog_dbg("output policerIndex %d,countIndex %d",policerIndex,countIndex);

	policerPtr=g_policer[policerIndex];	

	if(policerPtr==NULL) {
		ret = QOS_RETURN_CODE_BAD_PTR;
		npd_syslog_err("illegal policer index");
	}
	else{
		ret = npd_find_share_num_by_policer_id(policerIndex, &num);
		/*api 0 for share 1 for noshare*/
		/*when noshare cir = cir*n */
		if (!ret && (policerPtr->sharemode != IsShare)) {
			tmpcir = policerPtr->cir;
			tmpcbs = policerPtr->cbs;
			/*set share mode*/
			if (!IsShare) {
				policerPtr->sharemode = IsShare;
			}
			/*set noshare mode*/
			else {
				policerPtr->sharemode = IsShare;
				policerPtr->cir = (policerPtr->cir)*num;
				policerPtr->cbs = (policerPtr->cbs)*num;
			}
			ret=nam_qos_policer_entry_set(policerIndex,policerPtr,&cir,&cbs);
			if(ret!=QOS_RETURN_CODE_SUCCESS) {
			       npd_syslog_err("policer set fail\n");
			}
			else {
				ret = QOS_RETURN_CODE_SUCCESS;
			}

			policerPtr->cir = tmpcir;
			policerPtr->cbs = tmpcbs;
		}
		npd_syslog_dbg("output policerIndex %d,num %d, ret is %d",policerIndex,num, ret);
	}
	reply = dbus_message_new_method_return(msg);	
	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	return reply;

}

DBusMessage * npd_dbus_policer_enable(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter;
	DBusError			err;
	unsigned int    	policerIndex =0,IsEnable = 0;
	unsigned int    	ret = QOS_RETURN_CODE_ERROR;
	QOS_POLICER_STC 	*policerPtr=NULL;
	unsigned long		cir=0,cbs=0;
	
	if(!((productinfo.capbmp) & FUNCTION_QOS_VLAUE)){
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_QOS_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_QOS_VLAUE)){
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else{
		dbus_error_init(&err);
	   
		if (!(dbus_message_get_args ( msg, &err,  
		   DBUS_TYPE_UINT32,&policerIndex,
		   DBUS_TYPE_UINT32,&IsEnable,
		   DBUS_TYPE_INVALID))) {
		   npd_syslog_err("Unable to get input args ");
		   if (dbus_error_is_set(&err)) {
			   npd_syslog_err("%s raised: %s",err.name,err.message);
			   dbus_error_free(&err);
		   }
		   return NULL;
		}
		
		npd_syslog_dbg("output profileIndex %d,IsEnable %d",policerIndex,IsEnable);

		policerPtr = g_policer[policerIndex];
		if(policerPtr==NULL) {
			ret = QOS_RETURN_CODE_BAD_PTR;
			npd_syslog_err("illegal policer index");
		}
		else{		
			if(IsEnable ==QOS_POLICER_ENABLE){
				policerPtr->policerEnable=QOS_POLICER_ENABLE;
				ret=nam_qos_policer_entry_set(policerIndex,policerPtr,&cir,&cbs);
				if(ret!=QOS_RETURN_CODE_SUCCESS) {
					npd_syslog_err("policer set fail\n");
				}
			}
			/*if enable,set hw,then disable,just disable bit,after enable,but write hw again*/
			else if(IsEnable ==QOS_POLICER_DISABLE){
				policerPtr->policerEnable=QOS_POLICER_DISABLE;
				ret=nam_qos_policer_invalid(policerIndex);
				if(ret!=QOS_RETURN_CODE_SUCCESS) {
					npd_syslog_err("policer invalid fail\n");
				}
			}
		}
	}
	reply = dbus_message_new_method_return(msg);	
	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&cir);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&cbs);
	
	return reply;	
}

DBusMessage * npd_dbus_global_policer_meter_mode(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter;
	DBusError			err;
	unsigned int		mru = 0,ret=QOS_RETURN_CODE_ERROR;

	if(!((productinfo.capbmp) & FUNCTION_QOS_VLAUE)){
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_QOS_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_QOS_VLAUE)){
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else{
		dbus_error_init(&err);
		   
		if (!(dbus_message_get_args ( msg, &err,  
		   DBUS_TYPE_UINT32,&mru,
		   DBUS_TYPE_INVALID))) {
		   npd_syslog_err("Unable to get input args ");
		   if (dbus_error_is_set(&err)) {
			   npd_syslog_err("%s raised: %s",err.name,err.message);
			   dbus_error_free(&err);
		   }
		   return NULL;
		}
		g_g_policer.meter =POLICER_TB_LOOSE_E;	
		g_g_policer.mru = (QOS_POLICER_MRU_E)mru;

		ret = nam_qos_global_policer_meter(g_g_policer.meter,g_g_policer.mru);
		if(ret!=QOS_RETURN_CODE_SUCCESS) {
			npd_syslog_err("fail to set global policer register!\n");
		}
	}
	
	reply = dbus_message_new_method_return(msg);	
	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	
	return reply;	
}

DBusMessage * npd_dbus_global_policing_mode(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter;
	DBusError			err;
	unsigned int		mode=0,ret = 0;

	if(!((productinfo.capbmp) & FUNCTION_QOS_VLAUE)){
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_QOS_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_QOS_VLAUE)){
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else{
		dbus_error_init(&err);
			   
		if (!(dbus_message_get_args ( msg, &err,  
		   DBUS_TYPE_UINT32,&mode, 
		   DBUS_TYPE_INVALID))) {
		   npd_syslog_err("Unable to get input args ");
		   if (dbus_error_is_set(&err)) {
			   npd_syslog_err("%s raised: %s",err.name,err.message);
			   dbus_error_free(&err);
		   }
		   return NULL;
		}
		
		npd_syslog_dbg("output mode %d",mode);
		g_g_policer.meter =POLICER_TB_STRICT_E;
		g_g_policer.policing = (QOS_POLICING_MODE_E)mode;

		ret = nam_qos_global_policing(g_g_policer.policing);
		if(ret!=QOS_RETURN_CODE_SUCCESS) {
			npd_syslog_err("fail to set global policer register!\n");
		}
	}
	
	reply = dbus_message_new_method_return(msg);	
	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	
	return reply;
}

DBusMessage * npd_dbus_set_counter(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter;
	DBusError			err;
	unsigned int		countIndex=0,ret = 0;
	unsigned long 		inIndex=0,outIndex=0;
	QOS_COUNTER_STC		*counterptr=NULL;

	if(!((productinfo.capbmp) & FUNCTION_QOS_VLAUE))
	{
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_QOS_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_QOS_VLAUE))
	{
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		dbus_error_init(&err);
			   
		if (!(dbus_message_get_args ( msg, &err,  
		   DBUS_TYPE_UINT32,&countIndex, 
		   DBUS_TYPE_UINT32,&inIndex,
		   DBUS_TYPE_UINT32,&outIndex,
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
		
		npd_syslog_dbg("output countIndex %d,inIndex %ld,outIndex %ld",countIndex,inIndex,outIndex);

		counterptr=g_counter[countIndex];
		if(counterptr!=NULL)
		{
			npd_syslog_dbg("existed this policer\n");	
			counterptr->inProfileBytesCnt = inIndex;
			counterptr->outOfProfileBytesCnt =outIndex;
		}
		else 
		{
			counterptr = (QOS_COUNTER_STC *)malloc(sizeof(QOS_COUNTER_STC));
			if(NULL == counterptr) 
			{
				npd_syslog_err("memory alloc error for QOS_COUNTER_STC !!");
				ret = QOS_RETURN_CODE_ERROR; 	
			}
			else
			{
				/*code optimize: Dereference after null check
				zhangcl@autelan.com 2013-1-17*/
				memset(counterptr,0,sizeof(QOS_COUNTER_STC));
				counterptr->inProfileBytesCnt = inIndex;
				counterptr->outOfProfileBytesCnt =outIndex;	
				npd_syslog_dbg("inProfileBytesCnt %lu\n",counterptr->inProfileBytesCnt);
				npd_syslog_dbg("outOfProfileBytesCnt %lu\n",counterptr->outOfProfileBytesCnt);
				g_counter[countIndex]=counterptr;
			}
		}	
		ret = nam_qos_set_counter(countIndex,inIndex,outIndex);
		if(ret!=QOS_RETURN_CODE_SUCCESS) 
		{
			npd_syslog_err("fail to set counter!\n");
		}
	}

	reply = dbus_message_new_method_return(msg);	
	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	
	return reply;
}

DBusMessage * npd_dbus_read_counter(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter;
	DBusError			err;
	unsigned int		counterIndex=0,ret = 0;
	unsigned long		Inprofile=0,OutProfile=0;

	if(!((productinfo.capbmp) & FUNCTION_QOS_VLAUE)){
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_QOS_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_QOS_VLAUE)){
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else{
		dbus_error_init(&err);
			   
		if (!(dbus_message_get_args ( msg, &err,  
		   DBUS_TYPE_UINT32,&counterIndex, 
		   DBUS_TYPE_INVALID))) {
		   npd_syslog_err("Unable to get input args ");
		   if (dbus_error_is_set(&err)) {
			   npd_syslog_err("%s raised: %s",err.name,err.message);
			   dbus_error_free(&err);
		   }
		   return NULL;
		}	
		npd_syslog_dbg("output counterIndex %d",counterIndex);
		if(NULL==g_counter[counterIndex]) {
			ret = QOS_RETURN_CODE_BAD_PTR;
		}
		else{
			ret = nam_qos_read_counter(counterIndex,&Inprofile,&OutProfile);
			if(ret!=QOS_RETURN_CODE_SUCCESS) {
				npd_syslog_err("fail to read counter info!\n");
			}
		}
	}
	reply = dbus_message_new_method_return(msg);	
	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&Inprofile);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&OutProfile);
	
	return reply;
}

DBusMessage * npd_dbus_show_policer(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter_array, iter;
	DBusError			err;
	POLICER_SHOW_STC	*showProfile[MAX_POLICER_NUM];
	QOS_POLICER_STC 	*policer = NULL;
	unsigned int		j=0,i=0,count=0,null_num=0,ret=QOS_RETURN_CODE_SUCCESS;

	dbus_error_init(&err);

	for(i=0;i<MAX_POLICER_NUM;i++){
		if(NULL!=g_policer[i]){
		    policer = g_policer[i];
			npd_syslog_dbg("policer %p\n",policer);
			showProfile[count]=(POLICER_SHOW_STC*)malloc(sizeof(POLICER_SHOW_STC));
			if(NULL==showProfile[count]) return NULL;
			memset(showProfile[count],0,sizeof(QOS_POLICER_STC));
			showProfile[count]->policerIndex =i;
			showProfile[count]->sharemode=policer->sharemode;
			showProfile[count]->cbs = policer->cbs;
			showProfile[count]->cir = policer->cir;
			showProfile[count]->cmd = policer->cmd;
			showProfile[count]->counterEnable = policer->counterEnable;
			showProfile[count]->counterSetIndex = policer->counterSetIndex;
			showProfile[count]->meterColorMode = policer->meterColorMode;
			showProfile[count]->policerEnable =policer->policerEnable;
			if(policer->cmd == OUT_PROFILE_REMAP_ENTRY)
				showProfile[count]->qosProfile=policer->qosProfile;
			showProfile[count]->mode=g_g_policer.meter;	
			showProfile[count]->mru=g_g_policer.mru;
			showProfile[count]->packetsize =g_g_policer.policing;
			count++;
		}
		else{
			null_num++;
		}
	}

	if(null_num==MAX_POLICER_NUM){
		ret = QOS_RETURN_CODE_POLICER_NOT_EXISTED;
		npd_syslog_dbg("no policer existed\n");
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret); 
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &count);
	npd_syslog_dbg("count %d\n",count);
	dbus_message_iter_open_container (&iter,
										DBUS_TYPE_ARRAY,
										DBUS_STRUCT_BEGIN_CHAR_AS_STRING	 /*begin*/
										DBUS_TYPE_UINT32_AS_STRING	 /*policerIndex*/
										DBUS_TYPE_UINT32_AS_STRING	  /*policer sharemode*/
										DBUS_TYPE_UINT32_AS_STRING	 /*cbs*/
										DBUS_TYPE_UINT32_AS_STRING	 /*cir*/
										DBUS_TYPE_UINT32_AS_STRING	 /*cmd*/
										DBUS_TYPE_UINT32_AS_STRING	 /*counterEnable*/
										DBUS_TYPE_UINT32_AS_STRING    /*counterSetIndex*/
										DBUS_TYPE_UINT32_AS_STRING    /*meterColorMode	*/		
										DBUS_TYPE_UINT32_AS_STRING    /*policerEnable*/
										DBUS_TYPE_UINT32_AS_STRING    /*qosProfile	*/
										DBUS_TYPE_UINT32_AS_STRING	 /*globalmode*/
										DBUS_TYPE_UINT32_AS_STRING    /*globalmru*/
										DBUS_TYPE_UINT32_AS_STRING    /*globalpacket	*/
										DBUS_STRUCT_END_CHAR_AS_STRING, 	/*end*/
									&iter_array);
	for (j = 0; j < count; j++ ) {
		DBusMessageIter iter_struct;
		dbus_message_iter_open_container (&iter_array,
										   DBUS_TYPE_STRUCT,
										   NULL,
										   &iter_struct);
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(showProfile[j]->policerIndex));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(showProfile[j]->sharemode));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(showProfile[j]->cir));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(showProfile[j]->cbs));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(showProfile[j]->cmd));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(showProfile[j]->counterEnable)); 
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(showProfile[j]->counterSetIndex));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(showProfile[j]->meterColorMode)); 		
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(showProfile[j]->policerEnable)); 
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(showProfile[j]->qosProfile)); 
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(showProfile[j]->mode));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(showProfile[j]->mru)); 
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(showProfile[j]->packetsize)); 
		dbus_message_iter_close_container (&iter_array, &iter_struct);
		free(showProfile[j]);
		showProfile[j]=NULL;
		npd_syslog_dbg("bless you!\n");
	}
	dbus_message_iter_close_container (&iter, &iter_array);

	return reply;	
}

DBusMessage * npd_dbus_delete_policer(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter;
	DBusError			err;
	unsigned int		policerIndex =0, i = 0, indexcheck = 0;
	unsigned int		ret = QOS_RETURN_CODE_ERROR;
			
	dbus_error_init(&err);
   
	if (!(dbus_message_get_args ( msg, &err,  
	   DBUS_TYPE_UINT32,&policerIndex,
	   DBUS_TYPE_INVALID))) {
	   npd_syslog_err("Unable to get input args ");
	   if (dbus_error_is_set(&err)) {
		   npd_syslog_err("%s raised: %s",err.name,err.message);
		   dbus_error_free(&err);
	   }
	   return NULL;
	}
	npd_syslog_dbg("policer index %d",policerIndex);
	for (i = 0; i < MAX_POLICER_NUM; i++) {
		if (NULL != alias_policer[i]) {
			if (((policerIndex >= (alias_policer[i]->startpolicer)) && 
				(policerIndex <= (alias_policer[i]->endpolicer))) ||
				(policerIndex == (alias_policer[i]->aliaspolicer))){
				indexcheck = 1;
				break;
			}
		}
	}
	if(NULL == g_policer[policerIndex]) {
		ret = QOS_RETURN_CODE_POLICER_NOT_EXISTED;
	}
	else if (1 == indexcheck) {
		ret = QOS_RETURN_CODE_ERROR;
	}
	else {
		/* npd_error  can not delete,npd_succes ,can*/
		ret=npd_acl_policer_use_check(policerIndex);
		if(ret!=QOS_RETURN_CODE_SUCCESS){
			npd_syslog_dbg("error! can not delete!\n");
			ret=QOS_RETURN_CODE_POLICER_USE_IN_ACL;
		}
		else{
		    free(g_policer[policerIndex]);
			g_policer[policerIndex]=NULL;
		}
	}		 
   reply = dbus_message_new_method_return(msg);
   dbus_message_iter_init_append (reply, &iter);   
   dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,
									&ret);

   return reply;
}

DBusMessage * npd_dbus_delete_policer_range(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter;
	DBusError			err;
	unsigned int		policerIndex =0, startPid = 0, endPid = 0;
	unsigned int		ret = QOS_RETURN_CODE_ERROR, i = 0, j = 0, aliasindex = 0;
			
	dbus_error_init(&err);
   
	if (!(dbus_message_get_args ( msg, &err,  
		DBUS_TYPE_UINT32,&startPid,
		DBUS_TYPE_UINT32,&endPid,
		DBUS_TYPE_INVALID))) {
	npd_syslog_err("Unable to get input args ");
	if (dbus_error_is_set(&err)) {
		npd_syslog_err("%s raised: %s",err.name,err.message);
		dbus_error_free(&err);
	}
	   return NULL;
	}
	npd_syslog_dbg("policer index %d",policerIndex);
	for (i = 0; i < MAX_POLICER_NUM; i++) {
		if (NULL != alias_policer[i]) {
			if ((alias_policer[i]->startpolicer == startPid) &&
				(alias_policer[i]->endpolicer == endPid)) {
				aliasindex = i;
				break;
			}
		}
	}
	
	if (MAX_POLICER_NUM == i) {
		ret = QOS_RETURN_CODE_ERROR;
	}
	else {
		for (j = 0; j < (endPid - startPid + 1); j++) {
			policerIndex = startPid + j;
			/* npd_error  can not delete,npd_succes ,can*/
			ret = npd_acl_policer_use_check(policerIndex);
			if(ret!=QOS_RETURN_CODE_SUCCESS){
				npd_syslog_dbg("error! can not delete!\n");
				ret=QOS_RETURN_CODE_POLICER_USE_IN_ACL;
			}
			else{
			    free(g_policer[policerIndex]);
				g_policer[policerIndex]=NULL;
			}
		}
		free(alias_policer[aliasindex]);
		alias_policer[aliasindex] = NULL;
	}		 
   reply = dbus_message_new_method_return(msg);
   dbus_message_iter_init_append (reply, &iter);   
   dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,
									&ret);

   return reply;
}

DBusMessage * npd_dbus_queue_scheduler(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter;
	DBusError			err;
	unsigned int		algFlag=0;
	unsigned int		ret = QOS_RETURN_CODE_SUCCESS;
	unsigned int 		i = 0,j = 0,eth_g_index = 0,tmp = 0;
	unsigned char		devNum = 0,virPortId = 0;
	unsigned int     loop = 0,slot_index=0;

	if(!((productinfo.capbmp) & FUNCTION_QOS_VLAUE)){
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_QOS_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_QOS_VLAUE)){
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else{
	dbus_error_init(&err);
   
	if (!(dbus_message_get_args ( msg, &err,  
	   DBUS_TYPE_UINT32,&algFlag,
	   DBUS_TYPE_INVALID))) {
	   npd_syslog_err("Unable to get input args ");
	   if (dbus_error_is_set(&err)) {
		   npd_syslog_err("%s raised: %s",err.name,err.message);
		   dbus_error_free(&err);
	   }
	   return NULL;
	}
	if(algFlag == 0) {
		g_queue_type = QOS_PORT_TX_SP_ARB_GROUP_E;
	
		if(!PRODUCT_IS_BOX) {/* chassis product */
			i = 1; /* slot 0:CRSMU port not add to default vlan*/
				loop = CHASSIS_SLOT_COUNT;
			}
		else { /* box product */
			i = CHASSIS_SLOT_START_NO; /* as default we assume box product has subcard */
			loop = CHASSIS_SLOT_COUNT;

			/* !!!excetion!!!
			 * box product without subcard
			 * override loop count */
			if(CHASSIS_SLOT_START_NO == CHASSIS_SLOT_COUNT) {
				loop = CHASSIS_SLOT_COUNT+1;
			}
		}
		for(;i < loop; i++)
		{
			/*npd_syslog_dbg("Creating eth port for slot index %d and mod type %d",i,MODULE_TYPE_ON_SLOT_INDEX(i);*/
		
			if (CHASSIS_SLOTNO_ISLEGAL(i)){
				slot_index = CHASSIS_SLOT_NO2INDEX(i);
				npd_syslog_dbg("slot_index %d\n",slot_index);
				npd_syslog_dbg("ETH_LOCAL_PORT_COUNT(%d) %d\n",slot_index,ETH_LOCAL_PORT_COUNT(slot_index));
				
				for (j = ETH_LOCAL_PORT_START_NO(slot_index);
				     j < ETH_LOCAL_PORT_COUNT(slot_index); j++) 
				{
				
					if (ETH_LOCAL_PORTNO_ISLEGAL(i,j)){
					   eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(i,j);
					   npd_syslog_dbg("eth_g_index %d\n",eth_g_index);
					   tmp = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortId);
						if(tmp != 0){
							tmp = NPD_DBUS_ERROR_NO_SUCH_PORT;			
							npd_syslog_dbg("npd_get_devport_by_global_index::get devport Error.\n");
						}
						else 
						{
						#if 0
							for(tcQueue=0;tcQueue<8;tcQueue++){
							  tmp = cpssDxChPortTxQueueTxEnableSet(devNum,virPortId,tcQueue,0);
							  if(tmp != 0){
								  	  
								  npd_syslog_dbg("cpssDxChPortTxQueueTxEnableSet Error.\n");
							  }

							}
						 #endif
						}
					}
				}
			}
		}
	
		ret=nam_qos_queue_sp_set();
		if(ret!=QOS_RETURN_CODE_SUCCESS)	
			npd_syslog_err("set sp fail\n");
		
		else
		{
		#if 0
			ret=cpssDxChPortTxQueueUpdateScheduler(devNum,1);
			if(ret!=QOS_RETURN_CODE_SUCCESS)	
			    npd_syslog_err("cpssDxChPortTxQueueUpdateScheduler fail\n");
         #endif
		}
		i=0;j=0;devNum=0;virPortId=0;
		for(;i < loop; i++)
		{
			/*npd_syslog_dbg("Creating eth port for slot index %d and mod type %d",i,MODULE_TYPE_ON_SLOT_INDEX(i);*/
		
			if (CHASSIS_SLOTNO_ISLEGAL(i)){
				slot_index = CHASSIS_SLOT_NO2INDEX(i);
				npd_syslog_dbg("slot_index %d\n",slot_index);
				npd_syslog_dbg("ETH_LOCAL_PORT_COUNT(%d) %d\n",slot_index,ETH_LOCAL_PORT_COUNT(slot_index));
				
				for (j = ETH_LOCAL_PORT_START_NO(slot_index);
				     j < ETH_LOCAL_PORT_COUNT(slot_index); j++) 
				{
				
					if (ETH_LOCAL_PORTNO_ISLEGAL(i,j)){
					   eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(i,j);
					   npd_syslog_dbg("eth_g_index %d\n",eth_g_index);
					   tmp = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortId);
						if(tmp != 0){
							tmp = NPD_DBUS_ERROR_NO_SUCH_PORT;			
							npd_syslog_dbg("npd_get_devport_by_global_index::get devport Error.\n");
						}
						else 
						{
						#if 0
							for(tcQueue=0;tcQueue<8;tcQueue++){
							  tmp= cpssDxChPortTxQueueTxEnableSet(devNum,virPortId,tcQueue,1);
							  if(tmp != 0){
								  	  
								  npd_syslog_dbg("cpssDxChPortTxQueueTxEnableSet Error.\n");
							  }

							}
						#endif
						}
						
					}
				}
			}
		}
	
	}
	else if (algFlag == 2) {
		ret = nam_qos_queue_schedu_check(algFlag, 0);
		if (QOS_RETURN_CODE_SUCCESS == ret) {
			if (g_queue_type != QOS_PORT_TX_SP_WRR_ARB_E) {
				for (i=0; i<8; i++) {
					ret = nam_qos_schedu_wrr_set(1,0,i,i+1);
					if(ret != QOS_RETURN_CODE_SUCCESS)
						npd_syslog_err("nam_qos_schedu_wrr_set fail,ret %d\n",ret);
					g_hybrid[i].groupFlag = QOS_PORT_TX_WRR_ARB_GROUP0_E;
					g_hybrid[i].weight = 0;
					g_hybrid_showrunn[i].groupFlag = QOS_PORT_TX_WRR_ARB_GROUP0_E;
					g_hybrid_showrunn[i].weight = 0;
				}
				ret=nam_qos_queue_sp_set();
			      if(ret != QOS_RETURN_CODE_SUCCESS)
					npd_syslog_err("set sp fail\n");
				g_queue_type = QOS_PORT_TX_SP_WRR_ARB_E;
			}
		}
	}	
	else {
		if (g_queue_type != QOS_PORT_TX_WRR_ARB_E) {
			for(i=0 ;i<8; i++) {
				ret = nam_qos_schedu_wrr_set(1,0,i,i+1);
				if(ret != QOS_RETURN_CODE_SUCCESS)
					npd_syslog_err("nam_qos_schedu_wrr_set fail,ret %d\n",ret);
				g_queue[i].groupFlag = QOS_PORT_TX_WRR_ARB_GROUP0_E;
				g_queue[i].weight = i+1;
				g_queue_showrunn[i].groupFlag=QOS_PORT_TX_WRR_ARB_GROUP0_E;
				g_queue_showrunn[i].weight =0;
			}
			g_queue_type = QOS_PORT_TX_WRR_ARB_E;
		}
	}
	}
	/*note:if not config ,wrr,wrr+sp,both all TX in group1,for show more easy*/
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,
									&ret);
	
 	 return reply;
}

DBusMessage * npd_dbus_queue_sche_wrr_group(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter;
	DBusError			err;
	unsigned int		wrrflag = 0,groupFlag=0,tc =0,weight=0;
	unsigned int		ret = QOS_RETURN_CODE_SUCCESS;
	unsigned int		i = 0,j = 0,eth_g_index = 0,tmp = 0;
	unsigned char		devNum = 0,virPortId = 0;
	unsigned int	 loop = 0,slot_index=0;

	if(!((productinfo.capbmp) & FUNCTION_QOS_VLAUE)){
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_QOS_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_QOS_VLAUE)){
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else{
		dbus_error_init(&err);
	    
		if (!(dbus_message_get_args ( msg, &err, 
		   DBUS_TYPE_UINT32,&wrrflag,
		   DBUS_TYPE_UINT32,&groupFlag,
		   DBUS_TYPE_UINT32,&tc,
		   DBUS_TYPE_UINT32,&weight,
		   DBUS_TYPE_INVALID))) {
		   npd_syslog_err("Unable to get input args ");
		   if (dbus_error_is_set(&err)) {
			   npd_syslog_err("%s raised: %s",err.name,err.message);
			   dbus_error_free(&err);
		   }
		   return NULL;
		}
		ret = nam_qos_queue_schedu_check(wrrflag, groupFlag);
		if (QOS_RETURN_CODE_SUCCESS == ret) {
			if ((wrrflag == 1)&&(weight == QOS_SCH_GROUP_IS_SP)) {
				ret = QOS_RETURN_CODE_ERROR;
			}
			else if ((wrrflag == 2)&&(groupFlag == QOS_PORT_TX_WRR_ARB_GROUP1_E)&&(weight == QOS_SCH_GROUP_IS_SP)) {
				ret = QOS_RETURN_CODE_ERROR;
			}	
			else if ((wrrflag == 2)&&(weight == QOS_SCH_GROUP_IS_SP)) {
				g_queue_type =QOS_PORT_TX_SP_WRR_ARB_E;
				g_hybrid[tc].groupFlag = groupFlag;
				g_hybrid[tc].weight = 0;
				g_hybrid_showrunn[tc].groupFlag = groupFlag;
				g_hybrid_showrunn[tc].weight = 0;
				ret = nam_qos_queue_sp_set_one(tc);
				if(ret!=QOS_RETURN_CODE_SUCCESS) 	
					npd_syslog_err("nam_qos_queue_sp_set_one fail\n");
			}
			else {
				if(wrrflag==1){
					g_queue_type =QOS_PORT_TX_WRR_ARB_E;
					g_queue[tc].groupFlag=groupFlag;
					g_queue_showrunn[tc].groupFlag=groupFlag;
					g_queue[tc].weight=weight;
					g_queue_showrunn[tc].weight=weight;
				}
				else if(wrrflag==2){
					g_queue_type =QOS_PORT_TX_SP_WRR_ARB_E;
					g_hybrid[tc].groupFlag =groupFlag;
					g_hybrid_showrunn[tc].groupFlag =groupFlag;
					g_hybrid[tc].weight=weight;
					g_hybrid_showrunn[tc].weight=weight;
				}
				if(!PRODUCT_IS_BOX) {/* chassis product */
					i = 1; /* slot 0:CRSMU port not add to default vlan*/
						loop = CHASSIS_SLOT_COUNT;
					}
				else { /* box product */
					i = CHASSIS_SLOT_START_NO; /* as default we assume box product has subcard */
					loop = CHASSIS_SLOT_COUNT;
				
					/* !!!excetion!!!
					 * box product without subcard
					 * override loop count */
					if(CHASSIS_SLOT_START_NO == CHASSIS_SLOT_COUNT) {
						loop = CHASSIS_SLOT_COUNT+1;
					}
				}

				for(;i < loop; i++)
				{
					/*npd_syslog_dbg("Creating eth port for slot index %d and mod type %d",i,MODULE_TYPE_ON_SLOT_INDEX(i);*/
				
					if (CHASSIS_SLOTNO_ISLEGAL(i)){
						slot_index = CHASSIS_SLOT_NO2INDEX(i);
						npd_syslog_dbg("slot_index %d\n",slot_index);
						npd_syslog_dbg("ETH_LOCAL_PORT_COUNT(%d) %d\n",slot_index,ETH_LOCAL_PORT_COUNT(slot_index));
						
						for (j = ETH_LOCAL_PORT_START_NO(slot_index);
						     j < ETH_LOCAL_PORT_COUNT(slot_index); j++) 
						{
						
							if (ETH_LOCAL_PORTNO_ISLEGAL(i,j)){
							   eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(i,j);
							   npd_syslog_dbg("eth_g_index %d\n",eth_g_index);
							   tmp = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortId);
								if(tmp != 0){
									tmp = NPD_DBUS_ERROR_NO_SUCH_PORT;			
									npd_syslog_dbg("npd_get_devport_by_global_index::get devport Error.\n");
								}
								else 
								{
								#if 0
									for(tcQueue=0;tcQueue<8;tcQueue++){
										/* Disable dequeueing on ports associated with the profile by 
										clearing the <EnTxTC[7:0]> fieldfield in thePort<n> Txq 
										Configuration Register 	(Table 564 p. 1001). */						
									  tmp= cpssDxChPortTxQueueTxEnableSet(devNum,virPortId,tcQueue,0);
									  if(tmp != 0){
										  	  
										  npd_syslog_dbg("cpssDxChPortTxQueueTxEnableSet Error.\n");
									  }

									}
								#endif
								}
							}
						}
					}		
					/*port*/
					/* Update the profile attributes. */
				ret=nam_qos_schedu_wrr_set(wrrflag,groupFlag,tc,weight);
				if(ret!=QOS_RETURN_CODE_SUCCESS) {	
					npd_syslog_err("nam_qos_schedu_wrr_set fail\n");
				}
				
				else
				{
			#if 0
					/* Set the <UpdateSched VarTrigger> bit in the Transmit Queue Control 
					Register (Table 553 p. 995) whichloads the new profile configuration. */
					ret=cpssDxChPortTxQueueUpdateScheduler(devNum,1);
					if(ret!=QOS_RETURN_CODE_SUCCESS)	
					    npd_syslog_err("cpssDxChPortTxQueueUpdateScheduler fail\n");
		    #endif 
				}
				i=0;j=0;devNum=0;virPortId=0;
				for(;i < loop; i++)
				{
					/*npd_syslog_dbg("Creating eth port for slot index %d and mod type %d",i,MODULE_TYPE_ON_SLOT_INDEX(i);*/
				
					if (CHASSIS_SLOTNO_ISLEGAL(i)){
						slot_index = CHASSIS_SLOT_NO2INDEX(i);
						npd_syslog_dbg("slot_index %d\n",slot_index);
						npd_syslog_dbg("ETH_LOCAL_PORT_COUNT(%d) %d\n",slot_index,ETH_LOCAL_PORT_COUNT(slot_index));
						
						for (j = ETH_LOCAL_PORT_START_NO(slot_index);
						     j < ETH_LOCAL_PORT_COUNT(slot_index); j++) 
						{
						
							if (ETH_LOCAL_PORTNO_ISLEGAL(i,j)){
							   eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(i,j);
							   npd_syslog_dbg("eth_g_index %d\n",eth_g_index);
							   tmp = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortId);
								if(tmp != 0){
									tmp = NPD_DBUS_ERROR_NO_SUCH_PORT;			
									npd_syslog_dbg("npd_get_devport_by_global_index::get devport Error.\n");
								}
								else 
								{
							#if 0
									for(tcQueue=0;tcQueue<8;tcQueue++){
									  tmp= cpssDxChPortTxQueueTxEnableSet(devNum,virPortId,tcQueue,1);
									 /* tmp &= cpssDxChPortTxFlushQueuesSet(devNum, virPortId);*/
									  if(tmp != 0){
										  	  
										  npd_syslog_dbg("cpssDxChPortTxQueueTxEnableSet Error.\n");
									  }

									}
							#endif
								}
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
									DBUS_TYPE_UINT32,
									&ret);

   return reply;
}

DBusMessage * npd_dbus_show_queue_scheduler(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter, iter_array;
	DBusError			err;
	unsigned int		algFlag = 0;
	unsigned int		i = 0,j = 0,k = 0;
	QOS_WRR_TX_WEIGHT_E  show[8];
	
	dbus_error_init(&err);

	if(g_queue_type==QOS_PORT_TX_SP_ARB_GROUP_E) {
		algFlag=0;
	}
	else if(g_queue_type==QOS_PORT_TX_DEFAULT_WRR_ARB_E) {
		algFlag=1;
	}
	else if(g_queue_type==QOS_PORT_TX_WRR_ARB_E) {
		algFlag=2;	
		for(j=0;j<8;j++) {
			show[j]=g_queue[j];
		}
		
	}
	else if(g_queue_type==QOS_PORT_TX_SP_WRR_ARB_E){
		algFlag=3;
		for(k=0;k<8;k++) {
			show[k]=g_hybrid[k];
		}
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &algFlag);	
	
	dbus_message_iter_open_container (&iter,
								   DBUS_TYPE_ARRAY,
								    DBUS_STRUCT_BEGIN_CHAR_AS_STRING     /*begin*/
								       DBUS_TYPE_UINT32_AS_STRING    /*groupflag*/
									   DBUS_TYPE_UINT32_AS_STRING    /*weight*/
									DBUS_STRUCT_END_CHAR_AS_STRING,     /*end*/
								    &iter_array);
 	 for (i = 0; i < 8; i++ ) {
		DBusMessageIter iter_struct;
		dbus_message_iter_open_container (&iter_array,
										   DBUS_TYPE_STRUCT,
										   NULL,
										   &iter_struct);
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(show[i].groupFlag));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(show[i].weight));
				
		dbus_message_iter_close_container (&iter_array, &iter_struct);	
	}
	dbus_message_iter_close_container (&iter, &iter_array);

	return reply;	
   
}

DBusMessage * npd_dbus_traffic_shape(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter;
	DBusError			err;
	unsigned int	    algFlag=0,queueId=0,g_eth_index=0,burst=0,mode = 1,kmstate=0;
	unsigned int		ret = QOS_RETURN_CODE_SUCCESS;
	unsigned long       maxrate=0;
	unsigned char	    devNum=0,virPort=0;
	unsigned int	    portIns=NPD_DBUS_ERROR_NO_SUCH_PORT;
	struct eth_port_s	 *portInfo = NULL;
	QOS_TRAFFIC_SHAPE_PORT_STC  *shapePtr=NULL;

	if(!((productinfo.capbmp) & FUNCTION_QOS_VLAUE))
	{
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_QOS_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_QOS_VLAUE))
	{
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		dbus_error_init(&err);
	   
		if (!(dbus_message_get_args ( msg, &err,  
			DBUS_TYPE_UINT32,&g_eth_index,
			DBUS_TYPE_UINT32,&algFlag,
		    DBUS_TYPE_UINT32,&queueId,
		    DBUS_TYPE_UINT32,&maxrate,
		    DBUS_TYPE_UINT32,&kmstate,
		    DBUS_TYPE_UINT32,&burst,
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
		npd_syslog_dbg("g_eth_index %d,algFlag %d,queueId %d,maxRate %d,burst %d\n",g_eth_index,algFlag,queueId,maxrate,burst);
		portIns = npd_get_devport_by_global_index(g_eth_index,&devNum,&virPort);
		if(portIns!=QOS_RETURN_CODE_SUCCESS)
		{
			ret= NPD_DBUS_ERROR_NO_SUCH_PORT;
			npd_syslog_dbg("NPD_DBUS_ERROR_NO_SUCH_PORT\n");
		}
		else
		{
			portInfo = npd_get_port_by_index(g_eth_index);
			if(NULL == portInfo) 
			{
				ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
		    }
			else
			{
				shapePtr=portInfo->funcs.func_data[ETH_PORT_FUNC_QoS_SHAPE];
				if(NULL==shapePtr)
				{
					shapePtr=(QOS_TRAFFIC_SHAPE_PORT_STC*)malloc(sizeof(QOS_TRAFFIC_SHAPE_PORT_STC));
					if(NULL==shapePtr) 
						return NULL;
					memset(shapePtr,0,sizeof(QOS_TRAFFIC_SHAPE_PORT_STC));
				}
				
				if (kmstate == 0) 
				{
					if (maxrate > 4096) 
					{
						ret = ACL_RETURN_CODE_BAD_k;
					}	
					else 
					{
						maxrate = maxrate*64;
					}
				}
				else if (kmstate == 1) 
				{
					if (maxrate > 1000) 
					{
						ret = ACL_RETURN_CODE_BAD_M;
					}
					else 
					{
						maxrate = maxrate*1024;
					}
				}
				if (ret == QOS_RETURN_CODE_SUCCESS) 
				{
					if(algFlag==0)
					{
						shapePtr->portEnable=1;   /*ture*/
						shapePtr->burstSize=burst;
						shapePtr->kmstate = kmstate;
						shapePtr->Maxrate  =maxrate;
						ret = nam_qos_traffic_shape_port(devNum,virPort,mode,burst,maxrate);
						if(ret!=QOS_RETURN_CODE_SUCCESS)
						{
							npd_syslog_err("nam_qos_traffic_shape_port fail!");
							/*code optimize:  Resource leak
							zhangcl@autelan.com 2013-1-17*/
							free(shapePtr);
						}
					}/*port*/
					else if(algFlag==1)
					{
						shapePtr->queue[queueId].queueEnable=1;
						shapePtr->queue[queueId].Maxrate=maxrate;
						shapePtr->queue[queueId].burstSize=burst;
						shapePtr->queue[queueId].kmstate = kmstate;
						ret =nam_qos_traffic_shape_queue_port(devNum,virPort,mode,queueId,burst,maxrate);
					
						if(ret!=QOS_RETURN_CODE_SUCCESS) 
						{
							npd_syslog_err("nam_qos_traffic_shape_queue_port fail!");	
							/*code optimize:  Resource leak
							zhangcl@autelan.com 2013-1-17*/
							free(shapePtr);
						}
					}/*queue on port*/
					portInfo->funcs.func_data[ETH_PORT_FUNC_QoS_SHAPE]=shapePtr;
				}
				else
				{
					/*code optimize:  Resource leak
					zhangcl@autelan.com 2013-1-17*/
					free(shapePtr);
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

DBusMessage * npd_dbus_show_traffic_shape(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter, iter_array;
	DBusError			err;
	unsigned int		i = 0,j = 0,k = 0,ret = 0,g_eth_index=0;
	QOS_SHAPER_QUEUE    show[8];
   	struct eth_port_s	 *portInfo = NULL;
	QOS_TRAFFIC_SHAPE_PORT_STC	*shapePtr=NULL;
	unsigned int 		burstSize=0,portEnable=0;
	unsigned long       maxRate=0;

	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,  
			DBUS_TYPE_UINT32,&g_eth_index,			
			DBUS_TYPE_INVALID))) {
		   npd_syslog_err("Unable to get input args ");
		   if (dbus_error_is_set(&err)) {
			   npd_syslog_err("%s raised: %s",err.name,err.message);
			   dbus_error_free(&err);
		   }
		   return NULL;
	}
	for(k=0;k<8;k++){
		memset(&show[k],0,sizeof(QOS_SHAPER_QUEUE));
	}
	portInfo = npd_get_port_by_index(g_eth_index);
	if(NULL == portInfo) {
		ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
		npd_syslog_dbg("NPD_DBUS_ERROR_NO_SUCH_PORT\n");
	}
	else{
		shapePtr=portInfo->funcs.func_data[ETH_PORT_FUNC_QoS_SHAPE];
		npd_syslog_dbg("showPtr %p\n",shapePtr);
		if(NULL==shapePtr){
				ret=QOS_RETURN_CODE_ERROR;
				npd_syslog_dbg("no infos on port!\n");
		}
		else{
/*
			if(shapePtr->kmstate == 0)
				burstSize =shapePtr->burstSize;
			else
*/
			burstSize =shapePtr->burstSize;
			maxRate   =shapePtr->Maxrate;
			portEnable=shapePtr->portEnable;
			for(j=0;j<8;j++)
			{
				/*npd_syslog_dbg("show[%d] %p\n",j,show[j]);*/
/*
				if (shapePtr->queue[j].kmstate == 0)
					show[j].burstSize = shapePtr->queue[j].burstSize;
				else
*/
				show[j].burstSize = shapePtr->queue[j].burstSize;
				show[j].Maxrate = shapePtr->queue[j].Maxrate;
				show[j].queueEnable =shapePtr->queue[j].queueEnable;
/*				npd_syslog_dbg("show[j].burstSize %d\n",show[j].burstSize);
				npd_syslog_dbg("show[j].Maxrate %d\n",show[j].Maxrate);
				npd_syslog_dbg("show[j].queueEnable %d\n",show[j].queueEnable);
				npd_syslog_dbg("****\n");*/
			}
				ret=QOS_RETURN_CODE_SUCCESS;
		}
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &portEnable);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &burstSize);	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &maxRate);
	
	dbus_message_iter_open_container (&iter,
								   DBUS_TYPE_ARRAY,
								    DBUS_STRUCT_BEGIN_CHAR_AS_STRING     /*begin*/
									   DBUS_TYPE_UINT32_AS_STRING     /*queueEnable*/
									   DBUS_TYPE_UINT32_AS_STRING    /*burstsize*/
									   DBUS_TYPE_UINT32_AS_STRING    /*maxrate*/
									DBUS_STRUCT_END_CHAR_AS_STRING,     /*end*/
								    &iter_array);
 	 for (i = 0; i < 8; i++ ) {
		DBusMessageIter iter_struct;
		dbus_message_iter_open_container (&iter_array,
										   DBUS_TYPE_STRUCT,
										   NULL,
									   &iter_struct);
/*		npd_syslog_dbg("%d======\n",i);
		npd_syslog_dbg("show[i].queueEnable %d\n",show[i].queueEnable);
		npd_syslog_dbg("show[i].burstSize %d\n",show[i].burstSize);
		npd_syslog_dbg("show[i].Maxrate %d\n",show[i].Maxrate);
		npd_syslog_dbg("=====\n");*/
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(show[i].queueEnable));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(show[i].burstSize));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(show[i].Maxrate));
				
		dbus_message_iter_close_container (&iter_array, &iter_struct);	
	}
	dbus_message_iter_close_container (&iter, &iter_array);

	return reply;  
}

DBusMessage * npd_dbus_delete_traffic_shape(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*		reply;	  
	DBusMessageIter 	iter;
	DBusError			err;
	unsigned int	    algFlag=0,queueId=0,g_eth_index=0,mode = 0;
	unsigned int		ret = QOS_RETURN_CODE_ERROR;
	unsigned char	    devNum=0,virPort=0;
	unsigned int	    portIns=NPD_DBUS_ERROR_NO_SUCH_PORT;
	struct eth_port_s	 *portInfo = NULL;
	QOS_TRAFFIC_SHAPE_PORT_STC  *shapePtr=NULL;
	
	dbus_error_init(&err);
   
	if (!(dbus_message_get_args ( msg, &err,  
		DBUS_TYPE_UINT32,&g_eth_index,
		DBUS_TYPE_UINT32,&algFlag,
	    DBUS_TYPE_UINT32,&queueId,
	    DBUS_TYPE_INVALID))) {
	   npd_syslog_err("Unable to get input args ");
	   if (dbus_error_is_set(&err)) {
		   npd_syslog_err("%s raised: %s",err.name,err.message);
		   dbus_error_free(&err);
	   }
	   return NULL;
	}
    npd_syslog_dbg("g_eth_index %d,algFlag %d,queueId %d\n",g_eth_index,algFlag,queueId);
	portIns = npd_get_devport_by_global_index(g_eth_index,&devNum,&virPort);
	if(portIns!=QOS_RETURN_CODE_SUCCESS){
		ret= NPD_DBUS_ERROR_NO_SUCH_PORT;
		npd_syslog_dbg("NPD_DBUS_ERROR_NO_SUCH_PORT\n");
	}
	else{
		portInfo = npd_get_port_by_index(g_eth_index);
		if(NULL == portInfo) {
			ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
	    }
		else{
			shapePtr=portInfo->funcs.func_data[ETH_PORT_FUNC_QoS_SHAPE];
			if(NULL==shapePtr){
				npd_syslog_dbg("port has no traffic shape infos \n");
				ret = QOS_RETURN_CODE_TRAFFIC_NO_INFO;
			}
			else{
				if(algFlag==0){
					shapePtr->portEnable=0;
					shapePtr->burstSize=0;
					shapePtr->Maxrate  =0;
					ret = nam_qos_del_traffic_shape_port(devNum,virPort,mode);
					if(ret!=QOS_RETURN_CODE_SUCCESS)
						npd_syslog_err("clear!!!!!nam_qos_traffic_shape_port fail!");
					npd_syslog_dbg("port####################\n");
			
				}/*port*/
				else if(algFlag==1){
					shapePtr->queue[queueId].queueEnable=0;
					shapePtr->queue[queueId].Maxrate=0;
					shapePtr->queue[queueId].burstSize=0;
					ret =nam_qos_del_traffic_shape_queue_port(devNum,virPort,mode,queueId);
					if(ret!=QOS_RETURN_CODE_SUCCESS)
						npd_syslog_err("clear1!!!!nam_qos_traffic_shape_queue_port fail!");
					npd_syslog_dbg("queue$$$$$$$$$$$$$$$$$$\n");
			
				}/*queue on port*/
			}
			/*api ,check if all clear ,need or not*/
		}
	}
		
   reply = dbus_message_new_method_return(msg);
   dbus_message_iter_init_append (reply, &iter);   
   dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,
									&ret);
   return reply;
}

DBusMessage * npd_dbus_qos_counter_show_running_config
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	DBusMessage* reply;
	DBusMessageIter  iter;

	char *showStr = NULL,*cursor = NULL;
	int k=0, totalLen = 0;
	unsigned char enter_node_qos=0;	
	QOS_COUNTER_STC *counter = NULL;
	                   
	showStr = (char*)malloc(NPD_QOS_SHOWRUN_CFG_SIZE);
	if(NULL == showStr) {
		npd_syslog_err("memory malloc error\n");
		return NULL;
	}
	memset(showStr,0,NPD_QOS_SHOWRUN_CFG_SIZE);
	cursor = showStr;

	/* add for distributed qos, wangchong@autelan.com 2012-5-17 */
	totalLen += sprintf(cursor,"config qos switch-board %d\n",SLOT_ID);                       
	cursor = showStr + totalLen;
	enter_node_qos = 1;
	if(g_counter) {
		for(k=0;k<MAX_COUNTER_NUM;k++) {
			counter = g_counter[k];
			if(NULL==counter) {
				continue;
			}
			else {	
				totalLen += sprintf(cursor,"set counter %d inprofile %lu outprofile %lu\n",k,counter->inProfileBytesCnt,counter->outOfProfileBytesCnt);                       
				cursor = showStr + totalLen; 
			}
		}
	}
	if(enter_node_qos) 
	{
		totalLen += sprintf(cursor," exit\n");
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

DBusMessage * npd_dbus_qos_profile_show_running_config
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{	
	DBusMessage* reply;
	DBusMessageIter  iter;

	QOS_PROFILE_STC   *qosProfile=NULL; 

	char *showStr = NULL,*cursor = NULL;
	int k=0,totalLen = 0;
	unsigned char enter_node=0;
	unsigned char enter_node_qos=0;
	 	
                       
	showStr = (char*)malloc(NPD_QOS_SHOWRUN_CFG_SIZE);
	if(NULL == showStr) {
		npd_syslog_err("memory malloc error\n");
		return NULL;
	}
	memset(showStr,0,NPD_QOS_SHOWRUN_CFG_SIZE);
	cursor = showStr;
    /* add for distributed qos, wangchong@autelan.com 2012-5-11 */
	totalLen += sprintf(cursor,"config qos switch-board %d\n",SLOT_ID);                       
	cursor = showStr + totalLen;
	enter_node_qos = 1;

	if(g_qos_profile) {
		for(k=0;k<MAX_PROFILE_NUMBER;k++) {
			qosProfile = g_qos_profile[k];
			enter_node = 0;
			if(NULL == qosProfile) {
				continue;
			}
			else {
				char tmpBuf[2048]={0},*tmpPtr=NULL;
				tmpPtr = tmpBuf; 

				totalLen += sprintf(cursor,"set qos-profile %d\n",k);                       
				cursor = showStr + totalLen;   
	            enter_node = 1;
				
				if(enter_node) {
					totalLen += sprintf(cursor," dp ");
					cursor = showStr + totalLen;
					if(qosProfile->dropPrecedence ==0){
					    totalLen += sprintf(cursor,"0 ");
				  	    cursor = showStr + totalLen;
	                 }
	                 else if(qosProfile->dropPrecedence ==2){
	                    totalLen += sprintf(cursor,"1 ");
				  	    cursor = showStr + totalLen;
	                }
					totalLen += sprintf(cursor,"up %d ",qosProfile->userPriority);
					cursor = showStr + totalLen;  
					
	                totalLen += sprintf(cursor,"tc %d ",qosProfile->trafficClass);
					cursor = showStr + totalLen;  
					
					totalLen += sprintf(cursor,"dscp %d\n",qosProfile->dscp);
					cursor = showStr + totalLen; 
					
					totalLen += sprintf(cursor," exit\n");
					cursor = showStr + totalLen;
				}
			}/*if*/	                                                             
		}/*for*/
	}
	
	if(enter_node_qos) 
	{
		totalLen += sprintf(cursor," exit\n");
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

DBusMessage * npd_dbus_qos_remap_table_show_running_config
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	DBusMessage* reply;
	DBusMessageIter  iter;

	QOS_UP_PROFILE_MAP_STC     *upprofile = NULL;
	QOS_DSCP_PROFILE_MAP_STC   *dscpprofile = NULL;
	QOS_DSCP_DSCP_REMAP_STC    *dscpdscp = NULL;

	char *showStr = NULL,*cursor = NULL;
	int k=0,j=0,i=0,totalLen = 0;
	unsigned char enter_node_qos=0;
	showStr = (char*)malloc(NPD_QOS_SHOWRUN_CFG_SIZE);
	if(NULL == showStr) {
		npd_syslog_err("memory malloc error\n");
		return NULL;
	}
	memset(showStr,0,NPD_QOS_SHOWRUN_CFG_SIZE);
	cursor = showStr;
    /* add for distributed qos, wangchong@autelan.com 2012-5-11 */
	totalLen += sprintf(cursor,"config qos switch-board %d\n",SLOT_ID);                       
	cursor = showStr + totalLen;
	enter_node_qos = 1;

	if(g_up_profile) {
		for(k=0;k<MAX_UP_PROFILE_NUM;k++) {
			upprofile = g_up_profile[k];
			if(NULL == upprofile) {
				continue;
			}
			else {	             
				totalLen += sprintf(cursor,"set up-to-profile %d %d\n",k,upprofile->profileIndex);                       
				cursor = showStr + totalLen;   			
			}/*if	*/                                                             
		}/*for*/
	}

	if(g_dscp_profile) {
		for(j=0;j<MAX_DSCP_PROFILE_NUM;j++) {
			dscpprofile = g_dscp_profile[j];
			if(NULL ==dscpprofile) {
				continue;
			}
			else {	             
				totalLen += sprintf(cursor,"set dscp-to-profile %d %d\n",j,dscpprofile->profileIndex);                       
				cursor = showStr + totalLen;   			
			}/*if*/	                                                             
		}/*for*/
	}
	
	if(g_dscp_dscp) {
    	for(i=0;i<MAX_DSCP_PROFILE_NUM;i++) {
			dscpdscp = g_dscp_dscp[i];
			if(NULL ==dscpdscp) {
				continue;
			}
			else {             
				totalLen += sprintf(cursor,"set dscp-to-dscp %d %d\n",i,dscpdscp->NewDscp);                       
				cursor = showStr + totalLen;   			
			}/*if*/	                                                             
		}/*for*/
	}
	if(enter_node_qos) 
	{
		totalLen += sprintf(cursor," exit\n");
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

DBusMessage * npd_dbus_qos_policy_map_show_running_config
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	DBusMessage* reply;
	DBusMessageIter  iter;

	QOS_PORT_POLICY_MAP_ATTRIBUTE_STC  *policy=NULL;

	char *showStr = NULL,*cursor = NULL;
	int j=0,k=0,totalLen = 0;
	unsigned char enter_node=0;
	unsigned char enter_node_qos=0;
	unsigned int policymapId = 0;
                       
	showStr = (char*)malloc(NPD_QOS_SHOWRUN_CFG_SIZE);
	if(NULL == showStr) {
		npd_syslog_err("memory malloc error\n");
		return NULL;
	}
	memset(showStr,0,NPD_QOS_SHOWRUN_CFG_SIZE);
	cursor = showStr;
    /* add for distributed qos, wangchong@autelan.com 2012-5-11 */
	totalLen += sprintf(cursor,"config qos switch-board %d\n",SLOT_ID);                       
	cursor = showStr + totalLen;
	enter_node_qos = 1;

	if(g_policy_map) {
		for(k=1;k<MAX_POLICY_MAP_NUM;k++) {
			policy = g_policy_map[k];
			int length=0;
			enter_node = 0;
			if(NULL == policy) {
				continue;
			}
			else {
				char tmpBuf[2048]={0},*tmpPtr=NULL;
				tmpPtr = tmpBuf; 

				totalLen += sprintf(cursor,"create policy-map %d\n",k);                       
				cursor = showStr + totalLen;   
	           			          							
				if((0!=policy->trustFlag)||(0!=policy->assignPrecedence))
				{
	                enter_node =1;
	             }
	             
				
	            if(0!=policy->assignPrecedence){
	                length += sprintf(tmpPtr,"config qos-markers disable\n");
				    tmpPtr = tmpBuf + length;                                    
	            }
			    if(0!=policy->trustFlag){
	                 length += sprintf(tmpPtr,"trust-mode ");
	                  tmpPtr = tmpBuf + length; 
	                  enter_node =1;
	                  if(1==policy->trustFlag) {
	                      length += sprintf(tmpPtr,"l2 up ");
	                      tmpPtr = tmpBuf + length;  
	                      if(DISABLE_E==policy->modifyUp)  {
	                            length += sprintf(tmpPtr,"disable\n");
	                            tmpPtr = tmpBuf + length;                     
	                      } 
	                      else if(ENABLE_E==policy->modifyUp){
	                            length += sprintf(tmpPtr,"enable\n");
	                            tmpPtr = tmpBuf + length; 
	                      }         
	                  } 
	                  else if(2==policy->trustFlag){
	                        length += sprintf(tmpPtr,"l3 dscp ");
	                        tmpPtr = tmpBuf + length;  
	                        if(DISABLE_E==policy->modifyDscp)  {
	                            length += sprintf(tmpPtr,"disable remap ");
	                            tmpPtr = tmpBuf + length;                     
	                        } 
	                        else if(ENABLE_E==policy->modifyDscp){
	                            length += sprintf(tmpPtr,"enable remap ");
	                            tmpPtr = tmpBuf + length; 
	                        }         
	                       
	                        if(0==policy->remapDscp){
	                              length += sprintf(tmpPtr,"disable\n");
	                              tmpPtr = tmpBuf + length;                     
	                        }
	                        else if(1==policy->remapDscp){
	                              length += sprintf(tmpPtr,"enable\n");
	                              tmpPtr = tmpBuf + length;                     
	                        }
	                  }
	                  else if(3==policy->trustFlag) {
	                        length += sprintf(tmpPtr,"l2+l3 up ");
	                        tmpPtr = tmpBuf + length;  
	                        if(DISABLE_E==policy->modifyUp)  {
	                            length += sprintf(tmpPtr,"disable dscp ");
	                            tmpPtr = tmpBuf + length;                     
	                        } 
	                        else if(ENABLE_E==policy->modifyUp){
	                            length += sprintf(tmpPtr,"enable dscp ");
	                            tmpPtr = tmpBuf + length; 
	                        }         
	                       
	                        if(DISABLE_E==policy->modifyDscp)  {
	                            length += sprintf(tmpPtr,"disable remap ");
	                            tmpPtr = tmpBuf + length;                     
	                        } 
	                        else if(ENABLE_E==policy->modifyDscp){
	                            length += sprintf(tmpPtr,"enable remap ");
	                            tmpPtr = tmpBuf + length; 
	                        }         
	                       
	                        if(0==policy->remapDscp){
	                              length += sprintf(tmpPtr,"disable\n");
	                              tmpPtr = tmpBuf + length;                     
	                        }
	                        else if(1==policy->remapDscp){
	                              length += sprintf(tmpPtr,"enable\n");
	                              tmpPtr = tmpBuf + length;                     
	                        }
	                       
	                  }         
	            }	   
				if(enter_node) {
					
					totalLen += sprintf(cursor,"config policy-map %d\n",k);                       
				       cursor = showStr + totalLen;  
				
					totalLen += sprintf(cursor," %s",tmpBuf);
					cursor = showStr + totalLen;  
								
					totalLen += sprintf(cursor," exit\n");
					cursor = showStr + totalLen;
				}
			}/*if	*/                                                             
		}/*for*/
	}
	if(enter_node_qos) 
	{
		totalLen += sprintf(cursor," exit\n");
		cursor = showStr + totalLen;
	}
	/* wangchong@autelan.com if the configuration of qos policy-map bound by a port*/
 	unsigned int slot_index = SLOT_ID - 1;
	unsigned char local_port_count = ETH_LOCAL_PORT_COUNT(slot_index);
	syslog_ax_acl_dbg("*************** local_port_count %d\n",local_port_count);
	for (j = 0; j < local_port_count; j++ ) 
	{
		unsigned char local_port_no = ETH_LOCAL_INDEX2NO(slot_index,j);
		unsigned int eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(slot_index,j);
		char tmpBuf[2048] = {0},*tmpPtr = NULL;
		int length = 0;
		enter_node = 0;
		tmpPtr = tmpBuf;
        npd_syslog_dbg("*************** eth-port %d/%d\n",slot_index+1,local_port_no);		
	    npd_syslog_dbg("*************** eth_g_index %d\n",eth_g_index);
    	if(QOS_RETURN_CODE_POLICY_MAP_BIND==npd_qos_port_bind_check(eth_g_index,&policymapId))
		{
        	if(policymapId!=0)
			{
                enter_node = 1;	                
                length += sprintf(tmpPtr," bind policy-map %d\n",policymapId);
        		tmpPtr = tmpBuf + length;    
        	}
    	}
		if(enter_node) 
		{   /* port configured*/
			if((totalLen + length + 20 + 5) > NPD_ETHPORT_SHOWRUN_CFG_SIZE)
			{ /* 20 for enter node; 5 for exit node*/
				syslog_ax_acl_err("show ethport buffer full");
				break;
			}
			totalLen += sprintf(cursor,"config eth-port %d/%d\n",slot_index+1,local_port_no);
			cursor = showStr + totalLen;
			totalLen += sprintf(cursor,"%s",tmpBuf);
			cursor = showStr + totalLen;
			totalLen += sprintf(cursor,"exit\n");
			cursor = showStr + totalLen;
			enter_node = 0;
		}
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

DBusMessage * npd_dbus_qos_policer_show_running_config
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	DBusMessage* reply;
	DBusMessageIter  iter;
	
	QOS_POLICER_STC   *policerPtr=NULL;	
	QOS_POLICER_ALIAS	*aliasPtr = NULL;
	
	char *showStr = NULL,*cursor = NULL;
	int k=0,totalLen = 0;
	unsigned char enter_node_qos=0;
	                   
	showStr = (char*)malloc(NPD_QOS_SHOWRUN_CFG_SIZE);
	if(NULL == showStr) {
		npd_syslog_err("memory malloc error\n");
		return NULL;
	}
	memset(showStr,0,NPD_QOS_SHOWRUN_CFG_SIZE);
	cursor = showStr;
    /* add for distributed qos, wangchong@autelan.com 2012-5-11 */
	totalLen += sprintf(cursor,"config qos switch-board %d\n",SLOT_ID);                       
	cursor = showStr + totalLen;
	enter_node_qos = 1;

	/*global setting*/
	if(POLICER_TB_STRICT_E==g_g_policer.meter){
		if(POLICER_PACKET_SIZE_L1_INCLUDE_E!=g_g_policer.policing)
		{
			totalLen += sprintf(cursor,"policer strict packetsize ");                       
			cursor = showStr + totalLen; 

			if(POLICER_PACKET_SIZE_L3_ONLY_E==g_g_policer.policing)
			{
				totalLen += sprintf(cursor,"l3\n");                       
				cursor = showStr + totalLen; 
			}
			if(POLICER_PACKET_SIZE_L2_INCLUDE_E==g_g_policer.policing)
			{
				totalLen += sprintf(cursor,"l2\n");                       
				cursor = showStr + totalLen; 
			}
		}
	}
	else if(POLICER_TB_LOOSE_E==g_g_policer.meter){
		
		totalLen += sprintf(cursor,"policer loose mru ");						
		cursor = showStr + totalLen; 
		
		if(POLICER_MRU_1536_E==g_g_policer.mru)
		{
			totalLen += sprintf(cursor,"0\n");						
			cursor = showStr + totalLen;	
		}
		else if(POLICER_MRU_2K_E==g_g_policer.mru)
		{
			totalLen += sprintf(cursor,"1\n");						
			cursor = showStr + totalLen;	
		}
		else if(POLICER_MRU_10K_E==g_g_policer.mru)
		{
			totalLen += sprintf(cursor,"2\n");						
			cursor = showStr + totalLen;	
		}
	}
	
	if(g_policer) {
		for(k=0;k<MAX_POLICER_NUM;k++) {
			policerPtr = g_policer[k];
		if ((NULL == policerPtr) || (1 == policerPtr->rangepolicer)){
				continue;
			}
			else {
				totalLen += sprintf(cursor,"set policer %d\n",k);                       
				cursor = showStr + totalLen; 
				if((0!=policerPtr->cir)&&(0!=policerPtr->cbs)){
					totalLen += sprintf(cursor," policer cir %ld cbs %ld\n",policerPtr->cir,policerPtr->cbs);                       
					cursor = showStr + totalLen; 
				}
				if(0!=policerPtr->counterEnable){
					totalLen += sprintf(cursor," counter %d enable\n",policerPtr->counterSetIndex);                       
					cursor = showStr + totalLen; 
				}
				if(OUT_PROFILE_KEEP_E!=policerPtr->cmd){
					totalLen += sprintf(cursor," config out-profile\n");						 
					cursor = showStr + totalLen; 

					if(OUT_PROFILE_DROP==policerPtr->cmd){
						totalLen += sprintf(cursor,"  drop\n");						 
						cursor = showStr + totalLen;
					}
					else if(OUT_PROFILE_REMAP_ENTRY==policerPtr->cmd){
						totalLen += sprintf(cursor,"  remap %d\n",policerPtr->qosProfile);						 
						cursor = showStr + totalLen;
					}
					totalLen += sprintf(cursor," exit\n");						 
					cursor = showStr + totalLen;
				}
			
				totalLen += sprintf(cursor," exit\n");						 
				cursor = showStr + totalLen;

				/*policer enable*/
				if(1==policerPtr->policerEnable) {
					totalLen += sprintf(cursor,"policer %d enable\n",k);                       
					cursor = showStr + totalLen; 
				}
			}/*if	*/
		}/*for*/
	}
	
	if(alias_policer) {
		for(k=0;k<MAX_POLICER_NUM;k++) {
			aliasPtr = alias_policer[k];
			if(NULL == aliasPtr) {
				continue;
			}
			else {
				totalLen += sprintf(cursor, "policer-range %d %d alias %d\n", aliasPtr->startpolicer, aliasPtr->endpolicer, aliasPtr->aliaspolicer);                       
				cursor = showStr + totalLen; 
			}
		}
	}
	if(enter_node_qos) 
	{
		totalLen += sprintf(cursor," exit\n");
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

DBusMessage * npd_dbus_qos_queue_sch_show_running_config
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	DBusMessage* reply;
	DBusMessageIter  iter;
	char *showStr = NULL,*cursor = NULL;
	int i=0,totalLen = 0;
	unsigned char enter_node_qos=0;
	unsigned char enter_node=0;
		
	showStr = (char*)malloc(NPD_QOS_SHOWRUN_CFG_SIZE);
	if(NULL == showStr) {
		npd_syslog_err("memory malloc error\n");
		return NULL;
	}
	memset(showStr,0,NPD_QOS_SHOWRUN_CFG_SIZE);
	cursor = showStr;
    /* add for distributed qos, wangchong@autelan.com 2012-5-11 */
	totalLen += sprintf(cursor,"config qos switch-board %d\n",SLOT_ID);                       
	cursor = showStr + totalLen;
	enter_node_qos = 1;

	if (QOS_PORT_TX_SP_ARB_GROUP_E == g_queue_type) {
		totalLen += sprintf(cursor,"queue-scheduler sp\n");						 
		cursor = showStr + totalLen; 
	}
	else if (QOS_PORT_TX_WRR_ARB_E == g_queue_type) {
		totalLen += sprintf(cursor,"queue-scheduler wrr\n");						 
		cursor = showStr + totalLen;
		for(i=0; i<8; i++){
		if((0 == g_queue_showrunn[i].groupFlag)&&(g_queue_showrunn[i].weight != 0)) {					
			totalLen += sprintf(cursor," wrr group1 %d %d\n",i,g_queue_showrunn[i].weight);						 
			cursor = showStr + totalLen;
		}
		else if((1 == g_queue_showrunn[i].groupFlag)&&(g_queue_showrunn[i].weight != 0)) {
			totalLen += sprintf(cursor," wrr group2 %d %d\n",i,g_queue_showrunn[i].weight);						 
			cursor = showStr + totalLen;
			}
		}
		totalLen += sprintf(cursor," exit\n");				 
     		cursor = showStr + totalLen;
	}
	else if(QOS_PORT_TX_SP_WRR_ARB_E == g_queue_type) {
		totalLen += sprintf(cursor,"queue-scheduler hybrid\n");
		cursor = showStr + totalLen;
		
		for(i=0; i<8; i++) {
			if((0 == g_hybrid_showrunn[i].groupFlag)&&(g_hybrid_showrunn[i].weight != 0)) {					
				totalLen += sprintf(cursor," wrr group1 %d %d\n",i,g_hybrid_showrunn[i].weight);						 
				cursor = showStr + totalLen;
				}
			else if((1 == g_hybrid_showrunn[i].groupFlag)&&(g_hybrid_showrunn[i].weight != 0)) {
				totalLen += sprintf(cursor," wrr group2 %d %d\n",i,g_hybrid_showrunn[i].weight);						 
				cursor = showStr + totalLen;
				}
		}
		totalLen += sprintf(cursor," exit\n");					 
		cursor = showStr + totalLen;
	}
    if(enter_node_qos) 
	{
		totalLen += sprintf(cursor," exit\n");
		cursor = showStr + totalLen;
	}
	/*wangchong add qos traffic-shape for ethport*/
    int j=0;
 	unsigned int slot_index = SLOT_ID - 1;
	QOS_TRAFFIC_SHAPE_PORT_STC  shapeDate;
	unsigned char local_port_count = ETH_LOCAL_PORT_COUNT(slot_index);
	syslog_ax_acl_dbg("*************** local_port_count %d\n",local_port_count);
	
	for (j = 0; j < local_port_count; j++ ) 
	{
		unsigned char local_port_no = ETH_LOCAL_INDEX2NO(slot_index,j);
		unsigned int eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(slot_index,j);
		char tmpBuf[2048] = {0},*tmpPtr = NULL;
		int length = 0;
		enter_node = 0;
		
		tmpPtr = tmpBuf;
        npd_syslog_dbg("*************** eth-port %d/%d\n",slot_index+1,local_port_no);		
	    npd_syslog_dbg("*************** eth_g_index %d\n",eth_g_index);
		memset(&shapeDate, 0,sizeof(QOS_TRAFFIC_SHAPE_PORT_STC));
		
	if(0==npd_qos_traffic_shape_check(eth_g_index,&shapeDate)){
		enter_node = 1;
		int j=0;				
		if(NPD_TRUE==shapeDate.portEnable){
			if (shapeDate.kmstate == 0) {
				length += sprintf(tmpPtr," traffic-shape %ld k %ld\n",(shapeDate.Maxrate)/64,(shapeDate.burstSize));
				tmpPtr = tmpBuf + length;
			}
			else if (shapeDate.kmstate == 1) {
				length += sprintf(tmpPtr," traffic-shape %ld m %ld\n",(shapeDate.Maxrate)/1024,(shapeDate.burstSize));
				tmpPtr = tmpBuf + length;
			}
		}
		for(j=0;j<8;j++){
			if(NPD_TRUE==(shapeDate.queue[j].queueEnable))
			{
				if (shapeDate.queue[j].kmstate == 0) {
					length += sprintf(tmpPtr," traffic-shape queue %d %ld k %ld\n",j,(shapeDate.queue[j].Maxrate)/64,(shapeDate.queue[j].burstSize));
					tmpPtr = tmpBuf + length; 
				}
				else if (shapeDate.queue[j].kmstate == 1) {
					length += sprintf(tmpPtr," traffic-shape queue %d %ld m %ld\n",j,(shapeDate.queue[j].Maxrate)/1024,(shapeDate.queue[j].burstSize));
					tmpPtr = tmpBuf + length; 
				}
			}
		}
	}
		if(enter_node) 
		{   /* port configured*/
			if((totalLen + length + 20 + 5) > NPD_ETHPORT_SHOWRUN_CFG_SIZE)
			{ /* 20 for enter node; 5 for exit node*/
				syslog_ax_acl_err("show ethport buffer full");
				break;
			}
			totalLen += sprintf(cursor,"config eth-port %d/%d\n",slot_index+1,local_port_no);
			cursor = showStr + totalLen;
			totalLen += sprintf(cursor,"%s",tmpBuf);
			cursor = showStr + totalLen;
			totalLen += sprintf(cursor,"exit\n");
			cursor = showStr + totalLen;
			enter_node = 0;
		}
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

DBusMessage * npd_dbus_qos_mode_show_running_config
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	DBusMessage* reply;
	DBusMessageIter  iter;
	
	char *showStr = NULL,*cursor = NULL;
	int totalLen = 0;
	unsigned char enter_node_qos=0;
	                   
	showStr = (char*)malloc(NPD_QOS_SHOWRUN_CFG_SIZE);
	if(NULL == showStr) {
		npd_syslog_dbg("Malloc failed!\n");
		return NULL;
	}
	memset(showStr,0,NPD_QOS_SHOWRUN_CFG_SIZE);
	cursor = showStr;
    /* add for distributed qos, wangchong@autelan.com 2012-5-11 */
	totalLen += sprintf(cursor,"config qos switch-board %d\n",SLOT_ID);                       
	cursor = showStr + totalLen;
	enter_node_qos = 1;
	
      npd_syslog_dbg("qos_mode value %d\n",qos_mode);

	if (qos_mode != QOS_MODE_DEFAULT) {
		if (qos_mode == QOS_MODE_FLOW) {
			totalLen += sprintf(cursor,"config qos-mode flow\n");
			cursor = showStr + totalLen;
		}	
		else if (qos_mode == QOS_MODE_PORT) {
			totalLen += sprintf(cursor,"config qos-mode port\n");
			cursor = showStr + totalLen;
		}
		else if (qos_mode == QOS_MODE_HYBRID) {
			totalLen += sprintf(cursor,"config qos-mode hybrid\n");
			cursor = showStr + totalLen;
		}
	}
	if(enter_node_qos) 
	{
		totalLen += sprintf(cursor," exit\n");
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

#ifdef __cplusplus
}
#endif

