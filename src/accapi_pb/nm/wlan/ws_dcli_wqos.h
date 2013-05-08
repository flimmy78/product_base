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
* ws_dcli_wqos.h
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



#ifndef _WS_DCLI_WQOS_H
#define _WS_DCLI_WQOS_H
#include <syslog.h>
#include "ws_init_dbus.h"
#include "ws_dbus_list_interface.h"
#include "wcpss/wid/WID.h"
#include "wcpss/asd/asd.h"
#include "dbus/wcpss/ACDbusDef1.h"
#include "dbus/asd/ASDDbusDef1.h"
#include "ws_dcli_wlans.h"

typedef enum{
	qos_check_dot1p_state=0,
	qos_check_comma_state,
	qos_check_fail_state,
	qos_check_end_state,
	qos_check_success_state
}dot1p_list_state;


#define QOS_CWMIN_NUM	16
#define QOS_CWMAX_NUM	16
#define QOS_AIFS_NUM	16
#define QOS_TXOPLIMIT_NUM	8192
#define QOS_DOT1P_COMMA 	','	
#define DCLIWQOS_QOS_FLOW_NUM	4
#define DCLIWQOS_DOT1P_LIST_NUM	16



extern int create_qos(dbus_parameter parameter, DBusConnection *connection,char *id, char *qos_name);
																  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format������-2��ʾqos id should be 1 to QOS_NUM-1*/
												    			  /*����-3��ʾqos name is too long,it should be 1 to 15������-4��ʾqos id exist������-5��ʾerror*/
																  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int delete_qos(dbus_parameter parameter, DBusConnection *connection,char *id);
												  /*����0��ʾ ɾ��ʧ�ܣ�����1��ʾɾ���ɹ�������-1��ʾunknown id format������-2��ʾqos id should be 1 to QOS_NUM-1*/
												  /*����-3��ʾqos id does not exist������-4��ʾthis qos profile be used by some radios,please disable them first������-5��ʾerror*/
												  /*����-6��ʾthis qos now be used by some radios,please delete them*/
												  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_qos_one(DCLI_WQOS *WQOS);
/*����1ʱ������Free_qos_one()�ͷſռ�*/									
extern int show_qos_one(dbus_parameter parameter, DBusConnection *connection,char *id,DCLI_WQOS **WQOS);
																		/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format������-2��ʾqos id should be 1 to QOS_NUM-1*/
				                                                        /*����-3��ʾqos id does not exist������-4��ʾerror*/
																		/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_qos_head(DCLI_WQOS *WQOS);
/*����1ʱ������Free_qos_head()�ͷſռ�*/																		
extern int show_wireless_qos_profile_list(dbus_parameter parameter, DBusConnection *connection,DCLI_WQOS **WQOS);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾqos not exsit*/
																													 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*qos_stream�ķ�Χ��"voice","video","besteffort"��"background"*/
/*ACK�ķ�Χ��"ack"��"noack"*/
extern int config_radio_qos_service(dbus_parameter parameter, DBusConnection *connection,int qos_id,char *qos_stream,char *CWMIN,char *CWMAX,char *AIFS,char *TXOPLIMIT,char *ACK);  
																									/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown qos type������-2��ʾunknown id format*/
																						 			/*����-3��ʾqos cwmin should be 0 to QOS_CWMIN_NUM-1������-4��ʾqos cwmax should be 0 to QOS_CWMAX_NUM-1*/
																						 			/*����-5��ʾqos aifs should be 0 to QOS_AIFS_NUM-1������-6��ʾqos txoplimit should be 0 to QOS_TXOPLIMIT_NUM*/
																						 			/*����-7��ʾqos profile does not exist������-8��ʾthis qos profile is used by some radios,please disable them first*/
																						 			/*����-9��ʾerror������-10��ʾWQOS ID�Ƿ�������-11��ʾcwmin is not allow larger than cwmax*/
																									/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*qos_stream�ķ�Χ��"voice","video","besteffort"��"background"*/
extern int config_client_qos_service(dbus_parameter parameter, DBusConnection *connection,int qos_id,char *qos_stream,char *CWMIN,char *CWMAX,char *AIFS,char *TXOPLIMIT);  
																										  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown qos type������-2��ʾunknown id format*/
																										  /*����-3��ʾqos cwmin should be 0 to QOS_CWMIN_NUM-1������-4��ʾqos cwmax should be 0 to QOS_CWMAX_NUM-1*/
																										  /*����-5��ʾqos aifs should be 0 to QOS_AIFS_NUM-1������-6��ʾqos txoplimit should be 0 to QOS_TXOPLIMIT_NUM*/
																										  /*����-7��ʾqos profile does not exist������-8��ʾthis qos profile is used by some radios,please disable them first*/
																										  /*����-9��ʾerror������-10��ʾWQOS ID�Ƿ�������-11��ʾcwmin is not allow larger than cwmax*/
																										  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*state�ķ�Χ��"enable"��"disable"*/
extern int config_wmm_service(dbus_parameter parameter, DBusConnection *connection,int qos_id,char *state);  
																			 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter should be only 'enable' or 'disable'*/
																		     /*����-2��ʾqos id does not exist������-3��ʾthis qos profile be used by some radios,please disable them first*/
																		     /*����-4��ʾerror������-5��ʾWQOS ID�Ƿ�*/
																			 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*WMM_ORDER�ķ�Χ��"voice","video","besteffort"��"background"*/
extern int config_wmm_map_dotlp(dbus_parameter parameter, DBusConnection *connection,int qos_id,char *WMM_ORDER,char *DOTLP);  
																			 					 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter should be only 'voice' 'video' 'besteffort' or 'background'*/
																								 /*����-2��ʾunknown id format������-3��ʾqos dot1p should be 0 to 7������-4��ʾqos id does not exist*/
																								 /*����-5��ʾthis qos profile be used by some radios,please disable them first������-6��ʾthis qos map is disable,please enable it first*/
																								 /*����-7��ʾerror������-8��ʾWQOS ID�Ƿ�*/
																								 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*WMM_ORDER�ķ�Χ��"voice","video","besteffort"��"background"*/
extern int config_dotlp_map_wmm(dbus_parameter parameter, DBusConnection *connection,int qos_id,char *DOTLP,char *WMM_ORDER); 
																								 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter is illegal*/
																						 	     /*����-2��ʾinput parameter should be only 'voice' 'video' 'besteffort' or 'background'������-3��ʾqos id does not exist*/
																								 /*����-4��ʾthis qos profile be used by some radios,please disable them first������-5��ʾthis qos map is disable,please enable it first*/
																								 /*����-6��ʾerror������-7��ʾWQOS ID�Ƿ�*/
																								 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern void Free_qos_extension_info(DCLI_WQOS *WQOS);																								 
/*����1ʱ������Free_qos_extension_info()�ͷſռ�*/																								 
extern int show_qos_extension_info(dbus_parameter parameter, DBusConnection *connection,char *wqos_id,DCLI_WQOS **WQOS);
																						 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																						 /*����-1��ʾunknown id format*/
																						 /*����-2��ʾqos id should be 1 to QOS_NUM-1*/
																						 /*����-3��ʾqos id does not exist������-4��ʾerror*/
																						 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_qos_total_bandwidth(dbus_parameter parameter, DBusConnection *connection,int wqos_id,char *value);
																				 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																				 /*����-1��ʾunknown id format������-2��ʾqos dot1p should be 1 to 25*/
																				 /*����-3��ʾqos id does not exist������-4��ʾerror������-5��ʾWQOS ID�Ƿ�*/
																				 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*flow_type�ķ�Χ��"besteffort","background","video"��"voice"*/
/*par_type�ķ�Χ��"averagerate","maxburstiness","managepriority","shovepriority","grabpriority","maxparallel","bandwidth"��"bandwidthpercentage"*/
extern int wid_config_set_qos_flow_parameter(dbus_parameter parameter, DBusConnection *connection,int wqos_id,char *flow_type,char *par_type,char *para_value);
																				 						 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown qos flow type*/
																										 /*����-2��ʾunknown qos parameter type������-3��ʾunknown value format*/
																										 /*����-4��ʾqos id does not exist������-5��ʾerror������-6��ʾWQOS ID�Ƿ�*/
																										 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*par_type�ķ�Χ��"totalbandwidth","resourcescale","sharebandwidth"��"resourcesharescale"*/
extern int wid_config_set_qos_parameter(dbus_parameter parameter, DBusConnection *connection,int wqos_id,char *par_type,char *par_value);   
																										 	 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																											 /*����-1��ʾunknown qos parameter type������-2��ʾunknown value format*/
																											 /*����-3��ʾqos id does not exist������-4��ʾerror������-5��ʾWQOS ID�Ƿ�*/
																											 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*policy_type�ķ�Χ��"grab"��"shove"*/
extern int wid_config_set_qos_policy_used(dbus_parameter parameter, DBusConnection *connection,int wqos_id,char *policy_type,char *policy_state);  
																											 		 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																													 /*����-1��ʾunknown qos policy type*/
																													 /*����-2��ʾqos id does not exist������-3��ʾerror*/
																													 /*����-4��ʾWQOS ID�Ƿ�*/
																													 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/*policy_type�ķ�Χ��"grab"��"shove"*/
extern int wid_config_set_qos_policy_name(dbus_parameter parameter, DBusConnection *connection,int wqos_id,char *policy_type,char *policy_name);   
																													 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																													 /*����-1��ʾpolicy name too long������-2��ʾunknown qos policy type*/
																													 /*����-3��ʾqos id does not exist������-4��ʾerror������-5��ʾWQOS ID�Ƿ�*/
																													 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int wid_config_set_qos_manage_arithmetic_name(dbus_parameter parameter, DBusConnection *connection,int wqos_id,char *arithmetic_name);  
																													 /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/
																													 /*����-1��ʾarithmetic name too long*/
																													 /*����-2��ʾqos id does not exist������-3��ʾerror*/
																													 /*����-4��ʾWQOS ID�Ƿ�*/
																													 /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

/* typeΪ"besteffort"|"background"|"video"|"voice" */
/*R_L_ID�ķ�Χ��0-3*/
/*����1ʱ������Free_qos_one()�ͷſռ�*/
extern int wid_show_qos_radio_cmd(dbus_parameter parameter, DBusConnection *connection,char *wtp_id,char *R_L_ID,char *type,DCLI_WQOS **WQOS);
																												/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown wtpid format*/
																											    /*����-2��ʾunknown local radio id format������-3��ʾwtp id should be 1 to WTP_NUM-1*/
																											    /*����-4��ʾlocal radio id should be 0 to 3������-5��ʾunknown qos flow type*/
																											    /*����-6��ʾwtp id not exist������-7��ʾradio id not exist������-8��ʾqos id not exist*/
																											    /*����-9��ʾwtp WTPID radio radio_l_id didn't bind qos������-10��ʾqos id should be 1 to QOS_NUM-1*/
																											    /*����-11��ʾerror*/
																												/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
#endif	
