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



/*! \file cvm-pci.h
    \brief Core Driver: PCI instruction format and core mem mapping.
*/


#ifndef __CVM_PCI_H__
#define __CVM_PCI_H__

#include "cvmx.h"


/* The 4 PCI ports are from 32 to 35  */
#define    FIRST_PCI_PORT                   32
#define    LAST_PCI_PORT                    35

#define    DRV_REQUEST_DONE                 0




/** Core: The (Packet) Instruction Header appears in the format shown below for 
  * Octeon. Refer to the Octeon HW Manual to read more about the 
  * conversion from PCI instruction header to Packet Instruction Header.
  */
typedef union {
    uint64_t   u64;
    struct {
#if __BYTE_ORDER == __LITTLE_ENDIAN
       uint64_t   tag:32;        /**< 31-00 Tag for the Packet. */
       cvmx_pow_tag_type_t tt:2; /**< 33-32 Tagtype */
       uint64_t   rs:1;          /**< 34    Is the PCI packet a RAW-SHORT? */
       uint64_t   grp:4;         /**< 38-35 The group that gets this Packet */
       uint64_t   qos: 3;        /**< 41-39 The QOS set for this Packet. */
       uint64_t   rsvd3:6;       /**< 47-42 Reserved */
       uint64_t   sl:7;          /**< 54-48 Skip Length */
       uint64_t   rsvd2:1;       /**< 55    Reserved. */
       uint64_t   pm:2;          /**< 57-56 The parse mode to use for the packet. */
       uint64_t   rsvd1:5;       /**< 62-58 Reserved */
       uint64_t   r:1;           /**< 63    Is the PCI packet in RAW-mode? */
#else
       uint64_t   r:1;           /**< 63    Is the PCI packet in RAW-mode? */
       uint64_t   rsvd1:5;       /**< 62-58 Reserved */
       uint64_t   pm:2;          /**< 57-56 The parse mode to use for the packet. */
       uint64_t   rsvd2:1;       /**< 55    Reserved. */
       uint64_t   sl:7;          /**< 54-48 Skip Length */
       uint64_t   rsvd3:6;       /**< 47-42 Reserved */
       uint64_t   qos: 3;        /**< 41-39 The QOS set for this Packet. */
       uint64_t   grp:4;         /**< 38-35 The group that gets this Packet */
       uint64_t   rs:1;          /**< 34    Is the PCI packet a RAW-SHORT? */
       cvmx_pow_tag_type_t tt:2; /**< 33-32 Tagtype */
       uint64_t   tag:32;        /**< 31-00 Tag for the Packet. */
#endif
    } s;
} cvm_pci_inst_hdr2_t;




/** Core: Format of the input request header in an instruction. */
typedef union  {

    uint64_t  u64;
    struct {
#if __BYTE_ORDER == __LITTLE_ENDIAN
        uint64_t    rid:16;     /**< Request ID  */
		uint64_t    pcie_port:3;/**< PCIe port to send response. */ 
        uint64_t    scatter:1;  /**< Scatter indicator  1=scatter */
        uint64_t    rlenssz:14; /**< Size of Expected result OR no. of entries in scatter list */
        uint64_t    dport:6;    /**< Desired destination port for result */
        uint64_t    param:8;    /**< Opcode Specific parameters */
        uint64_t    opcode:16;  /**< Opcode for the return packet  */
#else
        uint64_t    opcode:16;  /**< Opcode for the return packet  */
        uint64_t    param:8;    /**< Opcode Specific parameters */
        uint64_t    dport:6;    /**< Desired destination port for result */
        uint64_t    rlenssz:14; /**< Size of Expected result OR no. of entries in scatter list */
        uint64_t    scatter:1;  /**< Scatter indicator  1=scatter */
		uint64_t    pcie_port:3;/**< PCIe port to send response. */
		uint64_t    rid:16;     /**< Request ID  */
#endif
    }s;

} cvmx_pci_inst_irh_t;




/** Core: Format of the front data for a raw instruction in the first 24 bytes
 *  of the wqe->packet_data or packet ptr when a core gets work from a PCI input
 *  port.
 */
typedef struct  {

  /** The instruction header. */
   cvm_pci_inst_hdr2_t   ih;

  /** The host physical address where a response (if any) is expected. */
   uint64_t              rptr;

  /** The input request header. */
   cvmx_pci_inst_irh_t   irh;

} cvmx_raw_inst_front_t;


#define CVM_RAW_FRONT_SIZE   (sizeof(cvmx_raw_inst_front_t))




/** Core: Format of the response header in the first 8 bytes of response sent
 * to the host. */
typedef union   {

  uint64_t      u64;
  struct {

   /** Opcode tells the host the type of response. */
   uint64_t      opcode:16;

    /** The response is for a host instruction that arrived at this port. */
   uint64_t      sport:6;

    /** The destination port or driver-specific queue number. */
   uint64_t      destqport:22;

   uint64_t      reserved:4;

    /** The request id of the host instruction. */
   uint64_t      request_id:16;
  }s;

}cvmx_resp_hdr_t;


#define  CVMX_RESP_HDR_SIZE   (sizeof(cvmx_resp_hdr_t))





/** Core: The core driver routines can handle local pointers of the these
  * types. 
  */
typedef enum {
   CVM_DIRECT_DATA = 0, /**< Local Pointer points directly to data */
   CVM_GATHER_DATA = 1, /**< Local Pointer points to a gather list of local pointers */
   CVM_LINKED_DATA = 2, /**< Local pointer points to data which has links to more buffers */
   CVM_NULL_DATA   = 3  /**< If no data is sent and a flag is required, use this. */
} cvm_ptr_type_t;






/** Get the size of data in the first packet buffer in a WQE from PCI.
  * @param bufptr - the packet pointer from wqe (wqe->packet_ptr).
  * @param raw    - flag to indicate if the PCI packet is in RAW mode.
  * @return Returns the size of data in this packet buffer.
  */
static inline int
cvm_get_first_buf_size(cvmx_buf_ptr_t   bufptr, int raw)
{
	int  len=0;

	len = ( ( (cvmx_read_csr(CVMX_IPD_PACKET_MBUFF_SIZE) & 0xfff)
	      - ((cvmx_read_csr(CVMX_IPD_1ST_MBUFF_SKIP) & 0x3f) + 1) ) * 8)
	      - (bufptr.s.addr & 0x7);
	if(raw)
		len -= CVM_RAW_FRONT_SIZE;

	return len;
}


/** Call this routine to get the size of data for packet buffers following
  * the first buffer in a WQE from PCI.
  * @return Returns the size of data in this packet buffer.
  */
static inline int
cvm_get_next_buf_size(void)
{
	return  (((cvmx_read_csr(CVMX_IPD_PACKET_MBUFF_SIZE) & 0xfff)
	         - ((cvmx_read_csr(CVMX_IPD_NOT_1ST_MBUFF_SKIP) & 0x3f) + 1)) * 8);
}



/** Call this routine with virtual address of packet buffer to get the
  * address of the next packet buffer in the WQE chain.
  * @param pktptr - virtual address of packet buffer
  * @return Returns the virtual address of next packet buffer.
  */
static inline void *
cvm_get_next_pkt_ptr(void  *pktptr)
{
	cvmx_buf_ptr_t  *ptr = (cvmx_buf_ptr_t *)((uint8_t *)pktptr - 8); 
	return (void *)cvmx_phys_to_ptr(ptr->s.addr);
}




#endif  /* __CVM_PCI_H__ */

/* $Id: cvm-pci.h 61122 2011-06-07 00:12:54Z panicker $ */
