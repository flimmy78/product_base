#ifndef _SEM_ETH_PORT_H
#define _SEM_ETH_PORT_H

#define ETH_PORT_NAME_LEN_MAX	20
#define SEM_IFNAMSIZ	16

#define uint64_t unsigned long long
#define uint16_t unsigned short

/*
 * Define 88E1145 Register name
 */
#define MV_88E1145_PHY_SPEC_STATUS			0x11
#define MV_88E1145_PHY_EXT_ADDR 			0x16
#define MV_88E1145_PHY_EXT_SPEC_CNTRL2		0x1A
#define MV_88E1145_PHY_EXT_SPEC_STATUS		0x1B

#define  CVMX_GMXX_RXX_FRM_CTL(offset,block_id)              (0x8001180008000018ull+(((offset)&3)*2048)+(((block_id)&1)*0x8000000ull))
#define  CVMX_GMXX_TX_OVR_BP(block_id)                       (0x80011800080004C8ull+(((block_id)&1)*0x8000000ull))
#define  CVMX_IPD_PORTX_BP_PAGE_CNT(offset)                  (0x80014F0000000028ull+(((offset)&63)*8))



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


#define SEM_ETHPORT_SHOWRUN_CFG_SIZE	(3*1024) /* for all 24GE ports configuration */


#endif

