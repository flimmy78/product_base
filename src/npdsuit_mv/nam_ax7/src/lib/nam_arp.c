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
* nam_arp.c
*
*
* CREATOR:
*		qinhs@autelan.com
*
* DESCRIPTION:
*		APIs used in NAM for ARP table driver.
*
* DATE:
*		02/21/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.39 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#define NULL  (0UL)

#include <stdio.h>
#include <string.h>

#include "sysdef/npd_sysdef.h"
#include "sysdef/returncode.h"
#include "util/npd_list.h"
#include "npd/nam/npd_amapi.h"
#include "nam_asic.h"
#include "nam_fdb.h"
#include "nam_utilus.h"
#include "nam_arp.h"
#include "nam_log.h"

#ifdef DRV_LIB_CPSS
#if 0
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpTypes.h>
#endif
#endif
#ifdef DRV_LIB_BCM
#include <bcm/l3.h>
#include <bcm/types.h>
#include <soc/drv.h>
#include <bcm_int/esw/firebolt.h>
#endif

tbl_index_t *mac_table_index = NULL;
tbl_index_t *nexthop_table_index = NULL;
unsigned int arp_aging_bcast = NPD_FALSE;


void print_out_mac
(
	unsigned char* mac
)
{
	if(NULL == mac) {
		return;
	}
	else {
		syslog_ax_nam_arp_dbg("********************\n");
		syslog_ax_nam_arp_dbg(" %02x:%02x:%02x:%02x:%02x:%02x \n", \
			mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
		syslog_ax_nam_arp_dbg("********************\n");
	}
	return;
}

/**********************************************************************************
 * nam_arp_table_index_init
 *
 * Initialize ARP Next-Hop table index and ARP Mac table index
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS  - index init success
 *		ARP_RETURN_CODE_ERROR  - index create failed
 *		
 *	NOTE:
 *
 **********************************************************************************/
int nam_arp_table_index_init
(
	void
)
{
	unsigned int status = 0;
#ifdef DRV_LIB_CPSS	
	mac_table_index = nam_index_create(ARP_MAC_TABLE_SIZE);
	if(NULL == mac_table_index) {
		syslog_ax_nam_arp_err("nam create mac table index null error\r\n");
		return ARP_RETURN_CODE_ERROR;
	}
#endif
	nexthop_table_index = nam_index_create(NEXTHOP_TABLE_SIZE);
	if(NULL == nexthop_table_index) {
		syslog_ax_nam_arp_err("nam create next hop table index null error\r\n");
		return ARP_RETURN_CODE_ERROR;
	}
	/* TODO: occupy Next-Hop entry 0 as default route(0.0.0.0/0): with action Hard-Drop*/
	status = nam_index_get(nexthop_table_index,0);
	if(0 != status) {
		syslog_ax_nam_arp_err("nam occupy next hop table entry index 0 as default route error %d\r\n",status);
	}

	/* TODO:occupy Next-Hop entry 1 as default IPv4 multi-cast: with action ROUTE_AND_MIRROR_CPU*/
	status = nam_index_get(nexthop_table_index,1);
	if(0 != status) {
		syslog_ax_nam_arp_err("nam occupy next hop table entry index 1 as ipv4 ucast route error %d\r\n",status);
	}

	/* TODO:occupy Next-Hop entry 2 as default IPv4 unicast: with action TRAP_CPU*/
	status = nam_index_get(nexthop_table_index,2);
	if(0 != status) {
		syslog_ax_nam_arp_err("nam occupy next hop table entry index 2 as ipv4 mcast route error %d\r\n",status);
	}
	
	return ARP_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 * nam_arp_get_mactbl_index
 *
 * Get arp mac table index
 *
 *	INPUT:
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS - if no error occurred
 *		ARP_RETURN_CODE_TBLINDEX_GET_FAILED - if fail to get index number
 *		
 *	NOTE:
 *
 **********************************************************************************/
int nam_arp_get_mactbl_index
(
	OUT index_elem	 *val
)
{
	index_elem tmpVal = 0;
	unsigned int retVal = 0;
	
	retVal = nam_index_alloc(mac_table_index,&tmpVal);
	if(0 != retVal) {
		syslog_ax_nam_arp_err("nam get arp mac table index error %d",retVal);
		return ARP_RETURN_CODE_TBLINDEX_GET_FAILED;
	}
	else {
		*val 	= tmpVal;
		retVal 	= ARP_RETURN_CODE_SUCCESS;
	}
	syslog_ax_nam_arp_dbg("nam arp mac table get index %d available %d",tmpVal, mac_table_index->available);

	return retVal;
}

/**********************************************************************************
 * nam_arp_free_mactbl_index
 *
 * Turn back arp mac table index
 *
 *	INPUT:
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS - if no error occurred
 *		ARP_RETURN_CODE_INDEX_FREE_FAILED  - if fail to free index number
 *		
 *	NOTE:
 *
 **********************************************************************************/
int nam_arp_free_mactbl_index
(
	IN index_elem	 val
)
{

	unsigned int retVal = 0;
	
	retVal = nam_index_free(mac_table_index,val);
	if(0 != retVal) {
		syslog_ax_nam_arp_err("nam free arp mac table index %d error %d",val,retVal);
		return ARP_RETURN_CODE_INDEX_FREE_FAILED;
	}
	syslog_ax_nam_arp_dbg("nam arp mac table free index %d available %d",val,mac_table_index->available);
	return ARP_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 * nam_arp_get_nexthop_tbl_index
 *
 * Get route next-hop table index
 *
 *	INPUT:
 *	
 *	OUTPUT:
 *           val - the pointer which point to the tbl index alloced
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS - if no error occurred
 *		ARP_RETURN_CODE_TBLINDEX_GET_FAILED - if fail to get index number
 *		
 *	NOTE:
 *
 **********************************************************************************/
int nam_arp_get_nexthop_tbl_index
(
	OUT index_elem	 *val
)
{
	index_elem tmpVal = 0;
	unsigned int retVal = 0;
	
	retVal = nam_index_alloc(nexthop_table_index,&tmpVal);
	if(0 != retVal) {
		syslog_ax_nam_arp_err("nam get route nexthop table index error %d",retVal);
		return ARP_RETURN_CODE_TBLINDEX_GET_FAILED;
	}
	else {
		*val 	= tmpVal;
		retVal 	= ARP_RETURN_CODE_SUCCESS;
	}
	syslog_ax_nam_arp_dbg("nam route nexthop table get index %d available %d",tmpVal, nexthop_table_index->available);

	return retVal;
}
/**********************************************************************************
 * nam_arp_get_nexthop_tbl_index
 *
 * Get route next-hop table index
 *
 *	INPUT:
 *	
 *	OUTPUT:
 *           val - the tblindex we try to get
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS - if no error occurred
 *		ARP_RETURN_CODE_TBLINDEX_GET_FAILED - if fail to get index number
 *		
 *	NOTE:
 *
 **********************************************************************************/
int nam_arp_nexthop_tbl_index_get
(
    index_elem val
)
{
    unsigned int retVal = 0;
	retVal = nam_index_get(nexthop_table_index,val);
	if(NAM_OK != retVal){
		syslog_ax_nam_arp_err("nam route nexthop table index %d get failed!\n",val);
		return ARP_RETURN_CODE_TBLINDEX_GET_FAILED;
	}
	syslog_ax_nam_arp_dbg("nam route nexthop table index %d get success.\n",val);
	return ARP_RETURN_CODE_SUCCESS;	
}

/**********************************************************************************
 * nam_arp_free_nexthop_tbl_index
 *
 * Turn back next-hop table index
 *
 *	INPUT:
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS - if no error occurred
 *		ARP_RETURN_CODE_INDEX_FREE_FAILED  - if fail to free index number
 *		
 *	NOTE:
 *
 **********************************************************************************/
int nam_arp_free_nexthop_tbl_index
(
	IN index_elem	 val
)
{

	unsigned int retVal = 0;

	/* 0 - as default route(initial drop)*/
	/* 1 - as ipv4 mcast route default entry(action route and mirror cpu)*/
	/* 2 - as ipv4 unicast route default entry(action trap)*/
	if((0 == val )||(1 == val) ||(2 == val)) {
		syslog_ax_nam_arp_dbg("nam route nexthop table release index %d available %d",val,nexthop_table_index->available);
		return ARP_RETURN_CODE_SUCCESS;
	}
	
	retVal = nam_index_free(nexthop_table_index,val);
	if(0 != retVal) {
		syslog_ax_nam_arp_err("nam free next-hop table index %d error %d",val,retVal);
		return ARP_RETURN_CODE_INDEX_FREE_FAILED;
	}
	
	syslog_ax_nam_arp_dbg("nam route nexthop table free index %d available %d",val,nexthop_table_index->available);
	return ARP_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 * nam_arp_snooping_op_item
 *
 * ARP snooping database add to/delete from ASIC or other operations.
 *
 *	INPUT:
 *		item - ARP snooping DB items
 *		action - add or delete operation
 *		tblIndex - used when action is NAM_ARP_SNOOP_UPDATE_ITEM
 *	
 *	OUTPUT:
 *		tblIndex - next-hop table index which used to hold next-hop item
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS - if no error occurred
 *		COMMON_RETURN_CODE_NULL_PTR - if input parameters have null pointer
 *		ARP_RETURN_CODE_BADPARAM - if parameters wrong
 *		ARP_RETURN_CODE_TBLINDEX_GET_FAILED - table index get failed 
 *		ARP_RETURN_CODE_NAM_ERROR - asic operation failed
 *		
 *	NOTE:
 *		action may be as follows:
 *		NAM_ARP_SNOOP_ADD_ITEM - add new item to HW table
 *		NAM_ARP_SNOOP_DEL_ITEM - delete existing item from HW table
 *		NAM_ARP_SNOOP_UPDATE_ITEM - update existing item(must be items occupied before)
 *		NAM_ARP_SNOOP_OCCUPY_ITEM - occupy a table entry with command TRAP_to_CPU
 *
 **********************************************************************************/
int nam_arp_snooping_op_item
(
	IN 	struct arp_snooping_item_s *dbItem,
	IN 	unsigned int action,
	INOUT	unsigned int *tblIndex
)
{
	int retVal = ARP_RETURN_CODE_SUCCESS;
	int i = 0,j = 0;
	index_elem	macIndex = 0,nextHopIndex = 0;
	unsigned char devNum = 0;
#ifdef DRV_LIB_CPSS
	ethernet_mac macTblItem;
#ifdef DRV_LIB_CPSS_3_4
	CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC tblEntry;
    memset(&tblEntry,0,sizeof(CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC));
	tblEntry.type = CPSS_DXCH_IP_UC_ROUTE_ENTRY_E;
#else
	nam_nexthop_entry_s tblEntry;
    memset(&tblEntry,0,sizeof(nam_nexthop_entry_s));
#endif
	memset(&macTblItem,0,sizeof(ethernet_mac));
#endif
#ifdef DRV_LIB_BCM
    bcm_l3_host_t    next_hop_entry,tmpEntry;
    bcm_l3_egress_t  nh_entry;
    int                unit = 0,num_of_asic = 0,flag = 0;
	unsigned int tmpIndex = 0;
	eth_mac_t    next_hop_mac;
	unsigned char cmpMac[6];
	memset(&tmpEntry,0,sizeof(bcm_l3_host_t));	
	memset(&next_hop_entry,0,sizeof(bcm_l3_host_t));
	memset(&nh_entry,0,sizeof(bcm_l3_egress_t));
	memset(&next_hop_mac,0,sizeof(eth_mac_t));
	memset(cmpMac,0,6);
	num_of_asic = nam_asic_get_instance_num();
#endif

#ifdef DRV_LIB_CPSS
	 if((NULL == dbItem)&&(action != NAM_ARP_SNOOP_OCCUPY_ITEM)) {
		 syslog_ax_nam_arp_err("nam arp snooping null item specified");
		 retVal = COMMON_RETURN_CODE_NULL_PTR;
		 return retVal;
	 }

	 /*!!!Attention!!!occupy table entry with dbItem NULL*/
	if(NAM_ARP_SNOOP_OCCUPY_ITEM != action) {
#ifdef DRV_LIB_CPSS_3_4
    memset(&tblEntry,0,sizeof(CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC));
	tblEntry.type = CPSS_DXCH_IP_UC_ROUTE_ENTRY_E;
#else
    memset(&tblEntry,0,sizeof(nam_nexthop_entry_s));
#endif
		devNum = dbItem->dstIntf.intf.port.devNum;
		memset(&macTblItem, 0, sizeof(ethernet_mac));
		memcpy((char*)(macTblItem.mac), (char *)(dbItem->mac),MAC_ADDRESS_LEN);
	}
#endif

	/* Get HW device number from reg 0x58  zhangdi 2011-09-27 */
	/* add for get hw devnum, for the hwDev==4 on XCAT, or L3 router is not OK */
    unsigned char hwDev = 0;
    retVal = cpssDxChCfgHwDevNumGet(devNum, &hwDev);
    if (0 != retVal)
    {
		syslog_ax_nam_arp_err("Get hwDev error! \n");
        return ARP_RETURN_CODE_NAM_ERROR;
    }
	syslog_ax_nam_arp_dbg("****** read hwDev == %d *****\n",hwDev);

	switch(action) {
		case NAM_ARP_SNOOP_ADD_ITEM:
		{
#ifdef DRV_LIB_CPSS

			retVal = nam_arp_get_mactbl_index(&macIndex);
			if(ARP_RETURN_CODE_SUCCESS != retVal) {
				syslog_ax_nam_arp_err("nam get mac table index error %d when add",retVal);
				return retVal;
			}
			syslog_ax_nam_arp_dbg("nam get mac table index %d when add\n",macIndex);
			/* TODO: add arp mac address to Mac Table*/

			retVal = cpssDxChIpRouterArpAddrWrite(devNum,macIndex,&macTblItem);
			if(NAM_OK != retVal) {
				syslog_ax_nam_arp_err("nam add arp mac table call driver error %d when add",retVal);
				nam_arp_free_mactbl_index(macIndex);
				return ARP_RETURN_CODE_NAM_ERROR;
			}
			syslog_ax_nam_arp_dbg("nam read dev %d mac tbl %d %02x-%02x-%02x-%02x-%02x-%02x when add\n", \
								devNum,macIndex,macTblItem.mac[0],macTblItem.mac[1],	\
								macTblItem.mac[2],macTblItem.mac[3],macTblItem.mac[4],macTblItem.mac[5]);
		
			/* TODO: add Next-Hop item to route Next-Hop Table*/
			retVal = nam_arp_get_nexthop_tbl_index(&nextHopIndex);
			if(ARP_RETURN_CODE_SUCCESS != retVal) {
				syslog_ax_nam_arp_err("nam get next hop table index error %d when add",retVal);
				nam_arp_free_mactbl_index(macIndex);
				return retVal;
			}
			syslog_ax_nam_arp_dbg("nam get next hop table index %d when add\n",nextHopIndex);
			/* build up next-hop table item*/
#ifdef DRV_LIB_CPSS_3_4
			tblEntry.type = CPSS_DXCH_IP_UC_ROUTE_ENTRY_E;
			tblEntry.entry.regularEntry.cmd = CPSS_PACKET_CMD_ROUTE_E;
			tblEntry.entry.regularEntry.nextHopARPPointer = macIndex;
			tblEntry.entry.regularEntry.nextHopVlanId = dbItem->vid;
			tblEntry.entry.regularEntry.nextHopInterface.vlanId = dbItem->vid;
			if(NAM_FALSE == dbItem->dstIntf.isTrunk) {				
				tblEntry.entry.regularEntry.nextHopInterface.type = CPSS_INTERFACE_PORT_E;
				#if 0
				tblEntry.entry.regularEntry.nextHopInterface.devPort.devNum  = dbItem->dstIntf.intf.port.devNum;
				#else
				/* the devNum must be same to the value read from 0x58, or the nexthop table is worong */
				tblEntry.entry.regularEntry.nextHopInterface.devPort.devNum  = hwDev;
				#endif
				tblEntry.entry.regularEntry.nextHopInterface.devPort.portNum = dbItem->dstIntf.intf.port.portNum;
			}
			else if(NAM_TRUE == dbItem->dstIntf.isTrunk) {
				tblEntry.entry.regularEntry.nextHopInterface.type = CPSS_INTERFACE_TRUNK_E;
				tblEntry.entry.regularEntry.nextHopInterface.trunkId = dbItem->dstIntf.intf.trunk.trunkId;
			}
			/*by zhubo@autelan.com 2008.7.18 TTL Dec enable*/
			tblEntry.entry.regularEntry.ttlHopLimitDecEnable = TRUE;
			/*by shancx@autelan.com 2008.8.07 for counter*/
			tblEntry.entry.regularEntry.countSet= 0;
			tblEntry.entry.regularEntry.appSpecificCpuCodeEnable = TRUE;

#else
			tblEntry.cmd = CPSS_PACKET_CMD_ROUTE_E;
			tblEntry.nextHopARPPointer = macIndex;
			tblEntry.nextHopVlanId = dbItem->vid;
			tblEntry.nextHopInterface.vlanId = dbItem->vid;
			if(NAM_FALSE == dbItem->dstIntf.isTrunk) {
				tblEntry.nextHopInterface.type = CPSS_INTERFACE_PORT_E;
				tblEntry.nextHopInterface.devPort.devNum  = dbItem->dstIntf.intf.port.devNum;
				tblEntry.nextHopInterface.devPort.portNum = dbItem->dstIntf.intf.port.portNum;
			}
			else if(NAM_TRUE == dbItem->dstIntf.isTrunk) {
				tblEntry.nextHopInterface.type = CPSS_INTERFACE_TRUNK_E;
				tblEntry.nextHopInterface.trunkId = dbItem->dstIntf.intf.trunk.trunkId;
			}
			/*by zhubo@autelan.com 2008.7.18 TTL Dec enable*/
			tblEntry.ttlHopLimitDecEnable = NAM_TRUE;
			/*by shancx@autelan.com 2008.8.07 for counter*/
			tblEntry.countSet= 0;
			tblEntry.appSpecificCpuCodeEnable = NAM_TRUE;
#endif
			retVal = cpssDxChIpUcRouteEntriesWrite(devNum,nextHopIndex,&tblEntry,1);
			if(NAM_OK != retVal) {
				syslog_ax_nam_arp_err("nam add next hop table call driver error %d when add",retVal);
				nam_arp_free_mactbl_index(macIndex);
				nam_arp_free_nexthop_tbl_index(nextHopIndex);
				return ARP_RETURN_CODE_NAM_ERROR;
			}
#endif
#ifdef DRV_LIB_BCM
            /*TODO: add Next-Hop item to route Next-Hop Table*/
			retVal = nam_arp_get_nexthop_tbl_index(&nextHopIndex);
			if(ARP_RETURN_CODE_SUCCESS != retVal) {
				syslog_ax_nam_arp_err("nam get next hop table index error %d when add",retVal);
				return retVal;
			}
			for(unit = 0; unit < num_of_asic; unit++){
				memset(&next_hop_entry,0,sizeof(bcm_l3_host_t));
				next_hop_entry.l3a_intf = dbItem->ifIndex;
				next_hop_entry.l3a_ip_addr = dbItem->ipAddr;
				next_hop_entry.l3a_nexthop_hw_idx = nextHopIndex;
				next_hop_entry.l3a_flags = BCM_L3_WITH_ID | BCM_L3_REPLACE;
				if(0 == dbItem->dstIntf.isTrunk) {
					next_hop_entry.l3a_modid = dbItem->dstIntf.intf.port.devNum;
					next_hop_entry.l3a_port_tgid = dbItem->dstIntf.intf.port.portNum;
				}
				else if(1 == dbItem->dstIntf.isTrunk) {
					next_hop_entry.l3a_flags |= BCM_L3_TGID;
					next_hop_entry.l3a_port_tgid = dbItem->dstIntf.intf.trunk.trunkId;
				}			
				memcpy(next_hop_entry.l3a_nexthop_mac,dbItem->mac,6);
				retVal = bcm_l3_host_add(unit,&next_hop_entry);
				if(NAM_OK != retVal) {
					syslog_ax_nam_arp_err("nam add next hop table call driver error %d when add",retVal);
					if(1 == unit){
                        bcm_l3_host_delete(0,&next_hop_entry);
					}
					return ARP_RETURN_CODE_NAM_ERROR;
				}
                if(next_hop_entry.l3a_nexthop_hw_idx != nextHopIndex){
                    syslog_ax_nam_arp_err("next hop input index %d is not equal return index %d\n",nextHopIndex,next_hop_entry.l3a_nexthop_hw_idx);
				}
				else{
                    syslog_ax_nam_arp_dbg("next hop ip %#x add in hw ok,nh index %d\n",next_hop_entry.l3a_ip_addr,next_hop_entry.l3a_nexthop_hw_idx);
				}
			}		
#endif      
			*tblIndex = nextHopIndex;
	  	}
		break;
		
		case NAM_ARP_SNOOP_DEL_ITEM:
		{
#ifdef DRV_LIB_CPSS
			/* TODO: to be implemented*/
			/* first read Next-Hop info to get MAC table index*/
			retVal = cpssDxChIpUcRouteEntriesRead(devNum,*tblIndex,&tblEntry,1);
			if(NAM_OK != retVal) {
				syslog_ax_nam_arp_err("nam read next hop table call driver error %d when del",retVal);
				nam_arp_free_nexthop_tbl_index(*tblIndex);
				return ARP_RETURN_CODE_NAM_ERROR;
			}
#ifdef DRV_LIB_CPSS_3_4
			macIndex = tblEntry.entry.regularEntry.nextHopARPPointer;
#else
			macIndex = tblEntry.nextHopARPPointer;
#endif
			retVal = cpssDxChIpRouterArpAddrRead(devNum,macIndex,&macTblItem);
			if(NAM_OK != retVal) {
				syslog_ax_nam_arp_err("nam read arp mac table call driver error %d when del",retVal);
				nam_arp_free_mactbl_index(macIndex);
				nam_arp_free_nexthop_tbl_index(*tblIndex);
				return ARP_RETURN_CODE_NAM_ERROR;
			}
			syslog_ax_nam_arp_dbg("nam read dev %d mac tbl %d %02x-%02x-%02x-%02x-%02x-%02x when del\n", \
								devNum,macIndex,macTblItem.mac[0],macTblItem.mac[1],	\
								macTblItem.mac[2],macTblItem.mac[3],macTblItem.mac[4],macTblItem.mac[5]);
			memset(&macTblItem,0,sizeof(ethernet_mac));

			retVal = cpssDxChIpRouterArpAddrWrite(devNum,macIndex,&macTblItem);
			if(NAM_OK != retVal) {
				syslog_ax_nam_arp_err("nam write arp mac table call driver error %d when del",retVal);
				nam_arp_free_mactbl_index(macIndex);
				nam_arp_free_nexthop_tbl_index(*tblIndex);
				return ARP_RETURN_CODE_NAM_ERROR;
			}
			syslog_ax_nam_arp_dbg("nam del dev %d mac tbl %d write to 0 ok\n",devNum,macIndex);
			nam_arp_free_mactbl_index(macIndex);

			/* reset next-hop table item*/

#ifdef DRV_LIB_CPSS_3_4
    		memset(&tblEntry,0,sizeof(CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC));
			tblEntry.type = CPSS_DXCH_IP_UC_ROUTE_ENTRY_E;
			tblEntry.entry.regularEntry.cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
			tblEntry.entry.regularEntry.appSpecificCpuCodeEnable = TRUE;
#else
    		memset(&tblEntry,0,sizeof(nam_nexthop_entry_s));
			tblEntry.cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
			tblEntry.appSpecificCpuCodeEnable = NAM_TRUE;
#endif
			retVal = cpssDxChIpUcRouteEntriesWrite(devNum,*tblIndex,&tblEntry,1);
			if(NAM_OK != retVal) {
				syslog_ax_nam_arp_err("nam delete next hop table call driver error %d when write back",retVal);
				nam_arp_free_nexthop_tbl_index(*tblIndex);
				return ARP_RETURN_CODE_NAM_ERROR;
			}
			nam_arp_free_nexthop_tbl_index(*tblIndex);
#endif

#ifdef DRV_LIB_BCM
            /* first read Next-Hop info to get MAC table index*/
            for(unit = 0;unit < num_of_asic;unit++){
				memset(&next_hop_entry,0,sizeof(bcm_l3_host_t));
				next_hop_entry.l3a_ip_addr = dbItem->ipAddr;
				#if 0
				retVal =bcm_l3_host_find(unit,&next_hop_entry);
				/*consider this later,now if not found ,continue.*/
				if(ARP_RETURN_CODE_SUCCESS != retVal) {
					if(NAM_E_NOT_FOUND != retVal){
                       syslog_ax_nam_arp_err("failed to find host %#x,ret %d\n",dbItem->ipAddr,retVal);
					   continue;
					}
					syslog_ax_nam_arp_err("nam read next hop table call driver error %d when del",retVal);
					return retVal;
				}
				#endif
	            retVal = bcm_l3_host_delete(unit,&next_hop_entry);
				if(NAM_OK != retVal) {
					if(NAM_E_NOT_FOUND != retVal){
                       syslog_ax_nam_arp_err("failed to find host %#x,ret %d\n",dbItem->ipAddr,retVal);
					   continue;
					}
					syslog_ax_nam_arp_err("nam delete next hop table call driver error %d when write back",retVal);
					return ARP_RETURN_CODE_NAM_ERROR;
				}
            }
			if(0 != *tblIndex){
			   nam_arp_free_nexthop_tbl_index(*tblIndex);
			}
#endif
		}
		break;
		
		case NAM_ARP_SNOOP_UPDATE_ITEM:
		{
#ifdef DRV_LIB_CPSS
			/* first read Next-Hop info to get MAC table index*/
			retVal = cpssDxChIpUcRouteEntriesRead(devNum,*tblIndex,&tblEntry,1);
			if(NAM_OK != retVal) {
				syslog_ax_nam_arp_err("nam update next hop table call driver error %d when read",retVal);
				return ARP_RETURN_CODE_NAM_ERROR;
			}
#ifdef DRV_LIB_CPSS_3_4
			if(0 != tblEntry.entry.regularEntry.nextHopARPPointer){
				macIndex = tblEntry.entry.regularEntry.nextHopARPPointer;
			}
#else
			if(0 != tblEntry.nextHopARPPointer){
				macIndex = tblEntry.nextHopARPPointer;
			}
#endif
			else {
				retVal = nam_arp_get_mactbl_index(&macIndex);
				if(ARP_RETURN_CODE_SUCCESS != retVal) {
					syslog_ax_nam_arp_err("nam get mac table index error %d when update",retVal);
					return retVal;
				}
				syslog_ax_nam_arp_dbg("nam get mac table index %d when update\n",macIndex);
			}
			
			/* TODO: add arp mac address to Mac Table*/
			retVal = cpssDxChIpRouterArpAddrWrite(devNum,macIndex,&macTblItem);
			if(NAM_OK != retVal) {
				syslog_ax_nam_arp_err("nam add arp mac table call driver error %d",retVal);
				return ARP_RETURN_CODE_NAM_ERROR;
			}

			
			syslog_ax_nam_arp_dbg("nam read dev %d mac tbl %d %02x-%02x-%02x-%02x-%02x-%02x when update\n", \
								devNum,macIndex,macTblItem.mac[0],macTblItem.mac[1],	\
								macTblItem.mac[2],macTblItem.mac[3],macTblItem.mac[4],macTblItem.mac[5]);
			
			/* TODO: update Next-Hop item to route Next-Hop Table*/
			/* build up next-hop table item*/
#ifdef DRV_LIB_CPSS_3_4
			tblEntry.type = CPSS_DXCH_IP_UC_ROUTE_ENTRY_E;
			tblEntry.entry.regularEntry.cmd = CPSS_PACKET_CMD_ROUTE_E;
			tblEntry.entry.regularEntry.nextHopARPPointer = macIndex;
			tblEntry.entry.regularEntry.nextHopVlanId = dbItem->vid;
			tblEntry.entry.regularEntry.nextHopInterface.vlanId = dbItem->vid;	
			if(NAM_FALSE == dbItem->dstIntf.isTrunk) {
				tblEntry.entry.regularEntry.nextHopInterface.type = CPSS_INTERFACE_PORT_E;
				#if 0
				tblEntry.entry.regularEntry.nextHopInterface.devPort.devNum  = dbItem->dstIntf.intf.port.devNum;
				#else
				/* the devNum must be same to the value read from 0x58, or the nexthop table is worong */
				tblEntry.entry.regularEntry.nextHopInterface.devPort.devNum  = hwDev;
				#endif

				tblEntry.entry.regularEntry.nextHopInterface.devPort.portNum = dbItem->dstIntf.intf.port.portNum;
			}
			else if(NAM_TRUE == dbItem->dstIntf.isTrunk) {
				tblEntry.entry.regularEntry.nextHopInterface.type = CPSS_INTERFACE_TRUNK_E;
				tblEntry.entry.regularEntry.nextHopInterface.trunkId = dbItem->dstIntf.intf.trunk.trunkId;
			}
			/*by zhubo@autelan.com 2008.7.18 TTL Dec enable*/
			tblEntry.entry.regularEntry.ttlHopLimitDecEnable = 1;
			/*by shancx@autelan.com 2008.8.07 for counter*/
			tblEntry.entry.regularEntry.countSet= 0;
			tblEntry.entry.regularEntry.appSpecificCpuCodeEnable = TRUE;
#else
			tblEntry.cmd = CPSS_PACKET_CMD_ROUTE_E;
			tblEntry.nextHopARPPointer = macIndex;
			tblEntry.nextHopVlanId = dbItem->vid;
			tblEntry.nextHopInterface.vlanId = dbItem->vid;
			if(NAM_FALSE == dbItem->dstIntf.isTrunk) {
				tblEntry.nextHopInterface.type = CPSS_INTERFACE_PORT_E;
				tblEntry.nextHopInterface.devPort.devNum  = dbItem->dstIntf.intf.port.devNum;
				tblEntry.nextHopInterface.devPort.portNum = dbItem->dstIntf.intf.port.portNum;
			}
			else if(NAM_TRUE == dbItem->dstIntf.isTrunk) {
				tblEntry.nextHopInterface.type = CPSS_INTERFACE_TRUNK_E;
				tblEntry.nextHopInterface.trunkId = dbItem->dstIntf.intf.trunk.trunkId;
			}
			/*by zhubo@autelan.com 2008.7.18 TTL Dec enable*/
			tblEntry.ttlHopLimitDecEnable = 1;
			/*by shancx@autelan.com 2008.8.07 for counter*/
			tblEntry.countSet= 0;
			tblEntry.appSpecificCpuCodeEnable = NAM_TRUE;
#endif
			nextHopIndex = *tblIndex;
			
			retVal = cpssDxChIpUcRouteEntriesWrite(devNum,nextHopIndex,&tblEntry,1);
			if(NAM_OK != retVal) {
				syslog_ax_nam_arp_err("nam update next hop table call driver error %d when update",retVal);
				return ARP_RETURN_CODE_NAM_ERROR;
			}
#endif

#ifdef DRV_LIB_BCM
            for(unit = 0;unit < num_of_asic;unit++){
				memset(&next_hop_entry,0,sizeof(bcm_l3_host_t));
				next_hop_entry.l3a_intf = dbItem->ifIndex;
				next_hop_entry.l3a_ip_addr = dbItem->ipAddr;
				next_hop_entry.l3a_nexthop_hw_idx = *tblIndex;
				next_hop_entry.l3a_flags = BCM_L3_WITH_ID | BCM_L3_REPLACE;
				if(0 == dbItem->dstIntf.isTrunk) {
					next_hop_entry.l3a_modid = dbItem->dstIntf.intf.port.devNum;
					next_hop_entry.l3a_port_tgid = dbItem->dstIntf.intf.port.portNum;
				}
				else if(1 == dbItem->dstIntf.isTrunk) {
					next_hop_entry.l3a_flags |= BCM_L3_TGID;
					next_hop_entry.l3a_port_tgid = dbItem->dstIntf.intf.trunk.trunkId;
				}			
				memcpy(next_hop_entry.l3a_nexthop_mac,dbItem->mac,6);
				retVal = bcm_l3_host_add(unit,&next_hop_entry);
				if(NAM_OK != retVal) {
					syslog_ax_nam_arp_err("nam add next hop table call driver error %d when add",retVal);
					if(1 == unit){
                        bcm_l3_host_delete(0,&next_hop_entry);
					}
					return ARP_RETURN_CODE_NAM_ERROR;
				}

			}
#endif
		}
		break;

		case NAM_ARP_SNOOP_OCCUPY_ITEM:
		{
			/* build up next-hop table item*/
			/* TODO: occupy Next-Hop item to route Next-Hop Table*/
			retVal = nam_arp_get_nexthop_tbl_index(&nextHopIndex);
			if(ARP_RETURN_CODE_SUCCESS != retVal) {
				syslog_ax_nam_arp_err("nam get next hop table index error %d when occupy",retVal);
				return retVal;
			}
			syslog_ax_nam_arp_dbg("nam get next hop table index %d when occupy\n",nextHopIndex);			
#ifdef DRV_LIB_CPSS
#ifdef DRV_LIB_CPSS_3_4
    		memset(&tblEntry,0,sizeof(CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC));
			tblEntry.type = CPSS_DXCH_IP_UC_ROUTE_ENTRY_E;
			tblEntry.entry.regularEntry.cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
			tblEntry.entry.regularEntry.ttlHopLimitDecEnable = TRUE;
			tblEntry.entry.regularEntry.appSpecificCpuCodeEnable = TRUE;
#else
    		memset(&tblEntry,0,sizeof(nam_nexthop_entry_s));
			tblEntry.cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
			tblEntry.ttlHopLimitDecEnable = NAM_TRUE;
			tblEntry.appSpecificCpuCodeEnable = NAM_TRUE;
#endif
			retVal = cpssDxChIpUcRouteEntriesWrite(devNum,nextHopIndex,&tblEntry,1);
			if(NAM_OK != retVal) {
				syslog_ax_nam_arp_err("nam add next hop table call driver error %d when occupy",retVal);
				return ARP_RETURN_CODE_NAM_ERROR;
			}
			*tblIndex = nextHopIndex;	
#endif
              
#ifdef DRV_LIB_BCM
            for(unit = 0;unit < num_of_asic;unit++){
				memset(&next_hop_entry,0,sizeof(bcm_l3_host_t));
	            bcm_l3_host_t_init(&next_hop_entry);
				next_hop_entry.l3a_ip_addr = dbItem->ipAddr;
				next_hop_entry.l3a_intf = dbItem->ifIndex;
				next_hop_entry.l3a_nexthop_hw_idx = nextHopIndex;
				next_hop_entry.l3a_flags = BCM_L3_L2TOCPU | BCM_L3_WITH_ID|BCM_L3_REPLACE;
                retVal = bcm_l3_host_add(unit,&next_hop_entry);
				if(NAM_OK != retVal) {
					syslog_ax_nam_arp_err("nam add next hop table call driver error %d when occupy on unit %d",retVal,unit);
					return ARP_RETURN_CODE_NAM_ERROR;
				}
                if(next_hop_entry.l3a_nexthop_hw_idx != nextHopIndex){
                    nam_syslog_err("occupy input nh index %d not equal get %d\n",nextHopIndex,next_hop_entry.l3a_nexthop_hw_idx);
					return ARP_RETURN_CODE_NAM_ERROR;
				}
			}     
			*tblIndex = nextHopIndex;
#endif		
		}
		break;
		/**
		  *	Difference between DELETE and RESET is that Next-Hop table index is not returned back
		  *	Next time new ARP info learned will overwrite it.
		  *	This is to protect Route from being disturbed when ARP used as route next-hop
		  */
		case NAM_ARP_SNOOP_RESET_ITEM:
		{
#ifdef DRV_LIB_CPSS
			/* TODO: to be implemented*/
			/* first read Next-Hop info to get MAC table index*/
			retVal = cpssDxChIpUcRouteEntriesRead(devNum,*tblIndex,&tblEntry,1);
			if(NAM_OK != retVal) {
				syslog_ax_nam_arp_err("nam read next hop table call driver error %d when reset",retVal);
				return ARP_RETURN_CODE_NAM_ERROR;
			}
#ifdef DRV_LIB_CPSS_3_4
			macIndex = tblEntry.entry.regularEntry.nextHopARPPointer;
#else
			macIndex = tblEntry.nextHopARPPointer;
#endif
			retVal = cpssDxChIpRouterArpAddrRead(devNum,macIndex,&macTblItem);
			if(NAM_OK != retVal) {
				syslog_ax_nam_arp_err("nam read arp mac table call driver error %d when reset",retVal);
				/*nam_arp_free_mactbl_index(macIndex);*/
				return ARP_RETURN_CODE_NAM_ERROR;
			}
			syslog_ax_nam_arp_dbg("nam read dev %d mac tbl %d %02x-%02x-%02x-%02x-%02x-%02x when reset\n", \
								devNum,macIndex,macTblItem.mac[0],macTblItem.mac[1],	\
								macTblItem.mac[2],macTblItem.mac[3],macTblItem.mac[4],macTblItem.mac[5]);
			memset(&macTblItem,0,sizeof(ethernet_mac));
			retVal = cpssDxChIpRouterArpAddrWrite(devNum,macIndex,&macTblItem);
			if(NAM_OK != retVal) {
				syslog_ax_nam_arp_err("nam reset arp mac table call driver error %d when write",retVal);
			}
			else {
				syslog_ax_nam_arp_dbg("nam reset dev %d mac tbl %d write to 0 ok\n",devNum,macIndex);
			}
			nam_arp_free_mactbl_index(macIndex);

			/* reset next-hop table item*/
#ifdef DRV_LIB_CPSS_3_4
    		memset(&tblEntry,0,sizeof(CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC));
			tblEntry.type = CPSS_DXCH_IP_UC_ROUTE_ENTRY_E;
			tblEntry.entry.regularEntry.cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
			tblEntry.entry.regularEntry.appSpecificCpuCodeEnable = TRUE;
#else
    		memset(&tblEntry,0,sizeof(nam_nexthop_entry_s));
			tblEntry.cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
			tblEntry.appSpecificCpuCodeEnable = NAM_TRUE;
#endif
			retVal = cpssDxChIpUcRouteEntriesWrite(devNum,*tblIndex,&tblEntry,1);
			if(NAM_OK != retVal) {
				syslog_ax_nam_arp_err("nam reset next hop table call driver error %d when write back",retVal);
				/*nam_arp_free_mactbl_index(macIndex);*/
				return ARP_RETURN_CODE_NAM_ERROR;
			}
#endif

#ifdef DRV_LIB_BCM
            for(unit = 0;unit < num_of_asic;unit++){
				memset(&next_hop_entry,0,sizeof(bcm_l3_host_t));
				next_hop_entry.l3a_ip_addr = dbItem->ipAddr;
				retVal = bcm_l3_host_find(unit,&next_hop_entry);
				if(NAM_OK != retVal){
                    nam_syslog_dbg("Reset nh,not find host %#x on unit %d,ret %d\n",dbItem->ipAddr,unit,retVal);
					continue;
				}
				nextHopIndex = next_hop_entry.l3a_nexthop_hw_idx;
				memset(&next_hop_entry,0,sizeof(bcm_l3_host_t));
				next_hop_entry.l3a_ip_addr = dbItem->ipAddr;
				next_hop_entry.l3a_intf = dbItem->ifIndex;
				next_hop_entry.l3a_nexthop_hw_idx = nextHopIndex;
				next_hop_entry.l3a_modid = unit;
				next_hop_entry.l3a_flags = BCM_L3_L2TOCPU |BCM_L3_WITH_ID |BCM_L3_REPLACE;
			    retVal = bcm_l3_host_add(unit,&next_hop_entry);
				if(NAM_OK != retVal) {
					syslog_ax_nam_arp_err("nam read next hop table call driver error %d when reset on unit %d\n",retVal,unit);
					return ARP_RETURN_CODE_NAM_ERROR;
				} 
				nam_syslog_dbg("reset next hop %#x, nh index get %d\n",next_hop_entry.l3a_ip_addr,next_hop_entry.l3a_nexthop_hw_idx);
			}
#endif
		}
		break;

		default:
		{
			syslog_ax_nam_arp_err("nam bad action %d specified",action);
			retVal = ARP_RETURN_CODE_BADPARAM;
		}
	}
	if(NAM_OK == retVal){
		retVal = ARP_RETURN_CODE_SUCCESS;
	}
	return retVal;
}


/**********************************************************************************
 * nam_arp_snooping_op_item_encaps
 *
 * ARP snooping database add to/delete from ASIC or other operations.
 *
 *	INPUT:
 *		item - ARP snooping DB items
 *		action - add or delete operation
 *		tblIndex - used when action is NAM_ARP_SNOOP_UPDATE_ITEM
 *	
 *	OUTPUT:
 *		tblIndex - next-hop table index which used to hold next-hop item
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS - if no error occurred
 *		COMMON_RETURN_CODE_NULL_PTR - if input parameters have null pointer
 *		ARP_RETURN_CODE_BADPARAM - if parameters wrong
 *		ARP_RETURN_CODE_TBLINDEX_GET_FAILED - table index get failed 
 *		ARP_RETURN_CODE_NAM_ERROR - asic operation failed
 *		
 *	NOTE:
 *		This is just used for occupy one item
 *
 **********************************************************************************/

int nam_arp_snooping_op_item_encaps
(
	IN 	struct arp_snooping_item_s *dbItem,
	IN 	unsigned int action,
	INOUT	unsigned int *tblIndex
)
{
  int ret = 0;
  if(action == NAM_ARP_SNOOP_OCCUPY_ITEM){
#ifdef DRV_LIB_CPSS
        ret =  nam_arp_snooping_op_item(NULL,action,tblIndex);
		return ret;
#endif
  }
     ret = nam_arp_snooping_op_item(dbItem,action,tblIndex);
     return ret;
}

/**********************************************************************************
 * nam_arp_snooping_get_item
 *
 * Get route next-hop table item by index
 *
 *	INPUT:
 *		devNum - device number
 *		tblIndex - table index to Next-Hop table
 *
 *	OUTPUT:
 *		val - arp snooping item info
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS - if no error occurred
 *		ARP_RETURN_CODE_NAM_ERROR - asic operation failed
 *		ARP_RETURN_CODE_ACTION_TRAP2CPU - the cmd is trap to cpu
 *		ARP_RETURN_CODE_ACTION_HARD_DROP - the cmd is drop
 *		
 *	NOTE:
 *
 **********************************************************************************/
int nam_arp_snooping_get_item
(
	IN	unsigned char devNum,
	IN	unsigned int  tblIndex,
	OUT struct arp_snooping_item_s *val
)
{
	unsigned int retVal = ARP_RETURN_CODE_SUCCESS;
#ifdef DRV_LIB_CPSS
	ethernet_mac arpMacAddr;
	unsigned int macTblIndex = 0;
#ifdef DRV_LIB_CPSS_3_4
	CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC entry;
    memset(&entry,0,sizeof(CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC));
	entry.type = CPSS_DXCH_IP_UC_ROUTE_ENTRY_E;
#else
	nam_nexthop_entry_s entry;
	memset(&entry,0,sizeof(nam_nexthop_entry_s));
#endif
	CPSS_INTERFACE_TYPE_ENT type = CPSS_INTERFACE_PORT_E;	
#endif
#ifdef DRV_LIB_BCM
	int unit = 0,num_of_asic = 0,flag = 0;
	bcm_l3_egress_t nh_idx;
	unsigned char tmpmac[6];
	memset(tmpmac,0xff,sizeof(unsigned char)*6);
	num_of_asic = nam_asic_get_instance_num();
#endif

#ifdef DRV_LIB_CPSS
	retVal = cpssDxChIpUcRouteEntriesRead(devNum,tblIndex,&entry,1);
#ifdef DRV_LIB_CPSS_3_4
syslog_ax_nam_arp_dbg("read nexthop table entry %d cmd %d\n", \
		tblIndex, entry.entry.regularEntry.cmd);
#else
	syslog_ax_nam_arp_dbg("read nexthop table entry %d cmd %d\n", \
			tblIndex, entry.cmd);
#endif
	if(NAM_OK != retVal) {
		syslog_ax_nam_arp_err("nam arp snooping read %s next-hop table entry %d error %x", \
						(0 == devNum) ? "98Dx275":"98Dx804",tblIndex,retVal);
		return ARP_RETURN_CODE_NAM_ERROR;
	}
#ifdef DRV_LIB_CPSS_3_4
if(CPSS_PACKET_CMD_TRAP_TO_CPU_E == entry.entry.regularEntry.cmd) {
	return ARP_RETURN_CODE_ACTION_TRAP2CPU;
}
else if(CPSS_PACKET_CMD_DROP_HARD_E == entry.entry.regularEntry.cmd){
	return ARP_RETURN_CODE_ACTION_HARD_DROP;
}

val->vid = entry.entry.regularEntry.nextHopVlanId;
type = entry.entry.regularEntry.nextHopInterface.type;
switch(type) {
	case CPSS_INTERFACE_PORT_E:
		val->dstIntf.isTrunk = FALSE;
		val->dstIntf.intf.port.devNum	= entry.entry.regularEntry.nextHopInterface.devPort.devNum;
		val->dstIntf.intf.port.portNum	= entry.entry.regularEntry.nextHopInterface.devPort.portNum;
		break;
	case CPSS_INTERFACE_TRUNK_E:
		val->dstIntf.isTrunk = TRUE;
		val->dstIntf.intf.trunk.trunkId = entry.entry.regularEntry.nextHopInterface.trunkId;
		break;
	case CPSS_INTERFACE_VID_E:
		val->vid = entry.entry.regularEntry.nextHopInterface.vlanId;
		break;
	case CPSS_INTERFACE_VIDX_E:
		val->vidx = entry.entry.regularEntry.nextHopInterface.vidx;
		break;
}

macTblIndex = entry.entry.regularEntry.nextHopARPPointer;

#else
	if(CPSS_PACKET_CMD_TRAP_TO_CPU_E == entry.cmd) {
		return ARP_RETURN_CODE_ACTION_TRAP2CPU;
	}
	else if(CPSS_PACKET_CMD_DROP_HARD_E == entry.cmd){
		return ARP_RETURN_CODE_ACTION_HARD_DROP;
	}
	
	val->vid = entry.nextHopVlanId;
	type = entry.nextHopInterface.type;
	switch(type) {
		case CPSS_INTERFACE_PORT_E:
			val->dstIntf.isTrunk = FALSE;
			val->dstIntf.intf.port.devNum	= entry.nextHopInterface.devPort.devNum;
			val->dstIntf.intf.port.portNum	= entry.nextHopInterface.devPort.portNum;
			break;
		case CPSS_INTERFACE_TRUNK_E:
			val->dstIntf.isTrunk = TRUE;
			val->dstIntf.intf.trunk.trunkId = (unsigned char)entry.nextHopInterface.trunkId;			
			break;
		case CPSS_INTERFACE_VID_E:
			val->vid = entry.nextHopInterface.vlanId;
			break;
		case CPSS_INTERFACE_VIDX_E:
			val->vidx = entry.nextHopInterface.vidx;
			break;
	}

	macTblIndex = entry.nextHopARPPointer;
#endif
	retVal = cpssDxChIpRouterArpAddrRead(devNum,macTblIndex,&arpMacAddr);
	if(NAM_OK != retVal) {
		syslog_ax_nam_arp_err("nam arp snooping read %s mac table entry %d error %x", \
						(0 == devNum) ? "98Dx275":"98Dx804",macTblIndex,retVal);
		return ARP_RETURN_CODE_NAM_ERROR;
	}
	memcpy(val->mac,arpMacAddr.mac,MAC_ADDR_LEN);
	return ARP_RETURN_CODE_SUCCESS;
#endif	

#ifdef DRV_LIB_BCM
    for(unit = 0;unit < num_of_asic;unit++){
	   memset(&nh_idx,0,sizeof(bcm_l3_egress_t));
       retVal = bcm_l3_egress_get(unit,tblIndex+ARP_EGRESS_IDX_MIN,&nh_idx);
	   if(NAM_E_NONE != retVal){
            nam_syslog_err("get nh entry failed,idx %d,ret %d\n",tblIndex,retVal);
			return ARP_RETURN_CODE_NAM_ERROR;
	   }
	   val->ifIndex = nh_idx.intf;
	   val->vid = nh_idx.vlan;
	   memcpy(val->mac,nh_idx.mac_addr,6);
	   if( !(nh_idx.flags & NAM_L3_TGID)){
		 val->dstIntf.isTrunk = FALSE;
         val->dstIntf.intf.port.devNum = nh_idx.module;
		 val->dstIntf.intf.port.portNum =  nh_idx.port;
	   }
	   else{
	  	 val->dstIntf.isTrunk = TRUE;
         val->dstIntf.intf.trunk.devNum = nh_idx.module;
		 val->dstIntf.intf.trunk.trunkId =  nh_idx.trunk;
	   }
	}
	if((0 == tblIndex)||((!memcmp(nh_idx.mac_addr,tmpmac,6))&&(nh_idx.port == 0))){/*has been reset or just occupied*/
         return ARP_RETURN_CODE_ACTION_TRAP2CPU;
	}
	return ARP_RETURN_CODE_SUCCESS;	
#endif
	return ARP_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 * nam_arp_solicit_send
 *	Send arp solicit packet via cpss driver
 * 
 *
 *	INPUT:
 *		item - arp snooping item info
 *		sysMac - system mac address
 *		gateway - L3 interface ip address (as source ip)
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS   - on success
 *		COMMON_RETURN_CODE_NULL_PTR - if null pointer found
 *		COMMON_RETURN_CODE_NO_RESOURCE - malloc failed
 *		ARP_RETURN_CODE_NAM_ERROR - asic operation failed
 *
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int nam_arp_solicit_send
(
	struct arp_snooping_item_s *item,
	unsigned char *sysMac,
	unsigned int gateway
)
{
	unsigned int result = ARP_RETURN_CODE_SUCCESS;
	unsigned char devNum = 0,portNum = 0, isTagged = 0,trunkId = 0;
	unsigned short vid = 0;
	unsigned char bcast[MAC_ADDR_LEN] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	unsigned int sip = 0, dip = 0;
	unsigned char destMac[MAC_ADDR_LEN]={0};
	
	if(NULL == item) {
		return COMMON_RETURN_CODE_NULL_PTR;
	}

	if(item->dstIntf.isTrunk) {
		trunkId = item->dstIntf.intf.trunk.trunkId;
	}
	else {
		devNum	= item->dstIntf.intf.port.devNum;
		portNum = item->dstIntf.intf.port.portNum;
	}
	vid = item->vid;
	isTagged = item->isTagged;
	sip = gateway;
	dip = item->ipAddr;
	if(NPD_TRUE != arp_aging_bcast){
	    memcpy(destMac,item->mac,MAC_ADDR_LEN);
	}
	else {
		memcpy(destMac,bcast,MAC_ADDR_LEN);
	}
	
	syslog_ax_nam_arp_dbg("send arp solicit (%d,%d) vid %d %02x:%02x:%02x:%02x:%02x:%02x %d.%d.%d.%d\n", \
			devNum,portNum,vid,destMac[0],destMac[1],destMac[2],destMac[3],destMac[4],destMac[5],	\
			(dip>>24)&0xFF,(dip>>16)&0xFF,(dip>>8)&0xFF,dip&0xFF);
	result = nam_arp_solicit_drv_send(devNum,portNum,vid,isTagged,sysMac,destMac,sip,dip);

	return result;
}

/**********************************************************************************
 * nam_arp_smac_check_enable
 *
 *	set if enable to excute smac check or not for arp
 *
 *	INPUT:
 *		isenable - binary value to indicate enable or disable
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS - if no error occurred
 *		ARP_RETURN_CODE_NAM_ERROR -asic set failed
 *
 **********************************************************************************/

unsigned int nam_arp_smac_check_enable
(
	unsigned int isenable
)
{
	unsigned char devNum = 0;
	int ret = ARP_RETURN_CODE_SUCCESS;
#ifdef DRV_LIB_CPSS	
    ret = cpssDxChBrgGenArpSaCheckEnable(devNum,isenable);
	if(NAM_OK != ret) {
		syslog_ax_nam_arp_err("smac check set enable error %d\n ",ret);
		return ARP_RETURN_CODE_NAM_ERROR;
	}
#endif	
	return ret;
}

/**********************************************************************************
 * nam_arp_aging_dest_mac_broadcast
 *
 *	set if enable to excute smac check or not for arp
 *
 *	INPUT:
 *		isBroadCast - value to indicate broadcast or unicast
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS 
 *
 **********************************************************************************/

unsigned int nam_arp_aging_dest_mac_broadcast
(
	unsigned int isBroadCast
)
{
	int ret = ARP_RETURN_CODE_SUCCESS;	
    arp_aging_bcast = isBroadCast;
	return ret;
}
/**
 *
 * RETURN:
 *		ARP_RETURN_CODE_SUCCESS - if no error occurred
 *		ARP_RETURN_CODE_TBLINDEX_GET_FAILED - index get failed
 **/
int nam_arp_mactbl_index_get
(
    index_elem val
)
{
    unsigned int retVal = 0;
	retVal = nam_index_get(mac_table_index, val);
	if(NAM_OK != retVal){
		syslog_ax_nam_arp_err("nam route nexthop table index %d get failed!\n",val);
		return ARP_RETURN_CODE_TBLINDEX_GET_FAILED;
	}
	syslog_ax_nam_arp_dbg("nam arp mac table index  get  %d ", val);
	return ARP_RETURN_CODE_SUCCESS;	
}
/********************************************************
 *nam_arp_broadcast_to_cpu_cmd_set
 *
 *  ARPBC CMD set 
 * INPUT:
 *      devNum  -- dev num
 *      cmd  -- cmd PACKET_CMD_FORWARD_E, 
 *                        PACKET_CMD_MIRROR_TO_CPU_E,
 *                        PACKET_CMD_TRAP_TO_CPU_E
 *OUTPUT:
 *     NONE
 *RETURN:
 *     ARP_RETURN_CODE_NAM_ERROR    -- 0     successed
 *     ARP_RETURN_CODE_NAM_ERROR   --1      failed
 ********************************************************/
unsigned int nam_arp_broadcast_to_cpu_cmd_set
(
    unsigned int devNum,
    unsigned int cmd
)
{
#ifdef DRV_LIB_CPSS
    if(NAM_OK != cpssDxChBrgGenArpBcastToCpuCmdSet(devNum,cmd)){
		nam_syslog_err("arp broadcast to cpu cmd %d set error\n",cmd);
		return ARP_RETURN_CODE_NAM_ERROR;
    }
	
	nam_syslog_dbg("arp broadcast to cpu cmd %d set success\n",cmd);
	return ARP_RETURN_CODE_SUCCESS;
#endif
#ifdef DRV_LIB_BCM
    return ARP_RETURN_CODE_NAM_ERROR;
#endif
}

#ifdef __cplusplus
}
#endif
