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
* npd_log.c
*
*
* CREATOR:
*		chenb@autelan.com
*
* DESCRIPTION:
*		APIs used in NPD for logging process.
*
* DATE:
*		02/21/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.25 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <syslog.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dbus/dbus.h>

#include "sysdef/npd_sysdef.h"
#include "npd_log.h"


/**
 * @defgroup NetworkPlatformDaemon Logging system
 * @ingroup NPDaemon
 * @brief Logging system for the NPD daemon
 * @{
 */


static int priority;
static const char *file;
static int line;
static const char *function;

static int log_pid  = 0;
static int is_enabled = 1;
static int syslog_enabled = 0;
unsigned int npd_log_close = 1;

unsigned int npd_log_level = SYSLOG_DBG_DEF;
char * npd_log_ident = "npd";

/* NPD startup state*/
int npd_startup_end = 0;

/* NPD startup bootlog file descriptor*/
int	npd_bootlog_fd = -1;

/* file to save system startup log */
#define NPD_SYSTEM_STARTUP_LOG_PATH	"/var/run/bootlog.npd"

/**********************************************************************************
 *	npd_log_set_debug_value
 * 
 *  DESCRIPTION:
 *	This function set up one npd debug level
 *
 *  INPUT:
 * 	 	val_mask - debug level value stands for one level 
 *  
 *  OUTPUT:
 * 	 NULL
 *
 *  RETURN:
 * 	 1 - if debug level has already been set before.
 *	 0 - debug level setup successfully.
 *
 **********************************************************************************/
int npd_log_set_debug_value
(
	unsigned int val_mask
)
{
	/*if(npd_log_level & val_mask) { this cause if any bit of npd_log_level is set ,we can't set flag to all success
		return 1;
	}*/
	
	npd_log_level |= val_mask;
	return 0;
}

/**********************************************************************************
 *	npd_log_set_no_debug_value
 * 
 *  DESCRIPTION:
 *	This function remove one npd debug level
 *
 *  INPUT:
 * 	 	val_mask - debug level value stands for one level 
 *  
 *  OUTPUT:
 * 	 NULL
 *
 *  RETURN:
 * 	 1 - if debug level has not been set before.
 *	 0 - remove debug level successfully.
 *
 **********************************************************************************/
int npd_log_set_no_debug_value(unsigned int val_mask)
{
	if(npd_log_level & val_mask) {
		npd_log_level &= ~val_mask;
		return 0;
	}
	else {
		return 1;
	}
}

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
)
{
	return npd_log_level;
}

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
)
{	
	npd_log_level = level;
	return 0;
}

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
)
{
	if(!buf)
		return;

	/* bootlog is output to different file*/
	/* check if system already startup*/
	if(npd_bootlog_fd < 0) {
		npd_bootlog_fd = open(NPD_SYSTEM_STARTUP_LOG_PATH, O_RDWR|O_CREAT|O_APPEND,0666);
	}
	
	if(npd_bootlog_fd > 0){
		if(0 == npd_startup_end){
			write(npd_bootlog_fd, buf, strlen(buf));
			return;
		}
	}
	/* write log*/
	syslog(priority,buf);
	return; 
}

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
)
{
	va_list ptr;
	static char buf[NPD_SYSLOG_BUFFER_SIZE] = {0};

	/* event not triggered*/
	if(0 == (SYSLOG_DBG_DBG & npd_log_level)) {
		return;
	}

	/* ident null or format message null*/
	if(!format) {
		return;
	}
	
	/* buffering log*/
	va_start(ptr, format);
	vsprintf(buf,format,ptr);
	va_end(ptr);

	/* assure log file open only once globally*/
	if(npd_log_close) {
		openlog(npd_log_ident, 0, LOG_DAEMON);
		npd_log_close = 0;
	}

	npd_syslog_emit(LOG_DEBUG,buf);

	return;	
}

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
)
{
	va_list ptr;
	static char buf[NPD_SYSLOG_BUFFER_SIZE] = {0};

	/* event not triggered*/
	if(0 == (SYSLOG_DBG_ERR & npd_log_level)) {
		return;
	}

	/* ident null or format message null*/
	if(!format) {
		return;
	}

	/* buffering log*/
	va_start(ptr, format);
	vsprintf(buf,format,ptr);
	va_end(ptr);

	/* assure log file open only once globally*/
	if(npd_log_close) {
		openlog(npd_log_ident, 0, LOG_DAEMON);
		npd_log_close = 0;
	}

	/* write log*/
	npd_syslog_emit(LOG_ERR,buf);

	return;	
}

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
)
{
	va_list ptr;
	static char buf[NPD_SYSLOG_BUFFER_SIZE] = {0};

	/* event not triggered*/
	if(0 == (SYSLOG_DBG_PKT_SED & npd_log_level)) {
		return;
	}

	/* ident null or format message null*/
	if(!format) {
		return;
	}

	/* buffering log*/
	va_start(ptr, format);
	vsprintf(buf,format,ptr);
	va_end(ptr);

	/* assure log file open only once globally*/
	if(npd_log_close) {
		openlog(npd_log_ident, 0, LOG_DAEMON);
		npd_log_close = 0;
	}

	/* write log*/
	npd_syslog_emit(LOG_DEBUG,buf);

	return;	
}
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
)
{
	va_list ptr;
	static char buf[NPD_SYSLOG_BUFFER_SIZE] = {0};

	/* event not triggered*/
	if(0 == (SYSLOG_DBG_PKT_REV & npd_log_level)) {
		return;
	}

	/* ident null or format message null*/
	if(!format) {
		return;
	}

	/* buffering log*/
	va_start(ptr, format);
	vsprintf(buf,format,ptr);
	va_end(ptr);

	/* assure log file open only once globally*/
	if(npd_log_close) {
		openlog(npd_log_ident, 0, LOG_DAEMON);
		npd_log_close = 0;
	}

	/* write log*/
	npd_syslog_emit(LOG_DEBUG,buf);

	return;	
}

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
)
{
	va_list ptr;
	static char buf[NPD_SYSLOG_BUFFER_SIZE] = {0};

	/* event not triggered*/
	if(0 == (SYSLOG_DBG_WAR & npd_log_level)) {
		return;
	}

	/* ident null or format message null*/
	if(!format) {
		return;
	}

	/* buffering log*/
	va_start(ptr, format);
	vsprintf(buf,format,ptr);
	va_end(ptr);

	/* assure log file open only once globally*/
	if(npd_log_close) {
		openlog(npd_log_ident, 0, LOG_DAEMON);
		npd_log_close = 0;
	}

	/*write log*/
	npd_syslog_emit(LOG_WARNING,buf);

	return;	
}

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
)
{
	va_list ptr;
	static char buf[NPD_SYSLOG_BUFFER_SIZE] = {0};

	/* event not triggered*/
	if(0 == (SYSLOG_DBG_EVT & npd_log_level)) {
		return;
	}

	/* ident null or format message null*/
	if(!format) {
		return;
	}

	/* buffering log*/
	va_start(ptr, format);
	vsprintf(buf,format,ptr);
	va_end(ptr);

	/* assure log file open only once globally*/
	if(npd_log_close) {
		openlog(npd_log_ident, 0, LOG_DAEMON);
		npd_log_close = 0;
	}

	/* write log*/
	npd_syslog_emit(LOG_INFO,buf);

	return;	
}

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
)
{
	va_list ptr;
	static char buf[NPD_SYSLOG_BUFFER_SIZE] = {0};

	/* event not triggered*/
	if(0 == (SYSLOG_DBG_INFO & npd_log_level)) {
		return;
	}

	/* ident null or format message null*/
	if(!format) {
		return;
	}

	/* buffering log*/
	va_start(ptr, format);
	vsprintf(buf,format,ptr);
	va_end(ptr);

	/* assure log file open only once globally*/
	if(npd_log_close) {
		openlog(npd_log_ident, 0, LOG_DAEMON);
		npd_log_close = 0;
	}

	/* write log*/
	npd_syslog_emit(LOG_INFO,buf);

	return;	
}


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
)
{
	va_list ptr;
	static char buf[NPD_SYSLOG_BUFFER_SIZE] = {0};

	/* ident null or format message null*/
	if(!format) {
		return;
	}
	
	/* buffering log*/
	va_start(ptr, format);
	vsprintf(buf,format,ptr);
	va_end(ptr);

	/* assure log file open only once globally*/
	if(npd_log_close) {
		openlog(npd_log_ident, 0, LOG_DAEMON);
		npd_log_close = 0;
	}

	/* write log*/
	syslog(LOG_LOCAL7 | LOG_NOTICE, buf);

	return;	
}
/** Disable all logging
 *
 */
void 
logger_disable (void)
{
	is_enabled = 0;
}

/** Enable all logging
 *
 */
void 
logger_enable (void)
{
	is_enabled = 1;
}

/** enable usage of syslog for logging  
 *
 */
void 
logger_enable_syslog (void)
{
	syslog_enabled = 1;
}

/** disable usage of syslog for logging  
 *
 */
void 
logger_disable_syslog (void)
{
	syslog_enabled = 0;
}

/** allow setup logger from a addon/prober via the env 
 *
 */
void
setup_logger (void)
{
	if ((getenv ("NPDD_VERBOSE")) != NULL) {
		is_enabled = 1;
		log_pid = 1;
	}
	else
		is_enabled = 0;

	if ((getenv ("NPDD_USE_SYSLOG")) != NULL)
		syslog_enabled = 1;
	else
		syslog_enabled = 0;
}

/** Setup logging entry
 *
 *  @param  priority            Logging priority, one of NPD_LOGPRI_*
 *  @param  file                Name of file where the log entry originated
 *  @param  line                Line number of file
 *  @param  function            Name of function
 */
void
logger_setup (int _priority, const char *_file, int _line, const char *_function)
{
	priority = _priority;
	file = _file;
	line = _line;
	function = _function;
}

/** Emit logging entry
 *
 *  @param  format              Message format string, printf style
 *  @param  ...                 Parameters for message, printf style
 */
void
logger_emit (const char *format, ...)
{
	va_list args;
	char buf[512];
	char *pri;
	char tbuf[256];
	char logmsg[1024];
	struct timeval tnow;
	struct tm *tlocaltime;
	struct tm tzone;
	static pid_t pid = -1;

	if (!is_enabled)
		return;

	va_start (args, format);
	vsnprintf (buf, sizeof (buf), format, args);

	switch (priority) {
		case NPD_LOGPRI_TRACE:
			pri = "[T]";
			break;
		case NPD_LOGPRI_DEBUG:
			pri = "[D]";
			break;
		case NPD_LOGPRI_INFO:
			pri = "[I]";
			break;
		case NPD_LOGPRI_WARNING:
			pri = "[W]";
			break;
			default:		/* explicit fallthrough */
		case NPD_LOGPRI_ERROR:
			pri = "[E]";
			break;
	}

	gettimeofday (&tnow, &tzone);
	tlocaltime = localtime (&tnow.tv_sec);
	strftime (tbuf, sizeof (tbuf), "%H:%M:%S", tlocaltime);

	if (log_pid) {
		if ((int) pid == -1)
			pid = getpid ();
		snprintf (logmsg, sizeof(logmsg), "[%d]: %s.%03d %s %s:%d: %s\n", pid, tbuf, (int)(tnow.tv_usec/1000), pri, file, line, buf);
	} else {
		snprintf (logmsg, sizeof(logmsg), "%s.%03d %s %s:%d: %s\n", tbuf, (int)(tnow.tv_usec/1000), pri, file, line, buf);
	}
		
	/** @todo Make programmatic interface to logging */
	if (priority != NPD_LOGPRI_TRACE && !syslog_enabled ) {
		fprintf (stderr, "%s", logmsg );
	} else if (priority != NPD_LOGPRI_TRACE && syslog_enabled ) {
		/* use syslog for debug/log messages if NPD started as daemon */
		switch (priority) {
			case NPD_LOGPRI_DEBUG:
			case NPD_LOGPRI_INFO:
				syslog(LOG_INFO, "%s", logmsg );
				break;
			case NPD_LOGPRI_WARNING:
				syslog(LOG_WARNING, "%s", logmsg );
				break;
				default:		 /* explicit fallthrough */
			case NPD_LOGPRI_ERROR:
				syslog(LOG_ERR, "%s", logmsg );
				break;
		}
	}

	va_end (args);
}

void
/*logger_forward_debug (const char *format, va_list args)*/
logger_forward_debug (const char *format, ...)
{
	va_list args;
	char buf[512];
	char tbuf[256];
	struct timeval tnow;
	struct tm *tlocaltime;
	struct tm tzone;
	static pid_t pid = -1;

	if (!is_enabled)
		return;

	if ((int) pid == -1)
		pid = getpid ();

	va_start (args, format);
	vsnprintf (buf, sizeof (buf), format, args);

	gettimeofday (&tnow, &tzone);
	tlocaltime = localtime (&tnow.tv_sec);
	strftime (tbuf, sizeof (tbuf), "%H:%M:%S", tlocaltime);

	if (syslog_enabled)
		syslog (LOG_INFO, "%d: %s.%03d: %s", pid, tbuf, (int)(tnow.tv_usec/1000), buf);
	else
		fprintf (stderr, "%d: %s.%03d: %s", pid, tbuf, (int)(tnow.tv_usec/1000), buf);

	va_end (args);
}

int npd_asic_debug_set
(
	unsigned int level
)
{
	return nam_asic_log_level_set(level);
}

int npd_asic_debug_clr
(
	unsigned int level
)
{
	return nam_asic_log_level_clr(level);
}

DBusMessage * npd_system_debug_enable(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusError err;
	unsigned int ret =NPD_ERR ;
	unsigned int flag = 0;
	
	dbus_error_init( &err );
	if(!(dbus_message_get_args(msg,&err,
								DBUS_TYPE_UINT32,&flag,
								DBUS_TYPE_INVALID)))
	{
		/*NPD_ERROR(("NPD>>Unable to get input args\n"));*/
		if(dbus_error_is_set( &err ))
		{
			/*NPD_ERROR(("NPD>>%s raised:%s\n",err.name ,err.message));*/
			dbus_error_free( &err );
		}
		return NULL;	
	}
	/* here call npd api */
	/*printf("%d received\n",flag);*/
	if( !(npd_log_set_debug_value(flag))){
		if(npd_log_close){/*if not opened ,open it*/
			openlog(npd_log_ident, 0, LOG_DAEMON); 
			npd_log_close = 0;
		}
		ret = NPD_OK;
	} 

	/*printf("%d received\n",flag);*/
	/*printf("%d is npd_log_level\n",npd_log_level);*/
	
	/*NPD_ERROR(("NPD>>Entering set npd debug!\n"));*/
	reply = dbus_message_new_method_return(msg);
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&flag,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
		
	/*dbus_message_iter_init_append (reply, &iter);
		
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,&flag);

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);*/
	

	/*printf("%d received\n",flag);*/
	
	
	return reply;
	
}

DBusMessage * npd_system_debug_disable(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusError err;
	unsigned int ret =NPD_ERR ;
	unsigned int flag = 0;
	
	dbus_error_init( &err );
	if(!(dbus_message_get_args(msg,&err,
								DBUS_TYPE_UINT32,&flag,
								DBUS_TYPE_INVALID)))
	{
		/*NPD_ERROR(("NPD>>Unable to get input args\n"));*/
		if(dbus_error_is_set( &err ))
		{
			/*NPD_ERROR(("NPD>>%s raised:%s\n",err.name ,err.message));*/
			dbus_error_free( &err );
		}
		return NULL;	
	}
	/* here call npd api */
	/*printf("%d received\n",flag);*/
	if( !(npd_log_set_no_debug_value(flag))){
		#if 0 		
		npd_log_close = 1;
		if((1 == npd_log_close)&&(1 == nam_log_close)&&(1 == nbm_log_close)){
			closelog();
		}
		#endif
		if(!npd_log_level){ /* close only after all debug level is disabled*/
			npd_log_close = 1;
			closelog();
		}
		ret = NPD_OK;
	} 
	/*NPD_ERROR(("NPD>>Entering set npd debug!\n"));*/
	reply = dbus_message_new_method_return(msg);
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&flag,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	
	return reply;
	
}

DBusMessage * nam_dbus_interface_syslog_debug(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	
	DBusMessage* reply;
	DBusError err;
	unsigned int ret =0 ;
	unsigned int flag = 0;
	
	dbus_error_init( &err );
	if(!(dbus_message_get_args(msg,&err,
					DBUS_TYPE_UINT32,&flag,
					DBUS_TYPE_INVALID)))
	{
		if(dbus_error_is_set( &err ))
		{
			dbus_error_free( &err );
		}
		return NULL;	
	}
	if( !(nam_log_set_debug_value(flag))){
		if(nam_log_close){
			openlog(npd_log_ident, 0, LOG_DAEMON);
			nam_log_close = 0;
		}
		ret = 1;
	} 
	
	reply = dbus_message_new_method_return(msg);
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&flag,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	return reply;
	
}

DBusMessage * nam_dbus_interface_syslog_no_debug(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusError err;
	unsigned int ret =0 ;
	unsigned int flag = 0;
	
	dbus_error_init( &err );
	if(!(dbus_message_get_args(msg,&err,
								DBUS_TYPE_UINT32,&flag,
								DBUS_TYPE_INVALID)))
	{
		if(dbus_error_is_set( &err ))
		{

			dbus_error_free( &err );
		}
		return NULL;	
	}

	/*printf("%d received\n",flag);*/
	if( !(nam_log_set_no_debug_value(flag))){
		nam_log_close = 1;
		if((1 == npd_log_close)&&(1 == nam_log_close)&&(1 == nbm_log_close)){
			closelog();
		}
		ret = 1;
	} 
	
	reply = dbus_message_new_method_return(msg);
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&flag,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	/*printf("%d received\n",flag);*/
	return reply;
	
}


DBusMessage * nbm_dbus_interface_syslog_debug(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusError err;
	unsigned int ret =0 ;
	unsigned int flag = 0;
	
	dbus_error_init( &err );
	if(!(dbus_message_get_args(msg,&err,
								DBUS_TYPE_UINT32,&flag,
								DBUS_TYPE_INVALID)))
	{
		if(dbus_error_is_set( &err ))
		{
			dbus_error_free( &err );
		}
		return NULL;	
	}

	/*printf("%d received\n",flag);*/
	if( !(nbm_log_set_debug_value(flag))){
		if(nbm_log_close){
			openlog(npd_log_ident, 0, LOG_DAEMON);
			nbm_log_close = 0;
		}
		ret = 1;
	} 
	
	reply = dbus_message_new_method_return(msg);
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&flag,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	
	return reply;
	
}

DBusMessage * nbm_dbus_interface_syslog_no_debug(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusError err;
	unsigned int ret =0 ;
	unsigned int flag = 0;
	
	dbus_error_init( &err );
	if(!(dbus_message_get_args(msg,&err,
								DBUS_TYPE_UINT32,&flag,
								DBUS_TYPE_INVALID)))
	{
		if(dbus_error_is_set( &err ))
		{

			dbus_error_free( &err );
		}
		return NULL;	
	}

	/*printf("%d received\n",flag);*/
	if( !(nbm_log_set_no_debug_value(flag))){
		nbm_log_close = 1;
		if((1 == npd_log_close)&&(1 == nam_log_close)&&(1 == nbm_log_close)){
			closelog();
		}
		ret = 1;
	} 
	
	reply = dbus_message_new_method_return(msg);
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&flag,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	
	return reply;
	
}

DBusMessage* npd_dbus_asic_syslog_debug(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusError err;
	unsigned int ret =0 ;
	unsigned int flag = 0;
	
	dbus_error_init( &err );
	if(!(dbus_message_get_args(msg,&err,
								DBUS_TYPE_UINT32,&flag,
								DBUS_TYPE_INVALID)))
	{
		if(dbus_error_is_set( &err ))
		{

			dbus_error_free( &err );
		}
		return NULL;	
	}

	npd_asic_debug_set(flag);
	
	reply = dbus_message_new_method_return(msg);
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	
	return reply;
}

DBusMessage* npd_dbus_asic_syslog_no_debug(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusError err;
	unsigned int ret =0 ;
	unsigned int flag = 0;
	
	dbus_error_init( &err );
	if(!(dbus_message_get_args(msg,&err,
								DBUS_TYPE_UINT32,&flag,
								DBUS_TYPE_INVALID)))
	{
		if(dbus_error_is_set( &err ))
		{

			dbus_error_free( &err );
		}
		return NULL;	
	}

	npd_asic_debug_clr(flag);
	
	reply = dbus_message_new_method_return(msg);
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	
	return reply;
}
	

#ifdef __cplusplus
}
#endif
