/*******************************************************************************
Copyright (C) Autelan Technology


This software file is owned and distributed by Autelan Technology 
********************************************************************************


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
********************************************************************************
* ws_sysinfo.c
*
*
* CREATOR:
* autelan.software.Network Dep. team
* zhouym@autelan.com
*
* DESCRIPTION:
* function for web
*
*
***************************************************************************/
#ifdef __cplusplus
	extern "C"
	{
#endif

#include "ws_sysinfo.h"
/*#include "cgic.h"*/
#include <sys/ioctl.h>
#include <stdio.h>
#include "ws_usrinfo.h"
//#include "ws_ec.h"
#include <string.h>
#include <fcntl.h> 
#include <sys/stat.h> 
#include <stdlib.h>
#include "ws_public.h"
int show_file_string(char *FILENAME,char *zstr);


//extern char *slot_status_str[MODULE_STAT_MAX];

#if 0
char *slot_status_str[MODULE_STAT_MAX] = {
	"NONE",
	"INITING",
	"RUNNING",
	"DISABLED"
};
#endif


int show_sys_ver(struct sys_ver *SysV) /*��ʾϵͳ��Ϣ���ɹ�����0��ʧ�ܷ���-1*/
{
	DBusMessage *query, *reply;
	DBusError err;

    int retu=0;
	unsigned int product_id;
	unsigned int sw_version;
	char *product_name = NULL;
	char *base_mac = NULL;
	char *serial_no = NULL;
	char *swname = NULL;

	query = dbus_message_new_method_call(NPD_DBUS_BUSNAME,NPD_DBUS_OBJPATH,NPD_DBUS_INTERFACE,NPD_DBUS_INTERFACE_METHOD_VER);

	dbus_error_init(&err);
	reply = dbus_connection_send_with_reply_and_block (ccgi_dbus_connection,query,-1, &err);
	
	dbus_message_unref(query);
	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		return -1;
	}

	if (dbus_message_get_args ( reply, &err,
		DBUS_TYPE_UINT32,&product_id,
		DBUS_TYPE_UINT32,&sw_version,
		DBUS_TYPE_STRING,&product_name,
		DBUS_TYPE_STRING,&base_mac,
		DBUS_TYPE_STRING,&serial_no,
		DBUS_TYPE_STRING,&swname,
		DBUS_TYPE_INVALID)) {	
		SysV->swname=(char *)malloc(50);
		if(SysV->swname)
		{
			memset(SysV->swname,0,50);
			strcpy(SysV->swname,swname);
		}
		SysV->sw_version=sw_version;
		//add 2009.2.27
		SysV->product_id=product_id;
		//changed 2008.6.26.pm
		//if (SW_COMPATIBLE_VER(sw_version) > 0 ) {}

		SysV->product_name=(char *)malloc(50);
		if(SysV->product_name)
		{
			memset(SysV->product_name,0,50);
			strcpy(SysV->product_name,product_name);
		}
		SysV->base_mac=(char *)malloc(50);
		if(SysV->base_mac)
		{
			memset(SysV->base_mac,0,50);
			strcpy(SysV->base_mac,base_mac);
		}
		SysV->serial_no=(char *)malloc(50);
		if(SysV->serial_no)
		{
			memset(SysV->serial_no,0,50);
			strcpy(SysV->serial_no,serial_no);
		}
		//software name
		{
        	char software_name_file[256]="";

        	if(access("/devinfo/software_name",0)==0)
            {
            	sprintf( software_name_file, "if [ -f /devinfo/software_name ];then cat /devinfo/software_name 2>/dev/null;fi" );
            	GET_CMD_STDOUT(SysV->sw_name,sizeof(SysV->sw_name),software_name_file);
            }
			
        }

		

		//product name
		{
        	char product_name_file[256]="";

        	if(access("/devinfo/product_name",0)==0)
    		{
            	sprintf( product_name_file, "if [ -f /devinfo/product_name ];then cat /devinfo/product_name 2>/dev/null;fi" );
            	GET_CMD_STDOUT(SysV->sw_product_name,sizeof(SysV->sw_product_name),product_name_file);
            }
		}

		
			
		//version
        {
        	char get_sw_ver_cmd[256]="";

        	if(access("/etc/version/verstring",0)==0)
            {
            	//sprintf( get_sw_ver_cmd, "if [ -f /etc/version/verstring ];then cat /etc/version/verstring 2>/dev/null;fi" );
            	sprintf( get_sw_ver_cmd, "if [ -f /etc/version/rversion ];then cat /etc/version/rversion 2>/dev/null;fi" );
            	GET_CMD_STDOUT(SysV->sw_version_str,sizeof(SysV->sw_version_str),get_sw_ver_cmd);
            }
        }


		
			
		//mac address
		{
        	char mac_address_file[256]="";
			
        	if(access("/devinfo/mac",0)==0)
            {
            	sprintf( mac_address_file, "if [ -f /devinfo/mac ];then cat /devinfo/mac 2>/dev/null;fi" );
            	GET_CMD_STDOUT(SysV->sw_mac,sizeof(SysV->sw_mac),mac_address_file);
            }
        }

		
			
		//serial number
		{
        	char serial_num_file[256]="";
			
        	if(access("/devinfo/sn",0)==0)
            {
            	sprintf( serial_num_file, "if [ -f /devinfo/sn ];then cat /devinfo/sn 2>/dev/null;fi" );
            	GET_CMD_STDOUT(SysV->sw_serial_num,sizeof(SysV->sw_serial_num),serial_num_file);
            }
        }
		
		
	} else {
		if (dbus_error_is_set(&err)) {
				dbus_error_free(&err);
		}
		retu=-1;
	}
		
	dbus_message_unref(reply);
	return retu;
}


/*---------------------------------------------------------
DEFUN(show_sys_hwconf_cmd_func,
	show_sys_hwconf_cmd,
	"show system hardware-configuration",
	SHOW_STR
	"Display system information\n"
	"Display system hardware-configuration information\n"
	)
  ---------------------------------------------------------
*/
#if 0

int show_hardware_configuration()
{
	DBusMessage *query, *reply;
	DBusError err;
	DBusMessageIter	 iter;
	DBusMessageIter	 iter_array;
	unsigned char slot_count = 0;
	int i;

	query = dbus_message_new_method_call(NPD_DBUS_BUSNAME,NPD_DBUS_OBJPATH,NPD_DBUS_INTERFACE,NPD_DBUS_INTERFACE_METHOD_HWCONF);
	

	dbus_error_init(&err);
	reply = dbus_connection_send_with_reply_and_block (ccgi_dbus_connection,query,-1, &err);
	
	dbus_message_unref(query);
	if (NULL == reply) {
		//vty_out(vty,"failed get reply.\n");
		if (dbus_error_is_set(&err)) {
			//printf("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return -1;
	}

	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&slot_count);
	
	fprintf(cgiOut,"<tr height=25 bgcolor=#eaeff9 style=font-size:14px>");
	if (1 == slot_count) {
		//vty_out(vty,"%-21s","PRODCTNAME");
		fprintf(cgiOut,"<th  style=font-size:12px>%s</th>","PRODCTNAME");
	} else {
    	//vty_out(vty,"%-5s%-10s%-20s","SLOT","STATUS","MODULE NAME");
		fprintf(cgiOut,"<th  style=font-size:12px>%s</th>","SLOT");
		fprintf(cgiOut,"<th  style=font-size:12px>%s</th>","STATUS");
		fprintf(cgiOut,"<th  style=font-size:12px>%s</th>","MODULE NAME");
	}
	//vty_out(vty,"%-22s%-6s%-11s\n","SN","HWVER","EXT SLOTNUM");
	//fprintf(cgiOut,"<th  style=font-size:12px>%s</th>","SN");
	//fprintf(cgiOut,"<th  style=font-size:12px>%s</th>","HWVER");
	fprintf(cgiOut,"<th  style=font-size:12px>%s</th>","ICM");
	fprintf(cgiOut,"<th  style=font-size:12px>%s</th>","EXT SLOTNUM");
	fprintf(cgiOut,"</tr>");

	
	dbus_message_iter_next(&iter);
	
	dbus_message_iter_recurse(&iter,&iter_array);

	for (i = 0; i < slot_count; i++) {
		DBusMessageIter iter_struct;
		unsigned char slotno;
		unsigned int module_id;
		unsigned char module_status;
		unsigned char hw_ver;
		unsigned char ext_slot_num;
		char *sn;
		char *modname;
		
		
		dbus_message_iter_recurse(&iter_array,&iter_struct);
		
		dbus_message_iter_get_basic(&iter_struct,&slotno);
		dbus_message_iter_next(&iter_struct);
		dbus_message_iter_get_basic(&iter_struct,&module_id);
		dbus_message_iter_next(&iter_struct);
		dbus_message_iter_get_basic(&iter_struct,&module_status);
		dbus_message_iter_next(&iter_struct);
		dbus_message_iter_get_basic(&iter_struct,&hw_ver);
		dbus_message_iter_next(&iter_struct);
		dbus_message_iter_get_basic(&iter_struct,&ext_slot_num);
		dbus_message_iter_next(&iter_struct);
		dbus_message_iter_get_basic(&iter_struct,&sn);
		dbus_message_iter_next(&iter_struct);
		dbus_message_iter_get_basic(&iter_struct,&modname);
		
		dbus_message_iter_next(&iter_array);
		fprintf(cgiOut,"<tr height=25 >");
		if (1 == slot_count) {
			//vty_out(vty," %-20s",modname);
			char key[128];
			memset(key,0,128);
			show_file_string(DEVINFO_PRODUCT_NAME,key); 
			fprintf(cgiOut,"<td align=center>%s</td>",key);
						
			

		} else {
			//vty_out(vty," %-4d%-10s%-20s",slotno,slot_status_str[module_status],(MODULE_STAT_NONE != module_status) ? modname:"-");
			fprintf(cgiOut,"<td align=center>%d</td>",slotno);
			fprintf(cgiOut,"<td align=center>%s</td>",slot_status_str[module_status]);
			fprintf(cgiOut,"<td align=center>%s</td>",(MODULE_STAT_NONE != module_status) ? modname:"-");
		}
		if(MODULE_STAT_NONE == module_status) {
			//vty_out(vty,"%-22s%-6s%-11s\n","-","-","-");
			fprintf(cgiOut,"<td align=center>%s</td>","-");
			fprintf(cgiOut,"<td align=center>%s</td>","-");
			//fprintf(cgiOut,"<td align=center>%s</td>","-");
		}
		else {
			//vty_out(vty,"%20s##%02d.%02d %-11d\n",(MODULE_STAT_NONE != module_status) ? sn:"-",HW_PCB_VER(hw_ver),HW_CPLD_VER(hw_ver),ext_slot_num);

			fprintf(cgiOut,"<td align=center>%20s##%02d.%02d</td>",(MODULE_STAT_NONE != module_status) ? sn:"-",HW_PCB_VER(hw_ver),HW_CPLD_VER(hw_ver));
			fprintf(cgiOut,"<td align=center>%d</td>",ext_slot_num);
		}
	}
		

	dbus_message_unref(reply);
	
	
	return 0;
}
#endif

/*-------------------------------------
DEFUN(show_running_boot_img_func,
	show_running_boot_img_func_cmd,
	"show system boot_img",
	SHOW_STR
	"Show system infomation\n"
	"Show running boot img infomation\n"
	)
  -------------------------------------
	*/
int show_system_bootimg(struct sys_img *bootimg)
{
	char imgname[128] ;
	int ret;

	memset(imgname,0,128);
	ret = get_bootimg_name(imgname);
	if(0==ret)
	{
		//vty_out(vty,"The current boot file is %s\n",imgname);
	
		//bootimg->boot_img=imgname;	

		memset(bootimg->boot_img,0,50);
	    strcpy(bootimg->boot_img,imgname);
		
		return 0;
	}
	else if(1==ret)
	{
		//vty_out(vty,"Can't open the file\n");
		return 1;
	
	}
	else
	{
		//vty_out(vty,"Can't get the file name\n");
		return 2;
	}

}

    

//��д�µĺ�������ȡ��ƷID

 int get_product_id()
{
	DBusMessage *query, *reply;
	DBusError err;

   // int retu=0;
	unsigned int product_id;
	unsigned int sw_version;
	char *product_name = NULL;
	char *base_mac = NULL;
	char *serial_no = NULL;
	char *swname = NULL;

	query = dbus_message_new_method_call(NPD_DBUS_BUSNAME,NPD_DBUS_OBJPATH,NPD_DBUS_INTERFACE,NPD_DBUS_INTERFACE_METHOD_VER);

	dbus_error_init(&err);
	reply = dbus_connection_send_with_reply_and_block (ccgi_dbus_connection,query,-1, &err);
	
	dbus_message_unref(query);
	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		//return -1;
	}

	if (dbus_message_get_args ( reply, &err,
		DBUS_TYPE_UINT32,&product_id,
		DBUS_TYPE_UINT32,&sw_version,
		DBUS_TYPE_STRING,&product_name,
		DBUS_TYPE_STRING,&base_mac,
		DBUS_TYPE_STRING,&serial_no,
		DBUS_TYPE_STRING,&swname,
		DBUS_TYPE_INVALID)) {	
		
		//pro->product_id=product_id;
		
		
	} else {
		if (dbus_error_is_set(&err)) {
				dbus_error_free(&err);
		}
		//retu=-1;
	}
		
	dbus_message_unref(reply);
	//return retu;
	return product_id;
}


int get_bootimg_name(char* imgname)
{
	int fd; 
	//FILE *fd;
	int retval; 
	boot_env_t	env_args;
	char *name = "bootfile";

	memset( &env_args, 0, sizeof(env_args) );
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
			return 2;	
		}
	else
		{		
			sprintf(imgname,env_args.value); 
		}
		close(fd);
		return 0;


}

//��ΪǶ��ҳ����룬show_hardware_configuration�޷��ٴε��ã������д
int show_sys_hwconf( hw_info * sys_head,int *slot_number)//if return SN equals with "-",ex_slotnum print "-",return 0 means successful,-1 means fail
{
	DBusMessage *query, *reply;
	DBusError err;
	DBusMessageIter	 iter;
	DBusMessageIter	 iter_array;
	unsigned char slot_count = 0;
	int i;
	hw_info *q,*tail;
	query = dbus_message_new_method_call(NPD_DBUS_BUSNAME,NPD_DBUS_OBJPATH,NPD_DBUS_INTERFACE,NPD_DBUS_INTERFACE_METHOD_HWCONF);
	

	dbus_error_init(&err);
	reply = dbus_connection_send_with_reply_and_block (ccgi_dbus_connection,query,-1, &err);
	
	dbus_message_unref(query);
	if (NULL == reply) 
	{
		//vty_out(vty,"failed get reply.\n");
		if (dbus_error_is_set(&err)) 
		{
			//printf("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return -1;
	}

	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&slot_count);
	#if 0
	if (1 == slot_count) 
	{
		vty_out(vty,"%-21s","PRODCTNAME");
	} 
	else 
	{
    		vty_out(vty,"%-5s%-10s%-20s","SLOT","STATUS","MODULE NAME");
	}
	vty_out(vty,"%-22s%-6s%-12s\n","SN","HWVER","EXT SLOTNUM");
	if (1 == slot_count) {
		vty_out(vty,"%-21s%-22s%-6s%-12s\n","--------------------","---------------------","-----","-----------");
	}
	else
		vty_out(vty,"%-5s%-10s%-20s%-22s%-6s%-12s\n","----","---------","-------------------","---------------------","-----","-----------");
	#endif
	dbus_message_iter_next(&iter);
	
	dbus_message_iter_recurse(&iter,&iter_array);
	*slot_number = slot_count;
	sys_head->next = NULL;
	tail = sys_head;
	
	for (i = 0; i < slot_count; i++) 
	{
		DBusMessageIter iter_struct;
		unsigned char slotno;
		unsigned int module_id;
		unsigned char module_status;
		unsigned char hw_ver;
		unsigned char ext_slot_num;
		char *sn;
		char *modname;
		
		
		dbus_message_iter_recurse(&iter_array,&iter_struct);
		
		dbus_message_iter_get_basic(&iter_struct,&slotno);
		dbus_message_iter_next(&iter_struct);
		dbus_message_iter_get_basic(&iter_struct,&module_id);
		dbus_message_iter_next(&iter_struct);
		dbus_message_iter_get_basic(&iter_struct,&module_status);
		dbus_message_iter_next(&iter_struct);
		dbus_message_iter_get_basic(&iter_struct,&hw_ver);
		dbus_message_iter_next(&iter_struct);
		dbus_message_iter_get_basic(&iter_struct,&ext_slot_num);
		dbus_message_iter_next(&iter_struct);
		dbus_message_iter_get_basic(&iter_struct,&sn);
		dbus_message_iter_next(&iter_struct);
		dbus_message_iter_get_basic(&iter_struct,&modname);
		
		dbus_message_iter_next(&iter_array);
		q=(hw_info *)malloc(sizeof(hw_info));
		memset(q,0,sizeof(hw_info));
		if (1 == slot_count) 
		{
			//vty_out(vty," %-20s",modname);
			q->slot = 0;
			strcpy(q->moduleName,modname);
			
		} 
		else 
		{
			q->slot = slotno;
			strcpy(q->hw_status,slot_status_str[module_status]);
			strcpy(q->moduleName,(MODULE_STAT_NONE != module_status) ? modname:"-");
			//vty_out(vty," %-4d%-10s%-20s",slotno,slot_status_str[module_status],(MODULE_STAT_NONE != module_status) ? modname:"-");
		}
		
		if(MODULE_STAT_NONE == module_status) 
		{
			strcpy(q->SN,"-");
			strcpy(q->hw_ver,"-");
			q->ext_slotnum = 0;
			//vty_out(vty,"%-22s%-6s%-11s\n","-","-","-");
		}
		else 
		{
			strcpy(q->SN,sn);
			sprintf(q->hw_ver,"%d.%d",HW_PCB_VER(hw_ver),HW_CPLD_VER(hw_ver));
			q->ext_slotnum = ext_slot_num;
			//vty_out(vty,"%-22s% 2d.%-2d %-11d\n",(MODULE_STAT_NONE != module_status) ? sn:"-",HW_PCB_VER(hw_ver),HW_CPLD_VER(hw_ver),ext_slot_num);
		}

		q->next = NULL;
		tail->next = q;
		tail = q;
	}
		

	dbus_message_unref(reply);
	
	
	return 0;
}
void free_hw_head(hw_info * hw_head)
{
	hw_info *f1,*f2;
	f1=hw_head->next;		 /*�ͷ�hw��Ϣ����Ŀռ�*/
	f2=f1->next;
	while(f2!=NULL)
	{
		free(f1);
		f1=f2;
		f2=f2->next;
	}
	free(f1);
}


//add new function show system performace


char* get_token(char *str_tok, unsigned int *pos)
{
    unsigned int temp = *pos;
    while(isspace(*(str_tok+temp)) && (*(str_tok+temp) != '\0'))
    {
        temp++;
    }
    while(!isspace(*(str_tok+temp)) && (*(str_tok+temp) != '\0'))
    {
        temp++;
    }
    while(isspace(*(str_tok+temp)) && (*(str_tok+temp) != '\0'))
    {
        temp++;
    }
    *pos = temp;
    return (char *)(str_tok+temp);
}

int get_cpu_status(unsigned int *cpu_times, unsigned int *count)
{
	FILE *fp = NULL;
	char buffer[BUF_LEN];
	char *iter = NULL;
	unsigned int begin = 0;
	unsigned int cpu_no = 0;
	unsigned int cpu_status = 0;
	(*count) = 0;

	memset(buffer, 0, sizeof(buffer));
	memset(cpu_times, 0, sizeof(unsigned int)*MAX_CPU*CPU_STATUS);

	if(NULL != (fp = fopen(cp_cpu_info,"r")))
	{  
		cpu_no = 0;
		cpu_status = 0;
		while( (NULL != fgets(buffer, BUF_LEN, fp)) && cpu_no < MAX_CPU )
		{    
			iter = buffer;
			begin = 0;
			cpu_status = 0;
			if(buffer[begin]=='c' && buffer[begin+1]=='p' && buffer[begin+2]=='u' )
			{
				if( buffer[begin+3]!=' ')
				{   
					(*count)++;
					get_token(buffer, &begin);
					iter += begin;
					while(cpu_status < CPU_STATUS)
					{
						*(cpu_times+cpu_no*CPU_STATUS+(cpu_status++)) = strtoul(iter, &iter, 0);
					}
					cpu_no ++;
				}
			}
			else
			{
				break;
			}
		}
		fclose(fp);
		return SUCCESS;
	}
	return SYS_ERR;
}

int get_cpu_usage(P_SYS_INFO pst_sys_info, unsigned int refresh )
{
	unsigned int cpu_no = 0;
	unsigned int cpu_stu = 0;
	int total_time = 0;
	unsigned int *cpu_time_old = NULL;
	unsigned int *cpu_time_now = NULL; 
	float *cpu_usage = NULL;
	int *diffs_temp = NULL;
	unsigned int time_old[MAX_CPU][CPU_STATUS];
	unsigned int time_cur[MAX_CPU][CPU_STATUS];
	int time_diffs[CPU_STATUS];
	unsigned int count_1 = 0;
	unsigned int count_2 = 0;

	memset((void *)time_old, 0, MAX_CPU*CPU_STATUS*sizeof(unsigned int));
	memset((void *)time_cur, 0, MAX_CPU*CPU_STATUS*sizeof(unsigned int));
	memset((void *)time_diffs, 0, CPU_STATUS*sizeof(unsigned int));

	if(SUCCESS == get_cpu_status((unsigned int *)time_old, &count_1))
	{   
		sleep(refresh);
		if(SUCCESS == get_cpu_status((unsigned int *)time_cur, &count_2))
		{
			pst_sys_info->pst_cpus_status->cpu_no = MIN(count_1, count_2);
			for(;cpu_no<(pst_sys_info->pst_cpus_status->cpu_no);cpu_no++)
			{
				cpu_stu = 0;
				total_time = 0;
				diffs_temp = time_diffs;
				cpu_time_old = (unsigned int *)time_old + CPU_STATUS*cpu_no;
				cpu_time_now = (unsigned int *)time_cur + CPU_STATUS*cpu_no;
				cpu_usage = (float *)pst_sys_info->pst_cpus_status->ar_cpu_usage + CPU_STATUS*cpu_no;
				memset(diffs_temp, 0, sizeof(int)*CPU_STATUS);
				for(;cpu_stu<CPU_STATUS;cpu_stu++)
				{
					*diffs_temp = (int)((*cpu_time_now++)-(*cpu_time_old++));
					total_time += *diffs_temp++;
				}
				total_time = (total_time == 0)?1:total_time;
				cpu_stu = 0;
				diffs_temp = time_diffs;
				for(;cpu_stu<CPU_STATUS;cpu_stu++)
				{
					*cpu_usage++ = (*diffs_temp++)*100.0/total_time;
				}
			}
		}
		return SYS_ERR;
	}
	return SYS_ERR;
}

int get_mem_usage( P_SYS_INFO pst_sys_info )
{
    FILE *fp = NULL;
    char buffer[BUF_LEN];
    char *iter = NULL;
    unsigned int begin = 0;
    
    memset(buffer, 0, sizeof(buffer));
    if(NULL != (fp = fopen(cp_mem_info,"r")))
    {  
        if(NULL == fgets(buffer, BUF_LEN, fp))
        {
        	fclose(fp);
            return SYS_ERR;
        }
        get_token(buffer, &begin);
        iter = buffer + begin;
        pst_sys_info->pst_mem_status->un_mem_total = strtoul(iter, &iter, 0);

        if(NULL == fgets(buffer, BUF_LEN, fp))
        {
        	fclose(fp);
            return SYS_ERR;
        }
        begin = 0;
        get_token(buffer, &begin);
        iter = buffer + begin;
        pst_sys_info->pst_mem_status->un_mem_free = strtoul(iter, &iter, 0);

        if(NULL == fgets(buffer, BUF_LEN, fp))
        {
        	fclose(fp);
            return SYS_ERR;
        }
        begin = 0;
        get_token(buffer, &begin);
        iter = buffer + begin;
        pst_sys_info->pst_mem_status->un_mem_buffers = strtoul(iter, &iter, 0);

        pst_sys_info->pst_mem_status->un_mem_used = pst_sys_info->pst_mem_status->un_mem_total - pst_sys_info->pst_mem_status->un_mem_free;
        
        fclose(fp);
        return SUCCESS;
    }
    return SYS_ERR;

}

int get_cpu_temperature( P_SYS_INFO pst_sys_info )
{
    struct sys_envir sys;
    memset((struct sys_envir *)&sys, 0, sizeof(struct sys_envir));
    
    ccgi_dbus_init();  //�ô˺����ˣ��ӵײ�ȡֵ�˰� 
	if( show_sys_envir(&sys) == CCGI_SUCCESS)
	{
	      pst_sys_info->pst_cpu_temperature->un_tem_core = (unsigned int)sys.core_tmprt;
		pst_sys_info->pst_cpu_temperature->un_tem_sface = (unsigned int)sys.surface_tmprt;

	    return SUCCESS;
	}
    return SYS_ERR;
}


int get_logo(char * logo)
{
	FILE * get_info;
	get_info = popen("cat /devinfo/enterprise_name | sed \"2,200d\"","r");
	if(get_info != NULL)
	{
		fgets(logo,50,get_info);
		pclose(get_info);
	}
	return 0;
}

//write to boorom

int write_to_boorom(char *path)
{
	int fd;
	bootrom_file bootrom;
	int retval;
	memset(bootrom.path_name,0,sizeof(bootrom.path_name));
	strcpy(bootrom.path_name,path);
#if 1
	fd = open("/dev/bm0",0);
	retval = ioctl(fd,BM_IOC_BOOTROM_EXCH,&bootrom);
	if(retval < 0){
		//vty_out(vty,"update bootrom failed return [%d]\n",retval);
		close(fd);
		return retval;
	}	
	close(fd);
#endif
	return 0;

}

int show_file_string(char *FILENAME,char *zstr)
{
	char buff[256];
	int len,i,fd;
	char temp[10];
	char zstring[128];
	memset(zstring,0,128);
	
	fd = open(FILENAME,O_RDONLY);
	if (fd < 0) {
		//vty_out(vty,"\r\nFailed to open file.\r\n");
		return 1;
	}	
	len = read(fd,buff,256);	
	for (i=0;i<len; i++) {
		if (buff[i] != '\n') 
        memset(temp,0,10);
		sprintf(temp,"%c",buff[i]);
		strcat(zstring,temp);
	}	
	memcpy(zstr,zstring,strlen(zstring));
	
	close(fd);
	return 0;

}
#ifdef __cplusplus
}
#endif


