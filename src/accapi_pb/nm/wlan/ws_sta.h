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
* ws_sta.h
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


#ifndef _WS_STA_H
#define _WS_STA_H

#include "ws_init_dbus.h"
#include "ws_dbus_list_interface.h"
#include "wcpss/wid/WID.h"
#include "dbus/wcpss/ACDbusDef1.h"
#include "dbus/asd/ASDDbusDef1.h"
#include "wcpss/asd/asd.h"
#include "ws_dcli_acl.h"




#define MAC2STRZ(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]

#define OKB 1024
#define OMB (1024*1024)
#define OGB (1024*1024*1024)



struct station_profile
{
  unsigned char mac[6];
  unsigned char in_addr[16];
  unsigned char ieee80211_state[20];
  unsigned char PAE[20];
  unsigned char BACKEND[20];    
  char StaTime[30];
  unsigned long long rxPackets;
  unsigned long long txPackets;
  double  flux;
  time_t online_time;
  int hour;
  int min;
  int sec;
  unsigned int sta_traffic_limit;
  unsigned int sta_send_traffic_limit;
  unsigned int vlan_id;
  struct station_profile *next;
};


struct extend_sta_info
{
	unsigned long long rx_bytes;
	unsigned long long tx_bytes;
	unsigned char mac[6];
	unsigned char mode;
	unsigned char channel;
	unsigned char rssi;
	unsigned short nRate; 
	unsigned char isPowerSave;
	unsigned char isQos;
	unsigned char in_addr[20];
	unsigned int snr;
	unsigned long long rr;
	unsigned long long tr;
	unsigned long long tp;
	double flux_rr;
	double flux_tr;
	double flux_tp;
	unsigned long long rx_pkts;
	unsigned long long tx_pkts;
	double rtx;
	time_t online_t;
	char  StaccTime[60];
	unsigned long long rtx_pkts;
	unsigned long long err_pkts;
	struct extend_sta_info *next;
};

struct extend_sta_profile
{
	unsigned int deny_num;
	unsigned int bss_num;
	unsigned int sta_num;
	unsigned int acc_tms;
	unsigned int auth_tms;
	unsigned int repauth_tms;
	unsigned int local_roam_count;
	unsigned int total_unconnect_count;
	struct extend_sta_info head;
};

struct wapi_protocol_info_profile
{
	unsigned int ConfigVersion;
	unsigned char WapiSupported;
	unsigned char WapiPreAuth;
	unsigned char WapiPreauthEnabled;
	unsigned char UnicastCipherEnabled;
	unsigned char AuthenticationSuiteEnabled;
	unsigned char MulticastRekeyStrict;
	unsigned char UnicastKeysSupported;
	unsigned int BKLifetime;
	unsigned int BKReauthThreshold;
	unsigned int SATimeout;
	unsigned char UnicastCipherSelected[4];
	unsigned char MulticastCipherSelected[4]; 
	unsigned char UnicastCipherRequested[4];
	unsigned char MulticastCipherRequested[4];
	unsigned int MulticastCipherSize;
	unsigned char MulticastCipher[4];
	unsigned int UnicastCipherSize;
	unsigned char UnicastCipher[4];
};

struct mac_list_profile
{
	unsigned char mac[6];
	struct mac_list_profile *next;
};
struct ac_mac_list_profile
{
	unsigned int maclist_acl;
	unsigned int black_mac_num;
	unsigned int white_mac_num;
	struct mac_list_profile *black_mac_list;
	struct mac_list_profile *white_mac_list;
};

extern 	void Free_bss_bymac(struct dcli_sta_info *sta);
/*ֻҪ���ú������͵���Free_bss_bymac()�ͷſռ�*/
extern  int show_sta_bymac(dbus_parameter parameter, DBusConnection *connection,char *arg_mac,struct dcli_sta_info **sta); /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾstation does not exist,����-2��ʾerror*/
																																/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern  int kick_sta_MAC(dbus_parameter parameter, DBusConnection *connection,char * mac_addr);
																	/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾstation does not exist*/
																	/*����-2��ʾUnknown mac addr format*/
																	/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_sta_summary(struct dcli_ac_info *ac);
/*����1ʱ������Free_sta_summary()�ͷſռ�*/
/*sort_typeΪ��ѡ������������Χ��"ascend"��"descend"��Ĭ��Ϊ"descend"*/
extern int show_sta_summary(dbus_parameter parameter, DBusConnection *connection,struct dcli_ac_info **ac,char *sort_type);
																												 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																												 /*����-2��ʾinput patameter should only be 'ascend' or 'descend'*/
																												 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*����1ʱ������Free_sta_summary()�ͷſռ�*/
extern  int show_station_list(dbus_parameter parameter, DBusConnection *connection,struct dcli_ac_info **ac);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																											   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*����1ʱ������Free_sta_summary()�ͷſռ�*/
extern 	int show_station_list_by_group(dbus_parameter parameter, DBusConnection *connection,struct dcli_ac_info **ac);/*ʧ�ܷ���0���ɹ�����1������-1��ʾerror*/
																											   			   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_sta_bywlanid(struct dcli_wlan_info *wlan);
/*����1ʱ������Free_sta_bywlanid()�ͷſռ�*/
extern	int show_sta_bywlanid(dbus_parameter parameter, DBusConnection *connection,int id,struct dcli_wlan_info **wlan);
																						/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																						/*����-1��ʾwlan id should be 1 to WLAN_NUM-1*/
																						/*����-2��ʾwlan does not exist������-3��ʾerror*/
																						/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																						
extern void Free_sta_bywtpid(struct dcli_wtp_info *wtp);
/*����1ʱ������Free_sta_bywtpid()�ͷſռ�*/
extern	int show_sta_bywtpid(dbus_parameter parameter, DBusConnection *connection,int id,struct dcli_wtp_info **wtp);
																					 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																					 /*����-1��ʾwtp id should be 1 to WTP_NUM-1*/
																					 /*����-2��ʾwtp does not provide service or it maybe does not exist*/
																					 /*����-3��ʾerror*/
																					 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																					 
/*statΪ"black"��"white"*/
extern int wlan_add_black_white(dbus_parameter parameter, DBusConnection *connection,int id,char *stat,char *arg_mac);
																					  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																					  /*����-1��ʾwlan id should be 1 to WLAN_NUM-1*/
																					  /*����-2��ʾinput patameter should only be 'black/white' or 'b/w'*/
																					  /*����-3��ʾUnknown mac addr format*/
																					  /*����-4��ʾwlan isn't existed������-5��ʾmac add already*/
																					  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*statΪ"black"��"white"*/																					 
extern int wlan_delete_black_white(dbus_parameter parameter, DBusConnection *connection,int id,char *stat,char *arg_mac);
																					  	/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																						/*����-1��ʾwlan id should be 1 to WLAN_NUM-1*/
																						/*����-2��ʾinput patameter should only be 'black/white' or 'b/w'*/
																						/*����-3��ʾUnknown mac addr format*/
																						/*����-4��ʾwlan isn't existed������-5��ʾmac is not in the list*/
																						/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*statΪ"none"/"black"/"white"*/
extern int wlan_use_none_black_white(dbus_parameter parameter, DBusConnection *connection,int id,char *stat);
																			  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																			  /*����-1��ʾwlan id should be 1 to WLAN_NUM-1*/
																			  /*����-2��ʾinput patameter should only be 'black/white/none' or 'b/w/n'*/
																			  /*����-3��ʾwlan isn't existed������-4��ʾwids has open,can't be set other except blank list*/
																			  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*statΪ"black"��"white"*/																	  
extern int wtp_add_black_white(dbus_parameter parameter, DBusConnection *connection,int id,char *stat,char *arg_mac);
																			  		 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																					 /*����-1��ʾwtp id should be 1 to WTP_NUM-1*/
																					 /*����-2��ʾinput patameter should only be 'black/white' or 'b/w'*/
																					 /*����-3��ʾUnknown mac addr format*/
																					 /*����-4��ʾwtp is not existed������-5��ʾmac add already*/
																					 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*statΪ"black"��"white"*/																			 
extern int wtp_delete_black_white(dbus_parameter parameter, DBusConnection *connection,int id,char *stat,char *arg_mac);
																					   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																					   /*����-1��ʾwtp id should be 1 to WTP_NUM-1*/
																					   /*����-2����input patameter should only be 'black/white' or 'b/w'*/
																					   /*����-3��ʾUnknown mac addr format������-4��ʾwtp is not existed*/
																					   /*����-5��ʾmac is not in the list*/
																					   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*statΪ"none"/"black"/"white"*/																				 
extern int wtp_use_none_black_white(dbus_parameter parameter, DBusConnection *connection,int id,char *stat); 
																			  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																			  /*����-1��ʾwtp id should be 1 to WTP_NUM-1*/
																			  /*����-2��ʾinput patameter should only be 'black/white' or 'b/w'*/
																			  /*����-3��ʾwtp isn't existed*/
																			  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*statΪ"black"��"white"*/																	  
extern int radio_bss_add_black_white(dbus_parameter parameter, DBusConnection *connection,int rid,char *wlanID,char *stat,char *arg_mac); 
																			  							 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																										 /*����-1��ʾradio id should be 1 to G_RADIO_NUM-1*/
																										 /*����-2��ʾwlan id should be 1 to WLAN_NUM-1*/
																										 /*����-3��ʾUnknown mac addr format*/
																										 /*����-4��ʾbss is not exist������-5��ʾmac add already*/
																										 /*����-6��ʾunknown id format������-7��ʾwlan is not exist*/
																										 /*����-8��ʾradio has not apply wlan������-9��ʾradio id is not exist*/
																										 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*statΪ"black"��"white"*/																							 
extern int radio_bss_delete_black_white(dbus_parameter parameter, DBusConnection *connection,int rid,char *wlanID,char *stat,char *arg_mac); 
																										    /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																											/*����-1��ʾradio id should be 1 to G_RADIO_NUM-1*/
																											/*����-2��ʾwlan id should be 1 to WLAN_NUM-1*/
																											/*����-3��ʾUnknown mac addr format*/
																											/*����-4��ʾbss is not exist������-5��ʾmac is not in the list*/
																											/*����-6��ʾunknown id format������-7��ʾwlan is not exist*/
																											/*����-8��ʾradio has not apply wlan������-9��ʾradio id is not exist*/
																											/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*statΪ"none"/"black"/"white"*/																								
extern int radio_bss_use_none_black_white(dbus_parameter parameter, DBusConnection *connection,int rid,char *wlanID,char *stat); 
																							  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																							  /*����-1��ʾradio id should be 1 to G_RADIO_NUM-1*/
																							  /*����-2��ʾwlan id should be 1 to WLAN_NUM-1*/
																							  /*����-3��ʾbss is not exist*/
																							  /*����-4��ʾunknown id format������-5��ʾwlan is not exist*/
																							  /*����-6��ʾradio has not apply wlan������-7��ʾmac add already*/
																						 	  /*����-8��ʾradio id is not exist*/
																							  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																					  

/*����1ʱ������Free_sta_bywlanid()�ͷź���*/
extern int show_wlan_mac_list(dbus_parameter parameter, DBusConnection *connection,char* id,struct dcli_wlan_info **wlan);
																						  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																						  /*����-1��ʾunknown id format*/
																						  /*����-2��ʾwlan id should be 1 to WLAN_NUM-1*/
																						  /*����-3��ʾwlan isn't existed������-4��ʾerror*/
																						  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*����1ʱ������Free_sta_bywtpid()�ͷſռ�*/
extern int show_wtp_mac_list(dbus_parameter parameter, DBusConnection *connection,int id,struct dcli_wtp_info **wtp);
																					 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																					 /*����-1��ʾwtp id should be 1 to WTP_NUM-1*/
																					 /*����-2��ʾwtp isn't existed������-3��ʾerror*/
																					 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_mac_head(struct dcli_bss_info *bss);
/*����1ʱ������Free_mac_head() �ͷſռ�*/																			 
extern int show_radio_bss_mac_list(dbus_parameter parameter, DBusConnection *connection,int rid,char *wlanID,struct dcli_bss_info **bss); 
																										  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																										  /*����-1��ʾradio id should be 1 to G_RADIO_NUM-1*/
																										  /*����-2��ʾwlan id should be 1 to WLAN_NUM-1*/
																										  /*����-3��ʾbss isn't existed������-4��ʾerror*/
																										  /*����-5��ʾunknown id format*/
																										  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*����1ʱ������Free_sta_summary()�ͷſռ�*/																							 
extern int show_all_wlan_mac_list(dbus_parameter parameter, DBusConnection *connection,struct dcli_ac_info **ac);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwlan isn't existed������-2��ʾerror*/
																										  			 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*����1ʱ������Free_sta_summary()�ͷſռ�*/																									 
extern int show_all_wtp_mac_list(dbus_parameter parameter, DBusConnection *connection,struct dcli_ac_info **ac);
																				/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp isn't existed*/
																				/*����-2��ʾinput wtp id should be 1 to WTP_NUM-1������-3��ʾerror*/
																				/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*����1ʱ������Free_sta_summary()�ͷſռ�*/																		
extern int show_all_bss_mac_list(dbus_parameter parameter, DBusConnection *connection,struct dcli_ac_info **ac);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																													/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																				
extern int extend_show_sta_mac(dbus_parameter parameter, DBusConnection *connection,char *MAC,struct extend_sta_info *sta_info);  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾstation does not exist*/
																																		/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*ֻҪ���ú������͵���Free_sta_bywtpid()�ͷſռ�*/
extern int extend_show_sta_bywtpid(dbus_parameter parameter, DBusConnection *connection,int id,struct dcli_wtp_info **wtp);
																							 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																							 /*����-1��ʾwtp id should be 1 to WTP_NUM-1*/
																							 /*����-2��ʾWTP does not provide service*/
																							 /*����-3��ʾwtp does not provide service or it maybe does not exist*/
																							 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern struct extend_sta_info *get_extend_sta_by_mac( struct extend_sta_profile *psta_profile, char mac[6]  );

/*ֻҪ���ú������͵���Free_sta_bywtpid(struct dcli_wtp_info *wtp)�ͷſռ�*/
extern int show_bss_info_cmd(dbus_parameter parameter, DBusConnection *connection,char *WTPID,struct dcli_wtp_info **wtp);
																						  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																						  /*����-1��ʾwtp id should be 1 to WTP_NUM-1*/
																						  /*����-2��ʾwtp does not provide service or it maybe does not exist*/
																						  /*����-3��ʾerror������-4��ʾunknown id format*/
																						  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*ֻҪ���ú������͵���Free_sta_bywtpid(struct dcli_wtp_info *wtp)�ͷſռ�*/																				 
extern int show_radio_info_cmd(dbus_parameter parameter, DBusConnection *connection,int WTPID,struct dcli_wtp_info **wtp);
																						  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																						  /*����-1��ʾwtp id should be 1 to WTP_NUM-1*/
																						  /*����-2��ʾwtp does not provide service or it maybe does not exist*/
																						  /*����-3��ʾerror*/
																						  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																				  
extern void Free_channel_access_time_head( struct dcli_channel_info *channel);
/*ֻҪ���ú������͵���Free_channel_access_time_head()�ͷſռ�*/
extern int show_channel_access_time_cmd(dbus_parameter parameter, DBusConnection *connection,struct dcli_channel_info **channel);
																				/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																				/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_mib_info_head( struct dcli_radio_info *radio);
/*ֻҪ���ú������͵���Free_mib_info_head()�ͷſռ�*/
extern int show_mib_info_cmd_func(dbus_parameter parameter, DBusConnection *connection,int Rid,struct dcli_radio_info **radio);
																								 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾradio id invalid*/
																								 /*����-2��ʾradio does not provide service or it maybe does not exist������-3��ʾerror*/
																								 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*����1ʱ������Free_sta_bywlanid()�ͷſռ�*/																						 
extern int show_wlan_info_cmd_func(dbus_parameter parameter, DBusConnection *connection,int WLANID,struct dcli_wlan_info **wlan);
																								   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwlan id should be 1 to WLAN_NUM-1*/
																								   /*����-2��ʾwlan does not provide service or it maybe does not exist������-3��ʾerror*/
																								   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																						   
/*ֻҪ���ú������͵���Free_sta_bywtpid()�ͷſռ�*/
extern int show_wapi_mib_info_cmd_func(dbus_parameter parameter, DBusConnection *connection,char *wtp_id,struct dcli_wtp_info **wtp);
																								   		/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format������-2��ʾwtpid id invalid*/
																										/*����-3��ʾwtp does not provide service or it maybe does not exist������-4��ʾerror*/
																										/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																								
extern int show_wapi_protocol_info_cmd_func(struct wapi_protocol_info_profile *wapi_protocol_info);
																										
/*ֻҪ���ú������͵���Free_sta_bywtpid()�ͷſռ�*/
extern int show_wapi_info_cmd_func(dbus_parameter parameter, DBusConnection *connection,char *WtpID,struct dcli_wtp_info **wtp); 
																								   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																								   /*����-2��ʾwtp id should be 1 to WTP_NUM-1������-3��ʾWTP does not provide service*/
																								   /*����-4��ʾwtp does not provide service or it maybe does not exist������-5��ʾerror*/
																								   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*state�ķ�Χ��"enable"��"disable"*/																						   
extern int set_ac_flow_cmd_func(dbus_parameter parameter, DBusConnection *connection,char *WlanID,char *state);
																			   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																			   /*����-2��ʾwlan id should be 1 to WLAN_NUM-1*/
																			   /*����-3��ʾinput patameter only with 'enable' or 'disable'������-4��ʾwlan isn't existed*/
																			   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																			   
/*����1ʱ������Free_sta_summary(�ͷſռ�*/
extern int show_traffic_limit_info_rd_cmd_func(dbus_parameter parameter, DBusConnection *connection,char *RID,struct dcli_ac_info **ac);
																			   							/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																										/*����-1��ʾunknown id format������-2��ʾradio id invalid*/
																										/*����-3��ʾradio does not provide service or it maybe does not exist*/
																										/*����-4��ʾerror*/
																										/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																								
/*������1ʱ������Free_traffic_limit_info()�ͷſռ�*/
extern int show_traffic_limit_info_cmd_func(dbus_parameter parameter, DBusConnection *connection,char *BssIndex,struct dcli_bss_info **bss);
																											/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown bssindex format*/
																											/*����-2��ʾbssindex invalid������-3��ʾbssindex BssIndex does not provide service or it maybe does not exist*/
																										    /*����-4��ʾerror*/
																											/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*����1ʱ������Free_sta_bywlanid()�ͷſռ�*/
extern int show_wlan_wids_MAC_list_cmd_func(dbus_parameter parameter, DBusConnection *connection,char *WLAN_ID,struct dcli_wlan_info  **wlan);
																												  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																												  /*����-2��ʾwlan id should be 1 to WLAN_NUM-1������-3��ʾwlan isn't existed*/
																												  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*stateΪ"enable"��"disable"*/
extern int set_wlan_extern_balance_cmd_func(dbus_parameter parameter, DBusConnection *connection,char *WlanID,char *state);
																						      /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																							  /*����-2��ʾwlan id should be 1 to WLAN_NUM-1*/
																							  /*����-3��ʾinput patameter should only be 'enable' or 'disable'*/
																							  /*����-4��ʾwlan isn't existed*/
																							  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																							  
extern void asd_state_check(unsigned char *ieee80211_state, unsigned int sta_flags, unsigned char *PAE, unsigned int pae_state, unsigned char *BACKEND, unsigned int backend_state);

extern void Free_sta_static_head(struct sta_static_info *tab);
/*����1ʱ������Free_sta_static_head( )�ͷſռ�*/
extern int show_all_sta_static_info_cmd(dbus_parameter parameter, DBusConnection *connection,struct sta_static_info **tab);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾthere is no static sta������-2��ʾstation does not exist������-3��ʾerror*/
																																/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*TypeΪ(vlanid | limit | send_limit)*/
/*��TypeΪvlanidʱ��Value�ķ�Χ��0-4095*/
/*wlanIDΪ��ѡ������������wlanIDʱ��������NULL*/
extern int set_sta_static_info_traffic_limit_cmd(dbus_parameter parameter, DBusConnection *connection,char *MAC,char *Type,char *Value,char *wlanID);
																									 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																								     /*����-1��ʾinput parameter should only be 'vlanid' or 'limit' or 'send_limit'*/
																								     /*����-2��ʾvlan id should be 0 to 4095������-3��ʾwtp isn't existed*/
																								     /*����-4��ʾradio isn't existed*/
																									 /*����-5��ʾstation traffic limit send value is more than bss traffic limit send value*/
																									 /*����-6��ʾradio doesn't bing wlan������-7��ʾwlan isn't existed*/
																									 /*����-8��ʾcheck sta set invalid value������-9��ʾerror*/
																									 /*����-10��ʾwlanid is not legal������-11��ʾwlanid should be 1 to WLAN_NUM-1*/
																									 /*����-12��ʾwtp doesn't bing wlan������-13��ʾinput wlanid is not sta accessed wlan*/
																									 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*wlanIDΪ��ѡ������������wlanIDʱ��������NULL*/
extern int del_sta_static_info_cmd(dbus_parameter parameter, DBusConnection *connection,char *MAC,char *wlanID);
																 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																 /*����-1��ʾstation does not exist������-2��ʾerror*/
																 /*����-3��ʾwlanid is not legal������-4��ʾwlanid should be 0 to WLAN_NUM-1*/
																 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/* ����1ʱ������Free_sta_static_head( )�ͷſռ�*/
/*wlanIDΪ��ѡ������������wlanIDʱ��������NULL*/
extern int show_sta_mac_static_info_cmd(dbus_parameter parameter, DBusConnection *connection,char *MAC,struct sta_static_info **sta,char *wlanID);
																 									   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																									   /*����-1��ʾstation does not exist������-2��ʾerror*/
																									   /*����-3��ʾwlanid is not legal������-4��ʾwlanid should be 1 to WLAN_NUM-1*/
																									   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*typeΪ(add|del)*/
extern int sta_arp_set_cmd(dbus_parameter parameter, DBusConnection *connection,char *type,char *IP,char *MAC,char *if_name);
																						   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																						   /*����-1��ʾUnkown Command*/
																						   /*����-2��ʾunknown ip format*/
																						   /*����-3��ʾUnknow mac addr format*/
																						   /*����-4��ʾset sta arp failed*/
																						   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*typeΪ"black"��"white"*/
extern int ac_add_MAC_list_cmd(dbus_parameter parameter, DBusConnection *connection,char *type,char *MAC);
																		  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																		  /*����-1��ʾinput patameter should only be 'black' or 'white'*/
																		  /*����-2��ʾUnknown mac addr format������-3��ʾerror*/
																		  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*typeΪ"black"��"white"*/
extern int ac_del_MAC_list_cmd(dbus_parameter parameter, DBusConnection *connection,char *type,char *MAC);
																		 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																		 /*����-1��ʾinput patameter should only be 'black' or 'white'*/
																		 /*����-2��ʾUnknown mac addr format������-3��ʾerror*/
																		 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																		 /*����-4��ʾmac is not in the list*/

/*typeΪ"none"��"black"��"white"*/
extern int ac_use_MAC_list_cmd(dbus_parameter parameter, DBusConnection *connection,char *type);
																 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																 /*����-1��ʾinput patameter should only be 'none','black' or 'white'*/
																 /*����-2��ʾWids is enable,ac can only use black list!������-3��ʾerror*/
																 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_ac_mac_head(struct ac_mac_list_profile *info);
/*����1ʱ������Free_ac_mac_head()�ͷſռ�*/
extern int show_ac_MAC_list_cmd(dbus_parameter parameter, DBusConnection *connection,struct ac_mac_list_profile *info);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																															  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_wlan_sta_of_all_cmd(struct dcli_wlan_info *wlan);
/*����1ʱ������Free_show_wlan_sta_of_all_cmd()�ͷſռ�*/
extern int show_wlan_sta_of_all_cmd(dbus_parameter parameter, DBusConnection *connection,struct dcli_wlan_info **wlan);
																						/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																						/*����-1��ʾwlan does not exist������-2��ʾerror*/
																						/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
extern void Free_show_distinguish_information_of_all_wtp_cmd(struct dcli_wtp_info *wtp);
/*ֻҪ���ú������͵���Free_show_distinguish_information_of_all_wtp_cmd()�ͷſռ�*/
extern int show_distinguish_information_of_all_wtp_cmd(dbus_parameter parameter, DBusConnection *connection,struct dcli_wtp_info **wtp);
																											/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																											/*����-1��ʾwtp does not provide service or it maybe does not exist*/
																											/*����-2��ʾerror*/
																											/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
extern void Free_show_wapi_mib_info_of_all_wtp_cmd(struct dcli_wtp_info *wtp);
/*ֻҪ���ú������͵���Free_show_wapi_mib_info_of_all_wtp_cmd()�ͷſռ�*/
extern int show_wapi_mib_info_of_all_wtp_cmd(dbus_parameter parameter, DBusConnection *connection,struct dcli_wtp_info **wtp);
																								  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																								  /*����-1��ʾwtp does not provide service or it maybe does not exist*/
																								  /*����-2��ʾerror*/
																								  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
extern void Free_show_sta_wapi_mib_info_of_all_wtp_cmd(struct wapi_mib_wtp_info *wtp);
/*ֻҪ���ú������͵���Free_show_sta_wapi_mib_info_of_all_wtp_cmd()�ͷſռ�*/
extern int show_sta_wapi_mib_info_of_all_wtp_cmd(dbus_parameter parameter, DBusConnection *connection,struct wapi_mib_wtp_info **wtp);
																										   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																										   /*����-1��ʾwtp does not provide service or it maybe does not exist*/
																										   /*����-2��ʾerror*/
																										   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
extern void Free_show_wlan_info_allwlan_cmd(struct dcli_wlan_info *wlan);

/*ֻҪ���ú������͵���Free_show_wlan_info_allwlan_cmd()�ͷſռ�*/
extern int show_wlan_info_allwlan_cmd(dbus_parameter parameter, DBusConnection *connection,struct dcli_wlan_info **wlan);
																						  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																						  /*����-1��ʾwlan does not provide service or it maybe does not exist*/
																						  /*����-2��ʾerror*/
																						  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_all_sta_base_info_cmd(struct dcli_base_bss_info *bsshead);
/*����1ʱ������Free_show_all_sta_base_info_cmd()�ͷſռ�*/
extern int show_all_sta_base_info_cmd(dbus_parameter parameter, DBusConnection *connection,struct dcli_base_bss_info **bsshead);
																			/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																			/*����-1��ʾthere is no station������-2��ʾerror*/
																			/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

#endif

