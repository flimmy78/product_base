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
* npd_route.c
*
*
* CREATOR:
*		flash@autelan.com
*
* DESCRIPTION:
*		APIs used in NPD for L3 routing process.
*
* DATE:
*		03/20/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.77 $	
*******************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/rtnetlink.h>
#include <linux/in.h>
#include <linux/if.h>
#include <pthread.h>
#include <net/if.h>
#include <bits/sockaddr.h>
#include <dbus/dbus.h>
#include <sysdef/npd_sysdef.h>
#include "sysdef/returncode.h"
#include "util/npd_list.h"
#include "npd/nam/npd_amapi.h"
#include "npd/nbm/npd_bmapi.h"
#include "dbus/npd/npd_dbus_def.h"
#include "npd_route.h"
#include "npd_log.h"
#include "npd_arpsnooping.h"
#include "npd_product.h"
#include "npd_c_slot.h"
#include "npd_eth_port.h"
#include "npd_intf.h"

extern int adptVirRxFd;
#ifdef CPU_ARM_XCAT
extern unsigned int g_npd_thread_number;
#endif

#define RTM_TABLE_DESC(table) 					\
	(RT_TABLE_UNSPEC == table) ? "UNSPEC": 		\
	(RT_TABLE_DEFAULT == table) ? "DEFAULT" : 	\
	(RT_TABLE_MAIN == table) ? "MAIN" : 		\
	(RT_TABLE_LOCAL == table) ? "LOCAL": "MAX"	
	
#define RTM_TYPE_DESC(type) 					\
	(RTN_UNSPEC == type) ? "UNSPEC" :			\
	(RTN_UNICAST == type) ? "UNICAST" :			\
	(RTN_LOCAL == type) ? "LOCAL" :				\
	(RTN_BROADCAST == type) ? "BROADCAST" :		\
	(RTN_ANYCAST == type) ? "ANYCAST" :			\
	(RTN_MULTICAST == type) ? "MULTICAST" :		\
	(RTN_BLACKHOLE == type) ? "BLACKHOLE" : 	\
	(RTN_UNREACHABLE == type) ? "UNREACHABLE" :	\
	(RTN_PROHIBIT == type) ? "PROHIBIT" :		\
	(RTN_THROW == type) ? "THROW" :				\
	(RTN_NAT == type) ? "NAT" :					\
	(RTN_XRESOLVE == type) ? "XRESOLVE" : "MAX"

#define NL_MSG_TYPE_DESC(type)					\
	(RTM_NEWLINK == type) ? "NEWLINK" :			\
	(RTM_DELLINK == type) ? "DELLINK" :			\
	(RTM_GETLINK == type) ? "GETLINK" :			\
	(RTM_SETLINK == type) ? "SETLINK" :			\
	(RTM_NEWADDR == type) ? "NEWADDR" :			\
	(RTM_DELADDR == type) ? "DELADDR" :			\
	(RTM_GETADDR == type) ? "GETADDR" :			\
	(RTM_NEWROUTE == type) ? "NEWROUTE" :		\
	(RTM_DELROUTE == type) ? "DELROUTE" :		\
	(RTM_GETROUTE == type) ? "GETROUTE" :		\
	(RTM_NEWNEIGH == type) ? "NEWNEIGH" :		\
	(RTM_DELNEIGH == type) ? "DELNEIGH" :		\
	(RTM_GETNEIGH == type) ? "GETNEIGH" : "OTHER"
	
/* Utility function for parse rtattr. */
static void
netlink_parse_rtattr (struct rtattr **tb, int max, struct rtattr *rta,
					  int len)
{
  while (RTA_OK (rta, len))
	{
	  if (rta->rta_type <= max)
		tb[rta->rta_type] = rta;
	  
	    if(tb[rta->rta_type]) {
	         char* tmp = tb[rta->rta_type] ;
			/* syslog_ax_route_dbg("type %d-%s",rta->rta_type,tmp);*/
		}
		rta = RTA_NEXT (rta, len);
    }
}

unsigned int get_NextHop_TBL_Entry(unsigned int ifIndex,unsigned int nextHopIp)
{
	unsigned int tblIndex;
	unsigned int retVal = 0;


	retVal = npd_route_nexthop_tblindex_get(ifIndex,nextHopIp,&tblIndex);
	if(0!=retVal) {
		 syslog_ax_route_err("get route next hop ip %#0x index %#0x at table error %d\n",ifIndex,nextHopIp,retVal);
		return 0;/* Next-Hop table entry 0 with action:TRAP-to-CPU*/
	}
	
	 syslog_ax_route_dbg("get route next hop ip %#0x index %#0x at table line %d\n",nextHopIp,ifIndex,tblIndex);
	return tblIndex;

}

unsigned char get_route_info( struct rttbl_info *pRT_info,struct rtmsg * pRTmsgData,int payloadlen)
{
	struct rtattr * rtattrptr = NULL;
	unsigned int *tmp;
	char ifname[IF_NAMESIZE] = {0};
	
	if(!pRTmsgData || !pRT_info)
	{
		 syslog_ax_route_err("get rt message paraments is NULL\n");
		return 1;
	}
/* 
	check the route table 
*/
	npd_syslog_dbg("rt message table %s type %s\n", \
					RTM_TABLE_DESC(pRTmsgData->rtm_table), RTM_TYPE_DESC(pRTmsgData->rtm_type));
	#if 0
	if(AF_INET != pRTmsgData->rtm_family || RT_TABLE_LOCAL == pRTmsgData->rtm_table)
	#else
	if(AF_INET != pRTmsgData->rtm_family)
	#endif
	{
		syslog_ax_route_dbg("ignore rt message family type(%d) non INET!\n", pRTmsgData->rtm_family);
		return 2;
	}
	if(RTN_BLACKHOLE == pRTmsgData->rtm_type){
		pRT_info->rt_type = RTN_BLACKHOLE;
	}
	else if(RTN_UNREACHABLE == pRTmsgData->rtm_type){
		pRT_info->rt_type = RTN_UNREACHABLE;
	}
	else if(RTN_LOCAL == pRTmsgData->rtm_type) {
		pRT_info->rt_type = RTN_LOCAL;
	}
	else if(RTN_BROADCAST == pRTmsgData->rtm_type) {
		syslog_ax_route_dbg("ignore rt broadcast message!\n");
		return 3;
	}
	else if(RTN_UNSPEC == pRTmsgData->rtm_type) {
		syslog_ax_route_dbg("ignore rt unspec message!\n");
		return 4;
	}
	else {		
		pRT_info->rt_type = 0;
	}
	
	pRT_info->masklen = pRTmsgData->rtm_dst_len;
	if((RTN_LOCAL == pRT_info->rt_type) && (pRT_info->masklen != 32)){
		syslog_ax_route_dbg("ignore rt local message scope not host(mask=%d)!\n", pRT_info->masklen);
		return 5;
	}
	
	rtattrptr = (struct rtattr *) RTM_RTA(pRTmsgData);

	for(;RTA_OK(rtattrptr, payloadlen);rtattrptr=RTA_NEXT(rtattrptr,payloadlen))
	{
  		switch(rtattrptr->rta_type) {
	    	/* destination IPv4 address*/
	    	case RTA_DST:
			tmp= (unsigned int *)(RTA_DATA(rtattrptr));
			pRT_info->DIP = (unsigned int)*tmp;
	      		break;

	    	/* next hop IPv4 address*/
	    	case RTA_GATEWAY:
			
			tmp = (unsigned int *)RTA_DATA(rtattrptr);
			pRT_info->nexthop =*tmp;
	      		break;

	        case RTA_OIF:
			tmp = (unsigned int *)RTA_DATA(rtattrptr);
			pRT_info->ifindex = *tmp;
			break;
			default:
			syslog_ax_route_dbg("rt message attribute type %d, ignore\n",rtattrptr->rta_type);
		}
    }

	if(pRT_info->ifindex) {
		if_indextoname(pRT_info->ifindex, ifname);
	}
	
	syslog_ax_route_dbg("rt message:%d.%d.%d.%d/%d nexthop %d.%d.%d.%d via %s\n", \
						(pRT_info->DIP>>24)&0xFF, (pRT_info->DIP>>16)&0xFF, (pRT_info->DIP>>8)&0xFF, \
						pRT_info->DIP&0xFF, pRT_info->masklen, (pRT_info->nexthop>>24)&0xFF, \
						(pRT_info->nexthop>>16)&0xFF,(pRT_info->nexthop>>8)&0xFF, pRT_info->nexthop&0xFF, ifname);
	return 0;
}

GT_STATUS write_msg_to_drv(void *ptr,int totlemsglen)
{
	struct rttbl_info rtInfo ={0};
	struct rtmsg *rtEntry=NULL;
	struct nlmsghdr *nlp = (struct nlmsghdr *)ptr;
	int payloadoff = 0;
	GT_STATUS status = 0;
	unsigned int ret = 0;


	for(;NLMSG_OK(nlp, totlemsglen);nlp=NLMSG_NEXT(nlp, totlemsglen))
	{

		if(RTM_NEWROUTE != nlp->nlmsg_type && RTM_DELROUTE != nlp->nlmsg_type &&RTM_GETROUTE != nlp->nlmsg_type)
			continue;

		rtEntry = (struct rtmsg *) NLMSG_DATA(nlp);
		payloadoff = RTM_PAYLOAD(nlp);
		memset(&rtInfo,0,sizeof(rtInfo));
		ret = get_route_info(&rtInfo,rtEntry,payloadoff);
		
		if(0 != ret)
			continue;
		switch( nlp->nlmsg_type )
		{
		case RTM_NEWROUTE:
		case RTM_GETROUTE:
			status = set_LPM_TBL_Entry(	\
				rtInfo.DIP,rtInfo.nexthop,rtInfo.masklen,rtInfo.ifindex,rtInfo.rt_type);		
			if(status)
			{
				syslog_ax_route_err("add route entry %d.%d.%d.%d/%d nexthop %d.%d.%d.%d if %d type %d fail %ld\n", \
					   (rtInfo.DIP>>24)&0xFF, (rtInfo.DIP>>16)&0xFF, (rtInfo.DIP>>8)&0xFF, rtInfo.DIP &0xFF, \
					   rtInfo.masklen, (rtInfo.nexthop>>24)&0xFF, (rtInfo.nexthop>>16)&0xFF, (rtInfo.nexthop>>8)&0xFF,	\
					   rtInfo.nexthop&0xFF, rtInfo.ifindex, rtInfo.rt_type, status);
				return status;
			}
			syslog_ax_route_dbg("add route entry %d.%d.%d.%d/%d nexthop %d.%d.%d.%d if %d type %d ok\n", \
				   (rtInfo.DIP>>24)&0xFF, (rtInfo.DIP>>16)&0xFF, (rtInfo.DIP>>8)&0xFF, rtInfo.DIP &0xFF, \
				   rtInfo.masklen, (rtInfo.nexthop>>24)&0xFF, (rtInfo.nexthop>>16)&0xFF, (rtInfo.nexthop>>8)&0xFF,	\
				   rtInfo.nexthop&0xFF, rtInfo.ifindex, rtInfo.rt_type);
			break;
		case RTM_DELROUTE:
			ret = nam_del_route_info(rtInfo.DIP,rtInfo.nexthop,rtInfo.masklen,rtInfo.ifindex,rtInfo.rt_type);
			if(ret)
			{
				syslog_ax_route_err("add route entry %d.%d.%d.%d/%d nexthop %d.%d.%d.%d if %d type %d fail %ld\n", \
					   (rtInfo.DIP>>24)&0xFF, (rtInfo.DIP>>16)&0xFF, (rtInfo.DIP>>8)&0xFF, rtInfo.DIP &0xFF, \
					   rtInfo.masklen, (rtInfo.nexthop>>24)&0xFF, (rtInfo.nexthop>>16)&0xFF, (rtInfo.nexthop>>8)&0xFF,	\
					   rtInfo.nexthop&0xFF, rtInfo.ifindex, rtInfo.rt_type, ret);
				 return ret;
			}
			syslog_ax_route_dbg("delete route entry %d.%d.%d.%d/%d nexthop %d.%d.%d.%d if %d type %d ok\n", \
				   (rtInfo.DIP>>24)&0xFF, (rtInfo.DIP>>16)&0xFF, (rtInfo.DIP>>8)&0xFF, rtInfo.DIP &0xFF, \
				   rtInfo.masklen, (rtInfo.nexthop>>24)&0xFF, (rtInfo.nexthop>>16)&0xFF, (rtInfo.nexthop>>8)&0xFF,	\
				   rtInfo.nexthop&0xFF, rtInfo.ifindex, rtInfo.rt_type);
            break;
		default:
			syslog_ax_route_err("ignore msg type %s size %d\n",NL_MSG_TYPE_DESC(nlp->nlmsg_type),totlemsglen);
			return status;
		}
	}
	return GT_OK;
}


GT_STATUS write_msg_to_drv_for_syn(void *ptr,int totlemsglen,unsigned int srcpid)
{
	struct rttbl_info rtInfo ={0};
	struct rtmsg *rtEntry=NULL;
	struct nlmsghdr *nlp = (struct nlmsghdr *)ptr;
	int payloadoff = 0;
	GT_STATUS status;
	unsigned int ret = 0,routeEntryBaseIndex = 0;
	char ifname[IF_NAMESIZE] = {0};


	for(;NLMSG_OK(nlp, totlemsglen);nlp=NLMSG_NEXT(nlp, totlemsglen))
	{
		if(RTM_NEWROUTE != nlp->nlmsg_type && RTM_DELROUTE != nlp->nlmsg_type &&RTM_GETROUTE != nlp->nlmsg_type)
		{	
			/*zhangcl@autelan.com clean the print message */
			#if 0
			syslog_ax_route_dbg("Discard non route msg pid %u type %s",srcpid, NL_MSG_TYPE_DESC(nlp->nlmsg_type));
			#endif
			continue;
		}	

		rtEntry = (struct rtmsg *) NLMSG_DATA(nlp);
		payloadoff = RTM_PAYLOAD(nlp);

		if(srcpid==0&&rtEntry->rtm_protocol!=RTPROT_KERNEL&&rtEntry->rtm_protocol!=RTPROT_BOOT)
		{
			syslog_ax_route_dbg("Discard kernel msg with protocol %d\n", rtEntry->rtm_protocol);
			continue;
		}	
		if(srcpid!=0&&rtEntry->rtm_protocol!=RTPROT_ZEBRA)
		{
			syslog_ax_route_dbg("Discard neither kernel nor zebra msg pid %u protocol %d", \
							srcpid,rtEntry->rtm_protocol);
			continue;
		}	
		memset(&rtInfo,0,sizeof(rtInfo));
		ret = get_route_info(&rtInfo,rtEntry,payloadoff);
		
		if(0 != ret)
		{
			syslog_ax_route_dbg("get msg route info error %d ,continue\n", ret);
			continue;
		}
		
		if(rtInfo.ifindex) 
		{
			if_indextoname(rtInfo.ifindex, ifname);
			if(0 == strncmp(ifname,"ve",2))
			{
				syslog_ax_route_dbg("interface %s do not need syn route table to asic.",ifname);
				memset(ifname,0,IF_NAMESIZE);	/* code optimize: Memset buffer size of 0 houxx@autelan.com  2013-1-19 */
				continue;
			}
		}

		switch( nlp->nlmsg_type )
		{
			case RTM_NEWROUTE:
			case RTM_GETROUTE:			
				status = set_LPM_TBL_Entry( \
					rtInfo.DIP,rtInfo.nexthop,rtInfo.masklen,rtInfo.ifindex,rtInfo.rt_type);		
				if(status)
				{
					 syslog_ax_route_err("add route entry %d.%d.%d.%d/%d nexthop %d.%d.%d.%d if %d type %d fail %ld\n", \
					 		(rtInfo.DIP>>24)&0xFF, (rtInfo.DIP>>16)&0xFF, (rtInfo.DIP>>8)&0xFF, rtInfo.DIP &0xFF, \
							rtInfo.masklen, (rtInfo.nexthop>>24)&0xFF, (rtInfo.nexthop>>16)&0xFF, (rtInfo.nexthop>>8)&0xFF,  \
							rtInfo.nexthop&0xFF, rtInfo.ifindex, rtInfo.rt_type, status);
					 return status;
				}
				syslog_ax_route_dbg("add route entry %d.%d.%d.%d/%d nexthop %d.%d.%d.%d if %d type %d ok\n", \
					   (rtInfo.DIP>>24)&0xFF, (rtInfo.DIP>>16)&0xFF, (rtInfo.DIP>>8)&0xFF, rtInfo.DIP &0xFF, \
					   rtInfo.masklen, (rtInfo.nexthop>>24)&0xFF, (rtInfo.nexthop>>16)&0xFF, (rtInfo.nexthop>>8)&0xFF,	\
					   rtInfo.nexthop&0xFF, rtInfo.ifindex, rtInfo.rt_type);
				break;
			case RTM_DELROUTE:
			   status = nam_del_route_info(rtInfo.DIP,rtInfo.nexthop,rtInfo.masklen,rtInfo.ifindex,rtInfo.rt_type);
               if(status){
				   syslog_ax_route_err("delete route entry %d.%d.%d.%d/%d nexthop %d.%d.%d.%d if %d type %d fail %ld\n", \
						  (rtInfo.DIP>>24)&0xFF, (rtInfo.DIP>>16)&0xFF, (rtInfo.DIP>>8)&0xFF, rtInfo.DIP &0xFF, \
						  rtInfo.masklen, (rtInfo.nexthop>>24)&0xFF, (rtInfo.nexthop>>16)&0xFF, (rtInfo.nexthop>>8)&0xFF,  \
						  rtInfo.nexthop&0xFF, rtInfo.ifindex, rtInfo.rt_type, status);
				   return status;
               }
			   syslog_ax_route_dbg("delete route entry %d.%d.%d.%d/%d nexthop %d.%d.%d.%d if %d type %d ok\n", \
					  (rtInfo.DIP>>24)&0xFF, (rtInfo.DIP>>16)&0xFF, (rtInfo.DIP>>8)&0xFF, rtInfo.DIP &0xFF, \
					  rtInfo.masklen, (rtInfo.nexthop>>24)&0xFF, (rtInfo.nexthop>>16)&0xFF, (rtInfo.nexthop>>8)&0xFF,  \
					  rtInfo.nexthop&0xFF, rtInfo.ifindex, rtInfo.rt_type);
			   break;				
			default:
				syslog_ax_route_err("ignore msg type %s size %d\n",NL_MSG_TYPE_DESC(nlp->nlmsg_type),totlemsglen);
				return status;
		}
	}
	return GT_OK;
}

GT_STATUS recv_rtMSG_from_kernel(int sockfd,void* ptr,int* buflen)
{
	int msglen=0,totlemsglen=0;
	char* p =ptr;
	GT_STATUS	status;

	while(1)
	{
		struct nlmsghdr *tmp=NULL;
		msglen = recv(sockfd, p, *buflen - totlemsglen, 0);
		if(msglen <= 0)
		{
			 syslog_ax_route_dbg("recv msg return error\n");
			status = 1;
			break;
		}
		tmp = (struct nlmsghdr *) p;

		if( (NLM_F_MULTI == tmp->nlmsg_flags) && (NLMSG_DONE == tmp->nlmsg_type)) 
		{
			syslog_ax_route_dbg("route receive message type is NLMSG_DONE\n");
			status = 0;
			break;
		}
		else if( NLMSG_ERROR == tmp->nlmsg_type)
		{
			syslog_ax_route_dbg("recv msg error\n");
			status = 1;
			break;
		}		
		else if( NLMSG_OVERRUN == tmp->nlmsg_type )
		{
			syslog_ax_route_dbg("the buf is overrun when recv msg\n");
			status = 1;
			break;
		}
		else if( NLMSG_NOOP == tmp->nlmsg_type )
		{
			syslog_ax_route_dbg("recev nothing\n");
			status = 1;
			break;
		}
		p += msglen;
		totlemsglen += msglen;

	}
	*buflen = totlemsglen;
	return status;

}

const char *
npd_route_inet_ntop (int family, const void *addrptr, char *strptr, int len)
{
  unsigned char *p = (unsigned char *) addrptr;

  if (family == AF_INET) 
    {
      char temp[16];

      snprintf(temp, sizeof(temp), "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);

      if (strlen(temp) >= len) 
	{
	  return NULL;
	}
      strcpy(strptr, temp);
      return strptr;
    }
  
  return NULL;
}


void read_ip(void* ptr,int totlemsglen)
{
	  int len = 0;
	  unsigned int isAdd = 1;
	  struct ifaddrmsg *ifa;
	  struct nlmsghdr *h = (struct nlmsghdr *)ptr;
	  struct rtattr *tb[IFA_MAX + 1];
	  char *label = NULL;
	  static char buf[BUFLENGTH],*ip = NULL;
	  unsigned short vlanid = 0;
	  unsigned int ipAddr = 0;
	  unsigned int netAddr = 0;
	  #if 0
	  DBusMessage *query, *reply;	
	  DBusMessageIter	 iter;
	  DBusError err;
	  #endif
	  ifa = NLMSG_DATA (h);
	  if(ifa == NULL)
	  	return -1;
	  if(h->nlmsg_type != RTM_NEWADDR && h->nlmsg_type != RTM_DELADDR)
		 return 0;
	
	  len = h->nlmsg_len - NLMSG_LENGTH (sizeof (struct ifaddrmsg));
	  if (len < 0)
		return -1;
	  if(h->nlmsg_type == RTM_NEWADDR){
         isAdd = 1;
	  }
	  else if(h->nlmsg_type == RTM_DELADDR){
		  isAdd = 0;
	  }
	  memset (tb, 0, sizeof tb);
	  netlink_parse_rtattr (tb, IFA_MAX, IFA_RTA (ifa), len);

	  npd_syslog_dbg("intf index is %d\n",ifa->ifa_index);
	  if (tb[IFA_LOCAL]){
		npd_syslog_dbg("  IFA_LOCAL	 %s/%d",
			npd_route_inet_ntop(ifa->ifa_family, RTA_DATA (tb[IFA_LOCAL]),
				   buf, BUFLENGTH), ifa->ifa_prefixlen);
         ip = buf;
	  }
      if(!isAdd ){
	  	  memcpy(&ipAddr,RTA_DATA (tb[IFA_LOCAL]),sizeof(unsigned int));
		  netAddr = ipAddr & (unsigned int )(-(1<<(32-(ifa->ifa_prefixlen))));
		  npd_syslog_dbg("delete arp by network %d.%d.%d.%d/%d in read_ip\n",\
		  	(netAddr>>24)&0xff,(netAddr>>16)&0xff,(netAddr>>8)&0xff,netAddr&0xff,\
		  	(ifa->ifa_prefixlen));
		  npd_arp_snooping_arp_del_by_ip_and_mask(ipAddr,(unsigned int )(-(1<<(32-(ifa->ifa_prefixlen)))));
      }
	  if (tb[IFA_LABEL]){
	      npd_syslog_dbg("  IFA_LABEL     %s", (char *)RTA_DATA (tb[IFA_LABEL]));
	      label = (char *)RTA_DATA (tb[IFA_LABEL]);
		  if(0 == strncmp(label,"ve",2))
		  {
			npd_syslog_dbg("interface %s do not need syn route table to asic.",label);
			return;
		  }
	  }
	  if (tb[IFA_CACHEINFO])
        {
          struct ifa_cacheinfo *ci = RTA_DATA (tb[IFA_CACHEINFO]);
          npd_syslog_dbg("  IFA_CACHEINFO pref %d, valid %d",
                      ci->ifa_prefered, ci->ifa_valid);
        }
	if((strlen(buf)> 0)&&(label != NULL)) { 
		#if 0
		query = dbus_message_new_signal(NPD_DBUS_OBJPATH,\
							"aw.trap",NPD_DBUS_ROUTE_METHOD_NOTIFY_SNMP_BY_IP);
		dbus_error_init(&err);

		dbus_message_append_args(query,
							DBUS_TYPE_UINT32,&isAdd,
							DBUS_TYPE_BYTE,&(ifa->ifa_prefixlen),
							DBUS_TYPE_STRING,&ip,							 
							DBUS_TYPE_INVALID);

		dbus_connection_send (npd_dbus_connection,query,NULL);
		
		dbus_message_unref(query);
		npd_syslog_dbg("npd route read ip send isadd %d,ip %s,mask %d\n",isAdd,ip,ifa->ifa_prefixlen);
		#endif
	
		#if 1
		DBusMessage* msg = NULL;
		static DBusConnection *npd_trap_snmp_ip_conn = NULL;
		DBusError err;
		int ret = 0;
		unsigned int serial = 0;

		dbus_error_init(&err);
	
		if (NULL == npd_trap_snmp_ip_conn) {
			/* connect to the DBUS system bus, and check for errors */
			npd_trap_snmp_ip_conn = dbus_bus_get_private(DBUS_BUS_SYSTEM, &err);
			if (dbus_error_is_set(&err)) {
				npd_syslog_err("Connection Error (%s)\n", err.message);
				dbus_error_free(&err);
				return;
			}
			if (NULL == npd_trap_snmp_ip_conn) {
				npd_syslog_err("Connection null\n");
				return;
			}
		}
		npd_syslog_dbg("bus get private ok\n");
	
		/* register our name on the bus, and check for errors */
		ret = dbus_bus_request_name(npd_trap_snmp_ip_conn,
									"aw.npd.snmpip.signal",
									0,
									&err);
		if (dbus_error_is_set(&err)) {
			npd_syslog_err("Name Error (%s)\n", err.message);
			dbus_error_free(&err);
			return;
		}
		npd_syslog_dbg("request name aw.npd.snmpip.signal ok\n");
		
		/* create a signal & check for errors */
		msg = dbus_message_new_signal(NPD_DBUS_OBJPATH, 			/* object name of the signal */
									  "aw.trap",					/* interface name of the signal */
									  NPD_DBUS_ROUTE_METHOD_NOTIFY_SNMP_BY_IP); /* name of the signal */
		if (NULL == msg) {
			npd_syslog_err("Message Null\n");
			return;
		}
		npd_syslog_dbg("new signal ok\n");
		
		/* append arguments onto signal */
		dbus_message_append_args(msg,
							DBUS_TYPE_UINT32, &isAdd,
							DBUS_TYPE_BYTE,   &(ifa->ifa_prefixlen),
							DBUS_TYPE_STRING, &ip,
							DBUS_TYPE_UINT32, &(ifa->ifa_index),
							DBUS_TYPE_INVALID);
		
		/* send the message and flush the connection */
		if (!dbus_connection_send(npd_trap_snmp_ip_conn, msg, &serial)) {
			npd_syslog_err("Signal send error, Out Of Memory!\n"); 
			return;
		}

		dbus_connection_flush(npd_trap_snmp_ip_conn);
		npd_syslog_event("sent snmp ip signal message to trap.\n");
	
		/* free the message */
		dbus_message_unref(msg);
		npd_syslog_dbg("npd route read ip send isadd %d,ip %s,mask %d\n",isAdd,ip,ifa->ifa_prefixlen);
	
		return;
		#endif
	}
	return 0;
}

void read_newlink(void* ptr,int totlemsglen, unsigned int src_pid)
{
	
	struct  ifinfomsg *rtEntry = NULL;
	struct nlmsghdr *nlp = (struct nlmsghdr *)ptr;
	int payloadoff = 0,ret = 0;
	unsigned char macComp[13] = {0}, oldIntfMac[6] = {0};
	char dftUniMac[6] = {0};
	struct rtattr *rtattrptr = NULL;
	unsigned char* ifaddr = NULL;
	unsigned char* ifname = NULL;
	unsigned int ifindex = 0,eth_g_index = 0;
	unsigned int ifi_flags = 0, ifi_index = ~0UI;;
	KAP_DEV_TYPE type;
    unsigned int vid = 0;
	unsigned char kapOrSpiIf = 0xFF, delOldL3 = 1;
	int isSysMac = 0;
	
	memset(&type,0,sizeof(KAP_DEV_TYPE));
	for(;NLMSG_OK(nlp, totlemsglen);nlp=NLMSG_NEXT(nlp, totlemsglen)) {
		rtEntry = (struct ifinfomsg *) NLMSG_DATA(nlp);
		payloadoff = RTM_PAYLOAD(nlp);
		switch( nlp->nlmsg_type ) {
		  case RTM_NEWLINK:
		  	ifi_flags = rtEntry->ifi_flags;  /* get interface */
		  	ifi_index = rtEntry->ifi_index; /* ifindex for kernel*/
			rtattrptr = (struct rtattr *)IFLA_RTA(rtEntry);
			for(;RTA_OK(rtattrptr, payloadoff);rtattrptr=RTA_NEXT(rtattrptr,payloadoff)) {
				switch(rtattrptr->rta_type) {
					case IFLA_ADDRESS:
						ifaddr = (unsigned char*)RTA_DATA(rtattrptr);
                        /*notify to npd to modified the sysmac and L3 static fdb entry*/
						sprintf(macComp,"%02x%02x%02x%02x%02x%02x",	\
				                ifaddr[0],ifaddr[1],ifaddr[2],ifaddr[3],ifaddr[4],ifaddr[5]);
						break;
					case IFLA_IFNAME:
						ifname = (unsigned char*)RTA_DATA(rtattrptr);
						/*npd_syslog_dbg("ifindex %d,vid %d\n",ifindex,vid);*/
						break;
					default:
						/*npd_syslog_dbg("other value ignore %d\n",nlp->nlmsg_type);*/
                        break;
				}
			}
			break;
		  default:
			 break;
		  }		    
	}
	
	/* check whether this message is for L3 interface mac address update*/
	if(ifaddr && ifname) {
		/*zhangcl@autelan.com clean the print message*/
		#if 0
		syslog_ax_route_dbg("RTM_NEWLINK message for interface %s change mac %02x:%02x:%02x:%02x:%02x:%02x\n", \
						ifname, ifaddr[0],ifaddr[1],ifaddr[2],ifaddr[3],ifaddr[4],ifaddr[5]);
		#endif
		/*notify to npd to modified the sysmac and L3 static fdb entry*/
		sprintf(macComp,"%02x%02x%02x%02x%02x%02x", \
				ifaddr[0],ifaddr[1],ifaddr[2],ifaddr[3],ifaddr[4],ifaddr[5]);

		/* check if same as system mac*/
		isSysMac = npd_system_verify_basemac(macComp);
		
		/* set default unique mac address as FF:FF:FF:FF:FF:FF*/
		memset(dftUniMac, 0xFF, 6);
		
		if(!strncasecmp("vlan", (char*)ifname, sizeof("vlan")-1)) {
			char *ptrNil = NULL;
			enum VLAN_PORT_SUBIF_FLAG ve_flag;
			unsigned char state = 0; /* l3 interface ustatus*/
			vid = strtoul((char*)&ifname[4],&ptrNil,10);
			if(ifi_flags & (IFF_UP | IFF_RUNNING)) {
				state = 1; 
			}
			ret = npd_vlan_get_interface_ve_flag((unsigned short)vid,&ve_flag);
			if(NPD_SUCCESS == ret && ve_flag == VLAN_PORT_SUBIF_EXIST) {
				syslog_ax_route_dbg("interface %s vid %d advanced-routing\n", ifname, vid);
				npd_vlan_l3intf_set_ustatus(vid,state);
				kapOrSpiIf = 0;/* advanced-routing vlan interface*/
			}
			else if(NPD_TRUE == npd_vlan_interface_check((unsigned short) vid, &ifindex)) {
				syslog_ax_route_dbg("interface %s vid %d kap interface\n", ifname, vid);
				npd_vlan_l3intf_set_ustatus(vid,state);
				kapOrSpiIf = 1;/* vlan interface created by KAP*/
			}
			else {
				kapOrSpiIf = 0xFF;
			}
		}
		else if( !strncasecmp("eth.r", (char*)ifname, sizeof("eth.r")-1)) {
			syslog_ax_route_dbg("interface %s mode route\n", ifname);
			kapOrSpiIf = 1;
		}
		else if( !strncasecmp("eth.p", (char*)ifname, sizeof("eth.p")-1)) {
			syslog_ax_route_dbg("interface %s mode promiscuous\n", ifname);
			kapOrSpiIf = 0;
		}
		else if( !strncasecmp("eth", (char*)ifname, sizeof("eth")-1)||\
			(('e' == ifname[0])&&('0' <= ifname[1])&&('9' >= ifname[1])&&('-' == ifname[2]))) {
			char *ptrNil = "-";
			unsigned char state = 0, link_down = 0,is_subif = 0; /* l3 interface ustatus*/
			unsigned char slot = 0, port = 0;
			unsigned int tag = 0;
			char * slotPtr = NULL;
			char *tagPtr = NULL;
			if(!strncasecmp("eth", (char*)ifname, sizeof("eth")-1)){
				slotPtr = ifname + 3;
			}
			else{
				slotPtr = ifname + 1;
			}
			if(ifi_flags & (IFF_UP | IFF_RUNNING)) {
				state = 1; 
			}

			tagPtr = strchr(ifname, '.');
			slot = strtoul(slotPtr,&ptrNil,10);
			ptrNil = NULL;
			/* add for resolve 10-N,the port is wrong with slotPtr+2, zhangdi 2011-06-29 */
			if(slot > 9)
			{
				/* slot >9 */
				port = strtoul(slotPtr+3, &ptrNil, 10);
			}
			else
			{
				/* 0< slot <9 */
    			port = strtoul(slotPtr+2, &ptrNil, 10);
			}
			if(tagPtr) {
				is_subif = 1;
				tag = strtoul((char*)&tagPtr[1], &ptrNil, 10);
				syslog_ax_route_dbg("interface %s on %d/%d tag %d flag %#x from pid %d\n", ifname, slot, port, tag, ifi_flags, src_pid);
				
			}
			else {
				syslog_ax_route_dbg("interface %s on %d/%d flag %#x from pid %d\n", ifname, slot, port, ifi_flags, src_pid);
			}
			if(CHASSIS_SLOTNO_ISLEGAL(slot) &&
				ETH_LOCAL_PORTNO_ISLEGAL(slot, port)) {
				eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot,port);
				if(NPD_TRUE == npd_check_port_promi_mode(eth_g_index)) {
					kapOrSpiIf = 0;
					/* log it if eth-port is CPU rgmii port */
					if(NPD_TRUE == npd_eth_port_rgmii_type_check(slot, port)) {
						/* IFF_UP && !IFF_RUNNING means NO_CARRIER, which has been view as link down */
						if((0 == src_pid) && !is_subif) { /* notice message skip subif */
							if(ifi_flags & IFF_UP) { /* notic message skip admin down */
								if(!(ifi_flags & IFF_RUNNING)) {
									link_down = 1;
								}
								else {
									link_down = 0;
								}
								syslog7_ax_route_notice("ethernet port %d/%d link %s\n",
												slot, port, (1 == link_down) ? "down" : "up");								
							}
						}
					}
				}
				else if(NPD_TRUE == npd_eth_port_interface_check(eth_g_index, &ifindex)) {
					npd_eth_port_l3intf_ustatus_set(eth_g_index,state);
					kapOrSpiIf = 1;
				}
			}
		}
		else {
			kapOrSpiIf = 0xFF;
		}

		/* promiscuous port or advanced-routing vlan interface*/
		if(0x0 == kapOrSpiIf) {
			/* TODO: call promiscuous port mac update*/
			syslog_ax_route_dbg("currently interface %s not support mac change\n", ifname);
		}
		else if(0x1 == kapOrSpiIf) {
			if(NPD_OK != (ret = npd_intf_get_info((char*)ifname,&ifindex,&vid,&eth_g_index,&type))){
			   syslog_ax_route_err("get interface %s info error\n",ifname);
			   return;
			}
			/* route mode l3 interface*/
			else if((KAP_INTERFACE_PORT_E == type) && 	\
						(NPD_OK != (ret = npd_intf_get_route_mode_mac( eth_g_index,ifindex,oldIntfMac)))){
				syslog_ax_route_err("get interface %s route port %#x mac address error %d\n", \
							ifname, eth_g_index, ret);
				return;
			}
			/* vlan interface via KAP*/
			else if((KAP_INTERFACE_VID_E == type) && 	\
						(NPD_OK != (ret = npd_intf_get_l3_vlan_mac(vid,ifindex,oldIntfMac)))) {
				syslog_ax_route_err("get interface %s vid %d mac address error %d\n", \
							ifname, vid, ret);
				return;
			}
			/* check whether old interface mac the same as system mac and first change*/
			else if((NPD_OK == memcmp(oldIntfMac, dftUniMac, 6)) &&
					(NPD_TRUE == isSysMac)){
				syslog_ax_route_dbg("interface %s mac same as system,no change\n", ifname);
				return;
			}
			else if(NPD_OK == memcmp(ifaddr, oldIntfMac, 6)) {/*check if same with previous  mac*/	
				syslog_ax_route_dbg("verify interface %s new mac found no change with previous mac\n",ifname);
				return;
			}
			else if(NPD_OK != (ret = npd_intf_set_mac_address(ifindex, ifaddr))) {
				syslog_ax_route_err("set interface %s new mac %02x:%02x:%02x:%02x:%02x:%02x error %d\n", \
							ifname, ifaddr[0],ifaddr[1],ifaddr[2],ifaddr[3],ifaddr[4],ifaddr[5], ret);
				return;
			}
			else {/*change the L3 static fdb*/
				if(NPD_OK == memcmp(oldIntfMac, dftUniMac, 6)) {
					npd_system_get_basemac(oldIntfMac,6);
					delOldL3 = (KAP_INTERFACE_PORT_E == type ? 0 : 1);
				}
				
				if(KAP_INTERFACE_PORT_E == type) {
					ret = npd_intf_set_route_mode_mac(eth_g_index, ifindex, \
											(NPD_TRUE == isSysMac) ? dftUniMac : ifaddr);
				}
				else if(KAP_INTERFACE_VID_E == type) {
					ret = npd_intf_set_l3_vlan_mac(vid, ifindex, \
											(NPD_TRUE == isSysMac) ? dftUniMac : ifaddr);
				}
			   	syslog_ax_route_dbg("set interface %s new mac %02x:%02x:%02x:%02x:%02x:%02x %s\n", \
				   						ifname, ifaddr[0],ifaddr[1],ifaddr[2],ifaddr[3],	\
				   						ifaddr[4],ifaddr[5], ret ? "fail":"success");
				if(delOldL3) {
					nam_no_static_fdb_entry_mac_vlan_set(oldIntfMac,vid);
				}
				if((KAP_INTERFACE_VID_E == type) || 
					((KAP_INTERFACE_PORT_E == type)&&(!isSysMac))) {
					nam_static_fdb_entry_mac_set_for_l3(ifaddr,vid,CPU_PORT_VINDEX);
				}
			}			
		}
		else {
			syslog_ax_route_err("can't do any help to manage interface %s\n", ifname);
			return;
		}
	}
		
	return;
}

int new_addr_gratuitous_arp_solicit(void* ptr,int totlemsglen)
{
	
	struct  ifinfomsg *rtEntry = NULL;
	struct nlmsghdr *nlp = (struct nlmsghdr *)ptr;
	int payloadoff = 0,ret = 0;
	struct rtattr *rtattrptr = NULL;
	unsigned char* ifaddr = NULL;
	unsigned char* ifname = NULL;
	unsigned int ifindex = 0,eth_g_index = 0;
	KAP_DEV_TYPE type;
    unsigned int vid = 0;
	int len = 0;
	unsigned int ipAddr = 0;
	struct rtattr *tb[IFA_MAX + 1];	

	memset(&type,0,sizeof(KAP_DEV_TYPE));
	for(;NLMSG_OK(nlp, totlemsglen);nlp=NLMSG_NEXT(nlp, totlemsglen)) {
		rtEntry = (struct ifinfomsg *) NLMSG_DATA(nlp);
		payloadoff = RTM_PAYLOAD(nlp);
		switch( nlp->nlmsg_type ) {
			  case RTM_NEWADDR:
					len = nlp->nlmsg_len - NLMSG_LENGTH (sizeof (struct ifaddrmsg));
					if (len < 0)
						return -1;
					if(nlp->nlmsg_type == RTM_NEWADDR){
						memset (tb, 0, sizeof tb);
						netlink_parse_rtattr (tb, IFA_MAX, IFA_RTA (rtEntry), len);
						memcpy(&ipAddr,RTA_DATA (tb[IFA_LOCAL]),sizeof(unsigned int));
						ipAddr = htonl(ipAddr);
					}
					rtattrptr = (struct rtattr *)IFLA_RTA(rtEntry);
					for(;RTA_OK(rtattrptr, payloadoff);rtattrptr=RTA_NEXT(rtattrptr,payloadoff)) {
						switch(rtattrptr->rta_type) {
							case IFLA_IFNAME:
								npd_syslog_dbg("get ip address %d.%d.%d.%d in %s\n",\
									(ipAddr>>24)&0xff,(ipAddr>>16)&0xff,(ipAddr>>8)&0xff,ipAddr&0xff,__func__);
								ifname = (unsigned char*)RTA_DATA(rtattrptr);
								npd_syslog_dbg("get ifname %s in %s\n",ifname,__func__);
								/* Jump the VE-subintf bug:	AXSSZFI-1052. zhangdi@autelan.com 2012-08-14 */
								#if 0
								if(0 != strcmp(ifname,"lo"))
								#else
								/* just get for L3 eth-port and L3 vlan */
								if((!strncmp(ifname,"eth",3))||(ifname,"vlan",4))									
								#endif
								{
									ret = npd_intf_get_info((char*)ifname,&ifindex,&vid,&eth_g_index,&type);
									if(NPD_SUCCESS == ret){
	       								ret = npd_arp_snooping_gratuitous_send_for_new_ipaddr(ifindex,ipAddr,(unsigned short)vid,eth_g_index,type);
										if(NPD_SUCCESS != ret){
											npd_syslog_dbg("gratuitous arp solicit send for new ip %d.%d.%d.%d ret %d\n", \
												(ipAddr>>24)&0xff,(ipAddr>>16)&0xff,(ipAddr>>8)&0xff,ipAddr&0xff,ret);
										}
									}
									else{
	                                    npd_syslog_err("npd_intf_get_info error ret %d \n",ret);
									}
								}
								else{
                                    npd_syslog_dbg("interface lo don't send arp solicit\n");
								}
								break;
							default:						
								break;
						}
					}
					break;
			  default:
				break;
		}		    
	}	
	return ret;
}


void syn_kernelRT_to_drv(int sockfd)
{
	int msglen=0;
	static char buf[BUFLENGTH];
	
	struct iovec iov = { buf, sizeof buf };
	struct sockaddr_nl snl ;
	struct msghdr msg = { (void *) &snl, sizeof snl, &iov, 1, NULL, 0, 0 };
	
	char* p = buf;
	struct nlmsghdr *nlp = (struct nlmsghdr *)p,*tmp=NULL;
	while(1)
	{
		msglen = recvmsg(sockfd, &msg, 0);
		if(msglen <= 0)
		{
			 syslog_ax_route_err("recv msg error %s\n", strerror(errno));
			continue;
		}
		
		tmp = (struct nlmsghdr *) p;
		if(tmp->nlmsg_flags == NLM_F_MULTI && tmp->nlmsg_type == NLMSG_DONE  ) 
		{
			syslog_ax_route_err("ignore NLMSG_DONE message\n");
			continue;
		}
		/*zhangcl@autelan.com clean the print message */
		#if 0
		syslog_ax_route_dbg("netlink socket %d rcv msg type %s len %d from pid %d group %#x\n",  \
							sockfd, NL_MSG_TYPE_DESC(nlp->nlmsg_type),msglen, snl.nl_pid, snl.nl_groups);
		#endif
		write_msg_to_drv_for_syn(nlp,msglen,snl.nl_pid);
		read_newlink(nlp,msglen, snl.nl_pid);
		read_ip(nlp,msglen);
		new_addr_gratuitous_arp_solicit(nlp,msglen);
	}

}

void form_Request_for_get_kRTs(struct netlink_req* req)
{
  	bzero(req, sizeof(struct netlink_req));		/* code optimize: Wrong sizeof argument houxx@autelan.com  2013-1-19 */

  	req->nl.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
  	req->nl.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
  	req->nl.nlmsg_type = RTM_GETROUTE;

  	req->rt.rtm_family = AF_INET;
  	req->rt.rtm_table = RT_TABLE_MAIN;
}

int send_Request_for_get_kRTs(int sockfd,struct netlink_req* req)
{
	struct msghdr msg;
	struct sockaddr_nl pa;
	struct iovec iov;

  	bzero(&pa, sizeof(pa));
  	pa.nl_family = AF_NETLINK;
  	bzero(&msg, sizeof(msg));
  	msg.msg_name = (void *) &pa;
  	msg.msg_namelen = sizeof(pa);

  	iov.iov_base = (void *) &req->nl;
  	iov.iov_len = req->nl.nlmsg_len;
  	msg.msg_iov = &iov;
  	msg.msg_iovlen = 1;
  	return sendmsg(sockfd, &msg, 0);
}

GT_STATUS copy_fib2mvdrv()
{
	int fd;
	int totlemsglen=BUFLENGTH;
	struct netlink_req req;
	struct sockaddr_nl la;
	static char buf[BUFLENGTH] = {0};

	fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	if(fd<0)
	{
		 syslog_ax_route_err("create socket error when copy fib to asic\n");
		return GT_ERROR;
	}
	
	bzero(&la, sizeof(la));
	la.nl_family = AF_NETLINK;
	la.nl_pid = getpid();
#ifdef CPU_ARM_XCAT
	la.nl_pid += g_npd_thread_number;
#endif
	if(bind(fd, (struct sockaddr*) &la, sizeof(la)))
	{
		syslog_ax_route_err("bind socket error when copy fib to asic\n");
		close(fd);	/* code optimize: recourse leak houxx@autelan.com  2013-1-19 */
		return GT_ERROR;
	}

	form_Request_for_get_kRTs(&req);
	if(send_Request_for_get_kRTs(fd,&req)<0)
	{		/* code optimize: Unchecked return value from library houxx@autelan.com  2013-1-19 */
		syslog_ax_route_err("send RT Netlink message from kernel error\n");
		close(fd);	/* code optimize: recourse leak houxx@autelan.com  2013-1-19 */
		return GT_ERROR;
	}
	if(recv_rtMSG_from_kernel(fd,(void*)buf,&totlemsglen))
	{
		syslog_ax_route_err("receive RT Netlink message from kernel error\n");
		close(fd);	/* code optimize: recourse leak houxx@autelan.com  2013-1-19 */
		return GT_ERROR;
	}

	write_msg_to_drv((void*)buf,totlemsglen);

	close(fd);

	return GT_OK;
}
unsigned char syn_fib2mvdrv()
{
	int fd = -1;
	struct sockaddr_nl la;
	
	/* tell my thread id*/
	npd_init_tell_whoami("NpdRoute",0);
	
	fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	if(fd<0)
	{
		 syslog_ax_route_err("create socket error when sync fib to asic\n");
		return GT_ERROR;
	}
	
	bzero(&la, sizeof(la));
	la.nl_family = AF_NETLINK;
	la.nl_pid = getpid();
	#ifdef CPU_ARM_XCAT
	la.nl_pid += g_npd_thread_number;
	#endif
	la.nl_groups = RTMGRP_IPV4_IFADDR|RTMGRP_IPV4_ROUTE |RTMGRP_LINK| RTMGRP_NOTIFY ;

	if(bind(fd, (struct sockaddr*) &la, sizeof(la)))
	{
		syslog_ax_route_err("bind socket error when sync fib to asic\n");
		close(fd);	/* code optimize: recourse leak houxx@autelan.com  2013-1-19 */
		return GT_ERROR;
	}
	syn_kernelRT_to_drv(fd);
	close(fd);	/* code optimize: recourse leak houxx@autelan.com  2013-1-19 */
	return GT_OK;
}

int npd_route_init()
{
	/*if(PRODUCT_ID_AX5K_E == PRODUCT_ID || PRODUCT_ID_AX5608== PRODUCT_ID){
		return;
	}*/

	if(!((productinfo.capbmp) & FUNCTION_ROUTE_VLAUE)){
		npd_syslog_dbg("do not support route initial!\n");
		return -1;		/* code optimize: recourse leak houxx@autelan.com  2013-1-19 */
	}
	else if(((productinfo.capbmp) & FUNCTION_ROUTE_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ROUTE_VLAUE)){
		npd_syslog_dbg("do not support route initial!\n");
		return -1;		/* code optimize: recourse leak houxx@autelan.com  2013-1-19 */
	}
	else{
		copy_fib2mvdrv();
		unsigned int ret = 0;
		ret = nam_ipCnt_mode_set(0,0,1,NULL);
		nam_thread_create("NpdRoute",(void *)syn_fib2mvdrv,NULL,NPD_TRUE,NPD_FALSE);

		return 0;
	}
}

GT_STATUS get_tblIndex_by_dip_mask(unsigned int DIP,unsigned int masklen,unsigned int *tblIndex)
{
    unsigned char status = -1;
	/*IP_TCAM_ROUTE_ENTRY_INFO_UNT defUcNextHop;*/
	unsigned int entryIndex = 0;
	status = lpm_entry_search(htonl(DIP),masklen,&entryIndex);	/* code optimize: Uninitialized scalar variable houxx@autelan.com  2013-1-19 */
    if(!status){
		/* *tblIndex = defUcNextHop.ipLttEntry.routeEntryBaseIndex;*/
        *tblIndex = entryIndex;
	}
	else{
		*tblIndex = 0;
	}
	return status;
}

GT_STATUS npd_route_get_route_tblIndex_by_dip(unsigned int DIP,unsigned int *tblIndex)
{
    int tmpMask = 0;
	int ret = -1;
	unsigned int tmpIndex = 0;
	*tblIndex = 0;
	for(tmpMask = 32;tmpMask >= 0;tmpMask--){
        ret = get_tblIndex_by_dip_mask(DIP,tmpMask,tblIndex);
		if((!ret)&&(*tblIndex > 2)){
            return GT_OK;
		}
		if((!tmpIndex)&&(*tblIndex)){
            tmpIndex = *tblIndex;
		}
	}
	*tblIndex = tmpIndex;
	return GT_NOT_FOUND;
}

DBusMessage * show_rtdrv_all(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusMessageIter	 iter_array;
	unsigned int Dip=~0UI,masklen=32;
    unsigned int baseindex = 0;
	unsigned int route_count = 0;
	int ret = 0,i = 0;
	drv_infos info[4096];
	memset(info,0,sizeof(drv_infos)*4096);
	nam_route_count_get(&route_count);
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &route_count);
	dbus_message_iter_open_container (&iter,
									   DBUS_TYPE_ARRAY,
									   DBUS_STRUCT_BEGIN_CHAR_AS_STRING
											DBUS_TYPE_UINT32_AS_STRING
											DBUS_TYPE_UINT32_AS_STRING
											DBUS_TYPE_UINT32_AS_STRING
											DBUS_STRUCT_END_CHAR_AS_STRING,
									   &iter_array);
	ret = nam_route_get_info(&info);
	for(i=0;i<route_count;i++){
		DBusMessageIter iter_struct;
		
		dbus_message_iter_open_container (&iter_array,
										   DBUS_TYPE_STRUCT,
										   NULL,
										   &iter_struct);
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &info[i].dip);
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &info[i].masklen);
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &info[i].baseindex);

		dbus_message_iter_close_container (&iter_array, &iter_struct);
	}
	dbus_message_iter_close_container (&iter, &iter_array);
		
	return reply;

}

DBusMessage * show_hostdrv_all(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusMessageIter	 iter_array;
	unsigned int Dip=~0UI,masklen=32;
    unsigned int baseindex = 0;
	int ret = 0,i = 0;
	static host_infos info[8192];
	memset(info,0,sizeof(host_infos)*8192);
	reply = dbus_message_new_method_return(msg);
	ret = nam_host_get_info(&info);
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	if(0 == ret){
       return reply;
	}
	dbus_message_iter_open_container (&iter,
									   DBUS_TYPE_ARRAY,
									   DBUS_STRUCT_BEGIN_CHAR_AS_STRING
											DBUS_TYPE_UINT32_AS_STRING
											DBUS_TYPE_UINT32_AS_STRING
											DBUS_TYPE_UINT32_AS_STRING
											DBUS_TYPE_BYTE_AS_STRING
											DBUS_TYPE_BYTE_AS_STRING
											DBUS_TYPE_BYTE_AS_STRING
											DBUS_TYPE_BYTE_AS_STRING
											DBUS_TYPE_BYTE_AS_STRING
											DBUS_TYPE_BYTE_AS_STRING
											DBUS_TYPE_BYTE_AS_STRING
											DBUS_TYPE_BYTE_AS_STRING
										DBUS_STRUCT_END_CHAR_AS_STRING,
									   &iter_array);
	for(i=0;i<ret;i++){
		DBusMessageIter iter_struct;
		
		dbus_message_iter_open_container (&iter_array,
										   DBUS_TYPE_STRUCT,
										   NULL,
										   &iter_struct);
		dbus_message_iter_append_basic (&iter_struct,
								 DBUS_TYPE_UINT32, 
								 &info[i].ip);
		
		dbus_message_iter_append_basic (&iter_struct,
								 DBUS_TYPE_UINT32, 
								 &info[i].baseindex);		

		dbus_message_iter_append_basic (&iter_struct,
								 DBUS_TYPE_UINT32, 
								 &info[i].ifindex);	
		
		dbus_message_iter_append_basic (&iter_struct,
								 DBUS_TYPE_BYTE, 
								 &info[i].dev);
		
		dbus_message_iter_append_basic (&iter_struct,
								 DBUS_TYPE_BYTE, 
								 &info[i].port);
		
		dbus_message_iter_append_basic (&iter_struct,
								 DBUS_TYPE_BYTE, 
								 &(info[i].mac[0]));
		dbus_message_iter_append_basic (&iter_struct,
								 DBUS_TYPE_BYTE, 
								 &(info[i].mac[1]));
		dbus_message_iter_append_basic (&iter_struct,
								 DBUS_TYPE_BYTE, 
								 &(info[i].mac[2]));
		dbus_message_iter_append_basic (&iter_struct,
								 DBUS_TYPE_BYTE, 
								 &(info[i].mac[3]));
		dbus_message_iter_append_basic (&iter_struct,
								 DBUS_TYPE_BYTE, 
								 &(info[i].mac[4]));
		dbus_message_iter_append_basic (&iter_struct,
								 DBUS_TYPE_BYTE, 
								 &(info[i].mac[5]));

		dbus_message_iter_close_container (&iter_array, &iter_struct);
	}
	dbus_message_iter_close_container (&iter, &iter_array);
		
	return reply;

}
DBusMessage * show_rtdrv_entry(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusMessageIter  iter;
	unsigned int	dip=0,masklen=0;
	unsigned int ret = 0,result = 0;
	IP_TCAM_ROUTE_ENTRY_INFO_UNT defUcNextHop = {{0}};
	unsigned int entryIndex = 0;
	DBusError 		err;
	struct arp_snooping_item_s arpInfo;
	unsigned int refCnt = 0;

	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT32,&dip,
		DBUS_TYPE_UINT32,&masklen,
		DBUS_TYPE_INVALID)))
		{		
			 syslog_ax_route_dbg("Unable to get input args \n");
			if (dbus_error_is_set(&err)) 
			{
				 syslog_ax_route_err("%s raised: %s\n",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}
	 syslog_ax_route_dbg("DIP = %X,masklen = %d\n",ntohl(dip),masklen);
	 ret = lpm_entry_search(dip,masklen,&entryIndex);

	/* read next-hop ARP info from HW table*/
	if(0 == ret) {
		memset(&arpInfo, 0,sizeof(struct arp_snooping_item_s));
		arpInfo.ipAddr = ntohl(dip);
		pthread_mutex_lock(&nexthopHashMutex);	
		result = npd_route_nexthop_iteminfo_get(0,entryIndex, &arpInfo,&refCnt);
		pthread_mutex_unlock(&nexthopHashMutex);

		/*only used for broadcom asic,considered it later with HanHui or QinHuasong*/
		/*ret = nam_get_drv_route_info(dip,masklen,&arpInfo);*/
		if(ROUTE_RETURN_CODE_ACTION_TRAP2CPU == result) {
			ret = NPD_ROUTE_NEXTHOP_ERR_ACTION_TRAP2CPU;
		}
		else if(ROUTE_RETURN_CODE_ACTION_HARD_DROP == result){
			ret = NPD_ROUTE_NEXTHOP_ERR_ACTION_HARD_DROP;
		}
	}
#if 0	
	if (0 != ret) {
		 syslog_ax_route_err(("npd_route::lpm_entry_search::ERROR.\n"));
	}
#endif

	reply = dbus_message_new_method_return(msg);
		
	dbus_message_iter_init_append (reply, &iter);
		
	dbus_message_iter_append_basic (&iter,
							 DBUS_TYPE_UINT32, 
							 &ret);
	
	dbus_message_iter_append_basic (&iter,
							 DBUS_TYPE_UINT32, 
							 &entryIndex);
	
	dbus_message_iter_append_basic (&iter,
							 DBUS_TYPE_UINT16, 
							 &arpInfo.vid);
	
	dbus_message_iter_append_basic (&iter,
							 DBUS_TYPE_UINT32, 
							 &arpInfo.ifIndex);
	
	dbus_message_iter_append_basic (&iter,
							 DBUS_TYPE_BYTE, 
							 &arpInfo.dstIntf.isTrunk);
	if(TRUE == arpInfo.dstIntf.isTrunk){
			
		dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_BYTE, 
								 &arpInfo.dstIntf.intf.trunk.devNum);
		
		dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_BYTE, 
								 &arpInfo.dstIntf.intf.trunk.trunkId);
	}
	else {
			
		dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_BYTE, 
								 &arpInfo.dstIntf.intf.port.devNum);

		dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_BYTE, 
								 &arpInfo.dstIntf.intf.port.portNum);
	}

	dbus_message_iter_append_basic (&iter,
							 DBUS_TYPE_BYTE, 
							 &(arpInfo.mac[0]));
	dbus_message_iter_append_basic (&iter,
							 DBUS_TYPE_BYTE, 
							 &(arpInfo.mac[1]));
	dbus_message_iter_append_basic (&iter,
							 DBUS_TYPE_BYTE, 
							 &(arpInfo.mac[2]));
	dbus_message_iter_append_basic (&iter,
							 DBUS_TYPE_BYTE, 
							 &(arpInfo.mac[3]));
	dbus_message_iter_append_basic (&iter,
							 DBUS_TYPE_BYTE, 
							 &(arpInfo.mac[4]));
	dbus_message_iter_append_basic (&iter,
							 DBUS_TYPE_BYTE, 
							 &(arpInfo.mac[5]));
	dbus_message_iter_append_basic (&iter,
							 DBUS_TYPE_UINT32, 
							 &refCnt);
			
	return reply;
}

#define WORD_SWAP(X)  ((((X)&0xff)<<24) |						   \
                                  (((X)&0xff00)<<8) |             \
                                  (((X)&0xff0000)>>8) |           \
                                  (((X)&0xff000000)>>24))

#if 0
static _GT_IPADDR Dip={0},masklen={0};
static unsigned int mc=0;
#else
static GT_IPADDR Dip=0,masklen=0;

#endif

DBusMessage * config_ucrpf_enable(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusMessageIter  iter;
	unsigned char	rpfen=0;
	unsigned char    ucRPFcheck = 0;
	DBusError 		err;
	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_BYTE,&rpfen,
		DBUS_TYPE_INVALID)))
		{		
			 syslog_ax_route_dbg("Unable to get input args\n ");
			if (dbus_error_is_set(&err)) 
			{
				 syslog_ax_route_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}
	 syslog_ax_route_dbg("rpfen = %X\n",rpfen);
	 nam_route_set_ucrpf_enable(rpfen);
	 ucRPFcheck= rpfen;
	 reply = dbus_message_new_method_return(msg);
		 
	 dbus_message_iter_init_append (reply, &iter);
		 
	 dbus_message_iter_append_basic (&iter,
							  DBUS_TYPE_BYTE, 
							  &ucRPFcheck);

	return reply;
}



DBusMessage * show_ucrpf_enable(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusMessageIter  iter;

	unsigned char ucRPFcheck = 0;
	nam_route_get_ucrpf(&ucRPFcheck);
	
	 reply = dbus_message_new_method_return(msg);
		 
	 dbus_message_iter_init_append (reply, &iter);
		 
	 dbus_message_iter_append_basic (&iter,
							  DBUS_TYPE_BYTE, 
							  &ucRPFcheck);

	return reply;
}

DBusMessage * config_route_status(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
    GT_STATUS                      result;
    GT_U8                          devNum = 0 ;
    IP_COUNTER_SET_STC   counters;
	IP_CNT_SET_ENT	  cntSetCnt;

	DBusMessage* reply;
	DBusError 		err;

	dbus_error_init(&err);

	if (!(dbus_message_get_args (msg, &err,
		DBUS_TYPE_BYTE,&cntSetCnt,
		DBUS_TYPE_INVALID)))
		{		
			if (dbus_error_is_set(&err)) 
			{
				 syslog_ax_route_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}
	
	if(cntSetCnt > 3)
	{
		syslog_ax_route_err("cntSetCnt is %d\n",cntSetCnt);
		return NULL;

	}
	result = nam_route_ipCnt_get(devNum,cntSetCnt,&counters);
	reply = dbus_message_new_method_return(msg);
	if (result != GT_OK)
	{
		syslog_ax_route_err("%s raised: %s\n",err.name,err.message);
		result = 1;
	}
		return reply;

}

DBusMessage * show_route_status(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
    int                     result;
	unsigned int rv = 0;
    GT_U8                          devNum = 0 ;

    IP_COUNTER_SET_STC   counters;
	IP_CNT_SET_ENT	  cntSetCnt;

	DBusMessage* reply;
	DBusMessageIter  iter;
	DBusError 		err;

	dbus_error_init(&err);

	if (!(dbus_message_get_args (msg, &err,
		DBUS_TYPE_BYTE,&cntSetCnt,
		DBUS_TYPE_INVALID)))
		{		
			if (dbus_error_is_set(&err)) 
			{
				 syslog_ax_route_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}
	if(cntSetCnt >= 3)
	{
		syslog_ax_route_err("cntSetCnt is %d\n",cntSetCnt);
		return NULL;

	}
	result = nam_route_ipCnt_get(devNum, cntSetCnt, &counters);
	if (result != GT_OK)
	{
		syslog_ax_route_err("can't get counter\n");
		return NULL;
	}

	reply = dbus_message_new_method_return(msg);
	if (!reply)
	{
		syslog_ax_route_err("reply is NULL\n");
		return reply;
	}

	dbus_message_iter_init_append (reply, &iter);
		
		dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_BYTE, &result);

		dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32, &counters.inUcPkts);
		dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32, &counters.inMcPkts);
		dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32, &counters.inUcNonRoutedExcpPkts);
		dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32, &counters.inUcNonRoutedNonExcpPkts);
		dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32, &counters.inMcNonRoutedExcpPkts);
		dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32, &counters.inMcNonRoutedNonExcpPkts);
		dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32, &counters.inUcTrappedMirrorPkts);
		dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32, &counters.inMcTrappedMirrorPkts);
		dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32, &counters.mcRfpFailPkts);
		dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32, &counters.outUcRoutedPkts);

		return reply;
}

/**********************************************************************************
 * npd_dbus_route_show_running_config
 *	This method is used to save route configuration
 *
 *	INPUT:
 *		char* showStr
 *		totalLength
 *		currentLen
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_dbus_route_show_running_config
(
	char ** showStr,
	int* avalidLen
)
{
#define ROUTE_RPF_ENABLE 1
#define ROUTE_RPF_DISABLE 0
	int len = 0, ret = 0;
	unsigned char ucRPFcheck = 0;
	
	if((NULL == showStr)||(NULL == *showStr )||(NULL == avalidLen)) {
		return -1;
	}
	else if( 30 > *avalidLen)
		return -1;
	else {
		nam_route_get_ucrpf(&ucRPFcheck);
		if(ROUTE_RPF_DISABLE == ucRPFcheck) {
			len = sprintf((*showStr),"config rpf disable\n");
			*showStr += len;
			*avalidLen -= len;
		}		
	}
	return 0;
}

#ifdef __cplusplus
}
#endif
