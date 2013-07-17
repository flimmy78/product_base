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
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>

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
#include "sem_fpga.h"
#include "sem_common.h"
#include "sem/sem_tipc.h"
#include "product/board/board_feature.h"
#include "product/product_ax8610.h"
#include "product/product_ax7605i.h"
#include "product/product_feature.h"
#include "board/netlink.h"

extern board_fix_param_t *local_board;
extern int sem_write_cpld(int reg, int write_value);
extern int sem_read_cpld(int reg, int *read_value);
/*
extern product_fix_param_t *product;
extern board_info_syn_t board_info_to_syn;
extern product_info_syn_t product_info_syn;

extern DBusConnection *dcli_dbus_connection;

extern int disable_keep_alive_time;
extern int disable_keep_alive_flag_time;
extern int disable_keep_alive_start_time;
*/
car_linklist_node_t *car_head = NULL;
wan_if_linklist_node_t *wan_if_head = NULL;
car_white_list_node_t *car_white_head =NULL;
unsigned long long car_count = 0;
unsigned long long car_white_count = 0;
unsigned long long wan_if_count = 0;

unsigned char StorRegCount = sizeof(DataTempStorageArea)/sizeof(long);

unsigned char MibRegCount = sizeof(MibXauiRegOffsetAll)/sizeof(long);

wan_if_linklist_node_t *creat_wan_if_linklist(void)
{
    wan_if_head = (wan_if_linklist_node_t *)malloc(sizeof(struct wan_if_linklist_node_s));
	if(wan_if_head == NULL)
	{
        sem_syslog_dbg("function:creat_wan_if_linklist.malloc memory struct wan_if_linklist_node_s fail!\n");
		return NULL;
	}
	memset(wan_if_head,0,sizeof(struct wan_if_linklist_node_s));
	wan_if_head->next = NULL;
	return wan_if_head;
}

int ensure_wan_if_exist(char *name)
{
	wan_if_linklist_node_t *temp = NULL;

	temp = wan_if_head->next;
    while(temp)
    {
		sem_syslog_event("function:ensure_wan_if_exist;name = %s,temp->name = %s\n",name,temp->name);
        if(!strncmp(name,temp->name,WAN_IF_MAX))
        {
			return 1;
		}
		temp = temp->next;
	}
	sem_syslog_event("this interface %s is not exist\n",name);
	return 0;
}

int insert_wan_if_linklist(char *name)
{
	int ret=-1;
    wan_if_linklist_node_t *wan_if_list_node = NULL;
    wan_if_list_node = (wan_if_linklist_node_t *)malloc(sizeof(struct wan_if_linklist_node_s));
	if(wan_if_list_node == NULL)
	{
        sem_syslog_dbg("function:insert_wan_if_linklist;wan_if_name = %s malloc memory struct wan_if_linklist_node_s fail!\n",name);
		return 1;
	}
	ret = ensure_wan_if_exist(name);
	if(ret)
	{
        sem_syslog_dbg("function:insert_wan_if_linklist;interface %s is exist!\n",name);
		free(wan_if_list_node);
		return 2;
	}
	memset(wan_if_list_node,0,sizeof(struct wan_if_linklist_node_s));
	if(wan_if_count >= WAN_IF_MAX)
	{
	    sem_syslog_dbg("function:insert_wan_if_linklist;wan interface list is full!\n");
		free(wan_if_list_node);
		return 1;
	}
	else
	{
        wan_if_count++;
	}
	wan_if_list_node->next = wan_if_head->next;
	wan_if_head->next = wan_if_list_node;

	//wan_if_list_node->name = name;
	memcpy(wan_if_list_node->name,name,WAN_IF_NAME_LEN);
	sem_syslog_event("function:insert_wan_if_linklist;insert wan interface %s list node success\n",wan_if_list_node->name);
	return 0;
}

//return 1 for delete success;0 for no exit
int delete_wan_if_node(char *name)
{
    wan_if_linklist_node_t *temp_front = NULL;
	wan_if_linklist_node_t *temp_back = NULL;

	temp_front = wan_if_head;
	temp_back = wan_if_head->next;
    while(temp_back)
    {
		sem_syslog_event("function:delete_wan_if_node;name = %s,temp_back->name = %s\n",name,temp_back->name);
        if(!strncmp(name,temp_back->name,WAN_IF_MAX))
        {
            temp_front->next = temp_back->next;
			wan_if_count--;
			free(temp_back);
			return 1;
		}
		temp_back = temp_back->next;
		temp_front = temp_front->next;
	}
	sem_syslog_event("function:delete_wan_if_node;there isn't wan intreface %s node node in the list!\n",name);
	return 0;
}

wan_if_linklist_node_t *show_wan_if_node(unsigned long long count)
{
    wan_if_linklist_node_t *temp = NULL;

	if(count > wan_if_count)
	{
        sem_syslog_dbg("function:show_wan_if_node;the parameter count is large than wan_if_count\n");
		return NULL;
	}
	temp = wan_if_head;
	
	if(count)
	{
        while(count)
        {
            temp = temp->next;
    		count--;
			sem_syslog_event("function:show_wan_if_node;find wan intreface %s node in the list!\n",temp->name);
    	}
	    return temp;
	}
	else
	{
    	sem_syslog_dbg("function:show_wan_if_node;the count can't be zero\n");
		return NULL;
	}
}

car_white_list_node_t *creat_car_white_list(void)
{
    car_white_head = (car_white_list_node_t *)malloc(sizeof(struct car_white_list_node_s));
	if(car_white_head == NULL)
	{
        sem_syslog_warning("function:creat_car_white_list;creat car list.malloc memory struct car_white_list_node_s fail!\n");
		return NULL;
	}
	car_white_head->next = NULL;
	car_white_head->linkup = 0;//0--down;1--up;
	car_white_head->usr_ip = 0;
	return car_white_head;
}
//return 0 for insert fail;1 for insert success
int insert_car_white_list(unsigned int ip,unsigned short linkup)
{
    car_white_list_node_t *car_white_list_node = NULL;
    car_white_list_node = (car_white_list_node_t *)malloc(sizeof(struct car_white_list_node_s));
	if(car_white_list_node == NULL)
	{
        sem_syslog_warning("function:insert_car_white_list;linkstate = %d;ip = %x malloc memory struct car_white_list_node_s fail!\n",linkup,ip);
		return 0;
	}
	
	if(car_white_count >= CAR_WHITE_MAX)
	{
	    sem_syslog_warning("function:insert_car_white_list;car white list is full!\n");
		free(car_white_list_node);
		return 0;
	}
	else
	{
        car_white_count++;
	}	
	car_white_list_node->next = car_white_head->next;
	car_white_head->next = car_white_list_node;

	car_white_list_node->linkup = linkup;
	car_white_list_node->usr_ip = ip;
	sem_syslog_event("function:insert_car_white_list;insert car white list node success\n");
	return 1;
}
//return 1 for delete success;0 for no exit
int delete_car_white_node(unsigned int ip,unsigned short linkup)
{
    car_white_list_node_t *temp_front = NULL;
	car_white_list_node_t *temp_back = NULL;

	temp_front = car_white_head;
	temp_back = car_white_head->next;
    while(temp_back)
    {
        if(ip == temp_back->usr_ip && temp_back->linkup == linkup)
        {
			sem_syslog_event("function:delete_car_white_node;find linkstate = %d;ip = %x car node in the white list!\n",linkup,ip);
            temp_front->next = temp_back->next;
			car_white_count--;
			free(temp_back);
			return 1;
		}
		temp_back = temp_back->next;
		temp_front = temp_front->next;
	}
	sem_syslog_event("function:delete_car_white_node;there isn't linkstate = %d;ip = %x car node in the white list!\n",linkup,ip);
	return 0;
}

int read_car_white_linklist(unsigned long long car_white_node_now,read_car_white_node_param_t *car_white_node_param)
{
	int i=1;
	car_white_list_node_t *temp = car_white_head->next;
    if(car_white_head->next != NULL)
    {
        while(i != car_white_node_now)
        {
            temp = temp->next;
			i++;
		}
        car_white_node_param->usr_ip = temp->usr_ip;
    	car_white_node_param->linkup = temp->linkup;
		sem_syslog_event("function:read_car_white_linklist;car_white_node_now =%lld;ip = %x;linkstate = %d;\n",car_white_node_now,car_white_node_param->usr_ip,car_white_node_param->linkup);
        return 1;
	}
	sem_syslog_warning("function:read_car_white_linklist;car_node_now =%lld;the car white linklist is empty.\n",car_white_node_now);
	return 0;
}

/*exist return 1;no exist return 0*/
int ensure_car_white_node_exist(unsigned int ip,unsigned short linkup)
{
    car_white_list_node_t *temp = NULL;
	//sem_syslog_warning("ensure_car_white_node_exist****ip = %x;linkup = %d\n",ip,linkup);
    if(car_white_head->next == NULL)
    {
        //sem_syslog_dbg("function:ensure_car_white_node_exist,ip =%x;linkup =%d,the white car linklist is empty.\n",ip,linkup);
		return 0;
    }
	temp = car_white_head->next;

    while(temp)
    {
		if(temp->usr_ip == ip && temp->linkup == linkup )
		{
            sem_syslog_event("function:ensure_car_white_node_exist,ip %x is exist.\n",ip);
			return 1;
		}
		temp = temp->next;
	}
	sem_syslog_dbg("function:ensure_car_white_node_exist,ip =%x;linkup =%d is not exit.\n",ip,linkup);
	return 0;
}

//no use
void display_car_white_linklist(void)
{
    car_white_list_node_t *temp = NULL;
    if(car_white_head->next == NULL)
    {
        sem_syslog_warning("function:display_car_white_linklist,the car white linklist is empty.\n");
    }
	temp = car_white_head->next;
	sem_syslog_warning("ip          link\n");
    while(temp)
    {
		sem_syslog_warning("%x      %u\n",temp->usr_ip,temp->linkup);
		temp = temp->next;
	}
}

/*
*return 0 for no exit insert fail;1 for no exit insert success;2 for exit
*/
int fpga_car_whitelist_insert(unsigned int user_ip,unsigned short linkstate)
{
    int ret =0;
	
	ret = ensure_car_white_node_exist(user_ip,linkstate);
	if(ret == 0)
	{
        //sem_syslog_warning("ip %x  linkstate %d,is not exist.\n",user_ip,linkstate);
        ret = insert_car_white_list(user_ip,linkstate);
		if(ret == 0)
		{
            //sem_syslog_warning("ip %x  linkstate %d,insert fail.\n",user_ip,linkstate);
			return 0;
		}
		else
		{
            //sem_syslog_warning("ip %x  linkstate %d,insert success.\n",user_ip,linkstate);
			return 1;
		}
	}
	else if(ret == 1)
	{
        //sem_syslog_warning("ip %x  linkstate %d,is exist.\n",user_ip,linkstate);
		return 2;
		
	}
	
}
//return0 for bad result;1 for delete success!
int fpga_car_whitelist_delete(unsigned int user_ip,unsigned short linkstate)
{
	int ret = 0;

    ret = delete_car_white_node(user_ip,linkstate);
	if(ret == 0)
	{
        sem_syslog_warning("ip %x  linkstate %d,no exit.\n",user_ip,linkstate);
		return 0;
	}
	else
	{
        //sem_syslog_warning("ip %x  linkstate %d,delete success.\n",user_ip,linkstate);
		return 1;
	}
}

car_linklist_node_t *creat_car_linklist(void)
{
    car_head = (car_linklist_node_t *)malloc(sizeof(struct car_linklist_node_s));
	if(car_head == NULL)
	{
        sem_syslog_warning("function:creat_car_linklist.malloc memory struct car_linklist_node_s fail!\n");
		return NULL;
	}
	car_head->next = NULL;
	car_head->linkup = 0;//0--down;1--up;
	car_head->reload = 0;
	car_head->usr_ip = 0;
	car_head->configuration_attribute = 0;
	return car_head;
}

int insert_car_linklist(unsigned int ip,unsigned int reload,unsigned short linkup,unsigned short configuration_attribute)
{
    car_linklist_node_t *car_linklist_node = NULL;
    car_linklist_node = (car_linklist_node_t *)malloc(sizeof(struct car_linklist_node_s));
	if(car_linklist_node == NULL)
	{
        sem_syslog_warning("function:insert_car_linklist;ip = %x,linkstate = %d,malloc memory struct car_linklist_node_s fail!\n",ip,linkup);
		return 0;
	}
	
	if(car_count >= CAR_MAX)
	{
        sem_syslog_warning("function:insert_car_linklist;ip = %x,linkstate = %d,car link list is full!\n",ip,linkup);
		free(car_linklist_node);
		return 0;
	}
	else
	{
        car_count++;
	}
	
	car_linklist_node->next = car_head->next;
	car_head->next = car_linklist_node;

	car_linklist_node->linkup = linkup;
	car_linklist_node->reload = reload;
	car_linklist_node->usr_ip = ip;
	car_linklist_node->configuration_attribute = configuration_attribute;
	return 1;
}

/*no this car node return 0;delete this car node return 1;*/
int delete_car_node(unsigned int ip,unsigned short linkup)
{
    car_linklist_node_t *temp_front = NULL;
	car_linklist_node_t *temp_back = NULL;
    if(car_head->next == NULL)
    {
        sem_syslog_dbg("function:delete_car_node,ip =%x,linkstate = %d,find the car linklist is empty.\n",ip,linkup);
		return 0;
    }

	temp_front = car_head;
	temp_back = car_head->next;
    while(temp_back)
    {
        if(ip == temp_back->usr_ip && temp_back->linkup == linkup)
        {
            temp_front->next = temp_back->next;
			car_count--;
			free(temp_back);
			return 1;
		}
		temp_back = temp_back->next;
		temp_front = temp_front->next;
	}
	sem_syslog_dbg("there isn't linkstate = %d;ip = %x car node in the car list!\n",linkup,ip);
	return 0;
}

void delete_car_linklist(void)
{
    car_linklist_node_t *temp_front = NULL;
	car_linklist_node_t *temp_back = NULL;
	car_linklist_node_t *temp = NULL;
    if(car_head->next == NULL)
    {
        sem_syslog_dbg("function:delete_car_linklist,find the car linklist is empty.\n");
    }

	temp_front = car_head;
	temp_back = car_head->next;
    while(temp_back)
    {
        temp_front->next = temp_back->next;
		temp = temp_back;
		temp_back = temp_back->next;
		free(temp);
	}
	car_head->next = NULL;
	car_count = 0;
	sem_syslog_event("delete car linklist success!\n");
}

//no use
void display_car_linklist(void)
{
    car_linklist_node_t *temp = NULL;
    if(car_head->next == NULL)
    {
        sem_syslog_warning("the car linklist is empty.\n");
    }
	temp = car_head->next;
	sem_syslog_warning("ip          link      reload\n");
    while(temp)
    {
		sem_syslog_warning("%x      %u      %u\n",temp->usr_ip,temp->linkup,temp->reload);
		temp = temp->next;
	}
}

int read_car_linklist(unsigned long long car_node_now,read_car_node_param_t *car_node_param)
{
	int i=1;
	car_linklist_node_t *read_car_node_temp = car_head->next;
    if(car_head->next != NULL)
    {
        while(i!=car_node_now)
        {
            read_car_node_temp = read_car_node_temp->next;
			i++;
		}
        car_node_param->usr_ip = read_car_node_temp->usr_ip;
    	car_node_param->linkup = read_car_node_temp->linkup;
    	car_node_param->reload = read_car_node_temp->reload;
		car_node_param->configuration_attribute = read_car_node_temp->configuration_attribute;
		//sem_syslog_warning("fcar_node_now =%lld;ip = %x;linkstate = %d;linkstate = %d\n",car_node_now,read_car_node_temp->usr_ip,read_car_node_temp->linkup,read_car_node_temp->reload);
        return 1;
	}
	//sem_syslog_warning("function:read_car_linklist;car_node_now =%lld;the car linklist is empty.\n",car_node_now);
	return 0;
}

/*exist return 1;no exist return 0*/
car_linklist_node_t *ensure_car_node_exist(unsigned int ip,unsigned short linkup)
{
    car_linklist_node_t *temp = NULL;
	//sem_syslog_warning("ensure_car_node_exist****ip = %x;linkup = %d\n",ip,linkup);
    if(car_head->next == NULL)
    {
        //sem_syslog_event("function:ensure_car_node_exist,ip =%x;linkup =%d,the car linklist is empty.\n",ip,linkup);
		return NULL;
    }
	temp = car_head->next;

    while(temp)
    {
		if(temp->usr_ip == ip && temp->linkup == linkup )
		{
            //sem_syslog_warning("ip %x is exist.\n",ip);
			return temp;
		}
		temp = temp->next;
	}
	//sem_syslog_event("function:ensure_car_node_exist,ip =%x;linkup =%d is not exit.\n",ip,linkup);
	return NULL;
}

int write_fpga_reg(bm_op_args * ops) 
{
	int fd = 0;
	int retval;
	
	fd = open(FPGA_BM_FILE_PATH,0);	
	if (fd == -1)
	{
        sem_syslog_warning("Open file:/dev/bm0 error!\n");
		return -1;
	}

	retval = ioctl (fd,BM_IOC_FPGA_W,ops);
	if (0 ==retval) 
	{
		sem_syslog_event("R &w at addr [0x%016llx]",ops->op_addr);
		sem_syslog_event("            [0x%04x]\n",(unsigned short)ops->op_value);
	} 
	else 
	{
		sem_syslog_warning("Write failed return [%d]\n",retval);
		close(fd);
		return -1;
	}

	close(fd);
	return retval;

}

int read_fpga_reg(bm_op_args * ops)
{
	int fd = 0;
	int retval;
	
	fd = open(FPGA_BM_FILE_PATH,0);	
	if (fd == -1)
	{
        sem_syslog_warning("Open file:/dev/bm0 error!\n");
		return -1;
	}

	retval = ioctl (fd,BM_IOC_G_,ops);
	if (0 == retval) 
	{
		sem_syslog_event("Read at addr [0x%016llx]",ops->op_addr);
		sem_syslog_event("            [0x%04x]\n",(unsigned short)ops->op_value);
	} 
	else 
	{
		sem_syslog_warning("Read failed return [%d]\n",retval);
	}

	close(fd);
	return retval;

}

int write_cam_core(
	unsigned short Indirect_add_reg_H,
	unsigned short Indirect_add_reg_L,
	unsigned short SIP_H,
	unsigned short SIP_L,
    unsigned short DIP_H,
	unsigned short DIP_L,
	unsigned short SPORT,
	unsigned short DPORT,
	unsigned short PROTOCOL)
{
	bm_op_args opt;
	int ret;
    
    opt.op_len = 16;
    /*start write source ip address */		
	opt.op_value = SIP_H;
	opt.op_addr = CAM_CORE_REG_0;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write source ip [31:16] value failure!\n");
		return 0;
	}
	usleep(5000);
	
    opt.op_value = SIP_L;
	opt.op_addr = CAM_CORE_REG_1;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write source ip [15:0] value failure!\n");
		return 0;
	}
	usleep(5000);
    /*end write source ip */

    /*start write destination ip */		
	opt.op_value = DIP_H;
	opt.op_addr = CAM_CORE_REG_2;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write destination ip [31:16] value failure!\n");
		return 0;
	}
	usleep(5000);
    opt.op_value = DIP_L;
	opt.op_addr = CAM_CORE_REG_3;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write destination ip [15:0] value failure!\n");
		return 0;
	}
	usleep(5000);
    /*end write destination ip */

    /*start write source port */		
	opt.op_value = SPORT;     
	opt.op_addr = CAM_CORE_REG_4;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write source port value failure!\n");
		return 0;
	}
	usleep(5000);
    /*end write source port */

    /*start write destination port */		
	opt.op_value = DPORT;     
	opt.op_addr = CAM_CORE_REG_5;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write destination port value failure!\n");
		return 0;
	}
	usleep(5000);
    /*end write destination port */

    /*start write protocol */	
	opt.op_value = PROTOCOL;
	opt.op_addr = CAM_CORE_REG_6;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write protocol value failure!\n");
		return 0;
	}
	usleep(5000);
    /*end write protocol */
	
    /*start write 0080 */	
	opt.op_value = 0x0080;
	opt.op_addr = CAM_CORE_REG_7;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write protocol value failure!\n");
		return 0;
	}
	usleep(5000);
    /*end write 0080 */
	
	/* write cam_id to address register*/
	opt.op_value = Indirect_add_reg_L;
	opt.op_addr = INDIRECT_ACCESS_ADDRESS_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write cam_id L value failure!\n");
		return 0;
	}
	usleep(5000);
	
	opt.op_value = Indirect_add_reg_H;
	opt.op_addr = INDIRECT_ACCESS_ADDRESS_REG_H;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write cam_id H value failure!\n");
		return 0;
	}
	usleep(5000);
    /* end */


	/* cam core into write operation */
	opt.op_addr = INDIRECT_ACCESS_CONTROL_REG;
	opt.op_value = 0x8040;     /* write operation-- set bit 15 for 1,cam core select_id=0x4 */
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write fpga indirect access control register failure!\n");
		return 0;
	}
	return 1;
    /*end*/
}

//
int read_cam_core(
	read_cam_core_result_t *result,
	unsigned short SIP_H,
	unsigned short SIP_L,
    unsigned short DIP_H,
	unsigned short DIP_L,
	unsigned short SPORT,
	unsigned short DPORT,
	unsigned short PROTOCOL)
{
	//read_cam_core_t result;
	bm_op_args opt;
	int ret;
	int match_flag = -1;
	int cam_id = -1;
	
	opt.op_len = 16;

    /*start write source ip address */		
	opt.op_value = SIP_H;
	opt.op_addr = CAM_CORE_REG_0;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write source ip [31:16] value failure!\n");
		return 0;
	}
	usleep(5000);
    opt.op_value = SIP_L;
	opt.op_addr = CAM_CORE_REG_1;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write source ip [15:0] value failure!\n");
		return 0;
	}
	usleep(5000);
    /*end write source ip */

    /*start write destination ip */		
	opt.op_value = DIP_H;
	opt.op_addr = CAM_CORE_REG_2;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write destination ip [31:16] value failure!\n");
		return 0;
	}
	usleep(5000);
    opt.op_value = DIP_L;
	opt.op_addr = CAM_CORE_REG_3;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write destination ip [15:0] value failure!\n");
		return 0;
	}
	usleep(5000);
    /*end write destination ip */

    /*start write source port */		
	opt.op_value = SPORT;     
	opt.op_addr = CAM_CORE_REG_4;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write source port value failure!\n");
		return 0;
	}
	usleep(5000);
    /*end write source port */

    /*start write destination port */		
	opt.op_value = DPORT;     
	opt.op_addr = CAM_CORE_REG_5;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write destination port value failure!\n");
		return 0;
	}
	usleep(5000);
    /*end write destination port */

    /*start write protocol */
	opt.op_value = PROTOCOL;
	opt.op_addr = CAM_CORE_REG_6;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write protocol value failure!\n");
		return 0;
	}
	usleep(5000);
    /*end write protocol */

	/*start write 0080 */	
	opt.op_value = 0x0080;
	opt.op_addr = CAM_CORE_REG_7;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write protocol value failure!\n");
		return 0;
	}
	usleep(5000);
    /*end write 0080 */

	/* cam core into read operation */
	opt.op_addr = INDIRECT_ACCESS_CONTROL_REG;
	opt.op_value = CAM_CORE_VALUE;     /* read operation-- set bit 15 for 0,cam core select_id=0x4 */
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write fpga indirect access control register failure!\n");
		return 0;
	}
	usleep(5000);
    /*end*/

    /* wait ready register into 1 */
	int ready = 0;
	opt.op_addr = INDIRECT_ACCESS_STATE_REG;
    while(ready == 0)
	{
		ret = read_fpga_reg(&opt);
		if(ret)
		{
			sem_syslog_warning("**read fpga indirect access status register failure!\n");
			return 0;
		}
        ready = opt.op_value & READY_REG_VALUE;           //Check Status of ready
    }
	
	/*end*/
	
    /*start read cam_id and match_flag*/		
	opt.op_addr = CAM_CORE_REG_0;
	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**read cam core register 0 failure!\n");
		return 0;
	}
	usleep(5000);
	
    if(0 == ((opt.op_value & 0x40) >> 6))
	{
        sem_syslog_warning("cam have not this tuple\n");
	}
	else
	{
		match_flag = (unsigned short)((opt.op_value & 0x40) >> 6);
		//sem_syslog_warning("\nmatch flag is 0x%x\n",match_flag);
		cam_id = (unsigned short)(opt.op_value & 0x3f);
	    //sem_syslog_warning("cam id is 0x%x\n\n",cam_id);

		result->match_flag = match_flag;
		result->cam_id = cam_id;
	}    
	return 1;
}

/// flag:hash table flag
int read_hash(struct hash_table_s *stat,unsigned int hash_id,int flag)
{
 	int ret;
	int k;
	int start = 0;	
	bm_op_args opt;
	
	unsigned int temp_value1 = 0;
	unsigned int temp_value2 = 0;
	unsigned int value[sizeof(DataTempStorageArea)/sizeof(long)]; /* register value */
	int ready = 0;
	
    /*test whether the  previous operation is completed */
	memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;
	opt.op_addr = INDIRECT_ACCESS_STATE_REG;
	while(ready == 0)
    {
		ret = read_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**read fpga indirect access control register failure!\n");
    		ret = -1;
    	}
        ready = opt.op_value & READY_REG_VALUE;           //Check Status of ready
	}
	ready = 0;
	/*end*/

	/* write table id to address register*/	
	memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;
	opt.op_value = hash_id & 0xffff;
	opt.op_addr = INDIRECT_ACCESS_ADDRESS_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write hash table value failure!\n");
		return 0;
	}

    opt.op_value = hash_id >> 16;
	opt.op_addr = INDIRECT_ACCESS_ADDRESS_REG_H;        /* write table id to address register*/
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("##write hash table value failure!\n");
		return 0;
	}	
    /* end */
	
	/* hash table into read operation */
	opt.op_addr = INDIRECT_ACCESS_CONTROL_REG;
	if(flag == 1)
	{
	    opt.op_value = HASH_TABLE_1_VALUE;
	}
	if(flag == 2)
	{
	    opt.op_value = HASH_TABLE_2_VALUE;
	}
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write fpga indirect access control register failure!\n");
		return 0;
	}		
    /*end*/

    /* wait ready register into 1 */
	opt.op_addr = INDIRECT_ACCESS_STATE_REG;
    while(ready == 0)
	{
		ret = read_fpga_reg(&opt);
		if(ret)
		{
			sem_syslog_warning("**read fpga indirect access control register failure!\n");
			return 0;
		}
    	ready = opt.op_value ;    //Read ready Register
        ready = ready & READY_REG_VALUE;           //Check Status of ready
    }
	
	//sem_syslog_warning("***** ready == 1 ****\n");
	/*end*/

    memset(value,0,sizeof(value));
	for(k=start;k < StorRegCount; k++)
	{
		opt.op_addr = Data_Temp_Stor_Base_Addr + DataTempStorageArea[k];
		ret = read_fpga_reg(&opt);
		if(ret)
		{
			sem_syslog_warning("**read hash table failure!  k = %d**\n",k);
			return 0;
		}

		value[k] = opt.op_value;
	}

    k = start;
	/* Cycle_1 */	
    /* source ip address */
    stat->sip_H= value[k];
	stat->sip_L= value[k+1];
    k +=2;
	
    /* destination ip address */
    stat->dip_H= value[k];
    stat->dip_L= value[k+1];
    k +=2;
	
    /* source port */
    stat->sport = value[k];
    k +=1;
	
    /* destination port */
    stat->dport = value[k];
    k +=1;
	
	/* usr protocol */
	stat->protocol = value[k];    
    /* setup time high 8*/
	temp_value1 = (value[k] & 0xff) << 8;
    k +=1;
	
    /* setup_time low 8*/
	temp_value2 = value[k] >> 8;
    stat->setup_time = temp_value1 + temp_value2;
	/* hash_ctl */
	stat->hash_ctl = value[k];
    k +=1;

	/* Cycle_2 */
    /* destination mac */
    stat->dmac_H = value[k];
	stat->dmac_M = value[k+1];
    stat->dmac_L = value[k+2];
    k +=3;
	
    /* source mac */
    stat->smac_H = value[k];
    stat->smac_M = value[k+1];
	stat->smac_L = value[k+2];
    k +=3;
	
    /* vlan out */
    stat->vlan_out_H= value[k];
	stat->vlan_out_L= value[k+1];
    k +=2;
	
	/* Cycle_3 */
    /* vlan in */
    stat->vlan_in_H= value[k];
	stat->vlan_in_L= value[k+1];
    k +=2;
	
    /* ethernet protocol */
    stat->ethernet_protocol = value[k];
    k +=1;
	
    /* ip TOS */
	stat->ip_tos = value[k];
    k +=1;
	
    /* ip len */
    stat->ip_len = value[k];
    k +=1;
	
    /* ip identification */
    stat->ip_identification = value[k];
    k +=1;
	
    /* ip offset */
    stat->ip_offset = value[k];
    k +=1;
	
    /* ip ttl */
	stat->ip_ttl = value[k] >> 8;
    /* ip protocol */
	stat->ip_protocol = value[k];
    k +=1;

    /* Cycle_4 */
    /* ip checksum */
    stat->ip_checksum = value[k];
    k +=1;
	
    /* tunnel source ip */
    stat->tunnel_sip_H = value[k];
	stat->tunnel_sip_L = value[k+1];
    k +=2;
	
    /* tunnel destination ip */
    stat->tunnel_dip_H= value[k];
	stat->tunnel_dip_L= value[k+1];
    k +=2;
	
    /* tunnel src port */
    stat->tunnel_sport= value[k];
    k +=1;
	
    /* tunnel dst port */
    stat->tunnel_dport= value[k];
    k +=1;
	
    /* udp len */
    stat->udp_len= value[k];
    k +=1;

	/* Cycle_5 */
    /* udp checksum */
    stat->udp_checksum= value[k];
    k +=1;

    /* capwap head */
    stat->capwap_header_b0 = value[k];
    k +=1;
    stat->capwap_header_b2 = value[k];
    k +=1;
    stat->capwap_header_b4 = value[k];
    k +=1;
    stat->capwap_header_b6 = value[k];
    k +=1;
    stat->capwap_header_b8 = value[k];
    k +=1;
    stat->capwap_header_b10 = value[k];
    k +=1;
    stat->capwap_header_b12 = value[k];
    k +=1;
    stat->capwap_header_b14 = value[k];
    k +=1;

    /* 802.11 head */
    stat->header_802_11_b0= value[k];
    k +=1;
    stat->header_802_11_b2= value[k];
    k +=1;
    stat->header_802_11_b4= value[k];
    k +=1;
    stat->header_802_11_b6= value[k];
    k +=1;
    stat->header_802_11_b8= value[k];
    k +=1;
    stat->header_802_11_b10= value[k];
    k +=1;
    stat->header_802_11_b12= value[k];
    k +=1;
    stat->header_802_11_b14= value[k];
    k +=1;
    stat->header_802_11_b16= value[k];
    k +=1;
    stat->header_802_11_b18= value[k];
    k +=1;
    stat->header_802_11_b20= value[k];
    k +=1;
    stat->header_802_11_b22= value[k];
    k +=1;
    stat->header_802_11_b24= value[k];
    k +=1;
    stat->header_802_11_b26= value[k];
    k +=1;
	/*capwap protocol type*/
    stat->capwap_protocol = value[k];
    k +=1;
/*
	sem_syslog_warning("\nDetail Information of HASH Table %x\n",hash_id);
 	sem_syslog_warning("=====================================================================\n");
    sem_syslog_warning("\nUSR");

	sem_syslog_warning("\tsource ip address H:                %x\n",stat->sip_H);
	sem_syslog_warning("\tsource ip address L:                %x\n",stat->sip_L);
    sem_syslog_warning("\tdestination ip address H:           %x\n",stat->dip_H);	
	sem_syslog_warning("\tdestination ip address L:           %x\n",stat->dip_L);
	sem_syslog_warning("\tsource port:                      %x\n",stat->sport);
	sem_syslog_warning("\tdestination port:                 %x\n",stat->dport);
	sem_syslog_warning("\tprotocol:                         %x\n",stat->protocol);
	sem_syslog_warning("\thash table setup time:            %x\n",stat->setup_time);
	sem_syslog_warning("\thash ctl:                         %x\n",stat->hash_ctl);

	sem_syslog_warning("\nMAC");
	
    sem_syslog_warning("\tdestination mac:                  %x\n",stat->dmac_H);
	sem_syslog_warning("\tdestination mac:                  %x\n",stat->dmac_M);
	sem_syslog_warning("\tdestination mac:                  %x\n",stat->dmac_L);
    sem_syslog_warning("\tsource mac:                       %x\n",stat->smac_H);
	sem_syslog_warning("\tsource mac:                       %x\n",stat->smac_M);
	sem_syslog_warning("\tsource mac:                       %x\n",stat->smac_L);
	sem_syslog_warning("\tvlan out:                         %x\n",stat->vlan_out_H);
	sem_syslog_warning("\tvlan out:                         %x\n",stat->vlan_out_L);
    sem_syslog_warning("\tvlan in:                          %x\n",stat->vlan_in_H);
	sem_syslog_warning("\tvlan in:                          %x\n",stat->vlan_in_L);
	sem_syslog_warning("\tethernet protocol:                %x\n",stat->ethernet_protocol);
    
    sem_syslog_warning("\nIP");
	
    sem_syslog_warning("\tip tos:                           %x\n",stat->ip_tos);
	sem_syslog_warning("\tip length:                        %x\n",stat->ip_len);
	sem_syslog_warning("\tip offset:                        %x\n",stat->ip_offset);
    sem_syslog_warning("\tip ttl:                           %x\n",stat->ip_ttl);
	sem_syslog_warning("\tip protocol:                      %x\n",stat->ip_protocol);
	sem_syslog_warning("\tip checksum:                      %x\n",stat->ip_checksum);

	sem_syslog_warning("\nUDP");

    sem_syslog_warning("\ttunnel source ip:                 %x\n",stat->tunnel_sip_H);
	sem_syslog_warning("\ttunnel source ip:                 %x\n",stat->tunnel_sip_L);
    sem_syslog_warning("\ttunnel destination ip:            %x\n",stat->tunnel_dip_H);
	sem_syslog_warning("\ttunnel destination ip:            %x\n",stat->tunnel_dip_L);
    sem_syslog_warning("\ttunnel source port:               %x\n",stat->tunnel_sport);		
    sem_syslog_warning("\ttunnel destination port:          %x\n",stat->tunnel_dport);
	sem_syslog_warning("\tudp length:                       %x\n",stat->udp_len);
	sem_syslog_warning("\tudp checksum:                     %x\n",stat->udp_checksum);

    sem_syslog_warning("\nCAPWAP");
	
	sem_syslog_warning("\tcapwap header:                    0x%x%x%x%x%x%x%x%x\n",stat->capwap_header_b0,stat->capwap_header_b2,stat->capwap_header_b4,stat->capwap_header_b6,stat->capwap_header_b8,stat->capwap_header_b10,stat->capwap_header_b12,stat->capwap_header_b14);
	sem_syslog_warning("\t802.11 header:                    0x%x%x%x%x%x%x%x%x%x%x%x%x%x%x\n",stat->header_802_11_b0,stat->header_802_11_b2,stat->header_802_11_b4,stat->header_802_11_b6,stat->header_802_11_b8,stat->header_802_11_b10,stat->header_802_11_b12,stat->header_802_11_b14,stat->header_802_11_b16,stat->header_802_11_b18,stat->header_802_11_b20,stat->header_802_11_b22,stat->header_802_11_b24,stat->header_802_11_b26);
	sem_syslog_warning("\tprotocols type:                   %x\n",stat->capwap_protocol);

    sem_syslog_warning("\n");
    sem_syslog_warning("=====================================================================\n");
*/
	return 1;
}

// flag:hash table flag
int write_hash(write_cam_param_t *write_cam_param,int flag)
{
    bm_op_args opt;
    int ret;
	unsigned short hash_id_0;
	unsigned short hash_id_1;
	int ready = 0;
	
    /*test whether the  previous operation is completed */
	memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;
	opt.op_addr = INDIRECT_ACCESS_STATE_REG;
	while(ready == 0)
    {
		ret = read_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**read fpga indirect access control register failure!\n");
    		ret = -1;
    	}
        ready = opt.op_value & READY_REG_VALUE;           //Check Status of ready
	}
	ready = 0;
	/*end*/

	memset(&opt,0,sizeof(bm_op_args));	
	opt.op_len = 16;

	/* write hash id to address register*/
	hash_id_0 = ((write_cam_param->Indirect_add_reg_L & 0xffff) >> 8) + ((write_cam_param->Indirect_add_reg_L & 0xff) <<8);
	hash_id_1 = (write_cam_param->Indirect_add_reg_H & 0x3f) << 8;
	//sem_syslog_warning("**%x\n",(write_cam_param->Indirect_add_reg_L & 0xffff) >> 8);
	//sem_syslog_warning("**%x\n",(write_cam_param->Indirect_add_reg_L & 0xff) <<8);
	//sem_syslog_warning("**hash_id_1   %x\n",hash_id_1);
	//sem_syslog_warning("**hash_id_0   %x\n",hash_id_0);
	opt.op_value = hash_id_0;
	opt.op_addr = HASH_TABLE_ID_REG_0;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write hash table id value failure!\n");
		return 0;
	}

	opt.op_value = hash_id_1;  /* write hash_id[21:16] to data buffer address 1*/	
	opt.op_addr = HASH_TABLE_ID_REG_1;  
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("##write hash table id value failure!\n");
		return 0;
	}	
    /* end */

    /*start write source ip address */		
	opt.op_value = write_cam_param->SIP_H;
	opt.op_addr = SOURCE_IP_ADDRESS_REG_H;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write source ip [31:16] value failure!\n");
		return 0;
	}
    opt.op_value = write_cam_param->SIP_L;
	opt.op_addr = SOURCE_IP_ADDRESS_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write source ip [15:0] value failure!\n");
		return 0;
	}
    /*end write source ip */

    /*start write destination ip */		
	opt.op_value = write_cam_param->DIP_H;
	opt.op_addr = DESTINATION_IP_ADDRESS_REG_H;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write destination ip [31:16] value failure!\n");
		return 0;
	}
    opt.op_value = write_cam_param->DIP_L;
	opt.op_addr = DESTINATION_IP_ADDRESS_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write destination ip [15:0] value failure!\n");
		return 0;
	}
    /*end write destination ip */

    /*start write source port */		
	opt.op_value = write_cam_param->SPORT;     
	opt.op_addr = SOURCE_PORT_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write source port value failure!\n");
		return 0;
	}
    /*end write source port */

    /*start write destination port */		
		opt.op_value = write_cam_param->DPORT;     
	opt.op_addr = DESTINATION_PORT_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write destination port value failure!\n");
		return 0;
	}
    /*end write destination port */

    /*start write protocol */	
	opt.op_value = write_cam_param->PROTOCOL;
	opt.op_addr = PROTOCOL_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write protocol value failure!\n");
		return 0;
	}
    /*end write protocol */

    /*start write hash ctl */	
	opt.op_value = write_cam_param->HASH_CTL;
	opt.op_addr = HASHCTL_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write hash ctl value failure!\n");
		return 0;
	}
    /*end write hash ctl */
	
    /*start write destination mac address */		
	opt.op_value = write_cam_param->DMAC_H;   /*[47:32]*/
	opt.op_addr = DESTINATION_MAC_ADDRESS_REG_H;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write destination mac [47:32] value failure!\n");
		return 0;
	}
	opt.op_value = write_cam_param->DMAC_M;  /*[31:16]*/
	opt.op_addr = DESTINATION_MAC_ADDRESS_REG_M;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write destination mac [31:16] value failure!\n");
		return 0;
	}
	opt.op_value = write_cam_param->DMAC_L;  /*[15:0]*/
	opt.op_addr = DESTINATION_MAC_ADDRESS_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write destination mac [15:0] value failure!\n");
		return 0;
	}
    /*end write destination mac */

    /*start write source mac address */		
	opt.op_value = write_cam_param->SMAC_H;   /*[47:32]*/
	opt.op_addr = SOURCE_MAC_ADDRESS_REG_H;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write source mac [47:32] value failure!\n");
		return 0;
	}
	opt.op_value = write_cam_param->SMAC_M;  /*[31:16]*/
	opt.op_addr = SOURCE_MAC_ADDRESS_REG_M;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write source mac [31:16] value failure!\n");
		return 0;
	}
	opt.op_value = write_cam_param->SMAC_L;  /*[15:0]*/
	opt.op_addr = SOURCE_MAC_ADDRESS_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write source mac [15:0] value failure!\n");
		return 0;
	}
    /* end write source mac */


    /* start write vlan out */		
	opt.op_value = write_cam_param->VLAN_OUT_H;
	opt.op_addr = VLAN_OUT_REG_H;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write vlan out [31:16] value failure!\n");
		return 0;
	}
    opt.op_value = write_cam_param->VLAN_OUT_L;
	opt.op_addr = VLAN_OUT_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write vlan out [15:0] value failure!\n");
		return 0;
	}
    /* end write vlan out */

    /* start write vlan in */	
	opt.op_value = write_cam_param->VLAN_IN_H;
	opt.op_addr = VLAN_IN_REG_H;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write vlan in [31:16] value failure!\n");
		return 0;
	}
    opt.op_value = write_cam_param->VLAN_IN_L;
	opt.op_addr = VLAN_IN_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write vlan in [15:0] value failure!\n");
		return 0;
	}
    /* end write vlan in */

	opt.op_value = write_cam_param->FIRST_0800;
	opt.op_addr = ETHERNET_PROTOCOL_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write 0800 value failure!\n");
		return 0;
	}
    usleep(5000);

    /* start write ip tos */	
	opt.op_value = write_cam_param->IPTOS;
	opt.op_addr = IP_TOS_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write ip tos value failure!\n");
		return 0;
	}
    /* end write ip tos */

    /* start write ip len */	
	opt.op_value = write_cam_param->IP_LEN;
	opt.op_addr = IP_LEN_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write ip LEN value failure!\n");
		return 0;
	}
    /* end write ip len */

    /* start write ip id */	
	opt.op_value = write_cam_param->IP_ID;
	opt.op_addr = IP_ID_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write ip id value failure!\n");
		return 0;
	}
    /* end write ip id */

    /* start write ip office */	
	opt.op_value = write_cam_param->IP_OFFSET;
	opt.op_addr = IP_OFFSET_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write ip offset value failure!\n");
		return 0;
	}
    /* end write ip office */

    /* start write ip ttl */	
	opt.op_value = write_cam_param->IP_TTL_IP_PROTOCOL;
	opt.op_addr = IP_TTL_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write ip tos value failure!\n");
		return 0;
	}
    /* end write ip ttl */

    /* start write ip checksum */	
	opt.op_value = write_cam_param->IP_CHECKSUM;
	opt.op_addr = IP_CHECKSUM_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write ip checksum value failure!\n");
		return 0;
	}
    /* end write ip checksum */

    /*start write tunnel source ip address */		
	opt.op_value = write_cam_param->TUNNEL_SIP_H;
	opt.op_addr = TUNNEL_SOURCE_IP_ADDRESS_REG_H;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write tunnel source ip [31:16] value failure!\n");
		return 0;
	}
    opt.op_value = write_cam_param->TUNNEL_SIP_L;
	opt.op_addr = TUNNEL_SOURCE_IP_ADDRESS_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write tunnel source ip [15:0] value failure!\n");
		return 0;
	}
    /*end write tunnel source ip */
	
	/*start write tunnel destination ip */		
	opt.op_value = write_cam_param->TUNNEL_DIP_H;
	opt.op_addr = TUNNEL_DESTINATION_IP_ADDRESS_REG_H;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write tunnel destination ip [31:16] value failure!\n");
		return 0;
	}
    opt.op_value = write_cam_param->TUNNEL_DIP_L;
	opt.op_addr = TUNNEL_DESTINATION_IP_ADDRESS_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write tunnel destination ip [15:0] value failure!\n");
		return 0;
	}
    /*end write tunnel destination ip */

    /*start write tunnel source port */		
	opt.op_value = write_cam_param->TUNNEL_SRC_PORT;     
	opt.op_addr = TUNNEL_SOURCE_PORT_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write tunnel source port value failure!\n");
		return 0;
	}
    /*end write tunnel source port */

    /*start write tunnel destination port */		
	opt.op_value = write_cam_param->TUNNEL_DST_PORT;     
	opt.op_addr = TUNNEL_DESTINATION_PORT_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write tunnel destination port value failure!\n");
		return 0;
	}
    /*end write tunnel destination port */

    /*start write udp len */		
	opt.op_value = write_cam_param->UDP_LEN;     
	opt.op_addr = UDP_LEN_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write udp len value failure!\n");
		return 0;
	}
    /*end write udp len */

    /*start write udp CHECKSUM */		
	opt.op_value = write_cam_param->UDP_CHECKSUM;     
	opt.op_addr = UDP_CHECKSUM_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write udp CHECKSUM value failure!\n");
		return 0;
	}
    /*end write udp CHECKSUM */

    /* start write capwap header */		

	opt.op_value = write_cam_param->CAPWAP_B0B1;
	opt.op_addr = CAPWAP_HEADER_REG_B0;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write capwap header [B0] value failure!\n");
		return 0;
	}
	opt.op_value = write_cam_param->CAPWAP_B2B3;
	//temp_value = (unsigned short)(opt.op_value);
	//vty_out(vty,"******capwap header register b2 = 0x%x******\n",temp_value);
	opt.op_addr = CAPWAP_HEADER_REG_B2;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write capwap header [B2] value failure!\n");
		return 0;
	}
	opt.op_value = write_cam_param->CAPWAP_B4B5;
	opt.op_addr = CAPWAP_HEADER_REG_B4;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write capwap header [B4] value failure!\n");
		return 0;
	}
	opt.op_value = write_cam_param->CAPWAP_B6B7;
	opt.op_addr = CAPWAP_HEADER_REG_B6;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write capwap header [B6] value failure!\n");
		return 0;
	}
	opt.op_value = write_cam_param->CAPWAP_B8B9;
	opt.op_addr = CAPWAP_HEADER_REG_B8;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write capwap header [B8] value failure!\n");
		return 0;
	}
	opt.op_value = write_cam_param->CAPWAP_B10B11;
	opt.op_addr = CAPWAP_HEADER_REG_B10;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write capwap header [B10] value failure!\n");
		return 0;
	}
	opt.op_value = write_cam_param->CAPWAP_B12B13;
	opt.op_addr = CAPWAP_HEADER_REG_B12;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write capwap header [B12] value failure!\n");
		return 0;
	}
	opt.op_value = write_cam_param->CAPWAP_B14B15;
	opt.op_addr = CAPWAP_HEADER_REG_B14;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write capwap header [B14] value failure!\n");
		return 0;
	}
    /*end write capwap header */


    /*start write 802.11 header */
	
	opt.op_value = write_cam_param->H_802_11_B0B1;
	opt.op_addr = HEADER_802_11_REG_B0;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write 802.11  header [B0] value failure!\n");
		return 0;
	}
	opt.op_value = write_cam_param->H_802_11_B2B3;
	opt.op_addr = HEADER_802_11_REG_B2;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write 802.11  header [B2] value failure!\n");
		return 0;
	}
	opt.op_value = write_cam_param->H_802_11_B4B5;
	opt.op_addr = HEADER_802_11_REG_B4;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write 802.11  header [B4] value failure!\n");
		return 0;
	}
	opt.op_value = write_cam_param->H_802_11_B6B7;
	opt.op_addr = HEADER_802_11_REG_B6;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write 802.11  header [B6] value failure!\n");
		return 0;
	}
	opt.op_value = write_cam_param->H_802_11_B8B9;
	opt.op_addr = HEADER_802_11_REG_B8;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("** write 802.11 header [B8] value failure!\n");
		return 0;
	}
	opt.op_value = write_cam_param->H_802_11_B10B11;
	opt.op_addr = HEADER_802_11_REG_B10;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write 802.11 header [B10] value failure!\n");
		return 0;
	}
	opt.op_value = write_cam_param->H_802_11_B12B13;
	opt.op_addr = HEADER_802_11_REG_B12;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write 802.11 header [B12] value failure!\n");
		return 0;
	}
	opt.op_value = write_cam_param->H_802_11_B14B15;
	opt.op_addr = HEADER_802_11_REG_B14;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write 802.11 header [B14] value failure!\n");
		return 0;
	}
	opt.op_value = write_cam_param->H_802_11_B16B17;
	opt.op_addr = HEADER_802_11_REG_B16;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write 802.11  header [B16] value failure!\n");
		return 0;
	}
	opt.op_value = write_cam_param->H_802_11_B18B19;
	opt.op_addr = HEADER_802_11_REG_B18;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write 802.11  header [B18] value failure!\n");
		return 0;
	}
	opt.op_value = write_cam_param->H_802_11_B20B21;
	opt.op_addr = HEADER_802_11_REG_B20;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write 802.11  header [B20] value failure!\n");
		return 0;
	}
	opt.op_value = write_cam_param->H_802_11_B22B23;
	opt.op_addr = HEADER_802_11_REG_B22;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write 802.11  header [B22] value failure!\n");
		return 0;
	}
	opt.op_value = write_cam_param->H_802_11_B24B25;
	opt.op_addr = HEADER_802_11_REG_B24;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("** write 802.11 header [B24] value failure!\n");
		return 0;
	}
	opt.op_value = write_cam_param->H_802_11_B26B27;
	opt.op_addr = HEADER_802_11_REG_B26;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write 802.11 header [B26] value failure!\n");
		return 0;
	}
	
    /*end write 802.11 header */
	opt.op_value = write_cam_param->SEC_0800;
	opt.op_addr = CAPWAP_HEADER_PROTOCOL_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write SEC_0800 value failure!\n");
		return 0;
	}

	/* cam table into write operation */
	opt.op_addr = INDIRECT_ACCESS_CONTROL_REG;
	if(flag == 1)
	{
	    opt.op_value = 0x8010;//hash table 1
	}
	if(flag == 2)
	{
        opt.op_value = 0x8020;//hash table 2
	}
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write fpga indirect access control register failure!\n");
		return 0;
	}
    /*end*/
	return 1;
}

int write_cam(write_cam_param_t *write_cam_param)
{
    bm_op_args opt;
    int ret;

	memset(&opt,0,sizeof(bm_op_args));	
	opt.op_len = 16;
	
	/* write cam id to address register*/
	opt.op_value = write_cam_param->Indirect_add_reg_L;
	opt.op_addr = INDIRECT_ACCESS_ADDRESS_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write cam table value failure!\n");
		return 0;
	}
	usleep(5000);

    opt.op_value = write_cam_param->Indirect_add_reg_H;
	opt.op_addr = INDIRECT_ACCESS_ADDRESS_REG_H;        /* write table id to address register*/
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("##write cam table value failure!\n");
		return 0;
	}
	usleep(5000);
    /* end */

    /*start write source ip address */		
	opt.op_value = write_cam_param->SIP_H;
	opt.op_addr = SOURCE_IP_ADDRESS_REG_H;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write source ip [31:16] value failure!\n");
		return 0;
	}
	usleep(5000);
    opt.op_value = write_cam_param->SIP_L;
	opt.op_addr = SOURCE_IP_ADDRESS_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write source ip [15:0] value failure!\n");
		return 0;
	}
	usleep(5000);
    /*end write source ip */

    /*start write destination ip */		
	opt.op_value = write_cam_param->DIP_H;
	opt.op_addr = DESTINATION_IP_ADDRESS_REG_H;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write destination ip [31:16] value failure!\n");
		return 0;
	}
	usleep(5000);
    opt.op_value = write_cam_param->DIP_L;
	opt.op_addr = DESTINATION_IP_ADDRESS_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write destination ip [15:0] value failure!\n");
		return 0;
	}
	usleep(5000);
    /*end write destination ip */

    /*start write source port */		
	opt.op_value = write_cam_param->SPORT;     
	opt.op_addr = SOURCE_PORT_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write source port value failure!\n");
		return 0;
	}
	usleep(5000);
    /*end write source port */

    /*start write destination port */		
		opt.op_value = write_cam_param->DPORT;     
	opt.op_addr = DESTINATION_PORT_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write destination port value failure!\n");
		return 0;
	}
	usleep(5000);
    /*end write destination port */

    /*start write protocol */	
	opt.op_value = write_cam_param->PROTOCOL;
	opt.op_addr = PROTOCOL_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write protocol value failure!\n");
		return 0;
	}
	usleep(5000);
    /*end write protocol */

    /*start write hash ctl */	
	opt.op_value = write_cam_param->HASH_CTL;
	opt.op_addr = HASHCTL_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write hash ctl value failure!\n");
		return 0;
	}
	usleep(5000);
    /*end write hash ctl */
	
    /*start write destination mac address */		
	opt.op_value = write_cam_param->DMAC_H;   /*[47:32]*/
	opt.op_addr = DESTINATION_MAC_ADDRESS_REG_H;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write destination mac [47:32] value failure!\n");
		return 0;
	}
	usleep(5000);
	opt.op_value = write_cam_param->DMAC_M;  /*[31:16]*/
	opt.op_addr = DESTINATION_MAC_ADDRESS_REG_M;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write destination mac [31:16] value failure!\n");
		return 0;
	}
	usleep(5000);
	opt.op_value = write_cam_param->DMAC_L;  /*[15:0]*/
	opt.op_addr = DESTINATION_MAC_ADDRESS_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write destination mac [15:0] value failure!\n");
		return 0;
	}
    usleep(5000);
    /*end write destination mac */

    /*start write source mac address */		
	opt.op_value = write_cam_param->SMAC_H;   /*[47:32]*/
	opt.op_addr = SOURCE_MAC_ADDRESS_REG_H;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write source mac [47:32] value failure!\n");
		return 0;
	}
	usleep(5000);
	opt.op_value = write_cam_param->SMAC_M;  /*[31:16]*/
	opt.op_addr = SOURCE_MAC_ADDRESS_REG_M;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write source mac [31:16] value failure!\n");
		return 0;
	}
	usleep(5000);
	opt.op_value = write_cam_param->SMAC_L;  /*[15:0]*/
	opt.op_addr = SOURCE_MAC_ADDRESS_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write source mac [15:0] value failure!\n");
		return 0;
	}
	usleep(5000);
    /* end write source mac */


    /* start write vlan out */		
	opt.op_value = write_cam_param->VLAN_OUT_H;
	opt.op_addr = VLAN_OUT_REG_H;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write vlan out [31:16] value failure!\n");
		return 0;
	}
	usleep(5000);
    opt.op_value = write_cam_param->VLAN_OUT_L;
	opt.op_addr = VLAN_OUT_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write vlan out [15:0] value failure!\n");
		return 0;
	}
	usleep(5000);
    /* end write vlan out */

    /* start write vlan in */	
	opt.op_value = write_cam_param->VLAN_IN_H;
	opt.op_addr = VLAN_IN_REG_H;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write vlan in [31:16] value failure!\n");
		return 0;
	}
	usleep(5000);
    opt.op_value = write_cam_param->VLAN_IN_L;
	opt.op_addr = VLAN_IN_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write vlan in [15:0] value failure!\n");
		return 0;
	}
	usleep(5000);
    /* end write vlan in */

	opt.op_value = 0x0800;
	opt.op_addr = ETHERNET_PROTOCOL_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write PROTOCOL [15:0] value failure!\n");
		return 0;
	}
    usleep(5000);
	
    /* start write ip tos */	
	opt.op_value = write_cam_param->IPTOS;
	opt.op_addr = IP_TOS_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write ip tos value failure!\n");
		return 0;
	}
	usleep(5000);
    /* end write ip tos */
	
	opt.op_value = 0;
	opt.op_addr = IP_LEN_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write IP_ID tos value failure!\n");
		return 0;
	}
	usleep(5000);
	
	opt.op_value = 0;
	opt.op_addr = IP_ID_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write IP_ID tos value failure!\n");
		return 0;
	}
	usleep(5000);

	opt.op_value = 0;
	opt.op_addr = IP_OFFSET_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write IP_OFF tos value failure!\n");
		return 0;
	}
	usleep(5000);
    /* start write ip ttl */	
	opt.op_value = write_cam_param->IP_TTL_IP_PROTOCOL;
	opt.op_addr = IP_TTL_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write ip_TTL tos value failure!\n");
		return 0;
	}
	usleep(5000);
    /* end write ip ttl */

    /* start write ip checksum */	
	opt.op_value = write_cam_param->IP_CHECKSUM;
	opt.op_addr = IP_CHECKSUM_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write ip checksum value failure!\n");
		return 0;
	}
    /* end write ip checksum */

    /*start write tunnel source ip address */		
	opt.op_value = write_cam_param->TUNNEL_SIP_H;
	opt.op_addr = TUNNEL_SOURCE_IP_ADDRESS_REG_H;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write tunnel source ip [31:16] value failure!\n");
		return 0;
	}
	usleep(5000);
    opt.op_value = write_cam_param->TUNNEL_SIP_L;
	opt.op_addr = TUNNEL_SOURCE_IP_ADDRESS_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write tunnel source ip [15:0] value failure!\n");
		return 0;
	}
	usleep(5000);
    /*end write tunnel source ip */
	
	/*start write tunnel destination ip */		
	opt.op_value = write_cam_param->TUNNEL_DIP_H;
	opt.op_addr = TUNNEL_DESTINATION_IP_ADDRESS_REG_H;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write tunnel destination ip [31:16] value failure!\n");
		return 0;
	}
	usleep(5000);
    opt.op_value = write_cam_param->TUNNEL_DIP_L;
	opt.op_addr = TUNNEL_DESTINATION_IP_ADDRESS_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write tunnel destination ip [15:0] value failure!\n");
		return 0;
	}
	usleep(5000);
    /*end write tunnel destination ip */

    /*start write tunnel source port */		
	opt.op_value = write_cam_param->TUNNEL_SRC_PORT;     
	opt.op_addr = TUNNEL_SOURCE_PORT_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write tunnel source port value failure!\n");
		return 0;
	}
	usleep(5000);
    /*end write tunnel source port */

    /*start write tunnel destination port */		
	opt.op_value = write_cam_param->TUNNEL_DST_PORT;     
	opt.op_addr = TUNNEL_DESTINATION_PORT_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write tunnel destination port value failure!\n");
		return 0;
	}
	usleep(5000);
    /*end write tunnel destination port */

    /*start write udp len reg*/		
	opt.op_value = 0;     
	opt.op_addr = UDP_LEN_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write udp len value failure!\n");
		return 0;
	}
	usleep(5000);
    /*end udp len reg */

    /*start write udp checksum reg*/		
	opt.op_value = 0;     
	opt.op_addr = UDP_CHECKSUM_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write udp checksum value failure!\n");
		return 0;
	}
	usleep(5000);
    /*end write udp checksum reg */


    /* start write capwap header */		

	opt.op_value = write_cam_param->CAPWAP_B0B1;
	opt.op_addr = CAPWAP_HEADER_REG_B0;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write capwap header [B0] value failure!\n");
		return 0;
	}
	usleep(5000);
	opt.op_value = write_cam_param->CAPWAP_B2B3;
	//temp_value = (unsigned short)(opt.op_value);
	//vty_out(vty,"******capwap header register b2 = 0x%x******\n",temp_value);
	opt.op_addr = CAPWAP_HEADER_REG_B2;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write capwap header [B2] value failure!\n");
		return 0;
	}
    usleep(5000);
	opt.op_value = write_cam_param->CAPWAP_B4B5;
	opt.op_addr = CAPWAP_HEADER_REG_B4;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write capwap header [B4] value failure!\n");
		return 0;
	}
    usleep(5000);
	opt.op_value = write_cam_param->CAPWAP_B6B7;
	opt.op_addr = CAPWAP_HEADER_REG_B6;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write capwap header [B6] value failure!\n");
		return 0;
	}
    usleep(5000);
	opt.op_value = write_cam_param->CAPWAP_B8B9;
	opt.op_addr = CAPWAP_HEADER_REG_B8;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write capwap header [B8] value failure!\n");
		return 0;
	}
    usleep(5000);
	opt.op_value = write_cam_param->CAPWAP_B10B11;
	opt.op_addr = CAPWAP_HEADER_REG_B10;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write capwap header [B10] value failure!\n");
		return 0;
	}
    usleep(5000);
	opt.op_value = write_cam_param->CAPWAP_B12B13;
	opt.op_addr = CAPWAP_HEADER_REG_B12;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write capwap header [B12] value failure!\n");
		return 0;
	}
    usleep(5000);
	opt.op_value = write_cam_param->CAPWAP_B14B15;
	opt.op_addr = CAPWAP_HEADER_REG_B14;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write capwap header [B14] value failure!\n");
		return 0;
	}
	usleep(5000);
    /*end write capwap header */


    /*start write 802.11 header */
	
	opt.op_value = write_cam_param->H_802_11_B0B1;
	opt.op_addr = HEADER_802_11_REG_B0;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write 802.11  header [B0] value failure!\n");
		return 0;
	}
    usleep(5000);	
	opt.op_value = write_cam_param->H_802_11_B2B3;
	opt.op_addr = HEADER_802_11_REG_B2;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write 802.11  header [B2] value failure!\n");
		return 0;
	}
    usleep(5000);
	opt.op_value = write_cam_param->H_802_11_B4B5;
	opt.op_addr = HEADER_802_11_REG_B4;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write 802.11  header [B4] value failure!\n");
		return 0;
	}
    usleep(5000);
	opt.op_value = write_cam_param->H_802_11_B6B7;
	opt.op_addr = HEADER_802_11_REG_B6;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write 802.11  header [B6] value failure!\n");
		return 0;
	}
    usleep(5000);
	opt.op_value = write_cam_param->H_802_11_B8B9;
	opt.op_addr = HEADER_802_11_REG_B8;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("** write 802.11 header [B8] value failure!\n");
		return 0;
	}
    usleep(5000);
	opt.op_value = write_cam_param->H_802_11_B10B11;
	opt.op_addr = HEADER_802_11_REG_B10;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write 802.11 header [B10] value failure!\n");
		return 0;
	}
    usleep(5000);
	opt.op_value = write_cam_param->H_802_11_B12B13;
	opt.op_addr = HEADER_802_11_REG_B12;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write 802.11 header [B12] value failure!\n");
		return 0;
	}
    usleep(5000);
	opt.op_value = write_cam_param->H_802_11_B14B15;
	opt.op_addr = HEADER_802_11_REG_B14;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write 802.11 header [B14] value failure!\n");
		return 0;
	}
    usleep(5000);	
	opt.op_value = write_cam_param->H_802_11_B16B17;
	opt.op_addr = HEADER_802_11_REG_B16;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write 802.11  header [B16] value failure!\n");
		return 0;
	}
    usleep(5000);	
	opt.op_value = write_cam_param->H_802_11_B18B19;
	opt.op_addr = HEADER_802_11_REG_B18;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write 802.11  header [B18] value failure!\n");
		return 0;
	}
    usleep(5000);
	opt.op_value = write_cam_param->H_802_11_B20B21;
	opt.op_addr = HEADER_802_11_REG_B20;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write 802.11  header [B20] value failure!\n");
		return 0;
	}
    usleep(5000);
	opt.op_value = write_cam_param->H_802_11_B22B23;
	opt.op_addr = HEADER_802_11_REG_B22;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write 802.11  header [B22] value failure!\n");
		return 0;
	}
    usleep(5000);
	opt.op_value = write_cam_param->H_802_11_B24B25;
	opt.op_addr = HEADER_802_11_REG_B24;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("** write 802.11 header [B24] value failure!\n");
		return 0;
	}
    usleep(5000);
	opt.op_value = write_cam_param->H_802_11_B26B27;
	opt.op_addr = HEADER_802_11_REG_B26;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write 802.11 header [B26] value failure!\n");
		return 0;
	}
    usleep(5000);
    /*end write 802.11 header */
	
	opt.op_value = 0x0800;
	opt.op_addr = CAPWAP_HEADER_PROTOCOL_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write 802.11 header [B26] value failure!\n");
		return 0;
	}
    usleep(5000);

	/* cam table into write operation */
	opt.op_addr = INDIRECT_ACCESS_CONTROL_REG;
	opt.op_value = 0x8050;     /* read operation-- set bit 15 for 1,cam table select_id=0x5 */
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write fpga indirect access control register failure!\n");
		return 0;
	}
	usleep(5000);
    /*end*/
	return 1;	
}

int read_cam(read_cam_result_t *read_cam_result,unsigned short Indirect_add_reg_H,unsigned short Indirect_add_reg_L)
{
	int k;
	bm_op_args opt;
	int ret;
	int ready = 0;
    struct hash_cam_table_s stat;
	unsigned int temp_value1 = 0;
	unsigned int temp_value2 = 0;
	unsigned short value[sizeof(DataTempStorageArea)/sizeof(long)]; /* register value */
    unsigned short cam_table_id = Indirect_add_reg_L;

	memset(&opt,0,sizeof(bm_op_args));	
	opt.op_len = 16;
	/* write table id to address register*/
	opt.op_value = Indirect_add_reg_L;
	opt.op_addr = INDIRECT_ACCESS_ADDRESS_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write cam table value failure!\n");
		return 0;
	}
    usleep(5000);
    opt.op_value = Indirect_add_reg_H;
	opt.op_addr = INDIRECT_ACCESS_ADDRESS_REG_H;        /* write table id to address register*/
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("##write cam table value failure!\n");
		return 0;
	}
	usleep(5000);
    /* end */
	
	/* cam table into read operation */
	opt.op_addr = INDIRECT_ACCESS_CONTROL_REG;
	opt.op_value = CAM_TABLE_VALUE;     /* read operation-- set bit 15 for 0,cam table select_id=0x5,sub_table_select for 0x0 */
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write fpga indirect access control register failure!\n");
		return 0;
	}
	usleep(5000);
    /*end*/

    /* wait ready register into 1 */
	opt.op_addr = INDIRECT_ACCESS_STATE_REG;
    while(ready == 0)
	{
		ret = read_fpga_reg(&opt);
		if(ret)
		{
			sem_syslog_warning("**read fpga indirect access status register failure!\n");
			return 0;
		}
    	ready = opt.op_value ;    //Read ready Register value
        ready = ready & READY_REG_VALUE;           //Check Status of ready
    }
	
	/*end*/

    memset(value,0,sizeof(value));
	int start = 0;
	for(k=start;k < StorRegCount; k++)
	{
		opt.op_addr = Data_Temp_Stor_Base_Addr + DataTempStorageArea[k];
		ret = read_fpga_reg(&opt);
		if(ret)
		{
			sem_syslog_warning("**read cam table failure!  k = %d**\n",k);
			return 0;
		}
        usleep(5000);
		value[k] = opt.op_value;
	}
	
    k = start;
	/* Cycle_1 */	
    /* source ip address */
    stat.src.sip_H  = (value[k]);
    stat.src.sip_L  = (value[k+1]);
    k +=2;
	
    /* destination ip address */
    stat.dst.dip_H = (value[k]);
	stat.dst.dip_L = (value[k+1]);
    k +=2;
	
    /* source port */
    stat.src.sport = value[k];
    k +=1;
	
    /* destination port */
    stat.dst.dport = value[k];
    k +=1;
	
	/* usr protocol */
	stat.protocol = value[k] >> 8;
    /* setup time high 8*/
	temp_value1 = (value[k] & 0xff) << 8;
    k +=1;
	
    /* setup_time low 8*/
	temp_value2 = value[k] >> 8;
    stat.setup_time = temp_value1 + temp_value2;
	/* hash_ctl */
	stat.hash_ctl = value[k] & 0xff;
    k +=1;

	/* Cycle_2 */
    /* destination mac */
    stat.dst.dmac_H = value[k];
    stat.dst.dmac_M = value[k+1];
	stat.dst.dmac_L = value[k+2];
    k +=3;
	
    /* source mac */
    stat.src.smac_H = value[k];
    stat.src.smac_M = value[k+1];
	stat.src.smac_L = value[k+2];
    k +=3;
	
    /* vlan out */
    stat.vlan_out_H = value[k];
	stat.vlan_out_L = value[k+1];
    k +=2;
	
	/* Cycle_3 */
    /* vlan in */
    stat.vlan_in_H = value[k];
	stat.vlan_in_L = value[k+1];
    k +=2;
	
    /* ethernet protocol */
    stat.ethernet_protocol = value[k];
    k +=1;
	
    /* ip TOS */
	stat.ip_tos = value[k] & 0xff;
    k +=1;
	
    /* ip len */
    stat.ip_len = value[k];
    k +=1;
	
    /* ip identification */
    stat.ip_identification = value[k];
    k +=1;
	
    /* ip offset */
    stat.ip_offset = value[k];
    k +=1;
	
    /* ip ttl */
	stat.ip_ttl = value[k] >> 8;
    /* ip protocol */
	stat.ip_protocol = value[k] & 0xff;
    k +=1;

    /* Cycle_4 */
    /* ip checksum */
    stat.ip_checksum = value[k];
    k +=1;
	
    /* tunnel source ip */
    stat.src.tunnel_sip_H = value[k];
	stat.src.tunnel_sip_L = value[k+1];
    k +=2;
	
    /* tunnel destination ip */
    stat.dst.tunnel_dip_H= value[k];
	stat.dst.tunnel_dip_L= value[k+1];
    k +=2;
	
    /* tunnel src port */
    stat.src.tunnel_sport= value[k];
    k +=1;
	
    /* tunnel dst port */
    stat.dst.tunnel_dport= value[k];
    k +=1;
	
    /* udp len */
    stat.udp_len= value[k];
    k +=1;

	/* Cycle_5 */
    /* udp checksum */
    stat.udp_checksum= value[k];
    k +=1;

    /* capwap head */
    stat.capwap_header_b0 = value[k];
    k +=1;
    stat.capwap_header_b2 = value[k];
    k +=1;
    stat.capwap_header_b4 = value[k];
    k +=1;
    stat.capwap_header_b6 = value[k];
    k +=1;
    stat.capwap_header_b8 = value[k];
    k +=1;
    stat.capwap_header_b10 = value[k];
    k +=1;
    stat.capwap_header_b12 = value[k];
    k +=1;
    stat.capwap_header_b14 = value[k];
    k +=1;

    /* 802.11 head */
    stat.header_802_11_b0= value[k];
    k +=1;
    stat.header_802_11_b2= value[k];
    k +=1;
    stat.header_802_11_b4= value[k];
    k +=1;
    stat.header_802_11_b6= value[k];
    k +=1;
    stat.header_802_11_b8= value[k];
    k +=1;
    stat.header_802_11_b10= value[k];
    k +=1;
    stat.header_802_11_b12= value[k];
    k +=1;
    stat.header_802_11_b14= value[k];
    k +=1;
    stat.header_802_11_b16= value[k];
    k +=1;
    stat.header_802_11_b18= value[k];
    k +=1;
    stat.header_802_11_b20= value[k];
    k +=1;
    stat.header_802_11_b22= value[k];
    k +=1;
    stat.header_802_11_b24= value[k];
    k +=1;
    stat.header_802_11_b26= value[k];
    k +=1;
	/*capwap protocol type*/
    stat.capwap_protocol = value[k];
    k +=1;

/*  
	sem_syslog_warning("\nDetail Information of CAM Table %x\n",cam_table_id);
 	sem_syslog_warning("=====================================================================\n");
    sem_syslog_warning("\nUSR");

	sem_syslog_warning("\tsource ip address H:                0x%x\n",stat.src.sip_H);
	sem_syslog_warning("\tsource ip address L:                0x%x\n",stat.src.sip_L);
    sem_syslog_warning("\tdestination ip address H:           0x%x\n",stat.dst.dip_H);
	sem_syslog_warning("\tdestination ip address L:           0x%x\n",stat.dst.dip_L);
	sem_syslog_warning("\tsource port:                      0x%x\n",stat.src.sport);
	sem_syslog_warning("\tdestination port:                 0x%x\n",stat.dst.dport);
	sem_syslog_warning("\tprotocol:                         0x%x\n",stat.protocol);
	sem_syslog_warning("\thash table setup time:            0x%x\n",stat.setup_time);
	sem_syslog_warning("\thash ctl:                         0x%x\n",stat.hash_ctl);

	sem_syslog_warning("\nMAC");
	
    sem_syslog_warning("\tdestination mac H:                  0x%x\n",stat.dst.dmac_H);
	sem_syslog_warning("\tdestination mac M:                  0x%x\n",stat.dst.dmac_M);
	sem_syslog_warning("\tdestination mac L:                  0x%x\n",stat.dst.dmac_L);
    sem_syslog_warning("\tsource mac H:                       0x%x\n",stat.src.smac_H);
	sem_syslog_warning("\tsource mac M:                       0x%x\n",stat.src.smac_M);
	sem_syslog_warning("\tsource mac L:                       0x%x\n",stat.src.smac_L);
	sem_syslog_warning("\tvlan out H:                         0x%x\n",stat.vlan_out_H);
	sem_syslog_warning("\tvlan out H:                         0x%x\n",stat.vlan_out_L);
    sem_syslog_warning("\tvlan in L:                          0x%x\n",stat.vlan_in_H);
	sem_syslog_warning("\tvlan in L:                          0x%x\n",stat.vlan_in_L);
	sem_syslog_warning("\tethernet protocol:                0x%x\n",stat.ethernet_protocol);
    
    sem_syslog_warning("\nIP");
	
    sem_syslog_warning("\tip tos:                           0x%x\n",stat.ip_tos);
	sem_syslog_warning("\tip length:                        0x%x\n",stat.ip_len);
	sem_syslog_warning("\tip offset:                        0x%x\n",stat.ip_offset);
    sem_syslog_warning("\tip ttl:                           0x%x\n",stat.ip_ttl);
	sem_syslog_warning("\tip protocol:                      0x%x\n",stat.ip_protocol);
	sem_syslog_warning("\tip checksum:                      0x%x\n",stat.ip_checksum);

	sem_syslog_warning("\nUDP");

    sem_syslog_warning("\ttunnel source ip H:                 0x%x\n",stat.src.tunnel_sip_H);
	sem_syslog_warning("\ttunnel source ip L:                 0x%x\n",stat.src.tunnel_sip_L);
    sem_syslog_warning("\ttunnel destination ip H:            0x%x\n",stat.dst.tunnel_dip_H);
	sem_syslog_warning("\ttunnel destination ip L:            0x%x\n",stat.dst.tunnel_dip_L);	
    sem_syslog_warning("\ttunnel source port:               0x%x\n",stat.src.tunnel_sport);		
    sem_syslog_warning("\ttunnel destination port:          0x%x\n",stat.dst.tunnel_dport);
	sem_syslog_warning("\tudp length:                       0x%x\n",stat.udp_len);
	sem_syslog_warning("\tudp checksum:                     0x%x\n",stat.udp_checksum);

    sem_syslog_warning("\nCAPWAP");
	
	sem_syslog_warning("\tcapwap header:                    0x%x%x%x%x%x%x%x%x\n",stat.capwap_header_b0,stat.capwap_header_b2,stat.capwap_header_b4,stat.capwap_header_b6,stat.capwap_header_b8,stat.capwap_header_b10,stat.capwap_header_b12,stat.capwap_header_b14);
	sem_syslog_warning("\t802.11 header:                    0x%x%x%x%x%x%x%x%x%x%x%x%x%x%x\n",stat.header_802_11_b0,stat.header_802_11_b2,stat.header_802_11_b4,stat.header_802_11_b6,stat.header_802_11_b8,stat.header_802_11_b10,stat.header_802_11_b12,stat.header_802_11_b14,stat.header_802_11_b16,stat.header_802_11_b18,stat.header_802_11_b20,stat.header_802_11_b22,stat.header_802_11_b24,stat.header_802_11_b26);
	sem_syslog_warning("\tprotocols type:                   0x%x\n",stat.capwap_protocol);

    sem_syslog_warning("\n");
    sem_syslog_warning("=====================================================================\n");
*/
	read_cam_result->cam_table_id = cam_table_id;
	read_cam_result->sip_H = stat.src.sip_H;
	read_cam_result->sip_L = stat.src.sip_L;
	read_cam_result->dip_H = stat.dst.dip_H;
	read_cam_result->dip_L = stat.dst.dip_L;	
	read_cam_result->sport = stat.src.sport;
	read_cam_result->dport = stat.dst.dport;
	read_cam_result->protocol = stat.protocol;
	read_cam_result->setup_time = stat.setup_time;	
	read_cam_result->hash_ctl = stat.hash_ctl;
	read_cam_result->dmac_H = stat.dst.dmac_H;
	read_cam_result->dmac_M = stat.dst.dmac_M;
	read_cam_result->dmac_L = stat.dst.dmac_L;	
	read_cam_result->smac_H = stat.src.smac_H;
	read_cam_result->smac_M = stat.src.smac_M;
	read_cam_result->smac_L = stat.src.smac_L;
	read_cam_result->vlan_out_H = stat.vlan_out_H;	
	read_cam_result->vlan_out_L = stat.vlan_out_L;
	read_cam_result->vlan_in_H = stat.vlan_in_H;
	read_cam_result->vlan_in_L = stat.vlan_in_L;
	read_cam_result->ethernet_protocol = stat.ethernet_protocol;	
	read_cam_result->ip_tos = stat.ip_tos;
	read_cam_result->ip_len = stat.ip_len;
	read_cam_result->ip_offset = stat.ip_offset;
	read_cam_result->ip_ttl = stat.ip_ttl;	
	read_cam_result->ip_protocol = stat.ip_protocol;
	read_cam_result->ip_checksum = stat.ip_checksum;
	read_cam_result->tunnel_sip_H = stat.src.tunnel_sip_H;
	read_cam_result->tunnel_sip_L = stat.src.tunnel_sip_L;	
	read_cam_result->tunnel_dip_H = stat.dst.tunnel_dip_H;
	read_cam_result->tunnel_dip_L = stat.dst.tunnel_dip_L;
	read_cam_result->tunnel_sport = stat.src.tunnel_sport;
	read_cam_result->tunnel_dport = stat.dst.tunnel_dport;
	read_cam_result->udp_len = stat.udp_len;
	read_cam_result->udp_checksum = stat.udp_checksum;
	read_cam_result->capwap_header_b0 = stat.capwap_header_b0;
	read_cam_result->capwap_header_b2 = stat.capwap_header_b2;	
	read_cam_result->capwap_header_b4 = stat.capwap_header_b4;
	read_cam_result->capwap_header_b6 = stat.capwap_header_b6;
	read_cam_result->capwap_header_b8 = stat.capwap_header_b8;
	read_cam_result->capwap_header_b10 = stat.capwap_header_b10;	
	read_cam_result->capwap_header_b12 = stat.capwap_header_b12;
	read_cam_result->capwap_header_b14 = stat.capwap_header_b14;
	read_cam_result->header_802_11_b0 = stat.header_802_11_b0;
	read_cam_result->header_802_11_b2 = stat.header_802_11_b2;	
	read_cam_result->header_802_11_b4 = stat.header_802_11_b4;
	read_cam_result->header_802_11_b6 = stat.header_802_11_b6;
	read_cam_result->header_802_11_b8 = stat.header_802_11_b8;
	read_cam_result->header_802_11_b10 = stat.header_802_11_b10;
    read_cam_result->header_802_11_b12 = stat.header_802_11_b12;
	read_cam_result->header_802_11_b14 = stat.header_802_11_b14;
	read_cam_result->header_802_11_b16 = stat.header_802_11_b16;
	read_cam_result->header_802_11_b18 = stat.header_802_11_b18;
    read_cam_result->header_802_11_b20 = stat.header_802_11_b20;
	read_cam_result->header_802_11_b22 = stat.header_802_11_b22;
	read_cam_result->header_802_11_b24 = stat.header_802_11_b24;
	read_cam_result->header_802_11_b26 = stat.header_802_11_b26;
    read_cam_result->capwap_protocol = stat.capwap_protocol;
	
	return 1;  
}
#if 0
int write_car(unsigned short Indirect_add_reg_H,
                unsigned short Indirect_add_reg_L,
                unsigned short RELOAD_H,
                unsigned short RELOAD_L,
                unsigned short LINKUP,
                unsigned short CAR_VALID,
                unsigned int CAR_ID,
                int FLAG)
{
	int ret;
	bm_op_args opt;
	int ready = 0;
	
	memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;
	opt.op_addr = INDIRECT_ACCESS_STATE_REG;
	while(ready == 0)
    {
		ret = read_fpga_reg(&opt);
    	if(ret == -1)
    	{
    		sem_syslog_warning("**when read car,read fpga indirect access control register failure!\n");
    	}
        ready = opt.op_value & READY_REG_VALUE;           //Check Status of ready
	}
	
	ready = 0;

	memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;

	/* write reload to data cache register 8*/
	opt.op_value = RELOAD_L;
	opt.op_addr = SOURCE_IP_ADDRESS_REG_H;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write car reload value failure!\n");
		return 0;
	}

    /* write car_valid linkup reload to data cache register 9*/
    opt.op_value = RELOAD_H+(LINKUP<<14)+(CAR_VALID<<15);
	opt.op_addr = SOURCE_IP_ADDRESS_REG_L;        
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("##write linkup value failure!\n");
		return 0;
	}
    /* end */

    /* write usr ip to temporary data cache register 10 */
    opt.op_value = Indirect_add_reg_L;
	opt.op_addr = DESTINATION_IP_ADDRESS_REG_H;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write usr ip [15:0] value failure!\n");
		return 0;
	}
	opt.op_value = Indirect_add_reg_H;
	opt.op_addr = DESTINATION_IP_ADDRESS_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write usr ip [31:16] value failure!\n");
		return 0;
	}
    /*end write usr ip */

    /* write credit to temporary data cache register 12 */
    opt.op_value = 0;//CREDIT_L
	opt.op_addr = SOURCE_PORT_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write credit [15:0] value failure!\n");
		return 0;
	}
	opt.op_value = 0;//CREDIT_H
	opt.op_addr = DESTINATION_PORT_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write credit [31:16] value failure!\n");
		return 0;
	}
    /*end write credit */

    /*start write byte drop count [39:32] to temporary data cache register 14*/		
	opt.op_value = 0;//BYTEDROPCOUNT_H
	opt.op_addr = PROTOCOL_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write byte drop count [39:32] to temporary data cache register failure!\n");
		return 0;
	}
	/*end*/

	/*start write byte sent count [39:32] to temporary data cache register 15*/		
    opt.op_value = 0;//BYTESENTCOUNT_H
	opt.op_addr = HASHCTL_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write byte sent count [39:32] to temporary data cache register failure!\n");
		return 0;
	}
    /*end*/

    /*start write byte sent count [15:0] to temporary data cache register 16*/		
    opt.op_value = 0;//BYTESENTCOUNT_L
	opt.op_addr = DESTINATION_MAC_ADDRESS_REG_H;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write byte sent count [15:0] to temporary data cache register failure!\n");
		return 0;
	}
    /*end*/
	
    /*start write byte sent count [31:16] to temporary data cache register 17*/		
    opt.op_value = 0;//BYTESENTCOUNT_M
	opt.op_addr = DESTINATION_MAC_ADDRESS_REG_M;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write byte sent count [31:16] to temporary data cache register failure!\n");
		return 0;
	}
    /*end*/

    /*start write byte drop count [15:0] to temporary data cache register 18*/		
	opt.op_value = 0;//BYTEDROPCOUNT_L
	opt.op_addr = DESTINATION_MAC_ADDRESS_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write byte drop count [15:0] to temporary data cache register failure!\n");
		return 0;
	}
	/*end*/

    /*start write byte drop count [31:16] to temporary data cache register 19*/	
	opt.op_value = 0;//BYTEDROPCOUNT_M
	opt.op_addr = SOURCE_MAC_ADDRESS_REG_H;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write byte drop count [31:16] to temporary data cache register failure!\n");
		return 0;
	}
	/*end*/

    /*start write package sent count [15:0] to temporary data cache register 20*/		
	opt.op_value = 0;//PACKAGESENTCOUNT_L
	opt.op_addr = SOURCE_MAC_ADDRESS_REG_M;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write  package sent count [15:0] to temporary data cache register failure!\n");
		return 0;
	}
	/*end*/

    /*start write package sent count [31:16] to temporary data cache register 21*/	
	opt.op_value = 0;//PACKAGESENTCOUNT_H
	opt.op_addr = SOURCE_MAC_ADDRESS_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write  package sent count [31:16] to temporary data cache register failure!\n");
		return 0;
	}
	/*end*/

    /*start write package drop count [15:0] to temporary data cache register 22*/		
	opt.op_value = 0;//PACKAGEDROPCOUNT_L
	opt.op_addr = VLAN_OUT_REG_H;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write  package drop count [15:0] to temporary data cache register failure!\n");
		return 0;
	}
	/*end*/

    /*start write package drop count [31:16] to temporary data cache register 23*/	
	opt.op_value = 0;//PACKAGEDROPCOUNT_H
	opt.op_addr = VLAN_OUT_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write  package drop count [31:16] to temporary data cache register failure!\n");
		return 0;
	}
	/*end*/
	if(FLAG == 1)
	{
    	opt.op_value = CAR_ID & 0Xffff;
    	opt.op_addr = INDIRECT_ACCESS_ADDRESS_REG_L;
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**write car CAR_ID value failure!\n");
    		return 0;
    	}
		opt.op_value = (CAR_ID & 0X30000)>>16;
    	opt.op_addr = INDIRECT_ACCESS_ADDRESS_REG_H;
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**write car CAR_ID value failure!\n");
    		return 0;
    	}
	}
	else
	{
    	opt.op_value = Indirect_add_reg_L;
    	opt.op_addr = INDIRECT_ACCESS_ADDRESS_REG_L;
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**write car CAR_ID value failure!\n");
    		return 0;
    	}
		opt.op_value = Indirect_add_reg_H;
    	opt.op_addr = INDIRECT_ACCESS_ADDRESS_REG_H;
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**write car CAR_ID value failure!\n");
    		return 0;
    	}        
	}

	/* car table into write operation */
	opt.op_addr = INDIRECT_ACCESS_CONTROL_REG;
	if(FLAG == 1)
	{
        opt.op_value = 0x8034+LINKUP;
	}
    else
    {
	    opt.op_value = 0x8030+LINKUP;     /* read operation-- set bit 15 for 1,car table select_id=0x3 */
    }
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write fpga indirect access control register failure!\n");
		return 0;
	}

	return 1;
    /*end*/
}
#endif

//FLAG:1 for car_id ;0 for car IP
//return 1 for success;o for fail
int read_car(read_car_result_t * read_car_result,unsigned short Indirect_add_reg_H,unsigned short Indirect_add_reg_L,unsigned short LINKUP,unsigned int CAR_ID,int FLAG)
{
	int ret;
	int k;
	int start = 0;	
	bm_op_args opt;
	unsigned long ready = 0;
	//unsigned short car_table_id;
	
    struct car_table_s stat;
	unsigned long long temp_value = 0;
	unsigned long long value[sizeof(DataTempStorageArea)/sizeof(long)]; 

	memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;
	opt.op_addr = INDIRECT_ACCESS_STATE_REG;
	while(ready == 0)
    {
		ret = read_fpga_reg(&opt);
    	if(ret == -1)
    	{
    		sem_syslog_warning("**when read car,read fpga indirect access control register failure!\n");
    	}
        ready = opt.op_value & READY_REG_VALUE;           //Check Status of ready
	}

	ready = 0;
	if(FLAG == 1)
	{
    	memset(&opt,0,sizeof(bm_op_args));
    	opt.op_len = 16;	
        opt.op_value = CAR_ID & 0xffff;
        opt.op_addr = INDIRECT_ACCESS_ADDRESS_REG_L;
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    	    sem_syslog_warning("**write car table id to indirect access address register lower failure!\n");
    	    return 0;
    	}
		opt.op_len = 16;	
        opt.op_value = (CAR_ID & 0xffff0000)>>16;
        opt.op_addr = INDIRECT_ACCESS_ADDRESS_REG_H;
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    	    sem_syslog_warning("**write car table id to indirect access address register lower failure!\n");
    	    return 0;
    	}
	}
	else
	{
        /* write ip to Indirect access address registers*/
    	memset(&opt,0,sizeof(bm_op_args));
    	opt.op_len = 16;	
        opt.op_value = Indirect_add_reg_L;
        opt.op_addr = INDIRECT_ACCESS_ADDRESS_REG_L;
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    	    sem_syslog_warning("**write car table id to indirect access address register lower failure!\n");
    	    return 0;
    	}

        opt.op_value = Indirect_add_reg_H;  
    	opt.op_addr = INDIRECT_ACCESS_ADDRESS_REG_H;        /* write table id to address register*/
    	ret = write_fpga_reg(&opt);
        if(ret)
    	{
    	    sem_syslog_warning("##write car table id to indirect access address register high failure!\n");
    	    return 0;
    	}

        /* end */
	}

	/* car table into read operation */
	opt.op_addr = INDIRECT_ACCESS_CONTROL_REG;
	if(FLAG == 1)
	{
        opt.op_value = 0x34;
	}
	else
	{
	    opt.op_value = CAR_TABLE_VALUE+LINKUP;     /* read operation-- set bit 15 for 0,car table select_id=0x3 */
	}
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write fpga indirect access control register failure!\n");
		return 0;
	}
    /*end*/

    /* wait ready register into 1 */
	opt.op_addr = INDIRECT_ACCESS_STATE_REG;
    while(ready == 0)
	{
		ret = read_fpga_reg(&opt);
		if(ret)
		{
			sem_syslog_warning("**read fpga indirect access control register failure!\n");
			return 0;
		}
    	ready = opt.op_value ;    //Read ready Register
        ready = ready & READY_REG_VALUE;           //Check Status of ready
    }
	/*end*/

    memset(value,0,sizeof(value));
	for(k=start;k < StorRegCount; k++)
	{
		//memset(&opt,0,sizeof(bm_op_args));
		opt.op_addr = Data_Temp_Stor_Base_Addr + DataTempStorageArea[k];
		ret = read_fpga_reg(&opt);
		if(ret)
		{
			sem_syslog_warning("**read hash table failure!  k = %d**\n",k);
			return 0;
		}
		value[k] = opt.op_value;

	}

    k = start;
    /* car id */
    //stat.car_id = value[k] + (((value[k+1]) & 0x3) << 16);
    //k +=2;
	
    /* reserves ,not used reg 6 */
    //k +=6;
	
    /* reload  */
    stat.reload = value[k] + (((value[k+1]) & 0xff) << 16);
	/* car valid */
    stat.car_valid = ((value[k+1] & 0x8000) >> 15);
	/* linkup */
	stat.linkup = ((value[k+1] & 0x4000) >> 14);
    k +=2;
	
    /* usr ip */
    stat.usr_ip = value[k] + (value[k+1] << 16);
    k +=2;
	
	/* usr credit */
	stat.credit = value[k] + (value[k+1] << 16);
    k +=2;
	
    /* byte drop count [39:32]*/
	temp_value = value[k];
    k +=1;

    /* byte sent count */
	stat.byte_set_count = ((value[k] >> 8) << 32) + (value[k+1]) + ((value[k+2] << 16));
    k +=3;
    
    /* byte drop count [15:0]  [31:16]*/
    stat.byte_drop_count = value[k] + (value[k+1] << 16) + ((temp_value>>8)<<32);
    k +=2;

    /* package sent count */
    stat.package_set_count = value[k] + (value[k+1] << 16);
    k +=2;
	
    /* package drop count */
    stat.package_drop_count = value[k] + (value[k+1] << 16);
    k +=2;
#if 0
	sem_syslog_warning("\nDetail Information of CAR Table %llx\n",stat.usr_ip);
 	sem_syslog_warning("=====================================================================\n");
    sem_syslog_warning("\n");

	//sem_syslog_warning("\tcar id:                    %llx\n",stat.car_id);
	sem_syslog_warning("\tlinkup:                    %llx\n",stat.linkup);
    sem_syslog_warning("\treload:                    %llx\n",stat.reload);		
	sem_syslog_warning("\tcar valid:                 %llx\n",stat.car_valid);
	sem_syslog_warning("\tusr ip:                    %x\n",stat.usr_ip);
	sem_syslog_warning("\tcredit:                    %llx\n",stat.credit);
	sem_syslog_warning("\tbyte drop count:           %llx\n",stat.byte_drop_count);
	sem_syslog_warning("\tbyte sent count:           %llx\n",stat.byte_set_count);
    sem_syslog_warning("\tpackage sent count:        %llx\n",stat.package_set_count);
    sem_syslog_warning("\tpackage drop count:        %llx\n",stat.package_drop_count);

    sem_syslog_warning("\n");
    sem_syslog_warning("=====================================================================\n");
#endif
    read_car_result->linkup = stat.linkup;
    read_car_result->reload = stat.reload;
    read_car_result->car_valid = stat.car_valid;
    read_car_result->usr_ip = stat.usr_ip;
    read_car_result->credit = stat.credit;
    read_car_result->byte_drop_count = stat.byte_drop_count;
    read_car_result->byte_set_count = stat.byte_set_count;
    read_car_result->package_set_count = stat.package_set_count;
    read_car_result->package_drop_count = stat.package_drop_count;
    return 1;
}

/*
FLAG :1 for write car_id;0 for write IP
return 1 for success;0 for fail 
*/
int write_car(unsigned short Indirect_add_reg_H,
                unsigned short Indirect_add_reg_L,
                unsigned short RELOAD_H,
                unsigned short RELOAD_L,
                unsigned short LINKUP,
                unsigned short CAR_VALID,
                unsigned int CAR_ID,
                int FLAG)
{
	int ret;
	int read_car_ret;
	bm_op_args opt;
	int ready = 0;
    int i=0;
	unsigned short ip_H_ensure = 0;
	unsigned short ip_L_ensure = 0;
    unsigned short RELOAD_H_ensure =0;
	unsigned short RELOAD_L_ensure =0;
    unsigned short LINKUP_ensure =0;
    unsigned short CAR_VALID_ensure =0;

	read_car_result_t *read_car_result = NULL;
	read_car_result = (read_car_result_t *)malloc(sizeof(read_car_result_t));

    for(;i<3;i++)
    {	
		memset(read_car_result,0,sizeof(read_car_result_t));
    	memset(&opt,0,sizeof(bm_op_args));
    	opt.op_len = 16;
    	opt.op_addr = INDIRECT_ACCESS_STATE_REG;
    	while(ready == 0)
        {
    		ret = read_fpga_reg(&opt);
        	if(ret == -1)
        	{
        		sem_syslog_warning("**when read car,read fpga indirect access control register failure!\n");
        	}
            ready = opt.op_value & READY_REG_VALUE;           //Check Status of ready
    	}
    	
    	ready = 0;

    	memset(&opt,0,sizeof(bm_op_args));
    	opt.op_len = 16;

    	/* write reload to data cache register 8*/
    	opt.op_value = RELOAD_L;
    	opt.op_addr = SOURCE_IP_ADDRESS_REG_H;
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**write car reload value failure!\n");
    		continue;
    	}

        /* write car_valid linkup reload to data cache register 9*/
        opt.op_value = RELOAD_H+(LINKUP<<14)+(CAR_VALID<<15);
    	opt.op_addr = SOURCE_IP_ADDRESS_REG_L;        
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("##write linkup value failure!\n");
    		continue;
    	}
        /* end */

        /* write usr ip to temporary data cache register 10 */
        opt.op_value = Indirect_add_reg_L;
    	opt.op_addr = DESTINATION_IP_ADDRESS_REG_H;
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**write usr ip [15:0] value failure!\n");
    		continue;
    	}
    	opt.op_value = Indirect_add_reg_H;
    	opt.op_addr = DESTINATION_IP_ADDRESS_REG_L;
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**write usr ip [31:16] value failure!\n");
    		continue;
    	}
        /*end write usr ip */

        /* write credit to temporary data cache register 12 */
        opt.op_value = 0;//CREDIT_L
    	opt.op_addr = SOURCE_PORT_REG;
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**write credit [15:0] value failure!\n");
    		continue;
    	}
    	opt.op_value = 0;//CREDIT_H
    	opt.op_addr = DESTINATION_PORT_REG;
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**write credit [31:16] value failure!\n");
    		continue;
    	}
        /*end write credit */

        /*start write byte drop count [39:32] to temporary data cache register 14*/		
    	opt.op_value = 0;//BYTEDROPCOUNT_H
    	opt.op_addr = PROTOCOL_REG;
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**write byte drop count [39:32] to temporary data cache register failure!\n");
    		continue;
    	}
    	/*end*/

    	/*start write byte sent count [39:32] to temporary data cache register 15*/		
        opt.op_value = 0;//BYTESENTCOUNT_H
    	opt.op_addr = HASHCTL_REG;
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**write byte sent count [39:32] to temporary data cache register failure!\n");
    		continue;
    	}
        /*end*/

        /*start write byte sent count [15:0] to temporary data cache register 16*/		
        opt.op_value = 0;//BYTESENTCOUNT_L
    	opt.op_addr = DESTINATION_MAC_ADDRESS_REG_H;
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**write byte sent count [15:0] to temporary data cache register failure!\n");
    		continue;
    	}
        /*end*/
    	
        /*start write byte sent count [31:16] to temporary data cache register 17*/		
        opt.op_value = 0;//BYTESENTCOUNT_M
    	opt.op_addr = DESTINATION_MAC_ADDRESS_REG_M;
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**write byte sent count [31:16] to temporary data cache register failure!\n");
    		continue;
    	}
        /*end*/

        /*start write byte drop count [15:0] to temporary data cache register 18*/		
    	opt.op_value = 0;//BYTEDROPCOUNT_L
    	opt.op_addr = DESTINATION_MAC_ADDRESS_REG_L;
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**write byte drop count [15:0] to temporary data cache register failure!\n");
    		continue;
    	}
    	/*end*/

        /*start write byte drop count [31:16] to temporary data cache register 19*/	
    	opt.op_value = 0;//BYTEDROPCOUNT_M
    	opt.op_addr = SOURCE_MAC_ADDRESS_REG_H;
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**write byte drop count [31:16] to temporary data cache register failure!\n");
    		continue;
    	}
    	/*end*/

        /*start write package sent count [15:0] to temporary data cache register 20*/		
    	opt.op_value = 0;//PACKAGESENTCOUNT_L
    	opt.op_addr = SOURCE_MAC_ADDRESS_REG_M;
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**write  package sent count [15:0] to temporary data cache register failure!\n");
    		continue;
    	}
    	/*end*/

        /*start write package sent count [31:16] to temporary data cache register 21*/	
    	opt.op_value = 0;//PACKAGESENTCOUNT_H
    	opt.op_addr = SOURCE_MAC_ADDRESS_REG_L;
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**write  package sent count [31:16] to temporary data cache register failure!\n");
    		continue;
    	}
    	/*end*/

        /*start write package drop count [15:0] to temporary data cache register 22*/		
    	opt.op_value = 0;//PACKAGEDROPCOUNT_L
    	opt.op_addr = VLAN_OUT_REG_H;
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**write  package drop count [15:0] to temporary data cache register failure!\n");
    		continue;
    	}
    	/*end*/

        /*start write package drop count [31:16] to temporary data cache register 23*/	
    	opt.op_value = 0;//PACKAGEDROPCOUNT_H
    	opt.op_addr = VLAN_OUT_REG_L;
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**write  package drop count [31:16] to temporary data cache register failure!\n");
    		continue;
    	}
    	/*end*/
    	if(FLAG == 1)
    	{
        	opt.op_value = CAR_ID & 0Xffff;
        	opt.op_addr = INDIRECT_ACCESS_ADDRESS_REG_L;
        	ret = write_fpga_reg(&opt);
        	if(ret)
        	{
        		sem_syslog_warning("**write car CAR_ID value failure!\n");
        		continue;
        	}
    		opt.op_value = (CAR_ID & 0X30000)>>16;
        	opt.op_addr = INDIRECT_ACCESS_ADDRESS_REG_H;
        	ret = write_fpga_reg(&opt);
        	if(ret)
        	{
        		sem_syslog_warning("**write car CAR_ID value failure!\n");
        		continue;
        	}
    	}
    	else
    	{
        	opt.op_value = Indirect_add_reg_L;
        	opt.op_addr = INDIRECT_ACCESS_ADDRESS_REG_L;
        	ret = write_fpga_reg(&opt);
        	if(ret)
        	{
        		sem_syslog_warning("**write car CAR_ID value failure!\n");
        		continue;
        	}
    		opt.op_value = Indirect_add_reg_H;
        	opt.op_addr = INDIRECT_ACCESS_ADDRESS_REG_H;
        	ret = write_fpga_reg(&opt);
        	if(ret)
        	{
        		sem_syslog_warning("**write car CAR_ID value failure!\n");
        		continue;
        	}        
    	}

    	/* car table into write operation */
    	opt.op_addr = INDIRECT_ACCESS_CONTROL_REG;
    	if(FLAG == 1)
    	{
            opt.op_value = 0x8034;
    	}
        else
        {
    	    opt.op_value = 0x8030+LINKUP;     /* read operation-- set bit 15 for 1,car table select_id=0x3 */
        }
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**write fpga indirect access control register failure!\n");
    		continue;
    	}
		
    	read_car_ret = read_car(read_car_result,Indirect_add_reg_H,Indirect_add_reg_L,LINKUP,0,0);
    	if(!read_car_ret)
        {
            sem_syslog_warning("**write -> read car failure!\n");
    		continue;
    	}
        else
        {
    		ip_L_ensure = read_car_result->usr_ip & 0xffff;
    		ip_H_ensure = (read_car_result->usr_ip & 0xffff0000)>>16;
    		RELOAD_H_ensure = (read_car_result->reload & 0xff0000)>>16;
    		RELOAD_L_ensure = read_car_result->reload & 0xffff;
    		LINKUP_ensure = read_car_result->linkup;
    		CAR_VALID_ensure = read_car_result->car_valid;
/*        	sem_syslog_warning("write******Indirect_add_reg_H = 0x%x******\n",Indirect_add_reg_H);
        	sem_syslog_warning("write******Indirect_add_reg_L = 0x%x******\n",Indirect_add_reg_L);			
        	sem_syslog_warning("write******RELOAD_H = 0x%x******\n",RELOAD_H);
        	sem_syslog_warning("write******RELOAD_L = 0x%x******\n",RELOAD_L);
        	sem_syslog_warning("write******LINKUP = 0x%x******\n",LINKUP);
        	sem_syslog_warning("write******CAR_VALID = 0x%x******\n",CAR_VALID);
			sem_syslog_warning("huangjing---ensure write == read\n");
			sem_syslog_warning("read******ip_L_ensure = 0x%x******\n",ip_L_ensure);
            sem_syslog_warning("read******ip_H_ensure = 0x%x******\n",ip_H_ensure);
            sem_syslog_warning("read******RELOAD_H_ensure = 0x%x******\n",RELOAD_H_ensure);
            sem_syslog_warning("read******RELOAD_L_ensure = 0x%x******\n",RELOAD_L_ensure);
            sem_syslog_warning("read******LINKUP_ensure = 0x%x******\n",LINKUP_ensure);
            sem_syslog_warning("read******CAR_VALID_ensure = 0x%x******\n",CAR_VALID_ensure);
*/
    		if(ip_H_ensure != Indirect_add_reg_H || ip_L_ensure != Indirect_add_reg_L || 
    			RELOAD_H_ensure != RELOAD_H || RELOAD_L_ensure != RELOAD_L ||
    			LINKUP != LINKUP_ensure || CAR_VALID_ensure != CAR_VALID)
    		{
                sem_syslog_warning("**the content of reading is not the content you writed.\n");
    			continue;
    		}
/*
        	sem_syslog_warning("write******Indirect_add_reg_H = 0x%x******\n",Indirect_add_reg_H);
        	sem_syslog_warning("write******Indirect_add_reg_L = 0x%x******\n",Indirect_add_reg_L);			
        	sem_syslog_warning("write******RELOAD_H = 0x%x******\n",RELOAD_H);
        	sem_syslog_warning("write******RELOAD_L = 0x%x******\n",RELOAD_L);
        	sem_syslog_warning("write******LINKUP = 0x%x******\n",LINKUP);
        	sem_syslog_warning("write******CAR_VALID = 0x%x******\n",CAR_VALID);
			sem_syslog_warning("huangjing---ensure write == read\n");
*/
			free(read_car_result);
	        read_car_result = NULL;
    		return 1;
    	}
    }
	free(read_car_result);
	read_car_result = NULL;
	sem_syslog_warning("write fail!\n");
	return 0;
    /*end*/
}

void empty_car_table(void)
{
	unsigned int i = 0;
	int ret;
	
    for(i=0;i<CAR_MAX;i++)
    {
        ret = write_car(0,0,0,0,0,0,i,1);
		if(!ret)
		{
            sem_syslog_warning("clear car table %d fail!\n",i);
		}
	}
}

int clear_hash_statistics(void)
{
	int ret;
	bm_op_args opt;
	memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;
	opt.op_addr = FPGA_LOCK_CLEAR_REG;
/*clear Hash_aging*/
	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**read FPGA_LOCK_CLEAR_REG  failure!\n");
		return 0;
	}
	opt.op_value |= 0x0400;//Hash_aging_clr = 1
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write Hash_aging_latch  failure!\n");
		return 0;
	}
	
	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("##read FPGA_LOCK_CLEAR_REG  failure!\n");
		return 0; 
	}
	opt.op_value &= 0xfbff;//Hash_aging_latch = 0
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("##write Hash_aging_latch  failure!\n");
		return 0;
	}
/*end*/

/*clear Hash_update*/
	memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;
	opt.op_addr = FPGA_LOCK_CLEAR_REG;

	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**read FPGA_LOCK_CLEAR_REG  failure!\n");
		return 0;
	}
	opt.op_value |= 0x0200;//Hash_update_clr = 1
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write Hash_aging_latch  failure!\n");
		return 0;
	}
	
	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("##read FPGA_LOCK_CLEAR_REG  failure!\n");
		return 0; 
	}
	opt.op_value &= 0xfdff;//Hash_update_clr = 0
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("##write Hash_aging_latch  failure!\n");
		return 0;
	}
/*end*/

/*clear Hash_learn*/
	memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;
	opt.op_addr = FPGA_LOCK_CLEAR_REG;

	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**read FPGA_LOCK_CLEAR_REG  failure!\n");
		return 0;
	}
	opt.op_value |= 0x0100;//Hash_learn_clr = 1
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write Hash_aging_latch  failure!\n");
		return 0;
	}
	
	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("##read FPGA_LOCK_CLEAR_REG  failure!\n");
		return 0; 
	}
	opt.op_value &= 0xfeff;//Hash_learn_clr = 0
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("##write Hash_aging_latch  failure!\n");
		return 0;
	}
/*end*/
	return 1;
}


int clear_port_statistics(unsigned short port_number)
{
	int ret;
	bm_op_args opt;
	memset(&opt,0,sizeof(bm_op_args));
	
	opt.op_len = 16;
	opt.op_addr = FPGA_LOCK_CLEAR_REG;
    
	switch (port_number) 
	{
		case 112:
			/*clr operation*/
			ret = read_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("**read_fpga_reg  failure!%d\n",port_number);
                return 0;
			}
			opt.op_value &= 0xfe;//112_clr [0]=0
			ret = write_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("**write_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			ret = read_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("##read_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			opt.op_value |= 0x1;//112_clr [0]=1
			ret = write_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("##write_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			ret = read_fpga_reg(&opt);
			if(ret)
			{
				sem_syslog_warning("&&read_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			opt.op_value &= 0xfe;//112_clr [0]=0
			ret = write_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("&&write_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			/*end*/

			/*latch operation*/
			ret = read_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("**read_fpga_reg  failure!%d\n",port_number);
                return 0;
			}
			opt.op_value &= 0xef;//112_latch [0]=0
			ret = write_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("**write_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			ret = read_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("##read_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			opt.op_value |= 0x10;//112_latch [0]=1
			ret = write_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("##write_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			ret = read_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("&&read_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			opt.op_value &= 0xef;//112_latch [0]=0
			ret = write_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("&&write_fpga_reg  failure!%d\n",port_number);
				return 0;
			}				
			/*end*/
			break;
		
		case 113:
			/*clr operation*/
			ret = read_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("**read_fpga_reg  failure!%d\n",port_number);
                return 0;
			}
			opt.op_value &= 0xfd;//113_clr [0]=0
			ret = write_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("**write_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			ret = read_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("##read_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			opt.op_value |= 0x2;//113_clr [0]=1
			ret = write_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("##write_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			ret = read_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("&&read_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			opt.op_value &= 0xfd;//113_clr [0]=0
			ret = write_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("&&write_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			/*end*/

			/*latch operation*/
			ret = read_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("**read_fpga_reg  failure!%d\n",port_number);
                return 0;
			}
			opt.op_value &= 0xdf;//113_latch [0]=0
			ret = write_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("**write_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			ret = read_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("##read_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			opt.op_value |= 0x20;//113_latch [0]=1
			ret = write_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("##write_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			ret = read_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("&&read_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			opt.op_value &= 0xdf;//113_latch [0]=0
			ret = write_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("&&write_fpga_reg  failure!%d\n",port_number);
				return 0;
			}				
			/*end*/
			break;						   
		case 115:
			/*clr operation*/
			ret = read_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("**read_fpga_reg  failure!%d\n",port_number);
                return 0;
			}
			opt.op_value &= 0xfb;//115_clr [0]=0
			ret = write_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("**write_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			ret = read_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("##read_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			opt.op_value |= 0x4;//115_clr [0]=1
			ret = write_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("##write_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			ret = read_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("&&read_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			opt.op_value &= 0xfb;//115_clr [0]=0
			ret = write_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("&&write_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			/*end*/

			/*latch operation*/
			ret = read_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("**read_fpga_reg  failure!%d\n",port_number);
                return 0;
			}
			opt.op_value &= 0xbf;//115_latch [0]=0
			ret = write_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("**write_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			ret = read_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("##read_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			opt.op_value |= 0x40;//115_latch [0]=1
			ret = write_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("##write_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			ret = read_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("&&read_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			opt.op_value &= 0xbf;//115_latch [0]=0
			ret = write_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("&&write_fpga_reg  failure!%d\n",port_number);
				return 0;
			}				
			/*end*/
			break;
		
		case 116:
			/*clr operation*/
			ret = read_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("**read_fpga_reg  failure!%d\n",port_number);
                return 0;
			}
			opt.op_value &= 0xf7;//116_clr [3]=0
			ret = write_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("**write_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			ret = read_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("##read_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			opt.op_value |= 0x8;//116_clr [3]=1
			ret = write_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("##write_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			ret = read_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("&&read_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			opt.op_value &= 0xf7;//116_clr [3]=0
			ret = write_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("&&write_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			/*end*/

			/*latch operation*/
			ret = read_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("**read_fpga_reg  failure!%d\n",port_number);
                return 0;
			}
			opt.op_value &= 0x7f;//116_latch [7]=0
			ret = write_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("**write_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			ret = read_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("##read_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			opt.op_value |= 0x80;//116_latch [7]=1
			ret = write_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("##write_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			ret = read_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("&&read_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			opt.op_value &= 0x7f;//116_latch [7]=0
			ret = write_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("&&write_fpga_reg  failure!%d\n",port_number);
				return 0;
			}				
			/*end*/
			break;

			#if 0
			ret = read_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("**read_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			opt.op_value &= 0x7f;//116_latch [7]=0
			ret = write_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("**write_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			ret = read_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("##read_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			opt.op_value |= 0x8;//116_clr [3]=0
			ret = write_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("##write_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			ret = read_fpga_reg(&opt);
			if(ret == -1)
			{
				sem_syslog_warning("&&read_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			opt.op_value &= 0x7f;
			ret = write_fpga_reg(&opt);//116_latch [7]=0
			if(ret == -1)
			{
				sem_syslog_warning("&&write_fpga_reg  failure!%d\n",port_number);
				return 0;
			}
			break;
		#endif
		default: 
			sem_syslog_warning("Port number error\r\n");
			return 0;
    }
	return 1;
}

int read_hash_statistics(hash_statistics_t *hash_statistics)
{
    int ret;
	int k;
	int start;
	bm_op_args opt;
	unsigned long long value[sizeof(MibXauiRegOffsetAll)/sizeof(long)]; /* register value */
	
	memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;
	opt.op_addr = FPGA_LOCK_CLEAR_REG;

	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**read FPGA_LOCK_CLEAR_REG  failure!\n");
		return 0;
	}
	opt.op_value |= 0x4000;//Hash_aging_latch = 1
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write Hash_aging_latch  failure!\n");
		return 0;
	}
	
	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("##read FPGA_LOCK_CLEAR_REG  failure!\n");
		return 0; 
	}
	opt.op_value 
&= 0xbfff;//Hash_aging_latch = 0
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("##write Hash_aging_latch  failure!\n");
		return 0;
	}
	start = 86;
	memset(value,0,sizeof(value));
	for(k=start;k < MibRegCount; k++)
	{
		opt.op_addr = Mib_Xaui_Reg_Base_Addr + MibXauiRegOffsetAll[k];
		ret = read_fpga_reg(&opt);
		if(ret)
		{
			sem_syslog_warning("**read_fpga_reg  failure!  k = %d**\n",k);
			return 0;
		}

		value[k] = opt.op_value;
		/*osPrintf("%#0x:Port<%2d>%-26s\t%#0x\r\n",(unsigned int)regAddr,portNum,mibRegDesc[k],value[k]);*/		
	}
	k=start;
	hash_statistics->hash_aging_num = (value[k]<<16) + (value[k+1]);

	memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;
	opt.op_addr = FPGA_LOCK_CLEAR_REG;

	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**read FPGA_LOCK_CLEAR_REG  failure!\n");
		return 0;
	}
	opt.op_value |= 0x2000;//Hash_update_latch = 1
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write Hash_aging_latch  failure!\n");
		return 0;
	}
	
	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("##read FPGA_LOCK_CLEAR_REG  failure!\n");
		return 0; 
	}
	opt.op_value 
&= 0xdfff;//Hash_update_latch = 0
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("##write Hash_aging_latch  failure!\n");
		return 0;
	}
	start = 88;
	memset(value,0,sizeof(value));
	for(k=start;k < MibRegCount; k++)
	{
		opt.op_addr = Mib_Xaui_Reg_Base_Addr + MibXauiRegOffsetAll[k];
		ret = read_fpga_reg(&opt);
		if(ret)
		{
			sem_syslog_warning("**read_fpga_reg  failure!  k = %d**\n",k);
			return 0;
		}

		value[k] = opt.op_value;
		/*osPrintf("%#0x:Port<%2d>%-26s\t%#0x\r\n",(unsigned int)regAddr,portNum,mibRegDesc[k],value[k]);*/		
	}
	k=start;
	hash_statistics->hash_update_num = (value[k]<<16) + (value[k+1]);

	memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;
	opt.op_addr = FPGA_LOCK_CLEAR_REG;

	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**read FPGA_LOCK_CLEAR_REG  failure!\n");
		return 0;
	}
	opt.op_value |= 0x1000;//Hash_learn_latch = 1
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write Hash_aging_latch  failure!\n");
		return 0;
	}
	
	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("##read FPGA_LOCK_CLEAR_REG  failure!\n");
		return 0; 
	}
	opt.op_value 
&= 0xefff;//Hash_learn_latch = 0
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("##write Hash_aging_latch  failure!\n");
		return 0;
	}
	start = 90;
	memset(value,0,sizeof(value));
	for(k=start;k < MibRegCount; k++)
	{
		opt.op_addr = Mib_Xaui_Reg_Base_Addr + MibXauiRegOffsetAll[k];
		ret = read_fpga_reg(&opt);
		if(ret)
		{
			sem_syslog_warning("**read_fpga_reg  failure!  k = %d**\n",k);
			return 0;
		}

		value[k] = opt.op_value;
		/*osPrintf("%#0x:Port<%2d>%-26s\t%#0x\r\n",(unsigned int)regAddr,portNum,mibRegDesc[k],value[k]);*/		
	}
	k=start;
	hash_statistics->hash_learn_num = (value[k]<<16) + (value[k+1]);
	return 1;
}

int read_port_statistics(xaui_port_counter_t *port_statistics,unsigned short port_number)
{
	int ret;
	int k,start;
	bm_op_args opt;
	unsigned long long value[sizeof(MibXauiRegOffsetAll)/sizeof(long)]; /* register value */

	memset(&opt,0,sizeof(bm_op_args));
	
	opt.op_len = 16;
	opt.op_addr = FPGA_LOCK_CLEAR_REG;
		
	switch (port_number) 
	{
		case 112:
			ret = read_fpga_reg(&opt);
			if(ret)
			{
				sem_syslog_warning("**read_fpga_reg  failure!	%d\n",port_number);
				return 0;
			}
			opt.op_value |= 0x0010;
			ret = write_fpga_reg(&opt);
			if(ret)
			{
				sem_syslog_warning("**write_fpga_reg  failure!	%d\n",port_number);
				return 0;
			}
			ret = read_fpga_reg(&opt);
			if(ret)
			{
				sem_syslog_warning("##read_fpga_reg  failure!	%d\n",port_number);
				return 0;
			}
			opt.op_value &= 0xffef;
			ret = write_fpga_reg(&opt);
			if(ret)
			{
				sem_syslog_warning("##write_fpga_reg  failure!	%d\n",port_number);
				return 0;
			}
			start = 0;
			break;
		
		case 113:
			ret = read_fpga_reg(&opt);
			if(ret)
			{
				sem_syslog_warning("**read_fpga_reg  failure!	%d\n",port_number);
				return 0;
			}
			opt.op_value |= 0x0020;
			ret = write_fpga_reg(&opt);
            if(ret)
			{
				sem_syslog_warning("**write_fpga_reg  failure!	%d\n",port_number);
                return 0;
			}
            ret = read_fpga_reg(&opt);
            if(ret)
            {
                sem_syslog_warning("##read_fpga_reg  failure!	%d\n",port_number);
                return 0;
            }
			opt.op_value &= 0xffdf;
			ret = write_fpga_reg(&opt);
            if(ret)
            {
                 sem_syslog_warning("##write_fpga_reg  failure!	%d\n",port_number);
                 return 0;
			}

            start = 25;
            break;
					   
		case 115:
            ret = read_fpga_reg(&opt);
            if(ret)
            {
				sem_syslog_warning("**read_fpga_reg  failure!	%d\n",port_number);
                return 0;
            }
            opt.op_value |= 0x0040;
			ret = write_fpga_reg(&opt);
            if(ret)
            {
                sem_syslog_warning("**write_fpga_reg  failure!	%d\n",port_number);
                return 0;
		    }
			ret = read_fpga_reg(&opt);
			if(ret)
			{
				sem_syslog_warning("##read_fpga_reg  failure!	%d\n",port_number);
				return 0;
			}
			opt.op_value &= 0xffbf;
			ret = write_fpga_reg(&opt);
			if(ret)
			{
				sem_syslog_warning("**write_fpga_reg  failure!	%d\n",port_number);
				return 0;
			}
			start = 50;
			break;
		
		case 116:
			ret = read_fpga_reg(&opt);
			if(ret)
			{
				sem_syslog_warning("**read_fpga_reg  failure!	%d\n",port_number);
				return 0;
			}
			opt.op_value |= 0x80;
			ret = write_fpga_reg(&opt);
			if(ret)
			{
				sem_syslog_warning("**write_fpga_reg  failure!	%d\n",port_number);
				return 0;
			}
			ret = read_fpga_reg(&opt);
			if(ret)
			{
				sem_syslog_warning("##read_fpga_reg  failure!	%d\n",port_number);
				return 0; 
			}
			opt.op_value &= 0xff7f;
			ret = write_fpga_reg(&opt);
			if(ret)
			{
				sem_syslog_warning("##write_fpga_reg  failure!	%d\n",port_number);
				return 0;
			}
			start = 75;
			break;
		
		default: 
			sem_syslog_warning("Port number error\r\n");
			return 0;
	}

    memset(value,0,sizeof(value));
	for(k=start;k < MibRegCount; k++)
	{
		opt.op_addr = Mib_Xaui_Reg_Base_Addr + MibXauiRegOffsetAll[k];
		ret = read_fpga_reg(&opt);
		if(ret)
		{
			sem_syslog_warning("**read_fpga_reg  failure!  k = %d**\n",k);
			return 0;
		}

		value[k] = opt.op_value;
		/*osPrintf("%#0x:Port<%2d>%-26s\t%#0x\r\n",(unsigned int)regAddr,portNum,mibRegDesc[k],value[k]);*/		
	}
	
	if(port_number == 112
    ||port_number == 113
    ||port_number == 115)
    {
        k = start;
        /*start RX*/
        /*good packet num */
        port_statistics->rx_good_package_num  = (value[k]<<16) + (value[k+1]);
        k +=2;
        /*good byte num*/
        port_statistics->rx_good_byte_num = (value[k]<<32) + (value[k+1]<<16) + (value[k+2]);
        k +=3;
        /*bad package num*/
        port_statistics->rx_bad_package_num = value[k];
        k +=1;
        /*fcserr package num*/
        port_statistics->rx_fcserr_package_num = value[k];
        k +=1;
        /*multicast package num*/
        port_statistics->rx_multicast_package_num = value[k];
        k +=1;
        /*broadcast package num*/
        port_statistics->rx_broadcast_package_num = value[k];
        k +=1;
        /*pause package num*/
        port_statistics->rx_pause_package_num = value[k];
        k +=1;
    	/*drop num*/
    	port_statistics->rx_drop_num = value[k];
        //vty_out(vty,"&&stat.rx.drop_num = %llx\n",stat.rx.drop_num);
        k +=1;
        /*ttl drop num*/
        port_statistics->rx_ttl_drop_num = value[k];
        k +=1;
        /*car drop num*/
        port_statistics->rx_car_drop_num = value[k];
        k +=1;
        /*hash col num*/
        port_statistics->rx_hash_col_num = (value[k]<<16) + (value[k+1]);
        k +=2;
        /*to cpu num*/
        port_statistics->rx_to_cpu_num = (value[k]<<16) + (value[k+1]);
        k +=2;
        /*end RX*/
        
        /*start TX*/
        /*package num*/
        port_statistics->tx_package_num = (value[k]<<16) + (value[k+1]);
		
        k +=2;
        /*byte num*/
        port_statistics->tx_bytenum = (value[k]<<32) + (value[k+1]<<16) + (value[k+2]);
        k +=3;
        /*multicast package num*/
        port_statistics->tx_multicast_package_num = value[k];
        k +=1;
        /*broadcast package num*/
        port_statistics->tx_broadcast_package_num = value[k];
        k +=1;
        /*pause package num*/
        port_statistics->tx_pause_package_num = value[k];
        k +=1;      
    }
    else if(port_number == 116)
    {
        k = start;
        /*start RX*/
        /*good packet num */
        port_statistics->rx_good_package_num = (value[k]<<16) + (value[k+1]);
        k +=2;
        /*good byte num*/
        port_statistics->rx_good_byte_num = (value[k]<<32) + (value[k+1]<<16) + (value[k+2]);
        k +=3;
        /*bad package num*/
        port_statistics->rx_bad_package_num = value[k];
        k +=1;
        /*fcserr package num*/
        port_statistics->rx_fcserr_package_num = value[k];
        k +=1;
        /*multicast package num*/
        port_statistics->rx_multicast_package_num = value[k];
        k +=1;
        /*broadcast package num*/
        port_statistics->rx_broadcast_package_num = value[k];
        k +=1;
        /*pause package num*/
        port_statistics->rx_pause_package_num = value[k];
        k +=1;
    	/*drop num*/
    	port_statistics->rx_drop_num = value[k];
        k +=1;
        /*end RX*/

        /*hash aging num*/
        //port_statistics->hash_aging_num = (value[k]<<16) + (value[k+1]);
        //k +=2;
        /*hash update num*/
        //port_statistics->hash_update_num = (value[k]<<16) + (value[k+1]);
        //k +=2;
        /*hash learn num*/
        //port_statistics->hash_learn_num = (value[k]<<16) + (value[k+1]);
        //k +=2;
        k +=6;/*skip hash statics*/
        
        /*start TX*/
        /*package num*/
        port_statistics->tx_package_num = (value[k]<<16) + (value[k+1]);
        k +=2;
        /*byte num*/
        port_statistics->tx_bytenum = (value[k]<<32) + (value[k+1]<<16) + (value[k+2]);
        k +=3;
        /*multicast package num*/
        port_statistics->tx_multicast_package_num = value[k];
        k +=1;
        /*broadcast package num*/
        port_statistics->tx_broadcast_package_num = value[k];
        k +=1;
        /*pause package num*/
        port_statistics->tx_pause_package_num = value[k];
        k +=1;
    }

	return 1;
}

int parse_slot_id_from_ifname(const char *if_name, int *slot_id)
{
	char *ptr;
	int i=0;
	*slot_id = 0;
	
	if (!if_name) {
		sem_syslog_warning("NULL if_name\n");
		return 1;
	}

	ptr = (char *)if_name;
	for (i=0; i<strlen(if_name); i++) {
		if (*(ptr+i) >= '1' && *(ptr+i) <= '9') {
			while (*(ptr+i) >= '0' && *(ptr+i) <= '9') {
				*slot_id = *slot_id * 10 + *(ptr+i) - '0';
				i++;
			}
			break;
		}
	}

	if (i == strlen(if_name)) {
		return 1;
	} else {
		return 0;
	}
	
}
/*
reload : [23:0]
linkstate : 1 for uplink ,0 for down link
return :
0 for this node don't exit;
-1 for parameter wrong;
1 for write car table wrong;
2 for success
*/
int fpga_car_modify_user(unsigned int user_ip,unsigned int reload,unsigned short linkstate,unsigned short property)
{
    int ret = 0;
	unsigned short Indirect_add_reg_H = 0;
	unsigned short Indirect_add_reg_L = 0;
    unsigned short RELOAD_H =0;
	unsigned short RELOAD_L =0;
    unsigned short LINKUP =0;
	unsigned short CAR_VALID =1;

	car_linklist_node_t *car_user = NULL;
	Indirect_add_reg_H = (user_ip & 0xffff0000)>>16;
	Indirect_add_reg_L = user_ip & 0xffff;
	RELOAD_H = (reload & 0xff0000)>>16;
    RELOAD_L = reload & 0xffff;
    LINKUP = linkstate;
	
    car_user = ensure_car_node_exist(user_ip,linkstate);
	if(car_user == NULL)
	{
		//sem_syslog_warning("function:fpga_car_modify_user->ensure_car_node_exist;linkstate = %d;user_ip = %x ,car list no this node!\n",linkstate,user_ip);
		return 0;
	}
	else
	{
        if(car_user->configuration_attribute == property)
        {
            car_user->reload = reload;
			ret = write_car(Indirect_add_reg_H,Indirect_add_reg_L,RELOAD_H,RELOAD_L,LINKUP,CAR_VALID,0,0);
            if(ret == 0)
            {
				sem_syslog_warning("function:fpga_car_modify_user->write_car;linkstate = %d;user_ip = %x ,write car failure!\n",linkstate,user_ip);
            	return 1;
        	}
        	//sem_syslog_warning("**write car success!\n");
			return 2;
		}
		else
		{
			//sem_syslog_warning("car_user->configuration_attribute != property\n");
            return -1;
		}
	}
}
/*
return :
0 for this node don't exit;
1 for delete success;
4 for delete fail;
2,3 for parameter property is not equal.
*/
int fpga_car_delete_user(unsigned int user_ip,unsigned short linkstate,unsigned short property)
{
	int ret = 0;
	unsigned short Indirect_add_reg_H = 0;
	unsigned short Indirect_add_reg_L = 0;
    unsigned short LINKUP =0;
	unsigned short CAR_VALID =0;

	car_linklist_node_t *car_user = NULL;
	Indirect_add_reg_H = (user_ip & 0xffff0000)>>16;
	Indirect_add_reg_L = user_ip & 0xffff;
    LINKUP = linkstate;

    car_user = ensure_car_node_exist(user_ip,linkstate);
    if(car_user == NULL)
    {
		//sem_syslog_warning("function:fpga_car_delete_user->ensure_car_node_exist;linkstate = %d;user_ip = %x ,car list no this node!\n",linkstate,user_ip);
        return 0;
	}
	else
	{
		//sem_syslog_warning("car list have this node!\n");
        if(car_user->configuration_attribute == 0)
        {
			//sem_syslog_warning("car_user->configuration_attribute == 0\n");
            if(property == 0)
            {
                //sem_syslog_warning("car_user->configuration_attribute == 0,property == 0\n");
				ret = delete_car_node(user_ip,linkstate);
				if(ret ==0)
				{
					sem_syslog_warning("function:fpga_car_delete_user->delete_car_node;linkstate = %d;user_ip = %x static,delete_car_node fail!\n",linkstate,user_ip);
					return 4;
				}
				else
				{
                    //sem_syslog_warning("delete_car_node success\n");
					ret = write_car(Indirect_add_reg_H,Indirect_add_reg_L,0,0,LINKUP,CAR_VALID,0,0);
                    if(ret == 0)
                    {
						sem_syslog_warning("function:fpga_car_delete_user->write_car;linkstate = %d;user_ip = %x static,write car failure!\n",linkstate,user_ip);
                    	return 4;
                	}
                	//sem_syslog_warning("**write car success!\n");
					return 1;
				}
			}
			else
			{
                //sem_syslog_warning("**static node ,can't delete!\n");
				return 2;
			}
		}
		if(car_user->configuration_attribute == 1)
		{
			//sem_syslog_warning("car_user->configuration_attribute == 1\n");
            if(property == 1)
            {
                //sem_syslog_warning("car_user->configuration_attribute == 1,property == 1\n");
				ret = delete_car_node(user_ip,linkstate);
				if(ret == 0)
				{
					sem_syslog_warning("function:fpga_car_delete_user->delete_car_node;linkstate = %d;user_ip = %x dynamic,delete_car_node fail!\n",linkstate,user_ip);
					return 4;
				}
				else
				{
                    //sem_syslog_warning("delete_car_node success..\n");
					ret = write_car(Indirect_add_reg_H,Indirect_add_reg_L,0,0,LINKUP,CAR_VALID,0,0);
                    if(ret == 0)
                    {
						sem_syslog_warning("function:fpga_car_delete_user->write_car;linkstate = %d;user_ip = %x dynamic,write car failure!\n",linkstate,user_ip);
                    	return 4;
                	}
                	//sem_syslog_warning("**write car success!..\n");
					return 1;                    
				}
			}
			else
			{
            	//sem_syslog_warning("car list node is d parameter is s\n");
				return 3;
			}
		}
        sem_syslog_warning("bad property parameter,never come here!\n");
        return -1;
	}
}

/*
*return :
0 for write car table fail;
1 for user already exit;
2 for user is static;
3 for user exit but reload is different, modify reload value;
4 for chang user property to static;
5 for chang user property to static and modify reload val
6 for user is in the white list;
7 for user is not exit,write success,insert success!;
8 for user is not exit,insert fail.
*/
int fpga_car_insert_user(unsigned int user_ip,unsigned int reload,unsigned short linkstate,unsigned short property)
{
	int ret = 0;
	unsigned short Indirect_add_reg_H = 0;
	unsigned short Indirect_add_reg_L = 0;
    unsigned short RELOAD_H =0;
	unsigned short RELOAD_L =0;
    unsigned short LINKUP =0;
	unsigned short CAR_VALID =1;

	car_linklist_node_t *car_user = NULL;
	Indirect_add_reg_H = user_ip >> 16;
	Indirect_add_reg_L = user_ip & 0xffff;
	RELOAD_H = (reload & 0xff0000)>>16;
    RELOAD_L = reload & 0xffff;
    LINKUP = linkstate;
/*
	sem_syslog_warning("Indirect_add_reg_H = 0x%x\n",Indirect_add_reg_H);
	sem_syslog_warning("Indirect_add_reg_L = 0x%x\n",Indirect_add_reg_L);
	sem_syslog_warning("RELOAD_H = 0x%x\n",RELOAD_H);
	sem_syslog_warning("RELOAD_L = 0x%x\n",RELOAD_L);
*/
    ret = ensure_car_white_node_exist(user_ip,linkstate);
	if(ret == 0)
	{
        car_user = ensure_car_node_exist(user_ip,linkstate);
    	if(car_user == NULL)
    	{
    		//sem_syslog_warning("car list no this node!\n");
        	ret = insert_car_linklist(user_ip,reload,linkstate,property);
    		if(ret == 0)
    		{
				sem_syslog_warning("function:fpga_car_insert_user->exit->insert_car_linklist;linkstate = %d;user_ip = %x is not exit,insert_car_linklist failure!\n",linkstate,user_ip);
				return 8;
    		}
			else
			{
                ret = write_car(Indirect_add_reg_H,Indirect_add_reg_L,RELOAD_H,RELOAD_L,LINKUP,CAR_VALID,0,0);
                if(ret == 0)
                {
    				sem_syslog_warning("function:fpga_car_insert_user->exit->write_car;linkstate = %d;user_ip = %x is not exit,write car failure!\n",linkstate,user_ip);
                	return 0;
        		}
    			else
    			{
        		    //sem_syslog_warning("**write car success!\n");
    				return 7;
    			}
			}
    	}
	    else
    	{
        	if(car_user->configuration_attribute == property)
        	{
        		//sem_syslog_warning("car list have this IP and linkstate and property!\n");
                if(car_user->reload == reload)
                {
                    //sem_syslog_warning("car list have this linkstate =%d and ip =%x and property and reload!\n",linkstate,user_ip);
        			return 1;
        		}
        		else
        		{
        			car_user->reload = reload;
    				ret = write_car(Indirect_add_reg_H,Indirect_add_reg_L,RELOAD_H,RELOAD_L,LINKUP,CAR_VALID,0,0);
                    if(ret == 0)
                    {
						sem_syslog_warning("function:fpga_car_insert_user->write_car;linkstate = %d;user_ip = %x is exit but reload is not equal,write car failure!\n",linkstate,user_ip);
                    	return 0;
            		}
        			return 3;
        		}
        	}
        	else
        	{
                if(property == 1 && car_user->configuration_attribute == 0)
                {
                    //sem_syslog_warning("property == 1 && car_user->configuration_attribute == 0\n");
        			return 2;
        		}
        		if(property == 0 && car_user->configuration_attribute == 1)
        		{
        			//sem_syslog_warning("property == 0 && car_user->configuration_attribute == 1\n");
                    car_user->configuration_attribute = 0;
                	if(car_user->reload == reload)
                    {
                        //sem_syslog_warning("property is diff reload is same\n");
                		return 4;
                	}
                	else
                	{
                		car_user->reload = reload;
    					ret = write_car(Indirect_add_reg_H,Indirect_add_reg_L,RELOAD_H,RELOAD_L,LINKUP,CAR_VALID,0,0);
                        if(ret == 0)
                        {
							sem_syslog_warning("function:fpga_car_insert_user->write_car;linkstate = %d;user_ip = %x,property is diff change reload,write car failure!\n",linkstate,user_ip);
                        	return 0;
                		}
                		return 5;
                	}
        		}
				else
				{
					//never come here!!!
                    return 9;
				}
				
        	}
    	}
	}
	else
	{
        //sem_syslog_warning("****ip = %x linkup = %d is in the white list\n",user_ip,linkstate);
		return 6;
	}
}
/*
return NULL for read car fail;
*/
int fpga_car_show_user_statistic(read_car_result_t * read_car_result,unsigned int user_ip,unsigned short linkstate)
{
	int ret;
	int read_car_ret;
	unsigned short Indirect_add_reg_H = 0;
	unsigned short Indirect_add_reg_L = 0;

    Indirect_add_reg_H = user_ip >> 16;
	Indirect_add_reg_L = user_ip &0xffff;

    read_car_ret = read_car(read_car_result,Indirect_add_reg_H,Indirect_add_reg_L,linkstate,0,0);
    if(!read_car_ret)
    { 
        sem_syslog_warning("**read car table failure!\n");
		return 0;
	}
    return 1;
}

DBusMessage *sem_dbus_get_wan_if_num(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
 	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	
    sem_syslog_dbg("**sem_dbus_get_wan_if_num \n");
	
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT64,
								 &wan_if_count);
	return reply;
}

DBusMessage *sem_dbus_show_wan_if(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
 	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	unsigned long long count;
	int ret = 0;
    int FLAG = 0;
	wan_if_linklist_node_t *wan_if=NULL;
	char *name = NULL;

    sem_syslog_dbg("**sem_dbus_show_wan_if \n");
    dbus_error_init(&err);

	name = (char *)malloc(WAN_IF_NAME_LEN);
	if(name == NULL)
	{
		sem_syslog_warning("*function:sem_dbus_show_wan_if malloc name failed!\n");
	    FLAG = 1;
	}

	if (!(dbus_message_get_args(msg, &err,
						        DBUS_TYPE_UINT64, &count,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		sem_syslog_dbg("sem_dbus_check_board_type get arg failed and reset normal\n");
	}
	
	wan_if = show_wan_if_node(count);
	if(!wan_if)
	{
        sem_syslog_warning("**show_wan_if_node failed \n");
		FLAG = 1;
	}else{
        memcpy(name,wan_if->name,WAN_IF_NAME_LEN);
        sem_syslog_event("**show_wan_if_node success %s  %s\n",wan_if->name,name);       
	}

	
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
                                 DBUS_TYPE_INT32,&FLAG);
	dbus_message_iter_append_basic (&iter,
							 DBUS_TYPE_STRING,
							 &name);
	//free(wan_if);
	free(name);
	return reply;
}

DBusMessage *sem_dbus_if_wan_state_set(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter	 iter;
	DBusError err;

	int wan_state;
	char *if_name;
	unsigned long long ret = 0x0ull;
	int retval = 0;
	int board_on_mask = 0x0;
	int temp_mask = 0;
	int i = 0;
	int timeout = 50;
	int dest_slot_id;
	unsigned int FLAG=0;
	FILE *fd_dbm;
    int is_active_master = 0;
	sem_pdu_head_t * head;
	sem_tlv_t *tlv_head;
	char send_buf[SEM_TIPC_SEND_BUF_LEN];
	struct vlan_ioctl_args *if_request;
	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
								DBUS_TYPE_UINT32, &wan_state,
								DBUS_TYPE_STRING, &if_name,
								DBUS_TYPE_INVALID)) ||
		(strlen(if_name) + 1) > sizeof(if_request->device1))
	{
		sem_syslog_dbg("set vlan flags failed\n");
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		FLAG = 2;
		
		reply = dbus_message_new_method_return(msg);
	
		dbus_message_iter_init_append (reply, &iter);
	
		dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
		return reply;
	}

	//sem_syslog_warning("ifname %s wan_state %d\n", if_name, wan_state);
	fd_dbm = fopen("/dbm/local_board/is_active_master", "r");
	if (!fd_dbm){
		fprintf(stderr,"Get production information [2] error\n");
		FLAG = 1;
	}else{
    	if(fscanf(fd_dbm, "%d", &is_active_master) != 1){
    		FLAG = 1;
		}
    	fclose(fd_dbm);
	}
	
	ret = parse_slot_id_from_ifname(if_name, &dest_slot_id);
	
	if (ret) {
		sem_syslog_dbg("parse_slot_id_from_ifname failed.\n");
		FLAG = 1;
	} else {
		int fd;
		struct vlan_ioctl_args if_request;

		if ((strlen(if_name) + 1) > sizeof(if_request.device1)) {
			sem_syslog_dbg("Interface name to long.\n");
			FLAG = 1;
		} else {
			if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
				FLAG = 1;
			} else {
				memset(&if_request, 0, sizeof(if_request));
				strncpy(if_request.device1, if_name, sizeof(if_request.device1));
				if_request.cmd = SET_VLAN_FLAG_CFI_CMD;
				if_request.vlan_qos = wan_state;
				if_request.u.flag = 4;

				if (ioctl(fd, SIOCSIFVLAN, &if_request) < 0) {
					sem_syslog_dbg("CFI ioctl fail!\n");
					close(fd);
					FLAG = 1;
				} else {
				    if(is_active_master)
				    {
						sem_syslog_event("*****************if_name = %s\n",if_name);
    				    if(wan_state)
    				    {
    				        ret = insert_wan_if_linklist(if_name);
    						if(ret == 1)
    						{
                                sem_syslog_dbg("**insert_wan_if_linklist failure!\n");
    							FLAG = 1;
    						}
    				    }
    					else
    				    {
                            ret = delete_wan_if_node(if_name);
    						if(!ret)
    						{
                                sem_syslog_dbg("**delete_wan_if_node failure!\n");
    							FLAG = 1;
    						}
    					}
				    }
					close(fd);
				}
			}
		}
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
								DBUS_TYPE_UINT32,
								&FLAG);

	return reply;
	
}
DBusMessage *sem_dbus_local_check_board_type(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
 	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	int ret;
    //unsigned short slot_id =0;

	dbus_error_init(&err);

    if(local_board->board_type == BOARD_TYPE_AX81_1X12G12S || local_board->board_type == BOARD_TYPE_AX71_1X12G12S)
    {
        ret = 1;
	}
	else
	{
        ret = 0;
	}
	sem_syslog_warning("function:sem_dbus_local_check_board_type.ret = %d,slot id = %d\n",ret,local_board->slot_id+1);
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_INT32,
								 &ret);
	sem_syslog_warning("function:sem_dbus_local_check_board_type.will return !\n");
	return reply;

}

DBusMessage *sem_dbus_check_board_type(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
 	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	int ret;
    unsigned short slot_id =0;

	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
						        DBUS_TYPE_UINT16, &slot_id,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		sem_syslog_dbg("sem_dbus_check_board_type get arg failed and reset normal\n");
	}

    if(local_board->board_type == BOARD_TYPE_AX81_1X12G12S || local_board->board_type == BOARD_TYPE_AX71_1X12G12S)
    {
        ret = 1;
	}
	else
	{
        ret = 0;
	}
	
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_INT32,
								 &ret);
	return reply;

}

DBusMessage *sem_dbus_show_hash_statistics(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
 	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int slot_id;
	int ret,clear_ret;
	int read_hash_ret=0;
	int ready = 0;
    unsigned short flag_c =0;

	hash_statistics_t *hash_statistics = NULL;
	unsigned long long hash_aging_num = 0;
    unsigned long long hash_update_num = 0;
    unsigned long long hash_learn_num = 0;

	hash_statistics = (hash_statistics_t *)malloc(sizeof(hash_statistics_t));
	memset(hash_statistics,0,sizeof(hash_statistics_t));
	
	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
						        DBUS_TYPE_UINT16, &flag_c,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		sem_syslog_dbg("sem_dbus_show_port_counter get arg failed and reset normal\n");
	}
	
	memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;
	opt.op_addr = INDIRECT_ACCESS_STATE_REG;
	while(ready == 0)
    {
		ret = read_fpga_reg(&opt);
    	if(ret == -1)
    	{
    		sem_syslog_warning("**read fpga indirect access control register failure!\n");
    	}
        ready = opt.op_value & READY_REG_VALUE;           //Check Status of ready
	}
	ready = 0;
	
	if(0 == flag_c)
	{
	    read_hash_ret = read_hash_statistics(hash_statistics);
		if(!read_hash_ret)
		{
            sem_syslog_warning("**read hash statistics failure!\n");
			ret = 0;
		}
		else
		{
		    ret = 1;
		}
	}
	if(1 == flag_c)
	{
        clear_ret = clear_hash_statistics();
		if(clear_ret == 0)
		{
            sem_syslog_warning("**clear hash statistics failure!\n");
		}
	}

	hash_aging_num = hash_statistics->hash_aging_num;
	hash_update_num = hash_statistics->hash_update_num;
	hash_learn_num = hash_statistics->hash_learn_num;
    free(hash_statistics);
	hash_statistics = NULL;
	if(0 == flag_c)
	{	    		
    	reply = dbus_message_new_method_return(msg);
    	dbus_message_iter_init_append (reply, &iter);
    	dbus_message_iter_append_basic (&iter,
    								 DBUS_TYPE_INT32,
    								 &ret);
		dbus_message_iter_append_basic (&iter,
    								 DBUS_TYPE_UINT64,
    								 &hash_aging_num);
		dbus_message_iter_append_basic (&iter,
    								 DBUS_TYPE_UINT64,
    								 &hash_update_num);
		dbus_message_iter_append_basic (&iter,
    								 DBUS_TYPE_UINT64,
    								 &hash_learn_num);
		return reply;
    }
	else if(1== flag_c)
	{
    	reply = dbus_message_new_method_return(msg);
    	dbus_message_iter_init_append (reply, &iter);
    	dbus_message_iter_append_basic (&iter,
    								 DBUS_TYPE_INT32,
    								 &clear_ret);
    	return reply;
	}
}



DBusMessage *sem_dbus_show_port_counter(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
 	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int slot_id;
	int ret,clear_ret;
    int ready = 0;
	unsigned short port_number;
	unsigned short flag_c =0;
    xaui_port_counter_t *port_statistics = NULL;

	unsigned long long package_num;
	unsigned long long bytenum;
	unsigned long long tx_multicast_package_num;
	unsigned long long tx_broadcast_package_num;
	unsigned long long tx_pause_package_num;
	unsigned long long good_package_num;
	unsigned long long good_byte_num;
	unsigned long long bad_package_num;
	unsigned long long fcserr_package_num;
	unsigned long long rx_multicast_package_num;
	unsigned long long rx_broadcast_package_num;
	unsigned long long rx_pause_package_num;
	unsigned long long drop_num;
	unsigned long long ttl_drop_num;
	unsigned long long car_drop_num;
	unsigned long long hash_col_num;
	unsigned long long to_cpu_num;
    //unsigned long long hash_aging_num;
    //unsigned long long hash_update_num;
    //unsigned long long hash_learn_num;

    port_statistics = (xaui_port_counter_t *)malloc(sizeof(xaui_port_counter_t));
	memset(port_statistics,0,sizeof(xaui_port_counter_t));
	
	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
						        DBUS_TYPE_UINT16, &port_number,
						        DBUS_TYPE_UINT16, &flag_c,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		sem_syslog_dbg("sem_dbus_show_port_counter get arg failed and reset normal\n");
	}
    //sem_syslog_warning("port_number = 0x%x\n", port_number);
    //sem_syslog_warning("flag_c = 0x%x\n", flag_c);
    /*test whether the  previous operation is completed */
	memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;
	opt.op_addr = INDIRECT_ACCESS_STATE_REG;
	while(ready == 0)
    {
		ret = read_fpga_reg(&opt);
    	if(ret == -1)
    	{
    		sem_syslog_warning("**read fpga indirect access control register failure!\n");
    	}
        ready = opt.op_value & READY_REG_VALUE;           //Check Status of ready
	}
	ready = 0;
	/*end*/
	if(0 == flag_c)
	{
	    ret = read_port_statistics(port_statistics,port_number);
	}
	if(1 == flag_c)
	{
        clear_ret = clear_port_statistics(port_number);
	}
	
/*      
        sem_syslog_warning("\tgood_package_num:           %lld\n", port_statistics->rx_good_package_num);		
        sem_syslog_warning("\tgood_byte_num:              %lld\n", port_statistics->rx_good_byte_num);		
        sem_syslog_warning("\tbad_package_num:            %lld\n", port_statistics->rx_bad_package_num);
        sem_syslog_warning("\tfcserr_package_num:         %lld\n", port_statistics->rx_fcserr_package_num);
        sem_syslog_warning("\tmulticast_package_num:      %lld\n", port_statistics->rx_multicast_package_num);
        sem_syslog_warning("\tbroadcast_package_num:      %lld\n", port_statistics->rx_broadcast_package_num);
        sem_syslog_warning("\tpause_package_num:          %lld\n", port_statistics->rx_pause_package_num);
        sem_syslog_warning("\tdrop_num:                   %lld\n", port_statistics->rx_drop_num);
        sem_syslog_warning("\tttl_drop_num:               %lld\n", port_statistics->rx_ttl_drop_num);
        sem_syslog_warning("\tcar_drop_num:               %lld\n", port_statistics->rx_car_drop_num);
        sem_syslog_warning("\thash_col_num:               %lld\n", port_statistics->rx_hash_col_num);
        sem_syslog_warning("\tto_cpu_num:                 %lld\n", port_statistics->rx_to_cpu_num);	
        sem_syslog_warning("\tpackage_num:                %lld\n", port_statistics->tx_package_num);
        sem_syslog_warning("\tbytenum:                    %lld\n", port_statistics->tx_bytenum);
        sem_syslog_warning("\tmulticast_package_num:      %lld\n", port_statistics->tx_multicast_package_num);
        sem_syslog_warning("\tbroadcast_package_num:      %lld\n", port_statistics->tx_broadcast_package_num);
        sem_syslog_warning("\tpause_package_num:          %lld\n", port_statistics->tx_pause_package_num);
        sem_syslog_warning("\thash_aging_num:             %lld\n", port_statistics->hash_aging_num);		
        sem_syslog_warning("\thash_update_num:            %lld\n", port_statistics->hash_update_num);		
        sem_syslog_warning("\thash_learn_num:             %lld\n", port_statistics->hash_learn_num);
*/

	package_num = port_statistics->tx_package_num;
	bytenum = port_statistics->tx_bytenum;
	tx_multicast_package_num = port_statistics->tx_multicast_package_num;
	tx_broadcast_package_num = port_statistics->tx_broadcast_package_num;
	tx_pause_package_num = port_statistics->tx_pause_package_num;
	good_package_num = port_statistics->rx_good_package_num;
	good_byte_num = port_statistics->rx_good_byte_num;
	bad_package_num = port_statistics->rx_bad_package_num;
	fcserr_package_num = port_statistics->rx_fcserr_package_num;
	rx_multicast_package_num = port_statistics->rx_multicast_package_num;
    rx_broadcast_package_num = port_statistics->rx_broadcast_package_num;
	rx_pause_package_num = port_statistics->rx_pause_package_num;
	drop_num = port_statistics->rx_drop_num;
	ttl_drop_num = port_statistics->rx_ttl_drop_num;
	car_drop_num = port_statistics->rx_car_drop_num;
	hash_col_num = port_statistics->rx_hash_col_num;
	to_cpu_num = port_statistics->rx_to_cpu_num;
	free(port_statistics);
	port_statistics = NULL;
	if(0 == flag_c)
	{	
    	if(port_number == 112 || port_number == 113 || port_number == 115)
        {
        	reply = dbus_message_new_method_return(msg);
        	dbus_message_iter_init_append (reply, &iter);
        	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT32,
        								 &ret);
         	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &package_num);
        	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &bytenum);
        	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &tx_multicast_package_num);
        	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &tx_broadcast_package_num);
        	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &tx_pause_package_num);
        	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &good_package_num);
        	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &good_byte_num);
        	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &bad_package_num);
        	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &fcserr_package_num);
    		dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &rx_multicast_package_num);
        	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &rx_broadcast_package_num);
        	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &rx_pause_package_num);
        	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &drop_num);
        	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &ttl_drop_num);
    		dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &car_drop_num);
    		dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &hash_col_num);
    		dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &to_cpu_num);
        	return reply;
    	}
        else if(port_number == 116)
        {    	
            //hash_aging_num = port_statistics->hash_aging_num;
        	//hash_update_num = port_statistics->hash_update_num;
        	//hash_learn_num = port_statistics->hash_learn_num;
    		
        	reply = dbus_message_new_method_return(msg);
        	dbus_message_iter_init_append (reply, &iter);
        	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT32,
        								 &ret);
         	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &package_num);
        	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &bytenum);
        	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &tx_multicast_package_num);
        	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &tx_broadcast_package_num);
        	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &tx_pause_package_num);
        	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &good_package_num);
        	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &good_byte_num);
        	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &bad_package_num);
        	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &fcserr_package_num);
    		dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &rx_multicast_package_num);
        	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &rx_broadcast_package_num);
        	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &rx_pause_package_num);
        	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &drop_num);
        	dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &ttl_drop_num);
    		dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &car_drop_num);
    		dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &hash_col_num);
    		dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &to_cpu_num);
/*    		dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &hash_aging_num);
    		dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &hash_update_num);
    		dbus_message_iter_append_basic (&iter,
        								 DBUS_TYPE_UINT64,
        								 &hash_learn_num);
*/
			return reply;
    	}
	}
	else if(1== flag_c)
	{
    	reply = dbus_message_new_method_return(msg);
    	dbus_message_iter_init_append (reply, &iter);
    	dbus_message_iter_append_basic (&iter,
    								 DBUS_TYPE_UINT32,
    								 &clear_ret);
    	return reply;
	}
}
DBusMessage *sem_dbus_read_car(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
 	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int slot_id;
	int ret =0;
	int read_car_ret=0;
	int flag =0;
    int ready = 0;
	unsigned short Indirect_add_reg_H = 0;
	unsigned short Indirect_add_reg_L = 0;
    unsigned short LINKUP_STATE =0;
	unsigned short property =0;
	unsigned long long reload = 0;
	unsigned long long linkup = 0;
	unsigned long long car_valid = 0;
	unsigned int usr_ip = 0;
    unsigned long long credit = 0;
	unsigned long long byte_drop_count = 0;  
	unsigned long long byte_set_count = 0;
	unsigned long long package_set_count = 0;
	unsigned long long package_drop_count = 0;
	read_car_result_t *read_car_result = NULL;
	car_linklist_node_t *car_user = NULL;
	read_car_result = (read_car_result_t *)malloc(sizeof(read_car_result_t));
	if(read_car_result == NULL)
	{
		sem_syslog_warning("*function:sem_dbus_read_car malloc failed!\n");
        ret = 1;
		flag = 1;
		goto finish;
	}
	memset(read_car_result,0,sizeof(read_car_result_t));
	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
						        DBUS_TYPE_UINT16, &Indirect_add_reg_H,
						        DBUS_TYPE_UINT16, &Indirect_add_reg_L,
						        DBUS_TYPE_UINT16, &LINKUP_STATE,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		sem_syslog_dbg("sem_dbus_read_car get arg failed and reset normal\n");
	}
/*
    sem_syslog_warning("Indirect_add_reg_H = 0x%x\n", Indirect_add_reg_H);
    sem_syslog_warning("Indirect_add_reg_L = 0x%x\n", Indirect_add_reg_L);
    sem_syslog_warning("LINKUP = 0x%x\n", LINKUP_STATE);
*/
	usr_ip = Indirect_add_reg_H;
	usr_ip = usr_ip << 16;
	usr_ip = usr_ip + Indirect_add_reg_L;

    car_user = ensure_car_node_exist(usr_ip,LINKUP_STATE);
	if(car_user == NULL)
    {
		sem_syslog_warning("car list no this node!\n");
		flag = 1;
		free(read_car_result);
	    read_car_result = NULL;
		goto finish;
	}
	else
	{
    	read_car_ret = read_car(read_car_result,Indirect_add_reg_H,Indirect_add_reg_L,LINKUP_STATE,0,0);
        if(!read_car_ret)
        {
    		ret = -1;
            sem_syslog_warning("**read_car failure!\n");
        	free(read_car_result);
        	goto finish;
    	}
        else
        {
    		ret = 0;
    	}
	}
/*
	sem_syslog_warning("**read_car_result->reload = %d\n",read_car_result->reload);
	sem_syslog_warning("**read_car_result->usr_ip = %d\n",read_car_result->usr_ip);
	sem_syslog_warning("**read_car_result->byte_drop_count = %d\n",read_car_result->byte_drop_count);
	sem_syslog_warning("**read_car_result->byte_set_count = %d\n",read_car_result->byte_set_count);
	sem_syslog_warning("**read_car_result->package_set_count %d\n",read_car_result->package_set_count);
	sem_syslog_warning("**read_car_result->package_drop_count = %d\n",read_car_result->package_drop_count);
*/
	reload = read_car_result->reload;
	linkup = read_car_result->linkup;
	car_valid = read_car_result->car_valid;
	usr_ip = read_car_result->usr_ip;
    credit = read_car_result->credit;
	byte_drop_count = read_car_result->byte_drop_count;  
	byte_set_count = read_car_result->byte_set_count;
	package_set_count = read_car_result->package_set_count;
	package_drop_count = read_car_result->package_drop_count;
	if(flag == 0)
	{
	    property = car_user->configuration_attribute;
	}
		
	free(read_car_result);
	read_car_result = NULL;
	
finish:
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_INT32,
								 &flag);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_INT32,
								 &ret);
 	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT64,
								 &reload);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT64,
								 &linkup);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT64,
								 &car_valid);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &usr_ip);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT64,
								 &credit);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT64,
								 &byte_drop_count);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT64,
								 &byte_set_count);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT64,
								 &package_set_count);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT64,
								 &package_drop_count);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &property);

	//sem_syslog_dbg("**sem_dbus_read_car success!\n");
	return reply;
}

DBusMessage *sem_dbus_get_car_list_count(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
 	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int slot_id;
	int ret;
	int i;
    int ready = 0;
	unsigned int car_node_ip;
	
	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
						        DBUS_TYPE_UINT32, &car_node_ip,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		sem_syslog_dbg("sem_dbus_read_cam_core get arg failed and reset normal\n");
	}
	
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT64,
								 &car_count);
	//sem_syslog_warning("**get the car count  = %lld success \n",car_count);
	return reply;
}
DBusMessage *sem_dbus_get_car_white_list_count(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
 	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int slot_id;
	int ret;
	int i;
    int ready = 0;
	unsigned int car_node_ip;
    sem_syslog_dbg("**sem_dbus_get_car_white_list_count \n");
	
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT64,
								 &car_white_count);
	//sem_syslog_warning("**get the car count  = %lld success \n",car_white_count);
	return reply;
}

DBusMessage *sem_dbus_restore_car_table(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
 	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	unsigned long long i;
	int ret = 0;
	int result =0;
	int read_ret =0;
    unsigned short Indirect_add_reg_H;
    unsigned short Indirect_add_reg_L;
    unsigned short RELOAD_H;
    unsigned short RELOAD_L;
    unsigned short LINKUP;
    unsigned short CAR_VALID=1;
    unsigned int CAR_ID =0;
    int FLAG;
	read_car_node_param_t * car_node_param = NULL;

    sem_syslog_dbg("**sem_dbus_restore_car_table \n");

	car_node_param = (read_car_node_param_t *)malloc(sizeof(read_car_node_param_t));
	if(car_node_param == NULL){
		sem_syslog_warning("function:sem_dbus_restore_car_table.malloc read_car_node_param_t fail!\n");
        ret =1;
	}else{
	    for(i=1;i<=car_count;i++)
	    {
            read_ret = read_car_linklist(i,car_node_param);
			if(!read_ret){
                sem_syslog_warning("function:sem_dbus_restore_car_table.read_car_linklist failed,list is empty!\n");
                ret = 2;
			}else{
                Indirect_add_reg_H = car_node_param->usr_ip >> 16;
                Indirect_add_reg_L = car_node_param->usr_ip & 0xffff;
                RELOAD_H = (car_node_param->reload & 0xff0000)>>16;
                RELOAD_L = car_node_param->reload & 0xffff;
                LINKUP = car_node_param->linkup;
				//sem_syslog_warning("Indirect_add_reg_H=0x%x\n",Indirect_add_reg_H);
				//sem_syslog_warning("Indirect_add_reg_L=0x%x\n",Indirect_add_reg_L);
				//sem_syslog_warning("RELOAD_H=0x%x\n",RELOAD_H);
				//sem_syslog_warning("RELOAD_L=0x%x\n",RELOAD_L);
				result = write_car(Indirect_add_reg_H,Indirect_add_reg_L,RELOAD_H,RELOAD_L,LINKUP,CAR_VALID,CAR_ID,0);
				if(!result){
		            sem_syslog_warning("function:sem_dbus_restore_car_table.write_car fail!\n");
					ret = 3;
				}
			}
		}
	}
	
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_INT32,
								 &ret);
	//sem_syslog_warning("**get the car count  = %lld ret=%d \n",car_count,ret);
	free(car_node_param);
	return reply;
}

DBusMessage *sem_dbus_empty_car_table(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
 	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	unsigned long long i;
	int ret = 0;
    unsigned int CAR_ID =0;
    int FLAG = 0;
	read_car_node_param_t * car_node_param = NULL;

    sem_syslog_dbg("**sem_dbus_empty_car_table \n");

    delete_car_linklist();
	
	empty_car_table();
	
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_INT32,
								 &FLAG);
	//sem_syslog_warning("**get the car count  = %lld ret=%d \n",car_count,ret);
	return reply;
}

#if 0
DBusMessage *sem_dbus_show_car_list_debug(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
 	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int slot_id;
	int ret;
	int i;
    int ready = 0;
	
	read_car_node_param_t *car_node_param = NULL;
	car_node_param = (read_car_node_param_t *)malloc(sizeof(read_car_node_param_t));
	
    sem_syslog_dbg("**sem_dbus_show_car_list_debug\n");
	
	dbus_error_init(&err);

    ret = display_car_linklist();
	sem_syslog_dbg("**there is %lld node in the list\n",car_count);

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_INT32,
								 &ret);
	return reply;
}
#endif

DBusMessage *sem_dbus_show_car_list(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
 	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int slot_id;
	int ret=0;
	int read_ret =0;
	int i;
    int ready = 0;
	unsigned int car_node_ip;
	unsigned int car_node_reload;
	unsigned short car_node_link;
	unsigned long long car_node_now;
	unsigned short car_node_property;
	read_car_node_param_t *car_node_param = NULL;
	
    sem_syslog_event("**sem_dbus_show_car_list\n");
	
	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
						        DBUS_TYPE_UINT64, &car_node_now,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		sem_syslog_dbg("sem_dbus_read_cam_core get arg failed and reset normal\n");
	}
	//display_car_linklist();
	car_node_param = (read_car_node_param_t *)malloc(sizeof(read_car_node_param_t));
	if(car_node_param == NULL)
	{
		sem_syslog_warning("function:sem_dbus_show_car_list.malloc read_car_node_param_t fail!\n");
        ret =1;
	}
	else
	{
		//sem_syslog_warning("car node now is %lld\n",car_node_now);
        read_ret = read_car_linklist(car_node_now,car_node_param);
        if(!read_ret)
        {
    		sem_syslog_warning("function:sem_dbus_show_car_list->read_car_linklist.the car link list is empty!\n");
            ret = 1;
    	}
		else
		{
        	car_node_ip = car_node_param->usr_ip;
        	car_node_reload = car_node_param->reload;
        	car_node_link = car_node_param->linkup;
        	car_node_property = car_node_param->configuration_attribute;
		}
        free(car_node_param);
        car_node_param = NULL;
	}
	
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_INT32,
								 &ret);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &car_node_ip);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &car_node_reload);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &car_node_link);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &car_node_property);
	return reply;
}

DBusMessage *sem_dbus_show_car_white_list(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
 	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int slot_id;
	int ret=0;
	int read_white_car =0;
	int i;
    int ready = 0;
	unsigned int car_node_ip;
	unsigned short car_node_link;
	unsigned long long car_node_now;
	read_car_white_node_param_t *car_node_param = NULL;
	
    //sem_syslog_warning("**sem_dbus_show_car_list\n");
	//display_car_white_linklist();
	
	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
						        DBUS_TYPE_UINT64, &car_node_now,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		sem_syslog_dbg("sem_dbus_read_cam_core get arg failed and reset normal\n");
	}
	
	car_node_param = (read_car_white_node_param_t *)malloc(sizeof(read_car_white_node_param_t));
	if(car_node_param == NULL)
	{
		sem_syslog_warning("function:sem_dbus_show_car_list.malloc read_car_node_param_t fail!\n");
        ret =1;
	}
	else
	{
    	//sem_syslog_warning("car_node_now = %lld\n",car_node_now);
        read_white_car = read_car_white_linklist(car_node_now,car_node_param);
        if(!read_white_car)
        {
    		sem_syslog_warning("function:sem_dbus_show_car_list->read_car_linklist.read %lld car linklist fail!\n",car_node_now);
            ret = 1;
    	}
		else
		{
        	car_node_ip = car_node_param->usr_ip;
        	car_node_link = car_node_param->linkup;
		}
	}
	
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_INT32,
								 &ret);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &car_node_ip);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &car_node_link);

	//sem_syslog_dbg("**get the %lld node success!\n",car_node_now);
	free(car_node_param);
	return reply;
}

#if 1
DBusMessage *sem_dbus_write_car_subnet(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
 	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int slot_id;
    int ready = 0;
	int ret = 0;
	int flag= 1;
	unsigned short Indirect_add_reg_H = 0;
	unsigned short Indirect_add_reg_L = 0;
	unsigned short subnet_mask_H = 0;
	unsigned short subnet_mask_L = 0;
    unsigned short RELOAD_H =0;
	unsigned short RELOAD_L =0;
    unsigned short LINKUP =0;
    unsigned short CAR_VALID =0;
	unsigned short CAR_PROPERTY =0;
	unsigned int num =0;
	unsigned int i;
	unsigned int car_node_ip = 0;
	unsigned int car_node_reload = 0;
	
	//sem_syslog_warning("sem write subnet car\n");
	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
                                DBUS_TYPE_UINT16, &Indirect_add_reg_H,
        	                    DBUS_TYPE_UINT16, &Indirect_add_reg_L,
        						DBUS_TYPE_UINT16, &RELOAD_H,
        						DBUS_TYPE_UINT16, &RELOAD_L,
        						DBUS_TYPE_UINT16, &LINKUP,
        						DBUS_TYPE_UINT16, &CAR_VALID,
        						DBUS_TYPE_UINT16, &CAR_PROPERTY,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		sem_syslog_dbg("sem_dbus_write_car get arg failed and reset normal\n");
	}
/*
    sem_syslog_warning("******RELOAD_H = 0x%x******\n",RELOAD_H);
	sem_syslog_warning("******RELOAD_L = 0x%x******\n",RELOAD_L);
	sem_syslog_warning("******LINKUP = 0x%x******\n",LINKUP);
	sem_syslog_warning("******CAR_VALID = 0x%x******\n",CAR_VALID);
*/
    car_node_ip = Indirect_add_reg_H;
	car_node_ip = car_node_ip << 16;
	car_node_ip = car_node_ip + Indirect_add_reg_L;
	car_node_reload = RELOAD_H;
	car_node_reload = car_node_reload <<16;
	car_node_reload = car_node_reload + RELOAD_L;
    
	if(CAR_VALID == 1)
	{
        flag = fpga_car_insert_user(car_node_ip,car_node_reload,LINKUP,CAR_PROPERTY);
	}
	else if(CAR_VALID == 0)
	{
        flag = fpga_car_delete_user(car_node_ip,LINKUP,CAR_PROPERTY);
	}
	else if(CAR_VALID == 2)
	{
		CAR_VALID = 1;
        flag = fpga_car_modify_user(car_node_ip,car_node_reload,LINKUP,CAR_PROPERTY);
	}
	else
	{
        sem_syslog_warning("bad CAR_VALID = 0x%x******\n",CAR_VALID);
		flag = 1;
	}

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &flag);
	return reply;

}
#endif

DBusMessage *sem_dbus_write_car(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
 	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int slot_id;
    int ready = 0;
	int ret = 0;
	int flag= 1;
	int i;
	unsigned short Indirect_add_reg_H = 0;
	unsigned short Indirect_add_reg_L = 0;
    unsigned short RELOAD_H =0;
	unsigned short RELOAD_L =0;
    unsigned short LINKUP =0;
    unsigned short CAR_VALID =0;
	unsigned short ip_H_ensure = 0;
	unsigned short ip_L_ensure = 0;
    unsigned short RELOAD_H_ensure =0;
	unsigned short RELOAD_L_ensure =0;
    unsigned short LINKUP_ensure =0;
    unsigned short CAR_VALID_ensure =0;
	unsigned short CAR_PROPERTY =0;
	unsigned short num =0;
	unsigned int car_node_ip = 0;
	unsigned int car_node_reload = 0;
	//read_car_result_t *read_car_result = NULL;


	//sem_syslog_dbg("sem write car\n");

	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
                                DBUS_TYPE_UINT16, &Indirect_add_reg_H,
        	                    DBUS_TYPE_UINT16, &Indirect_add_reg_L,
        						DBUS_TYPE_UINT16, &RELOAD_H,
        						DBUS_TYPE_UINT16, &RELOAD_L,
        						DBUS_TYPE_UINT16, &LINKUP,
        						DBUS_TYPE_UINT16, &CAR_VALID,
        						DBUS_TYPE_UINT16, &CAR_PROPERTY,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		sem_syslog_dbg("sem_dbus_write_car get arg failed and reset normal\n");
	}
/*
    sem_syslog_warning("******RELOAD_H = 0x%x******\n",RELOAD_H);
	sem_syslog_warning("******RELOAD_L = 0x%x******\n",RELOAD_L);
	sem_syslog_warning("******LINKUP = 0x%x******\n",LINKUP);
	sem_syslog_warning("******CAR_VALID = 0x%x******\n",CAR_VALID);
*/
    car_node_ip = Indirect_add_reg_H;
	car_node_ip = car_node_ip << 16;
	car_node_ip = car_node_ip + Indirect_add_reg_L;
	car_node_reload = RELOAD_H;
	car_node_reload = car_node_reload <<16;
	car_node_reload = car_node_reload + RELOAD_L;
    //sem_syslog_warning("******car_node_ip = 0x%x******\n",car_node_ip);
	//sem_syslog_warning("******car_node_reload = 0x%u******\n",car_node_reload);
    
	if(CAR_VALID == 1)
	{
        flag = fpga_car_insert_user(car_node_ip,car_node_reload,LINKUP,CAR_PROPERTY);
	}
	else if(CAR_VALID == 0)
	{
        flag = fpga_car_delete_user(car_node_ip,LINKUP,CAR_PROPERTY);
	}
	else if(CAR_VALID == 2)
	{
		CAR_VALID = 1;
        flag = fpga_car_modify_user(car_node_ip,car_node_reload,LINKUP,CAR_PROPERTY);
	}
	else
	{
        sem_syslog_warning("bad CAR_VALID = 0x%x******\n",CAR_VALID);
		flag = 1;
	}
    //display_car_linklist();
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &flag);
	return reply;
   
}	

DBusMessage *sem_dbus_write_white_car(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
 	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int slot_id;
    int ready = 0;
	int ret = 0;
	int flag= -1;
	int i;
	unsigned short Indirect_add_reg_H = 0;
	unsigned short Indirect_add_reg_L = 0;
    unsigned short LINKUP =0;
    unsigned short CAR_FLAG =0;
	unsigned short num =0;
	unsigned int car_node_ip = 0;

	//sem_syslog_dbg("sem write white car list!\n");

	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
		                        //DBUS_TYPE_UINT16, &num,
                                DBUS_TYPE_UINT16, &Indirect_add_reg_H,
        	                    DBUS_TYPE_UINT16, &Indirect_add_reg_L,
        						DBUS_TYPE_UINT16, &LINKUP,
        						DBUS_TYPE_UINT16, &CAR_FLAG,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		sem_syslog_dbg("sem_dbus_write_car get arg failed and reset normal\n");
	}

	//sem_syslog_warning("******LINKUP = 0x%x******\n",LINKUP);
	//sem_syslog_warning("******CAR_VALID = 0x%x******\n",CAR_FLAG);
    //sem_syslog_warning("******num = 0x%x******\n",num);
    car_node_ip = Indirect_add_reg_H;
	car_node_ip = car_node_ip << 16;
	car_node_ip = car_node_ip + Indirect_add_reg_L;

	if(CAR_FLAG == 1)
	{
        flag = fpga_car_whitelist_insert(car_node_ip,LINKUP);
	}
	else if(CAR_FLAG == 0)
	{
        flag = fpga_car_whitelist_delete(car_node_ip,LINKUP);
	}
	else
	{
        sem_syslog_warning("bad CAR_VALID = 0x%x******\n",CAR_FLAG);
	}
    //display_car_white_linklist();
	
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_INT32,
								 &flag);
	return reply;
   
}	


DBusMessage *sem_dbus_write_cam_core(DBusConnection *conn, DBusMessage *msg, void *user_data)
{	
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int slot_id;
    int ready = 0;
	unsigned short Indirect_add_reg_H = 0;
	unsigned short Indirect_add_reg_L = 0;
	unsigned short SIP_H=0;
	unsigned short SIP_L=0;
    unsigned short DIP_H=0;
	unsigned short DIP_L=0;
	unsigned short SPORT=0;
	unsigned short DPORT=0;
	unsigned short PROTOCOL=0;
	unsigned short NUM=0;
	int ret;
	int flag =1;

	sem_syslog_dbg("sem write cam core\n");

	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
                                DBUS_TYPE_UINT16, &Indirect_add_reg_H,
	                            DBUS_TYPE_UINT16, &Indirect_add_reg_L,
						        DBUS_TYPE_UINT16, &SIP_H,
						        DBUS_TYPE_UINT16, &SIP_L,
						        DBUS_TYPE_UINT16, &DIP_H,
						        DBUS_TYPE_UINT16, &DIP_L,
						        DBUS_TYPE_UINT16, &SPORT,
						        DBUS_TYPE_UINT16, &DPORT,
						        DBUS_TYPE_UINT16, &PROTOCOL,
						        DBUS_TYPE_UINT16, &NUM,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		sem_syslog_dbg("sem_dbus_write_cam_core get arg failed and reset normal\n");
	}

    /*test whether the  previous operation is completed */
	int i;
	for(i=0;i <=NUM-1;i++)
	{
    	memset(&opt,0,sizeof(bm_op_args));
    	opt.op_len = 16;
    	opt.op_addr = INDIRECT_ACCESS_STATE_REG;
    	while(ready == 0)
        {
    		ret = read_fpga_reg(&opt);
        	if(ret)
        	{
        		sem_syslog_warning("**read fpga indirect access control register failure!\n");
        		ret = -1;
        	}
            ready = opt.op_value & READY_REG_VALUE;           //Check Status of ready
    	}
		
		ready = 0;
    	/*end*/
/*		
        sem_syslog_warning("Indirect_add_reg_H = 0x%x\n", Indirect_add_reg_H);
        sem_syslog_warning("Indirect_add_reg_L = 0x%x\n", Indirect_add_reg_L);
        sem_syslog_warning("SIP_H = 0x%x\n", SIP_H);
        sem_syslog_warning("SIP_L = 0x%x\n", SIP_L);
        sem_syslog_warning("DIP_H = 0x%x\n", DIP_H);
    	sem_syslog_warning("DIP_L = 0x%x\n", DIP_L);
        sem_syslog_warning("SPORT = 0x%x\n", SPORT);
        sem_syslog_warning("DPORT = 0x%x\n", DPORT);
    	sem_syslog_warning("PROTOCOL = 0x%x\n", PROTOCOL);
    	sem_syslog_warning("num = 0x%x\n", NUM);
*/	
    	ret = write_cam_core(Indirect_add_reg_H,Indirect_add_reg_L,SIP_H,SIP_L,DIP_H,DIP_L,SPORT,DPORT,PROTOCOL);
        if(ret == 0)
        {
			flag =0;
            sem_syslog_warning("**write_cam_core failure!CAM ID = %x\n",Indirect_add_reg_L);
		}
        else if(ret == 1)
        {
			//sem_syslog_dbg("**write_cam_core success!\n");
		}
		
        Indirect_add_reg_L +=1;
				
		SPORT +=1;
		DPORT +=1;
	}
	
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &flag);
	return reply;

}
DBusMessage *sem_dbus_read_cam_core(DBusConnection *conn, DBusMessage *msg, void *user_data)
{	
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	read_cam_core_result_t *result = NULL;
	read_cam_core_param_t *read_cam_core_param = NULL;
	int slot_id;
    int ready = 0;
	unsigned short SIP_H=0;
	unsigned short SIP_L=0;
    unsigned short DIP_H=0;
	unsigned short DIP_L=0;
	unsigned short SPORT=0;
	unsigned short DPORT=0;
	unsigned short PROTOCOL=0;
	//unsigned short num=0;
	int ret;

	result = (read_cam_core_result_t *)malloc(sizeof(read_cam_core_result_t));

	//sem_syslog_warning("sem read cam core\n");

	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
						        DBUS_TYPE_UINT16, &SIP_H,
						        DBUS_TYPE_UINT16, &SIP_L,
						        DBUS_TYPE_UINT16, &DIP_H,
						        DBUS_TYPE_UINT16, &DIP_L,
						        DBUS_TYPE_UINT16, &SPORT,
						        DBUS_TYPE_UINT16, &DPORT,
						        DBUS_TYPE_UINT16, &PROTOCOL,
						        //DBUS_TYPE_UINT16, &num,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		sem_syslog_dbg("sem_dbus_read_cam_core get arg failed and reset normal\n");
	}
	/*
    sem_syslog_warning("SIP_H = 0x%x\n", SIP_H);
    sem_syslog_warning("SIP_L = 0x%x\n", SIP_L);
    sem_syslog_warning("DIP_H = 0x%x\n", DIP_H);
	sem_syslog_warning("DIP_L = 0x%x\n", DIP_L);
    sem_syslog_warning("SPORT = 0x%x\n", SPORT);
    sem_syslog_warning("DPORT = 0x%x\n", DPORT);
	sem_syslog_warning("PROTOCOL = 0x%x\n", PROTOCOL);
	//sem_syslog_warning("num = 0x%x\n", num);
    */
    /*test whether the  previous operation is completed */
	memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;
	opt.op_addr = INDIRECT_ACCESS_STATE_REG;
	while(ready == 0)
    {
		ret = read_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**read fpga indirect access control register failure!\n");
    	}
        ready = opt.op_value & READY_REG_VALUE;           //Check Status of ready
	}
	/*end*/

	ret = read_cam_core(result,SIP_H,SIP_L,DIP_H,DIP_L,SPORT,DPORT,PROTOCOL);
	{
        if(ret == 0)
        {
            sem_syslog_warning("**read_cam_core failure!\n");
		}
        else
        {
			//sem_syslog_warning("**read_cam_core success!\n");
		}
	}
	
	if(result->match_flag == 1)
	{
	    ret = result->cam_id;
	}
	else
	{
        ret = -1;
	}
	free(result);
	result = NULL;
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &ret);
	return reply;

}
DBusMessage *sem_dbus_write_cam(DBusConnection *conn, DBusMessage *msg, void *user_data)
{	
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	write_cam_param_t *write_cam_param = NULL;
	bm_op_args opt;
	int slot_id;
    int ready = 0;
	unsigned short Indirect_add_reg_H = 0;
	unsigned short Indirect_add_reg_L = 0;
	unsigned short SIP_H=0;
	unsigned short SIP_L=0;
    unsigned short DIP_H=0;
	unsigned short DIP_L=0;
	unsigned short SPORT=0;
	unsigned short DPORT=0;
	unsigned short PROTOCOL=0;
	unsigned short HASH_CTL=0;
	unsigned short DMAC_H=0;
	unsigned short DMAC_M=0;
	unsigned short DMAC_L=0;
	unsigned short SMAC_H=0;
	unsigned short SMAC_M=0;
	unsigned short SMAC_L=0;
	unsigned short VLAN_OUT_H=0;
	unsigned short VLAN_OUT_L=0;
	unsigned short VLAN_IN_H=0;
	unsigned short VLAN_IN_L=0;
	unsigned short IPTOS=0;
	unsigned short IP_TTL_IP_PROTOCOL=0;
	unsigned short IP_CHECKSUM=0;
	unsigned short TUNNEL_SIP_H=0;
	unsigned short TUNNEL_SIP_L=0;
	unsigned short TUNNEL_DIP_H=0;
	unsigned short TUNNEL_DIP_L=0;
	unsigned short TUNNEL_SRC_PORT=0;
	unsigned short TUNNEL_DST_PORT=0;
	unsigned short CAPWAP_B0B1=0;
	unsigned short CAPWAP_B2B3=0;
	unsigned short CAPWAP_B4B5=0;
	unsigned short CAPWAP_B6B7=0;
	unsigned short CAPWAP_B8B9=0;
	unsigned short CAPWAP_B10B11=0;
	unsigned short CAPWAP_B12B13=0;
	unsigned short CAPWAP_B14B15=0;
	unsigned short H_802_11_B0B1=0;
	unsigned short H_802_11_B2B3=0;
	unsigned short H_802_11_B4B5=0;
	unsigned short H_802_11_B6B7=0;
	unsigned short H_802_11_B8B9=0;
	unsigned short H_802_11_B10B11=0;
	unsigned short H_802_11_B12B13=0;
	unsigned short H_802_11_B14B15=0;
	unsigned short H_802_11_B16B17=0;
	unsigned short H_802_11_B18B19=0;
	unsigned short H_802_11_B20B21=0;
	unsigned short H_802_11_B22B23=0;
	unsigned short H_802_11_B24B25=0;
	unsigned short H_802_11_B26B27=0;
	unsigned short NUM = 0;
	int ret;
	int i;

    write_cam_param = (write_cam_param_t *)malloc(sizeof(write_cam_param_t));
	sem_syslog_dbg("sem write cam \n");

	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
                        DBUS_TYPE_UINT16, &Indirect_add_reg_H,
	                    DBUS_TYPE_UINT16, &Indirect_add_reg_L,
						DBUS_TYPE_UINT16, &SIP_H,
						DBUS_TYPE_UINT16, &SIP_L,
						DBUS_TYPE_UINT16, &DIP_H,
						DBUS_TYPE_UINT16, &DIP_L,
						DBUS_TYPE_UINT16, &SPORT,
						DBUS_TYPE_UINT16, &DPORT,
						DBUS_TYPE_UINT16, &PROTOCOL,
						DBUS_TYPE_UINT16, &HASH_CTL,
						DBUS_TYPE_UINT16, &DMAC_H,
						DBUS_TYPE_UINT16, &DMAC_M,
						DBUS_TYPE_UINT16, &DMAC_L,
						DBUS_TYPE_UINT16, &SMAC_H,
						DBUS_TYPE_UINT16, &SMAC_M,
						DBUS_TYPE_UINT16, &SMAC_L,
						DBUS_TYPE_UINT16, &VLAN_OUT_H,
						DBUS_TYPE_UINT16, &VLAN_OUT_L,
						DBUS_TYPE_UINT16, &VLAN_IN_H,
						DBUS_TYPE_UINT16, &VLAN_IN_L,
						DBUS_TYPE_UINT16, &IPTOS,
						DBUS_TYPE_UINT16, &IP_TTL_IP_PROTOCOL,
						DBUS_TYPE_UINT16, &IP_CHECKSUM,
						DBUS_TYPE_UINT16, &TUNNEL_SIP_H,
						DBUS_TYPE_UINT16, &TUNNEL_SIP_L,
						DBUS_TYPE_UINT16, &TUNNEL_DIP_H,
						DBUS_TYPE_UINT16, &TUNNEL_DIP_L,
						DBUS_TYPE_UINT16, &TUNNEL_SRC_PORT,
						DBUS_TYPE_UINT16, &TUNNEL_DST_PORT,
						DBUS_TYPE_UINT16, &CAPWAP_B0B1,
						DBUS_TYPE_UINT16, &CAPWAP_B2B3,
						DBUS_TYPE_UINT16, &CAPWAP_B4B5,
						DBUS_TYPE_UINT16, &CAPWAP_B6B7,
						DBUS_TYPE_UINT16, &CAPWAP_B8B9,
						DBUS_TYPE_UINT16, &CAPWAP_B10B11,
						DBUS_TYPE_UINT16, &CAPWAP_B12B13,
						DBUS_TYPE_UINT16, &CAPWAP_B14B15,
						DBUS_TYPE_UINT16, &H_802_11_B0B1,
						DBUS_TYPE_UINT16, &H_802_11_B2B3,
						DBUS_TYPE_UINT16, &H_802_11_B4B5,
						DBUS_TYPE_UINT16, &H_802_11_B6B7,
						DBUS_TYPE_UINT16, &H_802_11_B8B9,
						DBUS_TYPE_UINT16, &H_802_11_B10B11,
						DBUS_TYPE_UINT16, &H_802_11_B12B13,
						DBUS_TYPE_UINT16, &H_802_11_B14B15,
						DBUS_TYPE_UINT16, &H_802_11_B16B17,
						DBUS_TYPE_UINT16, &H_802_11_B18B19,
						DBUS_TYPE_UINT16, &H_802_11_B20B21,
						DBUS_TYPE_UINT16, &H_802_11_B22B23,
						DBUS_TYPE_UINT16, &H_802_11_B24B25,
						DBUS_TYPE_UINT16, &H_802_11_B26B27,
						DBUS_TYPE_UINT16, &NUM,
						DBUS_TYPE_INVALID)))
	{
    	if (dbus_error_is_set(&err)) 
    	{
    		sem_syslog_dbg("%s raised: %s", err.name, err.message);
    		dbus_error_free(&err);
    	}
		sem_syslog_dbg("sem_dbus_write_cam get arg failed and reset normal\n");
	}
	

/*
	sem_syslog_warning("Indirect_add_reg_H = 0x%x******\n",Indirect_add_reg_H);
	sem_syslog_warning("Indirect_add_reg_L = 0x%x******\n",Indirect_add_reg_L);
	sem_syslog_warning("SIP_H = 0x%x******\n",SIP_H);
	sem_syslog_warning("SIP_L = 0x%x******\n",SIP_L);
	sem_syslog_warning("DIP_H = 0x%x******\n",DIP_H);
	sem_syslog_warning("DIP_L = 0x%x******\n",DIP_L);
	sem_syslog_warning("SPORT = 0x%x******\n",SPORT);
    sem_syslog_warning("DPORT = 0x%x******\n",DPORT);
	sem_syslog_warning("PROTOCOL = 0x%x******\n",PROTOCOL);
    sem_syslog_warning("HASH_CTL = 0x%x******\n",HASH_CTL);
	sem_syslog_warning("DMAC_H = 0x%x******\n",DMAC_H);
	sem_syslog_warning("DMAC_M = 0x%x******\n",DMAC_M);
	sem_syslog_warning("DMAC_L = 0x%x******\n",DMAC_L);
	sem_syslog_warning("SMAC_H = 0x%x******\n",SMAC_H);
	sem_syslog_warning("SMAC_M = 0x%x******\n",SMAC_M);
	sem_syslog_warning("SMAC_L = 0x%x******\n",SMAC_L);
	sem_syslog_warning("VLAN_OUT_H = 0x%x******\n",VLAN_OUT_H);
	sem_syslog_warning("VLAN_OUT_L = 0x%x******\n",VLAN_OUT_L);
	sem_syslog_warning("VLAN_IN_H = 0x%x******\n",VLAN_IN_H);
	sem_syslog_warning("VLAN_IN_L = 0x%x******\n",VLAN_IN_L);
	sem_syslog_warning("IP_TOS = 0x%x******\n",IPTOS);
	sem_syslog_warning("IP_TTL_IP_PROTOCOL = 0x%x******\n",IP_TTL_IP_PROTOCOL);
	sem_syslog_warning("TUNNEL_SIP_H = 0x%x******\n",TUNNEL_SIP_H);
	sem_syslog_warning("TUNNEL_SIP_L = 0x%x******\n",TUNNEL_SIP_L);
	sem_syslog_warning("TUNNEL_DIP_H = 0x%x******\n",TUNNEL_DIP_H);
	sem_syslog_warning("TUNNEL_DIP_L = 0x%x******\n",TUNNEL_DIP_L);
	sem_syslog_warning("TUNNEL_SRC_PORT = 0x%x******\n",TUNNEL_SRC_PORT);
	sem_syslog_warning("TUNNEL_DST_PORT = 0x%x******\n",TUNNEL_DST_PORT);
	sem_syslog_warning("CAPWAP_B0B1 = 0x%x******\n",CAPWAP_B0B1);
	sem_syslog_warning("CAPWAP_B2B3 = 0x%x******\n",CAPWAP_B2B3);
	sem_syslog_warning("CAPWAP_B4B5 = 0x%x******\n",CAPWAP_B4B5);
	sem_syslog_warning("CAPWAP_B6B7 = 0x%x******\n",CAPWAP_B6B7);
	sem_syslog_warning("CAPWAP_B8B9 = 0x%x******\n",CAPWAP_B8B9);
	sem_syslog_warning("CAPWAP_B10B11 = 0x%x******\n",CAPWAP_B10B11);
	sem_syslog_warning("CAPWAP_B12B13 = 0x%x******\n",CAPWAP_B12B13);
	sem_syslog_warning("CAPWAP_B14B15 = 0x%x******\n",CAPWAP_B14B15);
	sem_syslog_warning("H_802_11_B0B1 = 0x%x******\n",H_802_11_B0B1);
	sem_syslog_warning("H_802_11_B2B3 = 0x%x******\n",H_802_11_B2B3);
	sem_syslog_warning("H_802_11_B4B5 = 0x%x******\n",H_802_11_B4B5);
	sem_syslog_warning("H_802_11_B6B7 = 0x%x******\n",H_802_11_B6B7);
	sem_syslog_warning("H_802_11_B8B9 = 0x%x******\n",H_802_11_B8B9);
	sem_syslog_warning("H_802_11_B10B11 = 0x%x******\n",H_802_11_B10B11);
	sem_syslog_warning("H_802_11_B12B13 = 0x%x******\n",H_802_11_B12B13);
	sem_syslog_warning("H_802_11_B14B15 = 0x%x******\n",H_802_11_B14B15);
	sem_syslog_warning("H_802_11_B16B17 = 0x%x******\n",H_802_11_B16B17);
	sem_syslog_warning("H_802_11_B18B19 = 0x%x******\n",H_802_11_B18B19);
	sem_syslog_warning("H_802_11_B20B21 = 0x%x******\n",H_802_11_B20B21);
	sem_syslog_warning("H_802_11_B22B23 = 0x%x******\n",H_802_11_B22B23);
	sem_syslog_warning("H_802_11_B24B25 = 0x%x******\n",H_802_11_B24B25);
	sem_syslog_warning("H_802_11_B26B27 = 0x%x******\n",H_802_11_B26B27);
	sem_syslog_warning("NUM = 0x%x\n", NUM);
*/
    write_cam_param->Indirect_add_reg_H = Indirect_add_reg_H;
    write_cam_param->Indirect_add_reg_L = Indirect_add_reg_L;
    write_cam_param->SIP_H = SIP_H;
	write_cam_param->SIP_L = SIP_L;
	write_cam_param->DIP_H = DIP_H;
	write_cam_param->DIP_L = DIP_L;
	write_cam_param->SPORT = SPORT;
    write_cam_param->DPORT = DPORT;
    write_cam_param->PROTOCOL = PROTOCOL;
    write_cam_param->HASH_CTL = HASH_CTL;
	write_cam_param->DMAC_H = DMAC_H;
	write_cam_param->DMAC_M = DMAC_M;
	write_cam_param->DMAC_L = DMAC_L;
	write_cam_param->SMAC_H = SMAC_H;
    write_cam_param->SMAC_M = SMAC_M;
    write_cam_param->SMAC_L = SMAC_L;
    write_cam_param->VLAN_OUT_H = VLAN_OUT_H;
	write_cam_param->VLAN_OUT_L = VLAN_OUT_L;
	write_cam_param->VLAN_IN_H = VLAN_IN_H;
	write_cam_param->VLAN_IN_L = VLAN_IN_L;
	write_cam_param->IPTOS = IPTOS ;
    write_cam_param->IP_TTL_IP_PROTOCOL = IP_TTL_IP_PROTOCOL;
	write_cam_param->IP_CHECKSUM = IP_CHECKSUM;
    write_cam_param->TUNNEL_SIP_H = TUNNEL_SIP_H;
    write_cam_param->TUNNEL_SIP_L = TUNNEL_SIP_L;
	write_cam_param->TUNNEL_DIP_H = TUNNEL_DIP_H;
	write_cam_param->TUNNEL_DIP_L = TUNNEL_DIP_L;
	write_cam_param->TUNNEL_SRC_PORT = TUNNEL_SRC_PORT;
	write_cam_param->TUNNEL_DST_PORT = TUNNEL_DST_PORT;
	write_cam_param->CAPWAP_B0B1 = CAPWAP_B0B1;
	write_cam_param->CAPWAP_B2B3 = CAPWAP_B2B3;
	write_cam_param->CAPWAP_B4B5 = CAPWAP_B4B5;
	write_cam_param->CAPWAP_B6B7 = CAPWAP_B6B7;
	write_cam_param->CAPWAP_B8B9 = CAPWAP_B8B9;
	write_cam_param->CAPWAP_B10B11 = CAPWAP_B10B11;
	write_cam_param->CAPWAP_B12B13 = CAPWAP_B12B13;
	write_cam_param->CAPWAP_B14B15 = CAPWAP_B14B15;
	write_cam_param->H_802_11_B0B1 = H_802_11_B0B1;
	write_cam_param->H_802_11_B2B3 = H_802_11_B2B3;
	write_cam_param->H_802_11_B4B5 = H_802_11_B4B5;
	write_cam_param->H_802_11_B6B7 = H_802_11_B6B7;
	write_cam_param->H_802_11_B8B9 = H_802_11_B8B9;
	write_cam_param->H_802_11_B10B11 = H_802_11_B10B11;
	write_cam_param->H_802_11_B12B13 = H_802_11_B12B13;
	write_cam_param->H_802_11_B14B15 = H_802_11_B14B15;
	write_cam_param->H_802_11_B16B17 = H_802_11_B16B17;
	write_cam_param->H_802_11_B18B19 = H_802_11_B18B19;
	write_cam_param->H_802_11_B20B21 = H_802_11_B20B21;
	write_cam_param->H_802_11_B22B23 = H_802_11_B22B23;
	write_cam_param->H_802_11_B24B25 = H_802_11_B24B25;
	write_cam_param->H_802_11_B26B27 = H_802_11_B26B27;

	for(i=0;i <=NUM-1;i++)
	{

        /*test whether the  previous operation is completed */
    	memset(&opt,0,sizeof(bm_op_args));
    	opt.op_len = 16;
    	opt.op_addr = INDIRECT_ACCESS_STATE_REG;
    	while(ready == 0)
        {
    		ret = read_fpga_reg(&opt);
        	if(ret)
        	{
        		sem_syslog_warning("**read fpga indirect access control register failure!\n");
        		ret = -1;
        	}
            ready = opt.op_value & READY_REG_VALUE;           //Check Status of ready
    	}
		ready = 0;
    	/*end*/
		
        ret = write_cam(write_cam_param);

        if(ret == 0)
        {
            sem_syslog_warning("**write_cam failure!\n");
		}
        else
        {
			//sem_syslog_warning("**write_cam success!\n");
		}
		
        write_cam_param->Indirect_add_reg_L +=1; 
		
	    write_cam_param->SPORT +=1; 
	    write_cam_param->DPORT +=1; 
	}
	free(write_cam_param);
	write_cam_param = NULL;
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &ret);
	return reply;

}
DBusMessage *sem_dbus_read_hash(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int slot_id;
    int ready = 0;
	unsigned int hash_id;
	unsigned short Indirect_add_reg_H = 0;
	unsigned short Indirect_add_reg_L = 0;
	unsigned short ret;

	unsigned short cam_table_id = 0;
	unsigned short sip_H = 0;
	unsigned short sip_L = 0;
	unsigned short sport = 0;
	unsigned short smac_H = 0;
    unsigned short smac_M = 0;
	unsigned short smac_L = 0;
	unsigned short tunnel_sip_H = 0;
	unsigned short tunnel_sip_L = 0;
	unsigned short tunnel_sport = 0;
	unsigned short dip_H = 0;
	unsigned short dip_L = 0;
	unsigned short dport = 0;
	unsigned short dmac_H = 0;
    unsigned short dmac_M = 0;
	unsigned short dmac_L = 0;
	unsigned short tunnel_dip_H = 0;
	unsigned short tunnel_dip_L = 0;
	unsigned short tunnel_dport = 0;
    unsigned short protocol = 0;
	unsigned short setup_time = 0;
	unsigned short hash_ctl = 0;
	unsigned short vlan_out_H = 0;
	unsigned short vlan_out_L = 0;
	unsigned short vlan_in_H = 0;
	unsigned short vlan_in_L = 0;
	unsigned short ethernet_protocol = 0;
	unsigned short ip_tos = 0;
	unsigned short ip_len = 0;
	unsigned short ip_offset = 0;
	unsigned short ip_ttl = 0;
	unsigned short ip_protocol = 0;
    unsigned short ip_checksum = 0;
	unsigned short udp_len = 0;
    unsigned short udp_checksum = 0;	
    unsigned short capwap_header_b0 = 0;
	unsigned short capwap_header_b2 = 0;
	unsigned short capwap_header_b4 = 0;
	unsigned short capwap_header_b6 = 0;
	unsigned short capwap_header_b8 = 0;
	unsigned short capwap_header_b10 = 0;
	unsigned short capwap_header_b12 = 0;
	unsigned short capwap_header_b14 = 0;
    unsigned short header_802_11_b0 = 0;
	unsigned short header_802_11_b2 = 0;
	unsigned short header_802_11_b4 = 0;
	unsigned short header_802_11_b6 = 0;
	unsigned short header_802_11_b8 = 0;
	unsigned short header_802_11_b10 = 0;
	unsigned short header_802_11_b12 = 0;
	unsigned short header_802_11_b14 = 0;
	unsigned short header_802_11_b16 = 0;
	unsigned short header_802_11_b18 = 0;
	unsigned short header_802_11_b20 = 0;
	unsigned short header_802_11_b22 = 0;
	unsigned short header_802_11_b24 = 0;
	unsigned short header_802_11_b26 = 0;
	unsigned short capwap_protocol = 0;
    hash_table_t *read_hash_result = NULL;

	read_hash_result = (hash_table_t *)malloc(sizeof(hash_table_t));
	memset(read_hash_result,0,sizeof(hash_table_t));
	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
                                DBUS_TYPE_UINT32, &hash_id,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		sem_syslog_dbg("sem_dbus_read_cam_core get arg failed and reset normal\n");
	}

	ret = read_hash(read_hash_result,hash_id,1);
    if(read_hash_result == 0)
    {
		ret = -1;
        sem_syslog_warning("**read_hash 1 failure!\n");
	}
    else
    {
		//sem_syslog_warning("read_hash 1 success\n");
		//sem_syslog_warning("read_hash_result->sip_H = %x\n",read_hash_result->sip_H);
    	sip_H = read_hash_result->sip_H;
    	sip_L = read_hash_result->sip_L;
    	dip_H = read_hash_result->dip_H;
    	dip_L = read_hash_result->dip_L;
    	sport = read_hash_result->sport;
    	dport = read_hash_result->dport;
    	protocol = read_hash_result->protocol >> 8;
    	setup_time = read_hash_result->setup_time;
    	hash_ctl = read_hash_result->hash_ctl & 0xff;
    	dmac_H = read_hash_result->dmac_H;
    	dmac_M = read_hash_result->dmac_M;
    	dmac_L = read_hash_result->dmac_L;
    	smac_H = read_hash_result->smac_H;
    	smac_M = read_hash_result->smac_M;
    	smac_L = read_hash_result->smac_L;
    	vlan_out_H = read_hash_result->vlan_out_H;
    	vlan_out_L = read_hash_result->vlan_out_L;
    	vlan_in_H = read_hash_result->vlan_in_H;
    	vlan_in_L = read_hash_result->vlan_in_L;
    	ethernet_protocol = read_hash_result->ethernet_protocol;
    	ip_tos = read_hash_result->ip_tos & 0xff;
    	ip_len = read_hash_result->ip_len;
    	ip_offset = read_hash_result->ip_offset;
    	ip_ttl = read_hash_result->ip_ttl >> 8;
    	ip_protocol = read_hash_result->ip_protocol & 0xff;
    	ip_checksum = read_hash_result->ip_checksum;
    	tunnel_sip_H = read_hash_result->tunnel_sip_H;
    	tunnel_sip_L = read_hash_result->tunnel_sip_L;
    	tunnel_dip_H = read_hash_result->tunnel_dip_H;
    	tunnel_dip_L = read_hash_result->tunnel_dip_L;
    	tunnel_sport = read_hash_result->tunnel_sport;
    	tunnel_dport = read_hash_result->tunnel_dport;
    	udp_len = read_hash_result->udp_len;
    	udp_checksum = read_hash_result->udp_checksum;
    	capwap_header_b0 = read_hash_result->capwap_header_b0;
    	capwap_header_b2 = read_hash_result->capwap_header_b2;
    	capwap_header_b4 = read_hash_result->capwap_header_b4;
    	capwap_header_b6 = read_hash_result->capwap_header_b6;
    	capwap_header_b8 = read_hash_result->capwap_header_b8;
    	capwap_header_b10 = read_hash_result->capwap_header_b10;
    	capwap_header_b12 = read_hash_result->capwap_header_b12;
    	capwap_header_b14 = read_hash_result->capwap_header_b14;
    	header_802_11_b0 = read_hash_result->header_802_11_b0;
    	header_802_11_b2 = read_hash_result->header_802_11_b2;
    	header_802_11_b4 = read_hash_result->header_802_11_b4;
    	header_802_11_b6 = read_hash_result->header_802_11_b6;
    	header_802_11_b8 = read_hash_result->header_802_11_b8;
    	header_802_11_b10 = read_hash_result->header_802_11_b10;
        header_802_11_b12 = read_hash_result->header_802_11_b12;
    	header_802_11_b14 = read_hash_result->header_802_11_b14;
    	header_802_11_b16 = read_hash_result->header_802_11_b16;
    	header_802_11_b18 = read_hash_result->header_802_11_b18;
        header_802_11_b20 = read_hash_result->header_802_11_b20;
    	header_802_11_b22 = read_hash_result->header_802_11_b22;
    	header_802_11_b24 = read_hash_result->header_802_11_b24;
    	header_802_11_b26 = read_hash_result->header_802_11_b26;
        capwap_protocol = read_hash_result->capwap_protocol;
		ret = 0;
	}
	free(read_hash_result);
	read_hash_result = NULL;
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);//58 parames

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &ret);	
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &sip_H);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &sip_L);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &sport);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &smac_H);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &smac_M);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &smac_L);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &tunnel_sip_H);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &tunnel_sip_L);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &tunnel_sport);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &dip_H);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &dip_L);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &dport);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &dmac_H);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &dmac_M);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &dmac_L);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &tunnel_dip_H);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &tunnel_dip_L);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &tunnel_dport);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &protocol);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &setup_time);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &hash_ctl);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &vlan_out_H);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &vlan_out_L);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &vlan_in_H);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &vlan_in_L);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &ethernet_protocol);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &ip_tos);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &ip_len);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &ip_offset);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &ip_ttl);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &ip_protocol);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &ip_checksum);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &udp_len);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &udp_checksum);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &capwap_header_b0);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &capwap_header_b2);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &capwap_header_b4);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &capwap_header_b6);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &capwap_header_b8);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &capwap_header_b10);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &capwap_header_b12);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &capwap_header_b14);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b0);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b2);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b4);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b6);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b8);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b10);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b12);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b14);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b16);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b18);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b20);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b22);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b24);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b26);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &capwap_protocol);
		
	return reply;

	
}
DBusMessage *sem_dbus_read_cam(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int slot_id;
    int ready = 0;
	unsigned short Indirect_add_reg_H = 0;
	unsigned short Indirect_add_reg_L = 0;
	unsigned short ret;

	unsigned short cam_table_id = 0;
	unsigned short sip_H = 0;
	unsigned short sip_L = 0;
	unsigned short sport = 0;
	unsigned short smac_H = 0;
    unsigned short smac_M = 0;
	unsigned short smac_L = 0;
	unsigned short tunnel_sip_H = 0;
	unsigned short tunnel_sip_L = 0;
	unsigned short tunnel_sport = 0;
	unsigned short dip_H = 0;
	unsigned short dip_L = 0;
	unsigned short dport = 0;
	unsigned short dmac_H = 0;
    unsigned short dmac_M = 0;
	unsigned short dmac_L = 0;
	unsigned short tunnel_dip_H = 0;
	unsigned short tunnel_dip_L = 0;
	unsigned short tunnel_dport = 0;
    unsigned short protocol = 0;
	unsigned short setup_time = 0;
	unsigned short hash_ctl = 0;
	unsigned short vlan_out_H = 0;
	unsigned short vlan_out_L = 0;
	unsigned short vlan_in_H = 0;
	unsigned short vlan_in_L = 0;
	unsigned short ethernet_protocol = 0;
	unsigned short ip_tos = 0;
	unsigned short ip_len = 0;
	unsigned short ip_offset = 0;
	unsigned short ip_ttl = 0;
	unsigned short ip_protocol = 0;
    unsigned short ip_checksum = 0;
	unsigned short udp_len = 0;
    unsigned short udp_checksum = 0;	
    unsigned short capwap_header_b0 = 0;
	unsigned short capwap_header_b2 = 0;
	unsigned short capwap_header_b4 = 0;
	unsigned short capwap_header_b6 = 0;
	unsigned short capwap_header_b8 = 0;
	unsigned short capwap_header_b10 = 0;
	unsigned short capwap_header_b12 = 0;
	unsigned short capwap_header_b14 = 0;
    unsigned short header_802_11_b0 = 0;
	unsigned short header_802_11_b2 = 0;
	unsigned short header_802_11_b4 = 0;
	unsigned short header_802_11_b6 = 0;
	unsigned short header_802_11_b8 = 0;
	unsigned short header_802_11_b10 = 0;
	unsigned short header_802_11_b12 = 0;
	unsigned short header_802_11_b14 = 0;
	unsigned short header_802_11_b16 = 0;
	unsigned short header_802_11_b18 = 0;
	unsigned short header_802_11_b20 = 0;
	unsigned short header_802_11_b22 = 0;
	unsigned short header_802_11_b24 = 0;
	unsigned short header_802_11_b26 = 0;
	unsigned short capwap_protocol = 0;
    read_cam_result_t *read_cam_result = NULL;

	read_cam_result = (read_cam_result_t *)malloc(sizeof(read_cam_result_t));
	
	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
                                DBUS_TYPE_UINT16, &Indirect_add_reg_H,
	                            DBUS_TYPE_UINT16, &Indirect_add_reg_L,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		sem_syslog_dbg("sem_dbus_read_cam_core get arg failed and reset normal\n");
	}
    //sem_syslog_warning("Indirect_add_reg_H = 0x%x\n", Indirect_add_reg_H);
    //sem_syslog_warning("Indirect_add_reg_L = 0x%x\n", Indirect_add_reg_L);
	//sem_syslog_warning("N = 0x%x\n", N);
    /*test whether the  previous operation is completed */
	memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;
	opt.op_addr = INDIRECT_ACCESS_STATE_REG;
	while(ready == 0)
    {
		ret = read_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**read fpga indirect access control register failure!\n");
    	}
        ready = opt.op_value & READY_REG_VALUE;           //Check Status of ready
	}
	/*end*/

	ret = read_cam(read_cam_result,Indirect_add_reg_H,Indirect_add_reg_L);
	{
        if(ret == 0)
        {
			ret = -1;
            sem_syslog_warning("**read_cam failure!\n");
		}
        else
        {
			ret = 0;
			cam_table_id = read_cam_result->cam_table_id;
        	sip_H = read_cam_result->sip_H;
        	sip_L = read_cam_result->sip_L;
        	dip_H = read_cam_result->dip_H;
        	dip_L = read_cam_result->dip_L;
        	sport = read_cam_result->sport;
        	dport = read_cam_result->dport;
        	protocol = read_cam_result->protocol;
        	setup_time = read_cam_result->setup_time;
        	hash_ctl = read_cam_result->hash_ctl;
        	dmac_H = read_cam_result->dmac_H;
        	dmac_M = read_cam_result->dmac_M;
        	dmac_L = read_cam_result->dmac_L;
        	smac_H = read_cam_result->smac_H;
        	smac_M = read_cam_result->smac_M;
        	smac_L = read_cam_result->smac_L;
        	vlan_out_H = read_cam_result->vlan_out_H;
        	vlan_out_L = read_cam_result->vlan_out_L;
        	vlan_in_H = read_cam_result->vlan_in_H;
        	vlan_in_L = read_cam_result->vlan_in_L;
        	ethernet_protocol = read_cam_result->ethernet_protocol;
        	ip_tos = read_cam_result->ip_tos;
        	ip_len = read_cam_result->ip_len;
        	ip_offset = read_cam_result->ip_offset;
        	ip_ttl = read_cam_result->ip_ttl;
        	ip_protocol = read_cam_result->ip_protocol;
        	ip_checksum = read_cam_result->ip_checksum;
        	tunnel_sip_H = read_cam_result->tunnel_sip_H;
        	tunnel_sip_L = read_cam_result->tunnel_sip_L;
        	tunnel_dip_H = read_cam_result->tunnel_dip_H;
        	tunnel_dip_L = read_cam_result->tunnel_dip_L;
        	tunnel_sport = read_cam_result->tunnel_sport;
        	tunnel_dport = read_cam_result->tunnel_dport;
        	udp_len = read_cam_result->udp_len;
        	udp_checksum = read_cam_result->udp_checksum;
        	capwap_header_b0 = read_cam_result->capwap_header_b0;
        	capwap_header_b2 = read_cam_result->capwap_header_b2;
        	capwap_header_b4 = read_cam_result->capwap_header_b4;
        	capwap_header_b6 = read_cam_result->capwap_header_b6;
        	capwap_header_b8 = read_cam_result->capwap_header_b8;
        	capwap_header_b10 = read_cam_result->capwap_header_b10;
        	capwap_header_b12 = read_cam_result->capwap_header_b12;
        	capwap_header_b14 = read_cam_result->capwap_header_b14;
        	header_802_11_b0 = read_cam_result->header_802_11_b0;
        	header_802_11_b2 = read_cam_result->header_802_11_b2;
        	header_802_11_b4 = read_cam_result->header_802_11_b4;
        	header_802_11_b6 = read_cam_result->header_802_11_b6;
        	header_802_11_b8 = read_cam_result->header_802_11_b8;
        	header_802_11_b10 = read_cam_result->header_802_11_b10;
            header_802_11_b12 = read_cam_result->header_802_11_b12;
        	header_802_11_b14 = read_cam_result->header_802_11_b14;
        	header_802_11_b16 = read_cam_result->header_802_11_b16;
        	header_802_11_b18 = read_cam_result->header_802_11_b18;
            header_802_11_b20 = read_cam_result->header_802_11_b20;
        	header_802_11_b22 = read_cam_result->header_802_11_b22;
        	header_802_11_b24 = read_cam_result->header_802_11_b24;
        	header_802_11_b26 = read_cam_result->header_802_11_b26;
            capwap_protocol = read_cam_result->capwap_protocol;
		}
	}
	free(read_cam_result);
	read_cam_result = NULL;
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);//58 parames

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &ret);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &cam_table_id);	
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &sip_H);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &sip_L);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &sport);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &smac_H);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &smac_M);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &smac_L);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &tunnel_sip_H);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &tunnel_sip_L);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &tunnel_sport);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &dip_H);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &dip_L);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &dport);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &dmac_H);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &dmac_M);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &dmac_L);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &tunnel_dip_H);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &tunnel_dip_L);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &tunnel_dport);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &protocol);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &setup_time);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &hash_ctl);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &vlan_out_H);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &vlan_out_L);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &vlan_in_H);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &vlan_in_L);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &ethernet_protocol);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &ip_tos);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &ip_len);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &ip_offset);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &ip_ttl);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &ip_protocol);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &ip_checksum);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &udp_len);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &udp_checksum);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &capwap_header_b0);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &capwap_header_b2);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &capwap_header_b4);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &capwap_header_b6);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &capwap_header_b8);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &capwap_header_b10);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &capwap_header_b12);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &capwap_header_b14);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b0);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b2);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b4);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b6);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b8);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b10);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b12);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b14);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b16);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b18);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b20);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b22);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b24);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &header_802_11_b26);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &capwap_protocol);
	
	sem_syslog_event("**sem_dbus_read_cam success!\n");
	
	return reply;

}

DBusMessage *sem_dbus_set_hash_update_time(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int slot_id;
    int ready = 0;
	unsigned short time = 0;
	int ret;

	//sem_syslog_dbg("sem set hash update time\n");

	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
                                DBUS_TYPE_UINT16, &time,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		sem_syslog_dbg("sem_dbus_set_hash_update_time get arg failed and reset normal\n");
	}
    //sem_syslog_warning("time = 0x%x\n", time);
	
	memset(&opt,0,sizeof(bm_op_args));	
	opt.op_len = 16;	
	opt.op_value = time;
	opt.op_addr = HASH_UPDATA;
	ret = write_fpga_reg(&opt);
    //sem_syslog_warning("ret = 0x%x\n", ret);
	
    reply = dbus_message_new_method_return(msg);
    
	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &ret);
	return reply;

}

DBusMessage *sem_dbus_set_hash_aging_time(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int slot_id;
    int ready = 0;
	unsigned short time = 0;
	int ret;

	//sem_syslog_dbg("sem set hash aging time\n");

	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
                                DBUS_TYPE_UINT16, &time,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		sem_syslog_dbg("sem_dbus_set_hash_aging_time get arg failed and reset normal\n");
	}
    //sem_syslog_warning("time = 0x%x\n", time);
	
	memset(&opt,0,sizeof(bm_op_args));	
	opt.op_len = 16;	
	opt.op_value = time;
	opt.op_addr = HASH_AGING_TIME;
	ret = write_fpga_reg(&opt);
    //sem_syslog_warning("ret = 0x%x\n", ret);
	
    reply = dbus_message_new_method_return(msg);
    
	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &ret);
	return reply;

}

DBusMessage *sem_dbus_show_hash_capacity(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	unsigned long long num = 0 ; 
    hash_statistics_t *hash_statistics = NULL;
	int read_hash_ret=0;

	hash_statistics = (hash_statistics_t *)malloc(sizeof(hash_statistics_t));
	memset(hash_statistics,0,sizeof(hash_statistics_t));

    read_hash_ret = read_hash_statistics(hash_statistics);
	if(!read_hash_ret)
	{
        sem_syslog_warning("read_hash_statistics fail\n");
	}
    num = hash_statistics->hash_learn_num - hash_statistics->hash_aging_num;
    //sem_syslog_warning("num = %llx\n",num);
    free(hash_statistics);
	hash_statistics =NULL;
	reply = dbus_message_new_method_return(msg);
    
	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT64,
								 &num);
	//sem_syslog_warning("sem_dbus_show_hash_capacity success!\n");

	return reply;

}

DBusMessage *sem_dbus_show_fpga_reg(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int ret = 0;
	
    unsigned int device_id = 0;
    unsigned int sys_version = 0;
    unsigned int sys_ctl_1 = 0;
	unsigned int sys_ctl_2 = 0;
    unsigned int sys_state = 0;
    unsigned int hash_age = 0;
    unsigned int hash_updata = 0;

    memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;	
	opt.op_addr = DEVICE_ID;
	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**read device id register failure!\n");
		return NULL;
	}
    device_id = opt.op_value ; 

	opt.op_addr = SYS_VERSION;
	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**read systerm version register failure!\n");
		return NULL;
	}
    sys_version = opt.op_value ; 

	opt.op_addr = SYS_CTL_1;
	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**read systerm control register failure!\n");
		return NULL;
	}
    sys_ctl_1 = opt.op_value ; 

	opt.op_addr = SYS_CTL_2;
	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**read systerm control register failure!\n");
		return NULL;
	}
    sys_ctl_2 = opt.op_value ; 

	opt.op_addr = SYS_STATE;
	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**read systerm state register failure!\n");
		return NULL;
	}
    sys_state = opt.op_value ; 

	opt.op_addr = HASH_AGE;
	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**read hash aging register failure!\n");
		return NULL;
	}
    hash_age = opt.op_value ;
	
	opt.op_addr = HASH_UPDATA;
	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**read hash updata register failure!\n");
		return NULL;
	}
    hash_updata = opt.op_value ;

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &device_id);
 	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &sys_version);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &sys_ctl_1);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &sys_ctl_2);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &sys_state);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &hash_age);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &hash_updata);
	//sem_syslog_dbg("**sem_dbus_show_fpga_reg success!\n");
	return reply;
}

DBusMessage *sem_dbus_config_fpga_sysreg_working(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int ret = 0;
	
    unsigned short work_flag = 1;
	unsigned int sys_ctl_1 = 0;

	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
		                        DBUS_TYPE_UINT16, &work_flag,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		sem_syslog_dbg("sem_dbus_write_car get arg failed and reset normal\n");
	}

    memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;	
	opt.op_addr = SYS_CTL_1;
	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**read systerm control register failure!\n");
		return NULL;
	}
    sys_ctl_1 = opt.op_value ;

	if(work_flag == 1)
	{
        sys_ctl_1 |= 1<<15;
	}
    else
    {
        sys_ctl_1 &= 0x7fff;
	}
	//sem_syslog_warning("**sys_ctl_1 = 0x%x!\n",sys_ctl_1);
	
	memset(&opt,0,sizeof(bm_op_args));	
	opt.op_len = 16;	
	opt.op_value = sys_ctl_1;
	opt.op_addr = SYS_CTL_1;
	ret = write_fpga_reg(&opt);

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_INT32,
								 &ret);
	//sem_syslog_dbg("**sem_dbus_config_fpga_sysreg_working success!\n");
	return reply;
}

DBusMessage *sem_dbus_config_fpga_sysreg_QoS(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int ret = 0;
	
    unsigned short QoS_flag = 1;
	unsigned int sys_ctl_1 = 0;

	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
		                        DBUS_TYPE_UINT16, &QoS_flag,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		sem_syslog_dbg("sem_dbus_config_fpga_sysreg_QoS get arg failed and reset normal\n");
	}

    memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;	
	opt.op_addr = SYS_CTL_1;
	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**read systerm control register failure!\n");
		return NULL;
	}
    sys_ctl_1 = opt.op_value ;

	if(QoS_flag == 1)
	{
        sys_ctl_1 |= 1<<14;
	}
    else
    {
        sys_ctl_1 &= 0xbfff;
	}
	//sem_syslog_warning("**sys_ctl_1 = 0x%x!\n",sys_ctl_1);
	memset(&opt,0,sizeof(bm_op_args));	
	opt.op_len = 16;	
	opt.op_value = sys_ctl_1;
	opt.op_addr = SYS_CTL_1;
	ret = write_fpga_reg(&opt);

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_INT32,
								 &ret);
	//sem_syslog_dbg("**sem_dbus_config_fpga_sysreg_QoS success!\n");
	return reply;
}

DBusMessage *sem_dbus_config_fpga_sysreg_mode(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int ret = 0;
	
    unsigned short mode_flag = 1;
	unsigned int sys_ctl_1 = 0;

	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
		                        DBUS_TYPE_UINT16, &mode_flag,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		sem_syslog_dbg("sem_dbus_config_fpga_sysreg_mode get arg failed and reset normal\n");
	}

    memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;	
	opt.op_addr = SYS_CTL_1;
	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**read systerm control register failure!\n");
		return NULL;
	}
    sys_ctl_1 = opt.op_value ;

	if(mode_flag == 1)
	{
        sys_ctl_1 |= 1<<13;
	}
    else
    {
        sys_ctl_1 &= 0xdfff;
	}
	//sem_syslog_warning("**sys_ctl_1 = 0x%x!\n",sys_ctl_1);
	memset(&opt,0,sizeof(bm_op_args));	
	opt.op_len = 16;	
	opt.op_value = sys_ctl_1;
	opt.op_addr = SYS_CTL_1;
	ret = write_fpga_reg(&opt);

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_INT32,
								 &ret);
	//sem_syslog_dbg("**sem_dbus_config_fpga_sysreg_mode success!\n");
	return reply;
}

DBusMessage *sem_dbus_config_fpga_sysreg_car_update_cfg(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int ret = 0;
	
    unsigned short car_update_cfg_flag = 0;
	unsigned int sys_ctl_1 = 0;

	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
		                        DBUS_TYPE_UINT16, &car_update_cfg_flag,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		sem_syslog_dbg("sem_dbus_config_fpga_sysreg_mode get arg failed and reset normal\n");
	}

    memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;	
	opt.op_addr = SYS_CTL_1;
	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**read systerm control register failure!\n");
		return NULL;
	}
    sys_ctl_1 = opt.op_value ;

	if(car_update_cfg_flag == 0)
	{
		sys_ctl_1 &= 0xff3f;
	}
    else if(car_update_cfg_flag == 1)
    {
		sys_ctl_1 |= 1<<6;
        sys_ctl_1 &= 0xff7f;
	}
	else if(car_update_cfg_flag == 2)
    {
        sys_ctl_1 |= 1<<6;
		sys_ctl_1 |= 1<<7;
	}
	else
	{
        sem_syslog_warning("**get bad param car_update_cfg_flag = %d!\n",car_update_cfg_flag);
	}
	//sem_syslog_warning("**sys_ctl_1 = 0x%x!\n",sys_ctl_1);
	memset(&opt,0,sizeof(bm_op_args));	
	opt.op_len = 16;	
	opt.op_value = sys_ctl_1;
	opt.op_addr = SYS_CTL_1;
	ret = write_fpga_reg(&opt);

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_INT32,
								 &ret);
	//sem_syslog_dbg("**sem_dbus_config_fpga_sysreg_car_update_cfg success!\n");
	return reply;
}

DBusMessage *sem_dbus_config_fpga_sysreg_trunk_tag_type(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int ret = 0;
	
    unsigned short vlan_flag = 0;
	unsigned int sys_ctl_1 = 0;

	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
		                        DBUS_TYPE_UINT16, &vlan_flag,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		sem_syslog_dbg("sem_dbus_config_fpga_sysreg_mode get arg failed and reset normal\n");
	}

    memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;	
	opt.op_addr = SYS_CTL_1;
	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**read systerm control register failure!\n");
		return NULL;
	}
    sys_ctl_1 = opt.op_value ;

	if(vlan_flag == 0)
	{
		sys_ctl_1 |= 1<<4;
		sys_ctl_1 |= 1<<5;
	}
    else if(vlan_flag == 1)
    {
        sys_ctl_1 &= 0xffcf;
	}
	else
	{
        sem_syslog_warning("**get bad param vlan_flag = %d!\n",vlan_flag);
	}
	//sem_syslog_warning("**sys_ctl_1 = 0x%x!\n",sys_ctl_1);
	memset(&opt,0,sizeof(bm_op_args));	
	opt.op_len = 16;	
	opt.op_value = sys_ctl_1;
	opt.op_addr = SYS_CTL_1;
	ret = write_fpga_reg(&opt);

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_INT32,
								 &ret);
	//sem_syslog_dbg("**sem_dbus_config_fpga_sysreg_trunk_tag_type success!\n");
	return reply;
}

DBusMessage *sem_dbus_config_fpga_sysreg_wan_tag_type(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int ret = 0;
	
    unsigned short wan_vlan_flag = 0;
	unsigned int sys_ctl_1 = 0;

	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
		                        DBUS_TYPE_UINT16, &wan_vlan_flag,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		sem_syslog_dbg("sem_dbus_config_fpga_sysreg_wan_tag_type get arg failed and reset normal\n");
	}

    memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;	
	opt.op_addr = SYS_CTL_1;
	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**read systerm control register failure!\n");
		return NULL;
	}
    sys_ctl_1 = opt.op_value ;

	if(wan_vlan_flag == 0)
	{
		sys_ctl_1 &= 0xfff3;
	}
    else if(wan_vlan_flag == 1)
    {
        sys_ctl_1 |= 1<<2;
		sys_ctl_1 &= 0xfff7;
	}
	else if(wan_vlan_flag == 2)
    {
        sys_ctl_1 |= 1<<3;
		sys_ctl_1 &= 0xfffb;
	}
	else
	{
        sem_syslog_warning("**get bad param wan_vlan_flag = %d!\n",wan_vlan_flag);
	}
	//sem_syslog_warning("**sys_ctl_1 = 0x%x!\n",sys_ctl_1);
	memset(&opt,0,sizeof(bm_op_args));	
	opt.op_len = 16;	
	opt.op_value = sys_ctl_1;
	opt.op_addr = SYS_CTL_1;
	ret = write_fpga_reg(&opt);

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_INT32,
								 &ret);
	//sem_syslog_dbg("**sem_dbus_config_fpga_sysreg_wan_tag_type success!\n");
	return reply;
}

DBusMessage *sem_dbus_config_fpga_sysreg_car_linkup(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int ret = 0;
	
    unsigned short car_linkup_flag = 0;
	unsigned int sys_ctl_1 = 0;

	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
		                        DBUS_TYPE_UINT16, &car_linkup_flag,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		sem_syslog_dbg("sem_dbus_config_fpga_sysreg_car_linkup get arg failed and reset normal\n");
	}

    memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;	
	opt.op_addr = SYS_CTL_1;
	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**read systerm control register failure!\n");
		return NULL;
	}
    sys_ctl_1 = opt.op_value ;

	if(car_linkup_flag == 0)
	{
		sys_ctl_1 &= 0xfffe;
	}
    else if(car_linkup_flag == 1)
    {
        sys_ctl_1 |= 1;
	}
	else
	{
        sem_syslog_warning("**get bad param car_linkup_flag = %d!\n",car_linkup_flag);
	}
	//sem_syslog_warning("**sys_ctl_1 = 0x%x!\n",sys_ctl_1);
	memset(&opt,0,sizeof(bm_op_args));	
	opt.op_len = 16;	
	opt.op_value = sys_ctl_1;
	opt.op_addr = SYS_CTL_1;
	ret = write_fpga_reg(&opt);

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_INT32,
								 &ret);
	//sem_syslog_dbg("**sem_dbus_config_fpga_sysreg_car_linkup success!\n");
	return reply;
}

DBusMessage *sem_dbus_config_fpga_sysreg_car_linkdown(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int ret = 0;
	
    unsigned short car_linkdown_flag = 0;
	unsigned int sys_ctl_1 = 0;

	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
		                        DBUS_TYPE_UINT16, &car_linkdown_flag,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		sem_syslog_dbg("sem_dbus_config_fpga_sysreg_car_linkup get arg failed and reset normal\n");
	}

    memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;	
	opt.op_addr = SYS_CTL_1;
	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**read systerm control register failure!\n");
		return NULL;
	}
    sys_ctl_1 = opt.op_value ;

	if(car_linkdown_flag == 0)
	{
		sys_ctl_1 &= 0xfffd;
	}
    else if(car_linkdown_flag == 1)
    {
        sys_ctl_1 = sys_ctl_1 | (1<<1);
	}
	else
	{
        sem_syslog_warning("**get bad param car_linkdown_flag = %d!\n",car_linkdown_flag);
	}
	//sem_syslog_warning("**sys_ctl_1 = 0x%x!\n",sys_ctl_1);
	memset(&opt,0,sizeof(bm_op_args));	
	opt.op_len = 16;	
	opt.op_value = sys_ctl_1;
	opt.op_addr = SYS_CTL_1;
	ret = write_fpga_reg(&opt);

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_INT32,
								 &ret);
	//sem_syslog_dbg("**sem_dbus_config_fpga_sysreg_car_linkdown success!\n");
	return reply;
}

//return 1 for valid ,0 for unvalid 
int read_hash_test(unsigned int hash_id)
{
 	int ret;
	int k;
	int start = 0;	
	bm_op_args opt;
	
	unsigned int temp_value1 = 0;
	unsigned int temp_value2 = 0;
	unsigned int value[sizeof(DataTempStorageArea)/sizeof(long)]; /* register value */
	int ready = 0;

	struct timeval read_start, read_end;
    int read_interval;

	struct timeval write_start, write_end;
    int write_interval;

	struct timeval read_start2, read_end2;
    int read_interval2;	
	
    /*test whether the  previous operation is completed */
	memset(&opt,0,sizeof(bm_op_args));
	//gettimeofday(&read_start, NULL);
	opt.op_len = 16;
	opt.op_addr = INDIRECT_ACCESS_STATE_REG;
	while(ready == 0)
    {
		ret = read_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**read fpga indirect access control register failure!\n");
    		ret = -1;
    	}
        ready = opt.op_value & READY_REG_VALUE;           //Check Status of ready
	}

	ready = 0;
	/*end*/

	/* write table id to address register*/	
	memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;
	opt.op_value = hash_id & 0xffff;
	opt.op_addr = INDIRECT_ACCESS_ADDRESS_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write hash table value failure!\n");
		return 2;
	}

    opt.op_value = hash_id >> 16;
	opt.op_addr = INDIRECT_ACCESS_ADDRESS_REG_H;        /* write table id to address register*/
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("##write hash table value failure!\n");
		return 2;
	}	
    /* end */
	
	/* hash table into read operation */
	opt.op_addr = INDIRECT_ACCESS_CONTROL_REG;
	opt.op_value = HASH_TABLE_1_VALUE;     /* read operation-- set bit 15 for 0,hash table select_id=0x1 */
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write fpga indirect access control register failure!\n");
		return 2;
	}

    /*end*/

    /* wait ready register into 1 */
	opt.op_addr = INDIRECT_ACCESS_STATE_REG;
    while(ready == 0)
	{
		ret = read_fpga_reg(&opt);
		if(ret)
		{
			sem_syslog_warning("**read fpga indirect access control register failure!\n");
			return 2;
		}
    	ready = opt.op_value ;    //Read ready Register
        ready = ready & READY_REG_VALUE;           //Check Status of ready
    }
	
	opt.op_addr = Data_Temp_Stor_Base_Addr + DataTempStorageArea[7];
	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**read hash table failure!\n");
		return 2;
	}

	//gettimeofday(&read_end, NULL);
	//read_interval = 1000000*(read_end.tv_sec - read_start.tv_sec) + (read_end.tv_usec - read_start.tv_usec);
	//sem_syslog_warning("**hash test interval = %d !\n",read_interval);

	if(((opt.op_value & 0x80) >> 7) == 1)
	{
        return 1;
	}
    return 0;
}


DBusMessage *sem_dbus_show_fpga_hash_valid(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int slot_id;
    int ready = 0;
	unsigned int hash_id;
	unsigned int i;
	unsigned int hash_id_temp;
	unsigned int count_valid;
	unsigned short Indirect_add_reg_H = 0;
	unsigned short Indirect_add_reg_L = 0;
	unsigned short ret=0;
	int flag;

	unsigned short hash_ctl = 0;
	struct timeval start, end;
    int interval;
	
    sem_syslog_dbg("come to sem_dbus_show_fpga_hash_valid\n");
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args(msg, &err,
		                        DBUS_TYPE_UINT32, &hash_id,
		                        DBUS_TYPE_INT32, &flag,
		                        DBUS_TYPE_UINT32, &count_valid,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		sem_syslog_dbg("sem_dbus_config_fpga_sysreg_car_linkup get arg failed and reset normal\n");
	}
	//sem_syslog_warning("&&&&&&&&&&&&&&&hash_id = %d\n",hash_id);
	if(flag == 1)
	{
        hash_id_temp = (hash_id * 10000)+10000;
	}
	else
	{
        hash_id_temp = (hash_id* 10000)+7152;
	}
	i = hash_id*10000;
	//sem_syslog_warning("&&&&&&&&&&&&&&&i = %d hash_id_temp =%d\n",hash_id,hash_id_temp);
	//gettimeofday(&start, NULL);
	for(;i<hash_id_temp ;i++)
	{
    	ret = read_hash_test(i);
    	if(ret == 2)
    	{
            sem_syslog_warning("read write fail\n");
    	}
    	else if(ret == 1)
    	{
            //sem_syslog_warning("hash_id = %d  valid\n",i);
			count_valid++;
    	}
    	else if(ret == 0)
    	{
    		//normal do nothing
    	}
    	else
    	{
            sem_syslog_warning("bad return!\n");
    	}
	}
    //gettimeofday(&end, NULL);
    //interval = 1000000*(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);
    //sem_syslog_warning("**read hash interval = %d !\n",interval);
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);//58 parames

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &count_valid);	
	return reply;
}

DBusMessage *sem_dbus_show_fpga_car_valid(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int slot_id;
    int ready = 0;
	unsigned int car_id;
	unsigned int i;
	unsigned int car_id_temp;
	unsigned int count_valid;
	unsigned short Indirect_add_reg_H = 0;
	unsigned short Indirect_add_reg_L = 0;
	unsigned short ret_car=0;
	int ret=0;
	int flag;

	unsigned short hash_ctl = 0;
	//struct timeval start, end;
    //int interval;
    sem_syslog_dbg("come to sem_dbus_show_fpga_car_valid\n");
	read_car_result_t *read_car_result = NULL;
	read_car_result = (read_car_result_t *)malloc(sizeof(read_car_result_t));

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args(msg, &err,
		                        DBUS_TYPE_UINT32, &car_id,
		                        DBUS_TYPE_INT32, &flag,
		                        DBUS_TYPE_UINT32, &count_valid,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err))
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		sem_syslog_dbg("sem_dbus_show_fpga_car_valid get arg failed and reset normal\n");
	}
	//sem_syslog_warning("&&&&&&&&&&&&&&&car_id = %d\n",car_id);
	if(flag == 1)
	{
        car_id_temp = (car_id * 10000)+10000;
	}
	else
	{
        car_id_temp = (car_id* 10000)+31072;
	}
	i = car_id*10000;
	//sem_syslog_warning("&&&&&&&&&&&&&&&i = %d car_id_temp =%d\n",car_id,car_id_temp);
	//gettimeofday(&start, NULL);
	for(;i<car_id_temp ;i++)
	{
		memset(read_car_result,0,sizeof(read_car_result_t));
    	ret_car = read_car(read_car_result,0,0,0,i,1);
    	if(!ret_car)
    	{
            sem_syslog_warning("read car table fail! car_id = %d\n",i);
    	}
    	else
    	{
            if(read_car_result->car_valid)
            {
                count_valid++;
			}
    	}
	}
	free(read_car_result);
	read_car_result = NULL;
    //gettimeofday(&end, NULL);
    //interval = 1000000*(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);
    //sem_syslog_warning("**read hash interval = %d !\n",interval);
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);//58 parames

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &count_valid);
	return reply;
}

//return 1 for not equal,0 for equal
int write_hash_equal_read_hash(	write_cam_param_t *write_cam_param,hash_table_t *stat)
{
    if(stat->sip_H != write_cam_param->SIP_H)
    {
        sem_syslog_warning("**stat->sip_H = %x\n",stat->sip_H);
		sem_syslog_warning("**write_cam_param->SIP_H = %x\n",write_cam_param->SIP_H);
		return 1;
	}
    if(stat->sip_L != write_cam_param->SIP_L)
    {
        sem_syslog_warning("**stat->sip_L = %x\n",stat->sip_L);
		sem_syslog_warning("**write_cam_param->SIP_L = %x\n",write_cam_param->SIP_L);
		return 1;
	}
	if(stat->dip_H != write_cam_param->DIP_H)
    {
        sem_syslog_warning("**stat->dip_H = %x\n",stat->dip_H);
		sem_syslog_warning("**write_cam_param->DIP_H = %x\n",write_cam_param->DIP_H);
		return 1;
	}
	if(stat->dip_L != write_cam_param->DIP_L)
    {
        sem_syslog_warning("**stat->dip_L = %x\n",stat->dip_L);
		sem_syslog_warning("**write_cam_param->DIP_L = %x\n",write_cam_param->DIP_L);
		return 1;
	}
	if(stat->sport != write_cam_param->SPORT)
    {
        sem_syslog_warning("**stat->sport = %x\n",stat->sport);
		sem_syslog_warning("**write_cam_param->SPORT = %x\n",write_cam_param->SPORT);
		return 1;
	}
	if(stat->dport != write_cam_param->DPORT)
    {
        sem_syslog_warning("**stat->dport = %x\n",stat->dport);
		sem_syslog_warning("**write_cam_param->DPORT = %x\n",write_cam_param->DPORT);
		return 1;
	}
	if(stat->protocol != write_cam_param->PROTOCOL)
    {
        sem_syslog_warning("**stat->protocol = %x\n",stat->protocol);
		sem_syslog_warning("**write_cam_param->PROTOCOL = %x\n",write_cam_param->PROTOCOL);
		return 1;
	}
	if(stat->hash_ctl != write_cam_param->HASH_CTL)
    {
        sem_syslog_warning("**stat->hash_ctl = %x\n",stat->hash_ctl);
		sem_syslog_warning("**write_cam_param->HASH_CTL = %x\n",write_cam_param->HASH_CTL);
		return 1;
	}
	if(stat->dmac_H!= write_cam_param->DMAC_H)
    {
        sem_syslog_warning("**stat->dmac_H = %x\n",stat->dmac_H);
		sem_syslog_warning("**write_cam_param->DMAC_H = %x\n",write_cam_param->DMAC_H);
		return 1;
	}
	if(stat->dmac_M != write_cam_param->DMAC_M)
    {
        sem_syslog_warning("**stat->dmac_M = %x\n",stat->dmac_M);
		sem_syslog_warning("**write_cam_param->DMAC_M = %x\n",write_cam_param->DMAC_M);
		return 1;
	}
	if(stat->dmac_L != write_cam_param->DMAC_L)
    {
        sem_syslog_warning("**stat->dmac_L = %x\n",stat->dmac_L);
		sem_syslog_warning("**write_cam_param->DMAC_L = %x\n",write_cam_param->DMAC_L);
		return 1;
	}
	if(stat->smac_L != write_cam_param->SMAC_L)
    {
        sem_syslog_warning("**stat->smac_L = %x\n",stat->smac_L);
		sem_syslog_warning("**write_cam_param->SMAC_L = %x\n",write_cam_param->SMAC_L);
		return 1;
	}
	if(stat->smac_H!= write_cam_param->SMAC_H)
    {
        sem_syslog_warning("**stat->smac_H = %x\n",stat->smac_H);
		sem_syslog_warning("**write_cam_param->SMAC_H = %x\n",write_cam_param->SMAC_H);
		return 1;
	}
	if(stat->smac_M != write_cam_param->SMAC_M)
    {
        sem_syslog_warning("**stat->smac_M = %x\n",stat->smac_M);
		sem_syslog_warning("**write_cam_param->SMAC_M = %x\n",write_cam_param->SMAC_M);
		return 1;
	}
	if(stat->vlan_out_H != write_cam_param->VLAN_OUT_H)
    {
        sem_syslog_warning("**stat->vlan_out_H = %x\n",stat->vlan_out_H);
		sem_syslog_warning("**write_cam_param->VLAN_OUT_H = %x\n",write_cam_param->VLAN_OUT_H);
		return 1;
	}
	if(stat->vlan_out_L != write_cam_param->VLAN_OUT_L)
    {
        sem_syslog_warning("**stat->vlan_out_L = %x\n",stat->vlan_out_L);
		sem_syslog_warning("**write_cam_param->VLAN_OUT_L = %x\n",write_cam_param->VLAN_OUT_L);
		return 1;
	}
	if(stat->vlan_in_H != write_cam_param->VLAN_IN_H)
    {
        sem_syslog_warning("**stat->vlan_in_H = %x\n",stat->vlan_in_H);
		sem_syslog_warning("**write_cam_param->VLAN_IN_H = %x\n",write_cam_param->VLAN_IN_H);
		return 1;
	}
	if(stat->ethernet_protocol != write_cam_param->FIRST_0800)
    {
        sem_syslog_warning("**stat->ethernet_protocol = %x\n",stat->ethernet_protocol);
		sem_syslog_warning("**write_cam_param->0800_FIRST = %x\n",write_cam_param->FIRST_0800);
		return 1;
	}
	if(stat->ip_tos!= write_cam_param->IPTOS)
    {
        sem_syslog_warning("**stat->ip_tos = %x\n",stat->ip_tos);
		sem_syslog_warning("**write_cam_param->IPTOS = %x\n",write_cam_param->IPTOS);
		return 1;
	}
	if(stat->ip_len!= write_cam_param->IP_LEN)
    {
        sem_syslog_warning("**stat->ip_len = %x\n",stat->ip_len);
		sem_syslog_warning("**write_cam_param->IP_LEN = %x\n",write_cam_param->IP_LEN);
		return 1;
	}
	if(stat->ip_identification!= write_cam_param->IP_ID)
    {
        sem_syslog_warning("**stat->ip_identification = %x\n",stat->ip_identification);
		sem_syslog_warning("**write_cam_param->IP_ID = %x\n",write_cam_param->IP_ID);
		return 1;
	}
	if(stat->ip_offset!= write_cam_param->IP_OFFSET)
    {
        sem_syslog_warning("**stat->ip_offset = %x\n",stat->ip_offset);
		sem_syslog_warning("**write_cam_param->IP_OFFSET = %x\n",write_cam_param->IP_OFFSET);
		return 1;
	}
	if(stat->ip_ttl!= (write_cam_param->IP_TTL_IP_PROTOCOL & 0xff))
    {
        sem_syslog_warning("**stat->ip_ttl = %x\n",stat->ip_ttl);
		sem_syslog_warning("**write_cam_param->IP_TTL_IP_PROTOCOL = %x\n",write_cam_param->IP_TTL_IP_PROTOCOL);
		return 1;
	}
	if(stat->ip_ttl!= (write_cam_param->IP_TTL_IP_PROTOCOL >> 8))
    {
        sem_syslog_warning("**stat->ip_ttl = %x\n",stat->ip_ttl);
		sem_syslog_warning("**write_cam_param->IP_TTL_IP_PROTOCOL = %x\n",write_cam_param->IP_TTL_IP_PROTOCOL);
		return 1;
	}
	if(stat->ip_checksum!= write_cam_param->IP_CHECKSUM)
    {
        sem_syslog_warning("**stat->ip_checksum = %x\n",stat->ip_checksum);
		sem_syslog_warning("**write_cam_param->IP_CHECKSUM = %x\n",write_cam_param->IP_CHECKSUM);
		return 1;
	}
	if(stat->tunnel_sip_H != write_cam_param->TUNNEL_SIP_H)
    {
        sem_syslog_warning("**stat->tunnel_sip_H = %x\n",stat->tunnel_sip_H);
		sem_syslog_warning("**write_cam_param->TUNNEL_SIP_H = %x\n",write_cam_param->TUNNEL_SIP_H);
		return 1;
	}
	if(stat->tunnel_sip_L != write_cam_param->TUNNEL_SIP_L)
    {
        sem_syslog_warning("**stat->tunnel_sip_L = %x\n",stat->tunnel_sip_L);
		sem_syslog_warning("**write_cam_param->TUNNEL_SIP_L = %x\n",write_cam_param->TUNNEL_SIP_L);
		return 1;
	}
	if(stat->tunnel_dip_H != write_cam_param->TUNNEL_DIP_H)
    {
        sem_syslog_warning("**stat->tunnel_dip_H = %x\n",stat->tunnel_dip_H);
		sem_syslog_warning("**write_cam_param->TUNNEL_DIP_H = %x\n",write_cam_param->TUNNEL_DIP_H);
		return 1;
	}
	if(stat->tunnel_dip_L != write_cam_param->TUNNEL_DIP_L)
    {
        sem_syslog_warning("**stat->tunnel_dip_L = %x\n",stat->tunnel_dip_L);
		sem_syslog_warning("**write_cam_param->TUNNEL_DIP_L = %x\n",write_cam_param->TUNNEL_DIP_L);
		return 1;
	}
	if(stat->sport != write_cam_param->SPORT)
    {
        sem_syslog_warning("**stat->sport = %x\n",stat->sport);
		sem_syslog_warning("**write_cam_param->SPORT = %x\n",write_cam_param->SPORT);
		return 1;
	}
	if(stat->dport!= write_cam_param->DPORT)
    {
        sem_syslog_warning("**stat->dport = %x\n",stat->dport);
		sem_syslog_warning("**write_cam_param->DPORT = %x\n",write_cam_param->DPORT);
		return 1;
	}
	if(stat->udp_len!= write_cam_param->UDP_LEN)
    {
        sem_syslog_warning("**stat->udp_len = %x\n",stat->udp_len);
		sem_syslog_warning("**write_cam_param->UDP_LEN = %x\n",write_cam_param->UDP_LEN);
		return 1;
	}
	if(stat->udp_checksum!= write_cam_param->UDP_CHECKSUM)
    {
        sem_syslog_warning("**stat->udp_checksum = %x\n",stat->udp_checksum);
		sem_syslog_warning("**write_cam_param->UDP_CHECKSUM = %x\n",write_cam_param->UDP_CHECKSUM);
		return 1;
	}
	if(stat->capwap_header_b0!= write_cam_param->CAPWAP_B0B1)
    {
        sem_syslog_warning("**stat->capwap_header_b0 = %x\n",stat->capwap_header_b0);
		sem_syslog_warning("**write_cam_param->CAPWAP_B0B1 = %x\n",write_cam_param->CAPWAP_B0B1);
		return 1;
	}
	if(stat->capwap_header_b2!= write_cam_param->CAPWAP_B2B3)
    {
        sem_syslog_warning("**stat->capwap_header_b2 = %x\n",stat->capwap_header_b2);
		sem_syslog_warning("**write_cam_param->CAPWAP_B2B3 = %x\n",write_cam_param->CAPWAP_B2B3);
		return 1;
	}
	if(stat->capwap_header_b4!= write_cam_param->CAPWAP_B4B5)
    {
        sem_syslog_warning("**stat->capwap_header_b4 = %x\n",stat->capwap_header_b4);
		sem_syslog_warning("**write_cam_param->CAPWAP_B4B5 = %x\n",write_cam_param->CAPWAP_B4B5);
		return 1;
	}
	if(stat->capwap_header_b6!= write_cam_param->CAPWAP_B6B7)
    {
        sem_syslog_warning("**stat->capwap_header_b6 = %x\n",stat->capwap_header_b6);
		sem_syslog_warning("**write_cam_param->CAPWAP_B6B7 = %x\n",write_cam_param->CAPWAP_B6B7);
		return 1;
	}
	if(stat->capwap_header_b8!= write_cam_param->CAPWAP_B8B9)
    {
        sem_syslog_warning("**stat->capwap_header_b8 = %x\n",stat->capwap_header_b8);
		sem_syslog_warning("**write_cam_param->CAPWAP_B8B9 = %x\n",write_cam_param->CAPWAP_B8B9);
		return 1;
	}
	if(stat->capwap_header_b10!= write_cam_param->CAPWAP_B10B11)
    {
        sem_syslog_warning("**stat->capwap_header_b10 = %x\n",stat->capwap_header_b10);
		sem_syslog_warning("**write_cam_param->CAPWAP_B10B11 = %x\n",write_cam_param->CAPWAP_B10B11);
		return 1;
	}
	if(stat->capwap_header_b12!= write_cam_param->CAPWAP_B12B13)
    {
        sem_syslog_warning("**stat->capwap_header_b12 = %x\n",stat->capwap_header_b12);
		sem_syslog_warning("**write_cam_param->CAPWAP_B12B13 = %x\n",write_cam_param->CAPWAP_B12B13);
		return 1;
	}
	if(stat->capwap_header_b14!= write_cam_param->CAPWAP_B14B15)
    {
        sem_syslog_warning("**stat->capwap_header_b14 = %x\n",stat->capwap_header_b14);
		sem_syslog_warning("**write_cam_param->CAPWAP_B14B15 = %x\n",write_cam_param->CAPWAP_B14B15);
		return 1;
	}
	if(stat->header_802_11_b0!= write_cam_param->H_802_11_B0B1)
    {
        sem_syslog_warning("**stat->header_802_11_b0 = %x\n",stat->header_802_11_b0);
		sem_syslog_warning("**write_cam_param->H_802_11_B0B1 = %x\n",write_cam_param->H_802_11_B0B1);
		return 1;
	}
	if(stat->header_802_11_b2!= write_cam_param->H_802_11_B2B3)
    {
        sem_syslog_warning("**stat->header_802_11_b2 = %x\n",stat->header_802_11_b2);
		sem_syslog_warning("**write_cam_param->H_802_11_B2B3 = %x\n",write_cam_param->H_802_11_B2B3);
		return 1;
	}
	if(stat->header_802_11_b4!= write_cam_param->H_802_11_B4B5)
    {
        sem_syslog_warning("**stat->header_802_11_b4 = %x\n",stat->header_802_11_b4);
		sem_syslog_warning("**write_cam_param->H_802_11_B4B5 = %x\n",write_cam_param->H_802_11_B4B5);
		return 1;
	}
	if(stat->header_802_11_b6!= write_cam_param->H_802_11_B6B7)
    {
        sem_syslog_warning("**stat->header_802_11_b6 = %x\n",stat->header_802_11_b6);
		sem_syslog_warning("**write_cam_param->H_802_11_B6B7 = %x\n",write_cam_param->H_802_11_B6B7);
		return 1;
	}
	if(stat->header_802_11_b8!= write_cam_param->H_802_11_B8B9)
    {
        sem_syslog_warning("**stat->header_802_11_b8 = %x\n",stat->header_802_11_b8);
		sem_syslog_warning("**write_cam_param->H_802_11_B8B9 = %x\n",write_cam_param->H_802_11_B8B9);
		return 1;
	}
	if(stat->header_802_11_b10!= write_cam_param->H_802_11_B10B11)
    {
        sem_syslog_warning("**stat->header_802_11_b10 = %x\n",stat->header_802_11_b10);
		sem_syslog_warning("**write_cam_param->H_802_11_B10B11 = %x\n",write_cam_param->H_802_11_B10B11);
		return 1;
	}
	if(stat->header_802_11_b12!= write_cam_param->H_802_11_B12B13)
    {
        sem_syslog_warning("**stat->header_802_11_b12 = %x\n",stat->header_802_11_b12);
		sem_syslog_warning("**write_cam_param->H_802_11_B12B13 = %x\n",write_cam_param->H_802_11_B12B13);
		return 1;
	}
	if(stat->header_802_11_b14!= write_cam_param->H_802_11_B14B15)
    {
        sem_syslog_warning("**stat->header_802_11_b14 = %x\n",stat->header_802_11_b14);
		sem_syslog_warning("**write_cam_param->H_802_11_B14B15 = %x\n",write_cam_param->H_802_11_B14B15);
		return 1;
	}
	if(stat->header_802_11_b16!= write_cam_param->H_802_11_B16B17)
    {
        sem_syslog_warning("**stat->header_802_11_b16 = %x\n",stat->header_802_11_b16);
		sem_syslog_warning("**write_cam_param->H_802_11_B16B17 = %x\n",write_cam_param->H_802_11_B16B17);
		return 1;
	}
	if(stat->header_802_11_b18!= write_cam_param->H_802_11_B18B19)
    {
        sem_syslog_warning("**stat->header_802_11_b18 = %x\n",stat->header_802_11_b18);
		sem_syslog_warning("**write_cam_param->H_802_11_B18B19 = %x\n",write_cam_param->H_802_11_B18B19);
		return 1;
	}
	if(stat->header_802_11_b20!= write_cam_param->H_802_11_B20B21)
    {
        sem_syslog_warning("**stat->header_802_11_b20 = %x\n",stat->header_802_11_b20);
		sem_syslog_warning("**write_cam_param->H_802_11_B20B21 = %x\n",write_cam_param->H_802_11_B20B21);
		return 1;
	}
	if(stat->header_802_11_b22!= write_cam_param->H_802_11_B22B23)
    {
        sem_syslog_warning("**stat->header_802_11_b22 = %x\n",stat->header_802_11_b22);
		sem_syslog_warning("**write_cam_param->H_802_11_B22B23 = %x\n",write_cam_param->H_802_11_B22B23);
		return 1;
	}
	if(stat->header_802_11_b24!= write_cam_param->H_802_11_B24B25)
    {
        sem_syslog_warning("**stat->header_802_11_b24 = %x\n",stat->header_802_11_b24);
		sem_syslog_warning("**write_cam_param->H_802_11_B24B25 = %x\n",write_cam_param->H_802_11_B24B25);
		return 1;
	}
	if(stat->header_802_11_b26!= write_cam_param->H_802_11_B26B27)
    {
        sem_syslog_warning("**stat->header_802_11_b26 = %x\n",stat->header_802_11_b26);
		sem_syslog_warning("**write_cam_param->H_802_11_B26B27 = %x\n",write_cam_param->H_802_11_B26B27);
		return 1;
	}
	if(stat->capwap_protocol!= write_cam_param->SEC_0800)
    {
        sem_syslog_warning("**stat->capwap_protocol = %x\n",stat->capwap_protocol);
		sem_syslog_warning("**write_cam_param->SEC_0800 = %x\n",write_cam_param->SEC_0800);
		return 1;
	}
	return 0;
}

//return 1 for equal 0 for not equal
int read_hash_not_equal(hash_table_t *stat_base,hash_table_t *stat)
{
    if(stat_base->sip_H == stat->sip_H)
    {
        sem_syslog_warning("**stat->sip_H = %x\n",stat->sip_H);
		sem_syslog_warning("**stat_base->sip_H = %x\n",stat_base->sip_H);
		return 1;
	}
    if(stat_base->sip_L == stat->sip_L)
    {
        sem_syslog_warning("**stat->sip_L = %x\n",stat->sip_L);
		sem_syslog_warning("**stat_base->sip_L = %x\n",stat_base->sip_L);
		return 1;
	}
	if(stat_base->dip_H == stat->dip_H)
    {
        sem_syslog_warning("**stat->dip_H = %x\n",stat->dip_H);
		sem_syslog_warning("**stat_base->dip_H = %x\n",stat_base->dip_H);
		return 1;
	}
	if(stat_base->dip_L == stat->dip_L)
    {
        sem_syslog_warning("**stat->dip_L = %x\n",stat->dip_L);
		sem_syslog_warning("**stat_base->dip_L = %x\n",stat_base->dip_L);
		return 1;
	}
/*
	if(stat->sport == stat->sport)
    {
        sem_syslog_warning("**stat->sport = %x\n",stat->sport);
		sem_syslog_warning("**write_cam_param->SPORT = %x\n",write_cam_param->SPORT);
		return 1;
	}
	if(stat->dport == stat->dport)
    {
        sem_syslog_warning("**stat->dport = %x\n",stat->dport);
		sem_syslog_warning("**write_cam_param->DPORT = %x\n",write_cam_param->DPORT);
		return 1;
	}
	if(stat->protocol == stat->protocol)
    {
        sem_syslog_warning("**stat->protocol = %x\n",stat->protocol);
		sem_syslog_warning("**write_cam_param->PROTOCOL = %x\n",write_cam_param->PROTOCOL);
		return 1;
	}
	if(stat->hash_ctl == stat->hash_ctl)
    {
        sem_syslog_warning("**stat->hash_ctl = %x\n",stat->hash_ctl);
		sem_syslog_warning("**write_cam_param->HASH_CTL = %x\n",write_cam_param->HASH_CTL);
		return 1;
	}
	if(stat->dmac_H== stat->dmac_H)
    {
        sem_syslog_warning("**stat->dmac_H = %x\n",stat->dmac_H);
		sem_syslog_warning("**write_cam_param->DMAC_H = %x\n",write_cam_param->DMAC_H);
		return 1;
	}
	if(stat->dmac_M == stat->dmac_M)
    {
        sem_syslog_warning("**stat->dmac_M = %x\n",stat->dmac_M);
		sem_syslog_warning("**write_cam_param->DMAC_M = %x\n",write_cam_param->DMAC_M);
		return 1;
	}
	if(stat->dmac_L == stat->dmac_L)
    {
        sem_syslog_warning("**stat->dmac_L = %x\n",stat->dmac_L);
		sem_syslog_warning("**write_cam_param->DMAC_L = %x\n",write_cam_param->DMAC_L);
		return 1;
	}
	if(stat->smac_L == stat->smac_L)
    {
        sem_syslog_warning("**stat->smac_L = %x\n",stat->smac_L);
		sem_syslog_warning("**write_cam_param->SMAC_L = %x\n",write_cam_param->SMAC_L);
		return 1;
	}
	if(stat->smac_H== stat->smac_H)
    {
        sem_syslog_warning("**stat->smac_H = %x\n",stat->smac_H);
		sem_syslog_warning("**write_cam_param->SMAC_H = %x\n",write_cam_param->SMAC_H);
		return 1;
	}
	if(stat->smac_M == stat->smac_M)
    {
        sem_syslog_warning("**stat->smac_M = %x\n",stat->smac_M);
		sem_syslog_warning("**write_cam_param->SMAC_M = %x\n",write_cam_param->SMAC_M);
		return 1;
	}
	if(stat->vlan_out_H == stat->vlan_out_H)
    {
        sem_syslog_warning("**stat->vlan_out_H = %x\n",stat->vlan_out_H);
		sem_syslog_warning("**write_cam_param->VLAN_OUT_H = %x\n",write_cam_param->VLAN_OUT_H);
		return 1;
	}
	if(stat->vlan_out_L == stat->vlan_out_L)
    {
        sem_syslog_warning("**stat->vlan_out_L = %x\n",stat->vlan_out_L);
		sem_syslog_warning("**write_cam_param->VLAN_OUT_L = %x\n",write_cam_param->VLAN_OUT_L);
		return 1;
	}
	if(stat->vlan_in_H == stat->vlan_in_H)
    {
        sem_syslog_warning("**stat->vlan_in_H = %x\n",stat->vlan_in_H);
		sem_syslog_warning("**write_cam_param->VLAN_IN_H = %x\n",write_cam_param->VLAN_IN_H);
		return 1;
	}
	if(stat->ethernet_protocol == stat->ethernet_protocol)
    {
        sem_syslog_warning("**stat->ethernet_protocol = %x\n",stat->ethernet_protocol);
		sem_syslog_warning("**write_cam_param->FIRST_0800 = %x\n",write_cam_param->FIRST_0800);
		return 1;
	}
	if(stat->ip_tos== stat->ip_tos)
    {
        sem_syslog_warning("**stat->ip_tos = %x\n",stat->ip_tos);
		sem_syslog_warning("**write_cam_param->IPTOS = %x\n",write_cam_param->IPTOS);
		return 1;
	}
	if(stat->ip_len== stat->ip_len)
    {
        sem_syslog_warning("**stat->ip_len = %x\n",stat->ip_len);
		sem_syslog_warning("**write_cam_param->IP_LEN = %x\n",write_cam_param->IP_LEN);
		return 1;
	}
	if(stat->ip_identification== stat->ip_identification)
    {
        sem_syslog_warning("**stat->ip_identification = %x\n",stat->ip_identification);
		sem_syslog_warning("**write_cam_param->IP_ID = %x\n",write_cam_param->IP_ID);
		return 1;
	}
	if(stat->ip_offset== stat->ip_offset)
    {
        sem_syslog_warning("**stat->ip_offset = %x\n",stat->ip_offset);
		sem_syslog_warning("**write_cam_param->IP_OFFSET = %x\n",write_cam_param->IP_OFFSET);
		return 1;
	}
	if(stat->ip_ttl== stat->ip_ttl)
    {
        sem_syslog_warning("**stat->ip_ttl = %x\n",stat->ip_ttl);
		sem_syslog_warning("**write_cam_param->IP_TTL_IP_PROTOCOL = %x\n",write_cam_param->IP_TTL_IP_PROTOCOL);
		return 1;
	}
	if(stat->ip_checksum== stat->ip_checksum)
    {
        sem_syslog_warning("**stat->ip_checksum = %x\n",stat->ip_checksum);
		sem_syslog_warning("**write_cam_param->IP_CHECKSUM = %x\n",write_cam_param->IP_CHECKSUM);
		return 1;
	}
	if(stat->tunnel_sip_H == stat->tunnel_sip_H)
    {
        sem_syslog_warning("**stat->tunnel_sip_H = %x\n",stat->tunnel_sip_H);
		sem_syslog_warning("**write_cam_param->TUNNEL_SIP_H = %x\n",write_cam_param->TUNNEL_SIP_H);
		return 1;
	}
	if(stat->tunnel_sip_L == stat->tunnel_sip_L)
    {
        sem_syslog_warning("**stat->tunnel_sip_L = %x\n",stat->tunnel_sip_L);
		sem_syslog_warning("**write_cam_param->TUNNEL_SIP_L = %x\n",write_cam_param->TUNNEL_SIP_L);
		return 1;
	}
	if(stat->tunnel_dip_H == stat->tunnel_dip_H)
    {
        sem_syslog_warning("**stat->tunnel_dip_H = %x\n",stat->tunnel_dip_H);
		sem_syslog_warning("**write_cam_param->TUNNEL_DIP_H = %x\n",write_cam_param->TUNNEL_DIP_H);
		return 1;
	}
	if(stat->tunnel_dip_L == stat->tunnel_dip_L)
    {
        sem_syslog_warning("**stat->tunnel_dip_L = %x\n",stat->tunnel_dip_L);
		sem_syslog_warning("**write_cam_param->TUNNEL_DIP_L = %x\n",write_cam_param->TUNNEL_DIP_L);
		return 1;
	}
	if(stat->sport == stat->sport)
    {
        sem_syslog_warning("**stat->sport = %x\n",stat->sport);
		sem_syslog_warning("**write_cam_param->SPORT = %x\n",write_cam_param->SPORT);
		return 1;
	}
	if(stat->dport== stat->dport)
    {
        sem_syslog_warning("**stat->dport = %x\n",stat->dport);
		sem_syslog_warning("**write_cam_param->DPORT = %x\n",write_cam_param->DPORT);
		return 1;
	}
	if(stat->udp_len== stat->udp_len)
    {
        sem_syslog_warning("**stat->udp_len = %x\n",stat->udp_len);
		sem_syslog_warning("**write_cam_param->UDP_LEN = %x\n",write_cam_param->UDP_LEN);
		return 1;
	}
	if(stat->udp_checksum== stat->udp_checksum)
    {
        sem_syslog_warning("**stat->udp_checksum = %x\n",stat->udp_checksum);
		sem_syslog_warning("**write_cam_param->UDP_CHECKSUM = %x\n",write_cam_param->UDP_CHECKSUM);
		return 1;
	}
	if(stat->capwap_header_b0== stat->capwap_header_b0)
    {
        sem_syslog_warning("**stat->capwap_header_b0 = %x\n",stat->capwap_header_b0);
		sem_syslog_warning("**write_cam_param->CAPWAP_B0B1 = %x\n",write_cam_param->CAPWAP_B0B1);
		return 1;
	}
	if(stat->capwap_header_b2== stat->capwap_header_b2)
    {
        sem_syslog_warning("**stat->capwap_header_b2 = %x\n",stat->capwap_header_b2);
		sem_syslog_warning("**write_cam_param->CAPWAP_B2B3 = %x\n",write_cam_param->CAPWAP_B2B3);
		return 1;
	}
	if(stat->capwap_header_b4== stat->capwap_header_b4)
    {
        sem_syslog_warning("**stat->capwap_header_b4 = %x\n",stat->capwap_header_b4);
		sem_syslog_warning("**write_cam_param->CAPWAP_B4B5 = %x\n",write_cam_param->CAPWAP_B4B5);
		return 1;
	}
	if(stat->capwap_header_b6== stat->capwap_header_b6)
    {
        sem_syslog_warning("**stat->capwap_header_b6 = %x\n",stat->capwap_header_b6);
		sem_syslog_warning("**write_cam_param->CAPWAP_B6B7 = %x\n",write_cam_param->CAPWAP_B6B7);
		return 1;
	}
	if(stat->capwap_header_b8== stat->capwap_header_b8)
    {
        sem_syslog_warning("**stat->capwap_header_b8 = %x\n",stat->capwap_header_b8);
		sem_syslog_warning("**write_cam_param->CAPWAP_B8B9 = %x\n",write_cam_param->CAPWAP_B8B9);
		return 1;
	}
	if(stat->capwap_header_b10== stat->capwap_header_b10)
    {
        sem_syslog_warning("**stat->capwap_header_b10 = %x\n",stat->capwap_header_b10);
		sem_syslog_warning("**write_cam_param->CAPWAP_B10B11 = %x\n",write_cam_param->CAPWAP_B10B11);
		return 1;
	}
	if(stat->capwap_header_b12== stat->capwap_header_b12)
    {
        sem_syslog_warning("**stat->capwap_header_b12 = %x\n",stat->capwap_header_b12);
		sem_syslog_warning("**write_cam_param->CAPWAP_B12B13 = %x\n",write_cam_param->CAPWAP_B12B13);
		return 1;
	}
	if(stat->capwap_header_b14== stat->capwap_header_b14)
    {
        sem_syslog_warning("**stat->capwap_header_b14 = %x\n",stat->capwap_header_b14);
		sem_syslog_warning("**write_cam_param->CAPWAP_B14B15 = %x\n",write_cam_param->CAPWAP_B14B15);
		return 1;
	}
	if(stat->header_802_11_b0== stat->header_802_11_b0)
    {
        sem_syslog_warning("**stat->header_802_11_b0 = %x\n",stat->header_802_11_b0);
		sem_syslog_warning("**write_cam_param->H_802_11_B0B1 = %x\n",write_cam_param->H_802_11_B0B1);
		return 1;
	}
	if(stat->header_802_11_b2== stat->header_802_11_b2)
    {
        sem_syslog_warning("**stat->header_802_11_b2 = %x\n",stat->header_802_11_b2);
		sem_syslog_warning("**write_cam_param->H_802_11_B2B3 = %x\n",write_cam_param->H_802_11_B2B3);
		return 1;
	}
	if(stat->header_802_11_b4== stat->header_802_11_b4)
    {
        sem_syslog_warning("**stat->header_802_11_b4 = %x\n",stat->header_802_11_b4);
		sem_syslog_warning("**write_cam_param->H_802_11_B4B5 = %x\n",write_cam_param->H_802_11_B4B5);
		return 1;
	}
	if(stat->header_802_11_b6== stat->header_802_11_b6)
    {
        sem_syslog_warning("**stat->header_802_11_b6 = %x\n",stat->header_802_11_b6);
		sem_syslog_warning("**write_cam_param->H_802_11_B6B7 = %x\n",write_cam_param->H_802_11_B6B7);
		return 1;
	}
	if(stat->header_802_11_b8== stat->header_802_11_b8)
    {
        sem_syslog_warning("**stat->header_802_11_b8 = %x\n",stat->header_802_11_b8);
		sem_syslog_warning("**write_cam_param->H_802_11_B8B9 = %x\n",write_cam_param->H_802_11_B8B9);
		return 1;
	}
	if(stat->header_802_11_b10== stat->header_802_11_b10)
    {
        sem_syslog_warning("**stat->header_802_11_b10 = %x\n",stat->header_802_11_b10);
		sem_syslog_warning("**write_cam_param->H_802_11_B10B11 = %x\n",write_cam_param->H_802_11_B10B11);
		return 1;
	}
	if(stat->header_802_11_b12== stat->header_802_11_b12)
    {
        sem_syslog_warning("**stat->header_802_11_b12 = %x\n",stat->header_802_11_b12);
		sem_syslog_warning("**write_cam_param->H_802_11_B12B13 = %x\n",write_cam_param->H_802_11_B12B13);
		return 1;
	}
	if(stat->header_802_11_b14== stat->header_802_11_b14)
    {
        sem_syslog_warning("**stat->header_802_11_b14 = %x\n",stat->header_802_11_b14);
		sem_syslog_warning("**write_cam_param->H_802_11_B14B15 = %x\n",write_cam_param->H_802_11_B14B15);
		return 1;
	}
	if(stat->header_802_11_b16== stat->header_802_11_b16)
    {
        sem_syslog_warning("**stat->header_802_11_b16 = %x\n",stat->header_802_11_b16);
		sem_syslog_warning("**write_cam_param->H_802_11_B16B17 = %x\n",write_cam_param->H_802_11_B16B17);
		return 1;
	}
	if(stat->header_802_11_b18== stat->header_802_11_b18)
    {
        sem_syslog_warning("**stat->header_802_11_b18 = %x\n",stat->header_802_11_b18);
		sem_syslog_warning("**write_cam_param->H_802_11_B18B19 = %x\n",write_cam_param->H_802_11_B18B19);
		return 1;
	}
	if(stat->header_802_11_b20== stat->header_802_11_b20)
    {
        sem_syslog_warning("**stat->header_802_11_b20 = %x\n",stat->header_802_11_b20);
		sem_syslog_warning("**write_cam_param->H_802_11_B20B21 = %x\n",write_cam_param->H_802_11_B20B21);
		return 1;
	}
	if(stat->header_802_11_b22== stat->header_802_11_b22)
    {
        sem_syslog_warning("**stat->header_802_11_b22 = %x\n",stat->header_802_11_b22);
		sem_syslog_warning("**write_cam_param->H_802_11_B22B23 = %x\n",write_cam_param->H_802_11_B22B23);
		return 1;
	}
	if(stat->header_802_11_b24== stat->header_802_11_b24)
    {
        sem_syslog_warning("**stat->header_802_11_b24 = %x\n",stat->header_802_11_b24);
		sem_syslog_warning("**write_cam_param->H_802_11_B24B25 = %x\n",write_cam_param->H_802_11_B24B25);
		return 1;
	}
	if(stat->header_802_11_b26== stat->header_802_11_b26)
    {
        sem_syslog_warning("**stat->header_802_11_b26 = %x\n",stat->header_802_11_b26);
		sem_syslog_warning("**write_cam_param->H_802_11_B26B27 = %x\n",write_cam_param->H_802_11_B26B27);
		return 1;
	}
	if(stat->capwap_protocol== stat->capwap_protocol)
    {
        sem_syslog_warning("**stat->capwap_protocol = %x\n",stat->capwap_protocol);
		sem_syslog_warning("**write_cam_param->SEC_0800 = %x\n",write_cam_param->SEC_0800);
		return 1;
	}
	*/
	return 0;
}

unsigned int test_hash_DDR_date_line(int flag)
{
	int ret = 0;
	unsigned int hash_id;
	unsigned int flag_hash = 0;

	unsigned int hash_table_id = 0;
	write_cam_param_t *write_cam_param =NULL;
	hash_table_t *stat =NULL;

    stat = (hash_table_t *)malloc(sizeof(hash_table_t));
	write_cam_param = (write_cam_param_t *)malloc(sizeof(write_cam_param_t));

/*test DDR1 data line*/
//test hash hash_id = 0x0;
    hash_id = 0;
    memset(write_cam_param,0,sizeof(write_cam_param_t));
	
    ret = write_hash(write_cam_param,flag);
	
    ret = read_hash(stat,hash_id,flag);
	
    if(write_hash_equal_read_hash(write_cam_param,stat))
    {
        flag_hash = flag_hash + 1;
    }
	//sem_syslog_warning("**flag_hash = %x\n",flag_hash);
	//sem_syslog_warning("*********************test hash hash_id = 0x0*************************\n");
	
//test hash hash_id = 0x1fffff;
    hash_id=0x1fffff;
	write_cam_param->Indirect_add_reg_L = hash_id & 0xffff;
    write_cam_param->Indirect_add_reg_H = (hash_id>>16);
    write_cam_param->SIP_H = 0xffff;
	write_cam_param->SIP_L = 0xffff;
	write_cam_param->DIP_H = 0xffff;
	write_cam_param->DIP_L = 0xffff;
	write_cam_param->SPORT = 0xffff;
    write_cam_param->DPORT = 0xffff;
    write_cam_param->PROTOCOL = 0xffff;
    write_cam_param->HASH_CTL = 0xffff;
	write_cam_param->DMAC_H = 0xffff;
	write_cam_param->DMAC_M = 0xffff;
	write_cam_param->DMAC_L = 0xffff;
	write_cam_param->SMAC_H = 0xffff;
    write_cam_param->SMAC_M = 0xffff;
    write_cam_param->SMAC_L = 0xffff;
    write_cam_param->VLAN_OUT_H = 0xffff;
	write_cam_param->VLAN_OUT_L = 0xffff;
	write_cam_param->VLAN_IN_H = 0xffff;
	write_cam_param->VLAN_IN_L = 0xffff;
	write_cam_param->FIRST_0800 = 0xffff;
	write_cam_param->IPTOS = 0xffff;
    write_cam_param->IP_LEN = 0xffff;
	write_cam_param->IP_ID = 0xffff;
	write_cam_param->IP_OFFSET = 0xffff;
    write_cam_param->IP_TTL_IP_PROTOCOL = 0xffff;
    write_cam_param->TUNNEL_SIP_H = 0xffff;
    write_cam_param->TUNNEL_SIP_L = 0xffff;
	write_cam_param->TUNNEL_DIP_H = 0xffff;
	write_cam_param->TUNNEL_DIP_L = 0xffff;
	write_cam_param->TUNNEL_SRC_PORT = 0xffff;
	write_cam_param->TUNNEL_DST_PORT = 0xffff;
	write_cam_param->UDP_LEN = 0xffff;
	write_cam_param->UDP_CHECKSUM = 0xffff;
	write_cam_param->CAPWAP_B0B1 = 0xffff;
	write_cam_param->CAPWAP_B2B3 = 0xffff;
	write_cam_param->CAPWAP_B4B5 = 0xffff;
	write_cam_param->CAPWAP_B6B7 = 0xffff;
	write_cam_param->CAPWAP_B8B9 = 0xffff;
	write_cam_param->CAPWAP_B10B11 = 0xffff;
	write_cam_param->CAPWAP_B12B13 = 0xffff;
	write_cam_param->CAPWAP_B14B15 = 0xffff;
	write_cam_param->H_802_11_B0B1 = 0xffff;
	write_cam_param->H_802_11_B2B3 = 0xffff;
	write_cam_param->H_802_11_B4B5 = 0xffff;
	write_cam_param->H_802_11_B6B7 = 0xffff;
	write_cam_param->H_802_11_B8B9 = 0xffff;
	write_cam_param->H_802_11_B10B11 = 0xffff;
	write_cam_param->H_802_11_B12B13 = 0xffff;
	write_cam_param->H_802_11_B14B15 = 0xffff;
	write_cam_param->H_802_11_B16B17 = 0xffff;
	write_cam_param->H_802_11_B18B19 = 0xffff;
	write_cam_param->H_802_11_B20B21 = 0xffff;
	write_cam_param->H_802_11_B22B23 = 0xffff;
	write_cam_param->H_802_11_B24B25 = 0xffff;
	write_cam_param->H_802_11_B26B27 = 0xffff;
	write_cam_param->SEC_0800 = 0xffff;
	write_cam_param->IP_CHECKSUM = 0xffff;
	
    ret = write_hash(write_cam_param,flag);

    ret = read_hash(stat,hash_id,flag);
	
	if(write_hash_equal_read_hash(write_cam_param,stat))
	{
        flag_hash = flag_hash + 1<<1;
	}
	//sem_syslog_warning("**flag_hash = %x\n",flag_hash);
	//sem_syslog_warning("****************test hash hash_id = 0x1fffff************************\n");

//test hash hash_id = 0x1a aaaa;
    hash_id=0x1aaaaa;
	write_cam_param->Indirect_add_reg_L = hash_id & 0xffff;
    write_cam_param->Indirect_add_reg_H = (hash_id>>16);
    write_cam_param->SIP_H = 0xaaaa;
	write_cam_param->SIP_L = 0xaaaa;
	write_cam_param->DIP_H = 0xaaaa;
	write_cam_param->DIP_L = 0xaaaa;
	write_cam_param->SPORT = 0xaaaa;
    write_cam_param->DPORT = 0xaaaa;
    write_cam_param->PROTOCOL = 0xaaaa;
    write_cam_param->HASH_CTL = 0xaaaa;
	write_cam_param->DMAC_H = 0xaaaa;
	write_cam_param->DMAC_M = 0xaaaa;
	write_cam_param->DMAC_L = 0xaaaa;
	write_cam_param->SMAC_H = 0xaaaa;
    write_cam_param->SMAC_M = 0xaaaa;
    write_cam_param->SMAC_L = 0xaaaa;
    write_cam_param->VLAN_OUT_H = 0xaaaa;
	write_cam_param->VLAN_OUT_L = 0xaaaa;
	write_cam_param->VLAN_IN_H = 0xaaaa;
	write_cam_param->VLAN_IN_L = 0xaaaa;
	write_cam_param->IPTOS = 0xaaaa ;
    write_cam_param->IP_TTL_IP_PROTOCOL = 0xaaaa;
    write_cam_param->TUNNEL_SIP_H = 0xaaaa;
    write_cam_param->TUNNEL_SIP_L = 0xaaaa;
	write_cam_param->TUNNEL_DIP_H = 0xaaaa;
	write_cam_param->TUNNEL_DIP_L = 0xaaaa;
	write_cam_param->TUNNEL_SRC_PORT = 0xaaaa;
	write_cam_param->TUNNEL_DST_PORT = 0xaaaa;
	write_cam_param->CAPWAP_B0B1 = 0xaaaa;
	write_cam_param->CAPWAP_B2B3 = 0xaaaa;
	write_cam_param->CAPWAP_B4B5 = 0xaaaa;
	write_cam_param->CAPWAP_B6B7 = 0xaaaa;
	write_cam_param->CAPWAP_B8B9 = 0xaaaa;
	write_cam_param->CAPWAP_B10B11 = 0xaaaa;
	write_cam_param->CAPWAP_B12B13 = 0xaaaa;
	write_cam_param->CAPWAP_B14B15 = 0xaaaa;
	write_cam_param->H_802_11_B0B1 = 0xaaaa;
	write_cam_param->H_802_11_B2B3 = 0xaaaa;
	write_cam_param->H_802_11_B4B5 = 0xaaaa;
	write_cam_param->H_802_11_B6B7 = 0xaaaa;
	write_cam_param->H_802_11_B8B9 = 0xaaaa;
	write_cam_param->H_802_11_B10B11 = 0xaaaa;
	write_cam_param->H_802_11_B12B13 = 0xaaaa;
	write_cam_param->H_802_11_B14B15 = 0xaaaa;
	write_cam_param->H_802_11_B16B17 = 0xaaaa;
	write_cam_param->H_802_11_B18B19 = 0xaaaa;
	write_cam_param->H_802_11_B20B21 = 0xaaaa;
	write_cam_param->H_802_11_B22B23 = 0xaaaa;
	write_cam_param->H_802_11_B24B25 = 0xaaaa;
	write_cam_param->H_802_11_B26B27 = 0xaaaa;
    write_cam_param->FIRST_0800 = 0xaaaa;
    write_cam_param->IP_LEN = 0xaaaa;
    write_cam_param->IP_ID = 0xaaaa;
    write_cam_param->IP_OFFSET = 0xaaaa;
    write_cam_param->UDP_LEN = 0xaaaa;
    write_cam_param->UDP_CHECKSUM = 0xaaaa;
    write_cam_param->SEC_0800 = 0xaaaa;
	write_cam_param->IP_CHECKSUM = 0xaaaa;
	
    ret = write_hash(write_cam_param,flag);

    ret = read_hash(stat,hash_id,flag);
	
	if(write_hash_equal_read_hash(write_cam_param,stat))
	{
        flag_hash = flag_hash + 1<<2;
	}
	//sem_syslog_warning("**flag_hash = %x\n",flag_hash);
	//sem_syslog_warning("**************test hash hash_id = 0x1a aaaa*********************\n");

//test hash hash_id = 0x055555;
    hash_id=0x055555;
	write_cam_param->Indirect_add_reg_L = 0x5555;
    write_cam_param->Indirect_add_reg_H = 0x5;
    write_cam_param->SIP_H = 0x5555;
	write_cam_param->SIP_L = 0x5555;
	write_cam_param->DIP_H = 0x5555;
	write_cam_param->DIP_L = 0x5555;
	write_cam_param->SPORT = 0x5555;
    write_cam_param->DPORT = 0x5555;
    write_cam_param->PROTOCOL = 0x5555;
    write_cam_param->HASH_CTL = 0x5555;
	write_cam_param->DMAC_H = 0x5555;
	write_cam_param->DMAC_M = 0x5555;
	write_cam_param->DMAC_L = 0x5555;
	write_cam_param->SMAC_H = 0x5555;
    write_cam_param->SMAC_M = 0x5555;
    write_cam_param->SMAC_L = 0x5555;
    write_cam_param->VLAN_OUT_H = 0x5555;
	write_cam_param->VLAN_OUT_L = 0x5555;
	write_cam_param->VLAN_IN_H = 0x5555;
	write_cam_param->VLAN_IN_L = 0x5555;
	write_cam_param->IPTOS = 0x5555 ;
    write_cam_param->IP_TTL_IP_PROTOCOL = 0x5555;
    write_cam_param->TUNNEL_SIP_H = 0x5555;
    write_cam_param->TUNNEL_SIP_L = 0x5555;
	write_cam_param->TUNNEL_DIP_H = 0x5555;
	write_cam_param->TUNNEL_DIP_L = 0x5555;
	write_cam_param->TUNNEL_SRC_PORT = 0x5555;
	write_cam_param->TUNNEL_DST_PORT = 0x5555;
	write_cam_param->CAPWAP_B0B1 = 0x5555;
	write_cam_param->CAPWAP_B2B3 = 0x5555;
	write_cam_param->CAPWAP_B4B5 = 0x5555;
	write_cam_param->CAPWAP_B6B7 = 0x5555;
	write_cam_param->CAPWAP_B8B9 = 0x5555;
	write_cam_param->CAPWAP_B10B11 = 0x5555;
	write_cam_param->CAPWAP_B12B13 = 0x5555;
	write_cam_param->CAPWAP_B14B15 = 0x5555;
	write_cam_param->H_802_11_B0B1 = 0x5555;
	write_cam_param->H_802_11_B2B3 = 0x5555;
	write_cam_param->H_802_11_B4B5 = 0x5555;
	write_cam_param->H_802_11_B6B7 = 0x5555;
	write_cam_param->H_802_11_B8B9 = 0x5555;
	write_cam_param->H_802_11_B10B11 = 0x5555;
	write_cam_param->H_802_11_B12B13 = 0x5555;
	write_cam_param->H_802_11_B14B15 = 0x5555;
	write_cam_param->H_802_11_B16B17 = 0x5555;
	write_cam_param->H_802_11_B18B19 = 0x5555;
	write_cam_param->H_802_11_B20B21 = 0x5555;
	write_cam_param->H_802_11_B22B23 = 0x5555;
	write_cam_param->H_802_11_B24B25 = 0x5555;
	write_cam_param->H_802_11_B26B27 = 0x5555;
    write_cam_param->FIRST_0800 = 0x5555;
    write_cam_param->IP_LEN = 0x5555;
    write_cam_param->IP_ID = 0x5555;
    write_cam_param->IP_OFFSET = 0x5555;
    write_cam_param->UDP_LEN = 0x5555;
    write_cam_param->UDP_CHECKSUM = 0x5555;
    write_cam_param->SEC_0800 = 0x5555;
	write_cam_param->IP_CHECKSUM = 0x5555;
	
    ret = write_hash(write_cam_param,flag);

    ret = read_hash(stat,hash_id,flag);
	
	if(write_hash_equal_read_hash(write_cam_param,stat))
	{
        flag_hash = flag_hash + 1<<3;
	}
	//sem_syslog_warning("**flag_hash = %x\n",flag_hash);
	//sem_syslog_warning("****************test hash hash_id = 0x055555*******************\n");
/*test DDR1 data line end*/
	free(stat);
    free(write_cam_param);
	return flag_hash;
}

//FLAG:1 for car_id,0 for IP
//return 0 for equal,1 for wrong
int test_QDR_car_write(car_table_t *write_car_param,unsigned int CAR_ID,int FLAG)
{
	int ret;
	int read_car_ret=0;
	bm_op_args opt;
	int ready = 0;
    int i=0;
	unsigned short Indirect_add_reg_H = 0;
    unsigned short Indirect_add_reg_L = 0;
	unsigned short ip_H_ensure = 0;
	unsigned short ip_L_ensure = 0;
    unsigned short RELOAD_H_ensure =0;
	unsigned short RELOAD_L_ensure =0;
    unsigned short LINKUP_ensure =0;
    unsigned short CAR_VALID_ensure =0;

	read_car_result_t *read_car_result = NULL;
	read_car_result = (read_car_result_t *)malloc(sizeof(read_car_result_t));

	memset(read_car_result,0,sizeof(read_car_result_t));
	memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;
	opt.op_addr = INDIRECT_ACCESS_STATE_REG;
	while(ready == 0)
    {
		ret = read_fpga_reg(&opt);
    	if(ret == -1)
    	{
    		sem_syslog_warning("**when read car,read fpga indirect access control register failure!\n");
    	}
        ready = opt.op_value & READY_REG_VALUE;           //Check Status of ready
	}
	ready = 0;

	memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;
	/* write reload to data cache register 8*/
	opt.op_value = write_car_param->reload & 0xffff;
	opt.op_addr = SOURCE_IP_ADDRESS_REG_H;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write car reload value failure!\n");
	}

    /* write car_valid linkup reload to data cache register 9*/
    opt.op_value = (write_car_param->reload >> 16)+(write_car_param->linkup<<14)+(write_car_param->car_valid<<15);
	opt.op_addr = SOURCE_IP_ADDRESS_REG_L;        
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("##write linkup value failure!\n");
	}
    /* end */

    /* write usr ip to temporary data cache register 10 */
    opt.op_value = write_car_param->usr_ip & 0xffff;
	opt.op_addr = DESTINATION_IP_ADDRESS_REG_H;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write usr ip [15:0] value failure!\n");
	}

	opt.op_value = write_car_param->usr_ip >> 16;
	opt.op_addr = DESTINATION_IP_ADDRESS_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write usr ip [31:16] value failure!\n");
	}
    /*end write usr ip */

    /* write credit to temporary data cache register 12 */
    opt.op_value = write_car_param->credit & 0xffff;//CREDIT_L
	opt.op_addr = SOURCE_PORT_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write credit [15:0] value failure!\n");
	}

	opt.op_value = write_car_param->credit >>16;//CREDIT_H
	opt.op_addr = DESTINATION_PORT_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write credit [31:16] value failure!\n");
	}
    /*end write credit */

    /*start write byte drop count [39:32] to temporary data cache register 14*/		
	opt.op_value = (write_car_param->byte_drop_count >> 24) & 0xff00;//BYTEDROPCOUNT_H
	opt.op_addr = PROTOCOL_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write byte drop count [39:32] to temporary data cache register failure!\n");
	}
	/*end*/

	/*start write byte sent count [39:32] to temporary data cache register 15*/		
    opt.op_value = write_car_param->byte_set_count >> 24;//BYTESENTCOUNT_H
	opt.op_addr = HASHCTL_REG;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write byte sent count [39:32] to temporary data cache register failure!\n");
	}
    /*end*/

    /*start write byte sent count [15:0] to temporary data cache register 16*/		
    opt.op_value = write_car_param->byte_set_count & 0xffff;//BYTESENTCOUNT_L
	opt.op_addr = DESTINATION_MAC_ADDRESS_REG_H;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write byte sent count [15:0] to temporary data cache register failure!\n");
	}
    /*end*/

    /*start write byte sent count [31:16] to temporary data cache register 17*/		
    opt.op_value = (write_car_param->byte_set_count & 0xffff0000)>>16;//BYTESENTCOUNT_M
	opt.op_addr = DESTINATION_MAC_ADDRESS_REG_M;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write byte sent count [31:16] to temporary data cache register failure!\n");
	}
    /*end*/

    /*start write byte drop count [15:0] to temporary data cache register 18*/		
	opt.op_value = write_car_param->byte_drop_count & 0xffff;//BYTEDROPCOUNT_L
	opt.op_addr = DESTINATION_MAC_ADDRESS_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write byte drop count [15:0] to temporary data cache register failure!\n");
	}
	/*end*/

    /*start write byte drop count [31:16] to temporary data cache register 19*/	
	opt.op_value = (write_car_param->byte_drop_count& 0xffff0000)>>16;//BYTEDROPCOUNT_M
	opt.op_addr = SOURCE_MAC_ADDRESS_REG_H;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write byte drop count [31:16] to temporary data cache register failure!\n");
	}
	/*end*/

    /*start write package sent count [15:0] to temporary data cache register 20*/		
	opt.op_value = write_car_param->package_set_count & 0xffff;//PACKAGESENTCOUNT_L
	opt.op_addr = SOURCE_MAC_ADDRESS_REG_M;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write  package sent count [15:0] to temporary data cache register failure!\n");
	}
	/*end*/

    /*start write package sent count [31:16] to temporary data cache register 21*/	
	opt.op_value = write_car_param->package_set_count >> 16;//PACKAGESENTCOUNT_H
	opt.op_addr = SOURCE_MAC_ADDRESS_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write  package sent count [31:16] to temporary data cache register failure!\n");
	}
	/*end*/

    /*start write package drop count [15:0] to temporary data cache register 22*/		
	opt.op_value = write_car_param->package_drop_count & 0xffff;//PACKAGEDROPCOUNT_L
	opt.op_addr = VLAN_OUT_REG_H;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write  package drop count [15:0] to temporary data cache register failure!\n");
	}
	/*end*/

    /*start write package drop count [31:16] to temporary data cache register 23*/	
	opt.op_value = write_car_param->package_drop_count >>16;//PACKAGEDROPCOUNT_H
	opt.op_addr = VLAN_OUT_REG_L;
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write  package drop count [31:16] to temporary data cache register failure!\n");
	}
	/*end*/
	if(FLAG == 1)
	{
    	opt.op_value = CAR_ID & 0Xffff;
    	opt.op_addr = INDIRECT_ACCESS_ADDRESS_REG_L;
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**write car CAR_ID value failure!\n");
    	}
		opt.op_value = (CAR_ID & 0X30000)>>16;
    	opt.op_addr = INDIRECT_ACCESS_ADDRESS_REG_H;
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**write car CAR_ID value failure!\n");
    	}
	}
	else
	{
    	opt.op_value = write_car_param->usr_ip & 0xffff;
    	opt.op_addr = INDIRECT_ACCESS_ADDRESS_REG_L;
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**write car CAR_ID value failure!\n");
    	}

		opt.op_value = write_car_param->usr_ip >> 16;
    	opt.op_addr = INDIRECT_ACCESS_ADDRESS_REG_H;
    	ret = write_fpga_reg(&opt);
    	if(ret)
    	{
    		sem_syslog_warning("**write car CAR_ID value failure!\n");
    	} 
	}

	/* car table into write operation */
	opt.op_addr = INDIRECT_ACCESS_CONTROL_REG;
	if(FLAG == 1)
	{
        opt.op_value = 0x8034;
	}
    else
    {
	    opt.op_value = 0x8030+write_car_param->linkup;     /* read operation-- set bit 15 for 1,car table select_id=0x3 */
    }
	ret = write_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**write fpga indirect access control register failure!\n");
	}
    Indirect_add_reg_H = write_car_param->usr_ip >> 16;
    Indirect_add_reg_L = write_car_param->usr_ip & 0xffff;
	read_car_ret = read_car(read_car_result,Indirect_add_reg_H,Indirect_add_reg_L,write_car_param->linkup,CAR_ID,FLAG);
	if(!read_car_ret)
    {
    	free(read_car_result);
    	read_car_result = NULL;
    	sem_syslog_warning("read car table fail!\n");
    	return 1;
	}
    else
    {
		if(read_car_result->reload != write_car_param->reload)
		{
            sem_syslog_warning("**read_car_result->reload = %llx\n",read_car_result->reload);
			sem_syslog_warning("**write_car_param->reload = %llx\n",write_car_param->reload);
			goto finish;
		}
		if(read_car_result->linkup!= write_car_param->linkup)
		{
            sem_syslog_warning("**read_car_result->linkup = %llx\n",read_car_result->linkup);
			sem_syslog_warning("**write_car_param->linkup = %llx\n",write_car_param->linkup);
			goto finish;
		}
		if(read_car_result->car_valid!= write_car_param->car_valid)
		{
            sem_syslog_warning("**read_car_result->car_valid = %llx\n",read_car_result->car_valid);
			sem_syslog_warning("**write_car_param->car_valid = %llx\n",write_car_param->car_valid);
			goto finish;
		}
		if(read_car_result->usr_ip!= write_car_param->usr_ip)
		{
            sem_syslog_warning("**read_car_result->usr_ip = %x\n",read_car_result->usr_ip);
			sem_syslog_warning("**write_car_param->usr_ip = %x\n",write_car_param->usr_ip);
			goto finish;
		}
		if(read_car_result->byte_drop_count!= write_car_param->byte_drop_count)
		{
            sem_syslog_warning("**read_car_result->byte_drop_count = %llx\n",read_car_result->byte_drop_count);
			sem_syslog_warning("**write_car_param->byte_drop_count = %llx\n",write_car_param->byte_drop_count);
			goto finish;
		}
		if(read_car_result->byte_set_count!= write_car_param->byte_set_count)
		{
            sem_syslog_warning("**read_car_result->byte_set_count = %llx\n",read_car_result->byte_set_count);
			sem_syslog_warning("**write_car_param->byte_set_count = %llx\n",write_car_param->byte_set_count);
			goto finish;
		}
		if(read_car_result->package_drop_count!= write_car_param->package_drop_count)
		{
            sem_syslog_warning("**read_car_result->package_drop_count = %llx\n",read_car_result->package_drop_count);
			sem_syslog_warning("**write_car_param->package_drop_count = %llx\n",write_car_param->package_drop_count);
			goto finish;
		}
		if(read_car_result->package_set_count!= write_car_param->package_set_count)
		{
            sem_syslog_warning("**read_car_result->package_set_count = %llx\n",read_car_result->package_set_count);
			sem_syslog_warning("**write_car_param->package_set_count = %llx\n",write_car_param->package_set_count);
			goto finish;
		}
		free(read_car_result);
        read_car_result = NULL;
		return 0;
	}
finish:
    free(read_car_result);
    read_car_result = NULL;
    return 1;
    /*end*/
}

DBusMessage *sem_dbus_show_ram_detection(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	int ret = 0;
	int i;
	int flag;
	unsigned int hash_id;
    
	unsigned int flag_hash_data_1 = 0;
	unsigned int flag_hash_data_2 = 0;
	unsigned int flag_hash_addr_low_1 = 0;
	unsigned int flag_hash_addr_1 = 0;
	unsigned int flag_hash_addr_low_2 = 0;
	unsigned int flag_hash_addr_2 = 0;	
	
    unsigned int flag_car_data = 0;
	unsigned int flag_car_addr_low = 0;
	unsigned int flag_car_addr = 0;

	
	unsigned int flag_car_for = 0;
	unsigned int flag_car = 0;
	
	unsigned short Indirect_add_reg_H = 0;
	unsigned short Indirect_add_reg_L = 0;
    unsigned short RELOAD_H =0;
	unsigned short RELOAD_L =0;
    unsigned short LINKUP =0;
    unsigned short CAR_VALID =0;
	unsigned int CAR_ID ;
	unsigned int hash_table_id = 0;

	car_table_t *write_car_param =NULL;
	car_table_t *write_car_param_base =NULL;
	car_table_t *write_car_param_compare =NULL;
	read_car_result_t *read_car_result = NULL;
	
	write_cam_param_t *write_cam_param_base =NULL;
	write_cam_param_t *write_cam_param_compare =NULL;
	hash_table_t *stat_base =NULL;
    hash_table_t *stat_compare =NULL;

	write_car_param = (car_table_t *)malloc(sizeof(car_table_t));
    write_car_param_base = (car_table_t *)malloc(sizeof(car_table_t));
    write_car_param_compare = (car_table_t *)malloc(sizeof(car_table_t));
	
	read_car_result = (read_car_result_t *)malloc(sizeof(read_car_result_t));
    stat_base = (hash_table_t *)malloc(sizeof(hash_table_t));
	write_cam_param_base = (write_cam_param_t *)malloc(sizeof(write_cam_param_t));
    stat_compare = (hash_table_t *)malloc(sizeof(hash_table_t));
	write_cam_param_compare = (write_cam_param_t *)malloc(sizeof(write_cam_param_t));
	
    /*test data line*/
    flag_hash_data_1 = test_hash_DDR_date_line(1);
	flag_hash_data_2 = test_hash_DDR_date_line(2);
	/*test data line end*/
	
/*test address*/
//first test low five bit
    hash_id = 0;
	flag = 1;
	write_cam_param_base->Indirect_add_reg_L = hash_id & 0xffff;
    write_cam_param_base->Indirect_add_reg_H = (hash_id>>16);
    write_cam_param_base->SIP_H = 0x0101;
	write_cam_param_base->SIP_L = 0x0202;
	write_cam_param_base->DIP_H = 0x0303;
	write_cam_param_base->DIP_L = 0x0404;
	write_cam_param_base->SPORT = 0x0505;
    write_cam_param_base->DPORT = 0x0606;
    write_cam_param_base->PROTOCOL = 0x0707;
    write_cam_param_base->HASH_CTL = 0x0808;
	write_cam_param_base->DMAC_H = 0x0909;
	write_cam_param_base->DMAC_M = 0x0a0a;
	write_cam_param_base->DMAC_L = 0x0b0b;
	write_cam_param_base->SMAC_H = 0x0c0c;
    write_cam_param_base->SMAC_M = 0x0e0e;
    write_cam_param_base->SMAC_L = 0x1010;
    write_cam_param_base->VLAN_OUT_H = 0x2020;
	write_cam_param_base->VLAN_OUT_L = 0x3030;
	write_cam_param_base->VLAN_IN_H = 0x4040;
	write_cam_param_base->VLAN_IN_L = 0x5050;
	write_cam_param_base->IPTOS = 0x6060 ;
    write_cam_param_base->IP_TTL_IP_PROTOCOL = 0x7070;
    write_cam_param_base->TUNNEL_SIP_H = 0x8080;
    write_cam_param_base->TUNNEL_SIP_L = 0x9090;
	write_cam_param_base->TUNNEL_DIP_H = 0xa0a0;
	write_cam_param_base->TUNNEL_DIP_L = 0xb0b0;
	write_cam_param_base->TUNNEL_SRC_PORT = 0xc0c0;
	write_cam_param_base->TUNNEL_DST_PORT = 0xd0d0;
	write_cam_param_base->CAPWAP_B0B1 = 0xe0e0;
	write_cam_param_base->CAPWAP_B2B3 = 0x0001;
	write_cam_param_base->CAPWAP_B4B5 = 0x0002;
	write_cam_param_base->CAPWAP_B6B7 = 0x0003;
	write_cam_param_base->CAPWAP_B8B9 = 0x0004;
	write_cam_param_base->CAPWAP_B10B11 = 0x0005;
	write_cam_param_base->CAPWAP_B12B13 = 0x0006;
	write_cam_param_base->CAPWAP_B14B15 = 0x0007;
	write_cam_param_base->H_802_11_B0B1 = 0x0008;
	write_cam_param_base->H_802_11_B2B3 = 0x0009;
	write_cam_param_base->H_802_11_B4B5 = 0x000a;
	write_cam_param_base->H_802_11_B6B7 = 0x000b;
	write_cam_param_base->H_802_11_B8B9 = 0x000c;
	write_cam_param_base->H_802_11_B10B11 = 0x000d;
	write_cam_param_base->H_802_11_B12B13 = 0x0010;
	write_cam_param_base->H_802_11_B14B15 = 0x0020;
	write_cam_param_base->H_802_11_B16B17 = 0x0030;
	write_cam_param_base->H_802_11_B18B19 = 0x0040;
	write_cam_param_base->H_802_11_B20B21 = 0x0050;
	write_cam_param_base->H_802_11_B22B23 = 0x0060;
	write_cam_param_base->H_802_11_B24B25 = 0x0070;
	write_cam_param_base->H_802_11_B26B27 = 0x0080;
    write_cam_param_base->FIRST_0800 = 0x0090;
    write_cam_param_base->IP_LEN = 0x00a0;
    write_cam_param_base->IP_ID = 0x0100;
    write_cam_param_base->IP_OFFSET = 0x0200;
    write_cam_param_base->UDP_LEN = 0x0300;
    write_cam_param_base->UDP_CHECKSUM = 0x0400;
    write_cam_param_base->SEC_0800 = 0x0500;
	write_cam_param_base->IP_CHECKSUM = 0x0600;
  
	
    ret = write_hash(write_cam_param_base,flag);
	
    ret = read_hash(stat_base,hash_id,flag);
	
    flag_hash_addr_low_1 = write_hash_equal_read_hash(write_cam_param_base,stat_base);

	sem_syslog_warning("**flag_hash_addr_low_1 = %x\n",flag_hash_addr_low_1);

	hash_id = 0;
	flag = 2;
	write_cam_param_base->Indirect_add_reg_L = hash_id & 0xffff;
    write_cam_param_base->Indirect_add_reg_H = (hash_id>>16);
    write_cam_param_base->SIP_H = 0x0101;
	write_cam_param_base->SIP_L = 0x0202;
	write_cam_param_base->DIP_H = 0x0303;
	write_cam_param_base->DIP_L = 0x0404;
	write_cam_param_base->SPORT = 0x0505;
    write_cam_param_base->DPORT = 0x0606;
    write_cam_param_base->PROTOCOL = 0x0707;
    write_cam_param_base->HASH_CTL = 0x0808;
	write_cam_param_base->DMAC_H = 0x0909;
	write_cam_param_base->DMAC_M = 0x0a0a;
	write_cam_param_base->DMAC_L = 0x0b0b;
	write_cam_param_base->SMAC_H = 0x0c0c;
    write_cam_param_base->SMAC_M = 0x0e0e;
    write_cam_param_base->SMAC_L = 0x1010;
    write_cam_param_base->VLAN_OUT_H = 0x2020;
	write_cam_param_base->VLAN_OUT_L = 0x3030;
	write_cam_param_base->VLAN_IN_H = 0x4040;
	write_cam_param_base->VLAN_IN_L = 0x5050;
	write_cam_param_base->IPTOS = 0x6060 ;
    write_cam_param_base->IP_TTL_IP_PROTOCOL = 0x7070;
    write_cam_param_base->TUNNEL_SIP_H = 0x8080;
    write_cam_param_base->TUNNEL_SIP_L = 0x9090;
	write_cam_param_base->TUNNEL_DIP_H = 0xa0a0;
	write_cam_param_base->TUNNEL_DIP_L = 0xb0b0;
	write_cam_param_base->TUNNEL_SRC_PORT = 0xc0c0;
	write_cam_param_base->TUNNEL_DST_PORT = 0xd0d0;
	write_cam_param_base->CAPWAP_B0B1 = 0xe0e0;
	write_cam_param_base->CAPWAP_B2B3 = 0x0001;
	write_cam_param_base->CAPWAP_B4B5 = 0x0002;
	write_cam_param_base->CAPWAP_B6B7 = 0x0003;
	write_cam_param_base->CAPWAP_B8B9 = 0x0004;
	write_cam_param_base->CAPWAP_B10B11 = 0x0005;
	write_cam_param_base->CAPWAP_B12B13 = 0x0006;
	write_cam_param_base->CAPWAP_B14B15 = 0x0007;
	write_cam_param_base->H_802_11_B0B1 = 0x0008;
	write_cam_param_base->H_802_11_B2B3 = 0x0009;
	write_cam_param_base->H_802_11_B4B5 = 0x000a;
	write_cam_param_base->H_802_11_B6B7 = 0x000b;
	write_cam_param_base->H_802_11_B8B9 = 0x000c;
	write_cam_param_base->H_802_11_B10B11 = 0x000d;
	write_cam_param_base->H_802_11_B12B13 = 0x0010;
	write_cam_param_base->H_802_11_B14B15 = 0x0020;
	write_cam_param_base->H_802_11_B16B17 = 0x0030;
	write_cam_param_base->H_802_11_B18B19 = 0x0040;
	write_cam_param_base->H_802_11_B20B21 = 0x0050;
	write_cam_param_base->H_802_11_B22B23 = 0x0060;
	write_cam_param_base->H_802_11_B24B25 = 0x0070;
	write_cam_param_base->H_802_11_B26B27 = 0x0080;
    write_cam_param_base->FIRST_0800 = 0x0090;
    write_cam_param_base->IP_LEN = 0x00a0;
    write_cam_param_base->IP_ID = 0x0100;
    write_cam_param_base->IP_OFFSET = 0x0200;
    write_cam_param_base->UDP_LEN = 0x0300;
    write_cam_param_base->UDP_CHECKSUM = 0x0400;
    write_cam_param_base->SEC_0800 = 0x0500;
	write_cam_param_base->IP_CHECKSUM = 0x0600;
  
	
    ret = write_hash(write_cam_param_base,flag);
	
    ret = read_hash(stat_base,hash_id,flag);
	
    flag_hash_addr_low_2 = write_hash_equal_read_hash(write_cam_param_base,stat_base);

	sem_syslog_warning("**flag_hash_addr_low_2 = %x\n",flag_hash_addr_low_2);


//second test diff hash_id address

    for(i=0;i<22;i++)
    {
		if(i == 18)
		{
			continue;
		}
//hash_table 1			
    	hash_id = 0;
    	write_cam_param_base->Indirect_add_reg_L = hash_id & 0xffff;
        write_cam_param_base->Indirect_add_reg_H = (hash_id>>16);
        write_cam_param_base->SIP_H = 0x5555;
    	write_cam_param_base->SIP_L = 0x5555;
    	write_cam_param_base->DIP_H = 0x5555;
    	write_cam_param_base->DIP_L = 0x5555;
    	write_cam_param_base->SPORT = 0x5555;
        write_cam_param_base->DPORT = 0x5555;
        write_cam_param_base->PROTOCOL = 0x5555;
        write_cam_param_base->HASH_CTL = 0x5555;
    	write_cam_param_base->DMAC_H = 0x5555;
    	write_cam_param_base->DMAC_M = 0x5555;
    	write_cam_param_base->DMAC_L = 0x5555;
    	write_cam_param_base->SMAC_H = 0x5555;
        write_cam_param_base->SMAC_M = 0x5555;
        write_cam_param_base->SMAC_L = 0x5555;
        write_cam_param_base->VLAN_OUT_H = 0x5555;
    	write_cam_param_base->VLAN_OUT_L = 0x5555;
    	write_cam_param_base->VLAN_IN_H = 0x5555;
    	write_cam_param_base->VLAN_IN_L = 0x5555;
    	write_cam_param_base->IPTOS = 0x5555 ;
        write_cam_param_base->IP_TTL_IP_PROTOCOL = 0x5555;
        write_cam_param_base->TUNNEL_SIP_H = 0x5555;
        write_cam_param_base->TUNNEL_SIP_L = 0x5555;
    	write_cam_param_base->TUNNEL_DIP_H = 0x5555;
    	write_cam_param_base->TUNNEL_DIP_L = 0x5555;
    	write_cam_param_base->TUNNEL_SRC_PORT = 0x5555;
    	write_cam_param_base->TUNNEL_DST_PORT = 0x5555;
    	write_cam_param_base->CAPWAP_B0B1 = 0x5555;
    	write_cam_param_base->CAPWAP_B2B3 = 0x5555;
    	write_cam_param_base->CAPWAP_B4B5 = 0x5555;
    	write_cam_param_base->CAPWAP_B6B7 = 0x5555;
    	write_cam_param_base->CAPWAP_B8B9 = 0x5555;
    	write_cam_param_base->CAPWAP_B10B11 = 0x5555;
    	write_cam_param_base->CAPWAP_B12B13 = 0x5555;
    	write_cam_param_base->CAPWAP_B14B15 = 0x5555;
    	write_cam_param_base->H_802_11_B0B1 = 0x5555;
    	write_cam_param_base->H_802_11_B2B3 = 0x5555;
    	write_cam_param_base->H_802_11_B4B5 = 0x5555;
    	write_cam_param_base->H_802_11_B6B7 = 0x5555;
    	write_cam_param_base->H_802_11_B8B9 = 0x5555;
    	write_cam_param_base->H_802_11_B10B11 = 0x5555;
    	write_cam_param_base->H_802_11_B12B13 = 0x5555;
    	write_cam_param_base->H_802_11_B14B15 = 0x5555;
    	write_cam_param_base->H_802_11_B16B17 = 0x5555;
    	write_cam_param_base->H_802_11_B18B19 = 0x5555;
    	write_cam_param_base->H_802_11_B20B21 = 0x5555;
    	write_cam_param_base->H_802_11_B22B23 = 0x5555;
    	write_cam_param_base->H_802_11_B24B25 = 0x5555;
    	write_cam_param_base->H_802_11_B26B27 = 0x5555;
	    write_cam_param_base->FIRST_0800 = 0x5555;
        write_cam_param_base->IP_LEN = 0x5555;
        write_cam_param_base->IP_ID = 0x5555;
        write_cam_param_base->IP_OFFSET = 0x5555;
        write_cam_param_base->UDP_LEN = 0x5555;
        write_cam_param_base->UDP_CHECKSUM = 0x5555;
        write_cam_param_base->SEC_0800 = 0x5555;
		write_cam_param_base->IP_CHECKSUM = 0x5555;
        ret = write_hash(write_cam_param_base,1);

        write_cam_param_compare->SIP_H = 0xaaaa;
    	write_cam_param_compare->SIP_L = 0xaaaa;
    	write_cam_param_compare->DIP_H = 0xaaaa;
    	write_cam_param_compare->DIP_L = 0xaaaa;
    	write_cam_param_compare->SPORT = 0xaaaa;
        write_cam_param_compare->DPORT = 0xaaaa;
        write_cam_param_compare->PROTOCOL = 0xaaaa;
        write_cam_param_compare->HASH_CTL = 0xaaaa;
    	write_cam_param_compare->DMAC_H = 0xaaaa;
    	write_cam_param_compare->DMAC_M = 0xaaaa;
    	write_cam_param_compare->DMAC_L = 0xaaaa;
    	write_cam_param_compare->SMAC_H = 0xaaaa;
        write_cam_param_compare->SMAC_M = 0xaaaa;
        write_cam_param_compare->SMAC_L = 0xaaaa;
        write_cam_param_compare->VLAN_OUT_H = 0xaaaa;
    	write_cam_param_compare->VLAN_OUT_L = 0xaaaa;
    	write_cam_param_compare->VLAN_IN_H = 0xaaaa;
    	write_cam_param_compare->VLAN_IN_L = 0xaaaa;
    	write_cam_param_compare->IPTOS = 0xaaaa ;
        write_cam_param_compare->IP_TTL_IP_PROTOCOL = 0xaaaa;
        write_cam_param_compare->TUNNEL_SIP_H = 0xaaaa;
        write_cam_param_compare->TUNNEL_SIP_L = 0xaaaa;
    	write_cam_param_compare->TUNNEL_DIP_H = 0xaaaa;
    	write_cam_param_compare->TUNNEL_DIP_L = 0xaaaa;
    	write_cam_param_compare->TUNNEL_SRC_PORT = 0xaaaa;
    	write_cam_param_compare->TUNNEL_DST_PORT = 0xaaaa;
    	write_cam_param_compare->CAPWAP_B0B1 = 0xaaaa;
    	write_cam_param_compare->CAPWAP_B2B3 = 0xaaaa;
    	write_cam_param_compare->CAPWAP_B4B5 = 0xaaaa;
    	write_cam_param_compare->CAPWAP_B6B7 = 0xaaaa;
    	write_cam_param_compare->CAPWAP_B8B9 = 0xaaaa;
    	write_cam_param_compare->CAPWAP_B10B11 = 0xaaaa;
    	write_cam_param_compare->CAPWAP_B12B13 = 0xaaaa;
    	write_cam_param_compare->CAPWAP_B14B15 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B0B1 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B2B3 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B4B5 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B6B7 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B8B9 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B10B11 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B12B13 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B14B15 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B16B17 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B18B19 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B20B21 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B22B23 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B24B25 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B26B27 = 0xaaaa;
        write_cam_param_compare->FIRST_0800 = 0xaaaa;
        write_cam_param_compare->IP_LEN = 0xaaaa;
        write_cam_param_compare->IP_ID = 0xaaaa;
        write_cam_param_compare->IP_OFFSET = 0xaaaa;
        write_cam_param_compare->UDP_LEN = 0xaaaa;
        write_cam_param_compare->UDP_CHECKSUM = 0xaaaa;
        write_cam_param_compare->SEC_0800 = 0xaaaa;
    	write_cam_param_compare->IP_CHECKSUM = 0xaaaa;

		hash_id = 1 << i;
    	write_cam_param_compare->Indirect_add_reg_L = hash_id & 0xffff;
        write_cam_param_compare->Indirect_add_reg_H = (hash_id>>16);
		
        ret = write_hash(write_cam_param_compare,1);

        ret= read_hash(stat_base,0,1);
		ret= read_hash(stat_compare,hash_id,1);

		if(read_hash_not_equal(stat_base,stat_compare))
		{
            flag_hash_addr_1 += 1<<i;
		}
		
    	sem_syslog_warning("**i = %d;flag_hash_addr_1 = %d\n",i,flag_hash_addr_1);
//hash_table 2		
    	hash_id = 0;
    	write_cam_param_base->Indirect_add_reg_L = hash_id & 0xffff;
        write_cam_param_base->Indirect_add_reg_H = (hash_id>>16);
        write_cam_param_base->SIP_H = 0x5555;
    	write_cam_param_base->SIP_L = 0x5555;
    	write_cam_param_base->DIP_H = 0x5555;
    	write_cam_param_base->DIP_L = 0x5555;
    	write_cam_param_base->SPORT = 0x5555;
        write_cam_param_base->DPORT = 0x5555;
        write_cam_param_base->PROTOCOL = 0x5555;
        write_cam_param_base->HASH_CTL = 0x5555;
    	write_cam_param_base->DMAC_H = 0x5555;
    	write_cam_param_base->DMAC_M = 0x5555;
    	write_cam_param_base->DMAC_L = 0x5555;
    	write_cam_param_base->SMAC_H = 0x5555;
        write_cam_param_base->SMAC_M = 0x5555;
        write_cam_param_base->SMAC_L = 0x5555;
        write_cam_param_base->VLAN_OUT_H = 0x5555;
    	write_cam_param_base->VLAN_OUT_L = 0x5555;
    	write_cam_param_base->VLAN_IN_H = 0x5555;
    	write_cam_param_base->VLAN_IN_L = 0x5555;
    	write_cam_param_base->IPTOS = 0x5555 ;
        write_cam_param_base->IP_TTL_IP_PROTOCOL = 0x5555;
        write_cam_param_base->TUNNEL_SIP_H = 0x5555;
        write_cam_param_base->TUNNEL_SIP_L = 0x5555;
    	write_cam_param_base->TUNNEL_DIP_H = 0x5555;
    	write_cam_param_base->TUNNEL_DIP_L = 0x5555;
    	write_cam_param_base->TUNNEL_SRC_PORT = 0x5555;
    	write_cam_param_base->TUNNEL_DST_PORT = 0x5555;
    	write_cam_param_base->CAPWAP_B0B1 = 0x5555;
    	write_cam_param_base->CAPWAP_B2B3 = 0x5555;
    	write_cam_param_base->CAPWAP_B4B5 = 0x5555;
    	write_cam_param_base->CAPWAP_B6B7 = 0x5555;
    	write_cam_param_base->CAPWAP_B8B9 = 0x5555;
    	write_cam_param_base->CAPWAP_B10B11 = 0x5555;
    	write_cam_param_base->CAPWAP_B12B13 = 0x5555;
    	write_cam_param_base->CAPWAP_B14B15 = 0x5555;
    	write_cam_param_base->H_802_11_B0B1 = 0x5555;
    	write_cam_param_base->H_802_11_B2B3 = 0x5555;
    	write_cam_param_base->H_802_11_B4B5 = 0x5555;
    	write_cam_param_base->H_802_11_B6B7 = 0x5555;
    	write_cam_param_base->H_802_11_B8B9 = 0x5555;
    	write_cam_param_base->H_802_11_B10B11 = 0x5555;
    	write_cam_param_base->H_802_11_B12B13 = 0x5555;
    	write_cam_param_base->H_802_11_B14B15 = 0x5555;
    	write_cam_param_base->H_802_11_B16B17 = 0x5555;
    	write_cam_param_base->H_802_11_B18B19 = 0x5555;
    	write_cam_param_base->H_802_11_B20B21 = 0x5555;
    	write_cam_param_base->H_802_11_B22B23 = 0x5555;
    	write_cam_param_base->H_802_11_B24B25 = 0x5555;
    	write_cam_param_base->H_802_11_B26B27 = 0x5555;
	    write_cam_param_base->FIRST_0800 = 0x5555;
        write_cam_param_base->IP_LEN = 0x5555;
        write_cam_param_base->IP_ID = 0x5555;
        write_cam_param_base->IP_OFFSET = 0x5555;
        write_cam_param_base->UDP_LEN = 0x5555;
        write_cam_param_base->UDP_CHECKSUM = 0x5555;
        write_cam_param_base->SEC_0800 = 0x5555;
		write_cam_param_base->IP_CHECKSUM = 0x5555;
        ret = write_hash(write_cam_param_base,2);

        write_cam_param_compare->SIP_H = 0xaaaa;
    	write_cam_param_compare->SIP_L = 0xaaaa;
    	write_cam_param_compare->DIP_H = 0xaaaa;
    	write_cam_param_compare->DIP_L = 0xaaaa;
    	write_cam_param_compare->SPORT = 0xaaaa;
        write_cam_param_compare->DPORT = 0xaaaa;
        write_cam_param_compare->PROTOCOL = 0xaaaa;
        write_cam_param_compare->HASH_CTL = 0xaaaa;
    	write_cam_param_compare->DMAC_H = 0xaaaa;
    	write_cam_param_compare->DMAC_M = 0xaaaa;
    	write_cam_param_compare->DMAC_L = 0xaaaa;
    	write_cam_param_compare->SMAC_H = 0xaaaa;
        write_cam_param_compare->SMAC_M = 0xaaaa;
        write_cam_param_compare->SMAC_L = 0xaaaa;
        write_cam_param_compare->VLAN_OUT_H = 0xaaaa;
    	write_cam_param_compare->VLAN_OUT_L = 0xaaaa;
    	write_cam_param_compare->VLAN_IN_H = 0xaaaa;
    	write_cam_param_compare->VLAN_IN_L = 0xaaaa;
    	write_cam_param_compare->IPTOS = 0xaaaa ;
        write_cam_param_compare->IP_TTL_IP_PROTOCOL = 0xaaaa;
        write_cam_param_compare->TUNNEL_SIP_H = 0xaaaa;
        write_cam_param_compare->TUNNEL_SIP_L = 0xaaaa;
    	write_cam_param_compare->TUNNEL_DIP_H = 0xaaaa;
    	write_cam_param_compare->TUNNEL_DIP_L = 0xaaaa;
    	write_cam_param_compare->TUNNEL_SRC_PORT = 0xaaaa;
    	write_cam_param_compare->TUNNEL_DST_PORT = 0xaaaa;
    	write_cam_param_compare->CAPWAP_B0B1 = 0xaaaa;
    	write_cam_param_compare->CAPWAP_B2B3 = 0xaaaa;
    	write_cam_param_compare->CAPWAP_B4B5 = 0xaaaa;
    	write_cam_param_compare->CAPWAP_B6B7 = 0xaaaa;
    	write_cam_param_compare->CAPWAP_B8B9 = 0xaaaa;
    	write_cam_param_compare->CAPWAP_B10B11 = 0xaaaa;
    	write_cam_param_compare->CAPWAP_B12B13 = 0xaaaa;
    	write_cam_param_compare->CAPWAP_B14B15 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B0B1 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B2B3 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B4B5 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B6B7 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B8B9 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B10B11 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B12B13 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B14B15 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B16B17 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B18B19 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B20B21 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B22B23 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B24B25 = 0xaaaa;
    	write_cam_param_compare->H_802_11_B26B27 = 0xaaaa;
        write_cam_param_compare->FIRST_0800 = 0xaaaa;
        write_cam_param_compare->IP_LEN = 0xaaaa;
        write_cam_param_compare->IP_ID = 0xaaaa;
        write_cam_param_compare->IP_OFFSET = 0xaaaa;
        write_cam_param_compare->UDP_LEN = 0xaaaa;
        write_cam_param_compare->UDP_CHECKSUM = 0xaaaa;
        write_cam_param_compare->SEC_0800 = 0xaaaa;
    	write_cam_param_compare->IP_CHECKSUM = 0xaaaa;

		hash_id = 1 << i;
    	write_cam_param_compare->Indirect_add_reg_L = hash_id & 0xffff;
        write_cam_param_compare->Indirect_add_reg_H = (hash_id>>16);
		
        ret = write_hash(write_cam_param_compare,2);

        ret= read_hash(stat_base,0,2);
		ret= read_hash(stat_compare,hash_id,2);

		if(read_hash_not_equal(stat_base,stat_compare))
		{
            flag_hash_addr_2 += 1<<i;
		}
		
    	sem_syslog_warning("**i = %d;flag_hash_addr_2 = %d\n",i,flag_hash_addr_2);
	}

#if 1
/*begin test QDR2 (car)*/
//test data link ffff
    write_car_param->reload = 0xffffffULL;
    write_car_param->linkup = 0x1;
    write_car_param->car_valid = 0x1;
    write_car_param->usr_ip = 0xffffffff;
	write_car_param->credit = 0x0;
    write_car_param->byte_drop_count = 0xffffffffffULL;
    write_car_param->byte_set_count = 0xffffffffffULL;
    write_car_param->package_drop_count = 0xffffffffULL;
    write_car_param->package_set_count = 0xffffffffULL;
    CAR_ID =1;
	if(test_QDR_car_write(write_car_param,CAR_ID,0))
	{
        flag_car_data = flag_car_data + 1;
	}
	sem_syslog_warning("**flag_car_data= %x\n",flag_car_data);
//test data link 0000
    write_car_param->reload = 0;
    write_car_param->linkup = 0;
    write_car_param->car_valid = 0;
    write_car_param->usr_ip = 0;
	write_car_param->credit = 0x0;
    write_car_param->byte_drop_count = 0;
    write_car_param->byte_set_count = 0;
    write_car_param->package_drop_count = 0;
    write_car_param->package_set_count = 0;
    CAR_ID =1;
	if(test_QDR_car_write(write_car_param,CAR_ID,0))
	{
        flag_car_data = flag_car_data + 1<<1;
	}
	sem_syslog_warning("**flag_car_data= %x\n",flag_car_data);
//test data link aaaa
    write_car_param->reload = 0xaaaaaa;
    write_car_param->linkup = 0x1;
    write_car_param->car_valid = 0x1;
    write_car_param->usr_ip = 0xaaaaaaaa;
	write_car_param->credit = 0x0;
    write_car_param->byte_drop_count = 0xaaaaaaaaaaULL;
    write_car_param->byte_set_count =  0xaaaaaaaaaaULL;
    write_car_param->package_drop_count = 0xaaaaaaaaULL;
    write_car_param->package_set_count = 0xaaaaaaaaULL;
    CAR_ID =1;
	if(test_QDR_car_write(write_car_param,CAR_ID,0))
	{
        flag_car_data = flag_car_data + 1<<2;
	}
	sem_syslog_warning("**flag_car_data= %x\n",flag_car_data);
//test data link 5555
    write_car_param->reload = 0x555555;
    write_car_param->linkup = 0x1;
    write_car_param->car_valid = 0x1;
    write_car_param->usr_ip = 0x55555555;
	write_car_param->credit = 0x0;
    write_car_param->byte_drop_count = 0x5555555555ULL;
    write_car_param->byte_set_count =  0x5555555555ULL;
    write_car_param->package_drop_count = 0x55555555ULL;
    write_car_param->package_set_count = 0x55555555ULL;
	CAR_ID =1;
	if(test_QDR_car_write(write_car_param,CAR_ID,0))
	{
        flag_car_data = flag_car_data + 1<<3;
    }
	sem_syslog_warning("**flag_car_data= %x\n",flag_car_data);

//test address link 
//first test low one bit
    write_car_param->reload = 0x374628;
    write_car_param->linkup = 0x1;
    write_car_param->car_valid = 0x1;
    write_car_param->usr_ip = 0x16347694;
	write_car_param->credit = 0x0;
    write_car_param->byte_drop_count = 0x1234567891ULL;
    write_car_param->byte_set_count =  0x9876543219ULL;
    write_car_param->package_drop_count = 0x13467985ULL;
    write_car_param->package_set_count = 0x24689635ULL;
	CAR_ID =0;
	flag_car_addr_low = test_QDR_car_write(write_car_param,CAR_ID,1);
    sem_syslog_warning("**flag_car_addr_low= %x\n",flag_car_addr_low);
//second test diff car_id address
    for(i=0;i<17;i++)
    {
        write_car_param_base->reload = 0x555555;
        write_car_param_base->linkup = 0x1;
        write_car_param_base->car_valid = 0x1;
        write_car_param_base->usr_ip = 0x55555555;
		write_car_param_base->credit = 0x0;
        write_car_param_base->byte_drop_count = 0x5555555555ULL;
        write_car_param_base->byte_set_count = 0x5555555555ULL;
        write_car_param_base->package_drop_count = 0x55555555ULL;
        write_car_param_base->package_set_count = 0x55555555ULL;
        CAR_ID = 0;
        ret = test_QDR_car_write(write_car_param_base,CAR_ID,1);

        write_car_param_compare->reload = 0xaaaaaa;
        write_car_param_compare->linkup = 0x1;
        write_car_param_compare->car_valid = 0x1;
        write_car_param_compare->usr_ip = 0xaaaaaaaa;
		write_car_param_base->credit = 0x0;
        write_car_param_compare->byte_drop_count = 0xaaaaaaaaaaULL;
        write_car_param_compare->byte_set_count = 0xaaaaaaaaaaULL;
        write_car_param_compare->package_drop_count = 0xaaaaaaaaULL;
        write_car_param_compare->package_set_count = 0xaaaaaaaaULL;
		CAR_ID = 1 << i;
    	ret = test_QDR_car_write(write_car_param_compare,CAR_ID,1);

        CAR_ID = 0;
		ret = read_car(read_car_result,Indirect_add_reg_H,Indirect_add_reg_L,write_car_param->linkup,CAR_ID,1);

		if(0x5555555555ULL != read_car_result->byte_drop_count ||
		   0x5555555555ULL != read_car_result->byte_set_count ||
		   0x55555555ULL != read_car_result->package_drop_count ||
		   0x55555555ULL != read_car_result->package_set_count)
		{
			sem_syslog_warning("**read_car_result->byte_drop_count = %llx\n",read_car_result->byte_drop_count);
			sem_syslog_warning("**read_car_result->byte_set_count = %llx\n",read_car_result->byte_set_count);
			sem_syslog_warning("**read_car_result->package_drop_count = %llx\n",read_car_result->package_drop_count);
			sem_syslog_warning("**read_car_result->package_set_count = %llx\n",read_car_result->package_set_count);
			flag_car_addr += 1<<i; 
		}
		sem_syslog_warning("**i = %d;flag_car_addr= %x\n",i,flag_car_addr);
	}
#endif
	free(stat_base);
	free(stat_compare);
    free(write_cam_param_base);
    free(write_cam_param_compare);
	free(write_car_param);
    free(write_car_param_base);
    free(write_car_param_compare);
	free(read_car_result);

	reply = dbus_message_new_method_return(msg);
    
	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &flag_hash_data_1 );
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &flag_hash_data_2 );
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &flag_hash_addr_low_1 );
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &flag_hash_addr_1 );
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &flag_hash_addr_low_2 );
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &flag_hash_addr_2 );
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &flag_car_data);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &flag_car_addr_low);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &flag_car_addr);

	//sem_syslog_warning("sem_dbus_show_ram_detection success!\n");
	return reply;

}

DBusMessage *sem_dbus_show_fpga_reg_arr(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	bm_op_args opt;
	unsigned short val;
	unsigned short add;
	int ret = 0;
	
	sem_syslog_event("**sem_dbus_show_fpga_reg_arr!\n");
	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
		                        DBUS_TYPE_UINT16, &add,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		sem_syslog_dbg("sem_dbus_show_fpga_reg_arr get arg failed and reset normal\n");
	}

    memset(&opt,0,sizeof(bm_op_args));
	opt.op_len = 16;	
	opt.op_addr = FPGA_REG_BASE+add;
	ret = read_fpga_reg(&opt);
	if(ret)
	{
		sem_syslog_warning("**read device id register failure!\n");
		return NULL;
	}
    val = (unsigned short)opt.op_value ; 
    sem_syslog_event("**val = 0x%x!\n",val);

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT16,
								 &val);
	return reply;
}


DBusMessage *sem_dbus_show_fpga_running_config(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
    DBusMessage* reply;
    DBusMessageIter  iter;

	char *tmpPtr = NULL;
	char *showStr = NULL,*cursor = NULL;
	read_car_white_node_param_t *white_car_node_param = NULL;
	read_car_node_param_t *car_node_param = NULL;

    char ip_str[16];
    unsigned int car_node_ip;
	int length = 0;
	int i = 0;
	int j = 0;
	int ret = 0;
	int read_white_car =0;

	showStr = (char *)malloc(sizeof(char)*128*(car_white_count+car_count));
	if(showStr != NULL)
	{	
        cursor = showStr;
        white_car_node_param = (read_car_white_node_param_t *)malloc(sizeof(read_car_white_node_param_t));
        if(white_car_node_param != NULL)
        {
        	//sem_syslog_warning("function:sem_dbus_show_fpga_running_config.malloc car_node_param success!\n");
        	if(car_white_head != NULL)
        	{
                for (i = 1; i <= car_white_count; i++)
        		{
                    char tmpBuf[128] = {0};
                    sem_syslog_warning("function:sem_dbus_show_fpga_running_config.i = %d\n",i);
        			memset(white_car_node_param,0,sizeof(read_car_white_node_param_t));
                    read_white_car = read_car_white_linklist(i,white_car_node_param);
                    if(!read_white_car)
                    {
                    	sem_syslog_warning("function:sem_dbus_show_fpga_running_config.read_car_white_linklist %d node fail\n",i);
                    }
        			else
        			{
            			car_node_ip = htonl(white_car_node_param->usr_ip);
                	 	if(inet_ntop(AF_INET,&car_node_ip,ip_str,16) == NULL)
                	 	{
                           sem_syslog_warning("ip conversion error \n");
                		}
    					
            			sem_syslog_warning("function:sem_dbus_show_fpga_running_config.ip_str = %s,link = %s\n",ip_str,white_car_node_param->linkup ? "uplink":"downlink");
                    	length += sprintf(tmpBuf,"set fpga car-white-list slot %d add ip %s %s num 1\n",local_board->slot_id+1,ip_str,white_car_node_param->linkup ? "uplink":"downlink");
            			sem_syslog_warning("function:sem_dbus_show_fpga_running_config.length = %d slot id = %d\n",length,local_board->slot_id+1);

    					sprintf(cursor,"%s",tmpBuf);
            			sem_syslog_warning("i = %d;cursor = %s\n",i,cursor);
            			cursor = cursor + length;
        			}
                }
				sem_syslog_warning("after for i\n");
				free(white_car_node_param);
        	}
        	else
        	{
        	    sem_syslog_warning("function:sem_dbus_show_fpga_running_config.car white list is empty!\n");
        	    free(white_car_node_param);
        	}
        }
        else
        {
            sem_syslog_warning("function:sem_dbus_show_fpga_running_config.malloc read_car_node_param_t fail!\n");
			showStr = NULL;
        }
	}
	else
	{
        sem_syslog_warning("function:sem_dbus_show_fpga_running_config.malloc showStr fail!\n");
	}
	sem_syslog_warning("will reply\n");
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_STRING,
									 &showStr);
	sem_syslog_warning("will free(showStr);\n");
	free(showStr);
	showStr = NULL;
	cursor = NULL;
	return reply;
}

DBusMessage *sem_dbus_set_fpga_reset(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	int reg_add;
	int reg_val;
	int ret = 0;
	int flag = 1;
	
	sem_syslog_dbg("**sem_dbus_set_fpga_reset!\n");
	dbus_error_init(&err);

    ret = sem_write_cpld(0x06,0xef);
    if(ret)
    {
        sem_syslog_warning("**write cpld address = 0x06;val = 0xef failed!ret = %d\n",ret);
		flag = 0;
	}
	sleep(1);
    ret = sem_write_cpld(0x06,0xff);
    if(ret)
    {
        sem_syslog_warning("**write cpld address = 0x06 val = 0xff failed!ret = %d\n",ret);
		flag = 0;
	}	

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_INT32,
								 &flag);
	return reply;
}

DBusMessage *sem_dbus_write_fpga(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	int reg_add;
	int reg_val;
	int ret = 0;
	bm_op_args opt;

	sem_syslog_dbg("**sem_dbus_write_fpga!\n");
	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
		                        DBUS_TYPE_INT32, &reg_add,
		                        DBUS_TYPE_INT32, &reg_val,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		sem_syslog_dbg("sem_dbus_write_cpld get arg failed and reset normal\n");
	}
	
	memset(&opt,0,sizeof(bm_op_args));	
	opt.op_len = 16;
	opt.op_value = reg_val;
	opt.op_addr = FPGA_REG_BASE + reg_add;
	ret = write_fpga_reg(&opt);
    if(ret)
    {
        sem_syslog_dbg("write fpga reg address:0x%x,val:0x%x",reg_add,reg_val);
	}

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_INT32,
								 &ret);
	return reply;
}

DBusMessage *sem_dbus_write_cpld(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	int reg_add;
	int reg_val;
	int ret = 0;
	
	sem_syslog_dbg("**sem_dbus_write_cpld!\n");
	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
		                        DBUS_TYPE_INT32, &reg_add,
		                        DBUS_TYPE_INT32, &reg_val,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		sem_syslog_dbg("sem_dbus_write_cpld get arg failed and reset normal\n");
	}
	
    ret = sem_write_cpld(reg_add,reg_val);
    if(ret)
    {
        sem_syslog_warning("**write cpld address = 0x%x;val = 0x%x! failed\n",reg_add,reg_val);
	}

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_INT32,
								 &ret);
	return reply;
}

DBusMessage *sem_dbus_read_cpld(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusMessageIter  iter;
	DBusError err;
	int reg_add;
	int reg_val;
	int ret = 0;
	
	sem_syslog_dbg("**sem_dbus_read_cpld!\n");
	dbus_error_init(&err);

	if (!(dbus_message_get_args(msg, &err,
		                        DBUS_TYPE_INT32, &reg_add,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			sem_syslog_dbg("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		
		sem_syslog_dbg("sem_dbus_read_cpld get arg failed and reset normal\n");
	}

    ret = sem_read_cpld(reg_add,&reg_val);
    if(ret)
    {
        sem_syslog_warning("**write cpld address = 0x%x;val = 0x%x!failed\n",reg_add,reg_val);
	}

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_INT32,
								 &ret);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_INT32,
								 &reg_val);
	return reply;
}
#ifdef __cplusplus
extern "C"
}
#endif

