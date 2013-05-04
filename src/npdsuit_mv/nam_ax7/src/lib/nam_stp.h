#ifndef __NAM_STP_H__
#define __NAM_STP_H__

#define FDB_CONFIG_FAIL 1
#define FDB_CONFIG_SUCCESS 0

	
typedef enum nam_error_e {
	NAM_E_NONE = 0, 
	NAM_E_INTERNAL = -1, 
	NAM_E_MEMORY = -2,
	NAM_E_UNIT = -3, 
	NAM_E_PARAM = -4, 
	NAM_E_EMPTY = -5, 
	NAM_E_FULL = -6, 
	NAM_E_NOT_FOUND = -7, 
	NAM_E_EXISTS = -8, 
	NAM_E_TIMEOUT = -9, 
	NAM_E_BUSY = -10, 
	NAM_E_FAIL = -11, 
	NAM_E_DISABLED = -12, 
	NAM_E_BADID = -13, 
	NAM_E_RESOURCE = -14, 
	NAM_E_CONFIG = -15, 
	NAM_E_UNAVAIL = -16, 
	NAM_E_INIT = -17, 
	NAM_E_PORT = -18 
} nam_error_t;

typedef enum
{
    CPSS_STP_DISABLED_E = 0,
    CPSS_STP_BLCK_LSTN_E,
    CPSS_STP_LRN_E,
    CPSS_STP_FRWRD_E
} CPSS_STP_STATE_ENT;


enum{
    NAM_STG_STP_DISABLE = 0, /* Disabled. */
    NAM_STG_STP_BLOCK , /* BPDUs/no learns. */
    NAM_STG_STP_LISTEN , /* BPDUs/no learns. */
    NAM_STG_STP_LEARN , /* BPDUs/learns. */
    NAM_STG_STP_FORWARD, /* Normal operation. */
    NAM_STG_STP_COUNT  
};

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

int nam_asic_stp_port_enable
(
   unsigned char devNum, 
   unsigned int portId,
   unsigned char stp_enable
);
int  nam_vlan_stpid_get
(
     unsigned char devNum,
     unsigned short vid,
     unsigned int* stg
);
int nam_stp_state_get
(
    unsigned char devNum,
    unsigned char port,
    unsigned short stg,
    unsigned int* stpState
);
int nam_stp_state_set
(
    unsigned char devNum,
    unsigned char port,
    unsigned short stg,
    unsigned int stpState

);

int nam_stp_vlan_bind_stg
(
   unsigned char devNum,
   unsigned short vlanid,
   unsigned int stg
);

int nam_stg_check
(
   int unit,
   int stg
);

#ifdef DRV_LIB_CPSS
extern unsigned int   cpssDxChBrgStpStateSet
(
    unsigned char               dev,
    unsigned char                port,
    unsigned short               stpId,
    CPSS_STP_STATE_ENT       state
);
extern unsigned int  cpssDxChBrgRoutedSpanEgressFilteringEnable
(
    unsigned char    dev,
    unsigned int   enable
);

extern unsigned int cpssDxChBrgVlanStpIdGet
(
    unsigned char    devNum,
    unsigned short   vlanId,
    unsigned short   *stpIdPtr
);
extern unsigned int  cpssDxChBrgStpEntryGet
(
    unsigned char       dev,
    unsigned short   stpId,
    unsigned int  stpEntryPtr[2]
);

extern unsigned int cpssDxChBrgVlanToStpIdBind
(
    unsigned char    devNum,
    unsigned short   vlanId,
    unsigned int     stpId
);
#endif

#endif
