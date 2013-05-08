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
* ws_dcli_vrrp.h
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
#ifndef _WS_DCLI_VRRP_H
#define _WS_DCLI_VRRP_H

#define HANSI_STR "High-Available Network Access Service Instance Configuration\n"

#define MAX_IFNAME_LEN        20
#define MAX_IPADDR_LEN        20

#define HANSIIPL   32
#define HANSIUNL   64


#define DCLI_VRRP_RETURN_CODE_BASE                 (0x150000)
#define DCLI_VRRP_RETURN_CODE_OK		              (DCLI_VRRP_RETURN_CODE_BASE + 1)
#define DCLI_VRRP_RETURN_CODE_ERR		          (DCLI_VRRP_RETURN_CODE_BASE + 2)
#define DCLI_VRRP_RETURN_CODE_PROFILE_OUT_OF_RANGE (DCLI_VRRP_RETURN_CODE_BASE + 3)
#define DCLI_VRRP_RETURN_CODE_PROFILE_EXIST        (DCLI_VRRP_RETURN_CODE_BASE + 4)
#define DCLI_VRRP_RETURN_CODE_PROFILE_NOTEXIST        (DCLI_VRRP_RETURN_CODE_BASE + 5)
#define DCLI_VRRP_RETURN_CODE_MALLOC_FAILED        (DCLI_VRRP_RETURN_CODE_BASE + 6)
#define DCLI_VRRP_RETURN_CODE_BAD_PARAM            (DCLI_VRRP_RETURN_CODE_BASE + 7)
#define DCLI_VRRP_RETURN_CODE_PROFILE_NOT_PREPARE  (DCLI_VRRP_RETURN_CODE_BASE + 8)  
#define DCLI_VRRP_RETURN_CODE_SERVICE_NOT_PREPARE  (DCLI_VRRP_RETURN_CODE_BASE + 9)  
#define DCLI_VRRP_RETURN_CODE_NO_CONFIG				(DCLI_VRRP_RETURN_CODE_BASE + 0x14)


#define VRRP_DEBUG_FLAG_ALL       0xFF
#define VRRP_DEBUG_FLAG_DBG       0x1
#define VRRP_DEBUG_FLAG_WAR       0x2
#define VRRP_DEBUG_FLAG_ERR       0x4
#define VRRP_DEBUG_FLAG_EVT       0x8
#define VRRP_DEBUG_FLAG_PKT_REV   0x10
#define VRRP_DEBUG_FLAG_PKT_SED   0x20
#define VRRP_DEBUG_FLAG_PKT_ALL   0x30
#define VRRP_DEBUG_FLAG_PROTOCOL  0x40


#define DCLI_VRRP_INSTANCE_CNT		(16)		/* count of VRRP instance allowed to created.	*/
#define DCLI_VRRP_OBJPATH_LEN		(64)		/* vrrp obj path length of special vrrp instance */
#define DCLI_VRRP_DBUSNAME_LEN		(64)		/* vrrp dbus name length of special vrrp instance */
#define DCLI_VRRP_SYS_COMMAND_LEN	(64)		/* vrrp used shell command length */

#define DCLI_VRRP_INSTANCE_CHECK_FAILED	(-1)
#define DCLI_VRRP_INSTANCE_CREATED		(1)		/* VRRP instance have created.		*/
#define DCLI_VRRP_INSTANCE_NO_CREATED	(0)		/* VRRP instance have not created.	*/

#define DCLI_VRRP_NOTIFY_ON				(0)		/* notify wid/portal	*/
#define DCLI_VRRP_NOTIFY_OFF			(1)		/* no notify wid/portal */

#define DCLI_VRRP_SHOW_RUNNING_CFG_LEN	(1024 * 1024)

/* eight threads in the current version of HAD module. */
#define DCLI_VRRP_THREADS_CNT			(8)

#define DCLI_VRRP_VGATEWAY_TF_FLG_OFF	(0)
#define DCLI_VRRP_VGATEWAY_TF_FLG_ON	(1)

/* add/delete virtual ip */
typedef enum
{
	DCLI_VRRP_VIP_OPT_TYPE_INVALID = 0,
	DCLI_VRRP_VIP_OPT_TYPE_ADD,
	DCLI_VRRP_VIP_OPT_TYPE_DEL
}DCLI_VRRP_VIP_OPT_TYPE;

typedef enum
{
	DCLI_VRRP_NOTIFY_OBJ_TYPE_WID = 0,			/* WID: wireless-control */
	DCLI_VRRP_NOTIFY_OBJ_TYPE_PORTAL,			/* PORTAL: easy-access-gateway */
	DCLI_VRRP_NOTIFY_OBJ_TYPE_INVALID,
}DCLI_VRRP_NOTIFY_OBJ_TYPE;

typedef enum
{
	DCLI_VRRP_LINK_TYPE_INVALID = 0,
	DCLI_VRRP_LINK_TYPE_UPLINK,
	DCLI_VRRP_LINK_TYPE_DOWNLINK,
	DCLI_VRRP_LINK_TYPE_VGATEWAY
}DCLI_VRRP_LINK_TYPE;

typedef struct{ 
 char gwname[30];
 char gwstate[10];
 char gwip[64];
 int gw_mask;
 int gwip1;
 int gwip2;
 int gwip3;
 int gwip4;
 
}Z_VRRP_VGATE;

typedef struct vrrp_link_ip_s{
	char ifname[64];
	char link_ip[32];
	int ip1;
	int ip2;
	int ip3;
	int ip4;
	struct vrrp_link_ip_s *next;
} vrrp_link_ip;

typedef struct {
 char state[20];
 unsigned int priority;
 unsigned int advert;
 char  preempt[10];
 char uplink_ifname[64];
 char upstate[10]; 
 char uplink_ip[64];
 char uplink_ip_list[255];
 char downlink_ifname[64];
 char downstate[10];
 char downlink_ip[64];
 char downlink_ip_list[255];
 vrrp_link_ip *uplink_list;
 vrrp_link_ip *downlink_list;
 vrrp_link_ip *vgatewaylink_list;
 char realipup[64];
 char realipdown[64];
 char macstate[10];
 char widstate[20];
 char portalstate[20];
 char hbinf[30];
 char hbstate[10];
 char hbip[64]; 
 Z_VRRP_VGATE gw[24];
 int gw_number;
 int ulip1;
 int dlip1;
 int ulip2;
 int dlip2;
 int ulip3;
 int dlip3;
 int ulip4;
 int dlip4;
 int urip1;
 int urip2;
 int urip3;
 int urip4;
 int drip1;
 int drip2;
 int drip3;
 int drip4;
 int hbip1;
 int hbip2;
 int hbip3;
 int hbip4;

}Z_VRRP;



/*functions*/
extern int ccgi_config_hansi_profile(char *pro_num);

extern int ccgi_downanduplink_ifname(char *provrrp,char *upifname,char *upip,char *downifname,char *downip,char *prio);

extern int ccgi_nohansi(char *provrrp);

extern int ccgi_vrrp_get_if_by_profile
(
    int profile,
    char* ifname1,
    char* ifname2
);

extern void ccgi_vrrp_notify_to_npd
(
    char* ifname1,
    char* ifname2,
    int   add
);

extern int ccgi_config_hansi_priority(char *provrrp,char * prio_num);

extern int ccgi_config_hansi_advertime(char *provrrp,char *adtime);

extern int ccgi_config_hansi_virtualmac(char *provrrp,char *macstates);

extern int ccgi_show_hansi(char *hnum,Z_VRRP *zvrrp);

extern void free_ccgi_show_hansi_profile(Z_VRRP *zvrrp);

extern int ccgi_show_hansi_profile(Z_VRRP *zvrrp, int profile);

extern int ccgi_config_downlink(char *profid,char * dlinkname,char *dlinkip,char *dlinkprio);


extern int ccgi_vrrp_check_ip_format
(
   char* buf,
   int* split_count
);

extern unsigned long ccgi_ip2ulong_vrrp(char *str);


extern int ccgi_str2ulong_vrrp(char *str,unsigned int *Value);

extern int ccgi_vrrp_check_ip_format
(
   char* buf,
   int* split_count
);


extern int ccgi_checkPoint_vrrp(char *ptr);
extern unsigned int ccgi_vrrp_config_service_disable
(
	unsigned int profile
);
extern int  send_arp(char *profid,char *ifnamez,char *ipz,char *macz);/*����0��ʾ�ɹ�������-1��ʾerror������-2��ʾ����mac��ʽ*/
extern int ccgi_vrrp_hansi_is_running
(
	unsigned int profileId
);
extern int ccgi_vrrp_check_service_started
(
	unsigned char *service_name,
	unsigned int profileId
);
extern void ccgi_vrrp_splice_objpath_string
(
	char *common_objpath,
	unsigned int profile,
	char *splice_objpath
);
extern void ccgi_vrrp_splice_dbusname
(
	char *common_dbusname,
	unsigned int profile,
	char *splice_dbusname
);

extern inline void reset_sigmaskz();
extern int delete_hansi_profile(char *profileid);/*����0��ʾ�ɹ�������-1��ʾprofileid������Χ������-2��ʾconfig hansi  service disable faild*/
                                         /*����-3��ʾdelete hansi  faild������-4��ʾcreate hansi  faild.����-5��ʾerror������-6��ʾhansi no exist*/

extern int config_vrrp_heartbeat_cmd_func(char * profid,char *ifnamez,char *ipz);/*����0��ʾ�ɹ�������-1��ʾʧ�ܣ�����-2��ʾ������ipΪ�գ�����-3��ʾ�����߽ӿ���Ϊ��*/
extern int ccgi_config_realip_downlink(char *profid,char *downifname,char *downip);/*����0��ʾ�ɹ�������-1��ʾerror,����-2��ʾ�ӿ���Ϊ��*/
extern int  set_hansi_vrid(char *profid,char *vidz);/*����0��ʾ�ɹ�������-1��ʾerror������-2��ʾvidz������Χ*/
extern int ccgi_downanduplink_ifname_mask(char *provrrp,char *upifname,char *upip,char *downifname,char *downip,char *prio,int upmask,int downmask);
                                                  /*����0��ʾ�ɹ�������-1��ʾerror������-2��ʾifnameΪ��,����-3��ʾipΪ��,����-4��ʾ����ֵ������Χ*/
extern int config_vrrp_uplink(char *provrrp,char *upifname,char *upip,char *prio,int upmask);
extern int config_vrrp_downlink_mask(char *proid,char *downifname,char *downip,char *prio,int downmask);/*����0��ʾ�ɹ�������-1��ʾerror������-2��ʾ�ӿ���Ϊ�գ�����-3��ʾ����ֵ������Χ*/
											  
extern int config_vrrp_link_add_vip(char * input_type, char *profid,char *linktype,char *ifnamez,char *ipz);

extern int config_vrrp_start_state(char *profid,char *statez);/*����0��ʾ�ɹ�������-1��ʾʧ��*/
extern int config_vrrp_service(char *profid,char *ablez);/*����0��ʾ�ɹ�������-1��ʾʧ��*/
extern int config_vrrp_gateway(char *profid,char *gwifname,char *gwip);/*����0��ʾ�ɹ�������-1��ʾʧ��*/
extern int cancel_vrrp_gateway(char *profid,char *gwifname,char *gwip);/*����0��ʾ�ɹ�������-1��ʾʧ��*/
extern int config_vrrp_preempt(char *profid,char *statez);/*����0��ʾ�ɹ�������-1��ʾʧ�ܣ�*/
extern int cancel_vrrp_transfer(char *profid);/*����0��ʾ�ɹ�������-1��ʾerror*/
extern int config_vrrp_max_down_count(char *maxcount);/*����0�ǳɹ�������-1��ʧ�ܣ�����-2���������������Χ*/
extern int config_vrrp_multi_link_detect(char *profid,char *statez);/*����0��ʾ�ɹ�������-1��ʾʧ��*/
extern int config_vrrp_set_vgateway_transform(char *profid,char *statez);
extern int config_vrrp_notify(char *profid,char *typez,char *statez);/*����0��ʾ�ɹ�������-1��ʾʧ�ܣ�����-2Unknown command format*/
//kehao modify  20110519
//extern int ccgi_show_hansi_profile_detail(Z_VRRP *zvrrp,int profile);
extern int ccgi_show_hansi_profile_detail(Z_VRRP *zvrrp,int profile,int *upnum,int *downnum,int* vgatenum,char *upstate[],char *downstate[],char *uplink[],char *downlink[],char *vgate[],char* vuplinkip[],char *vdownlinkip[],char* ruplinkip[],char* rdownlinkip[],char *vgateip[]);

extern int mask_bit(char *mask);
extern int config_vrrp_no_real_ip_uplink(char *upif,char *upip,char *profid);/*����0��ʾ�ɹ�������-1��ʾʧ��*/
extern int config_vrrp_no_real_ip_downlink(char *downif,char *downip,char *profid);/*����0��ʾ�ɹ�������-1��ʾʧ�ܣ�����-2��ʾerror*/
extern int config_vrrp_real_ip_uplink(char *upif,char *upip,char *profid);/*����0��ʾ�ɹ�������-1��ʾʧ�ܣ�����-2��ʾerror*/
extern int snmp_get_vrrp_state(int profile,int *hansi_state);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾʵ��������*/
int show_vrrp_switch_times(unsigned int vrrp_id, unsigned long *switch_times);

#endif

