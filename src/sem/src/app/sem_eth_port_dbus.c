#ifdef __cplusplus
extern "C"
{
#endif
/*
  Network Platform Daemon Ethernet Port Management
*/

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
#include <sys/mman.h> 
#include <linux/tipc.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <dbus/dbus.h>
#include <sys/syslog.h>



#include "sysdef/returncode.h"
#include "sysdef/sem_sysdef.h"
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
#define SEM_SLOT_COUNT_PATH         "/dbm/product/slotcount"
#define SEM_SLOT_NO_PATH            "/dbm/local_board/slot_id"
#define SEM_LOCAL_PORTNUM_PATH      "/dbm/local_board/port_num_on_panel"
#define SEM_BOARD_TYPE_PATH         "/dbm/local_board/board_code"
#define ETH_OCTEON_PORT 0
#define	ETH_ASIC_PORT   1
extern board_fix_param_t *local_board;
extern product_fix_param_t *product;
extern struct eth_port_s **global_ethport;
extern struct eth_port_s **g_eth_ports;
extern struct global_ethport_s *start_fp[MAX_SLOT_COUNT];
extern DBusConnection *dcli_dbus_connection;
extern thread_index_sd_t thread_id_arr[MAX_SLOT_NUM];

DBusMessage * sem_dbus_config_port_interface_mode(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusMessageIter  iter;
	unsigned int	mode;
	int ret = INTERFACE_RETURN_CODE_SUCCESS;
	unsigned char	slot,port;
	int ifnameType = 0;
	unsigned int ifIndex = ~0UI;
	DBusError err;
	unsigned char ifname[50 + 1] = {0};

	dbus_error_init(&err);

	if (!(dbus_message_get_args( msg, &err,
									DBUS_TYPE_UINT32,&ifnameType,
									DBUS_TYPE_BYTE,&slot,
									DBUS_TYPE_BYTE,&port,
									DBUS_TYPE_INVALID))){
		sem_syslog_dbg("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				sem_syslog_dbg("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
		return NULL;
	}
	sem_syslog_dbg("Entering config port %d/%d type %d interface mode!\n", slot, port, ifnameType);

	unsigned int eth_l_index = port - 1;
	
	ret = INTERFACE_RETURN_CODE_NO_SUCH_PORT;
	if(LOCAL_SLOT_ISLEGAL(slot) && LOCAL_PORT_ISLEGAL(port))
	{
		sem_syslog_dbg("%d/%d\n",slot,port);
		if(local_board->port_arr[eth_l_index].cpu_or_asic == ASIC)
		{
            ret = INTERFACE_RETURN_CODE_UNSUPPORT_COMMAND;
		}else {
		    ret = INTERFACE_RETURN_CODE_SUCCESS;
		}
	}

	if (INTERFACE_RETURN_CODE_SUCCESS == ret)
	{
		if(ifnameType == CONFIG_INTF_MNG)
		{
			sprintf(ifname,"mng%d-%d",slot,port);
		}
		else
		{
			sprintf(ifname,"eth%d-%d",slot,port);
		}

		if(INTERFACE_RETURN_CODE_SUCCESS != sem_intf_ifindex_get_by_ifname(ifname,&ifIndex)){
			ret = INTERFACE_RETURN_CODE_INTERFACE_NOTEXIST;
		}
		
		if(INTERFACE_RETURN_CODE_SUCCESS == ret){
			sem_intf_set_intf_state(ifname,ETH_ATTR_LINKUP);
		}
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,	&ret);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,	&ifIndex);

	dbus_message_iter_init_append (reply, &iter);
	
	return reply;
}



DBusMessage * sem_dbus_config_ethport_qinq_type(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
    
#ifndef INTERFACE_NAMSIZ 
#define INTERFACE_NAMSIZ 20
#endif

        DBusMessage* reply;
        DBusMessageIter  iter;
        DBusError err;
        unsigned int ret = INTERFACE_RETURN_CODE_SUCCESS;
        unsigned char intfName[INTERFACE_NAMSIZ + 1] = {0};
        unsigned int ifname_index = 0;
        unsigned char slot = 0;
        unsigned char port = 0;
		unsigned char cpu_no = 0;
		unsigned char cpu_port_no = 0;
        unsigned int vid1 = 0;
        unsigned int vid2 = 0;
        unsigned char *type = NULL;
        int ifnameType = CONFIG_INTF_ETH;;

        dbus_error_init(&err);
        if (!(dbus_message_get_args(msg, &err,
                                    DBUS_TYPE_UINT32, &ifnameType,
                                    DBUS_TYPE_UINT16, &slot,
                                    DBUS_TYPE_UINT16, &port,
                                    DBUS_TYPE_UINT16, &cpu_no,
                                    DBUS_TYPE_UINT16, &cpu_port_no,
                                    DBUS_TYPE_UINT32, &vid1,
                                    DBUS_TYPE_UINT32, &vid2,
                                    DBUS_TYPE_STRING, &type,
                                    DBUS_TYPE_INVALID))) 
        {
            sem_syslog_err("Unable to get input args ");
            if (dbus_error_is_set(&err)) 
            {
                sem_syslog_err("%s raised: %s", err.name, err.message);
                dbus_error_free(&err);
            }
            return NULL;
        }

        if((!vid1)||((vid1)&&(vid2)))/*judgement for the given interface is a subintf or not*/
        {
            ret = INTERFACE_RETURN_CODE_UNSUPPORT_COMMAND;
        }
        else
        {

            if(INTERFACE_RETURN_CODE_SUCCESS == ret)
            {
                if((ifnameType == CONFIG_INTF_E) || (ifnameType == CONFIG_INTF_ETH))
                {
                    sprintf(intfName,"%s%d-%d.%d",ifnameType?"e":"eth",slot,port,vid1);
                }
                else if(ifnameType == CONFIG_INTF_VE)
                {
					sprintf(intfName,"%s%02d%c%d.%d","ve", slot,(cpu_no==1)?'f':'s',cpu_port_no,vid1);
                }
                else
                {
                    ret = INTERFACE_RETURN_CODE_UNSUPPORT_COMMAND;
                    sem_syslog_dbg("unsupport  ifnameType !\n");
                }
                if(INTERFACE_RETURN_CODE_SUCCESS == ret)
                {
                    if(INTERFACE_RETURN_CODE_SUCCESS != sem_intf_ifindex_get_by_ifname(intfName,&ifname_index))
                    {
    			        ret = INTERFACE_RETURN_CODE_INTERFACE_NOTEXIST;
    		        }
                    else
                    {
                        sem_syslog_dbg("ifname_index = %d\n",ifname_index);
                        ret = sem_intf_set_intf_qinq_type(intfName,type);
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

/****************************************************
 *
 * RETURN:
 *		INTERFACE_RETURN_CODE_SUCCESS	
 *		INTERFACE_RETURN_CODE_ERROR
 *		INTERFACE_RETURN_CODE_NO_SUCH_PORT
 *		INTERFACE_RETURN_CODE_UNSUPPORT_COMMAND
 *		INTERFACE_RETURN_CODE_ALREADY_THIS_MODE
 *   no interface 
 *		INTERFACE_RETURN_CODE_PORT_HAS_SUB_IF
 *	advanced-routing disable
 *		COMMON_RETURN_CODE_NULL_PTR
 *		INTERFACE_RETURN_CODE_PORT_HAS_SUB_IF 
 *		INTERFACE_RETURN_CODE_NAM_ERROR 
 *		INTERFACE_RETURN_CODE_CHECK_MAC_ERROR
 *		INTERFACE_RETURN_CODE_FDB_SET_ERROR
 *		INTERFACE_RETURN_CODE_FD_ERROR
 *		INTERFACE_RETURN_CODE_IOCTL_ERROR
 *		INTERFACE_RETURN_CODE_MAC_GET_ERROR
 *	advanced-routing enable
 *		INTERFACE_RETURN_CODE_DEFAULT_VLAN_IS_L3_VLAN
 *******************************************************/
DBusMessage * sem_dbus_config_port_mode(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusMessageIter  iter;
	unsigned int	mode;
	int ret = INTERFACE_RETURN_CODE_SUCCESS;
	unsigned char	slot,port;
	DBusError err;
	int ifnameType = 0;
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT32,&ifnameType,
									DBUS_TYPE_BYTE,&slot,
									DBUS_TYPE_BYTE,&port,
									DBUS_TYPE_UINT32,&mode,
									DBUS_TYPE_INVALID))) {
		sem_syslog_dbg("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				sem_syslog_dbg("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
		return NULL;
	}
	sem_syslog_dbg("\tEntering config port %d/%d type %d mode %d!\n", \
							slot, port, ifnameType, mode);
	/*todo:check port type***************;*/
	/*type=npd_check_port_mode_type( slot,port);*/
	ret = INTERFACE_RETURN_CODE_NO_SUCH_PORT;

	unsigned int eth_l_index = port - 1;
	if(LOCAL_SLOT_ISLEGAL(slot) && LOCAL_PORT_ISLEGAL(port))
	{
		sem_syslog_dbg("%d/%d\n",slot,port);
		if(local_board->port_arr[eth_l_index].cpu_or_asic == ASIC)
		{
            ret = INTERFACE_RETURN_CODE_UNSUPPORT_COMMAND;
		}else {
		    ret = INTERFACE_RETURN_CODE_SUCCESS;
		}
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,	&ret);
	dbus_message_iter_init_append (reply, &iter);
	
	return reply;
}

DBusMessage * sem_dbus_config_ethport(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusMessageIter  iter;
	unsigned char slot_no = 0, local_port_no = 0;
	unsigned int eth_g_index = 0, ret = 0, eth_l_index = 0;
	DBusError err;
	
	sem_syslog_dbg("\tEntering sem_dbus_config_ethport!\n");

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_BYTE,&slot_no,
		DBUS_TYPE_BYTE,&local_port_no,
		DBUS_TYPE_INVALID))) {
		sem_syslog_dbg("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			sem_syslog_dbg("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	sem_syslog_dbg("\tconfigure ether port %d/%d!\n",slot_no,local_port_no);

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	ret = SEM_COMMAND_SUCCESS;

	if (!LOCAL_SLOT_ISLEGAL(slot_no)|| !LOCAL_PORT_ISLEGAL(local_port_no))
	{
		ret = SEM_WRONG_PARAM;
	} else {	
		eth_g_index = SLOT_PORT_COMBINATION(slot_no, local_port_no);
		eth_l_index = LOCAL_PORT_INDEX(local_port_no);
		sem_syslog_dbg("\teth_g_index = %d, eth_l_index = %d\n", eth_g_index, eth_l_index);
		
	    if (local_board->port_arr[eth_l_index].cpu_or_asic == ASIC)
		{	
			sem_syslog_dbg("\tPort %d-%d is ASIC port\n", slot_no, local_port_no);	
			ret = SEM_COMMAND_NOT_SUPPORT;
		}
		
		if ((product->product_type == XXX_YYY_AX8610 || product->product_type == XXX_YYY_AX8800) && \
			((slot_no == (product->master_slot_id[0] + 1)) || \
			(slot_no == (product->master_slot_id[1] + 1))))
		{
			if (ret != SEM_WRONG_PARAM)
			{
				ret = SEM_COMMAND_NOT_PERMIT;
			}
		}
	}
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &eth_g_index);
	return reply;
}



DBusMessage* sem_dbus_config_ethport_attr(DBusConnection *conn, DBusMessage *msg, void *user_data){

	DBusMessage* reply;
	DBusError err;
	DBusMessageIter  iter;

	unsigned int type = 0;
	unsigned int eth_g_index = 0, slot_index = 0, eth_l_index = 0;
	unsigned int value = 0;
	unsigned int slot_id = local_board->slot_id;
	int ret = 0, retval = 0;
    unsigned int speed = PORT_SPEED_1000_E;
	unsigned int mode = PORT_FULL_DUPLEX_E;
	int slot, local_port_no, devPort;
	char file_path[64];
	struct eth_port_s port_info;
	
	sem_syslog_dbg("\tEntering sem_dbus_config_ethport_attr!\n");
    sem_syslog_dbg("\t-------------------------config attr start------------------------\n");
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
    		DBUS_TYPE_UINT32,&type,
    		DBUS_TYPE_UINT32,&eth_g_index,
    		DBUS_TYPE_UINT32,&value,
    		DBUS_TYPE_INVALID))) {
		sem_syslog_dbg("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			sem_syslog_dbg("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	SLOT_PORT_ANALYSIS_SLOT(eth_g_index, slot);
	SLOT_PORT_ANALYSIS_PORT(eth_g_index, local_port_no);
	eth_l_index = LOCAL_PORT_INDEX(local_port_no);
	sem_syslog_dbg("\tConfigure (index = %d) slot %d port %d, value %d\n",eth_g_index, slot, local_port_no, value);
	devPort = eth_l_index;
		
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);

	if(!LOCAL_SLOT_ISLEGAL(slot) || !LOCAL_PORT_ISLEGAL(local_port_no))
	{	
		sem_syslog_dbg("board max portnum %d product max slotcount %d\n", local_board->port_num_on_panel, product->slotcount);	
		ret = SEM_WRONG_PARAM;
		dbus_message_iter_append_basic (&iter, DBUS_TYPE_UINT32, &ret);
		dbus_message_iter_append_basic (&iter, DBUS_TYPE_UINT32, &eth_g_index);
		
		return reply;
	}
	if (local_board->port_arr[eth_l_index].cpu_or_asic == ASIC)
	{	
		sem_syslog_dbg("\tPort %d-%d is ASIC port\n", slot, local_port_no);	
		ret = SEM_COMMAND_NOT_SUPPORT;
		dbus_message_iter_append_basic (&iter, DBUS_TYPE_UINT32, &ret);
		dbus_message_iter_append_basic (&iter, DBUS_TYPE_UINT32, &eth_g_index);
		
		return reply;
	}
	
	switch(type){
	    case ADMIN:
			sem_syslog_dbg("\tADMIN %s\n",value?"enable":"disable");
			ret =  local_board->set_port_admin_status(devPort, value);
			if(ret != SEM_COMMAND_SUCCESS)
				sem_syslog_dbg("set admin status failed\n");
			break;		
		case DUMOD:
			sem_syslog_dbg("\tDUMOD %s\n",value?"full":"half");
			mode = value ? PORT_FULL_DUPLEX_E : PORT_HALF_DUPLEX_E;
			ret = local_board->set_port_duplex_mode(devPort, mode);
			if(ret != SEM_COMMAND_SUCCESS)
				sem_syslog_dbg("set port duplex mode failed\n");
			break;
		case FLOWCTRL:
			sem_syslog_dbg("\tFLOWCTRL %s\n",value?"enable":"disable");
			ret = local_board->set_ethport_flowCtrl(devPort, value);
			if(ret != SEM_COMMAND_SUCCESS)
				sem_syslog_dbg("set port flow control failed\n");
			break;
		#if 1
		case BACKPRE:
			sem_syslog_dbg("BRACKUP %s\n",value?"enable":"disable");
			//ret = npd_set_port_backPressure_state(port_index,value,isBoard);
			ret = local_board->set_ethport_backPressure(devPort, value);
			if(ret != SEM_COMMAND_SUCCESS)
				sem_syslog_dbg("set backPressure failed\n");
			break;
		#endif
		#if 0
		case LINKS:
			sem_syslog_dbg("LINKS %s\n",(0x1 == value) ? "UP" : ((0x0 == value) ?"DOWN":"AUTO"));
			ret = npd_set_port_link_status(port_index,value,isBoard);
			if(ret != SEM_COMMAND_SUCCESS)
				sem_syslog_dbg("set link status failed\n");
			break;
		#endif
		case CFGMTU:
			sem_syslog_dbg("\tCFGMTU %d\n",value);
			if(value & 1){
				ret = SEM_WRONG_PARAM;
			}
			else {
				ret = local_board->set_ethport_mtu(slot, local_port_no, value);
			}
				
			if(ret != SEM_COMMAND_SUCCESS)
				sem_syslog_dbg("set mtu failed\n");
			break;	
		case SPEED:
			sem_syslog_dbg("\tSPEED %dM\n", value);
        	if(value == 1000)
        		speed = PORT_SPEED_1000_E;
        	else if(value == 100)
        		speed = PORT_SPEED_100_E;
        	else if(value == 10)
        		speed = PORT_SPEED_10_E;
			ret = local_board->set_ethport_speed(devPort, speed);
			
			if(ret != SEM_COMMAND_SUCCESS){
				sem_syslog_dbg("set port speed failed\n ");
			}
			break;	
		case AUTONEGT:
			sem_syslog_dbg("\tAUTONEGT %s\n",value?"enable":"disable");
			/*status Reg can NOT be written.*/
			ret = local_board->set_port_autoneg_status(devPort, value);
			if(ret != SEM_COMMAND_SUCCESS){
				sem_syslog_dbg("set autoNeg failed\n");
			}
			break;	
		case AUTONEGTS:
			sem_syslog_dbg("\tAUTONEGTS %s\n",value?"enable":"disable");
			ret = local_board->set_ethport_speed_autoneg(devPort, value);
			if(ret != SEM_COMMAND_SUCCESS){
				sem_syslog_dbg("set autoNeg speed failed\n");
			}
			break;		
		case AUTONEGTD:
			sem_syslog_dbg("\tAUTONEGTD %s\n",value?"enable":"disable");
			ret = local_board->set_ethport_duplex_autoneg(devPort, value);
			if(ret != SEM_COMMAND_SUCCESS){
				sem_syslog_dbg("set autoNeg duplex failed\n");
			}
			break;
		case AUTONEGTF:
			sem_syslog_dbg("\tAUTONEGTF %s\n",value?"enable":"disable");
			ret = local_board->set_ethport_fc_autoneg(devPort, value);
			if(ret != SEM_COMMAND_SUCCESS){
				sem_syslog_dbg("set autoNeg flowctrl failed\n");
			}
			break;	
		case DEFAULT:   
			ret = SEM_OPERATE_NOT_SUPPORT;
			break;
		default:
			 sem_syslog_dbg("unknown trans type %d\n",type);
			 ret = SEM_WRONG_PARAM;
			break;
    }
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &eth_g_index);
	if(ret == SEM_COMMAND_SUCCESS)
	{
		sleep(1);
    	retval = sem_read_eth_port_info(slot, devPort, &port_info);

		sem_syslog_dbg("\tUpdate port%d-%d for global-ethport table\n", slot, local_port_no);
		if(retval < 0)
		{
            sem_syslog_dbg("Failed to read port%d-%d info \n", slot, local_port_no);
		}
        start_fp[local_board->slot_id][eth_l_index].attr_bitmap = port_info.attr_bitmap;
		start_fp[local_board->slot_id][eth_l_index].mtu = port_info.mtu;
		msync(start_fp[local_board->slot_id], sizeof(struct global_ethport_s)*BOARD_GLOBAL_ETHPORTS_MAXNUM, MS_SYNC);	

        char chTmp[512] = {0};
	    memcpy(chTmp, &start_fp[local_board->slot_id][eth_l_index], sizeof(global_ethport_t));
				
        if(local_board->is_active_master)
        {
			if(product->master_slot_id[0] == product->active_master_slot_id)
    		    sem_tipc_send(product->master_slot_id[1], SEM_ETHPORT_INFO_SYN, chTmp, sizeof(global_ethport_t));
			else
				sem_tipc_send(product->master_slot_id[0], SEM_ETHPORT_INFO_SYN, chTmp, sizeof(global_ethport_t));				
        }else {
            sem_tipc_send(product->active_master_slot_id, SEM_ETHPORT_INFO_SYN, chTmp, sizeof(global_ethport_t));
        }

    }
    sem_syslog_dbg("\t-------------------------config attr end------------------------\n");
	return reply;
}

DBusMessage* sem_dbus_config_ethport_media(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusMessageIter  iter;
	DBusError err;

	unsigned int ret = SEM_FAIL;
	int retval = -1;
	unsigned int media, eth_g_index = 0;
	unsigned char dev = 0, port = 0;
	unsigned int slot_no = 0, slot_index = 0;
	unsigned int local_port_no = 0, eth_l_index = 0, port_index = 0, devPort = 0;
    char file_path[64];
    struct eth_port_s port_info;

	sem_syslog_dbg("\tsem_dbus_config_ethport_media!\n");
	sem_syslog_dbg("\t-------------------------config media start------------------------\n");
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
								DBUS_TYPE_UINT32,&eth_g_index,
								DBUS_TYPE_UINT32,&media,
								DBUS_TYPE_INVALID))) {
		sem_syslog_dbg("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			sem_syslog_dbg("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	SLOT_PORT_ANALYSIS_SLOT(eth_g_index, slot_no);
	SLOT_PORT_ANALYSIS_PORT(eth_g_index, local_port_no);
    eth_l_index = LOCAL_PORT_INDEX(local_port_no);
	devPort = eth_l_index;
	
	if (!LOCAL_SLOT_ISLEGAL(slot_no)|| !LOCAL_PORT_ISLEGAL(local_port_no))
	{
		ret = SEM_WRONG_PARAM;
	}
	else if (local_board->port_arr[eth_l_index].cpu_or_asic == ASIC)
	{
        ret = SEM_COMMAND_NOT_SUPPORT;
	}
	else
	{
		ret  = local_board->set_port_preferr_media(devPort, media);
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	if(ret == SEM_COMMAND_SUCCESS)
	{
		retval = sem_read_eth_port_info(slot_no, devPort, &port_info);

		sem_syslog_dbg("\tUpdate port%d-%d for global-ethport table\n", slot_no, local_port_no);
		if(retval < 0)
		{
            sem_syslog_dbg("Failed to read port%d-%d info \n", slot_no, local_port_no);
		}
		start_fp[local_board->slot_id][eth_l_index].attr_bitmap = port_info.attr_bitmap;
		start_fp[local_board->slot_id][eth_l_index].port_type = port_info.port_type;
		msync(start_fp[local_board->slot_id], sizeof(struct global_ethport_s)*BOARD_GLOBAL_ETHPORTS_MAXNUM, MS_SYNC);	

        char chTmp[512] = {0};
	    memcpy(chTmp, &start_fp[local_board->slot_id][eth_l_index], sizeof(global_ethport_t));
				
        if(local_board->is_active_master)
        {
			if(product->master_slot_id[0] == product->active_master_slot_id)
    		    sem_tipc_send(product->master_slot_id[1], SEM_ETHPORT_INFO_SYN, chTmp, sizeof(global_ethport_t));
			else
				sem_tipc_send(product->master_slot_id[0], SEM_ETHPORT_INFO_SYN, chTmp, sizeof(global_ethport_t));				
        }else {
            sem_tipc_send(product->active_master_slot_id, SEM_ETHPORT_INFO_SYN, chTmp, sizeof(global_ethport_t));
        }

	}

	sem_syslog_dbg("\t-------------------------config media end------------------------\n");
	return reply;
}


DBusMessage *sem_dbus_reboot(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter	 iter;
	local_board->reboot_self();
	return reply;
}

int get_stby_sd(int *sd)
{
	int stby_slot_id = USELESS_SLOT_ID;
	int i;

	for (i=0; i<product->master_slot_count; i++)
	{
		if (product->master_slot_id[i] == product->active_master_slot_id)
		{
			continue;	
		}
		else
		{
			stby_slot_id = product->master_slot_id[i];
			break;
		}
	}

	if (stby_slot_id == USELESS_SLOT_ID)
	{
		return 1;
	}
	
	for (i=0; i<product->slotcount; i++)
	{
		if (thread_id_arr[i].slot_id == stby_slot_id)
		{
			*sd = thread_id_arr[i].sd;
			sem_syslog_dbg("stby slot id =%d\n  sd = %d\n", stby_slot_id+1, *sd);
			return 0;
		}
	}

	return 1;
}

int get_stby_slot_id(int *slto_id)
{
	int stby_slot_id = USELESS_SLOT_ID;
	int i;

	for (i=0; i<product->master_slot_count; i++)
	{
		if (product->master_slot_id[i] == product->active_master_slot_id)
		{
			continue;	
		}
		else
		{
			stby_slot_id = product->master_slot_id[i];
			break;
		}
	}

	if (stby_slot_id == USELESS_SLOT_ID)
	{
		return 1;
	}
	else
	{
		*slto_id = stby_slot_id;
		return 0;	
	}
}
/*
 *ret 0:success -1:failed 1:no need
 */
DBusMessage *sem_dbus_syn_file(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter	 iter;
	int ret = 0;
	char *syn_file_name;
	int syn_to_blk;
	int stby_slot_id;
	DBusError err;

	if (!product->is_distributed)
	{
		ret = 0;
		reply = dbus_message_new_method_return(msg);
	
		dbus_message_iter_init_append (reply, &iter);
	
		dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
		return reply;
	}
	
	if (local_board->is_active_master)
	{
		sem_syslog_dbg("active MCB ready to syn file to stby MCB\n");
	}
	else
	{
		sem_syslog_dbg("non-active MCB.no need to syn file to stby MCB\n");
		ret = -1;
		
		reply = dbus_message_new_method_return(msg);
	
		dbus_message_iter_init_append (reply, &iter);
	
		dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
		return reply;
	}

	if (!product->more_than_one_master_board_on)
	{
		sem_syslog_dbg("only one MCB on,no need to syn file\n");
		ret = SYN_FILE_SUCCESS;
		reply = dbus_message_new_method_return(msg);
	
		dbus_message_iter_init_append (reply, &iter);
	
		dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
		return reply;
	}
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args(msg, &err,
								DBUS_TYPE_STRING, &syn_file_name,
								DBUS_TYPE_INT32,&syn_to_blk,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		ret = SYN_FILE_FAILED;
		reply = dbus_message_new_method_return(msg);
	
		dbus_message_iter_init_append (reply, &iter);
	
		dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
		return reply;
	}

	sem_syslog_dbg("syn file name is: %s\n", syn_file_name);
	ret = get_stby_slot_id(&stby_slot_id);

	if (ret)
	{
		sem_syslog_dbg("get stby sd failed . syn file to stby MCB failed\n");
		ret = SYN_FILE_FAILED;
	}
	else
	{
		if (product->sync_send_file(stby_slot_id, syn_file_name, syn_to_blk))
		{
			sem_syslog_dbg("syn config file failed\n");
			ret = SYN_FILE_FAILED;
		}
		else
		{
			sem_syslog_dbg("syn config file succeed\n");
			ret = SYN_FILE_SUCCESS;
		}
		
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	
	return reply;
}


DBusMessage *sem_dbus_ethports_show_running_config(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	DBusMessage* reply = NULL;
	DBusMessageIter	 iter;

	char *showStr = NULL,*cursor = NULL;
	int totalLen = 0;
	int i = 0, j = 0;
    struct eth_port_s * eth_port = NULL;

	showStr = (char*)malloc(SEM_ETHPORT_SHOWRUN_CFG_SIZE);

	sem_syslog_dbg("enter sem_dbus_ethports_show_running_config\n");
	if(NULL == showStr) {
		sem_syslog_dbg("memory malloc error\n");
		return reply;
	}
	memset(showStr, 0, SEM_ETHPORT_SHOWRUN_CFG_SIZE);
	cursor = showStr;

	
	unsigned char local_port_count = local_board->port_num_on_panel;
	for (j = 0; j < local_port_count; j++ ) 
	{
    	unsigned char local_port_no = j + 1;
    	unsigned int eth_g_index = SLOT_PORT_COMBINATION(i, local_port_no);
    	unsigned int local_port_attrmap = 0, local_port_mtu = 0,local_port_ipg = 0;
    	unsigned int slot_no = local_board->slot_id + 1;
    	unsigned int ret = 0;
    	struct eth_port_s portInfo, *portNode = NULL;
    	unsigned char enter_node = 0; /* need to enter eth-port node first*/
    	char tmpBuf[2048] = {0}, *tmpPtr = NULL;
    	int length = 0;
    	unsigned char an = 0,an_state = 0,an_fc = 0,fc = 0,an_duplex = 0,duplex = 0,an_speed = 0;
    	unsigned char admin_status = 0,bp = 0,copper = 0,fiber = 0;
    	PORT_SPEED_ENT speed = PORT_SPEED_10_E;

        if (local_board->port_arr[j].cpu_or_asic == ASIC)
        {
			sem_syslog_dbg("Port %d-%d is ASIC port\n",slot_no, local_port_no);
			continue;
        }
		sem_syslog_dbg("Port %d-%d is CPU port\n",slot_no, local_port_no);	
        
    	memset(&portInfo, 0, sizeof(struct eth_port_s));
    	ret = sem_read_eth_port_info(slot_no, j, &portInfo);
    		
    	if (ret != 0) 
    	{
    		sem_syslog_dbg("get port %d/%d attribute fail %d\n",slot_no, local_port_no, ret);
    	}
    	else 
    	{
    		local_port_attrmap	= portInfo.attr_bitmap;
    		local_port_mtu		= portInfo.mtu;
    		local_port_ipg      = portInfo.ipg;
    	}
    	
    	sem_syslog_dbg("local_port_attrmap 0x%x\n", local_port_attrmap);
    	tmpPtr = tmpBuf;

    	/* MRU*/

    	if(local_port_mtu != local_board->port_arr[j].port_attribute.mtu) 
    	{
    		enter_node = 1;
    		length += sprintf(tmpPtr," config mtu %d\n",local_port_mtu);
    		tmpPtr = tmpBuf+length;
    	}
    	
    	admin_status = (local_port_attrmap & ETH_ATTR_ADMIN_STATUS) >> ETH_ADMIN_STATUS_BIT;
    	bp = (local_port_attrmap & ETH_ATTR_BACKPRESSURE) >> ETH_BACKPRESSURE_BIT;
    	an_state = (local_port_attrmap & ETH_ATTR_AUTONEG) >> ETH_AUTONEG_BIT;
    	an = (local_port_attrmap & ETH_ATTR_AUTONEG_CTRL) >> ETH_AUTONEG_CTRL_BIT;
    	an_speed = (local_port_attrmap & ETH_ATTR_AUTONEG_SPEED) >> ETH_AUTONEG_SPEED_BIT;
    	speed = (local_port_attrmap & ETH_ATTR_SPEED_MASK) >> ETH_SPEED_BIT;
    	an_duplex = (local_port_attrmap & ETH_ATTR_AUTONEG_DUPLEX) >> ETH_AUTONEG_DUPLEX_BIT;
    	duplex = (local_port_attrmap & ETH_ATTR_DUPLEX) >> ETH_DUPLEX_BIT;
    	an_fc = (local_port_attrmap & ETH_ATTR_AUTONEG_FLOWCTRL) >> ETH_AUTONEG_FLOWCTRL_BIT;
    	fc = (local_port_attrmap & ETH_ATTR_FLOWCTRL) >> ETH_FLOWCTRL_BIT;
    	copper = (local_port_attrmap & ETH_ATTR_PREFERRED_COPPER_MEDIA) >> ETH_PREFERRED_COPPER_MEDIA_BIT; 
    	fiber = (local_port_attrmap & ETH_ATTR_PREFERRED_FIBER_MEDIA) >> ETH_PREFERRED_FIBER_MEDIA_BIT;

    	/*media priority*/
		unsigned int media;
    	ret = local_board->get_eth_port_trans_media(j, &media);
    	if(0 == media && 2 == media) {
    		enter_node = 1;
    		length += sprintf(tmpPtr," config media preferred copper\n");
    		tmpPtr = tmpBuf + length;				
    	}
    	else if(1 == media) {
    		enter_node = 1;
    		length += sprintf(tmpPtr," config media preferred fiber\n");
    		tmpPtr = tmpBuf + length;	
    	}	

    	/*  Auto-Nego - all AN options disable we need one command control*/
    	if((an != local_board->port_arr[j].port_attribute.autoNego) &&
    		!(an_speed != an && an_duplex != an && an_fc != an) /* &&
    		(an == an_speed) && (an == an_duplex) && (an == an_fc) */){
    		enter_node = 1;
    		length += sprintf(tmpPtr," config auto-negotiation %s\n", an ? "enable":"disable");
    		tmpPtr = tmpBuf+length;
    	}

    	/* Auto-Nego speed*/
    	if(((an_speed != local_board->port_arr[j].port_attribute.speed_an && 
    		an == local_board->port_arr[j].port_attribute.autoNego) ||
    		(an_speed != an && an != local_board->port_arr[j].port_attribute.autoNego)) &&
    		!(an_speed != an && an_duplex != an && an_fc != an)) {
    		enter_node = 1;
    		length += sprintf(tmpPtr," config auto-negotiation speed %s\n", an_speed ? "enable":"disable");
    		tmpPtr = tmpBuf+length;
    	}
    		
    	/* Auto-Nego duplex*/
    	if(((an_duplex != local_board->port_arr[j].port_attribute.duplex_an && 
    		an == local_board->port_arr[j].port_attribute.autoNego)||
    		(an_duplex != an  && an != local_board->port_arr[j].port_attribute.autoNego)) &&
    		!(an_speed != an && an_duplex != an && an_fc != an)) {
    		enter_node = 1;
    		length += sprintf(tmpPtr," config auto-negotiation duplex %s\n", an_duplex ? "enable":"disable");
    		tmpPtr = tmpBuf+length;
    	}

    	/* Auto-Nego flow-control*/
    	if(((an_fc != local_board->port_arr[j].port_attribute.fc_an && 
    		an == local_board->port_arr[j].port_attribute.autoNego)||
    		(an_fc != an && an != local_board->port_arr[j].port_attribute.autoNego)) &&
    		!(an_speed != an && an_duplex != an && an_fc != an)) {
    		enter_node = 1;
    		length += sprintf(tmpPtr," config auto-negotiation flow-control %s\n",an_fc ? "enable":"disable");
    		tmpPtr = tmpBuf+length;
    	}

    	/* speed - if not auto-nego speed and speed not default*/

		if((an_speed != ETH_ATTR_ON) && (speed != local_board->port_arr[j].port_attribute.speed)) {
			enter_node = 1;
			length += sprintf(tmpPtr," config speed %d\n",(speed == PORT_SPEED_100_E) ? 100:10);
			tmpPtr = tmpBuf+length;
		}	
    	

    	/* flow-control - if not auto-nego fc and fc mode not default*/
    	if((an_fc != ETH_ATTR_ON) && (fc != local_board->port_arr[j].port_attribute.fc)){
    		enter_node = 1;
    		length += sprintf(tmpPtr," config flow-control %s\n", fc ? "enable":"disable");
    		tmpPtr = tmpBuf+length;
    	}
    	/* duplex - if not auto-nego duplex and duplex mode not default*/
    	if((an_duplex != ETH_ATTR_ON) && (duplex != local_board->port_arr[j].port_attribute.duplex)){
    		enter_node = 1;
    		length += sprintf(tmpPtr," config duplex mode %s\n", duplex ? "half":"full");
    		tmpPtr = tmpBuf+length;
    	}	
    	/* back-pressure  - back pressure only relevant when port in half-duplex mode*/
    	if((bp != local_board->port_arr[j].port_attribute.bp) && (PORT_HALF_DUPLEX_E == duplex)) {
    		enter_node = 1;
    		length += sprintf(tmpPtr," config back-pressure %s\n", bp ? "enable":"disable");
    		tmpPtr = tmpBuf + length;
    	}


    	/* admin status*/
    	if(admin_status != local_board->port_arr[j].port_attribute.admin_state) {
    		enter_node = 1;
    		length += sprintf(tmpPtr," config admin state %s\n",admin_status ? "enable":"disable");
    		tmpPtr = tmpBuf+length;
    	}

    	if(enter_node) 
		{ /* port configured*/
    		if((totalLen + length + 20 + 5) > SEM_ETHPORT_SHOWRUN_CFG_SIZE) { /* 20 for enter node; 5 for exit node*/
    			sem_syslog_dbg("show ethport buffer full");
    			break;
    		}				

    		totalLen += sprintf(cursor,"config eth-port %d/%d\n",slot_no,local_port_no);
    		cursor = showStr + totalLen;
    		totalLen += sprintf(cursor,"%s",tmpBuf);
    		cursor = showStr + totalLen;
    		totalLen += sprintf(cursor," exit\n");
    		cursor = showStr + totalLen;
    		enter_node = 0;
    	}

	}		

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &showStr);	
	
	free(showStr);
	showStr = NULL;

	return reply;
}




/************************************************************************************
 *		sem dbus operation
 *
 ************************************************************************************/
DBusMessage * sem_dbus_show_ethport_attr(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{	
	DBusMessage* reply;
	DBusMessageIter  iter;
	unsigned char slot_no = 0, local_port_no = 0, type = 0, slot_index = 0;
	unsigned int port_index = 0, eth_l_index = 0;
	int eth_g_index = 0;
	struct eth_port_s port_info;

	unsigned int ret = 0;
	unsigned char subcard_type = 0;

	struct timeval tnow;
	struct timezone tzone;
	unsigned long last_link_change = 0;
	unsigned int link_keep_time = 0;

	memset(&tnow, 0, sizeof(struct timeval));
	memset(&tzone, 0, sizeof(struct timezone));
	gettimeofday (&tnow, &tzone);	
	
	DBusError err;

	sem_syslog_dbg("enter sem_dbus_show_ethport_attr\n");
	memset(&port_info, 0,sizeof(struct eth_port_s));
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_BYTE,&type,
		DBUS_TYPE_BYTE,&slot_no,
		DBUS_TYPE_BYTE,&local_port_no,
		DBUS_TYPE_UINT32,&port_index,
		DBUS_TYPE_INVALID))) {
		sem_syslog_dbg("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			sem_syslog_dbg("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}	

	if(0 == type){
		ret = SEM_WRONG_PARAM;
	}
	else{
		ret = SEM_COMMAND_FAILED;
		eth_g_index = port_index;
		SLOT_PORT_ANALYSIS_SLOT(port_index, slot_no);
		SLOT_PORT_ANALYSIS_PORT(port_index, local_port_no);
	    eth_l_index = LOCAL_PORT_INDEX(local_port_no);
	}

	if (!LOCAL_SLOT_ISLEGAL(slot_no) || !LOCAL_PORT_ISLEGAL(local_port_no))
	{
		sem_syslog_dbg("wrong param:slot_no=%d port=%d , product->slotcount=%d local_board->port_num_on_panel=%d\n",
					slot_no, local_port_no, product->slotcount, local_board->port_num_on_panel);
		ret = SEM_WRONG_PARAM;
	}
	else
	{
		sem_syslog_dbg("read port info for %d/%d\n", slot_no, local_port_no);
		ret = sem_read_eth_port_info(slot_no, eth_l_index, &port_info);
	}
	
	if(local_board->port_arr[eth_l_index].cpu_or_asic == ASIC)
	{
		sem_syslog_dbg("--------------ETH_ASIC_PORT-----------\n");
		ret = SEM_COMMAND_NOT_SUPPORT;
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &(port_info.port_type));

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &(port_info.attr_bitmap));

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &port_info.mtu);

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &port_info.lastLinkChange);	
	return reply;
}


DBusMessage * sem_dbus_ethports_interface_show_ethport_list
(
 DBusConnection *conn, 
 DBusMessage *msg, 
 void *user_data
)
{
	DBusMessage* reply;
	DBusMessageIter  iter;
	DBusMessageIter  iter_array, iter_struct;
	int i = 0, j = 0;
	unsigned char ret = 0;
	struct timeval tnow;
	struct timezone tzone;
	unsigned long last_link_change = 0;
	unsigned int link_keep_time = 0;
	unsigned int slot_no, portNum ,slotNum;
	int octeon_or_asic;

	memset(&tnow, 0, sizeof(struct timeval));
	memset(&tzone, 0, sizeof(struct timezone));
	gettimeofday (&tnow, &tzone);

	sem_syslog_dbg("Entering show ethport list!\n");

	slotNum = product->slotcount;


	reply = dbus_message_new_method_return(msg);	
	sem_syslog_dbg("dbus_message_new_method_return!\n");

	dbus_message_iter_init_append (reply, &iter);
	sem_syslog_dbg("dbus_message_iter_init_append!\n");

	slot_no = local_board->slot_id + 1;
	portNum = local_board->port_num_on_panel;

	dbus_message_iter_append_basic (&iter, DBUS_TYPE_UINT32, &slot_no);
	sem_syslog_dbg("dbus_message_iter_append_basic!slot_no::%d\n", slot_no);

	dbus_message_iter_append_basic (&iter, DBUS_TYPE_UINT32, &portNum);
	sem_syslog_dbg("dbus_message_iter_append_basic!portNum::%d\n", portNum);	


	/* Array of slot port information
	   slot no
	   total port count
	   Array of Port Infos.
	   port no
	   port type
	   port attriute_bitmap
	   port MTU
	   */

	dbus_message_iter_open_container (&iter,
			DBUS_TYPE_ARRAY,
			DBUS_STRUCT_BEGIN_CHAR_AS_STRING
			DBUS_TYPE_INT32_AS_STRING
			DBUS_TYPE_BYTE_AS_STRING
			DBUS_TYPE_BYTE_AS_STRING
			DBUS_TYPE_UINT32_AS_STRING
			DBUS_TYPE_UINT32_AS_STRING
			DBUS_TYPE_UINT32_AS_STRING
			DBUS_STRUCT_END_CHAR_AS_STRING,
			&iter_array);

	for (j = 0; j < portNum; j++ ) 
	{
		/*		
    		Array of Port Infos.
    		port no
    		port type
    		port attriute_bitmap
    		port MTU
		*/

		unsigned char local_port_no ;
		unsigned char local_port_type ;
		unsigned int local_port_attrmap ;
		unsigned int local_port_mtu;
		last_link_change  = 0;
        local_port_no = j + 1;

		unsigned int ret = 0;
		unsigned char subcard_type = 0;
		struct eth_port_s portInfo;

		memset(&portInfo, 0, sizeof(struct eth_port_s));

		
		ret = sem_read_eth_port_info(slot_no, j, &portInfo);

		if(!ret)
			octeon_or_asic = ETH_OCTEON_PORT;
		else if (ret == SEM_COMMAND_NOT_SUPPORT && local_board->port_arr[j].cpu_or_asic == ASIC)
			octeon_or_asic = ETH_ASIC_PORT;
		last_link_change = 8;
		if(SEM_SUCCESS != ret) {
			sem_syslog_dbg("read port(%d %d) infomation error.\n", slot_no, local_port_no);
		}
		
		local_port_type = portInfo.port_type;	
		local_port_attrmap	= portInfo.attr_bitmap;
		local_port_mtu		= portInfo.mtu;
		sem_syslog_dbg("port(%d %d)  port_type %d\n",slot_no, local_port_no, local_port_type);
		link_keep_time = tnow.tv_sec - last_link_change;
		sem_syslog_dbg("port %d/%d attr get time %u. \n",slot_no,local_port_no,link_keep_time);

		dbus_message_iter_open_container (&iter_array, DBUS_TYPE_STRUCT, NULL, &iter_struct);
		dbus_message_iter_append_basic(&iter_struct, DBUS_TYPE_INT32, &octeon_or_asic);
		
		if(octeon_or_asic == ETH_ASIC_PORT)
			sem_syslog_dbg("--------------ETH_ASIC_PORT-------------\n");
		else 
            sem_syslog_dbg("--------------ETH_OCTEON_PORT-----------\n");
		
		dbus_message_iter_append_basic(&iter_struct, DBUS_TYPE_BYTE, &(local_port_no));
		dbus_message_iter_append_basic(&iter_struct, DBUS_TYPE_BYTE, &(local_port_type));
		dbus_message_iter_append_basic(&iter_struct, DBUS_TYPE_UINT32, &(local_port_attrmap));
		dbus_message_iter_append_basic(&iter_struct, DBUS_TYPE_UINT32, &(local_port_mtu));
		dbus_message_iter_append_basic(&iter_struct, DBUS_TYPE_UINT32, &(link_keep_time));			
		dbus_message_iter_close_container (&iter_array, &iter_struct);

	}
	/* If there is ethernet port in extend slot , add them here */

	sem_syslog_dbg("get eth port end of for\n");
	dbus_message_iter_close_container (&iter, &iter_array);
	sem_syslog_dbg("dbus_message_iter_close_container!\n");	
	return reply;
}

DBusMessage *sem_dbus_get_slot_port_by_portindex
(
 DBusConnection *conn, 
 DBusMessage *msg,
 void *user_data
) 
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned char	slot_no = 0, local_port_no = 0, slot_index = 0;
	unsigned int 	eth_g_index = 0, eth_l_index = 0;
	int ret = 0;
	DBusError err;
	
	sem_syslog_dbg("Get slot/port!\n");
    sem_syslog_dbg(" npd_dbus_get_slot_port_by_portindex\n");
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
    		DBUS_TYPE_UINT32,&eth_g_index,
    		DBUS_TYPE_INVALID))) {
		 sem_syslog_dbg("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 sem_syslog_dbg("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}


	SLOT_PORT_ANALYSIS_SLOT(eth_g_index, slot_no);
	SLOT_PORT_ANALYSIS_PORT(eth_g_index, local_port_no);
	eth_l_index = LOCAL_PORT_INDEX(local_port_no);
/*
    if(local_board->port_arr[eth_l_index].port_type == ASIC)
    {
		ret = SEM_COMMAND_NOT_SUPPORT;
    }
*/	
	sem_syslog_dbg("local_port_no = %d, eth_g_index = %d\n",local_port_no, eth_g_index);
		
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_BYTE,
									 &slot_no);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_BYTE,
									 &local_port_no);
	
	return reply;

}


static unsigned int sem_ethport_packet_statistic
(
    unsigned int portNum,
    int clear,
	struct port_counter_s *eth_port_stats
)
{
	int retval;
	int fd = -1;
    char ifName[21];

	if ((local_board->name, "AX81_AC12C") && (portNum >=1 && portNum <= 4)) {
		sem_syslog_dbg("ax81_ac12c port %d not support statistics now.");
		return ETHPORT_RETURN_CODE_ERR_GENERAL;
	}
	
    fd = open("/dev/oct0", 0);
    if (fd < 0) {
        sem_syslog_dbg("open /dev/oct0 fail\n");
        return INTERFACE_RETURN_CODE_FD_ERROR;
    }
	
	eth_port_stats->clear = clear;
	local_board->get_port_name(local_board->slot_id+1, portNum, ifName);

	strcpy(eth_port_stats->ifName, ifName);
	
    retval = ioctl(fd, CVM_IOC_GET_ETH_STAT, eth_port_stats); /*read CSRs*/
	if (retval == -1)
	{
		sem_syslog_dbg("read eth-port %d statistics error!\n", portNum);
		close(fd);
		return ETHPORT_RETURN_CODE_ERR_GENERAL;
	}

	close(fd);
	return ETHPORT_RETURN_CODE_ERR_NONE;
}

struct eth_port_s *sem_get_port_by_index
(
	unsigned int eth_g_index
)
{
	struct eth_port_s *portInfo = NULL;
	
	/*eth port index should be in [0,MAX_ETH_GLOBAL_INDEX)*/
	if(eth_g_index > GLOBAL_ETHPORTS_MAXNUM) {
		return NULL;
	}
	portInfo = g_eth_ports[eth_g_index];

	return portInfo;
}

int sem_eth_port_counter_statistics
(
	unsigned int eth_g_index,
    struct port_counter_s *eth_port_stats
)
{
	struct eth_port_s * g_ptr = NULL;

	g_ptr = sem_get_port_by_index(eth_g_index);
	if(g_ptr){
		g_ptr ->counters.rx.goodbytesl = eth_port_stats->pip_stats.inb_octets & 0xffffffff;
		g_ptr ->counters.rx.goodbytesh = (eth_port_stats->pip_stats.inb_octets >> 32) & 0xffff;		
		g_ptr ->counters.rx.badbytes = eth_port_stats->pip_stats.inb_packets;	
		g_ptr ->counters.rx.uncastframes = eth_port_stats->pip_stats.octets;
		g_ptr ->counters.rx.bcastframes = eth_port_stats->pip_stats.broadcast_packets;
		g_ptr ->counters.rx.mcastframes = eth_port_stats->pip_stats.multicast_packets;
		g_ptr ->counters.rx.fcframe = eth_port_stats->pip_stats.packets;
		g_ptr ->counters.rx.fifooverruns = eth_port_stats->pip_stats.dropped_packets;
		g_ptr ->counters.rx.underSizepkt = eth_port_stats->pip_stats.inb_errors;
		g_ptr ->counters.rx.fragments = eth_port_stats->pip_stats.fcs_align_err_packets;		
		g_ptr ->counters.rx.overSizepkt = eth_port_stats->pip_stats.oversize_packets;
		g_ptr ->counters.rx.jabber = eth_port_stats->pip_stats.oversize_crc_packets;
		g_ptr ->counters.rx.errorframe = eth_port_stats->pip_stats.runt_packets;
	
		g_ptr ->counters.tx.goodbytesl = eth_port_stats->pko_stats.octets & 0xffffffff;
		g_ptr ->counters.tx.goodbytesh = (eth_port_stats->pko_stats.octets >> 32) & 0xffff;
		g_ptr ->counters.tx.uncastframe = eth_port_stats->pko_stats.packets;
		g_ptr ->counters.tx.sent_deferred = eth_port_stats->pko_stats.doorbell & 0xffffffff;
		g_ptr ->counters.tx.excessiveCollision = (eth_port_stats->pko_stats.doorbell >> 32) & 0xffff;
		g_ptr ->counters.tx.mcastframe = 0;	
		g_ptr ->counters.tx.bcastframe = 0;
		g_ptr ->counters.tx.sentMutiple = 0;
		g_ptr ->counters.tx.fcframe = 0;
		g_ptr ->counters.tx.crcerror_fifooverrun = 0;

		g_ptr ->counters.otc.badCRC = eth_port_stats->pip_stats.runt_crc_packets;
		g_ptr ->counters.otc.collision = eth_port_stats->pip_stats.pci_raw_packets;
		g_ptr ->counters.otc.late_collision = eth_port_stats->pip_stats.len_1519_max_packets;
		g_ptr ->counters.otc.b1024oct2max = eth_port_stats->pip_stats.len_1024_1518_packets;
		g_ptr ->counters.otc.b512oct21023 = eth_port_stats->pip_stats.len_512_1023_packets;
		g_ptr ->counters.otc.b256oct511 = eth_port_stats->pip_stats.len_256_511_packets;
		g_ptr ->counters.otc.b128oct255 = eth_port_stats->pip_stats.len_128_255_packets;
		g_ptr ->counters.otc.b64oct127 = eth_port_stats->pip_stats.len_65_127_packets;
		g_ptr ->counters.otc.b64oct = eth_port_stats->pip_stats.len_64_packets;
		
		return SEM_SUCCESS;
	}
	else{
		return SEM_FAIL;
	}
}
DBusMessage * sem_dbus_ethports_interface_show_ethport_stat
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	DBusMessage* reply;
	DBusMessageIter  iter, iter_array, iter_struct;
	DBusError err;
	unsigned int ret = 0;
	unsigned char slot_no = 0, local_port_no = 0;
	unsigned char portNum = 0, devNum = 0, type = 0;
	unsigned int port_index = 0;
	int eth_g_index = 0, i = 0, g_index = 0 ;
	unsigned int linkuptimes = 0, linkdowntimes = 0;
    unsigned long long tmp = 0;
	struct eth_port_s* portInfo = NULL;
	char ifName[21];
    struct port_counter_s *portPtr, portPktCount;
	portPtr = &portPktCount;
	memset(portPtr,0,sizeof(struct port_counter_s));
	sem_syslog_dbg("\tEntering show ethport stat!\n");

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_BYTE,&type,
		DBUS_TYPE_BYTE,&slot_no,
		DBUS_TYPE_BYTE,&local_port_no,
		DBUS_TYPE_UINT32,&eth_g_index,
		DBUS_TYPE_INVALID))) {
		sem_syslog_dbg("Unable to get input args\n");
		if (dbus_error_is_set(&err)) {
			sem_syslog_dbg("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	sem_syslog_dbg("\tShow ether port detail of port %d/%d !\n",slot_no,local_port_no);
    unsigned int eth_l_index = local_port_no -1;
	
	if (!LOCAL_SLOT_ISLEGAL(slot_no)|| !LOCAL_PORT_ISLEGAL(local_port_no))
	{
		ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
	}
	else if((local_board->board_code == AX81_1X12G12S_BOARD_CODE && local_port_no == 25) ||(local_board->board_code == AX71_1X12G12S_BOARD_CODE && local_port_no == 25))
	{
        ret = INTERFACE_RETURN_CODE_FPGA_ETH_XGE_FIBER;
	}
	else if(local_board->port_arr[eth_l_index].cpu_or_asic == ASIC)
	{
        ret = INTERFACE_RETURN_CODE_UNSUPPORT_COMMAND;
	}
	else
	{	
		portInfo = sem_get_port_by_index(eth_g_index);
		
		if(NULL == portInfo){
			sem_syslog_dbg("get port info errors\n");
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else
		{
        	ret = sem_ethport_packet_statistic(local_port_no, 0, portPtr);
        	if(0 != ret){
        		sem_syslog_dbg("sem_board_port_pkt_statistic ERROR\n");
        		ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
        	}
    		
            ret = sem_eth_port_counter_statistics(eth_g_index, portPtr);	
        	if(0 != ret){
        		sem_syslog_dbg("save counter info error\n");
        		ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
        	}
        	else {
        		ret = ETHPORT_RETURN_CODE_ERR_NONE;
        	}

        	/* get port link up&down times */
        	linkuptimes = portInfo->counters.linkupcount;
        	linkdowntimes = portInfo->counters.linkdowncount;
    	}
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter, DBUS_TYPE_UINT32, &ret);
	if(ETHPORT_RETURN_CODE_ERR_NONE == ret ) {

		dbus_message_iter_open_container (&iter,
										   DBUS_TYPE_ARRAY,
										   DBUS_STRUCT_BEGIN_CHAR_AS_STRING
										   DBUS_TYPE_UINT64_AS_STRING
										   DBUS_TYPE_UINT32_AS_STRING
										   DBUS_TYPE_UINT32_AS_STRING
										   DBUS_STRUCT_END_CHAR_AS_STRING,
										   &iter_array);

		for(i = 0; i<32; i++)
		{
			
			dbus_message_iter_open_container (&iter_array,
											   DBUS_TYPE_STRUCT,
											   NULL,
											   &iter_struct);
			
			tmp = *((unsigned long long*)(&(portInfo->counters)) + i);
			/*sem_syslog_dbg("<<tmp %d value %lld>>\n ",i,tmp);*/
			dbus_message_iter_append_basic(&iter_struct, 
				                     DBUS_TYPE_UINT64,
				                     &(tmp));

			dbus_message_iter_append_basic (&iter_struct,
									 DBUS_TYPE_UINT32,
									 &linkuptimes);
	
			dbus_message_iter_append_basic (&iter_struct,
									 DBUS_TYPE_UINT32,
									 &linkdowntimes);			
			dbus_message_iter_close_container (&iter_array, &iter_struct);	
		}

		dbus_message_iter_close_container (&iter, &iter_array);

	}

	return reply;
}


DBusMessage * sem_dbus_ethports_interface_clear_ethport_stat(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusMessageIter  iter;
	DBusError err;
	unsigned char slot_no = 0, local_port_no = 0;
	unsigned char type = 0;
	unsigned int eth_g_index = 0;
	struct eth_port_s* g_ptr = NULL;
	unsigned int ret = 0;

	struct port_counter_s *portPtr, portPktCount;
	portPtr = &portPktCount;
	memset(portPtr,0,sizeof(struct port_counter_s));

	sem_syslog_dbg("\tEntering clear ethport stat!\n");

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_BYTE,&type,
		DBUS_TYPE_BYTE,&slot_no,
		DBUS_TYPE_BYTE,&local_port_no,
		DBUS_TYPE_UINT32,&eth_g_index,
		DBUS_TYPE_INVALID))) {
		sem_syslog_dbg("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			sem_syslog_dbg("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	sem_syslog_dbg("\tShow ether port detail of port %d/%d !\n",slot_no,local_port_no);

	if (!LOCAL_SLOT_ISLEGAL(slot_no)|| !LOCAL_PORT_ISLEGAL(local_port_no))
	{
		ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
	}
	else if(local_board->port_arr[local_port_no-1].cpu_or_asic == ASIC)
	{
        ret = INTERFACE_RETURN_CODE_UNSUPPORT_COMMAND;
	}
	else
	{	
    	ret = sem_ethport_packet_statistic(local_port_no, 1, portPtr);
    	if(0 != ret){
    		sem_syslog_dbg("sem_board_port_pkt_statistic ERROR\n");
    		ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
    	}
		//sem_syslog_dbg("\teth_g_index = %d\n", eth_g_index);
    	/*get eth port stat*/
    	g_ptr = sem_get_port_by_index(eth_g_index);
    	if(NULL == g_ptr){
    		sem_syslog_dbg("get port info errors\n");
    		ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
    	} else {
    	
    		sem_syslog_dbg("\tclear counter info\n");
    		memset(&(g_ptr->counters),0,sizeof(struct eth_port_counter_s));
    	}
 	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32, 
									 &ret);
	return reply;

}

DBusMessage *sem_dbus_sync_master_eth_port_info(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter	 iter;
	DBusError err;
	int j = 0;
	int ret = 0;
	char file_path[64];
	int standby_master_slot_id = 0;
	int active_master_slot_id = product->active_master_slot_id;
    int update_active_or_standby = 0;
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		                        DBUS_TYPE_UINT32,&update_active_or_standby,
								DBUS_TYPE_INVALID))) {
		if (dbus_error_is_set(&err)) {
			sem_syslog_dbg("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	if(update_active_or_standby == 0)
	{
    	if(local_board->slot_id != active_master_slot_id)
        {
            sprintf(file_path, "/dbm/shm/ethport/shm%d", local_board->slot_id+1);
            sem_syslog_dbg("slot %d send ethport shm file to active_MCB\n", local_board->slot_id+1);
    		sem_syslog_dbg("@@@@@@@@@@@@ active_master_slot_id = %d\n", active_master_slot_id);
    		sem_syslog_dbg("@@@@@@@@@@@@ local_board->slot_id = %d\n", local_board->slot_id);
            product->sync_send_file(active_master_slot_id, file_path, 0);
        }
	}
	else
	{
		if(active_master_slot_id == product->master_slot_id[0])
    		standby_master_slot_id = product->master_slot_id[1];
    	else
    		standby_master_slot_id = product->master_slot_id[0];	
		
    	sem_syslog_dbg("\tActive_MCB send all of the shm file to standby_MCB\n");
		for(j = 0; j < product->slotcount; j++)
		{
			if(j != standby_master_slot_id)
			{
				sprintf(file_path, "/dbm/shm/ethport/shm%d", j+1);
			    product->sync_send_file(standby_master_slot_id, file_path, 0);
			}
		}
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	return reply;
}

