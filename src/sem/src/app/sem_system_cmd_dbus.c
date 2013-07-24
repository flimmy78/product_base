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
#include <linux/if.h>
#include <linux/if_arp.h>
#include <string.h>
#include <sys/ioctl.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <dbus/dbus.h>
#include <sys/syslog.h>
#include <linux/tipc.h>
#include <netdb.h>
#include <stdlib.h>

#include "sysdef/returncode.h"
#include "sysdef/sem_sysdef.h"
#include "cvm/ethernet-ioctl.h"
#include "dbus/sem/sem_dbus_def.h"
#include "sem_dbus.h"
#include "sem_common.h"
#include "sem/sem_tipc.h"
#include "product/board/board_feature.h"
#include "product/product_feature.h"
#include "board/board_define.h"
#include "sem_system_cmd_dbus.h"
#include "board/netlink.h"

#include "mtd-user.h"
#include "jffs2-user.h"

extern board_fix_param_t *local_board;
extern product_fix_param_t *product;
extern board_info_syn_t board_info_to_syn;
extern product_info_syn_t product_info_syn;

extern thread_index_sd_t thread_id_arr[MAX_SLOT_NUM];
extern sd_sockaddr_tipc_t g_send_sd_arr[MAX_SLOT_NUM];

extern DBusConnection *dcli_dbus_connection;

extern int disable_keep_alive_time;
extern int disable_keep_alive_flag_time;
extern int disable_keep_alive_start_time;

unsigned long long set_system_img_done_mask = 0x0ull;
int set_system_img_board_mask = 0;

int sor_checkfastfile(char* fastname)
{
    char result_file[64][128];		
	int ret,i,fastnum;
	FILE *fp = 	NULL;
	char cmdstr[128] = {0};
	char fastnamels[128] = {0}; 
	
	sprintf(fastnamels, "/blk/%s", fastname);    	
    sprintf(cmdstr,"sor.sh ls %s 120",fastname);	
	fp = popen( cmdstr, "r" );	
	if(fp)
	{
		i=0;
		while(i<64 && fgets( result_file[i], sizeof(result_file[i]), fp ))
			i++;
		fastnum=i;  		
		ret = pclose(fp);	
		switch (WEXITSTATUS(ret)) {
			case 0:
				for(i=0;i<fastnum;i++)
				{					if(!strncasecmp(result_file[i],fastnamels,strlen(fastnamels)))
						return 0;
				}
				return -2;
			default:
				return WEXITSTATUS(ret);
			}
	}
	else
		return -3;
}

int sor_checkimg(char* imgname)
{
	char result_file[64][128];
	char *cmdstr = "sor.sh imgls imgls 180";
	int ret,i,imgnum;
	FILE *fp = 	NULL;
	

	fp = popen( cmdstr, "r" );
	if(fp)
	{
		i=0;
		while(i<64 && fgets( result_file[i], sizeof(result_file[i]), fp ))
			i++;
		imgnum=i;

		ret = pclose(fp);
		
		switch (WEXITSTATUS(ret)) {
			case 0:
				for(i=0;i<imgnum;i++)
				{
					if(!strncasecmp(result_file[i],imgname,strlen(imgname)))
						return 0;
				}
				return -1;
			default:
				return WEXITSTATUS(ret);
			}
	}
	else
		return -2;
}

int set_boot_img_name(char* imgname)
{
	int fd; 
	int retval; 
	boot_env_t	env_args={0};	
	char *name = "bootfile";
	
	sprintf(env_args.name,name);
	sprintf(env_args.value,imgname);
	env_args.operation = SAVE_BOOT_ENV;

	fd = open("/dev/bm0",0);	
	if(fd < 0)
	{ 	
		return 1;	
	}		
	retval = ioctl(fd,BM_IOC_ENV_EXCH,&env_args);

	if(retval == -1)
	{	
	
		close(fd);
		return 2;	
	}
	close(fd);
	return 0;	
}



/*
 * set boot img on local board
 * caojia 20110829
 */
DBusMessage *sem_dbus_set_boot_img(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter	 iter;
	DBusError err;
	int slot_id;
	char *version_file;
	char file_path[64] = "/blk/";
	int ret = 0;
	int timeout = 3;

	/* if product is a distributed system */
	if (!product->is_distributed)
	{
		ret = 1;
		reply = dbus_message_new_method_return(msg);
	
		dbus_message_iter_init_append (reply, &iter);
	
		dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
		return reply;
	}

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args(msg, &err,
								DBUS_TYPE_UINT32, &slot_id,
								DBUS_TYPE_STRING, &version_file,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		ret = 1;
		reply = dbus_message_new_method_return(msg);
	
		dbus_message_iter_init_append (reply, &iter);
	
		dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
		return reply;
	}

	strcat(file_path, version_file);
	sem_syslog_dbg("sem_dbus_set_boot_img slot_id : %d\n", slot_id);
	sem_syslog_dbg("sem_dbus_set_boot_img version file : %s\n", version_file);

	if (slot_id == local_board->slot_id)
	{
		ret = 1;
		reply = dbus_message_new_method_return(msg);
	
		dbus_message_iter_init_append (reply, &iter);
	
		dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
		return reply;
	}

	while (timeout--)
	{
		ret = sor_checkimg(version_file);
		if(ret == 0 ){
			ret = set_boot_img_name(version_file);
			if( 0 == ret)
			{
				sem_syslog_dbg("Set boot img success\n");
				ret = 0;
			}
			else
			{
				ret = 1;
				sem_syslog_dbg("Config boot_img failed\n",ret);
			}

			break;
		}
		else
		{
			ret = 2;
		}
		usleep(500000);
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);

	return reply;
}

int sem_set_boot_img(char *img_name)
{
	int ret = 0;

	ret = sor_checkimg(img_name);
	if(ret == 0 ){
		ret = set_boot_img_name(img_name);
		if( 0 == ret)
		{
			sem_syslog_dbg("Set boot img success\n");

			return ret;
		}
		else
		{
			ret = 0xf;
			sem_syslog_dbg("Config boot_img failed\n",ret);

			return ret;
		}

	}
	else if (ret == -1)
	{
		ret = 6;
 		return 6;
	}
	else if (ret < -1 || ret > 0xf)
	{
		ret = 0xf;
		
		return ret;
	}
	else
	{
		return ret;
	}

	return ret;
}

int set_system_img_msg_send(int slot_id, char *img_name)
{
	sem_pdu_head_t * head;
	sem_tlv_t *tlv_head;
	char *img_head = NULL;
	
	board_info_syn_t *board_info_head;
    char send_buf[SEM_TIPC_SEND_BUF_LEN];

	memset(send_buf, 0, SEM_TIPC_SEND_BUF_LEN);
	head = (sem_pdu_head_t *)send_buf;
	head->slot_id = local_board->slot_id;
	head->version = 11;
	head->length = 22;

	tlv_head = (sem_tlv_t *)(send_buf + sizeof(sem_pdu_head_t));
	tlv_head->type = SEM_SET_SYSTEM_IMG;
	tlv_head->length = 33;

	img_head = (char *)(send_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));

	strcpy(img_head, img_name);

	if (sendto(g_send_sd_arr[slot_id].sd, send_buf, SEM_TIPC_SEND_BUF_LEN, 0, \
		(struct sockaddr*)&g_send_sd_arr[slot_id].sock_addr, sizeof(struct sockaddr_tipc)) < 0)
	{
		sem_syslog_warning("send set_system_img_msg to slot %d failed\n", slot_id + 1);
		return 1;
	}
	else
	{
		sem_syslog_dbg("send set_system_img_msg to slot %d succeed\n", slot_id + 1);
	}

	return 0;
}

/*
 * show bootrom environment variable
 * niehy@autelan.com 2012-09-18
 */
int set_boot_env_var_name(char* vername,char* value)
{
	int fd; 
	int retval; 
	boot_env_t	env_args={0};	
	char *name = vername;
	char *env_value = value;
	
	sprintf(env_args.name,name);
	sprintf(env_args.value,env_value);
	env_args.operation = SAVE_BOOT_ENV;

	fd = open("/dev/bm0",0);	
	if(fd < 0)
	{ 	
		return 1;	
	}		
	retval = ioctl(fd,BM_IOC_ENV_EXCH,&env_args);

	if(retval == -1)
	{	
	
		close(fd);
		return 2;	
	}
	close(fd);
	return 0;	
}

int get_boot_env_var_name(char* vername)
{
	int fd; 
	int retval; 
	boot_env_t	env_args={0};	
	char *name = vername;
	
	sprintf(env_args.name,name);
	env_args.operation = GET_BOOT_ENV;

	fd = open("/dev/bm0",0);	
	if(fd < 0)
	{ 	
		return 1;	
	}		
	retval = ioctl(fd,BM_IOC_ENV_EXCH,&env_args);

	if(retval == -1)
	{		
	
		close(fd);
		return 2;	
	}
	else
	{		
		sprintf(vername,env_args.value); 
	}
	close(fd);
	return 0;

}

DBusMessage *sem_dbus_set_boot_env_var(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter	 iter;
	DBusError err;

	char *envname;
	char *input_value;
	int ret = 0;
    char file_path[200] = {0};
	int value,board_id;
	
	dbus_error_init(&err);
	if (!(dbus_message_get_args(msg, &err,
								DBUS_TYPE_STRING, &envname,
							    DBUS_TYPE_STRING, &input_value,	
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
	}
	sem_syslog_dbg("Set environment variable : %s\n", envname);

	sem_syslog_dbg("Input value : %s  \n",input_value);
    sprintf(file_path,"/dbm/local_board/board_id");

    if(strcmp(envname,"sefile") == 0)
	{
        value = get_product_board_id(file_path, &board_id);
		if(value)
		{
			sem_syslog_dbg("Get product board name failure\n");
		}
		if(board_id == BOARD_SOFT_ID_AX7605I_2X12G12S) /*AX71_2x12g12s*/
		{
            sem_syslog_dbg("this board type  doesn't support fastfwd\n");
    	    ret = -1;
		}
		else if(board_id == BOARD_SOFT_ID_AX81_SMU)/*AX81_SMU*/
		{
            sem_syslog_dbg("this board type  doesn't support fastfwd\n");
			ret = -1;
		}
		else if(board_id == BOARD_SOFT_ID_AX81_12X)/* AX81_12x */
		{
            sem_syslog_dbg("this board type  doesn't support fastfwd\n");
			ret = -1;        
		}
		else if(board_id == BOARD_SOFT_ID_AX81_2X12G12S)/*AX81_2x12g12s*/
		{
            sem_syslog_dbg("this board type  doesn't support fastfwd\n");
			ret = -1;          
		}		
	}
	if (ret == 0)
	{
     	if(strcmp(envname,"sefile")==0)
	  {       
		
          ret=sor_checkfastfile(input_value);
		  if(0== ret)
		 {
			ret = set_boot_env_var_name(envname,input_value);
        	if(0 == ret)
        	{
        		sem_syslog_dbg("Set environment variable %s success\n",envname);
        	}
        	else if(1 == ret)
        	{
        		sem_syslog_dbg("Can't open the file\n");
        	}
        	else
        	{
        		sem_syslog_dbg("\nSet environment variable %s failure ret is %d\n",envname,ret);
        	}        			
		}		
     }	
    else 
	 {   
		ret = set_boot_env_var_name(envname, input_value);
		  if(0 == ret)
		  {
           sem_syslog_dbg("Set environment variable %s success\n",envname);     
		  }
		  else if(1 == ret)
		  {
           sem_syslog_dbg("Can't open the file\n"); 
		  }
		  else 
          {
           sem_syslog_dbg("\nSet environment variable %s failure ret is %d\n",envname,ret);
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
int get_product_board_id(char *filename, int *board_id)
{
	int fd;
	char buff[16] = {0};
	unsigned int data;

	if((filename == NULL) || (board_id == NULL))
	{
		printf("(filename == NULL) || (board_id == NULL)\n");
		return -1;
	}

	fd = open(filename, O_RDONLY, 0);
	if (fd >= 0) 
	{
		if(read(fd, buff, 16) < 0) 
			printf("Read error : no value\n");
		close(fd);
	}
	else
	{        
		printf("Open file:%s error!\n",filename);
		return -1;
	}
	
	data = strtoul(buff, NULL, 10);
	*board_id = data;
	return 0;
}

DBusMessage *sem_dbus_set_bootcmd(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter	 iter;
	DBusError err;

	char *envname;
	char *input_num_temp;
	int ret = 0;
	int board_id =0;
	int retval=0;
    int input_num;
	char file_path[200] = {0};
	
	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
								DBUS_TYPE_STRING, &envname,
							    DBUS_TYPE_STRING, &input_num_temp,	
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
	}
	sem_syslog_dbg("Set environment variable : %s\n", envname);
	sem_syslog_dbg("Input numeral = %s  \n",input_num_temp);    
    
	sprintf(file_path,"/dbm/local_board/board_id");
	retval = get_product_board_id(file_path,&board_id);
	if( retval ) 
		sem_syslog_dbg("Get product board_id  failure\n");

	
    input_num = strtoul(input_num_temp,NULL,10);
	sem_syslog_dbg("input_num = %d  \n",input_num);
	sem_syslog_dbg("board_id = %d  \n",board_id); 
	
	switch(input_num){
		
		case 1: 
		case 2: /*AX71_SMU */
			if ((board_id != BOARD_SOFT_ID_AX7605I_CRSMU))
			{
				sem_syslog_dbg( "input parameter error,this board type doesn't correct \n");
			    ret = -1;
			}   			 
				break;
		
		case 3: /* AX71_2x12g12s*/
			if ((board_id != BOARD_SOFT_ID_AX7605I_2X12G12S))
			{
				sem_syslog_dbg( "input parameter error,this board type doesn't correct \n");
			    ret = -1;
			}
			break;  				
		
		case 4:
		case 5:/*AX71_1x12g12s*/
			if ((board_id != BOARD_SOFT_ID_AX7605I_1X12G12S))
			{
				sem_syslog_dbg( "input parameter error,this board type doesn't correct \n");
			    ret = -1;
			}   			 
				break;  	

		case 6: /* AX81_SMU*/
			if ((board_id != BOARD_SOFT_ID_AX81_SMU))
			{
				sem_syslog_dbg( "input parameter error,this board type doesn't correct \n");
			    ret = -1;

			}   			 
				break;  				
			
		case 7: 
		case 8: /*AX81_AC8C/AX81_AC12C*/
			if (!((board_id == BOARD_SOFT_ID_AX81_AC12C )||(board_id == BOARD_SOFT_ID_AX81_AC8C)))
			{
				sem_syslog_dbg( "input parameter error,this board type doesn't correct \n");
			    ret = -1;
			}    				 
				break;
		case 9: 
	    case 10:/*AX81_AC4X */
			if ((board_id != BOARD_SOFT_ID_AX81_AC4X))
			{  
				sem_syslog_dbg( "input parameter error,this board type doesn't correct \n");
			    ret = -1;
			}   			 
				break;    		 
		case 11: /*AX81_12x */
			if ((board_id != BOARD_SOFT_ID_AX81_12X))
			{
				sem_syslog_dbg( "input parameter error,this board type doesn't correct \n");
			    ret = -1;
			}
			    break; 
		case 12: 
		case 13: /*AX81_1x12g12s*/
			if ((board_id != BOARD_SOFT_ID_AX81_1X12G12S))
			{
				sem_syslog_dbg( "input parameter error,this board type doesn't correct \n");
			    ret = -1;
			}    			 
				break; 
	   case 14: /*AX81_2x12g12s*/
	   	   	if ((board_id != BOARD_SOFT_ID_AX81_2X12G12S))
			{
				sem_syslog_dbg( "input parameter error,this board type doesn't correct \n");
			    ret = -1;
			}    		
				break; 
			
		default: 
			sem_syslog_dbg("Error! Not find the item: %d\n",input_num);
			ret = -2;
				break;
	}

    if (ret == 0)
    {
	  ret = set_boot_env_var_name(envname,input_num_temp);
        
	  if(0 == ret)
	  {
		 sem_syslog_dbg("Set environment variable bootcmd success\n");
	  }
	  else if(1 == ret)
	  {
		 sem_syslog_dbg("Can't open the file\n");
	  }
	  else
	  {
		 sem_syslog_dbg("\nSet environment variable bootcmd failure ret is %d\n",ret);
	  }
    }
	
	reply = dbus_message_new_method_return(msg);	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT32,
        								 &ret);	
	return reply;

}

DBusMessage *sem_dbus_show_boot_env_var(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter	 iter;
	DBusError err;

	char *envname;
	int ret = 0;
	
	dbus_error_init(&err);
		
	if (!(dbus_message_get_args(msg, &err,
								DBUS_TYPE_STRING, &envname,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}

	}

	sem_syslog_dbg("show bootrom environment variable : %s\n", envname);

	ret = get_boot_env_var_name(envname);
	if(0 == ret)
	{
		sem_syslog_dbg("The environment variable value is %s\n",envname);
	}
	else if(1 == ret)
	{
		sem_syslog_dbg("Can't open the file\n");
	}
	else
	{
		sem_syslog_dbg("Bootrom environment variable %s does not exis , ret is %d \n",envname,ret);
	}

	reply = dbus_message_new_method_return(msg);	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT32,
        								 &ret);	
	dbus_message_iter_append_basic (&iter,
								DBUS_TYPE_STRING,
								&envname);

	return reply;
}

/*
 * set boot img on every board in the whole system
 * caojia 20111027
 */
DBusMessage *sem_dbus_set_system_img(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter	 iter;
	DBusError err;

	char *version_file;
	unsigned long long ret = 0x0ull;
	int retval = 0;
	int board_on_mask = 0x0;
	int temp_mask = 0;
	int i = 0;
	int timeout = 50;

	
	dbus_error_init(&err);
		
	if (!(dbus_message_get_args(msg, &err,
								DBUS_TYPE_STRING, &version_file,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		ret = 0xffffffffffull;
		
		reply = dbus_message_new_method_return(msg);
	
		dbus_message_iter_init_append (reply, &iter);
	
		dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT64,
									 &ret);
		return reply;
	}

	sem_syslog_dbg("set system_img version file : %s\n", version_file);

	set_system_img_done_mask = 0x0ull;
	set_system_img_board_mask = 0;
	
	board_on_mask |= (1 << local_board->slot_id);
	set_system_img_board_mask |= (1 << local_board->slot_id);

	sem_syslog_dbg("set_system_img_board_mask : %d\n", set_system_img_board_mask);

	for(i = 0; i < product->slotcount; i++) 
	{
		if ((i != local_board->slot_id) && \
			(g_send_sd_arr[i].sd > 0) && \
			(product->board_on_mask & (1 << i)))
		{
			retval = set_system_img_msg_send(i, version_file);
			if (!retval)
			{
				board_on_mask |= (1 << i);
			}
			else
			{
				ret |= (0xe << (i * 4));
			}
		}
	}

	sem_syslog_dbg("board on mask : %d\n", board_on_mask);
	sem_syslog_dbg("ret : %llx\n", ret);

	if (board_on_mask != product->board_on_mask)
	{
		temp_mask = board_on_mask ^ product->board_on_mask;
		for (i = 0; i < product->slotcount; i++)
		{
			if ((temp_mask >> i) == 0)
				break;
			
			if (((temp_mask >> i) % 2) == 1)
			{
				ret |= (0xe << (i * 4));
			}
		}
	}

	sem_syslog_dbg("ret : %llx\n", ret);

	retval = sem_set_boot_img(version_file);

	if (retval != 0xf)
	{
		retval++;
	}
	ret |= (retval << (local_board->slot_id * 4));

	sem_syslog_dbg("ret : %llx\n", ret);

	while (timeout--)
	{
		if (board_on_mask == set_system_img_board_mask)
		{
			ret |= set_system_img_done_mask;
			break;
		}
		
		sleep(1);
	}

	if (timeout < 0)
	{
		ret |= set_system_img_done_mask;
		
		temp_mask = board_on_mask ^ set_system_img_board_mask;
		for (i = 0; i < product->slotcount; i++)
		{
			if ((temp_mask >> i) == 0)
				break;
			
			if (((temp_mask >> i) % 2) == 1)
			{
				ret |= (0xe << (i * 4));
			}
		}
	}

	sem_syslog_dbg("set_system_img_done_mask : %llx\n", set_system_img_done_mask);
	sem_syslog_dbg("set_system_img_board_mask : %d\n", set_system_img_board_mask);
	sem_syslog_dbg("board_on_mask : %d\n", product->board_on_mask);
	sem_syslog_dbg("ret : %llx\n", ret);

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
								DBUS_TYPE_UINT64,
								&ret);

	return reply;
}


int mcb_active_standby_switch_notification(int slot_id)
{
	sem_pdu_head_t * head;
	sem_tlv_t *tlv_head;
	
	board_info_syn_t *board_info_head;
    char send_buf[SEM_TIPC_SEND_BUF_LEN];

	memset(send_buf, 0, SEM_TIPC_SEND_BUF_LEN);
	head = (sem_pdu_head_t *)send_buf;
	head->slot_id = local_board->slot_id;
	head->version = 11;
	head->length = 22;

	tlv_head = (sem_tlv_t *)(send_buf + sizeof(sem_pdu_head_t));
	tlv_head->type = SEM_MCB_ACTIVE_STANDBY_SWITCH_NOTIFICATION;
	tlv_head->length = 33;
	
	if (sendto(g_send_sd_arr[slot_id].sd, send_buf, SEM_TIPC_SEND_BUF_LEN, 0, \
		(struct sockaddr*)&g_send_sd_arr[slot_id].sock_addr, sizeof(struct sockaddr_tipc)) < 0)
	{
		sem_syslog_warning("send SEM_MCB_ACTIVE_STANDBY_SWITCH_NOTIFICATION failed\n");
		return 1;
	}
	else
	{
		sem_syslog_dbg("send SEM_MCB_ACTIVE_STANDBY_SWITCH_NOTIFICATION succeed\n");
	}

	return 0;
}

/*
 * mcb active standby switch
 * caojia 20110928
 */
DBusMessage *sem_dbus_mcb_active_standby_switch(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter	 iter;
	DBusError err;
	int ret = 0;
	int i = 0;
	int timeout = 10;

	int local_slot_id = local_board->slot_id;
	int remote_slot_id;
	int remote_sd = -1;
	
	if (local_board->slot_id == product->master_slot_id[0])
	{
		remote_slot_id = product->master_slot_id[1];
	}
	else
	{
		remote_slot_id = product->master_slot_id[0];
	}

	/* support distributed system, and only on active master board */
	if ((!product->is_distributed) || (!local_board->is_active_master) || \
		(!product->more_than_one_master_board_on) || \
		(product_info_syn.product_slot_board_info[local_slot_id].board_state != BOARD_READY) || \
		(product_info_syn.product_slot_board_info[remote_slot_id].board_state != BOARD_READY))
	{
		sem_syslog_dbg("### Condition not meet ###\n");
		sem_syslog_dbg("### is_active_master(%d) ###\n", local_board->is_active_master);
		sem_syslog_dbg("### more_than_one_master_board_on(%d) ###\n", product->more_than_one_master_board_on);
		sem_syslog_dbg("### [local_slot_id].board_state(%d) ###\n", product_info_syn.product_slot_board_info[local_slot_id].board_state);
		sem_syslog_dbg("### [remote_slot_id].board_state(%d) ###\n", product_info_syn.product_slot_board_info[remote_slot_id].board_state);
		
		ret = 1;
		
		reply = dbus_message_new_method_return(msg);
	
		dbus_message_iter_init_append (reply, &iter);
	
		dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
		return reply;
	}

	#if 1
	/* not support product ax7605i temporarily */
	if (product->product_type == XXX_YYY_AX7605I)
	{
		ret = 1;
		reply = dbus_message_new_method_return(msg);
	
		dbus_message_iter_init_append (reply, &iter);
	
		dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
		return reply;
	}
	#endif

	mcb_switch_arg_t mcb_switch_arg;
	mcb_switch_arg.switch_type = COMMAND_MODE;
	mcb_switch_arg.broadcast_domain = ALL_BROADCAST;

	for(i = 0; i < product->slotcount; i++) 
	{
		if ((i != local_board->slot_id) && \
			(g_send_sd_arr[i].sd >= 0) && \
			(product->board_on_mask & (1 << i)))
		{
			mcb_active_standby_switch_notification(i);
			sem_syslog_dbg("mcb_active_standby_switch_notification send to slot %d sd %d\n", \
				i + 1, g_send_sd_arr[i].sd);
		}
	}

	ret = product->active_stby_switch((void *)(&mcb_switch_arg));
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);

	return reply;
}

DBusMessage *sem_dbus_show_system_environment(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusError err;
	unsigned int ret = 0, is_active_master, cpu_num, board_id;
	env_state_t envi_state;
	
	dbus_error_init(&err);
	
	memset(&envi_state, 0, sizeof(env_state_t));	
	ret = local_board->show_system_environment(&envi_state);
	if(ret != 0){
		ret = 1;
		sem_syslog_dbg("set system alarm state err! ");
	}
	
	reply = dbus_message_new_method_return(msg);

	is_active_master = local_board->is_active_master;

	cpu_num = local_board->cpu_num;
	board_id = local_board->board_id;
	sem_syslog_dbg("local_board->cpu_num = %#x\n", local_board->cpu_num);
	dbus_message_append_args(reply,
		                     DBUS_TYPE_UINT32,&is_active_master,
		                     DBUS_TYPE_UINT32,&cpu_num,
		                     DBUS_TYPE_UINT32,&board_id,
							 DBUS_TYPE_UINT32,&(envi_state.fan_state),
							 DBUS_TYPE_UINT32,&(envi_state.fan_rpm),
							 DBUS_TYPE_UINT32,&(envi_state.power_state),
							 DBUS_TYPE_INT32,&(envi_state.master_inter_temp),
							 DBUS_TYPE_INT32,&(envi_state.master_remote_temp),
							 DBUS_TYPE_INT32,&(envi_state.slave_inter_temp),
							 DBUS_TYPE_INT32,&(envi_state.slave_remote_temp),
							 DBUS_TYPE_INVALID);
	return reply;	
}

DBusMessage *sem_dbus_reset_all(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	unsigned int isFast = 0;
	int slot_id = 0;
	unsigned int reset_bitmask = 0x0;
	int ret = 0;

	sem_syslog_warning("sem hardware reset all slots\n");

	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
								DBUS_TYPE_UINT32, &isFast,
								DBUS_TYPE_UINT32, &slot_id,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		sem_syslog_dbg("sem_dbus_reset_all get arg failed and reset normal\n");
	}

	slot_id -= 1;

	if (slot_id == -1)
	{
		reset_bitmask = 0x3fff;
	}
	else
	{
		reset_bitmask |= (1 << slot_id);
	}

	sem_syslog_dbg("reset bitmask : 0x%x\n", reset_bitmask);

	ret = product->hardware_reset_slot(reset_bitmask, isFast, RESET_CMD_FROM_VTYSH);
	if (ret)
	{ 
		sem_syslog_dbg("hardware reset failed\n");
	}
	
	return reply;
}

int disable_keep_alive_msg_send(int slot_id, int disable_time)
{
	sem_pdu_head_t * head;
	sem_tlv_t *tlv_head;
	int *time = NULL;
	
	board_info_syn_t *board_info_head;
    char send_buf[SEM_TIPC_SEND_BUF_LEN];

	memset(send_buf, 0, SEM_TIPC_SEND_BUF_LEN);
	head = (sem_pdu_head_t *)send_buf;
	head->slot_id = local_board->slot_id;
	head->version = 11;
	head->length = 22;

	tlv_head = (sem_tlv_t *)(send_buf + sizeof(sem_pdu_head_t));
	tlv_head->type = SEM_DISABLE_KEEP_ALIVE_TEMPORARILY;
	tlv_head->length = 33;

	time = (int *)(send_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));

	*time = disable_time;

	if (sendto(g_send_sd_arr[slot_id].sd, send_buf, SEM_TIPC_SEND_BUF_LEN, 0, \
		(struct sockaddr*)&g_send_sd_arr[slot_id].sock_addr, sizeof(struct sockaddr_tipc)) < 0)
	{
		sem_syslog_warning("send disable_keep_alive_msg to slot %d failed\n", slot_id + 1);
		return 1;
	}
	else
	{
		sem_syslog_dbg("send disable_keep_alive_msg to slot %d succeed\n", slot_id + 1);
	}

	return 0;
}

DBusMessage *show_all_slot_sys_info(DBusConnection *conn, DBusMessage *msg, void *user_data)
	{
		DBusMessage* reply = NULL;
		DBusMessageIter  iter;
		DBusError err;
	
		int time;
		int i;
		char ret[1024]={0};
		char ret_temp[1024]={0};
		char *tem=ret;
	

	
		dbus_error_init(&err);
			
		if ((dbus_message_get_args(msg, &err,
									DBUS_TYPE_UINT32, &time,
									DBUS_TYPE_INVALID)))
		{
			
			if (dbus_error_is_set(&err)) 
			{
				sem_syslog_dbg("%s raised: %s", err.name, err.message);
				dbus_error_free(&err);
			}
			//mem info
			{
				sprintf(ret,"=========================================================================\n"
 							"\t\t\t\tSLOT %d\n" 
 							"-------------------------------------------------------------------------\n"
							"memory info:\n"
							"-----------------\n",(local_board->slot_id + 1));
				FILE* fp=NULL;
				fp = popen("free","r");
				if(NULL != fp)
				{
					if(!fread(ret_temp,1,1024,fp))
					sem_syslog_dbg("fread error\n", time);
					fclose(fp);
				}else{
					sprintf(ret_temp,"Can not get mem info\n");
				}
				strcat(ret,ret_temp);
				memset(ret_temp,0,1024);
				
			}
			//cpu info
			/*
			{
				sprintf(ret_temp,"-------------------------------------------------------------------------\n"
							"cpu info:\n"
							"-----------------\n",local_board->slot_id);
				
				strcat(ret,ret_temp);
				memset(ret_temp,0,1024);
	//			system("top -n 1 > /var/run/temp_top");
				FILE* fp=NULL;
				fp = popen("top","wr");
				if(NULL != fp)
				{
					fread(ret_temp,1,1024,fp);
					fprintf(fp,"q");
					fclose(fp);
				}else{
					sprintf(ret_temp,"Can not get cpu info\n");
				}
				strcat(ret,ret_temp);
				memset(ret_temp,0,1024);
				
				
			}
			*/
			fprintf(stderr,"%s\n",ret);

			
			reply = dbus_message_new_method_return(msg);
		
			dbus_message_iter_init_append (reply, &iter);
		
			dbus_message_iter_append_basic (&iter,
										 DBUS_TYPE_STRING,
										 &tem);
			
			return reply;
		}
	
		sem_syslog_dbg("get args error\n", time);
		return NULL;
	
	}




/*added by zhaocg for md5 subcommand 2012-10-31*/

DBusMessage *sem_dbus_md5_patch_slot(DBusConnection *conn, DBusMessage *msg, void *user_data)
	{
		DBusMessage* reply = NULL;
		DBusMessageIter  iter;
		DBusError err;
	
		char *patchname=NULL;
		char cmdstr[512] = {0};
		char tmp[512] = {0};
		char ret[1024]={0};
		char ret_temp[1024]={0};
		char *tem=ret;
		char *token=NULL;
	
	

	
		dbus_error_init(&err);
			
		if ((dbus_message_get_args(msg, &err,
									DBUS_TYPE_STRING, &patchname,
									DBUS_TYPE_INVALID)))
		{
			
			if (dbus_error_is_set(&err)) 
			{
				sem_syslog_dbg("%s raised: %s", err.name, err.message);
				dbus_error_free(&err);
			}
			

			sprintf(ret,"SLOT %d md5 info:",(local_board->slot_id + 1));
			
			FILE* fp=NULL;
			
			sprintf(cmdstr,"sor.sh imgmd5 patch/%s 120",patchname);
				
			fp = popen( cmdstr,"r");
				
			if(NULL != fp)
			{
				fgets(ret_temp,sizeof(ret_temp),fp);
				strcpy(tmp,ret_temp);
				token = strtok(ret_temp,"=");
				token = strtok(NULL,"\r");
				if(NULL==token)
				{
					token= strtok(tmp,":");
					token = strtok(NULL,"\r");
				}
				sprintf(ret_temp,"MD5(%s)=%s",patchname,token);
				fclose(fp);
			}
			else
			{
				sprintf(ret_temp,"Can not get mem info\n");
			}

			
			strcat(ret,ret_temp);	
	
			memset(ret_temp,0,1024);

			reply = dbus_message_new_method_return(msg);
		
			dbus_message_iter_init_append (reply, &iter);
		
			dbus_message_iter_append_basic (&iter,
										 DBUS_TYPE_STRING,
										 &tem);
			
			return reply;
		}
	
		sem_syslog_dbg("get args error\n");
		return NULL;
	
	}


DBusMessage *sem_dbus_md5_img_slot(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
		DBusMessage* reply = NULL;
		DBusMessageIter  iter;
		DBusError err;
	
		char *imgname=NULL;
		char cmdstr[512] = {0};
		char ret[1024]={0};
		char ret_temp[1024]={0};
		char *tem=ret;
	

	
		dbus_error_init(&err);
			
		if ((dbus_message_get_args(msg, &err,
									DBUS_TYPE_STRING, &imgname,
									DBUS_TYPE_INVALID)))
		{
			
			if (dbus_error_is_set(&err)) 
			{
				sem_syslog_dbg("%s raised: %s", err.name, err.message);
				dbus_error_free(&err);
			}
			

			sprintf(ret,"SLOT %d md5 info:",(local_board->slot_id + 1));
			
			FILE* fp=NULL;
			
			sprintf(cmdstr,"sor.sh imgmd5 %s 300",imgname);
				
			fp = popen( cmdstr,"r");
				
			if(NULL != fp)
			{
				if(!fread(ret_temp,1,1024,fp));
				sem_syslog_dbg("fread error\n");
				fclose(fp);
			}
			else
			{
				sprintf(ret_temp,"Can not get mem info\n");
			}
				
			strcat(ret,ret_temp);
			
			memset(ret_temp,0,1024);

			reply = dbus_message_new_method_return(msg);
		
			dbus_message_iter_init_append (reply, &iter);
		
			dbus_message_iter_append_basic (&iter,
										 DBUS_TYPE_STRING,
										 &tem);
			
			return reply;
		}
	
		sem_syslog_dbg("get args error\n");
		return NULL;
}

/*ended by zhaocg for md5 subcommand*/

/*added by zhaocg for fastfwd command 2012-11-21*/

DBusMessage *sem_dbus_img_or_fastfwd_slot(DBusConnection *conn, DBusMessage *msg, void *user_data)
	{
		DBusMessage* reply = NULL;
		DBusMessageIter  iter;
		DBusError err;
	
		char *cmdstr;
		char file_name[64][256];
		char *tem_name[64] = {0};
		int i = 0;
		int file_num = 0;
		int slotid = 0;
	
		dbus_error_init(&err);
			
		if ((dbus_message_get_args(msg, &err,
									DBUS_TYPE_STRING, &cmdstr,
									DBUS_TYPE_INVALID)))
		{
			
			if (dbus_error_is_set(&err)) 
			{
				sem_syslog_dbg("%s raised: %s", err.name, err.message);
				dbus_error_free(&err);
			}
			
			FILE* fp=NULL;
				
			fp = popen( cmdstr,"r");
				
			if(NULL != fp)
			{
				while(i<64&&fgets(file_name[i],sizeof(file_name[i]),fp))
					i++;
				file_num = i;
				fclose(fp);
			}
			else
			{
				file_num = -1;
			}

			reply = dbus_message_new_method_return(msg);
		
			dbus_message_iter_init_append (reply, &iter);
		
			dbus_message_iter_append_basic (&iter,DBUS_TYPE_INT32,&file_num);
			
			for(i=0;i<file_num;i++)
			{
				tem_name[i] = file_name[i];
				dbus_message_iter_append_basic (&iter,DBUS_TYPE_STRING,&tem_name[i]);
			}
		
			return reply;
		}
	
		sem_syslog_dbg("get args error\n");
		return NULL;
	
	}

DBusMessage *sem_dbus_delete_img_or_fastfwd_slot(DBusConnection *conn, DBusMessage *msg, void *user_data)
	{
		DBusMessage* reply = NULL;
		DBusMessageIter  iter;
		DBusError err;
	
		char cmdstr[512] = {0};
		char *file_name = NULL;
		char buf[256] = {0};
		
		int slotid = 0;
		int ret;
		int status = 0;
	    
		dbus_error_init(&err);
			
		if ((dbus_message_get_args(msg, &err,
									DBUS_TYPE_STRING, &file_name,
									DBUS_TYPE_INVALID)))
		{
			
			if (dbus_error_is_set(&err)) 
			{
				sem_syslog_dbg("%s raised: %s", err.name, err.message);
				dbus_error_free(&err);
			}
			
			FILE* fp=NULL;
			memset(buf,0,256);	
			sprintf(cmdstr,"sor.sh ls %s 120",file_name);
	        fp = popen( cmdstr, "r" );
	        if(fp)
	        {	
		     	fgets(buf, sizeof(buf), fp ); 	
				ret= pclose(fp);	
				switch (WEXITSTATUS(ret))
				{			
				case 0: 
					if(*buf != 0)	
					{							
						sprintf(cmdstr,"sor.sh rm %s 120",file_name);
						system(cmdstr);
						status=0;/*Delete success*/	
					}
					else
					{
						status=1;/*file not exist*/
					}
					break;					
				default :
					sem_syslog_dbg("Delete error! Cant't get file\n");
					status=-1;
					break;									
				}	
			}	
			else	
			{
				sem_syslog_dbg("Delete error!\n"); 	
				return NULL; 
			}	

			reply = dbus_message_new_method_return(msg);
		
			dbus_message_iter_init_append (reply, &iter);
		
			dbus_message_iter_append_basic (&iter,DBUS_TYPE_INT32,&status);
			return reply;
		}
	
		sem_syslog_dbg("get args error\n");
		return NULL;
	
	}

/*ended by zhaocg for fastfwd command 2012-11-21*/


/*
 * Disable sem keep_alive function temporarily for seconds
 * caojia 20111102
 */
DBusMessage *sem_dbus_disable_keep_alive(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter	 iter;
	DBusError err;

	int time;
	int ret = 0;
	int retval = 0;
	int i;

	struct timeval tv;
	struct timezone tz;

	dbus_error_init(&err);
		
	if (!(dbus_message_get_args(msg, &err,
								DBUS_TYPE_UINT32, &time,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		ret = 1;
		
		reply = dbus_message_new_method_return(msg);
	
		dbus_message_iter_init_append (reply, &iter);
	
		dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
		return reply;
	}

	sem_syslog_dbg("Disable Time : %d\n", time);

	for(i = 0; i < product->slotcount; i++) 
	{
		if ((i != local_board->slot_id) && \
			(g_send_sd_arr[i].sd > 0) && \
			(product->board_on_mask & (1 << i)))
		{
			retval = disable_keep_alive_msg_send(i, time);
			if (retval)
			{
				ret = 1;
			}
		}
	}

	if (disable_keep_alive_time <= 0)
	{
		gettimeofday(&tv, &tz);
		disable_keep_alive_start_time = tv.tv_sec;
		disable_keep_alive_time = time;
		disable_keep_alive_flag_time = disable_keep_alive_time;
	}
	else if (disable_keep_alive_time > 0)
	{
		if (time > disable_keep_alive_time)
		{
			gettimeofday(&tv, &tz);
			disable_keep_alive_start_time = tv.tv_sec;
			disable_keep_alive_time = time;
			disable_keep_alive_flag_time = disable_keep_alive_time;
		}
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
								DBUS_TYPE_UINT32,
								&ret);

	return reply;
}

/*
*return 0 for command success exec;
*/
int exec_system_cmd(char * cmd)
{
	int result;
	int status;
	result = system(cmd);
	sem_syslog_dbg("cmd = %s\n",cmd);
	sem_syslog_dbg("function:exec_system_cmd.result = %d\n",result);
	
    if (-1 == result) {
           return 1;
    } else if (WIFEXITED(result)) {
            status = WEXITSTATUS(result);
            if (127 == status) {
                return 1;
            } else if (126 == status) {
                return 1;
            } else {
                return 0;//success
            }
    }
	return 1;
}

int execute_system_command_msg_send(int slot_id, char *cmd)
{
	sem_pdu_head_t * head;
	sem_tlv_t *tlv_head;
	char *cmd_head = NULL;
	
	board_info_syn_t *board_info_head;
    char send_buf[SEM_TIPC_SEND_BUF_LEN];

	memset(send_buf, 0, SEM_TIPC_SEND_BUF_LEN);
	head = (sem_pdu_head_t *)send_buf;
	head->slot_id = local_board->slot_id;
	head->version = 11;
	head->length = 22;

	tlv_head = (sem_tlv_t *)(send_buf + sizeof(sem_pdu_head_t));
	tlv_head->type = SEM_EXECUTE_SYSTEM_COMMAND;
	tlv_head->length = 33;

	cmd_head = (char *)(send_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));

	strcpy(cmd_head, cmd);

	if (sendto(g_send_sd_arr[slot_id].sd, send_buf, SEM_TIPC_SEND_BUF_LEN, 0, \
		(struct sockaddr*)&g_send_sd_arr[slot_id].sock_addr, sizeof(struct sockaddr_tipc)) < 0)
	{
		sem_syslog_warning("send execute_system_command_msg to slot %d failed\n", slot_id + 1);
		return 1;
	}
	else
	{
		sem_syslog_dbg("send execute_system_command_msg to slot %d succeed\n", slot_id + 1);
	}

	return 0;
}


DBusMessage *sem_dbus_execute_system_command(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter iter;
	DBusError err;

	int ret = 0;
	int retval;
	int i;
	int slot_id;
	char *cmd;

	dbus_error_init(&err);
		
	if (!(dbus_message_get_args(msg, &err,
								DBUS_TYPE_UINT32, &slot_id,
								DBUS_TYPE_STRING, &cmd,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		ret = 2;
		
		reply = dbus_message_new_method_return(msg);
	
		dbus_message_iter_init_append (reply, &iter);
	
		dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
		return reply;
	}

	slot_id -= 1;

	if (slot_id == local_board->slot_id)
	{
		if(exec_system_cmd(cmd))
		{
            ret = 1;
		}
		sem_syslog_dbg("execute cmd return value : %d\n", ret);
	}
	else if (slot_id == -1)
	{
		for(i = 0; i < product->slotcount; i++) 
		{
			if ((i != local_board->slot_id) && \
				(g_send_sd_arr[i].sd > 0) && \
				(product->board_on_mask & (1 << i)))
			{
				retval = execute_system_command_msg_send(i, cmd);
				if (retval)
				{
					ret = 1;
				}
			}
		}
		
		if(exec_system_cmd(cmd))
		{
            ret = 1;
		}
		sem_syslog_dbg("execute cmd return value : %d\n", ret);
	}
	else
	{
		retval = execute_system_command_msg_send(slot_id, cmd);
		if (retval)
		{
			ret = 1;
		}
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
								DBUS_TYPE_UINT32,
								&ret);

	return reply;
}

#define PATCH_EXECUTE_RESULT_BUF_LEN	1024
DBusMessage *sem_dbus_apply_patch(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	unsigned int isFast = 0;
	int slot_id = 0;
	unsigned int reset_bitmask = 0x0;
	int ret = 0;
	int status;
	char *name=NULL;
    char temp_buf[400]={0};
	int fd;
	//char execute_result_buf[PATCH_EXECUTE_RESULT_BUF_LEN] = {0};
	char *execute_result_buf = NULL;
	
	sem_syslog_dbg("Dbus to sem for apply patch!\n");

	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
		                                    DBUS_TYPE_STRING,&name,
											DBUS_TYPE_INVALID))) {
											
			sem_syslog_warning("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				sem_syslog_warning("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
	}

	sprintf(temp_buf, "/mnt/patch/%s", name);
	fd = open(temp_buf, O_RDONLY);
	if(fd < 0)
	{
		sem_syslog_dbg("apply patch %s, but the patch is not exist at /mnt/patch/\n", name);
		//ret == 2 meas the patch file is not exist
		ret = 2;
	} else {
		close(fd);
		
		sprintf(temp_buf,"cd /mnt/patch;/usr/bin/apply_patch.sh %s >& /mnt/patch.log",name);
		//sprintf(temp_buf,"vtysh -c \"apply patch %s\" >& /mnt/patch.log",name);
		sem_syslog_dbg("Execute command : %s\n",temp_buf);
		
	    status = system(temp_buf);
		
		if (-1 == status)  
	    {  
	        sem_syslog_warning("system error!");
	        ret = 1;
	    }  
	    else  
	    {  
	        sem_syslog_event("exit status value = [0x%x]\n", status);  
	  
	        if (WIFEXITED(status))  
	        {  
	            if (0 == WEXITSTATUS(status))  
	            {  
	                sem_syslog_event("run shell script %s successfully.\n",name);
					ret = 0;
	            }  
	            else  
	            {  
	                sem_syslog_warning("run shell script %s fail, script exit code: %d\n",name,WEXITSTATUS(status));  
	                ret = 1;
				}  
	        }  
	        else  
	        {  
	            sem_syslog_warning("%s exit status = [%d]\n",name,WEXITSTATUS(status));
	            ret = 1;
	        }  
	    }  
	}
	execute_result_buf = (char *)malloc(PATCH_EXECUTE_RESULT_BUF_LEN);
	if (!execute_result_buf) {
		sem_syslog_dbg("alloc buf failed\n");
		//get log failed
		ret = 3;
	} else {
		bzero(execute_result_buf, PATCH_EXECUTE_RESULT_BUF_LEN);
		fd = open("/mnt/patch.log", O_RDONLY);
		if (fd < 0) {
			sprintf(execute_result_buf, "get patch execute log failed\n");
			sem_syslog_dbg("get patch execute log failed:/mnt/patch.log\n");
		} else {
			read(fd, execute_result_buf, PATCH_EXECUTE_RESULT_BUF_LEN);
			close(fd);
		}
	}
	
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_INT32,
									 &ret);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_STRING,
									 &execute_result_buf);
	free(execute_result_buf);
	
	return reply;

}


/******************************************
xufujun

********************************************/

int flash_erase_one_partition(char * mtd_device) 
{
	static const char *exe_name;
    mtd_info_t meminfo;
    int fd, clmpos = 0, clmlen = 8;
    erase_info_t erase;
    int  bbtest = 1;
    int quiet = 0;	/* true -- don't output progress */

    if ((fd = open(mtd_device, O_RDWR)) < 0) {
	return -1;
    }


    if (ioctl(fd, MEMGETINFO, &meminfo) != 0) {
	close(fd);
	return -2;
    }

    erase.length = meminfo.erasesize;
    for (erase.start = 0; erase.start < meminfo.size; erase.start += meminfo.erasesize) {
	if (bbtest) {
	    loff_t offset = erase.start;
	    int ret = ioctl(fd, MEMGETBADBLOCK, &offset);
	    if (ret > 0) {
		if (!quiet)
		continue;
	    } else if (ret < 0) {
		if (errno == EOPNOTSUPP) {
		    bbtest = 0;
		} else {
			close(fd);
		    return 1;
		}
	    }
	}

	if (ioctl(fd, MEMERASE, &erase) != 0) {
	    continue;
	}

	if (lseek (fd, erase.start, SEEK_SET) < 0) {
	    continue;
	}
    }

	close(fd);
	printf("\n");
	
    return 0;
}

/******************************************************************************/  
static int dev_fd = -1,fil_fd = -1;

/*write/rend buff*/

#define BUFSIZE (10 * 1024)

int flash_writ_boot (char *filename,char *device)
{
   int i;
   ssize_t result;
   size_t size,written;
   struct mtd_info_user mtd;
   struct erase_info_user erase;
   struct stat filestat;
   unsigned char src[BUFSIZE],dest[BUFSIZE];

   dev_fd = open (device,O_SYNC | O_RDWR);
   if(dev_fd < 0){
       return 2;
       }
   
   if (ioctl (dev_fd,MEMGETINFO,&mtd) < 0)
	 {
	printf("ioctl memgetinfo fail\n");
	return 3;
	 }
   
   fil_fd = open (filename,O_RDONLY);   
   if(fil_fd < 0){
      return 2;
       }
   
   if (fstat (fil_fd,&filestat) < 0)
	 {
	     printf("fstat vaild\n");
		return 1;
	 }

   if (filestat.st_size > mtd.size)
	 {
		return 1;
	 }


   erase.start = 0;
   erase.length = filestat.st_size & ~(mtd.erasesize - 1);
   if (filestat.st_size % mtd.erasesize) erase.length += mtd.erasesize;
		int blocks = erase.length / mtd.erasesize;
		erase.length = mtd.erasesize;
		for (i = 1; i <= blocks; i++)
		  {
			 if (ioctl (dev_fd,MEMERASE,&erase) < 0)
			   {
				return 1;;
			   }
			 erase.start += mtd.erasesize;
		  }
	
  	printf("Erased %u / %luk bytes\n",erase.length,filestat.st_size);


   size = filestat.st_size;
   i = BUFSIZE;
   written = 0;
   
   while (size)
	 {
		if (size < BUFSIZE) i = size;

		if(read (fil_fd,src,i) < 0){		 
			return 4;
		    }

		result = write (dev_fd,src,i);
		if (i != result)
		  {
			 if (result < 0){			    
				
			   }
			return 5;
		}
		written += i;
		size -= i;
	 }

   if (lseek (fil_fd,0L,SEEK_SET) < 0)
	 {
	     printf("lseek error\n");
		return 1;
	 }
   
   if (lseek (dev_fd,0L,SEEK_SET) < 0)
	 {	    
		return 1;
   	}
   
   size = filestat.st_size;
   i = BUFSIZE;
   written = 0;
 
   while (size)
	 {
		if (size < BUFSIZE) i = size;
		
		if(read (fil_fd,src,i) < 0){
		    return 4;
		    }

		
		if(read (dev_fd,dest,i) < 0){
		    return 4;
		    }
		
		if (memcmp (src,dest,i))
		  {
			 return -1;
		  }

		written += i;
		size -= i;
	 }
	close(fil_fd);
	close(dev_fd);
   return 0;
}

DBusMessage  *flash_erase_partition(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	unsigned int isFast = 0;
	int slot_id = 0;
	unsigned int reset_bitmask = 0x0;
	int ret = 0;
	int status;
	char *name = NULL;
        char temp_buf[100]={0};

	char *execute_result_buf = NULL;
	sem_syslog_event("flash erase!\n");
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		                                    DBUS_TYPE_STRING,&name,
											DBUS_TYPE_INVALID))) {
											
			sem_syslog_warning("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				sem_syslog_warning("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}
	
	sprintf(temp_buf,"/dev/%s",name);
	sem_syslog_dbg("Execute command : %s\n",temp_buf);
//	status = system(temp_buf);
	status =  flash_erase_one_partition(temp_buf);

	if(status == -1){
		sem_syslog_warning("open device fail!\n");
		ret = 2;
	}
	else if(status == -2){
		sem_syslog_warning("unable to get MTD device\n");
		ret = 3;
	}
	else if(status == 1){
		sem_syslog_warning("unable to get MTD device\n");
		ret = 4;
	}
	else if(status == 0){
		sem_syslog_warning("succes\n");
		ret = 0;
	}
	else{
		ret = 1;
	}
	
    reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_INT32,
									 &ret);

	return reply;

}
DBusMessage *write_boot_to_flash(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	
	DBusMessage* reply = NULL;
		DBusMessageIter  iter;
		DBusError err;
		unsigned int isFast = 0;
		int slot_id = 0;
		unsigned int reset_bitmask = 0x0;
		int ret = 0;
		int status;
		char *name = NULL;
		char temp_buf[100]={0};
		char file_buf[100]={0};
		char mountblk[100] = {0};	
		int mstatus ;
		char *execute_result_buf = NULL;
		sem_syslog_event("flash write boot!\n");
		dbus_error_init(&err);
		
		if (!(dbus_message_get_args ( msg, &err,
												DBUS_TYPE_STRING,&name,
												DBUS_TYPE_INVALID))) {
												
				sem_syslog_warning("Unable to get input args ");
				if (dbus_error_is_set(&err)) {
					sem_syslog_warning("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
				return NULL;
			}


		
	//	strcpy(mountblk,"sudo mount /blk");
	//	sprintf(temp_buf, "flashcp  /blk/%s  /dev/mtd1",name);
	sprintf(mountblk, "sor.sh  cpfromblk  %s  30",name);
	
	sprintf(file_buf, "/mnt/%s",name);
	sprintf(temp_buf, "/dev/mtd1");
		sem_syslog_dbg("Execute command : %s\n",temp_buf);
		//vty_out(vty, "starting erase\n");
	//	mstatus = system(mountblk);
	//	status = system(temp_buf);
	mstatus = system(mountblk);
	
	 status = flash_writ_boot(file_buf,temp_buf);
	if(status == 2){
			sem_syslog_warning("open device failed!\n");
			ret = 2;
		}
		else if(status == 3){
			sem_syslog_warning("unable to get MTD device\n");
			ret = 3;
		}
		else if(status == 4){
			sem_syslog_warning("read file failed\n");
			ret = 1;
		}
		else if(status == 5){
			sem_syslog_warning("writing failed\n");
			ret = 0;
		}
		else if(status == 0){
			sem_syslog_warning("success\n");
			ret = 0;
		}
		else if(status == 1){
			sem_syslog_warning("writing failed\n");
			ret = 1;
		}
		else if(status == -1){
			sem_syslog_warning("compare file failed\n");
			ret = -1;
			}
		else{
			ret = 1;
		}

		reply = dbus_message_new_method_return(msg);
		
		dbus_message_iter_init_append (reply, &iter);
		
		dbus_message_iter_append_basic (&iter,
										 DBUS_TYPE_INT32,
										 &ret);
	
		return reply;
	
}



DBusMessage *sem_dbus_delete_patch(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	unsigned int isFast = 0;
	int slot_id = 0;
//	unsigned int reset_bitmask = 0x0;
	int ret = 0;
	int fd_e;
	char c;
//	int status;
	char *name = NULL;
    char temp_buf[128]={0};

	sem_syslog_dbg("Dbus to sem for delete patch!\n");

	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
		                                    DBUS_TYPE_STRING,&name,
											DBUS_TYPE_INVALID))) {
											
			sem_syslog_warning("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				sem_syslog_warning("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
	}
	//sprintf(temp_buf,"vtysh -c \"apply patch %s\" >& /blk/patch.log",name);

	//delete /mnt/patch/**.sp first
	{
		memset(temp_buf,0,128);
		sprintf(temp_buf,"rm /mnt/patch/%s -f > /dev/null 2>/dev/null",name);
		
		sem_syslog_dbg("Execute command : %s\n",temp_buf);
		system(temp_buf);
	}
	memset(temp_buf,0,128);
	sprintf(temp_buf,"sudo /usr/bin/sor.sh ls patch/%s 20 | wc -l > /mnt/exist6",name);
	system(temp_buf);
	fd_e = open("/mnt/exist6", O_RDONLY);
	if (fd_e < 0)
	{
		sem_syslog_warning("open /mnt/exist6 error\n");
		return NULL;
	}
	read(fd_e, &c, 1);
	close(fd_e);
	if (c-'0')
	{
		memset(temp_buf,0,128);
		sprintf(temp_buf,"sor.sh rm patch/%s 20",name);
		sem_syslog_dbg("Execute command : %s\n",temp_buf);
		
		ret = system(temp_buf);	
		ret=WEXITSTATUS(ret) ;	
	}
	else
		ret = 6;

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_INT32,
									 &ret);
	return reply;

}
	
DBusMessage *sem_dbus_sem_send_trap(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int ret = 0;
	int i = 0;
	
    unsigned int num = 1;
    sem_syslog_dbg("**sem_dbus_sem_send_trap !\n");
	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
		                        DBUS_TYPE_UINT32, &num,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		sem_syslog_dbg("sem_dbus_sem_send_trap get arg failed and reset normal\n");
	}

    for(;i<num;i++)
    {
		sem_syslog_dbg("send the %d signal:\n",i+1);
        sem_send_signal_to_trap_debug();

    }
	
    
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &ret);
	sem_syslog_dbg("**sem_dbus_sem_send_trap success!\n");
	return reply;
}


#ifdef __cplusplus
extern "C"
}
#endif

