#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <linux/tipc.h>
#include <string.h>
#include <linux/ioctl.h>
#include <dbus/sem/sem_dbus_def.h>
#include <sys/wait.h>
#include <unistd.h>

#include <stdlib.h> //huangjing
//#include <linux/in.h>
#include <sys/socket.h>
#include <linux/if_vlan.h>
#include <linux/sockios.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <errno.h>
#include <sys/mman.h> 
#include <linux/if.h>
#include <linux/if_arp.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syslog.h>
#include <fcntl.h>

#include "cvm/ethernet-ioctl.h"

#include "sysdef/sem_sysdef.h"
#include "../../sem_fpga.h"
#include "../../sem_eth_port.h"
#include "../../sem_common.h"
#include "../../sem_dbus.h"
#include "board_feature.h"
#include "ax81_1x12g12s.h"
#include "sysdef/returncode.h"
#include "../product_ax8610.h"
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
extern product_info_syn_t product_info_syn;
extern board_fix_param_t *local_board;
extern int sem_bm_phyQT2025_read(int regaddr, unsigned int location);
extern int write_fpga_reg(bm_op_args * ops);
extern int read_fpga_reg(bm_op_args * ops);
extern int board_self_reboot(void);
extern int write_car(unsigned short Indirect_add_reg_H,
                    unsigned short Indirect_add_reg_L,
                    unsigned short RELOAD_H,
                    unsigned short RELOAD_L,
                    unsigned short LINKUP,
                    unsigned short CAR_VALID,
                    unsigned int CAR_ID,
                    int FLAG);

//extern int set_mtu(const char *dev, int mtu);
//extern int get_mtu(const char *dev, unsigned int *mtu);
//extern int set_ethport_mtu(int slot, int port, unsigned int mtu_size);

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

	sprintf(buf, "/dbm/product/slot/slot%d", board->slot_id + 1);
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
	FILE *fd;
	system("mkdir -p /dbm/local_board/");

	//system("touch /dbm/local_board/name");

	fd = fopen("/dbm/local_board/name", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/name failed\n");
		return -1;
	}
	fprintf(fd, "%s\n", board->name);
	fclose(fd);

	fd = fopen("/dbm/local_board/board_code", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/board_code failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->board_code);
	fclose(fd);
	
	fd = fopen("/dbm/local_board/slot_id", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/slot_id failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->slot_id + 1);
	fclose(fd);
	
	fd = fopen("/dbm/local_board/is_master", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/is_master failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->is_master);
	fclose(fd);

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


	fd = fopen("/dbm/local_board/port_num_on_panel", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/port_num_on_panel failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->port_num_on_panel);
	fclose(fd);

	fd = fopen("/dbm/local_board/port_num_on_panel", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/port_num_on_panel failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->port_num_on_panel);
	fclose(fd);

	fd = fopen("/dbm/local_board/obc_port_num", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/obc_port_num failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->obc_port_num);
	fclose(fd);

	fd = fopen("/dbm/local_board/cscd_port_num", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/cscd_port_num failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->cscd_port_num);
	fclose(fd);

	fd = fopen("/dbm/local_board/function_type", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/function_type failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->function_type);
	fclose(fd);

	fd = fopen("/dbm/local_board/board_state", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/board_state failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->board_state);
	fclose(fd);
	
	fd = fopen("/dbm/local_board/board_id", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/board_id failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->board_id);
	fclose(fd);

	fd = fopen("/dbm/local_board/board_ap_max_counter", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/board_ap_max_counter failed\n");
		return -1;
	}
	#ifndef __AP_MAX_COUNTER
	fprintf(fd, "%d\n", board->board_ap_max_counter);
	sem_syslog_dbg("board->board_ap_max_counter = %d\n",board->board_ap_max_counter);
	#else
	board->board_ap_max_counter = (__AP_MAX_COUNTER > AX81_1X12G12S_AP_MAX) ? AX81_1X12G12S_AP_MAX: __AP_MAX_COUNTER;
	fprintf(fd, "%d\n", board->board_ap_max_counter);
	sem_syslog_dbg("__AP_MAX_COUNTER = %d\n",__AP_MAX_COUNTER);
	#endif
	fclose(fd);
	return 0;
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
 *board ax81_2x12g12s hardware reset self
 *
 *
 */
 
int ax81_1x12g12s_hard_reset(void)
{
	int ret;
	sem_syslog_warning("board %s on slot %d self reset\n", local_board->name, local_board->slot_id);
	ret = sem_write_cpld(CPLD_HW_SELFT_RESET_REG, CPLD_SELF_RESET_DATA);
	return ret;
}
#if 0
/*
    load fpga....
    return:-1-->fail;0-->success.
*/
int load_fpga(void)
{
	int ret;
	char temp_buf[100] = "vtysh -c \"burning fpga /opt/bin/erd_full_version.bin\"";
 	sem_syslog_warning("load fpag logic ....\n");
    ret = system(temp_buf);
    if (WEXITSTATUS(ret))
    {
        return -1;
    }
	return 0;
}
#endif
/*
    load fpga....
    return:-1-->fail;0-->success.
*/
int load_fpga(void)
{
    int fd;
	int retval;
	fpga_file *arg = NULL;
    arg = (fpga_file *)malloc(sizeof(fpga_file));
	if(!arg)
	{
        sem_syslog_warning("load_fpga malloc failed!\n");
		return -1;
	}
	
	memset(arg,0,sizeof(fpga_file));
    arg->result = 7;//can't be zero
	strncpy(arg->fpga_bin_name,"/opt/bin/erd_full_version.bin",sizeof("/opt/bin/erd_full_version.bin"));

	fd = open(FPGA_BM_FILE_PATH,0);
	if (fd == -1)
	{
        printf("Open file:/dev/bm0 error!\n");
		free(arg);
	    arg = NULL;
		return -1;
	}
    
	retval = ioctl (fd,BM_IOC_FPGA_WRITE,arg);
	if (!retval)
	{
		
		sem_syslog_dbg("load_fpga ioctl success.return %d\n",arg->result);
	} 
	else 
	{
		sem_syslog_warning("load_fpga ioctl return %d failed!return %d\n",retval,arg->result);
	}

	close(fd);
	free(arg);
	arg = NULL;
	return retval;

}

/*
    load QT2025 firmware...
    return:-1-->fail;0-->success.
*/
int load_QT2025_firmware(void)
{
	int fd;
	int ret_cmd = -1;
	int ret;
    fd = open(SEM_BM_FILE_PATH, 0);
	if (fd<0)
	{
		sem_syslog_dbg("cann't open /dev/bm0, file %s is not opened\n", SEM_BM_FILE_PATH);
		return -1;
	}

	ret = ioctl(fd, BM_IOC_LOAD_QT2025_FIRMWARE, &ret_cmd);
	close(fd);
	if (ret)
	{
		sem_syslog_dbg("load QT2025 firmware ioctl operation fail.\n");
		return -1;
	}
	if(ret_cmd == 1)
	{		
    	return 0;
	}
	else
	{
	    return -1;
	}   
}
int test_fpga_work_normal(void)
{
	int ret;
    bm_op_args opt;
	memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;	
    opt.op_value = 0xffff;
    opt.op_addr = LED_CONTROL_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
	    sem_syslog_warning("**write LED_CONTROL_REG failure!\n");
	    return 0;
	}
	
    opt.op_len = 16;
	opt.op_addr = LED_CONTROL_REG;
	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**read LED_CONTROL_REG register 0 failure!\n");
	}
	
	if(opt.op_value == 0xffff)
	{
		//sem_syslog_warning("*****FPGA work normal! *****\n");
		return 1;
	}
}
#if 0
/*
 *
 *ret val: 0 for success; 1 for failed.
 */
void ax81_1x12g12s_download_fpga_QT(void)
{
	int ret;
	int count=1;
	int i;
	uint64_t system_state_reg_value = 0x0;
	bm_op_args opt;
	char chTmp[512] = {0};
	struct fpga_abnormal_tipc_s *fpga_abnormal_tipc = NULL;
	
	memset(&opt,0,sizeof(bm_op_args));
	fpga_abnormal_tipc = (struct fpga_abnormal_tipc_s *)malloc(sizeof(fpga_abnormal_tipc_t));
	sem_syslog_warning("thread %s, thread id is %d\n", __FUNCTION__, getpid());
/*
    ret = load_fpga();
	
	opt.op_len = 16;
    opt.op_addr = SYSTEM_STATE_REG_ADDR;
    ret = read_fpga_reg(&opt);
    if(ret)
    {
        sem_syslog_warning("**read SYSTEM_STATE_REG_ADDR register 0 failure!\n");
    }
    if(opt.op_value != 0xffff)
    {
        if((opt.op_value &0x1) == 0)
        {
            sem_syslog_warning("\n******WARNING:DDR2 chip 1 init fail!******\n");
    	}
    	if((opt.op_value & 0x2)>>1 == 0)
    	{
            sem_syslog_warning("\n******WARNING:DDR2 chip 2 init fail!******\n");
    	}
    	if((opt.op_value & 0x8)>>3 == 0)
    	{
            sem_syslog_warning("\n******WARNING:QDR init fail!******\n");
    	}
    }
	else
	{
        sem_syslog_warning("Load fpga logic success!\n");
	}
	
	sleep(1);
	
	sem_syslog_warning("loading QT2025 firmware ......\n");
    ret = load_QT2025_firmware();
	if(ret == 0)
	{   
        sem_syslog_warning("load QT2025 firmware success\n");
	}
    else
    {
        sem_syslog_warning("load QT2025 firmware failed\n"); 
	}
*/
	while(1)
	{
    	memset(&opt,0,sizeof(bm_op_args));
		ret = test_fpga_work_normal();
    	if(ret == 1)
    	{
            sleep(1);
			//sem_syslog_warning("*****FPGA is ok! *****\n");
    	}
		else
		{
		    fpga_abnormal_tipc->slot_id = local_board->slot_id+1;
	        memcpy(chTmp, fpga_abnormal_tipc, sizeof(fpga_abnormal_tipc_t));
            sem_tipc_send(product->active_master_slot_id, SEM_CMD_FPGA_ABNORMAL, chTmp, sizeof(fpga_abnormal_tipc_t));
			
			ret = load_fpga();
    		opt.op_len = 16;
            opt.op_addr = SYSTEM_STATE_REG_ADDR;
            ret = read_fpga_reg(&opt);
            if(ret)
            {
                sem_syslog_warning("**read SYSTEM_STATE_REG_ADDR register 0 failure!\n");
            }
            if(opt.op_value != 0xffff)
            {
                if((opt.op_value &0x1) == 0)
                {
                    sem_syslog_warning("\n******WARNING:DDR2 chip 1 init fail!******\n");
                }
                if((opt.op_value & 0x2)>>1 == 0)
                {
                    sem_syslog_warning("\n******WARNING:DDR2 chip 2 init fail!******\n");
                }
                if((opt.op_value & 0x8)>>3 == 0)
                {
                    sem_syslog_warning("\n******WARNING:QDR init fail!******\n");
                }
            }
            else
            {
                sem_syslog_warning("Reload fpga logic success!\n");
            }

            ret = test_fpga_work_normal();
            if(ret == 1)
            {
                sem_syslog_warning("*****FPGA is ok! *****\n");
				//fpga_abnormal_tipc->slot_id = local_board->slot_id+1;
			    //fpga_abnormal_tipc->count = count;
	            //memcpy(chTmp, fpga_abnormal_tipc, sizeof(fpga_abnormal_tipc_t));
                //sem_tipc_send(product->active_master_slot_id, SEM_CMD_FPGA_ABNORMAL, chTmp, sizeof(fpga_abnormal_tipc_t));
				sem_syslog_warning("*****FPGA reload times success!\n");
				continue;
			}
			else
			{
				fpga_abnormal_tipc->slot_id = local_board->slot_id+1;
			    //fpga_abnormal_tipc->count = count;
	            memcpy(chTmp, fpga_abnormal_tipc, sizeof(fpga_abnormal_tipc_t));
                sem_tipc_send(product->active_master_slot_id, SEM_CMD_FPGA_ABNORMAL, chTmp, sizeof(fpga_abnormal_tipc_t));
    			sem_syslog_warning("*****FPGA reload %d times fail!board reboot\n",SYSTEM_FPGA_RELOAD_COUNT);
                board_self_reboot();//board reboot
			}
		}
	}
}
#endif

/*
 *board self special initialize
 *
 *ret val: 0 for success 1 for failed.
 */
int ax81_1x12g12s_init(void)
{
	int ret;
	int flag = 0;
	bm_op_args opt;
	char chTmp[512] = {0};
	struct fpga_abnormal_tipc_s *fpga_abnormal_tipc = NULL;
	sem_syslog_dbg("begin ax81_1x12c12s init fpga and QT2025!\n");
	memset(&opt,0,sizeof(bm_op_args));
    ret = load_fpga();
	if(ret)
	{
        sem_syslog_dbg("load_fpga success.\n");
	}

    opt.op_len = 16;
    opt.op_addr = SYSTEM_STATE_REG_ADDR;
    ret = read_fpga_reg(&opt);
    if(ret)
    {
        sem_syslog_warning("**read SYSTEM_STATE_REG_ADDR register 0 failure!\n");
    }
    if(opt.op_value != 0xffff)
    {
        if((opt.op_value &0x1) == 0)
        {
            sem_syslog_warning("WARNING:DDR2 chip 1 init fail!\n");
    		flag = 1;
    	}
    	if((opt.op_value & 0x2)>>1 == 0)
    	{
            sem_syslog_warning("WARNING:DDR2 chip 2 init fail!\n");
    		flag = 2;
    	}
    	if((opt.op_value & 0x8)>>3 == 0)
    	{
            sem_syslog_warning("WARNING:QDR init fail!\n");
    		flag = 3;
    	}
    }
	
	sleep(1);
	
    ret = load_QT2025_firmware();
	if(ret == 0)
	{
        sem_syslog_warning("load QT2025 firmware success\n");
	}
    else
    {
        sem_syslog_warning("load QT2025 firmware failed\n");
		return 0;
	}
    sem_syslog_dbg("ax81_1x12c12s init fpga and QT2025 end.\n");
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
	int fd;
	FILE *fp;
	int ret;
	int i = 0;
	char tmp_mac_buffer[13];
	unsigned char temp_buffer[3] = {0x00, 0x00, 0x00};
	unsigned char mac_addr[6];
	char output_buffer[13];
	char buf[1024];
	char *name;

	if (product->product_type != XXX_YYY_AX8603 && product->product_type != XXX_YYY_SINGLE_BOARD) {
		return 0;
	}

	/* copy base mac file to other positions */
	system("/etc/init.d/product_mac_info_copy");

	/* get base mac from file, and write local mac to local mac files */
	fd = open("/devinfo/base_mac", O_RDONLY);
	if (fd < 0)
	{
		sem_syslog_dbg("open /devinfo/base_mac failed.\n");
		return -1;
	}

	memset(tmp_mac_buffer, 0x00, 13);
	ret = read(fd, tmp_mac_buffer, 12);

	sem_syslog_dbg("Base Mac : ");
	for (i=0; i<12; i+=2)
	{
		memcpy(temp_buffer, tmp_mac_buffer+i, 2);
		mac_addr[i/2] = (unsigned char)strtoul((char *)temp_buffer, 0, 16);
		sem_syslog_dbg_without_time("%x", mac_addr[i/2]);
	}
	sem_syslog_dbg_without_time("\n");

	close(fd);

	for (i = 0; i < 6; i++)
	{
		product->base_mac_addr[i] = mac_addr[i];
		product_info_syn.base_mac_addr[i] = mac_addr[i];
	}

	mac_addr[5] += local_board->slot_id;

	fp = fopen("/devinfo/local_mac", "w+");
	if (fp == NULL)
	{
		sem_syslog_dbg("open /devinfo/local_mac failed.\n");
		return -1;
	}

	fprintf(fp, "%02X%02X%02X%02X%02X%02X\n", mac_addr[0], mac_addr[1], mac_addr[2],\
		mac_addr[3], mac_addr[4], mac_addr[5]);

	fclose(fp);

	system("cp /devinfo/local_mac /dbm/local_board/mac");

	/* change mac addr of local interfaces */

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
		ret = set_mac_addr(name, mac_addr);
	}

	fclose(fp);

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

	if (product->product_type == XXX_YYY_SINGLE_BOARD) {
		/* copy product sn file to other positions */
		system("cp /devinfo/sn /devinfo/product_sn");
		system("cp /devinfo/sn /dbm/product/sn");

		/* to get product board sn */
		fd = open("/devinfo/sn", O_RDONLY);
		if (fd < 0)
		{
			sem_syslog_dbg("open /devinfo/sn failed.\n");
			return -1;
		}

		memset(tmp_sn_buffer, 0x00, 21);
		ret = read(fd, tmp_sn_buffer, 20);

		sem_syslog_dbg("Product Sn : ");
		for (i=0; i<20; i+=2)
		{
			memcpy(temp_buffer, tmp_sn_buffer+i, 2);
			product_sn[i/2] = (unsigned char)strtoul((char *)temp_buffer, 0, 16);
			product->sn[i/2] = product_sn[i/2];
			product_info_syn.sn[i/2] = product_sn[i/2];
			sem_syslog_dbg_without_time("%02x", product_sn[i/2]);
		}
		sem_syslog_dbg_without_time("\n");

		close(fd);
	}

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

static int set_port_admin_status(unsigned int port, unsigned int status)
{
	int fd;
	int ret_cmd = status;
	int ret;

	if(!((ret_cmd == 1)||(ret_cmd == 0)))
    {
        return SEM_WRONG_PARAM;
	}
	
    fd = open(SEM_BM_FILE_PATH, 0);
	if (fd<0)
	{
		sem_syslog_dbg("cann't open /dev/bm0, file %s is not opened\n", SEM_BM_FILE_PATH);
		return -1;
	}
	
	ret = ioctl(fd, BM_IOC_PORT_admin_status, &ret_cmd);
	close(fd);

	if (ret)
	{
		sem_syslog_dbg("ioctl operation fail.\n");
		return SEM_COMMAND_FAILED;
	}
	if(ret_cmd == 1)
	{
		sem_syslog_dbg("admin_status powerup success.\n");		
    	return SEM_COMMAND_SUCCESS;
	}
	else
	{
		sem_syslog_dbg("admin_status powerdown success.\n");
	    return -1;
	}   

}
static int set_ethport_mtu(int slot, int port, unsigned int mtu_size)
{
    return SEM_OPERATE_NOT_SUPPORT;
}

static int set_port_preferr_media(unsigned int port, unsigned int media)
{
    return SEM_OPERATE_NOT_SUPPORT;
}

static int set_ethport_duplex_autoneg( unsigned char port, unsigned long state)
{
	return SEM_OPERATE_NOT_SUPPORT;
}

static int set_ethport_fc_autoneg( unsigned char port, unsigned long state)
{
	return SEM_OPERATE_NOT_SUPPORT;
}
			

static int set_ethport_speed_autoneg( unsigned char port,unsigned long state)
{
	return SEM_OPERATE_NOT_SUPPORT;
}

static int set_ethport_flowCtrl( unsigned char port, unsigned long state)
{
	return SEM_OPERATE_NOT_SUPPORT;
}

static int set_port_autoneg_status( unsigned int port, unsigned int state)
{
	return SEM_OPERATE_NOT_SUPPORT;
}
/*
	attr_bitmap-->
		 bit 0 : admin state
	     bit 1 : link state   //only this is available.;
	     bit 2 : autoneg state
	     bit 3 : duplex mode
	     bit 4 : flow control
	     bit 5 : backpressure
	     bit 6 : autoneg-speed
	     bit 7 : autoneg-duplex
	     bit 8 : autoneg-fc 
	     bit 9 : autoneg ctrl
	     bit 12-15 : speed
	     bit 16-19 : pluggable port status
	     bit 24-27 : basic functions
	     bit 28-29 : preferred media
*/
static int read_eth_port_info( int slot, int port, struct eth_port_s *port_info)
{

    unsigned int temp = 0;
	int val;
	
	if(port == 24)
	{
        val = sem_bm_phyQT2025_read(0x3, 0xd7fd);
    	sem_syslog_dbg("\nread_eth_port_info QT 3.d7fd = %x\n",val);		

    	if(val == 0x70)
    	{
            temp = 0x3003;
    	}
    	else
    	{
            temp = 0x3001;
    	}
    	port_info->attr_bitmap = temp;
    	port_info->mtu = 1518;
	}
	return SEM_COMMAND_SUCCESS;
}
static int ax81_1x12g12s_show_system_environment(env_state_t *env_state)
{
	int master_temp1_input, master_temp2_input;
	int slave_temp2_input, slave_temp1_input;

    unsigned long power_state = 0;
    int intState2,intState3;	
    int fan_rpm, rpmVal;
    int read_value;
    
    
    /* Fan RPM */
    if(sem_read_cpld(CPLD_FAN_RPM, &fan_rpm) < 0)
    	return -1;
    switch(fan_rpm & 0xff)
    {
    	case 0x01: 
    		rpmVal = FAN_125_RPM;break;
    	case 0x02: 
    		rpmVal = FAN_250_RPM;break; 	
    	case 0x04: 
    		rpmVal = FAN_375_RPM;break; 		
    	case 0x08: 
    		rpmVal = FAN_500_RPM;break; 		
    	case 0x10: 
    		rpmVal = FAN_625_RPM;break;
    	case 0x20: 
    		rpmVal = FAN_750_RPM;break; 	
    	case 0x40: 
    		rpmVal = FAN_875_RPM;break; 		
    	default: 
    		rpmVal = FAN_1000_RPM;
    		break;				
    }
    env_state->fan_rpm = rpmVal;
    sem_syslog_dbg("\tfan_rpm = %#x\n", fan_rpm);
    
    
    
    /* Fan state */
    if(sem_read_cpld(CPLD_INTERRUPT_RESULT_REG2, &intState2) < 0)
    	return -1;
    /* Fan module remove alse disposed as alarm*/
    env_state->fan_state = (((intState2 >> 2) & 1) & (((intState2 >> 1) & 1) ^ 1));
    sem_syslog_dbg("\tintState2 = %#x\n", intState2);
    
    /* Power supply */
    
    if(sem_read_cpld(CPLD_INTERRUPT_RESULT_REG3, &intState3) < 0){		
    	return -1;
    }
    
    sem_syslog_dbg("\tintState3 = %#x\n", intState2);
    /*The hardware can only distinguish power "ON" or "OFF"*/
    power_state |= ((intState3 ) & 1) + 1;
    power_state |= (((intState3 >> 1) & 1) + 1) << 4;
    env_state->power_state = power_state;
	

	/* CPU temperature */	
	if ((get_num_from_file(SLAVE_TEMP1_INPUT, &master_temp1_input) >= 0)
			&&(get_num_from_file(SLAVE_TEMP2_INPUT, &master_temp2_input) >= 0)){

		env_state->master_inter_temp = master_temp2_input/1000;
		env_state->master_remote_temp = master_temp1_input/1000;
		
	} else {
		sem_syslog_dbg("ax81_1x12g12s read cpu temperature error! \n");
	} 
	
	if ((get_num_from_file(TMP421_SLAVE_TEMP1_INPUT, &master_temp1_input) >= 0)
					&&(get_num_from_file(TMP421_SLAVE_TEMP2_INPUT, &master_temp2_input) >= 0)) {

		env_state->slave_inter_temp = master_temp2_input/1000;
		env_state->slave_remote_temp = master_temp1_input/1000;			
		
	} else {	
		sem_syslog_dbg("ax81_1x12g12s read fpga  temperature error! \n");
	}

	return 0;
}

board_fix_param_t ax81_1x12g12s =
{
	.board_code = AX81_1X12G12S_BOARD_CODE,
	.board_type = BOARD_TYPE_AX81_1X12G12S,
	.board_id = BOARD_SOFT_ID_AX81_1X12G12S,
	.name = "AX81_1X12G12S",
	.slot_id = UNKONWN_SLOT_ID,
	.is_master = NOT_MASTER,
	.cpu_num = SINGLE_CPU,
	.port_num_on_panel = 25,
	.asic_port_num = 24,
	.asic_start_no = 1,
	.port_arr = {{ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1},\
	             {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1},\
                 {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1},\
	             {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1},\
				 {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1},\
	             {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1}, {ASIC,-1,-1,-1,-1,-1},\
                 {ASIC,-1,-1,-1,-1,-1}, {CPU,ETH_XGE_FIBER,-1,-1,-1,AX81_1X12G12S_ETHPORT_DEFAULT_ATTR}},
	.function_type = AC_BOARD,
	.obc_port_num = 2,
	.obc_port = {{CPU, 0, 12, "obc0", OBC_PORT, NOT_CSCD_PORT, 0}, 
				 {CPU, 0, 13, "obc1", OBC_PORT, NOT_CSCD_PORT, 1}},
	.cscd_port_num = 2,
	.cscd_port = {{ASIC, 1, 12, "cscd0", NOT_OBC_PORT, CSCD_PORT, 0}, 
				  {ASIC, 1, 13, "cscd1", NOT_OBC_PORT, CSCD_PORT, 1}},
	.board_state = BOARD_INSERTED,
	.board_ap_max_counter = BOARD_DEFAULT_AP_COUNTER,
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
	.hard_self_reset = ax81_1x12g12s_hard_reset,
	.board_self_init = ax81_1x12g12s_init,
	.init_local_mac_info = init_local_mac_info,
	.update_local_mac_info = update_local_mac_info,
	.init_local_sn_info = init_local_sn_info,
	.set_port_preferr_media = set_port_preferr_media,
	.set_port_admin_status = set_port_admin_status,
	.set_port_autoneg_status = set_port_autoneg_status,
	.set_ethport_mtu = set_ethport_mtu,
	.read_eth_port_info = read_eth_port_info,
	.set_ethport_flowCtrl = set_ethport_flowCtrl,
	.show_system_environment = ax81_1x12g12s_show_system_environment
};


#ifdef __cplusplus
}
#endif
