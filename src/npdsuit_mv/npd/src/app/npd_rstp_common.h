#ifndef __NPD_RSTP_COMMON_H__
#define __NPD_RSTP_COMMON_H__

/* include header file begin */
/* kernel or sys part */
/* user or app part */
/* include header file end */

/* MACRO definition begin */
			/* notations */
/* MACRO definition end */

/* structure definition begin */

#ifndef __NPD_CMD_RSTP__


typedef enum 
{
		PORT_ENABLE_E = 0,
		PORT_DISABLE_E,
		INTERFACE_ADD_E,
		INTERFACE_DEL_E,
		VLAN_ADD_ON_MST_E,
		VLAN_DEL_ON_MST_E,
		FDB_ENTRY_ADD_E,
		FDB_ENTRY_DEL_E,
		FDB_ENTRY_CLEAR_E,
		STP_STATE_UPDATE_E,
		STP_RECEIVE_TCN_E,
		LINK_CHANGE_EVENT_E,
		STP_GET_SYS_MAC_E,
		RSTP_OP_TYPE_MAX
}RSTP_OP_TYPE_E;

typedef struct npd_rstp_intf_op_param
{
	unsigned short vid;
	unsigned int	portIdx;	
	unsigned int isWan;
}NPD_RSTP_INTF_OP_PARAM_T;

typedef struct npd_mstp_vlan_op_param
{
	unsigned short vid;
	unsigned int untagbmp[2];
	unsigned int tagbmp[2];
}NPD_MSTP_VLAN_OP_PARAM_T;

typedef struct npd_rstp_fdb_op_param
{
	unsigned int	portIdx;
	unsigned char	MacDa[6];
}NPD_RSTP_FDB_OP_PARAM_T;

typedef enum
{
	CPSS_RSTP_PORT_STATE_DISABLE_E = 0,
	CPSS_RSTP_PORT_STATE_DISCARD_E,
	CPSS_RSTP_PORT_STATE_LEARN_E,
	CPSS_RSTP_PORT_STATE_FORWARD_E
}RSTP_PORT_STATE_E;

typedef enum
{
    BCM_STG_STP_DISABLE = 0, /* Disabled. */
    BCM_STG_STP_BLOCK , /* BPDUs/no learns. */
    BCM_STG_STP_LISTEN , /* BPDUs/no learns. */
    BCM_STG_STP_LEARN, /* BPDUs/learns. */
    BCM_STG_STP_FORWARD , /* Normal operation. */
    BCM_STG_STP_COUNT  
}RSTP_PORT_STATE_BCM_E;

typedef enum
{
   NAM_STP_PORT_STATE_DISABLE_E = 0,
   NAM_STP_PORT_STATE_DISCARD_E,
   NAM_STP_PORT_STATE_LEARN_E,
   NAM_STP_PORT_STATE_FORWARD_E

}NAM_RSTP_PORT_STATE_E;

typedef struct npd_recv_info_param
{
	unsigned int 	mstid;
	unsigned short vid;
	unsigned int		portIdx;
	RSTP_PORT_STATE_E	portState;	
}NPD_RECV_INFO_PARAM_T;

typedef enum npd_rstp_link_event 
{
 	LINK_PORT_UP_E  = 0,
    LINK_PORT_DOWN_E ,
	LINK_PORT_MAX
}NPD_RSTP_LINK_ENT;

typedef struct npd_rstp_link_op_param
{
	unsigned int		portIdx;
	NPD_RSTP_LINK_ENT	event;
	unsigned int        speed;
	unsigned int        duplex_mode;
}NPD_RSTP_LINK_OP_PARAM_T;
	
typedef union
{
	NPD_RSTP_INTF_OP_PARAM_T	cmdIntf;
	NPD_MSTP_VLAN_OP_PARAM_T  cmdVlan;
	NPD_RSTP_FDB_OP_PARAM_T		cmdFdb;
	NPD_RECV_INFO_PARAM_T		cmdStp;
	NPD_RSTP_LINK_OP_PARAM_T	cmdLink;
	
}NPD_RSTP_OP_PARAM_U;

typedef struct
{
	RSTP_OP_TYPE_E		type;
	unsigned int		length;
	NPD_RSTP_OP_PARAM_U	value;
}NPD_CMD_STC;

typedef struct
{
	int ret;
	NPD_CMD_STC cmd;
}NPD_RECV_INFO;

/*
 * typedef: enum CPSS_STP_STATE_ENT
 *
 * Description: Enumeration of STP Port state
 *
 * Enumerations:
 *      CPSS_STP_DISABLED_E   -  STP disabled on this port.
 *      CPSS_STP_BLCK_LSTN_E  -  blocking/Listening.
 *      CPSS_STP_LRN_E        -  learning.
 *      CPSS_STP_FRWRD_E      -  forwarding.
 */
typedef enum
{
    CPSS_STP_DISABLED_E = 0,
    CPSS_STP_BLCK_LSTN_E,
    CPSS_STP_LRN_E,
    CPSS_STP_FRWRD_E
} CPSS_STP_STATE_ENT;

#endif

/*show running cfg mem*/
#define NPD_STP_RUNNING_CFG_MEM (3*1024) 

int	npd_rstp_sock_init();

 /*int	npd_cmd_write_ro_rstp
(
	NPD_CMD_STC		*npdCmdPtr,
	unsigned int	cmdLen
);*/

int npd_rstp_add_port
(
	unsigned int index
);

int npd_rstp_del_port
(
	unsigned int index
);

/*int npd_rstp_recv_info
(
	NPD_RECV_INFO	*stpInfoPtr,
	unsigned int  infoLen
);*/

int npd_rstp_link_change
(
	unsigned int eth_g_index,
	struct stp_info_s	*stpInfo,/*void	*	paramPtr,*/
	enum ETH_PORT_NOTIFIER_ENT	event
);
int npd_read_stp_ports_state();
int npd_read_socket();
int npd_stp_vlan_bind_to_stpid(void);

int npd_mstp_disable_port
(
	unsigned int port_index
);

int npd_mstp_enable_port
(
	unsigned int port_index
);

int npd_mstp_add_port
(
	unsigned short vid,
	unsigned int index,
	unsigned int iswan
);

int npd_mstp_del_port
(
	unsigned short vid,
	unsigned int index
);

int npd_mstp_add_vlan_on_mst
(
	unsigned short vid,
	unsigned int untagbmp[],
	unsigned int tagbmp[]
);

int npd_mstp_del_vlan_on_mst
(
	unsigned short vid
);
int npd_stp_struct_init();
int npd_mstp_get_port_state
(
    unsigned short vid,
    unsigned int port_index,
    NAM_RSTP_PORT_STATE_E*  state 
);

extern int nam_asic_stp_port_enable
(
	unsigned char devNum, 
	unsigned int  portId,
	unsigned char stp_enable
);

extern unsigned int nam_fdb_enable_port_auto_learn_set
(
	unsigned int index,
	unsigned int state
);

extern unsigned int npd_arp_clear_by_port
(
    unsigned int eth_g_index
);
extern unsigned int  cpssDxChBrgStpStateSet
(
    unsigned char                   dev,
    unsigned char                   port,
    unsigned short                  stpId,
    CPSS_STP_STATE_ENT              state			
);

extern unsigned int cpssDxChBrgVlanToStpIdBind
(
    unsigned char   devNum,
    unsigned short  vlanId,
    unsigned short  stpId
);

extern unsigned int cpssDxChBrgVlanStpIdGet
(
    unsigned char    devNum,
    unsigned short   vlanId,
    unsigned short* stpIdPtr
);
extern unsigned int cpssDxChBrgStpEntryGet
(
    unsigned char dev,
    unsigned short  stpId,
    unsigned int stpEntryPtr[2]
);
extern  void npd_get_port_speed
(
	unsigned int eth_g_index,
	unsigned int *speed
) ;
/**************************************************************************************
 *	 NPD dbus operation
 *
 **************************************************************************************/
DBusMessage * npd_dbus_ethports_interface_config_stp
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_stp_all_vlans_bind_to_stpid
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_stp_get_port_link_state
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) ;

DBusMessage * npd_dbus_rstp_slot_port_get
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) ;

DBusMessage * npd_dbus_rstp_get_one_port_index
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_rstp_get_all_port_index
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);
DBusMessage * npd_dbus_rstp_get_all_port_index_v1
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_stp_set_port_pathcost
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_stp_set_port_prio
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);
DBusMessage * npd_dbus_stp_set_port_nonstp
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_stp_set_port_p2p
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_stp_set_port_edge
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);
DBusMessage * npd_dbus_stp_show_running_cfg
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_get_broad_stp_function
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

/* local functions declearations end */
#endif /* __FILENAME_H_ */
