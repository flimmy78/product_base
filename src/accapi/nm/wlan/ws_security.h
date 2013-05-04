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
* ws_security.h
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

#ifndef _WS_SECURITY_H
#define _WS_SECURITY_H

#include <unistd.h>
#include <syslog.h>
#include "ws_init_dbus.h"
#include "ws_dbus_list_interface.h"
#include "wcpss/wid/WID.h"
#include "wcpss/asd/asd.h"
#include "dbus/wcpss/ACDbusDef1.h"
#include "dbus/asd/ASDDbusDef1.h"

#define SLOT_LLEGAL(slot_no)     ((slot_no)>0&&(slot_no)<=4)//xm 08/08/27
#define PORT_LLEGAL(port_no)     ((port_no)>0&&(port_no)<=24)//xm 08/08/27


#define SLOT_PORT_SPLIT_SLASH	'/'	//xm 08/08/27
#define SLOT_PORT_SPLIT_DASH 	'-'	//xm 08/08/27
#define SLOT_PORT_SPLIT_COMMA 	','	//xm 08/08/27

#define MAX_NUM_OF_VLANID 256		//xm 08/08/27

#define MAXINTERFACES 48			//ht 08.12.02
#define MAXNAMSIZ 16				//ht
#define MAX_VLAN_NUM 4094			//ht add ->sz


typedef struct  {
       unsigned char slot;
       unsigned char port;
}SLOT_PORT_S;					//xm 08/08/27

typedef enum {
		TEST_SLOT_STATE=1,
		TEST_SPLIT_STATE,
		TEST_PORT_STATE,
		TEST_COMMA_STATE,
		TEST_END_STATE,
		TEST_FAILURE_STATE,
		TEST_SUCESS_STATE
}PARSE_PORT_STATE;		//xm 08/08/27  

/////////////////////////////////////////////////
//sz20080825 

typedef struct  {
       unsigned int vlanid;
       unsigned int stat;
	   
}VLAN_ENABLE;

typedef enum{
	check_vlanid_state=0,
	check_comma_state,
	check_fail_state,
	check_end_state,
	check_success_state
}vlan_list_state;


struct asd_trap_state_info
{
	unsigned char type[8];
};


extern int parse_security_char_ID(char* str,unsigned char* ID);
extern int RemoveListRepId(int list[],int num);
extern int parse_vlan_list(char* ptr,int* count,int vlanId[]);
extern int parse_port(char* ptr,/*int* slot,int* port*/SLOT_PORT_VLAN_SECURITY* sp);

extern int _parse_port(char* ptr,/*int* slot,int* port*/SLOT_PORT_VLAN_ENABLE* sp);



extern int _parse_port_list(char* ptr,int* count,SLOT_PORT_S spL[]);  //xm 08/08/27


extern  void CheckSecurityType(char *type, unsigned int SecurityType);

extern 	void CheckEncryptionType(char *type, unsigned int EncryptionType);
extern  void CheckRekeyMethod(char *type, unsigned char SecurityType);

extern  void Free_security_head(struct dcli_security *sec);
/*����1ʱ������Free_security_head()�ͷſռ�*/
extern 	int show_security_list(dbus_parameter parameter, DBusConnection *connection,struct dcli_security **sec,int *security_num);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																																	/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_security_one(struct dcli_security *sec);
/*����1ʱ������Free_security_one()�ͷſռ�*/
extern 	int show_security_one(dbus_parameter parameter, DBusConnection *connection,int id,struct dcli_security **sec);
																					 /*����0��ʾʧ�ܣ�,����1��ʾ�ɹ�������-1��ʾsecurity ID�Ƿ�*/
																					 /*����-2��ʾSecurity id is not exited������-3��ʾerror*/
																					 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																					 
extern  int create_security(dbus_parameter parameter, DBusConnection *connection,char * id, char *sec_name);	/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾsecurity id�Ƿ�������-2��ʾsecurity ID existed������-3��ʾerror*/
																					 							/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																					 
/*sec_type�ķ�Χ��"open","shared","802.1x","WPA_P","WPA2_P","WPA_E","WPA2_E","MD5","WAPI_PSK"��"WAPI_AUTH"*/
extern	int security_type(dbus_parameter parameter, DBusConnection *connection,int id,char *sec_type);  
																	 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown security type������-2��ʾsecurity id not exist*/
				                                                     /*����-3��ʾThis Security Profile be used by some Wlans,please disable these Wlans first������-4��ʾerror������-5��ʾSecurity ID�Ƿ�*/
																	 /*����-6��ʾThe radius heart test is on,turn it off first!*/
																	 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																	 
extern	int encryption_type(dbus_parameter parameter, DBusConnection *connection,int id,char *enc_type);  
																	 	/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown encryption type*/
				                                                        /*����-2��ʾencryption type dosen't match with security type������-3��ʾsecurity id not exist*/
				                                                        /*����-4��ʾThis Security Profile be used by some Wlans,please disable these Wlans first*/
				                                                        /*����-5��ʾerror������-6��ʾSecurity ID�Ƿ�������-7��ʾThe radius heart test is on,turn it off first!*/
																		/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*service_type==1��ʾenable��service_type==0��ʾdisable*/																		
extern  int extensible_authentication(dbus_parameter parameter, DBusConnection *connection,int id,int service_type);	 
																					 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																					 /*����-1��ʾencryption type dosen't match with security type������-2��ʾsecurity id not exist*/
																					 /*����-3��ʾThis Security Profile be used by some Wlans,please disable these Wlans first*/
																					 /*����-4��ʾerror������-5��ʾSecurity ID�Ƿ�*/
																					 /*����-6��ʾextensible auth is supported open or shared*/
																					 /*����-7��ʾThe radius heart test is on,turn it off first!*/
																					 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*service_type==1��ʾwired��service_type==0��ʾwireless*/																					 
extern  int radius_server(dbus_parameter parameter, DBusConnection *connection,int id,int service_type); 
																	  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																 	  /*����-1��ʾencryption type dosen't match with security type������-2��ʾsecurity id not exist*/
																	  /*����-3��ʾThis Security Profile be used by some Wlans,please disable these Wlans first*/
																	  /*����-4��ʾerror������-5��ʾSecurity ID�Ƿ�*/
																	  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*inputype�ķ�Χ��"ascii"��"hex"*/																		
extern 	int security_key(dbus_parameter parameter, DBusConnection *connection,int id,char *SecurityKey,char*inputype); 	
																	  				/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾsecurity not exist*/
							                                                        /*����-2��ʾsecurity key not permit set������-3��ʾkey length error������-4��ʾKey has been set up*/
							                                                        /*����-5��ʾThis Security Profile be used by some Wlans,please disable these Wlans first������-6��ʾerror*/
																					/*����-7��ʾhex key length error������-8��ʾkey format is incorrect(key should be '0' to '9' or 'a' to 'f')*/
																				    /*����-9��ʾSecurity ID�Ƿ�*/
																					/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*aflag==0��ʾ"acct"��aflag==1��ʾ"auth"*/																				
extern	int security_auth_acct(dbus_parameter parameter, DBusConnection *connection,int aflag,int id,char *secu_ip,int secu_port,char *secret);
																											 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown port*/
				                                                                                             /*����-2��ʾsecurity type which you choose not supported 802.1X������-3��ʾChange radius info not permited*/
				                                                                                             /*����-4��ʾThis Security Profile be used by some Wlans,please disable these Wlans first������-5��ʾerror*/
				                                                                                             /*����-6��ʾSecurity ID�Ƿ�������-7��ʾThe radius heart test is on,turn it off first!*/
																											 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																											 
extern  int delete_security(dbus_parameter parameter, DBusConnection *connection,int id);  /*����0��ʾ ɾ��ʧ�ܣ�����1��ʾɾ���ɹ�*/
																							/*����-1��ʾsecurity ID�Ƿ�������-2��ʾsecurity ID not existed*/
																							/*����-3��ʾThis Security Profile be used by some Wlans,please disable these Wlans first*/
																							/*����-4��ʾ��������-5��ʾThe radius heart test is on,turn it off first!*/
																							/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																											 
extern  int config_port_cmd_func(char *post_list,char *secu_id);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																	/*����-2��ʾsecurity id should be 1 to WLAN_NUM-1������-3��ʾunknown port format*/
																	/*����-4��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																	
extern 	int config_port_enable_cmd_func(char *post_list,char *state);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknow port format������-2��ʾerror*/
extern  int set_acct_interim_interval(dbus_parameter parameter, DBusConnection *connection,int sid,int time);  
																			/*����0��ʾ ʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput time value should be 0 to 32767*/
																	        /*����-2��ʾsecurity profile does not exist������-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																	        /*����-4��ʾCan't set acct interim interval under current security type������-5��ʾSecurity ID�Ƿ�*/
																			/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																			
extern int	secondary_radius_acct(dbus_parameter parameter, DBusConnection *connection,int id,char *secuip,int secu_port,char *secret);  
																										 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾsecurity type which you choose not supported 802.1X*/
																										 /*����-2��ʾChange radius info not permited������-3��ʾThis Security Profile be used by some Wlans,please disable these Wlans first*/
																										 /*����-4��ʾPlease use radius acct ip port shared_secret first,command failed������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																										 /*����-7��ʾThe radius heart test is on,turn it off first!������-8��ʾunknown port id*/
																										 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																										 
extern int secondary_radius_auth(dbus_parameter parameter ,DBusConnection *connection,int id,char * secuip,int secport,char *secr);
																								   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾsecurity type which you choose not supported 802.1X*/
																								   /*����-2��ʾChange radius info not permited������-3��ʾThis Security Profile be used by some Wlans,please disable these Wlans first*/
																								   /*����-4��ʾPlease use radius auth ip port shared_secret first,command failed������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																								   /*����-7��ʾThe radius heart test is on,turn it off first!������-8��ʾunknown port id*/
																								   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																								   
extern int config_vlan_list_enable_cmd_func(char *vlanlist,char *state);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter is illegal������-2��ʾerror*/
																								   
extern int config_vlan_list_security_cmd_func(char *vlanlist,char *secu_id);   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter illegal������-2��ʾunknown id format*/
																					/*����-3��ʾsecurity id should be 1 to WLAN_NUM-1������-4��ʾerror*/
																					/*����-5��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																					
extern int config_port_vlan_security_cmd_func(char *port_id,char *vlan_id,char *secu_id);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter illegal*/
																								/*����-2��ʾunknown id format������-3��ʾvlan id should be 1 to 4094*/
																								/*����-4��ʾsecurity id should be 1 to WLAN_NUM-1������-5��ʾerror*/
																								/*����-6��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																								
extern int config_port_vlan_enable_cmd_func(char *port_id,char *vlan_id,char *state);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter illegal*/
																							/*����-2��ʾunknown id format������-3��ʾvlan id should be 1 to 4094*/

/*time�ķ�Χ��0-32767*/
extern int set_eap_reauth_period_cmd(dbus_parameter parameter, DBusConnection *connection,int id,int time);
																			/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																			/*����-1��ʾinput period value should be 0 to 32767*/
																			/*����-2��ʾsecurity profile does not exist*/
																			/*����-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																			/*����-4��ʾCan't set eap reauth period under current security type*/
																			/*����-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																			/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int security_host_ip(dbus_parameter parameter, DBusConnection *connection,int id,char * ip_addr);   
																		/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown ip format*/
					                                                    /*����-2��ʾcheck_local_ip error������-3��ʾnot local ip������-4��ʾsecurity profile does not exist*/
				                                                        /*����-5��ʾthis security profile is used by some wlans,please disable them first������-6��ʾerror������-7��ʾSecurity ID�Ƿ�*/
																		/*����-8��ʾThe radius heart test is on,turn it off first!*/
																		/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*cer_type�ķ�Χ��"X.509"��"GBW"*/																		
extern int config_wapi_auth(dbus_parameter parameter, DBusConnection *connection,int sid,char *ip_addr,char *cer_type);	
																						/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown certification type*/
																						/*����-2��ʾunknown ip format*/
																						/*����-3��ʾsecurity type which you chose does not support wapi authentication*/
																						/*����-4��ʾthis security profile be used by some wlans,please disable them first*/
																						/*����-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																						/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*cer_type�ķ�Χ��"as","ae"��"ca"*/																						
extern int config_wapi_auth_path(dbus_parameter parameter, DBusConnection *connection,int sid,char *cer_type,char *path);  
																							/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																							/*����-1��ʾcertification isn't exit or can't be read*/
																							/*����-2��ʾsecurity type which you chose does not support wapi authentication*/
																							/*����-3��ʾthis security profile be used by some wlans,please disable them first*/
																							/*����-4��ʾthis security profile isn't integrity������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																							/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*state�ķ�Χ��"enable"��"disable"*/																							
extern int config_pre_auth_cmd_func(dbus_parameter parameter, DBusConnection *connection,int sec_id,char *state); 
																					/*����0��ʾʧ�ܣ�����1��ʾ �ɹ�������-1��ʾunknown encryption type*/
																					/*����-2��ʾencryption type does not match security type������-3��ʾsecurity profile does not exist*/
																					/*����-4��ʾthis security profile is used by some wlans,please disable them first������-5��ʾerror*/
																					/*����-6��ʾSecurity ID�Ƿ�*/
																					/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																					
/*UormΪ"unicast"��"multicast"��TorpΪ"time"��"packet"��paraΪ<0-400000000>*/																										
extern int set_wapi_rekey_para_cmd_func(dbus_parameter parameter, DBusConnection *connection,int id,char *Uorm,char *Torp,char *para);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown command format*/
																										/*����-2��ʾinput value should be 0 to 400000000������-3��ʾsecurity profile does not exist*/
																										/*����-4��ʾThis Security Profile be used by some Wlans,please disable them first*/
																										/*����-5��ʾCan't set wapi rekey parameter under current config������-6��ʾerror������-7��ʾSecurity ID�Ƿ�*/
																										/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*UormΪ"unicast"��"multicast"��MethodΪ"disable"��"time_based"��"packet_based"��"both_based"*/
extern int set_wapi_ucast_rekey_method_cmd_func(dbus_parameter parameter, DBusConnection *connection,int id,char *Uorm,char *Method);
																										  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown command format*/
																										  /*����-2��ʾsecurity profile does not exist*/
																										  /*����-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																										  /*����-4��ʾCan't set wapi unicast rekey method under current security type*/
																										  /*����-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																										  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*periodΪ<0-65535>*/
extern int set_security_quiet_period_cmd_func(dbus_parameter parameter, DBusConnection *connection,int sec_id,char *period);
																							 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput time value should be 0 to 65535*/
																						     /*����-2��ʾsecurity profile does not exist������-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																						     /*����-4��ʾCan't set 1x quiet period under current security type������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																							 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																							 
/*stateΪ"enable"��"disable"*/
extern int config_accounting_on_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *state);
																			   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown encryption type*/
																			   /*����-2��ʾsecurity type is needn't 802.1X������-3��ʾsecurity profile does not exist*/
																			   /*����-4��ʾthis security profile is used by some wlans,please disable them first*/
																			   /*����-5��ʾerrorthis security profile is used by some wlans,please disable them first*/
																			   /*����-6��ʾSecurity ID�Ƿ�*/
																			   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																			  
/*stateΪ"enable"��"disable"*/
extern int set_mobile_open_cmd(dbus_parameter parameter, DBusConnection *connection,char *state);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput should be enable or disable������-2��ʾerror*/
																			   						  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																			   
/*stateΪ"enable"��"disable"*/
extern int config_radius_extend_attr_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *state);
																				  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown encryption type*/
																				  /*����-2��ʾsecurity type is needn't 802.1X������-3��ʾsecurity profile does not exist*/
																				  /*����-4��ʾthis security profile is used by some wlans,please disable them first*/
																				  /*����-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																				  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*stateΪ"enable"��"disable"*/
extern int set_wapi_sub_attr_wapipreauth_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *state);
																				  		 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾWapiPreauth should be enable or disable*/
																					     /*����-2��ʾsecurity profile does not exist������-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																					     /*����-4��ʾCan't set WapiPreauth under current security type������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																						 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																						
/*stateΪ"enable"��"disable"*/
extern int set_wapi_sub_attr_multicaserekeystrict_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *state);
																						 		 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾMulticaseRekeyStrict should be enable or disable*/
																								 /*����-2��ʾsecurity profile does not exist������-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																								 /*����-4��ʾCan't set MulticaseRekeyStrict under current security type������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																								 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																								 
/*stateΪ"enable"��"disable"*/
extern int set_wapi_sub_attr_unicastcipherenabled_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *state);
																								   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾUnicastCipherEnabled should be enable or disable*/
																								   /*����-2��ʾsecurity profile does not exist������-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																								   /*����-4��ʾCan't set UnicastCipherEnabled under current security type������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																								   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																								   
/*stateΪ"enable"��"disable"*/
extern int set_wapi_sub_attr_authenticationsuiteenable_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *state);
																								   	   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾAuthenticationSuiteEnable should be enable or disable*/
																									   /*����-2��ʾsecurity profile does not exist������-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																									   /*����-4��ʾCan't set AuthenticationSuiteEnable under current security type������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																									   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																									  
/*value�ķ�Χ��0-64*/
extern int set_wapi_sub_attr_certificateupdatecount_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *value);
																								   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput retry value should be 0 to 64*/
																								   /*����-2��ʾsecurity profile does not exist������-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																								   /*����-4��ʾCan't set CertificateUpdateCount under current security type������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																								   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																								   
/*value�ķ�Χ��0-64*/
extern int set_wapi_sub_attr_multicastupdatecount_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *value);
																								   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput retry value should be 0 to 64*/
																								   /*����-2��ʾsecurity profile does not exist������-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																								   /*����-4��ʾCan't set MulticastUpdateCount under current security type������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																								   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																								   
/*value�ķ�Χ��0-64*/
extern int set_wapi_sub_attr_unicastupdatecount_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *value);
																								 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput	value should be 0 to 64*/
																								 /*����-2��ʾsecurity profile does not exist������-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																								 /*����-4��ʾCan't set UnicastUpdateCount under current security type������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																								 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																								 
/*value�ķ�Χ��0-86400*/
extern int set_wapi_sub_attr_bklifetime_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *value);
																					  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput value should be 0 to 86400*/
																					  /*����-2��ʾsecurity profile does not exist������-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																					  /*����-4��ʾCan't set BKLifetime under current security type������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																					  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																					  
/*value�ķ�Χ��0-99*/
extern int set_wapi_sub_attr_bkreauththreshold_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *value);
																					  		   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput  value should be 0 to 99*/
																							   /*����-2��ʾsecurity profile does not exist������-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																							   /*����-4��ʾCan't set BKReauthThreshold under current security type������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																							   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																							   
/*value�ķ�Χ��0-120*/
extern int set_wapi_sub_attr_satimeout_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *value);
																					 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput retry value should be 0 to 120*/
																					 /*����-2��ʾsecurity profile does not exist������-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																					 /*����-4��ʾCan't set SATimeout under current security type������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																					 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_security_wapi_info(struct dcli_security *sec);
/*ֻҪ���ã���ͨ��Free_security_wapi_info()�ͷſռ�*/
extern int  show_security_wapi_info_cmd(dbus_parameter parameter, DBusConnection *connection,char *id,struct dcli_security **sec);
											 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
											 /*����-2��ʾsecurity id should be 1 to WLAN_NUM-1������-3��ʾsecurity id does not exist*/
											 /*����-4��ʾsecurity id should be wapi_psk or wapi_auth*/
											 /*����-5��ʾsecurity's wapi config  is not intergrity������-6��ʾerror*/
											 /*����-7��ʾillegal input:Input exceeds the maximum value of the parameter type*/
											 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
											 
/*stateΪ"enable"��"disable"*/
extern int  config_asd_get_sta_info_able_cmd(dbus_parameter parameter, DBusConnection *connection,char *state);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾparameter illegal������-2��ʾerror*/
											 																		  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
											 
/*value�ķ�Χ��5-3600*/
extern int config_asd_get_sta_info_time_cmd(dbus_parameter parameter, DBusConnection *connection,char *value);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput time value should be 5 to 3600������-2��ʾerror*/
																													/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*stateΪ"enable"��"disable"*/
extern int set_notice_sta_info_to_portal_open_cmd(dbus_parameter parameter, DBusConnection *connection,char *state);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput should be enable or disable������-2��ʾerror*/
																														   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*stateΪ"enable"��"disable"*/
extern int config_wapi_multi_cert_cmd(dbus_parameter parameter, DBusConnection *connection,int SID,char *state);
																				/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾparameter illegal*/
																				/*����-2��ʾsecurity type which you chose does not support wapi authentication*/
																				/*����-3��ʾthis security profile be used by some wlans,please disable them first*/
																				/*����-4��ʾerror������-5��ʾSecurity ID�Ƿ�*/
																				/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																				
extern void Free_wlanid_security_wapi_info(struct dcli_security *sec);
/*ֻҪ���ã���ͨ��Free_wlanid_security_wapi_info()�ͷſռ�*/
extern int show_wlanid_security_wapi_config_cmd(dbus_parameter parameter, DBusConnection *connection,char *wlanID,struct dcli_security	**sec);
																												   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																												   /*����-2��ʾwlan id should be 1 to WLAN_NUM-1������-3��ʾwlan id does not exist*/
																												   /*����-4��ʾsecurity id should be wapi_psk or wapi_auth*/
																												   /*����-5��ʾsecurity's wapi config  is not intergrity������-6 ��ʾwlan has not apply any security*/
																											       /*����-7��ʾerror������-8��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																												   /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_radius_cmd(struct dcli_security *sec);
/*����1ʱ������Free_radius_cmd()�ͷſռ�*/																																 
extern int show_radius_cmd(dbus_parameter parameter, DBusConnection *connection,char *Radius_ID,struct dcli_security **sec);
																							/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																							/*����-1��ʾunknown id format*/
																							/*����-2��ʾradius id should be 1 to WLAN_NUM-1*/
																							/*����-3��ʾradius does not exist������-4��ʾerror*/
																							/*����-5��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																							/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*value�ķ�Χ��1-255*/
extern int set_wapi_sub_attr_multicast_cipher_cmd(dbus_parameter parameter, DBusConnection *connection,int sid,char *value);
																							  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput retry value should be 1 to 255*/
																							  /*����-2��ʾsecurity profile does not exist������-3��ʾThis Security Profile be used by some Wlans,please disable them first*/
																							  /*����-4��ʾCan't set multicast sipher under current security type������-5��ʾerror������-6��ʾSecurity ID�Ƿ�*/
																							  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*stateΪ"enable"��"disable"*/
extern int config_asd_ieee_80211n_able_cmd(dbus_parameter parameter, DBusConnection *connection,char *state);
																				/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																			    /*����-1��ʾparameter illegal������-2��ʾerror*/
																				/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*sec_index�ķ�Χ��1-4*/
extern int set_security_wep_index_cmd(dbus_parameter parameter, DBusConnection *connection,int sid,char *sec_index);
																					 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format������-2��ʾinput security index should be 1 to 4*/
																					 /*����-3��ʾsecurity profile does not exist������-4��ʾThis Security Profile be used by some Wlans,please disable them first*/
																					 /*����-5��ʾthe encryption type of the security should be wep������-6��ʾerror������-7��ʾSecurity ID�Ƿ�*/
																					 /*����-8��ʾillegal input:Input exceeds the maximum value of the parameter type*/
																					 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*trap_typeΪ"wtp_deny_sta","sta_verify_failed","sta_assoc_failed","wapi_invalid_cert","wapi_challenge_replay","wapi_mic_juggle","wapi_low_safe","wapi_addr_redirection"*/
/*stateΪ"enable"��"disable"*/
extern int config_set_asd_trap_able_cmd(dbus_parameter parameter, DBusConnection *connection,char *trap_type,char *state);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾparameter illegal������-2��ʾerror*/
																					 											/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																					 
extern int show_asd_trap_state_cmd(dbus_parameter parameter, DBusConnection *connection,struct asd_trap_state_info *info);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																																/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																				
/*cer_typeΪ"as","ae"��"ca"*/
extern int config_wapi_p12_cert_auth_path_cmd(dbus_parameter parameter, DBusConnection *connection,int sid,char *cer_type,char *path,char *passwd);
																													  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																													  /*����-1��ʾcertification isn't exit or can't be read*/
																													  /*����-2��ʾsecurity type which you chose does not support wapi authentication*/
																													  /*����-3��ʾthis security profile be used by some wlans,please disable them first*/
																													  /*����-4��ʾthis security profile isn't integrity*/
																													  /*����-5��ʾp12 cert password error������-6��ʾerror*/
																													  /*����-7��ʾSecurity ID�Ƿ�*/
extern void Free_show_radius_all_cmd(struct dcli_security *sec);
/*����1ʱ������Free_show_radius_all_cmd()�ͷſռ�*/
extern int show_radius_all_cmd(dbus_parameter parameter, DBusConnection *connection,struct dcli_security **sec); 
																				  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																				  /*����-1��ʾno security support radius*/
																				  /*����-2��ʾerror*/
																				  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*stateΪ"enable"��"disable"*/
extern int config_hybrid_auth_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *state);
																			   /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown security type������-2��ʾSecurity ID�Ƿ�*/
																			   /*����-3��ʾsecurity profile does not exist������-4��ʾthis security profile is used by some wlans,please disable them first*/
																			   /*����-5��ʾerror������SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*value must multiple of 15,range is 30-86400*/
extern int set_ap_max_detect_interval_cmd(dbus_parameter parameter, DBusConnection *connection,int id,char *value);
																				/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown format,please input number*/
																				/*����-2��ʾthe number is not be multiple of 15������-3��ʾSecurity ID�Ƿ�*/
																				/*����-4��ʾsecurity profile does not exist������-5��ʾthis security profile is used by some wlans,please disable them first*/
																				/*����-6��ʾerror*/

#endif

