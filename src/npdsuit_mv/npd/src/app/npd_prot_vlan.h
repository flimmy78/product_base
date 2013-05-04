#ifndef __NPD_PROT_VLAN_H__
#define __NPD_PROT_VLAN_H__

unsigned short prot_vlan_entry_protocol[PROTOCOL_VLAN_MAP_MAX] = {0};
unsigned char *prot_vlan_prot_name[PROTOCOL_VLAN_MAP_MAX] = {"ipv4","ipv6","pppoe","pppoe_s","udf"};


void npd_prot_vlan_init(void);
DBusMessage * npd_dbus_prot_vlan_port_enable_config(DBusConnection *conn, DBusMessage *msg, void *user_data);

DBusMessage * npd_dbus_prot_vlan_config_vid_by_port_entry(DBusConnection *conn, DBusMessage *msg, void *user_data);

/****************************************************************************************
 * OUTPUT:
 *                              PROTOCOL_VLAN_RETURN_CODE_SUCCESS
 *                              COMMON_RETURN_CODE_BADPARAM
 *                              PROTOCOL_VLAN_RETURN_CODE_NAM_ERR
 *                              PROTOCOL_VLAN_RETURN_CODE_VLAN_NOT_MATCH
 *****************************************************************************************/

DBusMessage * npd_dbus_prot_vlan_no_vid_by_port_entry(DBusConnection *conn, DBusMessage *msg, void *user_data) ;

DBusMessage * npd_dbus_prot_vlan_config_udf_ethtype(DBusConnection *conn, DBusMessage *msg, void *user_data);

DBusMessage * npd_dbus_prot_vlan_show_port_cfg(DBusConnection *conn, DBusMessage *msg, void *user_data);

DBusMessage * npd_dbus_prot_vlan_show_port_cfg_list(DBusConnection *conn, DBusMessage *msg, void *user_data);

void npd_prot_vlan_config_udf_ethtype_save_cfg
(
	char** showStr,
	int* avalidLen
);
int np_prot_vlan_udf_ethtype_vid_enable_set
(
    unsigned int isEnable
);

int npd_prot_vlan_vid_check_for_all_port
(
    unsigned short vlanId
);

int npd_prot_vlan_vid_check_by_slot_port
(
    unsigned char slotno,
    unsigned char local_port_no,
    unsigned short vlanId
);

int npd_prot_vlan_vid_check_by_global_index
(
    unsigned int eth_g_index,
    unsigned short vlanId
);

#endif
