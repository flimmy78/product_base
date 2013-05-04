#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <linux/ioctl.h>
#include <linux/tipc.h>
#include <fcntl.h>
#include <string.h>

#include "../sem_common.h"
#include "sem/sem_tipc.h"
#include "board/board_feature.h"
#include "product_feature.h"
#include "sysdef/sem_sysdef.h"
#include "sem/product.h"

#include "product_single_board.h"
#include "board/netlink.h"

extern board_fix_param_t ax81_ac12c;
extern board_fix_param_t ax81_ac8c;
extern board_fix_param_t ax81_1x12g12s;
extern board_fix_param_t ax81_2x12g12s;
extern board_fix_param_t ax81_12x;
extern board_fix_param_t ax71_1x12g12s;

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
extern int sem_read_cpld(int reg, int *read_value);
extern int sem_write_cpld(int reg, int write_value);
extern int abort_check_product_state_reboot();

char active_mcb_version[MAX_SOFTWARE_VERSION_LEN];
char active_mcb_buildno[MAX_SOFTWARE_VERSION_LEN];

board_fix_param_t *product_single_board_support_board_arr[] =
{
	[BOARD_TYPE_AX81_AC12C] = &ax81_ac12c,
	[BOARD_TYPE_AX81_AC8C]	= &ax81_ac8c,
	[BOARD_TYPE_AX81_1X12G12S] = &ax81_1x12g12s,
	[BOARD_TYPE_AX81_2X12G12S] = &ax81_2x12g12s,
	[BOARD_TYPE_AX81_12X] = &ax81_12x,
	[BOARD_TYPE_AX81_12X] = &ax71_1x12g12s,
	[BOARD_TYPE_AX81_MAX] = NULL
};

static int get_product_sys_info(product_sysinfo_t *sys_info)
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

static int get_board_on_mask(unsigned int *mask, int slot_id, int default_slot_id)
{
	*mask &= 0;
	*mask |= (1 << slot_id);

	return 0;
}

/*
 *
 *param@insert_state: value:1 on; 0 not
 */
static int is_remote_master_on(int *insert_state)
{
	*insert_state = 0;

	return 0;
}

static int force_active_master(void)
{

	return 0;
}

/*
 *
 *return value:-1 failed, 0 success
 */

static int remote_master_state(int *state)
{
	*state = 1;

	return 0;
}

/*
 *send netlink message
 *
 */
static int board_inserted(int slot_id)
{

	return 0;
}

/*
 *send netlink message
 *
 */
static int board_removed(int slot_id)
{

	return 0;
}

static int tipc_init(board_fix_param_t *board)
{
#if 0
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
	sprintf(sbuff, "tipc-config -be=eth:%s", "obc0");
	system(sbuff);

	if (board->board_type != BOARD_TYPE_AX81_SMU)
	{
		#if 1
		memset(sbuff, 0, 64);
		sprintf(sbuff, "ifconfig %s up", "obc1");
		system(sbuff);
		#else
		memset(sbuff, 0, 64);
		sprintf(sbuff, "ifconfig %s down", "obc1");
		system(sbuff);
		#endif
		memset(sbuff, 0, 64);
		sprintf(sbuff, "tipc-config -be=eth:%s", "obc1");
		system(sbuff);
	}

	memset(sbuff, 0, 64);
	sprintf(sbuff, "ip addr add %d.%d.%d.%d/24 dev obc0", 169, 254, 1, node);
	system(sbuff);

	if ((!board->is_master && board->board_type == BOARD_TYPE_AX81_AC8C) | board->board_type == BOARD_TYPE_AX81_AC12C)
	{
		sem_syslog_dbg("config slot %d ve port\n", board->slot_id+1);
		
		memset(sbuff, 0, 64);
		sprintf(sbuff, "ip addr add %d.%d.%d.%d/24 dev ve%d", 169, 254, 2, board->slot_id+1, board->slot_id+1);
		system(sbuff);
	}
#endif
	unsigned char sbuff[64] = {0};

	if (board->board_type == BOARD_TYPE_AX81_AC8C ||
		board->board_type == BOARD_TYPE_AX81_AC12C ||
		board->board_type == BOARD_TYPE_AX81_1X12G12S ||
		board->board_type == BOARD_TYPE_AX71_1X12G12S)
	{
		memset(sbuff, 0, 64);
		sprintf(sbuff, "ifconfig ve%d up", board->slot_id + 1);
		sem_syslog_dbg("%s\n", sbuff);
		system(sbuff);
		system("echo 1 > /sys/module/e1000e/parameters/e1000e_rx_en");/*enable e1000e rx packet.*/
	}
	
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
		return 1;
	}

	//sem_syslog_dbg("buf len %d\n", strlen(buf));
	fwrite(buf, sizeof(char), version_byte_count, fp);
	fclose(fp);

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
static int sync_send_file_s(int slot_id, char *filename, int syn_to_blk)
{

	return 0;
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
static int sync_recv_file_s(char *recvbuf, int iflag, int slot_id)
{

	return 0;
}

/*
 *function:It is used for boards except active master board to synchronise current board software version with active master board
 *sem may block here
 *
 *
 */
static int synchronise_software_version(int sd, board_fix_param_t *board)
{
	
	return 0;
}

/* 
 * Evade the tipc connecttion problem after mcb active standby switch temporary
 * New active mcb can request other board to register during product_ready_test
 *
 * caojia@autelan.com, 2012-1-14
 */
static int request_board_register(int slot_id)
{
	
	return 0;
}

static void product_ready_test()
{
	
	return ;
}

extern pthread_t stby_keep_alive_thread;
extern pthread_t slave_server_thread;

extern pthread_t master_server_thread;
extern pthread_t master_keep_alive_test_thread;

static int master_active_or_stby_select(int *sd, board_fix_param_t *board)
{
	return 0;
}

/*
  *return 0 for success 1 for failed
  *
  */
static int active_stby_switch(void *arg)
{
	return 0;
}

/*
 *
 *return :1 need , 0 not need, -1 for failed
 *
 */
static int is_need_to_syn_software_version(char *version, char *buildno)
{
	return 0;
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

static int syn_software_version_response(int sd, sem_tlv_t *temp_tlv_head, board_fix_param_t *board, int slot_id)
{
	return 0;
}

static int product_dbm_file_create(product_info_syn_t *product)
{
	FILE *fd;
	int i;
	char temp_buf[100];
	char temp_buf2[100];

	system("mkdir -p /dbm/product/");

	//system("touch /dbm/product/name");
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
	}
	return 0;
}

/*
 *Update product board_on_mask and more_than_on_master_board_on files
 *
 */
static int update_product_state(void)
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
static int syn_product_info(board_info_syn_t *temp)
{
	int i, j;

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

	return 0;	
}

/*
 *param bit_mask:bit0~bit9 are valid bits, bit10~bit31 are ignored
 *
 *	bitX:0 not reset the slot; 1 reset the slot 
 *	ret: 0 for success 1 for failed
 */
 
static int hardware_reset_slot(unsigned int bit_mask, unsigned int flag, unsigned int operater_flag)
{
	int ret = 0;
	unsigned int mask = bit_mask;

	sem_syslog_dbg("hard reset saving bootlog.sem to /blk %s log\n", flag ? "save" : "not save");
	system("sudo mount /blk");
	system("cp /var/run/bootlog.sem /blk/");
	system("date >> /blk/bootlog.sem");
	system("sudo umount /blk");
	usleep(2000000);

	if (mask & (1 << local_board->slot_id)) {
		ret = local_board->hard_self_reset();
	}
		
	return ret;
}

int product_single_board_ready_state(void)
{
	int slot;

	for (slot = 0; slot < product->slotcount; slot++) {
		if (slot == local_board->slot_id) {
			continue;
		}
		product_info_syn.product_slot_board_info[slot].board_type = BOARD_EMPTY;
		board_info_to_syn.board_type = BOARD_EMPTY;
	}

	local_board->function_type |= MASTER_BOARD | AC_BOARD | SWITCH_BOARD;
	board_info_to_syn.function_type |= MASTER_BOARD | AC_BOARD | SWITCH_BOARD;
	local_board->is_master = 1;
	local_board->is_active_master = ACTIVE_MASTER;
	board_info_to_syn.is_master = local_board->is_master;
	board_info_to_syn.is_active_master = local_board->is_active_master;
	local_board->is_use_default_master = 1;
	board_info_to_syn.is_use_default_master = 1;
	local_board->board_state = BOARD_READY;
	board_info_to_syn.board_state = local_board->board_state;
	local_board->local_board_dbm_file_create(local_board);

	product->active_master_slot_id = local_board->slot_id;
	product_info_syn.active_master_slot_id = product->active_master_slot_id;
	product->default_master_slot_id = local_board->slot_id;
	product_info_syn.default_master_slot_id = product->default_master_slot_id;
	product->master_slot_id[0] = local_board->slot_id;
	product->master_slot_id[1] = local_board->slot_id;
	product_info_syn.master_slot_id[0] = product->master_slot_id[0];
	product_info_syn.master_slot_id[1] = product->master_slot_id[1];
	product->get_board_on_mask(&product->board_on_mask, local_board->slot_id, product->default_master_slot_id);
	product_info_syn.board_on_mask = product->board_on_mask;

	product->product_state = SYSTEM_READY;
	product_info_syn.product_state = SYSTEM_READY;
	product->syn_product_info(&board_info_to_syn);
	product->update_product_state();
	product_state_notifier(SYSTEM_READY);
	sem_syslog_dbg("SEM_MAIN:waiting for system running...\n");
	while (local_board->is_active_master)
	{
		int fd;
		char c;
		int i =0;
		char msgbuf[512] = {0};
		int msgLen = 0;
	
		
		fd = open("/dbm/is_loadconfig_done", O_RDONLY | O_CREAT, 0644);
		if(fd < 0)
		{
			sleep(1);
			continue;
		}
		read(fd, &c, 1);
		if(c == '1')
		{
			product->product_state = SYSTEM_RUNNING;
			product->syn_product_info(&board_info_to_syn);
			product->update_product_state();
			product_state_notifier(SYSTEM_RUNNING);
	
			sleep(1);//this sleep is important for ensure that sem send the dbus signal can be received by trap.
			/*sem_send_signal_to_trap();
			sem_dbus_system_state_trap(SYSTEM_RUNNING);*/
			close(fd);
			system("echo 0 > /sys/module/octeon_ethernet/parameters/pend_cvm");
			sem_syslog_warning("System Running OK!\n");
			break;
		}
		else
		{
			sem_syslog_dbg("Read /dbm/is_loadconfig_done is not 1.\n"); 				
			close(fd);
			sleep(1);
			continue;
		}						
	}
	return 0;
}

product_fix_param_t dt_single_board_fix_param =
{
	.product_type = XXX_YYY_SINGLE_BOARD,
	.product_id = PRODUCT_SINGLE_BOARD,
	.name = "Product-SINGLE_BOARD",
	.slotcount = 1,
	.master_slot_count = 1,
	.master_slot_id = {-1, -1},
	.default_master_slot_id = -1,
	.more_than_one_master_board_on = 0,
	.active_master_slot_id = -1,
	.fan_num = 0,
	.is_distributed = 1,
	.support_board_arr = product_single_board_support_board_arr,
	.board_on_mask = 0x0,
	.get_slot_id = get_slot_id,
	.is_remote_master_on = is_remote_master_on,
	.force_active_master = force_active_master,
	.remote_master_state = remote_master_state,
	.get_board_on_mask = get_board_on_mask,
	.get_product_sys_info = get_product_sys_info,
	.board_inserted = board_inserted,
	.board_removed = board_removed,
	.tipc_init = tipc_init,
	.master_active_or_stby_select = master_active_or_stby_select,
	.active_stby_switch = active_stby_switch,
	.synchronise_software_version = synchronise_software_version,
	.syn_software_version_response = syn_software_version_response,
	.product_dbm_file_create = product_dbm_file_create,
	.update_product_state = update_product_state,
	.syn_product_info = syn_product_info,	
	.sync_send_file = sync_send_file_s,
	.sync_recv_file = sync_recv_file_s,
	.hardware_reset_slot = hardware_reset_slot
};

#ifdef __cplusplus
}
#endif
