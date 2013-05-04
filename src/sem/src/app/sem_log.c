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
#include <time.h>

#include "sem_log.h"
#include <dbus/dbus.h>

/* SEM startup bootlog file descriptor*/
int	sem_bootlog_fd = -1;

unsigned int sem_log_close = 1;

char *sem_log_ident = "sem";

static int priority;
static const char *file;
static int line;
static const char *function;

static int log_pid  = 0;
static int is_enabled = 1;
static int syslog_enabled = 0;

unsigned int sem_log_level = SYSLOG_DBG_DEF;

/* SEM startup state*/
int sem_startup_end = 0;


/* file to save system startup log */
//#define SEM_SYSTEM_STARTUP_LOG_PATH	"/var/run/bootlog.sem"


char *sem_get_current_time(void)
{
	
	time_t now;
	struct tm *timenow;
	
	time(&now);
	timenow = localtime(&now);
	
	return asctime(timenow);
}

/**********************************************************************************
 *	sem_syslog_emit
 * 
 *	output the log info to /var/log/bootlog.sem or startup log file SEM_SYSTEM_STARTUP_LOG_PATH
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
void sem_syslog_emit
(
	int priority,
	char *buf
)
{
	if(!buf)
		return;

	/* bootlog is output to different file*/
	/* check if system already startup*/
	if(sem_bootlog_fd < 0) {
		sem_bootlog_fd = open(SEM_SYSTEM_STARTUP_LOG_PATH, O_RDWR|O_CREAT|O_APPEND,0666);
	}
	
	if(sem_bootlog_fd > 0){
		if(0 == sem_startup_end){
			write(sem_bootlog_fd, buf, strlen(buf));
			if (priority == LOG_WARNING||priority == LOG_ERR||priority == LOG_NOTICE)
			{
				syslog(priority, buf);
			}
			return;
		}
	}
	/* write log*/
	syslog(priority,buf);
	return; 
}



/**********************************************************************************
 *	sem_syslog_dbg
 * 
 *	output the daemon debug info to /var/log/bootlog.sem
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
void sem_syslog_dbg
(
	char *format,
	...
)
{
	va_list ptr;
	char *time_ptr;
	char *temp_ptr;
	static char buf[SEM_SYSLOG_BUFFER_SIZE] = {0};

	/* event not triggered*/
	if(0 == (SYSLOG_DBG_DBG & sem_log_level)) {
		return;
	}

	/* ident null or format message null*/
	if(!format) {
		return;
	}

	time_ptr = sem_get_current_time();
	strcpy(buf, time_ptr);
	temp_ptr = (char *)(buf + strlen(time_ptr) -1);
	*temp_ptr = ' ';
	temp_ptr++;
	
	/* buffering log*/
	va_start(ptr, format);
	vsprintf(temp_ptr,format,ptr);
	va_end(ptr);

	/* assure log file open only once globally*/
	if(sem_log_close) {
		openlog(sem_log_ident, 0, LOG_DAEMON);
		sem_log_close = 0;
	}

	sem_syslog_emit(LOG_DEBUG, buf);

	return;	
}





/**********************************************************************************
 *	sem_syslog_err
 * 
 *	output the daemon debug info to /var/log/bootlog.sem
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
void sem_syslog_err
(
	char *format,
	...
)
{
	char *time_ptr;
	char *temp_ptr;
	va_list ptr;
	static char buf[SEM_SYSLOG_BUFFER_SIZE] = {0};

	/* event not triggered*/
	if(0 == (SYSLOG_DBG_ERR& sem_log_level)) {
		return;
	}

	/* ident null or format message null*/
	if(!format) {
		return;
	}
	
	time_ptr = sem_get_current_time();
	strcpy(buf, time_ptr);
	temp_ptr = (char *)(buf + strlen(time_ptr) -1);
	*temp_ptr = ' ';
	temp_ptr++;
	
	/* buffering log*/
	va_start(ptr, format);
	vsprintf(temp_ptr,format,ptr);
	va_end(ptr);

	/* assure log file open only once globally*/
	if(sem_log_close) {
		openlog(sem_log_ident, 0, LOG_DAEMON);
		sem_log_close = 0;
	}
	
    /*write log*/
	sem_syslog_emit(LOG_ERR, buf);

	return;	
}

/**********************************************************************************
 *	sem_syslog_pkt_send
 * 
 *	output the daemon debug info to /var/log/bootlog.sem
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
void sem_syslog_pkt_send
(
	char *format,
	...
)
{
	va_list ptr;
	static char buf[SEM_SYSLOG_BUFFER_SIZE] = {0};

	/* event not triggered*/
	if(0 == (SYSLOG_DBG_PKT_SED & sem_log_level)) {
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
	if(sem_log_close) {
		openlog(sem_log_ident, 0, LOG_DAEMON);
		sem_log_close = 0;
	}

	/* write log*/
	sem_syslog_emit(LOG_DEBUG,buf);

	return;	
}
/**********************************************************************************
 *	sem_syslog_pkt_rev
 * 
 *	output the daemon debug info to /var/log/bootlog.sem
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
void sem_syslog_pkt_rev
(
	char *format,
	...
)
{
	va_list ptr;
	static char buf[SEM_SYSLOG_BUFFER_SIZE] = {0};

	/* event not triggered*/
	if(0 == (SYSLOG_DBG_PKT_REV & sem_log_level)) {
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
	if(sem_log_close) {
		openlog(sem_log_ident, 0, LOG_DAEMON);
		sem_log_close = 0;
	}

	/* write log*/
	sem_syslog_emit(LOG_DEBUG,buf);

	return;	
}


void sem_syslog7_notice
(
	char *format,
	...
)
{
	va_list ptr;
	char *time_ptr;
	char *temp_ptr;
	static char buf[SEM_SYSLOG_BUFFER_SIZE] = {0};

	/* ident null or format message null*/
	if(!format) {
		return;
	}

	time_ptr = sem_get_current_time();
	strcpy(buf, time_ptr);
	temp_ptr = (char *)(buf + strlen(time_ptr) -1);
	*temp_ptr = ' ';
	temp_ptr++;
	
	/* buffering log*/
	va_start(ptr, format);
	vsprintf(temp_ptr,format,ptr);
	va_end(ptr);

	/* assure log file open only once globally*/
	if(sem_log_close) {
		openlog(sem_log_ident, 0, LOG_DAEMON);
		sem_log_close = 0;
	}

	/* write log*/
	syslog(LOG_LOCAL7 | LOG_NOTICE, buf);

	return;	
}



/**********************************************************************************
 *	sem_syslog_warning
 * 
 *	output the daemon debug info to console
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
void sem_syslog_warning
(
	char *format,
	...
)
{
	va_list ptr;
	char *time_ptr;
	char *temp_ptr;
	static char buf[SEM_SYSLOG_BUFFER_SIZE] = {0};

	/* event not triggered*/
	if(0 == (SYSLOG_DBG_WAR & sem_log_level)) {
		return;
	}

	/* ident null or format message null*/
	if(!format) {
		return;
	}

	time_ptr = sem_get_current_time();
	strcpy(buf, time_ptr);
	temp_ptr = (char *)(buf + strlen(time_ptr) -1);
	*temp_ptr = ' ';
	temp_ptr++;

	/* buffering log*/
	va_start(ptr, format);
	vsprintf(temp_ptr,format,ptr);
	va_end(ptr);

	/* assure log file open only once globally*/
	if(sem_log_close) {
		openlog(sem_log_ident, 0, LOG_DAEMON);
		sem_log_close = 0;
	}

	/*write log*/
	sem_syslog_emit(LOG_WARNING,buf);

	return;	
}

/**********************************************************************************
 *	sem_syslog_event
 * 
 *	output the daemon debug info to /var/run/bootlog.sem
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
void sem_syslog_event
(
	char *format,
	...
)
{
	va_list ptr;
	char *time_ptr;
	char *temp_ptr;
	static char buf[SEM_SYSLOG_BUFFER_SIZE] = {0};

	/* event not triggered*/
	if(0 == (SYSLOG_DBG_EVT & sem_log_level)) {
		return;
	}

	/* ident null or format message null*/
	if(!format) {
		return;
	}

	time_ptr = sem_get_current_time();
	strcpy(buf, time_ptr);
	temp_ptr = (char *)(buf + strlen(time_ptr) -1);
	*temp_ptr = ' ';
	temp_ptr++;

	/* buffering log*/
	va_start(ptr, format);
	vsprintf(temp_ptr,format,ptr);
	va_end(ptr);

	/* assure log file open only once globally*/
	if(sem_log_close) {
		openlog(sem_log_ident, 0, LOG_DAEMON);
		sem_log_close = 0;
	}

	/* write log*/
	sem_syslog_emit(LOG_NOTICE,buf);

	return;	
}

/**********************************************************************************
 *	sem_syslog_info
 * 
 *	output the daemon debug info to /var/run/bootlog.sem
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
void sem_syslog_info
(
	char *format,
	...
)
{
	va_list ptr;
	char *time_ptr;
	char *temp_ptr;
	static char buf[SEM_SYSLOG_BUFFER_SIZE] = {0};

	/* event not triggered*/
	if(0 == (SYSLOG_DBG_INFO & sem_log_level)) {
		return;
	}

	/* ident null or format message null*/
	if(!format) {
		return;
	}

	time_ptr = sem_get_current_time();
	strcpy(buf, time_ptr);
	temp_ptr = (char *)(buf + strlen(time_ptr) -1);
	*temp_ptr = ' ';
	temp_ptr++;

	/* buffering log*/
	va_start(ptr, format);
	vsprintf(temp_ptr,format,ptr);
	va_end(ptr);

	/* assure log file open only once globally*/
	if(sem_log_close) {
		openlog(sem_log_ident, 0, LOG_DAEMON);
		sem_log_close = 0;
	}

	/* write log*/
	sem_syslog_emit(LOG_INFO,buf);

	return;	
}


/**********************************************************************************
 *	sem_syslog_dbg
 * 
 *	output the daemon debug info to /var/log/bootlog.sem
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
void sem_syslog_dbg_without_time
(
	char *format,
	...
)
{
	va_list ptr;
	static char buf[SEM_SYSLOG_BUFFER_SIZE] = {0};

	/* event not triggered*/
	if(0 == (SYSLOG_DBG_DBG & sem_log_level)) {
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
	if(sem_log_close) {
		openlog(sem_log_ident, 0, LOG_DAEMON);
		sem_log_close = 0;
	}

	sem_syslog_emit(LOG_DEBUG, buf);

	return;	
}

/**********************************************************************************
 *	sem_log_set_debug_value
 * 
 *  DESCRIPTION:
 *	This function set up one sem debug level
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
int sem_log_set_debug_value(unsigned int val_mask)
{
	sem_log_level |= val_mask;
	return 0;
}

DBusMessage * sem_system_debug_enable(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply = NULL;
	DBusError err;
	unsigned int ret =SEM_ERR;
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
	
	/* here call sem api */
	if( !(sem_log_set_debug_value(flag))){
		if(sem_log_close){/*if not opened ,open it*/
			openlog(sem_log_ident, 0, LOG_DAEMON);
			sem_log_close = 0;
		}
		ret = SEM_OK;
	} 

	//sem_syslog_dbg("sem_log_level = 0x%x\n",sem_log_level);
	
	reply = dbus_message_new_method_return(msg);
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&flag,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	return reply;
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
int sem_log_set_no_debug_value(unsigned int val_mask)
{
	if(sem_log_level & val_mask) {
		sem_log_level &= ~val_mask;
		return 0;
	}
	else {
		return 1;
	}
}
DBusMessage * sem_system_debug_disable(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusError err;
	unsigned int ret =SEM_ERR ;
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
	
	/* here call npd api */
	if( !(sem_log_set_no_debug_value(flag))){
		if(!sem_log_level){ /* close only after all debug level is disabled*/
			sem_log_close = 1;
			closelog();
		}
		ret = SEM_OK;
	} 
	
	sem_syslog_dbg("sem_log_level = 0x%x\n",sem_log_level);
	
	reply = dbus_message_new_method_return(msg);
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&flag,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	
	return reply;
}


#ifdef __cplusplus
}
#endif
