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

#include "cvm-drv.h"
#include "cvm-cntq.h"
#include "cvm-ddoq-list.h"
#include "cvm-ddoq.h"
#include "octeon-error.h"
#include <errno.h>
#include "cvm-driver-defs.h"
#include "cvm-drv-debug.h"



CVMX_SHARED   cvm_ddoq_list_t            cvm_ddoq_list;


void
cvm_drv_print_ddoq_list(void)
{
   void       ***dlist2,  **dlist3;
   uint16_t    i, j, k;

   DBG_PRINT(DBG_FLOW,"\n\n-----The DDOQ List-------\n");
   for( i = 0; i < cvm_ddoq_list.l2_count; i++)  {
      dlist2 = cvm_ddoq_list.level1[i];
      for( j = 0; j < cvm_ddoq_list.l3_count[i]; j++)  {
         dlist3 = dlist2[j];
         for( k = 0; k < (1 << LEVEL3_BITS); k++)  {
             if(dlist3[k])
               DBG_PRINT(DBG_FLOW,"DDOQ present at %d:%d:%d\n",i, j, k);
         }
      }
   }
}








/* Allocation of memory for the DDOQ lookup table.
 * Current executive requires that arena allocation and thread-safe 
 * allocation routines are provided by the application. 
 */
int
cvm_init_ddoq_list()
{
   void ***dlist2, **dlist3;

   /* The One and only Level 1  table */
   cvm_ddoq_list.level1 = (void ****)cvm_ddoq_get_buffer(PTR_SIZE * (1 << LEVEL1_BITS));
   if(!cvm_ddoq_list.level1) {
       printf("[ DRV ] DDOQ level 1 list allocation failed\n");
       return 1;
   }


   /* The first Level 2 table. There can be (2 ^ LEVEL1_BITS) Level 2 tables */
   cvm_ddoq_list.level1[0] = (void ***) cvm_ddoq_get_buffer(PTR_SIZE * (1 << LEVEL2_BITS));
   if(!cvm_ddoq_list.level1[0]) {
       printf("[ DRV ] DDOQ level 2 list allocation failed\n");
       return 1;
   }

   dlist2  = cvm_ddoq_list.level1[0];
   cvm_ddoq_list.l2_count  = 1;


   /* Array with one element for each l2 table to keep track
      of their Level3 table allocations. For each
      Level2 table there can be (2 ^ LEVEL2_BITS) Level3 tables.*/
   cvm_ddoq_list.l3_count = cvm_ddoq_get_buffer(sizeof(uint16_t)*(1 << LEVEL1_BITS));
   if(!cvm_ddoq_list.l3_count) {
       printf("[ DRV ] DDOQ level 3 count array allocation failed\n");
       return 1;
   }

   /* The first Level3 table created for the Level2 table above. Each
      Level3 table points to (2 ^ LEVEL3_BITS) ddoq's.  */
   dlist2[0] = cvm_ddoq_get_buffer(PTR_SIZE * (1 << LEVEL3_BITS));
   dlist3 = dlist2[0];
   if(!dlist3) {
       printf("[ DRV ] DDOQ level 3 list allocation failed\n");
       return 1;
   }

   cvm_ddoq_list.l3_count[0] = 1;
   memset(dlist3, 0, PTR_SIZE * (1 << LEVEL3_BITS) );

   cvmx_spinlock_init(&cvm_ddoq_list.lock);

#ifdef CVM_DDOQ_PROFILE
   cvm_drv_init_ddoq_profile();
#endif
   CVMX_SYNCW;
   return 0;
}






/* Add a DDOQ structure to the ddoq lookup table. */
int
cvm_add_to_ddoq_list(cvm_ddoq_t   *ddoq)
{
  void       ***dlist2,  **dlist3;
  uint16_t   l1, l2, l3;
  uint32_t   ddoq_id;

  ddoq_id  = ddoq->ddoq_id - DDOQ_ID_OFFSET;

  DBG_PRINT(DBG_FLOW, "----cvm_add_to_ddoq_list----\n");

  l3 = (ddoq_id & LEVEL3_MASK);
  l2 = ((ddoq_id & LEVEL2_MASK) >> LEVEL3_BITS);
  l1 = ((ddoq_id & LEVEL1_MASK) >> (LEVEL3_BITS + LEVEL2_BITS));


  cvmx_spinlock_lock(&cvm_ddoq_list.lock);
  dlist2 = cvm_ddoq_list.level1[l1];
  if(!dlist2) {
     cvm_ddoq_list.level1[l1] = cvm_ddoq_get_buffer(PTR_SIZE * (1 << LEVEL2_BITS));
     if(!cvm_ddoq_list.level1[l1])  {
         cvmx_spinlock_unlock(&cvm_ddoq_list.lock);
         printf("[ DRV ] Allocation of level 2 ddoq list failed\n");
         return 1;
     }
     cvm_ddoq_list.l2_count++;
     dlist2 = cvm_ddoq_list.level1[l1];
  }

  dlist3 = dlist2[l2];
  if(!dlist3) {
     dlist2[l2] = cvm_ddoq_get_buffer(PTR_SIZE * (1 << LEVEL3_BITS));
     if(!dlist2[l2])  {
        cvmx_spinlock_unlock(&cvm_ddoq_list.lock);
        printf("[ DRV ] Allocation of level 3 ddoq list failed\n");
        return 1;
     }
     cvm_ddoq_list.l3_count[l1]++;
     dlist3 = dlist2[l2];
     memset(dlist3, 0, PTR_SIZE * (1 << LEVEL3_BITS));
  }

  if(dlist3[l3]) {
      /* There could be a potential race if the ddoq id released after the
         last delete is reused immediately by the host for a new DDOQ (with a
         new tag, thereby not protected by ATOMIC tag). Since DDOQ delete and
         ADD are NORESPONSE, the processing for delete may be concurrent with
         the ADD leading to potential race. We alleviate it here by giving the
         ADD some more time.
      */
      int recheck = 1000;
      while(dlist3[l3] && recheck--) {
         cvmx_wait(10000);
         CVMX_SYNCW;
      }
      if(dlist3[l3]) {
          printf("[ DRV ] DDOQ: ddoq exists at %d:%d:%d\n", l1, l2, l3);
          cvmx_spinlock_unlock(&cvm_ddoq_list.lock);
          return 1;
      }
  }

  dlist3[l3] = ddoq;

  cvmx_spinlock_unlock(&cvm_ddoq_list.lock);

  DBG_PRINT(DBG_FLOW, "  Added ddoq[%d] with addr: 0x%016lx\n",
            ddoq->ddoq_id, (uint64_t)ddoq);
  CVMX_SYNCW;
  return 0;
}







/* Remove a DDOQ structure from the DDOQ lookup table. */
void
cvm_delete_from_ddoq_list(cvm_ddoq_t   *ddoq)
{
  void       ***dlist2,  **dlist3;
  uint16_t   l1, l2, l3;
  uint32_t   ddoq_id  = ddoq->ddoq_id - DDOQ_ID_OFFSET;
  cavium_list_t  *tmp, *tmp2;

  DBG_PRINT(DBG_FLOW, "----cvm_delete_from_ddoq_list----\n");
  DBG_PRINT(DBG_FLOW, "ddoq Addr: 0x%p  id: %d\n",
            ddoq, ddoq->ddoq_id);

  /* First remove the DDOQ entry in the DDOQ list. */
  l3 = (ddoq_id & LEVEL3_MASK);
  l2 = ((ddoq_id & LEVEL2_MASK) >> LEVEL3_BITS);
  l1 = ((ddoq_id & LEVEL1_MASK) >> (LEVEL3_BITS + LEVEL2_BITS));

  dlist2 = cvm_ddoq_list.level1[l1];
  dlist3 = dlist2[l2];

  cvmx_spinlock_lock(&cvm_ddoq_list.lock);

  /* The DELETE instruction should have a ATOMIC tag. So this entry
     in the level3 table will never be accessed at the same time as
     a packet or another instruction for the same DDOQ. */
  dlist3[l3] = NULL;

  cvmx_spinlock_unlock(&cvm_ddoq_list.lock);

  if(ddoq->wait_count) {
      DBG_PRINT(DBG_WARN,"[ DRV ] DDOQ: Wait listed packets found during ddoq[%d] delete\n", ddoq->ddoq_id);
      cavium_list_for_each_safe(tmp, tmp2, &(ddoq->wait_list)) {
          cavium_list_del(tmp);
          cvm_release_ddoq_packet((cvm_ddoq_pkt_t *)tmp);
      }
  }

}


/* $Id: cvm-ddoq-list.c 42868 2009-05-19 23:57:52Z panicker $ */


