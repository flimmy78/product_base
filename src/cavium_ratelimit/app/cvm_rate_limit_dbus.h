
#include "dbus/cvm_rate/cvm_rate_dbus_def.h"

#ifndef __CVM_RATE_LIMIT_H__
#define __CVM_RATE_LIMIT_H__

#define CVM_RATE_RULE_CONF_FILE_NAME "cvm_rate_limit_rule.conf"

#define CVM_RATE_RULE_CONF_FILE_PATH "/mnt/cvm_rate_limit_rule.conf"

#define CR_MAXNR 255

#define _1K 1024
#define RULES_INT_NUM_PER_LINE 4

#define CR_SET_FLAG_CVM_ETH  0x01
#define CR_SET_FLAG_FAST_FWD 0x02


#define DYNAMIC_MAJOR_NUM

#ifndef DYNAMIC_MAJOR_NUM

#define CRADDRULES 		_IOWR(CR_IOC_MAGIC, 0x1, struct cr_ioctl_struct)
#define CRDELRULES 		_IOWR(CR_IOC_MAGIC, 0x2, struct cr_ioctl_struct)
#define CRMODIFYRULES 	_IOWR(CR_IOC_MAGIC, 0x3, struct cr_ioctl_struct)
#define CRMODIFYMASKS 	_IOWR(CR_IOC_MAGIC, 0x4, struct cr_ioctl_struct)
#define CRMODIFYLIMITER _IOWR(CR_IOC_MAGIC, 0x5, struct cr_ioctl_struct)
#define CRMODIFYNAME 	_IOWR(CR_IOC_MAGIC, 0x6, struct cr_ioctl_struct)
#define CRGETRULE    	_IOWR(CR_IOC_MAGIC, 0x7, struct cr_ioctl_struct)
#define CRGETRULEBYINDEX    	_IOWR(CR_IOC_MAGIC, 0x8, struct cr_ioctl_struct)
#define CRENABLESET		_IOWR(CR_IOC_MAGIC, 0x9, struct cr_ioctl_struct)
#define CRCHECKDEFAULT 		_IOWR(CR_IOC_MAGIC, 0xa, struct cr_ioctl_struct)
#define CRENABLEGET 		_IOWR(CR_IOC_MAGIC, 0xb, struct cr_ioctl_struct)
#define CRSTACOUNTGET		_IOWR(CR_IOC_MAGIC, 0xc, struct cr_ioctl_struct)
#define CRSTACOUNTCLEAR		_IOWR(CR_IOC_MAGIC, 0xd, struct cr_ioctl_struct)
#define CRDMESGENABLESET	_IOWR(CR_IOC_MAGIC, 0xe, struct cr_ioctl_struct)
#define CRCLEARUDFRULES	_IOWR(CR_IOC_MAGIC, 0xf, struct cr_ioctl_struct)
#define CRGETRULEBYIDX4FFWD	_IOWR(CR_IOC_MAGIC, 0x10, struct cr_ioctl_struct)
#define CRGETRULE4FFWD	_IOWR(CR_IOC_MAGIC, 0x11, struct cr_ioctl_struct)
#define IOCTLCMD(cmd)	cmd
#else
#define CRADDRULES 		0x1
#define CRDELRULES 		0x2
#define CRMODIFYRULES 	0x3
#define CRMODIFYMASKS 	0x4
#define CRMODIFYLIMITER 0x5
#define CRMODIFYNAME 	0x6
#define CRGETRULE    	0x7
#define CRGETRULEBYINDEX    	0x8
#define CRENABLESET		0x9
#define CRCHECKDEFAULT 		0xa
#define CRENABLEGET 		0xb
#define CRSTACOUNTGET		0xc
#define CRSTACOUNTCLEAR		0xd
#define CRDMESGENABLESET	0xe
#define CRCLEARUDFRULES		0xf
#define CRGETRULEBYIDX4FFWD 0x10
#define CRGETRULE4FFWD		0x11
#define IOCTLCMD(nr) _IOWR(cr_major_num, nr, struct cr_ioctl_struct)
#endif



int cvm_rate_limit_dbus_init(void);
void * cvm_rate_init_dbus_thread_main(void *arg);
static DBusHandlerResult cvm_rate_limit_dbus_message_handler 
(
	DBusConnection *connection, 
	DBusMessage *message, 
	void *user_data
);

DBusMessage * cvm_rate_limit_dbus_enable_set(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * cvm_rate_limit_dbus_loglevel_set(DBusConnection *conn, DBusMessage *msg, void *user_data);

DBusMessage * cvm_rate_limit_dbus_show_rules(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * cvm_rate_limit_dbus_add_rule(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * cvm_rate_limit_dbus_del_rule(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * cvm_rate_limit_dbus_modify_rule(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * cvm_rate_limit_dbus_show_running(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * cvm_rate_limit_dbus_show_statistic(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * cvm_rate_limit_dbus_clear_statistic(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * cvm_rate_limit_dbus_dmesg_enable_set(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * cvm_rate_limit_dbus_config_rules_source(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * cvm_rate_limit_dbus_load_rules_from_file(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * cvm_rate_limit_dbus_restore_rules_to_file(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * cvm_rate_limit_dbus_clear_rule(DBusConnection *conn, DBusMessage *msg, void *user_data);
/* sunhanyu add for add tcp/udp:port rule much easier */
DBusMessage * cvm_rate_limit_dbus_add_rule_simp(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * cvm_rate_limit_dbus_service_load(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * cvm_rate_limit_dbus_service_unload(DBusConnection *conn, DBusMessage *msg, void *user_data);


#endif
