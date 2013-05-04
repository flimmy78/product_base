#ifndef __NPD_QOS_H__
#define __NPD_QOS_H__

#include <dbus/dbus.h>

#define QOS_ERROR_NONE	-1
#define QOS_SUCCESS			        (QOS_ERROR_NONE+1)
#define QOS_FAIL                    (QOS_ERROR_NONE+2)
#define QOS_BAD_PARAM               (QOS_ERROR_NONE+3)
#define QOS_PROFILE_EXISTED			(QOS_ERROR_NONE + 4)
#define QOS_PROFILE_NOT_EXISTED 	(QOS_ERROR_NONE + 5)
#define QOS_POLICY_EXISTED			(QOS_ERROR_NONE + 6)
#define QOS_POLICY_NOT_EXISTED 		(QOS_ERROR_NONE + 7)
#define QOS_POLICY_MAP_BIND		    (QOS_ERROR_NONE + 8)
#define QOS_POLICER_NOT_EXISTED 	(QOS_ERROR_NONE + 9)
#define QOS_COUNTER_NOT_EXISTED 	(QOS_ERROR_NONE + 10)
#define QOS_POLICY_MAP_PORT_WRONG   (QOS_ERROR_NONE + 11)
#define QOS_POLICER_USE_IN_ACL      (QOS_ERROR_NONE + 12)
#define QOS_TRAFFIC_NO_INFO         (QOS_ERROR_NONE + 13)
#define QOS_PROFILE_IN_USE         (QOS_ERROR_NONE + 14)
#define QOS_POLICER_CBS_BIG         (QOS_ERROR_NONE + 15)
#define QOS_POLICER_CBS_LITTLE         (QOS_ERROR_NONE + 16)
#define QOS_NO_MAPPED         (QOS_ERROR_NONE + 17)

#define QOS_BAD_PTR					(QOS_ERROR_NONE + 100)

#define MAX_PROFILE_NUMBER			128
#define MAX_UP_PROFILE_NUM  		8
#define MAX_DSCP_PROFILE_NUM  		64
#define MAX_DSCP_DSCP_NUM  			64
#define MAX_POLICY_MAP_NUM 			1001
#define MAX_POLICER_NUM				256
#define MAX_COUNTER_NUM				16
#define MAX_HYBRID_DSCP			64	/*1~64*/
#define MAX_HYBRID_UP				71	/*65~72*/
#define MAX_PORT   					24

#define QOS_POLICER_ENABLE	1
#define QOS_POLICER_DISABLE 0
#define QOS_SCH_GROUP_IS_SP	1024
#define ACL_ANY_PORT_CHAR 255

#define NPD_QOS_SHOWRUN_CFG_SIZE 100*1024

QOS_POLICER_STC				**g_policer;

/*#define QOS_POLICER_NULL_PTR_CHECK(ptr) if(ptr==NULL) return QOS_BAD_PTR*/
void npd_qos_init(void);
unsigned int npd_qos_mode_def(void);
unsigned int npd_qos_unbind_remap_port(void);

/*********************DBusMessage**************************/
DBusMessage * npd_dbus_config_qos_profile(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_qos_profile_attributes(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_dscp_to_qos_profile_table(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_dscp_to_dscp_table(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_up_to_qos_profile(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_delete_dscp_to_qos_profile_table(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_delete_dscp_to_dscp_table(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_delete_up_to_qos_profile(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_create_policy_map(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_config_policy_map(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_policy_modify_qos(DBusConnection *conn, DBusMessage *msg, void *user_data);
/*DBusMessage * npd_dbus_policy_default_up(DBusConnection *conn, DBusMessage *msg, void *user_data);*/
/*DBusMessage * npd_dbus_policy_default_qos_profile(DBusConnection *conn, DBusMessage *msg, void *user_data);*/
DBusMessage * npd_dbus_policy_trust_mode_l2_set(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_policy_trust_mode_l3_set(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_policy_trust_mode_l2_l3_set(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_policy_trust_mode_untrust_set(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_ethport_bind_policy_map(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_ethport_show_policy_map(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_show_qos_profile(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_show_policy_map(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_policer_set(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_policer_cir_cbs(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_policer_set_out_profile(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_policer_out_profile_cmd_keep_drop(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_policer_out_profile_cmd_remap(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_policer_enable(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_global_policer_meter_mode(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_global_policing_mode(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_set_counter(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_policer_counter(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_read_counter(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_policer_color(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_show_policer(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_ethport_unbind_policy_map(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_qos_profile_show_running_config(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_qos_remap_table_show_running_config(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_qos_policy_map_show_running_config(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_queue_sche_wrr_group(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_queue_scheduler(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_show_queue_scheduler(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_qos_policer_show_running_config(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_qos_queue_sch_show_running_config(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_qos_mode_show_running_config(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_traffic_shape(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_show_traffic_shape(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_delete_traffic_shape(DBusConnection *conn, DBusMessage *msg, void *user_data);






typedef struct
{
	unsigned int profileIndex;
	unsigned int TC;
	unsigned int DP;
	unsigned int UP;
	unsigned int DSCP;
	unsigned int EXP;

}QOS_PROFILE_SHOW_STC;

typedef struct
{
	unsigned int 					policyIndex;
	/*unsigned int 			 		defaultUp;*/
	unsigned int				    assignPrecedence;
	/*unsigned int 					defaultProfileIndex;*/
	QOS_PORT_TRUST_MODE_FLAG 		trustFlag;
	QOS_NORMAL_ENABLE_E      		modifyUp;
	QOS_NORMAL_ENABLE_E      		modifyDscp;
	unsigned int	         		remapDscp;
	unsigned int					bindport_num;
}POLICY_MAP_SHOW_STC;

typedef struct
{
	  unsigned int 								policerIndex;
	  unsigned int                              policerEnable;
	  unsigned int 								sharemode;
      QOS_POLICER_COLOR_MODE_ENT                meterColorMode;
	  unsigned long								cir;
	  unsigned long								cbs;
      unsigned int                              counterEnable;
      unsigned int                              counterSetIndex;
      QOS_PROFILE_OUT_PROFILE_ACTION_E          cmd;
      unsigned int                              qosProfile;    
	  unsigned int								mru;
	  unsigned int 								mode;
	  unsigned int								packetsize;
}POLICER_SHOW_STC;


typedef struct
{
	unsigned int profileId;
}QOS_UP_PROFILE_SHOW;

typedef struct
{
	unsigned int profileId;
}QOS_DSCP_PROFILE_SHOW;

typedef struct
{
	unsigned int remapDscp;
}QOS_DSCP_DSCP_SHOW;

typedef struct
{
	unsigned int specflag;
	unsigned int profileIndex;
}QOS_REMAP_BACK_VALUE_SHOW;
typedef struct{
	unsigned int	groupFlag;	
	unsigned int	weight;
}QOS_WRR_SHOW_TX_WEIGHT_E;
typedef enum
{
     CPSS_PORT_TX_SCHEDULER_PROFILE_1_E=0,
     CPSS_PORT_TX_SCHEDULER_PROFILE_2_E,
     CPSS_PORT_TX_SCHEDULER_PROFILE_3_E,
     CPSS_PORT_TX_SCHEDULER_PROFILE_4_E,
     CPSS_PORT_TX_SCHEDULER_PROFILE_5_E,
     CPSS_PORT_TX_SCHEDULER_PROFILE_6_E,
     CPSS_PORT_TX_SCHEDULER_PROFILE_7_E,
     CPSS_PORT_TX_SCHEDULER_PROFILE_8_E
} CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT;
typedef enum
{
     QOS_MODE_PORT=0,
     QOS_MODE_FLOW,
     QOS_MODE_HYBRID,
     QOS_MODE_DEFAULT
} QOS_MODE;
unsigned int npd_qos_port_bind_check
(
	unsigned int eth_g_index,
	unsigned int *ID
);
unsigned int npd_qos_port_bind_opt
(
	unsigned int eth_g_index,
	unsigned int policyMapIndex
);
unsigned int npd_qos_clear_eth_policymap_if_hwfail
(
	unsigned int eth_g_index,
	unsigned int policyMapIndex
);

extern int npd_acl_flow_to_port
(
	void
);

extern int npd_find_share_num_by_policer_id
(
	int policerId, 
	int *num
);

extern unsigned int nam_qos_queue_schedu_check
(
	int wrrflag, 
	int groupflag
);


extern unsigned long appDemoPpConfigDevAmount;

extern unsigned int nam_qos_profile_entry_set
(
     unsigned int     profileIndex,
     QOS_PROFILE_STC *profile
);
extern unsigned int nam_qos_up_to_profile_entry
(
	unsigned int up,
	unsigned int profileIndex,
	int unit,
	int port	
);

extern unsigned int bcm_nam_qos_up_to_profile_entry
(
	unsigned int up,
	unsigned int profileIndex,
	int unit,
	int port
);

extern unsigned int nam_qos_dscp_to_dscp_entry
(
	unsigned int oldDscp,
	unsigned int newDscp
);
extern unsigned int nam_qos_dscp_to_profile_entry
(
	unsigned int dscp,
	unsigned int profileIndex,
	int unit,
	int port
);

extern unsigned int bcm_nam_qos_dscp_to_profile_entry
(
	unsigned int dscp,
	unsigned int profileIndex,
	int unit,
	int port
);

extern unsigned int nam_qos_port_bind_policy_map
(
	unsigned char dev,
	unsigned char port,
	unsigned int policyIndex
);

extern unsigned int bcm_nam_qos_support_check
(
	void
);

extern unsigned int nam_qos_global_policing
(
	unsigned int policing
);
extern unsigned int nam_qos_global_policer_meter
(
	unsigned int metermode,
	unsigned int loosemru
);
extern unsigned int nam_qos_policer_invalid
(
	unsigned int policerIndex
);
extern unsigned int nam_qos_policer_entry_set
(
	unsigned int 		policerIndex,
	QOS_POLICER_STC 	*policer,
	unsigned long		*cir,
	unsigned long       *cbs
);
extern unsigned int nam_qos_set_counter
(
	unsigned int    countIndex,
	unsigned long   inIndex,
	unsigned long	outIndex
);

extern unsigned int nam_qos_read_counter
(
	unsigned int counterIndex,
	unsigned long *Inprofile,
	unsigned long *Outprofile
);
extern unsigned int nam_qos_policer_init();
unsigned int npd_qos_profile_index_check
(
	unsigned int profileIndex
);
extern unsigned int npd_qos_profile_in_acl_check
 (
 	unsigned int profileIndex
 );
unsigned int npd_qos_profile_relate_check
(
	unsigned int profileIndex
);
extern unsigned int npd_acl_policer_use_check
(
	unsigned int policerId
);
extern unsigned int nam_qos_port_unbind_policy_map
(
	unsigned char devNum,
	unsigned char  virPort,
	unsigned int  policyIndex
);
extern unsigned int nam_qos_schedu_wrr_set
(	
	unsigned int  wrrflag,
	unsigned int  groupFlag,
	unsigned int  tcQueue,
	unsigned int  wrrWeight
);
extern unsigned int nam_qos_queue_sp_set_one
(
	unsigned int  tcQueue
);

extern unsigned int nam_qos_queue_sp_set();
extern unsigned int nam_qos_queue_init();
extern unsigned int nam_qos_traffic_shape_port
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned char mode,
	unsigned int  burstSize,
	unsigned long maxRatePtr
);
extern unsigned int nam_qos_del_traffic_shape_port
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned char mode
);
extern unsigned int nam_qos_traffic_shape_queue_port
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned char mode,
	unsigned int  tcQueue,
	unsigned int  burstSize,
	unsigned long userRatePtr
);
extern unsigned int nam_qos_del_traffic_shape_queue_port
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned char mode,
	unsigned int tcQueue
);
extern unsigned int nam_qos_policer_cir_cbs_check
(
	unsigned int  cir,
	unsigned int  cbs
);
extern unsigned long cpssDxChPortTxQueueingEnableSet
(
    unsigned char     devNum,
    unsigned char     portNum,
    unsigned char     tcQueue,
    unsigned long     enable
);
extern unsigned long prvCpssDrvHwPpSetRegField
(
    unsigned char devNum,
    unsigned long regAddr,
    unsigned long fieldOffset,
    unsigned long fieldLength,
     unsigned int fieldData
);

#endif

