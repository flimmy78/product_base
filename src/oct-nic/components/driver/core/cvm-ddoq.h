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


/*! \file cvm-ddoq.h
    \brief Core driver: Definitions for DDOQ in the PCI core driver.
*/

#ifndef __CVM_DDOQ_H__
#define __CVM_DDOQ_H__




#include "cvmx-fpa.h"
#include "cvmx-wqe.h"
#include "cvmx-config.h"
#include "cvm-driver-defs.h"
#include "cvm-pci-dma.h"
#include "cavium-list.h"
#include "octeon-cntq-common.h"
#include "octeon-opcodes.h"


#define CVM_DDOQ_USE_FPA_POOL
#define CVM_DDOQ_PROFILE



#ifdef CVM_DDOQ_USE_FPA_POOL
#define FPA_DDOQ_POOL_COUNT      (4 * 1024)
#else
#define MALLOC_ARENA_SIZE        (8 * 1024 * 1024)
#endif



/* Driver allocates all buffers for DDOQ operation from this
   pool using the scratchpad location below. */
#define CVM_DDOQ_FPA_POOL      CVMX_FPA_PACKET_POOL
#define CVM_DDOQ_SCRATCHPAD    CVM_DRV_SCR_PACKET_BUF_PTR


#define CVM_DDOQ_ALLOC_BUFFER(ptr)    \
        ptr = cvm_drv_fpa_alloc_async(CVM_DDOQ_SCRATCHPAD, CVM_DDOQ_FPA_POOL);

#define CVM_DDOQ_FREE_BUFFER(ptr)     \
{  \
        cvm_drv_fpa_free(ptr, CVM_DDOQ_FPA_POOL, 0);   \
        ptr = NULL;  \
}

#define CVM_DDOQ_SET_FPA_POOL(lptr)   \
        lptr.s.pool = CVM_DDOQ_FPA_POOL;


/* Descriptor rings can be allocated from FPA Pool or from arena. */
#ifdef  CVM_DDOQ_USE_FPA_POOL

#define  cvm_ddoq_get_buffer(size)      \
	(((size) <= CVM_FPA_DDOQ_POOL_SIZE)?cvm_drv_fpa_alloc_sync(CVM_FPA_DDOQ_POOL):NULL)
#define  cvm_ddoq_put_buffer(ptr)      \
	(cvm_drv_fpa_free((ptr), CVM_FPA_DDOQ_POOL, 0))

#else

#define  cvm_ddoq_get_buffer(size)      \
	(CVM_COMMON_MALLOC_TSAFE(cvm_common_arenas, (size)))
#define  cvm_ddoq_put_buffer(ptr)      \
	(CVM_COMMON_FREE_TSAFE((ptr)))

#endif



/* This is the maximum number of local data pointers accepted by the
   cvm_ddoq_send_data() API. If the DDOQ has enough buffers to send
   the data, the API would chop up the data into multiple DMA commands
   and send it to host. */  
#define  MAX_DDOQ_DATA_PTRS        32



#ifdef  CVM_DDOQ_PROFILE
struct ddoq_profile {

	unsigned long   c_max;
	unsigned long   c_min;
	unsigned long   c_totaltime;
	unsigned long   c_lastcount;
	unsigned long   c_count;

	unsigned long   d_max;
	unsigned long   d_min;
	unsigned long   d_totaltime;
	unsigned long   d_lastcount;
	unsigned long   d_count;
};

void
cvm_drv_init_ddoq_profile(void);

void
cvm_drv_print_ddoq_profile(int interval);

#endif




/** The DDOQ structure.
  * Each ddoq will be represented in core local memory by this structure.
  */
typedef struct  {

     /** Lock for this DDOQ. */
     cvmx_spinlock_t         lock;

     /** A pointer to the local memory copy of the descriptor ring provided
         by the host. */
     octeon_ddoq_desc_t     *desc_ring;

     /** The id assigned to this ddoq */
     uint32_t                ddoq_id;

     /** Count of the actual descriptors that would be used in the ring */
     uint16_t                desc_count;

     /** The no. of descriptors currently available for the core to post
         packets */
     uint16_t                avl_desc;

     /**   The size of buffers for this ddoq's descriptors */
     uint16_t                buf_size;

     /** Index of the next descriptor that the core will write a packet */
     uint16_t                core_wr_index;

     /** Index of the next descriptor where the host is expected to read
         the next packet */
     uint16_t                host_rd_index;

     /** Index in the descriptor ring where the core is required to fill
         in newly-credited descriptors */
     uint16_t                refill_index;

     /** The numbers of buffers that will be used for data pushed to host
         before the host can send fresh buffers. */
     uint16_t                refill_threshold;

     uint16_t                reserved;

     /** The packet counter to use as request id in cntq_pkt->resp_hdr
         when sending the next ddoq packet. */
     uint32_t                next_pkt_cntr;

     /**  No. of pkts added to the wait list */
     uint16_t                wait_count;

     /** Indication that the DDOQ is undergoing delete processing. */
     uint16_t               delete_active;

     /** Statistics - Packets sent */
     uint64_t                pkts_sent;

     /** Statistics - Buffers sent */
     uint64_t                bufs_sent;

     /** Statistics - Credits received */
     uint64_t                credits_recvd;

     /** The wait list head */
     cavium_list_t           wait_list;

    /**  The pointer to the start of the wqe buffer that was used to store
         the ddoq. */
     cvmx_wqe_t              *wqe;

    /** The tag assigned to this DDOQ. */
    uint32_t                  tag;

    /** Type of DDOQ. Some DDOQ's may require additional operations based
        on their type. Type is specified by HOST. */
    uint32_t                  type;

    /** The address of the ddoq status field in host memory. */
    uint64_t                  host_status_addr;

} cvm_ddoq_t;


#define  CVM_DDOQ_SIZE   (sizeof(cvm_ddoq_t))



/** Format of the DDOQ INIT instruction sent by host.
  */ 
typedef struct {

   /** All raw instructions from the host have this 24-byte header in front */
   cvmx_raw_inst_front_t      instr;

   /** 16 bytes of front data header bytes followed by user defined data. 
       The ddoq_id, descriptor count, buffer size are available in the 
       front data. */
   ddoq_front_data_t          dfront;

   /** The descriptor ring starts from this location. This is the 1st element */
   octeon_ddoq_desc_t         desc_ring;

} cvm_ddoq_init_t;


/** The DDOQ CREDIT instruction has the same format as DDOQ INIT. */
typedef   cvm_ddoq_init_t    cvm_ddoq_credit_t;




/** Format of the DDOQ DELETE instruction sent by host.
  */ 
typedef struct {

   /** All raw instructions from the host have this 24-byte header in front */
   cvmx_raw_inst_front_t      instr;

   /** 16 bytes of front data header bytes followed by user defined data. 
       The ddoq_id, descriptor count, buffer size are available in the 
       front data. */
   ddoq_front_data_t          dfront;

}  cvm_ddoq_delete_t;




/** System wide statistics for DDOQ. */
typedef struct {

        /** Total number of packets sent on all ddoq's. */
        uint64_t   pkts_sent;

	/** Total packets dropped due to lack of memory. */
        uint64_t   pkts_dropped_nomem;

	/** Total packets dropped due to lack of ddoq buffers or when
	    the no. of bufs required exceeds max supported by DMA. */ 
        uint64_t   pkts_dropped_maxbufs;

	/** Total packets dropped due to PCI DMA error. */
        uint64_t   pkts_dropped_dmaerr;

	/** Total packets dropped due to unknown ddoq_id */
	uint64_t   pkts_dropped_noddoq;

	/** Total packets waitlisted by ddoq's. */
        uint64_t   pkts_waitlisted;

	/** Total credit pkts received from host for all ddoq's. */
	uint64_t   credit_pkts;

	/** Credits received for DDOQ undergoing deletion. 
            This error indicates an incorrect host driver behavior. */
        uint64_t   credit_on_delete; 

	/** Host driver did not credit the expected number of descriptors
	    to a DDOQ. Indicates an error condition. */
	uint64_t   invalid_credit;

	/** Total ddoq create failures due to lack of memory. */
	uint64_t   create_fail_nomem;

	/** Total ddoq create failures due to ddoq list errors */
	uint64_t   create_fail_list;

	/** Total ddoq create failures due to inability to send response. */
	uint64_t   create_resp_failed;

	/** Total DELETE instruction received for unknown ddoq */
	uint64_t   delete_unknown;

	/** Total ddoq delete failed due to lack of memory. */
	uint64_t   delete_nomem;

	/** Total ddoq delete failures due to inability to send response. */
	uint64_t   delete_resp_failed;

} cvm_ddoq_global_stats_t;





/** Format of a ddoq wait listed packet.
  * If data cannot be sent on a DDOQ, it is put into a wait list till
  * more descriptors are credited.
  */
typedef  struct {

  /** List manipulation pointers. */
  cavium_list_t                  list;

  /** The PCI command to be used for sending this packet. */
  cvmx_oct_pci_dma_inst_hdr_t    dma_hdr;

  /** These bits are reserved. */
  uint32_t                       reserved:10;

  /** The ddoq id */
  uint32_t                       ddoq_id:22;

  /** Size of data. */
  uint16_t                       pkt_size;

  /** User can send data as a single buffer, a linked list of buffers or as
      a gather list. */
  uint16_t                       ptr_type;

  /** Number of data buffers. */
  uint16_t                       buf_count;

  /** Number of DDOQ buffers required to send this data. */
  uint16_t                       bufs_reqrd;

  /** Response Header to be sent with the data. */
  cvmx_resp_hdr_t                resp_hdr;

  /** Address of the data buffers. Max allowed by DMA is 15. */
  cvmx_buf_ptr_t                 buf[MAX_DDOQ_DATA_PTRS];

} cvm_ddoq_pkt_t;

#define  CVM_DDOQ_PKT_SIZE     (sizeof(cvm_ddoq_pkt_t))



/** Macro to allocate space for a ddoq packet created to wait 
  * list data sent by aplication. 
  */
static inline cvm_ddoq_pkt_t  *
cvm_ddoq_alloc_pkt(void)
{
	cvm_ddoq_pkt_t *buf;

	CVM_DDOQ_ALLOC_BUFFER(buf);
	if (buf)  {
		CAVIUM_INIT_LIST_HEAD(&(buf->list));
		CVMX_SYNCWS;
	}
	return buf;
}







/** Macro to release space allocate for a ddoq packet. */
static inline void
cvm_release_ddoq_packet(cvm_ddoq_pkt_t *buf)
{
	CVM_DDOQ_FREE_BUFFER(buf);
}




/*------------ Function prototypes for routines in cvm-ddoq.c -------------*/
/** This routine initializes the ddoq lookup table structure. 
  * @return 0 on success, 1 on failure.
  */
int  cvm_init_ddoq_list(void);




/** Core applications should call this routine in their global initialization
  * routine to setup the driver's internal ddoq structures. 
  * This routine is also called by the cvm_drv_setup_dma_queues() in cvm-cntq.h
  * @return 0 on success; 1 on failure. 
  */
int  cvm_drv_ddoq_setup(void);




/** Call this routine to print the contents of a DDOQ structure.
  */
void cvm_print_ddoq(cvm_ddoq_t *ddoq);





/** Call this routine to send data to a DDOQ on the host. 
  * This routine checks for space in the DDOQ and sends the data to the host if 
  * there is space, else it queues the data in a wait list to be sent later 
  * when space becomes available. If the queue gets full, the data is not 
  * accepted and -ENOMEM is returned.  
  * NOTE that the number of local pointers in a linked or gather list should
  * not be greater than MAX_DDOQ_DATA_PTRS.
  * 
  * @param  ddoq_id  - Id of the DDOQ that should receive this data.
  * @param  dptr     - local buffer format (address/size) of data. Interpreted
  *                    depending on the value in ptr_type below.
  * @param  ptr_type - Pointer type for dptr. "dptr" can direct point to
  *                    the data, or the first buffer of a linked list or
  *                    to a gather list.
  * @param  segs     - Number of data buffers or gather list elements.
  *                    Cannot exceed MAX_DDOQ_DATA_PTRS.
  * @param  total_bytes - Sum of data in all local data pointers.
  * @param  user_resp_hdr - response to be sent with this data.
  * @return: -ve value indicating type of error on failure. Number of buffers
  *          consumed for packet if packet is sent or wait listed.
  * 
  */
int
cvm_ddoq_send_data(uint32_t         ddoq_id,
                   cvmx_buf_ptr_t   dptr,
                   cvm_ptr_type_t   ptr_type,
                   uint32_t         segs,
                   uint32_t         total_bytes,
                   cvmx_resp_hdr_t *user_resp_hdr);





/** Call this routine to change the tag for a DDOQ.
  * @param ddoq_id  - change the tag for this ddoq.
  * @param new_tag  - the new 32-bit tag.
  * @return: -ve value if ddoq does not exist, 0 otherwise.
  */
int
cvm_ddoq_change_tag(uint32_t   ddoq_id,
                    uint32_t   new_tag);





/**  Call this routine to know if there are packets in the wait list for
  *  a DDOQ.
  *  @param   ddoq_id - the ddoq whose wait list count is to be known.
  *  @return  -EINVAL if the DDOQ was not found, else the DDOQ's wait count
  *           is returned (which could be zero).
  */
int
cvm_ddoq_get_wait_count(uint32_t    ddoq_id);



/** Returns 1 if a DDOQ exists, else 0.
  * @param ddoq_id - Check for DDOQ with this id.
  */
int
cvm_ddoq_exists(uint32_t         ddoq_id);


/** Returns no. of packets sent on a DDOQ.
  * @param ddoq_id - Get packet count for this DDOQ.
  * @param - Count of packets sent if ddoq exits, else -EINVAL.
  */
int
cvm_ddoq_get_pkts_sent(uint32_t      ddoq_id);





/** Applications can register callbacks of this type with the driver for
  * certain types of DDOQ's. The driver will invoke this callback with 
  * the tag, id and front data sent for the DDOQ.
  * See cvm_ddoq_setup_callback().
  */
typedef int (*cvm_drv_ddoq_callback_t)(uint32_t  tag, uint32_t  ddoq_id, void* front_data);



/** Setup a callback with the driver to be called when an action defined by
  * "ddoq_action" happens on any ddoq of type "ddoq_type". "ddoq type" values
  * are defined in common/octeon-cntq-common.h.
  */
int
cvm_ddoq_setup_appcallback(uint32_t  ddoq_type, uint32_t ddoq_action, cvm_drv_ddoq_callback_t appcb);




#endif


/* $Id: cvm-ddoq.h 63425 2011-08-11 22:56:32Z panicker $ */
