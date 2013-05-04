#ifndef __NBM_CPLD_H__
#define __NBM_CPLD_H__

#define BM_BIG_ENDIAN 0
#define BM_LITTLE_ENDIAN 1

#define SYSINFO_SN_LENGTH	 20
#define SYSINFO_PRODUCT_NAME  24
#define SYSINFO_SOFTWARE_NAME 24
#define SYSINFO_ENTERPRISE_NAME  64
#define SYSINFO_ENTERPRISE_SNMP_OID  128
#define SYSINFO_BUILT_IN_ADMIN_USERNAME  32
#define SYSINFO_BUILT_IN_ADMIN_PASSWORD  32

#define SYSINFO_MODULE_SERIAL_NO  20
#define SYSINFO_MODULE_NAME       32

#ifndef MAC_ADDRESS_LEN
#define MAC_ADDRESS_LEN 	6
#endif

#define uint64_t unsigned long long
#define uint16_t unsigned short
#define temp1_input "/sys/bus/i2c/devices/0-0018/temp1_input"
#define temp2_input "/sys/bus/i2c/devices/0-0018/temp2_input"
#define NPD_BM_FILE_PATH "/dev/bm0"

/* GPIO for chassis board running state */
#define AX7_GPIO_STATE_REG_ADDR 		0x8001070000000880ULL

/* GPIO for reset chassis board */
#define AX7_GPIO_TX_SET_REG_ADDR 		0x8001070000000888ULL
#define AX7_GPIO_TX_CLR_REG_ADDR		0x8001070000000890ULL


/**
  * CPLD test register
  *		reset value is 0x55,used for testing CPLD work state
  *		Validate HOWTO:
  *			1.After system booting,read this register from CPU,check if value has 0x55;
  *			2.And then write value 0xaa, read back to check if value is consistent;
  *			3.If all of the above process OK,CPLD is working correctly.
  *			4.At the end write 0x55 to recovery reset value;
  */
#define AX_BOARD_STATE_CPLD_REG		0x00

/**
  *	System Board CPLD Type register
  *		same validate method as CPLD test register
  *		but different values for specific Board
  *			0x55 - Master CPLD on Master Board(with CPU)
  *			0x5A - Slave CPLD on Master Board(with CPU)
  *			0x33 - CPLD for Slave Board(sub card without CPLD)
  */
#define AX_CPLD_TYPE_CPLD_REG				AX_BOARD_STATE_CPLD_REG

/**
  *	Product Type register
  *		valid bit[3:0], value for specific product
  *			0b0111 - AX7000 series 
  *        		0b0101 - AX5000 series 
  *			0b0100 - AU4000 series
  *			0b0011 - AU3000 series 
  */
#define AX_PRODUCT_TYPE_CPLD_REG		0x01

/**
  *	Backplane Type and Version Register
  *		Relevant for chassis product only, valid if and only if chassis product and CPLD type has 0x55.
  *			bit[4:2] - backplane type
  *			bit[1:0] - backplane version
  *		NOTE:
  *			For box product, this register is reserved.
  */
#define AX_BACKPLANE_TYPE_CPLD_REG   	0x02

/**
  *	Single Board Type register
  *		For different product type, this register has different definition
  *		PRODUCT TYPE AX7000 series, value has 
  *			0x00 - 6GTX
  *			0x02 - 6SFP
  *			0x04 - 1XFP
  *			0x08 - 6GTX-PoE
  *
  *		PRODUCT TYPE AX5000 series, valid bit[2:0], value has
  *			0b000 - AX5612
  *			0b003 - AX5612I
  *			0b061 - AX5612E
  *			other values currently reserved.
  *
  *		PRODUCT TYPE AU4000 series, valid bit[2:0], value has
  *			0b000 - reserved
  *			0b001 - AU4626
  *			0b010 - AU4524
  *			0b011 - AU4524-P
  * 			...
  *		
  *		PRODUCT TYPE AU3000 series, valid bit[2:0], value has
  *			0b000 - AU3524
  *			0b001 - AU3052/AU3052E
  * 			0b010 - AU3028/AU3028E
  *			0b011 - 
  *			...
  */
#define AX_SINGLEPLANE_TYPE_CPLD_REG 	0x03

/**
  * 	Board Hw PCB Version register
  *		valid bit[3:0], PCB version number has sequencial value start from 0b0000 for 1, 0b0001 for 2,...
  */
#define AX_PCB_VERSION_CPLD_REG	0x04	

/**
  * 	CPLD Version register
  *		valid bit[3:0], CPLD version number has sequencial value start from 0b0000 for 1, 0b0001 for 2,...
  */
#define AX_CPLD_VERSION_CPLD_REG			0x05

/**
  * 	CPLD Fiber-optic and electric ports mutex register
  *		bit[4] for ge0 port, enable Fiber-optic ports and electric ports mutex
  */
#define AX_CPLD_LED_MUTEX_CPLD_REG			0x06

/*
  *	CPLD Watch Dog control register
  *		valid bit[0], 2b'0 means disable watch dog, 2b'1 means enable watch dog 
  * 	Note:
  *		by default, hardware reset with value 0xFE which means watch dog is disabled.
  *	As usual, a value 0xFF should given to enable watch dog.
  */
#define AX_CPLD_WATCHDOG_ENABLE_REG		0x0D

/*
  *	CPLD Watch Dog timeout specification register
  *		valid bit[7:0], by default, a value 0x1F is given, which means 32 seconds.
  *		This register gives a N value which represents N times of TIME_UNIT, and TIME_UNIT is defined
  *  by a watchdog chip. In adm706, a TIME_UNIT is about 1 ~ 1.6 seconds.
  */
#define AX_CPLD_WATCHDOG_TIMEOUT_REG	0x0E

/*
  * 	CPLD Watch Dog fillup register
  * 		valid bit[7:0], after each cycle of read or write operation to this regsiter , hardware watchdog counter is 
  *	cleared, or to say hardware watchdog is filled up once.
  */
#define AX_CPLD_WATCHDOG_FILLUP_REG	0x0F

/**
  * System HW State register
  *		This register monitor system Hw facilities: system fan and system power
  *		bit[7] - invalid
  *		bit[6:5] - 00 indicates CRSMU has inserted into back plane correctly.
  *				 other value indicate CRSMU has been partially inserted to back plane.
  *		bit[4] - 	1 for fan state alarm
  *			  	0 for fan state normal
  *		bit[3] -	1 for system power voltage is ok
  *				0 for system power abnormal
  *		bit[2:0] - system power module 1-3 state 
  */
#define AX7_SYS_FAN_STATE_CPLD_REG	0x11 /*change form 0x12 to 0x11*/

/**
  * System Initialize State register
  *		This register indicates system initialization process incomplete or completed
  *		bit[7:1] - invalid
  *		bit[0] - 	0 for initialization is undergo
  *				1 for initialization is done
  *
  */
#define AX_SYSTEM_INIT_STAGE_REG	0x21

/**
  * System shutdown power control register
  *		currently no detailed description available
  */
#define AX_CPLE_TYPE_POWER_OFF_REG	0x25 /*shutdown power*/

/**
  * System HW CPLD LED register
  *		This register  records port media, is fiber or copper
  *		bit[1:0] - port media
  *		bit[0] - port on unit 0, if 0 ,media is fiber ,if 1,media is copper
  *		bit[1] - port on unit 1, if 0 ,media is fiber ,if 1,media is copper
  */
#define AX_SYS_LED_PORT_MEDIA_CPLD_REG	0x27

/**
  * Local board slot number register
  *		This register indicates which slot the current board is on
  *		bit[0] - slot number, currently support 2 mainboards at most,
  *			  0 - current board on slot #0
  *			  1 - current board on slot #1
  */
#define AX_LOCALBOARD_SLOTNO_CPLD_REG	0x28

/**
  * Peer board on-slot status register
  *		This register indicates whether the peer slot (slot 0 with peer 1 and vice versa) 
  *	is plugin in or pull of. 
  *		bit[0] - slot plugin in status ,
  *			  0 - peer board is active
  *			  1 - peer board is inactive
  */
#define AX_PEERBOARD_STATUS_CPLD_REG	0x29

/**
  * Peer board reset control register
  *		This register controls reset operation to the peer board (slot 0 with peer 1 and vice versa) 
  *		resgister is writable only (read operation with no significance).
  *		When doing reset operation, obey the action sequece as follow: 
  *			1. write a 0 to this register
  *			2. delay for at least 10ms
  *			3. write a 1 to this register
  *		then the peer board will reload
  */
#define AX_PEERBOARD_RESET_CPLD_REG		0x2A

/**
  * System Power State register
  *		This register monitor system Hw facilities: system power
  *		bit[3] -	0 for system power4 was plugged in.
  *				1 for system power4 was plugged out.
  *		bit[2] -	0 for system power3 was plugged in.
  *				1 for system power3 was plugged out.
  *		bit[1] -	0 for system power2 was plugged in.
  *				1 for system power2 was plugged out.
  *		bit[0] -	0 for system power1 was plugged in.
  *				1 for system power1 was plugged out.
  */
#define AX8_SYS_POWER_STATE_CPLD_REG	0x32

/**
  * Local board temperature
  *         bit[0] -	0  Normal
  *				1  Alarm
  */
#define AX_LOCALBOARD_TMP_ALARM_CPLD_REG    0x3F

#define SYS_FAN_STATE_BIT	4
#define AX7_ERR_MSG_SECOND_PRINT_INTERVAL	1	 /* every   second  interval*/
#define AX7_ERR_MSG_MINUTE_PRINT_INTERVAL	60	 /* every   minute  interval*/

#define  CVMX_GMXX_RXX_RX_INBND(offset,block_id)             (0x8001180008000060ull+(((offset)&3)*2048)+(((block_id)&1)*0x8000000ull))
#define  CVMX_GMXX_PRTX_CFG(offset,block_id)                 (0x8001180008000010ull+(((offset)&3)*2048)+(((block_id)&1)*0x8000000ull))
#define  CVMX_GMXX_RXX_FRM_MAX(offset,block_id)              (0x8001180008000030ull+(((offset)&3)*2048)+(((block_id)&1)*0x8000000ull))
#define  CVMX_GMXX_RXX_FRM_CTL(offset,block_id)              (0x8001180008000018ull+(((offset)&3)*2048)+(((block_id)&1)*0x8000000ull))
#define  CVMX_GMXX_RXX_JABBER(offset,block_id)               (0x8001180008000038ull+(((offset)&3)*2048)+(((block_id)&1)*0x8000000ull))
#define  CVMX_GMXX_TX_OVR_BP(block_id)                       (0x80011800080004C8ull+(((block_id)&1)*0x8000000ull))
#define  CVMX_ASXX_RX_PRT_EN(block_id)                       (0x80011800B0000000ull+(((block_id)&1)*0x8000000ull))
#define  CVMX_ASXX_TX_PRT_EN(block_id)                       (0x80011800B0000008ull+(((block_id)&1)*0x8000000ull))
#define  CVMX_IPD_PORTX_BP_PAGE_CNT(offset)                  (0x80014F0000000028ull+(((offset)&63)*8))

/* system power check out*/
#define SYS_POWER_M1_BIT		0
#define SYS_POWER_M2_BIT		1
#define SYS_POWER_M3_BIT		2
#define SYS_POWER_ALL_BIT		3

#define SYS_POWER_M1_ALARM	( 1<<SYS_POWER_M1_BIT )
#define SYS_POWER_M2_ALARM	( 1<<SYS_POWER_M2_BIT )
#define SYS_POWER_M3_ALARM	( 1<<SYS_POWER_M3_BIT )
#define SYS_POWER_ALARM		( 1<<SYS_POWER_ALL_BIT)

#define SYS_POWER_M1_ISALARM(state) 	(state & SYS_POWER_M1_ALARM)	/* 1-normal 0-alarm*/
#define SYS_POWER_M2_ISALARM(state) 	(state & SYS_POWER_M2_ALARM)	/* 1-normal 0-alarm*/
#define SYS_POWER_M3_ISALARM(state) 	(state & SYS_POWER_M3_ALARM)	/* 1-normal 0-alarm*/
#define SYS_POWER_ISNORMAL(state)		(state & SYS_POWER_ALARM)		/* 1-normal 0-abnormal*/

/* CPU facility core temperature check out*/
#define  SYSTEM_CORE_TMPRT_ISALARM(value,current)              (current>=value)
#define	 SYSTEM_CORE_TMPRT_NOALARM(low,current)               (current<low)

/* product type code*/ 
#define AX86_CPLD_PRODUCT_TYPE_CODE	0x8
#define AX7K_CPLD_PRODUCT_TYPE_CODE	0x7
#define AX5K_CPLD_PRODUCT_TYPE_CODE	0x5
#define AU4K_CPLD_PRODUCT_TYPE_CODE 	0x4
#define AU3K_CPLD_PRODUCT_TYPE_CODE 	0x3

#define PRODUCT_TYPE_AX7000		0x0700
#define PRODUCT_TYPE_5612E		0x0561
#define PRODUCT_TYPE_5608		0x0562
#define PRODUCT_TYPE_5612I		0x0503

/* typde definition part*/
typedef union
{
    uint64_t u64;
    struct cvmx_gmxx_rxx_rx_inbnd_s
    {
        uint64_t reserved_4_63	: 60;
        uint64_t duplex       	: 1;       
        uint64_t speed        	: 2;       
        uint64_t status      	: 1;       
    } s;
} cvmx_gmxx_rxx_rx_inbnd_t;


typedef union
{
    uint64_t u64;
    struct cvmx_smi_cmd_s
    {

        uint64_t reserved_17_63  	: 47;
        uint64_t phy_op           	: 1;       
        uint64_t reserved_13_15	: 3;
        uint64_t phy_adr        	: 5;       
        uint64_t reserved_5_7   	: 3;
        uint64_t reg_adr         	: 5;       
    } s;
} cvmx_smi_cmd_t;

typedef union
{
    uint64_t u64;
    struct cvmx_gmxx_prtx_cfg_s
    {
        uint64_t reserved_14_63  	: 50;
        uint64_t tx_idle        	: 1;       
        uint64_t rx_idle         	: 1;       
        uint64_t reserved_9_11   	: 3;
        uint64_t speed_msb        	: 1;       
        uint64_t reserved_4_7    	: 4;
        uint64_t slottime        	: 1;       
        uint64_t duplex           	: 1;       
        uint64_t speed           	: 1;       
        uint64_t en               	: 1;       
    } s;
} cvmx_gmxx_prtx_cfg_t;

/**
 * cvmx_pow_wq_int_thr#
 *
 * POW_WQ_INT_THRX = POW Work Queue Interrupt Threshold Register (1 per group)
 * 
 * Contains the thresholds for enabling and setting work queue interrupts.  For more information
 * regarding this register, see the interrupt section.
 * 
 * Note: Up to 4 of the POW's internal storage buffers can be allocated for hardware use and are
 * therefore not available for incoming work queue entries.  Additionally, any PP that is not in the
 * NULL_NULL state consumes a buffer.  Thus in a 4 PP system, it is not advisable to set either
 * IQ_THR or DS_THR to greater than 512 - 4 - 4 = 504.  Doing so may prevent the interrupt from
 * ever triggering.
 */
typedef union
{
    uint64_t u64;
    struct cvmx_pow_wq_int_thrx_s
    {
        uint64_t reserved_29_63          : 35;
        uint64_t tc_en                   : 1;       /**< Time counter interrupt enable for group X
                                                         TC_EN must be zero when TC_THR==0 */
        uint64_t tc_thr                  : 4;       /**< Time counter interrupt threshold for group X
                                                         When TC_THR==0, POW_WQ_INT_CNT*[TC_CNT] is zero */
        uint64_t reserved_23_23          : 1;
        uint64_t ds_thr                  : 11;      /**< De-schedule count threshold for group X
                                                         DS_THR==0 disables the threshold interrupt */
        uint64_t reserved_11_11          : 1;
        uint64_t iq_thr                  : 11;      /**< Input queue count threshold for group X
                                                         IQ_THR==0 disables the threshold interrupt */
    } s;
  
} cvmx_pow_wq_int_thrx_t;




/**
 * cvmx_smi_rd_dat
 *
 * SMI_RD_DAT = SMI Read Data
 *
 *
 * Notes:
 * VAL will assert when the read xaction completes.  A read to this register
 * will clear VAL.  PENDING indicates that an SMI RD transaction is in flight.
 */
typedef union
{
    uint64_t u64;
    struct cvmx_smi_rd_dat_s
    {
        uint64_t reserved_18_63	:46;
        uint64_t pending			:1;       
        uint64_t val				:1;       
        uint64_t dat				:16;      
    } s;
} cvmx_smi_rd_dat_t;



/**
 * cvmx_smi_wr_dat
 *
 * SMI_WR_DAT = SMI Write Data
 *
 *
 * Notes:
 * VAL will assert when the write xaction completes.  A read to this register
 * will clear VAL.  PENDING indicates that an SMI WR transaction is in flight.
 */
typedef union
{
    uint64_t u64;
    struct cvmx_smi_wr_dat_s
    {
        uint64_t reserved_18_63          : 46;
        uint64_t pending                 : 1;       /**< Write Xaction Pending */
        uint64_t val                     : 1;       /**< Write Data Valid */
        uint64_t dat                     : 16;      /**< Write Data */
    } s;
} cvmx_smi_wr_dat_t;




/**
  * register for MTU.
  * add by baolc, 2008-06-30
  */
typedef union
{
	uint64_t u64;
	struct
	{
		uint64_t reserved_16_63 :48;
		uint64_t len            :16;
	} s;
} cvmx_gmxx_rxx_frm_max; 


/**
 * cvmx_gmx#_rx#_frm_ctl
 *
 * GMX_RX_FRM_CTL = Frame Control
 *
 *
 * Notes:
 * * PRE_CHK
 *   When set, the RX state expects a typical frame consisting of
 *   INTER_FRAME=>PREAMBLE(x7)=>SFD(x1)=>DAT.  The state machine watches for
 *   this exact sequence in order to recognize a valid frame and push frame
 *   data into the Octane.  There must be exactly 7 PREAMBLE cycles followed by
 *   the single SFD cycle for the frame to be accepted.
 * 
 *   When a problem does occur within the PREAMBLE seqeunce, the frame is
 *   marked as bad and not sent into the core.  The GMX_RX_INT_REG[PCTERR]
 *   interrupt is also raised.
 * 
 * * PRE_STRP
 *   When PRE_CFG is set (indicating that the PREAMBLE will be sent), PRE_STRP
 *   determines if the PREAMBLE+SFD bytes are thrown away or sent to the Octane
 *   core as part of the packet.
 * 
 *   In either mode, the PREAMBLE+SFD bytes are not counted toward the packet
 *   size when checking against the MIN and MAX bounds.  Furthermore, the bytes
 *   are skipped when locating the start of the L2 header for DMAC and Control
 *   frame recognition.
 * 
 * * CTL_BCK/CTL_DRP
 *   These bits control how the HW handles incoming PAUSE packets.  Here are
 *   the most common modes of operation:
 *     CTL_BCK=1,CTL_DRP=1   - HW does it all
 *     CTL_BCK=0,CTL_DRP=0   - SW sees all pause frames
 *     CTL_BCK=0,CTL_DRP=1   - all pause frames are completely ignored
 * 
 *   These control bits should be set to CTL_BCK=0,CTL_DRP=0 in halfdup mode.
 *   Since PAUSE packets only apply to fulldup operation, any PAUSE packet
 *   would constitute an exception which should be handled by the processing
 *   cores.  PAUSE packets should not be forwarded.
 */
typedef union
{
    uint64_t u64;
    struct cvmx_gmxx_rxx_frm_ctl_s
    {
        uint64_t reserved_10_63          : 54;
        uint64_t pre_align               : 1;       /**< When set, PREAMBLE parser aligns the the SFD byte
                                                         regardless of the number of previous PREAMBLE
                                                         nibbles.
                                                         (SGMII/1000Base-X only) */
        uint64_t pad_len                 : 1;       /**< When set, disables the length check for non-min
                                                         sized pkts with padding in the client data
                                                         (PASS3 Only) */
        uint64_t vlan_len                : 1;       /**< When set, disables the length check for VLAN pkts
                                                         (PASS2 only) */
        uint64_t pre_free                : 1;       /**< When set, PREAMBLE checking is  less strict.
                                                         0 - 254 cycles of PREAMBLE followed by SFD */
        uint64_t ctl_smac                : 1;       /**< Control Pause Frames can match station SMAC */
        uint64_t ctl_mcst                : 1;       /**< Control Pause Frames can match globally assign
                                                         Multicast address */
        uint64_t ctl_bck                 : 1;       /**< Forward pause information to TX block */
        uint64_t ctl_drp                 : 1;       /**< Drop Control Pause Frames */
        uint64_t pre_strp                : 1;       /**< Strip off the preamble (when present)
                                                         0=PREAMBLE+SFD is sent to core as part of frame
                                                         1=PREAMBLE+SFD is dropped */
        uint64_t pre_chk                 : 1;       /**< This port is configured to send PREAMBLE+SFD
                                                         to begin every frame.  GMX checks that the
                                                         PREAMBLE is sent correctly */
    } s;
} cvmx_gmxx_rxx_frm_ctl_t;



/**
 * cvmx_gmx#_tx_ovr_bp
 *
 * GMX_TX_OVR_BP = Packet Interface TX Override BackPressure
 *
 *
 * Notes:
 * In XAUI mode, only the lsb (corresponding to port0) of EN, BP, and IGN_FULL are used.
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_gmxx_tx_ovr_bp_s
    {
        uint64_t reserved_48_63          : 16;
        uint64_t tx_prt_bp               : 16;      /**< Per port BP sent to PKO
                                                         0=Port is available
                                                         1=Port should be back pressured */
        uint64_t reserved_12_31          : 20;
        uint64_t en                      : 4;       /**< Per port Enable back pressure override */
        uint64_t bp                      : 4;       /**< Per port BackPressure status to use
                                                         0=Port is available
                                                         1=Port should be back pressured */
        uint64_t ign_full                : 4;       /**< Ignore the RX FIFO full when computing BP */
    } s;
} cvmx_gmxx_tx_ovr_bp_t;



/**
 * cvmx_ipd_port#_bp_page_cnt
 *
 * IPD_PORTX_BP_PAGE_CNT = IPD Port Backpressure Page Count
 * 
 * The number of pages in use by the port that when exceeded, backpressure will be applied to the port.
 */
typedef union
{
    uint64_t u64;
    struct cvmx_ipd_portx_bp_page_cnt_s
    {
        uint64_t reserved_18_63          : 46;
        uint64_t bp_enb                  : 1;       /**< When set '1' BP will be applied, if '0' BP will
                                                         not be applied to port. */
        uint64_t page_cnt                : 17;      /**< The number of page pointers assigned to
                                                         the port, that when exceeded will cause
                                                         back-pressure to be applied to the port.
                                                         This value is in 256 page-pointer increments,
                                                         (i.e. 0 = 0-page-ptrs, 1 = 256-page-ptrs,..) */
    } s;
} cvmx_ipd_portx_bp_page_cnt_t;




/**
 * cvmx_asx#_tx_prt_en
 *
 * ASX_TX_PRT_EN = RGMII Port Enable
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_asxx_tx_prt_en_s
    {
        uint64_t reserved_4_63           : 60;
        uint64_t prt_en                  : 4;       /**< Port enable.  Must be set for Octane to send
                                                         RMGII traffic.   When this bit clear on a given
                                                         port, then all RGMII cycles will appear as
                                                         inter-frame cycles. */
    } s;
} cvmx_asxx_tx_prt_en_t;



/**
 * cvmx_asx#_rx_prt_en
 *
 * ASX_RX_PRT_EN = RGMII Port Enable
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_asxx_rx_prt_en_s
    {
        uint64_t reserved_4_63           : 60;
        uint64_t prt_en                  : 4;       /**< Port enable.  Must be set for Octane to receive
                                                         RMGII traffic.  When this bit clear on a given
                                                         port, then the all RGMII cycles will appear as
                                                         inter-frame cycles. */
    } s;
} cvmx_asxx_rx_prt_en_t;




/********** data types and macros for mdio read/write PHY ************/
#define  CVMX_SMI_CMD                                         0x8001180000001800ull

#define  CVMX_SMI_RD_DAT                                      0x8001180000001810ull
#define  CVMX_SMI_WR_DAT                                      (0x8001180000001808ull)

typedef union
{
	uint16_t  u16;
	struct
	{
		uint16_t reserved_12_15        :4;
		uint16_t preferred_media       :2; /* the fileds we cared about */
		uint16_t other_fields_0_9      :10;
	} s;
} cvmx_mdio_phy1145_reg26; /* 16 bit reg */

/*ctl register of copper*/
typedef union
{
	uint16_t  u16;
	struct
	{
		uint16_t reserved_14_15        :2;
		uint16_t linkup_media       :1; /* the fileds we cared about */
		uint16_t other_fields_0_12      :13;
	} s;
} cvmx_mdio_phy1145_reg27; /* 16 bit reg */

/*ctl register of copper*/
typedef union
{
	uint16_t  u16;
	struct
	{
		uint16_t reset					:1;
		uint16_t loop_back		      	:1; 
		uint16_t speedselect_bit13     	:1;
		uint16_t auto_negotiaton_en		:1;
		uint16_t power_down			:1;
		uint16_t isolate				:1;
		uint16_t restart_copper_auto	:1;
		uint16_t copper_duplex_mod		:1;
		uint16_t collision_test			:1;
		uint16_t speedselect_bit6		:1;
		uint16_t reservered				:6;
		
	} s;
} cvmx_mdio_phy1145_reg0_0; /* 16 bit reg */

/*ctl register of fiber*/
typedef union
{
	uint16_t  u16;
	struct
	{
		uint16_t reset					:1;
		uint16_t loop_back		      	:1; 
		uint16_t speedselect_bit13     	:1;
		uint16_t auto_negotiaton_en		:1;
		uint16_t power_down			:1;
		uint16_t isolate				:1;
		uint16_t restart_copper_auto	:1;
		uint16_t fiber_duplex_mod		:1;
		uint16_t collision_test			:1;
		uint16_t speedselect_bit6		:1;
		uint16_t reservered				:6;
		
	} s;
} cvmx_mdio_phy1145_reg0_1; /* 16 bit reg */


/*
 * Wang Jiankun added the union for PHY1145 Register 17
 *
 * Now using the Register 17 bit 10 for link state, rather than
 * the CPU GMX0/1_RX(0~3)_RX_INBND[STATUS]
 */
typedef union {
	uint16_t u16;
	struct phy_reg_17_t {
		uint16_t speed			:2;
		uint16_t duplex 		:1;
		uint16_t page_received	:1;
		uint16_t spe_dup_res	:1;
		uint16_t link			:1;
		uint16_t cable_length	:3;
		uint16_t mdi_crossover	:1;
		uint16_t downshift		:1;
		uint16_t energy_detect	:1;
		uint16_t transmit_pause :1;
		uint16_t receive_pause	:1;
		uint16_t polarity		:1; 	  
		uint16_t jabber 		:1; 		
	} s;
} phy_reg_17_dat;	
#if 0
/*status register of copper*/
typedef union
{
	uint16_t  u16;
	struct
	{
		uint16_t 100base_T4				:3;
		uint16_t 10Mbps_full			:1;
		uint16_t 10Mbps_half			:1;
		uint16_t 100Mbps_full			:1;
		uint16_t 100Mbps_half			:1;
		uint16_t no_care				:3;
		uint16_t copper_AN_complet		:1;
		uint16_t speedselect_bit6		:1;
		uint16_t copper_AN_ability		:1;
		uint16_t copper_link_sta		:1;
		
		
	} s;
} cvmx_mdio_phy1145_reg1_0; /* 16 bit reg */

/*status register of fiber*/
typedef union
{
	uint16_t  u16;
	struct
	{
		uint16_t reset					:1;
		uint16_t loop_back		      	:1; 
		uint16_t speedselect_bit13     	:1;
		uint16_t auto_negotiaton_en		:1;
		uint16_t powver_down			:1;
		uint16_t isolate				:1;
		uint16_t restart_copper_auto	:1;
		uint16_t copper_duplex_mod		:1;
		uint16_t collision_test			:1;
		uint16_t speedselect_bit6		:1;
		uint16_t reservered				:6;
		
	} s;
} cvmx_mdio_phy1145_reg1_1; /* 16 bit reg */

#endif
/************* data types for ioctl **************/
typedef struct bm_op_args_s {
	unsigned long long op_addr;
	unsigned long long op_value;  /* ignore on read in arg, fill read value on write return value*/
	unsigned short op_byteorder; /* destination byte order. default is bigendiana.*/
	unsigned short op_len;
	unsigned short op_ret; /* 0 for success, other value indicate different failure.*/
} bm_op_args;

typedef struct bm_op_cpld_read {
	unsigned char 	slot;		/*which slot's CPLD to read from*/
	unsigned int	regAddr;	/*CPLD register address*/
	unsigned char	regData;	/*CPLD register value*/
}bm_op_cpld_args;

typedef struct bm_op_bit64_read {
	unsigned long long 	regAddr;	/*64-bit register address*/
	unsigned long long	regData;	/*64-bit register data*/
}bm_op_rbit64_args;

typedef struct bm_op_sysinfo_common {
	unsigned char mac_addr[MAC_ADDRESS_LEN]; /* system mac address*/
	unsigned char sn[SYSINFO_SN_LENGTH];/* module or backplane or mainboard serial number*/
}bm_op_sysinfo_args;

typedef struct bm_op_module_sysinfo {
	unsigned char slotno;
	bm_op_sysinfo_args common;
}bm_op_module_sysinfo;

typedef bm_op_sysinfo_args bm_op_backplane_sysinfo;
typedef bm_op_sysinfo_args bm_op_mainboard_sysinfo;

/** 
  * Product sysinfo, the data comes from backplane-board.
  * codeby: baolc
  */
typedef struct ax_sysinfo_product_t
{
	char  ax_sysinfo_module_serial_no[21]; /*data should be 20 bytes */
	char  ax_sysinfo_module_name[25];  /*data max length should be 24 bytes */
	char  ax_sysinfo_product_serial_no[21]; /*data should be 20 bytes */
	char  ax_sysinfo_product_base_mac_address[13]; /*data should be 12 bytes */
	char  ax_sysinfo_product_name[25]; /*data max length should be 24 bytes */
	char  ax_sysinfo_software_name[25]; /*data max length should be 24 bytes */
	char  ax_sysinfo_enterprise_name[65]; /*data max length should be 64 bytes */
	char  ax_sysinfo_enterprise_snmp_oid[129]; /*data max length should be 128 bytes */
	char  ax_sysinfo_snmp_sys_oid[129]; /*data max length should be 128 bytes */
	char  ax_sysinfo_built_in_admin_username[33]; /*data max length should be 32 bytes */
	char  ax_sysinfo_built_in_admin_password[33]; /*data max length should be 32 bytes */ 
}ax_sysinfo_product_t;

/*typedef ax_sysinfo_product_t ax_backplane_sysinfo;*/
typedef ax_sysinfo_product_t ax_product_sysinfo;

typedef struct ax_read_module_sysinfo
{
	int product_id;					/*the product's id must be 3~7 for 3000~7000*/
	int slot_num;							/*0~4 */
	char  ax_sysinfo_module_serial_no[21]; /*data should be 21 bytes */
	char  ax_sysinfo_module_name[25];  /*data max length should be 24 bytes */
}ax_module_sysinfo;

/**
  * Board info, the data comes from mainboard or child-board.
  * code by baolc
  */
typedef struct ax_sysinfo_single_board_t
{
	char  ax_sysinfo_module_serial_no[21]; /*data should be 21 bytes */
	char  ax_sysinfo_module_name[25];  /*data max length should be 24 bytes */
}ax_sysinfo_single_board_t;


typedef enum ax7_board_state_e {
	AX7_BOARD_STATE_ACTIVE = 0,
	AX7_BOARD_STATE_INACTIVE
}AX7_BOARD_STATE_E;


/*************** param for read eth_port_stats *****************/

typedef struct bm_op_read_eth_port_stats_args
{
	int portNum;
	int clear; /* 1 for clear stats CSRs */
	struct npd_port_counter port_counter;
	struct port_oct_s		port_oct_count;
} bm_op_read_eth_port_stats_args;


#define BM_MINOR_NUM 0
#define BM_MAJOR_NUM 0xEC

#define BM_IOC_MAGIC 0xEC 
#define BM_IOC_RESET	_IO(BM_IOC_MAGIC,0)

#ifdef DRV_LIB_CPSS_3_4
/* add for read/write eeprom register */
#define MAX_CHARACTER_NUM 256
/* The TWSI interface supports both 7-bit and 10-bit addressing.            */
/* This enumerator describes addressing type.                               */
typedef enum _TwsiAddrType
{
    ADDR7_BIT,                      /* 7 bit address    */
    ADDR10_BIT                      /* 10 bit address   */
}TWSI_ADDR_TYPE;

/* This structure describes TWSI address.                                   */
typedef struct _mvTwsiAddr
{
    unsigned int     address;    /* address          */
    TWSI_ADDR_TYPE   type;       /* Address type     */
}MV_TWSI_ADDR;

typedef struct _mvTwsiSlave
{
    MV_TWSI_ADDR	slaveAddr;
    unsigned int	validOffset;		/* whether the slave has offset (i.e. Eeprom  etc.) 	*/
    unsigned int    offset;				/* offset in the slave.					*/
    unsigned int	moreThen256;		/* whether the ofset is bigger then 256 		*/
}MV_TWSI_SLAVE;

/* This structure describes a TWSI slave.  */
typedef struct twsi_op
{
	MV_TWSI_SLAVE twsi_slave;
	unsigned char	reg_val[MAX_CHARACTER_NUM];
	unsigned char	chanNum;
    unsigned int	reg_val_size;
}twsi_op_t;

#define BM_IOC_TWSI_READ8_			_IOWR(BM_IOC_MAGIC, 15, twsi_op_t)	/*hard read from eeprom 8bit  internal address*/
#define BM_IOC_TWSI_WRITE8_			_IOWR(BM_IOC_MAGIC, 16, twsi_op_t)	/*hard write to eerprom 8bit internal address*/
#define BM_IOC_TWSI_READ16_			_IOWR(BM_IOC_MAGIC, 17, twsi_op_t)  /*hard read from eeprom 16bit internal address*/
#define BM_IOC_TWSI_WRITE16_			_IOWR(BM_IOC_MAGIC, 18, twsi_op_t)	/*hard write to eeprom 16bit internal address*/
/*  add by jinpengcheng for temp change */
#define BM_IOC_TWSI_OPEN_LASER		_IO(BM_IOC_MAGIC, 19)
#endif

/*
 * S means "Set" through a ptr,
 * T means "Tell" directly with the argument value
 * G means "Get": reply by setting through a pointer
 * Q means "Query": response is on the return value
 * X means "eXchange": switch G and S atomically
 * H means "sHift": switch T and Q atomically
 */

#define BM_IOC_G_  			_IOWR(BM_IOC_MAGIC,1,bm_op_args) /* read values*/
#define BM_IOC_X_ 				_IOWR(BM_IOC_MAGIC,2,bm_op_args) /* write and readout wrote value*/
#define BM_IOC_CPLD_READ		_IOWR(BM_IOC_MAGIC,3,bm_op_cpld_args) /*read cpld registers*/
#define BM_IOC_BIT64_REG_STATE 			_IOWR(BM_IOC_MAGIC,4,bm_op_rbit64_args) /*read 64-bit status register*/
#define BM_IOC_CPLD_WRITE   				_IOWR(BM_IOC_MAGIC,5,bm_op_cpld_args) /*write cpld registers by zhang*/
/*#define BM_IOC_MAINBOARD_SYSINFO_READ	_IOWR(BM_IOC_MAGIC, 6, bm_op_mainboard_sysinfo) read main board sysinfo*/
/*#define BM_IOC_BACKPLANE_SYSINFO_READ	BM_IOC_MAINBOARD_SYSINFO_READ  read back plane sysinfo*/
#define BM_IOC_MODULE_SYSINFO_READ		_IOWR(BM_IOC_MAGIC, 7, bm_op_module_sysinfo) /* read chassis module sysinfo*/
#define BM_IOC_SYSINFO_EEPROM_READ		_IOWR(BM_IOC_MAGIC, 8, bm_op_sysinfo_eeprom_args) /*read sysinfo from eeprom*/
#define BM_IOC_READ_PRODUCT_SYSINFO  _IOWR(BM_IOC_MAGIC, 15, ax_product_sysinfo)/*read backplane or mainboard sysinfo from eeprom*/
#define BM_IOC_BACKPLANE_SYSINFO_READ	BM_IOC_READ_PRODUCT_SYSINFO	/* read back plane sysinfo*/
#define BM_IOC_MAINBOARD_SYSINFO_READ  BM_IOC_READ_PRODUCT_SYSINFO  /*read main board sysinfo*/
#define BM_IOC_READ_MODULE_SYSINFO _IOWR(BM_IOC_MAGIC, 6, ax_module_sysinfo)/*read module sysinfo for series 7 or series 6*/
#define BM_IOC_BOARD_ETH_PORT_STATS_READ  _IOWR(BM_IOC_MAGIC, 9, bm_op_read_eth_port_stats_args) /*read board eth-port statistics*/
#define BM_IOC_ENV_EXCH  		_IOWR(BM_IOC_MAGIC, 10,boot_env_t) /*read board eth-port statistics*/
#define BM_IOC_SOFT_SYSINFO_READ_16BIT _IOWR(BM_IOC_MAGIC, 10, ax_soft_sysinfo)/*read sysinfo from sub-card through soft i2c(16bit)*/

#define BM_IOC_SOFT_I2C_READ8_		_IOWR(BM_IOC_MAGIC, 11, soft_i2c_read_write_8bit)  /*read data from eeprom*/
#define BM_IOC_SOFT_I2C_WRITE8_		_IOWR(BM_IOC_MAGIC, 12, soft_i2c_read_write_8bit)  /*write data to eeprom*/
#define BM_IOC_SOFT_I2C_READ16_		_IOWR(BM_IOC_MAGIC, 13, soft_i2c_read_write_16bit)  /*read data from sub card eeprom */
#define BM_IOC_SOFT_I2C_WRITE16_	_IOWR(BM_IOC_MAGIC, 14, soft_i2c_read_write_16bit)  /*write data to sub card eeprom */
#define BM_IOC_MAXNR 16


#define  SYSTEM_CORE_HIGH_EXTREMUM                    105
#define  SYSTEM_CORE_HIGH_MAXNUM                      100
#define  SYSTEM_CORE_HIGH_CRITICAL                    95
#define  SYSTEM_CORE_LOW_CRITICAL                     90
#define  SYSTEM_SURFACE_HIGH_CRITICAL                 90
#define  SYSTEM_SURFACE_LOW_CRITICAL                  85

#define NBM_TRUE   1
#define NBM_FALSE   0
/** Logging levels for NPD daemon
 */
enum {
	NBM_LOGPRI_TRACE = (1 << 0),   /**< function call sequences */
	NBM_LOGPRI_DEBUG = (1 << 1),   /**< debug statements in code */
	NBM_LOGPRI_INFO = (1 << 2),    /**< informational level */
	NBM_LOGPRI_WARNING = (1 << 3), /**< warnings */
	NBM_LOGPRI_ERROR = (1 << 4)    /**< error */
};

extern int g_bm_fd;

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

/** Trace logging macro */
#define NBM_TRACE(expr)   do {logger_setup(NBM_LOGPRI_TRACE,   __FILE__, __LINE__, __FUNCTION__); logger_emit expr; } while(0)

/** Debug information logging macro */
#define NBM_DEBUG(expr)   do {logger_setup(NBM_LOGPRI_DEBUG,   __FILE__, __LINE__, __FUNCTION__); logger_emit expr; } while(0)

/** Information level logging macro */
#define NBM_INFO(expr)    do {logger_setup(NBM_LOGPRI_INFO,    __FILE__, __LINE__, __FUNCTION__); logger_emit expr; } while(0)

/** Warning level logging macro */
#define NBM_WARNING(expr) do {logger_setup(NBM_LOGPRI_WARNING, __FILE__, __LINE__, __FUNCTION__); logger_emit expr; } while(0)

/** Error leve logging macro */
#define NBM_ERROR(expr)   do {logger_setup(NBM_LOGPRI_ERROR,   __FILE__, __LINE__, __FUNCTION__); logger_emit expr; } while(0)

/** Macro for terminating the program on an unrecoverable error */
#define NBM_DIE(expr) do {printf("*** [DIE] %s:%s():%d : ", __FILE__, __FUNCTION__, __LINE__); printf expr; printf("\n"); exit(1); } while(0)

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
int nbm_init_cpld
(
	void
);

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
int nbm_query_mainboard_id
(
	void
);

int nbm_query_backplane_id
(
	void
);

unsigned char nbm_query_mainboard_version
(
	void
);

/**********************************************************************************
 * npd_init_one_chassis_slot
 *
 * init chassis slot global info structure,dedicated slot given by chssis_slot_index
 *
 *	INPUT:
 *		chassis_slot_index - chassis slot index to initialize
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_init_one_chassis_slot
(
	int chassis_slot_index
);

/**********************************************************************************
 * npd_chassis_slot_hotplugin
 *
 * npd callback functions called when chassis slot is hot plugged in,which reported by board check thread.
 *
 *	INPUT:
 *		chassis_slot_index - chassis slot index(or slot number)
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_chassis_slot_hot_plugin
(
	int chassis_slot_index
);

/**********************************************************************************
 * npd_chassis_slot_hot_pullout
 *
 * npd callback functions called when chassis slot is hot pulled out,which reported by board check thread.
 *
 *	INPUT:
 *		chassis_slot_index - chassis slot index(or slot number)
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_chassis_slot_hot_pullout
(
	int chassis_slot_index
);

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

/**********************************************************************************
 * npd_product_hardware_watchdog_function_check
 *
 * 	Check whether the product support hardware watch dog or not.
 *	Currently, all chassis and box product with CPLD supported has hardware watchdog funcationality.
 *  That means asic chip with XCAT series has no hardware watchdog functionality.
 *
 *	INPUT:
 *		productId - product id
 *	
 *	OUTPUT:
 *		NULL
 *
 *	RETURN:
 *		0 - no hardware watchdog on this product
 *		1 - hardware watchdog is deployed on this product
 *
 **********************************************************************************/
extern int npd_product_hardware_watchdog_function_check
(
	void
);

int nbm_probe_chassis_module_id
(
	int slotno
);

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
);

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
);

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
);

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
);

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
);

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
int nbm_cpld_reg_write
(
	int addr,
	unsigned char value
);

/**********************************************************************************
 * nbm_cpld_reg_read
 *
 * General purpose API to read a CPLD register
 *
 *	INPUT:
 *		addr	- CPLD register address
 *	
 *	OUTPUT:
 *		value	- CPLD register value
 *
 *	RETURN:
 *		0 - if no error occur.
 *		NPD_FAIL - if error occurred.
 *
 **********************************************************************************/
int nbm_cpld_reg_read
(
	int reg,
	unsigned char *value
);
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
);

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
);

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
);

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
);

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
);
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
);

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
);

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
);
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
);

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
void nbm_reset_led_mutex_cpld
(
	void
);

#endif

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
);

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
);

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
);
#endif

#endif
