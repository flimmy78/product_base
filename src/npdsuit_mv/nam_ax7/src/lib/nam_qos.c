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
* nam_qos.c
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
*  		$Revision: 1.40 $	
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
#include "sysdef/returncode.h"

#ifdef DRV_LIB_CPSS
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortTx.h>
#include <cpss/dxCh/dxChxGen/cos/cpssDxChCos.h>
#define PRV_CPSS_DXCH_CPU_PORT_PROFILE       CPSS_PORT_TX_SCHEDULER_PROFILE_1_E
#define PRV_CPSS_DXCH_NET_GE_PORT_PROFILE    CPSS_PORT_TX_SCHEDULER_PROFILE_2_E
#define PRV_CPSS_DXCH_NET_10GE_PORT_PROFILE  CPSS_PORT_TX_SCHEDULER_PROFILE_3_E
#define PRV_CPSS_DXCH_CASCADING_PORT_PROFILE CPSS_PORT_TX_SCHEDULER_PROFILE_4_E
#endif

#ifdef DRV_LIB_BCM
#include <sal/core/libc.h>
#include <bcm/cosq.h>
#include <bcm/types.h>
#endif

extern AclRuleList **g_acl_rule;

extern QOS_WRR_TX_WEIGHT_E	*g_queue, *g_hybrid;
extern QOS_PROFILE_STC 		   **g_qos_profile;
extern QOS_UP_PROFILE_MAP_STC     **g_up_profile ;
extern QOS_DSCP_PROFILE_MAP_STC   **g_dscp_profile;
extern QOS_DSCP_DSCP_REMAP_STC    **g_dscp_dscp ;
extern QOS_PORT_POLICY_MAP_ATTRIBUTE_STC **g_policy_map;

extern unsigned long nam_asic_get_instance_num
(
	void
);

/**********************************************************************************
 * nam_qos_profile_entry_set
 *
 * set qos profile entry
 *
 *	INPUT:
 *		unsigned int		profileIndex - qos profile index
 *		QOS_PROFILE_STC 	*profile	 - qos profile entry struct
 *
 *	OUTPUT:
 *
 *	RETURN:
 *		QOS_RETURN_CODE_SUCCESS	- set qos profile entry success
 *		NPD_DBUS__ERROR		- set rule tcam fail
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int nam_qos_profile_entry_set
(
     unsigned int     profileIndex,
     QOS_PROFILE_STC *profile
)
{
	unsigned int ret=QOS_RETURN_CODE_SUCCESS;
#ifdef DRV_LIB_BCM
	ret = QOS_RETURN_CODE_SUCCESS;
#endif

#ifdef DRV_LIB_CPSS

	CPSS_DXCH_COS_PROFILE_STC  cosPtr,reback;
	unsigned int devIdx,appDemoDevAmount,rs;
	
	memset (&cosPtr,0,sizeof(CPSS_DXCH_COS_PROFILE_STC));

	appDemoDevAmount = nam_asic_get_instance_num();

	cosPtr.dropPrecedence = (CPSS_DP_LEVEL_ENT)(profile->dropPrecedence);
	cosPtr.dscp = profile->dscp;
	cosPtr.trafficClass = profile->trafficClass;
	cosPtr.userPriority = profile->userPriority;

	for(devIdx=0;devIdx<appDemoDevAmount;devIdx++){
		ret=cpssDxChCosProfileEntrySet((unsigned char)devIdx,(unsigned char)profileIndex,&cosPtr);
		if(ret!=QOS_RETURN_CODE_SUCCESS) {
			nam_syslog_dbg("cpssDxChCosProfileEntrySet error! result %d\n",ret);
		}
		rs=cpssDxChCosProfileEntryGet((unsigned char)devIdx,(unsigned char)profileIndex,&reback);
		if(rs!=QOS_RETURN_CODE_SUCCESS) {
			nam_syslog_dbg("cpssDxChCosProfileEntryGet error! result %d\n",rs);
		}
	}
#endif
	
	return ret;
}

/**********************************************************************************
 * bcm_nam_qos_up_to_profile_entry
 *
 * set qos profile entry
 *
 *	INPUT:
 *		unsigned int		profileIndex - qos profile index
 *		QOS_PROFILE_STC 	*profile	 - qos profile entry struct
 *
 *	OUTPUT:
 *
 *	RETURN:
 *		QOS_RETURN_CODE_SUCCESS	- set qos profile entry success
 *		NPD_DBUS__ERROR		- set rule tcam fail
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int bcm_nam_qos_up_to_profile_entry
(
	unsigned int up,
	unsigned int profileIndex,
	int unit,
	int port
)
{
	unsigned int ret = QOS_RETURN_CODE_SUCCESS;
#ifdef DRV_LIB_BCM	
	unsigned char newdscp = 0;
	bcm_vlan_t newpri = 0;
	int cosqueue = 0;
	nam_syslog_dbg("start bcm_nam_qos_up_to_profile_entry \n");

	newdscp = g_qos_profile[profileIndex]->dscp;
	newpri = g_qos_profile[profileIndex]->userPriority;
	cosqueue = g_qos_profile[profileIndex]->trafficClass;

	ret = bcm_port_vlan_priority_map_set(unit, port, up, 0, newpri, bcmColorGreen);
	nam_syslog_dbg("bcm_port_vlan_priority_map_set unit %d, port %d, up %d, newpri %d\n",
		unit, port, up, newpri);
	if (QOS_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR ! bcm_port_vlan_priority_map_set ret %d\n",ret);	
	}
	ret = bcm_port_vlan_priority_map_set(unit, port, up, 1, newpri, bcmColorGreen);
	nam_syslog_dbg("bcm_port_vlan_priority_map_set unit %d, port %d, up %d, newpri %d\n",
		unit, port, up, newpri);
	if (QOS_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR ! bcm_port_vlan_priority_map_set ret %d\n",ret);	
	}
	ret = bcm_cosq_port_mapping_set(unit, port, newpri, cosqueue);
	nam_syslog_dbg("bcm_cosq_port_mapping_set unit %d, port %d, newpri %d, cosqueue %d\n",
		unit, port, newpri, cosqueue);
	if (QOS_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR ! bcm_cosq_port_mapping_set ret %d\n",ret);	
	}
#endif

#ifdef DRV_LIB_CPSS
	ret = QOS_RETURN_CODE_SUCCESS;
#endif

	return ret;
}

unsigned int nam_qos_up_to_profile_entry
(
	unsigned int up,
	unsigned int profileIndex,
	int unit,
	int port
)
{
	unsigned int ret = QOS_RETURN_CODE_SUCCESS;
#ifdef DRV_LIB_BCM
	if (0 == profileIndex) {
		ret = bcm_port_vlan_priority_map_set(unit, port, up, 0, up, bcmColorGreen);
		nam_syslog_dbg("bcm_port_vlan_priority_map_set unit %d, port %d, up %d, newpri %d\n",
			unit, port, up, up);
		if (QOS_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_err("ERROR ! bcm_port_vlan_priority_map_set ret %d\n",ret);	
		}
		ret = bcm_port_vlan_priority_map_set(unit, port, up, 1, up, bcmColorGreen);
		nam_syslog_dbg("bcm_port_vlan_priority_map_set unit %d, port %d, up %d, newpri %d\n",
			unit, port, up, up);
		if (QOS_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_err("ERROR ! bcm_port_vlan_priority_map_set ret %d\n",ret);	
		}
		ret = bcm_cosq_port_mapping_set(unit, port, up, up);
		nam_syslog_dbg("bcm_port_vlan_priority_map_set unit %d, port %d, up %d, newpri %d\n",
			unit, port, up, up);
		if (QOS_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_err("ERROR ! bcm_cosq_port_mapping_set ret %d\n",ret);	
		}
	}
	else if (ACL_ANY_PORT_CHAR == port){
		return ret;
	}
	else {
		ret = bcm_nam_qos_up_to_profile_entry(up, profileIndex, unit, port);
		if (QOS_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_err("ERROR ! bcm_cosq_port_mapping_set ret %d\n",ret);	
		}
	}
	ret = ACL_RETURN_CODE_BCM_DEVICE_NOT_SUPPORT;
#endif

#ifdef DRV_LIB_CPSS
	unsigned int devIdx = 0,appDemoDevAmount = 0;
	unsigned char getIndex=0;
	
	appDemoDevAmount = nam_asic_get_instance_num();

	for(devIdx=0; devIdx<appDemoDevAmount; devIdx++){
		#ifdef DRV_LIB_CPSS_3_4
			cpssDxChCosUpCfiDeiToProfileMapSet((unsigned char)devIdx, 0, (unsigned char)up, 0, 0);
			cpssDxChCosUpCfiDeiToProfileMapSet((unsigned char)devIdx, 0, (unsigned char)up, 1, 0);
			ret=cpssDxChCosUpCfiDeiToProfileMapSet((unsigned char)devIdx, 0, (unsigned char)up, 0, (unsigned char)profileIndex);
			if(ret!=QOS_RETURN_CODE_SUCCESS) {
				nam_syslog_err("cpssDxChCosUpToProfileMapSet error! result %d\n",ret);
			}
			ret=cpssDxChCosUpCfiDeiToProfileMapSet((unsigned char)devIdx, 0, (unsigned char)up, 1, (unsigned char)profileIndex);
			if(ret!=QOS_RETURN_CODE_SUCCESS) {
				nam_syslog_err("cpssDxChCosUpToProfileMapSet error! result %d\n",ret);
			}
			cpssDxChCosUpCfiDeiToProfileMapGet((unsigned char)devIdx, 0, (unsigned char)up, 1,&getIndex);
		#else
			cpssDxChCosUpToProfileMapSet((unsigned char)devIdx,(unsigned char)up,0);
			ret=cpssDxChCosUpToProfileMapSet((unsigned char)devIdx,(unsigned char)up,(unsigned char)profileIndex);
			if(ret!=QOS_RETURN_CODE_SUCCESS) {
				nam_syslog_err("cpssDxChCosUpToProfileMapSet error! result %d\n",ret);
			}
			cpssDxChCosUpToProfileMapGet((unsigned char)devIdx,(unsigned char)up,&getIndex);
			/* nam_syslog_dbg("getIndex %d\n",getIndex);*/
		#endif
	}
#endif

	return ret;
}

unsigned int nam_qos_dscp_to_dscp_entry
(
	unsigned int oldDscp,
	unsigned int newDscp
)
{
		unsigned int ret=QOS_RETURN_CODE_SUCCESS;
#ifdef DRV_LIB_BCM
		ret = ACL_RETURN_CODE_BCM_DEVICE_NOT_SUPPORT;
#endif
#ifdef DRV_LIB_CPSS

	unsigned int devIdx = 0,appDemoDevAmount = 0;
	unsigned char getIndex=0;
	
	appDemoDevAmount = nam_asic_get_instance_num();
	
	for(devIdx=0;devIdx<appDemoDevAmount;devIdx++){
		cpssDxChCosDscpMutationEntrySet((unsigned char)devIdx,(unsigned char)oldDscp,0);
		ret=cpssDxChCosDscpMutationEntrySet((unsigned char)devIdx,(unsigned char)oldDscp,(unsigned char)newDscp);
		if(ret!=QOS_RETURN_CODE_SUCCESS) {
			nam_syslog_err("cpssDxChCosDscpMutationEntrySet error! result %d\n",ret);
		}
		cpssDxChCosDscpMutationEntryGet((unsigned char)devIdx,(unsigned char)oldDscp,&getIndex);
		/*nam_syslog_dbg("getIndex %d\n",getIndex);*/
	}
#endif	

	return ret;
}


unsigned int bcm_nam_qos_dscp_to_profile_entry
(
	unsigned int dscp,
	unsigned int profileIndex,
	int unit,
	int port
)
{
	unsigned int ret = QOS_RETURN_CODE_SUCCESS;
	
#ifdef DRV_LIB_BCM
	unsigned char newdscp = 0;
	bcm_vlan_t newpri = 0;
	int cosqueue = 0;
	nam_syslog_dbg("start bcm_nam_qos_dscp_to_profile_entry\n");	

	newdscp = g_qos_profile[profileIndex]->dscp;
	newpri = g_qos_profile[profileIndex]->userPriority;
	cosqueue = g_qos_profile[profileIndex]->trafficClass;

/*     	    mode - (OUT)*/
/*           - BCM_PORT_DSCP_MAP_NONE*/
/*           - BCM_PORT_DSCP_MAP_ZERO Map if incomming DSCP = 0*/
/*           - BCM_PORT_DSCP_MAP_ALL DSCP -> DSCP mapping. */
	ret = bcm_port_dscp_map_mode_set(unit, port, 2);
	nam_syslog_dbg("bcm_port_dscp_map_mode_set ret %d, unit %d port %d \n",ret, unit ,port);
	if (QOS_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_err("ERROR ! bcm_port_dscp_map_mode_set ret %d\n",ret);	
	}
	ret = bcm_port_dscp_map_set(unit, port, dscp, newdscp, newpri);	
	nam_syslog_dbg("bcm_port_dscp_map_set ret %d, unit %d port %d dscp %d, newdscp %d, newpri %d\n",
		ret, unit ,port,dscp,newdscp,newpri);
	if (QOS_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_err("ERROR ! bcm_port_dscp_map_set ret %d\n",ret);	
	}
	ret = bcm_cosq_port_mapping_set(unit, port, newpri, cosqueue);
	nam_syslog_dbg("bcm_port_dscp_map_set ret %d, unit %d port %d  newpri %d cosqueue %d\n",
		ret, unit ,port, newpri, cosqueue);
	if (QOS_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_err("ERROR ! bcm_cosq_port_mapping_set ret %d\n",ret);	
	}
/*	bcm_cosq_mapping_set(unit, newpri, cosqueue);*/
#endif

#ifdef DRV_LIB_CPSS
	ret = QOS_RETURN_CODE_SUCCESS;
#endif

	return ret;
}

unsigned int nam_qos_dscp_to_profile_entry
(
	unsigned int dscp,
	unsigned int profileIndex,
	int unit,
	int port
)
{
	unsigned int ret = QOS_RETURN_CODE_SUCCESS;
	
#ifdef DRV_LIB_BCM
	/*nam_syslog_dbg("start nam_qos_dscp_to_profile_entry for bcm \n");*/
	if (0 == profileIndex) {
		if (dscp < MAX_UP_PROFILE_NUM) {
			ret = bcm_port_dscp_map_set(unit, port, dscp, dscp, dscp);
			nam_syslog_dbg("bcm_port_dscp_map_set ret %d, port %d, srcdscp %d, mapdscp %d, prio\n",
				ret, port, dscp, dscp, dscp);
			if (QOS_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_err("ERROR ! bcm_port_dscp_map_set ret %d\n",ret);	
			}
			ret = bcm_cosq_port_mapping_set(unit, port, dscp, dscp);
			nam_syslog_dbg("bcm_cosq_port_mapping_set ret %d, port %d, pri %d, cosq %d\n",
				ret, port, dscp, dscp);
			if (QOS_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_err("ERROR ! bcm_cosq_port_mapping_set ret %d\n",ret);	
			}
		}
		else {
			ret = bcm_port_dscp_map_set(unit, port, dscp, dscp, 0);
			nam_syslog_dbg("bcm_port_dscp_map_set ret %d, port %d, srcdscp %d, mapdscp %d, prio\n",
				ret, port, dscp, dscp, 0);
			if (QOS_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_err("ERROR ! bcm_port_dscp_map_set ret %d\n",ret);	
			}
			ret = bcm_cosq_port_mapping_set(unit, port, 0, 0);
			nam_syslog_dbg("bcm_cosq_port_mapping_set ret %d, port %d, pri %d, cosq %d\n",
				ret, port, 0, 0);
			if (QOS_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_err("ERROR ! bcm_cosq_port_mapping_set ret %d\n",ret);	
			}
		}
	}
	else if (ACL_ANY_PORT_CHAR == port){
	/*	nam_syslog_dbg("port is ACL_ANY_PORT_CHAR \n");*/
		return ret;
	}
	else {
		ret = bcm_nam_qos_dscp_to_profile_entry(dscp, profileIndex, unit, port);
		nam_syslog_dbg("bcm_nam_qos_dscp_to_profile_entry dscp %d, profileindex %d, unit %d, port %d,\n",
			dscp, profileIndex, unit, port);
		if (QOS_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_err("ERROR ! bcm_cosq_port_mapping_set ret %d\n",ret);	
		}
	}
#endif

#ifdef DRV_LIB_CPSS
	unsigned int devIdx = 0,appDemoDevAmount = 0;
	unsigned char getIndex=0;
	
	appDemoDevAmount = nam_asic_get_instance_num();	
	for(devIdx=0;devIdx<appDemoDevAmount;devIdx++) {
		cpssDxChCosDscpToProfileMapSet((unsigned char)devIdx,(unsigned char)dscp,0);
		ret=cpssDxChCosDscpToProfileMapSet((unsigned char)devIdx,(unsigned char)dscp,(unsigned char)profileIndex);
		if(ret!=QOS_RETURN_CODE_SUCCESS) {
			nam_syslog_err("cpssDxChCosDscpToProfileMapSet error! result %d\n",ret);
		}
		cpssDxChCosDscpToProfileMapGet((unsigned char)devIdx,(unsigned char)dscp,&getIndex);
		/*nam_syslog_dbg("getIndex %d\n",getIndex);*/
	}
#endif

	return ret;
}

#if 0
unsigned int nam_qos_read_qos
(
    unsigned char dev,
    unsigned char port,
    CPSS_QOS_PORT_TRUST_MODE_ENT *portQosTrustMode,
    CPSS_QOS_ENTRY_STC			 *portQosCfg_PTR,
    unsigned char				 *defaultUserPrio,
    unsigned char				 *enableDscpMutation
 )
{
    cpssDxChCosPortQosTrustModeGet(dev,port,&portQosTrustMode);
    /*  entry with QoS parameters bits: 7,8,13 and 25-31 */
	cpssDxChCosPortQosConfigGet(dev,port,&portQosCfg_PTR);	
	cpssDxChPortDefaultUPGet(dev,port,&defaultUserPrio);
	cpssDxChCosPortReMapDSCPGet(dev,port,&enableDscpMutation);

    return 0;
}
#endif	

unsigned int bcm_nam_qos_support_check
(
)
{
#ifdef DRV_LIB_BCM
	return ACL_RETURN_CODE_BCM_DEVICE_NOT_SUPPORT;
#endif

#ifdef DRV_LIB_CPSS
	return QOS_RETURN_CODE_SUCCESS;
#endif
}

unsigned int nam_qos_port_bind_policy_map
(
	unsigned char dev,
	unsigned char port,
	unsigned int  policyIndex
)
{
	unsigned int 		i = 0, j = 0, profileIndex = 0, ret = QOS_RETURN_CODE_SUCCESS;
	QOS_PORT_POLICY_MAP_ATTRIBUTE_STC   *policy=NULL;
	QOS_PORT_TRUST_MODE_FLAG		 portQosTrustMode = PORT_UNTRUST_E;

#ifdef DRV_LIB_BCM
	unsigned char dscp = 0;
	bcm_vlan_t pri = 0;
	nam_syslog_dbg("nam_qos_port_bind_policy_map \n");	
	policy = g_policy_map[policyIndex];
	if(NULL != policy) {
		portQosTrustMode = policy->trustFlag;
		if (PORT_L2_TRUST_E == portQosTrustMode) {
			nam_syslog_dbg("CPSS_QOS_PORT_TRUST_L2_E \n");
			for(i=0; i < MAX_UP_PROFILE_NUM; i++) {
				nam_syslog_dbg("MAX_UP_PROFILE_NUM \n");
				if (NULL != g_up_profile[i]) {
					nam_syslog_dbg("g_up_profile is %d \n", i);
					pri = i;
					profileIndex = g_up_profile[i]->profileIndex;
					nam_syslog_dbg("profileIndex is %d \n", profileIndex);
					ret = bcm_nam_qos_up_to_profile_entry(pri, profileIndex, dev, port);
					nam_syslog_dbg("bcm_nam_qos_up_to_profile_entry pri %d, profileIndex %d,dev %d, port %d\n", 
						pri, profileIndex, dev, port);	
					if(ret != QOS_RETURN_CODE_SUCCESS) {
						nam_syslog_err("fail to set up to qos profile map table entry "); 
					}
				}
			}
		}
		else if (PORT_L3_TRUST_E == portQosTrustMode) {
			for(j=0; j<MAX_DSCP_PROFILE_NUM; j++) {
				if (NULL != g_dscp_profile[j]) {
					dscp = j;
					profileIndex = g_dscp_profile[j]->profileIndex;
					ret = bcm_nam_qos_dscp_to_profile_entry(dscp, profileIndex, dev, port);
					nam_syslog_dbg("bcm_nam_qos_dscp_to_profile_entry dscp %d, profileIndex %d,dev %d, port %d\n", 
							dscp, profileIndex, dev, port);	
					if(ret != QOS_RETURN_CODE_SUCCESS) {
						nam_syslog_err("fail to set dscp to profile map table entry ");		
					}
				}
				else {
					if (j <  MAX_UP_PROFILE_NUM) {
						ret = bcm_port_dscp_map_set(dev, port, j, j, j);
						nam_syslog_dbg("bcm_port_dscp_map_set ret %d, unit %d, port %d, srcdscp %d, mapdscp %d, prio %d \n",
							ret, dev, port, j, j, j);
						if (QOS_RETURN_CODE_SUCCESS != ret) {
							nam_syslog_err("ERROR ! bcm_port_dscp_map_set ret %d\n",ret);	
						}
					}
					else {
						ret = bcm_port_dscp_map_set(dev, port, j, j, 0);
						nam_syslog_dbg("bcm_port_dscp_map_set ret %d, unit %d, port %d, srcdscp %d, mapdscp %d, prio %d \n",
							ret, dev, port, j, j, 0);
						if (QOS_RETURN_CODE_SUCCESS != ret) {
							nam_syslog_err("ERROR ! bcm_port_dscp_map_set ret %d\n",ret);	
						}
					}
				}
			}
		}
		else if (PORT_L2_L3_TRUST_E == portQosTrustMode) {
			return ACL_RETURN_CODE_BCM_DEVICE_NOT_SUPPORT;
		}
	}
#endif

#ifdef DRV_LIB_CPSS
	CPSS_QOS_ENTRY_STC					 portQosCfg_PTR;
	unsigned char						 enableDscpMutation=0;

	memset(&portQosCfg_PTR,0,sizeof(CPSS_QOS_ENTRY_STC));
	
	policy = g_policy_map[policyIndex];
	if(policy!=NULL) {
		portQosCfg_PTR.assignPrecedence =(CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)(policy->assignPrecedence);
		portQosCfg_PTR.enableModifyDscp =(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(policy->modifyDscp);
		portQosCfg_PTR.enableModifyUp	=(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)(policy->modifyUp);
/*		portQosCfg_PTR.qosProfileId 	=policy->defaultProfileIndex;*/

		ret = cpssDxChCosPortQosConfigSet(dev,port,&portQosCfg_PTR);
		if(ret!=QOS_RETURN_CODE_SUCCESS) {
			nam_syslog_dbg("cpssDxChCosPortQosConfigSet error! result %d\n",ret);
		}
		/*defaultUserPrio = policy->defaultUp;
		result = cpssDxChPortDefaultUPSet(dev,port,defaultUserPrio);
		if(result!=QOS_RETURN_CODE_SUCCESS)
			nam_syslog_dbg("cpssDxChPortDefaultUPSet error! result %d\n",result);*/

		portQosTrustMode = (QOS_PORT_TRUST_MODE_FLAG)(policy->trustFlag);
		ret = cpssDxChCosPortQosTrustModeSet(dev,port,portQosTrustMode);
		if(ret!=QOS_RETURN_CODE_SUCCESS) {
			nam_syslog_err("cpssDxChCosPortQosTrustModeSet error! result %d\n",ret);
		}
		enableDscpMutation = policy->remapDscp;
		ret = cpssDxChCosPortReMapDSCPSet(dev,port,enableDscpMutation);
		nam_syslog_dbg("trust mode is %d, port is %d, remap is %d", 
			portQosTrustMode, port, enableDscpMutation);
		if(ret!=QOS_RETURN_CODE_SUCCESS) {
			nam_syslog_err("cpssDxChCosPortReMapDSCPSet error! result %d\n",ret);
		}
	}/*for*/
#endif

	return ret;
}

unsigned int nam_qos_port_unbind_policy_map
(
	unsigned char dev,
	unsigned char port,
	unsigned int  policyIndex
)
{
	unsigned int 		i = 0, j = 0, profileIndex = 0, ret = QOS_RETURN_CODE_SUCCESS;
	QOS_PORT_POLICY_MAP_ATTRIBUTE_STC   *policy=NULL;
	QOS_PORT_TRUST_MODE_FLAG		 portQosTrustMode = PORT_UNTRUST_E;

#ifdef DRV_LIB_BCM	
	unsigned char dscp = 0;
	bcm_vlan_t pri = 0;
	int cosqueue = 0;
	policy = g_policy_map[policyIndex];
	nam_syslog_dbg("nam_qos_port_unbind_policy_map\n");
	if(policy!=NULL) {
		portQosTrustMode = policy->trustFlag;
		if (PORT_L2_TRUST_E == portQosTrustMode) {
			for(i=0; i < MAX_UP_PROFILE_NUM; i++) {
				ret = bcm_port_vlan_priority_map_set(dev, port, i, 0, i, bcmColorGreen);
				nam_syslog_dbg("bcm_port_vlan_priority_map_set ret %d, unit %d, port %d, pktpri %d, cfi 0, interpri %d, color bcmColorGreen\n",
					ret, dev, port, i, i);
				if (QOS_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_err("ERROR ! bcm_port_vlan_priority_map_set ret %d\n",ret);	
				}
				ret = bcm_port_vlan_priority_map_set(dev, port, i, 1, i, bcmColorGreen);
				nam_syslog_dbg("bcm_port_vlan_priority_map_set ret %d, unit %d, port %d, pktpri %d, cfi 1, interpri %d, color bcmColorGreen\n",
					ret, dev, port, i, i);
				if (QOS_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_err("ERROR ! bcm_port_vlan_priority_map_set ret %d\n",ret);	
				}
				ret = bcm_cosq_port_mapping_set(dev, port, i, i);
				nam_syslog_dbg("bcm_cosq_port_mapping_set ret %d, unit %d, port %d, pktpri %d, cosqueue %d\n",
					ret, dev, port, i, i);
				if (QOS_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_err("ERROR ! bcm_cosq_port_mapping_set ret %d\n",ret);	
				}
			}
		}
		else if (PORT_L3_TRUST_E == portQosTrustMode) {
			ret = bcm_port_dscp_map_mode_set(dev, port, BCM_PORT_DSCP_MAP_NONE);
			nam_syslog_dbg("bcm_port_dscp_map_mode_set ret %d, unit %d, port %d, mode 2 \n",
				ret, dev, port);
			if (QOS_RETURN_CODE_SUCCESS != ret) {
				nam_syslog_err("ERROR ! bcm_port_dscp_map_mode_set ret %d\n",ret);	
			}
			for(i = 0; i < MAX_UP_PROFILE_NUM; i++) {
				ret = bcm_cosq_port_mapping_set(dev, port, i, i);
				nam_syslog_dbg("bcm_cosq_port_mapping_set ret %d, unit %d, port %d, pktpri %d, cosqueue %d\n",
					ret, dev, port, i, i);
				if (QOS_RETURN_CODE_SUCCESS != ret) {
					nam_syslog_err("ERROR ! bcm_cosq_port_mapping_set ret %d\n",ret);	
				}
			}
			/*
			for(j=0; j<MAX_DSCP_PROFILE_NUM; j++) {
				if (j <  MAX_UP_PROFILE_NUM) {
					dscp = j;
					pri = j;
					cosqueue = j;
					ret = bcm_port_dscp_map_set(dev, port, j, dscp, pri);
					nam_syslog_dbg("bcm_port_dscp_map_set ret %d, unit %d, port %d, srcdscp %d, mapdscp %d, prio %d \n",
						ret, dev, port, j, dscp, pri);
					if (QOS_RETURN_CODE_SUCCESS != ret) {
						nam_syslog_dbg("ERROR ! bcm_port_dscp_map_set ret %d\n",ret);	
					}
					ret = bcm_cosq_port_mapping_set(dev, port, pri, cosqueue);
					nam_syslog_dbg("bcm_cosq_port_mapping_set ret %d, unit %d, port %d, prio %d, cosqueue %d\n",
						ret, dev, port, pri, cosqueue);					
					if (QOS_RETURN_CODE_SUCCESS != ret) {
						nam_syslog_dbg("ERROR ! bcm_cosq_port_mapping_set ret %d\n",ret);	
					}
				}
				else {
					ret = bcm_port_dscp_map_set(dev, port, j, j, 0);
					nam_syslog_dbg("bcm_port_dscp_map_set ret %d, unit %d, port %d, srcdscp %d, mapdscp %d, prio 0 \n",
						ret, dev, port, j, j);							
					if (QOS_RETURN_CODE_SUCCESS != ret) {
						nam_syslog_dbg("ERROR ! bcm_port_dscp_map_set ret %d\n",ret);	
					}
					ret = bcm_cosq_port_mapping_set(dev, port, 0, 0);
					nam_syslog_dbg("bcm_cosq_port_mapping_set ret %d, unit %d, port %d, prio 0, cosq 0\n",
						ret, dev, port);							
					if (QOS_RETURN_CODE_SUCCESS != ret) {
						nam_syslog_dbg("ERROR ! bcm_cosq_port_mapping_set ret %d\n",ret);	
					}
				}
			}
			*/
		}
	}
#endif

#ifdef DRV_LIB_CPSS	
	CPSS_QOS_ENTRY_STC					 portQosCfg_PTR;

	memset(&portQosCfg_PTR,0,sizeof(CPSS_QOS_ENTRY_STC));
	
	portQosCfg_PTR.enableModifyUp=CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
	portQosCfg_PTR.enableModifyDscp=CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
	
	ret = cpssDxChCosPortQosConfigSet(dev,port,&portQosCfg_PTR);
	if(ret!=QOS_RETURN_CODE_SUCCESS) {
		nam_syslog_err("cpssDxChCosPortQosConfigSet error! result %d\n",ret);
	}
	ret = cpssDxChPortDefaultUPSet(dev,port,0);
	if(ret!=QOS_RETURN_CODE_SUCCESS) {
		nam_syslog_err("cpssDxChPortDefaultUPSet error! result %d\n",ret);
	}
	ret = cpssDxChCosPortQosTrustModeSet(dev,port,CPSS_QOS_PORT_NO_TRUST_E);
	if(ret!=QOS_RETURN_CODE_SUCCESS) {
		nam_syslog_err("cpssDxChCosPortQosTrustModeSet error! result %d\n",ret);
	}
	ret = cpssDxChCosPortReMapDSCPSet(dev,port,0);
	if(ret!=QOS_RETURN_CODE_SUCCESS) {
		nam_syslog_err("cpssDxChCosPortReMapDSCPSet error! result %d\n",ret);
	}
	#if 0
	nam_syslog_dbg("==========after clear hw,get==========\n");
	 cpssDxChCosPortQosTrustModeGet(dev,port,&portQosTrustMode);
	 nam_syslog_dbg("portQosTrustMode %0x\n",portQosTrustMode);
	   /*  entry with QoS parameters bits: 7,8,13 and 25-31 */
	 cpssDxChCosPortQosConfigGet(dev,port,&portQosCfg_PTR);
	 nam_syslog_dbg("portQosCfg_PTR %0x\n",&portQosCfg_PTR);
	 cpssDxChPortDefaultUPGet(dev,port,&defaultUserPrio);
	 nam_syslog_dbg("defaultUserPrio %d\n",defaultUserPrio);
	 cpssDxChCosPortReMapDSCPGet(dev,port,&enableDscpMutation);
	 nam_syslog_dbg("enableDscpMutation %d\n",enableDscpMutation);
	nam_syslog_dbg("========================================\n");
    #endif
#endif

	return ret;
}

 unsigned int nam_qos_queue_init()
 {
	 unsigned int i = 0,ret=QOS_RETURN_CODE_SUCCESS,devIdx=0,appDemoDevAmount=0;
	 appDemoDevAmount = nam_asic_get_instance_num();
	 
#ifdef DRV_LIB_BCM 
/*not need*/
/*	
	for(devIdx=0;devIdx<appDemoDevAmount;devIdx++){
		ret = bcm_cosq_init(devIdx);
		if (QOS_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_dbg("ERROR ! bcm_esw_cosq_init ret %d\n",ret);	
		}
	}
*/	
#endif

#ifdef DRV_LIB_CPSS
	 for(devIdx=0;devIdx<appDemoDevAmount;devIdx++){ 
		 ret=cpssDxChPortTxInit(devIdx);
		 if(ret!=0) {
		 	nam_syslog_err("cpssDxChPortTxInit fail,ret %d\n",ret);
		 }
		  /*ret=cpssDxChPortTxWrrGlobalParamSet(devIdx,CPSS_PORT_TX_WRR_BYTE_MODE_E,CPSS_PORT_TX_WRR_MTU_2K_E);*/
		  ret=cpssDxChPortTxWrrGlobalParamSet(devIdx,CPSS_PORT_TX_WRR_PACKET_MODE_E,CPSS_PORT_TX_WRR_MTU_2K_E);
		   if(ret!=0) {
		 	nam_syslog_err("cpssDxChPortTxWrrGlobalParamSet fail,ret %d\n",ret);
		   }
		 /* add PortTxQueueEnable */
		  ret=cpssDxChPortTxQueueEnableSet(devIdx, 1);
		   if(ret!=0) {
		 	nam_syslog_err("cpssDxChPortTxQueueEnableSet fail,ret %d\n",ret);
		   }

		  for(i=0;i<8;i++){
		  	ret=nam_qos_schedu_wrr_set(1,CPSS_PORT_TX_WRR_ARB_GROUP_0_E,i,i+1);
			if(ret!=0) {
				nam_syslog_err("nam_qos_schedu_wrr_set fail,ret %d\n",ret);
			}
		 }
	 }
#endif

	 return ret;
}

unsigned int nam_qos_queue_schedu_check(int wrrflag, int groupflag)
{
#ifdef DRV_LIB_BCM
	if ((2 == wrrflag) || (1 == groupflag)){
		return ACL_RETURN_CODE_BCM_DEVICE_NOT_SUPPORT;
	}
	else {
		return QOS_RETURN_CODE_SUCCESS;
	}
#endif

#ifdef DRV_LIB_CPSS
	return QOS_RETURN_CODE_SUCCESS;
#endif
}

unsigned int nam_qos_queue_sp_set()
{
 	unsigned int		i = 0, ret=QOS_RETURN_CODE_SUCCESS , devNum=0, appDemoDevAmount=0;	
	appDemoDevAmount = nam_asic_get_instance_num();	
#ifdef DRV_LIB_BCM	
	int 	weights[BCM_COS_COUNT];
	nam_syslog_dbg("nam_qos_queue_sp_set for bcm \n");
	
	memset(weights, 0, BCM_COS_COUNT * sizeof(int));
	for (devNum = 0; devNum < appDemoDevAmount; devNum++) {
		ret = bcm_cosq_sched_set(devNum, BCM_COSQ_STRICT, weights, 0);
		nam_syslog_dbg("bcm_cosq_sched_set ret is %d,  set BCM_COSQ_STRICT \n", ret);
		if (QOS_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_err("ERROR ! bcm_cosq_sched_set ret %d\n",ret);	
		}
	}
#endif

#ifdef DRV_LIB_CPSS
	unsigned int								tcQueue=0;
	CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT		profileSet;
	CPSS_PORT_TX_Q_ARB_GROUP_ENT  				arbGroup;

	/*since no 10G port on device(5000,7000),so profile is fixed.*/
	/*if update ,need api to indentity 10G,G ports*/
	profileSet = PRV_CPSS_DXCH_NET_GE_PORT_PROFILE;
	arbGroup = CPSS_PORT_TX_SP_ARB_GROUP_E;

	for(devNum=0;devNum<appDemoDevAmount;devNum++){
		for(tcQueue=0;tcQueue<8;tcQueue++)
		{
			ret=cpssDxChPortTxQArbGroupSet(devNum, tcQueue, arbGroup, profileSet);
			if(ret!=QOS_RETURN_CODE_SUCCESS){
				nam_syslog_err("cpssDxChPortTxQArbGroupSet error!ret %d\n",ret);	
				break;
			}
			/*cpu port profile not need*/
/*			ret=cpssDxChPortTxQArbGroupSet(devNum, tcQueue, arbGroup, PRV_CPSS_DXCH_CPU_PORT_PROFILE);
			if(ret!=QOS_RETURN_CODE_SUCCESS){
				nam_syslog_err(("cpssDxChPortTxQArbGroupSet error!ret %d\n",ret));	
				break;
			}*/
			
		}
	}
#endif

	return ret;
}

unsigned int nam_qos_queue_sp_set_one
(
	unsigned int  tcQueue
)
{
	unsigned int		i = 0, ret=QOS_RETURN_CODE_SUCCESS;
	unsigned int		appDemoDevAmount=0,devNum=0;
	appDemoDevAmount = nam_asic_get_instance_num(); 

#ifdef DRV_LIB_BCM
/*
	int 	weights[BCM_COS_COUNT];
	
	memset(weights, 0, BCM_COS_COUNT * sizeof(int));
	for (i = 0; i< BCM_COS_COUNT, i++) {
		weights[i] = g_hybrid[i].weight;
	}
	for (devNum = 0; devNum < appDemoDevAmount; devNum++) {
		ret = bcm_cosq_sched_set(devNum, BCM_COSQ_STRICT, weights, 0);
		if(ret != QOS_RETURN_CODE_SUCCESS){
			nam_syslog_dbg(("ERROR ! bcm_cosq_sched_set ret %d\n",ret));	
			break;
		}
	}
	*/
	ret = ACL_RETURN_CODE_BCM_DEVICE_NOT_SUPPORT;
#endif

 #ifdef DRV_LIB_CPSS		
	CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT		profileSet;
	CPSS_PORT_TX_Q_ARB_GROUP_ENT  				arbGroup;

	/*since no 10G port on device(5000,7000),so profile is fixed.*/
	/*if update ,need api to indentity 10G,G ports*/
	profileSet = PRV_CPSS_DXCH_NET_GE_PORT_PROFILE;
	arbGroup = CPSS_PORT_TX_SP_ARB_GROUP_E;

	for(devNum=0;devNum<appDemoDevAmount;devNum++){
		ret=cpssDxChPortTxQArbGroupSet(devNum, tcQueue, arbGroup, profileSet);
		if(ret!=QOS_RETURN_CODE_SUCCESS){
			nam_syslog_err("cpssDxChPortTxQArbGroupSet error!ret %d\n",ret);	
			break;
		}
		
		/*cpu port profile not need*/
/*			ret=cpssDxChPortTxQArbGroupSet(devNum, tcQueue, arbGroup, PRV_CPSS_DXCH_CPU_PORT_PROFILE);
		if(ret!=QOS_RETURN_CODE_SUCCESS){
			nam_syslog_err(("cpssDxChPortTxQArbGroupSet error!ret %d\n",ret));	
			break;
		}*/		
	}
#endif

	return ret;
}

unsigned int nam_qos_schedu_wrr_set
(	
	unsigned int  wrrflag,
	unsigned int  groupFlag,
	unsigned int  tcQueue,
	unsigned int  wrrWeight
)
{							
	unsigned int		i = 0, ret=QOS_RETURN_CODE_SUCCESS , devNum=0, appDemoDevAmount=0;	
	appDemoDevAmount = nam_asic_get_instance_num();
	
#ifdef DRV_LIB_BCM
	nam_syslog_dbg("nam_qos_schedu_wrr_set for bcm \n");
	int		        weights[BCM_COS_COUNT];
	if (2 == wrrflag) {
		return ACL_RETURN_CODE_BCM_DEVICE_NOT_SUPPORT;
	}
	memset(weights, 0, BCM_COS_COUNT * sizeof(int));
	for (i = 0; i < MAX_UP_PROFILE_NUM; i++) {
		weights[i] = g_queue[i].weight;
		nam_syslog_err("nam_qos_schedu_wrr_set weight[%d] is %d \n", i, weights[i]);
	}
	
	for (devNum=0; devNum < appDemoDevAmount; devNum++) {
		ret = bcm_cosq_sched_set(devNum, BCM_COSQ_WEIGHTED_ROUND_ROBIN, weights, 0);
		nam_syslog_dbg("bcm_cosq_sched_set ret is %d  BCM_COSQ_WEIGHTED_ROUND_ROBIN\n", ret);
		if (QOS_RETURN_CODE_SUCCESS != ret) {
			nam_syslog_err("ERROR ! bcm_cosq_sched_set ret %d\n",ret);
			break;
		}
	}
#endif

#ifdef DRV_LIB_CPSS
	unsigned int	tmp = 0;
	CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT		profileSet;
	CPSS_PORT_TX_Q_ARB_GROUP_ENT				arbGroup;
	 
	/*since no 10G port on device(5000,7000),so profile is fixed.*/
	/*if update ,need api to indentity 10G,G ports*/

	profileSet = PRV_CPSS_DXCH_NET_GE_PORT_PROFILE;
	arbGroup = (CPSS_PORT_TX_Q_ARB_GROUP_ENT)groupFlag;

	/*wrr+sp,firstly clear all bits....wrr not need,keep group1*/
	
	for(devNum=0;devNum<appDemoDevAmount;devNum++){
/*		if(wrrflag==2){
			for(i=0;i<8;i++){
				ret=cpssDxChPortTxQArbGroupSet(devNum,i,CPSS_PORT_TX_SP_ARB_GROUP_E,profileSet);				
				if(ret!=QOS_RETURN_CODE_SUCCESS)
					nam_syslog_err(("cpssDxChPortTxQArbGroupSet clear error!ret %d\n",tmp));
				
				//cpu port profile clear not need
				ret=cpssDxChPortTxQArbGroupSet(devNum,i,CPSS_PORT_TX_SP_ARB_GROUP_E,PRV_CPSS_DXCH_CPU_PORT_PROFILE);				
				if(ret!=QOS_RETURN_CODE_SUCCESS)
					nam_syslog_err(("cpssDxChPortTxQArbGroupSet clear error!ret %d\n",tmp));
			
			}
		}*/ 
		tmp=cpssDxChPortTxQArbGroupSet(devNum, tcQueue, arbGroup, profileSet);
		if(tmp!=QOS_RETURN_CODE_SUCCESS){
			nam_syslog_err("cpssDxChPortTxQArbGroupSet error!ret %d\n",tmp);		
		}
		else{
			ret= cpssDxChPortTxQWrrProfileSet(devNum, tcQueue, wrrWeight,profileSet);
			 if(ret!=QOS_RETURN_CODE_SUCCESS){
				nam_syslog_err("cpssDxChPortTxQWrrProfileSet error!ret %d\n",ret);
			}
		}
		
		
/*		not need*/
/*		tmp=cpssDxChPortTxQArbGroupSet(devNum, tcQueue, arbGroup, PRV_CPSS_DXCH_CPU_PORT_PROFILE);
		if(tmp!=QOS_RETURN_CODE_SUCCESS){
			nam_syslog_err(("cpssDxChPortTxQArbGroupSet error!ret %d\n",tmp));		
		}
		else{
			ret= cpssDxChPortTxQWrrProfileSet(devNum, tcQueue, wrrWeight,PRV_CPSS_DXCH_CPU_PORT_PROFILE);
			 if(ret!=QOS_RETURN_CODE_SUCCESS){
				nam_syslog_err(("cpssDxChPortTxQWrrProfileSet error!ret %d\n",ret));
			}
		}*/
		
	  }
#endif

	return ret;
}

unsigned int nam_qos_traffic_shape_port
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned char mode,
	unsigned int  burstSize,
	unsigned long maxRatePtr
)
{
	unsigned int ret=QOS_RETURN_CODE_SUCCESS;
	
#ifdef DRV_LIB_BCM	
/*
	if (INGRESS_TRAFFIC_SHAPE == mode) {
		bcm_port_rate_ingress_set(devNum, portNum, maxRatePtr, burstSize);
	}
*/
	ret = bcm_port_rate_egress_set(devNum, portNum, maxRatePtr, burstSize);
	nam_syslog_dbg("bcm_port_rate_egress_set devnum %d, portnum %d, sec %u ,burstt %u\n",
		devNum, portNum, maxRatePtr, burstSize);
	if (QOS_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_err("ERROR ! bcm_port_rate_egress_set ret %d\n",ret);	
	}
#endif

#ifdef DRV_LIB_CPSS
	ret=cpssDxChPortTxShaperEnableSet(devNum, portNum, mode);
	if(ret!=0) {
		nam_syslog_err("cpssDxChPortTxShaperEnableSet enable fail!ret %d\n",ret);
	}
	ret=cpssDxChPortTxShaperProfileSet(devNum, portNum, (unsigned short)burstSize,&maxRatePtr);
	if(ret!=0) {
		nam_syslog_err("cpssDxChPortTxShaperProfileSet error! ret %d\n",ret);
	}
	nam_syslog_dbg("maxRatePtr %ld\n",maxRatePtr);
#endif	

	return ret;
}

unsigned int nam_qos_del_traffic_shape_port
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned char mode
)
{
	unsigned int ret=QOS_RETURN_CODE_SUCCESS;
#ifdef DRV_LIB_BCM	
/*
	if (INGRESS_TRAFFIC_SHAPE == mode) {
		bcm_port_rate_ingress_set(devNum, portNum, 0, 0);
	}
*/
	ret = bcm_port_rate_egress_set(devNum, portNum, 0, 0);
	nam_syslog_dbg("bcm_port_rate_egress_set devnum %d, portnum %d, sec 0 ,burstt 0\n",devNum, portNum);	
	if (QOS_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_err("ERROR ! bcm_port_rate_egress_set ret %d\n",ret);	
	}
#endif

#ifdef DRV_LIB_CPSS
	ret=cpssDxChPortTxShaperEnableSet(devNum, portNum, mode);
	if(ret!=0) {
		nam_syslog_err("cpssDxChPortTxShaperEnableSet enable fail!ret %d\n",ret);
	}
#endif

	return ret;
}

unsigned int nam_qos_traffic_shape_queue_port
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned char mode,
	unsigned int  tcQueue,
	unsigned int  burstSize,
	unsigned long userRatePtr
)
{
	unsigned int ret=QOS_RETURN_CODE_SUCCESS;
#ifdef DRV_LIB_BCM		
	ret = bcm_cosq_port_bandwidth_set(devNum, portNum, tcQueue,  userRatePtr, userRatePtr, 0);
	nam_syslog_dbg("bcm_cosq_port_bandwidth_set devnum %d, portnum %d, tcQueue  %d, userRatePtr %d\n",
		devNum, portNum, tcQueue, userRatePtr);
	if (QOS_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_err("ERROR ! bcm_cosq_port_bandwidth_set ret %d\n",ret);	
	}
#endif
	
#ifdef DRV_LIB_CPSS
	ret=cpssDxChPortTxQShaperEnableSet(devNum, portNum, tcQueue, mode);
	if(ret!=0) {
		nam_syslog_err("cpssDxChPortTxQShaperEnableSet enable fail!ret %d\n",ret);
	}
	ret=cpssDxChPortTxQShaperProfileSet(devNum, portNum, tcQueue,
													  (unsigned short)burstSize, &userRatePtr); 
	if(ret!=0) {
		nam_syslog_err("cpssDxChPortTxQShaperProfileSet error! ret %d\n", ret);
	}
	nam_syslog_dbg("userRatePtr %ld\n",userRatePtr);
#endif

	return ret;
}

unsigned int nam_qos_del_traffic_shape_queue_port
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned char mode,
	unsigned int  tcQueue
)
{
	unsigned int ret=QOS_RETURN_CODE_SUCCESS;
#ifdef DRV_LIB_BCM
	ret = bcm_cosq_port_bandwidth_set(devNum, portNum, tcQueue,  0, 0, 0);
	nam_syslog_dbg("bcm_cosq_port_bandwidth_set devnum %d, portnum %d, tcQueue  %d, userRatePtr 0\n",
		devNum, portNum, tcQueue);
	if (QOS_RETURN_CODE_SUCCESS != ret) {
		nam_syslog_dbg("ERROR ! bcm_cosq_port_bandwidth_set ret %d\n",ret);
	}
#endif

#ifdef DRV_LIB_CPSS
	ret = cpssDxChPortTxQShaperEnableSet(devNum, portNum, tcQueue, mode);
	if(ret!=0) {
		nam_syslog_dbg("cpssDxChPortTxQShaperEnableSet enable fail!ret %d\n",ret);
	}
#endif

	return ret;
}

#ifdef __cplusplus
}
#endif

