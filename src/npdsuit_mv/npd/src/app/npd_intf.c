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
* npd_intf.c
*
*
* CREATOR:
*       qinhs@autelan.com
*
* DESCRIPTION:
*       APIs used in NPD for L3 interface process.
*
* DATE:
*       02/21/2008
*
*  FILE REVISION NUMBER:
*       $Revision: 1.152 $
*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/ioctl.h>
#include <linux/if.h>
#include <linux/if_arp.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/in.h>
#include <sys/time.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/wait.h>



#include "util/npd_list.h"
#include "sysdef/npd_sysdef.h"
#include "sysdef/returncode.h"
#include "npd/nbm/npd_bmapi.h"
#include "npd/nam/npd_amapi.h"
#include "nam/nam_eth_api.h"
#include "dbus/npd/npd_dbus_def.h"
#include "cvm/ethernet-ioctl.h"
#include "npd_product.h"
#include "board/board_define.h"

#include "npd_log.h"
#include "npd_arpsnooping.h"
#include "npd_vlan.h"
#include "npd_eth_port.h"
#include "npd_intf.h"
#include "npd_c_slot.h"
#include "npd_trunk.h"

char g_l3intf_name[16] = {'\0'};

unsigned int promis_port_add2_intf = TRUE;
unsigned short advanced_routing_default_vid = (NPD_PORT_L3INTF_VLAN_ID - 1);

/* MODULE_ID_AX7I_XG_CONNECT panle port to virtual port map array */
struct asic_port_info_s ax7i_xg_conn_port_mapArr[NPD_AX7i_alpha_MAX_SLOTNO][NPD_AX7i_alpha_MAX_INTCONN_XPORT] = {
	/* port 24 & 25 on 98Dx804*/
	{{0x1, 0x18},{0x1, 0x19}},
};
int is_master;
int slot_num;
int product_info;

extern int adptVirRxFd;

extern struct Hash *arp_snoop_db_s;
extern pthread_mutex_t arpHashMutex;

#define INTERFACE_CHANGE_NAME_CHECK_NAME(oldName,newName) if(!((!strncmp(oldName,"eth",3)||\
			(('e' == newName[0])&&(!strcmp(oldName+3,newName+1))))||\
			(!strncmp(newName,"eth",3)||\
			(('e' == oldName[0])&&(!strcmp(newName+3,oldName+1)))))){\
						syslog_ax_intf_err("interface name check failed! oldname %s newname %s\n",oldName,newName);\
						return COMMON_RETURN_CODE_BADPARAM;\
			}
/**********************************************************************************
 *  kap_create_intf
 *
 *  DESCRIPTION:
 *      this routine create kernel virtual dev
 *
 *  INPUT:
 *      pif : struct if_cfg_struct* --  param of createing dev
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      COMMON_RETURN_CODE_NULL_PTR   -  NULL POINT  error
 *      INTERFACE_RETURN_CODE_FD_ERROR  - fd is invalidate
 *      INTERFACE_RETURN_CODE_IOCTL_ERROR   -  ioctl FAILED
 *      INTERFACE_RETURN_CODE_SUCCESS  - create successed
 *
 **********************************************************************************/
static int kap_create_intf
(
    struct if_cfg_struct* pif
) {

    if (NULL == pif) {
        syslog_ax_intf_err("add l3 interface to kap error parameter\n");
        return COMMON_RETURN_CODE_NULL_PTR;
    }

    if (!adptVirRxFd || adptVirRxFd < 0) {
        syslog_ax_intf_err("add l3 interface to kap error fd %d\n", adptVirRxFd);
        return INTERFACE_RETURN_CODE_FD_ERROR;
    }
    else {
        if (0 > ioctl(adptVirRxFd, KAPADDIFF, pif)) {
            syslog_ax_intf_err("add l3 interface to kap ioctl fd %d cmd %#x error\n", \
                               adptVirRxFd, KAPADDIFF);
            return INTERFACE_RETURN_CODE_IOCTL_ERROR;
        }
        else {
            syslog_ax_intf_dbg("add l3 interface to kap success\n");
            return INTERFACE_RETURN_CODE_SUCCESS;
        }
    }
}

/**********************************************************************************
 *  kap_adv_disable
 *
 *  DESCRIPTION:
 *      this routine create kernel virtual dev
 *
 *  INPUT:
 *      pif : struct if_cfg_struct* --  param of createing dev
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      COMMON_RETURN_CODE_NULL_PTR   -  NULL POINT  error
 *      INTERFACE_RETURN_CODE_FD_ERROR  - fd is invalidate
 *      INTERFACE_RETURN_CODE_IOCTL_ERROR   -  ioctl FAILED
 *      INTERFACE_RETURN_CODE_SUCCESS  - create successed
 *
 **********************************************************************************/
static int kap_adv_disable
(
    struct if_cfg_struct* pif
) {

    if (NULL == pif) {
        syslog_ax_intf_err("add l3 interface to kap error parameter\n");
        return COMMON_RETURN_CODE_NULL_PTR;
    }

    if (!adptVirRxFd || adptVirRxFd < 0) {
        syslog_ax_intf_err("add l3 interface to kap error fd %d\n", adptVirRxFd);
        return INTERFACE_RETURN_CODE_FD_ERROR;
    }
    else {
        if (0 > ioctl(adptVirRxFd, KAPSETADVDIS, pif)) {
            syslog_ax_intf_err("add l3 interface to kap ioctl fd %d cmd %#x error\n", \
                               adptVirRxFd, KAPSETADVDIS);
            return INTERFACE_RETURN_CODE_IOCTL_ERROR;
        }
        else {
            syslog_ax_intf_dbg("add l3 interface to kap success\n");
            return INTERFACE_RETURN_CODE_SUCCESS;
        }
    }
}
/**********************************************************************************
	*  npd_intf_kap_set_dev_info
	*
	*  DESCRIPTION:
	*	   this routine set kernel virtual dev info
	*
	*  INPUT:
	*	   pif : struct if_cfg_struct* --  param of createing dev
	*
	*  OUTPUT:
	*
	*
	*  RETURN:
	*	   COMMON_RETURN_CODE_NULL_PTR	 -	NULL POINT	error
	*	   INTERFACE_RETURN_CODE_FD_ERROR  - fd is invalidate
	*	   INTERFACE_RETURN_CODE_IOCTL_ERROR   -  ioctl FAILED
	*	   INTERFACE_RETURN_CODE_SUCCESS  - set successed
	*
	**********************************************************************************/
static int npd_intf_kap_set_dev_info
(
   struct if_cfg_struct* pif
)
{	
    if (NULL == pif)
	{
        syslog_ax_intf_err("set kap dev info error parameter\n");
        return COMMON_RETURN_CODE_NULL_PTR;
    }

    if (!adptVirRxFd || adptVirRxFd < 0)
	{
        syslog_ax_intf_err("set kap dev info error fd %d\n", adptVirRxFd);
        return INTERFACE_RETURN_CODE_FD_ERROR;
    }
    else 
	{
		
        if (0 > ioctl(adptVirRxFd, KAPSETDEVINFO, pif))
		{
    	    syslog_ax_intf_err("set kap dev info ioctl fd %d cmd %#x error\n", \
    						  adptVirRxFd, KAPSETDEVINFO);
    	    return INTERFACE_RETURN_CODE_IOCTL_ERROR;
        }
	    else
		{
    	    syslog_ax_intf_dbg("set kap dev info success\n");
    	    return INTERFACE_RETURN_CODE_SUCCESS;
        }
    }
}

/*
* RETURN:
* 	   INTERFACE_RETURN_CODE_ERROR  - name is null
*	   COMMON_RETURN_CODE_NULL_PTR	 -	NULL POINT	error
*	   INTERFACE_RETURN_CODE_FD_ERROR  - fd is invalidate
*	   INTERFACE_RETURN_CODE_IOCTL_ERROR   -  ioctl FAILED
*	   INTERFACE_RETURN_CODE_SUCCESS  - set successed
*/
int npd_intf_kap_dev_info_set(unsigned char * name, unsigned int eth_g_index, unsigned int vid){

struct if_cfg_struct cmd;
int ret = 0;
if(!name) return INTERFACE_RETURN_CODE_ERROR;
memset(&cmd, 0, sizeof(struct if_cfg_struct));
/*cmd.if_name[15] = '\0';*/
strcpy(cmd.if_name, name);

cmd.if_flag = KAP_IFF_TAP | KAP_IFF_NO_PI;
cmd.dev_type = KAP_INTERFACE_PORT_E;
cmd.l2_index = eth_g_index;
cmd.l3_index = if_nametoindex(name);
cmd.vId = vid;
npd_syslog_dbg("name %s ifindex %#x\n", name, cmd.l3_index);
return npd_intf_kap_set_dev_info(&cmd);

}
/**********************************************************************************
 *  kap_del_intf
 *
 *  DESCRIPTION:
 *      this routine delete kernel virtual dev
 *
 *  INPUT:
 *      pif : struct if_cfg_struct* --  param of deleting dev
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      COMMON_RETURN_CODE_NULL_PTR   -  NULL POINT  error
 *      INTERFACE_RETURN_CODE_FD_ERROR  - fd is invalidate
 *      INTERFACE_RETURN_CODE_IOCTL_ERROR   -  ioctl FAILED
 *      INTERFACE_RETURN_CODE_SUCCESS  - operation successed
 *
 **********************************************************************************/
static int kap_del_intf
(
    struct if_cfg_struct* pif
) {
    if (NULL == pif) {
        syslog_ax_intf_err("delete l3 interface from kap error parameter\n");
        return COMMON_RETURN_CODE_NULL_PTR;
    }

    if (!adptVirRxFd || adptVirRxFd < 0) {
        syslog_ax_intf_err("delete l3 interface from kap error fd %d\n", adptVirRxFd);
        return INTERFACE_RETURN_CODE_FD_ERROR;
    }
    else {
        if (0 > ioctl(adptVirRxFd, KAPDELIFF, pif)) {
            syslog_ax_intf_err("delete l3 interface from kap ioctl fd %d cmd %#x error\n", \
                               adptVirRxFd, KAPDELIFF);
            return INTERFACE_RETURN_CODE_IOCTL_ERROR;
        }
        else {
            syslog_ax_intf_dbg("delete l3 interface from kap success\n");
            return INTERFACE_RETURN_CODE_SUCCESS;
        }
    }
}


/**********************************************************************************
  *  kap_adv_enable
  *
  *  DESCRIPTION:
  * 	 this routine set dev to advanced-routing interface
  *
  *  INPUT:
  * 	 pif : struct if_cfg_struct* --  param of deleting dev
  *
  *  OUTPUT:
  *
  *
  *  RETURN:
  * 	 COMMON_RETURN_CODE_NULL_PTR   -  NULL POINT  error
  * 	 INTERFACE_RETURN_CODE_FD_ERROR  - fd is invalidate
  * 	 INTERFACE_RETURN_CODE_IOCTL_ERROR	 -	ioctl FAILED
  * 	 INTERFACE_RETURN_CODE_SUCCESS	- operation successed
  *
  **********************************************************************************/
 static int kap_adv_enable
 (
	 struct if_cfg_struct* pif
 ) {
	 if (NULL == pif) {
		 syslog_ax_intf_err("delete l3 interface from kap error parameter\n");
		 return COMMON_RETURN_CODE_NULL_PTR;
	 }

	 if (!adptVirRxFd || adptVirRxFd < 0) {
		 syslog_ax_intf_err("delete l3 interface from kap error fd %d\n", adptVirRxFd);
		 return INTERFACE_RETURN_CODE_FD_ERROR;
	 }
	 else {
		 if (0 > ioctl(adptVirRxFd, KAPSETADVEN, pif)) {
			 syslog_ax_intf_err("delete l3 interface from kap ioctl fd %d cmd %#x error\n", \
								adptVirRxFd, KAPSETADVEN);
			 return INTERFACE_RETURN_CODE_IOCTL_ERROR;
		 }
		 else {
			 syslog_ax_intf_dbg("delete l3 interface from kap success\n");
			 return INTERFACE_RETURN_CODE_SUCCESS;
		 }
	 }
 }

/**********************************************************************************
 *  kap_set_status
 *
 *  DESCRIPTION:
 *      this routine set kernel virtual dev status
 *
 *  INPUT:
 *      pif : struct if_cfg_struct* --  param of deleting dev
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      COMMON_RETURN_CODE_NULL_PTR   -  NULL POINT  error
 *      INTERFACE_RETURN_CODE_FD_ERROR  - fd is invalidate
 *      INTERFACE_RETURN_CODE_IOCTL_ERROR   -  ioctl FAILED
 *      INTERFACE_RETURN_CODE_SUCCESS  - operation successed
 *
 **********************************************************************************/
static int kap_set_status
(
    struct if_cfg_struct* pif
) {
    if (NULL == pif) {
        syslog_ax_intf_err("set l3 interface status to kap error parameter\n");
        return COMMON_RETURN_CODE_NULL_PTR;
    }

    if (!adptVirRxFd || adptVirRxFd < 0) {
        syslog_ax_intf_err("set l3 interface status to kap error fd %d\n", adptVirRxFd);
        return INTERFACE_RETURN_CODE_FD_ERROR;
    }
    else {
        if (0 > ioctl(adptVirRxFd, KAPSETLINK, pif)) {
            syslog_ax_intf_err("set l3 interface status to kap ioctl fd %d cmd %#x error\n", \
                               adptVirRxFd, KAPSETLINK);
            return INTERFACE_RETURN_CODE_IOCTL_ERROR;
        }
        else {
            syslog_ax_intf_dbg("set l3 interface status to kap success\n");
            return INTERFACE_RETURN_CODE_SUCCESS;
        }
    }
}

/**********************************************************************************
 *  cavim_do_intf
 *
 *  DESCRIPTION:
 *      this routine create or delete cavim dev
 *
 *  INPUT:
 *      cmd : unsigned int -- create or delete
 *      param : void* -- associated param
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_FD_ERROR  - fd is invalidate
 *      INTERFACE_RETURN_CODE_IOCTL_ERROR   -  ioctl FAILED
 *      INTERFACE_RETURN_CODE_SUCCESS  - create successed
 *
 **********************************************************************************/
int cavim_do_intf
(
    unsigned int cmd,
    void* param
) {
    int promi_fd = 0;
    int rc = 0;

    promi_fd = open("/dev/oct0", 0);
    if (promi_fd < 0) {
        syslog_ax_intf_dbg("open /dev/oct0 fail\n");
        return INTERFACE_RETURN_CODE_FD_ERROR;
    }

    rc = ioctl(promi_fd, cmd, param);
    if (rc < 0) {
        syslog_ax_intf_dbg("ioctl cmd %#x fd %d on /dev/oct0 error %d errno %#x, ERROR: %s\n", \
                           cmd, promi_fd, rc, errno,strerror(errno));
        close(promi_fd);
        return INTERFACE_RETURN_CODE_IOCTL_ERROR;
    }

    close(promi_fd);
    return INTERFACE_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 *  kap_get_l3intf_info
 *
 *  DESCRIPTION:
 *      this routine get kernel virtual dev some infor
 *
  * INPUT:
 *      pif : struct if_cfg_struct* --  param of kap ioctl
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      COMMON_RETURN_CODE_NULL_PTR   -  NULL POINT  error
 *      INTERFACE_RETURN_CODE_FD_ERROR  - fd is invalidate
 *      INTERFACE_RETURN_CODE_IOCTL_ERROR   -  ioctl FAILED
 *      INTERFACE_RETURN_CODE_SUCCESS  - operation successed
 *
 **********************************************************************************/
static int kap_get_l3intf_info
(
    struct if_cfg_struct* pif
) {
    if (NULL == pif) {
        syslog_ax_intf_err("get l3 interface info from kap error parameter\n");
        return COMMON_RETURN_CODE_NULL_PTR;
    }

    if (!adptVirRxFd || adptVirRxFd < 0) {
        syslog_ax_intf_err("get l3 interface info from kap error fd %d\n", adptVirRxFd);
        return INTERFACE_RETURN_CODE_FD_ERROR;
    }
    else {
        if (0 > ioctl(adptVirRxFd, KAPGETDEVINFO, pif)) {
            syslog_ax_intf_err("get l3 interface info from kap ioctl fd %d cmd %#x error\n", \
                               adptVirRxFd, KAPGETDEVINFO);
            return INTERFACE_RETURN_CODE_IOCTL_ERROR;
        }
        else {
            syslog_ax_intf_dbg("get l3 interface info from kap success\n");
            return INTERFACE_RETURN_CODE_SUCCESS;
        }
    }
}

/**********************************************************************************
 *  kap_set_l3intf_mac_address
 *
 *  DESCRIPTION:
 *      this routine set unique kap interface mac address
 *
  * INPUT:
 *      pif : struct if_cfg_struct* --  param of kap ioctl
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      COMMON_RETURN_CODE_NULL_PTR   -  NULL POINT  error
 *      INTERFACE_RETURN_CODE_FD_ERROR  - fd is invalidate
 *      INTERFACE_RETURN_CODE_IOCTL_ERROR   -  ioctl FAILED
 *      INTERFACE_RETURN_CODE_SUCCESS  - operation successed
 *
 **********************************************************************************/
static int kap_set_l3intf_mac_address
(
    struct if_cfg_struct* pif
) {
    if (NULL == pif) {
        syslog_ax_intf_err("set l3 interface mac to kap error parameter\n");
        return COMMON_RETURN_CODE_NULL_PTR;
    }

    if (!adptVirRxFd || adptVirRxFd < 0) {
        syslog_ax_intf_err("set l3 interface mac to kap error fd %d\n", adptVirRxFd);
        return INTERFACE_RETURN_CODE_FD_ERROR;
    }
    else {
        if (0 > ioctl(adptVirRxFd, KAPSETMACADDR, pif)) {
            syslog_ax_intf_err("set l3 interface %d mac %02x:%02x:%02x:%02x:%02x:%02x ioctl fd %d cmd %#x error\n", \
                               pif->l3_index, pif->mac_addr[0], pif->mac_addr[1], pif->mac_addr[2], \
                               pif->mac_addr[3], pif->mac_addr[4], pif->mac_addr[5], \
                               adptVirRxFd, KAPGETDEVINFO);
            return INTERFACE_RETURN_CODE_IOCTL_ERROR;
        }
        else {
            syslog_ax_intf_dbg("set l3 interface %d mac %02x:%02x:%02x:%02x:%02x:%02x success\n", \
                               pif->l3_index, pif->mac_addr[0], pif->mac_addr[1], pif->mac_addr[2], \
                               pif->mac_addr[3], pif->mac_addr[4], pif->mac_addr[5]);
            return INTERFACE_RETURN_CODE_SUCCESS;
        }
    }
}
/**********************************************************************************
 *  npd_create_intf_by_vId
 *
 *  DESCRIPTION:
 *      this routine construct param of creating dev and call associated API
 *
 *  INPUT:
 *      vid : unsigned int  -- mirror profile
 *      name : char* -- dev name
 *
 *  OUTPUT:
 *      ifindex : unsigned  int* -- dev index
 *
 *  RETURN:
 *      COMMON_RETURN_CODE_NULL_PTR   -  NULL POINT  error
 *      INTERFACE_RETURN_CODE_FD_ERROR  - fd is invalidate
 *      INTERFACE_RETURN_CODE_IOCTL_ERROR   -  ioctl FAILED
 *      INTERFACE_RETURN_CODE_SUCCESS  -  intf create SUCCESSED
 *
 *
 **********************************************************************************/
int npd_intf_create_by_vId
(
    unsigned int vId,
    unsigned int* ifIndex,
    char* name,
    unsigned int opCode
) {
    struct if_cfg_struct cmd;
    int ret = 0;

    memset(&cmd, 0, sizeof(struct if_cfg_struct));
    /*cmd.if_name[15] = '\0';*/
    strcpy(cmd.if_name, name);

    cmd.if_flag = KAP_IFF_TAP | KAP_IFF_NO_PI;
    cmd.l3_index = 0;
    cmd.dev_type = KAP_INTERFACE_VID_E;
    cmd.dev_state = DEV_LINK_UP;
    cmd.l2_index = 0;
    cmd.vId = vId;

    ret = npd_check_vlan_status(vId);
    if (VLAN_STATE_DOWN_E == ret) {
        cmd.dev_state = DEV_LINK_DOWN;
    }
    syslog_ax_intf_dbg("create l3 if by vlan %d name %s state %s\n", \
                       vId, cmd.if_name, cmd.dev_state ? "UP" : "DOWN");
	if(NPD_INTF_ADV_DIS == opCode){
		ret = kap_adv_disable(&cmd);
	}
	else{
    	ret = kap_create_intf(&cmd);
	}
    if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
        syslog_ax_intf_err("create l3 if by vlan %d error\n",vId);
        return ret;
    }
    else {
        *ifIndex = cmd.l3_index;
        strcpy(name, cmd.if_name);
        syslog_ax_intf_dbg("new created l3 if name %s index %d\n", cmd.if_name, cmd.l3_index);
        return INTERFACE_RETURN_CODE_SUCCESS;
    }

}

/**********************************************************************************
 *  npd_intf_create_by_port_index
 *
 *  DESCRIPTION:
 *      this routine construct param of creating dev and call associated API
 *
 *  INPUT:
 *      port_index -- port index
 *      pvid              -- vid
 *      name          --  dev name
 *
 *  OUTPUT:
 *      ifindex       -- dev index
 *
 *  RETURN:
 *      COMMON_RETURN_CODE_NULL_PTR   -  NULL POINT  error
 *      INTERFACE_RETURN_CODE_FD_ERROR  - fd is invalidate
 *      INTERFACE_RETURN_CODE_IOCTL_ERROR   -  ioctl FAILED
 *      INTERFACE_RETURN_CODE_SUCCESS  -  intf create SUCCESSED
 *
 **********************************************************************************/
int npd_intf_create_by_port_index
(
    unsigned int port_index,
    unsigned short pvid,
    unsigned int* ifIndex,
    unsigned char* name,
    unsigned int opCode
) {
    struct if_cfg_struct cmd;
    int ret = 0;

	/* code optimize: Dereference before null check. zhangdi@autelan.com 2013-01-18 */
    if ((NULL == ifIndex) || (NULL == name))
	{
        return COMMON_RETURN_CODE_NULL_PTR;
    }
    memset(&cmd, 0, sizeof(struct if_cfg_struct));
    strcpy(cmd.if_name, (char *)name);
    /*cmd.if_name[15] = '\0';*/
    cmd.if_flag = KAP_IFF_TAP | KAP_IFF_NO_PI;
    cmd.l3_index = 0;
    cmd.dev_type = KAP_INTERFACE_PORT_E;
    cmd.dev_state = DEV_LINK_UP;
    cmd.l2_index = port_index;
    cmd.vId = pvid;

    if (ETH_ATTR_LINKDOWN == npd_check_eth_port_status(port_index)) {
        cmd.dev_state = DEV_LINK_DOWN;
    }
    syslog_ax_intf_dbg("create port %#x l3 interface %s state %s to kap fd %d\n", \
                       port_index, name, cmd.dev_state ? "UP" : "DOWN", adptVirRxFd);
	if(NPD_INTF_ADV_DIS == opCode){
    	ret = kap_adv_disable(&cmd);
	}
	else{
    	ret = kap_create_intf(&cmd);
	}
    if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
        syslog_ax_intf_dbg("create port %#x l3 interface %s state %s to kap fd %d error\n", \
                           port_index, name, cmd.dev_state ? "UP" : "DOWN", adptVirRxFd);
        return ret;
    }
    else {
        *ifIndex = cmd.l3_index;
        strcpy((char *)name, cmd.if_name);
        syslog_ax_intf_dbg("created port %#x l3 interface if %d give name %s get %s\n", \
                           port_index, cmd.l3_index, cmd.if_name, name);
        return INTERFACE_RETURN_CODE_SUCCESS;
    }

}

/**********************************************************************************
 *  npd_intf_del_by_ifindex
 *
 *  DESCRIPTION:
 *      this routine delete virtual dev
 *
 *  INPUT:
 *      ifindex -- dev index
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      COMMON_RETURN_CODE_NULL_PTR   -  NULL POINT  error
 *      INTERFACE_RETURN_CODE_FD_ERROR  - fd is invalidate
 *      INTERFACE_RETURN_CODE_IOCTL_ERROR   -  ioctl FAILED
 *      INTERFACE_RETURN_CODE_SUCCESS  -  intf del SUCCESSED
 *
 *
 **********************************************************************************/
int npd_intf_del_by_ifindex
(
    unsigned int ifindex,
    unsigned int opCode
) {
    struct if_cfg_struct cmd;
    int ret = 0;

    memset(&cmd, 0, sizeof(struct if_cfg_struct));
    syslog_ax_intf_dbg("npd_del_intf_by_ifindex :: intf index %d\n", ifindex);

    cmd.l3_index = ifindex;
	if(NPD_INTF_ADV_EN == opCode){
		ret = kap_adv_enable(&cmd);
	}
	else{
    	ret = kap_del_intf(&cmd);
	}
    if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
        syslog_ax_intf_dbg("kap_del_intf err\n");
        return ret;
    }
    else {
        syslog_ax_intf_dbg("npd_del_intf succeed\n");
        return INTERFACE_RETURN_CODE_SUCCESS;
    }

}

/**********************************************************************************
 *  npd_intf_config_basemac
 *
 *  DESCRIPTION:
 *      this routine set route mac
 *
 *  INPUT:
 *      devNum
 *      port
 *      mac
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      COMMON_RETURN_CODE_NULL_PTR  - null pointer error
 *      INTERFACE_RETURN_CODE_NAM_ERROR  - maybe driver operation failed
 *      INTERFACE_RETURN_CODE_SUCCESS  - operation success
 *
 *
 **********************************************************************************/
int npd_intf_config_basemac
(
    unsigned char  devNum,
    unsigned short port,
    unsigned char* mac
) {
    GT_ETHERADDR  macPtr;
    int ret = INTERFACE_RETURN_CODE_SUCCESS;

    memset(&macPtr, 0, sizeof(GT_ETHERADDR));
    if (NULL == mac) {
        return COMMON_RETURN_CODE_NULL_PTR;
    }
    ret = nam_intf_config_basemac(devNum, port, mac);
    return ret;
}

/**********************************************************************************
 *  npd_intf_create_router_mode
 *
 *  create intf based on ports,support l3 transmit.
 *
 *  INPUT:
 *      port_index -port index
 *      pname - dev name
 *
 *  OUTPUT:
 *      NULL
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_SUCCESS
 *      COMMON_RETURN_CODE_NULL_PTR
 *      INTERFACE_RETURN_CODE_ERROR
 *      INTERFACE_RETURN_CODE_NAM_ERROR
 *      INTERFACE_RETURN_CODE_CHECK_MAC_ERROR
 *      INTERFACE_RETURN_CODE_FDB_SET_ERROR
 *      INTERFACE_RETURN_CODE_FD_ERROR
 *      INTERFACE_RETURN_CODE_IOCTL_ERROR
 *      INTERFACE_RETURN_CODE_MAC_GET_ERROR
 *
 **********************************************************************************/
int npd_intf_create_router_mode
(
    unsigned int port_index,
    unsigned char* pname,
    unsigned int opCode
) {
    int ret = INTERFACE_RETURN_CODE_SUCCESS;
    unsigned short  pvid = 0;
    unsigned char   addr[6] = {0};
    unsigned int     ifindex = ~0UI;
    unsigned char   dev = 0, port = 0;
	unsigned int rollBackCode = NPD_INTF_DEL_INTF;

	if(NPD_INTF_ADV_DIS == opCode){
		rollBackCode = NPD_INTF_ADV_EN;
	}

    npd_eth_port_get_pvid(port_index, &pvid);
    if (NPD_PORT_L3INTF_VLAN_ID != pvid) {
        ret = INTERFACE_RETURN_CODE_ERROR;
    }
    else {
        npd_get_devport_by_global_index(port_index, &dev, &port);
        syslog_ax_intf_dbg("set port %#x route mode\n", port_index);
        ret = npd_intf_port_check(pvid, port_index, &ifindex);
        if (FALSE == ret) {
            ret = npd_eth_port_l3intf_create(port_index, ifindex);
            if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
                syslog_ax_intf_dbg("npd create intf is error\n");
                ret = INTERFACE_RETURN_CODE_ERROR;
            }
            else {
                ret = npd_intf_create_by_port_index(port_index, pvid, &ifindex, pname,opCode);
                if (INTERFACE_RETURN_CODE_SUCCESS == ret) {

                    ret = npd_eth_port_l3intf_create(port_index, ifindex);
                    npd_all_arp_clear_by_port(port_index);
                    syslog_ax_intf_dbg("npd create intf by port_index %d correct ,ifindex = %d\n", port_index, ifindex);
                    if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
                        syslog_ax_intf_dbg("npd create intf is error\n");
                        ret = INTERFACE_RETURN_CODE_ERROR;
                    }
                    else {
                        ret = INTERFACE_RETURN_CODE_SUCCESS;
                    }
                }
                else {
                    syslog_ax_intf_dbg("npd create intf is error\n");
                    npd_eth_port_l3intf_destroy(port_index, ifindex);
                    /*ret = INTERFACE_RETURN_CODE_ERROR;*/
                }
            }
        }
        else {
            ret = INTERFACE_RETURN_CODE_ERROR;
        }
    }

    if (INTERFACE_RETURN_CODE_SUCCESS == ret) {
        ret = npd_intf_get_intf_mac(ifindex, addr);
        if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
            syslog_ax_intf_dbg("npd get dev  mac ERROR\n");
            npd_intf_del_route_mode(port_index,rollBackCode);
            ret = INTERFACE_RETURN_CODE_MAC_GET_ERROR;
        }
        /* Check the legality of mac address,
          * multicast mac , broadcast mac, and all zero mac is in vaild
          * add by jinpc@autelan.com 20090829
          */
        else if (FALSE == npd_intf_l3_mac_legality_check(addr)) {
            syslog_ax_intf_err("the interface mac is illegal\n");
            npd_intf_del_route_mode(port_index,rollBackCode);
            return INTERFACE_RETURN_CODE_CHECK_MAC_ERROR;
        }
        else {
            ret = nam_static_fdb_entry_mac_set_for_l3(addr, pvid, CPU_PORT_VINDEX);
            if (NPD_SUCCESS != ret) {
                syslog_ax_intf_dbg("set static mac addr ERROR ret %d\n", ret);
                npd_intf_del_route_mode(port_index,rollBackCode);
                return INTERFACE_RETURN_CODE_FDB_SET_ERROR;
            }

            ret = npd_intf_config_basemac(dev, port, addr);
            if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
                syslog_ax_intf_dbg("intf config basemac ERROR ret %d\n", ret);
                npd_intf_del_route_mode(port_index,rollBackCode);
                return ret;
            }

            ret = nam_l3_intf_on_vlan_enable(0, pvid, addr, 1, ifindex);
            if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
                syslog_ax_intf_err("l3 intf on vlan enable error %d\n", ret);
                return ret;
            }

            ret =  nam_l3_intf_on_port_enable(port_index, pvid, 1);
            if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
                syslog_ax_intf_dbg("nam_l3_intf_on_port_enable ERROR ret %d\n", ret);
                npd_intf_del_route_mode(port_index,rollBackCode);
                return ret;
            }

            /* 5 :CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV4_BCAST_E*/
            /* 2: CPSS_PACKET_CMD_TRAP_TO_CPU_E*/
            ret = nam_vlan_unreg_filter(0, pvid, 5, 2);
            if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
                syslog_ax_intf_dbg("npd_trans_ports_on_vlan ERROR ret %d\n", ret);
                npd_intf_del_route_mode(port_index,rollBackCode);
                return ret;
            }
			nam_vlan_unreg_filter(dev, pvid, 3, 2); /*3 : for ipv6 mcast, 2: trap to cpu*/
            /*
              * by qinhs@autelan.com 11-10-2008
              *     RIPv1 packet is globally enable to mirror CPU when asic initialization.
              * It's not proper to enable/disable when interface creatation/deletion.
              */
#if 0
            if (INTERFACE_RETURN_CODE_SUCCESS != (ret = nam_rip_send_to_cpu(1))) {
                syslog_ax_intf_dbg("nam_rip_send_to_cpu ERROR ret %d\n", ret);
                npd_intf_del_route_mode(port_index,rollBackCode);
                return INTERFACE_RETURN_CODE_ERROR;
            }
#endif

            ret = nam_arp_trap_enable(dev, port, 1);
            if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
                syslog_ax_intf_dbg("nam_arp_trap_enable ERROR ret %d\n", ret);
                npd_intf_del_route_mode(port_index,rollBackCode);
                return ret;
            }

        }
    }
    return ret;
}


/**********************************************************************************
 *  npd_intf_del_route_mode
 *
 *  delete intf based on ports.
 *
 *  INPUT:
 *      port_index -port index
 *
 *  OUTPUT:
 *      NULL
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_SUCCESS
 *      COMMON_RETURN_CODE_NULL_PTR   -  NULL POINT  error
 *      INTERFACE_RETURN_CODE_FD_ERROR  - fd is invalidate
 *      INTERFACE_RETURN_CODE_IOCTL_ERROR   -  ioctl FAILED
 *      INTERFACE_RETURN_CODE_MAC_GET_ERROR
 *      INTERFACE_RETURN_CODE_INTERFACE_NOTEXIST
 *      INTERFACE_RETURN_CODE_FDB_SET_ERROR
 *      INTERFACE_RETURN_CODE_ERROR
 *
 **********************************************************************************/
int npd_intf_del_route_mode
(
    unsigned int port_index,
    unsigned int opCode
) {
    int ret = INTERFACE_RETURN_CODE_SUCCESS;
    unsigned short pvid = 0;
    unsigned int ifindex = ~0UI;
    unsigned char addr[6] = {0};
    unsigned char dev = 0, port = 0;
    unsigned int eth_g_index[64] = {0};
    unsigned int portCount = 0;
    unsigned int portBmp[2] = {0};

    npd_eth_port_get_pvid(port_index, &pvid);
    if (NPD_PORT_L3INTF_VLAN_ID != pvid) {
        ret = INTERFACE_RETURN_CODE_ERROR;
    }
    else {
        syslog_ax_intf_dbg("stratr >>> port interface del port_index :: %d \n", port_index);
        ret = npd_eth_port_interface_check(port_index, &ifindex);
        if ((TRUE == ret) && (ifindex != (~0UI))) {

            ret = npd_intf_get_intf_mac(ifindex, addr);
            if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
                syslog_ax_intf_dbg("npd get dev  mac ERROR\n");
                ret = INTERFACE_RETURN_CODE_MAC_GET_ERROR;
            }
            else {
                pthread_mutex_lock(&semPktRxMutex);
                pthread_mutex_lock(&semKapIoMutex);
                syslog_ax_intf_dbg("port_index % d is not a l3 interface\n", port_index);
                ret = npd_intf_del_by_ifindex(ifindex, opCode);
                if (INTERFACE_RETURN_CODE_SUCCESS == ret) {
                    syslog_ax_intf_dbg("npd del intf by port_index %d correct ,ifindex = %d\n", port_index, ifindex);
                    npd_all_arp_clear_by_port_ifIndex(port_index, ifindex);
                    ret = npd_eth_port_l3intf_destroy(port_index, ifindex);
                    if (NPD_SUCCESS != ret) {
                        syslog_ax_intf_dbg("npd del  intf is error\n");
                        ret = INTERFACE_RETURN_CODE_ERROR;
                    }
                    else {
                        ret = INTERFACE_RETURN_CODE_SUCCESS;
                    }
                }
                pthread_mutex_unlock(&semKapIoMutex);
                pthread_mutex_unlock(&semPktRxMutex);
            }
        }
        else {
            syslog_ax_intf_dbg("this port not existed port_index %d \n", port_index);
            ret = INTERFACE_RETURN_CODE_INTERFACE_NOTEXIST;
        }
    }

    if (INTERFACE_RETURN_CODE_SUCCESS == ret) {
        npd_vlan_get_port_bmp_by_mode(portBmp, ETH_PORT_FUNC_IPV4);
        if (npd_eth_port_count_of_bmp(portBmp) < 1) {
            ret = nam_l3_intf_on_vlan_enable(0, pvid, addr, 0, ifindex);
            if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
                syslog_ax_intf_err(" l3 intf on vlan disable error %d \n", ret);
            }
            ret = nam_no_static_fdb_entry_mac_vlan_set(addr, pvid);
            if (NPD_SUCCESS != ret) {
                syslog_ax_intf_dbg("set static mac addr ERROR ret %d\n", ret);
                ret = INTERFACE_RETURN_CODE_FDB_SET_ERROR;
            }
            else {
                ret = INTERFACE_RETURN_CODE_SUCCESS;
            }
        }
        if (INTERFACE_RETURN_CODE_SUCCESS == ret) {
            ret =  nam_l3_intf_on_port_enable(port_index, pvid, 0);
            if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
                syslog_ax_intf_dbg("nam_l3_intf_on_port_enable ERROR ret %d\n", ret);
                ret = INTERFACE_RETURN_CODE_NAM_ERROR;
            }
            /* 0 :CPSS_DXCH_BRG_VLAN_PACKET_UNK_UCAST_E*/
            /* 2: CPSS_PACKET_CMD_TRAP_TO_CPU_E
            if (INTERFACE_RETURN_CODE_SUCCESS != (ret = nam_vlan_unreg_filter(0, pvid, 0, 2))) {
                syslog_ax_intf_dbg("cpssDxChBrgVlanUnkUnregFilterSet ERROR ret %d\n", ret);
                ret = INTERFACE_RETURN_CODE_NAM_ERROR;
            }*/
            /*
              * by qinhs@autelan.com 11-10-2008
              *     RIPv1 packet is globally enable to mirror CPU when asic initialization.
              * It's not proper to enable/disable when interface creatation/deletion.
              */
#if 0
            /*send rip packets to cpu*/
            if (INTERFACE_RETURN_CODE_SUCCESS != (ret = nam_rip_send_to_cpu(0))) {
                syslog_ax_intf_dbg("nam_rip_send_to_cpu ERROR ret %d\n", ret);
                return INTERFACE_RETURN_CODE_NAM_ERROR;
            }
#endif

            npd_get_devport_by_global_index(port_index, &dev, &port);
            ret = nam_arp_trap_enable(dev, port, 0);
            if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
                syslog_ax_intf_dbg("cpssDxChBrgGenArpTrapEnable ERROR ret %d\n", ret);
                return INTERFACE_RETURN_CODE_NAM_ERROR;
            }
        }
    }

    return ret;

}
/**********************************************************************
*
*	DESCRIPT:
*		set eth port interface hardware enable to enable or disable
*	INPUT:
*		ifindex - uint32   interface ifindex
*		eth_g_index  - uint32   port global index
*		enable   - uint32  set to enable or disable
*	OUTPUT:
*		NONE
*	RETURN:
*		INTERFACE_RETURN_CODE_SUCCESS
*		INTERFACE_RETURN_CODE_NAM_ERROR
*		INTERFACE_RETURN_CODE_FDB_SET_ERROR
*
**********************************************************************/

int npd_intf_eth_port_interface_hw_enable_set
(
unsigned int ifindex,
unsigned int eth_g_index,
unsigned int enable
)
{
    int ret = INTERFACE_RETURN_CODE_SUCCESS;
    unsigned short pvid = NPD_PORT_L3INTF_VLAN_ID;
    unsigned char addr[6] = {0};
    unsigned char dev = 0, port = 0;
    unsigned int portCount = 0;
    unsigned int portBmp[2] = {0};
	
	ret = npd_intf_get_intf_mac(ifindex, addr);
	if(INTERFACE_RETURN_CODE_SUCCESS == ret){
		if(!enable){/*set asic for l3 disable*/
	        npd_vlan_get_port_bmp_by_mode(portBmp, ETH_PORT_FUNC_IPV4);
	        if (npd_eth_port_count_of_bmp(portBmp) < 1) {
	            ret = nam_l3_intf_on_vlan_enable(0, pvid, addr, 0, ifindex);
	            if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
	                syslog_ax_intf_err(" l3 intf on vlan disable error %d \n", ret);
	            }
	            ret = nam_no_static_fdb_entry_mac_vlan_set(addr, pvid);
	            if (NPD_SUCCESS != ret) {
	                syslog_ax_intf_dbg("set static mac addr ERROR ret %d\n", ret);
	                ret = INTERFACE_RETURN_CODE_FDB_SET_ERROR;
	            }
	            else {
	                ret = INTERFACE_RETURN_CODE_SUCCESS;
	            }
	        }
	        if (INTERFACE_RETURN_CODE_SUCCESS == ret) {
	            ret =  nam_l3_intf_on_port_enable(eth_g_index, pvid, 0);
	            if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
	                syslog_ax_intf_dbg("nam_l3_intf_on_port_enable ERROR ret %d\n", ret);
	                ret = INTERFACE_RETURN_CODE_NAM_ERROR;
	            }
	            /* 0 :CPSS_DXCH_BRG_VLAN_PACKET_UNK_UCAST_E*/
	            /* 2: CPSS_PACKET_CMD_TRAP_TO_CPU_E*/
	            if (INTERFACE_RETURN_CODE_SUCCESS != (ret = nam_vlan_unreg_filter(0, pvid, 0, 2))) {
	                syslog_ax_intf_dbg("cpssDxChBrgVlanUnkUnregFilterSet ERROR ret %d\n", ret);
	                ret = INTERFACE_RETURN_CODE_NAM_ERROR;
	            }
	           
	            npd_get_devport_by_global_index(eth_g_index, &dev, &port);
	            ret = nam_arp_trap_enable(dev, port, 0);
	            if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
	                syslog_ax_intf_dbg("cpssDxChBrgGenArpTrapEnable ERROR ret %d\n", ret);
	                ret = INTERFACE_RETURN_CODE_NAM_ERROR;
	            }
	        }
		}
		else{/*set asic for l3 enable*/
                ret = nam_static_fdb_entry_mac_set_for_l3(addr, pvid, CPU_PORT_VINDEX);
                if (NPD_SUCCESS != ret) {
                    syslog_ax_intf_dbg("set static mac addr ERROR ret %d\n", ret);
                    return INTERFACE_RETURN_CODE_FDB_SET_ERROR;
                }

                ret = npd_intf_config_basemac(dev, port, addr);
                if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
                    syslog_ax_intf_dbg("intf config basemac ERROR ret %d\n", ret);
                    return ret;
                }

                ret = nam_l3_intf_on_vlan_enable(0, pvid, addr, 1, ifindex);
                if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
                    syslog_ax_intf_err("l3 intf on vlan enable error %d\n", ret);
                    return ret;
                }

                ret =  nam_l3_intf_on_port_enable(eth_g_index, pvid, 1);
                if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
                    syslog_ax_intf_dbg("nam_l3_intf_on_port_enable ERROR ret %d\n", ret);
                    return ret;
                }

                /* 5 :CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV4_BCAST_E*/
                /* 2: CPSS_PACKET_CMD_TRAP_TO_CPU_E*/
                ret = nam_vlan_unreg_filter(0, pvid, 5, 2);
                if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
                    syslog_ax_intf_dbg("npd_trans_ports_on_vlan ERROR ret %d\n", ret);
                    return ret;
                }
                ret = nam_arp_trap_enable(dev, port, 1);
                if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
                    syslog_ax_intf_dbg("nam_arp_trap_enable ERROR ret %d\n", ret);
                    return ret;
                }

        }
	}
	return ret;
		
}
/**********************************************************************************
 *  npd_intf_get_intf_mac
 *
 *  DESCRIPTION:
 *      this routine get dev mac address
 *
 *  INPUT:
 *      ifindex             --  dev index
 *
 *  OUTPUT:
 *      addr            -- mac address
 *
 *  RETURN:
 *      COMMON_RETURN_CODE_NULL_PTR
 *      INTERFACE_RETURN_CODE_FD_ERROR
 *      INTERFACE_RETURN_CODE_IOCTL_ERROR
 *      INTERFACE_RETURN_CODE_SUCCESS
 *
 *
 **********************************************************************************/
#define ENODEV 19
int npd_intf_get_intf_mac
(
    unsigned int ifIndex,
    unsigned char* addr
) {
    struct if_cfg_struct cmd;
    int i = 0;
    if (NULL == addr) {
        return COMMON_RETURN_CODE_NULL_PTR;
    }
    memset(&cmd, 0, sizeof(struct if_cfg_struct));
    syslog_ax_intf_dbg("npd_intf_get_intf_mac::KAP(fd %d) L3 %#x get mac addr\n", adptVirRxFd, ifIndex);
    if (!adptVirRxFd || adptVirRxFd < 0) {
        syslog_ax_intf_err("npd_intf_get_intf_mac::KAP fd %d invalided\n", adptVirRxFd);
        return INTERFACE_RETURN_CODE_FD_ERROR;
    }
    else {
        cmd.l3_index = ifIndex;
        if (0 > ioctl(adptVirRxFd, KAPGETMACADDR, &cmd)) {
            if (ENODEV != errno) {
                syslog_ax_intf_err("KAP ioctl %#x get L3 mac fail,errno %d\n", KAPGETMACADDR, errno);
                return INTERFACE_RETURN_CODE_IOCTL_ERROR;
            }
            else {
                for (i = 0; i < 6; i++) {
                    addr[i] = cmd.mac_addr[i];
                }
                syslog_ax_intf_dbg("get intf mac: %02x:%02x:%02x:%02x:%02x:%02x\n", \
                                   addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
                return INTERFACE_RETURN_CODE_SUCCESS;
            }
        }
        else {
            for (i = 0; i < 6; i++) {
                addr[i] = cmd.mac_addr[i];
            }
            syslog_ax_intf_dbg("get intf mac: %02x:%02x:%02x:%02x:%02x:%02x\n", \
                               addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
            return INTERFACE_RETURN_CODE_SUCCESS;
        }
    }
}

/**********************************************************************************
 *  npd_intf_set_vlan_l3intf_status
 *
 *  DESCRIPTION:
 *      this routine set vlan dev status
 *
 *  INPUT:
 *      cmd  --  param
 *      event -- up or down
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_ERROR   -  error occured,set failed
 *      INTERFACE_RETURN_CODE_SUCCESS  - set success
 *
 *
 **********************************************************************************/
int npd_intf_set_vlan_l3intf_status
(
    unsigned short vid,
    enum vlan_status_e  event
) {
    struct if_cfg_struct cmd;
    unsigned int ifindex = ~0UI;
    unsigned int ret = 0;
    enum VLAN_PORT_SUBIF_FLAG flag = VLAN_PORT_SUBIF_NOTEXIST;
    unsigned int ve_cmd = 0;
    ve_vlan_param_data_s ve_vlan_param;

    if (VLAN_STATE_UP_E == event) {
        ve_cmd = CVM_IOC_INTF_UP;
        cmd.dev_state = DEV_LINK_UP;
    }
    else if (VLAN_STATE_DOWN_E == event) {
        ve_cmd = CVM_IOC_INTF_DOWN;
        cmd.dev_state = DEV_LINK_DOWN;
    }
    else {
        syslog_ax_intf_err("set vlan %d l3 interface status with event %d error\n", \
                           vid, event);
        return INTERFACE_RETURN_CODE_ERROR;
    }

    ret = npd_vlan_get_interface_ve_flag(vid, &flag);
    if (NPD_SUCCESS == ret && flag == VLAN_PORT_SUBIF_EXIST) {
        syslog_ax_intf_dbg("vlan %d advanced-routing interface link %s\n", \
                           vid, (VLAN_STATE_UP_E == event) ? "UP" : "DOWN");
        memset(&ve_vlan_param, 0, sizeof(ve_vlan_param_data_s));
		if((PRODUCT_ID_AX7K_I == PRODUCT_ID)&&\
			((0 == LOCAL_CHASSIS_SLOT_NO)||(1 == LOCAL_CHASSIS_SLOT_NO))&&\
			(NPD_VLAN_OBC0_VLAN_ID == vid)){
        	sprintf(ve_vlan_param.intfName, "obc0");				
		}
		else{
        	sprintf(ve_vlan_param.intfName, "vlan%d", vid);
		}
        ve_vlan_param.vid = vid;

        ret = cavim_do_intf(ve_cmd, &ve_vlan_param);
        if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
            syslog_ax_eth_port_err("vlan %d advanced-routing link %s tell kmod error\n",  \
                                   vid, (VLAN_STATE_UP_E == event) ? "UP" : "DOWN");
            return INTERFACE_RETURN_CODE_ERROR;
        }
        return INTERFACE_RETURN_CODE_SUCCESS;
    }

    ret = npd_vlan_interface_check(vid, &ifindex);
    if ((TRUE == ret) && (ifindex != ~0UI)) {
        syslog_ax_intf_dbg("vlan %d l3 interface link %s\n", \
                           vid, (VLAN_STATE_UP_E == event) ? "UP" : "DOWN");
        cmd.l3_index = ifindex;

        if (!kap_set_status(&cmd)) {
            syslog_ax_intf_dbg("vlan %d interface link %s tell kmod success\n", \
                               vid, (VLAN_STATE_UP_E == event) ? "UP" : "DOWN");
            return INTERFACE_RETURN_CODE_SUCCESS;
        }
        else {
            syslog_ax_intf_err("vlan %d interface link %s tell kmod fail\n", \
                               vid, (VLAN_STATE_UP_E == event) ? "UP" : "DOWN");
            return INTERFACE_RETURN_CODE_ERROR;
        }
    }
    else {
        syslog_ax_intf_err("set vlan %d l3 status %s but not interface,do nothing\n", \
                           vid, (VLAN_STATE_UP_E == event) ? "UP" : "DOWN");
        return INTERFACE_RETURN_CODE_SUCCESS;
    }
}

/**********************************************************************************
 *  npd_intf_get_vlan_l3intf_status
 *
 *  DESCRIPTION:
 *      this routine get vlan dev status
 *
 *  INPUT:
 *      cmd  --  param
 *      event -- up or down
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      DEV_LINK_DOWN-the interface is down
 *           DEV_LINK_UP-the interface is up
 *           INTERFACE_RETURN_CODE_ERROR  -  status check failed
 *           INTERFACE_RETURN_CODE_INTERFACE_NOTEXIST - l3 interface not exists
 *
 *
 **********************************************************************************/

unsigned int npd_intf_get_l3intf_status
(
    unsigned short vid,
    unsigned int eth_g_index
) {
    struct if_cfg_struct cmd;
    unsigned int ifindex = ~0UI;
    int ret = 0;
    KAP_DEV_LINK_STATUS intfStatus = DEV_LINK_DOWN;
    memset(&cmd, 0, sizeof(struct if_cfg_struct));
    ret = npd_vlan_interface_check(vid, &ifindex);
    if (!((TRUE == ret) && (ifindex != ~0UI))) {
        ret = npd_eth_port_interface_check(eth_g_index, &ifindex);
        if (!((NPD_PORT_L3INTF_VLAN_ID == vid) && (TRUE == ret) && (ifindex != ~0UI))) {
            syslog_ax_intf_dbg("l3 check found neither port %#x nor vlan %d is l3 interface\n", \
                               eth_g_index, vid);
            ret = INTERFACE_RETURN_CODE_INTERFACE_NOTEXIST;
            return ret;
        }
        else {
            intfStatus = npd_check_eth_port_status(eth_g_index);
            if (ETH_ATTR_LINKUP == intfStatus) {
                ret = DEV_LINK_UP;
            }
            else if (ETH_ATTR_LINKDOWN == intfStatus) {
                ret = DEV_LINK_DOWN;
            }
            else {
                ret = INTERFACE_RETURN_CODE_ERROR;
            }
        }
    }
    else {
        intfStatus = npd_check_vlan_status(vid);
        if (VLAN_STATE_UP_E == intfStatus) {
            ret = DEV_LINK_UP;
        }
        else if (VLAN_STATE_DOWN_E == intfStatus) {
            ret = DEV_LINK_DOWN;
        }
        else {
            ret = INTERFACE_RETURN_CODE_ERROR;
        }
    }
    return ret;
}


/****************************************************
 *npd_intf_l3_mac_legality_check
 *  Description:
 *      to check the mac  of L3 interface legality,
 *      multicast mac , broadcast mac, and all zero mac is in vaild
 *
 *  Input:
 *      ethmac      - source mac
 *
 *  Output:
 *      NULL
 *
 *  ReturnCode:
 *      TRUE        - the L3 macs  legal
 *      FALSE       - the L3 macs  illegal
 *
 ****************************************************/
unsigned int npd_intf_l3_mac_legality_check
(
    unsigned char * ethmac
) {
    if (NULL == ethmac) {
        return FALSE;
    }

    syslog_ax_intf_dbg("check L3 interface mac legality:%02x:%02x:%02x:%02x:%02x:%02x\n",
                       ethmac[0], ethmac[1], ethmac[2], ethmac[3], ethmac[4], ethmac[5]);

    if (npd_arp_snooping_is_brc_mac(ethmac)) {
        syslog_ax_intf_err("the mac is broadcast mac!\n");
        return FALSE;
    }

    if (npd_arp_snooping_is_muti_cast_mac(ethmac)) {
        syslog_ax_intf_err("the mac is multicast mac!\n");
        return FALSE;
    }

    if (npd_arp_snooping_is_zero_mac(ethmac)) {
        syslog_ax_intf_err("the mac is zero mac!\n");
        return FALSE;
    }

    return TRUE;
}

/**********************************************************************************
 *  npd_intf_set_port_l3intf_status
 *
 *  DESCRIPTION:
 *      this routine set port dev status
 *
 *  INPUT:
 *      eth_g_index -- destination port index
 *      event           --  up or down
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_ERROR
 *      INTERFACE_RETURN_CODE_SUCCESS
 *
 *
 **********************************************************************************/
int npd_intf_set_port_l3intf_status
(
    unsigned int eth_g_index,
    enum ETH_PORT_NOTIFIER_ENT event
) {
    struct if_cfg_struct cmd;
    unsigned int ifindex = ~0UI;
    int ret = INTERFACE_RETURN_CODE_SUCCESS;

    ret = npd_eth_port_interface_check(eth_g_index, &ifindex);
    if (TRUE == ret && ifindex != ~0UI) {
        cmd.l3_index = ifindex;
        if (event == ETH_PORT_NOTIFIER_LINKUP_E)
            cmd.dev_state = DEV_LINK_UP;
        else if (event == ETH_PORT_NOTIFIER_LINKDOWN_E)
            cmd.dev_state = DEV_LINK_DOWN;

        if (!kap_set_status(&cmd)) {
            syslog_ax_intf_dbg("kap_set_status:: success\n");
            return INTERFACE_RETURN_CODE_SUCCESS;
        }
        else {
            syslog_ax_intf_dbg("kap_set_status:: fail\n");
            return INTERFACE_RETURN_CODE_ERROR;
        }
    }
    else {
        syslog_ax_intf_dbg("npd_set_port_l3intf_status:: l3intf not exists");
        return INTERFACE_RETURN_CODE_SUCCESS;
    }
}
#if 0
/**********************************************************************************
 *  npd_intf_get_ip_addr
 *
 *  This method is used to get L3 interface ip address and mask.
 *
 *  INPUT:
 *      ifindex - L3 interface index
 *
 *  OUTPUT:
 *      ipAddr - ip address
 *      mask - ip mask
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_SUCCESS
 *      INTERFACE_RETURN_CODE_ERROR
 *
 **********************************************************************************/
int npd_intf_get_ip_addr
(
    unsigned int   ifindex,
    unsigned int  *ipAddr,
    unsigned int  *mask
) {
    struct if_cfg_struct cmd;
    struct sockaddr_in *if_data;
    int ret = INTERFACE_RETURN_CODE_SUCCESS;

    if (!adptVirRxFd || adptVirRxFd < 0) {
        syslog_ax_intf_err("fd invalid\n");
        return INTERFACE_RETURN_CODE_ERROR;
    }
    else {
        memset(&cmd, 0, sizeof(cmd));

        /*find if_index*/
        cmd.l3_index = ifindex;
        if_data = (struct sockaddr_in *)cmd.if_addr;
        if_data->sin_addr.s_addr = INVALID_HOST_IP;
        cmd.netmask[0] = 0;
        ret = ioctl(adptVirRxFd, KAPGETIPADDR, &cmd);
        if (0 > ret) {
            syslog_ax_intf_err("get ip failed \n");
            return INTERFACE_RETURN_CODE_ERROR;
        }
        else {

            *ipAddr = if_data->sin_addr.s_addr;
            *mask = cmd.netmask[0];
            syslog_ax_intf_dbg("get if %#0x %d.%d.%d.%d %d.%d.%d.%d\n", ifindex, \
                               (*ipAddr >> 24)&0xFF, (*ipAddr >> 16)&0xFF, (*ipAddr >> 8)&0xFF, (*ipAddr)&0xFF, \
                               (*mask >> 24)&0xFF, (*mask >> 16)&0xFF, (*mask >> 8)&0xFF, (*mask)&0xFF);
        }
    }

    return INTERFACE_RETURN_CODE_SUCCESS;
}
#endif
/**********************************************************************************
 *  npd_intf_get_ip_addrs
 *
 *  This method is used to get L3 interface ip address and mask.
 *
 *  INPUT:
 *      ifindex - L3 interface index
 *
 *  OUTPUT:
 *      ipAddr - ip address
 *      mask - ip mask
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_SUCCESS  -  get ip success
 *      INTERFACE_RETURN_CODE_FD_ERROR  -  fd illegal
 *      INTERFACE_RETURN_CODE_IOCTL_ERROR  -  ioctl failed or error occured
 *
 **********************************************************************************/
int npd_intf_get_ip_addrs
(
    unsigned int   ifindex,
    unsigned int  *ipAddr,
    unsigned int  *mask
) {
    struct if_cfg_struct cmd;
    struct sockaddr_in *if_data;
    int ret = INTERFACE_RETURN_CODE_SUCCESS;
    int i = 0;

    if (!adptVirRxFd || adptVirRxFd < 0) {
        syslog_ax_intf_err("fd invalid\n");
        return INTERFACE_RETURN_CODE_FD_ERROR;
    }
    else {
        memset(&cmd, 0, sizeof(cmd));

        /*find if_index*/
        cmd.l3_index = ifindex;
        if_data = (struct sockaddr_in *)cmd.if_addr;
        for (i = 0; i < MAX_IP_COUNT; i++) {
            if_data[i].sin_family = AF_INET;
            if_data[i].sin_addr.s_addr = INVALID_HOST_IP;
            cmd.netmask[i] = 0;
        }
        pthread_mutex_lock(&semKapIoMutex);
        ret = ioctl(adptVirRxFd, KAPGETIPADDRS, &cmd);
        pthread_mutex_unlock(&semKapIoMutex);
        if (0 > ret) {
            syslog_ax_intf_dbg("npd_intf_get_ip_addrs::get ip failed \n");
            return INTERFACE_RETURN_CODE_IOCTL_ERROR;
        }
        else {
            syslog_ax_intf_dbg("npd_intf_get_ip_addrs::get if %#0x ip address:\n", ifindex);
            for (i = 0; i < MAX_IP_COUNT; i++) {
                ipAddr[i] = if_data[i].sin_addr.s_addr;
                mask[i] = cmd.netmask[i];
                if (INVALID_HOST_IP != ipAddr[i]) {
                    syslog_ax_intf_dbg("\t%d.%d.%d.%d %d.%d.%d.%d\n",  \
                                       (ipAddr[i] >> 24)&0xFF, (ipAddr[i] >> 16)&0xFF, (ipAddr[i] >> 8)&0xFF, (ipAddr[i])&0xFF, \
                                       (mask[i] >> 24)&0xFF, (mask[i] >> 16)&0xFF, (mask[i] >> 8)&0xFF, (mask[i])&0xFF);
                }
            }

        }
    }

    return INTERFACE_RETURN_CODE_SUCCESS;
}


/**********************************************************************************
 *  npd_intf_get_info
 *
 *  DESCRIPTION:
 *      this routine get dev infor
 *
 *  INPUT:
 *      name -- dev name
 *
 *  OUTPUT:
 *      ifindex -- dev index
 *      vid     -- vlan id
 *      eth-g-index -- port index
 *      type    -- dev type
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_ERROR
 *      INTERFACE_RETURN_CODE_SUCCESS
 *
 **********************************************************************************/
int npd_intf_get_info
(
    char * name,
    unsigned int *ifIndex,
    unsigned int* vid,
    unsigned int* eth_g_index,
    KAP_DEV_TYPE * type
) {
    struct if_cfg_struct cmd;
    int ret = INTERFACE_RETURN_CODE_SUCCESS;
    if ((NULL == name) || (NULL == ifIndex) || \
        (NULL == vid) || (NULL == eth_g_index) || \
        (NULL == type)) {
        return COMMON_RETURN_CODE_NULL_PTR;
    }
    memset(&cmd, 0, sizeof(struct if_cfg_struct));
    /*cmd.if_name[15] = '\0';*/
    strcpy(cmd.if_name, name);

    ret = kap_get_l3intf_info(&cmd);
    if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
        syslog_ax_intf_err("get interface %s info from kmod error %d\n", \
                           cmd.if_name, ret);
        return INTERFACE_RETURN_CODE_ERROR;
    }
    else {
        *ifIndex = cmd.l3_index;
        *vid = cmd.vId;
        *eth_g_index = cmd.l2_index;
        *type = cmd.dev_type;
        syslog_ax_intf_dbg("get interface %s info:if %d vid %d port %#x type %d state %d\n", \
                           cmd.if_name, cmd.l3_index, cmd.vId, cmd.l2_index, cmd.dev_type, cmd.dev_state);
        return INTERFACE_RETURN_CODE_SUCCESS;
    }

}

/**********************************************************************************
 *  npd_intf_set_mac_address
 *
 *  DESCRIPTION:
 *      this routine set mac address for a specified l3 interface
 *
 *  INPUT:
 *      name -- dev name
 *
 *  OUTPUT:
 *      ifindex -- dev index
 *      vid     -- vlan id
 *      eth-g-index -- port index
 *      type    -- dev type
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_ERROR
 *      INTERFACE_RETURN_CODE_SUCCESS
 *
 **********************************************************************************/
int npd_intf_set_mac_address
(
    unsigned int ifIndex,
    unsigned char * macAddr
) {
    struct if_cfg_struct pif;
    int ret = INTERFACE_RETURN_CODE_SUCCESS;

    if (!macAddr) {
        syslog_ax_intf_err("set interface %d mac address with mac null\n", ifIndex);
        return INTERFACE_RETURN_CODE_ERROR;
    }
    memset(&pif, 0, sizeof(struct if_cfg_struct));
    /*cmd.if_name[15] = '\0';*/
    memcpy(pif.mac_addr , macAddr, 6);
    pif.l3_index = ifIndex;

    ret = kap_set_l3intf_mac_address(&pif);
    if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
        syslog_ax_intf_err("set interface %d mac address to kmod error %d\n", \
                           pif.l3_index, ret);
        return INTERFACE_RETURN_CODE_ERROR;
    }
    else {
        syslog_ax_intf_dbg("set interface %d mac %02x:%02x:%02x:%02x:%02x:%02x success\n", \
                           pif.l3_index, pif.mac_addr[0], pif.mac_addr[1], pif.mac_addr[2], \
                           pif.mac_addr[3], pif.mac_addr[4], pif.mac_addr[5]);
        return INTERFACE_RETURN_CODE_SUCCESS;
    }

}

/**********************************************************************************
 *  npd_intf_trans_ports_on_vlan
 *
 *  trans all ports in the valn and enable ports router and arp trap.
 *
 *  INPUT:
 *      vid - vlan id
 *      enable - open/close
 *
 *  OUTPUT:
 *      NULL
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_SUCCESS
 *      INTERFACE_RETURN_CODE_VLAN_NOTEXIST
 *      INTERFACE_RETURN_CODE_ERROR
 *
 **********************************************************************************/
int npd_intf_trans_ports_on_vlan
(
    unsigned int vid,
    unsigned int enable,
    unsigned char * mac
) {
    struct vlan_s* node = NULL;
    struct vlan_port_list_node_s *portNode = NULL;
    vlan_port_list_s* portList = NULL;
    struct list_head *pos = NULL, *list  = NULL;
    unsigned int eth_g_index = 0;
    unsigned char dev = 0, port = 0;
    int ret = INTERFACE_RETURN_CODE_SUCCESS;

    node = npd_find_vlan_by_vid(vid);
    if (NULL == node) {
        syslog_ax_intf_dbg("npd_trans_ports_on_vlan:: don't find vlan %d  node \n", vid);
        return INTERFACE_RETURN_CODE_VLAN_NOTEXIST;
    }
	
	/* For interface vlan on MASTER, init cscd port. zhangdi@autelan.com 2011-07-21 */
	/* Just support distributed L3 vlan on AX81-SMU, not on AX71-crsmu % AX81-AC12C */
    if((IS_DISTRIBUTED == SYSTEM_TYPE)&&(BOARD_TYPE == BOARD_TYPE_AX81_SMU))
    {
        int i=0,devnum =0,portnum=0;
        for(i=0; i < asic_board->asic_cscd_port_cnt; i++)
        {
        	devnum = asic_board->asic_cscd_ports[i].dev_num;
        	portnum = asic_board->asic_cscd_ports[i].port_num;

    		/* Jump lion 1, for we do not use L3 on lion-1. zhangdi@autelan.com 2012-04-05 */
    		if(devnum == 1)
    		{
    			continue;				
    		}

            npd_get_global_index_by_devport(devnum, portnum, &eth_g_index);
            /* 1 */
            ret = nam_ports_route_on_vlan_enable(eth_g_index, enable);			
            if(0!= ret)
            {
                syslog_ax_intf_err("nam_ports_route_on_vlan_enable (%d,%d) error !\n",devnum,portnum);
                return -1;
            }
            syslog_ax_intf_dbg("nam_ports_route_on_vlan_enable (%d,%d) OK\n",devnum, portnum);

			/* 2 */
            /*port ARPTrapEnable set 0,same to default,prepare for ARP mirror-CPU*/
            ret = nam_arp_trap_enable(devnum, portnum, 0);
            if(0!= ret)
            {
                syslog_ax_intf_err("nam_arp_trap_enable (%d,%d) error !\n",devnum,portnum);
                return -1;
            }
            syslog_ax_intf_dbg("nam_arp_trap_enable (%d,%d) OK\n",devnum, portnum);
    		
            if (1 == enable)
                ret = npd_intf_config_basemac(devnum, portnum, mac);
        }
		if (INTERFACE_RETURN_CODE_SUCCESS != ret)
		{
            ret = INTERFACE_RETURN_CODE_ERROR;
        }
		
		/* If on AX81-SMU, just return because have not eth-port */
        return ret;				
    }

	/* Set the L3 attribute for eth-port */
    portList = node->untagPortList;
    if (NULL != portList) {
        /*syslog_ax_intf_dbg("npd_vlan_trans_ports_on_vlan :: into the untag ports list\n");*/
        if (0 != portList->count) {
            /*syslog_ax_intf_dbg("npd vlan trans untag ports:: vlan %d  port count %d",vid,portList->count);*/
            list = &(portList->list);
            __list_for_each(pos, list) {
                portNode = list_entry(pos, struct vlan_port_list_node_s, list);
                if (portNode) {
                    eth_g_index = portNode->eth_global_index;
                    ret |= nam_ports_route_on_vlan_enable(eth_g_index, enable);
                    npd_get_devport_by_global_index(eth_g_index, &dev, &port);
                    /*port ARPTrapEnable set 0,same to default,prepare for ARP mirror-CPU*/
                    ret |= nam_arp_trap_enable(dev, port, 0);
                    if (1 == enable)
                        ret |= npd_intf_config_basemac(dev, port, mac);
                }
            }
        }
    }

    portList = node->tagPortList;
    if (NULL != portList) {
        /*syslog_ax_intf_dbg("npd_vlan_trans_ports_on_vlan :: into the tag ports list\n");*/
        if (0 != portList->count) {
            /*syslog_ax_intf_dbg("npd vlan trans tag ports:: vlan %d port count %d",vid,portList->count);*/
            list = &(portList->list);
            __list_for_each(pos, list) {
                portNode = list_entry(pos, struct vlan_port_list_node_s, list);
                if (portNode) {
                    eth_g_index = portNode->eth_global_index;
                    ret |= nam_ports_route_on_vlan_enable(eth_g_index, enable);
                    npd_get_devport_by_global_index(eth_g_index, &dev, &port);
                    ret |= nam_arp_trap_enable(dev, port, 0);
                    if (1 == enable)
                        ret |= npd_intf_config_basemac(dev, port, mac);
                }
            }
        }
    }
    if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
        ret = INTERFACE_RETURN_CODE_ERROR;
    }
    return ret;

}


/**********************************************************************************
 *  npd_intf_vlan_check
 *
 *  check vlan member wether have port interface.
 *
 *  INPUT:
 *      vid - vlan id
 *
 *  OUTPUT:
 *
 *      ifindex - l3 interface index
 *
 *  RETURN:
 *      COMMON_RETURN_CODE_NULL_PTR  - null pointer error
 *      TRUE - the vlan is l3 intf
 *      FALSE - is not l3 intf
 *
 **********************************************************************************/
int npd_intf_vlan_check
(
    unsigned short vid,
    unsigned int* ifindex
) {
    struct vlan_s* node = NULL;
    struct vlan_port_list_node_s *portNode = NULL;
    vlan_port_list_s* portList = NULL;
    struct list_head *pos = NULL, *list  = NULL;
    int ret = 0;
    if (NULL == ifindex) {
        return COMMON_RETURN_CODE_NULL_PTR;
    }
    ret = npd_vlan_interface_check(vid, ifindex);
    if ((TRUE == ret) && (*ifindex != (~0UI))) {
        return TRUE;
    }
#if 0
    node = npd_find_vlan_by_vid(vid);
    if (NULL == node) {
        syslog_ax_intf_dbg("npd_l3intf_vlan_check:: don't find vlan %d node \n", vid);
        return FALSE;
    }
    /*
           this section is not necessary because the route mode port is belong to vlan 4095
        portList = node->untagPortList;
        if(NULL != portList)
        {
            //syslog_ax_intf_dbg("npd_l3intf_vlan_check :: into the untag ports list\n");
            if(0 != portList->count) {
                //syslog_ax_intf_dbg("npd vlan trans untag ports:: vlan %d  port count %d",vid,portList->count);
                list = &(portList->list);
                __list_for_each(pos,list) {
                    portNode = list_entry(pos,struct vlan_port_list_node_s,list);
                    if(NULL != portNode) {
                        ret |= npd_eth_port_interface_check(portNode->eth_global_index,ifindex);
                        if(ret == TRUE && *ifindex !=(~0UI)) {
                            return TRUE;
                        }
                    }
                }
            }
        }
    */

    portList = node->tagPortList;
    if (NULL != portList) {
        /*syslog_ax_intf_dbg("npd_vlan_trans_ports_on_vlan :: into the tag ports list\n");*/
        if (0 != portList->count) {
            /*syslog_ax_intf_dbg("npd vlan trans tag ports:: vlan %d port count %d",vid,portList->count);*/
            list = &(portList->list);
            __list_for_each(pos, list) {
                portNode = list_entry(pos, struct vlan_port_list_node_s, list);
                if (NULL != portNode) {
                    ret |= npd_eth_port_interface_check(portNode->eth_global_index, ifindex);
                    if (ret == TRUE && *ifindex != (~0UI)) {
                        return FALSE;
                    }
                }
            }
        }
    }
#endif
    return FALSE;
}

/**********************************************************************************
 *  npd_intf_udp_bc_trap_enable
 *
 *  DESCRIPTION:
 *      this routine set udp bc trap
 *
 *  INPUT:
 *      vlanid
 *      enable
 *
 *  OUTPUT:
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_NAM_ERROR  - enable set failed
 *      INTERFACE_RETURN_CODE_SUCCESS  -  set successed
 *
 **********************************************************************************/
unsigned int npd_intf_udp_bc_trap_enable
(
    unsigned int vlanId,
    unsigned int enable
) {
    unsigned int ret = INTERFACE_RETURN_CODE_SUCCESS;

    ret = nam_asic_udp_bc_trap_en((unsigned short)vlanId, enable);
    if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
        NPD_ERROR(("npd_udp_bc_trap_enable: return val %d.\n", ret));
    }
    return ret ;
}

/**********************************************************************************
 *  npd_intf_port_check
 *
 *  check port  wether has l3 interface or pvid has l3 interface.
 *
 *  INPUT:
 *      vid - vlan id
 *      eth_g_index - port global index
 *
 *  OUTPUT:
 *
 *      ifindex - l3 interface index
 *
 *  RETURN:
 *      COMMON_RETURN_CODE_NULL_PTR  - null pointer error
 *      TRUE - find  the vlan has port l3 interface
 *      FALSE - don't find
 *
 **********************************************************************************/
int npd_intf_port_check
(
    unsigned short vid,
    unsigned int eth_g_index,
    unsigned int* ifindex
) {
    int ret = 0;
    enum VLAN_PORT_SUBIF_FLAG  flag;
    if (NULL == ifindex) {
        return COMMON_RETURN_CODE_NULL_PTR;
    }
    ret = npd_eth_port_interface_check(eth_g_index, ifindex);
    if ((NPD_PORT_L3INTF_VLAN_ID == vid) && (TRUE == ret) && (*ifindex != (~0UI))) {
        return TRUE;
    }

    ret = npd_vlan_get_interface_ve_flag(vid, &flag);
    if (NPD_SUCCESS == ret && flag == VLAN_PORT_SUBIF_EXIST) {
        return TRUE;
    }
    ret = npd_vlan_interface_check(vid, ifindex);
    if ((TRUE == ret) && (*ifindex != (~0UI))) {
        return TRUE;
    }

    return FALSE;
}

/**********************************************************************************
 *  npd_intf_check_by_index
 *
 *  check port  wether has l3 interface or the vlan that port take part in has l3 interface
 *
 *  INPUT:
 *
 *      eth_g_index - port global index
 *
 *
 *  OUTPUT:
 *
 *      ifindex - l3 interface index
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_SUCCESS - find  the vlan has port l3 interface
 *      INTERFACE_RETURN_CODE_ERROR - don't find
 *
 **********************************************************************************/
int npd_intf_check_by_index
(
    unsigned int eth_g_index,
    unsigned int* isIfindex
) {
    int ret = 0;
    unsigned int ifindex = ~0UI;
    struct eth_port_s* ethPort = NULL;
    struct port_based_vlan_s * portVlan = NULL;
    dot1q_vlan_list_s * dot1qVlan = NULL;
    struct eth_port_dot1q_list_s* node = NULL;
    struct list_head *pos = NULL, *list  = NULL;


    ethPort = npd_get_port_by_index(eth_g_index);
    if (NULL == ethPort) {
        syslog_ax_eth_port_err("npd eth port link status change:index %#x node null", eth_g_index);
        return INTERFACE_RETURN_CODE_ERROR;
    }

    ret = npd_eth_port_interface_check(eth_g_index, &ifindex);
    if ((TRUE == ret) && (ifindex != (~0UI))) {
        *isIfindex = TRUE;
        return INTERFACE_RETURN_CODE_SUCCESS;
    }

    if (NULL != ethPort->funcs.func_data[ETH_PORT_FUNC_PORT_BASED_VLAN]) {
        /*untagged vlan*/
        portVlan = (struct port_based_vlan_s *)(ethPort->funcs.func_data[ETH_PORT_FUNC_PORT_BASED_VLAN]);
        if (NULL != portVlan) {
            ret = npd_vlan_interface_check(portVlan->vid, &ifindex);
            if ((TRUE == ret) && (ifindex != (~0UI))) {
                *isIfindex = TRUE;
                return INTERFACE_RETURN_CODE_SUCCESS;
            }
        }
    }
    else if (NULL != ethPort->funcs.func_data[ETH_PORT_FUNC_DOT1Q_BASED_VLAN]) {
        /*tagged vlan*/
        dot1qVlan = (dot1q_vlan_list_s *)(ethPort->funcs.func_data[ETH_PORT_FUNC_DOT1Q_BASED_VLAN]);
        if (NULL != dot1qVlan) {
            list = &(dot1qVlan->list);
            __list_for_each(pos, list) {
                node = list_entry(pos, struct eth_port_dot1q_list_s, list);
                if (node) {
                    ret = npd_vlan_interface_check(portVlan->vid, &ifindex);
                    if ((TRUE == ret) && (ifindex != (~0UI))) {
                        *isIfindex = TRUE;
                        return INTERFACE_RETURN_CODE_SUCCESS;
                    }
                }
            }
        }
    }

    return INTERFACE_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 *  npd_intf_set_vlan_attr
 *
 *  DESCRIPTION:
 *      this routine set vlan intf attribute
 *
 *  INPUT:
 *      vid -- vlan id
 *      addr -- MAC
 *      enable
 *
 *  OUTPUT:
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_ERROR
 *      INTERFACE_RETURN_CODE_SUCCESS
 *
 *
 **********************************************************************************/
int npd_intf_set_vlan_attr
(
    unsigned short vid,
    unsigned char* addr,
    unsigned int enable
) {
    int ret = INTERFACE_RETURN_CODE_SUCCESS;
    unsigned char dev = 0;
    unsigned int ipCntlType = 0;
    unsigned int ifindex = 0;

    if (enable) {
        ipCntlType = 3; /* CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E*/
    }
    else {
        ipCntlType = 0; /* CPSS_DXCH_BRG_IP_CTRL_NONE_E,*/
    }
    if (enable) {
        ret = npd_intf_vlan_check(vid, &ifindex);
        if (TRUE != ret) {
            npd_syslog_dbg("The vid %d has no l3 intf!\n", vid);
            return INTERFACE_RETURN_CODE_ERROR;
        }
    }
    if (INTERFACE_RETURN_CODE_SUCCESS != (ret = nam_l3_intf_on_vlan_enable(0, vid, addr, enable, ifindex))) {
        syslog_ax_intf_dbg("nam_l3_intf_on_vlan_enable ERROR ret %d\n", ret);
        ret = INTERFACE_RETURN_CODE_ERROR;
    }

    else if (INTERFACE_RETURN_CODE_SUCCESS != (ret = nam_intf_ip_ctrl_tocpu(dev, vid, ipCntlType))) {
        /* 1 --enable CPSS_DXCH_BRG_IP_CTRL_IPV4_E -- Enable IPv4 Control traffic to CPU */
        /* 0 --enbale CPSS_DXCH_BRG_IP_CTRL_NONE_E - Disable both IPv4 and Ipv6 Contro traffic to CPU*/
        syslog_ax_intf_dbg("set vlan %d on device %d ip control to CPU %s error %d\n",  \
                           vid, dev, (enable ? "enable" : "disable"), ret);
        ret = INTERFACE_RETURN_CODE_ERROR;
    }
    else if ((1 == enable) && (INTERFACE_RETURN_CODE_SUCCESS != (ret = npd_intf_trans_ports_on_vlan(vid, enable, addr)))) {
        syslog_ax_intf_err("npd_trans_ports_on_vlan ERROR ret %d\n", ret);
        ret = INTERFACE_RETURN_CODE_ERROR;
    }
    /* 5 :CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV4_BCAST_E*/
    /* 2: CPSS_PACKET_CMD_TRAP_TO_CPU_E*/
    else if (5 == enable) {
        if (INTERFACE_RETURN_CODE_SUCCESS != (ret = nam_vlan_unreg_filter(0, vid, 5, 2))) {
            syslog_ax_intf_dbg("npd_trans_ports_on_vlan ERROR ret %d\n", ret);/*packet type ipv6 mcast is 3,ipv4 mcast is 2*/
            ret = INTERFACE_RETURN_CODE_ERROR;
        }
    }
    else if (INTERFACE_RETURN_CODE_SUCCESS != (ret = npd_intf_udp_bc_trap_enable(vid, enable))) {
        NPD_DEBUG(("npd_udp_bc_trap_enable: return val %d error.", ret));
        ret = INTERFACE_RETURN_CODE_ERROR;
    }
    else
        ret = INTERFACE_RETURN_CODE_SUCCESS;


	nam_vlan_unreg_filter(dev, vid, 3, enable ? 1 : 0); /*3 : for ipv6 mcast, 1: mirror to cpu, 0 forward */
    return ret;
}

/**********************************************************************************
 *  npd_intf_create_vlan_l3intf
 *
 *  DESCRIPTION:
 *      this routine create vlan virtual dev and set static FDB
 *
 *  INPUT:
 *      vid -- vlan id
 *      name  -- dev name
 *      addr      --  system mac address
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_ERROR
 *      INTERFACE_RETURN_CODE_SUCCESS
 *      INTERFACE_RETURN_CODE_ALREADY_ADVANCED
 *      INTERFACE_RETURN_CODE_VLAN_NOTEXIST
 *      INTERFACE_RETURN_CODE_INTERFACE_EXIST
 *
 **********************************************************************************/
int npd_intf_create_vlan_l3intf
(
    unsigned short      vid,
    char*   name,
    unsigned char*  addr,
    unsigned int opCode
) {
    int ret = 0;
    unsigned int ifindex  = ~0UI;
    enum VLAN_PORT_SUBIF_FLAG flag = VLAN_PORT_SUBIF_NOTEXIST;

    syslog_ax_intf_dbg("create l3 interface %s with vid %d mac %02x:%02x:%02x:%02x:%02x:%02x\n", \
                       name ? name : "nil", vid, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
    ret = npd_vlan_get_interface_ve_flag(vid, &flag);
    if (NPD_SUCCESS == ret && flag == VLAN_PORT_SUBIF_EXIST) {
        ret = INTERFACE_RETURN_CODE_ALREADY_ADVANCED;
        return ret;
    }

    ret = npd_intf_vlan_check(vid, &ifindex);
    if (FALSE == ret) {
        ret = npd_intf_create_by_vId(vid, &ifindex, name, opCode);
        if (INTERFACE_RETURN_CODE_SUCCESS == ret) {
            syslog_ax_intf_dbg("npd create vlan %d intf correct, ifindex =%d\n", vid, ifindex);
            ret = npd_vlan_l3intf_create(vid, ifindex);
            if (VLAN_RETURN_CODE_VLAN_NOT_EXISTS == ret) {
                syslog_ax_intf_dbg("npd_vlan_l3intf_create error\n");
                ifindex = ~0UI;
                ret = INTERFACE_RETURN_CODE_VLAN_NOTEXIST;
            }
            else if (VLAN_RETURN_CODE_ERR_NONE == ret) {
                ret = INTERFACE_RETURN_CODE_SUCCESS;
            }
            else {
                ret = INTERFACE_RETURN_CODE_ERROR;
            }
        }
		else if(INTERFACE_RETURN_CODE_IOCTL_ERROR==ret)
		{
            syslog_ax_intf_dbg("npd create vlan %d on kap failed, ifindex =%d\n", vid, ifindex);			
		}
        else {
            ret = INTERFACE_RETURN_CODE_ERROR;
        }
    }
    else {
        if ((TRUE == ret) && (ifindex != ~0UI)) {
            ret = INTERFACE_RETURN_CODE_INTERFACE_EXIST;
        }
        else {
            ret = INTERFACE_RETURN_CODE_ERROR;
        }
    }

    if (INTERFACE_RETURN_CODE_SUCCESS == ret) {
        if (NPD_SUCCESS != (ret = nam_static_fdb_entry_mac_set_for_l3(addr, vid, CPU_PORT_VINDEX))) {
            syslog_ax_intf_err("set static mac addr ERROR ret %d\n", ret);
            pthread_mutex_lock(&semPktRxMutex);
			if(NPD_INTF_ADV_DIS == opCode){
				npd_intf_del_by_ifindex(ifindex, NPD_INTF_ADV_EN);
			}
			else{
            	npd_intf_del_by_ifindex(ifindex, NPD_INTF_DEL_INTF);
			}
            npd_arp_clear_by_vid_ifIndex(vid, ifindex);
            npd_vlan_l3intf_destroy(vid, ifindex);
            pthread_mutex_unlock(&semPktRxMutex);
            ret = INTERFACE_RETURN_CODE_ERROR;
        }
        else {
            ret = INTERFACE_RETURN_CODE_SUCCESS;
        }
    }
    return ret;
}


/**********************************************************************************
 *  npd_intf_del_vlan_l3intf
 *
 *  DESCRIPTION:
 *      this routine delete vlan l3intf
 *
 *  INPUT:
 *      vid -- vlan id
 *      ifindex -- dev index
 *      addr            --  mac
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_ERROR
 *      INTERFACE_RETURN_CODE_VLAN_NOTEXIST
 *      INTERFACE_RETURN_CODE_SUCCESS
 *
 *
 **********************************************************************************/
int npd_intf_del_vlan_l3intf
(
    unsigned short vid,
    unsigned int ifindex,
    unsigned char* addr,
    unsigned int opCode
) {
    unsigned int ret = INTERFACE_RETURN_CODE_SUCCESS;


    syslog_ax_intf_dbg("stratr >>>del l3_intf vid :: %d  \n", vid);

    pthread_mutex_lock(&semPktRxMutex);
    ret = npd_intf_del_by_ifindex(ifindex, opCode);
    if (INTERFACE_RETURN_CODE_SUCCESS == ret) {
        npd_arp_clear_by_vid_ifIndex(vid, ifindex); /*add by hanhui*/
        ret =  npd_vlan_l3intf_destroy(vid, ifindex);
        if (VLAN_RETURN_CODE_VLAN_NOT_EXISTS == ret) {
            syslog_ax_intf_dbg("npd_vlan_l3intf_destroy error\n");
            ifindex = ~0UI;
            ret = INTERFACE_RETURN_CODE_VLAN_NOTEXIST;
        }
        else if (VLAN_RETURN_CODE_ERR_NONE == ret) {
            ret = INTERFACE_RETURN_CODE_SUCCESS;
        }
        else {
            ret = INTERFACE_RETURN_CODE_ERROR;
        }
    }
    else {
        ret = INTERFACE_RETURN_CODE_ERROR;
    }
    pthread_mutex_unlock(&semPktRxMutex);
    if (INTERFACE_RETURN_CODE_SUCCESS == ret) {
        ret = nam_no_static_fdb_entry_mac_vlan_set(addr, vid);
        if (NPD_SUCCESS != ret) {
            syslog_ax_intf_err("set static mac addr ERROR ret %d\n", ret);
        }
        ret = INTERFACE_RETURN_CODE_SUCCESS;
    }

    return ret;
}

/**********************************************************************************
 *  npd_intf_execute_cmd
 *
 *  DESCRIPTION:
 *      this routine call shell command
 *
 *  INPUT:
 *      command -- command name
 *      cmd_param
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      -1   - error
 *      0   - success and not status change
 *      pid - the pid whose status changed
 *
 **********************************************************************************/
int npd_intf_execute_cmd
(
    const char* command,
    const char** cmd_param
) {
    int ret;
    pid_t pid;
    int status;
    int errno;

    /* Call fork(). */
    pid = fork();

    if (pid < 0) {
        /* Failure of fork(). */
        syslog_ax_intf_dbg("Can't fork: %s\n", strerror(errno));
        exit(1);
    }
    else if (pid == 0) {
        /* This is child process. */
        ret = execvp(command, (const char * const *)cmd_param);
        /* When execlp suceed, this part is not executed. */
        syslog_ax_intf_dbg("Can't execute %s: %s\n", command, strerror(errno));
        exit(1);
    }
    else {
        /* This is parent. */
        ret = wait4(pid, &status, 0, NULL);
        syslog_ax_intf_dbg("%s: wait4 exit code %#x\n", __func__,WEXITSTATUS(status));
        return WEXITSTATUS(status);
    }

    return ret;
}

/**********************************************************************************
 *  npd_intf_create_subif
 *
 *  DESCRIPTION:
 *      this routine call npd_intf_execute_cmd create subintf
 *
 *  INPUT:
 *      slot
 *      port
 *      vid     --  vlan id
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_SUCCESS
 *      INTERFACE_RETURN_CODE_ERROR
 *
 **********************************************************************************/
int npd_intf_create_subif
(
	int ifnameType,
    unsigned char slot,
    unsigned char local_port_no,
    unsigned int    vid,
    unsigned int    vid2,
    unsigned int  status
) {

#define UP 1
#define DOWN 0
    int   ret  = 0;
    char name[21] = {0};
	char newName[21] = {0};
    char cvid[5] = {0};
    char *cmd[16] = {0};
	
    sprintf(name, "eth%d-%d",slot, local_port_no);/*parent interface ethx-y*/
    sprintf(cvid, "%d", vid);
    sprintf(newName, "%s", name);
	
    ret |= npd_intf_set_intf_state(name,UP);

    if (0 != vid2) {/*create qinq subif parent interface is subif*/
        sprintf(newName, "%s.%d", newName, vid);
        sprintf(cvid, "%d", vid2);
		if(ifnameType){
			memset(newName,0,21);
			sprintf(newName,"e%s.%d",(name+3),vid);
		}
        ret |= npd_intf_set_intf_state(newName,UP);
    }

    memset(cmd, 0, 16);
    cmd[0] = "vconfig";
    cmd[1] = "add";
	cmd[2] = newName;
    cmd[3] = cvid;
    ret |= npd_intf_execute_cmd("vconfig", (const char **)cmd);
	
    sprintf(newName, "%s.%d",newName, (0 == vid2) ? vid : vid2);/*set the name as subif or qinq subif*/
	sprintf(name,"%s",newName);
	if((0 == vid2)&&(ifnameType)){/*create subif as new ifname format*/
		memset(newName,0,21);
		sprintf(newName,"e%s",name+3);
		npd_intf_change_intf_name(name,newName,UP);
	}
	else{
    	ret |= npd_intf_set_intf_state(name,UP);
	}
	
    if (!status) {
		memset(name,0,21);
		memset(newName,0,21);
        sprintf(name, "eth%d-%d",slot, local_port_no);
        npd_intf_set_intf_state(name,DOWN);/*parent*/
        sprintf(name, "%s.%d", name, vid);			
        sprintf(newName, "%s", name);
		if(ifnameType){
            sprintf(newName, "e%s", name+3);
		}
        npd_intf_set_intf_state(newName,DOWN);/*subif*/
		
        if (0 != vid2) {
            sprintf(newName, "%s.%d", newName, vid2);
            npd_intf_set_intf_state(newName,DOWN);/*qinq subif*/
        }
    }
	if(ret){
		return INTERFACE_RETURN_CODE_ERROR;
	}
    return INTERFACE_RETURN_CODE_SUCCESS;

}

int get_ioctl_fd(void)
{
	int s_errno;
	int fd = -1;

	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if (fd >= 0)
	{
		return fd;
	}
	
	s_errno = errno;
	fd = socket(PF_PACKET, SOCK_DGRAM, 0);
	if (fd >= 0)
	{
		return fd;
	}
	
	fd = socket(PF_INET6, SOCK_DGRAM, 0);
	if (fd >= 0)
	{
		return fd;
	}
	
	errno = s_errno;
	syslog_ax_intf_err("Cannot create control socket\n");
	
	return -1;
}


static int change_ifName(const char *ifName, char *ifNewname)
{
	struct ifreq ifr;
	int fd;
    syslog_ax_intf_dbg("chang ifname!\n");

	if(!ifName || !ifNewname)
		return -1;
	
	fd = get_ioctl_fd();
	if (fd < 0)
	{
		return -1;
	}

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, ifName, IFNAMSIZ); 
	strncpy(ifr.ifr_newname, ifNewname, IFNAMSIZ); 
	if (ioctl(fd, SIOCSIFNAME, &ifr) < 0) {
		syslog_ax_intf_err("SIOCSIFNAME IOCTL error!\n");
		close(fd);
		return -1;
	}
	close(fd);

	return INTERFACE_RETURN_CODE_SUCCESS; 
}



static int change_mac_addr(const char *ifName, char *ifHwaddr)
{
	struct ifreq ifr;
	int fd;
	
    syslog_ax_intf_dbg("Chang %s mac address!\n",ifName);

	if(!ifName || !ifHwaddr)
		return -1;
	
	fd = get_ioctl_fd();
	if (fd < 0)
	{
		return -1;
	}

	memset(&ifr, 0, sizeof(ifr));

    ifr.ifr_addr.sa_family = ARPHRD_ETHER;
    strncpy(ifr.ifr_name, (const char *)ifName, IFNAMSIZ - 1);
    memcpy((unsigned char *)ifr.ifr_hwaddr.sa_data, ifHwaddr, 13);
	
	if (ioctl(fd, SIOCSIFHWADDR, &ifr) < 0) {
		syslog_ax_intf_err("SIOCSIFHWADDR IOCTL error!\n");
		close(fd);
		return -1;
	}
	close(fd);

	return INTERFACE_RETURN_CODE_SUCCESS; 
}

int npd_intf_create_ve_subif
(
	int ifnameType,
    unsigned char slot,
    unsigned char cpu_no,
    unsigned char cpu_port_no,    
    unsigned int  vid,
    unsigned int  vid2
) 
{

#define UP 1
#define DOWN 0
    int   ret  = 0;
    char name[21] = {0};
	char newName[21] = {0};
    char cvid[5] = {0};
    char *cmd[16] = {0};
	macAddr_data_t macAddr_data;

    if(ifnameType == CONFIG_INTF_VE){		
        /*sprintf(name, "ve%d",slot);*/
        sprintf(name,"ve%02d%c%d",slot,(cpu_no==1)?'f':'s',cpu_port_no);		
    }
	
	if(PRODUCT_ID == PRODUCT_ID_AX7K_I && slot == SLOT_ID)
	{
        strcpy(name, "obc0");		
	} 

    sprintf(cvid, "%d", vid);
    sprintf(newName, "%s", name);
	
    /*ret |= npd_intf_set_intf_state(name,UP);*/

    if (0 != vid2) {/*create qinq subif parent interface is subif*/
		if(PRODUCT_ID == PRODUCT_ID_AX7K_I && slot == SLOT_ID) {
			sprintf(newName,"ve%02d%c%d.%d", slot,(cpu_no==1)?'f':'s',cpu_port_no, vid);			
			/*sprintf(newName, "ve%d.%d", slot, vid);*/
		}else {
            sprintf(newName, "%s.%d", newName, vid);
		}
        sprintf(cvid, "%d", vid2);
        /*ret |= npd_intf_set_intf_state(newName,UP);*/
    }
    memset(cmd, 0, 16);
    cmd[0] = "vconfig";
    cmd[1] = "add";
	cmd[2] = newName;
    cmd[3] = cvid;

	syslog_ax_intf_dbg("cmd[0] %s \n",cmd[0]);			
	syslog_ax_intf_dbg("cmd[1] %s \n",cmd[1]);			
	syslog_ax_intf_dbg("cmd[2] %s \n",cmd[2]);			
	syslog_ax_intf_dbg("cmd[3] %s \n",cmd[3]);		
	
    ret |= npd_intf_execute_cmd("vconfig", (const char **)cmd);
	
    sprintf(newName, "%s.%d",newName, (0 == vid2) ? vid : vid2);/*set the name as subif or qinq subif*/
	sprintf(name,"%s",newName);

    /*ret |= npd_intf_set_intf_state(name,DOWN);*/
	
	if(PRODUCT_ID == PRODUCT_ID_AX7K_I && slot == SLOT_ID && vid2 == 0)
    {
		sprintf(newName,"ve%02d%c%d.%d", slot,(cpu_no==1)?'f':'s',cpu_port_no, vid);		
		/*sprintf(newName, "ve%d.%d",slot,vid);*/
        change_ifName(name, newName);
	}
	
	memset(&macAddr_data, 0, sizeof(macAddr_data_t));
	
	macAddr_data.slot_id = slot;
    cavim_do_intf(CVM_IOC_GET_ETHER_HWADDR, &macAddr_data);

	change_mac_addr(newName, macAddr_data.hwaddr);


	/* Update the state of ve sub-interface on Active-MCB */
    if(((g_vlanlist[vid-1].bond_slot[slot-1])&(unsigned int)(1<<(8*(cpu_no-1)+(cpu_port_no-1))))!=0)
    {
		/* if bonded, and vlan is UP, set interface UP */
		if(g_vlanlist[vid-1].updown==1)
		{	
            ret |= npd_intf_set_intf_state(newName,UP);	
            syslog_ax_intf_dbg("vlan is up,set %s UP ret %d\n",newName,ret);			
		}		
    }

	if(ret){
		return INTERFACE_RETURN_CODE_ERROR;
	}
    return INTERFACE_RETURN_CODE_SUCCESS;

}

int npd_intf_set_intf_state(unsigned char * ifname,unsigned int state){
	
    int   ret  = 0;
    char *cmd[16] = {0};
	if(!ifname){
		return COMMON_RETURN_CODE_NULL_PTR;
	}
    cmd[0] = "ifconfig";
    cmd[1] = ifname;
	cmd[2] = "down";
	if(state){
    	cmd[2] = "up";
	}
    ret = npd_intf_execute_cmd("ifconfig", (const char **)cmd);
	
	return ret;
}
/**********************************************************************************
    *  npd_intf_set_intf_qinq_type
    *
    *  DESCRIPTION:
    *      this routine call npd_intf_execute_cmd set vlan dev qinq type
    *
    *  INPUT:
    *      intfName -- dev name
    *      type -- type value string eg. "0x88A8","0x8100"
    *
    *  OUTPUT:
    *
    *
    *  RETURN:
    *      INTERFACE_RETURN_CODE_SUCCESS
    *      INTERFACE_RETURN_CODE_ERROR
    *
    **********************************************************************************/
   int npd_intf_set_intf_qinq_type
   (
       unsigned char *intfName,
       unsigned char *type
   ) {
       int   ret  = 0;
       char *cmd[16] = {0};
       unsigned int typeValue = 0;
#ifndef ETH_P_ARP
#define ETH_P_ARP  0x0806
#define ETH_P_IP   0x0800
#define ETH_P_IPV6 0x86DD
#endif
       if((!intfName)||(!type)){
           return COMMON_RETURN_CODE_NULL_PTR;
       }
       typeValue = strtoul(type,NULL,0);
       if(((typeValue)&&(typeValue<1536))||\
           (ETH_P_ARP == typeValue)||\
           (ETH_P_IP == typeValue)||\
           (ETH_P_IPV6 == typeValue)){
           return COMMON_RETURN_CODE_BADPARAM;
       }
       cmd[0] = "vconfig";
       cmd[1] = "set_qinq_type";
       cmd[2] = intfName;
       cmd[3] = type;
       ret = npd_intf_execute_cmd("vconfig", (const char **)cmd);
       if(ret){
           syslog_ax_intf_dbg("set qinq type,ret %#x \n",ret);
           if(0x1b == ret){
               return INTERFACE_RETURN_CODE_QINQ_TYPE_FULL;
           }
           return INTERFACE_RETURN_CODE_ERROR;
       }
       return INTERFACE_RETURN_CODE_SUCCESS;
   }

/**********************************************************************************
 *  npd_intf_del_subif
 *
 *  DESCRIPTION:
 *      this routine delete subif
 *
 *  INPUT:
 *      slot
 *      port
 *      vid         --  vlan id
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_SUCCESS
 *
 *
 **********************************************************************************/
int npd_intf_del_subif
(
    unsigned char * pname,
    unsigned int    vid
) {
    int   ret  = 0;
    char name[16] = {0};
    char *cmd[16] = {0};
    if (NULL == pname) return INTERFACE_RETURN_CODE_ERROR;
    sprintf(name, "%s.%d", pname, vid);

    cmd[0] = "vconfig";
    cmd[1] = "rem";
    cmd[2] = name;

	syslog_ax_intf_dbg("cmd[0] %s \n",cmd[0]);			
	syslog_ax_intf_dbg("cmd[1] %s \n",cmd[1]);			
	syslog_ax_intf_dbg("cmd[2] %s \n",cmd[2]);			

    ret = npd_intf_execute_cmd("vconfig", (const char **)cmd);
	if(ret){
		return INTERFACE_RETURN_CODE_ERROR;
	}
    return INTERFACE_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 *  npd_intf_advanced_route_cvm_kmod_op
 *
 *  DESCRIPTION:
 *      this routine install or remove cavium-ip-offload.ko from kernel
 *
 *  INPUT:
 *      install - install(1) or remove(0) kernel module
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_SUCCESS
 *
 *
 **********************************************************************************/
int npd_intf_advanced_route_cvm_kmod_op
(
    unsigned int  install
) {
#define CVM_OFFLOAD_KMOD_NAME "/lib/modules/2.6.16.26-Cavium-Octeon/misc/cavium-ip-offload.ko"
    int   ret  = 0;
    char opStr[16] = {0};
    char *cmd[16] = {0};

    sprintf(opStr, "%s", install ? "insmod" : "rmmod");


    cmd[0] = "sudo";
    cmd[1] = opStr;
    cmd[2] = CVM_OFFLOAD_KMOD_NAME;

    ret = npd_intf_execute_cmd("sudo", (const char **)cmd);

    return INTERFACE_RETURN_CODE_SUCCESS;
}


/**********************************************************************************
 *  npd_intf_create_advanced_route_l3intf
 *
 *  DESCRIPTION:
 *      this routine create cavim dev
 *
 *  INPUT:
 *      vid -- vlan id
 *
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_SUCCESS
 *      INTERFACE_RETURN_CODE_ERROR
 *
 *
 **********************************************************************************/
int npd_intf_create_advanced_route_l3intf
(
    unsigned int vid,
    unsigned int opCode
) {
    int ret = INTERFACE_RETURN_CODE_SUCCESS;
    struct DRV_VLAN_PORT_BMP_S untagBmp, tagBmp;
    ve_vlan_param_data_s advaIntf;

    memset(&untagBmp, 0, sizeof(struct DRV_VLAN_PORT_BMP_S));
    memset(&tagBmp, 0, sizeof(struct DRV_VLAN_PORT_BMP_S));
    memset(&advaIntf, 0, sizeof(ve_vlan_param_data_s));

    advaIntf.vid = vid;
	if((PRODUCT_ID_AX7K_I == PRODUCT_ID)&&\
			((0 == LOCAL_CHASSIS_SLOT_NO)||(1 == LOCAL_CHASSIS_SLOT_NO))&&\
			(NPD_VLAN_OBC0_VLAN_ID == vid)){
    	sprintf(advaIntf.intfName, "obc0");			
	}
	else{
    	sprintf(advaIntf.intfName, "vlan%d", vid);
	}

    if (NPD_SUCCESS != nam_asic_vlan_get_drv_port_bmp(vid, &untagBmp, &tagBmp)) {
        syslog_ax_intf_err("create advanced-routing interface get vlan %d pbmp error\n", vid);
        return INTERFACE_RETURN_CODE_ERROR;
    }

    /*syslog_ax_intf_dbg("untag[0] %02x ,untag [1] %02x\n",untagBmp.ports[0],untagBmp.ports[1]);*/
    /*syslog_ax_intf_dbg("tag[0] %02x ,tag [1] %02x\n",tagBmp.ports[0],tagBmp.ports[1]);*/

    advaIntf.untagPortBitmap[0] = untagBmp.ports[0];
    advaIntf.untagPortBitmap[1] = untagBmp.ports[1];
    advaIntf.tagPortBitmap[0] = tagBmp.ports[0];
    advaIntf.tagPortBitmap[1] = tagBmp.ports[1];

    syslog_ax_intf_dbg("create vlan %d advanced-routing pbmp untag[%#02x %#02x] tag[%#02x %#02x]\n", \
                       vid, advaIntf.untagPortBitmap[0], advaIntf.untagPortBitmap[1], \
                       advaIntf.tagPortBitmap[0], advaIntf.tagPortBitmap[1]);

    /*ret = ioctl(promi_fd,BM_IOC_REG_INTF_ ,&advaIntf);*/
	if(NPD_INTF_ADV_EN == opCode){
    	ret = cavim_do_intf(CVM_IOC_ADV_EN_INTF_, &advaIntf);
	}
	else{
		ret = cavim_do_intf(CVM_IOC_REG_INTF_, &advaIntf);
	}
    if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
        syslog_ax_intf_err("create vlan %d advanced-routing tell kmod error %d\n", vid, ret);
        return INTERFACE_RETURN_CODE_ERROR;
    }

    /* update advanced-routing interface link status*/
    ret = npd_check_vlan_status(vid);
    if ((VLAN_STATE_UP_E != ret) && (VLAN_STATE_DOWN_E != ret)) {
        ret = INTERFACE_RETURN_CODE_ERROR;
    }
    else if (VLAN_STATE_DOWN_E == ret) {
        syslog_ax_intf_dbg("update vlan %d advanced-routing interface link down\n", vid);
        memset(&advaIntf, 0, sizeof(ve_vlan_param_data_s));
		if((PRODUCT_ID_AX7K_I == PRODUCT_ID)&&\
			((0 == LOCAL_CHASSIS_SLOT_NO)||(1 == LOCAL_CHASSIS_SLOT_NO))&&\
			(NPD_VLAN_OBC0_VLAN_ID == vid)){
        	sprintf(advaIntf.intfName, "obc0");			
		}
		else{
        	sprintf(advaIntf.intfName, "vlan%d", vid);
		}
        advaIntf.vid = vid;

        ret = cavim_do_intf(CVM_IOC_INTF_DOWN, &advaIntf);
        if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
            syslog_ax_eth_port_err("update vlan %d advanced-routing link down tell kmod error %d\n", vid, ret);
            return INTERFACE_RETURN_CODE_ERROR;
        }
    }
    else {
        ret = INTERFACE_RETURN_CODE_SUCCESS;
    }

    return ret;
}

/**********************************************************************************
 *  npd_intf_del_advanced_route_l3intf
 *
 *  DESCRIPTION:
 *      this routine delete cavim dev
 *
 *  INPUT:
 *      vid -- vlan id
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_ERROR
 *      INTERFACE_RETURN_CODE_SUCCESS
 *
 *
 **********************************************************************************/
int npd_intf_del_advanced_route_l3intf
(
    unsigned short vid,
    unsigned int opCode
) {
    int ret = INTERFACE_RETURN_CODE_SUCCESS;
    ve_vlan_param_data_s advaIntf = {0};
    struct DRV_VLAN_PORT_BMP_S untagBmp, tagBmp;

    memset(&untagBmp, 0, sizeof(struct DRV_VLAN_PORT_BMP_S));
    memset(&tagBmp, 0, sizeof(struct DRV_VLAN_PORT_BMP_S));

    advaIntf.vid = vid;
    sprintf(advaIntf.intfName, "vlan%d", vid);

    if (NPD_SUCCESS != nam_asic_vlan_get_drv_port_bmp(vid, &untagBmp, &tagBmp)) {
        syslog_ax_intf_dbg("delete advanced-routing interface get vlan %d pbmp error\n", vid);
        return INTERFACE_RETURN_CODE_ERROR;
    }

    memcpy(advaIntf.untagPortBitmap, untagBmp.ports, sizeof(struct DRV_VLAN_PORT_BMP_S));
    memcpy(advaIntf.tagPortBitmap, tagBmp.ports, sizeof(struct DRV_VLAN_PORT_BMP_S));

    syslog_ax_intf_dbg("delete vlan %d advanced-routing pbmp untag[%#02x %#02x] tag[%#02x %#02x]\n", \
                       vid, advaIntf.untagPortBitmap[0], advaIntf.untagPortBitmap[1], \
                       advaIntf.tagPortBitmap[0], advaIntf.tagPortBitmap[1]);
    /* remove cavium_ip_offload*/
    npd_intf_advanced_route_cvm_kmod_op(0);

	if(NPD_INTF_ADV_DIS == opCode){
        /*ret = ioctl(promi_fd,BM_IOC_UNREG_INTF_ ,&advaIntf);*/
        ret = cavim_do_intf(CVM_IOC_ADV_DIS_INTF_, &advaIntf);
	}
	else{
        /*ret = ioctl(promi_fd,BM_IOC_UNREG_INTF_ ,&advaIntf);*/
        ret = cavim_do_intf(CVM_IOC_UNREG_INTF_, &advaIntf);
	}

    if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
        syslog_ax_intf_err("delete vlan %d advanced-routing tell kmod error %d\n", vid, ret);
        ret = INTERFACE_RETURN_CODE_ERROR;
    }
    /* install cavium_ip_offload*/
    npd_intf_advanced_route_cvm_kmod_op(1);

    return ret;
}


/**********************************************************************************
 *  npd_intf_update_advanced_l3intf
 *
 *  DESCRIPTION:
 *      this routine update advance route intf
 *
 *  INPUT:
 *      vid   -- vlan id
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_ERROR
 *      INTERFACE_RETURN_CODE_SUCCESS
 *
 *
 **********************************************************************************/
int npd_intf_update_advanced_l3intf
(
    unsigned short vid
) {
    int ret = 0;
    ve_vlan_param_data_s advaIntf = {0};
    struct DRV_VLAN_PORT_BMP_S untagBmp, tagBmp;

    memset(&untagBmp, 0, sizeof(struct DRV_VLAN_PORT_BMP_S));
    memset(&tagBmp, 0, sizeof(struct DRV_VLAN_PORT_BMP_S));

    advaIntf.vid = vid;
    sprintf(advaIntf.intfName, "vlan%d", vid);

    if (NPD_SUCCESS != nam_asic_vlan_get_drv_port_bmp(vid, &untagBmp, &tagBmp)) {
        syslog_ax_intf_dbg("update advanced-routing interface get vlan %d pbmp error\n", vid);
        return INTERFACE_RETURN_CODE_ERROR;
    }

    memcpy(advaIntf.untagPortBitmap, untagBmp.ports, sizeof(struct DRV_VLAN_PORT_BMP_S));
    memcpy(advaIntf.tagPortBitmap, tagBmp.ports, sizeof(struct DRV_VLAN_PORT_BMP_S));
    syslog_ax_intf_dbg("update vlan %d advanced-routing pbmp untag[%#02x %#02x] tag[%#02x %#02x]\n", \
                       vid, advaIntf.untagPortBitmap[0], advaIntf.untagPortBitmap[1], \
                       advaIntf.tagPortBitmap[0], advaIntf.tagPortBitmap[1]);

    /*ret = ioctl(promi_fd,BM_IOC_UPDATE_INTF_ ,&advaIntf);*/
    ret = cavim_do_intf(CVM_IOC_UPDATE_INTF_, &advaIntf);
    if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
        syslog_ax_intf_err("update vlan %d advanced-routing tell kmod error %d\n", vid, ret);
        ret = INTERFACE_RETURN_CODE_ERROR;
    }

    return ret;
}

/**********************************************************************************
 *  npd_intf_advanced_route_add_cscd_port
 *
 *  DESCRIPTION:
 *      this routine add cscd  port to advance route intf
 *
 *  INPUT:
 *      vid   -- vlan id
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_NAM_ERROR
 *      INTERFACE_RETURN_CODE_SUCCESS
 *
 *
 **********************************************************************************/
int npd_intf_advanced_route_add_cscd_port
(
    unsigned short vid
) {
    int ret = INTERFACE_RETURN_CODE_SUCCESS;
    enum product_id_e  productId = PRODUCT_ID;
    ret = nam_intf_advanced_route_add_cscd_port(vid, productId);
    return ret;
}


/**********************************************************************************
 *  npd_intf_advanced_route_del_cscd_port
 *
 *  DESCRIPTION:
 *      this routine delete cscd  port from advance route intf
 *
 *  INPUT:
 *      vid   -- vlan id
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_NAM_ERROR
 *      INTERFACE_RETURN_CODE_SUCCESS
 *
 *
 **********************************************************************************/
int npd_intf_advanced_route_del_cscd_port
(
    unsigned int vid
) {
    int ret = INTERFACE_RETURN_CODE_SUCCESS;
    enum product_id_e  productId = PRODUCT_ID;
    ret = nam_intf_advanced_route_del_cscd_port(vid);
    return ret;
}

/**********************************************************************************
 *  npd_intf_advanced_route_enable
 *
 *  DESCRIPTION:
 *      this routine enbale advanced route intf
 *
 *  INPUT:
 *      vid -- vlan id
 *      addr -- MAC
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_ERROR
 *      INTERFACE_RETURN_CODE_SUCCESS
 *
 *
 **********************************************************************************/
int npd_intf_advanced_route_enable
(
    unsigned int vid,
    unsigned char* addr
) {
    int ret = INTERFACE_RETURN_CODE_SUCCESS;
    enum VLAN_PORT_SUBIF_FLAG flag = VLAN_PORT_SUBIF_NOTEXIST;
    unsigned int ifindex = ~0UI;

    ret = npd_vlan_get_interface_ve_flag(vid, &flag);
    if (NPD_SUCCESS == ret && flag == VLAN_PORT_SUBIF_EXIST) {
        ret = INTERFACE_RETURN_CODE_ALREADY_ADVANCED;
    }
    else if ((TRUE == npd_vlan_interface_check(vid, &ifindex)) && ifindex != ~0UI) {
        ret = INTERFACE_RETURN_CODE_INTERFACE_EXIST;
    }
    else if (INTERFACE_RETURN_CODE_SUCCESS != npd_intf_create_advanced_route_l3intf(vid, \
				((PRODUCT_ID_AX7K_I == PRODUCT_ID)&&\
			((0 == LOCAL_CHASSIS_SLOT_NO)||(1 == LOCAL_CHASSIS_SLOT_NO))&&\
			(NPD_VLAN_OBC0_VLAN_ID == vid)) ? \
					NPD_INTF_CREATE_INTF : NPD_INTF_ADV_EN)) {
        ret = INTERFACE_RETURN_CODE_ERROR;
        syslog_ax_intf_dbg("call npd_create_advanced_route_intf error\n");
    }
    else if (INTERFACE_RETURN_CODE_SUCCESS != npd_vlan_set_interface_ve_flag(vid, VLAN_PORT_SUBIF_EXIST)) {
        ret = INTERFACE_RETURN_CODE_ERROR;
        syslog_ax_intf_dbg("call npd_vlan_set_interface_ve_flag error\n");
    }
    else if (((PRODUCT_ID_AX7K_I == PRODUCT_ID) || (PRODUCT_ID_AX7K == PRODUCT_ID)) && (NPD_SUCCESS != nam_static_fdb_entry_mac_set_for_l3(addr, vid, HSC_PORT_VINDEX_AX7))) {
        ret = INTERFACE_RETURN_CODE_ERROR;
        syslog_ax_intf_dbg("create vlan %d advanced-routing on PRODUCT_ID_AX7K, add fdb for l3 error\n", vid);
    }
    else if ((PRODUCT_ID_AX5K == PRODUCT_ID) && (NPD_SUCCESS != nam_static_fdb_entry_mac_set_for_l3(addr, vid, HSC_PORT_VINDEX_AX5))) {
        ret = INTERFACE_RETURN_CODE_ERROR;
        syslog_ax_intf_dbg("create vlan %d advanced-routing on PRODUCT_ID_AX5K, add fdb for l3 error\n", vid);
    }
    else if ((PRODUCT_ID_AX5K_I == PRODUCT_ID) && (NPD_SUCCESS != nam_static_fdb_entry_mac_set_for_l3(addr, vid, HSC_PORT_VINDEX_AX5I))) {
        ret = INTERFACE_RETURN_CODE_ERROR;
        syslog_ax_intf_dbg("create vlan %d advanced-routing on PRODUCT_ID_AX5K_I, add fdb for l3 error\n", vid);
    }
    else if (INTERFACE_RETURN_CODE_SUCCESS != npd_intf_advanced_route_add_cscd_port(vid)) {
        ret = INTERFACE_RETURN_CODE_ERROR;
        syslog_ax_intf_dbg("call npd_advanced_route_add_cscd_port error\n");
    }

    return ret;
}

/**********************************************************************************
 *  npd_intf_advanced_route_disable
 *
 *  DESCRIPTION:
 *      this routine disable advanced route intf
 *
 *  INPUT:
 *      vid -- vlan id
 *      addr -- MAC
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_ERROR
 *      INTERFACE_RETURN_CODE_SUCCESS
 *
 *
 **********************************************************************************/
int npd_intf_advanced_route_disable
(
    unsigned int     vid,
    unsigned char* addr,
    unsigned int opCode
) {
    int ret = INTERFACE_RETURN_CODE_SUCCESS;
    if (INTERFACE_RETURN_CODE_SUCCESS != npd_intf_del_advanced_route_l3intf(vid, opCode)) {
        ret = INTERFACE_RETURN_CODE_ERROR;
        syslog_ax_intf_dbg("call npd_advanced_route_vlan_l3intf_del_subif error\n");
    }
    else if (INTERFACE_RETURN_CODE_SUCCESS != npd_vlan_set_interface_ve_flag(vid, VLAN_PORT_SUBIF_NOTEXIST)) {
        ret = INTERFACE_RETURN_CODE_ERROR;
        syslog_ax_intf_dbg("call npd_vlan_set_interface_ve_flag error\n");
    }
    else if (INTERFACE_RETURN_CODE_SUCCESS != (ret = npd_intf_advanced_route_del_cscd_port(vid))) {
        ret = INTERFACE_RETURN_CODE_ERROR;
        syslog_ax_intf_dbg("call npd_advanced_route_del_cscd_port error\n");
    }
    else if (NPD_SUCCESS != (ret = nam_no_static_fdb_entry_mac_vlan_set(addr, vid))) {
        syslog_ax_intf_dbg("del static mac addr ERROR ret %d\n", ret);
        ret = INTERFACE_RETURN_CODE_ERROR;
    }
    else {
        ret = INTERFACE_RETURN_CODE_SUCCESS;
    }

    return ret;

}

/**********************************************************************************
 *  npd_intf_vlan_set_attr_for_cscd
 *
 *  DESCRIPTION:
 *      this routine set attribute of cscd port added the l3intf vlan
 *
 *  INPUT:
 *      ifindex  -- dev index
 *      devNum
 *      portNum
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_ERROR
 *      INTERFACE_RETURN_CODE_MAC_GET_ERROR
 *      INTERFACE_RETURN_CODE_CHECK_MAC_ERROR
 *      INTERFACE_RETURN_CODE_SUCCESS
 *
 *
 **********************************************************************************/
unsigned int npd_intf_vlan_set_attr_for_cscd
(
    unsigned int  ifindex,
    unsigned char devNum,
    unsigned char portNum
) 
{
    int ret = INTERFACE_RETURN_CODE_SUCCESS;
    unsigned char mac[6] = {0};

    ret = npd_intf_get_intf_mac(ifindex, mac);
    if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
        syslog_ax_intf_err("npd get dev  mac ERROR\n");
        ret = INTERFACE_RETURN_CODE_MAC_GET_ERROR;
    }
    /* Check the legality of mac address,
      * multicast mac , broadcast mac, and all zero mac is in vaild
      * add by jinpc@autelan.com 20090829
      */
    else if (FALSE == npd_intf_l3_mac_legality_check(mac)) {
        syslog_ax_intf_err("the interface mac is illegal\n");
        ret = INTERFACE_RETURN_CODE_CHECK_MAC_ERROR;
    }
    else if (INTERFACE_RETURN_CODE_SUCCESS != nam_cscd_route_on_vlan_enable(devNum, portNum, 1)) {
        syslog_ax_intf_err("npd enable vlan cscd port route error\n");
        ret = INTERFACE_RETURN_CODE_ERROR;
    }

    else if (INTERFACE_RETURN_CODE_SUCCESS != nam_arp_trap_enable(devNum, portNum, 0)) {
        syslog_ax_intf_err("npd enable arp trap cpu error\n");
        ret = INTERFACE_RETURN_CODE_ERROR;
    }

    else if (INTERFACE_RETURN_CODE_SUCCESS !=  npd_intf_config_basemac(devNum, portNum, mac)) {
        syslog_ax_intf_err("npd_l3intf_config_basemac error\n");
        ret = INTERFACE_RETURN_CODE_ERROR;
    }

    return ret;

}

/**********************************************************************************
 *  npd_intf_vlan_set_attr_for_newport
 *
 *  DESCRIPTION:
 *      this routine set attribute of port added the l3intf vlan
 *
 *  INPUT:
 *      ifindex  -- dev index
 *      eth_g_index -- port index
 *      devNum
 *      portNum
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_ERROR
 *      INTERFACE_RETURN_CODE_MAC_GET_ERROR
 *      INTERFACE_RETURN_CODE_CHECK_MAC_ERROR
 *      INTERFACE_RETURN_CODE_SUCCESS
 *
 *
 **********************************************************************************/
unsigned int npd_intf_vlan_set_attr_for_newport
(
    unsigned int    ifindex,
    unsigned int    eth_g_index,
    unsigned char devNum,
    unsigned char portNum
) {
    int ret = INTERFACE_RETURN_CODE_SUCCESS;
    unsigned char mac[6] = {0};

    ret = npd_intf_get_intf_mac(ifindex, mac);
    if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
        syslog_ax_intf_err("npd get dev  mac ERROR\n");
        ret = INTERFACE_RETURN_CODE_MAC_GET_ERROR;
    }
    /* Check the legality of mac address,
      * multicast mac , broadcast mac, and all zero mac is in vaild
      * add by jinpc@autelan.com 20090829
      */
    else if (FALSE == npd_intf_l3_mac_legality_check(mac)) {
        syslog_ax_intf_err("the interface mac is illegal\n");
        ret = INTERFACE_RETURN_CODE_CHECK_MAC_ERROR;
    }
    else if (INTERFACE_RETURN_CODE_SUCCESS != nam_ports_route_on_vlan_enable(eth_g_index, 1)) {
        syslog_ax_intf_err("npd enable vlan port route error\n");
        ret = INTERFACE_RETURN_CODE_ERROR;
    }

    else if (INTERFACE_RETURN_CODE_SUCCESS != nam_arp_trap_enable(devNum, portNum, 0)) {
        syslog_ax_intf_err("npd enable arp trap cpu error\n");
        ret = INTERFACE_RETURN_CODE_ERROR;
    }

    else if (INTERFACE_RETURN_CODE_SUCCESS !=  npd_intf_config_basemac(devNum, portNum, mac)) {
        syslog_ax_intf_err("npd_l3intf_config_basemac error\n");
        ret = INTERFACE_RETURN_CODE_ERROR;
    }

    return ret;

}

/**********************************************************************************
 *  npd_intf_vlan_set_default_attr_for_oldport
 *
 *  DESCRIPTION:
 *      this routine set default attribute for port that deleted from l3intf vlan
 *
 *  INPUT:
 *      eth_g_index -- destination port index
 *      devNum
 *      portNum
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_ERROR
 *      INTERFACE_RETURN_CODE_SUCCESS
 *
 **********************************************************************************/
unsigned int npd_intf_vlan_set_default_attr_for_oldport
(
    unsigned int    eth_g_index,
    unsigned char devNum,
    unsigned char portNum
) {
    int ret = INTERFACE_RETURN_CODE_SUCCESS;

    if (INTERFACE_RETURN_CODE_SUCCESS != nam_ports_route_on_vlan_enable(eth_g_index, 0)) {
        syslog_ax_intf_dbg("npd enable vlan port route error\n");
        ret = INTERFACE_RETURN_CODE_ERROR;
    }
    else if (INTERFACE_RETURN_CODE_SUCCESS != nam_arp_trap_enable(devNum, portNum, 0)) {
        syslog_ax_intf_dbg("npd enable arp trap cpu error\n");
        ret = INTERFACE_RETURN_CODE_ERROR;
    }
    return ret;
}

/**********************************************************************************
 *  npd_intf_update_vlan_info_to_advanced_routing_l3intf
 *
 *  DESCRIPTION:
 *      this routine update ports info to kernel dev
 *
 *  INPUT:
 *      vid -- vlan id
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_ERROR
 *      INTERFACE_RETURN_CODE_SUCCESS
 *
 *
 **********************************************************************************/
unsigned int npd_intf_update_vlan_info_to_advanced_routing_l3intf
(
    unsigned short vid
) {
    int ret = INTERFACE_RETURN_CODE_SUCCESS;
    enum VLAN_PORT_SUBIF_FLAG flag = VLAN_PORT_SUBIF_NOTEXIST;

    ret = npd_vlan_get_interface_ve_flag(vid, &flag);
    if (NPD_SUCCESS == ret && flag == VLAN_PORT_SUBIF_EXIST) {
        npd_intf_update_advanced_l3intf(vid);
        ret = INTERFACE_RETURN_CODE_SUCCESS;
    }
    else {
        ret = INTERFACE_RETURN_CODE_ERROR;
    }

    return ret;
}

/**********************************************************************************
 *  npd_intf_get_route_mode_mac
 *
 *  DESCRIPTION:
 *      this routine get specific mac address for route mode eth-port
 *      as default, L3 eth-port interface in route mode has same system mac, and
 *      value saved in sw is bcast address FF:FF:FF:FF:FF:FF
 *
 *  INPUT:
 *      eth_g_index  -  global eth-port index
 *      ifIndex - L3 interface index
 *
 *  OUTPUT:
 *      macAddr - mac address to set*
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_ERROR - if error occurred
 *      INTERFACE_RETURN_CODE_SUCCESS - if all ok
 *
 *
 **********************************************************************************/
unsigned int npd_intf_get_route_mode_mac
(
    unsigned int eth_g_index,
    unsigned int ifIndex,
    unsigned char *macAddr
) {
    unsigned int result = INTERFACE_RETURN_CODE_SUCCESS;
    unsigned int l3Index = 0;

    result = npd_eth_port_interface_check(eth_g_index, &l3Index);
    if ((TRUE == result) &&
        (~0UI != l3Index) &&
        (ifIndex == l3Index)) {
        result = npd_eth_port_l3intf_mac_get(eth_g_index, macAddr);
        if (NPD_SUCCESS == result) {
            result = INTERFACE_RETURN_CODE_SUCCESS;
        }
        else {
            result = INTERFACE_RETURN_CODE_ERROR;
        }
    }
    else {
        result = INTERFACE_RETURN_CODE_ERROR;
    }
    return result;
}

/**********************************************************************************
 *  npd_intf_set_route_mode_mac
 *
 *  DESCRIPTION:
 *      this routine set specific mac address for route mode eth-port
 *      as default, L3 eth-port interface in route mode has same system mac, and
 *      value saved in sw is bcast address FF:FF:FF:FF:FF:FF
 *
 *  INPUT:
 *      eth_g_index  -  global eth-port index
 *      ifIndex - L3 interface index
 *      macAddr - mac address to set
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_ERROR - if error occurred
 *      INTERFACE_RETURN_CODE_SUCCESS - if all ok
 *
 *
 **********************************************************************************/
unsigned int npd_intf_set_route_mode_mac
(
    unsigned int eth_g_index,
    unsigned int ifIndex,
    unsigned char *macAddr
) {
    unsigned int result = INTERFACE_RETURN_CODE_SUCCESS;
    unsigned int l3Index = 0;

    result = npd_eth_port_interface_check(eth_g_index, &l3Index);
    if ((TRUE == result) &&
        (~0UI != l3Index) &&
        (ifIndex == l3Index)) {
        result = npd_eth_port_l3intf_mac_set(eth_g_index, macAddr);
        if (NPD_SUCCESS == result) {
            result = INTERFACE_RETURN_CODE_SUCCESS;
        }
        else {
            result = INTERFACE_RETURN_CODE_ERROR;
        }
    }
    else {
        result = INTERFACE_RETURN_CODE_ERROR;
    }
    return result;
}

/**********************************************************************************
 *  npd_intf_get_l3_vlan_mac
 *
 *  DESCRIPTION:
 *      this routine get specific mac address for vlan interface
 *      as default, vlan interface has system mac, and
 *      value saved in sw is bcast address FF:FF:FF:FF:FF:FF
 *
 *  INPUT:
 *      vid  -  vlan id
 *      ifIndex - L3 interface index
 *
 *  OUTPUT:
 *      macAddr - mac address got
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_ERROR - if error occurred
 *      INTERFACE_RETURN_CODE_SUCCESS - if all ok
 *
 **********************************************************************************/
unsigned int npd_intf_get_l3_vlan_mac
(
    unsigned short vid,
    unsigned int ifIndex,
    unsigned char *macAddr
) {
    unsigned int result = INTERFACE_RETURN_CODE_SUCCESS;
    unsigned int l3Index = 0;

    result = npd_vlan_interface_check(vid, &l3Index);
    if ((TRUE == result) &&
        (~0UI != l3Index) &&
        (ifIndex == l3Index)) {
        result = npd_vlan_l3intf_get_mac(vid, macAddr);
        if (NPD_SUCCESS == result) {
            result = INTERFACE_RETURN_CODE_SUCCESS;
        }
        else {
            result = INTERFACE_RETURN_CODE_ERROR;
        }
    }
    else {
        result = INTERFACE_RETURN_CODE_ERROR;
    }
    return result;
}

/**********************************************************************************
 *  npd_intf_set_l3_vlan_mac
 *
 *  DESCRIPTION:
 *      this routine set specific mac address for vlan interface
 *      as default, vlan interface has system mac, and
 *      value saved in sw is bcast address FF:FF:FF:FF:FF:FF
 *
 *  INPUT:
 *      vid  -  vlan id
 *      ifIndex - L3 interface index
 *      macAddr - mac address to set
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_ERROR - if error occurred
 *      INTERFACE_RETURN_CODE_SUCCESS - if all ok
 *
 *
 **********************************************************************************/
unsigned int npd_intf_set_l3_vlan_mac
(
    unsigned short vid,
    unsigned int ifIndex,
    unsigned char *macAddr
) {
    unsigned int result = INTERFACE_RETURN_CODE_SUCCESS;
    unsigned int l3Index = 0;

    result = npd_vlan_interface_check(vid, &l3Index);
    if ((TRUE == result) &&
        (~0UI != l3Index) &&
        (ifIndex == l3Index)) {
        result = npd_vlan_l3intf_set_mac(vid, macAddr);
        if (NPD_SUCCESS == result) {
            result = INTERFACE_RETURN_CODE_SUCCESS;
        }
        else {
            result = INTERFACE_RETURN_CODE_ERROR;
        }
    }
    else {
        result = INTERFACE_RETURN_CODE_ERROR;
    }
    return result;
}

/**********************************************************************************
 *  npd_intf_get_promisc_mode_mac
 *
 *  DESCRIPTION:
 *      this routine get specific mac address for promiscuous mode eth-port
 *      as default, L3 eth-port interface in promiscuous mode has same system mac, and
 *      value saved in sw is bcast address FF:FF:FF:FF:FF:FF
 *
 *  INPUT:
 *      eth_g_index  -  global eth-port index
 *      ifIndex - L3 interface index
 *
 *  OUTPUT:
 *      macAddr - mac address to set*
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_SUCCESS
 *
 **********************************************************************************/
unsigned int npd_intf_get_promisc_mode_mac
(
    unsigned int eth_g_index,
    unsigned int ifIndex,
    unsigned char *macAddr
) {
    /* TODO: need implemented*/
    unsigned int result = INTERFACE_RETURN_CODE_SUCCESS;

    return result;
}

/**********************************************************************************
 *  npd_intf_set_promisc_mode_mac
 *
 *  DESCRIPTION:
 *      this routine set specific mac address for promiscuous mode eth-port
 *      as default, L3 eth-port interface in promiscuous mode has same system mac, and
 *      value saved in sw is bcast address FF:FF:FF:FF:FF:FF
 *
 *  INPUT:
 *      eth_g_index  -  global eth-port index
 *      ifIndex - L3 interface index
 *      macAddr - mac address to set
 *
 *  OUTPUT:
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_SUCCESS
 *
 **********************************************************************************/
unsigned int npd_intf_set_promisc_mode_mac
(
    unsigned int eth_g_index,
    unsigned int ifIndex,
    unsigned char *macAddr
) {
    /* TODO: need implemented*/
    unsigned int result = INTERFACE_RETURN_CODE_SUCCESS;

    return result;
}

/**********************************************************************************
 *  npd_intf_get_advanced_route_vlan_mac
 *
 *  DESCRIPTION:
 *      this routine get specific mac address for advanced-route vlan interface
 *      as default, advanced-route vlan interface has system mac, and
 *      value saved in sw is bcast address FF:FF:FF:FF:FF:FF
 *
 *  INPUT:
 *      vid  -  vlan id
 *      ifIndex - L3 interface index
 *
 *  OUTPUT:
 *      macAddr - mac address got
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_SUCCESS
 *
 *
 **********************************************************************************/
unsigned int npd_intf_get_advanced_route_vlan_mac
(
    unsigned short vid,
    unsigned int ifIndex,
    unsigned char *macAddr
) {
    /* TODO: need implemented*/
    unsigned int result = INTERFACE_RETURN_CODE_SUCCESS;

    return result;
}

/**********************************************************************************
 *  npd_intf_set_advanced_route_vlan_mac
 *
 *  DESCRIPTION:
 *      this routine set specific mac address for advanced-route vlan interface
 *      as default, advanced-route vlan interface has system mac, and
 *      value saved in sw is bcast address FF:FF:FF:FF:FF:FF
 *
 *  INPUT:
 *      vid  -  vlan id
 *      ifIndex - L3 interface index
 *      macAddr - mac address to set
 *
 *  OUTPUT:
 *
 *
 *  RETURN:
 *      INTERFACE_RETURN_CODE_SUCCESS
 *
 *
 **********************************************************************************/
unsigned int npd_intf_set_advanced_route_vlan_mac
(
    unsigned short vid,
    unsigned int ifIndex,
    unsigned char *macAddr
) {
    /* TODO: need implemented*/
    unsigned int result = INTERFACE_RETURN_CODE_SUCCESS;

    return result;
}

/**********************************************************************************
 *  npd_intf_init
 *
 *  init global L3 interface structure.
 *
 *  INPUT:
 *      NULL
 *
 *  OUTPUT:
 *      NULL
 *
 *  RETURN:
 *      NULL
 *
 *
 **********************************************************************************/
void npd_intf_init
(
    void
) {
	unsigned char macAddr[6] = {0};
    syslog_ax_intf_dbg("npd intf init...\n");
    /*nam_route_config();*/
	
#if 0
	if(PRODUCT_ID_AX7K_I == PRODUCT_ID){
		int local_slot = LOCAL_CHASSIS_SLOT_NO;	
		if((0 == local_slot) || (1 == local_slot)){
			npd_system_get_basemac(macAddr, 6);
			npd_intf_advanced_route_enable(NPD_VLAN_OBC0_VLAN_ID, macAddr);
			npd_intf_set_vlan_l3intf_status(NPD_VLAN_OBC0_VLAN_ID,VLAN_STATE_UP_E);
		}
	}		
#endif
}

/***********************************************************
 *
 * RETURN:
 *      INTERFACE_RETURN_CODE_SUCCESS
 *      INTERFACE_RETURN_CODE_VLAN_NOTEXIST
 *      INTERFACE_RETURN_CODE_CONTAIN_PROMI_PORT
 *      INTERFACE_RETURN_CODE_MAC_GET_ERROR
 *      INTERFACE_RETURN_CODE_CHECK_MAC_ERROR
 *      INTERFACE_RETURN_CODE_ALREADY_ADVANCED
 *      INTERFACE_RETURN_CODE_INTERFACE_EXIST
 *      INTERFACE_RETURN_CODE_UNSUPPORT_COMMAND
 *      INTERFACE_RETURN_CODE_ERROR
 *
 ***********************************************************/
DBusMessage * npd_dbus_create_intf_by_vid(DBusConnection *conn, DBusMessage *msg, void *user_data) {

    DBusMessage* reply;
    DBusMessageIter  iter;
    DBusError err;

    unsigned int ifindex = ~0UI, ret = INTERFACE_RETURN_CODE_SUCCESS;
    unsigned short vId = 0;
    unsigned char addr[6] = {0};
    unsigned int advanced = 0;
    char* pname = g_l3intf_name;
    unsigned int *eth_g_indexs = NULL;
    unsigned int portCount = 0;
    unsigned int i = 0;
    static unsigned int callTime = 0;

    memset(pname, '\0', 16);
    enum product_id_e productId = PRODUCT_ID;

    dbus_error_init(&err);

    if (!(dbus_message_get_args(msg, &err,
                                DBUS_TYPE_UINT16, &vId,
                                DBUS_TYPE_UINT32, &advanced,
                                DBUS_TYPE_INVALID))) {
        syslog_ax_intf_err("Unable to get input args ");
        if (dbus_error_is_set(&err)) {
            syslog_ax_intf_err("%s raised: %s", err.name, err.message);
            dbus_error_free(&err);
        }
        return NULL;
    }

    sprintf(pname, "vlan%d", vId);
    syslog_ax_intf_dbg("call %s line %d %d times\r\n", __func__, __LINE__, ++callTime);
    syslog_ax_intf_dbg("create interface by vlan %d,name %s\n", vId, pname);
    ret = npd_check_vlan_exist(vId);
    if (ret != VLAN_RETURN_CODE_VLAN_EXISTS) {
        syslog_ax_intf_err("vid %d is bad param\n", vId);
        ifindex = ~0UI;
        ret = INTERFACE_RETURN_CODE_VLAN_NOTEXIST;
    }
    else {
        eth_g_indexs = (unsigned int *)malloc(MAX_ETHPORT_PER_BOARD * CHASSIS_SLOT_COUNT * sizeof(unsigned int));
        if (NULL == eth_g_indexs) {
            syslog_ax_intf_err("get temporary memory alloc error\n");
            return NULL;
        }
        memset(eth_g_indexs, 0, (MAX_ETHPORT_PER_BOARD * CHASSIS_SLOT_COUNT * sizeof(unsigned int)));
        if (!promis_port_add2_intf) {
            npd_vlan_member_port_index_get(vId, FALSE, eth_g_indexs, &portCount);
            for (i = 0; i < portCount; i++) {
                ret = npd_check_port_promi_mode(eth_g_indexs[i]);
                if (TRUE == ret) {
                    ret = INTERFACE_RETURN_CODE_CONTAIN_PROMI_PORT;
                    break;
                }
            }
            if (INTERFACE_RETURN_CODE_CONTAIN_PROMI_PORT != ret) {
                npd_vlan_member_port_index_get(vId, TRUE, eth_g_indexs, &portCount);
                for (i = 0; i < portCount; i++) {
                    ret = npd_check_port_promi_mode(eth_g_indexs[i]);
                    if (TRUE == ret) {
                        ret = INTERFACE_RETURN_CODE_CONTAIN_PROMI_PORT;
                        break;
                    }
                }
#if 0
                if (NPD_INTF_VLAN_CONTAIN_PROMI_PORT != ret) {
                    npd_vlan_member_port_index_get(vId, NPD_TRUE, eth_g_indexs, &portCount);
                    for (i = 0; i < portCount; i++) {
                        ret = npd_check_port_promi_mode(eth_g_indexs[i]);
                        if (NPD_TRUE == ret) {
                            ret = NPD_INTF_VLAN_CONTAIN_PROMI_PORT;
                            break;
                        }
                    }
                }
#endif
            }
        }
        if (INTERFACE_RETURN_CODE_CONTAIN_PROMI_PORT != ret) {
            ret = npd_intf_get_intf_mac(0, addr);
            if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
                syslog_ax_intf_err("npd get system mac ERROR\n");
                ret = INTERFACE_RETURN_CODE_MAC_GET_ERROR;
            }
            else if (((PRODUCT_ID_AX7K == productId) ||
				  (PRODUCT_ID_AX7K_I == productId) ||
                      (PRODUCT_ID_AX5K == productId) ||
                      (PRODUCT_ID_AX5K_I == productId))
                     && (1 == advanced) && (INTERFACE_RETURN_CODE_SUCCESS != (ret = npd_intf_advanced_route_enable(vId, addr)))) {
                syslog_ax_intf_err(" advanced-routing interface create error,ret %d\n", ret);
            }
            /* Check the legality of mac address,
              * multicast mac , broadcast mac, and all zero mac is in vaild
              * add by jinpc@autelan.com 20090829
              */
            else if (TRUE != npd_intf_l3_mac_legality_check(addr)) {
                syslog_ax_intf_err("the interface mac is illegal\n");
                ret = INTERFACE_RETURN_CODE_CHECK_MAC_ERROR;
            }
            else if ((0 == advanced) && (INTERFACE_RETURN_CODE_SUCCESS != (ret = npd_intf_create_vlan_l3intf(vId, pname, addr, NPD_INTF_CREATE_INTF)))) {
                syslog_ax_intf_err("call npd_create_vlan_intf_by_vid ret %d,vid %d,ifindex %d\n", ret , vId, ifindex);
            }
            else if ((0 == advanced) && (INTERFACE_RETURN_CODE_SUCCESS != (ret = npd_intf_set_vlan_attr(vId, addr, 1)))) {
                syslog_ax_intf_err("call npd_set_intf_attr_by_vid err, vid %d,ret %d\n", vId, ret);
            }
            else {
                if ((PRODUCT_ID_AX7K != productId) &&
				(PRODUCT_ID_AX7K_I != productId) &&
                    (PRODUCT_ID_AX5K != productId) &&
                    (PRODUCT_ID_AX5K_I != productId) &&
                    (1 == advanced)) {
                    syslog_ax_intf_err("unsupport this command\n");
                    ret =  INTERFACE_RETURN_CODE_UNSUPPORT_COMMAND;
                }
            }
        }

        free(eth_g_indexs);
        eth_g_indexs = NULL;

    }


    reply = dbus_message_new_method_return(msg);

    dbus_message_iter_init_append(reply, &iter);

    dbus_message_iter_append_basic(&iter,
                                   DBUS_TYPE_UINT32,
                                   &ret);

    dbus_message_iter_append_basic(&iter,
                                   DBUS_TYPE_UINT32,
                                   &ifindex);

    dbus_message_iter_append_basic(&iter,
                                   DBUS_TYPE_STRING,
                                   &pname);

    return reply;
}

/***********************************************************
 *
 * RETURN:
 *      INTERFACE_RETURN_CODE_SUCCESS
 *      INTERFACE_RETURN_CODE_VLAN_NOTEXIST
 *      INTERFACE_RETURN_CODE_CONTAIN_PROMI_PORT
 *      INTERFACE_RETURN_CODE_MAC_GET_ERROR
 *      INTERFACE_RETURN_CODE_CHECK_MAC_ERROR
 *      INTERFACE_RETURN_CODE_ALREADY_ADVANCED
 *      INTERFACE_RETURN_CODE_INTERFACE_EXIST
 *      INTERFACE_RETURN_CODE_UNSUPPORT_COMMAND
 *      INTERFACE_RETURN_CODE_ERROR
 *
 ***********************************************************/

DBusMessage * npd_dbus_create_intf_by_vlan_ifname(DBusConnection *conn, DBusMessage *msg, void *user_data) {

    DBusMessage* reply;
    DBusMessageIter  iter;
    DBusError err;

    unsigned int ifindex = ~0UI, ret = INTERFACE_RETURN_CODE_SUCCESS;
    unsigned short vId = 0;
    unsigned char addr[6] = {0};
    char* pname = g_l3intf_name;
    unsigned int *eth_g_indexs = NULL;
    unsigned int portCount = 0;
    unsigned int i = 0;
    static unsigned int callTime = 0;
    enum VLAN_PORT_SUBIF_FLAG flag = VLAN_PORT_SUBIF_NOTEXIST;

    memset(pname, '\0', 16);
    enum product_id_e productId = PRODUCT_ID;

if(!((productinfo.capbmp) & FUNCTION_VLAN_VLAUE)){
	npd_syslog_dbg("do not support vlan!\n");
	ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
}
else if(((productinfo.capbmp) & FUNCTION_VLAN_VLAUE) \
		&& !((productinfo.runbmp) & FUNCTION_VLAN_VLAUE)){
	npd_syslog_dbg("do not support vlan!\n");
	ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
}
else {
    dbus_error_init(&err);

    if (!(dbus_message_get_args(msg, &err,
                                DBUS_TYPE_UINT16, &vId,
                                DBUS_TYPE_INVALID))) {
        syslog_ax_intf_err("Unable to get input args ");
        if (dbus_error_is_set(&err)) {
            syslog_ax_intf_err("%s raised: %s", err.name, err.message);
            dbus_error_free(&err);
        }
        return NULL;
    }
	if((PRODUCT_ID_AX7K_I == PRODUCT_ID)&&\
			((0 == LOCAL_CHASSIS_SLOT_NO)||(1 == LOCAL_CHASSIS_SLOT_NO))&&\
			(NPD_VLAN_OBC0_VLAN_ID == vId)){
		sprintf(pname, "obc0");
	}
	else{
    	sprintf(pname, "vlan%d", vId);
	}
    syslog_ax_intf_dbg("call %s line %d %d times\r\n", __func__, __LINE__, ++callTime);
    syslog_ax_intf_dbg("create interface by vlan %d,name %s\n", vId, pname);
	
	if((PRODUCT_ID_AX7K_I == PRODUCT_ID)&&\
			((0 == LOCAL_CHASSIS_SLOT_NO)||(1 == LOCAL_CHASSIS_SLOT_NO))&&\
			(NPD_VLAN_OBC0_VLAN_ID == vId)){
		ret = npd_vlan_get_interface_ve_flag(vId, &flag);
		if (NPD_SUCCESS == ret && flag == VLAN_PORT_SUBIF_EXIST) {
			ret = INTERFACE_RETURN_CODE_SUCCESS;
		}
		else{
			ret = INTERFACE_RETURN_CODE_INTERFACE_NOTEXIST;
		}
	}
	else{

	if(PRODUCT_ID_AX5K_E == PRODUCT_ID){/*5612e not support*/
		syslog_ax_product_dbg("this produc do not support vlan interface.\n");
		ret = INTERFACE_RETURN_CODE_UNSUPPORT_COMMAND;
	}
	else {
        ret = npd_check_vlan_exist(vId);
        if (ret != VLAN_RETURN_CODE_VLAN_EXISTS) {
            syslog_ax_intf_err("vid %d is bad param\n", vId);
            ifindex = ~0UI;
            ret = INTERFACE_RETURN_CODE_VLAN_NOTEXIST;
        }
        else if (advanced_routing_default_vid == vId) {
            ret = INTERFACE_RETURN_CODE_ADVAN_VLAN_SET2_INTF;
        }
        else {
            eth_g_indexs = (unsigned int *)malloc(MAX_ETHPORT_PER_BOARD * CHASSIS_SLOT_COUNT * sizeof(unsigned int));
            if (NULL == eth_g_indexs) {
                syslog_ax_intf_err("get temporary memory alloc error\n");
                return NULL;
            }
            memset(eth_g_indexs, 0, (MAX_ETHPORT_PER_BOARD * CHASSIS_SLOT_COUNT * sizeof(unsigned int)));
            if (!promis_port_add2_intf) {
                npd_vlan_member_port_index_get(vId, FALSE, eth_g_indexs, &portCount);
                for (i = 0; i < portCount; i++) {
                    ret = npd_check_port_promi_mode(eth_g_indexs[i]);
                    if (TRUE == ret) {
                        ret = INTERFACE_RETURN_CODE_CONTAIN_PROMI_PORT;
                        break;
                    }
                }
                if (INTERFACE_RETURN_CODE_CONTAIN_PROMI_PORT != ret) {
                    npd_vlan_member_port_index_get(vId, TRUE, eth_g_indexs, &portCount);

                    for (i = 0; i < portCount; i++) {
                        ret = npd_check_port_promi_mode(eth_g_indexs[i]);
                        if (TRUE == ret) {
                            ret = INTERFACE_RETURN_CODE_CONTAIN_PROMI_PORT;
                            break;
                        }
                    }
#if 0
                    if (NPD_INTF_VLAN_CONTAIN_PROMI_PORT != ret) {
                        npd_vlan_member_port_index_get(vId, TRUE, eth_g_indexs, &portCount);
                        for (i = 0; i < portCount; i++) {
                            ret = npd_check_port_promi_mode(eth_g_indexs[i]);
                            if (NPD_TRUE == ret) {
                                ret = NPD_INTF_VLAN_CONTAIN_PROMI_PORT;
                                break;
                            }
                        }
                    }
#endif
                }
            }
            if (INTERFACE_RETURN_CODE_CONTAIN_PROMI_PORT != ret) {
                ret = npd_vlan_get_interface_ve_flag(vId, &flag);
                if (NPD_SUCCESS == ret && flag == VLAN_PORT_SUBIF_EXIST) {
                    ret = INTERFACE_RETURN_CODE_SUCCESS;
                }
                else if ((TRUE == npd_vlan_interface_check(vId, &ifindex)) && ifindex != ~0UI) {
                    ret = INTERFACE_RETURN_CODE_SUCCESS;
                }
                else {
                    ret = npd_intf_get_intf_mac(0, addr);
                    if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
                        syslog_ax_intf_err("npd get system mac ERROR\n");
                        ret = INTERFACE_RETURN_CODE_MAC_GET_ERROR;

                    }
                    /* Check the legality of mac address,
                      * multicast mac , broadcast mac, and all zero mac is in vaild
                      * add by jinpc@autelan.com 20090829
                      */
                    else if (FALSE == npd_intf_l3_mac_legality_check(addr)) {
                        syslog_ax_intf_err("the interface mac is illegal\n");
                        ret = INTERFACE_RETURN_CODE_CHECK_MAC_ERROR;
                    }
                    else if (INTERFACE_RETURN_CODE_SUCCESS != (ret = npd_intf_create_vlan_l3intf(vId, pname, addr, NPD_INTF_CREATE_INTF))) {
                        syslog_ax_intf_err("call npd_create_vlan_intf_by_vid ret %d,vid %d,ifindex %d\n", ret , vId, ifindex);

                    }
                    else if (INTERFACE_RETURN_CODE_SUCCESS != (ret = npd_intf_set_vlan_attr(vId, addr, 1))) {
                        syslog_ax_intf_err("call npd_set_intf_attr_by_vid err, vid %d,ret %d\n", vId, ret);
                    }
                    if (~0UL == ifindex) {
                        npd_vlan_interface_check(vId, &ifindex);
                    }
                }
            }

            free(eth_g_indexs);
            eth_g_indexs = NULL;
        }
	}
}
}


    reply = dbus_message_new_method_return(msg);

    dbus_message_iter_init_append(reply, &iter);

    dbus_message_iter_append_basic(&iter,
                                   DBUS_TYPE_UINT32,
                                   &ret);

    dbus_message_iter_append_basic(&iter,
                                   DBUS_TYPE_UINT32,
                                   &ifindex);

    dbus_message_iter_append_basic(&iter,
                                   DBUS_TYPE_STRING,
                                   &pname);

    return reply;
}
/***********************************************************
 *
 * RETURN:
 *      INTERFACE_RETURN_CODE_SUCCESS
 *      INTERFACE_RETURN_CODE_VLAN_NOTEXIST
 *      INTERFACE_RETURN_CODE_CONTAIN_PROMI_PORT
 *      INTERFACE_RETURN_CODE_MAC_GET_ERROR
 *      INTERFACE_RETURN_CODE_CHECK_MAC_ERROR
 *      INTERFACE_RETURN_CODE_ALREADY_ADVANCED
 *      INTERFACE_RETURN_CODE_INTERFACE_EXIST
 *      INTERFACE_RETURN_CODE_NOT_ADVANCED
 *      INTERFACE_RETURN_CODE_UNSUPPORT_COMMAND
 *      INTERFACE_RETURN_CODE_ERROR
 *
 ***********************************************************/
DBusMessage * npd_dbus_vlan_interface_advanced_routing_enable(DBusConnection *conn, DBusMessage *msg, void *user_data) {

    DBusMessage* reply;
    DBusMessageIter  iter;
    DBusError err;

    unsigned int ifindex = ~0UI, ret = INTERFACE_RETURN_CODE_SUCCESS;
    unsigned short vId = 0;
    unsigned char addr[6] = {0};
    unsigned int advanced = 0;
    char* pname = g_l3intf_name;
    unsigned int *eth_g_indexs = NULL;
    unsigned int portCount = 0;
    unsigned int i = 0;
    static unsigned int callTime = 0;
    enum VLAN_PORT_SUBIF_FLAG flag = VLAN_PORT_SUBIF_NOTEXIST;

    enum product_id_e productId = PRODUCT_ID;

    memset(pname, '\0', 16);

    dbus_error_init(&err);

    if (!(dbus_message_get_args(msg, &err,
                                DBUS_TYPE_UINT16, &vId,
                                DBUS_TYPE_UINT32, &advanced,
                                DBUS_TYPE_INVALID))) {
        syslog_ax_intf_err("Unable to get input args ");
        if (dbus_error_is_set(&err)) {
            syslog_ax_intf_err("%s raised: %s", err.name, err.message);
            dbus_error_free(&err);
        }
        return NULL;
    }

	if((PRODUCT_ID_AX7K_I == PRODUCT_ID)&&\
			((0 == LOCAL_CHASSIS_SLOT_NO)||(1 == LOCAL_CHASSIS_SLOT_NO))&&\
			(NPD_VLAN_OBC0_VLAN_ID == vId)){
    	sprintf(pname, "obc0");
		ret = INTERFACE_RETURN_CODE_UNSUPPORT_COMMAND;
	}
	else{
    	sprintf(pname, "vlan%d", vId);
        syslog_ax_intf_dbg("call %s line %d %d times\r\n", __func__, __LINE__, ++callTime);
        syslog_ax_intf_dbg("create interface by vlan %d,name %s\n", vId, pname);			
	
        ret = npd_check_vlan_exist(vId);
        if (ret != VLAN_RETURN_CODE_VLAN_EXISTS) {
            syslog_ax_intf_err("vid %d is bad param\n", vId);
            ifindex = ~0UI;
            ret = INTERFACE_RETURN_CODE_VLAN_NOTEXIST;
        }
        else {
            eth_g_indexs = (unsigned int *)malloc(MAX_ETHPORT_PER_BOARD * CHASSIS_SLOT_COUNT * sizeof(unsigned int));
            if (NULL == eth_g_indexs) {
                syslog_ax_intf_err("get temporary memory alloc error\n");
                return NULL;
            }
            memset(eth_g_indexs, 0, (MAX_ETHPORT_PER_BOARD * CHASSIS_SLOT_COUNT * sizeof(unsigned int)));
            if (!promis_port_add2_intf) {
                npd_vlan_member_port_index_get(vId, FALSE, eth_g_indexs, &portCount);
                for (i = 0; i < portCount; i++) {
                    ret = npd_check_port_promi_mode(eth_g_indexs[i]);
                    if (TRUE == ret) {
                        ret = INTERFACE_RETURN_CODE_CONTAIN_PROMI_PORT;
                        break;
                    }
                }
#if 0
            if (INTERFACE_RETURN_CODE_CONTAIN_PROMI_PORT != ret) {
                npd_vlan_member_port_index_get(vId, TRUE, eth_g_indexs, &portCount);
                for (i = 0; i < portCount; i++) {
                    ret = npd_check_port_promi_mode(eth_g_indexs[i]);
                    if (NPD_TRUE == ret) {
                        ret = INTERFACE_RETURN_CODE_CONTAIN_PROMI_PORT;
                        break;
                    }
                }
            }
#endif
            }
            if (INTERFACE_RETURN_CODE_CONTAIN_PROMI_PORT != ret) {
                ret = npd_intf_get_intf_mac(0, addr);
                if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
                    syslog_ax_intf_err("npd get system mac ERROR\n");
                    ret = INTERFACE_RETURN_CODE_MAC_GET_ERROR;
                }
                /* Check the legality of mac address,
                  * multicast mac , broadcast mac, and all zero mac is in vaild
                  * add by jinpc@autelan.com 20090829
                  */
                else if (FALSE == npd_intf_l3_mac_legality_check(addr)) {
                    syslog_ax_intf_err("the interface mac is illegal\n");
                    ret = INTERFACE_RETURN_CODE_CHECK_MAC_ERROR;
                }
                else if (npd_product_adv_routing_cmd_support()
                         && (1 == advanced)) {
                    ret = npd_vlan_get_interface_ve_flag(vId, &flag);
                    if (NPD_SUCCESS == ret && flag == VLAN_PORT_SUBIF_EXIST) {
                        ret = INTERFACE_RETURN_CODE_ALREADY_ADVANCED;
                    }
                    else {
                        ret = npd_vlan_interface_check(vId, &ifindex);
                        if ((TRUE == ret) && (~0UI != ifindex)) { /*del vlan l3intf*/
                            ret = npd_intf_del_vlan_l3intf(vId, ifindex, addr, NPD_INTF_ADV_EN);
                            if (INTERFACE_RETURN_CODE_SUCCESS == ret) {
                                ret = npd_intf_set_vlan_attr(vId, addr, 0);
                            }
                        }
                        else {
                            ret = INTERFACE_RETURN_CODE_ERROR;
                        }
                        if (INTERFACE_RETURN_CODE_SUCCESS == ret) {
                            ret = npd_intf_advanced_route_enable(vId, addr);
                        }
                    }
                }
                else if (0 == advanced) {
                    ret = npd_vlan_get_interface_ve_flag(vId, &flag);
                    if (NPD_SUCCESS == ret && flag == VLAN_PORT_SUBIF_EXIST) {
                        ret = npd_intf_advanced_route_disable(vId, addr, NPD_INTF_ADV_DIS);/* del advanced-routing intf*/
                    }
                    else {
                        ret = INTERFACE_RETURN_CODE_NOT_ADVANCED;
                    }
                    if (INTERFACE_RETURN_CODE_SUCCESS == ret) {
                        ret = npd_intf_create_vlan_l3intf(vId, pname, addr, NPD_INTF_ADV_DIS);
                        if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
                            syslog_ax_intf_err("call npd_intf_create_vlan_l3intf ret %d,vid %d,ifindex %d\n", ret , vId, ifindex);
                        }
                        else {
                            ret = npd_intf_set_vlan_attr(vId, addr, 1);
                            if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
                                syslog_ax_intf_err("call npd_intf_set_vlan_attr err, vid %d,ret %d\n", vId, ret);
                            }
                        }
                    }
                }
                else {
                    if ((PRODUCT_ID_AX7K != productId) &&
					(PRODUCT_ID_AX7K_I != productId) &&
                        (PRODUCT_ID_AX5K != productId) &&
                        (PRODUCT_ID_AX5K_I != productId) &&
                        (1 == advanced)) {
                        syslog_ax_intf_err("unsupport this command\n");
                        ret =  INTERFACE_RETURN_CODE_UNSUPPORT_COMMAND;
                    }
                }
            }

            free(eth_g_indexs);
            eth_g_indexs = NULL;
        }
	}


    reply = dbus_message_new_method_return(msg);

    dbus_message_iter_init_append(reply, &iter);

    dbus_message_iter_append_basic(&iter,
                                   DBUS_TYPE_UINT32,
                                   &ret);

    dbus_message_iter_append_basic(&iter,
                                   DBUS_TYPE_UINT32,
                                   &ifindex);

    dbus_message_iter_append_basic(&iter,
                                   DBUS_TYPE_STRING,
                                   &pname);

    return reply;
}

/*************************************************************
 *
 * RETURN:
 *      INTERFACE_RETURN_CODE_SUCCESS
 *      INTERFACE_RETURN_CODE_MAC_GET_ERROR
 *      INTERFACE_RETURN_CODE_CHECK_MAC_ERROR
 *      INTERFACE_RETURN_CODE_VLAN_NOTEXIST
 *      INTERFACE_RETURN_CODE_INTERFACE_NOTEXIST
 *      INTERFACE_RETURN_CODE_ERROR
 *
 *************************************************************/
DBusMessage * npd_dbus_del_intf_by_vid(DBusConnection *conn, DBusMessage *msg, void *user_data) {

    DBusMessage* reply;
    DBusMessageIter  iter;
    DBusError err;

    unsigned int ret = INTERFACE_RETURN_CODE_SUCCESS;
    unsigned int ifindex = ~0UI;
    unsigned short vId = 0;
    unsigned char addr[6] = {0};
    enum VLAN_PORT_SUBIF_FLAG flag = VLAN_PORT_SUBIF_NOTEXIST;

    syslog_ax_intf_dbg("npd_dbus_del_intf_by_vId!\n");

    dbus_error_init(&err);

    if (!(dbus_message_get_args(msg, &err,
                                DBUS_TYPE_UINT16, &vId,
                                DBUS_TYPE_INVALID))) {
        syslog_ax_intf_err("Unable to get input args ");
        if (dbus_error_is_set(&err)) {
            syslog_ax_intf_err("%s raised: %s", err.name, err.message);
            dbus_error_free(&err);
        }
        return NULL;
    }
    else {
#if 1
        /*code added by zhengcs@autelan.com at 20081204 for read mac according interface--ifIndex*/
        ret = npd_intf_get_intf_mac(ifindex, addr);/*modified by zhengcs,from 0 ->ifindex*/
        if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
            syslog_ax_intf_dbg("npd get dev  mac ERROR\n");
            ret = INTERFACE_RETURN_CODE_MAC_GET_ERROR;
        }
#endif
        /* Check the legality of mac address,
          * multicast mac , broadcast mac, and all zero mac is in vaild
          * add by jinpc@autelan.com 20090829
          */
        else if (FALSE == npd_intf_l3_mac_legality_check(addr)) {
            syslog_ax_intf_err("the interface mac is illegal\n");
            ret = INTERFACE_RETURN_CODE_CHECK_MAC_ERROR;
        }
        else {
            ret = INTERFACE_RETURN_CODE_SUCCESS;
        }
        if (INTERFACE_RETURN_CODE_SUCCESS == ret) {
            ret = npd_vlan_get_interface_ve_flag(vId, &flag);
            if (NPD_SUCCESS == ret && flag == VLAN_PORT_SUBIF_EXIST) {
                ret = npd_intf_advanced_route_disable(vId, addr, NPD_INTF_DEL_INTF);
            }
            else if ((TRUE == (ret = npd_vlan_interface_check(vId, &ifindex))) && \
                     (ifindex != (~0UI))) {
                ret = npd_intf_del_vlan_l3intf(vId, ifindex, addr, NPD_INTF_DEL_INTF);
                if (INTERFACE_RETURN_CODE_SUCCESS == ret) {
                    ret = npd_intf_set_vlan_attr(vId, addr, 0);
                }
            }
            else {
                ret  = INTERFACE_RETURN_CODE_INTERFACE_NOTEXIST;
            }
        }

    }


    reply = dbus_message_new_method_return(msg);

    dbus_message_iter_init_append(reply, &iter);

    dbus_message_iter_append_basic(&iter,
                                   DBUS_TYPE_UINT32,
                                   &ret);

    dbus_message_iter_append_basic(&iter,
                                   DBUS_TYPE_UINT32,
                                   &ifindex);

    return reply;
}


/*******************************************************
 *
 * OUTPUT:
 *      COMMON_RETURN_CODE_NO_SUCH_PORT
 *      INTERFACE_RETURN_CODE_VLAN_NOTEXIST
 *      INTERFACE_RETURN_CODE_SUBIF_EXISTS
 *      INTERFACE_RETURN_CODE_ADD_PORT_FAILED
 *      INTERFACE_RETURN_CODE_SUBIF_CREATE_FAILED
 *      INTERFACE_RETURN_CODE_ROUTE_CREATE_SUBIF
 *      INTERFACE_RETURN_CODE_VLAN_IF_CREATE_SUBIF
 *      INTERFACE_RETURN_CODE_INTERFACE_NOTEXIST
 *
 ********************************************************/
DBusMessage * npd_dbus_create_sub_intf(DBusConnection *conn, DBusMessage *msg, void *user_data) {

    DBusMessage* reply;
    DBusMessageIter  iter;
    DBusError err;

    unsigned int ret = INTERFACE_RETURN_CODE_SUCCESS;
    unsigned int vid = 0, vid2 = 0;
    unsigned int eth_g_index = 0, ifindex = ~0UI;
    unsigned char slot = 0, local_port_no = 0;
    unsigned char cpu_no = 0, cpu_port_no = 0;	
    unsigned int isTag = 0;
    unsigned int rc = FALSE;
    unsigned int    linkState = ETH_ATTR_LINKDOWN;
    unsigned char tagMode = FALSE;
    unsigned char pname[21] = {0};
    struct eth_port_s portInfo;
	int ifnameType = 0;
	unsigned char * ifnamePrefix = NULL;
	unsigned int ifIndex = ~0UI;
	unsigned char ifname[MAX_IFNAME_LEN + 1] = {0};
	unsigned char file_path[64] = {0};
	int function_type = -1;

    dbus_error_init(&err);

    if (!(dbus_message_get_args(msg, &err,
                                DBUS_TYPE_UINT32, &ifnameType,
                                DBUS_TYPE_BYTE, &slot,
                                DBUS_TYPE_BYTE, &local_port_no,
                                DBUS_TYPE_BYTE, &cpu_no,
                                DBUS_TYPE_BYTE, &cpu_port_no,
                                DBUS_TYPE_UINT32, &vid,
                                DBUS_TYPE_UINT32, &vid2, /*for qinq subif*/
                                DBUS_TYPE_INVALID))) {
        syslog_ax_intf_err("Unable to get input args ");
        if (dbus_error_is_set(&err)) {
            syslog_ax_intf_err("%s raised: %s", err.name, err.message);
            dbus_error_free(&err);
        }
        return NULL;
    }
	
	switch(ifnameType)
	{
    case CONFIG_INTF_ETH:
		ifnamePrefix = "eth";
		break;
	case CONFIG_INTF_E:
	    ifnamePrefix = "e";
		break;
	case CONFIG_INTF_CSCD:
		ifnamePrefix = "cscd";
		break;
	case CONFIG_INTF_OBC:
		ifnamePrefix = "obc0";
		break;
	case CONFIG_INTF_VE:
		ifnamePrefix = "ve";
		break;
	default :
		return COMMON_RETURN_CODE_BADPARAM;
	}

	if(PRODUCT_ID == PRODUCT_ID_AX7K_I || SYSTEM_TYPE == IS_DISTRIBUTED)
	{
        if (0 == vid) {
            ret = COMMON_RETURN_CODE_BADPARAM;
        }
		
		ret = INTERFACE_RETURN_CODE_NO_SUCH_PORT;		
		if(ifnameType == CONFIG_INTF_VE)
		{
			if (CHASSIS_SLOTNO_ISLEGAL(slot)) 
			{  
			    ret = INTERFACE_RETURN_CODE_SUCCESS;
				sprintf(file_path,"/dbm/product/slot/slot%d/function_type", slot);
				function_type = get_num_from_file(file_path);
				
				if(function_type&AC_BOARD)
				{
					if(vid2 != 0) {
						sprintf(pname,"%s%02d%c%d.%d",ifnamePrefix, slot,(cpu_no==1)?'f':'s',cpu_port_no, vid);
						sprintf(ifname,"%s.%d", pname, vid2);
                        syslog_ax_intf_dbg("Create QinQ %s\n",ifname);
					}else {
                        sprintf(pname,"%s%02d%c%d",ifnamePrefix, slot,(cpu_no==1)?'f':'s',cpu_port_no);
						sprintf(ifname,"%s.%d", pname, vid);
						if(PRODUCT_ID == PRODUCT_ID_AX7K_I)
						{							
							if(cpu_port_no!=1)
							{
								ret = INTERFACE_RETURN_CODE_PARENT_IF_NOTEXIST;
								goto Done;
							}
							else    /* the parent on AX71-CRSMU is obc0 */
							{
							    sprintf(pname, "obc0");
							}
						}
                        syslog_ax_intf_dbg("Create Subif %s\n",ifname);
					}
					
                    npd_intf_ifindex_get_by_ifname(pname, &ifIndex);
					syslog_ax_intf_dbg("Parent interface %s ifindex %d\n", pname, ifIndex);
					if(!ifIndex) {
                        ret = INTERFACE_RETURN_CODE_PARENT_IF_NOTEXIST;
					}else {
                        ret = npd_check_vlan_exist(vid);
                        if (ret != VLAN_RETURN_CODE_VLAN_EXISTS) {
                            syslog_ax_intf_dbg("create subif found vlan %d invalid\n", vid);
                            ret = INTERFACE_RETURN_CODE_VLAN_NOTEXIST;
                        }else {
                            ret = INTERFACE_RETURN_CODE_SUCCESS;
    						npd_intf_ifindex_get_by_ifname(ifname, &ifIndex);
							syslog_ax_intf_dbg("Interface %s ifindex %d\n", ifname, ifIndex);
            				if(!ifIndex)
                    		{
                                rc = npd_intf_create_ve_subif(ifnameType, slot, cpu_no, cpu_port_no, vid, vid2);
    							syslog_ax_intf_dbg("npd_intf_create_ve_subif ret: %d\n",rc);
                                if(INTERFACE_RETURN_CODE_SUCCESS == rc){
                                    /*zhangcl add for bug CHINAMOBILE-368  modified the default outer tag to 0x8100*/
									ret = npd_intf_set_intf_qinq_type(ifname,"0x8100");
									if(ret != INTERFACE_RETURN_CODE_SUCCESS)
									{
										syslog_ax_intf_err("set intf qinq type err !\n");
									}
									ret = INTERFACE_RETURN_CODE_SUCCESS;
                    			}else{
                    				ret = INTERFACE_RETURN_CODE_SUBIF_CREATE_FAILED;
                                }
            				}
                        }
					}
				}
				else
				{
                    ret = INTERFACE_RETURN_CODE_UNSUPPORT_COMMAND;
				}
			}   
		}
	}
    else
	{
        if (0 == vid) {
            ret = COMMON_RETURN_CODE_BADPARAM;
        }
        /* tag1 equals to tag2 is allowed
        else if(vid == vid2){
            ret = INTERFACE_RETURN_CODE_QINQ_TWO_SAME_TAG;
        }
        */
        
        else {
            if (0 == vid2) {
                syslog_ax_intf_dbg("create subif %s%d-%d.%d\n",ifnamePrefix, slot, local_port_no, vid);
            }
            else if ((0 != vid) && (0 != vid2)) {
                syslog_ax_intf_dbg("create subif %s%d-%d.%d.%d\n", ifnamePrefix,slot, local_port_no, vid, vid2);
            }

            ret = INTERFACE_RETURN_CODE_NO_SUCH_PORT;
            if (CHASSIS_SLOTNO_ISLEGAL(slot)) {
                if (ETH_LOCAL_PORTNO_ISLEGAL(slot, local_port_no)) {
                    eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot, local_port_no);
                    ret = INTERFACE_RETURN_CODE_SUCCESS;
                }
            }
            if ((INTERFACE_RETURN_CODE_NO_SUCH_PORT == ret) && \
                (PRODUCT_ID != PRODUCT_ID_AX7K ) && \
                (PRODUCT_ID != PRODUCT_ID_AX7K_I) && \
    			(PRODUCT_ID != PRODUCT_ID_AX5K_E) && \
    			(PRODUCT_ID != PRODUCT_ID_AX5608) && \
                (0 == slot)) {
                eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot, local_port_no);
                ret = INTERFACE_RETURN_CODE_SUCCESS;
            }

            if (INTERFACE_RETURN_CODE_SUCCESS == ret) {
                ret = npd_check_vlan_exist(vid);
                if (((ret != VLAN_RETURN_CODE_VLAN_EXISTS) ||
                     ((0 != vid2) && (VLAN_RETURN_CODE_VLAN_EXISTS != npd_check_vlan_exist(vid2)))) && \
                    (TRUE != npd_eth_port_rgmii_type_check(slot, local_port_no))) {
                    syslog_ax_intf_dbg("create subif found vlan %d invalid\n", vid);
                    ret = INTERFACE_RETURN_CODE_VLAN_NOTEXIST;
                }
                /*else if((!npd_vlan_check_contain_port(vid,eth_g_index,(unsigned char *)&isTag)) && (0 != slot)) {
                        //  ret = NPD_VLAN_PORT_NOTEXISTS;
                        //}*/
                else if ((TRUE != npd_eth_port_rgmii_type_check(slot, local_port_no))&&\
                        ((advanced_routing_default_vid == vid) || \
                         (advanced_routing_default_vid == vid2))) {
                    ret = INTERFACE_RETURN_CODE_TAG_IS_ADVACED_VID;
                }
                else if (((0 == vid2) && (TRUE == npd_eth_port_check_promi_subif(eth_g_index, vid))) ||
                         ((0 != vid2) && (npd_eth_port_check_qinq_subif(eth_g_index, vid, vid2)))) {
                    ret = INTERFACE_RETURN_CODE_SUBIF_EXISTS;					
    				sprintf(ifname,"%s%d-%d.%d",ifnamePrefix,slot,local_port_no,vid);
    				if(vid2){
    					sprintf(ifname,"%s.%d",ifname,vid2);
    				}
    				if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(ifname,&ifIndex)){
    					ret = INTERFACE_RETURN_CODE_INTERFACE_NOTEXIST;
    				}
                }
                else if ((0 != vid2) && (TRUE != npd_eth_port_check_promi_subif(eth_g_index, vid))) {
                    ret = INTERFACE_RETURN_CODE_PARENT_IF_NOTEXIST;
                }
                else if (TRUE == npd_check_port_promi_mode(eth_g_index)) {
                    ret = INTERFACE_RETURN_CODE_SUCCESS;
    				sprintf(ifname,"eth%d-%d",slot,local_port_no);
    				if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(ifname,&ifIndex)){
    					ret = INTERFACE_RETURN_CODE_PARENT_IF_NOTEXIST;
    				}
                    else if (TRUE != npd_eth_port_rgmii_type_check(slot, local_port_no)) {
                        if (TRUE != npd_vlan_check_contain_port(vid, eth_g_index, &tagMode)) {
                            ret = npd_vlan_tag_port_add_for_subif(vid, eth_g_index);
                            if (VLAN_RETURN_CODE_ERR_NONE == ret) {
                                ret = INTERFACE_RETURN_CODE_SUCCESS;
                            }
                            else {
                                ret = INTERFACE_RETURN_CODE_ADD_PORT_FAILED;
                            }
                        }
                        if ((0 != vid2) && (TRUE != npd_vlan_check_contain_port(vid2, eth_g_index, &tagMode))) {
                            ret = npd_vlan_tag_port_add_for_subif(vid2, eth_g_index);
                            if (VLAN_RETURN_CODE_ERR_NONE == ret) {
                                ret = INTERFACE_RETURN_CODE_SUCCESS;
                            }
                            else {
                                ret = INTERFACE_RETURN_CODE_ADD_PORT_FAILED;
                            }
                        }
                    }
                    else {
                        ret = INTERFACE_RETURN_CODE_SUCCESS;
                    }
                    if (INTERFACE_RETURN_CODE_SUCCESS == ret) {
                        if (TRUE == npd_eth_port_rgmii_type_check(slot, local_port_no)) {
                            linkState = ETH_ATTR_LINKUP;
                        }
                        else {
                            npd_get_port_link_status(eth_g_index, &linkState);
                        }
                        if (0 != vid2) {
                            sprintf(pname, "%s%d-%d.%d",ifnamePrefix, slot, local_port_no, vid);
                            rc = npd_intf_create_subif(ifnameType,slot, local_port_no, vid, vid2, linkState);
    						if(INTERFACE_RETURN_CODE_SUCCESS == rc){
                                if (NPD_SUCCESS != (ret = npd_eth_port_create_qinq_subif(eth_g_index, vid, vid2))) {
                                    npd_intf_del_subif(pname, vid2);
                                    if (TRUE != npd_eth_port_check_tag_for_other_subif(eth_g_index, vid, vid2)) {
                                        npd_vlan_tag_port_del_for_subif(vid2, eth_g_index);
                                    }
                                    ret = INTERFACE_RETURN_CODE_SUBIF_CREATE_FAILED;
                                }
                                else {
                                    ret = INTERFACE_RETURN_CODE_SUCCESS;
                                }
    						}
    						else{
    							ret = INTERFACE_RETURN_CODE_SUBIF_CREATE_FAILED;
    						}
                            syslog_ax_intf_dbg("create qinq subif result %d\n", rc);
                        }
                        else {
                            sprintf(pname, "%s%d-%d",ifnamePrefix, slot, local_port_no);
                            rc = npd_intf_create_subif(ifnameType,slot, local_port_no, vid, 0, linkState);
    						if(INTERFACE_RETURN_CODE_SUCCESS == rc){
                            if (INTERFACE_RETURN_CODE_SUCCESS != (ret = npd_eth_port_create_promi_subif(eth_g_index, vid))) {
                                npd_intf_del_subif(pname, vid);
                                if (TRUE != npd_eth_port_check_tag_for_other_subif(eth_g_index, vid, vid2)) {
                                    npd_vlan_tag_port_del_for_subif(vid, eth_g_index);
                                }
                                ret = INTERFACE_RETURN_CODE_SUBIF_CREATE_FAILED;
                            }
                            else {
                                ret = INTERFACE_RETURN_CODE_SUCCESS;
                                }
    						}
    						else{
    							ret = INTERFACE_RETURN_CODE_SUBIF_CREATE_FAILED;
                            }
                            syslog_ax_intf_dbg("create subif result %d\n", rc);
                        }

                    }
                }
                else if (npd_eth_port_interface_check(eth_g_index, &ifindex)      \
                         && (ifindex != ~0UI)) {
                    syslog_ax_intf_err("create port %#x subif on vlan %d not allowed for route mode interface\n", \
                                       eth_g_index, vid);
                    ret = INTERFACE_RETURN_CODE_ROUTE_CREATE_SUBIF;
                }
                else if ((TRUE == npd_vlan_interface_check(vid, &ifindex)) && (ifindex != ~0UI)) {
                    syslog_ax_intf_err("create port %#x subif on vlan %d not allowed for vlan interface\n", \
                                       eth_g_index, vid);
                    ret = INTERFACE_RETURN_CODE_VLAN_IF_CREATE_SUBIF;
                }
                else
                    ret = INTERFACE_RETURN_CODE_PARENT_IF_NOTEXIST;
            }
        }
	}

Done:	
    reply = dbus_message_new_method_return(msg);
    dbus_message_iter_init_append(reply, &iter);
    dbus_message_iter_append_basic(&iter,
                                   DBUS_TYPE_UINT32,
                                   &ret);

    return reply;
}
/*********************************************************
 *
 * RETURN:
 *      COMMON_RETURN_CODE_NO_SUCH_PORT
 *      INTERFACE_RETURN_CODE_INTERFACE_NOTEXIST
 *      INTERFACE_RETURN_CODE_PORT_NOT_IN_VLAN
 *      INTERFACE_RETURN_CODE_SUBIF_NOTEXIST
 *      INTERFACE_RETURN_CODE_ROUTE_CREATE_SUBIF
 *      INTERFACE_RETURN_CODE_VLAN_IF_CREATE_SUBIF
 *      INTERFACE_RETURN_CODE_INTERFACE_NOTEXIST
 *      INTERFACE_RETURN_CODE_ERROR
 *      INTERFACE_RETURN_CODE_SUCCESS
 *
 *********************************************************/
DBusMessage * npd_dbus_del_sub_intf(DBusConnection *conn, DBusMessage *msg, void *user_data) {

    DBusMessage* reply;
    DBusMessageIter  iter;
    DBusError err;

    unsigned int ret = INTERFACE_RETURN_CODE_SUCCESS;
    unsigned int vid = 0, vid2 = 0, eth_g_index = 0, ifindex = ~0UI;
    unsigned char slot = 0, local_port_no = 0;
    unsigned char cpu_no = 0, cpu_port_no = 0;	
    unsigned int isTag = 0;
    unsigned int rc = FALSE;
    unsigned char pname[21] = {0};
	int ifnameType = 0;
	unsigned char * ifnamePrefix = NULL;
	unsigned int ifIndex = ~0UI;
	unsigned char ifname[MAX_IFNAME_LEN + 1] = {0};
	unsigned char file_path[64] = {0};
	int function_type = -1;
	
    syslog_ax_intf_dbg("npd_dbus_del_sub_intf_by_vId!\n");

    dbus_error_init(&err);

    if (!(dbus_message_get_args(msg, &err,
                                DBUS_TYPE_UINT32, &ifnameType,
                                DBUS_TYPE_BYTE, &slot,
                                DBUS_TYPE_BYTE, &local_port_no,
                                DBUS_TYPE_BYTE, &cpu_no,
                                DBUS_TYPE_BYTE, &cpu_port_no,                                  
                                DBUS_TYPE_UINT32, &vid,
                                DBUS_TYPE_UINT32, &vid2,
                                DBUS_TYPE_INVALID))) {
        syslog_ax_intf_err("Unable to get input args ");
        if (dbus_error_is_set(&err)) {
            syslog_ax_intf_err("%s raised: %s", err.name, err.message);
            dbus_error_free(&err);
        }
        return NULL;
    }
	switch(ifnameType)
	{
    case CONFIG_INTF_ETH:
		ifnamePrefix = "eth";
		break;
	case CONFIG_INTF_E:
	    ifnamePrefix = "e";
		break;
	case CONFIG_INTF_CSCD:
		ifnamePrefix = "cscd";
		break;
	case CONFIG_INTF_OBC:
		ifnamePrefix = "obc0";
		break;
	case CONFIG_INTF_VE:
		ifnamePrefix = "ve";
		break;
	default :
		return COMMON_RETURN_CODE_BADPARAM;
	}

	if(PRODUCT_ID == PRODUCT_ID_AX7K_I || SYSTEM_TYPE == IS_DISTRIBUTED)
	{
		ret = INTERFACE_RETURN_CODE_NO_SUCH_PORT;
	    if(ifnameType == CONFIG_INTF_VE)
		{
			if (CHASSIS_SLOTNO_ISLEGAL(slot)) {  
			     
				sprintf(file_path,"/dbm/product/slot/slot%d/function_type", slot);
				function_type = get_num_from_file(file_path);
				
				if(function_type&AC_BOARD)
				{
					ret = INTERFACE_RETURN_CODE_SUCCESS;
				}
				else
				{
                    ret = INTERFACE_RETURN_CODE_UNSUPPORT_COMMAND;
				}	 
			}
			
			if(vid2 != 0) {
				/*rintf(pname,"%s%d.%d",ifnamePrefix, slot, vid);*/
				sprintf(pname,"%s%02d%c%d.%d",ifnamePrefix, slot,(cpu_no==1)?'f':'s',cpu_port_no, vid);				
				sprintf(ifname,"%s.%d", pname, vid2);
                syslog_ax_intf_dbg("Delete QinQ %s\n",ifname);
				vid = vid2; /* set vid2 the arg for npd_intf_del_subif(). zhangdi@autelan.com 2012-06-27 */
			}else {
                /*rintf(pname,"%s%d",ifnamePrefix, slot);*/
                sprintf(pname,"%s%02d%c%d",ifnamePrefix, slot,(cpu_no==1)?'f':'s',cpu_port_no);				
				sprintf(ifname,"%s.%d", pname, vid);
                syslog_ax_intf_dbg("Delete Subif %s\n",ifname);
			}
			
    		npd_intf_ifindex_get_by_ifname(ifname,&ifIndex);
            if (INTERFACE_RETURN_CODE_SUCCESS == ret) 
    		{
    			if(ifIndex > 0)
    			{
                    rc = npd_intf_del_subif(pname, vid);
                    if(INTERFACE_RETURN_CODE_SUCCESS == rc){
                        ret = INTERFACE_RETURN_CODE_SUCCESS;
        			}else{
        				ret = INTERFACE_RETURN_CODE_SUBIF_CREATE_FAILED;
                    }
    			}
            }
	    }
	}
	else
	{
	    sprintf(ifname,"%s%d-%d.%d",ifnamePrefix,slot,local_port_no,vid);
    	if(vid2){
    		sprintf(ifname,"%s.%d",ifname,vid2);
	    }
	
        syslog_ax_intf_dbg("destroy subif %s\n",ifname);
    	
        ret = INTERFACE_RETURN_CODE_NO_SUCH_PORT;
        if (CHASSIS_SLOTNO_ISLEGAL(slot)) {
            if (ETH_LOCAL_PORTNO_ISLEGAL(slot, local_port_no)) {
                eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot, local_port_no);
                ret = INTERFACE_RETURN_CODE_SUCCESS;
            }
        }
        if ((INTERFACE_RETURN_CODE_NO_SUCH_PORT == ret) && \
            (PRODUCT_ID_AX7K != PRODUCT_ID) && \
            (PRODUCT_ID != PRODUCT_ID_AX7K_I) && \
    		(PRODUCT_ID != PRODUCT_ID_AX5K_E) && \
    		(PRODUCT_ID != PRODUCT_ID_AX5608) && \
            (0 == slot)) {
            eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot, local_port_no);
            ret = INTERFACE_RETURN_CODE_SUCCESS;
        }

    	if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(ifname,&ifIndex)){
    		ret = INTERFACE_RETURN_CODE_INTERFACE_NOTEXIST;
    	}

        if (INTERFACE_RETURN_CODE_SUCCESS == ret) {
            ret = npd_check_vlan_exist(vid);
            if (((ret != VLAN_RETURN_CODE_VLAN_EXISTS) ||
                 ((0 != vid2) && (VLAN_RETURN_CODE_VLAN_EXISTS != npd_check_vlan_exist(vid2)))) && \
                (TRUE != npd_eth_port_rgmii_type_check(slot, local_port_no))) {
                syslog_ax_intf_dbg("destroy subif found vlan %d invalid\n", vid);
                ret = INTERFACE_RETURN_CODE_VLAN_NOTEXIST;
            }
            else if ((TRUE != npd_eth_port_check_promi_subif(eth_g_index, vid)) ||
                     ((0 != vid2) && (TRUE != npd_eth_port_check_qinq_subif(eth_g_index, vid, vid2)))) {
                ret = INTERFACE_RETURN_CODE_SUBIF_NOTEXIST;
            }
            else if ((TRUE != npd_eth_port_rgmii_type_check(slot, local_port_no)) && \
                     ((!npd_vlan_check_contain_port((unsigned short)vid, eth_g_index, (unsigned char *)&isTag)) ||
                      ((0 != vid2) && (!npd_vlan_check_contain_port((unsigned short)vid2, eth_g_index, (unsigned char *)&isTag))))) {
                ret = INTERFACE_RETURN_CODE_PORT_NOT_IN_VLAN;
            }
            else if (npd_check_port_promi_mode(eth_g_index)) {
                if (0 != vid2) {
                    sprintf(pname, "%s%d-%d.%d", ifnamePrefix,slot, local_port_no, vid);
                    if (INTERFACE_RETURN_CODE_SUCCESS == (ret = npd_eth_port_del_qinq_subif(eth_g_index, vid, vid2))) {
                        rc = npd_intf_del_subif(pname, vid2);
    					if(INTERFACE_RETURN_CODE_SUCCESS == rc){
                        if (TRUE != npd_eth_port_rgmii_type_check(slot, local_port_no)) {
                            if (TRUE != npd_eth_port_check_tag_for_other_subif(eth_g_index, vid, vid2)) {
                                npd_vlan_tag_port_del_for_subif(vid2, eth_g_index);
                            }
                        }
                        	ret = INTERFACE_RETURN_CODE_SUCCESS;
    					}
    					else{
    						ret = INTERFACE_RETURN_CODE_ERROR;
    					}
                        syslog_ax_intf_dbg("del qinq subif result %d\n", rc);
                    }
                }
                else {
                    sprintf(pname, "%s%d-%d", ifnamePrefix,slot, local_port_no);
                    if (INTERFACE_RETURN_CODE_SUCCESS == (ret = npd_eth_port_del_promi_subif(eth_g_index, vid))) {
                        rc = npd_intf_del_subif(pname, vid);
    					if(INTERFACE_RETURN_CODE_SUCCESS == rc){
                        if (TRUE != npd_eth_port_rgmii_type_check(slot, local_port_no)) {
                            if (TRUE != npd_eth_port_check_tag_for_other_subif(eth_g_index, vid, vid2)) {
                                npd_vlan_tag_port_del_for_subif(vid, eth_g_index);
                            }
                        }
    						ret = INTERFACE_RETURN_CODE_SUCCESS;
    					}
    					else{
    						ret = INTERFACE_RETURN_CODE_ERROR;
    					}
                        syslog_ax_intf_dbg("del subif result %d\n", rc);
                    }
                }
            }
            else
                ret = INTERFACE_RETURN_CODE_PARENT_IF_NOTEXIST;
        }
	}


    reply = dbus_message_new_method_return(msg);
    dbus_message_iter_init_append(reply, &iter);
    dbus_message_iter_append_basic(&iter,
                                   DBUS_TYPE_UINT32,
                                   &ret);


    return reply;
}
/***********************************************
 *
 * OUTPUT:
 *      INTERFACE_RETURN_CODE_ERROR
 *      INTERFACE_RETURN_CODE_INTERFACE_NOTEXIST
 *      INTERFACE_RETURN_CODE_GET_SYSMAC_ERROR
 *      INTERFACE_RETURN_CODE_SUCCESS
 *
 ************************************************/
DBusMessage * npd_dbus_advanced_route(DBusConnection *conn, DBusMessage *msg, void *user_data) {

    DBusMessage* reply;
    DBusMessageIter  iter;
    DBusError err;

    unsigned int ret = INTERFACE_RETURN_CODE_SUCCESS;
    unsigned int vid = 0, ifindex = ~0UI;
    unsigned int enable = 0;
    unsigned char addr[MAC_ADDR_LEN] = {0};



    syslog_ax_intf_dbg("npd_dbus_del_intf_by_vId!\n");

    dbus_error_init(&err);

    if (!(dbus_message_get_args(msg, &err,
                                DBUS_TYPE_UINT32, &enable,
                                DBUS_TYPE_UINT32, &vid,
                                DBUS_TYPE_INVALID))) {
        syslog_ax_intf_err("Unable to get input args ");
        if (dbus_error_is_set(&err)) {
            syslog_ax_intf_err("%s raised: %s", err.name, err.message);
            dbus_error_free(&err);
        }
        return NULL;
    }

    syslog_ax_intf_dbg("advanced route vid %d enable %s\n", vid, enable ? "enable" : "disable");

    if (INVALID_L3_INTF_VID == vid)
        ret = INTERFACE_RETURN_CODE_ERROR;
    else if (TRUE != npd_vlan_interface_check(vid, &ifindex))
        ret = INTERFACE_RETURN_CODE_INTERFACE_NOTEXIST;
    else if (ifindex == ~0UI)
        ret = INTERFACE_RETURN_CODE_ERROR;
    else {
        ret = npd_system_get_basemac(addr, MAC_ADDR_LEN);
        if (0 != ret) {
            syslog_ax_intf_err("get system mac error when set advanced route %s \n", enable ? "enable" : "disable");
            ret = INTERFACE_RETURN_CODE_GET_SYSMAC_ERROR;
        }
        else {
            if (enable)
                ret = npd_intf_advanced_route_enable(vid, addr);
            else
                ret = npd_intf_advanced_route_disable(vid, addr, NPD_INTF_ADV_DIS);
        }
    }

    reply = dbus_message_new_method_return(msg);
    dbus_message_iter_init_append(reply, &iter);
    dbus_message_iter_append_basic(&iter,
                                   DBUS_TYPE_UINT32,
                                   &ret);
    return reply;

}

/***************************************
 *
 * RETURN:
 *      INTERFACE_RETURN_CODE_SUCCESS
 *
 ***************************************/
unsigned int npd_promis_port_add2_vlan_intf
(
    unsigned int permit
) {
    promis_port_add2_intf = (permit != 0);
    return INTERFACE_RETURN_CODE_SUCCESS;
}
/*************************************************
  * npd_intf_netlink_get_ip_addrs
 *
 *DESCRIPTION:
 *    get ip addresses by netlink
 *
 *INPUT:
 *    ifName -- the interface name we want to get its ip
 *OUTPUT:
 *    ifIndex -- the interface ifIndex
 *    ipAddrs -- the pointer of ip addresses we got
 *    masks  -- the pointer of masks for ip address above
 *RETURN:
 *    INTERFACE_RETURN_CODE_ERROR -- get ip addresses FAILED
 *    INTERFACE_RETURN_CODE_SUCCESS -- get ip address success
 *NOTE:
 *
 ************************************************/
int npd_intf_netlink_get_ip_addrs
(
    unsigned char* ifName,
    unsigned int* ifIndex,
    unsigned int* ipAddrs,
    unsigned int* masks
) {
    int fd = -1;
    int status = 0;
    int err = 0;
    int len = 0;
    int i = 0;
    int ret = INTERFACE_RETURN_CODE_SUCCESS;
    int sndbuf = 32768;
    int rcvbuf = 32768;
    int seq = time(NULL);
    static char buf[16384] = {0};
    char abuf[256] = {0};
    socklen_t addr_len;
    struct nlmsghdr *h = NULL;
    struct ifaddrmsg *ifa = NULL;
    struct rtattr * rta_tb[IFA_MAX+1] = {NULL};
    struct rtattr *rta = NULL;
    struct sockaddr_nl  local;
    struct sockaddr_nl nladdr;
    struct sockaddr_nl nladdr2;
    struct iovec iov;
    struct msghdr msg = {
        .msg_name = &nladdr,
        .msg_namelen = sizeof(struct sockaddr_nl),
        .msg_iov = &iov,
        .msg_iovlen = 1,
    };
    struct {
        struct nlmsghdr nlh;
        struct rtgenmsg g;
    } req;
    memset(&nladdr, 0, sizeof(struct sockaddr_nl));
    memset(&iov, 0, sizeof(struct iovec));
    if ((NULL == ifName) || (NULL == ifIndex) || \
        (NULL == ipAddrs) || (NULL == masks)) {
        return INTERFACE_RETURN_CODE_ERROR;
    }
    ret = npd_intf_ifindex_get_by_ifname(ifName, ifIndex);
    if (INTERFACE_RETURN_CODE_SUCCESS != ret) {
        return INTERFACE_RETURN_CODE_ERROR;
    }
    for (i = 0; i < MAX_IP_COUNT; i++) {
        ipAddrs[i] = INVALID_HOST_IP;
        masks[i] = 0;
    }
    iov.iov_base = buf;
    iov.iov_len = sizeof(buf);
    h = (struct nlmsghdr*)buf;
    fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (fd < 0) {
        syslog_ax_intf_err(" socket error %d \n", fd);
        return INTERFACE_RETURN_CODE_ERROR;
    }
    if ((ret = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf))) < 0) {
        syslog_ax_intf_err("setsocketopt SO_SNDBUF error %d \n", ret);
        close(fd);
        return INTERFACE_RETURN_CODE_ERROR;
    }

    if ((ret = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf))) < 0) {
        syslog_ax_intf_err("setsockopt SO_RCVBUF error %d\n", ret);
        close(fd);
        return INTERFACE_RETURN_CODE_ERROR;
    }

    memset(&local, 0, sizeof(local));
    local.nl_family = AF_NETLINK;
    local.nl_groups = RTMGRP_IPV4_IFADDR;

    if (bind(fd, (struct sockaddr*)&local, sizeof(local)) < 0) {
        syslog_ax_intf_err("Cannot bind netlink socket\n");
        close(fd);
        return INTERFACE_RETURN_CODE_ERROR;
    }
    addr_len = sizeof(local);
    if (getsockname(fd, (struct sockaddr*)&local, &addr_len) < 0) {
        syslog_ax_intf_err("Cannot getsockname");
        close(fd);
        return INTERFACE_RETURN_CODE_ERROR;
    }
    if (addr_len != sizeof(local)) {
        syslog_ax_intf_err("Wrong address length %d\n", addr_len);
        close(fd);
        return INTERFACE_RETURN_CODE_ERROR;
    }
    if (local.nl_family != AF_NETLINK) {
        syslog_ax_intf_err("Wrong address family %d\n", local.nl_family);
        close(fd);
        return INTERFACE_RETURN_CODE_ERROR;
    }

    memset(&nladdr2, 0, sizeof(nladdr2));
    nladdr2.nl_family = AF_NETLINK;

    memset(&req, 0, sizeof(req));
    req.nlh.nlmsg_len = sizeof(req);
    req.nlh.nlmsg_type = RTM_GETADDR;
    req.nlh.nlmsg_flags = NLM_F_ROOT | NLM_F_MATCH | NLM_F_REQUEST;
    req.nlh.nlmsg_pid = 0;
    req.nlh.nlmsg_seq = ++seq;
    req.g.rtgen_family = AF_NETLINK;

    if ((ret = sendto(fd, (void*) & req, sizeof(req), 0,
                      (struct sockaddr*) & nladdr2, sizeof(nladdr2))) < 0) {
        syslog_ax_intf_err("Send request error %d\n", ret);
        close(fd);
        return INTERFACE_RETURN_CODE_ERROR;
    }
    status = recvmsg(fd, &msg, 0);
    if (status < 0) {
        syslog_ax_intf_err("get msg error when get ip address,status %d \n", status);
        close(fd);
        return INTERFACE_RETURN_CODE_ERROR;
    }

    if (status == 0) {
        syslog_ax_intf_err("EOF on netlink\n");
        close(fd);
        return INTERFACE_RETURN_CODE_ERROR;
    }
    i = 0;
    syslog_ax_intf_dbg("npd_intf_get_intf_ip_addrs::get if %#0x ip address:\n", *ifIndex);
    while ((NLMSG_OK(h, status)) && (i < MAX_IP_COUNT)) {
        if ((h->nlmsg_type != RTM_NEWADDR) && (h->nlmsg_type != RTM_DELADDR)) {
            close(fd);
            return INTERFACE_RETURN_CODE_SUCCESS;
        }
        if ((len = (h->nlmsg_len - NLMSG_LENGTH(sizeof(*ifa)))) < 0) {
            syslog_ax_intf_err("wrong nlmsg len %d\n", len);
        }
        ifa = NLMSG_DATA(h);
        if (*ifIndex == ifa->ifa_index) {
            memset(rta_tb, 0, sizeof(struct rtattr *) *(IFA_MAX + 1));
            rta = IFA_RTA(ifa);
            while (RTA_OK(rta, len)) {
                if (rta->rta_type <= IFA_MAX) {
                    rta_tb[rta->rta_type] = rta;
                }
                rta = RTA_NEXT(rta, len);
            }
            ipAddrs[i] = (unsigned int)(*((unsigned int *)RTA_DATA(rta_tb[IFA_LOCAL])));
            ipAddrs[i] = htonl(ipAddrs[i]);
            masks[i] = (unsigned int)(-(1 << (32 - ifa->ifa_prefixlen)));
            masks[i] = htonl(masks[i]);
            if (INVALID_HOST_IP != ipAddrs[i]) {
                syslog_ax_intf_dbg("\t%d.%d.%d.%d %d.%d.%d.%d\n",  \
                                   (ipAddrs[i] >> 24)&0xFF, (ipAddrs[i] >> 16)&0xFF, (ipAddrs[i] >> 8)&0xFF, (ipAddrs[i])&0xFF, \
                                   (masks[i] >> 24)&0xFF, (masks[i] >> 16)&0xFF, (masks[i] >> 8)&0xFF, (masks[i])&0xFF);
                i++;
            }
        }
        h = NLMSG_NEXT(h, status);
    }
    close(fd);
    return INTERFACE_RETURN_CODE_SUCCESS;
}


/****************************************************
 *
 *RETURN:
 *      INTERFACE_RETURN_CODE_ERROR -error occured
 *      INTERFACE_RETURN_CODE_SUCCESS - ifindex get successed
 *
 ****************************************************/
unsigned int npd_intf_ifindex_get_by_ifname
(
    unsigned char *ifName,
    unsigned int *ifIndex
) {
	int ret = INTERFACE_RETURN_CODE_SUCCESS;
	
	if(!(*ifIndex = if_nametoindex(ifName))){
		ret = INTERFACE_RETURN_CODE_ERROR;
	} 
	
	#if 0
    int fd = -1;
    int status = 0;
    int err = 0;
    int len = 0;
    int i = 0;
    		
    int sndbuf = 32768;
    int rcvbuf = 32768;
    int seq = time(NULL);
    static char buf[16384] = {0};
    char abuf[256] = {0};
    char tmpIfName[21] = {0};
    socklen_t addr_len;
    struct nlmsghdr *h = NULL;
    struct rtattr * rta_tb[IFLA_MAX+1] = {NULL};
    struct rtattr *rta = NULL;
    struct ifinfomsg *ifi = NULL;
    struct sockaddr_nl  local;
    struct sockaddr_nl nladdr;
    struct sockaddr_nl nladdr2;
    struct iovec iov;
    struct msghdr msg = {
        .msg_name = &nladdr,
        .msg_namelen = sizeof(struct sockaddr_nl),
        .msg_iov = &iov,
        .msg_iovlen = 1,
    };
    struct {
        struct nlmsghdr nlh;
        struct rtgenmsg g;
    } req;        
    memset(&nladdr, 0, sizeof(struct sockaddr_nl));
    memset(&iov, 0, sizeof(struct iovec));
    if ((NULL == ifName) || (NULL == ifIndex)) {
        syslog_ax_intf_err("ifName or inIndex null pointer error when get ifindex ty ifname\n");
        return INTERFACE_RETURN_CODE_ERROR;
    }
    *ifIndex = ~0UI;
    iov.iov_base = buf;
    iov.iov_len = sizeof(buf);
    h = (struct nlmsghdr*)buf;
    fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (fd < 0) {
        syslog_ax_intf_err(" socket error %d \n", fd);
        return INTERFACE_RETURN_CODE_ERROR;
    }
    if ((ret = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf))) < 0) {
        syslog_ax_intf_err("setsocketopt SO_SNDBUF error %d \n", ret);
        close(fd);
        return INTERFACE_RETURN_CODE_ERROR;
    }

    if ((ret = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf))) < 0) {
        syslog_ax_intf_err("setsockopt SO_RCVBUF error %d\n", ret);
        close(fd);
        return INTERFACE_RETURN_CODE_ERROR;
    }

    memset(&local, 0, sizeof(local));
    local.nl_family = AF_NETLINK;
    local.nl_groups = RTMGRP_IPV4_IFADDR;

    if (bind(fd, (struct sockaddr*)&local, sizeof(local)) < 0) {
        syslog_ax_intf_err("Cannot bind netlink socket\n");
        close(fd);
        return INTERFACE_RETURN_CODE_ERROR;
    }
    addr_len = sizeof(local);
    if (getsockname(fd, (struct sockaddr*)&local, &addr_len) < 0) {
        syslog_ax_intf_err("Cannot getsockname");
        close(fd);
        return INTERFACE_RETURN_CODE_ERROR;
    }
    if (addr_len != sizeof(local)) {
        syslog_ax_intf_err("Wrong address length %d\n", addr_len);
        close(fd);
        return INTERFACE_RETURN_CODE_ERROR;
    }
    if (local.nl_family != AF_NETLINK) {
        syslog_ax_intf_err("Wrong address family %d\n", local.nl_family);
        close(fd);
        return INTERFACE_RETURN_CODE_ERROR;
    }

    memset(&nladdr2, 0, sizeof(nladdr2));
    nladdr2.nl_family = AF_NETLINK;

    memset(&req, 0, sizeof(req));
    req.nlh.nlmsg_len = sizeof(req);
    req.nlh.nlmsg_type = RTM_GETLINK;
    req.nlh.nlmsg_flags = NLM_F_ROOT | NLM_F_MATCH | NLM_F_REQUEST;
    req.nlh.nlmsg_pid = 0;
    req.nlh.nlmsg_seq = ++seq;
    req.g.rtgen_family = AF_UNSPEC;

    if ((ret = sendto(fd, (void*) & req, sizeof(req), 0,
                      (struct sockaddr*) & nladdr2, sizeof(nladdr2))) < 0) {
        syslog_ax_intf_err("Send request error %d\n", ret);
        close(fd);
        return INTERFACE_RETURN_CODE_ERROR;
    }
    status = recvmsg(fd, &msg, 0);
    if (status < 0) {
        syslog_ax_intf_err("get msg error when find kernal arp,status %d \n", status);
        close(fd);
        return INTERFACE_RETURN_CODE_ERROR;
    }

    if (status == 0) {
        syslog_ax_intf_err("EOF on netlink\n");
        close(fd);
        return INTERFACE_RETURN_CODE_ERROR;
    }
    i = 0;
    while (NLMSG_OK(h, status)) {
        if (h->nlmsg_type != RTM_NEWLINK) {
            syslog_ax_intf_err("type is not RTM_NEWLINK,type: %d\n",h->nlmsg_type);
            close(fd);
            return INTERFACE_RETURN_CODE_ERROR;
        }
        if ((len = (h->nlmsg_len - NLMSG_LENGTH(sizeof(*ifi)))) < 0) {
            syslog_ax_intf_err("wrong nlmsg len %d\n", len);
        }
        ifi = NLMSG_DATA(h);
        memset(rta_tb, 0, sizeof(struct rtattr *) *(IFLA_MAX + 1));
        rta = IFLA_RTA(ifi);
        while (RTA_OK(rta, len)) {
            if (rta->rta_type <= NDA_MAX) {
                rta_tb[rta->rta_type] = rta;
            }
            rta = RTA_NEXT(rta, len);
        }
        if (RTA_DATA(rta_tb[IFLA_IFNAME])) {
            strcpy(tmpIfName, RTA_DATA(rta_tb[IFLA_IFNAME]));
            if (0 == strcmp((char*)ifName, tmpIfName)) {
                *ifIndex = ifi->ifi_index;
                syslog_ax_intf_dbg("get the ifindex %#0x ifname %s \n", *ifIndex, ifName);
                close(fd);
                return INTERFACE_RETURN_CODE_SUCCESS;
            }
        }
        h = NLMSG_NEXT(h, status);
    }
    syslog_ax_intf_dbg("not get the ifindex in kernal \n");
    close(fd);
    return INTERFACE_RETURN_CODE_ERROR;
	#endif
	return ret;
}
/************************************************************
 *npd_intf_get_l3_intf_mac
 *
 *DISCRIPTION:
 *      get interface mac by ifindex and vid or eth_g_index
 *INPUT:
 *      ifIndex - interface index
 *      vid - vlan id
 *      eth_g_index - eth port global index
 *OUTPUT:
 *      macAddr - mac address
 *RETURN:
 *      INTERFACE_RETURN_CODE_ERROR -error occured
 *      INTERFACE_RETURN_CODE_SUCCESS - get mac successed
 *NOTE:
 *
 *
 **************************************************************/
unsigned int npd_intf_get_l3_intf_mac
(
    unsigned int ifIndex,
    unsigned short vid,
    unsigned int eth_g_index,
    unsigned char * macAddr
) {
    unsigned int tmpIfIndex = ~0UI;
    unsigned int flag = 0;
    unsigned char slot_no = 0;
    unsigned char port_no = 0;
    unsigned char ifname[MAX_IFNAME_LEN + 1] = {0};
    unsigned char zeroMac [MAC_ADDR_LEN] = {0};
    unsigned char bcMac [MAC_ADDR_LEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    unsigned char defaultMac [MAC_ADDR_LEN] = {0x00, 0x0d, 0xef, 0x00, 0x0d, 0xef};

    if ((0 != vid) && (0xfff != vid) && (TRUE == npd_vlan_interface_check(vid, &tmpIfIndex)) && \
        (~0UI != tmpIfIndex)) {
        if (ifIndex == tmpIfIndex) { /*  vlan interface */
            if (INTERFACE_RETURN_CODE_SUCCESS == npd_intf_get_intf_mac(ifIndex, macAddr)) {
                if ((0 == memcmp(macAddr, bcMac, MAC_ADDR_LEN)) || \
                    (0 == memcmp(macAddr, zeroMac, MAC_ADDR_LEN)) || \
                    (0 == memcmp(macAddr, defaultMac, MAC_ADDR_LEN))) {
                    npd_system_get_basemac(macAddr, MAC_ADDR_LEN);
                }
                return INTERFACE_RETURN_CODE_SUCCESS;
            }
            else {
                syslog_ax_intf_err("vlan intf get mac FAILLED \n");

            }
        }
        else {
            syslog_ax_intf_err("vlan intf ifIndex %d not matched %d", tmpIfIndex, ifIndex);
        }
    }
    else if ((0 != vid) && (0xfff != vid) && \
             (NPD_SUCCESS == npd_vlan_get_interface_ve_flag(vid, &flag)) && \
             (flag == VLAN_PORT_SUBIF_EXIST)) {
        sprintf(ifname, "vlan%d", vid);
        if ((INTERFACE_RETURN_CODE_SUCCESS == npd_intf_ifindex_get_by_ifname(ifname, &tmpIfIndex)) && \
            (~0UI != tmpIfIndex) && (ifIndex == tmpIfIndex)) { /* vlan advanced-routing interface*/
            if (INTERFACE_RETURN_CODE_SUCCESS == npd_intf_get_l3_vlan_mac(vid, ifIndex, macAddr)) {
                if ((0 == memcmp(macAddr, bcMac, MAC_ADDR_LEN)) || \
                    (0 == memcmp(macAddr, zeroMac, MAC_ADDR_LEN)) || \
                    (0 == memcmp(macAddr, defaultMac, MAC_ADDR_LEN))) {
                    npd_system_get_basemac(macAddr, MAC_ADDR_LEN);
                }
                return INTERFACE_RETURN_CODE_SUCCESS;
            }
            else {
                syslog_ax_intf_err("advanced-routing %s get mac FAILED \n", ifname);
            }

        }
        else {
            syslog_ax_intf_err("advanced-routing %s ifindex %d not matched %d \n", \
                               ifname, tmpIfIndex, ifIndex);
        }

    }
    if ((TRUE == npd_eth_port_interface_check(eth_g_index, &tmpIfIndex)) && \
        (~0UI != tmpIfIndex)) {
        if (ifIndex == tmpIfIndex) { /* port interface */
            if (INTERFACE_RETURN_CODE_SUCCESS == npd_intf_get_intf_mac(ifIndex, macAddr)) {
                if ((0 == memcmp(macAddr, bcMac, MAC_ADDR_LEN)) || \
                    (0 == memcmp(macAddr, zeroMac, MAC_ADDR_LEN)) || \
                    (0 == memcmp(macAddr, defaultMac, MAC_ADDR_LEN))) {
                    npd_system_get_basemac(macAddr, MAC_ADDR_LEN);
                }

                return INTERFACE_RETURN_CODE_SUCCESS;
            }
            else {
                syslog_ax_intf_err("port intf get mac FAILED \n");
            }
        }
        else {
            syslog_ax_intf_err("port intf ifIndex %d not matched %d\n", tmpIfIndex, ifIndex);
        }
    }
    else if (TRUE == npd_check_port_promi_mode(eth_g_index)) {
        if (NPD_SUCCESS == npd_eth_port_get_slotno_portno_by_eth_g_index(eth_g_index, &slot_no, &port_no)) {
            sprintf(ifname, "eth%d-%d", slot_no, port_no);
            if ((INTERFACE_RETURN_CODE_SUCCESS == npd_intf_ifindex_get_by_ifname(ifname, &tmpIfIndex)) && \
                (~0UI != tmpIfIndex) && (ifIndex == tmpIfIndex)) { /* port advaced-routing interface */
                if (NPD_SUCCESS == npd_eth_port_l3intf_mac_get((unsigned short)eth_g_index, macAddr)) {
                    if ((0 == memcmp(macAddr, bcMac, MAC_ADDR_LEN)) || \
                        (0 == memcmp(macAddr, zeroMac, MAC_ADDR_LEN)) || \
                        (0 == memcmp(macAddr, defaultMac, MAC_ADDR_LEN))) {
                        npd_system_get_basemac(macAddr, MAC_ADDR_LEN);
                    }
                    return INTERFACE_RETURN_CODE_SUCCESS;
                }
                else {
                    syslog_ax_intf_err("advanced-routing intf %s get mac FAILED\n", ifname);
                }
            }
            else {
				memset(ifname,0,(MAX_IFNAME_LEN + 1));
				sprintf(ifname, "e%d-%d", slot_no, port_no);
                if ((INTERFACE_RETURN_CODE_SUCCESS == npd_intf_ifindex_get_by_ifname(ifname, &tmpIfIndex)) && \
                    (~0UI != tmpIfIndex) && (ifIndex == tmpIfIndex)) { /* port advaced-routing interface */
                    if (NPD_SUCCESS == npd_eth_port_l3intf_mac_get((unsigned short)eth_g_index, macAddr)) {
                        if ((0 == memcmp(macAddr, bcMac, MAC_ADDR_LEN)) || \
                            (0 == memcmp(macAddr, zeroMac, MAC_ADDR_LEN)) || \
                            (0 == memcmp(macAddr, defaultMac, MAC_ADDR_LEN))) {
                            npd_system_get_basemac(macAddr, MAC_ADDR_LEN);
                        }
                        return INTERFACE_RETURN_CODE_SUCCESS;
                    }
                    else {
                        syslog_ax_intf_err("advanced-routing intf %s get mac FAILED\n", ifname);
                    }
                }
                syslog_ax_intf_err("advanced-routing intf %s ifindex %d not matched %d\n", \
                                   ifname, tmpIfIndex, ifIndex);
            }
        }
    }
    /*else if for sub interface*/
    else {
        syslog_ax_intf_err("interface dose not exist,ifIndex %d vid %d eth_g_index %d \n", ifIndex, vid, eth_g_index);
    }

    return INTERFACE_RETURN_CODE_MAC_GET_ERROR;
}

/*******************************************************
 * DISCRIPTION:
 *      show interface's advanced-routing
 *
 * INPUT:
 *      flag : uint32  - the flag indicates the interface is a vlan interface
 *                      or a eth-port interface
 *                      0 - vlan interface
 *                      1 - eth-port interface
 *                          or sub interface(when the vid is not 0)
 *                      other value - bad parameter
 *      slot_no : uint32  - the slot no when the interface is a eth-port or sub interface(flag is 1)
 *      port_no : uint32 - the port no when the interface is a eth-port or sub interface(flag is 1)
 *      vid : uint32  - the vlan id (flag is 0),or the sub interface's flag (when the parameter flag is 1)
 *
 * OUTPUT:
 *      ret : uint32 - the return code indicates error occured ro not
 *                  values:
 *                      COMMON_RETURN_CODE_BADPARAM
 *                      INTERFACE_RETURN_CODE_VLAN_NOTEXIST
 *                      INTERFACE_RETURN_CODE_UNSUPPORT_COMMAND
 *                      INTERFACE_RETURN_CODE_SUCCESS  - successed
 *
 *      isEnable : uint32 - whether the interface is advaced-routing or not
 *                          avalidate only when the ret is INTERFACE_RETURN_CODE_SUCCESS
 *                      TRUE - the interface is advanced-routing
 *                      FALSE - the interface is not advanced-routing
 *
 * RETURN:
 *      NULL - dbus error occured
 *      reply - success
 *
 *******************************************************/

DBusMessage * npd_dbus_vlan_eth_port_interface_advanced_routing_show(DBusConnection *conn, DBusMessage *msg, void *user_data) {
    DBusMessage* reply;
    DBusMessageIter  iter;
    DBusError err;

    unsigned int flag = 2;
    unsigned int slot_no = 0;
    unsigned int port_no = 0;
    unsigned int eth_g_index = ~0UI;
    unsigned int vid = 0;
    unsigned int ret = INTERFACE_RETURN_CODE_SUCCESS;
    unsigned int isEnable = FALSE;

    if(!((productinfo.capbmp) & FUNCTION_INTERFACE_VLAUE)){
    	npd_syslog_dbg("do not support interface!\n");
    	ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
    }
    else if(((productinfo.capbmp) & FUNCTION_INTERFACE_VLAUE) \
    		&& !((productinfo.runbmp) & FUNCTION_INTERFACE_VLAUE)){
    	npd_syslog_dbg("do not support interface!\n");
    	ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
    }
    else {
    dbus_error_init(&err);
    if (!(dbus_message_get_args(msg, &err,
                                DBUS_TYPE_UINT32, &flag,
                                DBUS_TYPE_UINT32, &slot_no,
                                DBUS_TYPE_UINT32, &port_no,
                                DBUS_TYPE_UINT32, &vid,
                                DBUS_TYPE_INVALID))) {
        syslog_ax_intf_err("Unable to get input args ");
        if (dbus_error_is_set(&err)) {
            syslog_ax_intf_err("%s raised: %s", err.name, err.message);
            dbus_error_free(&err);
        }
        return NULL;
    }
    if (0 == flag) {
        if (vid >= NPD_MAX_VLAN_ID) {
            ret = COMMON_RETURN_CODE_BADPARAM;
        }
        if (VLAN_RETURN_CODE_VLAN_EXISTS == npd_check_vlan_exist(vid)) {
            if (NULL != npd_find_vlan_by_vid(vid)) {
                if ((NPD_SUCCESS == npd_vlan_get_interface_ve_flag(vid, &flag)) && \
                    (VLAN_PORT_SUBIF_EXIST == flag)) {
                    isEnable = TRUE;
                }
            }
            else {
                ret = INTERFACE_RETURN_CODE_VLAN_NOTEXIST;
            }
        }
        else {
            ret = INTERFACE_RETURN_CODE_VLAN_NOTEXIST;
        }
    }
    else if (((1 == flag)||(2 == flag)) && (0 == vid)) {/*flag == 2 is for ifname like e1-1*/
        eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no, port_no);
        if ((TRUE == npd_eth_port_rgmii_type_check(slot_no, port_no)) || \
            (TRUE == npd_check_port_promi_mode(eth_g_index))) {
            isEnable = TRUE;
        }
    }
    else {
        ret = COMMON_RETURN_CODE_BADPARAM;
        if (1 == flag) {
            ret = INTERFACE_RETURN_CODE_UNSUPPORT_COMMAND;
        }
        }
    }
    reply = dbus_message_new_method_return(msg);

    dbus_message_iter_init_append(reply, &iter);

    dbus_message_iter_append_basic(&iter,
                                   DBUS_TYPE_UINT32,
                                   &ret);
    dbus_message_iter_append_basic(&iter,
                                   DBUS_TYPE_UINT32,
                                   &isEnable);

    return reply;

}
/*******************************************************
 * DISCRIPTION:
 *      show advanced-routing-interfaces for vlan or eth-port
 *
 * INPUT:
 *      vlanAdv : uint32  - are the interfaces vlan advanced-routing
 *                          or eth-port advanced-routing
 *                         TRUE - vlan advanced-routing interfaces
 *                         FALSE  - eth-port advanced-routing interfaces
 *      includeRgmii : uint32  - whether the eth-port advanced-routing
 *                          include rgmii port intefaces or not
 *                          TRUE - include rgmii port interfaces
 *                          FALSE - not include rgmii port interfaces
 *
 * OUTPUT:
 *      showStr : String - the result of the advanced-routing interfaces
 *                          the same format with the config command
 *                          e.g. "interface eth0-1\n exit\n"
 *                              "interface eth1-3\n advanced-routing\n exit\n"
 *                              "interface vlan2\n advanced-routing\n exit\n"
 * RETURN:
 *      NULL - dbus error occured
 *      reply - success
 *
 *******************************************************/
DBusMessage * npd_dbus_interface_advanced_routing_show_running(DBusConnection *conn, DBusMessage *msg, void *user_data) {

    DBusMessage* reply;
    DBusMessageIter  iter;
    DBusError err;
#ifndef RGMII_PORT_COUNT
#define RGMII_PORT_COUNT    4
#endif

    char * showStr = NULL;
    char * currPos = NULL;
    unsigned int vlanAdv = 0;
    unsigned int includeRgmii = 0;
    unsigned int avalidLen = 0;
    unsigned int mallocSize = 0;
    unsigned int intfStrLen = strlen("interface vlan4096\n advanced-routing enable\n exit\n");
    unsigned int subCount = 0;

    dbus_error_init(&err);
    if (!(dbus_message_get_args(msg, &err,
                                DBUS_TYPE_UINT32, &vlanAdv,
                                DBUS_TYPE_UINT32, &includeRgmii,
                                DBUS_TYPE_INVALID))) {
        syslog_ax_intf_err("Unable to get input args ");
        if (dbus_error_is_set(&err)) {
            syslog_ax_intf_err("%s raised: %s", err.name, err.message);
            dbus_error_free(&err);
        }
        return NULL;
    }
    if (TRUE == vlanAdv) {
        mallocSize = intfStrLen * npd_intf_get_vlan_advanced_routing_count() + 1;/* +1 : avoid malloc(0)*/
        showStr = malloc(mallocSize);
        if (NULL == showStr) {
            return NULL;
        }
        memset(showStr, 0, mallocSize);
        currPos = showStr;
        avalidLen = mallocSize;
        npd_intf_vlan_advanced_routing_save_cfg(&currPos, &avalidLen);
    }
    else {
        unsigned int promisBmp[2] = {0};
        npd_vlan_get_promis_port_bmp(&promisBmp);
        mallocSize = npd_eth_port_count_of_bmp(&promisBmp) * intfStrLen + 1;/* +1 : avoid malloc(0)*/
        if (includeRgmii) {
            mallocSize += (RGMII_PORT_COUNT * intfStrLen);
        }
        showStr = malloc(mallocSize);
        if (NULL == showStr) {
            return NULL;
        }
        memset(showStr, 0, mallocSize);
        currPos = showStr;
        avalidLen = mallocSize;
        npd_intf_eth_port_advanced_routing_save_cfg(&currPos, &avalidLen, includeRgmii);
    }
    reply = dbus_message_new_method_return(msg);

    dbus_message_iter_init_append(reply, &iter);

    dbus_message_iter_append_basic(&iter,
                                   DBUS_TYPE_STRING,
                                   &showStr);
    free(showStr);      /* code optimize: Resource leak. zhangdi@autelan.com 2013-01-18 */
	showStr = NULL;
    return reply;
}
/*************************************************
 *
 * RETURN:
 *      the count of vlan advanced-routing interfaces
 *
 **************************************************/
int npd_intf_get_vlan_advanced_routing_count
(
    void
) {
    int len = 0;
    int i = 0;
    int rc = 0;
    int count = 0;
    enum VLAN_PORT_SUBIF_FLAG flag = VLAN_PORT_SUBIF_NOTEXIST;
    for (i = DEFAULT_VLAN_ID + 1; i < NPD_MAX_VLAN_ID; i++) {
        if (VLAN_RETURN_CODE_VLAN_EXISTS == npd_check_vlan_exist(i)) {
            if (NULL != npd_find_vlan_by_vid(i)) {
                rc = npd_vlan_get_interface_ve_flag(i, &flag);
                if (NPD_SUCCESS == rc && VLAN_PORT_SUBIF_EXIST == flag) {
                    count ++;
                }
            }
        }
    }
    rc = npd_vlan_get_interface_ve_flag(DEFAULT_VLAN_ID, &flag);
    if (NPD_SUCCESS == rc && VLAN_PORT_SUBIF_EXIST == flag) {
        count ++;
    }
    return count;
}

void npd_intf_vlan_advanced_routing_save_cfg
(
    char ** showStr,
    int* avalidLen
) {
    int len = 0;
    int i = 0;
    int rc = 0;
    enum VLAN_PORT_SUBIF_FLAG flag = VLAN_PORT_SUBIF_NOTEXIST;
    char * vlanIntfStr = "interface vlan%d\n advanced-routing enable\n exit\n";
    int strLen = strlen(vlanIntfStr) + strlen("4096") - strlen("%d");

    if ((NULL == showStr) || (NULL == *showStr) || (NULL == avalidLen)) {
        return ;
    }
    else if (strLen > *avalidLen) {
        return ;
    }
    else {
        for (i = DEFAULT_VLAN_ID/* + 1*/; i < NPD_MAX_VLAN_ID; i++) {
            if (VLAN_RETURN_CODE_VLAN_EXISTS == npd_check_vlan_exist(i)) {
                if (NULL != npd_find_vlan_by_vid(i)) {
                    rc = npd_vlan_get_interface_ve_flag(i, &flag);
                    if (NPD_SUCCESS == rc && VLAN_PORT_SUBIF_EXIST == flag) {
                        len = sprintf((*showStr), vlanIntfStr, i);
                        *showStr += len;
                        *avalidLen -= len;
                    }
                }
            }
        }
        /*
        rc = npd_vlan_get_interface_ve_flag(DEFAULT_VLAN_ID, &flag);
        if (NPD_SUCCESS == rc && VLAN_PORT_SUBIF_EXIST == flag) {
            len = sprintf((*showStr), vlanIntfStr, DEFAULT_VLAN_ID);
            *showStr += len;
            *avalidLen -= len;
        }*/
    }
    return ;
}

void npd_intf_eth_port_advanced_routing_save_cfg
(
    char ** showStr,
    int* avalidLen,
    unsigned int includeRgmii
) {
    int len = 0;
    int i = 0;
    int j = 0;
	unsigned char ifname[MAX_IFNAME_LEN + 1] = {0};
	unsigned int ifIndex = 0;
    char * ethIntfStr = "interface %s\n advanced-routing enable\n exit\n";
    int strLen = strlen(ethIntfStr) + strlen("eth4-52") - strlen("%s");

    if ((NULL == showStr) || (NULL == *showStr) || (NULL == avalidLen)) {
        return ;
    }
    else if (strLen > *avalidLen) {
        return ;
    }
    else {
        for (i = 0 ; i < CHASSIS_SLOT_COUNT; i++) {
            unsigned char local_port_count = ETH_LOCAL_PORT_COUNT(i);
            for (j = 0; j < local_port_count; j++) {
                unsigned int local_port_no = ETH_LOCAL_INDEX2NO(i, j);
                unsigned int eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(i, j);
                unsigned int slot_no = CHASSIS_SLOT_INDEX2NO(i);
				memset(ifname,0,(MAX_IFNAME_LEN + 1));
				sprintf(ifname,"eth%d-%d",slot_no, local_port_no);
				if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(ifname,&ifIndex)){						
					memset(ifname,0,(MAX_IFNAME_LEN + 1));
					sprintf(ifname,"e%d-%d",slot_no, local_port_no);
					if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(ifname,&ifIndex)){
						continue;
					}
				}
                if (includeRgmii) {
                    if (TRUE == npd_eth_port_rgmii_type_check(slot_no, local_port_no)) {
                        len = sprintf((*showStr), "interface %s\n exit\n", ifname);
                        *showStr += len;
                        *avalidLen -= len;
                    }
                }
                if ((TRUE != npd_eth_port_rgmii_type_check(slot_no, local_port_no)) && \
                    (TRUE == npd_check_port_promi_mode(eth_g_index))) {
                    len = sprintf((*showStr), ethIntfStr, ifname);
                    *showStr += len;
                    *avalidLen -= len;
                }
                
            }
        }
    }
    return ;
}

/****************************************************************
 * RETURN:
 *      INTERFACE_RETURN_CODE_SUCCESS
 *      COMMON_RETURN_CODE_BADPARAM
 *      INTERFACE_RETURN_CODE_PROMIS_PORT_TAG_IN_VLAN
 *      INTERFACE_RETURN_CODE_VLAN_IS_L3INTF
 *      INTERFACE_RETURN_CODE_ERROR
 *      INTERFACE_RETURN_CODE_CAN_NOT_SET2_EMPTY
 *      INTERFACE_RETURN_CODE_VLAN_NOTEXIST
 ****************************************************************/
DBusMessage * npd_dbus_interface_config_advanced_routing_default_vid
(
    DBusConnection *conn,
    DBusMessage *msg,
    void *user_data
) {
    DBusMessage* reply;
    DBusMessageIter  iter;
    DBusError err;
    struct vlan_s *vlanNode = NULL;
    unsigned short vid = 0;
    unsigned short oldvid = advanced_routing_default_vid;
    unsigned int ret = INTERFACE_RETURN_CODE_SUCCESS;
    unsigned int promisPortBmp[2] = {0};
    unsigned int promisCount = 0;
    unsigned int ifIndex = 0;
    unsigned int eth_g_indexs[64] = {0};
    unsigned int i = 0;
    unsigned char isTagged = FALSE;
    unsigned int untagbmp[2] = {0};
    unsigned int tagbmp[2] = {0};

if(!((productinfo.capbmp) & FUNCTION_INTERFACE_VLAUE)){
	npd_syslog_dbg("do not support this function!\n");
	ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
}
else if(((productinfo.capbmp) & FUNCTION_INTERFACE_VLAUE) \
		&& !((productinfo.runbmp) & FUNCTION_INTERFACE_VLAUE)){
	npd_syslog_dbg("do not support this function!\n");
	ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
}
else {
    dbus_error_init(&err);
    if (!(dbus_message_get_args(msg, &err,
                                DBUS_TYPE_UINT16, &vid,
                                DBUS_TYPE_INVALID))) {
        syslog_ax_intf_err("Unable to get input args ");
        if (dbus_error_is_set(&err)) {
            syslog_ax_intf_err("%s raised: %s", err.name, err.message);
            dbus_error_free(&err);
        }
        return NULL;
    }
    if((PRODUCT_ID_AX7K != PRODUCT_ID)&&\
        (PRODUCT_ID_AX5K!= PRODUCT_ID)&&\
        (PRODUCT_ID_AX5K_I != PRODUCT_ID)&&\
        (PRODUCT_ID_AX7K_I != PRODUCT_ID)){
        ret = INTERFACE_RETURN_CODE_UNSUPPORT_COMMAND;
    }
    else if ((vid < 0) || (vid > (NPD_PORT_L3INTF_VLAN_ID - 1))) {
        ret = COMMON_RETURN_CODE_BADPARAM;
    }
    else if (oldvid != vid) {
        promisCount = 64;
        if(INTERFACE_RETURN_CODE_SUCCESS != npd_eth_port_get_global_indexes_by_mode(eth_g_indexs, &promisCount, ETH_PORT_FUNC_MODE_PROMISCUOUS)){
            ret = INTERFACE_RETURN_CODE_ERROR;
        }
        else if (TRUE == npd_check_vlan_real_exist(vid)) {
            if ((FALSE == npd_vlan_interface_check(vid, &ifIndex)) || (~0UI == ifIndex)) {
                if (promisCount > 0) {
                    for (i = 0; i < promisCount; i++) {
                        if (npd_vlan_check_contain_port(vid, eth_g_indexs[i], &isTagged) && \
                            (TRUE == isTagged)) {
                            ret = INTERFACE_RETURN_CODE_PROMIS_PORT_TAG_IN_VLAN;
                        }
                    }
                }
                if (INTERFACE_RETURN_CODE_SUCCESS == ret) {
                    advanced_routing_default_vid = vid;
                    if (promisCount > 0) {
                        for (i = 0; i < promisCount; i++) {
                            ret = npd_vlan_add_or_del_port(oldvid, eth_g_indexs[i], FALSE, FALSE);
                            if(VLAN_RETURN_CODE_ERR_NONE != ret){
                                syslog_ax_intf_err("del port (eth_g_index:%#x) from old advanced-routing default vlan %d ERROR when config default-vid, go on\n",eth_g_indexs[i],oldvid);
                            }
                            ret = npd_vlan_add_or_del_port(vid, eth_g_indexs[i], FALSE, TRUE);
                            if(VLAN_RETURN_CODE_ERR_NONE != ret){
                                syslog_ax_intf_err("add port (eth_g_index:%#x) to new advanced-routing default vlan %d ERROR when config default-vid, go on\n",eth_g_indexs[i],vid);
                            }
                        }
                        ret = INTERFACE_RETURN_CODE_SUCCESS;
                    }
#if 0
                    if ((NPD_PORT_L3INTF_VLAN_ID - 1) == oldvid) {
                        vlanNode = npd_find_vlan_by_vid(oldvid);
                        if (vlanNode->isAutoCreated) {
                            vlanNode->isAutoCreated = FALSE;
                            npd_vlan_interface_destroy_node(oldvid);
                            if (VLAN_RETURN_CODE_ERR_NONE == ret) { /*no use if struct*/
                                npd_mstp_del_vlan_on_mst(oldvid);
                            }
                        }
                    }
#endif
                }
            }
            else {
                ret = INTERFACE_RETURN_CODE_VLAN_IS_L3INTF;
            }
        }
        else {
#if 1
            if ((NPD_PORT_L3INTF_VLAN_ID - 1) == vid) {
                if (promisCount > 0) {
                    ret = npd_vlan_activate(vid, "4094vlan");
                    if (VLAN_RETURN_CODE_ERR_NONE == ret) {
                        advanced_routing_default_vid = vid;
                        npd_mstp_add_vlan_on_mst(vid, untagbmp, tagbmp);
                        for (i = 0; i < promisCount; i++) {
                            ret = npd_vlan_add_or_del_port(oldvid, eth_g_indexs[i], FALSE, FALSE);
                            if(VLAN_RETURN_CODE_ERR_NONE != ret){
                                syslog_ax_intf_err("del port (eth_g_index:%#x) from old advanced-routing default vlan %d ERROR when config default-vid, go on\n",eth_g_indexs[i],oldvid);
                            }
                            ret = npd_vlan_add_or_del_port(vid, eth_g_indexs[i], FALSE, TRUE);
                            if(VLAN_RETURN_CODE_ERR_NONE != ret){
                                syslog_ax_intf_err("add port (eth_g_index:%#x) to new advanced-routing default vlan %d ERROR when config default-vid, go on\n",eth_g_indexs[i],vid);
                            }
                        }
                        ret = INTERFACE_RETURN_CODE_SUCCESS;
                    }
                    else {
                        ret = INTERFACE_RETURN_CODE_ERROR;
                    }
                }
            }
            else
#endif
                if (0 == vid) {
                    if (promisCount <= 0) {
                        advanced_routing_default_vid = vid;
                    }
                    else {
                        ret = INTERFACE_RETURN_CODE_CAN_NOT_SET2_EMPTY;
                    }
                }
                else {
                    ret = INTERFACE_RETURN_CODE_VLAN_NOTEXIST;
                }
        }
    }
}
    reply = dbus_message_new_method_return(msg);

    dbus_message_iter_init_append(reply, &iter);

    dbus_message_iter_append_basic(&iter,
                                   DBUS_TYPE_UINT32,
                                   &ret);

    return reply;
}
DBusMessage * npd_dbus_interface_show_advanced_routing_default_vid
(
    DBusConnection *conn,
    DBusMessage *msg,
    void *user_data
) {
    DBusMessage* reply;
    DBusMessageIter  iter;
    DBusError err;
    unsigned short vid = 0;

    dbus_error_init(&err);
    vid = advanced_routing_default_vid;
    reply = dbus_message_new_method_return(msg);

    dbus_message_iter_init_append(reply, &iter);

    dbus_message_iter_append_basic(&iter,
                                   DBUS_TYPE_UINT32,
                                   &vid);
    return reply;

}


/*********************************************************
*INPUT:
*   intfName  - vlan dev name
*   type -  qinq type vlaue 
*OUTPUT:
*   ret  - INTERFACE_RETURN_CODE_SUCCESS -success
*            INTERFACE_RETURN_CODE_ERROR  - on error occured
*
*********************************************************/
DBusMessage * npd_dbus_subif_set_qinq_type(DBusConnection *conn, DBusMessage *msg, void *user_data) {
#ifndef INTERFACE_NAMSIZ 
#define INTERFACE_NAMSIZ 20
#endif
    DBusMessage* reply;
    DBusMessageIter  iter;
    DBusError err;
    unsigned int ret = 0;
    unsigned char intfName[INTERFACE_NAMSIZ + 1] = {0};
    unsigned int eth_g_index = 0;
    unsigned char slot = 0;
    unsigned char port = 0;
    unsigned int vid1 = 0;
    unsigned int vid2 = 0;
    unsigned char *type = NULL;
    static unsigned int qinqCount = 0;
    static unsigned short qinqTypes[MAX_QINQ_TYPE_LEN] = {0};
    unsigned int typeValue = 0;
	int ifnameType = 0;
if(!((productinfo.capbmp) & FUNCTION_INTERFACE_VLAUE)){
	npd_syslog_dbg("do not support this function!\n");
	ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
}
else if(((productinfo.capbmp) & FUNCTION_INTERFACE_VLAUE) \
		&& !((productinfo.runbmp) & FUNCTION_INTERFACE_VLAUE)){
	npd_syslog_dbg("do not support this function!\n");
	ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
}
else {
    dbus_error_init(&err);
    if (!(dbus_message_get_args(msg, &err,
								DBUS_TYPE_UINT32, &ifnameType,
                                DBUS_TYPE_UINT16, &slot,
                                DBUS_TYPE_UINT16, &port,
                                DBUS_TYPE_UINT32, &vid1,
                                DBUS_TYPE_UINT32, &vid2,
                                DBUS_TYPE_STRING, &type,
                                DBUS_TYPE_INVALID))) {
        syslog_ax_intf_err("Unable to get input args ");
        if (dbus_error_is_set(&err)) {
            syslog_ax_intf_err("%s raised: %s", err.name, err.message);
            dbus_error_free(&err);
        }
        return NULL;
    }
    if((!vid1)||((vid1)&&(vid2))){
        ret = INTERFACE_RETURN_CODE_UNSUPPORT_COMMAND;
    }
    else{
        ret = INTERFACE_RETURN_CODE_NO_SUCH_PORT;
        if (CHASSIS_SLOTNO_ISLEGAL(slot)) {
            if (ETH_LOCAL_PORTNO_ISLEGAL(slot, port)) {
                eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot, port);
                ret = INTERFACE_RETURN_CODE_SUCCESS;
            }
        }
        if ((INTERFACE_RETURN_CODE_NO_SUCH_PORT == ret) && \
            (PRODUCT_ID_AX7K != PRODUCT_ID) && \
            (PRODUCT_ID != PRODUCT_ID_AX7K_I) && \
			(PRODUCT_ID != PRODUCT_ID_AX5K_E) && \
			(PRODUCT_ID != PRODUCT_ID_AX5608) && \
            (0 == slot)) {
            eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot, port);
            ret = INTERFACE_RETURN_CODE_SUCCESS;
        }
        if(INTERFACE_RETURN_CODE_SUCCESS == ret){
            if((TRUE != npd_eth_port_check_promi_subif(eth_g_index,vid1))&&\
                (TRUE != npd_eth_port_rgmii_type_check(slot,port))){
                ret = INTERFACE_RETURN_CODE_SUBIF_NOTEXIST;
            }
            else if(qinqCount >= MAX_QINQ_TYPE_LEN){
                ret = INTERFACE_RETURN_CODE_QINQ_TYPE_FULL;
            }
            else{
                
                sprintf(intfName,"%s%d-%d.%d",ifnameType?"e":"eth",slot,port,vid1);
                ret = npd_intf_set_intf_qinq_type(intfName,type);
            }
        }                
    }
}
    reply = dbus_message_new_method_return(msg);

    dbus_message_iter_init_append(reply, &iter);

    dbus_message_iter_append_basic(&iter,
                                   DBUS_TYPE_UINT32,
                                   &ret);

    return reply;
}
DBusMessage * npd_dbus_subif_qinq_type_save_cfg
(
    DBusConnection *conn, 
    DBusMessage *msg, 
    void *user_data
) 
    {            
    DBusMessage* reply;
    DBusMessageIter  iter;
    DBusError err;

    char * showStr = NULL;
    char * currPos = NULL;
    unsigned int avalidLen = 0;
    unsigned int mallocSize = 0;
    unsigned int subCount = 0;

    dbus_error_init(&err);

    subCount = npd_get_promis_subif_count();
    mallocSize = subCount*60+1;
    showStr = malloc(mallocSize);
    if (NULL == showStr) {
        return NULL;
    }
    memset(showStr, 0, mallocSize);
    currPos = showStr;
    avalidLen = mallocSize;        
    npd_eth_port_save_intf_qinq_type_cfg(&currPos,&avalidLen);
    reply = dbus_message_new_method_return(msg);

    dbus_message_iter_init_append(reply, &iter);

    dbus_message_iter_append_basic(&iter,
                                   DBUS_TYPE_STRING,
                                   &showStr);

    return reply;

}

DBusMessage *  get_static_neigh_from_kern(DBusConnection *conn, DBusMessage *msg, void *user_data ) {

		DBusMessage* reply;
    	DBusMessageIter  iter;
    	DBusError err;
        int fd = -1;
        int status = 0;
        int len = 0;
        int ret = ARP_RETURN_CODE_SUCCESS;
        int sndbuf = _1K*32;
        int rcvbuf = _1K*32;
        int seq = time(NULL);
		int dump = 0;
        static char buf[_1K*16] = {0};
        char abuf[256] = {0};
        socklen_t addr_len;
		unsigned int ipAddr = 0;
		unsigned char *ifName = NULL;
		unsigned int state = 0;
		unsigned char macAddr[MAC_ADDR_LEN] = {0};
		unsigned int signal = FALSE;
		unsigned int filterType = 0;
		unsigned int filter = 0;
		unsigned char f_macAddr[MAC_ADDR_LEN] = {0};
		int count = 0;
		#define MACADDR macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]
		#define IPADDR  (ipAddr>>24)&0xff,(ipAddr>>16)&0xff,(ipAddr>>8)&0xff,ipAddr&0xff
		unsigned char * currPos = NULL;
        unsigned int mallocSize = 0;;
        unsigned int intfStrLen = strlen("interface eth1-24.4096.4096\n");
        unsigned int staticArpStrLen = strlen(" config static-arp 00:00:00:00:00:00 255.255.255.255 1/24\n");
        unsigned char * advRoutStr = " advanced-routing enable\n";
        unsigned char * staticArpStr = " config static-arp %02x:%02x:%02x:%02x:%02x:%02x %d.%d.%d.%d\n";			
		unsigned char * intfStr = "interface %s\n";
		unsigned int validLen = 0;
		unsigned char * showStr = NULL;
		unsigned int haveMore = FALSE;
		
		dbus_error_init(&err);
		
		reply = dbus_message_new_method_return(msg);	 
		
		dbus_message_iter_init_append(reply, &iter);
        struct nlmsghdr *h = NULL;
        struct ndmsg *ndm = NULL;
        struct rtattr * rta_tb[NDA_MAX + 1] = {NULL};
        struct rtattr *rta = NULL;
        struct sockaddr_nl  local;
        struct sockaddr_nl nladdr;
        struct sockaddr_nl nladdr2;
        struct iovec iov;
        struct msghdr msgh = {
            .msg_name = &nladdr,
            .msg_namelen = sizeof(struct sockaddr_nl),
            .msg_iov = &iov,
            .msg_iovlen = 1,
        };
        struct {
            struct nlmsghdr nlh;
            struct rtgenmsg g;
        } req;
		
		ifName = (unsigned char *)malloc(MAX_IFNAME_LEN + 1);
		if(!ifName){
			npd_syslog_err("memory malloc failed!\n");
			return NULL;
		}
		memset(ifName, 0, MAX_IFNAME_LEN + 1);
        memset(&nladdr, 0, sizeof(struct sockaddr_nl));
        memset(&iov, 0, sizeof(struct iovec));
       
        iov.iov_base = buf;
        iov.iov_len = sizeof(buf);
        h = (struct nlmsghdr*)buf;
        fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
        if (fd < 0) {
            npd_syslog_err(" socket error %d \n", fd);
            ret = ARP_RETURN_CODE_ERROR;
			goto end_without_close_fd;
        }
        if ((ret = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf))) < 0) {
            npd_syslog_err("setsocketopt SO_SNDBUF error %d \n", ret);
            ret = ARP_RETURN_CODE_ERROR;
			goto end;
        }

        if ((ret = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf))) < 0) {
            npd_syslog_err("setsockopt SO_RCVBUF error %d\n", ret);
            ret = ARP_RETURN_CODE_ERROR;
			goto end;
        }

        memset(&local, 0, sizeof(local));
        local.nl_family = AF_NETLINK;
        local.nl_groups = RTMGRP_IPV4_IFADDR;

        if (bind(fd, (struct sockaddr*)&local, sizeof(local)) < 0) {
            npd_syslog_err("Cannot bind netlink socket\n");
            ret = ARP_RETURN_CODE_ERROR;
			goto end;
        }
        addr_len = sizeof(local);
        if (getsockname(fd, (struct sockaddr*)&local, &addr_len) < 0) {
            npd_syslog_err("Cannot getsockname");
            ret = ARP_RETURN_CODE_ERROR;
			goto end;
        }
        if (addr_len != sizeof(local)) {
            npd_syslog_err("Wrong address length %d\n", addr_len);
            ret = ARP_RETURN_CODE_ERROR;
			goto end;
        }
        if (local.nl_family != AF_NETLINK) {
            npd_syslog_err("Wrong address family %d\n", local.nl_family);
            ret = ARP_RETURN_CODE_ERROR;
			goto end;
        }

        memset(&nladdr2, 0, sizeof(nladdr2));
        nladdr2.nl_family = AF_NETLINK;

        memset(&req, 0, sizeof(req));
        req.nlh.nlmsg_len = sizeof(req);
        req.nlh.nlmsg_type = RTM_GETNEIGH;
        req.nlh.nlmsg_flags = NLM_F_ROOT | NLM_F_MATCH | NLM_F_REQUEST;
        req.nlh.nlmsg_pid = 0;
        req.nlh.nlmsg_seq = dump = ++seq;
        req.g.rtgen_family = AF_INET;

        if ((ret = sendto(fd, (void*) & req, sizeof(req), 0,
                          (struct sockaddr*) & nladdr2, sizeof(nladdr2))) < 0) {
            npd_syslog_err("Send request error %d\n", ret);
            ret = ARP_RETURN_CODE_ERROR;
			goto end;
        }
		npd_syslog_dbg("send request to kernel\n");

		while (1) {

			iov.iov_len = sizeof(buf);
			status = recvmsg(fd, &msgh, 0);
			npd_syslog_dbg("receive msg from kernel %d times \n",++count);

			if (status < 0) {
				continue;
			}

			if (status == 0) {
				ret = ARP_RETURN_CODE_ERROR;
				goto end;
			}

			h = (struct nlmsghdr*)buf;
			while (NLMSG_OK(h, status)) {
				if (nladdr.nl_pid != 0 ||
				    h->nlmsg_pid != local.nl_pid) {
					goto skip_it;
				}

				if (h->nlmsg_type == NLMSG_DONE){
					ret = ARP_RETURN_CODE_SUCCESS;
					goto end;
				}
				if (h->nlmsg_type == NLMSG_ERROR) {
					struct nlmsgerr *nlerr = (struct nlmsgerr*)NLMSG_DATA(h);
					if (h->nlmsg_len >= NLMSG_LENGTH(sizeof(struct nlmsgerr))) { 
						errno = -nlerr->error;
					}
					ret = ARP_RETURN_CODE_ERROR;
					goto end;
				}			
	            if ((h->nlmsg_type != RTM_NEWNEIGH) && (h->nlmsg_type != RTM_DELNEIGH)) {
	                goto skip_it;
	            }
	            if ((len = (h->nlmsg_len - NLMSG_LENGTH(sizeof(*ndm)))) < 0) {
	                npd_syslog_err("wrong nlmsg len %d\n", len);
	            }
	            ndm = NLMSG_DATA(h);
                memset(rta_tb, 0, sizeof(struct rtattr *) *(NDA_MAX + 1));
                rta = NDA_RTA(ndm);
                while (RTA_OK(rta, len)) {
                    if (rta->rta_type <= NDA_MAX) {
                        rta_tb[rta->rta_type] = rta;
                    }
                    rta = RTA_NEXT(rta, len);
                }
				signal = TRUE;
				ipAddr = 0;
				memset (ifName, 0, (MAX_IFNAME_LEN + 1));
				memset (macAddr, 0, MAC_ADDR_LEN);
				state = 0;
				
                if(rta_tb[NDA_DST]){
					ipAddr = *((unsigned int *)RTA_DATA(rta_tb[NDA_DST]));
                }
				if(ndm->ndm_ifindex){
					if_indextoname(ndm->ndm_ifindex, ifName);
				}
				else{
					strcpy (ifName, "null");
				}
				if(rta_tb[NDA_LLADDR]){
					memcpy(macAddr, RTA_DATA(rta_tb[NDA_LLADDR]), MAC_ADDR_LEN);
				}
				if(ndm->ndm_state){
					state = ndm->ndm_state;
				}
				if(ARP_FILTER_MAC == filterType){
					if(memcmp(macAddr, f_macAddr, MAC_ADDR_LEN)){
						goto skip_it;
					}
				}
				else if(ARP_FILTER_NONE != filterType){
					if((ARP_FILTER_IP == filterType)&&(ipAddr != filter)){
						goto skip_it;
					}
					else if((ARP_FILTER_IFINDEX == filterType)&&(filter != ndm->ndm_ifindex)){
						goto skip_it;
					}
					else if((ARP_FILTER_STATE == filterType)&&(!(filter&state))){
						goto skip_it;
					}
				}
					
				
				npd_syslog_dbg("get ip neigh item: ip %#x ifname %s mac %.2x:%.2x:%.2x:%.2x:%.2x:%.2x state %d\n",\
					ipAddr,ifName,macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5],state);

				if (state == 128) {
					 mallocSize = intfStrLen  + 1024* staticArpStrLen + 1;
   					 showStr = malloc(mallocSize);
    				 memset (showStr,0,mallocSize);
     				 currPos = showStr;
					 len = sprintf(currPos,intfStr,ifName);
					 npd_syslog_dbg("showStr == %s\n",showStr);
       				 currPos += len;
					 len = sprintf(currPos,staticArpStr,MACADDR,IPADDR);/* notice: no '\n' here*/
			   		 npd_syslog_dbg("showStr == %s\n",showStr);
              		 currPos += len;
					 len = sprintf(currPos," exit\n");
					 currPos += len;

				}

			   haveMore = TRUE;
               if(showStr){
                        dbus_message_iter_append_basic(&iter,
                                                       DBUS_TYPE_UINT32,
                                                       &haveMore);
                        dbus_message_iter_append_basic(&iter,
                                                       DBUS_TYPE_STRING,
                                                       &showStr);
                        free(showStr);
                        showStr = NULL;
                }
			   
skip_it:
				h = NLMSG_NEXT(h, status);
			}

			if (msgh.msg_flags & MSG_TRUNC) {
				continue;
			}
			if (status) {
				ret = ARP_RETURN_CODE_ERROR;
				goto end;
			}
		}

        ret = ARP_RETURN_CODE_SUCCESS;

end:
	
		close(fd);
		
end_without_close_fd:
	    free(ifName);   /* code optimize: Resource leak. zhangdi@autelan.com 2013-01-18 */
		signal = FALSE;
		haveMore = FALSE;
		dbus_message_iter_append_basic(&iter,
							DBUS_TYPE_UINT32,
								 &haveMore);
		return reply;
		

}
DBusMessage * npd_dbus_interface_static_arp_show_running(DBusConnection *conn, DBusMessage *msg, void *user_data) {

    DBusMessage* reply;
    DBusMessageIter  iter;
    DBusError err;
    char * showStr = NULL;
    int i = 0,j = 0;
	int ret;
    unsigned int subCount = 0;
	unsigned int ifIndex = ~0UI;
    unsigned int rc = 0;
    unsigned int haveMore = FALSE;
    unsigned char ifname[MAX_IFNAME_LEN+1] = {0};
    enum VLAN_PORT_SUBIF_FLAG flag = VLAN_PORT_SUBIF_NOTEXIST;
    enum NPD_ARP_INTF_TYPE intfType = 0;

    dbus_error_init(&err);

  /*  reply = dbus_message_new_method_return(msg);  */
    
   /* dbus_message_iter_init_append(reply, &iter); */
	/* remove mo use code. zhangdi@autelan.com 2013-01-18 */
	if((PRODUCT_TYPE == AUTELAN_PRODUCT_AX7605I)||(SYSTEM_TYPE == IS_DISTRIBUTED))
	{
		reply  = get_static_neigh_from_kern(conn, msg, user_data);
		return reply;
	}
    else
	{
    reply = dbus_message_new_method_return(msg);     
    
    dbus_message_iter_init_append(reply, &iter);

    for (i = 0 ; i < CHASSIS_SLOT_COUNT; i++ ) {
		unsigned char local_port_count = ETH_LOCAL_PORT_COUNT(i);
		for (j = 0; j < local_port_count; j++ ) {
			unsigned char local_port_no = ETH_LOCAL_INDEX2NO(i,j);
			unsigned int eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(i,j);
			unsigned int slot_no = CHASSIS_SLOT_INDEX2NO(i);
			unsigned int ret = 0;
			struct eth_port_s *portNode = NULL;
            struct eth_port_promi_s* promiInfo = NULL;
        	struct eth_port_dot1q_list_s *subifNode = NULL;
            struct eth_port_qinq_list_s *qinqIfNode = NULL;
        	struct list_head *list=NULL,*pos=NULL;
        	struct list_head *list2=NULL,*pos2=NULL;
            
            portNode = npd_get_port_by_index(eth_g_index);
			if(NULL == portNode) {
				continue;
			}
            if((TRUE == npd_eth_port_interface_check(eth_g_index,&ifIndex))&&(~0UI != ifIndex)){
                intfType = ARP_INTF_SIMPLE_ETH_PORT;
				memset(ifname,0,(MAX_IFNAME_LEN+1));
                sprintf(ifname,"eth%d-%d",slot_no,local_port_no);
                if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_interface_static_arp_show_running(intfType,ifname,ifIndex,&showStr)){
                    continue;
                }
                else{
                    haveMore = TRUE;
                    if(showStr){
                        dbus_message_iter_append_basic(&iter,
                                                       DBUS_TYPE_UINT32,
                                                       &haveMore);
                        dbus_message_iter_append_basic(&iter,
                                                       DBUS_TYPE_STRING,
                                                       &showStr);
                        free(showStr);
                        showStr = NULL;
                    }
                }
            }
            else if((TRUE == npd_eth_port_rgmii_type_check(slot_no,local_port_no))||\
                (TRUE == npd_check_port_promi_mode(eth_g_index))){
                intfType = ARP_INTF_ADVANCED_ETH_PORT;
                if(TRUE == npd_eth_port_rgmii_type_check(slot_no,local_port_no)){
                    intfType = ARP_INTF_RGMII_ETH_PORT;
                }
				memset(ifname,0,(MAX_IFNAME_LEN+1));
                sprintf(ifname,"eth%d-%d",slot_no,local_port_no);
                if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(ifname,&ifIndex)){
					memset(ifname,0,(MAX_IFNAME_LEN+1));
					sprintf(ifname,"e%d-%d",slot_no,local_port_no);
					if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(ifname,&ifIndex)){
                    continue;
                    }
                }
                if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_interface_static_arp_show_running(intfType,ifname,ifIndex,&showStr)){
                    continue;
                }
                else{
                    haveMore = TRUE;
                    if(showStr){
                        dbus_message_iter_append_basic(&iter,
                                                       DBUS_TYPE_UINT32,
                                                       &haveMore);
                        dbus_message_iter_append_basic(&iter,
                                                       DBUS_TYPE_STRING,
                                                       &showStr);
                        free(showStr);
                        showStr = NULL;
                    }
                }
                promiInfo = portNode->funcs.func_data[ETH_PORT_FUNC_MODE_PROMISCUOUS];
                if(promiInfo->subifList.count){
                	list = &(promiInfo->subifList.list);
                	__list_for_each(pos,list) {
                		subifNode = list_entry(pos,struct eth_port_dot1q_list_s,list);
                        if(subifNode){
                            intfType = ARP_INTF_SUBIF;
							memset(ifname,0,(MAX_IFNAME_LEN+1));
                    		sprintf(ifname,"eth%d-%d.%d",slot_no,local_port_no,subifNode->vid);
                            if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(ifname,&ifIndex)){
								memset(ifname,0,(MAX_IFNAME_LEN+1));
								sprintf(ifname,"e%d-%d.%d",slot_no,local_port_no,subifNode->vid);
								if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(ifname,&ifIndex)){
                                continue;
                                }
                            }
                            if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_interface_static_arp_show_running(intfType,ifname,ifIndex,&showStr)){
                                continue;
                            }
                            else{
                                haveMore = TRUE;
                                if(showStr){
                                    dbus_message_iter_append_basic(&iter,
                                                                   DBUS_TYPE_UINT32,
                                                                   &haveMore);
                                    dbus_message_iter_append_basic(&iter,
                                                                   DBUS_TYPE_STRING,
                                                                   &showStr);
                                    free(showStr);
                                    showStr = NULL;
                                }
                            }
                            if(subifNode->subifList.count){
                                list2 = &(subifNode->subifList.list);		
            					__list_for_each(pos2,list2) {
            						qinqIfNode = list_entry(pos2,struct eth_port_qinq_list_s,list);
            						if(qinqIfNode){
                                        intfType = ARP_INTF_QINQ_SUBIF;
										memset(ifname,0,(MAX_IFNAME_LEN+1));
                                		sprintf(ifname,"eth%d-%d.%d.%d",slot_no,local_port_no,subifNode->vid,qinqIfNode->vid);
                                        if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(ifname,&ifIndex)){
											memset(ifname,0,(MAX_IFNAME_LEN+1));
                                    		sprintf(ifname,"e%d-%d.%d.%d",slot_no,local_port_no,subifNode->vid,qinqIfNode->vid);
											if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(ifname,&ifIndex)){
                                            continue;
			                                }
                                        }
                                        if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_interface_static_arp_show_running(intfType,ifname,ifIndex,&showStr)){
                                            continue;
                                        }
                                        else{
                                            haveMore = TRUE;
                                            if(showStr){
                                                dbus_message_iter_append_basic(&iter,
                                                                               DBUS_TYPE_UINT32,
                                                                               &haveMore);
                                                dbus_message_iter_append_basic(&iter,
                                                                               DBUS_TYPE_STRING,
                                                                               &showStr);
                                                free(showStr);
                                                showStr = NULL;
                                            }
                                        }
            							
            						}
            					}
                            }
                        }
                	}
                	
                }
                
            }    		
		}		
	}

    for(i = DEFAULT_VLAN_ID; i < NPD_MAX_VLAN_ID; i++) {
		if(VLAN_RETURN_CODE_VLAN_EXISTS == npd_check_vlan_exist(i)) {
			if(NULL != npd_find_vlan_by_vid(i)) {
            	rc = npd_vlan_get_interface_ve_flag(i,&flag);
				if(NPD_DBUS_SUCCESS == rc && VLAN_PORT_SUBIF_EXIST == flag ) {
                    sprintf(ifname,"vlan%d",i);
                    if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(ifname,&ifIndex)){
                        continue;
                    }
                    intfType = ARP_INTF_ADVANCED_VLAN;
                    if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_interface_static_arp_show_running(intfType,ifname,ifIndex,&showStr)){
                        continue;
                    }
                    else{
                        haveMore = TRUE;
                        if(showStr){
                            dbus_message_iter_append_basic(&iter,
                                                           DBUS_TYPE_UINT32,
                                                           &haveMore);
                            dbus_message_iter_append_basic(&iter,
                                                           DBUS_TYPE_STRING,
                                                           &showStr);
                            free(showStr);
                            showStr = NULL;
                        }
                    }
				}
				else if((TRUE == (rc = npd_vlan_interface_check(i, &ifIndex))) && (ifIndex != ~0UI)){
                    sprintf(ifname,"vlan%d",i);
                    intfType = ARP_INTF_SIMPLE_VLAN;
					if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_interface_static_arp_show_running(intfType,ifname,ifIndex,&showStr)){                            
                        continue;
                    }
                    else{
                        haveMore = TRUE;
                        if(showStr){
                            dbus_message_iter_append_basic(&iter,
                                                           DBUS_TYPE_UINT32,
                                                           &haveMore);
                            dbus_message_iter_append_basic(&iter,
                                                           DBUS_TYPE_STRING,
                                                           &showStr);
                            free(showStr);
                            showStr = NULL;
                        }
                    }
				}
			}
		}
    }
    
    haveMore = FALSE;
    dbus_message_iter_append_basic(&iter,
                                   DBUS_TYPE_UINT32,
                                   &haveMore);
    return reply;
    }	
}
/****************************************************
*
* DESCRIPTION:
*     
****************************************************/
int npd_intf_interface_static_arp_show_running
(
    enum NPD_ARP_INTF_TYPE intfType,
    unsigned char * ifname,
    unsigned int ifIndex,
    unsigned char ** showStr
)
{
                                  
    unsigned char *mac = NULL;
    unsigned int ipAddr = 0;
#define MACADDR mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]
#define IPADDR  (ipAddr>>24)&0xff,(ipAddr>>16)&0xff,(ipAddr>>8)&0xff,ipAddr&0xff
    struct arp_snooping_item_s tempItem;
    struct arp_snooping_item_s * intfStaticArps[1024];
    unsigned int intfStrLen = strlen("interface eth1-24.4096.4096\n");
    unsigned int adRouteLen = strlen(" advanced-routing enable\n");
    unsigned int exitStrLen = strlen(" exit\n");
    unsigned int trunkLen = strlen(" 127\n");
    unsigned int portLen = strlen(" 1/24\n");
    unsigned int staticArpStrLen = strlen(" config static-arp 00:00:00:00:00:00 255.255.255.255 1/24\n");
    unsigned char * intfStr = "interface %s\n";
    unsigned char * advRoutStr = " advanced-routing enable\n";
    unsigned char * staticArpStr = " config static-arp %02x:%02x:%02x:%02x:%02x:%02x %d.%d.%d.%d";
    unsigned char * exitStr = " exit\n";
    unsigned char * currPos = NULL;
    unsigned int mallocSize = 0;
    unsigned int staticArpCount = 0;
    unsigned int validLen = 0;
    unsigned int arpIndex = 0;
    unsigned int isTrunk = 0;
    unsigned int devNum = 0,portNum = 0;
    unsigned int slot_no = 0,port_no = 0;
    unsigned int len = 0;
    unsigned char trunkId = 0;

    if((!showStr)||(!ifname)||('\0' == ifname[0])){
        syslog_ax_intf_err("showStr is %s ifname %s\n",showStr?"not null":"null",ifname?(('\0' == ifname[0])?"nil str":ifname):"null");
        return INTERFACE_RETURN_CODE_ERROR;
    }
    memset(&tempItem,0,sizeof(struct arp_snooping_item_s));   
    tempItem.isStatic = TRUE;
    tempItem.ifIndex = ifIndex;
    pthread_mutex_lock(&arpHashMutex);
    staticArpCount = npd_count_static_arp_by_ifindex(ifIndex);
    if((!staticArpCount)/*&&\
        ((ARP_INTF_ADVANCED_ETH_PORT != intfType)&&\
            (ARP_INTF_ADVANCED_VLAN != intfType))*/){
            syslog_ax_intf_dbg(" interface no static arp config \n");
            pthread_mutex_unlock(&arpHashMutex);
            return INTERFACE_RETURN_CODE_ERROR;
    }
    hash_return(arp_snoop_db_s,&tempItem,intfStaticArps,staticArpCount,npd_arp_filter_by_static_ifindex);
    mallocSize = intfStrLen + adRouteLen + staticArpCount* staticArpStrLen + 1;
    *showStr = malloc(mallocSize);
    if(!(*showStr)){
        syslog_ax_intf_err("memory malloc failed when interface show running\n");
        pthread_mutex_unlock(&arpHashMutex);
        return INTERFACE_RETURN_CODE_ERROR;
    }
    memset (*showStr,0,mallocSize);
    currPos = *showStr;
    validLen = mallocSize;
    if(validLen < intfStrLen) {
        goto memerr;
    }
    else{
        len = sprintf(currPos,intfStr,ifname);
        currPos += len;
        validLen -= len;
        /*if((ARP_INTF_ADVANCED_ETH_PORT == intfType)||\
            (ARP_INTF_ADVANCED_VLAN == intfType)){
            if(validLen < adRouteLen){
                goto memerr;
            }
            len = sprintf(currPos,advRoutStr);
            currPos += len;
            validLen -= len;
        }*/
        for(arpIndex = 0;arpIndex < staticArpCount; arpIndex++){                                
            mac = intfStaticArps[arpIndex]->mac;
            ipAddr = intfStaticArps[arpIndex]->ipAddr;
            isTrunk = intfStaticArps[arpIndex]->dstIntf.isTrunk;
            devNum = (unsigned int)(intfStaticArps[arpIndex]->dstIntf.intf.port.devNum);
            portNum = (unsigned int)(intfStaticArps[arpIndex]->dstIntf.intf.port.portNum);
            trunkId = intfStaticArps[arpIndex]->dstIntf.intf.trunk.trunkId;
            if(validLen < staticArpStrLen){
                goto memerr;
            }
            len = sprintf(currPos,staticArpStr,MACADDR,IPADDR);/* notice: no '\n' here*/
            currPos += len;
            validLen -= len;
            if(ARP_INTF_SIMPLE_VLAN == intfType){
                if(isTrunk){
                    if(validLen < trunkLen){
                        goto memerr;
                    }
                    len = sprintf(currPos," %d\n",trunkId);
                    currPos += len;
                    validLen -= len;
                }
                else{                    
                    if(nam_get_slotport_by_devport(devNum,portNum,PRODUCT_ID,&slot_no,&port_no)){                        
                        syslog_ax_intf_err("get slot/port failed when interface show running\n");
                        pthread_mutex_unlock(&arpHashMutex);
                        free(*showStr);
                        return INTERFACE_RETURN_CODE_ERROR;
                    }
                    if(validLen < portLen){
                        goto memerr;
                    }
                    len = sprintf(currPos," %d/%d\n",slot_no,port_no);
                    currPos += len;
                    validLen -= len;
                }
                                        
            }
            else {
                if(validLen < strlen("\n")){
                        goto memerr;
                    }
                    len = sprintf(currPos,"\n");
                    currPos += len;
                    validLen -= len;
            }
        }
        if(validLen < exitStrLen){
            goto memerr;
        }
        len = sprintf(currPos," exit\n");
        currPos += len;
        validLen -= len;
    }

    pthread_mutex_unlock(&arpHashMutex);
    return INTERFACE_RETURN_CODE_SUCCESS;
    memerr:
    syslog_ax_intf_err("memory malloc size too small when interface show running\n");
    pthread_mutex_unlock(&arpHashMutex);
    free(*showStr);
    return INTERFACE_RETURN_CODE_ERROR;
}

/********************************************
 * this function only can be used for ethxxx  -> exxx
 *							or exxx  -> ethxxx
 *
 ********************************************/
int npd_intf_change_intf_name
(
	unsigned char * oldName,
	unsigned char * newName,
	unsigned int 	state
)
{
#define INTF_CMD_LEN 16
    int   ret  = 0;
    char cvid[5] = {0};
    char *cmd[INTF_CMD_LEN] = {0};
	if((!oldName)||(!newName)){
		return COMMON_RETURN_CODE_NULL_PTR;
	}
	INTERFACE_CHANGE_NAME_CHECK_NAME(oldName,newName)
	memset(cmd,0,INTF_CMD_LEN * sizeof(char *));
	cmd[0] = "ifconfig"; 
	cmd[1] = oldName;
	cmd[2] = "down";
	ret |= npd_intf_execute_cmd("ifconfig",(const char **)cmd);
	memset(cmd,0,INTF_CMD_LEN * sizeof(char *));
	cmd[0] = "ip"; 
	cmd[1] = "link";
	cmd[2] = "set";
	cmd[3] = oldName;
	cmd[4] = "name";
	cmd[5] = newName;
	ret |= npd_intf_execute_cmd("ip",(const char **)cmd);
	memset(cmd,0,INTF_CMD_LEN * sizeof(char *));
	if(ETH_ATTR_LINKUP == state){
		cmd[0] = "ifconfig"; 
		cmd[1] = newName;
		if(ret){
			cmd[1] = oldName;
		}
		cmd[2] = "up";			
		npd_intf_execute_cmd("ifconfig",(const char **)cmd);/*ignore the fail*/
	}
	if(ret){
		syslog_ax_intf_err("npd intf change intf name failed, errno %#x\n",ret);
		return INTERFACE_RETURN_CODE_ERROR;
	}
	syslog_ax_intf_dbg("npd intf change intf name success\n");
	return INTERFACE_RETURN_CODE_SUCCESS;
}

   
DBusMessage * npd_dbus_intf_change_intf_name
(
    DBusConnection *conn, 
    DBusMessage *msg, 
    void *user_data
) 
    {			
    DBusMessage* reply;
    DBusMessageIter	iter;
    DBusError err;
       unsigned int ret = INTERFACE_RETURN_CODE_SUCCESS;
    unsigned char oldName[MAX_IFNAME_LEN + 1] = {0}, newName[MAX_IFNAME_LEN + 1] = {0};
    int ifnameType = 0;
    unsigned char slot = 0,port = 0;
    unsigned int tag1 = 0,tag2 = 0;
       unsigned int ifindex = ~0UI;
    unsigned int state = 0;
    unsigned int eth_g_index = 0;
    dbus_error_init(&err);

    if (!(dbus_message_get_args(msg, &err,
                                DBUS_TYPE_UINT32, &ifnameType,
                                DBUS_TYPE_BYTE, &slot,
                                DBUS_TYPE_BYTE, &port,
                                DBUS_TYPE_UINT32, &tag1,
                                DBUS_TYPE_UINT32, &tag2,
                                DBUS_TYPE_INVALID))) {
        syslog_ax_intf_err("Unable to get input args ");
        if (dbus_error_is_set(&err)) {
            syslog_ax_intf_err("%s raised: %s", err.name, err.message);
            dbus_error_free(&err);
        }
        return NULL;
    }
    /*if have subif or be subif can't change ifname */
    if(tag2||(!tag1)){
    		ret = INTERFACE_RETURN_CODE_SHORTEN_IFNAME_NOT_SUPPORT;
    }
    else{
       eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot,port);
       if(!npd_eth_port_check_promi_subif(eth_g_index,(unsigned short)tag1)){
       		syslog_ax_intf_err(" not found the subif when change ifname\n");
       		ret = INTERFACE_RETURN_CODE_SUBIF_NOTEXIST;
       }
       else if(npd_eth_port_get_subif_count_for_subif(eth_g_index,(unsigned short)tag1)){
       		syslog_ax_intf_err(" qinq subif found when change name for subif,should delete qinq subif first\n");
       		ret = INTERFACE_RETURN_CODE_PORT_HAS_SUB_IF;
       }
       else{
    	   	if(ifnameType){/*change to shorten ifname*/
    			sprintf(oldName,"eth%d-%d.%d",slot,port,tag1);
    			sprintf(newName,"e%d-%d.%d",slot,port,tag1);
    	   	}
    		else{/*change to regular ifname*/
    			sprintf(newName,"eth%d-%d.%d",slot,port,tag1);
    			sprintf(oldName,"e%d-%d.%d",slot,port,tag1);
    		}
    	   if (TRUE == npd_eth_port_rgmii_type_check(slot, port)) {
                state = ETH_ATTR_LINKUP;
            }
            else {
                npd_get_port_link_status(eth_g_index, &state);
            }
    	   
    	   ret = npd_intf_change_intf_name(oldName,newName,state);
       }
    }
    reply = dbus_message_new_method_return(msg);

    dbus_message_iter_init_append(reply, &iter);

    dbus_message_iter_append_basic(&iter,
    							  DBUS_TYPE_UINT32,
    							  &ret);

    return reply;
}
/*********************************************************************
*	DISCRIPT:
*		set eth port interface hardware enable/disable 
*			and set the port to L3 vlan or default vlan
*	INPUT:
*		slot -uint8
*		port -uint8
*		tag1 - uint32
*		tag2 - uint32
*		enable - uint32
*	OUTPUT:
*		ret  - uint32	return code
*			INTERFACE_RETURN_CODE_SUCCESS
*			INTERFACE_RETURN_CODE_NAM_ERROR
*			INTERFACE_RETURN_CODE_FDB_SET_ERROR
*			INTERFACE_RETURN_CODE_UNSUPPORT_COMMAND
*			INTERFACE_RETURN_CODE_ERROR
*
**********************************************************************/
DBusMessage * npd_dbus_intf_eth_interface_enable_set
(
   DBusConnection *conn, 
   DBusMessage *msg, 
   void *user_data
) 
{			 
	DBusMessage* reply;
	DBusMessageIter  iter;
	DBusError err;
	unsigned int ret = INTERFACE_RETURN_CODE_SUCCESS;
	unsigned char slot = 0,port = 0;
	unsigned int tag1 = 0,tag2 = 0;
	unsigned int state = 0;
	unsigned int eth_g_index = 0;
	unsigned int ifindex = ~0UI;
	unsigned int enable = 2;
	unsigned int oldEnable = 0;
	unsigned char ifName[MAX_IFNAME_LEN + 1] = {0};
	unsigned int isSlave = FALSE;
	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
								 DBUS_TYPE_BYTE, &slot,
								 DBUS_TYPE_BYTE, &port,
								 DBUS_TYPE_UINT32, &tag1,
								 DBUS_TYPE_UINT32, &tag2,
								 DBUS_TYPE_UINT32, &enable,
								 DBUS_TYPE_INVALID))) {
		 syslog_ax_intf_err("Unable to get input args ");
		 if (dbus_error_is_set(&err)) {
			 syslog_ax_intf_err("%s raised: %s", err.name, err.message);
			 dbus_error_free(&err);
		 }
		 return NULL;
	 }
	/*if have subif or be subif can't change ifname */
	if(tag1||tag2){
		 syslog_ax_intf_err("set eth interface hw enable failed, %s unsupport this operation\n",tag2?"qinq subif":"subif");
		 ret = INTERFACE_RETURN_CODE_UNSUPPORT_COMMAND;
	}
	else{
		eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot,port);
		if((NPD_TRUE == npd_eth_port_interface_check(eth_g_index, &ifindex))&&\
			(~0UI != ifindex)){
			if(NPD_SUCCESS == npd_eth_port_interface_l3_flag_get(eth_g_index, &oldEnable)){
				if(oldEnable != enable){
					if(NPD_SUCCESS != npd_eth_port_interface_l3_flag_set(eth_g_index, enable)){
						syslog_ax_intf_err("eth interface l3 flag set failed when try to set %s \n",enable ? "ENABLE" : "DISABLE");
						ret = INTERFACE_RETURN_CODE_ERROR;
					}
					else{
						sprintf(ifName, "eth%d-%d", slot, port);
						if(enable) {
							if((!npd_dynamic_trunk_port_isslave_check(ifName, &isSlave)) && isSlave){
								ret = INTERFACE_RETURN_CODE_INTERFACE_ISSLAVE;
							}
							else{
								ret = npd_vlan_interface_port_add(NPD_PORT_L3INTF_VLAN_ID,eth_g_index,NPD_FALSE);
								if(VLAN_RETURN_CODE_ERR_NONE != ret){
									syslog_ax_intf_err("vlan port %s failed when set eth interface l3 %s ret %#x\n",enable?"add":"del",enable?"ENABLE":"DISABLE",ret);											
									npd_eth_port_interface_l3_flag_set(eth_g_index, oldEnable);
									ret = INTERFACE_RETURN_CODE_ERROR;
								}
								else{/*show add to l3 intf vlan first then set interface enable*/
									ret = npd_intf_eth_port_interface_hw_enable_set(ifindex, eth_g_index, enable);
									if(INTERFACE_RETURN_CODE_SUCCESS != ret){
										syslog_ax_intf_err("eth interface l3 enable hw set to %s failed!\n",enable?"ENABLE":"DISABLE");
										/*npd_vlan_interface_port_del(NPD_PORT_L3INTF_VLAN_ID,eth_g_index,NPD_FALSE);*/
										npd_eth_port_interface_l3_flag_set(eth_g_index, oldEnable);
									}
									else{
										if(INTERFACE_RETURN_CODE_SUCCESS \
											== (ret = npd_intf_kap_dev_info_set(ifName, eth_g_index, NPD_PORT_L3INTF_VLAN_ID))){
											syslog_ax_intf_dbg("eth interface enable set success!\n");
										}
										else{
											syslog_ax_intf_dbg("eth interface enable set failed,ret %#x\n", ret);
											/* TODO: rollback it*/
										}
									}
								}
							}
						}
						else{
							ret = npd_intf_eth_port_interface_hw_enable_set(ifindex, eth_g_index, enable);
							if(INTERFACE_RETURN_CODE_SUCCESS == ret) {										
								if(!enable){/* set the interface disable then delete from l3intf vlan*/
							    	ret = npd_vlan_interface_port_del(NPD_PORT_L3INTF_VLAN_ID,eth_g_index,NPD_FALSE);
									if(VLAN_RETURN_CODE_ERR_NONE != ret){
										syslog_ax_intf_err("vlan port %s failed when set eth interface l3 %s \n",enable?"add":"del",enable?"ENABLE":"DISABLE");
										npd_intf_eth_port_interface_hw_enable_set(ifindex, eth_g_index, oldEnable);
										npd_eth_port_interface_l3_flag_set(eth_g_index, oldEnable);
										ret = INTERFACE_RETURN_CODE_ERROR;
									}
									else{			
										if(INTERFACE_RETURN_CODE_SUCCESS \
											== (ret = npd_intf_kap_dev_info_set(ifName, eth_g_index, DEFAULT_VLAN_ID))){
											syslog_ax_intf_dbg("eth interface enable set success!\n");
										}
										else{
											syslog_ax_intf_dbg("eth interface enable set failed,ret %#x\n", ret);
											/* TODO: rollback it*/
										}
									}
								}										
							}
							else{
								syslog_ax_intf_err("eth interface l3 enable hw set to %s failed!\n",enable?"ENABLE":"DISABLE");
								npd_eth_port_interface_l3_flag_set(eth_g_index, oldEnable);
							}
						}
					}
				}
				else{
					syslog_ax_intf_dbg("eth interface already %s!\n",enable ? "ENABLE":"DISABLE");
				}
			}
			else{
				syslog_ax_intf_err("get eth interface l3 flag failed\n");
				ret = INTERFACE_RETURN_CODE_ERROR;
			}
		}
		else{
			syslog_ax_intf_err("set eth interface enable failed, simple eth interface not exists\n");
			ret = INTERFACE_RETURN_CODE_UNSUPPORT_COMMAND;
		}
	}
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append(reply, &iter);

	dbus_message_iter_append_basic(&iter,
								   DBUS_TYPE_UINT32,
								   &ret);

	return reply;
	
}

/*******************************************************
 * npd_dbus_eth_interface_enable_set_show_running
 * DISCRIPTION:
 *      show l3 disabled eth interfaces
 *
 * INPUT:
 *
 * OUTPUT:
 *      showStr : String - the result of the interfaces
 *                          the same format with the config command
 *                          e.g. "interface eth1-1\n l3-function\n exit\n"
 * RETURN:
 *      NULL - dbus error occured
 *      reply - success
 *
 ********************************************************/
DBusMessage * npd_dbus_eth_interface_enable_set_show_running(DBusConnection *conn, DBusMessage *msg, void *user_data) {

    DBusMessage* reply;
    DBusMessageIter  iter;
    DBusError err;

    char * showStr = NULL;
    char * currPos = NULL;
    unsigned int avalidLen = 0;
    unsigned int mallocSize = 0;
	unsigned char * intfStr = "interface %s\n l3-function disable\n exit\n";
    unsigned int intfStrLen = strlen(intfStr) + strlen("eth1-24") + strlen("%s");
    unsigned int intfCount = 0;

    dbus_error_init(&err);		
    mallocSize = npd_intf_get_disable_eth_interface_count() * intfStrLen + 1;/* +1 : avoid malloc(0)*/
    
    showStr = malloc(mallocSize);
    if (NULL == showStr) {
        return NULL;
    }
    memset(showStr, 0, mallocSize);
    currPos = showStr;
    avalidLen = mallocSize;
    npd_intf_eth_interface_enable_set_save_cfg(&currPos, &avalidLen);
    reply = dbus_message_new_method_return(msg);

    dbus_message_iter_init_append(reply, &iter);

    dbus_message_iter_append_basic(&iter,
                                   DBUS_TYPE_STRING,
                                   &showStr);

    return reply;
}

int npd_intf_get_disable_eth_interface_count
( 
)
{

    int len = 0;
    int i = 0;
    int j = 0;   
	unsigned int ifIndex = ~0UI;
	unsigned int enable = 0;
	unsigned int count = 0;
    for (i = 0 ; i < CHASSIS_SLOT_COUNT; i++) {
        unsigned char local_port_count = ETH_LOCAL_PORT_COUNT(i);
        for (j = 0; j < local_port_count; j++) {
            unsigned int eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(i, j);
			if((NPD_TRUE == npd_eth_port_interface_check(eth_g_index,&ifIndex))&&(~0UI != ifIndex)){
				if((NPD_SUCCESS == npd_eth_port_interface_l3_flag_get(eth_g_index, &enable))&&(!enable)){
					count++;
				}
			}
        }
    }
    return count;
}


int npd_intf_eth_interface_enable_set_save_cfg
(
    char ** showStr,
    int* avalidLen
)
{

    int len = 0;
    int i = 0;
    int j = 0;
	unsigned char ifname[MAX_IFNAME_LEN + 1] = {0};
	unsigned int ifIndex = 0;
    char * ethIntfStr = "interface %s\n l3-function disable\n exit\n";
    int strLen = strlen(ethIntfStr) + strlen("eth1-24") - strlen("%s");
	unsigned int enable = 0;

    if ((NULL == showStr) || (NULL == *showStr) || (NULL == avalidLen)) {
        return ;
    }
    else if (strLen > *avalidLen) {
        return ;
    }
    else {
        for (i = 0 ; i < CHASSIS_SLOT_COUNT; i++) {
            unsigned char local_port_count = ETH_LOCAL_PORT_COUNT(i);
            for (j = 0; j < local_port_count; j++) {
                unsigned int eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(i, j);
				if((NPD_TRUE == npd_eth_port_interface_check(eth_g_index,&ifIndex))&&(~0UI != ifIndex)){
					if((NPD_SUCCESS == npd_eth_port_interface_l3_flag_get(eth_g_index, &enable))&&(!enable)){
						if_indextoname(ifIndex, ifname);
						len = sprintf((*showStr), ethIntfStr, ifname);
                        *showStr += len;
                        *avalidLen -= len;
					}
				}
            }
        }
    }
    return ;
}

#ifdef __cplusplus
}
#endif
