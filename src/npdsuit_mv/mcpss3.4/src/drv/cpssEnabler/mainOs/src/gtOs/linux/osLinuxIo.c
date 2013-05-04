/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* osLinuxIo.c
*
* DESCRIPTION:
*       Linux User Mode Operating System wrapper. Input/output facility.
*
* DEPENDENCIES:
*       Linux , CPU independed.
*
* FILE REVISION NUMBER:
*       $Revision: 1.3 $
*******************************************************************************/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>

#include <gtOs/gtOsIo.h>


enum {
	SYSLOG_DBG_DBG = 0x1,	/*normal */
	SYSLOG_DBG_WAR = 0x2,	/*warning */
	SYSLOG_DBG_ERR = 0x4,	/* error */
	SYSLOG_DBG_EVT = 0x8,	/* event */
	SYSLOG_DBG_PKT_REV = 0x10, /*pkt_rev*/
	SYSLOG_DBG_PKT_SED = 0x20, /*pkt_sed*/
	SYSLOG_DBG_PKT_ALL = 0x30, /*pkt_sed*/
	SYSLOG_DBG_DEF = (SYSLOG_DBG_WAR|SYSLOG_DBG_ERR|SYSLOG_DBG_EVT),	/* default value ,error,warning and event*/
	SYSLOG_DBG_ALL = 0xFF	/* all */
};

/*  syslog buffer size  - shared by npd/nam/nbm/cpss */
#define MV_SYSLOG_BUFFER_SIZE	(256)

unsigned int cpss_syslog_level = SYSLOG_DBG_DEF;
unsigned char cpss_log_open = 0;/* need open log or not */
extern unsigned int npd_log_level;
extern unsigned int npd_log_close;
extern char * npd_log_ident;
// NPD startup state
extern int npd_startup_end;

// NPD startup bootlog file descriptor
extern int	npd_bootlog_fd;

/* crit log file descriptor */
int crit_log_fd = -1;

/* file to save system startup log */
#define NPD_SYSTEM_STARTUP_LOG_PATH	"/var/run/bootlog.npd"

/* file to save system critical log */
#define CPSS_CRIT_LOGFILE	"/etc/motd"

extern int fsync(int fd);
extern int fileno(FILE * stream);

/************* Global Arguments ***********************************************/

GT_STATUS (*osPrintSyncUartBindFunc)(char *, GT_U32) = NULL;

/************* Static variables ***********************************************/
static OS_BIND_STDOUT_FUNC_PTR writeFunctionPtr = NULL;
static void* writeFunctionParam = NULL;

/************ Public Functions ************************************************/

/*******************************************************************************
* osBindStdOut
*
* DESCRIPTION:
*       Bind Stdout to handler
*
* INPUTS:
*       writeFunction - function to call for output
*       userPtr       - first parameter to pass to write function
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osBindStdOut(
    IN OS_BIND_STDOUT_FUNC_PTR writeFunction,
    IN void* userPtr
)
{
    writeFunctionPtr = writeFunction;
    writeFunctionParam = userPtr;
    return GT_OK;
}
#ifndef __AX_PLATFORM__
/**********************************************************************************
 *	cpss_syslog_emit
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
void cpss_syslog_emit
(
	int priority,
	char *buf
);

/*******************************************************************************
* osPrintf
*
* DESCRIPTION:
*       Write a formatted string to the standard output stream.
*
* INPUTS:
*       format  - format string to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The number of characters written, or a negative value if
*       an output error occurs.
*
* COMMENTS:
*       None
*
*******************************************************************************/
int osPrintf(const char* format, ...)
{
    char buff[MV_SYSLOG_BUFFER_SIZE] = {0};
    va_list args;
    int i, ret=0;
	static int needOpen = 1;
	static int fd = -1;
	

	if(SYSLOG_DBG_ALL != npd_log_level) {
		return 0;
	}

	if(npd_log_close) {
		openlog(npd_log_ident, 0, LOG_DAEMON);
		npd_log_close = 0;
	}
	
    va_start(args, format);
    i = vsprintf(buff, format, args);
    va_end(args);
	
	#ifdef AX_WRITE_LOG
	if(needOpen){
		fd = open("/dev/marvell.log",O_RDWR|O_CREAT|O_APPEND,0666);
		if(fd<0)
			printf("open /dev/marvell.log error.\n");
		needOpen = 0;
	}
	else if(fd<0){
		printf("fd %d error",fd);
		needOpen = 1;
	}
	else
		write(fd,buff,strlen(buff));
	#endif
	
	#ifdef AX_LOG_STDOUT
    ret = printf(buff);
    if (ret <= 0)
    {
        return ret;
    }
	#endif
	
	cpss_syslog_emit(LOG_DEBUG,buff);

#if 1
    {
        /* VVV TBD - should be checked if needed */
        fflush(stdout);
        ret = fsync(fileno(stdout));
		if(ret < 0)         /* code optmize: Unchecked return value. zhangdi@autelan.com 2013-01-17 */
		{
			return ret;
		}
    }
#endif
    return ret;
}
#else
/*******************************************************************************
* osPrintf
*
* DESCRIPTION:
*       Write a formatted string to the standard output stream.
*
* INPUTS:
*       format  - format string to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The number of characters written, or a negative value if
*       an output error occurs.
*
* COMMENTS:
*       None
*
*******************************************************************************/
int osPrintf(const char* format, ...)
{
    va_list args;
    int     ret;

    va_start(args, format);
    ret = osVprintf(format, args);
    va_end(args);

    return ret;
}
#endif
/*******************************************************************************
* osVsprintf
*
* DESCRIPTION:
*       Write a formatted string to the standard output stream.
*
* INPUTS:
*       format  - format string to write
*       ap      - va_list
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The number of characters written, or a negative value if
*       an output error occurs.
*
* COMMENTS:
*       None
*
*******************************************************************************/
int osVprintf(const char* format, va_list args)
{
    int ret;

    if (writeFunctionPtr != NULL)
    {
        static char buff[2048];

        ret = vsnprintf(buff, sizeof(buff), format, args);
        if (ret <= 0)
            return ret;

        return writeFunctionPtr(writeFunctionParam, buff, ret);
    }

    ret = vprintf(format, args);
    if (ret <= 0)
    {
        return ret;
    }

    /* VVV TBD - should be checked if needed */
    fflush(stdout);
    /* fsync(fileno(stdout)); */

    return ret;
}


/*******************************************************************************
* osSprintf
*
* DESCRIPTION:
*       Write a formatted string to a buffer.
*
* INPUTS:
*       buffer  - buffer to write to
*       format  - format string
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The number of characters copied to buffer, not including
*       the NULL terminator.
*
* COMMENTS:
*       None
*
*******************************************************************************/
int osSprintf(char * buffer, const char* format, ...)
{
    va_list args;
    int i;

    va_start(args, format);
    i = vsprintf(buffer, format, args);
    va_end(args);

    return i;
}

/*******************************************************************************
* osVsprintf
*
* DESCRIPTION:
*       Write a formatted string to a buffer.
*
* INPUTS:
*       buffer  - buffer to write to
*       format  - format string
*       args    - va_list
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The number of characters copied to buffer, not including
*       the NULL terminator.
*
* COMMENTS:
*       None
*
*******************************************************************************/
int osVsprintf(char * buffer, const char* format, va_list args)
{
    return vsprintf(buffer, format, args);
}

/*******************************************************************************
* osGets
*
* DESCRIPTION:
*       Reads characters from the standard input stream into the array
*       'buffer' until end-of-file is encountered or a new-line is read.
*       Any new-line character is discarded, and a null character is written
*       immediately after the last character read into the array.
*
* INPUTS:
*       buffer  - pointer to buffer to write to
*
* OUTPUTS:
*       buffer  - buffer with read data
*
* RETURNS:
*       A pointer to 'buffer', or a null pointer if end-of-file is
*       encountered and no characters have been read, or there is a read error.
*
* COMMENTS:
*       None
*
*******************************************************************************/
char * osGets(char * buffer)
{
     /* In Linux, linker returns:
      * "the `gets' function is dangerous and should not be used.
      * So we use fgets.
      */

     fgets(buffer, 1024, stdin);
     return buffer ;
}

/*******************************************************************************
* osPrintSync
*
* DESCRIPTION:
*       Write a formatted string to the standard output stream, in polling mode
*		The device driver will print the string in the same instance this function
*       is called, with no delay.
*
* INPUTS:
*       format  - format string to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The number of characters written, or a negative value if
*       an output error occurs.
*
* COMMENTS:
*       None
*
*******************************************************************************/
int osPrintSync(const char* format, ...)
{
    char buff[2048];
    va_list args;
    int i, retVal = 0;

    va_start(args, format);
    i = vsprintf(buff, format, args);
    va_end(args);

    if ( osPrintSyncUartBindFunc != NULL )
      retVal = (int)((osPrintSyncUartBindFunc)(buff, (GT_U32)strlen(buff)));
      
    return (retVal);
}

#ifndef __AX_PLATFORM__
/*******************************************************************************
* cpss_debug_level_set
*
* DESCRIPTION:
*      Setup syslog debug level
*
* INPUTS:
*       level  - syslog debug level
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - if setup syslog level successfully.
*	  GT_FAIL - if syslog level already setup.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpss_debug_level_set
(
	unsigned int level
)
{
    osPrintf("will make log level %02x -> %02x\n", cpss_syslog_level,level);
	if(SYSLOG_DBG_ALL == level) { /* set debug level SYSLOG_DBG_ALL */
		cpss_syslog_level |= level;
	}
	else if(cpss_syslog_level  & level) {
		return GT_FAIL;
	}
	else {
		cpss_syslog_level |= level;
	}

    osPrintf("update log level %02x -> %02x\n", cpss_syslog_level,level);
	return GT_OK;
}
/*******************************************************************************
* cpss_debug_level_clr
*
* DESCRIPTION:
*      Cancel syslog debug level
*
* INPUTS:
*       level  - syslog debug level
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - if setup syslog level successfully.
*	  GT_FAIL - if syslog level already setup.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpss_debug_level_clr
(
	unsigned int level
)
{
	if(SYSLOG_DBG_ALL == level) {/* set no debug level SYSLOG_DBG_ALL */
		cpss_syslog_level = 0;
		if(cpss_log_open) {
			/*closelog(); never closelog */
			cpss_log_open = 0;
		}		
	}
	else if(cpss_syslog_level  & level) {
		cpss_syslog_level &= ~level;
	}
	else {
		return GT_FAIL;
	}

	return GT_OK;
}

/*******************************************************************************
* cpss_debug_status_get
*
* DESCRIPTION:
*      Get currrent syslog debug level
*
* INPUTS:
*
* OUTPUTS:
*       None
*
* RETURNS:
*      driver syslog level value.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpss_debug_status_get
( 

)
{
	return cpss_syslog_level;
}

/*******************************************************************************
* cpss_debug_status_set
*
* DESCRIPTION:
*      Set currrent syslog debug level value
*
* INPUTS:
*	  level - syslog debug level value.
*
* OUTPUTS:
*       None
*
* RETURNS:
*      None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID cpss_debug_status_set
( 
	unsigned int level
)
{
	cpss_syslog_level = level;
	return;
}

/**********************************************************************************
 *	cpss_syslog_emit
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
void cpss_syslog_emit
(
	int priority,
	char *buf
)
{
	char state_buf[4] = {0};

	if(!buf)
		return;

	// bootlog is output to different file
	// check if system already startup
	if(npd_bootlog_fd < 0) {
		npd_bootlog_fd = open(NPD_SYSTEM_STARTUP_LOG_PATH, O_RDWR|O_CREAT|O_APPEND,0666);
	}
	
	if(npd_bootlog_fd > 0){
		if(0 == npd_startup_end){
			write(npd_bootlog_fd, buf, strlen(buf));
			return;
		}
	}
	// write log
	syslog(LOG_DEBUG,buf);
	return;	
}

/*******************************************************************************
* osPrintfDbg
*
* DESCRIPTION:
*       Write a formatted string to syslog daemon according to syslog level SYSLOG_DBG_DBG
*
* INPUTS:
*       format  - format string to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The number of characters written, or a negative value if
*       an output error occurs.
*
* COMMENTS:
*       None
*
*******************************************************************************/
int osPrintfDbg(const char* format, ...)
{
    char buff[MV_SYSLOG_BUFFER_SIZE] = {0};
    va_list args;
    int i = 0;
	

	if(!(SYSLOG_DBG_DBG & npd_log_level)) {
		return 0;
	}

	if(npd_log_close) {
		openlog(npd_log_ident, 0, LOG_DAEMON);
		npd_log_close = 0;
	}
	
    va_start(args, format);
    i = vsprintf(buff, format, args);
    va_end(args);
	
	cpss_syslog_emit(LOG_DEBUG,buff);

    return i;
}

/*******************************************************************************
* osPrintfErr
*
* DESCRIPTION:
*       Write a formatted string to syslog daemon according to syslog level SYSLOG_DBG_ERR
*
* INPUTS:
*       format  - format string to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The number of characters written, or a negative value if
*       an output error occurs.
*
* COMMENTS:
*       None
*
*******************************************************************************/
int osPrintfErr(const char* format, ...)
{
    char buff[MV_SYSLOG_BUFFER_SIZE] = {0};
    va_list args;
    int i = 0;
	

	if(!(SYSLOG_DBG_ERR & npd_log_level)) {
		return 0;
	}

	if(npd_log_close) {
		openlog(npd_log_ident, 0, LOG_DAEMON);
		npd_log_close = 0;
	}
	
    va_start(args, format);
    i = vsprintf(buff, format, args);
    va_end(args);
	
	cpss_syslog_emit(LOG_ERR,buff);

    return i;
}

/*******************************************************************************
* osPrintfWarn
*
* DESCRIPTION:
*       Write a formatted string to syslog daemon according to syslog level SYSLOG_DBG_WAR
*
* INPUTS:
*       format  - format string to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The number of characters written, or a negative value if
*       an output error occurs.
*
* COMMENTS:
*       None
*
*******************************************************************************/
int osPrintfWarn(const char* format, ...)
{
    char buff[MV_SYSLOG_BUFFER_SIZE] = {0};
    va_list args;
    int i = 0;
	

	if(!(SYSLOG_DBG_WAR & npd_log_level)) {
		return 0;
	}

	if(npd_log_close) {
		openlog(npd_log_ident, 0, LOG_DAEMON);
		npd_log_close = 0;
	}
	
    va_start(args, format);
    i = vsprintf(buff, format, args);
    va_end(args);
	
	cpss_syslog_emit(LOG_WARNING,buff);

    return i;
}

/*******************************************************************************
* osPrintfEvt
*
* DESCRIPTION:
*       Write a formatted string to syslog daemon according to syslog level SYSLOG_DBG_EVT
*
* INPUTS:
*       format  - format string to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The number of characters written, or a negative value if
*       an output error occurs.
*
* COMMENTS:
*       None
*
*******************************************************************************/
int osPrintfEvt(const char* format, ...)
{
    char buff[MV_SYSLOG_BUFFER_SIZE] = {0};
    va_list args;
    int i = 0;
	

	if(!(SYSLOG_DBG_EVT & npd_log_level)) {
		return 0;
	}

	if(npd_log_close) {
		openlog(npd_log_ident, 0, LOG_DAEMON);
		npd_log_close = 0;
	}
	
    va_start(args, format);
    i = vsprintf(buff, format, args);
    va_end(args);
	
	cpss_syslog_emit(LOG_INFO,buff);

    return i;
}

/*******************************************************************************
* osPrintfPktRx
*
* DESCRIPTION:
*       Write a formatted string to syslog daemon according to syslog level SYSLOG_DBG_PKT_REV
*
* INPUTS:
*       format  - format string to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The number of characters written, or a negative value if
*       an output error occurs.
*
* COMMENTS:
*       None
*
*******************************************************************************/
int osPrintfPktRx(const char* format, ...)
{
    char buff[MV_SYSLOG_BUFFER_SIZE] = {0};
    va_list args;
    int i = 0;
	

	if(!(SYSLOG_DBG_PKT_REV & npd_log_level)) {
		return 0;
	}
    
	if(npd_log_close) {
		openlog(npd_log_ident, 0, LOG_DAEMON);
		npd_log_close = 0;
	}
	
    va_start(args, format);
    i = vsprintf(buff, format, args);
    va_end(args);
	
	cpss_syslog_emit(LOG_DEBUG,buff);

    return i;
}

/*******************************************************************************
* osPrintfPktRx
*
* DESCRIPTION:
*       Write a formatted string to syslog daemon according to syslog level SYSLOG_DBG_PKT_SED
*
* INPUTS:
*       format  - format string to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The number of characters written, or a negative value if
*       an output error occurs.
*
* COMMENTS:
*       None
*
*******************************************************************************/
int osPrintfPktTx(const char* format, ...)
{
    char buff[MV_SYSLOG_BUFFER_SIZE] = {0};
    va_list args;
    int i = 0;
	
	
	if(!(SYSLOG_DBG_PKT_SED & npd_log_level)) {
		return 0;
	}

	if(npd_log_close) {
		openlog(npd_log_ident, 0, LOG_DAEMON);
		npd_log_close = 0;
	}
	
    va_start(args, format);
    i = vsprintf(buff, format, args);
    va_end(args);
	
	cpss_syslog_emit(LOG_DEBUG,buff);

    return i;
}

/**********************************************************************************
 *	osPrintfSyslog7Notice
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
void osPrintfSyslog7Notice
(
	char *format,
	...
)
{
	va_list ptr;
	static char buf[MV_SYSLOG_BUFFER_SIZE] = {0};

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

/**********************************************************************************
 *	cpss_write_crit_log
 * 
 *	write critical log to /etc/motd
 *
 *  INPUT: 
 *		format - message stream
 *  
 *  OUTPUT:
 * 	 NULL
 *
 *  RETURN:
 * 	 NULL
 * 	 
 *
 **********************************************************************************/
void cpss_write_crit_log
(
	char *format,
	...
)
{
	va_list ptr;
	static char buf[MV_SYSLOG_BUFFER_SIZE] = {0};
	int len = 0;

	/* ident null or format message null*/
	if(!format) {
		return;
	}
	
	/* buffering log*/
	va_start(ptr, format);
	len = vsprintf(buf,format,ptr);
	va_end(ptr);

	/* assure log file open only once globally*/
	if(crit_log_fd < 0) {
		crit_log_fd = open(CPSS_CRIT_LOGFILE, O_CREAT | O_RDWR | O_APPEND);
		if(crit_log_fd < 0) {
			osPrintfSyslog7Notice("write crit log to %s open error!\n", CPSS_CRIT_LOGFILE);
			return;
		}
	}

	write(crit_log_fd, buf, (len < MV_SYSLOG_BUFFER_SIZE) ? len : MV_SYSLOG_BUFFER_SIZE);
	return;	
}

#endif
