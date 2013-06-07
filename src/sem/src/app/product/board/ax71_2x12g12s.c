#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <linux/tipc.h>
#include <string.h>
#include <linux/ioctl.h>
#include <dbus/sem/sem_dbus_def.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include "cvm/ethernet-ioctl.h"

#include "sysdef/sem_sysdef.h"
#include "../../sem_eth_port.h"
#include "../../sem_common.h"
#include "../../sem_dbus.h"
#include "board_feature.h"
#include "ax71_2x12g12s.h"
#include "sysdef/returncode.h"
#include "../product_ax7605i.h"
#include "sem/sem_tipc.h"
#include "../product_feature.h"
#include "config/auteware_config.h"

//extern int update_local_board_state(board_fix_param_t *board);
//extern int local_board_dbm_file_create(board_fix_param_t *board);

/*
 * 0x1140 means: Enable Auto-Negotiation Process, Full-duplex, 100 Mbps
 */
static unsigned short phy_ctl_copper[] = {0x1140, 0x1140, 0x1140, 0x1140};
static unsigned short phy_ctl_fiber[] = {0x1140, 0x1140, 0x1140, 0x1140};

extern product_fix_param_t *product;
extern board_fix_param_t *local_board;

extern int get_interface_name(char *buf, char **name);
extern int set_mac_addr(char *name, unsigned char *mac_addr);

static int update_local_board_state(board_fix_param_t *board)
{
	FILE *fd;
	char buf[200];
	char buf1[200];
	
	fd = fopen("/dbm/local_board/is_active_master", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/is_active_master failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->is_active_master);
	fclose(fd);

	fd = fopen("/dbm/local_board/is_use_default_master", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/is_use_default_master failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->is_use_default_master);
	fclose(fd);

	fd = fopen("/dbm/local_board/board_state", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/board_state failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->board_state);
	fclose(fd);

	sprintf(buf, "/dbm/product/slot/slot%d", board->slot_id+1);
	strcpy(buf1, buf);
	strcat(buf1, "/board_state");
	fd = fopen(buf1, "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("update %s error\n", buf1);
		return -1;
	}
	fprintf(fd, "%d\n", board->board_state);
	fclose(fd);

	return 0;
}


static int syn_board_info(int sd, board_info_syn_t *board_info)
{
	sem_pdu_head_t *head;
	sem_tlv_t *tlv_head;
	board_info_syn_t *board_info_head;
	char send_buf[SEM_TIPC_SEND_BUF_LEN] = {0};

	sem_syslog_dbg("syn board info local_board->slot_id = %d\n", board_info->slot_id+1);
	

	head = (sem_pdu_head_t *)send_buf;
	head->slot_id = board_info->slot_id;
	head->version = 11;
	head->length = 22;

	tlv_head = (sem_tlv_t *)(send_buf + sizeof(sem_pdu_head_t));
	tlv_head->type = SEM_BOARD_INFO_SYN;
	tlv_head->length = 33;

	board_info_head = (board_info_syn_t *)(send_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
	memcpy(board_info_head, board_info, sizeof(board_info_syn_t));

	if (sendto(g_send_sd, send_buf, sizeof(sem_pdu_head_t)+sizeof(sem_tlv_t)+sizeof(board_info_syn_t), 0, (struct sockaddr*)&g_send_sock_addr, sizeof(struct sockaddr_tipc)) < 0)
	{
		sem_syslog_dbg("	sync board info failed\n");
		return 1;
	}
	else
	{
		sem_syslog_dbg("	sync board info succeed\n");
	}
	
	return 0;
}

static int local_board_dbm_file_create(board_fix_param_t *board)
{
	if(!board_dbm_file_create(board))
	{
        sem_syslog_dbg("board dbm file create failed!\n");
		return -1;
	}
}

/*
 *board to reboot itself's cpu
 *
 *
 */
static void reboot_self(void)
{
	system ("dcli_reboot.sh");

	#if 0
	sem_write_cpld(CPLD_HW_RESET_CONTROL_REG1, 0xff);
	#if 1
    struct timeval tv;
    tv.tv_sec = 1; //1s
    tv.tv_usec = 1;//1us  	
	int rc = select(0, NULL, NULL, NULL, &tv);
	#else
	sleep(1);
	#endif
	
	sem_write_cpld(CPLD_HW_RESET_CONTROL_REG1, 0xfe);
	#endif	
}


/*
 *
 *board ax71_2x12g12s hardware reset self
 *
 *
 */
 
int ax71_2x12g12s_hard_reset(void)
{
	int ret;
	sem_syslog_warning("board %s on slot %d self reset\n", local_board->name, local_board->slot_id);
	ret = sem_write_cpld(CPLD_HW_SELFT_RESET_REG, CPLD_SELF_RESET_DATA);
	return ret;
}


/*
 *board self special initialize
 *
 *ret val: 0 for success 1 for failed.
 */
int ax71_2x12g12s_init(void)
{
	return 0;
}


/*
 *function:It is used for boards except active master board to register current board state to active master board.
 *sem may block here
 *
 */
static int board_register(board_fix_param_t *board, int sd, board_info_syn_t *board_info)
{
	sem_pdu_head_t *head;
	sem_tlv_t *tlv_head;
	board_info_syn_t *board_info_head;
	
	char send_buf[SEM_TIPC_SEND_BUF_LEN] = {0};

	sem_syslog_dbg("register board state local_board->slot_id = %d\n", board->slot_id+1);

	head = (sem_pdu_head_t *)send_buf;
	head->slot_id = board->slot_id;
	head->version = 11;
	head->length = 22;

	tlv_head = (sem_tlv_t *)(send_buf + sizeof(sem_pdu_head_t));
	tlv_head->type = SEM_NON_ACTIVE_BOARD_REGISTER;
	tlv_head->length = 33;

	board_info_head = (board_info_syn_t *)(send_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
	memcpy(board_info_head, board_info, sizeof(board_info_syn_t));

	if (sendto(g_send_sd, send_buf, sizeof(sem_pdu_head_t)+sizeof(sem_tlv_t)+sizeof(board_info_syn_t), 0, (struct sockaddr*)&g_send_sock_addr, sizeof(struct sockaddr_tipc)) < 0)
	{
		sem_syslog_dbg("	board send register message failed\n");
		return 1;
	}
	else
	{
		sem_syslog_dbg("	board send register message succeed\n");
	}
	
	return 0;
}


/*
* for master board to init mac files
* add by caojia 20110730
*/
static int init_local_mac_info(void)
{
	return 0;
}

/*
* for business board update mac addr to files and interfaces 
* add by caojia , 20110730
*/
static int update_local_mac_info(unsigned char *mac_addr)
{
	int i = 0;
	FILE *fp;
	unsigned char local_mac_addr[6];
	char buf[1024];
	char *name;
	int ret;

	/* copy base mac to other positions */
	system("cp /dbm/product/base_mac /devinfo/base_mac");

	/* create local mac files */
	for (i = 0; i < 6; i++)
	{
		if (i == 5)
		{
			local_mac_addr[i] = mac_addr[i] + local_board->slot_id;
		}
		else
		{
			local_mac_addr[i] = mac_addr[i];
		}
	}
	fp = fopen("/devinfo/local_mac", "w+");
	if (fp == NULL)
	{
		sem_syslog_dbg("Open /devinfo/local_mac failed.\n");
	}
	else
	{
		fprintf(fp, "%02X%02X%02X%02X%02X%02X\n", local_mac_addr[0], local_mac_addr[1], local_mac_addr[2],\
			local_mac_addr[3], local_mac_addr[4], local_mac_addr[5]);
		fclose(fp);
	}

	system("cp /devinfo/local_mac /dbm/local_board/mac");

	/* change mac of all interfaces */

	/* open /proc/net/dev */
	fp = fopen("/proc/net/dev", "r");
	if (fp == NULL)
	{
		sem_syslog_dbg("Can't open proc file dev.\n");
		sem_syslog_dbg("Update mac address of interfaces failed.\n");
		return -1;
	}

	/* Drop header lines. */
	fgets (buf, 1024, fp);
	fgets (buf, 1024, fp);

	while (fgets(buf, 1024, fp) != NULL)
	{
		ret = get_interface_name(buf, &name);
		/* filter the obc interface */
		if ((!strncmp(name, "obc", 3)) || (!strncmp(name, "lo", 2)) ||\
			(!strncmp(name, "sit", 3)) || (!strncmp(name, "pimreg", 6)))
		{
			continue;
		}
		sem_syslog_dbg("device name : %s\n", name);
		ret = set_mac_addr(name, local_mac_addr);
	}

	fclose(fp);

	return 0;
}

/*
* for master board copy product sn to necessary positions and get local board sn to write to some files
* add by caojia, 20110809
*/
static int init_local_sn_info(void)
{
	int i = 0, ret = 0;
	int fd;
	FILE *fp;
	char tmp_sn_buffer[21];
	unsigned char temp_buffer[3] = {0x00, 0x00, 0x00};
	unsigned char product_sn[10];
	unsigned char board_sn[10];
	ax_module_sysinfo sysinfo;

	/* get local board sn */
	fd = open("/dev/bm0", 0);
	if (fd < 0)
	{
		sem_syslog_dbg("Open dev /dev/bm0 failed.\n");
		return -1;
	}

	sysinfo.slot_num = 0;
	sysinfo.product_id = 7;

	ret = ioctl(fd, BM_IOC_READ_MODULE_SYSINFO, &sysinfo);
	close(fd);
	if (ret != 0)
	{
		sem_syslog_dbg("read module info failed.\n");
	}

	sem_syslog_dbg("Module sn read from eeprom : %s\n", sysinfo.ax_sysinfo_module_serial_no);

	sem_syslog_dbg("Module Sn : ");
	for (i=0; i<20; i+=2)
	{
		memcpy(temp_buffer, sysinfo.ax_sysinfo_module_serial_no+i, 2);
		board_sn[i/2] = (unsigned char)strtoul((char *)temp_buffer, 0, 16);
		local_board->sn[i/2] = board_sn[i/2];
		sem_syslog_dbg_without_time("%02x", board_sn[i/2]);
	}
	sem_syslog_dbg_without_time("\n");

	fp = fopen("/devinfo/module_sn", "w+");
	if (fp == NULL)
	{
		sem_syslog_dbg("open /devinfo/module_sn failed.\n");
		return -1;
	}
	else
	{
		fprintf(fp, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n", board_sn[0], board_sn[1], board_sn[2],\
			board_sn[3], board_sn[4], board_sn[5], board_sn[6], board_sn[7], board_sn[8], board_sn[9]);

		fclose(fp);
	}

	system("cp /devinfo/module_sn /dbm/local_board/sn");
	
	return 0;
}

static int ax71_2x12g12s_show_system_environment(env_state_t *env_state)
{
	int master_temp1_input, master_temp2_input;

	if ((get_num_from_file(MASTER_TEMP1_INPUT, &master_temp1_input) >= 0)
			&&(get_num_from_file(MASTER_TEMP2_INPUT, &master_temp2_input) >= 0)){

		env_state->master_inter_temp = master_temp2_input/1000;
		env_state->master_remote_temp = master_temp1_input/1000;
		
	} else if ((get_num_from_file(TMP421_MASTER_TEMP1_INPUT, &master_temp1_input) >= 0)
					&&(get_num_from_file(TMP421_MASTER_TEMP2_INPUT, &master_temp2_input) >= 0)) {

		env_state->master_inter_temp = master_temp2_input/1000;
		env_state->master_remote_temp = master_temp1_input/1000;			
		
	} else {
		
		sem_syslog_dbg("ax71_2x12g12s read cpu temperature error! \n");
		return -1;	
	}

	return 0;
}


board_fix_param_t ax71_2x12g12s =
{
	.board_code = AX71_2X12G12S_BOARD_CODE,
	.board_type = BOARD_TYPE_AX71_2S12G12S,
	.board_id = BOARD_SOFT_ID_AX7605I_2X12G12S,
	.name = "AX71_2X12G12S",
	.slot_id = UNKONWN_SLOT_ID,
	.is_master = NOT_MASTER,
	.cpu_num = SINGLE_CPU,
	.port_num_on_panel = 26,
	.asic_port_num = 26,
	.asic_start_no = 1,
	.port_arr = {{ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1},\
	             {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1},\
                 {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1},\
	             {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1},\
				 {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1},\
	             {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1},\
                 {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1}},
	.function_type = SWITCH_BOARD,
	.obc_port_num = 2,
	.obc_port = {{CPU, 0, 12, "obc0", OBC_PORT, NOT_CSCD_PORT, 0}, 
				 {CPU, 0, 13, "obc1", OBC_PORT, NOT_CSCD_PORT, 1}},
	.cscd_port_num = 2,
	.cscd_port = {{ASIC, 1, 12, "cscd0", NOT_OBC_PORT, CSCD_PORT, 0}, 
				  {ASIC, 1, 13, "cscd1", NOT_OBC_PORT, CSCD_PORT, 1}},
	.board_state = BOARD_INSERTED,
	.board_ap_counter = BOARD_DEFAULT_AP_COUNTER,
	.board_ap_max_counter = AX71_2X12G12S_AP_MAX,
	.master_active_or_backup_state_set = NULL,
	.get_remote_board_master_state = NULL,
	.get_slot_id = NULL,
	.set_initialize_state = NULL,
	.get_initialize_state = NULL,
	.slot_reset = NULL,
	.get_cpld_version = NULL,
	.get_board_type = NULL,
	.get_product_type = NULL,
	.get_product_serial = NULL,
	.cpld_test = NULL,
	.update_local_board_state = update_local_board_state,
	.board_register = board_register,
	.syn_board_info = syn_board_info,
	.local_board_dbm_file_create = local_board_dbm_file_create,
	.reboot_self = reboot_self,
	.hard_self_reset = ax71_2x12g12s_hard_reset,
	.board_self_init = ax71_2x12g12s_init,
	.init_local_mac_info = init_local_mac_info,
	.update_local_mac_info = update_local_mac_info,
	.init_local_sn_info = init_local_sn_info,
	.show_system_environment = ax71_2x12g12s_show_system_environment
};


#ifdef __cplusplus
}
#endif
