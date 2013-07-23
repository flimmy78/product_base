#ifndef __NPD_LOG_H__
#define __NPD_LOG_H__

#include <dbus/dbus.h>

/**
 * @addtogroup HalDaemonLogging
 *
 * @{
 */


/** Logging levels for NPD daemon
 */

#define NPD_OK              0
#define NPD_ERR             (NPD_OK + 1)

#define SYSLOG_LOG_EMERG    LOG_EMERG
#define SYSLOG_LOG_ALERT    LOG_ALERT
#define SYSLOG_LOG_CRIT     LOG_CRIT
#define SYSLOG_LOG_ERR      LOG_ERR
#define SYSLOG_LOG_WARNING  LOG_WARNING
#define SYSLOG_LOG_NOTICE   LOG_NOTICE
#define SYSLOG_LOG_INFO     LOG_INFO

enum {
	SYSLOG_DBG_DBG     = 0x1,	/*normal */
	SYSLOG_DBG_WAR     = 0x2,	/*warning*/
	SYSLOG_DBG_ERR     = 0x4,	/* error*/
	SYSLOG_DBG_EVT     = 0x8,	/* event*/
	SYSLOG_DBG_PKT_REV = 0x10,  /*packet receive*/
	SYSLOG_DBG_PKT_SED = 0x20,  /*packet send*/
	SYSLOG_DBG_PKT_ALL = 0x30,  /*packet send and receive*/
	SYSLOG_DBG_INFO    = 0x40,  /* info , for important thread of distributed system */
	SYSLOG_DBG_DEF = SYSLOG_DBG_INFO,/*(SYSLOG_DBG_WAR|SYSLOG_DBG_ERR|SYSLOG_DBG_EVT)*/	/* default value ,error,warning and event*/
	SYSLOG_DBG_ALL = 0xFF	/* all*/
};

extern unsigned int npd_log_level ;
extern unsigned int npd_log_close;
extern char * npd_log_ident;


extern unsigned int nam_log_close;
extern unsigned int nbm_log_close;

/**********************************************************************************
 *	npd_syslog_dbg
 * 
 *	output the daemon debug info to /var/log/daemon.log
 *
 *  INPUT:
 * 	 	event - 
 *		format - 
 *  
 *  OUTPUT:
 * 	 NULL
 *
 *  RETURN:
 * 	 NULL
 * 	 
 *
 **********************************************************************************/
void npd_syslog_dbg
(
	char *format,
	...
);

/**********************************************************************************
 *	npd_syslog_err
 * 
 *	output the daemon debug info to /var/log/daemon.log
 *
 *  INPUT:
 * 	 	event - 
 *		format - 
 *  
 *  OUTPUT:
 * 	 NULL
 *
 *  RETURN:
 * 	 NULL
 * 	 
 *
 **********************************************************************************/
void npd_syslog_err
(
	char *format,
	...
);

/**********************************************************************************
 *	npd_syslog_pkt_send
 * 
 *	output the daemon debug info to /var/log/daemon.log
 *
 *  INPUT:
 * 	 	event - 
 *		format - 
 *  
 *  OUTPUT:
 * 	 NULL
 *
 *  RETURN:
 * 	 NULL
 * 	 
 *
 **********************************************************************************/
void npd_syslog_pkt_send
(
	char *format,
	...
);


/**********************************************************************************
 *	npd_syslog_pkt_rev
 * 
 *	output the daemon debug info to /var/log/daemon.log
 *
 *  INPUT:
 * 	 	event - 
 *		format - 
 *  
 *  OUTPUT:
 * 	 NULL
 *
 *  RETURN:
 * 	 NULL
 * 	 
 *
 **********************************************************************************/
void npd_syslog_pkt_rev
(
	char *format,
	...
);


/**********************************************************************************
 *	npd_syslog_warning
 * 
 *	output the daemon debug info to /var/log/daemon.log
 *
 *  INPUT:
 * 	 	event - 
 *		format - 
 *  
 *  OUTPUT:
 * 	 NULL
 *
 *  RETURN:
 * 	 NULL
 * 	 
 *
 **********************************************************************************/
void npd_syslog_warning
(
	char *format,
	...
);

/**********************************************************************************
 *	npd_syslog_event
 * 
 *	output the daemon debug info to /var/log/daemon.log
 *
 *  INPUT:
 * 	 	event - 
 *		format - 
 *  
 *  OUTPUT:
 * 	 NULL
 *
 *  RETURN:
 * 	 NULL
 * 	 
 *
 **********************************************************************************/
void npd_syslog_event
(
	char *format,
	...
);

/**********************************************************************************
 *	npd_syslog_info
 * 
 *	output the daemon debug info to /var/log/daemon.log
 *
 *  INPUT:
 * 	 	event - 
 *		format - 
 *  
 *  OUTPUT:
 * 	 NULL
 *
 *  RETURN:
 * 	 NULL
 * 	 
 *
 **********************************************************************************/
void npd_syslog_info
(
	char *format,
	...
);


/**********************************************************************************
 *	npd_syslog7_notice
 * 
 *	output the daemon debug info to /var/log/daemon.log
 *
 *  INPUT:
 * 	 	event - 
 *		format - 
 *  
 *  OUTPUT:
 * 	 NULL
 *
 *  RETURN:
 * 	 NULL
 * 	 
 *
 **********************************************************************************/
void npd_syslog7_notice
(
	char *format,
	...
);


int npd_log_set_debug_value(unsigned int val_mask);
int npd_log_set_no_debug_value(unsigned int val_mask);
/**********************************************************************************
 *	npd_log_status_get
 * 
 *  DESCRIPTION:
 *	This function get npd debug level
 *
 *  INPUT:
 *  
 *  OUTPUT:
 * 	 NULL
 *
 *  RETURN:
 *	 current log settings
 *
 **********************************************************************************/
unsigned int npd_log_status_get
(
	void
);

/**********************************************************************************
 *	npd_log_level_set
 * 
 *  DESCRIPTION:
 *	This function set up npd debug level
 *
 *  INPUT:
 * 	 	level - debug level value
 *  
 *  OUTPUT:
 * 	 NULL
 *
 *  RETURN:
 *	 0 - debug level setup successfully.
 *
 **********************************************************************************/
int npd_log_level_set
(
	unsigned int level
);

/**********************************************************************************
 *	npd_syslog_emit
 * 
 *	output the log info to /var/log/daemon.log or startup log file NPD_SYSTEM_STARTUP_LOG_PATH
 *
 *  INPUT:
 * 	 	event - 
 *		format - 
 *  
 *  OUTPUT:
 * 	 NULL
 *
 *  RETURN:
 * 	 NULL
 * 	 
 *
 **********************************************************************************/
void npd_syslog_emit
(
	int priority,
	char *buf
);

/* syslog for trunk*/
#define syslog_ax_trunk_err	          npd_syslog_err
#define syslog_ax_trunk_dbg	          npd_syslog_dbg
#define syslog_ax_trunk_pkt_rev       npd_syslog_pkt_rev
#define syslog_ax_trunk_pkt_send      npd_syslog_pkt_send
#define syslog_ax_trunk_warning       npd_syslog_warning
#define syslog_ax_trunk_event         npd_syslog_event

/* syslog for fdb*/
#define syslog_ax_fdb_err	          npd_syslog_err
#define syslog_ax_fdb_dbg	          npd_syslog_dbg
#define syslog_ax_fdb_pkt_rev       npd_syslog_pkt_rev
#define syslog_ax_fdb_pkt_send      npd_syslog_pkt_send
#define syslog_ax_fdb_warning       npd_syslog_warning
#define syslog_ax_fdb_event         npd_syslog_event


/* syslog for acl*/
#define syslog_ax_acl_err             npd_syslog_err
#define syslog_ax_acl_dbg             npd_syslog_dbg
#define syslog_ax_acl_pkt_rev         npd_syslog_pkt_rev
#define syslog_ax_acl_pkt_send        npd_syslog_pkt_send
#define syslog_ax_acl_warning         npd_syslog_warning
#define syslog_ax_acl_event           npd_syslog_event

/*syslog for acl_drv*/
#define syslog_ax_acl_drv_err         npd_syslog_err
#define syslog_ax_acl_drv_dbg         npd_syslog_dbg
#define syslog_ax_acl_drv_event       npd_syslog_event
#define syslog_ax_acl_drv_pkt_rev     npd_syslog_pkt_rev
#define syslog_ax_acl_drv_pkt_send    npd_syslog_pkt_send
#define syslog_ax_acl_drv_warning     npd_syslog_warning

/* syslog for eth-port*/
#define syslog_ax_eth_port_err        npd_syslog_err
#define syslog_ax_eth_port_dbg        npd_syslog_dbg
#define syslog_ax_eth_port_pkt_rev    npd_syslog_pkt_rev
#define syslog_ax_eth_port_pkt_send   npd_syslog_pkt_send
#define syslog_ax_eth_port_evt        npd_syslog_event
#define syslog_ax_eth_port_warning    npd_syslog_warning
#define syslog7_ax_eth_port_notice    npd_syslog7_notice

/* syslog for hash */
#define syslog_ax_hash_err            npd_syslog_err
#define syslog_ax_hash_dbg            npd_syslog_dbg
#define syslog_ax_hash_pkt_rev        npd_syslog_pkt_rev
#define syslog_ax_hash_pkt_send       npd_syslog_pkt_send
#define syslog_ax_hash_event          npd_syslog_event
#define syslog_ax_hash_warning        npd_syslog_warning


/* syslog for interface*/
#define syslog_ax_intf_err            npd_syslog_err
#define syslog_ax_intf_dbg            npd_syslog_dbg
#define syslog_ax_intf_event          npd_syslog_event
#define syslog_ax_intf_warning        npd_syslog_warning
#define syslog_ax_intf_pkt_rev        npd_syslog_pkt_rev
#define syslog_ax_intf_pkt_send       npd_syslog_pkt_send


/* syslog for private vlan*/
#define syslog_ax_pvlan_err           npd_syslog_err
#define syslog_ax_pvlan_dbg           npd_syslog_dbg
#define syslog_ax_pvlan_event         npd_syslog_event
#define syslog_ax_pvlan_pkt_rev       npd_syslog_pkt_rev
#define syslog_ax_pvlan_pkt_send      npd_syslog_pkt_send
#define syslog_ax_pvlan_warning       npd_syslog_warning


/* syslog for vlan */
#define syslog_ax_vlan_err             npd_syslog_err
#define syslog_ax_vlan_dbg             npd_syslog_dbg
#define syslog_ax_vlan_event           npd_syslog_event
#define syslog_ax_vlan_pkt_rev         npd_syslog_pkt_rev
#define syslog_ax_vlan_pkt_send        npd_syslog_pkt_send
#define syslog_ax_vlan_warning         npd_syslog_warning

/*syslog for connect-table */
#define syslog_ax_conntable_err             npd_syslog_err
#define syslog_ax_conntable_dbg             npd_syslog_dbg
#define syslog_ax_conntable_event           npd_syslog_event
#define syslog_ax_conntable_pkt_rev         npd_syslog_pkt_rev
#define syslog_ax_conntable_pkt_send        npd_syslog_pkt_send
#define syslog_ax_conntable_warning         npd_syslog_warning



/* syslog for igmp*/
#define syslog_ax_igmp_err             npd_syslog_err
#define syslog_ax_igmp_dbg             npd_syslog_dbg
#define syslog_ax_igmp_pkt_rev         npd_syslog_pkt_rev
#define syslog_ax_igmp_pkt_send        npd_syslog_pkt_send
#define syslog_ax_igmp_warning         npd_syslog_warning
#define syslog_ax_igmp_event           npd_syslog_event


/* syslog for route*/
#define syslog_ax_route_err            npd_syslog_err
#define syslog_ax_route_dbg            npd_syslog_dbg
#define syslog_ax_route_pkt_rev        npd_syslog_pkt_rev
#define syslog_ax_route_pkt_send       npd_syslog_pkt_send
#define syslog_ax_route_warning        npd_syslog_warning
#define syslog_ax_route_event          npd_syslog_event
#define syslog7_ax_route_notice        npd_syslog7_notice


/* syslog for arp snooping*/
#define syslog_ax_arpsnooping_err      npd_syslog_err
#define syslog_ax_arpsnooping_dbg      npd_syslog_dbg
#define syslog_ax_arpsnooping_event    npd_syslog_event
#define syslog_ax_arpsnooping_warning  npd_syslog_warning
#define syslog_ax_arpsnooping_pkt_rev  npd_syslog_pkt_rev
#define syslog_ax_arpsnooping_pkt_send npd_syslog_pkt_send


/* syslog for chassis slot*/
#define syslog_ax_c_slot_err          npd_syslog_err
#define syslog_ax_c_slot_dbg          npd_syslog_dbg
#define syslog_ax_c_slot_event        npd_syslog_event
#define syslog_ax_c_slot_warning      npd_syslog_warning
#define syslog_ax_c_slot_pkt_rev      npd_syslog_pkt_rev
#define syslog_ax_c_slot_pkt_send     npd_syslog_pkt_send

/* syslog for system forwarding engine*/
#define syslog_ax_engine_err          npd_syslog_err
#define syslog_ax_engine_dbg          npd_syslog_dbg 
#define syslog_ax_engine_event        npd_syslog_event 
#define syslog_ax_engine_pkt_rev      npd_syslog_pkt_rev 
#define syslog_ax_engine_pkt_send     npd_syslog_pkt_send
#define syslog_ax_engine_warning      npd_syslog_warning 


/* syslog for chassis extended slot*/
#define syslog_ax_e_slot_err          npd_syslog_err
#define syslog_ax_e_slot_dbg          npd_syslog_dbg
#define syslog_ax_e_slot_event        npd_syslog_event
#define syslog_ax_e_slot_pkt_rev      npd_syslog_pkt_rev
#define syslog_ax_e_slot_pkt_send     npd_syslog_pkt_send
#define syslog_ax_e_slot_warning      npd_syslog_warning


/*syslog for mirror*/
#define syslog_ax_mirror_err          npd_syslog_err
#define syslog_ax_mirror_dbg          npd_syslog_dbg
#define syslog_ax_mirror_event        npd_syslog_event
#define syslog_ax_mirror_warning      npd_syslog_warning
#define syslog_ax_mirror_pkt_rev      npd_syslog_pkt_rev
#define syslog_ax_mirror_pkt_send     npd_syslog_pkt_send


/* syslog for system dbus*/
#define syslog_ax_dbus_err            npd_syslog_err
#define syslog_ax_dbus_dbg            npd_syslog_dbg
#define syslog_ax_dbus_event          npd_syslog_event
#define syslog_ax_dbus_warning        npd_syslog_warning
#define syslog_ax_dbus_pkt_rev        npd_syslog_pkt_rev
#define syslog_ax_dbus_pkt_send       npd_syslog_pkt_send


/* syslog for product*/
#define syslog_ax_product_err         npd_syslog_err
#define syslog_ax_product_dbg         npd_syslog_dbg
#define syslog_ax_product_event       npd_syslog_event
#define syslog_ax_product_warning     npd_syslog_warning
#define syslog_ax_product_pkt_rev     npd_syslog_pkt_rev
#define syslog_ax_product_pkt_send    npd_syslog_pkt_send


/* syslog for rstp*/
#define syslog_ax_rstp_err            npd_syslog_err
#define syslog_ax_rstp_dbg            npd_syslog_dbg
#define syslog_ax_rstp_event          npd_syslog_event
#define syslog_ax_rstp_pkt_rev        npd_syslog_pkt_rev
#define syslog_ax_rstp_pkt_send       npd_syslog_pkt_send
#define syslog_ax_rstp_warning        npd_syslog_warning


/* syslog for main function*/
#define syslog_ax_main_err            npd_syslog_err
#define syslog_ax_main_dbg            npd_syslog_dbg
#define syslog_ax_main_event          npd_syslog_event
#define syslog_ax_main_pkt_rev        npd_syslog_pkt_rev
#define syslog_ax_main_pkt_send       npd_syslog_pkt_send
#define syslog_ax_main_warning        npd_syslog_warning

/* syslog for dldp function*/
#define syslog_ax_dldp_err            npd_syslog_err
#define syslog_ax_dldp_dbg            npd_syslog_dbg
#define syslog_ax_dldp_event          npd_syslog_event
#define syslog_ax_dldp_pkt_rev        npd_syslog_pkt_rev
#define syslog_ax_dldp_pkt_send       npd_syslog_pkt_send
#define syslog_ax_dldp_warning        npd_syslog_warning

/* syslog for dhcp-snooping function*/
#define syslog_ax_dhcp_snp_err        npd_syslog_err
#define syslog_ax_dhcp_snp_dbg        npd_syslog_dbg
#define syslog_ax_dhcp_snp_event      npd_syslog_event
#define syslog_ax_dhcp_snp_pkt_rev    npd_syslog_pkt_rev
#define syslog_ax_dhcp_snp_pkt_send   npd_syslog_pkt_send
#define syslog_ax_dhcp_snp_warning    npd_syslog_warning

/* syslog for dbm function*/
#define syslog_ax_dbm_err            npd_syslog_err
#define syslog_ax_dbm_dbg            npd_syslog_dbg
#define syslog_ax_dbm_event          npd_syslog_event
#define syslog_ax_dbm_pkt_rev        npd_syslog_pkt_rev
#define syslog_ax_dbm_pkt_send       npd_syslog_pkt_send
#define syslog_ax_dbm_warning        npd_syslog_warning

/* syslog for dccnetlink function*/
#define dccn_syslog_err            npd_syslog_err
#define dccn_syslog_dbg            npd_syslog_dbg
#define dccn_syslog_event          npd_syslog_event
#define dccn_syslog_rev			   npd_syslog_pkt_rev	
#define dccn_syslog_pkt_send       npd_syslog_pkt_send
#define dccn_syslog_warning        npd_syslog_warning

/* syslog for board function*/
#define syslog_ax_board_err            npd_syslog_err
#define syslog_ax_board_dbg            npd_syslog_dbg
#define syslog_ax_board_event          npd_syslog_event
#define syslog_ax_board_pkt_rev        npd_syslog_pkt_rev
#define syslog_ax_board_pkt_send       npd_syslog_pkt_send
#define syslog_ax_board_warning        npd_syslog_warning
/* syslog for important log, on distributed system  */
#define syslog_ax_netlink_info           npd_syslog_info


enum {
	NPD_LOGPRI_TRACE = (1 << 0),   /**< function call sequences */
	NPD_LOGPRI_DEBUG = (1 << 1),   /**< debug statements in code */
	NPD_LOGPRI_INFO = (1 << 2),    /**< informational level */
	NPD_LOGPRI_WARNING = (1 << 3), /**< warnings */
	NPD_LOGPRI_ERROR = (1 << 4)    /**< error */
};

void logger_setup (int priority, const char *file, int line, const char *function);

void logger_emit (const char *format, ...);
/*void logger_forward_debug (const char *format, va_list args);*/
void logger_forward_debug (const char *format, ...);

void logger_enable (void);
void logger_disable (void);

void logger_enable_syslog (void);
void logger_disable_syslog (void);

void setup_logger (void);

#ifdef __SUNPRO_C
#define __FUNCTION__ __func__
#endif

/** Trace logging macro */
#define NPD_TRACE(expr)   do {logger_setup(NPD_LOGPRI_TRACE,   __FILE__, __LINE__, __FUNCTION__); logger_emit expr; } while(0)

/** Debug information logging macro */
#define NPD_DEBUG(expr)   do {logger_setup(NPD_LOGPRI_DEBUG,   __FILE__, __LINE__, __FUNCTION__); logger_emit expr; } while(0)

/** Information level logging macro */
#define NPD_INFO(expr)    do {logger_setup(NPD_LOGPRI_INFO,    __FILE__, __LINE__, __FUNCTION__); logger_emit expr; } while(0)

/** Warning level logging macro */
#define NPD_WARNING(expr) do {logger_setup(NPD_LOGPRI_WARNING, __FILE__, __LINE__, __FUNCTION__); logger_emit expr; } while(0)

/** Error leve logging macro */
#define NPD_ERROR(expr)   do {logger_setup(NPD_LOGPRI_ERROR,   __FILE__, __LINE__, __FUNCTION__); logger_emit expr; } while(0)

/** Macro for terminating the program on an unrecoverable error */


#define DIE(expr) do {printf("*** [DIE] %s:%s():%d : ", __FILE__, __FUNCTION__, __LINE__); printf expr; printf("\n"); exit(1); } while(0)

/** @} */

extern int nam_log_set_debug_value(unsigned int val_mask);



extern int nam_log_set_no_debug_value(unsigned int val_mask);



extern int nbm_log_set_debug_value(unsigned int val_mask);



extern int nbm_log_set_no_debug_value(unsigned int val_mask);

int npd_asic_debug_set
(
	unsigned int level
);

int npd_asic_debug_clr
(
	unsigned int level
);

DBusMessage * npd_system_debug_enable
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_system_debug_disable
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);
DBusMessage * nbm_dbus_interface_syslog_debug
( 
   DBusConnection *conn, 
   DBusMessage *msg, 
   void *user_data
);
DBusMessage * nbm_dbus_interface_syslog_debug
( 
   DBusConnection *conn, 
   DBusMessage *msg, 
   void *user_data
);
DBusMessage * nam_dbus_interface_syslog_debug
( 
   DBusConnection *conn, 
   DBusMessage *msg, 
   void *user_data
);

DBusMessage * nam_dbus_interface_syslog_debug
( 
   DBusConnection *conn, 
   DBusMessage *msg, 
   void *user_data
);

#endif				/* LOGGER_H */
