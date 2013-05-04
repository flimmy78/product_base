/***********************************************************************

  OCTEON TOOLKITS                                                         
  Copyright (c) 2007 Cavium Networks. All rights reserved.

  This file, which is part of the OCTEON TOOLKIT from Cavium Networks,
  contains proprietary and confidential information of Cavium Networks
  and in some cases its suppliers.

  Any licensed reproduction, distribution, modification, or other use of
  this file or confidential information embodied in this file is subject
  to your license agreement with Cavium Networks. The applicable license
  terms can be found by contacting Cavium Networks or the appropriate
  representative within your company.

  All other use and disclosure is prohibited.

  Contact Cavium Networks at info@caviumnetworks.com for more information.

 ************************************************************************/ 

#ifndef __CVM_COMMON_FPA_H__
#define __CVM_COMMON_FPA_H__

#include "cvmx.h"

#if defined (__KERNEL__)

#include <linux/kernel.h>
#include <linux/uio.h>
#include <asm-generic/memory_model.h>
#undef printf
#include <linux/mm.h>
#define printf printk
#include <asm-mips/page.h>
#include <linux/mm_types.h>

#endif

#include "cvmx-fau.h"

#if !defined(__KERNEL__)

/* structure to hold fpa buffer information */

#define CVM_COMMON_MAX_FPA_INFO_ENTRIES 5

typedef struct _cvm_common_fpa_info_entry {
    uint64_t start_addr;
    uint64_t end_addr;
    uint64_t element_size;
    uint64_t info_base;
} cvm_common_fpa_info_entry_t;

typedef struct _cvm_common_fpa_info {
    int count;
    int unused;
    cvm_common_fpa_info_entry_t entry[CVM_COMMON_MAX_FPA_INFO_ENTRIES];
} cvm_common_fpa_info_t;

/* fpa information */
extern cvm_common_fpa_info_t cvm_common_fpa_info[CVMX_FPA_NUM_POOLS];





#ifdef FPA_SANITY_BREAKPOINT
#define CVM_COMMON_BREAK asm volatile ("sdbbp 1");
#else
#define CVM_COMMON_BREAK 
#endif



extern CVMX_SHARED char pools_initialized;

/** Add FPA allocated memory information
 *
 *  @param pool       - FPA pool number
 *  @param buffer     - pointer to the start address of the allocated memory
 *  @param block_size - size of each individual buffer in this pool (in bytes)
 *  @param num_blocks - number of buffers allocated
 *  @param base       - address of extra memory allocated to keep track of FPA buffers
 *
 * This function is used to add the FPA allocated 
 * information to future reference.
 *
 * Note that this function should be called at
 * the initializations time by the boot core.
 *
 */
static inline int cvm_common_fpa_add_pool_info(uint64_t pool, void* buffer, uint64_t block_size, uint64_t num_blocks, uint64_t base)
{
    int ptr = 0;

    if (!pools_initialized)
    {
        int k = 0;
	for (k=0; k<CVMX_FPA_NUM_POOLS; k++) cvm_common_fpa_info[k].count = 0;

        pools_initialized = 1;
    }

    if (!buffer)
    {
        printf("ERROR: cvm_fpa_add_pool: NULL buffer pointer!\n");
        return(-1);
    }
    if (pool >= CVMX_FPA_NUM_POOLS)
    {
        printf("ERROR: cvm_fpa_add_pool: Illegal pool!\n");
        return(-1);
    }
    if (block_size < CVMX_FPA_MIN_BLOCK_SIZE)
    {
        printf("ERROR: cvm_fpa_add_pool: Block size too small.\n");
        return(-1);
    }

    if (cvm_common_fpa_info[pool].count == CVM_COMMON_MAX_FPA_INFO_ENTRIES)
    {
        printf("ERROR: cvm_fpa_add_pool: No more space for pool %lld to hold another entry!\n", CAST64(pool));
        return(-1);
    }

    ptr = cvm_common_fpa_info[pool].count;
    cvm_common_fpa_info[pool].entry[ptr].start_addr = CAST64(buffer);
    cvm_common_fpa_info[pool].entry[ptr].end_addr   = (CAST64(buffer) + (block_size*num_blocks))-1;
    cvm_common_fpa_info[pool].entry[ptr].element_size = block_size;
    cvm_common_fpa_info[pool].entry[ptr].info_base = base;
    ptr++;
    cvm_common_fpa_info[pool].count = ptr;

    return (0);
}


static inline int cvm_common_fpa_display_pool_info(uint64_t pool)
{
    int i=0;
    cvm_common_fpa_info_t pool_info = cvm_common_fpa_info[pool];

    printf("POOL %lld (each buffer is %lld bytes):\n", CAST64(pool), CAST64(pool_info.entry[0].element_size));
    for (i=0; i<pool_info.count; i++)
    {
      printf("  [%d] : start = 0x%llX,  end = 0x%llX, entries = %lld\n",
             i, 
             CAST64(pool_info.entry[i].start_addr), 
	     CAST64(pool_info.entry[i].end_addr),
             CAST64((pool_info.entry[i].end_addr- pool_info.entry[i].start_addr+1)/pool_info.entry[0].element_size));
    }

    printf("\n");

    return (0);
}


/** Display FPA allocated memory information
 *
 * This function displays various FPA allocated
 * pools and their corresponding memory usage
 *
 */
static inline int cvm_common_fpa_display_all_pool_info(void)
{
    int i = 0;
    for (i=0; i<CVMX_FPA_NUM_POOLS; i++)
    {
        if (cvm_common_fpa_info[i].count)
	{
            cvm_common_fpa_display_pool_info(i);
	}
    }

    return (0);
}


static inline int cvm_common_fpa_find_pool (uint64_t ptr, uint64_t *size)
{
    int pool = -1;
    cvm_common_fpa_info_t pool_info; 
    
    for (pool=0; pool<CVMX_FPA_NUM_POOLS; pool++)
    {
        pool_info = cvm_common_fpa_info[pool];
	
	if ( (uint64_t)CAST64(cvmx_phys_to_ptr(ptr)) >= pool_info.entry[pool].start_addr &&
	     (uint64_t)CAST64(cvmx_phys_to_ptr(ptr)) <= pool_info.entry[pool].end_addr) 
        {
            *size = pool_info.entry[pool].element_size; 

	    return (pool);
	}
    }

    CVM_COMMON_SIMPRINTF ("cvm_common_fpa_find_pool: address: 0x%llx not found in any pool\n", CAST64(cvmx_phys_to_ptr(ptr)));
    return (pool);
}

#define CVM_COMMON_FPA_AVAIL_COUNT(pool) cvmx_read_csr(CVMX_FPA_QUEX_AVAILABLE(pool))

#define CVM_COMMON_GET_FPA_USE_COUNT(pool) \
({ \
    uint32_t count = 0; \
    switch(pool) \
    { \
        case 0:  \
            count = cvmx_fau_fetch_and_add32(CVM_FAU_REG_POOL_0_USE_COUNT, 0); \
            break; \
        case 1:  \
            count = cvmx_fau_fetch_and_add32(CVM_FAU_REG_POOL_1_USE_COUNT, 0); \
            break; \
        case 2:  \
            count = cvmx_fau_fetch_and_add32(CVM_FAU_REG_POOL_2_USE_COUNT, 0); \
            break; \
        case 3:  \
            count = cvmx_fau_fetch_and_add32(CVM_FAU_REG_POOL_3_USE_COUNT, 0); \
            break; \
        case 4:  \
            count = cvmx_fau_fetch_and_add32(CVM_FAU_REG_POOL_4_USE_COUNT, 0); \
            break; \
        case 5:  \
            count = cvmx_fau_fetch_and_add32(CVM_FAU_REG_POOL_5_USE_COUNT, 0); \
            break; \
        case 6:  \
            count = cvmx_fau_fetch_and_add32(CVM_FAU_REG_POOL_6_USE_COUNT, 0); \
            break; \
        case 7:  \
            count = cvmx_fau_fetch_and_add32(CVM_FAU_REG_POOL_7_USE_COUNT, 0); \
            break; \
    } \
    count;\
})

#define CVM_COMMON_FPA_USE_COUNT_ADD(pool, val) \
{ \
    switch(pool) \
    { \
        case 0:  \
            cvmx_fau_atomic_add32(CVM_FAU_REG_POOL_0_USE_COUNT, val); \
            break; \
        case 1:  \
            cvmx_fau_atomic_add32(CVM_FAU_REG_POOL_1_USE_COUNT, val); \
            break; \
        case 2:  \
            cvmx_fau_atomic_add32(CVM_FAU_REG_POOL_2_USE_COUNT, val); \
            break; \
        case 3:  \
            cvmx_fau_atomic_add32(CVM_FAU_REG_POOL_3_USE_COUNT, val); \
            break; \
        case 4:  \
            cvmx_fau_atomic_add32(CVM_FAU_REG_POOL_4_USE_COUNT, val); \
            break; \
        case 5:  \
            cvmx_fau_atomic_add32(CVM_FAU_REG_POOL_5_USE_COUNT, val); \
            break; \
        case 6:  \
            cvmx_fau_atomic_add32(CVM_FAU_REG_POOL_6_USE_COUNT, val); \
            break; \
        case 7:  \
            cvmx_fau_atomic_add32(CVM_FAU_REG_POOL_7_USE_COUNT, val); \
            break; \
    } \
}



/** Checks whether the address belongs to correct FPA pool or not
 *
 *  @param pool     - FPA pool number
 *  @param ptr      - address to verify
 *
 * This macro checks that the address 'ptr' belongs
 * to the 'pool' or not.
 *
 */
#define CVM_COMMON_CHECK_FPA_POOL(pool, ptr) \
({ \
    char correct_pool = 0; \
    cvm_common_fpa_info_t pool_info = cvm_common_fpa_info[pool]; \
    int i=0; \
    for (i=0; i<pool_info.count; i++) \
    { \
      if (( (uint64_t)(CAST64(cvmx_phys_to_ptr(CAST64(ptr)))) >= pool_info.entry[i].start_addr) && ( (uint64_t)(CAST64(cvmx_phys_to_ptr(CAST64(ptr)))) <= pool_info.entry[i].end_addr)) \
        { \
            correct_pool = 1; \
            break; \
        } \
    } \
    correct_pool; \
})


/** Macro to return FPA pool number for a memory address
 *
 *  @param ptr      - address pointer
 *
 * This macro goes through the stored information to
 * find out which pool the pointer 'ptr' belongs to
 *
 */
#define CVM_COMMON_WHICH_FPA_POOL(ptr) \
({ \
    int pool = -1; \
    int i = 0; \
    for (i=0; i<CVMX_FPA_NUM_POOLS; i++) \
    { \
        cvm_common_fpa_info_t pool_info = cvm_common_fpa_info[i]; \
        int j=0; \
        for (j=0; j<pool_info.count; j++) \
        { \
	  if (( (uint64_t)(CAST64(cvmx_phys_to_ptr(CAST64(ptr)))) >= pool_info.entry[j].start_addr) && ((uint64_t)(CAST64(cvmx_phys_to_ptr(CAST64(ptr)))) <= pool_info.entry[j].end_addr)) \
            { \
                pool = i; \
                break; \
            } \
        } \
        if (pool != -1) break; \
    } \
    pool;\
})

#define CVM_COMMON_WHICH_FPA_POOL_SEGMENT(ptr, pool) \
({ \
    int segment = -1; \
    cvm_common_fpa_info_t pool_info = cvm_common_fpa_info[pool]; \
    int j = 0; \
    for (j=0; j<pool_info.count; j++) \
    { \
      if (((uint64_t)(CAST64(cvmx_phys_to_ptr(CAST64(ptr)))) >= pool_info.entry[j].start_addr) && ((uint64_t)(CAST64(cvmx_phys_to_ptr(CAST64(ptr)))) <= pool_info.entry[j].end_addr)) \
        { \
            segment = j; \
            break; \
        } \
    } \
    segment;\
})

#endif /* __KERNEL__*/


#define CVM_COMMON_MARK_FPA_BUFFER_ALLOC(ptr, pool, label);
#define CVM_COMMON_MARK_FPA_BUFFER_FREE(ptr, pool, label);

static inline int cvm_common_mark_fpa_buffer_alloc(char* file, int line, uint64_t ptr, uint32_t pool, int label)
{
    return (0);
}

static inline int cvm_common_mark_fpa_buffer_free(char* file, int line, uint64_t ptr, uint32_t pool, int label)
{
    return (0);
}

#define CVM_COMMON_CHECK_BUFFER_BOUNDRIES(ptr, pool, back, calculate_ptr_using_back) \
({ \
  int ret = 0; \
  ret; \
})

static inline int cvm_common_check_buffer_boundries(char* file, int line, uint64_t ptr, int pool, int back, int calculate_ptr_using_back)
{
    return (0);
}

#define CVM_COMMON_INIT_FPA_CHECKS(pool_mem_base, pool_count, single_buffer_size) \
({ \
    void* base = NULL; \
    base; \
})

#define IS_PTR_WITHIN_BUFFER(ptr_in_buffer, ptr_to_test) \
({ \
  int ret = 0; \
  ret; \
})



static inline int cvm_common_is_fpa_buffer_available(uint64_t scratch_ptr)
{
#if defined(__KERNEL__) && defined (linux)
	/* FIXME: */
	return 1;
#else
    uint64_t  so = 0; 
    so = cvmx_scratch_read64(scratch_ptr);
    if (so == CVM_COMMON_ALLOC_INIT)
    {
        CVMX_SYNCIOBDMA;
        so = cvmx_scratch_read64(scratch_ptr);
    }

    if (so == 0x0)
    {
        return (0);
    }
    return (1);
#endif
}


#define CVM_COMMON_INC_OOB_COUNT(pool) \
{ \
    int offset = pool*4 + CVM_FAU_REG_FPA_OOB_COUNT; \
    cvmx_fau_atomic_add32( offset, 1); \
}


/** Macro to allocate an FPA buffer
 *
 *  @param pool     - FPA pool from which the buffer should be allocated
 *
 * This macro is a wraper around the cvmx_fpa_alloc()
 * executive function.
 *
 */


#define cvm_common_alloc_fpa_buffer_sync(pool) _alloc_fpa_buffer_sync(__FILE__, __LINE__, pool)

static inline void* _alloc_fpa_buffer_sync(char* file, int line, uint64_t pool)
{
    void *so = NULL;
#if defined(__KERNEL__)
    so = (void *)cvmx_fpa_alloc(pool);
    if (so == NULL)
    {
	CVM_COMMON_INC_OOB_COUNT(pool);
	return NULL;
    }
    return so;

#else

    CVM_COMMON_TRACE_P1 (CVM_COMMON_TRACE_ENTER, CVM_COMMON_TRACE_FNPTR, cvm_common_alloc_fpa_buffer_sync, pool);
  
    so = (void *)cvmx_fpa_alloc(pool);
    if (so == 0)
    {
        /* out of buffers */
        CVM_COMMON_INC_OOB_COUNT(pool);
        CVM_COMMON_TRACE_P1 (CVM_COMMON_TRACE_EXIT, CVM_COMMON_TRACE_FNPTR, cvm_common_alloc_fpa_buffer_sync, so);
        return (NULL);
    }

    CVM_COMMON_TRACE_P1 (CVM_COMMON_TRACE_BODY, CVM_COMMON_TRACE_FNPTR, cvm_common_alloc_fpa_buffer_sync, so);


    CVM_COMMON_TRACE_P1 (CVM_COMMON_TRACE_EXIT, CVM_COMMON_TRACE_FNPTR, cvm_common_alloc_fpa_buffer_sync, so);
    return (so);
#endif /*__KERNEL__*/
}



/** Macro to allocate an FPA buffer asynchronously
 *
 *  @param scratch_ptr - Scratch pad register address
 *  @param pool        - FPA pool from which the buffer should be allocated
 *
 * This macro is a wraper around the cvmx_fpa_alloc()
 * executive function. It tries to read an already
 * allocated FPA buffer address from the scratch pad
 * register. Once that is done, another buffer allocation
 * request is initiated to work in the background and
 * results are stored in the same scratch pad register.
 * This register value is used for the next allocation.
 *
 */


#define cvm_common_alloc_fpa_buffer(scratch_ptr, pool) _alloc_fpa_buffer(__FILE__, __LINE__, scratch_ptr, pool)

static inline void* _alloc_fpa_buffer(char* file, int line, uint64_t scratch_ptr, uint64_t pool)
{
#if defined(USE_SYNC_ALLOC) || defined (__KERNEL__)
  return ( _alloc_fpa_buffer_sync(file, line, pool) );
#else

    void *so_ptr = NULL;
    uint64_t  so64 = 0;

    CVM_COMMON_TRACE_P2 (CVM_COMMON_TRACE_ENTER, CVM_COMMON_TRACE_FNPTR, cvm_common_alloc_fpa_buffer, scratch_ptr, pool);

    so64 = cvmx_scratch_read64(scratch_ptr);
    if (so64 == CVM_COMMON_ALLOC_INIT)
    {
        CVMX_SYNCIOBDMA;
        so64 = cvmx_scratch_read64(scratch_ptr);
    }

    CVM_COMMON_TRACE_P1 (CVM_COMMON_TRACE_BODY, CVM_COMMON_TRACE_FNPTR, cvm_common_alloc_fpa_buffer, so64);

    if (so64 == 0)
    {
        /* out of buffers */
        CVM_COMMON_INC_OOB_COUNT(pool);
        CVM_COMMON_TRACE_P1 (CVM_COMMON_TRACE_EXIT, CVM_COMMON_TRACE_FNPTR, cvm_common_alloc_fpa_buffer, so64);
    }
    else
    {
        so_ptr = cvmx_phys_to_ptr(so64);
    }

    /* set scratch reg to CVM_COMMON_ALLOC_INIT so that we can verify a new ptr is written */
    cvmx_scratch_write64(scratch_ptr, CVM_COMMON_ALLOC_INIT);
    CVMX_SYNCWS;
    cvmx_fpa_async_alloc(scratch_ptr, pool);

    CVM_COMMON_TRACE_P1 (CVM_COMMON_TRACE_EXIT, CVM_COMMON_TRACE_FNPTR, cvm_common_alloc_fpa_buffer, so64);
    return (so_ptr);
#endif /* USE_SYNC_ALLOC */
}



/** Macro to free an allocated FPA buffer
 *
 *  @param ptr       - FPA address to be freed
 *  @param pool      - FPA pool to which the buffer should be freed
 *  @num_cache_lines - number of cache lines to be invalidated
 *
 * This macro is a wraper around the cvmx_fpa_free()
 * executive function. 
 */


#define cvm_common_free_fpa_buffer(ptr, pool, num_cache_lines) _free_fpa_buffer(__FILE__, __LINE__, ptr, pool, num_cache_lines)

static inline void _free_fpa_buffer(char* file, int line, void *ptr, uint64_t pool, uint64_t num_cache_lines)
{
#if defined(__KERNEL__)
  cvmx_buf_ptr_t buffer = (cvmx_buf_ptr_t) ptr;

  CVMX_SYNCW;

  /* use bit 35 to mark struct page buffer */
  if (buffer.s.addr & 0x800000000)
  {
      /* free the page here; make sure to move back address to page boundry */
      struct page *page = NULL;
      uint64_t ptr_u64 = (uint64_t) ptr & (~0x800000000ULL);

      ptr_u64 &= PAGE_MASK;
      ptr_u64 = cvmx_ptr_to_phys(CASTPTR(void, ptr_u64));
      page = pfn_to_page(ptr_u64 >> PAGE_SHIFT);
      put_page(page);
  }
  else
  {
      cvmx_fpa_free(ptr, pool, 0);
  }
#else

  CVM_COMMON_TRACE_P3 (CVM_COMMON_TRACE_ENTER, CVM_COMMON_TRACE_FNPTR, cvm_common_free_fpa_buffer, ptr, pool, num_cache_lines);


    CVMX_SYNCWS;

    if (pool == CVMX_FPA_WQE_POOL)
    {
       cvmx_fpa_free(ptr, pool, CVMX_FPA_WQE_POOL_SIZE / CVMX_CACHE_LINE_SIZE);
    }
    else
    {
       cvmx_fpa_free(ptr, pool, 0);
       num_cache_lines = 0; /* keep the compiler happy */
    }

    CVM_COMMON_TRACE_P2 (CVM_COMMON_TRACE_EXIT, CVM_COMMON_TRACE_FNPTR, cvm_common_free_fpa_buffer, ptr, pool);
#endif /* __KERNEL __*/
}




/** Macro to free an allocated FPA buffer after adjusting the
 *  start address correctly using the back field
 *
 *  @param ptr       - FPA address to be freed (cvmx_buf_ptr_t format)
 *
 * This macro calculates the start address of the
 * buffer correctly using the 'back' field before
 * freeing the buffer. The pool information is also
 * extracted from the 'ptr' parameter.
 *
 */
#define cvm_common_mbuf_free(ptr) _mbuf_free(__FILE__, __LINE__, ptr)

static inline void _mbuf_free(char* file, int line, cvmx_buf_ptr_t ptr)
{
   uint64_t num_cache_lines = 0;

   CVM_COMMON_TRACE_P1 (CVM_COMMON_TRACE_ENTER, CVM_COMMON_TRACE_FNPTR, cvm_common_mbuf_free, ptr.u64);

   if ((ptr.s.pool == CVMX_FPA_WQE_POOL) && (ptr.s.back == CVM_COMMON_MALLOC_BACK_VAL))
   {
       CVM_COMMON_FREE_TSAFE((void *)(cvmx_phys_to_ptr(ptr.s.addr)) );
   }
   else
   {
      switch (ptr.s.pool)
      {
	 case 0:
	    num_cache_lines = CVMX_FPA_POOL_0_SIZE / CVMX_CACHE_LINE_SIZE;
	    break;
	 case 1:
	    num_cache_lines = CVMX_FPA_POOL_1_SIZE / CVMX_CACHE_LINE_SIZE;
	    break;
	 case 2:
	    num_cache_lines = CVMX_FPA_POOL_2_SIZE / CVMX_CACHE_LINE_SIZE;
	    break;
	 case 3:
	    num_cache_lines = CVMX_FPA_POOL_3_SIZE / CVMX_CACHE_LINE_SIZE;
	    break;
	 case 4:
	    num_cache_lines = CVMX_FPA_POOL_4_SIZE / CVMX_CACHE_LINE_SIZE;
	    break;
	 case 5:
	    num_cache_lines = CVMX_FPA_POOL_5_SIZE / CVMX_CACHE_LINE_SIZE;
	    break;
	 case 6:
	    num_cache_lines = CVMX_FPA_POOL_6_SIZE / CVMX_CACHE_LINE_SIZE;
	    break;
	 case 7:
	    num_cache_lines = CVMX_FPA_POOL_7_SIZE / CVMX_CACHE_LINE_SIZE;
	    break;
      }

      _free_fpa_buffer(file, line,  (void *)(cvmx_phys_to_ptr((ptr.s.addr & ~127) - 128*ptr.s.back)), ptr.s.pool, num_cache_lines);

   }

   CVM_COMMON_TRACE_P1 (CVM_COMMON_TRACE_EXIT, CVM_COMMON_TRACE_FNPTR, cvm_common_mbuf_free, ptr.u64);
   return;
}


/** Macro to free linked/gather buffer pointed to
 *  by the packet_ptr field of work queue entry
 *
 *  @param swp       - pointer to work queue entry
 *
 * This macro frees linked or gather list pointed 
 * to by the packet_ptr field of work queue entry.
 *
 */
#define cvm_common_packet_free(swp) _packet_free(__FILE__, __LINE__, swp)

static inline void _packet_free(char* file, int line, cvm_common_wqe_t *swp)
{
   int i;
 
   CVM_COMMON_TRACE_P2 (CVM_COMMON_TRACE_ENTER, CVM_COMMON_TRACE_FNPTR, cvm_common_packet_free, swp, swp->control.gth);

   if (swp->control.gth == 0)
   {
       cvmx_buf_ptr_t next_ptr;
       cvmx_buf_ptr_t ptr = swp->hw_wqe.packet_ptr;
       uint8_t *ptr_virt =  cvmx_phys_to_ptr(ptr.s.addr);
       int buf_cnt = swp->hw_wqe.word2.s.bufs;

       CVM_COMMON_TRACE_P2 (CVM_COMMON_TRACE_BODY, CVM_COMMON_TRACE_FNPTR, cvm_common_packet_free, ptr.u64, buf_cnt);

       if (buf_cnt > 0) 
       {
	   for (i=0; i<(buf_cnt - 1); i++)
	   {
	       next_ptr = *((cvmx_buf_ptr_t *)(ptr_virt - 8));

   	       _mbuf_free(file, line, ptr);
	       ptr = next_ptr;
               ptr_virt =  cvmx_phys_to_ptr(ptr.s.addr);
	   }
	   _mbuf_free(file, line, ptr);
       }
   }
   else
   {
        cvmx_buf_ptr_t  *gather_list_addr = (cvmx_buf_ptr_t *)cvmx_phys_to_ptr(swp->hw_wqe.packet_ptr.s.addr);

        CVM_COMMON_TRACE_P2 (CVM_COMMON_TRACE_BODY, CVM_COMMON_TRACE_FNPTR, cvm_common_packet_free, CAST64(gather_list_addr), swp->hw_wqe.word2.s.bufs);

        for (i=0; i<swp->hw_wqe.word2.s.bufs; i++)
	{
	    if (gather_list_addr[i].s.i == 1)
	    {
	        _mbuf_free(file, line, gather_list_addr[i]);
	    }
	}
	_mbuf_free(file, line, swp->hw_wqe.packet_ptr);
   }

   CVM_COMMON_TRACE_P1 (CVM_COMMON_TRACE_EXIT, CVM_COMMON_TRACE_FNPTR, cvm_common_packet_free, swp);
}


/** Macro to free both the linked/gather buffer pointed 
 *  to by the packet_ptr field of work queue entry as
 *  well as the work queue entry itself
 *
 *  @param swp       - pointer to work queue entry
 *
 * This macro frees linked or gather list pointed 
 * to by the packet_ptr field of work queue entry
 * as well as the work qneue entry
 *
 */
#define cvm_common_discard_swp(swp) _discard_swp(__FILE__, __LINE__, swp)

static inline void _discard_swp(char* file, int line, cvm_common_wqe_t *swp)
{
   cvm_common_wqe_t *tmp_swp;

   CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_7, "discard_swp: %p\n", swp);
   CVM_COMMON_TRACE_P1 (CVM_COMMON_TRACE_ENTER, CVM_COMMON_TRACE_FNPTR, cvm_common_discard_swp, swp);

   while (swp) 
   {
       _packet_free(file, line, swp);
      tmp_swp = swp;
      swp = swp->next;
      _free_fpa_buffer(file, line, tmp_swp, CVMX_FPA_WQE_POOL, CVMX_FPA_WQE_POOL_SIZE / CVMX_CACHE_LINE_SIZE);
   }
   CVM_COMMON_TRACE_P1 (CVM_COMMON_TRACE_EXIT, CVM_COMMON_TRACE_FNPTR, cvm_common_discard_swp, swp);
}


/*
 * Function prototypes
 */
void* cvm_common_setup_fpa_pool(int pool, int entry_size, int buf_count, char* pool_name);


#endif /* _CVM_COMMON_FPA_H__ */
