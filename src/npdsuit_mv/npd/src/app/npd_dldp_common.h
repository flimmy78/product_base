#ifndef __NPD_DLDP_COMMON_H__
#define __NPD_DLDP_COMMON_H__

/*********************************************************
*	head files														*
**********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>

#include "util/npd_list.h"
#include "sysdef/npd_sysdef.h"
#include "npd/nam/npd_amapi.h"
#include "npd/nbm/npd_bmapi.h"
#include <dbus/dbus.h>

#include "npd_c_slot.h"
#include "npd_vlan.h"
#include "npd_product.h"
#include "npd_eth_port.h"
#include "npd_log.h"
#include "npd_rstp_common.h"

/*********************************************************
*	macro define													*
**********************************************************/
#define NPD_DLDP_RETURN_CODE_0			(0)						/* success					*/
#define NPD_DLDP_RETURN_CODE_1			(1)						/* error						*/
#define NPD_DLDP_RETURN_CODE_BASE		(0x140000)				/* return code base 			*/
#define NPD_DLDP_RETURN_CODE_OK			(0x140000)				/* success					*/
#define NPD_DLDP_RETURN_CODE_ERROR		(0x140001)				/* error 						*/
#define NPD_DLDP_RETURN_CODE_NOT_ENABLE_GBL (0x140004)			/* DLDP not enabled global		*/

#define NPD_DLDP_RETURN_CODE_VLAN_NOT_EXIST (0x140009)			/* L2 vlan not exist			*/
#define NPD_DLDP_RETURN_CODE_NOTENABLE_VLAN (0x14000a)			/* L2 vlan not enable DLDP		*/
#define NPD_DLDP_RETURN_CODE_HASENABLE_VLAN (0x14000b)			/* L2 vlan has enable DLDP		*/
#define NPD_DLDP_RETURN_CODE_NULL_PTR		(0x14000c)			/* parameter pointer is null		*/

#define	NPD_DLDP_INIT_0					(0)						/* init int/short/long variable	*/
#define NPD_DLDP_INIT_FD				(-1)					/* initialize fd, default is -1		*/

#define	NPD_DLDP_ENABLE					(1)						/* DLDP global status, enable	*/
#define	NPD_DLDP_DISABLE				(0)						/* DLDP global status, disenable	*/
#define	NPD_DLDP_VLAN_ENABLE			(1)						/* DLDP vlan status, enable		*/
#define	NPD_DLDP_VLAN_DISABLE			(0)						/* DLDP vlan status, disenable	*/
#define	NPD_DLDP_PORT_ENABLE			(1)						/* DLDP port status, enable		*/
#define	NPD_DLDP_PORT_DISABLE			(0)						/* DLDP port status, disenable	*/

#define NPD_DLDP_MAC_ADDR_LEN			(6)						/* length of the mac address	*/
#define NPD_MSG_FROM_DLDP_MAX_SIZE		sizeof(NPD_MSG_FROM_DLDP_T)
																/* 1248						*/ 
#define NPD_MSG_TO_DLDP_MAX_SIZE		sizeof(NPD_MSG_TO_DLDP_T)

#define NPD_NPD2DLDP_CLIENT				"/tmp/npd2dldp_client"	/* npd to DLDP client			*/
#define NPD_NPD2DLDP_SERVER				"/tmp/npd2dldp_server"	/* npd to DLDP server			*/	

#define NPD_DLDP_SYS_SET_INIT			(1)						/* type of DLDP system set, init	*/
#define NPD_DLDP_SYS_SET_STOP			(2)						/* type of DLDP system set, stop	*/
#define NPD_DLDP_SYS_RECOVER_PORT		(3)						/* revover port which support DLDP	*/
#define NPD_DLDP_SYS_SHUTDOWN_PORT		(4)						/* shutdown port which support DLDP	*/

#define NPD_DLDP_MAC_ADDR_LEN			(6)						/* length of the mac address	*/

#define NPD_DLDP_TYPE_DEVICE_EVENT		(3)						/* device message				*/
#define	NPD_DLDP_TYPE_NOTIFY_MSG		(1)						/* notify message				*/
#define	NPD_DLDP_FLAG_ADDR_MOD			(3)						/* notify infor for modify address	*/

/* device notify event 			*/
#define NPD_DLDP_EVENT_DEV_UP			(0x0001)
#define NPD_DLDP_EVENT_DEV_DOWN			(0x0002)
#define NPD_DLDP_EVENT_DEV_REGISTER		(0x0003)
#define NPD_DLDP_EVENT_DEV_UNREGISTER	(0x0004)
#define NPD_DLDP_EVENT_DEV_VLAN_ADDIF	(0x0005)
#define NPD_DLDP_EVENT_DEV_VLAN_DELIF	(0x0006)
#define NPD_DLDP_EVENT_DEV_SYS_MAC_NOTI	(0x0007)
#define NPD_DLDP_EVENT_DEV_SYS_DISABLE  (0x0008)

/*show running cfg mem*/
#define NPD_DLDP_RUNNING_CFG_MEM (1024*1024)


/*********************************************************
*	struct define													*
**********************************************************/
typedef struct NPD_DLDP_NLMSGHDR_S
{
	unsigned int	nlmsg_len;					/* Length of message including header */
	unsigned short	nlmsg_type; 				/* Message content	*/
	unsigned short	nlmsg_flags;				/* Additional flags 	*/
	unsigned int	nlmsg_seq;					/* Sequence number	*/
	unsigned int	nlmsg_pid;					/* Sending process PID	*/
}NPD_DLDP_NLMSGHDR_T;

typedef struct NPD_DEV_EVENT_CMD_DLDP_S
{
	unsigned long  event_type;					/* event type					*/
	unsigned short vlan_id;						/* vlan id					*/
	unsigned int   ifindex;						/* eth-port index				*/
	unsigned int   soltno;						/* asic-port soltno			*/
	unsigned int   portno;						/* asic-port portno			*/
	unsigned int   tagmode;						/* asic-port tag mode			*/
}NPD_DEV_EVENT_CMD_DLDP_T;


/*******************************************/
/* NPD notify message to DLDP						*/
/*******************************************/
typedef struct NPD_NOTIFY_MOD_DLDP_S
{
	unsigned long  mod_type;
	unsigned short vlan_id;
	unsigned long  ifindex;
	unsigned int   slot_no; 					/* solt no of port		*/
	unsigned int   port_no; 					/* port no of port		*/
	unsigned int   tagmode; 					/* asic-port tag mode	*/
	unsigned char  sys_mac[NPD_DLDP_MAC_ADDR_LEN];
	unsigned long  reserve;
}NPD_NOTIFY_MOD_DLDP_T;

typedef struct NPD_MSG_DLDP_S{
	NPD_DLDP_NLMSGHDR_T nlh;
	NPD_NOTIFY_MOD_DLDP_T npd_notify_msg;
}NPD_MSG_TO_DLDP_T;


/*******************************************/
/* DLDP notify message to NPD						*/
/*******************************************/
typedef struct NPD_DLDP_NOTIFY_MOD_NPD_S
{
	unsigned long mod_type;			/* modify type		*/
	unsigned short vlan_id;			/* vlan id			*/
	unsigned long ifindex;			/* port index			*/
	unsigned long reserve;			/*					*/
}NPD_DLDP_NOTIFY_MOD_NPD_T;

typedef struct NPD_MSG_FROM_DLDP_S{
	NPD_DLDP_NLMSGHDR_T nlh;
	NPD_DLDP_NOTIFY_MOD_NPD_T dldp_notify_msg;
}NPD_MSG_FROM_DLDP_T;


/*********************************************************
*	function declare												*
**********************************************************/

/**********************************************************************************
 * npd_get_dldp_global_status
 *		get DLDP enable/disable global status
 *
 *	INPUT:
 *		NULL
 *
 *	OUTPUT:
 *		unsigned char *status
 *
 *	RETURN:
 *		NPD_DLDP_RETURN_CODE_OK		- success
 *	 	NPD_DLDP_RETURN_CODE_ERROR		- fail
 **********************************************************************************/
unsigned int npd_get_dldp_global_status
(
	unsigned char *status
);

/**********************************************************************************
 * npd_dldp_init
 *		init DLDP enable/disable global status in npd
 *
 *	INPUT:
 *		NULL
 *
 *	OUTPUT:
 *		NULL
 *
 *	RETURN:
 *		NULL
 *	 
 **********************************************************************************/
void npd_dldp_init
(
	void
);

/**********************************************************************************
 * npd_dldp_init
 *		init npd to dldp socket 
 *
 *	INPUT:
 *		NULL
 *
 *	OUTPUT:
 *		NULL
 *
 *	RETURN:
 *		sock								- fd of the socket
 *	 	(-NPD_DLDP_RETURN_CODE_ERROR)	- fail
 *
 **********************************************************************************/
int	npd_dldp_sock_init
(
	void
);

/**********************************************************************************
 * npd_dldp_recv_info
 *		receive message from DLDP
 *
 *	INPUT:
 *		NULL
 *
 *	OUTPUT:
 *		NULL
 *
 *	RETURN:
 *		len								- length of receive message
 *	 	(-NPD_DLDP_RETURN_CODE_ERROR)	- fail
 *
 **********************************************************************************/
unsigned int npd_dldp_recv_info
(
	NPD_MSG_FROM_DLDP_T *msg,
	unsigned int infoLen
);

/**********************************************************************************
 * npd_cmd_sendto_dldp
 *		send message to DLDP
 *
 *	INPUT:
 *		NPD_DEV_EVENT_CMD_DLDP_T *mngCmd
 *
 *	OUTPUT:
 *		NULL
 *
 *	RETURN:
 *		NPD_DLDP_RETURN_CODE_OK		- success
 *	 	NPD_DLDP_RETURN_CODE_ERROR		- fail
 *
 **********************************************************************************/
 unsigned int npd_cmd_sendto_dldp
(
	NPD_DEV_EVENT_CMD_DLDP_T *mngCmd	
);

/**********************************************************************************
 * npd_dldp_sysmac_notifer
 *		init npd to dldp socket 
 *
 *	INPUT:
 *		NULL
 *
 *	OUTPUT:
 *		NULL
 *
 *	RETURN:
 *		NPD_DLDP_RETURN_CODE_OK		- success
 *	 	NPD_DLDP_RETURN_CODE_ERROR		- fail
 *
 **********************************************************************************/
unsigned int npd_dldp_sysmac_notifer
(
	void
);

/**********************************************************************************
 * npd_dldp_global_disable
 *		clear flag in vlan node and port node
 *
 *	INPUT:
 *		NULL
 *
 *	OUTPUT:
 *		NULL
 *
 *	RETURN:
 *		NPD_DLDP_RETURN_CODE_OK		- success
 *	 	NPD_DLDP_RETURN_CODE_ERROR		- fail
 *
 **********************************************************************************/
unsigned int npd_dldp_global_disable
(
	void
);

/**********************************************************************************
 * npd_dldp_recover_port_status
 *		recover port state of DLDP
 *
 *	INPUT:
 *		dldp_notify	- message for modify hardware
 *
 *	OUTPUT:
 *		NULL
 *
 *	RETURN:
 *		NPD_DLDP_RETURN_CODE_OK				- success
 *		NPD_DLDP_RETURN_CODE_ERROR				- fail
 *	 	NPD_DLDP_RETURN_CODE_NULL_PTR			- parameter is null
 *
 **********************************************************************************/
unsigned int npd_dldp_recover_port_status
(
	NPD_DLDP_NOTIFY_MOD_NPD_T *dldp_notify
);

/**********************************************************************************
 * npd_dldp_discard_port_status
 *		set port state of DLDP is discard
 *
 *	INPUT:
 *		dldp_notify	- message for modify hardware
 *
 *	OUTPUT:
 *		NULL
 *
 *	RETURN:
 *		NPD_DLDP_RETURN_CODE_OK				- success
 *		NPD_DLDP_RETURN_CODE_ERROR				- fail
 *	 	NPD_DLDP_RETURN_CODE_NULL_PTR			- parameter is null
 *
 **********************************************************************************/
unsigned int npd_dldp_discard_port_status
(
	NPD_DLDP_NOTIFY_MOD_NPD_T *dldp_notify
);

/**********************************************************************************
 * npd_dldp_check_vlan_status
 *		check vlan exist, if exist get DLDP status.
 *
 *	INPUT:
 *		 unsigned short vlanid		- l2 vlan id
 *
 *	OUTPUT:
 *		unsigned char *status		- vlan's  DLDP status

 *
 *	RETURN:
 *		NPD_DLDP_RETURN_CODE_OK				- success
 *		NPD_DLDP_RETURN_CODE_ERROR				- fail
 *	 	NPD_DLDP_RETURN_CODE_VLAN_NOT_EXIST	- L2 vlan not exist
 *
 **********************************************************************************/
unsigned int npd_dldp_check_vlan_status
(
	unsigned short vlanid,
	unsigned char *status
);

/**********************************************************************************
 * npd_dldp_vlan_port_endis_flag_set
 *		set port DLDP flag in vlan
 *
 *	INPUT:
 *		unsigned short vlanId		- vlan id
 *		unsigned int  eth_g_idx		- eth-port index
 *		unsigned char tagMode		- tag/untag
 *		unsigned char en_dis		- enable/disable
 *
 *	OUTPUT:
 *		NULL
 *
 *	RETURN:
 *		NPD_DLDP_RETURN_CODE_OK				- success
 *		NPD_DLDP_RETURN_CODE_ERROR				- fail
 *	 	NPD_DLDP_RETURN_CODE_VLAN_NOT_EXIST	- L2 vlan not exist
 *
 **********************************************************************************/
unsigned int npd_dldp_vlan_port_endis_flag_set
(
	unsigned short vlanId,
	unsigned int  eth_g_idx,
	unsigned char tagMode,
	unsigned char en_dis
);

/**********************************************************************************
 * npd_dldp_vlan_endis_config
 *		
 *
 *	INPUT:
 *		unsigned char isEnable		- enable/disable DLDP on vlan
 *		unsigned short vlanid		- l2 vlan id
 *
 *	OUTPUT:
 *		NULL
 *
 *	RETURN:
 *		NPD_DLDP_RETURN_CODE_OK				- success
 *		NPD_DLDP_RETURN_CODE_ERROR				- fail
 *	 	NPD_DLDP_RETURN_CODE_VLAN_NOT_EXIST	- L2 vlan not exist
 *
 **********************************************************************************/
unsigned int npd_dldp_vlan_endis_config
(
	unsigned char isEnable,
	unsigned short vlanid
);

/**********************************************************************************
 * npd_dldp_recvmsg_proc
 *		config dev according to running result of dldp protocol
 *
 *	INPUT:
 *		NULL
 *
 *	OUTPUT:
 *		NULL
 *
 *	RETURN:
 *		NPD_DLDP_RETURN_CODE_OK		- success
 *	 	NPD_DLDP_RETURN_CODE_ERROR		- fail
 *
 **********************************************************************************/
unsigned int npd_dldp_recvmsg_proc
(
	NPD_DLDP_NOTIFY_MOD_NPD_T *dldp_notify
);

/**********************************************************************************
 * npd_dbus_dldp_check_global_status
 *		check and get DLDP enable/disable global status
 *
 * 	INPUT:
 *		DBusConnection *conn, 
 *		DBusMessage *msg, 
 *		void *user_data
 *
 *	OUTPUT:
 * 		NULL
 *
 *	RETURN:
 * 	 	reply
 * 	 
 **********************************************************************************/
DBusMessage *npd_dbus_dldp_check_global_status
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);


/**********************************************************************************
 * npd_dbus_dldp_config_vlan
 *		enable/disable DLDP on vlan and its ports, base vlan 
 *
 *	INPUT:
 *		DBusConnection *conn, 
 *		DBusMessage *msg, 
 *		void *user_data
 *
 *	OUTPUT:
 *		NULL
 *
 *	RETURN:
 *		reply
 *	 	ret:
  *			NPD_DLDP_RETURN_CODE_OK				- success
 *			NPD_DLDP_RETURN_CODE_ERROR				- fail
 *	 		NPD_DLDP_RETURN_CODE_VLAN_NOT_EXIST	- L2 vlan not exist
 *			NPD_DLDP_RETURN_CODE_NOT_ENABLE_GBL	- not enable DLDP global
 *			NPD_DLDP_RETURN_CODE_NOTENABLE_VLAN	- L2 vlan not enable DLDP
 *			NPD_DLDP_RETURN_CODE_HASENABLE_VLAN	- L2 vlan has enabled DLDP
 *
 **********************************************************************************/
DBusMessage *npd_dbus_dldp_config_vlan
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

/**********************************************************************************
 * npd_dbus_dldp_get_product_function
 *		DLDP get product function
 *
 *	INPUT:
 *		DBusConnection *conn, 
 *		DBusMessage *msg, 
 *		void *user_data
 *
 *	OUTPUT:
 *		NULL
 *
 *	RETURN:
 *		reply
 *	 	IsSupport:
 *			1				- function do not support
 *			0				- function support
 *
 **********************************************************************************/

DBusMessage * npd_dbus_dldp_get_product_function
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);


/**********************************************************************************
*npd_dldp_mng_thread_dgram()
*	DESCRIPTION:
*		DLDP command message handle thread

*	INPUTS:
*		NULL
*
*	OUTPUTS:
*		NULL
*
*	RETURN VALUE:
*		NULL
*
***********************************************************************************/
void *npd_dldp_mng_thread_dgram
(
	void
);

/**********************************************************************************
 * npd_dldp_save_vlan_cfg
 *		get string of DLDP show running config
 *
 *	INPUT:
 *		unsigned int bufLen
 *
 *	OUTPUT:
 *		unsigned char *buf
 *		unsigned char *enDis
 *
 *	RETURN:
 *		NULL
 *
 **********************************************************************************/
void npd_dldp_save_vlan_cfg
(
	unsigned char *buf,
	unsigned int bufLen,
	unsigned char *enDis
);

/*********************************************************
*	extern Functions												*
**********************************************************/
extern unsigned int npd_get_slot_index_from_eth_index
(
	unsigned int eth_index
);

extern unsigned int npd_get_port_index_from_eth_index
(
	unsigned int eth_index
);

extern unsigned int  npd_get_slot_no_from_index
(
	 unsigned int slot_index
);

extern unsigned int npd_get_port_no_from_index
(
   unsigned int slot_index,
   unsigned int port_index
);


extern int nam_vlan_stpid_get
(
     unsigned char devNum,
     unsigned short vid,
     unsigned int* stg
);

extern int nam_stp_state_set
(
    unsigned char devNum,
    unsigned char port,
    unsigned short stg,
    unsigned int stpState

);

extern int nam_stp_vlan_bind_stg
(
   unsigned char devNum,
   unsigned short vlanid,
   unsigned int stg
);

extern int nam_dldp_set_vlan_filter
(
	unsigned short vlanId,
	unsigned char enable
);


/**********************************************************************************
 * npd_fdb_add_dldp_vlan_system_mac()
 *	DESCRIPTION:
 *		add a FDB item(system mac and vlanId) when enable DLDP on vlan
 *
 *	INPUTS:
 *		unsigned short vlanId
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		NPD_FDB_ERR_VLAN_NONEXIST	- vlan not exist
 *		NPD_FDB_ERR_NONE			- success
 *		NPD_FDB_ERR_OCCUR_HW		- error when add FDB item to table FDB
***********************************************************************************/
extern unsigned int npd_fdb_add_dldp_vlan_system_mac
(
	unsigned short vlanId
);

/**********************************************************************************
 * npd_fdb_del_dldp_vlan_system_mac()
 *	DESCRIPTION:
 *		del the  FDB item(system mac and vlanId) when enable DLDP on vlan
 *
 *	INPUTS:
 *		unsigned short vlanId
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		NPD_FDB_ERR_VLAN_NONEXIST	- vlan not exist
 *		NPD_FDB_ERR_NONE			- success
 *		NPD_FDB_ERR_OCCUR_HW		- error when del FDB item to table FDB
***********************************************************************************/
extern unsigned int npd_fdb_del_dldp_vlan_system_mac
(
	unsigned short vlanId
);


#endif

