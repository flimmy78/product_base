#ifdef __cplusplus
extern "C"
{
#endif


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <linux/if_vlan.h>
#include <linux/sockios.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <errno.h>
#include <linux/tipc.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <dbus/dbus.h>
#include <sys/syslog.h>
#include <sys/resource.h>
#include <net/if.h>

#include "sysdef/returncode.h"
#include "cvm/ethernet-ioctl.h"
#include "dbus/sem/sem_dbus_def.h"
#include "sem_dbus.h"
#include "sem_common.h"
#include "sem/sem_tipc.h"
#include "product/board/board_feature.h"
#include "product/product_feature.h"
#include "sem_eth_port.h"
#include "sem_eth_port_dbus.h"
#include "sem_intf.h"

/**********************************************************************************
*  sem_intf_execute_cmd
*
*  DESCRIPTION:
*	   this routine call shell command
*
*  INPUT:
*	   command -- command name
*	   cmd_param
*
*  OUTPUT:
*
*
*  RETURN:
*	   -1	- error
*	   0   - success and not status change
*	   pid - the pid whose status changed
*
**********************************************************************************/
int sem_intf_execute_cmd(const char* command, const char** cmd_param) 
{
	int ret;
	pid_t pid;
	int status;
	int errno;
	/* Call fork(). */
	pid = fork();

	if (pid < 0) {
		/* Failure of fork(). */
		sem_syslog_dbg("Can't fork: %s\n", strerror(errno));
		exit(1);
	}
	else if (pid == 0)
	{
		/* This is child process. */
		ret = execvp(command, (char **)cmd_param);
		/* When execlp suceed, this part is not executed. */
		sem_syslog_dbg("Can't execute %s: %s\n", command, strerror(errno));
		exit(1);
	}
	else
	{
		/* This is parent. */
		ret = wait4(pid, &status, 0, NULL);
		sem_syslog_dbg("%s: wait4 exit code %#x\n", __func__,WEXITSTATUS(status));
		return WEXITSTATUS(status);
	}

	return ret;
}

/**********************************************************************************
*  sem_intf_set_intf_qinq_type
*
*  DESCRIPTION:
*      this routine call sem_intf_execute_cmd set vlan dev qinq type
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
int sem_intf_set_intf_qinq_type(unsigned char *intfName,unsigned char *type) 
{
	int   ret  = 0;
	char *cmd[16] = {0};
	unsigned int typeValue = 0;
#ifndef ETH_P_ARP
#define ETH_P_ARP  0x0806
#define ETH_P_IP   0x0800
#define ETH_P_IPV6 0x86DD
#endif
	if((!intfName)||(!type))
	{
		return COMMON_RETURN_CODE_NULL_PTR;
	}
	typeValue = strtoul(type,NULL,0);
	if(((typeValue)&&(typeValue<1536))||\
		(ETH_P_ARP == typeValue)||\
		(ETH_P_IP == typeValue)||\
		(ETH_P_IPV6 == typeValue))
	{
		return COMMON_RETURN_CODE_BADPARAM;
	}
	cmd[0] = "vconfig";
	cmd[1] = "set_qinq_type";
	cmd[2] = intfName;
	cmd[3] = type;
	ret = sem_intf_execute_cmd("vconfig", (const char **)cmd);
	if(ret)
	{
		sem_syslog_dbg("set qinq type,ret %#x \n",ret);
		if(0x1b == ret)
		{
			return INTERFACE_RETURN_CODE_QINQ_TYPE_FULL;
		}
		return INTERFACE_RETURN_CODE_ERROR;
	}
	return INTERFACE_RETURN_CODE_SUCCESS;
}

unsigned int sem_intf_ifindex_get_by_ifname(unsigned char *ifName, unsigned int *ifIndex)
{
	int ret = INTERFACE_RETURN_CODE_SUCCESS;
	
	if(!(*ifIndex = if_nametoindex(ifName))){
		ret = INTERFACE_RETURN_CODE_ERROR;
	}

	return ret;
}

int sem_intf_set_intf_state(unsigned char * ifname,unsigned int state)
{
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
	ret = sem_intf_execute_cmd("ifconfig", (const char **)cmd);

	return ret;
}

int sem_intf_create_subif
(
	int ifnameType,
    unsigned char slot,
    unsigned char local_port_no,
    unsigned int    vid,
    unsigned int    vid2
) 
{
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
	
    ret |= sem_intf_set_intf_state(name,UP);

    if (0 != vid2) {/*create qinq subif parent interface is subif*/
        sprintf(newName, "%s.%d", newName, vid);
        sprintf(cvid, "%d", vid2);
        ret |= sem_intf_set_intf_state(newName,UP);
    }
    memset(cmd, 0, 16);
    cmd[0] = "vconfig";
    cmd[1] = "add";
	cmd[2] = newName;
    cmd[3] = cvid;
    ret |= sem_intf_execute_cmd("vconfig", (const char **)cmd);
	
    sprintf(newName, "%s.%d",newName, (0 == vid2) ? vid : vid2);/*set the name as subif or qinq subif*/
	sprintf(name,"%s",newName);

    ret |= sem_intf_set_intf_state(name,UP);
	
	if(ret){
		return SEM_COMMAND_FAILED;
	}
    return SEM_COMMAND_SUCCESS;

}

int sem_intf_del_subif(unsigned char *pname, unsigned int vid) 
{
    int   ret  = 0;
    char name[16] = {0};
    char *cmd[16] = {0};
    if (NULL == pname) 
		return SEM_COMMAND_FAILED;
    sprintf(name, "%s.%d", pname, vid);

    cmd[0] = "vconfig";
    cmd[1] = "rem";
    cmd[2] = name;
    ret = sem_intf_execute_cmd("vconfig", (const char **)cmd);
	if(ret){
		return SEM_COMMAND_FAILED;
	}
    return SEM_COMMAND_SUCCESS;
}

DBusMessage * sem_dbus_create_sub_intf(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{

    DBusMessage* reply;
    DBusMessageIter  iter;
    DBusError err;

    unsigned int ret = SEM_COMMAND_SUCCESS;
    unsigned int vid = 0, vid2 = 0;
    unsigned int eth_g_index = 0, ifIndex = ~0UI;
    unsigned char slot = 0, local_port_no = 0;
    unsigned int rc = FALSE;
    unsigned char pname[21] = {0};
	unsigned char * ifnamePrefix = NULL;
	int ifnameType = 0;
	unsigned char ifname[MAX_IFNAME_LEN + 1] = {0};

    sem_syslog_dbg("sem_dbus_create_sub_intf\n"); 

    dbus_error_init(&err);

    if (!(dbus_message_get_args(msg, &err,
                                DBUS_TYPE_UINT32, &ifnameType,
                                DBUS_TYPE_BYTE, &slot,
                                DBUS_TYPE_BYTE, &local_port_no,
                                DBUS_TYPE_UINT32, &vid,
                                DBUS_TYPE_UINT32, &vid2, /*for qinq subif*/
                                DBUS_TYPE_INVALID))) {
        sem_syslog_dbg("Unable to get input args ");
        if (dbus_error_is_set(&err)) {
            sem_syslog_dbg("%s raised: %s", err.name, err.message);
            dbus_error_free(&err);
        }
        return NULL;
    }

	switch(ifnameType)
	{
    case CONFIG_INTF_ETH:
		ifnamePrefix = "eth";
		break;
	case CONFIG_INTF_MNG:
		ifnamePrefix = "mng";
		break;
	default :
		return NULL;
	}	
	
    if (0 == vid) {
        ret = SEM_WRONG_PARAM;
    }


	
    if(vid2 != 0)
    {
	    sprintf(pname,"%s%d-%d.%d", ifnamePrefix, slot, local_port_no, vid);
		if(!if_nametoindex(pname))
			ret = SEM_COMMAND_FAILED;
		else {
    		sprintf(ifname,"%s.%d", pname, vid2);
            sem_syslog_dbg("Create QinQ %s\n",ifname); 
    		if(!if_nametoindex(ifname))
        	{
                ret = sem_intf_create_subif(ifnameType,slot, local_port_no, vid, vid2);
        	}	
		}
    }else {
	    sprintf(pname,"%s%d-%d", ifnamePrefix, slot, local_port_no);
		if(!if_nametoindex(pname))
			ret = SEM_COMMAND_FAILED;
		else {
            sprintf(ifname,"%s.%d", pname, vid);
    		sem_syslog_dbg("Create Subif %s\n",ifname);
            if(!if_nametoindex(ifname))
        	{
                ret = sem_intf_create_subif(ifnameType,slot, local_port_no, vid, vid2);
        	}
		}
    }
    reply = dbus_message_new_method_return(msg);
    dbus_message_iter_init_append(reply, &iter);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &ret);

	return reply;
}

DBusMessage * sem_dbus_delete_sub_intf(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
    DBusMessage* reply;
    DBusMessageIter  iter;
    DBusError err;

    unsigned int ret = SEM_COMMAND_SUCCESS;
    unsigned int vid = 0, vid2 = 0;
    unsigned int eth_g_index = 0, ifIndex = ~0UI;
    unsigned char slot = 0, local_port_no = 0;
    unsigned int rc = FALSE;
    unsigned char pname[21] = {0};
	unsigned char * ifnamePrefix = NULL;
	int ifnameType = 0;
	unsigned char ifname[MAX_IFNAME_LEN + 1] = {0};

    dbus_error_init(&err);

    if (!(dbus_message_get_args(msg, &err,
                                DBUS_TYPE_UINT32, &ifnameType,
                                DBUS_TYPE_BYTE, &slot,
                                DBUS_TYPE_BYTE, &local_port_no,
                                DBUS_TYPE_UINT32, &vid,
                                DBUS_TYPE_UINT32, &vid2, /*for qinq subif*/
                                DBUS_TYPE_INVALID))) {
        sem_syslog_dbg("Unable to get input args ");
        if (dbus_error_is_set(&err)) {
            sem_syslog_dbg("%s raised: %s", err.name, err.message);
            dbus_error_free(&err);
        }
        return NULL;
    }
	
	switch(ifnameType)
	{
    case CONFIG_INTF_ETH:
		ifnamePrefix = "eth";
		break;
	case CONFIG_INTF_MNG:
		ifnamePrefix = "mng";
		break;
	default :
		return NULL;
	}
	
    if (0 == vid) {
        ret = SEM_WRONG_PARAM;
    }

	if (!LOCAL_SLOT_ISLEGAL(slot) || !LOCAL_PORT_ISLEGAL(local_port_no))
	{
		sem_syslog_dbg("wrong param:slot_no=%d port=%d , product->slotcount=%d local_board->port_num_on_panel=%d\n",
					slot, local_port_no, product->slotcount, local_board->port_num_on_panel);
		ret = SEM_WRONG_PARAM;
	}

    if(vid2 != 0)
    {
	    sprintf(pname,"%s%d-%d.%d", ifnamePrefix, slot, local_port_no, vid);		
        sem_syslog_dbg("Delete QinQ %s\n",ifname); 
		
		vid = vid2;
    }else {
        sprintf(pname,"%s%d-%d", ifnamePrefix, slot, local_port_no);
		sem_syslog_dbg("Delete Subif %s\n",ifname);
    }
	sprintf(ifname,"%s.%d", pname, vid);
	
    if(if_nametoindex(ifname) > 0)
	{
        ret = sem_intf_del_subif(pname, vid);
	}	
	
    reply = dbus_message_new_method_return(msg);
    dbus_message_iter_init_append(reply, &iter);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &ret);

	return reply;
}



