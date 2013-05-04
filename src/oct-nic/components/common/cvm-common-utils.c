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

#if defined(__KERNEL__) && defined(linux)
#include <linux/module.h>
#include <linux/kernel.h>
#include "cvmx.h"
#include "cvmx-wqe.h"

#define printf printk
#elif defined(linux)
#include <malloc.h>
#include "cvmx.h"
#include "cvmx-wqe.h"
#else
#include <stdio.h>
#include <string.h>
#include "cvmx.h"
#include "cvmx-wqe.h"
#endif


#include "cvmx-config.h"
#include "executive-config.h"

#if !defined(__KERNEL__)
#include "global-config.h"

#include "cvmx-spinlock.h"
#include "cvmx-malloc.h"
#include "cvmx-pow.h"
#include "cvmx-fau.h"
#include "cvmx-bootmem.h"
#include "cvmx-coremask.h"
#include "cvmx-scratch.h"
#include "cvmx-helper.h"
#include "cvmx-helper-util.h"
#endif

#include "cvmx-fpa.h"
#include "cvmx-fau.h"
#include "cvm-common-wqe.h"
#include "cvm-common-defs.h"
#include "cvm-common-misc.h"
#include "cvm-common-fpa.h"
  
extern CVMX_SHARED cvmx_spinlock_t cvm_common_malloc_lock;
/*
 * returns the version number based on CVS tag
 */
inline const char *cvm_common_get_version(void)
{
    static char version[80];
    const char *cavium_parse_array = "$Name: COMMON_2_2_0_build_61 ";
    const char cavium_parse[80];

    strcpy( (void*)cavium_parse, (void*)cavium_parse_array );

    if (cavium_parse[7] == ' ')
    {
        snprintf(version, sizeof(version), "Internal %s", __DATE__);
    }
    else
    {
        char *major = NULL;
        char *minor1 = NULL;
        char *minor2 = NULL;
        char *build = NULL;
        char *buildnum = NULL;
        char *end = NULL;
        char buf[80];

        strncpy(buf, cavium_parse, sizeof(buf));
        buf[sizeof(buf)-1] = 0;

        major = strchr(buf, '_');
        if (major)
        {
            major++;
            minor1 = strchr(major, '_');
            if (minor1)
            {
                *minor1 = 0;
                minor1++;
                minor2 = strchr(minor1, '_');
                if (minor2)
                {
                    *minor2 = 0;
                    minor2++;
                    build = strchr(minor2, '_');
                    if (build)
                    {
                        *build = 0;
                        build++;
                        buildnum = strchr(build, '_');
                        if (buildnum)
                        {
                            *buildnum = 0;
                            buildnum++;
                            end = strchr(buildnum, ' ');
                            if (end)
                                *end = 0;
                        }
                    }
                }
            }
        }

        if (major && minor1 && minor2 && build && buildnum && (strcmp(build, "build") == 0))
            snprintf(version, sizeof(version), "%s.%s.%s, build %s", major, minor1, minor2, buildnum);
        else
            snprintf(version, sizeof(version), "%s", cavium_parse);
    }

    return version;
}




int cvm_common_global_init(void)
{
    int i = 0;

#if !defined(__KERNEL__)
    int common_malloc_arena_size = CVM_COMMON_ARENA_SIZE;
    int num_common_arenas = CVM_COMMON_ARENA_COUNT;

    cvmx_spinlock_init(&cvm_common_malloc_lock);
#endif

#ifdef OCTEON_DEBUG_LEVEL
    if ((OCTEON_DEBUG_LEVEL >= CVM_COMMON_DBG_LVL_MUST_PRINT) &&
	(OCTEON_DEBUG_LEVEL < CVM_COMMON_MAX_DBG_LVL))
    {
        cvm_common_debug_level = OCTEON_DEBUG_LEVEL;
    }
#endif

#ifdef CVM_CLI_APP
    cvm_common_history = (cvm_common_history_t*)cvmx_bootmem_alloc(sizeof(cvm_common_history_t), CVMX_CACHE_LINE_SIZE);
    if (cvm_common_history != NULL) memset( (void*)cvm_common_history, 0x0, sizeof(cvm_common_history_t) ); 
#endif

#if !defined(__KERNEL__)
    /* allocate arenas for components */
    if ( (cvm_common_allocate_arenas(common_malloc_arena_size, num_common_arenas)) < 0 )
    {
        return (-1);
    }
#endif

    /* init FPA out-of-buffer counters */
    for (i=0; i<CVMX_FPA_NUM_POOLS; i++)
    {
        cvmx_fau_atomic_write32( (i*4)+CVM_FAU_REG_FPA_OOB_COUNT, 0x0);
    }

    return (0);
}

int cvm_common_local_init(void)
{
#if !defined(__KERNEL__)
    cvm_common_mbuf_size_t mbuf_info;

    mbuf_info.s.mbuff_size = (uint16_t)cvmx_read_csr(CVMX_IPD_PACKET_MBUFF_SIZE);
    mbuf_info.s.mbuff_skip_first = (uint16_t)cvmx_read_csr(CVMX_IPD_1ST_MBUFF_SKIP);
    mbuf_info.s.mbuff_skip_not_first = (uint16_t)cvmx_read_csr(CVMX_IPD_NOT_1ST_MBUFF_SKIP);
    mbuf_info.s.initialized = 1;

    cvmx_scratch_write64(CVM_SCR_MBUFF_INFO_PTR, mbuf_info.u64);
#endif

    return (0);
}

/*
 * Common Memory allocation routines
 */
int cvm_common_allocate_arenas(uint64_t arena_size, uint64_t arena_count)
{
#if defined(__KERNEL__) 
   return 0;
#elif defined(linux)
   return 0;
#else

    uint32_t i;
    uint8_t *memptr=NULL;

    for(i=0;i<arena_count;i++)
    {
        memptr = (uint8_t*)cvmx_bootmem_alloc(arena_size, arena_size);

        if (!memptr || cvmx_add_arena(&cvm_common_arenas, memptr, arena_size) < 0)
        {
	    printf("%s : error adding arena: %u!\n", __FUNCTION__, (unsigned int)i);
            return (-1);
        }
    }
#endif

    return (0);
}


#if !defined(__KERNEL__)
/*
 * Return: NULL on failure; otherwire a valid memory address
 */
void* cvm_common_setup_fpa_pool(int pool, int entry_size, int buf_count, char* pool_name)
{
    void* memory = NULL;
    void* base = NULL;

    memory = cvmx_bootmem_alloc(entry_size * buf_count, entry_size);
    if (memory == NULL)
    {
        printf("%s: Out of memory initializing fpa %s pool...\n", __FUNCTION__, pool_name);
        return (NULL);
    }

#ifdef CVMX_ABI_N32
    /* for N32 mode, bit 31 should not be set for a valid address */
    if ( ((uint32_t)memory) >> 31)
    {
        printf("************************ CRITICAL ERROR *************************\n");
        printf("%s: Invalid address reported for N32 mode (invalid address is %p)\n", __FUNCTION__, memory);
	printf("%s: Address bit 31 should not be set. Unable to continue...\n", __FUNCTION__);
        printf("************************ CRITICAL ERROR *************************\n");
	while(1);
    }
#endif

    base = CVM_COMMON_INIT_FPA_CHECKS(memory, buf_count, entry_size);
    cvmx_fpa_setup_pool(pool, pool_name, memory, entry_size, buf_count);
    cvm_common_fpa_add_pool_info(pool, memory, entry_size, buf_count, CAST64(base)); 

    return (memory);
}
#endif


/*
 * we are only handling fpa out-of-buffer print throttling right now
 *
 * Assumption: should be called by the first stack core only
 */
int cvm_common_do_idle_processing(void)
{
    static uint32_t last_fpa_oob_count[CVMX_FPA_NUM_POOLS];
    uint32_t new_fpa_oob_count[CVMX_FPA_NUM_POOLS];
    static int init_done = 0;
    int i = 0;
    int found = 0;

    if (!init_done)
    {
        for (i=0; i<CVMX_FPA_NUM_POOLS; i++)
	{
            last_fpa_oob_count[i] = cvmx_fau_fetch_and_add32 ( (i*4)+CVM_FAU_REG_FPA_OOB_COUNT, 0);
	}

	init_done = 1;

	return (0);
    }

    /* compare the values */
    for (i=0; i<CVMX_FPA_NUM_POOLS; i++)
    {
        uint32_t oob_diff = 0;

        new_fpa_oob_count[i] = cvmx_fau_fetch_and_add32 ( (i*4)+CVM_FAU_REG_FPA_OOB_COUNT, 0);

        oob_diff = new_fpa_oob_count[i] - last_fpa_oob_count[i];
	
        if (new_fpa_oob_count[i] < last_fpa_oob_count[i]) oob_diff = (0xFFFFFFFF - last_fpa_oob_count[i]) + new_fpa_oob_count[i];

	if (oob_diff > 0)
	{
	    printf("FPA out of buffer for pool %d [ Message repeated %lld time(s) ]\n", i, CVM_COMMON_UCAST64(oob_diff));
	    found = 1;
	}

	last_fpa_oob_count[i] = new_fpa_oob_count[i];
    }

    if (found) printf("\n");

    return (0);
}


#if !defined(__KERNEL__)


void
cvm_common_dump_data(void *d, unsigned int size)
{
    int a=0;
    char* buf = (char*)d;
    unsigned char tmp=0;

    printf("0x%04X : ", a);
    for (a=0;a<((int)size);a++)
    {
        if(a && ((a%8) == 0))
        {
            printf("%s", "\n");
            printf("0x%04X : ", a);
        }
	tmp = (buf)[a];
        tmp &= 0xff;
        printf("%02x ",tmp);
    }
    printf("\n\n");
}


/**
 * Debug routine to dump the packet structure to the console
 * 
 * @param work   Work queue entry containing the packet to dump
 */
void cvm_common_dump_special(cvmx_wqe_t *work)
{
    CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_8, "Packet Type:   %u\n", cvmx_wqe_get_unused8(work));
    CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_8, "Packet Length:   %u\n", cvmx_wqe_get_len(work));
    CVM_COMMON_DBG_DUMP(CVM_COMMON_DBG_LVL_8, (uint8_t *)work->packet_data, cvmx_wqe_get_len(work));
}

/**
 * Debug routine to dump the packet structure to the console
 * 
 * @param work   Work queue entry containing the packet to dump
 */
void cvm_common_dump_packet(cvmx_wqe_t *work, int dir)
{
    cvmx_buf_ptr_t  buffer_ptr;
    uint32_t sadr, dadr;
    uint16_t sprt, dprt, tflg;
    buffer_ptr = work->packet_ptr;
    int iplen;
    int             len;
    uint64_t        remaining_bytes;
    uint64_t        start_of_buffer;
    uint64_t virt_ptr = CAST64(cvmx_phys_to_ptr(buffer_ptr.s.addr));

    if (*( CASTPTR(uint16_t,(virt_ptr+12)) ) == 0x800) 
      {
	sadr = ( CASTPTR(cvm_common_ip_hdr_t,(virt_ptr+14)))->ip_src;
	dadr = ( CASTPTR(cvm_common_ip_hdr_t,(virt_ptr+14)))->ip_dst;
	iplen = (( CASTPTR(cvm_common_ip_hdr_t,(virt_ptr+14)) )->ip_hl)<<2;
	sprt = ( CASTPTR(cvm_common_tcp_hdr_t,(virt_ptr+14+iplen)))->th_sport;
	dprt = ( CASTPTR(cvm_common_tcp_hdr_t,(virt_ptr+14+iplen)))->th_dport;
	tflg = ( CASTPTR(cvm_common_tcp_hdr_t,(virt_ptr+14+iplen)))->th_flags;
	
	if (dir == 0) /* rx */
	  {
	    CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_6, "RX len=%u sadr=%llx dadr=%llx sprt=%x dprt=%x tflg=%x\n",
			       cvmx_wqe_get_len(work), CVM_COMMON_UCAST64(sadr), CVM_COMMON_UCAST64(dadr), sprt, dprt, tflg);
	  }
	else /* dir == 1 , tx */
	  {
	    CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_6, "TX len=%u sadr=%llx dadr=%llx sprt=%x dprt=%x tflg=%x\n",
			       cvmx_wqe_get_len(work), CVM_COMMON_UCAST64(sadr), CVM_COMMON_UCAST64(dadr), sprt, dprt, tflg);
	  }
      }
    else if (cvmx_wqe_get_port(work) != 32) /* not IP */
      {
	if (dir == 0) /* rx */
	  {
	    CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_6, "RX len=%u NOT IP\n", cvmx_wqe_get_len(work));
	  }
	else
	  {
	    CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_6, "TX len=%u NOT IP\n", cvmx_wqe_get_len(work));
	  }
      }

    CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_8, "Packet Length:   %u\n", cvmx_wqe_get_len(work));
    CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_8, "    Input Port:  %u\n", cvmx_wqe_get_port(work));
    CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_8, "    QoS:         %u\n", cvmx_wqe_get_qos(work));
    buffer_ptr = work->packet_ptr;
    remaining_bytes = cvmx_wqe_get_len(work);

    while (remaining_bytes)
    {
        virt_ptr = CAST64(cvmx_phys_to_ptr(buffer_ptr.s.addr));

        start_of_buffer = ((virt_ptr >> 7) - buffer_ptr.s.back) << 7;
        CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_8, "    Buffer Start:%llx\n", CAST64(start_of_buffer));
        CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_8, "    Buffer I   : %u\n", buffer_ptr.s.i);
        CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_8, "    Buffer Back: %u\n", buffer_ptr.s.back);
        CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_8, "    Buffer Pool: %u\n", buffer_ptr.s.pool);
        CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_8, "    Buffer Data: %llx\n", CAST64(virt_ptr));
        CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_8, "    Buffer Size: %u\n", buffer_ptr.s.size);

        len = (remaining_bytes < buffer_ptr.s.size) ? remaining_bytes : buffer_ptr.s.size;
        CVM_COMMON_DBG_DUMP(CVM_COMMON_DBG_LVL_8, CASTPTR(uint8_t,virt_ptr), len)
	remaining_bytes -= len;

        buffer_ptr = *CASTPTR(cvmx_buf_ptr_t,(virt_ptr - 8));
    }
}

/**
 * Debug routine to dump the packet structure to the console
 * 
 * @param work   Work queue entry containing the packet to dump
 */
void cvm_common_dump_packet_gth(cvm_common_wqe_t *work)
{
    cvmx_buf_ptr_t  buffer_ptr;
    uint64_t        remaining_bytes;
    uint64_t        start_of_buffer;
    int i = 0;

    uint32_t sadr, dadr;
    uint16_t sprt, dprt, tflg;
    /*buffer_ptr = *((cvmx_buf_ptr_t *)((work->hw_wqe.packet_ptr).s.addr));*/
    buffer_ptr = work->hw_wqe.packet_ptr;
    int iplen;
    int len;
    uint64_t virt_ptr = CAST64(cvmx_phys_to_ptr(buffer_ptr.s.addr));

    if (*( CASTPTR(uint16_t,(virt_ptr+12))) == 0x800) 
      {
	sadr = ( CASTPTR(cvm_common_ip_hdr_t,(virt_ptr+14)))->ip_src;
	dadr = ( CASTPTR(cvm_common_ip_hdr_t,(virt_ptr+14)))->ip_dst;
	iplen = (( CASTPTR(cvm_common_ip_hdr_t,(virt_ptr+14)))->ip_hl)<<2;
	sprt = ( CASTPTR(cvm_common_tcp_hdr_t,(virt_ptr+14+iplen)))->th_sport;
	dprt = ( CASTPTR(cvm_common_tcp_hdr_t,(virt_ptr+14+iplen)))->th_dport;
	tflg = ( CASTPTR(cvm_common_tcp_hdr_t,(virt_ptr+14+iplen)))->th_flags;
	
	CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_6, "TX len=%u sadr=%llx dadr=%llx sprt=%x dprt=%x tflg=%x\n",
			   work->hwqe_w1_len, CVM_COMMON_UCAST64(sadr), CVM_COMMON_UCAST64(dadr), sprt, dprt, tflg);
      }
    else /* not IP */
      {
	CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_6, "TX len=%u NOT IP\n", work->hwqe_w1_len);
      }

    CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_8, "Packet Length:   %u\n", work->hwqe_w1_len);
    CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_8, "    Input Port:  %u\n",
        cvm_common_wqe_get_port(work));
    CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_8, "    QoS:         %u\n",
        cvm_common_wqe_get_qos(work));
    buffer_ptr = ( CASTPTR(cvmx_buf_ptr_t,(virt_ptr)))[i++];
    remaining_bytes = work->hwqe_w1_len;

    while (1)
    {
        uint64_t buf_ptr = CAST64(cvmx_phys_to_ptr(buffer_ptr.s.addr));

        start_of_buffer = ((buf_ptr >> 7) - buffer_ptr.s.back) << 7;
        CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_8, "    Buffer Start:%llx\n", CAST64(start_of_buffer));
        CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_8, "    Buffer I   : %u\n", buffer_ptr.s.i);
        CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_8, "    Buffer Back: %u\n", buffer_ptr.s.back);
        CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_8, "    Buffer Pool: %u\n", buffer_ptr.s.pool);
        CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_8, "    Buffer Data: %llx\n", CAST64(virt_ptr));
        CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_8, "    Buffer Size: %u\n", buffer_ptr.s.size);

        len = (remaining_bytes < buffer_ptr.s.size) ? remaining_bytes : buffer_ptr.s.size;
        CVM_COMMON_DBG_DUMP(CVM_COMMON_DBG_LVL_8, CASTPTR(uint8_t,buf_ptr), len)
	remaining_bytes -= len;
        if (remaining_bytes == 0)
	    break;
        buffer_ptr = ( CASTPTR(cvmx_buf_ptr_t,(virt_ptr)))[i++];
    }
}


#ifdef CVM_CLI_APP

/*
 * Helper functions for CLI
 */

void cvm_common_help(void)
{
    printf("\n");
    printf("\tcvm_common_display_fpa_alloc_info\t\tDisplays fpa buffer allocation information\n");
    printf("\tcvm_common_display_fpa_buffer_count\t\tDisplays current fpa buffer count for various pools\n");
    printf("\tcvm_common_analyze_fpa <pool>\t\t\tAnalyzes the fpa pool for duplicate or invalid entries\n");
    printf("\tcvm_common_core_info\t\t\t\tDisplay the status of all the cores\n");
    printf("\tcvm_common_read_csr\t\t\t\tRead a CSR (param: hex addr)\n");
    printf("\tcvm_common_write_csr\t\t\t\tWrite a CSR (param: hex addr, hex val)\n");
    printf("\tcvm_common_dump_pko\t\t\t\tDump useful PKO registers\n");
#ifdef EXTRA_STATS
    printf("\tcvm_common_display_extra_stats\t\t\tDisplay few additional stats\n");
#endif
#ifdef OCTEON_DEBUG_LEVEL
    printf("\tcvm_common_get_debug_level\t\t\tDisplay the current debug level\n");
    printf("\tcvm_common_set_debug_level <debug level>\t\t\tSets the debug level\n");
#endif
#ifdef CVM_FLOW_TRACE
    printf("\tcvm_common_backtrace\t\t\t\tDisplays backtrace for a core\n");
#endif
    printf("\n");
}

void cvm_common_display_fpa_alloc_info(void)
{
    printf("\n");
    cvm_common_fpa_display_all_pool_info();
}

void cvm_common_display_fpa_buffer_count(void)
{
    int i=0;

    printf("\n");
    for (i=0; i<8; i++)
    {
        printf("Pool %2d :%10lld\n", i, CAST64(CVM_COMMON_FPA_AVAIL_COUNT(i)) );
    }
}


#ifdef OCTEON_DEBUG_LEVEL
void cvm_common_get_debug_level(void)
{
    printf("\n");
    printf("Current debug level is %d\n", cvm_common_debug_level);
}

void cvm_common_set_debug_level(int dbg_lvl)
{
    if ((dbg_lvl >= CVM_COMMON_DBG_LVL_MUST_PRINT) &&
	(dbg_lvl < CVM_COMMON_MAX_DBG_LVL))
    {
        printf("\nDebug level changed from %d to %d\n", cvm_common_debug_level, dbg_lvl);
        cvm_common_debug_level = dbg_lvl;
    }
    else
    {
        printf("\nInvalied debug level (%d) passed\n", dbg_lvl);
    }
}
#endif

#endif /* CVM_CLI_APP */


#define CVM_COMMON_GET_NPI_ADDR_HI(pool)    (0x2807 + (pool*2))
#define CVM_COMMON_GET_NPI_ADDR_LOW(pool)   (0x2807 + (pool*2) + 1)


uint64_t cvm_common_get_top_fpa_mem(int pool)
{
    uint64_t low_addr = 0;
    uint64_t high_addr = 0;
    uint64_t final_addr = 0;

    if ( (OCTEON_IS_MODEL(OCTEON_CN56XX_PASS1))  ||
         (OCTEON_IS_MODEL(OCTEON_CN56XX_PASS2)) )
    {
        cvmx_write_csr(CVMX_PEXP_NPEI_DBG_SELECT, CVM_COMMON_GET_NPI_ADDR_HI(pool));
        high_addr = cvmx_read_csr(CVMX_PEXP_NPEI_DBG_DATA);
    }
    else
    {
        cvmx_write_csr(CVMX_NPI_DBG_SELECT, CVM_COMMON_GET_NPI_ADDR_HI(pool));
        high_addr = cvmx_read_csr(CVMX_DBG_DATA);
    }
    high_addr = (high_addr & 0xFFF) << 16;

    if ( (OCTEON_IS_MODEL(OCTEON_CN56XX_PASS1)) ||
	 (OCTEON_IS_MODEL(OCTEON_CN56XX_PASS2)) )
    {
        cvmx_write_csr(CVMX_PEXP_NPEI_DBG_SELECT, CVM_COMMON_GET_NPI_ADDR_LOW(pool));
        low_addr = cvmx_read_csr(CVMX_PEXP_NPEI_DBG_DATA);
    }
    else
    {
        cvmx_write_csr(CVMX_NPI_DBG_SELECT, CVM_COMMON_GET_NPI_ADDR_LOW(pool));
        low_addr = cvmx_read_csr(CVMX_DBG_DATA);
    }
    low_addr = (low_addr & 0xFFFF);

    final_addr = (high_addr) | (low_addr);
    final_addr = final_addr << 7;

    return (final_addr);
}


/*
 * In fpa page, two pointers are stored in a 64-bit fashion.
 * The details of each 64-bit word is:
 *
 *   [63]   :  should always be 1
 *  [62:61] :  these 2 bits from the 16, 64-bit words 
 *             written to the L2C is used to build a predictable 
 *             value. High order bits [31:30] are sent first then 
 *             the next 2 MSBits
 *
 *  [60:32] :  first pointer 
 *  [31:29] :  should be 0x7 
 *  [28:0]  :  second pointer
 *
 *
 * Format of 32 bit predictable word is:
 *
 *  [31:29] : should be 0x7 
 *  [28:26] : FPA pool number 
 *    [25]  : should be 0x0 
 *  [24:0]  : Page count number 
 *
 */
int cvm_common_analyze_single_fpa_page(uint64_t page_addr, int pool)
{
    int i = 0;
    uint32_t  verify_word = 0;
    uint64_t *ptr = CASTPTR(uint64_t, page_addr);

    for (i=0; i<16; i++)
    {
        /* see if we have a valid pointer */
        if (ptr == NULL)
	{
	    printf("%s: pool=%d, page addr=0x%llx, free word pointer is NULL\n",
		   __FUNCTION__, pool, CAST64(page_addr));
	    return (1);
	}

        /* bit [63] should be 1 */
        if (!(*ptr >> 63))
	{
	    printf("%s: pool=%d, page addr=0x%llx, free word=0x%llx  [bit 63 is not set]\n",
		   __FUNCTION__, pool, CAST64(page_addr), CAST64(*ptr));
	    return (1);
	}

	/* bit [31:29] should be 0x7 */
        if ( ((*ptr >> 29) & 0x7) != 0x7 )
	{
	    printf("%s: pool=%d, page addr=0x%llx, free word=0x%llx  [bit 31:29 is not 0x7]\n",
		   __FUNCTION__, pool, CAST64(page_addr), CAST64(*ptr));
	    return (1);
	}

	verify_word <<=2;
	verify_word |= (( (*ptr) >> 61) & 0x3);

	ptr++;
    }

    /* verify word: bit [31:29] should be 0x7 */
    if ( ((verify_word >> 29) & 0x7) != 0x7 )
    {
        printf("%s: pool=%d, page addr=0x%llx, verify word=0x%llx  [bit 31:29 is not 0x7]\n",
	       __FUNCTION__, pool, CAST64(page_addr), CAST64(verify_word));
        return (1);
    }

    /* verify word: bit 25 should be 0x0 */
    if ( ((verify_word >> 25) & 0x1) != 0x0 )
    {
        printf("%s: pool=%d, page addr=0x%llx, verify word=0x%llx  [bit 25 is not 0x0]\n",
	       __FUNCTION__, pool, CAST64(page_addr), CAST64(verify_word));
        return (1);
    }

    /* verify word: pool should match */
    if ( ((verify_word >> 26) & 0x7) != (uint32_t)pool )
    {
        printf("%s: pool=%d, page addr=0x%llx, verify word=0x%llx  [pool number %lld is invalid]\n",
	       __FUNCTION__, pool, CAST64(page_addr), CAST64(verify_word), CAST64((verify_word >> 26) & 0x7) );
        return (1);
    }

    /*
    printf("VerifyWord = 0x%x\n", verify_word);
    printf("[31:29]    = 0x%x\n", (verify_word >> 29) & 0x3);
    printf("pool       = %d\n", (verify_word >> 26) & 0x7);
    printf("[25]       = 0x%x\n", (verify_word >> 25) & 0x1);
    printf("Page count = %d\n", (verify_word >> 0) & 0x1ffffff);
    */

    return (0);
}


/*
 * parses the FPA free list
 */
int cvm_common_parsefpa_free_list(int pool)
{
    uint64_t start_address = 0;
    uint64_t total_free_buffers = 0;
    uint64_t total_free_pages = 0;
    uint64_t mem_rd_addr = 0;
    uint64_t mem_rd_data = 0;
    uint64_t mem_rd_low = 0;
    uint64_t mem_rd_high = 0;
    int      retval = 0;

    total_free_buffers = cvmx_read_csr(CVMX_FPA_QUEX_AVAILABLE(pool));
    start_address = cvm_common_get_top_fpa_mem(pool);

    mem_rd_addr = start_address;

    /* check if we have at least a single page buffer   */
    /* Note: the pointer read from the NPI register has */
    /*       28 bits while the one in the page buffer   */
    /*       has 29 bits                                */

    if ( ((mem_rd_addr >> 7) & (0xFFFFFFF)) == 0xFFFFFFF) goto done;

    /* loop through the free list */
    while ( mem_rd_addr !=  (0x1FFFFFFFULL << 7) )
    {
        retval = cvm_common_analyze_single_fpa_page(mem_rd_addr, pool);
	if (retval)
	{
	    printf("Corrupted FPA page buffer for pool %d (page buffers scanned = %llu)\n", pool, CAST64(total_free_pages));
	    if (mem_rd_addr != 0x0)
	    {
	        printf("\nCorrupted page buffer dump:\n");
                cvm_common_dump_data(CASTPTR(char,mem_rd_addr), 128);
	    }
	    return (1);
	}

        total_free_pages++;

        mem_rd_data  = *(CASTPTR(uint64_t,mem_rd_addr));

        mem_rd_low  = mem_rd_data & 0x1FFFFFFF;
        mem_rd_high = (mem_rd_data >> 32) & 0x1FFFFFFF;

        mem_rd_addr = mem_rd_low << 7;
    }


done:
    /*
    printf("Pool %d: Total pages found   = %lld\n", pool, CAST64(total_free_pages));
    printf("Pool %d: Total buffers found = %lld\n", pool, CAST64(total_free_pages*31 + total_free_pages));
    */

    return (0);
}


void cvm_common_analyze_fpa(int pool)
{
    uint16_t *buf_array = NULL;
    uint64_t *saved_buffers = NULL;
    uint32_t no_of_buffers = 0;
    int i = 0, j = 0, k = 0;
    int cur_index = 0;
    uint64_t buffer_addr;
    int offset[CVM_COMMON_MAX_FPA_INFO_ENTRIES];
    int cur_offset = 0;
    cvm_common_fpa_info_t pool_info;
    int segment;
    cvm_common_fpa_info_entry_t entry;
    uint32_t index;
    uint16_t state=0;
    cvm_common_wqe_t *swp=NULL;
    cvmx_fpa_int_sum_t int_sum;

    /* stats */
    int buffer_analyzed = 0;
    int duplicate_buffers = 0;
    int invalid_buffers = 0;
    int unaligned_buffers = 0;


    if ( (pool < 0) || (pool > CVMX_FPA_NUM_POOLS) )
    {
        printf("%s: Invalid pool number [%d]\n", __FUNCTION__, pool);
        return;
    }

    /*
     * Check FPA interrupts (may show signs of FPA corruption)
     */
    int_sum.u64 = cvmx_read_csr(CVMX_FPA_INT_SUM);

    if (int_sum.s.fed0_sbe) printf("%s: INT: Single Bit Error detected in FPF0\n", __FUNCTION__);
    if (int_sum.s.fed0_dbe) printf("%s: INT: Double Bit Error detected in FPF0\n", __FUNCTION__);
    if (int_sum.s.fed1_sbe) printf("%s: INT: Single Bit Error detected in FPF1\n", __FUNCTION__);
    if (int_sum.s.fed1_dbe) printf("%s: INT: Double Bit Error detected in FPF1\n", __FUNCTION__);

    if ( (int_sum.u64 >> ((pool*3)+4)) & 0x1 ) printf("%s: INT: Page count available goes negative\n", __FUNCTION__);
    if ( (int_sum.u64 >> ((pool*3)+4)) & 0x2 ) printf("%s: INT: Count available is greater than pointers present in the FPA\n", __FUNCTION__);
    if ( (int_sum.u64 >> ((pool*3)+4)) & 0x4 ) printf("%s: INT: Pointer read from the stack in the L2C does not have the FPA owner ship bit set\n", __FUNCTION__);

    /* 
     * parse the FPA free list and
     * check for issues
     */
    if (cvm_common_parsefpa_free_list(pool))
    {
        printf("%s: FPA PAGE list parse error; Exiting..\n", __FUNCTION__);
        return;
    }

    /* populate the offset array */
    cur_offset = 0;
    pool_info = cvm_common_fpa_info[pool];

    for (i=0; i<pool_info.count; i++)
    {
        offset[i] = cur_offset;
        cur_offset += ( (pool_info.entry[i].end_addr - pool_info.entry[i].start_addr+1)/pool_info.entry[0].element_size);
    }

    no_of_buffers = cur_offset;

    buf_array = (uint16_t*)CVM_COMMON_MALLOC_TSAFE(cvm_common_arenas, no_of_buffers*sizeof(uint16_t) );
    saved_buffers = (uint64_t*)CVM_COMMON_MALLOC_TSAFE(cvm_common_arenas, no_of_buffers*8);
    if ( (buf_array == NULL) || (saved_buffers == NULL) )
    {
        printf("%s: Unable to allocate memory to hold %lld entries\n", __FUNCTION__, CVM_COMMON_UCAST64(no_of_buffers));

	if (buf_array)       CVM_COMMON_FREE_TSAFE(buf_array);
	if (saved_buffers)   CVM_COMMON_FREE_TSAFE(saved_buffers);

	return;
    }

    memset( (void*)buf_array, 0x0, no_of_buffers*sizeof(uint16_t));

    /* setup the array to check buffer boundries */
    k = 0;
    for (i=0; i<pool_info.count; i++)
    {
        uint32_t buf_count_in_this_seg = ( (pool_info.entry[i].end_addr - pool_info.entry[i].start_addr+1)/pool_info.entry[0].element_size );
	uint64_t ptr_addr = (uint64_t)pool_info.entry[i].start_addr;

	for (j=0; j<((int)buf_count_in_this_seg); j++)
	{
	    buf_array[k] = (ptr_addr & (0x7fffull) );
	    k++;
            ptr_addr += (uint64_t)(pool_info.entry[i].element_size);

	}
    }


    /* analyze this pool */
    printf("Analyzing %lld buffers. Wait...\n", CAST64(CVM_COMMON_FPA_AVAIL_COUNT(pool)) );

    /* Get all the buffers from POW */
    while(1)
    {
        swp = (cvm_common_wqe_t *)cvmx_pow_work_request_sync(0);
        if (swp == NULL)
        {
	    break;
	}

        cvmx_helper_free_packet_data ((cvmx_wqe_t*)swp);
        cvmx_fpa_free( (void*)swp, CVMX_FPA_WQE_POOL, 0);
    }

    cur_index = 0;
    while(1)
    {
        buffer_addr = CAST64(cvmx_fpa_alloc(pool));
	if (buffer_addr == 0x0)
	{
	    if ( (CVM_COMMON_FPA_AVAIL_COUNT(pool)) != 0x0)
	    {
	        invalid_buffers++;
	        printf("%s: Invalid buffer address (buffer=0x%llX, index=%d, no. of buffers still in pool=%lld)\n", 
		       __FUNCTION__, CAST64(buffer_addr), cur_index, CAST64(CVM_COMMON_FPA_AVAIL_COUNT(pool)) );
	        continue;
	    }
	    else
	    {
	        break;
	    }
	}

	buffer_analyzed++;

        if (!(CVM_COMMON_CHECK_FPA_POOL(pool, buffer_addr)))
        {
 	    int correct_pool = 0;

	    invalid_buffers++;
	    printf("%s: Invalid buffer address (buffer=0x%llX, index=%d)\n", __FUNCTION__, CAST64(buffer_addr), cur_index);
	    correct_pool = CVM_COMMON_WHICH_FPA_POOL(buffer_addr);
	    if (correct_pool != -1)
	    {
	        printf("%s: Buffer 0x%llX belongs to pool %d, but was freed to pool %d\n", __FUNCTION__, CAST64(buffer_addr), correct_pool, pool);
	    }
	    continue;
        }

	/* find duplicate entries */
        segment = CVM_COMMON_WHICH_FPA_POOL_SEGMENT((uint64_t)buffer_addr, (uint32_t)pool);
	if (segment == -1)
        {
	    invalid_buffers++;
	    printf("%s: Invalid buffer address (buffer=0x%llX, index=%d)\n", __FUNCTION__, CAST64(buffer_addr), cur_index);
	    continue;
        }
        entry = cvm_common_fpa_info[pool].entry[segment];
        index = (uint32_t)(buffer_addr - entry.start_addr) / (uint32_t)entry.element_size;
	state = buf_array[index + offset[segment] ];

	if ( state & 0x8000 )
	{
	    duplicate_buffers++;
            printf("%s: Duplicate entries for buffer 0x%llX (previous entry is at %d)\n", __FUNCTION__, CAST64(buffer_addr), cur_index);
	    continue;
	}
	else
	{
	    state |= 0x8000;
	}

	/* remember the state */
	buf_array[index + offset[segment] ] = state;


	/* check the buffer boundry */
	if (  (state & 0x7fff) != (buffer_addr & 0x7fff) )
	{
 	    printf("%s: Un-aligned buffer 0x%llX at index %d\n", __FUNCTION__, CAST64(buffer_addr), cur_index);
	    unaligned_buffers++;
	    continue;
	}

	saved_buffers[cur_index] = buffer_addr;
	cur_index++;
    }

    for (i=0; i<cur_index; i++)
    {
        cvmx_fpa_free( CASTPTR(void,saved_buffers[i]), pool, 0);
    }

    CVM_COMMON_FREE_TSAFE(buf_array);
    CVM_COMMON_FREE_TSAFE(saved_buffers);

    printf("Buffer analyzation completed.\n");
    printf("    buffers analyzed          = %d\n", buffer_analyzed);
    printf("    duplicate entries         = %d\n", duplicate_buffers);
    printf("    invalid address entreis   = %d\n", invalid_buffers);
    printf("    unaligned address entries = %d\n", unaligned_buffers);
    printf("\n");
}


#ifdef CVM_CLI_APP

extern CVMX_SHARED unsigned int coremask_app;
extern CVMX_SHARED unsigned int coremask_data;

extern char* get_func_name(char* func_name, unsigned int func_addr);

void cvm_common_core_info(void)
{
  int i = 0;
  uint64_t core_old_status[CVMX_MAX_CORES];

  for (i=0; i<CVMX_MAX_CORES; i++) core_old_status[i] = CVM_COMMON_HISTORY_GET_CYCLE(i);

  cvmx_wait(600000000);

  printf("\n");

  for (i=0; i<16; i++)
  {
      if ((cvmx_coremask_core(i) & coremask_app) != 0)
      {
	  printf("Core %d : Application\n", i);
      }
  }

  for (i=0; i<16; i++)
  {
      if ((cvmx_coremask_core(i) & coremask_data) != 0)
      {
          printf("Core %d : Stack [%s]\n", i, (core_old_status[i] == CVM_COMMON_HISTORY_GET_CYCLE(i)) ? "Dead" : "Alive");
      }
  }

  printf("\n");

}


#ifdef CVM_FLOW_TRACE
void cvm_common_backtrace(int core_num)
{
    cvm_common_trace_t *trace=NULL;
    int i = 0, j = 0;
    int index = 0;
    char fname[256] = "< Function Name Not Specified >";

    if ( (core_num < 0) || (core_num >= CVMX_MAX_CORES) )
    {
        printf("cvm_common_backtrace : Invalid core number (%d)\n", core_num);
        return;
    }

    trace = CVM_COMMON_GET_HISTORY(core_num);

    printf("  -> no. of entries = %d\n", trace->no_of_entries);
    printf("  -> currnet index  = %d\n", trace->cur_index);

    index = (trace->no_of_entries < CVM_COMMON_TRACE_MAX_ENTRIES ? 0 : trace->cur_index);
    for (i=0; i<trace->no_of_entries; i++)
    {
        if (index >= CVM_COMMON_TRACE_MAX_ENTRIES) index = 0;

        if (trace->entry[index].trace_info.s.is_fn_ptr == CVM_COMMON_TRACE_FNPTR)
	{
	    get_func_name(fname, (unsigned int)trace->entry[index].trace_info.s.addr);
        }
	else
	{
	    strcpy( fname, cvm_common_trace_macro_names[trace->entry[index].trace_info.s.addr]);
	}

	printf(" %d --> [%lu] %s %s @ line no. %d ",
	       index,
	       trace->entry[index].cycles,
	       ( (trace->entry[index].trace_info.s.entry_exit == CVM_COMMON_TRACE_ENTER ? "Entring" : 
                 (trace->entry[index].trace_info.s.entry_exit == CVM_COMMON_TRACE_EXIT ? "Exiting" : "Inside")) ),
	       fname,
               trace->entry[index].trace_info.s.line_no);

	for (j=0; j<trace->entry[index].trace_info.s.info_count; j++)
	{
	    if (j==0) printf("(");
	    printf("0x%lx ", trace->entry[index].u_64_info[j]);
	}
	if (j>0) printf(")");
	printf("\n");

	index++;
	if (index >= CVM_COMMON_TRACE_MAX_ENTRIES) index = 0;
    }

}
#endif /* CVM_FLOW_TRACE */


void cvm_common_read_csr(uint64_t addr)
{
    uint64_t val = cvmx_read_csr(CVMX_ADD_IO_SEG(addr));

    printf("\n\tRegister = 0x%llX\n", CAST64(addr));
    printf("\tValue = %02llX %02llX %02llX %02llX - %02llX %02llX %02llX %02llX\n\n",
	   CAST64((uint32_t)( (val >> 56) & 0xff)),
	   CAST64((uint32_t)( (val >> 48) & 0xff)),
	   CAST64((uint32_t)( (val >> 40) & 0xff)),
	   CAST64((uint32_t)( (val >> 32) & 0xff)),
	   CAST64((uint32_t)( (val >> 24) & 0xff)),
	   CAST64((uint32_t)( (val >> 16) & 0xff)),
	   CAST64((uint32_t)( (val >> 8) & 0xff)),
	   CAST64((uint32_t)( (val >> 0) & 0xff))
	   );
}


void cvm_common_write_csr(uint64_t addr, uint64_t val)
{
    cvmx_write_csr(CVMX_ADD_IO_SEG(addr), val);
    printf("\n\tValue 0x%llX written to register = 0x%llX\n\n", CAST64(val), CAST64(addr));
}


void cvm_common_dump_pko(void)
{
    int i = 0;

    printf("\n");

    for (i=0; i<36; i++)
    {
        /* get the PKO stats for port 0 and PCI ports */
        cvmx_write_csr(CVMX_PKO_REG_READ_IDX, i);

        printf("\tPort %d : packets = %llu  ", i, CAST64(cvmx_read_csr(CVMX_PKO_MEM_COUNT0)) );
        printf("bytes   = %llu  ", CAST64(cvmx_read_csr(CVMX_PKO_MEM_COUNT1)) );
        printf("PKO_MEM_DBG9 = 0x%llx\n", CAST64(cvmx_read_csr(CVMX_PKO_MEM_DEBUG9)) );
    }

    printf("PKO_REG_ERROR  = 0x%llX\n", CAST64(cvmx_read_csr(CVMX_PKO_REG_ERROR)) );
    printf("PKO_REG_DEBUG0 = 0x%llX\n", CAST64(cvmx_read_csr(CVMX_PKO_REG_DEBUG0)));

    return;
}


#ifdef EXTRA_STATS
void cvm_common_display_extra_stats(void)
{
    printf("\nExtra stats\n\n");

    printf("\tTotal WQEs received           = %llu\n", CAST64(CVM_COMMON_EXTRA_STATS_GET64 (CVM_FAU_REG_WQE_RCVD)) );
    printf("\tTotal pkts received from wire = %llu\n", CAST64(CVM_COMMON_EXTRA_STATS_GET64 (CVM_FAU_REG_WQE_RCVD_FROM_WIRE)) );
    printf("\tTotal Non-IP packets received = %llu\n", CAST64(CVM_COMMON_EXTRA_STATS_GET64 (CVM_FAU_REG_NONIP_RECV)) );
    printf("\tTotal packets sent            = %llu\n", CAST64(CVM_COMMON_EXTRA_STATS_GET64 (CVM_FAU_REG_PKTS_OUT)) );
    printf("\tTotal bytes sent              = %llu\n", CAST64(CVM_COMMON_EXTRA_STATS_GET64 (CVM_FAU_REG_BYTES_OUT)) );
    printf("\tTotal Non-TCP/UDP pkts sent   = %llu\n", CAST64(CVM_COMMON_EXTRA_STATS_GET64 (CVM_FAU_REG_NONTCPUDP_SENT)) );
    printf("\tTCP listen backlog limit hit  = %llu\n", CAST64(CVM_COMMON_EXTRA_STATS_GET32 (CVM_FAU_REG_TCP_BKLOG_LIMIT_HIT)) );

    return;
}
#endif /* #ifdef EXTRA_STATS */

#endif /* CVM_CLI_APP */

#endif /*(__KERNEL__)*/



/**
 * cvm_common_packet_copy
 * - Makes a copy of the packet_buffer 
 *   (including chained list of packet buffers)
 * - allocates memory for packet_buffers
 *
 * Return value:
 * - ( 0) - success
 * - (-1) - failure (memory cannot be allocated)
 *        - frees all the allocated memory for packet buffers
 *        - sets ptr_to to NULL
 */
inline int cvm_common_packet_copy(cvmx_buf_ptr_t *ptr_to, cvmx_buf_ptr_t *ptr_from, int num_bufs, int total_len)
{
    int retval = 0;
    uint64_t ptr_from_start_of_buffer;
    uint64_t bytes_delta;

    if (cvmx_unlikely(num_bufs > 1))
    {
        int             nn;
        uint64_t        addr_new;
        cvmx_buf_ptr_t  ptr_new;
        cvmx_buf_ptr_t  ptr_prev;
        int             remaining_bytes;

        ptr_new.u64     = 0;
        ptr_prev.u64    = 0;
        remaining_bytes = total_len;
        nn              = 1;

        while (nn <= num_bufs)
        {
            addr_new = CAST64(cvmx_ptr_to_phys(cvm_common_alloc_fpa_buffer_sync(ptr_from->s.pool)));
            if (addr_new == 0x0)
            {
                CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_WARNING,
                        "%s: addr_new 0x%llx, cvm_common_alloc_fpa_buffer_sync FAILED, pool %d\n",
                        __FUNCTION__, CAST64(addr_new), ptr_from->s.pool);

                if (nn > 0)
                {

                    cvm_common_wqe_t swp_tmp;
                    cvm_common_wqe_t *swp_tmp_ptr = &swp_tmp;

                    swp_tmp_ptr->hw_wqe.packet_ptr   = *ptr_to;
                    swp_tmp_ptr->hw_wqe.word2.s.bufs = (nn - 1);

                    cvm_common_packet_free(swp_tmp_ptr);
                }

                ptr_to = NULL;
                return -1;
            }

            ptr_new.u64    = 0;
            ptr_new.s.pool = ptr_from->s.pool;
            ptr_new.s.back = ptr_from->s.back;
            ptr_new.s.i    = ptr_from->s.i;

            if (nn == num_bufs)  // last buffer
            {
                ptr_new.s.size = remaining_bytes;
            }
            else
            {
                ptr_new.s.size = ptr_from->s.size;
                remaining_bytes = remaining_bytes - ptr_from->s.size;
            }

            ptr_from_start_of_buffer = ((ptr_from->s.addr >> 7) - ptr_from->s.back) << 7;
            bytes_delta              = ptr_from->s.addr - ptr_from_start_of_buffer;

            memcpy(cvmx_phys_to_ptr(addr_new), cvmx_phys_to_ptr(ptr_from_start_of_buffer), ptr_new.s.size + bytes_delta);
            ptr_new.s.addr = addr_new + bytes_delta;
            ptr_new.s.back = CVM_COMMON_CALC_BACK_POWER_2(ptr_new);

            if (nn == 1)
                *ptr_to = ptr_new;
            else
                *((cvmx_buf_ptr_t *)(cvmx_phys_to_ptr(ptr_prev.s.addr - 8))) = ptr_new;

            ptr_prev = ptr_new;
            ptr_from = (cvmx_buf_ptr_t *)(cvmx_phys_to_ptr(ptr_from->s.addr - 8));
            ++nn;
        }

        CVMX_SYNCWS;
    }
    else
    {
        cvmx_buf_ptr_t ptr_new;
        uint64_t       addr_new;

        addr_new = CAST64(cvmx_ptr_to_phys(cvm_common_alloc_fpa_buffer_sync(ptr_from->s.pool)));
        if (addr_new == 0x0)
        {
            CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_WARNING,
                    "%s: addr_new 0x%llx, cvm_common_alloc_fpa_buffer_sync FAILED, pool %d\n",
                    __FUNCTION__, CAST64(addr_new), ptr_from->s.pool);
            return -1;
        }

        ptr_new.u64    = 0;
        ptr_new.s.pool = ptr_from->s.pool;
        ptr_new.s.back = ptr_from->s.back;
        ptr_new.s.i    = ptr_from->s.i;

        ptr_new.s.size           = total_len;
        ptr_from_start_of_buffer = ((ptr_from->s.addr >> 7) - ptr_from->s.back) << 7;
        bytes_delta              = ptr_from->s.addr - ptr_from_start_of_buffer;

        memcpy(cvmx_phys_to_ptr(addr_new), cvmx_phys_to_ptr(ptr_from_start_of_buffer), ptr_new.s.size + bytes_delta);
        ptr_new.s.addr = addr_new + bytes_delta;
        ptr_new.s.back = CVM_COMMON_CALC_BACK_POWER_2(ptr_new);

        *ptr_to = ptr_new;
    }

    return retval;
}


/**
 * cvm_common_swp_packet_copy
 * - Makes a copy of the work_queue_entry and the packet_buffer 
 *   (including chained list of packet buffers)
 * - allocates memory for work_queue_entry
 * - calls cvm_common_packet_copy
 *
 * Return value:
 * - ( 0) - success
 * - (-1) - failure (memory cannot be allocated)
 *        - frees the new work_queue_entry
 *        - sets swp_to to NULL
 */
int cvm_common_swp_packet_copy (cvm_common_wqe_t **swp_to_ptr, cvm_common_wqe_t *swp_from)
{
    int retval = 0;

    cvm_common_wqe_t *swp_to = NULL;
    swp_to = (cvm_common_wqe_t *)cvm_common_alloc_fpa_buffer_sync(CVMX_FPA_WQE_POOL);
    if (swp_to == NULL)
    {
        retval = -1;
        CVM_COMMON_DBG_MSG(CVM_COMMON_DBG_LVL_WARNING,
                "%s: retval %d, swp_to %p, cvm_common_alloc_fpa_buffer_sync FAILED, pool %d\n", 
                __FUNCTION__, retval, swp_to, CVMX_FPA_WQE_POOL);
        return retval;
    }
    memcpy(swp_to, swp_from, sizeof(cvm_common_wqe_t));

    retval = cvm_common_packet_copy (&swp_to->hw_wqe.packet_ptr, &swp_from->hw_wqe.packet_ptr, swp_from->hw_wqe.word2.s.bufs, swp_from->hwqe_w1_len);

    if (retval)
    {
        cvm_common_free_fpa_buffer (swp_to, CVMX_FPA_WQE_POOL, CVMX_FPA_WQE_POOL_SIZE/CVMX_CACHE_LINE_SIZE);
        swp_to = NULL;
    }
    else
    {
        *swp_to_ptr = swp_to;
    }

    return retval;
}


#define CVM_COMMON_PACKET_PTR_PRINT(packet_ptr) \
    printf("packet_ptr u64 0x%016llx, i %d, back %d, pool %d, size %4d, addr 0x%08llx\n", \
            CAST64(packet_ptr->u64), \
            packet_ptr->s.i,     \
            packet_ptr->s.back,  \
            packet_ptr->s.pool,  \
            packet_ptr->s.size,  \
            CAST64(packet_ptr->s.addr)); \


/**
 * cvm_common_packet_ptr_dump
 * - Prints packet_ptr info (including chained list of packet_ptr) for given swp
 *
 * Return value:
 * - none
 */
void cvm_common_packet_ptr_dump(cvm_common_wqe_t *swp)
{
    int nn       = 0;
    int num_bufs = swp->hw_wqe.word2.s.bufs;

    cvmx_buf_ptr_t *packet_ptr = &swp->hw_wqe.packet_ptr;

    if (num_bufs > 1)
    {
        printf("Chained List  ::: swp %p, len %d, bufs %d, gth %d --- packet_ptr u64 0x%llx, i %d, back %d, pool %d, size %4d, addr 0x%llx\n",
                swp, swp->hwqe_w1_len, swp->hw_wqe.word2.s.bufs, swp->control.gth, 
                CAST64(swp->hw_wqe.packet_ptr.u64),
                swp->hw_wqe.packet_ptr.s.i, 
                swp->hw_wqe.packet_ptr.s.back, 
                swp->hw_wqe.packet_ptr.s.pool, 
                swp->hw_wqe.packet_ptr.s.size, 
                CAST64(swp->hw_wqe.packet_ptr.s.addr));

        nn = 0;
        while (nn < swp->hw_wqe.word2.s.bufs)
        {
            printf("[%4d] --- ", nn); 
            CVM_COMMON_PACKET_PTR_PRINT(packet_ptr);
            packet_ptr = (cvmx_buf_ptr_t *)(cvmx_phys_to_ptr(packet_ptr->s.addr - 8));
            ++nn;
        }
    }
    else
    {
        printf("Single buffer ::: swp %p, len %d, bufs %d, gth %d --- ", 
                swp, swp->hwqe_w1_len, swp->hw_wqe.word2.s.bufs, swp->control.gth);
        CVM_COMMON_PACKET_PTR_PRINT(packet_ptr);
    }
    printf("\n");
    return;
}



/**
 * cvm_common_gather_list_dump
 * - Prints the gather list
 *
 * Return value:
 * - none
 */
void cvm_common_gather_list_dump(cvm_common_wqe_t *swp)
{
    cvmx_buf_ptr_t *packet_ptr;
    int nn; 
    int bufs;

    printf("Gather List   ::: swp %p, len %d, bufs %d, gth %d --- packet_ptr u64 0x%llx, i %d, back %d, pool %d, size %4d, addr 0x%llx\n",
            swp, swp->hwqe_w1_len, swp->hw_wqe.word2.s.bufs, swp->control.gth, 
            CAST64(swp->hw_wqe.packet_ptr.u64),
            swp->hw_wqe.packet_ptr.s.i, 
            swp->hw_wqe.packet_ptr.s.back, 
            swp->hw_wqe.packet_ptr.s.pool, 
            swp->hw_wqe.packet_ptr.s.size, 
            CAST64(swp->hw_wqe.packet_ptr.s.addr));

    if (swp->control.gth > 0)
    {
        bufs    = swp->hw_wqe.word2.s.bufs;
        packet_ptr = (cvmx_buf_ptr_t *)cvmx_phys_to_ptr(swp->hw_wqe.packet_ptr.s.addr);

        for (nn=0; nn<bufs; nn++)
        {
            printf("[%4d] --- ", nn); 
            CVM_COMMON_PACKET_PTR_PRINT(packet_ptr);
            ++packet_ptr;
        }
        printf("\n");
    }

    return;
}




 
