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

#ifndef __CVM_COMMON_DEFS_H__
#define __CVM_COMMON_DEFS_H__

#ifdef INET6
#undef CVM_IP_DYNAMIC_SHORT_SUPPORT
#endif

#ifdef CVM_IP_DYNAMIC_SHORT_SUPPORT
#define CVM_COMMON_PD_ALIGN (4+16) /* Hardware aligns IP header in packet_data */
#define CVM_COMMON_IP6_PD_ALIGN 16 /* Hardware aligns IP header in packet_data */
#else
#define CVM_COMMON_PD_ALIGN 4 /* Hardware aligns IP header in packet_data */
#define CVM_COMMON_IP6_PD_ALIGN 0 /* Hardware aligns IP header in packet_data */
#endif

#define CVM_COMMON_BIG_ENDIAN 4321
#define CVM_COMMON_LITTLE_ENDIAN 1234
#define CVM_COMMON_BYTE_ORDER CVM_COMMON_BIG_ENDIAN

#define CVM_COMMON_WQE_INVALID                      0xff
#define CVM_COMMON_WQE_TIMER_INTERRUPT              1
#define CVM_COMMON_WQE_LONG_TIMER_INTERRUPT         2
#define CVM_COMMON_WQE_APP_TO_TCP                   3
#define CVM_COMMON_WQE_APP_TO_UDP                   4
#define CVM_COMMON_WQE_ENET_SLOW_TIMER_INTERRUPT    5
#define CVM_COMMON_WQE_APP_TO_RAW                   6
#define CVM_COMMON_WQE_ND6_TIMER_INTERRUPT          7
#define CVM_COMMON_WQE_ND6_SLOW_TIMER_INTERRUPT     8
#define CVM_COMMON_WQE_ND6_LLINFO_TIMER_INTERRUPT   9
#define CVM_COMMON_WQE_ND6_DAD_TIMER_INTERRUPT      10
#define CVM_COMMON_WQE_IPSEC_DECRYPT_DONE           11

/* Timer ID for IGMP fast 200ms timer */
#define CVM_COMMON_WQE_IGMP_FAST_TIMER_INTERRUPT    12

/* Timer ID for IGMP slow 500ms timer */
#define CVM_COMMON_WQE_IGMP_SLOW_TIMER_INTERRUPT    13

#define CVM_COMMON_MINUS_ONE_64 (0xffffffffffffffffULL)
#define CVM_COMMON_ALLOC_INIT   (0xBADDEED0BADDEEDDULL)

/* Common Arena Allocation Parameters */
#define CVM_COMMON_ARENA_SIZE    (8 * 1024 * 1024)
#define CVM_COMMON_ARENA_COUNT   (4)


#define CVM_COMMON_TICK_LEN_US (2000)      /* tick length in microsec */
#define CVM_COMMON_WHEEL_LEN_TICKS (2500)  /* wheel length in ticks */

#define CVM_COMMON_IDLE_PROCESSING_INTERVAL  5  /* secs */

#if defined(__KERNEL__) && defined(linux)
#define cvm_common_panic(...) \
    { \
       printk("PANIC:" __VA_ARGS__); \
       panic("%s:%d\n", __FILE__, __LINE__); \
    }
#else
extern void exit(int);

#define cvm_common_panic(...) \
    { \
       printf("PANIC:" __VA_ARGS__); \
       exit(-1); \
    }
#endif

#define CVM_COMMON_KASSERT(exp,msg) \
    { \
    if(!(exp)) cvm_common_panic msg; \
    }
	

#define cvm_common_ntohs(a) (a)
#define cvm_common_htons(a) (a)
#define cvm_common_ntohl(a) (a)
#define cvm_common_htonl(a) (a)

/* Macros for counting and rounding. */
#define cvm_common_howmany(x, y)   (((x)+((y)-1))/(y))
#define cvm_common_rounddown(x, y) (((x)/(y))*(y))
#define cvm_common_roundup(x, y)   ((((x)+((y)-1))/(y))*(y))  /* to any y */
#define cvm_common_roundup2(x, y)  (((x)+((y)-1))&(~((y)-1))) /* if y is powers of two */
#define cvm_common_powerof2(x)     ((((x)-1)&(x))==0)

#define CVM_COMMON_MALLOC_BACK_VAL 0xF

static inline int cvm_common_imax(int a, int b) { return (a > b ? a : b); }
static inline int cvm_common_imin(int a, int b) { return (a < b ? a : b); }

static inline int cvm_common_splnet(void) { return 1; }
static inline int cvm_common_splimp(void) { return 1; }
static inline void cvm_common_splx(int x) { return; }


/* Groups for work queue entries */
typedef enum {
    CVM_COMMON_PACKET_GRP,
    CVM_COMMON_HOST_MSG_GRP
} cvm_common_work_grp;

/* debug macros & associated enums */

/* local typedef for long long */
/* used with SIMPRINTF macro   */
typedef long long    ll64_t;

#ifdef REAL_HW
#define CVM_COMMON_PRINT_FN printf
#else
#define CVM_COMMON_PRINT_FN simprintf
#endif

/* macro to use either printf or simprintf   */
/* depending on whether the code is running  */
/* on simulator or real hardware             */
/*                                           */
/* this macro has following limitations:     */
/*                                           */
/* 1. cannot take more than 6 parameters     */
/* 2. each parameter should be used with     */
/*    %lld or %llX and should be typecast    */
/*    with (long long). For example          */
/*                                           */
/*  int a = 0x45667544A;                     */
/*  SIMPRINTF("a = 0x%llX\n", (long long)a); */


/** Common Printf Wrapper macro
 *  @param format    - standard printf format
 *  @param args      - printf arguments
 *
 * While running on hardware, this macro is
 * translated to normal printf. For the simulaotr
 * the macro uses simprintf which is a faster
 * variant of printf, but only applicable to
 * the Octeon simulaotr.
 *
 */
#define CVM_COMMON_SIMPRINTF(format, args...) \
{ \
    CVM_COMMON_PRINT_FN(format, ##args); \
}

CVMX_SHARED extern int cvm_common_debug_level;

/* debug levels 1-9 */
/* 1 : lowest level with minimum debugging information  */
/* 9 : highest level with maximim debugging information */
typedef enum { 
        CVM_COMMON_DBG_LVL_MUST_PRINT = 1,
        CVM_COMMON_DBG_LVL_CRITICAL = 2,
        CVM_COMMON_DBG_LVL_ERROR = 3,
        CVM_COMMON_DBG_LVL_WARNING = 4,
        CVM_COMMON_DBG_LVL_5 = 5,
        CVM_COMMON_DBG_LVL_6 = 6,
        CVM_COMMON_DBG_LVL_7 = 7,
        CVM_COMMON_DBG_LVL_8 = 8,
        CVM_COMMON_DBG_LVL_INFO = 9,
        CVM_COMMON_MAX_DBG_LVL 
} cvm_common_debug_level_t;


#ifdef OCTEON_DEBUG_LEVEL

/** Common Debug Message Print macro
 *
 *  @param lvl      - debug level for the message
 *  @param format   - message print format
 *  @param args     - print arguments
 *
 * This macro is used for debug message printng. The
 * lvl parameter specifies the debug level for this 
 * message. The OCTEON_DEBUG_LEVEL specifies system
 * wide debug level. The message is only printed if
 * the lvl paramter is less than or equal to the
 * OCTEON_DEBUG_LEVEL.
 *
 * Note that the debug message printing is enabled
 * only when the OCTEON_DEBUG_LEVEL is defined and
 * assigned a number from 0-9. If OCTEON_DEBUG_LEVEL
 * is not defined, all the calls to this macro are
 * ignored. 
 *
 */
#define CVM_COMMON_DBG_MSG(lvl, format, args...) \
{ \
    if ((lvl <= cvm_common_debug_level) && (lvl < CVM_COMMON_MAX_DBG_LVL)) \
    { \
        printf("[DBG %d] ", lvl); \
        printf(format, ##args); \
    } \
}


/** Common Debug Memory Dump macro
 *
 *  @param lvl      - debug level
 *  @param buf      - char pointer to the buffer
 *  @param len      - number of bytes to dump
 *
 * This macro is used to dump memory. The
 * lvl parameter specifies the debug level for this 
 * dump. The OCTEON_DEBUG_LEVEL specifies system
 * wide debug level. The message is only printed if
 * the lvl paramter is less than or equal to the
 * OCTEON_DEBUG_LEVEL.
 *
 * Note that the debug memory dump macro is enabled
 * only when the OCTEON_DEBUG_LEVEL is defined and
 * assigned a number from 0-9. If OCTEON_DEBUG_LEVEL
 * is not defined, all the calls to this macro are
 * ignored. 
 *
 */
#define CVM_COMMON_DBG_DUMP(lvl, buf, len) \
{ \
    if ((lvl <= cvm_common_debug_level) && (lvl < CVM_COMMON_MAX_DBG_LVL)) \
    { \
        int a=0; \
        unsigned char tmp=0; \
        printf("[DBG %d]\n", lvl); \
        printf("0x%04X : ", a); \
        for (a=0;a<(len);a++) \
        { \
	    if(a && ((a%8) == 0)) \
            { \
		printf("%s", "\n"); \
                printf("0x%04X : ", a); \
            } \
	    tmp = (buf)[a];\
	    tmp &= 0xff;\
	    printf("%02x ",tmp);\
        } \
    printf("\n\n"); \
    } \
}
#else
#define CVM_COMMON_DBG_MSG(lvl,format, args...);
#define CVM_COMMON_DBG_DUMP(lvl, buf, len);
#endif


#if defined(__KERNEL__) && defined(linux)
#define CVM_COMMON_MALLOC_TSAFE(arena_list,size) \
({ \
    void *ptr= NULL; \
    ptr = kmalloc(size, GFP_DMA); \
    ptr; \
})
  
  
#define CVM_COMMON_FREE_TSAFE(ptr) \
{ \
    kfree(ptr); \
}

#elif defined(linux)

#define CVM_COMMON_MALLOC_TSAFE(arena_list,size) \
({ \
    void *ptr= NULL; \
    ptr = malloc(size); \
    ptr; \
})
  
#define CVM_COMMON_FREE_TSAFE(ptr) \
{ \
    free(ptr); \
}

#else
/*
 * threadsafe functionality of cvmx_malloc
 */
extern CVMX_SHARED cvmx_spinlock_t cvm_common_malloc_lock;

#define CVM_COMMON_MALLOC_TSAFE(arena_list, size) \
({ \
    void *ptr = NULL; \
    cvmx_spinlock_lock(&cvm_common_malloc_lock); \
    ptr = cvmx_malloc(arena_list, size); \
    cvmx_spinlock_unlock(&cvm_common_malloc_lock); \
    ptr; \
})

#define CVM_COMMON_FREE_TSAFE(ptr) \
{ \
    cvmx_spinlock_lock(&cvm_common_malloc_lock); \
    cvmx_free(ptr); \
    cvmx_spinlock_unlock(&cvm_common_malloc_lock); \
}

#define CVM_COMMON_REALLOC_TSAFE(arena_list, oldptr, size)	\
({ \
    void *ptr = NULL; \
    cvmx_spinlock_lock(&cvm_common_malloc_lock); \
    ptr = cvmx_realloc(arena_list, oldptr, size);	\
    cvmx_spinlock_unlock(&cvm_common_malloc_lock); \
    ptr; \
})
#endif


#ifdef OUT_SWP_IN_SCRATCH // {
#define out_swp (*((cvm_common_wqe_t **)(CVMX_SCRATCH_BASE + CVMX_SCR_OUT_SWP)))
#define out_swp_tail (*((cvm_common_wqe_t **)(CVMX_SCRATCH_BASE + CVMX_SCR_OUT_SWP_TAIL)))
#else
#ifdef __KERNEL__
DECLARE_PER_CPU(cvm_common_wqe_t *, __out_swp);
DECLARE_PER_CPU(cvm_common_wqe_t * , __out_swp_tail);
#define out_swp __get_cpu_var(__out_swp)
#define out_swp_tail __get_cpu_var(__out_swp_tail)
#else
extern cvm_common_wqe_t *out_swp;
extern cvm_common_wqe_t *out_swp_tail;
#endif
#endif // OUT_SWP_IN_SCRATCH }


/* common function declarations */
int cvm_common_allocate_arenas(uint64_t arena_size, uint64_t arena_count);
int cvm_common_global_init(void);
int cvm_common_local_init(void);
inline const char *cvm_common_get_version(void);
int cvm_common_do_idle_processing(void);



/* packet dump related misc routines */
void cvm_common_dump_special(cvmx_wqe_t *work);
void cvm_common_dump_packet(cvmx_wqe_t *work, int dir);
void cvm_common_dump_packet_gth(cvm_common_wqe_t *work);

#if !defined(__KERNEL__) && !defined(linux)
/* common externs */
extern CVMX_SHARED cvmx_arena_list_t  cvm_common_arenas;
#endif

#if defined(__KERNEL__) && defined(linux)
#define bcopy(s,d,n) memcpy(d,s,n)
#define bzero(s,n) memset(s,0,n)
#define bcmp(s1,s2,n) memcmp(s1,s2,n)
static inline uint64_t ulmin(uint64_t a, uint64_t b)
{
 return (((a)<(b))?(a):(b));
}
#endif

#define cvm_common_bootmem_alloc(size, flags) cvmx_bootmem_alloc(size,flags)

/*
 * Various network packet headers
 */

/*
 * Structure of IP header (IPV4)
 */
typedef struct cvm_common_ip_hdr 
{

    uint32_t ip_v:4;		/* version */
    uint32_t ip_hl:4;		/* header length */
    uint32_t ip_tos:8;		/* type of service */
    uint32_t ip_len:16;		/* total length */
    uint32_t ip_id:16;		/* identification */
    uint32_t ip_off:16;		/* fragment offset field */
    uint32_t ip_ttl:8;		/* time to live */
    uint32_t ip_p:8;		/* protocol */
    uint32_t ip_sum:16;		/* checksum */
    uint32_t ip_src;	        /* source address */
    uint32_t ip_dst;	        /* dest address */

} cvm_common_ip_hdr_t;


/*
 * Structure of TCP header.
 */
typedef struct cvm_common_tcp_hdr 
{
    uint16_t  th_sport;		/* source port */
    uint16_t  th_dport;		/* destination port */
    uint32_t  th_seq;		/* sequence number */
    uint32_t  th_ack;		/* acknowledgement number */
    uint32_t  th_off:4,		/* data offset */
              th_x2:4;		/* (unused) */
    uint8_t   th_flags;
    uint16_t  th_win;	        /* window */
    uint16_t  th_sum;	        /* checksum */
    uint16_t  th_urp;		/* urgent pointer */

} cvm_common_tcp_hdr_t;


/* various TCP flags */
#define	CVM_COMMON_TCP_TH_FIN	 0x01
#define	CVM_COMMON_TCP_TH_SYN	 0x02
#define	CVM_COMMON_TCP_TH_RST	 0x04
#define	CVM_COMMON_TCP_TH_PUSH	 0x08
#define	CVM_COMMON_TCP_TH_ACK	 0x10
#define	CVM_COMMON_TCP_TH_URG	 0x20
#define	CVM_COMMON_TCP_TH_ECE	 0x40
#define	CVM_COMMON_TCP_TH_CWR	 0x80

/* various protocols */
#define CVM_COMMON_IPPROTO_IP	    0
#define CVM_COMMON_IPPROTO_ICMP     1
#define CVM_COMMON_IPPROTO_TCP	    6
#define CVM_COMMON_IPPROTO_UDP	   17
#define CVM_COMMON_IPPROTO_RAW	  255


/** zero copy 64 bit buffer pointer (BP) related macros 
 *                                                     
 * A note about these macros:                         
 *                                                   
 * The input 'p' to all SET_BP* macros could either be
 * a uint64_t value or a pointer. e.g.               
 *                                                  
 *    uint64_t addr = 0;                           
 *    void *ptr = 0;                              
 *                                               
 *    SET_BP_BACK(addr, 0xd);                   
 *    SET_BP_BACK(ptr,  0x3);                  
 *                                            
 * but you shouldn't do a typecast a pointer to a
 * uint64_t within the parameters of the macro i.e.
 *                                                
 *   SET_BP_BACK( (uint64_t)ptr, 0x3);           
 *                                              
 * this will confuse the compiler as it will not be    
 * able to get the address of the ptr                 
 *                                                   
 * This restriction does not apply to GET_BP* macros
 */

#define CVM_COMMON_GET_BP_I_BIT(p) ( ((cvmx_buf_ptr_t)((uint64_t)(p))).s.i )
#define CVM_COMMON_GET_BP_BACK(p)  ( ((cvmx_buf_ptr_t)((uint64_t)(p))).s.back )
#define CVM_COMMON_GET_BP_POOL(p)  ( ((cvmx_buf_ptr_t)((uint64_t)(p))).s.pool )
#define CVM_COMMON_GET_BP_SIZE(p)  ( ((cvmx_buf_ptr_t)((uint64_t)(p))).s.size )
#define CVM_COMMON_GET_BP_ADDR(p)  ( ((cvmx_buf_ptr_t)((uint64_t)(p))).s.addr )

#define CVM_COMMON_SET_BP_I_BIT(p,v) ( (((cvmx_buf_ptr_t*)(&(p))))->s.i=v )
#define CVM_COMMON_SET_BP_BACK(p,v)  ( (((cvmx_buf_ptr_t*)(&(p))))->s.back=v )
#define CVM_COMMON_SET_BP_POOL(p,v)  ( (((cvmx_buf_ptr_t*)(&(p))))->s.pool=v )
#define CVM_COMMON_SET_BP_SIZE(p,v)  ( (((cvmx_buf_ptr_t*)(&(p))))->s.size=v  )
#define CVM_COMMON_SET_BP_ADDR(p,v)  ( (((cvmx_buf_ptr_t*)(&(p))))->s.addr=v )


#define CVM_COMMON_SET_BP(bp, addr, size, pool, back, i) \
{ \
    CVM_COMMON_SET_BP_ADDR(bp, addr); \
    CVM_COMMON_SET_BP_SIZE(bp, size); \
    CVM_COMMON_SET_BP_POOL(bp, pool); \
    CVM_COMMON_SET_BP_BACK(bp, back); \
    CVM_COMMON_SET_BP_I_BIT(bp, i);  \
}

#define CVM_COMMON_MARK_BP_AS_SW_BUFFER(bp) \
{ \
    CVM_COMMON_SET_BP_POOL((bp), CVMX_FPA_WQE_POOL); \
    CVM_COMMON_SET_BP_BACK((bp), CVM_COMMON_MALLOC_BACK_VAL); \
}


#define CVM_COMMON_IS_BP_SW_BUFFER(bp) ( ( ((CVM_COMMON_GET_BP_POOL(bp)) == CVMX_FPA_WQE_POOL) && ( (CVM_COMMON_GET_BP_BACK(bp)) == CVM_COMMON_MALLOC_BACK_VAL)) ? 1 : 0 )



/*
 * Various ptr types
 */

#define CVM_COMMON_DIRECT_DATA  0     /**< Local Pointer points directly to data */
#define CVM_COMMON_GATHER_DATA  1     /**< Local Pointer points to a gather list of local pointers */
#define CVM_COMMON_LINKED_DATA  2     /**< Local pointer points to data which has links to more buffers */
#define CVM_COMMON_NULL_DATA    3     /**< If no data is sent and a flag is required, use this. */

typedef struct cvm_common_pci_sanity_hdr
{
    cvmx_buf_ptr_t      lptr;
    uint32_t            pko_ptr_type;
    uint32_t            segs;
    uint32_t            total_bytes;
    uint32_t            port;

} cvm_common_pci_sanity_hdr_t;


/*
 * CAST macro
 */
#define CVM_COMMON_UCAST64(v) ((long long)(unsigned long)(v))


/*
 * macro to calculate the back field (should only be used when
 * FPA buffer size is power of 2)
 */
#define CVM_COMMON_CALC_BACK_POWER_2(p) ( (((p.s.addr - (p.s.addr & ~((cvmx_fpa_get_block_size(p.s.pool))-1))) >> 7) & 0xf) )


/*
 * macro to calculate the back field correctly using
 * the current address of an FPA buffer
 */
#define CVM_COMMON_CALC_BACK(p, offset)                                                                               \
({                                                                                                                     \
    int back = 0;                                                                                                      \
    uint64_t buf_start = ( ((p.s.addr - offset) & ~(CVMX_CACHE_LINE_SIZE-1)) - (p.s.back*CVMX_CACHE_LINE_SIZE) );      \
    if (p.s.addr <= buf_start)  back = 0;                                                                              \
    else                        back = (p.s.addr - buf_start) >> 7;                                                    \
    back;                                                                                                              \
})      


/*
 * macro to calculate how much space is left in the buffer
 * Assumptions:
 *
 * p.s.pool = pool of the buffer
 * p.s.size = size of the existing data
 * p.s.addr = pointer to the start of the current data
 * p.s.back = back pointer value
 *
 */
#define CVM_COMMON_SPACE_LEFT(p)  \
({ \
  unsigned int space = 0; \
  uint64_t buf_start = ( (p.s.addr & ~(CVMX_CACHE_LINE_SIZE-1)) - (p.s.back*CVMX_CACHE_LINE_SIZE) ); \
  space = cvmx_fpa_get_block_size(p.s.pool) - ( (p.s.addr+p.s.size) - buf_start); \
  space; \
})


/*
 * Pointer <-> Pointer_Offset conversion macros
 */
#define CVM_OFFSET_TO_PTR(ptr_offset, ptr_type, bit_shift) \
        (CASTPTR(ptr_type, cvmx_phys_to_ptr(CAST64(ptr_offset) << bit_shift)))

#define CVM_PTR_TO_OFFSET(ptr, bit_shift) \
        ((uint32_t)(CAST64(cvmx_ptr_to_phys(ptr)) >> bit_shift))


/* stirct aliasing macros */
typedef uint8_t  uint8_all_alias   __attribute__ ((may_alias));
typedef uint16_t uint16_all_alias  __attribute__ ((may_alias));
typedef uint32_t uint32_all_alias  __attribute__ ((may_alias));
typedef uint64_t uint64_all_alias  __attribute__ ((may_alias));

#endif /* __CVM_COMMON_DEFS_H__ */
