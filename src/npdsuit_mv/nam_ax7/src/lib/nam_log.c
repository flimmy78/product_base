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
* nam_log.c
*
*
* CREATOR:
*		chenb@autelan.com
*
* DESCRIPTION:
*		APIs used in NAM for logging process.
*
* DATE:
*		06/12/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.17 $	
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
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "nam_log.h"
#include "sysdef/npd_sysdef.h"
#include "util/npd_list.h"
#include "npd/nam/npd_amapi.h"
#include "nam_asic.h"



/**
 * @defgroup NetworkPlatformDaemon Logging system
 * @ingroup NAMaemon
 * @brief Logging system for the NAM daemon
 * @{
 */


unsigned int nam_log_level = SYSLOG_DBG_DEF;
unsigned int nam_log_close = 1;



/* NPD startup state*/
extern int npd_startup_end;

/* NPD startup bootlog file descriptor*/
extern int	npd_bootlog_fd;

/* file to save system startup log */
#define NPD_SYSTEM_STARTUP_LOG_PATH	"/var/run/bootlog.npd"

/**********************************************************************************
 *	nam_log_set_debug_value
 * 
 *  DESCRIPTION:
 *	This function set up one nam debug level
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
int nam_log_set_debug_value
(
	unsigned int val_mask
)
{
	/*if(nam_log_level & val_mask) { this cause if any bit of npd_log_level is set ,we can't set flag to all success
		return 1;
	}*/
	
	nam_log_level |= val_mask;
	return 0;
}

/**********************************************************************************
 *	nam_log_set_no_debug_value
 * 
 *  DESCRIPTION:
 *	This function remove one nam debug level
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
int nam_log_set_no_debug_value(unsigned int val_mask)
{
	if(nam_log_level & val_mask) {
		nam_log_level &= ~val_mask;
		return 0;
	}
	else {		
		return 1;
	}
}

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
)
{
	return nam_log_level;
}

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
)
{	
	nam_log_level = level;
	return 0;
}

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
	char * buf
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

	/*write log*/
	nam_syslog_emit(LOG_DEBUG,buf);

	return;	
}

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

	/*write log*/
	nam_syslog_emit(LOG_ERR,buf);

	return;	
}


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
	nam_syslog_emit(LOG_WARNING,buf);

	return;	
}

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

	/*write log*/
	nam_syslog_emit(LOG_INFO,buf);

	return;	
}

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

	/*write log*/
	nam_syslog_emit(LOG_DEBUG,buf);

	return;	
}

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

	/*write log*/
	nam_syslog_emit(LOG_DEBUG,buf);

	return;	
}

#ifdef __cplusplus
}
#endif
