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



/*! \file cvm-driver-defs.h
    \brief Core Driver: Debug macros and wqe manipulation.
           Include this file in all applications.
*/


#ifndef __CVM_DRIVER_DEFS_H__
#define __CVM_DRIVER_DEFS_H__

#include "cvmx-config.h"
#include "executive-config.h"
#include "cvmx.h"
#include "cvmx-fpa.h"
#include "cvmx-npi.h"
#include "cvmx-pko.h"
#include "cvmx-pip.h"
#include "cvmx-ipd.h"
#include "cvmx-malloc.h"
#include "cvmx-scratch.h"
#include "cvmx-bootmem.h"
#include "cvmx-spinlock.h"
#include "cvmx-version.h"
#include "cvmx-helper.h"
#include "cvmx-helper-util.h"
#include "cvm-common-wqe.h"
#include "cvm-common-defs.h"
#include "cvm-common-misc.h"
#include "cvm-common-fpa.h"
#include "cvm-drv-debug.h"

#ifndef __CAVIUM_LITTLE_ENDIAN
#define __CAVIUM_LITTLE_ENDIAN 1234
#endif

#ifndef __CAVIUM_BIG_ENDIAN
#define __CAVIUM_BIG_ENDIAN     4321
#endif

#define MAX_DRV_CORES		32

/* In order to use PCIe port #1, set this macro to 1 */
#define OCTEON_PCIE_PORT        1   // 1


#ifdef __BYTE_ORDER
   #if __BYTE_ORDER == __LITTLE_ENDIAN
   #define __CAVIUM_BYTE_ORDER __CAVIUM_LITTLE_ENDIAN
   #else
   #define __CAVIUM_BYTE_ORDER __CAVIUM_BIG_ENDIAN
   #endif
#else
#error __BYTE_ORDER undefined
#endif

/* Driver header files inserted below. */

/** Debug levels for driver prints. */
#define DBG_ALL       7
#define DBG_STRUCT    6
#define DBG_NORM      4
#define DBG_FLOW      3
#define DBG_WARN      2
#define DBG_ERROR     1
#define DBG_CRIT      0

#define DBG_TEMP      0

#define PTR_SIZE   (sizeof(void*))



#define   cast64(val)    ((unsigned long long)val)

#define  castptr(type, val)    ((type)((unsigned long)val))



/** Print debug messages.
 *
 *  @param lvl    - debug level at which to print this message.
 *  @param format - format to use to print the message.
 *  @param args   - variable length arg list to create the message.
 */
#ifdef OCTEON_DEBUG_LEVEL
#define DBG_PRINT(lvl, format, args...) \
{ \
    if (lvl <= OCTEON_DEBUG_LEVEL) \
    { \
        printf("[ DRV ]"); \
        printf(format, ##args); \
    } \
}
#else
#define DBG_PRINT(lvl, format, args...)  do{ }while(0)
#endif


#define drv_err(format, args...)    printf(format, ##args)

//#define dbg_printf(format, ...)    printf( format, ## __VA_ARGS__)

#define dbg_printf(format, ...)    do { } while(0)


#ifndef ROUNDUP4
#define ROUNDUP4(val) (((val) + 3)&0xfffffffc)
#endif

#ifndef ROUNDUP8
#define ROUNDUP8(val) (((val) + 7)&0xfffffff8)
#endif

#ifndef ROUNDUP16
#define ROUNDUP16(val) (((val) + 15)&0xfffffff0)
#endif


/**
 getstructptr - get the address of structure given the address of a member
  Params:
        ptr: the pointer to the member.
       type: the type of the struct this ptr is a member.
     member: the name of the member within the struct.
*/
#define getstructptr(ptr, type, member)                   \
	 (type *)( (char *)ptr - offsetof(type,member) )



#define   CVM_DRV_GET_PHYS(ptr)         (cast64(cvmx_ptr_to_phys(ptr)))
#define   CVM_DRV_GET_PTR(phys_addr)    cvmx_phys_to_ptr((phys_addr))



/* octeon-common should be included after the endianness has been set. */
#include "octeon-common.h"
#include "cvm-pci.h"
#include "cvm-pci-pko.h"
#include "cvm-pci-dma.h"
#include "cvm-drv-reqresp.h"


#ifdef  CVM_SPINLOCK_DEBUG

static inline void
cvm_drv_spinlock_trylockloop(cvmx_spinlock_t  *lockp)
{
   volatile uint32_t lock_loops=0;
   while(cvmx_spinlock_locked(lockp) && (lock_loops++ < 1000));
   if(lock_loops == 1000)
      printf("[ DRV ] Spinlock @ %p failed at %s: %d \n",lockp, __FILE__, __LINE__);
   cvmx_spinlock_lock(lockp);
}

#define  cvm_drv_spinlock_lock(lockp)                 cvm_drv_spinlock_trylockloop(lockp);

#else

#define  cvm_drv_spinlock_lock(lockp)                 cvmx_spinlock_lock(lockp);

#endif






#define cvm_drv_fpa_alloc_sync(pool)                 \
		cvm_common_alloc_fpa_buffer_sync((pool))

#define cvm_drv_fpa_free(ptr, pool, cache_lines)     \
		cvm_common_free_fpa_buffer((ptr), (pool), (cache_lines))





static inline void
cvm_drv_free_pkt_buffer(cvmx_buf_ptr_t  pkt)
{
	void  *buf;

	buf = (void *)CVM_DRV_GET_PTR((((uint64_t)pkt.s.addr >> 7) - pkt.s.back) << 7);
	DBG_PRINT(DBG_FLOW,"Freeing pkt buf @ %p (pkt: 0x%016lx)\n", buf, pkt.u64);
	cvm_drv_fpa_free(buf, pkt.s.pool, 0);
}





/** Core: Call this routine to free the packet buffers in a work queue entry (WQE).
  * The packet pointer is made NULL and buf count is 0 in the WQE when this routine
  * returns.
  * @param wqe - the wqe whose buffers are to be freed.
  */
static inline void
cvm_free_wqe_packet_bufs(cvmx_wqe_t  *wqe)
{
#ifdef SANITY_CHECKS
	if(wqe->word2.s.bufs) {
		cvm_common_linked_buffer_sanity_and_count_update(wqe->packet_ptr,
		                               wqe->word2.s.bufs, 0);
	}
#endif
    cvmx_helper_free_packet_data(wqe);

    wqe->word2.s.bufs = 0;
    wqe->packet_ptr.u64 = 0;
    CVMX_SYNCWS;
}





/** Core: Free a work queue entry (WQE) and its buffers received from the
 *  hardware. 
 *  @param  wqe - work queue entry buffer to be freed.
 *  Frees a work queue entry and its packet data buffer.
 */
static inline void
cvm_free_host_instr(cvmx_wqe_t    *wqe)
{
   if(wqe == NULL) {
      printf("[ DRV ]: cvm_free_host_instr: NULL WQE\n");
      return;
   }
   DBG_PRINT(DBG_FLOW,"--free_host_instr, wqe @ %p, pkt @ %llx-\n",
             wqe, cast64(wqe->packet_ptr.s.addr));
   cvm_free_wqe_packet_bufs(wqe);
   DBG_PRINT(DBG_FLOW,"Freeing wqe @ 0x%p \n",wqe);
   cvm_drv_fpa_free(wqe, CVMX_FPA_WQE_POOL, 0);
}


#define  cvm_free_wqe_and_pkt_bufs(wqe) cvm_free_host_instr((wqe))




/** Core: Common Asynchronous allocation routine for all core driver components.
  * @param scratch_pad: scratch pad location where address of new buffer
  *                     will be written by hardware.
  * @param pool       : The pool from which async allocation is done.
  * @return If allocation was successful, return ptr to buffer, else NULL.
  */
static inline  void  *
cvm_drv_fpa_alloc_async(uint64_t  scratch_pad, uint64_t  pool)
{
   void  *buf = NULL;
   buf = cvm_common_alloc_fpa_buffer(scratch_pad, pool);
   DBG_PRINT(DBG_FLOW,"buffer allocated at 0x%p\n",buf);
   return buf;
}


typedef union {
    uint64_t   u64;
    struct {
#if __BYTE_ORDER == __LITTLE_ENDIAN
       uint64_t   tag:32;        /**< 31-00 Tag for the Packet. */
       cvmx_pow_tag_type_t tt:2; /**< 33-32 Tagtype */
       uint64_t   rs:1;          /**< 34    Is the PCI packet a RAW-SHORT? */
       uint64_t   grp:4;         /**< 38-35 The group that gets this Packet */
       uint64_t   qos: 3;        /**< 41-39 The QOS set for this Packet. */
       uint64_t   fsz:6;
       uint64_t   dlengsz:14;
       uint64_t   gather:1;
       uint64_t   r:1;           /**< 63    Is the PCI packet in RAW-mode? */
#else
       uint64_t   r:1;           /**< 63    Is the PCI packet in RAW-mode? */
       uint64_t   gather:1;
       uint64_t   dlengsz:14;
       uint64_t   fsz:6;
       uint64_t   qos: 3;        /**< 41-39 The QOS set for this Packet. */
       uint64_t   grp:4;         /**< 38-35 The group that gets this Packet */
       uint64_t   rs:1;          /**< 34    Is the PCI packet a RAW-SHORT? */
       cvmx_pow_tag_type_t tt:2; /**< 33-32 Tagtype */
       uint64_t   tag:32;        /**< 31-00 Tag for the Packet. */
#endif
    } s;
} cvm_pci_host_inst_hdr_t;



/* PCI Command in the Instruction Queue */
typedef struct   {

  uint64_t                   dptr;
  cvm_pci_host_inst_hdr_t    ih;
  uint64_t                   rptr;
  cvmx_pci_inst_irh_t        irh;

}octeon_instr_32B_t;




#endif

/* $Id: cvm-driver-defs.h 67088 2011-11-15 19:39:17Z mchalla $ */
