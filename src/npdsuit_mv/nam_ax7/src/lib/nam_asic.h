#ifndef __NAM_ASIC_H__
#define __NAM_ASIC_H__

/* CPU PORT NUMBER Definition */
#define CPSS_CPU_PORT_NUM_CNS           63

/** Logging levels for NPD daemon
 */
enum {
	NAM_LOGPRI_TRACE = (1 << 0),   /**< function call sequences */
	NAM_LOGPRI_DEBUG = (1 << 1),   /**< debug statements in code */
	NAM_LOGPRI_INFO = (1 << 2),    /**< informational level */
	NAM_LOGPRI_WARNING = (1 << 3), /**< warnings */
	NAM_LOGPRI_ERROR = (1 << 4)    /**< error */
};

extern void logger_setup 
(
	int _priority, 
	const char *_file, 
	int _line, 
	const char *_function
);

extern void	logger_emit 
(
	const char *format, 
	...
);

typedef enum
{
    STP_DISABLED_E = 0,
    STP_BLCK_LSTN_E,
    STP_LRN_E,
    STP_FRWRD_E
} STP_STATE_ENT;

/** Trace logging macro */
#define NAM_TRACE(expr)   do {logger_setup(NAM_LOGPRI_TRACE,   __FILE__, __LINE__, __FUNCTION__); logger_emit expr; } while(0)

/** Debug information logging macro */
#define NAM_DEBUG(expr)   do {logger_setup(NAM_LOGPRI_DEBUG,   __FILE__, __LINE__, __FUNCTION__); logger_emit expr; } while(0)

/** Information level logging macro */
#define NAM_INFO(expr)    do {logger_setup(NAM_LOGPRI_INFO,    __FILE__, __LINE__, __FUNCTION__); logger_emit expr; } while(0)

/** Warning level logging macro */
#define NAM_WARNING(expr) do {logger_setup(NAM_LOGPRI_WARNING, __FILE__, __LINE__, __FUNCTION__); logger_emit expr; } while(0)

/** Error leve logging macro */
#define NAM_ERROR(expr)   do {logger_setup(NAM_LOGPRI_ERROR,   __FILE__, __LINE__, __FUNCTION__); logger_emit expr; } while(0)

/** Macro for terminating the program on an unrecoverable error */
#define NAM_DIE(expr) do {printf("*** [DIE] %s:%s():%d : ", __FILE__, __FUNCTION__, __LINE__); printf expr; printf("\n"); exit(1); } while(0)


#define REG_ADDR 0x01800010

#define CPSS_INITIALIZED			1
#define CPSS_UNINITIALIZED		0
#define NAM_ASIC_INSTANCE_MAX		2

typedef union
{
    unsigned char   c[8];
    unsigned short  s[4];
    unsigned long   l[2];
}GT_U64;

#if 0   /* redefine follow cpss3.4 */
typedef struct
{
    GT_U64 goodOctetsRcv;
    GT_U64 badOctetsRcv;
    GT_U64 macTransmitErr;
    GT_U64 goodPktsRcv;
    GT_U64 badPktsRcv;
    GT_U64 brdcPktsRcv;
    GT_U64 mcPktsRcv;
    GT_U64 pkts64Octets;
    GT_U64 pkts65to127Octets;
    GT_U64 pkts128to255Octets;
    GT_U64 pkts256to511Octets;
    GT_U64 pkts512to1023Octets;
    GT_U64 pkts1024tomaxOoctets;
    GT_U64 goodOctetsSent;
    GT_U64 goodPktsSent;
    GT_U64 excessiveCollisions;
    GT_U64 mcPktsSent;
    GT_U64 brdcPktsSent;
    GT_U64 unrecogMacCntrRcv;
    GT_U64 fcSent;
    GT_U64 goodFcRcv;
    GT_U64 dropEvents;
    GT_U64 undersizePkts;
    GT_U64 fragmentsPkts;
    GT_U64 oversizePkts;
    GT_U64 jabberPkts;
    GT_U64 macRcvError;
    GT_U64 badCrc;
    GT_U64 collisions;
    GT_U64 lateCollisions;
    GT_U64 badFcRcv;

} CPSS_PORT_MAC_COUNTER_SET_STC;

#else
/*
 * typedef: struct CPSS_PORT_MAC_COUNTER_SET_STC
 *
 * Description: Counters for BULK API  of the ethernet MAC Counters.
 *              The actual counter size (32 or 64 bits counters) is according
 *              the hardware device, refer to data sheet for more info.
 *
 * Fields:
 *  goodOctetsRcv           - Number of ethernet frames received that are
 *                            not bad ethernet frames or MAC Control pkts.
 *                            This includes Bridge Control packets (LCAP, BPDU)
 *  badOctetsRcv            - Sum of lengths of all bad ethernet frames received
 *  goodPktsRcv             - Number of ethernet frames received that are
 *                            neither bad ethernet frames nor MAC Control pkts.
 *                            This includes Bridge Control packets (LCAP, BPDU)
 *                            Not Supported for DxCh and ExMxPm.
 *  badPktsRcv              - Number of corrupted ethernet frames received
 *                            Not Supported for DxCh and ExMxPm.
 *  brdcPktsRcv             - Total number of undamaged packets received that
 *                            were directed to the broadcast address
 *  mcPktsRcv               - Total number of undamaged packets received that
 *                            were directed to a multicast address
 *  goodPktsSent            - Number of ethernet frames sent from this MAC.
 *                            This does not include 802.3 Flow Control packets,
 *                            packets dropped due to excessive collision or
 *                            packets with a Tx Error.
 *                            Not Supported for DxCh and ExMxPm.
 *  goodOctetsSent          - Sum of lengths of all good ethernet frames sent
 *                            from this MAC.
 *                            This does not include 802.3 Flow Control packets,
 *                            packets dropped due to excessive collision or
 *                            packets with a Tx Error.
 *  brdcPktsSent            - Total number of good packets sent that have a
 *                            broadcast destination MAC address.
 *                            This does not include 802.3 Flow Control packets,
 *                            packets dropped due to excessive collision or
 *                            packets with a Tx Error.
 *  mcPktsSent              - Total number of good packets sent that have a
 *                            multicast destination MAC address.
 *                            This does not include 802.3 Flow Control packets,
 *                            packets dropped due to excessive collision or
 *                            packets with a Tx Error.
 *  pkts64Octets            - Total number of received and transmitted undamaged
 *                            and damaged frames which are 64 bytes in size.
 *                            This does not include MAC Control Frames.
 *  pkts65to127Octets       - Total number of received and transmitted undamaged
 *                            and damaged frames which are 65 to 127 bytes in
 *                            size. This does not include MAC Control Frames.
 *  pkts128to255Octets      - Total number of received and transmitted undamaged
 *                            and damaged frames which are 128 to 255 bytes in
 *                            size. This does not include MAC Control Frames.
 *  pkts256to511Octets      - Total number of received and transmitted undamaged
 *                            and damaged frames which are 256 to 511 bytes in
 *                            size. This does not include MAC Control Frames.
 *  pkts512to1023Octets     - Total number of received and transmitted undamaged
 *                            and damaged frames which are 512 to 1023 bytes in
 *                            size. This does not include MAC Control Frames.
 *  pkts1024tomaxOoctets    - Total number of received and transmitted undamaged
 *                            and damaged frames which are more than 1024 bytes
 *                            in size. This does not include MAC Control Frames.
 *                            (xCat2: the behavior of this counter is determined by
 *                            oversized packets counter mode.
 *                            See: cpssDxChMacOversizedPacketsCounterModeSet.) 
 *  collisions              - Total number of collisions seen by the MAC
 *  lateCollisions          - Total number of late collisions seen by the MAC
 *                            (xCat2: also counts unknown MAC control frames)
 *  excessiveCollisions     - Number of frames dropped in the transmit MAC due
 *                            to excessive collisions. This is an applicable for
 *                            Half-Duplex mode only.
 *  macRcvError             - Number of Rx Error events seen by the receive side
 *                            of the MAC
 *  macTransmitErr          - Number of frames not transmitted correctly or
 *                            dropped due to internal MAC Tx error
 *  badCrc                  - Number of CRC error events.
 *  dropEvents              - Number of instances that the port was unable to
 *                            receive packets due to insufficient bandwidth to
 *                            one of the PP internal resources, such as the DRAM
 *                            or buffer allocation.
 *  undersizePkts           - Number of undersize packets received.
 *  fragmentsPkts           - Number of fragments received.
 *  oversizePkts            - Number of oversize packets received.
 *  jabberPkts              - Number of jabber packets received.
 *  unrecogMacCntrRcv       - Number of received MAC Control frames that have an
 *                            opcode different from 00-01.
 *                            Not Supported for DxCh and ExMxPm.
 *  goodFcRcv               - Number of good Flow Control frames received
 *  fcSent                  - Number of Flow Control frames sent.
 *  badFcRcv                - Number of bad Flow Control frames received.
 *                            Not Supported for DxCh and ExMxPm.
 *  ucPktsRcv               - Number of Ethernet Unicast frames received that
 *                            are not bad Ethernet frames or MAC Control packets.
 *                            Note that this number includes Bridge Control
 *                            packets such as LCAP and BPDU.
 *                            Supported for DxCh and ExMxPm only.
 *  ucPktsSent              - Number of good frames sent that had a Unicast
 *                            destination MAC Address.
 *                            Supported for DxCh and ExMxPm only.
 *  multiplePktsSent        - Valid Frame transmitted on half-duplex link that
 *                            encountered more then one collision.
 *                            Byte count and cast are valid.
 *                            Supported for DxCh and ExMxPm only.
 *  deferredPktsSent       - Valid frame transmitted on half-duplex link with
 *                            no collisions, but where the frame transmission
 *                            was delayed due to media being busy. Byte count
 *                            and cast are valid.
 *                            Supported for DxCh and ExMxPm only.
 *                            (xCat2: the behavior of this counter is determined by
 *                            oversized packets counter mode.
 *                            See: cpssDxChMacOversizedPacketsCounterModeSet.)
 */

typedef struct
{
    GT_U64 goodOctetsRcv;
    GT_U64 badOctetsRcv;
    GT_U64 macTransmitErr;
    GT_U64 goodPktsRcv;
    GT_U64 badPktsRcv;
    GT_U64 brdcPktsRcv;
    GT_U64 mcPktsRcv;
    GT_U64 pkts64Octets;
    GT_U64 pkts65to127Octets;
    GT_U64 pkts128to255Octets;
    GT_U64 pkts256to511Octets;
    GT_U64 pkts512to1023Octets;
    GT_U64 pkts1024tomaxOoctets;
    GT_U64 goodOctetsSent;
    GT_U64 goodPktsSent;
    GT_U64 excessiveCollisions;
    GT_U64 mcPktsSent;
    GT_U64 brdcPktsSent;
    GT_U64 unrecogMacCntrRcv;
    GT_U64 fcSent;
    GT_U64 goodFcRcv;
    GT_U64 dropEvents;
    GT_U64 undersizePkts;
    GT_U64 fragmentsPkts;
    GT_U64 oversizePkts;
    GT_U64 jabberPkts;
    GT_U64 macRcvError;
    GT_U64 badCrc;
    GT_U64 collisions;
    GT_U64 lateCollisions;
    GT_U64 badFcRcv;
    GT_U64 ucPktsRcv;
    GT_U64 ucPktsSent;
    GT_U64 multiplePktsSent;
    GT_U64 deferredPktsSent;

} CPSS_PORT_MAC_COUNTER_SET_STC;
#endif

unsigned int nam_asic_legal_device_number
(
	unsigned int devNum
);

/**********************************************************************************
 * nam_asic_get_instance_num
 * 
 * get asic instance number
 *
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		number of asic instance
 *		
 **********************************************************************************/
unsigned long nam_asic_get_instance_num
(
	void
);
/**********************************************************************************
 * nam_asic_get_cpu_interface_type
 * 
 * get asic CPU interface type :using SDMA or using Ethernet MAC
 *
 *
 *	INPUT:
 *		devNum - device number
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 - using SDMA (PCI interface)
 *		1 - using Ethernet MAC (SMI interface)
 *		
 **********************************************************************************/
unsigned long nam_asic_get_cpu_interface_type
(
	unsigned char devNum
);

int nam_asic_stp_port_enable
(
	unsigned char devNum, 
	unsigned int portId,
	unsigned char stp_enable
);

int nam_asic_stp_port_state_update
(
	unsigned char devNum,
	unsigned int portId,
	unsigned char stpState
);

int nam_asic_stp_init
(
	void
);

/**********************************************************************************
 * nam_asic_set_cpufc_queue_quota
 * 
 * DESCRIPTION:
 *	This method set CPU port queue quota.	 
 *
 *
 *	INPUT:
 *		queue - queue number
 *		quota - quota assigned to the queue
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		
 *		
 **********************************************************************************/
unsigned long nam_asic_set_cpufc_queue_quota
(
	unsigned int queue,
	unsigned int quota
);

/**********************************************************************************
 * nam_asic_get_cpufc_queue_quota
 * 
 * DESCRIPTION:
 *	This method get CPU port queue quota.	 
 *
 *
 *	INPUT:
 *		queue - queue number
 *	
 *	OUTPUT:
 *		quota - quota assigned to the queue
 *
 * 	RETURN:
 *		
 *		
 **********************************************************************************/
unsigned long nam_asic_get_cpufc_queue_quota
(
	unsigned int queue,
	unsigned int *quota
);

/**********************************************************************************
 * nam_asic_set_cpufc_queue_shaper
 * 
 * DESCRIPTION:
 *	This method set CPU port queue shaper value.	 
 *
 *
 *	INPUT:
 *		queue - queue number
 *		shaper - shaper value assigned to the queue (unit is PPS)
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		
 *		
 **********************************************************************************/
unsigned long nam_asic_set_cpufc_queue_shaper
(
	unsigned int queue,
	unsigned int shaper
);

/**********************************************************************************
 * nam_asic_set_cpufc_port_shaper
 * 
 * DESCRIPTION:
 *	This method set CPU port shaper value.	 
 *
 *
 *	INPUT:
 *		queue - queue number
 *		shaper - shaper value assigned to the queue (unit is PPS)
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		
 *		
 **********************************************************************************/
unsigned long nam_asic_set_cpufc_port_shaper
(
	unsigned int shaper
);

/**********************************************************************************
 * nam_asic_show_cpu_sdma_info
 * 
 * DESCRIPTION:
 *	This method show CPU sdma channel descriptor info
 *
 *
 *	INPUT:
 *		devNum - asic device number
 *		queue - queue number
 *		direction - cpu SDMA channel direction: Rx or Tx or Both
 *		showSize - show buffer size
 *	
 *	OUTPUT:
 *		outStr - output string buffer
 *
 * 	RETURN:
 *		
 *		
 **********************************************************************************/
unsigned long nam_asic_show_cpu_sdma_info
(
	unsigned char devNum,
	unsigned char queue,
	unsigned char direction,
	unsigned char **outStr,
	unsigned int showSize
);

/**********************************************************************************
 * nam_asic_get_cpu_sdma_mib
 * 
 * DESCRIPTION:
 *	This method get CPU sdma channel Rx packets/octets and resource error counter
 *
 *
 *	INPUT:
 *		devNum - asic device number
 *	
 *	OUTPUT:
 *		stats - cpu port dma channel mib info
 *
 * 	RETURN:
 *		0 - if no error occurred.
 *		other - if error found.
 *		
 **********************************************************************************/
unsigned long nam_asic_get_cpu_sdma_mib
(
	unsigned char devNum,
	CPU_PORT_DMA_CHANNEL_MIB_S **stats
);

/**********************************************************************************
 * nam_asic_get_cpu_mac_mib
 * 
 * DESCRIPTION:
 *	This method get CPU port Ethernet MAC mib counter
 *
 *
 *	INPUT:
 *		devNum - asic device number
 *	
 *	OUTPUT:
 *		stats - cpu port ethernet mac mib info
 *
 * 	RETURN:
 *		0 - if no error occurred.
 *		other - if error found.
 *		
 **********************************************************************************/
unsigned long nam_asic_get_cpu_mac_mib
(
	unsigned char devNum,
	CPU_PORT_MAC_MIB_S *stats
);

/**********************************************************************************
 * nam_asic_get_port_mac_mib
 * 
 * DESCRIPTION:
 *	This method get port Ethernet MAC mib counter
 *
 *
 *	INPUT:
 *		devNum - asic device number
 *		portNum - asic port number
 *	
 *	OUTPUT:
 *		stats - port ethernet mac mib info
 *
 * 	RETURN:
 *		0 - if no error occurred.
 *		other - if error found.
 *		
 **********************************************************************************/
unsigned long nam_asic_get_port_mac_mib
(
	unsigned char devNum,
	unsigned char portNum,
	ASIC_PORT_MAC_MIB_DIAG_S *stats
);


/**********************************************************************************
 * nam_oam_traffic_setup
 * 
 * DESCRIPTION:
 *	This method set OAM traffic configuration.	 
 *
 *
 *	INPUT:
 *		oamType - OAM traffic type
 *		oamInfo - OAM traffic controller
 *	
 *	OUTPUT:
 *		oamInfo - OAM traffic controller
 *
 * 	RETURN:
 *		
 *		
 **********************************************************************************/
int nam_oam_traffic_setup
(
	enum OAM_TRAFFIC_TYPE_ENT oamType,
	struct oam_ctrl_s *oamInfo
);

unsigned int nam_board_after_enable();

#ifdef DRV_LIB_CPSS
extern int cpssUserAppMain
(
	unsigned int product_id
);

#ifdef DRV_LIB_CPSS_3_4
extern unsigned long cpssDrvPpHwRegisterRead
(
    unsigned char 	devNum,
    unsigned long	portGroupId,
    unsigned long	regAddr,
    unsigned long	*data
);
#else
extern unsigned long cpssDrvPpHwRegisterRead
(
    unsigned char 	devNum,
    unsigned long	regAddr,
    unsigned long	*data
);
#endif

extern unsigned long prvCpssDrvHwPpSetRegField
(
    unsigned char 	devNum,
    unsigned long 	regAddr,
    unsigned long 	fieldOffset,
    unsigned long 	fieldLength,
    unsigned int 	fieldData
);

/*******************************************************************************
* cpssDxChNetIfCpufcRateLimiterSet
*
* DESCRIPTION:
*       Set CPU port Rx flow control for specified packet
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum    	- Device number.
*	  packetType - Packet type to set cpu flow control
*       windowSize        - window size for this Rate Limiter in steps of
*                           Rate Limiter Window Resolution set by
*                       cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet(...)
*                           range 0..0xFFF (12 bits)
*       pktLimit          - number of packets allowed to be sent to CPU
*                           during within the configured windowSize
*                           range 0..0xFFFF (16 bits)
*	
* OUTPUTS:
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
extern unsigned long cpssDxChNetIfCpufcRateLimiterSet
(
	unsigned char devNum,
	unsigned int 	packetType,
	unsigned int	windowSize,
	unsigned int 	pktCnt
);

/*******************************************************************************
* cpssDxChNetIfOamTrafficSet
*
* DESCRIPTION:
*       Set range to TCP/UPD Destination Port Range CPU Code Table with specific
*       CPU Code. There are 16 ranges may be defined.
*       The feature enabled for packets if one of conditions are true:
*        1. MAC DA is found in the FDB and Application Specific CPU Code field
*           of FDB entry is True (see appCpuCodeEn in GT_MAC_ENTRY)
*        2. Packet passed Routing and Application Specific CPU Code field is
*           True in the Route entry (Next Hop)
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum  - Device number.
*       oamType - OAM traffic type
*       dstPort - OAM traffic destination port.
*	  rangeIdx - tcp/udp range index add to hw table
*
* OUTPUTS:
*       rangeIdx - tcp/udp range index acutally add to hw table
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad rangeIndex or
*                          bad packetType or bad protocol or bad cpuCode
*
* COMMENTS:
*	  if hw table range index need allocate by driver, value ~0UL should given to rangeIdx;
*  otherwise, the given value of rangeIdx will be used. 
*
*******************************************************************************/
extern unsigned long cpssDxChNetIfOamTrafficSet
(
	unsigned char					devNum,
	enum OAM_TRAFFIC_TYPE_ENT		oamType,
	unsigned short					dstPort,
	unsigned long					*rangeIdx
);

/*******************************************************************************
* cpssDxChSdmaRxQueueQuotaSet
*
* DESCRIPTION:
*       This method set SDMA Rx queue quota for the specified {device,queue}
*
* INPUTS:
* 	devNum  - device number
*	queue - queue number
*	quota - quota assigned to the queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
extern unsigned long cpssDxChSdmaRxQueueQuotaSet
(
	unsigned char devNum,
	unsigned int queue,
	unsigned int quota
);

/*******************************************************************************
* cpssDxChSdmaRxQueueQuotaGet
*
* DESCRIPTION:
*       This method get SDMA Rx queue quota for the specified {device,queue}
*
* INPUTS:
* 	devNum  - device number
*	queue - queue number
*
* OUTPUTS:
*	quota - quota assigned to the queue
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
extern unsigned long cpssDxChSdmaRxQueueQuotaGet
(
	unsigned char devNum,
	unsigned int queue,
	unsigned int *quota
);
#if 0
/*******************************************************************************
* cpssDxChPortTxShaperEnableSet
*
* DESCRIPTION:
*       Enable/Disable Token Bucket rate shaping on specified port of
*       specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       enable  - GT_TRUE, enable Shaping
*                 GT_FALSE, disable Shaping
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
extern unsigned long cpssDxChPortTxShaperEnableSet
(
    unsigned char devNum,
    unsigned char portNum,
    unsigned long  enable
);

/*******************************************************************************
* cpssDxChPortTxShaperProfileSet
*
* DESCRIPTION:
*       Set Token Bucket Shaper Profile on specified port of specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*       burstSize  - burst size in units of 4K bytes
*                    (max value is 4K which results in 16K burst size)
*       maxRatePtr - Requested Rate in Kbps
*
* OUTPUTS:
*       maxRatePtr   - (pointer to) the actual Rate value in Kbps.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
extern unsigned long cpssDxChPortTxShaperProfileSet
(
    unsigned char  devNum,
    unsigned char  portNum,
    unsigned short burstSize,
    unsigned long  *maxRatePtr
);

/*******************************************************************************
* cpssDxChPortTxQShaperEnableSet
*
* DESCRIPTION:
*       Enable/Disable shaping of transmitted traffic from a specified Traffic
*       Class Queue and specified port of specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum  - physical device number
*       portNUm - physical port number
*       tcQueue - traffic class queue on this port
*       enable  - GT_TRUE, enable shaping on this traffic queue
*                 GT_FALSE, disable shaping on this traffic queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
extern unsigned long cpssDxChPortTxQShaperEnableSet
(
    unsigned char  devNum,
    unsigned char  portNum,
    unsigned char  tcQueue,
    unsigned long  enable
);

/*******************************************************************************
* cpssDxChPortTxQShaperProfileSet
*
* DESCRIPTION:
*       Set Shaper Profile for Traffic Class Queue of specified port on
*       specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum       - physical device number
*       portNum      - physical port number
*       tcQueue      - traffic class queue on this Logical Port
*       burstSize    - burst size in units of 4K bytes
*                      (max value is 4K which results in 16K burst size)
*       userRate Ptr - Requested Rate in Kbps
*
* OUTPUTS:
*       userRatePtr  - (pointer to) the actual Rate value in Kbps
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
extern unsigned long cpssDxChPortTxQShaperProfileSet
(
    unsigned char  devNum,
    unsigned char  portNum,
    unsigned char  tcQueue,
    unsigned short burstSize,
    unsigned long  *userRatePtr
);
#endif
/*******************************************************************************
* cpssDxChNetIfSdmaPrint
*
* DESCRIPTION:
*       This function print out SDMA Rx/Tx DESC and corresponding packet buffers
*       
*
* APPLICABLE DEVICES: ALL DXCH Devices
*
* INPUTS:
*     devNum  - The device that was removed.
*     queueIdx - SDMA queue index (0-7)
*	direction - for Rx/Tx or both directions( 0 for Rx, 1 for Tx, 2 for both)
* 	printSize - print out buffer size
*
* OUTPUTS:
*       None if printStr is NULL
*	  SDMA queues' Rx or Tx descriptor detailed info
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_BAD_PARAM - one of the parameters value is wrong
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void cpssDxChNetIfSdmaPrint
(
	unsigned char	devNum,
	unsigned char	queueIdx,
	unsigned char   direction,
	unsigned char 	**printStr,
	unsigned long	printSize
);

/*******************************************************************************
* cpssDxChNetIfSdmaRxMibGet
*
* DESCRIPTION:
*       This function get SDMA Rx channel packet/octets and resource error counters.
*       
*
* APPLICABLE DEVICES: ALL DXCH Devices
*
* INPUTS:
*     devNum  - The device that was removed.
*
* OUTPUTS:
*     statistics - all SDMA queues' mib counter info.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_BAD_PARAM - one of the parameters value is wrong
*	  GT_BAD_PTR - output parameter with null pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
extern unsigned long cpssDxChNetIfSdmaRxMibGet
(
	unsigned char	devNum,
	CPU_PORT_DMA_CHANNEL_MIB_S **statistics
);

/*******************************************************************************
* cpssDxChPortMacCountersOnPortGet
*
* DESCRIPTION:
*       Gets Ethernet MAC counter for a particular Port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*                  CPU port counters are valid only when using "Ethernet CPU
*                  port" (not using the SDMA interface).
*                  when using the SDMA interface refer to the API
*                  cpssDxChNetIfSdmaRxCountersGet(...)
*
* OUTPUTS:
*       portMacCounterSetArray - array of current counter values.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
extern unsigned long cpssDxChPortMacCountersOnPortGet
(
    unsigned char devNum,
    unsigned char portNum,
    CPSS_PORT_MAC_COUNTER_SET_STC   *portMacCounterSetArray
);

extern unsigned long cpssDrvPpHwRegFieldGet
(
	unsigned char	devNum,
	unsigned long	regAddr,
    unsigned long   fieldOffset,
    unsigned long   fieldLength,
    unsigned long  *fieldData
);
#endif
/*******************************************************************************
* npd_init_tell_whoami
*
* DESCRIPTION:
*      This function is used by each thread to tell its name and pid to NPD_ALL_THREAD_PID_PATH
*
* INPUTS:
*	  tName - thread name.
*	  lastTeller - is this the last teller or not, pid file should be closed if true.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*	  None.
*
* COMMENTS:
*       
*
*******************************************************************************/
extern void npd_init_tell_whoami
(
	unsigned char *tName,
	unsigned char lastTeller
);
#ifdef DRV_LIB_BCM
extern int bcmUserAppMain
(
    unsigned int productid
);

extern int bcm_debug_level_set
(
	int level
);

extern int bcm_debug_level_clr
(
	int level
);
#endif
#ifdef DRV_LIB_CPSS
extern int  cpss_debug_level_set
(
	unsigned int level
);

extern int cpss_debug_level_clr
(
	unsigned int level
);
#endif
#endif
