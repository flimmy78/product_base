/* cgicTempDir is the only setting you are likely to need
	to change in this file. */

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
* ws_dcli_ap_group.c
*
*
* CREATOR:
* autelan.software.Network Dep. team
* qiaojie@autelan.com
*
* DESCRIPTION:
*
*
*
*******************************************************************************/


#ifdef __cplusplus
extern "C"
{
#endif

#include "ws_dcli_ap_group.h"
#include <syslog.h>
#include <sys/wait.h>
#include <dirent.h>


int create_ap_group_cmd(int instance_id,char *ap_g_id,char *ap_g_name)/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																		   /*����-2��ʾap_g_id should be 1 to WTP_GROUP_NUM-1������-3��ʾname is too long,out of the limit of 128*/
																		   /*����-4��ʾid exist������-5��ʾerror*/
{
	if((NULL == ap_g_id)||(NULL == ap_g_name))
		return 0;
	
	int ret,len;
	unsigned char isAdd;	
	unsigned int id;
	char *name;
	DBusMessage *query, *reply;	
	DBusMessageIter	 iter;
	DBusError err;
	isAdd = 1;
	int retu = 0;
	
	ret = parse_int_ID((char*)ap_g_id, &id);
	if(ret != WID_DBUS_SUCCESS){
		return -1;
	}	
	if(id >= WTP_GROUP_NUM || id == 0){
		return -2;
	}
	len = strlen(ap_g_name);
	if(len > 128){		
		return -3;
	}
	name = (char*)malloc(strlen(ap_g_name)+1);
	if(NULL == name)
		return 0;
	memset(name, 0, strlen(ap_g_name)+1);
	memcpy(name, ap_g_name, strlen(ap_g_name));		
	
	int index;
	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];
	/*if(vty->node == CONFIG_NODE){
		index = 0;
	}else if(vty->node == HANSI_NODE){
		index = vty->index;
	}*/
	index = instance_id;
	
	ReInitDbusPath(index,WID_DBUS_BUSNAME,BUSNAME);
	ReInitDbusPath(index,WID_DBUS_AP_GROUP_OBJPATH,OBJPATH);
	ReInitDbusPath(index,WID_DBUS_AP_GROUP_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,WID_DBUS_AP_GROUP_METHOD_CREATE);

	dbus_error_init(&err);

	dbus_message_append_args(query,
						DBUS_TYPE_UINT32,&id,
						DBUS_TYPE_STRING,&name,
						DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (ccgi_dbus_connection,query,-1, &err);
	
	dbus_message_unref(query);
	
	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		if(name)
		{
			free(name);
			name = NULL;
		}
		return 0;
	}
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == 0)
	{
		retu = 1;
	}
	else if(ret == WLAN_ID_BE_USED)
	{
		retu = -4;
	}
	else
	{
		retu = -5;
	}

	dbus_message_unref(reply);
	FREE_OBJECT(name);
	return retu;	
}

int del_ap_group_cmd(int instance_id,char *ap_g_id)/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
													    /*����-2��ʾap_g_id should be 1 to WTP_GROUP_NUM-1������-3��ʾap group id does not exist*/
													    /*����-4��ʾerror*/
{
	if(NULL == ap_g_id)
		return 0;
	
	int ret;
	unsigned int id;
	DBusMessage *query, *reply;	
	DBusMessageIter	 iter;
	DBusError err;	
	int retu = 0;
	
	ret = parse_int_ID((char*)ap_g_id, &id);
	if(ret != WID_DBUS_SUCCESS){
		return -1;
	}	
	if(id >= WTP_GROUP_NUM || id == 0){
		return -2;
	}
	
	int index;
	char BUSNAME[PATH_LEN];
	char OBJPATH[PATH_LEN];
	char INTERFACE[PATH_LEN];
	/*if(vty->node == CONFIG_NODE){
		index = 0;
	}else if(vty->node == HANSI_NODE){
		index = vty->index;
	}*/
	index = instance_id;
	
	ReInitDbusPath(index,WID_DBUS_BUSNAME,BUSNAME);
	ReInitDbusPath(index,WID_DBUS_AP_GROUP_OBJPATH,OBJPATH);
	ReInitDbusPath(index,WID_DBUS_AP_GROUP_INTERFACE,INTERFACE);
	query = dbus_message_new_method_call(BUSNAME,OBJPATH,INTERFACE,WID_DBUS_AP_GROUP_METHOD_DEL);

	dbus_error_init(&err);

	dbus_message_append_args(query,
						DBUS_TYPE_UINT32,&id,
						DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (ccgi_dbus_connection,query,-1, &err);
	
	dbus_message_unref(query);
	
	if (NULL == reply) {
		if (dbus_error_is_set(&err)) {
			dbus_error_free(&err);
		}
		return 0;
	}
	dbus_message_iter_init(reply,&iter);
	dbus_message_iter_get_basic(&iter,&ret);

	if(ret == 0)
	{
		retu = 1;
	}
	else if(ret == WLAN_ID_NOT_EXIST)
	{
		retu = -3;
	}
	else
	{
		retu = -4;
	}

	dbus_message_unref(reply);

	return retu;	
}

/*operΪ"add"��"delete"*/
/*wtp_id_listΪ"all"��AP ID�б���ʽΪ1,8,9-20,33*/
int add_del_ap_group_member_cmd(int instance_id,int ap_g_id,char *oper,char *wtp_id_list)/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown command*/
																								  /*����-2��ʾset wtp list error,like 1,8,9-20,33������-3��ʾap group id�Ƿ�*/
																							      /*����-4��ʾap group id does not exist������-5��ʾerror*/
{
	if((NULL == oper)||(NULL == wtp_id_list))
		return 0;
	
	int ret;
	unsigned int isadd = 1;	
	unsigned int GROUPID = 0;	
	update_wtp_list *wtplist = NULL;
	unsigned int *wtp_list = NULL;
	unsigned int apcount = 0;
	int retu = 0;
	
	if(strncmp("add",oper,(strlen(oper)>3)?3:strlen(oper))==0){
		isadd = 1;
	}else if(strncmp("delete",oper,(strlen(oper)>6)?6:strlen(oper))==0){
		isadd = 0;
	}else{
		return -1;
	}
	
	wtplist = (struct tag_wtpid_list*)malloc(sizeof(struct tag_wtpid_list));
	if(NULL == wtplist)
		return 0;
	wtplist->wtpidlist = NULL ; 	
	wtplist->count = 0;
	
	if (!strcmp(wtp_id_list,"all"))
	{
		;	
	}else{
		ret = parse_wtpid_list((char*)wtp_id_list,&wtplist);
		if(ret != 0)
		{
			struct tag_wtpid * tmp = wtplist->wtpidlist;
			while(tmp){
				tmp = tmp->next;
			}
			destroy_input_wtp_list(wtplist);
			return -2;
		}
		else
		{
			delsame(wtplist);			
		}
	}
	int index = 0;
	/*if(vty->node == AP_GROUP_NODE){
		index = 0;
		GROUPID = vty->index;
	}else if(vty->node == HANSI_AP_GROUP_NODE){
		index = vty->index;
		GROUPID = vty->index_sub;
	}*/
	index = instance_id;
	GROUPID = ap_g_id;
	if(GROUPID >= WTP_GROUP_NUM || GROUPID == 0){
		syslog(LOG_DEBUG,"ap group id in add_del_ap_group_member_cmd is %d\n",GROUPID);
		return -3;
	}
		
	int(*dcli_init_func)(
						int ,
						unsigned int ,
						int ,
						struct tag_wtpid_list * ,
						unsigned int **,
						unsigned int *,
						DBusConnection *
						);

	dcli_init_func = dlsym(ccgi_dl_handle,"dcli_ap_group_add_del_member");

	ret =(*dcli_init_func)
		  (
			  index,
			  GROUPID,
			  isadd,
			  wtplist,
			  &wtp_list,
			  &apcount,
			  ccgi_dbus_connection
		  );

	if(ret == 0)
	{
		retu = 1;
		if(apcount != 0){
			FREE_OBJECT(wtp_list);
		}
	}
	else if(ret == WLAN_ID_NOT_EXIST)
	{
		retu = -4;
	}
	else
	{
		retu = -5;
	}

	return retu;	
}

void Free_show_group_member_cmd(unsigned int *wtp_list)
{	
	if(wtp_list){
		free(wtp_list);
		wtp_list = NULL;
	}
}

/*ֻҪ���ú������͵���Free_show_group_member_cmd()�ͷſռ�*/
int show_group_member_cmd(int instance_id,int ap_g_id,unsigned int **wtp_list)/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾap group id�Ƿ�*/
																					   /*����-2��ʾap group id does not exist������-3��ʾerror*/
{
	int ret;
	unsigned int groupid = 0;	
	unsigned int apcount = 0;
	int index = 0;
	int retu = 0;
    
	/*if(vty->node == AP_GROUP_NODE)
	{
	    vty_out(vty,"AP_GROUP_NODE = %d",vty->node);
		index = 0;
		groupid = vty->index;
	}
	else if(vty->node == HANSI_AP_GROUP_NODE)
	{
	    vty_out(vty,"HANSI_AP_GROUP_NODE = %d",vty->node);
		index = vty->index;
		groupid = vty->index_sub;
	}*/
	index = instance_id;
	groupid = ap_g_id;
	if(groupid >= WTP_GROUP_NUM || groupid == 0){
		syslog(LOG_DEBUG,"ap group id in show_group_member_cmd is %d\n",groupid);
		return -1;
	}
		
	int(*dcli_init_func)(
						int ,
						unsigned int ,
						unsigned int **,
						unsigned int *,
						DBusConnection *
						);

	dcli_init_func = dlsym(ccgi_dl_handle,"dcli_ap_group_show_member");

	ret =(*dcli_init_func)
		  (
			  index,
			  groupid,
			  wtp_list,
			  &apcount,
			  ccgi_dbus_connection
		  );

	if(ret == 0)
	{
		retu = 1;
	}
	else if(ret == WLAN_ID_NOT_EXIST)
	{
		retu = -2;
	}
	else
	{
		retu = -3;
	}

	return retu;	
}



#ifdef __cplusplus
}
#endif


