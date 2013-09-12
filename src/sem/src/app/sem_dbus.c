/******************************************************************************
Copyright (C) Autelan Technology

This software file is owned and distributed by Autelan Technology
*******************************************************************************

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
*******************************************************************************
* sem_dbus.c
*
* DESCRIPTION:
*  SEM  Module Dbus implement.
*
* DATE:
*  2011-04-13
*
* CREATOR:
*  zhangdx@autelan.com
*
* CHANGE LOG:
*  2011-04-13 <zhangdx> Create file.
*
******************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <dbus/sem/sem_dbus_def.h>
#include <dbus/dbus.h>
#include "sem_dbus.h"
#include <sys/syslog.h>
#include <linux/tipc.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "sem/sem_tipc.h"
#include "sem_common.h"
#include "product/board/board_feature.h"
#include "product/product_feature.h"

/* MAX VRID Number */
#define MAX_VRID			16
#define SEM_PATH_MAX		64
extern product_info_syn_t product_info_syn;
extern board_fix_param_t *local_board;
extern thread_index_sd_t thread_id_arr[MAX_SLOT_NUM];/*clx*/
extern sd_sockaddr_tipc_t g_send_sd_arr[MAX_SLOT_NUM];/*clx*/
extern sd_sockaddr_tipc_t g_recv_sd_arr[MAX_SLOT_NUM];/*clx*/
extern product_fix_param_t *product;/*clx*/
extern int miisw_read(unsigned int devaddr, unsigned int regaddr,unsigned int phyaddr);
extern int miisw_write(unsigned int devaddr, unsigned int regaddr, unsigned short value,unsigned int phyaddr);
extern DBusMessage * sem_dbus_config_port_interface_mode(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage * sem_dbus_config_port_mode(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage * sem_dbus_config_ethport(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage * sem_dbus_config_ethport_attr(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage * sem_dbus_show_ethport_attr(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage * sem_dbus_reboot(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage * sem_dbus_reset_all(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage * sem_dbus_syn_file(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage * sem_dbus_set_boot_img(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage * sem_dbus_show_system_environment(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage * sem_dbus_set_system_img(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage * sem_dbus_config_ethport_media(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage * sem_dbus_ethports_show_running_config(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage * sem_dbus_get_slot_port_by_portindex(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage * sem_dbus_ethports_interface_show_ethport_stat(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage * sem_dbus_ethports_interface_clear_ethport_stat(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage * sem_dbus_delete_sub_intf(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage * sem_dbus_mcb_active_standby_switch(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_disable_keep_alive(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage * sem_dbus_ethports_interface_show_ethport_list( DBusConnection *conn,  DBusMessage *msg,  void *user_data);
extern DBusMessage * sem_dbus_create_sub_intf(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_mcb_active_standby_switch(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_execute_system_command(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage * sem_dbus_config_ethport_qinq_type(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_sync_master_eth_port_info(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_if_wan_state_set(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_read_car(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_show_car_list(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_get_car_list_count(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_write_car(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_write_cam_core(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_set_hash_aging_time(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_set_hash_update_time(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_read_cam_core(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_write_cam(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_read_cam(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_show_hash_capacity(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_show_port_counter(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_show_fpga_reg(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_show_car_list_debug(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_show_fpga_car_valid(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_empty_car_table(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_write_car_subnet(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_config_fpga_sysreg_working(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_config_fpga_sysreg_QoS(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_config_fpga_sysreg_car_update_cfg(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_config_fpga_sysreg_trunk_tag_type(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_config_fpga_sysreg_wan_tag_type(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_config_fpga_sysreg_car_linkup(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_config_fpga_sysreg_car_linkdown(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_config_fpga_sysreg_mode(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_show_hash_statistics(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_show_ram_detection(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_check_board_type(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_show_car_white_list(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_get_car_white_list_count(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_restore_car_table(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_write_white_car(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_read_hash(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_show_fpga_hash_valid(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_show_fpga_running_config(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_local_check_board_type(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_show_fpga_reg_arr(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_set_fpga_reset(DBusConnection *conn, DBusMessage *msg, void  *user_data);
extern DBusMessage *sem_dbus_write_fpga(DBusConnection *conn, DBusMessage *msg, void  *user_data);
extern DBusMessage *sem_dbus_show_wan_if(DBusConnection *conn, DBusMessage *msg, void  *user_data);
extern DBusMessage *sem_dbus_get_wan_if_num(DBusConnection *conn, DBusMessage *msg, void  *user_data);

extern DBusMessage *sem_system_debug_enable(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_system_debug_disable(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_apply_patch(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *show_all_slot_sys_info(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_delete_patch(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage * sem_dbus_show_boot_env_var(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage * sem_dbus_set_bootcmd(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage * sem_dbus_set_boot_env_var(DBusConnection *conn, DBusMessage *msg, void *user_data);
/*added by zhaocg 2012-10-30*/
extern DBusMessage *sem_dbus_md5_img_slot(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_md5_patch_slot(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_img_or_fastfwd_slot(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_delete_img_or_fastfwd_slot(DBusConnection *conn, DBusMessage *msg, void *user_data);

extern DBusMessage *sem_dbus_user_add_slot(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_user_del_slot(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_user_role_slot(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_user_show_slot(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_user_show_running(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_user_passwd_slot(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_user_is_exsit_slot(DBusConnection *conn, DBusMessage *msg, void *user_data);

extern DBusMessage *sem_dbus_download_img_slot(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *sem_dbus_download_cpy_config_slot(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage  *flash_erase_partition(DBusConnection *conn, DBusMessage *msg, void *user_data);
extern DBusMessage *write_boot_to_flash(DBusConnection *conn, DBusMessage *msg, void  *user_data);

extern DBusMessage *sem_dbus_write_cpld(DBusConnection *conn, DBusMessage *msg, void  *user_data);
extern DBusMessage *sem_dbus_read_cpld(DBusConnection *conn, DBusMessage *msg, void  *user_data);
extern DBusMessage *sem_dbus_sem_send_trap(DBusConnection *conn, DBusMessage *msg, void  *user_data);
extern DBusMessage *sem_dbus_show_6185(DBusConnection *conn, DBusMessage *msg, void  *user_data);
extern DBusMessage *sem_dbus_set_6185(DBusConnection *conn, DBusMessage *msg, void  *user_data);

static DBusConnection * sem_dbus_conn = NULL;

/***************************************************************************/
/**
* Descripton:
*  
*
*/

DBusMessage *sem_conf_48GE(DBusConnection *conn,
						DBusMessage *msg,
						void *user_data)
{
	DBusMessage* reply;
    DBusMessageIter iter;
    DBusError err;

	char *name;
	
	name = (char *)malloc(MAX_BOARD_NAME_LEN + 1);
	memset(name, 0, MAX_BOARD_NAME_LEN+1);
	dbus_error_init(&err);

	strcpy(name, product_info_syn.product_slot_board_info[2].name);
	
	reply = dbus_message_new_method_return(msg);
    dbus_message_iter_init_append(reply, &iter);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &name);

	free(name);
	return reply;
}


void send_file(void *arg)
{
	char file_path[30];
	char send_buf[60];
	int i = *(int *)arg;
	int fd;
	fd = open(MD5_PATH, O_RDONLY);
	if(fd < 0)
	{
		sem_syslog_dbg("file %s open failed.%s\n", MD5_PATH, __FUNCTION__);
        return;
	}
	read(fd, send_buf, 60);
	close(fd);
	sprintf(file_path, TESTFILE_PATH);
	product->sync_send_file(i, file_path, 0);
	sem_tipc_send(i, SEM_SEND_FILE_TEST, send_buf, sizeof(send_buf));
}

DBusMessage *sem_send_file(DBusConnection *conn,
						DBusMessage *msg,
						void *user_data)
{
	DBusMessage* reply;
    DBusMessageIter iter;
    DBusError err;

	int active_master_slot_id, i;
	static int arg[MAX_SLOT_COUNT];
	pthread_t p_send[MAX_SLOT_COUNT];
	
	dbus_error_init(&err);

	if(product->active_master_slot_id == product->master_slot_id[1])
		active_master_slot_id = product->master_slot_id[1];
	else
		active_master_slot_id = product->master_slot_id[0];

	for (i = 0; i < product->slotcount; i++)
	{
		arg[i]=i;
		if(i != active_master_slot_id)
		{
			if (sem_thread_create(&p_send[i], (void *)send_file, &arg[i], !IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
		    {
		    	sem_syslog_warning("create thread p_send%d failed.\n", i);
		    }
		}
	}

	reply = dbus_message_new_method_return(msg);

	return reply;
}

void make_XOR_checkcode(char * input_buf,char * checkcode)
{
    int m=1024; 
    int i,n;
        
    char ret;       
    
    char s[m];
    char b[9];
    
    int x=0x80;       
    
    memcpy(s,input_buf, m);
    
    for(i=0;i<m-1;i++)
    {
     if(i==0)
     {
         ret=s[i]^s[i+1]; 
     }
     else
     {
         ret=ret^s[i+1]; 
     }
    }
    
    for(n=0;n<8;n++)
    {
     if((ret&x)==0)
     {
         b[n]='0'; 
     }
     else
     {
         b[n]='1';
     }
     
     x=x>>1;
    }
    
    b[8]='\0';
            
    
    strcpy(checkcode, b);
    
}


DBusMessage *sem_conf_tipc(DBusConnection *conn,
						DBusMessage *msg,
						void *user_data)
{
	DBusMessage* reply;
    DBusMessageIter iter;
    DBusError err;	
	dbus_error_init(&err);
	int slot_id, num, usleep_tm;
	sem_pdu_head_t *head;
	sem_tlv_t *tlv_head;
	tipc_buf_t *tipcbuf;
	char *data;
	int i, len;
	char chkcode[9];
	int addr_len;
	if (!(dbus_message_get_args (msg, &err,
								DBUS_TYPE_INT32, &slot_id,
								DBUS_TYPE_INT32, &num,
								DBUS_TYPE_INT32, &usleep_tm,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err))
		{
			dbus_error_free(&err);
		}
		return NULL;
	}
	
	char recv_buf[SEM_TIPC_RECV_BUF_LEN];
	char send_buf[SEM_TIPC_SEND_BUF_LEN];
	char input_str[SEM_TIPC_SEND_BUF_LEN];
	memset(send_buf, 0x0, sizeof(send_buf));
	memset(input_str, 0x0, sizeof(input_str)); 
	memset(chkcode, 0x0, sizeof(chkcode)); 
	
	if (slot_id < 1 || slot_id > product_info_syn.slotcount)
	{
		sem_syslog_dbg("input slot_id error\n");
		return NULL;
	}
	
	head = (sem_pdu_head_t *)send_buf;
	head->slot_id = local_board->slot_id;
	head->version = 11;
	head->length = 22;

	tlv_head = (sem_tlv_t *)(send_buf + sizeof(sem_pdu_head_t));
	tlv_head->type = SEM_TIPC_TEST;
	tlv_head->length = 33;

	tipcbuf = (tipc_buf_t *)(send_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
	tipcbuf->num = 0;
	tipcbuf->total = num;

	data = (char*)(send_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t) + sizeof(tipc_buf_t));
	len = sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t) + sizeof(tipc_buf_t);
	for (i = 0; i<SEM_TIPC_SEND_BUF_LEN-len; i++)
		data[i] = 'a' + i%22;

	while (tipcbuf->num<num)
	{
		memset(tipcbuf->checkcode, 0, 9);
		make_XOR_checkcode(send_buf, chkcode);
		strcpy(tipcbuf->checkcode, chkcode);
		//sem_syslog_warning("tipcbuf->num=%d, tipcbuf->total=%d, tipcbuf->checkcode=%s\n", tipcbuf->num, tipcbuf->total, tipcbuf->checkcode);
		//sem_syslog_warning("tipcbuf->checkcode=%s, chkcode=%s\n", tipcbuf->checkcode, chkcode);		if (thread_id_arr[slot_id-1].sd >= 0 && thread_id_arr[slot_id-1].slot_id != USELESS_SLOT_ID)
		if (sendto(g_send_sd_arr[thread_id_arr[slot_id-1].slot_id].sd, send_buf, sizeof(send_buf), 0, (struct sockaddr*)&g_send_sd_arr[thread_id_arr[slot_id-1].slot_id].sock_addr, sizeof(struct sockaddr_tipc)) < 0)
		{
			sem_syslog_warning("%s send failed\n", __FUNCTION__);
			return NULL;
		}
		else
		{
			//sem_syslog_warning("send to slot %d sd = %d succeed, num = %d\n", thread_id_arr[slot_id-1].slot_id+1, thread_id_arr[slot_id-1].sd, tipcbuf->num);
		}
		tipcbuf->num++;
		usleep(usleep_tm);
	}
    reply = dbus_message_new_method_return(msg);
	return reply;
}

DBusMessage *sem_dbus_show_info(DBusConnection *conn,
                        DBusMessage *msg,
                        void *user_data)
{
    DBusMessage* reply;
    DBusMessageIter iter;
    DBusError err;
    unsigned char able;
    int ret = 0;

    if (!(dbus_message_get_args ( msg, &err,
                                DBUS_TYPE_BYTE,&able,
                                DBUS_TYPE_INVALID)))
    {               
        if (dbus_error_is_set(&err))
        {
            printf("%s: %s raised %s\n", __func__, err.name, err.message);
            dbus_error_free(&err);
        }

        return NULL;
    }

    switch(able)
    {
		case 1:
		    able = local_board->slot_id;
			break;
		default:
			break;
    }
    
    reply = dbus_message_new_method_return(msg);
    dbus_message_iter_init_append(reply, &iter);

    dbus_error_init(&err);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &ret);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_BYTE, &able);

    printf("able = %d\n", able);

    return reply;

}


DBusMessage *sem_dbus_show_slot_id(DBusConnection *conn,
                        DBusMessage *msg,
                        void *user_data)
{
	DBusMessage *reply = NULL;
	DBusMessageIter iter;
	DBusError err;

	int slot_id = 0;

	slot_id = local_board->slot_id+1;
	dbus_error_init(&err);
	
	reply = dbus_message_new_method_return(msg);
    dbus_message_iter_init_append(reply, &iter);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &slot_id);
    return reply;
}


DBusMessage *sem_dbus_show_6185(DBusConnection *conn,
                        DBusMessage *msg,
                        void *user_data)
{
	DBusMessage *reply = NULL;
	DBusMessageIter iter;
	DBusError err;
    unsigned int dev_addr;
	unsigned int reg_addr;
	unsigned int phy_addr;
	unsigned int val;

	sem_syslog_dbg("sem_dbus_show_6185\n");
	dbus_error_init(&err);

	if (!(dbus_message_get_args (msg, &err,
        				 		DBUS_TYPE_UINT32, &dev_addr,
        				 		DBUS_TYPE_UINT32, &reg_addr,
        				 		DBUS_TYPE_UINT32, &phy_addr,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err))
		{
			dbus_error_free(&err);
		}
		return NULL;
	}
	
	//sem_syslog_dbg("dev_addr = 0x%x\n",dev_addr);
	//sem_syslog_dbg("reg_addr = 0x%x\n",reg_addr);
	val = miisw_read(dev_addr, reg_addr,phy_addr);
	//sem_syslog_dbg("val = 0x%x\n",val);
	
	reply = dbus_message_new_method_return(msg);
    dbus_message_iter_init_append(reply, &iter);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &val);
    return reply;
}

DBusMessage *sem_dbus_set_6185(DBusConnection *conn,
                        DBusMessage *msg,
                        void *user_data)
{
	DBusMessage *reply = NULL;
	DBusMessageIter iter;
	DBusError err;
    unsigned int dev_addr;
	unsigned int reg_addr;
	unsigned int phy_addr;
	unsigned short val;
	int ret;

	sem_syslog_dbg("sem_dbus_set_6185\n");
	dbus_error_init(&err);

	if (!(dbus_message_get_args (msg, &err,
        				 		DBUS_TYPE_UINT32, &dev_addr,
        				 		DBUS_TYPE_UINT32, &reg_addr,
        				 		DBUS_TYPE_UINT16, &val,
        				 		DBUS_TYPE_UINT32, &phy_addr,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err))
		{
			dbus_error_free(&err);
		}
		return NULL;
	}
	
	//sem_syslog_dbg("dev_addr = 0x%x\n",dev_addr);
	//sem_syslog_dbg("reg_addr = 0x%x\n",reg_addr);
	//sem_syslog_dbg("val = 0x%x\n",val);
	ret = miisw_write(dev_addr, reg_addr,val,phy_addr);
	
	reply = dbus_message_new_method_return(msg);
    dbus_message_iter_init_append(reply, &iter);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &ret);
    return reply;
}



DBusMessage *sem_dbus_show_slot_n_info(DBusConnection *conn,
                        DBusMessage *msg,
                        void *user_data)
{
	DBusMessage *reply = NULL;
	DBusMessageIter iter;
	DBusError err;
	int slot_id;
	int board_code;
	int is_master, is_active_master;
	unsigned int function_type;
	char *name;
	
	name = (char *)malloc(MAX_BOARD_NAME_LEN + 1);

	memset(name, 0, MAX_BOARD_NAME_LEN+1);
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args (msg, &err,
								DBUS_TYPE_INT32, &slot_id,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err))
		{
			dbus_error_free(&err);
		}
		free(name);
		return NULL;
	}

	if (slot_id < 1 || slot_id > product_info_syn.slotcount)
	{
		sem_syslog_dbg("input slot_id error\n");
		free(name);
		return NULL;
	}
	
	board_code = product_info_syn.product_slot_board_info[slot_id-1].board_code;
	function_type = product_info_syn.product_slot_board_info[slot_id-1].function_type;
	is_master = product_info_syn.product_slot_board_info[slot_id-1].is_master;
	is_active_master = product_info_syn.product_slot_board_info[slot_id-1].is_active_master;
	strcpy(name, product_info_syn.product_slot_board_info[slot_id-1].name);
	/*
	sem_syslog_dbg("slotcount:\t\t%d\n", product_info_syn.slotcount);
	sem_syslog_dbg("SLOT_ID:\t\t%d\n", slot_id);
	sem_syslog_dbg("BOARD_CODE:\t\t0x%x\n", board_code);
	sem_syslog_dbg("FUNCTION_TYPE:\t\t0x%x\n", function_type);
	sem_syslog_dbg("IS_MASTER:\t\t%s\n", is_master ? "YES" : "NO");
	sem_syslog_dbg("IS_ACTIVE_MASTER:\t%s\n", is_active_master ? "YES" : "NO");
	sem_syslog_dbg("BOARD_NAME:\t\t%s\n", name);
	*/
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append(reply, &iter);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &board_code);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &function_type);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &is_master);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &is_active_master);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &name);

	free(name);
	return reply;
}


DBusMessage *sem_dbus_show_board_info(DBusConnection *conn,
                        DBusMessage *msg,
                        void *user_data)
{
	DBusMessage *reply = NULL;
	DBusMessageIter iter;
	DBusError err;

	int board_code;
	char *name;
	int slot_id;
	int is_master, is_active_master;
	unsigned int port_num_on_panel, function_type, obc_port_num, cscd_port_num;

	name = (char *)malloc(MAX_BOARD_NAME_LEN + 1);

	memset(name, 0, MAX_BOARD_NAME_LEN+1);
	dbus_error_init(&err);

	board_code = local_board->board_code;
	cscd_port_num = local_board->cscd_port_num;
	function_type = local_board->function_type;
	is_active_master = local_board->is_active_master;
	is_master = local_board->is_master;
	strcpy(name, local_board->name);
	obc_port_num = local_board->obc_port_num;
	port_num_on_panel = local_board->port_num_on_panel;
	slot_id = local_board->slot_id+1;
	
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append(reply, &iter);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &board_code);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &cscd_port_num);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &function_type);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &is_active_master);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &is_master);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &name);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &obc_port_num);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &port_num_on_panel);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &slot_id);
	
	free(name);
	return reply;
	#if 0
	strcpy(temp_buf, "/dbm/local_board");
	
	strcpy(temp_buf2, temp_buf);
	strcat(temp_buf2, "/board_code");
	fd = fopen(temp_buf2, "r");
	if (fd == NULL)
	{
		
	}
	fscanf(fd, "%d", &board_code);
	fclose(fd);

	strcpy(temp_buf2, temp_buf);
	strcat(temp_buf2, "/cscd_port_num");
	fd = fopen(temp_buf2, "r");
	if (fd == NULL)
	{
		
	}
	fscanf(fd, "%u", &cscd_port_num);
	fclose(fd);

	strcpy(temp_buf2, temp_buf);
	strcat(temp_buf2, "/function_type");
	fd = fopen(temp_buf2, "r");
	if (fd == NULL)
	{
		
	}
	fscanf(fd, "%u", &function_type);
	fclose(fd);

	strcpy(temp_buf2, temp_buf);
	strcat(temp_buf2, "/is_active_master");
	fd = fopen(temp_buf2, "r");
	if (fd == NULL)
	{
		
	}
	fscanf(fd, "%d", &is_active_master);
	fclose(fd);

	strcpy(temp_buf2, temp_buf);
	strcat(temp_buf2, "/is_master");
	fd = fopen(temp_buf2, "r");
	if (fd == NULL)
	{
		
	}
	fscanf(fd, "%d", &is_master);
	fclose(fd);

	strcpy(temp_buf2, temp_buf);
	strcat(temp_buf2, "/name");
	fd = fopen(temp_buf2, "r");
	if (fd == NULL)
	{
		
	}
	fscanf(fd, "%s", name);
	fclose(fd);

	strcpy(temp_buf2, temp_buf);
	strcat(temp_buf2, "/obc_port_num");
	fd = fopen(temp_buf2, "r");
	if (fd == NULL)
	{
		
	}
	fscanf(fd, "%u", &obc_port_num);
	fclose(fd);

	strcpy(temp_buf2, temp_buf);
	strcat(temp_buf2, "/port_num_on_panel");
	fd = fopen(temp_buf2, "r");
	if (fd == NULL)
	{
		
	}
	fscanf(fd, "%u", &port_num_on_panel);
	fclose(fd);

	strcpy(temp_buf2, temp_buf);
	strcat(temp_buf2, "/slot_id");
	fd = fopen(temp_buf2, "r");
	if (fd == NULL)
	{
		
	}
	fscanf(fd, "%d", &slot_id);
	fclose(fd);
	#endif
}

DBusMessage *sem_dbus_show_product_info(DBusConnection *conn,
                        DBusMessage *msg,
                        void *user_data)
{
	DBusMessage *reply = NULL;
	DBusMessageIter iter;
	DBusError err;

	int product_type, slotcount, master_slot_count, default_master_slot_id;
	int more_than_one_master_board_on, is_distributed;
	int fan_num, master_slot_id[MAX_MASTER_SLOT_NUM];
	unsigned int board_on_mask;
	char *name;
	int i;

	name = (char *)malloc(PRODUCT_NAME_LEN + 1);
	memset(name, 0, PRODUCT_NAME_LEN+1);

	dbus_error_init(&err);

	board_on_mask = product_info_syn.board_on_mask;
	default_master_slot_id = product_info_syn.default_master_slot_id+1;
	fan_num = product_info_syn.fan_num;
	is_distributed = product_info_syn.is_distributed;
	master_slot_count = product_info_syn.master_slot_count;
	for (i=0; i<MAX_MASTER_SLOT_NUM; i++)
	{
		master_slot_id[i] = product_info_syn.master_slot_id[i]+1;
	}
	more_than_one_master_board_on = product_info_syn.more_than_one_master_board_on;
	strcpy(name, product_info_syn.name);
	product_type = product_info_syn.product_type;
	slotcount = product_info_syn.slotcount;
	
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append(reply, &iter);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &board_on_mask);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &default_master_slot_id);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &fan_num);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &is_distributed);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &master_slot_count);
	for (i=0; i<MAX_MASTER_SLOT_NUM; i++)
	{
		dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &master_slot_id[i]);
	}
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &more_than_one_master_board_on);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &name);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &product_type);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &slotcount);
	
	free(name);
	return reply;
}


DBusMessage *sem_dbus_show_slot_info(DBusConnection *conn,
                        DBusMessage *msg,
                        void *user_data)
{
	DBusMessage *reply = NULL;
	DBusMessageIter iter;
	DBusError err;

	int i;
	int board_code;
	int is_master, is_active_master;
	unsigned int function_type;
	char *name;
	int slot_count;
	unsigned int board_on_mask;
	int board_state;

	name = (char *)malloc(MAX_BOARD_NAME_LEN + 1);
	memset(name, 0, MAX_BOARD_NAME_LEN+1);

	dbus_error_init(&err);

	slot_count = product_info_syn.slotcount;
	board_on_mask = product_info_syn.board_on_mask;

	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append(reply, &iter);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &slot_count);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &board_on_mask);

	for (i=0; i<slot_count; i++)
	{
			board_state = product_info_syn.product_slot_board_info[i].board_state;
			board_code = product_info_syn.product_slot_board_info[i].board_code;
			function_type = product_info_syn.product_slot_board_info[i].function_type;
			is_master = product_info_syn.product_slot_board_info[i].is_master;
			is_active_master = product_info_syn.product_slot_board_info[i].is_active_master;
			strcpy(name, product_info_syn.product_slot_board_info[i].name);	
			dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &board_state);
			dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &board_code);
			dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &function_type);
			dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &is_master);
			dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &is_active_master);
			dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &name);
	}
	free(name);
	return reply;
}

/* added by zhengbo for upload snapshot */
#define SNAPSHOT_PATH "/mnt/snapshot.zip"
#define SNAPSHOT_PASSWD "autelan.com_Snapshot_1.1"
/*
 * dbus return value -1 failed
 * dbus return value  0 success
 * dbus return value  1 no snapshot to upload
 * dbus return value  2 compress snapshot failed
 */
DBusMessage *sem_dbus_upload_snapshot(DBusConnection *conn,
                        DBusMessage *msg,
                        void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter	 iter;
	DBusError err;
	char cmd[1024];
	int ret = 0, cmd_ret = 0;

	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err, DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
			ret = -1;
			goto finish;
		}
	}

	sprintf(cmd, "rm -rf /mnt/snapshot %s", SNAPSHOT_PATH);
	system(cmd);

	sem_syslog_dbg("copy & compress snapshot...\n");

	system("sor.sh cpfromblk snapshot 60");
	ret = access("/mnt/snapshot", 0);
	if(ret)
	{
		sem_syslog_dbg("no snapshot to upload\n");
		ret = 1;
		goto finish;
	}

	sprintf(cmd,
			"zip -r -P %s %s /mnt/snapshot",
			SNAPSHOT_PASSWD, SNAPSHOT_PATH);
	system(cmd);
	ret = access(SNAPSHOT_PATH, 0);
	if(ret)
	{
		sem_syslog_dbg("compress snapshot failed\n");
		ret = 2;
		goto cleanup;
	}

cleanup:
	sprintf(cmd, "rm -rf /mnt/snapshot");
	system(cmd);

finish:
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append(reply, &iter);
 	dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &ret);
	return reply;
}

/**
* Description:
*  CAPWAP tunnel dbus node, show-running function.
*
*/
DBusMessage *sem_dbus_capwap_tunnel_show_running(DBusConnection *conn,
    DBusMessage *msg,
    void *user_data)
{
    DBusMessage *reply;
    DBusMessageIter iter;
    DBusError err;
    unsigned char *cmd = NULL;
    int rval = 0, stat = 0;
    FILE *stream1 = NULL;
    int rstat1 = 0;
    int len = 0, total_len = 0;
    unsigned char retbuf1[64] = {0};

    dbus_error_init(&err);
    cmd = (unsigned char*)malloc(1024);
    if (!cmd)
    {            
        goto out;
    }
    bzero(cmd, 1024);

    rval = system("test -d /sys/module/ip_fast_forwarding");
    stat = WEXITSTATUS(rval);
    /* ipfwd has been loaded */
    if (!stat)
    {
        len = sprintf(cmd, "config capwap-tunnel\nset flow-based-forwarding enable\nexit\n");
        total_len = len;
    }

    /* Set pmtu enable or disable */
    stream1 = popen("cat /proc/sys/net/ipv4/ipfrag_ingress_pmtu", "r");

    if (stream1 != NULL)
    {
        if(!fread(retbuf1, sizeof(char), sizeof(retbuf1), stream1))
		sem_syslog_dbg("fread error\n");
        pclose(stream1);
        rstat1 = atoi(retbuf1);
    }

    if (rstat1 == 1)
    {
        len = sprintf(cmd + total_len, "config capwap-tunnel\nset ipfrag_ingress_pmtu enable\nexit\n");
        total_len += len;
    }

out:
    reply = dbus_message_new_method_return(msg);
    dbus_message_iter_init_append (reply, &iter);
    dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &cmd);
	
    free(cmd);
    return reply;
}


/*
* Description:
*  Re-format path string based on vrid.
*
* Parameter:
*  path: new string buffer
*  base: old string pointer
*
* Return:
*  NULL: Failed.
*  path: Successed.
*
*/
unsigned char *get_vrid_path(unsigned char *path, unsigned char *base)
{
    if (path == NULL || base == NULL)
    {
        printf("%s: Parameter Dbus path == NULL.\n", __func__);
        return NULL;
    }

    sprintf (path, "%s", base);

    return path;
}



/*
* Description:
*  Re-format Path string for Hansi N + 1
*
* Parameter:
*  same to function dbus_message_is_method_call
*
* Return:
*  0: Failed.
*  nonzero: Successed, see return value of dbus_message_is_method_call for detail.
*
*/
unsigned int dbus_msg_is_method_call(DBusMessage *msg, unsigned char *dbus_if, unsigned char *path)
{
    unsigned char if_path[128] = {0};
    unsigned char cmd_path[128] = {0};

    if (get_vrid_path(if_path, dbus_if) == NULL || get_vrid_path(cmd_path, path) == NULL)
    {
        printf("%s: get_vrid_path failed\n", __func__);
        return 0;
    }

    return dbus_message_is_method_call(msg, if_path, cmd_path);
}

unsigned int sem_dbus_system_state_trap(unsigned int system_state) {
	DBusMessage* msg = NULL;
	static DBusConnection *sem_trap_conn = NULL;
	DBusError err;
	int ret = 0;
	unsigned int serial = 0;

	/* for debug*/
	sem_syslog_dbg("\ttrap system state: %d\n", system_state);

	dbus_error_init(&err);

	if (NULL == sem_trap_conn) {
		/* connect to the DBUS system bus, and check for errors */
		sem_trap_conn = dbus_bus_get_private(DBUS_BUS_SYSTEM, &err);
		if (dbus_error_is_set(&err)) {
			sem_syslog_dbg("Connection Error (%s)\n", err.message);
			dbus_error_free(&err);
			return 1;
		}
		if (NULL == sem_trap_conn) {
			sem_syslog_dbg("Connection null\n");
			return 1;
		}
	}

	/* register our name on the bus, and check for errors */
	ret = dbus_bus_request_name(sem_trap_conn,
								"aw.sem.system.state.signal",
								0,
								&err);
	if (dbus_error_is_set(&err)) {
		sem_syslog_dbg("Name Error (%s)\n", err.message);
		dbus_error_free(&err);
		return 1;
	}
	
	/* create a signal & check for errors */
	msg = dbus_message_new_signal(SEM_DBUS_OBJPATH, 			/* object name of the signal */
								  SEM_TRAP_INTERFACE,					/* interface name of the signal */
								  SEM_TRAP_SYSTEM_STATE); /* name of the signal */
	if (NULL == msg) {
		sem_syslog_dbg("Message Null\n");
		return 1;
	}
	
	/* append arguments onto signal */
	dbus_message_append_args(msg,
							DBUS_TYPE_UINT32, &system_state,
							DBUS_TYPE_INVALID);
	
	/* send the message and flush the connection */
	if (!dbus_connection_send(sem_trap_conn, msg, &serial)) {
		sem_syslog_dbg("Signal send error, Out Of Memory!\n"); 
		return 1;
	}
	
	dbus_connection_flush(sem_trap_conn);
	sem_syslog_dbg("\tsent message to trap.\n");

	/* free the message */
	dbus_message_unref(msg);
	
	return 0;
}

/**********************************************************************************
 * sem_dbus_power_state_abnormal
 *	- send warning message to tarp-helper, when  power state was changed
 *
 *	INPUT:
 *		power_state_flg  - 0 power  is ok, 1 power abnormal
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0x1 - if error occurred.
 *		0x0 - check success.
 *
 **********************************************************************************/
unsigned int sem_dbus_power_state_abnormal
(
	unsigned int power_state_flg,
	unsigned int power_index
)
{
	DBusMessage* msg = NULL;
	static DBusConnection *sem_trap_conn = NULL;
	DBusError err;
	int ret = 0;
	unsigned int serial = 0;

	/* for debug*/
	sem_syslog_dbg("\ttrap power state abnormal: power_state_flg %s\n",
						(power_state_flg == 1) ? "abnormal" :"ok");

	dbus_error_init(&err);

	if (NULL == sem_trap_conn) {
		/* connect to the DBUS system bus, and check for errors */
		sem_trap_conn = dbus_bus_get_private(DBUS_BUS_SYSTEM, &err);
		if (dbus_error_is_set(&err)) {
			sem_syslog_dbg("Connection Error (%s)\n", err.message);
			dbus_error_free(&err);
			return 1;
		}
		if (NULL == sem_trap_conn) {
			sem_syslog_dbg("Connection null\n");
			return 1;
		}
	}

	/* register our name on the bus, and check for errors */
	ret = dbus_bus_request_name(sem_trap_conn,
								"aw.sem.power.signal",
								0,
								&err);
	if (dbus_error_is_set(&err)) {
		sem_syslog_dbg("Name Error (%s)\n", err.message);
		dbus_error_free(&err);
		return 1;
	}
	
	/* create a signal & check for errors */
	msg = dbus_message_new_signal(SEM_DBUS_OBJPATH,				/* object name of the signal */
								  SEM_TRAP_INTERFACE, 					/* interface name of the signal */
								  SEM_DBUS_TRAP_POWER_STATE_CHANGE); /* name of the signal */
	if (NULL == msg) {
		sem_syslog_dbg("Message Null\n");
		return 1;
	}
	
	/* append arguments onto signal */
	dbus_message_append_args(msg,
							DBUS_TYPE_UINT32, &power_state_flg,
							DBUS_TYPE_UINT32, &power_index,
							DBUS_TYPE_INVALID);
	
	/* send the message and flush the connection */
	if (!dbus_connection_send(sem_trap_conn, msg, &serial)) {
		sem_syslog_dbg("Signal send error, Out Of Memory!\n"); 
		return 1;
	}
	
	dbus_connection_flush(sem_trap_conn);
	sem_syslog_dbg("\tsent message to trap.\n");

	/* free the message */
	dbus_message_unref(msg);

	return 0;
}

unsigned int sem_dbus_fan_state_abnormal
(
	unsigned int fan_state_flg
)
{
	DBusMessage* msg = NULL;
	static DBusConnection *sem_trap_conn = NULL;
	DBusError err;
	int ret = 0;
	unsigned int serial = 0;

	/* for debug*/
	sem_syslog_dbg("\ttrap fan state abnormal: fan_state_flg %s\n",
						(fan_state_flg < 1) ? "abnormal" :"ok");

	dbus_error_init(&err);

	if (NULL == sem_trap_conn) {
		/* connect to the DBUS system bus, and check for errors */
		sem_trap_conn = dbus_bus_get_private(DBUS_BUS_SYSTEM, &err);
		if (dbus_error_is_set(&err)) {
			sem_syslog_dbg("Connection Error (%s)\n", err.message);
			dbus_error_free(&err);
			return 1;
		}
		if (NULL == sem_trap_conn) {
			sem_syslog_dbg("Connection null\n");
			return 1;
		}
	}

	/* register our name on the bus, and check for errors */
	ret = dbus_bus_request_name(sem_trap_conn,
								"aw.sem.fan.signal",
								0,
								&err);
	if (dbus_error_is_set(&err)) {
		sem_syslog_dbg("Name Error (%s)\n", err.message);
		dbus_error_free(&err);
		return 1;
	}
	
	/* create a signal & check for errors */
	msg = dbus_message_new_signal(SEM_DBUS_OBJPATH,				/* object name of the signal */
								  SEM_TRAP_INTERFACE, 					/* interface name of the signal */
								  SEM_DBUS_TRAP_FAN_STATE_CHANGE); /* name of the signal */
	if (NULL == msg) {
		sem_syslog_dbg("Message Null\n");
		return 1;
	}
	
	/* append arguments onto signal */
	dbus_message_append_args(msg,
							DBUS_TYPE_UINT32, &fan_state_flg,
							DBUS_TYPE_INVALID);
	
	/* send the message and flush the connection */
	if (!dbus_connection_send(sem_trap_conn, msg, &serial)) {
		sem_syslog_dbg("Signal send error, Out Of Memory!\n"); 
		return 1;
	}
	
	dbus_connection_flush(sem_trap_conn);
	sem_syslog_dbg("\tsent message to trap.\n");

	/* free the message */
	dbus_message_unref(msg);

	return 0;
}

unsigned int sem_dbus_board_state_trap
(
	unsigned int board_state,
	unsigned int slot_id
)
{
	DBusMessage* msg = NULL;
	static DBusConnection *sem_trap_conn = NULL;
	DBusError err;
	int ret = 0;
	unsigned int serial = 0;

	/* for debug*/
	sem_syslog_dbg("\ttrap board state : %d\n", board_state);

	dbus_error_init(&err);

	if (NULL == sem_trap_conn) {
		/* connect to the DBUS system bus, and check for errors */
		sem_trap_conn = dbus_bus_get_private(DBUS_BUS_SYSTEM, &err);
		if (dbus_error_is_set(&err)) {
			sem_syslog_dbg("Connection Error (%s)\n", err.message);
			dbus_error_free(&err);
			return 1;
		}
		if (NULL == sem_trap_conn) {
			sem_syslog_dbg("Connection null\n");
			return 1;
		}
	}

	/* register our name on the bus, and check for errors */
	ret = dbus_bus_request_name(sem_trap_conn,
								"aw.sem.board.state.signal",
								0,
								&err);
	if (dbus_error_is_set(&err)) {
		sem_syslog_dbg("Name Error (%s)\n", err.message);
		dbus_error_free(&err);
		return 1;
	}
	
	/* create a signal & check for errors */
	msg = dbus_message_new_signal(SEM_DBUS_OBJPATH,				/* object name of the signal */
								  SEM_TRAP_INTERFACE, 					/* interface name of the signal */
								  SEM_TRAP_BOARD_STATE); /* name of the signal */
	if (NULL == msg) {
		sem_syslog_dbg("Message Null\n");
		return 1;
	}
	
	/* append arguments onto signal */
	dbus_message_append_args(msg,
							DBUS_TYPE_UINT32, &board_state,
							DBUS_TYPE_UINT32, &slot_id,
							DBUS_TYPE_INVALID);
	
	/* send the message and flush the connection */
	if (!dbus_connection_send(sem_trap_conn, msg, &serial)) {
		sem_syslog_dbg("Signal send error, Out Of Memory!\n"); 
		return 1;
	}
	
	dbus_connection_flush(sem_trap_conn);
	sem_syslog_dbg("\tsent message to trap.\n");

	/* free the message */
	dbus_message_unref(msg);

	return 0;
}

static DBusHandlerResult sem_dbus_msg_handler (
    DBusConnection *conn,
    DBusMessage *msg,
    void *user_data)
{
    DBusMessage *reply = NULL;
    unsigned char sem_dbus_vrid_path[SEM_PATH_MAX] = {0};

    if (strcmp(dbus_message_get_path(msg), SEM_DBUS_OBJPATH) == 0)
    {
        if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SHOW_INFO))
		{
            reply = sem_dbus_show_info(conn,msg,user_data);
        }
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SHOW_SLOT_ID))
		{
			reply = sem_dbus_show_slot_id(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SHOW_6185))
		{
			reply = sem_dbus_show_6185(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SET_6185))
		{
			reply = sem_dbus_set_6185(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_CONF_48GE))
		{
			reply = sem_conf_48GE(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_CONF_TIPC))
		{
			reply = sem_conf_tipc(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SEND_FILE))
		{
			reply = sem_send_file(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SME_DBUS_SHOW_SLOT_N_INFO))
		{
			reply = sem_dbus_show_slot_n_info(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SHOW_SLOT_INFO))
		{
			reply = sem_dbus_show_slot_info(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_APPLY_PATCH))
		{
			reply = sem_dbus_apply_patch(conn, msg, user_data);		
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_DELETE_PATCH))
		{
			reply = sem_dbus_delete_patch(conn, msg, user_data);		
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_CONFIG_SEM_TO_TRAP))
		{
			reply = sem_dbus_sem_send_trap(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SHOW_BOARD_INFO))
		{
			reply = sem_dbus_show_board_info(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SHOW_PRODUCT_INFO))
		{
			reply = sem_dbus_show_product_info(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_INTERFACE_METHOD_SYSTEM_DEBUG_STATE))
		{
			reply = sem_system_debug_enable(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_INTERFACE_METHOD_SYSTEM_UNDEBUG_STATE))
		{
			reply = sem_system_debug_disable(conn, msg, user_data);
		}

		
		/********************************************
			xufujun
	  ********************************************/
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE,SEM_DBUS_FLASH_ERASE_PARTITIO))
		{
			reply = flash_erase_partition(conn, msg, user_data);		
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE,WRITE_BOOT_TO_FLASH))
		{
			reply = write_boot_to_flash (conn, msg, user_data);		
		}
		

		
        else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SUB_INTERFACE_ETHPORT_QINQ_TYPE_SET))
		{
			reply = sem_dbus_config_ethport_qinq_type(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_ETHPORTS_INTERFACE_METHOD_CONFIG_ETHPORT_INTERFACE))
		{
			reply = sem_dbus_config_port_interface_mode(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_ETHPORTS_INTERFACE_METHOD_CONFIG_ETHPORT_MODE))
		{
		    reply = sem_dbus_config_port_mode(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_ETHPORTS_INTERFACE_METHOD_CONFIG_ETHPORT_MODE))
		{
			reply = sem_dbus_config_port_mode(conn, msg, user_data);
		}			
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_CONFIG_ETHPORT))
		{
			reply = sem_dbus_config_ethport(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_CONFIG_ETHPORT_ATTR))
		{
			reply = sem_dbus_config_ethport_attr(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SHOW_ETHPORT_ATTR))
		{
			reply = sem_dbus_show_ethport_attr(conn, msg, user_data);
		}
/*		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_CONFIG_MEDIA_PREFERR))
		{
			reply = sem_dbus_config_ethport_media(conn, msg, user_data);
		}
*/		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_REBOOT))
		{
			reply = sem_dbus_reboot(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_RESET_ALL))
		{
			reply = sem_dbus_reset_all(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SYN_FILE))
		{
			reply = sem_dbus_syn_file(conn, msg, user_data);		
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SET_BOOT_IMG))
		{
			reply = sem_dbus_set_boot_img(conn, msg, user_data);		
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_MCB_ACTIVE_STANDBY_SWITCH))
		{
			reply = sem_dbus_mcb_active_standby_switch(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SET_IF_WAN_STATE))
		{
			reply = sem_dbus_if_wan_state_set(conn, msg, user_data);	
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SHOW_PORT_COUNTER))
		{
			reply = sem_dbus_show_port_counter(conn, msg, user_data);	
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_CHECK_BOARD_TYPE))
		{
			reply = sem_dbus_check_board_type(conn, msg, user_data);	
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_LOCAL_CHECK_BOARD_TYPE))
		{
			reply = sem_dbus_local_check_board_type(conn, msg, user_data);	
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SHOW_HASH_STATISTICS))
		{
			reply = sem_dbus_show_hash_statistics(conn, msg, user_data);	
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_CAR_READ))
		{
			reply = sem_dbus_read_car(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SHOW_CAR_LIST))
		{
			reply = sem_dbus_show_car_list(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SHOW_CAR_WHITE_LIST))
		{
			reply = sem_dbus_show_car_white_list(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_GET_CAR_LIST_COUNT))
		{
			reply = sem_dbus_get_car_list_count(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_GET_CAR_WHITE_LIST_COUNT))
		{
			reply = sem_dbus_get_car_white_list_count(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_GET_CAR_TABLE_RESTORE))
		{
			reply = sem_dbus_restore_car_table(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_GET_CAR_TABLE_EMPTY))
		{
			reply = sem_dbus_empty_car_table(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SHOW_FPGA_WAN_IF))
		{
			reply = sem_dbus_show_wan_if(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_GET_FPGA_WAN_IF_NUM))
		{
			reply = sem_dbus_get_wan_if_num(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_CAR_WRITE))
		{
			reply = sem_dbus_write_car(conn, msg, user_data);	
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_CAR_WHITE_WRITE))
		{
			reply = sem_dbus_write_white_car(conn, msg, user_data);	
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_CAR_SUBNET_WRITE))
		{
			reply = sem_dbus_write_car_subnet(conn, msg, user_data);	
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_CAM_CORE_WRITE))
		{
			reply = sem_dbus_write_cam_core(conn, msg, user_data);	
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SET_HASH_AGING_TIME))
		{
			reply = sem_dbus_set_hash_aging_time(conn, msg, user_data);	
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SET_HASH_UPDATE_TIME))
		{
			reply = sem_dbus_set_hash_update_time(conn, msg, user_data);	
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SHOW_HASH_CAPACITY))
		{
			reply = sem_dbus_show_hash_capacity(conn, msg, user_data);	
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SHOW_FPGA_DDR_QDR))
		{
			reply = sem_dbus_show_ram_detection(conn, msg, user_data);	
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SHOW_FPGA_REG_ARR))
		{
			reply = sem_dbus_show_fpga_reg_arr(conn, msg, user_data);	
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SHOW_FPGA_REG))
		{
			reply = sem_dbus_show_fpga_reg(conn, msg, user_data);	
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_CONFIG_FPGA_SYSREG_WORKING))
		{
			reply = sem_dbus_config_fpga_sysreg_working(conn, msg, user_data);	
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_CONFIG_FPGA_SYSREG_QOS))
		{
			reply = sem_dbus_config_fpga_sysreg_QoS(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_CONFIG_FPGA_SYSREG_MODE))
		{
			reply = sem_dbus_config_fpga_sysreg_mode(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_CONFIG_FPGA_SYSREG_CAR_UPDATE_CFG))
		{
			reply = sem_dbus_config_fpga_sysreg_car_update_cfg(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_CONFIG_FPGA_SYSREG_TRUNK_TAG_TYPE))
		{
			reply = sem_dbus_config_fpga_sysreg_trunk_tag_type(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_CONFIG_FPGA_SYSREG_WAN_TAG_TYPE))
		{
			reply = sem_dbus_config_fpga_sysreg_wan_tag_type(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_CONFIG_FPGA_SYSREG_CAR_LINKUP))
		{
			reply = sem_dbus_config_fpga_sysreg_car_linkup(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SET_FPGA_RESET))
		{
			reply = sem_dbus_set_fpga_reset(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_CONFIG_WRITE_FPGA))
		{
			reply = sem_dbus_write_fpga(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_CONFIG_WRITE_CPLD))
		{
			reply = sem_dbus_write_cpld(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_CONFIG_READ_CPLD))
		{
			reply = sem_dbus_read_cpld(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_FPGA_SHOW_RUNNING_CONFIG))
		{
			reply = sem_dbus_show_fpga_running_config(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_CONFIG_FPGA_SYSREG_CAR_LINKDOWN))
		{
			reply = sem_dbus_config_fpga_sysreg_car_linkdown(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SHOW_FPGA_HASH_VALID))
		{
			reply = sem_dbus_show_fpga_hash_valid(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SHOW_FPGA_CAR_VALID))
		{
			reply = sem_dbus_show_fpga_car_valid(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_CAM_CORE_READ))
		{
			reply = sem_dbus_read_cam_core(conn, msg, user_data);	
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_CAM_WRITE))
		{
			reply = sem_dbus_write_cam(conn, msg, user_data);	
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_CAM_READ))
		{
			reply = sem_dbus_read_cam(conn, msg, user_data);	
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SHOW_HASH_TABLE))
		{
			reply = sem_dbus_read_hash(conn, msg, user_data);	
		}	
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SHOW_SYSTEM_ENVIRONMENT))
		{
			reply = sem_dbus_show_system_environment(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SET_SYSTEM_IMG))
		{
			reply = sem_dbus_set_system_img(conn, msg, user_data);	
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SHOW_BOOTROM_ENVIRONMENT_VARIABLE))
		{
			reply = sem_dbus_show_boot_env_var(conn, msg, user_data);	
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SET_BOOTCMD))
		{
			reply = sem_dbus_set_bootcmd(conn, msg, user_data);	
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SET_ENVIRONMENT_VARIABLE))
		{
			reply = sem_dbus_set_boot_env_var(conn, msg, user_data);	
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_DISABLE_KEEP_ALIVE_TEMPORARILY))
		{
			reply = sem_dbus_disable_keep_alive(conn, msg, user_data);		
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_EXECUTE_SYSTEM_COMMAND))
		{
			reply = sem_dbus_execute_system_command(conn, msg, user_data);		
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SHOW_ALL_SLOT_SYS_INFO))
		{
			reply = show_all_slot_sys_info(conn, msg, user_data);		
		}
		/*added by zhaocg for md5 img command*/
		else if(dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_MD5_IMG_SLOT))
		{
			reply = sem_dbus_md5_img_slot(conn,msg,user_data);
		}
		else if(dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_MD5_PATCH_SLOT))
		{
			reply = sem_dbus_md5_patch_slot(conn,msg,user_data);
		}
		else if(dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_IMG_OR_FASTFWD_SLOT))
		{
			reply = sem_dbus_img_or_fastfwd_slot(conn,msg,user_data);
		}
		else if(dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_DEL_IMG_OR_FASTFWD_SLOT))
		{
			reply = sem_dbus_delete_img_or_fastfwd_slot(conn,msg,user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_UPLOAD_SNAPSHOT))
		{
			reply = sem_dbus_upload_snapshot(conn, msg, user_data);		
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_USER_ADD_SLOT))
		{
			reply = sem_dbus_user_add_slot(conn, msg, user_data);		
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_USER_DEL_SLOT))
		{
			reply = sem_dbus_user_del_slot(conn, msg, user_data);		
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_USER_ROLE_SLOT))
		{
			reply = sem_dbus_user_role_slot(conn, msg, user_data);		
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_USER_SHOW_SLOT))
		{
			reply = sem_dbus_user_show_slot(conn, msg, user_data);		
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_USER_SHOW_RUNNING))
		{
			reply = sem_dbus_user_show_running(conn, msg, user_data);		
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_USER_PASSWD_SLOT))
		{
			reply = sem_dbus_user_passwd_slot(conn, msg, user_data);		
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_USER_IS_EXSIT_SLOT))
		{
			reply = sem_dbus_user_is_exsit_slot(conn, msg, user_data);		
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_DOWNLOAD_IMG_SLOT))
		{
			reply = sem_dbus_download_img_slot(conn, msg, user_data);		
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_DOWNLOAD_CPY_CONFIG_SLOT))
		{
			reply = sem_dbus_download_cpy_config_slot(conn, msg, user_data);		
		}
    }
	else if (strcmp(dbus_message_get_path(msg), SEM_DBUS_ETHPORTS_OBJPATH) == 0) 
	{
		sem_syslog_dbg("sem obj path : %s\n", SEM_DBUS_ETHPORTS_OBJPATH);
        if (dbus_message_is_method_call(msg, SEM_DBUS_ETHPORTS_INTERFACE, SEM_DBUS_ETHPORTS_INTERFACE_METHOD_SHOW_ETHPORT_LIST)) 
		{
			reply = sem_dbus_ethports_interface_show_ethport_list(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_ETHPORTS_INTERFACE, SEM_DBUS_CONFIG_MEDIA_MODE))
		{
			reply = sem_dbus_config_ethport_media(conn, msg, user_data);
		}
		else if (dbus_message_is_method_call(msg, SEM_DBUS_ETHPORTS_INTERFACE, SEM_DBUS_ETHPORTS_INTERFACE_METHOD_SHOW_RUNNING_CONFIG)) 
		{
			reply = sem_dbus_ethports_show_running_config(conn, msg, user_data);
		}
		else if (dbus_message_is_method_call(msg, SEM_DBUS_ETHPORTS_INTERFACE, SEM_DBUS_ETHPORTS_GET_SLOT_PORT)) 
		{
			reply = sem_dbus_get_slot_port_by_portindex(conn, msg, user_data);
		}		
		else if (dbus_message_is_method_call(msg, SEM_DBUS_ETHPORTS_INTERFACE, SEM_DBUS_ETHPORTS_INTERFACE_METHOD_SHOW_ETHPORT_STAT)) 
		{
			reply = sem_dbus_ethports_interface_show_ethport_stat(conn, msg, user_data);
		}
		else if (dbus_message_is_method_call(msg, SEM_DBUS_ETHPORTS_INTERFACE, SEM_DBUS_ETHPORTS_INTERFACE_METHOD_CLEAR_ETHPORT_STAT)) 
		{
			reply = sem_dbus_ethports_interface_clear_ethport_stat(conn, msg, user_data);
		}
		else if (dbus_msg_is_method_call(msg, SEM_DBUS_ETHPORTS_INTERFACE, SEM_DBUS_SYNC_MASTER_ETH_PORT_INFO))
		{
			reply = sem_dbus_sync_master_eth_port_info(conn, msg, user_data);
		}
	}
	else if (strcmp(dbus_message_get_path(msg), SEM_DBUS_INTF_OBJPATH) == 0)
	{
        sem_syslog_dbg("sem obj path : %s\n", SEM_DBUS_INTF_OBJPATH);
        if (dbus_message_is_method_call(msg, SEM_DBUS_INTF_INTERFACE, SEM_DBUS_SUB_INTERFACE_CREATE)) 
		{
			reply = sem_dbus_create_sub_intf(conn, msg, user_data);
		}
        else if (dbus_message_is_method_call(msg, SEM_DBUS_INTF_INTERFACE, SEM_DBUS_SUB_INTERFACE_DELETE)) 
		{
			reply = sem_dbus_delete_sub_intf(conn, msg, user_data);
		}
	}
	else {	
	    sem_syslog_dbg("unknow obj path %s", dbus_message_get_path(msg));
	}
#if 0
	else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SHOW_PRODUCT_INFO))
	{
		reply = sem_dbus_show_product_info(conn, msg, user_data);
	}
	else if (dbus_msg_is_method_call(msg, SEM_DBUS_INTERFACE, SEM_DBUS_SHOW_SLOT_INFO))
	{
		reply = sem_dbus_show_slot_info(conn, msg, user_data);
	}
#endif
	
    if (reply)
    {
        if (!dbus_connection_send (conn, reply, NULL)){
    		sem_syslog_dbg("Signal send error, Out Of Memory!\n"); 
    		return 1;
	    }
        dbus_connection_flush(conn);
        dbus_message_unref (reply);
    }

    return DBUS_HANDLER_RESULT_HANDLED ;
}

/**
* Description:
*   SEM dbus filter function.
*
*
*/
DBusHandlerResult sem_dbus_filter_function(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
    if (dbus_message_is_signal(msg, DBUS_INTERFACE_LOCAL, "Disconnected") &&
            strcmp(dbus_message_get_path(msg), DBUS_PATH_LOCAL) == 0 )
    {
        printf("%s: Got disconnected from the system message bus.\n",
                        __func__);
        dbus_connection_unref (sem_dbus_conn);
        sem_dbus_conn = NULL;
    }
    else if (dbus_message_is_signal(msg, DBUS_INTERFACE_DBUS, "NameOwnerChanged"))
    {
    }
    else
    {
        return 1;
    }

    return DBUS_HANDLER_RESULT_HANDLED;
}


int sem_dbus_init(void)
{
    DBusError err;
    DBusObjectPathVTable sem_vtbl = {NULL, &sem_dbus_msg_handler, NULL, NULL, NULL, NULL};
    unsigned char dbus_name[SEM_PATH_MAX] = {0};
    unsigned char obj_path[SEM_PATH_MAX] = {0};

    sprintf(dbus_name, "%s", SEM_DBUS_BUSNAME);
    sprintf(obj_path, "%s", SEM_DBUS_OBJPATH);

    printf("%s: dbus name : %s\n", __func__, dbus_name);
    printf("%s: obj name : %s\n", __func__, obj_path);

    dbus_threads_init_default();
    dbus_connection_set_change_sigpipe (1);
    dbus_error_init (&err);
    sem_dbus_conn = dbus_bus_get_private (DBUS_BUS_SYSTEM, &err);

    if (!sem_dbus_conn)
    {
        printf("%s: sem_dbus_conn = NULL.\n", __func__);
        return -1;
    }

    if (!dbus_connection_register_fallback (sem_dbus_conn, obj_path, &sem_vtbl, NULL))
    {
        printf("%s: register fallback failed.\n", __func__);
        return -1;
    }

    dbus_bus_request_name (sem_dbus_conn, dbus_name, 0, &err);

    if (dbus_error_is_set (&err))
    {
        printf("%s: dbus_bus_request_name() ERR:%s\n", __func__,
                        err.message);
        return -1;
    }

    dbus_connection_add_filter (sem_dbus_conn, sem_dbus_filter_function, NULL, NULL);
    dbus_bus_add_match (sem_dbus_conn,
                            "type='signal'"
                            ",interface='"DBUS_INTERFACE_DBUS"'"
                            ",sender='"DBUS_SERVICE_DBUS"'"
                            ",member='NameOwnerChanged'",
                            NULL);

    return 0;
}

/**
* Description:
*   SEM DBus thread function.
*
* Parameter:
*   NULL
*
* Return:
*   Void
*
*/
void sem_dbus_main(void)
{	
	sem_syslog_warning("thread %s, thread id is %d\n", __FUNCTION__, getpid());
    if (sem_dbus_init())
    {
        printf("%s: Initialize failed.\n", __func__);
        exit(-1);
    }

    while (1)
    {
        if (!dbus_connection_read_write_dispatch(sem_dbus_conn, DBUS_TIMEOUT))
        {
            break;
        }
    }

	sem_syslog_dbg("sem dbus shouldn't be printed out\n");
}

