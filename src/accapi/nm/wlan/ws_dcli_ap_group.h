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
* ws_dcli_ap_group.h
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
#include <fcntl.h> 
#include "ws_init_dbus.h"
//#include "ws_fdb.h"
//#include "ws_dcli_acl.h"
#include <dbus/dbus.h>
#include "wcpss/waw.h"
#include "wcpss/wid/WID.h"
#include "dbus/wcpss/ACDbusDef1.h"
#include "dbus/asd/ASDDbusDef1.h"
//#include "dbus/wcpss/dcli_wid_wtp.h"
//#include "dbus/wcpss/dcli_wid_wlan.h"
//#include "memory.h"
//#include "sysdef/npd_sysdef.h"
//#include "dbus/npd/npd_dbus_def.h"
//#include "wcpss/asd/asd.h"

extern int create_ap_group_cmd(int instance_id,char *ap_g_id,char *ap_g_name);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
																				   /*����-2��ʾap_g_id should be 1 to WTP_GROUP_NUM-1������-3��ʾname is too long,out of the limit of 128*/
																				   /*����-4��ʾid exist������-5��ʾerror*/

extern int del_ap_group_cmd(int instance_id,char *ap_g_id);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown id format*/
															   /*����-2��ʾap_g_id should be 1 to WTP_GROUP_NUM-1������-3��ʾap group id does not exist*/
															   /*����-4��ʾerror*/

/*operΪ"add"��"delete"*/
/*wtp_id_listΪ"all"��AP ID�б���ʽΪ1,8,9-20,33*/
extern int add_del_ap_group_member_cmd(int instance_id,int ap_g_id,char *oper,char *wtp_id_list);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾunknown command*/
																										   /*����-2��ʾset wtp list error,like 1,8,9-20,33������-3��ʾap group id�Ƿ�*/
																										   /*����-4��ʾap group id does not exist������-5��ʾerror*/

extern void Free_show_group_member_cmd(unsigned int *wtp_list);
/*ֻҪ���ú������͵���Free_show_group_member_cmd()�ͷſռ�*/
extern int show_group_member_cmd(int instance_id,int ap_g_id,unsigned int **wtp_list);/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾap group id�Ƿ�*/
																							  /*����-2��ʾap group id does not exist������-3��ʾerror*/


