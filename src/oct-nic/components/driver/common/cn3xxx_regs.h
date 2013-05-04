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




/*! \file   cn38xx_regs.h
    \brief  Host Driver: Octeon register addresses/offsets for CN38XX/CN58XX.
*/

#ifndef __CN38XX_REGS_H__
#define __CN38XX_REGS_H__


/*
  ##############  PCI Config Registers ###############
*/
#define    PCI_CFG_COMMAND_STATUS        0x04
#define    PCI_CFG_CACHE_LINE            0x0C
#define    PCI_CFG_TGT_IMPL              0x40
#define    PCI_CFG_19                    0x4C
#define    PCI_CFG_MASTER_ARB_CTL        0x58
#define    PCI_CFG_CAPABILITIES          0xE0





/*
  ##############  BAR0 Registers ################
*/
#define    PCI_BAR1_INDEX_START          0x100
#define    PCI_BAR1_OFFSET               0x4

/*------------------ REQUEST QUEUE ---------------*/
#define    PCI_IQ_DOORBELL_START         0x080
#define    PCI_IQ_INSTR_COUNT_START      0x84
#define    PCI_IQ_OFFSET                 0x8


/*------------------ OUTPUT QUEUE ----------------*/
#define    PCI_PKTS_SENT_START           0x040
#define    PCI_PKTS_CREDITS_START        0x044
#define    PCI_PKTS_SENT_INT_LEV_START   0x048
#define    PCI_PKTS_SENT_TIME_START      0x04C
#define    PCI_PKTS_OFFSET               0x10


/*-------------------- DMA QUEUE -----------------
   No macros are defined for these registers since the definition for 
   38XX and 56XX are different. 56XX has a 1 register for both Time and
   Pkt count interrupt and 1 register for both DMA 0 & 1 Counts.
*/
#define    PCI_DMA_CNT_START             0x0A0
#define    PCI_DMA_INT_LEV_START         0x0A4
#define    PCI_DMA_TIME_START            0x0B0

#define    CN3XXX_DMA_CNT(dq)                      \
           (PCI_DMA_CNT_START + (dq * 8))

#define    CN3XXX_DMA_PKT_INT_LEVEL(dq)            \
           (PCI_DMA_INT_LEV_START + (dq * 8))

#define    CN3XXX_DMA_TIME_INT_LEVEL(dq)           \
           (PCI_DMA_TIME_START + (dq * 4))




/*----------------- INTERRUPTS -------------------*/
#define    PCI_INT_SUM                   0x030
#define    PCI_INT_ENABLE                0x038

#define    PCI_INT_SUM_2                 0x198
#define    PCI_INT_ENABLE_2              0x1A0


/*----------PCI Windowed Register Access----------*/

#define    CN3XXX_WIN_RD_ADDR_LO            0x008
#define    CN3XXX_WIN_RD_ADDR_HI            0x00C
#define    CN3XXX_WIN_RD_DATA_LO            0x020
#define    CN3XXX_WIN_RD_DATA_HI            0x024

#define    CN3XXX_WIN_WR_ADDR_LO            0x000
#define    CN3XXX_WIN_WR_ADDR_HI            0x004
#define    CN3XXX_WIN_WR_DATA_LO            0x010
#define    CN3XXX_WIN_WR_DATA_HI            0x014
#define    CN3XXX_WIN_WR_MASK_REG           0x018

/*----------  PCI_CNT_REG (CN58XX Only) ----------*/
#define    PCI_CNT_REG_LO                0x1B8
#define    PCI_CNT_REG_HI                0x1BC

#define    PCI_CTL_STATUS_2              0x18C










/*
  ##############  NPI Registers ################
*/
 

#define    NPI_PCI_READ_TIMEOUT          0x00011F00000000B0ULL

#define    NPI_INPUT_CONTROL             0x00011F0000000138ULL
#define    NPI_OUTPUT_CONTROL            0x00011F0000000100ULL
#define    NPI_DMA_CONTROL               0x00011F0000000128ULL
#define    NPI_CTL_STATUS                0x00011F0000000010ULL
#define    NPI_PCI_BURST_SIZE            0x00011F00000000D8ULL



/*--NPI windowed registers  for input --*/
#define    NPI_IQ_BASE_ADDR_START        0x00011F0000000070ULL
#define    NPI_IQ_SIZE_START             0x00011F0000000078ULL
#define    NPI_IQ_INSTR_HDR_START        0x00011F00000001F8ULL
#define    NPI_IQ_OFFSET                 0x10

/*--NPI windowed registers  for output --*/
#define    NPI_OQ_BASE_ADDR_START        0x00011F00000000B8ULL
#define    NPI_OQ_NUM_DESC_START         0x00011F0000000050ULL
#define    NPI_OQ_BUFF_SIZE_START        0x00011F00000000E0ULL
#define    NPI_OQ_OFFSET                 0x8


/*--NPI windowed registers for DMA --*/
#define    NPI_DMA_LOWP_COUNTS           0x00011F0000000140ULL
#define    NPI_DMA_HIP_COUNTS            0x00011F0000000148ULL 
#define    NPI_DMA_LOWP_NADDR            0x00011F0000000150ULL 
#define    NPI_DMA_HIP_NADDR             0x00011F0000000158ULL 
#define    NPI_DMA_LOWP_IBUFF_SADDR      0x00011F0000000108ULL 
#define    NPI_DMA_HIP_IBUFF_SADDR       0x00011F0000000110ULL 



/* NPI DEBUG REGISTERS */
#define    NPI_INSTR_CNTS_DEBUG_BASE     0x00011F00000001A0ULL
#define    NPI_INSTR_ADDR_DEBUG_BASE     0x00011F00000001C0ULL
#define    NPI_INPUT_DEBUG_OFFSET        8

#define    NPI_DBPAIR_ADDR_DEBUG_BASE    0x00011F0000000180ULL
#define    NPI_PAIR_CNTS_DEBUG_BASE      0x00011F0000000160ULL



#define    PCI_SOFT_RESET_DONE_MASK      0x00080000



/*----------------  CIU registers ----------------*/
#define    CN3XXX_CIU_PP_RST                     0x0001070000000700ULL
#define    CN3XXX_CIU_SOFT_BIST                  0x0001070000000738ULL
#define    CN3XXX_CIU_SOFT_PRST                  0x0001070000000748ULL
#define    CN3XXX_CIU_SOFT_RST                   0x0001070000000740ULL


/*---------- TWSI/MIO Registers -------------------*/

#define    MIO_BOOT_REG_CFG0              0x0001180000000000ULL
#define    MIO_BOOT_LOC_CFG0              0x0001180000000080ULL
#define    MIO_BOOT_LOC_ADR               0x0001180000000090ULL
#define    MIO_BOOT_LOC_DAT               0x0001180000000098ULL
#define    MIO_TWS_SW_TWSI                0x0001180000001000ULL
#define    MIO_TWS_TWSI_SW                0x0001180000001008ULL
#define    MIO_TWS_INT                    0x0001180000001010ULL
#define    MIO_TWS_SW_TWSI_EXT            0x0001180000001018ULL




/*-------------------------  Macros  --------------------------*/ 

#define    OCT_IQ_BASE_ADDR_REG(q_no)           \
           (NPI_IQ_BASE_ADDR_START + (NPI_IQ_OFFSET * q_no))

#define    OCT_IQ_SIZE_REG(q_no)                \
           (NPI_IQ_SIZE_START + (NPI_IQ_OFFSET * q_no))

#define    OCT_IQ_INSTR_HDR_REG(q_no)           \
           (NPI_IQ_INSTR_HDR_START + (NPI_IQ_OFFSET * q_no))

#define    OCT_OQ_BASE_ADDR_REG(q_no)           \
           (NPI_OQ_BASE_ADDR_START +  (NPI_OQ_OFFSET * q_no))

#define    OCT_OQ_SIZE_REG(q_no)                \
           (NPI_OQ_NUM_DESC_START +  (NPI_OQ_OFFSET * q_no))

#define    OCT_OQ_BUFF_SIZE_REG(q_no)           \
           (NPI_OQ_BUFF_SIZE_START +  (NPI_OQ_OFFSET * q_no))





#define    OCT_BAR1_INDEX_REG(idx)              \
           (PCI_BAR1_INDEX_START + (PCI_BAR1_OFFSET * idx))

#define    OCT_IQ_DOORBELL_REG(q_no)            \
           (PCI_IQ_DOORBELL_START + (PCI_IQ_OFFSET * q_no))

#define    OCT_IQ_COUNT_REG(q_no)               \
           (PCI_IQ_INSTR_COUNT_START + (PCI_IQ_OFFSET * q_no))

#define    OCT_OQ_PKTS_SENT_REG(q_no)           \
           (PCI_PKTS_SENT_START + (PCI_PKTS_OFFSET * q_no))

#define    OCT_OQ_PKTS_CREDITS_REG(q_no)        \
           (PCI_PKTS_CREDITS_START + (PCI_PKTS_OFFSET * q_no))

#define    OCT_OQ_INTR_COUNT_LEV_REG(q_no)      \
           (PCI_PKTS_SENT_INT_LEV_START + (PCI_PKTS_OFFSET * q_no))

#define    OCT_OQ_INTR_TIME_LEV_REG(q_no)       \
           (PCI_PKTS_SENT_TIME_START + (PCI_PKTS_OFFSET * q_no))












#define    PCI_CTL_STATUS_2_PCIX_MODE    0x00002000
#define    PCI_CTL_STATUS_2_64BIT_BUS    0x00001000
#define    READ_BURST_SIZE               64
#define    WRITE_BURST_SIZE              0   /* Write as 0 to set to 128 */
#define    PCI_BURST_SIZE        ((WRITE_BURST_SIZE << 7) | READ_BURST_SIZE )

/* USE CSR for ES, NS, RO. No snoop or relaxed ordering.
   Endian-swap data. Round-robin priority for queues. */
#if  __CAVIUM_BYTE_ORDER == __CAVIUM_LITTLE_ENDIAN
#define    NPI_INPUT_CTL_MASK            (0x0000000000000050ULL  | (1 << 22))
#else
/* For big-endian systems, enable 64-bit endian-swap for gather list and
   instruction reads. */
#define    NPI_INPUT_CTL_MASK            (0x0000000000000052ULL  | (1 << 22)) 
#endif
#define    NPI_CTL_ENABLE_ALL_INPUT_MASK 0x0003C00000000000ULL



/*------------------ OUTPUT QUEUE ----------------*/

/* USE CSR for ES, NS, RO. No snoop or relaxed ordering. endian-swap data */
/* Use info-ptr to store len/data for Output port */
#if  __CAVIUM_BYTE_ORDER == __CAVIUM_LITTLE_ENDIAN
#define    NPI_OUTPUT_CONTROL_MASK_0     0x0000000041010000ULL
#define    NPI_OUTPUT_CONTROL_MASK_1     0x0000000402020000ULL
#define    NPI_OUTPUT_CONTROL_MASK_2     0x0000004004040000ULL
#define    NPI_OUTPUT_CONTROL_MASK_3     0x0000040008080000ULL
#else
/* For big-endian systems, enable 64-bit swap for slist[0..3] reads */
#define    NPI_OUTPUT_CONTROL_MASK_0     0x0000000041010004ULL
#define    NPI_OUTPUT_CONTROL_MASK_1     0x0000000402020040ULL
#define    NPI_OUTPUT_CONTROL_MASK_2     0x0000004004040400ULL
#define    NPI_OUTPUT_CONTROL_MASK_3     0x0000040008084000ULL
#endif

/* In addition, also enable Round-robin priority for queues. */
#define    NPI_OUTPUT_CTL_MASK    \
       ( (1ULL << 48) |  NPI_OUTPUT_CONTROL_MASK_0 | NPI_OUTPUT_CONTROL_MASK_1 \
                | NPI_OUTPUT_CONTROL_MASK_2 | NPI_OUTPUT_CONTROL_MASK_3 )

#define    NPI_CTL_ENABLE_ALL_OUTPUT_MASK  0x003C000000000000ULL



/*   DROQ interrupts */
#define    DROQ0_CNT_INT             0x00020000
#define    DROQ0_TIME_INT            0x00200000
#define    PCI_INT_DROQ0_MASK   ( DROQ0_TIME_INT )

#define    DROQ1_CNT_INT             0x00040000
#define    DROQ1_TIME_INT            0x00400000
#define    PCI_INT_DROQ1_MASK   ( DROQ1_TIME_INT )

#define    DROQ2_CNT_INT             0x00080000
#define    DROQ2_TIME_INT            0x00800000
#define    PCI_INT_DROQ2_MASK   ( DROQ2_TIME_INT )

#define    DROQ3_CNT_INT             0x00100000
#define    DROQ3_TIME_INT            0x01000000
#define    PCI_INT_DROQ3_MASK   ( DROQ3_TIME_INT )


#define    PCI_INT_DROQ_MASK    \
            ( PCI_INT_DROQ0_MASK  | PCI_INT_DROQ1_MASK | PCI_INT_DROQ2_MASK \
              | PCI_INT_DROQ3_MASK )




#define    DMA0_CNT_INT             0x02000000
#define    DMA0_TIME_INT            0x08000000
#define    DMA0_FORCE_INT           0x20000000
#define    DMA1_CNT_INT             0x04000000
#define    DMA1_TIME_INT            0x10000000
#define    DMA1_FORCE_INT           0x40000000

#define    PCI_INT_DMA0_MASK    ( DMA0_TIME_INT )
#define    PCI_INT_DMA1_MASK    ( DMA1_TIME_INT )

#define    PCI_DMA_FORCE_INT    (DMA0_FORCE_INT | DMA1_FORCE_INT)


/* PCI_INT_ENB_MASK will not have force int enabled by default.
 insmod enables force int in addition to int_enb_mask */ 
#define    PCI_INT_DMA_MASK     (PCI_INT_DMA0_MASK | PCI_INT_DMA1_MASK)






/* MSI specific interrupt summaries are not enabled */
#define    PCI_INT_PCI_ERR_MASK          0x0000FFFF



/*  Interrupt Enable Mask. */
/* Above Error mask + all summary for o/p queue and DMA q enabled.
   RSL_INT summary bit is not enabled. */
#define    PCI_INT_ENB_MASK   \
           ( PCI_INT_DROQ_MASK | PCI_INT_DMA_MASK | PCI_INT_PCI_ERR_MASK )



/*---------------   PCI BAR1 index registers -------------*/

/* BAR1 Mask */
#define    PCI_BAR1_ENABLE_CA            1
#define    PCI_BAR1_ENDIAN_MODE          OCTEON_PCI_64BIT_SWAP 
#define    PCI_BAR1_ENTRY_VALID          1
#define    PCI_BAR1_MASK                 (  (PCI_BAR1_ENABLE_CA << 3)   \
                                          | (PCI_BAR1_ENDIAN_MODE << 1) \
                                          | PCI_BAR1_ENTRY_VALID )

#define    INVALID_MAP    0xffff

#endif    /* __CN38XX_REGS_H__ */

/* $Id: cn3xxx_regs.h 53786 2010-10-08 22:09:32Z panicker $ */
