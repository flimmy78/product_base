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
* npd_acl.c
*
*
* CREATOR:
*		qinhs@autelan.com
*
* DESCRIPTION:
*		API used in NPD dbus process for ACL module
*
* DATE:
*		02/21/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.116 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "sysdef/npd_sysdef.h"
#include "util/npd_list.h"
#include "npd/nam/npd_amapi.h"
#include "npd/nbm/npd_bmapi.h"
#include "npd_c_slot.h"
#include "npd_eth_port.h"
#include "npd_log.h"
#include "npd_product.h"
#include "npd_acl_drv.h"
#include "npd_acl.h"
#include "npd_qos.h"
#include "npd_acl_api.h"
#include "npd_vlan.h"
#include "dbus/npd/npd_dbus_def.h"
#include "sysdef/returncode.h"


unsigned char g_acl_enable = ACL_FALSE;
AclRuleList  **g_acl_rule = NULL;
struct acl_range_rule	**r_acl_rule = NULL;
struct acl_group_list_s *acl_group[MAX_GROUP_NUM];
struct acl_group_list_s *egress_acl_group[MAX_GROUP_NUM];
QosAppendProfile  **qos_append = NULL;
ACL_TIME_RANGE_STC		**g_time=NULL;
extern QOS_MODE			qos_mode;
struct group_udp_vlan_rule		**udp_rule = NULL;

static char *packetTypeStr[] = {
	"IP","UDP","TCP","ICMP","ETHERNET","ERR"
};

static char *actionTypeStr[] = {
	"permit","deny","trap","mirror","redirect","err"
};

/* pcl rule format descriptors */
static char *ruleFormatStr[] = {	\
	"STD_NOIP",	\
	"STD_IPL2",	\
	"STD_IPv4L4",	\
	"STD_IPv6Dip",	\
	"EXT_NOIPv6",	\
	"EXT_IPv6L2",		\
	"EXT_IPv6L4",		\
	"QoS",			\
	"Unknown"
};

/*********************************************************
*  npd_acl_init
*
*  DESCRIPTION:
* 	  Acl rule struction init ,and acl group ,group info on port init
*
*  INPUT:
*  
*  OUTPUT:
*
*  RETURN: 	
*
***********************************************************/
void npd_acl_init(void) {
	int i =0,j=0,k=0,m=0;
	
	g_acl_rule = (AclRuleList**)malloc(sizeof(AclRuleList*)*MAX_ACL_RULE_NUMBER);
	if(NULL == g_acl_rule) {
		syslog_ax_acl_dbg("memory alloc error for acl init!!");
		goto errRet;
	}
	r_acl_rule = (struct acl_range_rule**)malloc(sizeof(struct acl_range_rule*)*MAX_ACL_RULE_NUMBER);
	if(NULL == r_acl_rule) {
		syslog_ax_acl_dbg("memory alloc error for acl init!!");
		goto errRet;
	}
	g_time = (ACL_TIME_RANGE_STC**)malloc(sizeof(ACL_TIME_RANGE_STC*)*1000);	
	if(NULL == g_time) {
		syslog_ax_acl_dbg("memory alloc error for time-based acl init!!");
		goto errRet;
	}
	qos_append = (QosAppendProfile**)malloc(sizeof(QosAppendProfile*)*MAX_ACL_RULE_NUMBER);
	if(NULL == qos_append) {
		syslog_ax_acl_dbg("memory alloc error for qos init!!");
		goto errRet;
	}
	udp_rule = (struct group_udp_vlan_rule**)malloc(sizeof(struct group_udp_vlan_rule*)*4093);
	if(NULL == udp_rule) {
		syslog_ax_acl_dbg("memory alloc error for udp vlan rule init!!");
		goto errRet;
	}
	for(;i<MAX_ACL_RULE_NUMBER;i++) {
		g_acl_rule[i] = NULL;
		r_acl_rule[i] = NULL;
	}
	for(;m<MAX_ACL_EXT_RULE_NUMBER;m++) {
		qos_append[m] = NULL;
	}
	for(;j<MAX_GROUP_NUM;j++){
		acl_group[j]=NULL;			
		egress_acl_group[j]=NULL;
	}
	for(;k<1000;k++){
		g_time[k]=NULL;			
	}
	/*if(PRODUCT_ID_AX5K_E == PRODUCT_ID || PRODUCT_ID_AX5608== PRODUCT_ID){
		syslog_ax_acl_dbg("PRODUCT_ID_AX5K_E not support acl.\n");
		return;
	}*/
	if(!((productinfo.capbmp) & FUNCTION_ACL_VLAUE)){
		npd_syslog_dbg("do not support acl initial!\n");
		return;
	}
	else if(((productinfo.capbmp) & FUNCTION_ACL_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ACL_VLAUE)){
		npd_syslog_dbg("do not support acl initial!\n");
		return;
	}
	else{
		nam_acl_service_init();
		npd_acl_service_enable(ACL_FALSE, ACL_TRUE);
		nam_acl_service(g_acl_enable);

		return;
		
		errRet:
		if(g_acl_rule) {
			free(g_acl_rule);
			g_acl_rule = NULL;
		}
		if(r_acl_rule) {
			free(r_acl_rule);
			r_acl_rule = NULL;
		}
		if(g_time)	{
			free(g_time);
			g_time = NULL;
		}
		if(qos_append)	{
			free(qos_append);
			qos_append = NULL;
		}
		if(udp_rule)	{
			free(udp_rule);
			udp_rule = NULL;
		}
		return;
	}
}

DBusMessage * npd_dbus_config_acl(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	DBusMessage* 	reply = NULL;
	DBusMessageIter	iter;
	unsigned int	ret = 0;
	unsigned char 	acl_enable = 0;
	DBusError 		err;
	
	syslog_ax_acl_dbg("Entering config acl!\n");

	if(!((productinfo.capbmp) & FUNCTION_ACL_VLAUE)){
		npd_syslog_dbg("do not support acl!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_ACL_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ACL_VLAUE)){
		npd_syslog_dbg("do not support acl!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else{
		dbus_error_init(&err);

		if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_BYTE,&acl_enable,
									DBUS_TYPE_INVALID))) {
			syslog_ax_acl_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				syslog_ax_acl_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}	
		ret=ACL_RETURN_CODE_ERROR;
		if(((ACL_TRUE == g_acl_enable) && ( acl_enable ))||
			((ACL_FALSE == g_acl_enable) && ( !acl_enable)))
			ret=ACL_RETURN_CODE_ERROR;
		else {
			g_acl_enable = acl_enable;		
			ret = nam_acl_service(g_acl_enable);
					if(ret != 0) {
				syslog_ax_acl_err("npd_acl>>npd_dbus_config_acl::cpssDxChAclEnable ret %d\n",ret);
				ret=ACL_RETURN_CODE_ERROR;
			}
			else {
				ret = ACL_RETURN_CODE_SUCCESS;
			}
			ret = npd_acl_service_enable(g_acl_enable, 0);
			if(ret != 0) {
				syslog_ax_acl_err("npd_acl>>npd_dbus_config_acl::cpssDxChAclEnable ret %d\n",ret);
				ret=ACL_RETURN_CODE_ERROR;
			}
			else {
				ret = ACL_RETURN_CODE_SUCCESS;
			}
		}
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	return reply;
}

DBusMessage * npd_dbus_show_acl_service(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage* 	reply = NULL;
	DBusMessageIter	iter;
	DBusError 		err;
	unsigned int	Isable = 0;
	syslog_ax_acl_dbg("show access-list service!\n");

	dbus_error_init(&err);
	if(g_acl_enable==ACL_TRUE) {
	   	syslog_ax_acl_err("acl service is enabled");
	}
	else {
		syslog_ax_acl_dbg("acl service is disabled");
	}
	Isable = g_acl_enable;
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&Isable);
	return reply;	
}


DBusMessage * npd_dbus_config_trap_to_cpu_ip(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage* 	reply = NULL;
	DBusMessageIter	iter;
	DBusError 		err;
	unsigned long   ruleIndex = 0;
	unsigned int   sipmaskLen = 0,dipmaskLen = 0;
      unsigned long   dipno = 0,sipno = 0,sipmask = 0,dipmask = 0;
	unsigned int    ruleType = 0,rs=ACL_RETURN_CODE_ERROR;	
	struct ACCESS_LIST_STC *store = NULL ;

	if(!((productinfo.capbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_ACL_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		store=(struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
		if(NULL == store) 
		{
			rs=ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
		}
		else 
		{
			memset(store,0,sizeof(struct ACCESS_LIST_STC));
			dbus_error_init(&err);
			if (!(dbus_message_get_args ( msg, &err,
										DBUS_TYPE_UINT32,&ruleIndex,
										DBUS_TYPE_UINT32,&ruleType,
										DBUS_TYPE_UINT32,&sipmaskLen,
										DBUS_TYPE_UINT32,&dipmaskLen,
										DBUS_TYPE_UINT32,&dipno,
										DBUS_TYPE_UINT32,&sipno,
										DBUS_TYPE_INVALID))) 
			{
				syslog_ax_acl_err("Unable to get input args ");
				if (dbus_error_is_set(&err)) 
				{
					syslog_ax_acl_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				return NULL;
			}
			npd_acl_len2mask(sipmaskLen,&sipmask);
			npd_acl_len2mask(dipmaskLen,&dipmask);
			
			store->ruleIndex=ruleIndex;
			if(0==ruleType)
				store->ruleType = STANDARD_ACL_RULE;
			else if(1==ruleType)
				store->ruleType = EXTENDED_ACL_RULE;
			store->ActionPtr.actionType=ACL_ACTION_TYPE_TRAP_TO_CPU; 
			if (ruleType == 0)
				store->TcamPtr.ruleFormat=ACL_RULE_STD_IPv4_L4;
			else if (ruleType == 1)
				store->TcamPtr.ruleFormat=ACL_RULE_EXT_IPv4_L4;
			store->TcamPtr.packetType=ACL_TCAM_RULE_PACKETTYPE_IP;
			store->TcamPtr.ruleInfo.stdIpv4L4.rule.Dip=dipno;
			store->TcamPtr.ruleInfo.stdIpv4L4.rule.Sip=sipno;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDip=dipmask;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSip=sipmask;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskLenDip=dipmaskLen;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskLenSip=sipmaskLen;
			store->ActionPtr.actionInfo.trap.cpucode = 500;
			
			rs=npd_acl_add(store);/*add rule */	
			if(ACL_RETURN_CODE_SUCCESS!=rs) 
			{
				free(store);
			}
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&rs);
	return reply;	

}

DBusMessage * npd_dbus_config_trap_to_cpu_tcp(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage* 	reply = NULL;
	DBusMessageIter	iter;
	DBusError 		err;
	unsigned long   ruleIndex = 0;
	unsigned int   sipmaskLen = 0, dipmaskLen = 0;
	unsigned long   dipno = 0, sipno = 0,sipmask = 0,dipmask = 0;
	unsigned long   dstport = 0,srcport = 0,masksrcport = 0,maskdstport = 0;
	unsigned int    ruleType = 0,rs=ACL_RETURN_CODE_ERROR;	

	struct ACCESS_LIST_STC *store = NULL;

	if(!((productinfo.capbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_ACL_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		store = (struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
		if(NULL == store) 
		{
			rs=ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
		}
		else 
		{
			memset(store,0,sizeof(struct ACCESS_LIST_STC));
			dbus_error_init(&err);

			if (!(dbus_message_get_args ( msg, &err,
										  DBUS_TYPE_UINT32,&ruleIndex,
										  DBUS_TYPE_UINT32,&ruleType,
									      DBUS_TYPE_UINT32,&sipmaskLen,
									      DBUS_TYPE_UINT32,&dipmaskLen,
									      DBUS_TYPE_UINT32,&dipno,
									      DBUS_TYPE_UINT32,&sipno,
									      DBUS_TYPE_UINT32,&dstport,
									      DBUS_TYPE_UINT32,&srcport,
									      DBUS_TYPE_INVALID))) 
			{
				syslog_ax_acl_err("Unable to get input args ");
				if (dbus_error_is_set(&err)) 
				{
					syslog_ax_acl_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				return NULL;
			}
				
			npd_acl_len2mask(sipmaskLen,&sipmask);
			npd_acl_len2mask(dipmaskLen,&dipmask);
			
			if(ACL_ANY_PORT==srcport) 
			{
				srcport = 0;
			    masksrcport=0;
			}
			else 
			{
				masksrcport=65535;
			}
			if(ACL_ANY_PORT==dstport) 
			{
				dstport = 0;
			    maskdstport=0;
			}
			else 
			{
				maskdstport=65535;
			}
			store->ruleIndex=ruleIndex;
			store->ruleType=ruleType;
			store->ActionPtr.actionType=ACL_ACTION_TYPE_TRAP_TO_CPU;
			store->ActionPtr.actionInfo.trap.cpucode = 500;
			if (ruleType == 0) 
			{
				store->TcamPtr.ruleFormat=ACL_RULE_STD_IPv4_L4;
			}
			else if (ruleType == 1) 
			{
				store->TcamPtr.ruleFormat=ACL_RULE_EXT_IPv4_L4;
			}
			store->TcamPtr.packetType=ACL_TCAM_RULE_PACKETTYPE_TCP;
			store->TcamPtr.ruleInfo.stdIpv4L4.rule.Dip=dipno;
			store->TcamPtr.ruleInfo.stdIpv4L4.rule.Sip=sipno;
			store->TcamPtr.ruleInfo.stdIpv4L4.rule.DstPort=dstport;
			store->TcamPtr.ruleInfo.stdIpv4L4.rule.SrcPort=srcport;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDip=dipmask;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSip=sipmask;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskLenDip=dipmaskLen;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskLenSip=sipmaskLen;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSrcPort=masksrcport;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDstPort=maskdstport;
		
			rs=npd_acl_add(store);	/*add rule */
			if(ACL_RETURN_CODE_SUCCESS!=rs) 
			{
				free(store);
			}
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&rs);
	return reply;	
}

DBusMessage * npd_dbus_config_trap_to_cpu_udp(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage* 	reply = NULL;
	DBusMessageIter	iter;
	DBusError 		err;
	unsigned long   ruleIndex = 0;
	unsigned int    sipmaskLen = 0,dipmaskLen = 0;
	unsigned long   dipno = 0, sipno = 0,sipmask = 0,dipmask = 0;
	unsigned long   dstport = 0,srcport = 0,masksrcport = 0,maskdstport = 0;
	unsigned int    ruleType = 0,rs=ACL_RETURN_CODE_ERROR;	

	if(!((productinfo.capbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_ACL_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		struct ACCESS_LIST_STC *store = NULL;
		store=(struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
		if(NULL == store) 
		{
			rs = ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
		}
		else 
		{
			memset(store,0,sizeof(struct ACCESS_LIST_STC));
			dbus_error_init(&err);
			if (!(dbus_message_get_args ( msg, &err,
										  DBUS_TYPE_UINT32,&ruleIndex,
										  DBUS_TYPE_UINT32,&ruleType,
									      DBUS_TYPE_UINT32,&sipmaskLen,
									      DBUS_TYPE_UINT32,&dipmaskLen,
									      DBUS_TYPE_UINT32,&dipno,
									      DBUS_TYPE_UINT32,&sipno,
									      DBUS_TYPE_UINT32,&dstport,
									      DBUS_TYPE_UINT32,&srcport,
									      DBUS_TYPE_INVALID))) 
			{
				syslog_ax_acl_err("Unable to get input args ");
				if (dbus_error_is_set(&err)) 
				{
					syslog_ax_acl_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				return NULL;
			}
			npd_acl_len2mask(sipmaskLen,&sipmask);
			npd_acl_len2mask(dipmaskLen,&dipmask);
			
			if(ACL_ANY_PORT==srcport) 
			{
				srcport = 0;
				masksrcport=0;
			}
			else 
			{           
		           masksrcport=65535;
			}
		    if(ACL_ANY_PORT==dstport) 
			{
			  	dstport = 0;
		        maskdstport=0;
		    }
			else 
			{           
		        maskdstport=65535;
			}
			store->ruleIndex=ruleIndex;
			store->ruleType=ruleType;
			store->ActionPtr.actionType=ACL_ACTION_TYPE_TRAP_TO_CPU; 
			store->ActionPtr.actionInfo.trap.cpucode = 500;
			if (ruleType == 0) 
			{
				store->TcamPtr.ruleFormat=ACL_RULE_STD_IPv4_L4;
			}
			else if (ruleType == 1) 
			{
				store->TcamPtr.ruleFormat=ACL_RULE_EXT_IPv4_L4;
			}
			store->TcamPtr.packetType=ACL_TCAM_RULE_PACKETTYPE_UDP;
			store->TcamPtr.ruleInfo.stdIpv4L4.rule.Dip=dipno;
			store->TcamPtr.ruleInfo.stdIpv4L4.rule.Sip=sipno;
			store->TcamPtr.ruleInfo.stdIpv4L4.rule.DstPort=dstport;
			store->TcamPtr.ruleInfo.stdIpv4L4.rule.SrcPort=srcport;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDip=dipmask;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSip=sipmask;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskLenDip=dipmaskLen;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskLenSip=sipmaskLen;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSrcPort=masksrcport;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDstPort=maskdstport;
			
			rs=npd_acl_add(store);  /*add rule*/ 
			if(ACL_RETURN_CODE_SUCCESS != rs) 
			{
				free(store);
			}
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&rs);
	return reply;
	
}

DBusMessage * npd_dbus_config_trap_to_cpu_ethernet(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage* 	reply = NULL;
	DBusMessageIter	iter;
	DBusError 		err;
	unsigned long   ruleIndex = 0;
	ETHERADDR	    smacAddr, dmacAddr;
	unsigned  int   ruleType = 0, i = 0, rs=ACL_RETURN_CODE_ERROR;
	unsigned int    count1=0, count2=0, j=0, ethertype = 0;

	struct ACCESS_LIST_STC *store = NULL;

	if(!((productinfo.capbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_ACL_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		store = (struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
		if(NULL == store)  
		{
			rs = ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
		}
		else 
		{
			memset(store,0,sizeof(struct ACCESS_LIST_STC));
			memset(&smacAddr,0,sizeof(ETHERADDR));
			memset(&dmacAddr,0,sizeof(ETHERADDR));
			
			dbus_error_init(&err);

			if (!(dbus_message_get_args ( msg, &err,
										DBUS_TYPE_UINT32,&ruleIndex,
										DBUS_TYPE_UINT32,&ruleType,
										DBUS_TYPE_UINT32,&ethertype,
										DBUS_TYPE_BYTE,  &dmacAddr.arEther[0],
										DBUS_TYPE_BYTE,  &dmacAddr.arEther[1],
										DBUS_TYPE_BYTE,  &dmacAddr.arEther[2],
										DBUS_TYPE_BYTE,  &dmacAddr.arEther[3],
										DBUS_TYPE_BYTE,  &dmacAddr.arEther[4],
										DBUS_TYPE_BYTE,  &dmacAddr.arEther[5],
										DBUS_TYPE_BYTE,  &smacAddr.arEther[0],
										DBUS_TYPE_BYTE,  &smacAddr.arEther[1],
										DBUS_TYPE_BYTE,  &smacAddr.arEther[2],
										DBUS_TYPE_BYTE,  &smacAddr.arEther[3],
										DBUS_TYPE_BYTE,  &smacAddr.arEther[4],
										DBUS_TYPE_BYTE,  &smacAddr.arEther[5],						     
										DBUS_TYPE_INVALID))) 
			{
				syslog_ax_acl_err("Unable to get input args ");
				if (dbus_error_is_set(&err)) 
				{
					syslog_ax_acl_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				store = NULL;
				return NULL;
			}
			
	    	for(j=0;j<6;j++)
			{
				if(0==dmacAddr.arEther[j])
					count1+=1;
				if(0==smacAddr.arEther[j])
					count2+=1;
			}
					
			/*store acl rule*/
			store->ruleIndex=ruleIndex;
			store->ruleType=ruleType;
			store->ActionPtr.actionType=ACL_ACTION_TYPE_TRAP_TO_CPU; 
			store->ActionPtr.actionInfo.trap.cpucode = 500;
			store->TcamPtr.ruleFormat=ACL_RULE_STD_NOT_IP;
			store->TcamPtr.packetType=ACL_TCAM_RULE_PACKETTYPE_ETHERNET;
			if (ethertype < 0xffff)	
			{
				store->TcamPtr.ruleInfo.stdNotIp.rule.ethertype = ethertype;
				store->TcamPtr.ruleInfo.stdNotIp.mask.maskethertype = 0xffff;
			}

			for(i=0;i<6;i++)
			{
				store->TcamPtr.ruleInfo.stdNotIp.rule.dMac[i]=dmacAddr.arEther[i];
				store->TcamPtr.ruleInfo.stdNotIp.rule.sMac[i]=smacAddr.arEther[i];	
				if(count1!=6) 
				{
					store->TcamPtr.ruleInfo.stdNotIp.mask.maskDMac[i]=0xFF;
				}
				if(count2!=6) {
					store->TcamPtr.ruleInfo.stdNotIp.mask.maskSMac[i]=0xFF;
				}
			}
				
			rs=npd_acl_add_notIp(store); 
			if(ACL_RETURN_CODE_SUCCESS!=rs) 
			{
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				store = NULL;
			}

		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&rs);
	return reply;

}
DBusMessage * npd_dbus_config_trap_to_cpu_icmp(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage* 	reply = NULL;
	DBusMessageIter	iter;
	DBusError 		err;
	unsigned long   ruleIndex = 0, sipmask = 0, dipmask = 0;	  
	unsigned long   dipno = 0, sipno = 0;
	unsigned int    dipmaskLen = 0, sipmaskLen = 0,ruleType = 0,rs=ACL_RETURN_CODE_ERROR;	
	unsigned char   typeno=0, codeno=0, masktype=0, maskcode=0, acl_any = 0;

	struct ACCESS_LIST_STC *store = NULL;

	if(!((productinfo.capbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_ACL_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		store = (struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
		if(NULL == store) 
		{
			rs = ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
		}
		else 
		{
			memset(store, 0, sizeof(struct ACCESS_LIST_STC));
			
			syslog_ax_acl_dbg("Setting ACL Rule as for trap icmp packet to cpu!\n");

			dbus_error_init(&err);

			if (!(dbus_message_get_args ( msg, &err,
										DBUS_TYPE_UINT32,&ruleIndex,
										DBUS_TYPE_UINT32,&ruleType,
										DBUS_TYPE_UINT32,&sipmaskLen,
										DBUS_TYPE_UINT32,&dipmaskLen,
										DBUS_TYPE_UINT32,&dipno,
										DBUS_TYPE_UINT32,&sipno,
										DBUS_TYPE_BYTE,&typeno,
										DBUS_TYPE_BYTE,&codeno,
										DBUS_TYPE_BYTE,&acl_any,
										DBUS_TYPE_INVALID))) 
			{
				syslog_ax_acl_err("Unable to get input args ");
				if (dbus_error_is_set(&err)) 
				{
					syslog_ax_acl_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				return NULL;
			}
			
			npd_acl_len2mask(sipmaskLen,&sipmask);
			npd_acl_len2mask(dipmaskLen,&dipmask);
			
			if(1&acl_any) 
			{
		           masktype=0;
			}
			else 
			{           
		           masktype=255;
			}
		    if(2&acl_any) 
			{
				maskcode=0;
		    }
			else 
			{           
		        maskcode=255;
			}
			store->ruleIndex=ruleIndex;
			store->ruleType=ruleType;
			store->ActionPtr.actionType=ACL_ACTION_TYPE_TRAP_TO_CPU;
			store->ActionPtr.actionInfo.trap.cpucode = 500;
			if (ruleType == 0) 
			{
				store->TcamPtr.ruleFormat=ACL_RULE_STD_IPv4_L4;
			}
			else if (ruleType == 1) 
			{
				store->TcamPtr.ruleFormat=ACL_RULE_EXT_IPv4_L4;
			}
			store->TcamPtr.packetType=ACL_TCAM_RULE_PACKETTYPE_ICMP;
			store->TcamPtr.ruleInfo.stdIpv4L4.rule.Dip=dipno;
			store->TcamPtr.ruleInfo.stdIpv4L4.rule.Sip=sipno;
			store->TcamPtr.ruleInfo.stdIpv4L4.rule.Type=typeno;
			store->TcamPtr.ruleInfo.stdIpv4L4.rule.Code=codeno;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDip=dipmask;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSip=sipmask;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskLenDip=dipmaskLen;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskLenSip=sipmaskLen;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskType=masktype;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskCode=maskcode;
					
			rs=npd_acl_add(store);  /*add rule */
			if(ACL_RETURN_CODE_SUCCESS != rs) 
			{
				free(store);
			}

			
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&rs);
	return reply;
	
}
DBusMessage * npd_dbus_config_trap_to_cpu_arp(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage*	reply = NULL;
	DBusMessageIter iter;
	DBusError		err;
	unsigned long	ruleIndex,ret=ACL_RETURN_CODE_ERROR;
	ETHERADDR		smacAddr,maskMac;
	unsigned char   slot_no=0,port_no=0;
	unsigned  int	ruleType = 0, i = 0, rs=ACL_RETURN_CODE_ERROR, vlanId=0;
	unsigned int    eth_g_index = 0,maskVlanId=0,maskvirPort=0;
	unsigned char	 devNum = 0,virPortNo=0;

	struct ACCESS_LIST_STC *store = NULL;

	if(!((productinfo.capbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_ACL_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		store = (struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
		if(NULL == store) 
		{
			rs = ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
		}
		else 
		{
			memset(store,0,sizeof(struct ACCESS_LIST_STC));
			memset(&smacAddr,0,sizeof(ETHERADDR));
			memset(&maskMac,0,sizeof(ETHERADDR));
			

			dbus_error_init(&err);

			if (!(dbus_message_get_args ( msg, &err,
										DBUS_TYPE_UINT32,&ruleIndex,
										DBUS_TYPE_UINT32,&ruleType,
										DBUS_TYPE_BYTE,  &smacAddr.arEther[0],
										DBUS_TYPE_BYTE,  &smacAddr.arEther[1],
										DBUS_TYPE_BYTE,  &smacAddr.arEther[2],
										DBUS_TYPE_BYTE,  &smacAddr.arEther[3],
										DBUS_TYPE_BYTE,  &smacAddr.arEther[4],
										DBUS_TYPE_BYTE,  &smacAddr.arEther[5],
										DBUS_TYPE_BYTE,  &maskMac.arEther[0],
										DBUS_TYPE_BYTE,  &maskMac.arEther[1],
										DBUS_TYPE_BYTE,  &maskMac.arEther[2],
										DBUS_TYPE_BYTE,  &maskMac.arEther[3],
										DBUS_TYPE_BYTE,  &maskMac.arEther[4],
										DBUS_TYPE_BYTE,  &maskMac.arEther[5],	
										DBUS_TYPE_UINT32,&vlanId,
										DBUS_TYPE_BYTE,  &slot_no,
										DBUS_TYPE_BYTE,  &port_no,  
										DBUS_TYPE_INVALID))) 
			{
				syslog_ax_acl_err("Unable to get input args ");
				if (dbus_error_is_set(&err)) 
				{
					syslog_ax_acl_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				store = NULL;
				return NULL;
			}
			
			if(vlanId!=0) 
			{
				maskVlanId =0xFF;
			}
			
			ret = NPD_DBUS_ERROR_NO_SUCH_PORT;

			if((ACL_ANY_PORT_CHAR == slot_no)&&(ACL_ANY_PORT_CHAR == port_no)) 
			{
				maskvirPort = 0;
				virPortNo = 0;
				ret =ACL_RETURN_CODE_SUCCESS;
			}
			else 
			{
				if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) 
				{
					if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,port_no)) 
					{
						eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
						syslog_ax_acl_dbg("index for %d/%d is %#0x\n",slot_no,port_no,eth_g_index);
						ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortNo);
						if(ret != 0)
						{
							syslog_ax_acl_dbg("Npd_dbus::npd_get_devport_by_global_index::get devport Error.\n");
							ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
						}
						else
						{
							syslog_ax_acl_dbg("npd_get_devport_by_global_index::devNum %d,portNum %d\n",devNum,virPortNo);
							
							ret = ACL_RETURN_CODE_SUCCESS;
							maskvirPort = 0xFF;
							syslog_ax_acl_dbg("Npd_dbus::npd_get_devport_by_global_index::get devport Error.\n");
						}
					}
				}
			}
				
			if(ACL_RETURN_CODE_SUCCESS==ret)
			{
				/*store acl rule*/
				store->ruleIndex=ruleIndex;
				store->ruleType=ruleType;
				store->ActionPtr.actionType=ACL_ACTION_TYPE_TRAP_TO_CPU; 
				store->ActionPtr.actionInfo.trap.cpucode = 500;
				store->TcamPtr.ruleFormat=ACL_RULE_STD_IP_L2_QoS;
				store->TcamPtr.packetType=ACL_TCAM_RULE_PACKETTYPE_ARP;

				for(i=0;i<6;i++)
				{
					store->TcamPtr.ruleInfo.stdNotIp.rule.sMac[i]=smacAddr.arEther[i]; 
					store->TcamPtr.ruleInfo.stdNotIp.mask.maskSMac[i]=maskMac.arEther[i];
				}
				
				store->TcamPtr.ruleInfo.stdNotIp.rule.vlanId = vlanId;
				store->TcamPtr.ruleInfo.stdNotIp.mask.maskVlanId = maskVlanId;		
		   	 	store->TcamPtr.ruleInfo.stdNotIp.rule.portno = virPortNo;
				store->TcamPtr.ruleInfo.stdNotIp.mask.maskPortno = maskvirPort;
				store->TcamPtr.ruleInfo.stdNotIp.rule.modid = devNum;		
			/*add notIp rule*/			
			    rs=npd_acl_add_notIp(store);  
				if(ACL_RETURN_CODE_SUCCESS!=rs) 
				{
					/*code optimize:  Resource leak
					zhangcl@autelan.com 2013-1-17*/
					free(store);
					store = NULL;
				}
				
			}
			else
			{
				free(store);
			}
		}	
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
								   DBUS_TYPE_UINT32,
								   &rs);
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,
									&ret);

	return reply;

}

DBusMessage * npd_dbus_config_permit_deny_ip(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage*	reply = NULL;
	DBusMessageIter iter;
	DBusError		err;
	unsigned long	ruleIndex = 0;
	unsigned int	sipmaskLen = 0,dipmaskLen = 0;
	unsigned long	dipno = 0,sipno = 0,sipmask = 0,dipmask = 0;
	unsigned int    actionType = 0;
	unsigned int	ruleType = 0, rs=ACL_RETURN_CODE_ERROR, ret=ACL_RETURN_CODE_ERROR;	
	unsigned int    policer = 0, policerId = 0;
	struct ACCESS_LIST_STC *store = NULL ;
	
	if(!((productinfo.capbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_ACL_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		npd_syslog_dbg("acl support!\n");
		store = (struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
		if(NULL== store)  
		{
			rs = ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
		}
		else
		{
			memset(store,0,sizeof(struct ACCESS_LIST_STC));
			dbus_error_init(&err);

			if (!(dbus_message_get_args ( msg, &err,
										  DBUS_TYPE_UINT32,&ruleIndex,
										  DBUS_TYPE_UINT32,&ruleType,
										  DBUS_TYPE_UINT32,&actionType,
										  DBUS_TYPE_UINT32,&sipmaskLen,
										  DBUS_TYPE_UINT32,&dipmaskLen,
										  DBUS_TYPE_UINT32,&dipno,
										  DBUS_TYPE_UINT32,&sipno,
										  DBUS_TYPE_UINT32,&policer,
										  DBUS_TYPE_UINT32,&policerId,
										  DBUS_TYPE_INVALID))) 
			{
				syslog_ax_acl_err("Unable to get input args ");
				if (dbus_error_is_set(&err)) 
				{
					syslog_ax_acl_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				store = NULL;
				return NULL;
			}
			npd_acl_len2mask(sipmaskLen,&sipmask);
			npd_acl_len2mask(dipmaskLen,&dipmask);
			
			
			store->ruleIndex=ruleIndex;
			store->ruleType=ruleType;
			if(0==actionType)
			{
		        store->ActionPtr.actionType=ACL_ACTION_TYPE_PERMIT;	
				store->ActionPtr.policerPtr.policerEnable = policer;
				store->ActionPtr.policerPtr.policerId = policerId;
			}
			else if(1==actionType) 
			{
				store->ActionPtr.actionType=ACL_ACTION_TYPE_DENY;
			}
			if (ruleType == 0) 
			{
				store->TcamPtr.ruleFormat=ACL_RULE_STD_IPv4_L4;
			}
			else if (ruleType == 1) 
			{
				store->TcamPtr.ruleFormat=ACL_RULE_EXT_IPv4_L4;
			}
			store->TcamPtr.packetType=ACL_TCAM_RULE_PACKETTYPE_IP;
			store->TcamPtr.ruleInfo.stdIpv4L4.rule.Dip=dipno;
			store->TcamPtr.ruleInfo.stdIpv4L4.rule.Sip=sipno;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDip=dipmask;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSip=sipmask;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskLenDip=dipmaskLen;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskLenSip=sipmaskLen;
			
			npd_syslog_dbg("dstipdata %x\n", dipno);
			npd_syslog_dbg("dstipmask %x\n", dipmask);
			npd_syslog_dbg("srcipdata %x\n", sipno);
			npd_syslog_dbg("srcipmask %x\n", sipmask);

			if(policer==1)
			{
				ret=npd_acl_policer_id_check(policerId);
				if(ret!=ACL_RETURN_CODE_SUCCESS) 
				{
					syslog_ax_acl_err("policer id not existed!\n");
					rs=ACL_RETURN_CODE_POLICER_ID_NOT_SET;
					free(store);
				}
				else 
				{
					rs=npd_acl_add(store);/*add rule*/	
					if(ACL_RETURN_CODE_SUCCESS!=rs) 
					{	
						free(store);
					}
				}
			}
			else if(policer==0)
			{
				rs=npd_acl_add(store);/*add rule*/	
				if(ACL_RETURN_CODE_SUCCESS!=rs)
				{
					free(store);
				}
			}
			else
			{
				free(store);
			}
		}	
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&rs);
	return reply;	

}

DBusMessage * npd_dbus_config_permit_deny_ipv6(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage*	reply = NULL;
	DBusMessageIter iter;
	DBusError		err;
	unsigned long	ruleIndex = 0;
	unsigned int	sipmaskLen = 0,dipmaskLen = 0;
	unsigned int    actionType = 0, nextheader = 0;
	unsigned int	ruleType = 0, rs=ACL_RETURN_CODE_ERROR, ret=ACL_RETURN_CODE_ERROR;	
	unsigned int    policer = 0, policerId = 0;
	struct ipv6addr dip, sip;
	struct ACCESS_LIST_STC *store = NULL ;

	
	memset(&dip, 0, sizeof(struct ipv6addr));
	memset(&sip, 0, sizeof(struct ipv6addr));

	store = (struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
	if(NULL== store)  
	{
		rs = ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
	}
	else
	{
		memset(store,0,sizeof(struct ACCESS_LIST_STC));
		dbus_error_init(&err);

		if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT32,&ruleIndex,
									DBUS_TYPE_UINT32,&ruleType,
									DBUS_TYPE_UINT32,&actionType,	
									DBUS_TYPE_UINT32, &nextheader,
									DBUS_TYPE_UINT32,&sipmaskLen,
									DBUS_TYPE_UINT32,&dipmaskLen,
									DBUS_TYPE_BYTE, &(dip.ipbuf[0]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[1]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[2]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[3]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[4]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[5]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[6]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[7]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[8]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[9]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[10]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[11]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[12]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[13]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[14]),							
									DBUS_TYPE_BYTE, &(dip.ipbuf[15]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[0]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[1]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[2]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[3]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[4]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[5]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[6]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[7]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[8]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[9]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[10]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[11]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[12]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[13]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[14]),							
									DBUS_TYPE_BYTE, &(sip.ipbuf[15]),
									DBUS_TYPE_UINT32,&policer,
									DBUS_TYPE_UINT32,&policerId,
									DBUS_TYPE_INVALID))) 
		{
			syslog_ax_acl_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) 
			{
				syslog_ax_acl_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			/*code optimize:  Resource leak
			zhangcl@autelan.com 2013-1-17*/
			free(store);
			store = NULL;
			return NULL;
		}		
		
		store->ruleIndex=ruleIndex;
		store->ruleType=ruleType;
		if(0==actionType)
		{	
	        store->ActionPtr.actionType=ACL_ACTION_TYPE_PERMIT;	
			store->ActionPtr.policerPtr.policerEnable = policer;
			store->ActionPtr.policerPtr.policerId = policerId;
			
			npd_syslog_dbg("policer ---> %x\n", policer);
			npd_syslog_dbg("policerId ---> %x\n", policerId);
		}
		else if(1==actionType) 
		{	
			store->ActionPtr.actionType=ACL_ACTION_TYPE_DENY;
		}
		else if(2 == actionType)
		{	
			store->ActionPtr.actionType=ACL_ACTION_TYPE_TRAP_TO_CPU;
			store->ActionPtr.actionInfo.trap.cpucode = 500;
		}
	
		store->TcamPtr.ruleFormat = ACL_RULE_EXT_IPv6_L4;
		store->TcamPtr.packetType = ACL_TCAM_RULE_PACKETTYPE_IPV6;
		store->TcamPtr.nextheader = nextheader;
		if (dipmaskLen) 
		{
			store->TcamPtr.ruleInfo.extIpv6L4.rule.Dip=dip;			
			memset(&(store->TcamPtr.ruleInfo.extIpv6L4.mask.maskDip), 0xff, 16);
			store->TcamPtr.ruleInfo.extIpv6L4.mask.maskLenDip=16;
		}
		else 
		{
			store->TcamPtr.ruleInfo.extIpv6L4.rule.Dip=dip;			
			memset(&(store->TcamPtr.ruleInfo.extIpv6L4.mask.maskDip), 0, 16);
			store->TcamPtr.ruleInfo.extIpv6L4.mask.maskLenDip=0;
		}

		if (sipmaskLen) 
		{
			store->TcamPtr.ruleInfo.extIpv6L4.rule.Sip=sip;			
			memset(&(store->TcamPtr.ruleInfo.extIpv6L4.mask.maskSip), 0xff, 16);
			store->TcamPtr.ruleInfo.extIpv6L4.mask.maskLenSip=16;
		}
		else 
		{
			store->TcamPtr.ruleInfo.extIpv6L4.rule.Sip=sip;			
			memset(&(store->TcamPtr.ruleInfo.extIpv6L4.mask.maskSip), 0, 16);
			store->TcamPtr.ruleInfo.extIpv6L4.mask.maskLenSip=0;
		}
	
		if(policer==1)
		{
			ret=npd_acl_policer_id_check(policerId);
			if(ret!=ACL_RETURN_CODE_SUCCESS) 
			{
				syslog_ax_acl_err("policer id not existed!\n");
				rs=ACL_RETURN_CODE_POLICER_ID_NOT_SET;
				free(store);
			}
			else 
			{
				rs=npd_acl_add(store);/*add rule*/	
				if(ACL_RETURN_CODE_SUCCESS!=rs) 
				{	
					free(store);
				}
			}
		}
		else if(policer==0) 
		{
			rs=npd_acl_add(store);/*add rule*/	
			if(ACL_RETURN_CODE_SUCCESS!=rs)
			{
				free(store);
			}
		}
		else
		{
			/*code optimize:  Resource leak
			zhangcl@autelan.com 2013-1-17*/
			free(store);
			store = NULL;
		}
	}	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&rs);
	return reply;	

}


/***********************************************************************************************
 *  npd_dbus_config_redirect_ipv6
 *		set ipv6 redirect operation
 *
 * 	INPUT:
 *		DBusConnection *conn, 
 *		DBusMessage *msg, 
 *		void *user_data
 *
 *	OUTPUT:
 * 		NULL
 *
 *	RETURN:
 * 	 	reply
 *		DHCP_SNP_RETURN_CODE_OK
 * 	 	DHCP_SNP_RETURN_CODE_ERROR
 **********************************************************************************************/
DBusMessage * npd_dbus_config_redirect_ipv6(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage*	reply = NULL;
	DBusMessageIter iter;
	DBusError		err;
	unsigned long	ruleIndex = 0;
	unsigned int	sipmaskLen = 0,dipmaskLen = 0, nextheader = 0;
	unsigned int    actionType = 0, eth_g_index=0;;
	unsigned int	ruleType = 0, rs=ACL_RETURN_CODE_ERROR, ret=ACL_RETURN_CODE_ERROR, tmp;	
	struct ipv6addr dip, sip;
	struct ACCESS_LIST_STC *store = NULL ;
	unsigned char   mslot=0,mport=0, devNum = 0,virPortId = 0;	

	memset(&dip, 0, sizeof(struct ipv6addr));
	memset(&sip, 0, sizeof(struct ipv6addr));
	
	store = (struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
	if(NULL== store)  
	{		
		rs = ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
	}
	else
	{
		memset(store,0,sizeof(struct ACCESS_LIST_STC));
		dbus_error_init(&err);

		if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT32, &ruleIndex,				  
									DBUS_TYPE_UINT32, &ruleType,
									DBUS_TYPE_UINT32, &actionType,
									DBUS_TYPE_UINT32, &nextheader,
									DBUS_TYPE_BYTE,  &mslot,
									DBUS_TYPE_BYTE,  &mport,
									DBUS_TYPE_UINT32, &sipmaskLen,
									DBUS_TYPE_UINT32, &dipmaskLen,
									DBUS_TYPE_BYTE, &(dip.ipbuf[0]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[1]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[2]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[3]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[4]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[5]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[6]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[7]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[8]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[9]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[10]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[11]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[12]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[13]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[14]),							
									DBUS_TYPE_BYTE, &(dip.ipbuf[15]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[0]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[1]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[2]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[3]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[4]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[5]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[6]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[7]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[8]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[9]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[10]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[11]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[12]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[13]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[14]),							
									DBUS_TYPE_BYTE, &(sip.ipbuf[15]),									
									DBUS_TYPE_INVALID))) 
		{
			syslog_ax_acl_err("ipv6_Npd_acl::Unable to get input args in npd_dbus_config_redirect_ipv6");
			if (dbus_error_is_set(&err)) 
			{
				syslog_ax_acl_err("ipv6_Npd_acl:: %s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			/*code optimize:  Resource leak
			zhangcl@autelan.com 2013-1-17*/
			free(store);
			store = NULL;
			return NULL;
		}	

		tmp = NPD_DBUS_ERROR_NO_SUCH_PORT;		
		if (CHASSIS_SLOTNO_ISLEGAL(mslot)) 
		{
			if (ETH_LOCAL_PORTNO_ISLEGAL(mslot,mport)) 
			{
				eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(mslot,mport);
				syslog_ax_acl_dbg("index for %d/%d is %#0x\n",mslot,mport,eth_g_index);			
				tmp = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortId);
				if(tmp != 0)
				{
					syslog_ax_acl_dbg("ipv6_Npd_acl::npd_get_devport_by_global_index::get devport Error.\n");
					tmp = NPD_DBUS_ERROR_NO_SUCH_PORT;
					syslog_ax_acl_dbg("ipv6_Npd_acl::npd_get_devport_by_global_index::get devport Error.\n");
				}
				else 
				{
					syslog_ax_acl_dbg("ipv6_Npd_acl::npd_get_devport_by_global_index::devNum %d,portNum %d\n",devNum,virPortId); 			
					tmp=ACL_RETURN_CODE_SUCCESS;
				}
			}
		}	
		if(ACL_RETURN_CODE_SUCCESS == tmp)
		{
		  	store->ruleIndex=ruleIndex;
			store->ruleType=ruleType;
			if(4==actionType)
			{
				store->ActionPtr.actionType=ACL_ACTION_TYPE_REDIRECT;	
				store->ActionPtr.actionInfo.redirect.portNum = virPortId;
				store->ActionPtr.actionInfo.redirect.modid = devNum;				 
			}
			store->TcamPtr.ruleFormat = ACL_RULE_EXT_IPv6_L4;
			store->TcamPtr.packetType = ACL_TCAM_RULE_PACKETTYPE_IPV6;
			store->TcamPtr.nextheader = nextheader;
			if (dipmaskLen) 
			{
				store->TcamPtr.ruleInfo.extIpv6L4.rule.Dip=dip;			
				memset(&(store->TcamPtr.ruleInfo.extIpv6L4.mask.maskDip), 0xff, 16);
				store->TcamPtr.ruleInfo.extIpv6L4.mask.maskLenDip=16;
			}
			else 
			{
				store->TcamPtr.ruleInfo.extIpv6L4.rule.Dip=dip;			
				memset(&(store->TcamPtr.ruleInfo.extIpv6L4.mask.maskDip), 0, 16);
				store->TcamPtr.ruleInfo.extIpv6L4.mask.maskLenDip=0;
			}

			if (sipmaskLen)
			{
				store->TcamPtr.ruleInfo.extIpv6L4.rule.Sip=sip;			
				memset(&(store->TcamPtr.ruleInfo.extIpv6L4.mask.maskSip), 0xff, 16);
				store->TcamPtr.ruleInfo.extIpv6L4.mask.maskLenSip=16;
			}
			else 
			{
				store->TcamPtr.ruleInfo.extIpv6L4.rule.Sip=sip;			
				memset(&(store->TcamPtr.ruleInfo.extIpv6L4.mask.maskSip), 0, 16);
				store->TcamPtr.ruleInfo.extIpv6L4.mask.maskLenSip=0;
			}			
				
			rs=npd_acl_add(store);/*add rule*/	
			if(ACL_RETURN_CODE_SUCCESS!=rs)
			{
				free(store);
			}	
		}	
		else
		{
			/*code optimize:  Resource leak
			zhangcl@autelan.com 2013-1-17*/
			free(store);
			store = NULL;
		}
	}	
	reply = dbus_message_new_method_return(msg);	
	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&rs);
	return reply;	
}


/***********************************************************************************************
 *  npd_dbus_config_redirect_tcp_udp_ipv6
 *		set ipv6 redirect operation
 *
 * 	INPUT:
 *		DBusConnection *conn, 
 *		DBusMessage *msg, 
 *		void *user_data
 *
 *	OUTPUT:
 * 		NULL
 *
 *	RETURN:
 * 	 	reply
 *		DHCP_SNP_RETURN_CODE_OK
 * 	 	DHCP_SNP_RETURN_CODE_ERROR
 **********************************************************************************************/
DBusMessage * npd_dbus_config_redirect_tcp_udp_ipv6(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage*	reply = NULL;
	DBusMessageIter iter;
	DBusError		err;
	unsigned long	ruleIndex = 0;
	unsigned int	sipmaskLen = 0,dipmaskLen = 0, nextheader = 0, packetType;
	unsigned int	actionType = 0, eth_g_index=0,  srcport=0,dstport=0;
	unsigned int   masksrcport=0,maskdstport=0;
	unsigned int	ruleType = 0, rs=ACL_RETURN_CODE_ERROR, ret=ACL_RETURN_CODE_ERROR, tmp; 
	struct ipv6addr dip, sip;
	struct ACCESS_LIST_STC *store = NULL ;
	unsigned char	mslot=0,mport=0, devNum = 0,virPortId = 0;	

	memset(&dip, 0, sizeof(struct ipv6addr));
	memset(&sip, 0, sizeof(struct ipv6addr));
	
	store = (struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
	if(NULL== store)  
	{ 	
		rs = ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
	}
	else
	{
		memset(store,0,sizeof(struct ACCESS_LIST_STC));
		dbus_error_init(&err);

		if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT32, &ruleIndex,				  
									DBUS_TYPE_UINT32, &ruleType,
									DBUS_TYPE_UINT32, &actionType,
									DBUS_TYPE_UINT32, &packetType,
									DBUS_TYPE_BYTE,  &mslot,
									DBUS_TYPE_BYTE,  &mport,
									DBUS_TYPE_UINT32, &sipmaskLen,
									DBUS_TYPE_UINT32, &dipmaskLen,
									DBUS_TYPE_UINT32, &dstport,
									DBUS_TYPE_UINT32, &srcport,
									DBUS_TYPE_BYTE, &(dip.ipbuf[0]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[1]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[2]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[3]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[4]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[5]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[6]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[7]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[8]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[9]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[10]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[11]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[12]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[13]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[14]),							
									DBUS_TYPE_BYTE, &(dip.ipbuf[15]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[0]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[1]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[2]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[3]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[4]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[5]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[6]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[7]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[8]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[9]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[10]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[11]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[12]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[13]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[14]),							
									DBUS_TYPE_BYTE, &(sip.ipbuf[15]),									
									DBUS_TYPE_INVALID))) 
		{
			syslog_ax_acl_err("ipv6_Npd_acl::Unable to get input args in npd_dbus_config_redirect_ipv6");
			if (dbus_error_is_set(&err)) 
			{
				syslog_ax_acl_err("ipv6_Npd_acl:: %s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}			
			/*code optimize:  Resource leak
			zhangcl@autelan.com 2013-1-17*/
			free(store);
			store = NULL;
			return NULL;
		}	

		tmp = NPD_DBUS_ERROR_NO_SUCH_PORT;		
		if (CHASSIS_SLOTNO_ISLEGAL(mslot)) 
		{
			if (ETH_LOCAL_PORTNO_ISLEGAL(mslot,mport)) 
			{
				eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(mslot,mport);
				syslog_ax_acl_dbg("index for %d/%d is %#0x\n",mslot,mport,eth_g_index); 		
				tmp = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortId);
				if(tmp != 0)
				{
					syslog_ax_acl_dbg("ipv6_Npd_acl::npd_get_devport_by_global_index::get devport Error.\n");
					tmp = NPD_DBUS_ERROR_NO_SUCH_PORT;
					syslog_ax_acl_dbg("ipv6_Npd_acl::npd_get_devport_by_global_index::get devport Error.\n");
				}
				else 
				{
					syslog_ax_acl_dbg("ipv6_Npd_acl::npd_get_devport_by_global_index::devNum %d,portNum %d\n",devNum,virPortId);			
					tmp=ACL_RETURN_CODE_SUCCESS;
				}
			}
		 }		 
		 if(ACL_RETURN_CODE_SUCCESS == tmp)
		 {
			store->ruleIndex=ruleIndex;
			store->ruleType=ruleType;
			if(4==actionType)
			{
				store->ActionPtr.actionType=ACL_ACTION_TYPE_REDIRECT;	
				store->ActionPtr.actionInfo.redirect.portNum = virPortId;
				store->ActionPtr.actionInfo.redirect.modid = devNum;				 
			}
			store->TcamPtr.ruleFormat = ACL_RULE_EXT_IPv6_L4;
			store->TcamPtr.packetType = ACL_TCAM_RULE_PACKETTYPE_TCPV6;
			if(1==packetType) 
			{
				store->TcamPtr.nextheader=17;
			}
			else if(2==packetType) 
			{
				store->TcamPtr.nextheader=6;
			}			
			if (dipmaskLen) 
			{
				store->TcamPtr.ruleInfo.extIpv6L4.rule.Dip=dip; 		
				memset(&(store->TcamPtr.ruleInfo.extIpv6L4.mask.maskDip), 0xff, 16);
				store->TcamPtr.ruleInfo.extIpv6L4.mask.maskLenDip=16;
			}
			else 
			{
				store->TcamPtr.ruleInfo.extIpv6L4.rule.Dip=dip; 		
				memset(&(store->TcamPtr.ruleInfo.extIpv6L4.mask.maskDip), 0, 16);
				store->TcamPtr.ruleInfo.extIpv6L4.mask.maskLenDip=0;
			}

			if (sipmaskLen) 
			{
				store->TcamPtr.ruleInfo.extIpv6L4.rule.Sip=sip; 		
				memset(&(store->TcamPtr.ruleInfo.extIpv6L4.mask.maskSip), 0xff, 16);
				store->TcamPtr.ruleInfo.extIpv6L4.mask.maskLenSip=16;
			}
			else 
			{
				store->TcamPtr.ruleInfo.extIpv6L4.rule.Sip=sip; 		
				memset(&(store->TcamPtr.ruleInfo.extIpv6L4.mask.maskSip), 0, 16);
				store->TcamPtr.ruleInfo.extIpv6L4.mask.maskLenSip=0;
			}			

			if(ACL_ANY_PORT==srcport) 
			{
				srcport = 0;
				masksrcport = 0;
			}
			else 
			{          
				masksrcport=65535;
			}

			if(ACL_ANY_PORT==dstport) 
			{
				dstport = 0;
				maskdstport=0;
			} 
			else 
			{          
				maskdstport=65535;
			}

			store->TcamPtr.ruleInfo.extIpv6L4.rule.DstPort=dstport;
       		store->TcamPtr.ruleInfo.extIpv6L4.rule.SrcPort=srcport;
       		store->TcamPtr.ruleInfo.extIpv6L4.mask.maskSrcPort=masksrcport;
       		store->TcamPtr.ruleInfo.extIpv6L4.mask.maskDstPort=maskdstport;
			
			rs=npd_acl_add(store);/*add rule*/	
			if(ACL_RETURN_CODE_SUCCESS!=rs)
			{
				free(store);
			}
		} 	
		else
		{
			/*code optimize:  Resource leak
			zhangcl@autelan.com 2013-1-17*/
			free(store);
			store = NULL;
		}
	}	
	reply = dbus_message_new_method_return(msg);	
	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&rs);
	return reply;	
}


DBusMessage * npd_dbus_config_ext_permit_deny_ipv6(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage* 	reply = NULL;
	DBusMessageIter	iter;
	DBusError 		err;
	unsigned int   dstport=0,srcport=0,masksrcport=0,maskdstport=0;
	unsigned int    actionType = 0,ruleType = 0,packetType = 0,sipmaskLen=0,dipmaskLen=0;
	unsigned int    ruleIndex=0, ret = ACL_RETURN_CODE_ERROR, rs = ACL_RETURN_CODE_ERROR;	
	unsigned int    policer=0,policerId=0;
	struct ipv6addr dip, sip;
	struct ACCESS_LIST_STC *store = NULL ;

	memset(&dip, 0, sizeof(struct ipv6addr));
	memset(&sip, 0, sizeof(struct ipv6addr));

	store=(struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
	if(store==NULL) {
		rs=ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
	}
	else 
	{
		memset(store,0,sizeof(struct  ACCESS_LIST_STC)); 
				
		dbus_error_init(&err);

		if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT32,&ruleIndex,	
									DBUS_TYPE_UINT32,&ruleType,
									DBUS_TYPE_UINT32,&actionType,
									DBUS_TYPE_UINT32,&packetType,
									DBUS_TYPE_BYTE, &(dip.ipbuf[0]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[1]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[2]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[3]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[4]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[5]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[6]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[7]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[8]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[9]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[10]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[11]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[12]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[13]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[14]),
									DBUS_TYPE_BYTE, &(dip.ipbuf[15]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[0]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[1]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[2]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[3]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[4]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[5]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[6]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[7]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[8]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[9]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[10]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[11]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[12]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[13]),
									DBUS_TYPE_BYTE, &(sip.ipbuf[14]),							
									DBUS_TYPE_BYTE, &(sip.ipbuf[15]),
									DBUS_TYPE_UINT32,&dipmaskLen,
									DBUS_TYPE_UINT32,&sipmaskLen,
									DBUS_TYPE_UINT32,&dstport,
									DBUS_TYPE_UINT32,&srcport,
									DBUS_TYPE_UINT32,&policer,
									DBUS_TYPE_UINT32,&policerId,
									DBUS_TYPE_INVALID))) {
			syslog_ax_acl_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				syslog_ax_acl_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			free(store);
			store = NULL;
			/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
			return NULL;
		}
		
		if(ACL_ANY_PORT==srcport) {
			srcport = 0;
			masksrcport = 0;
		}
		else {          
			masksrcport=65535;
		}
		
		if(ACL_ANY_PORT==dstport) {
			dstport = 0;
			maskdstport=0;
		} 
		else {          
			maskdstport=65535;
		}
		
		if(0==actionType) {
			store->ActionPtr.actionType=ACL_ACTION_TYPE_PERMIT;
			store->ActionPtr.policerPtr.policerEnable = policer;
			store->ActionPtr.policerPtr.policerId = policerId;
			
			npd_syslog_dbg("policer ---> %x\n", policer);
			npd_syslog_dbg("policerId ---> %x\n", policerId);
		}
		else if(1==actionType) {
			store->ActionPtr.actionType=ACL_ACTION_TYPE_DENY;
		}
		else if(2==actionType){
			store->ActionPtr.actionType= ACL_ACTION_TYPE_TRAP_TO_CPU;
			store->ActionPtr.actionInfo.trap.cpucode = 500;
		}
		
		store->TcamPtr.packetType = ACL_TCAM_RULE_PACKETTYPE_TCPV6;

		if(1==packetType) {
			store->TcamPtr.nextheader=17;
		}
		else if(2==packetType) {
			store->TcamPtr.nextheader=6;
		}

		store->ruleIndex=ruleIndex;
		store->ruleType = EXTENDED_ACL_RULE;  
		store->TcamPtr.ruleFormat=ACL_RULE_EXT_IPv6_L4;

		if (dipmaskLen) {
			store->TcamPtr.ruleInfo.extIpv6L4.rule.Dip=dip;			
			memset(&(store->TcamPtr.ruleInfo.extIpv6L4.mask.maskDip), 0xff, 16);
			store->TcamPtr.ruleInfo.extIpv6L4.mask.maskLenDip=16;
		}
		else {
			store->TcamPtr.ruleInfo.extIpv6L4.rule.Dip=dip;			
			memset(&(store->TcamPtr.ruleInfo.extIpv6L4.mask.maskDip), 0, 16);
			store->TcamPtr.ruleInfo.extIpv6L4.mask.maskLenDip=0;
		}

		if (sipmaskLen) {
			store->TcamPtr.ruleInfo.extIpv6L4.rule.Sip=sip;			
			memset(&(store->TcamPtr.ruleInfo.extIpv6L4.mask.maskSip), 0xff, 16);
			store->TcamPtr.ruleInfo.extIpv6L4.mask.maskLenSip=16;
		}
		else {
			store->TcamPtr.ruleInfo.extIpv6L4.rule.Sip=sip;			
			memset(&(store->TcamPtr.ruleInfo.extIpv6L4.mask.maskSip), 0, 16);
			store->TcamPtr.ruleInfo.extIpv6L4.mask.maskLenSip=0;
		}
		
		store->TcamPtr.ruleInfo.extIpv6L4.rule.DstPort=dstport;
		store->TcamPtr.ruleInfo.extIpv6L4.rule.SrcPort=srcport;
		store->TcamPtr.ruleInfo.extIpv6L4.mask.maskSrcPort=masksrcport;
		store->TcamPtr.ruleInfo.extIpv6L4.mask.maskDstPort=maskdstport;		
		
		if(policer==1)
		{
			ret=npd_acl_policer_id_check(policerId);
			if(ret!=ACL_RETURN_CODE_SUCCESS)
			{
				syslog_ax_acl_err("policer id not existed!\n");
				rs=ACL_RETURN_CODE_POLICER_ID_NOT_SET;
				free(store);
			}
			else
			{
				rs=npd_acl_add(store);
				if(ACL_RETURN_CODE_SUCCESS!=rs) 
				{
					free(store);
				}
			}
		}
		else if(policer==0)
		{
			rs=npd_acl_add(store);
			if(ACL_RETURN_CODE_SUCCESS!=rs) 
			{
				free(store);
			}
		}
		else
		{
			/*code optimize:  Resource leak
			zhangcl@autelan.com 2013-1-17*/
			free(store);
			store = NULL;
		}

	}

    reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&rs);
	return reply;

}

DBusMessage * npd_dbus_config_ip_range(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage*	reply = NULL;
	DBusMessageIter iter;
	DBusError		err;
	unsigned int	index = 0, startIndex = 0, endIndex = 0, i = 0, j = 0, group_num = 0;
	unsigned long	startDip = 0, endDip = 0, startSip = 0, endSip = 0, maskLen = 0;
	unsigned long	op_ret = 0;
	unsigned int	policer = 0, startPid = 0, endPid = 0, range = 0, enrange = 0;
	unsigned int	ruleType = 0, rs=ACL_RETURN_CODE_ERROR, ret=ACL_RETURN_CODE_ERROR;	
	struct ACCESS_LIST_STC **storeArr = NULL ;

	if(!((productinfo.capbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_ACL_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		storeArr = (AclRuleList**)malloc(sizeof(AclRuleList*)*MAX_ACL_RULE_NUMBER);
		if(NULL == storeArr) 
		{
			/*code optimize:  Dereference after null check
			zhangcl@autelan.com 2013-1-17*/
			syslog_ax_acl_dbg("memory alloc error for acl init!!");
		}
		else
		{
			dbus_error_init(&err);

			if (!(dbus_message_get_args ( msg, &err,
										DBUS_TYPE_UINT32, &startIndex,
										DBUS_TYPE_UINT32, &endIndex,
										DBUS_TYPE_UINT32, &ruleType,
										DBUS_TYPE_UINT32, &startDip,
										DBUS_TYPE_UINT32, &endDip,
										DBUS_TYPE_UINT32, &startSip,
										DBUS_TYPE_UINT32, &endSip,
										DBUS_TYPE_UINT32, &policer,
										DBUS_TYPE_UINT32, &startPid,						   
										DBUS_TYPE_UINT32, &endPid,
										DBUS_TYPE_INVALID))) 
			{
				syslog_ax_acl_err("Unable to get input args ");
				if (dbus_error_is_set(&err)) 
				{
					syslog_ax_acl_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(storeArr);
				storeArr = NULL;
				return NULL;
			}
			for (i = 0; i < MAX_ACL_RULE_NUMBER; i++) 
			{
				if (NULL != r_acl_rule[i]) 
				{
					range++;
					if (((startIndex < r_acl_rule[i]->startIndex) && (endIndex <  r_acl_rule[i]->endIndex)) ||
						((startIndex > r_acl_rule[i]->startIndex) && (endIndex >  r_acl_rule[i]->endIndex))) 
					{
						enrange++;
					}
				}
			}
			if (enrange == range) 
			{
				for (i = 0; i < MAX_ACL_RULE_NUMBER; i++) 
				{
					if (NULL == r_acl_rule[i]) 
					{
						r_acl_rule[i] = (struct acl_range_rule *)malloc(sizeof(struct acl_range_rule));
						if(NULL == r_acl_rule[i]) 
						{
							npd_syslog_err(("memory alloc error for r_acl_rule !!"));
							ret = ACL_RETURN_CODE_ERROR; 	
						}
						memset(r_acl_rule[i], 0, sizeof(struct acl_range_rule));
						r_acl_rule[i]->startIndex = startIndex;
						r_acl_rule[i]->endIndex = endIndex;
						r_acl_rule[i]->ruleType = ruleType;
						r_acl_rule[i]->startDip = startDip;
						r_acl_rule[i]->endDip = endDip;
						r_acl_rule[i]->startSip = startSip;
						r_acl_rule[i]->endSip = endSip;
						r_acl_rule[i]->policer = policer;
						r_acl_rule[i]->startPid = startPid;						   
						r_acl_rule[i]->endPid = endPid;
						range = i; /* the index of r_acl_rule in array*/
						break;
					}
					else 
					{
						npd_syslog_dbg("r_acl_rule  exist startIndex is %d\n", r_acl_rule[i]->startIndex);
					}
				}
				for (i = 0; i < (endIndex - startIndex + 1); i++) 
				{
					storeArr[i] = (AclRuleList*)malloc(sizeof(AclRuleList));
					memset(storeArr[i], 0, sizeof(AclRuleList));
					storeArr[i]->ruleIndex = startIndex + i;
					storeArr[i]->ruleType = ruleType;
					storeArr[i]->ActionPtr.actionType = ACL_ACTION_TYPE_PERMIT; 
					if (policer) 
					{
						storeArr[i]->ActionPtr.policerPtr.policerEnable = 1;
					}
					else 
					{
						storeArr[i]->ActionPtr.policerPtr.policerEnable = 0;
					}
					if (ruleType == 0) 
					{
						storeArr[i]->TcamPtr.ruleFormat = ACL_RULE_STD_IPv4_L4;
					}
					else if (ruleType == 1) 
					{
						storeArr[i]->TcamPtr.ruleFormat = ACL_RULE_EXT_IPv4_L4;
					}
					storeArr[i]->TcamPtr.packetType = ACL_TCAM_RULE_PACKETTYPE_IP;
					if (0 == startDip) 
					{
						storeArr[i]->TcamPtr.ruleInfo.stdIpv4L4.rule.Dip = 0;
						storeArr[i]->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDip = 0;
						storeArr[i]->TcamPtr.ruleInfo.stdIpv4L4.mask.maskLenDip = 0;
					}
					else 
					{
						storeArr[i]->TcamPtr.ruleInfo.stdIpv4L4.rule.Dip = startDip + i;
						storeArr[i]->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDip = 0xffffffff;
						storeArr[i]->TcamPtr.ruleInfo.stdIpv4L4.mask.maskLenDip = 32;
					}
					if (0 == startSip) 
					{
						storeArr[i]->TcamPtr.ruleInfo.stdIpv4L4.rule.Sip = 0;			
						storeArr[i]->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSip = 0;			
						storeArr[i]->TcamPtr.ruleInfo.stdIpv4L4.mask.maskLenSip = 0;				
					}
					else 
					{
						storeArr[i]->TcamPtr.ruleInfo.stdIpv4L4.rule.Sip = startSip + i;			
						storeArr[i]->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSip = 0xffffffff;			
						storeArr[i]->TcamPtr.ruleInfo.stdIpv4L4.mask.maskLenSip = 32;
					}
					storeArr[i]->rangerule = 1;
					npd_syslog_dbg("startDip %x\n", startDip);
					npd_syslog_dbg("endDip %x\n", endDip);
					npd_syslog_dbg("startSip %x\n", startDip);
					npd_syslog_dbg("endSip %x\n", endSip);
					
					if(policer == 0) 
					{
						rs = npd_acl_add(storeArr[i]);
						if(ACL_RETURN_CODE_SUCCESS != rs) 
						{
							free(storeArr[i]);
							break;
						}
					}
					else if(policer == 1) 
					{
						ret=npd_acl_policer_id_check(startPid);
						storeArr[i]->ActionPtr.policerPtr.policerId = startPid;
						if(ret!=ACL_RETURN_CODE_SUCCESS)
						{
							syslog_ax_acl_err("policer id not existed!\n");
							rs = ACL_RETURN_CODE_ERROR;
							free(storeArr[i]);
							break;
						}
						else 
						{
							rs=npd_acl_add(storeArr[i]);
							if(ACL_RETURN_CODE_SUCCESS!=rs)
							{	
								free(storeArr[i]);
								break;
							}
						}
					}
					else if(policer == 2) 
					{
						npd_syslog_dbg("policer is %d, startPid is %d, i is %d \n", policer, startPid, i);
						ret = npd_acl_policer_id_check(startPid + i);
						if(ret != ACL_RETURN_CODE_SUCCESS)
						{
							syslog_ax_acl_err("policer id not existed!\n");
							rs = ACL_RETURN_CODE_ERROR;
							free(storeArr[i]);
							break;
						}
						else 
						{
							storeArr[i]->ActionPtr.policerPtr.policerId = startPid + i;
							rs=npd_acl_add(storeArr[i]);
							if(ACL_RETURN_CODE_SUCCESS != rs) 
							{	
								free(storeArr[i]);
								break;
							}
						}
					}
				}
				if (((endIndex - startIndex + 1) !=  i) || (ACL_RETURN_CODE_SUCCESS != rs))
				{
					for (j = 0; j < i; j++) 
					{
						npd_acl_delete(storeArr[j]->ruleIndex, &group_num);
						free(r_acl_rule[range]);
						r_acl_rule[range] = NULL;	
					}
				}
			}	
			else 
			{
				
				rs = ACL_RETURN_CODE_ERROR;
				free(storeArr);
				storeArr = NULL;
			}
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&rs);
	return reply;	

}

DBusMessage * npd_dbus_config_permit_deny_ip_range(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage*	reply = NULL;
	DBusMessageIter iter;
	DBusError		err;
	unsigned int	index = 0, startIndex = 0, endIndex = 0, i = 0, j = 0, group_num = 0;
	unsigned long	startDip = 0, endDip = 0, startSip = 0, endSip = 0, maskLen = 0;
	unsigned long	op_ret = 0, sipmask = 0,dipmask = 0,actionType = 0;
	unsigned int	policer = 0, startPid = 0, endPid = 0, range = 0, enrange = 0;
	unsigned int	ruleType = 0, rs=ACL_RETURN_CODE_ERROR, ret=ACL_RETURN_CODE_ERROR;	
	struct ACCESS_LIST_STC *store = NULL ;

	store = (struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
	if(NULL== store)  
	{
		rs = ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
	}
	else
	{
		memset(store,0,sizeof(struct ACCESS_LIST_STC));
		dbus_error_init(&err);

		if (!(dbus_message_get_args ( msg, &err,								
									DBUS_TYPE_UINT32, &index,
									DBUS_TYPE_UINT32, &startIndex,
									DBUS_TYPE_UINT32, &endIndex,
									DBUS_TYPE_UINT32, &ruleType,
									DBUS_TYPE_UINT32, &actionType,
									DBUS_TYPE_UINT32, &startDip,
									DBUS_TYPE_UINT32, &endDip,
									DBUS_TYPE_UINT32, &startSip,
									DBUS_TYPE_UINT32, &endSip,
									DBUS_TYPE_UINT32, &policer,
									DBUS_TYPE_UINT32, &startPid,						   
									DBUS_TYPE_UINT32, &endPid,
									DBUS_TYPE_INVALID))) 
		{
			syslog_ax_acl_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) 
			{
				syslog_ax_acl_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			/*code optimize:  Resource leak
			zhangcl@autelan.com 2013-1-17*/
			free(store);
			store = NULL;
			return NULL;
		}
		npd_acl_len2mask(16, &sipmask);
		npd_acl_len2mask(0, &dipmask);
		
		store->ruleIndex=index;		
		store->startIndex=startIndex;
		store->endIndex=endIndex;
		store->ruleType=ruleType;
		
		if(0==actionType)
		{
	        store->ActionPtr.actionType=ACL_ACTION_TYPE_PERMIT;	
			store->ActionPtr.policerPtr.policerEnable = policer;
			store->ActionPtr.policerPtr.policerId = startPid;	
			npd_syslog_dbg("policer %d, startPid\n", policer, startPid);
		}
		else if(1==actionType) 
		{
			store->ActionPtr.actionType=ACL_ACTION_TYPE_DENY;
		}

		if (ruleType == 0) 
		{
			store->TcamPtr.ruleFormat=ACL_RULE_STD_IPv4_L4;
		}
		else if (ruleType == 1) 
		{
			store->TcamPtr.ruleFormat=ACL_RULE_EXT_IPv4_L4;
		}
		store->TcamPtr.packetType=ACL_TCAM_RULE_PACKETTYPE_IP;
		store->TcamPtr.ruleInfo.stdIpv4L4.rule.Dip=startDip;
		store->TcamPtr.ruleInfo.stdIpv4L4.rule.Sip=startSip;
		store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDip=dipmask;
		store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSip=sipmask;
		store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskLenDip=0;
		store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskLenSip=16;
		
		npd_syslog_dbg("dstipdata %x\n", startDip);
		npd_syslog_dbg("dstipmask %x\n", dipmask);
		npd_syslog_dbg("srcipdata %x\n", startSip);
		npd_syslog_dbg("srcipmask %x\n", sipmask);

		if(policer==1)
		{
			ret=npd_acl_policer_id_check(startPid);
			if(ret!=ACL_RETURN_CODE_SUCCESS) 
			{
				syslog_ax_acl_err("policer id not existed! id is %d\n", startPid);
				rs=ACL_RETURN_CODE_POLICER_ID_NOT_SET;
				free(store);
			}
			else 
			{
				rs=npd_acl_add(store);/*add rule*/	
				if(ACL_RETURN_CODE_SUCCESS!=rs) 
				{	
					free(store);
				}
			}
		}
		else if(policer==0) 
		{
			rs=npd_acl_add(store);/*add rule*/	
			if(ACL_RETURN_CODE_SUCCESS!=rs)
			{
				free(store);
			}
		}
		else
		{
			/*code optimize:  Resource leak
			zhangcl@autelan.com 2013-1-17*/
			free(store);
			store = NULL;
		}
	}	
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&rs);
	
	return reply;	
}

DBusMessage * npd_dbus_config_permit_deny_icmp(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage*	reply = NULL;
	DBusMessageIter iter;
	DBusError		err;
	unsigned long	ruleIndex = 0;
	unsigned int	sipmaskLen = 0, dipmaskLen = 0;
	unsigned long	dipno = 0, sipno = 0, sipmask = 0, dipmask = 0;
	unsigned int    actionType = 0;
	unsigned int	ruleType,rs=ACL_RETURN_CODE_ERROR,ret=ACL_RETURN_CODE_ERROR;	
	unsigned char   typeno=0,codeno=0,masktype=0,maskcode=0,acl_any=0;
	unsigned int    policer=0,policerId=0;
	struct ACCESS_LIST_STC *store = NULL ;

	if(!((productinfo.capbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_ACL_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		store=(struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
		if(store==NULL) 
		{
			rs=ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
		}
		else 
		{
			memset(store,0,sizeof(struct ACCESS_LIST_STC));
			dbus_error_init(&err);

			if (!(dbus_message_get_args ( msg, &err,
						DBUS_TYPE_UINT32,&ruleIndex,
						DBUS_TYPE_UINT32,&ruleType,
						DBUS_TYPE_UINT32,&actionType,
						DBUS_TYPE_UINT32,&sipmaskLen,
						DBUS_TYPE_UINT32,&dipmaskLen,
						DBUS_TYPE_UINT32,&dipno,
						DBUS_TYPE_UINT32,&sipno,
						DBUS_TYPE_BYTE,&typeno,
						DBUS_TYPE_BYTE,&codeno,
						DBUS_TYPE_BYTE,&acl_any,
						DBUS_TYPE_UINT32,&policer,
						DBUS_TYPE_UINT32,&policerId,
						DBUS_TYPE_INVALID))) 
			{
				syslog_ax_acl_err("Unable to get input args ");
				if (dbus_error_is_set(&err)) 
				{
					syslog_ax_acl_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				store = NULL;
				return NULL;
			}
			
			npd_acl_len2mask(sipmaskLen,&sipmask);
			npd_acl_len2mask(dipmaskLen,&dipmask);
			
			if(acl_any&1) 
			{
		    	masktype=0;
			}
			else 
			{           
		        masktype=255;
			}
		    if(acl_any&2) 
			{
		        maskcode=0;
		    }
			else 
			{           
		        maskcode=255;
		    }
			
			store->ruleIndex=ruleIndex;
			store->ruleType=ruleType;
			if(0==actionType) 
			{
				store->ActionPtr.actionType=ACL_ACTION_TYPE_PERMIT;	
				store->ActionPtr.policerPtr.policerEnable = policer;
				store->ActionPtr.policerPtr.policerId = policerId;
			}
			else if(1==actionType) 
			{
				store->ActionPtr.actionType=ACL_ACTION_TYPE_DENY;	
			}
			if (ruleType == 0) 
			{
				store->TcamPtr.ruleFormat=ACL_RULE_STD_IPv4_L4;
			}
			else if (ruleType == 1) 
			{
				store->TcamPtr.ruleFormat=ACL_RULE_EXT_IPv4_L4;
			}
			store->TcamPtr.packetType=ACL_TCAM_RULE_PACKETTYPE_ICMP;
			store->TcamPtr.ruleInfo.stdIpv4L4.rule.Dip=dipno;
			store->TcamPtr.ruleInfo.stdIpv4L4.rule.Sip=sipno;
			store->TcamPtr.ruleInfo.stdIpv4L4.rule.Type=typeno;
			store->TcamPtr.ruleInfo.stdIpv4L4.rule.Code=codeno;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDip=dipmask;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSip=sipmask;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskLenDip=dipmaskLen;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskLenSip=sipmaskLen;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskType=masktype;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskCode=maskcode;

			npd_syslog_dbg("dstipdata %x\n", dipno);
			npd_syslog_dbg("dstipmask %x\n", dipmask);
			npd_syslog_dbg("srcipdata %x\n", sipno);
			npd_syslog_dbg("srcipmask %x\n", sipmask);
			
			if(policer==1) 
			{
				ret=npd_acl_policer_id_check(policerId);
				if(ret != ACL_RETURN_CODE_SUCCESS) 
				{
					syslog_ax_acl_err("policer id not existed!\n");
					rs=ACL_RETURN_CODE_POLICER_ID_NOT_SET;
					free(store);
				}
				else 
				{
					rs = npd_acl_add(store);/*add rule	*/
					if(ACL_RETURN_CODE_SUCCESS != rs)
						free(store);
				}
			}
			else if(policer == 0) 
			{
				rs=npd_acl_add(store);/*add rule	*/
				if(ACL_RETURN_CODE_SUCCESS!=rs)
					free(store);
			}
			else
			{
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				store = NULL;
			}
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&rs);
	return reply;	

}

DBusMessage * npd_dbus_config_permit_deny_mac(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage*	reply = NULL;
	DBusMessageIter iter;
	DBusError		err;		
	unsigned long   ruleIndex = 0, actionType,ret=ACL_RETURN_CODE_ERROR;
	ETHERADDR	    smacAddr,dmacAddr;
	unsigned int    ruleType = 0, i = 0, rs=ACL_RETURN_CODE_ERROR;
	struct ACCESS_LIST_STC *store = NULL;
	unsigned int 	count1 = 0,count2 = 0,j = 0;
	unsigned int	policer = 0,policerId = 0, ethertype = 0;

	if(!((productinfo.capbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_ACL_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		store=(struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
		if(store==NULL)  
		{
			rs=ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
		}
		else 
		{
			memset(store,0,sizeof(struct ACCESS_LIST_STC));
			memset(&smacAddr,0,sizeof(ETHERADDR));
			memset(&dmacAddr,0,sizeof(ETHERADDR));
			
			dbus_error_init(&err);

			if (!(dbus_message_get_args (msg, &err,
										DBUS_TYPE_UINT32, &ruleIndex,
										DBUS_TYPE_UINT32, &ruleType,
										DBUS_TYPE_UINT32, &actionType,
										DBUS_TYPE_UINT32, &ethertype,
										DBUS_TYPE_BYTE,  &dmacAddr.arEther[0],
										DBUS_TYPE_BYTE,  &dmacAddr.arEther[1],
										DBUS_TYPE_BYTE,  &dmacAddr.arEther[2],
										DBUS_TYPE_BYTE,  &dmacAddr.arEther[3],
										DBUS_TYPE_BYTE,  &dmacAddr.arEther[4],
										DBUS_TYPE_BYTE,  &dmacAddr.arEther[5],
										DBUS_TYPE_BYTE,  &smacAddr.arEther[0],
										DBUS_TYPE_BYTE,  &smacAddr.arEther[1],
										DBUS_TYPE_BYTE,  &smacAddr.arEther[2],			
										DBUS_TYPE_BYTE,  &smacAddr.arEther[3],
										DBUS_TYPE_BYTE,  &smacAddr.arEther[4],
										DBUS_TYPE_BYTE,  &smacAddr.arEther[5],	
										DBUS_TYPE_UINT32, &policer,
										DBUS_TYPE_UINT32, &policerId,
										DBUS_TYPE_INVALID))) 
			{
				syslog_ax_acl_err("Unable to get input args ");
				if (dbus_error_is_set(&err)) 
				{
					syslog_ax_acl_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				store = NULL;
				return NULL;
			}
			
			for(j=0;j<6;j++)
			{

				if(0==dmacAddr.arEther[j]) 
				{
					count1+=1;
				}
				if(0==smacAddr.arEther[j]) 
				{
					count2+=1;
				}	
	/*
				if(0==dmacAddr.arEther[j]) {
					store->TcamPtr.ruleInfo.stdNotIp.mask.maskDMac[i] = 0x0;
				}
				else {
					store->TcamPtr.ruleInfo.stdNotIp.mask.maskDMac[i] = 0xFF;
				}
				if(0==smacAddr.arEther[j]) {
					store->TcamPtr.ruleInfo.stdNotIp.mask.maskSMac[i]=0x0;
				}
				else {
				 	store->TcamPtr.ruleInfo.stdNotIp.mask.maskSMac[i]=0xFF;	
				}
	*/
			}
			
			store->ruleIndex=ruleIndex;
			store->ruleType=ruleType;
			if(ACL_ACTION_TYPE_PERMIT == actionType) 
			{
		        	store->ActionPtr.actionType=ACL_ACTION_TYPE_PERMIT;	
				store->ActionPtr.policerPtr.policerEnable = policer;
				store->ActionPtr.policerPtr.policerId = policerId;
			}
			else if(ACL_ACTION_TYPE_DENY == actionType) 
			{
				store->ActionPtr.actionType=ACL_ACTION_TYPE_DENY;
			}
			store->TcamPtr.ruleFormat=ACL_RULE_STD_NOT_IP;
			/*store->TcamPtr.ruleFormat=ACL_RULE_EXT_NOT_IPv6;*/
			store->TcamPtr.packetType=ACL_TCAM_RULE_PACKETTYPE_ETHERNET;
			if (ethertype < 0xffff)	
			{
				store->TcamPtr.ruleInfo.stdNotIp.rule.ethertype = ethertype;
				store->TcamPtr.ruleInfo.stdNotIp.mask.maskethertype = 0xffff;
			}
			for(i=0;i<6;i++) 
			{
				 store->TcamPtr.ruleInfo.stdNotIp.rule.dMac[i]=dmacAddr.arEther[i];
				 store->TcamPtr.ruleInfo.stdNotIp.rule.sMac[i]=smacAddr.arEther[i];	
				 
				 if(count1!=6) 
				 {
				 	store->TcamPtr.ruleInfo.stdNotIp.mask.maskDMac[i]=0xFF;
				 }
				 if(count2!=6) 
				 {
				 	store->TcamPtr.ruleInfo.stdNotIp.mask.maskSMac[i]=0xFF;
				 }
			}
			
			if(policer==1) 
			{
				ret=npd_acl_policer_id_check(policerId);
				if(ret!=ACL_RETURN_CODE_SUCCESS)
				{
					syslog_ax_acl_err("policer id not existed!\n");
					rs=ACL_RETURN_CODE_POLICER_ID_NOT_SET;
					free(store);
				}
				else
				{
					rs=npd_acl_add_notIp(store);/*add rule*/	
					if(ACL_RETURN_CODE_SUCCESS!=rs)
						free(store);
				}
			}
			else if(policer==0)
			{
				rs=npd_acl_add_notIp(store);/*add rule	*/
				if(ACL_RETURN_CODE_SUCCESS!=rs)
					free(store);
			}
			else
			{
				/*permit bpdu,added by zhengcs*/
				/*npd_acl_permit_bpdu(ruleIndex,policer,policerId);*/
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				store = NULL;
			}
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&rs);
	return reply;
	
}

/*deny_tcp_or_udp*/
DBusMessage * npd_dbus_config_permit_deny_tcp_or_udp(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage* 	reply = NULL;
	DBusMessageIter	iter;
	DBusError 		err;
	unsigned long   ruleIndex = 0;
	unsigned long   dipno = 0,sipno = 0,sipmaskLen = 0,dipmaskLen = 0,sipmask = 0,dipmask = 0;
	unsigned long   dstport = 0,srcport = 0,masksrcport = 0,maskdstport = 0;
      unsigned int   actionType = 0,ruleType = 0;
	int packetType = 0;
	unsigned int    rs=ACL_RETURN_CODE_ERROR,ret=ACL_RETURN_CODE_ERROR;	
	unsigned int	policer = 0,policerId = 0;

	struct ACCESS_LIST_STC *store =NULL;

	if(!((productinfo.capbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_ACL_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		store=(struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
		if(store==NULL) 
		{
			rs=ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
		}
		else
		{
			memset(store,0,sizeof(struct  ACCESS_LIST_STC));
			dbus_error_init(&err);

			if (!(dbus_message_get_args ( msg, &err,
										DBUS_TYPE_UINT32,&ruleIndex,
										DBUS_TYPE_UINT32,&ruleType,
										DBUS_TYPE_UINT32,&sipmaskLen,
										DBUS_TYPE_UINT32,&dipmaskLen,
										DBUS_TYPE_UINT32,&dipno,
										DBUS_TYPE_UINT32,&sipno,
										DBUS_TYPE_UINT32,&dstport,
										DBUS_TYPE_UINT32,&srcport,
										DBUS_TYPE_UINT32,&actionType,
										DBUS_TYPE_UINT32,&packetType,
										DBUS_TYPE_UINT32,&policer,
										DBUS_TYPE_UINT32,&policerId,
										DBUS_TYPE_INVALID))) 
			{
				syslog_ax_acl_err("Unable to get input args ");
				if (dbus_error_is_set(&err)) 
				{
					syslog_ax_acl_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				store = NULL;
				return NULL;
			}

			npd_acl_len2mask(sipmaskLen,&sipmask);
			npd_acl_len2mask(dipmaskLen,&dipmask);
			
			
			if(ACL_ANY_PORT==srcport) 
			{
				srcport = 0;
		        masksrcport=0;
			}
			else 
			{          
		        masksrcport=65535;
			}
		    if(ACL_ANY_PORT==dstport) 
			{
			   dstport = 0;
		       maskdstport=0;
		    } 
			else 
			{          
		    	maskdstport=65535;
			}

			store->ruleIndex=ruleIndex;
			store->ruleType=ruleType;
			if(0==actionType)
			{
		        store->ActionPtr.actionType=ACL_ACTION_TYPE_PERMIT;	
				store->ActionPtr.policerPtr.policerEnable = policer;
				store->ActionPtr.policerPtr.policerId = policerId;
			}
			else if(1==actionType)
				store->ActionPtr.actionType=ACL_ACTION_TYPE_DENY;	
			if (ruleType == 0)
				store->TcamPtr.ruleFormat=ACL_RULE_STD_IPv4_L4;
			else if (ruleType == 1)
				store->TcamPtr.ruleFormat=ACL_RULE_EXT_IPv4_L4;
			
		    if(1==packetType)
		        store->TcamPtr.packetType=ACL_TCAM_RULE_PACKETTYPE_UDP;
		    else if(2==packetType)
		         store->TcamPtr.packetType=ACL_TCAM_RULE_PACKETTYPE_TCP;
			
			store->TcamPtr.ruleInfo.stdIpv4L4.rule.Dip=dipno;
			store->TcamPtr.ruleInfo.stdIpv4L4.rule.Sip=sipno;
			store->TcamPtr.ruleInfo.stdIpv4L4.rule.DstPort=dstport;
			store->TcamPtr.ruleInfo.stdIpv4L4.rule.SrcPort=srcport;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDip=dipmask;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSip=sipmask;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskLenDip=dipmaskLen;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskLenSip=sipmaskLen;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSrcPort=masksrcport;
			store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDstPort=maskdstport;

			if(policer==1)
			{
				ret=npd_acl_policer_id_check(policerId);
				if(ret!=ACL_RETURN_CODE_SUCCESS)
				{
					syslog_ax_acl_err("policer id not existed!\n");
					rs=ACL_RETURN_CODE_POLICER_ID_NOT_SET;
					free(store);
				}
				else
				{
					rs=npd_acl_add(store);/*add rule*/	
					if(ACL_RETURN_CODE_SUCCESS!=rs)
						free(store);
				}
			}
			else if(policer==0)
			{
				rs=npd_acl_add(store);/*add rule*/	
				if(ACL_RETURN_CODE_SUCCESS!=rs)
					free(store);
			}
			else
			{
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				store = NULL;
			}
			
		}		
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&rs);
	return reply;

}

DBusMessage * npd_dbus_config_mirror_redirect_ip(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage*	reply = NULL;
	DBusMessageIter iter;
	DBusError		err;
	unsigned long	ruleIndex = 0;
	unsigned long	sipmaskLen = 0,dipmaskLen = 0;
	unsigned long	dipno = 0,sipno = 0,sipmask = 0,dipmask = 0;
	unsigned int    actionType = 0,eth_g_index=0;
	unsigned int	ruleType,rs=ACL_RETURN_CODE_ERROR,tmp;	
	unsigned char    slot_no = 0,port_no,devNum = 0,virPortId = 0;
	struct ACCESS_LIST_STC *store = NULL ;
	unsigned int	policer=0,policerId=0,ret = 0;

	if(!((productinfo.capbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_ACL_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		store=(struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
		if(store==NULL)  
		{
			rs=ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
		}
		else
		{
			memset(store,0,sizeof(struct ACCESS_LIST_STC));
			
			dbus_error_init(&err);

			if (!(dbus_message_get_args ( msg, &err,
										  DBUS_TYPE_UINT32,&ruleIndex,
										  DBUS_TYPE_UINT32,&ruleType,
										  DBUS_TYPE_UINT32,&actionType,
										  DBUS_TYPE_UINT32,&sipmaskLen,
										  DBUS_TYPE_UINT32,&dipmaskLen,
										  DBUS_TYPE_UINT32,&dipno,
										  DBUS_TYPE_UINT32,&sipno,
										  DBUS_TYPE_BYTE,  &slot_no,
										  DBUS_TYPE_BYTE,  &port_no,
										   DBUS_TYPE_UINT32,&policer,
										  DBUS_TYPE_UINT32,&policerId,
										  DBUS_TYPE_INVALID))) 
			{
				syslog_ax_acl_err("Unable to get input args ");
				if (dbus_error_is_set(&err)) 
				{
					syslog_ax_acl_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				return NULL;
			}
			npd_acl_len2mask(sipmaskLen,&sipmask);
			npd_acl_len2mask(dipmaskLen,&dipmask);
					
			tmp = NPD_DBUS_ERROR_NO_SUCH_PORT;

			if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) 
			{
				if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,port_no)) 
				{
					eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
					syslog_ax_acl_dbg("index for %d/%d is %#0x\n",slot_no,port_no,eth_g_index);			
					tmp = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortId);
					if(tmp != 0)
					{
						syslog_ax_acl_dbg("Npd_acl::npd_get_devport_by_global_index::get devport Error.\n");
						tmp = NPD_DBUS_ERROR_NO_SUCH_PORT;
						syslog_ax_acl_dbg("Npd_acl::npd_get_devport_by_global_index::get devport Error.\n");
					}
					else 
					{
						syslog_ax_acl_dbg("npd_get_devport_by_global_index::devNum %d,portNum %d\n",devNum,virPortId); 			
						tmp=ACL_RETURN_CODE_SUCCESS;
					}
				}
			}
							
		    if(ACL_RETURN_CODE_SUCCESS==tmp)
			{
				store->ruleIndex=ruleIndex;
				store->ruleType=ruleType;
				if(4==actionType)
				{
					store->ActionPtr.actionType=ACL_ACTION_TYPE_REDIRECT;	
					store->ActionPtr.actionInfo.redirect.portNum = virPortId;
					store->ActionPtr.actionInfo.redirect.modid = devNum;
				}
						
				if (ruleType == 0) 
				{
					store->TcamPtr.ruleFormat=ACL_RULE_STD_IPv4_L4;
				}
				else if (ruleType == 1) 
				{
					store->TcamPtr.ruleFormat=ACL_RULE_EXT_IPv4_L4;
				}
				store->TcamPtr.packetType=ACL_TCAM_RULE_PACKETTYPE_IP;
				store->TcamPtr.ruleInfo.stdIpv4L4.rule.Dip=dipno;
				store->TcamPtr.ruleInfo.stdIpv4L4.rule.Sip=sipno;
				store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDip=dipmask;
				store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSip=sipmask;
				store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskLenDip=dipmaskLen;
				store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskLenSip=sipmaskLen;
				store->ActionPtr.policerPtr.policerEnable = policer;
				store->ActionPtr.policerPtr.policerId = policerId;	
				
				if(policer==1)
				{
					ret=npd_acl_policer_id_check(policerId);
					if(ret!=ACL_RETURN_CODE_SUCCESS)
					{
						syslog_ax_acl_err("policer id not existed!\n");
						rs=ACL_RETURN_CODE_POLICER_ID_NOT_SET;
						free(store);
					}
					else
					{
						rs=npd_acl_add(store);/*add rule*/	
						if(ACL_RETURN_CODE_SUCCESS!=rs)
							free(store);
					}
				}
				else if(policer==0)
				{
					rs=npd_acl_add(store);/*add rule*/	
					if(ACL_RETURN_CODE_SUCCESS!=rs)
						free(store);
				}
				else
				{
					/*code optimize:  Resource leak
					zhangcl@autelan.com 2013-1-17*/
					free(store);
					store = NULL;
				}
				
			}
			else
			{
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				store = NULL;
			}
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&rs);
	return reply;	

}
DBusMessage * npd_dbus_config_mirror_redirect_tcp_udp(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage*	reply = NULL;
	DBusMessageIter iter;
	DBusError		err;
	unsigned long	ruleIndex = 0;
	unsigned long	sipmaskLen = 0, dipmaskLen = 0;
	unsigned long	dipno = 0, sipno = 0, sipmask = 0, dipmask = 0;
	unsigned int    packetType = 0, actionType = 0, eth_g_index=0;
	unsigned int	rs=ACL_RETURN_CODE_ERROR, tmp = 0, ruleType = 0;	
	unsigned char    slot_no = 0, port_no = 0, devNum = 0, virPortId = 0;
	unsigned long   dstport = 0, srcport = 0,masksrcport = 0,maskdstport = 0;
	struct ACCESS_LIST_STC *store = NULL ;
	unsigned int	policer=0,policerId=0,ret = 0;

	if(!((productinfo.capbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_ACL_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		store=(struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
		if(store==NULL) 
		{
			rs=ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
		}
		else 
		{
			memset(store,0,sizeof(struct ACCESS_LIST_STC));
			dbus_error_init(&err);

			if (!(dbus_message_get_args ( msg, &err,
										DBUS_TYPE_UINT32,&ruleIndex,
										DBUS_TYPE_UINT32,&ruleType,
										DBUS_TYPE_UINT32,&sipmaskLen,
										DBUS_TYPE_UINT32,&dipmaskLen,
										DBUS_TYPE_UINT32,&dipno,
										DBUS_TYPE_UINT32,&sipno,
										DBUS_TYPE_UINT32,&dstport,
										DBUS_TYPE_UINT32,&srcport,
										DBUS_TYPE_UINT32,&actionType,
										DBUS_TYPE_UINT32,&packetType,
										DBUS_TYPE_BYTE,  &slot_no,
										DBUS_TYPE_BYTE,  &port_no,
										DBUS_TYPE_UINT32,&policer,
										DBUS_TYPE_UINT32,&policerId,
										DBUS_TYPE_INVALID))) 
			{
				syslog_ax_acl_err("Unable to get input args ");
				if (dbus_error_is_set(&err)) 
				{
					syslog_ax_acl_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				store = NULL;
				return NULL;
			}
			
			npd_acl_len2mask(sipmaskLen,&sipmask);
			npd_acl_len2mask(dipmaskLen,&dipmask);
			
			if(ACL_ANY_PORT==srcport) 
			{
				srcport = 0;
		        masksrcport=0;
			}
			else 
			{          
		        masksrcport=65535;
			}
		    if(ACL_ANY_PORT==dstport) 
			{
				dstport = 0;
		        maskdstport=0;
		    } 
			else 
			{          
		    	maskdstport=65535;
			}

			
			 tmp = NPD_DBUS_ERROR_NO_SUCH_PORT;

			 if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) 
			 {
				if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,port_no)) 
				{
					eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
					syslog_ax_acl_dbg("index for %d/%d is %#0x\n",slot_no,port_no,eth_g_index);			
					tmp = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortId);
					if(tmp != 0)
					{
						tmp = NPD_DBUS_ERROR_NO_SUCH_PORT;
						syslog_ax_acl_dbg("Npd_acl::npd_get_devport_by_global_index::get devport Error.\n");
					}
					else 
					{
						syslog_ax_acl_dbg("npd_get_devport_by_global_index::devNum %d,portNum %d\n",devNum,virPortId); 			

					}
				}
			}
			
		    if(ACL_RETURN_CODE_SUCCESS==tmp)
			{
				store->ruleIndex=ruleIndex;
				store->ruleType=ruleType;
				if(4==actionType)
				{
					store->ActionPtr.actionType=ACL_ACTION_TYPE_REDIRECT;	
					store->ActionPtr.actionInfo.redirect.portNum = virPortId;
					store->ActionPtr.actionInfo.redirect.modid = devNum;
				}
				if(1==packetType)
					store->TcamPtr.packetType=ACL_TCAM_RULE_PACKETTYPE_UDP;
			   	else if(2==packetType)
					store->TcamPtr.packetType=ACL_TCAM_RULE_PACKETTYPE_TCP;

				if (ruleType == 0)
					store->TcamPtr.ruleFormat=ACL_RULE_STD_IPv4_L4;
			      else if (ruleType == 1)
					store->TcamPtr.ruleFormat=ACL_RULE_EXT_IPv4_L4;
			
				store->TcamPtr.ruleInfo.stdIpv4L4.rule.Dip=dipno;
				store->TcamPtr.ruleInfo.stdIpv4L4.rule.Sip=sipno;
				store->TcamPtr.ruleInfo.stdIpv4L4.rule.DstPort=dstport;
				store->TcamPtr.ruleInfo.stdIpv4L4.rule.SrcPort=srcport;
				store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDip=dipmask;
				store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSip=sipmask;
				store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskLenDip=dipmaskLen;
				store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskLenSip=sipmaskLen;
				store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSrcPort=masksrcport;
				store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDstPort=maskdstport;
				store->ActionPtr.policerPtr.policerEnable = policer;
				store->ActionPtr.policerPtr.policerId = policerId;	
				
				if(policer==1)
				{
					ret=npd_acl_policer_id_check(policerId);
					if(ret!=ACL_RETURN_CODE_SUCCESS)
					{
						syslog_ax_acl_err("policer id not existed!\n");
						rs=ACL_RETURN_CODE_POLICER_ID_NOT_SET;
						free(store);
					}
					else
					{
						rs=npd_acl_add(store);/*add rule*/	
						if(ACL_RETURN_CODE_SUCCESS!=rs)
							free(store);
					}
				}
				else if(policer==0)
				{
					rs=npd_acl_add(store);/*add rule*/	
					if(ACL_RETURN_CODE_SUCCESS!=rs)
						free(store);
				}
				else
				{
					/*code optimize:  Resource leak
					zhangcl@autelan.com 2013-1-17*/
					free(store);
					store = NULL;
				}
			}
			else
			{
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				store = NULL;
			}
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&rs);
	return reply;	

}
DBusMessage * npd_dbus_config_mirror_redirect_icmp(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage*	reply = NULL;
	DBusMessageIter iter;
	DBusError		err;
	unsigned long	ruleIndex = 0;
	unsigned long	sipmaskLen = 0,dipmaskLen = 0;
	unsigned long	dipno = 0,sipno = 0,sipmask = 0,dipmask = 0;
	unsigned int    actionType = 0,eth_g_index=0;
	unsigned int	ruleType = 0,rs=ACL_RETURN_CODE_ERROR,tmp = 0;	
	unsigned char    slot_no = 0,port_no = 0,devNum = 0,virPortId = 0;
	unsigned char    typeno=0,codeno=0,masktype=0,maskcode=0,acl_any = 0;
	unsigned int    policer=0,policerId=0,ret;
	struct ACCESS_LIST_STC *store = NULL ;

	if(!((productinfo.capbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_ACL_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		store=(struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
		if(store==NULL) 
		{
			rs=ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
		}
		else 
		{
			memset(store,0,sizeof(struct ACCESS_LIST_STC));
			dbus_error_init(&err);

			if (!(dbus_message_get_args ( msg, &err,
										DBUS_TYPE_UINT32,&ruleIndex,
										DBUS_TYPE_UINT32,&ruleType,
										DBUS_TYPE_UINT32,&actionType,
										DBUS_TYPE_UINT32,&sipmaskLen,
										DBUS_TYPE_UINT32,&dipmaskLen,
										DBUS_TYPE_UINT32,&dipno,
										DBUS_TYPE_UINT32,&sipno,
										DBUS_TYPE_BYTE,&typeno,
										DBUS_TYPE_BYTE,&codeno,
										DBUS_TYPE_BYTE,&acl_any,
										DBUS_TYPE_BYTE,  &slot_no,
										DBUS_TYPE_BYTE,  &port_no,
										DBUS_TYPE_UINT32,&policer,
										DBUS_TYPE_UINT32,&policerId,
										DBUS_TYPE_INVALID))) 
			{
				syslog_ax_acl_err("Unable to get input args ");
				if (dbus_error_is_set(&err)) 
				{
					syslog_ax_acl_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				store = NULL;
				return NULL;
			}
			npd_acl_len2mask(sipmaskLen,&sipmask);
			npd_acl_len2mask(dipmaskLen,&dipmask);
			
			if(1&acl_any) 
			{
				masktype=0;
			}
			else	 
			{	   
				masktype=255;
			}
			if(2&acl_any)
			{
				maskcode=0;
			}
			else	 
			{  
				maskcode=255;
			}
			
			tmp = NPD_DBUS_ERROR_NO_SUCH_PORT;

			if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) 
			{
				if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,port_no)) 
				{
					eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
					syslog_ax_acl_dbg("index for %d/%d is %#0x\n",slot_no,port_no,eth_g_index);			
					tmp = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortId);
					if(tmp != 0)
					{
						tmp = NPD_DBUS_ERROR_NO_SUCH_PORT;
						syslog_ax_acl_dbg("Npd_acl::npd_get_devport_by_global_index::get devport Error.\n");
					}
					else 
						syslog_ax_acl_dbg("npd_get_devport_by_global_index::devNum %d,portNum %d\n",devNum,virPortId); 			
				}
			}
		    if(ACL_RETURN_CODE_SUCCESS==tmp)
			{
				store->ruleIndex=ruleIndex;
				store->ruleType=ruleType;
				syslog_ax_acl_dbg("actiontype %d\n ",actionType);
				 if(4==actionType)
				 {
					store->ActionPtr.actionType=ACL_ACTION_TYPE_REDIRECT;	
					store->ActionPtr.actionInfo.redirect.portNum = virPortId;
					store->ActionPtr.actionInfo.redirect.modid = devNum;
					syslog_ax_acl_dbg("redirect virportId %d\n",(store->ActionPtr.actionInfo.redirect.portNum));
				}
				if (ruleType == 0)
					store->TcamPtr.ruleFormat=ACL_RULE_STD_IPv4_L4;
				else if (ruleType == 1)
					store->TcamPtr.ruleFormat=ACL_RULE_EXT_IPv4_L4;
				store->TcamPtr.packetType=ACL_TCAM_RULE_PACKETTYPE_ICMP;
				store->TcamPtr.ruleInfo.stdIpv4L4.rule.Dip=dipno;
				store->TcamPtr.ruleInfo.stdIpv4L4.rule.Sip=sipno;
				store->TcamPtr.ruleInfo.stdIpv4L4.rule.Type=typeno;
				store->TcamPtr.ruleInfo.stdIpv4L4.rule.Code=codeno;
				store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDip=dipmask;
				store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSip=sipmask;
				store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskLenDip=dipmaskLen;
				store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskLenSip=sipmaskLen;
				store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskType=masktype;
				store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskCode=maskcode;
				store->ActionPtr.policerPtr.policerEnable = policer;
				store->ActionPtr.policerPtr.policerId = policerId;	
				
				if(policer==1)
				{
					ret=npd_acl_policer_id_check(policerId);
					if(ret!=ACL_RETURN_CODE_SUCCESS)
					{
						syslog_ax_acl_err("policer id not existed!\n");
						rs=ACL_RETURN_CODE_POLICER_ID_NOT_SET;
						free(store);
					}
					else
					{
						rs=npd_acl_add(store);/*add rule*/	
						if(ACL_RETURN_CODE_SUCCESS!=rs)
							free(store);
					}
				}
				else if(policer==0)
				{
					rs=npd_acl_add(store);/*add rule*/	
					if(ACL_RETURN_CODE_SUCCESS!=rs)
						free(store);
				}
				else
				{
					/*code optimize:  Resource leak
					zhangcl@autelan.com 2013-1-17*/
					free(store);
					store = NULL;
				}
			}
			else
			{
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				store = NULL;
			}
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&rs);
	return reply;	

}

DBusMessage * npd_dbus_config_mirror_redirect_mac(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage*	reply = NULL;
	DBusMessageIter iter;
	DBusError		err;		
	unsigned long   ruleIndex = 0,actionType = 0,ret = 0;
	ETHERADDR	    smacAddr,dmacAddr;
	unsigned int    ruleType = 0, i = 0, j = 0, rs=ACL_RETURN_CODE_ERROR, tmp = 0, eth_g_index = 0;
	unsigned char    slot_no = 0,port_no = 0,devNum = 0,virPortId = 0;
	unsigned int    count1=0,count2=0, ethertype = 0;
	unsigned int	policer=0,policerId=0;

	struct ACCESS_LIST_STC *store = NULL;

	if(!((productinfo.capbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_ACL_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		store=(struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
		if(store==NULL) 
		{
			rs=ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
		}
		else 
		{
			memset(store,0,sizeof(struct ACCESS_LIST_STC));
			memset(&smacAddr,0,sizeof(ETHERADDR));
			memset(&dmacAddr,0,sizeof(ETHERADDR));
			
			dbus_error_init(&err);

			if (!(dbus_message_get_args (msg, &err,
										DBUS_TYPE_UINT32,&ruleIndex,
										DBUS_TYPE_UINT32,&ruleType,
										DBUS_TYPE_UINT32,&actionType,
										DBUS_TYPE_UINT32,&ethertype,
										DBUS_TYPE_BYTE,  &dmacAddr.arEther[0],
										DBUS_TYPE_BYTE,  &dmacAddr.arEther[1],
										DBUS_TYPE_BYTE,  &dmacAddr.arEther[2],
										DBUS_TYPE_BYTE,  &dmacAddr.arEther[3],
										DBUS_TYPE_BYTE,  &dmacAddr.arEther[4],
										DBUS_TYPE_BYTE,  &dmacAddr.arEther[5],
										DBUS_TYPE_BYTE,  &smacAddr.arEther[0],
										DBUS_TYPE_BYTE,  &smacAddr.arEther[1],
										DBUS_TYPE_BYTE,  &smacAddr.arEther[2],
										DBUS_TYPE_BYTE,  &smacAddr.arEther[3],
										DBUS_TYPE_BYTE,  &smacAddr.arEther[4],
										DBUS_TYPE_BYTE,  &smacAddr.arEther[5],
										DBUS_TYPE_BYTE,  &slot_no,
										DBUS_TYPE_BYTE,  &port_no,
										DBUS_TYPE_UINT32,&policer,
										DBUS_TYPE_UINT32,&policerId,
										DBUS_TYPE_INVALID))) 
			{
				syslog_ax_acl_err("Unable to get input args ");
				if (dbus_error_is_set(&err)) 
				{
					syslog_ax_acl_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				store = NULL;
				return NULL;
			}	
			for(j=0;j<6;j++){
				if(dmacAddr.arEther[j]==0)
					count1+=1;
				if(smacAddr.arEther[j]==0)
					count2+=1;
			}
				
			 tmp = NPD_DBUS_ERROR_NO_SUCH_PORT;

			 if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) 
			 {
				if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,port_no)) 
				{
					eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
					syslog_ax_acl_dbg("index for %d/%d is %#0x\n",slot_no,port_no,eth_g_index);			
					tmp = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortId);
					if(tmp != 0)
					{
						tmp = NPD_DBUS_ERROR_NO_SUCH_PORT;
						syslog_ax_acl_dbg("Npd_acl::npd_get_devport_by_global_index::get devport Error.\n");
					}
					else 
						syslog_ax_acl_dbg("npd_get_devport_by_global_index::devNum %d,portNum %d\n",devNum,virPortId); 			
				}
			}
		    if(ACL_RETURN_CODE_SUCCESS==tmp)
			{
				store->ruleIndex=ruleIndex;
				store->ruleType=ruleType;
				if(3==actionType)
				{
					store->ActionPtr.actionType=ACL_ACTION_TYPE_MIRROR_TO_ANALYZER;
					store->ActionPtr.actionInfo.mirror.portNum = virPortId;
				}
				else if(4==actionType)
				{
					store->ActionPtr.actionType=ACL_ACTION_TYPE_REDIRECT;	
					store->ActionPtr.actionInfo.redirect.portNum = virPortId;
					store->ActionPtr.actionInfo.redirect.modid = devNum;
				}
				store->TcamPtr.ruleFormat=ACL_RULE_STD_NOT_IP;	
				store->TcamPtr.packetType=ACL_TCAM_RULE_PACKETTYPE_ETHERNET;
				if (ethertype < 0xffff) 
				{
					 store->TcamPtr.ruleInfo.stdNotIp.rule.ethertype = ethertype;
					 store->TcamPtr.ruleInfo.stdNotIp.mask.maskethertype = 0xffff;
				}

				for(i=0;i<6;i++)
				{
				 store->TcamPtr.ruleInfo.stdNotIp.rule.dMac[i]=dmacAddr.arEther[i];
				 store->TcamPtr.ruleInfo.stdNotIp.rule.sMac[i]=smacAddr.arEther[i];	
				 if(count1!=6)
			 		store->TcamPtr.ruleInfo.stdNotIp.mask.maskDMac[i]=0xFF;
				 if(count2!=6)
					store->TcamPtr.ruleInfo.stdNotIp.mask.maskSMac[i]=0xFF;
				}
				store->ActionPtr.policerPtr.policerEnable = policer;
				store->ActionPtr.policerPtr.policerId = policerId;	
							
				ret=ACL_RETURN_CODE_ERROR;
			
				if(policer==1)
				{
					ret=npd_acl_policer_id_check(policerId);
					if(ret!=ACL_RETURN_CODE_SUCCESS)
					{
						syslog_ax_acl_err("policer id not existed!\n");
						rs=ACL_RETURN_CODE_POLICER_ID_NOT_SET;
						free(store);
					}
					else
					{
						rs=npd_acl_add_notIp(store);/*add rule*/	
						if(ACL_RETURN_CODE_SUCCESS!=rs)
							free(store);
					}
				}
				else if(policer==0)
				{
					rs=npd_acl_add_notIp(store);/*add rule	*/
					if(ACL_RETURN_CODE_SUCCESS!=rs)
						free(store);
				}	
				else
				{
					/*code optimize:  Resource leak
					zhangcl@autelan.com 2013-1-17*/
					free(store);
					store = NULL;
				}
		    }
			else
			{
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				store = NULL;
			}
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&rs);
	return reply;
	
}
DBusMessage * npd_dbus_config_permit_deny_arp(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage*	reply = NULL;
	DBusMessageIter iter;
	DBusError		err;
	unsigned long	ruleIndex = 0,ret=ACL_RETURN_CODE_ERROR;
	ETHERADDR		smacAddr,maskMac;
	unsigned char	slot_no=0,port_no=0;
	unsigned  int	ruleType = 0,i = 0,rs=ACL_RETURN_CODE_ERROR,vlanId=0;
	unsigned int	eth_g_index = 0,actionType = 0;
	/*code optimize:  Uninitialized scalar variable
	zhangcl@autelan.com 2013-1-17*/
	unsigned char	 devNum = 0,virPortNo=0;
	unsigned int    maskVlanId=0,maskVirPort=0;
	unsigned int	policer=0,policerId=0;

	struct ACCESS_LIST_STC *store = NULL;

	if(!((productinfo.capbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_ACL_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		store=(struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
		if(store==NULL) 
		{
			rs=ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
		}
		else 
		{
			memset(store,0,sizeof(struct ACCESS_LIST_STC));
			memset(&smacAddr,0,sizeof(ETHERADDR));
			memset(&maskMac,0,sizeof(ETHERADDR));

			dbus_error_init(&err);

			if (!(dbus_message_get_args ( msg, &err,
											 DBUS_TYPE_UINT32,&ruleIndex,
											 DBUS_TYPE_UINT32,&ruleType,
											 DBUS_TYPE_UINT32,&actionType,
											 DBUS_TYPE_BYTE,  &smacAddr.arEther[0],
											 DBUS_TYPE_BYTE,  &smacAddr.arEther[1],
											 DBUS_TYPE_BYTE,  &smacAddr.arEther[2],
											 DBUS_TYPE_BYTE,  &smacAddr.arEther[3],
											 DBUS_TYPE_BYTE,  &smacAddr.arEther[4],
											 DBUS_TYPE_BYTE,  &smacAddr.arEther[5],
											 DBUS_TYPE_BYTE,  &maskMac.arEther[0],
											 DBUS_TYPE_BYTE,  &maskMac.arEther[1],
											 DBUS_TYPE_BYTE,  &maskMac.arEther[2],
											 DBUS_TYPE_BYTE,  &maskMac.arEther[3],
											 DBUS_TYPE_BYTE,  &maskMac.arEther[4],
											 DBUS_TYPE_BYTE,  &maskMac.arEther[5],	
											 DBUS_TYPE_UINT32,&vlanId,
											 DBUS_TYPE_BYTE,  &slot_no,
											 DBUS_TYPE_BYTE,  &port_no,  
											 DBUS_TYPE_UINT32,&policer,
										 	 DBUS_TYPE_UINT32,&policerId,
											 DBUS_TYPE_INVALID))) 
			{
				syslog_ax_acl_err("Unable to get input args ");
				if (dbus_error_is_set(&err)) 
				{
					syslog_ax_acl_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				store = NULL;
				return NULL;
			}

			if(vlanId!=0)
				maskVlanId =0xFF;
			
			ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
			if((ACL_ANY_PORT_CHAR == slot_no)&&(ACL_ANY_PORT_CHAR == port_no)) 
			{
				virPortNo = 0;
			      maskVirPort = 0;
				ret = ACL_RETURN_CODE_SUCCESS;
			}
			else
			{
				if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) 
				{
					if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,port_no)) 
					{
						eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
						syslog_ax_acl_dbg("index for %d/%d is %#0x\n",slot_no,port_no,eth_g_index);

						ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortNo);
						if(ret != 0)
						{
							syslog_ax_acl_err("Npd_dbus::npd_get_devport_by_global_index::get devport Error.\n");
							ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
						}
						else
						{
							syslog_ax_acl_dbg("npd_get_devport_by_global_index::devNum %d,portNum %d\n",devNum,virPortNo);
							maskVirPort = 0xFF;
							ret=ACL_RETURN_CODE_SUCCESS;
							syslog_ax_acl_dbg("Npd_dbus::npd_get_devport_by_global_index::get devport Error.\n");
						}
					}
				}
			}
				
			if(ACL_RETURN_CODE_SUCCESS==ret)
			{
				/*store acl rule*/
				store->ruleIndex=ruleIndex;
				store->ruleType=ruleType;
				if(0==actionType)
				{
				    store->ActionPtr.actionType=ACL_ACTION_TYPE_PERMIT; 
					store->ActionPtr.policerPtr.policerEnable = policer;
					store->ActionPtr.policerPtr.policerId = policerId;	
				}
				else if(1==actionType)
				{
					 store->ActionPtr.actionType=ACL_ACTION_TYPE_DENY; 
				}
				
				store->TcamPtr.ruleFormat=ACL_RULE_STD_IP_L2_QoS;
				/*store->TcamPtr.ruleFormat=ACL_RULE_EXT_NOT_IPv6;*/
				store->TcamPtr.packetType=ACL_TCAM_RULE_PACKETTYPE_ARP;

				for(i=0;i<6;i++)
				{
				 store->TcamPtr.ruleInfo.stdNotIp.rule.sMac[i]=smacAddr.arEther[i]; 
				 store->TcamPtr.ruleInfo.stdNotIp.mask.maskSMac[i]=maskMac.arEther[i];
				}
				
				store->TcamPtr.ruleInfo.stdNotIp.rule.vlanId = vlanId;
				store->TcamPtr.ruleInfo.stdNotIp.mask.maskVlanId = maskVlanId;
		    		store->TcamPtr.ruleInfo.stdNotIp.rule.portno = virPortNo;
				store->TcamPtr.ruleInfo.stdNotIp.mask.maskPortno = maskVirPort;
				store->TcamPtr.ruleInfo.stdNotIp.rule.modid = devNum;
				
				/*add notIp rule*/	

				if(policer==1)
				{
					rs=npd_acl_policer_id_check(policerId);
					if(rs!=ACL_RETURN_CODE_SUCCESS){
						syslog_ax_acl_err("policer id not existed!\n");
						rs=ACL_RETURN_CODE_POLICER_ID_NOT_SET;
						free(store);
					}
					else
					{
						rs=npd_acl_add_notIp(store);/*add rule*/	
						if(ACL_RETURN_CODE_SUCCESS!=rs)
							free(store);
					}
				}
				else if(policer==0)
				{
					syslog_ax_acl_dbg("add rule!\n");
					rs=npd_acl_add_notIp(store);/*add rule	*/
					if(ACL_RETURN_CODE_SUCCESS!=rs)
						free(store);
				}
				else
				{
					/*code optimize:  Resource leak
					zhangcl@autelan.com 2013-1-17*/
					free(store);
					store = NULL;
				}
			}
			else
			{
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				store = NULL;
			}
		}
	}
	syslog_ax_acl_dbg("sw_rs %d,eth_ret %d\n",rs,ret);	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
								   DBUS_TYPE_UINT32,
								   &rs);
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,
									&ret);

	return reply;

}

DBusMessage * npd_dbus_config_mirror_redirect_arp(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage*	reply = NULL;
	DBusMessageIter iter;
	DBusError		err;
	unsigned long	ruleIndex = 0,ret=ACL_RETURN_CODE_ERROR;
	ETHERADDR		smacAddr,maskMac;
	unsigned char	slot_no=0,port_no=0,mslot=0,mport=0;
	unsigned  int   m_g_index=0;
	unsigned  int	ruleType = 0,i = 0,rs=ACL_RETURN_CODE_ERROR,vlanId=0;
	unsigned int	eth_g_index = 0,actionType = 0;
	unsigned char	 devNum = 0,virPortNo=0,mdevNum,mvirPortNo=0;
	unsigned int     maskVlanId=0,maskVirPort=0;
	unsigned int	policer=0,policerId=0;

	struct ACCESS_LIST_STC *store = NULL;

	if(!((productinfo.capbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_ACL_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		store=(struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
		if(store==NULL)
		{
			rs=ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
		}
		else 
		{
			memset(store,0,sizeof(struct ACCESS_LIST_STC));
			memset(&smacAddr,0,sizeof(ETHERADDR));
			memset(&maskMac,0,sizeof(ETHERADDR));

			dbus_error_init(&err);

			if (!(dbus_message_get_args ( msg, &err,
											DBUS_TYPE_UINT32,&ruleIndex,
											DBUS_TYPE_UINT32,&ruleType,
											DBUS_TYPE_UINT32,&actionType,
											DBUS_TYPE_BYTE,  &mslot,
											DBUS_TYPE_BYTE,  &mport,
											DBUS_TYPE_BYTE,  &smacAddr.arEther[0],
											DBUS_TYPE_BYTE,  &smacAddr.arEther[1],
											DBUS_TYPE_BYTE,  &smacAddr.arEther[2],
											DBUS_TYPE_BYTE,  &smacAddr.arEther[3],
											DBUS_TYPE_BYTE,  &smacAddr.arEther[4],
											DBUS_TYPE_BYTE,  &smacAddr.arEther[5],
											DBUS_TYPE_BYTE,  &maskMac.arEther[0],
											DBUS_TYPE_BYTE,  &maskMac.arEther[1],
											DBUS_TYPE_BYTE,  &maskMac.arEther[2],
											DBUS_TYPE_BYTE,  &maskMac.arEther[3],
											DBUS_TYPE_BYTE,  &maskMac.arEther[4],
											DBUS_TYPE_BYTE,  &maskMac.arEther[5],	
											DBUS_TYPE_UINT32,&vlanId,
											DBUS_TYPE_BYTE,  &slot_no,
											DBUS_TYPE_BYTE,  &port_no, 
											DBUS_TYPE_UINT32,&policer,
											DBUS_TYPE_UINT32,&policerId,
											DBUS_TYPE_INVALID))) 
			{
				syslog_ax_acl_err("Unable to get input args ");
				if (dbus_error_is_set(&err)) 
				{
					syslog_ax_acl_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				store = NULL;
				return NULL;
			}
			
			if(vlanId!=0)
				maskVlanId =0xFF;
			
			ret = NPD_DBUS_ERROR_NO_SUCH_PORT;

			if (CHASSIS_SLOTNO_ISLEGAL(mslot)) 
			{
				if (ETH_LOCAL_PORTNO_ISLEGAL(mslot,mport)) 
				{
					m_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(mslot,mport);
					syslog_ax_acl_dbg("index for %d/%d is %#0x\n",mslot,mport,m_g_index);
					
					ret = npd_get_devport_by_global_index(m_g_index,&mdevNum,&mvirPortNo);
					if(ret != 0)
					{
						syslog_ax_acl_err("Npd_acl::npd_get_devport_by_global_index::get devport Error.\n");
						ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
						syslog_ax_acl_err("Npd_acl::npd_get_devport_by_global_index::get devport Error.\n");
					}
					else
					{
						syslog_ax_acl_dbg("npd_get_devport_by_global_index::mdevNum %d,mportNum %d\n",mdevNum,mvirPortNo);

						ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
						if((ACL_ANY_PORT_CHAR == slot_no)&&(ACL_ANY_PORT_CHAR == port_no)) 
						{
							maskVirPort = 0;
							virPortNo = 0;
							ret =ACL_RETURN_CODE_SUCCESS;
						}	
						else
						{
							if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) 
							{
								if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,port_no)) 
								{
									eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
									syslog_ax_acl_dbg("index for %d/%d is %#0x\n",slot_no,port_no,eth_g_index);
									
									ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortNo);
									if(ret != 0)
									{
										syslog_ax_acl_err("Npd_acl::npd_get_devport_by_global_index::get devport Error.\n");
										ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
									}
									else
									{
										syslog_ax_acl_dbg("npd_get_devport_by_global_index::devNum %d,portNum %d\n",devNum,virPortNo);
										ret =ACL_RETURN_CODE_SUCCESS;
										maskVirPort = 0xFF;
									}
								}
							}					
						}
				     }
			   }
			}
						
		   if(ACL_RETURN_CODE_SUCCESS==ret)
		   {	
				/*store acl rule*/
				store->ruleIndex=ruleIndex;
				store->ruleType=ruleType;
				if(3==actionType)
				{
				   store->ActionPtr.actionType=ACL_ACTION_TYPE_MIRROR_TO_ANALYZER; 
				   store->ActionPtr.actionInfo.mirror.portNum = mvirPortNo;
				}
				else if(4==actionType)
				{
					 store->ActionPtr.actionType=ACL_ACTION_TYPE_REDIRECT; 
					 store->ActionPtr.actionInfo.redirect.portNum = mvirPortNo;
					 store->ActionPtr.actionInfo.redirect.modid = devNum;
				}
				
				store->TcamPtr.ruleFormat=ACL_RULE_STD_IP_L2_QoS;
				store->TcamPtr.packetType=ACL_TCAM_RULE_PACKETTYPE_ARP;

				for(i=0;i<6;i++)
				{
				 store->TcamPtr.ruleInfo.stdNotIp.rule.sMac[i]=smacAddr.arEther[i]; 
				 store->TcamPtr.ruleInfo.stdNotIp.mask.maskSMac[i]=maskMac.arEther[i];
				}
				store->TcamPtr.ruleInfo.stdNotIp.rule.vlanId = vlanId;
				store->TcamPtr.ruleInfo.stdNotIp.mask.maskVlanId = maskVlanId;
				store->TcamPtr.ruleInfo.stdNotIp.rule.portno = virPortNo;
				store->TcamPtr.ruleInfo.stdNotIp.mask.maskPortno = maskVirPort;
				store->TcamPtr.ruleInfo.stdNotIp.rule.modid = devNum;
				store->ActionPtr.policerPtr.policerEnable = policer;
				store->ActionPtr.policerPtr.policerId = policerId;		
								
				syslog_ax_acl_dbg("add rule!\n");

				if(policer==1)
				{
					ret=npd_acl_policer_id_check(policerId);
					if(ret!=ACL_RETURN_CODE_SUCCESS)
					{
						syslog_ax_acl_err("policer id not existed!\n");
						rs=ACL_RETURN_CODE_POLICER_ID_NOT_SET;
						free(store);
					}
					else
					{
						rs=npd_acl_add_notIp(store);/*add rule	*/
						if(ACL_RETURN_CODE_SUCCESS!=rs)
							free(store);
					}
				}
				else if(policer==0)
				{
					rs=npd_acl_add_notIp(store);/*add rule	*/
					if(ACL_RETURN_CODE_SUCCESS!=rs)
						free(store);
				}
				else
				{
					/*code optimize:  Resource leak
					zhangcl@autelan.com 2013-1-17*/
					ret = ACL_RETURN_CODE_ERROR;
					free(store);
					store = NULL;
				}
			}
			else
		   	{
			   	/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				ret = ACL_RETURN_CODE_ERROR;
				free(store);
				store = NULL;
		   	}
		}	
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
								   DBUS_TYPE_UINT32,
								   &rs);
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,
									&ret);
	return reply;

}
DBusMessage * npd_dbus_config_ext_mirror_redirect(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{

	DBusMessage* 	reply = NULL;
	DBusMessageIter	iter;
	DBusError 		err;
	unsigned long   ruleIndex=0;
	unsigned long   dipno=0,sipno=0,sipmask=0,dipmask=0;
	unsigned long   dstport=0,srcport=0,masksrcport=0,maskdstport=0;
	unsigned int    actionType,ruleType,packetType,sipmaskLen=0,dipmaskLen=0;
	ETHERADDR		dmacAddr,smacAddr;
	unsigned char   slot_no=0,port_no=0,mirr_slot=0,mirr_port=0;
	unsigned int    eth_g_index = 0,mirr_g_index=0,vlanId=0;
	unsigned char	 i,devNum=0,virPortNo=0,mirr_virPortNo=0,mdevNum=0;	
	unsigned int    rs=ACL_RETURN_CODE_ERROR,ret=ACL_RETURN_CODE_ERROR;	
	unsigned long	ipAddr = 0;
	unsigned int    j,count1=0,count2=0,maskVirPort=0,maskVlanId=0;
	unsigned int    policer=0,policerId=0;
	struct ACCESS_LIST_STC *store =NULL;

	if(!((productinfo.capbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_ACL_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		store=(struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
		if(store==NULL) 
		{
			rs=ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
		}
		else 
		{
			memset(store,0,sizeof(struct  ACCESS_LIST_STC));
			memset(&smacAddr,0,sizeof(ETHERADDR));
			memset(&dmacAddr,0,sizeof(ETHERADDR));
				
			dbus_error_init(&err);

			if (!(dbus_message_get_args ( msg, &err,
									 DBUS_TYPE_UINT32,&ruleIndex,	
		               DBUS_TYPE_UINT32,&ruleType,
									 DBUS_TYPE_UINT32,&actionType,
									 DBUS_TYPE_BYTE,& mirr_slot,
									 DBUS_TYPE_BYTE,& mirr_port,
									 DBUS_TYPE_UINT32,&packetType,
									 DBUS_TYPE_UINT32,&dipno,
									 DBUS_TYPE_UINT32,&sipno,
									 DBUS_TYPE_UINT32,&dipmaskLen,
									 DBUS_TYPE_UINT32,&sipmaskLen,
									 DBUS_TYPE_UINT32,&dstport,
									 DBUS_TYPE_UINT32,&srcport,
									 DBUS_TYPE_BYTE,  &dmacAddr.arEther[0],
									 DBUS_TYPE_BYTE,  &dmacAddr.arEther[1],
									 DBUS_TYPE_BYTE,  &dmacAddr.arEther[2],
									 DBUS_TYPE_BYTE,  &dmacAddr.arEther[3],
									 DBUS_TYPE_BYTE,  &dmacAddr.arEther[4],
									 DBUS_TYPE_BYTE,  &dmacAddr.arEther[5],
									 DBUS_TYPE_BYTE,  &smacAddr.arEther[0],
									 DBUS_TYPE_BYTE,  &smacAddr.arEther[1],
									 DBUS_TYPE_BYTE,  &smacAddr.arEther[2],
									 DBUS_TYPE_BYTE,  &smacAddr.arEther[3],
									 DBUS_TYPE_BYTE,  &smacAddr.arEther[4],
									 DBUS_TYPE_BYTE,  &smacAddr.arEther[5],						 
									 DBUS_TYPE_UINT32,&vlanId,
									 DBUS_TYPE_BYTE,  &slot_no,
									 DBUS_TYPE_BYTE,  &port_no,
									 DBUS_TYPE_UINT32,&policer,
									 DBUS_TYPE_UINT32,&policerId,
				                     DBUS_TYPE_INVALID))) 
			{
				syslog_ax_acl_err("Unable to get input args ");
				if (dbus_error_is_set(&err)) 
				{
					syslog_ax_acl_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
				/*code optimize:  Resource leak. zhangcl@autelan.com 2013-1-17*/
				free(store);
				store = NULL;
				return NULL;
			}

			if(vlanId!=0)
				maskVlanId=0xFF;

			for(j=0;j<6;j++)
			{
				if(dmacAddr.arEther[j]==0)
					count1+=1;
				if(smacAddr.arEther[j]==0)
					count2+=1;
			}
				
			npd_acl_len2mask(sipmaskLen,&sipmask);
			npd_acl_len2mask(dipmaskLen,&dipmask);
					
			if(ACL_ANY_PORT==srcport) 
			{
				srcport = 0;
		           masksrcport=0;
			}
			else 
			{          
		           masksrcport=65535;
			}
		    if(ACL_ANY_PORT==dstport) 
			{
				dstport = 0;
				maskdstport=0;
		    } 
			else 
			{          
		        maskdstport=65535;
			}
			ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
				
		    if (CHASSIS_SLOTNO_ISLEGAL(mirr_slot)) 
			{
				if (ETH_LOCAL_PORTNO_ISLEGAL(mirr_slot,mirr_port)) 
				{
					mirr_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(mirr_slot,mirr_port);
					syslog_ax_acl_dbg("index for %d/%d is %#0x\n",mirr_slot,mirr_port,mirr_g_index);
					
					ret = npd_get_devport_by_global_index(mirr_g_index,&mdevNum,&mirr_virPortNo);
					if(ret != 0)
					{
						syslog_ax_acl_err("Npd_acl::npd_get_devport_by_global_index::get devport Error.\n");
						ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
						syslog_ax_acl_err("Npd_acl::npd_get_devport_by_global_index::get devport Error.\n");
					}
					else
					{
						syslog_ax_acl_dbg("npd_get_devport_by_global_index::mirr_devNum %d,mirr_portNum %d\n",mdevNum,mirr_virPortNo);
						syslog_ax_acl_dbg("npd_get_devport_by_global_index::mirr_devNum %d,mirr_portNum %d\n",mdevNum,mirr_virPortNo);
					

						ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
						
						if((ACL_ANY_PORT_CHAR == slot_no)&&(ACL_ANY_PORT_CHAR == port_no)) 
						{
							maskVirPort = 0;
							virPortNo = 0;
							ret =ACL_RETURN_CODE_SUCCESS;
						}
						else 
						{
							if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) 
							{
								if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,port_no)) 
								{
									eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
									syslog_ax_acl_dbg("index for %d/%d is %#0x\n",slot_no,port_no,eth_g_index);
									
									ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortNo);
									if(ret != 0)
									{
										syslog_ax_acl_dbg("Npd_acl::npd_get_devport_by_global_index::get devport Error.\n");
										ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
									}
									else
									{
										syslog_ax_acl_dbg("npd_get_devport_by_global_index::devNum %d,portNum %d\n",devNum,virPortNo);
										ret =ACL_RETURN_CODE_SUCCESS;
										maskVirPort =0xFF;
									}
								}
							}
						}
						 
					}
				}
			}

			if(ACL_RETURN_CODE_SUCCESS==ret)
			{
				if(4==actionType)
				{
					store->ActionPtr.actionType=ACL_ACTION_TYPE_REDIRECT; 
					store->ActionPtr.actionInfo.redirect.portNum = mirr_virPortNo;
					store->ActionPtr.actionInfo.redirect.modid = devNum;
				}
					
			    if(1==packetType)
			        store->TcamPtr.packetType=ACL_TCAM_RULE_PACKETTYPE_UDP;
			    else if(2==packetType)
			         store->TcamPtr.packetType=ACL_TCAM_RULE_PACKETTYPE_TCP;
			  
			   
				store->ruleIndex=ruleIndex;
				store->ruleType = EXTENDED_ACL_RULE;  
				store->TcamPtr.ruleFormat=ACL_RULE_EXT_NOT_IPv6;
				store->TcamPtr.ruleInfo.extNotIpv6.rule.Dip=dipno;
				store->TcamPtr.ruleInfo.extNotIpv6.rule.Sip=sipno;
				store->TcamPtr.ruleInfo.extNotIpv6.rule.DstPort=dstport;
				store->TcamPtr.ruleInfo.extNotIpv6.rule.SrcPort=srcport;
				store->TcamPtr.ruleInfo.extNotIpv6.mask.maskDip=dipmask;
				store->TcamPtr.ruleInfo.extNotIpv6.mask.maskSip=sipmask;
				store->TcamPtr.ruleInfo.extNotIpv6.mask.maskLenDip=dipmaskLen;
				store->TcamPtr.ruleInfo.extNotIpv6.mask.maskLenSip=sipmaskLen;
				store->TcamPtr.ruleInfo.extNotIpv6.mask.maskSrcPort=masksrcport;
				store->TcamPtr.ruleInfo.extNotIpv6.mask.maskDstPort=maskdstport;
				for(i=0;i<6;i++)
				{
					store->TcamPtr.ruleInfo.extNotIpv6.rule.dMac[i]=dmacAddr.arEther[i];
					store->TcamPtr.ruleInfo.extNotIpv6.rule.sMac[i]=smacAddr.arEther[i];	
					if(count1!=6)
				 		store->TcamPtr.ruleInfo.extNotIpv6.mask.maskDmac[i]=0xFF;
					if(count2!=6)
				   		store->TcamPtr.ruleInfo.extNotIpv6.mask.maskSmac[i]=0xFF;
				}
				store->TcamPtr.ruleInfo.extNotIpv6.rule.vlanId = vlanId;
				store->TcamPtr.ruleInfo.extNotIpv6.mask.maskVlanId =maskVlanId;
			   	store->TcamPtr.ruleInfo.extNotIpv6.rule.portNo = virPortNo;
				store->TcamPtr.ruleInfo.extNotIpv6.mask.maskPortNo = maskVirPort;
				store->TcamPtr.ruleInfo.extNotIpv6.rule.modid = devNum;
				store->ActionPtr.policerPtr.policerEnable = policer;
				store->ActionPtr.policerPtr.policerId = policerId;	

				syslog_ax_acl_dbg("rule info\n");
				syslog_ax_acl_dbg("***********************************************\n");
				syslog_ax_acl_dbg("%-15s:%ld\n","index",ruleIndex);	
				syslog_ax_acl_dbg("%-15s:%-s\n","Packet Type",packetTypeStr[store->TcamPtr.packetType]);
				syslog_ax_acl_dbg("%-15s:%-s\n","Action Type",actionTypeStr[store->ActionPtr.actionType]);
				ipAddr = store->TcamPtr.ruleInfo.extNotIpv6.rule.Dip;
				syslog_ax_acl_dbg("%-15s:%ld.%ld.%ld.%ld/%ld\n","Dest IP",(ipAddr>>24)&0xFF,(ipAddr>>16)&0xFF, \
						(ipAddr>>8)&0xFF,ipAddr&0xFF,dipmaskLen);
				ipAddr = store->TcamPtr.ruleInfo.extNotIpv6.rule.Sip;
				syslog_ax_acl_dbg("%-15s:%ld.%ld.%ld.%ld/%ld\n","Src IP",(ipAddr>>24)&0xFF,(ipAddr>>16)&0xFF,	\
						(ipAddr>>8)&0xFF,ipAddr&0xFF,sipmaskLen);
				 syslog_ax_acl_dbg("acl dmac addr %0x:%0x:%0x:%0x:%0x:%0x.\n",dmacAddr.arEther[0],
															dmacAddr.arEther[1],
															dmacAddr.arEther[2],
															dmacAddr.arEther[3],
															dmacAddr.arEther[4],
															dmacAddr.arEther[5]);
				syslog_ax_acl_dbg("acl smac addr %0x:%0x:%0x:%0x:%0x:%0x.\n",smacAddr.arEther[0],
															smacAddr.arEther[1],
															smacAddr.arEther[2],
															smacAddr.arEther[3],
															smacAddr.arEther[4],
															smacAddr.arEther[5]);
				syslog_ax_acl_dbg("vlan id %d\n",store->TcamPtr.ruleInfo.extNotIpv6.rule.vlanId);
				syslog_ax_acl_dbg("port %d\n",store->TcamPtr.ruleInfo.extNotIpv6.rule.portNo);											
				
				syslog_ax_acl_dbg("***********************************************\n");
					
				syslog_ax_acl_dbg("add rule!\n");

				if(policer==1)
				{
					ret=npd_acl_policer_id_check(policerId);
					if(ret!=ACL_RETURN_CODE_SUCCESS)
					{
						syslog_ax_acl_err("policer id not existed!\n");
						rs=ACL_RETURN_CODE_POLICER_ID_NOT_SET;
						free(store);
					}
					else
					{
						rs=npd_acl_add_extNotIpv6(store);/*add rule	*/
						if(ACL_RETURN_CODE_SUCCESS!=rs)
							free(store);
					}
				}
				else if(policer==0)
				{
					rs=npd_acl_add_extNotIpv6(store);/*add rule	*/
					if(ACL_RETURN_CODE_SUCCESS!=rs)
						free(store);
				}
				else
				{
					/*code optimize:  Resource leak
					zhangcl@autelan.com 2013-1-17*/
					ACL_RETURN_CODE_ERROR;
					free(store);
					store = NULL;
				}
		
			}
			else
			{
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				ACL_RETURN_CODE_ERROR;
				free(store);
				store = NULL;
			}
		}
	}
    reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&rs);
	return reply;

}

DBusMessage * npd_dbus_config_ext_permit_deny_trap(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage* 	reply = NULL;
	DBusMessageIter	iter;
	DBusError 		err;
	unsigned long   ruleIndex=0;
	unsigned long   dipno=0,sipno=0,sipmask=0,dipmask=0;
	unsigned long   dstport=0,srcport=0,masksrcport=0,maskdstport=0;
	unsigned int    actionType = 0,ruleType = 0,packetType = 0,sipmaskLen=0,dipmaskLen=0;
	ETHERADDR		dmacAddr,smacAddr;
	unsigned char   slot_no=0,port_no=0;
	unsigned int    i = 0,eth_g_index = 0,vlanId=0;
	/*code optimize:  Uninitialized scalar variable
	zhangcl@autelan.com 2013-1-17*/
	unsigned char	 devNum = 0,virPortNo=0;	
	unsigned int    rs=ACL_RETURN_CODE_ERROR,ret=ACL_RETURN_CODE_ERROR;	
	unsigned int    j=0, count1=0,count2=0,maskVlanId=0,maskVirPort=0;
	unsigned int    policer=0,policerId=0;
	struct ACCESS_LIST_STC *store =NULL;

	if(!((productinfo.capbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_ACL_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		store=(struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
		if(store==NULL) 
		{
			rs=ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
		}
		else 
		{
			memset(store,0,sizeof(struct  ACCESS_LIST_STC)); 
			memset(&smacAddr,0,sizeof(ETHERADDR));
			memset(&dmacAddr,0,sizeof(ETHERADDR));
					
			dbus_error_init(&err);

			if (!(dbus_message_get_args ( msg, &err,
										DBUS_TYPE_UINT32,&ruleIndex,	
		                             DBUS_TYPE_UINT32,&ruleType,
									 DBUS_TYPE_UINT32,&actionType,
									 DBUS_TYPE_UINT32,&packetType,
									 DBUS_TYPE_UINT32,&dipno,
									 DBUS_TYPE_UINT32,&sipno,
									 DBUS_TYPE_UINT32,&dipmaskLen,
									 DBUS_TYPE_UINT32,&sipmaskLen,
									 DBUS_TYPE_UINT32,&dstport,
									 DBUS_TYPE_UINT32,&srcport,
									 DBUS_TYPE_BYTE,  &dmacAddr.arEther[0],
									 DBUS_TYPE_BYTE,  &dmacAddr.arEther[1],
									 DBUS_TYPE_BYTE,  &dmacAddr.arEther[2],
									 DBUS_TYPE_BYTE,  &dmacAddr.arEther[3],
									 DBUS_TYPE_BYTE,  &dmacAddr.arEther[4],
									 DBUS_TYPE_BYTE,  &dmacAddr.arEther[5],
									 DBUS_TYPE_BYTE,  &smacAddr.arEther[0],
									 DBUS_TYPE_BYTE,  &smacAddr.arEther[1],
									 DBUS_TYPE_BYTE,  &smacAddr.arEther[2],
									 DBUS_TYPE_BYTE,  &smacAddr.arEther[3],
									 DBUS_TYPE_BYTE,  &smacAddr.arEther[4],
									 DBUS_TYPE_BYTE,  &smacAddr.arEther[5],
									 DBUS_TYPE_UINT32,&vlanId,
									 DBUS_TYPE_BYTE,  &slot_no,
									 DBUS_TYPE_BYTE,  &port_no,
									 DBUS_TYPE_UINT32,&policer,
									 DBUS_TYPE_UINT32,&policerId,
									 DBUS_TYPE_INVALID))) 
			{
				syslog_ax_acl_err("Unable to get input args ");
				if (dbus_error_is_set(&err)) 
				{
					syslog_ax_acl_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				store = NULL;
				return NULL;
			}
			
			for(j=0;j<6;j++)
			{
				if(dmacAddr.arEther[j]==0)
					count1+=1;
				if(smacAddr.arEther[j]==0)
					count2+=1;
			}

			if(vlanId!=0)
				maskVlanId=0xFF;
			
			npd_acl_len2mask(sipmaskLen,&sipmask);
			npd_acl_len2mask(dipmaskLen,&dipmask);
					
			if(ACL_ANY_PORT==srcport) 
			{
				srcport = 0;
		        masksrcport = 0;
			}
			else 
			{          
		        masksrcport=65535;
			}
		    if(ACL_ANY_PORT==dstport) 
			{
				dstport = 0;
		        maskdstport=0;
		    } 
			else 
			{          
		        maskdstport=65535;
			}
			
			ret = NPD_DBUS_ERROR_NO_SUCH_PORT;

			if((ACL_ANY_PORT_CHAR == slot_no)&&(ACL_ANY_PORT_CHAR == port_no)) 
			{
				maskVirPort = 0;
				virPortNo = 0;
				ret =ACL_RETURN_CODE_SUCCESS;
			}
			else
			{
				if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) 
				{
					if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,port_no)) 
					{
						eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
						syslog_ax_acl_dbg("index for %d/%d is %#0x\n",slot_no,port_no,eth_g_index);
						ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortNo);
						if(ret != 0)
						{
							syslog_ax_acl_err("Npd_dbus::npd_get_devport_by_global_index::get devport Error.\n");
							ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
						}
						else
						{
							syslog_ax_acl_dbg("npd_get_devport_by_global_index::devNum %d,portNum %d\n",devNum,virPortNo);				
							ret = ACL_RETURN_CODE_SUCCESS;
							maskVirPort =0xFF;
							syslog_ax_acl_dbg("Npd_dbus::npd_get_devport_by_global_index::get devport Error.\n");
						}
					}
				}
			}
			
			if(ACL_RETURN_CODE_SUCCESS==ret)
			{
				if(0==actionType)
			        store->ActionPtr.actionType=ACL_ACTION_TYPE_PERMIT;	
				else if(1==actionType)
					store->ActionPtr.actionType=ACL_ACTION_TYPE_DENY;
			    else if(2==actionType)
				{
			         store->ActionPtr.actionType=ACL_ACTION_TYPE_TRAP_TO_CPU;
					 store->ActionPtr.actionInfo.trap.cpucode=500;
				}
					
			    if(1==packetType)
			        store->TcamPtr.packetType=ACL_TCAM_RULE_PACKETTYPE_UDP;
			    else if(2==packetType)
			         store->TcamPtr.packetType=ACL_TCAM_RULE_PACKETTYPE_TCP;
			  
			   
				store->ruleIndex=ruleIndex;
				store->ruleType = EXTENDED_ACL_RULE;  
				store->TcamPtr.ruleFormat=ACL_RULE_EXT_NOT_IPv6;
				store->TcamPtr.ruleInfo.extNotIpv6.rule.Dip=dipno;
				store->TcamPtr.ruleInfo.extNotIpv6.rule.Sip=sipno;
				store->TcamPtr.ruleInfo.extNotIpv6.rule.DstPort=dstport;
				store->TcamPtr.ruleInfo.extNotIpv6.rule.SrcPort=srcport;
				store->TcamPtr.ruleInfo.extNotIpv6.mask.maskDip=dipmask;
				store->TcamPtr.ruleInfo.extNotIpv6.mask.maskSip=sipmask;
				store->TcamPtr.ruleInfo.extNotIpv6.mask.maskLenDip=dipmaskLen;
				store->TcamPtr.ruleInfo.extNotIpv6.mask.maskLenSip=sipmaskLen;
				store->TcamPtr.ruleInfo.extNotIpv6.mask.maskSrcPort=masksrcport;
				store->TcamPtr.ruleInfo.extNotIpv6.mask.maskDstPort=maskdstport;
				for(i=0;i<6;i++)
				{
					store->TcamPtr.ruleInfo.extNotIpv6.rule.dMac[i]=dmacAddr.arEther[i];
					store->TcamPtr.ruleInfo.extNotIpv6.rule.sMac[i]=smacAddr.arEther[i];	
					if(count1!=6)
				 		store->TcamPtr.ruleInfo.extNotIpv6.mask.maskDmac[i]=0xFF;
					if(count2!=6)
						store->TcamPtr.ruleInfo.extNotIpv6.mask.maskSmac[i]=0XFF;
				}
				store->TcamPtr.ruleInfo.extNotIpv6.rule.vlanId = vlanId;
				store->TcamPtr.ruleInfo.extNotIpv6.mask.maskVlanId = maskVlanId;
			    store->TcamPtr.ruleInfo.extNotIpv6.rule.portNo = virPortNo;
				store->TcamPtr.ruleInfo.extNotIpv6.mask.maskPortNo = maskVirPort;
				store->TcamPtr.ruleInfo.extNotIpv6.rule.modid = devNum;
				store->ActionPtr.policerPtr.policerEnable = policer;
				store->ActionPtr.policerPtr.policerId = policerId;	

				if(policer==1)
				{
					ret=npd_acl_policer_id_check(policerId);
					if(ret!=ACL_RETURN_CODE_SUCCESS)
					{
						syslog_ax_acl_err("policer id not existed!\n");
						rs=ACL_RETURN_CODE_POLICER_ID_NOT_SET;
						free(store);
					}
					else
					{
						rs=npd_acl_add_extNotIpv6(store);/*add rule*/	
						if(ACL_RETURN_CODE_SUCCESS!=rs)
							free(store);
					}
				}
				else if(policer==0)
				{
					rs=npd_acl_add_extNotIpv6(store);/*add rule	*/
					if(ACL_RETURN_CODE_SUCCESS!=rs)
						free(store);
				}
				else
				{
					/*code optimize:  Resource leak
					zhangcl@autelan.com 2013-1-17*/
					free(store);
					store = NULL;
				}
			}
			else
			{
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				store = NULL;
			}
		}
	}
    reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&rs);
	return reply;

}

DBusMessage * npd_dbus_config_qos_base_acl_ingress(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage* 	reply = NULL;
	DBusMessageIter	iter;
	DBusError 		err;
	unsigned long   ruleIndex=0,ruleType=0; 
	unsigned int    profileIndex=0,up=0,dscp=0,rs=ACL_RETURN_CODE_ERROR;
	struct ACCESS_LIST_STC *store = NULL;
	unsigned int    policer=0,policerId=0;
	unsigned int    precedence=0,ret=0;

	if(!((productinfo.capbmp) & FUNCTION_QOS_VLAUE))
	{
		npd_syslog_dbg("do not support qos!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_QOS_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_QOS_VLAUE))
	{
		npd_syslog_dbg("do not support qos!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		store=(struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
		if(store==NULL) 
		{
			rs=ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
		}
		else 
		{
			memset(store,0,sizeof(struct ACCESS_LIST_STC));
			dbus_error_init(&err);
			
			if (!(dbus_message_get_args ( msg, &err,
										  DBUS_TYPE_UINT32,&ruleIndex,
										  DBUS_TYPE_UINT32,&ruleType,
										  DBUS_TYPE_UINT32,&profileIndex,
										  DBUS_TYPE_UINT32,&up,
										  DBUS_TYPE_UINT32,&dscp,
										  DBUS_TYPE_UINT32,&policer,
										  DBUS_TYPE_UINT32,&policerId,
										  DBUS_TYPE_UINT32,&precedence,
										  DBUS_TYPE_INVALID))) 
			{
				syslog_ax_acl_err("Unable to get input args ");
				if (dbus_error_is_set(&err)) 
				{
					syslog_ax_acl_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				store = NULL;
				return NULL;
			}	
			rs = npd_qos_profile_index_check(profileIndex);
			if(rs!=0) 
			{
				syslog_ax_acl_dbg("qos profile not exiseted\n");
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				store = NULL;
			}
			else 
			{
				if ((qos_mode != QOS_MODE_FLOW)&&(qos_mode != QOS_MODE_HYBRID)) 
				{
					if (qos_mode == QOS_MODE_DEFAULT) 
					{
						rs = ACL_RETURN_CODE_NO_QOS_MODE;
					}
					else 
					{
						rs = ACL_RETURN_CODE_ALREADY_PORT;
					}	
					/*code optimize:  Resource leak
					zhangcl@autelan.com 2013-1-17*/
					free(store);
					store = NULL;
				}
				else if ((qos_mode == QOS_MODE_HYBRID)&&(profileIndex < (MAX_HYBRID_UP+1))) 
				{
					rs = ACL_RETURN_CODE_HYBRID_FLOW;
					/*code optimize:  Resource leak
					zhangcl@autelan.com 2013-1-17*/
					free(store);
					store = NULL;
				}
				else
				{
					store->ruleIndex=ruleIndex;
					store->ruleType=ruleType;
					store->ActionPtr.actionType=ACL_ACTION_TYPE_INGRESS_QOS;
				      store->TcamPtr.ruleFormat=ACL_RULE_QOS;
					store->TcamPtr.packetType=ACL_TCAM_RULE_PACKETTYPE_QOS;
					store->ActionPtr.actionInfo.qos.qos.ingress.profileIndex=profileIndex;
					store->ActionPtr.actionInfo.qos.qos.ingress.profileAssignIndex = ACL_TRUE;
					store->ActionPtr.actionInfo.qos.qos.ingress.profilePrecedence = (QOS_ATTRIBUTE_ASSIGN_PRECEDENCE_E)precedence;
					store->ActionPtr.policerPtr.policerEnable = policer;
					store->ActionPtr.policerPtr.policerId = policerId;	
					if(up==10)
					{
						syslog_ax_acl_dbg("keep previous up !");
						store->ActionPtr.actionInfo.qos.modifyUp=KEEP_E;
						store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskUP = 0;
					}
					else 
					{
						store->TcamPtr.ruleInfo.stdIpv4L4.rule.UP = up;
						store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskUP = 0x7;
						store->ActionPtr.actionInfo.qos.modifyUp = ENABLE_E;
					}
					if(dscp ==100)
					{
						syslog_ax_acl_dbg("keep previous dscp");
						store->ActionPtr.actionInfo.qos.modifyDscp= KEEP_E;
					    store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDSCP = 0;
					}
					else
					{
						store->TcamPtr.ruleInfo.stdIpv4L4.rule.DSCP = dscp;
					    store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDSCP = 0x3F;
						store->ActionPtr.actionInfo.qos.modifyDscp = ENABLE_E;
					}
					if(policer==1)
					{
						ret=npd_acl_policer_id_check(policerId);
						if(ret!=ACL_RETURN_CODE_SUCCESS)
						{
							syslog_ax_acl_err("policer id not existed!\n");
							rs=ACL_RETURN_CODE_POLICER_ID_NOT_SET;
							free(store);
						}
						else
						{
							rs=npd_acl_qos_add(store);/*add rule*/	
							if(ACL_RETURN_CODE_SUCCESS!=rs)
							{
								free(store);
							}
						}
					}
					else if(policer==0)
					{
						rs=npd_acl_qos_add(store);/*add rule	*/
						if(ACL_RETURN_CODE_SUCCESS!=rs)
						{
							free(store);
						}
					}		
					else
					{
						/*code optimize:  Resource leak
						zhangcl@autelan.com 2013-1-17*/
						free(store);
						store = NULL;
					}
				}
			}
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&rs);
	return reply;
}

DBusMessage * npd_dbus_config_qos_base_acl_egress(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage*	reply = NULL;
	DBusMessageIter iter;
	DBusError		err;
	unsigned long	ruleIndex=0,ruleType=0; 
	unsigned int	egrUp=0,egrDscp=0,up=0,dscp=0,rs=ACL_RETURN_CODE_SUCCESS;
	struct ACCESS_LIST_STC *store = NULL;

	if(!((productinfo.capbmp) & FUNCTION_QOS_VLAUE))
	{
		npd_syslog_dbg("do not support qos!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_QOS_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_QOS_VLAUE))
	{
		npd_syslog_dbg("do not support qos!\n");
		rs = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
		store=(struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
		if(store==NULL) 
		{
			rs=ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
		}
		else 
		{
			memset(store,0,sizeof(struct ACCESS_LIST_STC));	
			dbus_error_init(&err);
			
			if (!(dbus_message_get_args ( msg, &err,
										  DBUS_TYPE_UINT32,&ruleIndex,
										  DBUS_TYPE_UINT32,&ruleType,
										  DBUS_TYPE_UINT32,&egrUp,
										  DBUS_TYPE_UINT32,&egrDscp,
										  DBUS_TYPE_UINT32,&up,
										  DBUS_TYPE_UINT32,&dscp,							
										  DBUS_TYPE_INVALID))) 
			{
				syslog_ax_acl_err("Unable to get input args ");
				if (dbus_error_is_set(&err)) 
				{
					syslog_ax_acl_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				store = NULL;
				return NULL;
			 }
			if ((qos_mode != QOS_MODE_FLOW)&&(qos_mode != QOS_MODE_HYBRID)) 
			{
				if (qos_mode == QOS_MODE_DEFAULT) 
				{
					rs = ACL_RETURN_CODE_NO_QOS_MODE;
				}
				else 
				{
					rs = ACL_RETURN_CODE_ALREADY_PORT;
				}	
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-17*/
				free(store);
				store = NULL;

			}	
			else 
			{
				syslog_ax_acl_dbg("ruleindex is  %d\n",ruleIndex);
				store->ruleIndex=ruleIndex;
				store->ruleType=ruleType;
				store->ActionPtr.actionType=ACL_ACTION_TYPE_EGRESS_QOS;
				store->TcamPtr.ruleFormat=ACL_RULE_QOS;
				store->TcamPtr.packetType=ACL_TCAM_RULE_PACKETTYPE_QOS;
				store->ActionPtr.actionInfo.qos.qos.egress.egrDscp = egrDscp;
				store->ActionPtr.actionInfo.qos.qos.egress.egrUp = egrUp;

				if(up==10)
				{
					syslog_ax_acl_dbg("keep previous up !");
					store->ActionPtr.actionInfo.qos.modifyUp=KEEP_E;
					store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskUP = 0;
				}
				else 
				{
					store->TcamPtr.ruleInfo.stdIpv4L4.rule.UP = up;
					store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskUP = 0x7;
					store->ActionPtr.actionInfo.qos.modifyUp = ENABLE_E;
				}
				if(dscp ==100)
				{
					syslog_ax_acl_dbg("keep previous dscp");
					store->ActionPtr.actionInfo.qos.modifyDscp = KEEP_E;
					store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDSCP= 0;
				}
				else
				{
					store->TcamPtr.ruleInfo.stdIpv4L4.rule.DSCP = dscp;
					store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDSCP = 0x3F;
					store->ActionPtr.actionInfo.qos.modifyDscp = ENABLE_E;
				}
				syslog_ax_acl_dbg("ruleindex is  %d\n",ruleIndex);		
				rs=npd_acl_qos_egrAdd(store);	/*add rule */
				if(ACL_RETURN_CODE_SUCCESS!=rs)
				{
					free(store);
				}
			}
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&rs);
	return reply;
}

DBusMessage * npd_dbus_show_accesslist_index(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage*			reply = NULL;
	DBusMessageIter 			iter;
	DBusError				err;		
	int 						ret=ACL_RETURN_CODE_SUCCESS,i=0,k = 0,l = 0;
	char						protype[10];
	ACL_SHOW_STC			*AclList=NULL;
	ACL_RULE_STDIPV4L4_T	*stdIpv4L4=NULL;
	ACL_RULE_STDNOTIP_T 	*stdNotIp = NULL;
	ACL_RULE_EXTNOTIPV6_T	*extNotIpv6=NULL;
	ACL_RULE_EXTIPV6L4_T	*extIpv6=NULL;
	AclRuleList 				*aclNodePtr=NULL;
	unsigned int				 sourceport=0,sourceportmask=0,mirrorport=0,redirectport=0;
	ACL_TCAM_RULE_PACKETTYPE_E pType = ACL_TCAM_RULE_PACKETTYPE_MAX;/*packet type*/
	unsigned int rType = 0;/* rule type */
	ACL_ACTION_TYPE_E aType = ACL_ACTION_TYPE_MAX;/* action type*/
	unsigned int retVal = 0, modid = 0;
	unsigned int slot_no = 0, port_no = 0;

	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
								  DBUS_TYPE_UINT32,&i,						
								  DBUS_TYPE_INVALID))) 
	{
		npd_syslog_err(("Unable to get input args "));
		if (dbus_error_is_set(&err))
		{
			npd_syslog_err(("%s raised: %s",err.name,err.message));
			dbus_error_free(&err);
		}
		return NULL;
	}	
	AclList = (ACL_SHOW_STC*)malloc(sizeof(ACL_SHOW_STC));
	if(NULL==AclList)
	{
		syslog_ax_acl_err("get memory fail for acl rule show buffer\n");
		return NULL;
	}
	memset(AclList,0,sizeof(ACL_SHOW_STC));
 
	if(NULL==g_acl_rule[i])
	{
		ret=ACL_RETURN_CODE_ERROR;
		syslog_ax_acl_err("acl rule %d not found\n",i);
	}
	else 
	{
		pType=g_acl_rule[i]->TcamPtr.packetType;
		rType = g_acl_rule[i]->ruleType;
		aType = g_acl_rule[i]->ActionPtr.actionType;
		aclNodePtr = g_acl_rule[i];
		  
		switch(pType)
		{
			case 0:strcpy(protype,"IP");		break;
			case 1:strcpy(protype,"UDP");	break;
			case 2:strcpy(protype,"TCP");	break;
			case 3:strcpy(protype,"ICMP");	break;
			case 4:strcpy(protype,"ethernet");break;
			case 5:strcpy(protype,"arp");	break;
			case 6:strcpy(protype,"QoS");	break;					
			default: break; 								
		}
		syslog_ax_acl_dbg("show acl rule %d type %s\n",		\
				i, (STANDARD_ACL_RULE==rType) ? "standard":"extended");
		
		AclList->ruleIndex =(aclNodePtr->ruleIndex+1);
		AclList->ruleType = rType;
		AclList->actionType = aType;
		AclList->packetType = pType;
		AclList->policer   =aclNodePtr->ActionPtr.policerPtr.policerEnable;
		AclList->policerId =aclNodePtr->ActionPtr.policerPtr.policerId;

		stdIpv4L4  = &(aclNodePtr->TcamPtr.ruleInfo.stdIpv4L4);
		stdNotIp	= &(aclNodePtr->TcamPtr.ruleInfo.stdNotIp);
		extNotIpv6	= &(aclNodePtr->TcamPtr.ruleInfo.extNotIpv6);
		extIpv6 = &(aclNodePtr->TcamPtr.ruleInfo.extIpv6L4);
		
		/* standard acl rule*/
		/*if(STANDARD_ACL_RULE == rType) {*/
		if((ACL_TCAM_RULE_PACKETTYPE_IP == pType) ||
				(ACL_TCAM_RULE_PACKETTYPE_ICMP == pType) ) 
		{
			AclList->dip	   	=stdIpv4L4->rule.Dip;
			AclList->sip	   	=stdIpv4L4->rule.Sip;
			AclList->maskdip	=stdIpv4L4->mask.maskLenDip;
			AclList->masksip	=stdIpv4L4->mask.maskLenSip;
			AclList->srcport	=stdIpv4L4->rule.SrcPort;
			AclList->dstport	=stdIpv4L4->rule.DstPort;
			AclList->icmp_code = stdIpv4L4->rule.Code;
			AclList->code_mask = stdIpv4L4->mask.maskCode;
			AclList->icmp_type	= stdIpv4L4->rule.Type;
			AclList->type_mask	= stdIpv4L4->mask.maskType;

			if(ACL_ACTION_TYPE_REDIRECT== aType)
			{					
				redirectport = aclNodePtr->ActionPtr.actionInfo.redirect.portNum;
				modid = aclNodePtr->ActionPtr.actionInfo.redirect.modid;
				retVal = nam_get_slotport_by_devport(modid,redirectport,PRODUCT_ID,&slot_no,&port_no);
				if(retVal!=0) 
				{
					syslog_ax_acl_err("show acl rule redirect port %#x convert error!\n",redirectport);
					ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
				}
				else
				{
					syslog_ax_acl_dbg("show acl rule %d get redirect port %d/%d \n", i,slot_no,port_no);
					AclList->redirectslot=(unsigned char)slot_no;
					AclList->redirectport =(unsigned char)port_no;
				}
			}
			
			if(ACL_ACTION_TYPE_MIRROR_TO_ANALYZER== aType)
			{ 				
				mirrorport = aclNodePtr->ActionPtr.actionInfo.mirror.portNum;
				modid = aclNodePtr->ActionPtr.actionInfo.mirror.modid;
				retVal=nam_get_slotport_by_devport(modid,mirrorport,PRODUCT_ID,&slot_no,&port_no);
				if(retVal!=0) 
				{
					syslog_ax_acl_err("show acl rule mirror target port %#x convert error!\n",redirectport);
					ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
				}
				else
				{
					syslog_ax_acl_dbg("show acl rule %d get mirror target port %d/%d \n", i, slot_no ,port_no);
					AclList->mirrorslot=(unsigned char)slot_no;
					AclList->mirrorport=(unsigned char)port_no;
				}
			}

			/* append info*/
			if(32==(aclNodePtr->appendInfo.appendType)) 
			{
				AclList->appendInfo =32;
				AclList->qosprofileindex=aclNodePtr->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
			}
		}	
		else if((ACL_TCAM_RULE_PACKETTYPE_ETHERNET == pType) ||
				(ACL_TCAM_RULE_PACKETTYPE_ARP == pType)) 
		{					   					
			for(k=0;k<6;k++)
			{
				AclList->dmac[k] = stdNotIp->rule.dMac[k];
				AclList->smac[k] = stdNotIp->rule.sMac[k];
			}
				
			AclList->vlanid = stdNotIp->rule.vlanId;
			sourceport=stdNotIp->rule.portno;
			sourceportmask = stdNotIp->mask.maskPortno;
			modid = stdNotIp->rule.modid;
			if(0 != sourceportmask)
			{
				retVal=nam_get_slotport_by_devport(modid,sourceport,PRODUCT_ID,&slot_no,&port_no);
				if(retVal!=0) 
				{
					syslog_ax_acl_err("show acl rule %d convert source port %#x error \n", i, sourceport);
					free(AclList);
					ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
				}
				else
				{
					syslog_ax_acl_dbg("show acl rule %d get source port %d/%d \n", i, slot_no ,port_no);
					AclList->sourceslot = slot_no;
					AclList->sourceport = port_no;
				}
			}
			else 
			{
					AclList->sourceslot = ACL_ANY_PORT_CHAR;
					AclList->sourceport = ACL_ANY_PORT_CHAR;
			}

			if((ACL_ACTION_TYPE_REDIRECT == aType) && (ret == ACL_RETURN_CODE_SUCCESS))
			{					
				redirectport=aclNodePtr->ActionPtr.actionInfo.redirect.portNum;
				modid = aclNodePtr->ActionPtr.actionInfo.redirect.modid;
				retVal=nam_get_slotport_by_devport(modid,redirectport,PRODUCT_ID,&slot_no,&port_no);
				if(retVal!=0) 
				{
					syslog_ax_acl_err("show acl rule %d convert redirect port %#x error \n",i,redirectport);
					free(AclList);
					ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
				}
				else
				{
					syslog_ax_acl_dbg("show acl rule %d get redirect port %d/%d \n", i,slot_no,port_no);
					AclList->redirectslot = slot_no;
					AclList->redirectport = port_no;
				}
			}
			if((ACL_ACTION_TYPE_MIRROR_TO_ANALYZER == aType) && (ret == ACL_RETURN_CODE_SUCCESS))
			{					
				mirrorport=aclNodePtr->ActionPtr.actionInfo.mirror.portNum;
				modid = aclNodePtr->ActionPtr.actionInfo.mirror.modid;
				retVal=nam_get_slotport_by_devport(modid,mirrorport,PRODUCT_ID,&slot_no,&port_no);
				if(retVal!=0) 
				{
					syslog_ax_acl_err("show acl rule %d convert mirror port %#x error \n",i,mirrorport);
					free(AclList);
					ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
				}
				else
				{
					syslog_ax_acl_dbg("show acl rule %d get mirror port %d/%d \n", i,slot_no,port_no);
					AclList->mirrorslot = slot_no;
					AclList->mirrorport = port_no;
				}
			}
			if((32 == (aclNodePtr->appendInfo.appendType)) && (ret == ACL_RETURN_CODE_SUCCESS))
			{
				AclList->appendInfo =32;
				AclList->qosprofileindex=aclNodePtr->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
			}

		}
		else if(ACL_TCAM_RULE_PACKETTYPE_QOS == pType)
		{
			AclList->up 		= (stdIpv4L4->rule.UP)&(stdIpv4L4->mask.maskUP);
			AclList->dscp		= (stdIpv4L4->rule.DSCP)&(stdIpv4L4->mask.maskDSCP);
		       AclList->precedence = aclNodePtr->ActionPtr.actionInfo.qos.qos.ingress.profilePrecedence;
			AclList->modifyUP	= aclNodePtr->ActionPtr.actionInfo.qos.modifyUp;
			AclList->modifyDSCP = aclNodePtr->ActionPtr.actionInfo.qos.modifyDscp;
			if(ACL_ACTION_TYPE_INGRESS_QOS ==(aclNodePtr->ActionPtr.actionType))
				AclList->qosprofileindex=aclNodePtr->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;		
				
			if(ACL_ACTION_TYPE_EGRESS_QOS==(aclNodePtr->ActionPtr.actionType))
			{
				AclList->egrDSCP=aclNodePtr->ActionPtr.actionInfo.qos.qos.egress.egrDscp ;
				AclList->egrUP=aclNodePtr->ActionPtr.actionInfo.qos.qos.egress.egrUp ;
			}
			if(32==(aclNodePtr->appendInfo.appendType)) 
			{
				AclList->appendInfo =32;
				AclList->qosprofileindex=aclNodePtr->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
			}
		}
	 	else if ((ACL_TCAM_RULE_PACKETTYPE_UDP == pType) ||(ACL_TCAM_RULE_PACKETTYPE_TCP == pType)) 
		{
			if (STANDARD_ACL_RULE==rType) 
			{
				AclList->dip	   	=stdIpv4L4->rule.Dip;
				AclList->sip	   	=stdIpv4L4->rule.Sip;
				AclList->maskdip	=stdIpv4L4->mask.maskLenDip;
				AclList->masksip	=stdIpv4L4->mask.maskLenSip;
				if (0 == stdIpv4L4->mask.maskSrcPort) 
				{
					AclList->srcport = ACL_ANY_PORT;
				}
				else  
				{
					AclList->srcport = stdIpv4L4->rule.SrcPort;
				}
				
				if (0 == stdIpv4L4->mask.maskDstPort) 
				{
					AclList->dstport	= ACL_ANY_PORT;
				}
				else 
				{
					AclList->dstport	=stdIpv4L4->rule.DstPort;
				}
				AclList->icmp_code=stdIpv4L4->rule.Code;
				AclList->icmp_type	=stdIpv4L4->rule.Type;

				if(ACL_ACTION_TYPE_REDIRECT== aType)
				{					
					redirectport = aclNodePtr->ActionPtr.actionInfo.redirect.portNum;
					modid = aclNodePtr->ActionPtr.actionInfo.redirect.modid;
					retVal = nam_get_slotport_by_devport(modid,redirectport,PRODUCT_ID,&slot_no,&port_no);
					if(retVal!=0) 
					{
						syslog_ax_acl_err("show acl rule redirect port %#x convert error!\n",redirectport);
						free(AclList);
						ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
					}
					else
					{
						syslog_ax_acl_dbg("show acl rule %d get redirect port %d/%d \n", i,slot_no,port_no);
						AclList->redirectslot=(unsigned char)slot_no;
						AclList->redirectport =(unsigned char)port_no;
					}
				}
				
				if((ACL_ACTION_TYPE_MIRROR_TO_ANALYZER== aType) && (ret == ACL_RETURN_CODE_SUCCESS))
				{ 				
					mirrorport=aclNodePtr->ActionPtr.actionInfo.mirror.portNum;
					modid = aclNodePtr->ActionPtr.actionInfo.mirror.modid;
					retVal=nam_get_slotport_by_devport(modid,mirrorport,PRODUCT_ID,&slot_no,&port_no);
					if(retVal!=0) 
					{
						syslog_ax_acl_err("show acl rule mirror target port %#x convert error!\n",redirectport);
						free(AclList);
						ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
					}
					else
					{
						syslog_ax_acl_dbg("show acl rule %d get mirror target port %d/%d \n", i, slot_no ,port_no);
						AclList->mirrorslot=(unsigned char)slot_no;
						AclList->mirrorport=(unsigned char)port_no;
					}
				}

				/* append info*/
				if((32==(aclNodePtr->appendInfo.appendType)) && (ret == ACL_RETURN_CODE_SUCCESS)) 
				{
					AclList->appendInfo =32;
					AclList->qosprofileindex=aclNodePtr->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
				}
			}
			else if(EXTENDED_ACL_RULE==rType)
			{
				AclList->dip	   = extNotIpv6->rule.Dip;
				AclList->sip	   = extNotIpv6->rule.Sip;
				AclList->maskdip	= extNotIpv6->mask.maskLenDip;
				AclList->masksip	= extNotIpv6->mask.maskLenSip;
				if (0 == extNotIpv6->mask.maskSrcPort) 
				{
					AclList->srcport = ACL_ANY_PORT;
				}
				else  
				{
					AclList->srcport = extNotIpv6->rule.SrcPort;
				}
				
				if (0 == extNotIpv6->mask.maskDstPort) 
				{
					AclList->dstport	= ACL_ANY_PORT;
				}
				else 
				{
					AclList->dstport	=extNotIpv6->rule.DstPort;
				}

				AclList->icmp_code  = 0;
				AclList->icmp_type	= 0;
				for(l=0;l<6;l++)
				{
					AclList->dmac[l] = extNotIpv6->rule.dMac[l];
					AclList->smac[l] = extNotIpv6->rule.sMac[l];
				}

				AclList->vlanid = extNotIpv6->rule.vlanId;
				sourceport = extNotIpv6->rule.portNo;
				sourceportmask = extNotIpv6->mask.maskPortNo;
				modid = extNotIpv6->rule.modid;
				if(0 != sourceportmask)
				{
					retVal = nam_get_slotport_by_devport(modid,sourceport,PRODUCT_ID,&slot_no,&port_no);
					if(retVal!= 0) 
					{
						syslog_ax_acl_err("show acl rule %d convert source port %#x error \n",i,sourceport);
						free(AclList);
						ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
					}
					else
					{
						syslog_ax_acl_dbg("show acl rule %d get source port %d/%d \n",i, slot_no,port_no);
						AclList->sourceslot = slot_no;
						AclList->sourceport = port_no;

					}			
				}
				else 
				{
					AclList->sourceslot = ACL_ANY_PORT_CHAR;
					AclList->sourceport = ACL_ANY_PORT_CHAR;
				}
				if((ACL_ACTION_TYPE_REDIRECT==(aclNodePtr->ActionPtr.actionType))  && (ret == ACL_RETURN_CODE_SUCCESS))
				{
					redirectport=aclNodePtr->ActionPtr.actionInfo.redirect.portNum;
					modid = aclNodePtr->ActionPtr.actionInfo.redirect.modid;
					retVal=nam_get_slotport_by_devport(modid,redirectport,PRODUCT_ID,&slot_no,&port_no);
					if(retVal!=0) 
					{
						syslog_ax_acl_err("show acl rule %d convert redirect port %#x error \n",i,redirectport);
						free(AclList);
						ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
					}
					else
					{
						syslog_ax_acl_dbg("show acl rule %d get redirect port %d/%d \n",i, slot_no,port_no);
					  	AclList->redirectslot = slot_no;
					  	AclList->redirectport = port_no;
					}
				}
				if((ACL_ACTION_TYPE_MIRROR_TO_ANALYZER==(aclNodePtr->ActionPtr.actionType)) && (ret == ACL_RETURN_CODE_SUCCESS))
				{					
					mirrorport=aclNodePtr->ActionPtr.actionInfo.mirror.portNum;
					modid = aclNodePtr->ActionPtr.actionInfo.mirror.modid;
					retVal=nam_get_slotport_by_devport(modid,mirrorport,PRODUCT_ID,&slot_no,&port_no);
					if(retVal!=0) 
					{
						syslog_ax_acl_err("show acl rule %d convert mirror port %#x error \n",i,mirrorport);
						free(AclList);
						ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
					}
					else
					{  
						syslog_ax_acl_dbg("show acl rule %d get mirror port %d/%d \n", i, slot_no,port_no);
						AclList->mirrorslot = slot_no;
						AclList->mirrorport = port_no;
					}
				}
				/*code optimize:  Various
				zhangcl@autelan.com 2013-1-17*/
				if((32==(aclNodePtr->appendInfo.appendType)) && (ret == ACL_RETURN_CODE_SUCCESS)) 
				{
					AclList->appendInfo = 32;
					AclList->qosprofileindex=aclNodePtr->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
				}
			} /*extend rule*/
		}
		else 
		{
			AclList->ruleIndex  = (aclNodePtr->ruleIndex+1);
			AclList->ruleType   = aclNodePtr->ruleType;
			AclList->actionType = aclNodePtr->ActionPtr.actionType;
			AclList->packetType = aclNodePtr->TcamPtr.packetType;
			AclList->dipv6	   = extIpv6->rule.Dip;
			AclList->sipv6	   = extIpv6->rule.Sip;
			AclList->maskdip    = extIpv6->mask.maskLenDip;
			AclList->masksip    = extIpv6->mask.maskLenSip;
			AclList->nextheader = aclNodePtr->TcamPtr.nextheader;			
			if(ACL_ACTION_TYPE_REDIRECT==(aclNodePtr->ActionPtr.actionType))
			{					
				redirectport=aclNodePtr->ActionPtr.actionInfo.redirect.portNum;
				modid = aclNodePtr->ActionPtr.actionInfo.redirect.modid;
				retVal=nam_get_slotport_by_devport(modid,redirectport,PRODUCT_ID,&slot_no,&port_no);
				if(retVal!=0) 
				{
					syslog_ax_acl_err("show acl rule %d convert redirect port %#x error \n",i,redirectport);
					free(AclList);
					ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
				}
				else
				{
					syslog_ax_acl_dbg("show acl rule %d get redirect port %d/%d \n",i, slot_no,port_no);
					AclList->redirectslot = slot_no;
					AclList->redirectport = port_no;
				}
			}
			/*code optimize:  Various
			zhangcl@autelan.com 2013-1-17*/
			if(ret == ACL_RETURN_CODE_SUCCESS)
			{
				if (0 == extIpv6->mask.maskSrcPort) 
				{
					AclList->srcport = ACL_ANY_PORT;
				}
				else  
				{
					AclList->srcport = extIpv6->rule.SrcPort;
				}
				
				if (0 == extIpv6->mask.maskDstPort) 
				{
					AclList->dstport = ACL_ANY_PORT;
				}
				else 
				{
					AclList->dstport = extIpv6->rule.DstPort;
				}
			}
		}
	}	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&AclList->ruleType);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&AclList->actionType);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&AclList->packetType);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&AclList->dip);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&AclList->maskdip);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&AclList->sip);	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&AclList->masksip);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->dipv6.ipbuf[0]); 
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->dipv6.ipbuf[1]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->dipv6.ipbuf[2]); 
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->dipv6.ipbuf[3]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->dipv6.ipbuf[4]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->dipv6.ipbuf[5]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->dipv6.ipbuf[6]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->dipv6.ipbuf[7]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->dipv6.ipbuf[8]); 
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->dipv6.ipbuf[9]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->dipv6.ipbuf[10]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->dipv6.ipbuf[11]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->dipv6.ipbuf[12]); 
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->dipv6.ipbuf[13]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->dipv6.ipbuf[14]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->dipv6.ipbuf[15]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->sipv6.ipbuf[0]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->sipv6.ipbuf[1]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->sipv6.ipbuf[2]); 
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->sipv6.ipbuf[3]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->sipv6.ipbuf[4]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->sipv6.ipbuf[5]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->sipv6.ipbuf[6]); 
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->sipv6.ipbuf[7]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->sipv6.ipbuf[8]); 
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->sipv6.ipbuf[9]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->sipv6.ipbuf[10]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->sipv6.ipbuf[11]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->sipv6.ipbuf[12]); 
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->sipv6.ipbuf[13]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->sipv6.ipbuf[14]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->sipv6.ipbuf[15]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&AclList->nextheader);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&AclList->dstport);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&AclList->srcport);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->icmp_code);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->icmp_type);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->code_mask);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->type_mask);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->dmac[0]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->dmac[1]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->dmac[2]); 
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->dmac[3]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->dmac[4]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->dmac[5]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->smac[0]); 
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->smac[1]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->smac[2]); 
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->smac[3]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->smac[4]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->smac[5]);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&AclList->vlanid);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&AclList->sourceslot);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&AclList->sourceport);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&AclList->redirectslot);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&AclList->redirectport);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&AclList->mirrorslot);	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&AclList->mirrorport);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&AclList->policer);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&AclList->policerId);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&AclList->modifyUP);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&AclList->modifyDSCP);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&AclList->up);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&AclList->dscp);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&AclList->egrUP);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&AclList->egrDSCP);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&AclList->qosprofileindex);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&AclList->precedence);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_BYTE,&AclList->appendInfo);
	
	free(AclList);
	AclList = NULL;

	return reply;
}

DBusMessage * npd_dbus_show_accesslist(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage*	                     reply = NULL;
	DBusMessageIter                      iter_array,iter;
	DBusError		                     err;		
	int 	                             ret=ACL_RETURN_CODE_SUCCESS,i=0,j=0,k=0,l=0,pt1=0,pt2=0, ts=0,mm=0,kkk=0;
	char  			                     protype[10];
	ACL_SHOW_STC                         *AclList[MAX_ACL_RULE_NUMBER];
	ACL_RULE_STDIPV4L4_T                 *stdIpv4L4=NULL;
	ACL_RULE_STDNOTIP_T                  *stdNotIp = NULL;
	ACL_RULE_EXTNOTIPV6_T				 *extNotIpv6=NULL;
	ACL_RULE_EXTIPV6L4_T				*extIpv6=NULL;
	unsigned int                         acl_count=0;
	AclRuleList 						 *aclNodePtr=NULL,*aclNode2Ptr=NULL,*aclNode3Ptr=NULL,*aclNode4Ptr=NULL;
	unsigned int						 sourceport=0,sourceportmask = 0,mirrorport=0,redirectport=0,modid = 0;	
	dbus_error_init(&err);

	ret=npd_acl_nodesearch();

	if(ACL_RETURN_CODE_ERROR==ret) {
		syslog_ax_acl_dbg("no access list existed!\n");
	}
	else {
	  for(i=0;i<MAX_ACL_RULE_NUMBER;i++) {
		if(NULL!=g_acl_rule[i]) {
			ts=g_acl_rule[i]->TcamPtr.packetType;
	
			switch(ts) {
				case 0:strcpy(protype,"IP");	break;
				case 1:strcpy(protype,"UDP"); 	break;
				case 2:strcpy(protype,"TCP"); 	break;
				case 3:strcpy(protype,"ICMP");	break;
				case 4:strcpy(protype,"ethernet");break;
				case 5:strcpy(protype,"arp");	break;
				case 6:strcpy(protype,"QoS");	break;					
				default: break;									
			}
			mm = g_acl_rule[i]->ruleType;
			syslog_ax_acl_dbg("g_acl_rule[i]->ruleType %d\n",mm);
		/*	if(STANDARD_ACL_RULE==mm) {*/
			if ((ts != 1)&&(ts != 2)) {
			   if((ts==0)||(ts==3)){

					/*type_flag = 0;*/
					
					pt1=npd_acl_isIp_display(ts,g_acl_rule[i]);	
					if(pt1 != 0){
  						 	syslog_ax_acl_err("npd_acl>>npd_dbus_how_acl_lists::npd_acl_isIp_display pt1 %d\n",pt1);
   				     		pt1=ACL_RETURN_CODE_ERROR;								
 					}
 					
					AclList[acl_count] = (ACL_SHOW_STC*)malloc(sizeof(ACL_SHOW_STC));
					if(NULL==AclList[acl_count]){
						syslog_ax_acl_dbg("malloc fail \n");
						return NULL;
					}
					memset(AclList[acl_count],0,sizeof(ACL_SHOW_STC));
                  
					aclNodePtr = g_acl_rule[i];
					
					stdIpv4L4  = &(aclNodePtr->TcamPtr.ruleInfo.stdIpv4L4);
					AclList[acl_count]->ruleIndex =(aclNodePtr->ruleIndex+1);		
					AclList[acl_count]->startIndex =(aclNodePtr->startIndex);		
					AclList[acl_count]->endIndex=(aclNodePtr->endIndex);
					AclList[acl_count]->ruleType =aclNodePtr->ruleType;
					AclList[acl_count]->actionType = aclNodePtr->ActionPtr.actionType;
					AclList[acl_count]->packetType = aclNodePtr->TcamPtr.packetType;
					AclList[acl_count]->dip 	   =stdIpv4L4->rule.Dip;
					AclList[acl_count]->sip        =stdIpv4L4->rule.Sip;
					AclList[acl_count]->maskdip    =stdIpv4L4->mask.maskLenDip;
					AclList[acl_count]->masksip    =stdIpv4L4->mask.maskLenSip;
					AclList[acl_count]->srcport = stdIpv4L4->rule.SrcPort;			
					AclList[acl_count]->dstport	=stdIpv4L4->rule.DstPort;
					AclList[acl_count]->icmp_code  =stdIpv4L4->rule.Code;
					AclList[acl_count]->code_mask=stdIpv4L4->mask.maskCode;
					AclList[acl_count]->icmp_type  =stdIpv4L4->rule.Type;
					AclList[acl_count]->type_mask=stdIpv4L4->mask.maskType;
					AclList[acl_count]->vlanid     = 0;
					AclList[acl_count]->sourceport = 0;
					for(j=0;j<6;j++){
						AclList[acl_count]->dmac[j]=0;
						AclList[acl_count]->smac[j]=0;
					}
					if(ACL_ACTION_TYPE_REDIRECT==(aclNodePtr->ActionPtr.actionType)){					
						  redirectport=aclNodePtr->ActionPtr.actionInfo.redirect.portNum;
						  modid = aclNodePtr->ActionPtr.actionInfo.redirect.modid;
						  unsigned  int slot_no = 0, port_no = 0;
						  kkk=nam_get_slotport_by_devport(modid,redirectport,PRODUCT_ID,&(slot_no),&(port_no));
						  if(kkk!=0) {
								syslog_ax_acl_err("get slot port for redirct %d error!\n",redirectport);
								free(AclList[acl_count]);
								ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
							}
						  else{
						  	syslog_ax_acl_dbg("slot/port %d/%d\n",slot_no,port_no);
							AclList[acl_count]->redirectslot=(unsigned char)slot_no;
							AclList[acl_count]->redirectport =(unsigned char)port_no;
						  }
					}
					if(ACL_ACTION_TYPE_MIRROR_TO_ANALYZER==(aclNodePtr->ActionPtr.actionType)){					
						  mirrorport=aclNodePtr->ActionPtr.actionInfo.mirror.portNum;
						  modid = aclNodePtr->ActionPtr.actionInfo.mirror.modid;
						  unsigned int mmslot_no = 0, mmport_no = 0;
						  kkk=nam_get_slotport_by_devport(modid,mirrorport,PRODUCT_ID,&(mmslot_no),&(mmport_no));
						  if(kkk!=0) {
								syslog_ax_acl_err("get slot port for mirror %d error!\n",mirrorport);
								free(AclList[acl_count]);
								ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
							}
						  else{
						  	syslog_ax_acl_dbg("slot/port %d/%d\n",mmslot_no,mmport_no);
							AclList[acl_count]->mirrorslot=(unsigned char)mmslot_no;
							AclList[acl_count]->mirrorport=(unsigned char)mmport_no;

						  }
					}
					if(32==(aclNodePtr->appendInfo.appendType))
					{
						AclList[acl_count]->appendInfo =32;
						AclList[acl_count]->qosprofileindex=aclNodePtr->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
					}
					AclList[acl_count]->policer   =aclNodePtr->ActionPtr.policerPtr.policerEnable;
					AclList[acl_count]->policerId =aclNodePtr->ActionPtr.policerPtr.policerId;
					acl_count++;
			}	
			else if((ts==4)||(ts==5)) {                     
					 /*type_flag = 1;*/
					 
				pt2=npd_acl_notIp_display(g_acl_rule[i]);
				if(pt2 != 0){
				 	syslog_ax_acl_err("npd_acl>>npd_dbus_show_acl_lists::npd_acl_notIp_display pt2 %d\n",pt2);
					free(AclList[acl_count]);
				 	pt2=ACL_RETURN_CODE_ERROR;
				}
                    		AclList[acl_count] = (ACL_SHOW_STC*)malloc(sizeof(ACL_SHOW_STC));
				if(NULL==AclList[acl_count]){
					syslog_ax_acl_err("malloc fail!\n");
					return NULL;
				}
				
                   		memset(AclList[acl_count],0,sizeof(ACL_SHOW_STC));
										 
				aclNode2Ptr = g_acl_rule[i];
					
                          stdNotIp  = &(aclNode2Ptr->TcamPtr.ruleInfo.stdNotIp);
                          AclList[acl_count]->ruleIndex =(aclNode2Ptr->ruleIndex+1);
			       AclList[acl_count]->ruleType =aclNode2Ptr->ruleType;
                          AclList[acl_count]->actionType =aclNode2Ptr->ActionPtr.actionType;
				AclList[acl_count]->packetType = aclNode2Ptr->TcamPtr.packetType;		
                        	for(k=0;k<6;k++) {
	                         AclList[acl_count]->dmac[k]= stdNotIp->rule.dMac[k];
	                         AclList[acl_count]->smac[k]= stdNotIp->rule.sMac[k];
                       	}
					
				AclList[acl_count]->vlanid =stdNotIp->rule.vlanId;
				
				sourceport=stdNotIp->rule.portno;
				sourceportmask = stdNotIp->mask.maskPortno;
				if(0 != sourceportmask){
					unsigned int sslot_no=0,sport_no=0;
					modid = stdNotIp->rule.modid;
					kkk=nam_get_slotport_by_devport(modid,sourceport,PRODUCT_ID,&(sslot_no),&(sport_no));
				    if(kkk!=0) {
						syslog_ax_acl_dbg("get slot port for source %d error!\n",sourceport);
						free(AclList[acl_count]);
						ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
					}
					else{
						syslog_ax_acl_dbg("slot/port %d/%d\n", sslot_no,sport_no);
						AclList[acl_count]->sourceslot=sslot_no;
						AclList[acl_count]->sourceport=sport_no;
					}
				}
				else {
					AclList[acl_count]->sourceslot = ACL_ANY_PORT_CHAR;
					AclList[acl_count]->sourceport = ACL_ANY_PORT_CHAR;
				}

				if(ACL_ACTION_TYPE_REDIRECT==(aclNode2Ptr->ActionPtr.actionType)){					
					  redirectport=aclNode2Ptr->ActionPtr.actionInfo.redirect.portNum;
					  modid = aclNode2Ptr->ActionPtr.actionInfo.redirect.modid;
					  unsigned int rslot=0,rport=0;
					  kkk=nam_get_slotport_by_devport(modid,redirectport,PRODUCT_ID,&(rslot),&(rport));
					  if(kkk!=0) {
							syslog_ax_acl_err("get slot port for redirct %d error!\n",redirectport);
							free(AclList[acl_count]);
							ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
						}
					  else{
							syslog_ax_acl_dbg("slot/port %d/%d\n", rslot,rport);
							AclList[acl_count]->redirectslot=rslot;
							AclList[acl_count]->redirectport=rport;
					  }
				}
				if(ACL_ACTION_TYPE_MIRROR_TO_ANALYZER==(aclNode2Ptr->ActionPtr.actionType)){					
					  mirrorport=aclNode2Ptr->ActionPtr.actionInfo.mirror.portNum;
					  modid = aclNode2Ptr->ActionPtr.actionInfo.mirror.modid;
					  unsigned int mslot=0,mport=0;
					  kkk=nam_get_slotport_by_devport(modid,mirrorport,PRODUCT_ID,&(mslot),&(mport));
					  if(kkk!=0) {
							syslog_ax_acl_err("get slot port for mirror %d error!\n",mirrorport);
							free(AclList[acl_count]);
							ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
						}
					  else{
					  		syslog_ax_acl_dbg("slot/port %d/%d\n", mslot,mport);
							AclList[acl_count]->mirrorslot=mslot;
							AclList[acl_count]->mirrorport=mport;
					  }
				}
				if(32==(aclNode2Ptr->appendInfo.appendType))
				{
					AclList[acl_count]->appendInfo =32;
					AclList[acl_count]->qosprofileindex=aclNode2Ptr->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
				}
				AclList[acl_count]->policer   =aclNode2Ptr->ActionPtr.policerPtr.policerEnable;
				AclList[acl_count]->policerId =aclNode2Ptr->ActionPtr.policerPtr.policerId;						

			acl_count++;			  
		}
		else if(ts==6) {
				AclList[acl_count] = (ACL_SHOW_STC*)malloc(sizeof(ACL_SHOW_STC));

				if(NULL==AclList[acl_count]){
						syslog_ax_acl_err("malloc fail!\n");
					return NULL;
				}
				memset(AclList[acl_count],0,sizeof(ACL_SHOW_STC));
	 
				aclNode3Ptr = g_acl_rule[i];

				AclList[acl_count]->ruleIndex   = (aclNode3Ptr->ruleIndex+1);
				AclList[acl_count]->ruleType =aclNode3Ptr->ruleType;
				AclList[acl_count]->actionType  = aclNode3Ptr->ActionPtr.actionType;
				AclList[acl_count]->packetType  = aclNode3Ptr->TcamPtr.packetType;		
				AclList[acl_count]->up	= (aclNode3Ptr->TcamPtr.ruleInfo.stdIpv4L4.rule.UP)&(aclNode3Ptr->TcamPtr.ruleInfo.stdIpv4L4.mask.maskUP);
				AclList[acl_count]->dscp		= (aclNode3Ptr->TcamPtr.ruleInfo.stdIpv4L4.rule.DSCP)&(aclNode3Ptr->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDSCP);
				AclList[acl_count]->precedence 	= aclNode3Ptr->ActionPtr.actionInfo.qos.qos.ingress.profilePrecedence;
				AclList[acl_count]->modifyUP	= aclNode3Ptr->ActionPtr.actionInfo.qos.modifyUp;
				AclList[acl_count]->modifyDSCP  = aclNode3Ptr->ActionPtr.actionInfo.qos.modifyDscp;
				if(ACL_ACTION_TYPE_INGRESS_QOS ==(aclNode3Ptr->ActionPtr.actionType))
					AclList[acl_count]->qosprofileindex=aclNode3Ptr->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;			
					
				if(ACL_ACTION_TYPE_EGRESS_QOS==(aclNode3Ptr->ActionPtr.actionType)){
					AclList[acl_count]->egrDSCP=aclNode3Ptr->ActionPtr.actionInfo.qos.qos.egress.egrDscp ;
				    AclList[acl_count]->egrUP=aclNode3Ptr->ActionPtr.actionInfo.qos.qos.egress.egrUp ;
				}
				if(32==(aclNode3Ptr->appendInfo.appendType))
				{
					AclList[acl_count]->appendInfo =32;
				    AclList[acl_count]->qosprofileindex=aclNode3Ptr->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
				}
				AclList[acl_count]->policer   =aclNode3Ptr->ActionPtr.policerPtr.policerEnable;
				AclList[acl_count]->policerId =aclNode3Ptr->ActionPtr.policerPtr.policerId;
											
				acl_count++;
			}
			/*ACL_TCAM_RULE_PACKETTYPE_TCPV6 or ACL_TCAM_RULE_PACKETTYPE_IPV6*/
			else {
				AclList[acl_count] = (ACL_SHOW_STC*)malloc(sizeof(ACL_SHOW_STC));
				if(NULL==AclList[acl_count]){
					syslog_ax_acl_err("malloc fail!\n");
					return NULL;
				}
				memset(AclList[acl_count],0,sizeof(ACL_SHOW_STC));
			  
				aclNode4Ptr = g_acl_rule[i];
				
				extIpv6	= &(aclNode4Ptr->TcamPtr.ruleInfo.extIpv6L4);
				AclList[acl_count]->ruleIndex  = (aclNode4Ptr->ruleIndex+1);
				AclList[acl_count]->ruleType   = aclNode4Ptr->ruleType;
				AclList[acl_count]->actionType = aclNode4Ptr->ActionPtr.actionType;
				AclList[acl_count]->packetType = aclNode4Ptr->TcamPtr.packetType;
				AclList[acl_count]->dipv6	   = extIpv6->rule.Dip;
				AclList[acl_count]->sipv6 	   = extIpv6->rule.Sip;
				AclList[acl_count]->maskdip    = extIpv6->mask.maskLenDip;
				AclList[acl_count]->masksip    = extIpv6->mask.maskLenSip;
				AclList[acl_count]->nextheader = aclNode4Ptr->TcamPtr.nextheader;				
				if(ACL_ACTION_TYPE_REDIRECT==(aclNode4Ptr->ActionPtr.actionType)){ 
					  redirectport=aclNode4Ptr->ActionPtr.actionInfo.redirect.portNum;					 
					  modid = aclNode4Ptr->ActionPtr.actionInfo.redirect.modid;					  
				         unsigned int rslot=0,rport=0;					  
					  kkk=nam_get_slotport_by_devport(modid,redirectport,PRODUCT_ID,&(rslot),&(rport));
					  if(kkk!=0) {
							syslog_ax_acl_err("get slot port for redirct %d error!\n",redirectport);
							free(AclList[acl_count]);
							ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
						}
					  else{
							syslog_ax_acl_dbg("slot/port %d/%d\n", rslot,rport);
							AclList[acl_count]->redirectslot=rslot;
							AclList[acl_count]->redirectport=rport;
					  }
				}

				if (0 == extIpv6->mask.maskSrcPort) {
					AclList[acl_count]->srcport = ACL_ANY_PORT;
				}
				else  {
					AclList[acl_count]->srcport = extIpv6->rule.SrcPort;
				}
				
				if (0 == extIpv6->mask.maskDstPort) {
					AclList[acl_count]->dstport = ACL_ANY_PORT;
				}
				else {
					AclList[acl_count]->dstport = extIpv6->rule.DstPort;
				}
/*
				AclList[acl_count]->icmp_code  = 0;
				AclList[acl_count]->icmp_type  = 0;
				for(l=0;l<6;l++){
					AclList[acl_count]->dmac[l]= extNotIpv6->rule.dMac[l];
					AclList[acl_count]->smac[l]= extNotIpv6->rule.sMac[l];
				}
				
				AclList[acl_count]->vlanid =extNotIpv6->rule.vlanId;
				sourceport = extNotIpv6->rule.portNo;
				sourceportmask = extNotIpv6->mask.maskPortNo;
				if(0 != sourceportmask){
					unsigned int slot=0,port=0;
					modid = extNotIpv6->rule.modid;
					kkk=nam_get_slotport_by_devport(modid,sourceport,PRODUCT_ID,&(slot),&(port));
				    if(kkk!=0) {
						syslog_ax_acl_err("get slot port for source %d error!\n",sourceport);
						free(AclList[acl_count]);
						ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
					}
					else{
						syslog_ax_acl_dbg("slot/port %d/%d\n", slot,port);
						AclList[acl_count]->sourceslot=slot;
						AclList[acl_count]->sourceport=port;
					}			
				}
				else {
					AclList[acl_count]->sourceslot = ACL_ANY_PORT_CHAR;
					AclList[acl_count]->sourceport = ACL_ANY_PORT_CHAR;
				}

				if(ACL_ACTION_TYPE_REDIRECT==(aclNode4Ptr->ActionPtr.actionType)){					
					  redirectport=aclNode4Ptr->ActionPtr.actionInfo.redirect.portNum;
					  modid = aclNode4Ptr->ActionPtr.actionInfo.redirect.modid;
					  unsigned int rrrslot=0,rrrport=0;
					  kkk=nam_get_slotport_by_devport(modid,redirectport,PRODUCT_ID,&(rrrslot),&(rrrport));
					  if(kkk!=0) {
						syslog_ax_acl_err("get slot port for redirct %d error!\n",redirectport);
						free(AclList[acl_count]);
						ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
					 }
					 else{
						  syslog_ax_acl_dbg("slot/port %d/%d\n", rrrslot,rrrport);
						  AclList[acl_count]->redirectslot=rrrslot;
						  AclList[acl_count]->redirectport=rrrport;
					  }
				}
				if(ACL_ACTION_TYPE_MIRROR_TO_ANALYZER == (aclNode4Ptr->ActionPtr.actionType)) {					
					  mirrorport=aclNode4Ptr->ActionPtr.actionInfo.mirror.portNum;
					  modid = aclNode4Ptr->ActionPtr.actionInfo.mirror.modid;
					  unsigned int mmmslot=0,mmmport=0;
					  kkk=nam_get_slotport_by_devport(modid,mirrorport,PRODUCT_ID,&(mmmslot),&(mmmport));
					  if(kkk!=0) {
						syslog_ax_acl_err("get slot port for mirror %d error!\n",mirrorport);
						free(AclList[acl_count]);
						ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
					  }
					  else{  
					  	  syslog_ax_acl_dbg("slot/port %d/%d\n", mmmslot,mmmport);
						  AclList[acl_count]->mirrorslot=mmmslot;
						  AclList[acl_count]->mirrorport=mmmport;
					  }
				}*/
				if(32==(aclNode4Ptr->appendInfo.appendType)) {
					AclList[acl_count]->appendInfo =32;
					AclList[acl_count]->qosprofileindex=aclNode4Ptr->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
				}

				AclList[acl_count]->policer   =aclNode4Ptr->ActionPtr.policerPtr.policerEnable;
				AclList[acl_count]->policerId =aclNode4Ptr->ActionPtr.policerPtr.policerId;
						
				acl_count++;
			}
		}
		/*else if(EXTENDED_ACL_RULE==mm){*/
		else {
			if (STANDARD_ACL_RULE==mm) {					
				pt1=npd_acl_isIp_display(ts,g_acl_rule[i]);	
				if(pt1 != 0){
						syslog_ax_acl_err("npd_acl>>npd_dbus_how_acl_lists::npd_acl_isIp_display pt1 %d\n",pt1);
				     		pt1=ACL_RETURN_CODE_ERROR;								
					}
					
				AclList[acl_count] = (ACL_SHOW_STC*)malloc(sizeof(ACL_SHOW_STC));
				if(NULL==AclList[acl_count]){
					syslog_ax_acl_dbg("malloc fail \n");
					return NULL;
				}
				memset(AclList[acl_count],0,sizeof(ACL_SHOW_STC));
              
				aclNodePtr = g_acl_rule[i];
				
	                    stdIpv4L4  = &(aclNodePtr->TcamPtr.ruleInfo.stdIpv4L4);
	                    AclList[acl_count]->ruleIndex =(aclNodePtr->ruleIndex+1);
				 AclList[acl_count]->ruleType =aclNodePtr->ruleType;
	                    AclList[acl_count]->actionType = aclNodePtr->ActionPtr.actionType;
	                    AclList[acl_count]->packetType = aclNodePtr->TcamPtr.packetType;
	                    AclList[acl_count]->dip 	   =stdIpv4L4->rule.Dip;
	                    AclList[acl_count]->sip        =stdIpv4L4->rule.Sip;
	                    AclList[acl_count]->maskdip    =stdIpv4L4->mask.maskLenDip;
	                    AclList[acl_count]->masksip    =stdIpv4L4->mask.maskLenSip;
				if (0 == stdIpv4L4->mask.maskSrcPort) {
					AclList[acl_count]->srcport = ACL_ANY_PORT;
				}
				else  {
					AclList[acl_count]->srcport = stdIpv4L4->rule.SrcPort;
				}
				
				if (0 == stdIpv4L4->mask.maskDstPort) {
					AclList[acl_count]->dstport = ACL_ANY_PORT;
				}
				else {
					AclList[acl_count]->dstport =stdIpv4L4->rule.DstPort;
				}

	                    AclList[acl_count]->icmp_code  =stdIpv4L4->rule.Code;
	                    AclList[acl_count]->icmp_type  =stdIpv4L4->rule.Type;
				AclList[acl_count]->vlanid     = 0;
				AclList[acl_count]->sourceport = 0;
				for(j=0;j<6;j++){
						AclList[acl_count]->dmac[j]=0;
						AclList[acl_count]->smac[j]=0;
				}
				if(ACL_ACTION_TYPE_REDIRECT==(aclNodePtr->ActionPtr.actionType)){					
					  redirectport=aclNodePtr->ActionPtr.actionInfo.redirect.portNum;
					  modid = aclNodePtr->ActionPtr.actionInfo.redirect.modid;
					  unsigned  int slot_no = 0, port_no = 0;
					  kkk=nam_get_slotport_by_devport(modid,redirectport,PRODUCT_ID,&(slot_no),&(port_no));
					  if(kkk!=0) {
							syslog_ax_acl_err("get slot port for redirct %d error!\n",redirectport);
							free(AclList[acl_count]);
							ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
						}
					  else{
					  	syslog_ax_acl_dbg("slot/port %d/%d\n",slot_no,port_no);
						AclList[acl_count]->redirectslot=(unsigned char)slot_no;
						AclList[acl_count]->redirectport =(unsigned char)port_no;
					  }
				}
				if(ACL_ACTION_TYPE_MIRROR_TO_ANALYZER==(aclNodePtr->ActionPtr.actionType)) {					
					  mirrorport=aclNodePtr->ActionPtr.actionInfo.mirror.portNum;
					  modid = aclNodePtr->ActionPtr.actionInfo.mirror.modid;
					  unsigned int mmslot_no = 0, mmport_no = 0;
					  kkk=nam_get_slotport_by_devport(modid,mirrorport,PRODUCT_ID,&(mmslot_no),&(mmport_no));
					  if(kkk!=0) {
							syslog_ax_acl_err("get slot port for mirror %d error!\n",mirrorport);
							free(AclList[acl_count]);
							ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
					  }
					  else {
					  	syslog_ax_acl_dbg("slot/port %d/%d\n",mmslot_no,mmport_no);
						AclList[acl_count]->mirrorslot=(unsigned char)mmslot_no;
						AclList[acl_count]->mirrorport=(unsigned char)mmport_no;

					 }
				}
				if(32==(aclNodePtr->appendInfo.appendType)) {
					AclList[acl_count]->appendInfo =32;
				    AclList[acl_count]->qosprofileindex=aclNodePtr->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
				}
				AclList[acl_count]->policer   =aclNodePtr->ActionPtr.policerPtr.policerEnable;
				AclList[acl_count]->policerId =aclNodePtr->ActionPtr.policerPtr.policerId;
				acl_count++;
			}
			else if(EXTENDED_ACL_RULE==mm) {
				AclList[acl_count] = (ACL_SHOW_STC*)malloc(sizeof(ACL_SHOW_STC));
				if(NULL==AclList[acl_count]){
					syslog_ax_acl_err("malloc fail!\n");
					return NULL;
				}
				memset(AclList[acl_count],0,sizeof(ACL_SHOW_STC));
			  
				aclNode4Ptr = g_acl_rule[i];
				
				extNotIpv6	= &(aclNode4Ptr->TcamPtr.ruleInfo.extNotIpv6);
				AclList[acl_count]->ruleIndex  = (aclNode4Ptr->ruleIndex+1);
				AclList[acl_count]->ruleType   = aclNode4Ptr->ruleType;
				AclList[acl_count]->actionType = aclNode4Ptr->ActionPtr.actionType;
				AclList[acl_count]->packetType = aclNode4Ptr->TcamPtr.packetType;
				AclList[acl_count]->dip 	   = extNotIpv6->rule.Dip;
				AclList[acl_count]->sip 	   = extNotIpv6->rule.Sip;
				AclList[acl_count]->maskdip    = extNotIpv6->mask.maskLenDip;
				AclList[acl_count]->masksip    = extNotIpv6->mask.maskLenSip;
				if (0 == extNotIpv6->mask.maskSrcPort) {
					AclList[acl_count]->srcport = ACL_ANY_PORT;
				}
				else  {
					AclList[acl_count]->srcport = extNotIpv6->rule.SrcPort;
				}
				
				if (0 == extNotIpv6->mask.maskDstPort) {
					AclList[acl_count]->dstport = ACL_ANY_PORT;
				}
				else {
					AclList[acl_count]->dstport =extNotIpv6->rule.DstPort;
				}

				AclList[acl_count]->icmp_code  = 0;
				AclList[acl_count]->icmp_type  = 0;
				for(l=0;l<6;l++){
					AclList[acl_count]->dmac[l]= extNotIpv6->rule.dMac[l];
					AclList[acl_count]->smac[l]= extNotIpv6->rule.sMac[l];
				}
				
				AclList[acl_count]->vlanid =extNotIpv6->rule.vlanId;
				sourceport = extNotIpv6->rule.portNo;
				sourceportmask = extNotIpv6->mask.maskPortNo;
				if(0 != sourceportmask){
					unsigned int slot=0,port=0;
					modid = extNotIpv6->rule.modid;
					kkk=nam_get_slotport_by_devport(modid,sourceport,PRODUCT_ID,&(slot),&(port));
				    if(kkk!=0) {
						syslog_ax_acl_err("get slot port for source %d error!\n",sourceport);
						free(AclList[acl_count]);
						ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
					}
					else{
						syslog_ax_acl_dbg("slot/port %d/%d\n", slot,port);
						AclList[acl_count]->sourceslot=slot;
						AclList[acl_count]->sourceport=port;
					}			
				}
				else {
					AclList[acl_count]->sourceslot = ACL_ANY_PORT_CHAR;
					AclList[acl_count]->sourceport = ACL_ANY_PORT_CHAR;
				}

				if(ACL_ACTION_TYPE_REDIRECT==(aclNode4Ptr->ActionPtr.actionType)){					
					  redirectport=aclNode4Ptr->ActionPtr.actionInfo.redirect.portNum;
					  modid = aclNode4Ptr->ActionPtr.actionInfo.redirect.modid;
					  unsigned int rrrslot=0,rrrport=0;
					  kkk=nam_get_slotport_by_devport(modid,redirectport,PRODUCT_ID,&(rrrslot),&(rrrport));
					  if(kkk!=0) {
						syslog_ax_acl_err("get slot port for redirct %d error!\n",redirectport);
						free(AclList[acl_count]);
						ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
					 }
					 else{
						  syslog_ax_acl_dbg("slot/port %d/%d\n", rrrslot,rrrport);
						  AclList[acl_count]->redirectslot=rrrslot;
						  AclList[acl_count]->redirectport=rrrport;
					  }
				}
				if(ACL_ACTION_TYPE_MIRROR_TO_ANALYZER == (aclNode4Ptr->ActionPtr.actionType)) {					
					  mirrorport=aclNode4Ptr->ActionPtr.actionInfo.mirror.portNum;
					  modid = aclNode4Ptr->ActionPtr.actionInfo.mirror.modid;
					  unsigned int mmmslot=0,mmmport=0;
					  kkk=nam_get_slotport_by_devport(modid,mirrorport,PRODUCT_ID,&(mmmslot),&(mmmport));
					  if(kkk!=0) {
						syslog_ax_acl_err("get slot port for mirror %d error!\n",mirrorport);
						free(AclList[acl_count]);
						ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
					  }
					  else{  
					  	  syslog_ax_acl_dbg("slot/port %d/%d\n", mmmslot,mmmport);
						  AclList[acl_count]->mirrorslot=mmmslot;
						  AclList[acl_count]->mirrorport=mmmport;
					  }
				}
				if(32==(aclNode4Ptr->appendInfo.appendType)) {
					AclList[acl_count]->appendInfo =32;
					AclList[acl_count]->qosprofileindex=aclNode4Ptr->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
				}

				AclList[acl_count]->policer   =aclNode4Ptr->ActionPtr.policerPtr.policerEnable;
				AclList[acl_count]->policerId =aclNode4Ptr->ActionPtr.policerPtr.policerId;
						

				acl_count++;
				}
			} /*extend rule*/
		}  /*if			*/					
	} /*for*/
	}/*else*/
	
	if(acl_count>0)
	   ret =ACL_RETURN_CODE_SUCCESS;
	   
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &acl_count);
	syslog_ax_acl_dbg("return basic param acl_count %d",acl_count);
	

	dbus_message_iter_open_container (&iter,
									DBUS_TYPE_ARRAY,
									DBUS_STRUCT_BEGIN_CHAR_AS_STRING     /*begin*/
									DBUS_TYPE_UINT32_AS_STRING     /*rule index*/									
									DBUS_TYPE_UINT32_AS_STRING	   /*start index*/
									DBUS_TYPE_UINT32_AS_STRING	   /*end index*/
									DBUS_TYPE_UINT32_AS_STRING	  /*ruleType*/
									DBUS_TYPE_UINT32_AS_STRING     /*actiontype*/
									DBUS_TYPE_UINT32_AS_STRING     /*packtype*/
									DBUS_TYPE_UINT32_AS_STRING     /*dip*/
									DBUS_TYPE_UINT32_AS_STRING     /*maskdip*/
									DBUS_TYPE_UINT32_AS_STRING     /*sip*/
									DBUS_TYPE_UINT32_AS_STRING     /*masksip*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*dipv6[0]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*dipv6[1]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*dipv6[2]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*dipv6[3]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*dipv6[4]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*dipv6[5]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*dipv6[6]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*dipv6[7] */
									DBUS_TYPE_BYTE_AS_STRING 	   /*dipv6[8]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*dipv6[9]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*dipv6[10]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*dipv6[11]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*dipv6[12]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*dipv6[13]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*dipv6[14]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*sipv6[15]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*sipv6[0]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*sipv6[1] */
									DBUS_TYPE_BYTE_AS_STRING 	   /*sipv6[2]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*sipv6[3]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*sipv6[4]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*sipv6[5]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*sipv6[6]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*sipv6[7]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*sipv6[8]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*sipv6[9]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*sipv6[10]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*sipv6[11]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*sipv6[12]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*sipv6[13]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*sipv6[14]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*sipv6[15]*/	
									DBUS_TYPE_UINT32_AS_STRING	   /*nextheader	*/					
									DBUS_TYPE_UINT32_AS_STRING     /*dstport    */                  
									DBUS_TYPE_UINT32_AS_STRING     /*srcport*/
									DBUS_TYPE_BYTE_AS_STRING     /*icmp code*/
									DBUS_TYPE_BYTE_AS_STRING     /*icmp type  */
									DBUS_TYPE_BYTE_AS_STRING     /*icmp code mask*/
									DBUS_TYPE_BYTE_AS_STRING     /*icmp type mask*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*dmac[0]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*dmac[1]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*dmac[2]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*dmac[3]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*dmac[4]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*dmac[5]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*smac[0]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*smac[1] */
									DBUS_TYPE_BYTE_AS_STRING 	   /*smac[2]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*smac[3]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*smac[4]*/
									DBUS_TYPE_BYTE_AS_STRING 	   /*smac[5]*/
									DBUS_TYPE_UINT32_AS_STRING      /*vid*/
									DBUS_TYPE_BYTE_AS_STRING        /*sourceslot*/
									DBUS_TYPE_BYTE_AS_STRING			/*sourceport*/
									DBUS_TYPE_BYTE_AS_STRING			/*redirectslot*/
									DBUS_TYPE_BYTE_AS_STRING			/*redirectport*/
									DBUS_TYPE_BYTE_AS_STRING			/*mirrorport*/
									DBUS_TYPE_BYTE_AS_STRING			/*mirrorport  */                   
									DBUS_TYPE_UINT32_AS_STRING	  /*policer;*/
									DBUS_TYPE_UINT32_AS_STRING     /*policerId;  */
									DBUS_TYPE_UINT32_AS_STRING     /*modifyUP;*/
									DBUS_TYPE_UINT32_AS_STRING     /*modifyDSCP*/
									DBUS_TYPE_UINT32_AS_STRING     /*up;*/
									DBUS_TYPE_UINT32_AS_STRING     /*dscp;*/
									DBUS_TYPE_UINT32_AS_STRING     /*egrUP;*/
									DBUS_TYPE_UINT32_AS_STRING     /*egrDSCP;    */                  
									DBUS_TYPE_UINT32_AS_STRING     /*qosprofileindex;*/
									DBUS_TYPE_UINT32_AS_STRING
									DBUS_TYPE_BYTE_AS_STRING       /*appendIndex   */            
									DBUS_STRUCT_END_CHAR_AS_STRING,     /*end*/
								   &iter_array);
	syslog_ax_acl_dbg("open outer container");
	for (j = 0; j < acl_count; j++ ) {
		DBusMessageIter iter_struct;
		dbus_message_iter_open_container (&iter_array,
										   DBUS_TYPE_STRUCT,
										   NULL,
										   &iter_struct);				
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(AclList[j]->ruleIndex));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(AclList[j]->startIndex));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(AclList[j]->endIndex));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(AclList[j]->ruleType));
		dbus_message_iter_append_basic 
				(&iter_struct,
				 DBUS_TYPE_UINT32,
				 &(AclList[j]->actionType));
		
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(AclList[j]->packetType));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(AclList[j]->dip));

		 dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(AclList[j]->maskdip));
					  
         dbus_message_iter_append_basic
                 (&iter_struct,
        		  DBUS_TYPE_UINT32,
        		  &(AclList[j]->sip)); 

		 dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(AclList[j]->masksip));
		 /*dst ip v6 address*/
		  dbus_message_iter_append_basic
				   (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->dipv6.ipbuf[0]));
		 dbus_message_iter_append_basic
				  (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->dipv6.ipbuf[1]));
		 dbus_message_iter_append_basic
				   (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->dipv6.ipbuf[2]));
		 dbus_message_iter_append_basic
				   (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->dipv6.ipbuf[3]));
		 dbus_message_iter_append_basic
				   (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->dipv6.ipbuf[4]));
		 dbus_message_iter_append_basic
				   (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->dipv6.ipbuf[5]));
		  dbus_message_iter_append_basic
				   (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->dipv6.ipbuf[6]));
		 dbus_message_iter_append_basic
				  (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->dipv6.ipbuf[7]));
		 dbus_message_iter_append_basic
				   (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->dipv6.ipbuf[8]));
		 dbus_message_iter_append_basic
				   (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->dipv6.ipbuf[9]));
		 dbus_message_iter_append_basic
				   (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->dipv6.ipbuf[10]));
		 dbus_message_iter_append_basic
				  (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->dipv6.ipbuf[11]));
		 dbus_message_iter_append_basic
				   (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->dipv6.ipbuf[12]));
		 dbus_message_iter_append_basic
				   (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->dipv6.ipbuf[13]));
		 dbus_message_iter_append_basic
				   (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->dipv6.ipbuf[14]));
		 dbus_message_iter_append_basic
				   (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->dipv6.ipbuf[15]));
 
		 /*source ip v6 address*/		
		 dbus_message_iter_append_basic
				   (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->sipv6.ipbuf[0]));
		 dbus_message_iter_append_basic
				  (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->sipv6.ipbuf[1]));
		 dbus_message_iter_append_basic
				   (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->sipv6.ipbuf[2]));
		 dbus_message_iter_append_basic
				   (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->sipv6.ipbuf[3]));
		 dbus_message_iter_append_basic
				   (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->sipv6.ipbuf[4]));
		 dbus_message_iter_append_basic
				   (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->sipv6.ipbuf[5]));
		 dbus_message_iter_append_basic
				  (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->sipv6.ipbuf[6]));
		 dbus_message_iter_append_basic
				   (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->sipv6.ipbuf[7]));
		 dbus_message_iter_append_basic
				   (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->sipv6.ipbuf[8]));
		 dbus_message_iter_append_basic
				   (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->sipv6.ipbuf[9]));
		 dbus_message_iter_append_basic
				   (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->sipv6.ipbuf[10]));
		 dbus_message_iter_append_basic
				  (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->sipv6.ipbuf[11]));
		 dbus_message_iter_append_basic
				   (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->sipv6.ipbuf[12]));
		 dbus_message_iter_append_basic
				   (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->sipv6.ipbuf[13]));
		 dbus_message_iter_append_basic
				   (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->sipv6.ipbuf[14]));
		 dbus_message_iter_append_basic
				   (&iter_struct,
				   DBUS_TYPE_BYTE,
				   &(AclList[j]->sipv6.ipbuf[15]));
		 
		 dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(AclList[j]->nextheader));

		 dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(AclList[j]->dstport));
		 dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(AclList[j]->srcport));	
	     dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(AclList[j]->icmp_code));
	     dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(AclList[j]->icmp_type));
	 dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(AclList[j]->code_mask));
	     dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(AclList[j]->type_mask));
         dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(AclList[j]->dmac[0]));
		dbus_message_iter_append_basic
 				 (&iter_struct,
 				  DBUS_TYPE_BYTE,
 				  &(AclList[j]->dmac[1]));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(AclList[j]->dmac[2]));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(AclList[j]->dmac[3]));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(AclList[j]->dmac[4]));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(AclList[j]->dmac[5]));
		
        dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(AclList[j]->smac[0]));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(AclList[j]->smac[1]));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(AclList[j]->smac[2]));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(AclList[j]->smac[3]));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(AclList[j]->smac[4]));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(AclList[j]->smac[5]));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(AclList[j]->vlanid));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(AclList[j]->sourceslot));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(AclList[j]->sourceport));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(AclList[j]->redirectslot));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(AclList[j]->redirectport));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(AclList[j]->mirrorslot));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(AclList[j]->mirrorport));	
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(AclList[j]->policer));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(AclList[j]->policerId));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(AclList[j]->modifyUP));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(AclList[j]->modifyDSCP));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(AclList[j]->up));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(AclList[j]->dscp));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(AclList[j]->egrUP));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(AclList[j]->egrDSCP));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(AclList[j]->qosprofileindex));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(AclList[j]->precedence));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(AclList[j]->appendInfo));
		
		dbus_message_iter_close_container (&iter_array, &iter_struct);
		free(AclList[j]);
		AclList[j]=NULL;
	}
	dbus_message_iter_close_container (&iter, &iter_array);

	return reply;
}
	
DBusMessage * npd_dbus_delete_acl (DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage*			    reply = NULL;
	DBusMessageIter 		    iter;
	DBusError				    err;
	unsigned long   		     index = 0;
	unsigned int			    ret = 0,group_num = 0;	
	
	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
								DBUS_TYPE_UINT32,&index,
								DBUS_TYPE_INVALID))) {
		syslog_ax_acl_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_acl_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	
	if(NULL != g_acl_rule[index]) {
		if (g_acl_rule[index]->appendInfo.appendType == 32) {
			if (NULL != qos_append[index]) {
				qos_append[index] = NULL;
			}
		}
	}

	/*if (1 == g_acl_rule[index]->rangerule) {
		ret = ACL_RETURN_CODE_ERROR;
	}
	else {*/
		ret=npd_acl_delete(index,&group_num);
		
		if(ret == ACL_RETURN_CODE_ERROR){
			syslog_ax_acl_dbg("ERROR! Range must range in 1-1024\n");	
		}
		else if(ret==ACL_RETURN_CODE_GLOBAL_NOT_EXISTED)	{
			syslog_ax_acl_dbg("rule %ld not existed!\n",index);
		}
		else if((ret == ACL_RETURN_CODE_GROUP_RULE_EXISTED)||(ret ==ACL_RETURN_CODE_EGRESS_GROUP_RULE_EXISTED)){
			syslog_ax_acl_dbg("rule %ld is used in group %d,",index,group_num);
			syslog_ax_acl_dbg("you can not delete the rule %ld\n",index);						
		}
		else if(ACL_RETURN_CODEL_MIRROR_USE==ret)
		{
			syslog_ax_acl_dbg("use in mirror,cancel mirror ,then delete\n");

		}
	
	 reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&group_num);
	return reply;	
}

DBusMessage * npd_dbus_delete_acl_range (DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage*			    reply = NULL;
	DBusMessageIter 		    iter;
	DBusError				    err;
	unsigned int   		     startIndex = 0, endIndex = 0, index = 0;
	unsigned int			i = 0, ret = 0,group_num = 0, rangeindex = 0;	
	struct acl_range_rule	*rangeNodePtr = NULL;
	
	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
								DBUS_TYPE_UINT32, &startIndex,
								DBUS_TYPE_UINT32, &endIndex,
								DBUS_TYPE_INVALID))) {
		syslog_ax_acl_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_acl_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	for(i = 0; i < MAX_ACL_RULE_NUMBER; i ++) {
		rangeNodePtr = r_acl_rule[i];
		
		if(NULL == rangeNodePtr) {
			continue;
		}
		else {
			if ((startIndex == rangeNodePtr->startIndex) && 
				(endIndex == rangeNodePtr->endIndex)) {
				rangeindex = i;
				npd_syslog_dbg("acl range %d %d is exist \n", startIndex, endIndex);
				break;
			}
		}
	}
	if (MAX_ACL_RULE_NUMBER != i) {
		for (i = 0; i < (endIndex - startIndex + 1); i++) {
			index = startIndex + i;
			if(NULL != g_acl_rule[index]) {
				if (g_acl_rule[index]->appendInfo.appendType == 32) {
					if (NULL != qos_append[index]) {
						qos_append[index] = NULL;
					}
				}
			}

			ret=npd_acl_delete(index,&group_num);
			
			if(ret == ACL_RETURN_CODE_ERROR){
				syslog_ax_acl_dbg("ERROR! Range must range in 1-1024\n");
				break;
			}
			else if(ret==ACL_RETURN_CODE_GLOBAL_NOT_EXISTED)	{
				syslog_ax_acl_dbg("rule %ld not existed!\n",index);
				break;
			}
			else if((ret == ACL_RETURN_CODE_GROUP_RULE_EXISTED)||(ret ==ACL_RETURN_CODE_EGRESS_GROUP_RULE_EXISTED)){
				syslog_ax_acl_dbg("rule %ld is used in group %d,",index,group_num);
				syslog_ax_acl_dbg("you can not delete the rule %ld\n",index);
				break;
			}
			else if(ACL_RETURN_CODEL_MIRROR_USE==ret)
			{
				syslog_ax_acl_dbg("use in mirror,cancel mirror ,then delete\n");
				break;
			}
		}
		free(r_acl_rule[rangeindex]);
		r_acl_rule[rangeindex] = NULL;
	}
	else {
		ret = ACL_RETURN_CODEL_RANGE_NOT_EXIST;
	}
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&group_num);
	return reply;	
}

DBusMessage * npd_dbus_creat_acl_group(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*	reply;    
	DBusMessageIter	iter;
	DBusError		err;
	unsigned int 	groupnum = 0,ret = 0,dir_info=0;
    struct acl_group_list_s *groupPtr=NULL;

	if(!((productinfo.capbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_ACL_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ACL_VLAUE))
	{
		npd_syslog_dbg("do not support acl!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else
	{
	    dbus_error_init(&err);
	   
	    if (!(dbus_message_get_args ( msg, &err,  
		   DBUS_TYPE_UINT32,&dir_info,
		   DBUS_TYPE_UINT32,&groupnum,
		   DBUS_TYPE_INVALID))) 
		{
		   syslog_ax_acl_err("Unable to get input args ");
		   if (dbus_error_is_set(&err)) 
		   {
			   syslog_ax_acl_err("%s raised: %s",err.name,err.message);
			   dbus_error_free(&err);
		   }
		   return NULL;
	    }
	 	if((groupnum>1023)||(groupnum<1)) 
		{
			ret=ACL_RETURN_CODE_GROUP_INDEX_ERROR;
	 	}
		else
		{
			switch (dir_info) 
			{ 
				case ACL_DIRECTION_INGRESS:
					if (acl_group[groupnum]!=NULL) 
					{
						syslog_ax_acl_dbg("sorry,the %d ingress acl group has been created!\n",groupnum);
						ret=ACL_RETURN_CODE_ERROR;									
					}				
					else if (NULL==acl_group[groupnum]) 
					{	
					 	/*creat link to maintain rule numbers*/
						/*creat link to maintain group's port infomations*/
						if(NULL!=egress_acl_group[groupnum]) 
						{
							ret =ACL_RETURN_CODE_GROUP_SAME_ID;
					 	}
						else 
						{
							groupPtr = (struct acl_group_list_s *)malloc(sizeof(struct acl_group_list_s));
							/*code optimize: Dereference after null check
							zhangcl@autelan.com 2013-1-17*/
						 	if(NULL==groupPtr) 
							{
								syslog_ax_acl_dbg("malloc failure!\n");
								ret=ACL_RETURN_CODE_ERROR;
							  	break;
						 	}
							memset(groupPtr,0,sizeof(struct acl_group_list_s));
							  
							syslog_ax_acl_dbg("initing acl group head....\n");
							INIT_LIST_HEAD(&(groupPtr->list1));
							INIT_LIST_HEAD(&(groupPtr->list2));
							INIT_LIST_HEAD(&(groupPtr->list3));
							groupPtr->count = 0;
							groupPtr->devnum = ACL_DEFAULT_DEVID;
							groupPtr->pclId = groupnum;
							groupPtr->eth_count = 0;
							groupPtr->vlan_count = 0;
							acl_group[groupnum]=groupPtr;

							syslog_ax_acl_dbg("count %d pcl id%d\n",acl_group[groupnum]->count,acl_group[groupnum]->pclId);

							ret = ACL_RETURN_CODE_SUCCESS;	
						}			
					 }
					 break;
				case ACL_DIRECTION_EGRESS:
					ret = nam_bcm_acl_support_check();
					if (ACL_RETURN_CODE_BCM_DEVICE_NOT_SUPPORT != ret) 
					{
						if (egress_acl_group[groupnum] != NULL) 
						{
							syslog_ax_acl_dbg("sorry,the %d egress acl group has been created!\n",groupnum);
							ret=ACL_RETURN_CODE_ERROR;							
						}			
						else if (NULL==egress_acl_group[groupnum]) 
						{	
						 	 if(NULL!=acl_group[groupnum]) 
							 {
							  	 ret =ACL_RETURN_CODE_GROUP_SAME_ID;
						 	 } 
							 else 
							 {
							 	  /*creat link to maintain rule numbers*/
							 	  /*creat link to maintain group's port infomations*/
								  /*code optimize: Dereference after null check
								  zhangcl@autelan.com 2013-1-17*/
								  groupPtr = (struct acl_group_list_s *)malloc(sizeof(struct acl_group_list_s));
							 	  if(NULL==groupPtr) 
								  {
								  	syslog_ax_acl_dbg("malloc failure!\n");
									ret=ACL_RETURN_CODE_ERROR;
									break;
							 	  }
								  memset(groupPtr,0,sizeof(struct acl_group_list_s));
								  
								  syslog_ax_acl_dbg("initing acl group head....\n");
								  INIT_LIST_HEAD(&(groupPtr->list1));
								  INIT_LIST_HEAD(&(groupPtr->list2));
								  INIT_LIST_HEAD(&(groupPtr->list3));
								  groupPtr->count = 0;
								  groupPtr->devnum = ACL_DEFAULT_DEVID;
								  groupPtr->pclId = groupnum;
								  groupPtr->eth_count = 0;
								  groupPtr->vlan_count = 0;
								  egress_acl_group[groupnum]=groupPtr;

								  syslog_ax_acl_dbg("count %d pcl id%d\n",egress_acl_group[groupnum]->count,egress_acl_group[groupnum]->pclId);
							 
								  ret = ACL_RETURN_CODE_SUCCESS;	
							 }
						 }
					}
					 break;
				default :
					ret=ACL_RETURN_CODE_ERROR;
					break;

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
DBusMessage * npd_dbus_config_acl_group(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*	reply;    
	DBusMessageIter	iter;
	DBusError		err;
	unsigned int 	groupnum = 0,ret=ACL_RETURN_CODE_ERROR;
	struct acl_group_list_s *groupPtr=NULL;
	unsigned int     dir_info=0;
	
    dbus_error_init(&err);
   
    if (!(dbus_message_get_args ( msg, &err,  
	   DBUS_TYPE_UINT32,&dir_info,
	   DBUS_TYPE_UINT32,&groupnum,
	   DBUS_TYPE_INVALID))) {
	   syslog_ax_acl_err("Unable to get input args ");
	   if (dbus_error_is_set(&err)) {
		   syslog_ax_acl_err("%s raised: %s",err.name,err.message);
		   dbus_error_free(&err);
	   }
	   return NULL;
    }
    syslog_ax_acl_dbg("output \n");
    syslog_ax_acl_dbg("acl-group num is :%d\n",groupnum);
	if((groupnum>1023)||(groupnum<1))
		ret=ACL_RETURN_CODE_GROUP_INDEX_ERROR;
	else{
		switch(dir_info){
			case ACL_DIRECTION_INGRESS:
				 groupPtr = acl_group[groupnum];
				 if (NULL == groupPtr) {
					syslog_ax_acl_dbg(" %d acl group not existed!\n",groupnum);
					ret = ACL_RETURN_CODE_GROUP_NOT_EXISTED;							
				 }
				 else if((groupPtr->eth_count != 0)||(groupPtr->vlan_count != 0)) {
				 	syslog_ax_acl_dbg(" acl group eth_count %d \n", groupPtr->eth_count);
				      syslog_ax_acl_dbg(" acl group vlan_count %d\n", groupPtr->vlan_count);
				 	ret = ACL_RETURN_CODE_UNBIND_FRIST;
				 }	
				 else if (NULL != groupPtr) {		 		  
					  ret = ACL_RETURN_CODE_GROUP_SUCCESS;	
					  #if 0
					  /*api ,check if been bind ,*/
					  rs=npd_acl_group_bind_check(groupPtr);
					  if(rs==ACL_RETURN_CODE_SUCCESS)
					  {
							syslog_ax_acl_dbg("acl group %d has been bind to port or vlan,not allow to config!\n",groupnum);
							ret = ACL_RETURN_CODE_GROUP_PORT_BINDED;
					  }
					  else if(rs==ACL_RETURN_CODE_ERROR)
					  	ret = ACL_RETURN_CODE_GROUP_SUCCESS;		  
					  #endif
				 }
				 break;
	        case ACL_DIRECTION_EGRESS:
				 groupPtr = egress_acl_group[groupnum];
				 if (NULL==groupPtr) {
					   syslog_ax_acl_dbg(" %d egress acl group not existed!\n",groupnum);
					   ret=ACL_RETURN_CODE_GROUP_NOT_EXISTED;							
				 }
				 else if((groupPtr->eth_count != 0)||(groupPtr->vlan_count != 0)) {
				 	ret = ACL_RETURN_CODE_UNBIND_FRIST;
				 }	
				 else if (NULL!=groupPtr) {		 		  
					ret = ACL_RETURN_CODE_GROUP_SUCCESS;		
					#if 0
					  /*api ,check if been bind ,*/
					  rs=npd_acl_group_bind_check(groupPtr);
					  if(rs==ACL_RETURN_CODE_SUCCESS)
					  {
							syslog_ax_acl_dbg("egress acl group %d has been bind to port or vlan,not allow to config!\n",groupnum);
							ret = ACL_RETURN_CODE_GROUP_PORT_BINDED;
					  }
					  else if(rs==ACL_RETURN_CODE_ERROR)
					  	ret = ACL_RETURN_CODE_GROUP_SUCCESS;		  
					#endif
				 }
				 break;
	        default :
					ret = ACL_RETURN_CODE_ERROR;
					break;
			}
		}
	syslog_ax_acl_dbg("ret %d\n",ret);
   reply = dbus_message_new_method_return(msg);
   
   dbus_message_iter_init_append (reply, &iter);
   
   dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,
									&ret);
   return reply;
}

DBusMessage * npd_dbus_add_delete_rule_to_acl_group(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*	    reply;
	DBusMessageIter	iter;
	DBusError		err; 
	unsigned int 	group_num = 0,add_index = 0,op_flag = 0,delete_index = 0;
	int				ret = 0,group_inf = 0;
	unsigned int 	op_info=ACL_RETURN_CODE_SUCCESS,result,num=0;
	unsigned int     dir_info=0,exist_dir = 0, tmp = 0, policerid = 0;

	struct acl_rule_info_s *ruleInfo = NULL;
	struct list_head       *Ptr = NULL;
	struct acl_group_list_s *groupInfo = NULL;

	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
		   DBUS_TYPE_UINT32,&dir_info,
		   DBUS_TYPE_UINT32,&op_flag,
		   DBUS_TYPE_UINT32,&group_num,
		   DBUS_TYPE_UINT32,&add_index,
		   DBUS_TYPE_INVALID))) {
	   syslog_ax_acl_err("Unable to get input args ");
	   if (dbus_error_is_set(&err)) {
		   syslog_ax_acl_err("%s raised: %s",err.name,err.message);
		   dbus_error_free(&err);
	   }
	   return NULL;
	}
   
	delete_index = add_index;
	if ((qos_mode != QOS_MODE_FLOW)&&(qos_mode != QOS_MODE_HYBRID)) {
		if (qos_mode == QOS_MODE_DEFAULT) {
			op_info = ACL_RETURN_CODE_NO_QOS_MODE;
			tmp = 1;
		}
		else {
			op_info = ACL_RETURN_CODE_ALREADY_PORT;
		 	tmp = 1;
		}	
	}	   
	if(ACL_DIRECTION_INGRESS == dir_info) {
		groupInfo = acl_group[group_num];
	}
	else if(ACL_DIRECTION_EGRESS == dir_info) {
		groupInfo = egress_acl_group[group_num];
	}	
		   
	if (groupInfo == NULL) {
		syslog_ax_acl_dbg("Acl Group %d not existed!\n",group_num);
		syslog_ax_acl_dbg("you should creat a acl group firstly!\n");
		group_inf = ACL_RETURN_CODE_ERROR;							  
	}
	else {	
	      group_inf = ACL_RETURN_CODE_SUCCESS;	
		/*add*/
		if(0==op_flag) {		
			ret = search_acl_group_rule_index_exist(add_index,&num,&exist_dir);
			if(ACL_RETURN_CODE_GROUP_RULE_EXISTED == ret) {								
				syslog_ax_acl_dbg("you have add the rule %d to ingress group %d\n",add_index,num);			 
			}
			else if(ACL_RETURN_CODE_EGRESS_GROUP_RULE_EXISTED == ret) {
				syslog_ax_acl_dbg("you have add the rule %d to egress group %d\n",add_index,num);
			}
			else if(ACL_RETURN_CODE_GLOBAL_NOT_EXISTED == ret) {
				syslog_ax_acl_dbg("you have not set the rule %d\n",add_index);
			}
			else {
				 if (g_acl_rule[add_index]->ActionPtr.policerPtr.policerEnable == 1) {
					 policerid = g_acl_rule[add_index]->ActionPtr.policerPtr.policerId;
					 if (NULL != g_policer[policerid]) {
						 if (g_policer[policerid]->qosProfile == 0) {
							tmp = 0;
						 }
					 }
				 }
				 ret = npd_acl_group_add_equal_rule(add_index, group_num, dir_info);
				 syslog_ax_acl_dbg("npd_acl_group_add_equal_rule ret is  %d\n", ret);
				 if(ret == ACL_RETURN_CODE_ADD_EQUAL_RULE) {
				 	op_info = ACL_RETURN_CODE_ADD_EQUAL_RULE;
				 }	
				 else {
					 if (((g_acl_rule[add_index]->TcamPtr.packetType == ACL_TCAM_RULE_PACKETTYPE_QOS) ||
						(g_acl_rule[add_index]->appendInfo.appendType == 32) ||
						(g_acl_rule[add_index]->ActionPtr.policerPtr.policerEnable == 1)) &&
						(tmp == 1)) {
							ret = ACL_RETURN_CODE_ERROR;
					 	}
					 else {
						  if(ACL_RETURN_CODE_SUCCESS == ret) {	
							if(ACL_DIRECTION_INGRESS == dir_info){
								if(ACL_RETURN_CODE_SUCCESS == npd_acl_drv_rule_in_action_check(add_index)){
									op_info = npd_acl_drv_ingress_rule_check(add_index, group_num);
									if (op_info == ACL_RETURN_CODE_SUCCESS) {
										op_info= npd_acl_drv_add_rule2group(groupInfo,add_index,group_num,dir_info);
										if(op_info!=ACL_RETURN_CODE_SUCCESS)
											syslog_ax_acl_dbg("ingress group add fail!~~~~~~\n");
									}
									else {
										op_info = ACL_RETURN_CODE_STD_RULE;
									}
							    }
							    else {
								    op_info = ACL_RETURN_CODE_GROUP_EGRESS_ERROR;
								    syslog_ax_acl_dbg("ingress group not support egress qos rule \n");
							    }
					     		}
							else if(dir_info == ACL_DIRECTION_EGRESS){
								if(ACL_RETURN_CODE_SUCCESS == npd_acl_drv_rule_action_check(add_index)){
									op_info = npd_acl_drv_egress_rule_check(add_index, group_num);
									if (op_info == ACL_RETURN_CODE_SUCCESS) {
										op_info = npd_acl_drv_add_rule2group(groupInfo,add_index,group_num,dir_info);
										if(ACL_RETURN_CODE_SUCCESS != op_info)
											syslog_ax_acl_dbg("egress group add fail!~~~~~~\n");
									}
									else {
										op_info = ACL_RETURN_CODE_STD_RULE;
									}	
								}	
								else{
									op_info = ACL_RETURN_CODE_GROUP_EGRESS_NOT_SUPPORT;
									syslog_ax_acl_dbg("trap,mirror,redirect,ingressQoSInit not support!\n");
								}
							}
			       	 }
					 else {
						npd_syslog_dbg("fail to add acl rule ret is %d \n", ret);
					 }
				}
			}
		}
	}			
	/*delete*/
		else if(1==op_flag) {
			ret=search_acl_group_rule_index_exist(delete_index,&num,&exist_dir);
			if(ACL_RETURN_CODE_GLOBAL_NOT_EXISTED==ret) {
				syslog_ax_acl_dbg("you have not set the rule %d\n",delete_index);
			}
			else if(ACL_RETURN_CODE_SUCCESS==ret) {
				syslog_ax_acl_dbg("group %d has no this rule %d\n",group_num,delete_index);
			}
			else if(((ACL_RETURN_CODE_GROUP_RULE_EXISTED == ret)&&(ACL_DIRECTION_INGRESS == dir_info)) \
				   ||((ACL_RETURN_CODE_EGRESS_GROUP_RULE_EXISTED == ret)&&(ACL_DIRECTION_EGRESS == dir_info))) {
				if(num==group_num) {					 
			    		 __list_for_each(Ptr,&(groupInfo->list1)) {

						  ruleInfo=list_entry(Ptr,struct acl_rule_info_s,list);
						  if(NULL == ruleInfo) {
							  syslog_ax_acl_dbg("null rule node on bind group %d when search\n",group_num);
						  }
						  else if(delete_index==ruleInfo->ruleIndex) {
							  result=nam_acl_drv_tcam_clear(delete_index, 0);
							  if(result!=ACL_RETURN_CODE_SUCCESS)
								  syslog_ax_acl_err("delete index %d in TCAM ERROR!\n",delete_index);
							  else if(ACL_RETURN_CODE_SUCCESS==result){
								syslog_ax_acl_dbg("clear tcam success\n");
								op_info=ACL_RETURN_CODE_SUCCESS; 
								__list_del((ruleInfo->list).prev, (ruleInfo->list).next);
								syslog_ax_acl_dbg("rule %d been deleted!\n",delete_index);							
								free(ruleInfo);
								ruleInfo = NULL;
								groupInfo->count-=1; /*count--*/
							}
							 break;							  	
					  	}
					} 
				} /*if*/
			       else {
				  	 op_info=ACL_RETURN_CODE_ERROR;
				  	 syslog_ax_acl_dbg("rule %d in group %d ,so can not delete\n",delete_index,num);
			      }
		     }
	   }
}
	  
   reply = dbus_message_new_method_return(msg);
   
   dbus_message_iter_init_append (reply, &iter);
   
   dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,
									&group_inf);
   dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,
									&ret);
   dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,
									&num);
   dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,
									&op_info);
   
   return reply;	 

}

DBusMessage * npd_dbus_delete_acl_group(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*			reply;	  
	DBusMessageIter 		iter;
	DBusError				err;
	unsigned int			groupnum,ret=ACL_RETURN_CODE_ERROR,group_info = 0,tmp = 0,dir_info=0;
 	struct acl_group_list_s  *groupPtr=NULL; 
	
	dbus_error_init(&err);
   
	if (!(dbus_message_get_args ( msg, &err,
	   DBUS_TYPE_UINT32,&dir_info,
	   DBUS_TYPE_UINT32,&groupnum,
	   DBUS_TYPE_INVALID))) {
	   syslog_ax_acl_err("Unable to get input args ");
	   if (dbus_error_is_set(&err)) {
		   syslog_ax_acl_err("%s raised: %s",err.name,err.message);
		   dbus_error_free(&err);
	   }
	   return NULL;
	}
	syslog_ax_acl_dbg("output \n");
	syslog_ax_acl_dbg("acl-group num is :%d\n",groupnum);

	if((groupnum>1023)||(groupnum<1)) {
		ret=ACL_RETURN_CODE_GROUP_INDEX_ERROR;
	}
	else {
		if(ACL_DIRECTION_INGRESS==dir_info)
			groupPtr = acl_group[groupnum];
	 	else if(ACL_DIRECTION_EGRESS==dir_info)
			groupPtr = egress_acl_group[groupnum];
		
		if (NULL==groupPtr){
			   syslog_ax_acl_dbg("no %d acl group \n",groupnum);	  
				group_info = ACL_RETURN_CODE_ERROR;
				/*ret = ACL_RETURN_CODE_SUCCESS;*/
			 						
		 }			
	    else if (NULL!=groupPtr) {		
			group_info = ACL_RETURN_CODE_SUCCESS;
			tmp=npd_acl_group_bind_check(groupPtr);
			if(ACL_RETURN_CODE_ERROR==tmp)/*api*/
			{
				ret=npd_drv_delete_group_acl(groupnum,dir_info);
				if(ret!=ACL_RETURN_CODE_SUCCESS) {
					syslog_ax_acl_dbg("delete acl group failure\n");
				}
				else {
					groupPtr = NULL;
				}
			}
			else if(ACL_RETURN_CODE_SUCCESS==tmp)
			{
		      syslog_ax_acl_dbg("acl group %d has been binded on port\n",groupnum);
			  syslog_ax_acl_dbg("please unbind acl group  firstly!\n");
			  ret=ACL_RETURN_CODE_GROUP_PORT_BINDED;
			}
	    	}	
	}
   reply = dbus_message_new_method_return(msg);
   
   dbus_message_iter_init_append (reply, &iter);

   dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,
									&group_info);  
   dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,
									&ret);
   

   return reply;

}
DBusMessage * npd_dbus_show_acl_group_index(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*			 reply = NULL;
	DBusMessageIter 		 iter;
	DBusMessageIter 		 iter_array;
	DBusError				err;	
	unsigned int			i=0,index=0,count=0,ret=ACL_RETURN_CODE_SUCCESS;
	unsigned int			n=0,vid_count=0;
	static GROUP_RULE_INDEX_STC	*RuleId[MAX_ACL_RULE_NUMBER];
	static GROUP_ETH_INDEX_STC	*ethindex[MAX_PORT_NUMBER];
	static GROUP_ETH_INDEX_STC *vid_index[MAX_VID_IDNUM];
	struct acl_group_list_s *groupNodePtr=NULL;
	struct list_head		 *ptr=NULL;
	struct acl_rule_info_s	 *ruleInfo=NULL;
	struct group_eth_info	*gethinfo=NULL;
	struct group_vlan_info	*vidinfo=NULL;
	unsigned int			 dir_info=0,j=0,m=0,portcount=0,k=0;
	unsigned char			devNum=0,portNum=0;
	unsigned int    		slot_no=0,local_port_no=0;
	
	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
	   DBUS_TYPE_UINT32,&dir_info,
	   DBUS_TYPE_UINT32,&index,
	   DBUS_TYPE_INVALID))) {
	   npd_syslog_err(("Unable to get input args "));
	   if (dbus_error_is_set(&err)) {
		   npd_syslog_err(("%s raised: %s",err.name,err.message));
		   dbus_error_free(&err);
	   }
	   return NULL;
	}
	
	if((index>1023)||(index<1)) {
		ret=ACL_RETURN_CODE_GROUP_INDEX_ERROR;
	}
	else {
		if(ACL_DIRECTION_INGRESS==dir_info) {
			groupNodePtr = acl_group[index];
		}
		else if(ACL_DIRECTION_EGRESS==dir_info) {
			groupNodePtr = egress_acl_group[index];
		}
					
		if(NULL != groupNodePtr) {
			count = groupNodePtr->count;
			portcount = groupNodePtr->eth_count;
			vid_count = groupNodePtr->vlan_count;
			__list_for_each(ptr,&(groupNodePtr->list1)) {
				ruleInfo=list_entry(ptr,struct acl_rule_info_s,list); 
				if(NULL!=ruleInfo){
					RuleId[j]= (GROUP_RULE_INDEX_STC*)malloc(sizeof(GROUP_RULE_INDEX_STC));
					if(NULL == RuleId[j]) {
						syslog_ax_acl_err("acl group %d has no rule\n",index);
					}
					else {
						memset(RuleId[j],0,sizeof(GROUP_RULE_INDEX_STC));
						RuleId[j]->rule_index = 1+(ruleInfo->ruleIndex);
						syslog_ax_acl_dbg("rule[%d] is %d\n", j, RuleId[j]->rule_index);
						syslog_ax_acl_dbg("ruleInfo address is %p\n", ruleInfo);
						syslog_ax_acl_dbg("Rule[%d] address is %p\n", j, RuleId[j]);
						j++;
						syslog_ax_acl_dbg("j %d\n",j);
						ruleInfo = NULL;
					}
				 } 
				 else
					syslog_ax_acl_dbg("acl group %d has no rule\n",index);
			}				
			__list_for_each(ptr,&(groupNodePtr->list2)) {
					gethinfo=list_entry(ptr,struct group_eth_info,list);	
					if(NULL!=gethinfo){
						ethindex[m]= (GROUP_ETH_INDEX_STC*)malloc(sizeof(GROUP_ETH_INDEX_STC));
						memset(ethindex[m],0,sizeof(GROUP_ETH_INDEX_STC));
						ethindex[m]->ethindex = 1+(gethinfo->eth_index);
						/*code optimize: Unchecked return value
						zhangcl@autelan.com 2013-1-17*/
						ret = npd_get_devport_by_global_index(gethinfo->eth_index,&devNum,&portNum);
						if(ret != 0)
						{
							syslog_ax_acl_dbg("get devport by global index failed \n");
							continue;
						}
						ret = nam_get_slotport_by_devport(devNum, portNum, PRODUCT_ID, &(slot_no), &(local_port_no));
						if(ret != 0)
						{
							syslog_ax_acl_dbg("get slotport by devport failed \n");
							continue;
						}
						syslog_ax_acl_dbg("slot num %d\n", slot_no);
						syslog_ax_acl_dbg("port num %d\n", local_port_no);
						syslog_ax_acl_dbg("group%d is bind by port %d\n",m,ethindex[m]->ethindex);
						ethindex[m]->slot = slot_no;
						ethindex[m]->port = local_port_no;
						m++;
						syslog_ax_acl_dbg("m %d\n",m);
					}				  
					else
						syslog_ax_acl_dbg("acl group %d has no rule\n",index);
			}	
			__list_for_each(ptr,&(groupNodePtr->list3)) {
				  vidinfo=list_entry(ptr,struct group_vlan_info,list);	
				      if(NULL!=vidinfo){
						vid_index[n]= (GROUP_ETH_INDEX_STC*)malloc(sizeof(GROUP_ETH_INDEX_STC));
						memset(vid_index[n],0,sizeof(GROUP_ETH_INDEX_STC));
						vid_index[n]->vidindex = vidinfo->vlan_index;
						syslog_ax_acl_dbg("binded by vlan %d\n", vid_index[n]->vidindex);
						n++;
					}				  
					else {
						syslog_ax_acl_dbg("acl group %d has no rule\n",index);
					}
				}
		 }
		else
			ret=ACL_RETURN_CODE_GROUP_NOT_EXISTED;
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret); 
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &count);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &portcount);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &vid_count);
	dbus_message_iter_open_container (&iter,
									   DBUS_TYPE_ARRAY,
									   DBUS_STRUCT_BEGIN_CHAR_AS_STRING  
									   DBUS_TYPE_UINT32_AS_STRING    /*slot_no*/
									   DBUS_TYPE_UINT32_AS_STRING    /*port_no*/
									   DBUS_STRUCT_END_CHAR_AS_STRING,				
									&iter_array);
	for(k=0;k<portcount;k++){
		DBusMessageIter iter_struct;				
		
		dbus_message_iter_open_container (&iter_array,
											DBUS_TYPE_STRUCT,
											NULL,
										  &iter_struct);
		dbus_message_iter_append_basic (&iter_struct,
										 DBUS_TYPE_UINT32,
										 &(ethindex[k]->slot));
		dbus_message_iter_append_basic (&iter_struct,
										 DBUS_TYPE_UINT32,
										 &(ethindex[k]->port));
		dbus_message_iter_close_container (&iter_array, &iter_struct);
		free(ethindex[k]);
		ethindex[k] = NULL;
   }
   dbus_message_iter_close_container (&iter, &iter_array);

	dbus_message_iter_open_container (&iter,
									   DBUS_TYPE_ARRAY,
									   DBUS_STRUCT_BEGIN_CHAR_AS_STRING  
									   DBUS_TYPE_UINT32_AS_STRING    /*vid_no*/
									   DBUS_STRUCT_END_CHAR_AS_STRING,				
									&iter_array);
	for(k=0;k<vid_count;k++){
		DBusMessageIter iter_struct;				
		
		dbus_message_iter_open_container (&iter_array,
											DBUS_TYPE_STRUCT,
											NULL,
										  &iter_struct);
		dbus_message_iter_append_basic (&iter_struct,
										 DBUS_TYPE_UINT32,
										 &(vid_index[k]->vidindex));
		dbus_message_iter_close_container (&iter_array, &iter_struct);
		free(vid_index[k]);
		vid_index[k] = NULL;
   }
   dbus_message_iter_close_container (&iter, &iter_array);


   dbus_message_iter_open_container (&iter,
									   DBUS_TYPE_ARRAY,
									   DBUS_STRUCT_BEGIN_CHAR_AS_STRING  
									   DBUS_TYPE_UINT32_AS_STRING    /*acl index*/
									   DBUS_STRUCT_END_CHAR_AS_STRING,				
									&iter_array);
   for (i = 0; i < count; i++ ) {
		DBusMessageIter iter_struct;
		
		dbus_message_iter_open_container (&iter_array,
										   DBUS_TYPE_STRUCT,
										   NULL,
										   &iter_struct);	
		dbus_message_iter_append_basic (&iter_struct,
										 DBUS_TYPE_UINT32,
										 &(RuleId[i]->rule_index));						
		dbus_message_iter_close_container (&iter_array, &iter_struct);
		free(RuleId[i]);
		RuleId[i]=NULL;
	}
			
	dbus_message_iter_close_container (&iter, &iter_array);

	return reply;

}

DBusMessage * npd_dbus_show_acl_group(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*			 reply = NULL;
	DBusMessageIter 		 iter;
	DBusMessageIter 		 iter_array;
	DBusError				err;	
	unsigned int    		i=0,k=0,index=0,ts=0,ret=ACL_RETURN_CODE_SUCCESS,group_count=0,count=0,port_count=0;
	unsigned int			vid_count=0,n=0,remvid=0;

	static GROUP_SHOW_STC  		*group[MAX_GROUP_NUM];      /*In Xcat if the array > 1000 we must  use static array*/
	static GROUP_RULE_INDEX_STC    *RuleId[MAX_ACL_RULE_NUMBER];
	static GROUP_ETH_INDEX_STC	*ethindex[MAX_PORT_NUMBER];
	static GROUP_ETH_INDEX_STC	*vid_index[MAX_VID_IDNUM];

	struct acl_group_list_s *groupNodePtr=NULL;
	struct list_head		 *ptr=NULL;
	struct acl_rule_info_s   *ruleInfo=NULL;
	struct group_eth_info	*gethinfo=NULL;
	struct group_vlan_info	*vidinfo=NULL;
	unsigned int             dir_info=0,j=0,m=0,nul_number=0,rem=0,remport=0;
	unsigned char			devNum=0,portNum=0;
	unsigned int    		slot_no=0,local_port_no=0;
	
	syslog_ax_acl_dbg("show acl group !\n");

	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
	   DBUS_TYPE_UINT32,&dir_info,
	   DBUS_TYPE_INVALID))) {
	   syslog_ax_acl_err("Unable to get input args ");
	   if (dbus_error_is_set(&err)) {
		   syslog_ax_acl_err("%s raised: %s",err.name,err.message);
		   dbus_error_free(&err);
	   }
	   return NULL;
	}
	
    for(index=0;index<MAX_GROUP_NUM;index++)
	  {
	  	if(ACL_DIRECTION_INGRESS==dir_info)
			groupNodePtr = acl_group[index];
 		else if(ACL_DIRECTION_EGRESS==dir_info)
			groupNodePtr = egress_acl_group[index];
		
		if(NULL!=groupNodePtr)
			{
				group[group_count] = (GROUP_SHOW_STC*)malloc(sizeof(GROUP_SHOW_STC));
			      memset(group[group_count],0,sizeof(GROUP_SHOW_STC));				
			      group[group_count]->group_num = index;
				group[group_count]->count = groupNodePtr->count;
				group[group_count]->portnum = groupNodePtr->eth_count;
				group[group_count]->vid = groupNodePtr->vlan_count;
				/*group[group_count]->vid = groupNodePtr->vlan_count;*/
				syslog_ax_acl_dbg("group[%d]->count %d\n",group_count,group[group_count]->count);
				__list_for_each(ptr,&(groupNodePtr->list1)) {
					 ruleInfo=list_entry(ptr,struct acl_rule_info_s,list);	
					if(NULL!=ruleInfo){
					 /* for(j=0;j<count;j++){*/	
				 		RuleId[j]= (GROUP_RULE_INDEX_STC*)malloc(sizeof(GROUP_RULE_INDEX_STC));
				  		memset(RuleId[j],0,sizeof(GROUP_RULE_INDEX_STC));
			 			RuleId[j]->rule_index = 1+(ruleInfo->ruleIndex);
						syslog_ax_acl_dbg("rule[%d] is %d\n",j,RuleId[j]->rule_index);
						j++;
						syslog_ax_acl_dbg("j %d\n",j);
					  	/*}*/
				       }
				}
				__list_for_each(ptr,&(groupNodePtr->list2)) 
				{
					gethinfo=list_entry(ptr,struct group_eth_info,list);
					if(NULL!=gethinfo)
					{
						ethindex[m]= (GROUP_ETH_INDEX_STC*)malloc(sizeof(GROUP_ETH_INDEX_STC));
				  		memset(ethindex[m],0,sizeof(GROUP_ETH_INDEX_STC));
			 			ethindex[m]->ethindex = 1+(gethinfo->eth_index);
						/*code optimize: Unchecked return value
						zhangcl@autelan.com 2013-1-17*/
						ret = npd_get_devport_by_global_index(gethinfo->eth_index,&devNum,&portNum);
						if(ret != 0)
						{
							syslog_ax_acl_dbg("get devport by global index failed \n");
							continue;
						}
						ret = nam_get_slotport_by_devport(devNum, portNum, PRODUCT_ID, &(slot_no), &(local_port_no));
						if(ret != 0)
						{
							syslog_ax_acl_dbg("get slot port by dev port failed !\n");
							continue;
						}
						syslog_ax_acl_dbg("slot num %d\n", slot_no);
						syslog_ax_acl_dbg("port num %d\n", local_port_no);
						syslog_ax_acl_dbg("group%d is bind by port %d\n",m,ethindex[m]->ethindex);
						ethindex[m]->slot = slot_no;
						ethindex[m]->port = local_port_no;
						m++;
						syslog_ax_acl_dbg("m %d\n",m);
				  }				  
				  else 
				  {
				  	syslog_ax_acl_dbg("acl group %d has no rule\n",index);
				  }
			   }
			   __list_for_each(ptr,&(groupNodePtr->list3)) {
				  vidinfo=list_entry(ptr,struct group_vlan_info,list);	
				      if(NULL!=vidinfo){
						vid_index[n]= (GROUP_ETH_INDEX_STC*)malloc(sizeof(GROUP_ETH_INDEX_STC));
						memset(vid_index[n],0,sizeof(GROUP_ETH_INDEX_STC));
						vid_index[n]->vidindex = vidinfo->vlan_index;
						syslog_ax_acl_dbg("binded by vlan %d\n", vid_index[n]->vidindex);
						n++;
					}				  
					else {
						syslog_ax_acl_dbg("acl group %d has no rule\n",index);
					}
				}	
			    group_count++;
		     }	
		else {
			nul_number++;
		}
      }
	if(MAX_GROUP_NUM==nul_number){
			ret=ACL_RETURN_CODE_ERROR;
			syslog_ax_acl_dbg("no acl group existed!\n");
	}
		
	ts=npd_show_acl_group(dir_info);
		if(ts!=ACL_RETURN_CODE_SUCCESS)
			syslog_ax_acl_dbg("no acl group info \n");

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &group_count);
	syslog_ax_acl_dbg("return basic param group_count %d",group_count);
   

	dbus_message_iter_open_container (&iter,
								DBUS_TYPE_ARRAY,
								DBUS_STRUCT_BEGIN_CHAR_AS_STRING     /*begin	*/								 
								DBUS_TYPE_UINT32_AS_STRING    /*group num*/
								DBUS_TYPE_UINT32_AS_STRING    /*count*/
								DBUS_TYPE_UINT32_AS_STRING    /*portnum*/
								DBUS_TYPE_UINT32_AS_STRING    /*vidnum*/
								DBUS_TYPE_ARRAY_AS_STRING
								DBUS_STRUCT_BEGIN_CHAR_AS_STRING  
								DBUS_TYPE_UINT32_AS_STRING    /*slot_no*/
								DBUS_TYPE_UINT32_AS_STRING		/*port_no*/
								DBUS_STRUCT_END_CHAR_AS_STRING
								DBUS_TYPE_ARRAY_AS_STRING
								DBUS_STRUCT_BEGIN_CHAR_AS_STRING  
								DBUS_TYPE_UINT32_AS_STRING    /*vid_no*/
								DBUS_STRUCT_END_CHAR_AS_STRING
								DBUS_TYPE_ARRAY_AS_STRING 
								DBUS_STRUCT_BEGIN_CHAR_AS_STRING  
								DBUS_TYPE_UINT32_AS_STRING    /*ruleindex*/
								DBUS_STRUCT_END_CHAR_AS_STRING 
								DBUS_STRUCT_END_CHAR_AS_STRING,     /*end*/
								    &iter_array);
	syslog_ax_acl_dbg("open outer container");
	if(group_count>0){
		 for (i = 0; i < group_count; i++ ) {
			DBusMessageIter iter_struct;
			DBusMessageIter iter_sub_array;
			
			dbus_message_iter_open_container (&iter_array,
											   DBUS_TYPE_STRUCT,
											   NULL,
											   &iter_struct);
			syslog_ax_acl_dbg("open first-inner container");		
			syslog_ax_acl_dbg("loop iter i=%d",i);
			syslog_ax_acl_dbg("%-20s %d\n","groupnum",group[i]->group_num);
			syslog_ax_acl_dbg("%-20s %ld\n","count",group[i]->count);
			syslog_ax_acl_dbg("%-20s %ld\n","portnum",group[i]->portnum);
			syslog_ax_acl_dbg("%-20s %ld\n","vid",group[i]->vid);
								
			dbus_message_iter_append_basic
					(&iter_struct,
					  DBUS_TYPE_UINT32,
					  &(group[i]->group_num));
			dbus_message_iter_append_basic 
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &(group[i]->count));
			dbus_message_iter_append_basic 
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &(group[i]->portnum));
			dbus_message_iter_append_basic 
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &(group[i]->vid));

			port_count = group[i]->portnum;
					dbus_message_iter_open_container (&iter_struct,
											   DBUS_TYPE_ARRAY,
									      	   DBUS_STRUCT_BEGIN_CHAR_AS_STRING  
									           DBUS_TYPE_UINT32_AS_STRING    /*slot_no*/
									           DBUS_TYPE_UINT32_AS_STRING	/*port_no*/
									           DBUS_STRUCT_END_CHAR_AS_STRING, 
											   &iter_sub_array);
					for(k=0;k<port_count;k++){
			
					DBusMessageIter iter_sub_struct;				
					
					dbus_message_iter_open_container (&iter_sub_array,
														DBUS_TYPE_STRUCT,
														NULL,
													  &iter_sub_struct);
					dbus_message_iter_append_basic 
							(&iter_sub_struct,
							 DBUS_TYPE_UINT32,
							 &(ethindex[k+remport]->slot));
					dbus_message_iter_append_basic 
							(&iter_sub_struct,
							 DBUS_TYPE_UINT32,
							 &(ethindex[k+remport]->port));
					syslog_ax_acl_dbg("slot num :%d\n",ethindex[k+remport]->slot);
					syslog_ax_acl_dbg("port num :%d\n",ethindex[k+remport]->port);
					dbus_message_iter_close_container (&iter_sub_array, &iter_sub_struct);
					free(ethindex[k+remport]);
					ethindex[k+remport] = NULL;
				}
					/*close the container */
				dbus_message_iter_close_container (&iter_struct, &iter_sub_array);	
				remport += port_count;
				
				vid_count = group[i]->vid;
						dbus_message_iter_open_container (&iter_struct,
												   DBUS_TYPE_ARRAY,
												   DBUS_STRUCT_BEGIN_CHAR_AS_STRING  
												   DBUS_TYPE_UINT32_AS_STRING	 /*vid_no*/
												   DBUS_STRUCT_END_CHAR_AS_STRING, 
												   &iter_sub_array);
						for(k=0;k<vid_count;k++){
				
						DBusMessageIter iter_sub_struct;				
						
						dbus_message_iter_open_container (&iter_sub_array,
															DBUS_TYPE_STRUCT,
															NULL,
														  &iter_sub_struct);
						dbus_message_iter_append_basic 
								(&iter_sub_struct,
								 DBUS_TYPE_UINT32,
								 &(vid_index[k+remvid]->vidindex));
						syslog_ax_acl_dbg("binded by vlan %d\n",vid_index[k+remvid]->vidindex);
						dbus_message_iter_close_container (&iter_sub_array, &iter_sub_struct);
						free(vid_index[k+remvid]);
						vid_index[k+remvid] = NULL;
					}
						/*close the container */
					dbus_message_iter_close_container (&iter_struct, &iter_sub_array);
				       remvid += vid_count;

				
			    count=group[i]->count;
					dbus_message_iter_open_container (&iter_struct,
											   DBUS_TYPE_ARRAY,
										       DBUS_STRUCT_BEGIN_CHAR_AS_STRING  
										       DBUS_TYPE_UINT32_AS_STRING    /*ruleindex*/
										       DBUS_STRUCT_END_CHAR_AS_STRING, 
											   &iter_sub_array);										   
			for(k=0;k<count;k++){

				DBusMessageIter iter_sub_struct;				
				
				dbus_message_iter_open_container (&iter_sub_array,
											      DBUS_TYPE_STRUCT,
											   	  NULL,
											   	  &iter_sub_struct);
				dbus_message_iter_append_basic 
						(&iter_sub_struct,
						 DBUS_TYPE_UINT32,
						 &(RuleId[k+rem]->rule_index));
				syslog_ax_acl_dbg("rule num :%d\n",RuleId[k+rem]->rule_index);
			 	dbus_message_iter_close_container (&iter_sub_array, &iter_sub_struct);
				free(RuleId[k+rem]);
				RuleId[k+rem] = NULL;
			}
			rem += count;
							
			dbus_message_iter_close_container (&iter_struct, &iter_sub_array);			
			dbus_message_iter_close_container (&iter_array, &iter_struct);
			free(group[i]);
			group[i] = NULL;
		} /*for( i < group_count)*/
	} /*if(group_count>0)*/
	dbus_message_iter_close_container (&iter, &iter_array);

	return reply;

}

DBusMessage * npd_dbus_ethports_interface_config_acl(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*     		reply;
	DBusMessageIter	 		iter;
	DBusError        		err;
	unsigned int 	 		dir_info=0,g_index = 0,EnableInfo=ACL_RETURN_CODE_SUCCESS;
	unsigned char	 		acl_enable = 0;	
	/*code optimize:  Uninitialized scalar variable
	zhangcl@autelan.com 2013-1-17*/
	unsigned int            in_flag = 0,flag = 0,i=0,j=0,op_ret=0,vlanEnable=ACL_RETURN_CODE_ERROR;
	unsigned short			vlanId = 0;
	struct vlan_ports       untagPorts,tagPorts;
	int 					mm=0;
	unsigned int ret = 0, tmp = 0, groupNo = 0;

	if(!((productinfo.capbmp) & FUNCTION_ACL_VLAUE)){
		npd_syslog_dbg("do not support acl!\n");
		op_ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_ACL_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ACL_VLAUE)){
		npd_syslog_dbg("do not support acl!\n");
		op_ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else{
		dbus_error_init(&err);
		
		if (!(dbus_message_get_args ( msg, &err,
			DBUS_TYPE_UINT32,&dir_info,
			DBUS_TYPE_UINT32,&in_flag,
			DBUS_TYPE_UINT32,&g_index,
			DBUS_TYPE_BYTE,&acl_enable,
			DBUS_TYPE_INVALID))) {
			syslog_ax_acl_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				syslog_ax_acl_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}
		syslog_ax_acl_dbg("index %#0x %s\n",g_index,acl_enable ? "enable":"disable");

		syslog_ax_acl_dbg("flag %d\n",in_flag);
		syslog_ax_acl_dbg("g_index %d\n",g_index);
		syslog_ax_acl_dbg("acl_enable %d\n",acl_enable);

		if(0==in_flag) {
			flag=ACCESS_PORT_TYPE;
		}
		else if(1==in_flag) {
			flag=ACCESS_VID_TYPE;
		}

		if(ACCESS_VID_TYPE==flag) {
			vlanId = (unsigned short)g_index;
			tmp = npd_vlan_acl_bind_check(vlanId,&groupNo,dir_info);
			if ((ACL_RETURN_CODE_GROUP_VLAN_BINDED == tmp) &&  (0 == acl_enable)){
				npd_syslog_dbg("group %d bind to vlan yet\n",groupNo);
				op_ret = ACL_RETURN_CODE_GROUP_VLAN_BINDED;/*bind*/
			}
			else {
				ret = nam_bcm_acl_support_check();
				if (ACL_RETURN_CODE_BCM_DEVICE_NOT_SUPPORT != ret) {
					syslog_ax_acl_dbg("vlan mode ...\n");
					ret = npd_acl_check_udp_vlan_disable(vlanId);
					if (!CHASSIS_VLANID_ISLEGAL(vlanId)) {
						op_ret = NPD_DBUS_ERROR_NO_SUCH_VLAN +1;
						/*return op_ret;*/
					}
					else if(VLAN_RETURN_CODE_VLAN_NOT_EXISTS == npd_check_vlan_exist(vlanId)) {
						op_ret = NPD_VLAN_NOTEXISTS;
						syslog_ax_acl_dbg("vlanId =%d NOT exists.\n",vlanId);
						/*return op_ret;*/
					}
					else if ((1 == ret )&& (0 == acl_enable)) {
						op_ret = ACL_RETURN_CODE_UDP_VLAN_RULE_ENABLE;
					}
					else {	
					  syslog_ax_acl_dbg("vlan existed...\n");

					  /*enable vlan*/
					  vlanEnable = npd_vlan_acl_enable(vlanId,acl_enable,dir_info);
					  if(ACL_RETURN_CODE_SUCCESS==vlanEnable){
						 syslog_ax_acl_dbg("g_vlans has been enabled!\n");
						 syslog_ax_acl_dbg("fetch ports\n");
						/*fetch ports*/
			  		    mm=npd_vlan_get_all_ports(vlanId,&untagPorts,&tagPorts);

						syslog_ax_acl_dbg("mm %d if 15 vlanNotExisted\n",mm);
						syslog_ax_acl_dbg("untagPorts.count %d\n",untagPorts.count);
						syslog_ax_acl_dbg("tagPorts.count %d\n",tagPorts.count);
						if(NPD_VLAN_NOTEXISTS==mm)
							op_ret = NPD_VLAN_NOTEXISTS;
						else if(NPD_VLAN_NOTEXISTS!=mm){
							syslog_ax_acl_dbg("searching port index...\n");
							syslog_ax_acl_dbg("untagPorts.count %d\n",untagPorts.count);
							for(i=0;i<untagPorts.count;i++)
							{
								syslog_ax_acl_dbg("untagport.port[%d] %d\n",i,untagPorts.ports[i]);
								EnableInfo=npd_drv_vlan_acl_enable(untagPorts.ports[i],acl_enable,vlanId,dir_info);
								if(NPD_DBUS_ERROR_NO_SUCH_PORT==EnableInfo){
									op_ret=NPD_DBUS_ERROR_NO_SUCH_PORT;
									syslog_ax_acl_dbg("no such untag port in vlan %d",untagPorts.ports[i]);
								}						
								else if(ACL_RETURN_CODE_ERROR==EnableInfo)
									syslog_ax_acl_dbg("enable fail!\n");
								else if(ACL_RETURN_CODE_SUCCESS==EnableInfo)
									syslog_ax_acl_dbg("enable untagport.port[%d] %d success\n",i,untagPorts.ports[i]);
							}
							for(j=0;j<tagPorts.count;j++)
							{
								syslog_ax_acl_dbg("tagport.port[%d] %d\n",j,tagPorts.ports[j]);
								EnableInfo=npd_drv_vlan_acl_enable(tagPorts.ports[j],acl_enable,vlanId,dir_info);
								if(NPD_DBUS_ERROR_NO_SUCH_PORT==EnableInfo){
									op_ret=NPD_DBUS_ERROR_NO_SUCH_PORT;
									syslog_ax_acl_dbg("no such tag port in vlan %d",tagPorts.ports[j]);
								}				
								else if(ACL_RETURN_CODE_ERROR==EnableInfo)
									syslog_ax_acl_dbg("enable fail!\n");
								else if(ACL_RETURN_CODE_ERROR==EnableInfo)
									syslog_ax_acl_dbg("enable tagport.port[%d] %d success\n",j,tagPorts.ports[j]);
					
							}
						}
					 }		
					}
				}
				else {
					op_ret = ACL_RETURN_CODE_BCM_DEVICE_NOT_SUPPORT;
				}
			}
		}
		else if(ACCESS_PORT_TYPE==flag){
			syslog_ax_acl_dbg("port_index %d\n",g_index);
			tmp = npd_port_bind_group_check(g_index, &groupNo, dir_info);
			if ((ACL_RETURN_CODE_GROUP_PORT_BINDED == tmp) && (0 == acl_enable)){
				npd_syslog_dbg("please unbind acl group firstly!\n");
				op_ret = ACL_RETURN_CODE_GROUP_PORT_BINDED;
			} 
			else {
				ret = nam_bcm_acl_support_check();
				if ((ACL_RETURN_CODE_BCM_DEVICE_NOT_SUPPORT == ret) && (ACL_DIRECTION_EGRESS == dir_info)){
					op_ret = ACL_RETURN_CODE_BCM_DEVICE_NOT_SUPPORT;
					syslog_ax_acl_dbg("ret is  ACL_RETURN_CODE_BCM_DEVICE_NOT_SUPPORT\n");
				}
				else {
					EnableInfo = npd_drv_port_acl_enable(g_index,acl_enable,dir_info);
					if(NPD_DBUS_ERROR_NO_SUCH_PORT==EnableInfo){
						op_ret=NPD_DBUS_ERROR_NO_SUCH_PORT;
						syslog_ax_acl_dbg("NPD_DBUS_ERROR_NO_SUCH_PORT");
					}
					else if(ACL_RETURN_CODE_ERROR==EnableInfo) {
						syslog_ax_acl_dbg("ACL_RETURN_CODE_ERROR ,enable fail\n");
					}
					else if(ACL_RETURN_CODE_SUCCESS==EnableInfo) {
						syslog_ax_acl_dbg("ACL_RETURN_CODE_SUCCESS!enable success\n");
					}
					else if (ACL_PORT_NOT_SUPPORT_BINDED == EnableInfo) {
						op_ret = ACL_PORT_NOT_SUPPORT_BINDED;
					}
				}
			}
		}						 	
	}
				 	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &EnableInfo);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &op_ret);
	
	return reply;
}

DBusMessage * npd_dbus_ethport_bind_acl_group(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage*	 reply;
	DBusMessageIter  iter;
	DBusError		 err;		  
	unsigned int	 group_num = 0,g_index = 0;
	unsigned char	 devNum = 0,virPortNo = 0;
	unsigned int	 op_ret=0,bind_ret=ACL_RETURN_CODE_ERROR;
	unsigned int 	 pvEnable=ACL_RETURN_CODE_SUCCESS;
	/*code optimize:  Uninitialized scalar variable
	zhangcl@autelan.com 2013-1-17*/
	unsigned int 	 i=0,j=0,flag = 0,in_flag = 0,ret = 0,remm = 0,renn = 0;
	unsigned short	 vlanId;
	struct acl_group_list_s	*groupPtr=NULL;
       struct vlan_ports       untagPorts, tagPorts;
	unsigned int 		groupNo=0,mm=0,nn,qq,dir_info=0,stdrule=0,bindId=0,devret =0;
	enum product_id_e productId = PRODUCT_ID; 
	int isBoard = NPD_FALSE;/* 1 -if main board port, 0 - slave board port*/ 
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT32,&dir_info,
		DBUS_TYPE_UINT32,&in_flag,
		DBUS_TYPE_UINT32,&g_index,
		DBUS_TYPE_UINT32,&group_num,		
		DBUS_TYPE_INVALID))) {
		syslog_ax_acl_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_acl_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	npd_syslog_dbg("bind acl group	%d to index %d\n",group_num,g_index);
	if((group_num > 1023)||(group_num < 1)) {
			op_ret=ACL_RETURN_CODE_GROUP_INDEX_ERROR;	
	}
	else{
		if(0==in_flag) { 
			flag=ACCESS_PORT_TYPE;
		}
		else if(1==in_flag) {
			flag=ACCESS_VID_TYPE;
		}
				
		if(flag==ACCESS_VID_TYPE){
			ret = nam_bcm_acl_support_check();
				if (ACL_RETURN_CODE_BCM_DEVICE_NOT_SUPPORT != ret) {
				vlanId = (unsigned short)g_index;
				
				npd_syslog_dbg("vlan mode ...\n");
				npd_syslog_dbg("vlan id %d\n",vlanId);
				
				if (!CHASSIS_VLANID_ISLEGAL(vlanId)) {
					op_ret = NPD_DBUS_ERROR_NO_SUCH_VLAN +1;/*vlan*/
					/*return op_ret;*/
				}
				else if(VLAN_RETURN_CODE_VLAN_NOT_EXISTS == npd_check_vlan_exist(vlanId)) {
					op_ret = NPD_VLAN_NOTEXISTS;
					npd_syslog_dbg("vlanId =%d NOT exists.\n",vlanId);
					/*return op_ret;*/
				}
				else {	
					npd_syslog_dbg("checking if vlan acl enabled\n");
					/*check if vlan enable*/
					ret=npd_vlan_acl_bind_check(vlanId,&groupNo,dir_info);
					npd_syslog_dbg("npd vlan acl bind check ret : %d \n",ret);
					if(NPD_VLAN_NOTEXISTS==ret)
						op_ret = NPD_VLAN_NOTEXISTS;/*vlan*/
					else if((ACL_RETURN_CODE_ERROR==ret)||(ACL_RETURN_CODE_ON_PORT_DISABLE == ret)){
						npd_syslog_dbg("please enable the port acl service!\n");
						pvEnable = ACL_RETURN_CODE_ERROR;/*enable*/
					}
					else if(ACL_RETURN_CODE_GROUP_VLAN_BINDED==ret){
						npd_syslog_dbg("group %d bind to vlan yet\n",groupNo);
						op_ret = ACL_RETURN_CODE_GROUP_VLAN_BINDED;/*bind*/
					}
					else if(ACL_RETURN_CODE_SUCCESS==ret)
					{
						pvEnable = ACL_RETURN_CODE_SUCCESS;
						
						npd_syslog_dbg("vlan acl enabled!\n");
						/*vlan bind group,sw retain group info*/
						nn=npd_vlan_bind_group(vlanId,group_num,dir_info);
						if(nn==ACL_RETURN_CODE_GROUP_NOT_EXISTED){
						    syslog_ax_acl_dbg("you should creat acl group %d firstly!\n",group_num);
							op_ret=ACL_RETURN_CODE_GROUP_NOT_EXISTED;/*no group*/
						}
						else if(nn==ACL_RETURN_CODE_SUCCESS){
							/*ports in vlan ,hw cfg table*/
							/*search all ports*/
							npd_syslog_dbg("npd_lookup_ports_by_vlanid\n");
							mm=npd_vlan_get_all_ports(vlanId,&untagPorts,&tagPorts);
							npd_syslog_dbg("mm %d if 15 vlanNotExisted\n",mm);
							npd_syslog_dbg("untagPorts.count %d\n",untagPorts.count);
							npd_syslog_dbg("tagPorts.count %d\n",tagPorts.count);
							if(NPD_VLAN_NOTEXISTS==mm){
								op_ret = NPD_VLAN_NOTEXISTS;
							}
							else if(NPD_VLAN_NOTEXISTS!=mm){
								if(dir_info == ACL_DIRECTION_INGRESS) {			
									groupPtr = acl_group[group_num];
									stdrule = acl_group[group_num]->stdrule;
								}
								else if(dir_info == ACL_DIRECTION_EGRESS) {
									groupPtr = egress_acl_group[group_num];
									stdrule = egress_acl_group[group_num]->stdrule;
								}						
								for(i=0;i<untagPorts.count;i++){
									devNum = 0;
									virPortNo =0;
									npd_syslog_dbg("untagPorts.ports[%d] %d\n",i,untagPorts.ports[i]);
									renn = npd_get_devport_by_global_index(untagPorts.ports[i],&devNum,&virPortNo);
									if(renn != 0){
										npd_syslog_dbg("npd_acl:vlan::get devport Error.\n");
									}
									else {
										npd_syslog_dbg("devNum %d,portNum %d\n",devNum,virPortNo);
										bind_ret = npd_drv_bind_acl_group(devNum,virPortNo,group_num,g_index,ACCESS_VID_TYPE,vlanId,dir_info,stdrule);
										if(ACL_RETURN_CODE_SUCCESS==bind_ret){
											 npd_syslog_dbg("vlan bind acl group successfully!\n");
										}				
									    else {
										   npd_syslog_dbg("vlan bind rule fail %d!\n",bind_ret);
									    }
								    }/*else*/
									
								} /*IF untag*/
								
								for(j=0;j<tagPorts.count;j++){

									devNum=0;
									virPortNo =0;
									npd_syslog_dbg("tagPorts.ports[%d] %d\n",j,tagPorts.ports[j]);
									renn = npd_get_devport_by_global_index(tagPorts.ports[j],&devNum,&virPortNo);
									if(renn != 0){
										npd_syslog_dbg("npd_acl:vlan::get devport Error.\n");
									}
									else {
										npd_syslog_dbg("devNum %d,portNum %d\n",devNum,virPortNo);
										bind_ret = npd_drv_bind_acl_group(devNum,virPortNo,group_num,g_index,ACCESS_VID_TYPE,vlanId,dir_info,stdrule);
										if(ACL_RETURN_CODE_SUCCESS==bind_ret){
											 npd_syslog_dbg("vlan bind acl group successfully!\n");
										}				
									    else {
										   npd_syslog_dbg("vlan bind rule fail %d!\n",bind_ret);
									    }
									}
								} /*if tag	*/
								/* check for vlan have a udp rule for dhcp*/
								ret = npd_acl_bind_check_vlan_udp(vlanId, stdrule);
								if (0 != ret) {
									npd_syslog_dbg("vlan %d have udp rules for dhcp \n", vlanId);
								}
					      }
						}				
				  } /*else*/
			   }
			  } /*vid*/
		}
		  else if(flag==ACCESS_PORT_TYPE){
		  	devret = nam_bcm_acl_support_check();
			op_ret = npd_qos_port_bind_check(g_index, &bindId);
			if ((QOS_RETURN_CODE_POLICY_MAP_BIND == op_ret) && (ACL_RETURN_CODE_BCM_DEVICE_NOT_SUPPORT == devret)) {
				op_ret = ACL_RETURN_CODE_BCM_DEVICE_NOT_SUPPORT;
			}
			/*group_info = ACL_RETURN_CODE_SUCCESS; */
			else {
				remm= npd_get_devport_by_global_index(g_index,&devNum,&virPortNo);
				if(remm != 0){
					if(((PRODUCT_ID_AX7K_I == productId)||(PRODUCT_ID_AX7K == productId))
						&& (g_index < 4)) {
						isBoard = NPD_TRUE;
						op_ret = ACL_PORT_NOT_SUPPORT_BINDED;
					}
					else if((PRODUCT_ID_AX5K_I == productId) && ((g_index >= 8) && (g_index <= 11))) {
						isBoard = NPD_TRUE;
						op_ret = ACL_PORT_NOT_SUPPORT_BINDED;
					}
					else {
						op_ret =NPD_DBUS_ERROR_NO_SUCH_PORT;
						npd_syslog_dbg("Npd_dbus::npd_get_devport_by_global_index::get devport Error.\n");
					}
				}
				else {	
					npd_syslog_dbg("npd_get_devport_by_global_index::devNum %d,portNum %d\n",devNum,virPortNo);

					ret = npd_eth_port_acl_bind_check(g_index,dir_info);
							
					if(NPD_TRUE!=ret){
						npd_syslog_dbg("please enable the port acl service!\n");
						pvEnable = ACL_RETURN_CODE_ERROR;
					}
					else
					{
						pvEnable = ACL_RETURN_CODE_SUCCESS;
										
						/*port bind group sw*/
						qq=npd_port_bind_group(g_index,group_num,dir_info);
						if(NPD_DBUS_ERROR_NO_SUCH_PORT==qq){
							op_ret=NPD_DBUS_ERROR_NO_SUCH_PORT;
							npd_syslog_dbg("port info error\n");
						}
						else if(ACL_RETURN_CODE_GROUP_NOT_EXISTED==qq){
							op_ret=ACL_RETURN_CODE_GROUP_NOT_EXISTED;
							npd_syslog_dbg("group not exist\n");
						}
						else if(ACL_RETURN_CODE_GROUP_PORT_BINDED==qq){
							op_ret=ACL_RETURN_CODE_GROUP_PORT_BINDED;
							npd_syslog_dbg("port bind group yet\n");
						}
						else if(ACL_RETURN_CODE_SUCCESS==qq){
							if(dir_info == ACL_DIRECTION_INGRESS) {			
								groupPtr = acl_group[group_num];
								stdrule = acl_group[group_num]->stdrule;
							}
							else if(dir_info == ACL_DIRECTION_EGRESS) {
								groupPtr = egress_acl_group[group_num];
								stdrule = egress_acl_group[group_num]->stdrule;
							}
							op_ret =ACL_RETURN_CODE_SUCCESS;
							npd_syslog_dbg("bind success\n");
							/*port cfg table hw*/
							bind_ret = npd_drv_bind_acl_group(devNum,virPortNo,group_num,g_index,ACCESS_PORT_TYPE,0,dir_info,stdrule);
							npd_syslog_dbg("binded by devNum %d\n", devNum);
							npd_syslog_dbg("binded by virportno %d\n", virPortNo);
							npd_syslog_dbg("binded by goupr_num %d\n", group_num);
							npd_syslog_dbg("binded by g_index %d \n", g_index);
							if(ACL_RETURN_CODE_SUCCESS==bind_ret){
								 npd_syslog_dbg("port bind acl group successfully!\n");
							}				
						    else {
							    npd_syslog_dbg("port bind rule fail %d!\n",bind_ret);
						    }				
						}
						else{
							npd_syslog_dbg("sw port bind group fail\n ");
							op_ret =ACL_RETURN_CODE_ERROR;
						}				
					}/*...*/
				}
			}
		}/*port*/
	}
	/*npd_syslog_dbg("op_ret %d\n",op_ret);*/
	npd_syslog_dbg("bind_ret %d\n",bind_ret);
	npd_syslog_dbg("pvEnable %d\n",pvEnable);
	 reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &op_ret);/*sw bind*/
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &bind_ret);/*port cfg*/
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &pvEnable);/*enable*/
								 
	return reply;	   

}

int npd_acl_add_udp_vlan_rule
(
	int 	enable
)
{
	unsigned char	 devNum = 0, virPortNo = 0;
	int ret = 0, i = 0, j = 0;
	struct ACCESS_LIST_STC *store = NULL;
	
	store=(struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
	/*code optimize: Dereference after null check
	zhangcl@autelan.com 2013-1-17*/
	if(NULL == store) 
	{
		ret = ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
		return ret;
	}
	memset(store,0,sizeof(struct ACCESS_LIST_STC));

	if(enable) {
		store->ruleIndex = 0;
		store->ruleType = STANDARD_ACL_RULE;
		store->ActionPtr.actionType = ACL_ACTION_TYPE_TRAP_TO_CPU; 
		store->ActionPtr.actionInfo.trap.cpucode = 500;
		store->TcamPtr.ruleFormat = ACL_RULE_STD_IPv4_L4;
		store->TcamPtr.packetType = ACL_TCAM_RULE_PACKETTYPE_UDP;
		store->TcamPtr.ruleInfo.stdIpv4L4.rule.DstPort = 67;
		store->TcamPtr.ruleInfo.stdIpv4L4.rule.SrcPort = 68;
		store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSrcPort = 65535;
		store->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDstPort = 65535;

		ret = nam_drv_acl_stdIpv4L4_no_group_num(store, 0, 0);
		if(ret != 0){
			npd_syslog_err("npd_acl:vlan::get devport Error.\n");
			free(store);
			return 1;
		}
		
		store->ruleIndex = 1;
		store->TcamPtr.ruleInfo.stdIpv4L4.rule.DstPort = 68;
		store->TcamPtr.ruleInfo.stdIpv4L4.rule.SrcPort = 67;
		ret = nam_drv_acl_stdIpv4L4_no_group_num(store, 0, 0);
		if(ret != 0){
			npd_syslog_err("npd_acl:vlan::get devport Error.\n");
			free(store);
			return 1;
		}

		memset(store,0,sizeof(struct ACCESS_LIST_STC));
		store->ruleIndex = 2;
		store->ruleType = EXTENDED_ACL_RULE;  
		store->ActionPtr.actionType = ACL_ACTION_TYPE_TRAP_TO_CPU; 
		store->ActionPtr.actionInfo.trap.cpucode = 500;
		store->TcamPtr.ruleFormat = ACL_RULE_EXT_NOT_IPv6;
		store->TcamPtr.packetType = ACL_TCAM_RULE_PACKETTYPE_UDP;
		store->TcamPtr.ruleInfo.extNotIpv6.rule.DstPort = 67;
		store->TcamPtr.ruleInfo.extNotIpv6.rule.SrcPort = 68;
		store->TcamPtr.ruleInfo.extNotIpv6.mask.maskSrcPort = 65535;
		store->TcamPtr.ruleInfo.extNotIpv6.mask.maskDstPort = 65535;
		ret = nam_drv_acl_extNotIpv6_no_group_num(store, 0, 0);
		if(ret != 0){
			npd_syslog_err("npd_acl:vlan::get devport Error.\n");
			free(store);
			return 1;
		}

		store->ruleIndex = 3;
		store->TcamPtr.ruleInfo.extNotIpv6.rule.DstPort = 68;
		store->TcamPtr.ruleInfo.extNotIpv6.rule.SrcPort = 67;
		ret = nam_drv_acl_extNotIpv6_no_group_num(store, 0, 0);
		if(ret != 0){
			npd_syslog_err("npd_acl:vlan::get devport Error.\n");
			free(store);
			return 1;
		}
		/*
		i = 2;
		do {
			if (0 == ruletype) {
				ret = nam_drv_acl_stdIpv4L4_no_group_num(store, 0, 0);
				if(ret != 0){
					npd_syslog_err("npd_acl:vlan::get devport Error.\n");
					free(store);
					return 1;
				}
			}
			else if (1 == ruletype) {
				store->ruleType = EXTENDED_ACL_RULE;  
				store->TcamPtr.ruleFormat=ACL_RULE_EXT_NOT_IPv6;
				ret = nam_drv_acl_extNotIpv6_no_group_num(store, 0, 0);
				if(ret != 0){
					npd_syslog_err("npd_acl:vlan::get devport Error.\n");
					free(store);
					return 1;
				}
			}
			else {
				npd_syslog_dbg("ruletype bad Parameter \n");
				return 1;
			}
			
			i--;
			
			if (i) {
				store->ruleIndex = 1;
				store->TcamPtr.ruleInfo.stdIpv4L4.rule.DstPort = 68;
				store->TcamPtr.ruleInfo.stdIpv4L4.rule.SrcPort = 67;
			}		
		}while(i);
		*/
	}
	else {
		ret = nam_acl_drv_tcam_clear_udp_vlan(0, 0);
		if(0 != ret) {
			npd_syslog_err("acl drv tcam clear Error.\n");
			free(store);
			return 1;
		}
		ret = nam_acl_drv_tcam_clear_udp_vlan(1, 0);
		if(0 != ret) {
			npd_syslog_err("acl drv tcam clear Error.\n");
			free(store);
			return 1;
		}
		ret = nam_acl_drv_tcam_clear_udp_vlan(2, 1);
		if(0 != ret) {
			npd_syslog_err("acl drv tcam clear Error.\n");
			free(store);
			return 1;
		}
		ret = nam_acl_drv_tcam_clear_udp_vlan(3, 1);
		if(0 != ret) {
			npd_syslog_err("acl drv tcam clear Error.\n");
			free(store);
			return 1;
		}
	}
	
	free(store);
	return 0;
}

int npd_acl_add_udp_vlan_enable
(
	unsigned short	 vlanId,
	int 				enable
)
{
	unsigned char	 devNum = 0, virPortNo = 0;
	unsigned int ret = 0, i = 0, j = 0, groupNo = 0, dir_info = 0, stdrule = 0, binded = 0;
	struct vlan_ports       untagPorts, tagPorts;
	struct acl_group_list_s *groupPtr = NULL;

	ret = npd_vlan_get_all_ports(vlanId, &untagPorts, &tagPorts);
	if(NPD_VLAN_NOTEXISTS == ret) {
		npd_syslog_dbg("vlan get all ports  Error.\n");
		return 1;
	}
	/*code optimize: Dereference after null check
	zhangcl@autelan.com 2013-1-17*/
	#if 0
	if (NULL != udp_rule) 
	{
		if (NULL != udp_rule[vlanId]) 
		{
			npd_syslog_dbg("vlan %d hava a udp rule for dhcpsnooping \n");
		}
		else 
		{
			udp_rule[vlanId] = (struct group_udp_vlan_rule *)malloc(sizeof(struct group_udp_vlan_rule));
		}
	}
	#endif
	if (NULL != udp_rule) 
	{
		if (NULL != udp_rule[vlanId]) 
		{
			npd_syslog_dbg("vlan %d hava a udp rule for dhcpsnooping \n");
		}
		else 
		{
			udp_rule[vlanId] = (struct group_udp_vlan_rule *)malloc(sizeof(struct group_udp_vlan_rule));
		}
	}
	else
	{
		return 1;
	}
	/* find ingress acl group on vlan */
	ret = npd_vlan_acl_bind_check(vlanId, &groupNo, 0);
	if(ACL_RETURN_CODE_GROUP_VLAN_BINDED == ret) {
		if (NULL != acl_group[groupNo]) {
			groupPtr = acl_group[groupNo];
			stdrule = acl_group[groupNo]->stdrule;
		}
		else {
			npd_syslog_err("group %d not exist \n", groupNo);
			free(udp_rule[vlanId]);
			return 1;	
		}
		binded = 1;
	}
	else {
		groupNo = ACL_SAVE_FOR_UDP_GROUP;
		stdrule = STANDARD_ACL_RULE;
	}
	for(i = 0; i < untagPorts.count; i++) {
		devNum = 0;
		virPortNo =0;
		npd_syslog_dbg("untagPorts.ports[%d] %d\n", i, untagPorts.ports[i]);
		ret = npd_get_devport_by_global_index(untagPorts.ports[i], &devNum, &virPortNo);
		if(0 != ret){
			npd_syslog_dbg("npd_acl:vlan::get devport Error.\n");
			free(udp_rule[vlanId]);
			return 1;
		}
		else {
			npd_syslog_dbg("devNum %d,portNum %d\n", devNum, virPortNo);
			if (enable) {
				ret = nam_acl_drv_cfg_table_set(devNum, virPortNo, groupNo, enable, ACCESS_VID_TYPE, vlanId, 0, stdrule);
				if(0 != ret){
				 	npd_syslog_err("vlan bind acl group fail!\n");
					free(udp_rule[vlanId]);
					return 1;
				}
			}
			else {
				ret = nam_acl_drv_cfg_table_set(devNum, virPortNo, 0, ACL_FALSE,ACCESS_VID_TYPE, vlanId, 0, stdrule);
				if(0 != ret){
				 	npd_syslog_err("vlan bind acl group fail!\n");
					free(udp_rule[vlanId]);
					return 1;
				}
			}
		}
		ret = nam_acl_port_acl_enable(devNum, virPortNo, enable|binded, 0, 0);
		if(0 != ret){
			npd_syslog_dbg("npd_acl:vlan::get devport Error.\n");
			free(udp_rule[vlanId]);
			return 1;
		}
	}/*if untag*/
	
	for(j = 0; j < tagPorts.count; j++) {
		devNum = 0;
		virPortNo = 0;
		npd_syslog_dbg("tagPorts.ports[%d] %d\n", j, tagPorts.ports[j]);
		ret = npd_get_devport_by_global_index(tagPorts.ports[j], &devNum, &virPortNo);
		if(0 != ret){
			npd_syslog_dbg("npd_acl:vlan::get devport Error.\n");
		}
		else {
			npd_syslog_dbg("devNum %d,portNum %d\n",devNum,virPortNo);
			if (enable) { 
				ret = nam_acl_drv_cfg_table_set(devNum, virPortNo, groupNo, enable, ACCESS_VID_TYPE, vlanId, 0, stdrule);
				if(0 != ret){
				 	npd_syslog_err("vlan bind acl group fail!\n");
					free(udp_rule[vlanId]);
					return 1;
				}
			}
			else {
				ret = nam_acl_drv_cfg_table_set(devNum, virPortNo, 0, ACL_FALSE,ACCESS_VID_TYPE, vlanId, 0, stdrule);
				if(0 != ret){
				 	npd_syslog_err("vlan bind acl group fail!\n");
					free(udp_rule[vlanId]);
					return 1;
				}
			}
		}
		ret = nam_acl_port_acl_enable(devNum, virPortNo, enable|binded, 0, 0);
		if(0 != ret) {
			npd_syslog_dbg("npd_acl:vlan::get devport Error.\n");
			free(udp_rule[vlanId]);
			return 1;
		}
	} /*if tag	*/
	
	if (enable) {
		udp_rule[vlanId]->vlanId = vlanId;
		udp_rule[vlanId]->groupNo = groupNo;
		udp_rule[vlanId]->ruleType = stdrule;
	}
	else {
		free(udp_rule[vlanId]);
		udp_rule[vlanId] = NULL;
	}
	
	return 0;
}

int npd_acl_bind_check_vlan_udp
(
	unsigned int		vlanId,
	unsigned int 		ruleType
) 
{
	int ret = 0;
	
	if ((NULL != udp_rule) && (NULL != udp_rule[vlanId])) {
		if (udp_rule[vlanId]->ruleType == ruleType) {
			npd_syslog_dbg("we have the same ruleType good !!!\n");
			return 1;
		}
		else {
			ret = npd_acl_add_udp_vlan_enable(vlanId, 1);
			if (0 != ret) {
				npd_syslog_dbg("acl add udp vlan enable fail \n");
				return 1;
			}
		}
	}
	else {
		npd_syslog_dbg("vlan %d has no udp rule for dhcp \n", vlanId);
	}

	return 0;
}

int npd_acl_unbind_check_vlan_udp
(
	unsigned int		vlanId
) 
{
	int ret = 0;
	if ((NULL != udp_rule) && (NULL != udp_rule[vlanId])) 
	{
		ret = npd_acl_add_udp_vlan_enable(vlanId, 1);
		if (0 != ret) 
		{
			npd_syslog_dbg("acl add udp vlan enable fail \n");
			return 1;
		}
	}
	/*code optimize:  Dereference after null check
	zhangcl@autelan.com 2013-1-17*/
	return ret;
}

int npd_acl_check_udp_vlan_disable
(
	unsigned int		vlanId
) 
{
	int ret = 0;
	if ((NULL != udp_rule) && (NULL != udp_rule[vlanId])) {
		return 1;
	}
	else {
		return 0;
	}
}

DBusMessage * npd_dbus_ethport_delete_acl_group(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage*	 		reply;
	DBusMessageIter  		iter;
	DBusError		 		err;		
	/*code optimize:  Uninitialized scalar variable
	zhangcl@autelan.com 2013-1-17*/
	unsigned int	 		group_num = 0,g_index = 0,access_type = 0,flag = 0;
	unsigned char	 		devNum = 0,virPortNo = 0;
	int 					port_ret = 0,cfg_ret = 0;
	struct eth_port_s     	*portInfo = NULL;
	struct eth_group_info   *groupNode = NULL;
	struct acl_group_list_s	*groupPtr=NULL;
	unsigned int			mm = 0,i = 0,j = 0,op_ret,ret;
	unsigned short			vlanId=0;
	struct vlan_ports       untagPorts, tagPorts;
	struct vlan_s			*node=NULL;
	unsigned int 			groupNo=0,rem = 0,rm = 0,stdrule = 0;
	unsigned int			sw_info,cfg_info = ACL_RETURN_CODE_SUCCESS,dir_info=0;
	unsigned int 			rr1=ACL_RETURN_CODE_SUCCESS,rr2=ACL_RETURN_CODE_SUCCESS;
	struct list_head		 *ptr=NULL;
	struct group_eth_info	*gethinfo=NULL;

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
						    DBUS_TYPE_UINT32,&dir_info, 
							DBUS_TYPE_UINT32,&flag, 
							DBUS_TYPE_UINT32,&g_index,
							DBUS_TYPE_UINT32,&group_num,	
							DBUS_TYPE_INVALID))) 
	{
		syslog_ax_acl_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) 
		{
			syslog_ax_acl_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	syslog_ax_acl_dbg("delete acl group %d on index %d\n",group_num,g_index);
	
	if((group_num>1023)||(group_num<1)) 
	{
		sw_info=ACL_RETURN_CODE_GROUP_INDEX_ERROR;
	}
	else
	{
		if(0==flag) 
		{
			access_type=ACCESS_PORT_TYPE;
		}
		else if(1==flag) 
		{
			access_type=ACCESS_VID_TYPE;
		}

		if(ACCESS_PORT_TYPE==access_type)
		{

			sw_info =	 NPD_DBUS_ERROR_NO_SUCH_PORT;
			/*code optimize:  Dereference after null check
			zhangcl@autelan.com 2013-1-17*/
			portInfo = npd_get_port_by_index(g_index);
			if(NULL == portInfo) 
			{
				syslog_ax_acl_dbg("find port by index %#x error\n",g_index);
				sw_info = NPD_DBUS_ERROR_NO_SUCH_PORT;
			}
			else
			{
					
				ret = npd_eth_port_acl_bind_check(g_index,dir_info);
				if(NPD_TRUE!=ret)
				{
					syslog_ax_acl_dbg("please enable the port acl service!\n");
					sw_info = ACL_RETURN_CODE_ENABLE_FIRST;
				}
				else 
				{	
					port_ret = npd_get_devport_by_global_index(g_index,&devNum,&virPortNo);
					if(port_ret != 0)
					{
						syslog_ax_acl_dbg("Npd_dbus::npd_get_devport_by_global_index::get devport Error.\n");
						sw_info = NPD_DBUS_ERROR_NO_SUCH_PORT;
					}
					else
					{
						/*check if group on port*/
						rem = npd_port_bind_group_check(g_index,&groupNo,dir_info);		
						if(ACL_RETURN_CODE_ERROR==rem)
						{
							syslog_ax_acl_dbg("eth_g_index error!\n");
							sw_info = NPD_DBUS_ERROR_NO_SUCH_PORT;
						}
						else if(ACL_RETURN_CODE_GROUP_PORT_NOTFOUND==rem)
						{
							syslog_ax_acl_dbg("no group info on port\n");
							sw_info = ACL_RETURN_CODE_GROUP_PORT_NOTFOUND;
						}
						else if(ACL_RETURN_CODE_GROUP_PORT_BINDED==rem)
						{
						   	syslog_ax_acl_dbg("this interface has binded acl group %d\n",groupNo);			 
							if(groupNo == group_num) 
						   	{
						   		groupNode = portInfo->funcs.func_data[ETH_PORT_FUNC_ACL_RULE];
							   	if(NULL!=groupNode)
								{
									 /*free cfg table*/
									if(dir_info==ACL_DIRECTION_INGRESS) 
									{			
										groupPtr = acl_group[group_num];
										stdrule = acl_group[group_num]->stdrule;
									}
									else if(dir_info==ACL_DIRECTION_EGRESS) 
									{
										groupPtr = egress_acl_group[group_num];
										stdrule = egress_acl_group[group_num]->stdrule;
									}
									
									cfg_info = npd_drv_unbind_acl_group(devNum, virPortNo, group_num ,g_index, access_type, vlanId, dir_info, stdrule);	 	
									if(cfg_info != ACL_RETURN_CODE_SUCCESS) 
									{
										npd_syslog_dbg("interface delete acl group error %d!\n",cfg_ret);	
										cfg_info =ACL_RETURN_CODE_ERROR;
									}
									else if (cfg_info==ACL_RETURN_CODE_SUCCESS)
									{
										__list_for_each(ptr,&(groupPtr->list2)) 
										{
											gethinfo=list_entry(ptr,struct group_eth_info,list);	
											if(NULL != gethinfo)
											{
												if(gethinfo->eth_index == g_index ) 
												{
													__list_del((gethinfo->list).prev, (gethinfo->list).next);
													break;
												}
												syslog_ax_acl_dbg("group been unbind by %d \n",g_index);
											}				  
											else
												syslog_ax_acl_dbg(" port have not group %d\n",groupNo);
										}
										groupPtr->eth_count-=1;
										syslog_ax_acl_dbg("groupPtr->eth_count %d\n",groupPtr->eth_count);
										if(dir_info==ACL_DIRECTION_INGRESS)
										{
												groupNode->groupId = 0;
												syslog_ax_acl_dbg("successfully delete ingress group on port!\n");						
										}
										else if(dir_info==ACL_DIRECTION_EGRESS)
										{
												groupNode->EgGroupId= 0;
												syslog_ax_acl_dbg("successfully delete egress group on port!\n");
										}

									}
									/*note:ignore direction ,just unbind group 	*/										
									
									sw_info = ACL_RETURN_CODE_SUCCESS;
								 }
								 
							}
							else
							{
								syslog_ax_acl_dbg("......group by checking ,binded ,but null!!strange\n");
		                        /* add for bug when delete acl,zhangdi 2011-10-19 */
								sw_info = ACL_RETURN_CODE_GROUP_NOT_EXISTED;
							}
						}  /*grouNo=groupNum*/
						else if(groupNo!=group_num)
						{
							sw_info = ACL_RETURN_CODE_GROUP_NOT_EXISTED;
							syslog_ax_acl_dbg("wrong group num\n");
						}
					}/*bind*/
				}	/*port right  */ 
			}
		}/*check port*/
		else if(ACCESS_VID_TYPE==access_type)
		{
			vlanId = (unsigned short)g_index;
			if (!CHASSIS_VLANID_ISLEGAL(vlanId)) 
			{
				sw_info= NPD_DBUS_ERROR_NO_SUCH_VLAN +1;
				/*return op_ret;*/
			}
			else if(VLAN_RETURN_CODE_VLAN_NOT_EXISTS == npd_check_vlan_exist(vlanId))
			{
				sw_info = NPD_VLAN_NOTEXISTS;
				syslog_ax_acl_dbg("vlanId =%d NOT exists.\n",vlanId);
				/*return op_ret;*/
			}
			else 
			{	
				syslog_ax_acl_dbg("checking if vlan acl enabled\n");
				ret=npd_vlan_acl_bind_check(g_index,&groupNo,dir_info);
				if(NPD_VLAN_NOTEXISTS==ret)
				{
					sw_info = NPD_VLAN_NOTEXISTS;
				}
				else if(ACL_RETURN_CODE_ON_PORT_DISABLE == ret)
				{
					syslog_ax_acl_dbg("please enable the vlan acl service!\n");
					sw_info = ACL_RETURN_CODE_ENABLE_FIRST;/*enable*/
				}
				else if((ACL_RETURN_CODE_ERROR==ret)||(ACL_RETURN_CODE_SUCCESS==ret))
				{
					syslog_ax_acl_dbg("no any bind information!\n");
					sw_info = ACL_RETURN_CODE_GROUP_NOT_BINDED;  /*no group on vlan*/
				}
				else if(ACL_RETURN_CODE_GROUP_VLAN_BINDED==ret)
				{
				  	syslog_ax_acl_dbg("vlan bind group\n");
					node = npd_find_vlan_by_vid(vlanId);
					if(NULL == node)
					{
						syslog_ax_acl_err("npd_lookup_ports_by_vlanid:: don't find vlan %d  node \n",vlanId);
						sw_info=NPD_VLAN_NOTEXISTS;
					}
					else
					{
						if(group_num==groupNo) 
						{
							/*search ports*/
							mm=npd_vlan_get_all_ports(vlanId,&untagPorts,&tagPorts);
							syslog_ax_acl_dbg("untagPorts->count %d\n",untagPorts.count);
							syslog_ax_acl_dbg("tagPorts->count %d\n",tagPorts.count);
							if(NPD_VLAN_NOTEXISTS!=mm ) 
							{		
								for(i=0;i<untagPorts.count;i++) 
								{
									devNum = 0;
									virPortNo =0;
									op_ret = npd_get_devport_by_global_index(untagPorts.ports[i],&devNum,&virPortNo);
									if(op_ret != 0)
									{
										syslog_ax_acl_err("npd_acl:vlan::get devport Error.\n");
									}
									else 
									{
										if(dir_info==ACL_DIRECTION_INGRESS) 
										{			
											groupPtr = acl_group[group_num];
											stdrule = acl_group[group_num]->stdrule;
										}
										else if(dir_info==ACL_DIRECTION_EGRESS) 
										{
											groupPtr = egress_acl_group[group_num];
											stdrule = egress_acl_group[group_num]->stdrule;
										}
										syslog_ax_acl_dbg(" untag devNum %d,portNum %d\n",devNum,virPortNo);
								      	rr1=nam_acl_drv_cfg_table_set(devNum,virPortNo,0,ACL_FALSE,ACCESS_VID_TYPE,vlanId,dir_info,stdrule);	
										if(rr1 != ACL_RETURN_CODE_SUCCESS) 
										{
											syslog_ax_acl_err("vlan delete acl group ,cfg table ,untagport,error %d!\n",cfg_ret);	
											rr1 =ACL_RETURN_CODE_ERROR;
									    }
									}
								}/*untag*/
								for(j=0;j<tagPorts.count;j++) 
								{
									devNum = 0;
									virPortNo =0;
									op_ret = npd_get_devport_by_global_index(tagPorts.ports[j],&devNum,&virPortNo);
									if(op_ret != 0)
									{
										syslog_ax_acl_err("npd_acl:vlan::get devport Error.\n");
									}
									else 
									{
										syslog_ax_acl_err("tag devNum %d,portNum %d\n",devNum,virPortNo);
								       	rr2=nam_acl_drv_cfg_table_set(devNum,virPortNo,0,ACL_FALSE,ACCESS_VID_TYPE,vlanId,dir_info,stdrule);	
										if(rr2 != ACL_RETURN_CODE_SUCCESS) 
										{
									  		syslog_ax_acl_err("vlan delete acl group error %d!\n",cfg_ret);	
									  		rr2 =ACL_RETURN_CODE_ERROR;
								     	}
									}								 	   
								}/*tag	*/

								cfg_info = rr1|rr2;
								if(cfg_info!=ACL_RETURN_CODE_SUCCESS)
								{
									syslog_ax_acl_err("hw delete group fail! should not delete sw info\n");
								}
								else
								{
								 /*sw delete groupId in vlan*/
									rm=	npd_vlan_unbind_group(vlanId,group_num,dir_info);
									if(ACL_RETURN_CODE_SUCCESS==rm)
									{
										sw_info =ACL_RETURN_CODE_SUCCESS;\
										syslog_ax_acl_dbg("sw delete group id in vlan success\n");					  
									}	
									else if(ACL_RETURN_CODE_GROUP_WRONG_INDEX==rm)
									{
										sw_info =ACL_RETURN_CODE_GROUP_WRONG_INDEX;
										syslog_ax_acl_err("sw delete wrong groupId in vlan !\n");
									}
								}
								/* add for check vlan udp rule for dhcp */
								ret = npd_acl_unbind_check_vlan_udp(vlanId);
								if (0 != ret) 
								{
									npd_syslog_dbg("vlan %d have udp rules for dhcp FAIL \n", vlanId);
								}
							} 
						}
			    		else if(groupNo!=group_num) 
						{
							sw_info = ACL_RETURN_CODE_GROUP_WRONG_INDEX;
							syslog_ax_acl_err("wrong group num\n");
						} 	
					}/*vlan ok*/
				}/*can delete	*/	  
			}/*vlan legal*/
		} /*vid*/
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &sw_info);	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &cfg_info);
					
	return reply;	   

}

DBusMessage * npd_dbus_ethport_show_accesslist(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage*			reply = NULL;
	DBusMessageIter 		iter_array,iter;
	DBusError				err;	
	unsigned int			dir_info=0,g_index = 0,ret=ACL_RETURN_CODE_SUCCESS,group_num=0;
	unsigned int    		slot_no=0,local_port_no=0;
	unsigned int 			groupNo=0,i = 0,j=0,k=0,count=0,node_flag = 0,in_flag = 0;
	static GROUP_RULE_INDEX_STC	*RuleId[MAX_ACL_RULE_NUMBER];
	struct acl_group_list_s *groupNodePtr=NULL;
	struct list_head		 *ptr=NULL;
	struct acl_rule_info_s   *ruleInfo=NULL;
	struct   eth_port_s     *portPtr = NULL;
	struct eth_group_info    *ethInfo=NULL;
	unsigned short			vlanId=0;	
	struct vlan_s			*node=NULL;
	unsigned int 			tm = 0,xm = 0,rem = 0,portemp=ACL_RETURN_CODE_SUCCESS;
	unsigned char			devNum=0,portNum=0;

	syslog_ax_acl_dbg("show access-list !\n");

	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
			DBUS_TYPE_UINT32,&dir_info,
			DBUS_TYPE_UINT32,&in_flag,
			DBUS_TYPE_UINT32,&g_index,
			DBUS_TYPE_INVALID))) 
	{
		syslog_ax_acl_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) 
		{
			syslog_ax_acl_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	if(0==in_flag) 
	{
		node_flag=ACCESS_PORT_TYPE;
	}
	else if(1==in_flag) 
	{
		node_flag=ACCESS_VID_TYPE;
	}
	
	if(node_flag==ACCESS_PORT_TYPE)
	{
		portPtr = npd_get_port_by_index(g_index);
		if(NULL==portPtr)
		{
			syslog_ax_acl_dbg("wrong port infomation when search by eth_g_index %d!\n",g_index);
			ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
		}
		else 
		{
			#if 0
			ts=npd_show_eth_rule(g_index);
			
			if(ts!=ACL_RETURN_CODE_SUCCESS)
				syslog_ax_acl_dbg("port no bind rule info \n");
			#endif
			/*code optimize: Unchecked return value
			zhangcl@autelan.com 2013-1-17*/
			ret = npd_get_devport_by_global_index(g_index,&devNum,&portNum);
			if(ret == 0)
			{
				ret = nam_get_slotport_by_devport(devNum,portNum,PRODUCT_ID,&(slot_no),&(local_port_no));
				if(ret == 0)
				{
					npd_syslog_dbg("slot/port %d/%d",slot_no,local_port_no);
					if(NULL==(g_eth_ports[g_index]->funcs.func_data[ETH_PORT_FUNC_ACL_RULE]))
					{
						syslog_ax_acl_dbg("interface no group infomation!\n");
						portemp=ACL_RETURN_CODE_ON_PORT_DISABLE;
						ret = ACL_RETURN_CODE_GROUP_PORT_NOTFOUND;
					}
				   	else
					{
				   	    tm=npd_eth_port_acl_bind_check(g_index,dir_info);
						if(NPD_TRUE!=tm)
						{
							syslog_ax_acl_dbg("please enable the port acl service!\n");
							portemp = ACL_RETURN_CODE_ON_PORT_DISABLE;
						}
						 				 	
						rem=npd_port_bind_group_check(g_index,&groupNo,dir_info);
						if(ACL_RETURN_CODE_ERROR==rem)
						{
							syslog_ax_acl_err("eth_g_index error!\n");
							ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
						}
						else if(ACL_RETURN_CODE_GROUP_PORT_NOTFOUND==rem)
						{
							syslog_ax_acl_dbg("no group info on port\n");
							ret = ACL_RETURN_CODE_GROUP_PORT_NOTFOUND;
						}
						else if(ACL_RETURN_CODE_GROUP_PORT_BINDED==rem)
						{
							syslog_ax_acl_dbg("this interface has binded acl group %d\n",groupNo);	
							portPtr = npd_get_port_by_index(g_index);
							if(NULL==portPtr)
							{
								syslog_ax_acl_dbg("wrong port infomation when search by g_index %d!\n",g_index);
								ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
							}		
							else
							{
								ethInfo=portPtr->funcs.func_data[ETH_PORT_FUNC_ACL_RULE];
							 	if(dir_info==ACL_DIRECTION_INGRESS)
								{
									group_num = ethInfo->groupId;
									syslog_ax_acl_dbg("bind group num on port %d\n",group_num);
									groupNodePtr=acl_group[group_num];
							 	}
							 	else if(dir_info==ACL_DIRECTION_EGRESS)
								{
									group_num = ethInfo->EgGroupId;
									syslog_ax_acl_dbg("bind group num on port %d\n",group_num);
									groupNodePtr=egress_acl_group[group_num];
							 	}
				
							 	if(groupNodePtr!=NULL)
								{
							   		count=groupNodePtr->count;
							   		syslog_ax_acl_dbg("count is %d\n",count);			  
							   		__list_for_each(ptr,&(groupNodePtr->list1)) 
									{
										ruleInfo=list_entry(ptr,struct acl_rule_info_s,list);					
										if(ruleInfo!=NULL)
										{
											RuleId[j]=(GROUP_RULE_INDEX_STC*)malloc(sizeof(GROUP_RULE_INDEX_STC));			
											RuleId[j]->rule_index= 1+(ruleInfo->ruleIndex);
											syslog_ax_acl_dbg("group->ruleIndex %ld\n",1+(ruleInfo->ruleIndex));
											syslog_ax_acl_dbg("j %d\n",j);
											j++;
											ret =ACL_RETURN_CODE_SUCCESS;
										}
										else
										{
											ret=ACL_RETURN_CODE_GROUP_RULE_NOTEXISTED;
										}
									} 		  
								}/*if*/
							}
						}
					}
				}
				else
				{
					syslog_ax_acl_err("get slot port by devport failed \n");
				}
			}
			else
			{
				syslog_ax_acl_err("get dev port by global index failed \n");
			}
			reply = dbus_message_new_method_return(msg);
			
			dbus_message_iter_init_append (reply, &iter);
			
			dbus_message_iter_append_basic (&iter,
											 DBUS_TYPE_UINT32,
											 &ret);
			dbus_message_iter_append_basic (&iter,
											 DBUS_TYPE_UINT32,
											 &portemp);
			dbus_message_iter_append_basic (&iter,
											 DBUS_TYPE_UINT32,
											 &slot_no);
			dbus_message_iter_append_basic (&iter,
											 DBUS_TYPE_UINT32,
											 &local_port_no);
			dbus_message_iter_append_basic (&iter,
											 DBUS_TYPE_UINT32,
											 &group_num);
			dbus_message_iter_append_basic (&iter,
											 DBUS_TYPE_UINT32,
											 &count);
			
			dbus_message_iter_open_container (&iter,
										   DBUS_TYPE_ARRAY,
											DBUS_STRUCT_BEGIN_CHAR_AS_STRING	 /*begin*/
											DBUS_TYPE_UINT32_AS_STRING	  /*index		*/			  
											DBUS_STRUCT_END_CHAR_AS_STRING, 	/*end*/
											&iter_array);
			syslog_ax_acl_dbg("open outer container");
			for (i = 0; i < count; i++ ) {
				DBusMessageIter iter_struct;
				
				dbus_message_iter_open_container (&iter_array,
												   DBUS_TYPE_STRUCT,
												   NULL,
												   &iter_struct);	
				dbus_message_iter_append_basic 
							(&iter_struct,
							 DBUS_TYPE_UINT32,
							 &(RuleId[i]->rule_index));
				syslog_ax_acl_dbg("rule[%d] %d\n",i,RuleId[i]->rule_index);
				
				syslog_ax_acl_dbg("very good!\n");
				dbus_message_iter_close_container (&iter_array, &iter_struct);
				free(RuleId[i]);
				RuleId[i]=NULL;
			}
			dbus_message_iter_close_container (&iter, &iter_array);
		}
	
	} /*port*/

	else if(node_flag==ACCESS_VID_TYPE){
		
		vlanId=(unsigned short)g_index;
		
		if (!CHASSIS_VLANID_ISLEGAL(vlanId)) {
			ret = NPD_DBUS_ERROR_NO_SUCH_VLAN +1;
			/*return op_ret;*/
		}
		else if(VLAN_RETURN_CODE_VLAN_NOT_EXISTS == npd_check_vlan_exist(vlanId)) {
			ret = NPD_VLAN_NOTEXISTS;
			syslog_ax_acl_dbg("vlanId =%d NOT exists.\n",vlanId);
			/*return op_ret;*/
		}
		else{
			node = npd_find_vlan_by_vid(vlanId);
			if(NULL == node) {
				syslog_ax_acl_dbg("npd_lookup_ports_by_vlanid:: don't find vlan %d  node \n",vlanId);
				ret=NPD_VLAN_NOTEXISTS;
			}
			else {
			      xm=npd_vlan_acl_enable_check(vlanId,dir_info);
				if(xm==ACL_RETURN_CODE_ERROR) {
					syslog_ax_acl_dbg("please enable the port acl service!\n");
					portemp = ACL_RETURN_CODE_ON_PORT_DISABLE;
				}
			      if(dir_info==ACL_DIRECTION_INGRESS) {
					group_num = 0;
					if(NULL==node->aclList) {
						ret = ACL_RETURN_CODE_ERROR;
						syslog_ax_acl_dbg("####vlan has no acl rule\n ");
					}
					else {
					   group_num=node->aclList->groupId;
						 if(acl_group[group_num]!=NULL) {
							groupNodePtr=acl_group[group_num];
							count=groupNodePtr->count;
							syslog_ax_acl_dbg("count is %d\n",count);
								__list_for_each(ptr,&(groupNodePtr->list1)) {
								  ruleInfo=list_entry(ptr,struct acl_rule_info_s,list); 				  
								  if(ruleInfo!=NULL) {
									  RuleId[j]=(GROUP_RULE_INDEX_STC*)malloc(sizeof(GROUP_RULE_INDEX_STC));		  
									  RuleId[j]->rule_index= 1+(ruleInfo->ruleIndex);
									  syslog_ax_acl_dbg("group->ruleIndex %ld\n",1+(ruleInfo->ruleIndex));
									  syslog_ax_acl_dbg("j %d\n",j);
									  j++;
									  ret =ACL_RETURN_CODE_SUCCESS;
								 }					  
								 else
									ret=ACL_RETURN_CODE_GROUP_RULE_NOTEXISTED;
							   }			
						} /*if*/
					}
				}
			  else if(dir_info==ACL_DIRECTION_EGRESS) {
			  	   group_num = 0;
				   if(NULL==node->aclList) {
					ret = ACL_RETURN_CODE_ERROR;
					syslog_ax_acl_dbg("####vlan has no acl rule\n ");
				   }
				   else {
					 group_num=node->aclList->EgrGroupId;
					 if(egress_acl_group[group_num]!=NULL) {
						groupNodePtr=egress_acl_group[group_num];
						count=groupNodePtr->count;
						syslog_ax_acl_dbg("count is %d\n",count);
							__list_for_each(ptr,&(groupNodePtr->list1)) {
							  ruleInfo=list_entry(ptr,struct acl_rule_info_s,list); 				  
							 if(ruleInfo!=NULL) {
								  RuleId[j]=(GROUP_RULE_INDEX_STC*)malloc(sizeof(GROUP_RULE_INDEX_STC));		  
								  RuleId[j]->rule_index= 1+(ruleInfo->ruleIndex);
								  syslog_ax_acl_dbg("group->ruleIndex %ld\n",1+(ruleInfo->ruleIndex));
								  syslog_ax_acl_dbg("j %d\n",j);
								  j++;
								  ret =ACL_RETURN_CODE_SUCCESS;
							 }					  
							 else {
								ret=ACL_RETURN_CODE_GROUP_RULE_NOTEXISTED;
							 }
						 }			
					} /*if*/
				  }				  
			  }
		   } /*else	*/			
		} /*else*/
		reply = dbus_message_new_method_return(msg);
		
		dbus_message_iter_init_append (reply, &iter);
		
		dbus_message_iter_append_basic (&iter,
						 DBUS_TYPE_UINT32,
						 &ret);
		dbus_message_iter_append_basic (&iter,
						 DBUS_TYPE_UINT32,
						 &portemp);
		dbus_message_iter_append_basic (&iter,
						 DBUS_TYPE_UINT32,
						 &group_num);
		dbus_message_iter_append_basic (&iter,
						 DBUS_TYPE_UINT32,
						&count);
				
		dbus_message_iter_open_container (&iter,
									   DBUS_TYPE_ARRAY,
									    DBUS_STRUCT_BEGIN_CHAR_AS_STRING     /*begin*/
											DBUS_TYPE_UINT32_AS_STRING    /*index    */                 
										DBUS_STRUCT_END_CHAR_AS_STRING,     /*end*/
									    &iter_array);
		syslog_ax_acl_dbg("open outer container");
		for (k = 0; k < count; k++ ) {
			DBusMessageIter iter_struct;
					
			dbus_message_iter_open_container (&iter_array,
											   DBUS_TYPE_STRUCT,
											   NULL,
											   &iter_struct);		
			dbus_message_iter_append_basic 
							(&iter_struct,
							DBUS_TYPE_UINT32,
								&(RuleId[k]->rule_index));
			syslog_ax_acl_dbg("rule[%d] %d\n",k,RuleId[k]->rule_index);
					
			syslog_ax_acl_dbg("very good!\n");
			dbus_message_iter_close_container (&iter_array, &iter_struct);
			free(RuleId[k]);
			RuleId[k]=NULL;
		}
		
			dbus_message_iter_close_container (&iter, &iter_array);
    } /*vid*/
	
	return reply;
}

#if 0
int npd_acl_permit_bpdu(unsigned long ruleIndex,unsigned int policer,unsigned int policerId)
{
    unsigned long   actionType,ret=ACL_RETURN_CODE_ERROR;
	ETHERADDR	    smacAddr,dmacAddr;
	unsigned int    ruleType,i,rs=ACL_RETURN_CODE_ERROR;
	struct ACCESS_LIST_STC *store = NULL;
	unsigned int 	count1=0,count2=0,j=0;


	store=(struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
	if(store==NULL)  
	{
		rs=ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
	}
	else
	{
		memset(store,0,sizeof(struct ACCESS_LIST_STC));
		memset(&smacAddr,0,sizeof(ETHERADDR));
		memset(&dmacAddr,0,sizeof(ETHERADDR));

		
		store->ruleIndex=ruleIndex;
        store->ActionPtr.actionType=ACL_ACTION_TYPE_PERMIT;	
		store->ActionPtr.policerPtr.policerEnable = policer;
		store->ActionPtr.policerPtr.policerId = policerId;

		 store->TcamPtr.ruleFormat=ACL_RULE_STD_NOT_IP;	
		 store->TcamPtr.packetType=ACL_TCAM_RULE_PACKETTYPE_ETHERNET;
		dmacAddr.arEther[0] = 0x01;
		dmacAddr.arEther[1] = 0x80;
		dmacAddr.arEther[2] = 0xC2;
		dmacAddr.arEther[3] = 0x00;
		dmacAddr.arEther[4] = 0x00;
		dmacAddr.arEther[5] = 0x00;
		for(i=0;i<6;i++)
		{
		 store->TcamPtr.ruleInfo.stdNotIp.rule.dMac[i]=dmacAddr.arEther[i];
		 store->TcamPtr.ruleInfo.stdNotIp.mask.maskSMac[i]=0xFF;
		}

		rs=npd_acl_add_notIp(store);
		if(ACL_RETURN_CODE_SUCCESS!=rs){
			   free(store);
		}
		
	}
}
#endif

int npd_acl_flow_to_port(void)
{
	unsigned int i = 0, groupnum = 0, exist_dir = 0, num = 0, groupempty = 0;
	unsigned int ret = ACL_RETURN_CODE_SUCCESS;
	struct acl_rule_info_s *ruleInfo = NULL;
	struct list_head       *Ptr = NULL;
	struct acl_group_list_s *groupInfo = NULL;
	for(i = 0; i < MAX_ACL_EXT_RULE_NUMBER; i++) {
		if (g_acl_rule[i] == NULL) {
			continue;
		}
		if ((g_acl_rule[i]->TcamPtr.packetType == ACL_TCAM_RULE_PACKETTYPE_QOS) ||
		(g_acl_rule[i]->appendInfo.appendType == 32) ||
		(g_acl_rule[i]->ActionPtr.policerPtr.policerEnable == 1)) {
			num = g_acl_rule[i]->ActionPtr.policerPtr.policerId;
			ret = search_acl_group_rule_index_exist(i, &groupnum, &exist_dir);
			if(ACL_RETURN_CODE_GLOBAL_NOT_EXISTED==ret) {
				syslog_ax_acl_dbg("you have not set the rule %d\n",i);
				continue;
			}
			else if (g_acl_rule[i]->ActionPtr.policerPtr.policerEnable == 1) {
				if(g_policer[num]->qosProfile == 0) {
					continue;
				}
				else {
					free(g_policer[num]);
					g_policer[num]=NULL;
				}
			}

			if(ACL_DIRECTION_INGRESS == exist_dir) {
				groupInfo = acl_group[groupnum];
			}
			else if(ACL_DIRECTION_EGRESS == exist_dir) {
				groupInfo = egress_acl_group[groupnum];
			}

			if (groupInfo != NULL) {
				groupempty = 1;
			}

			if (NULL != qos_append[i]) {
				qos_append[i] = NULL;
			}

			ret = nam_acl_drv_tcam_clear(i, 1);
			if(ACL_RETURN_CODE_SUCCESS != ret) {
				syslog_ax_acl_err("delete index %d in TCAM ERROR!\n",ret);
				break;
			}
			
			if(ACL_RETURN_CODE_SUCCESS != ret) {
				syslog_ax_acl_err("delete index %d in TCAM ERROR!\n",ret);
			}
			else {
				if(groupempty == 1) {
					__list_for_each(Ptr,&(groupInfo->list1)) {
						ruleInfo=list_entry(Ptr,struct acl_rule_info_s,list);
						if(NULL == ruleInfo) {
							syslog_ax_acl_dbg("null rule node on bind group %d when search\n", groupnum);
						}
						else if(i == ruleInfo->ruleIndex) {					  
							syslog_ax_acl_dbg("clear tcam success\n"); 
							__list_del((ruleInfo->list).prev, (ruleInfo->list).next);
							syslog_ax_acl_dbg("rule %d been deleted!\n", i);							

							groupInfo->count-=1; /*count--*/
							break;
						}
					}
				}	
				ret = npd_acl_delete(i, &groupnum);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					ret = ACL_RETURN_CODE_ERROR;
				}
			} 
		}
	}
	for (i = 0; i < MAX_POLICER_NUM; i++) {
		if (NULL != g_policer[i]) {
			if(0 != g_policer[i]->qosProfile) {
				free(g_policer[i]);
				g_policer[i]=NULL;
			}
		}
	}
	return ret;
}

int npd_find_share_num_by_policer_id(int policerId, int *num)
{
	unsigned int i = 0;

	for(i = 0; i < MAX_ACL_EXT_RULE_NUMBER; i++) {
		if (g_acl_rule[i] == NULL) {
			continue;
		}
		if ((g_acl_rule[i]->ActionPtr.policerPtr.policerEnable == 1) &&
			g_acl_rule[i]->ActionPtr.policerPtr.policerId == policerId) {	
				*num = g_acl_rule[i]->endIndex - g_acl_rule[i]->startIndex + 1;	
				npd_syslog_dbg("output endIndex %d,startIndex %d",g_acl_rule[i]->endIndex,g_acl_rule[i]->startIndex);
				return ACL_RETURN_CODE_SUCCESS;
		}
	}

	return ACL_RETURN_CODE_ERROR;
}

int npd_acl_time_name_check(char *Name,unsigned int *timeId)
{
   ACL_TIME_RANGE_STC *node = NULL;
   int iter = 0;

	for(iter = 0;iter < 1000; iter++) {
		node = g_time[iter];
		if(NULL == node)
			continue;
		else if('\0' == node->timeName[0]) {
			continue;	
		}
		else if(strcmp(Name,node->timeName)) {			
			continue;	
		}
		else {
		npd_syslog_dbg("time name has existed!\n");
		*timeId = node->timeId;
		return ACL_TIME_NAME_EXIST;
		}
	}

   return ACL_RETURN_CODE_SUCCESS;
}

 int npd_acl_time_activate
(
	 char *timeName,
     unsigned int *timeId
)
{
	ACL_TIME_RANGE_STC* node = NULL;
	int i=1,ret=ACL_RETURN_CODE_ERROR;

	node = (ACL_TIME_RANGE_STC *)malloc(sizeof(ACL_TIME_RANGE_STC));
	if(NULL == node) {
		npd_syslog_dbg("npd_acl_time_activate error:null memory allocated!");
		return ACL_RETURN_CODE_ERROR;
	}
	memset(node,0,sizeof(ACL_TIME_RANGE_STC));
	memcpy(node->timeName , timeName ,NPD_TIME_RANGE_SIZE);

	for(i=1;i<1000;i++) 
	{
		if(g_time[i] == NULL) 
		{				
			node->timeId  = i;				
			g_time[i]=node;

			*timeId = i;
			ret =ACL_RETURN_CODE_SUCCESS;
			break;
		}                  
	}
	/*code optimize:  Resource leak
	zhangcl@autelan.com 2013-1-17*/
	if(ret != ACL_RETURN_CODE_SUCCESS)
	{
		free(node);
	}
	return ret;
}
 
 ACL_TIME_RANGE_STC *npd_acl_time_get_by_index(unsigned int timeId)
 {
	 ACL_TIME_RANGE_STC *node=NULL;
	  	   
	 if(g_time[timeId]!=NULL) {
	  node = g_time[timeId];
	 /* npd_syslog_dbg("***g_time[timeId] %p\n",g_time[timeId]);*/
	  return node;		   
	 }
	   				  
	 return NULL; 
 }
 
int npd_acl_time_period_check(unsigned int timeId,unsigned int period)
{
	ACL_TIME_RANGE_STC *node=NULL;
	unsigned int ret=ACL_RETURN_CODE_ERROR;
	unsigned int num=0;

	/*npd_syslog_dbg("timeId %d,period %d\n",timeId,period);*/
	
	node=npd_acl_time_get_by_index(timeId);

	/*npd_syslog_dbg("***node %p\n",node);*/

	if(NULL!=node) {
		num = node->periodTime[period].index;
		if((period==num)&&(period!=0)) {
			ret =ACL_TIME_PERIOD_EXISTED;
		}
		else if((0==num)||(period==0)) {
			ret =ACL_RETURN_CODE_SUCCESS;
		}	
	}
	return ret;
}

DBusMessage * npd_dbus_time_range_set(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	unsigned int	timeId=0;
	char *timeName = NULL;
	unsigned int	ret = ACL_RETURN_CODE_ERROR;
	DBusError err;
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
							 DBUS_TYPE_STRING,&timeName,
							 DBUS_TYPE_INVALID))) {
		 syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	
	ret = npd_acl_time_name_check(timeName,&timeId);
	if(ACL_TIME_NAME_EXIST==ret)
	 {
	  npd_syslog_dbg("time name existed!\n");							
	 }
	else {
		ret = npd_acl_time_activate(timeName,&timeId);
		if(ret!=ACL_RETURN_CODE_SUCCESS)
		{
		 npd_syslog_dbg("create time fail!\n");
		}
	}
	npd_syslog_dbg("timeId %d\n",timeId);
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32, 
									 &ret);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32, 
									 &timeId);								 
	return reply;
	
}

DBusMessage * npd_dbus_time_absolute_set(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	unsigned int	 timeId=0;
	unsigned int	 startyear=0,startmonth=0,startday=0,starthour=0,startminute=0;
	unsigned int	 endyear=0,endmonth=0,endday=0,endhour=0,endminute=0;
	unsigned int	ret = ACL_RETURN_CODE_SUCCESS;
	ACL_TIME_RANGE_STC	*node=NULL;
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
							 DBUS_TYPE_UINT32,&timeId,
							 DBUS_TYPE_UINT32,&startyear,
							 DBUS_TYPE_UINT32,&startmonth,
							 DBUS_TYPE_UINT32,&startday,
							 DBUS_TYPE_UINT32,&starthour,
							 DBUS_TYPE_UINT32,&startminute,
							 DBUS_TYPE_UINT32,&endyear,
							 DBUS_TYPE_UINT32,&endmonth,
							 DBUS_TYPE_UINT32,&endday,
							 DBUS_TYPE_UINT32,&endhour,
							 DBUS_TYPE_UINT32,&endminute,	
							 DBUS_TYPE_INVALID))) {
		 syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
   npd_syslog_dbg("timeId %d\n",timeId);
	  node=npd_acl_time_get_by_index(timeId); 
	  if(node!=NULL)
	  {
			node->abstartime[0]=	startyear;
			node->abstartime[1]=	startmonth;
			node->abstartime[2]=	startday;
			node->abstartime[3]=	starthour;
			node->abstartime[4]=	startminute;  
			node->absendtime[0]=	endyear;
			node->absendtime[1]=	endmonth;
			node->absendtime[2]=	endday;
			node->absendtime[3]=	endhour;
			node->absendtime[4]=	endminute;
			ret =ACL_RETURN_CODE_SUCCESS;
	  }
	  else {
		  ret=ACL_TIME_NAME_NOTEXIST;
	  }
  
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32, 
									 &ret);
						 
	return reply;
	
}
	
	

DBusMessage * npd_dbus_time_periodic_set(DBusConnection *conn, DBusMessage *msg, void *user_data)	
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	unsigned int	 timeId=0,period=0;
	unsigned int	flag=0,starthour=0,startmin=0,endhour=0,endmin=0;
	unsigned int	ret = ACL_RETURN_CODE_SUCCESS;
	ACL_TIME_RANGE_STC	*node=NULL;
	PERIODINFO_STC		 *periodPtr=NULL;
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
							 DBUS_TYPE_UINT32,&timeId,
							 DBUS_TYPE_UINT32,&period,
							 DBUS_TYPE_UINT32,&flag,
							 DBUS_TYPE_UINT32,&starthour,
							 DBUS_TYPE_UINT32,&startmin,						 
							 DBUS_TYPE_UINT32,&endhour,
							 DBUS_TYPE_UINT32,&endmin,	
							 DBUS_TYPE_INVALID))) {
		 syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
   	npd_syslog_dbg("timeId %d\n",timeId);
	  node=npd_acl_time_get_by_index(timeId); 
	  if(node!=NULL)
	  {
		   ret=npd_acl_time_period_check(timeId,period);
		   if(ret==ACL_TIME_PERIOD_EXISTED){
			   npd_syslog_dbg("period existed!\n"); 								 
		   }
		   else if(ret==ACL_RETURN_CODE_SUCCESS)
		   {
				periodPtr=&(node->periodTime[period]);   
			   
				periodPtr->index       =period;
				periodPtr->timeType    =(ACL_TIME_RANGE_TYPE)flag;
				periodPtr->startime[0] =starthour;
				periodPtr->startime[1] =startmin;
				periodPtr->endtime[0]  =endhour;
				periodPtr->endtime[1]  =endmin;
				
				node->periodCount++;
				npd_syslog_dbg("node->periodCount %d\n",node->periodCount);
				ret =ACL_RETURN_CODE_SUCCESS;
		   }
	  }
	  else {
		  ret=ACL_TIME_NAME_NOTEXIST;
	  }
  
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32, 
									 &ret);
						 
	return reply;
	
}
	
DBusMessage * npd_dbus_time_range_show(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*			 reply = NULL;
	DBusMessageIter 		 iter;
	DBusMessageIter 		 iter_array;
	DBusError				err;	

	unsigned int		  index=0,time_count=0,period_count=0,j=0,m=0,n=0,count=0,tmpCount=0,rem=0; 
	ACL_TIME_RANGE_STC    *timePtr=NULL;
	ACL_TIME_RANGE_SHOW   *timeShow[1000];
	PERIODINFO_STC		  *periodShow[65],*periodPtr=NULL;
	unsigned int 			ret = 0;

	dbus_error_init(&err);
	
	for(index=0;index<1000;index++)
	  {
		 timePtr = g_time[index];	
		 if(NULL!=timePtr)
			{
				timeShow[time_count] = (ACL_TIME_RANGE_SHOW*)malloc(sizeof(ACL_TIME_RANGE_SHOW));
				memset(timeShow[time_count],0,sizeof(ACL_TIME_RANGE_STC));	

				timeShow[time_count]->timeName	=timePtr->timeName;
				timeShow[time_count]->abstartime[0] =timePtr->abstartime[0];
				timeShow[time_count]->abstartime[1] =timePtr->abstartime[1];
				timeShow[time_count]->abstartime[2] =timePtr->abstartime[2];
				timeShow[time_count]->abstartime[3] =timePtr->abstartime[3];
				timeShow[time_count]->abstartime[4] =timePtr->abstartime[4];
				timeShow[time_count]->absendtime[0] =timePtr->absendtime[0];
				timeShow[time_count]->absendtime[1] =timePtr->absendtime[1];
				timeShow[time_count]->absendtime[2] =timePtr->absendtime[2];
				timeShow[time_count]->absendtime[3] =timePtr->absendtime[3];
				timeShow[time_count]->absendtime[4] =timePtr->absendtime[4];
				timeShow[time_count]->periodCount	=timePtr->periodCount;
				
				count = timePtr->periodCount;
				npd_syslog_dbg("count %d\n",count);    
				
				/*for(i=0;i<count;i++)*/
			/*{	*/				   
					for(j=0;j<65;j++)
					{
						 ret=npd_acl_time_period_check(index,j);
						 if(ret==ACL_TIME_PERIOD_EXISTED){
						/*	npd_syslog_dbg("period %d existed!\n",j);	*/
						  /*  npd_syslog_dbg("timePtr->periodTime[%d] %p\n",j,timePtr->periodTime[j]);*/
							periodPtr=&(timePtr->periodTime[j]);
						/*    npd_syslog_dbg("periodPtr %p\n",periodPtr);*/

							periodShow[period_count] =  (PERIODINFO_STC*)malloc(sizeof(PERIODINFO_STC));
				   			memset(periodShow[period_count],0,sizeof(PERIODINFO_STC));
				   
							periodShow[period_count]->index 	 =periodPtr->index;
							periodShow[period_count]->timeType	 =periodPtr->timeType; 
							periodShow[period_count]->startime[0]=periodPtr->startime[0];
							periodShow[period_count]->startime[1]=periodPtr->startime[1];
							periodShow[period_count]->endtime[0] =periodPtr->endtime[0];
							periodShow[period_count]->endtime[1] =periodPtr->endtime[1];						 
						  
							period_count++;
						 }			  
					}							   
				/*}*/
												
				time_count++;
			 }	
	  }
	npd_syslog_dbg("***********\n");
	if(time_count == 0) {
			ret=ACL_TIME_NAME_NOTEXIST;
			npd_syslog_dbg("no time  existed!\n");
	}
	if(period_count == 0) {
			ret = ACL_TIME_PERIOD_NOT_EXISTED;
			npd_syslog_dbg("no period  existed!\n");
	}	
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret); 
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &time_count);
	npd_syslog_dbg("time_count %d\n",time_count);
	dbus_message_iter_open_container (&iter,
								   DBUS_TYPE_ARRAY,
									DBUS_STRUCT_BEGIN_CHAR_AS_STRING	 /*begin	*/								 
									   DBUS_TYPE_STRING_AS_STRING	 /*name */
									   DBUS_TYPE_UINT32_AS_STRING	 
									   DBUS_TYPE_UINT32_AS_STRING
									   DBUS_TYPE_UINT32_AS_STRING
									   DBUS_TYPE_UINT32_AS_STRING
									   DBUS_TYPE_UINT32_AS_STRING
									   DBUS_TYPE_UINT32_AS_STRING
									   DBUS_TYPE_UINT32_AS_STRING
									   DBUS_TYPE_UINT32_AS_STRING
									   DBUS_TYPE_UINT32_AS_STRING
									   DBUS_TYPE_UINT32_AS_STRING
									   DBUS_TYPE_UINT32_AS_STRING  /*pecount*/
									   DBUS_TYPE_ARRAY_AS_STRING
										  DBUS_STRUCT_BEGIN_CHAR_AS_STRING	
										  DBUS_TYPE_UINT32_AS_STRING	
										  DBUS_TYPE_UINT32_AS_STRING
										  DBUS_TYPE_UINT32_AS_STRING
										  DBUS_TYPE_UINT32_AS_STRING
										  DBUS_TYPE_UINT32_AS_STRING
										  DBUS_TYPE_UINT32_AS_STRING
										  DBUS_STRUCT_END_CHAR_AS_STRING 
									DBUS_STRUCT_END_CHAR_AS_STRING, 	/*end*/
									&iter_array);

	if(time_count>0){
		 for (m = 0; m < time_count; m++ ) {
			DBusMessageIter iter_struct;
			DBusMessageIter iter_sub_array;
			
			dbus_message_iter_open_container (&iter_array,
											   DBUS_TYPE_STRUCT,
											   NULL,
											   &iter_struct);
			npd_syslog_dbg("open first-inner container\n");		
								
			dbus_message_iter_append_basic
					(&iter_struct,
					  DBUS_TYPE_STRING,
					  &timeShow[m]->timeName);
			dbus_message_iter_append_basic 
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &(timeShow[m]->abstartime[0]));
		   dbus_message_iter_append_basic 
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &(timeShow[m]->abstartime[1]));
		   dbus_message_iter_append_basic 
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &(timeShow[m]->abstartime[2]));
		   dbus_message_iter_append_basic 
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &(timeShow[m]->abstartime[3]));
		   dbus_message_iter_append_basic 
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &(timeShow[m]->abstartime[4]));
	   dbus_message_iter_append_basic 
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &(timeShow[m]->absendtime[0]));
		   dbus_message_iter_append_basic 
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &(timeShow[m]->absendtime[1]));
		   dbus_message_iter_append_basic 
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &(timeShow[m]->absendtime[2]));
		   dbus_message_iter_append_basic 
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &(timeShow[m]->absendtime[3]));
		   dbus_message_iter_append_basic 
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &(timeShow[m]->absendtime[4]));		
			dbus_message_iter_append_basic 
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &(timeShow[m]->periodCount));		  
						 
			tmpCount=timeShow[m]->periodCount;
			npd_syslog_dbg("tmpCount %d\n",tmpCount);
			npd_syslog_dbg("open second-inner container\n");

			dbus_message_iter_open_container (&iter_struct,
											   DBUS_TYPE_ARRAY,
											   DBUS_STRUCT_BEGIN_CHAR_AS_STRING  
												  DBUS_TYPE_UINT32_AS_STRING	
												   DBUS_TYPE_UINT32_AS_STRING
												   DBUS_TYPE_UINT32_AS_STRING
												   DBUS_TYPE_UINT32_AS_STRING
												   DBUS_TYPE_UINT32_AS_STRING
												   DBUS_TYPE_UINT32_AS_STRING
											   DBUS_STRUCT_END_CHAR_AS_STRING, 
											   &iter_sub_array);
			for(n=0;n<tmpCount;n++) {

				DBusMessageIter iter_sub_struct;				
				
				dbus_message_iter_open_container (&iter_sub_array,
													DBUS_TYPE_STRUCT,
													NULL,
												  &iter_sub_struct);
				dbus_message_iter_append_basic 
						(&iter_sub_struct,
						 DBUS_TYPE_UINT32,
						 &(periodShow[n+rem]->index));
				dbus_message_iter_append_basic 
						(&iter_sub_struct,
						 DBUS_TYPE_UINT32,
						 &(periodShow[n+rem]->timeType));
				dbus_message_iter_append_basic 
						(&iter_sub_struct,
						 DBUS_TYPE_UINT32,
						 &(periodShow[n+rem]->startime[0]));
				dbus_message_iter_append_basic 
						(&iter_sub_struct,
						 DBUS_TYPE_UINT32,
						 &(periodShow[n+rem]->startime[1]));
				dbus_message_iter_append_basic 
						(&iter_sub_struct,
						 DBUS_TYPE_UINT32,
						 &(periodShow[n+rem]->endtime[0]));
				dbus_message_iter_append_basic 
						(&iter_sub_struct,
						 DBUS_TYPE_UINT32,
						 &(periodShow[n+rem]->endtime[1]));
						 
				npd_syslog_dbg("index:%d\n",periodShow[n+rem]->index);
				dbus_message_iter_close_container (&iter_sub_array, &iter_sub_struct);
				free(periodShow[n+rem]);
				periodShow[n+rem] = NULL;
			}
			rem += tmpCount;
			dbus_message_iter_close_container (&iter_struct, &iter_sub_array);			
			dbus_message_iter_close_container (&iter_array, &iter_struct);
			free(timeShow[m]);
			timeShow[m] = NULL;
		} 
	} 
	dbus_message_iter_close_container (&iter, &iter_array);

	return reply;

}
DBusMessage * npd_dbus_time_range_acl(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*			 reply = NULL;
	DBusMessageIter 		 iter;
	DBusError				 err;	
	AclRuleList 			*rulePtr=NULL;
	unsigned int			ruleIndex=0;
	char 					*timeName=NULL;
	unsigned int			timeId = 0,ret = 0,rs = 0;
	unsigned int			groupNum = 0,exist_dir = 0;

	dbus_error_init(&err);
		
	if (!(dbus_message_get_args ( msg, &err,
								DBUS_TYPE_UINT32,&ruleIndex,
								DBUS_TYPE_STRING,&timeName,
								DBUS_TYPE_INVALID))) {
		syslog_ax_acl_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_acl_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	if(NULL == g_acl_rule[ruleIndex]) {
		npd_syslog_err("rule %ld not existed!\n",ruleIndex);
		ret= ACL_RETURN_CODE_GLOBAL_NOT_EXISTED;
	}
	else {
		/*check time name*/
		ret=npd_acl_time_name_check(timeName,&timeId);
		if(ret!=ACL_RETURN_CODE_SUCCESS){
			ret = ACL_TIME_NAME_NOTEXIST;
		}
		else{
			/*check action permit or deny*/
		    rs=npd_acl_drv_time_rule_in_action_check(ruleIndex);
			if(rs != ACL_RETURN_CODE_SUCCESS) {
				ret=ACL_RETURN_CODE_RULE_TIME_NOT_SUPPORT;
			}
			else {
				/*append qos mark atrributs to acl*/
				rulePtr=g_acl_rule[ruleIndex];
				rulePtr->appendInfo.appendType =128;
				memcpy(rulePtr->timeName , timeName ,NPD_TIME_RANGE_SIZE);
			
				rs=search_acl_group_rule_index_exist(ruleIndex,&groupNum,&exist_dir);			
				if(ACL_RETURN_CODE_GROUP_RULE_EXISTED == rs) {
					npd_syslog_dbg("acl in group %d\n",groupNum);
					/*update HW action
				ret = nam_acl_time_action_update(ruleIndex);
				if(ret!=0)
					npd_syslog_dbg("update fail!\n");*/		      	
				}
				
				if(ACL_ACTION_TYPE_PERMIT==rulePtr->ActionPtr.actionType) {	
					rulePtr->ActionPtr.actionType= ACL_ACTION_TYPE_DENY;
				}
				else if(ACL_ACTION_TYPE_DENY==rulePtr->ActionPtr.actionType) {
					rulePtr->ActionPtr.actionType = ACL_ACTION_TYPE_PERMIT;
				}
		    }
		}
	}
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	return reply;	


}
DBusMessage * npd_dbus_qos_acl_apend(DBusConnection *conn, DBusMessage *msg, void *user_data)
{	
	DBusMessage*		reply = NULL;
	DBusMessageIter 	iter;
	DBusError			err;	
	AclRuleList			*rulePtr=NULL;
      unsigned int 		ruleIndex=0,profileIndex=0;
	unsigned int    	ret=0,rs;
	unsigned int 		groupNum=0,exist_dir=0;
	struct QOS_APPEND_PROFILE_STC *qosappend = NULL;
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
								DBUS_TYPE_UINT32,&ruleIndex,
								DBUS_TYPE_UINT32,&profileIndex,
								DBUS_TYPE_INVALID))) {
		syslog_ax_acl_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_acl_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	
	qosappend = (struct QOS_APPEND_PROFILE_STC *)malloc(sizeof(struct QOS_APPEND_PROFILE_STC));
	if (qosappend == NULL) {
		rs = ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
	}
	else {
		memset(qosappend, 0, sizeof(struct QOS_APPEND_PROFILE_STC));
	}

	rs=npd_qos_profile_index_check(profileIndex);
	if(rs != 0) {
		syslog_ax_acl_dbg("qos profile not exiseted\n");
		free(qosappend);
		ret = QOS_RETURN_CODE_PROFILE_NOT_EXISTED;
	}
	else {
		if ((qos_mode != QOS_MODE_FLOW)&&(qos_mode != QOS_MODE_HYBRID)) {
			if (qos_mode == QOS_MODE_DEFAULT) {
				free(qosappend);
				ret = ACL_RETURN_CODE_NO_QOS_MODE;
			}
			else {
				free(qosappend);
				ret = ACL_RETURN_CODE_ALREADY_PORT;
			}	
		}	
		else if ((qos_mode == QOS_MODE_HYBRID)&&(profileIndex < (MAX_HYBRID_UP+1))) {
			free(qosappend);
			ret = ACL_RETURN_CODE_HYBRID_FLOW;
		}	
		else{
			if(NULL == g_acl_rule[ruleIndex]) {
				npd_syslog_err("rule %ld not existed!\n",ruleIndex);
				free(qosappend);
				ret= ACL_RETURN_CODE_GLOBAL_NOT_EXISTED;
			}
			else if (g_acl_rule[ruleIndex]->ruleType != EXTENDED_ACL_RULE) {
				free(qosappend);
				ret = ACL_RETURN_CODE_RULE_EXT_ONLY;
			}
			else {
				if ((g_acl_rule[ruleIndex]->TcamPtr.packetType == ACL_TCAM_RULE_PACKETTYPE_QOS) ||
					(g_acl_rule[ruleIndex]->ActionPtr.policerPtr.policerEnable == 1) ||
					(g_acl_rule[ruleIndex]->ActionPtr.actionType != ACL_ACTION_TYPE_PERMIT)) {
					free(qosappend);
					ret = ACL_RETURN_CODE_ERROR;
				}
				else {					
					/*check if acl in group ,if yes,update action in HW*/
					rs=search_acl_group_rule_index_exist(ruleIndex,&groupNum,&exist_dir);
					
					if((ACL_RETURN_CODE_GROUP_RULE_EXISTED == rs)||(ACL_RETURN_CODE_EGRESS_GROUP_RULE_EXISTED == rs)) {
						npd_syslog_dbg("acl in group %d\n", groupNum);
						free(qosappend);
						ret = rs;
					}	
					else {
						/*append qos mark atrributs to acl*/
						qosappend->aclIndex = ruleIndex;
						qosappend->profileIndex = profileIndex;	
						syslog_ax_acl_dbg("aclIndex %d, profileIndex %d\n",qosappend->aclIndex, qosappend->profileIndex);
						qos_append[ruleIndex] = qosappend;
						syslog_ax_acl_dbg("aclIndex %d, profileIndex %d\n",qos_append[ruleIndex]->aclIndex, qos_append[ruleIndex]->profileIndex);
						rulePtr=g_acl_rule[ruleIndex];
						rulePtr->appendInfo.appendType = 32;
						rulePtr->ActionPtr.actionInfo.qos.qos.ingress.profileIndex=profileIndex;
						rulePtr->ActionPtr.actionInfo.qos.qos.ingress.profileAssignIndex = ACL_TRUE;
						rulePtr->ActionPtr.actionInfo.qos.qos.ingress.profilePrecedence =QOS_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
						rulePtr->ActionPtr.actionInfo.qos.modifyDscp = ENABLE_E;
						rulePtr->ActionPtr.actionInfo.qos.modifyUp = ENABLE_E;
						/*check if acl in group not permit this action now */
						/*
						rs=search_acl_group_rule_index_exist(ruleIndex,&groupNum,&exist_dir);
						
						if((ACL_RETURN_CODE_GROUP_RULE_EXISTED == rs)||(ACL_RETURN_CODE_EGRESS_GROUP_RULE_EXISTED == rs)) {
							npd_syslog_dbg("acl in group %d\n",groupNum);

							if (g_acl_rule[ruleIndex]->ruleType == EXTENDED_ACL_RULE)  {
								ret = nam_acl_qos_action_update(ruleIndex);
								if(ret!= 0)
									npd_syslog_dbg("update fail!\n");
							}
							else if (g_acl_rule[ruleIndex]->ruleType == STANDARD_ACL_RULE) {
								ret = ACL_RETURN_CODE_STD_RULE;
							}	
						}
						*/
					}
				}
			}			
		}
	}
	/*
	ret = nam_bcm_acl_support_check();
	*/
	npd_syslog_dbg("ret %d\n", ret);
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	return reply;	

}

DBusMessage * npd_dbus_delete_acl_to_qos_profile_table(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
    DBusMessage*	reply;    
	DBusMessageIter	iter;
	DBusError		err;      
    
    unsigned int       aclIndex = 0;
    unsigned int       ret = ACL_RETURN_CODE_SUCCESS, exist_dir = 0, num = 0;

    dbus_error_init(&err);
    if (!(dbus_message_get_args ( msg, &err,  
       DBUS_TYPE_UINT32,&aclIndex,	  
	   DBUS_TYPE_INVALID))) {
	   npd_syslog_err(("Unable to get input args "));
	   if (dbus_error_is_set(&err)) {
		   npd_syslog_err(("%s raised: %s",err.name,err.message));
		   dbus_error_free(&err);
	   }
	   return NULL;
    }  

	aclIndex = aclIndex - 1;
	ret = search_acl_group_rule_index_exist(aclIndex, &num, &exist_dir);
	if((ACL_RETURN_CODE_GROUP_RULE_EXISTED == ret)||(ACL_RETURN_CODE_EGRESS_GROUP_RULE_EXISTED == ret)) {
		ret = ACL_RETURN_CODE_ERROR;
	}
	else {
		if(NULL != qos_append[aclIndex]) {
			ret = nam_acl_qos_action_clear(aclIndex);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				ret = ACL_RETURN_CODE_ERROR;
			}
			else {
				memset(&(g_acl_rule[aclIndex]->appendInfo), 0, sizeof(ACL_APPEND_INFO_STC));
				free(qos_append[aclIndex]);
				qos_append[aclIndex] = NULL;	
			}
		}
		else {
			ret = QOS_RETURN_CODE_NO_MAPPED;
		}
	}		 
  	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	
	return reply;	    
}

DBusMessage * npd_dbus_show_qos_acl_apend(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage*			 reply = NULL;
	DBusMessageIter 		 iter, iter_array;		 
	DBusError				 err;	
	unsigned int			 i=0,j=0,count=0,ret =ACL_RETURN_CODE_SUCCESS;
	QosAppendProfile *show_append[MAX_ACL_RULE_NUMBER];
	/*QosAppendProfile *append = NULL;*/
	dbus_error_init(&err);

	for(i=0;i < MAX_ACL_EXT_RULE_NUMBER;i++){
		if(qos_append[i] != NULL){
			/*append = qos_append[i];*/
			syslog_ax_acl_dbg("aclIndex %d, profileIndex %d\n",qos_append[i]->aclIndex, qos_append[i]->profileIndex);
			show_append[count] = (QosAppendProfile*)malloc(sizeof(QosAppendProfile));
			memset(show_append[count],0,sizeof(QosAppendProfile));
			show_append[count]->aclIndex = qos_append[i]->aclIndex + 1;
			show_append[count]->profileIndex =  qos_append[i]->profileIndex;
			syslog_ax_acl_dbg("aclIndex %d, profileIndex %d\n",show_append[count]->aclIndex, show_append[count]->profileIndex);
			count++;
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
	dbus_message_iter_open_container (&iter,
								   DBUS_TYPE_ARRAY,
									DBUS_STRUCT_BEGIN_CHAR_AS_STRING	 /*begin*/
									   DBUS_TYPE_UINT32_AS_STRING	 /*acl index*/
									   DBUS_TYPE_UINT32_AS_STRING	 /*qos profile index*/
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
				  &(show_append[j]->aclIndex));
		dbus_message_iter_append_basic
				  (&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(show_append[j]->profileIndex));
		syslog_ax_acl_dbg("aclIndex %d, profileIndex %d\n",show_append[j]->aclIndex, show_append[j]->profileIndex);
		dbus_message_iter_close_container (&iter_array, &iter_struct);
		free(show_append[j]);
		show_append[j]=NULL;
	}
	dbus_message_iter_close_container (&iter, &iter_array);

	return reply;	
}

int npd_acl_check_maczero(unsigned char *mac) {
	int i = 0;
	unsigned int ret = NPD_TRUE;

	if(!mac) {
		return ret;
	}
	
	for(i = 0;i < MAC_ADDRESS_LEN; i++) {
		if(mac[i]!=0) {
			ret = NPD_FALSE;
			break;
		}
	}

	return ret;
}
int npd_acl_show_policer_rule
(
	char **buffer,
	struct	ACL_ACTION_STC *actionPtr,
	int *byteCnt
)
{
	int length = 0,totalLen = 0;
	char *tmpPtr = NULL;
	unsigned int policerid = actionPtr->policerPtr.policerId;

	/*code optimize:  Dereference before null check
	zhangcl@autelan.com 2013-1-17*/
	if(!buffer) 
	{
		return ACL_RETURN_CODE_ERROR;
	}
	tmpPtr = *buffer;

	if(!tmpPtr) 
	{
		return ACL_RETURN_CODE_ERROR;
	}
	length = sprintf(tmpPtr,"policer %d\n", policerid);
	tmpPtr += length;
	totalLen += length;
		
	*byteCnt = totalLen;
	return ACL_RETURN_CODE_SUCCESS;

}

int npd_acl_show_running_action
(
	char **buffer,
	unsigned int ruleType,
	unsigned int ruleIndex,
	struct	ACL_ACTION_STC *actionPtr,
	int *byteCnt
)
{	
	int length = 0,totalLen = 0;
	char *tmpPtr = NULL;
	unsigned int retVal=0;
	unsigned int mslot_no=0,mport_no=0;	 
	ACL_ACTION_TYPE_E action = actionPtr->actionType;
	struct ACL_ACTION_REDIRECT_STC			*redirect = NULL;
	struct ACL_ACTION_QOS_STC				*qos = NULL;
	/*ACL_RULE_STDIPV4L4_T 					*stdIpv4L4=NULL;*/
	/*enum product_id_e productId = PRODUCT_ID; */

	/*code optimize:  Dereference before null check
	zhangcl@autelan.com 2013-1-17*/
	if(!buffer) 
	{
		return ACL_RETURN_CODE_ERROR;
	}
	tmpPtr = *buffer;

	if(!tmpPtr) 
	{
		return ACL_RETURN_CODE_ERROR;
	}
	
	length = sprintf(tmpPtr," acl %s %d ",(ruleType == STANDARD_ACL_RULE) ? "standard":"extended" ,ruleIndex);
	tmpPtr += length;
	totalLen += length;
	
	if(ACL_ACTION_TYPE_PERMIT == action) {
		length = sprintf(tmpPtr,"permit "); 					
	}
	else if(ACL_ACTION_TYPE_DENY == action) {
		length = sprintf(tmpPtr,"deny ");						 
	}
	else if(ACL_ACTION_TYPE_TRAP_TO_CPU==action) {
		length = sprintf(tmpPtr,"trap ");					   
	}
	else if(ACL_ACTION_TYPE_MIRROR_TO_ANALYZER==action) {
		length = sprintf(tmpPtr,"permit "); 
	}
	else if(ACL_ACTION_TYPE_REDIRECT==action){
		length = sprintf(tmpPtr,"redirect ");						
		tmpPtr += length;
		totalLen += length;

		redirect = &(actionPtr->actionInfo.redirect);
		/*if(0 != redirect->portNum) {*/
		/*code optimize:  unsigned compared against 0
			zhangcl@autelan.com 2013-1-17*/
		if(redirect->portNum < CPU_PORT_ASIC_PORTNO) {
			retVal=nam_get_slotport_by_devport((unsigned int)(redirect->modid),(unsigned int)(redirect->portNum),PRODUCT_ID,&mslot_no,&mport_no); 
			if(retVal!=0) {
				syslog_ax_acl_err("get acl rule slot port from redirect to port %d fail!",redirect->portNum);
				return (ACL_RETURN_CODE_ERROR-2);
			}
			else {
				syslog_ax_acl_dbg("find redirect to port %d on %d/%d",redirect->portNum,mslot_no,mport_no);
			}
			length = sprintf(tmpPtr,"%d/%d ",mslot_no,mport_no);					  
		} 													   
	}
	else if(ACL_ACTION_TYPE_INGRESS_QOS==action){
		length =sprintf(tmpPtr,"ingress-qos ");
		tmpPtr += length;
		totalLen += length;

		qos=&(actionPtr->actionInfo.qos);	
		length = sprintf(tmpPtr,"%d ",qos->qos.ingress.profileIndex);	
		tmpPtr += length;
		totalLen += length;

		length =sprintf(tmpPtr,"sub-qos-markers ");
		tmpPtr += length;
		totalLen += length;

		if(QOS_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E==(qos->qos.ingress.profilePrecedence))
			length =sprintf(tmpPtr,"enable ");
		else if(QOS_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E==(qos->qos.ingress.profilePrecedence))
			length =sprintf(tmpPtr,"disable ");		
	}
	else if(ACL_ACTION_TYPE_EGRESS_QOS==action){
		length =sprintf(tmpPtr,"egress-qos ");	
		tmpPtr += length;
		totalLen += length;

		qos=&(actionPtr->actionInfo.qos);	

		length=sprintf(tmpPtr,"egress-up %d ",qos->qos.egress.egrUp);
		tmpPtr += length;
		totalLen += length;

		length=sprintf(tmpPtr,"egress-dscp %d ",qos->qos.egress.egrDscp);		
	}
	else {
		return (ACL_RETURN_CODE_ERROR-3);
	}
	
	tmpPtr += length;
	totalLen += length;

	*byteCnt = totalLen;
	return ACL_RETURN_CODE_SUCCESS;
}

int npd_acl_show_standard_qos_rule
(
	char **buffer,
	ACL_RULE_STDIPV4L4_T *stdIp,
	int *byteCnt

)
{
	char *tmpPtr = NULL;
	int length = 0,totalLen = 0;
	/*unsigned long ipAddr = 0;*/

	/*code optimize:  Dereference before null check
	zhangcl@autelan.com 2013-1-17*/
	if(!buffer) 
	{
		return ACL_RETURN_CODE_ERROR;
	}
	tmpPtr = *buffer;

	if(!tmpPtr) 
	{
		return ACL_RETURN_CODE_ERROR;
	}

	length = sprintf(tmpPtr,"source-up "); 					 
	tmpPtr += length;
	totalLen += length;

	if(0x7==(stdIp->mask.maskUP))
		length = sprintf(tmpPtr,"%d ",stdIp->rule.UP); 		
	else
		length = sprintf(tmpPtr,"none "); 

	tmpPtr += length;						
	totalLen += length;

	
	if(0x3F==(stdIp->mask.maskDSCP))
		length = sprintf(tmpPtr,"source-dscp %d ",stdIp->rule.DSCP);		
	else
		length = sprintf(tmpPtr,"source-dscp none "); 
	
	tmpPtr += length;						
	totalLen += length;

	*byteCnt = totalLen;

	return ACL_RETURN_CODE_SUCCESS;
}

int npd_acl_show_standard_ipv4_rule
(
	char **buffer,
	ACL_RULE_STDIPV4L4_T *stdIp,
	int *byteCnt
) {
	char *tmpPtr = NULL;
	int length = 0,totalLen = 0;
	unsigned long ipAddr = 0;

	/*code optimize:  Dereference before null check
	zhangcl@autelan.com 2013-1-17*/
	if(!buffer) 
	{
		return ACL_RETURN_CODE_ERROR;
	}
	tmpPtr = *buffer;

	if(!tmpPtr) 
	{
		return ACL_RETURN_CODE_ERROR;
	}

	length = sprintf(tmpPtr,"ip ");						 
	tmpPtr += length;
	totalLen += length;

	/* destination ip */
	ipAddr = stdIp->rule.Dip;
	if(ipAddr != 0) {  
		length = sprintf(tmpPtr,"dip %ld.%ld.%ld.%ld/%ld ",(ipAddr>>24)&0xff,(ipAddr>>16)&0xff,	\
								(ipAddr>>8)&0xff,ipAddr&0xff,stdIp->mask.maskLenDip);					   
	}
	else {
		length = sprintf(tmpPtr,"dip any ");					   
	}
	tmpPtr += length;						
	totalLen += length;

	/* source ip*/
	ipAddr = stdIp->rule.Sip;
	if(ipAddr != 0) {  
		length = sprintf(tmpPtr,"sip %ld.%ld.%ld.%ld/%ld ",(ipAddr>>24)&0xff,(ipAddr>>16)&0xff,	\
								(ipAddr>>8)&0xff,ipAddr&0xff,stdIp->mask.maskLenSip);					   
	}
	else {
		length = sprintf(tmpPtr,"sip any ");					   
	}
	tmpPtr += length;						
	totalLen += length;

	*byteCnt = totalLen;

	return ACL_RETURN_CODE_SUCCESS;
	}

/* L4type: 0 for tcp, 1 for udp*/
int npd_acl_show_standard_ipv4_L4_rule
(
	char **buffer,
	ACL_RULE_STDIPV4L4_T *stdIpv4L4,
	unsigned char L4type,
	int *byteCnt
) {
	char *tmpPtr = NULL;
	int length = 0,totalLen = 0;
	unsigned long ipAddr = 0,L4port = 0;
	
	/*code optimize:  Dereference before null check
	zhangcl@autelan.com 2013-1-17*/
	if(!buffer) 
	{
		return ACL_RETURN_CODE_ERROR;
	}
	tmpPtr = *buffer;

	if(!tmpPtr) 
	{
		return ACL_RETURN_CODE_ERROR;
	}

	if((L4type != 0) && (L4type != 1)) {
		return (ACL_RETURN_CODE_ERROR-1);
	}
	length = sprintf(tmpPtr,"%s ",L4type ? "udp":"tcp");						 
	tmpPtr += length;
	totalLen += length;

	ipAddr = stdIpv4L4->rule.Dip;
	if(ipAddr != 0) {  
		length = sprintf(tmpPtr,"dip %ld.%ld.%ld.%ld/%ld ",(ipAddr>>24)&0xff,(ipAddr>>16)&0xff,	\
								(ipAddr>>8)&0xff,ipAddr&0xff,stdIpv4L4->mask.maskLenDip);					   
	}
	else {
		length = sprintf(tmpPtr,"dip any ");					   
	}
	tmpPtr += length;						
	totalLen += length;

	L4port = stdIpv4L4->rule.DstPort;
	if(0 != stdIpv4L4->mask.maskDstPort) {
		 length = sprintf(tmpPtr,"dst-port %ld ",stdIpv4L4->rule.DstPort);						 
	}
	else {
		 length = sprintf(tmpPtr,"dst-port any ");						 
	}
	tmpPtr += length;							 
	totalLen += length;

	ipAddr = stdIpv4L4->rule.Sip;
	if(ipAddr != 0) {  
		length = sprintf(tmpPtr,"sip %ld.%ld.%ld.%ld/%ld ",(ipAddr>>24)&0xff,(ipAddr>>16)&0xff,	\
								(ipAddr>>8)&0xff,ipAddr&0xff,stdIpv4L4->mask.maskLenSip);					   
	}
	else {
		length = sprintf(tmpPtr,"sip any ");					   
	}
	tmpPtr += length;						
	totalLen += length;

	L4port = stdIpv4L4->rule.SrcPort;
	if(0 != stdIpv4L4->mask.maskSrcPort) {
		 length = sprintf(tmpPtr,"src-port %ld ",stdIpv4L4->rule.SrcPort);						 
	}
	else {
		 length = sprintf(tmpPtr,"src-port any ");						 
	}
	tmpPtr += length;							 
	totalLen += length;
	
	*byteCnt = totalLen;

	return ACL_RETURN_CODE_SUCCESS;
}

int npd_acl_show_standard_icmp_rule
(
	char **buffer,
	ACL_RULE_STDIPV4L4_T *stdIp,
	int *byteCnt
) {
	char *tmpPtr = NULL;
	int length = 0,totalLen = 0;
	unsigned long ipAddr = 0;
	unsigned char type =0, masktype = 0,maskcode = 0, code = 0;
	
	/*code optimize:  Dereference before null check
	zhangcl@autelan.com 2013-1-17*/
	if(!buffer) 
	{
		return ACL_RETURN_CODE_ERROR;
	}
	tmpPtr = *buffer;

	if(!tmpPtr) 
	{
		return ACL_RETURN_CODE_ERROR;
	}
	
	length = sprintf(tmpPtr,"icmp ");						 
	tmpPtr += length;
	totalLen += length;

	ipAddr = stdIp->rule.Dip;
	if(ipAddr != 0) {  
		length = sprintf(tmpPtr,"dip %ld.%ld.%ld.%ld/%ld ",(ipAddr>>24)&0xff,(ipAddr>>16)&0xff,	\
								(ipAddr>>8)&0xff,ipAddr&0xff,stdIp->mask.maskLenDip);					   
	}
	else {
		length = sprintf(tmpPtr,"dip any ");					   
	}
	tmpPtr += length;						
	totalLen += length;

	ipAddr = stdIp->rule.Sip;
	if(ipAddr != 0) {  
		length = sprintf(tmpPtr,"sip %ld.%ld.%ld.%ld/%ld ",(ipAddr>>24)&0xff,(ipAddr>>16)&0xff,	\
								(ipAddr>>8)&0xff,ipAddr&0xff,stdIp->mask.maskLenSip);					   
	}
	else {
		length = sprintf(tmpPtr,"sip any ");					   
	}
	tmpPtr += length;						
	totalLen += length;

	masktype = stdIp->mask.maskType;
	type = stdIp->rule.Type;
	if(0 != masktype) {
		length = sprintf(tmpPtr,"type %d ",type);								  
	}	
	else{
		length = sprintf(tmpPtr,"type any ");					   
	} 
	tmpPtr += length;						
	totalLen += length;

	maskcode = stdIp->mask.maskCode;
	code = stdIp->rule.Code;
	if(0 != maskcode) {
		length = sprintf(tmpPtr,"code %d ",code);											  
	}	
	else{
		length = sprintf(tmpPtr,"code any ");					   
	}	   
	tmpPtr += length;						
	totalLen += length;

	*byteCnt = totalLen;

	return ACL_RETURN_CODE_SUCCESS;
}

int npd_acl_show_standard_ethernet_rule
(
	char **buffer,
	ACL_RULE_STDNOTIP_T *stdNonIp,
	int *byteCnt
) {
	char *tmpPtr = NULL;
	int length = 0,totalLen = 0;
	unsigned char *mac = NULL;
	
	/*code optimize:  Dereference before null check
	zhangcl@autelan.com 2013-1-17*/
	if(!buffer) 
	{
		return ACL_RETURN_CODE_ERROR;
	}
	tmpPtr = *buffer;

	if(!tmpPtr) 
	{
		return ACL_RETURN_CODE_ERROR;
	}
	
	length = sprintf(tmpPtr,"ethernet ");						 
	tmpPtr += length;
	totalLen += length;

	mac = stdNonIp->rule.dMac;
	if(NPD_FALSE == npd_acl_check_maczero(mac)) {  
		length = sprintf(tmpPtr,"dmac %02x:%02x:%02x:%02x:%02x:%02x ",
								mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);					   
	}
	else {
		length = sprintf(tmpPtr,"dmac any ");					   
	}
	tmpPtr += length;						
	totalLen += length;
	
	mac = stdNonIp->rule.sMac;
	if(NPD_FALSE == npd_acl_check_maczero(mac)) {  
		length = sprintf(tmpPtr,"smac %02x:%02x:%02x:%02x:%02x:%02x ",
								mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);					   
	}
	else {
		length = sprintf(tmpPtr,"smac any ");					   
	}
	tmpPtr += length;						
	totalLen += length;

	*byteCnt = totalLen;

	return ACL_RETURN_CODE_SUCCESS;
}

int npd_acl_show_standard_arp_rule
(
	char **buffer,
	ACL_RULE_STDNOTIP_T *stdNonIp,
	int *byteCnt
) {
	char *tmpPtr = NULL;
	int length = 0,totalLen = 0;
	unsigned int retVal = 0;
	unsigned int vid = 0;
	unsigned char *mac = NULL;
	unsigned int sport = 0, sportmask = 0,modid = 0;
	unsigned int slot_no=0,port_no=0;
	/*enum product_id_e productId = PRODUCT_ID; */
	
	/*code optimize:  Dereference before null check
	zhangcl@autelan.com 2013-1-17*/
	if(!buffer) 
	{
		return ACL_RETURN_CODE_ERROR;
	}
	tmpPtr = *buffer;

	if(!tmpPtr) 
	{
		return ACL_RETURN_CODE_ERROR;
	}

	length = sprintf(tmpPtr,"arp ");						 
	tmpPtr += length;
	totalLen += length;

	mac = stdNonIp->rule.sMac;
	if(NPD_FALSE == npd_acl_check_maczero(mac)) {  
		length = sprintf(tmpPtr,"smac %02x:%02x:%02x:%02x:%02x:%02x ",
								mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);					   
	}
	else {
		length = sprintf(tmpPtr,"smac any ");					   
	}
	tmpPtr += length;						
	totalLen += length;
	
	vid = stdNonIp->rule.vlanId;
	if(0 != vid) {  
		length = sprintf(tmpPtr,"vid %d ",vid);					   
	}
	else {
		length = sprintf(tmpPtr,"vid any ");					   
	}
	tmpPtr += length;						
	totalLen += length;

	sport = stdNonIp->rule.portno;
	sportmask = stdNonIp->mask.maskPortno;
	modid = stdNonIp->rule.modid;
	if(0 != sportmask) {
		/*all device 0,if updata ,here should repair*/
		retVal=nam_get_slotport_by_devport(modid,sport,PRODUCT_ID,&slot_no,&port_no); 
		if(retVal!=0) {
			syslog_ax_acl_err("acl rule arp get slot port for source %d error!\n",sport);
			return (ACL_RETURN_CODE_ERROR-1);
		}
		length = sprintf(tmpPtr,"sourceport %d/%d ",slot_no,port_no);						 
	}
	else {
		length = sprintf(tmpPtr,"sourceport any ");	
	}
	tmpPtr += length;						
	totalLen += length;

	*byteCnt = totalLen;

	return ACL_RETURN_CODE_SUCCESS;
}

int npd_acl_show_extended_rule
(
	char **buffer,
	ACL_RULE_EXTNOTIPV6_T  *nodeExtNotIpv6,
	unsigned char L4type,
	int *byteCnt
) {
	char *tmpPtr = NULL;
	int length = 0,totalLen = 0;
	unsigned long ipAddr = 0,L4port = 0;
	unsigned int retVal = 0;
	unsigned int vid = 0;
	unsigned char *mac = NULL;
	unsigned int sport = 0, sportmask = 0, modid = 0;
	unsigned int slot_no=0,port_no=0;
	/*enum product_id_e productId = PRODUCT_ID;*/ 	
	/*code optimize:  Dereference before null check
	zhangcl@autelan.com 2013-1-17*/
	if(!buffer) 
	{
		return ACL_RETURN_CODE_ERROR;
	}
	tmpPtr = *buffer;

	if(!tmpPtr) 
	{
		return ACL_RETURN_CODE_ERROR;
	}

	if((L4type != 0) && (L4type != 1)) {
		return (ACL_RETURN_CODE_ERROR-1);
	}
	length = sprintf(tmpPtr,"%s ",L4type ? "udp":"tcp");						 
	tmpPtr += length;
	totalLen += length;

	ipAddr = nodeExtNotIpv6->rule.Dip;
	if(ipAddr != 0) {  
		length = sprintf(tmpPtr,"dip %ld.%ld.%ld.%ld/%ld ",(ipAddr>>24)&0xff,(ipAddr>>16)&0xff,	\
								(ipAddr>>8)&0xff,ipAddr&0xff,nodeExtNotIpv6->mask.maskLenDip);					   
	}
	else {
		length = sprintf(tmpPtr,"dip any ");					   
	}
	tmpPtr += length;						
	totalLen += length;

	L4port = nodeExtNotIpv6->rule.DstPort;
	if(0 != nodeExtNotIpv6->mask.maskDstPort){
		 length = sprintf(tmpPtr,"dst-port %ld ",nodeExtNotIpv6->rule.DstPort);						 
	}
	else {
		 length = sprintf(tmpPtr,"dst-port any ");						 
	}
	tmpPtr += length;							 
	totalLen += length;

	ipAddr = nodeExtNotIpv6->rule.Sip;
	if(ipAddr != 0) {  
		length = sprintf(tmpPtr,"sip %ld.%ld.%ld.%ld/%ld ",(ipAddr>>24)&0xff,(ipAddr>>16)&0xff,	\
								(ipAddr>>8)&0xff,ipAddr&0xff,nodeExtNotIpv6->mask.maskLenSip);					   
	}
	else {
		length = sprintf(tmpPtr,"sip any ");					   
	}
	tmpPtr += length;						
	totalLen += length;

	L4port = nodeExtNotIpv6->rule.SrcPort;
	if(0 != nodeExtNotIpv6->mask.maskSrcPort) {
		 length = sprintf(tmpPtr,"src-port %ld ",nodeExtNotIpv6->rule.SrcPort);						 
	}
	else {
		 length = sprintf(tmpPtr,"src-port any ");						 
	}
	tmpPtr += length;							 
	totalLen += length;

	mac = nodeExtNotIpv6->rule.dMac;
	if(NPD_FALSE == npd_acl_check_maczero(mac)) {  
		length = sprintf(tmpPtr,"dmac %02x:%02x:%02x:%02x:%02x:%02x ",
								mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);					   
	}
	else {
		length = sprintf(tmpPtr,"dmac any ");					   
	}
	tmpPtr += length;						
	totalLen += length;

	mac = nodeExtNotIpv6->rule.sMac;
	if(NPD_FALSE == npd_acl_check_maczero(mac)) {  
		length = sprintf(tmpPtr,"smac %02x:%02x:%02x:%02x:%02x:%02x ",
								mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);					   
	}
	else {
		length = sprintf(tmpPtr,"smac any ");					   
	}
	tmpPtr += length;						
	totalLen += length;
	
	vid = nodeExtNotIpv6->rule.vlanId;
	if(0 != vid) {  
		length = sprintf(tmpPtr,"vid %d ",vid);					   
	}
	else {
		length = sprintf(tmpPtr,"vid any ");					   
	}
	tmpPtr += length;						
	totalLen += length;

	sport = nodeExtNotIpv6->rule.portNo;
	sportmask = nodeExtNotIpv6->mask.maskPortNo;
	modid = nodeExtNotIpv6->rule.modid;
	if(0 != sportmask) {
		/*all device 0,if updata ,here should repair*/
		retVal=nam_get_slotport_by_devport(modid,sport,PRODUCT_ID,&slot_no,&port_no); 
		if(retVal!=0) {
			syslog_ax_acl_err("acl extend rule get slot port for source %d error!\n",sport);
			return (ACL_RETURN_CODE_ERROR-1);
		}
		length = sprintf(tmpPtr,"sourceport %d/%d ",slot_no,port_no);						 
	}
	else {
		length = sprintf(tmpPtr,"sourceport any ");	
	}
	tmpPtr += length;						
	totalLen += length;
	
	*byteCnt = totalLen;

	return ACL_RETURN_CODE_SUCCESS;
}

int npd_acl_show_extended_rule_ipv6
(
	char **buffer,
	AclRuleList *aclNodePtr,
	int *byteCnt
) {
	char *tmpPtr = NULL;
	int length = 0,totalLen = 0;
	unsigned int retVal = 0;
	ACL_RULE_EXTIPV6L4_T	*extIpv6 = &(aclNodePtr->TcamPtr.ruleInfo.extIpv6L4);
	char dbuf[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")];
	char sbuf[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")];

	/*code optimize:  Dereference before null check
	zhangcl@autelan.com 2013-1-17*/
	if(!buffer) 
	{
		return ACL_RETURN_CODE_ERROR;
	}
	tmpPtr = *buffer;

	if(!tmpPtr) 
	{
		return ACL_RETURN_CODE_ERROR;
	}

	if (ACL_TCAM_RULE_PACKETTYPE_TCPV6 == aclNodePtr->TcamPtr.packetType) {
		length = sprintf(tmpPtr,"%s ", (17 == aclNodePtr->TcamPtr.nextheader) ? "udp":"tcp");
	}
	else {
		length = sprintf(tmpPtr,"next-header %u ", aclNodePtr->TcamPtr.nextheader);
	}
	tmpPtr += length;
	totalLen += length;

	if(extIpv6->mask.maskLenDip) {
		/*
		length = sprintf(tmpPtr,"dipv6 %x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x ", 
							extIpv6->rule.Dip.ipbuf[0], extIpv6->rule.Dip.ipbuf[1], extIpv6->rule.Dip.ipbuf[2], extIpv6->rule.Dip.ipbuf[3], 
							extIpv6->rule.Dip.ipbuf[4], extIpv6->rule.Dip.ipbuf[5], extIpv6->rule.Dip.ipbuf[6], extIpv6->rule.Dip.ipbuf[7], 
							extIpv6->rule.Dip.ipbuf[8], extIpv6->rule.Dip.ipbuf[9], extIpv6->rule.Dip.ipbuf[10], extIpv6->rule.Dip.ipbuf[11], 
							extIpv6->rule.Dip.ipbuf[12], extIpv6->rule.Dip.ipbuf[13], extIpv6->rule.Dip.ipbuf[14], extIpv6->rule.Dip.ipbuf[15]);					   
	*/
		length = sprintf(tmpPtr,"dipv6 %s ", inet_ntop(AF_INET6, extIpv6->rule.Dip.ipbuf, dbuf, sizeof(dbuf)));

	}
	else {
		length = sprintf(tmpPtr,"dipv6 any ");					   
	}
	tmpPtr += length;						
	totalLen += length;

	if (ACL_TCAM_RULE_PACKETTYPE_TCPV6 == aclNodePtr->TcamPtr.packetType) {
		if(extIpv6->mask.maskDstPort){
			 length = sprintf(tmpPtr,"dst-port %ld ",extIpv6->rule.DstPort);						 
		}
		else {
			 length = sprintf(tmpPtr,"dst-port any ");						 
		}
		tmpPtr += length;							 
		totalLen += length;
	}

	if(extIpv6->mask.maskLenSip) {  
		length = sprintf(tmpPtr,"sipv6 %s ", inet_ntop(AF_INET6, extIpv6->rule.Sip.ipbuf, sbuf, sizeof(sbuf)));
	}
	else {
		length = sprintf(tmpPtr,"sipv6 any ");					   
	}
	tmpPtr += length;						
	totalLen += length;
	
	if (ACL_TCAM_RULE_PACKETTYPE_TCPV6 == aclNodePtr->TcamPtr.packetType) {
		if(extIpv6->mask.maskSrcPort) {
			 length = sprintf(tmpPtr,"src-port %ld ",extIpv6->rule.SrcPort);						 
		}
		else {
			 length = sprintf(tmpPtr,"src-port any ");						 
		}
		tmpPtr += length;							 
		totalLen += length;
	}
	
	*byteCnt = totalLen;

	return ACL_RETURN_CODE_SUCCESS;
}

int npd_acl_show_running_rule_one
(
	char *buffer,
	AclRuleList *aclNodePtr
)
{	
	unsigned int	packet=0,ruleT=0,action=0;
	char tmpBuf[2048]={0},*tmpPtr=NULL,*cursor = NULL;
	int byteCntA = 0,byteCnt = 0;
	unsigned int ruleIndex = 0;
	int retVal=0;
 	static int totalLen = 0;
	ACL_RULE_STDIPV4L4_T *stdIpv4L4 = NULL; 					  
	ACL_RULE_STDNOTIP_T *stdNotIp=NULL;
	ACL_RULE_EXTNOTIPV6_T  *nodeExtNotIpv6=NULL;
	unsigned char L4type = 0;
	unsigned int  policerEnable=0, policerid = 0;

	cursor = buffer;
	if(!buffer || !cursor) {
		return (ACL_RETURN_CODE_ERROR);	
	}
	
	tmpPtr 	= tmpBuf;  
	action 	= aclNodePtr->ActionPtr.actionType;
	packet	= aclNodePtr->TcamPtr.packetType;
	ruleT 	= aclNodePtr->ruleType;
	ruleIndex = aclNodePtr->ruleIndex;
	policerEnable =aclNodePtr->ActionPtr.policerPtr.policerEnable;
	policerid = aclNodePtr->ActionPtr.policerPtr.policerId;
	
	retVal = npd_acl_show_running_action(&tmpPtr,ruleT,ruleIndex+1,&(aclNodePtr->ActionPtr),&byteCntA);
	if(ACL_RETURN_CODE_SUCCESS != retVal) {
		syslog_ax_acl_err("show acl rule %d action error %d",ruleIndex,retVal);
		return (ACL_RETURN_CODE_ERROR - 1);
	}
	if(byteCntA >= 2048) {
		syslog_ax_acl_err("show acl rule %d action string size %d too long",byteCntA);
		return (ACL_RETURN_CODE_ERROR -2);
	}
	tmpPtr += byteCntA;
	totalLen += byteCntA;
	
	if ((ACL_TCAM_RULE_PACKETTYPE_UDP != packet)&&(ACL_TCAM_RULE_PACKETTYPE_TCP != packet)) {
		switch(packet) {
			case ACL_TCAM_RULE_PACKETTYPE_IP:
				stdIpv4L4 = &(aclNodePtr->TcamPtr.ruleInfo.stdIpv4L4);
				retVal = npd_acl_show_standard_ipv4_rule(&tmpPtr,stdIpv4L4,&byteCnt);
				break;
			case ACL_TCAM_RULE_PACKETTYPE_ICMP:
				stdIpv4L4 = &(aclNodePtr->TcamPtr.ruleInfo.stdIpv4L4);
				retVal = npd_acl_show_standard_icmp_rule(&tmpPtr,stdIpv4L4,&byteCnt);
				break;
			case ACL_TCAM_RULE_PACKETTYPE_ETHERNET:
				stdNotIp= &(aclNodePtr->TcamPtr.ruleInfo.stdNotIp);
				retVal = npd_acl_show_standard_ethernet_rule(&tmpPtr,stdNotIp,&byteCnt);
				break;
			case ACL_TCAM_RULE_PACKETTYPE_ARP:
				stdNotIp= &(aclNodePtr->TcamPtr.ruleInfo.stdNotIp);
				retVal = npd_acl_show_standard_arp_rule(&tmpPtr,stdNotIp,&byteCnt);
				break;
			case ACL_TCAM_RULE_PACKETTYPE_QOS:
				stdIpv4L4 = &(aclNodePtr->TcamPtr.ruleInfo.stdIpv4L4);
				retVal = npd_acl_show_standard_qos_rule(&tmpPtr,stdIpv4L4,&byteCnt);
				break;		
			case ACL_TCAM_RULE_PACKETTYPE_IPV6:
			case ACL_TCAM_RULE_PACKETTYPE_TCPV6:
				retVal = npd_acl_show_extended_rule_ipv6(&tmpPtr, aclNodePtr, &byteCnt);
				break;
			default:
				return (ACL_RETURN_CODE_ERROR-3);
		}
		
		if(ACL_RETURN_CODE_SUCCESS == retVal) {
			if((totalLen + byteCnt) < NPD_ACL_RULE_SHOWRUN_CFG_SIZE) {
				totalLen += sprintf(cursor,"%s",tmpBuf);
			}
		}
		else {
			return (ACL_RETURN_CODE_ERROR-4);
		}
	}
	else {
		/*code optimize: dead default in switch
		zhangcl@autelan.com 2013-1-17*/
		/*the packet type must be udp or tcp */
		if(STANDARD_ACL_RULE == ruleT) {
			#if 0
			switch(packet) {
				case ACL_TCAM_RULE_PACKETTYPE_UDP:
				case ACL_TCAM_RULE_PACKETTYPE_TCP:
				L4type = (ACL_TCAM_RULE_PACKETTYPE_UDP == packet) ? 1:0;
				stdIpv4L4 = &(aclNodePtr->TcamPtr.ruleInfo.stdIpv4L4);
				retVal = npd_acl_show_standard_ipv4_L4_rule(&tmpPtr,stdIpv4L4,packet,&byteCnt);
				break;
				default:
					break;
			}	
			#endif
			L4type = (ACL_TCAM_RULE_PACKETTYPE_UDP == packet) ? 1:0;
			stdIpv4L4 = &(aclNodePtr->TcamPtr.ruleInfo.stdIpv4L4);
			retVal = npd_acl_show_standard_ipv4_L4_rule(&tmpPtr,stdIpv4L4,packet,&byteCnt);
			if(ACL_RETURN_CODE_SUCCESS == retVal) 
			{
				if((totalLen + byteCnt) < NPD_ACL_RULE_SHOWRUN_CFG_SIZE) 
				{
					totalLen += sprintf(cursor,"%s",tmpBuf);
				}
			}
			else 
			{
				return (ACL_RETURN_CODE_ERROR-4);
			}
		}
		else if(EXTENDED_ACL_RULE == ruleT) {		
			L4type = (ACL_TCAM_RULE_PACKETTYPE_UDP == packet) ? 1:0;
			nodeExtNotIpv6= &(aclNodePtr->TcamPtr.ruleInfo.extNotIpv6);
			retVal = npd_acl_show_extended_rule(&tmpPtr,nodeExtNotIpv6,L4type,&byteCnt);
			if(ACL_RETURN_CODE_SUCCESS == retVal) {
				if((totalLen + byteCnt) < NPD_ACL_RULE_SHOWRUN_CFG_SIZE) {
					totalLen += sprintf(cursor,"%s",tmpBuf);
				}
			}
			else {
				return (ACL_RETURN_CODE_ERROR-7);
			}
		}
		else {
			return (ACL_RETURN_CODE_ERROR-6);
		}
	}
	tmpPtr += byteCnt;
	totalLen += byteCnt;
	
	/*policer*/
	if(1==policerEnable){
		/*
		retVal = npd_acl_show_policer_rule(&tmpPtr,&(aclNodePtr->ActionPtr),&byteCntP);
		if(ACL_RETURN_CODE_SUCCESS == retVal) {
			if((totalLen + byteCntP) < NPD_ACL_RULE_SHOWRUN_CFG_SIZE) {
				totalLen += sprintf(cursor,"%s",tmpBuf);
			}
		}
		else {
			return (ACL_RETURN_CODE_ERROR-5);
		}
		tmpPtr += byteCntP;
		totalLen += byteCntP;
		*/
		totalLen += sprintf(tmpPtr, "policer %d\n", policerid);
		totalLen += sprintf(cursor, "%s", tmpBuf);
	}
	else{
		strcat(tmpPtr,"\n");
		totalLen += sprintf(cursor,"%s",tmpBuf);		
	}
	return ACL_RETURN_CODE_SUCCESS;
}

DBusMessage *npd_dbus_acl_rule_show_running_config(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage* reply;
	DBusMessageIter  iter;

	char *showStr = NULL,*cursor = NULL;
	unsigned char enter_node_acl=0;	
	int i = 0;
	int totalLen = 0,retVal = ACL_RETURN_CODE_SUCCESS;
	AclRuleList 	*aclNodePtr=NULL;
	struct acl_range_rule	*rangeNodePtr = NULL;
    		
	showStr = (char*)malloc(NPD_ACL_RULE_SHOWRUN_CFG_SIZE);
	if(NULL == showStr) {
		syslog_ax_acl_err("memory malloc error\n");
		return NULL;
	}
	memset(showStr,0,NPD_ACL_RULE_SHOWRUN_CFG_SIZE);
	cursor = showStr;

    /* add for distributed acl, zhangdi@autelan.com 2011-11-23 */
	/* into acl config node */
	totalLen += sprintf(cursor,"config acl switch-board %d\n",SLOT_ID);                       
	cursor = showStr + totalLen;
	enter_node_acl = 1;
	
    /*global acl service*/
    if(g_acl_enable == ACL_TRUE){
    	totalLen += sprintf(cursor," config acl service enable\n");
    	cursor = showStr + totalLen;			
    }
    /* acl rule	*/
  	if(r_acl_rule) {
		for(i=0;i<MAX_ACL_RULE_NUMBER;i++){
			rangeNodePtr = r_acl_rule[i];
			
			if(NULL == rangeNodePtr) {
				continue;
			}
			else {
				totalLen += sprintf(cursor, " acl-range standard %d %d ip-range destination ", rangeNodePtr->startIndex + 1, rangeNodePtr->endIndex + 1);
				cursor = showStr + totalLen;
				
				if (rangeNodePtr->startDip) {
					totalLen += sprintf(cursor, "%d.%d.%d.%d %d.%d.%d.%d source ",
					(rangeNodePtr->startDip>>24) & 0xFF, (rangeNodePtr->startDip>>16) & 0xFF,(rangeNodePtr->startDip>>8) & 0xFF,(rangeNodePtr->startDip & 0xFF),
					(rangeNodePtr->endDip>>24) & 0xFF, (rangeNodePtr->endDip>>16) & 0xFF,(rangeNodePtr->endDip>>8) & 0xFF,rangeNodePtr->endDip & 0xFF);	
					cursor = showStr + totalLen;
				}
				else {
					totalLen += sprintf(cursor, "any source ");
					cursor = showStr + totalLen;
				}
			
				if (rangeNodePtr->startSip) {
					totalLen += sprintf(cursor, "%d.%d.%d.%d %d.%d.%d.%d ",
					(rangeNodePtr->startSip>>24) & 0xFF, (rangeNodePtr->startSip>>16) & 0xFF,(rangeNodePtr->startSip>>8) & 0xFF,(rangeNodePtr->startSip & 0xFF),
					(rangeNodePtr->endSip>>24) & 0xFF, (rangeNodePtr->endSip>>16) & 0xFF,(rangeNodePtr->endSip>>8) & 0xFF,rangeNodePtr->endSip & 0xFF);	
					cursor = showStr + totalLen;
				}
				else {
					totalLen += sprintf(cursor, "any ");
					cursor = showStr + totalLen;
				}
				
				if (1 == rangeNodePtr->policer) {
					totalLen += sprintf(cursor, "policer %d\n", rangeNodePtr->startPid);
					cursor = showStr + totalLen;
				}
				else if (2 == rangeNodePtr->policer) {
					totalLen += sprintf(cursor, "policer-range %d %d\n", rangeNodePtr->startPid, rangeNodePtr->endPid);
					cursor = showStr + totalLen;
				}
				else {
					totalLen += sprintf(cursor, "\n");
					cursor = showStr + totalLen;				
				}
			}	  
		} 
  	}

	if(g_acl_rule) {
		for(i=0;i<MAX_ACL_RULE_NUMBER;i++){
			aclNodePtr = g_acl_rule[i];			
			char tmpBuf[2048]={0},*tmpPtr=tmpBuf;
			
			if(NULL == aclNodePtr) {
				continue;
			}
			else {
				if (1 == aclNodePtr->rangerule) {
					continue;
				}
				retVal = npd_acl_show_running_rule_one(tmpPtr,aclNodePtr);
				if(ACL_RETURN_CODE_SUCCESS != retVal) {
					syslog_ax_acl_err("show running config rule index %d at %d error",aclNodePtr->ruleIndex,i);
					break;
				}
				totalLen += sprintf(cursor,"%s\n",tmpBuf);
				cursor = showStr + totalLen;/* move ahead with totalLen bytes*/
			}	  
		} /*for */
	}

    /* exit acl config node */
	if(enter_node_acl) {
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

DBusMessage * npd_dbus_acl_qos_show_running_config
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
    DBusMessage* reply;
	DBusMessageIter  iter;

   QosAppendProfile     *qosappend = NULL;

	char *showStr = NULL,*cursor = NULL;
	int i = 0,totalLen = 0;
	unsigned char enter_node_acl=0;
	                   
	showStr = (char*)malloc(NPD_QOS_SHOWRUN_CFG_SIZE);
	if(NULL == showStr) {
		npd_syslog_err(("memory malloc error\n"));
		return NULL;
	}
	memset(showStr,0,NPD_QOS_SHOWRUN_CFG_SIZE);
	cursor = showStr;
	
   /* add for distributed acl, wangchong@autelan.com 2012-05-17 */
	totalLen += sprintf(cursor,"config acl switch-board %d\n",SLOT_ID);                       
	cursor = showStr + totalLen;
	enter_node_acl = 1;

	if(qos_append) {
		for(i = 0;i < MAX_ACL_EXT_RULE_NUMBER; i++) {
			qosappend = qos_append[i];
			if(NULL == qosappend) {
				continue;
			}
			else {            
				totalLen += sprintf(cursor,"append acl %d ingress-qos %d\n", (i + 1), qos_append[i]->profileIndex);                       
				cursor = showStr + totalLen;   			
			}/*if*/	                                                             
		}/*for*/
	}
	if(enter_node_acl) {
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
DBusMessage * npd_dbus_acl_group_show_running_config
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	DBusMessage* reply;
	DBusMessageIter  iter;

	struct acl_rule_info_s         *ruleInfo = NULL;
	struct list_head			   *ptr = NULL;
	struct acl_group_list_s	       *group = NULL;   

	char *showStr = NULL,*cursor = NULL;
	int i = 0, k = 0,j=0,totalLen = 0,index[1024], startrule = 0, endrule = 0, indexcount = 0, rangecount = 0;
	unsigned char enter_node=0;
	unsigned char enter_node_acl=0;
	unsigned int aclGroupNo = 0;
	 	                      
	showStr = (char*)malloc(NPD_ACL_RULE_SHOWRUN_CFG_SIZE);
	if(NULL == showStr) {
		syslog_ax_acl_err("memory malloc error\n");
		return NULL;
	}
	memset(showStr,0,NPD_ACL_RULE_SHOWRUN_CFG_SIZE);
	cursor = showStr;

	/* into acl config node */
	totalLen += sprintf(cursor,"config acl switch-board %d\n",SLOT_ID);                       
	cursor = showStr + totalLen;
	enter_node_acl = 1;
	
	/* acl group */
	for(k=0;k<MAX_GROUP_NUM;k++) {
		group = acl_group[k];
		enter_node = 0;
		memset(index, 0, sizeof(int)*1024);
		indexcount = 0;
		endrule = 0;
		rangecount = 0;
		i = 0;
		j = 0;
		if(NULL == group) {
			continue;
		}
		else {
			totalLen += sprintf(cursor," create ingress acl-group %d\n",k);                       
			cursor = showStr + totalLen;   

			__list_for_each(ptr,&(group->list1)) {
				ruleInfo = list_entry(ptr,struct acl_rule_info_s,list);	
				if(NULL != ruleInfo) {
					if(0 == enter_node) { /* acl-group node not entered before*/
						totalLen += sprintf(cursor," config ingress acl-group %d\n",k);
						cursor = showStr + totalLen;
						enter_node = 1;
					}
					index[indexcount] = ruleInfo->ruleIndex;
					indexcount++;	
					/*
					totalLen += sprintf(cursor,"  add acl %ld\n",(ruleInfo->ruleIndex)+1);
					cursor = showStr + totalLen;
					*/
				}		
			}
			j = 0;
			while(j <= indexcount) {
				if (i == index[j]) {
					endrule = index[j];
					rangecount++;
					i++;
					j++;
				}
				else {
					if (1 == rangecount) {
						totalLen += sprintf(cursor,"  add acl %d\n", endrule+1);
						cursor = showStr + totalLen;
					}
					else if (rangecount > 1) {
						totalLen += sprintf(cursor,"  add acl-range %d %d\n", endrule-rangecount+2, endrule+1);
						cursor = showStr + totalLen;
					}
					if (j == indexcount) {
						j++;
					}
					i++;
					rangecount = 0;
				}
			}
			if(enter_node) {
				totalLen += sprintf(cursor,"  exit\n");
				cursor = showStr + totalLen;
			}
		}/*if	 */                                                            
	}/*for*/

	for(j=0;j<MAX_GROUP_NUM;j++) {
		group = egress_acl_group[j];
		enter_node = 0;
		if(NULL == group) {
			continue;
		}
		else {
			totalLen += sprintf(cursor," create egress acl-group %d\n",j);                       
			cursor = showStr + totalLen;   

			__list_for_each(ptr,&(group->list1)) {
				ruleInfo = list_entry(ptr,struct acl_rule_info_s,list);	
				if(NULL != ruleInfo) {
					if(0 == enter_node) { /* egress acl-group node not entered before*/
						totalLen += sprintf(cursor," config egress acl-group %d\n",j);
						cursor = showStr + totalLen;
						enter_node = 1;
					}
					totalLen += sprintf(cursor,"  add acl %ld\n",(ruleInfo->ruleIndex)+1);
					cursor = showStr + totalLen;
				}			  
			}  
			if(enter_node) {
				totalLen += sprintf(cursor,"  exit\n");
				cursor = showStr + totalLen;
			}
		}/*if	   */                                                          
	}/*for*/
	
    /* exit acl config node */
	if(enter_node_acl) {
		totalLen += sprintf(cursor," exit\n");
		cursor = showStr + totalLen;
	}

	/* if the configuration of acl-group bound by a port is set in ETH PORT section,
	    the message "acl-group is not exist" will alarm while loading the configuration 
 	    in reboot,so move the configuration to ACL section.*/
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
        syslog_ax_acl_dbg("*************** eth-port %d/%d\n",slot_index+1,local_port_no);		
	    syslog_ax_acl_dbg("*************** eth_g_index %d\n",eth_g_index);

		/* ingress port ACL config*/
		if(NPD_TRUE == npd_eth_port_acl_bind_check(eth_g_index,ACL_DIRECTION_INGRESS))
		{
			enter_node = 1;
			length += sprintf(tmpPtr," ingress acl enable\n");
			tmpPtr = tmpBuf + length;

			/* NPD_DBUS_SUCCESS + 7 means ACL_GROUP_PORT_BINDED*/
			if((ACL_RETURN_CODE_BASE + 7) == npd_port_bind_group_check(eth_g_index,&aclGroupNo,ACL_DIRECTION_INGRESS))
			{
				length += sprintf(tmpPtr," bind ingress acl-group %d\n",aclGroupNo);
				tmpPtr = tmpBuf + length;
			}
		}

		/*egress port ACL config*/
		if(NPD_TRUE == npd_eth_port_acl_bind_check(eth_g_index,ACL_DIRECTION_EGRESS)) 
		{
			enter_node = 1;
			length += sprintf(tmpPtr," egress acl enable\n");
			tmpPtr = tmpBuf + length;

			/* NPD_DBUS_SUCCESS + 7 means ACL_GROUP_PORT_BINDED*/
			if((ACL_RETURN_CODE_BASE + 7) == npd_port_bind_group_check(eth_g_index,&aclGroupNo,ACL_DIRECTION_EGRESS)) 
			{
				length += sprintf(tmpPtr," bind egress acl-group %d\n",aclGroupNo);
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
	/* end of acl-group bind, zhangdi 2011-11-24 */
		
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

