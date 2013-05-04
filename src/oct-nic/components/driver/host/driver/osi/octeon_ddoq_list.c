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

#include "octeon_ddoq_list.h"
#include "octeon_ddoq.h"

/* Time (in seconds) to wait for ddoq delete completion. */
#define DDOQ_DELETE_WAIT_TIME                1


extern oct_poll_fn_status_t 
ddoq_list_poll_fn(void   *octeon_dev, unsigned long  arg);




int
octeon_init_ddoq_list(int  octeon_id)
{
  int i;
  octeon_device_t  *octeon_dev;
  ddoq_entry_t  **dlist2, *dlist3;
  octeon_ddoq_list_t    *ddoq_list;
  octeon_poll_ops_t  poll_ops;



  cavium_print(PRINT_FLOW, "\n======= INIT DDOQ LIST started\n");

  octeon_dev = get_octeon_device_ptr(octeon_id);
  if(!octeon_dev) {
     cavium_error("OCTEON_CNTQ: DDOQ LIST INIT Invalid Octeon Id %d\n",
                  octeon_id);
     return ENODEV;
  }

  ddoq_list = cavium_malloc_dma(sizeof(octeon_ddoq_list_t),  __CAVIUM_MEM_GENERAL);
  if(!ddoq_list) {
     cavium_error("OCTEON_CNTQ: Memory Allocated failed at %s:%d\n",
                   __CVM_FUNCTION__, __CVM_LINE__);
     return ENOMEM;
  }

  octeon_dev->ddoq_list = (octeon_ddoq_list_t *)ddoq_list;
  cavium_memset(ddoq_list, 0, sizeof(octeon_ddoq_list_t));

  ddoq_list->level1 = cavium_malloc_dma(sizeof(void *) * (1 << LEVEL1_BITS), __CAVIUM_MEM_GENERAL);
  if(ddoq_list->level1 == NULL)
    goto DDOQ_INIT_FREE_LIST;

  ddoq_list->level1[0]  = cavium_malloc_dma(sizeof(void *) * (1 << LEVEL2_BITS), __CAVIUM_MEM_GENERAL);
  if(ddoq_list->level1[0] == NULL) 
    goto DDOQ_INIT_FREE_LIST1;
  dlist2     = ddoq_list->level1[0];
  ddoq_list->l2_count=1;


  dlist2[0]  = cavium_malloc_dma(OCT_DDOQ_ENTRY_SIZE * (1 << LEVEL3_BITS), __CAVIUM_MEM_GENERAL);
  if(dlist2[0] == NULL) 
    goto DDOQ_INIT_FREE_LIST2;
  dlist3     = dlist2[0];


  /* An array with one element for each level2 array
     -  to indicate how many level3 table have been made 
  */
  ddoq_list->l3_count = (uint16_t *)cavium_malloc_dma(sizeof(uint16_t) * ( 1 << LEVEL1_BITS), __CAVIUM_MEM_GENERAL);
  if (ddoq_list->l3_count == NULL)
    goto DDOQ_INIT_FREE_LIST3;
  ddoq_list->l3_count[0] = 1;
  cavium_memset(dlist3, 0, OCT_DDOQ_ENTRY_SIZE * ( 1 << LEVEL3_BITS));
  for (i = 0 ; i < (1 << LEVEL3_BITS); i++)  {
    dlist3[i].index = DDOQ_ID_OFFSET + ddoq_list->ddoq_allocated++;
  }

  ddoq_list->next_free_index=0;

  cavium_spin_lock_init(&ddoq_list->lock);
  CAVIUM_INIT_LIST_HEAD(&ddoq_list->deferred);

  poll_ops.fn      = ddoq_list_poll_fn;
  poll_ops.fn_arg  = 0;
  poll_ops.ticks   = 1;
  strcpy(poll_ops.name, "DDOQ List");
  octeon_register_poll_fn(octeon_id, &poll_ops);

  cavium_print(PRINT_FLOW, "\n========INIT DDOQ LIST completed\n");

  return 0;


DDOQ_INIT_FREE_LIST3:
   cavium_free_dma(dlist3);
DDOQ_INIT_FREE_LIST2:
   cavium_free_dma(dlist2);
DDOQ_INIT_FREE_LIST1:
   cavium_free_dma(ddoq_list->level1);
DDOQ_INIT_FREE_LIST:
   cavium_free_dma(ddoq_list);

   return 1;
}







int
octeon_delete_ddoq_list(int octeon_id)
{
   uint32_t              i, j, k, delete_wait=0;
   ddoq_entry_t        **dlist2, *dlist3;
   octeon_device_t      *octeon_dev;
   octeon_ddoq_list_t   *ddoq_list;

   octeon_dev = get_octeon_device_ptr(octeon_id);
   if(!octeon_dev) {
      cavium_error("OCTEON_CNTQ: DDOQ LIST DELETE: Invalid Octeon Id %d\n",
                   octeon_id);
      return ENODEV;
   }
   ddoq_list = (octeon_ddoq_list_t *)octeon_dev->ddoq_list;
   if(ddoq_list == NULL) {
      cavium_error("OCTEON_CNTQ: No ddoq list to delete for Octeon %d\n",
                   octeon_id);
      return ENODEV;
   }

   octeon_unregister_poll_fn(octeon_id, ddoq_list_poll_fn, 0);


   cavium_spin_lock_softirqsave(&ddoq_list->lock);
   ddoq_list->cleanup_active = 1;
   cavium_spin_unlock_softirqrestore(&ddoq_list->lock);

   /* A call to delete DDOQ's was made earlier at cleanup() start */
   /* The DDOQ's should have been deleted at that time. If not there's
      not much to be done here. */

   /* Do not wait for ever. After a time out period, delete ddoq list. */
   while(ddoq_list->stats.active_ddoqs) {
     cavium_error("OCTEON_CNTQ: still waiting for active DDOQ's to die\n");
     cavium_sleep_timeout(CAVIUM_TICKS_PER_SEC);
     if(++delete_wait == DDOQ_DELETE_WAIT_TIME) {
        cavium_error("OCTEON_CNTQ: Time-out waiting for DDOQ delete completion\n");
        break;
     }
   }
   for( i = 0; i < ddoq_list->l2_count; i++)  {
      dlist2 = ddoq_list->level1[i];
      for( j = 0; j < ddoq_list->l3_count[i]; j++)  {

         dlist3 = dlist2[j];
         for( k = 0; k < (1 << LEVEL3_BITS); k++)  {

              if(dlist3[k].ddoq_ptr) {
                 void  *ddoq = dlist3[k].ddoq_ptr;
                 cavium_free_dma(ddoq);
              }
         }
         cavium_print(PRINT_DEBUG,"Releasing ddoq_list3 ddoq ptr mem at %p\n",
                      dlist2[j]);
         cavium_free_dma(dlist2[j]);
      }
      cavium_print(PRINT_DEBUG, "Releasing level2 ddoq ptr memory at %p\n",
                   ddoq_list->level1[i]);
      cavium_free_dma(ddoq_list->level1[i]);
   }

   cavium_print(PRINT_DEBUG,"Releasing level1 ddoq ptr memory at %p\n",
                ddoq_list->level1);
   cavium_free_dma(ddoq_list->level1);

   cavium_print(PRINT_DEBUG, "Releasing l3_count array\n");
   cavium_free_dma(ddoq_list->l3_count);

	cavium_print_msg("DDOQ STATS at module unload:\n");
	cavium_print_msg("Created: %llu  Create Failed: %llu Active: %llu\n",
             CVM_CAST64(ddoq_list->stats.ddoqs_created),
             CVM_CAST64(ddoq_list->stats.create_failed),
             CVM_CAST64(ddoq_list->stats.active_ddoqs));
	cavium_print_msg("Pending Core Termination: %llu DeleteB4Create: %llu\n",
             CVM_CAST64(ddoq_list->stats.ddoqs_pending_eot),
             CVM_CAST64(ddoq_list->stats.del_b4_crt));
   cavium_free_dma(ddoq_list);

   /* This field can be checked during hot reset. Make it NULL so that if
      reset occurs twice, the stale ddoq_list won't be dereferenced. */
   octeon_dev->ddoq_list = NULL;

   return 0;
}















/*
  Puts the ddoq id into the free list. Releases the memory for DDOQ and 
  make the ptr at ddoq_id location NULL.
  this routine SHOULD NOT be called with the DDOQ lock held 
*/

int
release_ddoq_entry(octeon_device_t    *oct,
                   uint32_t            ddoq_id)
{
   octeon_ddoq_list_t   *ddoq_list = (octeon_ddoq_list_t  *)oct->ddoq_list;
   ddoq_entry_t       **list2, *list3;
   register uint32_t    i, idx, idx_level1, idx_level2, idx_level3;


   idx = ddoq_id - DDOQ_ID_OFFSET;
   idx_level3 = (idx & LEVEL3_MASK);
   idx_level2 = ((idx & LEVEL2_MASK) >> LEVEL3_BITS);
   idx_level1 = ((idx & LEVEL1_MASK) >> (LEVEL3_BITS + LEVEL2_BITS));

   list2 = ddoq_list->level1[idx_level1];
   list3 = list2[idx_level2];

   cavium_spin_lock_softirqsave(&ddoq_list->lock);

   if(list3[idx_level3].ddoq_ptr)  {
      list3[idx_level3].ddoq_ptr = NULL;
   } else {
      cavium_error("OCTEON_CNTQ: No DDOQ pointer found at index %d\n", idx);
   }

#if 0
   /* Save the freed ddoq id in a temp list so that the number is not
      re-assigned immediately. Save only upto 255. The 256th will be
      the current ddoq_id. */
   if(ddoq_list->free_id_count < MAX_DEFERRED_IDS) {
      ddoq_list->free_ids[ddoq_list->free_id_count++] = ddoq_id; 
      cavium_spin_unlock_softirqrestore(&ddoq_list->lock);
      return 0;
   }
#endif

   ddoq_list->free_ids[ddoq_list->free_id_count++] = ddoq_id;

   /* Free the ids in a FIFO manner */
   for(i = 0; i < ddoq_list->free_id_count; i++) {
       ddoq_list->next_free_index--;
       idx_level3 = (ddoq_list->next_free_index & LEVEL3_MASK);
       idx_level2 = ((ddoq_list->next_free_index & LEVEL2_MASK) >> LEVEL3_BITS);
       idx_level1 = ((ddoq_list->next_free_index & LEVEL1_MASK) >> (LEVEL3_BITS + LEVEL2_BITS));

       list2 = ddoq_list->level1[idx_level1];
       list3 = list2[idx_level2];
       list3[idx_level3].index = ddoq_list->free_ids[i]; 
    }
    ddoq_list->free_id_count = 0;
 
    cavium_spin_unlock_softirqrestore(&ddoq_list->lock);
    return 0;
}






uint32_t
get_next_idx(octeon_ddoq_list_t  *ddoq_list, 
             uint32_t              *ddoq_id,
             void                ***ddoq_ptr)
{
	uint32_t   idx, idx_level1, idx_level2, idx_level3;
	register ddoq_entry_t  **dlist2, *dlist3;
	uint16_t  *l3_count, l2_count;
	uint16_t  i;


	cavium_print(PRINT_FLOW, "\n\n---# get_next_idx #---\n");
	cavium_print(PRINT_DEBUG, "\nnext_free_index: %d\n", ddoq_list->next_free_index);
	cavium_spin_lock_softirqsave(&ddoq_list->lock);

	if (ddoq_list->next_free_index < ddoq_list->ddoq_allocated)   {

		idx_level3 = (ddoq_list->next_free_index & LEVEL3_MASK);
		idx_level2 = ((ddoq_list->next_free_index & LEVEL2_MASK) >>LEVEL3_BITS);
		idx_level1 = ((ddoq_list->next_free_index & LEVEL1_MASK) >>(LEVEL3_BITS + LEVEL2_BITS));

		dlist2 = ddoq_list->level1[idx_level1];
		dlist3 = dlist2[idx_level2];
		*ddoq_id = dlist3[idx_level3].index;
		idx      = *ddoq_id - DDOQ_ID_OFFSET;

		if (idx != ddoq_list->next_free_index) {
			idx_level3 = (idx & LEVEL3_MASK);
			idx_level2 = ((idx & LEVEL2_MASK) >> LEVEL3_BITS);
			idx_level1 = ((idx & LEVEL1_MASK) >> (LEVEL3_BITS + LEVEL2_BITS));
		}

		dlist2 = ddoq_list->level1[idx_level1];
		dlist3 = dlist2[idx_level2];
		*ddoq_ptr = &dlist3[idx_level3].ddoq_ptr;

	} else {

		l3_count = ddoq_list->l3_count;
		l2_count = ddoq_list->l2_count;

		if(ddoq_list->ddoq_allocated < MAX_DDOQ) {

			if(l3_count[l2_count-1] <  (1 << LEVEL2_BITS)) {
				dlist2 = ddoq_list->level1[l2_count - 1];
			} else {

				if(l2_count < (1 << LEVEL1_BITS))  {

					ddoq_list->level1[l2_count] = cavium_malloc_dma(sizeof(void *) * ( 1 << LEVEL2_BITS), __CAVIUM_MEM_ATOMIC);
					if(ddoq_list->level1[l2_count] == NULL) {
						goto get_next_idx_failed;
					}
					dlist2 = ddoq_list->level1[l2_count];
					ddoq_list->l2_count++;
					l2_count = ddoq_list->l2_count;

				} else  {
					goto get_next_idx_failed;
				}
			}

			dlist2[l3_count[l2_count - 1]] = cavium_malloc_dma(OCT_DDOQ_ENTRY_SIZE * (1 << LEVEL3_BITS), __CAVIUM_MEM_ATOMIC);

			if(dlist2[l3_count[l2_count - 1]] == NULL) {
				goto get_next_idx_failed;
			}

			dlist3 =  dlist2[l3_count[l2_count - 1]];
			l3_count[l2_count - 1]++;
			*ddoq_id = DDOQ_ID_OFFSET + ddoq_list->ddoq_allocated ;
			*ddoq_ptr = &dlist3[0].ddoq_ptr;
			cavium_memset(dlist3, 0, OCT_DDOQ_ENTRY_SIZE * ( 1 << LEVEL3_BITS));

			for (i = 0 ; i < (1 << LEVEL3_BITS); i++)  {
				dlist3[i].index = DDOQ_ID_OFFSET + ddoq_list->ddoq_allocated++;
				cavium_print(PRINT_DEBUG, "Val at %p is %d\n", &dlist3[i], dlist3[i].index);
			}

		} else
			goto get_next_idx_failed;
	} /* if next_free_index == ddoq_allocated */

	ddoq_list->next_free_index++;
	cavium_spin_unlock_softirqrestore(&ddoq_list->lock);
	return 1;

get_next_idx_failed:
	cavium_spin_unlock_softirqrestore(&ddoq_list->lock);
	*ddoq_ptr = NULL;
	*ddoq_id   = 0;
	return 0;
}




void
ddoq_list_update_count(octeon_device_t  *octeon_dev,
                       uint32_t          action)
{
   octeon_ddoq_list_t   *ddoq_list  = (octeon_ddoq_list_t *)octeon_dev->ddoq_list;

   cavium_spin_lock_softirqsave(&ddoq_list->lock);
   switch(action) {
     /* INIT is sent after the creation of DDOQ is successful */
       case  DDOQ_INIT_OP :
             ddoq_list->stats.ddoqs_created++;
             ddoq_list->stats.active_ddoqs++;
             break;
       case  DDOQ_DELETE_OP :
             ddoq_list->stats.ddoqs_pending_eot++;
             break;
       case  DDOQ_EOT_OP :
             ddoq_list->stats.ddoqs_pending_eot--;
             ddoq_list->stats.active_ddoqs--;
             break;
       case  DDOQ_FAIL_CODE :
             ddoq_list->stats.create_failed++;
             break;
       case  DDOQ_DELETE_B4_CREATE_CODE:
             ddoq_list->stats.del_b4_crt++;
             break;
   } 
   cavium_spin_unlock_softirqrestore(&ddoq_list->lock);
}










uint64_t
octeon_get_active_ddoq_count(int   octeon_id)
{
	octeon_device_t       *octeon_dev;
	octeon_ddoq_list_t    *ddoq_list;
	uint64_t               retval=CVM_CAST64(~0);

	octeon_dev = get_octeon_device_ptr(octeon_id);
	if(octeon_dev == NULL) {
		cavium_error("OCTEON_CNTQ: %s: Invalid octeon id %d\n",
		             __CVM_FUNCTION__, octeon_id);
		return retval;
	}
	ddoq_list = octeon_dev->ddoq_list;
	if(ddoq_list == NULL) {
		cavium_error("OCTEON_CNTQ: No ddoq list for octeon device %d\n",
		              octeon_id);
		return retval;
	}
	cavium_spin_lock_softirqsave(&ddoq_list->lock);
	retval = ddoq_list->stats.active_ddoqs;
	cavium_spin_unlock_softirqrestore(&ddoq_list->lock);
	return retval;
}





int
octeon_ddoq_list_get_stats(int                     octeon_id,
                           oct_ddoq_list_stats_t  *ddoq_list_stats)
{
	octeon_device_t   *octeon_dev = get_octeon_device_ptr(octeon_id);

	if(octeon_dev == NULL) {
		cavium_error("OCTEON_CNTQ: %s: No octeon device\n", __CVM_FUNCTION__);
		return ENODEV;
	}
	cavium_memcpy(ddoq_list_stats, &(((octeon_ddoq_list_t *)octeon_dev->ddoq_list)->stats), sizeof(oct_ddoq_list_stats_t));
	return 0;
}






void
oct_ddoq_add_to_deferred_list(octeon_device_t  *oct, octeon_ddoq_t   *ddoq)
{
	octeon_ddoq_list_t  *dlist = oct->ddoq_list;

//	printk("%s Adding DDOQ[%d] to deferred list dlist @ %p\n",
//	        __FUNCTION__, ddoq->ddoq_id, dlist);

	cavium_spin_lock_softirqsave(&dlist->lock);

	cavium_list_add_tail(&ddoq->list, &dlist->deferred);
	dlist->def_count++;

	cavium_spin_unlock_softirqrestore(&dlist->lock);
}





extern void
octeon_ddoq_free_resources(octeon_device_t *,  octeon_ddoq_t  *);



oct_poll_fn_status_t 
ddoq_list_poll_fn(void   *octeon_dev, unsigned long  arg)
{
	octeon_device_t     *oct = (octeon_device_t  *)octeon_dev;
	octeon_ddoq_list_t  *dlist = oct->ddoq_list;
	cavium_list_t        list, *tmp, *tmp2;
	int                  cnt = 0;

	CAVIUM_INIT_LIST_HEAD(&list);


	cavium_spin_lock_softirqsave(&dlist->lock);

	cavium_list_for_each_safe(tmp, tmp2, &dlist->deferred) {
		octeon_ddoq_t  *ddoq = (octeon_ddoq_t *)tmp;

		if(ddoq->state == DDOQ_STATE_NULL) {
			cavium_list_del(tmp);
			cavium_list_add_tail(tmp, &list);
			cnt++;
			dlist->def_count--;
		}
	}

	cavium_spin_unlock_softirqrestore(&dlist->lock);

	if(cnt == 0)
		return OCT_POLL_FN_CONTINUE;


	tmp = NULL;
	cavium_list_for_each_safe(tmp, tmp2, &list) {
		octeon_ddoq_t  *ddoq = (octeon_ddoq_t *)tmp;
        cavium_list_del(tmp); 
		release_ddoq_entry(oct, ddoq->ddoq_id);
		ddoq_list_update_count(oct, DDOQ_DELETE_OP);
		ddoq_list_update_count(oct, DDOQ_EOT_OP );
		octeon_ddoq_free_resources(oct, ddoq);
		cnt--;
	}

	if(cnt) {
		cavium_error("%s: deferred count is %d at end of processing\n",
		             __CVM_FUNCTION__, cnt);
	}

	return OCT_POLL_FN_CONTINUE;
}







/* $Id: octeon_ddoq_list.c 53785 2010-10-08 22:08:08Z panicker $ */

