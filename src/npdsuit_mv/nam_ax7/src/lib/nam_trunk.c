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
* nam_trunk.c
*
*
* CREATOR:
*		qinhs@autelan.com
*
* DESCRIPTION:
*		APIs used in NAM by TRUNK module.
*
* DATE:
*		02/21/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.25 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <string.h>
#include "util/npd_list.h"
#include "sysdef/npd_sysdef.h"
#include "npd/nam/npd_amapi.h"
#include "nam_asic.h"
#include "nam_eth.h"
#include "nam_trunk.h"
#include "nam_log.h"
#include "sysdef/returncode.h"

#ifdef DRV_LIB_BCM
#include <bcm/types.h>
#include <soc/types.h>
#include <bcm/trunk.h>
#endif

extern unsigned long nam_get_distributed_devnum(unsigned char asic_num,unsigned int *devNum);
extern enum product_id_e npd_query_product_id
(
	void
);

unsigned int nam_asic_trunk_entry_active
(
	unsigned short trunkId
)
{
	unsigned long	ret = 0;
	
#ifdef DRV_LIB_CPSS
	unsigned char	devNum =0;
	unsigned int	numOfEnabledMembers = 0;
	unsigned int numOfDisabledMembers = 0;
	
	ret = cpssDxChTrunkMembersSet(devNum, trunkId, numOfEnabledMembers, NULL, numOfDisabledMembers, NULL);
	if (0 == ret) {
		ret = TRUNK_CONFIG_SUCCESS;
	}
	else { ret = TRUNK_CONFIG_FAIL;}
	 syslog_ax_nam_trunk_dbg("dev %d, trunkId %d,return Value %d\n",devNum,trunkId,ret);	
#endif

#ifdef DRV_LIB_BCM		
	unsigned long   numOfPp = 0;
	unsigned int    i = 0;	

	numOfPp = nam_asic_get_instance_num();
	for (i=0; i<numOfPp; i++) {	
		ret = bcm_trunk_create_id(i, trunkId);
		if (0 == ret) {
			ret =TRUNK_CONFIG_SUCCESS;
		}
		else { 
			ret = TRUNK_CONFIG_FAIL;
		}
	}
#endif
	return ret;
}

unsigned int nam_asic_trunk_ports_add
(
	unsigned char devNum,
	unsigned short trunkId,
	unsigned char portNum,
	unsigned char enDis
)
{
	unsigned long ret = 0;
	unsigned int devnum;
	
#ifdef DRV_LIB_CPSS
	CPSS_TRUNK_MEMBER_STC memberPtr;
	memset(&memberPtr, 0, sizeof(CPSS_TRUNK_MEMBER_STC));	
	if(IS_DISTRIBUTED == SYSTEM_TYPE)
	{
		
		ret = nam_get_distributed_devnum(devNum,&devnum);
		if(ret != TRUNK_CONFIG_SUCCESS)
		{
			syslog_ax_nam_trunk_err("get distributed dev num failure !!! \n");
		}

		memberPtr.devNum = devnum;

	}
	else
	{
		memberPtr.devNum = devNum;	
	}
	memberPtr.portNum = portNum;
	 syslog_ax_nam_trunk_dbg("add asic %d dev %d port %d to trunk %d\n",devNum,memberPtr.devNum,memberPtr.portNum,trunkId);

	ret = cpssDxChTrunkMemberAdd(devNum, trunkId, &memberPtr);
	
	if (0 == ret) {
		ret = TRUNK_CONFIG_SUCCESS;
	}
	else if (0x1b == ret ){		/*0x1B indicates port was Already member of this Trunk in Galtis TrunkDB */
		ret = TRUNK_PORT_EXISTS_GTDB;
		return ret;
	}
	else if (0x14 == ret) {		/*0X14 indicates trunk is full of port mbrs.*/
		ret = TRUNK_PORT_MBRS_FULL;	
		return ret;		
	}
	else { 
		ret = TRUNK_CONFIG_FAIL;
		return ret;		
	}
	/*add by qily@autelan.com 08 11 20*/		
	if(enDis){		 
		ret = cpssDxChTrunkMemberEnable(devNum, trunkId, &memberPtr);
	}
	else
		ret = cpssDxChTrunkMemberDisable(devNum,trunkId, &memberPtr);
#endif

#ifdef DRV_LIB_BCM
	bcm_trunk_add_info_t get1_info,get2_info,add_info,getdev0_info,getdev1_info;
	bcm_pbmp_t pbmp,arg_pbmp,get1_pbmp,get2_pbmp;
	int i = 0,j = 0,num_ports = 0;

	memset(&add_info,0,sizeof(bcm_trunk_add_info_t));
	memset(&get1_info,0,sizeof(bcm_trunk_add_info_t));
	memset(&get2_info,0,sizeof(bcm_trunk_add_info_t));
	SOC_PBMP_CLEAR(pbmp);
	SOC_PBMP_CLEAR(arg_pbmp);
	SOC_PBMP_CLEAR(get1_pbmp);
	SOC_PBMP_CLEAR(get2_pbmp);	
	
	ret = bcm_trunk_get(devNum,trunkId,&get1_info);
	syslog_ax_nam_trunk_dbg("flag : %d;num_ports : %d,psc : %d,dlf_index : %d,  \
							mc_index : %d,ipmc_index : %d,tp[0] : %d,tp[1] : %d,tm[0] : %d\n",
							get1_info.flags,get1_info.num_ports,get1_info.psc, \
							get1_info.dlf_index,get1_info.mc_index,get1_info.ipmc_index,  \
							get1_info.tp[0],get1_info.tp[1],get1_info.tm[0],get2_info.tm[1]);

	SOC_PBMP_PORT_ADD(arg_pbmp, portNum);

	for (i = 0; i < get1_info.num_ports; i++) {
		SOC_PBMP_PORT_ADD(pbmp, get1_info.tp[i]);
	}
	SOC_PBMP_OR(pbmp, arg_pbmp);

	j = 0;
	SOC_PBMP_ITER(pbmp, i) {
		add_info.tm[j] = devNum;
		add_info.tp[j++] = i;
	};
	bcm_trunk_get(0,trunkId,&getdev0_info);
	bcm_trunk_get(1,trunkId,&getdev1_info);
	add_info.num_ports = j;
	add_info.psc = get1_info.psc;
	add_info.dlf_index = -1;
	add_info.mc_index = -1;
	add_info.ipmc_index = -1;	
	num_ports = getdev0_info.num_ports+getdev1_info.num_ports;
	syslog_ax_nam_trunk_dbg("trunk %d current port num %d\n",trunkId,num_ports);	
	if(TRUNK_MEMBER_NUM_MAX == num_ports) {
		return TRUNK_PORT_MBRS_FULL;
	}	
	ret = bcm_trunk_set(devNum,trunkId,&add_info);	
	if (0 == ret) {
		ret = TRUNK_CONFIG_SUCCESS;
	}
	else { 
		syslog_ax_nam_trunk_err("trunk %d set port(%d,%d) error\n",trunkId,devNum,portNum);
		ret = TRUNK_CONFIG_FAIL;
	}
	ret = bcm_trunk_get(devNum,trunkId,&get2_info);
	syslog_ax_nam_trunk_dbg("flag : %d;num_ports : %d,psc : %d,dlf_index : %d,  \
							mc_index : %d,ipmc_index : %d,tp[0] : %d,tp[1] : %d,tm[0] : %d\n",
							get2_info.flags,get2_info.num_ports,get2_info.psc, \
							get2_info.dlf_index,get2_info.mc_index,get2_info.ipmc_index,  \
							get2_info.tp[0],get2_info.tp[1],get2_info.tm[0],get2_info.tm[1]);

#endif
	return ret;
}

unsigned int nam_asic_trunk_map_table_update
(
	unsigned char devNum,
	unsigned short trunkId,
	unsigned char actdevNum,
	unsigned char portNum,
	unsigned char isAdd,
	unsigned char enDis
)
{
	unsigned long ret = 0;
	unsigned int devnum;
	
#ifdef DRV_LIB_CPSS
	CPSS_TRUNK_MEMBER_STC memberPtr;
	memset(&memberPtr, 0, sizeof(CPSS_TRUNK_MEMBER_STC));	
	memberPtr.devNum = actdevNum;	
	memberPtr.portNum = portNum;
	syslog_ax_nam_trunk_dbg("%s asic %d dev %d port %d to trunk %d\n",isAdd?"Add":"Del",devNum,memberPtr.devNum,memberPtr.portNum,trunkId);
    if(isAdd == NAM_TRUE)
    {
		ret = cpssDxChTrunkMemberAdd(devNum, trunkId, &memberPtr);
		
		if (0 == ret) {
			ret = TRUNK_CONFIG_SUCCESS;
		}
		else if (0x1b == ret ){		/*0x1B indicates port was Already member of this Trunk in Galtis TrunkDB */
			ret = TRUNK_PORT_EXISTS_GTDB;
			return ret;
		}
		else if (0x14 == ret) {		/*0X14 indicates trunk is full of port mbrs.*/
			ret = TRUNK_PORT_MBRS_FULL;	
			return ret;		
		}
		else { 
			ret = TRUNK_CONFIG_FAIL;
			return ret;		
		}
		/*add by qily@autelan.com 08 11 20*/		
		if(enDis)
		{		 
			ret = cpssDxChTrunkMemberEnable(devNum, trunkId, &memberPtr);
		}
		else
		{
			ret = cpssDxChTrunkMemberDisable(devNum,trunkId, &memberPtr);
		}
    }
	else
	{
		ret = cpssDxChTrunkMemberRemove(devNum, trunkId, &memberPtr);
		if (0 == ret) 
		{
			ret = TRUNK_CONFIG_SUCCESS;
		}
		else 
		{ 
			syslog_ax_nam_trunk_err("nam asic trunk ports delete failed, ret %#x\n", ret);
			ret = TRUNK_CONFIG_FAIL;
		}
	}
#endif
	return ret;
}


unsigned int nam_asic_trunk_ports_del
(	
	unsigned char devNum,
	unsigned short trunkId,
	unsigned char portNum
)
{
	unsigned long ret = 0;
	
#ifdef DRV_LIB_CPSS 
	CPSS_TRUNK_MEMBER_STC memberPtr;
	unsigned int devnum = 0;

	if(IS_DISTRIBUTED == SYSTEM_TYPE)
	{
		
		ret = nam_get_distributed_devnum(devNum,&devnum);
		if(ret != TRUNK_CONFIG_SUCCESS)
		{
			syslog_ax_nam_trunk_err("get distributed dev num failure !!! \n");
		}

		memberPtr.devNum = devnum;

	}
	else
	{
		memberPtr.devNum = devNum;	
	}
	memberPtr.portNum = portNum;
	ret = cpssDxChTrunkMemberRemove(devNum, trunkId, &memberPtr);
	if (0 == ret) {
		ret = TRUNK_CONFIG_SUCCESS;
	}
	else { 
		syslog_ax_nam_trunk_err("nam asic trunk ports delete failed, ret %#x\n", ret);
		ret = TRUNK_CONFIG_FAIL;
	}
#endif

#ifdef DRV_LIB_BCM
	bcm_trunk_add_info_t add_info;
	bcm_pbmp_t pbmp,arg_pbmp;

	int i = 0,j = 0;

	memset(&add_info,0,sizeof(bcm_trunk_add_info_t));

	ret = bcm_trunk_get(devNum,trunkId,&add_info);
	syslog_ax_nam_trunk_dbg("flag : %d;num_ports : %d,psc : %d,dlf_index : %d,  \
							mc_index : %d,ipmc_index : %d,tp[0] : %d,tp[1] : %d,tm[0] : %d\n",
							add_info.flags,add_info.num_ports,add_info.psc, \
							add_info.dlf_index,add_info.mc_index,add_info.ipmc_index,  \
							add_info.tp[0],add_info.tp[1],add_info.tm[0]);

	SOC_PBMP_CLEAR(pbmp);
	SOC_PBMP_CLEAR(arg_pbmp);
	SOC_PBMP_PORT_ADD(arg_pbmp, portNum);
	for (i = 0; i < add_info.num_ports; i++) {
		SOC_PBMP_PORT_ADD(pbmp, add_info.tp[i]);
	}
	SOC_PBMP_REMOVE(pbmp, arg_pbmp);
	
    j = 0;
    SOC_PBMP_ITER(pbmp, i) {
		add_info.tm[j] = 0;
		add_info.tp[j++] = i;
    }
    add_info.num_ports = j;	

	/*add_info.tp[--add_info.num_ports] = 0; 	*/
	ret = bcm_trunk_set(devNum,trunkId,&add_info);
	if (0 == ret) {
		ret = TRUNK_CONFIG_SUCCESS;
	}
	else { 
		syslog_ax_nam_trunk_err("trunk %d set port(%d,%d) error\n",trunkId,devNum,portNum);
		ret = TRUNK_CONFIG_FAIL;
	}

	syslog_ax_nam_trunk_dbg("flag : %d;num_ports : %d,psc : %d,dlf_index : %d,  \
							mc_index : %d,ipmc_index : %d,tp[0] : %d,tp[1] : %d,tm[0] : %d\n",
							add_info.flags,add_info.num_ports,add_info.psc, \
							add_info.dlf_index,add_info.mc_index,add_info.ipmc_index,  \
							add_info.tp[0],add_info.tp[1],add_info.tm[0]);	
#endif
	return ret;
}

unsigned int nam_asic_trunk_port_endis_for_distributed(unsigned char asic_num,unsigned char devNum,unsigned char portNum,unsigned short trunkId,unsigned char endis)
{
	
	unsigned long ret = 0;
		
#ifdef DRV_LIB_CPSS
	CPSS_TRUNK_MEMBER_STC memberPtr;

	memberPtr.devNum = devNum;
	memberPtr.portNum = portNum;
	syslog_ax_nam_trunk_dbg("%s asic %d dev %d port %d to trunk %d\n",(endis)?"En":"Dis", \
																asic_num,\
																memberPtr.devNum, \
																memberPtr.portNum,trunkId);
	if(endis){
		ret = cpssDxChTrunkMemberEnable(asic_num, trunkId, &memberPtr);
	}
	else
		ret = cpssDxChTrunkMemberDisable(asic_num,trunkId, &memberPtr);
#endif
	return ret;
}

unsigned int nam_asic_trunk_port_endis
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned short trunkId,
	unsigned char enDis
)
{
	unsigned long ret = 0;
	unsigned int devnum;/*used for modified devNum*/
	
#ifdef DRV_LIB_CPSS
	CPSS_TRUNK_MEMBER_STC memberPtr;

	if(IS_DISTRIBUTED == SYSTEM_TYPE)
	{
		
		ret = nam_get_distributed_devnum(devNum,&devnum);
		if(ret != TRUNK_CONFIG_SUCCESS)
		{
			syslog_ax_nam_trunk_err("get distributed dev num failure !!! \n");
		}

		memberPtr.devNum = devnum;

	}
	else
	{
		memberPtr.devNum = devNum;	
	}
	memberPtr.portNum = portNum;
	 syslog_ax_nam_trunk_dbg("%s asic %d dev %d port %d to trunk %d\n",(enDis)?"En":"Dis", \
	 															devNum,\
	 															memberPtr.devNum, \
	 															memberPtr.portNum,trunkId);
	if(enDis){
		ret = cpssDxChTrunkMemberEnable(devNum, trunkId, &memberPtr);
	}
	else
		ret = cpssDxChTrunkMemberDisable(devNum,trunkId, &memberPtr);
#endif

#ifdef DRV_LIB_BCM
	if(enDis) {
		ret = nam_asic_trunk_ports_add(devNum,trunkId,portNum,enDis);
		if(0 != ret) {
			syslog_ax_nam_trunk_err("port(%d,%d) add to trunk %d error,ret %d",devNum,portNum,trunkId,ret);
			ret = TRUNK_CONFIG_FAIL;
		}
	}
	else {
		ret = nam_asic_trunk_ports_del(devNum,trunkId,portNum);
		if(0 != ret) {
			syslog_ax_nam_trunk_err("port(%d,%d) delete from trunk %d error,ret %d",devNum,portNum,trunkId,ret);
			ret = TRUNK_CONFIG_FAIL;
		}
	}
	#if 0
	int tid = 0,ptrunkId = 0,psc = 0;
	bcm_trunk_add_info_t get_info;
	
	memset(&get_info,0,sizeof(bcm_trunk_add_info_t));

	ret = bcm_trunk_get(devNum,trunkId,&get_info);
	syslog_ax_nam_trunk_dbg("flag : %d;num_ports : %d,psc : %d,dlf_index : %d,  \
							mc_index : %d,ipmc_index : %d,tp[0] : %d,tp[1] : %d,tm[0] : %d\n",
							get_info.flags,get_info.num_ports,get_info.psc, \
							get_info.dlf_index,get_info.mc_index,get_info.ipmc_index,  \
							get_info.tp[0],get_info.tp[1],get_info.tm[0]);
	syslog_ax_nam_trunk_dbg("port(%d,%d) trunkId : %d psc : %d.\n",devNum,portNum,trunkId,psc);

	if(enDis) {
		ptrunkId = trunkId;
		psc = get_info.psc;
	}
	else {
		ptrunkId = -1;
	}
	syslog_ax_nam_trunk_dbg("port(%d,%d) trunkId : %d psc : %d.\n",devNum,portNum,ptrunkId,psc);
	
	ret = bcm_esw_port_tgid_set(devNum,portNum,ptrunkId,psc);
	if (0 == ret) {
		ret = TRUNK_CONFIG_SUCCESS;
	}
	else { 
		syslog_ax_nam_trunk_dbg("set port trunk id error.\n");
		ret = TRUNK_CONFIG_FAIL;
	}
	ret = bcm_trunk_get(devNum,trunkId,&get_info);
	syslog_ax_nam_trunk_dbg("flag : %d;num_ports : %d,psc : %d,dlf_index : %d,  \
							mc_index : %d,ipmc_index : %d,tp[0] : %d,tp[1] : %d,tm[0] : %d\n",
							get_info.flags,get_info.num_ports,get_info.psc, \
							get_info.dlf_index,get_info.mc_index,get_info.ipmc_index,  \
							get_info.tp[0],get_info.tp[1],get_info.tm[0]);	
	syslog_ax_nam_trunk_dbg("port(%d,%d) trunkId : %d psc : %d.\n",devNum,portNum,trunkId,psc);
	bcm_esw_port_tgid_get(devNum,portNum,&tid,&psc);
	syslog_ax_nam_trunk_dbg("port(%d,%d) trunkId : %d psc : %d.\n",devNum,portNum,tid,psc);
	#endif
#endif
	return ret;
}

unsigned int nam_asic_trunk_delete
(
	unsigned short trunkId
)
{
	unsigned long	ret = TRUNK_CONFIG_SUCCESS;

#ifdef DRV_LIB_CPSS
	unsigned char	devNum = 0;
	unsigned int	numOfEnabledMembers = 0;
	unsigned int 	numOfDisabledMembers = 0;
	
	ret = cpssDxChTrunkMembersSet(devNum, trunkId, numOfEnabledMembers, NULL, numOfDisabledMembers, NULL);
	if (0 == ret) {
		ret =TRUNK_CONFIG_SUCCESS;
	}
	else { ret = TRUNK_CONFIG_FAIL;}
#endif

#ifdef DRV_LIB_BCM
	unsigned long   numOfPp = 0;
	unsigned int    i = 0;
	
	numOfPp = nam_asic_get_instance_num();
	for (i=0; i<numOfPp; i++) {	
		ret = bcm_trunk_destroy(i, trunkId);
		if (0 == ret) {
			ret =TRUNK_CONFIG_SUCCESS;
		}
		else { 
			ret = TRUNK_CONFIG_FAIL;
		}
	}
#endif
	return ret;
}

unsigned int nam_asic_trunk_load_balanc_set
(
	unsigned char  devNum,
	unsigned short trunkId,
	unsigned int  lbMode
)
{
	unsigned long ret = TRUNK_CONFIG_SUCCESS;

#ifdef DRV_LIB_CPSS
	switch(lbMode){
		case LOAD_BANLC_SOURCE_DEV_PORT:
			ret = cpssDxChTrunkHashGlobalModeSet(devNum, CPSS_DXCH_TRUNK_LBH_INGRESS_PORT_E);
			if(TRUNK_CONFIG_SUCCESS != ret)
			{
				syslog_ax_nam_trunk_err("set trunk hash mode to ingress port failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			break;
		case LOAD_BANLC_SRC_DEST_MAC:
			ret = cpssDxChTrunkHashGlobalModeSet(devNum, CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_E);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk hash based on packets failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			ret = cpssDxChTrunkHashIpAddMacModeSet(devNum, 1);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk hash mode based mac failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			ret =cpssDxChTrunkHashIpModeSet(devNum, 0);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk disabled ip hash mode failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			ret = cpssDxChTrunkHashL4ModeSet(devNum,CPSS_DXCH_TRUNK_L4_LBH_DISABLED_E);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk disabled L4 hash mode failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			break;
		case LOAD_BANLC_SRC_DEST_IP:
			ret = cpssDxChTrunkHashGlobalModeSet(devNum, CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_E);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk hash based on packets failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			ret =cpssDxChTrunkHashIpModeSet(devNum, 1);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk hash based ip hash mode failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			ret = cpssDxChTrunkHashIpAddMacModeSet(devNum, 0);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk disabled mac hash mode failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			ret = cpssDxChTrunkHashL4ModeSet(devNum,CPSS_DXCH_TRUNK_L4_LBH_DISABLED_E);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk disabled L4 hash mode failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			break;
		case LOAD_BANLC_TCP_UDP_RC_DEST_PORT:
			ret = cpssDxChTrunkHashGlobalModeSet(devNum, CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_E);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk hash based on packets failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			ret = cpssDxChTrunkHashL4ModeSet(devNum,CPSS_DXCH_TRUNK_L4_LBH_LONG_E);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk based L4 hash mode failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			ret = cpssDxChTrunkHashIpAddMacModeSet(devNum, 0);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk disabled mac hash mode failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			ret =cpssDxChTrunkHashIpModeSet(devNum, 0);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk disabled ip hash mode failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			break;
		case LOAD_BANLC_MAC_IP:
			ret = cpssDxChTrunkHashGlobalModeSet(devNum, CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_E);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk hash based on packets failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			ret = cpssDxChTrunkHashIpAddMacModeSet(devNum, 1);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk hash mode based mac failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			ret =cpssDxChTrunkHashIpModeSet(devNum, 1);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk hash based ip hash mode failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			ret = cpssDxChTrunkHashL4ModeSet(devNum,CPSS_DXCH_TRUNK_L4_LBH_DISABLED_E);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk disabled L4 hash mode failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			break;
		case LOAD_BANLC_MAC_L4:
			ret = cpssDxChTrunkHashGlobalModeSet(devNum, CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_E);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk hash based on packets failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			ret = cpssDxChTrunkHashIpAddMacModeSet(devNum, 1);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk hash mode based mac failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			ret = cpssDxChTrunkHashL4ModeSet(devNum,CPSS_DXCH_TRUNK_L4_LBH_LONG_E);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk based L4 hash mode failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			ret =cpssDxChTrunkHashIpModeSet(devNum, 0);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk disabled ip hash mode failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			break;
		case LOAD_BANLC_IP_L4:
			ret = cpssDxChTrunkHashGlobalModeSet(devNum, CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_E);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk hash based on packets failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			ret =cpssDxChTrunkHashIpModeSet(devNum, 1);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk hash based ip hash mode failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			ret = cpssDxChTrunkHashL4ModeSet(devNum,CPSS_DXCH_TRUNK_L4_LBH_LONG_E);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk based L4 hash mode failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			ret = cpssDxChTrunkHashIpAddMacModeSet(devNum, 0);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk disabled mac hash mode failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			break;
		case LOAD_BANLC_MAC_IP_L4:
			ret = cpssDxChTrunkHashGlobalModeSet(devNum, CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_E);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk hash based on packets failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			ret = cpssDxChTrunkHashIpAddMacModeSet(devNum, 1);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk hash mode based mac failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			ret =cpssDxChTrunkHashIpModeSet(devNum, 1);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk hash based ip hash mode failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			ret = cpssDxChTrunkHashL4ModeSet(devNum,CPSS_DXCH_TRUNK_L4_LBH_LONG_E);
			if(ret != TRUNK_CONFIG_SUCCESS)
			{
				syslog_ax_nam_trunk_err("set trunk based L4 hash mode failed\n");
				return TRUNK_CONFIG_FAIL;
			}
			break;

	}
#endif

#ifdef DRV_LIB_BCM
	unsigned long   numOfPp = 0;
	unsigned int    i = 0;	

	numOfPp = nam_asic_get_instance_num();
	for (i=0; i<numOfPp; i++) {	
		switch(lbMode){
			case LOAD_BANLC_SOURCE_DEV_PORT:
			case LOAD_BANLC_TCP_UDP_RC_DEST_PORT:	
			case LOAD_BANLC_MAC_IP:	
			case LOAD_BANLC_MAC_L4:
			case LOAD_BANLC_IP_L4:
			case LOAD_BANLC_MAC_IP_L4:	
				syslog_ax_nam_trunk_dbg("dev:%d,load balance mode :%d\n",i,lbMode);
				return TRUNK_RETURN_CODE_UNSUPPORT;
				break;
			case LOAD_BANLC_SRC_DEST_MAC:
				ret = bcm_trunk_psc_set(i,trunkId,BCM_RTAG_SRCDSTMAC);
				if(0 != ret) {
					return TRUNK_CONFIG_SUCCESS;
				}
				break;
			case LOAD_BANLC_SRC_DEST_IP:
				ret = bcm_trunk_psc_set(i,trunkId,BCM_RTAG_SRCDSTIP);
				if(0 != ret) {
					return TRUNK_CONFIG_SUCCESS;
				}	
				break;			
			default :
				return TRUNK_CONFIG_FAIL;
				break;
		}	
	}
#endif
	return TRUNK_CONFIG_SUCCESS;
}

unsigned int nam_asic_trunk_get_port_member
(
	unsigned short	trunkId,
	unsigned int* 	memberCont,
	unsigned int*	dev0MbrBmp,
	unsigned int*	dev1MbrBmp
)
{
	unsigned long ret = TRUNK_CONFIG_SUCCESS;
	
#ifdef DRV_LIB_CPSS
	unsigned char i,numOfPp = 0;
	unsigned int product_id = PRODUCT_ID_NONE;
	CPSS_PORTS_BMP_STC	nonTrunkPorts[2];
	CPSS_PORTS_BMP_STC  trunkPorts;
	struct DRV_VLAN_PORT_BMP_S mbrBmp;
	unsigned int mbrSlotPortBmp = 0;

	nonTrunkPorts[0].ports[0] = 0;
	nonTrunkPorts[0].ports[1] = 0;
	nonTrunkPorts[1].ports[0] = 0;
	nonTrunkPorts[1].ports[1] = 0;
	trunkPorts.ports[0] = 0;
	trunkPorts.ports[1] = 0;

	product_id = npd_query_product_id();
	numOfPp = nam_asic_get_instance_num();
	for (i = 0;i <numOfPp;i++) {
		/*check module type NOT done*/
		ret = trunkDbNonTrunkPortsCalc(i,trunkId,&nonTrunkPorts[i]);
	}
	trunkPorts.ports[0] = ~(nonTrunkPorts[0].ports[0]);
	trunkPorts.ports[0] &= 0x3ffffff;
	trunkPorts.ports[1] = nonTrunkPorts[0].ports[1];
	syslog_ax_nam_trunk_dbg("trunkPorts.ports[0] = %0x",trunkPorts.ports[0]);
	mbrBmp.ports[0] = trunkPorts.ports[0];
	mbrBmp.ports[1] = trunkPorts.ports[1];
	nam_asic_convert_mbrbmp_slotportbmp(mbrBmp,&mbrSlotPortBmp);
	*dev0MbrBmp = mbrSlotPortBmp;
	trunkPorts.ports[0] = ~(nonTrunkPorts[1].ports[0]);
	trunkPorts.ports[0] &= 0x3ffffff;
	trunkPorts.ports[1] = nonTrunkPorts[1].ports[1];
	syslog_ax_nam_trunk_dbg("trunkPorts.ports[0] = %0x",trunkPorts.ports[0]);
	mbrBmp.ports[0] = trunkPorts.ports[0];
	mbrBmp.ports[1] = trunkPorts.ports[1];
	nam_asic_convert_mbrbmp_slotportbmp(mbrBmp,&mbrSlotPortBmp);
	*dev1MbrBmp = mbrSlotPortBmp;
#endif
	return ret;
}

unsigned int nam_asic_trunk_get_port_member_bmp
(
	unsigned short	trunkId,
	unsigned int*	dev0MbrBmp,
	unsigned int*	dev1MbrBmp
)
{
	unsigned long ret = TRUNK_CONFIG_SUCCESS;

#ifdef DRV_LIB_CPSS
	unsigned char i,numOfPp = 0;
	CPSS_PORTS_BMP_STC	nonTrunkPorts[2];
	CPSS_PORTS_BMP_STC  trunkPorts;
	unsigned int tmpMbrBmp = 0;

	nonTrunkPorts[0].ports[0] = 0;
	nonTrunkPorts[0].ports[1] = 0;
	nonTrunkPorts[1].ports[0] = 0;
	nonTrunkPorts[1].ports[1] = 0;
	trunkPorts.ports[0] = 0;
	trunkPorts.ports[1] = 0;
	numOfPp = nam_asic_get_instance_num();
	for (i = 0;i <numOfPp;i++) {
		/*check module type NOT done*/
		ret = trunkDbNonTrunkPortsCalc(i,trunkId,&nonTrunkPorts[i]);
	}
	trunkPorts.ports[0] = ~(nonTrunkPorts[0].ports[0]);
	trunkPorts.ports[0] &= 0x3ffffff;
	trunkPorts.ports[1] = nonTrunkPorts[0].ports[1];
	 syslog_ax_nam_trunk_dbg("trunkPorts.ports[0] = %0x",trunkPorts.ports[0]);
	*dev0MbrBmp = trunkPorts.ports[0];

	trunkPorts.ports[0] = ~(nonTrunkPorts[1].ports[0]);
	trunkPorts.ports[0] &= 0x3ffffff;
	trunkPorts.ports[1] = nonTrunkPorts[1].ports[1];
	 syslog_ax_nam_trunk_dbg("trunkPorts.ports[0] = %0x",trunkPorts.ports[0]);
	*dev1MbrBmp = tmpMbrBmp;
#endif

#ifdef DRV_LIB_BCM
	bcm_trunk_add_info_t get0_info,get1_info;
	bcm_pbmp_t pbmp,arg_pbmp;
	unsigned int num_ports = 0,i = 0;

	memset(&get0_info,0,sizeof(bcm_trunk_add_info_t));
	memset(&get1_info,0,sizeof(bcm_trunk_add_info_t));

	SOC_PBMP_CLEAR(pbmp);
	SOC_PBMP_CLEAR(arg_pbmp);

	ret = bcm_trunk_get(0,trunkId,&get0_info);
	ret = bcm_trunk_get(1,trunkId,&get1_info);
	num_ports = get0_info.num_ports + get1_info.num_ports;
	for (i = 0; i < get0_info.num_ports; i++) {
		SOC_PBMP_PORT_ADD(pbmp, get0_info.tp[i]);
	}
	for (i = 0; i < get1_info.num_ports; i++) {
		SOC_PBMP_PORT_ADD(arg_pbmp, get1_info.tp[i]+32);
	}
	SOC_PBMP_OR(pbmp, arg_pbmp);

	*dev0MbrBmp = pbmp.pbits[0];
	*dev1MbrBmp = pbmp.pbits[1];	
	syslog_ax_nam_trunk_dbg("trunk port num : %d,bitmap dev0MbrBmp = %0x dev1MbrBmp = %0x\n",num_ports,*dev0MbrBmp,*dev1MbrBmp);
#endif
	return ret;
}

int nam_trunk_port_trunkId_get(unsigned char devNum,unsigned char portNum,unsigned long *member,unsigned short *trunkId)
{
	int ret = TRUNK_CONFIG_FAIL;
#ifdef DRV_LIB_CPSS
	ret = cpssDxChTrunkPortTrunkIdGet(devNum,portNum,member,trunkId);
	if(ret != TRUNK_CONFIG_SUCCESS)
	{
		syslog_ax_nam_trunk_dbg("get trunk id Err !\n");
		return ret;
	}
#endif
	return ret;
}

int nam_trunk_table_entry_get(unsigned char devNum,unsigned short trunkId,unsigned long *numMembersPtr,CPSS_TRUNK_MEMBER_STC  membersArray[TRUNK_MEMBER_NUM_MAX])
{
	int ret = TRUNK_CONFIG_FAIL;
#ifdef DRV_LIB_CPSS
	ret = cpssDxChTrunkTableEntryGet(devNum,trunkId,numMembersPtr,membersArray);
	if(ret != TRUNK_CONFIG_SUCCESS)
	{
		syslog_ax_nam_trunk_dbg("get trunk table entry FAIL !\n");
		return ret;
	}
#endif
	return ret;
}

int nam_trunk_table_entry_set(unsigned char devNum,unsigned short trunkId,unsigned long numMembersPtr,CPSS_TRUNK_MEMBER_STC  membersArray[])
{
	int ret = TRUNK_CONFIG_FAIL;
#ifdef DRV_LIB_CPSS
	ret = cpssDxChTrunkTableEntrySet(devNum,trunkId,numMembersPtr,membersArray);
	if(ret != TRUNK_CONFIG_SUCCESS)
	{
		syslog_ax_nam_trunk_dbg("get trunk table entry FAIL !\n");
		return ret;
	}
#endif
	return ret;
}



unsigned long nam_cscd_dev_map_set
(
	unsigned char devNum,
	unsigned char targetDevNum,
    unsigned char targetPortNum,
    CPSS_CSCD_LINK_TYPE_STC *cascadeLinkPtr,
    CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT srcPortTrunkHashEn
)
{
	unsigned long ret = TRUNK_CONFIG_FAIL;
#ifdef DRV_LIB_CPSS
	ret = cpssDxChCscdDevMapTableSet(devNum,targetDevNum,targetPortNum,cascadeLinkPtr,srcPortTrunkHashEn);
	if(ret != TRUNK_CONFIG_SUCCESS)
	{
		syslog_ax_nam_trunk_err("Set cscd dev map table FAIL  !\n");
		return ret;
	}
#endif
	return ret;
}

unsigned long nam_cscd_dev_map_get
(
	unsigned char devNum,
	unsigned char targetDevNum,
    unsigned char targetPortNum,
    CPSS_CSCD_LINK_TYPE_STC *cascadeLinkPtr,
    CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT *srcPortTrunkHashEnPtr
)
{
	unsigned long ret = TRUNK_CONFIG_FAIL;
#ifdef DRV_LIB_CPSS
	ret = cpssDxChCscdDevMapTableGet(devNum,targetDevNum,targetPortNum,cascadeLinkPtr,srcPortTrunkHashEnPtr);
	if(ret != TRUNK_CONFIG_SUCCESS)
	{
		syslog_ax_nam_trunk_err("Get cscd dev map table FAIL  !\n");
		return ret;
	}
#endif
	return ret;
}


#ifdef __cplusplus
}
#endif

