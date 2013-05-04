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
* npd_dynamic_trunk.c
*
*
* CREATOR:
*		hanhui@autelan.com
*
* DESCRIPTION:
*		APIs used in NPD for TRUNK module.
*
* DATE:
*		06/28/2010
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.3 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/socket.h>


#include "npd_log.h"
#include "util/npd_list.h"
#include "sysdef/npd_sysdef.h"
#include "dbus/npd/npd_dbus_def.h"
#include "npd/nam/npd_amapi.h"
#include "npd/nbm/npd_bmapi.h"
#include "npd_product.h"
#include "npd_c_slot.h"
#include "npd_vlan.h"
#include "npd_intf.h"
#include "npd_trunk.h"
#include "npd_dynamic_trunk.h"
#include "npd_eth_port.h"
#include "sysdef/returncode.h"
unsigned int dynamic_trunk_member[MAX_DYNAMIC_TRUNKID + 1][MAX_DYNAMIC_TRUNK_MEMBER] = {0};/* & 0xff0000,trunk is add, &0xff00, slot, &0xff port*/
unsigned int dynamic_trunk_member_count[MAX_DYNAMIC_TRUNKID + 1] = {0};/*dynamic trunk port member count */

unsigned int dynamin_trunk_allow_vlan_untag[MAX_DYNAMIC_TRUNKID+1][NPD_MAX_VLAN_ID] = {0};
unsigned int dynamin_trunk_allow_vlan_tag[MAX_DYNAMIC_TRUNKID+1][NPD_MAX_VLAN_ID] = {0};

#define CMD_LINE_LEN 128  /*longgest command length*/

#define DYNAMIC_ONLY_HW 0

extern int errno;

/*****************************************************************************
 * npd_dynamic_trunk_port_exist_check
 *
 * DESCRIPTION:
 *			check whether the interface [ifname] is a port of the dynamic trunk [trunkId]
 * INPUT:
 *			trunkId : uint16  -  the trunkId which we want to check
 *			ifname : string  -  the interface ifname of the port
 *			slot	    : uint8  -   the slot number of the port
 *			port	    : uint8	  -   the port number of the port
 * OUTPUT:
 *			result  : uint32  -  whether the port is added to the dynamic trunk
 *							TRUE  :  already added to the dynamic trunk
 *							FALSE :  not added to the dynamic trunk
 * RETURN:
 *			NPD_SUCCESS		-    check success
 *			NPD_FAIL		-    failed occured 
 * NOTE:
 *
 *****************************************************************************/
int npd_dynamic_trunk_port_exist_check
(
	unsigned short trunkId,
	unsigned char * ifname,
	unsigned char slot,
	unsigned char port,
	unsigned int *result
)
{/*used to check when delete port from dynamic trunk*/
	unsigned int flag = 0;
	int i = 0;
	if(POINTER_IS_NULL(ifname) ||POINTER_IS_NULL(result)){
		npd_syslog_err("dynamic trunk port exist, null pointer error, %s is null\n",\
			POINTER_IS_NULL(ifname) ? (POINTER_IS_NULL(result) ? "ifname and result" : "ifname") : "result");
		return NPD_FAIL;
	}
	  
	if(!DYNAMIC_TRUNK_ID_IS_AVALIABLE(trunkId)){
		npd_syslog_err("dynamic trunk port exist, trunkId %d is out of range\n", trunkId);
		return NPD_FAIL;
	}
	*result = FALSE;
	if(!npd_dynamic_trunk_port_isslave_check(ifname,&flag)){
		if(flag){/*is added to some dynamic trunk, then we try to match the port with slot/port */
			for(i = 0; i < MAX_DYNAMIC_TRUNK_MEMBER; i++){
				if(DYNAMIC_TRUNK_PORT_IS_ADDED(trunkId,i) && 
					DYNAMIC_TRUNK_PORT_SLOT_NUM(trunkId,i) == slot &&
					DYNAMIC_TRUNK_PORT_PORT_NUM(trunkId,i) == port){
					*result = TRUE;
					break;
				}
			}
		}
		/*else not added to any dynamic trunk , we'll get result FALSE*/
		return NPD_SUCCESS;
	}
	else{/* port isslave check failed,we can't affirm whether the port is added to any dynamic trunk*/
		npd_syslog_err("dynamic trunk port exist check failed, port %d/%d  trunk %d\n", slot, port, trunkId);
		return NPD_FAIL;
	}
}
/*****************************************************************************
 * npd_dynamic_trunk_port_isslave_check
 *
 * DESCRIPTION:
 *			check whether the interface [ifname] is a port of any dynamic trunk
 * INPUT:
 *			ifname : string  -  the interface ifname of the port
 * OUTPUT:
 *			flag  : uint32  -  whether the port is added to any dynamic trunk
 *							TRUE  :  already added to a dynamic trunk
 *							FALSE :  not added to any dynamic trunk
 * RETURN:
 *			NPD_SUCCESS		-    check success
 *			NPD_FAIL		-    failed occured 
 * NOTE:
 *
 *****************************************************************************/
int npd_dynamic_trunk_port_isslave_check
(
	unsigned char * ifname,
	unsigned int * flag
)
{
	int skfd = -1;
	int ret = 0;
	struct ifreq req_ifr;
	if(POINTER_IS_NULL(ifname) || POINTER_IS_NULL(flag)){
		npd_syslog_err("dynamic trunk port isslave check, null pointer error, %s is null\n", \
			POINTER_IS_NULL(ifname) ? (POINTER_IS_NULL(flag) ? "ifname and flag" : "ifname") : "flag");
		return NPD_FAIL;
	}
	 
	memset(&req_ifr, 0, sizeof(struct ifreq));
	if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { /*try to open a socket*/
		npd_syslog_err("dynamic trunk port isslave check, open socket failed, skfd %d\n", skfd);
		return NPD_FAIL;
	}
	
	strncpy(req_ifr.ifr_name, ifname, IFNAMSIZ);
	ret = ioctl(skfd, SIOCGIFFLAGS, &req_ifr);
	if(ret < 0){/*ioctl failed*/
		npd_syslog_err("dynamic trunk port isslave check, ioctl failed , ret %#x\n", ret);
		close(skfd);    /* code optimize: Resource leak. zhangdi@autelan.com 2012-10-18 */
		return NPD_FAIL;
	}

	if(req_ifr.ifr_flags & IFF_SLAVE){
		*flag = TRUE;
	}
	else{
		*flag = FALSE;
	}
	
	close(skfd);    /* code optimize: Resource leak. zhangdi@autelan.com 2012-10-18 */		
	return NPD_SUCCESS;
}
/*****************************************************************************
 * npd_dynamic_trunk_add_del_port
 *
 * DESCRIPTION:
 *			add or delete a port [slot/port] (interface [ifname]) to a dynamic trunk [trunkId]
 * INPUT:
 *			trunkId : uint16  -  the trunkId which we want to add port to 
 *			ifname : string  -  the interface ifname of the port
 *			slot	    : uint8  -   the slot number of the port
 *			port	    : uint8	  -   the port number of the port
 * OUTPUT:
 *			isAdd  : uint32  -  add port to the dynamic trunk 
 *								or delete the port from the dynamic trunk
 *							TRUE  :  already added to the dynamic trunk
 *							FALSE :  not added to the dynamic trunk
 * RETURN:
 *			NPD_SUCCESS		-    add or delete  success
 *			NPD_FAIL		-    failed occured 
 * NOTE:
 *
 *****************************************************************************/
int npd_dynamic_trunk_add_del_port
(
	unsigned short trunkId,
	unsigned char * ifname,
	unsigned char slot,
	unsigned char port,
	unsigned int isAdd
)
{
	int ret = 0;
	unsigned char trunkIfname[MAX_IFNAME_LEN + 1] = {0};
	unsigned char cmd[CMD_LINE_LEN] = {0};
	int index = 0;
	unsigned int eth_g_index = 0;
	unsigned int linkState = 0;
	if(POINTER_IS_NULL(ifname)){
		npd_syslog_err("dynamic trunk %s port %d/%d %s trunk %d, ifname is null pointer\n",\
			isAdd ? "add" : "del", slot, port, isAdd ? "to" : "from", trunkId);
		return NPD_FAIL;
	}
	
	if(!DYNAMIC_TRUNK_ID_IS_AVALIABLE(trunkId)){
		npd_syslog_err("dynamic trunk %s port %d/%d %s trunk, trunkId %d is out of range\n", \
			isAdd ? "add" : "del", slot, port, isAdd ? "to" : "from", trunkId);
		return NPD_FAIL;
	}
	
	sprintf(trunkIfname, "trunk%d", trunkId);
	if(isAdd){/*add eth interface to trunking interface*/
		memset(cmd, 0, CMD_LINE_LEN);
		sprintf(cmd, "sudo ifenslave -f %s %s", trunkIfname, ifname);
	}
	else{/*delete eth interface from trunking interface*/
		memset(cmd, 0, CMD_LINE_LEN);
		sprintf(cmd, "sudo ifenslave -d %s %s", trunkIfname, ifname);
	}
	if((isAdd && (DYNAMIC_TRUNK_MEMBER_IS_NOT_FULL(trunkId)))||\
		((!isAdd) && (DYNAMIC_TRUNK_MEMBER_IS_NOT_EMPTY(trunkId)))){/*check member count is normal*/
		ret = system(cmd);
		if(ret){/*cmd execute failed*/
			npd_syslog_err("cmd:'%s',ret %#x,error:%s\n",cmd,ret,errno ? strerror(errno) : "null");
			return NPD_FAIL;
		}
	}
	else{
		npd_syslog_err("dynamic trunk %s port %d/%d %s trunk %d, trunk member count %d is not normal\n",
			isAdd ? "add" : "del", slot, port, isAdd ? "to" : "from", trunkId, DYNAMIC_TRUNK_MEMBER_COUNT(trunkId));
		return NPD_FAIL;
	}

	/* why the interface is down ? */
	eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot,port);
	if(NPD_SUCCESS != npd_get_port_link_status(eth_g_index, &linkState)){
		/*if failed just output an error info, set link state as DOWN, then go on*/
		npd_syslog_err("dynamic trunk %s port %d/%d %s trunk %d, get port link state failed\n",\
			isAdd ? "add" : "del", slot, port, isAdd ? "to" : "from", trunkId);
		linkState = ETH_ATTR_LINKDOWN;
	}	
	npd_intf_set_intf_state(ifname, linkState);/*here ignore the command execute error, just try to set */
	if(!ret){
		if(isAdd){/*add */
			for(index = 0; index < MAX_DYNAMIC_TRUNK_MEMBER; index++){
				if(!DYNAMIC_TRUNK_PORT_IS_ADDED(trunkId, index)){
					DYNAMIC_TRUNK_PORT_SET(trunkId, index, slot, port);
					DYNAMIC_TRUNK_MEMBER_COUNT_INCREASE(trunkId);
					break;
				}
			}
		}
		else{/*delete*/
			for(index = 0; index < MAX_DYNAMIC_TRUNK_MEMBER; index++){/*try to match the port and clear it*/
				if(DYNAMIC_TRUNK_PORT_IS_ADDED(trunkId,index) &&
						(DYNAMIC_TRUNK_PORT_SLOT_NUM(trunkId,index) == slot) &&
						(DYNAMIC_TRUNK_PORT_PORT_NUM(trunkId,index) == port)){
					DYNAMIC_TRUNK_PORT_CLEAR(trunkId, index);
					DYNAMIC_TRUNK_MEMBER_COUNT_DECREASE(trunkId);
					break;
				}
			}
		}
	}
	return NPD_SUCCESS;
}
/*********************************************************************
 * npd_dynamic_trunk_create_delete_trunk
 * DESCRIPTION:
 *			create or delete a  dynamic trunk
 * INPUT:
 *			trunkId : uint16  - dynamic trunk id <1-8>
 *			isCreate : uint32 - is create or delete
 *							0   :  delete
 *							else: create
 * OUTPUT:
 *			NONE
 * RETURN:
 *			NPD_SUCCESS		-  		create or delete success
 *			NPD_FAIL  	  	-		failed occured
 * NOTE:
 *
 *********************************************************************/
int npd_dynamic_trunk_create_delete_trunk
(
	unsigned short trunkId,
	unsigned int isCreate
)
{
	int ret = 0;
	unsigned char trunkIfname[MAX_IFNAME_LEN + 1] = {0};
	unsigned char cmd[CMD_LINE_LEN] = {0};
	int index = 0;
	
	if(!DYNAMIC_TRUNK_ID_IS_AVALIABLE(trunkId)){
		npd_syslog_err("dynamic trunk %s trunk, trunkId %d is out of range\n", isCreate ? "create" : "delete", trunkId);
		return NPD_FAIL;
	}
	 
	sprintf(trunkIfname, "trunk%d", trunkId);
	if(isCreate){/*create dynamic trunk interface*/
		memset(cmd, 0, CMD_LINE_LEN);
		sprintf(cmd, "sudo modprobe trunking -o %s bondname=%s", trunkIfname, trunkIfname);
		ret = system(cmd);
		if(ret){
			npd_syslog_err("cmd %s, ret %#x,error: %s\n", cmd, ret, errno ? strerror(errno) : "null");
			return NPD_FAIL;
		}
		/*change mode 802.3ad*/
		memset(cmd, 0, CMD_LINE_LEN);
		sprintf(cmd, "sudo echo %d > %s/%s/%s", \
			DYNAMIC_TRUNK_MODE_802_3AD, DYNAMIC_TRUNK_SYS_CLASS_NET_PATH, trunkIfname,
			DYNAMIC_TRUNK_BONDING_MODE_PATH);	
		ret = system(cmd);
		if(ret){
			npd_syslog_err("cmd:'%s',ret %#x,error:%s\n", cmd, ret, errno ? strerror(errno) : "null");
			
			/* set mode failed, delete the dynamic trunk interface */
			memset(cmd, 0, CMD_LINE_LEN);
			sprintf(cmd, "sudo rmmod %s\n", trunkIfname);
			ret = system(cmd);
			npd_syslog_err("try to delete dynamic trunk interface, ret %#x\n", ret);
			return NPD_FAIL;
		}
		/*set fast mode*//*
		memset(cmd, 0, CMD_LINE_LEN);
		sprintf(cmd, "sudo echo 1 > %s/%s/%s", \
		DYNAMIC_TRUNK_SYS_CLASS_NET_PATH, trunkIfname, 
		DYNAMIC_TRUNK_BONDING_LACP_RATE );
		ret = system(cmd);*/
	}
	else{/* delete the dynamic trunk interface*/
		memset(cmd, 0, CMD_LINE_LEN);
		sprintf(cmd, "sudo rmmod %s\n", trunkIfname);
		ret = system(cmd);
		if(ret){
			npd_syslog_err("cmd:'%s',ret %d,error:%s\n", cmd, ret, errno ? strerror(errno) : "null");
			return NPD_FAIL;
		}
	}
	if(isCreate){/* if we create the dynamic trunk interface, try to set the interface up */
		/* get the return value just for output syslog */
		ret = npd_intf_set_intf_state(trunkIfname, ETH_ATTR_LINKUP);
		npd_syslog_dbg("try to set the interface %s up , ret %#x\n", trunkIfname, ret);
	}
	for(index = 0; index < MAX_DYNAMIC_TRUNK_MEMBER; index++){/*clear the port list for this trunk*/
		DYNAMIC_TRUNK_PORT_CLEAR(trunkId, index);
	}
	/* clear trunk member count for this trunk*/
	DYNAMIC_TRUNK_MEMBER_COUNT_CLEAR(trunkId);
	return NPD_SUCCESS;
}

/*********************************************************************
 * npd_dynamic_trunk_check_exists
 * DESCRIPTION:
 *			check dynamic trunk exists 
 * INPUT:
 *			trunkId : uint16  - dynamic trunk id <1-8>
 * OUTPUT:
 *			NONE
 * RETURN:
 *			TRUE   -  the dynamic trunk exists
 *			FALSE  -  the dynamic trunk not exists or error occured
 * NOTE:
 *
 *********************************************************************/
int npd_dynamic_trunk_check_exists
(
	unsigned short trunkId
)
{
	int ret = 0;
#if 0
	unsigned char cmd[CMD_LINE_LEN] = {0};

	memset(cmd, 0, CMD_LINE_LEN);
	sprintf(cmd,"test -f %s/%s%d\n", \
		DYNAMIC_TRUNK_PROC_NET_TRUNKING_PATH, \
		DYNAMIC_TRUNK_TRUNKING_IFNAME_PREFIX, \
		trunkId);/*replace it with open ()*/
	ret = system(cmd);	
	if(!ret){
		return TRUE;
	}
#else
	char trunkPathName [128] = {0};
	sprintf(trunkPathName,"%s/%s%d", \
		DYNAMIC_TRUNK_PROC_NET_TRUNKING_PATH, \
		DYNAMIC_TRUNK_TRUNKING_IFNAME_PREFIX, \
		trunkId);
	ret = open(trunkPathName, O_RDONLY);
	if(0 <= ret){/*open success , file exist (ret is a fd here)*/
		close(ret);
		return TRUE;
	}
#endif
	
	/*if not exists or check failed */
	return FALSE;
}

/*********************************************************************
 * npd_dbus_dynamic_trunk_add_delete_port_member
 * DESCRIPTION:
 *			dbus function for add port to or delete port from dynamic trunk
 * INPUT:
 *			isAdd   : uint32  - is add or delete port for dynamic trunk
 *			slot     : uint8    - the slot no we want to add to or delete from the dynamic trunk
 *			port     : uint8	   -  the port no we want to add to or delete from the dynamic trunk
 *			trunkId : uint16  - dynamic trunk id <1-8>
 * OUTPUT:
 *			ret : uint32  - the return value 
 *				TRUNK_RETURN_CODE_ERR_NONE            -         success
 *				TRUNK_RETURN_CODE_UNSUPPORT		-         rgmii not support this command
 *				TRUNK_RETURN_CODE_PORT_MBRS_FULL	-         port memeber for this trunk is full
 *				TRUNK_RETURN_CODE_TRUNK_NOTEXISTS		-	trunk not exists
 *				TRUNK_RETURN_CODE_ERR_GENERAL		-	add or delete failed
 *				TRUNK_RETURN_CODE_PORT_EXISTS		-	this port is already add to this trunk
 *				TRUNK_RETURN_CODE_INTERFACE_NOT_EXIST		-   the port is not a interface 
 *				TRUNK_RETURN_CODE_INTERFACE_L3_ENABLE		-   the port interface is not l3 disable interface
 *				TRUNK_RETURN_CODE_MEMBERSHIP_CONFICT          -   the port is already member of a dynamic trunk
 *				TRUNK_RETURN_CODE_BADPARAM		-	bad parameter
 *				TRUNK_RETURN_CODE_ERR_HW			-     hardware  execute failed
 * RETURN:
 *			NULL  -  get args failed 
 *			reply  -  else
 * NOTE:
 *
 *********************************************************************/
DBusMessage * npd_dbus_dynamic_trunk_add_delete_port_member
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply = NULL;
	DBusMessageIter	 iter;

	unsigned int	isAdd = 0;
	unsigned char	slot_no = 0, local_port_no = 0, slot_index = 0, local_port_index = 0;
	unsigned short	trunkId = 0;	
	unsigned int	eth_g_index = 0;
	unsigned char 	devNum = 0, portNum = 0;
	unsigned int 	ret = TRUNK_RETURN_CODE_ERR_NONE;
	unsigned char ethIntfName[MAX_IFNAME_LEN + 1] = {0};
	unsigned int ifindex = ~0UI;
	unsigned int enable = 0;
	unsigned int isSlave = 0;
	
	DBusError err;	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
							DBUS_TYPE_UINT32,&isAdd,
							DBUS_TYPE_BYTE,&slot_no,
							DBUS_TYPE_BYTE,&local_port_no,
							DBUS_TYPE_UINT16,&trunkId,
							DBUS_TYPE_INVALID))) {
		 npd_syslog_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 npd_syslog_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}	

	if(SLOT_ID == slot_no)
	{
		eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,local_port_no);
		if(TRUNK_RETURN_CODE_ERR_NONE == ret) 
		{
			if(isAdd)
			{
				if(DYNAMIC_TRUNK_ID_IS_AVALIABLE(trunkId))
				{
					if(!DYNAMIC_TRUNK_MEMBER_IS_NOT_FULL(trunkId))
					{
						npd_syslog_dbg("add port (%d/%d) to dynamic trunk %d failed, port member for this dynamic trunk is full", \
							slot_no, local_port_no, trunkId);
						ret = TRUNK_RETURN_CODE_PORT_MBRS_FULL;
					}
					else if(!npd_dynamic_trunk_check_exists(trunkId))
					{
						npd_syslog_dbg("add port to dynamic trunk failed, the dynamic trunk %d is not exists\n", trunkId);
						ret = TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;
					}
					else if((NPD_TRUE == npd_eth_port_interface_check(eth_g_index, &ifindex))&&(~0UI != ifindex))
					{
						if_indextoname(ifindex, ethIntfName);
						if((NPD_SUCCESS == npd_eth_port_interface_l3_flag_get(eth_g_index,&enable))&&(!enable))
						{
							if(!npd_dynamic_trunk_port_isslave_check(ethIntfName,&isSlave))
							{
								if(!isSlave)
								{
									npd_syslog_dbg("add interface %s to interface trunk%d\n",ethIntfName,trunkId);																								
									ret = TRUNK_RETURN_CODE_ERR_NONE;
									if(npd_dynamic_trunk_add_del_port(trunkId, ethIntfName, slot_no, local_port_no, isAdd))
									{
										npd_syslog_err("add port(%d/%d) to dynamic trunk %d, command execute failed\n", \
											slot_no, local_port_no, trunkId);
										ret = TRUNK_RETURN_CODE_ERR_GENERAL;
									}								
								}
								else
								{
									npd_syslog_dbg("add port (%d/%d) to trunk %d failed, the port is already a member of a dynamic trunk\n ", slot_no, local_port_no, trunkId);
									ret = TRUNK_RETURN_CODE_MEMBERSHIP_CONFICT;
								}
							}
							else 
							{
								npd_syslog_err("add port (%d/%d) to trunk %d, port is slave check failed\n", slot_no, local_port_no, trunkId);
								ret = TRUNK_RETURN_CODE_ERR_GENERAL;
							}								
						}
						else
						{
							npd_syslog_dbg("port(%d %d) index %d is not a l3 disable interface, do not allow add to trunk.\n", \
								slot_no, local_port_no, eth_g_index);
							ret = TRUNK_RETURN_CODE_INTERFACE_L3_ENABLE;
						}
					}
					else
					{
						npd_syslog_dbg("add port (%d/%d) to trunk %d failed, the interface is not exists\n", \
							slot_no, local_port_no, trunkId);
						ret = TRUNK_RETURN_CODE_INTERFACE_NOT_EXIST;
					}
				}
				else
				{
					npd_syslog_dbg("add port (%d/%d) to trunk %d failed, trunkId is out of range\n", \
							slot_no, local_port_no, trunkId);						
					ret = TRUNK_RETURN_CODE_TRUNKID_OUT_OF_RANGE;
				}
			}
			else
			{
				if(DYNAMIC_TRUNK_ID_IS_AVALIABLE(trunkId))
				{
					if(!npd_dynamic_trunk_check_exists(trunkId))
					{
						npd_syslog_dbg("delete port from dynamic trunk failed, the dynamic trunk %d is not exists\n", trunkId);
						ret = TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;
					}
					else if((NPD_TRUE == npd_eth_port_interface_check(eth_g_index, &ifindex))&&(~0UI != ifindex))
					{
						if_indextoname(ifindex, ethIntfName);							
						npd_syslog_dbg("delete interface %s from interface trunk%d\n",ethIntfName,trunkId);
						if(npd_dynamic_trunk_slave_port_trunkId_get(ethIntfName) != trunkId)
						{
							npd_syslog_dbg("delete port from dynamic trunk failed, port is not a member of this dynamic trunk\n");
							ret = TRUNK_RETURN_CODE_PORT_NOTEXISTS;
						}
						else
						{
#if DYNAMIC_ONLY_HW
							if(NPD_SUCCESS == npd_get_devport_by_global_index(eth_g_index, &devNum, &portNum))
							{/*delete port in hardware*/
								/*we need not check if the port is in the trunk, the function will check it before delete*/
								ret = nam_asic_trunk_ports_del(devNum, trunkId + MAX_STATIC_TRUNK_ID, portNum);
								if(TRUNK_CONFIG_SUCCESS != ret)
								{
									npd_syslog_err("delete port from dynamic trunk failed, ret %#x\n", ret);
									ret = TRUNK_RETURN_CODE_ERR_HW;
								}
								else
								{
									ret = TRUNK_RETURN_CODE_ERR_NONE;
								}
							}
							else
							{
								npd_syslog_err("delete port from dynamic trunk, get devnum portnum failed\n");
								ret = TRUNK_RETURN_CODE_ERR_GENERAL;
							}
#else
							ret = npd_trunk_port_del_for_dynamic_trunk(trunkId + MAX_STATIC_TRUNK_ID, eth_g_index);
							if(ret && (TRUNK_RETURN_CODE_PORT_NOTEXISTS != ret))
							{
								npd_syslog_err("delete port from dynamic trunk failed, ret %#x\n", ret);
								ret = TRUNK_RETURN_CODE_ERR_GENERAL;
							}
							else
							{
								ret = TRUNK_RETURN_CODE_ERR_NONE;
							}

#endif
						}
						if(TRUNK_RETURN_CODE_ERR_NONE == ret)
						{/*delete port from dynamict trunk in software*/
							if(npd_dynamic_trunk_add_del_port(trunkId, ethIntfName, slot_no, local_port_no, isAdd))
							{
								npd_syslog_err("delete port from dynamic trunk failed in software, rollback it in hardware\n");
#if DYNAMIC_ONLY_HW
								/*rollback , add port to trunk*/
								ret = nam_asic_trunk_ports_add(devNum, trunkId + MAX_STATIC_TRUNK_ID, portNum, 1);
								if(TRUNK_CONFIG_SUCCESS != ret)
								{
									npd_syslog_err("try to add port to trunk %d in hardware failed, ret %#x\n", trunkId + MAX_STATIC_TRUNK_ID, ret);
								}
									
#else
								/*rollback , add port to trunk*/
								ret = npd_trunk_port_add_for_dynamic_trunk(trunkId + MAX_STATIC_TRUNK_ID, eth_g_index);
								if(ret)
								{
									npd_syslog_err("try to add port to trunk %d failed, ret %#x\n", trunkId + MAX_STATIC_TRUNK_ID, ret);
								}

#endif
								ret = TRUNK_RETURN_CODE_ERR_GENERAL;
							}
						}
					}						
				}
				else
				{
					npd_syslog_dbg("delete port (%d/%d) from trunk %d failed, trunkId is out of range\n", \
							slot_no, local_port_no, trunkId);		
					ret = TRUNK_RETURN_CODE_TRUNKID_OUT_OF_RANGE;
				}
			}
		}
	}
	else 
	{
		npd_syslog_dbg("%s port (%d/%d) %s trunk %d failed, no such port\n", \
							isAdd ? "add" : "delete", slot_no, local_port_no, isAdd ? "to" : "from", trunkId);	
		ret = TRUNK_RETURN_CODE_NO_SUCH_PORT;
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	return reply;
}


/*********************************************************************
 * npd_dbus_dynamic_trunk_create_trunk
 * DESCRIPTION:
 *			dbus function for create a  dynamic trunk
 * INPUT:
 *			trunkId : uint16  - dynamic trunk id <1-8>
 * OUTPUT:
 *			ret : uint32  - the return value 
 *				TRUNK_RETURN_CODE_ERR_NONE            -         success
 *				TRUNK_RETURN_CODE_TRUNK_EXISTS      -	dynamic  trunk exists 
 *				TRUNK_RETURN_CODE_ERR_GENERAL		-	create failed
 *				TRUNK_RETURN_CODE_BADPARAM		-	bad parameter
 *				TRUNK_RETURN_CODE_ERR_HW			-     hardware  execute failed
 * RETURN:
 *			NULL  -  get args failed 
 *			reply  -  else
 * NOTE:
 *
 *********************************************************************/
DBusMessage * npd_dbus_dynamic_trunk_create_trunk
(	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	DBusMessage* reply = NULL;
	DBusMessageIter	 iter;
	unsigned short	trunkId = 0;
	unsigned char   name[ALIAS_NAME_SIZE] = {0};
	unsigned char   dytrunkname[ALIAS_NAME_SIZE] = {0};
	unsigned int	ret = TRUNK_RETURN_CODE_ERR_NONE;
	unsigned int isCreate = TRUE;
	DBusError err;
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
							 DBUS_TYPE_UINT16,&trunkId,
							 DBUS_TYPE_INVALID))) {
		 syslog_ax_trunk_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_trunk_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	sprintf(name, "trunk%d", trunkId);
	sprintf(dytrunkname, "dy%s", name);
	syslog_ax_trunk_dbg("create dynamic trunk %d,trunk name %s.\n",trunkId,name);
	
	if(!DYNAMIC_TRUNK_ID_IS_AVALIABLE(trunkId)){
		npd_syslog_dbg("create dynamic trunk failed, trunkId %d is out of range\n", trunkId);
		ret = TRUNK_RETURN_CODE_BADPARAM;
	}
	else if(npd_dynamic_trunk_check_exists(trunkId)){
		npd_syslog_dbg("create dynamic trunk failed, dynamic trunk is already exists\n");
		ret = TRUNK_RETURN_CODE_TRUNK_EXISTS;
	}
	else{		
		syslog_ax_trunk_dbg("Create interface trunk%d\n",trunkId);
		ret = npd_dynamic_trunk_create_delete_trunk(trunkId, isCreate);
		if(!ret){
#if DYNAMIC_ONLY_HW
			ret = nam_asic_trunk_entry_active(trunkId + MAX_STATIC_TRUNK_ID);
			if(TRUNK_CONFIG_SUCCESS != ret){
				npd_syslog_err("create dynamic trunk, create in hardware failed, ret %#x\n", ret);
				/*try to rollback the dynamic trunk and ignore the error*/
				npd_dynamic_trunk_create_delete_trunk(trunkId, !isCreate);
				ret = TRUNK_RETURN_CODE_ERR_HW;
			}
			else{
				ret = TRUNK_RETURN_CODE_ERR_NONE;
			}
#else
			ret = npd_trunk_activate(trunkId + MAX_STATIC_TRUNK_ID,dytrunkname);
			if(TRUNK_RETURN_CODE_ERR_NONE != ret){
				npd_syslog_err("create dynamic trunk, create trunk failed, ret %#x\n", ret);
				/*try to rollback the dynamic trunk and ignore the error*/
				npd_dynamic_trunk_create_delete_trunk(trunkId, !isCreate);
				ret = TRUNK_RETURN_CODE_ERR_HW;
			}
			else{
				ret = TRUNK_RETURN_CODE_ERR_NONE;
			}
#endif
		}
		else{
			npd_syslog_err("create dynamic trunk, command execute failed, ret %#x\n", ret);
			ret = TRUNK_RETURN_CODE_ERR_GENERAL;
		}
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32, 
									 &ret);
	return reply;

}

/*********************************************************************
 * npd_dbus_dynamic_trunk_create_trunk
 * DESCRIPTION:
 *			dbus function for create a  dynamic trunk
 * INPUT:
 *			trunkId : uint16  - dynamic trunk id <1-8>
 * OUTPUT:
 *			ret : uint32  - the return value 
 *				TRUNK_RETURN_CODE_ERR_NONE            -         success
 *				TRUNK_RETURN_CODE_TRUNK_EXISTS      -	dynamic  trunk exists 
 *				TRUNK_RETURN_CODE_ERR_GENERAL		-	create failed
 *				TRUNK_RETURN_CODE_BADPARAM		-	bad parameter
 *				TRUNK_RETURN_CODE_ERR_HW			-     hardware  execute failed
 * RETURN:
 *			NULL  -  get args failed 
 *			reply  -  else
 * NOTE:
 *
 *********************************************************************/
DBusMessage * npd_dbus_dynamic_trunk_delete_trunk
(	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	DBusMessage* reply = NULL;
	DBusMessageIter	 iter;
	unsigned short	trunkId = 0;
	struct trunk_s	*trunkNode = NULL;
	unsigned int	ret = TRUNK_RETURN_CODE_ERR_NONE;
	unsigned int flag = FALSE;
	unsigned int	isAdd = FALSE, isCreate = FALSE;
	unsigned char	slot_no = 0, local_port_no = 0;
	unsigned int	eth_g_index = 0;
	unsigned char ethIntfName[MAX_IFNAME_LEN + 1] = {0};
	unsigned int ifindex = ~0UI;
	int i = 0;
	unsigned int oldPorts [MAX_DYNAMIC_TRUNK_MEMBER] = {0};
	DBusError err;
	
	 syslog_ax_trunk_dbg("Entering delete dynamic trunk one!\n");
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT16,&trunkId,
		DBUS_TYPE_INVALID))) {
		 syslog_ax_trunk_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_trunk_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	if(!DYNAMIC_TRUNK_ID_IS_AVALIABLE(trunkId)){
		npd_syslog_dbg("dynamic trunk port exist, trunkId %d is out of range\n", trunkId);
		ret = TRUNK_RETURN_CODE_BADPARAM;
	}
	else if(!npd_dynamic_trunk_check_exists(trunkId)){
		npd_syslog_dbg("delete dynamie trunk failed, dynamic trunk %d not exists\n", trunkId);
		ret = TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;
	}
	else{
		if(DYNAMIC_TRUNK_MEMBER_IS_NOT_EMPTY(trunkId)){
			DYNAMIC_TRUNK_GET_TRUNK_PORTS_INFO(oldPorts, trunkId);
			/*try to delete ports from dynamic trunk*/
			isAdd = FALSE; /*delete ports info */
			npd_dynamic_trunk_add_del_ports_for_trunk(trunkId, oldPorts, isAdd);
		}
		if(TRUNK_RETURN_CODE_ERR_NONE == ret){
			isCreate = FALSE;/*delete*/
			if(npd_dynamic_trunk_create_delete_trunk(trunkId, isCreate)){
				/*rollback, try add ports to dynamic trunk*/
				isAdd = TRUE;
				npd_dynamic_trunk_add_del_ports_for_trunk(trunkId, oldPorts, isAdd);
				npd_syslog_err("delete dynamic trunk, command execute failed, trunkId %d\n", trunkId);
				ret = TRUNK_RETURN_CODE_ERR_GENERAL;
			}
		}
			
		if(TRUNK_RETURN_CODE_ERR_NONE == ret){
			syslog_ax_trunk_dbg("delete dynamic trunk entry %d.\n",trunkId);
#if DYNAMIC_ONLY_HW
			ret = nam_asic_trunk_delete(trunkId + MAX_STATIC_TRUNK_ID);
			if(TRUNK_CONFIG_SUCCESS != ret){					
				npd_syslog_err("npd trunk destroy failed, ret %#x\n", ret);
				/*try to rollback and ignore the rollback error*/
				npd_dynamic_trunk_create_delete_trunk(trunkId, !isCreate);
				/*rollback, try add ports to dynamic trunk*/
				isAdd = TRUE;
				npd_dynamic_trunk_add_del_ports_for_trunk(trunkId, oldPorts, isAdd);
				ret = TRUNK_RETURN_CODE_ERR_HW;
			}
#else
			ret = npd_trunk_destroy_node(trunkId + MAX_STATIC_TRUNK_ID);
			if(TRUNK_RETURN_CODE_ERR_NONE == ret){
				/*delete trunk fdb entries*/
				nam_fdb_table_delete_entry_with_trunk(trunkId + MAX_STATIC_TRUNK_ID);
			}
			else{
				npd_syslog_err("npd trunk destroy failed, ret %#x\n", ret);
				/*try to rollback and ignore the rollback error*/
				npd_dynamic_trunk_create_delete_trunk(trunkId, !isCreate);
				/*rollback, try add ports to dynamic trunk*/
				isAdd = TRUE;
				npd_dynamic_trunk_add_del_ports_for_trunk(trunkId, oldPorts, isAdd);
				ret = TRUNK_RETURN_CODE_ERR_HW;
			}					
#endif				
			
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32, 
									 &ret);
	
	return reply;

}

/*********************************************************************
 * npd_dynamic_trunk_add_del_ports_for_trunk
 * DESCRIPTION:
 *			for add 1-8 ports to dynamic trunk
 * INPUT:
 *			trunkId : uint16  - dynamic trunk id <1-8>
 *			portsInfo : uint32 *  -  ports info we want to add to the dynamic trunk
 *			isAdd : uint32	-  is add ports to trunk or delete ports from trunk
 * OUTPUT:
 *			NONE 
 * RETURN:
 *			NPD_SUCCESS            -         success
 *			NPD_FAIL		-	error ocuured
 * NOTE:
 *
 *********************************************************************/

int npd_dynamic_trunk_add_del_ports_for_trunk(unsigned short trunkId, unsigned int * portsInfo, unsigned int isAdd){
	int i = 0;
	unsigned char slot_no = 0, local_port_no = 0;
	unsigned int eth_g_index = 0;
	unsigned int ifindex = ~0UI;
	unsigned char ethIntfName[MAX_IFNAME_LEN + 1] = {0};
	
	if(POINTER_IS_NULL(portsInfo)){
		npd_syslog_err("add ports to dynamic trunk %d, ports info is null pointer\n", trunkId);
		return NPD_FAIL;
	}
	for(i = 0; i < MAX_DYNAMIC_TRUNK_MEMBER; i++){
		if((portsInfo[i]>>16)&0xff){/*is need add*/
			slot_no = (portsInfo[i]>>8)&0xff;/*get slot no*/
			local_port_no = portsInfo[i]&0xff;/*get port no*/
			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no, local_port_no);
			if((NPD_TRUE == npd_eth_port_interface_check(eth_g_index, &ifindex))&&(~0UI != ifindex)){/*for get ifname*/
				if_indextoname(ifindex, ethIntfName);							
				syslog_ax_trunk_dbg("delete interface %s from interface trunk%d\n",ethIntfName,trunkId);
				if(npd_dynamic_trunk_add_del_port(trunkId, ethIntfName, slot_no, local_port_no, isAdd)){/*just output syslog,and go on*/
					npd_syslog_err("%s dynamic trunk , %s port (%d/%d) %s dynamic trunk %d error\n", \
						isAdd ? "add" : "delete", isAdd ? "add" : "delete", \
						slot_no, local_port_no, isAdd ? "to" : "from", trunkId);
				}
			}							
		}
		return NPD_SUCCESS;
	}
}

/*********************************************************************
 * npd_dbus_dynamic_trunk_create_trunk
 * DESCRIPTION:
 *			dbus function for config dynamic trunk, enter dynamic trunk configure node
 * INPUT:
 *			trunkId : uint16  - dynamic trunk id <1-8>
 * OUTPUT:
 *			ret : uint32  - the return value 
 *				TRUNK_RETURN_CODE_ERR_NONE            -         success
 *				TRUNK_RETURN_CODE_TRUNK_NOTEXISTS      -	dynamic  trunk not exists 
 *				TRUNK_RETURN_CODE_BADPARAM		-	bad parameter, trunkId out of range
 * RETURN:
 *			NULL  -  get args failed 
 *			reply  -  else
 * NOTE:
 *
 *********************************************************************/
DBusMessage * npd_dbus_dynamic_trunk_config
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply = NULL;
	DBusMessageIter	 iter;
	unsigned short	trunkId = 0;
	unsigned int	ret = 0,op_ret = TRUNK_RETURN_CODE_ERR_NONE;
	DBusError err;
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT16,&trunkId,
		DBUS_TYPE_INVALID))) {
		 syslog_ax_trunk_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_trunk_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	syslog_ax_trunk_dbg("get trunk Id %d from Dcli.\n",trunkId);
	
	if(!DYNAMIC_TRUNK_ID_IS_AVALIABLE(trunkId)){
		npd_syslog_dbg("dynamic trunk port exist, trunkId %d is out of range\n", trunkId);
		ret = TRUNK_RETURN_CODE_BADPARAM;
	}
	else{
		ret = npd_dynamic_trunk_check_exists(trunkId);
		if(TRUE == ret){
			op_ret = TRUNK_RETURN_CODE_ERR_NONE;
		}
		else{
			npd_syslog_dbg("config dynamic trunk failed,dynamic trunk %d not exists\n", trunkId);
			op_ret = TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;
		}
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32, 
									 &op_ret);
	return reply;
}

/*********************************************************************
 * npd_dbus_dynamic_trunk_show_running_config
 * DESCRIPTION:
 *			dbus function for show running config
 * INPUT:
 *			NONE
 * OUTPUT:
 *			showStr : string  -  the show running result string, eg. "create dynamic-trunk 1
 *														    config dynamic-trunk 1
 *															add port 1/1
 *														    exit
 *														    "
 * RETURN:
 *			NULL  -  get args failed 
 *			reply  -  else
 * NOTE:
 *
 *********************************************************************/
DBusMessage * npd_dbus_dynamic_trunk_show_running_config
(	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	char *showStr = NULL;

	showStr = (char*)malloc(_1K);
	if(NULL == showStr) {
		 syslog_ax_trunk_err("memory malloc error\n");
		return NULL;
	}
	memset(showStr,0,_1K);
	/*save trunk cfg*/
	npd_dynamic_trunk_save_cfg(showStr,_1K);
	syslog_ax_trunk_dbg("dynamic trunk config:%s\n",showStr);
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_STRING,
									 &showStr);	
	free(showStr);
	showStr = NULL;
	return reply;
}

/*********************************************************************
 * npd_dbus_dynamic_trunk_show_running_config
 * DESCRIPTION:
 *			for show running config
 * INPUT:
 *			showStr : char *   - buf for show string
 *			bufLen : uint32	-  the length of the buf input
 * OUTPUT:
 *			showStr : string  -  the show running result string, eg. "create dynamic-trunk 1
 *														    config dynamic-trunk 1
 *															add port 1/1
 *														    exit
 *														    "
 * RETURN:
 *			NONE
 * NOTE:
 *
 *********************************************************************/
void npd_dynamic_trunk_save_cfg(char* buf,int bufLen)
{
	char *showStr = buf,*cursor = NULL;
	int totalLen = 0;
	unsigned short trunkId = 0;
	unsigned int index = 0;
	unsigned char slot_no = 0,local_port_no = 0;
	unsigned char enter_node_dynamic_trunk=0;
	unsigned short vlanid = 0;
	cursor = showStr;
	if(!buf){ return;}
	totalLen +=sprintf(cursor,"config dynamic trunk switch-board %d\n",SLOT_ID);
	cursor = showStr + totalLen;
	enter_node_dynamic_trunk = 1;
	for(trunkId = MIN_DYNAMIC_TRUNKID; trunkId <= MAX_DYNAMIC_TRUNKID; trunkId++) 
	{
		char tmpBuf[_1K] = {0},*tmpPtr = NULL;
		int length = 0;
		tmpPtr = tmpBuf;
		
		if(TRUE == npd_dynamic_trunk_check_exists(trunkId)) 
		{
			if((length + 25) >= _1K) 
			{
				totalLen += sprintf(cursor,"%s",tmpBuf);
				cursor = showStr + totalLen;
				memset(tmpBuf, 0, _1K);
			}
			length += sprintf(tmpPtr,"create dynamic-trunk %d\n",trunkId);
			syslog_ax_trunk_dbg("%s\n",tmpPtr);
			tmpPtr = tmpBuf+length;
			if(DYNAMIC_TRUNK_MEMBER_IS_NOT_EMPTY(trunkId))
			{
				if((length + 25) >= _1K) 
				{
					totalLen += sprintf(cursor,"%s",tmpBuf);
					cursor = showStr + totalLen;
					memset(tmpBuf, 0, _1K);
					tmpPtr = tmpBuf;
				}
				length += sprintf(tmpPtr,"config dynamic-trunk %d\n",trunkId);
				syslog_ax_trunk_dbg("%s\n",tmpPtr);
				tmpPtr = tmpBuf+length;
				for(index = 0; index < MAX_DYNAMIC_TRUNK_MEMBER; index++)
				{
					if(DYNAMIC_TRUNK_PORT_IS_ADDED(trunkId, index))
					{
						slot_no = DYNAMIC_TRUNK_PORT_SLOT_NUM(trunkId, index);
						local_port_no = DYNAMIC_TRUNK_PORT_PORT_NUM(trunkId, index);
						if((length + 20) >= _1K) 
						{
							totalLen += sprintf(cursor,"%s",tmpBuf);
							cursor = showStr + totalLen;
							memset(tmpBuf, 0, _1K);
							tmpPtr = tmpBuf;
						}
						length += sprintf(tmpPtr," enable port %d/%d lacp function\n", slot_no, local_port_no);
						syslog_ax_trunk_dbg("%s\n",tmpPtr);
						tmpPtr = tmpBuf+length;
						length += sprintf(tmpPtr," add port %d/%d\n", slot_no, local_port_no);
						syslog_ax_trunk_dbg("%s\n",tmpPtr);
						tmpPtr = tmpBuf+length;
					}
				}
				for(vlanid = 0;vlanid<NPD_MAX_VLAN_ID;vlanid++)
				{
					if(dynamin_trunk_allow_vlan_untag[trunkId-1][vlanid] == 1)
					{
						length += sprintf(tmpPtr," allow vlan %d untag\n", vlanid+1);
						syslog_ax_trunk_dbg("%s\n",tmpPtr);
						tmpPtr = tmpBuf+length;
					}
					if(dynamin_trunk_allow_vlan_tag[trunkId-1][vlanid] == 1)
					{
						length += sprintf(tmpPtr," allow vlan %d tag\n", vlanid+1);
						syslog_ax_trunk_dbg("%s\n",tmpPtr);
						tmpPtr = tmpBuf+length;
					}
				}
				if((length + 10) < _1K)
				{
					length += sprintf(tmpPtr," exit\n");
					syslog_ax_trunk_dbg("%s\n",tmpPtr);
					tmpPtr = tmpBuf+length;
				}
			}		
			if(totalLen + strlen(tmpBuf) < bufLen)
			{
				totalLen += sprintf(cursor,"%s",tmpBuf);
				cursor = showStr + totalLen;			
			}
		}
	}
	if(enter_node_dynamic_trunk == 1)
	{
		totalLen += sprintf(cursor," exit\n");
		cursor = showStr + totalLen;
	}
}
/*********************************************************************
 * npd_dbus_dynamic_trunk_show_dynamic_trunk_list
 * DESCRIPTION:
 *			dbus function for show dynamic trunk list
 * INPUT:
 *			trunkId : uint16 -  the dynamic trunkId we want to show port list
 *							0  -  show dynamic trunk port list
 *							else - show dynamic trunk [trunkId] port list
 * OUTPUT:
 *			1) the trunkId input is 0:
 *					id  : uint32   -  the trunkId or 0(no more dynamic trunk to show)
 *							0  -  no more dynamic trunk to show
 *							else - show dynamic trunk[id]
 *						while(id){
 *								haveMore : uint32  - have port list or not
 *										0  -  no more port 
 *										else  - have more port to show
 *									while(haveMore){
 *											slot : uint8  -  slot no
 *											port : uint8  -  port no
 *											haveMore : uint32  - have port list or not
 *									}
 *								id : uint32  -  the trunkId or 0(no more dynamic trunk to show)
 *						}
 *			2) the trunkId input is not 0:
 *					ret : uint32	-  return value
 *								TRUNK_RETURN_CODE_TRUNKID_OUT_OF_RANGE
 *								TRUNK_RETURN_CODE_TRUNK_NOTEXISTS
 *								TRUNK_RETURN_CODE_TRUNK_ERR_NONE
 *						if(TRUNK_RETURN_CODE_TRUNK_ERR_NONE == ret){
 *								haveMore : uint32  - have port list or not
 *										0  -  no more port 
 *										else  - have more port to show
 *									while(haveMore){
 *											slot : uint8  -  slot no
 *											port : uint8  -  port no
 *											haveMore : uint32  - have port list or not
 *									}
 *						}
 * RETURN:
 *			NULL  -  get args failed 
 *			reply  -  else
 * NOTE:
 *
 *********************************************************************/
DBusMessage * npd_dbus_dynamic_trunk_show_dynamic_trunk_list
(	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	DBusMessage* reply = NULL;
	DBusMessageIter	 iter;
	DBusError err;
	
	unsigned int ret = 0;
	unsigned char slot = 0, port = 0;
	unsigned short trunkId = 0, id = 0;
	unsigned int haveMore = FALSE;
	int index = 0;
	
	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT16,&trunkId,
		DBUS_TYPE_INVALID))) {
		 syslog_ax_trunk_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_trunk_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	
	reply = dbus_message_new_method_return(msg);	
	
	dbus_message_iter_init_append (reply, &iter);	
	
	if(!trunkId){/*get trunkId 0, means we want to show all dynamic trunk port list*/
			
		for(id = MIN_DYNAMIC_TRUNKID; id <= MAX_DYNAMIC_TRUNKID; id++){
			if(npd_dynamic_trunk_check_exists(id)){/* if dynamic trunk exists */
				dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT16,
									 &id);/*append the trunkId to dcli*/
				for(index = 0; index < MAX_DYNAMIC_TRUNK_MEMBER; index++){
					slot = 0;
					port = 0;
					if(DYNAMIC_TRUNK_PORT_IS_ADDED(id,index)){
						slot = DYNAMIC_TRUNK_PORT_SLOT_NUM(id,index);
						port = DYNAMIC_TRUNK_PORT_PORT_NUM(id,index);
						haveMore = TRUE;
						dbus_message_iter_append_basic (&iter,
											 DBUS_TYPE_UINT32,
											 &haveMore);/*if this port is set append a signal to dcli, notify dcli to get slot/port*/
						dbus_message_iter_append_basic (&iter,
										 DBUS_TYPE_BYTE,
										 &slot);
						dbus_message_iter_append_basic (&iter,
										 DBUS_TYPE_BYTE,
										 &port);
					}
				}
				haveMore = FALSE;				
				dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &haveMore);/* slot port append over, append a "FALSE" to dcli,means no more port for this trunk*/
			}
		}
		id = 0;/* 0 : end*/
		dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT16,
									 &id);/*set 0 to dcli. means no more trunk would append to dcli */
	}
	else{
		id = trunkId;
		if(!DYNAMIC_TRUNK_ID_IS_AVALIABLE(trunkId)){
			npd_syslog_err("dynamic trunk show trunk list, trunkId %d is out of range\n", trunkId);
			ret = TRUNK_RETURN_CODE_TRUNKID_OUT_OF_RANGE;
		}
		else{
			ret = TRUNK_RETURN_CODE_ERR_NONE;
			if(!npd_dynamic_trunk_check_exists(id)){
				npd_syslog_dbg("show dynamic trunk port list failed, dynamic trunk %d not exists\n", id);
				ret = TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;
			}
		}
		dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);/*append a return value, if no error then append slot/port list*/
		if(TRUNK_RETURN_CODE_ERR_NONE == ret){
			for(index = 0; index < MAX_DYNAMIC_TRUNK_MEMBER; index++){
				slot = 0;
				port = 0;
				if(DYNAMIC_TRUNK_PORT_IS_ADDED(id,index)){
					slot = DYNAMIC_TRUNK_PORT_SLOT_NUM(id,index);
					port = DYNAMIC_TRUNK_PORT_PORT_NUM(id,index);
					haveMore = TRUE;
					dbus_message_iter_append_basic (&iter,
										 DBUS_TYPE_UINT32,
										 &haveMore);/*if this port is set append a signal to dcli, notify dcli to get slot/port*/
					dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_BYTE,
									 &slot);
					dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_BYTE,
									 &port);
				}
			}
			haveMore = FALSE;				
			dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &haveMore);/* slot port append over, append a "FALSE" to dcli,means no more port for this trunk*/
		}
	}
	
	return reply;
}

int npd_dynamic_trunk_show_hardware_information
(
    unsigned short trunkId,
	NPD_DYNAMIC_TRUNK_SLOT_PORT  *DynamicTrunkArray,
	unsigned int     *numofPortHW
)
{
	unsigned int	 ret = TRUNK_RETURN_CODE_ERR_NONE;
	unsigned int     portnum = 0,portSum = 0;
	unsigned int     eth_g_index = 0;
    unsigned int     numOfEnabledMembers = 0;
	unsigned int     numOfDisabledMembers = 0;
	unsigned char    slot_no = 0;
    unsigned char    port_no = 0;
	NPD_DYNAMIC_TRUNK_MEMBER_STC  EnabledMembersArray[MAX_DYNAMIC_TRUNK_MEMBER];
	NPD_DYNAMIC_TRUNK_MEMBER_STC  DisabledMembersArray[MAX_DYNAMIC_TRUNK_MEMBER];   
	
    memset(EnabledMembersArray,0,sizeof(NPD_DYNAMIC_TRUNK_MEMBER_STC)*MAX_DYNAMIC_TRUNK_MEMBER);
	memset(DisabledMembersArray,0,sizeof(NPD_DYNAMIC_TRUNK_MEMBER_STC)*MAX_DYNAMIC_TRUNK_MEMBER);

	ret = nam_asic_show_dynamic_trunk_hwinfo(trunkId+MAX_STATIC_TRUNK_ID,&numOfEnabledMembers,&numOfDisabledMembers,EnabledMembersArray,DisabledMembersArray);

	if(TRUNK_RETURN_CODE_ERR_NONE == ret)
	  {
	  /*port in up status*/
	   for(portnum = 0;portnum<numOfEnabledMembers;portnum++)
		  {/*get port index*/
		   if(!npd_get_global_index_by_devport(EnabledMembersArray[portnum].devNum,EnabledMembersArray[portnum].portNum,&eth_g_index))
			  {/*get slot and port number according to index*/
			   if(!npd_eth_port_get_slotno_portno_by_eth_g_index(eth_g_index,&slot_no,&port_no))
			    {
				   DynamicTrunkArray[portnum].slotNum = slot_no;
				   DynamicTrunkArray[portnum].portNum = port_no;
				   DynamicTrunkArray[portnum].status = 'e';				   
			    }
			   else{return NPD_FAIL;}
			}
		   else{return NPD_FAIL;}
		}
      portSum = portnum;/*store disabled member after enabled member*/
	   /*port in down status*/
	  for(portnum = 0;portnum<numOfDisabledMembers;portnum++)
		  {/*get port index*/
		   if(!npd_get_global_index_by_devport(DisabledMembersArray[portnum].devNum,DisabledMembersArray[portnum].portNum,&eth_g_index))
			  {/*get slot and port number according to index*/
			   if(!npd_eth_port_get_slotno_portno_by_eth_g_index(eth_g_index,&slot_no,&port_no))
			   	{ 
				   DynamicTrunkArray[portSum].slotNum = slot_no;
				   DynamicTrunkArray[portSum].portNum = port_no;
				   DynamicTrunkArray[portSum].status = 'd';
				   portSum++;			  
			   	}
			   else{return NPD_FAIL;}
		   }
		   else{return NPD_FAIL;}
	   }
	}
	*numofPortHW = portSum;
    return NPD_SUCCESS;
}

int npd_dynamic_trunk_check_slot_port
	(
	NPD_DYNAMIC_TRUNK_SLOT_PORT *DynamicTrunkArray,
	unsigned int portSum
	)
{
    unsigned int i = 0;
	unsigned int flag = 0;
	for(i = 0;i<portSum;i++)
		{
	     if((0==DynamicTrunkArray[i].slotNum)||(0==DynamicTrunkArray[i].portNum))
		     {flag = 1;
		     break;}
		}
    return flag;
}

/*****************************************************************************
 *npd_dbus_dynamic_trunk_show_dynamic_trunk_hardware_information
 *
 * DESCRIPTION:
 *			dbus function for show port members in a dynamic trunk
 * INPUT:
 *			trunkId : uint16 -  the dynamic trunkId that want to show
 *							0  -  show dynamic trunk port list
 *							else - show dynamic trunk [trunkId] port list
 * OUTPUT:
 *                1) the trunkId input is 0:
 *					ret : uint32	-  return value
 *                                                 TRUNK_RETURN_CODE_ERR_HW
 *                                                 TRUNK_RETURN_CODE_SLOT_PORT_FAIL
 *								TRUNK_RETURN_CODE_TRUNK_ERR_NONE
 *                id	: uint32   -  the trunkId or 0(no more dynamic trunk to show)
 *							0  -  no more dynamic trunk to show
 *							else - show dynamic trunk[id]
 *						while(id){
 *                                                for(numofPortHW){
 *											slot : uint8  -  slot no
 *											port : uint8  -  port no
 *											status:char  - e:enabled/d:disabled
 *                                                         }
 *								}
 *								id : uint32  -	the trunkId or 0(no more dynamic trunk to show)
 *						}
 *			2) the trunkId input is not 0:
 *			the trunkId input range in <1-8>:
 *					ret : uint32	-  return value
 *								TRUNK_RETURN_CODE_TRUNKID_OUT_OF_RANGE
 *								TRUNK_RETURN_CODE_TRUNK_NOTEXISTS
 *                                                 TRUNK_RETURN_CODE_ERR_HW
 *                                                 TRUNK_RETURN_CODE_SLOT_PORT_FAIL
 *								TRUNK_RETURN_CODE_TRUNK_ERR_NONE
 *						if(TRUNK_RETURN_CODE_TRUNK_ERR_NONE == ret){ 
 *                                                for(numofPortHW){
 *											slot : uint8  -  slot no
 *											port : uint8  -  port no
 *											status:char  - e:enabled/d:disabled
 *                                                         }
 *							}
 * RETURN:
 *			NULL  -  get args failed 
 *			reply  -  else
 * NOTE:
 *
 *****************************************************************************/
DBusMessage * npd_dbus_dynamic_trunk_show_dynamic_trunk_hardware_information
(	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	DBusMessage*     reply = NULL;
	DBusMessageIter	 iter;
	DBusError        err;
	unsigned short	 trunkId = 0,id = 0;
	unsigned int     i=0;
	unsigned int     haveMore = FALSE;
	unsigned int     numofPortHW = 0;
	unsigned int	 ret = TRUNK_RETURN_CODE_ERR_NONE;
	NPD_DYNAMIC_TRUNK_SLOT_PORT   DynamicTrunkArray[MAX_DYNAMIC_TRUNK_MEMBER];
	memset(DynamicTrunkArray,0,sizeof(NPD_DYNAMIC_TRUNK_SLOT_PORT)*MAX_DYNAMIC_TRUNK_MEMBER);

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT16,&trunkId,
		DBUS_TYPE_INVALID))) {
		 syslog_ax_trunk_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_trunk_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	
	syslog_ax_trunk_dbg("show trunk %d. \n",trunkId);
	reply = dbus_message_new_method_return(msg);	
	dbus_message_iter_init_append (reply, &iter);
    /*show list*/
	if(!trunkId){
		for(id = MIN_DYNAMIC_TRUNKID; id <= MAX_DYNAMIC_TRUNKID; id++){			
			if(npd_dynamic_trunk_check_exists(id)){
				dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT16,&id);
				/*get slot and port num*/
				if (NPD_FAIL == npd_dynamic_trunk_show_hardware_information(id,DynamicTrunkArray,&numofPortHW)){
			   	    npd_syslog_err("get slot or port number from nam failed\n");
			   	    ret = TRUNK_RETURN_CODE_ERR_HW;
				}									 			
			    /*check slot or port num, TRUE means slot and port num get failed*/
				else if(TRUE == npd_dynamic_trunk_check_slot_port(DynamicTrunkArray,numofPortHW)){
					npd_syslog_err("after checking, slot or port number of Dynamic Trunk %d is fault.\n",id);
					ret = TRUNK_RETURN_CODE_SLOT_PORT_FAIL;
				}  
				else{ret = TRUNK_RETURN_CODE_ERR_NONE;}
				/*append a return value, if no error then append slot/port list*/
				dbus_message_iter_append_basic (&iter,
										 DBUS_TYPE_UINT32,
										 &ret);				
				if(TRUNK_RETURN_CODE_ERR_NONE==ret){
				dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT16,&id);
			    dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&numofPortHW);				   
		        for(i = 0;i<numofPortHW;i++)
					{
		             dbus_message_iter_append_basic(&iter,DBUS_TYPE_BYTE,&DynamicTrunkArray[i].slotNum);
		             dbus_message_iter_append_basic(&iter,DBUS_TYPE_BYTE,&DynamicTrunkArray[i].portNum);
					 dbus_message_iter_append_basic(&iter,DBUS_TYPE_BYTE,&DynamicTrunkArray[i].status);
		        	}
				}	
			}
		}
		id = 0;/*0 : end*/
		dbus_message_iter_append_basic(&iter,DBUS_TYPE_UINT16,&id);/*set 0 to dcli. means no more trunk would append to dcli */
	}
	
    /*show one*/
	else
		{
		id = trunkId;
		/*check trunk ID whether in range <1-8>*/
		if(!DYNAMIC_TRUNK_ID_IS_AVALIABLE(trunkId)){
			npd_syslog_err("dynamic trunk show trunk list, trunkId %d is out of range\n", trunkId);
			ret = TRUNK_RETURN_CODE_TRUNKID_OUT_OF_RANGE;
		}
		/*check trunk exist or not*/
		else if(!npd_dynamic_trunk_check_exists(id)){
				npd_syslog_dbg("show dynamic trunk port list failed, dynamic trunk %d not exists\n", id);
				ret = TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;
		}
		/*get slot and port num*/
		else if (NPD_FAIL == npd_dynamic_trunk_show_hardware_information(id,DynamicTrunkArray,&numofPortHW)){
	   	    npd_syslog_err("get slot or port number from nam failed\n");
	   	    ret = TRUNK_RETURN_CODE_ERR_HW;
		}									 			
	    /*check slot or port num, TRUE means slot and port num get failed*/
		else if(TRUE == npd_dynamic_trunk_check_slot_port(DynamicTrunkArray,numofPortHW)){
			npd_syslog_err("after checking, slot or port number of Dynamic Trunk %d is fault.\n",id);
			ret = TRUNK_RETURN_CODE_SLOT_PORT_FAIL;
		}  
		else{ret = TRUNK_RETURN_CODE_ERR_NONE;}
		/*append a return value, if no error then append slot/port list*/
		dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &ret);
		if(TRUNK_RETURN_CODE_ERR_NONE==ret){
	    dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&numofPortHW);				   
        for(i = 0;i<numofPortHW;i++)
			{
             dbus_message_iter_append_basic(&iter,DBUS_TYPE_BYTE,&DynamicTrunkArray[i].slotNum);
             dbus_message_iter_append_basic(&iter,DBUS_TYPE_BYTE,&DynamicTrunkArray[i].portNum);
			 dbus_message_iter_append_basic(&iter,DBUS_TYPE_BYTE,&DynamicTrunkArray[i].status);
        	}
		}			    
	}
	return reply;	
}
int npd_dynamic_trunk_allow_refuse_vlan_update
(
	unsigned short trunkId,
	unsigned int vlan_count,
	unsigned short vid[],
	unsigned char tagMode,
	unsigned char isAllow
)
{
	unsigned short vlanid = 0;
	int i = 0;
	for(i=0;i<vlan_count;i++)
	{
		vlanid = vid[i];
		syslog_ax_trunk_dbg("trunkId = %d,tagMode = %d,isAllow = %d\n",trunkId,tagMode,isAllow);
		syslog_ax_trunk_dbg("vlan id is %d\n",vlanid);
		if(isAllow)
		{
			if(tagMode)
			{
				dynamin_trunk_allow_vlan_tag[trunkId-1][vlanid-1] = 1;
				syslog_ax_trunk_dbg("dynamin_trunk_allow_vlan_tag[%d][%d] = 1",trunkId-1,vlanid-1);
			}
			else
			{
				dynamin_trunk_allow_vlan_untag[trunkId-1][vlanid-1] = 1;
				syslog_ax_trunk_dbg("dynamin_trunk_allow_vlan_untag[%d][%d] = 1",trunkId-1,vlanid-1);
			}
		}
		else
		{
			if(tagMode)
			{
				dynamin_trunk_allow_vlan_tag[trunkId-1][vlanid-1] = 0;
			}
			else
			{
				dynamin_trunk_allow_vlan_untag[trunkId-1][vlanid-1] = 0;
			}
		}
	}
	return TRUNK_RETURN_CODE_ERR_NONE;
}
int npd_dynamic_trunk_allow_vlan
(
	unsigned short trunkId,
	unsigned int vlan_count,
	unsigned short vid[],
	unsigned char tagMode
)
{
	unsigned char  isTagged = 0, hasAllow = NPD_FALSE;
	unsigned int i = 0, j = 0, mbr_count = 0, eth_g_index[24] = {0};
	unsigned int  ret = TRUNK_RETURN_CODE_ERR_NONE;
	unsigned char devNum = 0;
	unsigned char virPortNum = 0;
	unsigned short pvid = 0;
	unsigned char needJoin = 0;

	/*check if vlan has already allowed by this trunk*/
	for(i=0;i<vlan_count;i++)
	{
		ret = npd_vlan_check_contain_trunk(vid[i], trunkId, &isTagged);
		if(NPD_TRUE == ret )
		{
			syslog_ax_trunk_err("vlan %d has trunk %d member already.\n",vid[i],trunkId);
			return VLAN_RETURN_CODE_TRUNK_EXISTS;
		}
	}
	/*check this trunk is untag member of other vlans*/
	for(i=0;i<vlan_count;i++) 
	{
		if(0 == tagMode && 1 != vid[i]) 
		{
			for(i=2;i<NPD_VLAN_NUMBER_MAX ;i++) 
			{
				for(j=0;j<vlan_count;j++) 
				{
					if(VLAN_RETURN_CODE_VLAN_EXISTS == npd_check_vlan_exist(i) && i != vid[j]) 
					{
						ret = npd_vlan_check_trunk_membership(i, trunkId, NPD_FALSE);
						if(NPD_TRUE == ret ) 
						{
							syslog_ax_vlan_dbg("Trunk %d is untagged member of vlan %d\n",trunkId,i);
							return VLAN_RETURN_CODE_TRUNK_CONFLICT;
						}
					}
				}
			}

			if(NPD_TRUE == npd_trunk_member_port_index_get_all(trunkId,eth_g_index,&mbr_count)) 
			{
				/*if trunk add to vlan as untagged, delete the trunk from default vlan 1*/
				npd_vlan_trunk_member_del(1, trunkId,tagMode);
				npd_trunk_vlan_list_delfrom(trunkId,1,tagMode);			

				for(i = 0; i < mbr_count; i ++) 
				{
					syslog_ax_vlan_dbg("delete trunk %d port %d from vlan %d %s.\n",
										trunkId,eth_g_index[i],1,(tagMode)?"tag":"untag");
					npd_vlan_interface_port_del_for_dynamic_trunk(1, eth_g_index[i], tagMode);
				}
			}
		}
	}
	
	/*modify trunk_vlan_list and vlan_trunk_list and add all port member of the trunk to vlan.*/
	if(NPD_TRUE == npd_trunk_member_port_index_get_all(trunkId,eth_g_index,&mbr_count))
	{
		syslog_ax_trunk_dbg("######## trunk %d has %d members \n",trunkId,mbr_count);
		for(i=0;i<vlan_count;i++) 
		{
			ret = npd_trunk_check_vlan_allowship(trunkId,vid[i]);
			if(NPD_TRUE == ret) 
			{
				syslog_ax_trunk_dbg("trunk %d has allow vlan %d!\n",trunkId,vid[i]);
				hasAllow = NPD_TRUE;
				continue;
			}
			else 
			{
				/*add vlanNode to trunk_vlan_list_node*/
				npd_trunk_vlan_list_addto(trunkId,vid[i],tagMode);

				/*add trunkNode to vlan_trunk_list_node*/
				npd_vlan_trunk_member_add(vid[i],trunkId,tagMode);

				/*add trunk member port to vlan*/
				for(j=0;j<mbr_count;j++) 
				{
					ret = npd_vlan_check_contain_port(vid[i],eth_g_index[j], &isTagged);
					if(NPD_TRUE == ret && tagMode == isTagged)
					{
						continue;
					}
					else if(NPD_TRUE == ret && tagMode != isTagged) 
					{
						#if 1
						ret = npd_vlan_interface_port_del_for_dynamic_trunk(vid[i],eth_g_index[j],isTagged);
						ret = npd_vlan_interface_port_add_for_dynamic_trunk(vid[i],eth_g_index[j],tagMode);
						#endif
						if(VLAN_RETURN_CODE_ERR_NONE != ret && VLAN_RETURN_CODE_PORT_EXISTS != ret && NPD_TRUE != ret)
						{
							syslog_ax_trunk_dbg("port %d add to vlan %d error, tagMode %d. ret = %#x.\n",eth_g_index[j], tagMode, vid[i],ret);
							/*if add error,delete the node add before*/
							npd_trunk_vlan_list_delfrom(trunkId,vid[i],tagMode);

							npd_vlan_trunk_member_del(vid[i],trunkId,tagMode);
							return TRUNK_RETURN_CODE_ALLOW_ERR;
						}
					}
					else if(NPD_FALSE == ret) 
					{
						ret = npd_vlan_interface_port_add_for_dynamic_trunk(vid[i],eth_g_index[j],tagMode);
						if(VLAN_RETURN_CODE_ERR_NONE != ret && VLAN_RETURN_CODE_PORT_EXISTS != ret && NPD_TRUE != ret)
						{
							syslog_ax_trunk_dbg("port %d add to vlan %d error. ret = %#x.\n",eth_g_index[j],vid[i],ret);
							/*if add error,delete the node add before*/
							npd_trunk_vlan_list_delfrom(trunkId,vid[i],tagMode);

							npd_vlan_trunk_member_del(vid[i],trunkId,tagMode);
							return TRUNK_RETURN_CODE_ALLOW_ERR;
						}
					}
				}
			}
		}
	}
	else 
	{
		/*trunk has not any port member add to vlan*/
		return TRUNK_RETURN_CODE_NO_MEMBER;
	}

	if(NPD_TRUE == hasAllow)
	{
		/*trunk has already been the member of this vlan*/
		return TRUNK_RETURN_CODE_ALLOW_VLAN;
	}
	return TRUNK_RETURN_CODE_ERR_NONE;
}
int npd_dynamic_trunk_refuse_vlan
(
	unsigned short trunkId,
	unsigned int vlan_count,
	unsigned short vid[],
	unsigned char tagMode
)
{
	unsigned char  isTagged = 0,notAllow = NPD_FALSE;
	unsigned int i,j,mbr_count = 0,eth_g_index[24] = {0};
	unsigned int  ret = TRUNK_RETURN_CODE_ERR_NONE;
	unsigned char devNum = 0;
	unsigned char virPortNum = 0;

	/*every vlan enterd by DCLI exist*/
	if(NPD_TRUE == npd_trunk_member_port_index_get_all(trunkId,eth_g_index,&mbr_count))
	{
		for(i=0;i<vlan_count;i++) {
			ret = npd_trunk_check_vlan_allowship(trunkId, vid[i]);
			if(NPD_FALSE == ret) {
				syslog_ax_trunk_err("trunk %d has refused vlan %d!\n",trunkId,vid[i]);
				notAllow = NPD_TRUE;
				continue;
			}
			else {
				/*delete trunk_vlan_list node*/
				if(TRUNK_RETURN_CODE_VLAN_TAGMODE_ERR == npd_trunk_vlan_list_delfrom(trunkId,vid[i],tagMode)){
					return TRUNK_RETURN_CODE_VLAN_TAGMODE_ERR;
				}

				/*delete trunkNode from vlan trunklist*/
				/*if(NPD_VLAN_EXISTS == npd_check_vlan_exist(vid[i])) {*/
				npd_vlan_trunk_member_del(vid[i],trunkId,tagMode);

				for(j= 0;j<mbr_count;j++) {
					/*delete trunk member port to vlan*/
					ret = npd_vlan_check_contain_port(vid[i],eth_g_index[j], &isTagged);
					if(NPD_TRUE == ret ) {
						ret = npd_vlan_interface_port_del_for_dynamic_trunk(vid[i], eth_g_index[j],isTagged);
						if(VLAN_RETURN_CODE_ERR_NONE != ret) { 
							return TRUNK_RETURN_CODE_REFUSE_ERR;
						}
					}
				}
				/*add trunk to default vlan 1 as untagged if refuse this trunk as untagged*/
				if(NPD_FALSE == tagMode) {
					if(NPD_TRUE == npd_trunk_member_port_index_get_all(trunkId,eth_g_index,&mbr_count)) {
						npd_vlan_trunk_member_add(1, trunkId,tagMode);
						npd_trunk_vlan_list_addto(trunkId,1, tagMode);		
						for(i = 0; i<mbr_count;i++) {
							syslog_ax_vlan_dbg("add trunk %d port %#x to vlan %d %s\n",
												trunkId,eth_g_index[i],1,(tagMode)?"tag":"untag");
							npd_vlan_interface_port_add_for_dynamic_trunk(1, eth_g_index[i], tagMode);
						}
					}
				}
			}
		}
	}
	if(NPD_TRUE == notAllow)
		return TRUNK_RETURN_CODE_NOTALLOW_VLAN;
	return TRUNK_RETURN_CODE_ERR_NONE;
}

DBusMessage * npd_dbus_dynamic_trunk_allow_refuse_vlan
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned char  isAllow = 0, tagMode = 0;
	unsigned short trunkId = 0;
	static unsigned short vid[NPD_MAX_VLAN_ID] = {0}, nonVid[NPD_MAX_VLAN_ID] = {0};
	unsigned int i = 0, vCount = 0, nonVCount = 0;
	unsigned int ret = TRUNK_RETURN_CODE_ERR_NONE;
	
	syslog_ax_trunk_dbg("Entering trunk allow/refuse vlans..\n");
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
							DBUS_TYPE_BYTE,&isAllow,
							DBUS_TYPE_BYTE,&tagMode,
							DBUS_TYPE_UINT32,&vCount,
							DBUS_TYPE_UINT16,&vid,
							DBUS_TYPE_UINT16,&trunkId,
							DBUS_TYPE_INVALID)))
	{
		syslog_ax_trunk_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) 
		{
			syslog_ax_trunk_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	for(i=0;i<vCount;i++)
	{
		ret = npd_check_vlan_exist(vid[i]);
		if(VLAN_RETURN_CODE_VLAN_EXISTS != ret)
		{
			nonVid[nonVCount++] = vid[i];
			continue;
		}
	}
	if(nonVCount>0) 
	{
		ret = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;

		reply = dbus_message_new_method_return(msg);
		
		dbus_message_iter_init_append (reply, &iter);
		
		dbus_message_iter_append_basic (&iter,
										 DBUS_TYPE_UINT32, 
										 &ret);

		return reply;
	}
	npd_syslog_dbg("trunkId is %d\n",trunkId);
	/*no need to check trunk existing or NOT*/
	if(isAllow)
		ret = npd_dynamic_trunk_allow_vlan(trunkId+MAX_STATIC_TRUNK_ID,vCount,vid,tagMode);
	else
		ret = npd_dynamic_trunk_refuse_vlan(trunkId+MAX_STATIC_TRUNK_ID,vCount,vid,tagMode);
	if(ret == TRUNK_RETURN_CODE_ERR_NONE)
	{
		npd_dynamic_trunk_allow_refuse_vlan_update(trunkId,vCount,vid,tagMode,isAllow);
		npd_syslog_dbg("update dynamic trunk allow refuse vlan info successfully !!!\n");
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32, 
									 &ret);
	return reply;
	
}

/*********************************************************************
 * npd_dynamic_trunk_init
 * DESCRIPTION:
 *			init asic for dynamic trunk
 * INPUT:
 *			NONE
 * OUTPUT:
 *			NONE
 * RETURN:
 *			NPD_SUCCESS  -  init success
 *			NPD_FAIL    -    init failed 
 * NOTE:
 *
 *********************************************************************/
int npd_dynamic_trunk_init(){
	int ret = 0;
	int devNum = 0;
	int devCount = 0;
	devCount = nam_asic_get_instance_num();
	for(devNum = 0; devNum < devCount; devNum++){
		ret = nam_dynamic_trunk_lacp_trap_init(devNum);
		if(NPD_SUCCESS != ret){
			npd_syslog_err(" nam dynamic trunk lacp init failed,devNum %d\n", devNum);
			return NPD_FAIL;
		}
	}	
	/*do it at npd...?*/
	nam_thread_create("DynamicTrunkNetlink",(void *)npd_dynamic_trunk_netlink_thread_main,NULL,NPD_TRUE,NPD_FALSE);
	return NPD_SUCCESS;
}


int npd_dynamic_trunk_netlink_thread_main(void)
{
	struct sockaddr_nl src_addr, dest_addr;
	struct nlmsghdr *nlh = NULL;
	struct iovec iov;
	int sock_fd = 0;
	struct msghdr msg;
	char nameb[MAX_IFNAME_LEN + 1];
	char names[MAX_IFNAME_LEN + 1];
	unsigned int trunkId = 0;
	unsigned char slot = 0, port = 0;
	unsigned int eth_g_index = 0;
	unsigned char devNum = 0, portNum = 0;
	struct ad_event *nl_msg = NULL;
	int ret = 0;
	
	/* Initialize data field */
	npd_init_tell_whoami("DynamicTrunkNetlink", 0);
	memset(&src_addr, 0, sizeof(src_addr));
	memset(&dest_addr, 0, sizeof(dest_addr));
	memset(&iov, 0, sizeof(iov));
	memset(&msg, 0, sizeof(msg));
	/* Create netlink socket use NETLINK_SELINUX(7) */
	if ((sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE)) < 0) {
		npd_syslog_err("dynamic trunk netlink thread socket failed \n");
		return -1;
	}

	/* Fill in src_addr */
	src_addr.nl_family = AF_NETLINK;
	src_addr.nl_pid = getpid(); /* Thread method */
	src_addr.nl_groups = GRP_ID;
	
	if (bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr)) < 0) {
		npd_syslog_err("dynamic trunk netlink thread bind failed \n");
		return -1;
	}
	
	/* Fill in dest_addr */
	dest_addr.nl_pid = 0; /* From kernel */
	dest_addr.nl_family = AF_NETLINK;
	dest_addr.nl_groups = GRP_ID;

	/* Initialize buffer */
	if((nlh = (struct nlmsghdr*)malloc(NLMSG_SPACE(MAX_PAYLOAD))) == NULL) {
		npd_syslog_err("dynamic trunk netlink thread malloc failed \n");
		close(sock_fd);
		return -1;
	}

	while (1) {
		memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
		iov.iov_base = (void *)nlh;
		iov.iov_len = NLMSG_SPACE(MAX_PAYLOAD);
		msg.msg_name = (void *)&dest_addr;
		msg.msg_namelen = sizeof(dest_addr);
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;
	
		/* Recv message from kernel */
		ret = recvmsg(sock_fd, &msg, 0); /* code optimize:Unchecked return value. zhangdi@autelan.com 2013-01-18 */
		if(ret <= 0)
		{
			npd_syslog_err("dynamic trunk netlink recv msg error %s\n", strerror(errno));
			continue;
		}		

		nl_msg = (struct ad_event*)NLMSG_DATA(nlh);/*check nl_msg*/
		npd_syslog_dbg("dynamic trunk netlink thread get flag %x \n", nl_msg->flag);
		if(DYNAMIC_TRUNK_3AD_CHANE_IF_MSG == nl_msg->flag){
			memset(nameb, 0, MAX_IFNAME_LEN + 1);
			memset(names, 0, MAX_IFNAME_LEN + 1);
			if_indextoname(nl_msg->bond_ifindex, nameb);
			if_indextoname( nl_msg->slave_ifindex, names);
			npd_syslog_dbg("bond_ifname %s, slave_ifname %s, state %d \n\n",
					nameb, names, nl_msg->state);
			if(!strncmp("trunk", nameb, 5)){
				trunkId = strtoul(nameb + 5, NULL, 10);
				if(!strncmp("eth", names, 3)){
					slot = port = 0;
					if(NPD_SUCCESS != npd_dynamic_trunk_parse_slot_port_no(names + 3, &slot, &port)){
						npd_syslog_err("dynamic trunk eth interface name %s parse slot port no failed \n", names);
					}
					else{						
						eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot,port);
						
						if(!(nl_msg->state)/*MICRO*/){
							npd_syslog_dbg("dynamic trunk netlink add port to trunk %d \n", trunkId + MAX_STATIC_TRUNK_ID);
#if DYNAMIC_ONLY_HW							
							if(!npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum)){
								/*ret*/ nam_asic_trunk_ports_add(devNum, trunkId, portNum, 1);
							}
							/*else*/
#else
							ret = npd_trunk_port_add_for_dynamic_trunk(trunkId + MAX_STATIC_TRUNK_ID, eth_g_index);
							if(TRUNK_RETURN_CODE_ERR_NONE != ret){
								npd_syslog_err("dynamic trunk netlink add port %d/%d to trunk%d failed, ret %#x", slot, port, trunkId, ret);
							}
#endif
						}
						else if(1/*MICRO*/ == (nl_msg->state)){
							npd_syslog_dbg("dynamic trunk netlink del port from trunk %d \n", trunkId + MAX_STATIC_TRUNK_ID);
#if DYNAMIC_ONLY_HW
							if(!npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum)){
								nam_asic_trunk_ports_del(devNum, trunkId, portNum);
							}
#else
							ret = npd_trunk_port_del_for_dynamic_trunk(trunkId + MAX_STATIC_TRUNK_ID, eth_g_index);
							if(TRUNK_RETURN_CODE_ERR_NONE != ret){
								npd_syslog_err("dynamic trunk netlink del port %d/%d from trunk%d failed, ret %#x", slot, port, trunkId, ret);
							}
#endif
						}
						else {
							npd_syslog_err("dynamic trunk netlink state error,state %d\n",nl_msg->state);
						}
					}
				}
			}
		}
	}

	close(sock_fd);

	return 0;
}
/******************************************************************************
 * npd_dynamic_trunk_parse_slot_port_no
 * DESCRIPTION:
 *				pase slot port string such as "1-12" or "1/10" to slot no and port no
 * INPUT:
 *				str : string - eth-port string 
 * OUTPUT:
 *				slotno : uint8   -  slot no
 *				portno : uint8   -  port no
 * RETURN:
 *				NPD_SUCCESS   -  parse success
 *				NPD_FAIL	  -  parse failed
 * NOTE:
 *
 *******************************************************************************/
int npd_dynamic_trunk_parse_slot_port_no(char *str,unsigned char *slotno,unsigned char *portno) {
	char *endptr = NULL;
	char *endptr2 = NULL;

	if ((!str)||(!slotno)||(!portno)){ 
		npd_syslog_err(" null pointer %s %s %s \n", str ? "" : "str",\
			slotno ? "" : "slotno", portno ? "" : "portno");
		return NPD_FAIL;
	}
	
	*portno = strtoul(str,&endptr,10);
	if (endptr) {
		if ((SLOT_PORT_SPLIT_DASH == endptr[0])||(SLOT_PORT_SPLIT_SLASH == endptr[0])) {
            *slotno = *portno;
			*portno = strtoul((char *)&(endptr[1]),&endptr2,10);
			if('\0' == endptr2[0]) {
				return NPD_SUCCESS;
			}
			else {
				npd_syslog_err(" unknow char %d %c \n", endptr2[0],endptr2[0]);
				return NPD_FAIL;
			}
		}
		if ('\0' == endptr[0]) {
			*slotno = 0;
			return NPD_SUCCESS;
		}		
		npd_syslog_err("endptr is not null str %s endptr %s\n", str, endptr);
	}
	npd_syslog_err("endptr is %s SLOT_PORT_SPLIT_DASH %c SLOT_PORT_SPLIT_SLASH %c  \n", \
		endptr ? "not" : "null", SLOT_PORT_SPLIT_DASH, SLOT_PORT_SPLIT_SLASH);
	return NPD_FAIL;	
}
/*********************************************************************
 * npd_dynamic_trunk_slave_port_trunkId_get
 * DESCRIPTION:
 *			npd dynamic trunk slave port trunkId get in software
 * INPUT:
 *			ethIfname : string - eth interface name 
 * OUTPUT:
 *			NONE
 * RETURN:
 *			trunkId : uint32   -  0 or trunkId
 *						  0  - if port is not added to any dynamic trunk
 *						  else -  dynamic trunkId
 * NOTE:
 *
 *********************************************************************/
unsigned int npd_dynamic_trunk_slave_port_trunkId_get
(
	unsigned char * ethIfname
)
{
	unsigned char slot = 0, port = 0;
	int trunkId = 0, index = 0;
	if(!ethIfname){
		return 0;
	}
	if(strncmp("eth", ethIfname, 3)){
		return 0;
	}
	if(NPD_SUCCESS != npd_dynamic_trunk_parse_slot_port_no(ethIfname + 3, &slot, &port)){
		return 0;
	}
	for(trunkId = MIN_DYNAMIC_TRUNKID; trunkId <= MAX_DYNAMIC_TRUNKID; trunkId++){
		for(index = 0; index < MAX_DYNAMIC_TRUNK_MEMBER; index++){
			if(DYNAMIC_TRUNK_PORT_IS_ADDED(trunkId, index) &&\
				(port == DYNAMIC_TRUNK_PORT_PORT_NUM(trunkId,index)) &&\
				(slot == DYNAMIC_TRUNK_PORT_SLOT_NUM(trunkId,index))){
				return trunkId;
			}
		}
	}
	return 0;
}
#ifdef __cplusplus
}
#endif
