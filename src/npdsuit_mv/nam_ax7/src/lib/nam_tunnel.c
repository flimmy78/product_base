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
* nam_tunnel.c
*
*
* CREATOR:
*		qinhs@autelan.com
*
* DESCRIPTION:
*		APIs used in NPD for TUNNEL module driver process.
*
* DATE:
*		05/01/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.12 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "util/npd_list.h"
#include "sysdef/npd_sysdef.h"
#include "npd/nam/npd_amapi.h"
#include "npd/nbm/npd_bmapi.h"
#include "dbus/npd/npd_dbus_def.h"
#include "nam_tunnel.h"

#ifdef DRV_LIB_CPSS
#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpmTypes.h>
#include <cpss/dxCh/dxChxGen/tunnel/cpssDxChTunnel.h>
#include <cpss/generic/tunnel/cpssGenTunnelTypes.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpTypes.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwTables.h>
#ifdef DRV_LIB_CPSS_3_4
#include <cpss/dxCh/dxChxGen/tti/cpssDxChTtiTypes.h>
#include <cpss/dxCh/dxChxGen/tti/cpssDxChTti.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#else

#endif
#endif

extern unsigned char gIndex;
extern pthread_mutex_t semRtRwMutex;

int nam_tunnel_init
(
	void
)
{
	return 0;
}

int nam_tunnel_start_set_entry
(
	unsigned char devNum,
	unsigned int routerArpTunnelStartLineIndex,
	unsigned int tunnelType,
	struct tunnel_item_s	*configPtr
)
{
#ifdef DRV_LIB_CPSS
	int ret = 0, i = 0;
	CPSS_TUNNEL_TYPE_ENT tunneltype;
	CPSS_DXCH_TUNNEL_START_CONFIG_UNT	*configptr = NULL;

	if (NULL == configPtr) {
		nam_syslog_err("configPtr is NULL \n");
		return 1;
	}
	
	configptr = (CPSS_DXCH_TUNNEL_START_CONFIG_UNT *)malloc(sizeof(CPSS_DXCH_TUNNEL_START_CONFIG_UNT));
	if (NULL == configptr) {
		nam_syslog_err("configptr malloc fail \n");
		return 1;
	}
	memset(configptr, 0, sizeof(CPSS_DXCH_TUNNEL_START_CONFIG_UNT));
		
	tunneltype = CPSS_TUNNEL_X_OVER_IPV4_E;
	configptr->ipv4Cfg.srcIp.u32Ip = configPtr->kmsg.srcip;
	configptr->ipv4Cfg.destIp.u32Ip = configPtr->kmsg.dstip;
	configptr->ipv4Cfg.vlanId = configPtr->kmsg.vid;
	for (i = 0; i < 6; i++) {
		configptr->ipv4Cfg.macDa.arEther[i] = configPtr->kmsg.mac[i];
	}
		
	ret = cpssDxChTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunneltype, configptr);
	if (0 != ret) {
		nam_syslog_err("cpssDxChTunnelStartEntrySet fail ! ret is %d \n", ret);
		free(configptr);
		return 1;
	}

	free(configptr);	
#endif	
	return 0;
}

int nam_tunnel_start_del_entry
(
	unsigned char devNum,
	unsigned int routerArpTunnelStartLineIndex,
	unsigned int tunnelType
)
{
#ifdef DRV_LIB_CPSS
	int ret = 0;
	CPSS_TUNNEL_TYPE_ENT tunneltype;
	CPSS_DXCH_TUNNEL_START_CONFIG_UNT	*configptr = NULL;

	configptr = (CPSS_DXCH_TUNNEL_START_CONFIG_UNT *)malloc(sizeof(CPSS_DXCH_TUNNEL_START_CONFIG_UNT));
	if (NULL == configptr) {
		nam_syslog_err("configptr malloc fail \n");
		return 1;
	}
	memset(configptr, 0, sizeof(CPSS_DXCH_TUNNEL_START_CONFIG_UNT));
	
	tunneltype = CPSS_TUNNEL_X_OVER_IPV4_E;
		
	ret = cpssDxChTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunneltype, configptr);
	if (0 != ret) {
		nam_syslog_err("cpssDxChTunnelStartEntrySet fail ! ret is %d \n", ret);
		free(configptr);
		return 1;
	}

	free(configptr);
#endif	
	return 0;
}

int nam_tunnel_nh_set
(
	unsigned char devNum,
	struct tunnel_item_s	*configPtr
)
{
#ifdef DRV_LIB_CPSS
	int ret = 0, i = 0;
	unsigned int   nextHopIndex = 0;
	CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC *tblEntry = NULL;
nam_syslog_dbg("nam nh run here \n");	

	tblEntry = (CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC *)malloc(sizeof(CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC));
	if (NULL == tblEntry) {
		nam_syslog_err("configptr malloc fail \n");
		return 1;
	}
	memset(tblEntry, 0, sizeof(CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC));
nam_syslog_dbg("nam nh run here \n");	

	nextHopIndex = configPtr->nhindex;
	nam_syslog_dbg("nam get next hop table index %d when add", nextHopIndex);
	/* build up next-hop table item*/
/*struct changed so wait for nam_arp.c*/
#ifdef DRV_LIB_CPSS_3_4	

#else
	tblEntry->nextHopInterface.type = 0; /*CPSS_INTERFACE_PORT_E;*/
	tblEntry->nextHopInterface.devPort.devNum  = configPtr->kmsg.devnum;
	tblEntry->nextHopInterface.devPort.portNum = configPtr->kmsg.portnum;
	tblEntry->cmd = CPSS_PACKET_CMD_ROUTE_E;
	tblEntry->nextHopVlanId = configPtr->kmsg.vid;
	tblEntry->nextHopInterface.vlanId = configPtr->kmsg.vid;
	tblEntry->isTunnelStart = 1;
	tblEntry->nextHopTunnelPointer = configPtr->tsindex;
	/* by zhubo@autelan.com 2008.7.18 TTL Dec enable*/
	tblEntry->ttlHopLimitDecEnable = 1;
	/* by shancx@autelan.com 2008.8.07 for counter*/
	tblEntry->countSet= 0;
	tblEntry->appSpecificCpuCodeEnable = 1;
nam_syslog_dbg("nam_tunnel_nh_set vid is %d, tsindex is %d \n", configPtr->kmsg.vid, configPtr->tsindex);	
#endif
	ret = cpssDxChIpUcRouteEntriesWrite(devNum, nextHopIndex, tblEntry, 1);
	if(0 != ret) {
		nam_syslog_err("nam add next hop table call driver error %d when add", ret);
		/*nam_arp_free_mactbl_index(macIndex);*/
		/*nam_arp_free_nexthop_tbl_index(nextHopIndex);*/
		free(tblEntry);
		return 1;
	}

	free(tblEntry);
#endif	
	return 0;
}

int nam_tunnel_nh_del
(
	unsigned char devNum,
	unsigned int  nexthopindex
)
{
#ifdef DRV_LIB_CPSS
	unsigned int    ret = 0, nextHopIndex = 0;;
	CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC *tblEntry = NULL;

	/* reset next-hop table item*/
	tblEntry = (CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC *)malloc(sizeof(CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC));
	if (NULL == tblEntry) {
		nam_syslog_err("tblEntry malloc fail \n");
		return 1;
	}
	memset(tblEntry, 0, sizeof(CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC));
	nextHopIndex = nexthopindex;
	nam_syslog_dbg("nam get next hop table index %d when add", nextHopIndex);
#if 0	
	tblEntry->cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
	tblEntry->appSpecificCpuCodeEnable = 1;
#endif	
	ret = cpssDxChIpUcRouteEntriesWrite(devNum, nextHopIndex, tblEntry, 1);
	if(0 != ret) {
		nam_syslog_err("cpssDxChIpUcRouteEntriesWrite call driver error %d when write back", ret);
		free(tblEntry);
		return 1;
	}
	
	free(tblEntry);
#endif	
	return 0;
}

int nam_tunnel_set_tcam_ipltt
(
	unsigned char devnum,
	unsigned int hostdip,
	unsigned int hdiplen,
	struct tunnel_item_s	*configPtr
)
{
#ifdef DRV_LIB_CPSS
	unsigned int ret = 0, lpmDBId = 0, vrId = 0, prefixLen = 0;
	GT_BOOL 								   override = 1;
	unsigned int								 ipAddr = 0;
	CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT	 *ipltt = NULL;
nam_syslog_dbg("nam_tunnel_set_tcam_ipltt run here \n");
	/* set route tcam table and ltt table*/
    	lpmDBId = 0; 
    	vrId = 0;
	ipAddr = hostdip; /*wait for kernel msg*/
    	prefixLen = 32; /*wait for kernel msg and len is 32 for host    hdiplen*/
    	ipltt = (CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT *)malloc(sizeof(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT));
	if (NULL == ipltt) {
		nam_syslog_err("ipltt malloc fail \n");
		return 1;
	}
	memset(ipltt, 0, sizeof(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT));
nam_syslog_dbg("nam_tunnel_set_tcam_ipltt run here \n");
		nam_syslog_dbg("ipAddr is 0x%x, len is %d \n", ipAddr, prefixLen);	
    	ipltt->ipLttEntry.routeType = 0; /*CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E*/
    	ipltt->ipLttEntry.numOfPaths = 0;
    	ipltt->ipLttEntry.routeEntryBaseIndex = configPtr->nhindex;
    	ipltt->ipLttEntry.ucRPFCheckEnable = 0; /* ucRPFCheckEnable*/
    	ipltt->ipLttEntry.sipSaCheckMismatchEnable = 0;
    	ipltt->ipLttEntry.ipv6MCGroupScopeLevel = 0;
nam_syslog_dbg("nam_tunnel_set_tcam_ipltt run here \n");

    	/* call cpss api function */
	pthread_mutex_lock(&semRtRwMutex);
    	ret =  cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, ipltt, override);
	pthread_mutex_unlock(&semRtRwMutex);

	if (0 != ret) {
		nam_syslog_err("cpssDxChIpLpmIpv4UcPrefixAdd ret is %d\n", ret);
		free(ipltt);
		return 1;
	}
nam_syslog_dbg("nam_tunnel_set_tcam_ipltt run here and sucess \n");

	gIndex++;

	free(ipltt);
#endif	
	return 0;
}

int  nam_tunnel_del_tcam_ipltt
(
	unsigned int dstip,
	unsigned int masklen
)
{
#ifdef DRV_LIB_CPSS
	int   ret = 0;
	/* not need because no default route here 
	if (0 == dstip + masklen){ //if default route,override to init value
		ret = nam_route_override_default_route(dstip);
		if(0 != ret){
			return ret;
		}
	}
	else {
	*/
	pthread_mutex_lock(&semRtRwMutex);
	ret = cpssDxChIpLpmIpv4UcPrefixDel(0, 0, dstip, masklen);
	pthread_mutex_unlock(&semRtRwMutex);
	if(0 == ret) {
		gIndex--;
	}
	else {
		nam_syslog_err("delete lpm entry error\n");
		return ret;
	}
#endif
	return 0;
}

int nam_tunnel_start_set_check
(
	unsigned char devNum,
	unsigned int routerArpTunnelStartLineIndex
)
{
#ifdef DRV_LIB_CPSS

	int ret = 0;
	CPSS_DXCH_TUNNEL_START_CONFIG_UNT	*configptr = NULL;
	CPSS_TUNNEL_TYPE_ENT tunnelTypePtr;

	configptr = malloc(sizeof(CPSS_DXCH_TUNNEL_START_CONFIG_UNT));
	if (NULL == configptr) {
		nam_syslog_err("configptr malloc fail \n");
		return 1;
	}
	memset(configptr, 0, sizeof(CPSS_DXCH_TUNNEL_START_CONFIG_UNT));

	ret = cpssDxChTunnelStartEntryGet(devNum, routerArpTunnelStartLineIndex, &tunnelTypePtr, configptr);
	if (0 != ret) {
		nam_syslog_err("cpssDxChTunnelStartEntryGet fail ! ret is %d \n", ret);
		free(configptr);
		return 1;
	}

	free(configptr);
#endif	
	return 0;
}

int nam_tunnel_ipv4_tunnel_term_port_set
(
	unsigned char       devNum,
	unsigned char       port,
	unsigned long		enable
)
{
#ifdef DRV_LIB_CPSS

	int ret = 0;
#ifdef DRV_LIB_CPSS_3_4
	if(cpssDxChFamilyCheck(devNum,CPSS_CH2_E)){		
		ret = cpssDxChIpv4TunnelTermPortSet(devNum, port, enable);
	}
	else{
		cpssDxChTtiPortLookupEnableSet(devNum, port, CPSS_DXCH_TTI_KEY_IPV4_E, enable);
		cpssDxChTtiPortIpv4OnlyTunneledEnableSet(devNum, port, enable);
	}

#else
	ret = cpssDxChIpv4TunnelTermPortSet(devNum, port, enable);
#endif
	if (0 != ret) {
		nam_syslog_err("cpssDxChIpv4TunnelTermPortSet fail ! ret is %d \n", ret);
		return 1;
	}
#endif
	return 0;
}

int nam_tunnel_term_entry_set
(
	unsigned char devNum,
	unsigned int routerTunnelTermTcamIndex,
	unsigned int  tunnelType,
	struct tunnel_item_s    *configPtr
)
{
#ifdef DRV_LIB_CPSS

	int ret = 0, i = 0;
	CPSS_TUNNEL_TYPE_ENT tunneltype;
	CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT *configptr = NULL;
	CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT *conmaskptr = NULL;
	CPSS_DXCH_TUNNEL_TERM_ACTION_STC *actionptr = NULL;

	if (NULL == configPtr) {
		nam_syslog_err("configPtr is NULL \n");
		return 1;
	}

	configptr = (CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT *)malloc(sizeof(CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT));
	if (NULL == configptr) {
		nam_syslog_err("configptr malloc fail \n");
		goto errRet;
	}
	conmaskptr = (CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT *)malloc(sizeof(CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT));
	if (NULL == conmaskptr) {	/* Code Optimize: Logically dead code Houxx@Autelan.Com  2013-1-21 */
		nam_syslog_err("conmaskptr malloc fail \n");
		goto errRet;
	}
	actionptr = (CPSS_DXCH_TUNNEL_TERM_ACTION_STC *)malloc(sizeof(CPSS_DXCH_TUNNEL_TERM_ACTION_STC));
	if (NULL == actionptr ) {	/* Code Optimize: Logically dead code Houxx@Autelan.Com  2013-1-21 */
		nam_syslog_err("actionptr malloc fail \n");
		goto errRet;
	}
	memset(configptr, 0, sizeof(CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT));
	memset(conmaskptr, 0, sizeof(CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT));
	memset(actionptr, 0, sizeof(CPSS_DXCH_TUNNEL_TERM_ACTION_STC));

	tunneltype = tunnelType;
	configptr->ipv4Cfg.srcIp.u32Ip = configPtr->kmsg.dstip;/* change bcause the ip is src  for remote dev*/
	nam_syslog_dbg("tt srcip is 0x%x \n", configptr->ipv4Cfg.srcIp);
	configptr->ipv4Cfg.destIp.u32Ip = configPtr->kmsg.srcip;/* change bcause the ip is dst  for remote dev*/
	nam_syslog_dbg("tt dstip is 0x%x \n", configptr->ipv4Cfg.destIp);
	configptr->ipv4Cfg.vid = configPtr->kmsg.vid;
	nam_syslog_dbg("tt vid is %d \n", configptr->ipv4Cfg.vid);
	for (i = 0; i < 6; i++) {
		configptr->ipv4Cfg.macDa.arEther[i] = configPtr->sysmac[i];
	}

	conmaskptr->ipv4Cfg.srcIp.u32Ip = 0xFFFFFFFF;
	conmaskptr->ipv4Cfg.destIp.u32Ip = 0xFFFFFFFF;
	conmaskptr->ipv4Cfg.vid = 0xFFF;
	for (i = 0; i < 6; i++) {
		configptr->ipv4Cfg.macDa.arEther[i] = 0xFF;
	}

	/*if (configPtr->istuact) {
		actionptr->isTunnelStart = 1;
		actionptr->tunnelStartIdx = configPtr->tsindex;
	}*/
#ifdef DRV_LIB_CPSS_3_4
	/* cpssDxChTtiRuleSet(devNum, routerTunnelTermTcamIndex, tunnelType,); */
#else
	actionptr->command = CPSS_PACKET_CMD_ROUTE_E; /*must set this command !!!*/
	ret = cpssDxChTunnelTermEntrySet(devNum, routerTunnelTermTcamIndex, tunnelType, configptr, conmaskptr, actionptr);
	if (0 != ret) {
		nam_syslog_err("cpssDxChTunnelTermEntrySet fail ! ret is %d \n", ret);
		goto errRet;
	}
#endif

	free(configptr);
	free(conmaskptr);
	free(actionptr);
#endif	
	return 0;

#ifdef DRV_LIB_CPSS

	errRet:
	if(configptr) {
		free(configptr);
		configptr = NULL;
	}
	if(conmaskptr) {
		free(conmaskptr);
		conmaskptr = NULL;
	}
	if(actionptr) {
		free(actionptr);
		actionptr = NULL;
	}
	return 1;
#endif	
}

int nam_tunnel_term_entry_del
(
	unsigned char devNum,
	unsigned int routerTunnelTermTcamIndex,
	unsigned int  tunnelType
)
{
#ifdef DRV_LIB_CPSS

	int ret = 0;
	CPSS_TUNNEL_TYPE_ENT tunneltype;
	CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT *configptr = NULL;
	CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT *conmaskptr = NULL;
	CPSS_DXCH_TUNNEL_TERM_ACTION_STC *actionptr = NULL;

	configptr = (CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT *)malloc(sizeof(CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT));
	if (NULL == configptr) {
		nam_syslog_err("configptr malloc fail \n");
		goto errRet;
	}
	conmaskptr = (CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT *)malloc(sizeof(CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT));
	if (NULL == conmaskptr) {	/* Code Optimize: Logically dead code Houxx@Autelan.Com  2013-1-21 */
		nam_syslog_err("conmaskptr malloc fail \n");
		goto errRet;
	}
	actionptr = (CPSS_DXCH_TUNNEL_TERM_ACTION_STC *)malloc(sizeof(CPSS_DXCH_TUNNEL_TERM_ACTION_STC));
	if (NULL == actionptr) {
		nam_syslog_err("actionptr malloc fail \n");
		goto errRet;
	}
	memset(configptr, 0, sizeof(CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT));
	memset(conmaskptr, 0, sizeof(CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT));
	memset(actionptr, 0, sizeof(CPSS_DXCH_TUNNEL_TERM_ACTION_STC));

	tunneltype = tunnelType;	
	actionptr->command = CPSS_PACKET_CMD_ROUTE_E;
	ret = cpssDxChTunnelTermEntrySet(devNum, routerTunnelTermTcamIndex, tunnelType, configptr, conmaskptr, actionptr);
	if (0 != ret) {
		nam_syslog_err("cpssDxChTunnelTermEntrySet fail ! ret is %d \n", ret);
		goto errRet;
	}
	
	free(configptr);
	free(conmaskptr);
	free(actionptr);
#endif	
	return 0;

#ifdef DRV_LIB_CPSS

	errRet:
	if(configptr) {
		free(configptr);
		configptr = NULL;
	}
	if(conmaskptr) {
		free(conmaskptr);
		conmaskptr = NULL;
	}
	if(actionptr) {
		free(actionptr);
		actionptr = NULL;
	}
	return 1;
#endif	
}

int nam_tunnel_term_entry_invalidate
(
	unsigned char devNum,
	unsigned int routerTunnelTermTcamIndex
)
{
#ifdef DRV_LIB_CPSS

	int ret = 0;

	ret = cpssDxChTunnelTermEntryInvalidate(devNum, routerTunnelTermTcamIndex);
	if (0 != ret) {
		nam_syslog_err("cpssDxChTunnelTermEntryInvalidate fail ! ret is %d \n", ret);
		return 1;
	}
#endif
	return 0;
}

#ifdef __cplusplus
}
#endif

