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
* npd_dbus.c
*
*
* CREATOR:
*		chenb@autelan.com
*
* DESCRIPTION:
*		dbus message main routine for NPD module.
*
* DATE:
*		12/21/2007	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.184 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
/*
  NPD dbus implementation
 */
#include <string.h>
#include <dbus/dbus.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "sysdef/npd_sysdef.h"
#include "dbus/npd/npd_dbus_def.h"
#include "util/npd_list.h"
#include "npd/nbm/npd_bmapi.h"
#include "npd/nam/npd_amapi.h"
#include "npd_product.h"
#include "npd_c_slot.h"
#include "npd_vlan.h"
#include "npd_eth_port.h"
#include "npd_main.h"
#include "npd_log.h"
#include "npd_dbus.h"
#include "npd_rstp_common.h"
#include "npd_trunk.h"
#include "npd_dynamic_trunk.h"
#include "npd_acl_api.h"
#include "npd_qos.h"
#include "npd_intf.h"
#include "npd_igmp_snp_com.h"
#include "nam/nam_main_api.h" 
#include "npd_pvlan.h"
#include "npd_fdb.h"
#include "npd_engine.h"
#include "npd_dldp_common.h"
#include "npd_dhcp_snp_com.h"

 DBusConnection *npd_dbus_connection = NULL;
static int reinit_count = 1;
unsigned char dbusThreadName[32] = {0};

static DBusHandlerResult npd_dbus_message_handler (DBusConnection *connection, DBusMessage *message, void *user_data)
{
	DBusMessage		*reply = NULL;
		
	if (strcmp(dbus_message_get_path(message),NPD_DBUS_OBJPATH) == 0) {
    	/*syslog_ax_dbus_dbg("npd obj path"NPD_DBUS_OBJPATH"\r\n");*/
		if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_INTERFACE_METHOD_VER)) {
			reply = npd_dbus_interface_show_ver(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_INTERFACE_METHOD_HWCONF)) {
			reply = npd_dbus_interface_show_hwconf(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_SHOW_STATE)) {
			reply = show_system_environment_state(connection,message,user_data);
		}	
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_SHUTDOWN_STATE)) {
			reply = npd_system_shut_down_enable(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_ARP_CHECK_STATE)) {
			reply = npd_system_arp_smac_check_enable(connection,message,user_data);
		}		
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_ARP_STRICT_CHECK_STATE)) {
			reply = npd_system_arp_strict_check_enable(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_INTERFACE_METHOD_SYSTEM_DEBUG_STATE)) {
			reply = npd_system_debug_enable(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_INTERFACE_METHOD_SYSTEM_UNDEBUG_STATE)) {
			reply = npd_system_debug_disable(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NBM_DBUS_METHOD_SYSLOG_DEBUG)) {
			reply = nbm_dbus_interface_syslog_debug(connection,message,user_data);
		 }
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NBM_DBUS_METHOD_SYSLOG_NO_DEBUG)) {
			reply = nbm_dbus_interface_syslog_no_debug(connection,message,user_data);
		 }
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_CONFIG_COUNTER_DROP_STATISTIC)) {
			reply = npd_dbus_sys_global_config_counter_drop_statistic(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_SHOW_COUNTER_DROP_STATISTIC)) {
			reply = npd_dbus_sys_global_counter_drop_statistic(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_CONFIG_EGRESS_COUNTER)) {
			reply = npd_dbus_sys_egress_drop_statistic(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE, NPD_DBUS_CONFIG_VLAN_EGRESS_COUNTER)) {
			reply = npd_dbus_vlan_egress_drop_statistic(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_SHOW_COUNTER_EGRESS)) {
			reply = npd_dbus_sys_show_egress_counter(connection,message,user_data);
	   }
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_SHOW_COUNTER_INGRESS)) {
			reply = npd_dbus_sys_show_ingress_counter(connection,message,user_data);
	   }
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_CONFIG_HOST_DMAC_INGRESS_COUNTER)) {
			reply = npd_dbus_sys_host_dmac_ingress_counter(connection,message,user_data);
	   }
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_CONFIG_HOST_SMAC_INGRESS_COUNTER)) {
			reply = npd_dbus_sys_host_smac_ingress_counter(connection,message,user_data);
	   }
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_CONFIG_HOST_INGRESS_COUNTER)) {
			reply = npd_dbus_sys_host_ingress_counter(connection,message,user_data);
	   }
		else  if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NAM_DBUS_METHOD_SYSLOG_DEBUG)) {
			reply = nam_dbus_interface_syslog_debug(connection,message,user_data);
		 }
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NAM_DBUS_METHOD_SYSLOG_NO_DEBUG)) {
			reply = nam_dbus_interface_syslog_no_debug(connection,message,user_data);
		 }
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_METHOD_ASIC_SYSLOG_DEBUG)) {
			reply = npd_dbus_asic_syslog_debug(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_METHOD_ASIC_SYSLOG_NO_DEBUG)) {
			reply = npd_dbus_asic_syslog_no_debug(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_METHOD_CPU_PROTECTION_CONFIG)) {
			reply = npd_dbus_asic_bcast_rate_limiter(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_METHOD_CPU_FC_CONFIG)) {
			reply = npd_dbus_cpufc_config(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_METHOD_SYS_GLOBAL_CFG_SAVE)) {
			reply = npd_dbus_sys_global_cfg_save(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_METHOD_CPUFC_SET_QUEUE_QUOTA)) {
			reply = npd_dbus_cpufc_set_queue_quota(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_METHOD_CPUFC_GET_QUEUE_QUOTA)) {
			reply = npd_dbus_cpufc_get_queue_quota(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_METHOD_CPUFC_SET_QUEUE_SHAPER)) {
			reply = npd_dbus_cpufc_set_queue_shaper(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_METHOD_CPUFC_SET_PORT_SHAPER)) {
			reply = npd_dbus_cpufc_set_port_shaper(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_METHOD_CPU_SHOW_QUEUE_DESC)) {
			reply = npd_dbus_cpu_show_sdma_info(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_METHOD_CPU_SHOW_PORT_MIB)) {
			reply = npd_dbus_cpu_show_sdma_mib(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_METHOD_CPU_CLEAR_PORT_MIB)) {
			reply = npd_dbus_cpu_clear_sdma_mib(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_ARP_AGING_DEST_MAC)) {
			reply = npd_dbus_arp_aging_destmac_broadcast(connection,message,user_data);
		}
		
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_CONFIG_PROMIS_PORT_ADD2_INTERFACE)){
            reply = npd_dbus_promis_port_add2_vlan_intf(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_DIAGNOSIS_PHY_HW_RW_REG)){
            reply = npd_dbus_diagnosis_phy_hw_rw_reg(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_DIAGNOSIS_XAUI_PHY_HW_RW_REG)){
            reply = npd_dbus_diagnosis_xaui_phy_hw_rw_reg(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_SHOW_MIB_GE_XG_PORT)){
            reply = npd_dbus_show_mib_ge_xg(connection,message,user_data);
		}

		/* add for create vlan on debug node */
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_DIAG_CREATE_VLAN)){
            reply = npd_dbus_diagnosis_create_vlan(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_DIAG_VLAN_FDB_DELETE)){
            reply = npd_dbus_diagnosis_vlan_fdb_delete(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_DIAG_SHOW_CSCD_PORT_STATUS)){
            reply = npd_dbus_diagnosis_show_cscd_port_status(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_DIAG_BOARD_TEST)){
            reply = npd_dbus_diagnosis_board_test_mode_set(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_DIAG_VLAN_ADD_DEL_PORT)){
            reply = npd_dbus_diagnosis_vlan_add_del_port(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_DIAGNOSIS_SET_AP_FAKE_SINGAL_STRENGTH)){
            reply = npd_dbus_diagnosis_set_ap_fake_singal_strength(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_DIAG_READ_ASIC_MIB)){
            reply = npd_dbus_diagnosis_show_port_mib(connection,message,user_data);
		}				
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_METHOD_GET_PORT_RATE)) 
		{
			reply = npd_dbus_ethports_interface_get_port_rate(connection,message,user_data);
		} 
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_DIAG_ENDIS_ASIC)){
            reply = npd_dbus_diagnosis_endis_asic(connection,message,user_data);
		}	
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_DIAG_PORT_CSCD_MODE_SET)){
            reply = npd_dbus_diagnosis_port_cscd_mode_set(connection,message,user_data);
		}	
		/* add for test MCB state changed */
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_MCB_STATE_TEST)){
            reply = npd_dbus_mcb_state_change_test(connection,message,user_data);
		}
		/*show cpu temperature*/
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_CPU_TEMPERATURE_TEST)){
            reply = npd_dubs_equipment_cpu_temperature_test(connection,message,user_data);
		}
		/*add lion 1 enable trunk for test*/
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_LION1_TRUNK_TEST)){
            reply = npd_dubs_equipment_lion1_trunk_test(connection,message,user_data);
		}
		/* add for ax81-1x12g12s equipment test */	
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_TRUNK_PORT_TEST)){
            reply = npd_dubs_equipment_1x12g12s_trunk_port_test(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_GE_XG_TEST)){
            reply = npd_dbus_equipment_test(connection,message,user_data);
		}		
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_AX8610_PRBS_TEST)){
            reply = npd_dbus_hw_test_ax8610_prbs(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_DIAGNOSIS_PCI_HW_RW_REG)){
            reply = npd_dbus_diagnosis_pci_hw_rw_reg(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_DIAGNOSIS_CPU_HW_RW_REG)){
            reply = npd_dbus_diagnosis_cpu_hw_rw_reg(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_DIAGNOSIS_CPLD_HW_RW_REG)){
            reply = npd_dbus_diagnosis_cpld_hw_rw_reg(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message, NPD_DBUS_INTERFACE, NPD_DBUS_SYSTEM_DIAGNOSIS_EEPROM_HW_RW_REG)){
            reply = npd_dbus_diagnosis_eeprom_hw_rw_reg(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_DIAGNOSIS_MAC_HW_RW_REG)){
            reply = npd_dbus_diagnosis_mac_hw_rw_reg(connection,message,user_data);
		}		
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_CONFIG_ARP_MUTI_NETWORK)){
			reply = npd_dbus_arp_muti_network_enable(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_DIAGNOSIS_HW_RW_REG)){
            reply = npd_dbus_diagnosis_hw_rw_reg(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_DIAGNOSIS_HW_PRBS_TEST)){
            reply = npd_dbus_diagnosis_hw_prbs_test(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_DIAGNOSIS_HW_PORT_MODE_SET)){
            reply = npd_dbus_diagnosis_hw_port_mode_set(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_DIAGNOSIS_HW_CSCD_PORT_PRBS_TEST)){
            reply = npd_dbus_diagnosis_hw_cscd_port_prbs_test(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_DIAGNOSIS_FIELD_HW_DUMP_TAB)){
          		  reply = npd_dbus_diagnosis_dump_hw_field_tab(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_DIAGNOSIS_COSQ_HW_DUMP_TAB)){
           		 reply = npd_dbus_diagnosis_dump_hw_cosq_tab(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_DIAGNOSIS_HW_DUMP_TAB)){
           		 reply = npd_dbus_diagnosis_dump_hw_tab(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE, NPD_DBUS_SYSTEM_DIAGNOSIS_ACL_HW_SHOW)){
            		reply = npd_dbus_diagnosis_show_hw_acl(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTERFACE, NPD_DBUS_METHOD_SYSTEM_WATCHDOG_CONTROL)) {
			reply = npd_dbus_diagnosis_hw_watchdog_control(connection, message, user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTERFACE, NPD_DBUS_METHOD_SYSTEM_WATCHDOG_TIMEOUT)) {
			reply = npd_dbus_diagnosis_hw_watchdog_timeout_op(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE, NPD_DBUS_IP_TUNNEL_ADD)) {
			reply = npd_dbus_config_ip_tunnel_add(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE, NPD_DBUS_IP_TUNNEL_DELETE)) {
			reply = npd_dbus_config_ip_tunnel_delete(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_IP_TUNNEL_HOST_ADD)) {
			reply = npd_dbus_config_ip_tunnel_host_add(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_IP_TUNNEL_HOST_DELETE)) {
			reply = npd_dbus_config_ip_tunnel_host_delete(connection,message,user_data);

		}
		else if (dbus_message_is_method_call(message, NPD_DBUS_INTERFACE, NPD_DBUS_IP_TUNNEL_SHOW_TAB)){
            		reply = npd_dbus_ip_tunnel_show_tab(connection,message,user_data);
		}	
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_FDB_METHOD_CREATE_VRRP_BY_IFNAME))
		{
			reply = npd_dbus_create_vrrp_by_ifname(connection,message,user_data);
		}	
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_CONFIG_ARP_INSPECTION)){
			reply = npd_dbus_arp_inspection_enable(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_CONFIG_ARP_PROXY)){
			reply = npd_dbus_arp_proxy_enable(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTERFACE,NPD_DBUS_SYSTEM_SHOW_IP_NEIGH)){
			reply = npd_dbus_show_ip_neigh(connection,message,user_data);
		}
	} 
	else if (strcmp(dbus_message_get_path(message),NPD_DBUS_SLOTS_OBJPATH) == 0)  {
			syslog_ax_dbus_dbg("npd obj path"NPD_DBUS_SLOTS_OBJPATH);
	} 
	else if(strcmp(dbus_message_get_path(message),NPD_DBUS_VLAN_OBJPATH) == 0) {
		syslog_ax_dbus_dbg("npd obj path"NPD_DBUS_VLAN_OBJPATH);

		if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE, \
										NPD_DBUS_VLAN_METHOD_CREATE_VLAN_ONE)) {
			reply = npd_dbus_vlan_create_vlan_entry_one(connection,message,user_data);
		}/*create_vlan_interface_node in SW&HW.*/
		else if(dbus_message_is_method_call(message, NPD_DBUS_VLAN_INTERFACE,	\
										NPD_DBUS_VLAN_METHOD_CONFIG_LAYER2_ONE)) {

			reply = npd_dbus_vlan_config_layer2(connection,message,user_data);
		}/*enter_vlan_config_node.*/
		else if(dbus_message_is_method_call(message, NPD_DBUS_VLAN_INTERFACE,	\
										NPD_DBUS_VLAN_METHOD_CONFIG_LAYER2_VIA_VLANNAME)) {

			reply = npd_dbus_vlan_config_layer2_vname(connection,message,user_data);
		}/*enter_vlan_config_node.*/
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE, \
										NPD_DBUS_VLAN_METHOD_CONFIG_PORT_MEMBER_QINQ_ENDIS)) {
			reply = npd_dbus_vlan_config_qinq_endis(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE, \
										NPD_DBUS_VLAN_METHOD_CONFIG_ALLBACKPORT_QINQ_ENDIS)) {
			reply = npd_dbus_vlan_config_allbackport_qinq_endis(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE, \
										NPD_DBUS_VLAN_METHOD_CONFIG_TOCPUPORT_QINQ_ENDIS)) {
			reply = npd_dbus_vlan_config_tocpuport_qinq_endis(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE, \
										NPD_DBUS_VLAN_METHOD_CONFIG_TOCPUPORT_QINQ_ENDIS_FOR_OLD)) {
			reply = npd_dbus_vlan_config_tocpuport_qinq_endis_for_old(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE, \
										NPD_DBUS_VLAN_METHOD_CONFIG_TOCPUPORT_QINQ_UPDATE_FOR_MASTER)) {
			reply = npd_dbus_vlan_config_tocpuport_qinq_update_for_master(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE, \
										NPD_DBUS_VLAN_METHOD_CONFIG_QINQ_UPDATE_FOR_MASTER)) {
			reply = npd_dbus_vlan_config_qinq_update_for_master(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE, \
										NPD_DBUS_VLAN_METHOD_CONFIG_PORT_MEMBER_ADD_DEL)) {
			reply = npd_dbus_vlan_config_port_add_del(connection, message, user_data);
		}/*add or del vlan port member */
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE, \
										NPD_DBUS_VLAN_METHOD_CONFIG_VLANLIST_PORT_MEMBER_ADD_DEL)) {
			reply = npd_dbus_vlan_config_vlanlist_port_add_del(connection, message, user_data);
		}/*add or del vlan port member,this for update g_vlanlist[] on SMU. zhangdi 2011-06-14 */		
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE, \
										NPD_DBUS_VLAN_METHOD_UPDATE_VID)){
			reply = npd_dbus_vlan_config_vlan_update_vid(connection,message,user_data);
		}/*update_vlan_id*/
		#if 0
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE, \
										NPD_DBUS_VLAN_METHOD_CONFIG_TRUNK_MEMBER_ADD_DEL)) {
			reply = npd_dbus_vlan_config_trunk_add_del(connection, message, user_data);
		}/*add or del vlan TRUNK member */
		#endif
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE, \
										NPD_DBUS_VLAN_METHOD_CONFIG_TRUNK_MEMBER_UNTAG_TAG_ADD_DEL)) {
			reply = npd_dbus_vlan_config_trunk_untag_tag_add_del(connection, message, user_data);
		}/*add or del vlan TRUNK member */
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE, \
										NPD_DBUS_VLAN_METHOD_SHOW_VLAN_PORT_MEMBERS)) 
		{
			reply = npd_dbus_vlan_show_vlan_port_member(connection,message,user_data);
		}/*show vlan(s) port members*/
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE, \
										NPD_DBUS_VLAN_METHOD_SHOW_VLAN_PORT_MEMBERS_V1)) 
		{
			reply = npd_dbus_vlan_show_vlan_port_member_v1(connection,message,user_data);
		}/*show vlan(s) port members*/
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE, \
										NPD_DBUS_VLAN_METHOD_SHOW_VLAN_TRUNK_MEMBERS)) 
		{
			reply = npd_dbus_vlan_show_vlan_trunk_member(connection,message,user_data);
		}/*show vlan(s) trunk members*/
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE, \
										NPD_DBUS_VLAN_METHOD_SHOW_VLAN_PORT_MEMBERS_VNAME)) 
		{
			reply = npd_dbus_vlan_show_vlan_port_member_vname(connection,message,user_data);
		}/*show vlan(s) port members*/
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE, \
										NPD_DBUS_VLAN_METHOD_SHOW_VLAN_PORT_MEMBERS_VNAME_V1)) 
		{
			reply = npd_dbus_vlan_show_vlan_port_member_vname_v1(connection,message,user_data);
		}/*show vlan(s) port members*/
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE, \
										NPD_DBUS_VLAN_METHOD_SHOW_VLAN_TRUNK_MEMBERS_VNAME)) 
		{
			reply = npd_dbus_vlan_show_vlan_trunk_member_vname(connection,message,user_data);
		}/*show vlan(s) trunk members*/

		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_SHOW_VLANLIST_PORT_MEMBERS)) {
			reply = npd_dbus_vlan_show_vlanlist_port_member(connection,message,user_data);
		}/*show vlans port members*/
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_SHOW_VLANLIST_PORT_MEMBERS_V1)) {
			reply = npd_dbus_vlan_show_vlanlist_port_member_v1(connection,message,user_data);
		}/* show vlans port members of distributed system */
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_SHOW_VLANLIST_PORT_MEMBERS_DISTRIBUTED)) {
			reply = npd_dbus_vlan_show_vlanlist_port_member_distributed(connection,message,user_data);
		}/* Bond vlan to cpu port on special slot of distributed system */
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_BOND_VLAN_TO_SLOT_CPU)) {
			reply = npd_dbus_vlan_to_cpu_port_add_del(connection,message,user_data);
		}
		/*show vlans port members*/
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_EXIST_INTERFACE_UNDER_VLAN_TO_SLOT_CPU)) {
			reply = npd_dbus_vlan_to_cpu_exist_interface_under_vlan_on_slot(connection,message,user_data);
		}
		/*show vlans port members*/
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_SHOW_VLANLIST_PORT_MEMBERS_V2)) {
			reply = npd_dbus_vlan_show_vlanlist_port_member_v2(connection,message,user_data);
		}/*show vlans port members*/
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_SHOW_VLANLIST_TRUNK_MEMBERS)) {
			reply = npd_dbus_vlan_show_vlanlist_trunk_member(connection,message,user_data);
		}/*show vlans trunk members*/

		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_CONFIG_DELETE_VLAN_ENTRY)) {
			reply = npd_dbus_vlan_delete_vlan_entry_one(connection,message,user_data);
		}/*delete vlan */
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_DELETE_VLAN_ENTRY_VIA_NAME)) {
			reply = npd_dbus_vlan_delete_vlan_entry_vname(connection,message,user_data);
		}/*delete vlan by vlanname*/
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_SET_ONE_PORT_PVID)) {
			reply = npd_dbus_vlan_set_one_port_pvid(connection,message,user_data);
		}/*show ports pvid */
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_SHOW_ONE_PORT_PVID)) {
			reply = npd_dbus_vlan_show_one_port_pvid(connection,message,user_data);
		}/*show one port pvid */
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_SHOW_PORTS_LIST_PVID)) {
			reply = npd_dbus_vlan_show_ports_pvid(connection,message,user_data);
		}/*show ports pvid */
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_SHOW_EGRESS_FILTER)) {
			reply = npd_dbus_vlan_show_egress_filter(connection,message,user_data);
		}/*show vlan egress filter */
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_SET_PORT_VLAN_INGRES_FLITER)) {
			reply = npd_dbus_vlan_set_port_vlan_ingresflt(connection,message,user_data);
		}/*set port vlan ingress Fliter */
		else if(dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_SHOW_RUNNING_CONFIG)) {
			reply = npd_dbus_vlan_show_running_config(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_EGRESS_FILTER_SHOW_RUNNING_CONFIG)) {
			reply = npd_dbus_vlan_egress_filter_show_running_config(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_CHECK_IGMP_SNP_STATUS)) {
			reply = npd_dbus_vlan_check_igmp_snoop(connection,message,user_data);
		} 
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_CHECK_VLAN_IGMP_SNP_STATUS)) {
			reply = npd_dbus_vlan_check_vlan_igmp_snoop(connection,message,user_data);
		} 
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_CHECK_IGMP_SNP_VLANMBR_STATUS)) {
			reply = npd_dbus_vlan_portmbr_check_igmp_snoop(connection,message,user_data);
		} 
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_IGMP_SNP_VLAN_COUNT)) {
			reply = npd_dbus_vlan_igmp_snoop_count(connection,message,user_data);
		} 
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_CONFIG_IGMP_SNP_VLAN)) {
			reply = npd_dbus_vlan_config_igmp_snoop(connection,message,user_data);
		} 
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_IGMP_SNP_VLAN_LIST_SHOW)) {
			reply = npd_dbus_vlan_igmp_snoop_info_get(connection,message,user_data);
		} 
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_IGMP_SNP_VLAN_LIST_SHOW_V1)) {
			reply = npd_dbus_vlan_igmp_snoop_info_get_v1(connection,message,user_data);
		} 
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										IGMP_DBUS_METHOD_GET_SUPPORT_FUNCTION)) {
			reply = npd_dbus_vlan_igmp_get_product_function(connection,message,user_data);
		} 
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_CONFIG_IGMP_SNP_ETHPORT)) {
			reply = npd_dbus_vlan_portmbr_config_igmp_snoop(connection,message,user_data);
		} 
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_IGMP_SNP_ADD_DEL_MCROUTE_PORT)) {
			reply = npd_dbus_vlan_mcrouterport_config_igmp_snoop(connection,message,user_data);
		} 
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_IGMP_SNP_SHOW_ROUTE_PORT)) {
			reply = npd_dbus_vlan_show_vlan_routeport_member(connection,message,user_data);
		} 
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_IGMP_SNP_SHOW_ROUTE_PORT_V1)) {
			reply = npd_dbus_vlan_show_vlan_routeport_member_v1(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_IGMP_SNP_EXCHANGE_IFINDEX_TO_SLOTPORT)) {
			reply = npd_dbus_vlan_exchange_ifindex_igmp_to_npd(connection,message,user_data);
		} 
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_SHOW_ONE_MCGROUP_PORT_MEMBERS)) {
				syslog_ax_dbus_dbg("vlan IGMP method Match :%s!",NPD_DBUS_VLAN_METHOD_SHOW_ONE_MCGROUP_PORT_MEMBERS);
			reply = npd_dbus_vlan_show_one_group_port_member(connection,message,user_data);
		} 
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_SHOW_VLAN_MCGROUP_LIST_PORT_MEMBERS)) {
			reply = npd_dbus_vlan_show_vlan_group_list_port_mbr(connection,message,user_data);
		} 
		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_SHOW_VLAN_MCGROUP_LIST_PORT_MEMBERS_V1)) {
			reply = npd_dbus_vlan_show_vlan_group_list_port_mbr_v1(connection,message,user_data);
		} 

		else if (dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_GET_SLOTPORT_BY_INDEX)) {
			reply = npd_dbus_vlan_get_slotport_by_ethportindex(connection,message,user_data);
		} 
		else if(dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_CONFIG_MTU)) {
			reply = npd_dbus_vlan_config_vlan_mtu(connection,message,user_data);
		} 
		else if(dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_CONFIG_EGRESS_FILTER)) {
			reply = npd_dbus_vlan_config_vlan_egress_filter(connection,message,user_data);
		}		
		else if(dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_CONFIG_FILTER)) {
			reply = npd_dbus_vlan_config_vlan_filter(connection,message,user_data);
		} 
		else if(dbus_message_is_method_call(message,NPD_DBUS_VLAN_INTERFACE,  \
										NPD_DBUS_VLAN_METHOD_IGMP_SNP_VLAN_SHOW_RUNNING_CONFIG)) {
			reply = npd_dbus_vlan_igmp_snp_show_running_config(connection,message,user_data);
		}
	}  
	else if(strcmp(dbus_message_get_path(message),NPD_DBUS_TRUNK_OBJPATH) == 0){
			syslog_ax_dbus_dbg("npd obj path"NPD_DBUS_TRUNK_OBJPATH);
		if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,	\
								NPD_DBUS_TRUNK_METHOD_CREATE_TRUNK_ONE)) {
			reply = npd_dbus_trunk_create_one(connection,message,user_data);
		}/*create trunk one*/
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,	\
								NPD_DBUS_TRUNK_METHOD_CONFIG_ONE)) {
			reply = npd_dbus_trunk_config_one(connection,message,user_data);
		}/*config trunk Id*/
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,	\
								NPD_DBUS_TRUNK_METHOD_CONFIG_VIA_TRUNKNAME)) {
			reply = npd_dbus_trunk_config_by_name(connection,message,user_data);
		}/*config trunk name*/
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,	\
								NPD_DBUS_TRUNK_METHOD_PORT_MEMBER_ADD_DEL)) {
			reply = npd_dbus_trunk_add_delete_port_member(connection,message,user_data);
		}/*trunk add delete port*/
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,	\
								NPD_DBUS_TRUNK_METHOD_PORT_MEMBER_TRUNK_LIST_ADD_DEL)) {
			reply = npd_dbus_trunk_add_delete_port_trunk_list(connection,message,user_data);
		}/*trunk add delete port*/		
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,	\
								NPD_DBUS_TRUNK_METHOD_TRUNK_MAP_TABLE_UPDATE)) {
			reply = npd_dbus_trunk_update_map_table(connection,message,user_data);
		}/*trunk update map table*/
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,	\
								NPD_DBUS_TRUNK_METHOD_PORT_MEMBER_MASTERSHIP_CONFIG)) {
			reply = npd_dbus_trunk_master_port_set(connection,message,user_data);
		}/*master port config */
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,	\
								NPD_DBUS_TRUNK_METHOD_PORT_MEMBER_ENALBE_DISABLE)) {
			reply = npd_dbus_trunk_port_endis(connection,message,user_data);
		}/*rtunk port enable/disable */
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,	\
								NPD_DBUS_TRUNK_METHOD_CONFIG_LOAD_BANLC_MODE)) {
			reply = npd_dbus_trunk_load_banlc_config(connection,message,user_data);
		}/*rtunk port enable/disable */ 
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,	\
								NPD_DBUS_TRUNK_METHOD_ALLOW_REFUSE_VLAN_LIST)) {
			reply = npd_dbus_dynamic_trunk_allow_refuse_vlan(connection,message,user_data);
		}/*allow refuse vlan*/
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,	\
								NPD_DBUS_TRUNK_METHOD_DELETE_TRUNK_ENTRY)) {
			reply = npd_dbus_trunk_delete_one(connection,message,user_data);
		}/*delete trunk Id*/
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,	\
								NPD_DBUS_TRUNK_METHOD_DELETE_TRUNK_ENTRY_VIA_NAME)) {
			reply = npd_dbus_trunk_delete_by_name(connection,message,user_data);
		} /*delete trunk name*/
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,	\
								NPD_DBUS_TRUNK_METHOD_SHOW_TRUNK_PORT_MEMBERS)) {
			reply = npd_dbus_trunk_show_one(connection,message,user_data);
		} /*show trunk one*/
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,	\
								NPD_DBUS_TRUNK_METHOD_SHOW_TRUNK_PORT_MEMBERS_V1)) {
			reply = npd_dbus_trunk_show_one_v1(connection,message,user_data);
		} /*show trunk one*/
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,	\
								NPD_DBUS_TRUNK_METHOD_SHOW_TRUNK_BY_NAME)) {
			reply = npd_dbus_trunk_show_by_name(connection,message,user_data);
		} /*show trunk by name*/
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,	\
								NPD_DBUS_TRUNK_METHOD_SHOW_TRUNK_BY_NAME_V1)) {
			reply = npd_dbus_trunk_show_by_name_v1(connection,message,user_data);
		} /*show trunk by name*/
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,	\
								NPD_DBUS_TRUNK_METHOD_SHOW_TRUNKLIST_PORT_MEMBERS)) {
			reply = npd_dbus_trunk_show_trunklist_port_member(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,	\
								NPD_DBUS_TRUNK_METHOD_SHOW_TRUNKLIST_PORT_MEMBERS_V1)) {
			reply = npd_dbus_trunk_show_trunklist_port_member_v1(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,	\
								NPD_DBUS_TRUNK_METHOD_SHOW_TRUNK_VLAN_AGGREGATION)) {
			reply = npd_dbus_trunk_show_vlanlist(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,  \
										NPD_DBUS_TRUNK_METHOD_SHOW_RUNNING_CONFIG)) {
			reply = npd_dbus_trunk_show_running_config(connection,message,user_data);
		} /*show trunk list*/
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,  \
										NPD_DBUS_TRUNK_METHOD_CLEAR_TRUNK_ARP)) {
			reply = npd_dbus_trunk_clear_arp(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,	\
								NPD_DBUS_TRUNK_METHOD_CREATE_DYNAMIC_TRUNK)) {
			reply = npd_dbus_dynamic_trunk_create_trunk(connection,message,user_data);
		}/*create dynamic trunk*/	
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,	\
								NPD_DBUS_TRUNK_METHOD_DELETE_DYNAMIC_TRUNK)) {
			reply = npd_dbus_dynamic_trunk_delete_trunk(connection,message,user_data);
		}/*delete dynamic trunk*/	
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,	\
								NPD_DBUS_TRUNK_METHOD_DYNAMIC_TRUNK_MAP_TABLE_UPDATE)) {
			reply = npd_dbus_dynamic_trunk_update_map_table(connection,message,user_data);
		}/*delete dynamic trunk*/	
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,	\
								NPD_DBUS_TRUNK_METHOD_DYNAMIC_TRUNK_DEL_MAP_TABLE)) {
			reply = npd_dbus_dynamic_trunk_delete_map_table(connection,message,user_data);
		}/*delete dynamic trunk*/	
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,	\
								NPD_DBUS_TRUNK_METHOD_DYNAMIC_TRUNK_PORT_MEMBER_ADD_DEL)) {
			reply = npd_dbus_dynamic_trunk_add_delete_port_member(connection,message,user_data);
		}/*add or delete port for dynamic trunk*/
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,	\
								NPD_DBUS_DYNAMIC_TRUNK_METHOD_CONFIG)) {
			reply = npd_dbus_dynamic_trunk_config(connection,message,user_data);
		}/*config dynamic trunk*/	
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,  \
										NPD_DBUS_DYNAMIC_TRUNK_METHOD_SHOW_RUNNING_CONFIG)) {
			reply = npd_dbus_dynamic_trunk_show_running_config(connection,message,user_data);
		} /*show dynamic trunk */
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,  \
			                            NPD_DBUS_TRUNK_METHOD_SHOW_DYNAMIC_TRUNK_HW_INFO))
			reply = npd_dbus_dynamic_trunk_show_dynamic_trunk_hardware_information(connection,message,user_data);
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,  \
										NPD_DBUS_DYNAMIC_TRUNK_METHOD_SHOW_TRUNK_MEMBER_LIST)) {
			reply = npd_dbus_dynamic_trunk_show_dynamic_trunk_list(connection,message,user_data);
		} /*show dynamic trunk */
		else if(dbus_message_is_method_call(message,NPD_DBUS_TRUNK_INTERFACE,  \
										NPD_DBUS_DYNAMIC_TRUNK_METHOD_SHOW_TRUNK_VLAN_MEMBER_LIST)) {
			reply = npd_dbus_dynamic_trunk_show_dynamic_trunk_vlan_member_list(connection,message,user_data);
		} /*show dynamic trunk */
		
	}
	else if(strcmp(dbus_message_get_path(message),NPD_DBUS_PVE_OBJPATH) == 0) {
		syslog_ax_dbus_dbg("npd obj path"NPD_DBUS_PVE_OBJPATH);

		if(dbus_message_is_method_call(message,NPD_DBUS_PVE_INTERFACE,	\
								NPD_DBUS_PVE_METHOD_CONFIG_PVE_PORT)) {					
			reply = npd_dbus_pvlan_config_port(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_PVE_INTERFACE,	\
								NPD_DBUS_PVE_METHOD_CONFIG_PVE_SPI_PORT)) {					
			reply = npd_dbus_pvlan_config_spi_port(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_PVE_INTERFACE,	\
								NPD_DBUS_PVE_METHOD_DELETE_PVE_PORT)) {
			reply = npd_dbus_pvlan_port_delete(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_PVE_INTERFACE,	\
								NPD_DBUS_PVE_METHOD_CREATE_PVE)) {
			reply = npd_dbus_pvlan_create(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_PVE_INTERFACE,	\
								NPD_DBUS_PVE_METHOD_ADD_PORT)) {
			reply = npd_dbus_pvlan_add_port(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_PVE_INTERFACE,	\
								NPD_DBUS_PVE_METHOD_DELETE_PORT)) {
			reply = npd_dbus_pvlan_delete_port(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_PVE_INTERFACE,	\
								NPD_DBUS_PVE_METHOD_CONFIG_PVE_CPUPORT)) {
			reply = npd_dbus_pvlan_cpu_port(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_PVE_INTERFACE,	\
								NPD_DBUS_PVE_METHOD_CONFIG_PVE_TRUNK)) {
			reply = npd_dbus_pvlan_config_trunk(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_PVE_INTERFACE,	\
								NPD_DBUS_PVE_METHOD_SHOW_PVE)) {
			reply = npd_dbus_pvlan_show_pvlan(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_PVE_INTERFACE,	\
								NPD_DBUS_PVE_METHOD_SHOW_RUNNING_CFG)) {
			reply = npd_dbus_pvlan_show_running_cfg(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_PVE_INTERFACE,	\
								NPD_DBUS_PVE_METHOD_CONFIG_PVE_CONTROL)) {
			reply = npd_dbus_pvlan_config_control(connection,message,user_data);
		}
	}
	else if(strcmp(dbus_message_get_path(message),NPD_DBUS_FDB_OBJPATH) == 0) {
		syslog_ax_dbus_dbg("npd obj path"NPD_DBUS_FDB_OBJPATH"\n");

		if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,	\
								NPD_DBUS_FDB_METHOD_CONFIG_FDB_AGINGTIME)) {
			reply = npd_dbus_fdb_config_agingtime(connection,message,user_data);
		}/*config fdb agingtime*/
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,	\
								NPD_DBUS_FDB_METHOD_CONFIG_FDB_DELETE_WITH_VLAN )) {
			reply = npd_dbus_fdb_delete_fdb_with_vlan(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,	\
								NPD_DBUS_FDB_METHOD_CONFIG_FDB_DELETE_WITH_PORT)) {
			reply = npd_dbus_fdb_delete_fdb_with_port(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,	\
								NPD_DBUS_FDB_METHOD_CONFIG_FDB_STATIC_DELETE_WITH_VLAN )) {
			reply = npd_dbus_fdb_delete_static_fdb_with_vlan(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,	\
								NPD_DBUS_FDB_METHOD_CONFIG_DEBUG_FDB_STATIC_DELETE_WITH_VLAN )) {
			reply = npd_dbus_fdb_delete_static_debug_fdb_with_vlan(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,	\
								NPD_DBUS_FDB_METHOD_CONFIG_FDB_STATIC_DELETE_WITH_PORT)) {
			reply = npd_dbus_fdb_delete_static_fdb_with_port(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,	\
								NPD_DBUS_FDB_METHOD_CONFIG_FDB_DELETE_WITH_TRUNK)) {
			reply = npd_dbus_fdb_delete_fdb_with_trunk(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,	\
								NPD_DBUS_FDB_METHOD_CONFIG_FDB_DEFAULT_AGINGTIME)) {
			reply = npd_dbus_fdb_no_config_agingtime(connection,message,user_data);
		}/*config fdb no agingtime*/
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_CONFIG_FDB_NO_DROP)) {
			reply = npd_dbus_fdb_delete_blacklist(connection,message,user_data);
		}/*config fdb mac_vlan_port*/
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_CONFIG_FDB_NO_DROP_WITH_NAME)) {
			reply = npd_dbus_fdb_delete_blacklist_with_name(connection,message,user_data);
		}/*conf*/
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_CONFIG_FDB_DROP)) {
			reply = npd_dbus_fdb_create_blacklist(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_CONFIG_FDB_DROP_WITH_NAME)) {
			reply = npd_dbus_fdb_create_blacklist_with_vlanname(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_SHOW_FDB_AGINGTIME)) {
			reply = npd_dbus_fdb_show_agingtime(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_CONFIG_SYSTEM_FDB)) {
			reply = npd_dbus_fdb_config_system_mac(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_CONFIG_FDB_STATIC)) {
			reply = npd_dbus_fdb_config_static_fdb_item(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_CONFIG_DEBUG_FDB_STATIC)) {
			reply = npd_dbus_fdb_config_static_debug_fdb_item(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_CONFIG_FDB_STATIC_SAVE)) {
			reply = npd_dbus_fdb_config_static_fdb_save(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_CHECK_FDB_STATIC_EXISTS)) {
			reply = npd_dbus_fdb_check_fdb_static_exists(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_CONFIG_FDB_TRUNK_STATIC)) {
			reply = npd_dbus_fdb_config_static_fdb_trunk_item(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_CONFIG_DEBUG_FDB_TRUNK_STATIC)) {
			reply = npd_dbus_fdb_config_static_debug_fdb_trunk_item(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_CONFIG_FDB_STATIC_WITH_NAME)) {
			reply = npd_dbus_fdb_config_static_fdb_with_name(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_CONFIG_FDB_STATIC_TRUNK_WITH_NAME)) {
			reply = npd_dbus_fdb_config_static_fdb_trunk_with_name(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_CONFIG_FDB_NO_STATIC)) {
			reply =  npd_dbus_fdb_delete_static_fdb(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_CONFIG_FDB_NO_STATIC_WITH_NAME)) {
			reply = npd_dbus_fdb_delete_static_fdb_with_name(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_SHOW_FDB_TABLE)) {
			reply = npd_dbus_fdb_show_fdb(connection,message,user_data);
		}
		/* for show debug fdb test */
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_SHOW_FDB_TABLE_DEBUG)) {
			reply = npd_dbus_fdb_show_fdb_debug(connection,message,user_data);
		}	
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_SYN_FDB_TABLE)) {
			reply = npd_dbus_fdb_syn_fdb_table(connection,message,user_data);
		}	
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_SHOW_FDB_DYNAMIC_TABLE)) {
			reply = npd_dbus_fdb_show_dynamic_fdb(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_SHOW_FDB_STATIC_TABLE)) {
			reply = npd_dbus_fdb_show_static_fdb(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_SHOW_FDB_BLACKLIST_TABLE)){
			reply = npd_dbus_fdb_show_blacklist_fdb(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_SHOW_FDB_TABLE_PORT)) {
			reply = npd_dbus_fdb_show_fdb_port(connection,message,user_data);
			}	
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_SHOW_FDB_TABLE_VLAN)) {
			reply = npd_dbus_fdb_show_fdb_vlan(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_SHOW_FDB_TABLE_VLAN_WITH_NAME)) {
			reply = npd_dbus_fdb_show_fdb_vlan_with_name(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_SHOW_FDB_TABLE_ONE)) {
			reply = npd_dbus_fdb_show_fdb_one(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_SHOW_FDB_TABLE_MAC)) {
			reply = npd_dbus_fdb_show_fdb_mac(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_SHOW_FDB_TABLE_COUNT)) {
			reply = npd_dbus_fdb_show_fdb_count(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,	\
								NPD_DBUS_STATIC_FDB_METHOD_SHOW_RUNNING_CONFIG)) {
			reply = npd_dbus_fdb_show_running_config(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message, NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_CONFIG_FDB_NUMBER_WITH_PORT)){
			reply = npd_dbus_fdb_config_port_number(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message, NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_CONFIG_FDB_NUMBER_WITH_VLAN)){
			reply = npd_dbus_fdb_config_vlan_number(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message, NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_SHOW_FDB_NUMBER_LIMIT_ITEM)){
			reply = npd_dbus_fdb_show_limit_item(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_FDB_INTERFACE,NPD_DBUS_FDB_METHOD_CONFIG_FDB_NUMBER_WITH_VLAN_PORT)){
			reply = npd_dbus_fdb_config_vlan_port_number(connection,message,user_data);
		}
	
	}
	else if(strcmp(dbus_message_get_path(message),NPD_DBUS_MIRROR_OBJPATH) == 0) {	/*mirror*/
		if (dbus_message_is_method_call(message,NPD_DBUS_MIRROR_INTERFACE,NPD_DBUS_METHOD_CONFIG_MIRROR)) {
			reply = npd_dbus_mirror_config(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_MIRROR_INTERFACE,NPD_DBUS_METHOD_MIRROR_DEST_PORT_CREATE)) {
			reply = npd_dbus_mirror_create_dest_port(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_MIRROR_INTERFACE,NPD_DBUS_METHOD_DEBUG_MIRROR_DEST_PORT_CREATE)) {
			reply = npd_dbus_debug_mirror_create_dest_port(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_MIRROR_INTERFACE,NPD_DBUS_METHOD_MIRROR_DEST_PORT_DEL)) {
			reply = npd_dbus_mirror_del_dest_port(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_MIRROR_INTERFACE,NPD_DBUS_METHOD_APPEND_MIRROR_BASE_ACL)) {
			reply = npd_dbus_mirror_create_source_acl(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_MIRROR_INTERFACE,NPD_DBUS_METHOD_CANCEL_MIRROR_BASE_ACL)) {
			reply = npd_dbus_mirror_delete_source_acl(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_MIRROR_INTERFACE,NPD_DBUS_METHOD_APPEND_MIRROR_BASE_PORT_CREATE)) {
			reply = npd_dbus_mirror_create_source_port(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_MIRROR_INTERFACE,NPD_DBUS_METHOD_DEBUG_APPEND_MIRROR_BASE_PORT_CREATE)) {
			reply = npd_dbus_debug_mirror_create_source_port(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_MIRROR_INTERFACE,NPD_DBUS_METHOD_APPEND_MIRROR_BASE_PORT_DEL)) {
			reply = npd_dbus_mirror_del_source_port(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_MIRROR_INTERFACE,NPD_DBUS_METHOD_DEBUG_APPEND_MIRROR_BASE_PORT_DEL)) {
			reply = npd_dbus_debug_mirror_del_source_port(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_MIRROR_INTERFACE,NPD_DBUS_METHOD_APPEND_MIRROR_BASE_VLAN_CREATE)) {
			reply = npd_dbus_mirror_create_source_vlan(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_MIRROR_INTERFACE,NPD_DBUS_METHOD_APPEND_MIRROR_BASE_VLAN_DEL)) {
			reply = npd_dbus_mirror_del_source_vlan(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_MIRROR_INTERFACE,NPD_DBUS_METHOD_APPEND_MIRROR_BASE_FDB)) {
			reply = npd_dbus_mirror_fdb_mac_vlanid_port_set(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_MIRROR_INTERFACE,NPD_DBUS_METHOD_CANCEL_MIRROR_BASE_FDB)) {
			reply = npd_dbus_mirror_fdb_mac_vlanid_port_cancel(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_MIRROR_INTERFACE,NPD_DBUS_METHOD_MIRROR_SHOW)) {
			reply = npd_dbus_mirror_show(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_MIRROR_INTERFACE,NPD_DBUS_METHOD_MIRROR_DELETE)) {
			reply = npd_dbus_mirror_delete(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_MIRROR_INTERFACE,NPD_DBUS_METHOD_SHOW_RUNNING_CGF)) {
			reply = npd_dbus_mirror_show_running_cfg(connection,message,user_data);
		}
	}
	else if(strcmp(dbus_message_get_path(message),NPD_DBUS_QOS_OBJPATH) == 0){
		/*QOS*/
		syslog_ax_dbus_dbg("npd obj path"NPD_DBUS_QOS_OBJPATH);
		if (dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_CONFIG_QOS_PROFILE)) {
			reply = npd_dbus_config_qos_profile(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_CONFIG_QOS_MODE)) {
			reply =  npd_dbus_config_qos_mode(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_SHOW_QOS_MODE)) {
			reply =  npd_dbus_show_qos_mode(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_QOS_PROFILE_ATTRIBUTE)) {
			reply =  npd_dbus_qos_profile_attributes(connection,message,user_data);
		}
	   else if (dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_SET_DSCP_PROFILE_TABLE)) {
			reply =  npd_dbus_dscp_to_qos_profile_table(connection,message,user_data);
		}
	   else if (dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_SET_DSCP_DSCP_TABLE)) {
			reply =  npd_dbus_dscp_to_dscp_table(connection,message,user_data);
		}
        else if (dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_SET_UP_PROFILE_TABLE)) {
			reply =  npd_dbus_up_to_qos_profile(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_DELETE_DSCP_PROFILE_TABLE)) {
			reply =  npd_dbus_delete_dscp_to_qos_profile_table(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_DELETE_ACL_PROFILE_TABLE)) {
			reply =  npd_dbus_delete_acl_to_qos_profile_table(connection,message,user_data);
		}
	   else if (dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_DELETE_DSCP_DSCP_TABLE)) {
			reply =  npd_dbus_delete_dscp_to_dscp_table(connection,message,user_data);
		}
        else if (dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_DELETE_UP_PROFILE_TABLE)) {
			reply =  npd_dbus_delete_up_to_qos_profile(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_CREATE_POLICY_MAP)) {
			reply =  npd_dbus_create_policy_map(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_CONFIG_POLICY_MAP)) {
			reply =  npd_dbus_config_policy_map(connection,message,user_data);
		}
		#if 0
		else if (dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_SET_DEFAULT_UP)) {
			reply =  npd_dbus_policy_default_up(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_SET_DEFAULT_QOS_PROFILE)) {
			reply =  npd_dbus_policy_default_qos_profile(connection,message,user_data);
		}
		#endif
		else if (dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_SET_PORT_TRUST_L2_MODE)) {
			reply =  npd_dbus_policy_trust_mode_l2_set(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_SET_PORT_TRUST_L3_MODE)) {
			reply =  npd_dbus_policy_trust_mode_l3_set(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_SET_PORT_TRUST_L2_L3_MODE)) {
			reply =  npd_dbus_policy_trust_mode_l2_l3_set(connection,message,user_data);
		}
		#if 0
		else if (dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_SET_PORT_TRUST_UNTRUST_MODE)) {
			reply =  npd_dbus_policy_trust_mode_untrust_set(connection,message,user_data);
		}
		#endif
		else if (dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_MODIFY_MARK_QOS)) {
			reply =  npd_dbus_policy_modify_qos(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_SET_QOS_INGRESS_POLICY_BASE_ON_ACL))
		{
			reply = npd_dbus_config_qos_base_acl_ingress(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_SET_QOS_EGRESS_POLICY_BASE_ON_ACL))
		{
			reply = npd_dbus_config_qos_base_acl_egress(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_SHOW_POLICY_MAP))
		{
			reply = npd_dbus_show_policy_map(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_SHOW_QOS_PROFILE))
		{
			reply = npd_dbus_show_qos_profile(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_CONFIG_POLICER))
		{
			reply = npd_dbus_policer_set(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_CONFIG_POLICER_RANGE))
		{
			reply = npd_dbus_policer_set_range(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_CONFIG_CIR_CBS))
		{
			reply = npd_dbus_policer_cir_cbs(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_CONFIG_OUT_PROFILE))
		{
			reply = npd_dbus_policer_set_out_profile(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_OUT_PROFILE_DROP_KEEP))
		{
			reply = npd_dbus_policer_out_profile_cmd_keep_drop(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_OUT_PROFILE_REMAP))
		{
			reply = npd_dbus_policer_out_profile_cmd_remap(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_POLICER_ENABLE))
		{
			reply = npd_dbus_policer_enable(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_GLOBAL_METER_MODE))
		{
			reply = npd_dbus_global_policer_meter_mode(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_GLOBAL_PACKET_SIZE))
		{
			reply = npd_dbus_global_policing_mode(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_SET_COUNTER))
		{
			reply = npd_dbus_set_counter(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_POLICER_COUNTER))
		{
			reply = npd_dbus_policer_counter(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_POLICER_SHARE))
		{
			reply = npd_dbus_policer_share_mode(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_GET_COUNTER))
		{
			reply = npd_dbus_read_counter(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_SHOW_POLICER))
		{
			reply = npd_dbus_show_policer(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_POLICER_COLOR))
		{
			reply = npd_dbus_policer_color(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_ETHPORT_METHOD_SHOW_REMAP_TABLE))
		{
			reply = npd_dbus_show_remap_table(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_DELETE_QOS_PROFILE))
		{
			reply = npd_dbus_delete_qos_profile(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_DELETE_POLICY_MAP))
		{
			reply = npd_dbus_delete_policy_map(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_DELETE_POLICER))
		{
			reply = npd_dbus_delete_policer(connection,message,user_data);
		}		
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_DELETE_POLICER_RANGE))
		{
			reply = npd_dbus_delete_policer_range(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_SHOW_REMAP_TABLE_RUNNIG_CONFIG))
		{
			reply = npd_dbus_qos_remap_table_show_running_config(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_SHOW_POLICY_MAP_RUNNIG_CONFIG))
		{
			reply = npd_dbus_qos_policy_map_show_running_config(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_SHOW_POLICER_RUNNIG_CONFIG))
		{
			reply = npd_dbus_qos_policer_show_running_config(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_SHOW_QUEUE_SCH_RUNNIG_CONFIG))
		{
			reply = npd_dbus_qos_queue_sch_show_running_config(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_SHOW_QOS_MODE_RUNNIG_CONFIG))
		{
			reply = npd_dbus_qos_mode_show_running_config(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_SHOW_QOS_PROFILE_RUNNIG_CONFIG))
		{
			reply = npd_dbus_qos_profile_show_running_config(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_SHOW_QOS_COUNTER_RUNNIG_CONFIG))
		{
			reply = npd_dbus_qos_counter_show_running_config(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_QUEQUE_SCH))
		{
			reply = npd_dbus_queue_scheduler(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_QUEQUE_WRR_GROUP))
		{
			reply = npd_dbus_queue_sche_wrr_group(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_SHOW_QUEUE))
		{
			reply = npd_dbus_show_queue_scheduler(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_METHOD_APPEND_QOS_MARK_BASE_ACL))
		{
			reply = npd_dbus_qos_acl_apend(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_QOS_INTERFACE,NPD_DBUS_SHOW_APPEND_QOS_MARK_BASE_ACL))
		{
			reply = npd_dbus_show_qos_acl_apend(connection,message,user_data);
		}
	}
	else if (strcmp(dbus_message_get_path(message),NPD_DBUS_ACL_OBJPATH) == 0) {
		syslog_ax_dbus_dbg("npd obj path"NPD_DBUS_ACL_OBJPATH);
		if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_CONFIG_ACL)) {
			reply = npd_dbus_config_acl(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_SHOW_ACL_SERVICE)) {
			reply =  npd_dbus_show_acl_service(connection,message,user_data);
		}
	   else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_SHOW_ACL)) {
			reply =  npd_dbus_show_accesslist(connection,message,user_data);
		}
	   else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_SHOW_ACL_INDEX)) {
			reply =  npd_dbus_show_accesslist_index(connection,message,user_data);
		}
	   else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_DELETE_ACL)) {
			reply =  npd_dbus_delete_acl(connection,message,user_data);
		}
	    else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_DELETE_ACL_RANGE)) {
			reply =  npd_dbus_delete_acl_range(connection,message,user_data);
		}
	   else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_SHOW_ACL_GROUP_RUNNIG_CONFIG)) {
			reply =  npd_dbus_acl_group_show_running_config(connection,message,user_data);
		}
	   else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_SHOW_ACL_QOS_RUNNIG_CONFIG)) {
			reply =  npd_dbus_acl_qos_show_running_config(connection,message,user_data);
		}
	   else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_SHOW_ACL_RULE_RUNNIG_CONFIG)) {
			reply =  npd_dbus_acl_rule_show_running_config(connection,message,user_data);
		}
		/*trap*/
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_CONFIG_ACL_RULE_TRAP_IP)) {
			reply =  npd_dbus_config_trap_to_cpu_ip(connection,message,user_data);
		}
		
	   else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_CONFIG_ACL_RULE_TRAP_TCP)) {
			reply =  npd_dbus_config_trap_to_cpu_tcp(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_CONFIG_ACL_RULE_TRAP_UDP)) {
			reply =  npd_dbus_config_trap_to_cpu_udp(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_CONFIG_ACL_RULE_TRAP_ICMP)) {
			reply =  npd_dbus_config_trap_to_cpu_icmp(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_CONFIG_ACL_RULE_TRAP_MAC)) {
			reply = npd_dbus_config_trap_to_cpu_ethernet(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_CONFIG_ACL_RULE_TRAP_ARP)) {
			reply = npd_dbus_config_trap_to_cpu_arp(connection,message,user_data);
		}
		/*permit deny*/
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_CONFIG_ACL_RULE_DENY_TCP_OR_UDP)) {
			reply = npd_dbus_config_permit_deny_tcp_or_udp(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_CONFIG_ACL_RULE_DENY_MAC)) {
			reply = npd_dbus_config_permit_deny_mac(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_CONFIG_ACL_RULE_DENY_IP)) {
			reply = npd_dbus_config_permit_deny_ip(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_CONFIG_ACL_RULE_DENY_IPV6)) {
			reply = npd_dbus_config_permit_deny_ipv6(connection,message,user_data);
		}		
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_CONFIG_ACL_RULE_IP_RANGE)) {
			reply = npd_dbus_config_ip_range(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_CONFIG_ACL_PERMIT_RULE_IP_RANGE)) {
			reply = npd_dbus_config_permit_deny_ip_range(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_CONFIG_ACL_RULE_DENY_ICMP)) {
			reply = npd_dbus_config_permit_deny_icmp(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_CONFIG_ACL_RULE_PERMIT_DENY_ARP)) {
			reply = npd_dbus_config_permit_deny_arp(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_CONFIG_ACL_RULE_EXTENDED_PERMIT_DENY_TRAP_TCP_UDP)) {
			reply = npd_dbus_config_ext_permit_deny_trap(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_CONFIG_ACL_RULE_EXTENDED_PERMIT_DENY_TCP_UDP_IPV6)) {
			reply = npd_dbus_config_ext_permit_deny_ipv6(connection,message,user_data);
		}
		
		/*mirror redirect*/
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_CONFIG_ACL_RULE_MIRROR_OR_REDIRECT_MAC)) {
			reply =  npd_dbus_config_mirror_redirect_mac(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_CONFIG_ACL_RULE_MIRROR_OR_REDIRECT_TCP_UDP)) {
			reply = npd_dbus_config_mirror_redirect_tcp_udp(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_CONFIG_ACL_RULE_MIRROR_OR_REDIRECT_ICMP)) {
			reply = npd_dbus_config_mirror_redirect_icmp(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_CONFIG_ACL_RULE_MIRROR_OR_REDIRECT_IP)) {
			reply = npd_dbus_config_mirror_redirect_ip(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_CONFIG_ACL_RULE_MIRROR_REDIRECT_ARP)) {
			reply = npd_dbus_config_mirror_redirect_arp(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_CONFIG_ACL_RULE_EXTENDED_MIRROR_REDIRECT_TCP_UDP)) {
			reply = npd_dbus_config_ext_mirror_redirect(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_CONFIG_ACL_REDIRECT_IPV6)) {
			reply = npd_dbus_config_redirect_ipv6(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_CONFIG_ACL_REDIRECT_TCP_UDP_IPV6)) {
			reply = npd_dbus_config_redirect_tcp_udp_ipv6(connection,message,user_data);
		}
		
		/*acl group*/
		else if(dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_CREATE_ACL_GROUP))
		{
			reply = npd_dbus_creat_acl_group(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_DELETE_ACL_GROUP))
		{
			reply = npd_dbus_delete_acl_group(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_ADD_ACL_TO_GROUP)) 
		{
			reply =  npd_dbus_add_delete_rule_to_acl_group(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_SHOW_ACL_GROUP)) 
		{
			reply =  npd_dbus_show_acl_group(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_SHOW_ACL_GROUP_INDEX)) 
		{
			reply =  npd_dbus_show_acl_group_index(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_METHOD_CONFIG_ACL_GROUP)) 
		{
			reply =  npd_dbus_config_acl_group(connection,message,user_data);
		}	
		/*time*/
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_ACL_METHOD_SET_TIME_RANGE)) 
		{
			reply =  npd_dbus_time_range_set(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_ACL_METHOD_SET_ABSOLUTE)) 
		{
			reply =  npd_dbus_time_absolute_set(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_ACL_METHOD_SET_PERIODIC)) 
		{
			reply =  npd_dbus_time_periodic_set(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_ACL_METHOD_SHOW_TIME_RANGE)) 
		{
			reply =  npd_dbus_time_range_show(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ACL_INTERFACE,NPD_DBUS_ACL_METHOD_ACL_TIME_RANGE)) 
		{
			reply =  npd_dbus_time_range_acl(connection,message,user_data);
		}
	}
	else if(strcmp(dbus_message_get_path(message),NPD_DBUS_INTF_OBJPATH) == 0) {
		/*config interface*/
		syslog_ax_dbus_dbg("npd obj path"NPD_DBUS_INTF_OBJPATH"\n");
		if (dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_INTF_METHOD_CREATE_VID_INTF)) 
		{
			reply = npd_dbus_create_intf_by_vid(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_INTF_METHOD_CREATE_VID_INTF_BY_VLAN_IFNAME))
		{
			reply = npd_dbus_create_intf_by_vlan_ifname(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_INTF_METHOD_VLAN_INTERFACE_ADVANCED_ROUTING_ENABLE))
		{
			reply = npd_dbus_vlan_interface_advanced_routing_enable(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_INTF_METHOD_DEL_VID_INTF))
		{
			reply = npd_dbus_del_intf_by_vid(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_SUB_INTERFACE_CREATE))
		{
			reply = npd_dbus_create_sub_intf(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_SUB_INTERFACE_DELETE))
		{
			reply = npd_dbus_del_sub_intf(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_INTERFACE_ADVANCED_ROUTING))
		{
			reply = npd_dbus_advanced_route(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_INTERFACE_IP_STATIC_ARP))
		{
            reply = npd_dbus_ip_static_arp(connection,message,user_data);
		}		
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_INTERFACE_IP_STATIC_ARP_FOR_TRUNK))
		{
            reply = npd_dbus_ip_static_arp_for_trunk(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_INTERFACE_NO_IP_STATIC_ARP))
		{
            reply = npd_dbus_no_ip_static_arp(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_INTERFACE_NO_IP_STATIC_ARP_FOR_TRUNK))
		{
            reply = npd_dbus_no_ip_static_arp_for_trunk(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_DYNAMIC_ARP))/*wangchao add*/
		{
            reply = npd_dbus_dynamic_arp(connection,message,user_data);
		}  
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_INTERFACE_STATIC_ARP))
		{
            reply = npd_dbus_interface_static_arp(connection,message,user_data);
		}     
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_INTERFACE_SET_STALE_TIME))
		{
            reply = npd_dbus_set_stale_time_for_interface(connection,message,user_data);
		} 
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_INTERFACE_STATIC_ARP_TRUNK))
		{
            reply = npd_dbus_interface_static_arp_trunk(connection,message,user_data);
		}     
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_INTF_METHOD_VLAN_ETH_PORT_INTERFACE_ADVANCED_ROUTING_SHOW))
		{
            reply = npd_dbus_vlan_eth_port_interface_advanced_routing_show(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_STATIC_ARP_METHOD_SHOW_RUNNING_CONFIG))
		{
            reply = npd_dbus_static_arp_show_running(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_INTERRUPT_RXMAX_SHOW_RUNNING_CONFIG))
		{
            reply = npd_dbus_interrupt_rxmax_show_running(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_INTF_ADVANCED_ROUTING_SAVE_CFG))
		{
            reply = npd_dbus_interface_advanced_routing_show_running(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_INTF_ETH_INTERFACE_ENABLE_SET_SAVE_CFG))
		{
            reply = npd_dbus_eth_interface_enable_set_show_running(connection,message,user_data);
		}
        else if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_INTF_INTERFACE_STATIC_ARP_SHOW_RUNNING))
		{
            reply = npd_dbus_interface_static_arp_show_running(connection,message,user_data);
		}
        
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_CONFIG_ADVANCED_ROUTING_DEFAULT_VID))
		{
			reply = npd_dbus_interface_config_advanced_routing_default_vid(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_SHOW_ADVANCED_ROUTING_DEFAULT_VID))
		{
			reply = npd_dbus_interface_show_advanced_routing_default_vid(connection,message,user_data);
		}else if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_INTF_SUBIF_SET_QINQ_TYPE))
		{
			reply = npd_dbus_subif_set_qinq_type(connection,message,user_data);
		}else if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_INTF_SET_QINQ_TYPE_SAVE_CFG))
		{
			reply = npd_dbus_subif_qinq_type_save_cfg(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_INTF_CHANGE_INTF_NAME))
		{
			reply = npd_dbus_intf_change_intf_name(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_INTF_ETH_INTERFACE_ENABLE_SET))
		{
			reply = npd_dbus_intf_eth_interface_enable_set(connection,message,user_data);
		}		
#if 0
		if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_INTF_METHOD_CREATE_PORT_INTF))
		{

			reply = npd_dbus_create_intf_by_port_index(connection,message,user_data);
		}
		if(dbus_message_is_method_call(message,NPD_DBUS_INTF_INTERFACE,NPD_DBUS_INTF_METHOD_DEL_PORT_INTF))
		{
			reply = npd_dbus_del_intf_by_port_index(connection,message,user_data);
		}
#endif
	}
	/*added by scx*/
	else if(strcmp(dbus_message_get_path(message),RTDRV_DBUS_OBJPATH)== 0) {
		syslog_ax_dbus_dbg("npd obj path"RTDRV_DBUS_OBJPATH);
		syslog_ax_dbus_dbg("get dbus msg for drv rt\n");
		if (dbus_message_is_method_call(message,RTDRV_DBUS_INTERFACE,RTDRV_DBUS_METHOD_SHOW_ALL)) {
			syslog_ax_dbus_dbg("get dbus msg for drv rt RTDRV_DBUS_METHOD_SHOW_ALL\n");
			reply =  show_rtdrv_all(connection,message,user_data);
		}
		if (dbus_message_is_method_call(message,RTDRV_DBUS_INTERFACE,RTDRV_DBUS_METHOD_SHOW_HOST)) {
			syslog_ax_dbus_dbg("get dbus msg for drv rt RTDRV_DBUS_METHOD_SHOW_HOST\n");
			reply =  show_hostdrv_all(connection,message,user_data);
		}
		if (dbus_message_is_method_call(message,RTDRV_DBUS_INTERFACE,RTDRV_DBUS_METHOD_SHOW_ENTRY)) {
			syslog_ax_dbus_dbg("get dbus msg for drv rt RTDRV_DBUS_METHOD_SHOW_ENTRY\n");
			reply =  show_rtdrv_entry(connection,message,user_data);
		}
		if (dbus_message_is_method_call(message,RTDRV_DBUS_INTERFACE,RTDRV_DBUS_METHOD_CONFIG_RPF)) {
			syslog_ax_dbus_dbg("get dbus msg for drv rt RTDRV_DBUS_METHOD_CONFIG_RPF\n");
			reply =  config_ucrpf_enable(connection,message,user_data);
		}
		if (dbus_message_is_method_call(message,RTDRV_DBUS_INTERFACE,RTDRV_DBUS_METHOD_SHOW_RPF)) {
			syslog_ax_dbus_dbg("get dbus msg for drv rt RTDRV_DBUS_METHOD_SHOW_RPF\n");
			reply =  show_ucrpf_enable(connection,message,user_data);
		}
		if (dbus_message_is_method_call(message,RTDRV_DBUS_INTERFACE,RTDRV_DBUS_METHOD_SHOW_STATUES)) {
			syslog_ax_dbus_dbg("get dbus msg for drv rt RTDRV_DBUS_METHOD_SHOW_STATUES\n");
			reply =  show_route_status(connection,message,user_data);
		}
		if(dbus_message_is_method_call(message,RTDRV_DBUS_INTERFACE,RTDRV_DBUS_METHOD_ARP_AGING_TIME)) {
			syslog_ax_dbus_dbg("get dbus msg for drv rt RTDRV_DBUS_METHOD_ARP_AGING_TIME\n");
			reply =  npd_dbus_set_arp_aging_time(connection,message,user_data);
		}
	}

	else if(strcmp(dbus_message_get_path(message), NPD_DBUS_DLDP_OBJPATH) == 0) {	/* DLDP */
		syslog_ax_dbus_dbg("npd obj path"NPD_DBUS_DLDP_OBJPATH"\n");

		if (dbus_message_is_method_call(message,
										NPD_DBUS_DLDP_INTERFACE,
										NPD_DBUS_DLDP_METHOD_CHECK_GLOBAL_STATUS))
		{
			reply = npd_dbus_dldp_check_global_status(connection, message, user_data);
		}
		#if 0
		// base port
		else if (dbus_message_is_method_call(message,
												NPD_DBUS_DLDP_INTERFACE,
												NPD_DBUS_DLDP_METHOD_CONFIG_ETHPORT))
		{
			reply = npd_dbus_dldp_config_ethport(connection, message, user_data);
		} 
		#endif
		/* base vlan*/
		else if (dbus_message_is_method_call(message,
											NPD_DBUS_DLDP_INTERFACE,
											NPD_DBUS_DLDP_METHOD_CONFIG_VLAN))
		{
			reply = npd_dbus_dldp_config_vlan(connection, message, user_data);
		} 
		else if (dbus_message_is_method_call(message,
											NPD_DBUS_DLDP_INTERFACE,
											NPD_DBUS_DLDP_METHOD_GET_VLAN_COUNT))
		{
			reply = npd_dbus_dldp_get_vlan_count(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,
											NPD_DBUS_DLDP_INTERFACE,
											NPD_DBUS_DLDP_METHOD_GET_PRODUCT_ID))
		{
			reply = npd_dbus_dldp_get_product_id(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,
											NPD_DBUS_DLDP_INTERFACE,
											NPD_DBUS_DLDP_METHOD_EXCHANGE_IFINDEX_TO_SLOTPORT))
		{
			reply = npd_dbus_dldp_exchange_ifindex_to_slotport(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,
											NPD_DBUS_DLDP_INTERFACE,
											NPD_DBUS_DLDP_METHOD_VLAN_SHOW_RUNNING_CONFIG))
		{
			reply = npd_dbus_dldp_vlan_show_running_config(connection, message, user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_DLDP_INTERFACE,DLDP_DBUS_METHOD_GET_SUPPORT_FUNCTION))
		{
			reply = npd_dbus_dldp_get_product_function(connection,message,user_data);
		}
	}
	else if(strcmp(dbus_message_get_path(message), NPD_DBUS_DHCP_SNP_OBJPATH) == 0) {	/* DHCP_Snooping */
		syslog_ax_dbus_dbg("npd obj path"NPD_DBUS_DHCP_SNP_OBJPATH"\n");

		if (dbus_message_is_method_call(message,
										NPD_DBUS_DHCP_SNP_INTERFACE,
										NPD_DBUS_DHCP_SNP_METHOD_CHECK_GLOBAL_STATUS))
		{
			reply = npd_dbus_dhcp_snp_check_global_status(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,
										NPD_DBUS_DHCP_SNP_INTERFACE,
										NPD_DBUS_DHCP_SNP_METHOD_GLOBAL_ENABLE))
		{
			reply = npd_dbus_dhcp_snp_enable_global_status(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,
										NPD_DBUS_DHCP_SNP_INTERFACE,
										NPD_DBUS_DHCP_SNP_METHOD_VLAN_ENABLE))
		{
			reply = npd_dbus_dhcp_snp_enable_vlan_status(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,
										NPD_DBUS_DHCP_SNP_INTERFACE,
										NPD_DBUS_DHCP_SNP_METHOD_SET_ETHPORT_TRUST_MODE))
		{
			reply = npd_dbus_dhcp_snp_config_port(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,
										NPD_DBUS_DHCP_SNP_INTERFACE,
										NPD_DBUS_DHCP_SNP_METHOD_SHOW_BIND_TABLE))
		{
			reply = npd_dbus_dhcp_snp_show_bind_table(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,
										NPD_DBUS_DHCP_SNP_INTERFACE,
										NPD_DBUS_DHCP_SNP_METHOD_SHOW_STATIC_BIND_TABLE))
		{
			reply = npd_dbus_dhcp_snp_show_static_bind_table(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,
										NPD_DBUS_DHCP_SNP_INTERFACE,
										NPD_DBUS_DHCP_SNP_METHOD_SHOW_STATIC_BIND_TABLE_BY_VLAN))
		{
			reply = npd_dbus_dhcp_snp_show_static_bind_table_by_vlan(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,
										NPD_DBUS_DHCP_SNP_INTERFACE,
										NPD_DBUS_DHCP_SNP_METHOD_SHOW_STATIC_BIND_TABLE_BY_ETHPORT))
		{
			reply = npd_dbus_dhcp_snp_show_static_bind_table_by_ethport(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,
										NPD_DBUS_DHCP_SNP_INTERFACE,
										NPD_DBUS_DHCP_SNP_METHOD_SHOW_TRUST_PORTS))
		{
			reply = npd_dbus_dhcp_snp_show_trust_ports(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,
										NPD_DBUS_DHCP_SNP_INTERFACE,
										NPD_DBUS_DHCP_SNP_METHOD_OPT82_ENABLE))
		{
			reply = npd_dbus_dhcp_snp_enable_opt82(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,
										NPD_DBUS_DHCP_SNP_INTERFACE,
										NPD_DBUS_DHCP_SNP_METHOD_SET_OPT82_FORMAT_TYPE))
		{
			reply = npd_dbus_dhcp_snp_set_opt82_format_type(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,
										NPD_DBUS_DHCP_SNP_INTERFACE,
										NPD_DBUS_DHCP_SNP_METHOD_SET_OPT82_FILL_FORMAT_TYPE))
		{
			reply = npd_dbus_dhcp_snp_set_opt82_fill_format_type(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,
										NPD_DBUS_DHCP_SNP_INTERFACE,
										NPD_DBUS_DHCP_SNP_METHOD_SET_OPT82_REMOTEID_CONTENT))
		{
			reply = npd_dbus_dhcp_snp_set_opt82_remoteid_content(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,
										NPD_DBUS_DHCP_SNP_INTERFACE,
										NPD_DBUS_DHCP_SNP_METHOD_SET_OPT82_PORT_STRATEGY))
		{
			reply = npd_dbus_dhcp_snp_set_opt82_port_strategy(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,
										NPD_DBUS_DHCP_SNP_INTERFACE,
										NPD_DBUS_DHCP_SNP_METHOD_SET_OPT82_PORT_CIRCUITID_CONTENT))
		{
			reply = npd_dbus_dhcp_snp_set_opt82_port_circuitid_content(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,
										NPD_DBUS_DHCP_SNP_INTERFACE,
										NPD_DBUS_DHCP_SNP_METHOD_SET_OPT82_PORT_REMOTEID_CONTENT))
		{
			reply = npd_dbus_dhcp_snp_set_opt82_port_remoteid_content(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,
										NPD_DBUS_DHCP_SNP_INTERFACE,
										NPD_DBUS_DHCP_SNP_METHOD_ADD_DEL_STATIC_BINDING))
		{
			reply = npd_dbus_dhcp_snp_add_del_static_binding(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,
										NPD_DBUS_DHCP_SNP_INTERFACE,
										NPD_DBUS_DHCP_SNP_METHOD_ADD_BINDING))
		{
			reply = npd_dbus_dhcp_snp_add_binding(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,
										NPD_DBUS_DHCP_SNP_INTERFACE,
										NPD_DBUS_DHCP_SNP_METHOD_SHOW_RUNNING_GLOBAL_CONFIG))
		{
			reply = npd_dbus_dhcp_snp_show_running_global_config(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,
										NPD_DBUS_DHCP_SNP_INTERFACE,
										NPD_DBUS_DHCP_SNP_METHOD_SHOW_RUNNING_VLAN_CONFIG))
		{
			reply = npd_dbus_dhcp_snp_show_running_vlan_config(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message,
										NPD_DBUS_DHCP_SNP_INTERFACE,
										NPD_DBUS_DHCP_SNP_METHOD_SHOW_RUNNING_SAVE_BIND_TABLE))
		{
			reply = npd_dbus_dhcp_snp_show_running_save_bind_table(connection, message, user_data);
		}
	}
	
	else if(strcmp(dbus_message_get_path(message), NPD_DBUS_VRRP_OBJPATH) == 0) {	/* DLDP */
		syslog_ax_dbus_dbg("npd obj path"NPD_DBUS_VRRP_OBJPATH"\n");
		if (dbus_message_is_method_call(message,
										NPD_DBUS_VRRP_INTERFACE,
										NPD_DBUS_VRRP_METHOD_GET_SYSMAC))
		{
			reply = npd_dbus_vrrp_get_sysmac(connection, message, user_data);
		}

    }
    else if (strcmp(dbus_message_get_path(message),NPD_DBUS_PROT_VLAN_OBJPATH) == 0) {
		syslog_ax_dbus_dbg("npd obj path"NPD_DBUS_PROT_VLAN_OBJPATH);
		if (dbus_message_is_method_call(message,NPD_DBUS_PROT_VLAN_INTERFACE,NPD_DBUS_PROT_VLAN_METHOD_PORT_ENABLE_CONFIG)) {
			reply = npd_dbus_prot_vlan_port_enable_config(connection,message,user_data);
		}
        else if(dbus_message_is_method_call(message,NPD_DBUS_PROT_VLAN_INTERFACE,NPD_DBUS_PROT_VLAN_METHOD_CONFIG_VID_BY_PORT_ENTRY)){
            reply = npd_dbus_prot_vlan_config_vid_by_port_entry(connection,message,user_data);
        }
        else if(dbus_message_is_method_call(message,NPD_DBUS_PROT_VLAN_INTERFACE,NPD_DBUS_PROT_VLAN_METHOD_NO_VID_BY_PORT_ENTRY)){
            reply = npd_dbus_prot_vlan_no_vid_by_port_entry(connection,message,user_data);
        }
        else if(dbus_message_is_method_call(message,NPD_DBUS_PROT_VLAN_INTERFACE,NPD_DBUS_PROT_VLAN_METHOD_SHOW_PORT_PROT_VLAN)){
            reply = npd_dbus_prot_vlan_show_port_cfg(connection,message,user_data);
        }
        else if(dbus_message_is_method_call(message,NPD_DBUS_PROT_VLAN_INTERFACE,NPD_DBUS_PROT_VLAN_METHOD_SHOW_PORT_PROT_VLAN_LIST)){
            reply = npd_dbus_prot_vlan_show_port_cfg_list(connection,message,user_data);
        }
        else if(dbus_message_is_method_call(message,NPD_DBUS_PROT_VLAN_INTERFACE,NPD_DBUS_PROT_VLAN_METHOD_CONFIG_UDF_ETHTYPE_VALUE)){
            reply = npd_dbus_prot_vlan_config_udf_ethtype(connection,message,user_data);
        }
    }
	if (reply) {
		dbus_connection_send (connection, reply, NULL);
		dbus_connection_flush(connection); /* TODO    Maybe we should let main loop process the flush*/
		dbus_message_unref (reply);
	}

/*	dbus_message_unref(message); //TODO who should unref the incoming message? */
	return DBUS_HANDLER_RESULT_HANDLED ;
}

static DBusHandlerResult npd_dbus_ethports_message_handler (DBusConnection *connection, DBusMessage *message, void *user_data)
{
	DBusMessage		*reply = NULL;
	
	if (strcmp(dbus_message_get_path(message),NPD_DBUS_ETHPORTS_OBJPATH) == 0) 
	{
		syslog_ax_dbus_dbg("npd obj path "NPD_DBUS_ETHPORTS_OBJPATH);
		if (dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_METHOD_SHOW_ETHPORT_LIST)) 
		{
			reply = npd_dbus_ethports_interface_show_ethport_list(connection,message,user_data);
		} 
		else if (dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_METHOD_CONFIG_ETHPORT_MODE)) 
		{
			reply = npd_dbus_config_port_mode(connection,message,user_data);
		} 
		else if (dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_METHOD_CONFIG_ETHPORT_MODE_DEL_VE)) 
		{
			reply = npd_dbus_del_ve_intf(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_METHOD_CONFIG_ETHPORT_INTERFACE)) 
		{
			reply = npd_dbus_config_port_interface_mode(connection,message,user_data);
		} 
		else if (dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_LACP_FUNCTION_ENDIS)) 
		{
			reply = npd_dbus_port_lacp_function_enable(connection,message,user_data);
		} 
		else if (dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_METHOD_CONFIG_ETHPORT_MODE_VE)) 
		{
			reply = npd_dbus_create_ve_intf(connection,message,user_data);
		} 
		/* add by yinlm@autelan.com for queue wrr and sp */
		else if (dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_METHOD_CONFIG_BUFFER_MODE)) 
		{
			reply = npd_dbus_config_buffer_mode(connection,message,user_data);
		} 
		else if (dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_METHOD_SHOW_BUFFER_MODE)) 
		{
			reply = npd_dbus_show_buffer_mode(connection,message,user_data);
		} 
		else if (dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_METHOD_SHOW_ETHPORT_ATTR)) 
		{
			reply = npd_dbus_ethports_interface_show_ethport_attr(connection,message,user_data);
		} 
		else if (dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_METHOD_SHOW_ETHPORT_IPG)) 
		{
			reply = npd_dbus_ethports_interface_show_ethport_ipg(connection,message,user_data);
		} 
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_METHOD_SHOW_ETHPORT_STAT))
		{
			reply = npd_dbus_ethports_interface_show_ethport_stat(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_METHOD_SHOW_XG_ETHPORT_STAT))
		{
			reply = npd_dbus_ethports_interface_show_xg_ethport_stat(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_METHOD_CLEAR_ETHPORT_STAT))
		{
			reply = npd_dbus_ethports_interface_clear_ethport_stat(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_METHOD_CONFIG_PORT))
		{
			reply = npd_dbus_ethports_interface_config_ethport_one(connection,message,user_data);
		}

		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_METHOD_CONFIG_PORT_IPG))
		{
			reply = npd_dbus_ethports_ipg(connection,message,user_data);
		}
		
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_METHOD_CONFIG_PORT_ATTR))
		{
			reply = npd_dbus_ethports_interface_config_ports_attr(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_METHOD_CONFIG_PORT_VCT))
		{
			reply = npd_dbus_ethports_interface_config_ports_vct(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_METHOD_READ_PORT_VCT))
		{
			reply = npd_dbus_ethports_interface_read_ports_vct(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_METHOD_CONFIG_ETHPORT_MEDIA))
		{
			reply = npd_dbus_ethports_interface_config_ports_media(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_METHOD_CLEAR_ETHPORT_ARP)) 
		{
			reply = npd_dbus_ethport_clear_arp(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_METHOD_SHOW_ETHPORT_ARP)) 
		{
			reply = npd_dbus_ethport_show_arp(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_METHOD_SHOW_TRUNK_ARP)) 
		{
			reply = npd_dbus_trunk_show_arp(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_METHOD_SHOW_TRUNK_NEXTHOP)) 
		{
			reply = npd_dbus_trunk_show_nexthop(connection,message,user_data);
		}		
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_METHOD_SHOW_ETHPORT_NEXTHOP))
		{
			reply = npd_dbus_ethport_show_nexthop(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_METHOD_CONFIG_STP))
		{
			reply = npd_dbus_ethports_interface_config_stp(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_METHOD_CONFIG_G_ALL_STP))
		{
			reply = npd_dbus_stp_all_vlans_bind_to_stpid(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,STP_DBUS_METHOD_GET_BROAD_TYPE))
		{
			reply = npd_dbus_stp_get_broad_id(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,STP_DBUS_METHOD_FUNCTION_SUPPORT))
		{
			reply = npd_dbus_get_broad_stp_function(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_METHOD_STP_GET_PORT_LINK_STATE))
		{
			reply = npd_dbus_stp_get_port_link_state(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_METHOD_STP_GET_PORT_SPEED))
		{
			reply = npd_dbus_stp_get_port_speed(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_METHOD_STP_GET_PORT_DUPLEX_MODE))
		{
			reply = npd_dbus_stp_get_port_duplex_mode(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,RSTP_DBUS_METHOD_CONFIG_NONSTP))
		{
			reply = npd_dbus_stp_set_port_nonstp(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,RSTP_DBUS_METHOD_CONFIG_P2P))
		{
			reply = npd_dbus_stp_set_port_p2p(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,RSTP_DBUS_METHOD_CONFIG_EDGE))
		{
			reply = npd_dbus_stp_set_port_edge(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,MSTP_DBUS_METHOD_CFG_VLAN_ON_MST))
		{
			reply = npd_dbus_stp_set_stpid_for_vlan(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,RSTP_DBUS_METHOD_CONFIG_PORTPRIO))
		{
			reply = npd_dbus_stp_set_port_prio(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,RSTP_DBUS_METHOD_CONFIG_PORT_PATHCOST))
		{
			reply = npd_dbus_stp_set_port_pathcost(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,RSTP_DBUS_METHOD_SHOW_STP_RUNNING_CFG))
		{
			reply = npd_dbus_stp_show_running_cfg(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_METHOD_GET_SLOT_PORT))
		{
			reply = npd_dbus_get_slot_port_by_portindex(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_METHOD_STORM_CONTROL_GLOBAL_MODEL))
		{
			reply = npd_dbus_config_sc_global_model(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_INTERFACE_METHOD_CONFIG_STORM_CONTROL))
		{
			reply = npd_dbus_config_storm_control(connection,message,user_data);
		}			
		/*QOS*/
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_METHOD_BIND_POLICY_MAP))
		{
			reply = npd_dbus_ethport_bind_policy_map(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORT_METHOD_SHOW_POLICY_MAP))
		{
			reply = npd_dbus_ethport_show_policy_map(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_METHOD_UNBIND_POLICY_MAP))
		{
			reply = npd_dbus_ethport_unbind_policy_map(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_METHOD_TRAFFIC_SHAPE))
		{
			reply = npd_dbus_traffic_shape(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_METHOD_SHOW_TRAFFIC))
		{
			reply = npd_dbus_show_traffic_shape(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_METHOD_DELETE_TRAFFIC))
		{
			reply = npd_dbus_delete_traffic_shape(connection,message,user_data);
		}
		
		 /*ACL*/
					
		else if (dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_METHOD_CONFIG_ACL)) 
		{
			reply =  npd_dbus_ethports_interface_config_acl(connection,message,user_data);
		}
		
		else if (dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_METHOD_SHOW_BIND_ACL)) 
		{
			reply = npd_dbus_ethport_show_accesslist(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_METHOD_BIND_ACL_GROUP)) 
		{
			reply = npd_dbus_ethport_bind_acl_group(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_ETHPORTS_METHOD_DELETE_ACL_GROUP)) 
		{
			reply = npd_dbus_ethport_delete_acl_group(connection,message,user_data);
		}
		else if (dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,STP_DBUS_METHOD_GET_PORT_INDEX)) 
		{
			reply = npd_dbus_rstp_get_one_port_index(connection,message,user_data);
		} 
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,STP_DBUS_METHOD_GET_ALL_PORTS_INDEX))
		{
			reply = npd_dbus_rstp_get_all_port_index(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,STP_DBUS_METHOD_GET_ALL_PORTS_INDEX_V1))
		{
			reply = npd_dbus_rstp_get_all_port_index_v1(connection,message,user_data);
		}
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,STP_DBUS_METHOD_GET_SLOTS_PORTS_FROM_INDEX))
		{
			reply = npd_dbus_rstp_get_slot_port_from_index(connection,message,user_data);
		}		
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,	\
										NPD_DBUS_ETHPORTS_INTERFACE_METHOD_SHOW_RUNNING_CONFIG))
		{
			reply = npd_dbus_ethports_show_running_config(connection,message,user_data);
		}		
		else if(dbus_message_is_method_call(message,NPD_DBUS_ETHPORTS_INTERFACE,NPD_DBUS_SYSTEM_CONFIG_PORT_LINK)) {
			reply = npd_dbus_eth_link_state_config (connection,message,user_data);
		}
		else 
		{
			syslog_ax_dbus_err("unknow method call %s",dbus_message_get_member(message));
		}
		
	}
	else {
	
		syslog_ax_dbus_err("unknow obj path %s",dbus_message_get_path(message));
	}
	
	if (reply) 
	{
		dbus_connection_send (connection, reply, NULL);
		dbus_connection_flush(connection); /* TODO    Maybe we should let main loop process the flush*/
		dbus_message_unref (reply);
	}

/*	dbus_message_unref(message); //TODO who should unref the incoming message? */
		return DBUS_HANDLER_RESULT_HANDLED ;
}


/** Message handler for Signals
 *  or normally there should be no signals except dbus-daemon related.
 *
 *  @param  connection          D-BUS connection
 *  @param  message             Message
 *  @param  user_data           User data
 *  @return                     What to do with the message
 */
DBusHandlerResult
npd_dbus_filter_function (DBusConnection * connection,
					   DBusMessage * message, void *user_data)
{
	if (dbus_message_is_signal (message, DBUS_INTERFACE_LOCAL, "Disconnected") &&
		   strcmp (dbus_message_get_path (message), DBUS_PATH_LOCAL) == 0) {

		/* this is a local message; e.g. from libdbus in this process */

		npd_syslog7_notice ("Got disconnected from the system message bus; "
				"trying to reconnect system message bus daemon");
		dbus_connection_unref (npd_dbus_connection);
		npd_dbus_connection = NULL;
		memset(dbusThreadName, 0, 32);
		sprintf(dbusThreadName, "NpdDBUS.r%d", ++reinit_count);
		nam_thread_create(dbusThreadName, npd_dbus_restart_thread, NULL, NPD_TRUE, NPD_TRUE);

		/*g_timeout_add (3000, reinit_dbus, NULL);*/

	} else if (dbus_message_is_signal (message,
			      DBUS_INTERFACE_DBUS,
			      "NameOwnerChanged")) {

		/*if (services_with_locks != NULL)  service_deleted (message);*/
	} else
		return TRUE;
		/*return hald_dbus_filter_handle_methods (connection, message, user_data, FALSE);*/

	return DBUS_HANDLER_RESULT_HANDLED;
}


int npd_dbus_init(void)
{
	
	DBusError dbus_error;
	DBusObjectPathVTable	npd_vtable = {NULL, &npd_dbus_message_handler, NULL, NULL, NULL, NULL};
	DBusObjectPathVTable	ports_vtable = {NULL, &npd_dbus_ethports_message_handler, NULL, NULL, NULL, NULL};
	
	dbus_connection_set_change_sigpipe (TRUE);

	dbus_error_init (&dbus_error);
	npd_dbus_connection = dbus_bus_get (DBUS_BUS_SYSTEM, &dbus_error);
	if (npd_dbus_connection == NULL) {
		syslog_ax_dbus_err ("dbus_bus_get(): %s", dbus_error.message);
		return FALSE;
	}

	/* Use npd to handle subsection of NPD_DBUS_OBJPATH including slots*/
	if (!dbus_connection_register_fallback (npd_dbus_connection, NPD_DBUS_OBJPATH, &npd_vtable, NULL)) {
		syslog_ax_dbus_err("can't register D-BUS handlers (fallback NPD). cannot continue.");
		return FALSE;
		
	}
	
    /* use port handler to handle exact port related functions.*/
	if (!dbus_connection_register_object_path (npd_dbus_connection, NPD_DBUS_ETHPORTS_OBJPATH, &ports_vtable, NULL)) {
        	syslog_ax_dbus_err("can't register D-BUS handlers (obj PORTS)). cannot continue.");
		return FALSE;
		
	}
	
	dbus_bus_request_name (npd_dbus_connection, NPD_DBUS_BUSNAME,
			       0, &dbus_error);
	
	
	if (dbus_error_is_set (&dbus_error)) {
		syslog_ax_dbus_err ("dbus_bus_request_name(): %s",
			    dbus_error.message);
		return FALSE;
	}

	dbus_connection_add_filter (npd_dbus_connection, npd_dbus_filter_function, NULL, NULL);

	dbus_bus_add_match (npd_dbus_connection,
			    "type='signal'"
					    ",interface='"DBUS_INTERFACE_DBUS"'"
					    ",sender='"DBUS_SERVICE_DBUS"'"
					    ",member='NameOwnerChanged'",
			    NULL);

	return TRUE;
  
}

int npd_dbus_reinit(void)
{
	DBusError dbus_error;
	DBusObjectPathVTable	npd_vtable = {NULL, &npd_dbus_message_handler, NULL, NULL, NULL, NULL};
	DBusObjectPathVTable	ports_vtable = {NULL, &npd_dbus_ethports_message_handler, NULL, NULL, NULL, NULL};
	
	dbus_connection_set_change_sigpipe (TRUE);

	dbus_error_init (&dbus_error);
	npd_dbus_connection = dbus_bus_get (DBUS_BUS_SYSTEM, &dbus_error);
	syslog_ax_dbus_dbg("reinit npd dbus connection %d\n", reinit_count);/*remove*/
	if (npd_dbus_connection == NULL) {
		syslog_ax_dbus_err ("dbus_bus_get(): %s", dbus_error.message);
		return FALSE;
	}

	/* Use npd to handle subsection of NPD_DBUS_OBJPATH including slots*/
	if (!dbus_connection_register_fallback (npd_dbus_connection, NPD_DBUS_OBJPATH, &npd_vtable, NULL)) {
		syslog_ax_dbus_err("can't register D-BUS handlers (fallback NPD). cannot continue.");
		return FALSE;
		
	}
	
    /* use port handler to handle exact port related functions.*/
	if (!dbus_connection_register_object_path (npd_dbus_connection, NPD_DBUS_ETHPORTS_OBJPATH, &ports_vtable, NULL)) {
        	syslog_ax_dbus_err("can't register D-BUS handlers (obj PORTS)). cannot continue.");
		return FALSE;
		
	}
	
	dbus_bus_request_name (npd_dbus_connection, NPD_DBUS_BUSNAME,
			       0, &dbus_error);
	
	
	if (dbus_error_is_set (&dbus_error)) {
		syslog_ax_dbus_err ("dbus_bus_request_name(): %s",
			    dbus_error.message);
		return FALSE;
	}

	dbus_connection_add_filter (npd_dbus_connection, npd_dbus_filter_function, NULL, NULL);

	dbus_bus_add_match (npd_dbus_connection,
			    "type='signal'"
					    ",interface='"DBUS_INTERFACE_DBUS"'"
					    ",sender='"DBUS_SERVICE_DBUS"'"
					    ",member='NameOwnerChanged'",
			    NULL);

	return TRUE;
  
}

void * npd_dbus_thread_main(void *arg)
{
	/* tell my thread id*/
	npd_init_tell_whoami((unsigned char*)"NpdDBUS",0);

	/* tell about my initialization process done*/
	npd_init_tell_stage_end();
	
	/*
	For all OAM method call, synchronous is necessary.
	Only signal/event could be asynchronous, it could be sent in other thread.
	*/	
	while (dbus_connection_read_write_dispatch(npd_dbus_connection,-1)) {
		;
	}
	return NULL;
}

void *npd_dbus_restart_thread()
{
	int loop_count = 0;
	/*
	For all OAM method call, synchronous is necessary.
	Only signal/event could be asynchronous, it could be sent in other thread.
	*/
	syslog_ax_dbus_dbg("restart dbus connection total %d times\n",reinit_count);	

	/* tell my thread id*/
	memset(dbusThreadName, 0, 32);
	sprintf(dbusThreadName, "NpdDBUS.r%d", reinit_count);
	npd_init_tell_whoami((unsigned char*)dbusThreadName,0);/* remove*/
	if(npd_dbus_reinit()){
		while (dbus_connection_read_write_dispatch(npd_dbus_connection,500)) {
			syslog_ax_dbus_dbg("restarted dbus connection%d dispatch loop %d\n",reinit_count, loop_count++);
		}
	}
	syslog_ax_dbus_err("there is something wrong with npd dbus handler\n");
    return NULL;	/* code optimize: Missing return statement. zhangdi@autelan.com 2013-01-18 */
}

#ifdef __cplusplus
}
#endif
