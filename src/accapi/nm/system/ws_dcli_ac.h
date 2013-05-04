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
* ws_dcli_ac.h
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


#ifndef _DCLI_AC_H
#define _DCLI_AC_H

#include "ws_init_dbus.h"
#include "ws_dbus_list_interface.h"
#include "wcpss/wid/WID.h"
#include "wcpss/asd/asd.h"
#include "dbus/wcpss/ACDbusDef1.h"
#include "dbus/asd/ASDDbusDef1.h"
#include "ws_returncode.h"
#include "ws_public.h"

void dcli_ac_init(void);
#define DCLIAC_MAC_LEN	6
#define DCLIAC_RADIO_NUM_LEN	4
#define DCLIAC_OUI_LEN	3
#define WIDS_TYPE_LEN 16
#define WTP_WTP_IP_LEN 21
#define DCLIAC_BUF_LEN	16



struct mac_profile
{
    unsigned char       macaddr[DCLIAC_MAC_LEN];
	struct mac_profile *next;
};

typedef struct mac_profile WIDMACADDR;


typedef struct
{
    unsigned char       oui[DCLIAC_OUI_LEN];
}OUI_S;


struct acAccessWtpCount { 
    unsigned int license_type;
    unsigned int license_count[5];

    struct acAccessWtpCount *next;
};
struct acAccessBindLicCount { 
	int cur_count;
	int max_count;
	int bind_flag;

    struct acAccessBindLicCount *next;
};



extern int parse_int_ID(char* str,unsigned int* ID);
extern void CheckWIDSType(char *pattacktype, char* pframetype, unsigned char attacktype,unsigned char frametype);

extern void Free_wc_config(DCLI_AC_API_GROUP_FIVE *wirelessconfig);
/*����1ʱ������Free_wc_config()�ͷſռ�*/
extern int show_wc_config(dbus_parameter parameter, DBusConnection *connection,DCLI_AC_API_GROUP_FIVE **wirelessconfig); /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																															 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_wid_hw_version_func(dbus_parameter parameter, DBusConnection *connection,char *para);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																										 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_wid_sw_version_func(dbus_parameter parameter, DBusConnection *connection,char *para);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																										 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_wid_lev3_protocol_func(dbus_parameter parameter, DBusConnection *connection,char *protocol_type);
																				/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾThis version only surport IPv4,we will update later*/
																				/*����-2��ʾinput patameter should only be 'IPv4' or 'IPv6'������-3��ʾerror*/
																				/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_wid_auth_security_func(dbus_parameter parameter, DBusConnection *connection,char *auth_secu_type);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter should only be 'X509_CERTIFCATE' or 'PRESHARED'������-2��ʾerror*/
																													  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_wid_name_func(dbus_parameter parameter, DBusConnection *connection,char *para);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																								  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int config_wireless_max_mtu(dbus_parameter parameter, DBusConnection *connection,char*mtu);
																  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																  /*����-1��ʾinput parameter should be 500 to 1500*/
																  /*����-2��ʾerror*/
																  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int	set_wid_log_switch_cmd(dbus_parameter parameter, DBusConnection *connection,char*stat);
																    /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																	/*����-1��ʾerror*/
																	/*����-2��ʾinput patameter should only be 'ON' or 'OFF'*/
																	/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_wid_log_level_func(dbus_parameter parameter, DBusConnection *connection,char *log_lever_type);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter should only be 'info' 'debug' or 'all'������-2��ʾerror*/
																												/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*stateΪopen����close*/
/*typeΪ"default"/"dbus"/"wtp"/"mb"/"all"*/
extern int set_wid_daemonlog_debug_open_func(dbus_parameter parameter, DBusConnection *connection,char *type,char *state);
																							   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																							   /*����-1��ʾinput patameter should only be default|dbus|wtp|mb|all*/
																							   /*����-2��ʾinput patameter should only be 'open' or 'close'������-3��ʾerror*/
																							   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int  set_wid_log_size_cmd(dbus_parameter parameter, DBusConnection *connection,char *size);
																		  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/	 
																		  /*����-1��ʾinput parameter should be 1000000 to 500000000*/
																		  /*����-2��ʾerror*/
																		  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_ap_scanning_report_interval_cmd(dbus_parameter parameter, DBusConnection *connection,char*time); 
																		  		 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																				 /*����-1��ʾinput patameter should be 30 to 32767*/
																				 /*����-2��ʾerror*/
																				 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int update_ap_scanning_info_func(dbus_parameter parameter, DBusConnection *connection);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																				 					/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*state==0��ʾ"disable"��state==1��ʾ"enable"*/
extern int set_radio_resource_management(dbus_parameter parameter, DBusConnection *connection,int state);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																												/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_mac_whitelist(dbus_parameter parameter, DBusConnection *connection,char *mac);
														  /*����0��ʾʧ�ܣ�����1 ��ʾ�ɹ�*/
														  /*����-1��ʾUnknown mac addr format*/
														  /*����-2��ʾerror*/
														  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int delete_mac_whitelist(dbus_parameter parameter, DBusConnection *connection,char *mac);
														  	  /*����0��ʾʧ�ܣ�����1 ��ʾ�ɹ�*/
														 	  /*����-1��ʾUnknown mac addr format*/
															  /*����-2��ʾerror*/
															  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_wireless_control_whitelist(DCLI_AC_API_GROUP_ONE *LIST);
/*����1ʱ������Free_wireless_control_whitelist()�ͷſռ�*/
extern int show_wireless_control_whitelist(dbus_parameter parameter, DBusConnection *connection,DCLI_AC_API_GROUP_ONE  **LIST);
																								/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������2��ʾthere is no white list������-1��ʾerror*/
																								/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_mac_blacklist(dbus_parameter parameter, DBusConnection *connection,char *mac);
														  /*����0��ʾʧ�ܣ�����1 ��ʾ�ɹ�*/
												  		  /*����-1��ʾUnknown mac addr format*/
														  /*����-2��ʾerror*/
														  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int delete_mac_blacklist(dbus_parameter parameter, DBusConnection *connection,char *mac);
														  	  /*����0��ʾʧ�ܣ�����1 ��ʾ�ɹ�*/
															  /*����-1��ʾUnknown mac addr format*/
															  /*����-2��ʾerror*/
															  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_wireless_control_blacklist(DCLI_AC_API_GROUP_ONE *LIST);
/*����1ʱ������Free_wireless_control_blacklist()�ͷſռ�*/
extern int show_wireless_control_blacklist(dbus_parameter parameter, DBusConnection *connection,DCLI_AC_API_GROUP_ONE  **LIST);
																							/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������2��ʾthere is no black list������-1��ʾerror*/
																							/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_rogue_ap_head(DCLI_AC_API_GROUP_TWO *LIST);
/*����1ʱ������Free_ap_head()�ͷſռ�*/
extern int show_rogue_ap_list(dbus_parameter parameter, DBusConnection *connection,DCLI_AC_API_GROUP_TWO **LIST);  
																				  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������2��ʾthere is no rouge ap*/
                                                                                  /*����-1��ʾradio resource managment is disable please enable first*/ 
																				  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_rogue_aplist_bywtpid(DCLI_AC_API_GROUP_TWO *LIST);
/*����1ʱ������Free_rogue_aplist_bywtpid()�ͷſռ�*/
extern int show_rogue_aplist_bywtpid(dbus_parameter parameter, DBusConnection *connection,int wtp_id,DCLI_AC_API_GROUP_TWO **LIST);  
																							  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������2��ʾthere is no rouge ap*/
																							  /*����-1��ʾinput wtp id should be 1 to WTP_NUM-1������-2��ʾwtp does not exist*/
																							  /*����-3��ʾradio resource managment is disable please enable first*/
																							  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_neighbor_aplist_bywtpid(DCLI_AC_API_GROUP_TWO *LIST);
/*����1ʱ������Free_neighbor_aplist_bywtpid()�ͷſռ�*/
extern int show_neighbor_aplist_bywtpid(dbus_parameter parameter, DBusConnection *connection,int wtp_id,DCLI_AC_API_GROUP_TWO **LIST);
																									   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������2��ʾthere is no neighbor ap*/
																								       /*����-1��ʾinput wtp id should be 1 to WTP_NUM-1������-2��ʾwtp does not exist*/
																									   /*����-3��ʾradio resource managment is disable please enable first*/	
																									   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_neighbor_ap_list_bywtpid2_cmd(DCLI_AC_API_GROUP_TWO *dcli_list);				
/*����1ʱ������Free_show_neighbor_ap_list_bywtpid2_cmd()�ͷſռ�*/
extern int show_neighbor_ap_list_bywtpid2_cmd(dbus_parameter parameter, DBusConnection *connection,char *wtp_id,DCLI_AC_API_GROUP_TWO **dcli_list);
																											   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
				                                                                                               /*����-1��ʾinput wtp id should be 1 to WTP_NUM-1*/
				                                                                                               /*����-2��ʾwtp does not existt*/
				                                                                                               /*����-3��ʾradio resource managment is disable please enable first*/
																											   /*����-4��ʾthere is no neighbor ap*/
																											   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int dynamic_channel_selection_cmd(dbus_parameter parameter, DBusConnection *connection,char*state);
																			/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
		                                                                    /*����-1��ʾinput patameter only with 'enable' or 'disable'*/
		                                                                    /*����-2��ʾyou should enable radio resource management first*/
																			/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*wtp_id��radio_G_idֻ����дһ�����ֱ��ʾ��wtp�ڵ��radio�ڵ�������*/
/*wtp_idΪ0ʱ����ʾȫ������*/
extern int set_system_country_code_func(dbus_parameter parameter, DBusConnection *connection,int wtp_id,int radio_G_id,char *Country_code);
																					/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput country code should be capital letters*/
																					/*����-2��ʾinput country code error������-3��ʾsystem country code is already Country_code, no need to change*/
																					/*����-4��ʾsystem country code error������-5��ʾWTP ID�Ƿ�������-6��ʾRadio ID�Ƿ�*/
																					/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																					
extern int undo_system_country_code_func(dbus_parameter parameter, DBusConnection *connection);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾsystem country code is default, no need to change������-2��ʾsystem country code error*/
																										/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																					
/*stateΪopen��close*/
/*typeΪ"default"/"dbus"/"80211"/"1x"/"wpa"/"wapi"/"leave"/"all"*/
extern int set_asd_daemonlog_debug_open_func(dbus_parameter parameter, DBusConnection *connection,char *type,char *state);
																							   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																					 		   /*����-1��ʾinput patameter should only be default|dbus|80211|1x|wpa|wapi|leave|all*/
																							   /*����-2��ʾinput patameter should only be 'open' or 'close'������-3��ʾerror*/
																							   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_hostapd_logger_printflag_open_func(dbus_parameter parameter, DBusConnection *connection,char *state);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter should only be 'open' or 'close'������-2��ʾerror*/
																							   							   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																							   
extern int dynamic_power_selection_cmd(dbus_parameter parameter, DBusConnection *connection,char*state);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾyou should enable radio resource management first*/
																												/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_transmit_power_control_scope(dbus_parameter parameter, DBusConnection *connection,char *state); /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																													/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_wirelesscontrol_auto_ap_switch(dbus_parameter parameter, DBusConnection *connection,char *stat);	
																				/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																		        /*����-1��ʾinput patameter should only be 'enable' or 'disable'*/
																				/*����-2��ʾauto_ap_login interface has not set yet*/
																				/*����-3��ʾerror*/
																				/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_wirelesscontrol_auto_ap_binding_l3_interface(dbus_parameter parameter, DBusConnection *connection,char *state,char* inter); 
																											 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinterface name is too long,should be no more than 15*/
																											 /*����-2��ʾinput patameter only with 'uplink'or 'downlink'������-3��ʾauto ap login switch is enable,you should disable it first*/
																											 /*����-4��ʾinterface argv[1] error, no index or interface down������-5��ʾinterface argv[1] is down������-6��ʾinterface argv[1] is no flags*/
																											 /*����-7��ʾinterface argv[1] is no index������-8��ʾinterface argv[1] error������-9��ʾinterface has be binded in other hansi*/
																											 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																											 
extern int set_wirelesscontrol_auto_ap_binding_wlan(dbus_parameter parameter, DBusConnection *connection,char*wlan_id,char *ifname);	
																										/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinterface name is too long,should be no more than 15*/
																										/*����-2��ʾwlanid should be 1 to WLAN_NUM-1������-3��ʾwlan not exist������-4��ʾwlan has not bind interface*/
																										/*����-5��ʾ interface not in the auto ap login interface������-6��ʾauto_ap_login interface has not set yet*/
																										/*����-7��ʾauto_ap_login interface argv[1] wlan num is already L_BSS_NUM*/
																										/*����-8��ʾauto ap login switch is enable,you should disable it first*/
																										/*����-9��ʾis no local interface, permission denial������-10��ʾinterface error, no index or interface down*/
																										/*����-11��ʾerror*/
																										/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																										
extern int del_wirelesscontrol_auto_ap_binding_wlan_func(dbus_parameter parameter, DBusConnection *connection,char *wlan_id,char *ifname); 
																												/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinterface name is too long,should be no more than 15*/
																												/*����-2��ʾwlanid should be 1 to WLAN_NUM-1������-3��ʾwlan not exist������-4��ʾwlan has not bind interface*/
																												/*����-5��ʾinterface argv[1] not in the auto ap login interface������-6��ʾauto_ap_login interface has not set yet*/
																												/*����-7��ʾauto_ap_login interface argv[1] wlan num is 0������-8��ʾauto ap login switch is enable,you should disable it first*/
																												/*����-9��ʾinput interface dosen't exist!������-10��ʾerror*/
																												/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																												
extern int set_wirelesscontrol_auto_ap_save_config_switch(dbus_parameter parameter, DBusConnection *connection,char *stat);
																							  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																							  /*����-1��ʾinput patameter should only be 'enable' or 'disable'*/
																							  /*����-2��ʾauto ap login switch is enable,you should disable it first*/
																							  /*����-3��ʾerror*/
																							  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_auto_ap_config(DCLI_AC_API_GROUP_FIVE *auto_ap_login);
/*����1ʱ������Free_auto_ap_config()�ͷſռ�*/
extern int show_auto_ap_config(dbus_parameter parameter, DBusConnection *connection,DCLI_AC_API_GROUP_FIVE **auto_ap_login);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																																/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_ap_timestamp(dbus_parameter parameter, DBusConnection *connection);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																					  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_monitor_time(dbus_parameter parameter, DBusConnection *connection,char * mon_time);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format������-2��ʾerror*/
																					  				 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_sample_time(dbus_parameter parameter, DBusConnection *connection,char * sample_time);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format������-2��ʾerror*/
																									   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_sample_info(DCLI_AC_API_GROUP_FIVE *sample_info);
/*����1ʱ������Free_qos_head()�ͷſռ�*/
extern int show_sample_info(dbus_parameter parameter, DBusConnection *connection,DCLI_AC_API_GROUP_FIVE **sample_info);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																															/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_monitor_enable(dbus_parameter parameter, DBusConnection *connection,char * able);
															  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
															  /*����-1��ʾinput patameter only with 'enable' or 'disable'*/
															  /*����-2��ʾerror*/
															  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_sample_enable(dbus_parameter parameter, DBusConnection *connection,char * able);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																								  /*����-1��ʾinput patameter only with 'enable' or 'disable'*/
																								  /*����-2��ʾerror*/
																								  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_ap_txpower_control(DCLI_AC_API_GROUP_FIVE *tx_control);
/*����1ʱ������Free_ap_txpower_control()�ͷſռ�*/
extern int show_ap_txpower_control(dbus_parameter parameter, DBusConnection *connection,DCLI_AC_API_GROUP_FIVE **tx_control);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																																   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_receiver_signal_level(dbus_parameter parameter, DBusConnection *connection,int level_num);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																											/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_receiver_signal_level(DCLI_AC_API_GROUP_FIVE *receiver_sig_lev);
/*����1ʱ������Free_receiver_signal_level()�ͷſռ�*/
extern int show_receiver_signal_level(dbus_parameter parameter, DBusConnection *connection,DCLI_AC_API_GROUP_FIVE **receiver_sig_lev);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																																		  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int clear_auto_ap_config_func(dbus_parameter parameter, DBusConnection *connection);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																							   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*able==1��ʾ"enable",able == 0��ʾ"disable"*/
extern int set_ap_statistics(dbus_parameter parameter, DBusConnection *connection,int able);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																							 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_attack_mac_list_head(DCLI_AC_API_GROUP_ONE  *LIST);
/*����1ʱ������Free_attack_mac_list_head()�ͷſռ�*/
extern int show_attack_mac_list(dbus_parameter parameter, DBusConnection *connection,DCLI_AC_API_GROUP_ONE  **LIST);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾthere is no attack mac list������-2��ʾerror*/
																														/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_legal_essid_list_head(DCLI_AC_API_GROUP_ONE *LIST);
/*����1ʱ������Free_legal_essid_list_head()�ͷſռ�*/
extern int show_legal_essid_list_cmd(dbus_parameter parameter, DBusConnection *connection,DCLI_AC_API_GROUP_ONE **LIST);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾthere is no legal essid list������-2��ʾerror*/
																															/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_manufacturer_oui_list_head(DCLI_AC_API_GROUP_ONE *LIST);
/*����1ʱ������Free_manufacturer_oui_list_head()�ͷſռ�*/
extern int show_manufacturer_oui_list(dbus_parameter parameter, DBusConnection *connection,DCLI_AC_API_GROUP_ONE **LIST);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾthere is no legal manufacturer list������-2��ʾerror*/
																															  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int add_legal_manufacturer_func(dbus_parameter parameter, DBusConnection *connection,char *OUI);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾUnknown OUI format������-2��ʾerror*/
																											/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int add_legal_essid_func(dbus_parameter parameter, DBusConnection *connection,char *ESSID);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																									  /*����-2��ʾessid is too long,out of the limit of 32*/
																									  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int add_attack_ap_mac_func(dbus_parameter parameter, DBusConnection *connection,char *MAC);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾUnknown mac addr format������-2��ʾerror*/
																									  	/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int del_legal_manufacturer_func(dbus_parameter parameter, DBusConnection *connection,char *OUI);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾUnknown OUI format������-2��ʾerror*/
																											/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int del_legal_essid_func(dbus_parameter parameter, DBusConnection *connection,char *ESSID);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																									/*����-2��ʾessid is too long,out of the limit of 32*/
																									/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int del_attack_ap_mac_func(dbus_parameter parameter, DBusConnection *connection,char *MAC);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾUnknown mac addr format������-2��ʾerror*/
																										/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_ipfwd_func(char *state);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'enable' or 'disable'������-2��ʾerror*/

extern void Free_ApMode_head(DCLI_AC_API_GROUP_FOUR *conf_info);
/*����1ʱ������Free_ApMode_head()�ͷſռ�*/
extern int show_model_list_cmd(dbus_parameter parameter, DBusConnection *connection,DCLI_AC_API_GROUP_FOUR **conf_info);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																															/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_model_cmd(DCLI_AC_API_GROUP_FOUR *modelinfo);
/*����1ʱ������Free_model_cmd()�ͷſռ�*/
extern int show_model_cmd(dbus_parameter parameter, DBusConnection *connection,char *mode,DCLI_AC_API_GROUP_FOUR **modelinfo);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾthis model doesn't exist*/
																																	/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_model_cmd(dbus_parameter parameter, DBusConnection *connection,char *mode,char *new_mode);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾnew model is not configuration please change other name������-2��ʾthis model doesn't exist*/
																											/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_ap_model_code(DCLI_AC_API_GROUP_FOUR *codeinfo);
/*����1ʱ������Free_ap_model_code()�ͷſռ�*/
extern int show_ap_model_code_func(dbus_parameter parameter, DBusConnection *connection, char *mode,DCLI_AC_API_GROUP_FOUR **codeinfo);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾthis model doesn't exist*/
																																				/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_rogue_ap_list_v1_head(DCLI_AC_API_GROUP_TWO *LIST);
/*����1ʱ������Free_rogue_ap_list_v1_head()�ͷſռ�*/
extern int show_rogue_ap_list_v1_func(dbus_parameter parameter, DBusConnection *connection,DCLI_AC_API_GROUP_TWO **LIST);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾradio resource managment is disable please enable first������-2��ʾgood luck there is no rouge ap*/
																															   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_txpower_threshold_cmd(dbus_parameter parameter, DBusConnection *connection,char *value);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter should be 20 to 35������-2��ʾerror*/
																											/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_coverage_threshold_cmd(dbus_parameter parameter, DBusConnection *connection,char *value);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter should be 5 to 15������-2��ʾerror*/
																											  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_neighbor_rssi_info_bywtpid(DCLI_AC_API_GROUP_THREE *RSSI);
/*����1ʱ������Free_neighbor_rssi_info_bywtpid()�ͷſռ�*/
extern int show_neighbor_rssi_info_bywtpid_cmd(dbus_parameter parameter, DBusConnection *connection,int wtp_id,DCLI_AC_API_GROUP_THREE **RSSI); 
																				/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwtp id should be 1 to WTP_NUM-1*/
																				/*����-2��ʾwtp does not exist������-3��ʾradio resource managment is disable please enable first*/
																				/*����-4��ʾtransmit power control is disable please enable first������-5��ʾthere is no neighbor ap*/
																				/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																									
extern int set_ap_cm_threshold_func(dbus_parameter parameter, DBusConnection *connection,char *thr_type,char *thr_value); 
																							/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'cpu','temperature'or 'memoryuse'*/
																							/*����-2��ʾunknown id format������-3��ʾap cpu threshold parameters error������-4��ʾap memory use threshold parameters error*/
																							/*����-5��ʾwtp id does not run������-6��ʾerror*/
																							/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_ap_threshold(DCLI_AC_API_GROUP_FIVE *ap_threshold);
/*����1ʱ������Free_ap_threshold()�ͷſռ�*/																			
extern int show_ap_threshold_func(dbus_parameter parameter, DBusConnection *connection,DCLI_AC_API_GROUP_FIVE **ap_threshold);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																																	/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_ap_rrm_config(DCLI_AC_API_GROUP_FIVE *resource_mg);
/*����1ʱ������Free_ap_rrm_config()�ͷſռ�*/
extern int show_ap_rrm_config_func(dbus_parameter parameter, DBusConnection *connection,DCLI_AC_API_GROUP_FIVE **resource_mg);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ� ������-1��ʾerror*/
																																	/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int show_ipfwd_func(int *ipfwd_state);	/*ipfwd_state==0��ʾdisable��ipfwd_state==1��ʾenable*/
													/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunexpected flow-based-forwarding state*/

extern void Free_wids_device_head(DCLI_AC_API_GROUP_TWO *LIST);
/*����1ʱ������Free_wids_device_head()�ͷſռ�*/
extern int show_wids_device_list_cmd_func(dbus_parameter parameter, DBusConnection *connection,DCLI_AC_API_GROUP_TWO **LIST);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾthere is no wids device������-2��ʾerror*/
																																	/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_wids_device_list_bywtpid_head(DCLI_AC_API_GROUP_TWO *LIST);
/*����1ʱ������Free_wids_device_list_bywtpid_head()�ͷſռ�*/
extern int show_wids_device_list_bywtpid_cmd_func(dbus_parameter parameter, DBusConnection *connection,char *WtpID,DCLI_AC_API_GROUP_TWO **LIST);
																							 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput wtp id should be 1 to WTP_NUM-1*/
																					 		 /*����-2��ʾwtp does not exist������-3��ʾthere is no wids device������-4��ʾerror*/
																							 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int clear_wids_device_list_cmd_func(dbus_parameter parameter, DBusConnection *connection);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																									  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																													 
extern int clear_wids_device_list_bywtpid_cmd_func(dbus_parameter parameter, DBusConnection *connection,char *WtpID);
																						/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput wtp id should be 1 to WTP_NUM-1*/
																						/*����-2��ʾwtp does not exist������-3��ʾerror*/
																						/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_wids_statistics_list_bywtpid_head(DCLI_AC_API_GROUP_THREE *widsstatis);
/*����1ʱ������Free_wids_statistics_list_bywtpid_head()�ͷſռ�*/																		
extern int show_wids_statistics_list_bywtpid_cmd_func(dbus_parameter parameter, DBusConnection *connection,char *WtpID,DCLI_AC_API_GROUP_THREE **widsstatis);
																										/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput wtp id should be 1 to WTP_NUM-1*/
																									    /*����-2��ʾwtp does not exist������-3��ʾerror*/
																										/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_wids_statistics_list_head(DCLI_AC_API_GROUP_THREE *widsstatis);
/*����1ʱ������Free_wids_statistics_list_head()�ͷſռ�*/																																
extern int show_wids_statistics_list_cmd_func(dbus_parameter parameter, DBusConnection *connection,DCLI_AC_API_GROUP_THREE **widsstatis);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																																			   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int clear_wids_statistics_list_bywtpid_cmd_func(dbus_parameter parameter, DBusConnection *connection,char *WtpID);
																						  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput wtp id should be 1 to WTP_NUM-1*/
																						  /*����-2��ʾwtp does not exist������-3��ʾerror*/
																						  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																						  
extern int clear_wids_statistics_list_cmd_func(dbus_parameter parameter, DBusConnection *connection);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																						  				 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																						  
extern int set_neighbordead_interval_cmd_func(dbus_parameter parameter, DBusConnection *connection,char *value);
																				  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																				  /*����-1��ʾinput patameter should be 20 to 2000������-2��ʾerror*/
																				  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_neighbordead_interval(DCLI_AC_API_GROUP_FIVE *interval);
/*����1ʱ������Free_neighbordead_interval()�ͷſռ�*/
extern int show_neighbordead_interval_cmd_func(dbus_parameter parameter, DBusConnection *connection,DCLI_AC_API_GROUP_FIVE **interval);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																																				/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int update_bak_ac_config_func(dbus_parameter parameter, DBusConnection *connection);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																								/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int synchronize_wsm_table_func(dbus_parameter parameter, DBusConnection *connection);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																								  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int notice_vrrp_state_func(dbus_parameter parameter, DBusConnection *connection);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																						   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*para�ķ�Χ��1-5����λ��second*/
extern int set_wtp_wids_interval_cmd_func(dbus_parameter parameter, DBusConnection *connection,char *para);
																			/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																			/*����-2��ʾwtp wids interval error,should be 1 to 5 second*/
																			/*����-3��ʾwids switch is enable������-4��ʾerror*/
																			/*����-5��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																			/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*policy_typeΪ"probe"��"other"*/
/*para�ķ�Χ��1-100*/
extern int set_wtp_wids_threshold_cmd_func(dbus_parameter parameter, DBusConnection *connection,char *policy_type,char *para);
																								 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter should only be 'probe' or 'other'*/
																								 /*����-2��ʾunknown id format������-3��ʾwtp wids threshold error,should be 1 to 100*/
																								 /*����-4��ʾwids switch is enable������-5��ʾerror������-6��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																								 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*para�ķ�Χ��1-36000*/
extern int set_wtp_wids_lasttime_cmd_func(dbus_parameter parameter, DBusConnection *connection,char *para);
																			 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																			 /*����-2��ʾwtp wids lasttime in black error,should be 1 to 36000*/
																			 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*level�ķ�Χ��0-25*/
extern int set_wid_trap_open_func(dbus_parameter parameter, DBusConnection *connection,char *level);/*����1��ʾ�ɹ�������0��ʾʧ�ܣ�����-1��ʾerror*/
																			 							 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_rogue_ap_trap_threshold(DCLI_AC_API_GROUP_FIVE *rogue_trap);
/*����1ʱ������Free_rogue_ap_trap_threshold()�ͷſռ�*/															 
extern int show_rogue_ap_trap_threshold_func(dbus_parameter parameter, DBusConnection *connection,DCLI_AC_API_GROUP_FIVE **rogue_trap);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																																			   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*value�ķ�Χ��1-200*/
extern int set_rogue_ap_trap_threshold_func(dbus_parameter parameter, DBusConnection *connection,char *value);
																			   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter should be 1 to 200*/
																			   /*����-2��ʾradio resource managment is disable please enable first������-3��ʾerror*/
																			   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																			   
/*typeΪ"add"��"del"*/
extern int set_wirelesscontrol_auto_ap_binding_l3_interface_new_cmd(dbus_parameter parameter, DBusConnection *connection,char *type,char *ifname);
																			   										   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																													   /*����-1��ʾinterface name is too long,should be no more than 15*/
																													   /*����-2��ʾinput patameter only with 'add'or 'del'*/
																													   /*����-3��ʾauto ap login switch is enable,you should disable it first*/
																													   /*����-4��ʾinterface argv[1] error, no index or interface down*/
																													   /*����-5��ʾinterface argv[1] is down������-6��ʾinterface argv[1] is no flags*/
																													   /*����-7��ʾinterface argv[1] is no index*/
																													   /*����-8��ʾis no local interface, permission denial*/
																													   /*����-9��ʾinterface argv[1] error*/
																													   /*����-10��ʾinterface has not been added or has already been deleted*/
																													   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*update_time�ķ�Χ��5-3600*/																													   
extern int set_ap_update_img_timer_cmd(dbus_parameter parameter, DBusConnection *connection,char *update_time);
																				 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																				 /*����-1��ʾunknown id format������-2��ʾerror*/
																				 /*����-3��ʾinput time should be 5-3600*/
																				 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_ap_update_img_timer(DCLI_AC_API_GROUP_FIVE *up_timer);
/*����1ʱ������Free_ap_update_img_timer()�ͷſռ�*/
extern int show_ap_update_img_timer_cmd(dbus_parameter parameter, DBusConnection *connection,DCLI_AC_API_GROUP_FIVE **up_timer);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																																		 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int update_wtpcompatible_cmd(dbus_parameter parameter, DBusConnection *connection);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																								/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_ap_update_fail_count_cmd(dbus_parameter parameter, DBusConnection *connection,char *update_fail_count);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																															/*����-1��ʾunknown id format*/
																															/*����-2��ʾerror*/
																															/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_ap_update_fail_count(DCLI_AC_API_GROUP_FIVE *update_fail);
/*����1ʱ������Free_ap_update_fail_count()�ͷſռ�*/
extern int show_ap_update_fail_count_cmd(dbus_parameter parameter, DBusConnection *connection,DCLI_AC_API_GROUP_FIVE **update_fail);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																																			/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_wid_watch_dog_cmd(dbus_parameter parameter, DBusConnection *connection,char *state);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																										  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_ap_network_bywtpid(DCLI_AC_API_GROUP_THREE *network);
/*����1ʱ������Free_ap_network_bywtpid()�ͷſռ�*/
extern int show_ap_network_bywtpid_cmd(dbus_parameter parameter, DBusConnection *connection,char *wtp_id,DCLI_AC_API_GROUP_THREE **network);
																												/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																											    /*����-1��ʾinput wtp id should be 1 to WTP_NUM-1*/
																											    /*����-2��ʾap have not ip information*/
																											    /*����-3��ʾwtp id no exist*/
																												/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																												
/*method_typeΪ"number","flow"��"disable"*/
extern int ac_load_balance_cmd(dbus_parameter parameter, DBusConnection *connection,char *method_type);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾoperation fail������-2��ʾerror*/
																											/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_ac_balance(DCLI_AC_API_GROUP_FIVE *balance);
/*����1ʱ������Free_ac_balance()�ͷſռ�*/
extern int show_ac_balance_cmd(dbus_parameter parameter, DBusConnection *connection,DCLI_AC_API_GROUP_FIVE **balance);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																															/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																   
/*stateΪ"enable"��"disable"*/
extern int set_ap_hotreboot_cmd(dbus_parameter parameter, DBusConnection *connection,char *state);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'enable' or 'disable'������-2��ʾerror*/
																										/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*stateΪ"enable"��"disable"*/
extern int set_ap_access_through_nat_cmd(dbus_parameter parameter, DBusConnection *connection,char *state);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'enable' or 'disable'������-2��ʾerror*/
																												   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*policy_typeΪ"no"��"forbid"*/
extern int set_wtp_wids_policy_cmd(dbus_parameter parameter, DBusConnection *connection,char *policy_type);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwids switch is enable������-2��ʾerror*/
																												 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int add_wids_mac_cmd(dbus_parameter parameter, DBusConnection *connection,char *MAC);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾUnknown mac addr format������-2��ʾerror*/
																								  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int del_wids_mac_cmd(dbus_parameter parameter, DBusConnection *connection,char *MAC);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾUnknown mac addr format������-2��ʾerror*/
																								 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*����1��mac_num>0ʱ������Free_maclist_head()�ͷſռ�*/
extern int show_wids_mac_list_cmd(dbus_parameter parameter, DBusConnection *connection,WIDMACADDR *mac_head,int *mac_num);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾthere is no wids ignore mac list������-2��ʾerror*/
																								 								 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*stateΪ"enable"��"disable"*/
extern int set_ap_countermeasures_cmd(dbus_parameter parameter, DBusConnection *connection,char *state);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'enable' or 'disable'������-2��ʾerror*/
																												/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*modeΪ"ap"��"adhoc"��"all"*/
extern int set_ap_countermeasures_mode_cmd(dbus_parameter parameter, DBusConnection *connection,char *mode);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput patameter only with 'enable' or 'disable'������-2��ʾerror*/
																													  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int dynamic_channel_selection_range_cmd(dbus_parameter parameter, DBusConnection *connection,int argc,char **argv);
																							  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																							  /*����-1��ʾrange of the channel num is 2-4.������-2��ʾpatameter format error.*/
																							  /*����-3��ʾyou should enable radio resource management first*/
																							  /*����-4��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																							  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_vrrp_state(DCLI_AC_API_GROUP_FIVE *vrrp_state);
/*����1ʱ������Free_vrrp_state()�ͷſռ�*/
extern int show_vrrp_state_cmd(dbus_parameter parameter, DBusConnection *connection,DCLI_AC_API_GROUP_FIVE **vrrp_state);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																															  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_vrrp_sock(DCLI_AC_API_GROUP_FOUR *baksock);
/*����1ʱ������Free_vrrp_sock()�ͷſռ�*/
extern int show_vrrp_sock_cmd(dbus_parameter parameter, DBusConnection *connection,DCLI_AC_API_GROUP_FOUR **baksock);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																														   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*typeΪ"number"��"flow"*/
/*number�����ķ�Χ��1-10*/
/*flow�����ķ�Χ��1-30*/
extern int ac_balance_parameter_cmd(dbus_parameter parameter, DBusConnection *connection,char *type,char *para);
																				  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown input*/
																				  /*����-2��ʾbalance parameter should be 1 to 10������-3��ʾbalance parameter should be 1 to 30*/
																				  /*����-4��ʾoperation fail������-5��ʾerror*/
																				  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*trap_typeΪ"ap_run_quit","ap_cpu_threshold","ap_mem_threshold","ap_update_fail","rrm_change","rogue_ap_threshold",
				  "rogue_terminal_threshold","rogue_ap","rogue_device","wireless_interface_down","channel_count_minor"��"channel_change"*/
/*stateΪ"enable"��"disable"*/
extern int set_wid_trap_switch_able_cmd(dbus_parameter parameter, DBusConnection *connection,char *trap_type,char *state);
																				  		   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾthe first input patameter error*/
																						   /*����-2��ʾinput patameter should only be 'enable' or 'disable'������-3��ʾerror*/
																						   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int show_wid_trap_switch_info_cmd(dbus_parameter parameter, DBusConnection *connection,WID_TRAP_SWITCH **INFO);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/

extern int modify_legal_essid_cmd(dbus_parameter parameter, DBusConnection *connection,char *old_essid,char *new_essid);
																		/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																		/*����-1��ʾThe essid list is null,there is no essid*/
																		/*����-2��ʾThe essid input is not exit*/
																		/*����-3��ʾerror*/
																		/*����-4��ʾfirst essid is too long,out of the limit of 32*/
																		/*����-5��ʾsecond essid is too long,out of the limit of 32*/
																		/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_ac_all_ap_extension_information_enable_cmd(dbus_parameter parameter, DBusConnection *connection,char *state);
																								/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																								/*����-1��ʾinput patameter only with 'enable' or 'disable'*/
																								/*����-2��ʾerror*/
																								/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*stateΪ"enable"��"disable"*/
extern int set_wid_mac_whitelist_cmd(dbus_parameter parameter, DBusConnection *connection,char *state);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																											/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*stateΪ"enable"��"disable"*/
extern int set_wid_essid_whitelist_cmd(dbus_parameter parameter, DBusConnection *connection,char *state);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																											  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int change_wirelesscontrol_whitelist_cmd(dbus_parameter parameter, DBusConnection *connection,char *old_mac,char *new_mac);
																								    /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾUnknown mac addr format*/
																									/*����-2��ʾsrc mac isn't exist������-3��ʾdst mac already in white list������-4��ʾerror*/
																									/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int change_wirelesscontrol_blacklist_cmd(dbus_parameter parameter, DBusConnection *connection,char *old_mac,char *new_mac);
																									/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾUnknown mac addr format*/
																									/*����-2��ʾsrc mac isn't exist������-3��ʾdst mac already in white list������-4��ʾerror*/
																									/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

	

/*config�ڵ���wtp_idΪ0*/
/*value�ķ�Χ��1-32767*/
extern int set_ap_statistics_inter_cmd(dbus_parameter parameter, DBusConnection *connection,int wtp_id,char *value); 
																				   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																				   /*����-1��ʾinput interval should be 1 to 32767*/
																				   /*����-2��ʾinvalid wtpid������-3��ʾerror*/
																				   /*����-4��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																				   /*����-5��ʾunknown id format*/
																				   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_neighbor_ap_list_cmd(struct allwtp_neighborap *neighborap);
/*ֻҪ���ú������͵���Free_show_neighbor_ap_list_cmd()�ͷſռ�*/
extern int show_neighbor_ap_list_cmd(dbus_parameter parameter, DBusConnection *connection,struct allwtp_neighborap **neighborap);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																																		/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_show_wids_device_of_all_cmd(DCLI_AC_API_GROUP_TWO *dcli_list);
/*����1ʱ������Free_show_wids_device_of_all_cmd()�ͷſռ�*/
extern int show_wids_device_of_all_cmd(dbus_parameter parameter, DBusConnection *connection,DCLI_AC_API_GROUP_TWO **dcli_list,unsigned int *last_time);
																														 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																														 /*����-1��ʾgood luck there is no wids device*/
																														 /*����-2��ʾerror*/
																														 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/


extern void Free_show_ac_access_wtp_vendor_count(struct acAccessWtpCount *count_head, struct acAccessBindLicCount *head);
/*����1ʱ������Free_show_ac_access_wtp_vendor_count()�ͷſռ�*/
extern int show_ac_access_wtp_vendor_count(dbus_parameter parameter, DBusConnection *connection, struct acAccessWtpCount **count_head, struct acAccessBindLicCount **bind_lic_head);





/*config�ڵ���wtp_idΪ0*/
/*scan_modeΪ"2--monitor"��"3--halfmonitor"��"1--disable"*/
extern int set_wids_monitor_mode_cmd(dbus_parameter parameter, DBusConnection *connection, unsigned int wtp_id, unsigned int scan_mode);
																																/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinvalid input*/
																			                                                    /*����-2��ʾinvalid wtp id������-3��ʾerror*/


#endif

