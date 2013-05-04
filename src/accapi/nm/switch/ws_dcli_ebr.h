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
* ws_dcli_ebr.h
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



#ifndef _WS_DCLI_EBR_H
#define _WS_DCLI_EBR_H

#include <syslog.h>
#include "ws_init_dbus.h"
#include "ws_dbus_list_interface.h"
#include "wcpss/wid/WID.h"
#include "dbus/wcpss/ACDbusDef1.h"
#include "dbus/asd/ASDDbusDef1.h"

extern int create_ethereal_bridge_cmd(dbus_parameter parameter, DBusConnection *connection,char *id,char *brname);  
																					/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																					/*����-2��ʾebr id should be 1 to EBR_NUM-1������-3��ʾebr name is too long,it should be 1 to 15*/
																					/*����-4��ʾebr id exist������-5��ʾebr  is already exist������-6��ʾsystem cmd error������-7��ʾerror*/
																					/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																					
extern int delete_ethereal_bridge_cmd(dbus_parameter parameter, DBusConnection *connection,char *id);  
																		/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format������-2��ʾebr id should be 1 to EBR_NUM-1*/
																		/*����-3��ʾebr id does not exist������-4��ʾsystem cmd error������-5��ʾebr is enable,please disable it first������-6��ʾerror*/		
																		/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																		
extern void Free_ethereal_bridge_one_head(DCLI_EBR_API_GROUP *EBRINFO);
/*����1ʱ������Free_ethereal_bridge_one_head()�ͷſռ�*/
extern int show_ethereal_bridge_one(dbus_parameter parameter, DBusConnection *connection,char *id,DCLI_EBR_API_GROUP **EBRINFO );  
																									/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																									/*����-2��ʾebr id should be 1 to EBR_NUM-1������-3��ʾebr id does not exist*/
																									/*����-4��ʾerror*/ 			
																									/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																									
extern void Free_ethereal_bridge_head(DCLI_EBR_API_GROUP *EBRINFO);
/*����1ʱ������Free_ethereal_bridge_head()�ͷſռ�*/
extern int show_ethereal_bridge_list(dbus_parameter parameter, DBusConnection *connection,DCLI_EBR_API_GROUP **EBRINFO);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾno ebr exist*/
																															/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int config_ethereal_bridge_enable_cmd(dbus_parameter parameter, DBusConnection *connection,int ebr_id,char *ebr_state);
																								/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter should only be 'enable' or 'disable'*/
																								/*����-2��ʾebr id does not exist������-3��ʾebr if error������-4��ʾsystem cmd process error*/
																								/*����-5��ʾerror������-6ʾEBR ID�Ƿ�*/
																								/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																								
extern int ebr_set_bridge_isolation_func(dbus_parameter parameter, DBusConnection *connection,int ebr_id,char *isolate_state);	
																								/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter should only be 'enable' or 'disable'*/
																								/*����-2��ʾebr id does not exist������-3��ʾebr should be disable first������-4��ʾebr if error*/
																								/*����-5��ʾsystem cmd process error������-6��ʾsameportswitch and isolation are conflict,disable sameportswitch first*/
																								/*����-7��ʾerror������-8ʾEBR ID�Ƿ�������-9��ʾapply security in this wlan first*/
																								/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																								
extern int ebr_set_bridge_multicast_isolation_func(dbus_parameter parameter, DBusConnection *connection,int ebr_id,char *mult_isolate_state);   
																												/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter should only be 'enable' or 'disable'*/
																												/*����-2��ʾebr id does not exist������-3��ʾebr should be disable first������-4��ʾebr if error*/
																												/*����-5��ʾsystem cmd process error������-6��ʾsameportswitch and isolation are conflict,disable sameportswitch first*/
																												/*����-7��ʾerror������-8ʾEBR ID�Ƿ�������-9��ʾapply security in this wlan first*/
																												/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																												
extern int set_ebr_add_del_if_cmd(dbus_parameter parameter, DBusConnection *connection,int ebr_id,char *if_state,char *if_name);
																								/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter should only be 'add' or 'delete'*/
																								/*����-2��ʾif name too long������-3��ʾebr id does not exist������-4��ʾebr should be disable first*/
																								/*����-5��ʾif_name already exist/remove some br,or system cmd process error������-6��ʾinput ifname error*/
																								/*����-7��ʾebr if error������-8��ʾerror������-9ʾEBR ID�Ƿ�*/
																								/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
																								
extern int ebr_set_bridge_sameportswitch_func(dbus_parameter parameter, DBusConnection *connection,int ebr_id,char *spswitch_state);  
																										/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾinput parameter should only be 'enable' or 'disable'*/
																										/*����-2��ʾebr id does not exist������-3��ʾebr should be disable first������-4��ʾebr if error*/
																										/*����-5��ʾsystem cmd process error������-6��ʾisolation or multicast are enable,disable isolation and multicast first*/
																										/*����-7��ʾerror������-8ʾEBR ID�Ƿ�*/
																										/*����SNMPD_CONNECTION_ERROR��ʾconnection error*/

extern int set_ebr_add_del_uplink_cmd(dbus_parameter parameter, DBusConnection *connection,int ebr_id,char *addordel,char *ifnamez); 
																									  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾerror*/
																									  /*����-2��ʾinput parameter should only be 'add' or 'delete'������-3��ʾif name too long*/
																									  /*����-4��ʾmalloc error������-5��ʾebr should be disable first*/
																									  /*����-6��ʾalready exist/remove some br,or system cmd process error������-7��ʾinput ifname error*/
																									  /*����-8��ʾebr if error������-9��ʾinterface does not add to br or br uplink������-10��ʾebr id does not exist*/
																									  /*����-11ʾEBR ID�Ƿ�*/
																									  /*����SNMPD_CONNECTION_ERROR��ʾconnection error*/
#endif

