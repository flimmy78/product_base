#ifndef __CVMX_CONFIG_H__
#define __CVMX_CONFIG_H__



#define CONFIG_OCTEON_ETHERNET_LOCKED 1

extern int cvmx_helper_ports_on_interface(int);

static inline int octeon_pko_get_total_queues(void)
{
	if (OCTEON_IS_MODEL(OCTEON_CN38XX))
		return 128;
	else if (OCTEON_IS_MODEL(OCTEON_CN3XXX))
		return 32;
	else if (OCTEON_IS_MODEL(OCTEON_CN50XX))
		return 32;
	else
		return 256;
}


static inline int octeon_pko_lockless(void)
{
#ifdef CONFIG_OCTEON_ETHERNET_LOCKED
	return 0;
#else
	int queues = 16 * (cvmx_helper_ports_on_interface(0) + cvmx_helper_ports_on_interface(1));

	/* CN3XXX require workarounds in xmit.  Disable lockless for
	 * CN3XXX to optimize the lockless case with out the workarounds. */
	if (OCTEON_IS_MODEL(OCTEON_CN3XXX))
		return 0;

	if (OCTEON_IS_MODEL(OCTEON_CN68XX))
		return 0;

	queues += 4; /* For PCI/PCIe */

	if ((OCTEON_IS_MODEL(OCTEON_CN56XX) || OCTEON_IS_MODEL(OCTEON_CN52XX) || OCTEON_IS_MODEL(OCTEON_CN6XXX)))
		queues += 4; /* For loopback */

	if (octeon_has_feature(OCTEON_FEATURE_SRIO))
		queues += 4; /* For SRIO */

	return queues <= octeon_pko_get_total_queues();
#endif
}



/************************* Config Specific Defines ************************/
#define CVMX_LLM_NUM_PORTS 1
#define CVMX_NULL_POINTER_PROTECT 1
#define CVMX_ENABLE_DEBUG_PRINTS 1
/* PKO queues per port for interface 0 (ports 0-15) */
#define CVMX_PKO_QUEUES_PER_PORT_INTERFACE0 (octeon_pko_lockless() ? 16 : 2)


/*(octeon_pko_lockless() ? 16 : 1)*/
/* PKO queues per port for interface 1 (ports 16-31) */
#define CVMX_PKO_QUEUES_PER_PORT_INTERFACE1 (octeon_pko_lockless() ? 16 : 2)

/*(octeon_pko_lockless() ? 16 : 1)*/
#ifdef CONFIG_OCTEON_ETHERNET_LOCKED
#define CVMX_PKO_MAX_PORTS_INTERFACE0 CVMX_HELPER_PKO_MAX_PORTS_INTERFACE0
#define CVMX_PKO_MAX_PORTS_INTERFACE1 CVMX_HELPER_PKO_MAX_PORTS_INTERFACE1
#else
/* Limit on the number of PKO ports enabled for interface 0 */
#define CVMX_PKO_MAX_PORTS_INTERFACE0 cvmx_helper_ports_on_interface(0)
/* Limit on the number of PKO ports enabled for interface 1 */
#define CVMX_PKO_MAX_PORTS_INTERFACE1 cvmx_helper_ports_on_interface(1)
#endif
/* PKO queues per port for PCI (ports 32-35) */
#define CVMX_PKO_QUEUES_PER_PORT_PCI 1
/* PKO queues per port for Loop devices (ports 36-39) */
#define CVMX_PKO_QUEUES_PER_PORT_LOOP 1
#define CVMX_PKO_QUEUES_PER_PORT_SRIO0 2
#define CVMX_PKO_QUEUES_PER_PORT_SRIO1 2


/************************* FPA allocation *********************************/
/* Pool sizes in bytes, must be multiple of a cache line */
#define CVMX_FPA_POOL_0_SIZE (16 * CVMX_CACHE_LINE_SIZE)
#define CVMX_FPA_POOL_1_SIZE (1 * CVMX_CACHE_LINE_SIZE)
#define CVMX_FPA_POOL_2_SIZE (8 * CVMX_CACHE_LINE_SIZE)
#define CVMX_FPA_POOL_3_SIZE (8 * CVMX_CACHE_LINE_SIZE)
#define CVMX_FPA_POOL_4_SIZE (1 * CVMX_CACHE_LINE_SIZE)
#define CVMX_FPA_POOL_5_SIZE (0 * CVMX_CACHE_LINE_SIZE)
#define CVMX_FPA_POOL_6_SIZE (0 * CVMX_CACHE_LINE_SIZE)
#define CVMX_FPA_POOL_7_SIZE (0 * CVMX_CACHE_LINE_SIZE)

/* Pools in use */
/* Packet buffers */
#define CVMX_FPA_PACKET_POOL                (0)
#define CVMX_FPA_PACKET_POOL_SIZE           CVMX_FPA_POOL_0_SIZE
/* Work queue entrys */
#define CVMX_FPA_WQE_POOL                   (1)
#define CVMX_FPA_WQE_POOL_SIZE              CVMX_FPA_POOL_1_SIZE
/* PKO queue command buffers */
#define CVMX_FPA_OUTPUT_BUFFER_POOL         (2)
#define CVMX_FPA_OUTPUT_BUFFER_POOL_SIZE    CVMX_FPA_POOL_2_SIZE
/**< TIM command buffers */
#define CVMX_FPA_TIMER_POOL                 (3)             /**< TIM command buffers */
#define CVMX_FPA_TIMER_POOL_SIZE            CVMX_FPA_POOL_3_SIZE

/* Work queue entrys for TX side */
#define CVMX_FPA_TX_WQE_POOL			(4)
#define CVMX_FPA_TX_WQE_POOL_SIZE		CVMX_FPA_POOL_4_SIZE


/*************************  FAU allocation ********************************/
/* The fetch and add registers are allocated here.  They are arranged
 * in order of descending size so that all alignment constraints are
 * automatically met.  The enums are linked so that the following enum
 * continues allocating where the previous one left off, so the
 * numbering within each enum always starts with zero.  The macros
 * take care of the address increment size, so the values entered
 * always increase by 1.  FAU registers are accessed with byte
 * addresses.
 */

#define CVMX_FAU_REG_64_ADDR(x) ((x << 3) + CVMX_FAU_REG_64_START)
typedef enum {
	CVMX_FAU_REG_64_START	= 0,
	//CVMX_FAU_REG_64_END	= CVMX_FAU_REG_64_ADDR(0),
	/*add by fastfwd*/
	CVMX_FAU_REG_OQ_ADDR_INDEX     = CVMX_FAU_REG_64_ADDR(0), /**< FAU registers for the position in PKO command buffers */
        /* Array of 128 */
	CVM_FAU_SE_COEXIST_FLAG        = CVMX_FAU_REG_64_ADDR(128), /**< se coexist flag */
    CVM_FAU_INIT_STATE_FLAG        = CVMX_FAU_REG_64_ADDR(129), /**< init state flag */
    CVM_FAU_SE_CORE_MASK           = CVMX_FAU_REG_64_ADDR(130), /**< se core mask */
    CVM_FAU_PKO_ERRORS             = CVMX_FAU_REG_64_ADDR(131), /**< 64-bit counter used for total ethernet output error packets */
    CVM_FAU_ENET_INPUT_BYTES       = CVMX_FAU_REG_64_ADDR(132), /**< 64-bit counter used for total ethernet input bytes */
    CVM_FAU_ENET_INPUT_PACKETS     = CVMX_FAU_REG_64_ADDR(133), /**< 64-bit counter used for total ethernet input packets */
    CVM_FAU_ENET_OUTPUT_BYTES      = CVMX_FAU_REG_64_ADDR(134), /**< 64-bit counter used for total ethernet output bytes */
    CVM_FAU_ENET_OUTPUT_PACKETS    = CVMX_FAU_REG_64_ADDR(135), /**< 64-bit counter used for total ethernet output packets */
    CVM_FAU_ENET_DROP_PACKETS      = CVMX_FAU_REG_64_ADDR(136), /**< 64-bit counter used for total ethernet drop packets */
    CVM_FAU_ENET_TO_LINUX_PACKETS  = CVMX_FAU_REG_64_ADDR(137), /**< 64-bit counter used for total ethernet to linux packets */
    CVM_FAU_ENET_NONIP_PACKETS     = CVMX_FAU_REG_64_ADDR(138), /**< 64-bit counter used for total ethernet input noneip packets */
    CVM_FAU_ENET_ERROR_PACKETS     = CVMX_FAU_REG_64_ADDR(139), /**< 64-bit counter used for total ethernet input noneip packets */
    CVM_FAU_ENET_FRAGIP_PACKETS    = CVMX_FAU_REG_64_ADDR(140), /**< 64-bit counter used for total ethernet input noneip packets */
    CVM_FAU_IP_SHORT_PACKETS       = CVMX_FAU_REG_64_ADDR(141), /**< 64-bit counter used for short IP packets rcvd */
    CVM_FAU_IP_BAD_HDR_LEN         = CVMX_FAU_REG_64_ADDR(142), /**< 64-bit counter used for IP packets with bad hdr len */
    CVM_FAU_IP_BAD_LEN             = CVMX_FAU_REG_64_ADDR(143), /**< 64-bit counter used for IP packets with bad len */
    CVM_FAU_IP_BAD_VERSION         = CVMX_FAU_REG_64_ADDR(144), /**< 64-bit counter used for IP packets with bad version */
    CVM_FAU_IP_SKIP_ADDR           = CVMX_FAU_REG_64_ADDR(145), /**< 64-bit counter used for IP packets with SKIP addr */
    CVM_FAU_IP_ICMP                = CVMX_FAU_REG_64_ADDR(146), /**< 64-bit counter used for ICMP packets */
    CVM_FAU_CAPWAP_ICMP            = CVMX_FAU_REG_64_ADDR(147), /**< 64-bit counter used for Capwap ICMP packets */
    CVM_FAU_IP_PROTO_ERROR         = CVMX_FAU_REG_64_ADDR(148), /**< 64-bit counter used for ip packets with proto error */
    CVM_FAU_UDP_BAD_DPORT          = CVMX_FAU_REG_64_ADDR(149), /**< 64-bit counter used for udp dport=0 packets */
    CVM_FAU_UDP_BAD_LEN            = CVMX_FAU_REG_64_ADDR(150), /**< 64-bit counter used for udp packets with len error */
    CVM_FAU_UDP_TO_LINUX           = CVMX_FAU_REG_64_ADDR(151), /**< 64-bit counter used for udp packets that trap to Linux */
    CVM_FAU_FLOWTABLE_HIT_PACKETS  = CVMX_FAU_REG_64_ADDR(152), /**< 64-bit counter used for ACL HIT packets number */
    CVM_FAU_ENET_OUTPUT_PACKETS_8021Q = CVMX_FAU_REG_64_ADDR(153), /**< 64-bit counter used for total ethernet output 802.1qpackets */
    CVM_FAU_ENET_OUTPUT_PACKETS_QINQ = CVMX_FAU_REG_64_ADDR(154), /**< 64-bit counter used for total ethernet output qinq packets */
    CVM_FAU_FLOW_LOOKUP_ERROR      = CVMX_FAU_REG_64_ADDR(155), /**< 64-bit counter used for total flow lookup failed counter */
    CVM_FAU_RECV_FCCP_PACKETS      = CVMX_FAU_REG_64_ADDR(156), /**< 64-bit counter used for total received fccp packets counter */
    CVM_FAU_RECV_TOTAL_WORKS       = CVMX_FAU_REG_64_ADDR(157), /**< 64-bit counter used for total received works counter, include input packets and fccp packets */
    CVM_FAU_TOTAL_ACL_LOOKUP       = CVMX_FAU_REG_64_ADDR(158), /**< 64-bit counter used for total acl lookup counter */
    CVM_FAU_ACL_REG                = CVMX_FAU_REG_64_ADDR(159), /**< 64-bit counter used for total acl setup and regist packets counter */
    CVM_FAU_CW802_11_DECAP_ERROR   = CVMX_FAU_REG_64_ADDR(160), /**< 64-bit counter used for total capwap 802.11 decap error */
    CVM_FAU_CW802_3_DECAP_ERROR    = CVMX_FAU_REG_64_ADDR(161), /**< 64-bit counter used for total capwap 802.3 decap error */
    CVM_FAU_ENET_TO_LINUX_BYTES    = CVMX_FAU_REG_64_ADDR(162), /**< 64-bit counter used for total ethernet to linux bytes */
    CVM_FAU_ALLOC_RULE_FAIL        = CVMX_FAU_REG_64_ADDR(163), /**< 64-bit counter used for total alloc rule fail counter */
    CVM_FAU_MAX_RULE_ENTRIES       = CVMX_FAU_REG_64_ADDR(164), /**< 64-bit counter used for reach max rule entries */
    CVM_FAU_CW_80211_ERR           = CVMX_FAU_REG_64_ADDR(165), /**< 64-bit counter used for 802.11 error packet */
    CVM_FAU_CW_NOIP_PACKETS        = CVMX_FAU_REG_64_ADDR(166), /**< 64-bit counter used for capwap no ip packet */
    CVM_FAU_CW_SPE_PACKETS         = CVMX_FAU_REG_64_ADDR(167), /**< 64-bit counter used for capwap special packet */
    CVM_FAU_CW_FRAG_PACKETS        = CVMX_FAU_REG_64_ADDR(168), /**< 64-bit counter used for capwap frag packet */
    CVMX_FAU_REG_64_END            = CVMX_FAU_REG_64_ADDR(169),
    
} cvmx_fau_reg_64_t;

#define CVMX_FAU_REG_32_ADDR(x) ((x << 2) + CVMX_FAU_REG_32_START)
typedef enum {
	CVMX_FAU_REG_32_START	= CVMX_FAU_REG_64_END,
	CVM_FAU_REG_FPA_OOB_COUNT      = CVMX_FAU_REG_32_ADDR(0), /**< FPA out of buffer counters */
        /* Array of 8 */
    CVM_FAU_REG_POOL_0_USE_COUNT   = CVMX_FAU_REG_32_ADDR(8), /**< pool 0 use count */
    CVM_FAU_REG_POOL_1_USE_COUNT   = CVMX_FAU_REG_32_ADDR(9), /**< pool 1 use count */
    CVM_FAU_REG_POOL_2_USE_COUNT   = CVMX_FAU_REG_32_ADDR(10), /**< pool 2 use count */
    CVM_FAU_REG_POOL_3_USE_COUNT   = CVMX_FAU_REG_32_ADDR(11), /**< pool 3 use count */
    CVM_FAU_REG_POOL_4_USE_COUNT   = CVMX_FAU_REG_32_ADDR(12), /**< pool 4 use count */
    CVM_FAU_REG_POOL_5_USE_COUNT   = CVMX_FAU_REG_32_ADDR(13), /**< pool 5 use count */
    CVM_FAU_REG_POOL_6_USE_COUNT   = CVMX_FAU_REG_32_ADDR(14), /**< pool 6 use count */
    CVM_FAU_REG_POOL_7_USE_COUNT   = CVMX_FAU_REG_32_ADDR(15), /**< pool 7 use count */
    CVMX_FAU_REG_32_END            = CVMX_FAU_REG_32_ADDR(16),
} cvmx_fau_reg_32_t;

#define CVMX_FAU_REG_16_ADDR(x) ((x << 1) + CVMX_FAU_REG_16_START)
typedef enum {
	CVMX_FAU_REG_16_START	= CVMX_FAU_REG_32_END,
	CVMX_FAU_REG_16_END	= CVMX_FAU_REG_16_ADDR(0),
} cvmx_fau_reg_16_t;

#define CVMX_FAU_REG_8_ADDR(x) ((x) + CVMX_FAU_REG_8_START)
typedef enum {
	CVMX_FAU_REG_8_START	= CVMX_FAU_REG_16_END,
	CVMX_FAU_REG_8_END	= CVMX_FAU_REG_8_ADDR(0),
} cvmx_fau_reg_8_t;

/*
 * The name CVMX_FAU_REG_AVAIL_BASE is provided to indicate the first
 * available FAU address that is not allocated in cvmx-config.h. This
 * is 64 bit aligned.
 */
#define CVMX_FAU_REG_AVAIL_BASE ((CVMX_FAU_REG_8_END + 0x7) & (~0x7ULL))
#define CVMX_FAU_REG_END (2048)

/********************** scratch memory allocation *************************/
/* Scratchpad memory allocation.  Note that these are byte memory
 * addresses.  Some uses of scratchpad (IOBDMA for example) require
 * the use of 8-byte aligned addresses, so proper alignment needs to
 * be taken into account.
 */
/* Generic scratch iobdma area */
//#define CVMX_SCR_SCRATCH               (0)
/* First location available after cvmx-config.h allocated region. */
//#define CVMX_SCR_REG_AVAIL_BASE        (8)  //modify by fastfwd module


#define CVMX_SCR_OQ_BUF_PRE_ALLOC      (0)                  /**< Pre allocation for PKO queue command buffers */
#define CVMX_SCR_SCRATCH               (8)                  /**< Generic scratch iobdma area */
#define CVMX_SCR_SCRATCH_RESERVED0     (16)                 /**< Generic scratch iobdma area */
#define CVMX_SCR_SCRATCH_RESERVED1     (24)                 /**< Generic scratch iobdma area */
#define CVM_SCR_ACL_CACHE_PTR          (32)                 /**< Pointer to acl entry */
#define CVM_SCR_USER_CACHE_PTR         (40)                 /**< Pointer to user_table entry */
#define CVM_SCR_MBUFF_INFO_PTR         (48)                 /**< MBuff information */
#define CVM_DRV_SCR_PACKET_BUF_PTR     (56)                 /**< shit! */
#define CVM_DRV_SCR_WQE_BUF_PTR        (64)                 /**< shit! */
#define CVMX_SCR_REG_AVAIL_BASE        (72)                 /**< First location available after cvmx-config.h allocated region. */


/*
 * CVMX_HELPER_FIRST_MBUFF_SKIP is the number of bytes to reserve
 * before the beginning of the packet. If necessary, override the
 * default here.  See the IPD section of the hardware manual for MBUFF
 * SKIP details.
 */
//#define CVMX_HELPER_FIRST_MBUFF_SKIP 184
#define CVMX_HELPER_FIRST_MBUFF_SKIP 120


/*
 * CVMX_HELPER_NOT_FIRST_MBUFF_SKIP is the number of bytes to reserve
 * in each chained packet element. If necessary, override the default
 * here.
 */
#define CVMX_HELPER_NOT_FIRST_MBUFF_SKIP 0

/*
 * CVMX_HELPER_ENABLE_BACK_PRESSURE controls whether back pressure is
 * enabled for all input ports. This controls if IPD sends
 * backpressure to all ports if Octeon's FPA pools don't have enough
 * packet or work queue entries. Even when this is off, it is still
 * possible to get backpressure from individual hardware ports. When
 * configuring backpressure, also check
 * CVMX_HELPER_DISABLE_*_BACKPRESSURE below. If necessary, override
 * the default here.
 */
#define CVMX_HELPER_ENABLE_BACK_PRESSURE 1

/*
 * CVMX_HELPER_ENABLE_IPD controls if the IPD is enabled in the helper
 * function. Once it is enabled the hardware starts accepting
 * packets. You might want to skip the IPD enable if configuration
 * changes are need from the default helper setup. If necessary,
 * override the default here.
 */
#define CVMX_HELPER_ENABLE_IPD 0

/*
 * CVMX_HELPER_INPUT_TAG_TYPE selects the type of tag that the IPD assigns
 * to incoming packets.
 */
#define CVMX_HELPER_INPUT_TAG_TYPE CVMX_POW_TAG_TYPE_ORDERED

/*
 * The following select which fields are used by the PIP to generate
 * the tag on INPUT
 * 0: don't include
 * 1: include
 */
#define CVMX_HELPER_INPUT_TAG_IPV6_SRC_IP	1
#define CVMX_HELPER_INPUT_TAG_IPV6_DST_IP   	1
#define CVMX_HELPER_INPUT_TAG_IPV6_SRC_PORT 	1
#define CVMX_HELPER_INPUT_TAG_IPV6_DST_PORT 	1
#define CVMX_HELPER_INPUT_TAG_IPV6_NEXT_HEADER 	1
#define CVMX_HELPER_INPUT_TAG_IPV4_SRC_IP	1
#define CVMX_HELPER_INPUT_TAG_IPV4_DST_IP   	1
#define CVMX_HELPER_INPUT_TAG_IPV4_SRC_PORT 	1
#define CVMX_HELPER_INPUT_TAG_IPV4_DST_PORT 	1
#define CVMX_HELPER_INPUT_TAG_IPV4_PROTOCOL	1
#define CVMX_HELPER_INPUT_TAG_INPUT_PORT	0

/* Select skip mode for input ports */
#define CVMX_HELPER_INPUT_PORT_SKIP_MODE	CVMX_PIP_PORT_CFG_MODE_SKIPL2


/*
 * Force backpressure to be disabled.  This overrides all other
 * backpressure configuration.
 */
//#define CVMX_HELPER_DISABLE_RGMII_BACKPRESSURE 0

#define CVMX_HELPER_DISABLE_RGMII_BACKPRESSURE 1

/* Disable the SPI4000's processing of backpressure packets and backpressure
** generation. When this is 1, the SPI4000 will not stop sending packets when
** receiving backpressure. It will also not generate backpressure packets when
** its internal FIFOs are full. */
#define CVMX_HELPER_DISABLE_SPI4000_BACKPRESSURE 1

/* Select the number of low latency memory ports (interfaces) that
** will be configured.  Valid values are 1 and 2.
*/
#define CVMX_LLM_CONFIG_NUM_PORTS 1

/* Enable the fix for PKI-100 errata ("Size field is 8 too large in WQE and next
** pointers"). If CVMX_ENABLE_LEN_M8_FIX is set to 0, the fix for this errata will 
** not be enabled. 
** 0: Fix is not enabled
** 1: Fix is enabled, if supported by hardware
*/
#define CVMX_ENABLE_LEN_M8_FIX  1
#endif /* __CVMX_CONFIG_H__ */

