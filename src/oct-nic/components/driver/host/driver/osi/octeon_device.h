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



/*! \file octeon_device.h
    \brief Host Driver: This file defines the octeon device structure.
*/


#ifndef  _OCTEON_DEVICE_H_
#define  _OCTEON_DEVICE_H_


#include "cavium_sysdep.h"
#include "octeon_config.h"

typedef struct _OCTEON_DEVICE octeon_device_t;


#ifdef USE_BUFFER_POOL
#include "buffer_pool.h"
#endif


#include "octeon_stats.h"
#include "octeon_iq.h"
#include "octeon_instr.h"
#include "octeon_droq.h"
#include "pending_list.h"
#include "response_manager.h"

#include "octeon_debug.h"
#include "octeon-common.h"

#include "cn3xxx_device.h"
#include "cn56xx_device.h"
#include "cn63xx_device.h"
#include "cn66xx_device.h"
#include "cn68xx_device.h"




#define PCI_DMA_64BIT                  0xffffffffffffffffULL

#define CAVIUM_PCI_CACHE_LINE_SIZE     2


/** PCI VendorId Device Id */
#define  OCTEON_CN38XX_PASS2_PCIID    0x4177d
#define  OCTEON_CN38XX_PASS3_PCIID    0x5177d
#define  OCTEON_CN58XX_PCIID          0x40177d
#define  OCTEON_CN56XX_PCIID          0x50177d
#define  OCTEON_CN63XX_PCIID          0x90177d
#define  OCTEON_CN68XX_PCIID          0x91177d
#define  OCTEON_CN66XX_PCIID          0x92177d


/** Driver identifies chips by these Ids, created by clubbing together
    DeviceId+RevisionId; Where Revision Id is not used to distinguish
    between chips, a value of 0 is used for revision id.
*/
#define  OCTEON_CN38XX_PASS2          0x0004
#define  OCTEON_CN38XX_PASS3          0x0005

#define  OCTEON_CN58XX                0x0040
#define  OCTEON_CN56XX                0x0050
#define  OCTEON_CN63XX                0x0090
#define  OCTEON_CN68XX                0x0091
#define  OCTEON_CN66XX                0x0092




/** Endian-swap modes supported by Octeon. */
enum octeon_pci_swap_mode {
    OCTEON_PCI_PASSTHROUGH     = 0,
    OCTEON_PCI_64BIT_SWAP      = 1,
    OCTEON_PCI_32BIT_BYTE_SWAP = 2,
    OCTEON_PCI_32BIT_LW_SWAP   = 3
} ;







/** Octeon Device state.
 *  Each octeon device goes through each of these states 
 *  as it is initialized.
 */
#define    OCT_DEV_BEGIN_STATE            0x0
#define    OCT_DEV_PCI_MAP_DONE           0x1
#define    OCT_DEV_DISPATCH_INIT_DONE     0x2
#ifdef USE_BUFFER_POOL
#define    OCT_DEV_BUF_POOL_INIT_DONE     0x3
#endif
#define    OCT_DEV_INSTR_QUEUE_INIT_DONE  0x4
#define    OCT_DEV_PEND_LIST_INIT_DONE    0x5
#define    OCT_DEV_RESP_LIST_INIT_DONE    0x6
#define    OCT_DEV_DROQ_INIT_DONE         0x7
#define    OCT_DEV_HOST_OK                0x8
#define    OCT_DEV_CORE_OK                0x9
#define    OCT_DEV_RUNNING                0xA
#define    OCT_DEV_IN_RESET               0xB
#define    OCT_DEV_RESET_CLEANUP_DONE     0xC
#define    OCT_DEV_STOPPING               0xD
#define    OCT_DEV_STOPPED                0xE
#define    OCT_DEV_STATE_INVALID          0xF

#define    OCT_DEV_STATES                 OCT_DEV_STATE_INVALID





#define OCTEON_MAX_MODULES 3
enum {
	OCTEON_START_MODULE=1,
	OCTEON_RESET_MODULE=2,
	OCTEON_STOP_MODULE
};




/*---------------------------DISPATCH LIST-------------------------------*/


/** The dispatch list entry.
 *  The driver keeps a record of functions registered for each 
 *  response header opcode in this structure. Since the opcode is
 *  hashed to index into the driver's list, more than one opcode
 *  can hash to the same entry, in which case the list field points
 *  to a linked list with the other entries.
 */
typedef struct {

  /** List head for this entry */
    cavium_list_t             list;

  /** The opcode for which the above dispatch function & arg should be
      used */
    octeon_opcode_t           opcode;

  /** The function to be called for a packet received by the driver */
    octeon_dispatch_fn_t      dispatch_fn;

  /** The application specified argument to be passed to the above
      function along with the received packet */
    void                      *arg;

} octeon_dispatch_t;



/** The dispatch list structure. */
typedef struct {

	cavium_spinlock_t      lock;

	/** Count of dispatch functions currently registered */
	uint32_t               count;

	/** The list of dispatch functions */
	octeon_dispatch_t     *dlist;


} octeon_dispatch_list_t;








/*-----------------------  THE OCTEON DEVICE  ---------------------------*/



#define OCT_MEM_REGIONS     3
/** PCI address space mapping information.
 *  Each of the 3 address spaces given by BAR0, BAR2 and BAR4 of
 *  Octeon gets mapped to different physical address spaces in
 *  the kernel. 
 */
typedef struct  {

  /** PCI address to which the BAR is mapped. */
   unsigned long   start;

  /** Length of this PCI address space. */
   unsigned long   len;

  /** Length that has been mapped to phys. address space. */
   unsigned long   mapped_len;

  /** The physical address to which the PCI address space is mapped. */
   void           *hw_addr;

  /** Flag indicating the mapping was successful. */
   int             done;

}octeon_mmio;


#define   MAX_OCTEON_MAPS    32


/** Map of Octeon core memory address to Octeon BAR1 indexed space. */
typedef  struct  {

  /** Starting Core address mapped */
  uint64_t      core_addr;

  /** Physical address (of the BAR1 mapped space) 
      corressponding to core_addr. */
  void       *mapped_addr;

  /** Indicator that the mapping is valid. */
  int         valid;

} octeon_range_table_t;




/* \cond */

typedef struct {

	uint32_t    iq;

	uint32_t    oq;

	uint32_t    iq64B;

}  octeon_io_enable_t;




/* \endcond */



struct octeon_reg_list {

	uint32_t            *pci_win_wr_addr_hi;
	uint32_t            *pci_win_wr_addr_lo;
	uint64_t            *pci_win_wr_addr;

	uint32_t            *pci_win_rd_addr_hi;
	uint32_t            *pci_win_rd_addr_lo;
	uint64_t            *pci_win_rd_addr;

	uint32_t            *pci_win_wr_data_hi;
	uint32_t            *pci_win_wr_data_lo;
	uint64_t            *pci_win_wr_data;

	uint32_t            *pci_win_rd_data_hi;
	uint32_t            *pci_win_rd_data_lo;
	uint64_t            *pci_win_rd_data;
};



struct octeon_fn_list {

	void                (* setup_iq_regs)(struct _OCTEON_DEVICE *, int);
	void                (* setup_oq_regs)(struct _OCTEON_DEVICE *, int);

	cvm_intr_return_t   (* interrupt_handler)(void *);
	int                 (* soft_reset)(struct _OCTEON_DEVICE *);
	int                 (* setup_device_regs)(struct _OCTEON_DEVICE *);
	void                (* reinit_regs)(struct _OCTEON_DEVICE *);
	void                (* bar1_idx_setup)(struct _OCTEON_DEVICE *, uint64_t, int, int);
	void                (* bar1_idx_write)(struct _OCTEON_DEVICE *, int, uint32_t);
	uint32_t            (* bar1_idx_read)(struct _OCTEON_DEVICE *, int);
	uint32_t            (* update_iq_read_idx)(octeon_instr_queue_t  *);

	void                (* enable_oq_pkt_time_intr)(octeon_device_t *, int );
	void                (* disable_oq_pkt_time_intr)(octeon_device_t *, int );

	void                (* enable_interrupt)(void  *);
	void                (* disable_interrupt)(void  *);

	void                (* enable_io_queues)(struct _OCTEON_DEVICE  *);
	void                (* disable_io_queues)(struct _OCTEON_DEVICE  *);
};







#ifdef USE_DDOQ_THREADS

#define CVM_MAX_DDOQ_THREADS		8 //16
#define CVM_DDOQ_MAX_THREAD_PKTS	(2 * MAX_OCTEON_DMA_QUEUES * 256)//CNTQ_PKTS_PER_INTERRUPT];
//#define CVM_DDOQ_MAX_THREAD_PKTS	(2 * MAX_OCTEON_DMA_QUEUES * 512)

typedef struct cvm_ddoq_thread_info {
	int ddoq_id;
	int req_id;
	int num_pkts;
} octeon_ddoq_thread_info_t;

typedef struct cvm_ddoq_thread {
   octeon_device_t	     *oct_dev;
   cvm_kthread_t	     thread;
   cavium_wait_channel	 wc;
   int 			     stop_thread;
   cavium_atomic_t	     thread_active;
   cavium_atomic_t      ddoq_pkts_queued;

   cavium_spinlock_t    th_lock;
   int                  th_read_idx;
   int                  th_write_idx;
   
   /* On each interrupt we can handle at most */
   octeon_ddoq_thread_info_t th_info[CVM_DDOQ_MAX_THREAD_PKTS];
} cvm_ddoq_thread_t;
#endif


/** The Octeon device. 
 *  Each Octeon device has this structure to represent all its
 *  components.
 */
struct _OCTEON_DEVICE {

   /** Lock for this Octeon device */
   cavium_spinlock_t        oct_lock;

   /** OS dependent PCI device pointer */
   cavium_pci_device_t     *pci_dev;

   /** Chip specific information. */
   void                    *chip;

   /** Octeon Chip type. */
   uint16_t                 chip_id;
   uint16_t                 rev_id;

   /** This device's id - set by the driver. */
   uint16_t                 octeon_id;

   /** This device's PCIe port used for traffic. */
   uint16_t                 pcie_port;


   /** The state of this device */
   cavium_atomic_t          status;

   /** memory mapped io range */
   octeon_mmio              mmio[OCT_MEM_REGIONS];

   struct octeon_reg_list   reg_list;

   struct octeon_fn_list    fn_list;

   cavium_atomic_t          interrupts;

   cavium_atomic_t          in_interrupt;

   int                      num_iqs;

   /** The 4 input instruction queues */
   octeon_instr_queue_t     *instr_queue[MAX_OCTEON_INSTR_QUEUES];

   int                       pend_list_size;
   octeon_pending_list_t    *plist;

   /** The doubly-linked list of instruction response */
   octeon_response_list_t    response_list[MAX_RESPONSE_LISTS];


   int                        num_oqs;

   /** The 4 output queues  */
   octeon_droq_t            *droq[MAX_OCTEON_OUTPUT_QUEUES];

#if  !defined(USE_DROQ_THREADS)
   /** Tasklet structures for this device. */
   struct tasklet_struct     droq_tasklet;
#endif
   struct tasklet_struct     comp_tasklet;

   uint32_t                  napi_mask;

   void                     *poll_list;
   cavium_spinlock_t         poll_lock;

   struct tasklet_struct     cntq_tasklet;

   uint32_t                  cntq_ready;

   /* The 2 Octeon DMA Counter Queues */
   void                     *cntq[MAX_OCTEON_DMA_QUEUES];

   /* The DDOQ lookup table */
   void                     *ddoq_list;

   /** Operations on the DMA queues */
   octeon_dma_ops_t          dma_ops;


   /** A table maintaining maps of core-addr to BAR1 mapped address. */
   octeon_range_table_t      range_table[MAX_OCTEON_MAPS];

   /** Total number of core-address ranges mapped (Upto 32). */
   uint32_t                  map_count;


   octeon_io_enable_t        io_qmask;


   /** List of dispatch functions */
   octeon_dispatch_list_t    dispatch;

#ifdef USE_BUFFER_POOL
   /** The buffer pool implementation */
   cavium_buffer_t           buf[BUF_POOLS];
   cavium_frag_buf_t         fragments[MAX_BUFFER_CHUNKS];
   uint16_t                  fragment_free_list[MAX_BUFFER_CHUNKS];
   uint16_t                  fragment_free_list_index;
   cavium_spinlock_t         fragment_lock;
#endif

   /** The /proc file entries */
   void                     *proc_root_dir;

   /** Statistics for this octeon device. Does not include IQ, DROQ stats */
   oct_dev_stats_t           stats;

   /** IRQ assigned to this device. */
   int                       irq;

   int                       msi_on;

   /** The core application is running in this mode. See octeon-drv-opcodes.h
       for values. */
   int                       app_mode;
#ifdef CVMCS_DMA_IC
   /* When DMA interrupt raised we have these many packets DMAed by Octeon */
   cavium_atomic_t          dma_cnt_to_process;
#endif

#ifdef USE_DDOQ_THREADS
   cvm_ddoq_thread_t	     ddoq_thread[CVM_MAX_DDOQ_THREADS];
#endif

   /** The name given to this device. */
   char                      device_name[32];

};







#define CHIP_FIELD(oct, TYPE, field)             \
	(((octeon_##TYPE##_t *)(oct->chip))->field)



/*------------------ Function Prototypes ----------------------*/

/** Allocate memory for Input and Output queue structures for a octeon device */
octeon_device_t *   octeon_allocate_device_mem(int);


/** Free memory for Input and Output queue structures for a octeon device */
void octeon_free_device_mem(octeon_device_t *);

/** Look up a free entry in the octeon_device table and allocate resources
    for the octeon_device_t  structure for an octeon device. Called at init
    time. */
octeon_device_t  *
octeon_allocate_device(int pci_id);


/**  Initialize the driver's dispatch list which is a mix of a hash table
  *  and a linked list. This is done at driver load time.
  *  @param octeon_dev - pointer to the octeon device structure.
  *  @return 0 on success, else -ve error value
  */
int    octeon_init_dispatch_list(octeon_device_t  *octeon_dev);



/**  Delete the driver's dispatch list and all registered entries.
  *  This is done at driver unload time.
  *  @param octeon_dev - pointer to the octeon device structure.
  */
void    octeon_delete_dispatch_list(octeon_device_t  *octeon_dev);




/**  Register dispatch functions for packets from core that are of 
  *  interest to the driver.
  *  @param octeon_id - octeon device id.
  */
void    octeon_setup_driver_dispatches(uint32_t octeon_id);




/* Perform hot reset of Octeon device. The Octeon input & output queues
 * do not have to be re-allocated. The existing queue setup is reprogrammed
 * into the octeon device after this reset. */
int
octeon_hot_reset(octeon_device_t  *octeon_dev);



/** Get the octeon device pointer.
 *  @param octeon_id  - The id for which the octeon device pointer is required.
 *  @return Success: Octeon device pointer.
 *  @return Failure: NULL.
 */
octeon_device_t *  get_octeon_device(uint32_t octeon_id);



/** Gets the octeon device id when the device structure is given.
 *  @return - The octeon device id.
 */
uint32_t  get_octeon_id(octeon_device_t *octeon_dev);


/** Return the core clock cycles per microsecond. */
uint32_t
octeon_get_cycles_per_usec(octeon_device_t  *oct);


/** Called by the base device driver at init time to initialize the
  * module handler data structures.
  */
void   octeon_init_module_handler_list(void);

/** Called by the base driver (in the poll thread) for each octeon
  * device when its state changes to CORE_OK (core application 
  * informs the host of type of application running). This routine
  * checks for handlers for the application type and calls the
  * handler start routine.
  * @param app_type  - The application type detected for the octeon device.
  * @param octeon_id - The device id for the octeon device.
  * @return -ENODEV if no handler was found for the application type or an
  *         invalid octeon id was passed.
  *         -EINVAL if invalid operation was specified.
  *         0 on success.
  */
int    octeon_start_module(uint32_t app_type, uint32_t  octeon_id);




/** Called by the base driver for each octeon device when the device is being
  * reset (due to hot-reset initiated by user). This function calls the reset
  * routine of handlers for the application type specified.
  * @param app_type  - The application type detected for the octeon device.
  * @param octeon_id - The device id for the octeon device.
  * @return -ENODEV if no handler was found for the application type or an
  *         invalid octeon id was passed.
  *         -EINVAL if invalid operation was specified.
  *         0 on success.
  */
int    octeon_reset_module(uint32_t app_type, uint32_t  octeon_id);




/** Called by the base driver for each octeon device when the device is being
  * removed (either due to hot-plug or due to base module unload). This 
  * function calls the stop routine of handlers for the application type
  * specified.
  * @param app_type  - The application type detected for the octeon device.
  * @param octeon_id - The device id for the octeon device.
  * @return -ENODEV if no handler was found for the application type or an
  *         invalid octeon id was passed.
  *         -EINVAL if invalid operation was specified.
  *         0 on success.
  */
int    octeon_stop_module(uint32_t app_type, uint32_t  octeon_id);


char*  get_oct_state_string(cavium_atomic_t  *state_ptr);

char*  get_oct_app_string(int  app_mode);


int
octeon_setup_instr_queues(octeon_device_t  *oct);

int
octeon_setup_output_queues(octeon_device_t  *oct);

void
octeon_set_io_queues_off(octeon_device_t  *oct);


int
octeon_send_short_command(octeon_device_t  *oct,
                          uint16_t          opcode,
                          uint8_t           param,
                          void             *rptr,
                          int               rsize);

void *
oct_get_config_info(octeon_device_t   *oct);

int
check_bp_on(octeon_device_t   *oct);

void
print_octeon_state_errormsg(octeon_device_t  *oct);


#endif 


/* $Id: octeon_device.h 67094 2011-11-15 20:41:21Z mchalla $ */
