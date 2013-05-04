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
* ws_dcli_wlans.h
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


#ifndef _WS_DCLI_WLANS_H
#define _WS_DCLI_WLANS_H

#include "wcpss/wid/WID.h"
#include "wcpss/asd/asd.h"
#include "dbus/wcpss/dcli_wid_wtp.h"
#include "dbus/wcpss/dcli_wid_wlan.h"
#include "dbus/wcpss/dcli_wid_radio.h"
#include "ws_dbus_list_interface.h"

#define VLANID_RANGE_MAX			4094
#define DCLIAC_BUF_LEN	16
#define CMD_FAILURE -1
#define WTP_WTP_IP_LEN 21
#define STRING_LEN 256


#define PARSE_RADIO_IFNAME_SUB '-'
#define PARSE_RADIO_IFNAME_POINT '.'


typedef enum{
	e_check_wtpid=0,
	e_check_sub,
	e_check_radioid,
	e_check_wlanid,
	e_check_point,
	e_check_fail,
	e_check_end,
	e_check_success
}radio_ifname_state;



#define RATE_SPLIT_COMMA 	','
#define RADIO_IF_NAME_LEN 20
#define RADIO_RATE_LIST_LEN 20
#define WLAN_IF_NAME_LEN 20
#define WTP_ARRAY_NAME_LEN 20
#define WTP_COMMAND_LEN 256
#define WTP_IP_BUFF_LEN 16
#define WTP_WLAN_VLAN_LEN 4
#define WTP_LIST_SPLIT_COMMA 	','

#define WTP_LIST_SPLIT_COMMA 	','	
#define WTP_LIST_SPLIT_BAR 	'-'	



typedef enum{
	check_rate=0,
	check_comma,
	check_fail,
	check_end,
	check_success
}rate_list_state;

#if 0
struct extern_info{
	int wtpid;
  float cpu;
  unsigned int tx_mgmt;
  unsigned int rx_mgmt;
  unsigned int totle_mgmt;
  unsigned int tx_packets;
  unsigned int tx_errors;
  unsigned int tx_retry;
  unsigned char eth_count;
  unsigned char eth_updown_time[AP_ETH_IF_NUM];
  unsigned char ath_count;
  unsigned char ath_updown_time[AP_ATH_IF_NUM];
} ;
typedef struct extern_info wid_info;
#endif

struct wtp_eth_inf_infor_profile
{
	char *name;
	unsigned char state;
	struct wtp_eth_inf_infor_profile *next;
};

struct ap_reportinterval_profile
{
	unsigned int moment_report_value;
	unsigned int routine_report_value;
	unsigned char moment_report_switch;
	int collect_time;
	unsigned int sample_time;
};

typedef enum{
	dcli_wtp_check_wtpid=0,
	dcli_wtp_check_comma,
	dcli_wtp_check_fail,
	dcli_wtp_check_end,
	dcli_wtp_check_success
}wtp_list_state;

struct SSIDStatsInfo_sub_wlan_v2{
	unsigned char wlanCurrID;
	/*should input update wtp bss pakets infomation*/
	/*------------------------------------------*/
	unsigned long long SSIDChStatsDwlinkTotRetryPkts;	//SSID�����ش�����		//wtp f
	unsigned long long SSIDChStatsUplinkUniFrameCnt;			//SSID���е���֡��//wtp f
	unsigned long long SSIDChStatsDwlinkUniFrameCnt;			//SSID���е���֡��//wtp f
	unsigned long long SSIDUpChStatsFrameNonUniFrameCnt;		//SSID���зǵ���֡��//wtp f
	unsigned long long SSIDDownChStatsFrameNonUniFrameCnt;		//SSID���зǵ���֡��//wtp f
	
	unsigned long long SSIDDwlinkTotFrameCnt;		//SSID���е���֡��//wtp f
	unsigned long long SSIDUplinkTotFrameCnt;		//SSID���е���֡��//wtp f
	/*------------------------------------------*/

	unsigned int SSIDRxCtrlFrameCnt;			//SSID���յĿ���֡��
	unsigned int SSIDRxDataFrameCnt;			//SSID���յ�����֡��
	unsigned int SSIDRxAuthenFrameCnt;			//SSID���յ���֤֡��
	unsigned int SSIDRxAssociateFrameCnt;		//SSID���յĹ���֡��
	unsigned int SSIDTxCtrlFrameCnt;			//SSID���͵Ŀ���֡��
	unsigned int SSIDTxDataFrameCnt;			//SSID���͵�����֡��
	unsigned int SSIDTxAuthenFrameCnt;			//SSID���͵���֤֡��
	unsigned int SSIDTxAssociateFrameCnt;		//SSID���͵Ĺ���֡��

	unsigned int SSIDDwErrPkts;                 //SSID���еĴ������
	unsigned int SSIDDwDropPkts;				//SSID���е��ܶ�֡��
	unsigned int SSIDDwTotErrFrames;			//SSID���е��ܴ�֡��
	unsigned int SSIDUpErrPkts;					//SSID���еĴ������
	unsigned int SSIDUpDropPkts;				//SSID���е��ܶ�֡��
	unsigned int SSIDUpTotErrFrames;			//SSID���е��ܴ�֡��
	
	unsigned int SSIDApChStatsNumStations;			//ʹ�ø��ŵ����ն���
	unsigned int SSIDAccessTimes;				//access times
	/*table 34*/
	/*--------------------------------------*/
	/*��wtp ���������update wtp bss pakets infomation*/
	unsigned char *wtpSSIDName;
	unsigned int wtpSSIDMaxLoginUsr;
	unsigned char wtpSSIDState;
	unsigned char wtpSSIDSecurityPolicyID;
	unsigned char wtpSSIDLoadBalance;
	char *wtpSSIDESSID;
	unsigned int vlanid;	//xiaodawei add vlanid 20101028

	/*--------------------------------------*/

	struct SSIDStatsInfo_sub_wlan_v2 *next;
};

struct SSIDStatsInfo_v2 {
    unsigned char *wtpMacAddr;
    int wtpCurrID;

    unsigned char wtpBwlanRadioNum;//wtp �¿��õ�radio��
    unsigned int wtpSupportRadioId[L_RADIO_NUM];
    
 	unsigned char wtpSupportSSID[WLAN_NUM] ; 		//should make ]
 	unsigned char wtpBssNum;
	
    unsigned int SSIDTxSignalPkts;			//SSID���͵��������	//wtp f
    unsigned int SSIDRxSignalPkts;			//SSID���յ��������	//wtp f
    
    unsigned int SSIDTxDataPkts;			//SSID���͵����ݰ���//wtp f
    unsigned int SSIDRxDataPkts;			//SSID���յ����ݰ���//wtp f
    unsigned long long SSIDUplinkDataOctets;			//SSID���յ��ֽ���//wtp f
    unsigned long long SSIDDwlinkDataOctets;			//SSID���͵��ֽ���//wtp f
    unsigned int SSIDApChStatsFrameFragRate;			//�ŵ���֡�ķֶ�����//wtp f
	
	struct SSIDStatsInfo_sub_wlan_v2 *SSIDStatsInfo_sub_wlan_head;
    struct SSIDStatsInfo_v2 *next;
};

struct ap_num_by_interface
{
	char if_name[255];
	int ap_num;
	struct ap_num_by_interface *next;
};
typedef struct ap_num_by_interface AP_NUM_INF;

typedef enum{
	dcli_mcs_check_mcsid=0,
	dcli_mcs_check_comma=1,
	dcli_mcs_check_fail=2,
	dcli_mcs_check_end=3,
	dcli_mcs_check_success=4,
	dcli_mcs_check_bar=5
}mcs_list_state;
#define MCS_LIST_SPLIT_COMMA  ','	
#define MCS_LIST_SPLIT_BAR 	  '-'

struct model_tar_file {
	char *apmodel;
	char *filename;
    struct model_tar_file *next;
};


extern int parse_short_ID(char* str,unsigned short* ID);
extern int parse_char_ID(char* str,unsigned char* ID);
extern int parse_int_ID(char* str,unsigned int* ID);
extern void str2lower(char **str);



extern int create_wlan(dbus_parameter parameter, DBusConnection *connection,char * id, char *wlan_name, char *wlan_essid);/*����0��ʾ����ʧ�ܣ�����1��ʾ�����ɹ�������-1��ʾWLAN ID�Ƿ�������-2��ʾwlan�Ѵ��ڣ�����-3��ʾ����*/
																															/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int create_wlan_cmd_cn(dbus_parameter parameter, DBusConnection *connection,char *id, unsigned char *wlan_name, unsigned char *wlan_essid);
																					  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																					  /*����-2��ʾwlan id should be 1 to WLAN_NUM-1������-3��ʾwlan name is too long,out of the limit of 15*/
																					  /*����-4��ʾwlan name is illegal������-5��ʾessid is too long,out of the limit of 32*/
																					  /*����-6��ʾwlan id exist������-7��ʾillegal input������-8��ʾerror*/
																					  /*����-9��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																					  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int delete_wlan(dbus_parameter parameter, DBusConnection *connection,int id);    /*����0��ʾ ɾ��ʧ�ܣ�����1��ʾɾ���ɹ�*/
																						/*����-1��ʾWLAN ID�Ƿ�������-2��ʾWLAN ID NOT existed*/
																						/*����-3��ʾplease disable WLAN first������-4��ʾ����*/
																						/*����-5��ʾsome radios interface in ebr,please delete it from ebr first*/
																						/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int config_wlan_service(dbus_parameter parameter, DBusConnection *connection,int id,char *state);	 
																		 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾno security profile binded������-2��ʾwtp interface policy conflict*/
						                                                 /*����-3��ʾyou map layer3 interace error������-4��ʾyou should bind interface first������-5��ʾerror������-6ʾWLAN ID�Ƿ�*/
																		 /*����-7��ʾwlan bingding securithindex same*/
																		 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																		 
extern int wlan_apply_interface(dbus_parameter parameter, DBusConnection *connection,int id,char *inter_name);   
																		 	   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾthe length of interface name excel 16*/
						                                                       /*����-2��ʾwlan id does not exist������-3��ʾinterface dose not exist*/
						                                                       /*����-4��ʾwlan is enable,please disable it first������-5��ʾerror������-6ʾWLAN ID�Ƿ�*/
																			   /*����-7��ʾis no local interface, permission denial������-8��ʾinterface has be binded in other hansi*/
																			   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int wlan_apply_ipv6interface(dbus_parameter parameter, DBusConnection *connection,int id,char *inter_name);
																			    /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																				/*����-1��ʾinput parameter size is excel the limit of 16*/
																				/*����-2��ʾwlan id does not exist������-3��ʾinterface dose not exist*/
																				/*����-4��ʾwlan is enable,please disable it first*/
																				/*����-5��ʾwlan bingding ipv6 addr error make sure interface have ipv6 address*/
																				/*����-6��ʾerror������-7ʾWLAN ID�Ƿ�*/
																				/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																				
extern int  set_interface_nasid_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *inter_name,char *nasid); 
																							  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
		 																					  /*����-1��ʾthe length of input parameter is excel the limit of 16*/
		 																					  /*����-2��ʾthe length of input parameter is excel the limit of 128*/
		 																					  /*����-3��ʾnas identifier include unknow character*/
		 																					  /*����-4��ʾinterface does not exist*/
		 																					  /*����-5��ʾinterface unused*/
		 																					  /*����-6��ʾyou should apply security first*/
		 																					  /*����-7��ʾno nas_id needed,please apply interface without nas_identifier*/
		 																					  /*����-8��ʾwlan be enable,please service disable first*/
		 																					  /*����-9��ʾerror������-10ʾWLAN ID�Ƿ�*/
																							  /*����-11��ʾinterface has be binded in other hansi*/
																							  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int remove_interface_nasid_cmd_func(dbus_parameter parameter, DBusConnection *connection,int WlanID,char *if_name);
																							/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																							/*����-1��ʾthe length of if_name is excel the limit of 16*/
																							/*����-2��ʾwlan id is not exist*/
																							/*����-3��ʾwlan WlanID is not binding interface if_name*/
																							/*����-4��ʾinterface error*/
																							/*����-5��ʾno nas_id needed,please use <wlan apply interface IFNAME>,without nas_identifier*/
																							/*����-6��ʾwlan be enable,please service disable first*/
																							/*����-7��ʾerror������-8ʾWLAN ID�Ƿ�*/
																							/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																							
extern int wlan_delete_interface(dbus_parameter parameter, DBusConnection *connection,int id,char *inter_name);  
																			   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwlan doesn't binding this interface������-2��ʾWlan ID Not existed*/
																			   /*����-3��ʾInterface not existed������-4��ʾWlan is enable binding error! please disable first������-5��ʾerror������-6ʾWLAN ID�Ƿ�*/
																			   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																			   
extern void CheckWIDIfPolicy(char *whichinterface, unsigned char wlan_if_policy);
														   
extern void Free_wlan_head(DCLI_WLAN_API_GROUP *WLANINFO);
/*����1ʱ������Free_wlan_head()�ͷſռ�*/														   
extern int show_wlan_list(dbus_parameter parameter, DBusConnection *connection,DCLI_WLAN_API_GROUP **WLANINFO);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwlan not exsit*/
																												  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_one_wlan_head(DCLI_WLAN_API_GROUP *WLANINFO);
/*����1ʱ������Free_one_wlan_head()�ͷſռ�*/
extern int show_wlan_one(dbus_parameter parameter, DBusConnection *connection,int id,DCLI_WLAN_API_GROUP **WLANINFO); 
																					 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
						                                                             /*����-1��ʾwlan id should be 1 to WLAN_NUM-1*/
						                                                             /*����-2��ʾwlan id is not exited������-3��ʾerror*/
																					 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*num�ķ�Χ��1-65536*/																					 
extern int config_wlan_max_sta_num(dbus_parameter parameter, DBusConnection *connection,int id,char * num);  
																				/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																				/*����-1��ʾunknown NUM format������-2��ʾwlan id not exist.*/
																				/*����-3��ʾmore sta(s) has accessed before you set max sta num*/
																				/*����-4��ʾoperation fail������-5��ʾerror������-6ʾWLAN ID�Ƿ�*/
																				/*����-7��ʾinput max sta num should be 1-65536*/
																				/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*sec_ID��ʾsecurity id;w_id��ʾwlan id*/																
extern int apply_wlanID(dbus_parameter parameter, DBusConnection *connection,int sec_id,int w_id);
																/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
			                                                    /*����-1��ʾwlan id not exist������-2��ʾasd security profile not exist*/
			                                                    /*����-3��ʾasd security profile not integrity������-4��ʾencryption type dosen't match with security type*/
			                                                    /*����-5��ʾshould be disable wlan first������-6��ʾsecurity ID�Ƿ�������-7ʾWLAN ID�Ƿ�*/
																/*����-8��ʾsecurity rdc has not config!*/
																/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*Hessid==1��ʾyes��Hessid==0��ʾno*/																
extern int set_hideessid(dbus_parameter parameter, DBusConnection *connection,int id,char *Hessid);
																/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwlan id not exist*/
												  				/*����-2��ʾWLAN is enable,please disable it first������-3��ʾerror������-4ʾWLAN ID�Ƿ�*/
																/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																
extern int wlan_map_interface(dbus_parameter parameter, DBusConnection *connection,int id);	
															/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾWlan ID Not existed*/
														    /*����-2��ʾWLAN is enable, please disable it first*/
				                                            /*����-3��ʾWLAN have already binding vlan,please undo wlan-vlan binding first*/
				                                            /*����-4��ʾerror������-5ʾWLAN ID�Ƿ�*/
															/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
															
extern int wlan_unmap_interface(int id);	/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾWlan ID Not existed*/
												/*����-2��ʾWLAN is enable, please disable it first������-3��ʾerror������-4ʾWLAN ID�Ƿ�*/
extern int wlan_bss_map_interface(int id);	  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾWlan ID Not existed*/
											      /*����-2��ʾWLAN is enable, please disable it first������-3��ʾerror������-4ʾWLAN ID�Ƿ�*/
extern int wlan_bss_unmap_interface(int id);		/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾWlan ID Not existed*/
     													/*����-2��ʾWLAN is enable, please disable it first������-3��ʾerror������-4ʾWLAN ID�Ƿ�*/		  


extern int no_interface_ifname(char *ifname);//ȡ������ӿ�ӳ��

/*typeΪ"number","flow"��"disable"*/
extern int config_wlan_load_balance(dbus_parameter parameter, DBusConnection *connection,int wid,char *type);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwlan id does not exist������-2��ʾoperation fail������-3��ʾerror������-4��ʾWLAN ID�Ƿ�*/
																												  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																						
extern int config_wlan_number_balance_parameter(dbus_parameter parameter, DBusConnection *connection,int id,char* param);
																							 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾbalance parameter should be 1 to 10*/
					                                                                         /*����-2��ʾwlan id does not exist������-3��ʾoperation fail ������-4��ʾerror������-5ʾWLAN ID�Ƿ�*/
																							 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																							 
extern int config_wlan_flow_balance_parameter(dbus_parameter parameter, DBusConnection *connection,int wid,char * para);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾbalance parameter should be 1 to 30������-2��ʾwlan id does not exist�� ����-3��ʾoperation fail������-4��ʾerror������-5ʾWLAN ID�Ƿ�*/
																							 								   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*typeΪ"l3"*/
extern int wlan_roam_policy(dbus_parameter parameter, DBusConnection *connection,int id,char *type,char*stat);	
																				/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwlan id does not exist*/
																				/*����-2��ʾoperation fail������-3��ʾwlan should be disable first*/
																				/*����-4��ʾroaming should be disable first������-5��ʾerror������-6ʾWLAN ID�Ƿ�*/
																				/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																				
extern int set_wlan_vlan_id(dbus_parameter parameter, DBusConnection *connection,int wlan_id,char *vlan_id);  
																			/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown input*/
																			/*����-2��ʾinput parameter should be 1 to 4094������-3��ʾwlan id does not exist*/
																			/*����-4��ʾwlan is in other L3 interface������-5��ʾwlan should be disable first*/
																			/*����-6��ʾerror������-7ʾWLAN ID�Ƿ�*/
																			/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																			
extern int set_wlan_vlan_priority(dbus_parameter parameter, DBusConnection *connection,int wlan_id,char *value);  
																				/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown input*/
																				/*����-2��ʾinput parameter should be 0 to 7������-3��ʾwlan id does not exist*/
																				/*����-4��ʾwlan is in other L3 interface������-5��ʾwlan has not binding vlan*/
																				/*����-6��ʾwlan should be disable first������-7��ʾwlan is under tunnel wlan-vlan policy*/
																				/*����-8��ʾerror������-9ʾWLAN ID�Ƿ�*/
																				/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																
extern int undo_wlan_vlan_cmd(dbus_parameter parameter, DBusConnection *connection,int wlan_id);   
																	/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwlan id does not exist*/
																	/*����-2��ʾwlan is in other L3 interface������-3��ʾwlan should be disable first*/
																	/*����-4��ʾerror������-5ʾWLAN ID�Ƿ�*/
																	/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_wlan_vlan_info(DCLI_WLAN_API_GROUP *WLANINFO);
/*����1ʱ������Free_wlan_vlan_info()�ͷſռ�*/													
extern int show_wlan_vlan_info(dbus_parameter parameter, DBusConnection *connection,int wlan_id,DCLI_WLAN_API_GROUP **WLANINFO); /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwlan id does not exist������-2��ʾerror������-3ʾWLAN ID�Ƿ�*/
																																	  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int wlan_set_bridge_isolation_func(dbus_parameter parameter, DBusConnection *connection,int wlan_id,char *STATE);
																						/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter should only be 'enable' or 'disable'*/
																						/*����-2��ʾwlan id does not exist������-3��ʾwlan is not wlan if policy������-4��ʾwlan should be disable first*/
																						/*����-5��ʾwlan bridge error������-6��ʾsystem cmd process error*/
																						/*����-7��ʾsameportswitch and isolation are conflict,disable sameportswitch first������-8��ʾerror������-9ʾWLAN ID�Ƿ�*/
																						/*����-10��ʾapply security in this wlan first*/
																						/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																						
extern int wlan_set_bridge_multicast_isolation_func(dbus_parameter parameter, DBusConnection *connection,int wlan_id,char *STATE); 
																									/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter should only be 'enable' or 'disable'*/
																									/*����-2��ʾwlan id does not exist������-3��ʾwlan is not wlan if policy������-4��ʾwlan should be disable first*/
																									/*����-5��ʾwlan bridge error������-6��ʾsystem cmd process error*/
																									/*����-7��ʾsameportswitch and isolation are conflict,disable sameportswitch first������-8��ʾerror������-9ʾWLAN ID�Ƿ�*/
																									/*����-10��ʾapply security in this wlan first*/
																									/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_bridge_isolation(DCLI_WLAN_API_GROUP *WLANINFO);
/*����1ʱ������Free_bridge_isolation()�ͷſռ�*/
extern int wlan_show_bridge_isolation_func(dbus_parameter parameter, DBusConnection *connection,int wlan_id,DCLI_WLAN_API_GROUP **WLANINFO);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwlan id does not exist������-2��ʾerror������-3ʾWLAN ID�Ƿ�*/
																																				  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_tunnel_wlan_vlan_cmd_func(dbus_parameter parameter, DBusConnection *connection,int wlan_id,char *STATE,char *IFNAME);  
																										/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter should only be 'add' or 'delete'*/
																										/*����-2��ʾinput interface name should only start with 'radio',other interface you should use ebr configuration*/
																										/*����-3��ʾif name too long������-4��ʾmalloc error������-5��ʾwlan id does not exist*/
																										/*����-6��ʾwlan is in local wlan-vlan interface������-7��ʾwlan should be disable first������-8��ʾinput ifname is wrong*/
																										/*����-9��ʾwlan is not in tunnel mode������-10��ʾif is already STATE,or system cmd error������-11��ʾerror������-12ʾWLAN ID�Ƿ�*/
																										/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_tunnel_wlan_vlan_head(DCLI_WLAN_API_GROUP *WLANINFO);
/*����1ʱ������Free_tunnel_wlan_vlan()�ͷſռ�*/
extern int show_tunnel_wlan_vlan_cmd_func(dbus_parameter parameter, DBusConnection *connection,int wlan_id,DCLI_WLAN_API_GROUP **WLANINFO);
																			/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwlan id does not exist*/
																			/*����-2��ʾwlan is in local wlan-vlan interface������-3��ʾwlan should be disable first*/
																			/*����-4��ʾinput ifname is wrong������-5��ʾwlan is not in tunnel mode*/
																			/*����-6��ʾadd if to br fail������-7��ʾremove if from br fail������-8��ʾerror������-9ʾWLAN ID�Ƿ�*/
																			/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																			
extern int wlan_set_sameportswitch_func(dbus_parameter parameter, DBusConnection *connection,int wlan_id,char *spswitch_state);
																								/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter should only be 'enable' or 'disable'*/
																								/*����-2��ʾwlan id does not exist������-3��ʾwlan is not wlan if policy������-4��ʾwlan should be disable first*/
																								/*����-5��ʾwlan bridge error������-6��ʾsystem cmd process error*/
																								/*����-7��ʾsameportswitch and isolation are conflict,disable isolation first������-8��ʾerror������-9ʾWLAN ID�Ƿ�*/
																								/*����-10��ʾapply security in this wlan first*/
																								/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*typeΪ"wds"��"mesh"*/
/*stateΪ"enable"��"disable"*/																								
extern int config_wds_service_cmd_func(dbus_parameter parameter, DBusConnection *connection,int wlanID,char *type,char *state);  
																						/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾyou should bind interface first*/
																						/*����-2��ʾyou map layer3 interace error������-3��ʾyou must first service enable wlan*/
																						/*����-4��ʾerror������-5ʾWLAN ID�Ƿ�*/
																						/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																						
extern int set_wlan_essid_func(dbus_parameter parameter, DBusConnection *connection,int wlan_id,char *ESSID);
																			/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																			/*����-1��ʾessid is too long,out of the limit of 32*/
																			/*����-2��ʾwlan id does not exist*/
																			/*����-3��ʾwlan is enable,please disable it first*/
																			/*����-4��ʾerror������-5ʾWLAN ID�Ƿ�*/
																			/*����-6��ʾUNKNOWN COMMAND*/
																			/*����-7��ʾillegal essid name!! ` \ \" & * ( ) not supported!*/
																			/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*stateΪ"enable"��"disable"*/
extern int set_wlan_sta_ip_mac_binding_cmd(dbus_parameter parameter, DBusConnection *connection,int wlan_id,char *state);
																							/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																							/*����-1��ʾinput patameter only with 'enable' or 'disable'*/
																							/*����-2��ʾerror������-3ʾWLAN ID�Ƿ�*/
																							/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*typeΪ(starttimer|stoptimer)��stateΪ(enable|disable)*/
extern int set_wlan_timer_able_cmd(dbus_parameter parameter, DBusConnection *connection,int wlan_id,char *time_type,char *state);
																								 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																								 /*����-1��ʾfirst input patameter only with 'starttimer' or 'stoptimer'*/
																								 /*����-2��ʾsecond input patameter only with 'enable' or 'disable'*/
																								 /*����-3��ʾerror������-4ʾWLAN ID�Ƿ�*/
																								 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*actionΪ(start|stop)��Time's format should be like 12:32:56��TimeΪ(once|cycle)*/
/*argv_num��ʾ�����weekdays�ĸ���*/
/*char *weekdays[]������������*/
extern int set_wlan_servive_timer_func_cmd(dbus_parameter parameter, DBusConnection *connection,int wlan_id,char *action,char *Time,char *type,int argv_num,char *weekdays[]);
																						 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																					     /*����-1��ʾinput patameter only with 'start' or 'stop'*/
																					     /*����-2��ʾinput patameter format should be 12:32:56*/
																					     /*����-3��ʾinput patameter only with 'once' or 'cycle'*/
																					     /*����-4��ʾweekdays you want (like Sun Mon Tue Wed Thu Fri Sat or hebdomad)*/																																	  
																					     /*����-5��ʾerror������-6ʾWLAN ID�Ƿ�*/	
																						 /*����-7��ʾthe starttimer or stoptimer should be disabled*/
																						 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*value�ķ�Χ��0-300000*/
extern int set_whole_wlan_traffic_limit_cmd(dbus_parameter parameter, DBusConnection *connection,int wlan_id,char *value); 
																						   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																						   /*����-1��ʾinput parameter error*/
																						   /*����-2��ʾwlan id does not exist*/
																						   /*����-3��ʾerror������-4ʾWLAN ID�Ƿ�*/
																						   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																						   /*����-5��ʾinput parameter should be 0~300000*/

/*value�ķ�Χ��0-300000*/
extern int set_whole_wlan_send_traffic_limit_cmd(dbus_parameter parameter, DBusConnection *connection,int wlan_id,char *value);
																						   	 	/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																							    /*����-1��ʾinput parameter error*/
																							    /*����-2��ʾwlan id does not exist*/
																							    /*����-3��ʾerror������-4ʾWLAN ID�Ƿ�*/
																								/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																								/*����-5��ʾinput parameter should be 0~300000*/

/*value�ķ�Χ��0-300000*/																						   
extern int set_whole_wlan_station_average_traffic_limit_cmd(dbus_parameter parameter, DBusConnection *connection,int wlan_id,char *value);
																											/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																											/*����-1��ʾinput parameter error*/
																											/*����-2��ʾwlan id does not exist*/
																											/*����-3��ʾerror������-4ʾWLAN ID�Ƿ�*/
																											/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																											/*����-5��ʾinput parameter should be 0~300000*/

/*value�ķ�Χ��0-300000*/
extern int set_whole_wlan_station_average_send_traffic_limit_cmd(dbus_parameter parameter, DBusConnection *connection,int wlan_id,char *value);
																												   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																												   /*����-1��ʾinput parameter error*/
																												   /*����-2��ʾwlan id does not exist*/
																												   /*����-3��ʾerror������-4ʾWLAN ID�Ƿ�*/
																												   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																												   /*����-5��ʾinput parameter should be 0~300000*/

extern void Free_show_all_wlan_wapi_basic_information_cmd(struct ConfigWapiInfo *WlanHead);		
/*ֻҪ���ã���ͨ��Free_show_all_wlan_wapi_basic_information_cmd()�ͷſռ�*/
extern int show_all_wlan_wapi_basic_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct ConfigWapiInfo **WlanHead);
																											   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																											   /*����-1��ʾThere is no Wlan now*/
																											   /*����-2��ʾThere is no Wapi Wlan now*/
																											   /*����-3��ʾerror*/
																											   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_all_wlan_ssid_config_information_cmd(struct SSIDConfigInfo *WlanHead);																											   
/*��*WlanHead��Ϊ��ʱ������Free_show_all_wlan_ssid_config_information_cmd()�ͷſռ�*/
extern int show_all_wlan_ssid_config_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct SSIDConfigInfo **WlanHead);
																												/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																											    /*����-1��ʾthere is no wlan*/
																											    /*����-2��ʾerror*/
																												/*����-3��ʾsecurity profile does not exist*/
																												/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_all_wlan_unicast_information_cmd(struct UnicastInfo *WlanHead);
/*ֻҪ���ã���ͨ��Free_show_all_wlan_unicast_information_cmd()�ͷſռ�*/
extern int show_all_wlan_unicast_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct UnicastInfo **WlanHead);
																										/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																									    /*����-1��ʾThere is no Wlan now*/
																									    /*����-2��ʾThere is no Wapi Wlan now*/
																									    /*����-3��ʾerror*/
																										/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																												
extern void Free_show_all_wlan_wapi_performance_stats_information_cmd(struct WtpWAPIPerformanceStatsInfo *WlanHead);
/*ֻҪ���ã���ͨ��Free_show_all_wlan_wapi_performance_stats_information_cmd()�ͷſռ�*/
extern int show_all_wlan_wapi_performance_stats_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct WtpWAPIPerformanceStatsInfo **WlanHead);
																					   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																					   /*����-1��ʾThere is no Wlan now*/
																					   /*����-2��ʾThere is no Wapi Wlan now*/
																					   /*����-3��ʾerror*/
																					   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																																		   
extern void Free_show_all_wlan_wapi_extend_config_information_cmd(struct WtpWAPIExtendConfigInfo *WlanHead);
/*ֻҪ���ã���ͨ��Free_show_all_wlan_wapi_extend_config_information_cmd()�ͷſռ�*/
extern int show_all_wlan_wapi_extend_config_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct WtpWAPIExtendConfigInfo **WlanHead);
																								  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																								  /*����-1��ʾThere is no Wlan now*/
																								  /*����-2��ʾThere is no Wapi Wlan now*/
																								  /*����-3��ʾerror*/
																								  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_all_wlan_bss_wapi_performance_stats_information_cmd(struct BssWAPIPerformanceStatsInfo *WlanHead);
/*ֻҪ���ã���ͨ��Free_show_all_wlan_bss_wapi_performance_stats_information_cmd()�ͷſռ�*/
extern int show_all_wlan_bss_wapi_performance_stats_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct BssWAPIPerformanceStatsInfo **WlanHead);
																									/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																									/*����-1��ʾThere is no Wlan now*/
																									/*����-2��ʾThere is no Wapi Wlan now*/
																									/*����-3��ʾerror*/
																									/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_wlan_ascii_essid_cmd(dbus_parameter parameter, DBusConnection *connection,int wlan_id,unsigned char *new_essid); 
																								/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																								/*����-1��ʾessid is too long,out of the limit of 64*/
																								/*����-2��ʾwlan id does not exist*/
																								/*����-3��ʾwlan is enable,please disable it first*/
																								/*����-4��ʾerror������-5ʾWLAN ID�Ƿ�*/
																								/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_wlan_of_all_cmd(struct WLAN_INFO *wlan_info);
/*ֻҪ���ã���ͨ��Free_show_wlan_of_all_cmd()�ͷſռ�*/
extern int show_wlan_of_all_cmd(dbus_parameter parameter, DBusConnection *connection,struct WLAN_INFO **wlan_info);
																				   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																				   /*����-1��ʾwlan id does not exist*/
																				   /*����-2��ʾerror*/

/*Switch��ȡֵ��"enable"��"disable"*/
extern int set_wlan_bss_multi_user_optimize_cmd(dbus_parameter parameter, DBusConnection *connection,int wlan_id,char *Switch);
																			   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																			   /*����-1��ʾWLAN ID�Ƿ�������-2��ʾbss not exist*/
																			   /*����-3��ʾoperation fail������-4��ʾwlan is not binded radio*/
																			   /*����-5��ʾerror*/







/*WTP*/												  
extern void CheckWTPState(char *state, unsigned char WTPstate);
extern void CheckWTPQuitReason(char *quitreason,unsigned char quitstate);
//modify by qiaojie
#if 0
extern void DcliWReInit();   
#endif
extern void DcliWInit();
extern int wtp_check_wtp_ip_addr(char *ipaddr, char *WTPIP);
extern int create_wtp(dbus_parameter parameter, DBusConnection *connection,char * id, char *wtp_name, char* wtp_model, char *wtp_sn);  
																									/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																									/*����-2��ʾwtp id should be 1 to WTP_NUM-1������-3��ʾwtp name is too long,should be 1 to DEFAULT_LEN-1*/
																									/*����-4��ʾthe model is wrong������-5��ʾwtp id exist*/
																									/*����-6��ʾcreate wtp count reach to max wtp count������-7��ʾwtp sn exist*/
																									/*����-8��ʾerror������-9��ʾwtp sn is too long,should be 1 to 127*/
																									/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																									
extern int create_wtp_bymac_cmd_func(dbus_parameter parameter, DBusConnection *connection,char *id, char *wtp_name, char* wtp_model, char *wtp_mac); 
																									/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																									/*����-2��ʾwtp id should be 1 to WTP_NUM-1������-3��ʾwtp name is too long,should be 1 to DEFAULT_LEN-1*/
																									/*����-4��ʾUnknow mac addr format������-5��ʾinput should not be broadcast or multicast mac*/
																									/*����-6��ʾthe model is wrong������-7��ʾwtp id exist*/
																									/*����-8��ʾcreate wtp count reach to max wtp count������-9��ʾwtp mac exist������-10��ʾerror*/
																									/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																														
extern int delete_wtp(dbus_parameter parameter, DBusConnection *connection,int id); /*����0��ʾɾ��ʧ�ܣ�����1��ʾɾ���ɹ�*/
																					 /*����-1��ʾinput wtp id should be 1 to WTP_NUM-1*/
																					 /*����-2��ʾwtp id not exist������-3��ʾplease unused first*/
																					 /*����-4��ʾerror������-5��ʾinput wtp has some radios interface in ebr,please delete it first*/
																					 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_one_wtp_head(DCLI_WTP_API_GROUP_ONE *WTPINFO);
/*����1ʱ������Free_one_wtp_head()�ͷſռ�*/
extern int show_wtp_one(dbus_parameter parameter, DBusConnection *connection,int id,DCLI_WTP_API_GROUP_ONE **WTPINFO);
																					  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
		                                                                              /*����-1��ʾwtp id should be 1 to WTP_NUM-1*/
																					  /*����-2��ʾwtp�����ڣ�����-3��ʾerror*/		
																					  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																					  
extern void Free_wtp_list_new_head(DCLI_WTP_API_GROUP_ONE *WTPINFO);
/*����1ʱ������Free_wtp_list_new_head()�ͷſռ�*/
extern int show_wtp_list_new_cmd_func(dbus_parameter parameter, DBusConnection *connection,DCLI_WTP_API_GROUP_ONE **WTPINFO);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾno wtp exist*/
																																	 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																					  
extern void Free_wtp_list_by_sn_head(DCLI_WTP_API_GROUP_ONE *WTPINFO);
/*����1ʱ������Free_wtp_list_by_sn_head()�ͷſռ�*/
extern int show_wtp_list_by_sn_cmd_func(dbus_parameter parameter, DBusConnection *connection,DCLI_WTP_API_GROUP_ONE **WTPINFO);  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																																		 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_wtp_list_by_mac_head(DCLI_WTP_API_GROUP_ONE *WTPINFO);
/*����1ʱ������Free_wtp_list_by_mac_head()�ͷſռ�*/
extern int show_wtp_list_by_mac_cmd_func(dbus_parameter parameter, DBusConnection *connection,DCLI_WTP_API_GROUP_ONE **WTPINFO);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																																		 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_wtp_apply_interface_group(struct WtpList *WtpList_Head);

/*����-7ʱ������Free_wtp_apply_interface_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int wtp_apply_interface_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *inter_name,struct WtpList **WtpList_Head);
						/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾthe length of interface name excel 16*/
						/*����-2��ʾinterface does not exist������-3��ʾif you want to change binding interface, please delete binding wlan id first*/
						/*����-4��ʾerror������-5ʾWTP ID�Ƿ�������-6��ʾGroup ID�Ƿ�������-7��ʾpartial failure*/
						/*����-8��ʾis no local interface, permission denial*/
#endif

extern int wtp_apply_interface(dbus_parameter parameter, DBusConnection *connection,int id,char *inter_name);  
																			  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾthe length of interface name excel 16*/
																			  /*����-2��ʾinterface does not exist������-3��ʾif you want to change binding interface, please delete binding wlan id first*/
																			  /*����-4��ʾerror������-5ʾWTP ID�Ƿ�*/
																			  /*����-8��ʾis no local interface, permission denial*/
																			  /*����-9��ʾif you want to change binding interface, please unused wtp  first*/
																			  /*����-10��ʾinterface has be binded in other hansi*/
																			  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																			  
extern int wtp_apply_ipv6interface(dbus_parameter parameter, DBusConnection *connection,int id,char *inter_name);
																			    /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																				/*����-1��ʾthe length of input parameter is excel the limit of 16*/
																				/*����-2��ʾwtp id does not exist������-3��ʾwtp is using*/
																				/*����-4��ʾinterface does not exist������-5��ʾapply interface failed*/
																				/*����-6��ʾif you want to change binding interface, please delete binding wlan id first*/
																				/*����-7��ʾwtp bingding ipv6 addr error make sure interface have ipv6 address*/
																				/*����-8��ʾerror������-9ʾWTP ID�Ƿ�*/
																				/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_wtp_apply_wlan_group(struct WtpList *WtpList_Head);

/*����-15ʱ������Free_wtp_apply_wlan_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*ע���ж�wlan ID�ĺϷ���1--15*/
extern int wtp_apply_wlan_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,int wlanID,struct WtpList **WtpList_Head);
										/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾWTP ID Not existed*/
										/*����-2��ʾWTP be using, you can't binding wlan ID������-3��ʾ binding wlan is not exist*/
										/*����-4��ʾWlan does not binding interface������-5��ʾ Wtp does not binding interface*/
										/*����-6��ʾwlan and wtp binding interface don't match������-7��ʾ Clear wtp binding wlan list successfully*/
										/*����-8��ʾwlan being enable������-9��ʾwtp over max bss count*/
										/*����-10��ʾwtp over max wep wlan count������-11��ʾ error������-12ʾWTP ID�Ƿ�*/
										/*����-13��ʾwep conflict, wtp binding wlan securityindex is same with others������-14��ʾGroup ID�Ƿ�*/
										/*����-15��ʾpartial failure*/
#endif

extern int wtp_apply_wlan(dbus_parameter parameter, DBusConnection *connection,int wtpID,int wlanID);   
																	   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾWTP ID Not existed*/
				                                                       /*����-2��ʾWTP be using, you can't binding wlan ID������-3��ʾ binding wlan is not exist*/
				                                                       /*����-4��ʾWlan does not binding interface������-5��ʾ Wtp does not binding interface*/
				                                                       /*����-6��ʾwlan and wtp binding interface don't match������-7��ʾ Clear wtp binding wlan list successfully*/
																	   /*����-8��ʾwlan being enable������-9��ʾwtp over max bss count*/
																	   /*����-10��ʾwtp over max wep wlan count������-11��ʾ error������-12ʾWTP ID�Ƿ�*/
																	   /*����-13��ʾwep conflict, wtp binding wlan securityindex is same with others*/
																	   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*wlanID==0����ʾall*/																	   
extern int wtp_delete_wlan(dbus_parameter parameter, DBusConnection *connection,int wtpID,int wlanID);    
																	    /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
				                                                        /*����-1��ʾ wtp id not exist������-2��ʾWTP be using, you can't binding wlan ID*/
				                                                        /*����-3��ʾwlan is not exist������-4��ʾ Wlan does not binding interface*/
				                                                        /*����-5��ʾWtp does not binding interface������-6��ʾ wlan and wtp binding interface don't match*/
				                                                        /*����-7��ʾClear wtp binding wlan list successfully������-8��ʾ wlan being enable*/
				                                                        /*����-9��ʾBSS being enable������-10��ʾwtp doesn't binding this wlan id������-11��ʾ error*/
																		/*����-12ʾWTP ID�Ƿ�������-13��ʾwtp interface is in ebr,please delete it from ebr first*/
																		/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_wtp_disable_wlan_group(struct WtpList *WtpList_Head);

/*����-8ʱ������Free_wtp_disable_wlan_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*ע���ж�wlan ID�ĺϷ���1--15*/
extern int wtp_disable_wlan_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,int wlanID,struct WtpList **WtpList_Head);
										/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾWTP ID Not existed*/
										/*����-2��ʾbinding wlan is not exist������-3��ʾ Wtp does not binding this wlan*/
										/*����-4��ʾwtp binding wlan id not match������-5��ʾ error������-6ʾWTP ID�Ƿ�*/
										/*����-7��ʾGroup ID�Ƿ�������-8��ʾpartial failure������-9��ʾgroup id does not exist*/
#endif

extern int wtp_disable_wlan(dbus_parameter parameter, DBusConnection *connection,int wtpID,int wlanID);	
																		/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾWTP ID Not existed*/
																	 	/*����-2��ʾbinding wlan is not exist������-3��ʾ Wtp does not binding this wlan*/
																		/*����-4��ʾwtp binding wlan id not match������-5��ʾ error������-6ʾWTP ID�Ƿ�*/
																		/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_wtp_enable_wlan_group(struct WtpList *WtpList_Head);

/*����-13ʱ������Free_wtp_enable_wlan_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*ע���ж�wlan ID�ĺϷ���1--15*/
extern int wtp_enable_wlan_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,int wlanID,struct WtpList **WtpList_Head);
														/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾWTP ID Not existed*/
														/*����-2��ʾbinding wlan is not exist������-3��ʾ Wtp does not binding this wlan*/
														/*����-4��ʾwtp not in run state������-5��ʾ wtp binding wlan id not match*/
														/*����-6��ʾWlan is not enable������-7��ʾmap L3 interace no ip error*/
														/*����-8��ʾBSS interface policy conflict������-9����wtp over max wep wlan count*/
														/*����-10��ʾ error������-11ʾWTP ID�Ƿ�������-12��ʾGroup ID�Ƿ�*/
														/*����-13��ʾpartial failure������-14��ʾgroup id does not exist*/
#endif

extern int wtp_enable_wlan(dbus_parameter parameter, DBusConnection *connection,int wtpID,int wlanID);	
																		/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾWTP ID Not existed*/
																   	    /*����-2��ʾbinding wlan is not exist������-3��ʾ Wtp does not binding this wlan*/
																		/*����-4��ʾwtp not in run state������-5��ʾ wtp binding wlan id not match*/
																		/*����-6��ʾWlan is not enable������-7��ʾmap L3 interace no ip error*/
																		/*����-8��ʾBSS interface policy conflict������-9����wtp over max wep wlan count*/
																		/*����-10��ʾ error������-11ʾWTP ID�Ƿ�*/
																		/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_wtp_used_group(struct WtpList *WtpList_Head);

/*����-9ʱ������Free_wtp_used_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*use_sta=0��ʾ"used"��use_sta=1��ʾ"unused"*/
extern int wtp_used_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,int use_sta,struct WtpList **WtpList_Head);
				/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾWTP ID Not existed������-2��ʾYou should be apply interface first*/
				/*����-3��ʾYou should be apply wlan id first������-4��ʾmap L3 interace error������-5��ʾBSS interface policy conflict*/
				/*����-6��ʾerror������-7ʾWTP ID�Ƿ�������-8��ʾGroup ID�Ƿ�������-9��ʾpartial failure*/
#endif

extern int wtp_used(dbus_parameter parameter, DBusConnection *connection,int id,int use_sta);
														   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾWTP ID Not existed������-2��ʾYou should be apply interface first*/
				                                           /*����-3��ʾYou should be apply wlan id first������-4��ʾmap L3 interace error������-5��ʾBSS interface policy conflict*/
				                                           /*����-6��ʾerror������-7ʾWTP ID�Ƿ�*/
														   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_wtp_model(DCLI_WTP_API_GROUP_ONE *WTPINFO);
/*����1ʱ������Free_wtp_model()�ͷſռ�*/
extern  int show_version(dbus_parameter parameter, DBusConnection *connection,DCLI_WTP_API_GROUP_ONE **WTPINFO);/*����1��ʾ�ɹ�������0��ʾʧ��*/
																												  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern  int delete_model(dbus_parameter parameter, DBusConnection *connection,char*model);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
											 											   /*����-1��ʾmodel does not exist or the model is system default model*/
											 											   /*����-2��ʾerror*/            
																						   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern  int set_ap_model(dbus_parameter parameter, DBusConnection *connection,char*model,char*ver,char*path,char* radio,char *bss);  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwireless-control does not surport this model������-2��ʾerror*/
													   																					/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*echotime�ķ�Χ��3-30*/
/*wtp_idΪ0ʱ����ʾȫ������*/
extern  int set_ap_echotimer(dbus_parameter parameter, DBusConnection *connection,int wtp_id,int echotime); /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror������-2��ʾWTP ID�Ƿ�������-3��ʾinput echotimer should be 3~30*/
																											   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void free_show_ap_echotimer(DCLI_WTP_API_GROUP_THREE *WTPINFO);
/*retu =1 ��ʱ������ͷź���free_show_ap_echotimer(DCLI_WTP_API_GROUP_THREE *WTPINFO)*/
extern 	int show_ap_echotimer(dbus_parameter parameter, DBusConnection *connection,DCLI_WTP_API_GROUP_THREE **WTPINFO); /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																															 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int download_ap_version(char *url,char *user,char *passwd);//����ap�����°汾

#if _GROUP_POLICY
extern void Free_config_wtp_max_sta_num_group(struct WtpList *WtpList_Head);

/*����-8ʱ������Free_config_wtp_max_sta_num_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int config_wtp_max_sta_num_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char*max_num,struct WtpList **WtpList_Head,int *fail_num);  
															/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp not exist*/
															/*����-2��ʾmore sta(s) has accessed before you set max sta num*/
															/*����-3��ʾoperation fail������-4��ʾerror������-5ʾWTP ID�Ƿ�*/
															/*����-6��ʾunknown NUM format������-7��ʾGroup ID�Ƿ�*/
															/*����-8��ʾpartial failure������-9��ʾgroup id does not exist*/
															/*����-10��ʾinput num should be 0-64*/
#endif
/*max_num�ķ�Χ��0-32767*/
extern int config_wtp_max_sta_num(dbus_parameter parameter, DBusConnection *connection,int id,char*max_num);  
																				/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp not exist*/
																				/*����-2��ʾmore sta(s) has accessed before you set max sta num*/
																				/*����-3��ʾoperation fail������-4��ʾerror������-5ʾWTP ID�Ƿ�*/
																				/*����-10��ʾinput num should be 0-32767*/
																				/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_config_wtp_triger_num_group(struct WtpList *WtpList_Head);

/*����-7ʱ������Free_config_wtp_triger_num_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*triger_num�ķ�Χ��1-64*/
extern int config_wtp_triger_num_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char* triger_num,struct WtpList **WtpList_Head);
												/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp id does not exist*/
												/*����-2��ʾoperation fail������-3��ʾtriger number must be little than max sta num*/
												/*����-4��ʾerror������-5ʾWTP ID�Ƿ�������-6��ʾGroup ID�Ƿ�*/
												/*����-7��ʾpartial failure������-8��ʾgroup id does not exist*/
												/*����-9��ʾunknown id format������-10��ʾinput triger num should be 1~64*/
#endif

/*triger_num�ķ�Χ��1-64*/
extern int config_wtp_triger_num(dbus_parameter parameter, DBusConnection *connection,int id,char* triger_num);	/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp id does not exist������-2��ʾoperation fail������-3��ʾtriger number must be little than max sta num������-4��ʾerror������-5ʾWTP ID�Ƿ�*/
																													/*����-9��ʾunknown id format������-10��ʾinput triger num should be 1~64*/
																													/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																				
#if _GROUP_POLICY
extern void Free_set_wtp_flow_trige_group(struct WtpList *WtpList_Head);

/*����-7ʱ������Free_set_wtp_flow_trige_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*triger_num�ķ�Χ��0-1024*/
extern int set_wtp_flow_trige_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char* triger_num,struct WtpList **WtpList_Head);
											/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp id does not exist*/
											/*����-2��ʾoperation fail������-3��ʾflow triger must be <0-1024>*/
											/*����-4��ʾerror������-5ʾWTP ID�Ƿ�������-6��ʾGroup ID�Ƿ�*/
											/*����-7��ʾpartial failure������-8��ʾgroup id does not exist*/
											/*����-9��ʾunknown id format������-10��ʾinput flow triger should be 0~1024*/
#endif

extern int set_wtp_flow_trige(dbus_parameter parameter, DBusConnection *connection,int id,char* triger_num);  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp id does not exist������-2��ʾoperation fail������-3��ʾflow triger must be <0-1024>������-4��ʾerror������-5ʾWTP ID�Ƿ�*/
																												/*����-9��ʾunknown id format������-10��ʾinput flow triger should be 0~1024*/
																												/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_ap_max_throughout_group(struct WtpList *WtpList_Head);

/*����-8ʱ������Free_set_ap_max_throughout_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_ap_max_throughout_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *para,struct WtpList **WtpList_Head);
													/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
													/*����-2��ʾap max throughout should be 1 to 108������-3��ʾwtp id does not exist*/
													/*����-4��ʾerror������-5ʾWTP ID�Ƿ�*/
													/*����-6��ʾillegal input:Input exceeds the maximum value of the parameter type*/
													/*����-7��ʾGroup ID�Ƿ�������-8��ʾpartial failure*/
													/*����-9��ʾgroup id does not exist*/
#endif

extern int set_ap_max_throughout(dbus_parameter parameter, DBusConnection *connection,int id,char *para); /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																											   /*����-2��ʾap max throughout should be 1 to 108������-3��ʾwtp id does not exist*/
																											   /*����-4��ʾerror������-5ʾWTP ID�Ƿ�*/
																											   /*����-6��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																											   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_ap_extension_command_func_group(struct WtpList *WtpList_Head);

/*����-6ʱ������Free_set_ap_extension_command_func_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*����������AC��ֱ�Ӳ���AP��һ��������������Ÿ��û���WEB�ϲ���ʵ�֡�*/
extern int set_ap_extension_command_func_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,const char *command,struct WtpList **WtpList_Head);
												   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
												   /*����-2��ʾwtp id does not exist������-3��ʾerror������-4ʾWTP ID�Ƿ�*/
												   /*����-5��ʾGroup ID�Ƿ�������-6��ʾpartial failure������-7��ʾgroup id does not exist*/
#endif

extern int set_ap_extension_command_func(dbus_parameter parameter, DBusConnection *connection,int WtpId,const char *command);
																										/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																										/*����-2��ʾwtp id does not exist������-3��ʾerror������-4ʾWTP ID�Ƿ�*/
																										/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																										
extern int set_ap_ip_address(dbus_parameter parameter, DBusConnection *connection,int id,char *IP,char *MASK,char *GATEWAY);  
																							/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown ip format������-2��ʾunknown id format*/
																							/*����-3��ʾunknown mask parameters������-4��ʾwtp id does not exist������-5��ʾerror������-6ʾWTP ID�Ƿ�*/
																							/*����-7��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																							/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																										
extern void free_show_wtp_runtime(DCLI_WTP_API_GROUP_THREE *WTPINFO);
/*retu==1�����ͷź���free_show_wtp_runtime(DCLI_WTP_API_GROUP_THREE *WTPINFO)*/
extern int show_wtp_runtime(dbus_parameter parameter, DBusConnection *connection,int id,DCLI_WTP_API_GROUP_THREE **WTPINFO);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp id does not exist������-2��ʾWTP ID�Ƿ�*/
																																/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_wtp_location_group(struct WtpList *WtpList_Head);

/*����-5ʱ������Free_set_wtp_location_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_wtp_location_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *Location,struct WtpList **WtpList_Head);
										/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp location is too long,should be 1 to 15*/
										/*����-2��ʾwtp id does not exist������-3ʾWTP ID�Ƿ�������-4��ʾGroup ID�Ƿ�*/
										/*����-5��ʾpartial failure������-6��ʾgroup id does not exist*/
#endif

extern int set_wtp_location(dbus_parameter parameter, DBusConnection *connection,int id,char *Location); /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp location is too long,should be 1 to 15������-2��ʾwtp id does not exist������-3ʾWTP ID�Ƿ�*/
																										   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																		
extern void free_show_wtp_location(DCLI_WTP_API_GROUP_THREE *WTPINFO);
/*����1ʱ������free_show_wtp_location()�ͷſռ�*/
extern int show_wtp_location(dbus_parameter parameter, DBusConnection *connection,int id,DCLI_WTP_API_GROUP_THREE **WTPINFO);   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp does not set location������-2��ʾwtp id does not exist������-3ʾWTP ID�Ƿ�*/
																																	/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_ap_ip_bywtpid(DCLI_AC_API_GROUP_THREE *network);
/*����1ʱ������Free_ap_ip_bywtpid()�ͷſռ�*/
extern int show_ap_ip_bywtpid_cmd(dbus_parameter parameter, DBusConnection *connection,int id,DCLI_AC_API_GROUP_THREE **network);
																							   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput wtp id should be 1 to WTP_NUM-1*/
																							   /*����-2��ʾap have not ip information������-3��ʾwtp id no exist*/
																							   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_ap_model_infomation(DCLI_AC_API_GROUP_FOUR *modelinfo);
/*����1ʱ������Free_ap_model_infomation()�ͷſռ�*/																								   
extern int show_ap_model_infomation(dbus_parameter parameter, DBusConnection *connection,char *modetype,DCLI_AC_API_GROUP_FOUR **modelinfo);
																								/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput model is wrong*/
																							    /*����-2��ʾac support model does not set������-3��ʾthis model does not supportted, set it first*/
																							    /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_wtp_netid_group(struct WtpList *WtpList_Head);

/*����-5ʱ������Free_set_wtp_netid_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_wtp_netid_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char*content,struct WtpList **WtpList_Head);
													  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
													  /*����-1��ʾwtp location is too long,should be 1 to 32*/
													  /*����-2��ʾwtp id does not exist������-3ʾWTP ID�Ƿ�*/
													  /*����-4��ʾGroup ID�Ƿ�������-5��ʾpartial failure*/
													  /*����-6��ʾgroup id does not exist*/
#endif

extern int set_wtp_netid(dbus_parameter parameter, DBusConnection *connection,int id,char*content);
																/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																/*����-1��ʾwtp location is too long,should be 1 to 32*/
																/*����-2��ʾwtp id does not exist������-3ʾWTP ID�Ƿ�*/
																/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
														
extern void free_show_wtp_netid(DCLI_WTP_API_GROUP_THREE *WTPINFO);
/*retu ==1 �����ͷź���free_show_wtp_netid(DCLI_WTP_API_GROUP_THREE *WTPINFO)*/																
extern int show_wtp_netid(dbus_parameter parameter, DBusConnection *connection,int id,DCLI_WTP_API_GROUP_THREE **WTPINFO);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp does not set netid������-2��ʾ wtp id does not exist������-3ʾWTP ID�Ƿ�*/
																															  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																
extern int show_wtp_extension_information(int id,wid_wifi_info *head);

extern void free_show_wtp_sample_throughput_information(DCLI_WTP_API_GROUP_TWO *INFO);
/*retu==1 �����ͷź���free_show_wtp_sample_throughput_information(DCLI_WTP_API_GROUP_TWO *INFO)*/
extern int show_wtp_sample_throughput_information(dbus_parameter parameter, DBusConnection *connection,int id,DCLI_WTP_API_GROUP_TWO **INFO);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp is not in run state������-2��ʾwtp id does not exist������-3��ʾsample switch disable,enable it first������-4ʾWTP ID�Ƿ�*/
																																					   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_wtp_wtpname_group(struct WtpList *WtpList_Head);

/*����-5ʱ������Free_set_wtp_wtpname_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_wtp_wtpname_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char * wtpname,struct WtpList **WtpList_Head);
																/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																/*����-1��ʾwtp name is too long,should be 1 to DEFAULT_LEN-1*/
																/*����-2��ʾwtp id does not exist������-3ʾWTP ID�Ƿ�*/
																/*����-4��ʾGroup ID�Ƿ�������-5��ʾpartial failure*/
																/*����-6��ʾgroup id does not exist*/
#endif

extern int set_wtp_wtpname(dbus_parameter parameter, DBusConnection *connection,int id,char * wtpname);
																	   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																	   /*����-1��ʾwtp name is too long,should be 1 to DEFAULT_LEN-1*/
																	   /*����-2��ʾwtp id does not exist������-3ʾWTP ID�Ƿ�*/
																	   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_ap_extension_infomation_enable_group(struct WtpList *WtpList_Head);

/*����-7ʱ������Free_set_ap_extension_infomation_enable_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_ap_extension_infomation_enable_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *state,struct WtpList **WtpList_Head);
						   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'enable' or 'disable'*/
						   /*����-2��ʾwtp id does not exist������-3��ʾwtp id does not run������-4��ʾerror*/
						   /*����-5ʾWTP ID�Ƿ�������-6��ʾGroup ID�Ƿ�������-7��ʾpartial failure*/
						   /*����-8��ʾgroup id does not exist*/
#endif

extern int set_ap_extension_infomation_enable(dbus_parameter parameter, DBusConnection *connection,int wtp_id,char *state);
																							/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'enable' or 'disable'*/
																							/*����-2��ʾwtp id does not exist������-3��ʾwtp id does not run������-4��ʾerror������-5ʾWTP ID�Ƿ�*/
																							/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_ap_extension_infomation_reportinterval_group(struct WtpList *WtpList_Head);

/*����-9ʱ������Free_set_ap_extension_infomation_reportinterval_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_ap_extension_infomation_reportinterval_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *para,struct WtpList **WtpList_Head);
									/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
									/*����-2��ʾap extension infomation reportinterval error������-3��ʾwtp is not in run state*/
									/*����-4��ʾwtp id does not exist������-5��ʾerror������-6ʾWTP ID�Ƿ�*/
									/*����-7��ʾillegal input:Input exceeds the maximum value of the parameter type*/
									/*����-8��ʾGroup ID�Ƿ�������-9��ʾpartial failure������-10��ʾgroup id does not exist*/
#endif

extern int set_ap_extension_infomation_reportinterval(dbus_parameter parameter, DBusConnection *connection,int WTPID,char *para);  
																									/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																									/*����-2��ʾap extension infomation reportinterval error������-3��ʾwtp is not in run state*/
																									/*����-4��ʾwtp id does not exist������-5��ʾerror������-6ʾWTP ID�Ƿ�*/
																									/*����-7��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																									/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void free_show_wtp_wlan_vlan_information(DCLI_WTP_API_GROUP_THREE *WTPINFO);
/*retu==1,����free_show_wtp_wlan_vlan_information(DCLI_WTP_API_GROUP_THREE *WTPINFO)*/																									
extern int show_wtp_wlan_vlan_information(dbus_parameter parameter, DBusConnection *connection,int wtp_id,DCLI_WTP_API_GROUP_THREE **WTPINFO);  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp id does not exist������-2��ʾWTP ID�Ƿ�*/
																																						/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int show_wtp_extension_information_v2(int wtp_id,wid_wifi_info *exten_info);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp id does not exist������-2��ʾwtp extension info report switch disable������-3ʾWTP ID�Ƿ�*/

extern void free_how_wtp_extension_information_v3(DCLI_WTP_API_GROUP_TWO *WTPINFO);
/*retu==1 �����ͷź���free_how_wtp_extension_information_v3(DCLI_WTP_API_GROUP_TWO *WTPINFO)*/ 																									
extern int show_wtp_extension_information_v3(dbus_parameter parameter, DBusConnection *connection,int wtp_id,DCLI_WTP_API_GROUP_TWO **WTPINFO);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp id does not exist������-2��ʾwtp extension info report switch disable������-3ʾWTP ID�Ƿ�*/
																																					   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int show_wtp_ethernet_interface_information(dbus_parameter parameter, DBusConnection *connection,int wtp_id,struct wtp_eth_inf_infor_profile *eth_inf_info_head,int *NUM,int *temperature);  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp id does not exist������-2��ʾWTP ID�Ƿ�*/
																																					   													   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_ap_l2_isolation_func_group(struct WtpList *WtpList_Head);

/*����-10ʱ������Free_set_ap_l2_isolation_func_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*wlan_id�ķ�Χ��1-15*/
/*stateΪ"enable"��"disable"*/
extern int set_ap_l2_isolation_func_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,int wlan_id,int state,struct WtpList **WtpList_Head);
																	/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																	/*����-1��ʾinput patameter only with 'enable' or 'disable'*/
																	/*����-2��ʾwlan not exist������-3��ʾwtp not binding wlan*/
																	/*����-4��ʾwtp id does not run������-5��ʾbinding wlan error*/
																	/*����-6��ʾerror������-7ʾWTP ID�Ƿ�*/
																	/*����-8��ʾwlan l2 isolation state already������-9��ʾGroup ID�Ƿ�*/
																	/*����-10��ʾpartial failure������-11��ʾgroup id does not exist*/
																	/*����-12��ʾinput wlanid should be 1~15*/
#endif

/*wlan_id�ķ�Χ��1-15*/
extern int set_ap_l2_isolation_func(dbus_parameter parameter, DBusConnection *connection,int wtp_id,int wlan_id,int state);	
																							/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																							/*����-1��ʾinput patameter only with 'enable' or 'disable'*/
																							/*����-2��ʾwlan not exist������-3��ʾwtp not binding wlan*/
																							/*����-4��ʾwtp id does not run������-5��ʾbinding wlan error*/
																							/*����-6��ʾerror������-7ʾWTP ID�Ƿ�*/
																							/*����-8��ʾwlan l2 isolation state already*/
																							/*����-12��ʾinput wlanid should be 1~15*/
																							/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_ap_dos_def_func_group(struct WtpList *WtpList_Head);

/*����-6ʱ������Free_set_ap_dos_def_func_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*stateΪ1��ʾstate��stateΪ0��ʾdisable*/
extern int set_ap_dos_def_func_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,int state,struct WtpList **WtpList_Head);
						/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'enable' or 'disable'*/
						/*����-2��ʾwtp id does not run������-3��ʾerror������-4ʾWTP ID�Ƿ�*/
						/*����-5��ʾGroup ID�Ƿ�������-6��ʾpartial failure������-7��ʾgroup id does not exist*/
#endif

/*stateΪ1��ʾenable��stateΪ0��ʾdisable*/																							
extern int set_ap_dos_def_func(dbus_parameter parameter, DBusConnection *connection,int wtp_id,int state); /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'enable' or 'disable'������-2��ʾwtp id does not run������-3��ʾerror������-4ʾWTP ID�Ƿ�*/
																												/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_ap_igmp_snoop_func_group(struct WtpList *WtpList_Head);

/*����-6ʱ������Free_set_ap_igmp_snoop_func_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*stateΪ1��ʾenable��stateΪ0��ʾdisable*/
extern int set_ap_igmp_snoop_func_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,int state,struct WtpList **WtpList_Head);
					/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'enable' or 'disable'*/
					/*����-2��ʾwtp id does not run������-3��ʾerror������-4ʾWTP ID�Ƿ�*/
					/*����-5��ʾGroup ID�Ƿ�������-6��ʾpartial failure������-7��ʾgroup id does not exist*/
#endif

/*stateΪ1��ʾenable��stateΪ0��ʾdisable*/																							
extern int set_ap_igmp_snoop_func(dbus_parameter parameter, DBusConnection *connection,int wtp_id,int state); /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'enable' or 'disable'������-2��ʾwtp id does not run������-3��ʾerror������-4ʾWTP ID�Ƿ�*/
																													/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void free_show_ap_mib_information(DCLI_WTP_API_GROUP_TWO *WTPINFO);
/*retu==1,�����ͷź���free_show_ap_mib_information(DCLI_WTP_API_GROUP_TWO *WTPINFO)*/
extern int show_ap_mib_information_func(dbus_parameter parameter, DBusConnection *connection,int wtp_id,DCLI_WTP_API_GROUP_TWO **WTPINFO); /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp id does not exist������-2��ʾWTP ID�Ƿ�*/
																																				  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void free_show_ap_cm_statistics(DCLI_WTP_API_GROUP_TWO *WTPINFO);
/*����1ʱ������free_show_ap_cm_statistics()�ͷſռ�*/
extern int show_ap_cm_statistics_func(dbus_parameter parameter, DBusConnection *connection,int wtp_id,DCLI_WTP_API_GROUP_TWO **WTPINFO);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp id does not exist������-2��ʾWTP ID�Ƿ�*/
																																			  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_ap_sta_infomation_report_enable_func_group(struct WtpList *WtpList_Head);

/*����-7ʱ������Free_set_ap_sta_infomation_report_enable_func_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_ap_sta_infomation_report_enable_func_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *state,struct WtpList **WtpList_Head);
									/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'enable' or 'disable'*/
									/*����-2��ʾwtp id does not exist������-3��ʾwtp id does not run������-4��ʾerror*/
									/*����-5ʾWTP ID�Ƿ�������-6��ʾGroup ID�Ƿ�������-7��ʾpartial failure*/
									/*����-8��ʾgroup id does not exist*/
#endif

extern int set_ap_sta_infomation_report_enable_func(dbus_parameter parameter, DBusConnection *connection,int wtp_id,char *state); 
																									/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'enable' or 'disable'*/
																									/*����-2��ʾwtp id does not exist������-3��ʾwtp id does not run������-4��ʾerror������-5ʾWTP ID�Ƿ�*/
																									/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_ap_sta_infomation_reportinterval_cmd_func_group(struct WtpList *WtpList_Head);

/*����-9ʱ������Free_set_ap_sta_infomation_reportinterval_cmd_func_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_ap_sta_infomation_reportinterval_cmd_func_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *para,struct WtpList **WtpList_Head);
												/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
												/*����-2��ʾap sta infomation reportinterval error������-3��ʾwtp is not in run state*/
												/*����-4��ʾwtp id does not exist������-5��ʾerror������-6ʾWTP ID�Ƿ�*/
												/*����-7��ʾillegal input:Input exceeds the maximum value of the parameter type*/
												/*����-8��ʾGroup ID�Ƿ�������-9��ʾpartial failure*/
												/*����-10��ʾgroup id does not exist*/
#endif

/*para�ķ�Χ��5-32767*/
extern int set_ap_sta_infomation_reportinterval_cmd_func(dbus_parameter parameter, DBusConnection *connection,int WtpID,char *para); 
																								/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																								/*����-2��ʾap sta infomation reportinterval error,should be 5-32767������-3��ʾwtp is not in run state*/
																								/*����-4��ʾwtp id does not exist������-5��ʾerror������-6ʾWTP ID�Ƿ�*/
																								/*����-7��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																								/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_ap_if_info_report_enable_func_group(struct WtpList *WtpList_Head);

/*����-7ʱ������Free_set_ap_if_info_report_enable_func_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*stateΪ"enable"��"disable"*/
extern int set_ap_if_info_report_enable_func_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *state,struct WtpList **WtpList_Head);
									/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'enable' or 'disable'*/
									/*����-2��ʾwtp id does not exist������-3��ʾwtp id does not run������-4��ʾerror*/
									/*����-5ʾWTP ID�Ƿ�������-6��ʾGroup ID�Ƿ�������-7��ʾpartial failure*/
									/*����-8��ʾgroup id does not exist*/
#endif

/*stateΪ"enable"��"disable"*/
extern int set_ap_if_info_report_enable_func(dbus_parameter parameter, DBusConnection *connection,int wtp_id,char *state); 
																							/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'enable' or 'disable'*/
																							/*����-2��ʾwtp id does not exist������-3��ʾwtp id does not run������-4��ʾerror������-5ʾWTP ID�Ƿ�*/
																							/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_ap_if_info_reportinterval_cmd_func_group(struct WtpList *WtpList_Head);

/*����-9ʱ������Free_set_ap_if_info_reportinterval_cmd_func_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_ap_if_info_reportinterval_cmd_func_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *para,struct WtpList **WtpList_Head);
													/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
													/*����-2��ʾap sta infomation reportinterval error������-3��ʾwtp is not in run state*/
													/*����-4��ʾwtp id does not exist������-5��ʾerror������-6ʾWTP ID�Ƿ�*/
													/*����-7��ʾillegal input:Input exceeds the maximum value of the parameter type*/
													/*����-8��ʾGroup ID�Ƿ�������-9��ʾpartial failure������-10��ʾgroup id does not exist*/
#endif

extern int set_ap_if_info_reportinterval_cmd_func(dbus_parameter parameter, DBusConnection *connection,int WtpID,char *para);   
																						/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																						/*����-2��ʾap sta infomation reportinterval error������-3��ʾwtp is not in run state*/
																						/*����-4��ʾwtp id does not exist������-5��ʾerror������-6ʾWTP ID�Ƿ�*/
																						/*����-7��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																						/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_ap_if_updown_func_group(struct WtpList *WtpList_Head);

/*����-9ʱ������Free_set_ap_if_updown_func_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*inter_typeΪ"eth"��"wifi"*/
/*stateΪ"uplink"��"downlink"*/
extern int set_ap_if_updown_func_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *inter_type,char *If_index,char *state,struct WtpList **WtpList_Head);
								/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput interface only with 'ath' 'eth' or 'wifi'*/
								/*����-2��ʾunknown id format������-3��ʾinput interface only with 'uplink' or 'downlink'*/
								/*����-4��ʾwtp is not in run state������-5��ʾwtp id does not exist������-6��ʾerror*/
								/*����-7ʾWTP ID�Ƿ�������-8��ʾGroup ID�Ƿ�������-9��ʾpartial failure*/
								/*����-10��ʾgroup id does not exist*/
#endif

extern int set_ap_if_updown_func(dbus_parameter parameter, DBusConnection *connection,int WtpID,char *inter_type,char *id,char *state); 
																						/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput interface only with 'ath' 'eth' or 'wifi'*/
																						/*����-2��ʾunknown id format������-3��ʾinput interface only with 'uplink' or 'downlink'*/
																						/*����-4��ʾwtp is not in run state������-5��ʾwtp id does not exist������-6��ʾerror������-7ʾWTP ID�Ƿ�*/
																						/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_ap_if_rate_cmd_group(struct WtpList *WtpList_Head);

/*����-9ʱ������Free_set_ap_if_rate_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*rate�ķ�Χ��"10","100"��"1000"*/
extern int set_ap_if_rate_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *if_index,char *if_rate,struct WtpList **WtpList_Head);
											/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
											/*����-2��ʾinput interface only with '10'  or '100' or '1000'������-3��ʾwtp is not in run state*/
											/*����-4��ʾwtp id does not exist������-5��ʾeth if_index does not exist*/
											/*����-6��ʾerror������-7ʾWTP ID�Ƿ�������-8��ʾGroup ID�Ƿ�*/
											/*����-9��ʾpartial failure������-10��ʾgroup id does not exist*/
#endif

/*rate�ķ�Χ��"10","100"��"1000"*/
extern int set_ap_if_rate_cmd(dbus_parameter parameter, DBusConnection *connection,int WtpID,char *if_index,char *if_rate);
																						  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																						  /*����-2��ʾinput interface only with '10' or '100' or '1000'������-3��ʾwtp is not in run state*/
																						  /*����-4��ʾwtp id does not exist������-5��ʾeth if_index does not exist*/
																						  /*����-6��ʾerror������-7ʾWTP ID�Ƿ�*/
																						  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void free_show_ap_if_info(DCLI_WTP_API_GROUP_TWO *WTPINFO);
/*retu=1,�ͷź���free_show_ap_if_info(DCLI_WTP_API_GROUP_TWO *WTPINFO)*/																						  
extern int show_ap_if_info_func(dbus_parameter parameter, DBusConnection *connection, int wtp_id,DCLI_WTP_API_GROUP_TWO **WTPINFO);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp id does not exist������-2��ʾwtp interface infomation report switch is disable������-3ʾWTP ID�Ƿ�*/
																																	   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_get_wtp_bss_pkt_info_func_group(struct WtpList *WtpList_Head);

/*����-6ʱ������Free_get_wtp_bss_pkt_info_func_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int get_wtp_bss_pkt_info_func_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,struct WtpList **WtpList_Head);
												/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾnot expect error*/
												/*����-2��ʾwtp id does not exist������-3��ʾerror������-4ʾWTP ID�Ƿ�*/
												/*����-5��ʾGroup ID�Ƿ�������-6��ʾpartial failure������-7��ʾgroup id does not exist*/
#endif

extern int get_wtp_bss_pkt_info_func(dbus_parameter parameter, DBusConnection *connection,int WtpID);   
																		/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾnot expect error*/
																		/*����-2��ʾwtp id does not exist������-3��ʾerror������-4ʾWTP ID�Ƿ�*/
																		/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void free_show_wtp_bss_pkt_info(DCLI_WTP_API_GROUP_TWO *INFO);
/*��retu==1�����ͷź���free_show_wtp_bss_pkt_info(DCLI_WTP_API_GROUP_TWO *INFO)*/																		
extern int show_wtp_bss_pkt_info_func(dbus_parameter parameter, DBusConnection *connection,int wtp_id,DCLI_WTP_API_GROUP_TWO **INFO);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp id does not exist������-2��ʾWTP ID�Ƿ�*/
																																			/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void free_show_wtp_eth_pkt_info_info(DCLI_WTP_API_GROUP_TWO *WTPINFO);
/*��retu==1�����ͷź���free_dcli_wtp_free_fun_two(DCLI_WTP_API_GROUP_TWO *WTPINFO)*/
extern int show_wtp_eth_pkt_info_func(dbus_parameter parameter, DBusConnection *connection,int wtp_id,DCLI_WTP_API_GROUP_TWO **WTPINFO);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp id does not exist������-2��ʾWTP ID�Ƿ�*/
																																			  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void free_show_wtp_radio_pkt_info(DCLI_WTP_API_GROUP_TWO *WTPINFO);
/*��retu==1ʱ������free_show_wtp_radio_pkt_info(DCLI_WTP_API_GROUP_TWO *WTPINFO)�ͷſռ�*/
extern int show_wtp_radio_pkt_info_func(dbus_parameter parameter, DBusConnection *connection,int wtp_id,DCLI_WTP_API_GROUP_TWO **WTPINFO);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp id does not exist������-2��ʾWTP ID�Ƿ�*/
																																				/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void free_show_wtp_wifi_snr(DCLI_WTP_API_GROUP_TWO *WTPINFO);
/*����1ʱ������free_show_wtp_wifi_snr()�ͷſռ�*/
extern int show_wtp_wifi_snr_func(dbus_parameter parameter, DBusConnection *connection,int wtp_id,DCLI_WTP_API_GROUP_TWO **WTPINFO);   
																										/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																										/*����-1��ʾwtp id does not exist*/
																										/*����-2��ʾwtp wifi snr report switch is������-3ʾWTP ID�Ƿ�*/ 	
																										/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_ac_ap_ntp_func_group(struct WtpList *WtpList_Head);

/*����-8ʱ������Free_set_ac_ap_ntp_func_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*typeΪ"start"��"stop"*/
/*value�ķ�Χ��60-65535*/
extern int set_ac_ap_ntp_func_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *Type,char *value,struct WtpList **WtpList_Head);
												/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
												/*����-2��ʾwtp id does not exist������-3��ʾerror������-4ʾWTP ID�Ƿ�*/
												/*����-5��ʾinput interface only with 'start' or 'stop'������-6��ʾinterval should be 60-65535*/
												/*����-7��ʾGroup ID�Ƿ�������-8��ʾpartial failure������-9��ʾgroup id does not exist*/
#endif

/*typeΪ"start"��"stop"*/
/*value�ķ�Χ��60-65535s��Ĭ��3600s*/
extern int set_ac_ap_ntp_func(dbus_parameter parameter, DBusConnection *connection,int WtpId,char *type,char *value);
																					/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																					/*����-2��ʾwtp id does not exist������-3��ʾerror������-4ʾWTP ID�Ƿ�*/
																					/*����-5��ʾinput interface only with 'start' or 'stop'������-6��ʾinterval should be 60-65535*/
																					/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_ap_config_update_func_group(struct WtpList *WtpList_Head);

/*����-7ʱ������Free_set_ap_config_update_func_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_ap_config_update_func_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *IP,struct WtpList **WtpList_Head);
											/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown ip format*/
											/*����-2��ʾwtp is not in run state������-3��ʾwtp id does not exist*/
											/*����-4��ʾerror������-5ʾWTP ID�Ƿ�������-6��ʾGroup ID�Ƿ�*/
											/*����-7��ʾpartial failure������-8��ʾgroup id does not exist*/
#endif

extern int set_ap_config_update_func(dbus_parameter parameter, DBusConnection *connection,int WtpID,char *IP); 
																				/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown ip format*/
																				/*����-2��ʾwtp is not in run state������-3��ʾwtp id does not exist*/
																				/*����-4��ʾerror������-5ʾWTP ID�Ƿ�*/
																				/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																				
extern void free_show_wtp_extension_information_v4(DCLI_WTP_API_GROUP_TWO *WTPINFO);
/*reut = 1 �����ͷź���free_show_wtp_extension_information_v4(DCLI_WTP_API_GROUP_TWO *WTPINFO)*/
extern int show_wtp_extension_information_v4_func(dbus_parameter parameter, DBusConnection *connection,int wtp_id,DCLI_WTP_API_GROUP_TWO **WTPINFO);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp id does not exist������-2��ʾwtp extension info report switch disable������-3ʾWTP ID�Ƿ�*/
																																							 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void free_show_ap_wids_set_cmd(DCLI_WTP_API_GROUP_THREE *WTPINFO);
/*retu==1�����ͷź��� free_show_ap_wids_set_cmd(DCLI_WTP_API_GROUP_THREE *WTPINFO)*/																				
extern int show_ap_wids_set_cmd_func(dbus_parameter parameter, DBusConnection *connection,DCLI_WTP_API_GROUP_THREE **WTPINFO);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																																	  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_ap_wids_set_cmd_func(dbus_parameter parameter, DBusConnection *connection,char *type1,char *type2,char *type3,char *state); /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter error������-2��ʾerror*/
																																	  			 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void free_show_wtp_max_power_base_model(DCLI_WTP_API_GROUP_THREE *WTPINFO);
/*retu = 1 ʱ������free_show_wtp_max_power_base_model()�ͷſռ�*/
extern int show_wtp_max_power_base_model_func(dbus_parameter parameter, DBusConnection *connection,int WtpID,DCLI_WTP_API_GROUP_THREE **WTPINFO);
																										/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																									    /*����-1��ʾwtp id does not exist*/
																									    /*����-2��ʾwtp model is wrong������-3ʾWTP ID�Ƿ�*/
																										/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_ap_reboot_func_group(struct WtpList *WtpList_Head);

/*����-6ʱ������Free_set_ap_reboot_func_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_ap_reboot_func_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,struct WtpList **WtpList_Head);
												/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp is not in run state*/
												/*����-2��ʾwtp id does not exist������-3��ʾerror������-4ʾWTP ID�Ƿ�*/
												/*����-5��ʾGroup ID�Ƿ�������-6��ʾpartial failure������-7��ʾgroup id does not exist*/
#endif

extern int set_ap_reboot_func(dbus_parameter parameter, DBusConnection *connection,int WtpID);
															 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp is not in run state*/
															 /*����-2��ʾwtp id does not exist������-3��ʾerror������-4ʾWTP ID�Ƿ�*/
															 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
															 
extern int set_ap_reboot_all_func(dbus_parameter parameter, DBusConnection *connection);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
															 								/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
															 
extern int set_ap_reboot_by_wlanid_func(dbus_parameter parameter, DBusConnection *connection,char *WLAN_ID);
																			 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																			 /*����-2��ʾwlan id should be 1 to WLAN_NUM-1*/
																			 /*����-3��ʾwlan id does not exist������-4��ʾerror*/
																			 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																			 
extern int set_ap_reboot_by_interface_func(dbus_parameter parameter, DBusConnection *connection,char *IF_Name);
																			   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																			   /*����-1��ʾthe length of input parameter is excel the limit of 16*/
																			   /*����-2��ʾinterface error������-3��ʾerror*/
																			   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																			   
extern int set_ap_reboot_by_model_func(dbus_parameter parameter, DBusConnection *connection,char *AP_Model);
																			  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																		      /*����-2��ʾmodel is not exist������-3��ʾerror*/
																			  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																			  
extern int set_ap_reboot_by_list_func(dbus_parameter parameter, DBusConnection *connection,char *ap_list);
																		 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																	     /*����-1��ʾinput wtp id is too long,you should input less than 80 letters*/
																	     /*����-2��ʾparse wtp list failed������-3��ʾerror*/
																		 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*stateΪ(join|configure|datacheck|run|quit|imagedata|bak_run)*/
/*����1ʱ������Free_wtp_list_new_head()�ͷſռ�*/
extern int show_wtp_list_by_state_func(dbus_parameter parameter, DBusConnection *connection,DCLI_WTP_API_GROUP_ONE **WTPINFO);
																		 	  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������2��ʾno wtp exist*/
																			  /*����-1��ʾinput patameter should only be join|configure|datacheck|run|quit|imagedata|bak_run*/
																			  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																										  
/*����1ʱ������Free_wtp_list_new_head()�ͷſռ�*/
extern int show_wtp_list_by_ip_func(dbus_parameter parameter, DBusConnection *connection,char *IP,char *MASK,DCLI_WTP_API_GROUP_ONE **WTPINFO);
																			  			   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������2��ʾno wtp exist*/
																						   /*����-1��ʾunknown ip format*/
																						   /*����-2��ʾunknown mask format*/
																						   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
/*����1ʱ������Free_wtp_list_new_head()�ͷſռ�*/
extern int show_wtp_list_by_macex_func(dbus_parameter parameter, DBusConnection *connection,char *MAC,char *MASK,DCLI_WTP_API_GROUP_ONE **WTPINFO);
																						   			 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������2��ʾno wtp exist*/
																								     /*����-1��ʾUnknown mac addr format*/
																								     /*����-2��ʾUnknown macmask addr format*/
																									 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																													 
extern void Free_wtp_model_list_head(DCLI_WTP_API_GROUP_ONE *WTPINFO);
/*����1ʱ������Free_wtp_model_list_head()�ͷſռ�*/
extern int show_wtp_model_list_cmd(dbus_parameter parameter, DBusConnection *connection,DCLI_WTP_API_GROUP_ONE **WTPINFO);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾno wtp exist*/
																																 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int wtp_set_ap_update_config_cmd_func(dbus_parameter parameter, DBusConnection *connection,char *path,char *count,char *model,char *version);
																											   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																											   /*����-1��ʾset update failed due to system cann't find file*/
																											   /*����-2��ʾset update failed due to file version error*/
																											   /*����-3��ʾwireless-control does not surport model MODEL*/
																											   /*����-4��ʾupdate is process,please wait several minutes*/
																											   /*����-5��ʾerror*/
																											   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void free_wtp_show_ap_update_config(DCLI_WTP_API_GROUP_THREE *WTPINFO);
/*����1ʱ������free_wtp_show_ap_update_config()�ͷſռ�*/
extern int wtp_show_ap_update_config_func(dbus_parameter parameter, DBusConnection *connection,DCLI_WTP_API_GROUP_THREE **WTPINFO);
																									  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																									  /*����-1��ʾno update config information*/
																									  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																					  
extern int wtp_clear_ap_update_config_func(dbus_parameter parameter, DBusConnection *connection);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾno update config information*/
																									   /*����-2��ʾupgrade is in process,changement of configuration is not allowed now*/
																									   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_update_wtp_list(DCLI_WTP_API_GROUP_ONE *WTPINFO);
/*����1ʱ������Free_show_update_wtp_list()�ͷſռ�*/
extern int show_update_wtp_list_func(dbus_parameter parameter, DBusConnection *connection,DCLI_WTP_API_GROUP_ONE **WTPINFO);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾupdate wtp does not exist*/
																																  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int uplink_detect_cmd_func(dbus_parameter parameter, DBusConnection *connection,char *ip,char *WlanID,char *State);
																						  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																						  /*����-1��ʾunknown ip format*/
																						  /*����-2��ʾunknown wlanid format*/
																						  /*����-3��ʾwlan id should be 1 to WLAN_NUM-1*/
						                                                                  /*����-4��ʾwlan no exist*/
																						  /*����-5��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																						  /*����-6��ʾunknown format,just be enable or disable*/
																						  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void free_show_wtp_model_code_version(DCLI_WTP_API_GROUP_ONE *WTPINFO);
/*����1ʱ������free_show_wtp_model_code_version()�ͷſռ�*/																						  
extern int show_wtp_model_code_version_cmd_func(dbus_parameter parameter, DBusConnection *connection,int WtpID,DCLI_WTP_API_GROUP_ONE **WTPINFO);
																														/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																											  			/*����-1��ʾwtp id does not exist*/
																											  			/*����-2��ʾerror������-3ʾWTP ID�Ƿ�*/
																														/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_update_wtp_img_cmd_func_group(struct WtpList *WtpList_Head);

/*����-6ʱ������Free_update_wtp_img_cmd_func_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*timeΪ"now"��"later"*/
extern int update_wtp_img_cmd_func_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *filename,char *version,char *time,struct WtpList **WtpList_Head);
																			/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																			/*����-1��ʾset update failed due to system cann't find file*/
																			/*����-2��ʾset update failed due to file version error*/
																			/*����-3��ʾerror������-4ʾWTP ID�Ƿ�*/
																			/*����-5��ʾGroup ID�Ƿ�������-6��ʾpartial failure*/
																			/*����-7��ʾgroup id does not exist*/
#endif

/*timeΪ"now"��"later"*/
extern int update_wtp_img_cmd_func(dbus_parameter parameter, DBusConnection *connection,int WtpID,char *filename,char *version,char *time);
																											 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																											 /*����-1��ʾset update failed due to system cann't find file*/
																											 /*����-2��ʾset update failed due to file version error*/
																											 /*����-3��ʾerror������-4ʾWTP ID�Ƿ�*/
																											 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_clear_wtp_img_cmd_func_group(struct WtpList *WtpList_Head);

/*����-4ʱ������Free_clear_wtp_img_cmd_func_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int clear_wtp_img_cmd_func_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,struct WtpList **WtpList_Head);
							 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror������-2��ʾWTP ID�Ƿ�*/
							 /*����-3��ʾGroup ID�Ƿ�������-4��ʾpartial failure������-5��ʾgroup id does not exist*/
#endif

extern int clear_wtp_img_cmd_func(dbus_parameter parameter, DBusConnection *connection,int WtpID);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror������-2��ʾWTP ID�Ƿ�*/
																										/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_ap_networkaddr_command_cmd_group(struct WtpList *WtpList_Head);

/*����-9ʱ������Free_set_ap_networkaddr_command_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_ap_networkaddr_command_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *ap_ip,char *ap_mask,char *ap_gateway,char *ap_dns1,char *ap_dns2,struct WtpList **WtpList_Head);
														/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown ip format*/
														/*����-2��ʾunknown mask format������-3��ʾunknown gateway format*/
														/*����-4��ʾunknown dns format������-5��ʾwtp id does not exist*/
														/*����-6��ʾerror������-7ʾWTP ID�Ƿ�������-8��ʾGroup ID�Ƿ�*/
														/*����-9��ʾpartial failure������-10��ʾgroup id does not exist*/
#endif

extern int set_ap_networkaddr_command_cmd(dbus_parameter parameter, DBusConnection *connection,int WtpID,char *ap_ip,char *ap_mask,char *ap_gateway,char *ap_dns1,char *ap_dns2);
														/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown ip format*/
														/*����-2��ʾunknown mask format������-3��ʾunknown gateway format*/
														/*����-4��ʾunknown dns format������-5��ʾwtp id does not exist*/
														/*����-6��ʾerror������-7ʾWTP ID�Ƿ�*/
														/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void free_show_old_ap_img_data(DCLI_WTP_API_GROUP_THREE *WTPINFO);
/*retu = 1 �����ͷź���free_show_old_ap_img_data(DCLI_WTP_API_GROUP_THREE *WTPINFO)*/														
extern int show_old_ap_img_data_cmd(dbus_parameter parameter, DBusConnection *connection,DCLI_WTP_API_GROUP_THREE **WTPINFO );/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																																	  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
														
extern int old_ap_img_data_cmd(dbus_parameter parameter, DBusConnection *connection,char *state);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/														
																									   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_update_img_file_name(dbus_parameter parameter, DBusConnection *connection,char *fpath,char *fversion,char *ftime,char *plist);
																							/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾset update failed due to system cann't find file*/
																							/*����-2��ʾset update failed due to file version error������-3��ʾset wtp list error������-4��ʾerror*/
																							/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																											
extern int clear_ap_img_info(dbus_parameter parameter, DBusConnection *connection,char *iflist);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾset wtp list error������-2��ʾerror*/
																								   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																											
extern void free_show_ac_access_wtp_info(DCLI_WTP_API_GROUP_ONE *WTPINFO);
/*����1ʱ������free_show_ac_access_wtp_info()�ͷſռ�*/
extern int show_ac_access_wtp_info_cmd(dbus_parameter parameter, DBusConnection *connection,DCLI_WTP_API_GROUP_ONE **WTPINFO);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																																	   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void free_show_update_fail_wtp_list(DCLI_WTP_API_GROUP_ONE *WTPINFO);
/*����1ʱ������free_show_update_fail_wtp_list()�ͷſռ�*/
extern int show_update_fail_wtp_list(dbus_parameter parameter, DBusConnection *connection,DCLI_WTP_API_GROUP_ONE **WTPINFO);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																																/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int clear_update_fail_wtp(dbus_parameter parameter, DBusConnection *connection,char *iflist);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾset wtp list error������-2��ʾerror*/
																									   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_wtp_sn_group(struct WtpList *WtpList_Head);

/*����-6ʱ������Free_set_wtp_sn_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_wtp_sn_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *wtpsn,struct WtpList **WtpList_Head);
								/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp sn is too long,should be 1 to 127*/
								/*����-2��ʾwtp id does not exist������-3��ʾwtp be running������-4ʾWTP ID�Ƿ�*/
								/*����-5��ʾGroup ID�Ƿ�������-6��ʾpartial failure������-7��ʾgroup id does not exist*/
#endif

extern int set_wtp_sn(dbus_parameter parameter, DBusConnection *connection,int id,char *wtpsn);
 															/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp sn is too long,should be 1 to 127*/
															/*����-2��ʾwtp id does not exist������-3��ʾwtp be running������-4ʾWTP ID�Ƿ�*/
															/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
															
extern int clean_acaccess_wtpinfo_list(dbus_parameter parameter, DBusConnection *connection);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																								 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*stateΪ"enable"��"disable"*/
/*wtp_listΪ"all"��like 1,8,9-20,33*/
extern int set_wtp_list_dhcp_snooping_enable_cmd(dbus_parameter parameter, DBusConnection *connection,char *state,char *wtp_list);
																								 	  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																									  /*����-1��ʾinput patameter only with 'enable' or 'disable'*/
																									  /*����-2��ʾset wtp list error,like 1,8,9-20,33*/
																									  /*����-3��ʾerror*/
																									  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_wtp_dhcp_snooping_enable_cmd_group(struct WtpList *WtpList_Head);

/*����-5ʱ������Free_set_wtp_dhcp_snooping_enable_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*stateΪ"enable"��"disable"*/
extern int set_wtp_dhcp_snooping_enable_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *state,struct WtpList **WtpList_Head);
																  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																  /*����-1��ʾinput patameter only with 'enable' or 'disable'*/
																  /*����-2��ʾerror������-3ʾWTP ID�Ƿ�*/
																  /*����-4��ʾGroup ID�Ƿ�������-5��ʾpartial failure*/
																  /*����-6��ʾgroup id does not exist*/
#endif

/*stateΪ"enable"��"disable"*/
extern int set_wtp_dhcp_snooping_enable_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *state);
																						 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																						 /*����-1��ʾinput patameter only with 'enable' or 'disable'*/
																						 /*����-2��ʾerror������-3ʾWTP ID�Ƿ�*/
																						 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*stateΪ"enable"��"disable"*/
/*wtp_listΪ"all"��like 1,8,9-20,33*/
extern int set_wtp_list_sta_info_report_enable_cmd(dbus_parameter parameter, DBusConnection *connection,char *state,char *wtp_list);
																						 			 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																								     /*����-1��ʾinput patameter only with 'enable' or 'disable'*/
																								     /*����-2��ʾset wtp list error,like 1,8,9-20,33*/
																								     /*����-3��ʾerror*/
																									 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_wtp_sta_info_report_enable_cmd_group(struct WtpList *WtpList_Head);

/*����-5ʱ������Free_set_wtp_sta_info_report_enable_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*stateΪ"enable"��"disable"*/
extern int set_wtp_sta_info_report_enable_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *state,struct WtpList **WtpList_Head);
																 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																 /*����-1��ʾinput patameter only with 'enable' or 'disable'*/
																 /*����-2��ʾerror������-3ʾWTP ID�Ƿ�*/
																 /*����-4��ʾGroup ID�Ƿ�������-5��ʾpartial failure*/
																 /*����-6��ʾgroup id does not exist*/
#endif

/*stateΪ"enable"��"disable"*/
extern int set_wtp_sta_info_report_enable_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *state);
																						 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																					     /*����-1��ʾinput patameter only with 'enable' or 'disable'*/
																					     /*����-2��ʾerror������-3ʾWTP ID�Ƿ�*/
																						 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*WtpIDΪ0��ʾȫ������*/
/*trap_typeΪ"rogueap","rogueterminal","cpu"��"memory"*/
/*cpu use threshold��Χ��0-100*/
/*ap memory use threshold��Χ��0-100*/
/*rogueap and rogueterminal ��Χ��0-50000*/																						 
extern int set_wtp_trap_threshold_cmd(dbus_parameter parameter, DBusConnection *connection,int WtpID,char *trap_type,char *VALUE);
																						 		    /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																									/*����-1��ʾinput patameter only with 'rogueap','rogueterminal','cpu' or 'memory'*/
																									/*����-2��ʾunknown id format������-3��ʾap cpu use threshold parameters error,should be 0-100*/
																									/*����-4��ʾap memory use threshold parameters error,should be 0-100*/
																									/*����-5��ʾwtp id does not run������-6��ʾerror������-7ʾWTP ID�Ƿ�*/
																						   			/*����-8��ʾparameters error, threshold should be less than 50000*/
																									/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_ap_trap_threshold(WID_TRAP_THRESHOLD *INFIO);
/*WtpIDΪ0��ʾȫ������*/																									
/*ֻҪ���ú������͵���Free_show_ap_trap_threshold()�ͷſռ�*/
extern int show_ap_trap_rogue_ap_ter_cpu_mem_threshold_cmd(dbus_parameter parameter, DBusConnection *connection, int WtpID,WID_TRAP_THRESHOLD **INFIO);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror������-2��ʾWTP ID�Ƿ�*/
																																									 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_wtp_trap_switch_cmd_group(struct WtpList *WtpList_Head);

/*����-5ʱ������Free_set_wtp_trap_switch_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*stateΪ"enable"��"disable"*/
extern int set_wtp_trap_switch_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *state,struct WtpList **WtpList_Head);
																		/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																	    /*����-1��ʾinput patameter only with 'enable'or'disable'*/
																		/*����-2��ʾerror������-3ʾWTP ID�Ƿ�*/
																		/*����-4��ʾGroup ID�Ƿ�������-5��ʾpartial failure*/
																		/*����-6��ʾgroup id does not exist*/
#endif

/*stateΪ"enable"��"disable"*/
extern int set_wtp_trap_switch_cmd(dbus_parameter parameter, DBusConnection *connection,int WtpID,char *state);
																				/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																			    /*����-1��ʾinput patameter only with 'enable'or'disable'*/
																				/*����-2��ʾerror������-3ʾWTP ID�Ƿ�*/
																				/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_ap_sta_wapi_info_report_enable_cmd_group(struct WtpList *WtpList_Head);

/*����-7ʱ������Free_set_ap_sta_wapi_info_report_enable_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*stateΪ"enable"��"disable"*/
extern int set_ap_sta_wapi_info_report_enable_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *state,struct WtpList **WtpList_Head);
						/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'enable'or'disable'*/
						/*����-2��ʾwtp id does not exist������-3��ʾwtp id does not run������-4��ʾerror������-5ʾWTP ID�Ƿ�*/
						/*����-6��ʾGroup ID�Ƿ�������-7��ʾpartial failure������-8��ʾgroup id does not exist*/
#endif

/*stateΪ"enable"��"disable"*/
extern int set_ap_sta_wapi_info_report_enable_cmd(dbus_parameter parameter, DBusConnection *connection,int WtpID,char *state);
																								 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'enable'or'disable'*/
																								 /*����-2��ʾwtp id does not exist������-3��ʾwtp id does not run������-4��ʾerror������-5ʾWTP ID�Ƿ�*/
																								 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_ap_sta_wapi_info_reportinterval_cmd_group(struct WtpList *WtpList_Head);

/*����-9ʱ������Free_set_ap_sta_wapi_info_reportinterval_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*para�ķ�Χ��1-120*/
extern int set_ap_sta_wapi_info_reportinterval_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *para,struct WtpList **WtpList_Head);
										 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
										 /*����-2��ʾap sta infomation reportinterval error������-3��ʾwtp is not in run state*/
										 /*����-4��ʾwtp id does not exist������-5��ʾerror������-6ʾWTP ID�Ƿ�*/
										 /*����-7��ʾillegal input:Input exceeds the maximum value of the parameter type*/
										 /*����-8��ʾGroup ID�Ƿ�������-9��ʾpartial failure������-10��ʾgroup id does not exist*/
#endif

/*para�ķ�Χ��1-32767*/
extern int set_ap_sta_wapi_info_reportinterval_cmd(dbus_parameter parameter, DBusConnection *connection,int WtpID,char *para);
																								/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																								/*����-2��ʾap sta infomation reportinterval error,should be 1-32767������-3��ʾwtp is not in run state*/
																								/*����-4��ʾwtp id does not exist������-5��ʾerror������-6ʾWTP ID�Ƿ�*/
																								/*����-7��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																								/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_all_wtp_the_radio_para_information_cmd(struct WtpRadioParaInfo *WlanHead);
/*ֻҪ���ã���ͨ��Free_show_all_wtp_the_radio_para_information_cmd()�ͷſռ�*/
extern int show_all_wtp_the_radio_para_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct WtpRadioParaInfo **WtpHead);
																												   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																												   /*����-1��ʾThere is no WTP now*/
																												   /*����-2��ʾerror*/
																												   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_all_wtp_radio_stats_information_cmd(struct RadioStatsInfo *WtpHead);
/*ֻҪ���ã���ͨ��Free_show_all_wtp_radio_stats_information_cmd()�ͷſռ�*/
extern int show_all_wtp_radio_stats_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct RadioStatsInfo **WtpHead);
																											 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																											 /*����-1��ʾThere is no WTP now*/
																											 /*����-2��ʾerror*/
																											 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																												   
extern void Free_show_all_wtp_rogue_ap_info_information_cmd(struct RogueAPInfo *WtpHead);
/*ֻҪ���ã���ͨ��Free_show_all_wtp_rogue_ap_info_information_cmd()�ͷſռ�*/
extern int show_all_wtp_rogue_ap_info_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct RogueAPInfo **WtpHead);
																											  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																											  /*����-1��ʾThere is no WTP now*/
																											  /*����-2��ʾap scanning disable ,please enable it*/
																											  /*����-3��ʾno ap has neighbor ap*/
																											  /*����-4��ʾerror*/
																											  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																											 
extern void Free_show_all_wtp_user_link_information_cmd(struct UsrLinkInfo *WtpHead);
/*ֻҪ���ã���ͨ��Free_show_all_wtp_user_link_information_cmd()�ͷſռ�*/
extern int show_all_wtp_user_link_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct UsrLinkInfo **WtpHead);
																										/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																									    /*����-1��ʾThere is no WTP now*/
																									    /*����-2��ʾerror*/
																										/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																											  
extern void Free_show_all_wtp_data_pkts_information_cmd(struct WtpDataPktsInfo *WtpHead);
/*ֻҪ���ã���ͨ��Free_show_all_wtp_data_pkts_information_cmd()�ͷſռ�*/
extern int show_all_wtp_data_pkts_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct WtpDataPktsInfo **WtpHead);
																											/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																											/*����-1��ʾThere is no WTP now*/
																											/*����-2��ʾerror*/
																											/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																										
extern void Free_show_all_wtp_device_information_cmd(struct WtpDeviceInfo *WtpHead);
/*ֻҪ���ã���ͨ��Free_show_all_wtp_device_information_cmd()�ͷſռ�*/
extern int show_all_wtp_device_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct WtpDeviceInfo **WtpHead);
																									   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																									   /*����-1��ʾThere is no WTP now*/
																									   /*����-2��ʾerror*/
																									   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_all_wtp_terminal_information_cmd(struct WtpTerminalInfo *WtpHead);
/*����1ʱ������Free_show_all_wtp_terminal_information_cmd()�ͷſռ�*/
extern int show_all_wtp_terminal_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct WtpTerminalInfo **WtpHead);
																										   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																										   /*����-1��ʾThere is no WTP now*/
																										   /*����-2��ʾerror*/
																										   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																									   
extern void Free_show_all_wtp_eth_port_information_cmd(struct WtpEthPortInfo *WtpHead);																										   
/*ֻҪ���ã���ͨ��Free_show_all_wtp_eth_port_information_cmd()�ͷſռ�*/
extern int show_all_wtp_eth_port_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct WtpEthPortInfo **WtpHead);
																										  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																										  /*����-1��ʾThere is no WTP now*/
																										  /*����-2��ʾerror*/
																										  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																										   
extern void Free_show_all_wtp_ifname_information_cmd(struct WtpIfnameInfo *WtpHead);																										  
/*ֻҪ���ã���ͨ��Free_show_all_wtp_ifname_information_cmd()�ͷſռ�*/
extern int show_all_wtp_ifname_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct WtpIfnameInfo **WtpHead);
																									   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																									   /*����-1��ʾThere is no WTP now*/
																									   /*����-2��ʾerror*/
																									   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_all_wtp_information_cmd(struct WtpInfor *WtpHead);
/*ֻҪ���ã���ͨ��Free_show_all_wtp_information_cmd()�ͷſռ�*/
extern int show_all_wtp_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct WtpInfor **WtpHead);
																						 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																						 /*����-1��ʾThere is no WTP now*/
																						 /*����-2��ʾerror*/
																						 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																									   
extern void Free_show_all_wtp_para_information_cmd(struct WtpParaInfo *WtpHead);
/*ֻҪ���ã���ͨ��Free_show_all_wtp_para_information_cmd()�ͷſռ�*/
extern int show_all_wtp_para_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct WtpParaInfo **WtpHead); 
																									/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																									/*����-1��ʾThere is no WTP now*/
																									/*����-2��ʾerror*/
																									/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_all_wtp_station_information_cmd(struct WtpStaInfo *StaHead);
/*����1ʱ������Free_show_all_wtp_station_information_cmd()�ͷſռ�*/
extern int show_all_wtp_station_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct WtpStaInfo **StaHead);
																									/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																								    /*����-1��ʾThere is no sta now*/
																								    /*����-2��ʾThere is no WTP now������-3��ʾerror*/
																									/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_all_wtp_stats_information_cmd(struct WtpStatsInfo *WtpHead);																									
/*ֻҪ���ã���ͨ��Free_show_all_wtp_stats_information_cmd()�ͷſռ�*/
extern int show_all_wtp_stats_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct WtpStatsInfo **WtpHead);
																									/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																									/*����-1��ʾThere is no WTP now*/
																									/*����-2��ʾerror������-3��ʾWTPID is larger than MAX*/
																									/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_all_wtp_wired_ifstats_information_cmd(struct WtpWiredIfStatsInfo *WtpHead);
/*ֻҪ���ã���ͨ��Free_show_all_wtp_wired_ifstats_information_cmd()�ͷſռ�*/
extern int show_all_wtp_wired_ifstats_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct WtpWiredIfStatsInfo **WtpHead);
																													/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																													/*����-1��ʾThere is no WTP now*/
																													/*����-2��ʾerror*/
																													/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																									
extern void Free_show_all_wtp_wireless_if_information_cmd(struct WtpWirelessIfInfo *WtpHead);
/*ֻҪ���ã���ͨ��Free_show_all_wtp_wireless_if_information_cmd()�ͷſռ�*/
extern int show_all_wtp_wireless_if_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct WtpWirelessIfInfo **WtpHead);
																												/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																												/*����-1��ʾThere is no WTP now*/
																												/*����-2��ʾerror*/
																												/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																													
extern void Free_show_all_wtp_wirelessifstats_information_cmd(struct WtpWirelessIfstatsInfo *WtpHead);
/*ֻҪ���ã���ͨ��Free_show_all_wtp_wirelessifstats_information_cmd()�ͷſռ�*/
extern int show_all_wtp_wirelessifstats_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct WtpWirelessIfstatsInfo **WtpHead);
																														/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																														/*����-1��ʾThere is no WTP now*/
																														/*����-2��ʾerror*/
																														/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_all_wtp_wlan_data_pkts_information_cmd(struct WtpWlanDataPktsInfo *WtpHead);
/*����1ʱ������Free_show_all_wtp_wlan_data_pkts_information_cmd()�ͷſռ�*/
extern int show_all_wtp_wlan_data_pkts_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct WtpWlanDataPktsInfo **WtpHead);
																										/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																										/*����-1��ʾThere is no WTP now*/
																										/*����-2��ʾerror*/
																										/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																														
extern void Free_show_all_wlan_stats_information_cmd(struct WtpWlanStatsInfo *WtpHead);
/*ֻҪ���ã���ͨ��Free_show_all_wlan_stats_information_cmd()�ͷſռ�*/
extern int show_all_wlan_stats_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct WtpWlanStatsInfo **WtpHead);
																										 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																										 /*����-1��ʾThere is no WTP now*/
																										 /*����-2��ʾerror*/
																										 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_all_wtp_radio_config_information_cmd(struct WtpConfigRadioInfo *WtpHead);
/*ֻҪ���ã���ͨ��Free_show_all_wtp_radio_config_information_cmd()�ͷſռ�*/
extern int show_all_wtp_radio_config_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct WtpConfigRadioInfo **WtpHead);
																												  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																												  /*����-1��ʾThere is no WTP now*/
																												  /*����-2��ʾerror*/
																												  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_all_wtp_basic_information_cmd(struct WtpBasicInfo *WtpHead);
/*ֻҪ���ã���ͨ��Free_show_all_wtp_basic_information_cmd()�ͷſռ�*/
extern int show_all_wtp_basic_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct WtpBasicInfo **WtpHead);
																									 /*����0��ʾdbus error������1��ʾ�ɹ�*/
																									 /*����-1��ʾThere is no WTP now*/
																									 /*����-2��ʾerror*/
																									 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_all_wtp_new_wtp_wireless_ifinfo_information_cmd(struct NewWtpWirelessIfInfo *RadioHead);
/*ֻҪ���ã���ͨ��Free_show_all_wtp_new_wtp_wireless_ifinfo_information_cmd()�ͷſռ�*/
extern int show_all_wtp_new_wtp_wireless_ifinfo_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct NewWtpWirelessIfInfo **RadioHead);
																							/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																							/*����-1��ʾThere is no WTP now*/
																							/*����-2��ʾerror*/
																							/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_all_wlan_ssid_stats_information_cmd(struct SSIDStatsInfo *WtpHead);
/*ֻҪ���ã���ͨ��Free_show_all_wlan_ssid_stats_information_cmd()�ͷſռ�*/
extern int show_all_wlan_ssid_stats_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct SSIDStatsInfo **WtpHead);
																											/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																											/*����-1��ʾThere is no WTP now*/
																											/*����-2��ʾerror*/
																											/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_all_wlan_ssid_stats_information_cmd_v2(struct SSIDStatsInfo_v2 *WtpHead_v2);
/*����1ʱ������Free_show_all_wlan_ssid_stats_information_cmd_v2()�ͷſռ�*/
extern int show_all_wlan_ssid_stats_information_cmd_v2(dbus_parameter parameter, DBusConnection *connection,struct SSIDStatsInfo_v2 **WtpHead_v2);
																													  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																													  /*����-1��ʾThere is no WTP now*/
																													  /*����-2��ʾerror*/
																													  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_all_wtp_collect_information_cmd(struct WtpCollectInfo *WtpHead);
/*ֻҪ���ã���ͨ��Free_show_all_wtp_collect_information_cmd()�ͷſռ�*/
extern int show_all_wtp_collect_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct WtpCollectInfo **WtpHead);
																										/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																										/*����-1��ʾThere is no WTP now*/
																										/*����-2��ʾerror*/
																										/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_ath_statistics_info_of_all_wtp_cmd(struct WtpAthStatisticInfo *WtpHead);
/*����1ʱ������Free_show_ath_statistics_info_of_all_wtp_cmd()�ͷſռ�*/
extern int show_ath_statistics_info_of_all_wtp_cmd(dbus_parameter parameter, DBusConnection *connection,struct WtpAthStatisticInfo **WtpHead);
																											   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																											   /*����-1��ʾthere is no ath interface*/
																											   /*����-2��ʾWID can't find wtp*/
																											   /*����-3��ʾASD can't find bss*/
																											   /*����-4��ʾerror*/
																											   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*stateΪ"enable"��"disable"*/
extern int set_ap_moment_infomation_enable_cmd(dbus_parameter parameter, DBusConnection *connection,int WtpID,char *state);
																							   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																							   /*����-1��ʾinput patameter only with 'enable' or 'disable'*/
																							   /*����-2��ʾWTP ID�Ƿ�������-3��ʾwtp id does not exist*/
																							   /*����-4��ʾwtp id does not run������-5��ʾThe switch is already state*/
																							   /*����-6��ʾerror*/
																							   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*value�ķ�Χ��2-32767*/
extern int set_ap_moment_infomation_reportinterval_cmd(dbus_parameter parameter, DBusConnection *connection,char *value);
																							 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																							 /*����-1��ʾap moment infomation reportinterval error*/
																							 /*����-2��ʾThe value is no need to change������-3��ʾerror*/
																							 /*����-4��ʾunknown id format*/
																							 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*value�ķ�Χ��2-32767*/
extern int set_ap_sample_infomation_reportinterval_cmd(dbus_parameter parameter, DBusConnection *connection,char *value); 
																							 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																							 /*����-1��ʾap sample infomation reportinterval error*/
																							 /*����-2��ʾThe value is no need to change������-3��ʾerror*/
																							 /*����-4��ʾunknown id format*/
																							 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*value�ķ�Χ��10-32767*/
extern int set_ap_routine_infomation_reportinterval_cmd(dbus_parameter parameter, DBusConnection *connection,char *value); 
																							 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																							 /*����-1��ʾap routine infomation reportinterval error*/
																							 /*����-2��ʾThe value is no need to change������-3��ʾerror*/
																							 /*����-4��ʾunknown id format*/
																							 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int show_ap_moment_information_reportinterval_cmd(dbus_parameter parameter, DBusConnection *connection,struct ap_reportinterval_profile *reportinterval_info);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																							 																					/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_trap_ignore_wtp_cmd(dbus_parameter parameter, DBusConnection *connection,int WtpID,char *percent);
																				 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																				 /*����-1��ʾWTP ID�Ƿ�������-2��ʾinput patameter error*/
																				 /*����-3��ʾinput patameter must be number*/
																				 /*����-4��ʾinput patameter must between 0 and 100������-5��ʾerror*/
																				 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int show_hide_quit_wtp_cmd(dbus_parameter parameter, DBusConnection *connection,int *value);
																	 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																	 /*����-1��ʾthe value of hide_quit_wtp_in is invalid*/
																	 /*����-2��ʾerror*/
																	 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*typeΪ"neighborchannelrssi"��"samechannelrssi"*/
/*Value�ķ�Χ��-120��0*/
extern int set_wtp_trap_neighbor_same_channelrssithreshold_cmd(dbus_parameter parameter, DBusConnection *connection, int WtpID,char *type,char *Value); 
																	 										  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																											  /*����-1��ʾWTP ID�Ƿ�*/
																											  /*����-2��ʾinput patameter only with 'neighborchannelrssi' or 'samechannelrssi'*/
																											  /*����-3��ʾunknown id format������-4��ʾinput patameter should be -120 to 0*/
																											  /*����-5��ʾwtp id does not run������-6��ʾerror*/
																											  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*config�ڵ���WtpIDΪ0*/
/*value�ķ�Χ��5-900*/
extern int set_ap_cpu_collect_time_cmd(dbus_parameter parameter, DBusConnection *connection,int WtpID,char *Value);
																					/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																				    /*����-1��ʾWTP ID�Ƿ�������-2��ʾerror*/
																					/*����-3��ʾunknown id format*/
																				    /*����-4��ʾinput collect time should be 5~900*/
																					/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_ap_terminal_distrub_infomation_switch_cmd_group(struct WtpList *WtpList_Head);

/*����-7ʱ������Free_set_ap_terminal_distrub_infomation_switch_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*stateΪ"enable"��"disable"*/
extern int set_ap_terminal_distrub_infomation_switch_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *state,struct WtpList **WtpList_Head);
							/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'enable' or 'disable'*/
							/*����-2��ʾWTP ID�Ƿ�������-3��ʾwtp id does not exist������-4��ʾwtp id does not run*/
							/*����-5��ʾerror������-6��ʾGroup ID�Ƿ�������-7��ʾpartial failure*/
							/*����-8��ʾgroup id does not exist*/
#endif

/*stateΪ"enable"��"disable"*/
extern int set_ap_terminal_distrub_infomation_switch_cmd(dbus_parameter parameter, DBusConnection *connection,int WtpID,char *state);
																										/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'enable' or 'disable'*/
																										/*����-2��ʾWTP ID�Ƿ�������-3��ʾwtp id does not exist������-4��ʾwtp id does not run*/
																										/*����-5��ʾerror*/
																										/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_ap_terminal_distrub_infomation_pkt_cmd_group(struct WtpList *WtpList_Head);

/*����-7ʱ������Free_set_ap_terminal_distrub_infomation_pkt_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*value�ķ�Χ��0-50000*/
extern int set_ap_terminal_distrub_infomation_pkt_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *value,struct WtpList **WtpList_Head);
							/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾap terminal distrub infomation reportpkt error*/
							/*����-2��ʾWTP ID�Ƿ�������-3��ʾwtp id does not exist������-4��ʾwtp id does not run*/
							/*����-5��ʾerror������-6��ʾGroup ID�Ƿ�������-7��ʾpartial failure*/
							/*����-8��ʾgroup id does not exist������-9��ʾunknown id format*/
#endif

/*value�ķ�Χ��0-50000*/
extern int set_ap_terminal_distrub_infomation_pkt_cmd(dbus_parameter parameter, DBusConnection *connection,int WtpID,char *value);
																									/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾap terminal distrub infomation reportpkt error*/
																									/*����-2��ʾWTP ID�Ƿ�������-3��ʾwtp id does not exist������-4��ʾwtp id does not run*/
																									/*����-5��ʾerror������-9��ʾunknown id format*/
																									/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_ap_terminal_distrub_infomation_sta_num_cmd_group(struct WtpList *WtpList_Head);

/*����-7ʱ������Free_set_ap_terminal_distrub_infomation_sta_num_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*value�ķ�Χ��1-50000*/
extern int set_ap_terminal_distrub_infomation_sta_num_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *value,struct WtpList **WtpList_Head);
							/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾap terminal distrub infomation report sta_num error*/
							/*����-2��ʾWTP ID�Ƿ�������-3��ʾwtp id does not exist������-4��ʾwtp id does not run*/
							/*����-5��ʾerror������-6��ʾGroup ID�Ƿ�������-7��ʾpartial failure*/
							/*����-8��ʾgroup id does not exist������-9��ʾunknown id format*/
#endif

/*value�ķ�Χ��1-50000*/
extern int set_ap_terminal_distrub_infomation_sta_num_cmd(dbus_parameter parameter, DBusConnection *connection,int WtpID,char *value); 
																											/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾap terminal distrub infomation report sta_num error*/
																											/*����-2��ʾWTP ID�Ƿ�������-3��ʾwtp id does not exist������-4��ʾwtp id does not run*/
																											/*����-5��ʾerror*/
																											/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_wlan_radio_information_cmd(struct WtpWlanRadioInfo *WtpHead);
/*����1ʱ������Free_show_wlan_radio_information_cmd()�ͷſռ�*/
extern int show_wlan_radio_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct WtpWlanRadioInfo **WtpHead);
																									  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																									  /*����-1��ʾthere is no wlan������-2��ʾMalloc Error*/
																									  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_all_wtp_network_info_cmd(struct WtpNetworkInfo *WtpHead);
/*ֻҪ���ã���ͨ��Free_show_all_wtp_network_info_cmd()�ͷſռ�*/
extern int show_all_wtp_network_info_cmd(dbus_parameter parameter, DBusConnection *connection,struct WtpNetworkInfo **WtpHead);
																								 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																							     /*����-1��ʾerror*/
																								 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_all_wtp_security_mech_information_cmd(struct SecurityMechInfo *WtpHead);
/*ֻҪ���ã���ͨ��Free_show_all_wtp_security_mech_information_cmd()�ͷſռ�*/
extern int show_all_wtp_security_mech_information_cmd(dbus_parameter parameter, DBusConnection *connection,struct SecurityMechInfo **WtpHead); 
																													/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																													/*����-1��ʾThere is no WTP now������-2��ʾerror*/
																													/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_wtp_config_of_all_cmd(struct WTP_CONFIG_INFORMATION *WTPconfig);
/*ֻҪ���ã���ͨ��Free_show_all_wtp_security_mech_information_cmd()�ͷſռ�*/
extern int show_wtp_config_of_all_cmd(dbus_parameter parameter, DBusConnection *connection,struct WTP_CONFIG_INFORMATION **WTPconfig);
																										/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																										/*����-1��ʾerror*/
																										/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_wtp_list_byinterface_cmd(DCLI_WTP_API_GROUP_ONE *WTPINFO);
/*����1ʱ������Free_show_all_wtp_security_mech_information_cmd()�ͷſռ�*/
extern int show_wtp_list_byinterface_cmd(dbus_parameter parameter, DBusConnection *connection,DCLI_WTP_API_GROUP_ONE **WTPINFO);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																																	  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_count_ap_num_by_interface_head(AP_NUM_INF *head);
/*����1ʱ������Free_count_ap_num_by_interface_head()�ͷſռ�*/
extern int count_ap_num_by_interface(dbus_parameter parameter, DBusConnection *connection,AP_NUM_INF *ANIF_head);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/

extern void Free_show_statistcs_information_of_all_wtp_whole_cmd(struct WtpAthStatisticInfo *WtpHead);

/*����1ʱ������Free_show_statistcs_information_of_all_wtp_whole_cmd()�ͷſռ�*/
extern int show_statistcs_information_of_all_wtp_whole_cmd(dbus_parameter parameter, DBusConnection *connection,struct WtpAthStatisticInfo **WtpHead);
																	/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																	/*����-1��ʾthere is no ath interface*/
																	/*����-2��ʾWID can't find wtp*/
																	/*����-3��ʾerror*/
																	/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_all_wtp_station_statistic_information_cmd(struct WtpStationinfo *StaHead);
/*����1ʱ������Free_show_statistcs_information_of_all_wtp_whole_cmd()�ͷſռ�*/
extern int show_statistcs_information_of_all_wtp_whole_cmd(dbus_parameter parameter, DBusConnection *connection,struct WtpAthStatisticInfo **WtpHead);
																	/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																	/*����-1��ʾthere is no ath interface*/
																	/*����-2��ʾWID can't find wtp*/
																	/*����-3��ʾerror*/
																	/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_ac_sta_information_cmd(struct WtpStationinfo *StaNode);
/*����1ʱ������Free_show_all_wtp_station_statistic_information_cmd()�ͷſռ�*/
extern int show_ac_sta_information_cmd(dbus_parameter parameter, DBusConnection *connection, struct WtpStationinfo **StaNode);
																			/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																			/*����-1��ʾThere is no sta now*/
																			/*����-2��ʾThere is no BSS now������-3��ʾerror*/
																			/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*tar_file��ʾ�汾ѹ���ļ����ƣ�ѹ����ʽ��.tar.bz2*/
extern int bind_ap_model_with_file_config(dbus_parameter parameter, DBusConnection *connection,char *model,char *tar_file);
																								/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																								/*����-1��ʾbuf malloc failed*/
																								/*����-2��ʾbind failed due to system cann't find file*/
																								/*����-3��ʾwireless-control does not surport model*/
																								/*����-4��ʾupdate is process,please wait several minutes*/
																								/*����-5��ʾfree memory is not enough*/
																								/*����-6��ʾthis model has been bound ever,please delete the bind relationship first*/
																								/*����-7��ʾerror*/
																								/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*Count_onetime��ʾͬʱ������AP��������Χ��1-50*/
extern int wtp_set_ap_update_base_model_config(dbus_parameter parameter, DBusConnection *connection,char *model);
																								/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																								/*����-1��ʾmalloc failed*/
																								/*����-2��ʾwireless-control does not surport model*/
																								/*����-3��ʾupdate is process,please wait several minutes*/
																								/*����-4��ʾmodel has been set ever, there's no need to set again*/
																								/*����-5��ʾerror*/
																								/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_model_tar_file_bind_info(struct model_tar_file *head);
/*����1��model_tar_file_num>0ʱ������Free_show_model_tar_file_bind_info()�ͷſռ�*/
extern int show_model_tar_file_bind_info(dbus_parameter parameter, DBusConnection *connection,struct model_tar_file *head,int *model_tar_file_num);
																/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int delete_model_bind_info_config(dbus_parameter parameter, DBusConnection *connection,char *model);
																	/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																	/*����-1��ʾbuf malloc failed*/
																	/*����-2��ʾwireless-control does not surport model*/
																	/*����-3��ʾupdate is process,please wait several minutes*/
																	/*����-4��ʾthis model hasn't been bound ever,please make sure that first*/
																	/*����-5��ʾerror*/
																	/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int wtp_clear_ap_one_model_update_config(dbus_parameter parameter, DBusConnection *connection,char *model);
																					  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																					  /*����-1��ʾbuf malloc failed*/
																					  /*����-2��ʾupgrade is in process,changement of configuration is not allowed now*/
																					  /*����-3��ʾno update config information of model*/
																					  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*Count_onetime��ʾͬʱ������AP��������Χ��1-50*/
extern int wtp_set_ap_update_count_config(dbus_parameter parameter, DBusConnection *connection,char *Count_onetime);
																					  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
 																					  /*����-1��ʾunknown id format*/
 																					  /*����-2��ʾupgrade has already started,it's not allowed to set the value unless upgrade is stopped*/
 																					  /*����-3��ʾerror*/
																					  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*state�ķ�Χ��"start"��"stop"*/
extern int wtp_set_ap_update_control_config(dbus_parameter parameter, DBusConnection *connection,char *state);
																			   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																			   /*����-1��ʾinput parameter can only be 'start' or 'stop'*/
																			   /*����-2��ʾthere's no upgrade configuration,it should be configured first*/
																			   /*����-3��ʾerror*/
																			   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/



/*Radio*/										  
extern void Radio_Type(int RType,char *r);

extern void  Free_radio_head(DCLI_RADIO_API_GROUP_ONE *RADIO);
/*����1ʱ������Free_radio_head()�ͷſռ�*/
extern int show_radio_list(dbus_parameter parameter, DBusConnection *connection,DCLI_RADIO_API_GROUP_ONE **RADIO);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																													/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_radio_one_head(DCLI_RADIO_API_GROUP_ONE *RADIO);
/*����1ʱ������Free_radio_one_head()�ͷſռ�*/
extern int show_radio_one(dbus_parameter parameter, DBusConnection *connection,int radio_id,DCLI_RADIO_API_GROUP_ONE **RADIO);
																							 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																							 /*����-1��ʾradio id does not exist*/
																							 /*����-2��ʾradio id should be 1 to G_RADIO_NUM*/ 
																							 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY		
extern void Free_config_radio_channel_group(struct RadioList *RadioList_Head);
/*����-17ʱ������Free_config_radio_channel_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int config_radio_channel_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char* channel_value,struct RadioList **RadioList_Head);
											 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾradio id ������*/
											 /*����-2��ʾRadio is disable,	please enable it first������-3��ʾerror*/
											 /*����-4��ʾchannel  is invalid in CHINA������-5��ʾchannel  is invalid in EUROPE*/
											 /*����-6��ʾchannel  is invalid in USA������-7��ʾchannel	is invalid in JAPAN*/
											 /*����-8��ʾchannel  is invalid in FRANCE������-9��ʾchannel  is invalid in SPAIN*/
											 /*����-10��ʾinput parameter error������-11��ʾ11a receive channel list is:  36 ..;149 153 157 161*/
											 /*����-12��ʾradio type doesn,t support this channel������-13��ʾerror������-14��ʾRadio ID�Ƿ�*/
											 /*����-15��ʾillegal input:Input exceeds the maximum value of the parameter type*/
											 /*����-16��ʾGroup ID�Ƿ�������-17��ʾpartial failure������-18��ʾgroup id does not exist*/
#endif
																							 
extern int config_radio_channel(dbus_parameter parameter, DBusConnection *connection,int id,char* channel_value);
																				/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾradio id ������*/
		 																	    /*����-2��ʾRadio is disable,  please enable it first������-3��ʾerror*/
		 																	    /*����-4��ʾchannel  is invalid in CHINA������-5��ʾchannel  is invalid in EUROPE*/
																				/*����-6��ʾchannel  is invalid in USA������-7��ʾchannel  is invalid in JAPAN*/
																			 	/*����-8��ʾchannel  is invalid in FRANCE������-9��ʾchannel  is invalid in SPAIN*/
		 																	    /*����-10��ʾinput parameter error������-11��ʾ11a receive channel list is:  36 ..;149 153 157 161*/
		 																	    /*����-12��ʾradio type doesn,t support this channel������-13��ʾerror������-14��ʾRadio ID�Ƿ�*/
																				/*����-15��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																				/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_config_radio_txpower_group(struct RadioList *RadioList_Head);

/*����-10ʱ������Free_config_radio_txpower_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*txpowerΪ100��ʾ"auto"*/
extern int config_radio_txpower_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,int txpower,struct RadioList **RadioList_Head);
																				/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾtxpower conflict with country-code������-2��ʾradio id does not exist*/
																				/*����-3��ʾradio is disable, please enable it first������-4��ʾradio mode is 11n,not allow to set txpower������-5��ʾthis radio max txpower is 20*/
																				/*����-6��ʾthis radio max txpower is 27������-7��ʾ��������-8��ʾRadio ID�Ƿ�������-9��ʾGroup ID�Ƿ�*/
																				/*����-10��ʾpartial failure������-11��ʾgroup id does not exist*/
#endif

/*txpowerΪ100��ʾ"auto"*/																							 
extern int config_radio_txpower(dbus_parameter parameter, DBusConnection *connection,int id,int txpower);	
																		/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾtxpower conflict with country-code������-2��ʾradio id does not exist*/
																		/*����-3��ʾradio is disable, please enable it first������-4��ʾradio mode is 11n,not allow to set txpower������-5��ʾthis radio max txpower is 20*/
																		/*����-6��ʾthis radio max txpower is 27������-7��ʾ��������-8��ʾRadio ID�Ƿ�*/
																		/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
#if _GROUP_POLICY
extern void Free_config_radio_rate_group(struct RadioList *RadioList_Head);

/*����-15ʱ������Free_config_radio_rate_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*rate list:10,20,55,60,90,110,120,180,240,360,480,540*/
extern int config_radio_rate_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char* radioRate,struct RadioList **RadioList_Head);
																/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾradio id does not exist������-2��ʾmode 11b support rate list:10 20 55 110*/
																/*����-3��ʾmode 11a support rate list:60 90 120 180 240 360 480 540������-4��ʾmode 11g support rate list:60 90 120 180 240 360 480 540*/
																/*����-5��ʾmode 11b/g support rate list:10 20 55 60 90 110 120 180 240 360 480 540������-6��ʾwtp radio does not support this rate,please check first*/ 
																/*����-7��ʾradio is disable, please enable it first������-8��ʾradio list is empty������-9��ʾradio mode is 11n,not allow to set rate*/
																/*����-10��ʾradio support rate does not exist������-11��ʾradio type is conflict, please check it first������-12��ʾ��������-13��ʾRadio ID�Ƿ�*/
																/*����-14��ʾGroup ID�Ƿ�������-15��ʾpartial failure������-16��ʾgroup id does not exist*/
																/*����-17��ʾmode 11an support rate list:60 90 120 180 240 360 480 540������-18��ʾmode 11gn support rate list:60 90 120 180 240 360 480 540*/
																/*����-19��ʾmode 11a/an support rate list:60 90 120 180 240 360 480 540������-20��ʾmode 11g/gn support rate list:60 90 120 180 240 360 480 540*/
																/*����-21��ʾmode 11b/g/n support rate list:10 20 60 90 110 120 180 240 360 480 540*/
#endif

/*rate list:10,20,55,60,90,110,120,180,240,360,480,540*/
extern int config_radio_rate(dbus_parameter parameter, DBusConnection *connection,int id,char* radioRate); 
																		/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾradio id does not exist������-2��ʾmode 11b support rate list:10 20 55 110*/
							                                            /*����-3��ʾmode 11a support rate list:60 90 120 180 240 360 480 540������-4��ʾmode 11g support rate list:60 90 120 180 240 360 480 540*/
																		/*����-5��ʾmode 11b/g support rate list:10 20 55 60 90 110 120 180 240 360 480 540������-6��ʾwtp radio does not support this rate,please check first*/	
																		/*����-7��ʾradio is disable, please enable it first������-8��ʾradio list is empty������-9��ʾradio mode is 11n,not allow to set rate*/
																		/*����-10��ʾradio support rate does not exist������-11��ʾradio type is conflict, please check it first������-12��ʾ��������-13��ʾRadio ID�Ƿ�*/
																		/*����-14��ʾmode 11an support rate list:60 90 120 180 240 360 480 540������-15��ʾmode 11gn support rate list:60 90 120 180 240 360 480 540*/
																		/*����-16��ʾmode 11a/an support rate list:60 90 120 180 240 360 480 540������-17��ʾmode 11g/gn support rate list:60 90 120 180 240 360 480 540*/
																		/*����-18��ʾmode 11b/g/n support rate list:10 20 60 90 110 120 180 240 360 480 540*/
																		/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
#if _GROUP_POLICY
extern void Free_config_radio_txpower_offset_group(struct RadioList *RadioList_Head);

/*����-13ʱ������Free_config_radio_txpower_offset_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int config_radio_txpower_offset_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,int txpower_off,struct RadioList **RadioList_Head);
																	/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter txpower error������-2��ʾtxpower conflict with country-code*/
																	/*����-3��ʾradio id does not exist������-4��ʾradio is disable, please enable it first������-5��ʾradio mode is 11n,not allow to set txpower*/
																	/*����-6��ʾthis radio max txpower is 20������-7��ʾthis radio max txpower is 27������-8��ʾthis wtp is not in run state*/
																	/*����-9��ʾthis radio is not binding wlan,binding wlan first.������-10��ʾerror������-11��ʾRadio ID�Ƿ�������-12��ʾGroup ID�Ƿ�*/
																	/*����-13��ʾpartial failure������-14��ʾgroup id does not exist������-15��ʾtxpoweroffset is larger than max txpower!Please checkout txpowerstep!*/
#endif

extern int config_radio_txpower_offset(dbus_parameter parameter, DBusConnection *connection,int id,int txpower_off);
																				   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter txpower error������-2��ʾtxpower conflict with country-code*/
																				   /*����-3��ʾradio id does not exist������-4��ʾradio is disable, please enable it first������-5��ʾradio mode is 11n,not allow to set txpower*/
																				   /*����-6��ʾthis radio max txpower is 20������-7��ʾthis radio max txpower is 27������-8��ʾthis wtp is not in run state*/
																				   /*����-9��ʾthis radio is not binding wlan,binding wlan first.������-10��ʾerror������-11��ʾRadio ID�Ƿ�*/
																				   /*����SNMPD_CONNECTION_ERROR��ʾconnection error������-15��ʾtxpoweroffset is larger than max txpower!Please checkout txpowerstep!*/

#if _GROUP_POLICY
extern void Free_config_radio_mode_group(struct RadioList *RadioList_Head);

/*����-8ʱ������Free_config_radio_mode_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*mode list:11a,11b,11g,11n,11b/g,11b/g/n,11a/n*/
extern int config_radio_mode_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *radioMode,struct RadioList **RadioList_Head);
										   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾmode�Ƿ�*/
										   /*����-2��ʾradio id �����ڣ�����-3��ʾRadio is disable,  please enable it first*/
										   /*����-4��ʾradio mode not allow to set with 11n������-5��ʾ����*/
										   /*����-6��ʾRadio ID�Ƿ�������-7��ʾGroup ID�Ƿ�*/
										   /*����-8��ʾpartial failure������-9��ʾgroup id does not exist*/
#endif

/*mode list:11a,11b,11g,11gn,11g/gn,11b/g,11b/g/n,11a/an,11an*/
extern int config_radio_mode(dbus_parameter parameter, DBusConnection *connection,int id,char *radioMode);
																			/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾmode�Ƿ�*/
																			/*����-2��ʾradio id �����ڣ�����-3��ʾRadio is disable,  please enable it first*/
																			/*����-4��ʾradio mode not allow to set with 11n������-5��ʾ����*/
																			/*����-6��ʾRadio ID�Ƿ�*/
																		    /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_config_max_rate_group(struct RadioList *RadioList_Head);

/*����-15ʱ������Free_config_max_rate_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int config_max_rate_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char*rad_rate,struct RadioList **RadioList_Head);
															/*����0��ʾʧ�ܣ�����1���سɹ�������-1��ʾradio id does not exist������-2��ʾmode 11b support rate list:10 20 55 110*/
															/*����-3��ʾmode 11a support rate list:60 90 120 180 240 360 480 540������-4��ʾmode 11g support rate list:60 90 120 180 240 360 480 540*/	 
															/*����-5��ʾmode 11b/g support rate list:10 20 55 60 90 110 120 180 240 360 480 540������-6��ʾwtp radio does not support this rate,please check first*/
															/*����-7��ʾradio is disable, please enable it first������-8��ʾradio mode is 11n,not allow to set rate������-9��ʾradio list is empty*/
															/*����-10��ʾradio support rate does not exist������-11��ʾradio type is conflict, please check it first������-12��ʾerror������-13��ʾRadio ID�Ƿ�*/
															/*����-14��ʾGroup ID�Ƿ�	������-15��ʾpartial failure������-16��ʾgroup id does not exist*/
															/*����-17��ʾmode 11an support rate list:60 90 120 180 240 360 480 540������-18��ʾmode 11gn support rate list:60 90 120 180 240 360 480 540*/
															/*����-19��ʾmode 11a/an support rate list:60 90 120 180 240 360 480 540������-20��ʾmode 11g/gn support rate list:60 90 120 180 240 360 480 540*/
															/*����-21��ʾmode 11b/g/n support rate list:10 20 60 90 110 120 180 240 360 480 540*/
#endif

/*Radio rate value: 11b/g: (10,20,55,60,90,110,120,180,240,360,480,540) */
/*11n: (65,130,135,150,195,260,270,300,390,405,450,520,540,585,600,650,780,810,900,1040,1080,1170,1200,1215,1300,1350,1500,1620,1800,2160,2400,2430,2700,3000)*/
extern int config_max_rate(dbus_parameter parameter, DBusConnection *connection,int id,char*rad_rate,struct RadioList **RadioList_Head);
																	/*����0��ʾʧ�ܣ�����1���سɹ�������-1��ʾRadio rate illegal*/
																	/*����-2��ʾradio id does not exist������-3��ʾmode 11b support rate list:10 20 55 110*/
																	/*����-4��ʾmode 11a support rate list:60 90 120 180 240 360 480 540*/
																	/*����-5��ʾmode 11g support rate list:60 90 120 180 240 360 480 540*/	 
																	/*����-6��ʾmode 11b/g support rate list:10 20 55 60 90 110 120 180 240 360 480 540*/
																	/*����-7��ʾmode 11b/g/n support rate list:10 20 60 90 110 120 180 240 360 480 540*/
																	/*����-8��ʾwtp radio does not support this rate,please check first*/
																	/*����-9��ʾradio is disable, please enable it first*/
																	/*����-10��ʾwtp is not binding wlan������-11��ʾradio list is empty*/
																	/*����-12��ʾradio support rate does not exist������-13��ʾradio type is conflict, please check it first*/
																	/*����-14��ʾerror������-15��ʾRadio ID�Ƿ�*/
																	/*����-16��ʾmode 11an support rate list:60 90 120 180 240 360 480 540*/
																	/*����-17��ʾmode 11gn support rate list:60 90 120 180 240 360 480 540*/
																	/*����-18��ʾmode 11a/an support rate list:60 90 120 180 240 360 480 540*/
																	/*����-19��ʾmode 11g/gn support rate list:60 90 120 180 240 360 480 540*/
#if _GROUP_POLICY
extern void Free_config_radio_beaconinterval_group(struct RadioList *RadioList_Head);

/*����-6ʱ������Free_config_radio_beaconinterval_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*ע���ж�BeaconInterval�ķ�Χ��Ĭ����100���趨��Χ��25-1000����λms*/
extern int config_radio_beaconinterval_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,int BeaconInterval,struct RadioList **RadioList_Head);
										/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾradio id ������*/
										/*����-2��ʾRadio is disable,  please enable it first������-3��ʾ����*/
										/*����-4��ʾRadio ID�Ƿ�������-6��ʾpartial failure������-7��ʾgroup id does not exist*/
#endif

extern int config_radio_beaconinterval(dbus_parameter parameter, DBusConnection *connection,int id,int BeaconInterval);	  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾradio id �����ڣ�����-2��ʾRadio is disable,  please enable it first������-3��ʾ��������-4��ʾRadio ID�Ƿ�*/
																															  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_config_radio_fragmentation_group(struct RadioList *RadioList_Head);

/*����-6ʱ������Free_config_radio_fragmentation_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*ע���ж�Fragmente�ķ�Χ��Ĭ����2346���趨��Χ��256-2346����λbyte*/
extern int config_radio_fragmentation_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,int Fragmente,struct RadioList **RadioList_Head);
											/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾradio id ������*/
											/*����-2��ʾRadio is disable,  please enable it first������-3��ʾ����*/
											/*����-4��ʾRadio ID�Ƿ�������-5��ʾGroup ID�Ƿ�*/
											/*����-6��ʾpartial failure������-7��ʾgroup id does not exist*/
#endif

extern int config_radio_fragmentation(dbus_parameter parameter, DBusConnection *connection,int id,int Fragmente);    /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾradio id �����ڣ�����-2��ʾRadio is disable,  please enable it first������-3��ʾ��������-4��ʾRadio ID�Ƿ�*/
																														/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_config_radio_dtim_group(struct RadioList *RadioList_Head);

/*����-6ʱ������Free_config_radio_dtim_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*ע���ж�Dtim�ķ�Χ��Ĭ����1 ��ȡֵ��Χ��1-15*/
extern int config_radio_dtim_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,int Dtim,struct RadioList **RadioList_Head);
													/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾradio id ������*/
													/*����-2��ʾRadio is disable,  please enable it first������-3��ʾ����*/
													/*����-4��ʾRadio ID�Ƿ�������-6��ʾpartial failure*/
													/*����-7��ʾgroup id does not exist*/
#endif

extern int config_radio_dtim(dbus_parameter parameter, DBusConnection *connection,int id,int Dtim);    /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾradio id �����ڣ�����-2��ʾRadio is disable,  please enable it first������-3��ʾ��������-4��ʾRadio ID�Ƿ�*/
																										 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_config_radio_rtsthreshold_group(struct RadioList *RadioList_Head);

/*����-6ʱ������Free_config_radio_rtsthreshold_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*ע���ж�TRS�ķ�Χ��Ĭ����2346 ��ȡֵ��Χ��256 - 2347*/
extern int config_radio_rtsthreshold_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,int RTS,struct RadioList **RadioList_Head);
										/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾradio id ������*/
										/*����-2��ʾRadio is disable,  please enable it first������-3��ʾ����*/
										/*����-4��ʾRadio ID�Ƿ�������-5��ʾGroup ID�Ƿ�*/
										/*����-6��ʾpartial failure������-7��ʾgroup id does not exist*/
#endif

extern int config_radio_rtsthreshold(dbus_parameter parameter, DBusConnection *connection,int id,int RTS);    /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾradio id �����ڣ�����-2��ʾRadio is disable,  please enable it first������-3��ʾ��������-4��ʾRadio ID�Ƿ�*/
																												 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_config_radio_service_group(struct RadioList *RadioList_Head);

/*����-7ʱ������Free_config_radio_service_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*stateΪ"enable"��"disable"*/
extern int config_radio_service_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *state,struct RadioList **RadioList_Head);
													/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾradio id ������*/
													/*����-2��ʾwtp not in run state������-3��ʾ��������-4��ʾRadio ID�Ƿ�*/
													/*����-5��ʾinput patameter should only be 'enable' or 'disable'������-6��ʾGroup ID�Ƿ�*/
													/*����-7��ʾpartial failure������-8��ʾgroup id does not exist*/
#endif

/*stateΪ"enable"��"disable"*/  
extern int config_radio_service(dbus_parameter parameter, DBusConnection *connection,int id,char *state); /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾradio id �����ڣ�����-2��ʾwtp not in run state������-3��ʾ��������-4��ʾRadio ID�Ƿ�*/
																											 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_config_radio_preamble_group(struct RadioList *RadioList_Head);

/*����-7ʱ������Free_config_radio_preamble_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*TypeΪ"short"��"long"*/
extern int config_radio_preamble_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *Type,struct RadioList **RadioList_Head);
										/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾradio id ������*/
										/*����-2��ʾRadio is disable,  please enable it first������-3��ʾ����*/
										/*����-4��ʾRadio ID�Ƿ�������-5��ʾinput parameter should only be 'long' or 'short'*/
										/*����-6��ʾGroup ID�Ƿ�	������-7��ʾpartial failure������-8��ʾgroup id does not exist*/
#endif

/*TypeΪ"short"��"long"*/	
extern int config_radio_preamble(dbus_parameter parameter, DBusConnection *connection,int id,char *Type);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾradio id �����ڣ�����-2��ʾRadio is disable,  please enable it first������-3��ʾ��������-4��ʾRadio ID�Ƿ�*/
																										 	 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_config_radio_longretry_group(struct RadioList *RadioList_Head);

/*����-6ʱ������Free_config_radio_longretry_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*ע���ж�Lretry�ķ�Χ��Ĭ��ֵ��4�� ȡֵ��Χ�� 1 -15*/
extern int config_radio_longretry_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,int Lretry,struct RadioList **RadioList_Head);
										/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾradio id ������*/
										/*����-2��ʾRadio is disable,  please enable it first������-3��ʾ����*/
										/*����-4��ʾRadio ID�Ƿ�������-5��ʾGroup ID�Ƿ�*/
									 	/*����-6��ʾpartial failure������-7��ʾgroup id does not exist*/
#endif

extern int config_radio_longretry(dbus_parameter parameter, DBusConnection *connection,int id,int Lretry);  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾradio id �����ڣ�����-2��ʾRadio is disable,	please enable it first������-3��ʾ��������-4��ʾRadio ID�Ƿ�*/
																											   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_config_radio_shortretry_group(struct RadioList *RadioList_Head);

/*����-6ʱ������Free_config_radio_shortretry_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*ע���ж�Sretry�ķ�Χ��Ĭ��ֵ��7�� ȡֵ��Χ�� 1 -15*/
extern int config_radio_shortretry_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,int Sretry,struct RadioList **RadioList_Head);
										/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾradio id ������*/
										/*����-2��ʾRadio is disable,  please enable it first������-3��ʾ����*/
										/*����-4��ʾRadio ID�Ƿ�������-6��ʾpartial failure*/
										/*����-7��ʾgroup id does not exist*/

#endif

extern int config_radio_shortretry(dbus_parameter parameter, DBusConnection *connection,int id,int Sretry); /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾradio id �����ڣ�����-2��ʾRadio is disable,  please enable it first������-3��ʾ��������-4��ʾRadio ID�Ƿ�*/
																											   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_bss_max_sta_num_group(struct RadioList *RadioList_Head);

/*����-9ʱ������Free_set_bss_max_sta_num_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_bss_max_sta_num_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *Wlan_id,char*bss_num,struct RadioList **RadioList_Head);
									/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter error*/
                                    /*����-2��ʾwlanid should be 1 to WLAN_NUM������-3��ʾmax station number should be greater than 0,and not cross 32767*/
                                    /*����-4��ʾbss not exist������-5��ʾmore sta(s) has accessed before you set max sta num */
                                    /*����-6��ʾoperation fail!������-7��ʾRadio ID�Ƿ�������-8��ʾGroup ID�Ƿ�*/
									/*����-9��ʾpartial failure������-10��ʾgroup id does not exist������-11��ʾwlan is not binded radio*/
									/*����-12��ʾbss is not exist*/
#endif

/*bss_num�ķ�Χ��0-32767*/
extern int set_bss_max_sta_num(dbus_parameter parameter, DBusConnection *connection,int id,char *Wlan_id,char*bss_num);
																						/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter error*/
				                                                                        /*����-2��ʾwlanid should be 1 to WLAN_NUM������-3��ʾmax station number should be greater than 0,and not cross 32767*/
				                                                                        /*����-4��ʾbss not exist������-5��ʾmore sta(s) has accessed before you set max sta num */
				                                                                        /*����-6��ʾoperation fail!������-7��ʾRadio ID�Ƿ�������-11��ʾwlan is not binded radio*/	
																						/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_radio_bss_l3_policy_group(struct RadioList *RadioList_Head);

/*����-17ʱ������Free_set_radio_bss_l3_policy_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_radio_bss_l3_policy_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *wlan_id,char *bss_policy,struct RadioList **RadioList_Head);
													/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter error*/
													/*����-2��ʾBSS is enable, if you want to operate this, please disable it first*/
													/*����-3��ʾBSS l3 interface is exist,you should delete this interface first*/
													/*����-4��ʾBSS create l3 interface fail������-5��ʾBSS delete l3 interface fail*/
													/*����-6��ʾWLAN policy is NO_INTERFACE,can not use this command*/
													/*����-7��ʾ BSS is not exist������-8��ʾcan not use this command*/
													/*����-9��ʾRADIO is not exist������-10��ʾWTP is not exist*/
													/*����-11��ʾWTP is not binding wlan������-12��ʾWLAN br is not exist*/
													/*����-13��ʾadd bss interface to wlan br fail������-14��ʾremove bss interface from wlan br fail*/
													/*����-15��ʾRadio ID�Ƿ�������-16��ʾGroup ID�Ƿ�*/
													/*����-17��ʾpartial failure������-18��ʾgroup id does not exist*/
													/*����-19��ʾinput parameter should be 1 to WLAN_NUM������-20��ʾwlan id is not exist*/
													/*����-21��ʾthis wlan id is not binding radio*/
#endif	

extern int set_radio_bss_l3_policy(dbus_parameter parameter, DBusConnection *connection,int rid,char *wlan_id,char *bss_policy); 
																							   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter error*/
																							   /*����-2��ʾBSS is enable, if you want to operate this, please disable it first*/
														  									   /*����-3��ʾBSS l3 interface is exist,you should delete this interface first*/
																							   /*����-4��ʾBSS create l3 interface fail������-5��ʾBSS delete l3 interface fail*/
																							   /*����-6��ʾWLAN policy is NO_INTERFACE,can not use this command*/
																							   /*����-7��ʾ BSS is not exist������-8��ʾcan not use this command*/
																							   /*����-9��ʾRADIO is not exist������-10��ʾWTP is not exist*/
																							   /*����-11��ʾWTP is not binding wlan������-12��ʾWLAN br is not exist*/
																							   /*����-13��ʾadd bss interface to wlan br fail������-14��ʾremove bss interface from wlan br fail*/
																							   /*����-15��ʾRadio ID�Ƿ�������-19��ʾinput parameter should be 1 to WLAN_NUM*/
																							   /*����-20��ʾwlan id is not exist������-21��ʾthis wlan id is not binding radio*/
																							   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_radio_apply_wlan_group(struct RadioList *RadioList_Head);

/*����-16ʱ������Free_radio_apply_wlan_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int radio_apply_wlan_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *wlan_id,struct RadioList **RadioList_Head);
										   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter error*/
										   /*����-2��ʾWLAN ID�Ƿ�������-3��ʾradio is not exist������-4��ʾWLAN is not exist*/
										   /*����-5��ʾbss num is already L_BSS_NUM������-6��ʾwtp wlan binding interface not match*/
										   /*����-7��ʾwtp not bind interface������-8��ʾwlan not bind interface*/
										   /*����-9��ʾwlan create wlan bridge fail������-10��ʾadd bss if to wlan bridge fail*/
										   /*����-11��ʾwtp over max wep wlan count 4������-12��ʾRadio ID�Ƿ�*/
										   /*����-13��ʾillegal input:Input exceeds the maximum value of the parameter type*/
										   /*����-14��ʾradio apply bingding securityindex is same with other*/
										   /*����-15��ʾGroup ID�Ƿ�������-16��ʾpartial failure*/
										   /*����-17��ʾgroup id does not exist*/
#endif

extern int radio_apply_wlan(dbus_parameter parameter, DBusConnection *connection,int rid,char *wlan_id);	
																		/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter error*/
				 														/*����-2��ʾWLAN ID�Ƿ�������-3��ʾradio is not exist������-4��ʾWLAN is not exist*/
																		/*����-5��ʾbss num is already L_BSS_NUM������-6��ʾwtp wlan binding interface not match*/
																		/*����-7��ʾwtp not bind interface������-8��ʾwlan not bind interface*/
																		/*����-9��ʾwlan create wlan bridge fail������-10��ʾadd bss if to wlan bridge fail*/
																		/*����-11��ʾwtp over max wep wlan count 4������-12��ʾRadio ID�Ƿ�*/
																		/*����-13��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																		/*����-14��ʾradio apply bingding securityindex is same with other*/
																		/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_radio_apply_qos_group(struct RadioList *RadioList_Head);

/*����-7ʱ������Free_radio_apply_qos_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*qos_id��Χ1-15*/
extern int radio_apply_qos_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *qos_id,struct RadioList **RadioList_Head);
												/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter error*/
												/*����-2��ʾQOS ID�Ƿ�������-3��ʾradio is not exist*/
												/*����-4��ʾQOS is not exist������-5��ʾRadio ID�Ƿ�*/
												/*����-6��ʾGroup ID�Ƿ�������-7��ʾpartial failure*/
												/*����-8��ʾgroup id does not exist*/
#endif

/*qos_id��Χ1-15*/
extern int radio_apply_qos(dbus_parameter parameter, DBusConnection *connection,int rid,char *qos_id);   
																	   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter error*/
																	   /*����-2��ʾQOS ID�Ƿ�������-3��ʾradio is not exist*/
																	   /*����-4��ʾQOS is not exist������-5��ʾRadio ID�Ƿ�*/
																	   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_radio_delete_qos_group(struct RadioList *RadioList_Head);

/*����-8ʱ������Free_radio_delete_qos_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*qos_id��Χ1-15*/
extern int radio_delete_qos_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *qos_id,struct RadioList **RadioList_Head);
												   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter error*/
												   /*����-2��ʾQOS ID�Ƿ�������-3��ʾradio is not exist������-4��ʾQOS is not exist*/
												   /*����-5��ʾradio is enable,please disable it first������-6��ʾRadio ID�Ƿ�*/
												   /*����-7��ʾGroup ID�Ƿ�������-8��ʾpartial failure������-9��ʾgroup id does not exist*/
#endif

/*qos_id��Χ1-15*/
extern int radio_delete_qos(dbus_parameter parameter, DBusConnection *connection,int rid,char *qos_id);	
																		/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter error*/
																	    /*����-2��ʾQOS ID�Ƿ�������-3��ʾradio is not exist������-4��ʾQOS is not exist*/
																	    /*����-5��ʾradio is enable,please disable it first������-6��ʾRadio ID�Ƿ�*/
																		/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_bss_max_throughput_group(struct RadioList *RadioList_Head);

/*����-7ʱ������Free_set_bss_max_throughput_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_bss_max_throughput_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *WLAN_ID,char *value,struct RadioList **RadioList_Head);
										/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾBSS is not exist*/
										/*����-2��ʾWTP is not exist������-3��ʾRADIO is not exist������-4��ʾerror*/
										/*����-5��ʾRadio ID�Ƿ�������-6��ʾGroup ID�Ƿ�*/
										/*����-7��ʾpartial failure������-8��ʾgroup id does not exist*/
										/*����-9��ʾinput parameter error������-10��ʾinput parameter should be 1 to WLAN_NUM*/
										/*����-11��ʾinput parameter should be 1-30������-12��ʾwlan id is not exist*/
#endif

extern int set_bss_max_throughput(dbus_parameter parameter, DBusConnection *connection,int RadioID,char *WLAN_ID,char *value);   
												/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾBSS is not exist*/
												/*����-2��ʾWTP is not exist������-3��ʾRADIO is not exist*/
												/*����-4��ʾerror������-5��ʾRadio ID�Ƿ�*/
												/*����-9��ʾinput parameter error������-10��ʾinput parameter should be 1 to WLAN_NUM*/
												/*����-11��ʾinput parameter should be 1-30������-12��ʾwlan id is not exist*/
												/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

																		
extern void Free_radio_bss_max_throughput_head(DCLI_RADIO_API_GROUP_ONE *RADIO);
/*����1ʱ������Free_radio_bss_max_throughput_head()�ͷſռ�*/
extern int show_radio_bss_max_throughput(dbus_parameter parameter, DBusConnection *connection,int radio_id ,DCLI_RADIO_API_GROUP_ONE **RADIO);
																												 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																												 /*����-1��ʾRADIO is not exist*/
																												 /*����-2��ʾWTP is not exist*/
																												 /*����-3��ʾerror������-4��ʾRadio ID�Ƿ�*/
																												 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_radio_delete_wlan_cmd_group(struct RadioList *RadioList_Head);

/*����-9ʱ������Free_set_radio_delete_wlan_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_radio_delete_wlan_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *wlan_id,struct RadioList **RadioList_Head);
											 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter error*/
											 /*����-2��ʾinput parameter should be 1 to WLAN_NUM-1������-3��ʾradio not exist*/
											 /*����-4��ʾwlan not exist������-5��ʾradio delete wlan fail������-6��ʾRadio ID�Ƿ�*/
											 /*����-7��ʾillegal input:Input exceeds the maximum value of the parameter type*/
											 /*����-8��ʾGroup ID�Ƿ�������-9��ʾpartial failure������-10��ʾgroup id does not exist*/
											 /*����-11��ʾbss is enable*/
#endif

extern int set_radio_delete_wlan_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *wlan_id);  
																					/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter error*/
																					/*����-2��ʾinput parameter should be 1 to WLAN_NUM-1������-3��ʾradio not exist*/
																					/*����-4��ʾwlan not exist������-5��ʾradio delete wlan fail������-6��ʾRadio ID�Ƿ�*/
																					/*����-7��ʾillegal input:Input exceeds the maximum value of the parameter type������-11��ʾbss is enable*/
																					/*����-12��ʾradio interface is in ebr,please delete it from ebr first*/
																					/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_radio_enable_wlan_cmd_group(struct RadioList *RadioList_Head);

/*����-14ʱ������Free_set_radio_enable_wlan_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_radio_enable_wlan_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *wlan_id,struct RadioList **RadioList_Head);
												/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter error*/
												/*����-2��ʾinput parameter should be 1 to WLAN_NUM-1������-3��ʾradio not exist*/
												/*����-4��ʾwlan not exist�� ����-5��ʾwtp over max wep wlan count 4*/
												/*����-6��ʾradio is already enable this wlan������-7��ʾwtp binding interface not match wlan binding interface*/
												/*����-8��ʾradio is not binding this wlan������-9��ʾwlan is disable ,you should enable it first*/
												/*����-10��ʾradio enable wlan fail������-11��ʾRadio ID�Ƿ�*/
												/*����-12��ʾillegal input:Input exceeds the maximum value of the parameter type*/
												/*����-13��ʾGroup ID�Ƿ�������-14��ʾpartial failure������-15��ʾgroup id does not exist*/
#endif

extern int set_radio_enable_wlan_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *wlan_id);  
																					/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter error*/
																					/*����-2��ʾinput parameter should be 1 to WLAN_NUM-1������-3��ʾradio not exist*/
																					/*����-4��ʾwlan not exist�� ����-5��ʾwtp over max wep wlan count 4*/
																					/*����-6��ʾradio is already enable this wlan������-7��ʾwtp binding interface not match wlan binding interface*/
																					/*����-8��ʾradio is not binding this wlan������-9��ʾwlan is disable ,you should enable it first*/
																					/*����-10��ʾradio enable wlan fail������-11��ʾRadio ID�Ƿ�*/
																					/*����-12��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																					/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_radio_disable_wlan_cmd_group(struct RadioList *RadioList_Head);

/*����-9ʱ������Free_set_radio_disable_wlan_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_radio_disable_wlan_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *wlan_id,struct RadioList **RadioList_Head);
													/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter error*/
													/*����-2��ʾinput parameter should be 1 to WLAN_NUM-1������-3��ʾradio not exist*/
													/*����-4��ʾwlan not exist������-5��ʾradio disable wlan fail������-6��ʾRadio ID�Ƿ�*/
													/*����-7��ʾillegal input:Input exceeds the maximum value of the parameter type*/
													/*����-8��ʾGroup ID�Ƿ�������-9��ʾpartial failure������-10��ʾgroup id does not exist*/
#endif

extern int set_radio_disable_wlan_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *wlan_id); 
																					/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter error*/
																					/*����-2��ʾinput parameter should be 1 to WLAN_NUM-1������-3��ʾradio not exist*/
																					/*����-4��ʾwlan not exist������-5��ʾradio disable wlan fail������-6��ʾRadio ID�Ƿ�*/
																					/*����-7��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																					/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																					
extern void Free_radio_channel_change(DCLI_RADIO_API_GROUP_ONE *RADIO);
/*����1ʱ������Free_radio_channel_change()�ͷſռ�*/
extern int show_radio_channel_change_func(dbus_parameter parameter, DBusConnection *connection,int rad_id,DCLI_RADIO_API_GROUP_ONE **RADIO);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾRADIO is not exist������-2��ʾWTP is not exist������-3��ʾerror������-4��ʾRadio ID�Ƿ�*/
																																					 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_radio_apply_wlan_base_vlan_cmd_group(struct RadioList *RadioList_Head);

/*����-20ʱ������Free_radio_apply_wlan_base_vlan_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int radio_apply_wlan_base_vlan_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *WlanID,char *VlanID,struct RadioList **RadioList_Head);
													/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter error*/
													/*����-2��ʾwlan id should be 1 to WLAN_NUM-1������-3��ʾvlan id should be 1 to VLANID_RANGE_MAX*/
													/*����-4��ʾradio id does not exist������-5��ʾwtp is in use, you should unused it first*/
													/*����-6��ʾbinding wlan does not exist������-7��ʾwlan does not bind interface*/
													/*����-8��ʾwtp does not bind interface������-9��ʾwlan and wtp bind interface did not match*/
													/*����-10��ʾclear wtp binding wlan list successfully������-11��ʾwlan is enable,you should disable it first*/
													/*����-12��ʾwtp over max bss count������-13��ʾbss is enable, you should disable it first*/
													/*����-14��ʾwtp over max wep wlan count 4������-15��ʾerror������-16��ʾRadio ID�Ƿ�*/
													/*����-17��ʾillegal input:Input exceeds the maximum value of the parameter type*/
													/*����-18��ʾradio apply bingding securityindex is same with other*/
													/*����-19��ʾGroup ID�Ƿ�������-20��ʾpartial failure������-21��ʾgroup id does not exist*/
#endif

extern int radio_apply_wlan_base_vlan_cmd(dbus_parameter parameter, DBusConnection *connection,int RadioID,char *WlanID,char *VlanID);
																										/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter error*/
																										/*����-2��ʾwlan id should be 1 to WLAN_NUM-1������-3��ʾvlan id should be 1 to VLANID_RANGE_MAX*/
																										/*����-4��ʾradio id does not exist������-5��ʾwtp is in use, you should unused it first*/
																										/*����-6��ʾbinding wlan does not exist������-7��ʾwlan does not bind interface*/
																										/*����-8��ʾwtp does not bind interface������-9��ʾwlan and wtp bind interface did not match*/
																										/*����-10��ʾclear wtp binding wlan list successfully������-11��ʾwlan is enable,you should disable it first*/
																										/*����-12��ʾwtp over max bss count������-13��ʾbss is enable, you should disable it first*/
																										/*����-14��ʾwtp over max wep wlan count 4������-15��ʾerror������-16��ʾRadio ID�Ƿ�*/
																										/*����-17��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																										/*����-18��ʾradio apply bingding securityindex is same with other*/
																										/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_radio_default_config_cmd_func_group(struct RadioList *RadioList_Head);

/*����-4ʱ������Free_set_radio_default_config_cmd_func_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_radio_default_config_cmd_func_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,struct RadioList **RadioList_Head);
									/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾrecover default config fail*/
									/*����-2��ʾRadio ID�Ƿ�������-3��ʾGroup ID�Ƿ�*/
									/*����-4��ʾpartial failure������-5��ʾgroup id does not exist*/
#endif

extern int set_radio_default_config_cmd_func(dbus_parameter parameter, DBusConnection *connection,int rad_id);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾrecover default config fail������-2��ʾRadio ID�Ƿ�*/
																													/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_radio_max_throughout_func_group(struct RadioList *RadioList_Head);

/*����-8ʱ������Free_set_radio_max_throughout_func_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_radio_max_throughout_func_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *value,struct RadioList **RadioList_Head);
												/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
												/*����-2��ʾmax throughout should be 1 to 108������-3��ʾWTP id does not exist*/
												/*����-4��ʾradio id does not exist������-5��ʾerror������-6��ʾRadio ID�Ƿ�*/
												/*����-7��ʾGroup ID�Ƿ�������-8��ʾpartial failure������-9��ʾgroup id does not exist*/
#endif

extern int set_radio_max_throughout_func(dbus_parameter parameter, DBusConnection *connection,int rad_id,char *value);
																						/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																						/*����-2��ʾmax throughout should be 1 to 108������-3��ʾWTP id does not exist*/
																						/*����-4��ʾradio id does not exist������-5��ʾerror������-6���ʾRadio ID�Ƿ�*/
																						/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																						
extern void Free_show_radio_qos(DCLI_RADIO_API_GROUP_ONE *RADIO);
/*����1ʱ������Free_show_radio_qos()�ͷſռ�*/
extern int show_radio_qos_cmd_func(dbus_parameter parameter, DBusConnection *connection,DCLI_RADIO_API_GROUP_ONE **RADIO);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾthere is no radio exist*/
																																 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_radio_bss_head(DCLI_RADIO_API_GROUP_ONE *RADIO);
/*����1ʱ������Free_radio_bss_head()�ͷſռ�*/
extern int show_radio_bss_cmd(dbus_parameter parameter, DBusConnection *connection,int radio_id,DCLI_RADIO_API_GROUP_ONE **RADIO);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾRadio ID�Ƿ�*/
																																		/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_radio_l2_isolation_func_group(struct RadioList *RadioList_Head);

/*����-12ʱ������Free_set_radio_l2_isolation_func_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_radio_l2_isolation_func_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *wlan_id,char *state,struct RadioList **RadioList_Head);
												/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
												/*����-2��ʾwlan id should be 1 to WLAN_NUM-1������-3��ʾinput patameter only with 'enable' or 'disable'*/
												/*����-4��ʾwlan not exist������-5��ʾwtp not binding wlan������-6��ʾwtp id does not run*/
												/*����-7��ʾbinding wlan error������-8��ʾerror������-9��ʾRadio ID�Ƿ�*/
												/*����-10��ʾillegal input:Input exceeds the maximum value of the parameter type*/
												/*����-11��ʾGroup ID�Ƿ�������-12��ʾpartial failure������-13��ʾgroup id does not exist*/
#endif

extern int set_radio_l2_isolation_func(dbus_parameter parameter, DBusConnection *connection,int radio_id,char *wlan_id,char *state);  
																									/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																									/*����-2��ʾwlan id should be 1 to WLAN_NUM-1������-3��ʾinput patameter only with 'enable' or 'disable'*/
																									/*����-4��ʾwlan not exist������-5��ʾwtp not binding wlan������-6��ʾwtp id does not run*/
																									/*����-7��ʾbinding wlan error������-8��ʾerror������-9��ʾRadio ID�Ƿ�*/
																									/*����-10��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																									/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_radio_11n_cwmmode_func_group(struct RadioList *RadioList_Head);

/*����-12ʱ������Free_set_radio_11n_cwmmode_func_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_radio_11n_cwmmode_func_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *wlan_id,char *cwmmode,struct RadioList **RadioList_Head);
													/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
													/*����-2��ʾwlan id should be 1 to WLAN_NUM-1������-3��ʾinput patameter only with 'enable' or 'disable'*/
													/*����-4��ʾwlan not exist������-5��ʾwtp not binding wlan������-6��ʾwtp id does not run*/
													/*����-7��ʾbinding wlan error������-8��ʾerror������-9��ʾRadio ID�Ƿ�*/
													/*����-10��ʾillegal input:Input exceeds the maximum value of the parameter type*/
													/*����-11��ʾGroup ID�Ƿ�������-12��ʾpartial failure������-13��ʾgroup id does not exist*/
#endif

extern int set_radio_11n_cwmmode_func(dbus_parameter parameter, DBusConnection *connection,int radio_id,char *wlan_id,char *cwmmode);
																										/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																										/*����-2��ʾwlan id should be 1 to WLAN_NUM-1������-3��ʾinput patameter only with 'enable' or 'disable'*/
																										/*����-4��ʾwlan not exist������-5��ʾwtp not binding wlan������-6��ʾwtp id does not run*/
																										/*����-7��ʾbinding wlan error������-8��ʾerror������-9��ʾRadio ID�Ƿ�*/
																										/*����-10��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																										/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
#if _GROUP_POLICY
extern void Free_set_wds_service_cmd_group(struct RadioList *RadioList_Head);

/*����-12ʱ������Free_set_wds_service_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_wds_service_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *WlanId,char *state,struct RadioList **RadioList_Head);
									/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter should only be 'enable' or 'disable'*/
									/*����-2��ʾwlan isn't existed������-3��ʾradio doesn't bind wlan argv[0]������-4��ʾradio id does not exist*/
									/*����-5��ʾwtp is not in run state������-6��ʾanother wds mode be used������-7��ʾerror*/
									/*����-8��ʾRadio ID�Ƿ�������-9��ʾillegal input:Input exceeds the maximum value of the parameter type*/
									/*����-10��ʾunknown id format������-11��ʾGroup ID�Ƿ�������-12��ʾpartial failure*/
									/*����-13��ʾgroup id does not exist*/
#endif
/*typeΪ"wds"��"mesh"*/
/*stateΪ"enable"��"disable"*/
extern int set_wds_service_cmd(dbus_parameter parameter, DBusConnection *connection,int RadID,char *WlanId,char *type,char *state);
																						/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter should only be 'enable' or 'disable'*/
																						/*����-2��ʾwlan isn't existed������-3��ʾradio doesn't bind wlan argv[0]������-4��ʾradio id does not exist*/
																						/*����-5��ʾwtp is not in run state������-6��ʾanother wds mode be used������-7��ʾerror*/
																						/*����-8��ʾRadio ID�Ƿ�������-9��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																						/*����-10��ʾunknown id format������-11��ʾinput patameter should only be 'wds' or 'mesh'*/
																						/*����-12��ʾanother mesh mode be used*/
																						/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_radio_wlan_wds_bssid_cmd_group(struct RadioList *RadioList_Head);

/*����-10ʱ������Free_radio_wlan_wds_bssid_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int radio_wlan_wds_bssid_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *WlanId,char *oper_type,char *MAC,struct RadioList **RadioList_Head);
										/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwlan id should be 1 to WLAN_NUM-1*/
										/*����-2��ʾinput patameter should only be 'add' or 'delelte'������-3��ʾUnknown mac addr format*/
										/*����-4��ʾwlan isn't existed������-5��ʾradio doesn't bind wlan argv[0]*/
										/*����-6��ʾanother WDS mode be used, disable first������-7��ʾWDS op failed������-8��ʾRadio ID�Ƿ�*/
										/*����-9��ʾGroup ID�Ƿ�������-10��ʾpartial failure������-11��ʾgroup id does not exist*/

#endif
/*typeΪ"add"��"delete"*/
/*LtypeΪ"wds_bssid"��"mesh_bssid"*/
extern int radio_wlan_wds_bssid_cmd(dbus_parameter parameter, DBusConnection *connection,int RadID,char *WlanId,char *type,char *Ltype,char *MAC);
																										/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwlan id should be 1 to WLAN_NUM-1*/
																										/*����-2��ʾinput patameter should only be 'add' or 'delelte'������-3��ʾUnknown mac addr format*/
																										/*����-4��ʾwlan isn't existed������-5��ʾradio doesn't bind wlan argv[0]*/
																										/*����-6��ʾanother WDS mode be used, disable first������-7��ʾWDS op failed*/
																										/*����-8��ʾRadio ID�Ƿ�������-9��ʾinput patameter should only be 'wds_bssid' or 'mesh_bssid'*/
																										/*����-10��ʾanother Mesh mode be used, disable first������-11��ʾMesh op faild*/
																										/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																										
extern void Free_wlan_wds_bssid_list_head(DCLI_RADIO_API_GROUP_ONE *RADIOINFO);
/*typeΪ"wds_bssid_list"��"mesh_bssid_list"*/
/*����1ʱ������Free_wlan_wds_bssid_list_head()�ͷſռ�*/																						
extern int show_wlan_wds_bssid_list_cmd(dbus_parameter parameter, DBusConnection *connection,int RadID,char *WlanId,char *type,DCLI_RADIO_API_GROUP_ONE **RADIOINFO);
																											/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																											/*����-2��ʾwlan id should be 1 to WLAN_NUM-1������-3��ʾwlan id does not exist*/
																											/*����-4��ʾerror������-5��ʾRadio ID�Ƿ�*/
																											/*����-6��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																											/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_ap_radio_auto_channel_func_group(struct RadioList *RadioList_Head);

/*����-7ʱ������Free_set_ap_radio_auto_channel_func_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_ap_radio_auto_channel_func_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *state,struct RadioList **RadioList_Head);
								/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'enable' or 'disable'*/
								/*����-2��ʾwtp id does not exist������-3��ʾradio id does not exist������-4��ʾerror*/
								/*����-5��ʾRadio ID�Ƿ�������-6��ʾGroup ID�Ƿ�������-7��ʾpartial failure*/
								/*����-8��ʾgroup id does not exist*/
#endif

extern int set_ap_radio_auto_channel_func(dbus_parameter parameter, DBusConnection *connection,int RID,char *state);
																					 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'enable' or 'disable'*/
																					 /*����-2��ʾwtp id does not exist������-3��ʾradio id does not exist������-4��ʾerror������-5��ʾRadio ID�Ƿ�*/
																					 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_ap_radio_auto_channel_cont_cmd_group(struct RadioList *RadioList_Head);

/*����-7ʱ������Free_set_ap_radio_auto_channel_cont_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_ap_radio_auto_channel_cont_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *state,struct RadioList **RadioList_Head);
																 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																 /*����-1��ʾinput patameter only with 'enable' or 'disable'*/
																 /*����-2��ʾwtp id does not exist*/
																 /*����-3��ʾradio id does not exist������-4��ʾerror*/
																 /*����-5��ʾRadio ID�Ƿ�������-6��ʾGroup ID�Ƿ�*/
																 /*����-7��ʾpartial failure������-8��ʾgroup id does not exist*/
#endif

/*stateΪenable��disable*/
extern int set_ap_radio_auto_channel_cont_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *state); 
																							 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																							 /*����-1��ʾinput patameter only with 'enable' or 'disable'*/
																							 /*����-2��ʾwtp id does not exist*/
																							 /*����-3��ʾradio id does not exist������-4��ʾerror*/
																							 /*����-5��ʾRadio ID�Ƿ�*/
																							 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_ap_radio_diversity_func_group(struct RadioList *RadioList_Head);

/*����-9ʱ������Free_set_ap_radio_diversity_func_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_ap_radio_diversity_func_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *state,struct RadioList **RadioList_Head);
						 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'enable' or 'disable'*/
						 /*����-2��ʾwtp id does not exist������-3��ʾradio id does not exist*/
						 /*����-4��ʾradio model not petmit to set diversity������-5��ʾerror������-6��ʾRadio ID�Ƿ�*/
						 /*����-7��ʾGroup ID�Ƿ�������-8��ʾto enable this function, you should restart wtp*/
						 /*����-9��ʾpartial failure������-10��ʾgroup id does not exist*/
#endif

extern int set_ap_radio_diversity_func(dbus_parameter parameter, DBusConnection *connection,int RID,char *state); 
																				 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'enable' or 'disable'*/
																				 /*����-2��ʾwtp id does not exist������-3��ʾradio id does not exist*/
																				 /*����-4��ʾradio model not petmit to set diversity������-5��ʾerror������-6��ʾRadio ID�Ƿ�*/
																				 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_ap_radio_txantenna_func_group(struct RadioList *RadioList_Head);

/*����-7ʱ������Free_set_ap_radio_txantenna_func_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_ap_radio_txantenna_func_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *txan_type,struct RadioList **RadioList_Head);
					 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'auto' 'main' or 'vice'*/
					 /*����-2��ʾwtp id does not exist������-3��ʾradio id does not exist������-4��ʾerror������-5��ʾRadio ID�Ƿ�*/
					 /*����-6��ʾGroup ID�Ƿ�������-7��ʾpartial failure������-8��ʾgroup id does not exist*/
#endif

extern int set_ap_radio_txantenna_func(dbus_parameter parameter, DBusConnection *connection,int RID,char *txan_type);
																					 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'auto' 'main' or 'vice'*/
																					 /*����-2��ʾwtp id does not exist������-3��ʾradio id does not exist������-4��ʾerror������-5��ʾRadio ID�Ƿ�*/
																					 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_radio_bss_traffic_limit_cmd_group(struct RadioList *RadioList_Head);

/*����-10ʱ������Free_radio_bss_traffic_limit_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*stateΪ"enable"��"disable"*/
extern int radio_bss_traffic_limit_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *WlanID,char *state,struct RadioList **RadioList_Head);
														 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
														 /*����-1��ʾwlan id should be 1 to WLAN_NUM-1*/
														 /*����-2��ʾinput patameter only with 'enable' or 'disable'*/
														 /*����-3��ʾwlan isn't existed������-4��ʾwtp isn't existed*/
														 /*����-5��ʾradio isn't existed*/
														 /*����-6��ʾradio doesn't bind wlan argv[0]������-7��ʾerror*/
														 /*����-8��ʾRadio ID�Ƿ�������-9��ʾGroup ID�Ƿ�*/
														 /*����-10��ʾpartial failure������-11��ʾgroup id does not exist*/
#endif

/*stateΪ"enable"��"disable"*/
extern int radio_bss_traffic_limit_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *WlanID,char *state);
																							/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																						    /*����-1��ʾwlan id should be 1 to WLAN_NUM-1*/
																						    /*����-2��ʾinput patameter only with 'enable' or 'disable'*/
																						    /*����-3��ʾwlan isn't existed������-4��ʾwtp isn't existed*/
																						    /*����-5��ʾradio isn't existed*/
																						    /*����-6��ʾradio doesn't bind wlan argv[0]������-7��ʾerror*/
																							/*����-8��ʾRadio ID�Ƿ�*/
																							/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_radio_bss_traffic_limit_value_cmd_group(struct RadioList *RadioList_Head);

/*����-11ʱ������Free_radio_bss_traffic_limit_value_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*VALUE�ķ�Χ��1-884736*/
extern int radio_bss_traffic_limit_value_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *WlanID,char *VALUE,struct RadioList **RadioList_Head);
													/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
													/*����-1��ʾwlan id should be 1 to WLAN_NUM-1*/
													/*����-2��ʾinput parameter argv[1] error*/
													/*����-3��ʾwlan isn't existed������-4��ʾwtp isn't existed*/
													/*����-5��ʾradio isn't existed*/
													/*����-6��ʾradio doesn't bind wlan argv[0]������-7��ʾerror*/
													/*����-8��ʾRadio ID�Ƿ�������-9��ʾinput value should be 1 to 884736*/
													/*����-10��ʾGroup ID�Ƿ�������-11��ʾpartial failure*/
													/*����-12��ʾgroup id does not exist*/
#endif

/*VALUE�ķ�Χ��1-884736*/
extern int radio_bss_traffic_limit_value_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *WlanID,char *VALUE);
																								   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																								   /*����-1��ʾwlan id should be 1 to WLAN_NUM-1*/
																								   /*����-2��ʾinput parameter argv[1] error*/
																								   /*����-3��ʾwlan isn't existed������-4��ʾwtp isn't existed*/
																								   /*����-5��ʾradio isn't existed*/
																								   /*����-6��ʾradio doesn't bind wlan argv[0]������-7��ʾerror*/
																								   /*����-8��ʾRadio ID�Ƿ�������-9��ʾinput value should be 1 to 884736*/
																								   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_radio_bss_traffic_limit_average_value_cmd_group(struct RadioList *RadioList_Head);

/*����-11ʱ������Free_radio_bss_traffic_limit_average_value_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int radio_bss_traffic_limit_average_value_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *WlanID,char *VALUE,struct RadioList **RadioList_Head);
												   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
												   /*����-1��ʾwlan id should be 1 to WLAN_NUM-1*/
												   /*����-2��ʾinput parameter argv[1] error*/
												   /*����-3��ʾwlan isn't existed������-4��ʾwtp isn't existed*/
												   /*����-5��ʾradio isn't existed������-6��ʾradio doesn't bind wlan argv[0]*/
												   /*����-7��ʾstation traffic limit value is more than bss traffic limit value*/
												   /*����-8��ʾerror������-9��ʾRadio ID�Ƿ�*/
												   /*����-10��ʾGroup ID�Ƿ�������-11��ʾpartial failure*/
												   /*����-12��ʾgroup id does not exist������-13��ʾinput value should be 1 to 884736*/
#endif

extern int radio_bss_traffic_limit_average_value_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *WlanID,char *VALUE);
																											 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																										     /*����-1��ʾwlan id should be 1 to WLAN_NUM-1*/
																										     /*����-2��ʾinput parameter argv[1] error*/
																										     /*����-3��ʾwlan isn't existed������-4��ʾwtp isn't existed*/
																										     /*����-5��ʾradio isn't existed������-6��ʾradio doesn't bind wlan argv[0]*/
																										     /*����-7��ʾstation traffic limit value is more than bss traffic limit value*/
																										     /*����-8��ʾerror������-9��ʾRadio ID�Ƿ�*/
																											 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																											 /*����-13��ʾinput value should be 1 to 884736*/
																											 
extern int radio_bss_traffic_limit_sta_value_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *WlanID,char *MAC,char *VALUE);
																											 	  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																												  /*����-1��ʾwlan id should be 1 to WLAN_NUM-1*/
																												  /*����-2��ʾinput parameter argv[2] error*/
																												  /*����-3��ʾwlan doesn't work*/
																												  /*����-4��ʾcan't find sta under wlan WlanID*/
																												  /*����-5��ʾstation traffic limit value is more than bss traffic limit value*/
																												  /*����-6��ʾerror������-7��ʾRadio ID�Ƿ�*/
																												  /*����-8��ʾinput value should be 1 to 884736*/

#if _GROUP_POLICY
extern void Free_radio_bss_traffic_limit_cancel_sta_value_cmd_group(struct RadioList *RadioList_Head);

/*����-11ʱ������Free_radio_bss_traffic_limit_cancel_sta_value_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int radio_bss_traffic_limit_cancel_sta_value_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *WlanID,char *MAC,struct RadioList **RadioList_Head);
															  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
															  /*����-1��ʾwlan id should be 1 to WLAN_NUM-1*/
															  /*����-2��ʾwlan doesn't work*/
															  /*����-3��ʾcan't find sta under wlan WlanID*/
															  /*����-4��ʾwlan isn't existed������-5��ʾwtp isn't existed*/
															  /*����-6��ʾradio isn't existed*/
															  /*����-7��ʾradio doesn't bind wlan argv[0]������-8��ʾerror*/
															  /*����-9��ʾRadio ID�Ƿ�������-10��ʾGroup ID�Ƿ�*/
															  /*����-11��ʾpartial failure������-12��ʾgroup id does not exist*/
#endif

extern int radio_bss_traffic_limit_cancel_sta_value_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *WlanID,char *MAC);
																											  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																										      /*����-1��ʾwlan id should be 1 to WLAN_NUM-1*/
																										      /*����-2��ʾwlan doesn't work*/
																										      /*����-3��ʾcan't find sta under wlan WlanID*/
																										      /*����-4��ʾwlan isn't existed������-5��ʾwtp isn't existed*/
																										      /*����-6��ʾradio isn't existed*/
																										      /*����-7��ʾradio doesn't bind wlan argv[0]������-8��ʾerror*/
																											  /*����-9��ʾRadio ID�Ƿ�*/
																											  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_radio_apply_wlan_clean_vlan_cmd_func_group(struct RadioList *RadioList_Head);

/*����-12ʱ������Free_radio_apply_wlan_clean_vlan_cmd_func_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int radio_apply_wlan_clean_vlan_cmd_func_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *WlanID,struct RadioList **RadioList_Head);
									/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
									/*����-2��ʾwlan id should be 1 to WLAN_NUM-1������-3��ʾwtp id does not exist*/
									/*����-4��ʾradio id does not exist������-5��ʾbinding wlan does not exist*/
									/*����-6��ʾradio is not binding this wlan������-7��ʾbss is enable, you should disable it first*/
									/*����-8��ʾerror������-9��ʾRadio ID�Ƿ�*/
									/*����-10��ʾillegal input:Input exceeds the maximum value of the parameter type*/
									/*����-11��ʾGroup ID�Ƿ�������-12��ʾpartial failure*/
									/*����-13��ʾgroup id does not exist*/
#endif

extern int radio_apply_wlan_clean_vlan_cmd_func(dbus_parameter parameter, DBusConnection *connection,int Rid,char *WlanID);
																							  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																							  /*����-2��ʾwlan id should be 1 to WLAN_NUM-1������-3��ʾwtp id does not exist*/
																							  /*����-4��ʾradio id does not exist������-5��ʾbinding wlan does not exist*/
																							  /*����-6��ʾradio is not binding this wlan������-7��ʾbss is enable, you should disable it first*/
																							  /*����-8��ʾerror������-9��ʾRadio ID�Ƿ�*/
																							  /*����-10��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																							  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_radio_bss_traffic_limit_send_value_cmd_group(struct RadioList *RadioList_Head);

/*����-10ʱ������Free_radio_bss_traffic_limit_send_value_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int radio_bss_traffic_limit_send_value_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *WlanID,char *VALUE,struct RadioList **RadioList_Head);
														  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
														  /*����-1��ʾwlan id should be 1 to WLAN_NUM-1*/
														  /*����-2��ʾinput parameter argv[1] error*/
														  /*����-3��ʾwtp isn't existed������-4��ʾradio isn't existed*/
														  /*����-5��ʾradio doesn't bind wlan argv[0]*/
														  /*����-6��ʾwlan isn't existed������-7��ʾerror*/
														  /*����-8��ʾRadio ID�Ƿ�������-9��ʾGroup ID�Ƿ�*/
														  /*����-10��ʾpartial failure������-11��ʾgroup id does not exist*/
														  /*����-12��ʾinput value should be 1 to 884736*/
#endif				

extern int radio_bss_traffic_limit_send_value_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *WlanID,char *VALUE);
																										  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																										  /*����-1��ʾwlan id should be 1 to WLAN_NUM-1*/
																										  /*����-2��ʾinput parameter argv[1] error*/
																										  /*����-3��ʾwtp isn't existed������-4��ʾradio isn't existed*/
																										  /*����-5��ʾradio doesn't bind wlan argv[0]*/
																										  /*����-6��ʾwlan isn't existed������-7��ʾerror*/
																										  /*����-8��ʾRadio ID�Ƿ�*/
																										  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																										  /*����-12��ʾinput value should be 1 to 884736*/

#if _GROUP_POLICY
extern void Free_radio_bss_traffic_limit_average_send_value_cmd_group(struct RadioList *RadioList_Head);

/*����-11ʱ������Free_radio_bss_traffic_limit_average_send_value_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int radio_bss_traffic_limit_average_send_value_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *WlanID,char *VALUE,struct RadioList **RadioList_Head);
												  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
												  /*����-1��ʾwlan id should be 1 to WLAN_NUM-1*/
												  /*����-2��ʾinput parameter argv[1] error*/
												  /*����-3��ʾstation traffic limit value is more than bss traffic limit value*/
												  /*����-4��ʾwtp isn't existed������-5��ʾradio isn't existed*/
												  /*����-6��ʾradio doesn't bind wlan argv[0]*/
												  /*����-7��ʾwlan isn't existed������-8��ʾerror*/
												  /*����-9��ʾRadio ID�Ƿ�������-10��ʾGroup ID�Ƿ�*/
												  /*����-11��ʾpartial failure������-12��ʾgroup id does not exist*/
												  /*����-13��ʾinput value should be 1 to 884736*/
#endif

extern int radio_bss_traffic_limit_average_send_value_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *WlanID,char *VALUE);
																													/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																												    /*����-1��ʾwlan id should be 1 to WLAN_NUM-1*/
																												    /*����-2��ʾinput parameter argv[1] error*/
																												    /*����-3��ʾstation traffic limit value is more than bss traffic limit value*/
																												    /*����-4��ʾwtp isn't existed������-5��ʾradio isn't existed*/
																												    /*����-6��ʾradio doesn't bind wlan argv[0]*/
																												    /*����-7��ʾwlan isn't existed������-8��ʾerror*/
																													/*����-9��ʾRadio ID�Ƿ�*/
																													/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																													/*����-13��ʾinput value should be 1 to 884736*/

#if _GROUP_POLICY
extern void Free_radio_bss_traffic_limit_sta_send_value_cmd_group(struct RadioList *RadioList_Head);

/*����-9ʱ������Free_radio_bss_traffic_limit_sta_send_value_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int radio_bss_traffic_limit_sta_send_value_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *WlanID,char *MAC,char *VALUE,struct RadioList **RadioList_Head);
																	/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																	/*����-1��ʾwlan id should be 1 to WLAN_NUM-1*/
																	/*����-2��ʾinput parameter argv[2] error*/
																	/*����-3��ʾwlan doesn't work*/
																	/*����-4��ʾcan't find sta under wlan WlanID*/
																	/*����-5��ʾstation traffic limit value is more than bss traffic limit value*/
																	/*����-6��ʾerror������-7��ʾRadio ID�Ƿ�*/
																	/*����-8��ʾGroup ID�Ƿ�������-9��ʾpartial failure*/
																	/*����-10��ʾgroup id does not exist*/
																	/*����-11��ʾinput value should be 1 to 884736*/
#endif

extern int radio_bss_traffic_limit_sta_send_value_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *WlanID,char *MAC,char *VALUE);
																														/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																													    /*����-1��ʾwlan id should be 1 to WLAN_NUM-1*/
																													    /*����-2��ʾinput parameter argv[2] error*/
																													    /*����-3��ʾwlan doesn't work*/
																													    /*����-4��ʾcan't find sta under wlan WlanID*/
																													    /*����-5��ʾstation traffic limit value is more than bss traffic limit value*/
																												        /*����-6��ʾerror������-7��ʾRadio ID�Ƿ�*/
																														/*����-11��ʾinput value should be 1 to 884736*/

#if _GROUP_POLICY
extern void Free_radio_bss_traffic_limit_cancel_sta_send_value_cmd_group(struct RadioList *RadioList_Head);

/*����-11ʱ������Free_radio_bss_traffic_limit_cancel_sta_send_value_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int radio_bss_traffic_limit_cancel_sta_send_value_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *WlanID,char *MAC,struct RadioList **RadioList_Head);
																	 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																	 /*����-1��ʾwlan id should be 1 to WLAN_NUM-1*/
																	 /*����-2��ʾwlan doesn't work*/
																	 /*����-3��ʾcan't find sta under wlan WlanID*/
																	 /*����-4��ʾwtp isn't existed������-5��ʾradio isn't existed*/
																	 /*����-6��ʾradio doesn't bind wlan argv[0]*/
																	 /*����-7��ʾwlan isn't existed������-8��ʾerror*/
																	 /*����-9��ʾRadio ID�Ƿ�������-10��ʾGroup ID�Ƿ�*/
																	 /*����-11��ʾpartial failure������-12��ʾgroup id does not exist*/
#endif

extern int radio_bss_traffic_limit_cancel_sta_send_value_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *WlanID,char *MAC);
																													 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																												     /*����-1��ʾwlan id should be 1 to WLAN_NUM-1*/
																												     /*����-2��ʾwlan doesn't work*/
																												     /*����-3��ʾcan't find sta under wlan WlanID*/
																												     /*����-4��ʾwtp isn't existed������-5��ʾradio isn't existed*/
																												     /*����-6��ʾradio doesn't bind wlan argv[0]*/
																												     /*����-7��ʾwlan isn't existed������-8��ʾerror*/
																													 /*����-9��ʾRadio ID�Ƿ�*/
																													 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_set_sta_mac_vlanid_cmd(struct dcli_sta_info *sta);
/*��flagΪ1ʱ������Free_set_sta_mac_vlanid_cmd()�ͷſռ�*/																													 
extern int set_sta_mac_vlanid_cmd(dbus_parameter parameter, DBusConnection *connection,unsigned int *flag,char *sta_mac,char *vlanID);
																									  /*����0��ʾʧ�ܣ�����1��ʾ������ -1��ʾvlan id should be 0 to 4095*/
																								      /*����-2��ʾinput parameter argv[2] error������-3��ʾset info error*/
																									  /*����-4��ʾwid set error������-5��ʾcan't find sta*/
																									  /*����-6��ʾcheck sta set invalid value������-7��ʾcheck sta error*/

#if _GROUP_POLICY
extern void Free_set_sta_dhcp_before_authorized_cmd_group(struct RadioList *RadioList_Head);

/*����-12ʱ������Free_set_sta_dhcp_before_authorized_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*stateΪ"enable"��"disable"*/
extern int set_sta_dhcp_before_authorized_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *wlan_id,char *state,struct RadioList **RadioList_Head);
								   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
								   /*����-2��ʾwlan id should be 1 to WLAN_NUM-1������-3��ʾinput patameter only with 'enable' or 'disable'*/
								   /*����-4��ʾwlan not exist������-5��ʾwtp not binding wlan argv[0]*/
								   /*����-6��ʾwtp id does not run������-7��ʾbinding wlan error������-8��ʾerror������-9��ʾRadio ID�Ƿ�*/
								   /*����-10��ʾillegal input:Input exceeds the maximum value of the parameter type*/
								   /*����-11��ʾGroup ID�Ƿ�������-12��ʾpartial failure������-13��ʾgroup id does not exist*/
#endif

/*stateΪ"enable"��"disable"*/
extern int set_sta_dhcp_before_authorized_cmd(dbus_parameter parameter, DBusConnection *connection,int Rid,char *wlan_id,char *state);
																										 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																										 /*����-2��ʾwlan id should be 1 to WLAN_NUM-1������-3��ʾinput patameter only with 'enable' or 'disable'*/
																										 /*����-4��ʾwlan not exist������-5��ʾwtp not binding wlan argv[0]*/
																										 /*����-6��ʾwtp id does not run������-7��ʾbinding wlan error������-8��ʾerror������-9��ʾRadio ID�Ƿ�*/
																										 /*����-10��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																										 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_sta_ip_mac_binding_cmd_group(struct RadioList *RadioList_Head);

/*����-11ʱ������Free_set_sta_ip_mac_binding_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*stateΪ"enable"��"disable"*/
extern int set_sta_ip_mac_binding_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *wlan_id,char *state,struct RadioList **RadioList_Head);
							 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
							 /*����-2��ʾwlan id should be 1 to WLAN_NUM-1������-3��ʾinput patameter only with 'enable' or 'disable'*/
							 /*����-4��ʾwlan not exist������-5��ʾwtp not binding wlan argv[0]*/
							 /*����-6��ʾbinding wlan error������-7��ʾerror������-8��ʾRadio ID�Ƿ�*/
							 /*����-9��ʾillegal input:Input exceeds the maximum value of the parameter type*/
							 /*����-10��ʾGroup ID�Ƿ�������-11��ʾpartial failure������-12��ʾgroup id does not exist*/
#endif

/*stateΪ"enable"��"disable"*/
extern int set_sta_ip_mac_binding_cmd(dbus_parameter parameter, DBusConnection *connection,int Rid,char *wlan_id,char *state);
																								/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																							    /*����-2��ʾwlan id should be 1 to WLAN_NUM-1������-3��ʾinput patameter only with 'enable' or 'disable'*/
																							    /*����-4��ʾwlan not exist������-5��ʾwtp not binding wlan argv[0]*/
																							    /*����-6��ʾbinding wlan error������-7��ʾerror������-8��ʾRadio ID�Ƿ�*/
																								/*����-9��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																								/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_radio_guard_interval_cmd_group(struct RadioList *RadioList_Head);

/*����-8ʱ������Free_set_radio_guard_interval_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*intervalΪ"800"��"400"*/
extern int set_radio_guard_interval_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *Interval,struct RadioList **RadioList_Head);
										 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter error*/
										 /*����-2��ʾradio id does not exist������-3��ʾradio is disable, please enable it first*/
										 /*����-4��ʾradio is not binging wlan������-5��ʾerror������-6��ʾRadio ID�Ƿ�*/
										 /*����-7��ʾGroup ID�Ƿ�������-8��ʾpartial failure������-9��ʾgroup id does not exist*/
#endif

/*intervalΪ"800"��"400"*/
extern int set_radio_guard_interval_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *Interval);
																					 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter error*/
																					 /*����-2��ʾradio id does not exist������-3��ʾradio is disable, please enable it first*/
																					 /*����-4��ʾradio is not binging wlan������-5��ʾerror������-6��ʾRadio ID�Ƿ�*/
																					 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_radio_mcs_cmd_group(struct RadioList *RadioList_Head);

/*����-9ʱ������Free_set_radio_mcs_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*mcs_value��һ����","�ָ������ּ���*/
extern int set_radio_mcs_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *mcs_value,struct RadioList **RadioList_Head);
											/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾparse mcs list failed*/
											/*����-2��ʾradio id does not exist������-3��ʾradio is disable, please enable it first*/
											/*����-4��ʾerror������-5��ʾRadio ID�Ƿ�������-6��ʾGroup ID�Ƿ�*/
										    /*����-7��ʾpartial failure������-8��ʾgroup id does not exist*/
										    /*����-9��ʾmcs cross the border, if your stream is one,mcs should be 0~7,if your stream is two,mcs should be 8~15,and if your stream is three,mcs should be 16~23*/
#endif

/*mcs_value��һ����","�ָ������ּ���*/
extern int set_radio_mcs_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *mcs_value);
																		   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput mcs should be 0-31,format should be 1-31 or 1,2,31*/
																		   /*����-2��ʾradio id does not exist������-3��ʾradio is disable, please enable it first*/
																		   /*����-4��ʾerror������-5��ʾRadio ID�Ƿ�*/
																		   /*����-9��ʾmcs cross the border, if your stream is one,mcs should be 0~7,if your stream is two,mcs should be 8~15,and if your stream is three,mcs should be 16~23*/
																		   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_radio_cmmode_cmd_group(struct RadioList *RadioList_Head);

/*����-10ʱ������Free_set_radio_cmmode_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*cwmode_valueΪ"ht20"��"ht20/40"��"ht40"*/
extern int set_radio_cmmode_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *cwmode_value,struct RadioList **RadioList_Head);
										  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter error*/
										  /*����-2��ʾradio id does not exist������-3��ʾradio is disable, please enable it first*/
										  /*����-4��ʾradio is not binging wlan������-5��ʾerror������-6��ʾRadio ID�Ƿ�*/
										  /*����-7��ʾradio mode is not 11N ,don't support this command������-8��ʾGroup ID�Ƿ�*/
										  /*����-9��ʾchannel offset should be set none������-10��ʾpartial failure������-11��ʾgroup id does not exist*/
										  /*����-12��ʾthe current radio channel is larger than the max channel,you are not allowed to set channel offset up*/
#endif

/*cwmode_valueΪ"ht20"��"ht20/40"��"ht40"*/
extern int set_radio_cmmode_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *cwmode_value);
																				  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter error*/
																				  /*����-2��ʾradio id does not exist������-3��ʾradio is disable, please enable it first*/
																				  /*����-4��ʾradio is not binging wlan������-5��ʾerror������-6��ʾRadio ID�Ƿ�*/
																				  /*����-7��ʾradio mode is not 11N ,don't support this command*/
																				  /*����-12��ʾthe current radio channel is larger than the max channel,you are not allowed to set channel offset up!Please turn down channel*/
																				  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_radio_inter_vap_forwarding_cmd_group(struct RadioList *RadioList_Head);

/*����-9ʱ������Free_set_radio_inter_vap_forwarding_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*stateΪ"enable"��"disable"*/
extern int set_radio_inter_vap_forwarding_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *state,struct RadioList **RadioList_Head);
															/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
															/*����-1��ʾinput patameter only with 'enable' or 'disable'*/
															/*����-2��ʾwtp isn't existed������-3��ʾradio isn't existed*/
															/*����-4��ʾradio doesn't bind wlan*/
															/*����-5��ʾradio not support this command������-6��ʾerror*/
															/*����-7��ʾRadio ID�Ƿ�������-8��ʾGroup ID�Ƿ�*/
															/*����-9��ʾpartial failure������-10��ʾgroup id does not exist*/
															/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
#endif

/*stateΪ"enable"��"disable"*/
extern int set_radio_inter_vap_forwarding_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *state);
																						  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																						  /*����-1��ʾinput patameter only with 'enable' or 'disable'*/
																						  /*����-2��ʾwtp isn't existed������-3��ʾradio isn't existed*/
																						  /*����-4��ʾradio doesn't bind wlan*/
																						  /*����-5��ʾradio not support this command������-6��ʾerror*/
																						  /*����-7��ʾRadio ID�Ƿ�*/
																						  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_radio_keep_alive_period_cmd_group(struct RadioList *RadioList_Head);

/*����-8ʱ������Free_set_radio_keep_alive_period_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*value�ķ�Χ��1-3600*/
extern int set_radio_keep_alive_period_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *value,struct RadioList **RadioList_Head);
																  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																  /*����-1��ʾinput parameter should be 1-3600*/
																  /*����-2��ʾwtp isn't existed*/
																  /*����-3��ʾradio isn't existed*/
																  /*����-4��ʾradio doesn't bind wlan*/
																  /*����-5��ʾerror������-6��ʾRadio ID�Ƿ�*/
																  /*����-7��ʾGroup ID�Ƿ�������-8��ʾpartial failure*/
																  /*����-9��ʾgroup id does not exist*/
																  /*����-10��ʾthis radio not supports those commands*/
#endif

/*value�ķ�Χ��1-3600*/
extern int set_radio_keep_alive_period_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *value);  
																						/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																						/*����-1��ʾinput parameter should be 1-3600*/
																						/*����-2��ʾwtp isn't existed*/
																						/*����-3��ʾradio isn't existed*/
																						/*����-4��ʾradio doesn't bind wlan*/
																						/*����-5��ʾerror������-6��ʾRadio ID�Ƿ�*/
																						/*����-10��ʾthis radio not supports those commands*/
																						/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_radio_keep_alive_idle_time_cmd_group(struct RadioList *RadioList_Head);

/*����-8ʱ������Free_set_radio_keep_alive_idle_time_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*value�ķ�Χ��1-3600*/
extern int set_radio_keep_alive_idle_time_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *value,struct RadioList **RadioList_Head);
														/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
														/*����-1��ʾinput parameter should be 1-3600*/
														/*����-2��ʾwtp isn't existed*/
														/*����-3��ʾradio isn't existed*/
														/*����-4��ʾradio doesn't bind wlan*/
														/*����-5��ʾerror������-6��ʾRadio ID�Ƿ�*/
														/*����-7��ʾGroup ID�Ƿ�*/
														/*����-8��ʾpartial failure������-9��ʾgroup id does not exist*/
														/*����-10��ʾthis radio not supports those commands*/
#endif

/*value�ķ�Χ��1-3600*/
extern int set_radio_keep_alive_idle_time_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *value); 
																						  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																						  /*����-1��ʾinput parameter should be 1-3600*/
																						  /*����-2��ʾwtp isn't existed*/
																						  /*����-3��ʾradio isn't existed*/
																						  /*����-4��ʾradio doesn't bing wlan*/
																						  /*����-5��ʾerror������-6��ʾRadio ID�Ƿ�*/
																						  /*����-10��ʾthis radio not supports those commands*/
																						  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_radio_congestion_avoidance_cmd_group(struct RadioList *RadioList_Head);

/*����-8ʱ������Free_set_radio_congestion_avoidance_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*state�ķ�Χ��(disable|tail-drop|red|fwred|)*/
extern int set_radio_congestion_avoidance_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *state,struct RadioList **RadioList_Head);
																  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																  /*����-1��ʾunknow command*/
																  /*����-2��ʾwtp isn't existed*/
																  /*����-3��ʾradio isn't existed*/
																  /*����-4��ʾradio doesn't bind wlan*/
																  /*����-5��ʾerror������-6��ʾRadio ID�Ƿ�*/
																  /*����-7��ʾGroup ID�Ƿ�	  ������-8��ʾpartial failure*/
																  /*����-9��ʾgroup id does not exist*/
																  /*����-10��ʾthis radio not supports those commands*/
#endif

/*state�ķ�Χ��(disable|tail-drop|red|fwred|)*/
extern int set_radio_congestion_avoidance_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *state); 
																							 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																							 /*����-1��ʾunknow command*/
																							 /*����-2��ʾwtp isn't existed*/
																							 /*����-3��ʾradio isn't existed*/
																							 /*����-4��ʾradio doesn't bind wlan*/
																							 /*����-5��ʾerror������-6��ʾRadio ID�Ƿ�*/
																							 /*����-10��ʾthis radio not supports those commands*/
																							 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*stateΪ"enable"��"disable"*/
extern int set_wtp_list_sta_static_arp_enable_cmd(dbus_parameter parameter, DBusConnection *connection,char *wlan_id,char *wtp_list,char *state,char *if_name); 
																							 				 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																											 /*����-1��ʾunknown id format*/
																											 /*����-2��ʾwlan id should be 1 to WLAN_NUM-1*/
																											 /*����-3��ʾset wtp list error,like 1,8,9-20,33*/
																											 /*����-4��ʾinput patameter only with 'enable' or 'disable'*/
																											 /*����-5��ʾinterface no exist������-6��ʾerror*/
																											 /*����-7��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																											 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_wtp_sta_static_arp_enable_cmd_group(struct RadioList *RadioList_Head);

/*����-8ʱ������Free_set_wtp_sta_static_arp_enable_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*stateΪ"enable"��"disable"*/
extern int set_wtp_sta_static_arp_enable_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *wlan_id,char *state,char *if_name,struct RadioList **RadioList_Head);
														 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
														 /*����-1��ʾunknown id format*/
														 /*����-2��ʾwlan id should be 1 to WLAN_NUM-1*/
														 /*����-3��ʾinput patameter only with 'enable' or 'disable'*/
														 /*����-4��ʾerror������-5��ʾRadio ID�Ƿ�*/
														 /*����-6��ʾillegal input:Input exceeds the maximum value of the parameter type*/
														 /*����-7��ʾGroup ID�Ƿ�������-8��ʾpartial failure*/
														 /*����-9��ʾgroup id does not exist*/
#endif

/*stateΪ"enable"��"disable"*/
extern int set_wtp_sta_static_arp_enable_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *wlan_id,char *state,char *if_name);
																													 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																													 /*����-1��ʾunknown id format*/
																													 /*����-2��ʾwlan id should be 1 to WLAN_NUM-1*/
																													 /*����-3��ʾinput patameter only with 'enable' or 'disable'*/
																													 /*����-4��ʾinterface no exist*/
																												     /*����-5��ʾerror������-6��ʾRadio ID�Ƿ�*/
																													 /*����-7��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																													 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_radio_11n_ampdu_able_cmd_group(struct RadioList *RadioList_Head);

/*����-12ʱ������Free_set_radio_11n_ampdu_able_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*TypeΪ"ampdu"��"amsdu"*/
/*stateΪ"enable"��"disable"*/
extern int set_radio_11n_ampdu_able_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *Type,char *state,struct RadioList **RadioList_Head);
																 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																 /*����-1��ʾinput patameter only with 'enable' or 'disable'*/
																 /*����-2��ʾradio id does not exist*/
																 /*����-3��ʾradio is not binding wlan, please bind it first*/
																 /*����-4��ʾradio is disable, please enable it first*/
																 /*����-5��ʾradio mode is not 11n,don't support this op*/
																 /*����-6��ʾerror������-7��ʾRadio ID�Ƿ�*/
																 /*����-8��ʾinput patameter only with 'ampdu' or 'amsdu'*/
																 /*����-9��ʾamsdu switch is enable, please disable it first*/
																 /*����-10��ʾampdu switch is enable, please disable it first*/
																 /*����-11��ʾGroup ID�Ƿ�������-12��ʾpartial failure*/
																 /*����-13��ʾgroup id does not exist*/
#endif

/*TypeΪ"ampdu"��"amsdu"*/
/*stateΪ"enable"��"disable"*/
extern int set_radio_11n_ampdu_able_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *Type,char *state); 
																								 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																							     /*����-1��ʾinput patameter only with 'enable' or 'disable'*/
																							     /*����-2��ʾradio id does not exist*/
																							     /*����-3��ʾradio is not binding wlan, please bind it first*/
																							     /*����-4��ʾradio is disable, please enable it first*/
																								 /*����-5��ʾradio mode is not 11n,don't support this op*/
																								 /*����-6��ʾerror������-7��ʾRadio ID�Ƿ�*/
																								 /*����-8��ʾinput patameter only with 'ampdu' or 'amsdu'*/
																								 /*����-9��ʾamsdu switch is enable, please disable it first*/
																								 /*����-10��ʾampdu switch is enable, please disable it first*/
																								 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_radio_11n_ampdu_limit_cmd_group(struct RadioList *RadioList_Head);

/*����-12ʱ������Free_set_radio_11n_ampdu_limit_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*TypeΪ"ampdu"��"amsdu"*/
/*value�ķ�Χ��ampdu:1024-65535 amsdu:2290-4096*/
extern int set_radio_11n_ampdu_limit_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *Type,char *value,struct RadioList **RadioList_Head);
															 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
															 /*����-1��ʾinput patameter error*/
															 /*����-2��ʾinput patameter error,ampdu limit should be 1024-65535*/
															 /*����-3��ʾradio id does not exist*/
															 /*����-4��ʾradio is not binding wlan, please bind it first*/
															 /*����-5��ʾradio is disable, please enable it first*/
															 /*����-6��ʾradio mode is not 11n,don't support this op*/
															 /*����-7��ʾerror������-8��ʾRadio ID�Ƿ�*/
															 /*����-9��ʾinput patameter only with 'ampdu' or 'amsdu'*/
															 /*����-10��ʾinput patameter error,amsdu limit should be 2290-4096*/
															 /*����-11��ʾGroup ID�Ƿ�������-12��ʾpartial failure*/
															 /*����-13��ʾgroup id does not exist*/
#endif

/*TypeΪ"ampdu"��"amsdu"*/
/*value�ķ�Χ��ampdu:1024-65535 amsdu:2290-4096*/
extern int set_radio_11n_ampdu_limit_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *Type,char *value);  
																								  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																								  /*����-1��ʾinput patameter error*/
																								  /*����-2��ʾinput patameter error,limit should be 1024-65535*/
																								  /*����-3��ʾradio id does not exist*/
																								  /*����-4��ʾradio is not binding wlan, please bind it first*/
																								  /*����-5��ʾradio is disable, please enable it first*/
																								  /*����-6��ʾradio mode is not 11n,don't support this op*/
																								  /*����-7��ʾerror������-8��ʾRadio ID�Ƿ�*/
																								  /*����-9��ʾinput patameter only with 'ampdu' or 'amsdu'*/
																								  /*����-10��ʾinput patameter error,amsdu limit should be 2290-4096*/
																								  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_radio_11n_ampdu_subframe_cmd_group(struct RadioList *RadioList_Head);

/*����-11ʱ������Free_set_radio_11n_ampdu_subframe_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*TypeΪ"ampdu"��"amsdu"*/
/*value�ķ�Χ��2-64*/
extern int set_radio_11n_ampdu_subframe_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *Type,char *value,struct RadioList **RadioList_Head);
														  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
														  /*����-1��ʾinput patameter only with 'ampdu' or 'amsdu'*/
														  /*����-2��ʾinput patameter error*/
														  /*����-3��ʾinput patameter error,limit should be 2-64*/
														  /*����-4��ʾRadio ID�Ƿ�������-5��ʾradio id does not exist*/
														  /*����-6��ʾradio is not binding wlan, please bind it first*/
														  /*����-7��ʾradio is disable, please enable it first*/
														  /*����-8��ʾradio mode is not 11n,don't support this op������-9��ʾerror*/
														  /*����-10��ʾGroup ID�Ƿ�������-11��ʾpartial failure*/
														  /*����-12��ʾgroup id does not exist*/
#endif

/*TypeΪ"ampdu"��"amsdu"*/
/*value�ķ�Χ��2-64*/
extern int set_radio_11n_ampdu_subframe_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *Type,char *value);
																									   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																									   /*����-1��ʾinput patameter only with 'ampdu' or 'amsdu'*/
																									   /*����-2��ʾinput patameter error*/
																									   /*����-3��ʾinput patameter error,limit should be 2-64*/
																									   /*����-4��ʾRadio ID�Ƿ�������-5��ʾradio id does not exist*/
																									   /*����-6��ʾradio is not binding wlan, please bind it first*/
																									   /*����-7��ʾradio is disable, please enable it first*/
																									   /*����-8��ʾradio mode is not 11n,don't support this op������-9��ʾerror*/
																									   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_radio_11n_puren_mixed_cmd_group(struct RadioList *RadioList_Head);

/*����-11ʱ������Free_set_radio_11n_puren_mixed_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*workmodeΪ"puren"��"mixed"*/
extern int set_radio_11n_puren_mixed_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *wlan_id,char *workmode,struct RadioList **RadioList_Head);
														   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
														   /*����-1��ʾinput patameter error*/
														   /*����-2��ʾinput patameter only with 'puren' or 'mixed'*/
														   /*����-3��ʾradio id does not exist*/
														   /*����-4��ʾradio is not binding wlan, please bind it first*/
														   /*����-5��ʾradio is disable, please enable it first*/
														   /*����-6��ʾradio mode is not 11n,don't support this op*/
														   /*����-7��ʾerror������-8��ʾRadio ID�Ƿ�*/
														   /*����-9��ʾillegal input:Input exceeds the maximum value of the parameter type*/
														   /*����-10��ʾGroup ID�Ƿ�������-11��ʾpartial failure*/
														   /*����-12��ʾgroup id does not exist*/
#endif

/*workmodeΪ"puren"��"mixed"*/
extern int set_radio_11n_puren_mixed_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *wlan_id,char *workmode);
																									  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																									  /*����-1��ʾinput patameter error*/
																									  /*����-2��ʾinput patameter only with 'puren' or 'mixed'*/
																									  /*����-3��ʾradio id does not exist*/
																									  /*����-4��ʾradio is not binding wlan, please bind it first*/
																									  /*����-5��ʾradio is disable, please enable it first*/
																									  /*����-6��ʾradio mode is not 11n,don't support this op*/
																									  /*����-7��ʾerror������-8��ʾRadio ID�Ƿ�*/
																									  /*����-9��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																									  /*����-10��ʾnow radio mode is an or gn, belong to puren,you can set it to mixed*/
																									  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
#if 0
/*stateΪ"enable"��"disable"*/
extern int set_tx_chainmask_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *list,char *state);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																					 /*����-1��ʾinput patameter only with 'enable' or 'disable'*/
																					 /*����-2��ʾinput parameter only should be 0,1,2, one or more of them,such as 1,2 or 0,2*/
																					 /*����-3��ʾradio id does not exist������-4��ʾradio not support this command*/
																					 /*����-5��ʾerror������-6��ʾRadio ID�Ƿ�*/
#endif

#if _GROUP_POLICY
extern void Free_set_radio_11n_channel_offset_cmd_group(struct RadioList *RadioList_Head);

/*����-12ʱ������Free_set_radio_11n_channel_offset_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*stateΪ"up"��"down"*/
extern int set_radio_11n_channel_offset_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *state,struct RadioList **RadioList_Head);
				/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'enable' or 'down'*/
				/*����-2��ʾradio id does not exist������-3��ʾradio is not binding wlan, please bind it first*/			
				/*����-4��ʾradio is disable, please enable it first������-5��ʾradio mode is not 11n,don't support this op*/
				/*����-6��ʾerror������-7��ʾRadio ID�Ƿ�������-8��ʾradio channel bandwidth is not 40,don't support this op*/
			    /*����-9��ʾthe current radio channel is larger than the max channel,you are not allowed to set channel offset up*/
				/*����-10��ʾthe current radio channel is less than the min channel ,you are not allowed to set channel offset down*/
				/*����-11��ʾGroup ID�Ƿ�������-12��ʾpartial failure������-13��ʾgroup id does not exist*/
				/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
#endif

/*stateΪ"up"��"down"*/
extern int set_radio_11n_channel_offset_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *state);
																						/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'enable' or 'down'*/
																						/*����-2��ʾradio id does not exist������-3��ʾradio is not binding wlan, please bind it first*/			
																						/*����-4��ʾradio is disable, please enable it first������-5��ʾradio mode is not 11n,don't support this op*/
																						/*����-6��ʾerror������-7��ʾRadio ID�Ƿ�������-8��ʾradio channel bandwidth is not 40,don't support this op*/																						
																						/*����-9��ʾthe current radio channel is larger than the max channel,you are not allowed to set channel offset up!Please turn down channel*/
																						/*����-10��ʾthe current radio channel is less than the min channel ,you are not allowed to set channel offset down!Please turn up channel*/
																						/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_tx_chainmask_v2_cmd_group(struct RadioList *RadioList_Head);

/*����-11ʱ������Free_set_tx_chainmask_v2_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
/*TypeΪ"tx_chainmask"��"rx_chainmask"*/
/*valueΪ"1.0.0","0.1.0","1.1.0","0.0.1","1.0.1","0.1.1"��"1.1.1"*/
extern int set_tx_chainmask_v2_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *Type,char *value,struct RadioList **RadioList_Head);
		/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with '0.0.1','0.1.0','0.1.1','1.0.0','1.0.1','1.1.0' or '1.1.1'*/
		/*����-2��ʾradio id does not exist������-3��ʾradio not support this command*/ 		
		/*����-4��ʾradio mode is not 11N ,don't support this command������-5��ʾerror������-6��ʾRadio ID�Ƿ�*/
		/*����-7��ʾinput patameter only with 'tx_chainmask' or 'rx_chainmask'*/
		/*����-8��ʾradio chainmask number is 1, don't support this value*/
		/*����-9��ʾradio chainmask number is 2, don't support this value*/
		/*����-10��ʾGroup ID�Ƿ�������-11��ʾpartial failure������-12��ʾgroup id does not exist*/
		/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
#endif

/*TypeΪ"tx_chainmask"��"rx_chainmask"*/
/*valueΪ"1.0.0","0.1.0","1.1.0","0.0.1","1.0.1","0.1.1"��"1.1.1"*/
extern int set_tx_chainmask_v2_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *Type,char *value); 
																						/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with '0.0.1','0.1.0','0.1.1','1.0.0','1.0.1','1.1.0' or '1.1.1'*/
																						/*����-2��ʾradio id does not exist������-3��ʾradio not support this command*/ 		  
																						/*����-4��ʾradio mode is not 11N ,don't support this command������-5��ʾerror������-6��ʾRadio ID�Ƿ�*/
																						/*����-7��ʾinput patameter only with 'tx_chainmask' or 'rx_chainmask'*/
																						/*����-8��ʾradio chainmask number is 1, don't support this value*/
																					    /*����-9��ʾradio chainmask number is 2, don't support this value*/
																						/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#if _GROUP_POLICY
extern void Free_set_radio_txpowerstep_cmd_group(struct RadioList *RadioList_Head);

/*����-7ʱ������Free_set_radio_txpowerstep_cmd_group()�ͷſռ�*/
/*group_typeΪ1����ʾ������*/
/*group_typeΪ0����ʾ��������*/
extern int set_radio_txpowerstep_cmd_group(dbus_parameter parameter, DBusConnection *connection,int group_type,int group_id,char *txp_step,struct RadioList **RadioList_Head);
										/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
										/*����-1��ʾInput exceeds the maximum value of the parameter type*/
										/*����-2��ʾunknown id format������-3��ʾRADIO ID�Ƿ�*/ 		
										/*����-4��ʾradio id does not exist������-5��ʾthis radio is not binding wlan,binding wlan first*/
										/*����-6��ʾGroup ID�Ƿ�������-7��ʾpartial failure������-8��ʾgroup id does not exist*/
										/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
#endif

extern int set_radio_txpowerstep_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *txp_step); 
																					 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																					 /*����-1��ʾInput exceeds the maximum value of the parameter type*/
																					 /*����-2��ʾunknown id format������-3��ʾRADIO ID�Ƿ�*/		  
																					 /*����-4��ʾradio id does not exist������-5��ʾthis radio is not binding wlan,binding wlan first*/
																					 /*����-9��ʾtxpowerstep should > 0*/
																					 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_all_wlan_ssid_stats_information_of_all_radio_cmd(struct SSIDStatsInfo *WtpHead);
/*ֻҪ���ã���ͨ��Free_show_all_wlan_ssid_stats_information_of_all_radio_cmd()�ͷſռ�*/
extern int show_all_wlan_ssid_stats_information_of_all_radio_cmd(dbus_parameter parameter, DBusConnection *connection,struct SSIDStatsInfo **WtpHead); 
																									   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																									   /*����-1��ʾThere is no WTP now*/
																									   /*����-2��ʾerror*/
																									   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*Rssi�ķ�Χ��0-95*/
extern int set_radio_wlan_limit_rssi_access_sta_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *wlan_id,char *Rssi);
																		   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																		   /*����-1��ʾunknown id format*/
																		   /*����-2��ʾwlan id should be 1 to WLAN_NUM-1*/
																		   /*����-3��ʾRSSI should be 0 to 95*/
																		   /*����-4��ʾRADIO ID�Ƿ�*/		   
																		   /*����-5��ʾradio id does not exist*/
																		   /*����-6��ʾwtp id does not exist*/
																		   /*����-7��ʾwlan id does not exist*/
																		   /*����-8��ʾbss id does not exist*/
																		   /*����-9��ʾwlan is not bind by this radio*/

/*Switch��ȡֵ��"enable"��"disable"*/
extern int set_bss_multi_user_optimize_cmd(dbus_parameter parameter, DBusConnection *connection,int RID,char *wlanID,char *Switch);
																				   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																				   /*����-1��ʾinput parameter error*/
																				   /*����-2��ʾwlanid should be 1 to WLAN_NUM-1*/
																				   /*����-3��ʾRADIO ID�Ƿ�������-4��ʾbss not exist*/		   
																				   /*����-5��ʾoperation fail������-6��ʾwlan is not binded radio*/
																				   /*����-7��ʾerror*/

#endif

