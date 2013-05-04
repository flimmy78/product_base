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
* nam_acl.c
*
*
* CREATOR:
*		qinhs@autelan.com
*
* DESCRIPTION:
*		APIs used in NAM for ACL module driver process.
*
* DATE:
*		02/21/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.66 $	
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
#include "npd/nam/npd_amapi.h"
#include "npd/nbm/npd_bmapi.h"
#include "nam_acl.h"
#include "nam_qos.h"
#include "dbus/npd/npd_dbus_def.h"
#include "nam_log.h"
#include "sysdef/returncode.h"

extern AclRuleList **g_acl_rule;
extern QOS_PROFILE_STC 		   **g_qos_profile;
extern QOS_POLICER_STC		    **g_policer;
extern struct eth_port_s **g_eth_ports;
extern struct acl_group_list_s *acl_group[MAX_GROUP_NUM];
extern struct acl_group_list_s *egress_acl_group[MAX_GROUP_NUM];
extern QOS_COUNTER_STC				**g_counter;

extern struct vlan_s	**g_vlans;

#ifdef DRV_LIB_CPSS
#include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwTables.h>
#ifdef DRV_LIB_CPSS_3_4
#include <cpss/dxCh/dxCh3/policer/cpssDxCh3Policer.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#else
#include <cpss/dxCh/dxChxGen/policer/cpssDxChPolicer.h>
#endif
#endif

#ifdef DRV_LIB_BCM
#include <sal/core/libc.h>
#include <bcm/field.h>
#include <bcm/port.h>
bcm_field_qset_t		*qset = NULL;
#endif

/* luoxun@autelan.com 2011-1-27 */
#ifdef DRV_LIB_CPSS_3_4
#define cpssDxChPclRuleSet(a1,a2,a3,a4,a5,a6) \
        cpssDxChPclRuleSet(a1,a2,a3,0,a4,a5,a6)
#endif


extern unsigned long nam_asic_get_instance_num
(
	void
);

/**********************************************************************************
 * nam_bcm_acl_support_check
 *
 * check broadcom or marvell chip
 *
 *	INPUT:
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_BCM_DEVICE_NOT_SUPPORT 	- if broadcom chip
 *		ACL_RETURN_CODE_SUCCESS  		- if marvell chip
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int nam_bcm_acl_support_check
(
)
{
#ifdef DRV_LIB_BCM
	return ACL_RETURN_CODE_BCM_DEVICE_NOT_SUPPORT;
#endif

#ifdef DRV_LIB_CPSS
	return ACL_RETURN_CODE_SUCCESS;
#endif
}

/**********************************************************************************
 * nam_bcm_acl_drv_tcam_set
 *
 * set tcam for broadcom, if chip is marvell ret is success
 *
 *	INPUT:
 *		unsigned int group_num	- group number
 *		int						- dev number
 *		unsigned int *pbmp		- ports bit map
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_SUCCESS 	- if marvell chip return success or set tcam success 
 *		ACL_RETURN_CODE_ERROR		- set tcam fail
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int nam_bcm_acl_drv_tcam_set
( 
	unsigned int group_num,
	int unit,
	unsigned int *pbmp
)
{
	unsigned int ret = ACL_RETURN_CODE_SUCCESS;
	
#ifdef DRV_LIB_CPSS
	ret = ACL_RETURN_CODE_SUCCESS;
#endif
	
#ifdef DRV_LIB_BCM
	ACL_TCAM_RULE_FORMAT_E		ruleFormat;  		
	struct acl_rule_info_s		 *ruleInfo = NULL;
	struct list_head			*Ptr = NULL;
	unsigned int 			aclId = 0, add_index = 0;
	unsigned int  	op_ret = 0, dir_info;
	struct acl_group_list_s	*groupPtr = NULL;
	bcm_pbmp_t pbmptmp;
	
	unsigned char 	devIdx;
	unsigned int	appDemoDevAmount;
	
	groupPtr = acl_group[group_num];
	aclId = group_num;

	appDemoDevAmount = nam_asic_get_instance_num(); 
	
	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++) {
		if (!((unit^ACL_DEFAULT_DEVID)&(1<<devIdx))) {
			continue;
		}
		memset(&pbmptmp, 0, sizeof(bcm_pbmp_t));
		memcpy(&pbmptmp, &pbmp[devIdx], sizeof(unsigned int));

		ret = bcm_field_group_ports_create_mode_id(devIdx, pbmptmp, *qset, 1, bcmFieldGroupModeAuto, aclId);
		 if(ACL_RETURN_CODE_SUCCESS!=ret){
			nam_syslog_dbg("ERROR bcm_field_group_ports_create_mode_id ret %d, unit %d , portbitmap %d aclId %d \n",
				ret, devIdx, pbmptmp, aclId);
		 }	
		nam_syslog_dbg("create group %d, binded by pbmptmp %u \n", aclId, pbmptmp);
		
		 __list_for_each(Ptr, &(groupPtr->list1)) {
			ruleInfo = list_entry(Ptr,struct acl_rule_info_s,list);
			if (NULL != ruleInfo) {	
				add_index = ruleInfo->ruleIndex;
				nam_syslog_dbg("add entry %d\n", add_index);
				if (NULL != g_acl_rule[add_index]) {
					ruleFormat = g_acl_rule[add_index]->TcamPtr.ruleFormat;
					if (ACL_RULE_STD_IPv4_L4 == ruleFormat) {
						nam_syslog_dbg("ACL_RULE_STD_IPv4_L4\n");
						ret = nam_drv_acl_stdIpv4L4_ListDeal(g_acl_rule[add_index], aclId, devIdx, dir_info);
						if (ACL_RETURN_CODE_SUCCESS != ret) {   
							nam_syslog_dbg("npd_dbus_add_rule_to_acl_group>>npd_drv_acl_ListDeal ERROR!");
						}	 
					} /*ipv4L4*/
					else if (ACL_RULE_EXT_IPv4_L4 == ruleFormat) {
						nam_syslog_dbg("ACL_RULE_EXT_IPv4_L4\n");
						ret = nam_drv_acl_extIpv4L4_ListDeal(g_acl_rule[add_index], aclId, devIdx, dir_info);
						if (ACL_RETURN_CODE_SUCCESS != ret) {   
							nam_syslog_dbg("npd_dbus_add_rule_to_acl_group>>npd_drv_acl_ListDeal ERROR!");
						}	 
					} /*extipv4L4*/
					else if (ACL_RULE_STD_NOT_IP == ruleFormat) {
						nam_syslog_dbg("ACL_RULE_STD_NOT_IP\n");
						ret=nam_drv_acl_stdNotIp_ListDeal(g_acl_rule[add_index], aclId, devIdx, dir_info);
						if (ACL_RETURN_CODE_SUCCESS != ret) {  
							nam_syslog_dbg("npd_dbus_add_rule_to_acl_group>>npd_drv_acl_ListDeal ERROR!");
						}	  
					} /*notIp	*/		
					else if (ACL_RULE_EXT_NOT_IPv6 == ruleFormat) {
						nam_syslog_dbg("ACL_RULE_EXT_NOT_IPv6\n");
						ret = nam_drv_acl_extNotIpv6_ListDeal(g_acl_rule[add_index], aclId, devIdx, dir_info);
						if (ACL_RETURN_CODE_SUCCESS != ret) {  
							nam_syslog_dbg("npd_dbus_add_rule_to_acl_group>>npd_drv_acl_ListDeal ERROR!");		  
						}	  
					} /*extNotIpv6*/
					else if(ACL_RULE_STD_IP_L2_QoS== ruleFormat) {
						nam_syslog_dbg("ACL_RULE_STD_IP_L2_QoS\n");
						ret = nam_drv_acl_arp_listDeal(g_acl_rule[add_index], aclId, devIdx, dir_info);
						if (ACL_RETURN_CODE_SUCCESS != ret) {  
							nam_syslog_dbg("npd_dbus_add_rule_to_acl_group>>npd_drv_arp_acl_ListDeal ERROR!");
						}	  
					}/*arp*/
					else if (ACL_RULE_QOS == ruleFormat) {
						nam_syslog_dbg("ACL_RULE_QOS\n");
						ret = nam_drv_acl_qos_listDeal(g_acl_rule[add_index], aclId,  devIdx, dir_info);
						if (ACL_RETURN_CODE_SUCCESS != ret) {  
							nam_syslog_dbg("npd_dbus_add_rule_to_acl_group>>nam_drv_acl_qos_listDeal ERROR!");
						}	  
					}
				}
			}
		}  /*list_for_each	*/
	}
	if (ret) {
		ret = ACL_RETURN_CODE_ERROR;
	}
#endif

	return ret;
}

/**********************************************************************************
 * nam_bcm_acl_drv_tcam_clear
 *
 * clear tcam for broadcom, if marvell chip ret is success
 *
 *	INPUT:
 *		unsigned int group_num	- group number
 *		int						- dev number
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_SUCCESS 	- if marvell chip return success or clear tcam success 
 *		ACL_RETURN_CODE_ERROR		- clear tcam fail
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int nam_bcm_acl_drv_tcam_clear
( 
	unsigned int group_num, 
	int unit
)
{
	unsigned int ret = ACL_RETURN_CODE_SUCCESS;
	
#ifdef DRV_LIB_CPSS
	ret = ACL_RETURN_CODE_SUCCESS;
#endif

#ifdef DRV_LIB_BCM
	struct acl_rule_info_s			    *ruleInfo = NULL;
	struct list_head					*Ptr = NULL;
	unsigned int 						delete_index = 0;
	struct acl_group_list_s	*groupPtr = NULL;

	unsigned char 	devIdx = 0;
	unsigned int	appDemoDevAmount = 0;
	
	groupPtr = acl_group[group_num];

	appDemoDevAmount = nam_asic_get_instance_num(); 
	
	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++) {
		if (!((unit^ACL_DEFAULT_DEVID)&(1<<devIdx))) {
			continue;
		}
		 __list_for_each(Ptr, &(groupPtr->list1)) {
			ruleInfo=list_entry(Ptr,struct acl_rule_info_s,list);
			if(NULL != ruleInfo) {
				delete_index = ruleInfo->ruleIndex;
				ret = bcm_field_counter_destroy(devIdx, delete_index);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_counter_destroy \n");
				}
				ret = bcm_field_meter_destroy(devIdx, delete_index);
				if(ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("bcm_field_meter_destroy error!result %d",ret);	
				}
				ret = bcm_field_entry_remove(devIdx, delete_index);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_entry_remove \n");
				}
				ret = bcm_field_entry_destroy(devIdx, delete_index);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_entry_destroy \n");
				}
				nam_syslog_dbg("remove entry %d\n", delete_index);
				nam_syslog_dbg("destroy entry %d\n", delete_index);
			}
		}  /*list_for_each	*/

		ret = bcm_field_group_destroy(devIdx, group_num);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_group_destroy \n");
		}
	}
	if (ret) {
		ret = ACL_RETURN_CODE_ERROR;
	}	
	nam_syslog_dbg("destroy group %d\n", group_num);
#endif
	
	return ret;
}

/**********************************************************************************
 * nam_acl_service_init
 *
 * init acl service for broadcom
 *
 *	INPUT:
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_SUCCESS 	- acl init success 
 *		ACL_RETURN_CODE_ERROR		- acl init fail
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int nam_acl_service_init
(
)
{	
	unsigned int ret = ACL_RETURN_CODE_SUCCESS;
	
#ifdef DRV_LIB_CPSS
	ret = ACL_RETURN_CODE_SUCCESS;
#endif

#ifdef DRV_LIB_BCM
	/*BCM_FIELD_QSET_INIT(*qset);*/	
	qset = (bcm_field_qset_t*)malloc(sizeof(bcm_field_qset_t));
	if(NULL == qset) {
		nam_syslog_dbg("memory alloc error  init!! \n");
		return ACL_RETURN_CODE_ERROR;
	}
	memset(qset, 0, sizeof(bcm_field_qset_t));
	BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyEtherType);
	BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyIpProtocol);
	BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifySrcMac);
	BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyDstMac);
	BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifySrcIp);
	BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyDstIp);
	BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyInPort);
	BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyOuterVlan);
	BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyL4SrcPort);
	BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyL4DstPort);
	BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyDSCP); 	
	nam_syslog_dbg("nam_acl_service_init for qset  \n");
#endif

	return ret;
}

/**********************************************************************************
 * nam_acl_service
 *
 * init acl service for marvell chip
 *
 *	INPUT:
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_SUCCESS 	- acl init success 
 *		ACL_RETURN_CODE_ERROR		- acl init fail
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int nam_acl_service
(
	unsigned char enable
)
{	
	unsigned int ret = ACL_RETURN_CODE_SUCCESS;
#ifdef DRV_LIB_BCM
	ret = ACL_RETURN_CODE_SUCCESS;
#endif
	
#ifdef DRV_LIB_CPSS
	unsigned char 	devIdx = 0;
	unsigned int	appDemoDevAmount = 0;

	appDemoDevAmount = nam_asic_get_instance_num(); 
	
	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++)
	{							
		/*IngressPolicyEnable*/
		ret=cpssDxChPclIngressPolicyEnable(devIdx,enable);
		if(ret!=0) {
			nam_syslog_dbg("IngressPolicyEnable dev %d ret %x Error!",devIdx,ret);
		}
		/*EgressPolicyEnable*/
		ret=cpssDxCh2PclEgressPolicyEnable(devIdx,enable);
		if(ret!=0) {
			nam_syslog_dbg(":EgressPolicyEnable dev %d ret %x Error!",devIdx,ret);
		}
	}
#endif

	return ret;	
}
#if 0
unsigned int nam_acl_service_get(unsigned long *enable)
{
	unsigned char	devIdx;
	unsigned int	ret,appDemoDevAmount;

	appDemoDevAmount = nam_asic_get_instance_num(); 

	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++)
	{							
		/*IngressPolicyEnable*/
		ret=cpssDxChPclIngressPolicyEnableGet(devIdx,enable);
		if(ret!=0)
			nam_syslog_dbg("IngressPolicyEnable dev %d ret %x Error!",devIdx,ret);
		/*EgressPolicyEnable*/
			}
	return ret;

}
#endif

/**********************************************************************************
 * nam_acl_port_acl_enable
 *
 * set tcam for broadcom, if chip is marvell ret is success
 *
 *	INPUT:
 *		unsigned char devNum		- dev number
 *		unsigned char portId		- port number
 *		unsigned char acl_enable	- enable or disable
 *		unsigned int  dir_info		- ingress or egress
 *		int			  serviceenable	- enable or disable
 *
 *	OUTPUT:
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_SUCCESS 	- if marvell chip return success or set tcam success 
 *		ACL_RETURN_CODE_ERROR		- set tcam fail
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int nam_acl_port_acl_enable
(
	unsigned char devNum, 
	unsigned char portId,
	unsigned char acl_enable,
	unsigned int  dir_info,
	int serviceenable
) 
{

	nam_syslog_dbg("Enable ACL Service on special Port\n");
	unsigned int	ret = ACL_RETURN_CODE_SUCCESS;
#ifdef DRV_LIB_CPSS
	switch(dir_info){
		case ACL_DIRECTION_INGRESS:
			ret = cpssDxChPclPortIngressPolicyEnable(devNum,(char)portId,acl_enable);				
			if(ret!= 0) {
				nam_syslog_dbg("Acl service set on port ingress Error.\n");
				return ACL_RETURN_CODE_ERROR;
			}	
			nam_syslog_dbg("Acl service set on port Ok:devNum %d portId %d\n",devNum,portId);			
			break;
		case ACL_DIRECTION_EGRESS:
			/*pktType = CPSS_DXCH_PCL_EGRESS_PKT_NON_TS_E;*/
			ret = cpssDxCh2EgressPclPacketTypesSet(devNum,(char)portId,CPSS_DXCH_PCL_EGRESS_PKT_NON_TS_E,acl_enable);
			if(ret!= 0) {
				nam_syslog_dbg("CPSS_DXCH_PCL_EGRESS_PKT_NON_TS_E error!ret %d\n",ret);
				return ACL_RETURN_CODE_ERROR;
			}	
			nam_syslog_dbg("CPSS_DXCH_PCL_EGRESS_PKT_NON_TS_E ok\n");
			/*
			//a bit per egress port to enable/disable egress policy on data packets form cpu
			//e.g. packet received with FROM_CPU DSA tag add DSA<egress-filteren> = 0
			ret = cpssDxCh2EgressPclPacketTypesSet(devNum,(char)portId,CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E,acl_enable);
			if(ret!= 0) {
				nam_syslog_dbg("CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E error!ret %d\n",ret);
				return ACL_RETURN_CODE_ERROR;
			}	
			nam_syslog_dbg("CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E ok\n");
			//a bit per egress port to enable/disable egress policy on data packets form cpu
			//e.g. packet received with FROM_CPU DSA tag add DSA<egress-filteren> = 1
			ret = cpssDxCh2EgressPclPacketTypesSet(devNum,(char)portId,CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_DATA_E,acl_enable);
			if(ret!= 0) {
				nam_syslog_dbg("CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_DATA_E error!ret %d\n",ret);
				return ACL_RETURN_CODE_ERROR;
			}	
			*/
			nam_syslog_dbg("CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_DATA_E ok\n");
		default :break;
	}
#endif

#ifdef DRV_LIB_BCM
	if (serviceenable) {
		ret = bcm_port_control_set((int)devNum, portId, bcmPortControlFilterIngress, (int)acl_enable);
		if(ret!= 0) {
			nam_syslog_dbg("ERROR ! bcm_port_control_set \n");
			return ACL_RETURN_CODE_ERROR;
		}
	}
	nam_syslog_dbg("bcm_port_control_set serviceenable %d dir_info is  %d acl enable is %d \n", 
		serviceenable, dir_info, acl_enable);
#endif
	return ret;
}  

unsigned int nam_bcm_acl_port_acl_enable
(
	unsigned char devNum, 
	unsigned char portId,
	unsigned char acl_enable,
	unsigned int  dir_info,
	int serviceenable
) 
{

	/*nam_syslog_dbg("Enable ACL Service on special Port\n");*/
	unsigned int	ret = ACL_RETURN_CODE_SUCCESS;
#ifdef DRV_LIB_CPSS

#endif

#ifdef DRV_LIB_BCM
	int vlaue = 0;
	ret = bcm_port_control_get((int)devNum, portId, bcmPortControlFilterIngress, &vlaue);
	if(ret!= 0) {
		/*nam_syslog_dbg("ERROR ! bcm_port_control_set \n");*/
		return ACL_RETURN_CODE_ERROR;
	}
	/*nam_syslog_dbg("bcm_port_control_get vlaue is %d \n", vlaue);*/
	
	if (dir_info) {
			ret = bcm_port_control_set((int)devNum, portId, bcmPortControlFilterIngress, (int)acl_enable);
			if(ret!= 0) {
		/*		nam_syslog_dbg("ERROR ! bcm_port_control_set \n");*/
				return ACL_RETURN_CODE_ERROR;
			}
	}
/*	nam_syslog_dbg("bcm_port_control_set serviceenable %d dir_info is  %d acl enable is %d \n", */
/*		serviceenable, dir_info, acl_enable);*/
#endif

	return ret;
}  

/**********************************************************************************
 * nam_acl_drv_cfg_table_set
 *
 * 
 *
 *	INPUT:
 *		unsigned char  devNum		- dev number
 *		unsigned char  portId		- port number
 *		unsigned int   aclId		- acl index
 *		unsigned int   flag			- enable or disable lookup
 *		unsigned int   access_type	- port or vlan
 *		unsigned short vlanId 		- vlan index
 *		unsigned int   dir_info 	- ingress or egress
 *		unsigned int   stdrule		- standard or extend rule
 *
 *	OUTPUT:
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_SUCCESS 	- if marvell chip return success or set tcam success 
 *		ACL_RETURN_CODE_ERROR		- set tcam fail
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int nam_acl_drv_cfg_table_set
(
	unsigned char devNum,
	unsigned char portId,
	unsigned int  aclId,
	unsigned int  flag,
	unsigned int access_type,
	unsigned short vlanId,
	unsigned int  dir_info,
	unsigned int stdrule
)
{
	unsigned int op_ret = ACL_RETURN_CODE_SUCCESS;
#ifdef DRV_LIB_BCM
	op_ret = ACL_RETURN_CODE_SUCCESS;
#endif

#ifdef DRV_LIB_CPSS
	CPSS_INTERFACE_INFO_STC 		interfaceInfoPtr;
	CPSS_PCL_DIRECTION_ENT			direction = CPSS_PCL_DIRECTION_INGRESS_E;
	CPSS_PCL_LOOKUP_NUMBER_ENT		lookupNum0;
	CPSS_DXCH_PCL_LOOKUP_CFG_STC	lookupCfgPtr0;
	CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT	  mode;
	unsigned long					result = 0;
	unsigned int					hwValue = 0,entryIndex = 0;
	/*code optimize:  Uninitialized scalar variable 
	zhangcl@autelan.com 2013-1-17*/
	PRV_CPSS_DXCH_TABLE_ENT 		tableId = PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E;

	if(dir_info==ACL_DIRECTION_INGRESS){
    	direction = CPSS_PCL_DIRECTION_INGRESS_E;
		tableId = PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E;
	}
	else if(dir_info==ACL_DIRECTION_EGRESS){
		direction = CPSS_PCL_DIRECTION_EGRESS_E;
		tableId = PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E;
	}
	 if (ACCESS_PORT_TYPE==access_type){

		interfaceInfoPtr.type = CPSS_INTERFACE_PORT_E; 
		interfaceInfoPtr.devPort.portNum =(char)portId;
		entryIndex = 4096 + interfaceInfoPtr.devPort.portNum;
		mode = CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E;
       }
	 else if (ACCESS_VID_TYPE==access_type){
		/*wenti zai zheli*/
		interfaceInfoPtr.type = CPSS_INTERFACE_VID_E; 
		interfaceInfoPtr.vlanId = vlanId;
		entryIndex = interfaceInfoPtr.vlanId;	
		mode = CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E;
      }
	else {
		nam_syslog_dbg("unknow access method!\n");
		return ACL_RETURN_CODE_ERROR;			
	}
	  
	/*cfg table*/   
	/*lookup0*/	
	lookupNum0 = CPSS_PCL_LOOKUP_0_E; 		
	lookupCfgPtr0.enableLookup = (flag == ACL_TRUE) ? 1 : 0;
	lookupCfgPtr0.pclId = aclId;
	if(stdrule == 0) {
		if (direction) {			
			lookupCfgPtr0.groupKeyTypes.nonIpKey =
									CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E;
			lookupCfgPtr0.groupKeyTypes.ipv4Key = 
									CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E;
			lookupCfgPtr0.groupKeyTypes.ipv6Key =
						       		CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E;
		}
		else {		
			lookupCfgPtr0.groupKeyTypes.nonIpKey =
									CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
			lookupCfgPtr0.groupKeyTypes.ipv4Key =
									CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;	
			lookupCfgPtr0.groupKeyTypes.ipv6Key =
						       		CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
		}
	}
	else if(stdrule == 1) {
		if (direction) {
			lookupCfgPtr0.groupKeyTypes.nonIpKey =
									CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E;
			lookupCfgPtr0.groupKeyTypes.ipv4Key =
									CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E;
			lookupCfgPtr0.groupKeyTypes.ipv6Key =
					   				CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E;
		}
		else {
			lookupCfgPtr0.groupKeyTypes.nonIpKey =
									CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
			lookupCfgPtr0.groupKeyTypes.ipv4Key =
									CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
			lookupCfgPtr0.groupKeyTypes.ipv6Key =
					       		    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E;
		}
	}
	op_ret=cpssDxChPclCfgTblSet(devNum, &interfaceInfoPtr, direction,
									  lookupNum0, &lookupCfgPtr0);
	if(op_ret!=0) {
		nam_syslog_dbg("cpssDxChPclCfgTblSet lookup0 ERROR %ld!\n",op_ret);
	}

    /*lookup0*/
#ifdef DRV_LIB_CPSS_3_4
	result = cpssDxChPclPortLookupCfgTabAccessModeSet(devNum, portId, direction,lookupNum0, 0, mode);
	if(result!=0) {
		nam_syslog_dbg("npd_acl_drv>>npd_drv_port_bind_acl::cpssDxChPclPortLookupCfgTabAccessModeSet lookup0 ERROR %ld!\n",result); 
	}
#else
	result = cpssDxChPclPortLookupCfgTabAccessModeSet(devNum, portId, direction,lookupNum0, mode);
	if(result!=0) {
		nam_syslog_dbg("npd_acl_drv>>npd_drv_port_bind_acl::cpssDxChPclPortLookupCfgTabAccessModeSet lookup0 ERROR %ld!\n",result); 
	}
#endif
/*
  lookupNum1 = CPSS_PCL_LOOKUP_1_E; 		
  lookupCfgPtr1.enableLookup = (flag == ACL_TRUE) ? 1 : 0;
  lookupCfgPtr1.pclId = aclId;
  lookupCfgPtr1.groupKeyTypes.nonIpKey =
						 CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
						 //CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
  lookupCfgPtr1.groupKeyTypes.ipv4Key =
  						CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;
  						//CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
					       // CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E||				      
  lookupCfgPtr1.groupKeyTypes.ipv6Key =
						CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E;
  op_ret=cpssDxChPclCfgTblSet(devNum, &interfaceInfoPtr, direction,
									  lookupNum1, &lookupCfgPtr1);
  if(op_ret!=0)
	 nam_syslog_dbg("cpssDxChPclCfgTblSet lookup0 ERROR %ld!\n",op_ret);					 
	

    //lookup1
	result = cpssDxChPclPortLookupCfgTabAccessModeSet(devNum, portId, direction,lookupNum1, mode);
	if(result!=0)
		nam_syslog_dbg("npd_acl_drv>>npd_drv_port_bind_acl::cpssDxChPclPortLookupCfgTabAccessModeSet lookup0 ERROR %ld!\n",result); 
*/
	/*fetch data from cfg table */

	result = prvCpssDxChReadTableEntry(devNum, tableId,entryIndex, &hwValue);
	if (result != 0) {
		nam_syslog_dbg("read cfg table failure!\n");
	}
	else if(ACL_RETURN_CODE_SUCCESS == result) {
		if(SYSTEM_TYPE == IS_DISTRIBUTED)
		{
			if(!direction)
			{
				/* acl config table for XCAT */
	    		nam_syslog_dbg("PCL configure table line %d\n",entryIndex);		
	    		nam_syslog_dbg("******************* new *********************\n");
	    		nam_syslog_dbg("%-20s:%-x\n","LookUp0En",hwValue&0x1);
	    		nam_syslog_dbg("%-20s:%-x\n","LookUp0NonIpType",(hwValue >> 1)&0x7);
	    		nam_syslog_dbg("%-20s:%-x\n","LookUp0PCLID",0x03FF & (hwValue >> 13));
	    		nam_syslog_dbg("%-20s:%-x\n","LookUp0IPv4ARPKeyType",0x07 & (hwValue >> 4));
	    		nam_syslog_dbg("%-20s:%-x\n","LookUp0IPv6KeyType",0x07 & (hwValue >> 7));
	    		/*nam_syslog_dbg("%-20s:%-x\n","LookUp1En",(hwValue>>16)&0x1);*/
	    		/*nam_syslog_dbg("%-20s:%-x\n","LookUp1NonIpType",(hwValue >> 17)&0x1);*/
	    		/*nam_syslog_dbg("%-20s:%-x\n","LookUp1PCLID",0x03FF & (hwValue >> 18));*/
	    		/*nam_syslog_dbg("%-20s:%-x\n","LookUp1IPv4ARPKeyType",0x03 & (hwValue >> 28));*/
	    		/*nam_syslog_dbg("%-20s:%-x\n","LookUp1IPv6KeyType",0x03 & (hwValue >> 30));*/
	    		nam_syslog_dbg("********************************************\n");
			}
			else
			{
				
				/* acl config table for XCAT */
	    		nam_syslog_dbg("EPCL configure table line %d\n",entryIndex);		
	    		nam_syslog_dbg("******************* new *********************\n");
	    		nam_syslog_dbg("%-20s:%-x\n","LookUp0En",hwValue&0x1);
	    		nam_syslog_dbg("%-20s:%-x\n","LookUp0NonIpType",(hwValue >> 1)&0x3);
	    		nam_syslog_dbg("%-20s:%-x\n","LookUp0PCLID",0x03FF & (hwValue >> 3));
	    		nam_syslog_dbg("%-20s:%-x\n","LookUp0IPv4ARPKeyType",0x03 & (hwValue >> 13));
	    		nam_syslog_dbg("%-20s:%-x\n","LookUp0IPv6KeyType",0x03 & (hwValue >> 15));
	    		/*nam_syslog_dbg("%-20s:%-x\n","LookUp1En",(hwValue>>16)&0x1);*/
	    		/*nam_syslog_dbg("%-20s:%-x\n","LookUp1NonIpType",(hwValue >> 17)&0x1);*/
	    		/*nam_syslog_dbg("%-20s:%-x\n","LookUp1PCLID",0x03FF & (hwValue >> 18));*/
	    		/*nam_syslog_dbg("%-20s:%-x\n","LookUp1IPv4ARPKeyType",0x03 & (hwValue >> 28));*/
	    		/*nam_syslog_dbg("%-20s:%-x\n","LookUp1IPv6KeyType",0x03 & (hwValue >> 30));*/
	    		nam_syslog_dbg("********************************************\n");
			}
		}
		else
		{
    		nam_syslog_dbg("PCL configure table line %d\n",entryIndex);
    		nam_syslog_dbg("********************************************\n");
    		nam_syslog_dbg("%-20s:%-x\n","LookUp0En",hwValue&0x1);
    		nam_syslog_dbg("%-20s:%-x\n","LookUp0NonIpType",(hwValue >> 1)&0x1);
    		nam_syslog_dbg("%-20s:%-x\n","LookUp0PCLID",0x03FF & (hwValue >> 2));
    		nam_syslog_dbg("%-20s:%-x\n","LookUp0IPv4ARPKeyType",0x03 & (hwValue >> 12));
    		nam_syslog_dbg("%-20s:%-x\n","LookUp0IPv6KeyType",0x03 & (hwValue >> 14));
    		/*nam_syslog_dbg("%-20s:%-x\n","LookUp1En",(hwValue>>16)&0x1);*/
    		/*nam_syslog_dbg("%-20s:%-x\n","LookUp1NonIpType",(hwValue >> 17)&0x1);*/
    		/*nam_syslog_dbg("%-20s:%-x\n","LookUp1PCLID",0x03FF & (hwValue >> 18));*/
    		/*nam_syslog_dbg("%-20s:%-x\n","LookUp1IPv4ARPKeyType",0x03 & (hwValue >> 28));*/
    		/*nam_syslog_dbg("%-20s:%-x\n","LookUp1IPv6KeyType",0x03 & (hwValue >> 30));*/
    		nam_syslog_dbg("********************************************\n");
        }
	}
#endif

	return op_ret;
}

/**********************************************************************************
 * nam_acl_port_acl_enable
 *
 * set tcam for broadcom, if chip is marvell ret is success
 *
 *	INPUT:
 *		unsigned char 	devNum		- dev number
 *		unsigned int 	flag		- lookup1 enable or disable
 *		unsigned short  vlanId	 	- vlan index
 *
 *	OUTPUT:
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_SUCCESS 	- if broadcom chips return success or set lookup1 success 
 *		ACL_RETURN_CODE_ERROR		- set lookup1 fail
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int nam_acl_drv_cfg_table_set_lookup1
(
	unsigned char devNum,
	unsigned int  flag,
	unsigned short vlanId
)
{
	unsigned int op_ret = ACL_RETURN_CODE_SUCCESS;
#ifdef DRV_LIB_BCM
	op_ret = ACL_RETURN_CODE_SUCCESS;
#endif

#ifdef DRV_LIB_CPSS
	CPSS_INTERFACE_INFO_STC 		interfaceInfoPtr;
	CPSS_PCL_DIRECTION_ENT			direction;
	CPSS_PCL_LOOKUP_NUMBER_ENT		lookupNum1;
	CPSS_DXCH_PCL_LOOKUP_CFG_STC	lookupCfgPtr1;
	CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT	  mode;
	unsigned long					result = 0;
	unsigned int					hwValue = 0,entryIndex = 0;
	PRV_CPSS_DXCH_TABLE_ENT 		tableId;


    	direction = CPSS_PCL_DIRECTION_INGRESS_E;
	tableId = PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E;


	interfaceInfoPtr.type = CPSS_INTERFACE_VID_E; 
	interfaceInfoPtr.vlanId = vlanId;
	entryIndex = interfaceInfoPtr.vlanId;	
	mode = CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E;
 
	  
	lookupNum1 = CPSS_PCL_LOOKUP_1_E; 		
	lookupCfgPtr1.enableLookup = (flag == ACL_TRUE) ? 1 : 0;
	lookupCfgPtr1.pclId = 999;
	lookupCfgPtr1.groupKeyTypes.nonIpKey =
						 CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
	lookupCfgPtr1.groupKeyTypes.ipv4Key =
							CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;			      
	lookupCfgPtr1.groupKeyTypes.ipv6Key =
						CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E;
	op_ret=cpssDxChPclCfgTblSet(devNum, &interfaceInfoPtr, direction,
									  lookupNum1, &lookupCfgPtr1);
	if(op_ret!=0) {
	 nam_syslog_dbg("cpssDxChPclCfgTblSet lookup0 ERROR %ld!\n",op_ret);					 
	}

    /*lookup1*/
#ifdef DRV_LIB_CPSS_3_4	
	result = cpssDxChPclPortLookupCfgTabAccessModeSet(devNum, 999, direction,lookupNum1, 0, mode);
	if(result!=0) {
		nam_syslog_dbg("npd_acl_drv>>npd_drv_port_bind_acl::cpssDxChPclPortLookupCfgTabAccessModeSet lookup0 ERROR %ld!\n",result); 
	}
#else 
	result = cpssDxChPclPortLookupCfgTabAccessModeSet(devNum, 999, direction,lookupNum1, mode);
	if(result!=0) {
		nam_syslog_dbg("npd_acl_drv>>npd_drv_port_bind_acl::cpssDxChPclPortLookupCfgTabAccessModeSet lookup0 ERROR %ld!\n",result); 
	}
#endif
	/*fetch data from cfg table */

	result = prvCpssDxChReadTableEntry(devNum, tableId,entryIndex, &hwValue);
	if (result != 0) {
		nam_syslog_dbg("read cfg table failure!\n");
	}
#endif

	return op_ret;
}

/**********************************************************************************
 * nam_acl_port_acl_enable
 *
 * set tcam for marvell, if chip is broadcom ret is success
 *
 *	INPUT:
 *		unsigned int 			group_num	- group number
 *		unsigned int 			add_index	- acl index
 *		ACL_TCAM_RULE_FORMAT_E  ruleFormat	- rule type
 *		unsigned int 			dir_info	- ingress or egress group
 *
 *	OUTPUT:
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_SUCCESS 	- if broadcom chips return success or set tcam success 
 *		ACL_RETURN_CODE_ERROR		- set tcam fail
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int nam_acl_drv_tcam_set
(
	unsigned int group_num,
	unsigned int add_index,
	ACL_TCAM_RULE_FORMAT_E   ruleFormat,
	unsigned int dir_info
)
{
	unsigned int ret = ACL_RETURN_CODE_SUCCESS;
#ifdef DRV_LIB_BCM
		ret = ACL_RETURN_CODE_SUCCESS;
#endif
	
#ifdef DRV_LIB_CPSS
	unsigned int aclId = 0;		
	aclId = group_num;

	if(NULL!=g_acl_rule[add_index]){

		if(ACL_RULE_STD_IPv4_L4==ruleFormat){
			nam_syslog_dbg("g_acl_rule index %d  masksip %d, len %d\n",add_index, 
				g_acl_rule[add_index]->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSip,
				g_acl_rule[add_index]->TcamPtr.ruleInfo.stdIpv4L4.mask.maskLenSip);
			ret=nam_drv_acl_stdIpv4L4_ListDeal(g_acl_rule[add_index],aclId,1, dir_info);
			if(ACL_RETURN_CODE_SUCCESS!=ret){   
				nam_syslog_dbg("npd_dbus_add_rule_to_acl_group>>npd_drv_acl_ListDeal ERROR!");
			}	 
		} /*ipv4L4*/
		if(ACL_RULE_EXT_IPv4_L4==ruleFormat){
			ret=nam_drv_acl_extIpv4L4_ListDeal(g_acl_rule[add_index],aclId,1, dir_info);
			if(ACL_RETURN_CODE_SUCCESS!=ret){   
				nam_syslog_dbg("npd_dbus_add_rule_to_acl_group>>npd_drv_acl_ListDeal ERROR!");
			}	 
		} /*extipv4L4*/
		else if(ACL_RULE_STD_NOT_IP==ruleFormat){
			ret=nam_drv_acl_stdNotIp_ListDeal(g_acl_rule[add_index],aclId,1, dir_info);
			if(ACL_RETURN_CODE_SUCCESS!=ret){  
				nam_syslog_dbg("npd_dbus_add_rule_to_acl_group>>npd_drv_acl_ListDeal ERROR!");
			}	  
		} /*notIp	*/		
		else if(ACL_RULE_EXT_NOT_IPv6==ruleFormat){
			ret=nam_drv_acl_extNotIpv6_ListDeal(g_acl_rule[add_index],aclId,1, dir_info);
			if(ACL_RETURN_CODE_SUCCESS!=ret){  
				nam_syslog_dbg("npd_dbus_add_rule_to_acl_group>>npd_drv_acl_ListDeal ERROR!");		  
			}	  
		} /*extNotIpv6*/
		else if(ACL_RULE_STD_IP_L2_QoS==ruleFormat){
			ret=nam_drv_acl_arp_listDeal(g_acl_rule[add_index],aclId,1, dir_info);
			if(ACL_RETURN_CODE_SUCCESS!=ret){  
				nam_syslog_dbg("npd_dbus_add_rule_to_acl_group>>npd_drv_arp_acl_ListDeal ERROR!");
			}	  
		}/*arp*/
		else if(ACL_RULE_QOS==ruleFormat){
			ret=nam_drv_acl_qos_listDeal(g_acl_rule[add_index],aclId,1, dir_info);
			if(ACL_RETURN_CODE_SUCCESS!=ret){  
				nam_syslog_dbg("npd_dbus_add_rule_to_acl_group>>nam_drv_acl_qos_listDeal ERROR!");
			}
		}/*ext i*/
		else if(ACL_RULE_EXT_IPv6_L4==ruleFormat){
			if(!dir_info)
			{
				ret = nam_drv_ingress_acl_extIpv6_ListDeal(g_acl_rule[add_index],aclId,1);
				if(ACL_RETURN_CODE_SUCCESS!=ret){  
					nam_syslog_dbg("npd_dbus_add_rule_to_acl_group>>nam_drv_acl_extIpv6_ListDeal ERROR!");
				}
			}
			else
			{
				ret = nam_drv_egress_acl_extIpv6_ListDeal(g_acl_rule[add_index],aclId,1);
				if(ACL_RETURN_CODE_SUCCESS!=ret){  
					nam_syslog_dbg("npd_dbus_add_rule_to_acl_group>>nam_drv_acl_extIpv6_ListDeal ERROR!");
				}
			}
		}
	}
#endif

   	return ret;
}

/**********************************************************************************
 * nam_acl_port_acl_enable
 *
 * set tcam for marvell, if chip is broadcom ret is success
 *
 *	INPUT:
 *		unsigned int 	index	- acl index
 *		int 			enable	- enable clear tcam
 *
 *	OUTPUT:
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_SUCCESS 	- if broadcom chips return success or set tcam success 
 *		ACL_RETURN_CODE_ERROR		- set tcam fail
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int nam_acl_drv_tcam_clear
(
	unsigned int index,
	int enable
)
{
	unsigned int ret = ACL_RETURN_CODE_SUCCESS;;
#ifdef DRV_LIB_BCM
	unsigned int	devIdx = 0,appDemoDevAmount = 0;

	appDemoDevAmount = nam_asic_get_instance_num(); 
	if (enable) {
		for(devIdx=0;devIdx < appDemoDevAmount;devIdx++)
		{
			ret = bcm_field_counter_destroy(devIdx, index);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_counter_destroy \n");
			}
			ret = bcm_field_meter_destroy(devIdx, index);
			if(ACL_RETURN_CODE_SUCCESS != ret){
				nam_syslog_dbg("bcm_field_meter_destroy error!result %d",ret);	
			}
			ret = bcm_field_entry_remove(devIdx, index);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_entry_remove \n");
			}
			ret = bcm_field_entry_destroy(devIdx, index);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_entry_destroy \n");
			}
			nam_syslog_dbg("remove entry %d\n", index);
			nam_syslog_dbg("destroy entry %d\n", index);
		}
	}
	else {
		ret = ACL_RETURN_CODE_SUCCESS;
	}
#endif

#ifdef DRV_LIB_CPSS
	CPSS_DXCH_PCL_RULE_FORMAT_UNT	    maskData;
	CPSS_DXCH_PCL_RULE_FORMAT_UNT	    patternData;
	CPSS_DXCH_PCL_ACTION_STC		    	actionPtr;
	CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
	CPSS_PCL_RULE_SIZE_ENT	ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
	unsigned char  devIdx = 0, appDemoDevAmount = 0;
	ACL_TCAM_RULE_FORMAT_E	 nodeRuleFormat = ACL_RULE_STD_NOT_IP;   

	appDemoDevAmount = nam_asic_get_instance_num(); 

	memset(&maskData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(&patternData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(&actionPtr,0,sizeof(CPSS_DXCH_PCL_ACTION_STC));  



	nodeRuleFormat = g_acl_rule[index]->TcamPtr.ruleFormat;

	switch(nodeRuleFormat) {
		default:
			break;
		case ACL_RULE_STD_IPv4_L4:
			ruleFormat=CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;
			ruleSize = CPSS_PCL_RULE_SIZE_EXT_E;
			break;
		case ACL_RULE_EXT_IPv4_L4:
			ruleFormat=CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
			ruleSize = CPSS_PCL_RULE_SIZE_EXT_E;
			break;
		case ACL_RULE_STD_NOT_IP:
			ruleFormat=CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
			ruleSize = CPSS_PCL_RULE_SIZE_EXT_E;
			break;
		case ACL_RULE_EXT_NOT_IPv6:
			ruleFormat=CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
			ruleSize = CPSS_PCL_RULE_SIZE_EXT_E;
			break;
		case ACL_RULE_STD_IP_L2_QoS:
			ruleFormat=CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
			ruleSize = CPSS_PCL_RULE_SIZE_EXT_E;
			break;
		case ACL_RULE_QOS:
			ruleFormat=CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
			ruleSize = CPSS_PCL_RULE_SIZE_EXT_E;
			break;
	}
	
	/* save 0 1 rule for udp */
	if (index >= (MAX_ACL_RULE_NUMBER/2)) {
		index += ACL_SAVE_FOR_UDP_BPDU*2;
	}
	else {
		index += ACL_SAVE_FOR_UDP_BPDU;
	}	
	
	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++){

		ret = cpssDxChPclRuleSet(devIdx, ruleFormat,index, &maskData,
										&patternData, &actionPtr);		
		if(ret!=0){												
			nam_syslog_dbg("clear tcam rule(%s) %d err %d!\n",ruleFormatStr[nodeRuleFormat],index,ret);
			return ret;
		}
		else{
			nam_syslog_dbg("tcam pcl rule(%s) %d cleared\n",ruleFormatStr[nodeRuleFormat],index);			
		}

		/* Invalidate Rule */
		ret = cpssDxChPclRuleInvalidate(devIdx, ruleSize, index);
		if(ret!=0){												
			nam_syslog_dbg("invalidate tcam rule %d err %d\n",index,ret);
		}
		else{
			nam_syslog_dbg("tcam rule %d invalidated\n",index);			
		}
	}
#endif

	return ret;
}

/**********************************************************************************
 * nam_acl_drv_tcam_clear_udp_vlan
 *
 * clear tcam for udp vlan rule(marvell), if chip is broadcom ret is success
 *
 *	INPUT:
 *		unsigned int 	index	- acl index
 *		int 			enable	- enable clear tcam
 *
 *	OUTPUT:
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_SUCCESS 	- if broadcom chips return success or set tcam success 
 *		ACL_RETURN_CODE_ERROR		- set tcam fail
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int nam_acl_drv_tcam_clear_udp_vlan
(
	unsigned int index,
	unsigned int ruletype
)
{
	unsigned int ret = ACL_RETURN_CODE_SUCCESS;;

#ifdef DRV_LIB_CPSS
	CPSS_DXCH_PCL_RULE_FORMAT_UNT	    maskData;
	CPSS_DXCH_PCL_RULE_FORMAT_UNT	    patternData;
	CPSS_DXCH_PCL_ACTION_STC		    	actionPtr;
	CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
	CPSS_PCL_RULE_SIZE_ENT	ruleSize = CPSS_PCL_RULE_SIZE_STD_E;
	unsigned char  devIdx = 0, appDemoDevAmount = 0;
	ACL_TCAM_RULE_FORMAT_E	 nodeRuleFormat = ACL_RULE_STD_NOT_IP;   

	appDemoDevAmount = nam_asic_get_instance_num(); 

	memset(&maskData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(&patternData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(&actionPtr,0,sizeof(CPSS_DXCH_PCL_ACTION_STC));  

	if (0 == ruletype) {
		ruleFormat=CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;
		ruleSize = CPSS_PCL_RULE_SIZE_EXT_E;
	}
	else if (1 == ruletype){
		ruleFormat=CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
		ruleSize = CPSS_PCL_RULE_SIZE_EXT_E;
	}
	else {
		nam_syslog_dbg("ruletype bad Parameter \n");
	}
		
	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++){

		ret = cpssDxChPclRuleSet(devIdx, ruleFormat,index, &maskData,
										&patternData, &actionPtr);		
		if(ret!=0){												
			nam_syslog_dbg("clear tcam rule(%s) %d err %d!\n",ruleFormatStr[nodeRuleFormat],index,ret);
			return ret;
		}
		else{
			nam_syslog_dbg("tcam pcl rule(%s) %d cleared\n",ruleFormatStr[nodeRuleFormat],index);			
		}

		/* Invalidate Rule */
		ret = cpssDxChPclRuleInvalidate(devIdx, ruleSize, index);
		if(ret!=0){												
			nam_syslog_dbg("invalidate tcam rule %d err %d\n",index,ret);
		}
		else{
			nam_syslog_dbg("tcam rule %d invalidated\n",index);			
		}
	}
#endif

	return ret;
}

/**********************************************************************************
 * nam_drv_acl_xxx_listDeal
 *
 * set single rule tcam for marvell
 *
 *	INPUT:
 *		struct ACCESS_LIST_STC  *node	 - struct for acl rule
 *		unsigned int 			aclId	 - acl index
 *		int 					unit	 - dev number
 *		unsigned int			dir_info - ingress or egress group
 *
 *	OUTPUT:
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_SUCCESS 	- set rule tcam success 
 *		ACL_RETURN_CODE_ERROR		- set rule tcam fail
 *		
 *	NOTE:
 *
 **********************************************************************************/
#ifdef DRV_LIB_CPSS
unsigned int nam_drv_acl_qos_listDeal
(
	struct ACCESS_LIST_STC *node,
	unsigned int aclId, 
	int unit,
	unsigned int dir_info
)
{
	unsigned int result = 0;
	if(dir_info){
		result = nam_drv_egress_acl_qos_listDeal(node,aclId,unit);
	}
	else{
		result = nam_drv_ingress_acl_qos_listDeal(node,aclId,unit);
	}
	return result;
}

unsigned int nam_drv_ingress_acl_qos_listDeal
(
	struct ACCESS_LIST_STC *node,
	unsigned int aclId, 
	int unit
)
{
	unsigned char						devIdx = 0,appDemoDevAmount = 0;
	unsigned long						ruleIndex = 0;
	unsigned long						GetIndex = 0;	
	unsigned int						result = 0;
	unsigned int						maskPtr[12] = {0};
	unsigned int						patternPtr[12] = {0};
	unsigned int						actionptr[3] = {0}; 
	unsigned long						bitPos=189;
	ACL_ACTION_TYPE_E					actionType;
	CPSS_PCL_RULE_SIZE_ENT				ruleSize;
	/* table cpssDxChPclRule global variables */
	CPSS_DXCH_PCL_RULE_FORMAT_UNT		*maskData;
	CPSS_DXCH_PCL_RULE_FORMAT_UNT		*patternData;
	CPSS_DXCH_PCL_ACTION_STC			*actionPtr;
	CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT	ruleFormat;
	CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC *maskQos = NULL,*patternQos = NULL;

	
	maskData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(maskData==NULL){
		nam_syslog_dbg("malloc fail!\n");
		return 1;
	}
	patternData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(patternData==NULL){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		maskData=NULL;
		return 1;
	}
	actionPtr =(CPSS_DXCH_PCL_ACTION_STC*)malloc(sizeof(CPSS_DXCH_PCL_ACTION_STC));
	if(actionPtr==NULL){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		free(patternData);
		maskData=NULL;
		patternData=NULL;
		return 1;
	}

	memset(maskData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(patternData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(actionPtr,0,sizeof(CPSS_DXCH_PCL_ACTION_STC));	
	
	maskQos		= &(maskData->ruleExtNotIpv6);
	patternQos	= &(patternData->ruleExtNotIpv6);

	actionType = node->ActionPtr.actionType;

	appDemoDevAmount = nam_asic_get_instance_num(); 

	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++)
		{		
			
		/* map input arguments to locals */
			/*devNum;*/	
			/* use for save 0 1 for udp */
			if (node->ruleIndex >= (MAX_ACL_RULE_NUMBER/2)) {
				ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU*2;
			}
			else {
				ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU;
			}
			
			ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
			
			maskQos->common.pclId = 0x3FF;			
			maskQos->common.up =node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskUP;
			maskQos->commonExt.dscp =node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDSCP;		

			patternQos->common.pclId = aclId;
			patternQos->common.up =node->TcamPtr.ruleInfo.stdIpv4L4.rule.UP;
			patternQos->commonExt.dscp =node->TcamPtr.ruleInfo.stdIpv4L4.rule.DSCP;
											
			actionPtr->pktCmd = CPSS_PACKET_CMD_FORWARD_E;	

			if(actionType ==ACL_ACTION_TYPE_INGRESS_QOS){
				actionPtr->qos.modifyDscp=(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyDscp);
				actionPtr->qos.modifyUp= (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyUp);
				actionPtr->qos.qos.ingress.profileIndex = node->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
				actionPtr->qos.qos.ingress.profileAssignIndex = 1;
				actionPtr->qos.qos.ingress.profilePrecedence	= (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)(node->ActionPtr.actionInfo.qos.qos.ingress.profilePrecedence);
				
				actionPtr->policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;
				actionPtr->policer.policerId    = node->ActionPtr.policerPtr.policerId ; 
			}
			else if(actionType==ACL_ACTION_TYPE_EGRESS_QOS){
				actionPtr->egressPolicy =ACL_TRUE;
				actionPtr->qos.modifyDscp=(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyDscp);
				actionPtr->qos.modifyUp= (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyUp);
				actionPtr->qos.qos.egress.dscp = (char)(node->ActionPtr.actionInfo.qos.qos.egress.egrDscp);
				actionPtr->qos.qos.egress.up = (char)(node->ActionPtr.actionInfo.qos.qos.egress.egrUp);
			}

		/* call cpss api function */

		ruleSize=CPSS_PCL_RULE_SIZE_EXT_E;
		
		result = cpssDxChPclRuleSet(devIdx, ruleFormat, ruleIndex, maskData,
													patternData, actionPtr);		
		if(result!=0)
			nam_syslog_dbg("cpssDxChPclRuleSet ERROR!\n");
		nam_syslog_dbg("cpssDxChPclRuleSet result is %d\n",result);		
			
	} /*for*/

	free(maskData);
	free(patternData);
	free(actionPtr);	
	maskData=NULL;
	patternData=NULL;
	actionPtr=NULL;
	return result;		
}

unsigned int nam_drv_egress_acl_qos_listDeal
(
	struct ACCESS_LIST_STC *node,
	unsigned int aclId, 
	int unit
)
{
	unsigned char						devIdx = 0,appDemoDevAmount = 0;
	unsigned long						ruleIndex = 0;
	unsigned long						GetIndex = 0;	
	unsigned int						result = 0;
	unsigned int						maskPtr[12] = {0};
	unsigned int						patternPtr[12] = {0};
	unsigned int						actionptr[3] = {0}; 
	unsigned long						bitPos=189;
	ACL_ACTION_TYPE_E					actionType;
	CPSS_PCL_RULE_SIZE_ENT				ruleSize;
	/* table cpssDxChPclRule global variables */
	CPSS_DXCH_PCL_RULE_FORMAT_UNT		*maskData;
	CPSS_DXCH_PCL_RULE_FORMAT_UNT		*patternData;
	CPSS_DXCH_PCL_ACTION_STC			*actionPtr;
	CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT	ruleFormat;
	CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_NOT_IPV6_STC *maskQos = NULL,*patternQos = NULL;

	
	maskData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(maskData==NULL){
		nam_syslog_dbg("malloc fail!\n");
		return 1;
	}
	patternData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(patternData==NULL){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		maskData=NULL;
		return 1;
	}
	actionPtr =(CPSS_DXCH_PCL_ACTION_STC*)malloc(sizeof(CPSS_DXCH_PCL_ACTION_STC));
	if(actionPtr==NULL){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		free(patternData);
		maskData=NULL;
		patternData=NULL;
		return 1;
	}

	memset(maskData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(patternData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(actionPtr,0,sizeof(CPSS_DXCH_PCL_ACTION_STC));	
	
	maskQos		= &(maskData->ruleEgrExtNotIpv6);
	patternQos	= &(patternData->ruleEgrExtNotIpv6);

	actionType = node->ActionPtr.actionType;

	appDemoDevAmount = nam_asic_get_instance_num(); 

	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++)
		{		
			
		/* map input arguments to locals */
			/*devNum;*/	
			/* use for save 0 1 for udp */
			if (node->ruleIndex >= (MAX_ACL_RULE_NUMBER/2)) {
				ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU*2;
			}
			else {
				ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU;
			}
			
			ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E;
			
			maskQos->common.pclId = 0x3FF;			
			maskQos->common.up =node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskUP;
			maskQos->commonExt.dscp =node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDSCP;		

			patternQos->common.pclId = aclId;
			patternQos->common.up =node->TcamPtr.ruleInfo.stdIpv4L4.rule.UP;
			patternQos->commonExt.dscp =node->TcamPtr.ruleInfo.stdIpv4L4.rule.DSCP;
											
			actionPtr->pktCmd = CPSS_PACKET_CMD_FORWARD_E;	

			if(actionType ==ACL_ACTION_TYPE_INGRESS_QOS){
				actionPtr->qos.modifyDscp=(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyDscp);
				actionPtr->qos.modifyUp= (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyUp);
				actionPtr->qos.qos.ingress.profileIndex = node->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
				actionPtr->qos.qos.ingress.profileAssignIndex = 1;
				actionPtr->qos.qos.ingress.profilePrecedence	= (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)(node->ActionPtr.actionInfo.qos.qos.ingress.profilePrecedence);
				
				actionPtr->policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;
				actionPtr->policer.policerId    = node->ActionPtr.policerPtr.policerId ; 
			}
			else if(actionType==ACL_ACTION_TYPE_EGRESS_QOS){
				actionPtr->egressPolicy =ACL_TRUE;
				actionPtr->qos.modifyDscp=(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyDscp);
				actionPtr->qos.modifyUp= (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyUp);
				actionPtr->qos.qos.egress.dscp = (char)(node->ActionPtr.actionInfo.qos.qos.egress.egrDscp);
				actionPtr->qos.qos.egress.up = (char)(node->ActionPtr.actionInfo.qos.qos.egress.egrUp);
			}

		/* call cpss api function */

		ruleSize=CPSS_PCL_RULE_SIZE_EXT_E;
		
		result = cpssDxChPclRuleSet(devIdx, ruleFormat, ruleIndex, maskData,
													patternData, actionPtr);		
		if(result!=0)
			nam_syslog_dbg("cpssDxChPclRuleSet ERROR!\n");
		nam_syslog_dbg("cpssDxChPclRuleSet result is %d\n",result);		
			
	} /*for*/

	free(maskData);
	free(patternData);
	free(actionPtr);	
	maskData=NULL;
	patternData=NULL;
	actionPtr=NULL;
	return result;		
}

unsigned int nam_drv_acl_arp_listDeal
(
	struct ACCESS_LIST_STC *node,
	unsigned int aclId, 
	int unit,
	unsigned int dir_info
)
{
	unsigned int result = 0;
	if(dir_info){
		result = nam_drv_egress_acl_arp_listDeal(node,aclId,unit);
	}
	else{
		result = nam_drv_ingress_acl_arp_listDeal(node,aclId,unit);
	}
	return result;
}

unsigned int nam_drv_ingress_acl_arp_listDeal
(
	struct ACCESS_LIST_STC *node,
	unsigned int aclId, 
	int unit
)
{
	unsigned char						devIdx = 0,appDemoDevAmount = 0;
	unsigned long						ruleIndex = 0;
	unsigned long						GetIndex = 0;	
	unsigned int						i = 0,result = 0;
	unsigned long						pkd = 0;
	unsigned int						maskPtr[12] = {0};
	unsigned int						patternPtr[12] = {0};
	unsigned int						actionptr[3] = {0}; 

	CPSS_PCL_RULE_SIZE_ENT				ruleSize;
	/* table cpssDxChPclRule global variables */
	CPSS_DXCH_PCL_RULE_FORMAT_UNT		*maskData=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_UNT		*patternData=NULL;
	CPSS_DXCH_PCL_ACTION_STC			*actionPtr=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT	ruleFormat;
	
	CPSS_PACKET_CMD_ENT 				cmdPkd;
	unsigned int devNum = 0;

	CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC *maskArp = NULL,*patternArp = NULL;
	result = prvCpssDrvHwPpGetRegField(0,0x58,4,5,&devNum); /*get devnum*/
	if(result!=0)
			nam_syslog_dbg("prvCpssDrvHwPpGetRegField ERROR!\n");

	maskData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(maskData==NULL){
		nam_syslog_dbg("malloc fail!\n");
		return 1;
	}
	patternData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(patternData==NULL){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		maskData=NULL;
		return 1;
	}
	actionPtr =(CPSS_DXCH_PCL_ACTION_STC*)malloc(sizeof(CPSS_DXCH_PCL_ACTION_STC));
	if(actionPtr==NULL){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		free(patternData);
		maskData=NULL;
		patternData=NULL;
		return 1;
	}

	memset(maskData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(patternData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(actionPtr,0,sizeof(CPSS_DXCH_PCL_ACTION_STC));	
	
	maskArp		= &(maskData->ruleExtNotIpv6);
	patternArp	= &(patternData->ruleExtNotIpv6);

	appDemoDevAmount = nam_asic_get_instance_num(); 
	
	pkd=node->ActionPtr.actionType;

	switch(pkd)
		{
			case ACL_ACTION_TYPE_PERMIT:			 cmdPkd=CPSS_PACKET_CMD_FORWARD_E; break;
			case ACL_ACTION_TYPE_DENY:				 cmdPkd=CPSS_PACKET_CMD_DROP_HARD_E;break;
			case ACL_ACTION_TYPE_TRAP_TO_CPU:		 cmdPkd=CPSS_PACKET_CMD_TRAP_TO_CPU_E;break;
			case ACL_ACTION_TYPE_MIRROR_TO_ANALYZER: cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;
			case ACL_ACTION_TYPE_REDIRECT:			 cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;	 
			default:
					  break;
		}
	
	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++)
		{		
			
		/* map input arguments to locals */
			/*devNum;*/
			/* use for save 0 1 for udp */
			if (node->ruleIndex >= (MAX_ACL_RULE_NUMBER/2)) {
				ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU*2;
			}
			else {
				ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU;
			}
			
			ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;	
			
			maskArp->common.pclId = 0x3FF;
			/*
			 etherType  - Valid when <IsL2Valid> =1.
              	If <L2 Encap Type> = 0, this field contains the
              		<Dsap-Ssap> of the LLC NON-SNAP packet.
              	If <L2 Encap Type> = 1, this field contains the <EtherType/>
              		of the Ethernet V2 or LLC with SNAP packet.
			*/

			maskArp->etherType=0xFFFF;
			/*maskArp->isArp = 1;*/
			maskArp->l2Encap =1;
			maskArp->common.sourcePort = node->TcamPtr.ruleInfo.stdNotIp.mask.maskPortno;
			maskArp->common.vid = node->TcamPtr.ruleInfo.stdNotIp.mask.maskVlanId;
			maskArp->common.isL2Valid =1;

			patternArp->etherType=0x0806;
			/*patternArp->isArp = 1;*/
			patternArp->l2Encap =1;
			if (ACL_ANY_PORT_CHAR == (node->TcamPtr.ruleInfo.stdNotIp.rule.portno)) {
				patternArp->common.sourcePort = 0;
			}
			else {
				patternArp->common.sourcePort = node->TcamPtr.ruleInfo.stdNotIp.rule.portno;
			}
			patternArp->common.vid = node->TcamPtr.ruleInfo.stdNotIp.rule.vlanId;
			patternArp->common.isL2Valid =1;
		
					
			for(i=0;i<6;i++){
			maskArp->macSa.arEther[i]=node->TcamPtr.ruleInfo.stdNotIp.mask.maskSMac[i];
			patternArp->macSa.arEther[i]=node->TcamPtr.ruleInfo.stdNotIp.rule.sMac[i];
			}	


		patternArp->common.pclId = aclId;
				
		actionPtr->pktCmd = cmdPkd;	
			
		if(ACL_ACTION_TYPE_PERMIT==pkd){
				actionPtr->policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;
				actionPtr->policer.policerId    = node->ActionPtr.policerPtr.policerId ; 
		}	
		else if(ACL_ACTION_TYPE_MIRROR_TO_ANALYZER==pkd){			
			 actionPtr->mirror.mirrorToRxAnalyzerPort = 1;			 
		}
		else if(ACL_ACTION_TYPE_REDIRECT==pkd){			
				/*note: only port redirect to egress interface,that's policy switching,no router redirect*/
				actionPtr->redirect.redirectCmd =  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
				actionPtr->redirect.data.outIf.outInterface.devPort.devNum = devNum;/*for redirect*/
				actionPtr->redirect.data.outIf.outInterface.devPort.portNum = node->ActionPtr.actionInfo.redirect.portNum;
		}
		else if(ACL_ACTION_TYPE_TRAP_TO_CPU == pkd) {
			actionPtr->mirror.cpuCode =node->ActionPtr.actionInfo.trap.cpucode;
		}
		/*qos*/
		
		actionPtr->qos.modifyDscp=(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyDscp);
		actionPtr->qos.modifyUp= (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyUp);
		actionPtr->qos.qos.ingress.profileIndex = node->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
		if(ACL_ACTION_TYPE_PERMIT == pkd && 0 == actionPtr->policer.policerEnable){
			actionPtr->qos.qos.ingress.profileAssignIndex = 1;
		}
		else{
			actionPtr->qos.qos.ingress.profileAssignIndex = 0;
		}
		actionPtr->qos.qos.ingress.profilePrecedence	= (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)(node->ActionPtr.actionInfo.qos.qos.ingress.profilePrecedence);
	   
		/* call cpss api function */

		ruleSize=CPSS_PCL_RULE_SIZE_EXT_E;
		
		result = cpssDxChPclRuleSet(devIdx, ruleFormat, ruleIndex, maskData,
													patternData, actionPtr);		
		if(result!=0)
			nam_syslog_dbg("cpssDxChPclRuleSet ERROR!\n");
		nam_syslog_dbg("cpssDxChPclRuleSet result is %d\n",result);
					
		GetIndex=cpssDxChPclRuleGet(devIdx,ruleSize,ruleIndex,maskPtr,patternPtr,actionptr);
		if(GetIndex!=0)
			nam_syslog_dbg("cpssDxChPclRuleGet ERROR!\n");
			
	} /*for*/
	free(maskData);
	free(patternData);
	free(actionPtr);	
	maskData=NULL;
	patternData=NULL;
	actionPtr=NULL;

	return result;		
}

unsigned int nam_drv_egress_acl_arp_listDeal
(
	struct ACCESS_LIST_STC *node,
	unsigned int aclId, 
	int unit
)
{
	unsigned char						devIdx = 0,appDemoDevAmount = 0;
	unsigned long						ruleIndex = 0;
	unsigned long						GetIndex = 0;	
	unsigned int						i = 0,result = 0;
	unsigned long						pkd = 0;
	unsigned int						maskPtr[12] = {0};
	unsigned int						patternPtr[12] = {0};
	unsigned int						actionptr[3] = {0}; 

	CPSS_PCL_RULE_SIZE_ENT				ruleSize;
	/* table cpssDxChPclRule global variables */
	CPSS_DXCH_PCL_RULE_FORMAT_UNT		*maskData=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_UNT		*patternData=NULL;
	CPSS_DXCH_PCL_ACTION_STC			*actionPtr=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT	ruleFormat;
	
	CPSS_PACKET_CMD_ENT 				cmdPkd;
	unsigned int devNum = 0;
	result = prvCpssDrvHwPpGetRegField(0,0x58,4,5,&devNum);
	if(result!=0)
			nam_syslog_dbg("prvCpssDrvHwPpGetRegField ERROR!\n");

	CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_NOT_IPV6_STC *maskArp = NULL,*patternArp = NULL;

	maskData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(maskData==NULL){
		nam_syslog_dbg("malloc fail!\n");
		return 1;
	}
	patternData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(patternData==NULL){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		maskData=NULL;
		return 1;
	}
	actionPtr =(CPSS_DXCH_PCL_ACTION_STC*)malloc(sizeof(CPSS_DXCH_PCL_ACTION_STC));
	if(actionPtr==NULL){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		free(patternData);
		maskData=NULL;
		patternData=NULL;
		return 1;
	}

	memset(maskData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(patternData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(actionPtr,0,sizeof(CPSS_DXCH_PCL_ACTION_STC));	
	
	maskArp		= &(maskData->ruleEgrExtNotIpv6);
	patternArp	= &(patternData->ruleEgrExtNotIpv6);

	appDemoDevAmount = nam_asic_get_instance_num(); 
	
	pkd=node->ActionPtr.actionType;

	switch(pkd)
		{
			case ACL_ACTION_TYPE_PERMIT:			 cmdPkd=CPSS_PACKET_CMD_FORWARD_E; break;
			case ACL_ACTION_TYPE_DENY:				 cmdPkd=CPSS_PACKET_CMD_DROP_HARD_E;break;
			case ACL_ACTION_TYPE_TRAP_TO_CPU:		 cmdPkd=CPSS_PACKET_CMD_TRAP_TO_CPU_E;break;
			case ACL_ACTION_TYPE_MIRROR_TO_ANALYZER: cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;
			case ACL_ACTION_TYPE_REDIRECT:			 cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;	 
			default:
					  break;
		}
	
	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++)
		{		
			
		/* map input arguments to locals */
			/*devNum;*/
			/* use for save 0 1 for udp */
			if (node->ruleIndex >= (MAX_ACL_RULE_NUMBER/2)) {
				ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU*2;
			}
			else {
				ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU;
			}
			
			ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E;	
			
			maskArp->common.pclId = 0x3FF;
			/*
			 etherType  - Valid when <IsL2Valid> =1.
              	If <L2 Encap Type> = 0, this field contains the
              		<Dsap-Ssap> of the LLC NON-SNAP packet.
              	If <L2 Encap Type> = 1, this field contains the <EtherType/>
              		of the Ethernet V2 or LLC with SNAP packet.
			*/

			maskArp->etherType=0xFFFF;
			/*maskArp->isArp = 1;*/
			maskArp->l2Encap =1;
			maskArp->common.sourcePort = node->TcamPtr.ruleInfo.stdNotIp.mask.maskPortno;
			maskArp->common.vid = node->TcamPtr.ruleInfo.stdNotIp.mask.maskVlanId;
			maskArp->common.isL2Valid =1;

			patternArp->etherType=0x0806;
			/*patternArp->isArp = 1;*/
			patternArp->l2Encap =1;
			if (ACL_ANY_PORT_CHAR == (node->TcamPtr.ruleInfo.stdNotIp.rule.portno)) {
				patternArp->common.sourcePort = 0;
			}
			else {
				patternArp->common.sourcePort = node->TcamPtr.ruleInfo.stdNotIp.rule.portno;
			}
			patternArp->common.vid = node->TcamPtr.ruleInfo.stdNotIp.rule.vlanId;
			patternArp->common.isL2Valid =1;
		
					
			for(i=0;i<6;i++){
			maskArp->macSa.arEther[i]=node->TcamPtr.ruleInfo.stdNotIp.mask.maskSMac[i];
			patternArp->macSa.arEther[i]=node->TcamPtr.ruleInfo.stdNotIp.rule.sMac[i];
			}	


		patternArp->common.pclId = aclId;
				
		actionPtr->pktCmd = cmdPkd;	
			
		if(ACL_ACTION_TYPE_PERMIT==pkd){
				actionPtr->policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;
				actionPtr->policer.policerId    = node->ActionPtr.policerPtr.policerId ; 
		}	
		else if(ACL_ACTION_TYPE_MIRROR_TO_ANALYZER==pkd){			
			 actionPtr->mirror.mirrorToRxAnalyzerPort = 1;			 
		}
		else if(ACL_ACTION_TYPE_REDIRECT==pkd){			
				/*note: only port redirect to egress interface,that's policy switching,no router redirect*/
				actionPtr->redirect.redirectCmd =  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
				actionPtr->redirect.data.outIf.outInterface.devPort.devNum = devNum;
				actionPtr->redirect.data.outIf.outInterface.devPort.portNum = node->ActionPtr.actionInfo.redirect.portNum;
		}
		else if(ACL_ACTION_TYPE_TRAP_TO_CPU == pkd) {
			actionPtr->mirror.cpuCode =node->ActionPtr.actionInfo.trap.cpucode;
		}
		/*qos*/
		
		actionPtr->qos.modifyDscp=(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyDscp);
		actionPtr->qos.modifyUp= (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyUp);
		actionPtr->qos.qos.ingress.profileIndex = node->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
		actionPtr->qos.qos.ingress.profileAssignIndex = 1;
		actionPtr->qos.qos.ingress.profilePrecedence	= (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)(node->ActionPtr.actionInfo.qos.qos.ingress.profilePrecedence);
	   
		/* call cpss api function */

		ruleSize=CPSS_PCL_RULE_SIZE_EXT_E;
		
		result = cpssDxChPclRuleSet(devIdx, ruleFormat, ruleIndex, maskData,
													patternData, actionPtr);		
		if(result!=0)
			nam_syslog_dbg("cpssDxChPclRuleSet ERROR!\n");
		nam_syslog_dbg("cpssDxChPclRuleSet result is %d\n",result);
					
		GetIndex=cpssDxChPclRuleGet(devIdx,ruleSize,ruleIndex,maskPtr,patternPtr,actionptr);
		if(GetIndex!=0)
			nam_syslog_dbg("cpssDxChPclRuleGet ERROR!\n");
			
	} /*for*/
	free(maskData);
	free(patternData);
	free(actionPtr);	
	maskData=NULL;
	patternData=NULL;
	actionPtr=NULL;

	return result;		
}

unsigned int nam_drv_acl_stdNotIp_ListDeal
(
	struct ACCESS_LIST_STC *node,
	unsigned int aclId, 
	int unit,
	unsigned int dir_info
)
{
	unsigned int result = 0;
	if(dir_info){
		result = nam_drv_egress_acl_stdNotIp_ListDeal(node,aclId,unit);
	}
	else{
		result = nam_drv_ingress_acl_stdNotIp_ListDeal(node,aclId,unit);
	}
	return result;
}

unsigned int nam_drv_ingress_acl_stdNotIp_ListDeal
(
	struct ACCESS_LIST_STC *node,
	unsigned int aclId, 
	int unit
)
{
	unsigned char 						devIdx = 0,appDemoDevAmount = 0;
	unsigned long 						ruleIndex = 0;
	unsigned long 						GetIndex = 0;	
	unsigned int		 				i = 0,result = 0;
	unsigned long           				pkd = 0;
	unsigned int           				maskPtr[12] = {0};
       unsigned int           				patternPtr[12] = {0};
       unsigned int           				actionptr[3] = {0};	
	unsigned long 						bitPos=189;
	CPSS_PCL_RULE_SIZE_ENT  			ruleSize;
	/* table cpssDxChPclRule global variables */
	CPSS_DXCH_PCL_RULE_FORMAT_UNT	    *maskData=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_UNT	    *patternData=NULL;
	CPSS_DXCH_PCL_ACTION_STC		    *actionPtr=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
	
	CPSS_PACKET_CMD_ENT 				cmdPkd;
	unsigned int devNum = 0;
		
	/*CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC *maskStdNotIp = NULL,*patternStdNotIp = NULL;*/
	CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC *maskStdNotIp = NULL,*patternStdNotIp = NULL;
	result = prvCpssDrvHwPpGetRegField(0,0x58,4,5,&devNum);
	if(result!=0)
			nam_syslog_dbg("prvCpssDrvHwPpGetRegField ERROR!\n");

	maskData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(maskData==NULL){
		nam_syslog_dbg("malloc fail!\n");
		return 1;
	}
	patternData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(patternData==NULL){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		maskData=NULL;
		return 1;
	}
	actionPtr =(CPSS_DXCH_PCL_ACTION_STC*)malloc(sizeof(CPSS_DXCH_PCL_ACTION_STC));
	if(actionPtr==NULL){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		free(patternData);
		maskData=NULL;
		patternData=NULL;
		return 1;
	}
	memset(maskData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(patternData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(actionPtr,0,sizeof(CPSS_DXCH_PCL_ACTION_STC));	

	maskStdNotIp		= &(maskData->ruleStdNotIp);
	patternStdNotIp		= &(patternData->ruleStdNotIp);

	appDemoDevAmount = nam_asic_get_instance_num(); 

	pkd=node->ActionPtr.actionType;

	switch(pkd)
		{
			case ACL_ACTION_TYPE_PERMIT:			      cmdPkd=CPSS_PACKET_CMD_FORWARD_E; break;
			case ACL_ACTION_TYPE_DENY:				      cmdPkd=CPSS_PACKET_CMD_DROP_HARD_E;break;
			case ACL_ACTION_TYPE_TRAP_TO_CPU:   	      cmdPkd=CPSS_PACKET_CMD_TRAP_TO_CPU_E;break;
		       case ACL_ACTION_TYPE_MIRROR_TO_ANALYZER:  cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;
			case ACL_ACTION_TYPE_REDIRECT:			      cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;	 
			default:
					  break;
		}
	
	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++)
		{		
			
		/* map input arguments to locals */
			/*devNum;*/
			/* use for save 0 1 for udp */
			if (node->ruleIndex >= (MAX_ACL_RULE_NUMBER/2)) {
				ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU*2;
			}
			else {
				ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU;
			}
			/*ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;	*/
			ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
			
			maskStdNotIp->common.pclId = 0x3FF;	

			if (node->TcamPtr.ruleInfo.stdNotIp.rule.ethertype < 0xffff) {
				patternStdNotIp->etherType = node->TcamPtr.ruleInfo.stdNotIp.rule.ethertype;
				maskStdNotIp->etherType = node->TcamPtr.ruleInfo.stdNotIp.mask.maskethertype;
				nam_syslog_dbg("set etherpye is %x  mask is %x\n", 
					patternStdNotIp->etherType, maskStdNotIp->etherType);
			}
			for(i=0;i<6;i++){
				maskStdNotIp->macDa.arEther[i]=node->TcamPtr.ruleInfo.stdNotIp.mask.maskDMac[i];
				maskStdNotIp->macSa.arEther[i]=node->TcamPtr.ruleInfo.stdNotIp.mask.maskSMac[i];
				patternStdNotIp->macDa.arEther[i]=node->TcamPtr.ruleInfo.stdNotIp.rule.dMac[i];
				patternStdNotIp->macSa.arEther[i]=node->TcamPtr.ruleInfo.stdNotIp.rule.sMac[i];
			}	

			patternStdNotIp->common.pclId = aclId;
				
			actionPtr->pktCmd = cmdPkd;			 
			if(ACL_ACTION_TYPE_PERMIT==pkd){
				actionPtr->policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;
				actionPtr->policer.policerId    = node->ActionPtr.policerPtr.policerId ; 
			}
		
			else if(ACL_ACTION_TYPE_MIRROR_TO_ANALYZER==pkd){

				/*actionPtr.policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;*/
				/*actionPtr.policer.policerId    = node->ActionPtr.policerPtr.policerId ;	*/

				 actionPtr->mirror.mirrorToRxAnalyzerPort = 1;					
			}
			else if(ACL_ACTION_TYPE_REDIRECT==pkd){			
					
					/*actionPtr.policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;*/
					/*actionPtr.policer.policerId    = node->ActionPtr.policerPtr.policerId ;*/
					/*note: only port redirect to egress interface,that's policy switching,no router redirect*/
					 actionPtr->redirect.redirectCmd =  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
					 actionPtr->redirect.data.outIf.outInterface.devPort.devNum = devNum;
					 actionPtr->redirect.data.outIf.outInterface.devPort.portNum = node->ActionPtr.actionInfo.redirect.portNum;
					 /*actionPtr.redirect.data.outIf.outInterface.vlanId = 1;*/
			}
			else if(ACL_ACTION_TYPE_TRAP_TO_CPU	== pkd) {
				actionPtr->mirror.cpuCode =node->ActionPtr.actionInfo.trap.cpucode;
			}
			/*qos*/
			nam_syslog_dbg("hahahaha\n");
			actionPtr->qos.modifyDscp=(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyDscp);
			actionPtr->qos.modifyUp= (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyUp);
			actionPtr->qos.qos.ingress.profileIndex = node->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
			if(ACL_ACTION_TYPE_PERMIT == pkd && 0 == actionPtr->policer.policerEnable){
				actionPtr->qos.qos.ingress.profileAssignIndex = 1;
			}
			else{
				actionPtr->qos.qos.ingress.profileAssignIndex = 0;
			}
			actionPtr->qos.qos.ingress.profilePrecedence	= (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)(node->ActionPtr.actionInfo.qos.qos.ingress.profilePrecedence);
			
			/*call cpss api function */
			
			ruleSize=CPSS_PCL_RULE_SIZE_EXT_E;
					
			result = cpssDxChPclRuleSet(devIdx, ruleFormat, ruleIndex, maskData,
																patternData, actionPtr);		
				
			if(result!=0)
				nam_syslog_dbg("cpssDxChPclRuleSet ERROR!\n");
			nam_syslog_dbg("cpssDxChPclRuleSet result is %d\n",result);						
		
	}/*for*/
	
	free(maskData);
	free(patternData);
	free(actionPtr);	
	maskData=NULL;
	patternData=NULL;
	actionPtr=NULL;

	return result;		
}

unsigned int nam_drv_egress_acl_stdNotIp_ListDeal
(
	struct ACCESS_LIST_STC *node,
	unsigned int aclId, 
	int unit
)
{
	unsigned char 						devIdx = 0,appDemoDevAmount = 0;
	unsigned long 						ruleIndex = 0;
	unsigned long 						GetIndex = 0;	
	unsigned int		 				i = 0,result = 0;
	unsigned long           				pkd = 0;
	unsigned int           				maskPtr[12] = {0};
       unsigned int           				patternPtr[12] = {0};
       unsigned int           				actionptr[3] = {0};	
	unsigned long 						bitPos=189;
	CPSS_PCL_RULE_SIZE_ENT  			ruleSize;
	/* table cpssDxChPclRule global variables */
	CPSS_DXCH_PCL_RULE_FORMAT_UNT	    *maskData=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_UNT	    *patternData=NULL;
	CPSS_DXCH_PCL_ACTION_STC		    *actionPtr=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
	
	CPSS_PACKET_CMD_ENT 				cmdPkd;
	unsigned int devNum = 0;
		
	/*CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC *maskStdNotIp = NULL,*patternStdNotIp = NULL;*/
	CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_NOT_IPV6_STC *maskStdNotIp = NULL,*patternStdNotIp = NULL;

	result = prvCpssDrvHwPpGetRegField(0,0x58,4,5,&devNum);
	if(result!=0)
			nam_syslog_dbg("prvCpssDrvHwPpGetRegField ERROR!\n");
	maskData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(maskData==NULL){
		nam_syslog_dbg("malloc fail!\n");
		return 1;
	}
	patternData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(patternData==NULL){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		maskData=NULL;
		return 1;
	}
	actionPtr =(CPSS_DXCH_PCL_ACTION_STC*)malloc(sizeof(CPSS_DXCH_PCL_ACTION_STC));
	if(actionPtr==NULL){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		free(patternData);
		maskData=NULL;
		patternData=NULL;
		return 1;
	}
	memset(maskData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(patternData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(actionPtr,0,sizeof(CPSS_DXCH_PCL_ACTION_STC));	

	maskStdNotIp		= &(maskData->ruleEgrExtNotIpv6);
	patternStdNotIp		= &(patternData->ruleEgrExtNotIpv6);

	appDemoDevAmount = nam_asic_get_instance_num(); 

	pkd=node->ActionPtr.actionType;

	switch(pkd)
		{
			case ACL_ACTION_TYPE_PERMIT:			      cmdPkd=CPSS_PACKET_CMD_FORWARD_E; break;
			case ACL_ACTION_TYPE_DENY:				      cmdPkd=CPSS_PACKET_CMD_DROP_HARD_E;break;
			case ACL_ACTION_TYPE_TRAP_TO_CPU:   	      cmdPkd=CPSS_PACKET_CMD_TRAP_TO_CPU_E;break;
		       case ACL_ACTION_TYPE_MIRROR_TO_ANALYZER:  cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;
			case ACL_ACTION_TYPE_REDIRECT:			      cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;	 
			default:
					  break;
		}
	
	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++)
		{		
			
		/* map input arguments to locals */
			/*devNum;*/
			/* use for save 0 1 for udp */
			if (node->ruleIndex >= (MAX_ACL_RULE_NUMBER/2)) {
				ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU*2;
			}
			else {
				ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU;
			}
			/*ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;	*/
			ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E;
			
			maskStdNotIp->common.pclId = 0x3FF;	

			if (node->TcamPtr.ruleInfo.stdNotIp.rule.ethertype < 0xffff) {
				patternStdNotIp->etherType = node->TcamPtr.ruleInfo.stdNotIp.rule.ethertype;
				maskStdNotIp->etherType = node->TcamPtr.ruleInfo.stdNotIp.mask.maskethertype;
				nam_syslog_dbg("set etherpye is %x  mask is %x\n", 
					patternStdNotIp->etherType, maskStdNotIp->etherType);
			}
			for(i=0;i<6;i++){
				maskStdNotIp->macDa.arEther[i]=node->TcamPtr.ruleInfo.stdNotIp.mask.maskDMac[i];
				maskStdNotIp->macSa.arEther[i]=node->TcamPtr.ruleInfo.stdNotIp.mask.maskSMac[i];
				patternStdNotIp->macDa.arEther[i]=node->TcamPtr.ruleInfo.stdNotIp.rule.dMac[i];
				patternStdNotIp->macSa.arEther[i]=node->TcamPtr.ruleInfo.stdNotIp.rule.sMac[i];
			}	

			patternStdNotIp->common.pclId = aclId;
				
			actionPtr->pktCmd = cmdPkd;			 
			if(ACL_ACTION_TYPE_PERMIT==pkd){
				actionPtr->policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;
				actionPtr->policer.policerId    = node->ActionPtr.policerPtr.policerId ; 
			}
		
			else if(ACL_ACTION_TYPE_MIRROR_TO_ANALYZER==pkd){

				/*actionPtr.policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;*/
				/*actionPtr.policer.policerId    = node->ActionPtr.policerPtr.policerId ;	*/

				 actionPtr->mirror.mirrorToRxAnalyzerPort = 1;					
			}
			else if(ACL_ACTION_TYPE_REDIRECT==pkd){			
					
					/*actionPtr.policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;*/
					/*actionPtr.policer.policerId    = node->ActionPtr.policerPtr.policerId ;*/
					/*note: only port redirect to egress interface,that's policy switching,no router redirect*/
					 actionPtr->redirect.redirectCmd =  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
					 actionPtr->redirect.data.outIf.outInterface.devPort.devNum = devNum;
					 actionPtr->redirect.data.outIf.outInterface.devPort.portNum = node->ActionPtr.actionInfo.redirect.portNum;
					 /*actionPtr.redirect.data.outIf.outInterface.vlanId = 1;*/
			}
			else if(ACL_ACTION_TYPE_TRAP_TO_CPU	== pkd) {
				actionPtr->mirror.cpuCode =node->ActionPtr.actionInfo.trap.cpucode;
			}
			/*qos*/
			
			actionPtr->qos.modifyDscp=(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyDscp);
			actionPtr->qos.modifyUp= (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyUp);
			actionPtr->qos.qos.ingress.profileIndex = node->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
			actionPtr->qos.qos.ingress.profileAssignIndex = 1;
			actionPtr->qos.qos.ingress.profilePrecedence	= (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)(node->ActionPtr.actionInfo.qos.qos.ingress.profilePrecedence);
			
			/*call cpss api function */
			
			ruleSize=CPSS_PCL_RULE_SIZE_EXT_E;
					
			result = cpssDxChPclRuleSet(devIdx, ruleFormat, ruleIndex, maskData,
																patternData, actionPtr);		
				
			if(result!=0)
				nam_syslog_dbg("cpssDxChPclRuleSet ERROR!\n");
			nam_syslog_dbg("cpssDxChPclRuleSet result is %d\n",result);						
		
	}/*for*/
	
	free(maskData);
	free(patternData);
	free(actionPtr);	
	maskData=NULL;
	patternData=NULL;
	actionPtr=NULL;

	return result;		
}

unsigned int nam_drv_acl_stdIpL2_ListDeal
(
	struct ACCESS_LIST_STC *node,
	unsigned int aclId, 
	int unit,
	unsigned int dir_info	
)
{
	unsigned char 						devIdx = 0,appDemoDevAmount = 0;
	unsigned long 						ruleIndex = 0;	
	unsigned int		 				i = 0,result = 0;
	unsigned long           			pkd = 0;
	CPSS_PCL_RULE_SIZE_ENT  			ruleSize;
	/* table cpssDxChPclRule global variables */
	CPSS_DXCH_PCL_RULE_FORMAT_UNT	    *maskData=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_UNT	    *patternData=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
	CPSS_DXCH_PCL_ACTION_STC		    *actionPtr=NULL;
	CPSS_PACKET_CMD_ENT 				cmdPkd;
		
	CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC  *maskStdIpL2 = NULL,*patternStdIpL2 = NULL;
	unsigned int devNum = 0;
	result = prvCpssDrvHwPpGetRegField(0,0x58,4,5,&devNum);
	if(result!=0)
			nam_syslog_dbg("prvCpssDrvHwPpGetRegField ERROR!\n");

	maskData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(maskData==NULL){
		nam_syslog_dbg("malloc fail!\n");
		return 1;
	}
	patternData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(patternData==NULL){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		maskData=NULL;
		return 1;
	}
	actionPtr =(CPSS_DXCH_PCL_ACTION_STC*)malloc(sizeof(CPSS_DXCH_PCL_ACTION_STC));
	if(actionPtr==NULL){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		free(patternData);
		maskData=NULL;
		patternData=NULL;
		return 1;
	}
	memset(maskData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(patternData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(actionPtr,0,sizeof(CPSS_DXCH_PCL_ACTION_STC));	

	maskStdIpL2		= &(maskData->ruleStdIpL2Qos);
	patternStdIpL2		= &(patternData->ruleStdIpL2Qos);

	appDemoDevAmount = nam_asic_get_instance_num(); 

	pkd=node->ActionPtr.actionType;

	switch(pkd)
		{
			case ACL_ACTION_TYPE_PERMIT:			      cmdPkd=CPSS_PACKET_CMD_FORWARD_E; break;
			case ACL_ACTION_TYPE_DENY:				      cmdPkd=CPSS_PACKET_CMD_DROP_HARD_E;break;
			case ACL_ACTION_TYPE_TRAP_TO_CPU:   	      cmdPkd=CPSS_PACKET_CMD_TRAP_TO_CPU_E;break;
		       case ACL_ACTION_TYPE_MIRROR_TO_ANALYZER:  cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;
			case ACL_ACTION_TYPE_REDIRECT:			      cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;	 
			default:
					  break;
		}
	
	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++)
		{		
			
		/* map input arguments to locals */
			/*devNum;*/
			/* use for save 0 1 for udp */
			if (node->ruleIndex >= (MAX_ACL_RULE_NUMBER/2)) {
				ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU*2;
			}
			else {
				ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU;
			}
			if (dir_info) {
				ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E;
			}
			else {
				ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
			}
			maskStdIpL2->common.pclId = 0x3FF;	
			
			for(i=0;i<6;i++){
				maskStdIpL2->macDa.arEther[i]=node->TcamPtr.ruleInfo.stdNotIp.mask.maskDMac[i];
				maskStdIpL2->macSa.arEther[i]=node->TcamPtr.ruleInfo.stdNotIp.mask.maskSMac[i];
				patternStdIpL2->macDa.arEther[i]=node->TcamPtr.ruleInfo.stdNotIp.rule.dMac[i];
				patternStdIpL2->macSa.arEther[i]=node->TcamPtr.ruleInfo.stdNotIp.rule.sMac[i];
			}	

			patternStdIpL2->common.pclId = aclId;
				
			actionPtr->pktCmd = cmdPkd;			 
			if(ACL_ACTION_TYPE_PERMIT==pkd){
				actionPtr->policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;
				actionPtr->policer.policerId    = node->ActionPtr.policerPtr.policerId ; 
			}
		
			else if(ACL_ACTION_TYPE_MIRROR_TO_ANALYZER==pkd){

				/*actionPtr.policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;*/
				/*actionPtr.policer.policerId    = node->ActionPtr.policerPtr.policerId ;	*/

				 actionPtr->mirror.mirrorToRxAnalyzerPort = 1;					
			}
			else if(ACL_ACTION_TYPE_REDIRECT==pkd){			
					
					/*actionPtr.policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;*/
					/*actionPtr.policer.policerId    = node->ActionPtr.policerPtr.policerId ; */
					/*note: only port redirect to egress interface,that's policy switching,no router redirect*/
					 actionPtr->redirect.redirectCmd =  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
					 actionPtr->redirect.data.outIf.outInterface.devPort.devNum = devNum;
					 actionPtr->redirect.data.outIf.outInterface.devPort.portNum = node->ActionPtr.actionInfo.redirect.portNum;
					 /*actionPtr.redirect.data.outIf.outInterface.vlanId = 1;*/
			}
			else if(ACL_ACTION_TYPE_TRAP_TO_CPU	== pkd) {
				actionPtr->mirror.cpuCode =node->ActionPtr.actionInfo.trap.cpucode;
			}
			/*qos*/
			
			actionPtr->qos.modifyDscp=(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyDscp);
			actionPtr->qos.modifyUp= (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyUp);
			actionPtr->qos.qos.ingress.profileIndex = node->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
			if(ACL_ACTION_TYPE_PERMIT == pkd && 0 == actionPtr->policer.policerEnable){
				actionPtr->qos.qos.ingress.profileAssignIndex = 1;
			}
			else{
				actionPtr->qos.qos.ingress.profileAssignIndex = 0;
			}
			actionPtr->qos.qos.ingress.profilePrecedence	= (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)(node->ActionPtr.actionInfo.qos.qos.ingress.profilePrecedence);
			
			/* call cpss api function */
			
			ruleSize=CPSS_PCL_RULE_SIZE_STD_E;
					
			result = cpssDxChPclRuleSet(devIdx, ruleFormat, ruleIndex, maskData,
																patternData, actionPtr);		
				
			if(result!=0)
				nam_syslog_dbg("cpssDxChPclRuleSet ERROR!\n");
			nam_syslog_dbg("cpssDxChPclRuleSet result is %d\n",result);
		}
	free(maskData);
	free(patternData);
	free(actionPtr);	
	maskData=NULL;
	patternData=NULL;
	actionPtr=NULL;

	return result;		
}

unsigned int nam_drv_acl_extNotIpv6_ListDeal
(
	struct ACCESS_LIST_STC *node,
	unsigned int aclId, 
	int unit,
	unsigned int dir_info
)
{
	unsigned int result = 0;
	if(dir_info){
		result = nam_drv_egress_acl_extNotIpv6_ListDeal(node,aclId,unit);
	}
	else{
		result = nam_drv_ingress_acl_extNotIpv6_ListDeal(node,aclId,unit);
	}
	return result;
}

unsigned int nam_drv_ingress_acl_extNotIpv6_ListDeal
(
	struct ACCESS_LIST_STC * node,
	unsigned int aclId, 
	int unit
)
{
	unsigned char						devIdx = 0,appDemoDevAmount = 0;
	unsigned long						ruleIndex = 0;
	unsigned long						GetIndex = 0;	
	unsigned int						i = 0,ipty = 0,result = 0;
	unsigned long						pkd = 0,nodeIpProtocal = 0;
	unsigned int						maskPtr[12] = {0};
	unsigned int						patternPtr[12] = {0};
	unsigned int						actionptr[3] = {0}; 
	unsigned long						bitPos=189;
	CPSS_PCL_RULE_SIZE_ENT				ruleSize;
	/* table cpssDxChPclRule global variables */
	CPSS_DXCH_PCL_RULE_FORMAT_UNT		*maskData=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_UNT		*patternData=NULL;
	CPSS_DXCH_PCL_ACTION_STC			*actionPtr=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT	ruleFormat;
	CPSS_PACKET_CMD_ENT 				cmdPkd;
	
	CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC *maskExtNotIpv6 = NULL,*patternExtNotIpv6 = NULL;
	unsigned int devNum = 0;
	result = prvCpssDrvHwPpGetRegField(0,0x58,4,5,&devNum);
	if(result!=0)
			nam_syslog_dbg("prvCpssDrvHwPpGetRegField ERROR!\n");

	maskData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(maskData==NULL){
		nam_syslog_dbg("malloc fail!\n");
		return 1;
	}
	patternData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(patternData==NULL){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		maskData=NULL;
		return 1;
	}
	actionPtr =(CPSS_DXCH_PCL_ACTION_STC*)malloc(sizeof(CPSS_DXCH_PCL_ACTION_STC));
	if(actionPtr==NULL){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		free(patternData);
		maskData=NULL;
		patternData=NULL;
		return 1;
	}

	memset(maskData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(patternData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(actionPtr,0,sizeof(CPSS_DXCH_PCL_ACTION_STC));	

	maskExtNotIpv6		= &(maskData->ruleExtNotIpv6);
	patternExtNotIpv6	= &(patternData->ruleExtNotIpv6);

	appDemoDevAmount = nam_asic_get_instance_num(); 
	
	ipty = node->TcamPtr.packetType;

	switch(ipty)
		{
			case ACL_TCAM_RULE_PACKETTYPE_UDP:		nodeIpProtocal=17;break;
			case ACL_TCAM_RULE_PACKETTYPE_TCP:		nodeIpProtocal=6;break;
		
			default:
					  break;
		}

	pkd=node->ActionPtr.actionType;

	switch(pkd)
		{
			case ACL_ACTION_TYPE_PERMIT:			 cmdPkd=CPSS_PACKET_CMD_FORWARD_E; break;
			case ACL_ACTION_TYPE_DENY:				 cmdPkd=CPSS_PACKET_CMD_DROP_HARD_E;break;
			case ACL_ACTION_TYPE_TRAP_TO_CPU:		 cmdPkd=CPSS_PACKET_CMD_TRAP_TO_CPU_E;break;
			case ACL_ACTION_TYPE_MIRROR_TO_ANALYZER: cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;
			case ACL_ACTION_TYPE_REDIRECT:			 cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;	 
			default:
					  break;
		}
	
	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++)
	{		
		
	/* map input arguments to locals */
		/*devNum;*/
		/* use for save 0 1 for udp */
		if (node->ruleIndex >= (MAX_ACL_RULE_NUMBER/2)) {
			ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU*2;
		}
		else {
			ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU;
		}
		
		ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
		
		maskExtNotIpv6->common.pclId = 0x3FF;
		maskExtNotIpv6->common.sourcePort = node->TcamPtr.ruleInfo.extNotIpv6.mask.maskPortNo;
		maskExtNotIpv6->common.vid = node->TcamPtr.ruleInfo.extNotIpv6.mask.maskVlanId;
		maskExtNotIpv6->common.isIp = 1;

		maskExtNotIpv6->commonExt.l4Byte0=((node->TcamPtr.ruleInfo.extNotIpv6.mask.maskSrcPort)&0xff00)>>8;
		maskExtNotIpv6->commonExt.l4Byte1=(node->TcamPtr.ruleInfo.extNotIpv6.mask.maskSrcPort)&0x00ff;
        	maskExtNotIpv6->commonExt.l4Byte2=((node->TcamPtr.ruleInfo.extNotIpv6.mask.maskDstPort)&0xff00)>>8;
		maskExtNotIpv6->commonExt.l4Byte3=(node->TcamPtr.ruleInfo.extNotIpv6.mask.maskDstPort)&0x00ff;		
       	maskExtNotIpv6->commonExt.ipProtocol = 255;
		maskExtNotIpv6->sip.u32Ip = node->TcamPtr.ruleInfo.extNotIpv6.mask.maskSip;
#ifdef DRV_LIB_CPSS_3_4_XXX
		ACL_SWAB32(maskExtNotIpv6->sip.u32Ip);
#endif
		maskExtNotIpv6->dip.u32Ip = node->TcamPtr.ruleInfo.extNotIpv6.mask.maskDip;
#ifdef DRV_LIB_CPSS_3_4_XXX
		ACL_SWAB32(maskExtNotIpv6->dip.u32Ip);
#endif		
		for(i=0;i<6;i++){
			maskExtNotIpv6->macSa.arEther[i]=node->TcamPtr.ruleInfo.extNotIpv6.mask.maskSmac[i];
			patternExtNotIpv6->macSa.arEther[i]=node->TcamPtr.ruleInfo.extNotIpv6.rule.sMac[i];
			maskExtNotIpv6->macDa.arEther[i]=node->TcamPtr.ruleInfo.extNotIpv6.mask.maskDmac[i];
			patternExtNotIpv6->macDa.arEther[i]=node->TcamPtr.ruleInfo.extNotIpv6.rule.dMac[i];
		}

		patternExtNotIpv6->common.pclId = aclId;
		if (ACL_ANY_PORT_CHAR == (node->TcamPtr.ruleInfo.extNotIpv6.rule.portNo)) {
			patternExtNotIpv6->common.sourcePort = 0;
		}
		else {
			patternExtNotIpv6->common.sourcePort = node->TcamPtr.ruleInfo.extNotIpv6.rule.portNo;
		}	
		patternExtNotIpv6->common.vid = node->TcamPtr.ruleInfo.extNotIpv6.rule.vlanId;
        	patternExtNotIpv6->common.isIp = 1;
       	patternExtNotIpv6->commonExt.l4Byte0=((node->TcamPtr.ruleInfo.extNotIpv6.rule.SrcPort)&0xff00)>>8;
		patternExtNotIpv6->commonExt.l4Byte1=(node->TcamPtr.ruleInfo.extNotIpv6.rule.SrcPort)&0x00ff;
      		patternExtNotIpv6->commonExt.l4Byte2=((node->TcamPtr.ruleInfo.extNotIpv6.rule.DstPort)&0xff00)>>8;
		patternExtNotIpv6->commonExt.l4Byte3=(node->TcamPtr.ruleInfo.extNotIpv6.rule.DstPort)&0x00ff;		
       	patternExtNotIpv6->commonExt.ipProtocol = nodeIpProtocal;
		patternExtNotIpv6->sip.u32Ip = node->TcamPtr.ruleInfo.extNotIpv6.rule.Sip;
#ifdef DRV_LIB_CPSS_3_4_XXX
		ACL_SWAB32(patternExtNotIpv6->sip.u32Ip);
#endif	
		patternExtNotIpv6->dip.u32Ip = node->TcamPtr.ruleInfo.extNotIpv6.rule.Dip;
#ifdef DRV_LIB_CPSS_3_4_XXX
		ACL_SWAB32(patternExtNotIpv6->dip.u32Ip);
#endif	                                                                                    
		actionPtr->pktCmd = cmdPkd;	
		if(ACL_ACTION_TYPE_PERMIT==pkd){
				actionPtr->policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;
				actionPtr->policer.policerId    = node->ActionPtr.policerPtr.policerId ; 
		}
		
		else if(ACL_ACTION_TYPE_MIRROR_TO_ANALYZER==pkd){

			/*actionPtr.policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;*/
			/*actionPtr.policer.policerId    = node->ActionPtr.policerPtr.policerId ; */
	
			 actionPtr->mirror.mirrorToRxAnalyzerPort = 1;			 
		}
		else if(ACL_ACTION_TYPE_REDIRECT==pkd){			
				/*actionPtr.policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;*/
				/*actionPtr.policer.policerId    = node->ActionPtr.policerPtr.policerId ; */
	
				/*note: only port redirect to egress interface,that's policy switching,no router redirect*/
				 actionPtr->redirect.redirectCmd =  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
				 actionPtr->redirect.data.outIf.outInterface.devPort.devNum = devNum;
				 actionPtr->redirect.data.outIf.outInterface.devPort.portNum = node->ActionPtr.actionInfo.redirect.portNum;
				 /*actionPtr.redirect.data.outIf.outInterface.vlanId = 1;*/
		}
		else if(ACL_ACTION_TYPE_TRAP_TO_CPU	== pkd) {
			actionPtr->mirror.cpuCode =node->ActionPtr.actionInfo.trap.cpucode;
		}
		/*QOS*/
		
		actionPtr->qos.modifyDscp=(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyDscp);
		actionPtr->qos.modifyUp= (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyUp);
		actionPtr->qos.qos.ingress.profileIndex = node->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
		if(ACL_ACTION_TYPE_PERMIT == pkd && 0 == actionPtr->policer.policerEnable){
			actionPtr->qos.qos.ingress.profileAssignIndex = 1;
		}
		else{
			actionPtr->qos.qos.ingress.profileAssignIndex = 0;
		}
		actionPtr->qos.qos.ingress.profilePrecedence	= (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)(node->ActionPtr.actionInfo.qos.qos.ingress.profilePrecedence);
		

		/* call cpss api function */

		ruleSize=CPSS_PCL_RULE_SIZE_EXT_E;
		
		result = cpssDxChPclRuleSet(devIdx, ruleFormat, ruleIndex, maskData,
													patternData, actionPtr);		
		if(result!=0)
			nam_syslog_dbg("cpssDxChPclRuleSet ERROR!\n");
		nam_syslog_dbg("cpssDxChPclRuleSet result is %d\n",result);
				
	}
	
	free(maskData);
	free(patternData);
	free(actionPtr);	
	maskData=NULL;
	patternData=NULL;
	actionPtr=NULL;

	return result;
}


unsigned int nam_drv_egress_acl_extNotIpv6_ListDeal
(
	struct ACCESS_LIST_STC * node,
	unsigned int aclId, 
	int unit
)
{
	unsigned char						devIdx = 0,appDemoDevAmount = 0;
	unsigned long						ruleIndex = 0;
	unsigned long						GetIndex = 0;	
	unsigned int						i = 0,ipty = 0,result = 0;
	unsigned long						pkd = 0,nodeIpProtocal = 0;
	unsigned int						maskPtr[12] = {0};
	unsigned int						patternPtr[12] = {0};
	unsigned int						actionptr[3] = {0}; 
	unsigned long						bitPos=189;
	CPSS_PCL_RULE_SIZE_ENT				ruleSize;
	/* table cpssDxChPclRule global variables */
	CPSS_DXCH_PCL_RULE_FORMAT_UNT		*maskData=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_UNT		*patternData=NULL;
	CPSS_DXCH_PCL_ACTION_STC			*actionPtr=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT	ruleFormat;
	CPSS_PACKET_CMD_ENT 				cmdPkd;
	
	CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_NOT_IPV6_STC *maskExtNotIpv6 = NULL,*patternExtNotIpv6 = NULL;

	unsigned int devNum = 0;
	result = prvCpssDrvHwPpGetRegField(0,0x58,4,5,&devNum);
	if(result!=0)
			nam_syslog_dbg("prvCpssDrvHwPpGetRegField ERROR!\n");
	maskData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(maskData==NULL){
		nam_syslog_dbg("malloc fail!\n");
		return 1;
	}
	patternData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(patternData==NULL){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		maskData=NULL;
		return 1;
	}
	actionPtr =(CPSS_DXCH_PCL_ACTION_STC*)malloc(sizeof(CPSS_DXCH_PCL_ACTION_STC));
	if(actionPtr==NULL){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		free(patternData);
		maskData=NULL;
		patternData=NULL;
		return 1;
	}

	memset(maskData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(patternData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(actionPtr,0,sizeof(CPSS_DXCH_PCL_ACTION_STC));	

	maskExtNotIpv6		= &(maskData->ruleEgrExtNotIpv6);
	patternExtNotIpv6	= &(patternData->ruleEgrExtNotIpv6);

	appDemoDevAmount = nam_asic_get_instance_num(); 
	
	ipty = node->TcamPtr.packetType;

	switch(ipty)
		{
			case ACL_TCAM_RULE_PACKETTYPE_UDP:		nodeIpProtocal=17;break;
			case ACL_TCAM_RULE_PACKETTYPE_TCP:		nodeIpProtocal=6;break;
		
			default:
					  break;
		}

	pkd=node->ActionPtr.actionType;

	switch(pkd)
		{
			case ACL_ACTION_TYPE_PERMIT:			 cmdPkd=CPSS_PACKET_CMD_FORWARD_E; break;
			case ACL_ACTION_TYPE_DENY:				 cmdPkd=CPSS_PACKET_CMD_DROP_HARD_E;break;
			case ACL_ACTION_TYPE_TRAP_TO_CPU:		 cmdPkd=CPSS_PACKET_CMD_TRAP_TO_CPU_E;break;
			case ACL_ACTION_TYPE_MIRROR_TO_ANALYZER: cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;
			case ACL_ACTION_TYPE_REDIRECT:			 cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;	 
			default:
					  break;
		}
	
	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++)
	{		
		
	/* map input arguments to locals */
		/*devNum;*/
		/* use for save 0 1 for udp */
		if (node->ruleIndex >= (MAX_ACL_RULE_NUMBER/2)) {
			ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU*2;
		}
		else {
			ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU;
		}
		
		ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E;		
		
		maskExtNotIpv6->common.pclId = 0x3FF;
		maskExtNotIpv6->common.sourcePort = node->TcamPtr.ruleInfo.extNotIpv6.mask.maskPortNo;
		maskExtNotIpv6->common.vid = node->TcamPtr.ruleInfo.extNotIpv6.mask.maskVlanId;
		maskExtNotIpv6->common.isIp = 1;

		maskExtNotIpv6->commonExt.l4Byte0=((node->TcamPtr.ruleInfo.extNotIpv6.mask.maskSrcPort)&0xff00)>>8;
		maskExtNotIpv6->commonExt.l4Byte1=(node->TcamPtr.ruleInfo.extNotIpv6.mask.maskSrcPort)&0x00ff;
        	maskExtNotIpv6->commonExt.l4Byte2=((node->TcamPtr.ruleInfo.extNotIpv6.mask.maskDstPort)&0xff00)>>8;
		maskExtNotIpv6->commonExt.l4Byte3=(node->TcamPtr.ruleInfo.extNotIpv6.mask.maskDstPort)&0x00ff;		
       	maskExtNotIpv6->commonExt.ipProtocol = 255;
		maskExtNotIpv6->sip.u32Ip = node->TcamPtr.ruleInfo.extNotIpv6.mask.maskSip;
#ifdef DRV_LIB_CPSS_3_4_XXX
		ACL_SWAB32(maskExtNotIpv6->sip.u32Ip);
#endif
		maskExtNotIpv6->dip.u32Ip = node->TcamPtr.ruleInfo.extNotIpv6.mask.maskDip;
#ifdef DRV_LIB_CPSS_3_4_XXX
		ACL_SWAB32(maskExtNotIpv6->dip.u32Ip);
#endif		
		for(i=0;i<6;i++){
			maskExtNotIpv6->macSa.arEther[i]=node->TcamPtr.ruleInfo.extNotIpv6.mask.maskSmac[i];
			patternExtNotIpv6->macSa.arEther[i]=node->TcamPtr.ruleInfo.extNotIpv6.rule.sMac[i];
			maskExtNotIpv6->macDa.arEther[i]=node->TcamPtr.ruleInfo.extNotIpv6.mask.maskDmac[i];
			patternExtNotIpv6->macDa.arEther[i]=node->TcamPtr.ruleInfo.extNotIpv6.rule.dMac[i];
		}

		patternExtNotIpv6->common.pclId = aclId;
		if (ACL_ANY_PORT_CHAR == (node->TcamPtr.ruleInfo.extNotIpv6.rule.portNo)) {
			patternExtNotIpv6->common.sourcePort = 0;
		}
		else {
			patternExtNotIpv6->common.sourcePort = node->TcamPtr.ruleInfo.extNotIpv6.rule.portNo;
		}	
		patternExtNotIpv6->common.vid = node->TcamPtr.ruleInfo.extNotIpv6.rule.vlanId;
        	patternExtNotIpv6->common.isIp = 1;
       	patternExtNotIpv6->commonExt.l4Byte0=((node->TcamPtr.ruleInfo.extNotIpv6.rule.SrcPort)&0xff00)>>8;
		patternExtNotIpv6->commonExt.l4Byte1=(node->TcamPtr.ruleInfo.extNotIpv6.rule.SrcPort)&0x00ff;
      		patternExtNotIpv6->commonExt.l4Byte2=((node->TcamPtr.ruleInfo.extNotIpv6.rule.DstPort)&0xff00)>>8;
		patternExtNotIpv6->commonExt.l4Byte3=(node->TcamPtr.ruleInfo.extNotIpv6.rule.DstPort)&0x00ff;		
       	patternExtNotIpv6->commonExt.ipProtocol = nodeIpProtocal;
		patternExtNotIpv6->sip.u32Ip = node->TcamPtr.ruleInfo.extNotIpv6.rule.Sip;
#ifdef DRV_LIB_CPSS_3_4_XXX
		ACL_SWAB32(patternExtNotIpv6->sip.u32Ip);
#endif	
		patternExtNotIpv6->dip.u32Ip = node->TcamPtr.ruleInfo.extNotIpv6.rule.Dip;
#ifdef DRV_LIB_CPSS_3_4_XXX
		ACL_SWAB32(patternExtNotIpv6->dip.u32Ip);
#endif	                                                                                    
		actionPtr->pktCmd = cmdPkd;	
		if(ACL_ACTION_TYPE_PERMIT==pkd){
				actionPtr->policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;
				actionPtr->policer.policerId    = node->ActionPtr.policerPtr.policerId ; 
		}
		
		else if(ACL_ACTION_TYPE_MIRROR_TO_ANALYZER==pkd){

			/*actionPtr.policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;*/
			/*actionPtr.policer.policerId    = node->ActionPtr.policerPtr.policerId ; */
	
			 actionPtr->mirror.mirrorToRxAnalyzerPort = 1;			 
		}
		else if(ACL_ACTION_TYPE_REDIRECT==pkd){			
				/*actionPtr.policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;*/
				/*actionPtr.policer.policerId    = node->ActionPtr.policerPtr.policerId ; */
	
				/*note: only port redirect to egress interface,that's policy switching,no router redirect*/
				 actionPtr->redirect.redirectCmd =  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
				 actionPtr->redirect.data.outIf.outInterface.devPort.devNum = devNum;
				 actionPtr->redirect.data.outIf.outInterface.devPort.portNum = node->ActionPtr.actionInfo.redirect.portNum;
				 /*actionPtr.redirect.data.outIf.outInterface.vlanId = 1;*/
		}
		else if(ACL_ACTION_TYPE_TRAP_TO_CPU	== pkd) {
			actionPtr->mirror.cpuCode =node->ActionPtr.actionInfo.trap.cpucode;
		}
		/*QOS*/
		
		actionPtr->qos.modifyDscp=(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyDscp);
		actionPtr->qos.modifyUp= (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyUp);
		actionPtr->qos.qos.ingress.profileIndex = node->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
		actionPtr->qos.qos.ingress.profileAssignIndex = 1;
		actionPtr->qos.qos.ingress.profilePrecedence	= (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)(node->ActionPtr.actionInfo.qos.qos.ingress.profilePrecedence);
		

		/* call cpss api function */

		ruleSize=CPSS_PCL_RULE_SIZE_EXT_E;
		
		result = cpssDxChPclRuleSet(devIdx, ruleFormat, ruleIndex, maskData,
													patternData, actionPtr);		
		if(result!=0)
			nam_syslog_dbg("cpssDxChPclRuleSet ERROR!\n");
		nam_syslog_dbg("cpssDxChPclRuleSet result is %d\n",result);
				
	}
	
	free(maskData);
	free(patternData);
	free(actionPtr);	
	maskData=NULL;
	patternData=NULL;
	actionPtr=NULL;

	return result;
}

unsigned int nam_drv_ingress_acl_extIpv6_ListDeal
(
	struct ACCESS_LIST_STC * node,
	unsigned int aclId, 
	int unit
)
{
	unsigned char						devIdx = 0,appDemoDevAmount = 0;
	unsigned long						ruleIndex = 0;
	unsigned long						GetIndex = 0;	
	unsigned int						i = 0,ipty = 0,result = 0;
	unsigned long						pkd = 0,nodeIpProtocal = 0;
	unsigned int						maskPtr[12] = {0};
	unsigned int						patternPtr[12] = {0};
	unsigned int						actionptr[3] = {0}; 
	unsigned long						bitPos=189;
	CPSS_PCL_RULE_SIZE_ENT				ruleSize;
	/* table cpssDxChPclRule global variables */
	CPSS_DXCH_PCL_RULE_FORMAT_UNT		*maskData=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_UNT		*patternData=NULL;
	CPSS_DXCH_PCL_ACTION_STC			*actionPtr=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT	ruleFormat;
	/*code optimize:  Uninitialized scalar variable 
	zhangcl@autelan.com 2013-1-17*/
	CPSS_PACKET_CMD_ENT				    cmdPkd = CPSS_PACKET_CMD_FORWARD_E;	
	
	CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L4_STC *maskExtIpv6 = NULL, *patternExtIpv6 = NULL;
	unsigned int devNum = 0;
	result = prvCpssDrvHwPpGetRegField(0,0x58,4,5,&devNum);
	if(result!=0)
			nam_syslog_dbg("prvCpssDrvHwPpGetRegField ERROR!\n");

	maskData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(maskData==NULL){
		nam_syslog_dbg("malloc fail!\n");
		return 1;
	}
	patternData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(patternData==NULL){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		maskData=NULL;
		return 1;
	}
	actionPtr =(CPSS_DXCH_PCL_ACTION_STC*)malloc(sizeof(CPSS_DXCH_PCL_ACTION_STC));
	if(actionPtr==NULL){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		free(patternData);
		maskData=NULL;
		patternData=NULL;
		return 1;
	}

	memset(maskData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(patternData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(actionPtr,0,sizeof(CPSS_DXCH_PCL_ACTION_STC));	

	maskExtIpv6		= &(maskData->ruleExtNotIpv6);
	patternExtIpv6	= &(patternData->ruleExtNotIpv6);

	appDemoDevAmount = nam_asic_get_instance_num(); 

	nodeIpProtocal = node->TcamPtr.nextheader;
/*	ipty = node->TcamPtr.packetType;

	switch(ipty)
		{
			case ACL_TCAM_RULE_PACKETTYPE_UDP:		nodeIpProtocal=17;break;
			case ACL_TCAM_RULE_PACKETTYPE_TCP:		nodeIpProtocal=6;break;
		
			default:
					  break;
		}
*/
	pkd=node->ActionPtr.actionType;

	switch(pkd) {
		case ACL_ACTION_TYPE_PERMIT:			 cmdPkd=CPSS_PACKET_CMD_FORWARD_E; break;
		case ACL_ACTION_TYPE_DENY:				 cmdPkd=CPSS_PACKET_CMD_DROP_HARD_E;break;
		case ACL_ACTION_TYPE_TRAP_TO_CPU:		 cmdPkd=CPSS_PACKET_CMD_TRAP_TO_CPU_E;break;
		case ACL_ACTION_TYPE_MIRROR_TO_ANALYZER: cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;
		case ACL_ACTION_TYPE_REDIRECT:			 cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;	 
		default:
				  break;
	}
	
	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++) {		
		
	/* map input arguments to locals */
		/*devNum;*/
		/* use for save 0 1 for udp */
		if (node->ruleIndex >= (MAX_ACL_RULE_NUMBER/2)) {
			ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU*2;
		}
		else {
			ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU;
		}
		ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E;

		maskExtIpv6->common.pclId = 0x3FF;
		maskExtIpv6->common.isIp = 1;
		
		maskExtIpv6->commonExt.isIpv6 = 1;
		maskExtIpv6->commonExt.l4Byte0=((node->TcamPtr.ruleInfo.extIpv6L4.mask.maskSrcPort)&0xff00)>>8;
		maskExtIpv6->commonExt.l4Byte1=(node->TcamPtr.ruleInfo.extIpv6L4.mask.maskSrcPort)&0x00ff;
        maskExtIpv6->commonExt.l4Byte2=((node->TcamPtr.ruleInfo.extIpv6L4.mask.maskDstPort)&0xff00)>>8;
		maskExtIpv6->commonExt.l4Byte3=(node->TcamPtr.ruleInfo.extIpv6L4.mask.maskDstPort)&0x00ff;		
       	maskExtIpv6->commonExt.ipProtocol = 255;
		memcpy(&(maskExtIpv6->sip.arIP), &(node->TcamPtr.ruleInfo.extIpv6L4.mask.maskSip.ipbuf), 16);
		memcpy(&(maskExtIpv6->dip.arIP), &(node->TcamPtr.ruleInfo.extIpv6L4.mask.maskDip.ipbuf), 16);;

		patternExtIpv6->common.pclId = aclId;
        patternExtIpv6->common.isIp = 1;

		patternExtIpv6->commonExt.isIpv6 = 1;
       	patternExtIpv6->commonExt.l4Byte0=((node->TcamPtr.ruleInfo.extIpv6L4.rule.SrcPort)&0xff00)>>8;
		patternExtIpv6->commonExt.l4Byte1=(node->TcamPtr.ruleInfo.extIpv6L4.rule.SrcPort)&0x00ff;
      	patternExtIpv6->commonExt.l4Byte2=((node->TcamPtr.ruleInfo.extIpv6L4.rule.DstPort)&0xff00)>>8;
		patternExtIpv6->commonExt.l4Byte3=(node->TcamPtr.ruleInfo.extIpv6L4.rule.DstPort)&0x00ff;		
       	patternExtIpv6->commonExt.ipProtocol = nodeIpProtocal;
		memcpy(&(patternExtIpv6->sip.arIP), &(node->TcamPtr.ruleInfo.extIpv6L4.rule.Sip.ipbuf), 16);
		memcpy(&(patternExtIpv6->dip.arIP), &(node->TcamPtr.ruleInfo.extIpv6L4.rule.Dip.ipbuf), 16);
		
		actionPtr->pktCmd = cmdPkd;	
		if(ACL_ACTION_TYPE_PERMIT==pkd){
				actionPtr->policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;
				actionPtr->policer.policerId    = node->ActionPtr.policerPtr.policerId ; 
		}
		
		else if(ACL_ACTION_TYPE_MIRROR_TO_ANALYZER==pkd){

			/*actionPtr.policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;*/
			/*actionPtr.policer.policerId    = node->ActionPtr.policerPtr.policerId ; */
	
			 actionPtr->mirror.mirrorToRxAnalyzerPort = 1;			 
		}
		else if(ACL_ACTION_TYPE_REDIRECT==pkd){			
				/*actionPtr.policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;*/
				/*actionPtr.policer.policerId    = node->ActionPtr.policerPtr.policerId ; */
	
				/*note: only port redirect to egress interface,that's policy switching,no router redirect*/
				 actionPtr->redirect.redirectCmd =  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
				 actionPtr->redirect.data.outIf.outInterface.devPort.devNum = devNum;
				 actionPtr->redirect.data.outIf.outInterface.devPort.portNum = node->ActionPtr.actionInfo.redirect.portNum;
				 /*actionPtr.redirect.data.outIf.outInterface.vlanId = 1;*/
		}
		else if(ACL_ACTION_TYPE_TRAP_TO_CPU	== pkd) {
			actionPtr->mirror.cpuCode =node->ActionPtr.actionInfo.trap.cpucode;
		}		

		/* call cpss api function */

		ruleSize=CPSS_PCL_RULE_SIZE_EXT_E;
		
		result = cpssDxChPclRuleSet(devIdx, ruleFormat, ruleIndex, maskData,
													patternData, actionPtr);		
		if(result!=0)
			nam_syslog_dbg("cpssDxChPclRuleSet ERROR!\n");
		nam_syslog_dbg("cpssDxChPclRuleSet result is %d\n",result);
				
	}
	
	free(maskData);
	free(patternData);
	free(actionPtr);	
	maskData=NULL;
	patternData=NULL;
	actionPtr=NULL;

	return result;
}

unsigned int nam_drv_egress_acl_extIpv6_ListDeal
(
	struct ACCESS_LIST_STC * node,
	unsigned int aclId, 
	int unit
)
{
	unsigned char						devIdx = 0,appDemoDevAmount = 0;
	unsigned long						ruleIndex = 0;
	unsigned long						GetIndex = 0;	
	unsigned int						i = 0,ipty = 0,result = 0;
	unsigned long						pkd = 0,nodeIpProtocal = 0;
	unsigned int						maskPtr[12] = {0};
	unsigned int						patternPtr[12] = {0};
	unsigned int						actionptr[3] = {0}; 
	unsigned long						bitPos=189;
	CPSS_PCL_RULE_SIZE_ENT				ruleSize;
	/* table cpssDxChPclRule global variables */
	CPSS_DXCH_PCL_RULE_FORMAT_UNT		*maskData=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_UNT		*patternData=NULL;
	CPSS_DXCH_PCL_ACTION_STC			*actionPtr=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT	ruleFormat;
	/*code optimize:  Uninitialized scalar variable 
	zhangcl@autelan.com 2013-1-17*/
	CPSS_PACKET_CMD_ENT				    cmdPkd = CPSS_PACKET_CMD_FORWARD_E;	
	CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_IPV6_L4_STC *maskExtIpv6 = NULL, *patternExtIpv6 = NULL;
	unsigned int devNum = 0;
	result = prvCpssDrvHwPpGetRegField(0,0x58,4,5,&devNum);
	if(result!=0)
			nam_syslog_dbg("prvCpssDrvHwPpGetRegField ERROR!\n");

	maskData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(maskData==NULL){
		nam_syslog_dbg("malloc fail!\n");
		return 1;
	}
	patternData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(patternData==NULL){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		maskData=NULL;
		return 1;
	}
	actionPtr =(CPSS_DXCH_PCL_ACTION_STC*)malloc(sizeof(CPSS_DXCH_PCL_ACTION_STC));
	if(actionPtr==NULL){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		free(patternData);
		maskData=NULL;
		patternData=NULL;
		return 1;
	}

	memset(maskData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(patternData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(actionPtr,0,sizeof(CPSS_DXCH_PCL_ACTION_STC));	

	maskExtIpv6		= &(maskData->ruleEgrExtIpv6L4);
	patternExtIpv6	= &(patternData->ruleEgrExtIpv6L4);

	appDemoDevAmount = nam_asic_get_instance_num(); 
	nodeIpProtocal = node->TcamPtr.nextheader;
/*	ipty = node->TcamPtr.packetType;

	switch(ipty)
		{
			case ACL_TCAM_RULE_PACKETTYPE_UDP:		nodeIpProtocal=17;break;
			case ACL_TCAM_RULE_PACKETTYPE_TCP:		nodeIpProtocal=6;break;
		
			default:
					  break;
		}
*/
	pkd=node->ActionPtr.actionType;
	switch(pkd) {
		case ACL_ACTION_TYPE_PERMIT:			 cmdPkd=CPSS_PACKET_CMD_FORWARD_E; break;
		case ACL_ACTION_TYPE_DENY:				 cmdPkd=CPSS_PACKET_CMD_DROP_HARD_E;break;
		case ACL_ACTION_TYPE_TRAP_TO_CPU:		 cmdPkd=CPSS_PACKET_CMD_TRAP_TO_CPU_E;break;
		case ACL_ACTION_TYPE_MIRROR_TO_ANALYZER: cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;
		case ACL_ACTION_TYPE_REDIRECT:			 cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;	 
		default:
				  break;
	}
	
	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++) {		
	/* map input arguments to locals */
		/*devNum;*/
		/* use for save 0 1 for udp */
		if (node->ruleIndex >= (MAX_ACL_RULE_NUMBER/2)) {
			ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU*2;
		}
		else {
			ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU;
		}
		ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E;		
		maskExtIpv6->common.pclId = 0x3FF;
		maskExtIpv6->common.isIp = 1;
		
		maskExtIpv6->commonExt.isIpv6 = 1;
		maskExtIpv6->commonExt.l4Byte0=((node->TcamPtr.ruleInfo.extIpv6L4.mask.maskSrcPort)&0xff00)>>8;
		maskExtIpv6->commonExt.l4Byte1=(node->TcamPtr.ruleInfo.extIpv6L4.mask.maskSrcPort)&0x00ff;
        maskExtIpv6->commonExt.l4Byte2=((node->TcamPtr.ruleInfo.extIpv6L4.mask.maskDstPort)&0xff00)>>8;
		maskExtIpv6->commonExt.l4Byte3=(node->TcamPtr.ruleInfo.extIpv6L4.mask.maskDstPort)&0x00ff;		
       	maskExtIpv6->commonExt.ipProtocol = 255;
		memcpy(&(maskExtIpv6->sip.arIP), &(node->TcamPtr.ruleInfo.extIpv6L4.mask.maskSip.ipbuf), 16);
		memcpy(&(maskExtIpv6->dip.arIP), &(node->TcamPtr.ruleInfo.extIpv6L4.mask.maskDip.ipbuf), 16);;

		patternExtIpv6->common.pclId = aclId;
        patternExtIpv6->common.isIp = 1;

		patternExtIpv6->commonExt.isIpv6 = 1;
       	patternExtIpv6->commonExt.l4Byte0=((node->TcamPtr.ruleInfo.extIpv6L4.rule.SrcPort)&0xff00)>>8;
		patternExtIpv6->commonExt.l4Byte1=(node->TcamPtr.ruleInfo.extIpv6L4.rule.SrcPort)&0x00ff;
      	patternExtIpv6->commonExt.l4Byte2=((node->TcamPtr.ruleInfo.extIpv6L4.rule.DstPort)&0xff00)>>8;
		patternExtIpv6->commonExt.l4Byte3=(node->TcamPtr.ruleInfo.extIpv6L4.rule.DstPort)&0x00ff;		
       	patternExtIpv6->commonExt.ipProtocol = nodeIpProtocal;
		memcpy(&(patternExtIpv6->sip.arIP), &(node->TcamPtr.ruleInfo.extIpv6L4.rule.Sip.ipbuf), 16);
		memcpy(&(patternExtIpv6->dip.arIP), &(node->TcamPtr.ruleInfo.extIpv6L4.rule.Dip.ipbuf), 16);
		
		actionPtr->pktCmd = cmdPkd;	
		if(ACL_ACTION_TYPE_PERMIT==pkd){
				actionPtr->policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;
				actionPtr->policer.policerId    = node->ActionPtr.policerPtr.policerId ; 
		}
		
		else if(ACL_ACTION_TYPE_MIRROR_TO_ANALYZER==pkd){

			/*actionPtr.policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;*/
			/*actionPtr.policer.policerId    = node->ActionPtr.policerPtr.policerId ; */
	
			 actionPtr->mirror.mirrorToRxAnalyzerPort = 1;			 
		}
		else if(ACL_ACTION_TYPE_REDIRECT==pkd){			
				/*actionPtr.policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;*/
				/*actionPtr.policer.policerId    = node->ActionPtr.policerPtr.policerId ; */
	
				/*note: only port redirect to egress interface,that's policy switching,no router redirect*/
				 actionPtr->redirect.redirectCmd =  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
				 actionPtr->redirect.data.outIf.outInterface.devPort.devNum = devNum;
				 actionPtr->redirect.data.outIf.outInterface.devPort.portNum = node->ActionPtr.actionInfo.redirect.portNum;
				 /*actionPtr.redirect.data.outIf.outInterface.vlanId = 1;*/
		}
		else if(ACL_ACTION_TYPE_TRAP_TO_CPU	== pkd) {
			actionPtr->mirror.cpuCode =node->ActionPtr.actionInfo.trap.cpucode;
		}		

		/* call cpss api function */

		ruleSize=CPSS_PCL_RULE_SIZE_EXT_E;
		result = cpssDxChPclRuleSet(devIdx, ruleFormat, ruleIndex, maskData,
													patternData, actionPtr);		
		if(result!=0)
			nam_syslog_dbg("cpssDxChPclRuleSet ERROR!\n");
				
	}
	
	free(maskData);
	free(patternData);
	free(actionPtr);	
	maskData=NULL;
	patternData=NULL;
	actionPtr=NULL;

	return result;
}

unsigned int nam_drv_acl_extIpv4L4_ListDeal
(
	struct ACCESS_LIST_STC *node,
	unsigned int aclId, 
	int unit,
	unsigned int dir_info
)
{
	unsigned int result = 0;
	if(dir_info){
		result = nam_drv_egress_acl_extIpv4L4_ListDeal(node,aclId,unit);
	}
	else{
		result = nam_drv_ingress_acl_extIpv4L4_ListDeal(node,aclId,unit);
	}
	return result;
}

/*RULE_INGRESS STDIPV4L4_DATA_DEAL*/
unsigned int nam_drv_ingress_acl_extIpv4L4_ListDeal
(
	struct ACCESS_LIST_STC *node,
	unsigned int aclId, 
	int unit
)
{
	unsigned char 						devIdx = 0;
	unsigned long 						ruleIndex = 0;
	unsigned long 						GetIndex = 0;	
	unsigned int		 				ipty = 0,result = 0;
	unsigned long           			pkd=ACL_ACTION_TYPE_PERMIT,nodeIpProtocal=0;
	unsigned int           				maskPtr[12] = {0};
	unsigned int           				patternPtr[12] = {0};
	unsigned int           				actionptr[3] = {0};	
	unsigned long 						bitPos=189;	
	unsigned int						appDemoDevAmount=0;
	CPSS_PCL_RULE_SIZE_ENT  			ruleSize;
	/* table cpssDxChPclRule global variables */
	CPSS_DXCH_PCL_RULE_FORMAT_UNT	    *maskData=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_UNT	    *patternData=NULL;
	CPSS_DXCH_PCL_ACTION_STC		    *actionPtr=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
	/*code optimize:  Uninitialized scalar variable 
	zhangcl@autelan.com 2013-1-17*/
	CPSS_PACKET_CMD_ENT				    cmdPkd = CPSS_PACKET_CMD_FORWARD_E;	
	CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC *maskStdIpv4L4 = NULL,*patternStdIpv4L4 = NULL;
	unsigned char			   redirectPort=0;

	unsigned int devNum = 0;
	result = prvCpssDrvHwPpGetRegField(0,0x58,4,5,&devNum);
	if(result!=0)
			nam_syslog_dbg("prvCpssDrvHwPpGetRegField ERROR!\n");
	appDemoDevAmount = nam_asic_get_instance_num(); 

	maskData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(NULL==maskData){
		nam_syslog_dbg("malloc fail!\n");
		return 1;
	}
	patternData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(NULL==patternData){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		maskData=NULL;
		return 1;
		
	}
	actionPtr =(CPSS_DXCH_PCL_ACTION_STC*)malloc(sizeof(CPSS_DXCH_PCL_ACTION_STC));
	if(NULL==actionPtr){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		free(patternData);
		maskData=NULL;
		patternData=NULL;
		return 1;
	}

	memset(maskData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(patternData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(actionPtr,0,sizeof(CPSS_DXCH_PCL_ACTION_STC));	


	maskStdIpv4L4 		=&(maskData->ruleExtNotIpv6);
	patternStdIpv4L4	= &(patternData->ruleExtNotIpv6);

	
	ipty = node->TcamPtr.packetType;

	switch(ipty)
		{
			case ACL_TCAM_RULE_PACKETTYPE_IP:		nodeIpProtocal=0;break;
			case ACL_TCAM_RULE_PACKETTYPE_UDP:		nodeIpProtocal=17;break;
			case ACL_TCAM_RULE_PACKETTYPE_TCP:		nodeIpProtocal=6;break;
			case ACL_TCAM_RULE_PACKETTYPE_ICMP: 	nodeIpProtocal=1;break;
			case ACL_TCAM_RULE_PACKETTYPE_ETHERNET: nodeIpProtocal=0;break;
			default:
					  break;
		}

	pkd=node->ActionPtr.actionType;

	switch(pkd)
		{
			case ACL_ACTION_TYPE_PERMIT:			 cmdPkd=CPSS_PACKET_CMD_FORWARD_E; break;
			case ACL_ACTION_TYPE_DENY:				 cmdPkd=CPSS_PACKET_CMD_DROP_HARD_E;break;
			case ACL_ACTION_TYPE_TRAP_TO_CPU:   	 cmdPkd=CPSS_PACKET_CMD_TRAP_TO_CPU_E;break;
		       case ACL_ACTION_TYPE_MIRROR_TO_ANALYZER: cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;
			case ACL_ACTION_TYPE_REDIRECT:			 cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;	 
			default:
					  break;
		}
	
	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++)
	{		
		
		memset(maskData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
		memset(patternData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
		memset(actionPtr,0,sizeof(CPSS_DXCH_PCL_ACTION_STC));	
	/* map input arguments to locals */
		/*devNum;*/
		/* use for save 0 1 for udp */
		if (node->ruleIndex >= (MAX_ACL_RULE_NUMBER/2)) {
			ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU*2;
		}
		else {
			ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU;
		}
		
		ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;
		
		maskStdIpv4L4->common.pclId = 0x3FF;
		maskStdIpv4L4->common.isIp = 1;
		/*maskStdIpv4L4->common.sourcePort = 0;*/
		/*maskStdIpv4L4->commonExt.isL4Valid = 1;*/
		nam_syslog_dbg("maskSip %ld\n",node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSip);
		maskStdIpv4L4->sip.u32Ip = node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSip;
#ifdef DRV_LIB_CPSS_3_4_XXX
		ACL_SWAB32(maskStdIpv4L4->sip.u32Ip);
#endif
		maskStdIpv4L4->dip.u32Ip = node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDip;
#ifdef DRV_LIB_CPSS_3_4_XXX
		ACL_SWAB32(maskStdIpv4L4->dip.u32Ip);
#endif
nam_syslog_dbg("mask sip %#x, dip %#x \n", maskStdIpv4L4->sip.u32Ip, maskStdIpv4L4->dip.u32Ip);
		maskStdIpv4L4->commonExt.l4Byte2=((node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDstPort)&0xff00)>>8;
		maskStdIpv4L4->commonExt.l4Byte3=(node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDstPort)&0x00ff;
		if(nodeIpProtocal==1){
		maskStdIpv4L4->commonExt.l4Byte0=node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskType;
		maskStdIpv4L4->commonExt.l4Byte1=node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskCode;
		}
		else if((nodeIpProtocal==6)||(nodeIpProtocal==17)){
		maskStdIpv4L4->commonExt.l4Byte0=((node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSrcPort)&0xff00)>>8;
		maskStdIpv4L4->commonExt.l4Byte1=(node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSrcPort)&0x00ff;
		}
		if((nodeIpProtocal==1)||(nodeIpProtocal==6)||(nodeIpProtocal==17)){
			maskStdIpv4L4->commonExt.ipProtocol = 255;
			patternStdIpv4L4->commonExt.ipProtocol = nodeIpProtocal;
		}
		else{
			maskStdIpv4L4->commonExt.ipProtocol = 0;
			patternStdIpv4L4->commonExt.ipProtocol = 0;
		}
			
			
		patternStdIpv4L4->common.pclId = aclId;
		patternStdIpv4L4->common.isIp = 1;
		/*patternStdIpv4L4->common.sourcePort = 0;*/
		/*patternStdIpv4L4->commonExt.isL4Valid = 1;*/
		patternStdIpv4L4->sip.u32Ip = node->TcamPtr.ruleInfo.stdIpv4L4.rule.Sip;
#ifdef DRV_LIB_CPSS_3_4_XXX
		ACL_SWAB32(patternStdIpv4L4->sip.u32Ip);
#endif
		patternStdIpv4L4->dip.u32Ip = node->TcamPtr.ruleInfo.stdIpv4L4.rule.Dip;
#ifdef DRV_LIB_CPSS_3_4_XXX
		ACL_SWAB32(patternStdIpv4L4->dip.u32Ip);
#endif
nam_syslog_dbg("acl sip %#x, dip %#x \n", patternStdIpv4L4->sip.u32Ip, patternStdIpv4L4->dip.u32Ip);

		patternStdIpv4L4->commonExt.l4Byte2=((node->TcamPtr.ruleInfo.stdIpv4L4.rule.DstPort)&0xff00)>>8;
		patternStdIpv4L4->commonExt.l4Byte3=(node->TcamPtr.ruleInfo.stdIpv4L4.rule.DstPort)&0x00ff;
		if(nodeIpProtocal==1){
			patternStdIpv4L4->commonExt.l4Byte0=node->TcamPtr.ruleInfo.stdIpv4L4.rule.Type;
			patternStdIpv4L4->commonExt.l4Byte1=node->TcamPtr.ruleInfo.stdIpv4L4.rule.Code;		
		}
		else if((nodeIpProtocal==6)||(nodeIpProtocal==17)){
			patternStdIpv4L4->commonExt.l4Byte0=((node->TcamPtr.ruleInfo.stdIpv4L4.rule.SrcPort)&0xff00)>>8;
			patternStdIpv4L4->commonExt.l4Byte1=(node->TcamPtr.ruleInfo.stdIpv4L4.rule.SrcPort)&0x00ff;
		}
		
		actionPtr->pktCmd = cmdPkd;           
		if(ACL_ACTION_TYPE_PERMIT==pkd){
			actionPtr->policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;
			actionPtr->policer.policerId    = node->ActionPtr.policerPtr.policerId ; 
		}

		else if(ACL_ACTION_TYPE_MIRROR_TO_ANALYZER==pkd){

			/*actionPtr.policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;*/
			/*actionPtr.policer.policerId    = node->ActionPtr.policerPtr.policerId ; */
	
			 actionPtr->mirror.mirrorToRxAnalyzerPort = 1;
		}
		else if(ACL_ACTION_TYPE_REDIRECT==pkd){			
				/*actionPtr.policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;*/
				/*actionPtr.policer.policerId    = node->ActionPtr.policerPtr.policerId ; */
	
				/*note: only port redirect to egress interface,that's policy switching,no router redirect*/
				 actionPtr->redirect.redirectCmd =  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
				#if 0
				 actionPtr->redirect.data.outIf.outInterface.devPort.devNum = 0;
				#endif
				actionPtr->redirect.data.outIf.outInterface.devPort.devNum = devNum;
				 actionPtr->redirect.data.outIf.outInterface.devPort.portNum = node->ActionPtr.actionInfo.redirect.portNum;
				redirectPort = actionPtr->redirect.data.outIf.outInterface.devPort.portNum;
				nam_syslog_dbg(" sw redirectPort %d\n",node->ActionPtr.actionInfo.redirect.portNum);
				nam_syslog_dbg("redirectPort %d\n",redirectPort);
		}
		else if(ACL_ACTION_TYPE_TRAP_TO_CPU	== pkd) {
			actionPtr->mirror.cpuCode =node->ActionPtr.actionInfo.trap.cpucode;
		}
		
		/*qos*/
		
		actionPtr->qos.modifyDscp=(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyDscp);
		actionPtr->qos.modifyUp= (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyUp);
		actionPtr->qos.qos.ingress.profileIndex = node->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
		if(ACL_ACTION_TYPE_PERMIT == pkd && 0 == actionPtr->policer.policerEnable){
			actionPtr->qos.qos.ingress.profileAssignIndex = 1;
		}
		else{
			actionPtr->qos.qos.ingress.profileAssignIndex = 0;
		}
		actionPtr->qos.qos.ingress.profilePrecedence	= (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)(node->ActionPtr.actionInfo.qos.qos.ingress.profilePrecedence);
		
		
	/*	actionPtr->egressPolicy =ACL_TRUE;*/
	/*	actionPtr->qos.qos.egress.dscp = (char)(node->ActionPtr.actionInfo.qos.qos.egress.egrDscp);*/
	/*	actionPtr->qos.qos.egress.up = (char)(node->ActionPtr.actionInfo.qos.qos.egress.egrUp);*/
		
	/*	call cpss api function */

		ruleSize=CPSS_PCL_RULE_SIZE_EXT_E;
		
		result = cpssDxChPclRuleSet(devIdx, ruleFormat, ruleIndex, maskData,
													patternData, actionPtr);		
		if(result!=0)
			nam_syslog_dbg("cpssDxChPclRuleSet ERROR!\n");
		
		nam_syslog_dbg("cpssDxChPclRuleSet result is %d\n",result);
	}		
	
   	free(maskData);
	free(patternData);
	free(actionPtr);
	maskData=NULL;
	patternData=NULL;
	actionPtr=NULL;
	
	return result;
}


/*RULE_EGRESS STDIPV4L4_DATA_DEAL*/
unsigned int nam_drv_egress_acl_extIpv4L4_ListDeal
(
	struct ACCESS_LIST_STC *node,
	unsigned int aclId, 
	int unit
)
{
	unsigned char 						devIdx = 0;
	unsigned long 						ruleIndex = 0;
	unsigned long 						GetIndex = 0;	
	unsigned int		 				ipty = 0,result = 0;
	unsigned long           			pkd=ACL_ACTION_TYPE_PERMIT,nodeIpProtocal=0;
	unsigned int           				maskPtr[12] = {0};
	unsigned int           				patternPtr[12] = {0};
	unsigned int           				actionptr[3] = {0};	
	unsigned long 						bitPos=189;	
	unsigned int						appDemoDevAmount=0;
	CPSS_PCL_RULE_SIZE_ENT  			ruleSize;
	/* table cpssDxChPclRule global variables */
	CPSS_DXCH_PCL_RULE_FORMAT_UNT	    *maskData=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_UNT	    *patternData=NULL;
	CPSS_DXCH_PCL_ACTION_STC		    *actionPtr=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
	/*code optimize:  Uninitialized scalar variable 
	zhangcl@autelan.com 2013-1-17*/
	CPSS_PACKET_CMD_ENT				    cmdPkd = CPSS_PACKET_CMD_FORWARD_E;	
	CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_NOT_IPV6_STC *maskStdIpv4L4 = NULL,*patternStdIpv4L4 = NULL;
	unsigned char			   redirectPort=0;
	unsigned int devNum = 0;
	result = prvCpssDrvHwPpGetRegField(0,0x58,4,5,&devNum);
	if(result!=0)
			nam_syslog_dbg("prvCpssDrvHwPpGetRegField ERROR!\n");

	appDemoDevAmount = nam_asic_get_instance_num(); 

	maskData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(NULL==maskData){
		nam_syslog_dbg("malloc fail!\n");
		return 1;
	}
	patternData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(NULL==patternData){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		maskData=NULL;
		return 1;
		
	}
	actionPtr =(CPSS_DXCH_PCL_ACTION_STC*)malloc(sizeof(CPSS_DXCH_PCL_ACTION_STC));
	if(NULL==actionPtr){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		free(patternData);
		maskData=NULL;
		patternData=NULL;
		return 1;
	}

	memset(maskData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(patternData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(actionPtr,0,sizeof(CPSS_DXCH_PCL_ACTION_STC));	


	maskStdIpv4L4 		=&(maskData->ruleEgrExtNotIpv6);
	patternStdIpv4L4	= &(patternData->ruleEgrExtNotIpv6);

	
	ipty = node->TcamPtr.packetType;

	switch(ipty)
		{
			case ACL_TCAM_RULE_PACKETTYPE_IP:		nodeIpProtocal=0;break;
			case ACL_TCAM_RULE_PACKETTYPE_UDP:		nodeIpProtocal=17;break;
			case ACL_TCAM_RULE_PACKETTYPE_TCP:		nodeIpProtocal=6;break;
			case ACL_TCAM_RULE_PACKETTYPE_ICMP: 	nodeIpProtocal=1;break;
			case ACL_TCAM_RULE_PACKETTYPE_ETHERNET: nodeIpProtocal=0;break;
			default:
					  break;
		}

	pkd=node->ActionPtr.actionType;

	switch(pkd)
		{
			case ACL_ACTION_TYPE_PERMIT:			 cmdPkd=CPSS_PACKET_CMD_FORWARD_E; break;
			case ACL_ACTION_TYPE_DENY:				 cmdPkd=CPSS_PACKET_CMD_DROP_HARD_E;break;
			case ACL_ACTION_TYPE_TRAP_TO_CPU:   	 cmdPkd=CPSS_PACKET_CMD_TRAP_TO_CPU_E;break;
		       case ACL_ACTION_TYPE_MIRROR_TO_ANALYZER: cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;
			case ACL_ACTION_TYPE_REDIRECT:			 cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;	 
			default:
					  break;
		}
	
	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++)
	{		
		
		memset(maskData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
		memset(patternData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
		memset(actionPtr,0,sizeof(CPSS_DXCH_PCL_ACTION_STC));	
	/* map input arguments to locals */
		/*devNum;*/
		/* use for save 0 1 for udp */
		if (node->ruleIndex >= (MAX_ACL_RULE_NUMBER/2)) {
			ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU*2;
		}
		else {
			ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU;
		}
		
		ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E;
		
		maskStdIpv4L4->common.pclId = 0x3FF;
		maskStdIpv4L4->common.isIp = 1;
		/*maskStdIpv4L4->common.sourcePort = 0;*/
		/*maskStdIpv4L4->commonExt.isL4Valid = 1;*/
		nam_syslog_dbg("maskSip %ld\n",node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSip);
		maskStdIpv4L4->sip.u32Ip = node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSip;
#ifdef DRV_LIB_CPSS_3_4_XXX
		ACL_SWAB32(maskStdIpv4L4->sip.u32Ip);
#endif
		maskStdIpv4L4->dip.u32Ip = node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDip;
#ifdef DRV_LIB_CPSS_3_4_XXX
		ACL_SWAB32(maskStdIpv4L4->dip.u32Ip);
#endif
nam_syslog_dbg("mask sip %#x, dip %#x \n", maskStdIpv4L4->sip.u32Ip, maskStdIpv4L4->dip.u32Ip);
		maskStdIpv4L4->commonExt.l4Byte2=((node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDstPort)&0xff00)>>8;
		maskStdIpv4L4->commonExt.l4Byte3=(node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDstPort)&0x00ff;
		if(nodeIpProtocal==1){
		maskStdIpv4L4->commonExt.l4Byte0=node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskType;
		maskStdIpv4L4->commonExt.l4Byte1=node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskCode;
		}
		else if((nodeIpProtocal==6)||(nodeIpProtocal==17)){
		maskStdIpv4L4->commonExt.l4Byte0=((node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSrcPort)&0xff00)>>8;
		maskStdIpv4L4->commonExt.l4Byte1=(node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSrcPort)&0x00ff;
		}
		if((nodeIpProtocal==1)||(nodeIpProtocal==6)||(nodeIpProtocal==17)){
			maskStdIpv4L4->commonExt.ipProtocol = 255;
			patternStdIpv4L4->commonExt.ipProtocol = nodeIpProtocal;
		}
		else{
			maskStdIpv4L4->commonExt.ipProtocol = 0;
			patternStdIpv4L4->commonExt.ipProtocol = 0;
		}
			
			
		patternStdIpv4L4->common.pclId = aclId;
		patternStdIpv4L4->common.isIp = 1;
		/*patternStdIpv4L4->common.sourcePort = 0;*/
		/*patternStdIpv4L4->commonExt.isL4Valid = 1;*/
		patternStdIpv4L4->sip.u32Ip = node->TcamPtr.ruleInfo.stdIpv4L4.rule.Sip;
#ifdef DRV_LIB_CPSS_3_4_XXX
		ACL_SWAB32(patternStdIpv4L4->sip.u32Ip);
#endif
		patternStdIpv4L4->dip.u32Ip = node->TcamPtr.ruleInfo.stdIpv4L4.rule.Dip;
#ifdef DRV_LIB_CPSS_3_4_XXX
		ACL_SWAB32(patternStdIpv4L4->dip.u32Ip);
#endif
nam_syslog_dbg("acl sip %#x, dip %#x \n", patternStdIpv4L4->sip.u32Ip, patternStdIpv4L4->dip.u32Ip);

		patternStdIpv4L4->commonExt.l4Byte2=((node->TcamPtr.ruleInfo.stdIpv4L4.rule.DstPort)&0xff00)>>8;
		patternStdIpv4L4->commonExt.l4Byte3=(node->TcamPtr.ruleInfo.stdIpv4L4.rule.DstPort)&0x00ff;
		if(nodeIpProtocal==1){
			patternStdIpv4L4->commonExt.l4Byte0=node->TcamPtr.ruleInfo.stdIpv4L4.rule.Type;
			patternStdIpv4L4->commonExt.l4Byte1=node->TcamPtr.ruleInfo.stdIpv4L4.rule.Code;		
		}
		else if((nodeIpProtocal==6)||(nodeIpProtocal==17)){
			patternStdIpv4L4->commonExt.l4Byte0=((node->TcamPtr.ruleInfo.stdIpv4L4.rule.SrcPort)&0xff00)>>8;
			patternStdIpv4L4->commonExt.l4Byte1=(node->TcamPtr.ruleInfo.stdIpv4L4.rule.SrcPort)&0x00ff;
		}
		
		actionPtr->pktCmd = cmdPkd;           
		if(ACL_ACTION_TYPE_PERMIT==pkd){
			actionPtr->policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;
			actionPtr->policer.policerId    = node->ActionPtr.policerPtr.policerId ; 
		}

		else if(ACL_ACTION_TYPE_MIRROR_TO_ANALYZER==pkd){

			/*actionPtr.policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;*/
			/*actionPtr.policer.policerId    = node->ActionPtr.policerPtr.policerId ; */
	
			 actionPtr->mirror.mirrorToRxAnalyzerPort = 1;
		}
		else if(ACL_ACTION_TYPE_REDIRECT==pkd){			
				/*actionPtr.policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;*/
				/*actionPtr.policer.policerId    = node->ActionPtr.policerPtr.policerId ; */
	
				/*note: only port redirect to egress interface,that's policy switching,no router redirect*/
				 actionPtr->redirect.redirectCmd =  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
				 actionPtr->redirect.data.outIf.outInterface.devPort.devNum = devNum;
				 actionPtr->redirect.data.outIf.outInterface.devPort.portNum = node->ActionPtr.actionInfo.redirect.portNum;
				redirectPort = actionPtr->redirect.data.outIf.outInterface.devPort.portNum;
				nam_syslog_dbg(" sw redirectPort %d\n",node->ActionPtr.actionInfo.redirect.portNum);
				nam_syslog_dbg("redirectPort %d\n",redirectPort);
		}
		else if(ACL_ACTION_TYPE_TRAP_TO_CPU	== pkd) {
			actionPtr->mirror.cpuCode =node->ActionPtr.actionInfo.trap.cpucode;
		}
		
		/*qos*/
		
		actionPtr->qos.modifyDscp=(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyDscp);
		actionPtr->qos.modifyUp= (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyUp);
		actionPtr->qos.qos.ingress.profileIndex = node->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
		actionPtr->qos.qos.ingress.profileAssignIndex = 1;
		actionPtr->qos.qos.ingress.profilePrecedence	= (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)(node->ActionPtr.actionInfo.qos.qos.ingress.profilePrecedence);
		
		
	/*	actionPtr->egressPolicy =ACL_TRUE;*/
	/*	actionPtr->qos.qos.egress.dscp = (char)(node->ActionPtr.actionInfo.qos.qos.egress.egrDscp);*/
	/*	actionPtr->qos.qos.egress.up = (char)(node->ActionPtr.actionInfo.qos.qos.egress.egrUp);*/
		
	/*	call cpss api function */

		ruleSize=CPSS_PCL_RULE_SIZE_EXT_E;
		
		result = cpssDxChPclRuleSet(devIdx, ruleFormat, ruleIndex, maskData,
													patternData, actionPtr);		
		if(result!=0)
			nam_syslog_dbg("cpssDxChPclRuleSet ERROR!\n");
		
		nam_syslog_dbg("cpssDxChPclRuleSet result is %d\n",result);
	}		
	
   	free(maskData);
	free(patternData);
	free(actionPtr);
	maskData=NULL;
	patternData=NULL;
	actionPtr=NULL;
	
	return result;
}

unsigned int nam_drv_acl_stdIpv4L4_ListDeal
(
	struct ACCESS_LIST_STC *node,
	unsigned int aclId, 
	int unit,
	unsigned int dir_info
)
{
	unsigned int result = 0;
	if(dir_info){
		result = nam_drv_egress_acl_stdIpv4L4_ListDeal(node,aclId,unit);
	}
	else{
		result = nam_drv_ingress_acl_stdIpv4L4_ListDeal(node,aclId,unit);
	}
	return result;
}

unsigned int nam_drv_ingress_acl_stdIpv4L4_ListDeal
(
	struct ACCESS_LIST_STC *node,
	unsigned int aclId, 
	int unit
)
{
	unsigned char 						devIdx = 0;
	unsigned long 						ruleIndex = 0;	
	unsigned int		 				ipty = 0,result = 0;
	unsigned long           			       pkd=ACL_ACTION_TYPE_PERMIT,nodeIpProtocal=0;		
	unsigned int						appDemoDevAmount=0;
	CPSS_PCL_RULE_SIZE_ENT  			ruleSize;
	/* table cpssDxChPclRule global variables */
	CPSS_DXCH_PCL_RULE_FORMAT_UNT	    *maskData=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_UNT	    *patternData=NULL;
	CPSS_DXCH_PCL_ACTION_STC		    *actionPtr=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
	/*code optimize:  Uninitialized scalar variable 
	zhangcl@autelan.com 2013-1-17*/
	CPSS_PACKET_CMD_ENT				    cmdPkd = CPSS_PACKET_CMD_FORWARD_E;	
	CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC *maskStdIpv4L4 = NULL,*patternStdIpv4L4 = NULL;
    	unsigned char			  redirectPort=0;
	unsigned int devNum = 0;
	result = prvCpssDrvHwPpGetRegField(0,0x58,4,5,&devNum);
	if(result!=0)
			nam_syslog_dbg("prvCpssDrvHwPpGetRegField ERROR!\n");
	appDemoDevAmount = nam_asic_get_instance_num(); 

	maskData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(NULL==maskData){
		nam_syslog_dbg("malloc fail!\n");
		return 1;
	}
	patternData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(NULL==patternData){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		maskData=NULL;
		return 1;
		
	}
	actionPtr =(CPSS_DXCH_PCL_ACTION_STC*)malloc(sizeof(CPSS_DXCH_PCL_ACTION_STC));
	if(NULL==actionPtr){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		free(patternData);
		maskData=NULL;
		patternData=NULL;
		return 1;
	}

	memset(maskData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(patternData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(actionPtr,0,sizeof(CPSS_DXCH_PCL_ACTION_STC));	


	maskStdIpv4L4 		=&(maskData->ruleStdIpv4L4);
	patternStdIpv4L4	= &(patternData->ruleStdIpv4L4);

	
	ipty = node->TcamPtr.packetType;

	switch(ipty)
		{
			case ACL_TCAM_RULE_PACKETTYPE_IP:		nodeIpProtocal=0;break;
			case ACL_TCAM_RULE_PACKETTYPE_UDP:		nodeIpProtocal=17;break;
			case ACL_TCAM_RULE_PACKETTYPE_TCP:		nodeIpProtocal=6;break;
			case ACL_TCAM_RULE_PACKETTYPE_ICMP: 	nodeIpProtocal=1;break;
			case ACL_TCAM_RULE_PACKETTYPE_ETHERNET: nodeIpProtocal=0;break;
			default:
					  break;
		}

	pkd=node->ActionPtr.actionType;

	switch(pkd)
		{
			case ACL_ACTION_TYPE_PERMIT:			 cmdPkd=CPSS_PACKET_CMD_FORWARD_E; break;
			case ACL_ACTION_TYPE_DENY:				 cmdPkd=CPSS_PACKET_CMD_DROP_HARD_E;break;
			case ACL_ACTION_TYPE_TRAP_TO_CPU:   	 cmdPkd=CPSS_PACKET_CMD_TRAP_TO_CPU_E;break;
		       case ACL_ACTION_TYPE_MIRROR_TO_ANALYZER: cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;
			case ACL_ACTION_TYPE_REDIRECT:			 cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;	 
			default:
					  break;
		}
	
	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++)
	{		
		
	   	memset(maskData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
		memset(patternData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
		memset(actionPtr,0,sizeof(CPSS_DXCH_PCL_ACTION_STC));	
	/* map input arguments to locals */
		/*devNum;*/
		/* use for save 0 1 for udp */
		if (node->ruleIndex >= (MAX_ACL_RULE_NUMBER/2)) {
			ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU*2;
		}
		else {
			ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU;
		}

		ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;
		
		maskStdIpv4L4->common.pclId = 0x3FF;
		maskStdIpv4L4->common.isIp = 1;
		/*maskStdIpv4L4->common.sourcePort = 0;*/
		maskStdIpv4L4->commonStdIp.isIpv4 = 1;	
		nam_syslog_dbg("maskSip %x\n",node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSip);
		maskStdIpv4L4->sip.u32Ip = node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSip;
#ifdef DRV_LIB_CPSS_3_4_XXX    /* the follow change for test acl of IP/32, zhangdi */
		ACL_SWAB32(maskStdIpv4L4->sip.u32Ip);
#endif
		maskStdIpv4L4->dip.u32Ip = node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDip;
		nam_syslog_dbg("*********maskStdIpv4L4->dip.u32Ip 0x%x\n",maskStdIpv4L4->dip.u32Ip);

#ifdef DRV_LIB_CPSS_3_4_XXX
		ACL_SWAB32(maskStdIpv4L4->dip.u32Ip);
#endif
		maskStdIpv4L4->commonStdIp.l4Byte2=((node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDstPort)&0xff00)>>8;
		maskStdIpv4L4->commonStdIp.l4Byte3=(node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDstPort)&0x00ff;
		if(nodeIpProtocal==1){
		maskStdIpv4L4->l4Byte0=node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskType;
		maskStdIpv4L4->l4Byte1=node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskCode;
		}
		else if((nodeIpProtocal==6)||(nodeIpProtocal==17)){
		maskStdIpv4L4->l4Byte0=((node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSrcPort)&0xff00)>>8;
		maskStdIpv4L4->l4Byte1=(node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSrcPort)&0x00ff;
		}
		if((nodeIpProtocal==1)||(nodeIpProtocal==6)||(nodeIpProtocal==17)){
			maskStdIpv4L4->commonStdIp.ipProtocol = 255;
			patternStdIpv4L4->commonStdIp.ipProtocol = nodeIpProtocal;
		}
		else{
			maskStdIpv4L4->commonStdIp.ipProtocol = 0;
			patternStdIpv4L4->commonStdIp.ipProtocol = 0;
		}
			
			
		patternStdIpv4L4->common.pclId = aclId;
		patternStdIpv4L4->common.isIp = 1;
		/*patternStdIpv4L4->common.sourcePort = 0;*/
		patternStdIpv4L4->commonStdIp.isIpv4 = 1;
		patternStdIpv4L4->sip.u32Ip = node->TcamPtr.ruleInfo.stdIpv4L4.rule.Sip;
#ifdef DRV_LIB_CPSS_3_4_XXX
		ACL_SWAB32(patternStdIpv4L4->sip.u32Ip);
#endif
		patternStdIpv4L4->dip.u32Ip = node->TcamPtr.ruleInfo.stdIpv4L4.rule.Dip;
		nam_syslog_dbg("*********patternStdIpv4L4->dip.u32Ip 0x%x\n",patternStdIpv4L4->dip.u32Ip);

#ifdef DRV_LIB_CPSS_3_4_XXX
		ACL_SWAB32(patternStdIpv4L4->dip.u32Ip);
#endif
		patternStdIpv4L4->commonStdIp.l4Byte2=((node->TcamPtr.ruleInfo.stdIpv4L4.rule.DstPort)&0xff00)>>8;
		patternStdIpv4L4->commonStdIp.l4Byte3=(node->TcamPtr.ruleInfo.stdIpv4L4.rule.DstPort)&0x00ff;
		if(nodeIpProtocal==1){
			patternStdIpv4L4->l4Byte0=node->TcamPtr.ruleInfo.stdIpv4L4.rule.Type;
			patternStdIpv4L4->l4Byte1=node->TcamPtr.ruleInfo.stdIpv4L4.rule.Code;		
		}
		else if((nodeIpProtocal==6)||(nodeIpProtocal==17)){
			patternStdIpv4L4->l4Byte0=((node->TcamPtr.ruleInfo.stdIpv4L4.rule.SrcPort)&0xff00)>>8;
			patternStdIpv4L4->l4Byte1=(node->TcamPtr.ruleInfo.stdIpv4L4.rule.SrcPort)&0x00ff;
		}
		
		actionPtr->pktCmd = cmdPkd;           
		if(ACL_ACTION_TYPE_PERMIT==pkd){
			actionPtr->policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;
			actionPtr->policer.policerId    = node->ActionPtr.policerPtr.policerId ; 
		}

		else if(ACL_ACTION_TYPE_MIRROR_TO_ANALYZER==pkd){

			/*actionPtr.policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;*/
			/*actionPtr.policer.policerId    = node->ActionPtr.policerPtr.policerId ; */
	
			 actionPtr->mirror.mirrorToRxAnalyzerPort = 1;
		}
		else if(ACL_ACTION_TYPE_REDIRECT==pkd){			
				/*actionPtr.policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;*/
				/*actionPtr.policer.policerId    = node->ActionPtr.policerPtr.policerId ; */
	
				/*note: only port redirect to egress interface,that's policy switching,no router redirect*/
				 actionPtr->redirect.redirectCmd =  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
				#if 0
				 actionPtr->redirect.data.outIf.outInterface.devPort.devNum = 0;
				#endif
				actionPtr->redirect.data.outIf.outInterface.devPort.devNum = devNum;
				 actionPtr->redirect.data.outIf.outInterface.devPort.portNum = node->ActionPtr.actionInfo.redirect.portNum;
				redirectPort = actionPtr->redirect.data.outIf.outInterface.devPort.portNum;
				nam_syslog_dbg(" sw redirectPort %d\n",node->ActionPtr.actionInfo.redirect.portNum);
				nam_syslog_dbg("redirectPort %d\n",redirectPort);
		}
		else if(ACL_ACTION_TYPE_TRAP_TO_CPU	== pkd) {
			actionPtr->mirror.cpuCode =node->ActionPtr.actionInfo.trap.cpucode;
		}
		
		/*qos*/
		
		actionPtr->qos.modifyDscp=(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyDscp);
		actionPtr->qos.modifyUp= (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyUp);
		actionPtr->qos.qos.ingress.profileIndex = node->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
		actionPtr->qos.qos.ingress.profileAssignIndex = 0;
		actionPtr->qos.qos.ingress.profilePrecedence	= (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)(node->ActionPtr.actionInfo.qos.qos.ingress.profilePrecedence);
		
		
		/*actionPtr->egressPolicy =ACL_TRUE;*/
		/*actionPtr->qos.qos.egress.dscp = (char)(node->ActionPtr.actionInfo.qos.qos.egress.egrDscp);*/
		/*actionPtr->qos.qos.egress.up = (char)(node->ActionPtr.actionInfo.qos.qos.egress.egrUp);*/
		
		/* call cpss api function */

		ruleSize=CPSS_PCL_RULE_SIZE_STD_E;	
		result = cpssDxChPclRuleSet(devIdx, ruleFormat, ruleIndex, maskData,
													patternData, actionPtr);		
		if(result!=0)
			nam_syslog_dbg("cpssDxChPclRuleSet ERROR!\n");
		
		nam_syslog_dbg("cpssDxChPclRuleSet std ip result is %d\n",result);
		
	}
	
    	free(maskData);
	free(patternData);
	free(actionPtr);
	maskData=NULL;
	patternData=NULL;
	actionPtr=NULL;

	return result;
}


unsigned int nam_drv_egress_acl_stdIpv4L4_ListDeal
(
	struct ACCESS_LIST_STC *node,
	unsigned int aclId, 
	int unit
)
{
	unsigned char 						devIdx = 0;
	unsigned long 						ruleIndex = 0;	
	unsigned int		 				ipty = 0,result = 0;
	unsigned long           			       pkd=ACL_ACTION_TYPE_PERMIT,nodeIpProtocal=0;		
	unsigned int						appDemoDevAmount=0;
	CPSS_PCL_RULE_SIZE_ENT  			ruleSize;
	/* table cpssDxChPclRule global variables */
	CPSS_DXCH_PCL_RULE_FORMAT_UNT	    *maskData=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_UNT	    *patternData=NULL;
	CPSS_DXCH_PCL_ACTION_STC		    *actionPtr=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
	/*code optimize:  Uninitialized scalar variable 
	zhangcl@autelan.com 2013-1-17*/
	CPSS_PACKET_CMD_ENT				    cmdPkd = CPSS_PACKET_CMD_FORWARD_E;	
	CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_IPV4_L4_STC *maskStdIpv4L4 = NULL,*patternStdIpv4L4 = NULL;
    unsigned char			  redirectPort=0;

	unsigned int devNum = 0;
	result = prvCpssDrvHwPpGetRegField(0,0x58,4,5,&devNum);
	if(result!=0)
			nam_syslog_dbg("prvCpssDrvHwPpGetRegField ERROR!\n");
	appDemoDevAmount = nam_asic_get_instance_num(); 

	maskData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(NULL==maskData){
		nam_syslog_dbg("malloc fail!\n");
		return 1;
	}
	patternData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(NULL==patternData){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		maskData=NULL;
		return 1;
		
	}
	actionPtr =(CPSS_DXCH_PCL_ACTION_STC*)malloc(sizeof(CPSS_DXCH_PCL_ACTION_STC));
	if(NULL==actionPtr){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		free(patternData);
		maskData=NULL;
		patternData=NULL;
		return 1;
	}

	memset(maskData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(patternData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(actionPtr,0,sizeof(CPSS_DXCH_PCL_ACTION_STC));	


	maskStdIpv4L4 		=&(maskData->ruleEgrStdIpv4L4);
	patternStdIpv4L4	= &(patternData->ruleEgrStdIpv4L4);

	
	ipty = node->TcamPtr.packetType;

	switch(ipty)
		{
			case ACL_TCAM_RULE_PACKETTYPE_IP:		nodeIpProtocal=0;break;
			case ACL_TCAM_RULE_PACKETTYPE_UDP:		nodeIpProtocal=17;break;
			case ACL_TCAM_RULE_PACKETTYPE_TCP:		nodeIpProtocal=6;break;
			case ACL_TCAM_RULE_PACKETTYPE_ICMP: 	nodeIpProtocal=1;break;
			case ACL_TCAM_RULE_PACKETTYPE_ETHERNET: nodeIpProtocal=0;break;
			default:
					  break;
		}

	pkd=node->ActionPtr.actionType;

	switch(pkd)
		{
			case ACL_ACTION_TYPE_PERMIT:			 cmdPkd=CPSS_PACKET_CMD_FORWARD_E; break;
			case ACL_ACTION_TYPE_DENY:				 cmdPkd=CPSS_PACKET_CMD_DROP_HARD_E;break;
			case ACL_ACTION_TYPE_TRAP_TO_CPU:   	 cmdPkd=CPSS_PACKET_CMD_TRAP_TO_CPU_E;break;
		       case ACL_ACTION_TYPE_MIRROR_TO_ANALYZER: cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;
			case ACL_ACTION_TYPE_REDIRECT:			 cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;	 
			default:
					  break;
		}
	
	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++)
	{		
		
	   	memset(maskData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
		memset(patternData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
		memset(actionPtr,0,sizeof(CPSS_DXCH_PCL_ACTION_STC));	
	/* map input arguments to locals */
		/*devNum;*/
		/* use for save 0 1 for udp */
		if (node->ruleIndex >= (MAX_ACL_RULE_NUMBER/2)) {
			ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU*2;
		}
		else {
			ruleIndex = node->ruleIndex + ACL_SAVE_FOR_UDP_BPDU;
		}
		
		ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E;
		
		maskStdIpv4L4->common.pclId = 0x3FF;
		maskStdIpv4L4->common.isIp = 1;
		/*maskStdIpv4L4->common.sourcePort = 0;*/
		maskStdIpv4L4->commonStdIp.isIpv4 = 1;	
		nam_syslog_dbg("maskSip %x\n",node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSip);
		maskStdIpv4L4->sip.u32Ip = node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSip;
#ifdef DRV_LIB_CPSS_3_4_XXX
		ACL_SWAB32(maskStdIpv4L4->sip.u32Ip);
#endif
		maskStdIpv4L4->dip.u32Ip = node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDip;
		nam_syslog_dbg("***********maskStdIpv4L4->dip.u32Ip 0x%x\n",maskStdIpv4L4->dip.u32Ip);

#ifdef DRV_LIB_CPSS_3_4_XXX
		ACL_SWAB32(maskStdIpv4L4->dip.u32Ip);
#endif
		maskStdIpv4L4->commonStdIp.l4Byte2=((node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDstPort)&0xff00)>>8;
		maskStdIpv4L4->commonStdIp.l4Byte3=(node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDstPort)&0x00ff;
		if(nodeIpProtocal==1){
		maskStdIpv4L4->l4Byte0=node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskType;
		maskStdIpv4L4->l4Byte1=node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskCode;
		}
		else if((nodeIpProtocal==6)||(nodeIpProtocal==17)){
		maskStdIpv4L4->l4Byte0=((node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSrcPort)&0xff00)>>8;
		maskStdIpv4L4->l4Byte1=(node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSrcPort)&0x00ff;
		}
		if((nodeIpProtocal==1)||(nodeIpProtocal==6)||(nodeIpProtocal==17)){
			maskStdIpv4L4->commonStdIp.ipProtocol = 255;
			patternStdIpv4L4->commonStdIp.ipProtocol = nodeIpProtocal;
		}
		else{
			maskStdIpv4L4->commonStdIp.ipProtocol = 0;
			patternStdIpv4L4->commonStdIp.ipProtocol = 0;
		}
			
			
		patternStdIpv4L4->common.pclId = aclId;
		patternStdIpv4L4->common.isIp = 1;
		/*patternStdIpv4L4->common.sourcePort = 0;*/
		patternStdIpv4L4->commonStdIp.isIpv4 = 1;
		patternStdIpv4L4->sip.u32Ip = node->TcamPtr.ruleInfo.stdIpv4L4.rule.Sip;
#ifdef DRV_LIB_CPSS_3_4_XXX
		ACL_SWAB32(patternStdIpv4L4->sip.u32Ip);
#endif
		patternStdIpv4L4->dip.u32Ip = node->TcamPtr.ruleInfo.stdIpv4L4.rule.Dip;
		nam_syslog_dbg("*********patternStdIpv4L4->dip.u32Ip 0x%x\n",patternStdIpv4L4->dip.u32Ip);

#ifdef DRV_LIB_CPSS_3_4_XXX
		ACL_SWAB32(patternStdIpv4L4->dip.u32Ip);
#endif
		patternStdIpv4L4->commonStdIp.l4Byte2=((node->TcamPtr.ruleInfo.stdIpv4L4.rule.DstPort)&0xff00)>>8;
		patternStdIpv4L4->commonStdIp.l4Byte3=(node->TcamPtr.ruleInfo.stdIpv4L4.rule.DstPort)&0x00ff;
		if(nodeIpProtocal==1){
			patternStdIpv4L4->l4Byte0=node->TcamPtr.ruleInfo.stdIpv4L4.rule.Type;
			patternStdIpv4L4->l4Byte1=node->TcamPtr.ruleInfo.stdIpv4L4.rule.Code;		
		}
		else if((nodeIpProtocal==6)||(nodeIpProtocal==17)){
			patternStdIpv4L4->l4Byte0=((node->TcamPtr.ruleInfo.stdIpv4L4.rule.SrcPort)&0xff00)>>8;
			patternStdIpv4L4->l4Byte1=(node->TcamPtr.ruleInfo.stdIpv4L4.rule.SrcPort)&0x00ff;
		}
		
		actionPtr->pktCmd = cmdPkd;           
		if(ACL_ACTION_TYPE_PERMIT==pkd){
			actionPtr->policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;
			actionPtr->policer.policerId    = node->ActionPtr.policerPtr.policerId ; 
		}

		else if(ACL_ACTION_TYPE_MIRROR_TO_ANALYZER==pkd){

			/*actionPtr.policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;*/
			/*actionPtr.policer.policerId    = node->ActionPtr.policerPtr.policerId ; */
	
			 actionPtr->mirror.mirrorToRxAnalyzerPort = 1;
		}
		else if(ACL_ACTION_TYPE_REDIRECT==pkd){			
				/*actionPtr.policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;*/
				/*actionPtr.policer.policerId    = node->ActionPtr.policerPtr.policerId ; */
	
				/*note: only port redirect to egress interface,that's policy switching,no router redirect*/
				 actionPtr->redirect.redirectCmd =  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
				 actionPtr->redirect.data.outIf.outInterface.devPort.devNum = devNum;
				 actionPtr->redirect.data.outIf.outInterface.devPort.portNum = node->ActionPtr.actionInfo.redirect.portNum;
				redirectPort = actionPtr->redirect.data.outIf.outInterface.devPort.portNum;
				nam_syslog_dbg(" sw redirectPort %d\n",node->ActionPtr.actionInfo.redirect.portNum);
				nam_syslog_dbg("redirectPort %d\n",redirectPort);
		}
		else if(ACL_ACTION_TYPE_TRAP_TO_CPU	== pkd) {
			actionPtr->mirror.cpuCode =node->ActionPtr.actionInfo.trap.cpucode;
		}
		
		/*qos*/
		
		actionPtr->qos.modifyDscp=(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyDscp);
		actionPtr->qos.modifyUp= (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyUp);
		actionPtr->qos.qos.ingress.profileIndex = node->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
		actionPtr->qos.qos.ingress.profileAssignIndex = 0;
		actionPtr->qos.qos.ingress.profilePrecedence	= (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)(node->ActionPtr.actionInfo.qos.qos.ingress.profilePrecedence);
		
		
		/*actionPtr->egressPolicy =ACL_TRUE;*/
		/*actionPtr->qos.qos.egress.dscp = (char)(node->ActionPtr.actionInfo.qos.qos.egress.egrDscp);*/
		/*actionPtr->qos.qos.egress.up = (char)(node->ActionPtr.actionInfo.qos.qos.egress.egrUp);*/
		
		/* call cpss api function */

		ruleSize=CPSS_PCL_RULE_SIZE_STD_E;	
		result = cpssDxChPclRuleSet(devIdx, ruleFormat, ruleIndex, maskData,
													patternData, actionPtr);
		if(result!=0)
			nam_syslog_dbg("cpssDxChPclRuleSet ERROR!\n");
		
		nam_syslog_dbg("cpssDxChPclRuleSet std ip result is %d\n",result);
		
	}
	
    	free(maskData);
	free(patternData);
	free(actionPtr);
	maskData=NULL;
	patternData=NULL;
	actionPtr=NULL;

	return result;
}

#endif

/**********************************************************************************
 * nam_drv_acl_xxx_listDeal
 *
 * set single rule tcam for marvell, no need group info
 *
 *	INPUT:
 *		struct ACCESS_LIST_STC  *node	 - struct for acl rule
 *		unsigned int 			aclId	 - acl index
 *		int 					unit	 - dev number
 *
 *	OUTPUT:
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_SUCCESS 	- set rule tcam success 
 *		ACL_RETURN_CODE_ERROR		- set rule tcam fail
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int nam_drv_acl_stdIpv4L4_no_group_num
(
	struct ACCESS_LIST_STC *node,
	unsigned int aclId, 
	int unit
)
{
#ifdef DRV_LIB_CPSS
	unsigned char 						devIdx = 0;
	unsigned long 						ruleIndex = 0;	
	unsigned int		 				ipty = 0,result = 0;
	unsigned long           			       pkd=ACL_ACTION_TYPE_PERMIT,nodeIpProtocal=0;		
	unsigned int						appDemoDevAmount=0;
	CPSS_PCL_RULE_SIZE_ENT  			ruleSize;
	/* table cpssDxChPclRule global variables */
	CPSS_DXCH_PCL_RULE_FORMAT_UNT	    *maskData=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_UNT	    *patternData=NULL;
	CPSS_DXCH_PCL_ACTION_STC		    *actionPtr=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
	/*code optimize:  Uninitialized scalar variable 
	zhangcl@autelan.com 2013-1-17*/
	CPSS_PACKET_CMD_ENT				    cmdPkd = CPSS_PACKET_CMD_FORWARD_E;		
	CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC *maskStdIpv4L4 = NULL,*patternStdIpv4L4 = NULL;
    	unsigned char			  redirectPort=0;
	unsigned int devNum = 0;
	result = prvCpssDrvHwPpGetRegField(0,0x58,4,5,&devNum);
	if(result!=0)
			nam_syslog_dbg("prvCpssDrvHwPpGetRegField ERROR!\n");

	appDemoDevAmount = nam_asic_get_instance_num(); 

	maskData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(NULL==maskData){
		nam_syslog_dbg("malloc fail!\n");
		return 1;
	}
	patternData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(NULL==patternData){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		maskData=NULL;
		return 1;
		
	}
	actionPtr =(CPSS_DXCH_PCL_ACTION_STC*)malloc(sizeof(CPSS_DXCH_PCL_ACTION_STC));
	if(NULL==actionPtr){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		free(patternData);
		maskData=NULL;
		patternData=NULL;
		return 1;
	}

	memset(maskData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(patternData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(actionPtr,0,sizeof(CPSS_DXCH_PCL_ACTION_STC));	


	maskStdIpv4L4 		=&(maskData->ruleStdIpv4L4);
	patternStdIpv4L4	= &(patternData->ruleStdIpv4L4);

	
	ipty = node->TcamPtr.packetType;

	switch(ipty)
		{
			case ACL_TCAM_RULE_PACKETTYPE_IP:		nodeIpProtocal=0;break;
			case ACL_TCAM_RULE_PACKETTYPE_UDP:		nodeIpProtocal=17;break;
			case ACL_TCAM_RULE_PACKETTYPE_TCP:		nodeIpProtocal=6;break;
			case ACL_TCAM_RULE_PACKETTYPE_ICMP: 	nodeIpProtocal=1;break;
			case ACL_TCAM_RULE_PACKETTYPE_ETHERNET: nodeIpProtocal=0;break;
			default:
					  break;
		}

	pkd=node->ActionPtr.actionType;

	switch(pkd)
		{
			case ACL_ACTION_TYPE_PERMIT:			 cmdPkd=CPSS_PACKET_CMD_FORWARD_E; break;
			case ACL_ACTION_TYPE_DENY:				 cmdPkd=CPSS_PACKET_CMD_DROP_HARD_E;break;
			case ACL_ACTION_TYPE_TRAP_TO_CPU:   	 cmdPkd=CPSS_PACKET_CMD_TRAP_TO_CPU_E;break;
		       case ACL_ACTION_TYPE_MIRROR_TO_ANALYZER: cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;
			case ACL_ACTION_TYPE_REDIRECT:			 cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;	 
			default:
					  break;
		}
	
	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++)
	{		
		
	   	memset(maskData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
		memset(patternData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
		memset(actionPtr,0,sizeof(CPSS_DXCH_PCL_ACTION_STC));	
	/* map input arguments to locals */
		/*devNum;*/
		ruleIndex = node->ruleIndex;
		ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;;		
		maskStdIpv4L4->common.pclId = 0;
		maskStdIpv4L4->common.isIp = 1;
		/*maskStdIpv4L4->common.sourcePort = 0;*/
		maskStdIpv4L4->commonStdIp.isIpv4 = 1;	
		nam_syslog_dbg("maskSip %ld\n",node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSip);
		maskStdIpv4L4->sip.u32Ip = node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSip;
		maskStdIpv4L4->dip.u32Ip = node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDip;
		maskStdIpv4L4->commonStdIp.l4Byte2=((node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDstPort)&0xff00)>>8;
		maskStdIpv4L4->commonStdIp.l4Byte3=(node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDstPort)&0x00ff;
		if(nodeIpProtocal==1){
		maskStdIpv4L4->l4Byte0=node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskType;
		maskStdIpv4L4->l4Byte1=node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskCode;
		}
		else if((nodeIpProtocal==6)||(nodeIpProtocal==17)){
		maskStdIpv4L4->l4Byte0=((node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSrcPort)&0xff00)>>8;
		maskStdIpv4L4->l4Byte1=(node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSrcPort)&0x00ff;
		}
		if((nodeIpProtocal==1)||(nodeIpProtocal==6)||(nodeIpProtocal==17)){
			maskStdIpv4L4->commonStdIp.ipProtocol = 255;
			patternStdIpv4L4->commonStdIp.ipProtocol = nodeIpProtocal;
		}
		else{
			maskStdIpv4L4->commonStdIp.ipProtocol = 0;
			patternStdIpv4L4->commonStdIp.ipProtocol = 0;
		}
			
			
		patternStdIpv4L4->common.pclId = aclId;
		patternStdIpv4L4->common.isIp = 1;
		/*patternStdIpv4L4->common.sourcePort = 0;*/
		patternStdIpv4L4->commonStdIp.isIpv4 = 1;
		patternStdIpv4L4->sip.u32Ip = node->TcamPtr.ruleInfo.stdIpv4L4.rule.Sip;
		patternStdIpv4L4->dip.u32Ip = node->TcamPtr.ruleInfo.stdIpv4L4.rule.Dip;
		patternStdIpv4L4->commonStdIp.l4Byte2=((node->TcamPtr.ruleInfo.stdIpv4L4.rule.DstPort)&0xff00)>>8;
		patternStdIpv4L4->commonStdIp.l4Byte3=(node->TcamPtr.ruleInfo.stdIpv4L4.rule.DstPort)&0x00ff;
		if(nodeIpProtocal==1){
			patternStdIpv4L4->l4Byte0=node->TcamPtr.ruleInfo.stdIpv4L4.rule.Type;
			patternStdIpv4L4->l4Byte1=node->TcamPtr.ruleInfo.stdIpv4L4.rule.Code;		
		}
		else if((nodeIpProtocal==6)||(nodeIpProtocal==17)){
			patternStdIpv4L4->l4Byte0=((node->TcamPtr.ruleInfo.stdIpv4L4.rule.SrcPort)&0xff00)>>8;
			patternStdIpv4L4->l4Byte1=(node->TcamPtr.ruleInfo.stdIpv4L4.rule.SrcPort)&0x00ff;
		}
		
		actionPtr->pktCmd = cmdPkd;           
		if(ACL_ACTION_TYPE_PERMIT==pkd){
			actionPtr->policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;
			actionPtr->policer.policerId    = node->ActionPtr.policerPtr.policerId ; 
		}

		else if(ACL_ACTION_TYPE_MIRROR_TO_ANALYZER==pkd){


	
			 actionPtr->mirror.mirrorToRxAnalyzerPort = 1;
		}
		else if(ACL_ACTION_TYPE_REDIRECT==pkd){			

				 actionPtr->redirect.redirectCmd =  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
				 actionPtr->redirect.data.outIf.outInterface.devPort.devNum = devNum;
				 actionPtr->redirect.data.outIf.outInterface.devPort.portNum = node->ActionPtr.actionInfo.redirect.portNum;
				redirectPort = actionPtr->redirect.data.outIf.outInterface.devPort.portNum;
				nam_syslog_dbg(" sw redirectPort %d\n",node->ActionPtr.actionInfo.redirect.portNum);
				nam_syslog_dbg("redirectPort %d\n",redirectPort);
		}
		else if(ACL_ACTION_TYPE_TRAP_TO_CPU	== pkd) {
			actionPtr->mirror.cpuCode =node->ActionPtr.actionInfo.trap.cpucode;
		}
		

		
		actionPtr->qos.modifyDscp=(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyDscp);
		actionPtr->qos.modifyUp= (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyUp);
		actionPtr->qos.qos.ingress.profileIndex = node->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
		actionPtr->qos.qos.ingress.profileAssignIndex = 1;
		actionPtr->qos.qos.ingress.profilePrecedence	= (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)(node->ActionPtr.actionInfo.qos.qos.ingress.profilePrecedence);
		
		


		ruleSize=CPSS_PCL_RULE_SIZE_STD_E;
		
		result = cpssDxChPclRuleSet(devIdx, ruleFormat, ruleIndex, maskData,
													patternData, actionPtr);		
		if(result!=0)
			nam_syslog_dbg("cpssDxChPclRuleSet ERROR!\n");
		
		nam_syslog_dbg("cpssDxChPclRuleSet result is %d\n",result);
		
	}
	
    	free(maskData);
	free(patternData);
	free(actionPtr);
	maskData=NULL;
	patternData=NULL;
	actionPtr=NULL;
#endif
	return 0;
}

/**********************************************************************************
 * nam_drv_acl_extNotIpv6_no_group_num
 *
 * set single rule tcam for marvell, no need group info
 *
 *	INPUT:
 *		struct ACCESS_LIST_STC  *node	 - struct for acl rule
 *		unsigned int 			aclId	 - acl index
 *		int 					unit	 - dev number
 *
 *	OUTPUT:
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_SUCCESS 	- set rule tcam success 
 *		ACL_RETURN_CODE_ERROR		- set rule tcam fail
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int nam_drv_acl_extNotIpv6_no_group_num
(
	struct ACCESS_LIST_STC * node,
	unsigned int aclId,
	int unit
)
{
#ifdef DRV_LIB_CPSS
	unsigned char						devIdx = 0,appDemoDevAmount = 0;
	unsigned long						ruleIndex = 0;
	unsigned long						GetIndex = 0;	
	unsigned int						i = 0,ipty = 0,result = 0;
	unsigned long						pkd = 0,nodeIpProtocal = 0;
	unsigned int						maskPtr[12] = {0};
	unsigned int						patternPtr[12] = {0};
	unsigned int						actionptr[3] = {0}; 
	unsigned long						bitPos=189;
	CPSS_PCL_RULE_SIZE_ENT				ruleSize;
	/* table cpssDxChPclRule global variables */
	CPSS_DXCH_PCL_RULE_FORMAT_UNT		*maskData=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_UNT		*patternData=NULL;
	CPSS_DXCH_PCL_ACTION_STC			*actionPtr=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT	ruleFormat;
	CPSS_PACKET_CMD_ENT 				cmdPkd;
	
	CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC *maskExtNotIpv6 = NULL,*patternExtNotIpv6 = NULL;

	unsigned int devNum = 0;
	result = prvCpssDrvHwPpGetRegField(0,0x58,4,5,&devNum);
	if(result!=0)
			nam_syslog_dbg("prvCpssDrvHwPpGetRegField ERROR!\n");
	maskData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(maskData==NULL){
		nam_syslog_dbg("malloc fail!\n");
		return 1;
	}
	patternData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(patternData==NULL){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		maskData=NULL;
		return 1;
	}
	actionPtr =(CPSS_DXCH_PCL_ACTION_STC*)malloc(sizeof(CPSS_DXCH_PCL_ACTION_STC));
	if(actionPtr==NULL){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		free(patternData);
		maskData=NULL;
		patternData=NULL;
		return 1;
	}

	memset(maskData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(patternData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(actionPtr,0,sizeof(CPSS_DXCH_PCL_ACTION_STC));	

	maskExtNotIpv6		= &(maskData->ruleExtNotIpv6);
	patternExtNotIpv6	= &(patternData->ruleExtNotIpv6);

	appDemoDevAmount = nam_asic_get_instance_num(); 
	
	ipty = node->TcamPtr.packetType;

	switch(ipty)
		{
			case ACL_TCAM_RULE_PACKETTYPE_UDP:		nodeIpProtocal=17;break;
			case ACL_TCAM_RULE_PACKETTYPE_TCP:		nodeIpProtocal=6;break;
		
			default:
					  break;
		}

	pkd=node->ActionPtr.actionType;

	switch(pkd)
		{
			case ACL_ACTION_TYPE_PERMIT:			 cmdPkd=CPSS_PACKET_CMD_FORWARD_E; break;
			case ACL_ACTION_TYPE_DENY:				 cmdPkd=CPSS_PACKET_CMD_DROP_HARD_E;break;
			case ACL_ACTION_TYPE_TRAP_TO_CPU:		 cmdPkd=CPSS_PACKET_CMD_TRAP_TO_CPU_E;break;
			case ACL_ACTION_TYPE_MIRROR_TO_ANALYZER: cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;
			case ACL_ACTION_TYPE_REDIRECT:			 cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;	 
			default:
					  break;
		}
	
	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++)
	{		
		
	/* map input arguments to locals */
		/*devNum;*/

		ruleIndex = node->ruleIndex;
	
		ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;		
		maskExtNotIpv6->common.pclId = 0;
		maskExtNotIpv6->common.sourcePort = node->TcamPtr.ruleInfo.extNotIpv6.mask.maskPortNo;
		maskExtNotIpv6->common.vid = node->TcamPtr.ruleInfo.extNotIpv6.mask.maskVlanId;
		maskExtNotIpv6->common.isIp = 1;

		maskExtNotIpv6->commonExt.l4Byte0=((node->TcamPtr.ruleInfo.extNotIpv6.mask.maskSrcPort)&0xff00)>>8;
		maskExtNotIpv6->commonExt.l4Byte1=(node->TcamPtr.ruleInfo.extNotIpv6.mask.maskSrcPort)&0x00ff;
        	maskExtNotIpv6->commonExt.l4Byte2=((node->TcamPtr.ruleInfo.extNotIpv6.mask.maskDstPort)&0xff00)>>8;
		maskExtNotIpv6->commonExt.l4Byte3=(node->TcamPtr.ruleInfo.extNotIpv6.mask.maskDstPort)&0x00ff;		
       	maskExtNotIpv6->commonExt.ipProtocol = 255;
		maskExtNotIpv6->sip.u32Ip = node->TcamPtr.ruleInfo.extNotIpv6.mask.maskSip;
		maskExtNotIpv6->dip.u32Ip = node->TcamPtr.ruleInfo.extNotIpv6.mask.maskDip;
		
		for(i=0;i<6;i++){
			maskExtNotIpv6->macSa.arEther[i]=node->TcamPtr.ruleInfo.extNotIpv6.mask.maskSmac[i];
			patternExtNotIpv6->macSa.arEther[i]=node->TcamPtr.ruleInfo.extNotIpv6.rule.sMac[i];
			maskExtNotIpv6->macDa.arEther[i]=node->TcamPtr.ruleInfo.extNotIpv6.mask.maskDmac[i];
			patternExtNotIpv6->macDa.arEther[i]=node->TcamPtr.ruleInfo.extNotIpv6.rule.dMac[i];
		}

		patternExtNotIpv6->common.pclId = aclId;
		if (ACL_ANY_PORT_CHAR == (node->TcamPtr.ruleInfo.extNotIpv6.rule.portNo)) {
			patternExtNotIpv6->common.sourcePort = 0;
		}
		else {
			patternExtNotIpv6->common.sourcePort = node->TcamPtr.ruleInfo.extNotIpv6.rule.portNo;
		}	
		patternExtNotIpv6->common.vid = node->TcamPtr.ruleInfo.extNotIpv6.rule.vlanId;
        	patternExtNotIpv6->common.isIp = 1;
       	patternExtNotIpv6->commonExt.l4Byte0=((node->TcamPtr.ruleInfo.extNotIpv6.rule.SrcPort)&0xff00)>>8;
		patternExtNotIpv6->commonExt.l4Byte1=(node->TcamPtr.ruleInfo.extNotIpv6.rule.SrcPort)&0x00ff;
      		patternExtNotIpv6->commonExt.l4Byte2=((node->TcamPtr.ruleInfo.extNotIpv6.rule.DstPort)&0xff00)>>8;
		patternExtNotIpv6->commonExt.l4Byte3=(node->TcamPtr.ruleInfo.extNotIpv6.rule.DstPort)&0x00ff;		
       	patternExtNotIpv6->commonExt.ipProtocol = nodeIpProtocal;
		patternExtNotIpv6->sip.u32Ip = node->TcamPtr.ruleInfo.extNotIpv6.rule.Sip;
		patternExtNotIpv6->dip.u32Ip = node->TcamPtr.ruleInfo.extNotIpv6.rule.Dip;
		                                                                                    
		actionPtr->pktCmd = cmdPkd;	
		if(ACL_ACTION_TYPE_PERMIT==pkd){
				actionPtr->policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;
				actionPtr->policer.policerId    = node->ActionPtr.policerPtr.policerId ; 
		}
		
		else if(ACL_ACTION_TYPE_MIRROR_TO_ANALYZER==pkd){

			/*actionPtr.policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;*/
			/*actionPtr.policer.policerId    = node->ActionPtr.policerPtr.policerId ; */
	
			 actionPtr->mirror.mirrorToRxAnalyzerPort = 1;			 
		}
		else if(ACL_ACTION_TYPE_REDIRECT==pkd){			
				/*actionPtr.policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;*/
				/*actionPtr.policer.policerId    = node->ActionPtr.policerPtr.policerId ; */
	
				/*note: only port redirect to egress interface,that's policy switching,no router redirect*/
				 actionPtr->redirect.redirectCmd =  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
				 actionPtr->redirect.data.outIf.outInterface.devPort.devNum = devNum;
				 actionPtr->redirect.data.outIf.outInterface.devPort.portNum = node->ActionPtr.actionInfo.redirect.portNum;
				 /*actionPtr.redirect.data.outIf.outInterface.vlanId = 1;*/
		}
		else if(ACL_ACTION_TYPE_TRAP_TO_CPU	== pkd) {
			actionPtr->mirror.cpuCode =node->ActionPtr.actionInfo.trap.cpucode;
		}
		/*QOS*/
		
		actionPtr->qos.modifyDscp=(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyDscp);
		actionPtr->qos.modifyUp= (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyUp);
		actionPtr->qos.qos.ingress.profileIndex = node->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
		actionPtr->qos.qos.ingress.profileAssignIndex = 1;
		actionPtr->qos.qos.ingress.profilePrecedence	= (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)(node->ActionPtr.actionInfo.qos.qos.ingress.profilePrecedence);
		

		/* call cpss api function */

		ruleSize=CPSS_PCL_RULE_SIZE_EXT_E;
		
		result = cpssDxChPclRuleSet(devIdx, ruleFormat, ruleIndex, maskData,
													patternData, actionPtr);		
		if(result!=0)
			nam_syslog_dbg("cpssDxChPclRuleSet ERROR!\n");
		
		nam_syslog_dbg("cpssDxChPclRuleSet result is %d\n",result);
				
	}
	free(maskData);
	free(patternData);
	free(actionPtr);	
	maskData=NULL;
	patternData=NULL;
	actionPtr=NULL;
#endif
	return 0;
}

/**********************************************************************************
 * nam_acl_qos_action_update
 *
 * update acl action
 *
 *	INPUT:
 *		unsigned int 		ruleIndex - acl rule index
 *
 *	OUTPUT:
 *
 *	RETURN:
 *		ACL_RETURN_CODE_SUCCESS
 *	NOTE:
 *
 **********************************************************************************/
unsigned int nam_acl_qos_action_update
(
	unsigned int ruleIndex
)
{
	unsigned int              devIdx=0,appDemoDevAmount=0,profileIndex = 0,ret = 0;
	AclRuleList               *node=NULL;
	appDemoDevAmount = nam_asic_get_instance_num();  
	
#ifdef DRV_LIB_CPSS	
	CPSS_DXCH_PCL_ACTION_STC	actionPtr2;
	unsigned int 				GetIndex=0;
	CPSS_PCL_RULE_SIZE_ENT  	ruleSize;
	                          
	node = g_acl_rule[ruleIndex];
	
	/* save 0 1 rule for udp */
	if (ruleIndex >= (MAX_ACL_RULE_NUMBER/2)) {
		ruleIndex += ACL_SAVE_FOR_UDP_BPDU*2;
	}
	else {
		ruleIndex += ACL_SAVE_FOR_UDP_BPDU;
	}

	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++) {

		ruleSize=CPSS_PCL_RULE_SIZE_EXT_E;
		GetIndex=cpssDxChPclRuleActionGet(devIdx,ruleSize,ruleIndex,CPSS_PCL_DIRECTION_INGRESS_E,&actionPtr2);
		if(GetIndex!=0){
			nam_syslog_dbg("cpssDxChPclRuleActionGet fail! GetIndex %d\n",GetIndex);
			return 1;
		}

		actionPtr2.qos.modifyDscp=(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyDscp);
		actionPtr2.qos.modifyUp= (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyUp);
		actionPtr2.qos.qos.ingress.profileIndex = node->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
		actionPtr2.qos.qos.ingress.profileAssignIndex = 1;
		actionPtr2.qos.qos.ingress.profilePrecedence =CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

		ret= cpssDxChPclRuleActionUpdate(devIdx,ruleSize,ruleIndex,&actionPtr2);
		if(ret!=0){
			nam_syslog_dbg("cpssDxChPclRuleActionUpdate %d\n",ret);
			return 1;
		}			
	}  
#endif

#ifdef DRV_LIB_BCM
	 uint8 newdscp = 0;
	 bcm_vlan_t newpri = 0;
	 int cosqueue = 0, unit = 0, i = 0;
 
	 node = g_acl_rule[ruleIndex];
 
	 profileIndex = node->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
	 newdscp = g_qos_profile[profileIndex]->dscp;
	 newpri = g_qos_profile[profileIndex]->userPriority;
	 cosqueue = g_qos_profile[profileIndex]->trafficClass;
	struct acl_rule_info_s		 *ruleInfo = NULL;
	struct list_head			*Ptr = NULL;
	struct acl_group_list_s	*groupPtr = NULL;
	unsigned int  add_index = 0;

	nam_syslog_dbg("nam_qos_policer_entry_set ! \n");
	for (i = 0; i < MAX_GROUP_NUM; i++) {
		groupPtr = acl_group[i];
		if (NULL != groupPtr) {
			 __list_for_each(Ptr, &(groupPtr->list1)) {
				ruleInfo = list_entry(Ptr,struct acl_rule_info_s,list);
				if (NULL != ruleInfo) { 		
					if (ruleIndex == ruleInfo->ruleIndex) {
						for(devIdx=0;devIdx < appDemoDevAmount;devIdx++) {
							if (!((acl_group[i]->devnum^ACL_DEFAULT_DEVID)&(1<<devIdx))) {
								continue;
							}
							bcm_field_entry_reinstall(devIdx, ruleIndex);
							nam_syslog_dbg("acl rule reinstall %d  \n", ruleIndex);
							
							bcm_field_action_add(devIdx, ruleIndex, bcmFieldActionDscpNew, newdscp, 0);
							/*bcm_field_action_add(devIdx, ruleIndex, bcmFieldActionPrioPktNew, newpri, 0);*/
							bcm_field_action_add(devIdx, ruleIndex, bcmFieldActionCosQNew, cosqueue, 0);
							nam_syslog_dbg("action add newdscp %d\n", newdscp);
							/*nam_syslog_dbg("action add newpri %d\n", newpri);*/
							nam_syslog_dbg("action add cosqueue %d\n", cosqueue);
								
							bcm_field_entry_install(devIdx, ruleIndex);
							nam_syslog_dbg("acl rule install %d\n", ruleIndex);
						}
					}
				}
			}
		}
	}		 
#endif

     return ret;
}

/**********************************************************************************
 * nam_acl_qos_action_clear
 *
 * clear acl action
 *
 *	INPUT:
 *		unsigned int 		ruleIndex - acl rule index
 *
 *	OUTPUT:
 *
 *	RETURN:
 *		ACL_RETURN_CODE_SUCCESS
 *	NOTE:
 *
 **********************************************************************************/
unsigned int nam_acl_qos_action_clear
(
	unsigned int ruleIndex
)
{
	unsigned int   devIdx=0,appDemoDevAmount=0 ,ret = ACL_RETURN_CODE_SUCCESS;
	appDemoDevAmount = nam_asic_get_instance_num();
	
#ifdef DRV_LIB_CPSS	
	AclRuleList               *node=NULL;
	CPSS_DXCH_PCL_ACTION_STC	actionPtr2;
	unsigned int 				GetIndex = 0;
	CPSS_PCL_RULE_SIZE_ENT  	ruleSize;
	                            
	node = g_acl_rule[ruleIndex];

	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++) {

		ruleSize=CPSS_PCL_RULE_SIZE_EXT_E;
		GetIndex=cpssDxChPclRuleActionGet(devIdx,ruleSize,ruleIndex,CPSS_PCL_DIRECTION_INGRESS_E,&actionPtr2);
		if(GetIndex!=0){
			nam_syslog_dbg("cpssDxChPclRuleActionGet fail! GetIndex %d\n",GetIndex);
			return 1;
		}

		memset(&(actionPtr2.qos), 0, sizeof(CPSS_DXCH_PCL_ACTION_QOS_STC));

		ret= cpssDxChPclRuleActionUpdate(devIdx,ruleSize,ruleIndex,&actionPtr2);
		if(ret!=0){
			nam_syslog_dbg("cpssDxChPclRuleActionUpdate %d\n",ret);
			return 1;
		}			
	}  
#endif

#ifdef DRV_LIB_BCM
	struct acl_rule_info_s		 *ruleInfo = NULL;
	struct list_head			*Ptr = NULL;
	struct acl_group_list_s *groupPtr = NULL;
	unsigned int  add_index = 0, i = 0;
	
	nam_syslog_dbg("nam_qos_policer_entry_set ! \n");
	for (i = 0; i < MAX_GROUP_NUM; i++) {
		groupPtr = acl_group[i];
		if (NULL != groupPtr) {
			 __list_for_each(Ptr, &(groupPtr->list1)) {
				ruleInfo = list_entry(Ptr,struct acl_rule_info_s,list);
				if (NULL != ruleInfo) { 		
					if (ruleIndex == ruleInfo->ruleIndex) {
						for(devIdx=0;devIdx < appDemoDevAmount;devIdx++) {
							if (!((acl_group[i]->devnum^ACL_DEFAULT_DEVID)&(1<<devIdx))) {
								continue;
							}
							ret = bcm_field_action_remove(devIdx, ruleIndex, bcmFieldActionDscpNew);
							if (ACL_RETURN_CODE_SUCCESS != ret) {
								nam_syslog_dbg("bcm_field_action_remove fail ! \n");
								return ACL_RETURN_CODE_ERROR;
							}
							ret = bcm_field_action_remove(devIdx, ruleIndex, bcmFieldActionPrioPktNew);
							if (ACL_RETURN_CODE_SUCCESS != ret) {
								nam_syslog_dbg("bcm_field_action_remove fail ! \n");
								return ACL_RETURN_CODE_ERROR;
							}	
							ret = bcm_field_action_remove(devIdx, ruleIndex, bcmFieldActionCosQNew);
							if (ACL_RETURN_CODE_SUCCESS != ret) {
								nam_syslog_dbg("bcm_field_action_remove fail ! \n");
								return ACL_RETURN_CODE_ERROR;
							}	
							ret = bcm_field_entry_reinstall(devIdx, ruleIndex);
							if (ACL_RETURN_CODE_SUCCESS != ret) {
								nam_syslog_dbg("bcm_field_entry_reinstall fail ! \n");
								return ACL_RETURN_CODE_ERROR;
							}	
							nam_syslog_dbg("action remove dscp prio add cosq for rule %d\n", ruleIndex);
							nam_syslog_dbg("acl rule reinstall %d for unit %d \n", ruleIndex, devIdx);
							ret = bcm_field_entry_install(devIdx, ruleIndex);
							if (ACL_RETURN_CODE_SUCCESS != ret) {
								nam_syslog_dbg("bcm_field_entry_install fail ! \n");
								return ACL_RETURN_CODE_ERROR;
							}	
						}
					}
				}
			}
		}
	}							
#endif

     return ret;
}

/**********************************************************************************
 * nam_acl_mirror_action_update
 *
 * update acl mirror action
 *
 *	INPUT:
 *		unsigned int ruleIndex  - acl rule index
 *		unsigned int enable		- enable or disable
 *		int 		 unit		- dev number
 *
 *	OUTPUT:
 *
 *	RETURN:
 *		ACL_RETURN_CODE_SUCCESS
 *	NOTE:
 *
 **********************************************************************************/
unsigned int nam_acl_mirror_action_update
(
	unsigned int ruleIndex,
	unsigned int enable,
	int unit
)
{
	  unsigned int              devIdx=0,appDemoDevAmount=0, ret = 0;
        AclRuleList               *node=NULL;
		
#ifdef DRV_LIB_CPSS		
	CPSS_DXCH_PCL_ACTION_STC	actionPtr2;
	unsigned int 				GetIndex=0;
	CPSS_PCL_RULE_SIZE_ENT  	ruleSize;
	memset(&actionPtr2, 0, sizeof(CPSS_DXCH_PCL_ACTION_STC));
	appDemoDevAmount = nam_asic_get_instance_num();                            
	node = g_acl_rule[ruleIndex];
		/* save 0 1 rule for udp */
	if (ruleIndex >= (MAX_ACL_RULE_NUMBER/2)) {
		ruleIndex += ACL_SAVE_FOR_UDP_BPDU*2;
	}
	else {
		ruleIndex += ACL_SAVE_FOR_UDP_BPDU;
	}  
	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++) {
		ruleSize=CPSS_PCL_RULE_SIZE_EXT_E;
		
		GetIndex=cpssDxChPclRuleActionGet(devIdx,ruleSize,ruleIndex,CPSS_PCL_DIRECTION_INGRESS_E,&actionPtr2);
		if(GetIndex!=0){
			nam_syslog_err("cpssDxChPclRuleActionGet fail! GetIndex %d\n",GetIndex);
			return 1;
		}
		
		/*actionPtr2.mirror.cpuCode = CPSS_NET_MIRROR_HOP_BY_HOP_E;*/
		actionPtr2.pktCmd = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
		actionPtr2.mirror.mirrorToRxAnalyzerPort = enable;
		actionPtr2.mirror.cpuCode = CPSS_NET_FIRST_USER_DEFINED_E;

		ret= cpssDxChPclRuleActionUpdate(devIdx,ruleSize,ruleIndex,&actionPtr2);
		if(ret!=0){
			nam_syslog_err("cpssDxChPclRuleActionUpdate %d\n",ret);
			return 1;
		}			
	} 
	nam_syslog_dbg("ruleindex is %d ret is %d\n", ruleIndex, ret);
	
	CPSS_DXCH_PCL_ACTION_STC	actionPtrt;
	memset(&actionPtrt, 0, sizeof(CPSS_DXCH_PCL_ACTION_STC));
	cpssDxChPclRuleActionGet(0, ruleSize, ruleIndex, 0, &actionPtrt);
	nam_syslog_dbg("mirror port is enable %d \n", actionPtrt.mirror.mirrorToRxAnalyzerPort);
#endif

#ifdef DRV_LIB_BCM
	int mirrorport = 0, modid = 0;
	node = g_acl_rule[ruleIndex];
	mirrorport = node->ActionPtr.actionInfo.mirror.portNum;
	modid = node->ActionPtr.actionInfo.mirror.modid;
	if (enable) {
		 /*bcm_field_action_remove_all(unit, ruleIndex);*/
		ret = bcm_field_entry_reinstall(unit, ruleIndex);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_entry_create_id \n");
		}		 
		ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionMirrorIngress, modid, mirrorport);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_entry_create_id \n");
		}		  		 
		ret = bcm_field_entry_install(unit, ruleIndex);	 
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_entry_create_id \n");
		}		  		 
	}
	else {
		ret =  bcm_field_entry_reinstall(unit, ruleIndex);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_entry_create_id \n");
		}
		ret = bcm_field_action_remove(unit, ruleIndex, bcmFieldActionMirrorIngress);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_entry_create_id \n");
		}		  		 		  		 	 
		ret = bcm_field_entry_install(unit, ruleIndex);	
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_entry_create_id \n");
		}		  		 
	}
#endif

     return ret;
}
/*
unsigned int  nam_acl_time_action_update(unsigned int ruleIndex)
{
	  unsigned int			    devIdx=0,appDemoDevAmount=0;
	  AclRuleList				*node=NULL;
	  CPSS_DXCH_PCL_ACTION_STC	actionPtr2;
	  unsigned int				GetIndex=0,ret;
	  CPSS_PCL_RULE_SIZE_ENT	ruleSize;
	  
	  appDemoDevAmount = nam_asic_get_instance_num();							 
	  node = g_acl_rule[ruleIndex];
	  
	  for(devIdx=0;devIdx < appDemoDevAmount;devIdx++)
	  {		
		ruleSize=CPSS_PCL_RULE_SIZE_EXT_E;
	
		GetIndex=cpssDxChPclRuleActionGet(devIdx,ruleSize,ruleIndex,CPSS_PCL_DIRECTION_INGRESS_E,&actionPtr2);
		if(GetIndex!=0){
			nam_syslog_dbg("cpssDxChPclRuleActionGet fail! GetIndex %d\n",GetIndex);
			return 1;
		}
		if(ACL_ACTION_TYPE_PERMIT==node->ActionPtr.actionType){
			actionPtr2.pktCmd =CPSS_PACKET_CMD_DROP_HARD_E;
		}
		else if(ACL_ACTION_TYPE_DENY==node->ActionPtr.actionType){
			actionPtr2.pktCmd =CPSS_PACKET_CMD_FORWARD_E;

		}	 
		 ret= cpssDxChPclRuleActionUpdate(devIdx,ruleSize,ruleIndex,&actionPtr2);
		 if(ret!=0){
			nam_syslog_dbg("cpssDxChPclRuleActionUpdate %d\n",ret);
			return 1;
		 }			
	  }  
		
	 return ret;

}
*/

/**********************************************************************************
 * nam_drv_acl_xxx_listDeal
 *
 * set single rule tcam for broadcom
 *
 *	INPUT:
 *		struct ACCESS_LIST_STC	*node	 - struct for acl rule
 *		unsigned int			aclId	 - acl index
 *		int 					unit	 - dev number
 *		unsigned int			dir_info - ingress or egress group
 *
 *	OUTPUT:
 *
 *	RETURN:
 *		ACL_RETURN_CODE_SUCCESS	- set rule tcam success 
 *		ACL_RETURN_CODE_ERROR		- set rule tcam fail
 *		
 *	NOTE:
 *
 **********************************************************************************/
#ifdef DRV_LIB_BCM
unsigned int nam_drv_acl_qos_listDeal
(
	struct ACCESS_LIST_STC *node,
	unsigned int aclId, 
	int unit,
	unsigned int dir_info
)
{
	unsigned int						ruleIndex = 0, profileIndex = 0;
	unsigned int						i = 0,ret = ACL_RETURN_CODE_SUCCESS;
	unsigned int						pkd = ACL_ACTION_TYPE_PERMIT;
	
	uint8 newdscp = 0, srcdscpdata = 0, srcdscpmask = 0;
	bcm_vlan_t newpri = 0, outervlandata = 0, outervlanmask = 0;
	int policerEnable = 0, cosqueue = 0;
	int meternum = 0, rate = 0, burst = 0;
	ACL_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT modifyDscp, modifyUp;

	ruleIndex = node->ruleIndex;

	nam_syslog_dbg("acl rule is ingress-qos\n");

	ret = bcm_field_entry_create_id(unit, aclId, ruleIndex);
	if (ACL_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR !bcm_field_entry_create_id \n");
	}
	
	outervlandata = ((node->TcamPtr.ruleInfo.stdIpv4L4.rule.UP)<<13)&0xe000;
	srcdscpdata = (node->TcamPtr.ruleInfo.stdIpv4L4.rule.DSCP)<<2;
	
	outervlanmask = 0xf000;
	srcdscpmask = 0xff;

	profileIndex = node->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
	newdscp = g_qos_profile[profileIndex]->dscp;
	newpri = g_qos_profile[profileIndex]->userPriority;
	cosqueue = g_qos_profile[profileIndex]->trafficClass;
	nam_syslog_dbg("profileIndex %d\n", profileIndex);
	nam_syslog_dbg("newdscp %d\n", newdscp);
	nam_syslog_dbg("newpri %d\n", newpri);
	nam_syslog_dbg("cosqueue %d\n", cosqueue);

	modifyDscp = node->ActionPtr.actionInfo.qos.modifyDscp;
	modifyUp = node->ActionPtr.actionInfo.qos.modifyUp;
	policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;
	if (policerEnable) {
		ret = bcm_field_meter_create(unit, ruleIndex);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_meter_create \n");
		}
		meternum  = node->ActionPtr.policerPtr.policerId;
		rate = g_policer[meternum]->cir;
		burst = g_policer[meternum]->cbs;
	}

	if (0 != outervlandata) {
		ret = bcm_field_qualify_OuterVlan(unit, ruleIndex, outervlandata, outervlanmask);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_qualify_OuterVlan \n");
		}
		nam_syslog_dbg("outervlandata 0x%04x\n", outervlandata);
		nam_syslog_dbg("outervlanmask 0x%04x\n", outervlanmask);
	}
	if (0 != srcdscpdata) {
		ret = bcm_field_qualify_DSCP(unit, ruleIndex, srcdscpdata, srcdscpmask);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_qualify_DSCP \n");
		}
		nam_syslog_dbg("srcdscpdata 0x%02x\n", srcdscpdata);
		nam_syslog_dbg("srcdscpmask 0x%02x\n", srcdscpmask);
	}
	
	if (ACL_PACKET_ATTRIBUTE_MODIFY_ENABLE_E == modifyDscp) {
		ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionDscpNew, newdscp, 0);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_action_add \n");
		}
	}
	if (ACL_PACKET_ATTRIBUTE_MODIFY_ENABLE_E == modifyUp) {
		ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionPrioPktNew, newpri, 0);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_action_add \n");
		}
	}	
	/*ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionCosQNew, cosqueue, 0);
	if (ACL_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR !bcm_field_action_add \n");
	}*/
	ret = bcm_field_entry_install(unit, ruleIndex);
	if (ACL_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR !bcm_field_entry_install \n");
	}
	
	return ret;
}

unsigned int nam_drv_acl_arp_listDeal
(
	struct ACCESS_LIST_STC *node,
	unsigned int aclId, 
	int unit,
	unsigned int dir_info
)
{
	unsigned int						ruleIndex = 0, profileIndex = 0;
	unsigned int						i = 0, counterindex = 0, ret = ACL_RETURN_CODE_SUCCESS;
	unsigned int						pkd = ACL_ACTION_TYPE_PERMIT;
	
	bcm_port_t inportdata  = 0, inportmask = 0;
	uint16 ethertypedata = 0, ethertypemask = 0; 
	bcm_vlan_t newpri = 0,outervlandata = 0, outervlanmask = 0;
	unsigned char  srcmacdata[6] , srcmacmask[6];
	int policerEnable = 0, cosqueue = 0;
	int redirectportnum = 0, modid = 0;
	int meternum = 0, rate = 0, burst = 0;
	uint8 newdscp = 0;

	ruleIndex = node->ruleIndex;
	pkd = node->ActionPtr.actionType;

	ret = bcm_field_entry_create_id(unit, aclId, ruleIndex);
	if (ACL_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR !bcm_field_entry_create_id \n");
	}
	ethertypemask = 0xffff;
	/*
	if (ACL_ANY_PORT_CHAR == (node->TcamPtr.ruleInfo.stdNotIp.rule.portno)) {
		inportmask = 0;
	}
	else {
		inportmask = node->TcamPtr.ruleInfo.stdNotIp.mask.maskPortno;
	}
	*/
	inportmask = node->TcamPtr.ruleInfo.stdNotIp.mask.maskPortno;
	outervlanmask = node->TcamPtr.ruleInfo.stdNotIp.mask.maskVlanId;

	ethertypedata = 0x0806;
	inportdata = node->TcamPtr.ruleInfo.stdNotIp.rule.portno;
	outervlandata = node->TcamPtr.ruleInfo.stdNotIp.rule.vlanId;
		
	for (i=0; i<6; i++) {
		srcmacmask[i] = node->TcamPtr.ruleInfo.stdNotIp.mask.maskSMac[i];
		srcmacdata[i] = node->TcamPtr.ruleInfo.stdNotIp.rule.sMac[i];
	}	

	policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;
	if (policerEnable) {
		ret = bcm_field_meter_create(unit, ruleIndex);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_meter_create \n");
		}
		meternum  = node->ActionPtr.policerPtr.policerId;
		rate = g_policer[meternum]->cir;
		burst = g_policer[meternum]->cbs;
		nam_syslog_dbg("bcm meter create %d\n", meternum);
		nam_syslog_dbg("meter cir %u, cbs %u \n", rate, burst);
	}

	ret = bcm_field_qualify_EtherType(unit, ruleIndex, ethertypedata, ethertypemask);
	if (ACL_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR !bcm_field_qualify_EtherType \n");
	}
	nam_syslog_dbg("acl rule type is arp \n");
	nam_syslog_dbg("ethertypedata 0x%04x\n", ethertypedata);
	nam_syslog_dbg("ethertypemask 0x%04x\n", ethertypemask);
	
	ret = bcm_field_qualify_SrcMac(unit, ruleIndex, srcmacdata, srcmacmask);
	if (ACL_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR !bcm_field_qualify_SrcMac \n");
	}
	nam_syslog_dbg("srcmacdata 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x\n", 
		srcmacdata[0],srcmacdata[1],srcmacdata[2],srcmacdata[3],srcmacdata[4],srcmacdata[5]);
	nam_syslog_dbg("srcmacmask 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x\n", 
		srcmacmask[0],srcmacmask[1],srcmacmask[2],srcmacmask[3],srcmacmask[4],srcmacmask[5]);
	
	if (0 != inportmask) {
		ret = bcm_field_qualify_InPort(unit, ruleIndex, inportdata, inportmask);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_qualify_InPort \n");
		}
		nam_syslog_dbg("inportdata 0x%04x\n", inportdata);
		nam_syslog_dbg("inportmask 0x%04x\n", inportmask);
	}

	if (0 != outervlanmask) {
		ret = bcm_field_qualify_OuterVlan(unit, ruleIndex, outervlandata, outervlanmask);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_qualify_OuterVlan \n");
		}
		nam_syslog_dbg("outervlandata 0x%04x\n", outervlandata);
		nam_syslog_dbg("outervlanmask 0x%04x\n", outervlanmask);
	}
	
	if (ACL_ACTION_TYPE_PERMIT == pkd) {
		if (policerEnable) {
			if (OUT_PROFILE_DROP == g_policer[meternum]->cmd) {
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRpDrop, 0, 0);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add \n");
				}
				nam_syslog_dbg("action add red packet drop\n");
			}
			else if (OUT_PROFILE_REMAP_ENTRY == g_policer[meternum]->cmd) {							
				profileIndex = g_policer[meternum]->qosProfile;
				newdscp = g_qos_profile[profileIndex]->dscp;
				newpri = g_qos_profile[profileIndex]->userPriority;
				cosqueue = g_qos_profile[profileIndex]->trafficClass;
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRpPrioPktNew, newpri, 0);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add \n");
				}
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRpDscpNew, newdscp, 0);\
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add \n");
				}
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRpCosQNew, cosqueue, 0);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add \n");
				}
				nam_syslog_dbg("action add new pri dscp cosq\n");
			}
			ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionMeterConfig, 1, 0);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_action_add \n");
			}
			nam_syslog_dbg("action add meter config \n");
			ret = bcm_field_meter_set(unit, ruleIndex, BCM_FIELD_METER_PEAK, rate, burst);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_meter_set \n");
			}
			nam_syslog_dbg("meter cir %u, cbs %u \n", rate, burst);
			if (g_policer[meternum]->counterEnable) {
				/*ret = bcm_field_counter_create(unit, ruleIndex);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_counter_create \n");
				}*/
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionUpdateCounter, BCM_FIELD_COUNTER_MODE_GREEN_RED, 0);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add \n");
				}	
				nam_syslog_dbg("bcm_field_action_add bcmFieldActionUpdateCounter  mode  BCM_FIELD_COUNTER_MODE_GREEN_RED\n");
				counterindex = g_policer[meternum]->counterSetIndex;
				/*   counter_num - (IN) Choice of upper (counter_num=1) or lower
                           (counter_num=0) of the counter pair.                 
				valptmp[0] = g_counter[counterindex]->inProfileBytesCnt;
				valptmp[1] = g_counter[counterindex]->outOfProfileBytesCnt;*/
				nam_syslog_dbg("inProfileBytesCnt is %u, outOfProfileBytesCnt is %u \n", 
					g_counter[counterindex]->inProfileBytesCnt, g_counter[counterindex]->outOfProfileBytesCnt);
				/*memcpy(&valp, valptmp, sizeof(long long));*/
				/*valp = 100;*/
				ret = bcm_field_counter_set(unit, ruleIndex, 0, g_counter[counterindex]->outOfProfileBytesCnt);/*g_counter[counterindex]->outOfProfileBytesCnt;*/
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_counter_set \n");
				}
				ret = bcm_field_counter_set(unit, ruleIndex, 1, g_counter[counterindex]->inProfileBytesCnt);/*g_counter[counterindex]->inProfileBytesCnt;*/
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_counter_set \n");
				}
				/*nam_syslog_dbg("action add counter %d, valp is %llu\n", counterindex, valp);*/
			}

		}
		if (32 == node->appendInfo.appendType) {
			profileIndex = node->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
			newdscp = g_qos_profile[profileIndex]->dscp;
			newpri = g_qos_profile[profileIndex]->userPriority;
			cosqueue = g_qos_profile[profileIndex]->trafficClass;
			/*ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionPrioPktNew, newpri, 0);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_action_add \n");
			}*/
			ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionDscpNew, newdscp, 0);
			nam_syslog_dbg("bcm_field_action_add bcmFieldActionDscpNew unit %d, ruleindex %d, newdscp %d",
				unit, ruleIndex, newdscp);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_action_add  bcmFieldActionDscpNew ret \n", ret);
			}
			ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionCosQNew, cosqueue, 0);
			nam_syslog_dbg("bcm_field_action_add bcmFieldActionCosQNew unit %d, ruleindex %d, cosqueue %d",
				unit, ruleIndex, cosqueue);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_action_add bcmFieldActionCosQNew ret \n", ret);
			}
			/*nam_syslog_dbg("append action add new pri dscp cosq\n");*/
		}
	}
	else if (ACL_ACTION_TYPE_DENY == pkd) {			
		ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionDrop, 0, 0);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_action_add \n");
		}
		nam_syslog_dbg("action add drop \n");
	}	
	else if (ACL_ACTION_TYPE_REDIRECT == pkd) {			
		redirectportnum = node->ActionPtr.actionInfo.redirect.portNum;
		modid = node->ActionPtr.actionInfo.redirect.modid;
		ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRedirectPort, modid, redirectportnum);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_action_add \n");
		}
		nam_syslog_dbg("action add redirectpbmp \n");
	}
	else if (ACL_ACTION_TYPE_TRAP_TO_CPU == pkd) {
		ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionCopyToCpu, 0, 0);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_action_add \n");
		}
		nam_syslog_dbg("action add trap to cpu \n");
	}
	
	ret = bcm_field_entry_install(unit, ruleIndex);
	if (ACL_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR !bcm_field_entry_install \n");
	}
	nam_syslog_dbg("install acl rule  %d \n", ruleIndex);
	
	return ret;		
}

unsigned int nam_drv_acl_stdNotIp_ListDeal
(
	struct ACCESS_LIST_STC *node,
	unsigned int aclId, 
	int unit,
	unsigned int dir_info
)
{
	unsigned int						ruleIndex = 0, profileIndex = 0;
	unsigned int						i = 0, counterindex = 0,ret = ACL_RETURN_CODE_SUCCESS;
	unsigned int						pkd = ACL_ACTION_TYPE_PERMIT;
	
	uint16 ethertypedata = 0, ethertypemask = 0; 
	unsigned char  srcmacdata[6] , srcmacmask[6];
	unsigned char  dstmacdata[6] , dstmacmask[6];
	int policerEnable = 0;
	int redirectportnum = 0, modid = 0;
	int meternum = 0, rate = 0, burst = 0;
	uint8 newdscp = 0;
	bcm_vlan_t newpri = 0;
	int cosqueue = 0;

	ruleIndex = node->ruleIndex;

	ret = bcm_field_entry_create_id(unit, aclId, ruleIndex);
	if (ACL_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR !bcm_field_entry_create_id \n");
	}
	
	pkd=node->ActionPtr.actionType;

	ethertypemask = 0xffff;
	ethertypedata = 0;
		
	for (i=0; i<6; i++) {
		srcmacmask[i] = node->TcamPtr.ruleInfo.stdNotIp.mask.maskSMac[i];
		srcmacdata[i] = node->TcamPtr.ruleInfo.stdNotIp.rule.sMac[i];
	}
	for (i=0; i<6; i++) {
		dstmacmask[i] = node->TcamPtr.ruleInfo.stdNotIp.mask.maskDMac[i];
		dstmacdata[i] = node->TcamPtr.ruleInfo.stdNotIp.rule.dMac[i];
	}

	policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;
	if (policerEnable) {
		ret = bcm_field_meter_create(unit, ruleIndex);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_meter_create \n");
		}
		meternum  = node->ActionPtr.policerPtr.policerId;
		rate = g_policer[meternum]->cir;
		burst = g_policer[meternum]->cbs;
	}

	ret = bcm_field_qualify_EtherType(unit, ruleIndex, ethertypedata, ethertypemask);
	if (ACL_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR !bcm_field_qualify_EtherType \n");
	}
	nam_syslog_dbg("acl rule type is ethernet \n");

	ret = bcm_field_qualify_SrcMac(unit, ruleIndex, srcmacdata, srcmacmask);
	if (ACL_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR !bcm_field_qualify_SrcMac \n");
	}
	nam_syslog_dbg("srcmacdata 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x\n", 
		srcmacdata[0],srcmacdata[1],srcmacdata[2],srcmacdata[3],srcmacdata[4],srcmacdata[5]);
	nam_syslog_dbg("srcmacmask 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x\n", 
		srcmacmask[0],srcmacmask[1],srcmacmask[2],srcmacmask[3],srcmacmask[4],srcmacmask[5]);
		
	ret = bcm_field_qualify_DstMac(unit, ruleIndex, dstmacdata, dstmacmask);
	if (ACL_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR !bcm_field_qualify_DstMac \n");
	}
	nam_syslog_dbg("dstmacdata 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x\n", 
		dstmacdata[0],dstmacdata[1],dstmacdata[2],dstmacdata[3],dstmacdata[4],dstmacdata[5]);
	nam_syslog_dbg("dstmacmask 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x\n", 
		dstmacmask[0],dstmacmask[1],dstmacmask[2],dstmacmask[3],dstmacmask[4],dstmacmask[5]);
	
	if (ACL_ACTION_TYPE_PERMIT == pkd) {
		if (policerEnable) {
			if (OUT_PROFILE_DROP == g_policer[meternum]->cmd) {
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRpDrop, 0, 0);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add \n");
				}
				nam_syslog_dbg("action add red packet drop\n");
			}
			else if (OUT_PROFILE_REMAP_ENTRY == g_policer[meternum]->cmd) {							
				profileIndex = g_policer[meternum]->qosProfile;
				newdscp = g_qos_profile[profileIndex]->dscp;
				newpri = g_qos_profile[profileIndex]->userPriority;
				cosqueue = g_qos_profile[profileIndex]->trafficClass;
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRpPrioPktNew, newpri, 0);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add \n");
				}
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRpDscpNew, newdscp, 0);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add \n");
				}
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRpCosQNew, cosqueue, 0);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add \n");
				}
				nam_syslog_dbg("action add new pri dscp cosq\n");
			}
			ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionMeterConfig, 1, 0);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_action_add \n");
			}
			nam_syslog_dbg("action add meter config \n");
			ret = bcm_field_meter_set(unit, ruleIndex, BCM_FIELD_METER_PEAK, rate, burst);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_meter_set \n");
			}
			nam_syslog_dbg("meter cir %u, cbs %u \n", rate, burst);
			if (g_policer[meternum]->counterEnable) {
				/*ret = bcm_field_counter_create(unit, ruleIndex);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_counter_create \n");
				}*/
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionUpdateCounter, BCM_FIELD_COUNTER_MODE_GREEN_RED, 0);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add \n");
				}	
				nam_syslog_dbg("bcm_field_action_add bcmFieldActionUpdateCounter  mode  BCM_FIELD_COUNTER_MODE_GREEN_RED\n");
				counterindex = g_policer[meternum]->counterSetIndex;
				/*   counter_num - (IN) Choice of upper (counter_num=1) or lower
                           (counter_num=0) of the counter pair.                 
				valptmp[0] = g_counter[counterindex]->inProfileBytesCnt;
				valptmp[1] = g_counter[counterindex]->outOfProfileBytesCnt;*/
				nam_syslog_dbg("inProfileBytesCnt is %u, outOfProfileBytesCnt is %u \n", 
					g_counter[counterindex]->inProfileBytesCnt, g_counter[counterindex]->outOfProfileBytesCnt);
				/*memcpy(&valp, valptmp, sizeof(long long));*/
				/*valp = 100;*/
				ret = bcm_field_counter_set(unit, ruleIndex, 0, g_counter[counterindex]->outOfProfileBytesCnt);/*g_counter[counterindex]->outOfProfileBytesCnt;*/
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_counter_set \n");
				}
				ret = bcm_field_counter_set(unit, ruleIndex, 1, g_counter[counterindex]->inProfileBytesCnt);/*g_counter[counterindex]->inProfileBytesCnt;*/
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_counter_set \n");
				}
				/*nam_syslog_dbg("action add counter %d, valp is %llu\n", counterindex, valp);*/
			}

		}
		if (32 == node->appendInfo.appendType) {
			profileIndex =	node->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
			newdscp = g_qos_profile[profileIndex]->dscp;
			newpri = g_qos_profile[profileIndex]->userPriority;
			cosqueue = g_qos_profile[profileIndex]->trafficClass;
			/*ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionPrioPktNew, newpri, 0);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_action_add \n");
			}*/
			ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionDscpNew, newdscp, 0);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_action_add \n");
			}			
			ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionCosQNew, cosqueue, 0);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_action_add \n");
			}			
			nam_syslog_dbg("append action add new pri dscp cosq\n");
		}
	}
	else if (ACL_ACTION_TYPE_DENY == pkd) {			
		ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionDrop, 0, 0);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_action_add \n");
		}		
		nam_syslog_dbg("action add drop \n");
	}	
	else if (ACL_ACTION_TYPE_REDIRECT == pkd) {			
		redirectportnum = node->ActionPtr.actionInfo.redirect.portNum;
		modid = node->ActionPtr.actionInfo.redirect.modid;
		ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRedirectPort, modid, redirectportnum);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_action_add \n");
		}
		nam_syslog_dbg("action add redirectpbmp \n");
	}
	else if (ACL_ACTION_TYPE_TRAP_TO_CPU == pkd) {
		ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionCopyToCpu, 0, 0);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_action_add \n");
		}
		nam_syslog_dbg("action add trap to cpu \n");
	}
	
	ret = bcm_field_entry_install(unit, ruleIndex);
	if (ACL_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR !bcm_field_action_add \n");
	}
	nam_syslog_dbg("install acl rule  %d \n", ruleIndex);

	return ret;				
}

unsigned int nam_drv_acl_stdIpL2_ListDeal
(
	struct ACCESS_LIST_STC *node, 
	unsigned int aclId, 
	int unit,
	unsigned int dir_info
)
{
	unsigned char 						devIdx = 0,appDemoDevAmount = 0;
	unsigned long 						ruleIndex = 0;	
	unsigned int		 				i = 0,result = ACL_RETURN_CODE_SUCCESS;
/*	
       unsigned long           			pkd;
	CPSS_PCL_RULE_SIZE_ENT  			ruleSize;
	// table cpssDxChPclRule global variables 
	CPSS_DXCH_PCL_RULE_FORMAT_UNT	    *maskData=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_UNT	    *patternData=NULL;
	CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
	CPSS_DXCH_PCL_ACTION_STC		    *actionPtr=NULL;
	CPSS_PACKET_CMD_ENT 				cmdPkd;
		
	CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC  *maskStdIpL2 = NULL,*patternStdIpL2 = NULL;
	

	maskData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(maskData==NULL){
		nam_syslog_dbg("malloc fail!\n");
		return 1;
	}
	patternData = (CPSS_DXCH_PCL_RULE_FORMAT_UNT*)malloc(sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	if(patternData==NULL){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		maskData=NULL;
		return 1;
	}
	actionPtr =(CPSS_DXCH_PCL_ACTION_STC*)malloc(sizeof(CPSS_DXCH_PCL_ACTION_STC));
	if(actionPtr==NULL){
		nam_syslog_dbg("malloc fail!\n");
		free(maskData);
		free(patternData);
		maskData=NULL;
		patternData=NULL;
		return 1;
	}
	memset(maskData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(patternData,0,sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
	memset(actionPtr,0,sizeof(CPSS_DXCH_PCL_ACTION_STC));	

	maskStdIpL2		= &(maskData->ruleStdIpL2Qos);
	patternStdIpL2		= &(patternData->ruleStdIpL2Qos);

	appDemoDevAmount = nam_asic_get_instance_num(); 

	pkd=node->ActionPtr.actionType;

	switch(pkd)
		{
			case ACL_ACTION_TYPE_PERMIT:			      cmdPkd=CPSS_PACKET_CMD_FORWARD_E; break;
			case ACL_ACTION_TYPE_DENY:				      cmdPkd=CPSS_PACKET_CMD_DROP_HARD_E;break;
			case ACL_ACTION_TYPE_TRAP_TO_CPU:   	      cmdPkd=CPSS_PACKET_CMD_TRAP_TO_CPU_E;break;
		       case ACL_ACTION_TYPE_MIRROR_TO_ANALYZER:  cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;
			case ACL_ACTION_TYPE_REDIRECT:			      cmdPkd=CPSS_PACKET_CMD_FORWARD_E;break;	 
			default:
					  break;
		}
	
	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++)
		{		
			
		// map input arguments to locals 
			//devNum;
			ruleIndex = node->ruleIndex;
			ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
			maskStdIpL2->common.pclId = 0x3FF;	
			
			for(i=0;i<6;i++){
				maskStdIpL2->macDa.arEther[i]=node->TcamPtr.ruleInfo.stdNotIp.mask.maskDMac[i];
				maskStdIpL2->macSa.arEther[i]=node->TcamPtr.ruleInfo.stdNotIp.mask.maskSMac[i];
				patternStdIpL2->macDa.arEther[i]=node->TcamPtr.ruleInfo.stdNotIp.rule.dMac[i];
				patternStdIpL2->macSa.arEther[i]=node->TcamPtr.ruleInfo.stdNotIp.rule.sMac[i];
			}	

			patternStdIpL2->common.pclId = aclId;
				
			actionPtr->pktCmd = cmdPkd;			 
			if(ACL_ACTION_TYPE_PERMIT==pkd){
				actionPtr->policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;
				actionPtr->policer.policerId    = node->ActionPtr.policerPtr.policerId ; 
			}
		
			else if(ACL_ACTION_TYPE_MIRROR_TO_ANALYZER==pkd){

				//actionPtr.policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;
				//actionPtr.policer.policerId    = node->ActionPtr.policerPtr.policerId ;	

				 actionPtr->mirror.mirrorToRxAnalyzerPort = 1;					
			}
			else if(ACL_ACTION_TYPE_REDIRECT==pkd){			
					
					//actionPtr.policer.policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;
					//actionPtr.policer.policerId    = node->ActionPtr.policerPtr.policerId ; 
					//note: only port redirect to egress interface,that's policy switching,no router redirect
					 actionPtr->redirect.redirectCmd =  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
					 actionPtr->redirect.data.outIf.outInterface.devPort.devNum = 0;
					 actionPtr->redirect.data.outIf.outInterface.devPort.portNum = node->ActionPtr.actionInfo.redirect.portNum;
					 //actionPtr.redirect.data.outIf.outInterface.vlanId = 1;
			}
			else if(ACL_ACTION_TYPE_TRAP_TO_CPU	== pkd) {
				actionPtr->mirror.cpuCode =node->ActionPtr.actionInfo.trap.cpucode;
			}
			//qos
			
			actionPtr->qos.modifyDscp=(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyDscp);
			actionPtr->qos.modifyUp= (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(node->ActionPtr.actionInfo.qos.modifyUp);
			actionPtr->qos.qos.ingress.profileIndex = node->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
			actionPtr->qos.qos.ingress.profileAssignIndex = 1;
			actionPtr->qos.qos.ingress.profilePrecedence	= (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)(node->ActionPtr.actionInfo.qos.qos.ingress.profilePrecedence);
			
			// call cpss api function 
			
			ruleSize=CPSS_PCL_RULE_SIZE_STD_E;
					
			result = cpssDxChPclRuleSet(devIdx, ruleFormat, ruleIndex, maskData,
																patternData, actionPtr);		
				
			if(result!=0)
				nam_syslog_dbg("cpssDxChPclRuleSet ERROR!\n");
			nam_syslog_dbg("cpssDxChPclRuleSet result is %d\n",result);
		}
	free(maskData);
	free(patternData);
	free(actionPtr);	
	maskData=NULL;
	patternData=NULL;
	actionPtr=NULL;
*/
	return result;		
}

unsigned int nam_drv_acl_extNotIpv6_ListDeal
(
	struct ACCESS_LIST_STC * node,
	unsigned int aclId, 
	int unit,
	unsigned int dir_info
)
{
	unsigned long 	ruleIndex = 0, i = 0, pkd = ACL_ACTION_TYPE_PERMIT, profileIndex = 0;	
	unsigned int	ipty = 0, counterindex = 0,ret = ACL_RETURN_CODE_SUCCESS;
	unsigned char	redirectPort=0;
	bcm_port_t inportdata  = 0, inportmask = 0;
	uint16 ethertypedata = 0, ethertypemask = 0; 
	bcm_vlan_t outervlandata = 0, outervlanmask = 0;
	uint8	ipprotocoldata = 0, ipprotocolmask = 0;
	bcm_l4_port_t l4srcportdata = 0, l4srcportmask = 0;
	bcm_l4_port_t l4dstportdata = 0, l4dstportmask = 0;
	bcm_ip_t srcipdata = 0, srcipmask = 0;
	bcm_ip_t dstipdata = 0, dstipmask = 0;
	unsigned char  srcmacdata[6] , srcmacmask[6];
	unsigned char  dstmacdata[6] , dstmacmask[6];
	int policerEnable = 0;
	int redirectportnum = 0, modid = 0;
	int meternum = 0, rate = 0, burst = 0;
	uint8 newdscp = 0;
	bcm_vlan_t newpri = 0;
	int cosqueue = 0;

	ipty = node->TcamPtr.packetType;
	ruleIndex = node->ruleIndex;
	pkd=node->ActionPtr.actionType;
	
	ret = bcm_field_entry_create_id(unit, aclId, ruleIndex);
	if (ACL_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR !bcm_field_entry_create_id \n");
	}

	switch(ipty) {
			case ACL_TCAM_RULE_PACKETTYPE_IP:		ipprotocoldata = 0;break;
			case ACL_TCAM_RULE_PACKETTYPE_UDP:		ipprotocoldata = 0x11;break;
			case ACL_TCAM_RULE_PACKETTYPE_TCP:		ipprotocoldata = 0x06;break;
			case ACL_TCAM_RULE_PACKETTYPE_ICMP: 	ipprotocoldata = 0x01;break;
			case ACL_TCAM_RULE_PACKETTYPE_ETHERNET: ipprotocoldata = 0;break;
			default:
					  break;
	}
	
	ethertypemask = 0xffff;
	srcipmask = node->TcamPtr.ruleInfo.extNotIpv6.mask.maskSip;
	dstipmask = node->TcamPtr.ruleInfo.extNotIpv6.mask.maskDip;
	l4dstportmask = node->TcamPtr.ruleInfo.extNotIpv6.mask.maskDstPort;
	if ((ACL_TCAM_RULE_PACKETTYPE_TCP == ipty)||(ACL_TCAM_RULE_PACKETTYPE_UDP == ipty)) {
		l4srcportmask = node->TcamPtr.ruleInfo.extNotIpv6.mask.maskSrcPort;
	}
	
	if ((ACL_TCAM_RULE_PACKETTYPE_TCP == ipty)||
		(ACL_TCAM_RULE_PACKETTYPE_UDP == ipty)) {
		ipprotocolmask = 0xff;
	}
	else {
		ipprotocolmask = 0;
	}
	inportmask = node->TcamPtr.ruleInfo.extNotIpv6.mask.maskPortNo;
	outervlanmask = node->TcamPtr.ruleInfo.extNotIpv6.mask.maskVlanId;

	for (i=0; i<6; i++) {
		srcmacmask[i] = node->TcamPtr.ruleInfo.extNotIpv6.mask.maskSmac[i];
		srcmacdata[i] = node->TcamPtr.ruleInfo.extNotIpv6.rule.sMac[i];
	}
	for (i=0; i<6; i++) {
		dstmacmask[i] = node->TcamPtr.ruleInfo.extNotIpv6.mask.maskDmac[i];
		dstmacdata[i] = node->TcamPtr.ruleInfo.extNotIpv6.rule.dMac[i];
	}

	ethertypedata = 0;
	srcipdata = node->TcamPtr.ruleInfo.extNotIpv6.rule.Sip;
	dstipdata = node->TcamPtr.ruleInfo.extNotIpv6.rule.Dip;
	l4dstportdata = node->TcamPtr.ruleInfo.extNotIpv6.rule.DstPort;
	if ((ACL_TCAM_RULE_PACKETTYPE_TCP == ipty)||(ACL_TCAM_RULE_PACKETTYPE_UDP == ipty)) {
		l4srcportdata = node->TcamPtr.ruleInfo.extNotIpv6.rule.SrcPort;
	}
	inportdata = node->TcamPtr.ruleInfo.extNotIpv6.rule.portNo;
	outervlandata = node->TcamPtr.ruleInfo.extNotIpv6.rule.vlanId;
	
	policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;
	if (policerEnable) {
		ret = bcm_field_meter_create(unit, ruleIndex);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_meter_create \n");
		}
		meternum  = node->ActionPtr.policerPtr.policerId;
		rate = g_policer[meternum]->cir;
		burst = g_policer[meternum]->cbs;
	}

	
	if (0 == ipprotocoldata) {
		if (ACL_TCAM_RULE_PACKETTYPE_ETHERNET == ipty) {
			ret = bcm_field_qualify_EtherType(unit, ruleIndex, ethertypedata, ethertypemask);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_qualify_EtherType \n");
			}
			nam_syslog_dbg("ethertypedata 0x%04x\n", ethertypedata);
			nam_syslog_dbg("ethertypemask 0x%04x\n", ethertypemask);
		}
		else if (ACL_TCAM_RULE_PACKETTYPE_IP == ipty) {
			ethertypedata = 0x0800;
			ret = bcm_field_qualify_EtherType(unit, ruleIndex, ethertypedata, ethertypemask);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_qualify_EtherType \n");
			}
			nam_syslog_dbg("ethertypedata 0x%04x\n", ethertypedata);
			nam_syslog_dbg("ethertypemask 0x%04x\n", ethertypemask);
		}
	}
	/*if (0 != ipprotocoldata) {*/
	else {
		ret = bcm_field_qualify_IpProtocol(unit, ruleIndex, ipprotocoldata, ipprotocolmask);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_qualify_IpProtocol \n");
		}
		nam_syslog_dbg("ipprotocoldata 0x%02x\n", ipprotocoldata);
		nam_syslog_dbg("ipprotocolmask 0x%02x\n", ipprotocolmask);
	}

	ret = bcm_field_qualify_SrcMac(unit, ruleIndex, srcmacdata, srcmacmask);
	if (ACL_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR !bcm_field_qualify_SrcMac \n");
	}
	nam_syslog_dbg("srcmacdata 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x\n", 
		srcmacdata[0],srcmacdata[1],srcmacdata[2],srcmacdata[3],srcmacdata[4],srcmacdata[5]);
	nam_syslog_dbg("srcmacmask 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x\n", 
		srcmacmask[0],srcmacmask[1],srcmacmask[2],srcmacmask[3],srcmacmask[4],srcmacmask[5]);
		
	ret = bcm_field_qualify_DstMac(unit, ruleIndex, dstmacdata, dstmacmask);
	if (ACL_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR !bcm_field_qualify_DstMac \n");
	}	
	nam_syslog_dbg("dstmacdata 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x\n", 
		dstmacdata[0],dstmacdata[1],dstmacdata[2],dstmacdata[3],dstmacdata[4],dstmacdata[5]);
	nam_syslog_dbg("dstmacmask 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x\n", 
		dstmacmask[0],dstmacmask[1],dstmacmask[2],dstmacmask[3],dstmacmask[4],dstmacmask[5]);
	
	if (0 != srcipdata) {
		ret = bcm_field_qualify_SrcIp(unit, ruleIndex, srcipdata, srcipmask);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_qualify_SrcIp \n");
		}	
		nam_syslog_dbg("srcipdata 0x%08x\n", srcipdata);
		nam_syslog_dbg("srcipmask 0x%08x\n", srcipmask);
	}
	if (0 != dstipdata) {
		ret = bcm_field_qualify_DstIp(unit, ruleIndex, dstipdata, dstipmask);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_qualify_DstIp \n");
		}			
		nam_syslog_dbg("dstipdata 0x%08x\n", dstipdata);
		nam_syslog_dbg("dstipmask 0x%08x\n", dstipmask);
	}
	
	if( (0 != l4srcportdata) && (0 != l4srcportmask)) {
		ret = bcm_field_qualify_L4SrcPort(unit, ruleIndex, l4srcportdata, l4srcportmask);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_qualify_L4SrcPort \n");
		}		
		nam_syslog_dbg("l4srcportdata 0x%04x\n", l4srcportdata);
		nam_syslog_dbg("l4srcportmask 0x%04x\n", l4srcportmask);
	}
	if ((0 != l4dstportdata) && (0 != l4dstportmask)) {
		ret = bcm_field_qualify_L4DstPort(unit, ruleIndex, l4dstportdata, l4dstportmask);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_qualify_L4DstPort \n");
		}		
		nam_syslog_dbg("l4dstportdata 0x%04x\n", l4dstportdata);
		nam_syslog_dbg("l4dstportmask 0x%04x\n", l4dstportmask);
	}

	if (0 != inportmask) {
		ret = bcm_field_qualify_InPort(unit, ruleIndex, inportdata, inportmask);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_qualify_InPort \n");
		}		
		nam_syslog_dbg("inportdata 0x%04x\n", inportdata);
		nam_syslog_dbg("inportmask 0x%04x\n", inportmask);
	}

	if (0 != outervlanmask) {
		ret = bcm_field_qualify_OuterVlan(unit, ruleIndex, outervlandata, outervlanmask);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_qualify_OuterVlan \n");
		}	
		nam_syslog_dbg("outervlandata 0x%04x\n", outervlandata);
		nam_syslog_dbg("outervlanmask 0x%04x\n", outervlanmask);
	}
	
	if (ACL_ACTION_TYPE_PERMIT == pkd) {
		if (policerEnable) {
			nam_syslog_dbg("action add policer \n");
			if (OUT_PROFILE_DROP == g_policer[meternum]->cmd) {
				nam_syslog_dbg("action add policer drop \n");
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRpDrop, 0, 0);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add \n");
				}
				nam_syslog_dbg("action add red packet drop\n");
			}
			else if (OUT_PROFILE_REMAP_ENTRY == g_policer[meternum]->cmd) {				
				nam_syslog_dbg("action add policer remap \n");
				profileIndex = g_policer[meternum]->qosProfile;
				nam_syslog_dbg("remaped profileindex %d \n", profileIndex);
				newdscp = g_qos_profile[profileIndex]->dscp;
				newpri = g_qos_profile[profileIndex]->userPriority;
				cosqueue = g_qos_profile[profileIndex]->trafficClass;
				nam_syslog_dbg("action add policer remap newdscp %d, newpri %d, cosqueue %d \n",
					newdscp, newpri, cosqueue);
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRpPrioPktNew, newpri, 0);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add \n");
				}				
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRpDscpNew, newdscp, 0);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add \n");
				}				
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRpCosQNew, cosqueue, 0);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add \n");
				}				
				nam_syslog_dbg("action add new pri dscp cosq\n");
			}
			ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionMeterConfig, 1, 0);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_action_add \n");
			}
			nam_syslog_dbg("action add meter config \n");
			ret = bcm_field_meter_set(unit, ruleIndex, BCM_FIELD_METER_PEAK, rate, burst);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_action_add \n");
			}			
			nam_syslog_dbg("meter cir %u, cbs %u \n", rate, burst);
			if (g_policer[meternum]->counterEnable) {
				/*ret = bcm_field_counter_create(unit, ruleIndex);
			//	if (ACL_RETURN_CODE_SUCCESS != ret) {
			//		nam_syslog_dbg("ERROR !bcm_field_counter_create \n");
			//	}*/
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionUpdateCounter, BCM_FIELD_COUNTER_MODE_GREEN_RED, 0);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add \n");
				}	
				nam_syslog_dbg("bcm_field_action_add bcmFieldActionUpdateCounter  mode  BCM_FIELD_COUNTER_MODE_GREEN_RED\n");
				counterindex = g_policer[meternum]->counterSetIndex;
				/*   counter_num - (IN) Choice of upper (counter_num=1) or lower
                         //  (counter_num=0) of the counter pair.                 
				//valptmp[0] = g_counter[counterindex]->inProfileBytesCnt;
				//valptmp[1] = g_counter[counterindex]->outOfProfileBytesCnt;*/
				nam_syslog_dbg("inProfileBytesCnt is %u, outOfProfileBytesCnt is %u \n", 
					g_counter[counterindex]->inProfileBytesCnt, g_counter[counterindex]->outOfProfileBytesCnt);
				/*memcpy(&valp, valptmp, sizeof(long long));*/
				/*valp = 100;*/
				ret = bcm_field_counter_set(unit, ruleIndex, 0, g_counter[counterindex]->outOfProfileBytesCnt);/*g_counter[counterindex]->outOfProfileBytesCnt;*/
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_counter_set \n");
				}
				ret = bcm_field_counter_set(unit, ruleIndex, 1, g_counter[counterindex]->inProfileBytesCnt);/*g_counter[counterindex]->inProfileBytesCnt;*/
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_counter_set \n");
				}
				/*nam_syslog_dbg("action add counter %d, valp is %llu\n", counterindex, valp);*/
			}
		}
		if (32 == node->appendInfo.appendType) {
			profileIndex = node->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
			newdscp = g_qos_profile[profileIndex]->dscp;
			newpri = g_qos_profile[profileIndex]->userPriority;
			cosqueue = g_qos_profile[profileIndex]->trafficClass;
			/*ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionPrioPktNew, newpri, 0);
			//if (ACL_RETURN_CODE_SUCCESS != ret) {
			//	nam_syslog_dbg("ERROR !bcm_field_action_add bcmFieldActionPrioPktNew\n");
			//}*/
			ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionDscpNew, newdscp, 0);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_action_add bcmFieldActionDscpNew\n");
			}			
			ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionCosQNew, cosqueue, 0);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_action_add bcmFieldActionCosQNew\n");
			}			
			nam_syslog_dbg("append action add new pri dscp cosq\n");
		}
	}
	else if (ACL_ACTION_TYPE_DENY == pkd) {			
		ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionDrop, 0, 0);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_action_add \n");
		}		
		nam_syslog_dbg("action add drop \n");
	}	
	else if (ACL_ACTION_TYPE_REDIRECT == pkd) {			
		redirectportnum = node->ActionPtr.actionInfo.redirect.portNum;
		modid = node->ActionPtr.actionInfo.redirect.modid;
		ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRedirectPort, modid, redirectportnum);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_action_add \n");
		}	
		nam_syslog_dbg("action add redirectpbmp \n");
	}
	else if (ACL_ACTION_TYPE_TRAP_TO_CPU == pkd) {
		ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionCopyToCpu, 0, 0);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_action_add \n");
		}		
		nam_syslog_dbg("action add trap to cpu \n");
	}
	
	ret = bcm_field_entry_install(unit, ruleIndex);
	if (ACL_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR !bcm_field_entry_install \n");
	}
	nam_syslog_dbg("install acl rule  %d \n", ruleIndex);

	return ret;
}


/*RULE_INGRESS STDIPV4L4_DATA_DEAL*/
unsigned int nam_drv_acl_extIpv4L4_ListDeal
(
	struct ACCESS_LIST_STC *node,
	unsigned int aclId, 
	int unit,
	unsigned int dir_info
)
{
	unsigned long 						ruleIndex = 0, pkd = ACL_ACTION_TYPE_PERMIT, profileIndex = 0;	
	unsigned int		 				ipty = 0, counterindex = 0,ret = ACL_RETURN_CODE_SUCCESS;
			
    	unsigned char			  redirectPort=0;
	bcm_port_t inportdata  = 0, inportmask = 0;
	uint16 ethertypedata = 0, ethertypemask = 0; 
	bcm_vlan_t outervlandata = 0, outervlanmask = 0;
	uint8	ipprotocoldata = 0, ipprotocolmask = 0;
	bcm_l4_port_t l4srcportdata = 0, l4srcportmask = 0;
	bcm_l4_port_t l4dstportdata = 0, l4dstportmask = 0;
	bcm_ip_t srcipdata = 0, srcipmask = 0;
	bcm_ip_t dstipdata = 0, dstipmask = 0;
	int policerEnable = 0;
	int redirectportnum = 0, modid = 0;
	int meternum = 0, rate = 0, burst = 0;
	uint8 newdscp = 0;
	bcm_vlan_t newpri = 0;
	int cosqueue = 0;
	unsigned long valptmp[2] = {0};
	long long valp = 0;
	
	ipty = node->TcamPtr.packetType;
	ruleIndex = node->ruleIndex;
	pkd=node->ActionPtr.actionType;

	ret = bcm_field_entry_create_id(unit, aclId, ruleIndex);
	if (ACL_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR !bcm_field_entry_create_id \n");
	}

	switch(ipty) {
			case ACL_TCAM_RULE_PACKETTYPE_IP:		ipprotocoldata = 0;break;
			case ACL_TCAM_RULE_PACKETTYPE_UDP:		ipprotocoldata = 0x11;break;
			case ACL_TCAM_RULE_PACKETTYPE_TCP:		ipprotocoldata = 0x06;break;
			case ACL_TCAM_RULE_PACKETTYPE_ICMP: 	ipprotocoldata = 0x01;break;
			case ACL_TCAM_RULE_PACKETTYPE_ETHERNET: ipprotocoldata = 0;break;
			default:
					  break;
	}

	ethertypemask = 0xffff;
	srcipmask = node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSip;
	dstipmask = node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDip;
	l4dstportmask = node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDstPort;
	if (ACL_TCAM_RULE_PACKETTYPE_ICMP == ipty) {
		l4srcportmask = (node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskType)<<8;
		l4srcportmask |= node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskCode;
	}
	else if ((ACL_TCAM_RULE_PACKETTYPE_TCP == ipty)||(ACL_TCAM_RULE_PACKETTYPE_UDP == ipty)) {
		l4srcportmask = node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSrcPort;
	}
	
	if ((ACL_TCAM_RULE_PACKETTYPE_ICMP == ipty)||
		(ACL_TCAM_RULE_PACKETTYPE_TCP == ipty)||
		(ACL_TCAM_RULE_PACKETTYPE_UDP == ipty)) {
		ipprotocolmask = 0xff;
	}
	else {
		ipprotocolmask = 0;
	}

	ethertypedata = 0x0800;
	srcipdata = node->TcamPtr.ruleInfo.stdIpv4L4.rule.Sip;
	dstipdata = node->TcamPtr.ruleInfo.stdIpv4L4.rule.Dip;
	l4dstportdata = node->TcamPtr.ruleInfo.stdIpv4L4.rule.DstPort;
	if (ACL_TCAM_RULE_PACKETTYPE_ICMP == ipty) {
		l4srcportdata = (node->TcamPtr.ruleInfo.stdIpv4L4.rule.Type)<<8;
		l4srcportdata |= node->TcamPtr.ruleInfo.stdIpv4L4.rule.Code;		
	}
	else if ((ACL_TCAM_RULE_PACKETTYPE_TCP == ipty)||(ACL_TCAM_RULE_PACKETTYPE_UDP == ipty)) {
		l4srcportdata = node->TcamPtr.ruleInfo.stdIpv4L4.rule.SrcPort;
	}
	
	policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;
	if (policerEnable) {
		ret = bcm_field_meter_create(unit, ruleIndex);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_meter_create \n");
		}
		meternum  = node->ActionPtr.policerPtr.policerId;
		rate = g_policer[meternum]->cir;
		burst = g_policer[meternum]->cbs;
		
		if (g_policer[meternum]->counterEnable) {
			ret = bcm_field_counter_create(unit, ruleIndex);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_counter_create \n");
			}
			nam_syslog_dbg("bcm_field_counter_creat for rule %d\n", ruleIndex);
		}
	}

	
	if (0 == ipprotocoldata) {
		ret = bcm_field_qualify_EtherType(unit, ruleIndex, ethertypedata, ethertypemask);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_qualify_EtherType \n");
		}
		nam_syslog_dbg("ethertypedata 0x%04x\n", ethertypedata);
		nam_syslog_dbg("ethertypemask 0x%04x\n", ethertypemask);
	}
	/*if (0 != ipprotocoldata) {*/
	else {
		ret = bcm_field_qualify_IpProtocol(unit, ruleIndex, ipprotocoldata, ipprotocolmask);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_qualify_IpProtocol \n");
		}
		nam_syslog_dbg("ipprotocoldata 0x%02x\n", ipprotocoldata);
		nam_syslog_dbg("ipprotocolmask 0x%02x\n", ipprotocolmask);
	}
	
	if (0 != srcipdata) {
		ret = bcm_field_qualify_SrcIp(unit, ruleIndex, srcipdata, srcipmask);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_qualify_SrcIp \n");
		}
		nam_syslog_dbg("srcipdata 0x%08x\n", srcipdata);
		nam_syslog_dbg("srcipmask 0x%08x\n", srcipmask);
	}
	if (0 != dstipdata) {
		ret = bcm_field_qualify_DstIp(unit, ruleIndex, dstipdata, dstipmask);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_qualify_DstIp \n");
		}
		nam_syslog_dbg("dstipdata 0x%08x\n", dstipdata);
		nam_syslog_dbg("dstipmask 0x%08x\n", dstipmask);
	}
	
	if( (0 != l4srcportdata) && (0 != l4srcportmask)) {
		ret = bcm_field_qualify_L4SrcPort(unit, ruleIndex, l4srcportdata, l4srcportmask);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_qualify_L4SrcPort \n");
		}
		nam_syslog_dbg("l4srcportdata 0x%04x\n", l4srcportdata);
		nam_syslog_dbg("l4srcportmask 0x%04x\n", l4srcportmask);
	}
	if ((0 != l4dstportdata) && (0 != l4dstportmask)) {
		ret = bcm_field_qualify_L4DstPort(unit, ruleIndex, l4dstportdata, l4dstportmask);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_qualify_L4DstPort \n");
		}
		nam_syslog_dbg("l4dstportdata 0x%04x\n", l4dstportdata);
		nam_syslog_dbg("l4dstportmask 0x%04x\n", l4dstportmask);
	}

	if (0 != inportmask) {
		ret = bcm_field_qualify_InPort(unit, ruleIndex, inportdata, inportmask);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_qualify_InPort \n");
		}
		nam_syslog_dbg("inportdata 0x%04x\n", inportdata);
		nam_syslog_dbg("inportmask 0x%04x\n", inportmask);
	}

	if (0 != outervlanmask) {
		ret = bcm_field_qualify_OuterVlan(unit, ruleIndex, outervlandata, outervlanmask);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_qualify_OuterVlan \n");
		}
		nam_syslog_dbg("outervlandata 0x%04x\n", outervlandata);
		nam_syslog_dbg("outervlanmask 0x%04x\n", outervlanmask);
	}
	
	if (ACL_ACTION_TYPE_PERMIT == pkd) {
		if (policerEnable) {
			if (OUT_PROFILE_DROP == g_policer[meternum]->cmd) {
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRpDrop, 0, 0);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add \n");
				}
				nam_syslog_dbg("action add red packet drop\n");
			}
			else if (OUT_PROFILE_REMAP_ENTRY == g_policer[meternum]->cmd) {							
				profileIndex = g_policer[meternum]->qosProfile;
				newdscp = g_qos_profile[profileIndex]->dscp;
				newpri = g_qos_profile[profileIndex]->userPriority;
				cosqueue = g_qos_profile[profileIndex]->trafficClass;
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRpPrioPktNew, newpri, 0);	
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add \n");
				}				
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRpDscpNew, newdscp, 0);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add \n");
				}				
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRpCosQNew, cosqueue, 0);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add \n");
				}				
				nam_syslog_dbg("action add new pri dscp cosq\n");
			}
			ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionMeterConfig, BCM_FIELD_METER_MODE_FLOW, 0);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_action_add \n");
			}			
			nam_syslog_dbg("action add meter config \n");
			ret = bcm_field_meter_set(unit, ruleIndex, BCM_FIELD_METER_PEAK, rate, burst);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_meter_set \n");
			}			
			nam_syslog_dbg("meter cir %u, cbs %u \n", rate, burst);
			if (g_policer[meternum]->counterEnable) {
				/*ret = bcm_field_counter_create(unit, ruleIndex);
			//	if (ACL_RETURN_CODE_SUCCESS != ret) {
			//		nam_syslog_dbg("ERROR !bcm_field_counter_create \n");
			//	}*/
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionUpdateCounter, BCM_FIELD_COUNTER_MODE_GREEN_RED, 0);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add \n");
				}	
				nam_syslog_dbg("bcm_field_action_add bcmFieldActionUpdateCounter  mode  BCM_FIELD_COUNTER_MODE_GREEN_RED\n");
				counterindex = g_policer[meternum]->counterSetIndex;
				/*   counter_num - (IN) Choice of upper (counter_num=1) or lower
                         //  (counter_num=0) of the counter pair.                 
				//valptmp[0] = g_counter[counterindex]->inProfileBytesCnt;
				//valptmp[1] = g_counter[counterindex]->outOfProfileBytesCnt;*/
				nam_syslog_dbg("inProfileBytesCnt is %u, outOfProfileBytesCnt is %u \n", 
					g_counter[counterindex]->inProfileBytesCnt, g_counter[counterindex]->outOfProfileBytesCnt);
				/*memcpy(&valp, valptmp, sizeof(long long));*/
				/*valp = 100;*/
				ret = bcm_field_counter_set(unit, ruleIndex, 0, g_counter[counterindex]->outOfProfileBytesCnt);/*g_counter[counterindex]->outOfProfileBytesCnt;*/
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_counter_set \n");
				}
				ret = bcm_field_counter_set(unit, ruleIndex, 1, g_counter[counterindex]->inProfileBytesCnt);/*g_counter[counterindex]->inProfileBytesCnt;*/
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_counter_set \n");
				}
				/*nam_syslog_dbg("action add counter %d, valp is %llu\n", counterindex, valp);*/
			}
		}
		if (32 == node->appendInfo.appendType) {
			profileIndex = node->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
			newdscp = g_qos_profile[profileIndex]->dscp;
			newpri = g_qos_profile[profileIndex]->userPriority;
			cosqueue = g_qos_profile[profileIndex]->trafficClass;
			ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionDscpNew, newdscp, 0);
			nam_syslog_dbg("bcm_field_action_add  newdscp %d\n", newdscp);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_action_add bcmFieldActionDscpNew ret %d\n", ret);
			}
			/*ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionPrioPktNew, newpri, 0);
			//nam_syslog_dbg("bcm_field_action_add  newpri %d\n", newpri);
			//if (ACL_RETURN_CODE_SUCCESS != ret) {
			//	nam_syslog_dbg("ERROR !bcm_field_action_add bcmFieldActionPrioPktNew ret %d\n", ret);
			//}	*/						
			ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionCosQNew, cosqueue, 0);
			nam_syslog_dbg("bcm_field_action_add  cosqueue %d\n", cosqueue);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_action_add bcmFieldActionCosQNew ret %d\n", ret);
			}	
			nam_syslog_dbg("append action add new pri dscp cosq\n");
		}
	}
	else if (ACL_ACTION_TYPE_DENY == pkd) {			
		ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionDrop, 0, 0);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_action_add \n");
		}			
		nam_syslog_dbg("action add drop \n");
	}	
	else if (ACL_ACTION_TYPE_REDIRECT == pkd) {			
		redirectportnum = node->ActionPtr.actionInfo.redirect.portNum;
		modid = node->ActionPtr.actionInfo.redirect.modid;
		ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRedirectPort, modid, redirectportnum);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_action_add \n");
		}					
		nam_syslog_dbg("action add redirectpbmp \n");
	}
	else if (ACL_ACTION_TYPE_TRAP_TO_CPU == pkd) {
		ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionCopyToCpu, 0, 0);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_action_add \n");
		}					
		nam_syslog_dbg("action add trap to cpu \n");
	}
	
	ret = bcm_field_entry_install(unit, ruleIndex);	
	if (ACL_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR !bcm_field_entry_install \n");
	}			
	nam_syslog_dbg("install acl rule  %d \n", ruleIndex);
	
	return ret;
}

unsigned int nam_drv_acl_stdIpv4L4_ListDeal
(
	struct ACCESS_LIST_STC *node,
	unsigned int aclId, 
	int unit,
	unsigned int dir_info
)
{
	unsigned long 						ruleIndex = 0, pkd = ACL_ACTION_TYPE_PERMIT, profileIndex = 0;	
	unsigned int		 				ipty = 0, counterindex = 0,ret = ACL_RETURN_CODE_SUCCESS;
			
    	unsigned char			  redirectPort=0;
	uint16 ethertypedata = 0, ethertypemask = 0; 
	uint8	ipprotocoldata = 0, ipprotocolmask = 0;
	bcm_l4_port_t l4srcportdata = 0, l4srcportmask = 0;
	bcm_l4_port_t l4dstportdata = 0, l4dstportmask = 0;
	bcm_ip_t srcipdata = 0, srcipmask = 0;
	bcm_ip_t dstipdata = 0, dstipmask = 0;
	int policerEnable = 0;
	int redirectportnum = 0, modid = 0;
	int meternum = 0, rate = 0, burst = 0;
	uint8 newdscp = 0;
	bcm_vlan_t newpri = 0;
	int cosqueue = 0;

	ipty = node->TcamPtr.packetType;

	ruleIndex = node->ruleIndex + 2;

	ret = bcm_field_entry_create_id(unit, aclId, ruleIndex);
	if (ACL_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR !bcm_field_entry_create_id ret %d , unit %d, aclId %d , ruleIndex %d  \n",
			ret, unit, aclId, ruleIndex);
	}

	switch(ipty) {
			case ACL_TCAM_RULE_PACKETTYPE_IP:		ipprotocoldata = 0;break;
			case ACL_TCAM_RULE_PACKETTYPE_UDP:		ipprotocoldata = 0x11;break;
			case ACL_TCAM_RULE_PACKETTYPE_TCP:		ipprotocoldata = 0x06;break;
			case ACL_TCAM_RULE_PACKETTYPE_ICMP: 	ipprotocoldata = 0x01;break;
			/*case ACL_TCAM_RULE_PACKETTYPE_ETHERNET: ipprotocoldata = 0;break;*/
			default:
					  break;
	}

	pkd=node->ActionPtr.actionType;
	
	srcipmask = node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSip;
	dstipmask = node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDip;
	l4dstportmask = node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDstPort;
	ethertypemask = 0xffff;
	if (ACL_TCAM_RULE_PACKETTYPE_ICMP == ipty) {
		l4srcportmask = (node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskType)<<8;
		l4srcportmask |= node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskCode;
	}
	else if ((ACL_TCAM_RULE_PACKETTYPE_TCP == ipty)||(ACL_TCAM_RULE_PACKETTYPE_UDP == ipty)) {
		l4srcportmask = node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskSrcPort;
		l4dstportmask = node->TcamPtr.ruleInfo.stdIpv4L4.mask.maskDstPort;
	}
	
	if ((ACL_TCAM_RULE_PACKETTYPE_ICMP == ipty)||
		(ACL_TCAM_RULE_PACKETTYPE_TCP == ipty)||
		(ACL_TCAM_RULE_PACKETTYPE_UDP == ipty)) {
		ipprotocolmask = 0xff;
	}
	else {
		ipprotocolmask = 0;
	}
		
	srcipdata = node->TcamPtr.ruleInfo.stdIpv4L4.rule.Sip;
	dstipdata = node->TcamPtr.ruleInfo.stdIpv4L4.rule.Dip;
	l4dstportdata = node->TcamPtr.ruleInfo.stdIpv4L4.rule.DstPort;
	ethertypedata = 0x0800;
	if (ACL_TCAM_RULE_PACKETTYPE_ICMP == ipty) {
		l4srcportdata = (node->TcamPtr.ruleInfo.stdIpv4L4.rule.Type)<<8;
		l4srcportdata |= node->TcamPtr.ruleInfo.stdIpv4L4.rule.Code;	
	}
	else if ((ACL_TCAM_RULE_PACKETTYPE_TCP == ipty)||(ACL_TCAM_RULE_PACKETTYPE_UDP == ipty)) {
		l4srcportdata = node->TcamPtr.ruleInfo.stdIpv4L4.rule.SrcPort;
		l4dstportdata = node->TcamPtr.ruleInfo.stdIpv4L4.rule.DstPort;
	}
	
	policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;
	if (policerEnable) {
		ret = bcm_field_meter_create(unit, ruleIndex);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_meter_create \n");
		}	
		meternum  = node->ActionPtr.policerPtr.policerId;
		rate = g_policer[meternum]->cir;
		burst = g_policer[meternum]->cbs;
	}

	if (0 == ipprotocoldata) {
		ret = bcm_field_qualify_EtherType(unit, ruleIndex, ethertypedata, ethertypemask);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_qualify_EtherType \n");
		}			
		nam_syslog_dbg("ethertypedata 0x%04x\n", ethertypedata);
		nam_syslog_dbg("ethertypemask 0x%04x\n", ethertypemask);
	}
	/*if (0 != ipprotocoldata) {*/
	else {
		ret = bcm_field_qualify_IpProtocol(unit, ruleIndex, ipprotocoldata, ipprotocolmask);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_qualify_IpProtocol \n");
		}
		nam_syslog_dbg("ipprotocoldata 0x%02x\n", ipprotocoldata);
		nam_syslog_dbg("ipprotocolmask 0x%02x\n", ipprotocolmask);
	}
	
	if (0 != srcipdata) {
		ret = bcm_field_qualify_SrcIp(unit, ruleIndex, srcipdata, srcipmask);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_qualify_SrcIp \n");
		}		
		nam_syslog_dbg("srcipdata 0x%08x\n", srcipdata);
		nam_syslog_dbg("srcipmask 0x%08x\n", srcipmask);
	}
	if (0 != dstipdata) {
		ret = bcm_field_qualify_DstIp(unit, ruleIndex, dstipdata, dstipmask);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_qualify_DstIp \n");
		}
		nam_syslog_dbg("dstipdata 0x%08x\n", dstipdata);
		nam_syslog_dbg("dstipmask 0x%08x\n", dstipmask);
	}
	
	if( (0 != l4srcportdata) && (0 != l4srcportmask)) {
		ret = bcm_field_qualify_L4SrcPort(unit, ruleIndex, l4srcportdata, l4srcportmask);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_qualify_L4SrcPort \n");
		}		
		nam_syslog_dbg("l4srcportdata 0x%04x\n", l4srcportdata);
		nam_syslog_dbg("l4srcportmask 0x%04x\n", l4srcportmask);
	}
	if ((0 != l4dstportdata) && (0 != l4dstportmask)) {
		ret = bcm_field_qualify_L4DstPort(unit, ruleIndex, l4dstportdata, l4dstportmask);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_qualify_L4DstPort \n");
		}		
		nam_syslog_dbg("l4dstportdata 0x%04x\n", l4dstportdata);
		nam_syslog_dbg("l4dstportmask 0x%04x\n", l4dstportmask);
	}
	
	if (ACL_ACTION_TYPE_PERMIT == pkd) {
		if (policerEnable) {
			if (OUT_PROFILE_DROP == g_policer[meternum]->cmd) {
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRpDrop, 0, 0);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add \n");
				}	
				nam_syslog_dbg("action add red packet drop\n");
			}
			else if (OUT_PROFILE_REMAP_ENTRY == g_policer[meternum]->cmd) {							
				profileIndex = g_policer[meternum]->qosProfile;
				newdscp = g_qos_profile[profileIndex]->dscp;
				newpri = g_qos_profile[profileIndex]->userPriority;
				cosqueue = g_qos_profile[profileIndex]->trafficClass;
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRpCosQNew, cosqueue, 0);
				nam_syslog_dbg("bcm_field_action_add  cosqueue %d\n", cosqueue);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add bcmFieldActionRpCosQNew ret %d\n", ret);
				}
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRpDscpNew, newdscp, 0);
				nam_syslog_dbg("bcm_field_action_add  newdscp %d\n", newdscp);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add  bcmFieldActionRpDscpNew ret %d\n", ret);
				}	
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRpPrioPktNew, newpri, 0);
				nam_syslog_dbg("bcm_field_action_add  newpri %d\n", newpri);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add  bcmFieldActionRpPrioPktNew ret %d\n", ret);
				}
				nam_syslog_dbg("action add new pri dscp cosq\n");
			}
			ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionMeterConfig, 1, 0);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_action_add \n");
			}		
			nam_syslog_dbg("action add meter config \n");
			ret = bcm_field_meter_set(unit, ruleIndex, BCM_FIELD_METER_PEAK, rate, burst);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_meter_set \n");
			}				
			nam_syslog_dbg("meter cir %u, cbs %u \n", rate, burst);
			if (g_policer[meternum]->counterEnable) {
				/*ret = bcm_field_counter_create(unit, ruleIndex);
			//	if (ACL_RETURN_CODE_SUCCESS != ret) {
			//		nam_syslog_dbg("ERROR !bcm_field_counter_create \n");
			//	}*/
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionUpdateCounter, BCM_FIELD_COUNTER_MODE_GREEN_RED, 0);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add \n");
				}	
				nam_syslog_dbg("bcm_field_action_add bcmFieldActionUpdateCounter  mode  BCM_FIELD_COUNTER_MODE_GREEN_RED\n");
				counterindex = g_policer[meternum]->counterSetIndex;
				/*   counter_num - (IN) Choice of upper (counter_num=1) or lower
                         //  (counter_num=0) of the counter pair.                 
				//valptmp[0] = g_counter[counterindex]->inProfileBytesCnt;
				//valptmp[1] = g_counter[counterindex]->outOfProfileBytesCnt;*/
				nam_syslog_dbg("inProfileBytesCnt is %u, outOfProfileBytesCnt is %u \n", 
					g_counter[counterindex]->inProfileBytesCnt, g_counter[counterindex]->outOfProfileBytesCnt);
				/*memcpy(&valp, valptmp, sizeof(long long));*/
				/*valp = 100;*/
				ret = bcm_field_counter_set(unit, ruleIndex, 0, g_counter[counterindex]->outOfProfileBytesCnt);/*g_counter[counterindex]->outOfProfileBytesCnt;*/
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_counter_set \n");
				}
				ret = bcm_field_counter_set(unit, ruleIndex, 1, g_counter[counterindex]->inProfileBytesCnt);/*g_counter[counterindex]->inProfileBytesCnt;*/
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_counter_set \n");
				}
				/*nam_syslog_dbg("action add counter %d, valp is %llu\n", counterindex, valp);*/
			}
		}
		if (32 == node->appendInfo.appendType) {
			profileIndex = node->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
			newdscp = g_qos_profile[profileIndex]->dscp;
			newpri = g_qos_profile[profileIndex]->userPriority;
			cosqueue = g_qos_profile[profileIndex]->trafficClass;
			ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionDscpNew, newdscp, 0);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_action_add  bcmFieldActionDscpNew ret %d\n", ret);
			}
		/*	ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionPrioPktNew, newpri, 0);
		//	if (ACL_RETURN_CODE_SUCCESS != ret) {
		//		nam_syslog_dbg("ERROR !bcm_field_action_add  bcmFieldActionPrioPktNew ret %d\n", ret);
		//	}	*/				
			ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionCosQNew, cosqueue, 0);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_action_add  bcmFieldActionCosQNew ret %d\n", ret);
			}	
			nam_syslog_dbg("append action add new pri dscp cosq\n");
		}
	}
	else if (ACL_ACTION_TYPE_DENY == pkd) {			
		ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionDrop, 0, 0);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_action_add \n");
		}		
		nam_syslog_dbg("action add drop \n");
	}	
	else if (ACL_ACTION_TYPE_REDIRECT == pkd) {			
		redirectportnum = node->ActionPtr.actionInfo.redirect.portNum;
		modid = node->ActionPtr.actionInfo.redirect.modid;
		ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRedirectPort, modid, redirectportnum);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_action_add \n");
		}			
		nam_syslog_dbg("action add redirectpbmp \n");
	}
	else if (ACL_ACTION_TYPE_TRAP_TO_CPU == pkd) {
		ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionCopyToCpu, 0, 0);
		if (ACL_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR !bcm_field_action_add \n");
		}	
		nam_syslog_dbg("action add trap to cpu \n");
	}
	
	ret = bcm_field_entry_install(unit, ruleIndex);
	if (ACL_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR !bcm_field_entry_install \n");
	}	
	nam_syslog_dbg("install acl rule  %d \n", ruleIndex);
	
	return ret;
}
#endif

/**********************************************************************************
 * nam_acl_policer_action_update
 *
 * init policer
 *
 *	INPUT:
 *		unsigned int ruleIndex	- acl index
 *		int 		 unit		- dev number
 *
 *	OUTPUT:
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_SUCCESS
 *	NOTE:
 *
 **********************************************************************************/
#ifdef DRV_LIB_BCM
unsigned int nam_acl_policer_action_update
(
	unsigned int ruleIndex, 
	int unit
)
{
	unsigned int						counterindex = 0;
	unsigned int						ret = ACL_RETURN_CODE_SUCCESS;
	AclRuleList               *node=NULL;
	int policerEnable = 0;
	int redirectportnum = 0;
	int meternum = 0, rate = 0, burst = 0;
	unsigned int		pkd = ACL_ACTION_TYPE_PERMIT;
	uint8 newdscp = 0;
	bcm_vlan_t newpri = 0;
	int cosqueue = 0, profileIndex = 0;
	
	nam_syslog_dbg("nam_acl_policer_action_update \n");

	node = g_acl_rule[ruleIndex];
	
	policerEnable= (char)node->ActionPtr.policerPtr.policerEnable ;
	if (policerEnable) {
		meternum  = node->ActionPtr.policerPtr.policerId;
		rate = g_policer[meternum]->cir;
		burst = g_policer[meternum]->cbs;
		nam_syslog_dbg("meter cir %u, cbs %u \n", rate, burst);
	}

	ret = bcm_field_action_remove(unit, ruleIndex, bcmFieldActionRpDrop);
	if (ACL_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR !bcm_field_action_remove \n");
	}	
	/*
	ret = bcm_field_action_remove(unit, ruleIndex, bcmFieldActionPrioPktNew);
	if (ACL_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR !bcm_field_action_remove \n");
	}	
	ret = bcm_field_action_remove(unit, ruleIndex, bcmFieldActionDscpNew);
	if (ACL_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR !bcm_field_action_remove \n");
	}	
	ret = bcm_field_action_remove(unit, ruleIndex, bcmFieldActionCosQNew);
	if (ACL_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR !bcm_field_action_remove \n");
	}	
	*/
	ret = bcm_field_action_remove(unit, ruleIndex, bcmFieldActionMeterConfig);
	if (ACL_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR !bcm_field_action_remove \n");
	}	
	ret = bcm_field_entry_reinstall(unit, ruleIndex);
	if (ACL_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR !bcm_field_entry_reinstall \n");
	}	
	
	nam_syslog_dbg("reinstall acl rule  %d \n", ruleIndex);
	
	if (ACL_ACTION_TYPE_PERMIT == pkd) {
		if (policerEnable) {
			if (OUT_PROFILE_DROP == g_policer[meternum]->cmd) {
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRpDrop, 0, 0);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add \n");
				}	
				nam_syslog_dbg("action add red packet drop\n");
			}
			else if (OUT_PROFILE_REMAP_ENTRY == g_policer[meternum]->cmd) {
				profileIndex = node->ActionPtr.actionInfo.qos.qos.ingress.profileIndex;
				newdscp = g_qos_profile[profileIndex]->dscp;
				newpri = g_qos_profile[profileIndex]->userPriority;
				cosqueue = g_qos_profile[profileIndex]->trafficClass;
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRpPrioPktNew, newpri, 0);	
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add \n");
				}				
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionRpDscpNew, newdscp, 0);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add \n");
				}				bcm_field_action_add(unit, ruleIndex, bcmFieldActionRpCosQNew, cosqueue, 0);
				nam_syslog_dbg("action add red packet drop\n");
			}
			ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionMeterConfig, 1, 0);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_action_add \n");
			}
			nam_syslog_dbg("action add meter config \n");
			ret = bcm_field_meter_set(unit, ruleIndex, BCM_FIELD_METER_PEAK, rate, burst);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_dbg("ERROR !bcm_field_meter_set \n");
			}
			nam_syslog_dbg("meter cir %u, cbs %u \n", rate, burst);
			if (g_policer[meternum]->counterEnable) {
				/*ret = bcm_field_counter_create(unit, ruleIndex);
			//	if (ACL_RETURN_CODE_SUCCESS != ret) {
			//		nam_syslog_dbg("ERROR !bcm_field_counter_create \n");
			//	}*/
				ret = bcm_field_action_add(unit, ruleIndex, bcmFieldActionUpdateCounter, BCM_FIELD_COUNTER_MODE_GREEN_RED, 0);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_action_add \n");
				}	
				nam_syslog_dbg("bcm_field_action_add bcmFieldActionUpdateCounter  mode  BCM_FIELD_COUNTER_MODE_GREEN_RED\n");
				counterindex = g_policer[meternum]->counterSetIndex;
				/*   counter_num - (IN) Choice of upper (counter_num=1) or lower
                         //  (counter_num=0) of the counter pair.                 
				//valptmp[0] = g_counter[counterindex]->inProfileBytesCnt;
				//valptmp[1] = g_counter[counterindex]->outOfProfileBytesCnt;*/
				nam_syslog_dbg("inProfileBytesCnt is %u, outOfProfileBytesCnt is %u \n", 
					g_counter[counterindex]->inProfileBytesCnt, g_counter[counterindex]->outOfProfileBytesCnt);
				/*memcpy(&valp, valptmp, sizeof(long long));*/
				/*valp = 100;*/
				ret = bcm_field_counter_set(unit, ruleIndex, 0, g_counter[counterindex]->outOfProfileBytesCnt);/*g_counter[counterindex]->outOfProfileBytesCnt;*/
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_counter_set \n");
				}
				ret = bcm_field_counter_set(unit, ruleIndex, 1, g_counter[counterindex]->inProfileBytesCnt);/*g_counter[counterindex]->inProfileBytesCnt;*/
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_dbg("ERROR !bcm_field_counter_set \n");
				}
				/*nam_syslog_dbg("action add counter %d, valp is %llu\n", counterindex, valp);*/
			}
			nam_syslog_dbg("action add counter %d config \n", counterindex);
		}
	}

	ret = bcm_field_entry_install(unit, ruleIndex);	
	if (ACL_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR !bcm_field_entry_install \n");
	}
	nam_syslog_dbg("install acl rule  %d \n", ruleIndex);
	
	return ret;
}
#endif
 
/*  
unsigned int  nam_acl_time_action_update(unsigned int ruleIndex)
{
	  unsigned int			    devIdx=0,appDemoDevAmount=0;
	  AclRuleList				*node=NULL;
	  CPSS_DXCH_PCL_ACTION_STC	actionPtr2;
	  unsigned int				GetIndex=0,ret = ACL_RETURN_CODE_SUCCESS;
	  CPSS_PCL_RULE_SIZE_ENT	ruleSize;
	
	  appDemoDevAmount = nam_asic_get_instance_num();							 
	  node = g_acl_rule[ruleIndex];
	  
	  for(devIdx=0;devIdx < appDemoDevAmount;devIdx++)
	  {		
		ruleSize=CPSS_PCL_RULE_SIZE_EXT_E;
	
		GetIndex=cpssDxChPclRuleActionGet(devIdx,ruleSize,ruleIndex,CPSS_PCL_DIRECTION_INGRESS_E,&actionPtr2);
		if(GetIndex!=0){
			nam_syslog_dbg("cpssDxChPclRuleActionGet fail! GetIndex %d\n",GetIndex);
			return 1;
		}
		if(ACL_ACTION_TYPE_PERMIT==node->ActionPtr.actionType){
			actionPtr2.pktCmd =CPSS_PACKET_CMD_DROP_HARD_E;
		}
		else if(ACL_ACTION_TYPE_DENY==node->ActionPtr.actionType){
			actionPtr2.pktCmd =CPSS_PACKET_CMD_FORWARD_E;

		}	 
		 ret= cpssDxChPclRuleActionUpdate(devIdx,ruleSize,ruleIndex,&actionPtr2);
		 if(ret!=0){
			nam_syslog_dbg("cpssDxChPclRuleActionUpdate %d\n",ret);
			return 1;
		 }			
	  }  
		
	 return ret;
}
*/

/**********************************************************************************
 * nam_acl_policer_action_update
 *
 * init policer
 *
 *	INPUT:
 *		unsigned int 		policerIndex - policer index
 *		QOS_POLICER_STC 	*policer	 - struct of QOS_POLICER_STC	
 *		unsigned long		*cir		 - commited information rate in kilobits/sec (kilo = 1000)
 *		unsigned long       *cbs		 - commited burst size in bytes
 *
 *	OUTPUT:
 *
 *	RETURN:
 *		ACL_RETURN_CODE_SUCCESS
 *	NOTE:
 *
 **********************************************************************************/
unsigned int nam_qos_policer_entry_set
(
	unsigned int 		policerIndex,
	QOS_POLICER_STC 	*policer,
	unsigned long		*cir,
	unsigned long       *cbs
)
{
	unsigned int i = 0, appDemoDevAmount = 0, devIdx = 0;
	int mirrorport = 0, ret = ACL_RETURN_CODE_SUCCESS;
	
	appDemoDevAmount = nam_asic_get_instance_num();
#ifdef DRV_LIB_BCM
	struct acl_rule_info_s		 *ruleInfo = NULL;
	struct list_head			*Ptr = NULL;
	struct acl_group_list_s	*groupPtr = NULL;
	unsigned int  add_index = 0;

	nam_syslog_dbg("nam_qos_policer_entry_set ! \n");
	for (i = 0; i < MAX_GROUP_NUM; i++) {
		groupPtr = acl_group[i];
		if (NULL != groupPtr) {
			 __list_for_each(Ptr, &(groupPtr->list1)) {
				ruleInfo = list_entry(Ptr,struct acl_rule_info_s,list);
				if (NULL != ruleInfo) { 		
					add_index = ruleInfo->ruleIndex;
							/* save 0 1 rule for udp */
					if (add_index >= (MAX_ACL_RULE_NUMBER/2)) {
						add_index += ACL_SAVE_FOR_UDP_BPDU*2;
					}
					else {
						add_index += ACL_SAVE_FOR_UDP_BPDU;
					}
					if (policerIndex == g_acl_rule[add_index]->ActionPtr.policerPtr.policerId) {
						for(devIdx=0;devIdx < appDemoDevAmount;devIdx++) {
							if (!((acl_group[i]->devnum^ACL_DEFAULT_DEVID)&(1<<devIdx))) {
								continue;
							}
							ret = nam_acl_policer_action_update(add_index, devIdx);
							if (ACL_RETURN_CODE_SUCCESS != ret) {
								nam_syslog_dbg("nam_acl_policer_action_update fail ! \n");
								return ACL_RETURN_CODE_ERROR;
							}
						}
					}
				}
			 }
		}
	}
#endif	

#ifdef DRV_LIB_CPSS
#ifdef DRV_LIB_CPSS_3_4
	CPSS_DXCH3_POLICER_METERING_ENTRY_STC		 entryPtr;
	CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT	tbParamsPtr;	
	memset(&entryPtr,0,sizeof(CPSS_DXCH3_POLICER_METERING_ENTRY_STC));
	memset(&tbParamsPtr,0,sizeof(CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT));
	entryPtr.meterColorMode = (CPSS_POLICER_COLOR_MODE_ENT)policer->meterColorMode;
	entryPtr.tokenBucketParams.srTcmParams.cir = policer->cir;
	entryPtr.tokenBucketParams.srTcmParams.cbs = policer->cbs;
	entryPtr.redPcktCmd = (CPSS_DXCH3_POLICER_NON_CONFORM_CMD_ENT)(policer->cmd);
	entryPtr.yellowPcktCmd = (CPSS_DXCH3_POLICER_NON_CONFORM_CMD_ENT)(policer->cmd);
	entryPtr.qosProfile = policer->qosProfile;
	entryPtr.modifyDscp = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(policer->modifyDscp);
	entryPtr.modifyUp = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(policer->modifyDscp);
/*
	if (policer->counterEnable) {
		entryPtr.mngCounterSet = CPSS_DXCH3_POLICER_MNG_CNTR_SET0_E;
	}
	else {
		entryPtr.mngCounterSet = CPSS_DXCH3_POLICER_MNG_CNTR_DISABLED_E;
	}
	*/
	entryPtr.countingEntryIndex = policer->counterSetIndex;
#else 
	CPSS_DXCH_POLICER_ENTRY_STC        entryPtr;
	CPSS_DXCH_POLICER_TB_PARAMS_STC    tbParamsPtr; 
	memset(&entryPtr, 0, sizeof(CPSS_DXCH_POLICER_ENTRY_STC));
	memset(&tbParamsPtr, 0, sizeof(CPSS_DXCH_POLICER_TB_PARAMS_STC));
	entryPtr.policerEnable = policer->policerEnable;
	entryPtr.meterColorMode = (CPSS_POLICER_COLOR_MODE_ENT)policer->meterColorMode;
	entryPtr.tbParams.cir = policer->cir;
	entryPtr.tbParams.cbs = policer->cbs;
	entryPtr.counterEnable = policer->counterEnable;
	entryPtr.counterSetIndex = policer->counterSetIndex;
	entryPtr.cmd = (CPSS_DXCH_POLICER_CMD_ENT)(policer->cmd);
	entryPtr.qosProfile = policer->qosProfile;
	entryPtr.modifyDscp = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(policer->modifyDscp);
	entryPtr.modifyUp = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(policer->modifyDscp);
#endif

	/* call cpss api function */
#ifdef DRV_LIB_CPSS_3_4
	ret = cpssDxCh3PolicerMeteringEntrySet(devIdx, 0, policerIndex, &entryPtr,  &tbParamsPtr);
#else
	ret = cpssDxChPolicerEntrySet(devIdx, policerIndex, &entryPtr,  &tbParamsPtr);
#endif

	if(ret != 0) {
		nam_syslog_dbg("cpssDxChPolicerEntrySet error!result %d",ret);
	}
	/*actual parameters configured in the hardware
	nam_syslog_dbg("cir %d ,cbs %d", tbParamsPtr.cir, tbParamsPtr.cbs);
	*cir=tbParamsPtr.cir;
	*cbs=tbParamsPtr.cbs; */
#endif
	return ret;
}

/**********************************************************************************
 * nam_qos_policer_invalid
 *
 * invalid a policer
 *
 *	INPUT:
 *		unsigned int 		policerIndex - policer index
 *
 *	OUTPUT:
 *
 *	RETURN:
 *		ACL_RETURN_CODE_SUCCESS
 *	NOTE:
 *
 **********************************************************************************/
unsigned int nam_qos_policer_invalid
(
	unsigned int policerIndex
)
{
	unsigned int i = 0, ret = ACL_RETURN_CODE_SUCCESS;
	unsigned int devIdx = 0,appDemoDevAmount = 0;
	appDemoDevAmount = nam_asic_get_instance_num(); 
	
#ifdef DRV_LIB_BCM
	struct acl_rule_info_s		 *ruleInfo = NULL;
	struct list_head			*Ptr = NULL;
	struct acl_group_list_s	*groupPtr = NULL;
	unsigned int  add_index = 0;

	nam_syslog_dbg("nam_qos_policer_entry_set ! \n");
	for (i = 0; i < MAX_GROUP_NUM; i++) {
		groupPtr = acl_group[i]; /*because the device not support egress acl group*/
		if (NULL != groupPtr) {
			 __list_for_each(Ptr, &(groupPtr->list1)) {
				ruleInfo = list_entry(Ptr,struct acl_rule_info_s,list);
				if (NULL != ruleInfo) { 		
					add_index = ruleInfo->ruleIndex;
					if (policerIndex == g_acl_rule[add_index]->ActionPtr.policerPtr.policerId) {
						for(devIdx=0;devIdx < appDemoDevAmount;devIdx++) {
							if (!((acl_group[i]->devnum^ACL_DEFAULT_DEVID)&(1<<devIdx))) {
								continue;
							}
							ret = bcm_field_meter_destroy(devIdx, i);
							if(ret!=0) {
								nam_syslog_dbg("bcm_field_meter_destroy error!result %d",ret);	
							}
							/*add for policer disable and binded rule bug 2009/8/25*/
							g_acl_rule[add_index]->ActionPtr.policerPtr.policerId = 0;
							g_acl_rule[add_index]->ActionPtr.policerPtr.policerEnable = 0;
						}
					}
				}
			 }
		}
	}
#endif

#ifdef DRV_LIB_CPSS
	for(devIdx=0;devIdx<appDemoDevAmount;devIdx++){
#ifdef DRV_LIB_CPSS_3_4
	/*do nothing
			ret = cpssDxCh3PolicerMeteringEnableSet(devIdx, CPSS_DXCH_POLICER_STAGE_INGRESS_0_E, 0);
			*/
			
		CPSS_DXCH3_POLICER_METERING_ENTRY_STC		 entryPtr;
		CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT	tbParamsPtr;	
		memset(&entryPtr,0,sizeof(CPSS_DXCH3_POLICER_METERING_ENTRY_STC));
		memset(&tbParamsPtr,0,sizeof(CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT));

		ret = cpssDxCh3PolicerMeteringEntrySet(devIdx, CPSS_DXCH_POLICER_STAGE_INGRESS_0_E, policerIndex, &entryPtr,  &tbParamsPtr);

#else		
		ret=cpssDxChPolicerEntryInvalidate(devIdx,policerIndex);
#endif
		if(ret!=0) {
			nam_syslog_dbg("cpssDxChPolicerEntryInvalidate error!result %d",ret);	
		}
	}
#endif

	return ret;
}

/**********************************************************************************
 * nam_qos_policer_init
 *
 * init policer
 *
 *	INPUT:
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int nam_qos_policer_init
(
)
{	
	unsigned int ret = ACL_RETURN_CODE_SUCCESS;
#ifdef DRV_LIB_BCM
			ret = ACL_RETURN_CODE_SUCCESS;
#endif

#ifdef DRV_LIB_CPSS
	unsigned int devIdx = 0,appDemoDevAmount = 0;

	appDemoDevAmount = nam_asic_get_instance_num(); 
#ifdef DRV_LIB_CPSS_3_4
	for(devIdx=0;devIdx<appDemoDevAmount;devIdx++){
		if(cpssDxChFamilyCheck(devIdx, CPSS_CH2_E)){
			ret = cpssDxChPolicerInit(devIdx);
			if(0 != ret) {
				nam_syslog_err("cpssDxChPolicerEntrySet error!result %d",ret ); 
				return 1;
			}
		}
		else{
	/*		
			ret = cpssDxCh3PolicerMeteringEnableSet(devIdx, 0, 0);
			if(0 != ret) {
				nam_syslog_dbg("cpssDxCh3PolicerMeteringEnableSet error!result %d",ret );
				return 1;
			}
			*/
			ret = cpssDxChPolicerStageMeterModeSet(devIdx, 0, CPSS_DXCH_POLICER_STAGE_METER_MODE_FLOW_E);
			if(0 != ret) {
				nam_syslog_dbg("cpssDxChPolicerStageMeterModeSet error!result %d",ret ); 
				return 1;
			}
			
			ret = cpssDxChPolicerCountingModeSet(devIdx, CPSS_DXCH_POLICER_STAGE_INGRESS_0_E, CPSS_DXCH_POLICER_COUNTING_BILLING_IPFIX_E);
			if(0 != ret) {
				nam_syslog_dbg("cpssDxCh3PolicerMeteringEnableSet error!result %d",ret ); 
				return 1;
			}
			
			ret = cpssDxCh3PolicerDropTypeSet(devIdx, 0, 1);
			if(0 != ret) {
				nam_syslog_err("cpssDxCh3PolicerDropTypeSet error!result %d",ret ); 
				return 1;
			}
			
			ret = cpssDxCh3PolicerCountingColorModeSet(devIdx, 0, CPSS_DXCH3_POLICER_COLOR_COUNT_CL_E);
			if(0 != ret) {
				nam_syslog_err("cpssDxCh3PolicerCountingColorModeSet error!result %d",ret ); 
				return 1;
			}	
			cpssDxCh3PolicerBillingCountingEnableSet(devIdx, 1);
			cpssDxCh3PolicerBillingCountersModeSet(devIdx, CPSS_DXCH3_POLICER_BILLING_CNTR_1_BYTE_E);
		}
	}
#else
	for(devIdx=0;devIdx<appDemoDevAmount;devIdx++){
		ret = cpssDxChPolicerInit(devIdx);
		if(0 != ret) {
			nam_syslog_err("cpssDxChPolicerEntrySet error!result %d",ret ); 
			return 1;
		}
	}
#endif

#endif	

	return ret;
}

/**********************************************************************************
 * nam_qos_portmeteringenableset
 *
 * enable or disable flow policer on port  
 *
 *	INPUT:
 *		unsigned char devId		- dev number
 *		unsigned char portnum	- port number 
 *		unsigned int enable		- enable or disable
 *
 *	OUTPUT:
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_SUCCESS		- enable or disable success
 *		ACL_RETURN_CODE_ERROR			- enable or disable fail
 *
 *	NOTE:
 *
 **********************************************************************************/
unsigned int nam_qos_portmeteringenableset
(
	unsigned char devId, 
	unsigned char portnum, 
	unsigned int enable
)
{	
	unsigned int ret = ACL_RETURN_CODE_SUCCESS;
#ifdef DRV_LIB_BCM
	ret = ACL_RETURN_CODE_SUCCESS;
#endif

#ifdef DRV_LIB_CPSS

#ifdef DRV_LIB_CPSS_3_4
	ret = cpssDxCh3PolicerPortMeteringEnableSet(devId, 0, portnum, enable);
	if (0 != ret) 
	{
		nam_syslog_dbg("policer port metering enable fail for xcat \n");
		return ACL_RETURN_CODE_ERROR;
	}
#else

#endif

#endif	
	/*code optimize:  Missing return statement 
	zhangcl@autelan.com 2013-1-17*/
	return ret;


}

/**********************************************************************************
 * nam_qos_global_policer_meter
 *
 * enable or disable flow policer on port  
 *
 *	INPUT:
 *		unsigned int metermode	- meter mode
 *		unsigned int loosemru	- loose or mru 
 *
 *	OUTPUT:
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_SUCCESS
 *		ACL_RETURN_CODE_ERROR
 *
 *	NOTE:
 *
 **********************************************************************************/
unsigned int nam_qos_global_policer_meter
(
	unsigned int metermode,
	unsigned int loosemru
)
{	
	unsigned int ret = ACL_RETURN_CODE_SUCCESS;
#ifdef DRV_LIB_BCM
	ret = ACL_RETURN_CODE_SUCCESS;
#endif

#ifdef DRV_LIB_CPSS
	CPSS_POLICER_TB_MODE_ENT mode;
	CPSS_POLICER_MRU_ENT     mru;
	unsigned int 			 appDemoDevAmount,devIdx;

	memset(&mode,0,sizeof(CPSS_POLICER_TB_MODE_ENT));
	memset(&mru,0,sizeof(CPSS_POLICER_MRU_ENT));
	
	mode = (CPSS_POLICER_TB_MODE_ENT)metermode;
	mru= (CPSS_POLICER_MRU_ENT)loosemru;
	appDemoDevAmount = nam_asic_get_instance_num(); 

	for(devIdx=0;devIdx<appDemoDevAmount;devIdx++)
	{
#ifdef DRV_LIB_CPSS_3_4
		/*find right api*/
#else
		ret = cpssDxChPolicerTokenBucketModeSet(devIdx,mode,mru);	
		if(0 != ret) 
		{
			nam_syslog_dbg("cpssDxChPolicerTokenBucketModeSet error!result %d",ret);	
		}	
#endif
	}
#endif

	return ret;
}

/* strict mode */
/**********************************************************************************
 * nam_qos_global_policer_meter
 *
 * enable or disable flow policer on port  
 *
 *	INPUT:
 *		unsigned int policing	- policer mode l1 l2 l3
 *
 *	OUTPUT:
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_SUCCESS
 *		ACL_RETURN_CODE_ERROR
 *
 *	NOTE:
 *
 **********************************************************************************/
unsigned int nam_qos_global_policing
(
	unsigned int policing
)
{
		unsigned int ret = ACL_RETURN_CODE_SUCCESS;
#ifdef DRV_LIB_BCM
	ret = ACL_RETURN_CODE_SUCCESS;
#endif
	
#ifdef DRV_LIB_CPSS
	unsigned int			 appDemoDevAmount,devIdx;
	CPSS_POLICER_PACKET_SIZE_MODE_ENT  mode;
	CPSS_POLICER_TB_MODE_ENT strictmode = CPSS_POLICER_TB_STRICT_E;
	CPSS_POLICER_MRU_ENT strictmru = CPSS_POLICER_MRU_1536_E;

	memset(&mode,0,sizeof(CPSS_POLICER_PACKET_SIZE_MODE_ENT));
	mode = (CPSS_POLICER_PACKET_SIZE_MODE_ENT)policing;
	appDemoDevAmount = nam_asic_get_instance_num(); 

	for(devIdx=0;devIdx<appDemoDevAmount;devIdx++){
#ifdef DRV_LIB_CPSS_3_4		
		ret = cpssDxCh3PolicerPacketSizeModeSet(devIdx, 0, mode);
#else		
		ret = cpssDxChPolicerPacketSizeModeSet(devIdx, mode);		
#endif
		if(ret!=0) {
			nam_syslog_dbg("cpssDxChPolicerPacketSizeModeSet error!result %d",ret);
		}
#ifdef DRV_LIB_CPSS_3_4
		/*find right api*/
#else	
		ret = cpssDxChPolicerTokenBucketModeSet(devIdx,strictmode,strictmru);
#endif
		if(ret !=0) {
			nam_syslog_dbg("cpssDxChPolicerTokenBucketModeSet error!result %d",ret);
		}
	}
#endif

	return ret;
}

/**********************************************************************************
 * nam_qos_set_counter
 *
 * set qos counter
 *
 *	INPUT:
 *		unsigned int 	policerIndex - policer index
 *		unsigned long   inIndex		 - inprofile
 *		unsigned long	outIndex	 - outprofile	
 *
 *	OUTPUT:
 *
 *	RETURN:
 *		ACL_RETURN_CODE_SUCCESS
 *	NOTE:
 *
 **********************************************************************************/
unsigned int nam_qos_set_counter
(
	unsigned int    counterSetIndex,
	unsigned long   inIndex,
	unsigned long	outIndex
)
 {
 	unsigned int ret = ACL_RETURN_CODE_SUCCESS;
	unsigned int	appDemoDevAmount = 0,devIdx= 0;
	appDemoDevAmount = nam_asic_get_instance_num(); 	
#ifdef DRV_LIB_BCM
		ret = ACL_RETURN_CODE_SUCCESS;
#endif

#ifdef DRV_LIB_CPSS
		/*bcm_field_counter_create(unit, eid);*/
		/*bcm_field_counter_set(unit, eid, counter_num, val);*/
	#ifdef DRV_LIB_CPSS_3_4
		CPSS_DXCH3_POLICER_BILLING_ENTRY_STC cntrptr;
		memset(&cntrptr, 0, sizeof(CPSS_DXCH3_POLICER_BILLING_ENTRY_STC));
		cntrptr.greenCntr.l[0]= inIndex;
		/*cntrptr.yellowCntr.l[0] = outIndex;*/
		cntrptr.redCntr.l[0] = outIndex;
		cntrptr.billingCntrMode = CPSS_DXCH3_POLICER_BILLING_CNTR_1_BYTE_E;
	#else
		CPSS_DXCH_POLICER_COUNTERS_STC	   countersPtr;	
		memset(&countersPtr,0,sizeof(CPSS_DXCH_POLICER_COUNTERS_STC));
		countersPtr.outOfProfileBytesCnt = outIndex;
		countersPtr.inProfileBytesCnt = inIndex;
	#endif	

		for(devIdx=0;devIdx<appDemoDevAmount;devIdx++){ 
	#ifdef DRV_LIB_CPSS_3_4
			/* 
			ret = cpssDxChPolicerPolicyCntrSet(devIdx, 0, counterSetIndex, countersPtr);
			*/
			ret = cpssDxCh3PolicerBillingEntrySet(devIdx, CPSS_DXCH_POLICER_STAGE_INGRESS_0_E, counterSetIndex, &cntrptr);
	#else		
			ret = cpssDxChPolicerCountersSet(devIdx, counterSetIndex, &countersPtr);
	#endif
			if(ret!=0) {
			   nam_syslog_dbg("cpssDxChPolicerCountersSet error!result %d",ret);	
			}
		}
#endif

	return ret;
 }

 /**********************************************************************************
  * nam_qos_set_counter
  *
  * set qos counter
  *
  *  INPUT:
  * 	 unsigned int	 policerIndex - policer index
  * 	 unsigned long	 *Inprofile	  - inprofile
  * 	 unsigned long	 *Outprofile  - outprofile	 
  *
  *  OUTPUT:
  *
  *  RETURN:
  * 	 ACL_RETURN_CODE_SUCCESS
  *  NOTE:
  *
  **********************************************************************************/
 unsigned int nam_qos_read_counter
(
	unsigned int   counterSetIndex,
	unsigned long  *Inprofile,
	unsigned long  *Outprofile
)
{
#ifdef DRV_LIB_CPSS
#ifdef DRV_LIB_CPSS_3_4
	CPSS_DXCH3_POLICER_BILLING_ENTRY_STC cntrptr;
	unsigned int countersPtr = 0;
#else
	CPSS_DXCH_POLICER_COUNTERS_STC 	countersPtr; 
	memset(&countersPtr,0,sizeof(CPSS_DXCH_POLICER_COUNTERS_STC));
#endif
#endif
	unsigned int	i =0 , j = 0, ret = ACL_RETURN_CODE_SUCCESS;
	unsigned int appDemoDevAmount, devIdx;
	appDemoDevAmount = nam_asic_get_instance_num(); 

#ifdef DRV_LIB_BCM 
/*	args_t *a;
	a = malloc(sizeof(args_t));
	a->a_argv[0]= malloc(3);
	a->a_argv[1] = malloc(7);
	strcpy(a->a_argv[0], "fp"); 
	strcpy(a->a_argv[1], "detach");
	a->a_argc = 1;
	if_esw_field_proc(0, &a);
*/
	nam_syslog_dbg("nam_qos_read_counter \n");
	nam_syslog_dbg("read counter %d  \n", counterSetIndex);
	struct acl_rule_info_s		 *ruleInfo = NULL;
	struct list_head			*Ptr = NULL;
	struct acl_group_list_s	*groupPtr = NULL;
	unsigned int  add_index = 0, policerIndex = 0;
	long long valp = 0;
	unsigned long  valptmp[2] = {0};

	for (i = 0; i <  MAX_POLICER_NUM; i++) {
		if (NULL != g_policer[i]) {
			if ((counterSetIndex == (g_policer[i]->counterSetIndex)) && (g_policer[i]->counterEnable)){
				policerIndex = i;
				break;
			}
		}
	}
	nam_syslog_dbg("find policerindex is %d \n", policerIndex);
	if ((MAX_POLICER_NUM == i) && ((MAX_POLICER_NUM-1) != policerIndex)) {
		*Outprofile = g_counter[counterSetIndex]->outOfProfileBytesCnt;
		*Inprofile =  g_counter[counterSetIndex]->inProfileBytesCnt;
	}
	else {
		for (i = 0; i < MAX_GROUP_NUM; i++) {
			groupPtr = acl_group[i];
			if (NULL != groupPtr) {
				 __list_for_each(Ptr, &(groupPtr->list1)) {
					ruleInfo = list_entry(Ptr,struct acl_rule_info_s,list);
					if (NULL != ruleInfo) { 		
						add_index = ruleInfo->ruleIndex;
						if (policerIndex == g_acl_rule[add_index]->ActionPtr.policerPtr.policerId) {
							for(devIdx = 0; devIdx < appDemoDevAmount; devIdx++) {
								if (!((acl_group[i]->devnum^ACL_DEFAULT_DEVID)&(1<<devIdx))) {
									continue;
								}
								 /*      counter_num - (IN) Choice of upper (counter_num=1) or 
								 //                                   lower (counter_num=0)
								 //                    of the counter pair.
								 //      valp        - (OUT) Pointer to counter value*/
								 
								ret = bcm_field_counter_get(devIdx, j, 0, &valp);
								nam_syslog_dbg("bcm_field_counter_get valp %llu \n", valp);
								memcpy(valptmp, &valp, sizeof(long long));
								*Outprofile = valptmp[1];
								
								ret = bcm_field_counter_get(devIdx, j, 1, &valp);
								nam_syslog_dbg("bcm_field_counter_get valp %llu \n", valp);
								memcpy(valptmp, &valp, sizeof(long long));
								*Inprofile = valptmp[1];
							
								nam_syslog_dbg("bcm_field_counter_get devid %d, ruleindex %d, Inprofile %d, Outprofile %u\n",
									devIdx, j, *Inprofile, *Outprofile);
								if(ret != 0) {
									nam_syslog_dbg("bcm_field_counter_get error!result %d", ret);
								}
							}
						}
					}
				 }
			}
		}
	}
#endif	

#ifdef DRV_LIB_CPSS
	
	for(devIdx=0;devIdx<appDemoDevAmount;devIdx++){    
	#ifdef DRV_LIB_CPSS_3_4
/*
		ret = cpssDxChPolicerPolicyCntrGet(devIdx, 0, counterSetIndex, &countersPtr);
*/
		ret = cpssDxCh3PolicerBillingEntryGet(devIdx, CPSS_DXCH_POLICER_STAGE_INGRESS_0_E, counterSetIndex, 0, &cntrptr);

		*Outprofile  = cntrptr.redCntr.l[0];
		*Inprofile	 = cntrptr.greenCntr.l[0];
		nam_syslog_dbg("read counter for xcat green %d, yellow %d, red %d \n", 
						cntrptr.greenCntr.l[0], cntrptr.yellowCntr.l[0], cntrptr.redCntr.l[0]);
	#else		
		ret = cpssDxChPolicerCountersGet(devIdx, counterSetIndex, &countersPtr);

		*Outprofile  = countersPtr.outOfProfileBytesCnt;
		*Inprofile	 = countersPtr.inProfileBytesCnt;
	#endif
		if(ret!=0) {
			nam_syslog_dbg("cpssDxChPolicerCountersSet error!result %d",ret);
		}

	}
	
	nam_syslog_dbg("Inprofile  %ld,Outprofile  %ld", *Inprofile, *Outprofile);
#endif

	return ret;
}
 
/* bcm_field_counter_destroy(unit, eid);*/
/**********************************************************************************
 * nam_qos_policer_cir_cbs_check
 *
 * check cir and cbs
 *
 *	INPUT:
 *		unsigned int	cir	- commited information rate in kilobits/sec (kilo = 1000)
 *		unsigned int	cbs - commited burst size in bytes
 *
 *	OUTPUT:
 *
 *	RETURN:
 *		ACL_RETURN_CODE_SUCCESS
 *		QOS_POLICER_CBS_LITTLE
 *		QOS_POLICER_CBS_BIG
 *
 *	NOTE:
 *
 **********************************************************************************/
unsigned int nam_qos_policer_cir_cbs_check
(
	unsigned int  cir,
	unsigned int  cbs
)
{
	unsigned int ret = ACL_RETURN_CODE_SUCCESS;
	if (cbs < 2000) {
		return QOS_RETURN_CODE_POLICER_CBS_LITTLE;
	}
	if (cbs < cir) {
		return QOS_RETURN_CODE_POLICER_CBS_LITTLE;
	}
	if (cbs > 40*cir) {
		return QOS_RETURN_CODE_POLICER_CBS_BIG;
	}
	/*
	if(cir<512) {
		if(cbs<1)
		ret = QOS_RETURN_CODE_POLICER_CBS_LITTLE;
		else if(cbs>64000)
		ret = QOS_RETURN_CODE_POLICER_CBS_BIG;
		else if((cbs>=1)&&(cbs<=64000))
		ret = ACL_RETURN_CODE_SUCCESS;
		return ret;
	}
	else if(cir<5120) {
		if(cbs<8)
		ret = QOS_RETURN_CODE_POLICER_CBS_LITTLE;
		else if(cbs>512000)
		ret = QOS_RETURN_CODE_POLICER_CBS_BIG;
		else if((cbs>=8)&&(cbs<=512000))
		ret = ACL_RETURN_CODE_SUCCESS;
		return ret;
	}
	else if(cir<51200) {
		if(cbs<64)
		ret = QOS_RETURN_CODE_POLICER_CBS_LITTLE;
		else if(cbs>4000000)
		ret = QOS_RETURN_CODE_POLICER_CBS_BIG;
		else if((cbs>=64)&&(cbs<=4000000))
		ret = ACL_RETURN_CODE_SUCCESS;
		return ret;
	}
	else if(cir<512000) {
		if(cbs<512)
		ret = QOS_RETURN_CODE_POLICER_CBS_LITTLE;
		else if(cbs>32000000)
		ret = QOS_RETURN_CODE_POLICER_CBS_BIG;
		else if((cbs>=512)&&(cbs<=32000000))
		ret = ACL_RETURN_CODE_SUCCESS;
		return ret;
	}		
	else if(cir<5120000) {
		if(cbs<4000)
		ret = QOS_RETURN_CODE_POLICER_CBS_LITTLE;
		else if(cbs>256000000)
		ret = QOS_RETURN_CODE_POLICER_CBS_BIG;
		else if((cbs>=4000)&&(cbs<=256000000))
		ret = ACL_RETURN_CODE_SUCCESS;
		return ret;
	}	
	else if (cir <= 100000000) {
		if (cbs < 32000)
		ret = QOS_RETURN_CODE_POLICER_CBS_LITTLE;
		else if (cbs > 2000000000)
		ret = QOS_RETURN_CODE_POLICER_CBS_BIG;
		else if ((cbs >= 32000)&&(cbs <= 2000000000))
		ret = ACL_RETURN_CODE_SUCCESS;
		return ret;
	}	
*/
	return ret;
}


#ifdef __cplusplus
}
#endif

