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
/*! \file cvm-pci-dma.h
    \brief  Core Driver: Structures and routines for PCI DMA queue
            management.
 */

#ifndef __CVM_PCI_DMA_H__
#define __CVM_PCI_DMA_H__

#include  "cvm-driver-defs.h"
#include  "cvm-oct-dev.h"


/* Enable this flag to turn on tracking of PCI DMA transactions.
   See cvm-pci-dma.c for a description of the tracking mechanism.
*/
//#define CVM_PCI_TRACK_DMA
#define CVM_PCI_DMA_TRACKER_PORT   0x2f
#define CVM_PCI_DMA_TRACKER_TAG    0x12345678


#define MAX_PCI_DMA_LOCAL_BUF_SIZE    ((1 << 13) - 8)


#define CVM_FPA_DMA_CHUNK_POOL        CVMX_FPA_PACKET_POOL
#define CVM_SCR_DMA_CHUNK_ALLOC       CVM_DRV_SCR_PACKET_BUF_PTR
#define CVM_DMA_CHUNK_SIZE    \
       (cvmx_fpa_get_block_size(CVM_FPA_DMA_CHUNK_POOL))


#define OCTEON_MAX_DMA_LOCAL_POINTERS          14
#define OCTEON_MAX_DMA_REMOTE_POINTERS         13
#define OCTEON_MAX_DMA_POINTERS                27

typedef enum {

	CVMX_DMA_QUEUE0 = 0,
	CVMX_DMA_QUEUE1 = 1,
	CVMX_DMA_QUEUE2 = 2,
	CVMX_DMA_QUEUE3 = 3,
	CVMX_DMA_QUEUE4 = 4,

}CVMX_DMA_QUEUE_TYPE;

#define  CVM_PCI_DMA_START_ADDR_REG(q_no)  \
	((q_no)?CVMX_NPI_LOWP_IBUFF_SADDR:CVMX_NPI_HIGHP_IBUFF_SADDR)

#define  CVM_PCI_DMA_DOORBELL_REG(q_no)  \
	((q_no)?CVMX_NPI_LOWP_DBELL:CVMX_NPI_HIGHP_DBELL)


#define	CVMX_HIGHP_DMA_QUEUE    CVMX_DMA_QUEUE0
#define	CVMX_LOWP_DMA_QUEUE     CVMX_DMA_QUEUE1 






/* Core: Flags in cvm_pci_dma_cmd_t can take one or more of the 
         values defined here. The flags are used in PCI send and receive API's.

  -------
|  Bit 7  | - Reserved
  -------
|  Bit 6  | - If set, DMA Command uses WQE for completion; 
  -------
|  Bit 5  | - If set, DMA Command FreeLocal bit is set.
  -------
|  Bit 4  | - If set, DMA Command IgnoreI bit is set.
  -------
|  Bit 3  | - If set, DMA Command Force Interrupt bit is set
  -------
|  Bit 2  | - If set, DMA Command Counter Add bit is set
  -------
| Bit 0-1 | - Direction.
  ------- 
*/


typedef enum {
	PCI_DMA_OUTBOUND  =  0,     /* Default DMA direction is outbound */
	PCI_DMA_INBOUND   =  1,
	PCI_DMA_EXTERNAL  =  2,
	PCI_DMA_INTERNAL  =  3,
	PCI_DMA_CNTRADD   =  4,
	PCI_DMA_FORCEINT  =  8,
	PCI_DMA_IGNOREI   =  16,
	PCI_DMA_FREELOCAL =  32, 
	PCI_DMA_PUTWQE    =  64,
} oct_pci_dma_flags_t;


/* There is no bit set in the DMA command for using a memory location for
   completion. This flag is defined here just to complete the set of flags.
*/
#define PCI_DMA_PUTWORD    0


/* By default, none of the above flags are set. The driver would still set the
   force interrupt bit for a instruction response.
*/
#define PCI_DMA_FLAGS_DEFAULT  0



/* Normal local buffer pointer and DMA local buffer pointer differ in the
 * bits below. 
 * Bit53 should not be set, Bits 39-36 should not be set. */
#define  CVM_PCI_DMA_LOCAL_PTR_MASK   0xFFDFFF0FFFFFFFFFULL




typedef enum {

	DMA_BLOCKING     = 1,
	DMA_NON_BLOCKING = 2

} DMA_ACTION;




/** Core: Hardware defined 64-bit PCI DMA command structure. Most of the bits
	are shared across all Octeon processors. Some bits were reserved in
	CN38XX/CN58XX processors but have meaning in CN56XX. */
typedef union {

	uint64_t       u64;

	struct {
#if __BYTE_ORDER == __LITTLE_ENDIAN
		uint64_t   ptr  :40;
		uint64_t   nl   : 4;
		uint64_t   nr   : 4;
		uint64_t   fl   : 1;
		uint64_t   ii   : 1;
		uint64_t   fi   : 1;
		uint64_t   ca   : 1;
		uint64_t   c    : 1;
		uint64_t   wqp  : 1;
		uint64_t   dir  : 2;  /**< This field is 1 bit wide in CN38xx/58xx */
		uint64_t   lport: 2;  /**< This field doesn't exist in CN38xx/58xx */
		uint64_t   fport: 2;  /**< This field doesn't exist in CN38xx/58xx */
		uint64_t   rsvd : 4;  
#else
		uint64_t   rsvd : 4;
		uint64_t   fport: 2;  /**< This field doesn't exist in CN38xx/58xx */
		uint64_t   lport: 2;  /**< This field doesn't exist in CN38xx/58xx */
		uint64_t   dir  : 2;  /**< This field is 1 bit wide in CN38xx/58xx */
		uint64_t   wqp  : 1;
		uint64_t   c    : 1;
		uint64_t   ca   : 1;
		uint64_t   fi   : 1;
		uint64_t   ii   : 1;
		uint64_t   fl   : 1;
		uint64_t   nr   : 4;
		uint64_t   nl   : 4;
		uint64_t   ptr  :40;
#endif
	} s;

} cvmx_oct_pci_dma_inst_hdr_t;






/** Core: Hardware defined 64-bit PCI DMA local buffer format. */
typedef union {
   uint64_t                   u64;
   void                    *ptr;
   uint64_t               *u64ptr;
   struct {
#if __BYTE_ORDER == __LITTLE_ENDIAN
      uint64_t                addr  :36;
      uint64_t            reserved  : 4;
      uint64_t                size  :13;
      uint64_t                   l  : 1;
      uint64_t                   a  : 1;
      uint64_t                   f  : 1;
      uint64_t                pool  : 3;
      uint64_t                back  : 4;
      uint64_t                   i  : 1;
#else
      uint64_t                   i  : 1;
      uint64_t                back  : 4;
      uint64_t                pool  : 3;
      uint64_t                   f  : 1;
      uint64_t                   a  : 1;
      uint64_t                   l  : 1;
      uint64_t                size  :13;
      uint64_t            reserved  : 4;
      uint64_t                addr  :36;
#endif
   } s;
} cvmx_oct_pci_dma_local_ptr_t;







/** Core: Information about PCI DMA operation.
  * This structure tells the core driver about the type of DMA 
  * operation to be submitted to Octeon PCI DMA engines. The
  * Octeon PCI DMA command is built from the field defined here.
  * This structure is passed to the PCI send/receive api's. It is
  * not used in the instruction response API's.
  */
typedef union {

	uint64_t   u64;

	struct {
		/** Number of remote pointers in the operation. */
		uint64_t   nr:4;

		/** Number of local pointers in the operation. */
		uint64_t   nl:4;

		/** Determines DMA queue to use for this operation.
		    There are 2 DMA queues forCN38XX, 5 for CN56XX and 8 for CN63XX. */
		uint64_t   q_no:3;

		/** CN56XX/63XX Only. Determines First PCI-Express Port for DMA. */
		uint64_t   pciefport:1;

		/** CN56XX/63XX Only. Determines Last PCI-Express Port for DMA. */
		uint64_t   pcielport:1;

		/** Reserved */
		uint64_t   rsvd:3;

		/** Flags describe the direction of DMA and post-DMA
		    operations as defined in the enum oct_pci_dma_flags_t */
		oct_pci_dma_flags_t   flags:8;

		/** Physical address of WQE buffer or location in Octeon memory
		    for post-DMA operation. */
		uint64_t   ptr:40;
	} s;

} cvm_pci_dma_cmd_t;


#define  INIT_PCI_DMA_CMD(pcmd)         (pcmd->u64 = 0)


/* Get the direction bits from the flags field of the CVM PCI DMA Command. */
#define  CVM_PCI_DMA_DIR(dmacmdptr)     (dmacmdptr->s.flags & 0x3)



/** Core: Scatter response information 
  * This structure is passed by application when calling a scatter response
  * API.
  */ 
typedef struct {

	/** The response header for the instruction. Ignored for
	    cvm_dma_send_scatter_response_direct()  since ORH is part of 
	    response data for direct API. */
	cvmx_resp_hdr_t      orh;

	/** The status to be returned to host. Ignored for 
	    cvm_dma_send_scatter_response_direct() since status is part of 
	    response data for direct API. */
	uint64_t             status;

	/** Number of entries in the scatter list. */
	uint32_t             remote_segs;

	/** PCIe port to use to read scatter list and send response. */
	uint8_t              pcie_port;

	/** Reserved. */ 
	uint8_t             reserved[3];

	/** The scatter list that contains the host output buffer addresses. */
	octeon_sg_entry_t   *sg_entry;

} cvm_pci_scatter_resp_t;


 



/** Core: Format for size of remote data buffer in a PCI DMA component.
  * Refer to section 10.5.3 in Octeon Hardware Manual. */
typedef union {

   uint64_t  u64;

   struct {
      uint16_t  len[4];
   } l;

} cvm_dma_remote_len_t; 





/**  Core:  Address of the host machine are presented to PCI DMA routines in
  *  this format. The driver translates this to the PCI component format.
  */
typedef union {

   uint64_t  u64;

   struct {
      uint64_t     addr:48;
      uint64_t     size:16;
   }s;

}  cvm_dma_remote_ptr_t;





#ifdef CVM_PCI_TRACK_DMA

/* Application should call this routine for all WQE's received from port 0x2f.
   The DMA tracker had modified the DMA command to generate these WQE. They
   should be sent to the DMA tracker routine for proper processing.
*/
void  cvm_pci_dma_remove_from_tracker_list(cvmx_wqe_t  *wqe);


/* Applications can call this routine to print the list of DMA commands that
   have not been processed by the DMA engine.
*/
void cvm_pci_dma_dump_tracker_list(void);


#endif



/*------------------------- Inline Routines -------------------------------*/





/** Core: Routine to fill the PCI DMA instruction local pointers. This 
  * routines translates LINKED, DIRECT and GATHER mode pointers to a list of
  * local pointers as required by the PCI DMA engine. Applications that
  * require calling a direct PCI DMA API like cvm_pci_dma_raw() should call
  * this function to prepare the local pointer list.
  *
  * @param local_ptr: Local pointer list to be passed to DMA instruction
  * @param lptr     : Local pointer list (linked/gather) available with app.
  * @param segs     : Number of local pointers.
  * @param ptr_type : CVM_DIRECT_DATA, CVM_LINKED_DATA or CVM_GATHER_DATA.
  * @return Sum of buffer sizes for the addresses copied into local_ptr.
  */
static inline int
cvm_dma_fill_local_ptrs(cvmx_oct_pci_dma_local_ptr_t    *local_ptr,
                        cvmx_buf_ptr_t                   lptr,
                        uint32_t                         segs,
                        cvm_ptr_type_t                   ptr_type)
{
	uint32_t         total_bytes=0, i;
	cvmx_buf_ptr_t   *list;

	switch(ptr_type) {
		case CVM_DIRECT_DATA:
			if(segs == 1) {
				if(lptr.s.size > MAX_PCI_DMA_LOCAL_BUF_SIZE)
					return -1;
				local_ptr->u64 = (lptr.u64 & CVM_PCI_DMA_LOCAL_PTR_MASK);
				total_bytes = lptr.s.size;
			}
			break;
		case CVM_LINKED_DATA:
			if(lptr.s.size > MAX_PCI_DMA_LOCAL_BUF_SIZE)
				return -1;
			local_ptr->u64 = (lptr.u64 & CVM_PCI_DMA_LOCAL_PTR_MASK);
			total_bytes    = local_ptr->s.size;
			local_ptr++;
			list = (cvmx_buf_ptr_t *)CVM_DRV_GET_PTR(lptr.s.addr - 8);
			for(i = 1; i < segs; i++, local_ptr++) {
				if(list->s.size > MAX_PCI_DMA_LOCAL_BUF_SIZE)
					return -1;
				local_ptr->u64  = (list->u64 & CVM_PCI_DMA_LOCAL_PTR_MASK);
				total_bytes    += local_ptr->s.size;
				list = (cvmx_buf_ptr_t *)CVM_DRV_GET_PTR(list->s.addr - 8);
			}
			break;
		case CVM_GATHER_DATA:
			list = (cvmx_buf_ptr_t *)CVM_DRV_GET_PTR(lptr.s.addr);
			DBG_PRINT(DBG_FLOW, "cvm_pci_dma: gptr @ 0x%p\n", list);
			DBG_PRINT(DBG_FLOW, "gather list contents\n");
			for(i = 0; i < segs; i++, local_ptr++) {
				if(list[i].s.size > MAX_PCI_DMA_LOCAL_BUF_SIZE)
					return -1;
				local_ptr->u64  = (list[i].u64 & CVM_PCI_DMA_LOCAL_PTR_MASK);
				total_bytes    += local_ptr->s.size;
				DBG_PRINT(DBG_FLOW, "gather list[%d]: 0x%llx\n", i, cast64(list[i].u64));
			}
			break;
		case CVM_NULL_DATA:  /* Nothing to do here/. */
			break;
	}
	CVMX_SYNCWS;
	return total_bytes;
}







/*-------------------------- FUNCTION PROTOTYPES ---------------------------*/

/** Core: Initialize a PCI DMA instruction chunk. This routine allocates the
 *  first chunk for each DMA instruction queue and writes the chunk addr,
 *  maximum writeable space in chunk to CSR. It also allocates extra chunk for
 *  fast chaining.
 *  @return Success: 0; Failure: 1.
 */
int  cvm_dma_queue_init(cvm_oct_dev_t  *oct);







/**  Core: This routines creates a PCI DMA instruction based on information in
 *   the DMA header and local and remote pointers and buffer sizes passed to it.
 *   Applications have more control over the DMA operation when using this API.
 *   This function can be called for INBOUND & OUTBOUND operations only for all
 *   variants of Octeon.
 *
 *   @param  dma_hdr  -  the DMA instruction header.
 *   @param  lptr     -  list of local  data pointers.
 *   @param  rptr     -  list of remote (host/peer) addresses.
 *   @return Success: 0; Failure: -ENOMEM, -EINVAL.
 */
int
cvm_pci_dma_raw(cvmx_oct_pci_dma_inst_hdr_t      *dma_hdr,
                cvmx_oct_pci_dma_local_ptr_t     *lptr,
                cvm_dma_remote_ptr_t             *rptr);



/**  Core: This routines creates a PCI DMA instruction based on information in
 *   the DMA header and local and remote pointers and buffer sizes passed to it.
 *   Applications have more control over the DMA operation when using this API.
 *   This function can be called for INBOUND & OUTBOUND operations.
 *   For CN56XX & CN63xx, an application can also achieve INTERNAL-ONLY or
 *   EXTERNAL-ONLY operations using this API.
 *   Based on the DMA type specified, the firstptrs and lastptrs will be
 *   interpreted differently. In all cases, the pointers from the firstptrs
 *   list will be copied into the DMA instruction chunk, followed by the 
 *   pointers in the lastptrs list.
 *
 *   @param  q_no      -  The PCI DMA queue to use for this DMA operation.
 *                        The DMA instruction header does not have a field for
 *                        the queue to be used. So this is passed separately for
 *                        CN56XX & CN63XX.
 *   @param  dma_hdr   -  the DMA instruction header.
 *   @param  firstptrs -  first set of pointers.
 *   @param  lastptrs  -  second set of pointers.
 *   @return Success: 0; Failure: -ENOMEM, -EINVAL.
 */
int
cvm_pcie_dma_raw(int                              q_no,
                 cvmx_oct_pci_dma_inst_hdr_t     *dma_hdr,
                 void                            *firstptrs,
                 void                            *lastptrs);



/** Core: Write data to host or PCI-Express memory using Octeon PCI DMA engine.
  * @param cmd  - describes the DMA operation. See cvm_pci_dma_cmd_t for
  *               description of fields. 
  * @param lptr - list of addresses in Octeon local memory (max 15).
  * @param rptr - list of addresses in host/PCI-E memory (max 15).
  * @return Success: 0; Failure: -ENOMEM, -EINVAL. 
  */
int
cvm_pci_dma_send_data(cvm_pci_dma_cmd_t     *cmd,
                      cvmx_buf_ptr_t        *lptr, 
                      cvm_dma_remote_ptr_t  *rptr);



/** Core: Read data from host or PCI-Express memory using Octeon PCI DMA engine.
  * @param cmd  - describes the DMA operation. See cvm_pci_dma_cmd_t for
  *               description of fields. 
  * @param lptr - list of addresses in Octeon local memory (max 15).
  * @param rptr - list of addresses in host/PCI-E memory (max 15).
  * @return Success: 0; Failure: -ENOMEM, -EINVAL. 
  */
int
cvm_pci_dma_recv_data(cvm_pci_dma_cmd_t     *cmd,
                      cvmx_buf_ptr_t        *lptr, 
                      cvm_dma_remote_ptr_t  *rptr);



/** Core: Function to get a completion pointer from core driver pool.
 *        The application would use the comp_byte in the structure as a 
 *        completion word for PCI DMA.
 * @return Success: Pointer to completion byte; Failure: NULL.
 */
cvm_dma_comp_ptr_t *    cvm_get_dma_comp_ptr(void);


/** Core: Function to free completion pointer to core driver pool.
 */
void    cvm_release_dma_comp_ptr(cvm_dma_comp_ptr_t *ptr);




/** Core: Wrapper around the cvm_pci_dma_raw() function.
 *        cvm_pci_dma_send_direct() existed in older releases and may be
 *        removed in the future.
 */
static inline int
cvm_pci_dma_send_direct(cvmx_oct_pci_dma_inst_hdr_t      *dma_hdr,
                        cvmx_oct_pci_dma_local_ptr_t     *lptr,
                        cvm_dma_remote_ptr_t             *rptr)
{
	return cvm_pci_dma_raw(dma_hdr, lptr, rptr);
}



void    cn56xx_setup_dma_intr_threshold(int q_no);

/* Parse and print the contents of the 64-bit PCI DMA header. */
void cvm_pci_dma_print_header(cvmx_oct_pci_dma_inst_hdr_t  *dmahdr);


void
cvm_drv_debug_print_dmaq(int q_no);


#endif  /*  __CVM_PCI_DMA_H__ */

/* $Id: cvm-pci-dma.h 63294 2011-08-09 21:48:01Z panicker $ */
