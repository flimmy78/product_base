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
* nam_route.c
*
*
* CREATOR:
*		flash@autelan.com
*
* DESCRIPTION:
*		APIs used in NAM for L3 routing process.
*
* DATE:
*		03/20/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.24 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/rtnetlink.h>
#include <linux/in.h>
#include <pthread.h>
#include <bits/sockaddr.h>
#include <sysdef/npd_sysdef.h>

#ifdef DRV_LIB_BCM
#include <bcm/l3.h>
#endif

#include "util/npd_list.h"
#include "sysdef/npd_sysdef.h"
#include "sysdef/returncode.h"
#include "npd/nam/npd_amapi.h"
#include "nam_asic.h"
#include "nam_log.h"
#include "nam_route.h"

unsigned char gIndex;
extern pthread_mutex_t semRtRwMutex;
extern pthread_mutex_t nexthopHashMutex;
#ifdef DRV_LIB_CPSS
static unsigned char ucRPFCheckEnable=1;
#endif

static unsigned char  maskbit[] = 
 {
   0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff
 };

int i2xdigit(int digit)
{
    digit &= 0xf;
    return (digit > 9) ? digit - 10 + 'a' : digit + '0';
}
void get_mask_by_ip_masklen(unsigned int ip,unsigned int masklen,int* mask)
{

  unsigned int pnt[4]={0};
  int host,index;
  int offset;

  index = masklen / 8;

  if (index < 4){
  	 /* pnt = (unsigned int *) &masklen;*/
      offset = masklen % 8;
	  
  	  for(host = 0;host < index;host++)
	  	 pnt[host] = 0xff;
	  
      pnt[index] = maskbit[offset];  
      index++;  
      while (index < 4)
	     pnt[index++] = 0;
  }
  host = 0;
  while(host < 4){
    *mask += pnt[host]<<(( 3 - host)*8);
    host++;
  }
}

void print_lpm_info(int unit,unsigned int DIP,unsigned int masklen)
{
#ifdef DRV_LIB_CPSS
    unsigned int                                   lpmDBId=0;
    unsigned int                                   vrId=0;
    GT_IPADDR                                ipAddr;
    unsigned int                                   prefixLen = masklen;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   defUcNextHop = {{0}};
    int i;
    char*p;
    unsigned char status;
#ifdef DRV_LIB_CPSS_3_4
		memcpy(ipAddr.arIP,&DIP,sizeof(unsigned int));
#else
		ipAddr = DIP;	
#endif

    
    status =  cpssDxChIpLpmIpv4UcPrefixSearch(lpmDBId, vrId, ipAddr, prefixLen,
                                              &defUcNextHop);
    if(status)
    {
    	nam_syslog_err("can't find the entry\n");
    	return;
    }	
    p = (char*) &defUcNextHop;
	
    for(i=0;i<sizeof(defUcNextHop);i++)
    {
    	if(i%16==0)
    		 nam_syslog_dbg("\n");
    	 nam_syslog_dbg("0x%2x ",(unsigned char)*(p+i));
    }
     nam_syslog_dbg("\n");
#endif

#ifdef DRV_LIB_BCM
     int ret = 0,num_of_asic = 0;
     bcm_l3_route_t rtinfo;
	 memset(&rtinfo,0,sizeof(bcm_l3_route_t));

 	 rtinfo.l3a_subnet = DIP;
	 get_mask_by_ip_masklen(DIP,masklen,&rtinfo.l3a_ip_mask);
     ret = bcm_l3_route_get(unit,&rtinfo);	 
	 if(0 == ret){
		nam_syslog_dbg("unit %d: flags %#x,vrf %d,dip %#x,intf %d,nexthop mac %02x:%02x:%02x:%02x:%02x:%02x\n",\
			     unit,rtinfo.l3a_flags,rtinfo.l3a_vrf,rtinfo.l3a_subnet,rtinfo.l3a_intf,rtinfo.l3a_nexthop_mac[0],\
			     rtinfo.l3a_nexthop_mac[1],rtinfo.l3a_nexthop_mac[2],rtinfo.l3a_nexthop_mac[3],rtinfo.l3a_nexthop_mac[4],\
			     rtinfo.l3a_nexthop_mac[5]);
		nam_syslog_dbg("modid %d,port(trunk) %d,nexthop index %#x\n",rtinfo.l3a_modid,rtinfo.l3a_port_tgid,rtinfo.l3a_nexthop_hw_idx);
	 }
	 memset(&rtinfo,0,sizeof(bcm_l3_route_t));
	 
#endif
}

#ifdef DRV_LIB_BCM
void
format_macaddr(char buf[MACADDR_STR_LEN], eth_mac_t macaddr)
{
    int i;

    for (i = 0; i <= 5; i++) {
        *buf++ = i2xdigit(macaddr[i] >> 4);
        *buf++ = i2xdigit(macaddr[i]);
        *buf++ = ':';
    }

    *--buf = 0;
}

int
format_ipaddr_masklen(unsigned int mask)
{
    int masklen = 0;
	int i = 0;
    for (i = 0; i < 32; i++) {
        if (mask & 0x80000000) {
            masklen++;
            mask = mask << 1;
        } else {
            break;
        }
    }
	return masklen;
}
void
format_ipaddr_mask(char *buf, unsigned int ipaddr, unsigned int mask)
{
    int masklen, i;

    /* Convert netmask to number of bits */ 

    masklen = 0;
    for (i = 0; i < 32; i++) {
        if (mask & 0x80000000) {
            masklen++;
            mask = mask << 1;
        } else {
            break;
        }
    }
    sprintf(buf, "%d.%d.%d.%d/%d",
	    (ipaddr >> 24) & 0xff, (ipaddr >> 16) & 0xff,
	    (ipaddr >> 8) & 0xff, ipaddr & 0xff, masklen);
}

void
format_ip6addr(char buf[IP6ADDR_STR_LEN], ip6_addr ipaddr)
{
    sprintf(buf, "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x", 
            (((unsigned short)ipaddr[0] << 8) | ipaddr[1]),
            (((unsigned short)ipaddr[2] << 8) | ipaddr[3]),
            (((unsigned short)ipaddr[4] << 8) | ipaddr[5]),
            (((unsigned short)ipaddr[6] << 8) | ipaddr[7]),
            (((unsigned short)ipaddr[8] << 8) | ipaddr[9]),
            (((unsigned short)ipaddr[10] << 8) | ipaddr[11]),
            (((unsigned short)ipaddr[12] << 8) | ipaddr[13]),
            (((unsigned short)ipaddr[14] << 8) | ipaddr[15]));
}

int
nam_ip6_mask_length(ip6_addr mask)
{
    int masklen, i, j;
    unsigned char temp;
            
    /* Convert netmask to number of bits */
    masklen = 0;
    
    for (i = 0; i < 16; i++) {
        temp = mask[i];
        for (j = 0; j < 8; j++) {
            if (temp & 0x80) {
                masklen++;
                temp = temp << 1;
            } else {
                return masklen;
            }
        }
    }

    return masklen;
}
void print_route_info
(
     bcm_l3_route_t rtinfo
)
{
    char ip_str[IP6ADDR_STR_LEN + 3];
    char if_mac_str[MACADDR_STR_LEN];
    char vrf_str[20];
    char *hit;
    char *trunk;
    int masklen;
    int priority;
    
    hit = (rtinfo.l3a_flags & NAM_L3_HIT) ? "y" : "n";
    trunk = (rtinfo.l3a_flags & NAM_L3_TGID) ? "t" : " ";
    priority = rtinfo.l3a_pri;

    switch (rtinfo.l3a_vrf){
      case NAM_L3_VRF_GLOBAL:
          strcpy(vrf_str,"Global"); 
          break;
      case NAM_L3_VRF_OVERRIDE:
          strcpy(vrf_str,"Override"); 
          break;
      default:
          sprintf(vrf_str,"%d",rtinfo.l3a_vrf);
    }

    format_macaddr(if_mac_str, rtinfo.l3a_nexthop_mac);

    if (rtinfo.l3a_flags & NAM_L3_IP6) {
        format_ip6addr(ip_str, rtinfo.l3a_ip6_net);
        masklen = nam_ip6_mask_length(rtinfo.l3a_ip6_mask);
        nam_syslog_dbg(" %-8s %-s/%d %18s %4d %4d %5d%1s %4d %4d %s", vrf_str,
               ip_str, masklen, if_mac_str, rtinfo.l3a_intf,
               rtinfo.l3a_modid, rtinfo.l3a_port_tgid, trunk, priority, 
               rtinfo.l3a_lookup_class, hit);
    } else {
        format_ipaddr_mask(ip_str, rtinfo.l3a_subnet, rtinfo.l3a_ip_mask);
        nam_syslog_dbg("vrf %-8s, ip %-19s,mac %18s, intf %4d,modif %4d,port %5d,trunk %1s,priority %4d,lookup class %4d,hit %s\n", 
			
             vrf_str,  ip_str, if_mac_str, rtinfo.l3a_intf,
               rtinfo.l3a_modid, rtinfo.l3a_port_tgid, trunk, priority, 
               rtinfo.l3a_lookup_class, hit);
    }

    if (rtinfo.l3a_flags & NAM_L3_MULTIPATH) {
        nam_syslog_dbg("      (ECMP)\n");
    } else if (rtinfo.l3a_flags & NAM_L3_DEFIP_LOCAL) {
        nam_syslog_dbg("      (LOCAL ROUTE)\n");
    } 

    if (rtinfo.l3a_flags & NAM_L3_ROUTE_LABEL) {
        nam_syslog_dbg(" (MPLS)%d\n", rtinfo.l3a_mpls_label);
    } else if (rtinfo.l3a_tunnel_option) {
        nam_syslog_dbg(" (UDP tunnel) %d\n", rtinfo.l3a_tunnel_option);
    } else { 
       nam_syslog_dbg("\n");
    }
    return NAM_E_NONE;
}

#endif
/*LATER CONSIDER THE BCM */
int  nam_route_override_default_route
(
	unsigned int DIP
)
{
#ifdef DRV_LIB_CPSS
	unsigned int                            routeEntryBaseIndex = 0;
	unsigned long	                        status = 0;
    unsigned long                           override = 1;/*set  true to override the old value*/
	unsigned int							lpmDBId = 0;
	unsigned int							vrId = 0;
	GT_IPADDR								ipAddr;
	unsigned int							prefixLen = 0;
	CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT	defUcNextHop;
	char* p = NULL;
#endif

#ifdef DRV_LIB_BCM
    int ret = 0,unit = 0,num_of_asic = 0;
    bcm_l3_route_t rtinfo;
	memset(&rtinfo,0,sizeof(bcm_l3_route_t));
	num_of_asic = nam_asic_get_instance_num();
#endif
#ifdef DRV_LIB_CPSS
    routeEntryBaseIndex = 0;/* default route entry nexthop index 0*/

	lpmDBId = 0; /*LPM表ID 应该在初始化时指定该值目前用0*/
	vrId = 0;	  /*在代码中看到该值定义了，但未实现，设为0即可*/
#ifdef DRV_LIB_CPSS_3_4
		memcpy(ipAddr.arIP,&DIP,sizeof(unsigned int));
#else
		ipAddr = DIP;	
#endif

	prefixLen = 0;/*masklen,default route is 0*/
	memset(&defUcNextHop, 0, sizeof(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT));
	defUcNextHop.ipLttEntry.routeEntryBaseIndex = (unsigned int)routeEntryBaseIndex;
	defUcNextHop.ipLttEntry.ucRPFCheckEnable = (unsigned char)ucRPFCheckEnable;


	p = (char*) &defUcNextHop;

	/* call cpss api function */
	pthread_mutex_lock(&semRtRwMutex);
	status =  cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen,
									   &defUcNextHop, override);
	pthread_mutex_unlock(&semRtRwMutex);

	if(status == 0){
		gIndex++;
		nam_syslog_err("override default route entry success!\n",status);
	}
	else{
		 nam_syslog_err("override default route entry error for %d\n",status);
	}
	return status;
#endif

#ifdef DRV_LIB_BCM
   rtinfo.l3a_subnet = DIP;
   for(unit = 0;unit < num_of_asic;unit++){
   	  ret = bcm_l3_route_get(unit,&rtinfo);
	  if(0 != ret){
	  	nam_syslog_err("Fail to get default route for ret % on unit %d\n",ret,unit);
        return ret;
	  }
	  rtinfo.l3a_flags |= (NAM_L3_DST_DISCARD | NAM_L3_REPLACE) ;/*DROP THE PACKET*/
      ret |= bcm_l3_route_add(unit,&rtinfo);
	  if(NAM_E_NONE != ret){
          nam_syslog_err("Fail to delete default route for ret % on unit %d\n",ret,unit);
	  }
   }
   #if 0
   if(NAM_E_NONE == ret){
      gIndex--;//default route in broadcom is not set in the init,so if delete it,should destroy the former entry
   }
   else{
		 nam_syslog_err("override default route entry error for %d\n",,status);
   }
   #endif
   if(NAM_E_NONE!= ret){
      nam_syslog_err("override default route entry error for %d\n",ret);
   }
   return ret;
#endif
}

unsigned long  LPM_TBL_HOST_ENTRY_SET
(
	unsigned int DIP,
	unsigned int masklen,
	unsigned int index
) 
{

   	unsigned long   status = 0;
   	GT_IPADDR										ipAddr;
#ifdef DRV_LIB_CPSS
   	unsigned int                                   lpmDBId = 0;
   	unsigned int                                   vrId = 0;
   	unsigned int                                   prefixLen;
   	CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   defUcNextHop;
	unsigned int   override=1;
#endif

#ifdef DRV_LIB_CPSS

#ifdef DRV_LIB_CPSS_3_4
			memcpy(ipAddr.arIP,&DIP,sizeof(unsigned int));
#else
			ipAddr = DIP;	
#endif

    prefixLen = masklen;
	if(prefixLen != 32)
	{
		 nam_syslog_err("notes : in func  LPM_TBL_HOST_ENTRY_SET masklen isn't 32\n");
	}
	memset(&defUcNextHop, 0, sizeof(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT));
    defUcNextHop.ipLttEntry.routeType = CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    defUcNextHop.ipLttEntry.numOfPaths = (unsigned int)0;
    defUcNextHop.ipLttEntry.routeEntryBaseIndex = (unsigned int)index;
    defUcNextHop.ipLttEntry.ucRPFCheckEnable = ucRPFCheckEnable;
    defUcNextHop.ipLttEntry.sipSaCheckMismatchEnable = 0;
    defUcNextHop.ipLttEntry.ipv6MCGroupScopeLevel = (CPSS_IPV6_PREFIX_SCOPE_ENT)0;
  
	/* call cpss api function */
	pthread_mutex_lock(&semRtRwMutex);
    status =  cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen,
                                           &defUcNextHop, override);
	pthread_mutex_unlock(&semRtRwMutex);

	if(status == NAM_OK){
		gIndex++;
		status = ROUTE_RETURN_CODE_SUCCESS;
	}
	else {
		status = ROUTE_RETURN_CODE_SET_ERROR;
	}
#endif
#ifdef DRV_LIB_BCM
         /*for broadcom, host with ip mask 32 is added as next hop in the process arp learning,so here do nothing*/
    status = 0; 
#endif
    return status;
}

unsigned long  del_LPM_TBL_Entry(unsigned int DIP,unsigned int masklen)
{
	
	unsigned long 	status = 0;
    int          ret = 0;
	GT_IPADDR		ipAddr;
#ifdef DRV_LIB_CPSS_3_4
			memcpy(ipAddr.arIP,&DIP,sizeof(unsigned int));
#else
			ipAddr = DIP;	
#endif
	if(!DIP && !masklen){/*if default route,override to init value*/
	   ret = nam_route_override_default_route(DIP);
	   if(0 != ret){
		  return ret;
	   }
	}
	else {
#ifdef DRV_LIB_CPSS
		pthread_mutex_lock(&semRtRwMutex);
		status = cpssDxChIpLpmIpv4UcPrefixDel(0,0,ipAddr,masklen);
		pthread_mutex_unlock(&semRtRwMutex);
		if(status == 0){
			gIndex--;
			status = ROUTE_RETURN_CODE_SUCCESS;
		}
		else
		{
			 nam_syslog_err("delete lpm entry error\n");	
			 status = ROUTE_RETURN_CODE_SET_ERROR;
		}
		return status;
#endif
#ifdef DRV_LIB_BCM
     return 0;
#endif
	}
    return ret;     /* code optimize: Missing return statement. zhangdi@autelan.com 2013-01-18 */	
}

int nam_set_route_info
(
    unsigned int dip,
	unsigned int nexthop,
	unsigned int masklen,
	unsigned int ifindex,
	unsigned int routeEntryBaseIndex
)
{
	int ret = 0;
	
#ifdef DRV_LIB_CPSS   
	unsigned int lpmDBId, vrId,status,prefixLen;
	GT_IPADDR								 ipAddr;
	CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT	 defUcNextHop;
	GT_BOOL 								   override = 1;
	char*p;
#endif

#ifdef DRV_LIB_BCM
	int unit = 0;
	unsigned int num_of_asic = 0;
	bcm_l3_route_t	 info;
	bcm_l3_host_t  next_hop_info,hostinfo;
	
	memset(&info,0,sizeof(bcm_l3_route_t));
	memset(&next_hop_info,0,sizeof(bcm_l3_host_t));	
	memset(&hostinfo,0,sizeof(bcm_l3_host_t));
#endif

#ifdef DRV_LIB_CPSS
    	lpmDBId = 0; /*LPM表ID 应该在初始化时指定该值目前用0*/
    	vrId = 0;     /*在代码中看到该值定义了，但未实现，设为0即可*/
#ifdef DRV_LIB_CPSS_3_4
			memcpy(ipAddr.arIP,&dip,sizeof(unsigned int));
#else
			ipAddr = dip;	
#endif

    	prefixLen = masklen;
		memset(&defUcNextHop, 0, sizeof(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT));
    	defUcNextHop.ipLttEntry.routeType = CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    	defUcNextHop.ipLttEntry.numOfPaths = 0;
    	defUcNextHop.ipLttEntry.routeEntryBaseIndex = (unsigned int)routeEntryBaseIndex;
    	defUcNextHop.ipLttEntry.ucRPFCheckEnable = ucRPFCheckEnable;
    	defUcNextHop.ipLttEntry.sipSaCheckMismatchEnable = 0;
    	defUcNextHop.ipLttEntry.ipv6MCGroupScopeLevel = 0;
    
    
    	p = (char*) &defUcNextHop;

    	/* call cpss api function */
		pthread_mutex_lock(&semRtRwMutex);
    	status =  cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen,
                                           &defUcNextHop, override);
		pthread_mutex_unlock(&semRtRwMutex);

		if(status == 0)
			gIndex++;
		else
			 nam_syslog_err("insert lpm entry error for ret %d\n",status);

    	return status;

#endif

#ifdef DRV_LIB_BCM
	/*Get next hop info*/
    num_of_asic = nam_asic_get_instance_num();  
	for(unit = 0;unit < num_of_asic;unit++){		        
		next_hop_info.l3a_ip_addr = nexthop;
		ret = bcm_l3_host_find(unit,&next_hop_info);
		if((NAM_E_NONE != ret)||(next_hop_info.l3a_flags & BCM_L3_DST_DISCARD)){
           nam_syslog_err("get next hop index %d info failed for ret %d on unit %d\n",routeEntryBaseIndex,ret,unit);
           return ret;
		}
		else{
		   if(32 == masklen){/*add to host table*/
              memcpy(&hostinfo,&next_hop_info,sizeof(bcm_l3_host_t));
			  hostinfo.l3a_ip_addr = dip;
			  hostinfo.l3a_flags |= BCM_L3_WITH_ID|BCM_L3_REPLACE;
			  ret = bcm_l3_host_add(unit,&hostinfo);
			  if(0 != ret){
                 nam_syslog_err("Add dip %#x masklen 32 to host table failed for ret %d\n",dip,ret);
				 return ret;
			  }
			  memset(&hostinfo,0,sizeof(bcm_l3_host_t));
		   }
		   else{
			   nam_syslog_dbg("dip %#x masklen %d next hop ip %#x exist,get nh index %d\n",dip,masklen,nexthop,next_hop_info.l3a_nexthop_hw_idx);
			   info.l3a_subnet = dip;
			   get_mask_by_ip_masklen(dip,masklen,&info.l3a_ip_mask);
			   info.l3a_nexthop_ip = nexthop;
			   info.l3a_intf = next_hop_info.l3a_intf;
			   info.l3a_nexthop_hw_idx = next_hop_info.l3a_nexthop_hw_idx;
			   info.l3a_flags = NAM_L3_REPLACE | NAM_L3_WITH_ID;
			   memcpy(info.l3a_nexthop_mac, next_hop_info.l3a_nexthop_mac,6);
			   if(next_hop_info.l3a_flags& NAM_L3_TGID){
				  info.l3a_flags |= NAM_L3_TGID;
				  info.l3a_port_tgid = next_hop_info.l3a_port_tgid;
			   }
			   else{
				  info.l3a_modid = next_hop_info.l3a_modid;
				  info.l3a_port_tgid = next_hop_info.l3a_port_tgid;
			   }
			   ret = bcm_l3_route_add(unit,&info);
			   if(NAM_E_NONE != ret){
				  nam_syslog_err("Failed to add LPM to driver %d for ret %d\n",unit,ret);
				  return ret;
			   }
			   else{
				  ret = bcm_l3_route_get(unit,&info);
				  if(0 != ret){
					 nam_syslog_err("Failed to get route info,ret %d\n",ret);
					 return ret;
				  }
				  else{
					 print_lpm_info(unit,info.l3a_subnet,masklen);
				  }
			   }
			   memset(&info,0,sizeof(bcm_l3_route_t));
		   }
		   memset(&next_hop_info,0,sizeof(bcm_l3_host_t));
		}
	}
	if(NAM_E_NONE != ret){
	    gIndex++;
	}
	return ret;
#endif
   return NAM_E_NONE;

}

GT_STATUS lpm_entry_search(unsigned int DIP,unsigned int masklen,unsigned int* entryIndex)
{
    unsigned int                                   lpmDBId=0;
    unsigned int                                   vrId=0;
    unsigned int                                   lpm_index=0;
    GT_IPADDR                                ipAddr;
    unsigned int                                   prefixLen = masklen;
    unsigned char status;
	CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT defUcNextHop;
#ifdef DRV_LIB_CPSS_3_4
	memcpy(ipAddr.arIP,&DIP,sizeof(unsigned int));
#else
    ipAddr = DIP;	
#endif
	memset(&defUcNextHop, 0, sizeof(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT));
#ifdef DRV_LIB_CPSS
    status =  cpssDxChIpLpmIpv4UcPrefixSearch_tmp(lpmDBId, vrId, ipAddr, prefixLen,&lpm_index,
                                              &defUcNextHop);

    if(status)
    {
    	 nam_syslog_dbg("can't find the entry %d.%d.%d.%d/%d\n", 
		 				(DIP>>24)&0xFF, (DIP>>16)&0xFF, (DIP>>8)&0xFF, DIP&0xFF, masklen);
		 return status;
    }	

	nam_syslog_dbg("LPM search entry %d.%d.%d.%d/%d index %d route base %d\n", \
						(DIP>>24)&0xFF, (DIP>>16)&0xFF, (DIP>>8)&0xFF, DIP&0xFF,  \
						masklen,lpm_index,defUcNextHop.ipLttEntry.routeEntryBaseIndex);
    *entryIndex = defUcNextHop.ipLttEntry.routeEntryBaseIndex;
	return status;
 #endif
#ifdef DRV_LIB_BCM
   int ret = 0;
   int unit = 0,num_of_asic = 0;
   int nh_idx = 0;
   bcm_l3_host_t htinfo;
   bcm_l3_route_t rtinfo;
   
   memset(&htinfo,0,sizeof(bcm_l3_host_t));
   memset(&rtinfo,0,sizeof(bcm_l3_route_t));
   num_of_asic = nam_asic_get_instance_num();
   for(unit = 0;unit < num_of_asic;unit++){
	   /*first search the host table*/
	   if(32 == masklen){
		   htinfo.l3a_ip_addr = DIP;
		   ret = bcm_l3_host_find(unit,&htinfo);
	       if(NAM_E_NONE != ret){
	           nam_syslog_err("find host drvroute error on unit %d for %d\n",unit,ret);
			   continue;
		   }
		   *entryIndex = htinfo.l3a_nexthop_hw_idx;
	   }
	   else{
	   	   memset(&rtinfo,0,sizeof(bcm_l3_route_t));
	   	   rtinfo.l3a_subnet = DIP;
		   get_mask_by_ip_masklen(DIP,masklen,&(rtinfo.l3a_ip_mask));
	       ret = bcm_l3_route_get(unit,&rtinfo);
		   if(NAM_E_NONE != ret){
	           nam_syslog_err("find fib drvroute error on unit %d for %d\n",unit,ret);
			   continue;
		   }
		   else{
             print_lpm_info(unit,DIP,masklen);
			 if(0 == unit){
			    *entryIndex = rtinfo.l3a_nexthop_hw_idx;
			 }
			 else if(*entryIndex != rtinfo.l3a_nexthop_hw_idx){
			 	nam_syslog_err("dip %#x,masklen %d nh index different,unit 0 is %d but unit 1 is %d\n",DIP,masklen,*entryIndex,rtinfo.l3a_nexthop_hw_idx);
                return NAM_E_NOT_FOUND;
			 }
			 else{
                nam_syslog_dbg(" nh index %d\n",*entryIndex);
			 }
		   }
	   }
   }
   return ret;
#endif

}

int nam_get_drv_route_info
(
    unsigned int DIP,
    unsigned int masklen,
    struct arp_snooping_item_s* arpInfo
)
{
   int ret = 0;	
#ifdef DRV_LIB_BCM
   int unit = 0,num_of_asic = 0;
   bcm_l3_host_t htinfo;
   bcm_l3_route_t rtinfo;

   memset(&htinfo,0,sizeof(bcm_l3_host_t));
   memset(&rtinfo,0,sizeof(bcm_l3_route_t));
   num_of_asic = nam_asic_get_instance_num();
   
   /*the route info in the two asic should be same with each other,so read two but record one!!*/
   for(unit = 0;unit < num_of_asic;unit++){
	   /*first search the host table*/
	   if(32 == masklen){
		   htinfo.l3a_ip_addr = DIP;
		   ret = bcm_l3_host_find(unit,&htinfo);
	       if(NAM_E_NONE != ret){
	           nam_syslog_err("find host drvroute error on unit %d for %d\n",unit,ret);
			   continue;
		   }
	   }
	   else{
	   	   rtinfo.l3a_subnet = DIP;
		   get_mask_by_ip_masklen(DIP,masklen,&(rtinfo.l3a_ip_mask));
	       ret = bcm_l3_route_get(unit,&rtinfo);
		   if(NAM_E_NONE != ret){
	           nam_syslog_err("find fib drvroute error on unit %d for %d\n",unit,ret);
			   continue;
		   }
		   else{
			  print_lpm_info(unit,rtinfo.l3a_subnet,masklen);
			  if(0 == unit)
			     memset(&rtinfo,0,sizeof(rtinfo));
		   }
	   }
   }
   if(0 != ret){
      return ret;
   }
   if(32 == masklen){
      arpInfo->ifIndex = htinfo.l3a_intf;
	  arpInfo->ipAddr = htinfo.l3a_ip_addr;
	  memcpy(arpInfo->mac,htinfo.l3a_nexthop_mac,6);
	  if(htinfo.l3a_flags & NAM_L3_TGID){
         arpInfo->dstIntf.isTrunk = 1;
		 arpInfo->dstIntf.intf.trunk.trunkId = htinfo.l3a_port_tgid;
		 arpInfo->dstIntf.intf.trunk.devNum = htinfo.l3a_modid;
	  }
	  else{
		  arpInfo->dstIntf.isTrunk = 0;
		  arpInfo->dstIntf.intf.port.portNum = htinfo.l3a_port_tgid;
		  arpInfo->dstIntf.intf.port.devNum = htinfo.l3a_modid;

	  }
   }
   else{
      arpInfo->ifIndex = rtinfo.l3a_intf;
	  arpInfo->ipAddr = rtinfo.l3a_subnet;
	  arpInfo->vid = rtinfo.l3a_vid;
	  memcpy(arpInfo->mac,rtinfo.l3a_nexthop_mac,6);
	  if(rtinfo.l3a_flags & NAM_L3_TGID){
         arpInfo->dstIntf.isTrunk = 1;
		 arpInfo->dstIntf.intf.trunk.trunkId = rtinfo.l3a_port_tgid;
		 arpInfo->dstIntf.intf.trunk.devNum = rtinfo.l3a_modid;
	  }
	  else{
		  arpInfo->dstIntf.isTrunk = 0;
		  arpInfo->dstIntf.intf.port.portNum = rtinfo.l3a_port_tgid;
		  arpInfo->dstIntf.intf.port.devNum = rtinfo.l3a_modid;
	  }  	
   }
   return ret;
#else
   return ret;
#endif
}
int nam_del_route_info
(
    unsigned int dip,
	unsigned int nexthop,
	unsigned int masklen,
	unsigned int ifindex,
    unsigned int rt_type
)
{
    unsigned int status = 0,refCnt = 0;
	int ret = 0;
    unsigned int entryIndex = 0;
	struct arp_snooping_item_s arpInfo = {0};
	memset(&arpInfo,0,sizeof(struct arp_snooping_item_s));
#ifdef DRV_LIB_BCM
    unsigned int tblIndex = 0;
    int unit = 0,num_of_asic = 0;
    bcm_l3_route_t  rtinfo;
	bcm_l3_host_t htinfo;
	struct route_nexthop_brief_s *data = NULL;

	memset(&rtinfo,0,sizeof(bcm_l3_route_t));
	memset(&htinfo,0,sizeof(bcm_l3_host_t));
#endif
	status = lpm_entry_search(dip,masklen,&entryIndex);
	if(status)
	{
		nam_syslog_dbg("lpm_entry_search error %d\n",status);
		return status;
	}

	if(entryIndex>2){
		pthread_mutex_lock(&nexthopHashMutex);
		ret = npd_route_nexthop_iteminfo_get(0,entryIndex, &arpInfo,&refCnt);		
		pthread_mutex_unlock(&nexthopHashMutex);
		if(ROUTE_RETURN_CODE_ACTION_TRAP2CPU == ret
			||ROUTE_RETURN_CODE_ACTION_HARD_DROP == ret
			|| 0 == ret){
			if(arpInfo.ifIndex != ifindex )
			{
				nam_syslog_dbg("RTM_DELROUTE error: Delete rt entry isn't same to exist\n");
				return 1;
			}
		}
		else{
			nam_syslog_dbg("RTM_DELROUTE error: Can't get HW arp info\n");
			return 1;
		}
		status = npd_route_nexthop_tblindex_release(ifindex,ntohl(nexthop),entryIndex);
		if((status!=0)&&(ROUTE_RETURN_CODE_NOTCONSISTENT != status)) {
			nam_syslog_dbg("del ARP TABLE error %d\n",status);
			return status;
		}
	}
	
#ifdef DRV_LIB_CPSS	
	else if((entryIndex == 0)&&(rt_type != RTN_BLACKHOLE)){
		return 1;
	}
	else if(entryIndex == 2){
		if(rt_type != RTN_UNREACHABLE && nexthop != 0){
			return 1;
		}
	}
	status = del_LPM_TBL_Entry(dip,masklen);
	if(status)
	{
		 nam_syslog_dbg("del LPM entry error %d\n",status);
		return status;
	}
	return status;
#endif

#ifdef DRV_LIB_BCM
	num_of_asic = nam_asic_get_instance_num();
	for(unit =0;unit < num_of_asic;unit++){
		if(32 == masklen){/*delete from host table*/
            htinfo.l3a_ip_addr = dip;
			ret = bcm_l3_host_delete(unit,&htinfo);
			if(0 != ret){
               nam_syslog_err("failed to delete ip %#x from host for ret %d\n",htinfo.l3a_ip_addr,ret);
			   return ret;
			}
	        memset(&htinfo,0,sizeof(bcm_l3_host_t));
		}
		else {
			memset(&rtinfo,0,sizeof(bcm_l3_route_t));
			rtinfo.l3a_subnet = dip;
			get_mask_by_ip_masklen(dip,masklen,&rtinfo.l3a_ip_mask);
			nam_syslog_dbg("Delete info:");
			print_lpm_info(unit,rtinfo.l3a_subnet,masklen);
			ret = bcm_l3_route_delete(unit,&rtinfo);
			if(0 != ret){
				 nam_syslog_dbg("Fail to delete route entry dip %d masklen %d on unit %d for ret %d\n",dip,masklen,unit,ret);
				 return ret;
			}
			else{
			  nam_syslog_dbg("Delete lpm on unit %d success!\n",unit);
			}
		}
	}
	return ret;
#endif

}


unsigned int  set_LPM_TBL_Entry
(
	unsigned int DIP,
	unsigned int nexthop,
	unsigned int masklen,
	unsigned int ifindex,
	unsigned int rt_type
) 
{
	unsigned int    status;
	unsigned int routeEntryBaseIndex = 0;
	int num_of_asic = 0,unit = 0;
	int ret = 0;
#ifdef DRV_LIB_BCM
    bcm_l3_route_t info;
#endif
	/*blackhole route need drop directly*/
	if(RTN_BLACKHOLE == rt_type) {
		nam_syslog_dbg("set LPM table entry type:%s\n","RTN_BLACKHOLE");
		
#ifdef DRV_LIB_CPSS
		routeEntryBaseIndex = 0; /* next-hop entry 0 action: hard-drop*/
#endif

#ifdef DRV_LIB_BCM
		num_of_asic = nam_asic_get_instance_num();
		for(unit = 0;unit < num_of_asic;unit++){
			memset(&info,0,sizeof(bcm_l3_route_t));   
			info.l3a_flags |=  NAM_L3_REPLACE | NAM_L3_DST_DISCARD;
			info.l3a_subnet = DIP;
			get_mask_by_ip_masklen(DIP,masklen,&info.l3a_ip_mask);
			nam_syslog_dbg("mask %#x from dip %#x, masklen %d\n",info.l3a_ip_mask,DIP,masklen);
			info.l3a_nexthop_ip = nexthop;
			info.l3a_intf = ifindex;
			info.l3a_modid = unit;
			ret = bcm_l3_route_add(unit,&info);
			if(NAM_E_NONE != ret){
				nam_syslog_dbg("add TO CPU route failed for ret %d on unit %d\n",ret,unit);
				return -1;
			}
		}
		nam_syslog_dbg("add TO CPU route success!\n");
	    return 0;
#endif
	}

	else if((RTN_UNREACHABLE == rt_type)||(0 == nexthop)) {
		nam_syslog_dbg("set LPM table entry type:%s\n",(RTN_UNREACHABLE == rt_type) ? "RTN_UNREACHABLE":"Connect");
#ifdef DRV_LIB_CPSS
		routeEntryBaseIndex = 2; /* next-hop entry 2 action: trap_to_CPU*/
#endif
#ifdef DRV_LIB_BCM
		num_of_asic = nam_asic_get_instance_num();
		for(unit = 0;unit < num_of_asic;unit++){
			memset(&info,0,sizeof(bcm_l3_route_t));	  
			info.l3a_flags = NAM_L3_DEFIP_LOCAL | NAM_L3_REPLACE;
			info.l3a_subnet = DIP;
			get_mask_by_ip_masklen(DIP,masklen,&info.l3a_ip_mask);
			nam_syslog_dbg("mask %#x from dip %#x, masklen %d\n",info.l3a_ip_mask,DIP,masklen);
			info.l3a_nexthop_ip = nexthop;
			info.l3a_intf = ifindex;
			info.l3a_modid = unit;
			ret = bcm_l3_route_add(unit,&info);
			if(NAM_E_NONE != ret){
				nam_syslog_dbg("add TO CPU route failed for ret %d on unit %d\n",ret,unit);
				return -1;
		   	}
			print_lpm_info(unit,DIP,masklen);
		}
		nam_syslog_dbg("add TO CPU route success!\n");
		return 0;
#endif
    }
	else {
		routeEntryBaseIndex = get_NextHop_TBL_Entry(ifindex,ntohl(nexthop));
		
#ifdef DRV_LIB_CPSS
		nam_syslog_dbg("get routeindex %d\n",routeEntryBaseIndex);
#endif
	}
	
	if(routeEntryBaseIndex == 0)    /* code optimize: it can not be < 0. zhangdi@autelan.com 2013-01-18 */
	{
	    nam_syslog_err("ERROR can't get neighbour table enter index\n");
		return -1;
	}
    ret = nam_set_route_info(DIP,nexthop,masklen,ifindex,routeEntryBaseIndex);
    return ret;
}
#ifdef DRV_LIB_BCM
int l3_route_traverse_cb
(
    int unit, 
    int index, 
    bcm_l3_route_t *rtinfo, 
    void *user_data
)
{
    int flag = 0;
    unsigned int i = 0,j = 0;
    char ip_str[IP6ADDR_STR_LEN + 3];
    char if_mac_str[MACADDR_STR_LEN];
    char vrf_str[20];
    char *hit;
    char *trunk;
    int masklen;
    int priority;
	rt_datas* drvinfo = (rt_datas*)user_data;
	
    if((NULL == rtinfo)||(NULL == user_data)){
       return 0;
	}
	if(drvinfo->count < 4096){
	    drvinfo->info[index].dip = rtinfo->l3a_subnet;
		drvinfo->info[index].masklen = rtinfo->l3a_ip_mask;
		drvinfo->info[index].nexthop = rtinfo->l3a_nexthop_ip;
		drvinfo->info[index].baseindex = rtinfo->l3a_nexthop_hw_idx;
		drvinfo->info[index].ifindex = rtinfo->l3a_intf;
		drvinfo->info[index].dev = rtinfo->l3a_modid;
		drvinfo->info[index].port = rtinfo->l3a_port_tgid;
		drvinfo->count++;
	}
#if 1 
    hit = (rtinfo->l3a_flags & NAM_L3_HIT) ? "y" : "n";
    trunk = (rtinfo->l3a_flags & NAM_L3_TGID) ? "t" : " ";
    priority = rtinfo->l3a_pri;

    switch (rtinfo->l3a_vrf){
      case NAM_L3_VRF_GLOBAL:
          strcpy(vrf_str,"Global"); 
          break;
      case NAM_L3_VRF_OVERRIDE:
          strcpy(vrf_str,"Override"); 
          break;
      default:
          sprintf(vrf_str,"%d",rtinfo->l3a_vrf);
    }

    format_macaddr(if_mac_str, rtinfo->l3a_nexthop_mac);

    if (rtinfo->l3a_flags & NAM_L3_IP6) {
        format_ip6addr(ip_str, rtinfo->l3a_ip6_net);
        masklen = nam_ip6_mask_length(rtinfo->l3a_ip6_mask);
        nam_syslog_dbg("%-5d %-8s %-s/%d %18s %4d %4d %5d%1s %4d %4d %s", index, vrf_str,
               ip_str, masklen, if_mac_str, rtinfo->l3a_intf,
               rtinfo->l3a_modid, rtinfo->l3a_port_tgid, trunk, priority, 
               rtinfo->l3a_lookup_class, hit);
    } else {
        format_ipaddr_mask(ip_str, rtinfo->l3a_subnet, rtinfo->l3a_ip_mask);
        nam_syslog_dbg("%-5d %-8s %-19s %18s %4d %4d %5d%1s %4d %4d %s", index, vrf_str,
               ip_str, if_mac_str, rtinfo->l3a_intf,
               rtinfo->l3a_modid, rtinfo->l3a_port_tgid, trunk, priority, 
               rtinfo->l3a_lookup_class, hit);
    }

    if (rtinfo->l3a_flags & NAM_L3_MULTIPATH) {
        nam_syslog_dbg("      (ECMP)\n");
    } else if (rtinfo->l3a_flags & NAM_L3_DEFIP_LOCAL) {
        nam_syslog_dbg("      (LOCAL ROUTE)\n");
    } 

    if (rtinfo->l3a_flags & NAM_L3_ROUTE_LABEL) {
        nam_syslog_dbg(" (MPLS)%d\n", rtinfo->l3a_mpls_label);
    } else if (rtinfo->l3a_tunnel_option) {
        nam_syslog_dbg(" (UDP tunnel) %d\n", rtinfo->l3a_tunnel_option);
    } else { 
       nam_syslog_dbg("\n");
    }
#endif
    return i;
}

int l3_host_traverse_cb
(
    int unit, 
    int index, 
    bcm_l3_host_t *htinfo, 
    void *user_data
)
{
    int flag = 0;
    unsigned int i = 0,j = 0;
    char ip_str[IP6ADDR_STR_LEN + 3];
    char if_mac_str[MACADDR_STR_LEN];
    int masklen = 0;
	host_datas* drvinfo = (host_datas*)user_data;

	memset(ip_str,0,IP6ADDR_STR_LEN + 3);
	memset(if_mac_str,0,MACADDR_STR_LEN);
	
    if((NULL == htinfo)||(NULL == drvinfo)){
       return 0;
	}
	if(index < 4096){	
		drvinfo->info[index].ip = htinfo->l3a_ip_addr;
	    drvinfo->info[index].ifindex = htinfo->l3a_intf;
		drvinfo->info[index].dev = htinfo->l3a_modid;
		drvinfo->info[index].baseindex = htinfo->l3a_nexthop_hw_idx;
		drvinfo->info[index].port = htinfo->l3a_port_tgid;
		memcpy(drvinfo->info[index].mac,htinfo->l3a_nexthop_mac,6);
		drvinfo->count++;
	}

   format_macaddr(if_mac_str, htinfo->l3a_nexthop_mac);
   format_ipaddr_mask(ip_str,htinfo->l3a_ip_addr,0xffffffff);
   nam_syslog_dbg("%-5d %-19s %18s %4d %4d %5d% %4d", index,
           ip_str, if_mac_str, htinfo->l3a_intf,
           htinfo->l3a_modid, htinfo->l3a_port_tgid, 
           htinfo->l3a_lookup_class);
   return 0;
}


#endif
unsigned long  lpm_entry_get_next(unsigned int *Dip,unsigned int *PrefixLen,unsigned int* baseindex)
{
   unsigned long  result;
#ifdef DRV_LIB_CPSS
    GT_U32                                    lpmDBId=0;
    GT_U32                                    vrId=0;
	GT_IPADDR								  ipAddr;
	CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT defUcNextHop;
#ifdef DRV_LIB_CPSS_3_4
			memcpy(ipAddr.arIP,Dip,sizeof(unsigned int));
#else
			ipAddr = *Dip;	
#endif
#endif

#ifdef DRV_LIB_BCM
   int ret = 0;
   int unit = 0,num_of_asic = 0;
   bcm_l3_info_t      info;
   bcm_l3_route_t     rtInfo;
   
   memset(&rtInfo,0,sizeof(bcm_l3_route_t));
   memset(&info,0,sizeof(bcm_l3_info_t));
#endif
#ifdef DRV_LIB_CPSS	
    result = cpssDxChIpLpmIpv4UcPrefixGetNext(lpmDBId, vrId, &ipAddr,
                                              PrefixLen, &defUcNextHop);
	if(result)
		 nam_syslog_err("error: lpm_entry_get_next-- can't get next entey\n");
#ifdef DRV_LIB_CPSS_3_4
			memcpy(Dip,ipAddr.arIP,sizeof(unsigned int));
#else
			*Dip = ipAddr;	
#endif
	*baseindex = defUcNextHop.ipLttEntry.routeEntryBaseIndex;
	return result;
#endif

#ifdef DRV_LIB_BCM
    unsigned int start = 0,end = 0,used = 0,i = 0;
    num_of_asic = nam_asic_get_instance_num();
    for(unit = 0;unit < num_of_asic;unit++){
	bcm_l3_info(unit,&info);
	end = info.l3info_max_route;
	used = info.l3info_used_route;
	rtInfo.l3a_subnet = Dip;
	rtInfo.l3a_ip_mask = PrefixLen;/*here should convert to mask*/
	ret = bcm_l3_route_traverse(unit,0,start,end,l3_route_traverse_cb,&rtInfo);
    }
	return ret;
#endif
}

int nam_route_get_info
(
    drv_infos* info
)
{
#ifdef DRV_LIB_CPSS
		GT_U32									  lpmDBId=0;
		GT_U32									  vrId=0;
		GT_IPADDR  Dip;
		unsigned int masklen=32;
        unsigned int baseindex = 0,i = 0;
		unsigned long  result;
		CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT defUcNextHop;
#endif
	
#ifdef DRV_LIB_BCM
	   
	   int ret = 0,i = 0,j = 0;
	   int unit = 0,num_of_asic = 0,free_defip,free_lpm_blk;
	   bcm_l3_info_t   l3_hw_status;
	   unsigned int start = 0,end = 0;
	   rt_datas rtdata;
	   memset(&l3_hw_status,0,sizeof(bcm_l3_info_t));
	   memset(&rtdata,0,sizeof(rt_datas));
#endif
#ifdef DRV_LIB_CPSS	
		memset(&Dip,0xff,sizeof(GT_IPADDR));
		while(GT_OK == (result = cpssDxChIpLpmIpv4UcPrefixGetNext(lpmDBId, vrId, &Dip,
												  masklen, &defUcNextHop))){
#ifdef DRV_LIB_CPSS_3_4
		  memcpy(&(info[i].dip),&(Dip.arIP),sizeof(unsigned int));
#else
          info[i].dip = Dip;
#endif
		  info[i].masklen = masklen;
		  info[i].baseindex = defUcNextHop.ipLttEntry.routeEntryBaseIndex;
          if(i < gIndex)
		     i++;
		  else{
             nam_syslog_err("route count filled!\n");
			 break;
		  }
		}
		return i;
#endif
#ifdef DRV_LIB_BCM  
   if((ret = bcm_l3_info(unit, &l3_hw_status)) < 0) {
	   nam_syslog_err("%s: Error L3 accessing: %d\n","GET ROUTE", ret);
	   return ret;
   }
   end = l3_hw_status.l3info_max_route;
   num_of_asic = nam_asic_get_instance_num();
   for(unit = 0;unit < num_of_asic;unit++){
	  ret = bcm_l3_route_traverse(unit,0,start,end,l3_route_traverse_cb,&rtdata);
      for(i = 0;i<rtdata.count;i++){
         if(unit != rtdata.info[i].dev){
            continue;
		 }
		 info[j].dip = rtdata.info[i].dip;
		 info[j].masklen= rtdata.info[i].masklen;
		 info[j].nexthop = rtdata.info[i].nexthop;
		 info[j].baseindex = rtdata.info[i].baseindex;
		 info[j].ifindex = rtdata.info[i].ifindex;
		 info[j].dev = rtdata.info[i].dev;
		 info[j].port = rtdata.info[i].port;
		 j++;
	  }
	  memset(&rtdata,0,sizeof(rt_datas));
   }
   return j;
#endif
}

int nam_host_get_info
(
    host_infos* info
)
{

#ifdef DRV_LIB_BCM
	   
	   int ret = 0,i = 0,j = 0;
	   int unit = 0,num_of_asic = 0,free_defip,free_lpm_blk;
	   bcm_l3_info_t   l3_hw_status;
	   unsigned int start = 0,end = 0;
	   host_datas    htdata;
	   
	   memset(&l3_hw_status,0,sizeof(bcm_l3_info_t));
	   memset(&htdata,0,sizeof(host_datas));
#endif
#ifdef DRV_LIB_CPSS	
		return 0;
#endif
#ifdef DRV_LIB_BCM  
   if((ret = bcm_l3_info(unit, &l3_hw_status)) < 0) {
	   nam_syslog_err("%s: Error L3 accessing: %d\n","GET ROUTE", ret);
	   return ret;
   }
   end = l3_hw_status.l3info_max_host;
   num_of_asic = nam_asic_get_instance_num();
   for(unit = 0;unit < num_of_asic;unit++){
	  ret = bcm_l3_host_traverse(unit,0,start,end,l3_host_traverse_cb,&htdata);
	  nam_syslog_dbg("host data count on unit %d is %d\n",unit,htdata.count);
      for(i = 0;i<htdata.count;i++){
	  	  if(unit != htdata.info[i].dev){
             continue;
		  }	  
	  	  info[j].baseindex = htdata.info[i].baseindex;
		  info[j].dev = htdata.info[i].dev;
		  info[j].ifindex = htdata.info[i].ifindex;
		  info[j].ip = htdata.info[i].ip;
		  memcpy(info[j].mac,htdata.info[i].mac,6);
		  info[j].port = htdata.info[i].port;
		  j++;
		  if(j>=4096){
            return j;
		  }
      }
	  memset(&htdata,0,sizeof(host_datas));
   }
   return j;
#endif
}

int nam_route_ipCnt_get
(
   unsigned char devNum,
   CPSS_IP_CNT_SET_ENT cntSet,
   CPSS_DXCH_IP_COUNTER_SET_STC*  counters
)
{
   unsigned long  result = 0;

#ifdef DRV_LIB_CPSS
   CPSS_DXCH_IP_COUNTER_SET_STC  val;

   memset(&val,0,sizeof(CPSS_DXCH_IP_COUNTER_SET_STC));
   result = cpssDxChIpCntGet(devNum, cntSet,&val);
   if(0!= result){
   	   nam_syslog_err("Ipcnt get failed for result %d\n",result);
	   return 1;
   }
   memcpy(counters,&val,sizeof(CPSS_DXCH_IP_COUNTER_SET_STC));
#endif
   return 0;
}


int nam_route_set_ucrpf_enable
(
    unsigned int rfpen
)
{
#ifdef DRV_LIB_CPSS
    ucRPFCheckEnable = rfpen;
	return ucRPFCheckEnable;
#endif
}

void nam_route_get_ucrpf
(
    unsigned char* ucrpf
)
{
#ifdef DRV_LIB_CPSS
    *ucrpf = ucRPFCheckEnable;
#endif

}
unsigned int     nam_ipCnt_mode_set
(
     unsigned char                              devNum,
     CPSS_IP_CNT_SET_ENT                        cntSet,
     CPSS_DXCH_IP_CNT_SET_MODE_ENT              cntSetMode,
     CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC* interfaceModeCfgPtr

)
{
    unsigned int ret = 0; 
#ifdef DRV_LIB_CPSS
	if(0 != (ret = cpssDxChIpCntSetModeSet(devNum,cntSet,cntSetMode,interfaceModeCfgPtr)))
	{
		nam_syslog_dbg("cpssDxChIpCntSetModeSet failure\n");
	}
#endif
	return ret;
}
void  nam_route_count_get
(
    unsigned int *route_count
)
{
   *route_count = gIndex; 
}
#ifdef DRV_LIB_CPSS
unsigned long lpm_entry_get_first(unsigned int *Dip,unsigned int *PrefixLen,CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT* defUcNextHop)
{
    unsigned long result;

    unsigned int	lpmDBId=0;
    unsigned int	vrId=0;
	GT_IPADDR		ipAddr;
	
    *Dip = 0; /*for first entry 0 is entered*/
    *PrefixLen = 0; /*for first entry 0 is entered*/
#ifdef DRV_LIB_CPSS_3_4
			memcpy(ipAddr.arIP,Dip,sizeof(unsigned int));
#else
			ipAddr = *Dip;	
#endif
    result = cpssDxChIpLpmIpv4UcPrefixGetNext(lpmDBId, vrId, &ipAddr,
                                              PrefixLen, defUcNextHop);
	if(result)
		 nam_syslog_dbg("error: lpm_entry_get_first-- can't get first entey\n");
#ifdef DRV_LIB_CPSS_3_4
			memcpy(Dip,ipAddr.arIP,sizeof(unsigned int));
#else
			*Dip = ipAddr;	
#endif
	return result;
}


void show_rtdrv()
{

	int index = 0;
	CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT defUcNextHop;
#if 0
	static _GT_IPADDR Dip={0},masklen={0};
	static unsigned int mc=0;
#else
	static GT_IPADDR SDip;
	static unsigned int Smasklen=0;
#endif

	while(GT_NOT_FOUND != cpssDxChIpLpmIpv4UcPrefixGetNext(0,0,&SDip,&Smasklen,&defUcNextHop))
	{
		index ++;
		 nam_syslog_dbg("index = %d\n",index);
#ifdef DRV_LIB_CPSS_3_4
		 nam_syslog_dbg("Dip = 0X%X\n",SDip.u32Ip);
#else
		 nam_syslog_dbg("Dip = 0X%X\n",SDip);
#endif
		 nam_syslog_dbg("masklen = %d\n",Smasklen);
		 nam_syslog_dbg("nexthop index = %d\n",defUcNextHop.ipLttEntry.routeEntryBaseIndex);
		 nam_syslog_dbg("\n");

	}
	 nam_syslog_dbg("there is totle %d entry\n",index);

}

#endif
#ifdef __cplusplus
}
#endif

