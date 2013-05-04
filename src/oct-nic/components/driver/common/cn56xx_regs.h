/*
 *
 * OCTEON SDK
 *
 * Copyright (c) 2011 Cavium Networks. All rights reserved.
 *
 * This file, which is part of the OCTEON SDK which also includes the
 * OCTEON SDK Package from Cavium Networks, contains proprietary and
 * confidential information of Cavium Networks and in some cases its
 * suppliers. 
 *
 * Any licensed reproduction, distribution, modification, or other use of
 * this file or the confidential information or patented inventions
 * embodied in this file is subject to your license agreement with Cavium
 * Networks. Unless you and Cavium Networks have agreed otherwise in
 * writing, the applicable license terms "OCTEON SDK License Type 5" can be found 
 * under the directory: $OCTEON_ROOT/components/driver/licenses/
 *
 * All other use and disclosure is prohibited.
 *
 * Contact Cavium Networks at info@caviumnetworks.com for more information.
 *
 */

/*! \file cn56xx_regs.h
    \brief Host Driver: Register Address and Register Mask values for
                        Octeon CN56XX devices.
*/


#ifndef __CN56XX_REGS_H__
#define __CN56XX_REGS_H__



#define     CN56XX_CONFIG_MSI_CAP          0x50
#define     CN56XX_CONFIG_MSI_ADDR_LO      0x54
#define     CN56XX_CONFIG_MSI_ADDR_HI      0x58
#define     CN56XX_CONFIG_MSI_DATA         0x5C


#define     CN56XX_CONFIG_PCIE_CAP         0x70
#define     CN56XX_CONFIG_PCIE_DEVCAP      0x74
#define     CN56XX_CONFIG_PCIE_DEVCTL      0x78
#define     CN56XX_CONFIG_PCIE_LINKCAP     0x7C
#define     CN56XX_CONFIG_PCIE_LINKCTL     0x80
#define     CN56XX_CONFIG_PCIE_SLOTCAP     0x84
#define     CN56XX_CONFIG_PCIE_SLOTCTL     0x88

#define     CN56XX_CONFIG_PCIE_ENH_CAP              0x100
#define     CN56XX_CONFIG_PCIE_UNCORR_ERR_STATUS    0x104
#define     CN56XX_CONFIG_PCIE_UNCORR_ERR_MASK      0x108
#define     CN56XX_CONFIG_PCIE_UNCORR_ERR_SEVERITY  0x10C
#define     CN56XX_CONFIG_PCIE_CORR_ERR_STATUS      0x110
#define     CN56XX_CONFIG_PCIE_CORR_ERR_MASK        0x114

#define     CN56XX_CONFIG_PCIE_ADV_ERR_CAP          0x118

#define     CN56XX_CONFIG_PCIE_ACK_REPLAY_TIMER     0x700
#define     CN56XX_CONFIG_PCIE_OTHER_MSG            0x704
#define     CN56XX_CONFIG_PCIE_PORT_FORCE_LINK      0x708
#define     CN56XX_CONFIG_PCIE_ACK_FREQ             0x70C
#define     CN56XX_CONFIG_PCIE_PORT_LINK_CTL        0x710
#define     CN56XX_CONFIG_PCIE_LANE_SKEW            0x714
#define     CN56XX_CONFIG_PCIE_SYM_NUM              0x718
#define     CN56XX_CONFIG_PCIE_SYM_TIMER_FILT_MASK  0x71C





/*   CN56XX Hardware Manual defines the BAR0 registers as 64-bit registers.
     CN56XX does not have any windowed-registers for PCI configuration.
*/

#define    CN56XX_NPEI_CTL_STATUS                  0x0570

#define    CN56XX_NPEI_CTL_STATUS_2                0x3C00

#define    CN56XX_NPEI_SCRATCH                     0x0270

#define    CN56XX_NPEI_DBG_DATA                    0x0510

#define    CN56XX_NPEI_DMA_PCIE_REQ_NUM            0x05B0







/*
  ##############  BAR0 Registers ################
*/


#define    CN56XX_WIN_WR_ADDR_LO                   0x0200
#define    CN56XX_WIN_WR_ADDR_HI                   0x0204
#define    CN56XX_WIN_WR_ADDR64                    CN56XX_WIN_WR_ADDR_LO

#define    CN56XX_WIN_RD_ADDR_LO                   0x0210
#define    CN56XX_WIN_RD_ADDR_HI                   0x0214
#define    CN56XX_WIN_RD_ADDR64                    CN56XX_WIN_RD_ADDR_LO

#define    CN56XX_WIN_WR_DATA_LO                   0x0220
#define    CN56XX_WIN_WR_DATA_HI                   0x0224
#define    CN56XX_WIN_WR_DATA64                    CN56XX_WIN_WR_DATA_LO

#define    CN56XX_WIN_RD_DATA_LO                   0x0240
#define    CN56XX_WIN_RD_DATA_HI                   0x0244
#define    CN56XX_WIN_RD_DATA64                    CN56XX_WIN_RD_DATA_LO

#define    CN56XX_WIN_WR_MASK_REG                  0x0230


#define    CN56XX_BAR1_INDEX_START                 0x0000
#define    CN56XX_PCI_BAR1_OFFSET                  0x10




/* 1 register (32-bit) to enable Input queues */
#define    CN56XX_NPEI_PKT_INSTR_ENB               0x1000

/* 1 register (32-bit) to enable Output queues */
#define    CN56XX_NPEI_PKT_OUT_ENB                 0x1010


/* 1 register (32-bit) to determine whether Output queues are in reset. */
#define    CN56XX_NPEI_PORT_IN_RST_OQ              0x0690

/* 1 register (32-bit) to determine whether Input queues are in reset. */
#define    CN56XX_NPEI_PORT_IN_RST_IQ              0x0694



#define    CN56XX_BAR1_INDEX_REG(idx)              \
           (CN56XX_BAR1_INDEX_START + (CN56XX_PCI_BAR1_OFFSET * (idx)))





/*############################ REQUEST QUEUE #########################*/

/*------- BAR0 Registers ---------*/
/* 32 registers for Input Queue Start Addr - NPEI_PKT0_INSTR_BADDR */
#define    CN56XX_NPEI_IQ_BASE_ADDR_START64       0x2800

/* 32 registers for Input Queue size - NPEI_PKT0_INSTR_FIFO_RSIZE */
#define    CN56XX_NPEI_IQ_SIZE_START              0x3000

/* 32 registers for Instruction Header Options - NPEI_PKT0_INSTR_HEADER */ 
#define    CN56XX_NPEI_IQ_PKT_INSTR_HDR_START64   0x3400

/* 32 registers for Input Doorbell - NPEI_PKT0_INSTR_BAOFF_DBELL */
#define    CN56XX_NPEI_IQ_DOORBELL_START          0x2C00

/* 32 registers for Input Queue Instr Count - NPEI_PKT_IN_DONE0_CNTS */
#define    CN56XX_NPEI_IQ_INSTR_COUNT_START       0x2000

/* Each Input Queue register is at a 16-byte Offset in BAR0 */
#define    CN56XX_IQ_OFFSET                       0x10


/* 1 register (32-bit) - instr. size of each input queue. */
#define    CN56XX_NPEI_PKT_INSTR_SIZE             0x1020


/* 1 register (64-bit) - Back Pressure setting for each input queue. */
#define    CN56XX_NPEI_INPUT_BP_START64           0x3800




/* 1 register (32-bit) - ES, RO, NS, Arbitration for Input Queue Data &
                         gather list fetches.
*/
#define    CN56XX_NPEI_PKT_INPUT_CONTROL           0x1150


/* 1 register (64-bit) - 2 bits for each input ring.
*/
#define    CN56XX_NPEI_PKT_INSTR_RD_SIZE           0x1190


/* 1 register (64-bit) - 2 bits for each input ring.
*/
#define    CN56XX_NPEI_IN_PCIE_PORT                0x11A0




/*------- Request Queue Macros ---------*/
/* 32 registers for Input Queue Start Addr */
#define    CN56XX_NPEI_IQ_BASE_ADDR64(iq)          \
       (CN56XX_NPEI_IQ_BASE_ADDR_START64 + ((iq) * CN56XX_IQ_OFFSET))


/* 32 registers for Input Queue size */
#define    CN56XX_NPEI_IQ_SIZE(iq)                 \
       (CN56XX_NPEI_IQ_SIZE_START + ((iq) * CN56XX_IQ_OFFSET))


/* 32 IQ Packet Instruction Header Registers */
#define    CN56XX_NPEI_IQ_PKT_INSTR_HDR64(iq)      \
       (CN56XX_NPEI_IQ_PKT_INSTR_HDR_START64 + ((iq) * CN56XX_IQ_OFFSET))


/* 32 registers for Input Doorbell */
#define    CN56XX_NPEI_IQ_DOORBELL(iq)             \
       (CN56XX_NPEI_IQ_DOORBELL_START + ((iq) * CN56XX_IQ_OFFSET))


/* 32 registers for Input Instruction Count */
#define    CN56XX_NPEI_IQ_INSTR_COUNT(iq)          \
       (CN56XX_NPEI_IQ_INSTR_COUNT_START + ((iq) * CN56XX_IQ_OFFSET))

#define    CN56XX_NPEI_IQ_BP64(iq)                 \
       (CN56XX_NPEI_INPUT_BP_START64 + ((iq) * CN56XX_IQ_OFFSET))



/*------------------ Masks ----------------*/
#define    CN56XX_INPUT_CTL_ROUND_ROBIN_ARB         (1 << 22)
#define    CN56XX_INPUT_CTL_DATA_NS                 (1 << 8)
#define    CN56XX_INPUT_CTL_DATA_ES_64B_SWAP        (1 << 6)
#define    CN56XX_INPUT_CTL_DATA_RO                 (1 << 5)
#define    CN56XX_INPUT_CTL_USE_CSR                 (1 << 4)
#define    CN56XX_INPUT_CTL_GATHER_NS               (1 << 3)
#define    CN56XX_INPUT_CTL_GATHER_ES_64B_SWAP      (2)
#define    CN56XX_INPUT_CTL_GATHER_RO               (1)

#if  __CAVIUM_BYTE_ORDER == __CAVIUM_LITTLE_ENDIAN
#define    CN56XX_INPUT_CTL_MASK                    \
            ( CN56XX_INPUT_CTL_ROUND_ROBIN_ARB      \
            | CN56XX_INPUT_CTL_DATA_ES_64B_SWAP     \
            | CN56XX_INPUT_CTL_USE_CSR ) 
#else
#define    CN56XX_INPUT_CTL_MASK                    \
           ( CN56XX_INPUT_CTL_ROUND_ROBIN_ARB       \
            | CN56XX_INPUT_CTL_DATA_ES_64B_SWAP     \
            | CN56XX_INPUT_CTL_USE_CSR              \
            | CN56XX_INPUT_CTL_GATHER_ES_64B_SWAP)
#endif



/*############################ OUTPUT QUEUE #########################*/

/*------- BAR0 Registers ---------*/

/* 32 registers for Output Queue Start Addr - NPEI_PKT0_SLIST_BADDR */
#define    CN56XX_NPEI_OQ_BASE_ADDR_START64       0x1400

/* 32 registers for Output Queue size - NPEI_PKT0_SLIST_FIFO_RSIZE */
#define    CN56XX_NPEI_OQ_SIZE_START              0x1C00

/* 32 registers for Output Queue Packet Count - NPEI_PKT0_CNTS */
#define    CN56XX_NPEI_OQ_PKT_SENT_START          0x2400

/* 32 registers for Output Queue Packet Credits - NPEI_PKT0_SLIST_BAOFF_DBELL */
#define    CN56XX_NPEI_OQ_PKT_CREDITS_START       0x1800


/* Each Output Queue register is at a 16-byte Offset in BAR0 */
#define    CN56XX_OQ_OFFSET                       0x10




/* 1 register for Output queue buffer and info size - NPEI_PKT_SLIST_ID_SIZE */
#define    CN56XX_NPEI_OQ_BUFF_INFO_SIZE          0x1180

/* 1 (64-bit) register for Output Queue Packet Count Interrupt Threshold
   & Time Threshold. We use the 32-bit offsets to define distinct addresses. */
#define    CN56XX_NPEI_OQ_INT_LEV_PKTS            0x1100
#define    CN56XX_NPEI_OQ_INT_LEV_TIME            0x1104


/* 1 register (64-bit) - PCIE port for each of 32 Output queues. */
#define    CN56XX_NPEI_PKT_PCIE_PORT64            0x10E0

/* 1 register (64-bit) - Endian-Swap mode for 32 Output Queue Data. */
#define    CN56XX_NPEI_PKT_DATA_OUT_ES64          0x10B0

/* 1 register (64-bit) - Endian-Swap mode for 32 Output Queue ScatterList. */
#define    CN56XX_NPEI_PKT_SLIST_ES64             0x1050




/* 1 register (32-bit) - Relaxed Ordering for 32 Output Queues Data. */
#define    CN56XX_NPEI_PKT_DATA_OUT_ROR           0x1090

/* 1 register (32-bit) - Relaxed Ordering for 32 Output Queues Scatter List. */
#define    CN56XX_NPEI_PKT_SLIST_ROR              0x1030

/* 1 register (32-bit) - No Snoop mode for 32 Output Queues Data. */
#define    CN56XX_NPEI_PKT_DATA_OUT_NS            0x10A0

/* 1 register (32-bit) - No Snoop mode for 32 Output Queues Scatter List. */
#define    CN56XX_NPEI_PKT_SLIST_NS               0x1040

/* 1 register (32-bit) - Packet OR Byte Count for 32 Output queues Interrupts.*/
#define    CN56XX_NPEI_PKT_OUT_BMODE              0x10D0

/* 1 register (32-bit) - source of ES,RO,NS bits for 32 OQ's.*/
#define    CN56XX_NPEI_PKT_DPADDR                 0x1080

/* 1 register (32-bit) - InfoPtr use for 32 Output Queues.*/
#define    CN56XX_NPEI_PKT_IPTR                   0x1070






/*------- Output Queue Macros ---------*/
/* 32 registers for Output Queue Start Addr */
#define    CN56XX_NPEI_OQ_BASE_ADDR64(oq)          \
       (CN56XX_NPEI_OQ_BASE_ADDR_START64 + ((oq) * CN56XX_OQ_OFFSET))


/* 32 registers for Output Queue size */
#define    CN56XX_NPEI_OQ_SIZE(oq)                 \
       (CN56XX_NPEI_OQ_SIZE_START + ((oq) * CN56XX_OQ_OFFSET))


/* 32 registers for Output Queue Packet Sent Count */
#define    CN56XX_NPEI_OQ_PKTS_SENT(oq)            \
       (CN56XX_NPEI_OQ_PKT_SENT_START + ((oq) * CN56XX_OQ_OFFSET))


/* 32 registers for Output Queue Packet Credit */
#define    CN56XX_NPEI_OQ_PKTS_CREDIT(oq)          \
       (CN56XX_NPEI_OQ_PKT_CREDITS_START + ((oq) * CN56XX_OQ_OFFSET))













/*############################ DMA QUEUE #########################*/

/* 1 register (64-bit) - 2 32-bit Counter - 1 for each DMA Queue 0/1. */ 
#define    CN56XX_DMA_CNT_START                   0x05E0

/* 2 registers (64-bit) - DMA count & Time Interrupt threshold -
   NPEI_DMA_0_INT_LEVEL */
#define    CN56XX_DMA_INT_LEVEL_START             0x05C0


#define    CN56XX_DMA_IBUFF_SADDR                 0x0400




/*---------- DMA Queue Macros ---------*/
#define    CN56XX_DMA_CNT(dq)                      \
           (CN56XX_DMA_CNT_START + ((dq) * 4))

#define    CN56XX_DMA_PKT_INT_LEVEL(dq)            \
           (CN56XX_DMA_INT_LEVEL_START + ((dq) * 0x10))

#define    CN56XX_DMA_TIME_INT_LEVEL(dq)           \
           (CN56XX_DMA_INT_LEVEL_START + 4 + ((dq) * 0x10))

#define    CN56XX_DMA_BASE_ADDR(dq)                \
           (CN56XX_DMA_IBUFF_SADDR + (0x10 * (dq)))




/*############################ INTERRUPTS #########################*/

/* 1 register (64-bit) for Interrupt Summary */
#define    CN56XX_NPEI_INT_SUM64                  0x0530


/* 1 register (64-bit) for Interrupt Enable */
#define    CN56XX_NPEI_INT_ENB64                  0x0540


/* 1 register (64-bit) for Additional Interrupt Summary */
#define    CN56XX_NPEI_INT_A_SUM64                0x0550


/* 1 register (64-bit) for Additional Interrupt Enable */
#define    CN56XX_NPEI_INT_A_ENB64                0x0560




/* 1 register (32-bit) to enable Output Queue Packet/Byte Count Interrupt */ 
#define    CN56XX_NPEI_PKT_CNT_INT_ENB            0x1130


/* 1 register (32-bit) to enable Output Queue Packet Timer Interrupt */ 
#define    CN56XX_NPEI_PKT_TIME_INT_ENB           0x1140



/* 1 register (32-bit) to indicate which Output Queue reached pkt threshold */
#define    CN56XX_NPEI_PKT_CNT_INT                0x1110

/* 1 register (32-bit) to indicate which Output Queue reached time threshold */
#define    CN56XX_NPEI_PKT_TIME_INT               0x1120






/*------------------ Interrupt Masks ----------------*/

#define    CN56XX_INTR_RML_WR_TIMEOUT            (1)
#define    CN56XX_INTR_RML_RD_TIMEOUT            (1 << 1)
#define    CN56XX_INTR_BAR0_RW_TIMEOUT           (1 << 2)
#define    CN56XX_INTR_IO2BIG                    (1 << 3)

#define    CN56XX_INTR_DMA0_DB_OF                (1 << 4)
#define    CN56XX_INTR_DMA1_DB_OF                (1 << 5)
#define    CN56XX_INTR_DMA2_DB_OF                (1 << 6)
#define    CN56XX_INTR_DMA3_DB_OF                (1 << 7)
#define    CN56XX_INTR_DMA4_DB_OF                (1 << 8)
#define    CN56XX_INTR_DMA_DB_OF                 (0x1F0)

#define    CN56XX_INTR_DMA0_FORCE                (1 << 9)
#define    CN56XX_INTR_DMA1_FORCE                (1 << 10)

#define    CN56XX_INTR_DMA0_COUNT                (1 << 11)
#define    CN56XX_INTR_DMA1_COUNT                (1 << 12)

#define    CN56XX_INTR_DMA0_TIME                 (1 << 13)
#define    CN56XX_INTR_DMA1_TIME                 (1 << 14)

#define    CN56XX_INTR_PKT_SLIST_OF              (1 << 15)
#define    CN56XX_INTR_PKT_INSTR_OF              (1 << 16)

#define    CN56XX_INTR_PKT_COUNT                 (1 << 17)
#define    CN56XX_INTR_PKT_TIME                  (1 << 18)

#define    CN56XX_INTR_RC_MODE                   ((1 << 19) | (1 << 21) \
                                                  |(1 << 26) | (1 << 28))

#define    CN56XX_INTR_CRS_TIMEOUT_PCIE0         ((1 << 20) | (1 << 22))

#define    CN56XX_INTR_PMU_WAKE_PCIE0            (1 << 23)
#define    CN56XX_INTR_PME_PCIE0                 (1 << 24)
#define    CN56XX_INTR_HOTPLUG_PCIE0             (1 << 25)

#define    CN56XX_INTR_CRS_TIMEOUT_PCIE1         ((1 << 27) | (1 << 29))
#define    CN56XX_INTR_PMU_WAKE_PCIE1            (1 << 30)
#define    CN56XX_INTR_PME_PCIE1                 (1 << 31)
#define    CN56XX_INTR_HOTPLUG_PCIE1             (1ULL << 32)

#define    CN56XX_INTR_UNSUPPORTED_TLP           (0x01FFFFFE00000000ULL)

#define    CN56XX_INTR_INT_A                     (1ULL << 61)
#define    CN56XX_INTR_MIO                       (1ULL << 63)




#define    CN56XX_INTR_DMA0_DATA                 \
            ( CN56XX_INTR_DMA0_TIME)

#define    CN56XX_INTR_DMA1_DATA                 \
            ( CN56XX_INTR_DMA1_TIME )

#define    CN56XX_INTR_DMA_DATA                  \
            ( CN56XX_INTR_DMA0_DATA              \
            | CN56XX_INTR_DMA1_DATA)


#define    CN56XX_INTR_PKT_DATA               (CN56XX_INTR_PKT_TIME)


/* Sum of interrupts for all PCI-Express Data Interrupts */
#define    CN56XX_INTR_PCIE_DATA                 \
           (  CN56XX_INTR_DMA_DATA               \
            | CN56XX_INTR_PKT_DATA )



/* Sum of interrupts for Internal Doorbell overflow events */
#define    CN56XX_INTR_ERR_OVERFLOW              \
           ( CN56XX_INTR_DMA_DB_OF               \
            | CN56XX_INTR_PKT_SLIST_OF           \
            | CN56XX_INTR_PKT_INSTR_OF )


/* Sum of interrupts for Miscellaneous errors. */
#define    CN56XX_INTR_ERR_MISC                  \
           (  CN56XX_INTR_RML_WR_TIMEOUT         \
            | CN56XX_INTR_RML_RD_TIMEOUT         \
            | CN56XX_INTR_BAR0_RW_TIMEOUT        \
            | CN56XX_INTR_IO2BIG )


/* Sum of interrupts for all error events */
#define    CN56XX_INTR_ERR                       \
           (  CN56XX_INTR_ERR_OVERFLOW           \
            | CN56XX_INTR_UNSUPPORTED_TLP        \
            | CN56XX_INTR_INT_A                  \
            | CN56XX_INTR_ERR_MISC)


/* Sum of interrupt for Power-Management Events - PCIE Core 0 */
#define    CN56XX_INTR_POWER_MGMT_PCIE0          \
           (  CN56XX_INTR_PMU_WAKE_PCIE0         \
            | CN56XX_INTR_PME_PCIE0              \
            | CN56XX_INTR_HOTPLUG_PCIE0)


/* Sum of interrupt for Power-Management Events - PCIE Core 1 */
#define    CN56XX_INTR_POWER_MGMT_PCIE1          \
           (  CN56XX_INTR_PMU_WAKE_PCIE1         \
            | CN56XX_INTR_PME_PCIE1              \
            | CN56XX_INTR_HOTPLUG_PCIE1)



/* Programmed Mask for Interrupt Sum */
#ifdef CVMCS_DMA_IC
#define    CN56XX_INTR_MASK                      \
           (  CN56XX_INTR_PCIE_DATA              \
	    | CN56XX_INTR_DMA0_COUNT		 \
	    | CN56XX_INTR_DMA0_TIME		 \
            | CN56XX_INTR_MIO                    \
            | CN56XX_INTR_ERR)
#else
#define    CN56XX_INTR_MASK                      \
           (  CN56XX_INTR_PCIE_DATA              \
            | CN56XX_INTR_DMA0_FORCE             \
            | CN56XX_INTR_DMA1_FORCE             \
            | CN56XX_INTR_MIO                    \
            | CN56XX_INTR_ERR)
#endif






/*------------ Interrupt INT_A  Masks -----------------------*/

#define    CN56XX_INT_A_DMA_URCA_PCIE0          (1 << 0)
#define    CN56XX_INT_A_DMA_URCA_PCIE1          (1 << 1)
#define    CN56XX_INT_A_INSTR_READ_ERR          (1 << 2)
#define    CN56XX_INT_A_SLIST_PTR_READ_ERR      (1 << 3)
#define    CN56XX_INT_A_INSTR_DATA_READ_ERR     (1 << 4)
#define    CN56XX_INT_A_INSTR_GLIST_READ_ERR    (1 << 5)
#define    CN56XX_INT_A_READ_LOCK_PCIE0         (1 << 6)
#define    CN56XX_INT_A_READ_LOCK_PCIE1         (1 << 7)
#define    CN56XX_INT_A_PKT_IN_WMARK            (1 << 8)
#define    CN56XX_INT_A_PKT_OUT_ERR             (1 << 9)



#define    CN56XX_INT_A_MASK      \
                ( CN56XX_INT_A_DMA_URCA_PCIE0               \
                 | CN56XX_INT_A_DMA_URCA_PCIE1              \
                 | CN56XX_INT_A_INSTR_READ_ERR              \
                 | CN56XX_INT_A_SLIST_PTR_READ_ERR          \
                 | CN56XX_INT_A_INSTR_DATA_READ_ERR         \
                 | CN56XX_INT_A_INSTR_GLIST_READ_ERR        \
                 | CN56XX_INT_A_READ_LOCK_PCIE0             \
                 | CN56XX_INT_A_READ_LOCK_PCIE1             \
                 | CN56XX_INT_A_PKT_IN_WMARK                \
                 | CN56XX_INT_A_PKT_OUT_ERR )






/*############################ CIU #########################*/

#define    CN56XX_CIU_PP_RST              0x0001070000000700ULL
#define    CN56XX_CIU_SOFT_BIST           0x0001070000000738ULL
#define    CN56XX_CIU_SOFT_PRST           0x0001070000000748ULL
#define    CN56XX_CIU_SOFT_RST            0x0001070000000740ULL
#define    CN56XX_CIU_PCI_INTA            0x0001070000000750ULL

/*---------- TWSI/MIO Registers -------------------*/

#define    MIO_BOOT_REG_CFG0              0x0001180000000000ULL
#define    MIO_BOOT_LOC_CFG0              0x0001180000000080ULL
#define    MIO_BOOT_LOC_ADR               0x0001180000000090ULL
#define    MIO_BOOT_LOC_DAT               0x0001180000000098ULL
#define    MIO_TWS_SW_TWSI                0x0001180000001000ULL
#define    MIO_TWS_TWSI_SW                0x0001180000001008ULL
#define    MIO_TWS_INT                    0x0001180000001010ULL
#define    MIO_TWS_SW_TWSI_EXT            0x0001180000001018ULL







#endif

/* $Id: cn56xx_regs.h 56301 2011-01-15 00:41:25Z skoteshwar $ */
