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
* nbm_cpld.c
*
*
* CREATOR:
*		chenb@autelan.com
*
* DESCRIPTION:
*		APIs used in NBM for CPLD related routine.
*
* DATE:
*		02/21/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.60 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dbus/npd/npd_dbus_def.h>
#include <dbus/dbus.h>

#include "sysdef/npd_sysdef.h"
#include "util/npd_list.h"
#include "npd/nam/npd_amapi.h"
#include "npd/nbm/npd_bmapi.h"
#include "nbm_util.h"
#include "nbm_cpld.h"
#include "nbm_log.h"
#include "sysdef/returncode.h"

extern struct chassis_slot_s **chassis_slots;
extern struct system_state_s	systemStateInfo;
static unsigned char shut_down_power = NBM_FALSE;
static unsigned char sys_power_state = 0xA;		/* 0xA is the normal value as bit[1][2][3] by hardware designed */
DBusConnection *nbm_dbus_connection = NULL;
int g_bm_fd = -1;

/**********************************************************************************
 * nbm_shut_down_power_state
 *
 *	Shut down system power
 *
 *	INPUT:
 *		slot - slot number used to access CPLD register
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *
 **********************************************************************************/
void nbm_shut_down_power_state(unsigned char slot)

{	
	bm_op_cpld_args cpld_args = {0};
	enum module_status_e retval = MODULE_STAT_NONE;
	int fd = -1;

	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			syslog_ax_cpld_err("open dev %s error(%d) when shutdown power!\n",NPD_BM_FILE_PATH,fd);
			return ;
		}
		g_bm_fd = fd;
	}
	cpld_args.regAddr = AX_CPLE_TYPE_POWER_OFF_REG;	
	cpld_args.slot    = slot;
	cpld_args.regData = 0x5A;

	retval = ioctl (g_bm_fd,BM_IOC_CPLD_WRITE,&cpld_args);

	if(retval) {
		syslog_ax_cpld_err("nbm write cpld error when shutting down system power!\n");
	}

	return;
}

/**********************************************************************************
 * nbm_get_system_environment_state
 *
 *	Get system environment parameters such as temperature(CPU core and surface) 
 * 	and fan/power state
 *
 *	INPUT:
 *		state - system environment paramters
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 - if no error occurred
 *
 **********************************************************************************/
unsigned int nbm_get_system_environment_state(NPD_ENVI* state)
{
    unsigned char ret = 0;
	/*unsigned int i;*/
	ret = nbm_check_fan_state();  /*the first bit indirect fan state 0 normal ,1 alarm*/
	if(ret==0) {
		state->fan_porwer |= 1;	  /*the first bit indirect fan state 1 normal ,0 alarm*/
	}   

    if(BOARD_TYPE == BOARD_TYPE_AX81_SMU)
    {
        ret = nbm_check_8610_power_state();
        state->fan_porwer |= (ret<<1);
    }
    else
    {
    	ret = nbm_check_power_state();
    	state->fan_porwer |= ((ret==14)<<1);
    }
	#if 0
	//printf("nbm_check_power_state ret = %20x\n",ret);
	for(i=0;i<4;i++){
		if(ret&(1<<i)){    //the second bit indirect power 1 state1 normal ,0 alarm
			state->fan_porwer |= (1<<(i+1));
				}
		else {
			state->fan_porwer |= (0<<(i+1));
		}
	}
	#endif
	nbm_get_sys_temperature(&(state->inter),&(state->surface));
	return 0;
}
/**********************************************************************************
 * nbm_set_system_shutdwon_enable
 *
 *	set if enable to shutdown system or not when critical condition occurred
 *
 *	INPUT:
 *		isenable - binary value to indicate enable or disable
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 - if no error occurred
 *
 **********************************************************************************/
unsigned int nbm_set_system_shutdwon_enable(unsigned short isenable)
{
     shut_down_power = isenable;
	 return 0;
}

/**********************************************************************************
 * nbm_get_board_state
 *
 * read CPLD register to get system board working state
 *
 *	INPUT:
 *		slot	- slot number 
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		MODULE_STAT_NONE - if board is not running.
 *		MODULE_STAT_RUNNING - if board is running.
 *		
 *
 **********************************************************************************/
enum module_status_e nbm_get_board_state
(
	unsigned int slot
)
{	
	bm_op_cpld_args cpld_args = {0};
	int fd = -1;
	enum module_status_e retval = MODULE_STAT_NONE;

/*	if((1 == slot)&&((MODULE_ID_AX7I_CRSMU == nbm_probe_chassis_module_id(slot))||(MODULE_ID_AX7I_GTX == nbm_probe_chassis_module_id(slot)))){
		slot = 0;
	}
*/	
	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			syslog_ax_cpld_err("open dev %s error(%d) when get board state!\n",NPD_BM_FILE_PATH,fd);
			#ifdef DIAG_XCAT
			return MODULE_STAT_RUNNING;
			#else
			return -1;
			#endif
		}
		g_bm_fd = fd;
	}
	cpld_args.regAddr = AX_BOARD_STATE_CPLD_REG;	
	cpld_args.slot = slot;

	retval = ioctl (g_bm_fd,BM_IOC_CPLD_READ,&cpld_args);

	if( 0 == retval ) {
		switch(slot) {
			case 0: /* CRSMU */
			if(0x55 == cpld_args.regData){
				retval = MODULE_STAT_RUNNING;
				cpld_args.regData = 0xaa;
				
				if(-1== ioctl (g_bm_fd,BM_IOC_CPLD_WRITE,&cpld_args)){/* write test value 0xaa*/
					syslog_ax_cpld_err("get CRSMU board state failed when test write 0xaa to cpld\n");
					retval = MODULE_STAT_NONE;
				}
				else if((-1 == ioctl (g_bm_fd,BM_IOC_CPLD_READ,&cpld_args))){  /* read*/
					syslog_ax_cpld_err("get CRSMU board state failed when test read back after write 0xaa to cpld\n");
					retval = MODULE_STAT_NONE;
				}
				else if(0xaa != cpld_args.regData) { /* check written value 0xa*/
					syslog_ax_cpld_err("get CRSMU board state error read back value not match with written value\n");
					retval = MODULE_STAT_NONE;
				}
				else {
					cpld_args.regData = 0x55;

					if (-1==ioctl (g_bm_fd,BM_IOC_CPLD_WRITE,&cpld_args)) { /*write back reset value 0x55*/
						syslog_ax_cpld_err("get CRSMU board state failed when reset cpld\n");
						retval = MODULE_STAT_NONE;
					}
				}
	 		}
			else if(0x5A == cpld_args.regData) {
				/* TODO: need implementation*/
				retval = MODULE_STAT_NONE;
			}
			else {
				retval = MODULE_STAT_NONE;
			}
			break;
			case 1: /* Sub card 1*/
			case 2: /* Sub card 2*/
			case 3:
			case 4:
			if(0x33 == cpld_args.regData){
				retval = MODULE_STAT_RUNNING;
				cpld_args.regData = 0xcc;
				if(-1== ioctl (g_bm_fd,BM_IOC_CPLD_WRITE,&cpld_args)) {/* write test value 0xCC*/
					syslog_ax_cpld_err("get slot %d board state failed when test write 0xcc to cpld\n",slot);
					retval = MODULE_STAT_NONE;
				}
				else if((-1== ioctl (g_bm_fd,BM_IOC_CPLD_READ,&cpld_args))){  /* read*/
					syslog_ax_cpld_err("get slot %d board state failed when test read back after write 0xcc to cpld\n",slot);
					retval = MODULE_STAT_NONE;
				}
				else if(0xcc != cpld_args.regData) {/* check written value 0xcc*/
					syslog_ax_cpld_err("get slot %d board state error read back value not match with written value\n",slot);
					retval = MODULE_STAT_NONE;
				}
				else {
					 cpld_args.regData = 0x33;
					 
					 if (-1==ioctl (g_bm_fd,BM_IOC_CPLD_WRITE,&cpld_args)) { /*write back reset value 0x33*/
							retval = MODULE_STAT_NONE;
							syslog_ax_cpld_err("get slot %d board state failed when reset cpld\n",slot);						
					}
				}
	 		}
			else {
				retval = MODULE_STAT_NONE;
			}
			break;
		}
	}
	else {
		syslog_ax_cpld_err("read slot %d cpld error!\n",slot);
		retval = -1;
	}

	/* modify by qinhs@autelan.com 2009-11-18 for AX7605i-alpha */
	retval = MODULE_STAT_RUNNING;
	return retval;
}
/**********************************************************************************
 * nbm_check_fan_state
 *
 * read CPLD register to get system fan working state
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		SYSTEM_FAN_STAT_NORMAL - if system fan running in normal state.
 *		SYSTEM_FAN_STAT_ALARM - if system fan running in abnormal state.
 *		SYSTEM_FAN_STAT_MAX - if error occurred.
 *
 **********************************************************************************/
enum system_fan_status_e nbm_check_fan_state
(
	void
)
{
	bm_op_cpld_args cpld_args = {0};
	int fd = -1,retval = 0;
	enum system_fan_status_e state = SYSTEM_FAN_STAT_MAX;

	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			syslog_ax_cpld_err("open dev %s error(%d) when check fan state!\n",NPD_BM_FILE_PATH,fd);
			return SYSTEM_FAN_STAT_MAX;
		}
		g_bm_fd = fd;
	}

	cpld_args.regAddr = AX7_SYS_FAN_STATE_CPLD_REG;	
	cpld_args.slot = AX7_CRSMU_SLOT_NUM;

	retval = ioctl (g_bm_fd,BM_IOC_CPLD_READ,&cpld_args);

	/*NBM_DEBUG(("CRSMU reg[%#0x] data[%#0x]",cpld_args.regAddr,cpld_args.regData));*/
	if( 0 == retval ) {
		/*NBM_DEBUG(("system fan state %s!",(BIT_IS_SET(cpld_args.regData,SYS_FAN_STATE_BIT)) ? "alarm":"normal"));*/
		if(BIT_IS_SET(cpld_args.regData,SYS_FAN_STATE_BIT)){
			state = SYSTEM_FAN_STAT_ALARM;
		}
		else {
			state = SYSTEM_FAN_STAT_NORMAL;
		}
	}
	else {
		syslog_ax_cpld_err("read board CRSMU cpld error for fan!\n");
	}
	
	return state;
}


/**********************************************************************************
 * nbm_check_power_state
 *
 * read CPLD register to get system power working state
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0xFF - if error occurred.
 *		state - system power state.
 *
 **********************************************************************************/
unsigned char nbm_check_power_state
(
	void
)
{
	bm_op_cpld_args cpld_args = {0};
	int fd = -1,retval = 0;
	unsigned char state = 0;

	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			syslog_ax_cpld_err("open dev %s error(%d) when check power state!\n",NPD_BM_FILE_PATH,fd);
			return ~0;
		}
		g_bm_fd = fd;
	}

	cpld_args.regAddr = AX7_SYS_FAN_STATE_CPLD_REG;	
	cpld_args.slot = AX7_CRSMU_SLOT_NUM;

	retval = ioctl (g_bm_fd,BM_IOC_CPLD_READ,&cpld_args);
	/*NBM_DEBUG(("CRSMU reg[%#0x] data[%#0x]",cpld_args.regAddr,cpld_args.regData));*/
	if( 0 == retval ) {
		
		state = (cpld_args.regData)&(0x0f);
	}
	else {
		syslog_ax_cpld_err("read board CRSMU cpld error for power!\n");
	}

	return state;
}


/**********************************************************************************
 * nbm_check_8610_power_state
 *
 * read CPLD register to get 8610 power working state
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0xFF - if error occurred.
 *		state - system power state.
 *
 **********************************************************************************/
unsigned char nbm_check_8610_power_state
(
	void
)
{
	bm_op_cpld_args cpld_args = {0};
	int fd = -1,retval = 0;
	unsigned char state = 0;
    unsigned char power_count = 4;

	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			syslog_ax_cpld_err("open dev %s error(%d) when check power state!\n",NPD_BM_FILE_PATH,fd);
			return ~0;
		}
		g_bm_fd = fd;
	}

	cpld_args.regAddr = AX8_SYS_POWER_STATE_CPLD_REG;	
	cpld_args.slot = 0;

	retval = ioctl (g_bm_fd,BM_IOC_CPLD_READ,&cpld_args);
	/*NBM_DEBUG(("CRSMU reg[%#0x] data[%#0x]",cpld_args.regAddr,cpld_args.regData));*/
	if( 0 == retval ) {
		state = (cpld_args.regData)&(0x0f);
	}
	else {
		syslog_ax_cpld_err("read board CRSMU cpld error for power!\n");
	}

    /* 
        * 0: plugged in 
        * 1: plugged out
        */
    if(state & 0x01) /* power1 */
        power_count--;
    if(state & 0x02) /* power2 */
        power_count--;
    if(state & 0x04) /* power3 */
        power_count--;
    if(state & 0x08) /* power4 */
        power_count--;

    if(power_count < 3)
        return 0; /* alarm */
    else
        return 1; /* normal */

}


/**********************************************************************************
 * nbm_dbus_power_state_abnormal
 *	- send warning message to tarp-helper, when  power state was changed
 *
 *	INPUT:
 *		power_state_flg  - 0 power  is ok, 1 power abnormal
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0x1 - if error occurred.
 *		0x0 - check success.
 *
 **********************************************************************************/
unsigned int nbm_dbus_power_state_abnormal
(
	unsigned int power_state_flg
)
{
#if 0
	DBusMessage *query;
	DBusError err;

	/* for debug*/
	syslog_ax_cpld_dbg("nbm_dbus_power_state_abnormal: power_state_flg %s\n",
						(power_state_flg == 1) ? "abnormal" :"ok");

	query = dbus_message_new_signal(RTDRV_DBUS_OBJPATH,	"aw.trap", "wid_dbus_trap_power_state_change");
	dbus_error_init(&err);

	dbus_message_append_args(query,
						DBUS_TYPE_UINT32, &power_state_flg,		
						DBUS_TYPE_INVALID);

	dbus_connection_send(nbm_dbus_connection, query, NULL);
	
	dbus_message_unref(query);	

	return NBM_OK;
#endif

#if 1
	DBusMessage* msg = NULL;
	static DBusConnection *nbm_trap_conn = NULL;
	DBusError err;
	int ret = 0;
	unsigned int serial = 0;

	/* for debug*/
	syslog_ax_cpld_dbg("trap power state abnormal: power_state_flg %s\n",
						(power_state_flg == 1) ? "abnormal" :"ok");

	dbus_error_init(&err);
	syslog_ax_cpld_dbg("error init ok\n");

	if (NULL == nbm_trap_conn) {
		/* connect to the DBUS system bus, and check for errors */
		nbm_trap_conn = dbus_bus_get_private(DBUS_BUS_SYSTEM, &err);
		if (dbus_error_is_set(&err)) {
			syslog_ax_cpld_err("Connection Error (%s)\n", err.message);
			dbus_error_free(&err);
			return 1;
		}
		if (NULL == nbm_trap_conn) {
			syslog_ax_cpld_err("Connection null\n");
			return 1;
		}
	}
	syslog_ax_cpld_dbg("bus get ok\n");

	/* register our name on the bus, and check for errors */
	ret = dbus_bus_request_name(nbm_trap_conn,
								"aw.nbm.signal",
								0,
								&err);
	if (dbus_error_is_set(&err)) {
		syslog_ax_cpld_err("Name Error (%s)\n", err.message);
		dbus_error_free(&err);
		return 1;
	}
	
	syslog_ax_cpld_dbg("request name ok\n");
	
	/* create a signal & check for errors */
	msg = dbus_message_new_signal(NPD_DBUS_OBJPATH,				/* object name of the signal */
								  "aw.trap", 					/* interface name of the signal */
								  "wid_dbus_trap_power_state_change"); /* name of the signal */
	if (NULL == msg) {
		syslog_ax_cpld_err("Message Null\n");
		return 1;
	}

	syslog_ax_cpld_dbg("new signal ok\n");
	
	/* append arguments onto signal */
	dbus_message_append_args(msg,
							DBUS_TYPE_UINT32, &power_state_flg,		
							DBUS_TYPE_INVALID);
	
	/* send the message and flush the connection */
	if (!dbus_connection_send(nbm_trap_conn, msg, &serial)) {
		syslog_ax_cpld_err("Signal send error, Out Of Memory!\n"); 
		return 1;
	}
	syslog_ax_cpld_dbg("Trap signal sent!\n");
	dbus_connection_flush(nbm_trap_conn);
	syslog_ax_cpld_event("sent message to trap.\n");

	/* free the message */
	dbus_message_unref(msg);

	return 0;
#endif
}

/**********************************************************************************
 * nbm_check_power_state_change
 *	- get system power working state , if power state was changed,
 *		send warning message to tarp-helper
 *
 *	INPUT:
 *	   	NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0x1 - if error occurred.
 *		0x0 - check success.
 *
 **********************************************************************************/
unsigned int nbm_check_power_state_change
(
	void
)
{
	unsigned int retval = NBM_OK;
	unsigned int power_state_flg = 0;		/*  0 power  is ok, 1 power abnormal */
	unsigned char state = 0;
	static unsigned int counter = 0;

	state = nbm_check_power_state();
	if (state == 0xFF) {
		syslog_ax_cpld_err("nbm check power state(%d) error!\n", state);
		retval = NBM_ERR;
		return retval;
	}

	if(AX7_ERR_MSG_MINUTE_PRINT_INTERVAL == counter) {
		syslog_ax_cpld_dbg("nbm get power state(%x)!\n", state);
	}
	/* bit[0:2] is power mode, bit[3] is power state
	 * Bit3-Bit0-powered alarm bits.
	 *  Bit3 is 1 means the voltage is normal, but it does not mean that
	 *      there was no power supply module failure;
	 *  Bit2 and Bit1 long as there is 1, that all power modules are OK;
	 *  Bit0 meaningless.
	 */
	state &= 0xA;

	if(AX7_ERR_MSG_MINUTE_PRINT_INTERVAL == counter) {
		syslog_ax_cpld_dbg("state previous(%x) new(%x)!\n", sys_power_state, state);
	}
	if (sys_power_state != state) {
		power_state_flg = (state == 0xA) ? 0 : 1;
		retval = nbm_dbus_power_state_abnormal(power_state_flg);
		if (retval != NBM_OK) {
			syslog_ax_cpld_err("send message to trap-helper error when power state is change, ret %d!\n",
								retval);
			retval = NBM_ERR;
		}
	}

	sys_power_state = state;

	if(counter % AX7_ERR_MSG_MINUTE_PRINT_INTERVAL) 
		counter++;
	else 
		counter = 0;
	
	return retval;
}

/**********************************************************************************
 * nbm_read_temperature
 *
 * read CPLD register to get system power working state
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0xFF - if error occurred.
 *		state - system power state.
 *
 **********************************************************************************/
int nbm_read_temperature(char *temp1, char *temp2)
{
    int fd1,fd2;
    int size;
    int ret=0;

	
    char a[2];  /* for test */
    fd1 = open(temp1_input,O_RDONLY); /* surface temperature*/
    if(fd1 < 0)
    {
        syslog_ax_cpld_err("open temp1_input failed,errno %d!\n",errno);
        return -1;
    }
	
    fd2 = open(temp2_input,O_RDONLY); /* internal temperature*/
    if(fd2 < 0)
    {
        syslog_ax_cpld_err("open temp2_input failed,errno %d!\n",errno);
		close(fd1);     /* code optimize: Resource leak. zhangdi@autelan.com 2013-01-18 */
        return -1;
    }
	
    memset(a,0,2);
    size = read(fd1,a,2);
    if(size < 0)
    {
        syslog_ax_cpld_err("read temp1 file failed,errno %d!\n",errno);
		close(fd1);
		close(fd2);
        return -1;
    }	
    strncpy(temp1,a,size);
	
    memset(a,0,2);
    size = read(fd2,a,2);
    if(size < 0)
    {
        syslog_ax_cpld_err("read temp2 file failed,errno %d!\n",errno);
        close(fd1);
        close(fd2);	
        return -1;
    }		
    strncpy(temp2,a,size);

    close(fd1);
    close(fd2);
    return ret;
}

/**********************************************************************************
 * nbm_get_sys_temperature
 *
 * read GPIO register to get system temperature.
 *
 *	INPUT:
 *		NULL
 *
 *	OUTPUT:
 *		intDegree - CPU internel temperature
 *		extDegree - CPU surface temperature
 *
 * 	RETURN:
 *		0xffffffff	- if error occurred.
 *		0 - if no error occurred
 *
 **********************************************************************************/
unsigned int nbm_get_sys_temperature
(
	unsigned short *intDegree,
	unsigned short *extDegree
)
{
    unsigned int ext = 0,core = 0;
    int ret =0;
    char temp1[4] = {0},temp2[4] = {0};
    /*ext is the temperature out of CPU Chip.*/
    /*core is core temperature*/

    ret= nbm_read_temperature(temp1,temp2);
    if(ret != 0)
	{
    	syslog_ax_cpld_err("nbm read system temperature error.\n");
    }
    ext = (unsigned int)atoi(temp1);
    core = (unsigned int)atoi(temp2);

    *intDegree = core;
    *extDegree = ext;

    return ret;
}

 /**********************************************************************************
  * nbm_sys_temperature_core_isalarm
  *
  * check if CPU core temperature is alarming
  *
  *  INPUT:
  * 	 	stateInfo - CPU core temperature info
  *
  *  OUTPUT:
  *		NULL
  *
  *  RETURN:
  *		NULL
  *
  **********************************************************************************/
void nbm_sys_temperature_core_isalarm
(
	struct system_temperature_s *stateInfo
)
{	
    if(SYSTEM_CORE_TMPRT_ISALARM(SYSTEM_CORE_HIGH_CRITICAL,stateInfo->current)){
		stateInfo->flag = 1;
		syslog_ax_cpld_dbg("WARNING system core temperature alarm !\n");
	}
	else if ((SYSTEM_CORE_TMPRT_ISALARM(SYSTEM_CORE_LOW_CRITICAL,stateInfo->current))&&
		(stateInfo->flag == NBM_TRUE)){
		syslog_ax_cpld_dbg("WARNING system core temperature alarm !\n");
	}
	else if (SYSTEM_CORE_TMPRT_NOALARM(SYSTEM_CORE_LOW_CRITICAL,stateInfo->current)){
		stateInfo->flag = 0;
	}

	return;
}
 /**********************************************************************************
  * nbm_sys_temperature_surface_isalarm
  *
  * check if CPU surface temperature is alarming
  *
  *  INPUT:
  * 	 	stateInfo - CPU surface temperature info
  *
  *  OUTPUT:
  *		NULL
  *
  *  RETURN:
  *		NULL
  *
  **********************************************************************************/
void nbm_sys_temperature_surface_isalarm
(
	struct system_temperature_s *stateInfo
)
{
	if(SYSTEM_CORE_TMPRT_ISALARM(stateInfo->criticalH,stateInfo->current)){
		stateInfo->flag = 1;
		syslog_ax_cpld_dbg("WARNING system surface temperature alarm !\n");
	}
	else if ((SYSTEM_CORE_TMPRT_ISALARM(stateInfo->criticalL,stateInfo->current))&&	\
			  (stateInfo->flag == NBM_TRUE)){/*alarm statae*/
		syslog_ax_cpld_dbg("WARNING system surface temperature alarm !\n");
	}
	else if (SYSTEM_CORE_TMPRT_NOALARM(stateInfo->criticalL,stateInfo->current)){
		stateInfo->flag = 0;
	}

	return;
}
 /**********************************************************************************
  * nbm_check_system_temperature_state
  *
  * check system temperature state(CPU core or surface is alarming or not)
  *
  *  INPUT:
  * 	 	stateInfo - CPU surface temperature info
  *		slot - slot number
  *		counter - check timer used to determine if warning message needed or not
  *
  *  OUTPUT:
  *		NULL
  *
  *  RETURN:
  *		NULL
  *
  **********************************************************************************/
void nbm_check_system_temperature_state
(
	struct system_state_s *stateInfo,
	unsigned char slot,
	unsigned int counter
)
{
	if(0 != nbm_get_sys_temperature(&(stateInfo->core.current),&(stateInfo->surface.current))){
		syslog_ax_cpld_err("nbm get CPU temperature error !\n");
	}
	if((stateInfo->core.current)>=SYSTEM_CORE_HIGH_EXTREMUM){/*compell interrupt power*/
		syslog_ax_cpld_err("WARNING:system need shut down power for extreme high temperature\n");
		/*shut down system*/
		nbm_shut_down_power_state(slot) ;
	}
	if((shut_down_power==NBM_TRUE)&&((stateInfo->core.current)>=SYSTEM_CORE_HIGH_MAXNUM)){/*open shutdown enable*/
		 syslog_ax_cpld_err("WARNING:system need shut down power for extreme high temperature\n");
		/*shut down system;*/
		nbm_shut_down_power_state(slot) ;
	}
	if(0 == (counter%AX7_ERR_MSG_MINUTE_PRINT_INTERVAL)){		
		nbm_sys_temperature_core_isalarm(&(stateInfo->core));
		nbm_sys_temperature_surface_isalarm(&(stateInfo->surface));
	}
    
    if( (BOARD_TYPE == BOARD_TYPE_AX81_SMU)
        || (BOARD_TYPE == BOARD_TYPE_AX71_2X12G12S)
        || (BOARD_TYPE == BOARD_TYPE_AX81_1X12G12S)
        || (BOARD_TYPE == BOARD_TYPE_AX81_AC8C)
        || (BOARD_TYPE == BOARD_TYPE_AX81_AC12C) ){
        if((stateInfo->core.current)>=SYSTEM_CORE_HIGH_CRITICAL){
            nbm_cpld_reg_write(AX_LOCALBOARD_TMP_ALARM_CPLD_REG, 1); /*alarm*/
        }
        else{
            nbm_cpld_reg_write(AX_LOCALBOARD_TMP_ALARM_CPLD_REG, 0); /*normal*/
        }
    }

	return;
}

/**********************************************************************************
 * nbm_check_system_fan_power_state
 *
 * read CPLD register to get system working state
 *
 *	INPUT:
 *		stateInfo - system state info structure
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS - if no error occurred.
 *		NPD_FAIL - if error occurred.
 *
 **********************************************************************************/
 int nbm_check_system_fan_power_state
(
	struct system_state_s *stateInfo ,unsigned int counter
)
{
	bm_op_cpld_args cpld_args = {0};
	int fd = -1,retval = NPD_SUCCESS;
	enum system_fan_status_e state = SYSTEM_FAN_STAT_MAX;
	unsigned char power_state = 0;

	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			syslog_ax_cpld_err("open dev %s error(%d) when check fan and power state!\n",NPD_BM_FILE_PATH,fd);
			return NPD_FAIL;
		}
		g_bm_fd = fd;
	}

	cpld_args.regAddr = AX7_SYS_FAN_STATE_CPLD_REG;	
	cpld_args.slot = AX7_CRSMU_SLOT_NUM;

	retval = ioctl (g_bm_fd,BM_IOC_CPLD_READ,&cpld_args);

	if( 0 == retval ) {/*ouput system info about fan and  power*/
		if(counter ==AX7_FATAL_ERR_MSG_PRINT_INTERVAL) {
			if(BIT_IS_SET(cpld_args.regData,SYS_FAN_STATE_BIT)) {
				syslog_ax_cpld_dbg("CRSMU reg[%#0x] data[%#0x]\n",cpld_args.regAddr,cpld_args.regData);
				syslog_ax_cpld_dbg("WARNING fan state alarm !\n");
			}
			if(	SYS_POWER_M1_ISALARM(cpld_args.regData) ||
				(!SYS_POWER_M2_ISALARM(cpld_args.regData)) ||
				(!SYS_POWER_M3_ISALARM(cpld_args.regData)) ||
				(!SYS_POWER_ISNORMAL(cpld_args.regData))) {
				syslog_ax_cpld_dbg("WARNING system power state alarm !\n");
			}
		}
		/* check fan state*/
		if(BIT_IS_SET(cpld_args.regData,SYS_FAN_STATE_BIT)){
			state = SYSTEM_FAN_STAT_ALARM;
		}
		else {
			state = SYSTEM_FAN_STAT_NORMAL;
		}

		/* check power state*/
		if(SYS_POWER_ISNORMAL(cpld_args.regData)) {/* voltage is nomar */
			power_state = (cpld_args.regData)&(0x0f); /*0  means err*/
		}
		else {
			syslog_ax_cpld_err("voltage is non-normal\n");
		}
	}
	else {
			syslog_ax_cpld_err("read board CRSMU cpld error!\n");
	}


	stateInfo->fan_state 	= state;
	stateInfo->power_state = power_state;
	return retval;
}

/**********************************************************************************
 * nbm_set_system_init_stage
 *
 *	Set system initialization stage
 *
 *	INPUT:
 *		stage - 0 for initialization is undergo, 1 for initialization is done
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *	
 *	NOTE:
 *		This API is used to light the SYS and RUN LEDs indicating system init stage
 *
 **********************************************************************************/
void nbm_set_system_init_stage
(
	unsigned char stage
)
{	
	bm_op_cpld_args cpld_args = {0};
	enum module_status_e retval = MODULE_STAT_NONE;
	int fd = -1;

	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			syslog_ax_cpld_err("open dev %s error(%d) when set system init stage!\n",NPD_BM_FILE_PATH,fd);
			return ;
		}
		g_bm_fd = fd;
	}

	cpld_args.regAddr = AX_SYSTEM_INIT_STAGE_REG;	
	cpld_args.slot    = 0;
	cpld_args.regData = stage ? 1:0;

	retval = ioctl (g_bm_fd,BM_IOC_CPLD_WRITE,&cpld_args);

	if(retval) {
		syslog_ax_cpld_err("nbm write cpld error when set system init stage!\n");
	}
	return;
}

#ifdef DRV_LIB_BCM
/**********************************************************************************
 * nbm_reset_led_mutex_cpld
 *		- reset CPLD register 0x06 bit[4]
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *
 *	 NOTE:
 *		This API is used to enable Fiber-optic ports and electric ports mutex,
 *		indicating system init stage
 *
 **********************************************************************************/
void nbm_reset_led_mutex_cpld()
{
	int fd = -1;
	int retval = 0;

	bm_op_cpld_args cpld_args = {0};

	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			syslog_ax_cpld_err("open dev %s error(%d) when write cpld!\n",NPD_BM_FILE_PATH, fd);
			return ;
		}
		g_bm_fd = fd;
	}

    /*********************
	 * reset cpld register 0x06
	 *********************/
	cpld_args.regAddr = AX_CPLD_LED_MUTEX_CPLD_REG;
	cpld_args.slot = 0;

	/* set reg 0x06 bit[4] 0*/
	retval = ioctl (g_bm_fd, BM_IOC_CPLD_READ, &cpld_args);
	if (0 != retval)
	{
		syslog_ax_cpld_err("set bit[4] 0, cpld reg 0x06 read error!\n");
		return ;
	}

	cpld_args.regData &= 0xEF;
	retval = ioctl (g_bm_fd, BM_IOC_CPLD_WRITE, &cpld_args);
	if (0 != retval)
	{
		syslog_ax_cpld_err("set bit[4] 0, cpld reg 0x06 write error!\n");
		return ;
	}

	/* set reg 0x06 bit[4] 1*/
	retval = ioctl (g_bm_fd, BM_IOC_CPLD_READ, &cpld_args);
	if (0 != retval)
	{
		syslog_ax_cpld_err("set bit[4] 1, cpld reg 0x06 read error!\n");
		return ;
	}

	cpld_args.regData |= 0x10;
	retval = ioctl (g_bm_fd, BM_IOC_CPLD_WRITE, &cpld_args);
	if (0 != retval)
	{
		syslog_ax_cpld_err("set bit[4] 1, cpld reg 0x06 write error!\n");
		return ;
	}
	syslog_ax_cpld_dbg("reset CPLD register 0x06 bit[4] success!\n");

	return ;
}

/**********************************************************************************
 * ledproc_write_led_port_media_cpld
 *		- set CPLD register 0x27 - CPLD type and test register
 *		- for mutex ferber/copper ports in phy5482
 *		- unit 0, if media is fiber, set reg27 bit[0] as 0,  if media is copper, set set reg27 bit[0] as 1
 *		- unit 1, if media is fiber, set reg27 bit[1] as 0,  if media is copper, set set reg27 bit[1] as 1
 *
 *	INPUT:
 *		NULL
 *
 *	OUTPUT:
 *		NULL
 *
 *	RETURN:
 *		NPD_SUCCESS - if no error occurred
 *		NPD_FAIL - if ioctl failed or result error
 *
 **********************************************************************************/
int nbm_cpld_write_port_media_type(int unit, int mediaType)
{
	int fd = -1;
	int retval = -1;
	bm_op_cpld_args cpld_args = {0};
	unsigned char regData = 0;

	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			syslog_ax_cpld_err("open dev %s error(%d) when write cpld!\n","/dev/bm0 \n", fd);
			return -1;
		}
		g_bm_fd = fd;
	}

	/******************************
	 * Read and test cpld led register 0x27
	 ******************************/
	cpld_args.regAddr = 0x27;
	cpld_args.slot = 0;

	retval = ioctl (g_bm_fd, BM_IOC_CPLD_READ, &cpld_args);
	if (0 == retval)
	{		
		regData = cpld_args.regData;
		if (unit == 0)						/* unit 0*/
		{
			if (mediaType == 0) 			/* port meida is fiber*/
			{
				regData &= 0xFE;
			}
			else if (mediaType == 1)		/* port media is copper*/
			{
				regData |= 0x01;
			}
		}
		else if (unit == 1) 				/* unit 1*/
		{
			if (mediaType == 0) 			/* port meida is fiber*/
			{
				regData &= 0xFD;
			}
			else if (mediaType == 1)		/* port media is copper*/
			{
				regData |= 0x02;
			}
		}

		cpld_args.regData = regData;
		/* write*/
		retval = ioctl (g_bm_fd, BM_IOC_CPLD_WRITE, &cpld_args);
		if (-1 == retval) {
			syslog_ax_cpld_err("cpld write reg 0x27 failed, value %#x\n", cpld_args.regData);
			retval = -1;
		} else {
			/*read*/
			retval = ioctl (g_bm_fd, BM_IOC_CPLD_READ, &cpld_args);
			if (-1 == retval) {
				syslog_ax_cpld_err("cpld read reg 0x27 failed after write %#x\n", cpld_args.regData);
				retval = -1;
			} else {
				 /* check written value regData*/
				if(regData != cpld_args.regData) {
					syslog_ax_cpld_err("cpld reg 0x27 error read %#x after write %#x\n", cpld_args.regData, regData);
					retval = -1;
				}
				else {
					retval = 0;
				}
			}
		}
	}
	else {
		syslog_ax_cpld_err("cpld reg 0x27 read error!\n");
		retval = -1;
	}
	syslog_ax_cpld_dbg("set CPLD register 0x27 success, regdata %#x!\n", regData);

	return retval;
}
#endif

/**********************************************************************************
 * nbm_probe_subcard_module_id
 *
 * get subcard module id
 *
 *	INPUT:
 *		slotno - slot count to check
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		4626,5612,5612i subcard module id
 *
 **********************************************************************************/
int nbm_get_subcard_module_id
(
	unsigned int productId,
	int card_num
)
{
	unsigned char ret = 0;
	int retval = MODULE_ID_NONE;
	
	ret = nbm_cvm_query_card_on(card_num);
	if(0 == ret) {
		switch(productId) {
			case PRODUCT_ID_AX5K:
				retval = MODULE_ID_SUBCARD_TYPE_A0;
				break;
			case PRODUCT_ID_AX5K_I:
				retval = MODULE_ID_SUBCARD_TYPE_A1;
				break;
			case PRODUCT_ID_AU4K:
				retval = MODULE_ID_SUBCARD_TYPE_B;
				break;
			case PRODUCT_ID_NONE:
				break;
			default:
				break;			
		}	
	}
	else if(1 == ret) {
		retval = MODULE_ID_NONE;
	}
	else {
		syslog_ax_cpld_err("get subcard %d whether on its slot or not error!\n",card_num);
	}	
	return retval;
}

/**********************************************************************************
 * nbm_check_board_state
 *
 * read CPLD register to get system working state
 *
 *	INPUT:
 *		slotCount - slot count to check
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		loop forever,no return
 *
 **********************************************************************************/
int nbm_check_board_state(unsigned char slotCount)
{
	static unsigned int counter = 0;
	bm_op_rbit64_args gpio_args = {0};
	unsigned int i;
	int fd = -1;
	int retval;
	int ret = 0;
	AX7_BOARD_STATE_E current_state[AX7_PRODUCT_SLOT_NUM] = {
			AX7_BOARD_STATE_ACTIVE,		/*slot 0 CRSMU*/
			AX7_BOARD_STATE_INACTIVE,	/*slot 1 sub card*/
			AX7_BOARD_STATE_INACTIVE,	/*slot 2 sub card*/
			AX7_BOARD_STATE_INACTIVE,	/*slot 3 sub card*/		
			AX7_BOARD_STATE_INACTIVE,	/*slot 4 sub card*/
	};

	/* tell my thread id*/
	npd_init_tell_whoami((unsigned char *)"BoardStateCheck",0);

	for(i=1 ; i < slotCount; i++) {
		if(MODULE_STAT_NONE == chassis_slots[i]->module.state) 
			current_state[i] = AX7_BOARD_STATE_INACTIVE;
		else
			current_state[i] = AX7_BOARD_STATE_ACTIVE;
	}
	syslog_ax_cpld_dbg("start board state checker thread for existing %d slots\n",slotCount);

	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			syslog_ax_cpld_err("open dev %s error(%d) when check board state!\n",NPD_BM_FILE_PATH,fd);
			return -1;
		}
		g_bm_fd = fd;
	}	
	
	for(;;) {
		/* check GPIO state for board running state*/
		gpio_args.regAddr = AX7_GPIO_STATE_REG_ADDR;
		retval = ioctl (g_bm_fd,BM_IOC_BIT64_REG_STATE,&gpio_args);
		if( 0 == retval ) {
			for( i = 1; i < slotCount; i++) {
				if(gpio_args.regData & (1<<(i-1))) {/*register value is 1 means the slot is inactive.*/
					if(current_state[i] == AX7_BOARD_STATE_ACTIVE) {
						syslog_ax_cpld_dbg("WARNING : slot %d being hot pulled out!!!\n",i);
						current_state[i] = AX7_BOARD_STATE_INACTIVE;
						
						npd_chassis_slot_hot_pullout(i);
					}
					else {
						/*no state change, do nothing*/
					}
				}
				else {
					if(current_state[i] == AX7_BOARD_STATE_INACTIVE) {
						syslog_ax_cpld_dbg("WARNING : slot %d being hot plugin!!!\n",i);
						current_state[i] = AX7_BOARD_STATE_ACTIVE;
						
						npd_chassis_slot_hot_plugin(i);
					}
					else {
						/* read CPLD to check board state*/
						ret = nbm_get_board_state(i);
						if((ret >= 0)&&(ret <= MODULE_STAT_MAX)){
						    chassis_slots[i]->module.state = ret;
						}
					}
				}
			}
		}
		else {
				syslog_ax_cpld_err("read gpio status error!\n");
		}

		/* read cpld check board accessories' state*/
		nbm_check_system_fan_power_state(&systemStateInfo,counter);

		/* check power state is changed, send warning message to tarp-helper*/
		ret = nbm_check_power_state_change();
		if (ret != NBM_OK) {
			syslog_ax_cpld_err("check power state change error %d!\n", ret);
		}

		/* read gpio get system temperature*/
		/* TODO: add api here to get system temperature*/
		nbm_check_system_temperature_state(&systemStateInfo,0,counter);
		if(AX7_FATAL_ERR_MSG_PRINT_INTERVAL== counter) {
			counter = 0; /* reset counter*/
		}
		else {
			counter++;
		}

		sleep(1); /*sleep one second*/
	}
	
	return retval;
}

/**********************************************************************************
 * nbm_check_8610_board_state
 *
 * read CPLD register to get system working state
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		loop forever,no return
 *
 **********************************************************************************/
int nbm_check_8610_board_state(void)
{
	static unsigned int counter = 0;
	int fd = -1;
	int retval;

	/* tell my thread id*/
	npd_init_tell_whoami((unsigned char *)"Board8610StateCheck",0);
	
	syslog_ax_cpld_dbg("start board state checker thread for 8610\n");

	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			syslog_ax_cpld_err("open dev %s error(%d) when check board state!\n",NPD_BM_FILE_PATH,fd);
			return -1;
		}
		g_bm_fd = fd;
	}	
	
	for(;;) {
		/* read gpio get system temperature*/
		/* TODO: add api here to get system temperature*/
		nbm_check_system_temperature_state(&systemStateInfo,0,counter);
		if(AX7_FATAL_ERR_MSG_PRINT_INTERVAL== counter) {
			counter = 0; /* reset counter*/
		}
		else {
			counter++;
		}

		sleep(1); /*sleep one second*/
	}
	
	return retval;
}


/*
  TODO The functions here should be replaced by actually read the cpld
*/
int nbm_probe_chassis_module_id(int slotno) {

	bm_op_cpld_args cpld_args = {0};
	int fd = -1;
	int retval = MODULE_ID_NONE, result = 0, backplan_id;
	enum product_id_e	product_id = PRODUCT_ID_NONE;
	unsigned char value = 0;

	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			syslog_ax_cpld_err("open dev %s error(%d) when probe chassis module id!\n",NPD_BM_FILE_PATH,fd);
			#ifdef DIAG_XCAT
			return MODULE_ID_AU3_3528;
			#else
			return -1;
			#endif
		}
		g_bm_fd = fd;
	}

	/**
	  *	phase 1:read product type
	  */
	cpld_args.regAddr = AX_PRODUCT_TYPE_CPLD_REG;
	cpld_args.slot	  = 0;
	result = ioctl(g_bm_fd,BM_IOC_CPLD_READ,&cpld_args);
	
	if (result != 0) {
		syslog_ax_cpld_err("read cpld register PRODUCT TYPE error!\n");
		return MODULE_ID_NONE;
	}	
	else {
		value = cpld_args.regData;
		if(AX7K_CPLD_PRODUCT_TYPE_CODE == (value & 0xF)){
			product_id = PRODUCT_ID_AX7K;
			
			backplan_id = nbm_query_backplane_id();
			if(BACKPLANE_TYPE_ID_AX7605I == backplan_id){
				product_id = PRODUCT_ID_AX7K_I;
			}			
		}
		else if(AX5K_CPLD_PRODUCT_TYPE_CODE == (value & 0xF)) {
			product_id = PRODUCT_ID_AX5K;
		}
		else if(AU4K_CPLD_PRODUCT_TYPE_CODE == (value & 0xF)){
			product_id = PRODUCT_ID_AU4K;
		}
		else if(AU3K_CPLD_PRODUCT_TYPE_CODE == (value & 0xF)){
			product_id = PRODUCT_ID_AU3K;
		}
	}

	#ifdef DIAG_XCAT
	product_id = PRODUCT_ID_AU3K;
	#endif
	
	memset(&cpld_args, 0, sizeof(bm_op_cpld_args));
	cpld_args.regAddr = AX_SINGLEPLANE_TYPE_CPLD_REG;	
	if(PRODUCT_ID_AX7K == product_id) { /* All box product use slot # 0 for CPLD R/W */
		cpld_args.slot = slotno;
	}

	result = ioctl(g_bm_fd,BM_IOC_CPLD_READ,&cpld_args);
	
	if (0 == result) {
		value = cpld_args.regData;
		syslog_ax_cpld_dbg("read cpld register SINGLE_BOARD_TYPE value %#x\n",value);
		switch(product_id) {
			case PRODUCT_ID_AX7K:
				retval = (0x80 == value) ? MODULE_ID_AX7_CRSMU : 	\
						 (0x00 == value) ? MODULE_ID_AX7_6GTX  :	\
						 (0x02 == value) ? MODULE_ID_AX7_6GE_SFP : 	\
						 (0x04 == value) ? MODULE_ID_AX7_XFP : 		\
						 (0x08 == value) ? MODULE_ID_AX7_6GTX_POE : \
						 MODULE_ID_NONE;
				break;
			case PRODUCT_ID_AX5K:
				retval = (0x00 == value) ? MODULE_ID_AX5_5612:	\
						 (0x03 == value) ? MODULE_ID_AX5_5612I:	\
						 (0x61 == value) ? MODULE_ID_AX5_5612E: \
						 (0x62 == value) ? MODULE_ID_AX5_5608: \
						 MODULE_ID_NONE;
				break;
			case PRODUCT_ID_AU4K:
				value &= 0x7; /* valid bit[2:0]*/
				retval = (0x01 == value) ? MODULE_ID_AU4_4626:	\
						 (0x02 == value) ? MODULE_ID_AU4_4524:	\
						 (0x03 == value) ? MODULE_ID_AU4_4524_POE:	\
						 MODULE_ID_NONE;
				break;
			case PRODUCT_ID_AU3K:
				/* TODO: AU3xxx board not implemented yet!*/
				value &= 0x7; /* valid bit[2:0]*/
				retval = (0x00 == value) ? MODULE_ID_AU3_3524:	\
						 (0x01 == value) ? MODULE_ID_AU3_3052:	\
						 (0x02 == value) ? MODULE_ID_AU3_3028:	\
						 MODULE_ID_NONE;
				#ifdef DIAG_XCAT
				retval = MODULE_ID_AU3_3528;
				#endif
				break;
			case PRODUCT_ID_AX7K_I:
				retval = (0x80 == value) ? MODULE_ID_AX7I_CRSMU :	\
				         (0x04 == value) ? MODULE_ID_AX71_2X12G12S :	\
						 MODULE_ID_NONE;
				/* modify by qinhs@autelan 2009-11-18 for 7605i  */
#if 0
				if(AX7i_XG_CONNECT_SLOT_NUM == slotno) {
					retval = MODULE_ID_AX7I_XG_CONNECT;
				}
#endif
				if(AX7i_SLAVE_SLOT_NUM == slotno) {
					retval = MODULE_ID_NONE;
				}

				break;
			case PRODUCT_ID_NONE:
				break;
			default:
				break;
		}
	}
	else {
		syslog_ax_cpld_err("read slot %d cpld register BOARD TYPE error!\n",slotno);
	}
	
	return retval;

}

/* Slot no is the number printed in the front panel, not slot array index used in software*/

unsigned char nbm_probe_chassis_module_hw_version(int slotno) {
	/* get chassis module HW(pcb) version*/
	bm_op_cpld_args cpld_args = {0};
	int fd = -1;
	int result = 0;
	unsigned char value = 0;

	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			syslog_ax_cpld_err("open dev %s error(%d) when probe chassis module hw version!\n",NPD_BM_FILE_PATH,fd);
			return -1;
		}
		g_bm_fd = fd;
	}

	cpld_args.regAddr = AX_PCB_VERSION_CPLD_REG;
	cpld_args.slot	  = slotno;
	result = ioctl(g_bm_fd,BM_IOC_CPLD_READ,&cpld_args);
	
	if (result != 0) {
		syslog_ax_cpld_err("read cpld register PCB VERSION error!\n");
		value = 0;
	}	
	else { /* valid bit[3:0] */
		value = cpld_args.regData;
		value &= 0xF;
	}

	return value;
}
/**********************************************************************************
 * nbm_init_cpld
 *
 * read CPLD register 0x00 - CPLD type and test register
 *
 *	INPUT:
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS - if no error occurred
 *		NPD_FAIL - if ioctl failed or result error
 *
 **********************************************************************************/
int nbm_init_cpld(void) {
    /* Read and test cpld version register*/
	bm_op_cpld_args cpld_args = {0};
	int fd = -1,retval = NPD_FAIL;

	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			syslog_ax_cpld_err("open dev %s error(%d) when init cpld!\n", NPD_BM_FILE_PATH,fd);
			return NPD_FAIL;
		}
		g_bm_fd = fd;
	}

	cpld_args.regAddr = AX_CPLD_TYPE_CPLD_REG;	
	cpld_args.slot = 0; /* box product has the same base address as chassis product slot 0*/

	retval = ioctl (g_bm_fd,BM_IOC_CPLD_READ,&cpld_args);

	if( NPD_SUCCESS == retval ) {
		if(0x55 == cpld_args.regData){
			cpld_args.regData = 0xaa;
			if(-1== ioctl (g_bm_fd,BM_IOC_CPLD_WRITE,&cpld_args)){/* write test value 0xaa*/
				syslog_ax_cpld_err("init cpld test failed when test write 0xaa\n");
				retval = NPD_FAIL;
			}
			else if((-1 == ioctl (g_bm_fd,BM_IOC_CPLD_READ,&cpld_args))){/* read*/
				syslog_ax_cpld_err("init cpld test failed when read after write 0xaa\n");
				retval = NPD_FAIL;
			}
			else if(0xaa != cpld_args.regData) { /* check written value 0xaa*/
				syslog_ax_cpld_err("init cpld test error read %#x after write 0xaa\n",cpld_args.regData);
				retval = NPD_FAIL;
			}
			else {
				cpld_args.regData = 0x55;
				if (-1==ioctl (g_bm_fd,BM_IOC_CPLD_WRITE,&cpld_args)) { /*write back reset value 0x55*/
					syslog_ax_cpld_err("init cpld test failed when reset to 0x55\n");
					retval = NPD_FAIL;
				}
				else
					syslog_ax_cpld_dbg("init cpld test success.\n");
			}
		}
		else if(0x5A == cpld_args.regData) {
			/* TODO: need implementation*/
		}
		else {
			/* TODO:need implementation*/
		}
	}
	else {
		syslog_ax_cpld_err("init cpld test read error!\n");
	}

	return retval;
}

/* local board*/
unsigned char nbm_query_mainboard_version(void) {
	/* get mainboard HW version*/
	bm_op_cpld_args cpld_args = {0};
	int fd = -1;
	int result = 0;
	unsigned char value = 0;

	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			syslog_ax_cpld_err("open dev %s error(%d) when query main board hw version!\n",NPD_BM_FILE_PATH,fd);
			return -1;
		}
		g_bm_fd = fd;
	}

	cpld_args.regAddr = AX_PCB_VERSION_CPLD_REG;
	cpld_args.slot	  = 0;
	result = ioctl(g_bm_fd,BM_IOC_CPLD_READ,&cpld_args);
	
	if (result != 0) {
		syslog_ax_cpld_err("read cpld register PCB VERSION error!\n");
		value = 0;
	}	
	else { /* valid bit[3:0] */
		value = cpld_args.regData;
		value &= 0xF;
	}
	
	return value;
}

/**********************************************************************************
 * nbm_init_cpld
 *
 * read CPLD register 0x00 - CPLD type and test register
 *
 *	INPUT:
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS - if no error occurred
 *		NPD_FAIL - if ioctl failed or result error
 *
 **********************************************************************************/
int nbm_query_mainboard_id(void) {
    /* Read and test cpld version register*/
	bm_op_cpld_args cpld_args = {0};
	int fd = -1,retval = OCTEON_MAINBOARD_ID_AX7_CRSMU,result = NPD_SUCCESS;

	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			syslog_ax_cpld_err("open dev %s error(%d) when query main board id!\n",NPD_BM_FILE_PATH,fd);
			return OCTEON_MAINBOARD_ID_UNKNOW;
		}
		g_bm_fd = fd;
	}

	cpld_args.regAddr = AX_PRODUCT_TYPE_CPLD_REG;	
	cpld_args.slot = 0; /* box product has the same base address as chassis product slot 0*/

	result = ioctl (g_bm_fd,BM_IOC_CPLD_READ,&cpld_args);

	if( NPD_SUCCESS == result ) {
		if(AX7K_CPLD_PRODUCT_TYPE_CODE == (cpld_args.regData & 0xF)){
			syslog_ax_cpld_dbg("nbm query main board id get OCTEON_AX7_CRSMU\n");
			retval = OCTEON_MAINBOARD_ID_AX7_CRSMU;
		}
		else if(AX5K_CPLD_PRODUCT_TYPE_CODE == (cpld_args.regData & 0xF)) {
			syslog_ax_cpld_dbg("nbm query main board id get OCTEON_AX5K\n");
			retval = OCTEON_MAINBOARD_ID_AX5K;
		}
		else if(AU4K_CPLD_PRODUCT_TYPE_CODE == (cpld_args.regData & 0xF)){
			syslog_ax_cpld_dbg("nbm query main board id get OCTEON_AU4K\n");
			retval = OCTEON_MAINBOARD_ID_AU4K;
		}
		else if(AU3K_CPLD_PRODUCT_TYPE_CODE == (cpld_args.regData & 0xF)){
			syslog_ax_cpld_dbg("nbm query main board id get OCTEON_AU3K\n");
			retval = OCTEON_MAINBOARD_ID_AU3K;
		}
		else if(AX86_CPLD_PRODUCT_TYPE_CODE == (cpld_args.regData & 0xF)){
			syslog_ax_cpld_dbg("nbm query main board id get OCTEON_AX8600\n");
			retval = OCTEON_MAINBOARD_ID_AX8600;   /*zhangdi add*/
		}		
		else {
			syslog_ax_cpld_dbg("nbm query main board id get %#x\n",cpld_args.regData);
			retval = OCTEON_MAINBOARD_ID_UNKNOW;
		}
	}
	else {
		syslog_ax_cpld_err("nbm query main board id read cpld error!\n");
	}

	return retval;
}

int nbm_query_backplane_id(void) {
	/* We should get 3bits backplane id and 2 bits backplane version here*/
    /* Read and test cpld version register*/
	bm_op_cpld_args cpld_args = {0};
	int fd = -1,retval = BACKPLANE_TYPE_NONE,result = NPD_SUCCESS;

	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			syslog_ax_cpld_err("open dev %s error(%d) when query back plane id!\n", NPD_BM_FILE_PATH,fd);
			return NPD_FAIL;
		}
		g_bm_fd = fd;
	}

	/* check if CPLD type register 0x00 has value 0x55 - for chassis product master CPLD*/	
	cpld_args.regAddr = AX_CPLD_TYPE_CPLD_REG;	
	cpld_args.slot = 0; /* chassis product slot 0*/

	result = ioctl (g_bm_fd,BM_IOC_CPLD_READ,&cpld_args);
	if( result || (0x55 != cpld_args.regData)) {
		syslog_ax_cpld_err("nbm query back plane id failed for wrong CPLD type %#x\n",cpld_args.regData);
		return BACKPLANE_TYPE_NONE;
	}
	
	cpld_args.regAddr = AX_BACKPLANE_TYPE_CPLD_REG;	
	cpld_args.slot = 0; /* chassis product slot 0*/

	result = ioctl (g_bm_fd,BM_IOC_CPLD_READ,&cpld_args);

	if( NPD_SUCCESS == result ) { /* back plane type bit[4:2],version bit[1:0]*/
		syslog_ax_cpld_dbg("nbm query back plane id get type %#x version %#x\n",	\
				(cpld_args.regData & 0x1C)>>2, cpld_args.regData & 0x3);
		if(1 == ((cpld_args.regData & 0x1C)>>2)) {
			retval = BACKPLANE_TYPE_ID_AX7605I;
		}
		else {
			retval = BACKPLANE_TYPE_ID_AX7605_V1;
		}
	}
	else {
		syslog_ax_cpld_err("nbm query back plane id read cpld error!\n");
		retval = BACKPLANE_TYPE_NONE;
	}

	return retval;
}

/**********************************************************************************
 * nbm_cpld_reg_write
 *
 * General purpose API to write a CPLD register
 *
 *	INPUT:
 *		addr	- CPLD register address
 *		value 	- CPLD register value
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 - if no error occur.
 *		NPD_FAIL - if error occurred.
 *
 **********************************************************************************/
int nbm_cpld_reg_write(int addr,unsigned char value)
{       
	bm_op_cpld_args phy_args_cmd;
	int retval = 0;
	int fd = -1;
    #if 0
	nbm_syslog_dbg("nbm write register %#x value %#x\n",addr,value);
    #endif
	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
	
		if(fd < 0)
		{
			nbm_syslog_err("open dev %s error(%d) when write cpld!",NPD_BM_FILE_PATH,fd);
			return CPLD_RETURN_CODE_OPEN_FAIL;
		}
		g_bm_fd = fd;
	}

	memset(&phy_args_cmd,0,sizeof(bm_op_cpld_args));

	phy_args_cmd.regAddr = addr;
	phy_args_cmd.slot = 0;
	phy_args_cmd.regData = value;

	retval = ioctl (g_bm_fd,BM_IOC_CPLD_WRITE,&phy_args_cmd);
	if(retval == -1) {
		nbm_syslog_err("write cpld register %#x value %#x ioctl error!", addr, value);
		return CPLD_RETURN_CODE_IOCTL_FAIL;
	}
	
	return retval;
}

 /**********************************************************************************
  * nbm_cpld_reg_read
  *
  * General purpose API to read a CPLD register
  *
  *  INPUT:
  * 	 addr	 - CPLD register address
  *  
  *  OUTPUT:
  * 	 value	 - CPLD register value
  *
  *  RETURN:
  * 	 0 - if no error occur.
  * 	 NPD_FAIL - if error occurred.
  *
  **********************************************************************************/
 int nbm_cpld_reg_read(int reg, unsigned char *value)
{
	bm_op_cpld_args cpld_args;
	int fd = -1,retval = 0;

	if(!value) {
		nbm_syslog_err("nbm read cpld register %#x null pointer error\n", reg);
		return NPD_FAIL;
	}
	
	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
	
		if(fd < 0)
		{
			nbm_syslog_err("open dev %s error(%d) when read cpld\n",NPD_BM_FILE_PATH,fd);
			return CPLD_RETURN_CODE_OPEN_FAIL;
		}
		g_bm_fd = fd;
	}

	memset(&cpld_args, 0, sizeof(bm_op_cpld_args));
	cpld_args.regAddr = reg;

	retval = ioctl (g_bm_fd,BM_IOC_CPLD_READ,&cpld_args);
	if(retval == -1) {
		nbm_syslog_err("nbm read cpld register %#x error!\n", reg);
		return CPLD_RETURN_CODE_IOCTL_FAIL;
	}

	/*get the return value*/
	if(AX_CPLD_WATCHDOG_FILLUP_REG != reg) { /* debug message ignore watchdog fillup */
		nbm_syslog_dbg("nbm read cpld register %#x value %#x\n",reg,cpld_args.regData);
	}
	*value = cpld_args.regData;
	return 0;
}
 
/**********************************************************************************
* nbm_cpld_driven_phy
*
* DESCRIPTION:
*   This function write CPLD register to drive AX51-GTX/AX51-SFP/AX51-XFP phy 
*
*  INPUT:
* 	 productId - product id
* 	 card_type - card type
*  
*  OUTPUT:
* 	 NONE
*
*  RETURN:
* 	 NONE
*
**********************************************************************************/
void nbm_cpld_driven_phy
(
	unsigned int productId,
	unsigned char card_type
)
{
	unsigned char regVal = 0;

	/* write CPLD reg 0x27 to drive AX51-GTX/AX51-SFP/AX51-XFP phy */
	if(PRODUCT_ID_AU4K == productId || PRODUCT_ID_AX5K == productId  \
		 || PRODUCT_ID_AX5K_I == productId) {
		if(2 == card_type || 3 == card_type || 1 == card_type) {
			nbm_cpld_reg_read(AX_SYS_LED_PORT_MEDIA_CPLD_REG, &regVal);
			regVal &= 0;
			nbm_cpld_reg_write(AX_SYS_LED_PORT_MEDIA_CPLD_REG, regVal);
		}
	}

	return;
}

/**********************************************************************************
 *	nbm_eeprom_reg_read
 * 
 *  DESCRIPTION:
 *		 read eeprom register
 *
 *  INPUT:
 *		unsigned char twsi_channel,		- TWSI channel
 *		unsigned int eeprom_addr,		- eeprom address
 *		unsigned int eeprom_type,		- eeprom type
 *		unsigned int validOffset,			- whether the slave has offset (i.e. Eeprom  etc.), true: valid false: in valid
 *		unsigned int moreThan256,		- whether the ofset is bigger than 256, true: valid false: in valid
 *		unsigned int regAddr				- address of eeprom's register
 *  
 *  OUTPUT:
 *		unsigned char *regValue			- value of eeprom's register
 *
 *  RETURN:
 * 		NBM_ERR		- set fail
 *	 	NBM_OK		- set ok
 *
 **********************************************************************************/
unsigned int nbm_eeprom_reg_read
(
	unsigned char twsi_channel,
	unsigned int eeprom_addr,
	unsigned int eeprom_type,
	unsigned int validOffset,
	unsigned int moreThan256,
	unsigned int regAddr,
	unsigned char *regValue
)
{
	int fd = -1,retval = NBM_OK;

#ifdef DRV_LIB_CPSS_3_4
	twsi_op_t twsi_rw8_args;
	memset(&twsi_rw8_args, 0, sizeof(twsi_op_t));

	if (!regValue) {
		nbm_syslog_err("nbm_eeprom_reg_read error, parameter is null\n");
		return NBM_ERR;
	}

	nbm_syslog_dbg("nbm_eeprom_reg_read, add=0x%x\n", regAddr);

	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			nbm_syslog_err("open dev %s error(%d) when read oct mdio!\n", NPD_BM_FILE_PATH, fd);
			return NBM_ERR;
		}
		g_bm_fd = fd;
	}

	twsi_rw8_args.chanNum = twsi_channel;
	twsi_rw8_args.twsi_slave.slaveAddr.address = eeprom_addr;
	twsi_rw8_args.twsi_slave.slaveAddr.type = eeprom_type;
	twsi_rw8_args.twsi_slave.validOffset = validOffset;
	twsi_rw8_args.twsi_slave.moreThen256 = moreThan256;
	twsi_rw8_args.twsi_slave.offset = regAddr;
	twsi_rw8_args.reg_val_size = 1;

	retval = ioctl (g_bm_fd, BM_IOC_TWSI_READ8_, &twsi_rw8_args);
	if (retval == -1) {
		nbm_syslog_err("read eeprom error!\n");
		retval = NBM_ERR;
	}

	nbm_syslog_dbg("nbm_eeprom_reg_read over,add=0x%x value=0x%x ret %d\n",
					twsi_rw8_args.twsi_slave.offset,twsi_rw8_args.reg_val[0], retval);
	/*get the return value*/
	*regValue = twsi_rw8_args.reg_val[0];
#endif

	return retval;
}

/**********************************************************************************
 *	nbm_eeprom_reg_write
 * 
 *  DESCRIPTION:
 *		write eeprom register
 *
 *  INPUT:
 *		unsigned char twsi_channel,		- TWSI channel
 *		unsigned int eeprom_addr,		- eeprom address
 *		unsigned int eeprom_type,		- eeprom type
 *		unsigned int validOffset,			- whether the slave has offset (i.e. Eeprom  etc.), true: valid false: in valid
 *		unsigned int moreThan256,		- whether the ofset is bigger than 256, true: valid false: in valid
 *		unsigned int regAddr				- address of eeprom's register
 *		unsigned char *regValue			- value of eeprom's register
 *  
 *  OUTPUT:
 *		NULL
 *
 *  RETURN:
 * 		NBM_ERR		- set fail
 *	 	NBM_OK		- set ok
 *
 **********************************************************************************/
unsigned int nbm_eeprom_reg_write
(
	unsigned char twsi_channel,
	unsigned int eeprom_addr,
	unsigned int eeprom_type,
	unsigned int validOffset,
	unsigned int moreThan256,
	unsigned int regAddr,
	unsigned char regValue
)
{
	int fd = -1,retval = NBM_OK;

#ifdef DRV_LIB_CPSS_3_4
	twsi_op_t twsi_rw8_args;
	memset(&twsi_rw8_args, 0, sizeof(twsi_op_t));

	nbm_syslog_dbg("nbm_eeprom_reg_write, add=0x%x\n", regAddr);


	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			nbm_syslog_err("open dev %s error(%d) when write oct mdio!\n", NPD_BM_FILE_PATH, fd);
			return NBM_ERR;
		}
		g_bm_fd = fd;
	}

	twsi_rw8_args.chanNum = twsi_channel;
	twsi_rw8_args.twsi_slave.slaveAddr.address = eeprom_addr;
	twsi_rw8_args.twsi_slave.slaveAddr.type = eeprom_type;
	twsi_rw8_args.twsi_slave.validOffset = validOffset;
	twsi_rw8_args.twsi_slave.moreThen256 = moreThan256;
	twsi_rw8_args.twsi_slave.offset = regAddr;
	twsi_rw8_args.reg_val_size = 1;
	twsi_rw8_args.reg_val[0] = regValue;

	retval = ioctl (g_bm_fd, BM_IOC_TWSI_WRITE8_, &twsi_rw8_args);
	if (retval == -1) {
		nbm_syslog_err("read eeprom error!\n");
		retval = NBM_ERR;
	}

	nbm_syslog_dbg("nbm_eeprom_reg_write over,add=0x%x value=0x%x ret %d\n",
					twsi_rw8_args.twsi_slave.offset,twsi_rw8_args.reg_val[0], retval);
#endif
	return retval;
}

#ifdef DRV_LIB_CPSS_3_4
/**********************************************************************************
 *	nbm_open_laser
 * 
 *  DESCRIPTION:
 *		open laser of fiber ports in xcat
 *
 *  INPUT:
 *		NULL
 *  
 *  OUTPUT:
 *		NULL
 *
 *  RETURN:
 *		void
 *
 **********************************************************************************/
void nbm_open_laser
(
	void
)
{
	int fd = -1;
	int retval = 0;

	nbm_syslog_dbg("nbm_open_laser: open laser\n");

	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			nbm_syslog_err("open dev %s error(%d) when write oct mdio!\n", NPD_BM_FILE_PATH, fd);
			return ;
		}
		g_bm_fd = fd;
	}

	retval = ioctl (g_bm_fd, BM_IOC_TWSI_OPEN_LASER);
	if (retval == -1) {
		nbm_syslog_err("nbm_open_laser: open laser error.\n");
	}

	nbm_syslog_dbg("nbm_open_laser: open laser success.\n");
	return ;
}
#endif
/**********************************************************************************
 * nbm_hardware_watchdog_control
 *
 * 	Set hardware watchdog enable or disable
 *
 *	INPUT:
 *		enabled	- enable/disable hardware watchdog
 *	
 *	OUTPUT:
 *		NULL
 *
 *	RETURN:
 *		0 - if no error occur.
 *		NPD_FAIL - if error occurred.
 *
 **********************************************************************************/
int nbm_hardware_watchdog_control_set
(
	unsigned int enabled
)
{
	unsigned char regValue = 0;
	int ret = 0;

	/* confirm if current product support hardware watchdog or not */
	if(NBM_FALSE == npd_product_hardware_watchdog_function_check()) {
		return NBM_OK;
	}
	
	ret = nbm_cpld_reg_read(AX_CPLD_WATCHDOG_ENABLE_REG, &regValue);
	if(ret) {
		return ret;
	}
	
	if(enabled) { /* set bit0 */
		regValue |= 0x1;
	}
	else { /* clear bit0 */
		regValue &= 0xFE;
	}

	ret = nbm_cpld_reg_write(AX_CPLD_WATCHDOG_ENABLE_REG, regValue);
	
	return ret;
}

/**********************************************************************************
 * nbm_hardware_watchdog_control_get
 *
 * 	Get hardware watchdog enable or disable
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		enabled	- enable/disable hardware watchdog
 *
 *	RETURN:
 *		0 - if no error occur.
 *		NPD_FAIL - if error occurred.
 *
 **********************************************************************************/
int nbm_hardware_watchdog_control_get
(
	unsigned int *enabled
)
{
	unsigned char regValue = 0;
	int ret = 0;

	/* confirm if current product support hardware watchdog or not */
	if(NBM_FALSE == npd_product_hardware_watchdog_function_check()) {
		return NBM_OK;
	}

	if(!enabled) {
		nbm_syslog_err("nbm hardware watchdog control get null pointer error!\n");
		return NPD_FAIL;
	}
	
	ret = nbm_cpld_reg_read(AX_CPLD_WATCHDOG_ENABLE_REG, &regValue);
	if(ret) {
		return ret;
	}

	nbm_syslog_dbg("nbm hardware watchdog control get value %#x\n", regValue);
	if(regValue & 0x1) {
		*enabled = SYSTEM_HARDWARE_WATCHDOG_ENABLE;
	}
	else {
		*enabled = SYSTEM_HARDWARE_WATCHDOG_DISABLE;
	}

	return ret;
}

/**********************************************************************************
 * nbm_hardware_watchdog_timeout_set
 *
 * 	Set hardware watchdog timeout value
 *
 *	INPUT:
 *		timeout	- hardware watchdog timeout value
 *	
 *	OUTPUT:
 *		NULL
 *
 *	RETURN:
 *		0 - if no error occur.
 *		NPD_FAIL - if error occurred.
 *
 **********************************************************************************/
int nbm_hardware_watchdog_timeout_set
(
	unsigned int timeout
)
{
	unsigned char regValue = 0;
	int ret = 0;

	/* confirm if current product support hardware watchdog or not */
	if(NBM_FALSE == npd_product_hardware_watchdog_function_check()) {
		return NBM_OK;
	}

	ret = nbm_cpld_reg_read(AX_CPLD_WATCHDOG_TIMEOUT_REG, &regValue);
	if(ret) {
		return ret;
	}
	nbm_syslog_dbg("nbm hardware watchdog timeout change %d -> %d\n", regValue, timeout);
	
	ret = nbm_cpld_reg_write(AX_CPLD_WATCHDOG_TIMEOUT_REG, timeout);
	
	return ret;
}

/**********************************************************************************
 * nbm_hardware_watchdog_timeout_get
 *
 * 	Get hardware watchdog timeout value
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		timeout	- hardware watchdog timeout value
 *
 *	RETURN:
 *		0 - if no error occur.
 *		NPD_FAIL - if error occurred.
 *
 **********************************************************************************/
int nbm_hardware_watchdog_timeout_get
(
	unsigned int *timeout
)
{
	unsigned char regValue = 0;
	int ret = 0;
	
	/* confirm if current product support hardware watchdog or not */
	if(NBM_FALSE == npd_product_hardware_watchdog_function_check()) {
		return NBM_OK;
	}

	if(!timeout) {
		nbm_syslog_err("nbm hardware watchdog timeout get null pointer error!\n");
		return NPD_FAIL;
	}
	
	ret = nbm_cpld_reg_read(AX_CPLD_WATCHDOG_TIMEOUT_REG, &regValue);
	if(ret) {
		*timeout = 0;
		return ret;
	}
	nbm_syslog_dbg("nbm hardware watchdog timeout get value %#x\n", regValue);

	*timeout = regValue;
	return ret;
}

/**********************************************************************************
 * nbm_hardware_watchdog_fillup
 *
 * 	Fillup hardware watch dog, this is done via read or write fillup register once.
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 *	RETURN:
 *		0 - if no error occur.
 *		NPD_FAIL - if error occurred.
 *
 **********************************************************************************/
int nbm_hardware_watchdog_fillup
(
	void
)
{
	unsigned char regValue = 0;
	int ret = 0;

	/* confirm if current product support hardware watchdog or not */
	if(NBM_FALSE == npd_product_hardware_watchdog_function_check()) {
		return NBM_OK;
	}

	ret = nbm_cpld_reg_read(AX_CPLD_WATCHDOG_FILLUP_REG, &regValue);
	
	return ret;
}

/**********************************************************************************
 * nbm_gpio_reset_single_board
 *
 * Write GPIO register to reset single board
 *
 *	INPUT:
 *		slotCount - slot count to check
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 - if no error occur.
 *		NPD_FAIL - if error occurred.
 *
 **********************************************************************************/
int nbm_gpio_reset_single_board
(
	int chassis_slot_index
)
{
	bm_op_args gpio_args = {0};
	int fd = -1;
	int retval;
	
	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			syslog_ax_cpld_err("open dev %s error when reset slot %d!\n",NPD_BM_FILE_PATH,chassis_slot_index);
			return -1;
		}
		g_bm_fd = fd;
	}	
	
	/* write GPIO for reset single board 
	  *	GPIO bit position map
	  *		0 ~ 3 - slot running state from #1 to 4
	  *		4 ~ 7 - slot INT state from #1 to 4
	  *		8 ~ 11 - slot reset control from  #1 to 4
	  *	reset op sequence
	  *		write TX_CLR register a 1'b and
	  *		delay for 150ms and
	  *		write TX_SET register a 1'b
	  */
	gpio_args.op_addr = AX7_GPIO_TX_CLR_REG_ADDR;
	gpio_args.op_value = 1 << (chassis_slot_index + 7); 
	gpio_args.op_len = 64;
	retval = ioctl (g_bm_fd,BM_IOC_X_,&gpio_args);
	if( 0 == retval ) {
		syslog_ax_cpld_dbg("write gpio TX_CLR register reset slot %d got %#-16x\n", chassis_slot_index, gpio_args.op_value);
		
	}
	else {
		syslog_ax_cpld_err("write gpio TX_CLR to reset slot %d error %d!\n", chassis_slot_index, retval);
		return retval;
	}

	/* delay for 150ms */
	usleep(150000);
	
	gpio_args.op_addr = AX7_GPIO_TX_SET_REG_ADDR;
	gpio_args.op_value = 1 << (chassis_slot_index + 7); 
	gpio_args.op_len = 64;
	retval = ioctl (g_bm_fd,BM_IOC_X_,&gpio_args);
	if( 0 == retval ) {
		syslog_ax_cpld_dbg("write gpio TX_SET register reset slot %d got %#-16x\n", chassis_slot_index, gpio_args.op_value);
		
	}
	else {
		syslog_ax_cpld_err("write gpio TX_SET to reset slot %d error %d!\n", chassis_slot_index, retval);
		return retval;
	}
	
	return retval;
}
/**********************************************************************************
 * nbm_query_localboard_slotno
 *
 * 	This function read CPLD register to get which chassis slot the current board is inserted in 
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		slotno - current board slot number.
 *
 *	RETURN:
 *		0 - if no error occur.
 *		NPD_FAIL - if error occurred.
 *
 **********************************************************************************/
int nbm_query_localboard_slotno
(
	unsigned char *slotno
)
{
	unsigned char regValue = 0;
	int ret = 0;

	ret = nbm_cpld_reg_read(AX_LOCALBOARD_SLOTNO_CPLD_REG, &regValue);
	if(!ret) {
		*slotno = (regValue & 0x1);
		nbm_syslog_dbg("nbm local board slot number read %#x means slot %d\n", regValue, *slotno);
	}
	else {
		nbm_syslog_err("nbm local board slot number read fail %d\n", ret);
	}
	
	return ret;
}
/**********************************************************************************
 * nbm_query_peerboard_status
 *
 * 	This function read CPLD register to get peer board on-slot status
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		state - peer board on-slot status.
 *				0 - peer board is on slot
 *				1 - peer board isn't on slot
 *
 *	RETURN:
 *		0 - if no error occur.
 *		NPD_FAIL - if error occurred.
 *
 **********************************************************************************/
int nbm_query_peerboard_status
(
	unsigned char *state
)
{
	unsigned char regValue = 0;
	int ret = 0;

	ret = nbm_cpld_reg_read(AX_PEERBOARD_STATUS_CPLD_REG, &regValue);
	if(!ret) {
		*state = (regValue & 0x1);
		nbm_syslog_dbg("nbm peer board status read %#x means slot %s\n", regValue, (0 == *state) ? "on":"off");
	}
	else {
		nbm_syslog_err("nbm peer board status read fail %d\n", ret);
	}
	
	return ret;
}

/**********************************************************************************
 * nbm_reset_peerboard
 *
 * 	This function write CPLD register to reset peer board.
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *
 *	RETURN:
 *		0 - if no error occur.
 *		NPD_FAIL - if error occurred.
 *	NOTE:
 *		To invoke a reset procedure, write 0 and delay 10 ms and then write 1 to
 *	the control register, illustrated as follows:
 *		WR 0 --> delay 10 ms --> WR 1
 *
 **********************************************************************************/
int nbm_reset_peerboard
(
	void
)
{
	unsigned char regValue = 0;
	int ret = 0;

	ret = nbm_cpld_reg_write(AX_PEERBOARD_RESET_CPLD_REG, regValue);
	if(ret) {
		nbm_syslog_err("nbm reset peer board phase 1 of 3 error %d\n",ret);
		return ret;
	}

	/* delay 10ms */
	usleep(10000);
	
	regValue = 1;
	ret = nbm_cpld_reg_write(AX_PEERBOARD_RESET_CPLD_REG, regValue);
	if(ret) {
		nbm_syslog_err("nbm reset peer board phase 3 of 3 error %d\n",ret);
		return ret;
	}
	
	nbm_syslog_dbg("nbm reset peer board done!\n");
	return ret;
}
#ifdef __cplusplus
}
#endif
