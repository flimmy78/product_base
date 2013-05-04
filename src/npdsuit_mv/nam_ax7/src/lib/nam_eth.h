#ifndef __NAM_ETH_H__
#define __NAM_ETH_H__

#define IN
#define OUT
#define INOUT
#define EGRESS_CNT_PORT_E  1
#define EGRESS_CNT_VLAN_E  2

#define PROMISCUOUS_TARGET_DEV_NUM 1    /* promiscuous target dev for 7K*/

#define PROMISCUOUS_AU5612_TARGET_DEV 0  /* promiscuous target dev for 5612 or 5612i*/
/* add for 7605i */
#define NAM_AX7605i_MAX_SLOTNO			3
#define NAM_AX7605i_MAX_PORT_PERSLOT		28
/* add end */
#define NAM_AX7_MAX_SLOTNO			4
#define NAM_AX7_MAX_PORT_PERSLOT		6
#define NAM_AX7i_alpha_MAX_SLOTNO	1
#define NAM_AX7i_alpha_MAX_PORT_PERSLOT	6
#define NAM_AX7i_alpha_MAX_INTCONN_XPORT 2
#define NAM_AX7_MAX_XG_PORT_PERSLOT	1
#define NAM_AX5_MAX_SLOTNO			1
#define NAM_AX5_MAX_PORT_PERSLOT		12
#define NAM_AX5I_MAX_SLOTNO			1
#define NAM_AX5I_MAX_PORT_PERSLOT		8
#define NAM_AX5I_MAX_XG_PORT_PERSLOT	1
#define NAM_AX5E_MAX_SLOTNO			1
#define NAM_AX5E_MAX_PORT_PERSLOT		8
#define NAM_AU4_MAX_SLOTNO			1
#define NAM_AU4_MAX_PORT_PERSLOT		24
#define NAM_AU3_MAX_SLOTNO			1
#define NAM_AU3_MAX_PORT_PERSLOT		24
#define NAM_AU3_BCM_MAX_SLOTNO			1
#define NAM_AU3_BCM_MAX_PORT_PERSLOT		52
#define NAM_AU3528_MAX_SLOTNO		(NAM_AU3_MAX_SLOTNO)
#define NAM_AU3528_MAX_PORT_PERSLOT		28
#define NAM_AU2528_MAX_SLOTNO		(NAM_AU3_MAX_SLOTNO)
#define NAM_AU2528_MAX_PORT_PERSLOT	(NAM_AU3528_MAX_PORT_PERSLOT)
#define NAM_AU4626_SUBCARD_MAX_SLOTNO		1
#define NAM_AU4626_SUBCARD_MAX_PORT_PERSLOT	2
#define PORT_STATE_GET_SUCCESS	0x0
#define PORT_STATE_GET_FAIL		0xFF
#define NAM_CONFIG_HOST_COUNTER_ERR 0xEE
#define HOST_STATE_GET_FAIL 0x12
#define HOST_DMAC_SMAC_GET_ERROR 0x13
#define NAM_PORT_STARTNUM_ONSLOT(slotnum)		1
#define NAM_SLOTNO_ISLEGAL(slotnum,low,high)	((slotnum >= low)&&(slotnum <= high))
#define NAM_PORTNO_ISLEGAL(portnum,start,end)	((portnum >= start)&&(portnum <= end))
#define CHASSIS_SLOT_NO2INDEX(slot_no) ((slot_no) - (CHASSIS_SLOT_START_NO))
#define ETH_PORT_TYPE_FROM_GLOBAL_INDEX(eth_g_index) g_eth_ports[(eth_g_index)]->port_type
/*#define ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slotno,portno) ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(CHASSIS_SLOT_NO2INDEX(slotno),ETH_LOCAL_NO2INDEX(CHASSIS_SLOT_NO2INDEX(slotno),(portno)))*/
/*struct eth_port_s **g_eth_ports = NULL;*/

/* NAM error macro */
#define NAM_ERR_NONE					0
#define	NAM_ERR_SLOT_OUT_OF_RANGE	(NAM_ERR_NONE + 1)
#define NAM_ERR_PORT_OUT_OF_RANGE	(NAM_ERR_NONE + 2)
#define NAM_ERR_MODULE_NOT_SUPPORT	(NAM_ERR_NONE + 3)
#define NAM_ERR_DEVICE_NUMBER			(NAM_ERR_NONE + 4)
#define NAM_ERR_PORT_ON_DEVICE		(NAM_ERR_NONE + 5)
#define NAM_ERR_NO_MATCH				(NAM_ERR_NONE + 6)
#define NAM_ERR_HW					(NAM_ERR_NONE + 7)
#define NAM_ERR_MAX					(NAM_ERR_NONE + 255)

#define NAM_ERR_UNSUPPORT           6
#define NAM_ERR_GENERNAL            1

/*used for port storm control in bcm*/

#define TOKEN_BUCKET_SIZE       64      /* Size of one token bucket in kbits */
#define METERING_TOKEN_MAX      0x7FFFF  /* Max number of supported tokens */
#define METER_BUCKETSIZE_MAX    12
#define BITSINBYTE              8
#define KBYTES                  (1024)
#define MBYTES                  ((1024) * KBYTES)
#define KBITS_IN_API            1000

#define METER_BUCKETSIZE_MAX    12
#define	PORT_STORM_CONTROL_STREAM_DLF            0x01
#define	PORT_STORM_CONTROL_STREAM_MCAST          0x02
#define	PORT_STORM_CONTROL_STREAM_BCAST          0x04
#define	PORT_STORM_CONTROL_STREAM_UCAST          0x08  
#define	PORT_STORM_CONTROL_STREAM_ALL          (PORT_STORM_CONTROL_STREAM_DLF | \
	                                            PORT_STORM_CONTROL_STREAM_MCAST | \
	                                            PORT_STORM_CONTROL_STREAM_MCAST| \
	                                            PORT_STORM_CONTROL_STREAM_UCAST) 
#define	ETH_PORT_SPEED_10000_PPS 14881000
#define ETH_PORT_SPEED_1000_PPS 1488100
#define	ETH_PORT_SPEED_100_PPS  148810
#define ETH_PORT_SPEED_10_PPS  14881

#define	ETH_PORT_SPEED_10000_BPS 10000000000
#define ETH_PORT_SPEED_1000_BPS 1000000000
#define	ETH_PORT_SPEED_100_BPS  100000000
#define ETH_PORT_SPEED_10_BPS  10000000


extern char* nam_err_msg[];

struct xg_port_info_s {
	unsigned char devNum;
	unsigned char portNum;
};

struct vlan_ports{
	unsigned int count;
	unsigned int ports[128];
};

typedef struct xg_port_info_s gen_asic_port_info_s;
typedef enum
{
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E = 0,
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E
} CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT;
typedef enum
{
    CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E = 0,
    CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E,
    CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E
} CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT;

typedef struct
{
    unsigned long                                qosProfileId;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT  assignPrecedence;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT        enableModifyUp;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT        enableModifyDscp;
}CPSS_QOS_ENTRY_STC;

typedef enum {
	PORT_FULL_DUPLEX_E,
	PORT_HALF_DUPLEX_E
} PORT_DUPLEX_ENT;

typedef enum {
	PORT_SPEED_10_E,
	PORT_SPEED_100_E,
	PORT_SPEED_1000_E,
	PORT_SPEED_10000_E,
	PORT_SPEED_12000_E,
	PORT_SPEED_2500_E,
	PORT_SPEED_5000_E
}PORT_SPEED_ENT;

typedef struct {
	unsigned char	portLink;
	PORT_SPEED_ENT	portSpeed;
	PORT_DUPLEX_ENT portDuplexity;
}PORT_ATTR_STC;

typedef enum
{
    CPSS_PORT_FULL_DUPLEX_E,
    CPSS_PORT_HALF_DUPLEX_E

}CPSS_PORT_DUPLEX_ENT;

/* Port type mode */
typedef enum {
	PORT_TYPE_SGMII = 0,
	PORT_TYPE_1000BASEX,
	PORT_TYPE_MAX	
}CPSS_DXCH_PORT_TYPE_E;

/* Port In-Band Auto-Negotiation mode */
typedef enum {	
	PORT_INBAND_AN_SGMII_MODE = 0,
	PORT_INBAND_AN_1000BASEX_MODE
}CPSS_DXCH_PORT_INBAND_AN_MODE;

/*GE or FE port*/
typedef enum {
   PORT_FE_TYPE_E = 0,
   PORT_GE_TYPE_E
}NAM_ETH_PORT_STREAM_TYPE_E;
/*
Head files for nam module internal usage.
*/
typedef struct
	{
		unsigned int  outUcFrames;
		unsigned int  outMcFrames;
		unsigned int  outBcFrames;
		unsigned int  brgEgrFilterDisc;
		unsigned int  txqFilterDisc;
		unsigned int  outCtrlFrames;
	} NAM_PORT_EGRESS_CNTR_STC;

typedef struct{

 unsigned char arEther[6];
}GT_ETHERADDR;

/*Hardware rate limit configuration*/

typedef enum
{
    NAM_DROP_MODE_SOFT_E = 0,
    NAM_DROP_MODE_HARD_E
}NAM_DROP_MODE_TYPE_ENT;

typedef enum
{
    NAM_RATE_LIMIT_PCKT_BASED_E = 1,
    NAM_RATE_LIMIT_BYTE_BASED_E
} NAM_RATE_LIMIT_MODE_ENT; 

typedef struct
{
    NAM_DROP_MODE_TYPE_ENT         dropMode;
    NAM_RATE_LIMIT_MODE_ENT        rMode;
    unsigned int                   win10Mbps;
    unsigned int                   win100Mbps;
    unsigned int                   win1000Mbps;
    unsigned int                   win10Gbps;
} NAM_BRG_RATE_LIMIT_T;

/*
 * Typedef: struct NAM_BRG_GEN_RATE_LIMIT_PORT_STC
 *
 * Description:
 *      Describe a port rate limit parameters.
 *
 * Fields:
 *     enableBc         - enable Bc packets rate limiting
 *     enableMc         - enable Mc packets rate limiting
 *     enableUcUnk      - enable Uc Unknown packets rate limiting
 *     enableUcKnown    - enable Uc Known   packets rate limiting
 *     rateLimit        - rate limit specified in 64 bytes/packets
 *                        depends on ratelimit mode
 *                        range 0 - 65535
 *
 * Comments:
 *     None
 */
typedef struct
{
    unsigned int   enableBc;
    unsigned int   enableMc;
    unsigned int   enableUcUnk;
    unsigned int   enableUcKnown;
    unsigned int   rateLimit;
} NAM_BRG_RATE_LIMIT_PORT_T;

typedef enum{
   ETH_PORT_STREAM_PPS_E,
   ETH_PORT_STREAM_BPS_E,
   ETH_PORT_STREAM_INVALID_E
}ETH_PORT_STREAM_MODE_E;
typedef struct
{
    unsigned int gtHostInPkts;
    unsigned int gtHostOutPkts;
    unsigned int gtHostOutBroadcastPkts;
    unsigned int gtHostOutMulticastPkts;
} BRIDGE_HOST_CNTR_STC;
unsigned int nam_get_devport_by_slotport
(
	IN 	unsigned int slotNo,
	IN 	unsigned int localPortNo,
	IN 	unsigned int module_type,
	OUT unsigned int* devNum,
	OUT unsigned int* virtPortNo
);

unsigned int nam_get_slotport_by_devport
(
	IN		unsigned int devNum,
	IN 		unsigned int virtPortNo,
	IN		unsigned int productId,
	OUT 	unsigned int* slotNo,
	OUT 	unsigned int* localPortNo
);
	
unsigned int nam_get_global_index_by_devport
(
	unsigned int devNum,
	unsigned int portNum,
	unsigned int* eth_g_index
);
	
unsigned int nam_get_devport_by_global_index
(
	unsigned int eth_g_index,
	unsigned int *devNum,
	unsigned int *portNum
);

/**********************************************************************************
 * nam_ethport_mask_link_interrupt_set
 * 
 * DESCRIPTION:
 *	This method set CPU port shaper value.	 
 *
 *
 *	INPUT:
 *		devNum - DXCH device number
 *		portNum - port number on the DXCH device
 *		enable - enable/disable flag(0 for disable interrupt and 1 for enable interrupt)
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		status - return value from cpss driver layer.
 *		
 **********************************************************************************/
unsigned long nam_ethport_mask_link_interrupt_set
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long enable
);
#ifdef DRV_LIB_CPSS
/* port type */
extern unsigned long cpssDxChPortTypeSet
(
    unsigned char	devNum,
    unsigned char	portNum,
    CPSS_DXCH_PORT_TYPE_E  portType
);
/* port enable*/
extern unsigned long cpssDxChPortEnableSet
(
    unsigned char	devNum,
    unsigned char	portNum,
    unsigned long	enable
);
extern unsigned long cpssDxChPortEnableGet
(
    unsigned char	devNum,
    unsigned char	portNum,
    unsigned long	*statePtr
);
/* port link status*/
extern unsigned long cpssDxChPortForceLinkPassEnableSet
(
    unsigned char     devNum,
    unsigned char     portNum,
    unsigned long	  state
);
extern unsigned long cpssDxChPortForceLinkDownEnableSet
(
    unsigned char     devNum,
    unsigned char     portNum,
    unsigned long	  state
);
extern unsigned long cpssDxChPortLinkStatusGet
(
    unsigned char	devNum,
    unsigned char	portNum,
    unsigned long	*isLinkUpPtr
);
/* port duplex staus*/
extern unsigned long cpssDxChPortDuplexModeSet
(
    unsigned char	devNum,
    unsigned char	portNum,
    CPSS_PORT_DUPLEX_ENT  dMode
);
extern unsigned long cpssDxChPortDuplexModeGet
(
    unsigned char	devNum,
    unsigned char	portNum,
    PORT_DUPLEX_ENT	*dModePtr
);
/* port speed */
extern unsigned long cpssDxChPortSpeedSet
(
    unsigned char	devNum,
    unsigned char	portNum,
    PORT_SPEED_ENT  speed
);
extern unsigned long cpssDxChPortSpeedGet
(
    unsigned char	devNum,
    unsigned char	portNum,
    PORT_SPEED_ENT	*speedPtr
);
extern unsigned long cpssDxChPortConfigSpeedGet
(
    unsigned char	devNum,
    unsigned char	portNum,
    PORT_SPEED_ENT	*speedPtr
);
extern unsigned long cpssDxChPortConfigFlowControlGet
(
    unsigned char	devNum,
    unsigned char	portNum,
    unsigned long	*statePtr
);

/* port auto negotiation for 3052*/
extern unsigned int nam_set_bcm_ethport_autoneg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long state
);
/*port AN status*/
extern unsigned long cpssDxChPortDuplexAutoNegEnableSet
(
    unsigned char     devNum,
    unsigned char     portNum,
    unsigned long	  state
);
extern unsigned long cpssDxChPortDuplexAutoNegEnableGet
(
    unsigned char     devNum,
    unsigned char     portNum,
    unsigned long	  *state
);
extern unsigned long cpssDxChPortSpeedAutoNegEnableSet
(
    unsigned char     devNum,
    unsigned char     portNum,
    unsigned long	  state
);
extern unsigned long cpssDxChPortSpeedAutoNegEnableGet
(
    unsigned char     devNum,
    unsigned char     portNum,
    unsigned long	  *state
);
extern unsigned long cpssDxChPortFlowCntrlAutoNegEnableSet
(
    unsigned char     devNum,
    unsigned char     portNum,
    unsigned long	  state,
    unsigned long	  pauseAdvertise 
);

#ifdef DRV_LIB_CPSS_3_4
extern unsigned long cpssDxChPortFlowCntrlAutoNegEnableGet
(
    unsigned char     devNum,
    unsigned char     portNum,
    unsigned long	  *state,
    unsigned long	  *pauseAdvertise
);
#else
extern unsigned long cpssDxChPortFlowCntrlAutoNegEnableGet
(
    unsigned char     devNum,
    unsigned char     portNum,
    unsigned long	  *state
);

#endif
extern unsigned long cpssDxChPortInbandAutoNegEnableSet
(
    unsigned char     devNum,
    unsigned char     portNum,
    unsigned long	  enable
);
extern unsigned long cpssDxChPortInbandAutoNegModeSet
(
    unsigned char     devNum,
    unsigned char     portNum,
    CPSS_DXCH_PORT_INBAND_AN_MODE mode
);
extern unsigned long cpssDxChPortAutoNegStatusGet
(
    unsigned char     devNum,
    unsigned char     portNum,
    unsigned long	  *isAutoNegPtr
);
/*port FC en/dis*/
extern unsigned long cpssDxChPortFlowControlEnableSet
(
    unsigned char     devNum,
    unsigned char     portNum,
    unsigned long	  state
);
extern unsigned long cpssDxChPortPeriodicFcEnableSet
(
    unsigned char     devNum,
    unsigned char     portNum,
    unsigned long	  enable
);
extern unsigned long cpssDxChPortFlowControlEnableGet
(
    unsigned char	devNum,
    unsigned char	portNum,
    unsigned long	*statePtr
);

/*port attr */
extern unsigned long cpssDxChPortAttributesOnPortGet
(
    unsigned char	devNum,
    unsigned char	portNum,
    PORT_ATTR_STC	*portAttrArrayPtr
);

/*port back Pressure*/
extern unsigned long cpssDxChPortBackPressureEnableSet
(
    unsigned char     devNum,
    unsigned char     portNum,
    unsigned long	  state
);
extern unsigned long cpssDxChPortBackPressureEnableGet
(
    unsigned char	devNum,
    unsigned char	portNum,
    unsigned long	*state
);

/*port MRU */
extern unsigned long cpssDxChPortMruSet
(
    unsigned char	devNum,
    unsigned char	portNum,
    unsigned int	mruSize
); 
extern unsigned long cpssDxChPortMruGet
(
    unsigned char	devNum,
    unsigned char	portNum,
    unsigned int	*mruSize
);
extern unsigned long cpssDxChPortLedIntfSet
(
    unsigned char     devNum,
    unsigned char     portNum,
    unsigned long	  state
);
extern unsigned long cpssDxChPortForceLinkDownEnableSet
(
    unsigned char     devNum,
    unsigned char     portNum,
    unsigned long	  state
); 

extern unsigned long cpssDxChBrgGEPortPktClear
(
    unsigned char	devNum,
    unsigned char	portNum
);

/*******************************************************************************
* cpssDxChPortLinkStatusIntMaskSet
*
* DESCRIPTION:
*       This routine unmasks/masks the interrupt event CPSS_PP_PORT_LINK_STATUS_CHANGED_E to
* enable/disable link status change interrupt.
*
* INPUTS:
*       	devNum - device number.
*  		portNum - port number on the DXCH device.
*	   	enable - enable/disable flag to unmask/mask HW interrupt.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL if failed.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
extern unsigned long cpssDxChPortLinkStatusIntMaskSet
(
    unsigned char devNum,
    unsigned char portNum,
    unsigned long enable
);
#endif
/*******************************************/
/******************for XG-port**************/
typedef enum
{
    CPSS_PORT_SPEED_10_E,
    CPSS_PORT_SPEED_100_E,
    CPSS_PORT_SPEED_1000_E,
    CPSS_PORT_SPEED_10000_E,
    CPSS_PORT_SPEED_12000_E,
    CPSS_PORT_SPEED_2500_E,
    CPSS_PORT_SPEED_5000_E
}CPSS_PORT_SPEED_ENT;

typedef enum
{
    CPSS_DXCH_PORT_BUFFERS_MODE_SHARED_E,
    CPSS_DXCH_PORT_BUFFERS_MODE_DIVIDED_E
}CPSS_DXCH_PORT_BUFFERS_MODE_ENT;


typedef enum
{

	CPSS_DXCH_XG_PORT_IPG_BASE_BYTE_MIN_E = 0,
	CPSS_DXCH_XG_PORT_IPG_BASE_BYTE_MAX_E
} CPSS_DXCH_NETWORK_XGPORT_IPG_BASE_ENT;

typedef enum 
{
	CPSS_DXCH_XG_PORT_IPG_LAN_MODE_E = 0,
	CPSS_DXCH_XG_PORT_IPG_WAN_MODE_E ,
	CPSS_DXCH_XG_PORT_IPG_FIXED_IPG_LAN_MODE_E ,
} CPSS_DXCH_NETWORK_XGPORT_IPG_MODE_ENT;

#ifdef DRV_LIB_CPSS
extern unsigned long cpssDxChPortIPGSet
(
    unsigned char     devNum,
    unsigned char     portNum,
    unsigned char 	  value
);

extern unsigned long  cpssDxChXGPortIPGModeSet
(
    unsigned char     devNum,
    unsigned char     portNum,
    CPSS_DXCH_NETWORK_XGPORT_IPG_MODE_ENT ipgMode
);

extern unsigned long  cpssDxChXGPortIPGBaseSet
(
    unsigned char     devNum,
    unsigned char     portNum,
    CPSS_DXCH_NETWORK_XGPORT_IPG_BASE_ENT ipgBase
);

extern unsigned long cpssDxChPortBuffersModeSet
(
    unsigned char     devNum,
    CPSS_DXCH_PORT_BUFFERS_MODE_ENT  bufferMode
);

extern unsigned long cpssDxChXsmiAcTimingSet
(
	unsigned char	devNum
);

extern unsigned long cpssDxChXgPortExternalPhyLedIntfSet
(
	unsigned char	devNum,
	unsigned char	portNum
);

/*******************************************************************************
* cpssDxChPhyPortSmiPreferredMediaSet
*
* DESCRIPTION:
*      Set media auto-selection preferred media(or say media priority)
*	 irrelevant when port PHY type is not combo(or say Media Auto-Selection Interface)
*
* APPLICABLE DEVICES: All DxCh Devices
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       autoMediaType   - preferred auto-selection interface type (copper or fiber)
*			0 - none
*			1 - fiber media interface
*			2 - copper media interface
*
* OUTPUTS:
*       NULL
*
* RETURNS:
*       GT_OK (0)  					- on success
*       GT_BAD_PARAM (4)    	   	- on wrong devNum or portNum.
*       GT_NOT_SUPPORTED (0x10)  	- for non-combo ports
*       GT_HW_ERROR (0x10017)     	- on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
extern unsigned int cpssDxChPhyPortSmiPreferredMediaSet
(
	IN unsigned char	devNum,
	IN unsigned char	portNum,
	IN unsigned char    autoMediaType
);

/*******************************************************************************
* cpssDxChPhyPortSmiPreferredMediaGet
*
* DESCRIPTION:
*      Get media auto-selection preferred media(or say media priority)
*	 irrelevant when port PHY type is not combo(or say Media Auto-Selection Interface)
*
* APPLICABLE DEVICES: All DxCh Devices
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       autoMediaType   - preferred auto-selection interface type (copper or fiber)
*			0 - none
*			1 - fiber media interface
*			2 - copper media interface
*
* RETURNS:
*       GT_OK (0)  					- on success
*       GT_BAD_PARAM (4)    	   	- on wrong devNum or portNum.
*	  GT_BAD_PTR (5)				- null pointer error
*       GT_NOT_SUPPORTED (0x10)  	- for non-combo ports
*       GT_HW_ERROR (0x10017)     	- on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
extern unsigned int cpssDxChPhyPortSmiPreferredMediaGet
(
	IN unsigned char	devNum,
	IN unsigned char	portNum,
	OUT unsigned char	*autoMediaType
);
/*******************************************************************************
* cpssDxChPhyPortAutonegSet
*
* DESCRIPTION:
*   This API implements PHY port Auto-negotiation enable or disable.
*
* INPUTS:
*	  devNum 	- CH2 Device Number
*	  portNum 	- Device Port Number
*	  state 	- Autoneg State(enable<1> or disable<0>)
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
******************************************************************************/
extern long cpssDxChPhyPortAutonegSet
(
	IN char  devNum,
	IN char  portNum,
	IN long  state
);

/*******************************************************************************
* cpssDxChPhyPortLinkForceSet
*
* DESCRIPTION:
*   This API implements PHY port link status force up/down
*
* INPUTS:
*	  devNum 	- CH2 Device Number
*	  portNum 	- Device Port Number
*	  state 	- Link Force State(UP<1> or DOWN<0> or AUTO<2>)
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
******************************************************************************/
extern long cpssDxChPhyPortLinkForceSet
(
	IN char  devNum,
	IN char  portNum,
	IN long  state
);
/*******************************************************************************
* cpssDxChPhyPortDuplexSet
*
* DESCRIPTION:
*   This API implements PHY port duplex set
*
* INPUTS:
*	  devNum 	- CH2 Device Number
*	  portNum 	- Device Port Number
*	  duplex 	- full mode<0> or half mode<1>
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
******************************************************************************/
extern long cpssDxChPhyPortDuplexSet
(
	IN char  devNum,
	IN char  portNum,
	IN long  duplex
);
/*******************************************************************************
* cpssDxChPhyPortSpeedSet
*
* DESCRIPTION:
*   This API implements PHY port speed set
*
* INPUTS:
*	  devNum 	- CH2 Device Number
*	  portNum 	- Device Port Number
*	  speed 	- Speed 10<0> or Speed 100<1> or Speed 1000<2>
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
******************************************************************************/
extern long cpssDxChPhyPortSpeedSet
(
	IN char  devNum,
	IN char  portNum,
	IN long  speed
);

/*******************************************************************************
* cpssDxChBrgGEPortPktStatistic
*
* DESCRIPTION:
*       Gets all the MIB counter value for GE ports of the specified device
*
* APPLICABLE DEVICES: ALL DxCh Devices.
*
* INPUTS:
*
* OUTPUTS:
*
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hw error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       Learned Entry Discards Counter is clear-on-read.
*
*******************************************************************************/
extern long cpssDxChBrgGEPortPktStatistic
(
	IN char devNum,
	IN char portNum,
	OUT struct npd_port_counter  *portPktCount,
	OUT struct port_oct_s	*portOtcCount
);

/*******************************************************************************
* cpssDxChPhyPortSmiInit
*
* DESCRIPTION:
*       Initialiaze the SMI control register port, Check all GE ports and
*       activate the errata initialization fix.
*
* APPLICABLE DEVICES: All DxCh Devices
*
* INPUTS:
*       devNum      - physical device number
*	  portNum     - Device port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong devNum
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
extern unsigned long cpssDxChPhyPortSmiReInit
(
    IN unsigned char devNum,
    IN unsigned char portNum
);

enum port_phy_media_type{
		PHY_MEDIA_FIBBER = 0 ,
		PHY_MEDIA_COPPER
};
extern unsigned int cpssDxChPhyPortMediaTypeGet
(
	IN unsigned char	devNum,
	IN unsigned char	portNum,
	OUT unsigned char	*phyMediaType
);

#ifndef __NAM_VLAN_H__
extern unsigned long cpssDxChBrgVlanLearningStateSet 
(
    unsigned char    devNum,
    unsigned short   vlanId,
    unsigned long  	status
);	
extern unsigned long cpssDxChBrgVlanNASecurEnable 
(
    unsigned char    devNum,
    unsigned short   vlanId,
    unsigned long 	 enable
);

extern unsigned int cpssDxChBrgVlanEgressFilteringEnable 
(
    unsigned char    dev,
    unsigned int  enable
);

extern unsigned int cpssDxChBrgPortEgrFltUregMcastEnable 
(
    unsigned char   dev,
    unsigned char    port,
    unsigned int  enable
);

extern unsigned long cpssDxChBrgGenVlanValidCheckEnable 
(
    unsigned char 	dev,
    unsigned int  	enable
);


extern unsigned long cpssDxChBrgVlanPortIngFltEnable 
(
    unsigned char  dev,
    unsigned char  port,
    unsigned int   enable
);

#endif

#endif
int nam_set_ethport_type
(
	unsigned char devNum,
	unsigned char portNum,
	CPSS_DXCH_PORT_TYPE_E portType
);

int nam_set_ethport_ipg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned char  value
);

int nam_set_port_buff_mode
(
	unsigned char devNum,
    CPSS_DXCH_PORT_BUFFERS_MODE_ENT  bufferMode
);
int nam_set_xg_port_ipg
(
	unsigned char devNum,
	unsigned char portNum,
	CPSS_DXCH_NETWORK_XGPORT_IPG_MODE_ENT ipgMode,
	CPSS_DXCH_NETWORK_XGPORT_IPG_BASE_ENT ipgBase
);
int nam_set_xsmi_ac_timing
(
	unsigned char devNum
);

int nam_set_xgport_led_on_external_phy
(
	unsigned char devNum,
	unsigned char portNum
);

int nam_set_ethport_inband_autoneg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long state
);
int nam_set_ethport_inband_an_mode
(
	unsigned char devNum,
	unsigned char portNum,
	CPSS_DXCH_PORT_INBAND_AN_MODE anMode
);
int nam_set_ethport_enable
(
	unsigned char	devNum,
	unsigned char	portNum,
	unsigned long	portAttr
);
int nam_set_ethport_force_linkup
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long state
);
int nam_set_ethport_force_linkdown
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long state
);
int nam_set_ethport_force_auto
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long state
);
int nam_set_ethport_duplex_autoneg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long state
);
int nam_get_ethport_duplex_autoneg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long *state
);
int nam_set_ethport_fc_autoneg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long state,
	unsigned long pauseAdvertise
);
int nam_get_ethport_fc_autoneg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long *state
);
int nam_set_ethport_speed_autoneg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long state
);
int nam_get_ethport_speed_autoneg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long *state
);
int nam_set_ethport_duplex_mode
(
	unsigned char devNum, 
	unsigned char portNum, 
	PORT_DUPLEX_ENT	dMode
);
int nam_set_ethport_speed
(
	unsigned char devNum, 
	unsigned char portNum, 
	PORT_SPEED_ENT speed
);
int nam_set_ethport_flowCtrl
(
	unsigned char devNum, 
	unsigned char portNum, 
	unsigned long state
);
int nam_set_ethport_PeriodFCtrl
(
	unsigned char devNum, 
	unsigned char portNum, 
	unsigned long enable
);
int nam_set_ethport_backPres
(
	unsigned char devNum, 
	unsigned char portNum, 
	unsigned long state
);
int nam_set_ethport_mru
(
	unsigned char devNum, 
	unsigned char portNum, 
	unsigned int mruSize
);
unsigned int nam_get_port_en_dis 
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long* portAttr
);
unsigned int nam_get_port_link_state
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long* portAttr
);
unsigned int nam_get_port_autoneg_state
(
	unsigned char devnum,
	unsigned char portnum,
	unsigned long* portAttr
);
int nam_set_phy_ethport_autoneg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long state
);

int nam_set_phy_reboot
(
	unsigned char devNum,
	unsigned char portNum
);

unsigned int nam_get_port_duplex_mode
(
	unsigned char devNum, 
	unsigned char portNum,
	unsigned int* portAttr
);
unsigned int nam_get_port_speed
(
	unsigned char devNum, 
	unsigned char portNum,
	unsigned int* portAttr
);
unsigned int nam_get_port_config_speed
(
	unsigned char devNum, 
	unsigned char portNum,
	unsigned int* portAttr
);
unsigned int nam_get_port_config_flowcontrol
(
	unsigned char devNum, 
	unsigned char portNum,
	unsigned int* portAttr
);

unsigned int nam_get_port_flowCtrl_state
(
	unsigned char devNum,
	unsigned char portNum, 
	unsigned long* portAttr
);
unsigned int nam_get_port_backPres_state
(
	unsigned char devNum, 
	unsigned char portNum, 
	unsigned long* portAttr
);
unsigned int nam_get_port_mru
(
	unsigned char devNum, 
	unsigned char portNum, 
	unsigned int* portMru
);

unsigned int nam_asic_port_pkt_statistic
(
	unsigned char devNum, 
	unsigned char portNum, 
	struct npd_port_counter *portPktCount,
	struct port_oct_s		*portOctCount
);
unsigned int nam_asic_xg_port_pkt_statistic
(   
    unsigned char devNum,
    unsigned char portNum,
	struct npd_port_counter *portPktCount,
	struct port_oct_s		*portOctCount

);

unsigned int nam_set_bcm_ethport_phy
(
	unsigned char devNum,
	unsigned char portNum
);


/* 
 * call cpss function cpssDxChBrgVlanLearningStateSet and cpssDxChBrgVlanNASecurEnable
 * if both return 0,then return 0; else return 1
 *
 */

unsigned int nam_learning_state_set_na_secur_enable(
	unsigned char dev,
	unsigned short vid,
	unsigned int flag
);


/**********************************************************************************
 *  nam_set_promi_attr
 *
 *	DESCRIPTION:
 * 		set promiscuous port attribute.
 *
 *	INPUT:
 *		eth_g_index - index of port's mode 
 *		dev   - device NO.
 *		port  - port NO.
 *		enable - en/disable device some attribute
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *			NAM_OK
 *			NAM_ERR
 *
 **********************************************************************************/
int nam_set_promi_attr_on_pvid
(
	unsigned char dev,
	unsigned char port,
	unsigned int enable,
	unsigned short vid,
	unsigned int flag
);

int nam_set_promi_attr_on_all_vlan
(
	unsigned int product_id,
	unsigned int eth_g_index,
	unsigned char dev,
	unsigned int flag
);

/* 
 * call cpss function cpssDxChBrgVlanEgressFilteringEnable and cpssDxChBrgPortEgrFltUregMcastEnable
 * if both return 0,then return 0; else return 1
 *
 */

int nam_egress_filter_enable
(
    unsigned char devNum,
    unsigned char portNum,
    unsigned long enable
);
unsigned int nam_ingress_host_counter_set(
	GT_ETHERADDR *DmacAddr,
    GT_ETHERADDR *SmacAddr
);
unsigned int nam_ingress_host_counter_smac_set(
    GT_ETHERADDR *SmacAddr
);

unsigned int nam_ingress_host_counter_dmac_set(
    GT_ETHERADDR *DmacAddr
);
unsigned int nam_asic_show_ingress_counter
(
    BRIDGE_HOST_CNTR_STC *hostGroupCntPtr,
    GT_ETHERADDR *DmacAddr,
    GT_ETHERADDR *SmacAddr,
    unsigned int *matrixCntSaDaPktsPtr 
);
unsigned int nam_asic_show_egress_counter
(
    NAM_PORT_EGRESS_CNTR_STC	*egrCntrPtr
);
 int nam_asic_config_vlan_egress_counter
(
	unsigned int vlanId
);

 int nam_asic_config_vlan_port_egress_counter
(
	unsigned int devNum,
	unsigned int portNum,
	unsigned int vlanId
);
  int nam_asic_config_port_egress_counter
(
	unsigned int devNum,
	unsigned int portNum
);
   int nam_asic_counter_drop_statistic
( 
	unsigned int *drop_Mode,
	unsigned int *drop_Count
);
 int nam_asic_config_counter_drop_statistic
( 
	   unsigned int  reason
);

extern int	bcm_port_l3_mtu_get
(
	int	unit,
	int	port,
	int *	size
);

#ifdef DRV_LIB_BCM /*added by zhengcs*/
extern int
bcm_port_autoneg_set(
	int	unit,
	int	port,
	int	autoneg);
#endif

extern int nam_set_port_preamble_length
(
    unsigned char devNum,
    unsigned char portNum,
    unsigned char direction,
    unsigned int length
);

/*******************************************************************************
* nam_eth_port_phy_reinit
*
* DESCRIPTION:
*      Reinit phy smi interface 
*
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*
* OUTPUTS:
*       NULL
*
* RETURNS:
*      0	- on success
*      other - on hardware error or parameters error 
*
* COMMENTS:
*       None.
*
*******************************************************************************/
unsigned int nam_eth_port_phy_reinit
(
	unsigned char dev,
	unsigned char port
);
#endif
