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
* ws_dcli_ac_ip_list.h
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


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "ws_init_dbus.h"
#include "ws_dbus_list_interface.h"
#include "wcpss/wid/WID.h"
#include "wcpss/asd/asd.h"
#include "dbus/wcpss/ACDbusDef1.h"
#include "dbus/asd/ASDDbusDef1.h"
#include <time.h>     //xm add
#include <sys/time.h> //xm add


extern void Free_show_ac_ip_list_one(DCLI_AC_IP_LIST_API_GROUP *IPLIST);
/*����1ʱ������Free_show_ac_ip_list_one()�ͷſռ�*/
/*id�ķ�Χ��1��ACIPLIST_NUM-1*/
extern int show_ac_ip_list_one_cmd(int instance_id,char *id, DCLI_AC_IP_LIST_API_GROUP **IPLIST);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																									   /*����-2��ʾAC IP LIST id should be 1 to ACIPLIST_NUM-1*/
											                                             			   /*����-3��ʾid does not exist������-4��ʾerror*/

extern void Free_show_ac_ip_list(DCLI_AC_IP_LIST_API_GROUP *IPLIST);
/*����1ʱ������Free_show_ac_ip_list()�ͷſռ�*/
extern int show_ac_ip_list_cmd(int instance_id,DCLI_AC_IP_LIST_API_GROUP **IPLIST);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�*/

/*ID�ķ�Χ��1��ACIPLIST_NUM-1*/
/*ifname�ĳ���ҪС��16 */
extern int create_ac_ip_list_cmd(int instance_id,char *ID,char *ifname);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																		   /*����-2��ʾid should be 1 to ACIPLIST_NUM-1������-3��ʾifname is too long,out of the limit of 16*/
																		   /*����-4��ʾid exist������-5��ʾerror������-6��ʾinterface has be binded in other hansi*/

/*ID�ķ�Χ��1��ACIPLIST_NUM-1*/
extern int del_ac_ip_list_cmd(int instance_id,char *ID);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
														   /*����-2��ʾid should be 1 to ACIPLIST_NUM-1������-3��ʾwlan id does not exist*/
														   /*����-4��ʾwlan is enable,please disable it first������-5��ʾerror*/

/*Priority�ķ�Χ��1-100*/
extern int add_ac_ip_cmd(int instance_id,char *id,char *IP,char *Priority);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown ip format*/
																			   /*����-2��ʾunknown id format������-3��ʾwlan id does not exist*/
																			   /*����-4��ʾinterface does not exist������-5��ʾwlan is enable,please disable it first*/
																			   /*����-6��ʾerror*/

extern int del_ac_ip_cmd(int instance_id,char *id,char *IP);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown ip format*/
															   /*����-2��ʾwlan id does not exist������-3��ʾinterface does not exist*/
															   /*����-4��ʾwlan is enable,please disable it first������-5��ʾerror*/

extern int modify_ac_ip_priority_cmd(int instance_id,char *id,char *IP,char *Priority);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown ip format*/
																						   /*����-2��ʾunknown id format������-3��ʾwlan id does not exist*/
																						   /*����-4��ʾinterface does not exist������-5��ʾwlan is enable,please disable it first*/
																						   /*����-6��ʾerror*/

extern int set_ac_ip_list_banlance_cmd(int instance_id,char *id,char *state);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾwlan id does not exist*/
																				  /*����-2��ʾinterface does not exist������-3��ʾwlan is enable,please disable it first*/
																				  /*����-4��ʾerror*/

extern int set_ac_ip_threshold_cmd(int instance_id,char *id,char *IP,char *value);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown ip format*/
																					   /*����-2��ʾunknown id format������-3��ʾwlan id does not exist*/
																					   /*����-4��ʾinterface does not exist������-5��ʾwlan is enable,please disable it first*/
																					   /*����-6��ʾip addr no exit*/
																						   
extern int set_ac_ip_diffnum_banlance_cmd(int instance_id,char *id,char *value);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																					   /*����-2��ʾwlan id does not exist������-3��ʾinterface does not exist*/
																					   /*����-4��ʾwlan is enable,please disable it first������-5��ʾerror*/


