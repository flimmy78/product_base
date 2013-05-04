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
* npd_product.c
*
*
* CREATOR:
*		chenb@autelan.com
*
* DESCRIPTION:
*		APIs used in NPD for product related routine.
*
* DATE:
*		02/21/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.79 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdlib.h>
#include <string.h>
#ifdef CPU_ARM_XCAT
#include <signal.h>
#else
#include <asm/signal.h>
#endif
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "util/npd_list.h"
#include "sysdef/npd_sysdef.h"
#include "sysdef/returncode.h"
#include "dbus/npd/npd_dbus_def.h"
#include "npd/nbm/npd_bmapi.h"
#include "npd/nam/npd_amapi.h"
#include "npd_product.h"
#include "npd_log.h"
#include "nbm/nbm_api.h"
#include "npd_c_slot.h"
#include "npd_main.h"
#include "npd_eth_port.h"
#include "npd_vlan.h"
#include "dbus/npd/npd_dbus_def.h"


struct product_s productinfo;

struct system_state_s	systemStateInfo;

struct oam_ctrl_s	g_oam_infos[OAM_TRAFFIC_TYPE_MAX] = {	\
	{OAM_TELNET_DPORT_DEFAULT, NPD_TRUE, 0},
	{OAM_SSH_DPORT_DEFAULT, NPD_TRUE, 0},
	{OAM_HTTP_DPORT_DEFAULT, NPD_TRUE, 0},
	{OAM_HTTPS_DPORT_DEFAULT, NPD_TRUE, 0},
	{OAM_SNMP_DPORT_DEFAULT, NPD_TRUE, 0},
	{WIRELESS_CAPWAP_CNTL_DEFAULT, NPD_TRUE, 0},
	{WIRELESS_CAPWAP_DATA_DEFAULT, NPD_TRUE, 0}
};

/*product's function that hardware all supportted */
NPD_PRODUCT_FUNCTION product_cap_mib[] = { \
	/* PRODUCT_ID_NONE */
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	/* PRODUCT_ID_AX7K */
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	/* PRODUCT_ID_AX7K_I */
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	/* PRODUCT_ID_AX5K */
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	/* PRODUCT_ID_AX5K_I */
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	/* PRODUCT_ID_AX5K_E */
	{0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0},
	/* PRODUCT_ID_AU4K */
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	/* PRODUCT_ID_AU3K */
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	/* PRODUCT_ID_AU3K_BCM */
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	/* PRODUCT_ID_AU3K_BCAT */
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	/* PRODUCT_ID_AU2K_TCAT*/
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	/* PRODUCT_ID_AX5608_E */
	{0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0},
    /* PRODUCT_ID_AX8610 */
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};


/*function what we need the product support*/
NPD_PRODUCT_FUNCTION function_run_map[] = { \
	/* PRODUCT_ID_NONE */
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	/* PRODUCT_ID_AX7K */
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	/* PRODUCT_ID_AX7K_I */
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	/* PRODUCT_ID_AX5K */
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	/* PRODUCT_ID_AX5K_I */
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	/* PRODUCT_ID_AX5K_E */
	{0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0},
	/* PRODUCT_ID_AU4K */
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	/* PRODUCT_ID_AU3K */
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	/* PRODUCT_ID_AU3K_BCM */
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	/* PRODUCT_ID_AU3K_BCAT */
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	/* PRODUCT_ID_AU2K_TCAT*/
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	/* PRODUCT_ID_AX5608_E */
	{0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0},
    /* PRODUCT_ID_AX8610 */
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};


static char * oam_traff_desc[OAM_TRAFFIC_TYPE_MAX] = {	\
	"TELNET",
	"SSH",
	"HTTP",
	"HTTPS",
	"SNMP",
	"CAPWAPc",
	"CAPWAPd"
};

/* signal number for device traffic enable */
#define APP_DEMO_TRAFFIC_ENABLE_SIGNUM	(SIGRTMIN+1)

/* file to save signal handler thread pid */
#define NPD_SIGHANDLER_PID_PATH 	"/var/run/npd_sig.pid"

/* NPD startup state file descriptor*/
int aw_state_fd = -1;
int board_state_fd = -1;

/* NPD startup state*/
extern int npd_startup_end;

extern unsigned int g_spi_port_index;

/* file to save system startup state */
#define NPD_SYSTEM_STARTUP_STATE_PATH	"/var/run/aw.state"

/* file to save single board startup state */
#define NPD_BOARD_STARTUP_STATE_PATH	"/var/run/board.state"

/* file to save NPD initialization stage: this is used by rc script to wait for npd initialization done. */
#define NPD_INIT_STAGE_FILE_PATH	"/var/run/asic.state"

/* file to save all npd thread pid */
#define NPD_ALL_THREAD_PID_PATH	"/var/run/npd_all.pid"

#define MAC_ALEN	6

/* thread count in NPD process - used under arm product e.g. XCAT series */
#ifdef CPU_ARM_XCAT
unsigned int g_npd_thread_number = 0;
#endif
/*
  NOTE list product type data according to npd_sysdef.h and product definition
*/

struct product_type_data_s product_type_mib[] = {
/*  chassis_slot_count, chassis_slot_start_no, chassis_subcard_count*/
	{0,0,0},  /* PRODUCT_ID_NONE*/
	{5,0,0},	/* PRODUCT_ID_AX7K*/
	{2,0,1},	/* PRODUCT_ID_AX5K*/
	{2,0,2},	/* PRODUCT_ID_AU4K*/
	{1,1,0},	/* PRODUCT_ID_AU3K*/
	{1,1,0},	/* PRODUCT_ID_AU3K_BCM*/
	{1,2,0},  /* PRODUCT_ID_AU3K_BCAT */
	{1,1,0},	/* PRODUCT_ID_AU2K_TCAT */
	{2,0,1},	/* PRODUCT_ID_AX5K_I*/
	{3,0,0},	/* PRODUCT_ID_AX7K_I */
	{1,1,0},	/* PRODUCT_ID_AX5K_E*/
	{1,1,0},	/* PRODUCT_ID_AX5608 */
    {1,1,0},	/* PRODUCT_ID_AX8610 */
};

struct product_type_data_s*(*product_type_data)(unsigned int);

struct product_type_data_s *product_type_data_old
(
	unsigned int productId
)
{
	switch(productId) {
		case PRODUCT_ID_NONE:
		default:
			return &product_type_mib[0];
		case PRODUCT_ID_AX7K:
			return &product_type_mib[1];
		case PRODUCT_ID_AX5K:
			return &product_type_mib[2];
		case PRODUCT_ID_AU4K:
			return &product_type_mib[3];
		case PRODUCT_ID_AU3K:
			return &product_type_mib[4];
		case PRODUCT_ID_AU3K_BCM:
			return &product_type_mib[5];
		case PRODUCT_ID_AU3K_BCAT:
			return &product_type_mib[6];
		case PRODUCT_ID_AU2K_TCAT:
			return &product_type_mib[7];
		case PRODUCT_ID_AX5K_I:
			return &product_type_mib[8];
		case PRODUCT_ID_AX7K_I:
			return &product_type_mib[9];
		case PRODUCT_ID_AX5K_E:
			return &product_type_mib[10];
		case PRODUCT_ID_AX5608:
			return &product_type_mib[11];
        case PRODUCT_ID_AX8600:
			return &product_type_mib[12];

	}
}

int npd_init_productinfo_old(void) {
	
	memset((void*)(&productinfo),0,sizeof(struct product_s));

	/*
	Read cpld and eeprom to get product information
	*/
	
	nbm_init_productinfo(&productinfo);
    
    syslog_ax_product_dbg("PRODUCT_ID = %d, LOCAL_MODULE_ID = %d\n", PRODUCT_ID, LOCAL_MODULE_ID);
	syslog_ax_product_dbg("product id %s local module id (%#x)%s hw version %d\n", 	\
			product_id_str(PRODUCT_ID),LOCAL_MODULE_ID,module_id_str(LOCAL_MODULE_ID), LOCAL_MODULE_HW_VERSION);
	/* init local board slot number info */
	nbm_init_system_localboard_slotno(PRODUCT_ID);
	
	npd_product_support_function_init(&productinfo); /*get product's function map to choose matching function*/
	return 0;
}

enum product_id_e npd_query_product_id(void) {
	return PRODUCT_ID;
}



struct product_sysinfo_s *npd_query_sysinfo(void) {
	return &(productinfo.sysinfo);
}

char *npd_query_product_serialno(void) {
	return productinfo.sysinfo.sn;
}

char *npd_query_product_basemac(void) {
	return productinfo.sysinfo.basemac;
}

char *npd_query_sw_name(void) {
	return productinfo.sysinfo.sw_name;
}

char *npd_query_enterprise_name(void) {
	return productinfo.sysinfo.enterprise_name;
}

char *npd_query_enterprise_snmp_oid(void) {
	return productinfo.sysinfo.enterprise_snmp_oid;
}

char *npd_query_snmp_sysid(void) {
	return productinfo.sysinfo.snmp_sys_oid;
}

char *npd_query_built_in_username(void) {
	return productinfo.sysinfo.built_in_admin_username;
}

char *npd_query_builnpd_query_built_in_passwd(void) {
	return productinfo.sysinfo.built_in_admin_passwd;
}

/**********************************************************************************
 * npd_system_verify_basemac
 * 
 * Verify if the given mac address is the same as system base mac address
 *
 *	INPUT:
 *		macAddr - mac address will compare to system base mac address
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		-1 - if parameters illegall.
 *		-2 - input parameter to long.
 *		0 - if the given mac address is not the same as system base mac address.
 *		1 - if the given mac address is the same as system base mac address.
 *
 *	NOTATION:
 *		input parameter macAddr is a ASCII code formatted MAC address, such as 
 *		"001122334455" stands for mac address 00:11:22:33:44:55 or 00-11-22-33-44-55
 *		
 **********************************************************************************/
int npd_system_verify_basemac
(
	char *macAddr
)
{
	unsigned char *sysMac = NULL;

	sysMac = (unsigned char*)PRODUCT_MAC_ADDRESS;

	if(!sysMac || !macAddr) {
		return -1;
	}

	if(strlen(macAddr) > 12) {
		return -2;
	}

	/*codeby zhengcs@autelan.com at 20081205 for no care capital and lowercase!!!*/
	if(!strncasecmp(macAddr,sysMac,12)) {
	   return 1;
	}
	else {
		return 0;
	}
}

/**********************************************************************************
 * npd_system_get_basemac
 * 
 * 	Get system mac address in Hex format
 *
 *	INPUT:
 *		size - macAddr buffer size
 *	
 *	OUTPUT:
 *		macAddr - mac address will be returned back
 *
 * 	RETURN:
 *		-1 - if mac address buffer size too small.
 *		-2 - illegal character found.
 *		0 - if no error occur
 *
 *	NOTATION:
 *		system mac address is a ASCII code formatted MAC address, such as 
 *		"001122334455" stands for mac address 00:11:22:33:44:55 or 00-11-22-33-44-55
 *		
 **********************************************************************************/
int npd_system_get_basemac
(
    unsigned char *macAddr,
    unsigned int  size
)
{
    unsigned char *sysMac = NULL;
	unsigned char sysmac[12]={0};
	unsigned char cur = 0,value = 0;
	unsigned int ret = 0, i = 0,j = 0;

	if(size < MAC_ALEN) {
		return -1;
	}
	
	sysMac = (unsigned char*)PRODUCT_MAC_ADDRESS;
    for( i = 0;i < 12;i++ ) {
        cur = sysMac[i];
		if((cur >= '0') &&(cur <='9')) {
			value = cur - '0';
		}
		else if((cur >= 'A') &&(cur <='F')) {
			value = cur - 'A';
			value += 0xa;
		}
		else if((cur >= 'a') &&(cur <='f')) {
			value = cur - 'a';
			value += 0xa;
		}
		else { /* illegal character found*/
			return -2;
		}

		if(0 == i%2) {
			macAddr[i/2] = value;
		}
		else {
			macAddr[i/2] <<= 4;
			macAddr[i/2] |= value;
		}
	}

	return 0;
}

/**********************************************************************************
 * npd_init_oam
 * 
 *  Initialize OAM settings such as TELNET/SSH/HTTP/HTTPS/SNMP traffic special treatment
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *
 *	NOTATION:
 *		
 **********************************************************************************/
void npd_init_oam
(
	void
) 
{
	/*if(PRODUCT_ID_AX5K_E == PRODUCT_ID || PRODUCT_ID_AX5608== PRODUCT_ID){/*5612e not support*/
	/*	syslog_ax_product_dbg("this produc do not support oam.\n");
		return;
	}*/

	if(!((productinfo.capbmp) & FUNCTION_OAM_VLAUE)){
		npd_syslog_dbg("do not support oam initial!\n");
		return;
	}
	else if(((productinfo.capbmp) & FUNCTION_OAM_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_OAM_VLAUE)){
		npd_syslog_dbg("do not support oam initial!\n");
		return;
	}
	else{
		enum OAM_TRAFFIC_TYPE_ENT i = 0;
		int rc = NPD_OK;

		for(; i < OAM_TRAFFIC_TYPE_MAX; i++) {
			rc = npd_oam_traffic_setup(i,&(g_oam_infos[i]));
			if(NPD_OK != rc) {
				syslog_ax_product_err("init oam traffic %s erro %d\n",oam_traff_desc[i], rc);
			}
		}

		return;
	}
}

/**********************************************************************************
 * npd_oam_traffic_setup
 *
 * 		Initialize system OAM traffic such as TELNET/SSH/HTTP/HTTPS/SNMP classfication to
 * 	provide high quality of service with high priority, low drop rate.
 *
 *	INPUT:
 *		type - OAM control traffic type
 *		oamInfo - detailed OAM control info
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 - if no error occurred.
 *		-1 - if error occurred.
 *		
 **********************************************************************************/
int npd_oam_traffic_setup
(
	enum OAM_TRAFFIC_TYPE_ENT type,
	struct oam_ctrl_s *oamInfo
	
)
{
	int rc = NPD_OK;
	
	if(!oamInfo) {
		return -1;
	}

	rc = nam_oam_traffic_setup(type,oamInfo);
	return rc;
}

/**********************************************************************************
 * npd_init_system_stateinfo
 * 
 * Initialize system accessories(such as fan,power,temperature etc.) state info
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 - if no error occurred.
 *		
 **********************************************************************************/
int npd_init_system_stateinfo
(
	void
)
{
	/*enum system_fan_status_e	fan_state = SYSTEM_FAN_STAT_MAX;*/
	
	memset((void*)(&systemStateInfo),0,sizeof(struct system_state_s));

	/*
	Read cpld and GPIO to get system state info
	*/	
	nbm_init_system_state_info(&systemStateInfo);
	syslog_ax_product_dbg("system fan %s power %d temperature %d(surface) %d(core)\n",	\
					(SYSTEM_FAN_STAT_NORMAL == systemStateInfo.fan_state) ? "running":"alarming",	\
					systemStateInfo.power_state,	\
					systemStateInfo.surface.current,systemStateInfo.core.current);
	return 0;
}

/*

cmd used to test this method
dbus-send --system --dest=aw.npd --type=method_call --print-reply=literal /aw/npd aw.npd.show_ver

 arg lists for method NPD_DBUS_INTERFACE_METHOD_VER
  in arg list:
	NONE
  out arg list:  // in the order as they are appended in the dbus message.
	uint32 product_id	// unsigned int of product type. uniquely identify a product, defined in npd_sysdef.h ,
	uint32 sw_version      // bitmap definition in npd_sysdef.h
  	string product_name  // backplane info for chassis product, board info for box product
	string product_base_mac_addr  // 12 char of mac address  with no : or - spliter.
  	string product_serial_number  // 32 bytes string
	string sw_name		// software description string
 

 */

DBusMessage * npd_dbus_interface_show_ver(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	unsigned int product_id = npd_query_product_id();
	unsigned int sw_version = npd_query_sw_version();
	struct product_sysinfo_s *p_sysinfo;

	p_sysinfo = npd_query_sysinfo();

	syslog_ax_product_dbg("sw version %#0x\n",sw_version);
	syslog_ax_product_dbg("Query sysinfo product name %s sn %s swname %s basemac %s\n",
			   p_sysinfo->name,
			   p_sysinfo->sn,
			   p_sysinfo->sw_name,
			   p_sysinfo->basemac);
	
	
	
	reply = dbus_message_new_method_return(msg);
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&product_id,
							 DBUS_TYPE_UINT32,&sw_version,
							 DBUS_TYPE_STRING,&(p_sysinfo->name),
							 DBUS_TYPE_STRING,&(p_sysinfo->basemac),
							 DBUS_TYPE_STRING,&(p_sysinfo->sn),
							 DBUS_TYPE_STRING,&(p_sysinfo->sw_name),
							 DBUS_TYPE_INVALID);
	
	return reply;
}

DBusMessage * npd_dbus_interface_show_hwconf(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusMessageIter	 iter_array;
	int i;
	
	unsigned char slot_count = CHASSIS_SLOT_COUNT;
	
	/* 4626, 5612, 5612i have 2 slots, just show the slot 1 */
	if(PRODUCT_ID_AU4K == PRODUCT_ID || PRODUCT_ID_AX5K == PRODUCT_ID \
		|| PRODUCT_ID_AX5K_I == PRODUCT_ID) {
		slot_count = 1;
	}		
	
	syslog_ax_product_dbg("Entering show hwconfig!\n");
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	/* Total slot count*/
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_BYTE,
									&slot_count);

	syslog_ax_product_dbg("Total slot count %d\n",slot_count);


	dbus_message_iter_open_container (&iter,
										DBUS_TYPE_ARRAY,
											DBUS_STRUCT_BEGIN_CHAR_AS_STRING
												DBUS_TYPE_BYTE_AS_STRING
												DBUS_TYPE_UINT32_AS_STRING
												DBUS_TYPE_BYTE_AS_STRING
												DBUS_TYPE_BYTE_AS_STRING
												DBUS_TYPE_BYTE_AS_STRING
												DBUS_TYPE_STRING_AS_STRING
												DBUS_TYPE_STRING_AS_STRING
											DBUS_STRUCT_END_CHAR_AS_STRING,
										&iter_array);
	
	
	for (i = 0; i < slot_count; i++ ) {
	/* Array of slot information
		slot no
		module id
		module status
		module hw_version
		ext slot count
		module serial no
		string module name
	*/

		/* 4626, 5612, 5612i have 2 slots, just show the slot 1 */
		if((PRODUCT_ID_AU4K == PRODUCT_ID || PRODUCT_ID_AX5K == PRODUCT_ID \
			|| PRODUCT_ID_AX5K_I == PRODUCT_ID) && 0 == i) {
			i = 1;
		}	
		DBusMessageIter iter_struct;
		unsigned char slotno =  CHASSIS_SLOT_INDEX2NO(i);
		unsigned char module_status = MODULE_STATUS_ON_SLOT_INDEX(i);
		unsigned char hw_ver = MODULE_HW_VER_ON_SLOT_INDEX(i);
		unsigned char ext_slot_num = EXT_SLOT_COUNT(i);
		unsigned int module_id = MODULE_TYPE_ON_SLOT_INDEX(i);
		char *sn = MODULE_SN_ON_SLOT_INDEX(i);
		char *modname = MODULE_NAME_ON_SLOT_INDEX(i);
		
		syslog_ax_product_dbg("Slot %d module id %#x status %d hwver %d extslotnum %d sn %s modname %s\n",
				   slotno,
				  module_id,
				  module_status,
				  hw_ver,
				  ext_slot_num,
				  sn,
				  modname);

		dbus_message_iter_open_container (&iter_array,
										   DBUS_TYPE_STRUCT,
										   NULL,
										   &iter_struct);
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(slotno));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(module_id));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(module_status));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(hw_ver));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(ext_slot_num));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_STRING,
				  &(sn));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_STRING,
				  &(modname));
		dbus_message_iter_close_container (&iter_array, &iter_struct);
		
	}

	dbus_message_iter_close_container (&iter, &iter_array);
	
	return reply;
}

DBusMessage * show_system_environment_state(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	/*DBusMessageIter	 iter;*/
	
	unsigned int	ret = NPD_DBUS_SUCCESS;
	NPD_ENVI    envi_state ;
	DBusError err;
	dbus_error_init(&err);
	memset(&envi_state,0,sizeof(NPD_ENVI));
	ret=npd_get_system_environment_state(&envi_state);
	if(ret != 0){
		ret = NPD_DBUS_ERROR;
		syslog_ax_product_err("set system alarm state err! ");
	}

	/* Box product has invalid CPLD state for FAN & POWER running status*/
	if(PRODUCT_IS_BOX) {
		envi_state.fan_porwer = 1; /* fan state normal*/
		envi_state.fan_porwer |= (1<<1); /* power state normal*/
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply,
							 DBUS_TYPE_BYTE,&(envi_state.fan_porwer),
							 DBUS_TYPE_UINT16,&(envi_state.inter),
							 DBUS_TYPE_UINT16,&(envi_state.surface),
							 DBUS_TYPE_INVALID);
	return reply;
	
}


DBusMessage * npd_system_shut_down_enable(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	
	unsigned short	isenable  = 0;
	unsigned int	ret = NPD_DBUS_SUCCESS;
	
	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT16,&isenable,
									DBUS_TYPE_INVALID))) {
		syslog_ax_product_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				syslog_ax_product_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
		ret = NPD_DBUS_ERROR;
	}
	ret=npd_system_shutdown_enable(isenable);
	if(ret != 0){
		ret = NPD_DBUS_ERROR;
		syslog_ax_product_err("set system alarm state err! ");
		}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	return reply;
	
}

/*******************************************************************************
* npd_system_arp_smac_check_enable
*
* DESCRIPTION:
*      Set whether the CHIP do the smac-check or not.
*
* INPUTS:
*	  isenable :
*                     1--set enable
*                     0--set disalbe
*
* OUTPUTS:
*       ret - ARP_RETURN_CODE_SUCCESS  -- enable success
*		   ARP_RETURN_CODE_NAM_ERROR  -- enable failed
* RETURNS:
*       reply.
*
* COMMENTS:
*       
*
*******************************************************************************/
DBusMessage * npd_system_arp_smac_check_enable(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	
	unsigned int	isenable  = 0;
	unsigned int	ret = NPD_DBUS_SUCCESS;
	
	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT32,&isenable,
									DBUS_TYPE_INVALID))) {
		syslog_ax_product_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				syslog_ax_product_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
		ret = NPD_DBUS_ERROR;
	}
	ret = nam_arp_smac_check_enable(isenable);
	if(ARP_RETURN_CODE_SUCCESS != ret){
		ret = ARP_RETURN_CODE_NAM_ERROR;
		syslog_ax_product_err("set system arp SMAC-check err! ");
	}
	else {
		arp_smac_check= isenable;
	}
	
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	return reply;
	
}


/*******************************************************************************
* npd_system_signal_hndlr_init
*
* DESCRIPTION:
*      Initialize board signal process to catch up external signals
*
* INPUTS:
*	  None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0   - on success,
*       other - initialize signal handler failed.
*
* COMMENTS:
*       
*
*******************************************************************************/
int npd_system_signal_hndlr_init
(
	void
)
{
	  int ret;
	  struct sigaction sig;
	  struct sigaction osig;
	  pid_t	self = 0;
	  unsigned char pidBuf[10] = {0};
	  int fd = 0;

	  self = getpid();
	  sprintf(pidBuf,"%d",self);
	  sig.sa_handler = &appDemoBoardSigHandler;
	  sigfillset (&sig.sa_mask);
	  sig.sa_flags = 0;

	  ret = sigaction (APP_DEMO_TRAFFIC_ENABLE_SIGNUM, &sig, &osig);
	  if (ret < 0) 
		return ret;
	  else {
		fd = open(NPD_SIGHANDLER_PID_PATH,O_CREAT|O_RDWR|O_TRUNC);
		if(fd < 0) {
			syslog_ax_product_err("open pid file %s error\n",NPD_SIGHANDLER_PID_PATH);
		}
		else {
			write(fd,pidBuf,strlen(pidBuf));
			syslog_ax_product_dbg("write signal handler pid %d to %s\n",self,NPD_SIGHANDLER_PID_PATH);
			close(fd);
		}
		return 0;
	  }
}

/*******************************************************************************
* npd_check_system_startup_state
*
* DESCRIPTION:
*      Check if board has started up correctly, then enable interrupt and led blinking.
*
* INPUTS:
*	  None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*	
*
* COMMENTS:
*       
*
*******************************************************************************/
void npd_check_system_startup_state
(
	void
)
{
	unsigned char buf[4] = {0};
	int ret = 0;

	/* tell my thread id */
	npd_init_tell_whoami("SysWaitStatePoll",0);

	while(1)
	{
        board_state_fd = open(NPD_BOARD_STARTUP_STATE_PATH,O_RDONLY);		
        if(board_state_fd < 0)
        {
        	syslog_ax_product_err("open product startup state file %s error\n",NPD_BOARD_STARTUP_STATE_PATH);
        }
	    else
		{
		    read(board_state_fd,buf,1);
		    if('1' == buf[0]) 
		  	{
    		  	/* enable asic interrupts, appDemoBoardAfterSysUpIntEnable();*/
    			ret = nam_board_after_enable();
            	if(0 != ret)
            	{
                	syslog_ax_product_err("nam_board_after_enable error ret %d.\n",ret);		
            	}
            	else
            	{
                	syslog_ax_product_dbg("Enable asic interrupts OK.\n");		
            	}
				
    			/* set system init done:LED RUN blinking */
    			nbm_set_system_init_stage(1);
               	syslog_ax_product_dbg("Set system init done:LED RUN blinking.\n");		
				
    			close(board_state_fd);
    			
    			/* set startup flag, make log not to bootlog.npd */
    			npd_startup_end = 1;
				
    			/* sleep for a while */
    			sleep(1);		
    			break;  /* the thread is return. */
		    }
            else
            {
            	/* if baord.state != '1', sleep(1), we must reopen it! */
                close(board_state_fd);
            }
	    }  
        sleep(1);
	}
}

#if 0
/*******************************************************************************
* npd_check_system_startup_state
*
* DESCRIPTION:
*      Check if system has started up correctly.
*
* INPUTS:
*	  None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*	
*
* COMMENTS:
*       
*
*******************************************************************************/
void npd_check_system_startup_state
(
	void
)
{
	unsigned char buf[4] = {0};
	int i = 0, ret = 0;
	unsigned int eth_g_index = 0;
	static unsigned int counter = 0;

	/*s tell my thread id*/
	npd_init_tell_whoami("SysWaitSfpPoll",0);

	while(1)
	{
        aw_state_fd = open(NPD_SYSTEM_STARTUP_STATE_PATH,O_RDONLY);		
        if(aw_state_fd < 0)
        {
        	syslog_ax_product_err("open product startup state file %s error\n",NPD_SYSTEM_STARTUP_STATE_PATH);
        }
	    else
		{
		    read(aw_state_fd,buf,1);
		    if('1' == buf[0]) 
		  	{
    			/*enable POW interrupts, so CPU can recieve packets from RGMII/SPI port*/
    			#if 0
    		  	nbm_set_ethport_POW_Int_enable(); /* Zhang Di delete for AXSSZFI-1071 */
    			#endif
    		  	/* enable asic interrupts */
    			/*appDemoBoardAfterSysUpIntEnable();*/
    			nam_board_after_enable();

    			#ifdef DRV_LIB_BCM
    			/* 
    			 * reset CPLD register 0x06 bit[4], enable Fiber-optic ports and electric ports mutex
    			 */
    			 
    			syslog_ax_product_dbg("reset led mutex cpld reg 0x06 bit[4] for BCM.\n");
    			nbm_reset_led_mutex_cpld();
    			#endif
    			
    			/* set system init done:LED RUN blinking */
    			nbm_set_system_init_stage(1);
    			close(aw_state_fd);
    			
    			/* set startup flag */
    			npd_startup_end = 1;

    			#if 0   /* remove enable for watch-dog, zhangdi 2012-10-19 */
    			/* enable hardware watchdog */
    			ret = nbm_hardware_watchdog_control_set(SYSTEM_HARDWARE_WATCHDOG_ENABLE);
    			if(ret) {
    				syslog_ax_product_warning("enable hardware watchdog error %d!", ret);
    			}
    			#endif
    			/* sleep for a while */
    			sleep(1);		
    			/*
    			 	Start probe thread to minitor fiber module link status 
    			*/
    			/*nam_thread_create("SfpPoll",(void *)npd_eth_port_status_polling_thread,NULL,NPD_TRUE,NPD_FALSE);*/
    			/*return;*/
    			break;
		    }
            else
            {
            	/* if aw.state != '1', sleep(1), we must reopen it! */
                close(aw_state_fd);
            }
	    }  
        /* fillup hardware watchdog */
        #if 0     /* remove fillup watch-dog, zhangdi 2012-10-19 */
        ret = nbm_hardware_watchdog_fillup();
        if(ret && !counter) {
          syslog_ax_product_warning("hardware watchdog fillup failed!\n");
        }
        if(AX7_FATAL_ERR_MSG_PRINT_INTERVAL == counter++) {
         counter = 0; /* reset counter value */
        }

        /*
        *  scan ethernet port link status
        */
        #if 0	 /* this is no use on SDK2.2, zhangdi 2012-02-08 */
        npd_scan_eth_ports_link_status();
        #endif
        #endif		
        sleep(1);
	}

	#if 0   /* remove fillup watch-dog, zhangdi 2012-10-19 */
	/* do SFP port status polling*/
	while(1)
	{
		for(i = 0; i < MAX_ETH_GLOBAL_INDEX; i++)
		{
			eth_g_index = g_fiber_port_poll[i];
			if(0 == eth_g_index) {
				continue;
			}
			portNotifier(eth_g_index,ETH_PORT_NOTIFIER_LINKPOLL_E);
		}

		/* fillup hardware watchdog */
		ret = nbm_hardware_watchdog_fillup();
		if(ret && !counter) {
		   syslog_ax_product_warning("hardware watchdog fillup failed!\n");
		}
		if(AX7_FATAL_ERR_MSG_PRINT_INTERVAL == counter++) {
		   counter = 0; /* reset counter value */
		}

		/* wait for a while	*/	
		sleep(1);
	}
	#endif
}
#endif
/*******************************************************************************
* npd_init_packet_socket
*
* DESCRIPTION:
*      This function initialize all packet sockets used for sending packets from/to other module.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*	  None.
*
* COMMENTS:
*       
*
*******************************************************************************/
void npd_init_packet_socket
( 
	void
)
{
	unsigned long status = 0;
	/*if(PRODUCT_ID_AX5K_E == PRODUCT_ID || PRODUCT_ID_AX5608== PRODUCT_ID){/*5612e not support*/
	/*	syslog_ax_product_dbg("PRODUCT_ID_AX5K_E and PRODUCT_ID_AX5608 not support packet init.\n");
		return;
	}	*/
	
	if(!((productinfo.capbmp) & FUNCTION_ASIC_VLAUE)){
		npd_syslog_dbg("do not support ASIC packet socket!\n");
		return;
	}
	else if(((productinfo.capbmp) & FUNCTION_ASIC_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ASIC_VLAUE)){
		npd_syslog_dbg("do not support ASIC packet socket!\n");
		return;
	}
	else{
		status = nam_packet_rx_adapter_init();
		if(status) {
			syslog_ax_product_err("init sockets for packet rx error %d\r\n", status);
		}

		status = nam_packet_tx_adapter_init();
		if(status) {
			syslog_ax_product_err("init sockets for packet tx error %d\r\n", status);
		}

		return;
	}
}

/*******************************************************************************
* npd_init_tell_whoami
*
* DESCRIPTION:
*      This function is used by each thread to tell its name and pid to NPD_ALL_THREAD_PID_PATH
*
* INPUTS:
*	  tName - thread name.
*	  lastTeller - is this the last teller or not, pid file should be closed if true.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*	  None.
*
* COMMENTS:
*       
*
*******************************************************************************/
void npd_init_tell_whoami
(
	unsigned char *tName,
	unsigned char lastTeller
)
{
	static unsigned char opened = 0;
	static int fd = 0;
	pid_t self = 0;
	unsigned char pidBuf[64] = {0};

	self = getpid();
	
	#ifdef CPU_ARM_XCAT
	g_npd_thread_number++;
	#endif
	
	if(!opened) {
		fd = open(NPD_ALL_THREAD_PID_PATH,O_CREAT|O_RDWR|O_TRUNC);
		if(fd < 0) {
			syslog_ax_product_err("open file error when %s tell its pid %d\n",	\
					tName ? tName : "null",self);
			return;
		}
		opened = 1;
	}
	syslog_ax_product_dbg("thread %s tell its pid %d to fd %d %s\n",	\
			tName ? tName : "null", self, fd, NPD_ALL_THREAD_PID_PATH);

	sprintf(pidBuf,"%d - %s\n",self, tName ? tName:"null");
	
	write(fd,pidBuf,strlen(pidBuf));

	if(lastTeller) {
		close(fd);
		syslog_ax_product_dbg("last teller %s %d closed pid file fd %d %s\n",  \
				tName ? tName:"null",self, fd, NPD_ALL_THREAD_PID_PATH);
	}
	
	return;
}

/*******************************************************************************
* npd_init_tell_stage_end
*
* DESCRIPTION:
*      This function is used by NPD to tell rc script loader about initialization process done.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*	  None.
*
* COMMENTS:
*       
*
*******************************************************************************/
void npd_init_tell_stage_end
(
	void
)
{
	/* NPD initialization stage file descriptor */
	int npd_stage_fd = -1;
	unsigned char buf[16] = {0};

	/* Set asic.state to 1 */
	npd_stage_fd = open(NPD_INIT_STAGE_FILE_PATH, O_CREAT|O_RDWR|O_TRUNC);
	if(npd_stage_fd < 0) {
		syslog_ax_product_err("open file %s error when tell init stage end\n",	\
				NPD_INIT_STAGE_FILE_PATH);
		return;
	}

	sprintf(buf,"%d",1);	
	write(npd_stage_fd,buf,strlen(buf));

	close(npd_stage_fd);	
	syslog_ax_product_dbg("npd tell init stage to %s ok\n", NPD_INIT_STAGE_FILE_PATH);
	return;
}

/*******************************************************************************
* npd_init_tell_system_state_end
*
* DESCRIPTION:
*      This function is used by NPD to tell system is running OK.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*	    0 ---OK.  
*      -1 ---fialed.
*
* COMMENTS:
*       
*
*******************************************************************************/
int npd_init_tell_system_state_end
(
	void
)
{
	int ret = 0;
	unsigned char buf[16] = {0};
	/* Set aw.state to 1 */
	aw_state_fd = open(NPD_SYSTEM_STARTUP_STATE_PATH, O_CREAT|O_RDWR|O_TRUNC);
	if(aw_state_fd < 0) {
		syslog_ax_product_err("open file %s error when tell init stage end\n",	\
				NPD_SYSTEM_STARTUP_STATE_PATH);
		ret = -1;
	}
	else
	{
    	sprintf(buf,"%d\n",1);	
    	write(aw_state_fd,buf,strlen(buf));

    	close(aw_state_fd);	
    	syslog_ax_product_dbg("NPD tell system running stage to %s OK.\n", NPD_SYSTEM_STARTUP_STATE_PATH);
		ret = 0;
	}
	return ret;
}

/****************************************************
 *	Description:
 *		check port's target is SPI or not
 *		SPI path with uplink   (1,26) on AX7 and
 *							(0,24) on AX5612i and
 *							(0,0) on AX5612
 *
 *	Input:
 *		unsigned char -device num of eth-port
 *		unsigned char -port num of eth-port
 *
 *	Output:
 *		NULL
 *
 *	ReturnCode:
 *		NPD_TRUE		- success
 *		NPD_FALSE		- fail
 *		
 ****************************************************/
unsigned int npd_product_check_spi_port
(
	unsigned char devNum,
	unsigned char portNum
)
{
	unsigned int result = NPD_FALSE;

	syslog_ax_product_dbg("asic port(%d, %d)\n", 
							devNum, portNum);
	
	/* target is SPI */
	switch(PRODUCT_ID) {
		case PRODUCT_ID_AX7K:
		case PRODUCT_ID_AX7K_I:
			if((HSC_PORT_UPLINK_DEVNO_AX7 == devNum) && (HSC_PORT_UPLINK_PORTNO_AX7 == portNum)) {
				result = NPD_TRUE;
			}
			break;
		case PRODUCT_ID_AX5K:
			if((HSC_PORT_UPLINK_DEVNO_AX5 == devNum) && (HSC_PORT_UPLINK_PORTNO_AX5 == portNum)) {
				result = NPD_TRUE;
			}
			break;
		case PRODUCT_ID_AX5K_I:
			if((HSC_PORT_UPLINK_DEVNO_AX5I == devNum) && (HSC_PORT_UPLINK_PORTNO_AX5I == portNum)) {
				result = NPD_TRUE;
			}
			break;
		default:
			result =  NPD_FALSE;
			break;
	}

	syslog_ax_product_dbg("asic port(%d,%d) is %son HSC path\n", 
							devNum, portNum, result ? "":"not ");
	return result;
}

/**********************************************************************************
 * npd_product_hardware_watchdog_function_check
 *
 * 	Check whether the product support hardware watch dog or not.
 *	Currently, all chassis and box product with CPLD supported has hardware watchdog funcationality.
 *  That means asic chip with XCAT series has no hardware watchdog functionality.
 *
 *	INPUT:
 *		productId - product id
 *	
 *	OUTPUT:
 *		NULL
 *
 *	RETURN:
 *		0 - no hardware watchdog on this product
 *		1 - hardware watchdog is deployed on this product
 *
 **********************************************************************************/
int npd_product_hardware_watchdog_function_check
(
	void
)
{
	int supported = NPD_TRUE;

	switch(PRODUCT_ID){
		default:
			supported = NPD_TRUE;
			break;
		case PRODUCT_ID_AU2K_TCAT:
		case PRODUCT_ID_AU3K_BCAT:
			supported = NPD_FALSE;
			break;
	}
	
	return supported;
}

/**********************************************************************************
 * npd_product_support_function_init
 * 
 * whether the product support the function
 *
 *	INPUT:
 *		product_info-product globle variable
 *	
 *	OUTPUT:
 *		product_info-product globle variable
 *
 * 	RETURN:
 *		
 *	NOTATION:
 *		In product globle variable, runbmp and capbmp is used for input and output
 *		
 **********************************************************************************/

int npd_product_support_function_init
(
	struct product_s *product_info
){
	unsigned int ret = 0;
	unsigned int fun_bmp = 0;
	unsigned int run_bmp = 0;
	NPD_PRODUCT_FUNCTION *funmap = NULL;
	NPD_PRODUCT_FUNCTION *runmap = NULL;

	if(!product_info){
		syslog_ax_product_err("get product info error!\n");
		return NPD_FAIL;
	}
	funmap = npd_get_product_function_funbmp(PRODUCT_ID);
	/*product's function that hardware all supportted */
	/*acl*/
	fun_bmp |= (funmap->function_acl << FUNCTION_ACL);
	/*qos*/
	fun_bmp |= (funmap->function_qos << FUNCTION_QOS);
	/*tunnel*/
	fun_bmp |= (funmap->function_tunnel << FUNCTION_TUNNEL);
	/*pvlan*/
	fun_bmp |= (funmap->function_pvlan << FUNCTION_PVLAN);
	/*igmp*/
	fun_bmp |= (funmap->function_igmp << FUNCTION_IGMP);
	/*mld*/
	fun_bmp |= (funmap->function_mld << FUNCTION_MLD);
	/*dldp*/
	fun_bmp |= (funmap->function_dldp << FUNCTION_DLDP);
	/*stp*/
	fun_bmp |= (funmap->function_stp << FUNCTION_STP);
	/*vlan*/
	fun_bmp |= (funmap->function_vlan << FUNCTION_VLAN);
	/*interface*/
	fun_bmp |= (funmap->function_interface << FUNCTION_INTERFACE);
	/*mirror*/
	fun_bmp |= (funmap->function_mirror << FUNCTION_MIRROR);
	/*fdb*/
	fun_bmp |= (funmap->function_fdb << FUNCTION_FDB);
	/*trunk*/
	fun_bmp |= (funmap->function_trunk << FUNCTION_TRUNK);
	/*arp*/
	fun_bmp |= (funmap->function_arp<< FUNCTION_ARP);
	/*eslot*/
	fun_bmp |= (funmap->function_eslot<< FUNCTION_ESLOT);
	/*asic*/
	fun_bmp |= (funmap->function_asic<< FUNCTION_ASIC);
	/*oam*/
	fun_bmp |= (funmap->function_oam<< FUNCTION_OAM);
	/*route*/
	fun_bmp |= (funmap->function_route<< FUNCTION_ROUTE);
	
	syslog_ax_product_dbg("fun_bmp : 0x%x",fun_bmp);

	runmap = npd_get_product_function_runbmp(PRODUCT_ID);
	/*function what we need the product support*/
	/*acl*/
	run_bmp |= (runmap->function_acl << FUNCTION_ACL);
	/*qos*/
	run_bmp |= (runmap->function_qos << FUNCTION_QOS);
	/*tunnel*/
	run_bmp |= (runmap->function_tunnel << FUNCTION_TUNNEL);
	/*pvlan*/
	run_bmp |= (runmap->function_pvlan << FUNCTION_PVLAN);
	/*igmp*/
	run_bmp |= (runmap->function_igmp << FUNCTION_IGMP);
	/*mld*/
	run_bmp |= (runmap->function_mld << FUNCTION_MLD);
	/*dldp*/
	run_bmp |= (runmap->function_dldp << FUNCTION_DLDP);
	/*stp*/
	run_bmp |= (runmap->function_stp << FUNCTION_STP);
	/*vlan*/
	run_bmp |= (runmap->function_vlan << FUNCTION_VLAN);
	/*interface*/
	run_bmp |= (runmap->function_interface << FUNCTION_INTERFACE);
	/*mirror*/
	run_bmp |= (runmap->function_mirror << FUNCTION_MIRROR);
	/*fdb*/
	run_bmp |= (runmap->function_fdb << FUNCTION_FDB);
	/*trunk*/
	run_bmp |= (runmap->function_trunk << FUNCTION_TRUNK);
	/*arp*/
	run_bmp |= (runmap->function_arp<< FUNCTION_ARP);
	/*eslot*/
	run_bmp |= (runmap->function_eslot<< FUNCTION_ESLOT);
	/*asic*/
	run_bmp |= (runmap->function_asic<< FUNCTION_ASIC);
	/*oam*/
	run_bmp |= (runmap->function_oam<< FUNCTION_OAM);
	/*route*/
	run_bmp |= (runmap->function_route<< FUNCTION_ROUTE);

	syslog_ax_product_dbg("fun_bmp : 0x%x,run_bmp : 0x%x\n",fun_bmp,run_bmp);

	product_info->runbmp = run_bmp;
	product_info->capbmp = fun_bmp;

	return NPD_SUCCESS;
}

/**********************************************************************************
 * npd_get_product_function_runbmp
 * 
 * get product's running bit map
 *
 *	INPUT:
 *		product_id-product ID NO.
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		product running bitmap's pointer that software support
 *		
 *	NOTATION:
 *		
 *		
 **********************************************************************************/

NPD_PRODUCT_FUNCTION *npd_get_product_function_runbmp
(
	unsigned int product_id	
)
{
	switch(PRODUCT_ID) {
		case PRODUCT_ID_NONE:
		default:
			return &function_run_map[0];
		case PRODUCT_ID_AX7K:
			return &function_run_map[1];
		case PRODUCT_ID_AX7K_I:
			return &function_run_map[2];
		case PRODUCT_ID_AX5K:
			return &function_run_map[3];
		case PRODUCT_ID_AX5K_I:
			return &function_run_map[4];
		case PRODUCT_ID_AX5K_E:
			return &function_run_map[5];
		case PRODUCT_ID_AU4K:
			return &function_run_map[6];
		case PRODUCT_ID_AU3K:
			return &function_run_map[7];
		case PRODUCT_ID_AU3K_BCM:
			return &function_run_map[8];
		case PRODUCT_ID_AU3K_BCAT:
			return &function_run_map[9];
		case PRODUCT_ID_AU2K_TCAT:
			return &function_run_map[10];
		case PRODUCT_ID_AX5608:
			return &function_run_map[11];
        case PRODUCT_ID_AX8600:
			return &function_run_map[12];
	}
}

/**********************************************************************************
 * npd_get_product_function_funbmp
 * 
 * get product's running bit map
 *
 *	INPUT:
 *		product_id-product ID NO.
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		product running bitmap's pointer that software support
 *		
 *	NOTATION:
 *		
 *		
 **********************************************************************************/


NPD_PRODUCT_FUNCTION *npd_get_product_function_funbmp
(
	unsigned int product_id	
)
{
	switch(PRODUCT_ID) {
		case PRODUCT_ID_NONE:
		default:
			return &product_cap_mib[0];
		case PRODUCT_ID_AX7K:
			return &product_cap_mib[1];
		case PRODUCT_ID_AX7K_I:
			return &product_cap_mib[2];
		case PRODUCT_ID_AX5K:
			return &product_cap_mib[3];
		case PRODUCT_ID_AX5K_I:
			return &product_cap_mib[4];
		case PRODUCT_ID_AX5K_E:
			return &product_cap_mib[5];
		case PRODUCT_ID_AU4K:
			return &product_cap_mib[6];
		case PRODUCT_ID_AU3K:
			return &product_cap_mib[7];
		case PRODUCT_ID_AU3K_BCM:
			return &product_cap_mib[8];
		case PRODUCT_ID_AU3K_BCAT:
			return &product_cap_mib[9];
		case PRODUCT_ID_AU2K_TCAT:
			return &product_cap_mib[10];
		case PRODUCT_ID_AX5608:
			return &product_cap_mib[11];
        case PRODUCT_ID_AX8600:
			return &product_cap_mib[12];
	}
}

DBusMessage * npd_dbus_cpufc_config(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusError err;
	DBusMessageIter	 iter;
	int ret = 0;
	unsigned int time = 0,rate = 0;
	CPU_FC_PACKET_TYPE cpuFc = CPU_FC_TYPE_MAX_E;

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT32,&cpuFc,
		DBUS_TYPE_UINT32,&time,
		DBUS_TYPE_UINT32,&rate,
		DBUS_TYPE_INVALID))) {
		syslog_ax_eth_port_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_eth_port_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	syslog_ax_eth_port_dbg("cpuFc %d, time %d, rate %d\r\n",cpuFc,time,rate);
	ret = nam_asic_set_cfc(cpuFc,time,rate);
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);	

	return reply;
}

DBusMessage * npd_dbus_sys_global_cfg_save(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	char *showStr = NULL;
	char* pos = NULL;
	int avalidLen = 0;
	int mallocSize = 0;
#if 0
	int promSubIfCount = 0;
	int promCount = 0;
	unsigned int promisPortBmp[2] = {0};
    promSubIfCount = npd_get_promis_subif_count();
	npd_vlan_get_promis_port_bmp(promisPortBmp);
	promCount = npd_eth_port_count_of_bmp(promisPortBmp);
#endif

	/* get the SIZE we should malloc */
	/* 80(show buffer mode,arp cfg),
	  *[50(eth port interface advanced-routing)* promis intf count],
	  *[40(config prot-vlan udf-ethtype 0x8888\n)],
	  *[subifcount*25(sub if config command)]
	  */
    mallocSize = 80+45+40; /*+((50)*promCount)+(promSubIfCount*25)*/ 
	showStr = (char*)malloc(mallocSize);
	if(NULL == showStr){
		syslog_ax_vlan_err("memory malloc error function %s line %d\n",__func__,__LINE__);
		return NULL;
	}
	memset(showStr,0,mallocSize);
	pos = showStr;
	avalidLen = mallocSize;

	/* global eth-port buffer mode */
	npd_dbus_save_buffer_mode_cfg(&pos,&avalidLen);	

	/* save route rpf config */
	npd_dbus_route_show_running_config(&pos, &avalidLen);
	
	/*save arp cfg*/
	npd_dbus_save_arp_cfg(&pos,&avalidLen);
	/* move to npd_save_vlan_cfg*/
	/*npd_dbus_save_advanced_routing_default_vid_cfg(&pos,&avalidLen);*/

    npd_prot_vlan_config_udf_ethtype_save_cfg(&pos,&avalidLen);
#if 0
	npd_eth_port_save_promi_cfg(&pos,&avalidLen);
#endif

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_STRING,
									 &showStr);	
	free(showStr);
	showStr = NULL;
	return reply;
}

DBusMessage * npd_dbus_sys_global_config_counter_drop_statistic
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
		DBusMessage*	reply = NULL;
		DBusMessageIter iter;
		DBusError err;
		unsigned int 	ret = 0;
        BRIDGE_DROP_CNTR_MODE_ENT     reason;
		
		dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT32,&reason,
		DBUS_TYPE_INVALID))) {
		syslog_ax_eth_port_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_eth_port_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
		
		
		ret = nam_asic_config_counter_drop_statistic(reason);

		reply = dbus_message_new_method_return(msg);
	
		dbus_message_iter_init_append (reply, &iter);	
		dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);

		return reply;	
}

DBusMessage * npd_dbus_sys_global_counter_drop_statistic
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	DBusMessage*	reply = NULL;
	DBusMessageIter iter;
	DBusError err;
	unsigned int 	ret = 0;
	unsigned int	dropModeCnt = 0;
	unsigned int    dropcnt=0;

	dbus_error_init(&err);	

	ret = nam_asic_counter_drop_statistic(&dropModeCnt,&dropcnt);
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_append_args (reply,
	                            DBUS_TYPE_UINT32,&dropModeCnt,
	                            DBUS_TYPE_UINT32,&dropcnt,
	                            DBUS_TYPE_INVALID);

	syslog_ax_dbus_dbg("dbus send back data %d\n",dropcnt);
	return reply;	
}

DBusMessage * npd_dbus_sys_egress_drop_statistic
(
 
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data

)

{

	DBusMessage*	reply = NULL;
	DBusMessageIter iter;
	DBusError err;
    unsigned char slot_no =0,local_port_no =0;
	unsigned char portNum =0,devNum =0;
	unsigned int  ret = 0;
	unsigned int  eth_g_index =0;
	unsigned int slot_index =0;
	
	syslog_ax_eth_port_dbg("Entering show ethport stat!\n");

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
	 	DBUS_TYPE_BYTE,&slot_no,
	 	DBUS_TYPE_BYTE,&local_port_no,
	 	DBUS_TYPE_INVALID))) {
	 	syslog_ax_eth_port_err("Unable to get input args ");
	 	if (dbus_error_is_set(&err)) {
	 		syslog_ax_eth_port_err("%s raised: %s",err.name,err.message);
	 		dbus_error_free(&err);
	 	}
	 	return NULL;
	}	
    
	ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
	if (CHASSIS_SLOTNO_ISLEGAL(slot_no)){ 
		if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,local_port_no)) {
			slot_index = CHASSIS_SLOT_NO2INDEX(slot_no);
			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,local_port_no);
			syslog_ax_eth_port_dbg("index for %d/%d is %#0x\n",slot_no,local_port_no,eth_g_index);
			ret = NPD_DBUS_SUCCESS;
			}
		}
	
	if(NPD_DBUS_SUCCESS == ret){
			ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
			if(0 == ret) {
                ret = nam_asic_config_port_egress_counter(devNum,portNum);
                syslog_ax_eth_port_dbg("the config port egress value is %d\n",ret);
			}
			else {
				ret = NPD_DBUS_ERROR;
			}
		
			if (ret != 0) {
				syslog_ax_eth_port_dbg("npd_dbus_ethport_attrs_get:: port %d/%d attr get FAIL. \n",slot_no,local_port_no);
			}
		}
    reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);

	return reply;	

}
DBusMessage *  npd_dbus_vlan_egress_drop_statistic
(
	  
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{  
	DBusMessage*	 reply = NULL;
	DBusMessageIter iter;
	DBusError err;
	unsigned char slot_no=0,local_port_no=0;
	unsigned char portNum=0,devNum=0,port=0;
	unsigned int  ret=0;
	unsigned int  eth_g_index=0;
	unsigned short vlanId=0;
	unsigned int   slot_index =0;
	unsigned char tagMode = 0;
	syslog_ax_eth_port_dbg("Entering show ethport stat!\n");

	dbus_error_init(&err);
	 
  	if (!(dbus_message_get_args ( msg, &err,
	  	 DBUS_TYPE_UINT16,&vlanId,
	  	 DBUS_TYPE_BYTE,&port,
		 DBUS_TYPE_BYTE,&slot_no,
		 DBUS_TYPE_BYTE,&local_port_no,
		 DBUS_TYPE_INVALID))) {
		syslog_ax_eth_port_err("Unable to get input args\n");
		if (dbus_error_is_set(&err)) {
			syslog_ax_eth_port_err("%s raised: %s\n",err.name,err.message);
			dbus_error_free(&err);
		}
	 	return NULL;
	}

	else {
		if(port == 1){
		if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
			if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,local_port_no)) {
				slot_index = CHASSIS_SLOT_NO2INDEX(slot_no);
				eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,local_port_no);
				syslog_ax_eth_port_dbg("index for %d/%d is %#0x\n",slot_no,local_port_no,eth_g_index);
				if(NULL == npd_find_vlan_by_vid(vlanId)){
					ret = NPD_VLAN_NOTEXISTS;
				}
				else if( 0 == npd_vlan_check_contain_port(vlanId,eth_g_index,&tagMode)) {
					ret = NPD_VLAN_NOTEXISTS;
				}
				else{
					ret = NPD_DBUS_SUCCESS;
				}
		  
		        if(NPD_DBUS_SUCCESS == ret){
					ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
					if(0 == ret) {
						ret = nam_asic_config_vlan_port_egress_counter(devNum,portNum,vlanId); 	
		                syslog_ax_eth_port_dbg("the vlan return value is %d\n",ret);
					}
					else {
						ret = NPD_DBUS_ERROR;
					}
				
					if (ret != 0) {
						syslog_ax_eth_port_dbg("npd_dbus_ethport_attrs_get:: port %d/%d attr get FAIL. \n",slot_no,local_port_no);
					}
			    } 
		   	  }
			}
		}
		else{
			if(NULL == npd_find_vlan_by_vid(vlanId)) {
				ret = NPD_VLAN_NOTEXISTS;
			}
			/*else if( 0 == npd_vlan_check_contain_port(vlanId,eth_g_index,&tagMode)) {
				ret = NPD_ERR_VLAN_NONEXIST;
			}*/
			else{
				ret = NPD_DBUS_SUCCESS;
			}

			if(NPD_DBUS_SUCCESS == ret){

				ret = nam_asic_config_vlan_egress_counter(vlanId);
				if (ret != 0) {
					return NPD_DBUS_ERROR;
				}
				else {
					ret = NPD_DBUS_SUCCESS;
				}
			}
		
		}
		
	}
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);	 
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);

	return reply;	 
}
DBusMessage *npd_dbus_sys_show_egress_counter


(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
)

{
	DBusMessage*   reply = NULL;
	DBusMessageIter iter;
	DBusError err;
	unsigned int    ret = 0;
	unsigned int op_ret=0;
	NPD_PORT_EGRESS_CNTR_STC	 egrCntrPtr;
	memset(&egrCntrPtr,0,sizeof(NPD_PORT_EGRESS_CNTR_STC));

	dbus_error_init(&err);  

	op_ret = nam_asic_show_egress_counter(&egrCntrPtr);

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter); 
	
	dbus_message_iter_append_basic (&iter,
		                              DBUS_TYPE_UINT32,&op_ret);
	dbus_message_iter_append_basic (&iter,		
	                                  DBUS_TYPE_UINT32,&egrCntrPtr.brgEgrFilterDisc);
	dbus_message_iter_append_basic (&iter,		
	                                  DBUS_TYPE_UINT32,&egrCntrPtr.outBcFrames);
	dbus_message_iter_append_basic (&iter,			
	                                  DBUS_TYPE_UINT32,&egrCntrPtr.outCtrlFrames);
	dbus_message_iter_append_basic (&iter,				
	                                  DBUS_TYPE_UINT32,&egrCntrPtr.outMcFrames);
	dbus_message_iter_append_basic (&iter,			
	                                  DBUS_TYPE_UINT32,&egrCntrPtr.outUcFrames);
	dbus_message_iter_append_basic (&iter,			
	                                  DBUS_TYPE_UINT32,&egrCntrPtr.txqFilterDisc);
	return reply;   
	 
 }
DBusMessage *npd_dbus_sys_show_ingress_counter


(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
)

{
	DBusMessage*   reply = NULL;
	DBusMessageIter iter;
	DBusError err;
	unsigned int    ret = 0;
	unsigned int op_ret=0;
	BRIDGE_HOST_CNTR_STC hostGroupCntPtr ;
	unsigned int matrixCntSaDaPkts = 0;
	NETHERADDR		dmacAddr;
	NETHERADDR		smacAddr;
	
	memset(&hostGroupCntPtr,0,sizeof(BRIDGE_HOST_CNTR_STC));
	memset(&dmacAddr,0,sizeof(NETHERADDR));
	memset(&smacAddr,0,sizeof(NETHERADDR));

	dbus_error_init(&err);  

	op_ret = nam_asic_show_ingress_counter(&hostGroupCntPtr,&dmacAddr,&smacAddr,&matrixCntSaDaPkts);

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter); 
	
	dbus_message_iter_append_basic (&iter,
		                              DBUS_TYPE_UINT32,&op_ret);
	dbus_message_iter_append_basic (&iter,				
	                                  DBUS_TYPE_BYTE,&dmacAddr.arEther[0]);
	dbus_message_iter_append_basic (&iter,				
	                                  DBUS_TYPE_BYTE,&dmacAddr.arEther[1]);
	dbus_message_iter_append_basic (&iter,				
	                                  DBUS_TYPE_BYTE,&dmacAddr.arEther[2]);
	dbus_message_iter_append_basic (&iter,				
	                                  DBUS_TYPE_BYTE,&dmacAddr.arEther[3]);
	dbus_message_iter_append_basic (&iter,				
	                                  DBUS_TYPE_BYTE,&dmacAddr.arEther[4]);
	dbus_message_iter_append_basic (&iter,				
	                                  DBUS_TYPE_BYTE,&dmacAddr.arEther[5]);
	dbus_message_iter_append_basic (&iter,				
	                                  DBUS_TYPE_BYTE,&smacAddr.arEther[0]);
	dbus_message_iter_append_basic (&iter,				
	                                  DBUS_TYPE_BYTE,&smacAddr.arEther[1]);
	dbus_message_iter_append_basic (&iter,				
	                                  DBUS_TYPE_BYTE,&smacAddr.arEther[2]);
	dbus_message_iter_append_basic (&iter,				
	                                  DBUS_TYPE_BYTE,&smacAddr.arEther[3]);
	dbus_message_iter_append_basic (&iter,				
	                                  DBUS_TYPE_BYTE,&smacAddr.arEther[4]);
	dbus_message_iter_append_basic (&iter,				
	                                  DBUS_TYPE_BYTE,&smacAddr.arEther[5]);
	dbus_message_iter_append_basic (&iter,		
	                                  DBUS_TYPE_UINT32,&hostGroupCntPtr.gtHostInPkts);
	dbus_message_iter_append_basic (&iter,		
	                                  DBUS_TYPE_UINT32,&hostGroupCntPtr.gtHostOutBroadcastPkts);
	dbus_message_iter_append_basic (&iter,			
	                                  DBUS_TYPE_UINT32,&hostGroupCntPtr.gtHostOutMulticastPkts);
	dbus_message_iter_append_basic (&iter,				
	                                  DBUS_TYPE_UINT32,&hostGroupCntPtr.gtHostOutPkts);
	dbus_message_iter_append_basic (&iter,				
	                                  DBUS_TYPE_UINT32,&matrixCntSaDaPkts);
	return reply;   
	 
 }


DBusMessage * npd_dbus_sys_host_dmac_ingress_counter
(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
)
	
{
	DBusMessage*   reply = NULL;
	DBusMessageIter iter;
	DBusError err;
    NETHERADDR dmacAddr;
	 int ret =0;
	int result = 0;
	
    dbus_error_init(&err);
	memset(&dmacAddr,0,sizeof(NETHERADDR));
	syslog_ax_product_dbg("enter host ingress counter ");
	if(!(dbus_message_get_args(msg,&err,

                                        DBUS_TYPE_BYTE,&dmacAddr.arEther[0],
	                                    DBUS_TYPE_BYTE,&dmacAddr.arEther[1],
						                DBUS_TYPE_BYTE,&dmacAddr.arEther[2],
										DBUS_TYPE_BYTE,&dmacAddr.arEther[3],
										DBUS_TYPE_BYTE,&dmacAddr.arEther[4],
										DBUS_TYPE_BYTE,&dmacAddr.arEther[5],
                                        DBUS_TYPE_INVALID ))){
			  
				npd_syslog_err("Unable to get input args!\n ");
				if (dbus_error_is_set(&err)) {
				     npd_syslog_err("%s raised: %s\n",err.name,err.message);
				     dbus_error_free(&err);
				}
			return NULL;
	    }
	else{  
		syslog_ax_product_dbg("the mac value is %02x%02x%02x%02x%02x%02x\n",dmacAddr.arEther[0],
		dmacAddr.arEther[1],dmacAddr.arEther[2],dmacAddr.arEther[3],dmacAddr.arEther[4],dmacAddr.arEther[5]);

		ret = nam_ingress_host_counter_dmac_set(&dmacAddr);
	}
		reply = dbus_message_new_method_return(msg);
		dbus_message_iter_init_append (reply, &iter);
		dbus_message_iter_append_basic (&iter,	DBUS_TYPE_UINT32, &ret);

		return reply;

}
DBusMessage * npd_dbus_sys_host_smac_ingress_counter
(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
)
	
{
	DBusMessage*   reply = NULL;
	DBusMessageIter iter;
	DBusError err;
    NETHERADDR smacAddr;
    unsigned int ret =0;
	
    dbus_error_init(&err);
	memset(&smacAddr,0,sizeof(NETHERADDR));
	if(!(dbus_message_get_args(msg,&err,

										DBUS_TYPE_BYTE,&smacAddr.arEther[0],
										DBUS_TYPE_BYTE,&smacAddr.arEther[1],
										DBUS_TYPE_BYTE,&smacAddr.arEther[2],
										DBUS_TYPE_BYTE,&smacAddr.arEther[3],
										DBUS_TYPE_BYTE,&smacAddr.arEther[4],
										DBUS_TYPE_BYTE,&smacAddr.arEther[5],
                                        DBUS_TYPE_INVALID ))){
			  
				npd_syslog_err("Unable to get input args!\n ");
				if (dbus_error_is_set(&err)) {
				     npd_syslog_err("%s raised: %s\n",err.name,err.message);
				     dbus_error_free(&err);
				}
			return NULL;
	    }
	else{  
		syslog_ax_product_dbg("the mac value is %02x%02x%02x%02x%02x%02x\n",smacAddr.arEther[0],
		smacAddr.arEther[1],smacAddr.arEther[2],smacAddr.arEther[3],smacAddr.arEther[4],smacAddr.arEther[5]);

		ret = nam_ingress_host_counter_smac_set(&smacAddr); 
	}
		reply = dbus_message_new_method_return(msg);

		dbus_message_iter_init_append (reply, &iter);

		dbus_message_iter_append_basic (&iter,	DBUS_TYPE_UINT32, &ret);

		return reply;


}
DBusMessage * npd_dbus_sys_host_ingress_counter
(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
)
	
{
	DBusMessage*   reply = NULL;
	DBusMessageIter iter;
	DBusError err;
    NETHERADDR dmacAddr;
	NETHERADDR smacAddr;
    unsigned int ret =0;
	
    dbus_error_init(&err);
	syslog_ax_product_dbg("enter host ingress counter ");

	memset(&dmacAddr,0,sizeof(NETHERADDR));
	memset(&smacAddr,0,sizeof(NETHERADDR));
	if(!(dbus_message_get_args(msg,&err,

                                        DBUS_TYPE_BYTE,&dmacAddr.arEther[0],
	                                    DBUS_TYPE_BYTE,&dmacAddr.arEther[1],
						                DBUS_TYPE_BYTE,&dmacAddr.arEther[2],
										DBUS_TYPE_BYTE,&dmacAddr.arEther[3],
										DBUS_TYPE_BYTE,&dmacAddr.arEther[4],
										DBUS_TYPE_BYTE,&dmacAddr.arEther[5],
										DBUS_TYPE_BYTE,&smacAddr.arEther[0],
										DBUS_TYPE_BYTE,&smacAddr.arEther[1],
										DBUS_TYPE_BYTE,&smacAddr.arEther[2],
										DBUS_TYPE_BYTE,&smacAddr.arEther[3],
										DBUS_TYPE_BYTE,&smacAddr.arEther[4],
                                        DBUS_TYPE_BYTE,&smacAddr.arEther[5],										
                                        DBUS_TYPE_INVALID ))){
			  
				npd_syslog_err("Unable to get input args!\n ");
				if (dbus_error_is_set(&err)) {
				     npd_syslog_err("%s raised: %s\n",err.name,err.message);
				     dbus_error_free(&err);
				}
			return NULL;
	    }
        ret = nam_ingress_host_counter_set(&dmacAddr,&smacAddr);
		 
		reply = dbus_message_new_method_return(msg);

		dbus_message_iter_init_append (reply, &iter);

		dbus_message_iter_append_basic (&iter,	DBUS_TYPE_UINT32, &ret);

		return reply;


}


/*******************************************************************************
* npd_dbus_arp_aging_destmac_broadcast
*
* DESCRIPTION:
*      Set whether the CHIP do the smac-check or not.
*
* INPUTS:
*	  broadcast :
*                     1--set broadcast
*                     0--set unicast
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       reply.
*
* COMMENTS:
*       
*
*******************************************************************************/
DBusMessage * npd_dbus_arp_aging_destmac_broadcast(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	
	unsigned int	isBroadCast  = 0;
	unsigned int	ret = NPD_SUCCESS;
	
	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT32,&isBroadCast,
									DBUS_TYPE_INVALID))) {
		syslog_ax_product_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				syslog_ax_product_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
		ret = NPD_FAIL;
	}
    ret = nam_arp_aging_dest_mac_broadcast(isBroadCast);
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	return reply;
	
}

DBusMessage * npd_dbus_promis_port_add2_vlan_intf(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	DBusMessage* reply;
	unsigned int permit = 1;
	unsigned int ret = NPD_SUCCESS;
	
	DBusError err;
		dbus_error_init(&err);
		if (!(dbus_message_get_args ( msg, &err,
										DBUS_TYPE_UINT32,&permit,
										DBUS_TYPE_INVALID))) {
			syslog_ax_product_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
					syslog_ax_product_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
			}
			ret = NPD_FAIL;
		}
		ret = npd_promis_port_add2_vlan_intf(permit);
		
		reply = dbus_message_new_method_return(msg);
		
		dbus_message_append_args(reply,
								 DBUS_TYPE_UINT32,&ret,
								 DBUS_TYPE_INVALID);
		return reply;

}

unsigned int npd_product_adv_routing_cmd_support(void)
{
	if((PRODUCT_ID_AX7K == PRODUCT_ID)||\
	        (PRODUCT_ID_AX5K == PRODUCT_ID)||\
	        (PRODUCT_ID_AX5K_I == PRODUCT_ID)||\
	        (PRODUCT_ID_AX7K_I == PRODUCT_ID)){
	        return TRUE;
	}
	return FALSE;
}


#ifdef __cplusplus
}
#endif
