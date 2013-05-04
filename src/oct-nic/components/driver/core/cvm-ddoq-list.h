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

/*! \file cvm-ddoq-list.h
    \brief Core driver: DDOQ Lookup table implementation
*/

#ifndef  __CVM_DDOQ_LIST_H__
#define  __CVM_DDOQ_LIST_H__

#include "cvm-ddoq.h"



/* Maximum number of DDOQ's that the driver supports */
#define MAX_DDOQ   (1 << 22)


/* Bit manipulation for the 3-level DDOQ lookup table. */
#define LEVEL3_BITS  10
#define LEVEL2_BITS  8
#define LEVEL1_BITS  4

#define LEVEL3_MASK  0x0003ff
#define LEVEL2_MASK  0x03fc00
#define LEVEL1_MASK  0x3c0000


/* id from 0 to 35 are reserved  */
#define DDOQ_ID_OFFSET     36



/** The DDOQ list.
  * This list provides a lookup table to get the ddoq structure when the
  * ddoq id is known. The lookup table is a three level heirarchy with the
  * third level nodes pointing to DDOQ's in the system. The level 1 table 
  * points to level 2 tables which in turn points to level 3 table.
  * The 22 bits of ddoq id is divided to form indexes into 3 tables to
  * reach to the DDOQ. The number of bits used to index into each table is
  * given by the LEVEL[1-3]_BITS macro. At start time the level 1 table
  * is fully allocated and 1 level 2 and level 3 table exist. More level2 
  * and level3 tables are added when more ddoq's are created.
  */
typedef struct {

     /** Lock to synchronize access to this list. */
     cvmx_spinlock_t       lock;

     /** Pointer to the level 1 table. */
     void              ****level1;

     /** Number of level 2 table currently present. */
     uint16_t              l2_count;

     /** Number of level 3 tables pointers in each level2 table. */
     uint16_t             *l3_count;

} cvm_ddoq_list_t;



extern CVMX_SHARED   cvm_ddoq_list_t    cvm_ddoq_list;

/*
   Get the ddoq pointer given the ddoq id.
   This routine checks that the 2nd and 3rd level tables are allocated
   for the required lookup just to make sure if its given an as-yet unallocated
   ddoq id, it wont cause a memory access violation
*/
static inline  cvm_ddoq_t *
cvm_get_ddoq_ptr(uint32_t  ddoq_id)
{
  void      ***dlist2, **dlist3;
  uint16_t  l1, l2, l3;

  ddoq_id -=  DDOQ_ID_OFFSET;

  l3 = (ddoq_id & LEVEL3_MASK);
  l2 = ((ddoq_id & LEVEL2_MASK) >> LEVEL3_BITS);
  l1 = ((ddoq_id & LEVEL1_MASK) >> (LEVEL3_BITS + LEVEL2_BITS));

  dlist2 = cvm_ddoq_list.level1[l1];
  if(dlist2) {
      dlist3 = dlist2[l2];
      if(dlist3)  {
         return dlist3[l3];
      }
  }
  return NULL;
}





static inline cvm_ddoq_t*
test_get_ddoq_ptr(uint32_t ddoq_id){
      return cvm_get_ddoq_ptr(ddoq_id);
}





void
cvm_drv_print_ddoq_list(void);


int
cvm_init_ddoq_list(void);


void
cvm_delete_from_ddoq_list(cvm_ddoq_t   *ddoq);


int
cvm_add_to_ddoq_list(cvm_ddoq_t   *ddoq);



#endif


/* $Id: cvm-ddoq-list.h 42868 2009-05-19 23:57:52Z panicker $ */

