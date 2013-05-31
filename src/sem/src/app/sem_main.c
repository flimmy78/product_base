#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>


#if 1
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/param.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <errno.h>
#include <sys/mman.h> 
#include <unistd.h>
#include <linux/tipc.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include <pthread.h>
#include <linux/if.h>//huangjing
#endif

#include "sem_dbus.h"
#include "sem_common.h"
#include "sem_fpga.h"
#include "sem/sem_tipc.h"
#include "product/board/board_feature.h"
#include "product/board/ax81_1x12g12s.h"
#include "product/product_feature.h"
#include "product/product_ax8610.h"
#include "product/product_ax7605i.h"
#include "sem/product.h"
#include "sysdef/sem_sysdef.h"
#include "board/board_common.h"
#include "board/netlink.h"
#include "sem_eth_port.h"
#include "sem_eth_port_dbus.h"
#include "sem_log.h"

extern int product_init(void);
extern int product_single_board_ready_state(void);
extern int module_init(void);
extern int sem_read_cpld(int reg, int *read_value);
extern int sem_write_cpld(int reg, int write_value);
extern int open_fd();
extern void close_fd();
extern int tipc_init();
extern bool sem_thread_create(pthread_t *new_thread, void * (*thread_fun)(void *), void *arg, int is_detached);
extern int active_link_test(int test_count);
extern int non_active_master_board_keep_alive_test_client(void *arg);
extern int hardware_reset_prepare(int sd, int slot_id);
extern int sem_set_boot_img(char * img_name);
extern int sem_startup_end;

extern car_linklist_node_t *creat_car_linklist(void);
extern car_linklist_node_t *car_head;
extern unsigned long long car_count;
extern car_white_list_node_t *creat_car_white_list(void);
extern car_white_list_node_t *car_white_head;
extern unsigned long long car_white_count;
extern wan_if_linklist_node_t *creat_wan_if_linklist(void);
extern wan_if_linklist_node_t *wan_if_head;
extern unsigned long long wan_if_count;

#define SLOT_ID_MASK	0xf

extern unsigned long long set_system_img_done_mask;
extern int set_system_img_board_mask;

extern board_fix_param_t *local_board;
extern product_fix_param_t *product;
extern board_info_syn_t board_info_to_syn;
extern product_info_syn_t product_info_syn;
extern int update_mac_enable;
extern int update_sn_enable;
extern struct global_ethport_s **global_ethport;
extern struct global_ethport_s *start_fp[MAX_SLOT_COUNT];
extern void poll_QT2025_link_status();
extern void ax81_1x12g12s_download_fpga_QT();

pthread_t stby_keep_alive_thread;
pthread_t slave_server_thread;

pthread_t master_server_thread;
pthread_t master_keep_alive_test_thread;

pthread_t slave_keep_alive_thread;
pthread_t sem_dbus_thread;
pthread_t netlink_recv_thread;
pthread_t sem_compatible_thread;
pthread_t QT2025_link_thread;
pthread_t FPGA_keep_alive_thread;
pthread_t slave_cpu_port_check;

int sem_compatible_send_flag[MAX_SLOT_COUNT];

pthread_mutex_t switch_lock = PTHREAD_MUTEX_INITIALIZER;
int switch_flag = 0;
unsigned int reset_ready_mask = 0x0;
int active_server_sd = SEM_FREE_SD;
int active_mcb_server_flag = 0;

int reinit = 1;

int active_mcb_state = IS_DEAD;
int disable_keep_alive_time = 0;
int disable_keep_alive_flag_time = 0;
long disable_keep_alive_start_time = 0;

int exit_flag = 0;
int start = 0;
int is_timeout = 3;

int dbm_effective_flag = 0;

//this three variable is defined for non-active MCB to send and recv tipc message
int g_recv_sd = SEM_FREE_SD;
struct sockaddr_tipc g_recv_sock_addr;

int g_send_sd = SEM_FREE_SD;
struct sockaddr_tipc g_send_sock_addr;

//this two variable array is defined for active MCB to send and recv tipc message

//for reciving tipc message
sd_sockaddr_tipc_t g_recv_sd_arr[MAX_SLOT_NUM];

//for sending tipc message
sd_sockaddr_tipc_t g_send_sd_arr[MAX_SLOT_NUM];


thread_index_sd_t thread_id_arr[MAX_SLOT_NUM];
version_sync_t version_sync_arr[MAX_SLOT_NUM];

int temp_arr[MAX_SLOT_COUNT] = {0};

int sem_tipc_send(unsigned int slot_id, int type, char*msgBuf, int len)
{
	sem_pdu_head_t *head;
	sem_tlv_t *tlv_head;
	int sd = -1, i;
	char send_buf[SEM_TIPC_SEND_BUF_LEN] = {0};
	char *chTmp = NULL;
	struct sockaddr_tipc sock_addr;
	
	if(local_board->is_active_master)
	{
		if (thread_id_arr[slot_id].sd >= 0 && slot_id != local_board->slot_id)
		{
			//sem_syslog_dbg("\tthread_id_arr[%d].slot_id = %d, sd = %d\n", 
			//	i, thread_id_arr[i].slot_id, thread_id_arr[i].sd);
			sd = g_send_sd_arr[slot_id].sd;
			sock_addr = g_send_sd_arr[slot_id].sock_addr;
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
	  
	head = (sem_pdu_head_t *)send_buf;
	head->slot_id = local_board->slot_id;
	head->version = 11;
	head->length = 22;

	tlv_head = (sem_tlv_t *)(send_buf + sizeof(sem_pdu_head_t));
	tlv_head->type = type;
	tlv_head->length = 33;
	
    chTmp = send_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t);
	memcpy(chTmp, msgBuf, len);
	
	if (sendto(sd, send_buf, sizeof(sem_pdu_head_t)+sizeof(sem_tlv_t)+len, 0, (struct sockaddr*)&sock_addr, sizeof(struct sockaddr_tipc)) < 0)
	{
		sem_syslog_dbg("Send to slot(%d) tipc tlv_type(%d) info failed\n", slot_id+1, type);
		return 1;
	}
	else
	{
		sem_syslog_dbg("\tSend to slot(%d) tipc tlv_type(%d) info succeed\n", slot_id+1, type);
	}

	return 0;
}

#define KEEP_ALIVE_TEST_FREQUENCY	1
#define NON_ACTIVE_TEST_ACTIVE_TIMEOUT	60
#define ACTIVE_TEST_NON_ACTIVE_TIEMOUT	40
/*
 *function:It is used for active master board to detect slave board and standby master board is alive or not
 *
 *
 **/
void active_master_board_keep_alive_thread(void)
{
	int i = 0;
	static int counter = 0;

	int flag[MAX_SLOT_NUM];
	
	char send_buf[SEM_TIPC_SEND_BUF_LEN];
	sem_pdu_head_t *head;
	sem_tlv_t *tlv_head;
	int print_flag[MAX_SLOT_NUM] = {0};
	int test_packet_count_per_second = KEEP_ALIVE_TEST_FREQUENCY;

	struct timeval tv;
	struct timezone tz;
	long current_time_sec = 0;
	int timeout[MAX_SLOT_NUM];
	char str[256];
	
	sem_syslog_warning("thread %s, thread id is %d\n", __FUNCTION__, getpid());

	for (i=0; i<product->slotcount; i++)
	{
		print_flag[i] = 1;
		flag[i] = 1;
		timeout[i] = ACTIVE_TEST_NON_ACTIVE_TIEMOUT;
	}
	
	while (1)
	{
#if 0
		for (i=0; i<product->slotcount; i++)
		{
			if (thread_id_arr[i].sd >= 0 && 
					thread_id_arr[i].slot_id != local_board->slot_id &&
					thread_id_arr[i].slot_id != USELESS_SLOT_ID)
			{
				if ((counter % 60) == 0)
					sem_syslog_warning("	test slot %d sd is %d\n", thread_id_arr[i].slot_id+1, thread_id_arr[i].sd);
				
				head = (sem_pdu_head_t *)send_buf;
				head->slot_id = local_board->slot_id;

				tlv_head = (sem_tlv_t *)(send_buf + sizeof(sem_pdu_head_t));
				tlv_head->type = SEM_CMD_PDU_STBY_ACTIVE_LINK_TEST_REQUEST;

				if (sendto(g_send_sd_arr[thread_id_arr[i].slot_id].sd, send_buf, sizeof(sem_pdu_head_t)+sizeof(sem_tlv_t), 0, (struct sockaddr*)&g_send_sd_arr[thread_id_arr[i].slot_id].sock_addr, sizeof(struct sockaddr_tipc)) < 0)
				{
					sem_syslog_warning("	send keep alive test packet to slot %d failed\n", thread_id_arr[i].slot_id+1);
				}
				interval[i] = interval[i] - 1;
			}
		}
		#else
		while (test_packet_count_per_second--)
		{
			for (i=0; i<product->slotcount; i++)
			{
				if (version_sync_arr[i].slot_id != USELESS_SLOT_ID)
				{
					if (version_sync_arr[i].version_sync_state == SYNC_DONE || 
						version_sync_arr[i].version_sync_state == IS_SYNCING)
					{
						if (version_sync_arr[i].version_sync_timeout >= 0)
						{
							version_sync_arr[i].version_sync_timeout--;
						}
						else
						{
							version_sync_arr[i].slot_id = USELESS_SLOT_ID;
							sem_syslog_warning("active MCB reset slot %d for sync version file timeout\n", i+1);
							product->hardware_reset_slot(1<<i, 0, RESET_CMD_FROM_SEM);
						}
					}
					else
					{
						version_sync_arr[i].slot_id = USELESS_SLOT_ID;
						sem_syslog_warning("active MCB reset slot %d for sync version file failed\n", i+1);
						product->hardware_reset_slot(1<<i, 0, RESET_CMD_FROM_SEM);
					}
							
				}
				
				if (thread_id_arr[i].sd >= 0 && 
						thread_id_arr[i].slot_id != local_board->slot_id)
				{
					//if ((counter % 600) == 0)
						//sem_syslog_warning("	slot %d sd is %d\n", thread_id_arr[i].slot_id+1, thread_id_arr[i].sd);
					
					head = (sem_pdu_head_t *)send_buf;
					head->slot_id = local_board->slot_id;

					tlv_head = (sem_tlv_t *)(send_buf + sizeof(sem_pdu_head_t));
					tlv_head->type = SEM_CMD_PDU_STBY_ACTIVE_LINK_TEST_REQUEST;

					if (thread_id_arr[i].slot_id != USELESS_SLOT_ID)
					{
						if (sendto(g_send_sd_arr[thread_id_arr[i].slot_id].sd, send_buf, sizeof(sem_pdu_head_t)+sizeof(sem_tlv_t), 0, (struct sockaddr*)&g_send_sd_arr[thread_id_arr[i].slot_id].sock_addr, sizeof(struct sockaddr_tipc)) < 0)
						{
							sem_syslog_warning("	send keep alive test packet to slot %d failed\n", thread_id_arr[i].slot_id+1);
						}
					}
					else
					{
						if (sendto(g_send_sd_arr[i].sd, send_buf, sizeof(sem_pdu_head_t)+sizeof(sem_tlv_t), 0, (struct sockaddr*)&g_send_sd_arr[i].sock_addr, sizeof(struct sockaddr_tipc)) < 0)
						{
							sem_syslog_dbg("	send keep alive test packet to slot %d failed sd %d\n", i + 1, g_send_sd_arr[i].sd);
							sem_syslog_dbg("	sendto error : %s\n", strerror(errno));
						}
					}
				}
			}
		}
		
		if (test_packet_count_per_second<=0)
		{
			test_packet_count_per_second = KEEP_ALIVE_TEST_FREQUENCY;	
		}
#endif
		counter++;
		
		sleep(1);

		#if 0
		//send keep alive packet every 1 seconds,check board state every 10 seconds
		if (!flag)
		{
			flag = 10;
		}
		else
		{
			flag--;
			continue;
		}
		#endif

		if (disable_keep_alive_time > 0)
		{
			while (disable_keep_alive_time > 0)
			{
				sleep(1);
				disable_keep_alive_time--;
				gettimeofday(&tv, &tz);
				current_time_sec = tv.tv_sec;
				if ((current_time_sec - disable_keep_alive_start_time - 5) > \
					(disable_keep_alive_flag_time - disable_keep_alive_time))
				{
					disable_keep_alive_time = disable_keep_alive_flag_time - (current_time_sec - disable_keep_alive_start_time);
				}
			}
			
			continue;
		}
		
		for (i=0; i<product->slotcount; i++)
		{
			if (thread_id_arr[i].slot_id != USELESS_SLOT_ID)
			{
				if (thread_id_arr[i].is_aliving)
				{
					flag[i] = 1;
					timeout[i] = ACTIVE_TEST_NON_ACTIVE_TIEMOUT;
					//if ((counter % 60) == 0)
							//sem_syslog_warning("	test:slot %d is aliving\n", thread_id_arr[i].slot_id+1);
				}
				else
				{
					if (timeout[i] == 20)
					{
						sem_syslog_warning("active test no non-active 20s, start ping.\n");
						sprintf(str, "ping 169.254.1.%d -w 18 >> /var/run/bootlog.sem &", thread_id_arr[i].slot_id+1);
						system(str);
					}
					if (flag[i] == 1 && timeout[i] <= 10)
					{
						flag[i] = 0;
						sem_syslog_warning("active MCB test non-active MCB is aliving count down\n");
					}

					timeout[i]--;
					//sem_syslog_warning("timeout[%d]=%d\n", i, timeout[i]);
					if (timeout[i] <= 9)
						sem_syslog_warning("slot %d not detected:%d\n", i+1, timeout[i]);			
					
					if (timeout[i] != 0)
					{
						continue;
					}

					sem_compatible_send_flag[i] = 0;
					sem_syslog_warning("	slot %d is dead\n", thread_id_arr[i].slot_id+1);
					if (product->product_type == XXX_YYY_AX7605I)
	    				{
			                        product->hardware_reset_slot(0x7, 1, RESET_CMD_FROM_SEM);
	    				}
					thread_id_arr[i].slot_id = USELESS_SLOT_ID;
					set_dbm_effective_flag(INVALID_DBM_FLAG);
					product->board_removed(i);
					set_dbm_effective_flag(VALID_DBM_FLAG);
				}			
				thread_id_arr[i].is_aliving = IS_DEAD;
			}
		}
	}

	sem_syslog_dbg("^^^^^^^^^^^^^^^^^^^^^^^^^^error^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
}


void sem_compatible_test_thread(void) 
{
	int exit_flag = 0;
	int i;
	sem_tlv_t *tlv_head;
	sem_pdu_head_t *head;
	char send_buf[SEM_TIPC_SEND_BUF_LEN] = {0};

	sem_syslog_warning("thread %s, thread id is %d\n", __FUNCTION__, getpid());
	
	head = (sem_pdu_head_t *)send_buf;
	head->slot_id = local_board->slot_id;
	head->version = 11;
	head->length = 22;

	tlv_head = (sem_tlv_t *)(send_buf + sizeof(sem_pdu_head_t));
	tlv_head->type = SEM_COMPATIBLE_SYNC_PRODUCT_INFO;
	tlv_head->length = 33;
	
	while (!exit_flag) {
		for (i=0; i<product->slotcount; i++) {
			if (i != local_board->slot_id && sem_compatible_send_flag[i] == 0 && thread_id_arr[i].slot_id != USELESS_SLOT_ID &&
				(product->board_on_mask>>i&0x1)) {
				if (sendto(g_send_sd_arr[i].sd, send_buf, sizeof(sem_pdu_head_t)+sizeof(sem_link_test_t), 0, 
					(struct sockaddr*)&g_send_sd_arr[i].sock_addr, sizeof(struct sockaddr_tipc)) < 0)
				{
					sem_syslog_dbg("	send active MCB board message to slot %d failed\n", i+1);
				}
				else
				{
					sem_syslog_dbg("	send active MCB board message to slot %d succeed\n", i+1);
				}
			}
		}
		for (i=0; i<product->slotcount; i++) {
			if (sem_compatible_send_flag[i] == 0 && i != local_board->slot_id) {
				sleep(1);
				continue;
			}
		}
		//exit_flag = 1;
		sleep(1);
	}
}
int fan_autoregulative  (int temp, int slot_id)
{
	int i;
	int max_temp = 0;
	int temp_local;
	int regData;
	
	temp_arr[slot_id] = temp;
	
	get_temerature_info(&temp_local);	
	temp_arr[local_board->slot_id] = temp_local;
	
	for(i = 0; i < MAX_SLOT_COUNT; i++){
		if (temp_arr[i] > max_temp){
			max_temp = temp_arr[i];
		}
	}

	max_temp = max_temp/1000;

	/*enable fan speed control*/
	usleep(1);
	if (sem_read_cpld(CPLD_REG16H,&regData)){
		return -1;
	}
	
	if (product->product_type == XXX_YYY_AX8603){

		/* fan speed control enable on bit 2@CPLD_REG16H */
		regData &= 0xfb;
		
	} else {
		
		/* fan speed control enable on bit 0@CPLD_REG16H */
		regData &= 0xfe;
		
	}
	
	usleep(1);
	if(sem_write_cpld(CPLD_REG16H, regData) < 0){
		return -1;
	}
#if 0
	if (max_temp <= 20) 
	{	/* 12.5% speed of fan */
		usleep(1);
		if(sem_write_cpld(CPLD_FAN_RPM, 0x01) < 0){			
			sem_syslog_dbg("write cpld error\n");
			return -1;
		}
	}
#endif 	
	else if (max_temp <= 28 ) 
	{	/* 37.5% speed of fan */
		usleep(1);
		if(sem_write_cpld(CPLD_FAN_RPM, 0x04) < 0){
			sem_syslog_dbg("write cpld error\n");
			return -1;
		}
	} 
	else if (max_temp >= 32 && max_temp <= 48) 
	{	/* 50% speed of fan */
		usleep(1);
		if(sem_write_cpld(CPLD_FAN_RPM, 0x08) < 0){
			sem_syslog_dbg("write cpld error\n");
			return -1;
		}	
	}
#if 0	
	else if (max_temp <= 60) 
	{	/* 62.5% speed of fan */
		usleep(1);
		if(sem_write_cpld(CPLD_FAN_RPM, 0x10) < 0){
			sem_syslog_dbg("write cpld error\n");
			return -1;
		}	
	}
#endif	
	else if (max_temp >= 52 && max_temp <= 69) 
	{	/* 87.5% speed of fan */
		usleep(1);
		if(sem_write_cpld(CPLD_FAN_RPM, 0x40) < 0){
			sem_syslog_dbg("write cpld error\n");			
			return -1;
		}	
	} 
	else if (max_temp >= 71 ) 
	{
		/* 100% speed of fan */
		usleep(1);
		if(sem_write_cpld(CPLD_FAN_RPM, 0x80) < 0){
			sem_syslog_dbg("write cpld error\n");			
			return -1;
		}	
	}

	return 0;
}


void sem_thread_recv(void *arg)
{
	sd_sockaddr_tipc_t *temp_sd_sockaddr;
	int i;
	int fd;
	char c;
	sem_pdu_head_t *head;
	sem_pdu_head_t *head1;
	sem_tlv_t *tlv_head;
	sem_tlv_t *tlv_head1;
	nl_msg_head_t *nl_head;
	sync_file_head_t *pfilehead;
	board_info_syn_t *temp_board_info_syn;
	global_ethport_t *ethport_info;
	fpga_abnormal_tipc_t *fpga_abnormal_tipc;
	unsigned int eth_g_index = 0;
	int ret ;
	char recv_buf[SEM_TIPC_RECV_BUF_LEN];
	char send_buf[SEM_TIPC_SEND_BUF_LEN] = {0};
	char file_path[64];
	int recv_len = SEM_TIPC_RECV_BUF_LEN;
	int addr_len = sizeof(struct sockaddr_tipc);
	struct sockaddr_tipc t_addr;
	sem_tmperature_info *temp_info;
    /**********wangchao add*************/
    char *link_status_str[2] = {
    	"DOWN",
    	"UP"
    };
	
	sem_syslog_warning("thread %s, thread id is %d\n", __FUNCTION__, getpid());
	if (!arg)
	{
		sem_syslog_warning("	arg is null\n");
		return;
	}
	
	temp_sd_sockaddr = (sd_sockaddr_tipc_t *)arg;
	sem_syslog_dbg("sem_thread_recv created sd=%d\n", temp_sd_sockaddr->sd);

	while(1)
	{
		int recv_len = SEM_TIPC_RECV_BUF_LEN;
        ret = recvfrom(temp_sd_sockaddr->sd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&t_addr, &addr_len);
		if (ret <= 0)
		{
			sem_syslog_warning("sem_thread_recv must exit\n");
			continue;
		}
		else
		{
			head = (sem_pdu_head_t *)recv_buf;
			tlv_head = (sem_tlv_t *)(recv_buf + sizeof(sem_pdu_head_t));
			pfilehead = (sync_file_head_t*)(recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
			if(tlv_head->type == SEM_FILE_SYNING ||tlv_head->type == SEM_FILE_SYN_FINISH)
            {
				sem_syslog_dbg("\tfrom slot %d recv_len %d\n",head->slot_id+1,ret);
            }
			int standby_master_slot_id;
			
        	if(product->active_master_slot_id == product->master_slot_id[0])
        		standby_master_slot_id = product->master_slot_id[1];
        	else
        		standby_master_slot_id = product->master_slot_id[0];

#if 0
			if (SEM_CMD_PDU_STBY_ACTIVE_LINK_TEST_REQUEST != tlv_head->type)
				sem_syslog_dbg("recv from slot %d from socket %d\n", head->slot_id+1, temp_sd_sockaddr->sd);
#endif
			if (head->slot_id < 0 || head->slot_id >= product->slotcount)
			{
				sem_syslog_warning("packet slot id is wrong:slot id = %d\n", head->slot_id);
				continue;
			}

			thread_id_arr[head->slot_id].is_aliving = IS_ALIVING;
			
			switch (tlv_head->type)
			{
				case SEM_COMPATIBLE_SYNC_PRODUCT_INFO:
					sem_syslog_dbg("sem_thread_recv:SEM_COMPATIBLE_SYNC_PRODUCT_INFO from slot %d\n", 
						head->slot_id+1);
					break;
				case SEM_COMPATIBLE_BOARD_REGIST:
					sem_compatible_send_flag[head->slot_id] = 1;
					sem_syslog_warning("sem_thread_recv:SEM_COMPATIBLE_BOARD_REGIST from slot %d\n", head->slot_id+1);
					temp_board_info_syn = (board_info_syn_t *)(recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
					/*this case will break at next one*/
				case SEM_NON_ACTIVE_BOARD_REGISTER:
					sem_syslog_dbg("sem_thread_recv:SEM_NON_ACTIVE_BOARD_REGISTER from slot %d\n", head->slot_id+1);
					int notify_flag = 0;
					if (product->product_state >= SYSTEM_READY) {						
						set_dbm_effective_flag(INVALID_DBM_FLAG);
						notify_flag = 1;
					}

					if (head->slot_id < product->slotcount && head->slot_id >= 0)
					{
						thread_id_arr[head->slot_id].slot_id = head->slot_id;
						sem_syslog_dbg("slot %d sd is %d\n", head->slot_id+1, temp_sd_sockaddr->sd);
					}
					else
					{
						sem_syslog_dbg("wrong slot id %d\n", head->slot_id+1);
					}

					
					
					temp_board_info_syn = (board_info_syn_t *)(recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));

					//test is the same board is removed and then hot inserted
					if (product->product_slot_board_info[head->slot_id].board_type == temp_board_info_syn->board_type
						&& product->product_slot_board_info[head->slot_id].board_state == BOARD_INSERTED_AND_REMOVED)
					{
						sem_syslog_warning("the same type board is reinserted, then the whole system must reset\n");
						sem_syslog_warning("saving log, please wait for few seconds...\n");
						product->hardware_reset_slot(0x3ff, 1, RESET_CMD_FROM_SEM);
						while (1)
						{
							usleep(1000);	
						}
					}
					else if ((product->product_slot_board_info[head->slot_id].board_type != -1) &&
						(product->product_slot_board_info[head->slot_id].board_state == BOARD_INSERTED_AND_REMOVED))
					{
						sem_syslog_warning("different kind board is inserted.\n");
						sem_syslog_warning("saving log, please wait for few seconds...\n");
						product->hardware_reset_slot(0x3ff, 1, RESET_CMD_FROM_SEM);
						while (1)
						{
							usleep(1000);	
						}
					}
				   else if((product->product_slot_board_info[head->slot_id].board_type == -1) &&  
						(product->product_slot_board_info[head->slot_id].board_state == BOARD_INSERTED_AND_REMOVED ))
					{

						 sem_syslog_warning("synchronise software version over,the board must be reset \n");
						 sem_syslog_warning("saving log, please wait for few seconds...\n");
						 product->hardware_reset_slot(0x3ff, 1,RESET_CMD_FROM_SEM);
						 while (1)
						 {
						    usleep(1000);	
						 }                      
					}					
					else
					{
						sem_syslog_dbg("insert a board\n");
					}
					
					
					if (product->product_state >= SYSTEM_READY)
					{
						if (temp_board_info_syn->board_state <= BOARD_INITIALIZING)
						{
							char file_name[64];
							sem_syslog_dbg("slot %d board is hot inserted\n", head->slot_id+1);
						}
						else
						{
							sem_syslog_dbg("slot %d board is re-inserted\n", head->slot_id+1);
						}
					}
					else
					{
						sem_syslog_dbg("slot %d board is cold inserted\n", head->slot_id+1);
					}
					
					product->syn_product_info(temp_board_info_syn);   /* avoid the board-insert at system-ready */					
					if (notify_flag) {
						set_dbm_effective_flag(VALID_DBM_FLAG);
						product->board_inserted(head->slot_id);
					}
					sem_compatible_send_flag[head->slot_id] = 1;
					break;
					
				case SEM_CMD_PDU_STBY_ACTIVE_LINK_TEST_REQUEST:
					/*wangchao add*/
					sem_syslog_event("recv keep alive packet from slot %d\n", head->slot_id+1);
					temp_info = (sem_tmperature_info*)(recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));	
										
					if (product->product_type == XXX_YYY_AX8610 || \
						product->product_type == XXX_YYY_AX8603 || \
						product->product_type == XXX_YYY_AX8800 || \
						product->product_type == XXX_YYY_AX8606)
					{
						fan_autoregulative (temp_info->core_temp, head->slot_id);
					}
					break;
				case SEM_CONNECT_REQUEST:
					sem_syslog_warning("sem_thread_recv:SEM_CONNECT_REQUEST from slot %d\n", head->slot_id+1);
					head1 = (sem_pdu_head_t *)send_buf;
					head1->slot_id = local_board->slot_id;
					head1->version = 11;
					head1->length = 22;

					tlv_head1 = (sem_tlv_t *)(send_buf + sizeof(sem_pdu_head_t));
					tlv_head1->type = SEM_CONNECT_CONFIRM;
					tlv_head1->length = 33;
											
					if (sendto(g_send_sd_arr[head->slot_id].sd, send_buf, sizeof(sem_pdu_head_t)+sizeof(sem_link_test_t), 0, (struct sockaddr*)&g_send_sd_arr[head->slot_id].sock_addr, sizeof(struct sockaddr_tipc)) < 0)
					{
						sem_syslog_dbg("	link confirm:send failed\n");
					}
					else
					{
						sem_syslog_dbg("	link confirm:send succeed\n");
					}
					break;
					
				case SEM_SOFTWARE_VERSION_SYN_REQUEST:
					sem_syslog_warning("sem_thread_recv:SEM_SOFTWARE_VERSION_SYN_REQUEST from slot %d\n", head->slot_id+1);
					if (head->slot_id >= 0 && head->slot_id < product->slotcount)
					{
						//thread_id_arr[head->slot_id].slot_id = head->slot_id;
					}
					else
					{
						sem_syslog_warning("wrong slot id %d\n", head->slot_id);
						break;
					}
					version_sync_arr[head->slot_id].slot_id = head->slot_id;
					version_sync_arr[head->slot_id].version_sync_state = IS_SYNCING;
					version_sync_arr[head->slot_id].version_sync_timeout = SOFTWARE_SYNC_TIMEOUT;
					
					ret = product->syn_software_version_response(temp_sd_sockaddr->sd, tlv_head, local_board, head->slot_id);
					if (ret != SOFTWARE_SYN_NO_NEED)
					{
						if (ret == SOFTWARE_SYN_SUCCESS)
						{
							version_sync_arr[head->slot_id].version_sync_state = SYNC_DONE;
							version_sync_arr[head->slot_id].version_sync_timeout = SOFTWARE_SYNC_TIMEOUT;
							sem_syslog_warning("syn version file to slot %d success\n", head->slot_id+1);
						}
						else
						{
							version_sync_arr[head->slot_id].version_sync_state = SYNC_FAILED;
							version_sync_arr[head->slot_id].version_sync_timeout = 0;
							sem_syslog_warning("syn version file to slot %d failed\n", head->slot_id+1);
						}
					}
					else
					{
						version_sync_arr[head->slot_id].slot_id = USELESS_SLOT_ID;
						version_sync_arr[head->slot_id].version_sync_state = SYNC_NO_NEED;
						version_sync_arr[head->slot_id].version_sync_timeout = 0;
					}
					break;
				case SEM_SOFTWARE_VERSION_SYNC_SUCCESS:
					sem_syslog_dbg("sem_thread_recv:SEM_SOFTWARE_VERSION_SYNC_SUCCESS from slot %d\n", head->slot_id+1);
					version_sync_arr[head->slot_id].slot_id = USELESS_SLOT_ID;
					version_sync_arr[head->slot_id].version_sync_state = SYNC_SUCCESS;
					version_sync_arr[head->slot_id].version_sync_timeout = 0;
					product->product_slot_board_info[head->slot_id].board_state = BOARD_INSERTED_AND_REMOVED;
					sem_syslog_dbg("slot %d save versionfile and bootfile done\n", head->slot_id+1);
					break;
				case SEM_SOFTWARE_VERSION_SYNC_FAILED:
					sem_syslog_dbg("sem_thread_recv:SEM_SOFTWARE_VERSION_SYNC_FAILED from slot %d\n", head->slot_id+1);
					version_sync_arr[head->slot_id].slot_id = USELESS_SLOT_ID;
					version_sync_arr[head->slot_id].version_sync_state = SYNC_FAILED;
					version_sync_arr[head->slot_id].version_sync_timeout = 0;
					sem_syslog_dbg("slot %d failed save versionfile and bootfile, please sync by yourself\n", head->slot_id+1);
					break;
				case SEM_BOARD_INFO_SYN:
					sem_syslog_dbg("sem_thread_recv:SEM_BOARD_INFO_SYN from slot %d\n", head->slot_id+1);
					temp_board_info_syn = (board_info_syn_t *)(recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
					product->syn_product_info(temp_board_info_syn);
					break;
					
				case SEM_FORCE_STBY:
					sem_syslog_dbg("sem_thread_recv:SEM_FORCE_STBY from slot %d\n", head->slot_id+1);
					pthread_t new_thread;
					if (sem_thread_create(&new_thread, (void *)product->active_stby_switch, NULL, IS_DETACHED) == SEM_THREAD_CREATE_SUCCESS)
					{
						sem_syslog_dbg("\n");
					}
					else
					{
						sem_syslog_dbg("thread create failed\n");
					}
					break;

				case SEM_FILE_SYNING:
					sem_syslog_dbg("sem_thread_recv:SEM_FILE_SYNING from slot %d\n", head->slot_id+1);
					{
					    char *pchTemp = recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t);
                        product->sync_recv_file(pchTemp, 0, head->slot_id);						    
				    }
				    break;
                case SEM_FILE_SYN_FINISH:
					sem_syslog_dbg("sem_thread_recv:SEM_FILE_SYN_FINISH from slot %d\n", head->slot_id+1);
					{
					    char *pchTemp = recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t);
                        product->sync_recv_file(pchTemp, 1, head->slot_id);						    

						sem_syslog_dbg("\tActive_MCB Receive file (%s) from slot %d OK\n",pfilehead->chFileName, head->slot_id+1);
	                }
					break;
				case SEM_ETHPORT_INFO_SYN:
					sem_syslog_dbg("sem_thread_recv:SEM_ETHPORT_INFO_SYN from slot %d\n", head->slot_id+1);
                    ethport_info = (global_ethport_t *)(recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
                    eth_g_index = ethport_info->eth_g_index;
					sem_syslog_dbg("\tUpdate port(%d-%d) attr_bitmap = %x on active_MCB\n", ethport_info->slot_no, ethport_info->local_port_no,ethport_info->attr_bitmap);

					/*wangchao_add for notifiering up/down info*/
                    if ((global_ethport[eth_g_index]->attr_bitmap & ETH_ATTR_LINK_STATUS) != (ethport_info->attr_bitmap & ETH_ATTR_LINK_STATUS))
                    {
				        sem_syslog7_notice("eth%d-%d %-5s", ethport_info->slot_no, ethport_info->local_port_no,link_status_str[(ethport_info->attr_bitmap & ETH_ATTR_LINK_STATUS) >> ETH_LINK_STATUS_BIT]);
					}

					memcpy(global_ethport[eth_g_index], ethport_info, sizeof(global_ethport_t));
                    
					if(head->slot_id != standby_master_slot_id && (product->board_on_mask>>standby_master_slot_id &0x1))
					{
                        char chTmp[512] = {0};
					    memcpy(chTmp, ethport_info, sizeof(global_ethport_t));
						sem_syslog_dbg("\tActive_MCB forwarding port(%d-%d) info to standby_MCB\n", ethport_info->slot_no, ethport_info->local_port_no);
						sem_tipc_send(standby_master_slot_id, SEM_ETHPORT_INFO_SYN, chTmp, sizeof(global_ethport_t));
					}
					break;
				case SEM_CMD_FPGA_ABNORMAL:
                    fpga_abnormal_tipc = (fpga_abnormal_tipc_t *)(recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
					sem_syslog_warning("\nWARNING:On the board %d,the fpga is abnormal!!!!!\n",fpga_abnormal_tipc->slot_id);
					break;
			    case SEM_NETLINK_MSG:
					sem_syslog_dbg("sem_thread_recv:SEM_NETLINK_MSG form slot %d\n", head->slot_id+1);
					nl_head = (nl_msg_head_t*)(recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
					break;
				case SEM_CMD_SEND_STATE_TRAP:
					sem_syslog_dbg("sem_thread_recv:SEM_CMD_SEND_STATE_TRAP form slot %d\n", head->slot_id+1);
					//nl_head = (nl_msg_head_t*)(recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
        			system ("sor.sh cpfromblk sem_trap.flag 50");
        			fd = open("/mnt/sem_trap.flag", O_RDONLY | O_CREAT, 0644);
            		if(fd < 0){
        				sem_syslog_warning("SEM WARNING:open /mnt/sem_trap.flag failed.\n");
            		}else{
                		read(fd, &c, 1);
                		if(c == '1'){
                		}else{
                            ret = sem_dbus_system_state_trap(SYSTEM_READY);
							if(ret){
                                sem_syslog_warning("sem_dbus_system_state_trap SYSTEM_READY failed.\n");
							}
            			}
        			    close(fd);
            		}
        			system("sor.sh rm sem_trap.flag 5");
					break;
				case SEM_CMD_SEND_STATE_TRAP_REBOOT:
					sem_syslog_dbg("sem_thread_recv:SEM_CMD_SEND_STATE_TRAP_REBOOT form slot %d\n", head->slot_id+1);
                    ret = sem_dbus_system_state_trap(SYSTEM_REBOOT);
					if(ret){
                        sem_syslog_warning("sem_dbus_system_state_trap SYSTEM_REBOOT failed.\n");
					}
					break;
				case SEM_NETLINK_MSG_BROADCAST:
					sem_syslog_dbg("sem_thread_recv:SEM_NETLINK_MSG_BROADCAST form slot %d\n", head->slot_id+1);
					nl_head = (nl_msg_head_t*)(recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
					char *chTmp = recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t);
                	int len = sizeof(nl_msg_head_t) + nl_head->count*sizeof(netlink_msg_t);

					sem_netlink_send(chTmp, len);
					for(i = 0; i < product->slotcount;i++)
					{
				       if(i != product->active_master_slot_id)
				       {
				            sem_tipc_send(i, SEM_NETLINK_MSG, chTmp, len);
					   }
					}
					break;
				case SEM_RESET_READY:
					sem_syslog_warning("sem_thread_recv:SEM_RESET_READY from slot from slot %d\n", head->slot_id+1);
					if ((head->slot_id != product->active_master_slot_id) && \
						(head->slot_id >= 0) && (head->slot_id < product->slotcount))
					{
						reset_ready_mask |= (0x1 << head->slot_id);
						sem_syslog_warning("slot %d ready to reset.\n", head->slot_id + 1);
					}
					break;
				case SEM_SET_SYSTEM_IMG_REPLY:
					sem_syslog_dbg("sem_thread_recv:SEM_SET_SYSTEM_IMG_REPLY from slot %d\n", head->slot_id+1);
					set_system_img_board_mask |= (1 << head->slot_id);
					int *retval;
					retval = (int *)(recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
					set_system_img_done_mask |= ((unsigned long long)(*retval) << (head->slot_id * 4));

					sem_syslog_dbg("reply set_system_img_done_mask : %llx\n", set_system_img_done_mask);
					sem_syslog_dbg("reply set_system_img_board_mask : %#x\n", set_system_img_board_mask);
					break;
				case SEM_TIPC_TEST:
					sem_syslog_warning("%s", recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
					break;
				case SEM_SEND_FILE_TEST:
					sem_syslog_warning("%s", recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
					break;
				default:
					sem_syslog_dbg("Error:recv an error message type from slot %d\n", head->slot_id+1);
					break;
			}
		}
	}
}


int find_free_index(void)
{
	int i;
	for (i=0; i<product->slotcount; i++)
	{
		if (thread_id_arr[i].thread_index == -1)
		{
			return i;
		}
	}
	return -1;
}

/*
 *fucntion:It is used to response to messages which is received from all the board except active master board. 
 *including:board register, software synchronise and so on.
 *
 *
 */
void active_master_board_server_thread(void)
{
	struct sockaddr_tipc sock_addr;
	int sd;
	int fd;
	int index=0;

	int i;
	int test_packet_count_per_second = KEEP_ALIVE_TEST_FREQUENCY;
	
	struct timeval before;
	struct timeval after;

	char send_buf[SEM_TIPC_SEND_BUF_LEN];
	sem_pdu_head_t *head;
	sem_tlv_t *tlv_head;
	
	sem_syslog_warning("thread %s, thread id is %d\n", __FUNCTION__, getpid());

	if (product->slotcount > MAX_SLOT_NUM)
	{
		sem_syslog_warning("product->slotcount=%d is bigger than MAX_SLOT_NUM=%d\n", product->slotcount, MAX_SLOT_NUM);
		// TODO:must reset the product, It is an fatal error.
	}

	active_mcb_server_flag = 0;
	
	for (i=0; i<product->slotcount; i++)
	{
		//sem_syslog_dbg("slotid=%d i=%d\n", local_board->slot_id+1, i+1);
		if (local_board->slot_id != i)
		{
			//for sending
			if (tipc_set_send_addr(&g_send_sd_arr[i].sock_addr, SEM_TIPC_INSTANCE_BASE_ACTIVE_TO_NON_ACTIVE+i))
			{
				sem_syslog_dbg("	set server tipc addr failed\n");
				return ;
			}
			
			g_send_sd_arr[i].sd = socket (AF_TIPC, SOCK_RDM, 0);
			if (g_send_sd_arr[i].sd < 0)
			{
				sem_syslog_dbg("	server create send sd failed:sd = %d for slot %d\n", g_send_sd_arr[i].sd, i+1);
				// TODO:must reset the product.
				return ;
			}
			else
			{
				sem_syslog_warning("	create send sd = %d for slot %d success\n", g_send_sd_arr[i].sd, i+1);
			}

			
			//for recving
						
			g_recv_sd_arr[i].sd = socket (AF_TIPC, SOCK_RDM, 0);

			if (g_recv_sd_arr[i].sd < 0)
			{
				sem_syslog_dbg("	server create recv sd failed:sd = %d for slot %d\n", g_recv_sd_arr[i].sd, i+1);
				// TODO:must reset the product.
				return ;
			}
			else
			{
				sem_syslog_warning("	create recv sd = %d for slot %d success\n", g_recv_sd_arr[i].sd, i+1);
			}

			if (tipc_set_recv_addr(&g_recv_sd_arr[i].sock_addr, SEM_TIPC_INSTANCE_BASE_NON_ACTIVE_TO_ACTIVE+i, SEM_TIPC_INSTANCE_BASE_NON_ACTIVE_TO_ACTIVE+i))
			{
				sem_syslog_dbg("	set server tipc addr failed\n");
				return ;
			}
			
		  	if (0 != bind (g_recv_sd_arr[i].sd, (struct sockaddr*)&g_recv_sd_arr[i].sock_addr, sizeof(struct sockaddr)))
		  	{
				sem_syslog_dbg("	bind failed, return\n");
				return;
		  	}

			thread_id_arr[i].sd = g_recv_sd_arr[i].sd;
			g_recv_sd_arr[i].slot = i;
			if (!sem_thread_create(&thread_id_arr[i].thread_index, (void *)sem_thread_recv, (void *)(&g_recv_sd_arr[i]), IS_DETACHED))
			{
				thread_id_arr[i].thread_index = FREE_THREAD_ID;
				sem_syslog_dbg("	create recv thread for slot %d failed\n", i+1);
			}
		}
		else
		{
			thread_id_arr[i].thread_index = FREE_THREAD_ID;	
		}
	}
	if (product->product_type == XXX_YYY_AX8610 || product->product_type == XXX_YYY_AX8603)
	{
		if (!sem_thread_create(&sem_compatible_thread, (void *)sem_compatible_test_thread, NULL, IS_DETACHED)) {
			sem_syslog_warning("create sem_compatible_test_thread failed.sem enter into non-compatible mode\n");
		} else {
			sem_syslog_warning("create sem_compatible_test_thread succeed.sem enter into compatible mode\n");
		}
	}
	active_mcb_server_flag = 1;
	sem_syslog_dbg("active_master_board_server_thread exit\n");
}



int update_product_info(product_info_syn_t *temp)
{
	/*update the active master board*/
	if (!temp)
	{
		sem_syslog_dbg("temp is null\n");
		return 1;
	}
	
	set_dbm_effective_flag(INVALID_DBM_FLAG);
	memcpy(&product_info_syn, temp, sizeof(product_info_syn_t));
	product->product_dbm_file_create(temp);
	set_dbm_effective_flag(VALID_DBM_FLAG);
	return 0;
}


/*
 *function:It is used for non active master board to recevie and process tipc messages from active master board.
 *include:exec commond,keep alive mode,synchronise software version
 *
 *
 */


extern void make_XOR_checkcode(char * input_buf,char * checkcode);

void test_timeout_thread(void)
{		
	while (1) 
	{		
		if (!is_timeout&&start) 
		{	
			exit_flag = 1;
		}		
		sleep(1);
		is_timeout--;
	}
}

int non_active_master_board_server_thread(void *arg)
{
	#if 1
	int sd;
	int ret;
	sem_pdu_head_t *head;
	sem_tlv_t *tlv_head;
	nl_msg_head_t *nl_head;
	sync_file_head_t *pfilehead;
	char recv_buf[SEM_TIPC_RECV_BUF_LEN];
	int recv_len = SEM_TIPC_RECV_BUF_LEN;
	product_info_syn_t *product_info_head;
	global_ethport_t *ethport_info;
	unsigned int eth_g_index, start_index, eth_l_index, slot_id;
	int flag = 1, i = 0;
	fd_set rdfds;
	char file_path[64];
	int count = 0;
	int addr_len = sizeof(struct sockaddr_tipc);
	char *img_name;
	pthread_t thread_timeout;
	int tipc_count=0, erro_count=0;
	tipc_buf_t *tipcbuf, *inputstr;
	char input_str[SEM_TIPC_RECV_BUF_LEN];
	char chkcode[9];
	FILE *fp = 	NULL;
	char cmdstr[128] = {0};
	char result_str[64];
	int fd;
	char c;
	char r_md5[60];
	char md5[60];
	char str[50];
	memset(input_str, 0x0, sizeof(input_str)); 
	memset(chkcode, 0x0, sizeof(chkcode)); 

	struct timeval tv;
	struct timezone tz;

	struct timeval tvstart, tvend;
	//struct sockaddr_tipc addr;

	sem_syslog_warning("thread %s, thread id is %d\n", __FUNCTION__, getpid());

	//sd = (int)arg;
	
	sem_syslog_dbg("non active master board sem_thread_recv created.recv sd is %d\n", g_recv_sd); 

	if (sem_thread_create(&thread_timeout, (void *)test_timeout_thread, NULL, 0) != SEM_THREAD_CREATE_SUCCESS)
	{
		sem_syslog_warning("create timeout thread failed\n");
	}
	
	while(1)
	{
		int recv_len = SEM_TIPC_RECV_BUF_LEN;
		if (recvfrom(g_recv_sd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&g_recv_sock_addr, &addr_len) <= 0)
		{
			if ((count % 60) == 0)
				sem_syslog_warning("non_active_master_board_server_thread:recv error\n");			
			count++;
			continue;
		}

		active_mcb_state = IS_ALIVING;
		
		head = (sem_pdu_head_t *)recv_buf;
		tlv_head = (sem_tlv_t *)(recv_buf + sizeof(sem_pdu_head_t));
		pfilehead = (sync_file_head_t*)(recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
		tipcbuf = (tipc_buf_t *)(recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
		if (head->slot_id < 0 || head->slot_id >= product->slotcount)
		{
			sem_syslog_warning("slave packet slot id is wrong:slot id = %d\n", head->slot_id);
			continue;
		}
			
		switch (tlv_head->type)
		{
			case SEM_CMD_PDU_STBY_ACTIVE_LINK_TEST_REQUEST:
				sem_syslog_event("recv keep alive packet from slot %d\n", head->slot_id+1);
				break;
			case SEM_HARDWARE_RESET_PREPARE:
				sem_syslog_warning("SEM_HARDWARE_RESET_PREPARE:prepare for hardware reset.\n");
				hardware_reset_prepare(g_send_sd, local_board->slot_id);
				
				break;
			case SEM_HARDWARE_RESET:
				sem_syslog_warning("SEM_HARDWARE_RESET:slot %d sd = %d recv hardware reset message from active MCB.reset itself\n", local_board->slot_id, g_recv_sd);
				if (local_board->hard_self_reset())
				{
					sem_syslog_warning("Warning:slot %d sd = %d self reset failed\n", local_board->slot_id, g_recv_sd);
				}
				break;
				
			case SEM_ACTIVE_MASTER_BOARD_SYN_PRODUCT_INFO:
				sem_syslog_dbg("SEM_ACTIVE_MASTER_BOARD_SYN_PRODUCT_INFO:syn product info from active master board\n");
				product_info_head = (product_info_syn_t *)(recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
				update_product_info(product_info_head);

				//update global pointer product
				product->active_master_slot_id = product_info_head->active_master_slot_id;
				product->more_than_one_master_board_on = product_info_head->more_than_one_master_board_on;
				product->product_state = product_info_head->product_state;
				product->board_on_mask = product_info_head->board_on_mask;
				
				//only execute once
				if (reinit && !local_board->is_master) {
					unsigned char sbuff[64] = {0};
					reinit = 0;
					sem_syslog_dbg("default_master_slot_id=%d active_master_slot_id=%d\n",
					product_info_head->default_master_slot_id, product_info_head->active_master_slot_id);
					if (product_info_head->default_master_slot_id != product_info_head->active_master_slot_id) {
						memset(sbuff, 0, 64);
						sprintf(sbuff, "ip addr del %d.%d.%d.%d/24 dev obc0", 169, 254, 1, local_board->slot_id+1);
						sem_syslog_dbg("execu:%s\n", sbuff);
						system(sbuff);

						memset(sbuff, 0, 64);
						sprintf(sbuff, "ip addr add %d.%d.%d.%d/24 dev obc1", 169, 254, 1, local_board->slot_id+1);
						sem_syslog_dbg("reinit execu:%s\n", sbuff);
						system(sbuff);
					}
					else {
						memset(sbuff, 0, 64);
						sprintf(sbuff, "ip addr del %d.%d.%d.%d/24 dev obc1", 169, 254, 1, local_board->slot_id+1);
						sem_syslog_dbg("execu:%s\n", sbuff);
						system(sbuff);

						memset(sbuff, 0, 64);
						sprintf(sbuff, "ip addr add %d.%d.%d.%d/24 dev obc0", 169, 254, 1, local_board->slot_id+1);
						sem_syslog_dbg("reinit execu:%s\n", sbuff);
						system(sbuff);
					}
				}
				
				if (update_mac_enable)
				{
					local_board->update_local_mac_info(product_info_head->base_mac_addr);
					base_mac_addr_notifier(product_info_head->base_mac_addr);
					update_mac_enable = 0;
				}
				
				if (update_sn_enable)
				{
					int i;
					for (i = 0; i < SERIAL_NUMBER_LEN; i++)
					{
						product->sn[i] = product_info_head->sn[i];
						product_info_syn.sn[i] = product_info_head->sn[i];
					}
					update_sn_enable = 0;
				}
				
				break;
			case SEM_FILE_SYNING:
				{
					sem_syslog_dbg("SEM_FILE_SYNING:syncing files from active master board\n");
				    char *pchTemp = recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t);
                    product->sync_recv_file(pchTemp, 0, head->slot_id);						    
			    }
			    break;
			case SEM_FILE_SYN_FINISH:
				{
					sem_syslog_dbg("SEM_FILE_SYN_FINISH:syncing files finished from active master board\n");
				    char *pchTemp = recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t);

                    product->sync_recv_file(pchTemp, 1, head->slot_id);						    
                    sem_syslog_dbg("\tReceive file (%s) from active_MCB OK\n", pfilehead->chFileName);
					
                    sprintf(file_path, "/dbm/shm/ethport/shm%d", local_board->slot_id+1); 
					if(strcmp(pfilehead->chFileName, file_path) == 0)
					{
						sem_syslog_dbg("\tconfig eth-port according to backup shm file\n");
                        for(i = 0; i < local_board->port_num_on_panel; i++)
                        {
                            sem_set_ethport_attr(i);
						}
					}
			    }
				break;	
			case SEM_ETHPORT_INFO_SYN:
                ethport_info = (global_ethport_t *)(recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
                eth_l_index = ethport_info->local_port_index;
				slot_id = ethport_info->slot_no - 1;
				sem_syslog_dbg("SEM_ETHPORT_INFO_SYN:Update port(%d-%d) on standby_MCB\n", ethport_info->slot_no, ethport_info->local_port_no);
				memcpy(&start_fp[slot_id][eth_l_index], ethport_info, sizeof(global_ethport_t));
				break;
		    case SEM_NETLINK_MSG:
				nl_head = (nl_msg_head_t*)(recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
				char *chTmp = recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t);
                int len = sizeof(nl_msg_head_t) + nl_head->count*sizeof(netlink_msg_t);
				netlink_msg_t *nl_msg = (netlink_msg_t*)(chTmp + sizeof(nl_msg_head_t));
				sem_syslog_dbg("SEM_NETLINK_MSG:Sem Receive netlink msg(%d) from active_MCB OK\n", nl_msg->msgType);
				sem_netlink_send(chTmp, len);
				break;
			case SEM_CMD_SEND_STATE_TRAP_REBOOT:
				sem_syslog_dbg("non_active_master_board_server_thread:SEM_CMD_SEND_STATE_TRAP_REBOOT form slot %d\n", head->slot_id+1);
                ret = sem_dbus_system_state_trap(SYSTEM_REBOOT);
				if(ret){
                    sem_syslog_warning("sem_dbus_system_state_trap SYSTEM_REBOOT failed.\n");
				}
				break;
			case SEM_CMD_SEND_STATE_TRAP:
				sem_syslog_dbg("non_active_master_board_server_thread:SEM_CMD_SEND_STATE_TRAP form slot %d\n", head->slot_id+1);
				//nl_head = (nl_msg_head_t*)(recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
    			system ("sor.sh cpfromblk sem_trap.flag 50");
    			fd = open("/mnt/sem_trap.flag", O_RDONLY | O_CREAT, 0644);
        		if(fd < 0){
    				sem_syslog_warning("SEM WARNING:open /mnt/sem_trap.flag failed.\n");
        		}else{
            		read(fd, &c, 1);
            		if(c == '1'){
            		}else{
                        ret = sem_dbus_system_state_trap(SYSTEM_READY);
						if(ret){
                            sem_syslog_warning("sem_dbus_system_state_trap SYSTEM_READY failed.\n");
						}
        			}
    			    close(fd);
        		}
    			system("sor.sh rm sem_trap.flag 5");
				break;
			case SEM_MCB_ACTIVE_STANDBY_SWITCH_NOTIFICATION:
				sem_syslog_dbg("SEM_MCB_ACTIVE_STANDBY_SWITCH_NOTIFICATION\n");
				/* standby active master board run standby to active switch function, support product 8610 temporarily */
				if (product->product_type == XXX_YYY_AX8610 || \
					product->product_type == XXX_YYY_AX8606 || \
					product->product_type == XXX_YYY_AX8603 || \
					product->product_type == XXX_YYY_AX8800)
				{
					if (local_board->is_master)
					{
						pthread_mutex_lock(&switch_lock);
						/* set switch flag to disable repetitive standby to active switch in keep alive thread, caojia added*/
						if (switch_flag == 1)
						{
							pthread_mutex_unlock(&switch_lock);
							continue;
						}
						else
						{
							switch_flag = 1;
						}
						pthread_mutex_unlock(&switch_lock);
						
						pthread_t new_thread;
						mcb_switch_arg_t *mcb_switch_arg;
						mcb_switch_arg = (mcb_switch_arg_t *)malloc(sizeof(mcb_switch_arg_t));
						memset(mcb_switch_arg, 0, sizeof(mcb_switch_arg_t));
						mcb_switch_arg->switch_type = COMMAND_MODE;
						mcb_switch_arg->broadcast_domain = ALL_BROADCAST;
						
						if (sem_thread_create(&new_thread, (void *)product->active_stby_switch, (void *)mcb_switch_arg, IS_DETACHED) == SEM_THREAD_CREATE_SUCCESS)
						{
							sem_syslog_dbg("create active_stby_switch thread\n");
						}
						else
						{
							sem_syslog_dbg("thread create failed\n");
							pthread_mutex_lock(&switch_lock);
							switch_flag = 0;
							pthread_mutex_unlock(&switch_lock);
						}
					}
					else
					{
						int re_connect_timeout = RE_CONNECT_TIMEOUT;

						active_mcb_state = IS_DEAD;
						while(active_mcb_state == IS_DEAD && re_connect_timeout)
						{
							if(active_link_test(SME_CONN_LINK_TEST_COUNT) != 0)
							{
								sem_syslog_dbg("businiess board link to activeMCB failed %d\n", i++);
								re_connect_timeout--;
								sleep(1);
								continue;
							}
							else
							{
								active_mcb_state = IS_ALIVING;
								break;
							}
						}

						if (re_connect_timeout)
						{
							sem_syslog_warning("	reconnect to active MCB done\n");
							sleep(1);
							local_board->board_register(local_board, g_send_sd, &board_info_to_syn);
							reinit = 1;
						}
						else
						{
							sem_syslog_warning("	reconnect to active MCB failed,save snapshot then self-reset\n");
							snapshot_before_reset();
							local_board->hard_self_reset();
						}
					}
				}
				break;
			case SEM_SET_SYSTEM_IMG:
				{
					int *retval;
					img_name = (char *)(recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
					sem_syslog_dbg("SEM_SET_SYSTEM_IMG:img_name = %s\n", img_name);

					ret = sem_set_boot_img(img_name);

					head->slot_id = local_board->slot_id;
					tlv_head->type = SEM_SET_SYSTEM_IMG_REPLY;

					retval = (int *)(recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
					ret += 1;
					*retval = ret;
					sem_syslog_dbg("SEM_SET_SYSTEM_IMG retval : %d\n", *retval);
					sem_syslog_dbg("SEM_SET_SYSTEM_IMG ret : %d\n", ret);

					if (sendto(g_send_sd, recv_buf, SEM_TIPC_SEND_BUF_LEN, 0, \
						(struct sockaddr*)&g_send_sock_addr, sizeof(struct sockaddr_tipc)) < 0)
					{
						sem_syslog_warning("send SEM_SET_SYSTEM_IMG_REPLY failed\n");
					}
					else
					{
						sem_syslog_dbg("send SEM_SET_SYSTEM_IMG_REPLY succeed\n");
					}
				}
				
				break;
			case SEM_DISABLE_KEEP_ALIVE_TEMPORARILY:
				sem_syslog_dbg("SEM_DISABLE_KEEP_ALIVE_TEMPORARILY\n");
				int *time;
				time = (int *)(recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
				sem_syslog_dbg("SEM_DISABLE_KEEP_ALIVE_TEMPORARILY time : %d\n", *time);

				if (disable_keep_alive_time <= 0)
				{
					gettimeofday(&tv, &tz);
					disable_keep_alive_start_time = tv.tv_sec;
					disable_keep_alive_time = *time;
					disable_keep_alive_flag_time = disable_keep_alive_time;
				}
				else if (disable_keep_alive_time > 0)
				{
					if (*time > disable_keep_alive_time)
					{
						gettimeofday(&tv, &tz);
						disable_keep_alive_start_time = tv.tv_sec;
						disable_keep_alive_time = *time;
						disable_keep_alive_flag_time = disable_keep_alive_time;
					}
				}
				
				break;
			case SEM_EXECUTE_SYSTEM_COMMAND:
				sem_syslog_dbg("SEM_EXECUTE_SYSTEM_COMMAND\n");
				char *cmd;
				cmd = (char *)(recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
				sem_syslog_dbg("SEM_EXECUTE_SYSTEM_COMMAND cmd : %s\n", cmd);

				ret = system(cmd);

				sem_syslog_dbg("execute cmd return value : %d\n", ret);
				
				break;
			case SEM_REQUEST_BOARD_REGISTER:
				sem_syslog_dbg("SEM_REQUEST_BOARD_REGISTER\n");
				/* Evade the tipc connecttion problem after mcb active standby switch temporary */
				local_board->board_register(local_board, g_send_sd, &board_info_to_syn);
				break;
			case SEM_PEND_CVM:
				sem_syslog_dbg("SEM_PEND_CVM\n");
				system("echo 0 > /sys/module/octeon_ethernet/parameters/pend_cvm");
				break;
			case SEM_SEND_FILE_TEST:
				strncpy(r_md5, recv_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t), 60);
				while (access(TESTFILE_PATH, F_OK)!=0)
				{
					sleep(1);
				}
				system("sudo /usr/bin/sor.sh imgmd5 /home/admin/testfile 120 > /home/admin/md5");
				while (access(MD5_PATH,F_OK)!=0)
				{
					sleep(1);
				}
				fd = open(MD5_PATH, O_RDONLY);
				if(fd < 0)
				{
					sem_syslog_dbg("file %s open failed.%s\n", MD5_PATH, __FUNCTION__);
					return -1;
				}
				read(fd, md5, 60);
				close(fd);
				r_md5[59] = '\0';
				md5[59] = '\0';
				if (strcmp(r_md5, md5)==0)
				{
					sprintf(str,"send file success with right MD5.from slot%d\n", local_board->slot_id+1);
					sem_tipc_send(product->active_master_slot_id, SEM_SEND_FILE_TEST, str, sizeof(str));
				}
				else
				{
					sprintf(str,"send file success with wrong MD5.from slot%d\n", local_board->slot_id+1);
					sem_tipc_send(product->active_master_slot_id, SEM_SEND_FILE_TEST, str, sizeof(str));
				}
				break;
			case SEM_TIPC_TEST:
				memcpy(input_str, recv_buf, SEM_TIPC_RECV_BUF_LEN);
				inputstr = (tipc_buf_t *)(input_str + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
				memset(inputstr->checkcode, 0, 9);
				make_XOR_checkcode(input_str, chkcode);
				//sem_syslog_warning("tipcbuf->num=%d, tipcbuf->total=%d, tipcbuf->checkcode=%s\n", tipcbuf->num, tipcbuf->total, tipcbuf->checkcode);
				//sem_syslog_warning("tipcbuf->checkcode=%s, chkcode=%s\n", tipcbuf->checkcode, chkcode);
				if(strcmp(tipcbuf->checkcode, chkcode)!=0)
	       		{
       				//sem_syslog_dbg("recv num%d error", tipcbuf->num);
					erro_count++;
				}
				else
				{					
					//sem_syslog_dbg(".");
				}
				start = 1;
				tipc_count++;
				is_timeout = 3;
				//char msgBuf[100] = "non_active_master had recved\n";
				//sem_tipc_send(product->active_master_slot_id, SEM_TIPC_TEST, msgBuf, sizeof(msgBuf));
				break;/*clx*/
			default:
				sem_syslog_dbg("Warning:recv message type %d from slot %d\n", tlv_head->type, head->slot_id+1);
				break;
		}
		if (exit_flag)
		{	
			char msgBuf[100];
			sprintf(msgBuf, "test done\n\nactive_master send %d, non_active_master recv %d, %d data error\n", tipcbuf->total, tipc_count, erro_count);
			sem_tipc_send(product->active_master_slot_id, SEM_TIPC_TEST, msgBuf, sizeof(msgBuf));
			sem_syslog_warning("test done\n\nactive_master send %d, non_active_master recv %d, %d data error\n", tipcbuf->total, tipc_count, erro_count);
 			start = 0;
			exit_flag = 0;
			tipc_count = 0;
			erro_count = 0;
		}
	}
	sem_syslog_dbg("Error:this thread should not be exit\n");
	#endif
}

int get_temerature_info(int *temp)
{
	if(get_num_from_file(MASTER_TEMP2_INPUT, temp) < 0){
		if(get_num_from_file(TMP421_MASTER_TEMP2_INPUT, temp) < 0){
			return -1;
		}	
	}
	return 0;

}


int non_active_master_board_keep_alive_test_client(void *arg)
{
    int alive_print_flag = 1;
	int dead_print_flag = 1;
	static int dead_count = 1;
	static int alive_count = 1;

	int flag = 1;
	int interval = 1;
	
	char send_buf[SEM_TIPC_SEND_BUF_LEN];
	sem_pdu_head_t *head;
	sem_tlv_t *tlv_head;

	sem_tmperature_info *temp_info;
	int temp = -1;
	int regData;
	
	int ret;
	static int counter = 0;
	int test_packet_count_per_second = KEEP_ALIVE_TEST_FREQUENCY;
	
	struct timeval tv;
	struct timezone tz;
	long current_time_sec = 0;
	int timeout = NON_ACTIVE_TEST_ACTIVE_TIMEOUT;
	char str[256];
	
	sem_syslog_warning("thread %s, thread id is %d\n", __FUNCTION__, getpid());
		
	if (!arg)
	{
		sem_syslog_dbg("arg is null\n");
		return 1;
	}

	sem_syslog_dbg("non active master keep alive thread is running\n");
	while (1)
	{
		head = (sem_pdu_head_t *)send_buf;
		head->slot_id = local_board->slot_id;
		
		tlv_head = (sem_tlv_t *)(send_buf + sizeof(sem_pdu_head_t));
		tlv_head->type = SEM_CMD_PDU_STBY_ACTIVE_LINK_TEST_REQUEST;
				
		temp_info = (sem_tmperature_info*)(send_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));

		if (get_temerature_info(&temp) < 0){
			sem_syslog_dbg("Get temperature failed\n");
		}
		/*wangchao add:  ALARM : when TEMPERATRUE >= 95
						 SHUT DOWN : when TEMPERATURE >= 105*/
		temp_info->core_temp = temp;
		
		temp = temp/1000;
		if (local_board->board_type == BOARD_TYPE_AX81_12X 
			|| local_board->board_type == BOARD_TYPE_AX81_1X12G12S
			|| local_board->board_type == BOARD_TYPE_AX71_1X12G12S 
			/*|| local_board->board_type == BOARD_TYPE_AX81_4X*/ )
		{
			if (temp >= 105){
				
				sem_syslog_dbg("The CPU temperature more than 105, shut down the board!!\n");
				
				if ( sem_write_cpld(CPLD_HW_SELFT_RESET_REG, 0xfd) < 0) {
					sem_syslog_dbg("write cpld error\n");
					return -1;						
				}
			} 
			else if (temp >= 95){
				
				sem_syslog_dbg("The CPU temperature more than 95, ALARM!!\n");
				ret = sem_read_cpld(CPLD_TEMPERATURE_ALARM, &regData);
				if (ret){
					sem_syslog_dbg("read cpld error\n");
					return -1;
				}
				regData |= 0x1;	
				if (sem_write_cpld(CPLD_TEMPERATURE_ALARM, regData)) {
					sem_syslog_dbg("write cpld error\n");
					return -1;	
				}
			}	

			
		}
		
		//sem_syslog_dbg("temp_info->core_temp == %d\n",temp_info->core_temp);
		while (test_packet_count_per_second--)
		{
			if (sendto(g_send_sd, send_buf, sizeof(sem_pdu_head_t)+sizeof(sem_tlv_t) + sizeof(temp_info), 0, (struct sockaddr*)&g_send_sock_addr, sizeof(struct sockaddr_tipc)) < 0)
			{
				sem_syslog_warning("	slave test:send keep aliving packet failed\n");
			}
		}

		if (test_packet_count_per_second<=0)
		{
			test_packet_count_per_second = KEEP_ALIVE_TEST_FREQUENCY;	
		}
			
		counter++;
		sleep(1);
		interval--;
		
		#if 0
		//send keep alive packet every 1 seconds,check board state every 10 seconds
		if (!flag)
		{
			flag = 10;
		}
		else
		{
			flag--;
			continue;
		}
		#endif

		if (disable_keep_alive_time > 0)
		{
			while (disable_keep_alive_time > 0)
			{
				sleep(1);
				disable_keep_alive_time--;
				gettimeofday(&tv, &tz);
				current_time_sec = tv.tv_sec;
				if ((current_time_sec - disable_keep_alive_start_time - 5) > \
					(disable_keep_alive_flag_time - disable_keep_alive_time))
				{
					disable_keep_alive_time = disable_keep_alive_flag_time - (current_time_sec - disable_keep_alive_start_time);
				}
				//sem_syslog_dbg("current time : %d\n", current_time_sec);
				//sem_syslog_dbg("disabling keep alive,rest times : %d\n", disable_keep_alive_time);
			}
			
			continue;
		}

		//active MCB is aliving
		if (active_mcb_state == IS_ALIVING)
		{
			flag = 1;
			timeout = NON_ACTIVE_TEST_ACTIVE_TIMEOUT;
		}
		//active MCB may dead
		else
		{
			if (timeout == 30)
			{
				sem_syslog_warning("non-active test no active 30s, start ping.\n");
				sprintf(str, "ping 169.254.1.%d -w 28 >> /var/run/bootlog.sem &", product_info_syn.active_master_slot_id+1);
				system(str);
			}
			if (flag==1 && timeout<=10)
			{
				flag = 0;
				sem_syslog_warning("non-active MCB test active MCB is aliving count down\n");
			}

			timeout--;
			//sem_syslog_warning("timeout=%d\n", timeout);
			if (timeout <= 9)
				sem_syslog_warning("active MCB not detected:%d\n", timeout);
			
			
			if (timeout != 0)
			{
				continue;
			}

			sem_syslog_warning("active MCB is dead\n");
			/* when lost connection with active master, run standby to active switch function, support product 8610 temporarily */
			if (local_board->is_master && (product->product_type == XXX_YYY_AX8610 || \
				product->product_type == XXX_YYY_AX8606 || \
				product->product_type == XXX_YYY_AX8800 || \
				product->product_type == XXX_YYY_AX8603))
			{
				sem_syslog_warning("	stby MCB change to active MCB\n");

				pthread_mutex_lock(&switch_lock);
				/* set switch flag to disable repetitive standby to active switch in keep alive thread, caojia added*/
				if (switch_flag == 1)
				{
					pthread_mutex_unlock(&switch_lock);
					continue;
				}
				else
				{
					switch_flag = 1;
				}
				pthread_mutex_unlock(&switch_lock);
				
				pthread_t new_thread;
				mcb_switch_arg_t *mcb_switch_arg;
				mcb_switch_arg = (mcb_switch_arg_t *)malloc(sizeof(mcb_switch_arg_t));
				memset(mcb_switch_arg, 0, sizeof(mcb_switch_arg_t));
				mcb_switch_arg->switch_type = PANIC_MODE;
				mcb_switch_arg->broadcast_domain = ALL_BROADCAST;
				
				if (sem_thread_create(&new_thread, (void *)product->active_stby_switch, (void *)mcb_switch_arg, IS_DETACHED) == SEM_THREAD_CREATE_SUCCESS)
				{
					sem_syslog_dbg("\n");
				}
				else
				{
					sem_syslog_dbg("thread create failed\n");

				}
			}
			else
			{
				sem_syslog_dbg("	businiess board reconnect to active MCB\n");
				int re_connect_timeout = 15;
				int i = 0;
				while(active_mcb_state == IS_DEAD && re_connect_timeout)
				{
					if(active_link_test(SME_CONN_LINK_TEST_COUNT) != 0)
					{
						sem_syslog_dbg("businiess board link to activeMCB failed %d\n", i++);
					}
					sleep(1);
					re_connect_timeout--;
				}

				if (re_connect_timeout)
				{
					sem_syslog_warning("	reconnect to active MCB done\n");
					flag = 1;
					timeout = NON_ACTIVE_TEST_ACTIVE_TIMEOUT;
					local_board->board_register(local_board, g_recv_sd, &board_info_to_syn);
				}
				else
				{
					sem_syslog_warning("	reconnect to active MCB failed,save snapshot then self-reset\n");
					snapshot_before_reset();
					local_board->hard_self_reset();
				}
			}	
		}
		active_mcb_state = IS_DEAD;
	}
	sem_syslog_dbg("^^^^^^^^^^^^^^^^^^^^^^^^^^stby error^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
}

int board_self_reboot(void)
{
	int ret = 0;

	ret = sem_write_cpld(CPLD_HW_SELFT_RESET_REG, 0x0);
	
	sem_syslog_dbg("Critical Error:System self reboot failed\n");
	return ret;
}

/*
  *return value:0 for success;1 for failed
  *
  */
int check_board_insert_slot(void)
{
	int i;
	int master_flag = 1, non_master_flag = 0;
	if (!product_info_syn.is_distributed) {
		sem_syslog_warning("non distributed product no need to check board\n");
		return 0;
	}
	if (!product || !local_board) {
		sem_syslog_warning("product is %s local_board is %s\n", product ? "true" : "null",
																local_board ? "true" : "null");
		return 1;
	}
	
	for (i=0; i<product->master_slot_count; i++) {
		if (local_board->slot_id == product->master_slot_id[i]) {
			if (local_board->is_master) {
				master_flag = 0;
				break;
			} else {
				non_master_flag = 1;
				break;
			}
		} else {
			continue;
		}
	}
	return local_board->is_master ? master_flag : non_master_flag;
}

void update_port_state(int slot, int port)
{
	struct eth_port_s port_info;
	
	int retval = sem_read_eth_port_info(slot, port, &port_info);
	if(retval < 0)
	{
        sem_syslog_dbg("Failed to read port%d-%d info \n", slot, port);
	}
	
    start_fp[slot][port].attr_bitmap = port_info.attr_bitmap;
    start_fp[slot][port].port_type = port_info.port_type;
	
	msync(start_fp[slot], sizeof(struct global_ethport_s)*BOARD_GLOBAL_ETHPORTS_MAXNUM, MS_SYNC);

	char chTmp[512] = {0};
    memcpy(chTmp, &start_fp[slot][port], sizeof(global_ethport_t));
			
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

void slave_cpu_port_check_fc(void)
{
	unsigned short status;
	int i;
	while (1)
	{
		for (i = 0; i < 4; i++)
		{
			status = sem_oct_mdio_read(i, 1);
			if ((start_fp[local_board->slot_id][i].attr_bitmap>>1&1) != (status>>2&1)/*check link status*/)
			{
				update_port_state(local_board->slot_id, i);
			}
		}
		sleep(2);
	}
}

int main(int argc,char **argv)
{
	int ret;
    int group = 1;
	int reboot_flag = 0;
	int fd = -1;
	int i;
	int nub = 0;
	
	fd = open(SEM_SYSTEM_STARTUP_LOG_PATH, O_RDWR,0666);
	if(fd != -1) {
		sem_syslog_warning("sem allready run, exit\n");
		close(fd);
		return 1;
	}

	sem_syslog_warning("thread %s, thread id is %d\n", __FUNCTION__, getpid());

	//create sem dbus thread to handle dbus message from dcli
    if (sem_thread_create(&sem_dbus_thread, (void *)sem_dbus_main, NULL, !IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
    {
    	sem_syslog_warning("create thread sem_dbus_main failed.System need to reboot\n");
		reboot_flag = 1;
    }

	// TODO:stby and active switch must do the same operate.
	memset(pkt_mark, 0, sizeof(packet_mark_t)*MAX_PKT_TYPE_NUM);
	memset(save_file_head, 0, sizeof(sync_save_file_head_t)*MAX_PKT_TYPE_NUM);
	
	for (ret=0; ret<MAX_PKT_TYPE_NUM; ret++)
	{
		pkt_mark[ret].packet_type = ret;
		save_file_head[ret].packet_type = ret;
	}
	
	for (ret=0; ret<MAX_SLOT_NUM; ret++)
	{
		thread_id_arr[ret].thread_index = FREE_THREAD_ID;
		thread_id_arr[ret].sd = SEM_FREE_SD;
		thread_id_arr[ret].slot_id = USELESS_SLOT_ID;
		thread_id_arr[ret].is_aliving = IS_DEAD;
		
		version_sync_arr[ret].slot_id = USELESS_SLOT_ID;
	}

	set_dbm_effective_flag(INVALID_DBM_FLAG);

	sem_syslog_dbg("SEM_MAIN:product init...\n");
	ret = product_init();
	if (ret)
	{
		sem_syslog_warning("sem:product init failed.System need to reboot\n");
		reboot_flag = 1;
	}
	else
		sem_syslog_dbg("SEM_MAIN:product init done\n");
	
	sem_syslog_dbg("SEM_MAIN:board init...\n");
	ret = board_init();
	if (ret)
	{
		sem_syslog_warning("local board init failed.System need to reboot\n");
		reboot_flag = 1;
	}
	else
		sem_syslog_dbg("SEM_MAIN:board init done\n");
    #if 0
	if(local_board->board_code == AX81_1X12G12S_BOARD_CODE)
	{
        if (sem_thread_create(&FPGA_keep_alive_thread, (void *)ax81_1x12g12s_download_fpga_QT, NULL, IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
    	{
    		sem_syslog_warning("create FPGA_keep_alive_thread failed.\n");
			reboot_flag = 1;
    	}
	}
    #endif

	if (check_board_insert_slot()) {
		sem_syslog_warning("slot %d:%s board insert the %s board slot\n", local_board->slot_id+1,
			local_board->is_master ? "master" : "business", local_board->is_master ? "business" : "master");
		reboot_flag = 1;
	}

	if (reboot_flag)
	{
		sem_syslog_dbg("Board reboot\n");
		system("sudo mount /blk");
		system("cp /var/run/bootlog.sem /blk/");
		system("date >> /blk/bootlog.sem");
		system("sync");
		board_self_reboot();
	}

	//non-distributed product block here.
	if (!product_info_syn.is_distributed)
	{
		sem_syslog_warning("non-distributed product init done\n");
		sem_init_completed();
		set_dbm_effective_flag(VALID_DBM_FLAG);
		while(1)
		{
			usleep(1000);
		}
	}

	if (!product || !local_board) {
		sem_syslog_warning("product or board init failed\n");
		return 1;
	}
	
	sem_syslog_dbg("SEM_MAIN:tipc interface init...\n");
	if (product->tipc_init(local_board))
	{
		sem_syslog_warning("tipc init failed.System need to reboot\n");
		reboot_flag = 1;
	}
	else
		sem_syslog_dbg("SEM_MAIN:tipc interface init done\n");

	/* caojia@autelan.com comment out the following code temporary */
	#if 1
    if (sem_ethports_init())
    {
    	sem_syslog_warning("ethports init failed.System need to reboot\n");
		reboot_flag = 1;
    }
	#endif
	
	if (sem_thread_create(&netlink_recv_thread, (void *)sem_netlink_recv_thread, NULL, IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
	{
		sem_syslog_warning("create sem_netlink_recv_thread failed.System need to reboot\n");
		reboot_flag = 1;	
	}
	/*huangjing@autelan.com.Init for FPGA*/
	#if 1
	if(local_board->board_code == AX81_1X12G12S_BOARD_CODE)
	{
		car_head = creat_car_linklist();
		if(car_head == NULL)
		{
            sem_syslog_warning("creat car table linklist failed.System need to reboot\n");
			reboot_flag = 1;
		}
		car_white_head = creat_car_white_list();
		if(car_white_head == NULL)
		{
            sem_syslog_warning("creat car white table linklist failed.System need to reboot\n");
			reboot_flag = 1;
		}

        if (sem_thread_create(&QT2025_link_thread, (void *)poll_QT2025_link_status, NULL, IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
    	{
    		sem_syslog_warning("create poll_QT2025_link_status failed.System need to reboot\n");
    		reboot_flag = 1;
    	}
	}
	if (local_board->is_master)
	{
    	wan_if_head = creat_wan_if_linklist();
    	if(wan_if_head == NULL)
    	{
            sem_syslog_warning("creat wan interface linklist failed.System need to reboot\n");
    		reboot_flag = 1;
    	}
	}
	#endif

	if (reboot_flag)
	{
		sem_syslog_warning("System saving bootlog.sem to blk and Board ready to reboot\n");
		system("sudo mount /blk");
		system("cp /var/run/bootlog.sem /blk/");
		system("date >> /blk/bootlog.sem");
		system("sync");
		board_self_reboot();
	}

	if (product->product_type == XXX_YYY_SINGLE_BOARD) {
		product_single_board_ready_state();
		set_dbm_effective_flag(VALID_DBM_FLAG);
		while (1) {
			usleep(1000);
		}
	}
	if (local_board->board_type == BOARD_TYPE_AX81_AC12C && local_board->board_code == AX81_AC12C_BOARD_CODE)
	{
		if (sem_thread_create(&slave_cpu_port_check, (void *)slave_cpu_port_check_fc, NULL, IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
	    {
	    	sem_syslog_warning("create thread slave_cpu_port_check failed.\n");
	    }
		else
		{
			sem_syslog_warning("create thread slave_cpu_port_check success.\n");
		}
	}
	if (local_board->is_master)
	{
		memset(g_recv_sd_arr, 0x00, MAX_SLOT_NUM * sizeof(sd_sockaddr_tipc_t));
		memset(g_send_sd_arr, 0x00, MAX_SLOT_NUM * sizeof(sd_sockaddr_tipc_t));
		
		for (ret=0; ret<MAX_SLOT_NUM; ret++)
		{
			g_recv_sd_arr[ret].sd = SEM_FREE_SD;
			g_send_sd_arr[ret].sd = SEM_FREE_SD;
		}
		
		sem_syslog_dbg("SEM_MAIN:master active or syby select...\n");
		if (product->master_active_or_stby_select(&g_recv_sd, local_board))
		{
			sem_syslog_warning("MCB master_active_or_stby_select failed.System need to reboot\n");
			sem_syslog_warning("System saving bootlog.sem to blk and Board ready to reboot\n");
			system("sudo mount /blk");
			system("cp /var/run/bootlog.sem /blk/");
			system("date >> /blk/bootlog.sem");
			system("sync");
			board_self_reboot();
		}
		else
			sem_syslog_dbg("SEM_MAIN:master active or syby select done\n");



		/* Add system_running netlink, chenlanxin@autelan.com 2012-07-26 */
		sem_syslog_dbg("SEM_MAIN:waiting for system running...\n");
		while (local_board->is_active_master)
		{
			int fd;
			char c;
			fd = open("/dbm/is_loadconfig_done", O_RDONLY | O_CREAT, 0644);
			if(fd < 0)
			{
				sleep(1);
				continue;
			}
			read(fd, &c, 1);
			if(c == '1')
			{
				sem_syslog_dbg("Loadconfig done\n"); 				
				product->product_state = SYSTEM_RUNNING;
				product->syn_product_info(&board_info_to_syn);
				product->update_product_state();
				
				product_state_notifier(SYSTEM_RUNNING);

				close(fd);
				sem_syslog_warning("System Running OK!\n");
				break;
			}
			else
			{
				if (nub++%10 == 0)
					sem_syslog_dbg("Read /dbm/is_loadconfig_done is not 1.\n");
				close(fd);
				sleep(1);
				continue;
			}						
		}
		if (local_board->is_active_master)
		{
			for(i = 0; i < product->slotcount;i++)
			{
			   if(i != product->active_master_slot_id)
			   {
					sem_tipc_send(i, SEM_PEND_CVM, NULL, 0);
			   }
			}
			system("echo 0 > /sys/module/octeon_ethernet/parameters/pend_cvm");
		}
		
		sem_syslog_dbg("enter into main thread\n");
        sleep(60);//this sleep is important for ensure that sem send the dbus signal can be received by trap.
        sem_send_signal_to_trap();
        sem_dbus_system_state_trap(SYSTEM_RUNNING);
		while(1)
		{
			usleep(1000);
		}
		
		sem_syslog_dbg("sem$$$$$$$$$$$$$$$$$shouldn't be printed$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
	}
	else
	{
		int i=0;
		int ret=-1;
		char file_path[64];

		sem_syslog_dbg("businiess board is linking to active MCB....");

		//for recving
		g_recv_sd = socket(AF_TIPC, SOCK_RDM, 0);

		if (g_recv_sd < 0)
		{
			sem_syslog_warning("create recv socket failed:g_recv_sd = %d.System need to reboot\n", g_recv_sd);
			reboot_flag = 1;
		}
		
		if (tipc_set_recv_addr(&g_recv_sock_addr, SEM_TIPC_INSTANCE_BASE_ACTIVE_TO_NON_ACTIVE+local_board->slot_id, SEM_TIPC_INSTANCE_BASE_ACTIVE_TO_NON_ACTIVE+local_board->slot_id))
		{
			sem_syslog_warning("set server tipc addr failed.System need to reboot\n");
			close(g_recv_sd);
			reboot_flag = 1;
		}

		if (0 != bind (g_recv_sd, (struct sockaddr*)&g_recv_sock_addr, sizeof(struct sockaddr)))
	  	{
			sem_syslog_warning("bind failed.System need to reboot\n");
			close(g_recv_sd);
			reboot_flag = 1;
	  	}

		//for STBY sending
		g_send_sd = socket(AF_TIPC, SOCK_RDM, 0);
		if (g_send_sd < 0)
		{
			sem_syslog_warning("	STBY create socket failed:g_send_sd = %d.System need to reboot\n", g_send_sd);
			reboot_flag = 1;
		}

		if (tipc_set_send_addr(&g_send_sock_addr, SEM_TIPC_INSTANCE_BASE_NON_ACTIVE_TO_ACTIVE+local_board->slot_id))
		{
			sem_syslog_dbg("default_master_slot_id	set client addr failed.System need to reboot\n");
			reboot_flag = 1;
		}

		if (reboot_flag)
		{
			sem_syslog_warning("System saving bootlog.sem to blk and Board ready to reboot\n");
			system("sudo mount /blk");
			system("cp /var/run/bootlog.sem /blk/");
			system("date >> /blk/bootlog.sem");
			system("sync");
			board_self_reboot();
		}
		
		while(ret=active_link_test(SME_CONN_LINK_TEST_COUNT) != 0)
		{
			sem_syslog_dbg("businiess board link to activeMCB failed %d\n", i++);
		}
		
		sem_syslog_dbg("link done\n");
		
		if(abort_sync_version())
			product->synchronise_software_version(g_recv_sd, local_board);
		else
			sem_syslog_dbg("Skip syncing version!!!\n");
		
		if (sem_thread_create(&slave_server_thread, (void *)non_active_master_board_server_thread, (void *)g_recv_sd, IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
		{
			sem_syslog_warning("non active master board server thread create failed.System need to reboot\n");
			reboot_flag = 1;
		}
		
		if (sem_thread_create(&slave_keep_alive_thread, (void *)non_active_master_board_keep_alive_test_client, (void *)g_recv_sd, IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
		{
			sem_syslog_warning("business board keep alive test thread create failed.System need to reboot\n");
			reboot_flag = 1;
		}
		
		if (reboot_flag)
		{
			sem_syslog_dbg("business board thread create error.System need to reboot\n");
			system("sudo mount /blk");
			system("cp /var/run/bootlog.sem /blk/");
			system("date >> /blk/bootlog.sem");
			system("sync");
			board_self_reboot();	
		}
		else
		{
			local_board->board_state = BOARD_READY;
			local_board->update_local_board_state(local_board);
			board_info_to_syn.board_state = local_board->board_state;
			local_board->board_register(local_board, g_send_sd, &board_info_to_syn);
            sprintf(file_path, "/dbm/shm/ethport/shm%d", local_board->slot_id+1);
        	sem_syslog_dbg("\tslot%d send ethport shm file to active_MCB\n", local_board->slot_id+1);
        	product->sync_send_file(product->active_master_slot_id, file_path, 0);
			sem_init_completed();
		}
		
		sem_syslog_dbg("business board main thread block here\n");
		while	(1)
		{
			usleep(1000);
		}
	}

	return 0;
}



#ifdef __cplusplus
}
#endif
