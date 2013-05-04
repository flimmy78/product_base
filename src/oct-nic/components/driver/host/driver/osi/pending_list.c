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




#include "octeon_main.h"
#include "octeon_debug.h"
#include "octeon_macros.h"


int
octeon_init_pending_list(octeon_device_t  *oct)
{
   uint32_t   i, count = 0;

   if(oct->chip_id <= OCTEON_CN58XX)
      count = (CHIP_FIELD(oct, cn3xxx, conf))->c.pending_list_size;

   if(oct->chip_id == OCTEON_CN56XX)
      count = (CHIP_FIELD(oct, cn56xx, conf))->c.pending_list_size;

   if(oct->chip_id == OCTEON_CN63XX)
      count = (CHIP_FIELD(oct, cn63xx, conf))->c.pending_list_size;

   if(oct->chip_id == OCTEON_CN66XX)
      count = (CHIP_FIELD(oct, cn66xx, conf))->c.pending_list_size;

   if(oct->chip_id == OCTEON_CN68XX)
      count = (CHIP_FIELD(oct, cn68xx, conf))->c.pending_list_size;
 
   oct->pend_list_size = count;

   oct->plist = cavium_alloc_virt(sizeof(octeon_pending_list_t));
   if(!oct->plist)  {
       cavium_error("OCTEON: Allocation failed for pending list\n");
       return 1;
   }
   cavium_memset(oct->plist, 0, sizeof(octeon_pending_list_t));

   oct->plist->list = (octeon_pending_entry_t *) cavium_alloc_virt(OCT_PENDING_ENTRY_SIZE * count);
   if(!oct->plist->list)  {
       cavium_error("OCTEON: Allocation failed for pending list\n");
       cavium_free_virt(oct->plist);
       return 1;
   }


   oct->plist->free_list = (uint32_t *) cavium_alloc_virt(sizeof(uint32_t) * count);

   if(!oct->plist->free_list) {
        cavium_error("OCTEON: Allocation failed for pending free_list\n");
        cavium_free_virt(oct->plist->list);
        cavium_free_virt(oct->plist);
        return 1;
   }
   oct->plist->entries = count;
   cavium_memset((void *)oct->plist->list, 0,OCT_PENDING_ENTRY_SIZE * count);
   for (i = 0; i < count; i++)  {
      oct->plist->free_list[i]   = i;
      oct->plist->list[i].status = OCTEON_PENDING_ENTRY_FREE;
   }

   cavium_spin_lock_init(&oct->plist->lock);
   oct->plist->free_index = 0;
   cavium_atomic_set(&oct->plist->instr_count, 0);

   return 0;
}









int
octeon_delete_pending_list(octeon_device_t    *oct)
{
  uint32_t                     i, pl_index;
  octeon_soft_instruction_t   *pend_instr;

  cavium_spin_lock(&oct->plist->lock);

  for ( i = 0; i < oct->plist->free_index; i++) {
    pl_index = oct->plist->free_list[i];
    if(oct->plist->list[pl_index].status != OCTEON_PENDING_ENTRY_FREE){
        cavium_print(PRINT_DEBUG,"pending list index: %d is used \n", pl_index);
        pend_instr = oct->plist->list[pl_index].instr;
        delete_soft_instr(oct, pend_instr);
        oct->plist->list[pl_index].status = OCTEON_PENDING_ENTRY_FREE;
     } 
  }
  if(oct->plist->list)
       cavium_free_virt (oct->plist->list);
  if(oct->plist->free_list)
       cavium_free_virt (oct->plist->free_list);

  cavium_spin_unlock(&oct->plist->lock);
  cavium_free_virt (oct->plist);

  return 0;

}









int
wait_for_pending_requests(octeon_device_t   *oct)
{
	int pcount = 0, loop_count = 100;

	/* Wait for pending requests to finish. */
	do {

		pcount = (int)cavium_atomic_read(&oct->plist->instr_count);
		if(pcount)
			cavium_sleep_timeout(CAVIUM_TICKS_PER_SEC/10);

	} while(pcount && (loop_count--));

	if(pcount) {
		cavium_error("OCTEON: There are %d outstanding requests\n", pcount);
#ifdef CAVIUM_DEBUG
		{
			int lvl = octeon_debug_level;
			octeon_debug_level = PRINT_DEBUG;
			print_pending_list(oct);
			octeon_debug_level = lvl;
		}
#endif
		return 1;
	}

	return 0;
}










octeon_pending_entry_t *
add_to_pending_list(octeon_device_t            *oct,
                    octeon_soft_instruction_t  *si)
{
    uint32_t pl_index;

    /* Grab the lock for pending list now */
    cavium_spin_lock_softirqsave(&oct->plist->lock);

    if(oct->plist->free_index == oct->pend_list_size)  {
        cavium_error("OCTEON: No space in pending list; free_index: %d\n",
                     oct->plist->free_index);
        /* Release the lock for pending list now */
        cavium_spin_unlock_softirqrestore(&oct->plist->lock);
        return NULL;
    }

    pl_index = oct->plist->free_list[oct->plist->free_index];
    if(oct->plist->list[pl_index].status != OCTEON_PENDING_ENTRY_FREE){
        cavium_error("OCTEON: Pending list entry at %d is occupied\n",pl_index);
        /* Release the lock for pending list now */
        cavium_spin_unlock_softirqrestore(&oct->plist->lock);
        return NULL;
    } 
    oct->plist->free_index++;


    oct->plist->list[pl_index].instr      = si;
    oct->plist->list[pl_index].request_id = pl_index;
    oct->plist->list[pl_index].status     = OCTEON_PENDING_ENTRY_USED;
    oct->plist->list[pl_index].iq_no      = SOFT_INSTR_IQ_NO(si);


    /* Increment the count of pending instructions. */
#ifdef CAVIUM_DEBUG
    cavium_atomic_check_and_inc(&oct->plist->instr_count, oct->plist->entries, __CVM_FILE__, __CVM_LINE__);
#else
    cavium_atomic_inc(&oct->plist->instr_count);
#endif

    /* Release the lock for pending list now */
    cavium_spin_unlock_softirqrestore(&oct->plist->lock);

    si->req_info.status     = OCTEON_REQUEST_PENDING;
    si->irh.rid             = pl_index;
    si->req_info.request_id = pl_index;

    /* The timeout is specified in millisecs. We need to convert to the
       internal clock tick representation. 05.10.05
    */
    si->req_info.timeout = CAVIUM_INTERNAL_TIME(si->req_info.timeout);
    cavium_print(PRINT_DEBUG,"Instr timeout is %u; timeout calculated is %lu\n",
                 si->req_info.timeout, si->timeout);

    return(&oct->plist->list[pl_index]);
}










void 
release_from_pending_list(octeon_device_t         *oct,
                          octeon_pending_entry_t  *pe)
{

   cavium_spin_lock_softirqsave(&oct->plist->lock);
   oct->plist->free_index--;
   oct->plist->free_list[oct->plist->free_index] = pe->request_id;
   pe->status = OCTEON_PENDING_ENTRY_FREE;

   /* Decrement the count of pending instructions. */
   cavium_atomic_dec(&oct->plist->instr_count);

   cavium_spin_unlock_softirqrestore(&oct->plist->lock);
   return ;
}



/* $Id: pending_list.c 66446 2011-10-25 02:31:59Z mchalla $ */

