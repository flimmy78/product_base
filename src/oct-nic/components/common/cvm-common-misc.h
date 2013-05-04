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

#ifndef __CVM_COMMON_MISC_H__
#define __CVM_COMMON_MISC_H__


#if !(defined(__KERNEL__) && defined(linux))
#include <string.h>
#endif

#if !(defined(__KERNEL__) && defined(linux))

#include "cvmx-scratch.h"

static inline void cvm_common_set_cycle(uint64_t cycle)
{
    uint32_t val_low  = cycle & 0xffffffff;
    uint32_t val_high = cycle  >> 32;
    
    uint32_t tmp; /* temp register */

    asm volatile (
        "  .set push\n"
        "  .set mips64                       \n"
        "  .set noreorder                    \n"
        /* Standard twin 32 bit -> 64 bit construction */
        "  dsll  %[valh], 32                 \n"
        "  dla   %[tmp], 0xffffffff          \n"
        "  and   %[vall], %[tmp], %[vall]    \n"
        "  daddu %[valh], %[valh], %[vall]   \n"
        /* Combined value is in valh */
        "  dmtc0 %[valh],$9, 6               \n"
        "  .set pop\n"                                             \
         :[tmp] "=&r" (tmp) : [valh] "r" (val_high), [vall] "r" (val_low) );

}


#endif /* KERNEL */

static inline void cvm_common_enqueue_frame (cvm_common_wqe_t *swp)
{
   if (out_swp == NULL)
   {
      swp->next = out_swp;
      out_swp = out_swp_tail = swp;
   }
   else
   {
      swp->next = NULL;
      out_swp_tail->next = swp;
      out_swp_tail = swp;
   }
}


#if !(defined(__KERNEL__) && defined(linux))
#ifdef REAL_HW

/* support routines and macros to output the results to LED display */
static inline int cvm_common_int2ascii_hex(int number, int digit_count, char* str)
{
  int max_digits = 8;
  int i = 0, j = 0;

  if (digit_count > max_digits) return (-1);

  memset(str, 0x30, max_digits);
  str[max_digits] = 0x0;

  j = max_digits - 1;

  for (i=0; i<digit_count; i++)
  {
    char no = 0;
    no = (number & 0xf);

    if ((no >= 0x0) && (no <= 0x9))      no = '0'+no;
    else if ((no >= 0xA) && (no <= 0xF)) no = 'A'+ (no - 10);

    str[j--] = no;
    number >>= 4;
  }

  return (0);
}

#define CVM_COMMON_LED_OUT(hexnum,digits) \
{ \
    char str[8+1]; \
    cvm_common_int2ascii_hex(hexnum, digits, str); \
    ebt3000_str_write(str); \
}

#endif /* REAL_HW */


/* Linked buffer size setup at input */

typedef union 
{
    uint64_t u64;

    struct 
    {
        uint16_t mbuff_size;              /* mbuff size in words */
        uint16_t mbuff_skip_first;        /* skip for first mbuff */
        uint16_t mbuff_skip_not_first;    /* skip for the rest of mbuffs */
        uint16_t initialized;             /* set to 1 if this structure is initialized */
    } s;
} cvm_common_mbuf_size_t;


static inline int cvm_common_get_max_buf_size(cvmx_buf_ptr_t packet_ptr, int bufs)
{
    int max_buf_size = 0;

    cvm_common_mbuf_size_t mbuf_info;

    mbuf_info.u64 = cvmx_scratch_read64(CVM_SCR_MBUFF_INFO_PTR);

    if (bufs)
    {
        max_buf_size = (( mbuf_info.s.mbuff_size & 0xfff)
                      - ( (mbuf_info.s.mbuff_skip_not_first & 0x3f) + 1)) * 8;
    }
    else
    {
        max_buf_size = ( (( mbuf_info.s.mbuff_size & 0xfff)
                       - (( mbuf_info.s.mbuff_skip_first & 0x3f) + 1) ) * 8)
                       - (packet_ptr.s.addr & 0x7);
    }

    return (max_buf_size);
}


static inline int cvm_common_fix_packet_size(cvmx_wqe_t *swp)
{
    int remaining_len = cvmx_wqe_get_len(swp);
    int i = 0;
    uint64_t raw_short_mask = 0x400000000ull;
    cvmx_buf_ptr_t *ptr=NULL;
    int max_data = 0;
    int current_data_len = 0;

    /* check if the packet is from PCI - if it is raw short, don't do anything */
    if ( (swp->word2.s.software) && ( ( (cvmx_wqe_get_port(swp) >= 32) && (cvmx_wqe_get_port(swp) <= 35) ) ) )
    {
        /* wqe is from the PCI */
         uint64_t inst_hdr = swp->packet_data[0];

	 if ( inst_hdr & raw_short_mask )
	 {
	     return (0);
	 }
    }

    /* get the first pointer */
    ptr = (cvmx_buf_ptr_t*)&(swp->packet_ptr.u64);

    for (i=0; i<swp->word2.s.bufs; i++)
    {
        max_data = cvm_common_get_max_buf_size(*ptr, i);
        current_data_len = cvm_common_imin(remaining_len, max_data);

        ptr->s.size = current_data_len;
	remaining_len -= current_data_len;

	/* update pointer if there are more links */
	if ( (i+1) < swp->word2.s.bufs)
	{
	    ptr = (cvmx_buf_ptr_t*)(((uint8_t *)cvmx_phys_to_ptr(ptr->s.addr))-8);
	}
    }

    return (0);
}

/* check the coremask for the last available core */
static inline uint8_t cvm_common_get_last_core(unsigned int mask)
{
   uint8_t i;
   uint8_t buf = 0;
                                                                                                                
   for(i=0;i<16;i++) {
        if(mask & (1 << i)) {
                buf = i;
        }
   }
   return buf;
}



/* Global per core history  */
#define CVM_COMMON_TRACE_MAX_ENTRIES    1000
#define CVM_COMMON_TRACE_MAX_INFO_FIELDS 4
#define CVM_COMMON_TRACE_ENTER       0
#define CVM_COMMON_TRACE_BODY        1
#define CVM_COMMON_TRACE_EXIT        2
#define CVM_COMMON_TRACE_FNPTR       0
#define CVM_COMMON_TRACE_NOT_FNPTR   1


#define CVM_COMMON_TRACE_MACRO_MARK_BUFFER_ALLOC     0x0
#define CVM_COMMON_TRACE_MACRO_MARK_BUFFER_FREE      0x1

extern char* cvm_common_trace_macro_names[];

typedef struct _cvm_common_trace_entry
{
    union
    {
        uint64_t  u64;
        struct
        {
            uint64_t   entry_exit  : 2;
            uint64_t   is_fn_ptr   : 1;
            uint64_t   info_count  : 4;
	    uint64_t   unused      : 5;
            uint64_t   line_no     : 12;
            uint64_t   addr        : 40;
        } s;
    } trace_info;

    uint64_t cycles;
    uint64_t u_64_info[CVM_COMMON_TRACE_MAX_INFO_FIELDS];

} cvm_common_trace_entry;

typedef struct _cvm_common_trace
{
    int cur_index;
    int no_of_entries;
    cvm_common_trace_entry entry[CVM_COMMON_TRACE_MAX_ENTRIES];
} cvm_common_trace_t;


typedef struct _cvm_common_per_core_history {
    uint64_t current_cycle;
#ifdef CVM_FLOW_TRACE
    cvm_common_trace_t trace;
#endif
} cvm_common_per_core_history_t;


typedef struct _cvm_common_history {
    cvm_common_per_core_history_t per_core_history[CVMX_MAX_CORES];
} cvm_common_history_t;

extern CVMX_SHARED cvm_common_history_t *cvm_common_history;

/* helping macros for common history */
#ifdef CVM_CLI_APP

#define CVM_COMMON_HISTORY_SET_CYCLE() \
{ \
    if (cvm_common_history) \
    { \
        cvm_common_history->per_core_history[cvmx_get_core_num()].current_cycle = cvmx_get_cycle(); \
    } \
}

#define CVM_COMMON_HISTORY_GET_CYCLE(core_num) \
({ \
    uint64_t c_val = 0; \
    if (cvm_common_history) \
    { \
        c_val = cvm_common_history->per_core_history[core_num].current_cycle; \
    } \
    c_val; \
})
#else
#define CVM_COMMON_HISTORY_SET_CYCLE() while(0);
#define CVM_COMMON_HISTORY_GET_CYCLE(core_num) \
({ \
    int c_val = 0; \
    c_val; \
})
#endif


#endif /* KERNEL */

static inline void cvm_common_buffer_init_fast(void *p, uint64_t size)
{
   uint64_t num_cache_lines = ((size + (CVMX_CACHE_LINE_SIZE - 1)) / CVMX_CACHE_LINE_SIZE) << 1;
   uint64_t *ptr = (uint64_t *) p;
   while(num_cache_lines-- > 0) {
      *ptr++ = 0x0L;
      *ptr++ = 0x0L;
      *ptr++ = 0x0L;
      *ptr++ = 0x0L;
      *ptr++ = 0x0L;
      *ptr++ = 0x0L;
      *ptr++ = 0x0L;
      *ptr++ = 0x0L;
   };
}

#if defined(CVM_CLI_APP) && defined(CVM_FLOW_TRACE)

#define CVM_COMMON_TRACE(entry_exit, is_fn_ptr, fn_addr)                             CVM_COMMON_TRACE_INFO (entry_exit, is_fn_ptr, fn_addr, 0, 0, 0, 0, 0)
#define CVM_COMMON_TRACE_P1(entry_exit, is_fn_ptr, fn_addr, inf1)                    CVM_COMMON_TRACE_INFO (entry_exit, is_fn_ptr, fn_addr, 1, inf1, 0, 0, 0)
#define CVM_COMMON_TRACE_P2(entry_exit, is_fn_ptr, fn_addr, inf1, inf2)              CVM_COMMON_TRACE_INFO (entry_exit, is_fn_ptr, fn_addr, 2, inf1, inf2, 0, 0)
#define CVM_COMMON_TRACE_P3(entry_exit, is_fn_ptr, fn_addr, inf1, inf2, inf3)        CVM_COMMON_TRACE_INFO (entry_exit, is_fn_ptr, fn_addr, 3, inf1, inf2, inf3, 0)
#define CVM_COMMON_TRACE_P4(entry_exit, is_fn_ptr, fn_addr, inf1, inf2, inf3, inf4)  CVM_COMMON_TRACE_INFO (entry_exit, is_fn_ptr, fn_addr, 4, inf1, inf2, inf3, inf4)


#define CVM_COMMON_TRACE_INFO(t_entry_exit, t_is_fn_ptr, t_fn_addr, t_info_count, t_info1, t_info2, t_info3, t_info4) \
{ \
    uint64_t u64_trace_info=0; \
    int core_num = cvmx_get_core_num(); \
    int cur_index = cvm_common_history->per_core_history[core_num].trace.cur_index; \
    int entry_count = cvm_common_history->per_core_history[core_num].trace.no_of_entries; \
    cvm_common_history->per_core_history[core_num].trace.entry[cur_index].cycles = cvmx_get_cycle(); \
    u64_trace_info = ((((uint64_t)t_fn_addr)&0x000000ffffffffffull) | \
                      ((((uint64_t)(__LINE__))<<40)&0x000fff0000000000ull) | \
                      ((((uint64_t)(t_info_count))<<57)&0x1e00000000000000ull) | \
                      ((((uint64_t)(t_is_fn_ptr))<<61)&0x2000000000000000ull) | \
                      ((((uint64_t)(t_entry_exit))<<62)&0xc000000000000000ull)); \
    cvm_common_history->per_core_history[core_num].trace.entry[cur_index].trace_info.u64 = u64_trace_info; \
    cvm_common_history->per_core_history[core_num].trace.entry[cur_index].trace_info.s.info_count = t_info_count; \
    cvm_common_history->per_core_history[core_num].trace.entry[cur_index].u_64_info[0] = (uint64_t)t_info1; \
    cvm_common_history->per_core_history[core_num].trace.entry[cur_index].u_64_info[1] = (uint64_t)t_info2; \
    cvm_common_history->per_core_history[core_num].trace.entry[cur_index].u_64_info[2] = (uint64_t)t_info3; \
    cvm_common_history->per_core_history[core_num].trace.entry[cur_index].u_64_info[3] = (uint64_t)t_info4; \
    cur_index++; \
    if (cur_index >= CVM_COMMON_TRACE_MAX_ENTRIES) cur_index = 0; \
    cvm_common_history->per_core_history[core_num].trace.cur_index = cur_index; \
    if (entry_count < CVM_COMMON_TRACE_MAX_ENTRIES) entry_count++; \
    cvm_common_history->per_core_history[core_num].trace.no_of_entries = entry_count; \
}

#define CVM_COMMON_GET_HISTORY(core_num) ((cvm_common_trace_t*)(&cvm_common_history->per_core_history[core_num].trace))

#else
#define CVM_COMMON_TRACE(entry_exit, is_fn_ptr, fn_addr);
#define CVM_COMMON_TRACE_P1(entry_exit, is_fn_ptr, fn_addr, inf1);
#define CVM_COMMON_TRACE_P2(entry_exit, is_fn_ptr, fn_addr, inf1, inf2);
#define CVM_COMMON_TRACE_P3(entry_exit, is_fn_ptr, fn_addr, inf1, inf2, inf3);
#define CVM_COMMON_TRACE_P4(entry_exit, is_fn_ptr, fn_addr, inf1, inf2, inf3, inf4);
#define CVM_COMMON_TRACE_INFO(t_entry_exit, t_is_fn_ptr, t_fn_addr, t_info_count, t_info1, t_info2, t_info3, t_info4);
#define CVM_COMMON_GET_HISTORY(core_num);
#endif


#ifdef EXTRA_STATS

#define CVM_COMMON_EXTRA_STATS_ADD32(fau, val)   cvmx_fau_atomic_add32 ( (fau), (val) )
#define CVM_COMMON_EXTRA_STATS_ADD64(fau, val)   cvmx_fau_atomic_add64 ( (fau), (val) )
#define CVM_COMMON_EXTRA_STATS_GET32(fau) \
({ \
    uint32_t retval = 0; \
    retval = cvmx_fau_fetch_and_add32 (fau, 0); \
    retval; \
})
#define CVM_COMMON_EXTRA_STATS_GET64(fau) \
({ \
    uint64_t retval = 0; \
    retval = cvmx_fau_fetch_and_add64 (fau, 0); \
    retval; \
})

#else

#define CVM_COMMON_EXTRA_STATS_ADD32(fau, val);
#define CVM_COMMON_EXTRA_STATS_ADD64(fau, val);
#define CVM_COMMON_EXTRA_STATS_GET32(fau) \
({ \
    uint32_t retval = 0; \
    retval; \
})
#define CVM_COMMON_EXTRA_STATS_GET64(fau) \
({ \
    uint64_t retval = 0; \
    retval; \
})

#endif /* #ifdef EXTRA_STATS */


inline int cvm_common_packet_copy (cvmx_buf_ptr_t *ptr_to, cvmx_buf_ptr_t *ptr_from, int num_bufs, int total_len);
int  cvm_common_swp_packet_copy(cvm_common_wqe_t **swp_to, cvm_common_wqe_t *swp_from);
void cvm_common_packet_ptr_dump(cvm_common_wqe_t *swp);
void cvm_common_gather_list_dump(cvm_common_wqe_t *swp);

#endif /* __CVM_COMMON_MISC_H__ */
