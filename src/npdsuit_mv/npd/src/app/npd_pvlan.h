#ifndef __NPD_PVLAN_H__
#define __NPD_PVLAN_H__

#include <dbus/dbus.h>
#include "npd/nam/npd_amapi.h"

#define PVE_MAX_PORT    64
#define PVE_SAVE_BUFF  (1024*1024)
#define NPD_DBUS_SUCCESS 0
#define NPD_DBUS_NOT_SUPPORT 3

typedef enum{
	PVE_FAIL=0,
	PVE_TRUE=1,
}PVE_BOOL;

typedef struct{
	unsigned short  fag;
	unsigned short  tkfag;
	unsigned char 	pvslot;
	unsigned char 	pvport;
	unsigned char 	lkslot;
	unsigned char 	lkport;
	unsigned int		mode;
}pvenode;

typedef struct{
	unsigned short  fag;
	unsigned char 	pvslot;
	unsigned char 	pvport;
	unsigned char   lktrunk;
}pvetrunk;


int npd_pvlan_init
(
	void
);

int npd_pvlan_check_port_ispvlan
(
	unsigned char slot,
	unsigned char port
);

extern  int nam_pvlan_enable
(
	void
);

extern int nam_pvlan_port_config(unsigned int eth_g_index1,unsigned int eth_g_index2);
extern int nam_pvlan_port_config_spi(unsigned int eth_g_index1,unsigned char dev,unsigned char port);

extern int nam_pvlan_trunk_config(unsigned int eth_g_index1, unsigned char trunkid);

extern int nam_pvlan_control_config(unsigned int eth_g_index1,int flag);

extern int nam_pvlan_port_delete(unsigned int eth_g_index1);

extern int nam_pvlan_create(unsigned int pvlanid);
extern int nam_pvlan_add_port(unsigned int eth_g_index1);
extern int nam_pvlan_delete_port(unsigned int eth_g_index1);
extern int nam_pvlan_cpu_port(unsigned int flag);


/*********************************************************************************
 *		NPD dbus operation
 *
 *********************************************************************************/
DBusMessage * npd_dbus_pvlan_create
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_pvlan_add_port
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) ;

DBusMessage * npd_dbus_pvlan_delete_port
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) ;

DBusMessage * npd_dbus_pvlan_cpu_port
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) ;

DBusMessage * npd_dbus_pvlan_config_port
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) ;

DBusMessage * npd_dbus_pvlan_config_spi_port
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) ;

DBusMessage * npd_dbus_pvlan_config_trunk
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) ;

DBusMessage * npd_dbus_pvlan_show_pvlan
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) ;

DBusMessage * npd_dbus_pvlan_port_delete
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) ;

DBusMessage * npd_dbus_pvlan_config_control
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) ;
#endif // _NPD_PVLAN_H
