#ifndef __NAM_QOS_H__
#define __NAM_QOS_H__

#include "nam_acl.h"

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
#define INGRESS_TRAFFIC_SHAPE			333
#define EGRESS_TRAFFIC_SHAPE 			444

#define TRUE 1
#define FAUSE 0
extern unsigned long appDemoPpConfigDevAmount;

#if 0

typedef enum
{
    CPSS_PORT_TX_WRR_BYTE_MODE_E,
    CPSS_PORT_TX_WRR_PACKET_MODE_E
} CPSS_PORT_TX_WRR_MODE_ENT;
typedef enum
{
    CPSS_PORT_TX_WRR_MTU_256_E,    /* for Twist backward compatibility */
    CPSS_PORT_TX_WRR_MTU_2K_E,
    CPSS_PORT_TX_WRR_MTU_8K_E
} CPSS_PORT_TX_WRR_MTU_ENT;

typedef struct
{
    unsigned long                                qosProfileId;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT  assignPrecedence;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT        enableModifyUp;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT        enableModifyDscp;
}CPSS_QOS_ENTRY_STC;
typedef enum
{
    CPSS_DP_GREEN_E = 0,
    CPSS_DP_YELLOW_E,
    CPSS_DP_RED_E

}CPSS_DP_LEVEL_ENT;

typedef struct
{
    CPSS_DP_LEVEL_ENT   dropPrecedence;
    unsigned int              userPriority;
    unsigned int              trafficClass;
    unsigned int              dscp;
    unsigned int              exp;
}CPSS_DXCH_COS_PROFILE_STC;

typedef enum
{
    CPSS_QOS_PORT_NO_TRUST_E = 0,
    CPSS_QOS_PORT_TRUST_L2_E,
    CPSS_QOS_PORT_TRUST_L3_E,
    CPSS_QOS_PORT_TRUST_L2_L3_E
}CPSS_QOS_PORT_TRUST_MODE_ENT;

typedef struct{
    unsigned long    cir;
    unsigned long    cbs;
} CPSS_DXCH_POLICER_TB_PARAMS_STC;
typedef enum
{
    CPSS_POLICER_COLOR_BLIND_E,
    CPSS_POLICER_COLOR_AWARE_E
} CPSS_POLICER_COLOR_MODE_ENT;
typedef enum {
    CPSS_DXCH_POLICER_CMD_NONE_E,
    CPSS_DXCH_POLICER_CMD_DROP_RED_E,
    CPSS_DXCH_POLICER_CMD_QOS_MARK_BY_ENTRY_E,
    CPSS_DXCH_POLICER_CMD_QOS_PROFILE_MARK_BY_TABLE_E
} CPSS_DXCH_POLICER_CMD_ENT;

typedef struct{
      unsigned long                             policerEnable;
      CPSS_POLICER_COLOR_MODE_ENT               meterColorMode;
      CPSS_DXCH_POLICER_TB_PARAMS_STC           tbParams;
      unsigned long                             counterEnable;
      unsigned long                             counterSetIndex;
      CPSS_DXCH_POLICER_CMD_ENT                 cmd;
      unsigned long                             qosProfile;
      CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT     modifyDscp;
      CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT     modifyUp;
} CPSS_DXCH_POLICER_ENTRY_STC;
typedef enum 
{
    CPSS_POLICER_TB_STRICT_E,
    CPSS_POLICER_TB_LOOSE_E
}CPSS_POLICER_TB_MODE_ENT;
typedef enum 
{
    CPSS_POLICER_MRU_1536_E,
    CPSS_POLICER_MRU_2K_E,
    CPSS_POLICER_MRU_10K_E
}CPSS_POLICER_MRU_ENT ;
typedef enum
{
    CPSS_POLICER_PACKET_SIZE_TUNNEL_PASSENGER_E, 
    CPSS_POLICER_PACKET_SIZE_L3_ONLY_E, 
    CPSS_POLICER_PACKET_SIZE_L2_INCLUDE_E,
    CPSS_POLICER_PACKET_SIZE_L1_INCLUDE_E
} CPSS_POLICER_PACKET_SIZE_MODE_ENT;
typedef struct{
      unsigned long    outOfProfileBytesCnt;
      unsigned long    inProfileBytesCnt;
} CPSS_DXCH_POLICER_COUNTERS_STC;
typedef enum
{
    CPSS_PORT_TX_WRR_ARB_GROUP_0_E = 0,
    CPSS_PORT_TX_WRR_ARB_GROUP_1_E,
    CPSS_PORT_TX_SP_ARB_GROUP_E
}CPSS_PORT_TX_Q_ARB_GROUP_ENT;
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

#define PRV_CPSS_DXCH_POLICER_TABLE_ENTRY_SIZE_CNS      2

extern unsigned long cpssDxChPortDefaultUPSet
(
    unsigned char    dev,
    unsigned char     port,
   	unsigned char    defaultUserPrio
);

extern unsigned long cpssDxChCosPortQosTrustModeSet
(
    unsigned char                        dev,
    unsigned char                         port,
    CPSS_QOS_PORT_TRUST_MODE_ENT  portQosTrustMode
);

extern unsigned long cpssDxChCosProfileEntrySet
(
	unsigned char						dev,
	unsigned char						profileIndex,
	CPSS_DXCH_COS_PROFILE_STC			*cosPtr
);
extern unsigned long cpssDxChCosDscpMutationEntrySet
(
    unsigned char   dev,
    unsigned char   dscp,
    unsigned char   newDscp
);
extern unsigned long cpssDxChCosUpToProfileMapSet
( 
	unsigned char 	devIdx,
  	unsigned char 	up,
  	unsigned char 	profileIndex
 );
extern unsigned long cpssDxChCosDscpToProfileMapSet
(
    unsigned char    dev,
    unsigned char    dscp,
    unsigned char    profileIndex
);
extern unsigned long cpssDxChCosDscpMutationEntryGet
(
    unsigned char   dev,
    unsigned char   dscp,
    unsigned char   *newDscp
);
extern unsigned long cpssDxChCosUpToProfileMapGet
( 
	unsigned char 	devIdx,
  	unsigned char 	up,
  	unsigned char 	*profileIndex
 );
extern unsigned long cpssDxChCosDscpToProfileMapGet
(
    unsigned char    dev,
    unsigned char    dscp,
    unsigned char    *profileIndex
);

extern unsigned long cpssDxChCosPortQosConfigSet
(
   unsigned char                dev,
   unsigned char                port,
   CPSS_QOS_ENTRY_STC  			*portQosCfg_PTR
);

extern unsigned long cpssDxChCosPortQosTrustModeSet
(
    unsigned char                           dev,
     unsigned char                          port,
    CPSS_QOS_PORT_TRUST_MODE_ENT  			portQosTrustMode
);
extern unsigned long cpssDxChCosPortReMapDSCPSet
(
    unsigned char                     dev,
    unsigned char                      port,
    unsigned long                      enableDscpMutation
);
extern unsigned long  cpssDxChPolicerEntrySet
(
      unsigned char                            devNum,
      unsigned int                      entryIndex,
      CPSS_DXCH_POLICER_ENTRY_STC       *entryPtr,
      CPSS_DXCH_POLICER_TB_PARAMS_STC   *tbParamsPtr
);
extern unsigned long cpssDxChPolicerEntryInvalidate
(
   unsigned char                                  devNum,
   unsigned int                                  entryIndex
);
extern unsigned long cpssDxChPolicerTokenBucketModeSet
(
   unsigned char              devNum,
   CPSS_POLICER_TB_MODE_ENT   mode,
   CPSS_POLICER_MRU_ENT       mru
);
extern unsigned long cpssDxChPolicerPacketSizeModeSet
(
    unsigned char                            devNum,
    CPSS_POLICER_PACKET_SIZE_MODE_ENT        mode
);
extern unsigned long cpssDxChPolicerInit
(
     unsigned char       devNum
);
extern unsigned long cpssDxChPolicerCountersSet
(
   unsigned char                         devNum,
   unsigned int                          counterSetIndex,
   CPSS_DXCH_POLICER_COUNTERS_STC  		*countersPtr
);
extern unsigned long cpssDxChPolicerCountersGet
(
     unsigned char                           devNum,
     unsigned int                            counterSetIndex,
    CPSS_DXCH_POLICER_COUNTERS_STC   		 *countersPtr
);
extern unsigned long cpssDxChCosProfileEntryGet
(
      unsigned char                     dev,
      unsigned char                     profileIndex,
     CPSS_DXCH_COS_PROFILE_STC  	   *cosPtr
);
extern unsigned long cpssDxChCosPortQosTrustModeGet
( 
	  unsigned char                     dev,
      unsigned char                     port,
      CPSS_QOS_PORT_TRUST_MODE_ENT      *portQosTrustMode
);
   /*  entry with QoS parameters bits: 7,8,13 and 25-31 */
 extern unsigned long  cpssDxChCosPortQosConfigGet
(
 	  unsigned char                     dev,
      unsigned char                     port,
      CPSS_QOS_ENTRY_STC				*portQosCfg_PTR
);  
 extern unsigned long  cpssDxChPortDefaultUPGet
 (
      unsigned char                     dev,
      unsigned char                     port,
      unsigned char						defaultUserPrio
 );
 extern unsigned long  cpssDxChCosPortReMapDSCPGet
 (
  	  unsigned char                     dev,
      unsigned char                     port,
      unsigned char						enableDscpMutation
 );
  extern unsigned long cpssDxChPortTxQArbGroupSet
(
    unsigned char                                   devNum,
    unsigned char                                   tcQueue,	 
    CPSS_PORT_TX_Q_ARB_GROUP_ENT            arbGroup,
    CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet
);
   extern unsigned long cpssDxChPortTxQWrrProfileSet
(
    unsigned char                                   devNum,
    unsigned char                                   tcQueue,
    unsigned char                                   wrrWeight,
    CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  		profileSet
);
   extern unsigned long cpssDxChPortTxBindPortToSchedulerProfileSet
(
     unsigned char                                  devNum,
     unsigned char                                  portNum,
     CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT 		 profileSet
);
  extern unsigned long  cpssDxChPortTxInit(unsigned char devNum);
  extern unsigned long cpssDxChPortTxShaperEnableSet
  (
	    unsigned char 	devNum,
	    unsigned char 	portNum,
	    unsigned long	enable
  );
  extern unsigned long cpssDxChPortTxShaperProfileSet
  (
	  	unsigned char	  devNum,
	  	unsigned char	  portNum,
	  	unsigned short	  burstSize,
	    unsigned long	  *maxRatePtr
  );
  extern unsigned long cpssDxChPortTxQShaperEnableSet
  (
	    unsigned char 	devNum,
	    unsigned char 	portNum,
	    unsigned char 	tcQueue,
	    unsigned long	enable
  );
  extern unsigned long cpssDxChPortTxQShaperProfileSet
  (
	  	unsigned char	  devNum,
	  	unsigned char	  portNum,
	  	unsigned char	  tcQueue,
	  	unsigned short	  burstSize,
	    unsigned long	  *userRatePtr
  );
   extern unsigned long cpssDxChPortTxQueueEnableSet
  (
	  unsigned char	 devNum,
	  unsigned long	 enable
  );
    extern unsigned long cpssDxChPortTxWrrGlobalParamSet
   (
	   unsigned char					   devNum,
	   CPSS_PORT_TX_WRR_MODE_ENT       wrrMode,
	   CPSS_PORT_TX_WRR_MTU_ENT         wrrMtu
   );
	
extern unsigned long cpssDxChPolicerEntryGet
(
    unsigned char                           devNum,
    unsigned int                          entryIndex,
   CPSS_DXCH_POLICER_ENTRY_STC     *entryPtr
);
#endif
 unsigned int nam_qos_queue_init();
   
 #if 0
unsigned int nam_qos_read_qos
(
	unsigned char					   dev,
	unsigned char 					   port,
	CPSS_QOS_PORT_TRUST_MODE_ENT	  *portQosTrustMode,
	CPSS_QOS_ENTRY_STC			   	  *portQosCfg_PTR,
	unsigned char					   *defaultUserPrio,
	unsigned char					   *enableDscpMutation
); 
#endif

unsigned int nam_qos_queue_schedu_check
(
	int wrrflag, 
	int groupflag
);

unsigned int nam_qos_profile_entry_set
(
     unsigned int     profileIndex,
     QOS_PROFILE_STC *profile
);
unsigned int nam_qos_up_to_profile_entry
(
	unsigned int up,
	unsigned int profileIndex,
	int unit,
	int port
);

unsigned int bcm_nam_qos_up_to_profile_entry
(
	unsigned int up,
	unsigned int profileIndex,
	int unit,
	int port
);

unsigned int nam_qos_dscp_to_dscp_entry
(
	unsigned int oldDscp,
	unsigned int newDscp
);
unsigned int nam_qos_dscp_to_profile_entry
(
	unsigned int dscp,
	unsigned int profileIndex,
	int unit,
	int port
);

unsigned int bcm_nam_qos_dscp_to_profile_entry
(
	unsigned int dscp,
	unsigned int profileIndex,
	int unit,
	int port
);

unsigned int nam_qos_port_bind_policy_map
(
	unsigned char dev,
	unsigned char port,
	unsigned int policyIndex
);
unsigned int bcm_nam_qos_support_check
(
	void
);

unsigned int nam_qos_port_unbind_policy_map
(
	unsigned char devNum,
	unsigned char  virPort,
	unsigned int  policyIndex
);

extern unsigned long nam_asic_get_instance_num
(
	void
);	
unsigned int nam_qos_schedu_wrr_set
(	
	unsigned int  wrrflag,
	unsigned int  groupFlag,
	unsigned int  tcQueue,
	unsigned int  wrrWeight
);
unsigned int nam_qos_queue_sp_set();
unsigned int nam_qos_queue_sp_set_one
(
	unsigned int  tcQueue
);

unsigned int nam_qos_queue_init();
unsigned int nam_qos_traffic_shape_port
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned char mode,
	unsigned int  burstSize,
	unsigned long maxRatePtr
);
unsigned int nam_qos_del_traffic_shape_port
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned char mode
);
unsigned int nam_qos_traffic_shape_queue_port
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned char mode,
	unsigned int  tcQueue,
	unsigned int  burstSize,
	unsigned long userRatePtr
);
unsigned int nam_qos_del_traffic_shape_queue_port
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned char mode,
	unsigned int  tcQueue
);

#endif
