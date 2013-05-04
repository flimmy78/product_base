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

/*! \file cn68xx_regs.h
    \brief Host Driver: Register Address and Register Mask values for
                        Octeon CN68XX devices.
*/


#ifndef __CN68XX_REGS_H__
#define __CN68XX_REGS_H__


#define     CN68XX_CONFIG_XPANSION_BAR             0x30


#define     CN68XX_CONFIG_PCIE_CAP                 0x70
#define     CN68XX_CONFIG_PCIE_DEVCAP              0x74
#define     CN68XX_CONFIG_PCIE_DEVCTL              0x78
#define     CN68XX_CONFIG_PCIE_LINKCAP             0x7C
#define     CN68XX_CONFIG_PCIE_LINKCTL             0x80
#define     CN68XX_CONFIG_PCIE_SLOTCAP             0x84
#define     CN68XX_CONFIG_PCIE_SLOTCTL             0x88

#define     CN68XX_CONFIG_PCIE_FLTMSK              0x720






/*
  ##############  BAR0 Registers ################
*/

#define    CN68XX_SLI_CTL_PORT0                    0x0050
#define    CN68XX_SLI_CTL_PORT1                    0x0060

#define    CN68XX_SLI_DBG_DATA                     0x0310
#define    CN68XX_SLI_SCRATCH1                     0x03C0
#define    CN68XX_SLI_SCRATCH2                     0x03D0
#define    CN68XX_SLI_CTL_STATUS                   0x0570


#define    CN68XX_WIN_WR_ADDR_LO                   0x0000
#define    CN68XX_WIN_WR_ADDR_HI                   0x0004
#define    CN68XX_WIN_WR_ADDR64                    CN68XX_WIN_WR_ADDR_LO

#define    CN68XX_WIN_RD_ADDR_LO                   0x0010
#define    CN68XX_WIN_RD_ADDR_HI                   0x0014
#define    CN68XX_WIN_RD_ADDR64                    CN68XX_WIN_RD_ADDR_LO

#define    CN68XX_WIN_WR_DATA_LO                   0x0020
#define    CN68XX_WIN_WR_DATA_HI                   0x0024
#define    CN68XX_WIN_WR_DATA64                    CN68XX_WIN_WR_DATA_LO

#define    CN68XX_WIN_RD_DATA_LO                   0x0040
#define    CN68XX_WIN_RD_DATA_HI                   0x0044
#define    CN68XX_WIN_RD_DATA64                    CN68XX_WIN_RD_DATA_LO

#define    CN68XX_WIN_WR_MASK_LO                   0x0030
#define    CN68XX_WIN_WR_MASK_HI                   0x0034
#define    CN68XX_WIN_WR_MASK_REG                  CN68XX_WIN_WR_MASK_LO




/* 1 register (32-bit) to enable Input queues */
#define    CN68XX_SLI_PKT_INSTR_ENB               0x1000

/* 1 register (32-bit) to enable Output queues */
#define    CN68XX_SLI_PKT_OUT_ENB                 0x1010


/* 1 register (32-bit) to determine whether Output queues are in reset. */
#define    CN68XX_SLI_PORT_IN_RST_OQ              0x11F0

/* 1 register (32-bit) to determine whether Input queues are in reset. */
#define    CN68XX_SLI_PORT_IN_RST_IQ              0x11F4




/*###################### REQUEST QUEUE #########################*/

/* 1 register (32-bit) - instr. size of each input queue. */
#define    CN68XX_SLI_PKT_INSTR_SIZE             0x1020

/* 32 registers for Input Queue Instr Count - SLI_PKT_IN_DONE0_CNTS */
#define    CN68XX_SLI_IQ_INSTR_COUNT_START       0x2000

/* 32 registers for Input Queue Start Addr - SLI_PKT0_INSTR_BADDR */
#define    CN68XX_SLI_IQ_BASE_ADDR_START64       0x2800

/* 32 registers for Input Doorbell - SLI_PKT0_INSTR_BAOFF_DBELL */
#define    CN68XX_SLI_IQ_DOORBELL_START          0x2C00

/* 32 registers for Input Queue size - SLI_PKT0_INSTR_FIFO_RSIZE */
#define    CN68XX_SLI_IQ_SIZE_START              0x3000

/* 32 registers for Instruction Header Options - SLI_PKT0_INSTR_HEADER */
#define    CN68XX_SLI_IQ_PKT_INSTR_HDR_START64   0x3400

#define    CN68XX_SLI_IQ_PORT0_PKIND             0x0800

/* 1 register (64-bit) - Back Pressure for each input queue - SLI_PKT0_IN_BP */
#define    CN68XX_SLI_INPUT_BP_START64           0x3800

/* Each Input Queue register is at a 16-byte Offset in BAR0 */
#define    CN68XX_IQ_OFFSET                      0x10





/* 1 register (32-bit) - ES, RO, NS, Arbitration for Input Queue Data &
                         gather list fetches. SLI_PKT_INPUT_CONTROL.
*/
#define    CN68XX_SLI_PKT_INPUT_CONTROL          0x1170


/* 1 register (64-bit) - Number of instructions to read at one time
                         - 2 bits for each input ring. SLI_PKT_INSTR_RD_SIZE.
*/
#define    CN68XX_SLI_PKT_INSTR_RD_SIZE          0x11A0


/* 1 register (64-bit) - Assign Input ring to MAC port
                         - 2 bits for each input ring. SLI_PKT_IN_PCIE_PORT.
*/
#define    CN68XX_SLI_IN_PCIE_PORT               0x11B0




/*------- Request Queue Macros ---------*/
#define    CN68XX_SLI_IQ_BASE_ADDR64(iq)          \
       (CN68XX_SLI_IQ_BASE_ADDR_START64 + ((iq) * CN68XX_IQ_OFFSET))

#define    CN68XX_SLI_IQ_SIZE(iq)                 \
       (CN68XX_SLI_IQ_SIZE_START + ((iq) * CN68XX_IQ_OFFSET))

#define    CN68XX_SLI_IQ_PKT_INSTR_HDR64(iq)      \
       (CN68XX_SLI_IQ_PKT_INSTR_HDR_START64 + ((iq) * CN68XX_IQ_OFFSET))

#define    CN68XX_SLI_IQ_DOORBELL(iq)             \
       (CN68XX_SLI_IQ_DOORBELL_START + ((iq) * CN68XX_IQ_OFFSET))

#define    CN68XX_SLI_IQ_INSTR_COUNT(iq)          \
       (CN68XX_SLI_IQ_INSTR_COUNT_START + ((iq) * CN68XX_IQ_OFFSET))

#define    CN68XX_SLI_IQ_BP64(iq)                 \
       (CN68XX_SLI_INPUT_BP_START64 + ((iq) * CN68XX_IQ_OFFSET))

#define    CN68XX_SLI_IQ_PORT_PKIND(iq)           \
       (CN68XX_SLI_IQ_PORT0_PKIND + ((iq) * CN68XX_IQ_OFFSET))




/*------------------ Masks ----------------*/
#define    CN68XX_INPUT_CTL_ROUND_ROBIN_ARB         (1 << 22)
#define    CN68XX_INPUT_CTL_DATA_NS                 (1 << 8)
#define    CN68XX_INPUT_CTL_DATA_ES_64B_SWAP        (1 << 6)
#define    CN68XX_INPUT_CTL_DATA_RO                 (1 << 5)
#define    CN68XX_INPUT_CTL_USE_CSR                 (1 << 4)
#define    CN68XX_INPUT_CTL_GATHER_NS               (1 << 3)
#define    CN68XX_INPUT_CTL_GATHER_ES_64B_SWAP      (2)
#define    CN68XX_INPUT_CTL_GATHER_RO               (1)

#if  __CAVIUM_BYTE_ORDER == __CAVIUM_LITTLE_ENDIAN
#define    CN68XX_INPUT_CTL_MASK                    \
            ( CN68XX_INPUT_CTL_DATA_ES_64B_SWAP     \
            | CN68XX_INPUT_CTL_USE_CSR )
#else
#define    CN68XX_INPUT_CTL_MASK                    \
           ( CN68XX_INPUT_CTL_DATA_ES_64B_SWAP      \
            | CN68XX_INPUT_CTL_USE_CSR              \
            | CN68XX_INPUT_CTL_GATHER_ES_64B_SWAP)
#endif



/*############################ OUTPUT QUEUE #########################*/

/* 32 registers for Output queue buffer and info size - SLI_PKT0_OUT_SIZE */
#define    CN68XX_SLI_OQ0_BUFF_INFO_SIZE         0x0C00

/* 32 registers for Output Queue Start Addr - SLI_PKT0_SLIST_BADDR */
#define    CN68XX_SLI_OQ_BASE_ADDR_START64       0x1400

/* 32 registers for Output Queue Packet Credits - SLI_PKT0_SLIST_BAOFF_DBELL */
#define    CN68XX_SLI_OQ_PKT_CREDITS_START       0x1800

/* 32 registers for Output Queue size - SLI_PKT0_SLIST_FIFO_RSIZE */
#define    CN68XX_SLI_OQ_SIZE_START              0x1C00

/* 32 registers for Output Queue Packet Count - SLI_PKT0_CNTS */
#define    CN68XX_SLI_OQ_PKT_SENT_START          0x2400


/* Each Output Queue register is at a 16-byte Offset in BAR0 */
#define    CN68XX_OQ_OFFSET                      0x10



/* 1 register (32-bit) - 1 bit for each output queue
   - Relaxed Ordering setting for reading Output Queues descriptors
   - SLI_PKT_SLIST_ROR */
#define    CN68XX_SLI_PKT_SLIST_ROR              0x1030

/* 1 register (32-bit) - 1 bit for each output queue
   - No Snoop mode for reading Output Queues descriptors
   - SLI_PKT_SLIST_NS */
#define    CN68XX_SLI_PKT_SLIST_NS               0x1040

/* 1 register (64-bit) - 2 bits for each output queue
   - Endian-Swap mode for reading Output Queue descriptors
   - SLI_PKT_SLIST_ES */
#define    CN68XX_SLI_PKT_SLIST_ES64             0x1050

/* 1 register (32-bit) - 1 bit for each output queue
   - InfoPtr mode for Output Queues.
   - SLI_PKT_IPTR */
#define    CN68XX_SLI_PKT_IPTR                   0x1070

/* 1 register (32-bit) - 1 bit for each output queue
   - DPTR format selector for Output queues.
   - SLI_PKT_DPADDR */
#define    CN68XX_SLI_PKT_DPADDR                 0x1080

/* 1 register (32-bit) - 1 bit for each output queue
   - Relaxed Ordering setting for reading Output Queues data
   - SLI_PKT_DATA_OUT_ROR */
#define    CN68XX_SLI_PKT_DATA_OUT_ROR           0x1090

/* 1 register (32-bit) - 1 bit for each output queue
   - No Snoop mode for reading Output Queues data
   - SLI_PKT_DATA_OUT_NS */
#define    CN68XX_SLI_PKT_DATA_OUT_NS            0x10A0

/* 1 register (64-bit)  - 2 bits for each output queue
   - Endian-Swap mode for reading Output Queue data
   - SLI_PKT_DATA_OUT_ES */
#define    CN68XX_SLI_PKT_DATA_OUT_ES64          0x10B0

/* 1 register (32-bit) - 1 bit for each output queue
   - Controls whether SLI_PKTn_CNTS is incremented for bytes or for packets.
   - SLI_PKT_OUT_BMODE */
#define    CN68XX_SLI_PKT_OUT_BMODE              0x10D0

/* 1 register (64-bit) - 2 bits for each output queue
   - Assign PCIE port for Output queues
   - SLI_PKT_PCIE_PORT. */
#define    CN68XX_SLI_PKT_PCIE_PORT64            0x10E0



/* 1 (64-bit) register for Output Queue Packet Count Interrupt Threshold
   & Time Threshold. The same setting applies to all 32 queues.
   The register is defined as a 64-bit registers, but we use the
   32-bit offsets to define distinct addresses. */
#define    CN68XX_SLI_OQ_INT_LEVEL_PKTS          0x1120
#define    CN68XX_SLI_OQ_INT_LEVEL_TIME          0x1124


/* 1 (64-bit register) for Output Queue backpressure across all rings. */
#define    CN68XX_SLI_OQ_WMARK                   0x1180


#define    CN68XX_SLI_TX_PIPE                    0x1230



/*------- Output Queue Macros ---------*/
#define    CN68XX_SLI_OQ_BASE_ADDR64(oq)          \
       (CN68XX_SLI_OQ_BASE_ADDR_START64 + ((oq) * CN68XX_OQ_OFFSET))

#define    CN68XX_SLI_OQ_SIZE(oq)                 \
       (CN68XX_SLI_OQ_SIZE_START + ((oq) * CN68XX_OQ_OFFSET))

#define    CN68XX_SLI_OQ_BUFF_INFO_SIZE(oq)                 \
       (CN68XX_SLI_OQ0_BUFF_INFO_SIZE + ((oq) * CN68XX_OQ_OFFSET))


#define    CN68XX_SLI_OQ_PKTS_SENT(oq)            \
       (CN68XX_SLI_OQ_PKT_SENT_START + ((oq) * CN68XX_OQ_OFFSET))

#define    CN68XX_SLI_OQ_PKTS_CREDIT(oq)          \
       (CN68XX_SLI_OQ_PKT_CREDITS_START + ((oq) * CN68XX_OQ_OFFSET))







/*######################### DMA Counters #########################*/

/* 2 registers (64-bit) - DMA Count - 1 for each DMA counter 0/1. */
#define    CN68XX_DMA_CNT_START                   0x0400

/* 2 registers (64-bit) - DMA count & Time Interrupt threshold -
   SLI_DMA_0_INT_LEVEL */
#define    CN68XX_DMA_INT_LEVEL_START             0x03E0

/* Each DMA register is at a 16-byte Offset in BAR0 */
#define    CN68XX_DMA_OFFSET                      0x10


/*---------- DMA Counter Macros ---------*/
#define    CN68XX_DMA_CNT(dq)                      \
           (CN68XX_DMA_CNT_START + ((dq) * CN68XX_DMA_OFFSET))

#define    CN68XX_DMA_INT_LEVEL(dq)                \
           (CN68XX_DMA_INT_LEVEL_START + ((dq) * CN68XX_DMA_OFFSET))

#define    CN68XX_DMA_PKT_INT_LEVEL(dq)            \
           (CN68XX_DMA_INT_LEVEL_START + ((dq) * CN68XX_DMA_OFFSET))

#define    CN68XX_DMA_TIME_INT_LEVEL(dq)           \
           (CN68XX_DMA_INT_LEVEL_START + 4 + ((dq) * CN68XX_DMA_OFFSET))








/*######################## INTERRUPTS #########################*/

/* 1 register (64-bit) for Interrupt Summary */
#define    CN68XX_SLI_INT_SUM64                  0x0330


/* 1 register (64-bit) for Interrupt Enable */
#define    CN68XX_SLI_INT_ENB64_PORT0            0x0340
#define    CN68XX_SLI_INT_ENB64_PORT1            0x0350

/* 1 register (32-bit) to enable Output Queue Packet/Byte Count Interrupt */
#define    CN68XX_SLI_PKT_CNT_INT_ENB            0x1150


/* 1 register (32-bit) to enable Output Queue Packet Timer Interrupt */
#define    CN68XX_SLI_PKT_TIME_INT_ENB           0x1160

/* 1 register (32-bit) to indicate which Output Queue reached pkt threshold */
#define    CN68XX_SLI_PKT_CNT_INT                0x1130

/* 1 register (32-bit) to indicate which Output Queue reached time threshold */
#define    CN68XX_SLI_PKT_TIME_INT               0x1140





/*------------------ Interrupt Masks ----------------*/

#define    CN68XX_INTR_RML_TIMEOUT_ERR           (1)

#define    CN68XX_INTR_RESERVED1                 (1 << 1)

#define    CN68XX_INTR_BAR0_RW_TIMEOUT_ERR       (1 << 2)
#define    CN68XX_INTR_IO2BIG_ERR                (1 << 3)

#define    CN68XX_INTR_PKT_COUNT                 (1 << 4)
#define    CN68XX_INTR_PKT_TIME                  (1 << 5)

#define    CN68XX_INTR_RESERVED2                 (3 << 6)

#define    CN68XX_INTR_M0UPB0_ERR                (1 << 8)
#define    CN68XX_INTR_M0UPWI_ERR                (1 << 9)
#define    CN68XX_INTR_M0UNB0_ERR                (1 << 10)
#define    CN68XX_INTR_M0UNWI_ERR                (1 << 11)
#define    CN68XX_INTR_M1UPB0_ERR                (1 << 12)
#define    CN68XX_INTR_M1UPWI_ERR                (1 << 13)
#define    CN68XX_INTR_M1UNB0_ERR                (1 << 14)
#define    CN68XX_INTR_M1UNWI_ERR                (1 << 15)

#define    CN68XX_INTR_MIO_INT0                  (1 << 16)
#define    CN68XX_INTR_MIO_INT1                  (1 << 17)

#define    CN68XX_INTR_MAC_INT0                  (1 << 18)
#define    CN68XX_INTR_MAC_INT1                  (1 << 19)

#define    CN68XX_INTR_RESERVED3                 (0xFFF << 20)

#define    CN68XX_INTR_DMA0_FORCE                (1ULL << 32)
#define    CN68XX_INTR_DMA1_FORCE                (1ULL << 33)

#define    CN68XX_INTR_DMA0_COUNT                (1ULL << 34)
#define    CN68XX_INTR_DMA1_COUNT                (1ULL << 35)

#define    CN68XX_INTR_DMA0_TIME                 (1ULL << 36)
#define    CN68XX_INTR_DMA1_TIME                 (1ULL << 37)

#define    CN68XX_INTR_RESERVED4                 (0x3FFULL << 38)

#define    CN68XX_INTR_INSTR_DB_OF_ERR           (1ULL << 48)
#define    CN68XX_INTR_SLIST_DB_OF_ERR           (1ULL << 49)
#define    CN68XX_INTR_POUT_ERR                  (1ULL << 50)
#define    CN68XX_INTR_PIN_BP_ERR                (1ULL << 51)
#define    CN68XX_INTR_PGL_ERR                   (1ULL << 52)
#define    CN68XX_INTR_PDI_ERR                   (1ULL << 53)
#define    CN68XX_INTR_POP_ERR                   (1ULL << 54)
#define    CN68XX_INTR_PINS_ERR                  (1ULL << 55)
#define    CN68XX_INTR_SPRT0_ERR                 (1ULL << 56)
#define    CN68XX_INTR_SPRT1_ERR                 (1ULL << 57)

#define    CN68XX_INTR_RESERVED5                 (3ULL << 58)

#define    CN68XX_INTR_ILL_PAD_ERR               (1ULL << 60)

#define    CN68XX_INTR_PIPE_ERR                  (1ULL << 61)



#define    CN68XX_INTR_DMA0_DATA                 (CN68XX_INTR_DMA0_TIME)

#define    CN68XX_INTR_DMA1_DATA                 (CN68XX_INTR_DMA1_TIME )

#define    CN68XX_INTR_DMA_DATA                  \
            (CN68XX_INTR_DMA0_DATA | CN68XX_INTR_DMA1_DATA )

#define    CN68XX_INTR_PKT_DATA                  (CN68XX_INTR_PKT_TIME)


/* Sum of interrupts for all PCI-Express Data Interrupts */
#define    CN68XX_INTR_PCIE_DATA                 \
           ( CN68XX_INTR_DMA_DATA | CN68XX_INTR_PKT_DATA )


#define    CN68XX_INTR_MIO                       \
           ( CN68XX_INTR_MIO_INT0 | CN68XX_INTR_MIO_INT1 )

#define    CN68XX_INTR_MAC                       \
           ( CN68XX_INTR_MAC_INT0 | CN68XX_INTR_MAC_INT1 )


/* Sum of interrupts for error events */
#define    CN68XX_INTR_ERR                       \
           (  CN68XX_INTR_BAR0_RW_TIMEOUT_ERR    \
            | CN68XX_INTR_IO2BIG_ERR             \
            | CN68XX_INTR_M0UPB0_ERR             \
            | CN68XX_INTR_M0UPWI_ERR             \
            | CN68XX_INTR_M0UNB0_ERR             \
            | CN68XX_INTR_M0UNWI_ERR             \
            | CN68XX_INTR_M1UPB0_ERR             \
            | CN68XX_INTR_M1UPWI_ERR             \
            | CN68XX_INTR_M1UPB0_ERR             \
            | CN68XX_INTR_M1UNWI_ERR             \
            | CN68XX_INTR_INSTR_DB_OF_ERR        \
            | CN68XX_INTR_SLIST_DB_OF_ERR        \
            | CN68XX_INTR_POUT_ERR               \
            | CN68XX_INTR_PIN_BP_ERR             \
            | CN68XX_INTR_PGL_ERR                \
            | CN68XX_INTR_PDI_ERR                \
            | CN68XX_INTR_POP_ERR                \
            | CN68XX_INTR_PINS_ERR               \
            | CN68XX_INTR_SPRT0_ERR              \
            | CN68XX_INTR_SPRT1_ERR              \
            | CN68XX_INTR_ILL_PAD_ERR)



/* Programmed Mask for Interrupt Sum */
#ifdef CVMCS_DMA_IC
#define    CN68XX_INTR_MASK                      \
           (  CN68XX_INTR_PCIE_DATA              \
            | CN68XX_INTR_DMA0_COUNT             \
            | CN68XX_INTR_DMA0_TIME             \
            | CN68XX_INTR_MIO                    \
            | CN68XX_INTR_MAC                    \
            | CN68XX_INTR_ERR)
#else
#define    CN68XX_INTR_MASK                      \
           (  CN68XX_INTR_PCIE_DATA              \
            | CN68XX_INTR_DMA0_FORCE             \
            | CN68XX_INTR_DMA1_FORCE             \
            | CN68XX_INTR_MIO                    \
            | CN68XX_INTR_MAC                    \
            | CN68XX_INTR_ERR)
#endif




#define    CN68XX_SLI_S2M_PORT0_CTL              0x3D80
#define    CN68XX_SLI_S2M_PORT1_CTL              0x3D90
#define    CN68XX_SLI_S2M_PORTX_CTL(port)        \
			(CN68XX_SLI_S2M_PORT0_CTL + (port * 0x10))

#define    CN68XX_SLI_INT_ENB64(port)            \
            (CN68XX_SLI_INT_ENB64_PORT0 + (port * 0x10))


#define    CN68XX_SLI_MAC_NUMBER                 0x3E00





/* CN63XX BAR1 Index registers. */
#define    CN68XX_PEM_BAR1_INDEX000                0x00011800C00000A8ULL

#define    CN68XX_BAR1_INDEX_START                 CN68XX_PEM_BAR1_INDEX000
#define    CN68XX_PCI_BAR1_OFFSET                  0x8

#define    CN68XX_BAR1_INDEX_REG(idx)              \
           (CN68XX_BAR1_INDEX_START + (CN68XX_PCI_BAR1_OFFSET * (idx)))






/*############################ DPI #########################*/

#define    CN68XX_DPI_CTL                 0x0001df0000000040ULL

#define    CN68XX_DPI_DMA_CONTROL         0x0001df0000000048ULL

#define    CN68XX_DPI_REQ_GBL_ENB         0x0001df0000000050ULL

#define    CN68XX_DPI_REQ_ERR_RSP         0x0001df0000000058ULL

#define    CN68XX_DPI_REQ_ERR_RST         0x0001df0000000060ULL


#define    CN68XX_DPI_DMA_ENG0_ENB        0x0001df0000000080ULL

#define    CN68XX_DPI_DMA_ENG_ENB(q_no)   \
              (CN68XX_DPI_DMA_ENG0_ENB + (q_no * 8))

#define    CN68XX_DPI_DMA_ENG0_BUF        0x0001df0000000880ULL

#define    CN68XX_DPI_DMA_ENG_BUF(q_no)   \
              (CN68XX_DPI_DMA_ENG0_BUF + (q_no * 8))

#define    CN68XX_DPI_SLI_PRT0_CFG        0x0001df0000000900ULL
#define    CN68XX_DPI_SLI_PRT1_CFG        0x0001df0000000908ULL
#define    CN68XX_DPI_SLI_PRTX_CFG(port)        \
			(CN68XX_DPI_SLI_PRT0_CFG + (port * 0x10))


#define    CN68XX_DPI_DMA_COMMIT_MODE     (1ULL << 58)
#define    CN68XX_DPI_DMA_PKT_HP          (1ULL << 57)
#define    CN68XX_DPI_DMA_PKT_EN          (1ULL << 56)
#define    CN68XX_DPI_DMA_ENB             (0x1FULL << 48)
#define    CN68XX_DPI_DMA_O_ES            (1 << 15)
#define    CN68XX_DPI_DMA_O_MODE          (1 << 14)

#ifdef CVMCS_DMA_IC
/* Set the DMA Control, to update packet count not byte count sent by DMA,
   when we use Interrupt Coalescing (CA mode) */
#define    CN68XX_DPI_DMA_O_ADD1	  (1 << 19)

#define    CN68XX_DPI_DMA_CTL_MASK             \
           (CN68XX_DPI_DMA_COMMIT_MODE    |    \
            CN68XX_DPI_DMA_PKT_HP         |    \
            CN68XX_DPI_DMA_PKT_EN         |    \
            CN68XX_DPI_DMA_O_ADD1         |    \
            CN68XX_DPI_DMA_O_ES           |    \
            CN68XX_DPI_DMA_O_MODE)
#else
#define    CN68XX_DPI_DMA_CTL_MASK             \
           (CN68XX_DPI_DMA_COMMIT_MODE    |    \
            CN68XX_DPI_DMA_PKT_HP         |    \
            CN68XX_DPI_DMA_PKT_EN         |    \
            CN68XX_DPI_DMA_O_ES           |    \
            CN68XX_DPI_DMA_O_MODE)
#endif

/*############################ CIU #########################*/

#define    CN68XX_CIU_SOFT_BIST           0x0001070000000738ULL
#define    CN68XX_CIU_SOFT_RST            0x0001070000000740ULL

/*############################ MIO #########################*/

#define    CN68XX_MIO_RST_BOOT            0x0001180000001600ULL





#endif

/* $Id:$ */
