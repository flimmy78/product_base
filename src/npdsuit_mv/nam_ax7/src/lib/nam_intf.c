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
* nam_intf.c
*
*
* CREATOR:
*       qinhs@autelan.com
*
* DESCRIPTION:
*       APIs used in NAM for L3 interface process.
*
* DATE:
*       02/21/2008
*
*  FILE REVISION NUMBER:
*       $Revision: 1.41 $
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <string.h>
#include "util/npd_list.h"
#include "sysdef/npd_sysdef.h"
#include "sysdef/returncode.h"
#include "npd/nam/npd_amapi.h"
#include "nam_asic.h"
#include "nam_log.h"
#include "nam_vlan.h"
#include "nam_intf.h"

#ifdef DRV_LIB_BCM
#include <bcm/l3.h>
#include <bcm/port.h>
#include <bcm/switch.h>
#endif
/*********************************************
 *
 * RETURN:
 *		INTERFACE_RETURN_CODE_NAM_ERROR
 *		INTERFACE_RETURN_CODE_SUCCESS
 *********************************************/
int nam_l3_intf_on_port_enable
(
	unsigned int port_index, 
	unsigned short vid,
	unsigned int enable
)
{
	int ret;
	unsigned char devNum = 0;
	unsigned char portNum = 0;
	ret = npd_get_devport_by_global_index(port_index,&devNum,&portNum);
	if(NAM_OK != ret )
	{
		syslog_ax_nam_intf_err("nam_l3_intf_on_port_enable::npd_get_devport_by_global_index %#0x => dev %d port %d error\n",
						port_index,devNum,portNum);
		return INTERFACE_RETURN_CODE_ERROR;
	}
#ifdef DRV_LIB_CPSS
#if 0
	ret = cpssDxChBrgVlanIpMcRouteEnable(devNum,vid,0,enable);
	if(0 != ret)
	{
		syslog_ax_nam_intf_err("nam_l3_intf_on_port_enable:: cpssDxChBrgVlanIpMcRouteEnable VLAN MC ERROR ret %d\n",ret);
		return ret;
	}

	ret = cpssDxChBrgVlanIpUcRouteEnable(devNum,vid,0,enable);
	if(0 != ret)
	{
		syslog_ax_nam_intf_err("nam_l3_intf_on_port_enable:: cpssDxChBrgVlanIpMcRouteEnable VLAN UC ERROR ret %d\n",ret);
		return ret;
	}
#endif
	ret = cpssDxChIpPortRoutingEnable(devNum,portNum,0,0,enable);
	if(NAM_OK != ret)
	{
		syslog_ax_nam_intf_err("nam_l3_intf_on_port_enable:: cpssDxChIpPortRoutingEnable PORT UC ERROR ret %d\n",ret);
		return INTERFACE_RETURN_CODE_NAM_ERROR;
	}

	ret = cpssDxChIpPortRoutingEnable(devNum,portNum,1,0,enable);
	if(NAM_OK != ret)
	{
		syslog_ax_nam_intf_err("nam_l3_intf_on_port_enable:: cpssDxChIpPortRoutingEnable PORT MC ERRORret %d\n",ret);
		return INTERFACE_RETURN_CODE_NAM_ERROR;
	}
#endif
    
#ifdef DRV_LIB_BCM
    /*==========================================================
         NOT FOUND VLAN SUPPORTING FOR ROUTE WITHOUT SET VLAN AS L3 INTF,SO NOW JUST
         SET PORT ATTRIBUTE.!!!!!!!!!!!!!!!!!
	===========================================================*/
    

    ret = bcm_port_l3_enable_set(devNum,portNum,enable);
    if(NAM_E_NONE != ret){
        syslog_ax_nam_intf_err("set unit %d port %d enable %d failed for ret %d \n",devNum,portNum,enable,ret);
        return INTERFACE_RETURN_CODE_NAM_ERROR;
	}
	ret = bcm_port_control_set(devNum,portNum, bcmPortControlPassControlFrames,enable);
    if(NAM_E_NONE != ret){
        syslog_ax_nam_intf_err("set unit %d port %d control enable %d failed for ret %d \n",devNum,portNum,enable,ret);
	}
	else{
        syslog_ax_nam_intf_dbg("set control packet on unit %d port %d success!\n",devNum,portNum);
	}
#endif
	syslog_ax_nam_intf_dbg("nam_l3_intf_on_port_enable:: SUCCESS\n");
	if(NAM_OK != ret){
		ret = INTERFACE_RETURN_CODE_NAM_ERROR;
	}
	else{
		ret = INTERFACE_RETURN_CODE_SUCCESS;
	}
	return ret;

}
/******************************************************
 *
 * RETURN:
 *		INTERFACE_RETURN_CODE_SUCCESS
 *		INTERFACE_RETURN_CODE_ERROR
 *
 ******************************************************/
int nam_cscd_route_on_vlan_enable
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long enable
)
{
	int ret = NPD_SUCCESS;
	
#ifdef DRV_LIB_CPSS	
	ret = cpssDxChIpPortRoutingEnable(devNum,portNum,0,0,enable);
	if(NAM_OK != ret)
	{
		syslog_ax_nam_intf_err("nam_cscd_route_on_vlan_enable:: cpssDxChIpPortRoutingEnable PORT UC ERROR ret %d\n",ret);
		return INTERFACE_RETURN_CODE_NAM_ERROR;
	}

	ret = cpssDxChIpPortRoutingEnable(devNum,portNum,1,0,enable);
	if(NAM_OK != ret)
	{
		syslog_ax_nam_intf_err("nam_cscd_route_on_vlan_enable:: cpssDxChIpPortRoutingEnable PORT MC ERROR ret %d\n",ret);
		return INTERFACE_RETURN_CODE_NAM_ERROR;
	}
#endif

	if(NPD_SUCCESS == ret){
		ret = INTERFACE_RETURN_CODE_SUCCESS;
	}
	else{
		ret = INTERFACE_RETURN_CODE_NAM_ERROR;
	}
	return ret;
}

/******************************************************
 *
 * RETURN:
 *		INTERFACE_RETURN_CODE_SUCCESS
 *		INTERFACE_RETURN_CODE_ERROR
 *
 ******************************************************/
int nam_ports_route_on_vlan_enable(unsigned int port_index,unsigned int enable)
{
	int ret;
	unsigned char devNum = 0;
	unsigned char portNum = 0;
	ret = npd_get_devport_by_global_index(port_index,&devNum,&portNum);
	if(NPD_SUCCESS != ret)
	{
		syslog_ax_nam_intf_err("eth-port %#x to asic port(%d,%d)failed for %d\n",\
						port_index,devNum,portNum,ret);
		return INTERFACE_RETURN_CODE_ERROR;
	}
#ifdef DRV_LIB_CPSS	
	ret = cpssDxChIpPortRoutingEnable(devNum,portNum,0,0,enable);
	if(NAM_OK != ret)
	{
		syslog_ax_nam_intf_err("nam_ports_route_on_vlan_enable:: cpssDxChIpPortRoutingEnable PORT UC ERROR ret %d\n",ret);
		return INTERFACE_RETURN_CODE_NAM_ERROR;
	}

	ret = cpssDxChIpPortRoutingEnable(devNum,portNum,1,0,enable);
	if(NAM_OK != ret)
	{
		syslog_ax_nam_intf_err("nam_ports_route_on_vlan_enable:: cpssDxChIpPortRoutingEnable PORT MC ERROR ret %d\n",ret);
		return INTERFACE_RETURN_CODE_NAM_ERROR;
	}


#endif
#ifdef DRV_LIB_BCM
	ret = bcm_port_l3_enable_set(devNum,portNum,enable);
    if(NAM_E_NONE != ret){
        syslog_ax_nam_intf_err("set unit %d port %d enable %d failed for ret %d \n",devNum,portNum,enable,ret);
	}
	else{
        syslog_ax_nam_intf_dbg("set l3 enable on unit %d port %d success!\n",devNum,portNum);
	}
	
	ret = bcm_port_control_set(devNum,portNum, bcmPortControlPassControlFrames,enable);
    if(NAM_E_NONE != ret){
        syslog_ax_nam_intf_err("set unit %d port %d control enable %d failed for ret %d \n",devNum,portNum,enable,ret);
	}
	else{
        syslog_ax_nam_intf_dbg("set control packet on unit %d port %d success!\n",devNum,portNum);
	}
#endif
	if(NPD_SUCCESS == ret){
		ret = INTERFACE_RETURN_CODE_SUCCESS;
	}
	else{
		ret = INTERFACE_RETURN_CODE_NAM_ERROR;
	}
	return ret;
}


/*********************************************
 *
 * RETURN:
 *		COMMON_RETURN_CODE_NULL_PTR
 *		INTERFACE_RETURN_CODE_ERROR
 *		INTERFACE_RETURN_CODE_NAM_ERROR
 *		INTERFACE_RETURN_CODE_SUCCESS
 *********************************************/
int nam_l3_intf_on_vlan_enable
(
    unsigned char devNum,
    unsigned int vid,
    unsigned char* addr,
    unsigned int enable,
    unsigned int ifindex)
{
	int ret = 0;
	int enabled = 1,disabled = 0;
#ifdef DRV_LIB_BCM	
	int rv = 0,unit = 0,num_of_asic = 0;
	bcm_l3_intf_t intf,tmp;
#endif
#ifdef DRV_LIB_CPSS
#if 0
	ret = cpssDxChBrgVlanIpMcRouteEnable(devNum,vid,0,enable);
	if(0 != ret)
	{
		syslog_ax_nam_intf_err("nam_l3_intf_on_vlan_enable:: cpssDxChBrgVlanIpMcRouteEnable VLAN MC ERROR ret %d\n",ret);
		return ret;
	}
#endif
	ret = cpssDxChBrgVlanIpUcRouteEnable(devNum,vid,0,enable);
	if(NAM_OK != ret)
	{
		syslog_ax_nam_intf_err("nam_l3_intf_on_vlan_enable:: cpssDxChBrgVlanIpMcRouteEnable VLAN UC ERROR ret %d\n",ret);
		return INTERFACE_RETURN_CODE_NAM_ERROR;
	}
  return 0;
#endif
#ifdef DRV_LIB_BCM    

	if(NULL == addr){
		syslog_ax_nam_fdb_err("Bad addr parameter to input for l3 intf create!vid %d\n",vid);
        return COMMON_RETURN_CODE_NULL_PTR;
	}
	syslog_ax_nam_fdb_dbg("vid %d,mac %02x:%02x:%02x:%02x:%02x:%02x enable %d\n",vid,addr[0],\
		addr[1],addr[2],addr[3],addr[4],addr[5],enable);
	
	num_of_asic = nam_asic_get_instance_num();

	switch(enable){
	   case INTF_ENABLE:   	
	   for(unit = 0;unit < num_of_asic; unit++){
	   	   memset(&intf,0,sizeof(bcm_l3_intf_t));
		   intf.l3a_vid = vid;
		   memcpy(intf.l3a_mac_addr,addr,6);
		   intf.l3a_intf_id = ifindex;
		   intf.l3a_ttl = 128;/*ttl set*/
		   intf.l3a_flags = (NAM_L3_WITH_ID  | NAM_L3_ADD_TO_ARL | NAM_L3_REPLACE);
#if 0
		   memset(&tmp,0,sizeof(bcm_l3_intf_t));
		   tmp.l3a_vid = vid;
		   ret = bcm_l3_intf_find_vlan(unit,&tmp);
	       if(NAM_E_NONE == ret){//not found in the previous items
	           syslog_ax_nam_intf_err("intf vid %d has already exist!\n",vid);
			   ret =  NAM_E_NONE;
		   }
	   	   if(NAM_E_NOT_FOUND != ret){
		   	  syslog_ax_nam_intf_err("intf vid %d has bad parameter input!\n",vid);
              ret = NAM_E_PARAM;
			  return ret;
		   }
		   else{
              ret = NAM_E_NONE;
		   	}
#endif
		   ret = bcm_l3_intf_create(unit,&intf);
		   if(NAM_E_NONE != ret){
		   	  syslog_ax_nam_intf_err("set intf vid %d failed on unit %d for ret %d\n",vid,unit,ret);
              return INTERFACE_RETURN_CODE_NAM_ERROR;
		   }
		   else {
		   	  syslog_ax_nam_intf_dbg("set intf vid %d success on unit %d,set flags %#x!get info: \n",vid,unit,intf.l3a_flags);		   	  
              memset(&tmp,0,sizeof(bcm_l3_intf_t));
              tmp.l3a_intf_id = intf.l3a_intf_id;
              ret = bcm_l3_intf_get(unit,&tmp);
			  if(NAM_E_NONE == ret){
                 syslog_ax_nam_intf_dbg("find info: vid %d,mac %02x:%02x:%02x:%02x:%02x:%02x,flags %#x,intf id %d\n",
				 	       tmp.l3a_vid,tmp.l3a_mac_addr[0],tmp.l3a_mac_addr[1],tmp.l3a_mac_addr[2],tmp.l3a_mac_addr[3],\
				 	       tmp.l3a_mac_addr[4],tmp.l3a_mac_addr[5],tmp.l3a_flags,tmp.l3a_intf_id);
			  }
			  else{
                 syslog_ax_nam_intf_err("ERROR:can not get intf info has been set for ret %d!\n",ret);
                 return INTERFACE_RETURN_CODE_NAM_ERROR;
			  }
		   }
	   }
	   break;
	   case INTF_DISABLE:
	   for(unit = 0;unit < num_of_asic; unit++){
	   	   memset(&tmp,0,sizeof(bcm_l3_intf_t));
		   tmp.l3a_vid = vid;
		  /* tmp.l3a_intf_id = ifindex;*/
		   ret = bcm_l3_intf_find_vlan(unit,&tmp);
		   if(NAM_E_NOT_FOUND == ret){/*not found in the  items*/
		      syslog_ax_nam_intf_err("intf vid %d has NO exist!\n",vid);
			  return INTERFACE_RETURN_CODE_NAM_ERROR;
		   }
		   else{
		   	  syslog_ax_nam_intf_err("del intf: vid %d,mac %02x:%02x:%02x:%02x:%02x:%02xintf id %d  on unit %d\n",
			 	       tmp.l3a_vid,tmp.l3a_mac_addr[0],tmp.l3a_mac_addr[1],tmp.l3a_mac_addr[2],tmp.l3a_mac_addr[3],\
			 	       tmp.l3a_mac_addr[4],tmp.l3a_mac_addr[5],tmp.l3a_intf_id,unit);
		       ret = bcm_l3_intf_delete(unit,&tmp);
			   if(NAM_E_NONE != ret){
			   	  syslog_ax_nam_intf_err("del intf vid %d failed on unit %d for ret %d\n",vid,unit,ret);
                  return INTERFACE_RETURN_CODE_NAM_ERROR;
			   }
			   else {
			   	  syslog_ax_nam_intf_dbg("del intf vid %d success on unit %d,set flags %#x!get info: \n",vid,unit,intf.l3a_flags);		   	  
		          memset(&tmp,0,sizeof(bcm_l3_intf_t));
				  /*tmp.l3a_intf_id = ifindex;*/
				  tmp.l3a_vid = vid;
		          ret = bcm_l3_intf_get(unit,&tmp);
				  if(NAM_E_NONE != ret){
		             syslog_ax_nam_intf_err("del intf: vid %d,mac %02x:%02x:%02x:%02x:%02x:%02xintf id %d failed for ret %d on unit %d\n",
					 	       tmp.l3a_vid,tmp.l3a_mac_addr[0],tmp.l3a_mac_addr[1],tmp.l3a_mac_addr[2],tmp.l3a_mac_addr[3],\
					 	       tmp.l3a_mac_addr[4],tmp.l3a_mac_addr[5],tmp.l3a_intf_id,ret,unit);
		             return INTERFACE_RETURN_CODE_NAM_ERROR;
				  }
				  else{
		             syslog_ax_nam_intf_dbg("del intf %d info success!\n",tmp.l3a_intf_id);
				  }
			   }
		   }
	   }
	   break;
	   default:
	    syslog_ax_nam_intf_err("Error flag input!\n");
		return INTERFACE_RETURN_CODE_ERROR;
		break;
	}
	syslog_ax_nam_intf_dbg("intf vid %d create result %d\n",vid,rv);
	return INTERFACE_RETURN_CODE_SUCCESS;	

#endif

}
/**********************************************
 *
 * RETURN:
 *		INTERFACE_RETURN_CODE_SUCCESS
 *		INTERFACE_RETURN_CODE_NAM_ERROR
 *
 **********************************************/

unsigned int nam_asic_udp_bc_trap_en(unsigned short vid,unsigned int enable)
{
	unsigned long ret = INTERFACE_RETURN_CODE_SUCCESS;
	unsigned char dev = 0;
	unsigned long numOfPp = 0;
	unsigned int value = 0;
	value =  enable ? 1 : 0;
	
#ifdef DRV_LIB_CPSS
	numOfPp = nam_asic_get_instance_num();
	NAM_DEBUG(("find %ld dev(s)",numOfPp));
	for(dev=0;dev<numOfPp;dev++){
		/*enable <BCUDPTrapMirroEn>field in VLAN entry*/
		ret = cpssDxChBrgVlanUdpBcPktsToCpuEnable(dev,vid,value);
		if(NAM_OK != ret) {
			syslog_ax_nam_intf_err("set vlan %d on device %d udp bc packet trap error %d",vid,dev,ret);
			return INTERFACE_RETURN_CODE_NAM_ERROR;
		}
		
		/*set UdpBcDestPortCfg entry*/
		/* UDP dest port 1234 is for CAPWAP */
		ret = cpssDxChBrgGenUdpBcDestPortCfgSet(
												dev,	/*devNum*/
												0,	/*entryIndex*/
												1234,	/*udpPortNum :0x4D2*/
												CPSS_NET_UDP_BC_MIRROR_TRAP1_E,	/*CPU code*/
												CPSS_PACKET_CMD_MIRROR_TO_CPU_E	/*packet Cmd*/
												);
		if(NAM_OK != ret ) {
			syslog_ax_nam_intf_err("set device %d capwap packet udp port 1234 trap error %d",dev,ret);
			return INTERFACE_RETURN_CODE_NAM_ERROR;
		}

		/* UDP dest port 68 is for DHCP offer packet from DHCP server  
   		 *  This is to support DHCP server answer with Broadcast DHCP offer 
		 */
		ret = cpssDxChBrgGenUdpBcDestPortCfgSet(
												dev,	/*devNum*/
												1,	/*entryIndex*/
												67,	/* udpPortNum :0x43 */
												CPSS_NET_UDP_BC_MIRROR_TRAP0_E,	/*CPU code*/
												CPSS_PACKET_CMD_MIRROR_TO_CPU_E	/*packet Cmd*/
												);
		if(NAM_OK != ret ) {
			syslog_ax_nam_intf_err("set device %d dhcp packet udp port 67 trap error %d",dev,ret);
			return INTERFACE_RETURN_CODE_NAM_ERROR;
		}

		
		/* UDP dest port 5246 is for CAPWAP discovery packet (another UDP port different from 1234) 
   		 *  This is to support CAPWAP discovery with new specification 
		 */
		ret = cpssDxChBrgGenUdpBcDestPortCfgSet(
												dev,	/*devNum*/
												2,	/*entryIndex*/
												5246,	/* udpPortNum :0x147E */
												CPSS_NET_UDP_BC_MIRROR_TRAP1_E,	/*CPU code*/
												CPSS_PACKET_CMD_MIRROR_TO_CPU_E	/*packet Cmd*/
												);
		if(NAM_OK != ret ) {
			syslog_ax_nam_intf_err("set device %d capwap packet udp port 5246 trap error %d",dev,ret);
			return INTERFACE_RETURN_CODE_NAM_ERROR;
		}
	}
#endif
	return INTERFACE_RETURN_CODE_SUCCESS;
}

#if 0
/*********************************************
 *
 * RETURN:
 *		INTERFACE_RETURN_CODE_NAM_ERROR
 *		INTERFACE_RETURN_CODE_SUCCESS
 *********************************************/
int nam_udp_send_to_cpu(unsigned short vid,unsigned int enable)
{
	unsigned char dev  = 0;
	int ret = 0;
#ifdef DRV_LIB_CPSS

	ret = cpssDxChBrgVlanUdpBcPktsToCpuEnable(dev,vid,enable);

#endif
	if(NAM_OK != ret){
		ret = INTERFACE_RETURN_CODE_NAM_ERROR;
	}
	else{
		ret = INTERFACE_RETURN_CODE_SUCCESS;
	}

	return ret;
}
#endif
/*********************************************
 *
 * RETURN:
 *		INTERFACE_RETURN_CODE_NAM_ERROR
 *		INTERFACE_RETURN_CODE_SUCCESS
 *********************************************/
int nam_rip_send_to_cpu(unsigned int enable)
{
	unsigned char dev  = 0;
	int ret = 0;
	
#ifdef DRV_LIB_CPSS
	ret  = cpssDxChBrgGenRipV1MirrorToCpuEnable(dev,enable);
	if(0 != ret) {
		NAM_DEBUG(("dev %d cpssDxChBrgGenRipV1MirrorToCpuEnable err %d \n",ret));
	}
#endif

#ifdef DRV_LIB_BCM

#endif
	if(NAM_OK != ret){
		ret = INTERFACE_RETURN_CODE_NAM_ERROR;
	}
	else{
		ret = INTERFACE_RETURN_CODE_SUCCESS;
	}
	return ret;
}

/*********************************************
 *
 * RETURN:
 *		INTERFACE_RETURN_CODE_NAM_ERROR
 *		INTERFACE_RETURN_CODE_SUCCESS
 *********************************************/
/*by zhubo@autelan.com add TTL dec ,trap the packet to cpu*/
int nam_route_config()
{
	int ret = 0;
	
	unsigned char devNum = 0;
	
#ifdef DRV_LIB_CPSS
	/* CPSS_DXCH_IP_EXCP_UC_TTL_EXCEED_E  = 14        - IP UC TTL Exceeded error.*/
	ret = cpssDxChIpExceptionCommandSet(devNum,14,CPSS_IP_PROTOCOL_IPV4_E,CPSS_PACKET_CMD_TRAP_TO_CPU_E);
#endif
	if(NAM_OK != ret){
		ret = INTERFACE_RETURN_CODE_NAM_ERROR;
	}
	else{
		ret = INTERFACE_RETURN_CODE_SUCCESS;
	}
	return ret;
}
/*****************************************
 * RETURN:
 *		INTERFACE_RETURN_CODE_SUCCESS
 *		COMMON_RETURN_CODE_NULL_PTR
 *		INTERFACE_RETURN_CODE_NAM_ERROR
 *
 *****************************************/
int nam_intf_config_basemac
(
	unsigned char  devNum,
	unsigned short port,
	unsigned char* mac
)

{
    int ret = INTERFACE_RETURN_CODE_SUCCESS;
	GT_ETHERADDR  macPtr;
	if(NULL == mac){
		return COMMON_RETURN_CODE_NULL_PTR;
	}
	memcpy(macPtr.arEther,mac,6);
#ifdef DRV_LIB_CPSS
	if((ret = cpssDxChIpRouterMacSaBaseSet(devNum,&macPtr)) != NAM_OK) {
		nam_syslog_err("cpssDxChIpRouterMacSaBaseSet error %d",ret);
		ret = INTERFACE_RETURN_CODE_NAM_ERROR;
	}
    else if((ret = cpssDxChIpRouterPortVlanMacSaLsbSet(devNum,port,mac[5])) != NAM_OK) {
		nam_syslog_err("cpssDxChIpRouterPortVlanMacSaLsbSet error %d",ret);
		ret = INTERFACE_RETURN_CODE_NAM_ERROR;
	}
	else{
		ret = INTERFACE_RETURN_CODE_SUCCESS;
	}
#endif
    return ret;
}

/*********************************************
 *
 * RETURN:
 *		INTERFACE_RETURN_CODE_NAM_ERROR
 *		INTERFACE_RETURN_CODE_SUCCESS
 *********************************************/
int nam_vlan_unreg_filter
(
	unsigned char devNum,
	unsigned short vlanId,
	unsigned int packetType,
	unsigned int cmd
)
{
  int ret=0;
#ifdef DRV_LIB_CPSS
  ret =  cpssDxChBrgVlanUnkUnregFilterSet(devNum,vlanId,packetType,cmd);
  nam_syslog_dbg("vlan unreg filter, ret %#0x \n",ret);
#endif
  if(NAM_OK != ret){
	  ret = INTERFACE_RETURN_CODE_NAM_ERROR;
  }
  else{
	  ret = INTERFACE_RETURN_CODE_SUCCESS;
  }

  return ret;
}

/*********************************************
 *
 * RETURN:
 *		INTERFACE_RETURN_CODE_NAM_ERROR
 *		INTERFACE_RETURN_CODE_SUCCESS
 *********************************************/
int nam_arp_trap_enable
(
	unsigned char dev,
	unsigned char port,
	unsigned int enable
)
{
   int ret = INTERFACE_RETURN_CODE_SUCCESS;
 #ifdef DRV_LIB_CPSS
   ret = cpssDxChBrgGenArpTrapEnable(dev,port,enable);
#endif

#ifdef DRV_LIB_BCM
    int arg = 0;
	ret = bcm_switch_control_port_set(dev,port,bcmSwitchArpReplyToCpu,1);
	if(NAM_E_NONE != ret){
		syslog_ax_nam_intf_err("set unit %d port %d control enable %d failed for ret %d \n",dev,port,enable,ret);
	}
	else{
		syslog_ax_nam_intf_dbg("set arp reply on unit %d port %d  1!\n",dev,port);
		ret = bcm_switch_control_port_get(dev,port,bcmSwitchArpReplyToCpu,&arg);
		if(0 != ret){
            syslog_ax_nam_intf_dbg("Failed to get asic port(%d,%d)arp replay arg for %d\n",dev,port,ret);
		}
		else{
            syslog_ax_nam_intf_dbg("get arp reply on unit %d port %d  arg %d!\n",dev,port,arg);
		}
	}
	
	ret = bcm_switch_control_port_set(dev,port,bcmSwitchArpRequestToCpu,1);
	if(NAM_E_NONE != ret){
		syslog_ax_nam_intf_err("set unit %d port %d control enable %d failed for ret %d \n",dev,port,enable,ret);
	}
	else{
		syslog_ax_nam_intf_dbg("set arp request on unit %d port %d success!\n",dev,port);
		ret = bcm_switch_control_port_get(dev,port,bcmSwitchArpRequestToCpu,&arg);
		if(0 != ret){
            syslog_ax_nam_intf_dbg("Failed to get asic port(%d,%d)arp request arg for %d\n",dev,port,ret);
		}
		else{
            syslog_ax_nam_intf_dbg("get arp request on unit %d port %d  arg %d!\n",dev,port,arg);
		}

	}
	/*bcmSwitchL3UcastTtl1ToCpu*/
	ret = bcm_switch_control_port_set(dev,port,bcmSwitchL3UcastTtl1ToCpu,1);
	if(NAM_E_NONE != ret){
		syslog_ax_nam_intf_err("set unit %d port %d control enable %d failed for ret %d \n",dev,port,enable,ret);
	}
	else{
		syslog_ax_nam_intf_dbg("set ttl 1 on unit %d port %d success!\n",dev,port);
		ret = bcm_switch_control_port_get(dev,port,bcmSwitchL3UcastTtl1ToCpu,&arg);
		if(NAM_OK != ret){
            syslog_ax_nam_intf_dbg("Failed to get asic port(%d,%d)ttl 1 arg for %d\n",dev,port,ret);
		}
		else{
            syslog_ax_nam_intf_dbg("get ttl 1 on unit %d port %d  arg %d!\n",dev,port,arg);
		}

	}
#endif
   if(NAM_OK != ret){
	   ret = INTERFACE_RETURN_CODE_NAM_ERROR;
   }
   else{
	   ret = INTERFACE_RETURN_CODE_SUCCESS;
   }

   return ret;
}

/***********************************************************
 *
 * RETURN:
 *		INTERFACE_RETURN_CODE_NAM_ERROR
 *		INTERFACE_RETURN_CODE_SUCCESS
 *
 ***********************************************************/
int nam_intf_advanced_route_del_cscd_port
(
	unsigned int vid,
	unsigned int productId
)
{
	int ret = INTERFACE_RETURN_CODE_SUCCESS;
#ifdef DRV_LIB_CPSS	
	switch(productId) {
		default:
			return ret;
		case PRODUCT_ID_AX7K:
		case PRODUCT_ID_AX7K_I:
			if(NAM_OK != (ret = cpssDxChBrgVlanPortDelete (0, vid, 26)))
				ret = INTERFACE_RETURN_CODE_NAM_ERROR;
			else if(NAM_OK != (ret = cpssDxChBrgVlanPortDelete (1, vid, 26)))
				ret = INTERFACE_RETURN_CODE_NAM_ERROR;	
			else if(NAM_OK != (ret = cpssDxChBrgVlanPortDelete (1, vid, 27)))
				ret = INTERFACE_RETURN_CODE_NAM_ERROR;
			
			break;
		case PRODUCT_ID_AX5K:
			if(NAM_OK != (ret = cpssDxChBrgVlanPortDelete(0, vid, 0))){
				ret = INTERFACE_RETURN_CODE_NAM_ERROR;
			}
			break;
		case PRODUCT_ID_AX5K_I:
			if(0 != (ret = cpssDxChBrgVlanPortDelete(0, vid, 24))) {
				ret = NAM_ERR;
			}
			break;

	}
#endif
	if(NAM_OK == ret){
		ret = INTERFACE_RETURN_CODE_SUCCESS;
	}
	return ret;
}
/***********************************************************
 *
 * RETURN:
 *		INTERFACE_RETURN_CODE_NAM_ERROR
 *		INTERFACE_RETURN_CODE_SUCCESS
 *
 ***********************************************************/
int nam_intf_advanced_route_add_cscd_port
(
	unsigned short vid,
	unsigned int productId
)
{
	int ret = INTERFACE_RETURN_CODE_SUCCESS;
	
#ifdef DRV_LIB_CPSS
	CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT portTaggingCmd;


	switch(productId) {
		default:
			return ret;
		case PRODUCT_ID_AX7K:
		case PRODUCT_ID_AX7K_I:
#ifdef DRV_LIB_CPSS_3_4
			if(NAM_OK != (ret = cpssDxChBrgVlanMemberAdd (0, vid, 26, NPD_TRUE, CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E))) {
				nam_syslog_dbg(" cpssDxChBrgVlanMemberAdd : ret %#0x\n",ret);
				ret = INTERFACE_RETURN_CODE_NAM_ERROR;
			}
			else if(NAM_OK != (ret = cpssDxChBrgVlanMemberAdd (1, vid, 26, NPD_TRUE, CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E))) {
				nam_syslog_dbg(" cpssDxChBrgVlanMemberAdd : ret %#0x\n",ret);
				ret = INTERFACE_RETURN_CODE_NAM_ERROR;	
			}
			else if(NAM_OK != (ret = cpssDxChBrgVlanMemberAdd (1, vid, 27, NPD_TRUE, CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E))) {
				nam_syslog_dbg(" cpssDxChBrgVlanMemberAdd : ret %#0x\n",ret);
				ret = INTERFACE_RETURN_CODE_NAM_ERROR;
			}
			break;
		case PRODUCT_ID_AX5K:
			if(NAM_OK != (ret = cpssDxChBrgVlanMemberAdd(0, vid, 0, NPD_TRUE, CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E))) {
				nam_syslog_dbg(" cpssDxChBrgVlanMemberAdd : ret %#0x\n",ret);
				ret = INTERFACE_RETURN_CODE_NAM_ERROR;
			}
			break;
		case PRODUCT_ID_AX5K_I:
			if(0 != (ret = cpssDxChBrgVlanMemberAdd(0, vid, 24, NPD_TRUE, CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E))) {
				ret = NAM_ERR;
			}
			break;

#else
			if(NAM_OK != (ret = cpssDxChBrgVlanMemberAdd (0, vid, 26, NPD_TRUE))) {
				nam_syslog_dbg(" cpssDxChBrgVlanMemberAdd : ret %#0x\n",ret);
				ret = INTERFACE_RETURN_CODE_NAM_ERROR;
			}
			else if(NAM_OK != (ret = cpssDxChBrgVlanMemberAdd (1, vid, 26, NPD_TRUE))) {
				nam_syslog_dbg(" cpssDxChBrgVlanMemberAdd : ret %#0x\n",ret);
				ret = INTERFACE_RETURN_CODE_NAM_ERROR;	
			}
			else if(NAM_OK != (ret = cpssDxChBrgVlanMemberAdd (1, vid, 27, NPD_TRUE))) {
				nam_syslog_dbg(" cpssDxChBrgVlanMemberAdd : ret %#0x\n",ret);
				ret = INTERFACE_RETURN_CODE_NAM_ERROR;
			}
			break;
		case PRODUCT_ID_AX5K:
			if(NAM_OK != (ret = cpssDxChBrgVlanMemberAdd(0, vid, 0, NPD_TRUE))) {
				nam_syslog_dbg(" cpssDxChBrgVlanMemberAdd : ret %#0x\n",ret);
				ret = INTERFACE_RETURN_CODE_NAM_ERROR;
			}
			break;
		case PRODUCT_ID_AX5K_I:
			if(0 != (ret = cpssDxChBrgVlanMemberAdd(0, vid, 24, NPD_TRUE))) {
				ret = INTERFACE_RETURN_CODE_NAM_ERROR;
			}
			break;
#endif			
	}
#endif
	if(NAM_OK == ret){
		ret = INTERFACE_RETURN_CODE_SUCCESS;
	}
	return ret;
}
/***************************************************
 *
 * RETURN:
 *		INTERFACE_RETURN_CODE_SUCCESS
 *		INTERFACE_RETURN_CODE_NAM_ERROR
 *
 ***************************************************/ 
int nam_intf_ip_ctrl_tocpu
(
	unsigned char devNum,
	unsigned short vlanId,
	unsigned int ipCntrlType
)
{

    int ret = 0;
#ifdef DRV_LIB_CPSS

    ret = cpssDxChBrgVlanIpCntlToCpuSet(devNum,vlanId,ipCntrlType);
#endif
	if(NAM_OK == ret){
		ret = INTERFACE_RETURN_CODE_SUCCESS;
	}
	else{
		ret = INTERFACE_RETURN_CODE_NAM_ERROR;
	}
    return ret;
}
#ifdef __cplusplus
}
#endif
