#ifndef __NAM_INTF_H__
#define __NAM_INTF_H__

#define IN
#define OUT

#define INTF_ENABLE 1
#define INTF_DISABLE 0
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

#define NAM_L3_L2ONLY           (1 << 0)   /* L2 switch only on interface. */
#define NAM_L3_UNTAG            (1 << 1)   /* Packet goes out untagged. */
#define NAM_L3_S_HIT            (1 << 2)   /* Source IP address match. */
#define NAM_L3_D_HIT            (1 << 3)   /* Destination IP address match. */
#define NAM_L3_HIT              (BCM_L3_S_HIT | BCM_L3_D_HIT) 
#define NAM_L3_HIT_CLEAR        (1 << 4)   /* Clear HIT bit. */
#define NAM_L3_ADD_TO_ARL       (1 << 5)   /* Add interface address MAC to ARL. */
#define NAM_L3_WITH_ID          (1 << 6)   /* ID is provided. */
#define NAM_L3_NEGATE           (1 << 7)   /* Negate a set. */
#define NAM_L3_REPLACE          (1 << 8)   /* Replace existing entry. */
#define NAM_L3_TGID             (1 << 9)   /* Port belongs to trunk. */
#define NAM_L3_RPE              (1 << 10)  /* Pick up new priority (COS). */
#define NAM_L3_IPMC             (1 << 11)  /* Set IPMC for real IPMC entry. */
#define NAM_L3_L2TOCPU          (1 << 12)  /* Packet to CPU unrouted, XGS12: Set*/

typedef unsigned char eth_mac_t[6];

typedef struct
{
    unsigned char       arEther[6];
}GT_ETHERADDR;

#ifdef DRV_LIB_CPSS
extern unsigned int cpssDxChIpRoutingEnable
(
    IN unsigned char    devNum,
    IN unsigned int  enableRouting
);


extern unsigned long cpssDxChIpPortRoutingEnable
(
    IN unsigned char                            devNum,
    IN unsigned char                           portNum,
    IN unsigned int    	ucMcEnable, 								/*CPSS_IP_UNICAST_MULTICAST_ENT*/
    IN unsigned int      	protocolStack,								/*CPSS_IP_PROTOCOL_STACK_ENT*/
    IN unsigned long                          enableRouting
);

extern unsigned int cpssDxChBrgVlanIpMcRouteEnable
(
    IN unsigned char                            devNum,
    IN unsigned short                           vlanId,
    IN unsigned int     			protocol, 							 /*CPSS_IP_PROTOCOL_STACK_ENT*/
    IN unsigned int                          enable
);

extern unsigned int cpssDxChBrgVlanIpUcRouteEnable
(
    IN unsigned char	devNum,
    IN unsigned short	vlanId,
    IN unsigned int		protocol,                 /*CPSS_IP_PROTOCOL_STACK_ENT*/
    IN unsigned int		enable
);

extern unsigned int cpssDxChBrgVlanUdpBcPktsToCpuEnable
(
    IN unsigned char	devNum,
    IN unsigned short	vlanId,
    IN unsigned int		enable
);

extern unsigned int cpssDxChBrgGenUdpBcDestPortCfgSet
(
   IN unsigned char	devNum,
   IN unsigned int	entryIndex,
   IN unsigned short	udpPortNum,
   IN CPSS_NET_RX_CPU_CODE_ENT  cpuCode,
   IN CPSS_PACKET_CMD_ENT       cmd
);

extern unsigned long cpssDxChBrgGenRipV1MirrorToCpuEnable
(
    unsigned char    dev,
    unsigned int  enable
);

#endif

#endif
