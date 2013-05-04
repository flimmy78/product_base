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



/*! \file  octeon_config.h 
    \brief Host Driver: Configuration data structures for the host driver.
*/


#ifndef __OCTEON_CONFIG_H__
#define __OCTEON_CONFIG_H__



/* For CN58XX, the PCI bus speed is auto-detected. For CN3XXX, the driver
   uses the value set here as the bus speed. */
#define OCTEON_PCI_BUS_HZ            133



/*--------------------------CONFIG VALUES------------------------*/
/*
    The following variables affect the way the driver data structures 
    are generated for Octeon devices.
    They can be modified.
*/

/*  Maximum no. of octeon devices that the driver can support. */
#define   MAX_OCTEON_DEVICES           4


#define   CN3XXX_MAX_INPUT_QUEUES      4
#define   CN3XXX_MAX_OUTPUT_QUEUES     4

#define   CN56XX_MAX_INPUT_QUEUES      32
#define   CN56XX_MAX_OUTPUT_QUEUES     32

#define   CN63XX_MAX_INPUT_QUEUES      32
#define   CN63XX_MAX_OUTPUT_QUEUES     32

#define   CN66XX_MAX_INPUT_QUEUES      32
#define   CN66XX_MAX_OUTPUT_QUEUES     32

#define   CN68XX_MAX_INPUT_QUEUES      32
#define   CN68XX_MAX_OUTPUT_QUEUES     32



#define   OCTEON_CONFIG_TYPE_DEFAULT          1
#define   OCTEON_CONFIG_TYPE_CN3XXX_CUSTOM    2
#define   OCTEON_CONFIG_TYPE_CN56XX_CUSTOM    3
#define   OCTEON_CONFIG_TYPE_CN63XX_CUSTOM    4
#define   OCTEON_CONFIG_TYPE_CN66XX_CUSTOM    5
#define   OCTEON_CONFIG_TYPE_CN68XX_CUSTOM    6



/** Structure for global configuration attributes that are common across
	all Octeon processors. */
typedef struct {

	/** Number of Input queues (usually 4). */
	uint16_t   num_iqs;

	/** Number of Output queues (usually 4) */
	uint16_t   num_oqs;

	/** Pending list size (usually set to the sum of the size of all Input
	    queues) */
	uint32_t   pending_list_size;

} octeon_common_config_t;




/** Structure to define the configuration attributes for each Input queue.
	Applicable to all Octeon processors */
typedef struct {

	/** Size of the Input queue (number of commands) */
	uint32_t   num_descs;

	/** Command size - 32 or 64 bytes */
	uint32_t   instr_type;

	/** Minimum number of commands pending to be posted to Octeon before driver
	    hits the Input queue doorbell. */
	uint32_t   db_min;

	/** Minimum ticks to wait before checking for pending instructions. */
	uint32_t   db_timeout;

} octeon_iq_config_t;









/** Structure to define configuration attributes for each Output queue
	in the CN38XX/CN58XX Octeon processors. */
typedef struct {

	/** Size of Output queue (number of descriptors) */
	uint32_t   num_descs;

	/** Size of buffer in this Output queue. */
	uint32_t   buf_size;

	/** If set, the Output queue uses info-pointer mode. (Default: 1 ) */
	uint32_t   info_ptr;

	/** Number of packets to be processed by driver tasklet every invocation
	    for this Output queue. */
	uint32_t   pkts_per_intr;

	/** Interrupt Coalescing (Packet Count). Octeon will interrupt the host
	    only if it sent as many packets as specified by this field. The driver
	    usually does not use packet count interrupt coalescing. */
	uint32_t   intr_pkt;

	/** Interrupt Coalescing (Time Interval). Octeon will interrupt the host
	    if atleast one packet was sent in the time interval specified by this
	    field. The driver uses time interval interrupt coalescing by default.
	    The time is specified in microseconds. */
	uint32_t   intr_time;

	/** The number of buffers that were consumed during packet processing by
	    the driver on this Output queue before the driver attempts to replenish
	    the descriptor ring with new buffers. */
	uint32_t   refill_threshold;

} cn3xxx_oq_config_t;





/** Structure to define configuration attributes for each Output queue
	in the CN56XX Octeon processors. */
typedef struct {

	/** Size of Output queue (number of descriptors) */
	uint32_t   num_descs;

	/** If set, the Output queue uses info-pointer mode. (Default: 1 ) */
	uint32_t   info_ptr;

	/** Number of packets to be processed by driver tasklet every invocation
	    for this Output queue. */
	uint32_t   pkts_per_intr;

	/** The number of buffers that were consumed during packet processing by
	    the driver on this Output queue before the driver attempts to replenish
	    the descriptor ring with new buffers. */
	uint32_t   refill_threshold;

} cn56xx_oq_config_t;



/** Structure to define configuration attributes for each Output queue
	in the CN63XX & CN68XX Octeon processors. */
struct cn6xxx_oq_config {

	/** Size of Output queue (number of descriptors) */
	uint32_t   num_descs;

	/** If set, the Output queue uses info-pointer mode. (Default: 1 ) */
	uint32_t   info_ptr;

	/** Output queue buffer size */
	uint32_t   buf_size;

	/** Number of packets to be processed by driver tasklet every invocation
	    for this Output queue. */
	uint32_t   pkts_per_intr;

	/** The number of buffers that were consumed during packet processing by
	    the driver on this Output queue before the driver attempts to replenish
	    the descriptor ring with new buffers. */
	uint32_t   refill_threshold;

} ;


typedef struct cn6xxx_oq_config    cn63xx_oq_config_t;
typedef struct cn6xxx_oq_config    cn66xx_oq_config_t;
typedef struct cn6xxx_oq_config    cn68xx_oq_config_t;





/** Structure to define the configuration for CN38XX/CN58XX	Octeon processors.*/
typedef  struct {

	/** Common attributes. */
	octeon_common_config_t   c;

	/** Input Queue attributes. */
	octeon_iq_config_t       iq[CN3XXX_MAX_INPUT_QUEUES];

	/** Output Queue attributes. */
	cn3xxx_oq_config_t       oq[CN3XXX_MAX_OUTPUT_QUEUES];

} cn3xxx_config_t;






/** Structure to define the configuration for CN56XX Octeon processors. */
typedef  struct {

	/** Common attributes. */
	octeon_common_config_t  c;

	/** Input Queue attributes. */
	octeon_iq_config_t      iq[CN56XX_MAX_INPUT_QUEUES];

	/** Some of the Output Queue attributes. */
	cn56xx_oq_config_t      oq[CN56XX_MAX_OUTPUT_QUEUES];

	/** Output queue buffer size (All Output queue buffers are of same size). */
	uint32_t                oq_buf_size;

	/** Interrupt Coalescing (Packet Count). Octeon will interrupt the host
	    only if it sent as many packets as specified by this field. The driver
	    usually does not use packet count interrupt coalescing. 
	    All output queues have the same packet count setting. */
	uint32_t                oq_intr_pkt;

	/** Interrupt Coalescing (Time Interval). Octeon will interrupt the host
	    if atleast one packet was sent in the time interval specified by this
	    field. The driver uses time interval interrupt coalescing by default.
	    The time is specified in microseconds.
	    All output queues have the same time interval setting. */
	uint32_t                oq_intr_time;

#ifdef CVMCS_DMA_IC
        /** Interrupt Coalescing (Packet Count). Octeon will interrupt the host
         *  only if it DMAed as many packets as specified by this field. */
        uint32_t                dma_intr_pkt;

        /** Interrupt Coalescing (Time Interval). Octeon will interrupt the host
         *  if atleast one packet was DMAed in the time interval specified by 
         *  this field. */
        uint32_t                dma_intr_time;
#endif

} cn56xx_config_t;




/** Structure to define the configuration for CN63XX Octeon processors. */
typedef  struct {

	/** Common attributes. */
	octeon_common_config_t  c;

	/** Input Queue attributes. */
	octeon_iq_config_t      iq[CN63XX_MAX_INPUT_QUEUES];

	/** Some of the Output Queue attributes. */
	cn63xx_oq_config_t      oq[CN63XX_MAX_OUTPUT_QUEUES];

	/** Interrupt Coalescing (Packet Count). Octeon will interrupt the host
	    only if it sent as many packets as specified by this field. The driver
	    usually does not use packet count interrupt coalescing. 
	    All output queues have the same packet count setting. */
	uint32_t                oq_intr_pkt;

	/** Interrupt Coalescing (Time Interval). Octeon will interrupt the host
	    if atleast one packet was sent in the time interval specified by this
	    field. The driver uses time interval interrupt coalescing by default.
	    The time is specified in microseconds.
	    All output queues have the same time interval setting. */
	uint32_t                oq_intr_time;

#ifdef CVMCS_DMA_IC
	/** Interrupt Coalescing (Packet Count). Octeon will interrupt the host
	    only if it DMAed as many packets as specified by this field. */
	uint32_t                dma_intr_pkt;

	/** Interrupt Coalescing (Time Interval). Octeon will interrupt the host
	    if atleast one packet was DMAed in the time interval specified by this
	    field. */ 
	uint32_t                dma_intr_time;
#endif

} cn63xx_config_t;





/** Structure to define the configuration for CN66XX Octeon processors. */
typedef  struct {

	/** Common attributes. */
	octeon_common_config_t  c;

	/** Input Queue attributes. */
	octeon_iq_config_t      iq[CN66XX_MAX_INPUT_QUEUES];

	/** Some of the Output Queue attributes. */
	cn66xx_oq_config_t      oq[CN66XX_MAX_OUTPUT_QUEUES];

	/** Interrupt Coalescing (Packet Count). Octeon will interrupt the host
	    only if it sent as many packets as specified by this field. The driver
	    usually does not use packet count interrupt coalescing. 
	    All output queues have the same packet count setting. */
	uint32_t                oq_intr_pkt;

	/** Interrupt Coalescing (Time Interval). Octeon will interrupt the host
	    if atleast one packet was sent in the time interval specified by this
	    field. The driver uses time interval interrupt coalescing by default.
	    The time is specified in microseconds.
	    All output queues have the same time interval setting. */
	uint32_t                oq_intr_time;

#ifdef CVMCS_DMA_IC
	/** Interrupt Coalescing (Packet Count). Octeon will interrupt the host
	    only if it DMAed as many packets as specified by this field. */
	uint32_t                dma_intr_pkt;

	/** Interrupt Coalescing (Time Interval). Octeon will interrupt the host
	    if atleast one packet was DMAed in the time interval specified by this
	    field. */ 
	uint32_t                dma_intr_time;
#endif

} cn66xx_config_t;





/** Structure to define the configuration for CN68XX Octeon processors. */
typedef  struct {

	/** Common attributes. */
	octeon_common_config_t  c;

	/** Input Queue attributes. */
	octeon_iq_config_t      iq[CN68XX_MAX_INPUT_QUEUES];

	/** Some of the Output Queue attributes. */
	cn68xx_oq_config_t      oq[CN68XX_MAX_OUTPUT_QUEUES];

	/** Interrupt Coalescing (Packet Count). Octeon will interrupt the host
	    only if it sent as many packets as specified by this field. The driver
	    usually does not use packet count interrupt coalescing. 
	    All output queues have the same packet count setting. */
	uint32_t                oq_intr_pkt;

	/** Interrupt Coalescing (Time Interval). Octeon will interrupt the host
	    if atleast one packet was sent in the time interval specified by this
	    field. The driver uses time interval interrupt coalescing by default.
	    The time is specified in microseconds.
	    All output queues have the same time interval setting. */
	uint32_t                oq_intr_time;

#ifdef CVMCS_DMA_IC
	/** Interrupt Coalescing (Packet Count). Octeon will interrupt the host
	    only if it DMAed as many packets as specified by this field. */
	uint32_t                dma_intr_pkt;

	/** Interrupt Coalescing (Time Interval). Octeon will interrupt the host
	    if atleast one packet was DMAed in the time interval specified by this
	    field. */ 
	uint32_t                dma_intr_time;
#endif

} cn68xx_config_t;





/* Maximum number of ordered requests to check for completion. */
#define OCTEON_MAX_ORDERED_COMPLETION  16





/* 
   The following config values are fixed and should not be modified.
*/

/* Maximum address space to be mapped for Octeon's BAR1 index-based access. */
#define  MAX_BAR1_MAP_INDEX        2
#define  OCTEON_BAR1_ENTRY_SIZE   (4 * 1024 * 1024)

/* BAR1 Index 0 to (MAX_BAR1_MAP_INDEX - 1) for normal mapped memory access.
   Bar1 register at MAX_BAR1_MAP_INDEX used by driver for dynamic access. */
#define  MAX_BAR1_IOREMAP_SIZE    ((MAX_BAR1_MAP_INDEX + 1) * OCTEON_BAR1_ENTRY_SIZE)




/* Response lists - 1 ordered, 1 unordered-blocking, 1 unordered-nonblocking */
/* NoResponse Lists are now maintained with each IQ. (Dec' 2007). */
#define MAX_RESPONSE_LISTS           3

/* Number of buffer pools - always fixed at 6. */
#define BUF_POOLS                    6

/*-------------- Dispatch function lookup table -----------*/
/* Opcode hash bits. The opcode is hashed on the lower 6-bits to lookup the
   dispatch table. */
#define OPCODE_MASK_BITS             6

/* Mask for the 6-bit lookup hash */
#define OCTEON_OPCODE_MASK           0x3f

/* Size of the dispatch table. The 6-bit hash can index into 2^6 entries */
#define DISPATCH_LIST_SIZE           (1 << OPCODE_MASK_BITS) 




/*----------- The buffer pool -------------------*/


/* Number of  32k buffers. */
#define HUGE_BUFFER_CHUNKS              32

/* Number of  16k buffers. */
#define LARGE_BUFFER_CHUNKS             64

/* Number of  8k buffers. */
#define MEDIUM_BUFFER_CHUNKS            64

/* Number of  4k buffers. */
#define SMALL_BUFFER_CHUNKS             128

/* Number of  2k buffers. */
#define TINY_BUFFER_CHUNKS              512

/* Number of  1k buffers. */
#define EX_TINY_BUFFER_CHUNKS           1024



#define HUGE_BUFFER_CHUNK_SIZE          (32*1024)

#define LARGE_BUFFER_CHUNK_SIZE         (16*1024)

#define MEDIUM_BUFFER_CHUNK_SIZE        (8*1024)

#define SMALL_BUFFER_CHUNK_SIZE         (4*1024)

#define TINY_BUFFER_CHUNK_SIZE	        (2*1024)

#define EX_TINY_BUFFER_CHUNK_SIZE       (1*1024)

#define  MAX_FRAGMENTS                  32

#define  MAX_BUFFER_CHUNKS              1500


/* Maximum number of Octeon Instruction (command) queues */
#define     MAX_OCTEON_INSTR_QUEUES         CN56XX_MAX_INPUT_QUEUES

/* Maximum number of Octeon Instruction (command) queues */
#define     MAX_OCTEON_OUTPUT_QUEUES        CN56XX_MAX_OUTPUT_QUEUES


/* Maximum number of DMA software output queues per Octeon device. 
   Though CN56XX supports 5 DMA engines, only the first 2 can generate
   interrupts on the host. So we stick with 2 for the time being. */
#define     MAX_OCTEON_DMA_QUEUES           2






#endif /* __OCTEON_CONFIG_H__  */

/* $Id: octeon_config.h 66446 2011-10-25 02:31:59Z mchalla $ */

