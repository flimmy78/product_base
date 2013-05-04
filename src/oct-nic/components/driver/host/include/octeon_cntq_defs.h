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


/*! \file  octeon_cntq_defs.h 
    \brief CNTQ: API exported to kernel applications to create/delete and
           manage DDOQ's.
*/



#ifndef __OCTEON_CNTQ_DEFS_H__
#define __OCTEON_CNTQ_DEFS_H__

#include    "cavium_defs.h"
#include    "cavium_kernel_defs.h"

/* Applications allocate buffers for DDOQ via this callback.
   Parameters: ddoq_id; buffer size;
*/
typedef  void* (*ddoq_buf_alloc_fn_t)(uint32_t, uint32_t);


/* Applications free up buffers from DDOQ via this callback.
   Parameters: ddoq_id; address of buffer;
*/
typedef  void (*ddoq_buf_dealloc_fn_t)(uint32_t, void *);



/* Applications receive confirmation of DDOQ deletion via this callback.
   Parameters: ddoq_id; status;
*/
typedef  void (*ddoq_delete_conf_fn_t)(uint32_t, uint32_t);




/** Setup for a DDOQ is presented to the driver in this structure.
  */
typedef struct  {

	/** No. of descriptors to be used for this DDOQ */
	uint32_t                    no_of_desc;

	/** The dispatch function to be called with the packets
	   received on this DDOQ - This field is required.  */
	octeon_dispatch_fn_t      dispatch_fn;

	/** Pointer to user-defined structure */
	void                      *fn_arg;

	/** The function to be called for allocation of buffers
	   for the descriptor ring _ This field can be NULL. */
	ddoq_buf_alloc_fn_t       buf_alloc_fn;

	/** The size of each buffer in the ring. This field can
	   be NULL, ONLY if buf_alloc_fn is NULL.   */
	uint32_t                     buffer_size;

	/** The function to be called when the DDOQ is torn down.
	   This field can be NULL ONLY if buf_alloc_fn is NULL. */
	ddoq_buf_dealloc_fn_t      buf_dealloc_fn;

	/** The function to call on delete confirmation.
	   This field is required.    */
	ddoq_delete_conf_fn_t      delete_conf_fn;


	/** Count of descriptors for which buffers will be allocated 
	    at DDOQ create time. */
	uint32_t                    initial_fill;

	/** If the buf_alloc_fn ptr is defined, it will be called
	   everytime there are as many desc without buffers
	   as defined by this field.  */
	uint32_t                     refill_threshold;

	/** Tag value to use for this DDOQ. */
	uint32_t                     tag;

	/** Type of DDOQ */
	uint32_t                     type;

	/** User defined data for this DDOQ. */
	void                        *ddoq_udd;

	/** Size of the user-defined data. */
	uint32_t                     ddoq_udd_size;

}  octeon_ddoq_setup_t;

#define OCT_DDOQ_SETUP_SIZE       (sizeof(octeon_ddoq_setup_t))


/** Called when a new DDOQ needs to be created. This routine
  * assigns a new id to this ddoq, adds the ddoq to its ddoq
  * list, allocates the descriptor ring space and calls the user
  * provided buffer allocation routine to allocate buffers for
  * the ring. It sends a CREATE instruction to the core driver
  * and if the instruction returns a success, returns the ddoq
  * id to the caller. If the local structures could not be created
  * or if the CREATE instruction fails, -1 is returned.
  * @param octeon_id: the octeon device in which to create the ddoq.
  * @param ddoq_op:   the parameters used to configure the ddoq.
  *                   includes no. of descriptors, buffer size,
  *                   refill threshold.
  * @return       :   ddoq_id (36 <= ddoq_id <= 2^22-1) if ddoq
  *                   was created successfully; else -1.
  */
int
octeon_ddoq_create(uint32_t               octeon_id,
                   octeon_ddoq_setup_t   *ddoq_op);



/**  Called when a ddoq is to be deleted. This routine sends a DELETE
  *  instruction to the core driver. If the core is successful in
  *  deleting its version of the DDOQ, it acknowledges the instruction
  *  with a REQUEST DONE status at which time, this routine deletes the
  *  host copy of the ddoq and calls the ddoq delete confirmation callback.
  *  If the core returns a failure, the host copy is not deleted and the
  *  status returned by the core driver is returned to the caller.
  *  @param octeon_id: octeon device on which the ddoq is to deleted.
  *  @param ddoq_id  : id of the ddoq to be deleted.
  *  @param udd      : Pointer to additional user data to send to core
  *  @param udd_size : Size of user data.
  *  @return:          0 if the DDOQ was successfully deleted; else 1.
  */
uint32_t
octeon_ddoq_delete(uint32_t    octeon_id,
                   uint32_t    ddoq_id,
                   void       *udd,
                   uint32_t    udd_size);



/**  Restart the DDOQ packet processing. This routine is called by an
  *  application which receives packet from this DDOQ to restart
  *  packet processing. The application may have earlier stopped DDOQ
  *  from dispatching packets by returning a non-zero value after the
  *  last dispatch function call.
  *  @param octeon_id: octeon device on which the ddoq is to be restarted.
  *  @param ddoq_id  : id of the ddoq to be restarted.
  *  @return : ENODEV id octeon_id or ddoq_id was incorrect. EBUSY if the
  *            ddoq was in an incorrect state. 0 on success.
  */
int
octeon_ddoq_restart_rx(uint32_t   octeon_id,
                       uint32_t   ddoq_id);




int
octeon_init_ddoq_list(int  octeon_id);
                                                                                                                                       
                                                                                                                                       
                                                                                                                                       
int
octeon_delete_ddoq_list(int octeon_id);
                                                                                                                                       

void
octeon_cleanup_active_ddoqs(int octeon_id);

#ifdef USE_DDOQ_THREADS
void octeon_ddoq_stop_threads(void *oct_dev);
#endif

int
octeon_ddoq_change_tag(uint32_t   octeon_id,
                       uint32_t   ddoq_id,
                       uint32_t   tag);

                                                                                                                                       
int
octeon_enable_cntq_ops(int   octeon_id, octeon_dma_ops_t *);
                                                                                                                                       
                                                                                                                                       
int
octeon_disable_cntq_ops(int   octeon_id);
                                                                                                                                       

int
octeon_cntq_get_stats(int octeon_id, int cntq_no, oct_cntq_stats_t  *cntq_stats);

int
octeon_ddoq_get_stats(int octeon_id, int ddoq_id, oct_ddoq_stats_t  *ddoq_stats);

int
octeon_ddoq_list_get_stats(int octeon_id, oct_ddoq_list_stats_t  *ddoq_list_stats);

uint64_t octeon_get_active_ddoq_count(int   octeon_id);


int octeon_change_cntq_status(int  octeon_id, int status);



                                                                                                                                       
/** Routine to initialize the host structure for Octeon DMA counter queue
  * operation. This routine allocates memory for the cntq ring and sets
  * the cntq parameters to their default starting values.
  *
  * @param octeon_id   - Octeon device identifier
  * @param cntq_conf   - Configuration parameters for the CNTQ.
  * @param cntq_no     - the CNTQ to initialize (0 <= cntq_no <= 1)
  * @return Returns 0 if the CNTQ was initialized successfully, 1 otherwise.
  */
int
octeon_init_cntq(int  octeon_id, int cntq_no, void  *conf);
                                                                                                                                       
                                                                                                                                       
                                                                                                                                       
/**  Routine to delete the host structures for a Octeon DMA counter queue.
  *  Called at driver unload time to free resources used for the DMQ queues.
  *  @param  octeon_id   -  Octeon device identifier.
  *  @param  cntq_no     -  the Counter queue (out of 2) that should be freed.
  *  @return  Returns 0 if the cntq_no is valid and all resources are
  *           successfully freed.
  */
int
octeon_delete_cntq(int  octeon_id, int cntq_no);


#endif

/* $Id: octeon_cntq_defs.h 67088 2011-11-15 19:39:17Z mchalla $ */
