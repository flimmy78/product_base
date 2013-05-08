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
* ws_usrinfo.h
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
#ifndef _WS_USRINFO_H
#define _WS_USRINFO_H

#include <stdio.h>                              /*�������ͷ�ļ�*/
#include <string.h>                             /*�ַ�����ͷ�ļ�*/
#include <stdlib.h>   						   /*��̬�洢����ͷ�ļ�*/
#include <ctype.h>

/*get_user()*/
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>

#define N 50                                   /*�û���������ĳ���*/   
#define BUF_LEN 128                           /*�����ַ����ĳ���*/

#define VIEWGROUP "vtyview"                    /*vtyview�û���*/
#define ADMINGROUP "vtyadmin"                  /*vtyadmin����Ա��*/

#define PATH_LENG 512 /*·���ַ������� */


//modified by kehao  03-21-2011
//#define MAX_USER_NUM 15

#define MAX_USER_NUM 32  //���ײ�ͳһ���,�����û��������Ϊ32

///////////////////////////////////

#define USER_LOG_IN "/var/run/cpp/web_session.xml"
#define USER_SYSLOG_STATUS	"/opt/services/status/user-syslog_status.status"
#define TIMEOUT_THRESHOLD_STATUS "/opt/services/status/timeout-threshold_status.status"
#define TIMEOUT_THRESHOLD_CONF "/opt/services/conf/timeout-threshold_conf.conf"
#define USER_SYSLOG_XML	"/opt/services/conf/user-syslog_conf.conf"
static char *syslog_level[] = {"debug","inform","notice","warning","err","crit"};	
static char *sysoper_level[] = {"none","read","delete"};	
#define DIFF_TIME 180         /*����ϵͳ��ʱ��������ֵ������û�������η���ϵͳ��ʱ�������ڸ�ֵ�����˳�ϵͳ���µ�½*/

///////////add by tangsiqi 2009-5-25
#define WEB_IPMASK_STRING_MAXLEN	(strlen("255.255.255.255"))
#define WEB_IPMASK_STRING_MINLEN	(strlen("0.0.0.0"))
#define WEB_MAC_STRING_MINLEN	(strlen("00:00:00:00:00:00"))


#define INPUT_OK 				0
#define INPUT_ERROR				-1

#define INPUT_NULL				INPUT_ERROR-1
#define INPUT_LENGTH_ERROR		INPUT_ERROR-2
#define INPUT_FORMAT_ERROR		INPUT_ERROR-3
#define INPUT_CHAR_ERROR		INPUT_ERROR-4
#define INPUT_IP_MASK_ERROR		INPUT_ERROR-5
#define INPUT_IP_ERROR			INPUT_ERROR-6
#define CONLOGINSETTINGFILE   "/etc/login.defs"
#define CONLOGINSETTINGFILEBK "/etc/login.defs.bak"
#define CONPWDERRSETTINGFILE  "/etc/pam.d/common-auth"
#define CONPWDSYSSETTING "/etc/pam.d/common-password"


typedef struct st_deluser{
	char name[32];
	char group[32];
	struct st_deluser *next;
} st_deluser;


typedef struct  {
       int session_id;
       char user_name[N];
	   int access_time;           /*�û���¼ʱ��*/
	   int last_access_time;      /*�û�������ʱ��*/
}SESSION_PROFILE;


//char* dcryption(char *str);
char* encryption(char *user, char *pass);
char* dcryption(char *session_id);



//int get_user(char* groupname);             /*����û���Ϣ*/
int set_deluser(char* groupname, char* current_user, st_deluser *ulist_head);          /*���Ҫɾ���û�����Ϣ*/
int free_deluser(st_deluser *ulist_head);/*�ù�set_deluser������Ҫ�ô˺����ͷ�����!!*/

char *setclour(int i);                     /*�����б���ʾ��ɫ*/
int checkuser_exist(char* username);     /*����û��Ƿ����*/
int checkuser_group(char* username);    /*�ж��û�Ȩ��*/
int get_integrate_config_file(char *integrate_default, char *config_file); /*��������ļ�·��*/

int checkpassword(char *password);//����û������Ƿ��зǷ��ַ�
int checkPoint(char *ptr);	//����Ƿ���ֵ
int str2ulong(char *str,unsigned int *Value,int min_num,int max_num); //�ַ���ת��ulong����

int new_user_profile();
void add_user_infor(SESSION_PROFILE session);
void del_user_infor(char* session_id);
void modify_Last_AccTime(char* session_id,int new_access_time);
int Search_user_infor_byID(char* session_id,int ser_type,char* ser_result);
int Search_user_infor_byName(char* user_name,int ser_type,char* ser_result);
int create_user_infor(char *user_name);


//////add by tangsiqi  2009-5-25/////////////
int Input_IP_address_Check(char * ipAddress);
int Input_MAC_address_Check(char * macAddress);
int Input_User_Name_Check(char * iName);
int Input_User_Index_Check(char * iIndex);
int WEB_Pagination_get_range(int pageNumForREQ, int perPage_show_data_num, int * showHead, int *showTail);

int if_user_outtime(char *user_name);

int update_usrtime(char *session_id);
/*--password check ---------------------*/
extern int ccgi_passwd_alive_time(int time);
extern int ccgi_get_login_setting(int* maxdays);
extern int ccgi_set_login_setting(int maxdays);
extern int ccgi_get_pwd_err_setting(int* times);
extern int ccgi_passwd_max_error(int time);
extern int ccgi_get_pwd_unrepeat_setting(int *unreplynum,int *minlen,int *strongflag);
extern int ccgi_passwd_min_length(int time);
extern int ccgi_passwd_unrepeat(int time);
extern void init_user_syslog_xml();
extern int get_user_syslog_by_name(char *nodename,char *log_info,char *oper_info);
extern void add_user_syslog_by_name(char *username);
extern void del_user_syslog_by_name(char *username);
extern int mod_user_syslog_by_name(char *username,char *log_info,char *oper_info);
extern int get_timeout_threshold();

#endif

