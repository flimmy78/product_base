#ifndef __NPD_ACL_API_H_
#define __NPD_ACL_API_H_

#include <dbus/dbus.h>

void npd_acl_init();
DBusMessage * npd_dbus_config_acl(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_show_acl_service(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_show_accesslist(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_delete_acl (DBusConnection *conn, DBusMessage *msg, void *user_data);

/*trap*/
DBusMessage * npd_dbus_config_trap_to_cpu_ip(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_config_trap_to_cpu_tcp(DBusConnection *conn, DBusMessage *msg, void *user_data); 
DBusMessage * npd_dbus_config_trap_to_cpu_udp(DBusConnection *conn, DBusMessage *msg, void *user_data); 
DBusMessage * npd_dbus_config_trap_to_cpu_icmp(DBusConnection *conn, DBusMessage *msg, void *user_data); 
DBusMessage * npd_dbus_config_trap_to_cpu_ethernet(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_config_trap_to_cpu_arp(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_config_ext_permit_deny_trap(DBusConnection *conn, DBusMessage *msg, void *user_data);

/*permit deny*/
DBusMessage * npd_dbus_config_permit_deny_tcp_or_udp(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_config_permit_deny_mac(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_config_permit_deny_ip(DBusConnection *conn, DBusMessage *msg, void *user_data); 
DBusMessage * npd_dbus_config_permit_deny_icmp(DBusConnection *conn, DBusMessage *msg, void *user_data); 
DBusMessage * npd_dbus_config_permit_deny_arp(DBusConnection *conn, DBusMessage *msg, void *user_data); 

/*mirror redirect*/
DBusMessage * npd_dbus_config_mirror_redirect_mac(DBusConnection *conn, DBusMessage *msg, void *user_data) ;
DBusMessage * npd_dbus_config_mirror_redirect_ip(DBusConnection *conn, DBusMessage *msg, void *user_data) ;
DBusMessage * npd_dbus_config_mirror_redirect_tcp_udp(DBusConnection *conn, DBusMessage *msg, void *user_data) ;
DBusMessage * npd_dbus_config_mirror_redirect_icmp(DBusConnection *conn, DBusMessage *msg, void *user_data) ;
DBusMessage * npd_dbus_config_mirror_redirect_arp(DBusConnection *conn, DBusMessage *msg, void *user_data) ;
DBusMessage * npd_dbus_config_ext_mirror_redirect(DBusConnection *conn, DBusMessage *msg, void *user_data);
/*acl group*/
DBusMessage * npd_dbus_creat_acl_group(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_add_delete_rule_to_acl_group(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_delete_acl_group(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_show_acl_group(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_config_acl_group(DBusConnection *conn, DBusMessage *msg, void *user_data);
/*interface*/
DBusMessage * npd_dbus_ethports_interface_config_acl(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_ethport_show_accesslist(DBusConnection *conn, DBusMessage *msg, void *user_data); 
DBusMessage * npd_dbus_ethport_bind_acl_group(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_ethport_delete_acl_group(DBusConnection *conn, DBusMessage *msg, void *user_data); 
/*running*/
DBusMessage * npd_dbus_acl_group_show_running_config(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_acl_rule_show_running_config(DBusConnection *conn, DBusMessage *msg, void *user_data); 

/*qos*/
DBusMessage * npd_dbus_config_qos_base_acl_ingress(DBusConnection *conn, DBusMessage *msg, void *user_data) ;
DBusMessage * npd_dbus_config_qos_base_acl_egress(DBusConnection *conn, DBusMessage *msg, void *user_data) ;

DBusMessage * npd_dbus_show_accesslist_index(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_show_acl_group_index(DBusConnection *conn, DBusMessage *msg, void *user_data); 

/*time-range*/
DBusMessage * npd_dbus_time_range_set(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_time_absolute_set(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_time_periodic_set(DBusConnection *conn, DBusMessage *msg, void *user_data); 
DBusMessage * npd_dbus_time_range_show(DBusConnection *conn, DBusMessage *msg, void *user_data);


#endif
