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


/*! \file cvm-pci-pko.h 
    \brief Core Driver: PKO initialization and packet send routines. 
*/



#ifndef __CVM_PCI_PKO_H__
#define __CVM_PCI_PKO_H__

#include "cvm-driver-defs.h"
#include "cvm-oct-dev.h"

#define   PKO_DIRECT_DATA   0x00
#define   PKO_GATHER_DATA   0x01
#define   PKO_LINKED_DATA   0x02

#define   CVM_USE_HW_CKSUM_OFFLOAD    1

#define CVM_PCI_PKO_FPA_POOL     CVMX_FPA_PACKET_POOL
#define CVM_PCI_PKO_SCRATCHPAD   CVM_DRV_SCR_PACKET_BUF_PTR

#define CVM_PCI_PKO_ALLOC_BUFFER(ptr)    \
      ptr = cvm_drv_fpa_alloc_async(CVM_PCI_PKO_SCRATCHPAD, CVM_PCI_PKO_FPA_POOL)

#define CVM_PCI_PKO_FREE_BUFFER(ptr)     \
        cvm_drv_fpa_free(ptr, CVM_PCI_PKO_FPA_POOL, 0)

#define CVM_PCI_PKO_SET_FPA_POOL(lptr)   \
        lptr.s.pool = CVM_PCI_PKO_FPA_POOL




/** Core: Sets up the map of PKO hardware port & queues corresponding to the
	PCI output queues.
    @return Success: 0; Failure: 1.
*/
int   setup_pci_pko_ports(void);





/** Core: Call this routine to send a data packet to the Octeon output
  *  queues. For a linked mode or direct mode data, this routine
  *  adds another buffer to the start of the linked list into which the
  *  response header is copied. For a gather mode data sent by user,
  *  this routine creates a new gather list and makes the user
  *  response header the first element of the gather list. Needless
  *  to say, if performance is important, you should make space
  *  to add 8 bytes of response header before your data and call
  *  cvm_pko_post_command() directly.
  *
  *  @param user_resp_hdr  - the response header that identifies the
  *                          user data.
  *  @param dptr           - pointer to the user data.
  *  @param ptr_type       - type of data pointed by dptr( linked/gather/direct)
  *  @param total_bytes    - amount of user data (excludes response header)
  *  @param segs  - number of user buffers (in linked and gather mode)
  *  @param port  - PKO port on which to send data ( 32 <= port <= 35)
  *
  *  @return  Success: returns 0, else 1.
  */
int
cvm_pko_send_data(cvmx_resp_hdr_t  *user_resp_hdr,
                  cvmx_buf_ptr_t    dptr,
                  cvm_ptr_type_t    ptr_type,
                  uint32_t          total_bytes,
                  uint32_t          segs,
                  uint32_t          port);





/** Core: The arguments have the same meaning as the cvm_pko_send_data() API
 * above. This API assumes that 8 bytes of response header is available at the
 * start of the first buffer pointed by lptr.
 *
 * @param lptr     - Pointer to data or gather buffer in Octeon's local memory.
 * @param ptr_type - Describes the type of buffer available at dptr.
 * @param segs     - Number of buffers.
 * @param total_bytes - Sum of data bytes from all buffers.
 * @param port  - PKO port on which to send data ( 32 <= port <= 35)
 *
 * @return Success: 0; Failure: 1.
 */
int
cvm_pko_send_direct(cvmx_buf_ptr_t   lptr, 
                    cvm_ptr_type_t   ptr_type,
                    uint32_t         segs,
                    uint32_t         total_bytes,
                    uint32_t         port);







/** Core: The first 5 parameters have the same meaning as cvm_pko_send_data()
 * API above. This API also assumes that 8 bytes of response header is
 * available at the start of the first buffer.  
 * In addition, the flags parameter is used to pass additional flags.
 * The flags_data parameter provides additional information when flags is
 * non-zero. Currently flags is reserved for use by the driver and should
 * always be set to zero.
 *
 * @param lptr     - Pointer to data or gather buffer in Octeon's local memory.
 * @param ptr_type - Describes the type of buffer available at dptr.
 * @param segs     - Number of buffers.
 * @param total_bytes - Sum of data bytes from all buffers.
 * @param port     - PKO port on which to send data ( 32 <= port <= 35)
 * @param flags    - additional flags.
 * @param flag_data - data associated with the flags.
 *
 *
 * @return Success: 0; Failure: 1.
 */

int
cvm_pko_send_direct_flags(cvmx_buf_ptr_t   lptr, 
                          cvm_ptr_type_t   ptr_type,
                          uint32_t         segs,
                          uint32_t         total_bytes,
                          uint32_t         port,
                          uint32_t         flags,
                          uint64_t         flag_data);






/** Core: The arguments have the same meaning as the cvm_pko_send_direct()
 * API above. This API assumes that 8 bytes of response header is available
 * at the start of the first buffer pointed by lptr.
 * The oq_no corresponds to the PCI output queue ring number (not the PCI port)
 * and ranges from 0 to 31. This API can be used when you have more than 1
 * output queue ring per PCI port to target the output queue directly. The
 * previous API.s always sent to the first queue in the PCI port. Additional
 * setup is required before using this API.
 *
 * @param lptr     - Pointer to data or gather buffer in Octeon's local memory.
 * @param ptr_type - Describes the type of buffer available at dptr.
 * @param segs     - Number of buffers.
 * @param total_bytes - Sum of data bytes from all buffers.
 * @param oq_no    - The PCI output queue to send this data.
 *
 * @return Success: 0; Failure: 1.
 */





int
cvm_send_pci_pko_direct(cvmx_buf_ptr_t   lptr,
                        cvm_ptr_type_t   ptr_type,
                        uint32_t         segs,
                        uint32_t         total_bytes,
                        uint32_t         oq_no);



/** Core: Get the PKO hardware port number corresponding to the PCI output queue
    number passed as argument. */
int
cvm_pci_get_oq_pkoport(int oq);


/** Core: Get the PKO hardware queue number corresponding to the PCI output queue
    number passed as argument. */
int
cvm_pci_get_oq_pkoqueue(int oq);


#endif  /* __CVM_PCI_PKO_H__ */


/* $Id: cvm-pci-pko.h 63425 2011-08-11 22:56:32Z panicker $ */
