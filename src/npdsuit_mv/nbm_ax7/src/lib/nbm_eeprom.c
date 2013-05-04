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
* nbm_eeprom.c
*
*
* CREATOR:
*		chenb@autelan.com
*
* DESCRIPTION:
*		APIs used in NBM for eeprom.
*
* DATE:
*		02/21/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.29 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "sysdef/npd_sysdef.h"
#include "util/npd_list.h"
#include "npd/nam/npd_amapi.h"
#include "npd/nbm/npd_bmapi.h"
#include "nbm_eeprom.h"
#include "nbm_cpld.h"
#include "nbm_log.h"

#define DEFAULT_PRODUCT_NAME "DEFPROD-AX7K"
#define DEFAULT_SW_NAME "Auteware"
#define DEFAULT_BASEMAC "000DEF000DEF"
#define DEFAULT_SYSSN "DEFsyssn1234"

#define DEFAULT_MODSN "DEFmodsn2222"
#define DEFAULT_MODNAME "DEFMODAX7-6GTX"

#define DEFAULT_MAINBOARD_NAME "DEFmb_MAINBOARD"
#define DEFAULT_MAINBOARD_SN "DEFmbsn33"
#define DEFAULT_AX7K_MAINBOARD_NAME	"DEFAX7K_CRSMU"
#define DEFAULT_AX7K_6GTX_NAME   "DEFAX7K_6GTX"
#define DEFAULT_AX7K_6GE_SFP_NAME   "DEFAX7K_6GE_SFP"
#define DEFAULT_AX7K_XFP_NAME   "DEFAX7K_XFP"
#define DEFAULT_AX7K_6GTX_POE_NAME   "DEFAX7K_6GTX_POE"
#define DEFAULT_AX7KI_ALPHA_MAINBOARD_NAME "DEFAX7K_I CRSMU"
#define DEFAULT_AX7KI_ALPHA_GTX_NAME "DEFAX7K_I GTX"
#define DEFAULT_AX7KI_ALPHA_XG_CONN_NAME "DEFAX7K_I XCONN"

#define DEFAULT_AX5_NOMAINBOARD_NAME	"DEFAX5K"
#define DEFAULT_AX5I_NOMAINBOARD_NAME	"DEFAX5KI"
#define DEFAULT_AX5E_NOMAINBOARD_NAME	"DEFAX5KE"
#define DEFAULT_AU4_NOMAINBOARD_NAME "DEFAU4K"

#define DEFAULT_BACKPLANE_NAME "DEFAX7K_BACKPLANE"
#define DEFAULT_BACKPLANE_SN "DEFbpsn23"

#define MODNAME_NULL  "null"
#define MODSN_NULL  MODNAME_NULL
#define SYSINFO_OEM_BTO 
#ifdef SYSINFO_OEM_BTO
#define SYSINFO_OEM_BTO_PRODUCT_NAME 	"/devinfo/product_name"
#define SYSINFO_OEM_BTO_SW_NAME 		"/devinfo/software_name"
#define SYSINFO_OEM_BTO_VENDOR_NAME 	"/devinfo/enterprise_name"
#define SYSINFO_OEM_BTO_VENDOR_SNMP_OID "/devinfo/enterprise_snmp_oid"
#define SYSINFO_OEM_BTO_SNMP_SYS_OID 		"/devinfo/snmp_sys_oid"
#define SYSINFO_OEM_BTO_MAC 			"/devinfo/mac"
#define SYSINFO_OEM_BTO_PROC_MAC        "/proc/sysinfo/product_base_mac_addr"

#define SYSINFO_OEM_BTO_LEN_PRODUCT_NAME 	24
#define SYSINFO_OEM_BTO_LEN_SW_NAME 		24
#define SYSINFO_OEM_BTO_LEN_VENDOR_NAME 	64
#define SYSINFO_OEM_BTO_LEN_VENDOR_SNMP_OID 128
#define SYSINFO_OEM_BTO_LEN_SNMP_SYS_OID 		128
#define SYSINFO_OEM_BTO_LEN_MAC 			12

typedef struct _sysinfo_oem_bto_info_ {
	char product_name[SYSINFO_OEM_BTO_LEN_PRODUCT_NAME];
	char sw_name[SYSINFO_OEM_BTO_LEN_SW_NAME];
	char enterprise_name[SYSINFO_OEM_BTO_LEN_VENDOR_NAME];
	char enterprise_snmp_oid[SYSINFO_OEM_BTO_LEN_VENDOR_SNMP_OID];
	char snmp_sys_oid[SYSINFO_OEM_BTO_LEN_SNMP_SYS_OID];
	char mac_addr[SYSINFO_OEM_BTO_LEN_MAC];
}SYSINFO_OEM_BTO_INFO;

static SYSINFO_OEM_BTO_INFO g_oem_bto_sysinfo;
#endif
/* Slot no is the number printed in the front panel, not slot array index used in software*/
int nbm_read_chassis_module_sysinfo(int slotno,struct module_info_s *module) {

	ax_module_sysinfo  sysinfo;
	int fd = -1;
	int result = 0;
	char *ptr = NULL;
	char * modnamefile = NULL;
	char tmpModeName[SYSINFO_MODULE_NAME] = {0};
	int len = 0;

	/* Setup default value for chassis module*/
	module->sn = DEFAULT_MODSN;
	module->modname = DEFAULT_MODNAME;
	switch(module->id) {
		
		case MODULE_ID_NONE:
			module->modname = MODNAME_NULL;
			break;
		case MODULE_ID_AX7_CRSMU:
			module->modname = DEFAULT_AX7K_MAINBOARD_NAME;
			modnamefile = "/devinfo/c5a001";
			break;
		case MODULE_ID_AX7_6GTX:
			module->modname = DEFAULT_AX7K_6GTX_NAME;
			modnamefile = "/devinfo/c5a002";
			break;
		case MODULE_ID_AX7_6GE_SFP:
			module->modname = DEFAULT_AX7K_6GE_SFP_NAME;
			modnamefile = "/devinfo/c5a003";
			break;
		case MODULE_ID_AX7_XFP:
			module->modname = DEFAULT_AX7K_XFP_NAME;
			modnamefile = "/devinfo/c5a004";
			break;
		case MODULE_ID_AX7I_CRSMU:
			module->modname = DEFAULT_AX7KI_ALPHA_MAINBOARD_NAME;
			modnamefile = "/devinfo/c2a001";
			break;
		case MODULE_ID_AX7I_GTX:
			module->modname = DEFAULT_AX7KI_ALPHA_GTX_NAME;
			modnamefile = "/devinfo/c2a002";
			break;
#if 0
		case MODULE_ID_AX7I_XG_CONNECT:
			/*return; filter the XG CONNECT --yangxs*/
			module->modname = DEFAULT_AX7KI_ALPHA_XG_CONN_NAME;
			modnamefile = "/devinfo/c2a003";
			break;
#endif
		case MODULE_ID_AX5_5612:
			module->modname = DEFAULT_AX5_NOMAINBOARD_NAME;
			break;
		case MODULE_ID_AX5_5612I:
			module->modname = DEFAULT_AX5I_NOMAINBOARD_NAME;
			break;
		case MODULE_ID_AX5_5612E:
			module->modname = DEFAULT_AX5E_NOMAINBOARD_NAME;
			break;
		case MODULE_ID_AX7_6GTX_POE:
			module->modname = DEFAULT_AX7K_6GTX_POE_NAME;
			/*modnamefile = "/devinfo/c5a005";  ? */
			break;
		case MODULE_ID_AU4_4626:
		case MODULE_ID_AU4_4524:
		case MODULE_ID_AU4_4524_POE:
		case MODULE_ID_AU3_3524:
			module->modname = DEFAULT_AU4_NOMAINBOARD_NAME;
			break;
		default:
			module->modname = MODNAME_NULL;
			break;
	}
	syslog_ax_eeprom_dbg("read chassis module sysinfo get system:\n");
	syslog_ax_eeprom_dbg("%-12s:%s\n","MODULE SN",module->sn);
	syslog_ax_eeprom_dbg("%-12s:%s\n","MODULE NAME",module->modname);

	
	memset(&sysinfo, 0, sizeof(ax_module_sysinfo));
	/* TODO: read EEPROM get module sn*/

	if(g_bm_fd < 0) 
	{
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			syslog_ax_eeprom_err("open dev %s error(%d) when read chassis module sysinfo!\n",NPD_BM_FILE_PATH,fd);
			/*return -1;*/

		}
		g_bm_fd = fd;
	}
    else
	{
		if((1 == slotno)&&((MODULE_ID_AX7I_CRSMU == module->id)||(MODULE_ID_AX7I_GTX == module->id)))
		{
			slotno = 0;
		}		
		sysinfo.slot_num = slotno;
		sysinfo.product_id = 7;
		
		result = ioctl(g_bm_fd,BM_IOC_READ_MODULE_SYSINFO,&sysinfo);
		if (result != 0) 
		{
			syslog_ax_eeprom_err("read chassis module sysinfo error!\n");
			/*return -1;*/
		}			
    }
	syslog_ax_eeprom_dbg("read chassis module sysinfo from eeprom:\n");
	syslog_ax_eeprom_dbg("%-12s:%s\n","MODULE SN",sysinfo.ax_sysinfo_module_serial_no);
	syslog_ax_eeprom_dbg("%-12s:%s\n","MODULE NAME",sysinfo.ax_sysinfo_module_name);
	/*syslog_ax_eeprom_dbg("%-12s:%d\n","MODULE SN",sysinfo.ax_sysinfo_module_serial_no[0]);
	syslog_ax_eeprom_dbg("%-12s:%d\n","MODULE NAME",sysinfo.ax_sysinfo_module_name[0]);	*/
	syslog_ax_eeprom_dbg("%-12s:%d\n","SLOT NO", sysinfo.slot_num);
	syslog_ax_eeprom_dbg("%-12s:%d\n","PRODUCT ID", sysinfo.product_id);
	
	/* read module SN */
	if('\0' != sysinfo.ax_sysinfo_module_serial_no[0]) 
	{
		ptr = (char *)malloc(SYSINFO_MODULE_SERIAL_NO+1);
		if(NULL == ptr) 
		{
			syslog_ax_eeprom_err("read chassis module sysinfo serial No. memory error!\n");
			return -1;
		}
		memset(ptr,0,SYSINFO_MODULE_SERIAL_NO+1);
		memcpy(ptr,sysinfo.ax_sysinfo_module_serial_no,SYSINFO_MODULE_SERIAL_NO+1);
		ptr[SYSINFO_MODULE_SERIAL_NO] = '\0';
		module->sn = ptr;	
	}
	/* read module name */
	if('\0' != sysinfo.ax_sysinfo_module_name[0]) 
	{
		ptr = (char *)malloc(SYSINFO_MODULE_NAME+1);
		if(NULL == ptr) 
		{
			syslog_ax_eeprom_err("read chassis module sysinfo name memory error!\n");
			return -1;
		}
		memset(ptr,0,SYSINFO_MODULE_NAME+1);
		strcpy(ptr,sysinfo.ax_sysinfo_module_name);	/* code optimize: Out-of-bounds access houxx@autelan.com  2013-1-20 */
		ptr[SYSINFO_MODULE_NAME] = '\0';
		module->modname = ptr;	
	}
#ifdef SYSINFO_OEM_BTO
	if(NULL != modnamefile)
	{
		if('\0' == sysinfo.ax_sysinfo_module_name[0])
		{
			ptr = NULL;
		    ptr = (char *)malloc(SYSINFO_MODULE_NAME+1);
			if(NULL == ptr) 
			{
				syslog_ax_eeprom_err("read chassis module sysinfo name memory error!\n");
				return -1;
			}
			module->modname = ptr;
			ptr = NULL;
		}
	    /* read OEM BTO  mac address*/
		fd = open(modnamefile,O_RDONLY);
		if(fd < 1) 
		{
			syslog_ax_eeprom_err("get oem bto sysinfo module name open error\r\n");
		}
		else 
		{
			read(fd, tmpModeName,SYSINFO_MODULE_NAME);
			/* override mac address read from eeprom*/
			if('\0' != tmpModeName[0])
			{
				if(((len = strlen(tmpModeName))>0)&&(tmpModeName[len-1]=='\n'))
				{
                      tmpModeName[len-1] = '\0';
				}
				memcpy(module->modname, tmpModeName, SYSINFO_MODULE_NAME);
				module->modname[SYSINFO_MODULE_NAME] = '\0';
			}
			close(fd);
		}
	}
#endif

	/* modify by qinhs@autelan 2009-11-18 for AX7605i-alpha */
	if(module->modname && ('\0' == module->modname[0])) 
	{
		if(MODULE_ID_AX7I_CRSMU == module->id) 
		{
			module->modname = DEFAULT_AX7KI_ALPHA_MAINBOARD_NAME;
		}
		else if(MODULE_ID_AX7I_GTX == module->id) 
		{
			module->modname = DEFAULT_AX7KI_ALPHA_GTX_NAME;
		}
#if 0
		else if(MODULE_ID_AX7I_XG_CONNECT == module->id) {
			module->modname = DEFAULT_AX7KI_ALPHA_XG_CONN_NAME;
		}
#endif
	}
	syslog_ax_eeprom_dbg("read chassis module sysinfo get system:\n");
	syslog_ax_eeprom_dbg("%-12s:%s\n","MODULE SN",module->sn);
	syslog_ax_eeprom_dbg("%-12s:%s\n","MODULE NAME",module->modname);
	return NPD_SUCCESS;
}

#define NBM_FILL_PRODUCT_SYSINFO { product_info->sysinfo.sn = DEFAULT_SYSSN;\
	product_info->sysinfo.name = DEFAULT_PRODUCT_NAME; \
	product_info->sysinfo.sw_name = DEFAULT_SW_NAME; \
	product_info->sysinfo.basemac = DEFAULT_BASEMAC; }


int nbm_read_backplane_sysinfo(struct product_s *product_info) {
    /* TODO read EEPROM and fill backplane sn and product sysinfo*/
	/*bm_op_backplane_sysinfo	 sysinfo;*/
	ax_product_sysinfo  sysinfo;
	int fd = -1;
	int result = 0;
	char *ptr = NULL;
	int len = 0;

	/* set default value*/
	product_info->chassis_backplane_sn = DEFAULT_BACKPLANE_SN;
	product_info->chassis_backplane_name = DEFAULT_BACKPLANE_NAME;
	NBM_FILL_PRODUCT_SYSINFO;
	
	memset(&sysinfo, 0, sizeof(ax_product_sysinfo));

	if(g_bm_fd < 0) 
	{
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			syslog_ax_eeprom_err("open dev %s error(%d) when read backplane sysinfo!\n",NPD_BM_FILE_PATH,fd);
		}
		g_bm_fd = fd;
	}
	else 
	{
		result = ioctl(g_bm_fd,BM_IOC_BACKPLANE_SYSINFO_READ,&sysinfo);
		if (result != 0) 
		{
			syslog_ax_eeprom_err("read backplane sysinfo error!\n");
		    memset(&sysinfo, 0, sizeof(ax_product_sysinfo));
		}
	}
	/* read module SN */
	ptr = (char *)malloc(SYSINFO_SN_LENGTH+1);
	if(NULL == ptr) 
	{
		syslog_ax_eeprom_err("read backplane sysinfo alloc SN memory error!\n");
		return -1;
	}
	memset(ptr,0,SYSINFO_SN_LENGTH+1);
	memcpy(ptr,sysinfo.ax_sysinfo_product_serial_no,SYSINFO_SN_LENGTH+1);
	ptr[SYSINFO_SN_LENGTH] = '\0';
	product_info->sysinfo.sn = ptr;
	/* read system base mac*/
	ptr = (char*)malloc(2*MAC_ADDRESS_LEN+1);
	if(NULL == ptr) 
	{
		syslog_ax_eeprom_err("read backplane sysinfo alloc MAC memory error\n");
		return -1;
	}
	memset(ptr,0,2*MAC_ADDRESS_LEN+1);
	memcpy(ptr,sysinfo.ax_sysinfo_product_base_mac_address,2*MAC_ADDRESS_LEN);
	ptr[2*MAC_ADDRESS_LEN] = '\0';
	product_info->sysinfo.basemac = ptr;

	/*read system name*/
	ptr = (char *)malloc(SYSINFO_PRODUCT_NAME+1);
	if(NULL == ptr) 
	{
		syslog_ax_eeprom_err("read backplane sysinfo alloc NAME memory error!\n");
		return -1;
	}
	memset(ptr,0,SYSINFO_PRODUCT_NAME+1);
	memcpy(ptr,sysinfo.ax_sysinfo_product_name,SYSINFO_PRODUCT_NAME+1);
	ptr[SYSINFO_PRODUCT_NAME] = '\0';
	product_info->sysinfo.name = ptr;


	/*read system sw_name*/
	ptr = (char *)malloc(SYSINFO_SOFTWARE_NAME+1);
	if(NULL == ptr) 
	{
		syslog_ax_eeprom_err("read backplane sysinfo alloc sw_name memory error!\n");
		return -1;
	}
	memset(ptr,0,SYSINFO_SOFTWARE_NAME+1);
	memcpy(ptr,sysinfo.ax_sysinfo_software_name,SYSINFO_SOFTWARE_NAME+1);
	ptr[SYSINFO_SOFTWARE_NAME] = '\0';
	product_info->sysinfo.sw_name = ptr;

	/*read system enterprise_name*/
	ptr = (char *)malloc(SYSINFO_ENTERPRISE_NAME+1);
	if(NULL == ptr)
	{
		syslog_ax_eeprom_err("read backplane sysinfo alloc enterprise_name memory error!\n");
		return -1;
	}
	memset(ptr,0,SYSINFO_ENTERPRISE_NAME+1);
	memcpy(ptr,sysinfo.ax_sysinfo_enterprise_name,SYSINFO_ENTERPRISE_NAME+1);
	ptr[SYSINFO_ENTERPRISE_NAME] = '\0';
	product_info->sysinfo.enterprise_name = ptr;

	/*read system enterprise_snmp_oid*/
	ptr = (char *)malloc(SYSINFO_ENTERPRISE_SNMP_OID+1);
	if(NULL == ptr)
	{
		syslog_ax_eeprom_err("read backplane sysinfo alloc enterprise_snmp_oid memory error!\n");
		return -1;
	}
	memset(ptr,0,SYSINFO_ENTERPRISE_SNMP_OID+1);
	memcpy(ptr,sysinfo.ax_sysinfo_enterprise_snmp_oid,SYSINFO_ENTERPRISE_SNMP_OID+1);
	ptr[SYSINFO_ENTERPRISE_SNMP_OID] = '\0';
	product_info->sysinfo.enterprise_snmp_oid = ptr;

	/*read system snmp_sys_oid*/
	ptr = (char *)malloc(SYSINFO_ENTERPRISE_SNMP_OID+1);
	if(NULL == ptr)
	{
		syslog_ax_eeprom_err("read backplane sysinfo alloc enterprise_snmp_oid memory error!\n");
		return -1;
	}
	memset(ptr,0,SYSINFO_ENTERPRISE_SNMP_OID+1);
	memcpy(ptr,sysinfo.ax_sysinfo_snmp_sys_oid,SYSINFO_ENTERPRISE_SNMP_OID+1);
	ptr[SYSINFO_ENTERPRISE_SNMP_OID] = '\0';
	product_info->sysinfo.snmp_sys_oid = ptr;

	/*read system built_in_admin_username*/
	ptr = (char *)malloc(SYSINFO_BUILT_IN_ADMIN_USERNAME+1);
	if(NULL == ptr) 
	{
		syslog_ax_eeprom_err("read backplane sysinfo alloc built_in_admin_username memory error!\n");
		return -1;
	}
	memset(ptr,0,SYSINFO_BUILT_IN_ADMIN_USERNAME+1);
	memcpy(ptr,sysinfo.ax_sysinfo_built_in_admin_username,SYSINFO_BUILT_IN_ADMIN_USERNAME+1);
	ptr[SYSINFO_BUILT_IN_ADMIN_USERNAME] = '\0';
	product_info->sysinfo.built_in_admin_username = ptr;

	/*read system built_in_admin_passwd*/
	ptr = (char *)malloc(SYSINFO_BUILT_IN_ADMIN_PASSWORD+1);
	if(NULL == ptr) 
	{
		syslog_ax_eeprom_err("read backplane sysinfo alloc built_in_admin_passwd memory error!\n");
		return -1;
	}
	memset(ptr,0,SYSINFO_BUILT_IN_ADMIN_PASSWORD+1);
	memcpy(ptr,sysinfo.ax_sysinfo_built_in_admin_password,SYSINFO_BUILT_IN_ADMIN_PASSWORD+1);
	ptr[SYSINFO_BUILT_IN_ADMIN_PASSWORD] = '\0';
	product_info->sysinfo.built_in_admin_passwd = ptr;

	ptr = NULL;
#ifdef SYSINFO_OEM_BTO
	memset(&g_oem_bto_sysinfo, 0, sizeof(SYSINFO_OEM_BTO_INFO));

	/* read OEM BTO  mac address*/
	fd = open(SYSINFO_OEM_BTO_MAC,O_RDONLY);
	if(fd < 1) 
	{
		syslog_ax_eeprom_err("get oem bto sysinfo mac open error\r\n");
		/* read OEM BTO  mac address form /proc/sysinfo/product_base_mac_addr*/
		fd = open(SYSINFO_OEM_BTO_PROC_MAC,O_RDONLY);
		if(fd < 1) 
		{
			syslog_ax_eeprom_err("get oem bto proc/sysinfo mac open error\r\n");
		}
		else 
		{
			read(fd, g_oem_bto_sysinfo.mac_addr,SYSINFO_OEM_BTO_LEN_MAC);
			/* override mac address read from eeprom*/
			if('\0' != g_oem_bto_sysinfo.mac_addr[0])
			{
				if(((len = strlen(g_oem_bto_sysinfo.mac_addr))>0)&&(g_oem_bto_sysinfo.mac_addr[len-1]=='\n'))
				{
                      g_oem_bto_sysinfo.mac_addr[len-1] = '\0';
				}
				memcpy(product_info->sysinfo.basemac, g_oem_bto_sysinfo.mac_addr, SYSINFO_OEM_BTO_LEN_MAC);
				product_info->sysinfo.basemac[SYSINFO_OEM_BTO_LEN_MAC] = '\0';
			}
			close(fd);
		}
	}
	else 
	{
		read(fd, g_oem_bto_sysinfo.mac_addr,SYSINFO_OEM_BTO_LEN_MAC);
		/* override mac address read from eeprom*/
		if('\0' != g_oem_bto_sysinfo.mac_addr[0]) 
		{
			if(((len = strlen(g_oem_bto_sysinfo.mac_addr))>0)&&(g_oem_bto_sysinfo.mac_addr[len-1]=='\n'))
			{
                  g_oem_bto_sysinfo.mac_addr[len-1] = '\0';
			}
			memcpy(product_info->sysinfo.basemac, g_oem_bto_sysinfo.mac_addr, SYSINFO_OEM_BTO_LEN_MAC);
			product_info->sysinfo.basemac[SYSINFO_OEM_BTO_LEN_MAC] = '\0';
		}
		else
		{
			close(fd);
			/* read OEM BTO  mac address form /proc/sysinfo/product_base_mac_addr*/
			fd = open(SYSINFO_OEM_BTO_PROC_MAC,O_RDONLY);
			if(fd < 1) 
			{
				syslog_ax_eeprom_err("get oem bto proc/sysinfo mac open error\r\n");
			}
			else 
			{
				read(fd, g_oem_bto_sysinfo.mac_addr,SYSINFO_OEM_BTO_LEN_MAC);
				/* override mac address read from eeprom*/
				if('\0' != g_oem_bto_sysinfo.mac_addr[0]) 
				{
					if(((len = strlen(g_oem_bto_sysinfo.mac_addr))>0)&&(g_oem_bto_sysinfo.mac_addr[len-1]=='\n'))
					{
	                      g_oem_bto_sysinfo.mac_addr[len-1] = '\0';
					}
					memcpy(product_info->sysinfo.basemac, g_oem_bto_sysinfo.mac_addr, SYSINFO_OEM_BTO_LEN_MAC);
					product_info->sysinfo.basemac[SYSINFO_OEM_BTO_LEN_MAC] = '\0';
				}
				close(fd);/* code optimize:Argument cannot be negative houxx@autelan.com  2013-1-20 */
			}	
		}		
	}
	/*read OEM BTO product name */
	fd = open(SYSINFO_OEM_BTO_PRODUCT_NAME,O_RDONLY);
	if(fd < 1) 
	{
		syslog_ax_eeprom_err("get oem bto sysinfo product name open error\r\n");
	}
	else 
	{
		read(fd, g_oem_bto_sysinfo.product_name,SYSINFO_OEM_BTO_LEN_PRODUCT_NAME);
		/* override product name read from eeprom*/
		if('\0' != g_oem_bto_sysinfo.product_name[0]) {
			if(((len = strlen(g_oem_bto_sysinfo.product_name))>0)&&(g_oem_bto_sysinfo.product_name[len-1]=='\n'))
			{
                  g_oem_bto_sysinfo.product_name[len-1] = '\0';
			}
			memcpy(product_info->sysinfo.name, g_oem_bto_sysinfo.product_name, SYSINFO_OEM_BTO_LEN_PRODUCT_NAME);
			product_info->sysinfo.name[SYSINFO_OEM_BTO_LEN_PRODUCT_NAME] = '\0';
		}
		close(fd); /* code optimize: recourse leak houxx@autelan.com  2013-1-20 */
	}
	/*read OEM BTO sotfware name */
	fd = open(SYSINFO_OEM_BTO_SW_NAME,O_RDONLY);
	if(fd < 1) 
	{
		syslog_ax_eeprom_err("get oem bto sysinfo software name open error\r\n");
	}
	else 
	{
		read(fd, g_oem_bto_sysinfo.sw_name,SYSINFO_OEM_BTO_LEN_SW_NAME);
		/* override software name read from eeprom*/
		if('\0' != g_oem_bto_sysinfo.sw_name[0])
		{
			if(((len = strlen(g_oem_bto_sysinfo.sw_name))>0)&&(g_oem_bto_sysinfo.sw_name[len-1]=='\n'))
			{
                  g_oem_bto_sysinfo.sw_name[len-1] = '\0';
			}
			memcpy(product_info->sysinfo.sw_name, g_oem_bto_sysinfo.sw_name, SYSINFO_OEM_BTO_LEN_SW_NAME);
			product_info->sysinfo.sw_name[SYSINFO_OEM_BTO_LEN_SW_NAME] = '\0';
		}
		close(fd);
	}	
	/*read OEM BTO enterprise name*/
	fd = open(SYSINFO_OEM_BTO_VENDOR_NAME,O_RDONLY);
	if(fd < 1) 
	{
		syslog_ax_eeprom_err("get oem bto sysinfo vendor name open error\r\n");
	}
	else 
	{
		read(fd, g_oem_bto_sysinfo.enterprise_name,SYSINFO_OEM_BTO_LEN_VENDOR_NAME);
		/* override enterprise name read from eeprom*/
		if('\0' != g_oem_bto_sysinfo.enterprise_name[0])
		{
			if(((len = strlen(g_oem_bto_sysinfo.enterprise_name))>0)&&(g_oem_bto_sysinfo.enterprise_name[len-1]=='\n'))
			{
                  g_oem_bto_sysinfo.enterprise_name[len-1] = '\0';
			}
			memcpy(product_info->sysinfo.enterprise_name, g_oem_bto_sysinfo.enterprise_name, SYSINFO_OEM_BTO_LEN_VENDOR_NAME);
			product_info->sysinfo.enterprise_name[SYSINFO_OEM_BTO_LEN_VENDOR_NAME] = '\0';
		}
		close(fd);
	}	
	/*read OEM BTO enterprise snmp oid */
	fd = open(SYSINFO_OEM_BTO_VENDOR_SNMP_OID,O_RDONLY);
	if(fd < 1) 
	{
		syslog_ax_eeprom_err("get oem bto sysinfo vendor snmp oid open error\r\n");
	}
	else 
	{
		read(fd, g_oem_bto_sysinfo.enterprise_snmp_oid,SYSINFO_OEM_BTO_LEN_VENDOR_SNMP_OID);
		/* override enterprise snmp oid read from eeprom*/
		if('\0' != g_oem_bto_sysinfo.enterprise_snmp_oid[0]) 
		{
			if(((len = strlen(g_oem_bto_sysinfo.enterprise_snmp_oid))>0)&&(g_oem_bto_sysinfo.enterprise_snmp_oid[len-1]=='\n'))
			{
                  g_oem_bto_sysinfo.enterprise_snmp_oid[len-1] = '\0';
			}
			memcpy(product_info->sysinfo.enterprise_snmp_oid, g_oem_bto_sysinfo.enterprise_snmp_oid, SYSINFO_OEM_BTO_LEN_VENDOR_SNMP_OID);
			product_info->sysinfo.enterprise_snmp_oid[SYSINFO_OEM_BTO_LEN_VENDOR_SNMP_OID] = '\0';
		}
		close(fd);
	}
	/*read OEM BTO snmp sys oid */
	fd = open(SYSINFO_OEM_BTO_SNMP_SYS_OID,O_RDONLY);
	if(fd < 1) 
	{
		syslog_ax_eeprom_err("get oem bto sysinfo snmp sys oid open error\r\n");
	}
	else 
	{
		read(fd, g_oem_bto_sysinfo.snmp_sys_oid,SYSINFO_OEM_BTO_LEN_SNMP_SYS_OID);
		/* override snmp sys oid read from eeprom*/
		if('\0' != g_oem_bto_sysinfo.snmp_sys_oid[0]) 
		{
			if(((len = strlen(g_oem_bto_sysinfo.snmp_sys_oid))>0)&&(g_oem_bto_sysinfo.snmp_sys_oid[len-1]=='\n'))
			{
                  g_oem_bto_sysinfo.snmp_sys_oid[len-1] = '\0';
			}
			memcpy(product_info->sysinfo.snmp_sys_oid, g_oem_bto_sysinfo.snmp_sys_oid, SYSINFO_OEM_BTO_LEN_SNMP_SYS_OID);
			product_info->sysinfo.snmp_sys_oid[SYSINFO_OEM_BTO_LEN_SNMP_SYS_OID] = '\0';
		}
		close(fd);
	}

#endif 
	syslog_ax_eeprom_dbg("read EEPROM get sysinfo:\n");
	syslog_ax_eeprom_dbg("%-12s:%s\n","PRODUCT NAME",product_info->sysinfo.name);
	syslog_ax_eeprom_dbg("%-12s:%s\n","SYSTEM MAC",product_info->sysinfo.basemac);
	syslog_ax_eeprom_dbg("%-12s:%s\n","SERIAL No.",product_info->sysinfo.sn);
	syslog_ax_eeprom_dbg("%-12s:%s\n","SOFTWARE ID",product_info->sysinfo.sw_name);
	syslog_ax_eeprom_dbg("%-12s:%s\n","VENDOR NAME",product_info->sysinfo.enterprise_name);
	syslog_ax_eeprom_dbg("%-12s:%s\n","SYSTEM OID",product_info->sysinfo.snmp_sys_oid);
	syslog_ax_eeprom_dbg("%-12s:%s\n","SNMP OID",product_info->sysinfo.enterprise_snmp_oid);
	syslog_ax_eeprom_dbg("%-12s:%s\n","USERNAME",product_info->sysinfo.built_in_admin_username);
	syslog_ax_eeprom_dbg("%-12s:%s\n","PASSWORD",product_info->sysinfo.built_in_admin_passwd);

	return NPD_SUCCESS;
}

int nbm_read_mainboard_sysinfo(struct product_s *product_info, struct module_info_s *module) {
    /* TODO read EEPROM and fill product sysinfo*/
	ax_product_sysinfo  sysinfo;
	int fd = -1;
	int result = 0;
	char *ptr = NULL;
	int len = 0;

	/* set default value*/
	module->sn = DEFAULT_MAINBOARD_SN;
	module->modname = DEFAULT_MAINBOARD_NAME;
	NBM_FILL_PRODUCT_SYSINFO

	if(g_bm_fd < 0)
	{
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			syslog_ax_eeprom_err("open dev %s error(%d) when read main board sysinfo!\n",NPD_BM_FILE_PATH,fd);
			return -1;
		}
		g_bm_fd = fd;
	}
	memset(&sysinfo, 0, sizeof(ax_product_sysinfo));
	
	result = ioctl(g_bm_fd,BM_IOC_MAINBOARD_SYSINFO_READ,&sysinfo);
	if (result != 0) 
	{
		syslog_ax_eeprom_err("read mainboard sysinfo error!,errno %d\n",errno);
		return -1;
	}
	/* read module SN */
	ptr = (char *)malloc(SYSINFO_SN_LENGTH+1);
	if(NULL == ptr)
	{
		syslog_ax_eeprom_err("read mainboard sysinfo alloc SN memory error!\n");
		return -1;
	}
	memset(ptr,0,SYSINFO_SN_LENGTH+1);
	memcpy(ptr,sysinfo.ax_sysinfo_product_serial_no,SYSINFO_SN_LENGTH+1);
	ptr[SYSINFO_SN_LENGTH] = '\0';
	product_info->sysinfo.sn = ptr;
	module->sn = ptr;

	/* read system base mac*/
	ptr = (char*)malloc(MAC_ADDRESS_LEN+1);
	if(NULL == ptr) 
	{
		syslog_ax_eeprom_err("read mainboard sysinfo alloc MAC memory error\n");
		return -1;
	}
	memset(ptr,0,2*MAC_ADDRESS_LEN+1);
	memcpy(ptr,sysinfo.ax_sysinfo_product_base_mac_address,2*MAC_ADDRESS_LEN);
	ptr[2*MAC_ADDRESS_LEN] = '\0';
	product_info->sysinfo.basemac = ptr;

	/* TODO:new sysinfo change will append here!!!*/
	/*read system name*/
	ptr = (char *)malloc(SYSINFO_PRODUCT_NAME+1);
	if(NULL == ptr) 
	{
		syslog_ax_eeprom_err("read mainboard sysinfo alloc NAME memory error!\n");
		return -1;
	}
	memset(ptr,0,SYSINFO_PRODUCT_NAME+1);
	memcpy(ptr,sysinfo.ax_sysinfo_product_name,SYSINFO_PRODUCT_NAME+1);
	ptr[SYSINFO_PRODUCT_NAME] = '\0';
	product_info->sysinfo.name = ptr;
	module->modname = ptr;


	/*read system sw_name*/
	ptr = (char *)malloc(SYSINFO_SOFTWARE_NAME+1);
	if(NULL == ptr) 
	{
		syslog_ax_eeprom_err("read mainboard sysinfo alloc sw_name memory error!\n");
		return -1;
	}
	memset(ptr,0,SYSINFO_SOFTWARE_NAME+1);
	memcpy(ptr,sysinfo.ax_sysinfo_software_name,SYSINFO_SOFTWARE_NAME+1);
	ptr[SYSINFO_SOFTWARE_NAME] = '\0';
	product_info->sysinfo.sw_name = ptr;

	/*read system enterprise_name*/
	ptr = (char *)malloc(SYSINFO_ENTERPRISE_NAME+1);
	if(NULL == ptr) 
	{
		syslog_ax_eeprom_err("read mainboard sysinfo alloc enterprise_name memory error!\n");
		return -1;
	}
	memset(ptr,0,SYSINFO_ENTERPRISE_NAME+1);
	memcpy(ptr,sysinfo.ax_sysinfo_enterprise_name,SYSINFO_ENTERPRISE_NAME+1);
	ptr[SYSINFO_ENTERPRISE_NAME] = '\0';
	product_info->sysinfo.enterprise_name = ptr;

	/*read system enterprise_snmp_oid*/
	ptr = (char *)malloc(SYSINFO_ENTERPRISE_SNMP_OID+1);
	if(NULL == ptr) 
	{
		syslog_ax_eeprom_err("read mainboard sysinfo alloc enterprise_snmp_oid memory error!\n");
		return -1;
	}
	memset(ptr,0,SYSINFO_ENTERPRISE_SNMP_OID+1);
	memcpy(ptr,sysinfo.ax_sysinfo_enterprise_snmp_oid,SYSINFO_ENTERPRISE_SNMP_OID+1);
	ptr[SYSINFO_ENTERPRISE_SNMP_OID] = '\0';
	product_info->sysinfo.enterprise_snmp_oid = ptr;

	/*read system snmp_sys_oid*/
	ptr = (char *)malloc(SYSINFO_ENTERPRISE_SNMP_OID+1);
	if(NULL == ptr) 
	{
		syslog_ax_eeprom_err("read mainboard sysinfo alloc enterprise_snmp_oid memory error!\n");
		return -1;
	}
	memset(ptr,0,SYSINFO_ENTERPRISE_SNMP_OID+1);
	memcpy(ptr,sysinfo.ax_sysinfo_snmp_sys_oid,SYSINFO_ENTERPRISE_SNMP_OID+1);
	ptr[SYSINFO_ENTERPRISE_SNMP_OID] = '\0';
	product_info->sysinfo.snmp_sys_oid = ptr;

	/*read system built_in_admin_username*/
	ptr = (char *)malloc(SYSINFO_BUILT_IN_ADMIN_USERNAME+1);
	if(NULL == ptr) 
	{
		syslog_ax_eeprom_err("read mainboard sysinfo alloc built_in_admin_username memory error!\n");
		return -1;
	}
	memset(ptr,0,SYSINFO_BUILT_IN_ADMIN_USERNAME+1);
	memcpy(ptr,sysinfo.ax_sysinfo_built_in_admin_username,SYSINFO_BUILT_IN_ADMIN_USERNAME+1);
	ptr[SYSINFO_BUILT_IN_ADMIN_USERNAME] = '\0';
	product_info->sysinfo.built_in_admin_username = ptr;

	/*read system built_in_admin_passwd*/
	ptr = (char *)malloc(SYSINFO_BUILT_IN_ADMIN_PASSWORD+1);
	if(NULL == ptr) 
	{
		syslog_ax_eeprom_err("read mainboard sysinfo alloc built_in_admin_passwd memory error!\n");
		return -1;
	}
	memset(ptr,0,SYSINFO_BUILT_IN_ADMIN_PASSWORD+1);
	memcpy(ptr,sysinfo.ax_sysinfo_built_in_admin_password,SYSINFO_BUILT_IN_ADMIN_PASSWORD+1);
	ptr[SYSINFO_BUILT_IN_ADMIN_PASSWORD] = '\0';
	product_info->sysinfo.built_in_admin_passwd = ptr;

	ptr = NULL;

#ifdef SYSINFO_OEM_BTO
		memset(&g_oem_bto_sysinfo, 0, sizeof(SYSINFO_OEM_BTO_INFO));
	
		/* read OEM BTO  mac address*/
		fd = open(SYSINFO_OEM_BTO_MAC,O_RDONLY);
		if(fd < 1) 
		{
			syslog_ax_eeprom_err("get oem bto sysinfo mac open error\r\n");
			/* read OEM BTO  mac address form /proc/sysinfo/product_base_mac_addr*/
			fd = open(SYSINFO_OEM_BTO_PROC_MAC,O_RDONLY);
			if(fd < 1) 
			{
				syslog_ax_eeprom_err("get oem bto proc/sysinfo mac open error\r\n");
			}
			else 
			{
				read(fd, g_oem_bto_sysinfo.mac_addr,SYSINFO_OEM_BTO_LEN_MAC);
				/* override mac address read from eeprom*/
				if('\0' != g_oem_bto_sysinfo.mac_addr[0])
				{
					if(((len = strlen(g_oem_bto_sysinfo.mac_addr))>0)&&(g_oem_bto_sysinfo.mac_addr[len-1]=='\n'))
					{
						  g_oem_bto_sysinfo.mac_addr[len-1] = '\0';
					}
					memcpy(product_info->sysinfo.basemac, g_oem_bto_sysinfo.mac_addr, SYSINFO_OEM_BTO_LEN_MAC);
					product_info->sysinfo.basemac[SYSINFO_OEM_BTO_LEN_MAC] = '\0';
				}
				close(fd);
			}
		}
		else 
		{
			read(fd, g_oem_bto_sysinfo.mac_addr,SYSINFO_OEM_BTO_LEN_MAC);
			/* override mac address read from eeprom*/
			if('\0' != g_oem_bto_sysinfo.mac_addr[0]) 
			{
				if(((len = strlen(g_oem_bto_sysinfo.mac_addr))>0)&&(g_oem_bto_sysinfo.mac_addr[len-1]=='\n'))
				{
					  g_oem_bto_sysinfo.mac_addr[len-1] = '\0';
				}
				memcpy(product_info->sysinfo.basemac, g_oem_bto_sysinfo.mac_addr, SYSINFO_OEM_BTO_LEN_MAC);
				product_info->sysinfo.basemac[SYSINFO_OEM_BTO_LEN_MAC] = '\0';
			}
			else
			{
				close(fd);
				/* read OEM BTO  mac address form /proc/sysinfo/product_base_mac_addr*/
				fd = open(SYSINFO_OEM_BTO_PROC_MAC,O_RDONLY);
				if(fd < 1) 
				{
					syslog_ax_eeprom_err("get oem bto proc/sysinfo mac open error\r\n");
				}
				else 
				{
					read(fd, g_oem_bto_sysinfo.mac_addr,SYSINFO_OEM_BTO_LEN_MAC);
					/* override mac address read from eeprom*/
					if('\0' != g_oem_bto_sysinfo.mac_addr[0]) 
					{
						if(((len = strlen(g_oem_bto_sysinfo.mac_addr))>0)&&(g_oem_bto_sysinfo.mac_addr[len-1]=='\n'))
						{
							  g_oem_bto_sysinfo.mac_addr[len-1] = '\0';
						}
						memcpy(product_info->sysinfo.basemac, g_oem_bto_sysinfo.mac_addr, SYSINFO_OEM_BTO_LEN_MAC);
						product_info->sysinfo.basemac[SYSINFO_OEM_BTO_LEN_MAC] = '\0';
					}
					close(fd);	/* code optimize:Argument cannot be negative houxx@autelan.com  2013-1-20 */
				}	
			}		

		}
		/*read OEM BTO product name */
		fd = open(SYSINFO_OEM_BTO_PRODUCT_NAME,O_RDONLY);
		if(fd < 1) 
		{
			syslog_ax_eeprom_err("get oem bto sysinfo product name open error\r\n");
		}
		else 
		{
			read(fd, g_oem_bto_sysinfo.product_name,SYSINFO_OEM_BTO_LEN_PRODUCT_NAME);
			/* override product name read from eeprom*/
			if('\0' != g_oem_bto_sysinfo.product_name[0]) 
			{
				if(((len = strlen(g_oem_bto_sysinfo.product_name))>0)&&(g_oem_bto_sysinfo.product_name[len-1]=='\n'))
				{
					  g_oem_bto_sysinfo.product_name[len-1] = '\0';
				}
				memcpy(product_info->sysinfo.name, g_oem_bto_sysinfo.product_name, SYSINFO_OEM_BTO_LEN_PRODUCT_NAME);
				product_info->sysinfo.name[SYSINFO_OEM_BTO_LEN_PRODUCT_NAME] = '\0';
			}
		}
		/*read OEM BTO sotfware name */
		fd = open(SYSINFO_OEM_BTO_SW_NAME,O_RDONLY);
		if(fd < 1) 
		{
			syslog_ax_eeprom_err("get oem bto sysinfo software name open error\r\n");
		}
		else 
		{
			read(fd, g_oem_bto_sysinfo.sw_name,SYSINFO_OEM_BTO_LEN_SW_NAME);
			/* override software name read from eeprom*/
			if('\0' != g_oem_bto_sysinfo.sw_name[0]) 
			{
				if(((len = strlen(g_oem_bto_sysinfo.sw_name))>0)&&(g_oem_bto_sysinfo.sw_name[len-1]=='\n'))
				{
					  g_oem_bto_sysinfo.sw_name[len-1] = '\0';
				}
				memcpy(product_info->sysinfo.sw_name, g_oem_bto_sysinfo.sw_name, SYSINFO_OEM_BTO_LEN_SW_NAME);
				product_info->sysinfo.sw_name[SYSINFO_OEM_BTO_LEN_SW_NAME] = '\0';
			}
			close(fd);
		}	
		/*read OEM BTO enterprise name*/
		fd = open(SYSINFO_OEM_BTO_VENDOR_NAME,O_RDONLY);
		if(fd < 1) 
		{
			syslog_ax_eeprom_err("get oem bto sysinfo vendor name open error\r\n");
		}
		else 
		{
			read(fd, g_oem_bto_sysinfo.enterprise_name,SYSINFO_OEM_BTO_LEN_VENDOR_NAME);
			/* override enterprise name read from eeprom*/
			if('\0' != g_oem_bto_sysinfo.enterprise_name[0]) 
			{
				if(((len = strlen(g_oem_bto_sysinfo.enterprise_name))>0)&&(g_oem_bto_sysinfo.enterprise_name[len-1]=='\n'))
				{
					  g_oem_bto_sysinfo.enterprise_name[len-1] = '\0';
				}
				memcpy(product_info->sysinfo.enterprise_name, g_oem_bto_sysinfo.enterprise_name, SYSINFO_OEM_BTO_LEN_VENDOR_NAME);
				product_info->sysinfo.enterprise_name[SYSINFO_OEM_BTO_LEN_VENDOR_NAME] = '\0';
			}
			close(fd);
		}	
		/*read OEM BTO enterprise snmp oid */
		fd = open(SYSINFO_OEM_BTO_VENDOR_SNMP_OID,O_RDONLY);
		if(fd < 1) 
		{
			syslog_ax_eeprom_err("get oem bto sysinfo vendor snmp oid open error\r\n");
		}
		else 
		{
			read(fd, g_oem_bto_sysinfo.enterprise_snmp_oid,SYSINFO_OEM_BTO_LEN_VENDOR_SNMP_OID);
			/* override enterprise snmp oid read from eeprom*/
			if('\0' != g_oem_bto_sysinfo.enterprise_snmp_oid[0]) 
			{
				if(((len = strlen(g_oem_bto_sysinfo.enterprise_snmp_oid))>0)&&(g_oem_bto_sysinfo.enterprise_snmp_oid[len-1]=='\n'))
				{
					  g_oem_bto_sysinfo.enterprise_snmp_oid[len-1] = '\0';
				}
				memcpy(product_info->sysinfo.enterprise_snmp_oid, g_oem_bto_sysinfo.enterprise_snmp_oid, SYSINFO_OEM_BTO_LEN_VENDOR_SNMP_OID);
				product_info->sysinfo.enterprise_snmp_oid[SYSINFO_OEM_BTO_LEN_VENDOR_SNMP_OID] = '\0';
			}
			close(fd);
		}
		/*read OEM BTO snmp sys oid */
		fd = open(SYSINFO_OEM_BTO_SNMP_SYS_OID,O_RDONLY);
		if(fd < 1)
		{
			syslog_ax_eeprom_err("get oem bto sysinfo snmp sys oid open error\r\n");
		}
		else 
		{
			read(fd, g_oem_bto_sysinfo.snmp_sys_oid,SYSINFO_OEM_BTO_LEN_SNMP_SYS_OID);
			/* override snmp sys oid read from eeprom*/
			if('\0' != g_oem_bto_sysinfo.snmp_sys_oid[0]) 
			{
				if(((len = strlen(g_oem_bto_sysinfo.snmp_sys_oid))>0)&&(g_oem_bto_sysinfo.snmp_sys_oid[len-1]=='\n'))
				{
					  g_oem_bto_sysinfo.snmp_sys_oid[len-1] = '\0';
				}
				memcpy(product_info->sysinfo.snmp_sys_oid, g_oem_bto_sysinfo.snmp_sys_oid, SYSINFO_OEM_BTO_LEN_SNMP_SYS_OID);
				product_info->sysinfo.snmp_sys_oid[SYSINFO_OEM_BTO_LEN_SNMP_SYS_OID] = '\0';
			}
			close(fd);
		}
	
#endif 

	syslog_ax_eeprom_dbg("read mainboard sysinfo get system:\n");
	syslog_ax_eeprom_dbg("%-12s:%s\n","PRODUCT NAME",product_info->sysinfo.name);
	syslog_ax_eeprom_dbg("%-12s:%s\n","SYSTEM MAC",product_info->sysinfo.basemac);
	syslog_ax_eeprom_dbg("%-12s:%s\n","SERIAL No.",product_info->sysinfo.sn);
	syslog_ax_eeprom_dbg("%-12s:%s\n","SOFTWARE ID",product_info->sysinfo.sw_name);
	syslog_ax_eeprom_dbg("%-12s:%s\n","VENDOR NAME",product_info->sysinfo.enterprise_name);
	syslog_ax_eeprom_dbg("%-12s:%s\n","SYSTEM OID",product_info->sysinfo.snmp_sys_oid);
	syslog_ax_eeprom_dbg("%-12s:%s\n","SNMP OID",product_info->sysinfo.enterprise_snmp_oid);
	syslog_ax_eeprom_dbg("%-12s:%s\n","USERNAME",product_info->sysinfo.built_in_admin_username);
	syslog_ax_eeprom_dbg("%-12s:%s\n","PASSWORD",product_info->sysinfo.built_in_admin_passwd);
	return NPD_SUCCESS;
}
#ifdef __cplusplus
}
#endif
