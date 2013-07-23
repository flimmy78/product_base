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
* npd_main.c
*
*
* CREATOR:
*		chenb@autelan.com
*
* DESCRIPTION:
*		NPD module main routine
*
* DATE:
*		12/21/2007	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.65 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>

#include "sysdef/npd_sysdef.h"
#include "util/npd_list.h"
#include "npd/nam/npd_amapi.h"
#include "npd/nbm/npd_bmapi.h"
#include "npd_cpss_acl_def.h"
#include "npd_product.h"
#include "npd_c_slot.h"
#include "npd_e_slot.h"

#include "npd_engine.h"
#include "npd_eth_port.h"
#include "npd_log.h"
#include "npd_dbus.h"
#include "npd_vlan.h"
#include "npd_trunk.h"
#include "npd_intf.h"
#include "npd_acl.h"
#include "npd_qos.h"
#include "npd_tunnel.h"
#include "npd_rstp_common.h"
#include "npd_igmp_snp_com.h"
#include "npd_pvlan.h"
#include "npd_dldp_common.h"
#include "npd_main.h"
#include "npd_board.h"
#include "npd_communication.h"
#include "board/netlink.h"
#include "nbm/nbm_api.h"

/*
  Sometimes we need version information to check compatibility, so this part is not customisable.
*/
#define SW_MAJ_V 1
#define SW_MIN_V 0
#define SW_BUI_V 0
#define SW_COM_V 0

unsigned char sw_maj_v = 1;
unsigned char sw_min_v = 0;
unsigned char sw_comp_v = 0;
unsigned short sw_build_v = 0;



/**
  *  SW version and build number now read from RAMFS /etc/version directory files 'version' and 'buildno'
  *  all version number and build number saved as ASCII code
  */
#define SW_VERSION_FILE 	"/etc/version/version"
#define SW_BUILD_NO_FILE	"/etc/version/buildno"
#define SW_VERSION_SPILTER	'.'
#define SW_VERSION_BUFSIZE	32

extern void npd_prot_vlan_init(void);


void npd_init_sw_version(void) {
	char buffer[SW_VERSION_BUFSIZE] = {0};
	int fd = 0, length = 0, i = 0,j = 0;
	unsigned char version[3] = {0};
	unsigned short buildno = 0;

	/* set default value */	
	sw_maj_v = SW_MAJ_V;
	sw_min_v = SW_MIN_V;
	sw_comp_v  = SW_COM_V;
	sw_build_v = SW_BUI_V;
	
	/* read SW version */
	fd = open(SW_VERSION_FILE,0);
	if(fd < 0) {
		syslog_ax_main_err("open version file %s error when query SW version.\n",SW_VERSION_FILE);
	}
	else {
		length = read(fd,buffer,SW_VERSION_BUFSIZE);
		if(length) {
			for(i = 0,j = 0; i < length; i++) {
				if(('\n' == buffer[i]) || 
					(SW_VERSION_SPILTER == buffer[i])) {
					j++;
				}
				else if(j < 3) { /* we need only 3 sub section of version number*/
					version[j] *= (10);
					version[j] += (buffer[i] - '0');
				}
			}/* end for(...)*/
			sw_maj_v = version[0];
			sw_min_v = version[1];
			sw_comp_v = version[2];
		}
		close(fd);
	}

	/* read SW build number */
	fd = open(SW_BUILD_NO_FILE,0);
	if(fd < 0) {
		syslog_ax_main_err("open build number file %s error when query SW version.\n",SW_BUILD_NO_FILE);
	}
	else {
		length = read(fd,buffer,SW_VERSION_BUFSIZE);
		if(length) {
			for(i=0;i<length;i++) {
				if('\n' == buffer[i]) {/* just read first line*/
					break;
				}
				buildno *= (10);
				buildno += (buffer[i] - '0');
			}
			sw_build_v = buildno;
		}
		close(fd);
	}

	return;
}
/*
  NOTICE: 
	1. Sub-slot in a slot information will be processed specially.
		a. slot no part of port index will be defined based on .
		b. sub-slot information will be initialized in extend_info of card_info.
*/


unsigned int npd_query_sw_version(void) {
	return SW_INT_VER(sw_maj_v,sw_min_v,sw_comp_v,sw_build_v);
}


int main(int argc,char **argv)
{

/*
	Tell my thread id
*/
	npd_init_tell_whoami((unsigned char *)"NpdMain",0);

	/* TODO: how many threads should be created here need to be considered carefully later*/
/*
      Open the syslog to record the init info
*/
    npd_log_set_debug_value(SYSLOG_DBG_ALL);
 /* not need any longer 
 	nam_log_set_debug_value(SYSLOG_DBG_ALL);
	nbm_log_set_debug_value(SYSLOG_DBG_ALL);
	npd_asic_debug_set(SYSLOG_DBG_ALL);
	*/	

/* 
	Read software version and build number
 */
 	npd_init_sw_version();
	syslog_ax_main_dbg("system software version %d.%d.%d build %d\n",sw_maj_v,sw_min_v,sw_comp_v,sw_build_v);

/*
	Get board info from /proc/board/
*/	
	syslog_ax_main_dbg("init board info\n");
	npd_init_boardinfo();
/*
    Add for init the function of distributed system.
*/
    npd_asic_func_init(SYSTEM_TYPE);

/*
	Read cpld and get product id and module id information.
*/	

	syslog_ax_main_dbg("init product info\n");

	npd_init_productinfo();

/*
	Read cpld and GPIO to get system accessories' state
*/
	syslog_ax_main_dbg("init system state\n");
	npd_init_system_stateinfo();
	
/*	
	Probe chassis slot status and build slots data
*/
	syslog_ax_main_dbg("init chassis info\n");
	npd_init_chassis_slots();

/*
	Probe extend slot information	
*/	
	syslog_ax_main_dbg("init extend slot info\n");
	npd_init_extend_slot();

/*
	Start probe thread to monitor slot status change
*/
	if(PRODUCT_ID_AX7K == PRODUCT_ID) {
		syslog_ax_main_dbg("init chassis probe thread\n");
		npd_init_board_check_thread();	
	}

#if 0   /* Remove here, move to ksem. */
/*
    Start probe thread to monitor temperature for 8610
*/
    if(SYSTEM_TYPE == IS_DISTRIBUTED){
        nam_thread_create("Board8610StateCheck",(void*)nbm_check_8610_board_state,NULL,NPD_TRUE,NPD_FALSE);
    }
#endif
	npd_fw_engines_init();

/*
	Wait until ASIC initialization process done
*/
	npd_fw_engine_initialization_check();
	syslog_ax_main_dbg("ASIC initialization done!\n");

#if 0
/*
    Start DBM(Distributed Board Management)
*/
    npd_dbm_init();
#endif
/*
    Protocol vlan 
*/

    npd_prot_vlan_init();
/*
	Build port data
*/
	syslog_ax_main_dbg("init eth port info\n");
	npd_init_eth_ports();

/*
	Get ethernet port default state
*/
	syslog_ax_main_dbg("read port status info from asic\n");
	npd_get_eth_ports_status();
	
	npd_eth_port_register_notifier_hook();

/*
	Build XG port data
*/
	syslog_ax_main_dbg("init eslot port info\n");
	npd_init_eslot_port();	

/*
	Build vlan data
*/
	syslog_ax_main_dbg("init vlan info\n");

	npd_init_vlan();

/*
	Build trunk data
*/
	syslog_ax_main_dbg("init trunk info\n");

	npd_init_trunks();
	npd_dynamic_trunk_init();

/*
    Build connect table
*/
    syslog_ax_main_dbg("init connect table");

    npd_init_connect_table();

/*
	Build L3 interface data
*/
	syslog_ax_main_dbg("init intf info\n");

	npd_intf_init();

/*
	Init OAM traffic settings
*/
	syslog_ax_main_dbg("init oam setting\n");
	npd_init_oam();

/*
	Init Acl Rule structure
 */
 	npd_acl_init();

/*
	Init x-over-ip tunnel 
*/
	npd_tunnel_init();

/*
   Init QOS structure
*/
	npd_qos_init();

/*
	Init mirror init();	
*/
	npd_mirror_init();
/*
	Init ARP Snooping Hash table
 */
 	npd_init_arpsnooping();

/*
	Init static FDB database
 */
 	npd_fdb_static_init();

	/*Init FDB blacklist database*/
    npd_fdb_blacklist_init();
#if 1
	/*Init fdb hash table*/
	npd_fdb_init_hash();
#endif
/*
	Init private vlan
*/
	npd_pvlan_init();

/*
	add netlink sysinfo
*/
	npd_route_init();

/*
	Init dbus to get ready for accept management command
*/	
	npd_dbus_init();

#ifdef CPU_ARM_XCAT
	nbm_open_laser();
	syslog_ax_main_dbg("open laser for xcat finish.\n");
#endif

/*
	Init different board
*/
	syslog_ax_main_dbg("init board.\n");
	npd_init_board();

/* 
 *	npd recieve netlink message from ksem and sem module. Jia Lihui 2011-9-29
 */
    nam_thread_create("Npd_netlink_recv", (void *)npd_netlink_recv_thread, NULL, NPD_TRUE, NPD_FALSE);

/* 
	Init socket for packet Rx/Tx
*/
	syslog_ax_main_dbg("init packet sockets\n");
	npd_init_packet_socket();

	nam_thread_create("RstpSock",(void *)npd_rstp_sock_init,NULL,NPD_TRUE,NPD_FALSE);

	if(PRODUCT_ID_AX5K_E != PRODUCT_ID &&  PRODUCT_ID_AX5608!= PRODUCT_ID){

		/*stream sock*/
		/*nam_thread_create("IgmpSnpSock",(void *)npd_igmp_snp_mng_thread,NULL,NPD_TRUE,NPD_FALSE);*/
		nam_thread_create("IgmpSnpSock",(void *)npd_igmp_snp_mng_thread_dgram,NULL,NPD_TRUE,NPD_FALSE);

		/* tunnel netlink */
		nam_thread_create("TunnelNetlink",(void *)npd_tunnel_recv_netlink_msg,NULL,NPD_TRUE,NPD_FALSE);

		nam_thread_create("dldpSock", (void *)npd_dldp_mng_thread_dgram, NULL, NPD_TRUE, NPD_FALSE);

	}

	
/* 
	check if local board has started up correctly and then open asic interrupt
*/
    nam_thread_create("SysWaitStatePoll",(void *)npd_check_system_startup_state,NULL,NPD_TRUE,NPD_FALSE);


#if 0
/*
	  close the syslog 
*/
	npd_log_set_no_debug_value(SYSLOG_DBG_ALL);

	/* It may affect normal running of the SEM module. */
    if(BOARD_TYPE != BOARD_TYPE_AX81_SMU)
        npd_communication_init();
#endif
/*   not needed any longer
	nam_log_set_no_debug_value(SYSLOG_DBG_ALL);
	nbm_log_set_no_debug_value(SYSLOG_DBG_ALL);

	npd_asic_debug_clr(SYSLOG_DBG_ALL);
*/
	/*set log level to default*/
	npd_log_level_set(SYSLOG_DBG_DEF);
	if(npd_log_level && npd_log_close){/*if need but not opened ,open it*/
		openlog(npd_log_ident, 0, LOG_DAEMON); 
		npd_log_close = 0;
	}
	if(PRODUCT_ID_AX5K_E != PRODUCT_ID && PRODUCT_ID_AX5608!= PRODUCT_ID) {
		nam_thread_create("NpdDBUS",(void *)npd_dbus_thread_main,NULL,NPD_TRUE,NPD_TRUE);
	}
	else {
		nam_thread_create("NpdDBUS",(void *)npd_dbus_thread_main,NULL,NPD_FALSE,NPD_TRUE);
	}
	
	return 0;
}
#ifdef __cplusplus
}
#endif
