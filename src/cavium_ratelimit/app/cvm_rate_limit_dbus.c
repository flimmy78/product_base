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
*cvm_rate_limit_dbus.c
*
*CREATOR:
*	<hanhui@autelan.com>
*
*DESCRIPTION:
*     for dbus interfaces for cavium-ratelimit
*
*DATE:
*	15/05/2012	
*
*  FILE REVISION NUMBER:
*       $Revision: 1.1 $
*
*******************************************************************************/
#ifdef __cplusplus
	extern "C"
	{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <sys/time.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <syslog.h>

#include <dbus/dbus.h>
#include <pthread.h>

#include "sysdef/returncode.h"
#include "cvm_rate_limit_dbus.h"


int cr_major_num = 0;
static DBusConnection * cvm_rate_limit_dbus_connection = NULL;
unsigned int cvm_rate_limit_rules_load_source = 0; /*0: add rules from console, 1: load rules from file*/

int ioctlfd = -1;
pthread_t		*dbus_thread;
pthread_attr_t	dbus_thread_attr;

/*
we use this 3 levels 

 LOG_ERR = 3
 LOG_WARNING = 4
 LOG_DEBUG = 7

 all levels
 
LOG_EMERG		= 0   system is unusable

LOG_ALERT		= 1    action must be taken immediately

LOG_CRIT		= 2    critical conditions

LOG_ERR 		= 3    error conditions

LOG_WARNING 	= 4    warning conditions

LOG_NOTICE		= 5    normal, but significant, condition

LOG_INFO		= 6    informational message

LOG_DEBUG		= 7    debug-level message

*/

unsigned int cvm_rate_log_level = LOG_WARNING;

typedef struct protocol_match_item{
	unsigned char name[PROTOCOL_NAME_LEN];
	MATCH_TYPE protocol_match_rule[PACKET_MATCH_BYTE_NUM];
	MATCH_TYPE protocol_match_mask [PACKET_MATCH_BYTE_NUM];
	unsigned int rateLimit;
	unsigned int rules_length;
	STATISTIC_TYPE drop_counter;/*for statistic */
	STATISTIC_TYPE pass_counter;/*for statistic */
	unsigned int counter; /*for timer*/
	unsigned int unused;
} protocol_match_item_t;

struct cr_ioctl_struct{
	union{
		protocol_match_item_t protocol_rule;
		struct names{
			unsigned char new_name[PROTOCOL_NAME_LEN];
			unsigned char old_name[PROTOCOL_NAME_LEN];
		}name;
		struct counter{
			STATISTIC_TYPE drop_counter;
			STATISTIC_TYPE pass_counter;
		}c;
		struct enable
		{
			unsigned int enable;
			unsigned int flag;
		}e;
		unsigned int index;
		unsigned int enable;
		unsigned int flag;
	}u;
};

int tp_module_check()
{
	if(0 > ioctlfd)
	{
		return CVM_RATELIMIT_RETURN_CODE_MODULE_NOTRUNNING;
	}
	return CVM_RATELIMIT_RETURN_CODE_SUCCESS;
}
/****************************************************
 * DESCRIPTION:
 * 	syslog output
 * INPUT:
 *	uint32 - loglevel, LOG_DEBUG, LOG_WARNING, LOG_ERR,...
 *	char * format, ... out format and argvs
 * OUTPUT:
 * 	NONE
 * RETURN:
 *	NONE
 * NOTE:
 *	NONE
 *
 ****************************************************/
void syslog_cvm_rate
(
	unsigned int loglevel,
	char *format,
	...
)
{
#define LOG_LINE_LEN 2048
/*
 LOG_ERR = 3
 LOG_WARNING = 4
 LOG_DEBUG = 7
*/
	va_list ptr;
	static char buf[LOG_LINE_LEN+1] = {0};

	/* event not triggered*/
	if(loglevel > cvm_rate_log_level) {
		return;
	}

	/* ident null or format message null*/
	if(!format) {
		return;
	}

	/* buffering log*/
	va_start(ptr, format);
	vsnprintf(buf,LOG_LINE_LEN,format,ptr);
	va_end(ptr);

	/* assure log file open only once globally*/
	/* write log*/
	syslog(loglevel,buf);
	return; 
}
/******************************************************
 * DESCRIPTION:
 *	log level set
 * INPUT:
 *	uint32 - level ,  log level : NONE, ERROR, WARNING, DEBUG
 * OUTPUT:
 * 	uint32 - cvm_rate_log_level , global integer various
 * RETURN:
 *	0
 * NOTE:
 *	NONE
 *
 ******************************************************/
static int cvm_rate_limit_syslog_level_set(unsigned int level)
{
	enum log_level{
		NONE = 0,
		ERROR = 1,
		WARNING = 2,
		DEBUG = 3
	};
	switch(level)
	{
		case NONE:
			cvm_rate_log_level = 0;
			break;
		case ERROR:
			cvm_rate_log_level = LOG_ERR;
			break;
		case WARNING:
			cvm_rate_log_level = LOG_WARNING;
			break;
		case DEBUG:
			cvm_rate_log_level = LOG_DEBUG;
			break;
		default:
			break;
	}
	return 0;
}
/**************************************************
 * DESCRIPTION:
 *	init dbus connetion
 * INPUT:
 * 	NONE
 * OUTPUT:
 *	NONE
 * RETURN:
 *	TRUE - init success
 *	FAULT  - init failed
 * NOTE:
 * 	NONE
 **************************************************/
int cvm_rate_limit_dbus_init(void)
{	
	DBusError dbus_error;
	DBusObjectPathVTable	cvm_rate_limit_vtable = {NULL, &cvm_rate_limit_dbus_message_handler, NULL, NULL, NULL, NULL};
	
	dbus_connection_set_change_sigpipe (TRUE);

	dbus_error_init (&dbus_error);
	cvm_rate_limit_dbus_connection = dbus_bus_get (DBUS_BUS_SYSTEM, &dbus_error);
	if (cvm_rate_limit_dbus_connection == NULL) {
		syslog_cvm_rate (LOG_ERR, "dbus_bus_get(): %s", dbus_error.message);
		return FALSE;
	}

	/* Use npd to handle subsection of CVM_RATE_LIMIT_DBUS_OBJPATH including slots*/
	if (!dbus_connection_register_fallback (cvm_rate_limit_dbus_connection, CVM_RATE_LIMIT_DBUS_OBJPATH, &cvm_rate_limit_vtable, NULL)) {
		syslog_cvm_rate (LOG_ERR, "can't register D-BUS handlers (fallback CVM-RATELIMIT). cannot continue.");
		return FALSE;
		
	}

	
	dbus_bus_request_name (cvm_rate_limit_dbus_connection, CVM_RATE_LIMIT_DBUS_BUSNAME,
			       0, &dbus_error);
	
	
	if (dbus_error_is_set (&dbus_error)) {
		syslog_cvm_rate (LOG_ERR, "dbus_bus_request_name(): %s",
			    dbus_error.message);
		return FALSE;
	}
	return TRUE;
}

int tp_after_load_module()
{	
#define CVM_RATELIMIT_DEV "/dev/cavium_ratelimit"
#define MAJOR_NUM_PATH 	"/sys/module/cavium_ratelimit/parameters/cr_major_num"
#define BUFF_LEN 10

	FILE * major_fd = NULL;
	unsigned char buf[BUFF_LEN] = {0};
	
	ioctlfd = open(CVM_RATELIMIT_DEV,O_RDWR);
	if(ioctlfd < 0)
	{
		syslog_cvm_rate (LOG_ERR, "open cavium_ratelimit failed err %s\n",strerror(errno));
		return -1;
	}
	else
	{
		syslog_cvm_rate (LOG_DEBUG, "open /dev/cavium_ratelimit success\n");
	}
	
	major_fd = fopen(MAJOR_NUM_PATH, "r");
	if(major_fd)
	{
		fread(buf, BUFF_LEN, 1, major_fd);
		cr_major_num = strtoul(buf, NULL, 0);
		syslog_cvm_rate (LOG_DEBUG, "read major num %d\n", cr_major_num);
		fclose(major_fd);
		if(0 == cr_major_num)
		{
			ioctlfd = -1;
		}
	}
	else
	{
		syslog_cvm_rate (LOG_ERR, "can't open major num file to read\n");
		close(ioctlfd);
		ioctlfd = -1;
		return -1;
	}
		
	return 0;
}
/**********************************************************
 * DESCRIPTION:
 * 	rate limit function enable or disable set
 * INPUT:
 *	int - value , 0  disable, !0 enable
 * OUTPUT:
 *	NONE
 * RETURN:
 *		0 - set success
 *		-1 - ioctl failed
 * NOTE:
 *	static function , for private use
 *
 **********************************************************/
static int cvm_rate_limit_enable_set(unsigned int value, unsigned int flag)
{
	struct cr_ioctl_struct rule_struct;
	int ret = 0;
	memset(&rule_struct, 0, sizeof(rule_struct));
	rule_struct.u.e.enable = value;
	rule_struct.u.e.flag = flag;

	if(tp_module_check())
	{
		syslog_cvm_rate (LOG_ERR, "enable set module start failed or get fd  failed : %s\n",strerror(errno));
		return CVM_RATELIMIT_RETURN_CODE_MODULE_NOTRUNNING;
	}
	else
	{
		ret = ioctl(ioctlfd, IOCTLCMD(CRENABLESET), &rule_struct);
		if(ret < 0)
		{
			syslog_cvm_rate (LOG_ERR, "enable set ioctl failed error : %s\n",strerror(errno));
			return CVM_RATELIMIT_RETURN_CODE_FAILED;
		}
		return CVM_RATELIMIT_RETURN_CODE_SUCCESS;
	}
}
static int get_mask_len(MATCH_TYPE mask)
{
	int i = 0;
	unsigned char * ptr = &mask;
	for(i = MATCH_TYPE_LEN; i > 0; i--)
	{
		if(ptr[i - 1])
		{
			return i;
		}
	}
}

/**********************************************************************************
 * DESCRIPTION:
 * 	add rate limit rule
 * INPUT:
 *	string     -  name  -- rule name [a-zA-Z0-9_]
 *	uint32 * -  rules   -- address of rule array with six uint32 elements
 *	uint32 * -  mask   -- address of mask array with six uint32 elements
 *	uint32    -  limiter -- the limiter value we want to set
 * OUTPUT:
 *	NONE
 * RETURN:
 *		0 - set success
 *		-1 - unknown error
 *		-2 - invalid mask
 *		errno - ioctl failed, EEXIST, EFBIG, ...
 *
 * NOTE:
 *	static function , for private use
 *
 **********************************************************************************/
static int cvm_rate_limit_add_rule(unsigned char * name, MATCH_TYPE *rules, MATCH_TYPE *mask, unsigned int limiter)
{
	struct cr_ioctl_struct rule_struct;
	int ret = 0;
	int i = 0;
	if(!name||!rules||!mask)
	{
		syslog_cvm_rate (LOG_ERR, "null pointer input!\n");
		return -1;
	}
	 
	memset(&rule_struct, 0, sizeof(rule_struct));
	strcpy(rule_struct.u.protocol_rule.name, name);
	memcpy(rule_struct.u.protocol_rule.protocol_match_rule, rules, PACKET_MATCH_BYTE_NUM*MATCH_TYPE_LEN);
	memcpy(rule_struct.u.protocol_rule.protocol_match_mask, mask, PACKET_MATCH_BYTE_NUM*MATCH_TYPE_LEN);
	for(i = PACKET_MATCH_BYTE_NUM; i > 0; i--)
	{
		if(mask[i-1])
		{
			rule_struct.u.protocol_rule.rules_length = ((i - 1) * MATCH_TYPE_LEN) \
							+ get_mask_len(mask[i-1]) ;
			syslog_cvm_rate (LOG_DEBUG, "get rule_length %d\n", rule_struct.u.protocol_rule.rules_length);
			break;
		}
		
	}
	if(!rule_struct.u.protocol_rule.rules_length)
	{
		syslog_cvm_rate (LOG_ERR, "get mask all 0 when add rules %s\n", name);
		return -2;
	}
	rule_struct.u.protocol_rule.rateLimit = limiter;
	rule_struct.u.protocol_rule.counter = 0;
	ret = ioctl(ioctlfd, IOCTLCMD(CRADDRULES), &rule_struct);
	if(ret < 0)
	{
		syslog_cvm_rate (LOG_ERR, "add rules ioctl failed error : %s\n",strerror(errno));
		
		return errno;
	}

	syslog_cvm_rate (LOG_DEBUG, "add rate limit rule %s success\n", rule_struct.u.protocol_rule.name);
	
	return 0;
	
}

/********************************************************
 * DESCRIPTION:
 * 	modify protocol match rule's rule/mask/limit value
 * INPUT:
 *	string -  name  -- the name of the rule which we want to modify
 *	uint32 - type,  1. rule, 2. mask, 3. limiter
 *	uint32 * - rules  address of rule (type 1) or mask (type 2) array 
 *					with six int type elements
 *	uint32 - limiter, limiter value, valid when type is 3
 * OUTPUT:
 *	NONE
 * RETURN:
 *		0 - set success
 *		-1 - unknown error
 *		-2 - mask all zero, invalid mask
 *		errno - the error no of ioctl : ENOENT, EPERM, and others
 * NOTE:
 *	static function , for private use
 *
 ********************************************************/
static int cvm_rate_limit_modify_rule(unsigned char * name, unsigned int type, MATCH_TYPE *rules,unsigned int limiter)
{
	struct cr_ioctl_struct rule_struct;
	int ret = 0;
	int i = 0;
	enum valuetype{
		RULE = 1,
		MASK = 2,
		LIMITER = 3
	};
	if(!name||!rules)
	{
		return -1;
	}
	 
	memset(&rule_struct, 0, sizeof(rule_struct));
	strcpy(rule_struct.u.protocol_rule.name, name);
	if(RULE == type)
	{
		memcpy(rule_struct.u.protocol_rule.protocol_match_rule, rules, PACKET_MATCH_BYTE_NUM*MATCH_TYPE_LEN);
		
		ret = ioctl(ioctlfd, IOCTLCMD(CRMODIFYRULES), &rule_struct);
		
	}
	else if(MASK == type)
	{
		ret = 0;
		for(i = 0; i < PACKET_MATCH_BYTE_NUM; i++)
		{
			ret |= rules[i];
		}
		if(!ret)
		{
			syslog_cvm_rate (LOG_ERR, "get mask all 0 when modify mask of %s\n", name);
			return -2;
		}
		memcpy(rule_struct.u.protocol_rule.protocol_match_mask, rules, PACKET_MATCH_BYTE_NUM*MATCH_TYPE_LEN);
		
		ret = ioctl(ioctlfd, IOCTLCMD(CRMODIFYMASKS), &rule_struct);
		
	}
	else if(LIMITER == type)
	{		
		rule_struct.u.protocol_rule.rateLimit = limiter;
		ret = ioctl(ioctlfd, IOCTLCMD(CRMODIFYLIMITER), &rule_struct);
		
	}
	else
	{
		return -1;
	}
	if(ret < 0)
	{
		syslog_cvm_rate (LOG_ERR, "modify rules %s type %d ioctl failed error : %s\n",name, type, strerror(errno));
		
		return errno;
	}

	return 0;
}
/*****************************************************************
  * DESCRIPTION:
 * 	delete rate limit rule
 * INPUT:
 *	string -  name --the name of the rule which we want to delete
 * OUTPUT:
 *	NONE
 * RETURN:
 *		0 - set success
 *		-1 - unknown error
 *		errno - the error no of ioctl : ENOENT, EPERM, and others
 * NOTE:
 *	static function , for private use
 *
 ********************************************************************/
static int cvm_rate_limit_del_rule(unsigned char * name)
{
	struct cr_ioctl_struct rule_struct;
	int ret = 0;
	int i = 0;
	if(!name)
	{
		return -1;
	}
	 
	memset(&rule_struct, 0, sizeof(rule_struct));
	strcpy(rule_struct.u.protocol_rule.name, name);
	ret = ioctl(ioctlfd, IOCTLCMD(CRDELRULES), &rule_struct);
	if(ret < 0)
	{
		syslog_cvm_rate (LOG_ERR, "delete rules ioctl failed error : %s\n",strerror(errno));
		return errno;
	}
	return 0;
}
/*****************************************************************
  * DESCRIPTION:
 * 	clear all rate limit rules
 * INPUT:
 *	NONE
 * OUTPUT:
 *	NONE
 * RETURN:
 *		0 - set success
 *		errno - the error no of ioctl : ENOENT, EPERM, and others
 * NOTE:
 *	static function , for private use
 *
 ********************************************************************/
static int cvm_rate_limit_clear_rule(void)
{
	struct cr_ioctl_struct rule_struct;
	int ret = 0;
	int i = 0;
		 
	memset(&rule_struct, 0, sizeof(rule_struct));
	ret = ioctl(ioctlfd, IOCTLCMD(CRCLEARUDFRULES), &rule_struct);
	if(ret < 0)
	{
		syslog_cvm_rate (LOG_ERR, "clear rules ioctl failed error : %s\n",strerror(errno));
		return errno;
	}
	return 0;
}
/*****************************************************************
  * DESCRIPTION:
 * 	get the rule item by name
 * INPUT:
 *	string -  name --the name of the rule which we want to delete
 * OUTPUT:
 *	protocol_match_item_t * out_struct -- the rule item we'll get
 * RETURN:
 *		0 - set success
 *		-1 - not exists or get failed
 * NOTE:
 *	static function , for private use
 *
 ********************************************************************/

static int cvm_rate_limit_get_rules(unsigned char * name, protocol_match_item_t * out_struct, int flag)
{
	struct cr_ioctl_struct rule_struct;
	int ret = 0;
	if(!name||!out_struct)
	{
		return -1;
	}
	 
	memset(&rule_struct, 0, sizeof(rule_struct));
	strcpy(rule_struct.u.protocol_rule.name, name);
	if(!flag)
	{
		ret = ioctl(ioctlfd, IOCTLCMD(CRGETRULE), &rule_struct);
	}
	else
	{
		ret = ioctl(ioctlfd, IOCTLCMD(CRGETRULE4FFWD), &rule_struct);
	}
	if(ret < 0)
	{
		syslog_cvm_rate (LOG_ERR, "get rules ioctl failed error : %s\n",strerror(errno));
		return -1;
	}
	memcpy(out_struct, &rule_struct, sizeof(protocol_match_item_t));
	return 0;
}
/*****************************************************************
  * DESCRIPTION:
 * 	delete rate limit rule
 * INPUT:
 *	uint32 - index -- the index of the rule we want to get
 * OUTPUT:
 *	protocol_match_item_t * out_struct -- the rule item we'll get
 * RETURN:
 *		0 - set success
 *		-1 - not exists or get failed
 * NOTE:
 *	static function , for private use
 *
 ********************************************************************/

static int cvm_rate_limit_get_rules_byindex(unsigned int index, protocol_match_item_t * out_struct, int flag)
{
	struct cr_ioctl_struct rule_struct;
	int ret = 0;
	memset(&rule_struct, 0, sizeof(rule_struct));
	if(!out_struct)
	{
		syslog_cvm_rate(LOG_DEBUG, "get rules by index %d failed, null pointer : out_struct\n");
		return -1;
	}
	rule_struct.u.index = index;
	if(!flag)
	{
		ret = ioctl(ioctlfd, IOCTLCMD(CRGETRULEBYINDEX), &rule_struct);
	}
	else
	{
		ret = ioctl(ioctlfd, IOCTLCMD(CRGETRULEBYIDX4FFWD), &rule_struct);		
	}
	if(ret < 0)
	{
		if(ENOENT != errno)
		{
			syslog_cvm_rate(LOG_DEBUG, "get rules by index %d ioctl failed error : %s\n",index, strerror(errno));
			return -1;
		}
		return -2;
	}
	
	memcpy(out_struct, &rule_struct, sizeof(protocol_match_item_t));
	return 0;
}

/*****************************************************************
  * DESCRIPTION:
 * 	read and parse rules from file 
 * INPUT:
 *	FILE * - fd -- the pointer of the file we want to read and parse
 * OUTPUT:
 *	string - name -- the rule name we got
 *	uint32 * - rule -- the address of the rule[16]
 *	uint32 * - mask -- the address of the mask[16]
 *    uint32 * - limiter -- the limiter value address , &uint32
 * RETURN:
 *		0 - set success
 *		-1 - not exists or get failed
 * NOTE:
 *	static function , for private use
 *
 ********************************************************************/
static int cvm_rate_limit_parse_rules(FILE * fd, unsigned char * name, MATCH_TYPE *rule, MATCH_TYPE *mask, unsigned int *limiter)
{


#define LINE_BUFF_LEN 256
#define FINDSECTION(str) while((line = fgets(line_buff, LINE_BUFF_LEN, fd)) && strncmp(line_buff, (str), strlen((str)))){;}


	unsigned char * line = NULL;
	unsigned char * numstr = NULL;
	unsigned char line_buff[LINE_BUFF_LEN] = {0};
	
	unsigned int tmpnum = 0;
	unsigned int *int_rule = rule;
	unsigned int *int_mask = mask;
		
	int i = 0;
	int j = 0;	
	int int_len = (MATCH_TYPE_LEN/sizeof(unsigned int))*MAX_MATCH_RULES_NUM;
	int rule_line_num = (PACKET_MATCH_BYTE_NUM/RULES_INT_NUM_PER_LINE);
	
	if(!fd||!name||!rule||!mask||!limiter){goto error;}
	
	FINDSECTION("#name");
	
	if(!line)
	{/*no #name line*/
		syslog_cvm_rate(LOG_DEBUG, "get #name failed when load rule from file\n");
		goto error;
	}
	
	memset(name, 0, PROTOCOL_NAME_LEN);
	line = fgets(line_buff, LINE_BUFF_LEN, fd);
	i = strlen(line) - 1;
	while((i >= 0) && ((line[i] == '\r')||(line[i] == '\n')||(line[i] == ' ')||(line[i] == '\t')))
	{
		line[i] = '\0';
		i--;
	}
	if(line)
	{
		for(i = strlen(line)-1; i >= 0; i--)
		{
			if(!(((line[i] >= '0')&&(line[i] <= '9'))||\
				((line[i] >= 'a')&&(line[i] <= 'z'))||\
				((line[i] >= 'A')&&(line[i] <= 'Z'))||\
				(line[i] == '_')))
			{
				
				syslog_cvm_rate(LOG_DEBUG, "get invalid name %s, include char '%c' ASCII %#x when load rule from file\n", line, line[i], line[i]);
				goto error;
			}
		}
		syslog_cvm_rate(LOG_DEBUG, "get name %s when load rule from file\n", line);
		memcpy(name, line, PROTOCOL_NAME_LEN-1);
	}
	else
	{
		syslog_cvm_rate(LOG_ERR, "get name failed when load rule from file\n");
		goto error;
	}
	
	FINDSECTION("#rule");
	
	if(!line)
	{/*no #rule line*/
		syslog_cvm_rate(LOG_ERR, "get #rule failed when load rule from file\n");
		goto error;
	}
	j = 0;
	memset(rule, 0, MATCH_TYPE_LEN*PACKET_MATCH_BYTE_NUM);
	for(i = 0; i < rule_line_num; i++)
	{
		line = fgets(line_buff, LINE_BUFF_LEN, fd);
		if(line)
		{
			if(!strncmp(line, "#mask", strlen("#mask")))
			{
				goto getmask;
			}
			if(j >= int_len)
			{				
				break;
			}
			numstr = strtok(line, " \n\t");
			while(numstr)
			{
				if(j >= int_len)
				{
					break;
				}
				tmpnum = strtoul(numstr, NULL, 16);
				int_rule[j] = (tmpnum&0xffff)<<16; /*high 16 bits*/
				numstr = strtok(NULL, " \n\t");
				if(numstr)
				{
					tmpnum = strtoul(numstr, NULL, 16);
					int_rule[j] |= (tmpnum&0xffff);/*low 16 bits*/
					numstr = strtok(NULL, " \n\t");
					j++;
				}
			}
		}
		else
		{
			syslog_cvm_rate(LOG_ERR, "get rule failed when load rule from file\n");
			goto error;
		}
	}

	FINDSECTION("#mask");
	
	if(!line)
	{/*no #mask line*/
		syslog_cvm_rate(LOG_ERR, "get #mask failed when load rule from file\n");
		goto error;
	}
	
getmask:
	j = 0;
	memset(mask, 0, MATCH_TYPE_LEN*PACKET_MATCH_BYTE_NUM);
	for(i = 0; i < rule_line_num; i++)
	{
		line = fgets(line_buff, LINE_BUFF_LEN, fd);
		if(line)
		{
			if(!strncmp(line, "#limiter", strlen("#limiter")))
			{
				goto getlimiter;
			}
			if(j >= int_len)
			{				
				break;
			}
			numstr = strtok(line, " \n\t");
			while(numstr)
			{
				if(j >= int_len)
				{
					break;
				}
				tmpnum = strtoul(numstr, NULL, 16);
				int_mask[j] = (tmpnum&0xffff)<<16; /*high 16 bits*/
				numstr = strtok(NULL, " \n\t");
				if(numstr)
				{
					tmpnum = strtoul(numstr, NULL, 16);
					int_mask[j] |= (tmpnum&0xffff);/*low 16 bits*/
					numstr = strtok(NULL, " \n\t");
					j++;
				}
			}
		}
		else
		{
			syslog_cvm_rate(LOG_ERR, "get mask failed when load rule from file\n");
		}
	}

	FINDSECTION("#limiter");
	
	if(!line)
	{/*no #limiter line*/
		syslog_cvm_rate(LOG_ERR, "get #limiter failed when load rule from file\n");
		goto error;
	}
	
getlimiter:	
	memset(limiter, 0, sizeof(unsigned int));
	line = fgets(line_buff, LINE_BUFF_LEN, fd);
	if(line)
	{
		if(!strncmp(line, "nolimit", strlen("nolimit")))
		{
			*limiter = CVM_RATE_NO_LIMIT;
		}
		else
		{
			*limiter = strtoul(line, NULL, 10);
		}
	}
	else
	{
		syslog_cvm_rate(LOG_ERR, "get limiter failed when load rule from file\n");
		goto error;
	}

	return 0;
error:
	return -1;
}

/***************************************************
 * DESCRIPTION:
 * 	load rate limit rules from file , add as udf rules (index 12~31)
 * INPUT:
 *	NONE
 * OUTPUT:
 *	NONE
 * RETURN:
 *		0 - set success
 *		-1 - not exists or get failed
 * NOTE:
 *	static function , for private use
 *
 ***************************************************/
static int cvm_rate_limit_load_rules_from_file(void)
{
	unsigned int ret = 0;
	unsigned char name[PROTOCOL_NAME_LEN] = {0};
	MATCH_TYPE rules[PACKET_MATCH_BYTE_NUM] = {0};
	MATCH_TYPE mask[PACKET_MATCH_BYTE_NUM] = {0};
	unsigned int limiter = 0;
	unsigned char * namePtr = name;
	unsigned int source = 0;
	FILE * fd = NULL;
	int i = 0;
	int len = (MAX_MATCH_RULES_NUM - CVM_RATE_LIMIT_DEFAULT_RULE_NUM);
	/*open file */
		fd = fopen(CVM_RATE_RULE_CONF_FILE_PATH, "r");
		if(NULL == fd)
		{
			syslog_cvm_rate(LOG_ERR, "open file %s failed\n", CVM_RATE_RULE_CONF_FILE_PATH);
			ret = CVM_RATELIMIT_RETURN_CODE_FAILED;
		}
		else{
			while((!ret) && (i < len))
			{
				ret = cvm_rate_limit_parse_rules(fd, namePtr, rules, mask, &limiter);/*get one item for add rule*/
				if(!ret)
				{
					cvm_rate_limit_add_rule(namePtr, rules, mask, limiter);
				}
				else
				{
					syslog_cvm_rate(LOG_DEBUG, "get new rule failed, i %d\n", i);
				}
				i++;
			}
			ret = CVM_RATELIMIT_RETURN_CODE_SUCCESS;
			fclose(fd);
		}
		return ret;
}


/***************************************************
 * DESCRIPTION:
 * 	restore rate limit rules to file , save as udf rules (index 12~31)
 * INPUT:
 *	NONE
 * OUTPUT:
 *	NONE
 * RETURN:
 *		0 - set success
 *		-1 - not exists or get failed
 * NOTE:
 *	static function , for private use
 *
 ***************************************************/
static int cvm_rate_limit_restore_rules_to_file(void)
{
#define CMD_LINE_LEN 64
	unsigned int ret = 0;
	unsigned int limiter = 0;
	unsigned int *rules = NULL;
	unsigned int *mask = NULL;
	
	unsigned char name[PROTOCOL_NAME_LEN] = {0};
	unsigned char * namePtr = name;
	unsigned char buffer[14*_1K] = {0};
	unsigned char cmd[CMD_LINE_LEN] = {0};
	
	FILE * fd = NULL;
	
	protocol_match_item_t rule_struct;
	
	int i = 0;
	int j = 0;
	int offset = 0;
	int rule_line_num = (RULES_INT_NUM_PER_LINE);
	int int_len = (MATCH_TYPE_LEN/sizeof(unsigned int))*PACKET_MATCH_BYTE_NUM;
	
	for(i = CVM_RATE_LIMIT_DEFAULT_RULE_NUM; i < MAX_MATCH_RULES_NUM; i++)
	{/* only for udf rules*/
		ret = cvm_rate_limit_get_rules_byindex(i, &rule_struct, 0);
		if(!ret)
		{
			rules = rule_struct.protocol_match_rule;
			mask = rule_struct.protocol_match_mask;
			offset += sprintf(buffer + offset, "#name\n%s\n#rule\n", \
				rule_struct.name);
			for(j = 0; j < int_len; j++)
			{
				offset += sprintf(buffer + offset, "%04x %04x%s", \
				((rules[j] >> 16)&0xffff), \
				(rules[j]&0xffff), \
				(((j+1)%rule_line_num) ? " " : "\n"));
			}
			offset += sprintf(buffer + offset, "#mask\n", \
				rule_struct.name);
			for(j = 0; j < int_len; j++)
			{
				offset += sprintf(buffer + offset, "%04x %04x%s", \
				((mask[j] >> 16)&0xffff), \
				(mask[j]&0xffff), \
				(((j+1)%rule_line_num) ? " " : "\n"));
			}
			offset += sprintf(buffer + offset, "#limiter\n", \
				rule_struct.name);
			if(CVM_RATE_NO_LIMIT <= rule_struct.rateLimit)
			{
				offset += sprintf(buffer + offset, "nolimit\n", \
					rule_struct.name);
			}
			else
			{
				offset += sprintf(buffer + offset, "%d\n", \
					rule_struct.rateLimit);
			}
			offset += sprintf(buffer + offset, "\n");
		
		}
		else if(-2 == ret)
		{
				syslog_cvm_rate(LOG_DEBUG, "restore rules : set rules to buffer end, last valid rule index is %d\n", i - 1);
				break;
		}
		/* else already loged inside the function */
	}
	/*open file */
	fd = fopen(CVM_RATE_RULE_CONF_FILE_PATH, "w");
	if(NULL == fd)
	{
		syslog_cvm_rate(LOG_ERR, "open file %s failed\n", CVM_RATE_RULE_CONF_FILE_PATH);
		ret = CVM_RATELIMIT_RETURN_CODE_FAILED;
	}
	else{		
		fwrite(buffer, _1K,(offset+_1K-1)/_1K, fd);
		fclose(fd);
		sprintf(cmd, "sor.sh %s %s %d ", "cp", CVM_RATE_RULE_CONF_FILE_NAME, 10);
		ret = system(cmd);
		if(!(WEXITSTATUS(ret)))
		{
			syslog_cvm_rate(LOG_DEBUG, "restore rules to file success! len %d \n", offset);
			ret = CVM_RATELIMIT_RETURN_CODE_SUCCESS;
		}
		else
		{
			/* write to /mnt/xxx, but sync to /blk failed ,
			 ret : 3 - busy, 4 - timeout, 5 - no space, others - unknown*/
			syslog_cvm_rate(LOG_ERR, "restore rules to file in /blk failed!, ret %#x, But already write to file in /mnt !\n", WEXITSTATUS(ret));
			ret = CVM_RATELIMIT_RETURN_CODE_FAILED;
		}
	}
	return ret;
}


/*****************************************************
 * DESCRIPTION:
 *	daemon for dbus connetion and call
 *
 *****************************************************/
static DBusHandlerResult cvm_rate_limit_dbus_message_handler 
(
	DBusConnection *connection, 
	DBusMessage *message, 
	void *user_data
)
{
		DBusMessage 	*reply = NULL;
		
		if (strcmp(dbus_message_get_path(message),CVM_RATE_LIMIT_DBUS_OBJPATH) == 0) 
		{
			if(dbus_message_is_method_call(message,CVM_RATE_LIMIT_DBUS_INTERFACE,CVM_RATE_LIMIT_DBUS_METHOD_ENABLE_SET))
			{
				reply = cvm_rate_limit_dbus_enable_set(connection,message,user_data);
			}
			else if(dbus_message_is_method_call(message,CVM_RATE_LIMIT_DBUS_INTERFACE,CVM_RATE_LIMIT_DBUS_METHOD_LOG_LEVEL_SET))
			{
				reply = cvm_rate_limit_dbus_loglevel_set(connection,message,user_data);
			}
			else if(dbus_message_is_method_call(message,CVM_RATE_LIMIT_DBUS_INTERFACE,CVM_RATE_LIMIT_DBUS_METHOD_SHOW_RULES))
			{
				reply = cvm_rate_limit_dbus_show_rules(connection,message,user_data);
			}
			else if(dbus_message_is_method_call(message,CVM_RATE_LIMIT_DBUS_INTERFACE,CVM_RATE_LIMIT_DBUS_METHOD_ADD_RULES))
			{
				reply = cvm_rate_limit_dbus_add_rule(connection,message,user_data);
			}
			else if(dbus_message_is_method_call(message,CVM_RATE_LIMIT_DBUS_INTERFACE,CVM_RATE_LIMIT_DBUS_METHOD_ADD_RULES_SIMP))
			{
				reply = cvm_rate_limit_dbus_add_rule_simp(connection,message,user_data);
			}
			else if(dbus_message_is_method_call(message,CVM_RATE_LIMIT_DBUS_INTERFACE,CVM_RATE_LIMIT_DBUS_METHOD_DEL_RULES))
			{
				reply = cvm_rate_limit_dbus_del_rule(connection,message,user_data);
			}
			else if(dbus_message_is_method_call(message,CVM_RATE_LIMIT_DBUS_INTERFACE,CVM_RATE_LIMIT_DBUS_METHOD_MODIFY_RULES))
			{
				reply = cvm_rate_limit_dbus_modify_rule(connection,message,user_data);
			}
			else if(dbus_message_is_method_call(message,CVM_RATE_LIMIT_DBUS_INTERFACE,CVM_RATE_LIMIT_DBUS_METHOD_SHOW_RUNNING))
			{
				reply = cvm_rate_limit_dbus_show_running(connection,message,user_data);
			}
			else if(dbus_message_is_method_call(message,CVM_RATE_LIMIT_DBUS_INTERFACE,CVM_RATE_LIMIT_DBUS_METHOD_SHOW_STATISTIC))
			{
				reply = cvm_rate_limit_dbus_show_statistic(connection,message,user_data);
			}
			else if(dbus_message_is_method_call(message,CVM_RATE_LIMIT_DBUS_INTERFACE,CVM_RATE_LIMIT_DBUS_METHOD_CLEAR_STATISTIC))
			{
				reply = cvm_rate_limit_dbus_clear_statistic(connection,message,user_data);
			}
			else if(dbus_message_is_method_call(message,CVM_RATE_LIMIT_DBUS_INTERFACE,CVM_RATE_LIMIT_DBUS_METHOD_DMESG_ENABLE_SET))
			{
				reply = cvm_rate_limit_dbus_dmesg_enable_set(connection,message,user_data);
			}
			else if(dbus_message_is_method_call(message,CVM_RATE_LIMIT_DBUS_INTERFACE,CVM_RATE_LIMIT_DBUS_METHOD_LOAD_RULES_FROM_FILE))
			{
				reply = cvm_rate_limit_dbus_load_rules_from_file(connection,message,user_data);
			}
			else if(dbus_message_is_method_call(message,CVM_RATE_LIMIT_DBUS_INTERFACE,CVM_RATE_LIMIT_DBUS_METHOD_LOAD_RULES_SOURCE_SET))
			{
				reply = cvm_rate_limit_dbus_config_rules_source(connection,message,user_data);
			}			
			else if(dbus_message_is_method_call(message,CVM_RATE_LIMIT_DBUS_INTERFACE,CVM_RATE_LIMIT_DBUS_METHOD_RESTORE_RULES_TO_FILE))
			{
				reply = cvm_rate_limit_dbus_restore_rules_to_file(connection,message,user_data);
			}			
			else if(dbus_message_is_method_call(message,CVM_RATE_LIMIT_DBUS_INTERFACE,CVM_RATE_LIMIT_DBUS_METHOD_CLEAR_RULES))
			{
				reply = cvm_rate_limit_dbus_clear_rule(connection,message,user_data);
			}			
			else if(dbus_message_is_method_call(message,CVM_RATE_LIMIT_DBUS_INTERFACE,CVM_RATE_LIMIT_DBUS_METHOD_SERVICE_LOAD))
			{
				reply = cvm_rate_limit_dbus_service_load(connection,message,user_data);
			}
			else if(dbus_message_is_method_call(message,CVM_RATE_LIMIT_DBUS_INTERFACE,CVM_RATE_LIMIT_DBUS_METHOD_SERVICE_UNLOAD))
			{
				reply = cvm_rate_limit_dbus_service_unload(connection,message,user_data);
			}		
		}
		
		if (reply)
		{
			dbus_connection_send (connection, reply, NULL);
			dbus_connection_flush(connection); // TODO	  Maybe we should let main loop process the flush
			dbus_message_unref (reply);
		}

		return DBUS_HANDLER_RESULT_HANDLED;
}

DBusMessage * cvm_rate_limit_dbus_loglevel_set(DBusConnection *conn, DBusMessage *msg, void *user_data){
	DBusMessage* reply;
	DBusError err;
	unsigned int ret = 0;
	unsigned int value = 0;
	
	dbus_error_init( &err );
	if(!(dbus_message_get_args(msg,&err,
								DBUS_TYPE_UINT32,&value,
								DBUS_TYPE_INVALID)))
	{
		if(dbus_error_is_set( &err ))
		{
			dbus_error_free( &err );
		}
		return NULL;	
	}
	if(cvm_rate_limit_syslog_level_set(value))
	{
		ret = CVM_RATELIMIT_RETURN_CODE_FAILED;
	}
	reply = dbus_message_new_method_return(msg);
	
	if(NULL == reply)
	{
		return NULL;
	}
	
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	
	return reply;
}

DBusMessage * cvm_rate_limit_dbus_service_load(DBusConnection *conn, DBusMessage *msg, void *user_data)
{	
#define CMD_LINE_LEN 128
#define LOAD_MODULE_CMD "sudo modprobe cavium_ratelimit cr_major_num=244 cr_minor_num=0 >> /var/log/modprobe.log"
#define CHECK_OR_CREATE_DEVFILE "[ -e /dev/cavium_ratelimit ] || mknod /dev/cavium_ratelimit c 244 0"

	DBusMessage* reply;
	DBusError err;
	unsigned int ret = 0;
	
	dbus_error_init( &err );
	
	syslog_cvm_rate (LOG_ERR, "cvm_rate_limit_dbus_service_load\n");
	if((CVM_RATELIMIT_RETURN_CODE_MODULE_NOTRUNNING == tp_module_check()))
	{/* consider as the module already start by manual, try to get the module fd, and major */
		if(tp_after_load_module())
		{
			char cmd[CMD_LINE_LEN] = {0};
			syslog_cvm_rate (LOG_DEBUG, "open cavium_ratelimit failed err %s, module not start,start it now\n",strerror(errno));
			//return -1;
			sprintf(cmd, LOAD_MODULE_CMD);
			ret = system(cmd);
			if(WEXITSTATUS(ret))
			{
				syslog_cvm_rate (LOG_ERR, "traffic-policer module start failed! err %s ret %#x \n", strerror(errno), WEXITSTATUS(ret));
				return -1;
			}
			
			memset(cmd, 0, CMD_LINE_LEN);
			sprintf(cmd, CHECK_OR_CREATE_DEVFILE);
			ret = system(cmd);
			if(WEXITSTATUS(ret))
			{
				syslog_cvm_rate (LOG_ERR, "traffic-policer create device file failed! err %s ret %#x \n", strerror(errno), WEXITSTATUS(ret));
				return -1;
			}
			else
			{
				if(!tp_after_load_module())
				{
					syslog_cvm_rate (LOG_ERR, "tp_after_load_module******CVM_RATELIMIT_RETURN_CODE_SUCCESS\n");
					ret = CVM_RATELIMIT_RETURN_CODE_SUCCESS; /*try to get the module fd, and major */
				}
				else
				{
					syslog_cvm_rate (LOG_ERR, "tp_after_load_module******CVM_RATELIMIT_RETURN_CODE_FAILED\n");
					ret = CVM_RATELIMIT_RETURN_CODE_FAILED;
				}
			}
		}
		else
		{
			syslog_cvm_rate (LOG_DEBUG, "open /dev/cavium_ratelimit success, module already start maybe by manual\n");
			ret = CVM_RATELIMIT_RETURN_CODE_SUCCESS;
		}
	}
	else
	{
		ret = CVM_RATELIMIT_RETURN_CODE_SERVICE_ALREADY_LOAD;
	}
	syslog_cvm_rate (LOG_ERR, "load cvm-rate module ret = %x\n",ret);
	
	reply = dbus_message_new_method_return(msg);
	
	if(NULL == reply)
	{
		return NULL;
	}

    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	
	return reply;
}

DBusMessage * cvm_rate_limit_dbus_service_unload(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	#define CMD_LINE_LEN 256
    DBusMessage* reply;
	DBusError err;
	unsigned char confirm[CMD_LINE_LEN] = {0};
	unsigned char cmd[CMD_LINE_LEN] = {0};
	unsigned int mloaded = 0;
	unsigned int op_ret = 0;
	int i = 0;
	int ret = 1;
	
	syslog_cvm_rate (LOG_ERR, "cvm_rate_limit_dbus_service_unload\n");
	dbus_error_init( &err );

	memset(cmd, 0, CMD_LINE_LEN);
	sprintf(cmd, "sudo lsmod|grep cavium_ratelimit > /dev/null\n");
	op_ret = system(cmd);
	if(op_ret)
	{
		syslog_cvm_rate (LOG_ERR, "%% Traffic-policer module is not started!\n");
		ret = CVM_RATELIMIT_RETURN_CODE_MODULE_NOTRUNNING;
	}
			
	close(ioctlfd);
	ioctlfd = -1;
	memset(cmd, 0, CMD_LINE_LEN);
	sprintf(cmd, "sudo rmmod cavium_ratelimit 2> /dev/null\n");
	op_ret = system(cmd);
	if(!op_ret)
	{
		ret = CVM_RATELIMIT_RETURN_CODE_SUCCESS;
	}
	else 
	{
		ret = CVM_RATELIMIT_RETURN_CODE_FAILED;
	}
	
	reply = dbus_message_new_method_return(msg);
	syslog_cvm_rate (LOG_ERR, "ret = %x\n",ret);
	if(NULL == reply)
	{
		return NULL;
	}
	
    dbus_message_append_args(reply,
							 DBUS_TYPE_INT32,&ret,
							 DBUS_TYPE_INVALID);
	
	return reply;
}

DBusMessage * cvm_rate_limit_dbus_enable_set(DBusConnection *conn, DBusMessage *msg, void *user_data){
	DBusMessage* reply;
	DBusError err;
	unsigned int ret = 0;
	unsigned int value = 0;
	unsigned int flag = 0;
	
	dbus_error_init( &err );
	if(!(dbus_message_get_args(msg,&err,
								DBUS_TYPE_UINT32,&value,
								DBUS_TYPE_UINT32,&flag,
								DBUS_TYPE_INVALID)))
	{
		if(dbus_error_is_set( &err ))
		{
			dbus_error_free( &err );
		}
		return NULL;	
	}
	
	ret = cvm_rate_limit_enable_set(value, flag);
	
	reply = dbus_message_new_method_return(msg);
	
	if(NULL == reply)
	{
		return NULL;
	}
	
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	
	return reply;
}

/* sunhanyu add for add tcp/udp:port rule much easier */
DBusMessage * cvm_rate_limit_dbus_add_rule_simp(DBusConnection *conn, DBusMessage *msg, void *user_data){
	DBusMessage *reply = NULL;
	DBusError err;
	unsigned int ret = 0;
	MATCH_TYPE rules[PACKET_MATCH_BYTE_NUM] = {0};
	MATCH_TYPE mask[PACKET_MATCH_BYTE_NUM] = {0};
	unsigned int limiter = 0;
	unsigned char * namePtr = NULL;
	unsigned int iptype = 0;
	unsigned int sport = 0;
	unsigned int dport = 0;

	struct skhead
	{
		char foo1[12];
		unsigned short int ethiptag;
		unsigned char ipvertag;
		char foo2[8];
		unsigned char ipprotag;
		char foo3[10];
		unsigned short int srcport;
		unsigned short int dstport;
	};

	struct skhead *ruleptr = (struct skhead *) rules;
	struct skhead *maskptr = (struct skhead *) mask;

	dbus_error_init( &err );
	
	if(!(dbus_message_get_args(msg,&err,
								DBUS_TYPE_STRING,&namePtr, 
								DBUS_TYPE_UINT32,&limiter,
								DBUS_TYPE_UINT32,&iptype,
								DBUS_TYPE_UINT32,&sport,
								DBUS_TYPE_UINT32,&dport,
								DBUS_TYPE_INVALID)))
	{
		if(dbus_error_is_set( &err ))
		{
			dbus_error_free( &err );
		}
		return NULL;	
	}
	
	ruleptr->ethiptag = 0x0800;
	maskptr->ethiptag = 0xffff;
	
	ruleptr->ipvertag = 0x45;
	maskptr->ipvertag = 0xff;

	maskptr->ipprotag = 0xff;	
	if(iptype == CVM_RATE_IPTCP)
	{
		ruleptr->ipprotag = 0x06;
	}
	else if(iptype == CVM_RATE_IPUDP)
	{
		ruleptr->ipprotag = 0x11;
	}

	if(sport == CVM_RATE_PORT_ANY)
	{
		maskptr->srcport = 0x0000;
	}
	else
	{
		maskptr->srcport = 0xffff;
		ruleptr->srcport = (unsigned short int) sport;
	}

	if(dport == CVM_RATE_PORT_ANY)
	{
		maskptr->dstport = 0x0000;
	}
	else
	{
		maskptr->dstport = 0xffff;
		ruleptr->dstport = (unsigned short int) dport;
	}

	ret = tp_module_check();
	if(ret)
	{
		goto end;
	}
	ret = cvm_rate_limit_add_rule(namePtr, rules, mask, limiter);
	
	if(ret)
	{
		if(EEXIST == ret)
		{
			ret = CVM_RATELIMIT_RETURN_CODE_RULE_EXIST;
		}
		else if(EFBIG == ret)
		{
			ret = CVM_RATELIMIT_RETURN_CODE_RULE_FULL;
		}
		else if(-2 == ret)
		{
			ret = CVM_RATELIMIT_RETURN_CODE_INVALID_RULE;
		}
		else
		{
			ret = CVM_RATELIMIT_RETURN_CODE_FAILED;
		}
	}
	else if(cvm_rate_limit_rules_load_source)
	{
		/*write the rule to file*/;
	}

end:	
	reply = dbus_message_new_method_return(msg);
	
	if(NULL == reply)
	{
		return NULL;
	}
	
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
		
	return reply;
}


DBusMessage * cvm_rate_limit_dbus_add_rule(DBusConnection *conn, DBusMessage *msg, void *user_data){
	DBusMessage* reply;
	DBusError err;
	unsigned int ret = 0;
	MATCH_TYPE rules[PACKET_MATCH_BYTE_NUM] = {0};
	MATCH_TYPE mask[PACKET_MATCH_BYTE_NUM] = {0};
	unsigned int limiter = 0;
	unsigned char * namePtr = NULL;
	
	dbus_error_init( &err );
	if(!(dbus_message_get_args(msg,&err,
								DBUS_TYPE_STRING,&namePtr,
								DBUS_MATCH_TYPE,&rules[0],
								DBUS_MATCH_TYPE,&rules[1],
								DBUS_MATCH_TYPE,&rules[2],
								DBUS_MATCH_TYPE,&rules[3],
								DBUS_MATCH_TYPE,&rules[4],
								DBUS_MATCH_TYPE,&rules[5],
								DBUS_MATCH_TYPE,&rules[6],
								DBUS_MATCH_TYPE,&rules[7],
								DBUS_MATCH_TYPE,&rules[8],
								DBUS_MATCH_TYPE,&rules[9],
								DBUS_MATCH_TYPE,&rules[10],
								DBUS_MATCH_TYPE,&rules[11],
								DBUS_MATCH_TYPE,&rules[12],
								DBUS_MATCH_TYPE,&rules[13],
								DBUS_MATCH_TYPE,&rules[14],
								DBUS_MATCH_TYPE,&rules[15],
#if 0
								DBUS_TYPE_UINT32,&rules[16],
								DBUS_TYPE_UINT32,&rules[17],
								DBUS_TYPE_UINT32,&rules[18],
								DBUS_TYPE_UINT32,&rules[19],
								DBUS_TYPE_UINT32,&rules[20],
								DBUS_TYPE_UINT32,&rules[21],
								DBUS_TYPE_UINT32,&rules[22],
								DBUS_TYPE_UINT32,&rules[23],
								DBUS_TYPE_UINT32,&rules[24],
								DBUS_TYPE_UINT32,&rules[25],
								DBUS_TYPE_UINT32,&rules[26],
								DBUS_TYPE_UINT32,&rules[27],
								DBUS_TYPE_UINT32,&rules[28],
								DBUS_TYPE_UINT32,&rules[29],
								DBUS_TYPE_UINT32,&rules[30],
								DBUS_TYPE_UINT32,&rules[31],
#endif
								DBUS_MATCH_TYPE,&mask[0],
								DBUS_MATCH_TYPE,&mask[1],
								DBUS_MATCH_TYPE,&mask[2],
								DBUS_MATCH_TYPE,&mask[3],
								DBUS_MATCH_TYPE,&mask[4],
								DBUS_MATCH_TYPE,&mask[5],
								DBUS_MATCH_TYPE,&mask[6],
								DBUS_MATCH_TYPE,&mask[7],
								DBUS_MATCH_TYPE,&mask[8],
								DBUS_MATCH_TYPE,&mask[9],
								DBUS_MATCH_TYPE,&mask[10],
								DBUS_MATCH_TYPE,&mask[11],
								DBUS_MATCH_TYPE,&mask[12],
								DBUS_MATCH_TYPE,&mask[13],
								DBUS_MATCH_TYPE,&mask[14],
								DBUS_MATCH_TYPE,&mask[15],
#if 0
								DBUS_TYPE_UINT32,&mask[16],
								DBUS_TYPE_UINT32,&mask[17],
								DBUS_TYPE_UINT32,&mask[18],
								DBUS_TYPE_UINT32,&mask[19],
								DBUS_TYPE_UINT32,&mask[20],
								DBUS_TYPE_UINT32,&mask[21],
								DBUS_TYPE_UINT32,&mask[22],
								DBUS_TYPE_UINT32,&mask[23],
								DBUS_TYPE_UINT32,&mask[24],
								DBUS_TYPE_UINT32,&mask[25],
								DBUS_TYPE_UINT32,&mask[26],
								DBUS_TYPE_UINT32,&mask[27],
								DBUS_TYPE_UINT32,&mask[28],
								DBUS_TYPE_UINT32,&mask[29],
								DBUS_TYPE_UINT32,&mask[30],
								DBUS_TYPE_UINT32,&mask[31],
#endif
								DBUS_TYPE_UINT32,&limiter,
								DBUS_TYPE_INVALID)))
	{
		if(dbus_error_is_set( &err ))
		{
			dbus_error_free( &err );
		}
		return NULL;	
	}
	
	ret = tp_module_check();
	if(ret)
	{
		goto end;
	}
	
	ret = cvm_rate_limit_add_rule(namePtr, rules, mask, limiter);
	if(ret)
	{
		if(EEXIST == ret)
		{
			ret = CVM_RATELIMIT_RETURN_CODE_RULE_EXIST;
		}
		else if(EFBIG == ret)
		{
			ret = CVM_RATELIMIT_RETURN_CODE_RULE_FULL;
		}
		else if(-2 == ret)
		{
			ret = CVM_RATELIMIT_RETURN_CODE_INVALID_RULE;
		}
		else
		{
			ret = CVM_RATELIMIT_RETURN_CODE_FAILED;
		}
	}
	else if(cvm_rate_limit_rules_load_source)
	{
		/*write the rule to file*/;
	}
end:
	reply = dbus_message_new_method_return(msg);
	
	if(NULL == reply)
	{
		return NULL;
	}
	
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	
	return reply;
}

DBusMessage * cvm_rate_limit_dbus_config_rules_source(DBusConnection *conn, DBusMessage *msg, void *user_data){
	DBusMessage* reply;
	DBusError err;
	unsigned int ret = 0;
	unsigned int source = 0;
	FILE * fd = NULL;
	int i = 0;
	struct cr_ioctl_struct rule_struct;
	
	dbus_error_init( &err );	
	if(!(dbus_message_get_args(msg,&err,
								DBUS_TYPE_UINT32,&source,
								DBUS_TYPE_INVALID)))
	{
		if(dbus_error_is_set( &err ))
		{
			dbus_error_free( &err );
		}
		return NULL;	
	}
	cvm_rate_limit_rules_load_source = source;
	if(source){
		
		ret = tp_module_check();
		if(ret)
		{
			goto end;
		}
		/*clear all corrent udf rules , then load from file */
		ret = ioctl(ioctlfd, IOCTLCMD(CRCLEARUDFRULES), &rule_struct);
		if(ret < 0)
		{
			syslog_cvm_rate(LOG_ERR, "clear all udf rules failed : %s\n", strerror(errno));
		}
		ret = cvm_rate_limit_load_rules_from_file();
		ret = CVM_RATELIMIT_RETURN_CODE_SUCCESS;/*ignore no config file error */
	}
end:
	reply = dbus_message_new_method_return(msg);
	
	if(NULL == reply)
	{
		return NULL;
	}
	
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	
	return reply;
}

DBusMessage * cvm_rate_limit_dbus_load_rules_from_file(DBusConnection *conn, DBusMessage *msg, void *user_data){
	DBusMessage* reply;
	DBusError err;
	unsigned int ret = 0;
	dbus_error_init( &err );

	ret = tp_module_check();
	if(ret)
	{
		goto end;
	}

	ret = cvm_rate_limit_load_rules_from_file();
end:
	reply = dbus_message_new_method_return(msg);
	
	if(NULL == reply)
	{
		return NULL;
	}
	
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	
	return reply;
}

DBusMessage * cvm_rate_limit_dbus_restore_rules_to_file(DBusConnection *conn, DBusMessage *msg, void *user_data){
	DBusMessage* reply;
	DBusError err;
	unsigned int ret = 0;
	
	dbus_error_init( &err );

	ret = tp_module_check();
	if(ret)
	{
		goto end;
	}

	ret = cvm_rate_limit_restore_rules_to_file();
end:
	reply = dbus_message_new_method_return(msg);
	
	if(NULL == reply)
	{
		return NULL;
	}
	
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	
	return reply;
}


DBusMessage * cvm_rate_limit_dbus_modify_rule(DBusConnection *conn, DBusMessage *msg, void *user_data){
	DBusMessage* reply;
	DBusError err;
	unsigned int ret = 0;
	MATCH_TYPE rules[PACKET_MATCH_BYTE_NUM] = {0};
	MATCH_TYPE mask[PACKET_MATCH_BYTE_NUM] = {0};
	unsigned int limiter = 0;
	unsigned char * namePtr = NULL;
	unsigned int type = 0;
	
	dbus_error_init( &err );
	if(!(dbus_message_get_args(msg,&err,
								DBUS_TYPE_STRING,&namePtr,
								DBUS_TYPE_UINT32,&type,
								DBUS_MATCH_TYPE,&rules[0],
								DBUS_MATCH_TYPE,&rules[1],
								DBUS_MATCH_TYPE,&rules[2],
								DBUS_MATCH_TYPE,&rules[3],
								DBUS_MATCH_TYPE,&rules[4],
								DBUS_MATCH_TYPE,&rules[5],
								DBUS_MATCH_TYPE,&rules[6],
								DBUS_MATCH_TYPE,&rules[7],
								DBUS_MATCH_TYPE,&rules[8],
								DBUS_MATCH_TYPE,&rules[9],
								DBUS_MATCH_TYPE,&rules[10],
								DBUS_MATCH_TYPE,&rules[11],
								DBUS_MATCH_TYPE,&rules[12],
								DBUS_MATCH_TYPE,&rules[13],
								DBUS_MATCH_TYPE,&rules[14],
								DBUS_MATCH_TYPE,&rules[15],
#if 0
								DBUS_TYPE_UINT32,&rules[16],
								DBUS_TYPE_UINT32,&rules[17],
								DBUS_TYPE_UINT32,&rules[18],
								DBUS_TYPE_UINT32,&rules[19],
								DBUS_TYPE_UINT32,&rules[20],
								DBUS_TYPE_UINT32,&rules[21],
								DBUS_TYPE_UINT32,&rules[22],
								DBUS_TYPE_UINT32,&rules[23],
								DBUS_TYPE_UINT32,&rules[24],
								DBUS_TYPE_UINT32,&rules[25],
								DBUS_TYPE_UINT32,&rules[26],
								DBUS_TYPE_UINT32,&rules[27],
								DBUS_TYPE_UINT32,&rules[28],
								DBUS_TYPE_UINT32,&rules[29],
								DBUS_TYPE_UINT32,&rules[30],
								DBUS_TYPE_UINT32,&rules[31],
#endif
								DBUS_TYPE_UINT32,&limiter,
								DBUS_TYPE_INVALID)))
	{
		if(dbus_error_is_set( &err ))
		{
			dbus_error_free( &err );
		}
		return NULL;	
	}

	ret = tp_module_check();
	if(ret)
	{
		goto end;
	}

	ret = cvm_rate_limit_modify_rule(namePtr, type, rules, limiter);
	if(ret)
	{
		if(ENOENT == ret)
		{
			ret = CVM_RATELIMIT_RETURN_CODE_RULE_NOTEXIST;
		}
		else if(EPERM == ret)
		{
			ret = CVM_RATELIMIT_RETURN_CODE_UNSUPPORT;
		}
		else if(-2 == ret)
		{
			ret = CVM_RATELIMIT_RETURN_CODE_INVALID_RULE;
		}
		else
		{
			ret = CVM_RATELIMIT_RETURN_CODE_FAILED;
		}
	}
	else if(cvm_rate_limit_rules_load_source)
	{
		/*find and update the rule to file*/;
	}
end:
	reply = dbus_message_new_method_return(msg);
	
	if(NULL == reply)
	{
		return NULL;
	}
	
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	
	return reply;
}


DBusMessage * cvm_rate_limit_dbus_del_rule(DBusConnection *conn, DBusMessage *msg, void *user_data){
	DBusMessage* reply;
	DBusError err;
	unsigned int ret = 0;
	unsigned char * namePtr = NULL;
	
	dbus_error_init( &err );
	if(!(dbus_message_get_args(msg,&err,
								DBUS_TYPE_STRING,&namePtr,
								DBUS_TYPE_INVALID)))
	{
		if(dbus_error_is_set( &err ))
		{
			dbus_error_free( &err );
		}
		return NULL;	
	}
	
	ret = tp_module_check();
	if(ret)
	{
		goto end;
	}
	
	ret = cvm_rate_limit_del_rule(namePtr);
	if(ret)
	{
		if(ENOENT == ret)
		{
			ret = CVM_RATELIMIT_RETURN_CODE_RULE_NOTEXIST;
		}
		else if(EPERM == ret)
		{
			ret = CVM_RATELIMIT_RETURN_CODE_UNSUPPORT;
		}
		else
		{
			ret = CVM_RATELIMIT_RETURN_CODE_FAILED;
		}
	}
	else if(cvm_rate_limit_rules_load_source)
	{
		/*find and delete the rule from file*/;
	}
end:
	reply = dbus_message_new_method_return(msg);
	
	if(NULL == reply)
	{
		return NULL;
	}
	
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	
	return reply;
}

DBusMessage * cvm_rate_limit_dbus_clear_rule(DBusConnection *conn, DBusMessage *msg, void *user_data){
	DBusMessage* reply;
	DBusError err;
	unsigned int ret = 0;
	
	dbus_error_init( &err );
	
	ret = tp_module_check();
	if(ret)
	{
		goto end;
	}
	ret = cvm_rate_limit_clear_rule();
	
	if(ret)
	{
		if(ENOENT == ret)
		{
			ret = CVM_RATELIMIT_RETURN_CODE_RULE_NOTEXIST;
		}
		else if(EPERM == ret)
		{
			ret = CVM_RATELIMIT_RETURN_CODE_UNSUPPORT;
		}
		else
		{
			ret = CVM_RATELIMIT_RETURN_CODE_FAILED;
		}
	}
	else if(cvm_rate_limit_rules_load_source)
	{
		/*find and clear the rule from file*/;
	}
end:
	reply = dbus_message_new_method_return(msg);
	
	if(NULL == reply)
	{
		return NULL;
	}
	
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	
	return reply;
}

DBusMessage * cvm_rate_limit_dbus_show_rules(DBusConnection *conn, DBusMessage *msg, void *user_data){

	DBusMessage* reply;
	DBusError err;
	DBusMessageIter		iter;
	DBusMessageIter		iter_array;
	unsigned int ret = 0;
	unsigned int limiter = 0;
	unsigned char *namePtr = NULL;
	protocol_match_item_t rule_struct;
	int i = 0;
	int j = 0;
	int flag = 0;

	memset(&rule_struct, 0, sizeof(rule_struct));
	dbus_error_init( &err );
	if(!(dbus_message_get_args(msg,&err,
								DBUS_TYPE_STRING,&namePtr,
								DBUS_TYPE_UINT32,&flag,
								DBUS_TYPE_INVALID)))
	{
		if(dbus_error_is_set( &err ))
		{
			dbus_error_free( &err );
		}
		return NULL;	
	}
	reply = dbus_message_new_method_return(msg);
	
	if(NULL == reply)
	{
		return NULL;
	}
	
	dbus_message_iter_init_append (reply, &iter);
	if(strcmp(namePtr, ""))
	{		
		ret = tp_module_check();
		if(!ret)			
		{
			ret = cvm_rate_limit_get_rules(namePtr, &rule_struct, flag);
			if(ret)
			{
				ret = CVM_RATELIMIT_RETURN_CODE_FAILED;
			}
			else
			{
				ret = CVM_RATELIMIT_RETURN_CODE_SUCCESS;
			}
		}
		namePtr = rule_struct.name;
		
		//if(!ret)
		{
			DBusMessageIter iter_struct;
			dbus_message_iter_open_container (&iter,
							   DBUS_TYPE_ARRAY,
								  DBUS_STRUCT_BEGIN_CHAR_AS_STRING /*begin*/
										DBUS_TYPE_UINT32_AS_STRING /*ret*/
										DBUS_TYPE_STRING_AS_STRING /*name*/
										DBUS_TYPE_UINT32_AS_STRING /*rule_length*/
										DBUS_MATCH_TYPE_AS_STRING /*rule[0]*/
										DBUS_MATCH_TYPE_AS_STRING /*mask[0]*/
										DBUS_MATCH_TYPE_AS_STRING /*rule[1]*/
										DBUS_MATCH_TYPE_AS_STRING /*mask[1]*/
										DBUS_MATCH_TYPE_AS_STRING /*rule[2]*/
										DBUS_MATCH_TYPE_AS_STRING /*mask[2]*/
										DBUS_MATCH_TYPE_AS_STRING /*rule[3]*/
										DBUS_MATCH_TYPE_AS_STRING /*mask[3]*/
										DBUS_MATCH_TYPE_AS_STRING /*rule[4]*/
										DBUS_MATCH_TYPE_AS_STRING /*mask[4]*/
										DBUS_MATCH_TYPE_AS_STRING /*rule[5]*/
										DBUS_MATCH_TYPE_AS_STRING /*mask[5]*/
										DBUS_MATCH_TYPE_AS_STRING /*rule[6]*/
										DBUS_MATCH_TYPE_AS_STRING /*mask[6]*/
										DBUS_MATCH_TYPE_AS_STRING /*rule[7]*/
										DBUS_MATCH_TYPE_AS_STRING /*mask[7]*/
										DBUS_MATCH_TYPE_AS_STRING /*rule[8]*/
										DBUS_MATCH_TYPE_AS_STRING /*mask[8]*/
										DBUS_MATCH_TYPE_AS_STRING /*rule[9]*/
										DBUS_MATCH_TYPE_AS_STRING /*mask[9]*/
										DBUS_MATCH_TYPE_AS_STRING /*rule[10]*/
										DBUS_MATCH_TYPE_AS_STRING /*mask[10]*/
										DBUS_MATCH_TYPE_AS_STRING /*rule[11]*/
										DBUS_MATCH_TYPE_AS_STRING /*mask[11]*/
										DBUS_MATCH_TYPE_AS_STRING /*rule[12]*/
										DBUS_MATCH_TYPE_AS_STRING /*mask[12]*/
										DBUS_MATCH_TYPE_AS_STRING /*rule[13]*/
										DBUS_MATCH_TYPE_AS_STRING /*mask[13]*/
										DBUS_MATCH_TYPE_AS_STRING /*rule[14]*/
										DBUS_MATCH_TYPE_AS_STRING /*mask[14]*/
										DBUS_MATCH_TYPE_AS_STRING /*rule[15]*/
										DBUS_MATCH_TYPE_AS_STRING /*mask[15]*/
#if 0
										DBUS_TYPE_UINT32_AS_STRING /*rule[16]*/
										DBUS_TYPE_UINT32_AS_STRING /*mask[16]*/
										DBUS_TYPE_UINT32_AS_STRING /*rule[17]*/
										DBUS_TYPE_UINT32_AS_STRING /*mask[17]*/
										DBUS_TYPE_UINT32_AS_STRING /*rule[18]*/
										DBUS_TYPE_UINT32_AS_STRING /*mask[18]*/
										DBUS_TYPE_UINT32_AS_STRING /*rule[19]*/
										DBUS_TYPE_UINT32_AS_STRING /*mask[19]*/
										DBUS_TYPE_UINT32_AS_STRING /*rule[20]*/
										DBUS_TYPE_UINT32_AS_STRING /*mask[20]*/
										DBUS_TYPE_UINT32_AS_STRING /*rule[21]*/
										DBUS_TYPE_UINT32_AS_STRING /*mask[21]*/
										DBUS_TYPE_UINT32_AS_STRING /*rule[22]*/
										DBUS_TYPE_UINT32_AS_STRING /*mask[22]*/
										DBUS_TYPE_UINT32_AS_STRING /*rule[23]*/
										DBUS_TYPE_UINT32_AS_STRING /*mask[23]*/
										DBUS_TYPE_UINT32_AS_STRING /*rule[24]*/
										DBUS_TYPE_UINT32_AS_STRING /*mask[24]*/
										DBUS_TYPE_UINT32_AS_STRING /*rule[25]*/
										DBUS_TYPE_UINT32_AS_STRING /*mask[25]*/
										DBUS_TYPE_UINT32_AS_STRING /*rule[26]*/
										DBUS_TYPE_UINT32_AS_STRING /*mask[26]*/
										DBUS_TYPE_UINT32_AS_STRING /*rule[27]*/
										DBUS_TYPE_UINT32_AS_STRING /*mask[27]*/
										DBUS_TYPE_UINT32_AS_STRING /*rule[28]*/
										DBUS_TYPE_UINT32_AS_STRING /*mask[28]*/
										DBUS_TYPE_UINT32_AS_STRING /*rule[29]*/
										DBUS_TYPE_UINT32_AS_STRING /*mask[29]*/
										DBUS_TYPE_UINT32_AS_STRING /*rule[30]*/
										DBUS_TYPE_UINT32_AS_STRING /*mask[30]*/
										DBUS_TYPE_UINT32_AS_STRING /*rule[31]*/
										DBUS_TYPE_UINT32_AS_STRING /*mask[31]*/
#endif
										DBUS_TYPE_UINT32_AS_STRING /*ratelimiter*/
										DBUS_STATISTIC_TYPE_AS_STRING /*drop_counter*/
										DBUS_STATISTIC_TYPE_AS_STRING /*pass_counter*/
								  DBUS_STRUCT_END_CHAR_AS_STRING, /*end*/
							   &iter_array);
			dbus_message_iter_open_container (&iter_array,
										   DBUS_TYPE_STRUCT,
										   NULL,
										   &iter_struct);
			dbus_message_iter_append_basic (&iter_struct,
								 DBUS_TYPE_UINT32,
								 &ret);
			dbus_message_iter_append_basic (&iter_struct,
								 DBUS_TYPE_STRING,
								 &namePtr);
			dbus_message_iter_append_basic (&iter_struct,
								 DBUS_TYPE_UINT32,
								 &rule_struct.rules_length);
			for(j = 0; j < PACKET_MATCH_BYTE_NUM; j++)
			{
				
				dbus_message_iter_append_basic (&iter_struct,
								 DBUS_MATCH_TYPE,
								 &rule_struct.protocol_match_rule[j]);
				dbus_message_iter_append_basic (&iter_struct,
								 DBUS_MATCH_TYPE,
								 &rule_struct.protocol_match_mask[j]);
			}
			dbus_message_iter_append_basic (&iter_struct,
								 DBUS_TYPE_UINT32,
								 &rule_struct.rateLimit);
			dbus_message_iter_append_basic (&iter_struct,
								 DBUS_STATISTIC_TYPE,
								 &rule_struct.drop_counter);
			dbus_message_iter_append_basic (&iter_struct,
								 DBUS_STATISTIC_TYPE,
								 &rule_struct.pass_counter);
			dbus_message_iter_close_container (&iter_array, &iter_struct);
			dbus_message_iter_close_container (&iter, &iter_array);
		}
	}
	else
	{
		
		dbus_message_iter_open_container (&iter,
					   DBUS_TYPE_ARRAY,
						  DBUS_STRUCT_BEGIN_CHAR_AS_STRING /*begin*/
								DBUS_TYPE_UINT32_AS_STRING /*ret*/
								DBUS_TYPE_STRING_AS_STRING /*name*/
								DBUS_TYPE_UINT32_AS_STRING /*rule_length*/
								DBUS_MATCH_TYPE_AS_STRING /*rule[0]*/
								DBUS_MATCH_TYPE_AS_STRING /*mask[0]*/
								DBUS_MATCH_TYPE_AS_STRING /*rule[1]*/
								DBUS_MATCH_TYPE_AS_STRING /*mask[1]*/
								DBUS_MATCH_TYPE_AS_STRING /*rule[2]*/
								DBUS_MATCH_TYPE_AS_STRING /*mask[2]*/
								DBUS_MATCH_TYPE_AS_STRING /*rule[3]*/
								DBUS_MATCH_TYPE_AS_STRING /*mask[3]*/
								DBUS_MATCH_TYPE_AS_STRING /*rule[4]*/
								DBUS_MATCH_TYPE_AS_STRING /*mask[4]*/
								DBUS_MATCH_TYPE_AS_STRING /*rule[5]*/
								DBUS_MATCH_TYPE_AS_STRING /*mask[5]*/
								DBUS_MATCH_TYPE_AS_STRING /*rule[6]*/
								DBUS_MATCH_TYPE_AS_STRING /*mask[6]*/
								DBUS_MATCH_TYPE_AS_STRING /*rule[7]*/
								DBUS_MATCH_TYPE_AS_STRING /*mask[7]*/
								DBUS_MATCH_TYPE_AS_STRING /*rule[8]*/
								DBUS_MATCH_TYPE_AS_STRING /*mask[8]*/
								DBUS_MATCH_TYPE_AS_STRING /*rule[9]*/
								DBUS_MATCH_TYPE_AS_STRING /*mask[9]*/
								DBUS_MATCH_TYPE_AS_STRING /*rule[10]*/
								DBUS_MATCH_TYPE_AS_STRING /*mask[10]*/
								DBUS_MATCH_TYPE_AS_STRING /*rule[11]*/
								DBUS_MATCH_TYPE_AS_STRING /*mask[11]*/
								DBUS_MATCH_TYPE_AS_STRING /*rule[12]*/
								DBUS_MATCH_TYPE_AS_STRING /*mask[12]*/
								DBUS_MATCH_TYPE_AS_STRING /*rule[13]*/
								DBUS_MATCH_TYPE_AS_STRING /*mask[13]*/
								DBUS_MATCH_TYPE_AS_STRING /*rule[14]*/
								DBUS_MATCH_TYPE_AS_STRING /*mask[14]*/
								DBUS_MATCH_TYPE_AS_STRING /*rule[15]*/
								DBUS_MATCH_TYPE_AS_STRING /*mask[15]*/
#if 0
								DBUS_TYPE_UINT32_AS_STRING /*rule[16]*/
								DBUS_TYPE_UINT32_AS_STRING /*mask[16]*/
								DBUS_TYPE_UINT32_AS_STRING /*rule[17]*/
								DBUS_TYPE_UINT32_AS_STRING /*mask[17]*/
								DBUS_TYPE_UINT32_AS_STRING /*rule[18]*/
								DBUS_TYPE_UINT32_AS_STRING /*mask[18]*/
								DBUS_TYPE_UINT32_AS_STRING /*rule[19]*/
								DBUS_TYPE_UINT32_AS_STRING /*mask[19]*/
								DBUS_TYPE_UINT32_AS_STRING /*rule[20]*/
								DBUS_TYPE_UINT32_AS_STRING /*mask[20]*/
								DBUS_TYPE_UINT32_AS_STRING /*rule[21]*/
								DBUS_TYPE_UINT32_AS_STRING /*mask[21]*/
								DBUS_TYPE_UINT32_AS_STRING /*rule[22]*/
								DBUS_TYPE_UINT32_AS_STRING /*mask[22]*/
								DBUS_TYPE_UINT32_AS_STRING /*rule[23]*/
								DBUS_TYPE_UINT32_AS_STRING /*mask[23]*/
								DBUS_TYPE_UINT32_AS_STRING /*rule[24]*/
								DBUS_TYPE_UINT32_AS_STRING /*mask[24]*/
								DBUS_TYPE_UINT32_AS_STRING /*rule[25]*/
								DBUS_TYPE_UINT32_AS_STRING /*mask[25]*/
								DBUS_TYPE_UINT32_AS_STRING /*rule[26]*/
								DBUS_TYPE_UINT32_AS_STRING /*mask[26]*/
								DBUS_TYPE_UINT32_AS_STRING /*rule[27]*/
								DBUS_TYPE_UINT32_AS_STRING /*mask[27]*/
								DBUS_TYPE_UINT32_AS_STRING /*rule[28]*/
								DBUS_TYPE_UINT32_AS_STRING /*mask[28]*/
								DBUS_TYPE_UINT32_AS_STRING /*rule[29]*/
								DBUS_TYPE_UINT32_AS_STRING /*mask[29]*/
								DBUS_TYPE_UINT32_AS_STRING /*rule[30]*/
								DBUS_TYPE_UINT32_AS_STRING /*mask[30]*/
								DBUS_TYPE_UINT32_AS_STRING /*rule[31]*/
								DBUS_TYPE_UINT32_AS_STRING /*mask[31]*/
#endif
								DBUS_TYPE_UINT32_AS_STRING /*ratelimiter*/
								DBUS_STATISTIC_TYPE_AS_STRING /*drop_counter*/
								DBUS_STATISTIC_TYPE_AS_STRING /*pass_counter*/
						  DBUS_STRUCT_END_CHAR_AS_STRING, /*end*/
					   &iter_array);
		for(i = 0; i < MAX_MATCH_RULES_NUM; i++)
		{	
			ret = tp_module_check();
			if(!ret)			
			{
				ret = cvm_rate_limit_get_rules_byindex(i, &rule_struct, flag);
				if(ret)
				{
					ret = CVM_RATELIMIT_RETURN_CODE_FAILED;
				}
				else
				{
					ret = CVM_RATELIMIT_RETURN_CODE_SUCCESS;
				}
			}
			namePtr = rule_struct.name;
			
			//if(!ret)
			{
			
				DBusMessageIter iter_struct;
				dbus_message_iter_open_container (&iter_array,
											   DBUS_TYPE_STRUCT,
											   NULL,
											   &iter_struct);
				dbus_message_iter_append_basic (&iter_struct,
									 DBUS_TYPE_UINT32,
									 &ret);
				dbus_message_iter_append_basic (&iter_struct,
									 DBUS_TYPE_STRING,
									 &namePtr);
				dbus_message_iter_append_basic (&iter_struct,
								 	 DBUS_TYPE_UINT32,
								 	 &rule_struct.rules_length);
				for(j = 0; j < PACKET_MATCH_BYTE_NUM; j++)
				{
					
					dbus_message_iter_append_basic (&iter_struct,
									 DBUS_MATCH_TYPE,
									 &rule_struct.protocol_match_rule[j]);
					dbus_message_iter_append_basic (&iter_struct,
									 DBUS_MATCH_TYPE,
									 &rule_struct.protocol_match_mask[j]);
				}
				dbus_message_iter_append_basic (&iter_struct,
									 DBUS_TYPE_UINT32,
									 &rule_struct.rateLimit);
				dbus_message_iter_append_basic (&iter_struct,
									 DBUS_STATISTIC_TYPE,
									 &rule_struct.drop_counter);
				dbus_message_iter_append_basic (&iter_struct,
									 DBUS_STATISTIC_TYPE,
									 &rule_struct.pass_counter);
				dbus_message_iter_close_container (&iter_array, &iter_struct);
			}
			if(CVM_RATELIMIT_RETURN_CODE_MODULE_NOTRUNNING == ret)
			{
				break;
			}
		}
		dbus_message_iter_close_container (&iter, &iter_array);
	}
	
	return reply;
}

DBusMessage * cvm_rate_limit_dbus_show_statistic(DBusConnection *conn, DBusMessage *msg, void *user_data){
	DBusMessage* reply;
	DBusError err;
	DBusMessageIter		iter;
	DBusMessageIter		iter_array;
	int ret = 0;
	struct cr_ioctl_struct rule_struct;
	unsigned int flag = 0;

	memset(&rule_struct, 0, sizeof(rule_struct));
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
	syslog_cvm_rate(LOG_DEBUG, "show cvm ratelimit statistic flag %d\n", flag);
	rule_struct.u.flag = flag;
	
	ret = tp_module_check();
	if(ret)
	{
		goto end;
	}
	ret = ioctl(ioctlfd, IOCTLCMD(CRSTACOUNTGET), &rule_struct);
	if(ret < 0)
	{
		syslog_cvm_rate(LOG_ERR, "statistic get ioctl failed error : %s\n",strerror(errno));
		ret = CVM_RATELIMIT_RETURN_CODE_FAILED;
	}
end:
	reply = dbus_message_new_method_return(msg);
	
	if(NULL == reply)
	{
		return NULL;
	}
	
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_STATISTIC_TYPE,&rule_struct.u.c.drop_counter,
							 DBUS_STATISTIC_TYPE,&rule_struct.u.c.pass_counter,
							 DBUS_TYPE_INVALID);
	return reply;
}

DBusMessage * cvm_rate_limit_dbus_clear_statistic(DBusConnection *conn, DBusMessage *msg, void *user_data){
	DBusMessage* reply;
	DBusError err;
	DBusMessageIter		iter;
	DBusMessageIter		iter_array;
	int ret = 0;
	struct cr_ioctl_struct rule_struct;
	unsigned int flag = 0;

	memset(&rule_struct, 0, sizeof(rule_struct));
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
	rule_struct.u.flag = flag;
	
	ret = tp_module_check();
	if(ret)
	{
		goto end;
	}
	syslog_cvm_rate(LOG_DEBUG, "clear cvm ratelimit statistic flag %d rule_struct.u.flag %d\n", flag, rule_struct.u.flag);
	ret = ioctl(ioctlfd, IOCTLCMD(CRSTACOUNTCLEAR), &rule_struct);
	if(ret < 0)
	{
		syslog_cvm_rate(LOG_ERR, "statistic clear ioctl failed error : %s\n",strerror(errno));
		ret = CVM_RATELIMIT_RETURN_CODE_FAILED;
	}
end:
	reply = dbus_message_new_method_return(msg);
	
	if(NULL == reply)
	{
		return NULL;
	}
	
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_STATISTIC_TYPE,&rule_struct.u.c.drop_counter,
							 DBUS_STATISTIC_TYPE,&rule_struct.u.c.pass_counter,
							 DBUS_TYPE_INVALID);
	return reply;
}

DBusMessage * cvm_rate_limit_dbus_dmesg_enable_set(DBusConnection *conn, DBusMessage *msg, void *user_data){
	DBusMessage* reply;
	DBusError err;
	DBusMessageIter		iter;
	DBusMessageIter		iter_array;
	int ret = 0;
	unsigned int enable = 0;
	struct cr_ioctl_struct rule_struct;

	memset(&rule_struct, 0, sizeof(rule_struct));
	dbus_error_init( &err );
	if(!(dbus_message_get_args(msg,&err,
								DBUS_TYPE_UINT32,&enable,
								DBUS_TYPE_INVALID)))
	{
		if(dbus_error_is_set( &err ))
		{
			dbus_error_free( &err );
		}
		return NULL;	
	}	
	rule_struct.u.enable = enable;
	
	ret = tp_module_check();
	if(ret)
	{
		goto end;
	}
	ret = ioctl(ioctlfd, IOCTLCMD(CRDMESGENABLESET), &rule_struct);
	if(ret < 0)
	{
		syslog_cvm_rate(LOG_ERR, "statistic clear ioctl failed error : %s\n",strerror(errno));
		ret = CVM_RATELIMIT_RETURN_CODE_FAILED;
	}
end:
	reply = dbus_message_new_method_return(msg);
	
	if(NULL == reply)
	{
		return NULL;
	}
	
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	return reply;
}

DBusMessage * cvm_rate_limit_dbus_show_running(DBusConnection *conn, DBusMessage *msg, void *user_data){
#define SHOW_RUNNING_STRLEN _1K*8
#define TEMP_STR_LEN 256
#define PARSE_TYPE unsigned int
	DBusMessage* reply;
	DBusError err;
	DBusMessageIter		iter;
	DBusMessageIter		iter_array;
	protocol_match_item_t rule_struct;
	int ret = 0;
	int i = 0;
	int j = 0;
	int offset = 0;
	unsigned char *showStr = NULL;
	struct cr_ioctl_struct rule_struct2;
	unsigned char ruleStr[TEMP_STR_LEN] = {0};
	unsigned char maskStr[TEMP_STR_LEN] = {0};
	unsigned char limitStr[TEMP_STR_LEN] = {0};
	unsigned int type_rule_len = 0;
	int len = 0;
	unsigned int slot_id;
	unsigned int * masks_ptr = NULL;
	unsigned int * rules_ptr = NULL;
	/*syslog_cvm_rate(LOG_DEBUG, "entering %s\n", __func__);*/

	dbus_error_init( &err );
	
	if(!(dbus_message_get_args(msg,&err,
								DBUS_TYPE_UINT32,&slot_id,
								DBUS_TYPE_INVALID)))
	{
		if(dbus_error_is_set( &err ))
		{
			dbus_error_free( &err );
		}
		return NULL;	
	}

	reply = dbus_message_new_method_return(msg);
	
	if(NULL == reply)
	{
		return NULL;
	}
	
	dbus_message_iter_init_append (reply, &iter);
	showStr = malloc(SHOW_RUNNING_STRLEN);
	memset(showStr, 0, SHOW_RUNNING_STRLEN);

	memset(&rule_struct2, 0, sizeof(rule_struct2));

	ret = tp_module_check();
	if(ret)
	{
		goto end;
	}
	offset += sprintf(offset + showStr,"traffic-policer service slot %d load\n",slot_id);

	for(i = 0; i < CVM_RATE_LIMIT_DEFAULT_RULE_NUM; i++)
	{
		memset(&rule_struct2, 0, sizeof(rule_struct2));
		rule_struct2.u.index = i;
		ret = ioctl(ioctlfd, IOCTLCMD(CRCHECKDEFAULT), &rule_struct2);
		if(ret < 0)
		{
			continue;
		}
		memset(limitStr, 0, TEMP_STR_LEN);
		if(rule_struct2.u.protocol_rule.rateLimit < CVM_RATE_NO_LIMIT)
		{
			sprintf(limitStr, "%d", rule_struct2.u.protocol_rule.rateLimit);
		}
		else
		{
			sprintf(limitStr, "nolimit");
		}
		offset += sprintf(offset + showStr,"config traffic-policer-rules slot %d %s limiter %s\n",slot_id, \
			rule_struct2.u.protocol_rule.name, limitStr);
		
	}

	if(cvm_rate_limit_rules_load_source)
	{
		offset += sprintf(offset + showStr, \
				"config traffic-policer-rules load-from slot %d file\n",slot_id);
	}
	else
	{
	
		for(i = CVM_RATE_LIMIT_DEFAULT_RULE_NUM; i < MAX_MATCH_RULES_NUM; i++)
		{	
			memset(&rule_struct, 0, sizeof(rule_struct));
			ret = cvm_rate_limit_get_rules_byindex(i, &rule_struct, 0);
			if(!ret)
			{
				len = 0;
				memset(ruleStr, 0, TEMP_STR_LEN);
				memset(maskStr, 0, TEMP_STR_LEN);
				type_rule_len = (rule_struct.rules_length + \
								sizeof(PARSE_TYPE) - 1)/sizeof(PARSE_TYPE);
				masks_ptr = rule_struct.protocol_match_mask;
				rules_ptr = rule_struct.protocol_match_rule;
				sprintf(len + maskStr, "%x", \
					type_rule_len ? masks_ptr[0] : 0);
				len += sprintf(len + ruleStr, "%x", \
					type_rule_len ? rules_ptr[0] : 0);
				for(j = 1; j < type_rule_len; j++)
				{
					sprintf(len + maskStr, ",%x", masks_ptr[j]);
					len += sprintf(len + ruleStr, ",%x", rules_ptr[j]);;
				}
				memset(limitStr, 0, TEMP_STR_LEN);
				if(rule_struct.rateLimit < CVM_RATE_NO_LIMIT)
				{
					sprintf(limitStr, "%d", rule_struct.rateLimit);
				}
				else
				{
					sprintf(limitStr, "nolimit");
				}
				offset += sprintf(offset + showStr, \
					"add traffic-policer-rules slot %d %s %s %s %s\n",slot_id, \
					rule_struct.name, ruleStr, maskStr, limitStr);
			}
		}
	}

	ret = ioctl(ioctlfd, IOCTLCMD(CRENABLEGET), &rule_struct2);
	/* modified by zhengbo for hide these trigger options */
#if 0
	if((CR_SET_FLAG_CVM_ETH | CR_SET_FLAG_FAST_FWD ) == rule_struct2.u.enable)
	{
		offset += sprintf(offset + showStr,"config traffic-policer slot %d enable\n",slot_id);
	}
	else if(CR_SET_FLAG_CVM_ETH  == rule_struct2.u.enable)
	{
		offset += sprintf(offset + showStr,"config traffic-policer slot %d enable phase2\n",slot_id);
	}
#endif
	if(CR_SET_FLAG_FAST_FWD  == rule_struct2.u.enable ||
		(CR_SET_FLAG_CVM_ETH | CR_SET_FLAG_FAST_FWD ) == rule_struct2.u.enable)
	{
		offset += sprintf(offset + showStr,"config traffic-policer slot %d enable phase1\n",slot_id);
	}

end:
	dbus_message_iter_append_basic(&iter,
								   DBUS_TYPE_STRING,
								   &showStr);

	free(showStr);
	showStr = NULL;
	
	return reply;
}
/**************************************************
 * DESCRIPTION:
 *	main loop of dbus thread
 * 
 *
 **************************************************/
void * cvm_rate_init_dbus_thread_main(void *arg)
{	
	/*
	  * For all OAM method call, synchronous is necessary.
	  * Only signal/event could be asynchronous, it could be sent in other thread.
	  */	
	while (dbus_connection_read_write_dispatch(cvm_rate_limit_dbus_connection,-1)) {
		;
	}
	return NULL;
}
/***************************************************
 * DESCRIPTION:
 * 	main function for dbus init and thread create ...
 *
 *
 ***************************************************/
int main()
{
	int ret = 0;
	
	char *ident = "cvm-rate";
	
	openlog(ident, 0, LOG_DAEMON);
	
	ret = cvm_rate_limit_dbus_init();
	if(FALSE == ret)
	{
		syslog_cvm_rate (LOG_ERR, "dbus init failed\n");
		return -1;
	}

	tp_after_load_module();/*try to get module fd and major*/
#if 0
	ioctlfd = open(CVM_RATELIMIT_DEV,O_RDWR);
	if(ioctlfd < 0)
	{
		syslog_cvm_rate (LOG_ERR, "open cavium_ratelimit failed err %s\n",strerror(errno));
		//return -1;
	}
	else
	{
		syslog_cvm_rate (LOG_DEBUG, "open /dev/cavium_ratelimit success\n");
	}
	if(ioctlfd > 0){
		major_fd = fopen(MAJOR_NUM_PATH, "r");
		if(major_fd)
		{
			fread(buf, BUFF_LEN, 1, major_fd);
			cr_major_num = strtoul(buf, NULL, 0);
			syslog_cvm_rate (LOG_DEBUG, "read major num %d\n", cr_major_num);
			fclose(major_fd);
		}
		else
		{
			syslog_cvm_rate (LOG_DEBUG, "can't open major num file to read\n");
			close(ioctlfd);
			return -1;
		}
	}
#endif
	dbus_thread = (pthread_t *)malloc(sizeof(pthread_t));
	if(!dbus_thread)
	{
		syslog_cvm_rate (LOG_ERR, "cvm rate dbus thread malloc failed!\n");
		if(ioctlfd > 0)
		{
	   		close(ioctlfd);
			ioctlfd = -1;
		}
		return -1;
	}
	pthread_attr_init(&dbus_thread_attr);
	syslog_cvm_rate (LOG_DEBUG, "dbus thread attr init success\n");
	ret = pthread_create(dbus_thread,&dbus_thread_attr,cvm_rate_init_dbus_thread_main,NULL);
	if(0 != ret)
	{
		if(ioctlfd > 0)
		{
	   		close(ioctlfd);
			ioctlfd = -1;
		}
		free(dbus_thread);
		syslog_cvm_rate (LOG_ERR, "cvm rate pthread create failed, ret %d\n",ret);
		return -1;
	}
	syslog_cvm_rate (LOG_DEBUG, "cvm rate pthread create success\n");
	pthread_join(*dbus_thread,NULL);
	syslog_cvm_rate (LOG_DEBUG, "cvm rate limit main function end\n");
	closelog();
	if(ioctlfd > 0)
	{
   		close(ioctlfd);
		ioctlfd = -1;
	}
	free(dbus_thread);
	return 0;
}

#ifdef __cplusplus
}
#endif

