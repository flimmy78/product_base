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

#include "cvm-drv-debug.h"
#include "cvm-driver-defs.h"


#define  MAX_DEBUG_FN    16

/* Array of pointers to debug functions. */
void (*cvm_drv_dbg_fn_list[MAX_DEBUG_FN])(void *);
/* Array of void * arguments to pass to the above functions. */
void *cvm_drv_dbg_fn_arg_list[MAX_DEBUG_FN];
/* Number of functions currently registered. */
int cvm_drv_dbg_fn_count=0;


#ifdef CVM_DRV_SANITY_CHECKS
#define MAX_DBG_PTR_CNT  128 
CVMX_SHARED  uint64_t  dbg_lptrs[MAX_DBG_PTR_CNT];
CVMX_SHARED  uint64_t  dbg_lptr_cnt=0;
CVMX_SHARED  uint64_t  dbg_lptr_rollover=0;
CVMX_SHARED  uint64_t  dbg_rptrs[MAX_DBG_PTR_CNT];
CVMX_SHARED  uint64_t  dbg_rptr_cnt=0;
CVMX_SHARED  uint64_t  dbg_rptr_rollover=0;

void 
cvm_drv_reset_dbg_ptr_cnt(void)
{
  dbg_lptr_cnt = 0;
  dbg_rptr_cnt = 0;
}

void
cvm_drv_add_dbg_lptr(uint64_t  ptr)
{
   dbg_lptrs[dbg_lptr_cnt++] =  ptr;
   if(dbg_lptr_cnt == MAX_DBG_PTR_CNT) {
      dbg_lptr_rollover=1;
      dbg_lptr_cnt = 0; 
   }
}



void
cvm_drv_add_dbg_rptr(uint64_t  ptr)
{
   dbg_rptrs[dbg_rptr_cnt++] =  ptr;
   if(dbg_rptr_cnt == MAX_DBG_PTR_CNT) {
      dbg_rptr_rollover=1;
      dbg_rptr_cnt = 0; 
   }
}


void
cvm_drv_print_dbg_lptr(void)
{
   int i,cnt;
   printf("\n--- Debug local pointers ---\n");
   cnt = (dbg_lptr_rollover)?MAX_DBG_PTR_CNT:dbg_lptr_cnt;
   if(dbg_lptr_rollover) 
     printf("Rollover occured. idx at %lu now\n", dbg_lptr_cnt);
   for(i = 0; i < cnt; i++) {
      printf("dbg_lptrs[%d]: 0x%016lx\n", i, dbg_lptrs[i]);
   }
}


void
cvm_drv_print_dbg_rptr(void)
{
   int i, cnt;
   printf("\n--- Debug remote pointers ---\n");
   cnt = (dbg_rptr_rollover)?MAX_DBG_PTR_CNT:dbg_rptr_cnt;
   if(dbg_rptr_rollover) 
     printf("Rollover occured. idx at %lu now\n", dbg_rptr_cnt);
   for(i = 0; i < cnt; i++) {
      printf("dbg_rptrs[%d]: 0x%016lx\n", i, dbg_rptrs[i]);
   }
}


void
cvm_drv_print_dbg_ptrs(void)
{
   cvm_drv_print_dbg_lptr();
   cvm_drv_print_dbg_rptr();
}
#endif



void
cvm_drv_print_data(void *udata, uint32_t size)
{
  uint32_t   i, j;
  uint8_t   *data = (uint8_t *)udata;

  printf("---   Printing %d bytes at %p\n", size, udata);
  j = 0;
  for(i = 0; i < size; i++) {
    printf(" %02x", data[i]);
    if((i & 0x7) == 0x7) {
      printf(" << %d - %d\n", j, i);
      j = i+1;
    }
  }
  printf("\n");
}
                                                                                                
void
cvm_drv_print_wqe(cvmx_wqe_t  *wqe)
{
	int len = cvmx_wqe_get_len(wqe);

    printf("\n------ PRINT WQE @ %p------\n", wqe);
    printf("swp: unused: 0x%x tag: 0x%x, tt: 0x%x iprt: 0x%x \n",
       cvmx_wqe_get_unused8(wqe), (uint32_t)wqe->word1.tag, (uint32_t)wqe->word1.tag_type, cvmx_wqe_get_port(wqe));
    printf("wqe->pkt_ptr: 0x%016llx (addr: 0x%llx size: %d)\n",
	       cast64(wqe->packet_ptr.u64), cast64(wqe->packet_ptr.s.addr), wqe->packet_ptr.s.size);
    printf("segs: %d len: %d\n", wqe->word2.s.bufs, len);
    printf("WQE Header - 4 64-bit Words\n");
    cvm_drv_print_data(wqe, 32);
    printf("WQE Data Payload - first 64 bytes\n");
    cvm_drv_print_data(((uint8_t *)wqe + 32), 64);
    if(wqe->packet_ptr.s.addr) {
       if(len > 128) len = 128;
       printf("WQE Packet Ptr - first %d bytes\n", len);
       cvm_drv_print_data(CVM_DRV_GET_PTR(wqe->packet_ptr.s.addr), len);
    }
    printf("\n------ PRINT WQE @ %p DONE------\n", wqe);
}



char *
cvm_drv_get_tag_type(int tt)
{
	switch(tt) {
		case CVMX_POW_TAG_TYPE_ORDERED: return "Ordered";
		case CVMX_POW_TAG_TYPE_ATOMIC:  return "Atomic";
		case CVMX_POW_TAG_TYPE_NULL:    return "Null";
		case CVMX_POW_TAG_TYPE_NULL_NULL: return "Null-Null";
		default: return "Invalid";
	}
	return "";
}

void
cvm_drv_print_pci_instr(cvmx_raw_inst_front_t     *front)
{
	printf("\n--- Printing PCI Instruction\n");

	printf("IH: 0x%016llx\n    ", cast64(front->ih.u64));
	if(front->ih.s.r) printf("RAW ");
	if(front->ih.s.rs) printf("SHORT ");
	printf("PM: %d ", front->ih.s.pm);
	printf("SL: %d ", front->ih.s.sl);
	printf("GRP: %d ", front->ih.s.grp);
	printf("QOS: %d ", front->ih.s.qos);
	printf("TAG: 0x%08x (%s)\n", front->ih.s.tag,
	       cvm_drv_get_tag_type(front->ih.s.tt));

	printf("IRH: 0x%016llx\n    ", cast64(front->irh.u64));
	if(front->irh.s.scatter) printf("SCATTER ");
	printf("Opcode: 0x%04x ", front->irh.s.opcode);
	printf("Param: 0x%02x ", front->irh.s.param);
	printf("DPort: %d ", front->irh.s.dport);
	printf("RLen: %d ", front->irh.s.rlenssz);
	printf("ReqId: %d\n", front->irh.s.rid);

	printf("RPTR: 0x%016llx\n", cast64(front->rptr));
	printf("\n");
}

  


int
cvm_drv_register_debug_fn(void (*fn)(void *), void *arg)
{
   if(cvm_drv_dbg_fn_count == MAX_DEBUG_FN) {
       printf("[ DRV ]: Reached maximum allowed debug functions\n");
       return 1;
   }
   cvm_drv_dbg_fn_list[cvm_drv_dbg_fn_count]       = fn;
   cvm_drv_dbg_fn_arg_list[cvm_drv_dbg_fn_count++] = arg;
   return 0;
}



void
cvm_drv_debug_fn(void)
{
   int i;

   for(i = 0; i < cvm_drv_dbg_fn_count; i++) {
       (*cvm_drv_dbg_fn_list[i])(cvm_drv_dbg_fn_arg_list[i]);
   }
}




#ifdef SANITY_CHECKS
int pci_output_sanity_and_buffer_count_update(cvmx_buf_ptr_t      lptr,
                                              uint32_t            pko_ptr_type,
                                              uint32_t            segs,
                                              uint32_t            total_bytes,
                                              uint32_t            port)
{
    int i = 0;

    if (pko_ptr_type == CVM_DIRECT_DATA) {
        /* lptr is pointing to the data packet to be sent out - no chaining */
        if ( (lptr.s.addr & ~127) == 0x0) {
	   printf("<< PCI O/P SANITY CHECK >> invalid buffer (0x%llX)\n", cast64(lptr.u64));
	   return (0);
	}

	if (lptr.s.i) {
           if(!CVM_COMMON_CHECK_FPA_POOL(lptr.s.pool, (lptr.s.addr & ~127))) {
                printf("<< PCI O/P SANITY CHECK >> DIRECT; trying to free buffer to incorrect pool lptr: 0x%016llx \n", cast64(lptr.u64));
		return (0);
            }
#ifdef FPA_CHECKS
            CVM_COMMON_MARK_FPA_BUFFER_FREE((lptr.s.addr & ~127),lptr.s.pool,2);
#endif
	}
    } /* CVM_DIRECT_DATA */
    else
    if (pko_ptr_type == CVM_LINKED_DATA) {
        cvm_common_buffer_check_single(lptr, total_bytes, 0, 0, 0);
    } /* CVM_LINKED_DATA */
    else 
    if (pko_ptr_type == CVM_GATHER_DATA)
    {
	uint64_t *gather_list_addr = NULL;

        if ((lptr.s.addr & ~127) == 0x0) {
	   printf("<< PCI O/P SANITY CHECK >> invalid buffer (0x%llX)\n", cast64(lptr.u64));
	   return (0);
	}

	if (lptr.s.i) {
           if (!CVM_COMMON_CHECK_FPA_POOL(lptr.s.pool, (lptr.s.addr & ~127))) {
                printf("<< PCI O/P SANITY CHECK >> GATHER ; trying to free buffer to incorrect pool (lptr=0x%llX)\n", cast64(lptr.u64));
		return (0);
            }
#ifdef FPA_CHECKS
            CVM_COMMON_MARK_FPA_BUFFER_FREE( (lptr.s.addr & ~127), lptr.s.pool, 3);
#endif
	}

	gather_list_addr = (uint64_t *)((unsigned long)lptr.s.addr);

	for (i=0; i<(int)segs; i++)
	{
	    cvmx_buf_ptr_t ptr = (cvmx_buf_ptr_t)((gather_list_addr)[i]);
            cvm_common_buffer_check_single(ptr, ptr.s.size, 0, 0, 0);
	}
    } /* CVM_FATHER_DATA */
    else {
        printf("<< PCI O/P SANITY CHECK >> Invalid pko ptr type specified (%d)\n", pko_ptr_type);
    }

    return (0);
}
#endif

/* $Id: cvm-drv-debug.c 58711 2011-04-02 00:52:08Z panicker $ */

