/**
  * Ioctl cmds for read board eth port statistics.
  * author: Autelan. Tech.
  * codeby: baolc
  * 2008-07-08
  */

#ifndef _BMK_ETH_PORT_STATS_H_
#define _BMK_ETH_PORT_STATS_H_

#if 0
/***************************************************************************/
/* CSRs for statics */

/************** CSRs addr for GMX RX STATS ***************/
#define  CVMX_GMXX_RXX_STATS_CTL(offset,block_id) 		   ((0x8001180008000050ull+(((offset)&3)*2048)+(((block_id)&1)*0x8000000ull)))
#define  CVMX_GMXX_RXX_STATS_OCTS(offset,block_id)		   ((0x8001180008000088ull+(((offset)&3)*2048)+(((block_id)&1)*0x8000000ull)))
#define  CVMX_GMXX_RXX_STATS_OCTS_CTL(offset,block_id)	   ((0x8001180008000098ull+(((offset)&3)*2048)+(((block_id)&1)*0x8000000ull)))
#define  CVMX_GMXX_RXX_STATS_OCTS_DMAC(offset,block_id)	   ((0x80011800080000A8ull+(((offset)&3)*2048)+(((block_id)&1)*0x8000000ull)))
#define  CVMX_GMXX_RXX_STATS_OCTS_DRP(offset,block_id)	   ((0x80011800080000B8ull+(((offset)&3)*2048)+(((block_id)&1)*0x8000000ull)))
#define  CVMX_GMXX_RXX_STATS_PKTS(offset,block_id)		   ((0x8001180008000080ull+(((offset)&3)*2048)+(((block_id)&1)*0x8000000ull)))
#define  CVMX_GMXX_RXX_STATS_PKTS_BAD(offset,block_id)	   ((0x80011800080000C0ull+(((offset)&3)*2048)+(((block_id)&1)*0x8000000ull)))
#define  CVMX_GMXX_RXX_STATS_PKTS_CTL(offset,block_id)	   ((0x8001180008000090ull+(((offset)&3)*2048)+(((block_id)&1)*0x8000000ull)))
#define  CVMX_GMXX_RXX_STATS_PKTS_DMAC(offset,block_id)	   ((0x80011800080000A0ull+(((offset)&3)*2048)+(((block_id)&1)*0x8000000ull)))
#define  CVMX_GMXX_RXX_STATS_PKTS_DRP(offset,block_id)	   ((0x80011800080000B0ull+(((offset)&3)*2048)+(((block_id)&1)*0x8000000ull)))


/************** CSRs addr for GMX TX STATS ***************/
#define  CVMX_GMXX_TXX_STAT0(offset,block_id) 			   ((0x8001180008000280ull+(((offset)&3)*2048)+(((block_id)&1)*0x8000000ull)))
#define  CVMX_GMXX_TXX_STAT1(offset,block_id) 			   ((0x8001180008000288ull+(((offset)&3)*2048)+(((block_id)&1)*0x8000000ull)))
#define  CVMX_GMXX_TXX_STAT2(offset,block_id) 			   ((0x8001180008000290ull+(((offset)&3)*2048)+(((block_id)&1)*0x8000000ull)))
#define  CVMX_GMXX_TXX_STAT3(offset,block_id) 			   ((0x8001180008000298ull+(((offset)&3)*2048)+(((block_id)&1)*0x8000000ull)))
#define  CVMX_GMXX_TXX_STAT4(offset,block_id) 			   ((0x80011800080002A0ull+(((offset)&3)*2048)+(((block_id)&1)*0x8000000ull)))
#define  CVMX_GMXX_TXX_STAT5(offset,block_id) 			   ((0x80011800080002A8ull+(((offset)&3)*2048)+(((block_id)&1)*0x8000000ull)))
#define  CVMX_GMXX_TXX_STAT6(offset,block_id) 			   ((0x80011800080002B0ull+(((offset)&3)*2048)+(((block_id)&1)*0x8000000ull)))
#define  CVMX_GMXX_TXX_STAT7(offset,block_id) 			   ((0x80011800080002B8ull+(((offset)&3)*2048)+(((block_id)&1)*0x8000000ull)))
#define  CVMX_GMXX_TXX_STAT8(offset,block_id) 			   ((0x80011800080002C0ull+(((offset)&3)*2048)+(((block_id)&1)*0x8000000ull)))
#define  CVMX_GMXX_TXX_STAT9(offset,block_id) 			   ((0x80011800080002C8ull+(((offset)&3)*2048)+(((block_id)&1)*0x8000000ull)))
#define  CVMX_GMXX_TXX_STATS_CTL(offset,block_id) 		   ((0x8001180008000268ull+(((offset)&3)*2048)+(((block_id)&1)*0x8000000ull)))


/************** CSRs addr for PIP STATS ***************/
#define  CVMX_PIP_STAT0_PRTX(offset)                         ((0x80011800A0000800ull+(((offset)&63)*80)))
#define  CVMX_PIP_STAT1_PRTX(offset)                         ((0x80011800A0000808ull+(((offset)&63)*80)))
#define  CVMX_PIP_STAT2_PRTX(offset)                         ((0x80011800A0000810ull+(((offset)&63)*80)))
#define  CVMX_PIP_STAT3_PRTX(offset)                         ((0x80011800A0000818ull+(((offset)&63)*80)))
#define  CVMX_PIP_STAT4_PRTX(offset)                         ((0x80011800A0000820ull+(((offset)&63)*80)))
#define  CVMX_PIP_STAT5_PRTX(offset)                         ((0x80011800A0000828ull+(((offset)&63)*80)))
#define  CVMX_PIP_STAT6_PRTX(offset)                         ((0x80011800A0000830ull+(((offset)&63)*80)))
#define  CVMX_PIP_STAT7_PRTX(offset)                         ((0x80011800A0000838ull+(((offset)&63)*80)))
#define  CVMX_PIP_STAT8_PRTX(offset)                         ((0x80011800A0000840ull+(((offset)&63)*80)))
#define  CVMX_PIP_STAT9_PRTX(offset)                         ((0x80011800A0000848ull+(((offset)&63)*80)))
#define  CVMX_PIP_STAT_CTL                                    (0x80011800A0000018ull)
#define  CVMX_PIP_STAT_INB_ERRSX(offset)                     ((0x80011800A0001A10ull+(((offset)&63)*32)))
#define  CVMX_PIP_STAT_INB_OCTSX(offset)                     ((0x80011800A0001A08ull+(((offset)&63)*32)))
#define  CVMX_PIP_STAT_INB_PKTSX(offset)                     ((0x80011800A0001A00ull+(((offset)&63)*32)))




/***************************************************************************/
/* CSRs' datatype def of pkt statics */

/************** RX statics **************/
typedef union
{
    uint64_t u64;
    struct cvmx_gmxx_rxx_stats_ctl_s
    {
        uint64_t reserved_1_63           : 63;
        uint64_t rd_clr                  : 1;       /**< RX Stats registers will clear on reads */
    } s;
} cvmx_gmxx_rxx_stats_ctl_t;

typedef union
{
    uint64_t u64;
    struct cvmx_gmxx_rxx_stats_octs_ctl_s
    {
        uint64_t reserved_48_63          : 16;
        uint64_t cnt                     : 48;      /**< Octet count of received pause packets */
    } s;
} cvmx_gmxx_rxx_stats_octs_ctl_t;

typedef union
{
    uint64_t u64;
    struct cvmx_gmxx_rxx_stats_octs_s
    {
        uint64_t reserved_48_63          : 16;
        uint64_t cnt                     : 48;      /**< Octet count of received good packets */
    } s;
} cvmx_gmxx_rxx_stats_octs_t;

typedef union
{
    uint64_t u64;
    struct cvmx_gmxx_rxx_stats_octs_dmac_s
    {
        uint64_t reserved_48_63          : 16;
        uint64_t cnt                     : 48;      /**< Octet count of filtered dmac packets */
    } s;
} cvmx_gmxx_rxx_stats_octs_dmac_t;

typedef union
{
    uint64_t u64;
    struct cvmx_gmxx_rxx_stats_octs_drp_s
    {
        uint64_t reserved_48_63          : 16;
        uint64_t cnt                     : 48;      /**< Octet count of dropped packets */
    } s;
} cvmx_gmxx_rxx_stats_octs_drp_t;

typedef union
{
    uint64_t u64;
    struct cvmx_gmxx_rxx_stats_pkts_s
    {
        uint64_t reserved_32_63          : 32;
        uint64_t cnt                     : 32;      /**< Count of received good packets */
    } s;
} cvmx_gmxx_rxx_stats_pkts_t;

typedef union
{
    uint64_t u64;
    struct cvmx_gmxx_rxx_stats_pkts_bad_s
    {
        uint64_t reserved_32_63          : 32;
        uint64_t cnt                     : 32;      /**< Count of bad packets */
    } s;
} cvmx_gmxx_rxx_stats_pkts_bad_t;

typedef union
{
    uint64_t u64;
    struct cvmx_gmxx_rxx_stats_pkts_ctl_s
    {
        uint64_t reserved_32_63          : 32;
        uint64_t cnt                     : 32;      /**< Count of received pause packets */
    } s;
} cvmx_gmxx_rxx_stats_pkts_ctl_t;

typedef union
{
    uint64_t u64;
    struct cvmx_gmxx_rxx_stats_pkts_dmac_s
    {
        uint64_t reserved_32_63          : 32;
        uint64_t cnt                     : 32;      /**< Count of filtered dmac packets */
    } s;
} cvmx_gmxx_rxx_stats_pkts_dmac_t;

typedef union
{
    uint64_t u64;
    struct cvmx_gmxx_rxx_stats_pkts_drp_s
    {
        uint64_t reserved_32_63          : 32;
        uint64_t cnt                     : 32;      /**< Count of dropped packets */
    } s;
} cvmx_gmxx_rxx_stats_pkts_drp_t;



/*********************** TX statics **********************/
/**
 * cvmx_gmx#_tx#_stat0
 *
 * GMX_TX_STAT0 = GMX_TX_STATS_XSDEF / GMX_TX_STATS_XSCOL
 *
 *
 * Notes:
 * - Read and write CSR operations must arbitrate for the STATS resources with the packet receiver
 * - Cleared either by a write (of any value) or a read when GMX_TX_STATS_CTL[RD_CLR] is set
 * - Counters will wrap
 */
typedef union
{
    uint64_t u64;
    struct cvmx_gmxx_txx_stat0_s
    {
        uint64_t xsdef                   : 32;      /**< Number of packets dropped (never successfully
                                                         sent) due to excessive deferal */
        uint64_t xscol                   : 32;      /**< Number of packets dropped (never successfully
                                                         sent) due to excessive collision.  Defined by
                                                         GMX_TX_COL_ATTEMPT[LIMIT]. */
    } s;
} cvmx_gmxx_txx_stat0_t;


/**
 * cvmx_gmx#_tx#_stat1
 *
 * GMX_TX_STAT1 = GMX_TX_STATS_SCOL  / GMX_TX_STATS_MCOL
 *
 *
 * Notes:
 * - Read and write CSR operations must arbitrate for the STATS resources with the packet receiver
 * - Cleared either by a write (of any value) or a read when GMX_TX_STATS_CTL[RD_CLR] is set
 * - Counters will wrap
 */
typedef union
{
    uint64_t u64;
    struct cvmx_gmxx_txx_stat1_s
    {
        uint64_t scol                    : 32;      /**< Number of packets sent with a single collision */
        uint64_t mcol                    : 32;      /**< Number of packets sent with multiple collisions
                                                         but < GMX_TX_COL_ATTEMPT[LIMIT]. */
    } s;
} cvmx_gmxx_txx_stat1_t;


/**
 * cvmx_gmx#_tx#_stat2
 *
 * GMX_TX_STAT2 = GMX_TX_STATS_OCTS
 *
 *
 * Notes:
 * - Octect counts are the sum of all data transmitted on the wire including
 *   packet data, pad bytes, fcs bytes, pause bytes, and jam bytes.  The octect
 *   counts do not include PREAMBLE byte or EXTEND cycles.
 * - Read and write CSR operations must arbitrate for the STATS resources with the packet receiver
 * - Cleared either by a write (of any value) or a read when GMX_TX_STATS_CTL[RD_CLR] is set
 * - Counters will wrap
 */
typedef union
{
    uint64_t u64;
    struct cvmx_gmxx_txx_stat2_s
    {
        uint64_t reserved_48_63          : 16;
        uint64_t octs                    : 48;      /**< Number of total octets sent on the interface.
                                                         Does not count octets from frames that were
                                                         truncated due to collisions in halfdup mode. */
    } s;
} cvmx_gmxx_txx_stat2_t;


/**
 * cvmx_gmx#_tx#_stat3
 *
 * GMX_TX_STAT3 = GMX_TX_STATS_PKTS
 *
 *
 * Notes:
 * - Read and write CSR operations must arbitrate for the STATS resources with the packet receiver
 * - Cleared either by a write (of any value) or a read when GMX_TX_STATS_CTL[RD_CLR] is set
 * - Counters will wrap
 */
typedef union
{
    uint64_t u64;
    struct cvmx_gmxx_txx_stat3_s
    {
        uint64_t reserved_32_63          : 32;
        uint64_t pkts                    : 32;      /**< Number of total frames sent on the interface.
                                                         Does not count frames that were truncated due to
                                                          collisions in halfdup mode. */
    } s;
} cvmx_gmxx_txx_stat3_t;


/**
 * cvmx_gmx#_tx#_stat4
 *
 * GMX_TX_STAT4 = GMX_TX_STATS_HIST1 (64) / GMX_TX_STATS_HIST0 (<64)
 *
 *
 * Notes:
 * - Packet length is the sum of all data transmitted on the wire for the given
 *   packet including packet data, pad bytes, fcs bytes, pause bytes, and jam
 *   bytes.  The octect counts do not include PREAMBLE byte or EXTEND cycles.
 * - Read and write CSR operations must arbitrate for the STATS resources with the packet receiver
 * - Cleared either by a write (of any value) or a read when GMX_TX_STATS_CTL[RD_CLR] is set
 * - Counters will wrap
 */
typedef union
{
    uint64_t u64;
    struct cvmx_gmxx_txx_stat4_s
    {
        uint64_t hist1                   : 32;      /**< Number of packets sent with an octet count of 64. */
        uint64_t hist0                   : 32;      /**< Number of packets sent with an octet count
                                                         of < 64. */
    } s;
} cvmx_gmxx_txx_stat4_t;


/**
 * cvmx_gmx#_tx#_stat5
 *
 * GMX_TX_STAT5 = GMX_TX_STATS_HIST3 (128- 255) / GMX_TX_STATS_HIST2 (65- 127)
 *
 *
 * Notes:
 * - Packet length is the sum of all data transmitted on the wire for the given
 *   packet including packet data, pad bytes, fcs bytes, pause bytes, and jam
 *   bytes.  The octect counts do not include PREAMBLE byte or EXTEND cycles.
 * - Read and write CSR operations must arbitrate for the STATS resources with the packet receiver
 * - Cleared either by a write (of any value) or a read when GMX_TX_STATS_CTL[RD_CLR] is set
 * - Counters will wrap
 */
typedef union
{
    uint64_t u64;
    struct cvmx_gmxx_txx_stat5_s
    {
        uint64_t hist3                   : 32;      /**< Number of packets sent with an octet count of
                                                         128 - 255. */
        uint64_t hist2                   : 32;      /**< Number of packets sent with an octet count of
                                                         65 - 127. */
    } s;
} cvmx_gmxx_txx_stat5_t;


/**
 * cvmx_gmx#_tx#_stat6
 *
 * GMX_TX_STAT6 = GMX_TX_STATS_HIST5 (512-1023) / GMX_TX_STATS_HIST4 (256-511)
 *
 *
 * Notes:
 * - Packet length is the sum of all data transmitted on the wire for the given
 *   packet including packet data, pad bytes, fcs bytes, pause bytes, and jam
 *   bytes.  The octect counts do not include PREAMBLE byte or EXTEND cycles.
 * - Read and write CSR operations must arbitrate for the STATS resources with the packet receiver
 * - Cleared either by a write (of any value) or a read when GMX_TX_STATS_CTL[RD_CLR] is set
 * - Counters will wrap
 */
typedef union
{
    uint64_t u64;
    struct cvmx_gmxx_txx_stat6_s
    {
        uint64_t hist5                   : 32;      /**< Number of packets sent with an octet count of
                                                         512 - 1023. */
        uint64_t hist4                   : 32;      /**< Number of packets sent with an octet count of
                                                         256 - 511. */
    } s;
} cvmx_gmxx_txx_stat6_t;


/**
 * cvmx_gmx#_tx#_stat7
 *
 * GMX_TX_STAT7 = GMX_TX_STATS_HIST7 (1024-1518) / GMX_TX_STATS_HIST6 (>1518)
 *
 *
 * Notes:
 * - Packet length is the sum of all data transmitted on the wire for the given
 *   packet including packet data, pad bytes, fcs bytes, pause bytes, and jam
 *   bytes.  The octect counts do not include PREAMBLE byte or EXTEND cycles.
 * - Read and write CSR operations must arbitrate for the STATS resources with the packet receiver
 * - Cleared either by a write (of any value) or a read when GMX_TX_STATS_CTL[RD_CLR] is set
 * - Counters will wrap
 */
typedef union
{
    uint64_t u64;
    struct cvmx_gmxx_txx_stat7_s
    {
        uint64_t hist7                   : 32;      /**< Number of packets sent with an octet count
                                                         of > 1518. */
        uint64_t hist6                   : 32;      /**< Number of packets sent with an octet count of
                                                         1024 - 1518. */
    } s;
} cvmx_gmxx_txx_stat7_t;


/**
 * cvmx_gmx#_tx#_stat8
 *
 * GMX_TX_STAT8 = GMX_TX_STATS_MCST  / GMX_TX_STATS_BCST
 *
 *
 * Notes:
 * - Read and write CSR operations must arbitrate for the STATS resources with the packet receiver
 * - Cleared either by a write (of any value) or a read when GMX_TX_STATS_CTL[RD_CLR] is set
 * - Counters will wrap
 * - Note, GMX determines if the packet is MCST or BCST from the DMAC of the
 *   packet.  GMX assumes that the DMAC lies in the first 6 bytes of the packet
 *   as per the 802.3 frame definition.  If the system requires additional data
 *   before the L2 header, then the MCST and BCST counters may not reflect
 *   reality and should be ignored by software.
 */
typedef union
{
    uint64_t u64;
    struct cvmx_gmxx_txx_stat8_s
    {
        uint64_t mcst                    : 32;      /**< Number of packets sent to multicast DMAC.
                                                         Does not include BCST packets. */
        uint64_t bcst                    : 32;      /**< Number of packets sent to broadcast DMAC.
                                                         Does not include MCST packets. */
    } s;
} cvmx_gmxx_txx_stat8_t;


/**
 * cvmx_gmx#_tx#_stat9
 *
 * GMX_TX_STAT9 = GMX_TX_STATS_UNDFLW / GMX_TX_STATS_CTL
 *
 *
 * Notes:
 * - Read and write CSR operations must arbitrate for the STATS resources with the packet receiver
 * - Cleared either by a write (of any value) or a read when GMX_TX_STATS_CTL[RD_CLR] is set
 * - Counters will wrap
 */
typedef union
{
    uint64_t u64;
    struct cvmx_gmxx_txx_stat9_s
    {
        uint64_t undflw                  : 32;      /**< Number of underflow packets */
        uint64_t ctl                     : 32;      /**< Number of Control packets (PAUSE flow control)
                                                         generated by GMX.  It does not include control
                                                         packets forwarded or generated by the PP's. */
    } s;
} cvmx_gmxx_txx_stat9_t;


/**
 * cvmx_gmx#_tx#_stats_ctl
 *
 * GMX_TX_STATS_CTL = TX Stats Control register
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_gmxx_txx_stats_ctl_s
    {
        uint64_t reserved_1_63           : 63;
        uint64_t rd_clr                  : 1;       /**< Stats registers will clear on reads */
    } s;
} cvmx_gmxx_txx_stats_ctl_t;



/************** PIP stats CSRs ******************/

/**
 * cvmx_pip_stat0_prt#
 *
 * PIP Statistics Counters
 * (RGMII/SPI Ports: 0-31 / PCI ports: 32-35)
 * 
 * Note: special stat counter behavior
 * 
 * 1) Read and write operations must arbitrate for the statistics resources
 *     along with the packet engines which are incrementing the counters.
 *     In order to not drop packet information, the packet HW is always a
 *     higher priority and the CSR requests will only be satisified when
 *     there are idle cycles.  This can potentially cause long delays if the
 *     system becomes full.
 * 
 * 2) stat counters can be cleared in two ways.  If PIP_STAT_CTL[RDCLR] is
 *     set, then all read accesses will clear the register.  In addition,
 *     any write to a stats register will also reset the register to zero.
 *     Please note that the clearing operations must obey rule \#1 above.
 * 
 * 3) all counters are wrapping - software must ensure they are read periodically
 * PIP_STAT0_PRT = PIP_STAT_DRP_PKTS / PIP_STAT_DRP_OCTS
 */
typedef union
{
    uint64_t u64;
    struct cvmx_pip_stat0_prtx_s
    {
        uint64_t drp_pkts                : 32;      /**< Inbound packets marked to be dropped by the IPD
                                                         QOS widget per port */
        uint64_t drp_octs                : 32;      /**< Inbound octets marked to be dropped by the IPD
                                                         QOS widget per port */
    } s;
} cvmx_pip_stat0_prtx_t;


/**
 * cvmx_pip_stat1_prt#
 *
 * PIP_STAT1_PRTX = PIP_STAT_OCTS
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_pip_stat1_prtx_s
    {
        uint64_t reserved_48_63          : 16;
        uint64_t octs                    : 48;      /**< Number of octets received by PIP (good and bad) */
    } s;
} cvmx_pip_stat1_prtx_t;


/**
 * cvmx_pip_stat2_prt#
 *
 * PIP_STAT2_PRTX = PIP_STAT_PKTS     / PIP_STAT_RAW
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_pip_stat2_prtx_s
    {
        uint64_t pkts                    : 32;      /**< Number of packets processed by PIP */
        uint64_t raw                     : 32;      /**< RAWFULL + RAWSCH Packets without an L1/L2 error
                                                         received by PIP per port */
    } s;
} cvmx_pip_stat2_prtx_t;


/**
 * cvmx_pip_stat3_prt#
 *
 * PIP_STAT3_PRTX = PIP_STAT_BCST     / PIP_STAT_MCST
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_pip_stat3_prtx_s
    {
        uint64_t bcst                    : 32;      /**< Number of indentified L2 broadcast packets
                                                         Does not include multicast packets
                                                         Only includes packets whose parse mode is
                                                         SKIP_TO_L2. */
        uint64_t mcst                    : 32;      /**< Number of indentified L2 multicast packets
                                                         Does not include broadcast packets
                                                         Only includes packets whose parse mode is
                                                         SKIP_TO_L2. */
    } s;
} cvmx_pip_stat3_prtx_t;


/**
 * cvmx_pip_stat4_prt#
 *
 * PIP_STAT4_PRTX = PIP_STAT_HIST1    / PIP_STAT_HIST0
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_pip_stat4_prtx_s
    {
        uint64_t h65to127                : 32;      /**< Number of 65-127B packets */
        uint64_t h64                     : 32;      /**< Number of 1-64B packets */
    } s;
} cvmx_pip_stat4_prtx_t;


/**
 * cvmx_pip_stat5_prt#
 *
 * PIP_STAT5_PRTX = PIP_STAT_HIST3    / PIP_STAT_HIST2
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_pip_stat5_prtx_s
    {
        uint64_t h256to511               : 32;      /**< Number of 256-511B packets */
        uint64_t h128to255               : 32;      /**< Number of 128-255B packets */
    } s;
} cvmx_pip_stat5_prtx_t;


/**
 * cvmx_pip_stat6_prt#
 *
 * PIP_STAT6_PRTX = PIP_STAT_HIST5    / PIP_STAT_HIST4
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_pip_stat6_prtx_s
    {
        uint64_t h1024to1518             : 32;      /**< Number of 1024-1518B packets */
        uint64_t h512to1023              : 32;      /**< Number of 512-1023B packets */
    } s;
} cvmx_pip_stat6_prtx_t;


/**
 * cvmx_pip_stat7_prt#
 *
 * PIP_STAT7_PRTX = PIP_STAT_FCS      / PIP_STAT_HIST6
 *
 *
 * Notes:
 * Note: FCS is not checked on the PCI ports 32..35.
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_pip_stat7_prtx_s
    {
        uint64_t fcs                     : 32;      /**< Number of packets with FCS or Align opcode errors */
        uint64_t h1519                   : 32;      /**< Number of 1519-max packets */
    } s;
} cvmx_pip_stat7_prtx_t;


/**
 * cvmx_pip_stat8_prt#
 *
 * PIP_STAT8_PRTX = PIP_STAT_FRAG     / PIP_STAT_UNDER
 *
 *
 * Notes:
 * Note: FCS is not checked on the PCI ports 32..35.
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_pip_stat8_prtx_s
    {
        uint64_t frag                    : 32;      /**< Number of packets with length < min and FCS error */
        uint64_t undersz                 : 32;      /**< Number of packets with length < min */
    } s;
} cvmx_pip_stat8_prtx_t;


/**
 * cvmx_pip_stat9_prt#
 *
 * PIP_STAT9_PRTX = PIP_STAT_JABBER   / PIP_STAT_OVER
 *
 *
 * Notes:
 * Note: FCS is not checked on the PCI ports 32..35.
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_pip_stat9_prtx_s
    {
        uint64_t jabber                  : 32;      /**< Number of packets with length > max and FCS error */
        uint64_t oversz                  : 32;      /**< Number of packets with length > max */
    } s;
} cvmx_pip_stat9_prtx_t;


/**
 * cvmx_pip_stat_ctl
 *
 * PIP_STAT_CTL = PIP's Stat Control Register
 * 
 * Controls how the PIP statistics counters are handled.
 */
typedef union
{
    uint64_t u64;
    struct cvmx_pip_stat_ctl_s
    {
        uint64_t reserved_1_63           : 63;
        uint64_t rdclr                   : 1;       /**< Stat registers are read and clear
                                                         0 = stat registers hold value when read
                                                         1 = stat registers are cleared when read */
    } s;
} cvmx_pip_stat_ctl_t;


/**
 * cvmx_pip_stat_inb_errs#
 *
 * PIP_STAT_INB_ERRSX = Inbound error packets received by PIP per port
 * 
 * Inbound stats collect all data sent to PIP from all packet interfaces.
 * Its the raw counts of everything that comes into the block.  The counts
 * will reflect all error packets and packets dropped by the PKI RED engine.
 * These counts are intended for system debug, but could convey useful
 * information in production systems.
 */
typedef union
{
    uint64_t u64;
    struct cvmx_pip_stat_inb_errsx_s
    {
        uint64_t reserved_16_63          : 48;
        uint64_t errs                    : 16;      /**< Number of packets with GMX/SPX/PCI errors
                                                         received by PIP */
    } s;
} cvmx_pip_stat_inb_errsx_t;


/**
 * cvmx_pip_stat_inb_octs#
 *
 * PIP_STAT_INB_OCTSX = Inbound octets received by PIP per port
 * 
 * Inbound stats collect all data sent to PIP from all packet interfaces.
 * Its the raw counts of everything that comes into the block.  The counts
 * will reflect all error packets and packets dropped by the PKI RED engine.
 * These counts are intended for system debug, but could convey useful
 * information in production systems.
 */
typedef union
{
    uint64_t u64;
    struct cvmx_pip_stat_inb_octsx_s
    {
        uint64_t reserved_48_63          : 16;
        uint64_t octs                    : 48;      /**< Total number of octets from all packets received
                                                         by PIP */
    } s;
} cvmx_pip_stat_inb_octsx_t;


/**
 * cvmx_pip_stat_inb_pkts#
 *
 * PIP_STAT_INB_PKTSX = Inbound packets received by PIP per port
 * 
 * Inbound stats collect all data sent to PIP from all packet interfaces.
 * Its the raw counts of everything that comes into the block.  The counts
 * will reflect all error packets and packets dropped by the PKI RED engine.
 * These counts are intended for system debug, but could convey useful
 * information in production systems.
 */
typedef union
{
    uint64_t u64;
    struct cvmx_pip_stat_inb_pktsx_s
    {
        uint64_t reserved_32_63          : 32;
        uint64_t pkts                    : 32;      /**< Number of packets without GMX/SPX/PCI errors
                                                         received by PIP */
    } s;
} cvmx_pip_stat_inb_pktsx_t;

#endif



/*****************************************************************************/
/**
  * Read board eth port's statistics.
  * @param port_num: port number.
  * @param clear: if clear=1, clear all statistics after reading CSRs.
  * @param port_counter: all statistics data.
  * @return: 0 for success. -1 for failure.
  */
int do_read_board_eth_port_stats
(
	int port_num, 
	int clear, 
	struct npd_port_counter* port_counter,
	struct port_oct_s* portOctCounter
);


#endif
