#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <linux/ioctl.h>
#include <linux/tipc.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#include "../sem_common.h"
#include "sem/sem_tipc.h"
#include "board/board_feature.h"
#include "product_feature.h"
#include "sysdef/sem_sysdef.h"
#include "sem/product.h"

#include "product_ax8603.h"
#include "board/netlink.h"

extern board_fix_param_t ax81_2x12g12s;
extern board_fix_param_t ax81_ac12c;
extern board_fix_param_t ax81_ac8c;
extern board_fix_param_t ax81_12x;
extern board_fix_param_t ax81_1x12g12s;
extern board_fix_param_t ax81_ac4x;



extern board_info_syn_t board_info_to_syn;
extern product_info_syn_t product_info_syn;
extern thread_index_sd_t thread_id_arr[MAX_SLOT_NUM];
extern version_sync_t version_sync_arr[MAX_SLOT_NUM];

extern product_fix_param_t *product;
extern board_fix_param_t *local_board;
extern int update_mac_enable;
extern int update_sn_enable;
extern int switch_flag;
extern int active_server_sd;
extern int active_mcb_server_flag;
extern unsigned int reset_ready_mask;
extern pthread_mutex_t switch_lock;
extern global_ethport_t *start_fp[];

extern int sock_s_fd;
extern struct msghdr s_msg;
extern struct nlmsghdr *s_nlh;
extern struct sockaddr_nl s_src_addr, s_dest_addr;
extern struct iovec s_iov;

extern int reset_bootfile(char *filename);
extern int non_active_master_board_server_thread(void *arg);
extern int non_active_master_board_keep_alive_test_client(void *arg);
extern void active_master_board_keep_alive_thread(void);
extern void active_master_board_server_thread(void);
extern int sem_read_cpld(int reg, int *read_value);
extern int sem_write_cpld(int reg, int write_value);
extern int abort_check_product_state_reboot();



char active_mcb_version[MAX_SOFTWARE_VERSION_LEN];
char active_mcb_buildno[MAX_SOFTWARE_VERSION_LEN];
enum
{
	BOARD_TYPE_AX81_SMU,
	BOARD_TYPE_AX81_2X12G12S,
	BOARD_TYPE_AX81_AC12C,
	BOARD_TYPE_AX81_AC8C,
	BOARD_TYPE_AX81_1X12G12S,
	BOARD_TYPE_AX81_12X,
	BOARD_TYPE_AX81_AC4X,
	BOARD_TYPE_AX81_MAX
};

board_fix_param_t *ax8603_support_board_arr[] =
{
	[BOARD_TYPE_AX81_2X12G12S] = &ax81_2x12g12s,
	[BOARD_TYPE_AX81_AC12C] = &ax81_ac12c,
	[BOARD_TYPE_AX81_AC8C] = &ax81_ac8c,
	[BOARD_TYPE_AX81_12X] = &ax81_12x,
	[BOARD_TYPE_AX81_1X12G12S] = &ax81_1x12g12s,
	[BOARD_TYPE_AX81_AC4X] = &ax81_ac4x,
	[BOARD_TYPE_AX81_MAX] = NULL
};

int ax_8603_get_product_sys_info(product_sysinfo_t *sys_info)
{
	
	return 0;
}


static int get_slot_id(bool is_master, int *slot_id)
{
	int ret = 0;
	int reg_data = 0;
	int reg_addr = 0;

	reg_addr = CPLD_SLOT_ID_REG;
	
	ret = sem_read_cpld(reg_addr, &reg_data);

	if (ret)
	{
		return ret;
	}
	*slot_id = reg_data & SLOT_ID_MASK;

	return ret;	
}

int ax8603_get_board_on_mask(unsigned int *mask, int slot_id, int default_slot_id)
{
	int ret;
	int reg_data;
	*mask = 0;
	ret = sem_read_cpld(CPLD_INTERRUPT_SOURCE_STATE_REG1, &reg_data);
	if (ret)
	{
		return ret;
	}

	reg_data = reg_data&0x1;
	if (!reg_data) {
		sem_syslog_dbg("business board inserted\n");
		*mask = (*mask | 1<<2);
	} else {
		sem_syslog_dbg("business board not inserted\n");
		*mask = 0;
	}
	
	ret = sem_read_cpld(CPLD_REMOTE_ON_STATE_REG, &reg_data);
	if (ret)
	{
		return ret;
	}
	//sem_syslog_dbg("reg_data = 0x%x\n", reg_data);
	if (reg_data&0x1)
	{
		if (slot_id == default_slot_id)
		{
			*mask &= (~(1 << DISTRIBUTE_AX8603_SECOND_MASTER_SLOT_ID));
			*mask |= (1 << DISTRIBUTE_AX8603_FIRST_MASTER_SLOT_ID);
			//sem_syslog_dbg("1 : mask = 0x%x\n", *mask);
		}
		else
		{
			*mask &= (~(1 << DISTRIBUTE_AX8603_FIRST_MASTER_SLOT_ID));
			*mask |= (1 << DISTRIBUTE_AX8603_SECOND_MASTER_SLOT_ID);
			//sem_syslog_dbg("2 : mask = 0x%x\n", *mask);
		}
	}
	else
	{
		*mask |= (1 << DISTRIBUTE_AX8603_SECOND_MASTER_SLOT_ID);
		*mask |= (1 << DISTRIBUTE_AX8603_FIRST_MASTER_SLOT_ID);
	}
	sem_syslog_dbg("mask = 0x%x\n", *mask);

	return 0;
}

/*
 *
 *param@insert_state: value:1 on; 0 not
 */
int ax8603_is_remote_master_on(int *insert_state)
{
	int ret = 0;
	int reg_data = 0;
	ret = sem_read_cpld(CPLD_REMOTE_ON_STATE_REG, &reg_data);
	if (ret)
	{
		sem_syslog_dbg("sem read cpld reg failed\n");
		return ret;
	}
	*insert_state = (!reg_data) & REMOTE_MASTER_INSERT_STATE_MASK;

	return ret;
}

int ax8603_force_active_master(void)
{
	int ret;
	ret = sem_write_cpld(CPLD_FORCE_ACTIVE_OR_STBY_REG, 2);
	
	if (ret)
	{
		sem_syslog_dbg("sem write cpld reg %d failed\n", CPLD_FORCE_ACTIVE_OR_STBY_REG);
		return -1;
	}
	
	return 0;
}

/*
 *
 *return value:-1 failed, 0 success
 */

int ax8603_remote_master_state(int *state)
{
	int ret = 0;
	int reg_data = 0;
	ret = sem_read_cpld(CPLD_REMOTE_MASTER_STATE_REG, &reg_data);
	if (ret)
	{
		sem_syslog_dbg("sem read cpld reg failed\n");
		return ret;
	}
	*state = reg_data & REMOTE_MASTER_STATE_MASK;

	return ret;
}

/*
 *send netlink message
 *
 */
int ax8603_board_inserted(int slot_id)
{
	sem_syslog_dbg("slot %d inserted a board\n", slot_id+1);
	char msgbuf[512] = {0};
    int msgLen = 0;
	int i = 0;

	if (product->product_state >= SYSTEM_READY)
	{
		sem_syslog_warning("slot %d hot insert a board.\n", slot_id+1);
		//product->hardware_reset_slot(0x3ff);
		
        nl_msg_head_t *head = (nl_msg_head_t *)msgbuf;
        netlink_msg_t *nl_msg =  (netlink_msg_t *)(msgbuf + sizeof(nl_msg_head_t));

        head->type = OVERALL_UNIT;
    	head->object = COMMON_MODULE;
    	head->pid = getpid();
        head->count = 1;
    	
        nl_msg->msgType = BOARD_STATE_NOTIFIER_EVENT;
    	nl_msg->msgData.boardInfo.action_type = BOARD_INSERTED;
    	nl_msg->msgData.boardInfo.slotid = slot_id + 1;

        msgLen = sizeof(nl_msg_head_t) + head->count*sizeof(netlink_msg_t);
    	
    	for(i = 0; i < product->slotcount; i++)
    	{
           if(i != product->active_master_slot_id)
           {
                sem_tipc_send(i, SEM_NETLINK_MSG, msgbuf, msgLen);
    	   }
    	}
		sem_netlink_send(msgbuf, msgLen);
	}

	return 0;
}

/*
 *send netlink message
 *
 */
int ax8603_board_removed(int slot_id)
{
	sem_syslog_dbg("slot %d board is removed\n", slot_id+1);
	char msgbuf[512] = {0};
    int msgLen = 0;
	int i = 0;
	
	if (slot_id < 0 || slot_id >product->slotcount)
	{
		sem_syslog_dbg("Error:slot id error. %d\n", slot_id+1);
		return 1;
	}

	thread_id_arr[slot_id].slot_id = USELESS_SLOT_ID;
	version_sync_arr[slot_id].slot_id = USELESS_SLOT_ID;

	product->product_slot_board_info[slot_id].board_state = BOARD_INSERTED_AND_REMOVED;
	product->get_board_on_mask(&product->board_on_mask, local_board->slot_id, product->default_master_slot_id);
	product_info_syn.board_on_mask = product->board_on_mask;
	product_info_syn.product_slot_board_info[slot_id].board_state = product->product_slot_board_info[slot_id].board_state;
	product->update_product_state();

	for(i = 0; i < BOARD_GLOBAL_ETHPORTS_MAXNUM; i++)
    {
        start_fp[slot_id][i].isValid = 0;
	}
	product->syn_product_info(NULL);


    nl_msg_head_t *head = (nl_msg_head_t *)msgbuf;
    netlink_msg_t *nl_msg =  (netlink_msg_t *)(msgbuf + sizeof(nl_msg_head_t));

    head->type = OVERALL_UNIT;
	head->object = COMMON_MODULE;
	head->pid = getpid();
    head->count = 1;
	
    nl_msg->msgType = BOARD_STATE_NOTIFIER_EVENT;
	nl_msg->msgData.boardInfo.action_type = BOARD_REMOVED;
	nl_msg->msgData.boardInfo.slotid = slot_id + 1;

    msgLen = sizeof(nl_msg_head_t) + head->count*sizeof(netlink_msg_t);
    sem_netlink_send(msgbuf, msgLen);

	for(i = 0; i < product->slotcount; i++)
	{
       if(i != product->active_master_slot_id)
       {
            sem_tipc_send(i, SEM_NETLINK_MSG, msgbuf, msgLen);
	   }
	}
	
	return 0;
}

int ax8603_tipc_init(board_fix_param_t *board)
{
	int zone = 3;
	int cluster = 1;
	int node = board->slot_id + 1;
	
	unsigned char sbuff[64] = {0};
    memset(sbuff, 0, 64);
	sprintf(sbuff, "ifconfig %s up", "obc0");
	system(sbuff);
	
    memset(sbuff, 0, 64);
	sprintf(sbuff, "tipc-config -netid=%d", SEM_NET_ID);
	system(sbuff);
	
    memset(sbuff, 0, 64);
	sprintf(sbuff, "tipc-config -addr=%d.%d.%d", zone, cluster, node);
	system(sbuff);

	
	memset(sbuff, 0, 64);
	sprintf(sbuff, "ifconfig %s up", "obc0");
	system(sbuff);

	memset(sbuff, 0, 64);
	sprintf(sbuff, "ifconfig %s up", "obc1");
	system(sbuff);
	
	memset(sbuff, 0, 64);
	sprintf(sbuff, "tipc-config -be=eth:%s", "obc0");
	system(sbuff);

	memset(sbuff, 0, 64);
	sprintf(sbuff, "tipc-config -be=eth:%s", "obc1");
	system(sbuff);
	
	if (board->slot_id == product->master_slot_id[DISTRIBUTE_AX8603_FIRST_MASTER_SLOT_INDEX]) {
		memset(sbuff, 0, 64);
		sprintf(sbuff, "ip addr add %d.%d.%d.%d/24 dev obc0", 169, 254, 1, node);
		system(sbuff);
	} else if (board->slot_id == product->master_slot_id[DISTRIBUTE_AX8603_SECOND_MASTER_SLOT_INDEX]) {
		memset(sbuff, 0, 64);
		sprintf(sbuff, "ip addr add %d.%d.%d.%d/24 dev obc1", 169, 254, 1, node);
		system(sbuff);
	} else if (!board->is_master) {
		memset(sbuff, 0, 64);
		sprintf(sbuff, "ip addr add %d.%d.%d.%d/24 dev obc1", 169, 254, 1, node);
		system(sbuff);
	} else {
		sem_syslog_dbg("no correct obc port to set ip address,please check\n");
	}
	#if 0
	if (board->board_type == BOARD_TYPE_AX81_AC8C || board->board_type == BOARD_TYPE_AX81_AC12C || 
		board->board_type == BOARD_TYPE_AX81_1X12G12S)
	{
		sem_syslog_dbg("config slot %d ve port\n", board->slot_id+1);
		
		memset(sbuff, 0, 64);
		sprintf(sbuff, "ip addr add %d.%d.%d.%d/24 dev ve%d", 169, 254, 2, board->slot_id+1, board->slot_id+1);
		system(sbuff);
	}
	
	if (board->board_type == BOARD_TYPE_AX81_AC8C ||
		board->board_type == BOARD_TYPE_AX81_AC12C ||
		board->board_type == BOARD_TYPE_AX81_1X12G12S)
	{
		memset(sbuff, 0, 64);
		sprintf(sbuff, "ifconfig ve%d up", board->slot_id + 1);
		sem_syslog_dbg("%s\n", sbuff);
		system(sbuff);
		system("echo 1 > /sys/module/e1000e/parameters/e1000e_rx_en");/*enable e1000e rx packet.*/
	}
	#else
	if (board->board_type == BOARD_TYPE_AX81_AC8C ||
		board->board_type == BOARD_TYPE_AX81_AC12C ||
		board->board_type == BOARD_TYPE_AX81_1X12G12S)
	{
		sem_syslog_dbg("config slot %d ve port\n", board->slot_id+1);
		
		memset(sbuff, 0, 64);
		sprintf(sbuff, "ip addr add %d.%d.%d.%d/24 dev ve%02df%d", 169, 254, 2, board->slot_id+1, board->slot_id+1, 1);
		system(sbuff);
	}
	
	if (board->board_type == BOARD_TYPE_AX81_AC8C ||
		board->board_type == BOARD_TYPE_AX81_1X12G12S)
	{
		memset(sbuff, 0, 64);
		sprintf(sbuff, "ifconfig ve%02df%d up", board->slot_id + 1, 1);
		sem_syslog_dbg("%s\n", sbuff);
		system(sbuff);
		system("echo 1 > /sys/module/e1000e/parameters/e1000e_rx_en");/*enable e1000e rx packet.*/
	}

	if (board->board_type == BOARD_TYPE_AX81_AC12C)
	{
		memset(sbuff, 0, 64);
		sprintf(sbuff, "ifconfig ve%02df%d up", board->slot_id + 1, 1);
		sem_syslog_dbg("%s\n", sbuff);
		system(sbuff);
		sprintf(sbuff, "ifconfig ve%02ds%d up", board->slot_id + 1, 1);
		sem_syslog_dbg("%s\n", sbuff);
		system(sbuff);
		system("echo 1 > /sys/module/e1000e/parameters/e1000e_rx_en");/*enable e1000e rx packet.*/
	}
	#endif
	return 0;
}

static int get_software_version(char *version, char *buildno)
{

	int len, i, fd;

	fd = open(SOFTWARE_VERSION_FILE, O_RDONLY);
	if (fd < 0) {
		sem_syslog_dbg("		open file %s failed\n", SOFTWARE_VERSION_FILE);
		return 1;
	}	

	len = read(fd, version, MAX_SOFTWARE_VERSION_LEN);
	close(fd);

	version[len-1] = '\0';
	sem_syslog_dbg("		local board version is %s ", version);


	fd = open(SOFTWARE_BUILDNO_FILE, O_RDONLY);
	if (fd < 0) {
		sem_syslog_dbg("		open file %s failed\n", SOFTWARE_VERSION_FILE);
		return 1;
	}	

	len = read(fd, buildno, MAX_SOFTWARE_BUILDNO_LEN);
	close(fd);
	
	buildno[len-1] = '\0';
	sem_syslog_dbg(" buildno is %s\n", buildno);
	return 0;	
}

static int recv_software_version(int fd, char *recv_buf, int *version_byte_count)
{
	if(recv_buf == NULL || fd < 0)
	{
		sem_syslog_dbg("recv_buf == null or fd < 0\n");
		return -1;
	}

	int addr_len = sizeof(struct sockaddr_tipc);
	int counter = 0;
    char *temp_buf;
	char r_md5[128];
	int fd_md5;
	int buf_len = sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t) + sizeof(software_version_text_t);
	
 	software_version_text_t* body_head;
	sem_pdu_head_t*          pdu_head;
	sem_tlv_t*               tlv_head; 

	*version_byte_count = 0;

	temp_buf = (char *)malloc(sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t) + sizeof(software_version_text_t));
	if (!temp_buf)
	{
		sem_syslog_dbg("			malloc temp_buf failed\n");
		return 1;
	}
	
	memset(temp_buf, 0, sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t) + sizeof(software_version_text_t));

	while(1)
	{
		if (recvfrom(g_recv_sd, temp_buf, sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t) + sizeof(software_version_text_t), 0, (struct sockaddr *)&g_recv_sock_addr, &addr_len) <= 0)
		{
			sem_syslog_warning("recv_software_version:connect terminated\n");
			free(temp_buf);
			return 1;
		}

		pdu_head = (sem_pdu_head_t *)temp_buf;	
		tlv_head = (sem_tlv_t *)(temp_buf+sizeof(sem_pdu_head_t));		  	

		if(tlv_head->type == SEM_SOFTWARE_VERSION_SYNING)
		{	
			body_head = (software_version_text_t *)(tlv_head->body);
			memcpy(recv_buf, body_head->buf, body_head->len);
			recv_buf += body_head->len;
			*version_byte_count += body_head->len;
		}
		else if(tlv_head->type == SEM_SEND_MD5_STR)
		{			
			strcpy(r_md5, temp_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
			fd_md5= open("/mnt/md5", O_RDWR|O_CREAT);
			if(fd_md5 < 0)
			{
				sem_syslog_dbg("file %s open failed.%s\n", "/mnt/md5", __FUNCTION__);
				free(temp_buf);
				return 1;
			}
			write(fd_md5, r_md5, strlen(r_md5)+1);
			close(fd_md5);
		}
		else if(tlv_head->type == SEM_SOFTWARE_VERSION_SYN_FINISH)
		{
			free(temp_buf);
			sem_syslog_dbg("			receive version file done.recv bytes %d\n", *version_byte_count);
			break;
		}
		else
		{
			sem_syslog_dbg("			wrong message type:%d from slot %d, counter=%d \n", tlv_head->type, pdu_head->slot_id+1, ++counter);
		}
	}
	
	return 0;
}

static int get_new_software_version_name(software_version_response_t *buf, char *new_version_name)
{
	if(buf == NULL || new_version_name == NULL)
	{
		return -1;
	}

	sprintf(new_version_name, "AW%s.%s.X7X5.IMG", buf->version, buf->buildno);
	
	return 0;
}

/*
 *
 *return value:1 for failed , 0 for success
 *
 */
static int save_software_version(char *buf, char *name, int version_byte_count)
{
	if(buf == NULL || name == NULL)
	{
		return 1;
	}
	char file_path[PATH_AND_NAME_LEN];
	sprintf(file_path, "/mnt/%s", name);
	FILE *fp = NULL;
	fp = fopen(file_path, "wb+");
	char chBuf[50] = {0};

	if(fp == NULL)
	{
		sem_syslog_dbg("open file %s failed\n", name);
		return -1;
	}

	//sem_syslog_dbg("buf len %d\n", strlen(buf));
	fwrite(buf, sizeof(char), version_byte_count, fp);
	fclose(fp);
	if (check_free_space(name))
	{
		return -1;
	}

	memset(chBuf, 0, 50);
	sprintf(chBuf, "sor.sh cp %s 50", name);
	system(chBuf);

	return 0;
}

/************************************************************
function:  int file_send(int sd, char *sendbuf)
describ:   setting option of tipc address
in:        int sd,          sd for tipc send or recv
           char *sendbuf    file data buf, now fill in a file name (link "/blk/filename")
out:       
return 0 : success
date:      2011-05-26
modify:    2011-05-26 <zhangdx@autelan.com>
*************************************************************/
int ax8603_sync_send_file_s(int slot_id, char *filename, int syn_to_blk)
{
	int i = 0;
	int itemp_len = 0;
	int send_len = 0;
	int file_len = 0;
	
	FILE *fp = NULL;
	
	char send_buf[SEM_TIPC_SEND_BUF_LEN];
	char read_buf[SEM_TIPC_SEND_BUF_LEN];
	
	sem_pdu_head_t *head;
	sem_tlv_t *tlv_head;
	sync_file_head_t *pfilehead;

	struct sockaddr_tipc sock_addr;
	int sd = -1;
	
	if(local_board->is_active_master)
	{
		for (i=0; i<product->slotcount; i++)
		{
			if (thread_id_arr[i].sd >= 0 && 
				thread_id_arr[i].slot_id != USELESS_SLOT_ID && 
				thread_id_arr[i].slot_id != local_board->slot_id)
			{
				//sem_syslog_dbg("\tthread_id_arr[%d].slot_id = %d, sd = %d\n", 
				//	i, thread_id_arr[i].slot_id, thread_id_arr[i].sd);
				if(thread_id_arr[i].slot_id == slot_id)
				{					
					sd = g_send_sd_arr[i].sd;
					sock_addr = g_send_sd_arr[i].sock_addr;
				}
			}
		}
	}else {
		sd = g_send_sd;
		sock_addr = g_send_sock_addr;
	}
	
	if (sd == -1)
	{
		sem_syslog_dbg("\tconnect is not created\n");
		return -1;
	}
	
	head = (sem_pdu_head_t*)(send_buf + itemp_len);         /*now itemp_len = 0*/
	head->slot_id = local_board->slot_id;
	
	itemp_len += sizeof(sem_pdu_head_t);                    /*now itemp_len = sizeof(sem_pdu_head_t)*/
	tlv_head =  (sem_tlv_t*)(send_buf + itemp_len); 
 
	itemp_len += sizeof(sem_tlv_t);                         /*now itemp_len = sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t)*/
	pfilehead =  (sync_file_head_t*)(send_buf + itemp_len); 
 
    itemp_len += sizeof(sync_file_head_t);                  /*now itemp_len =  sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t) + sizeof(sync_file_head_t)*/
 
	/*get file name*/
	memset(pfilehead->chFileName, 0, 64);
    strcpy(pfilehead->chFileName, filename);
	pfilehead->syn_to_blk = syn_to_blk;
	system("sudo mount /blk");

	fp = fopen(pfilehead->chFileName, "rb");
    if(!fp)
    {
    	sem_syslog_dbg("open file %s to syn failed\n", filename);
	    return -1;
    }

	fseek(fp , 0, SEEK_END);
    file_len=ftell(fp );

    fseek(fp , 0, SEEK_SET);
	
	//sleep(2);
	for(i = 0; i < MAX_PKT_TYPE_NUM; i++)
    {	
		if(pkt_mark[i].is_type_used == 0)
		{	
			strcpy(pkt_mark[i].chFileName, filename);
			pkt_mark[i].is_type_used = 1;
			
		    while(1)
		    {   
				memset(read_buf, 0, SEM_TIPC_SEND_BUF_LEN);
				
			    pfilehead->msg_len = fread(read_buf, sizeof(char), SEM_TIPC_SEND_BUF_LEN-(sizeof(sem_pdu_head_t)+sizeof(sem_tlv_t)+sizeof(sync_file_head_t)), fp); /*read file data*/
                
	            file_len -= pfilehead->msg_len;
	            if(file_len > 0)/*(!feof(fp))*/
	            {
	        	    tlv_head->type = SEM_FILE_SYNING;             /*no end of the file*/
	            }
	            else
	            {
	        	    tlv_head->type = SEM_FILE_SYN_FINISH;         /*end of the file*/
	            }
				
			    send_len = itemp_len + pfilehead->msg_len;         /*all data len , include head len and data len*/
			    pfilehead->packet_type = pkt_mark[i].packet_type;  /*mark file type*/
			    
			    memcpy(send_buf + itemp_len, read_buf, pfilehead->msg_len); /*copy file data to send buf*/

				if (sendto(sd, send_buf, send_len, 0, (struct sockaddr*)&sock_addr, sizeof(struct sockaddr_tipc)) < 0)
			    {
					fclose(fp);
					sem_syslog_dbg("Send file(%s) len(%d) Failed.\n", filename, file_len);
					return -1;
			    }
				
			    if(tlv_head->type == SEM_FILE_SYN_FINISH)
			    {			
					memset(pkt_mark[i].chFileName, 0, 128);  
					pkt_mark[i].is_type_used = 0;
					sem_syslog_dbg("\tSend file(%s) len(%d) successed.\n", filename, file_len);
		            fclose(fp);
				    return 0;
			    }
		    }
		}
	} 

	sem_syslog_dbg("not find the free index from pkt_mark\n");
	fclose(fp);
	return 1;
}

/************************************************************
function:  file_recv(int sd, char *recvbuf)
describ:   setting option of tipc address
in:        int sd,          sd for tipc send or recv
out:       char *recvbuf   if file recv successed£¬it will return filename (link "/blk/filename")
return 0 : success
date:      2011-05-26
modify:    2011-05-26 <zhangdx@autelan.com>
*************************************************************/
int ax8603_sync_recv_file_s(char *recvbuf, int iflag, int slot_id)
{
	int i = 0;
	int iCount = 0;
	int syn_to_blk = 0;
	char cmd_buf[100] ;
	sync_file_head_t *pfilehead = (sync_file_head_t *)recvbuf;  //get msg head info 

    for(i = 0; i < MAX_PKT_TYPE_NUM/*save_file_head[0].use_numb*/; i++)
    {
		if(save_file_head[i].packet_type == pfilehead->packet_type && save_file_head[i].fp[slot_id] != NULL)
		{			
		    if(iflag == 0)       //recv data buf
		    {
			    char *temp = recvbuf + sizeof(sync_file_head_t);
                int testlen = fwrite(temp, sizeof(char), pfilehead->msg_len, save_file_head[i].fp[slot_id]);
				
				return 0;
		    }
		    else if(iflag == 1)  //save buf to a file
		    {
                char *temp = recvbuf + sizeof(sync_file_head_t);
				syn_to_blk = pfilehead->syn_to_blk;
                int testlen = fwrite(temp, sizeof(char), pfilehead->msg_len, save_file_head[i].fp[slot_id]);	
			    //printf("write len = %d\n", testlen);
				
                fclose(save_file_head[i].fp[slot_id]);
			    save_file_head[i].fp[slot_id] = NULL;
                sem_syslog_dbg("\trecv file (%s) success\n", pfilehead->chFileName);
				//printf("%s,%s,%d\n",__FILE__, __FUNCTION__, __LINE__);	
				if (syn_to_blk) {
					if (!strncmp("/mnt", pfilehead->chFileName, 4)) {
						if (strlen(pfilehead->chFileName+5) >= 87) {
							sem_syslog_warning("sor failed:file name too long to call sor.sh\n");
							return 1;
						}
						sprintf(cmd_buf, "sor.sh cp %s 50", pfilehead->chFileName+5);
						sem_syslog_dbg(cmd_buf);
						system(cmd_buf);
					} else {
						sem_syslog_warning("sor failed:only can sor file which is at /mnt file is:%s\n", pfilehead->chFileName);
						return 1;
					}
				}
			    return 0;				
		    }
			else
			{
				sem_syslog_dbg("wrong flag :%d\n", iflag);
				return 1;
			}
		}
		else if(save_file_head[i].packet_type == pfilehead->packet_type && save_file_head[i].fp[slot_id] == NULL)
		{
			save_file_head[i].fp[slot_id] = fopen(pfilehead->chFileName,"wb+");

			if (!save_file_head[i].fp[slot_id])
			{
				sem_syslog_dbg("save file:open file %s failed\n", pfilehead->chFileName);
				return 1;
			}
			
		    if(iflag == 0)       //recv data buf
		    {
			    char *temp = recvbuf + sizeof(sync_file_head_t);
                int testlen = fwrite(temp, sizeof(char), pfilehead->msg_len, save_file_head[i].fp[slot_id]);	
				//sem_syslog_dbg("file %s save success\n", pfile);
				return 0;
		    }
		    else if(iflag == 1)  //save buf to a file
		    {
		    	syn_to_blk = pfilehead->syn_to_blk;
                char *temp = recvbuf + sizeof(sync_file_head_t);
			
                int testlen = fwrite(temp, sizeof(char), pfilehead->msg_len, save_file_head[i].fp[slot_id]);	
			    sem_syslog_dbg("file %s save success\n", pfilehead->chFileName);
				
                fclose(save_file_head[i].fp[slot_id]);
			    save_file_head[i].fp[slot_id] = NULL;
				if (syn_to_blk) {
					if (!strncmp("/mnt", pfilehead->chFileName, 4)) {
						if (strlen(pfilehead->chFileName+5) >= 87) {
							sem_syslog_warning("sor failed:file name too long to call sor.sh\n");
							return 1;
						}
						sprintf(cmd_buf, "sor.sh cp %s 50", pfilehead->chFileName+5);
						sem_syslog_dbg(cmd_buf);
						system(cmd_buf);
					} else {
						sem_syslog_warning("sor failed:only can sor file which is at /mnt file is:%s\n", pfilehead->chFileName);
						return 1;
					}
				}
			    return 0;				
		    }
			else
			{
				sem_syslog_dbg("wrong flag :%d\n", iflag);
				return 1;
			}
		}
		
    }


	sem_syslog_dbg("save file %s mismatch\n", pfilehead->chFileName);
	return 1;
}

/*
 *function:It is used for boards except active master board to synchronise current board software version with active master board
 *sem may block here
 *
 *
 */
int	ax8603_synchronise_software_version(int sd, board_fix_param_t *board)
{
	sem_pdu_head_t *head;
	sem_tlv_t *tlv_head;
	software_version_request_t *version_buildno;
	//software_version_request_t *version_buildno2;
	software_version_response_t *version_syn_response;
	int buf_len;
	char new_version_name[SOFTWARE_VERSION_NAME_LEN];
	char *recv_version_buf;
	
	char version[MAX_SOFTWARE_VERSION_LEN];
	char buildno[MAX_SOFTWARE_VERSION_LEN];

	char send_buf[SEM_TIPC_SEND_BUF_LEN];

	int version_byte_count = 0;

	int addr_len = sizeof(struct sockaddr_tipc);
	
	sem_syslog_dbg("synchronise_software_version local_board->slot_id = %d\n", board->slot_id+1);
	
	if (get_software_version(version, buildno))
	{
		sem_syslog_dbg("	get software version failed\n");	
		return -1;
	}
	
	if (g_recv_sd == SEM_FREE_SD)
	{
		sem_syslog_dbg("	connect is not created\n");
		return -1;
	}

	head = (sem_pdu_head_t *)send_buf;
	head->slot_id = board->slot_id;
	head->version = 11;
	head->length = 22;

	tlv_head = (sem_tlv_t *)(send_buf + sizeof(sem_pdu_head_t));
	tlv_head->type = SEM_SOFTWARE_VERSION_SYN_REQUEST;
	tlv_head->length = 33;
	
	version_buildno = (software_version_request_t *)(send_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
	strcpy(version_buildno->version, version);
	strcpy(version_buildno->buildno, buildno);

	recv_version_buf = (char *)(malloc(MAX_SOFTWARE_VERSION_SIZE));
	
	if (!recv_version_buf)
	{
		sem_syslog_dbg("malloc buffer for recving software version failed\n");
		return -1;
	}
	memset(recv_version_buf, 0, MAX_SOFTWARE_VERSION_SIZE);
					
	if (sendto(g_send_sd, send_buf, sizeof(sem_pdu_head_t)+sizeof(sem_tlv_t)+ sizeof(software_version_request_t), 0, (struct sockaddr*)&g_send_sock_addr, sizeof(struct sockaddr_tipc)) < 0)
	{
		sem_syslog_dbg("	%s send failed\n", __FUNCTION__);
		free(recv_version_buf);
		return -1;
	}
	else
	{
		sem_syslog_dbg("send software version succeed.wait for the server response\n");
	}
	
	while (1)
	{
		if (sd < 0)
		{
			sem_syslog_dbg("connect is break. cann't syn software version\n");
			free(recv_version_buf);
			return -1;
		}
		buf_len = SEM_TIPC_SEND_BUF_LEN;

		if (recvfrom(g_recv_sd, send_buf, sizeof(send_buf), 0, (struct sockaddr *)&g_recv_sock_addr, &addr_len) <= 0)
		{
			sem_syslog_dbg("	recv the server response failed\n");
		}
		else
		{
			tlv_head = (sem_tlv_t *)(send_buf + sizeof(sem_pdu_head_t));
			if (tlv_head->type == SEM_SOFTWARE_VERSION_SYN_RESPONSE)
			{
				sem_syslog_dbg("	recvd the server response\n");
				version_syn_response = (software_version_response_t *)(send_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
				/*need syn software version*/
				if (version_syn_response->is_need)
				{	
					sem_syslog_warning("	recving version file from actvie MCB,please wait .........\n");
					/*recv software version from server*/
					if (recv_software_version(sd, recv_version_buf, &version_byte_count))
					{
						sem_syslog_dbg("	recv software version failed\n");
						free(recv_version_buf);
						return -1;
					}
					else
					{
						sem_syslog_warning("	recv software version done,\nsaving software version,please wait.....\n");
					}
					
					sem_syslog_dbg("	get new version file name\n");
					if (get_new_software_version_name(version_syn_response, new_version_name))
					{
						sem_syslog_dbg("	get new software version name failed\n");
						free(recv_version_buf);
						return -1;
					}
					sem_syslog_dbg("	new version file name is %s\n", new_version_name);

					sem_syslog_dbg("	to save version file to storeage\n");
					/*save the received software version to file*/
					if (save_software_version(recv_version_buf, new_version_name, version_byte_count))
					{
						sem_syslog_dbg("save new software version failed\n");
						free(recv_version_buf);
						send_sync_failed_msg(board);
						return -1;
					}
					else
					{
						sem_syslog_dbg("	save new software version done\n");
					}
					if (check_software_version_md5(new_version_name))
					{
						sem_syslog_warning("wrong md5,need sync again\n");
						free(recv_version_buf);
						send_sync_failed_msg(board);
						return -1;
					}
					else
					{
						sem_syslog_warning("right md5,start set bootfile.\n");
					}

					sem_syslog_dbg("	reset bootfile\n");
					if (reset_bootfile(new_version_name))
					{
						sem_syslog_dbg("	set bootfile failed\n");
						free(recv_version_buf);
						send_sync_failed_msg(board);
						return -1;
					}
					else
					{
						sem_syslog_warning("	set bootfile done\n");
					}

					sem_syslog_warning("	version file save done,and to reboot itself\n");
					
					head = (sem_pdu_head_t *)send_buf;
					head->slot_id = board->slot_id;
					head->version = 11;
					head->length = 22;

					tlv_head = (sem_tlv_t *)(send_buf + sizeof(sem_pdu_head_t));
					tlv_head->type = SEM_SOFTWARE_VERSION_SYNC_SUCCESS;
					tlv_head->length = 33;

					if (sendto(g_send_sd, send_buf, sizeof(sem_pdu_head_t)+sizeof(sem_tlv_t)+ sizeof(software_version_request_t), 0, (struct sockaddr*)&g_send_sock_addr, sizeof(struct sockaddr_tipc)) < 0)
					{
						sem_syslog_warning("send to active MCB SEM_SOFTWARE_VERSION_SYNC_SUCCESS failed\n");
					}
					else
					{
						sem_syslog_dbg("send to active MCB SEM_SOFTWARE_VERSION_SYNC_SUCCESS success\n");
					}
					
					board->reboot_self();

					sem_syslog_dbg("	enter into dead cycle,waiting for system reboot itself\n");
					while (1)
					{
						usleep(1000);
					}
				}
				/*no need syn software version*/
				else
				{
					free(recv_version_buf);
					break;
				}
			}
		}
	}
	
	sem_syslog_dbg("	no need to synchronise software version.continue\n");
	
	return 0;
}

static void product_ready_test()
{
	int timeout = ACTIVE_MCB_WATI_TIMEOUT;	
	int i;
	int flag;

	int ready_flag[MAX_SLOT_COUNT];
	unsigned int mask = 0;
	
	for (i=0; i<MAX_SLOT_COUNT; i++)
		ready_flag[i] = 0;
	
	sem_syslog_dbg("product_ready_test product->board_on_mask = 0x%x\n", product->board_on_mask);
	while (1)
	{
		sleep(1);
		timeout -= 1;
		flag = 0;

		product->get_board_on_mask(&product->board_on_mask, local_board->slot_id, product->default_master_slot_id);
		for (i=0; i<product->slotcount; i++)
		{
			if (product->board_on_mask>>i & 0x1)
			{
				if (product_info_syn.product_slot_board_info[i].board_state >= BOARD_READY)
				{
					sem_syslog_dbg("slot %d is ok\n", i+1);
					ready_flag[i] = 1;
					continue;
				}
				else if (version_sync_arr[i].slot_id != USELESS_SLOT_ID)
				{
					sem_syslog_dbg("slot %d is sync version\n", i+1);
					ready_flag[i] = 2;
					continue;
				}
				else
				{
					flag = 1;
				}
			}
		}

		if (flag == 0)
		{
			sem_syslog_warning("all the boards are ready\n");
			break;
		}
		
		if (timeout<0)
		{
			for (i=0; i<product->slotcount; i++)
			{
				if ((ready_flag[i] == 0) && (product->board_on_mask>>i & 0x1))
				{
					mask = mask | (1<<i);
					sem_syslog_warning("slot %d is not registe\n", i+1);
				}
				else if ((ready_flag[i] == 2) && (product->board_on_mask>>i & 0x1))
				{
					sem_syslog_warning("slot %d is syncing verison file\n", i+1);
				}
				else if (product->board_on_mask>>i & 0x1)
				{
					mask = mask | (1<<i);
					sem_syslog_warning("slot %d is done\n", i+1);
				}
				else 
				{
					sem_syslog_warning("slot %d is empty\n", i+1);
				}
			}
				
			if (abort_check_product_state_reboot())
			{
				sem_syslog_warning("Warning:wait for boards ready timeout.System reboot mask=0x%x\n", mask);
				sleep(10);
				ax8603_hardware_reset_slot(mask, 0, RESET_CMD_FROM_SEM);
				break;
			}
			else
			{
				sem_syslog_warning("Warning:wait for boards ready timeout.But abort reboot\n");
				break;
			}
		}

		for (i=0; i<MAX_SLOT_COUNT; i++)
			ready_flag[i] = 0;
		
	}
}

static void product_sync_system_time(void)
{
	time_t now;
	struct tm *timenow;
	char *timestr;
	char cmdstr[256] = {0};
	int i;
	int ret;
	
	time(&now);
	timenow = localtime(&now);

	timestr = asctime(timenow);
	sprintf(cmdstr, "date -s '%s' > null", timestr); 

	for(i = 0; i < product->slotcount; i++) {
		if ((i != local_board->slot_id) && \
			(g_send_sd_arr[i].sd > 0) && \
			(product->board_on_mask & (1 << i))) {
			ret = execute_system_command_msg_send(i, cmdstr);
			if (ret) {
				sem_syslog_dbg("Sync system time to slot %d failed.\n", i + 1);
			}
			else {
				sem_syslog_dbg("Sync system time to slot %d successful.\n", i + 1);
			}
		}
	}

	return ;
}

extern pthread_t stby_keep_alive_thread;
extern pthread_t slave_server_thread;

extern pthread_t master_server_thread;
extern pthread_t master_keep_alive_test_thread;

int ax8603_master_active_or_stby_select(int *sd, board_fix_param_t *board)
{
	int ret;
	int insert_state, remote_state = 0;
	int counter = 0;
	char file_path[64];

	if (!product || !board)
	{
		sem_syslog_dbg("	product or board init is error\n");
		return 1;
	}

	/*default slot*/
	if (board->slot_id == product->default_master_slot_id)
	{
		sem_syslog_dbg("	default master slot board init\n");
		ret = product->is_remote_master_on(&insert_state);
		/*remote inserted*/
		if (insert_state)
		{
			sem_syslog_dbg("	remote master is on,and get remote master state\n");
			if (product->remote_master_state(&remote_state))
			{
				sem_syslog_warning("get remote master state failed.assume the remote master is active\n");
				remote_state = 0;
			}

			//remote master is non-active state
			if (remote_state)
			{
				ret = product->force_active_master();
				if (ret)
				{
					sem_syslog_dbg("	force active master failed.System need to reboot\n");
					return 1;
				}

				sem_syslog_dbg("remote master is not act as active MCB.now start as active MCB\n");
				
				if (sem_thread_create(&master_server_thread, (void *)active_master_board_server_thread, NULL, IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
				{
					sem_syslog_dbg("	thread active_master_board_server_thread create faile.System need to reboot\n");
					return 1;
				}
				
				board->is_active_master = ACTIVE_MASTER;
				board->is_use_default_master = 1;
				
				
				product->more_than_one_master_board_on = 0;
				product->active_master_slot_id = board->slot_id;
				product->update_product_state();
				
				if (get_software_version(active_mcb_version, active_mcb_buildno))
				{
					sem_syslog_dbg("	active master board get software version failed.System need to reboot\n");
					return 1;
				}
				else
				{
					sem_syslog_dbg("	active_mcb_version = %s, active_mcb_buildno = %s\n", active_mcb_version, active_mcb_buildno);
				}

				
				if (sem_thread_create(&master_keep_alive_test_thread, (void *)active_master_board_keep_alive_thread, NULL, IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
				{
					sem_syslog_dbg("	thread active_master_board_keep_alive_thread create failed.System need to reboot\n");
					return 1;
				}
				
				// TODO: active master board reinit product info and local board info

				sem_init_completed();
				board->board_state = BOARD_READY;
				board_info_to_syn.board_state = board->board_state;
				board_info_to_syn.is_active_master = board->is_active_master;
				board->update_local_board_state(board);
				product->product_state = SYSTEM_DSICOVER;
				product->syn_product_info(&board_info_to_syn);
				sem_syslog_dbg("	wait for boards ready.....");
				product->update_product_state();
				product_ready_test();
				sem_syslog_dbg("	done\n");
				
				product->product_state = SYSTEM_READY;
				product->syn_product_info(&board_info_to_syn);
				product->update_product_state();
				
				sem_syslog_dbg("SEM_MAIN:enable dbm effective flag!\n");
                set_dbm_effective_flag(VALID_DBM_FLAG);

				
				product_sync_system_time();
				product_state_notifier(SYSTEM_READY);

			}
			//remote master is act as active MCB
			else
			{
				sem_syslog_dbg("remote master is allready act as active MCB.now start as non-active MCB\n");
				sleep(STANDBY_DELAY_SECONDS);
				//for recving
				g_recv_sd = socket(AF_TIPC, SOCK_RDM, 0);

				if (g_recv_sd < 0)
				{
					sem_syslog_warning("	create recv socket failed:g_recv_sd = %d\n", g_recv_sd);
					return -1;
				}
				
				if (tipc_set_recv_addr(&g_recv_sock_addr, SEM_TIPC_INSTANCE_BASE_ACTIVE_TO_NON_ACTIVE+local_board->slot_id, SEM_TIPC_INSTANCE_BASE_ACTIVE_TO_NON_ACTIVE+local_board->slot_id))
				{
					sem_syslog_dbg("	set server tipc addr failed\n");
					close(g_recv_sd);
					return -1;
				}

				if (0 != bind (g_recv_sd, (struct sockaddr*)&g_recv_sock_addr, sizeof(struct sockaddr)))
			  	{
					sem_syslog_dbg("	bind failed, return\n");
					close(g_recv_sd);
					return -1;
			  	}

				//for STBY sending
				g_send_sd = socket(AF_TIPC, SOCK_RDM, 0);
				if (g_send_sd < 0)
				{
					sem_syslog_warning("	STBY create socket failed:*sd = %d\n", g_send_sd);
					return -1;
				}

				if (tipc_set_send_addr(&g_send_sock_addr, SEM_TIPC_INSTANCE_BASE_NON_ACTIVE_TO_ACTIVE+local_board->slot_id))
				{
					sem_syslog_dbg("default_master_slot_id	set client addr failed\n");
					return -1;
				}

				/*start keep alive client ot test is the link is on*/
				ret = active_link_test(SME_CONN_LINK_TEST_COUNT, sd);

				/*connect active MCB done*/
				if (ret == 0)
				{
					// TODO:default as backup master board
					sem_syslog_dbg("	link on\n");
				
					board->is_active_master = NOT_ACTIVE_MASTER;
					board->is_use_default_master = 0;
					
					product->more_than_one_master_board_on = 1;
					product->active_master_slot_id = board->slot_id;
					product->update_product_state();
					if (abort_sync_version())		
						product->synchronise_software_version(*sd, board);
					else
						sem_syslog_dbg("	Skip syncing version!!!\n");
					
					if (sem_thread_create(&slave_server_thread, (void *)non_active_master_board_server_thread, (void *)(*sd), IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
					{
						sem_syslog_dbg("	thread non_active_master_board_server_thread create failed.System need to reboot\n");
						return 1;
					}

					if (sem_thread_create(&stby_keep_alive_thread, (void *)non_active_master_board_keep_alive_test_client, (void *)(*sd), IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
					{
						sem_syslog_dbg("	thead non_active_master_board_keep_alive_test_client create failed.System need to reboot\n");
						return 1;
					}

					board->board_state = BOARD_READY;
					board->update_local_board_state(board);
					board_info_to_syn.board_state = board->board_state;
					board_info_to_syn.is_active_master = board->is_active_master;
					board->board_register(board, *sd, &board_info_to_syn);

					sprintf(file_path, "/dbm/shm/ethport/shm%d", local_board->slot_id+1);
	            	sem_syslog_dbg("\tslot%d send ethport shm file to active_MCB\n", local_board->slot_id+1);
	            	product->sync_send_file(product->active_master_slot_id, file_path, 0);
					sem_init_completed();			
				}
				else if (ret == -1)
				{
					sem_syslog_dbg("	active link test failed\n");
					close(g_send_sd);
					close(g_recv_sd);
					return 1;
				}
				else
				{
					// TODO:default as active master board
					sem_syslog_dbg("	link failed\n");
					ret = product->force_active_master();
					if (ret)
					{
						sem_syslog_dbg("	force active master failed.System need to reboot\n");
						return 1;
					}
					else
					{
						sem_syslog_dbg("	force active master done\n");
					}
					
					close(g_send_sd);
					close(g_recv_sd);
					
					if (sem_thread_create(&master_server_thread, (void *)active_master_board_server_thread, NULL, IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
					{
						sem_syslog_dbg("	thread active_master_board_server_thread create faile.System need to reboot\n");
						return 1;
					}
					
					board->is_active_master = ACTIVE_MASTER;
					board->is_use_default_master = 1;
					
					
					product->more_than_one_master_board_on = 0;
					product->active_master_slot_id = board->slot_id;
					product->update_product_state();
					
					if (get_software_version(active_mcb_version, active_mcb_buildno))
					{
						sem_syslog_dbg("	active master board get software version failed.System need to reboot\n");
						return 1;
					}
					else
					{
						sem_syslog_dbg("	active_mcb_version = %s, active_mcb_buildno = %s\n", active_mcb_version, active_mcb_buildno);
					}

					
					if (sem_thread_create(&master_keep_alive_test_thread, (void *)active_master_board_keep_alive_thread, NULL, IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
					{
						sem_syslog_dbg("	thread active_master_board_keep_alive_thread create failed.System need to reboot\n");
						return 1;
					}
					
					// TODO: active master board reinit product info and local board info

					sem_init_completed();
					board->board_state = BOARD_READY;
					board_info_to_syn.board_state = board->board_state;
					board_info_to_syn.is_active_master = board->is_active_master;
					board->update_local_board_state(board);
					product->product_state = SYSTEM_DSICOVER;
					product->syn_product_info(&board_info_to_syn);
					sem_syslog_dbg("	wait for boards ready.....");
					product->update_product_state();
					product_ready_test();
					sem_syslog_dbg("	done\n");
					
					product->product_state = SYSTEM_READY;
					product->syn_product_info(&board_info_to_syn);
					product->update_product_state();
        			sem_syslog_dbg("SEM_MAIN:enable dbm effective flag!\n");
                    set_dbm_effective_flag(VALID_DBM_FLAG);
					product_sync_system_time();
					product_state_notifier(SYSTEM_READY);

				}
			}
			
		}
		/*remote not inserted*/
		else
		{
			// TODO:default as active master board
			ret = product->force_active_master();
			if (ret)
			{
				sem_syslog_dbg("	force active master failed.System need to reboot\n");
				return 1;
			}
			else
			{
				sem_syslog_dbg("	force active master done\n");
			}
			
			sem_syslog_dbg("remote master is not on\n");
			if (sem_thread_create(&master_server_thread, (void *)active_master_board_server_thread, NULL, IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
			{
				sem_syslog_dbg("	thread active_master_board_server_thread create failed.System need to reboot\n");
				return 1;
			}
				
			board->is_active_master = ACTIVE_MASTER;
			board->is_use_default_master = 1;
			
			
			product->more_than_one_master_board_on = 0;
			product->active_master_slot_id = board->slot_id;
			product->update_product_state();
			
			if (get_software_version(active_mcb_version, active_mcb_buildno))
			{
				sem_syslog_dbg("	active master board get software version failed.System need to reboot\n");
				return 1;
			}
			
			sem_syslog_dbg("	active_mcb_version = %s, active_mcb_buildno = %s\n", active_mcb_version, active_mcb_buildno);
				
			if (sem_thread_create(&master_keep_alive_test_thread, (void *)active_master_board_keep_alive_thread, NULL, IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
			{
				sem_syslog_dbg("	thread active_master_board_keep_alive_thread create failed.System need to reboot\n");
				return 1;
			}
			// TODO: active master board reinit product info and local board info

			sem_init_completed();
			board->board_state = BOARD_READY;
			board->update_local_board_state(board);
			board_info_to_syn.board_state = board->board_state;
			board_info_to_syn.is_active_master = board->is_active_master;
			product->product_state = SYSTEM_DSICOVER;
			product->syn_product_info(&board_info_to_syn);
			
			sem_syslog_dbg("	wait for board ready.....");
			product->update_product_state();
			product_ready_test();
			product->product_state = SYSTEM_READY;
			product->syn_product_info(&board_info_to_syn);
			product->update_product_state();
			sem_syslog_dbg("	done\n");
			sem_syslog_dbg("SEM_MAIN:enable dbm effective flag!\n");
            set_dbm_effective_flag(VALID_DBM_FLAG);
			product_sync_system_time();
			product_state_notifier(SYSTEM_READY);

		}
	}
	/*not the default master slot*/
	else
	{
		sem_syslog_dbg("	not default master slot board init\n");
		ret = product->is_remote_master_on(&insert_state);
		/*remote is on*/
		if (insert_state)
		{
			sem_syslog_dbg("	remote master is on.wait for 10 seconds\n");
			sleep(STANDBY_DELAY_SECONDS);
			//for recving
			g_recv_sd = socket(AF_TIPC, SOCK_RDM, 0);

			if (g_recv_sd < 0)
			{
				sem_syslog_warning("	create recv socket failed:g_recv_sd = %d\n", g_recv_sd);
				return -1;
			}
			
			if (tipc_set_recv_addr(&g_recv_sock_addr, SEM_TIPC_INSTANCE_BASE_ACTIVE_TO_NON_ACTIVE+local_board->slot_id, SEM_TIPC_INSTANCE_BASE_ACTIVE_TO_NON_ACTIVE+local_board->slot_id))
			{
				sem_syslog_dbg("	set server tipc addr failed\n");
				close(g_recv_sd);
				return -1;
			}

			if (0 != bind (g_recv_sd, (struct sockaddr*)&g_recv_sock_addr, sizeof(struct sockaddr)))
		  	{
				sem_syslog_dbg("	bind failed, return\n");
				close(g_recv_sd);
				return -1;
		  	}

			//for STBY sending
			g_send_sd = socket(AF_TIPC, SOCK_RDM, 0);
			if (g_send_sd < 0)
			{
				sem_syslog_warning("	STBY create socket failed:*sd = %d\n", g_send_sd);
				return -1;
			}

			if (tipc_set_send_addr(&g_send_sock_addr, SEM_TIPC_INSTANCE_BASE_NON_ACTIVE_TO_ACTIVE+local_board->slot_id))
			{
				sem_syslog_dbg("default_master_slot_id	set client addr failed\n");
				return -1;
			}
			
			if (product->remote_master_state(&remote_state))
			{
				sem_syslog_warning("get remote master state failed.assume the remote master is active\n");
				remote_state = 0;
			}

			//remote master is act as non-active MCB
			if (remote_state)
			{
				ret = product->force_active_master();
				if (ret)
				{
					sem_syslog_dbg("	force active master failed.System need to reboot\n");
					return 1;
				}
				else
				{
					sem_syslog_dbg("	force active master done\n");
				}
				
				if (sem_thread_create(&master_server_thread, (void *)active_master_board_server_thread, NULL, IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
				{
					sem_syslog_dbg("	thread active_master_board_server_thread create failed.System need to reboot\n");					
					return 1;
				}

				close(g_send_sd);
				close(g_recv_sd);
				
				board->is_active_master = ACTIVE_MASTER;
				board->is_use_default_master = 0;
				
				
				product->more_than_one_master_board_on = 0;
				product->active_master_slot_id = board->slot_id;
				product->update_product_state();
				
				if (get_software_version(active_mcb_version, active_mcb_buildno))
				{
					sem_syslog_dbg("	active master board get software version failed.System need to reboot\n");
				}
				sem_syslog_dbg("	active_mcb_version = %s, active_mcb_buildno = %s\n", active_mcb_version, active_mcb_buildno);
				
				if (sem_thread_create(&master_keep_alive_test_thread, (void *)active_master_board_keep_alive_thread, NULL, IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
				{
					sem_syslog_dbg("	thread active_master_board_keep_alive_thread create failed.System need to reboot\n");
					return 1;
				}
				// TODO: active master board reinit product info and local board info

                sem_init_completed();
				board->board_state = BOARD_READY;
				board->update_local_board_state(board);
				board_info_to_syn.board_state = board->board_state;
				board_info_to_syn.is_active_master = board->is_active_master;
				product->product_state = SYSTEM_DSICOVER;
				product->syn_product_info(&board_info_to_syn);
				
				sem_syslog_dbg("	wait for board ready.....");
				product->update_product_state();
				product_ready_test();
				product->product_state = SYSTEM_READY;
				product->syn_product_info(&board_info_to_syn);
				product->update_product_state();
				sem_syslog_dbg("	done\n");
				sem_syslog_dbg("SEM_MAIN:enable dbm effective flag!\n");
                set_dbm_effective_flag(VALID_DBM_FLAG);
				product_sync_system_time();
				product_state_notifier(SYSTEM_READY);

			}
			//remote master is allready been act as active MCB
			else
			{
				sem_syslog_dbg("	remote master is allready been act as active MCB\n");
				sleep(2);
				ret = active_link_test(SME_CONN_LINK_TEST_COUNT);
			
				if (ret == 0)
				{
					// TODO:non-default as backup master board
					sem_syslog_dbg("	link on\n");

					board->is_active_master = NOT_ACTIVE_MASTER;
					board->is_use_default_master = 1;
					
					
					product->more_than_one_master_board_on = 1;
					product->active_master_slot_id = product->default_master_slot_id;
					product->update_product_state();
					
					if (abort_sync_version())		
						product->synchronise_software_version(*sd, board);
					else
						sem_syslog_dbg("	Skip syncing version!!!\n");

					if (sem_thread_create(&slave_server_thread, (void *)non_active_master_board_server_thread, (void *)(*sd), IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
					{
						sem_syslog_dbg("	thread non_active_master_board_server_thread create failed.System need to reboot\n");
						return 1;
					}
					
					sem_syslog_dbg("	stby board register board state\n");
					
					if (sem_thread_create(&stby_keep_alive_thread, (void *)non_active_master_board_keep_alive_test_client, (void *)(*sd), IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
					{
						sem_syslog_dbg("	thread non_active_master_board_keep_alive_test_client create failed.System need to reboot\n");
						return 1;
					}

					board->board_state = BOARD_READY;
					board->update_local_board_state(board);
					board_info_to_syn.board_state = board->board_state;
					board_info_to_syn.is_active_master = board->is_active_master;
					board->board_register(board, *sd, &board_info_to_syn);
					
	                sprintf(file_path, "/dbm/shm/ethport/shm%d", local_board->slot_id+1);
	            	sem_syslog_dbg("\tslot%d send ethport shm file to active_MCB\n", local_board->slot_id+1);
	            	product->sync_send_file(*sd, file_path, 0);
	    				
	                sem_init_completed();				
				}
				else if (ret == -1) 
				{
					sem_syslog_dbg("	active link test failed.System need to reboot\n");
					close(g_send_sd);
					close(g_recv_sd);
					return 1;
				}
				else
				{
					// TODO:non-default as active master board
					sem_syslog_dbg("	link failed\n");
					ret = product->force_active_master();
					if (ret)
					{
						sem_syslog_dbg("	force active master failed.System need to reboot\n");
						return 1;
					}
					else
					{
						sem_syslog_dbg("	force active master done\n");
					}
					
					if (sem_thread_create(&master_server_thread, (void *)active_master_board_server_thread, NULL, IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
					{
						sem_syslog_dbg("	thread active_master_board_server_thread create failed.System need to reboot\n");					
						return 1;
					}

					close(g_send_sd);
					close(g_recv_sd);
					
					board->is_active_master = ACTIVE_MASTER;
					board->is_use_default_master = 0;
					
					
					product->more_than_one_master_board_on = 0;
					product->active_master_slot_id = board->slot_id;
					product->update_product_state();
					
					if (get_software_version(active_mcb_version, active_mcb_buildno))
					{
						sem_syslog_dbg("	active master board get software version failed.System need to reboot\n");
					}
					sem_syslog_dbg("	active_mcb_version = %s, active_mcb_buildno = %s\n", active_mcb_version, active_mcb_buildno);
					
					if (sem_thread_create(&master_keep_alive_test_thread, (void *)active_master_board_keep_alive_thread, NULL, IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
					{
						sem_syslog_dbg("	thread active_master_board_keep_alive_thread create failed.System need to reboot\n");
						return 1;
					}
					// TODO: active master board reinit product info and local board info

	                sem_init_completed();
					board->board_state = BOARD_READY;
					board->update_local_board_state(board);
					board_info_to_syn.board_state = board->board_state;
					board_info_to_syn.is_active_master = board->is_active_master;
					product->product_state = SYSTEM_DSICOVER;
					product->syn_product_info(&board_info_to_syn);
					
					sem_syslog_dbg("	wait for board ready.....");
					product->update_product_state();
					product_ready_test();
					product->product_state = SYSTEM_READY;
					product->syn_product_info(&board_info_to_syn);
					product->update_product_state();
					sem_syslog_dbg("	done\n");
        			sem_syslog_dbg("SEM_MAIN:enable dbm effective flag!\n");
                    set_dbm_effective_flag(VALID_DBM_FLAG);
					product_sync_system_time();
					product_state_notifier(SYSTEM_READY);

				}
			}
		}
		/*remote is not on*/
		else
		{
			// TODO:non-default as active master board
			if (sem_thread_create(&master_server_thread, (void *)active_master_board_server_thread, NULL, IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
			{
				sem_syslog_dbg("	thread active_master_board_server_thread create failed.System need to reboot\n");
				return 1;
			}
			
			ret = product->force_active_master();
			if (ret)
			{
				sem_syslog_dbg("	force active master failed.System need to reboot\n");
				return 1;
			}
			else
			{
				sem_syslog_dbg("	force active master done\n");
			}
				
			board->is_active_master = ACTIVE_MASTER;
			board->is_use_default_master = 0;
			board->update_local_board_state(board);
			
			product->more_than_one_master_board_on = 0;
			product->active_master_slot_id = board->slot_id;
			product->update_product_state();
			
			if (get_software_version(active_mcb_version, active_mcb_buildno))
			{
				sem_syslog_dbg("	active master board get software version failed.System need to reboot\n");
				return 1;
			}
			sem_syslog_dbg("	active_mcb_version = %s, active_mcb_buildno = %s\n", active_mcb_version, active_mcb_buildno);
			
			if (sem_thread_create(&master_keep_alive_test_thread, (void *)active_master_board_keep_alive_thread, NULL, IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
			{
				sem_syslog_dbg("	thread active_master_board_keep_alive_thread create failed.System need to reboot\n");
				return 1;
			}
			
			// TODO: active master board reinit product info and local board info

            sem_init_completed();
			board->board_state = BOARD_READY;
			board->update_local_board_state(board);
			board_info_to_syn.board_state = board->board_state;
			board_info_to_syn.is_active_master = board->is_active_master;
			product->product_state = SYSTEM_DSICOVER;
			product->syn_product_info(&board_info_to_syn);

			sem_syslog_dbg("	wait for board ready.....");
			product->update_product_state();
			product_ready_test();
			product->product_state = SYSTEM_READY;
			product->syn_product_info(&board_info_to_syn);
			product->update_product_state();
			sem_syslog_dbg("	done\n");
			sem_syslog_dbg("SEM_MAIN:enable dbm effective flag!\n");
            set_dbm_effective_flag(VALID_DBM_FLAG);
			product_sync_system_time();
			product_state_notifier(SYSTEM_READY);

		}
	}
	return 0;
}

/*
  *return 0 for success 1 for failed
  *
  */
int ax8603_active_stby_switch(void *arg)
{
	int ret;
	int remote_board_state;
	int insert_state = 0;
	int retval = 0;
	mcb_switch_arg_t *mcb_switch_arg = NULL;

	set_dbm_effective_flag(INVALID_DBM_FLAG);
	sem_syslog_warning("thread %s, thread id is %d\n", __FUNCTION__, getpid());
	if (!product->more_than_one_master_board_on)
	{
		sem_syslog_dbg("only one master board.active and stby switch is not allowed\n");
		return 1;
	}
	
	// TODO:the board switch to be stby board
	if (local_board->is_active_master)
	{
		sem_syslog_dbg("active master board switch to stby board\n");
		// TODO:step one reset some global variable
		for (ret=0; ret<product->slotcount; ret++)
		{
			if (thread_id_arr[ret].thread_index != FREE_THREAD_ID)
			{
				if (!pthread_cancel(thread_id_arr[ret].thread_index))
				{
					sem_syslog_dbg("active MCB cancel recv thread for slot %d succeed\n", ret+1);
				}
				else
				{
					sem_syslog_warning("active MCB cancel recv thread for slot %d failed\n", ret+1);
					return 1;
				}
			}
			thread_id_arr[ret].thread_index = FREE_THREAD_ID;
			thread_id_arr[ret].sd = SEM_FREE_SD;
			thread_id_arr[ret].slot_id = USELESS_SLOT_ID;
			thread_id_arr[ret].is_aliving = IS_DEAD;
		}

		for (ret=0; ret<product->slotcount; ret++)
		{
			if (ret != local_board->slot_id)
			{
				close(g_send_sd_arr[ret].sd);
				close(g_recv_sd_arr[ret].sd);
			}
		}
		
		if (active_server_sd >= 0)
		{
			close(active_server_sd);
			active_server_sd = SEM_FREE_SD;
		}
		
		// TODO:step two cancel active threads
		
		if (!pthread_cancel(master_keep_alive_test_thread))
		{
			sem_syslog_dbg("cancel master_keep_alive_test_thread OK\n");
		}
		else
		{
			sem_syslog_dbg("cancel master_keep_alive_test_thread Failed\n");
			return 1;
		}

		//sleep(1);

		//get remote master board master state
		if (product->remote_master_state(&remote_board_state))
		{
			sem_syslog_warning("get remote master state failed\n");
			return 1;
		}
		
		//is active master
		if (!remote_board_state)
		{
			sem_syslog_dbg("remote is active MCB,wait for 3 seconds\n");
			sleep(1);
		}

		//for recving
		g_recv_sd = socket(AF_TIPC, SOCK_RDM, 0);

		if (g_recv_sd < 0)
		{
			sem_syslog_warning("	create recv socket failed:g_recv_sd = %d\n", g_recv_sd);
			return 1;
		}
		
		if (tipc_set_recv_addr(&g_recv_sock_addr, SEM_TIPC_INSTANCE_BASE_ACTIVE_TO_NON_ACTIVE+local_board->slot_id, SEM_TIPC_INSTANCE_BASE_ACTIVE_TO_NON_ACTIVE+local_board->slot_id))
		{
			sem_syslog_dbg("	set server tipc addr failed\n");
			close(g_recv_sd);
			return 1;
		}

		if (0 != bind (g_recv_sd, (struct sockaddr*)&g_recv_sock_addr, sizeof(struct sockaddr)))
		{
			sem_syslog_dbg("	bind failed, return\n");
			close(g_recv_sd);
			return 1;
		}

		//for STBY sending
		g_send_sd = socket(AF_TIPC, SOCK_RDM, 0);
		if (g_send_sd < 0)
		{
			sem_syslog_warning("	STBY create socket failed:*sd = %d\n", g_send_sd);
			return 1;
		}

		if (tipc_set_send_addr(&g_send_sock_addr, SEM_TIPC_INSTANCE_BASE_NON_ACTIVE_TO_ACTIVE+local_board->slot_id))
		{
			sem_syslog_dbg("default_master_slot_id	set client addr failed\n");
			return 1;
		}
				
		sem_syslog_dbg("active stby switch active link test........\n");
		sleep(3);
		ret = active_link_test(SME_CONN_LINK_TEST_COUNT);
		
		/*the remote board already been active*/
		if (ret == 0)
		{
			// TODO:default as backup master board
			sem_syslog_dbg("	link on:g_recv_sd = %d\n", g_recv_sd);
			
			local_board->is_active_master = NOT_ACTIVE_MASTER;
			local_board->is_use_default_master = 0;
				
			product->more_than_one_master_board_on = 1;
			if (local_board->slot_id == product->master_slot_id[0])
			{
				product->active_master_slot_id = product->master_slot_id[1];
			}
			else
			{
				product->active_master_slot_id = product->master_slot_id[0];
			}
			product->update_product_state();
			
			sem_thread_create(&slave_server_thread, (void *)non_active_master_board_server_thread, (void *)(g_recv_sd), IS_DETACHED);

			//local_board->board_register(local_board, *temp_sd, &board_info_to_syn);

			sem_thread_create(&stby_keep_alive_thread, (void *)non_active_master_board_keep_alive_test_client, (void *)(g_recv_sd), IS_DETACHED);

			local_board->board_state = BOARD_READY;
			local_board->update_local_board_state(local_board);
			board_info_to_syn.board_state = local_board->board_state;
			board_info_to_syn.is_active_master = local_board->is_active_master;
			//local_board->syn_board_info(*temp_sd, &board_info_to_syn);
			local_board->board_register(local_board, g_send_sd, &board_info_to_syn);

			return 0;
		}
		else if (ret == -1)
		{
			sem_syslog_dbg("	active link test failed\n");
			close(g_send_sd);
			close(g_recv_sd);
			return 1;
		}
		else
		{
			ret = product->force_active_master();
			if (ret)
			{
				sem_syslog_dbg("	force active master failed\n");
			}
			else
			{
				sem_syslog_dbg("	force active master done\n");
			}

			close(g_send_sd);
			close(g_recv_sd);
			sem_syslog_dbg("	link failed\n");
			
			local_board->is_active_master = ACTIVE_MASTER;
			local_board->is_use_default_master = 1;
			
			
			product->more_than_one_master_board_on = 1;
			product->active_master_slot_id = local_board->slot_id;
			product->update_product_state();
			
			if (get_software_version(active_mcb_version, active_mcb_buildno))
			{
				sem_syslog_dbg("	active master board get software version failed\n");
			}
			else
			{
				sem_syslog_dbg("	active_mcb_version = %s, active_mcb_buildno = %s\n", active_mcb_version, active_mcb_buildno);
			}
	
			if (sem_thread_create(&master_server_thread, (void *)active_master_board_server_thread, NULL, IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
			{
				sem_syslog_dbg("create active_master_board_server_thread failed\n");
			}
				
			if (sem_thread_create(&master_keep_alive_test_thread, (void *)active_master_board_keep_alive_thread, NULL, IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
			{
				sem_syslog_dbg("create active_master_board_keep_alive_thread failed\n");
			}
			
			sem_init_completed();
			local_board->board_state = BOARD_READY;
			board_info_to_syn.board_state = local_board->board_state;
			board_info_to_syn.is_active_master = local_board->is_active_master;
			local_board->update_local_board_state(local_board);
			product->product_state = SYSTEM_DSICOVER;
			product->syn_product_info(&board_info_to_syn);
			sem_syslog_dbg("wait for board ready.....");
			
			product->update_product_state();

			sem_syslog_dbg("done\n");
			product->product_state = SYSTEM_READY;
			product->syn_product_info(&board_info_to_syn);
			product->update_product_state();
			
			//product_state_notifier(SYSTEM_READY);
			set_dbm_effective_flag(VALID_DBM_FLAG);

			return 1;
		}
	}
	// TODO:the board switch to be active board
	else
	{
		int timeout = 3;
		int i = 0;
		
		// TODO:step four start active MCB threads
		ret = product->force_active_master();
		if (ret)
		{
			sem_syslog_dbg("	force active master failed\n");
		}
		else
		{
			sem_syslog_dbg("	force active master done\n");
		}
		
		sem_syslog_dbg("stby board switch to active MCB\n");
		// TODO: step one reinite part necessary variable

		for (ret=0; ret<product->slotcount; ret++)
		{
			thread_id_arr[ret].thread_index = FREE_THREAD_ID;
			thread_id_arr[ret].sd = SEM_FREE_SD;
			thread_id_arr[ret].slot_id = USELESS_SLOT_ID;
			thread_id_arr[ret].is_aliving = IS_DEAD;
		}

		for (ret=0; ret<MAX_SLOT_NUM; ret++)
		{
			g_recv_sd_arr[ret].sd = SEM_FREE_SD;
			g_send_sd_arr[ret].sd = SEM_FREE_SD;
		}
		
		sem_syslog_dbg("cancel slave_server_thread\n");
		pthread_cancel(slave_server_thread);
		sem_syslog_dbg("cancel stby_keep_alive_thread\n");
		pthread_cancel(stby_keep_alive_thread);

		close(g_send_sd);
		close(g_recv_sd);

		for (i = 0; i < product->slotcount; i++)
		{
			if (product_info_syn.product_slot_board_info[i].board_state >= BOARD_READY)
			{
				product_info_syn.product_slot_board_info[i].board_state = BOARD_INSERTED;
			}
		}
		
		// TODO:step four start active MCB threads
		sem_syslog_dbg("	create active_master_board_server_thread \n");		
		if (sem_thread_create(&master_server_thread, (void *)active_master_board_server_thread, NULL, IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
		{
			sem_syslog_dbg("	create active_master_board_server_thread failed\n");
			retval = -1;
		}

		local_board->is_active_master = ACTIVE_MASTER;
		if (local_board->slot_id == product->default_master_slot_id)
		{
			local_board->is_use_default_master = 1;
		}
		else
		{
			local_board->is_use_default_master = 0;
		}
		
		// TODO:more than on master board on may not correct.		
		ret = product->is_remote_master_on(&insert_state);
		if (insert_state == 1)
		{
			product->more_than_one_master_board_on = 1;
		}
		else
		{
			product->more_than_one_master_board_on = 0;
			for(i = 0; i < BOARD_GLOBAL_ETHPORTS_MAXNUM; i++)
			{
				start_fp[product->active_master_slot_id][i].isValid = 0;
			}
		}
		product->active_master_slot_id = local_board->slot_id;
		product->get_board_on_mask(&product->board_on_mask, local_board->slot_id, product->default_master_slot_id);
		product->update_product_state();
		
		if (get_software_version(active_mcb_version, active_mcb_buildno))
		{
			sem_syslog_dbg("	active master board get software version failed\n");
			retval = -1;
		}
		else
		{
			sem_syslog_dbg("	active_mcb_version = %s, active_mcb_buildno = %s\n", active_mcb_version, active_mcb_buildno);
		}

		sem_syslog_dbg("	create active_master_board_keep_alive_thread\n");
		if (sem_thread_create(&master_keep_alive_test_thread, (void *)active_master_board_keep_alive_thread, NULL, IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
		{
			sem_syslog_dbg("	create active_master_board_keep_alive_thread failed\n");
			retval = -1;
		}
		//sem_syslog_dbg("	active master board keep alive thread done\n");
		mcb_switch_arg = (mcb_switch_arg_t *)arg;
		while (timeout--)
		{
			sleep(1);
			if (active_mcb_server_flag)
			{
				sem_syslog_dbg("switch type : %d\n", mcb_switch_arg->switch_type);
				sem_syslog_dbg("broadcast domain : %d\n", mcb_switch_arg->broadcast_domain);
				if (retval != -1)
					mcb_active_standby_switch_notifier(mcb_switch_arg->switch_type, ACTIVE_STDBY_SWITCH_OCCUR_EVENT, 
						local_board->slot_id + 1, ALL_BROADCAST);
				if	(mcb_switch_arg->switch_type == HOTPLUG_MODE || mcb_switch_arg->switch_type == PANIC_MODE) {
					for(i = 0; i < product->slotcount; i++) 
					{
						sem_syslog_dbg("g_send_sd_arr[%d].sd = %d\n", i, g_send_sd_arr[i].sd);
						if ((i != local_board->slot_id) && \
							(g_send_sd_arr[i].sd >= 0) && \
							(product->board_on_mask & (1 << i)))
						{
							mcb_active_standby_switch_notification(i);
							sem_syslog_dbg("mcb_active_standby_switch_notification send to slot %d sd %d\n", \
								i + 1, g_send_sd_arr[i].sd);
						}
					}
				}
				break;
			}
		}
		
		// TODO: active master board reinit product info and local board info

		sem_init_completed();
		local_board->board_state = BOARD_READY;
		board_info_to_syn.board_state = local_board->board_state;
		board_info_to_syn.is_active_master = local_board->is_active_master;
		local_board->update_local_board_state(local_board);
		product->product_state = SYSTEM_DSICOVER;
		sem_syslog_dbg("syn product info\n");
		product->syn_product_info(&board_info_to_syn);
		
		sem_syslog_dbg("wait for board ready.....");		
		product->update_product_state();
		system("echo 1 > /dbm/parameters/abort_check_product_state_reboot");
		product_ready_test();
		sem_syslog_dbg("done\n");
		
		product->product_state = SYSTEM_READY;
		product->syn_product_info(&board_info_to_syn);
		product->update_product_state();
		
		//product_state_notifier(SYSTEM_READY);
		
		/* broadcast the active standby switch message to boards */
		mcb_active_standby_switch_notifier(mcb_switch_arg->switch_type, ACTIVE_STDBY_SWITCH_COMPLETE_EVENT, 
			local_board->slot_id + 1, ALL_BROADCAST);
		sem_syslog_warning("stby switch to active MCB done\n");
		pthread_mutex_lock(&switch_lock);
		switch_flag = 0;
		pthread_mutex_unlock(&switch_lock);
		set_dbm_effective_flag(VALID_DBM_FLAG);
		
		return retval;
	}
}

/*
 *
 *return :1 need , 0 not need, -1 for failed
 *
 */
static int is_need_to_syn_software_version(char *version, char *buildno)
{
	if (!version || !buildno)
	{
		return -1;
	}

	if ((strcmp(active_mcb_version, version) == 0) && (strcmp(active_mcb_buildno, buildno) == 0))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/*
 *
 *
 *return:1 for failed, 0 for success
 *
 */
static int get_remote_software_version(sem_tlv_t *tlv_head, char *version, char *buildno)
{
	software_version_request_t *head;
	if (!tlv_head || !version || !buildno)
	{
		return 1;
	}
	
	if (tlv_head->type == SEM_SOFTWARE_VERSION_SYN_REQUEST)
	{
		head = (software_version_request_t *)((char *)tlv_head + sizeof(sem_tlv_t));
		strcpy(version, head->version);
		strcpy(buildno, head->buildno);
		sem_syslog_dbg("remote version is %s , buildno is %s\n", version, buildno);
		return 0;
	}
	else
	{
		return 1;
	}
}

int ax8603_syn_software_version_response(int sd, sem_tlv_t *temp_tlv_head, board_fix_param_t *board, int slot_id)
{
	char remote_version[MAX_SOFTWARE_VERSION_LEN];
	char remote_buildno[MAX_SOFTWARE_VERSION_LEN];
	char version_name[MAX_SOFTWARE_VERSION_FILE_NAME_LEN];
	char version_path[MAX_SOFTWARE_VERSION_FILE_PATHE_LEN];
	char *send_buf;
	char *pdu_head;
	char str[128] = {0};
	char md5_buf[128];
	int fd_md5;
	sem_pdu_head_t *head;
	sem_tlv_t *tlv_head;
	struct sockaddr_tipc sock_addr;
	software_version_response_t *response_head;
	software_version_text_t *version_text_head;
	int counter = 0, byte_counter = 0;
	int send_soft_ware_version_try_count = SOFTWARE_VERSION_SEND_TRY_COUNT;
	
	memset(version_path, 0, MAX_SOFTWARE_VERSION_FILE_PATHE_LEN);
	
	FILE *fp = NULL;
	
	if (get_remote_software_version(temp_tlv_head, remote_version, remote_buildno))
	{
		sem_syslog_dbg("get reomte software version failed\n");
		return SOFTWARE_SYN_FAILED;
	}

	/*need to syn software version*/
	if (is_need_to_syn_software_version(remote_version, remote_buildno))
	{
		sem_syslog_warning("need to syn software version to slot %d.\n", slot_id+1);
		pdu_head = (char *)(malloc(sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t) + sizeof(software_version_text_t)));
		
		head = (sem_pdu_head_t *)pdu_head;
		head->slot_id = board->slot_id;
		
		tlv_head = (sem_tlv_t *)(pdu_head + sizeof(sem_pdu_head_t));
		tlv_head->type = SEM_SOFTWARE_VERSION_SYN_RESPONSE;

		response_head = (software_version_response_t *)(pdu_head + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
		response_head->is_need = 1;
		strcpy(response_head->version, active_mcb_version);
		strcpy(response_head->buildno, active_mcb_buildno);
		
		if (g_send_sd_arr[slot_id].sd < 0)			
		{
			sem_syslog_warning("error send sd to slot %d\n", slot_id+1);
			free(pdu_head);
			return SOFTWARE_SYN_FAILED;
		}
		
		if (sendto(g_send_sd_arr[slot_id].sd, pdu_head, sizeof(sem_pdu_head_t)+sizeof(sem_tlv_t)+ sizeof(software_version_response_t), 0, (struct sockaddr*)&g_send_sd_arr[slot_id].sock_addr, sizeof(struct sockaddr_tipc)) < 0)
		{
			sem_syslog_dbg("need:server response failed\n");
			free(pdu_head);
			return SOFTWARE_SYN_FAILED;
		}
		
		head = (sem_pdu_head_t *)pdu_head;
		head->slot_id = board->slot_id;
		tlv_head = (sem_tlv_t *)(pdu_head + sizeof(sem_pdu_head_t));
		tlv_head->type = SEM_SOFTWARE_VERSION_SYNING;

		version_text_head = (software_version_text_t *)(pdu_head + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
		//send_buf = version_text_head->buf;

		sprintf(version_name, "AW%s.%s.X7X5.IMG", active_mcb_version, active_mcb_buildno);
		system("sudo mount /blk");
		fp = fopen(strcat(strcat(version_path, NEW_SOFTWARE_VERSION_PATH), version_name), "r");
		
		if (!fp)
		{
			sem_syslog_dbg("version file %s open failed\n", version_path);
			free(pdu_head);
			// TODO:huxuefeng  It should send a message to notice the slave board
			return SOFTWARE_SYN_FAILED;
		}

		sem_syslog_dbg("open file %s success\n", version_path);
		//head->version = 0;
		
		while (!feof(fp))
		{
			//sem_syslog_dbg("send version file:%d\n", counter++);
			version_text_head->len = fread(version_text_head->buf, sizeof(char), MAX_SOFTWARE_TEXT_LEN, fp);
			byte_counter += version_text_head->len;
					
			while ((sendto(g_send_sd_arr[slot_id].sd, pdu_head, sizeof(sem_pdu_head_t)+sizeof(sem_tlv_t)+ sizeof(software_version_text_t), 0, (struct sockaddr*)&g_send_sd_arr[slot_id].sock_addr, sizeof(struct sockaddr_tipc))  < 0) &&
				send_soft_ware_version_try_count-- >= 0)
			{
				sem_syslog_dbg("send failed,retry %d\n", send_soft_ware_version_try_count);
				if (send_soft_ware_version_try_count < 0)
				{
					fclose(fp);
					return SOFTWARE_SYN_FAILED;
				}
			}
			usleep(100);
		}

		sem_syslog_dbg("send file done.send bytes:%d\n", byte_counter);

		sprintf(str, "sudo /usr/bin/sor.sh imgmd5 /blk/%s 120 > /mnt/md5", version_name);
		system(str);
		fd_md5= open("/mnt/md5", O_RDONLY);
		if(fd_md5 < 0)
		{
			sem_syslog_dbg("file %s open failed.%s\n", "/mnt/md5", __FUNCTION__);
			free(pdu_head);
			fclose(fp);
			return SOFTWARE_SYN_FAILED;
		}
		read(fd_md5, md5_buf, 128);
		close(fd_md5);
		sem_tipc_send(slot_id, SEM_SEND_MD5_STR, md5_buf, sizeof(md5_buf));

		tlv_head->type = SEM_SOFTWARE_VERSION_SYN_FINISH;
		
		if (sendto(g_send_sd_arr[slot_id].sd, pdu_head, sizeof(sem_pdu_head_t)+sizeof(sem_tlv_t), 0, (struct sockaddr*)&g_send_sd_arr[slot_id].sock_addr, sizeof(struct sockaddr_tipc))  < 0)
		{
			sem_syslog_dbg("send finish command failed.\n");
		}
		free(pdu_head);
		fclose(fp);
		return  SOFTWARE_SYN_SUCCESS;
	}
	/*no need to syn software version*/
	else
	{
		sem_syslog_warning("no need to syn software version file to slot %d.\n", slot_id+1);
		pdu_head = (char *)(malloc(sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t) + sizeof(software_version_response_t)));
		
		head = (sem_pdu_head_t *)pdu_head;
		head->slot_id = board->slot_id;
		
		tlv_head = (sem_tlv_t *)(pdu_head + sizeof(sem_pdu_head_t));
		tlv_head->type = SEM_SOFTWARE_VERSION_SYN_RESPONSE;

		response_head = (software_version_response_t *)(pdu_head + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
		response_head->is_need = 0;
		strcpy(response_head->version, active_mcb_version);
		strcpy(response_head->buildno, active_mcb_buildno);

		if (sendto(g_send_sd_arr[slot_id].sd, pdu_head, sizeof(sem_pdu_head_t)+sizeof(sem_tlv_t)+sizeof(software_version_response_t), 0, (struct sockaddr*)&g_send_sd_arr[slot_id].sock_addr, sizeof(struct sockaddr_tipc)) < 0)
		{
			sem_syslog_dbg("no need:server response failed\n");
			free(pdu_head);
			return SOFTWARE_SYN_FAILED;
		}
		free(pdu_head);
		return SOFTWARE_SYN_NO_NEED;
	}
}

int ax8603_product_dbm_file_create(product_info_syn_t *product)
{
	FILE *fd;
	int i;
	char temp_buf[100];
	char temp_buf2[100];

	system("mkdir -p /dbm/product/");

	//system("touch /dbm/product/name");	
	fd = fopen("/dbm/product/name", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/product/name failed\n");
		return -1;
	}
	else
	{
		fprintf(fd, "%s\n", product->name);
		fclose(fd);
	}

	if (update_mac_enable && local_board != NULL)
	{
		fd = fopen("/dbm/product/base_mac", "w+");
		if (fd == NULL)
		{
			sem_syslog_dbg("open /dbm/product/base_mac failed\n");
			return -1;
		}
		else
		{
			fprintf(fd, "%02X%02X%02X%02X%02X%02X\n", product->base_mac_addr[0], product->base_mac_addr[1],\
				product->base_mac_addr[2], product->base_mac_addr[3], product->base_mac_addr[4], product->base_mac_addr[5]);
			fclose(fd);
		}
	}

	if (update_sn_enable && local_board != NULL)
	{
		fd = fopen("/dbm/product/sn", "w+");
		if (fd == NULL)
		{
			sem_syslog_dbg("open /dbm/product/sn failed.\n");
			return -1;
		}
		else
		{
			fprintf(fd, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n", product->sn[0], product->sn[1], product->sn[2],\
				product->sn[3], product->sn[4], product->sn[5], product->sn[6], product->sn[7], product->sn[8], product->sn[9]);
			fclose(fd);

			system("cp /dbm/product/sn /devinfo/sn");
			system("cp /dbm/product/sn /devinfo/product_sn");
		}

		update_sn_enable = 0;
	}

	fd = fopen("/dbm/product/slotcount", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/product/slotcount failed\n");
		return -1;
	}
	else
	{
		fprintf(fd, "%d\n", product->slotcount);
		fclose(fd);
	}


	fd = fopen("/dbm/product/master_slot_id", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/product/master_slot_id failed\n");
		return -1;
	}
	else
	{
		for (i=0; i<product->master_slot_count; i++)
			fprintf(fd, "%d\n", product->master_slot_id[i]+1);
		fclose(fd);
	}

	fd = fopen("/dbm/product/fan_num", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/product/fan_num failed\n");
		return -1;
	}
	else
	{
		fprintf(fd, "%d\n", product->fan_num);
		fclose(fd);
	}

	fd = fopen("/dbm/product/product_type", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/product/product_type failed\n");
		return -1;
	}
	else
	{
		fprintf(fd, "%d\n", product->product_type);
		fclose(fd);
	}

	fd = fopen("/dbm/product/product_serial", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/product/product_serial failed\n");
		return -1;
	}
	else
	{
		fprintf(fd, "%d\n", product->product_serial);
		fclose(fd);
	}
	
	fd = fopen("/dbm/product/master_slot_count", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/product/master_slot_count failed\n");
		return -1;
	}
	else
	{
		fprintf(fd, "%d\n", product->master_slot_count);
		fclose(fd);
	}

	fd = fopen("/dbm/product/more_than_one_master_board_on", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/product/more_than_one_master_board_on failed\n");
		return -1;
	}
	else
	{
		fprintf(fd, "%d\n", product->more_than_one_master_board_on);
		fclose(fd);
	}

	fd = fopen("/dbm/product/active_master_slot_id", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/product/active_master_slot_id failed\n");
		return -1;
	}
	else
	{
		fprintf(fd, "%d\n", product->active_master_slot_id+1);
		fclose(fd);
	}
	
	fd = fopen("/dbm/product/default_master_slot_id", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/product/default_master_slot_id failed\n");
		return -1;
	}
	else
	{
		fprintf(fd, "%d\n", product->default_master_slot_id + 1);
		fclose(fd);
	}
	
	fd = fopen("/dbm/product/is_distributed", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/product/is_distributed failed\n");
		return -1;
	}
	else
	{
		fprintf(fd, "%d\n", product->is_distributed);
		fclose(fd);
	}
	
	fd = fopen("/dbm/product/board_on_mask", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/product/board_on_mask failed\n");
		return -1;
	}
	else
	{
		fprintf(fd, "%u\n", product->board_on_mask);
		fclose(fd);
	}
	
	for (i=0; i<product->slotcount; i++)
	{
		/*create folder*/
		sprintf(temp_buf, "/dbm/product/slot/slot%d", i+1);
		strcpy(temp_buf2, "mkdir -p ");
		strcat(temp_buf2, temp_buf);
		system(temp_buf2);

		strcpy(temp_buf2, temp_buf);
		strcat(temp_buf2, "/board_code");
		fd = fopen(temp_buf2, "w+");
		if (fd == NULL)
		{
			sem_syslog_dbg("open %s failed\n", temp_buf2);
			return -1;
		}
		else
		{
			//fprintf(fd, "%s\n", product->board_sub_info_arr[i].name);
			fprintf(fd, "%d\n", product->product_slot_board_info[i].board_code);
			fclose(fd);
		}
		
		strcpy(temp_buf2, temp_buf);
		strcat(temp_buf2, "/function_type");
		fd = fopen(temp_buf2, "w+");
		if (fd == NULL)
		{
			sem_syslog_dbg("open %s failed\n", temp_buf2);
			return -1;
		}
		else
		{
			//fprintf(fd, "%d\n", product->board_sub_info_arr[i].index);
			fprintf(fd, "%u\n", product->product_slot_board_info[i].function_type);
			fclose(fd);
		}
		strcpy(temp_buf2, temp_buf);
		strcat(temp_buf2, "/is_master");
		fd = fopen(temp_buf2, "w+");
		if (fd == NULL)
		{
			sem_syslog_dbg("open %s failed\n", temp_buf2);
			return -1;
		}
		else
		{
			//fprintf(fd, "%d\n", product->board_sub_info_arr[i].index);
			fprintf(fd, "%d\n", product->product_slot_board_info[i].is_master);
			fclose(fd);
		}
		
		strcpy(temp_buf2, temp_buf);
		strcat(temp_buf2, "/name");
		fd = fopen(temp_buf2, "w+");
		if (fd == NULL)
		{
			sem_syslog_dbg("open %s failed\n", temp_buf2);
			return -1;
		}
		else
		{
			//fprintf(fd, "%d\n", product->board_sub_info_arr[i].index);
			fprintf(fd, "%s\n", product->product_slot_board_info[i].name);
			fclose(fd);
		}
		
		strcpy(temp_buf2, temp_buf);
		strcat(temp_buf2, "/slot_id");
		fd = fopen(temp_buf2, "w+");
		if (fd == NULL)
		{
			sem_syslog_dbg("open %s failed\n", temp_buf2);
			return -1;
		}
		else
		{
			//fprintf(fd, "%d\n", product->board_sub_info_arr[i].index);
			fprintf(fd, "%d\n", i+1);
			fclose(fd);
		}
		
		strcpy(temp_buf2, temp_buf);
		strcat(temp_buf2, "/board_state");
		fd = fopen(temp_buf2, "w+");
		if (fd == NULL)
		{
			sem_syslog_dbg("open %s failed\n", temp_buf2);
			return -1;
		}
		else
		{
			//fprintf(fd, "%d\n", product->board_sub_info_arr[i].index);
			fprintf(fd, "%d\n", product->product_slot_board_info[i].board_state);
			fclose(fd);
		}

		strcpy(temp_buf2, temp_buf);
		strcat(temp_buf2, "/is_active_master");
		fd = fopen(temp_buf2, "w+");
		if (fd == NULL)
		{
			sem_syslog_dbg("open %s failed\n", temp_buf2);
			return -1;
		}
		//fprintf(fd, "%d\n", product->board_sub_info_arr[i].index);
		fprintf(fd, "%d\n", product->product_slot_board_info[i].is_active_master);
		fclose(fd);

		strcpy(temp_buf2, temp_buf);
		strcat(temp_buf2, "/asic_start_no");
		fd = fopen(temp_buf2, "w+");
		if (fd == NULL)
		{
			sem_syslog_dbg("open %s failed\n", temp_buf2);
			return -1;
		}
		fprintf(fd, "%d\n", product->product_slot_board_info[i].asic_start_no);
		fclose(fd);

		strcpy(temp_buf2, temp_buf);
		strcat(temp_buf2, "/asic_port_num");
		fd = fopen(temp_buf2, "w+");
		if (fd == NULL)
		{
			sem_syslog_dbg("open %s failed\n", temp_buf2);
			return -1;
		}
		fprintf(fd, "%d\n", product->product_slot_board_info[i].asic_port_num);
		fclose(fd);

		strcpy(temp_buf2, temp_buf);
		strcat(temp_buf2, "/board_ap_counter");
		fd = fopen(temp_buf2, "w+");
		if (fd == NULL)
		{
			sem_syslog_dbg("open %s failed\n", temp_buf2);
			return -1;
		}
		fprintf(fd, "%d\n", product->product_slot_board_info[i].board_ap_counter);
		fclose(fd);	
	}

	fd = fopen("/dbm/product_state", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/product_state failed\n");
		return -1;
	}
	else
	{
		fprintf(fd, "%d\n", product->product_state);
		fclose(fd);
	}
	
	return 0;
}

/*
 *Update product board_on_mask and more_than_on_master_board_on files
 *
 */
int update_product_ax8603_state(void)
{
	FILE *fd;
	int i;

	//sem_syslog_dbg("%s boad_on_mask=0x%x\n", __FUNCTION__, product->board_on_mask);
	fd = fopen("/dbm/product/board_on_mask", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("update:open /dbm/product/board_on_mask failed\n");
		return -1;
	}
	else
	{
		fprintf(fd, "%u\n", product->board_on_mask);
		fclose(fd);	
	}

	fd = fopen("/dbm/product/more_than_one_master_board_on", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/product/more_than_one_master_board_on failed\n");
		return -1;
	}
	else
	{
		fprintf(fd, "%d\n", product->more_than_one_master_board_on);
		fclose(fd);
	}
	

	fd = fopen("/dbm/product/active_master_slot_id", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/product/active_master_slot_id failed\n");
		return -1;
	}
	else
	{
		fprintf(fd, "%d\n", product->active_master_slot_id+1);
		fclose(fd);
	}

	fd = fopen("/dbm/product_state", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("update:open /dbm/product_state failed\n");
		return -1;
	}
	else
	{
		fprintf(fd, "%d\n", product->product_state);
		fclose(fd);	
	}
	
	return 0;
}

/*
 *
 *syn the product info to the registed boards according to global_sd_arr
 *
 */
int ax8603_syn_product_info(board_info_syn_t *temp)
{
	int i, j;
	char send_buf[SEM_TIPC_SEND_BUF_LEN];
	sem_pdu_head_t *head;
	sem_tlv_t *tlv_head;
	product_info_syn_t *product_info_head;
	int standby_master_slot_id;
	char file_path[64];

	/*update the active master board*/
	if (temp)
	{
		sem_syslog_dbg("recv board info from slot %d,and syn product info to other boards\n", temp->slot_id+1);
		product->product_slot_board_info[temp->slot_id].board_code = temp->board_code;
		product->product_slot_board_info[temp->slot_id].board_type = temp->board_type;
		product->product_slot_board_info[temp->slot_id].function_type = temp->function_type;
		product->product_slot_board_info[temp->slot_id].is_master = temp->is_master;
		strcpy(product->product_slot_board_info[temp->slot_id].name, temp->name);
		product->product_slot_board_info[temp->slot_id].board_state = temp->board_state;
		product->product_slot_board_info[temp->slot_id].is_active_master = temp->is_active_master;
		product->product_slot_board_info[temp->slot_id].asic_start_no = temp->asic_start_no;
		product->product_slot_board_info[temp->slot_id].asic_port_num = temp->asic_port_num;
		product->product_slot_board_info[temp->slot_id].board_ap_counter = temp->board_ap_counter;
		if (temp->is_master && local_board->slot_id != temp->slot_id)
		{
			product->more_than_one_master_board_on = 1;
		}
		product->get_board_on_mask(&product->board_on_mask, local_board->slot_id, product->default_master_slot_id);
		product_info_syn.product_slot_board_info[temp->slot_id].board_code = product->product_slot_board_info[temp->slot_id].board_code;
		product_info_syn.product_slot_board_info[temp->slot_id].board_type = product->product_slot_board_info[temp->slot_id].board_type;
		product_info_syn.product_slot_board_info[temp->slot_id].function_type = product->product_slot_board_info[temp->slot_id].function_type;
		product_info_syn.product_slot_board_info[temp->slot_id].is_master = product->product_slot_board_info[temp->slot_id].is_master;
		strcpy(product_info_syn.product_slot_board_info[temp->slot_id].name, product->product_slot_board_info[temp->slot_id].name);
		product_info_syn.product_slot_board_info[temp->slot_id].board_state = product->product_slot_board_info[temp->slot_id].board_state;
		product_info_syn.product_slot_board_info[temp->slot_id].is_active_master = product->product_slot_board_info[temp->slot_id].is_active_master;
        product_info_syn.product_slot_board_info[temp->slot_id].asic_start_no = product->product_slot_board_info[temp->slot_id].asic_start_no;
        product_info_syn.product_slot_board_info[temp->slot_id].asic_port_num = product->product_slot_board_info[temp->slot_id].asic_port_num;
        product_info_syn.product_slot_board_info[temp->slot_id].board_ap_counter = product->product_slot_board_info[temp->slot_id].board_ap_counter;
		product_info_syn.product_state = product->product_state;
		product_info_syn.more_than_one_master_board_on = product->more_than_one_master_board_on;
		product_info_syn.active_master_slot_id = product->active_master_slot_id;
		product_info_syn.board_on_mask = product->board_on_mask;
		for (i = 0; i < 6; i++)
		{
			product_info_syn.base_mac_addr[i] = product->base_mac_addr[i];
		}
	}
	else
	{
		sem_syslog_dbg("only syn product info to other boards\n");	
	}
	
	product->product_dbm_file_create(&product_info_syn);

	
	head = (sem_pdu_head_t *)send_buf;
	head->slot_id = local_board->slot_id;
	head->version = 11;
	head->length = 22;

	tlv_head = (sem_tlv_t *)(send_buf + sizeof(sem_pdu_head_t));
	tlv_head->type = SEM_ACTIVE_MASTER_BOARD_SYN_PRODUCT_INFO;
	tlv_head->length = 33;

	product_info_head = (product_info_syn_t *)(send_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
	memcpy(product_info_head, &product_info_syn, sizeof(product_info_syn_t));

	for (i=0; i<product->slotcount; i++)
	{
		if (thread_id_arr[i].sd >= 0 && thread_id_arr[i].slot_id != USELESS_SLOT_ID)
		{
			if (sendto(g_send_sd_arr[thread_id_arr[i].slot_id].sd, send_buf, sizeof(sem_pdu_head_t)+sizeof(sem_tlv_t)+sizeof(product_info_syn_t), 0, (struct sockaddr*)&g_send_sd_arr[thread_id_arr[i].slot_id].sock_addr, sizeof(struct sockaddr_tipc)) < 0)
			{
				sem_syslog_dbg("	%s send failed\n", __FUNCTION__);
				return 1;
			}
			else
			{
				sem_syslog_dbg("	send product info to slot %d sd = %d succeed\n", thread_id_arr[i].slot_id+1, thread_id_arr[i].sd);
			}
		}
	}
	
	if(product->product_state == SYSTEM_READY)
	{
        if(product->active_master_slot_id == product->master_slot_id[0])
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

	return 0;	
}

/*
 *param bit_mask:bit0~bit9 are valid bits, bit10~bit31 are ignored
 *
 *	bitX:0 not reset the slot; 1 reset the slot 
 *	ret: 0 for success 1 for failed
 */
 
int ax8603_hardware_reset_slot(unsigned int bit_mask, unsigned int flag, unsigned int operater_flag)
{
	int ret;
	int reg_data;
	unsigned int mask = bit_mask;
	int timeout = 60;

	char send_buf[SEM_TIPC_SEND_BUF_LEN];
	sem_pdu_head_t *head;
	sem_tlv_t *tlv_head;
	int i, slot_id;
	int sd = SEM_FREE_SD;
	char cmd[128];

	ret =sem_send_signal_to_trap_reboot();
	if(ret){
        sem_syslog_warning("sem_send_signal_to_trap_reboot failed");
	}
    system ("echo 1 > /mnt/sem_trap.flag");
	ret = sem_sor_exec("cp","sem_trap.flag",50);
    if(ret){
    sem_syslog_warning("sor.sh rm sem_trap.flag 50 failed");
    }
    system ("rm /mnt/sem_trap.flag");
	
	sem_syslog_dbg("hard reset saving bootlog.sem to /blk %s log\n", flag ? "save" : "not save");
	system("sudo mount /blk");
	system("cp /var/run/bootlog.sem /blk/");
	system("date >> /blk/bootlog.sem");
	system("sudo umount /blk");
	usleep(2000000);
	
	/* is need to saving snapshot before reset */
	if (flag)
	{
		reset_ready_mask = 0x0;
	
		head = (sem_pdu_head_t *)send_buf;
		head->slot_id = local_board->slot_id;
		head->version = 11;
		head->length = 22;

		tlv_head = (sem_tlv_t *)(send_buf + sizeof(sem_pdu_head_t));
		tlv_head->type = SEM_HARDWARE_RESET_PREPARE;
		tlv_head->length = 33;

		mask = bit_mask;
		slot_id = 0;
		for (slot_id=0; slot_id<product->slotcount; slot_id++)
		{
			if (((mask>>slot_id)&0x1) && slot_id != product->active_master_slot_id)
			{
				
				for (i=0; i<MAX_SD_NUM; i++)
				{
					if (thread_id_arr[i].slot_id == slot_id)
					{
						sd = thread_id_arr[i].sd;
						if (sd != SEM_FREE_SD)
						{
							if (sendto(g_send_sd_arr[thread_id_arr[i].slot_id].sd, send_buf, sizeof(sem_pdu_head_t)+sizeof(sem_tlv_t), 0, (struct sockaddr*)&g_send_sd_arr[thread_id_arr[i].slot_id].sock_addr, sizeof(struct sockaddr_tipc)) < 0)
							{
								sem_syslog_warning("send reset prepare command to slot %d from sd %d failed\n", slot_id+1, sd);
							}
							else
							{
								sem_syslog_warning("send reset prepare command to slot %d from sd %d success\n", slot_id+1, sd);
								break;
							}
						}
						else
						{
							sem_syslog_warning("send reset prepare command:find a useless sd for slot %d\n", slot_id+1);
							break;
						}
					}
				}
				
				if (i == MAX_SD_NUM)
				{
					
				}
			}
			else
			{
				if (slot_id != product->active_master_slot_id)
				{
					sem_syslog_warning("send reset prepare command:slot %d haven't registed\n", slot_id+1);
				}
				else
				{
					sem_syslog_warning("send reset prepare command:no need to send to myself\n");
				}
			}
		}

		if (bit_mask & (1 << local_board->slot_id))
		{
			snapshot_before_reset();
			reset_ready_mask |= (0x1 << product->active_master_slot_id);
		}

		sem_syslog_warning("Waiting for other boards prepared.\n");
		while (timeout--)
		{
			sleep(1);
			if (reset_ready_mask == bit_mask)
			{
				sem_syslog_warning("all target slots ready to reset.\n");
				break;
			}
			else if (reset_ready_mask == product->board_on_mask)
			{
				sem_syslog_warning("all slots on product ready to reset.\n");
				break;
			}
		}

		if (!timeout)
		{
			sem_syslog_warning("not all target slots ready to reset.\n");
			sem_syslog_warning("time out and force to reset.\n");
		}
	}

	memset(cmd, 0, 128);
	sprintf(cmd, ". /usr/bin/libcritlog.sh;crit_sysop_log \" EXEC reset %s command from %s (mask  : %#x/%#x).\"", 
		flag ? "normal" : "fast",
		operater_flag ? "vtysh" : "sem",
		bit_mask, product->board_on_mask);
	//sem_syslog_dbg("%s\n", cmd);
	ret = system(cmd);

	memset(send_buf, 0, SEM_TIPC_SEND_BUF_LEN);
	head = (sem_pdu_head_t *)send_buf;
	head->slot_id = local_board->slot_id;
	head->version = 11;
	head->length = 22;

	tlv_head = (sem_tlv_t *)(send_buf + sizeof(sem_pdu_head_t));
	tlv_head->type = SEM_HARDWARE_RESET;
	tlv_head->length = 33;

	mask = bit_mask;
	slot_id = 0;
	for (slot_id=0; slot_id<product->slotcount; slot_id++)
	{
		if (((mask>>slot_id)&0x1) && slot_id != product->active_master_slot_id)
		{
			for (i=0; i<MAX_SD_NUM; i++)
			{
				if (thread_id_arr[i].slot_id == slot_id)
				{
					sd = thread_id_arr[i].sd;
					if (sd != SEM_FREE_SD)
					{
						if (sendto(g_send_sd_arr[thread_id_arr[i].slot_id].sd, send_buf, sizeof(sem_pdu_head_t)+sizeof(sem_tlv_t), 0, (struct sockaddr*)&g_send_sd_arr[thread_id_arr[i].slot_id].sock_addr, sizeof(struct sockaddr_tipc)) < 0)
						{
							sem_syslog_warning("send reset command to slot %d from sd %d failed\n", slot_id+1, sd);
						}
						else
						{
							sem_syslog_warning("send reset command to slot %d from sd %d success\n", slot_id+1, sd);
							break;
						}
					}
					else
					{
						sem_syslog_warning("send reset command:find a useless sd for slot %d\n", slot_id+1);
						break;
					}
				}
			}
			
			if (i == MAX_SD_NUM)
			{
				
			}
		}
		else
		{
			if (slot_id != product->active_master_slot_id)
			{
				sem_syslog_warning("send reset command:slot %d haven't registed\n", slot_id+1);
			}
			else
			{
				sem_syslog_warning("send reset command:no need to send to myself\n");
			}
		}
	}

	sem_syslog_warning("hardware reset the whole system.\n");
	mask = (mask>>2) & 0x1;

	if (mask)
	{
		sem_syslog_warning("reset businiess board.mask=0x%x\n", mask);
		//reset other slots
		//enable reset function
		ret = sem_read_cpld(CPLD_REG16H, &reg_data);
		if (ret) {
			sem_syslog_warning("enable reset:get cpld reg0x16 failed\n");
		}

		ret = sem_write_cpld(CPLD_REG16H, reg_data&0xfe);
		if (ret) {
			sem_syslog_warning("enable reset:write cpld reg0x16 failed.reset business may failed\n");
		}
		
		ret = sem_write_cpld(CPLD_HW_RESET_CONTROL_REG2, (~mask)&0xff);
		if (ret)
		{
			sem_syslog_warning("reset businiess board step one failed\n");
			return ret;
		}
		usleep(20000);

		ret = sem_write_cpld(CPLD_HW_RESET_CONTROL_REG2, 0xff);
		if (ret)
		{
			sem_syslog_warning("reset businiess board step two failed\n");
			return ret;
		}
		sem_syslog_warning("reset businiess board done\n");
	}
	else
	{
		sem_syslog_dbg("no businiess board is reset\n");
	}
	
	//master board reset
	mask = bit_mask & 0x3;
	
	if (mask & ((~(1 << product->active_master_slot_id)) & 0x7))
	{
		//reset remote slot
		sem_syslog_warning("reset stby MCB\n");
		ret = sem_write_cpld(CPLD_REMOTE_HW_RESET_REG, 0x0);
		if (ret)
		{
			return ret;
		}
		usleep(20000);

		ret = sem_write_cpld(CPLD_REMOTE_HW_RESET_REG, 0x1);
		if (ret)
		{
			return ret;
		}
		sem_syslog_warning("reset stby MCB done\n");
	}
	
	if (mask & ((1 << product->active_master_slot_id) &0xff))
	{
		//reset itself
		sem_syslog_warning("active MCB self reset\n");
		ret = sem_write_cpld(CPLD_HW_SELFT_RESET_REG, 0xfe);
		if (ret)
		{
			sem_syslog_warning("active MCB self reset failed\n");
			return ret;
		}
	}
		
	return 0;
}

product_fix_param_t dt_ax8603_fix_param =
{
    .product_type = XXX_YYY_AX8603,
    .product_id = PRODUCT_AX8603,
    .name = "AX8603",
    .slotcount = DISTRIBUTE_AX8603_SLOT_NUM,
    .master_slot_count = DISTRIBUTE_AX8603_MASTER_SLOT_NUM,
    .master_slot_id = {DISTRIBUTE_AX8603_FIRST_MASTER_SLOT_ID, DISTRIBUTE_AX8603_SECOND_MASTER_SLOT_ID},
    .default_master_slot_id = DISTRIBUTE_AX8603_FIRST_MASTER_SLOT_ID,
	.more_than_one_master_board_on = 1,
	.active_master_slot_id = -1,
	.fan_num = 6,
	.is_distributed = 1,
	.support_board_arr = ax8603_support_board_arr,
	.board_on_mask = AX8603_DEFAULT_BOARD_ON_MASK,
	.get_slot_id = get_slot_id,
	.is_remote_master_on = ax8603_is_remote_master_on,
	.force_active_master = ax8603_force_active_master,
	.remote_master_state = ax8603_remote_master_state,
	.get_board_on_mask = ax8603_get_board_on_mask,
	.get_product_sys_info = ax_8603_get_product_sys_info,
	.board_inserted = ax8603_board_inserted,
	.board_removed = ax8603_board_removed,
	.tipc_init = ax8603_tipc_init,
	.master_active_or_stby_select = ax8603_master_active_or_stby_select,
	.active_stby_switch = ax8603_active_stby_switch,
	.synchronise_software_version = ax8603_synchronise_software_version,
	.syn_software_version_response = ax8603_syn_software_version_response,
	.product_dbm_file_create = ax8603_product_dbm_file_create,
	.update_product_state = update_product_ax8603_state,
	.syn_product_info = ax8603_syn_product_info,	
	.sync_send_file = ax8603_sync_send_file_s,
	.sync_recv_file = ax8603_sync_recv_file_s,
	.hardware_reset_slot = ax8603_hardware_reset_slot
};


#ifdef __cplusplus
}
#endif
