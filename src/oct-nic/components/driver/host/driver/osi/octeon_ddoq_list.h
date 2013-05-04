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

/*! \file octeon_ddoq_list.h
    \brief Host: DDOQ  Lookup table implementation
*/


#ifndef  __OCTEON_DDOQ_LIST_H__
#define  __OCTEON_DDOQ_LIST_H__

#include "octeon_main.h"
#include "octeon_device.h"
#include "cavium_sysdep.h"
#include "octeon_cntq_defs.h"
#include "octeon-cntq-common.h"
#include "octeon_stats.h"
#include "cavium-list.h"


#define MAX_DDOQ   (1 << 22)

#define LEVEL3_BITS  10
#define LEVEL2_BITS  8
#define LEVEL1_BITS  4

#define LEVEL3_MASK  0x0003ff
#define LEVEL2_MASK  0x03fc00
#define LEVEL1_MASK  0x3c0000


#define DDOQ_ID_OFFSET     36

#define DDOQ_FAIL_CODE                       0x2022
#define DDOQ_DELETE_B4_CREATE_CODE           0x2023

#define   MAX_DEFERRED_IDS       256



/** Entry in the ddoq list structure. The list maintains a free list
  * of ddoq indexes and a list of pointers to currently active ddoq's.
  */
typedef struct  {

   /** The next free index that can be assigned to a DDOQ. */
   uint32_t  index;

   /** Pointer to a Active DDOQ. */
   void    *ddoq_ptr;

} ddoq_entry_t;

#define  OCT_DDOQ_ENTRY_SIZE      (sizeof(ddoq_entry_t))



/** The Octeon DDOQ list structure. */

typedef struct {

    /** Lock for the list. */
    cavium_spinlock_t        lock;

    /** The next index where a new DDOQ entry can be added. */
    uint16_t                 next_free_index;

    /** Pointer to the level 1 lookup table. */
    ddoq_entry_t          ***level1;

    /** The total number of DDOQ's that can be managed with the
        current size of the list. */
    uint32_t                 ddoq_allocated;

    /** The number of level 2 tables created and pointed to by the
        level 1 table. */
    uint16_t                 l2_count;

    /** An array which keeps track of the level 3 pointers in each
        level 2 table. */
    uint16_t                *l3_count;

	/** Number of deferred ddoq delete operations. */
	uint16_t                 def_count;

	/** List of ddoqs be to be deleted. */
	cavium_list_t            deferred;

    /** If set indicates that the ddoq list is being deleted. */
    int                      cleanup_active;

    /** Statistics for the ddoq list. */
    oct_ddoq_list_stats_t    stats;

    /** A list into which id's of freed ddoq's are kept temporarily. */
    uint32_t                 free_ids[MAX_DEFERRED_IDS];
    uint32_t                 free_id_count;

} octeon_ddoq_list_t;








/** Returns the ddoq pointer when the ddoq id is given. If the ddoq is
  * not active state (its undergoing delete), NULL is returned.
  * @param ddoq_list - the octeon driver ddoq list 
  * @param ddoq_id   - fetch the ddoq ptr for this id
  */
static inline void  *
get_ddoq_ptr(octeon_device_t    *octeon_dev,
             uint32_t            ddoq_id)
{
	octeon_ddoq_list_t      *ddoq_list = (octeon_ddoq_list_t *)octeon_dev->ddoq_list;
	register ddoq_entry_t   **list2, *list3;
	uint32_t                  idx, idx_level1, idx_level2, idx_level3;
	void                    *ddoq=NULL;

	idx        = ddoq_id - DDOQ_ID_OFFSET;
	idx_level3 = (idx & LEVEL3_MASK);
	idx_level2 = ((idx & LEVEL2_MASK) >> LEVEL3_BITS);
	idx_level1 = ((idx & LEVEL1_MASK) >> (LEVEL3_BITS + LEVEL2_BITS));
	list2 = ddoq_list->level1[idx_level1];
	if(list2) {
		list3 = list2[idx_level2];
		if(list3) {
			ddoq = list3[idx_level3].ddoq_ptr;
		}
	}
	return ddoq;
}



int   octeon_init_ddoq_list(int  octeon_id);

int   octeon_delete_ddoq_list(int octeon_id);

void  octeon_cleanup_active_ddoqs(int octeon_id);

int   release_ddoq_entry(octeon_device_t  *oct, uint32_t  ddoq_id);

uint32_t  get_next_idx(octeon_ddoq_list_t  *ddoq_list, uint32_t  *ddoq_id, void ***ddoq_ptr);

void  ddoq_list_update_count(octeon_device_t  *octeon_dev, uint32_t  action);

uint64_t  octeon_get_active_ddoq_count(int   octeon_id);

int   octeon_ddoq_list_get_stats(int  octeon_id, oct_ddoq_list_stats_t  *ddoq_list_stats);




#endif

/* $Id: octeon_ddoq_list.h 42868 2009-05-19 23:57:52Z panicker $ */

