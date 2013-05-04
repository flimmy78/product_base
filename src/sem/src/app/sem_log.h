#ifndef __SEM_LOG_H_
#define __SEM_LOG_H_

#define SEM_OK              0
#define SEM_ERR             (SEM_OK + 1)

/*sem log*/
#define SEM_SYSLOG_BUFFER_SIZE		(256)

#define SEM_SYSTEM_STARTUP_LOG_PATH "/var/run/bootlog.sem"

enum {
	SYSLOG_DBG_DBG     = 0x1,	/*normal */
	SYSLOG_DBG_WAR     = 0x2,	/*warning*/
	SYSLOG_DBG_ERR     = 0x4,	/* error*/
	SYSLOG_DBG_EVT     = 0x8,	/* event*/
	SYSLOG_DBG_PKT_REV = 0x10,  /*packet receive*/
	SYSLOG_DBG_PKT_SED = 0x20,  /*packet send*/
	SYSLOG_DBG_PKT_ALL = 0x30,  /*packet send and receive*/
	SYSLOG_DBG_INFO    = 0x40,  /* info , for important thread of distributed system */
	//SYSLOG_DBG_DEF = SYSLOG_DBG_INFO,/*(SYSLOG_DBG_WAR|SYSLOG_DBG_ERR|SYSLOG_DBG_EVT)*/	/* default value ,error,warning and event*/
	SYSLOG_DBG_DEF = (SYSLOG_DBG_WAR|SYSLOG_DBG_ERR|SYSLOG_DBG_DBG),	/* default value ,error,warning and event*/
	SYSLOG_DBG_ALL = 0xFF	/* all*/
};



#endif
