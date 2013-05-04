#ifndef __NBM_LOG_H__
#define __NBM_LOG_H__

/**
 * @addtogroup HalDaemonLogging
 *
 * @{
 */


/** Logging levels for NPD daemon
 */

#define NBM_OK              0
#define NBM_ERR             (NBM_OK + 1)

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
	SYSLOG_DBG_DEF = (SYSLOG_DBG_WAR|SYSLOG_DBG_ERR|SYSLOG_DBG_EVT),	/* default value ,error,warning and event*/
	SYSLOG_DBG_ALL = 0xFF	/* all*/
};

extern unsigned int nbm_log_level ;
extern unsigned int npd_log_level;
extern unsigned int npd_log_close;
extern char * npd_log_ident;

int nbm_log_set_debug_value(unsigned int val_mask);
int nbm_log_set_no_debug_value(unsigned int val_mask);
/**********************************************************************************
 *	nbm_log_status_get
 * 
 *  DESCRIPTION:
 *	This function get nbm debug level
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
unsigned int nbm_log_status_get
(
	void
);

/**********************************************************************************
 *	nbm_log_level_set
 * 
 *  DESCRIPTION:
 *	This function set up nbm debug level
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
int nbm_log_level_set
(
	unsigned int level
);

/**********************************************************************************
 *	nbm_syslog_emit
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
void nbm_syslog_emit
(
	int priority,
	char *buf
);

/**********************************************************************************
 *	nbm_syslog_dbg
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
void nbm_syslog_dbg
(
	char *format,
	...
);

/**********************************************************************************
 *	nbm_syslog_err
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
void nbm_syslog_err
(
	char *format,
	...
);

/**********************************************************************************
 *	nbm_syslog_err
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
void nbm_syslog_pkt
(
	char *format,
	...
);

/**********************************************************************************
 *	nbm_syslog_warning
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
void nbm_syslog_warning
(
	char *format,
	...
);

/**********************************************************************************
 *	nbm_syslog_event
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
void nbm_syslog_event
(
	char *format,
	...
);

/* syslog for nbm cpld*/
#define syslog_ax_cpld_err     nbm_syslog_err 
#define syslog_ax_cpld_dbg     nbm_syslog_dbg 
#define syslog_ax_cpld_event   nbm_syslog_event
#define syslog_ax_cpld_warning nbm_syslog_warning
#define syslog_ax_cpld_pkt     nbm_syslog_pkt


/* syslog for nbm utilus*/
#define syslog_ax_util_err     nbm_syslog_err 
#define syslog_ax_util_dbg     nbm_syslog_dbg 
#define syslog_ax_util_event   nbm_syslog_event
#define syslog_ax_util_warning nbm_syslog_warning
#define syslog_ax_util_pkt     nbm_syslog_pkt


/* syslog for nbm eeprom*/
#define syslog_ax_eeprom_err   nbm_syslog_err 
#define syslog_ax_eeprom_dbg   nbm_syslog_dbg 
#define syslog_ax_eeprom_event nbm_syslog_event
#define syslog_ax_eeprom_warning nbm_syslog_warning
#define syslog_ax_eeprom_pkt   nbm_syslog_pkt


/* syslog for nbm eth-port*/
#define syslog_ax_nbm_eth_port_err   nbm_syslog_err 
#define syslog_ax_nbm_eth_port_dbg   nbm_syslog_dbg 
#define syslog_ax_nbm_eth_port_event nbm_syslog_event
#define syslog_ax_nbm_eth_port_warning nbm_syslog_warning
#define syslog_ax_nbm_eth_port_pkt   nbm_syslog_pkt

#endif				/* LOGGER_H */
