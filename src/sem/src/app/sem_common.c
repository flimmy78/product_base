#ifdef __cplusplus
extern "C"
{
#endif

#include <linux/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <syslog.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <linux/tipc.h>
#include <errno.h> 


#include "sem/sem_tipc.h"
#include "sem_common.h"
#include "board/netlink.h"
#include "sem/product.h"
#include "product/board/board_feature.h"
#include "product/product_feature.h"

int fd = -1;
pthread_t snapshot_thread;
extern board_fix_param_t *local_board;
extern product_fix_param_t *product;

//hot reboot send signal
int sem_send_signal_to_trap_debug(void)
{
   	int ret=0;
	int fd;
	char c;
	int i;
	char msgbuf[512] = {0};
    int msgLen = 0;
    nl_msg_head_t *head = (nl_msg_head_t *)msgbuf;
    netlink_msg_t *nl_msg =  (netlink_msg_t *)(msgbuf + sizeof(nl_msg_head_t));

    head->type = OVERALL_UNIT;
	head->object = COMMON_MODULE;
	head->pid = getpid();
    head->count = 1;
	
    nl_msg->msgType = BOARD_STATE_NOTIFIER_EVENT;
	nl_msg->msgData.boardInfo.action_type = BOARD_RUNNING;
	nl_msg->msgData.boardInfo.slotid = local_board->slot_id+1;
	msgLen = sizeof(nl_msg_head_t) + head->count*sizeof(netlink_msg_t);
	
	for(i = 0; i < product->slotcount; i++)
	{
	   if (product->board_on_mask>>i & 0x1)
	   {
	   	    if(i != product->active_master_slot_id)
            {
                ret = sem_tipc_send(i, SEM_CMD_SEND_STATE_TRAP_REBOOT, msgbuf, msgLen);
    			if(ret)
    			{
                    sem_syslog_warning("sem_tipc_send SEM_CMD_SEND_STATE_TRAP_REBOOT slot %d failed.ret =%d\n",++i,ret);
    			}
    			else
    			{
                    //sem_syslog_warning("sem_tipc_send SEM_CMD_SEND_STATE_TRAP_REBOOT slot %d success.ret =%d\n",++i,ret);
    			}
	   	    }
			else
			{
                ret = sem_dbus_system_state_trap(SYSTEM_REBOOT);
                if(ret){
                    sem_syslog_warning("sem_dbus_system_state_trap SYSTEM_REBOOT failed.\n");
                }
			}
	   }
	}
	return 0;
}

//hot reboot send signal
int sem_send_signal_to_trap_reboot(void)
{
   	int ret=0;
	#if 0
	int fd;
	char c;
	int i;
	char msgbuf[512] = {0};
    int msgLen = 0;
    nl_msg_head_t *head = (nl_msg_head_t *)msgbuf;
    netlink_msg_t *nl_msg =  (netlink_msg_t *)(msgbuf + sizeof(nl_msg_head_t));

    head->type = OVERALL_UNIT;
	head->object = COMMON_MODULE;
	head->pid = getpid();
    head->count = 1;
	
    nl_msg->msgType = BOARD_STATE_NOTIFIER_EVENT;
	nl_msg->msgData.boardInfo.action_type = BOARD_RUNNING;
	nl_msg->msgData.boardInfo.slotid = local_board->slot_id+1;
	msgLen = sizeof(nl_msg_head_t) + head->count*sizeof(netlink_msg_t);
	
	for(i = 0; i < product->slotcount; i++)
	{
	   if (product->board_on_mask>>i & 0x1)
	   {
	   	    if(i != product->active_master_slot_id)
            {
                ret = sem_tipc_send(i, SEM_CMD_SEND_STATE_TRAP_REBOOT, msgbuf, msgLen);
    			if(ret)
    			{
                    sem_syslog_warning("sem_tipc_send SEM_CMD_SEND_STATE_TRAP_REBOOT slot %d failed.ret =%d\n",++i,ret);
    			}
    			else
    			{
                    //sem_syslog_warning("sem_tipc_send SEM_CMD_SEND_STATE_TRAP_REBOOT slot %d success.ret =%d\n",++i,ret);
    			}
	   	    }
	   }
	}
	#endif
    ret = sem_dbus_system_state_trap(SYSTEM_REBOOT);
	if(ret){
        sem_syslog_warning("sem_dbus_system_state_trap SYSTEM_REBOOT failed.\n");
	}

	return 0;
}

//cool reboot send signal.
void sem_send_signal_to_trap(void)
{
	int ret=0;
	int fd;
	char c;
	int i;
	char msgbuf[512] = {0};
    int msgLen = 0;
    nl_msg_head_t *head = (nl_msg_head_t *)msgbuf;
    netlink_msg_t *nl_msg =  (netlink_msg_t *)(msgbuf + sizeof(nl_msg_head_t));

    head->type = OVERALL_UNIT;
	head->object = COMMON_MODULE;
	head->pid = getpid();
    head->count = 1;
	
    nl_msg->msgType = BOARD_STATE_NOTIFIER_EVENT;
	nl_msg->msgData.boardInfo.action_type = BOARD_RUNNING;
	nl_msg->msgData.boardInfo.slotid = local_board->slot_id+1;
	msgLen = sizeof(nl_msg_head_t) + head->count*sizeof(netlink_msg_t);
	
	for(i = 0; i < product->slotcount; i++)
	{
       if(i != product->active_master_slot_id)
       {
            sem_tipc_send(i, SEM_CMD_SEND_STATE_TRAP, msgbuf, msgLen);
	   }
	}
	
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
	
}

int sem_sor_exec(char* cmd,char* filename,int time)
{	    
    char cmdstr[512];
	int ret;
	sprintf(cmdstr,"sor.sh %s %s %d ",cmd,filename,time);
	ret = system(cmdstr);	
	switch (WEXITSTATUS(ret)) {
		case 0:
			return 0; 	
		case 1: 		
			sem_syslog_dbg("sor.sh :Sysetm internal error (1).\n");
			break;		
		case 2: 		
			sem_syslog_dbg("sor.sh :Sysetm internal error (2).\n");
			break;		
		case 3: 		
			sem_syslog_dbg("sor.sh :Storage media is busy.\n");
			break;		
		case 4: 		
			sem_syslog_dbg("sor.sh :Storage operation time out.\n");	
			break;		
		case 5: 		
			sem_syslog_dbg("sor.sh :No left space on storage media.\n");
			break;		
		default:			
			sem_syslog_dbg("sor.sh :Sysetm internal error (3).\n");			
			break;		
			}	
	return -1;
}

void test_CF_space(int *space_flag)
{
	char cf_space_buf[10] = {0};
	int cf_space=0;
	char *endptr=NULL;
	int ret =0;
	int i=0;
	int fd;

	system("sudo mount /blk");
    system("df -lh |grep blk|awk '{print $5}' >& /mnt/cf_card_space");

	fd = open("/mnt/cf_card_space", O_RDONLY);
	if (fd < 0) {
		sem_syslog_warning("get log failed:/mnt/cf_card_space.\n");
		sem_syslog_warning("errno=%d\n",errno); 

		/*A backup plan*/
		#if 1
	    system("dd if=/dev/zero of=/home/mnt/TestBlkSpace  bs=1M count=80");
    	ret = sem_sor_exec("cp","TestBlkSpace",50);
    	if(ret){
    		if(ret == 5){
    			*space_flag=1;
                sem_syslog_warning("Warning:The space of /blk is less than 80M for IMG.Please clean the CF card!!!\n");
    		}
    	}
        system("rm /home/mnt/TestBlkSpace");
    	ret = sem_sor_exec("rm","TestBlkSpace",50);
		#endif
		/*end*/
	} else {
		read(fd, cf_space_buf, CF_SPACE_BUF_LEN);
	    sem_syslog_dbg("the cf card Use:%s \n",cf_space_buf);
		close(fd);
		
		if(cf_space_buf[0] == '\0'){
            sem_syslog_dbg("/mnt/cf_card_space is empty file !!!\n");
    		#if 1
    	    system("dd if=/dev/zero of=/home/mnt/TestBlkSpace  bs=1M count=80");
        	ret = sem_sor_exec("cp","TestBlkSpace",50);
        	if(ret){
        		if(ret == 5){
        			*space_flag=1;
                    sem_syslog_warning("Warning:The space of /blk is less than 80M for IMG.Please clean the CF card!!!\n");
        		}
        	}
            system("rm /home/mnt/TestBlkSpace");
        	ret = sem_sor_exec("rm","TestBlkSpace",50);
    		#endif
	    }else{
    		for(;i<10;i++){
                if(cf_space_buf[i] == '%'){
    				cf_space_buf[i]='\0';
                }
    		}
			//sem_syslog_dbg("the cf card Use:%s \n",cf_space_buf);
            cf_space=strtoul(cf_space_buf, &endptr,10);
			sem_syslog_dbg("the cf card Use:%d \n",cf_space);
    		if(cf_space >= 85){
                *space_flag=1;
    		}
	    }
	}
	system("rm /mnt/cf_card_space");
}

int sem_read_cpld(int reg, int *read_value)
{
	bm_op_cpld_args read;
	int ret;

	if (fd<0)
	{
		sem_syslog_dbg("cann't read cpld, beacuse file %s is not opened\n", SEM_BM_FILE_PATH);
		return -1;
	}
	
	read.slot = 0;
	read.regAddr = reg;
	read.regData = 0;
	
	ret = ioctl(fd, BM_IOC_CPLD_READ, &read);

	if (ret)
	{
		sem_syslog_dbg("ioctl call failed.read cpld reg %d failed\n", reg);
		return -1;
	}

	*read_value = read.regData;
	
	return 0;
}


int sem_write_cpld(int reg, int write_value)
{
	int ret;
	bm_op_cpld_args write;

	if (fd<0)
	{
		sem_syslog_dbg("cann't write cpld reg, file %s is not opened\n", SEM_BM_FILE_PATH);
		return -1;
	}

	write.slot = 0;
	write.regAddr = reg;
	write.regData = (unsigned char)(write_value&0xff);

	ret = ioctl(fd, BM_IOC_CPLD_WRITE, &write);

	if (ret)
	{
		sem_syslog_dbg("ioctl call failed.write value %d to cpld reg %d failed\n", write_value&0xf, reg);
		return -1;
	}
	return 0;	
}

int open_fd()
{
	fd = open(SEM_BM_FILE_PATH, 0);

	if (fd <= 0)
	{
		sem_syslog_dbg("file %s open failed\n", SEM_BM_FILE_PATH);
		return -1;
	}

	return  0;
}

void close_fd()
{
	close(fd);
}

int abort_check_product_state_reboot()
{
	int fd;
	char c;
	fd = open(ABORT_CHECK_PRODUCT_STATE_REBOOT_PATH, O_RDONLY);
	if(fd < 0)
	{
		sem_syslog_dbg("file %s open failed\n", ABORT_CHECK_PRODUCT_STATE_REBOOT_PATH);
		return -1;
	}
	read(fd, &c, 1);
	close(fd);
	if(c == '1')
		return 0;
	return 1;
}

int abort_sync_version()
{
	int fd;
	char c;
	fd = open(ABORT_SYNC_VERSION_PATH, O_RDONLY);
	if(fd < 0)
	{
		sem_syslog_dbg("file %s open failed\n", ABORT_SYNC_VERSION_PATH);
		return -1;
	}
	read(fd, &c, 1);
	close(fd);
	if(c == '1')
		return 0;
	return 1;
}

char *get_start(char *str)
{
	while(1)
	{
		if (*str == ' ')
		{	
			str++;
			break;
		}
		else
		{
			str++;
			continue;
		}
	}
	return str;
}

void send_sync_failed_msg(board_fix_param_t *board)
{
	sem_pdu_head_t *head;
	sem_tlv_t *tlv_head;
	char send_buf[SEM_TIPC_SEND_BUF_LEN];
	
	head = (sem_pdu_head_t *)send_buf;
	head->slot_id = board->slot_id;
	head->version = 11;
	head->length = 22;
	
	tlv_head = (sem_tlv_t *)(send_buf + sizeof(sem_pdu_head_t));
	tlv_head->type = SEM_SOFTWARE_VERSION_SYNC_FAILED;
	tlv_head->length = 33;
	
	if (sendto(g_send_sd, send_buf, sizeof(sem_pdu_head_t)+sizeof(sem_tlv_t)+ sizeof(software_version_request_t), 0, (struct sockaddr*)&g_send_sock_addr, sizeof(struct sockaddr_tipc)) < 0)
	{
		sem_syslog_warning("send to active MCB SEM_SOFTWARE_VERSION_SYNC_FAILED failed\n");
	}
	else
	{
		sem_syslog_dbg("send to active MCB SEM_SOFTWARE_VERSION_SYNC_FAILED success\n");
	}
}

int check_software_version_md5(char *new_version_name)
{
	int fd_old, fd_new;
	char str[120];
	char md5_old[128], md5_new[128];
	char *str_old, *str_new;

	sprintf(str, "sudo /usr/bin/sor.sh imgmd5 /blk/%s 120 > /mnt/md5_new", new_version_name);
	system(str);
	fd_new = open("/mnt/md5_new", O_RDWR);
	if(fd_new < 0)
	{
		sem_syslog_dbg("file %s open failed.%s\n", "/mnt/md5_new", __FUNCTION__);
		return -1;
	}
	read(fd_new, md5_new, 128);
	close(fd_new);
	fd_old = open("/mnt/md5", O_RDWR);
	if(fd_old < 0)
	{
		sem_syslog_dbg("file %s open failed.%s\n", "/mnt/md5", __FUNCTION__);
		return -1;
	}
	read(fd_old, md5_old, 128);
	close(fd_old);
	str_old = get_start(md5_old);
	str_new = get_start(md5_new);
	str_old[32] = '\0';
	str_new[32] = '\0';
	if (strcmp(str_new, str_old)==0)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

int check_free_space(char *name)
{
	
	int fd, fd_name;
	char buf[15];
	char buf_name[25];
	char str[70];
	sprintf(str, "ls --block-size=1 -s /mnt/%s > /mnt/imgsize", name);
	system(str);
	fd_name = open("/mnt/imgsize", O_RDONLY);
	if (fd_name < 0)
	{
		sem_syslog_dbg("open file /mnt/imgsize failed\n");
		return -1;
	}
	read(fd_name, buf_name, 25);
	close(fd_name);
	fd = open("/var/run/sad/freespace", O_RDONLY);
	if (fd < 0)
	{
		sem_syslog_dbg("open file /var/run/sad/freespace failed\n");
		return -1;
	}
	read(fd, buf, 15);
	close(fd);
	if (atoi(buf)<atoi(buf_name))
	{
		sem_syslog_warning("***freespace[%d]<imgspace[%d]***not enough space, please release some space and try again\n", atoi(buf), atoi(buf_name));
		return -1;
	}
	return 0;
}
/*
 *to reset the environment param bootfile
 *
 */
int reset_bootfile(char *filename)
{
	int retval; 
	boot_env_t	env_args={0};	
	char *name = "bootfile";

	sem_syslog_dbg("filename is %s\n", filename);
	
	sprintf(env_args.name, name);
	sprintf(env_args.value, filename);
	env_args.operation = SAVE_BOOT_ENV;
	
	if(fd < 0)
	{	
		sem_syslog_dbg("fd error\n");
		return 1;	
	}		
	retval = ioctl(fd, BM_IOC_ENV_EXCH, &env_args);

	if(retval)
	{	
		sem_syslog_dbg("ioctl save bootfile failed.");
		return 1;	
	}
	sem_syslog_dbg("bootfile save successed\n");
	return 0;	
}


int sem_thread_create(pthread_t *new_thread, void * (*thread_fun)(void *), void *arg, int is_detached)
{
	pthread_attr_t attr;
	int s ;
	
	if (is_detached)
	{
		s = PTHREAD_CREATE_DETACHED;
	}
	else
	{
		s = PTHREAD_CREATE_JOINABLE;
	}
	
	if (!new_thread)
	{
		sem_syslog_dbg("new_thread is null\n");
		return SEM_THREAD_CREATE_FAILED;
	}
	pthread_attr_init(&attr);

	pthread_attr_setdetachstate(&attr, s);

	if (pthread_create(new_thread, &attr, thread_fun, arg) != 0)
	{
		sem_syslog_dbg("thread create failed\n");
		return SEM_THREAD_CREATE_FAILED;
	}

	return SEM_THREAD_CREATE_SUCCESS;	
}

/*******************************************************************************
* FUNCTION :    sem_init_completed
* DESCRIPTION:  This function is used by SEM to tell rc script loader about 
                initialization process done.
* INPUTS:  None.
* OUTPUTS: None.
* RETURNS: None.
* COMMENTS:
*******************************************************************************/
void sem_init_completed(void)
{
	/* SEM initialization stage file descriptor */
	int sem_stage_fd = -1;
	unsigned char buf[16] = {0};

	sem_stage_fd = open(SEM_INIT_STAGE_FILE_PATH, O_CREAT | O_RDWR | O_TRUNC);
	if(sem_stage_fd < 0) {
		sem_syslog_dbg("open file %s error when tell init stage end\n",	\
				SEM_INIT_STAGE_FILE_PATH);
		return;
	}

	sprintf(buf, "%d", 1);
	
	write(sem_stage_fd, buf, strlen(buf));

	close(sem_stage_fd);	
	sem_syslog_dbg("SEM tell init stage to %s ok\n", SEM_INIT_STAGE_FILE_PATH);
	
	return;
}
void take_snapshot(void)
{
	sem_syslog_warning("Taking snapshot......\n");
	system("takesnapshot.sh 1 3");
}
/**
  * take snapshot before product reset
  * return 0 : success 
  * caojia 20110921
  */
int snapshot_before_reset(void)
{
	sem_syslog_warning("Mounting /blk......\n");
	system("sudo mount /blk");
	usleep(500000);
	
	if (sem_thread_create(&snapshot_thread, (void *)take_snapshot, NULL, IS_DETACHED) != SEM_THREAD_CREATE_SUCCESS)
    {
    	sem_syslog_warning("create thread take_snapshot failed.\n");
    }

	sleep(60);
	
	sem_syslog_warning("Syncing......\n");
	system("sync");
	sem_syslog_warning("Ready to reset.\n");
	//sem_syslog_dbg("Ready to reset.\n");

	return 0;
}

int get_num_from_file(char *filename, int *value)
{
	int fd;
	char buff[16] = {0};

	if(filename == NULL)
	{
		return -1;
	}

	fd = open(filename, O_RDONLY);
	if (fd >= 0) 
	{
		if(read(fd, buff, 16) < 0) 
		sem_syslog_dbg("Read error : no value\n");
		close(fd);
	}
	else
	{
		return -1;
	}

	*value = strtoul(buff, NULL, 10);
	return 0;
}

void set_dbm_effective_flag(int flag)
{
	FILE* fd;
	dbm_effective_flag = flag;
	fd = fopen("/dbm/dbm_effective_flag", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/dbm_effective_flag failed\n");
		return;
	}
	else
	{
		fprintf(fd, "%d\n", dbm_effective_flag);
		fclose(fd);
	}
}

#ifdef __cplusplus
}
#endif
