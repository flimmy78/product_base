#ifndef __NAM_LOG_H__
#define __NAM_LOG_H__

/**
 * @addtogroup HalDaemonLogging
 *
 * @{
 */


/** Logging levels for NPD daemon
 */

#define NAM_OK              0
#define NAM_ERR             (NAM_OK + 1)

#define NAM_FALSE           0
#define NAM_TRUE            1

#define SYSLOG_LOG_EMERG    LOG_EMERG
#define SYSLOG_LOG_ALERT    LOG_ALERT
#define SYSLOG_LOG_CRIT     LOG_CRIT
#define SYSLOG_LOG_ERR      LOG_ERR
#define SYSLOG_LOG_WARNING  LOG_WARNING
#define SYSLOG_LOG_NOTICE   LOG_NOTICE
#define SYSLOG_LOG_INFO     LOG_INFO

enum {
	SYSLOG_DBG_DBG = 0x1,	/*normal */
	SYSLOG_DBG_WAR = 0x2,	/*warning*/
	SYSLOG_DBG_ERR = 0x4,	/* error*/
	SYSLOG_DBG_EVT = 0x8,	/* event*/
	SYSLOG_DBG_PKT_REV = 0x10,	/*packet receive*/
	SYSLOG_DBG_PKT_SED = 0x20,	/*packet send*/
	SYSLOG_DBG_PKT_ALL = 0x30,	/*packet send and receive*/
	SYSLOG_DBG_DEF = (SYSLOG_DBG_WAR|SYSLOG_DBG_ERR|SYSLOG_DBG_EVT),	/* default value ,error,warning and event*/
	SYSLOG_DBG_ALL = 0xFF	/* all*/
};

extern unsigned int nam_log_level ;
extern unsigned int npd_log_level;
extern unsigned int npd_log_close;
extern char * npd_log_ident;

/* NPD startup bootlog file descriptor*/
extern int npd_bootlog_fd;

int nam_log_set_debug_value(unsigned int val_mask);
int nam_log_set_no_debug_value(unsigned int val_mask);
/**********************************************************************************
 *	nam_log_status_get
 * 
 *  DESCRIPTION:
 *	This function get nam debug level
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
unsigned int nam_log_status_get
(
	void
);

/**********************************************************************************
 *	nam_log_level_set
 * 
 *  DESCRIPTION:
 *	This function set up nam debug level
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
int nam_log_level_set
(
	unsigned int level
);

/**********************************************************************************
 *	nam_syslog_emit
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
void nam_syslog_emit
(
	int priority,
	char *buf
);

/**********************************************************************************
 *	nam_syslog_dbg
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
void nam_syslog_dbg
(
	char *format,
	...
);

/**********************************************************************************
 *	nam_syslog_err
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
void nam_syslog_err
(
	char *format,
	...
);

/**********************************************************************************
 *	nam_syslog_warning
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
void nam_syslog_warning
(
	char *format,
	...
);

/**********************************************************************************
 *	nam_syslog_event
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
void nam_syslog_event
(
	char *format,
	...
);

/**********************************************************************************
 *	nam_syslog_pkt_rx
 * 
 *	output the daemon debug info to /var/log/daemon.log
 *
 *  INPUT:
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
void nam_syslog_pkt_rx
(
	char *format,
	...
);

/**********************************************************************************
 *	nam_syslog_pkt_tx
 * 
 *	output the daemon debug info to /var/log/daemon.log
 *
 *  INPUT:
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
void nam_syslog_pkt_tx
(
	char *format,
	...
);

/* syslog for nam trunk*/
#define syslog_ax_nam_trunk_err	    nam_syslog_err
#define syslog_ax_nam_trunk_dbg	    nam_syslog_dbg
#define syslog_ax_nam_trunk_event	nam_syslog_event
#define syslog_ax_nam_trunk_pkt	    nam_syslog_pkt
#define syslog_ax_nam_trunk_warning	nam_syslog_warning


/* syslog for nam acl*/
#define syslog_ax_nam_acl_err       nam_syslog_err
#define syslog_ax_nam_acl_dbg       nam_syslog_dbg
#define syslog_ax_nam_acl_event     nam_syslog_event
#define syslog_ax_nam_acl_pkt       nam_syslog_pkt
#define syslog_ax_nam_acl_warning   nam_syslog_warning


/* syslog for nam arp snooping*/
#define syslog_ax_nam_arp_err       nam_syslog_err
#define syslog_ax_nam_arp_dbg       nam_syslog_dbg
#define syslog_ax_nam_arp_event     nam_syslog_event
#define syslog_ax_nam_arp_pkt       nam_syslog_pkt
#define syslog_ax_nam_arp_warning   nam_syslog_warning


/* syslog for nam asic*/
#define syslog_ax_nam_asic_err      nam_syslog_err
#define syslog_ax_nam_asic_dbg      nam_syslog_dbg 
#define syslog_ax_nam_asic_event    nam_syslog_event
#define syslog_ax_nam_asic_pkt      nam_syslog_pkt
#define syslog_ax_nam_asic_warning  nam_syslog_warning


/* syslog for nam eth-port*/
#define syslog_ax_nam_eth_err       nam_syslog_err
#define syslog_ax_nam_eth_dbg       nam_syslog_dbg 
#define syslog_ax_nam_eth_event     nam_syslog_event
#define syslog_ax_nam_eth_pkt       nam_syslog_pkt
#define syslog_ax_nam_eth_warning   nam_syslog_warning


/* syslog for nam fdb*/
#define syslog_ax_nam_fdb_err       nam_syslog_err
#define syslog_ax_nam_fdb_dbg       nam_syslog_dbg 
#define syslog_ax_nam_fdb_event     nam_syslog_event
#define syslog_ax_nam_fdb_warning   nam_syslog_warning
#define syslog_ax_nam_fdb_pkt       nam_syslog_pkt


/* syslog for nam interface*/
#define syslog_ax_nam_intf_err      nam_syslog_err
#define syslog_ax_nam_intf_dbg      nam_syslog_dbg
#define syslog_ax_nam_intf_event    nam_syslog_event
#define syslog_ax_nam_intf_pkt      nam_syslog_pkt
#define syslog_ax_nam_intf_warning  nam_syslog_warning


/* syslog for nam private vlan*/
#define syslog_ax_nam_pvlan_err     nam_syslog_err
#define syslog_ax_nam_pvlan_dbg     nam_syslog_dbg
#define syslog_ax_nam_pvlan_event   nam_syslog_event
#define syslog_ax_nam_pvlan_pkt     nam_syslog_pkt
#define syslog_ax_nam_pvlan_warning nam_syslog_warning


/* syslog for nam vlan*/
#define syslog_ax_nam_vlan_err      nam_syslog_err
#define syslog_ax_nam_vlan_dbg      nam_syslog_dbg
#define syslog_ax_nam_vlan_event    nam_syslog_event
#define syslog_ax_nam_vlan_pkt      nam_syslog_pkt
#define syslog_ax_nam_vlan_warning  nam_syslog_warning


/* syslog for nam mirror*/
#define syslog_ax_nam_mirror_err    nam_syslog_err
#define syslog_ax_nam_mirror_dbg    nam_syslog_dbg
#define syslog_ax_nam_mirror_event  nam_syslog_event
#define syslog_ax_nam_mirror_pkt    nam_syslog_pkt
#define syslog_ax_nam_mirror_warning nam_syslog_warning


#endif				/* LOGGER_H */
