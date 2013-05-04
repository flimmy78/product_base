#ifndef __PRESTERAIRQ
#define __PRESTERAIRQ

#include <linux/types.h>
#include <linux/ioctl.h> /* needed for the _IOW etc stuff used later */
#include "prestera.h"
#define INT_ERROR           (-1)
#define INT_OK              (0x00)   /* Operation succeeded */
#define INT_FAIL            (0x01)   /* Operation failed    */

#define INT_TRUE            (0x01)   /* Operation passed    */
#define INT_FALSE           (0x00)   /* Operation failed    */

#define PRESTERA_IRQ       12
#define PRESTERA_IRQ_MASK  (1<<PRESTERA_IRQ)

#define GT_BAD_VALUE       (0x02)   /* Illegal value        */
#define GT_OUT_OF_RANGE    (0x03)   /* Value is out of range*/
#define GT_BAD_PARAM       (0x04)   /* Illegal parameter in function called  */
#define GT_BAD_PTR         (0x05)   /* Illegal pointer value                 */
#define GT_BAD_SIZE        (0x06)   /* Illegal size                          */
#define GT_BAD_STATE       (0x07)   /* Illegal state of state machine        */
#define GT_SET_ERROR       (0x08)   /* Set operation failed                  */
#define GT_GET_ERROR       (0x09)   /* Get operation failed                  */
#define GT_CREATE_ERROR    (0x0A)   /* Fail while creating an item           */
#define GT_NOT_FOUND       (0x0B)   /* Item not found                        */
#define GT_NO_MORE         (0x0C)   /* No more items found                   */
#define GT_NO_SUCH         (0x0D)   /* No such item                          */
#define GT_TIMEOUT         (0x0E)   /* Time Out                              */
#define GT_NO_CHANGE       (0x0F)   /* The parameter is already in this value*/
#define GT_NOT_SUPPORTED   (0x10)   /* This request is not support           */
#define GT_NOT_IMPLEMENTED (0x11)   /* This request is not implemented       */
#define GT_NOT_INITIALIZED (0x12)   /* The item is not initialized           */
#define GT_NO_RESOURCE     (0x13)   /* Resource not available (memory ...)   */
#define GT_FULL            (0x14)   /* Item is full (Queue or table etc...)  */
#define GT_EMPTY           (0x15)   /* Item is empty (Queue or table etc...) */
#define GT_INIT_ERROR      (0x16)   /* Error occured while INIT process      */
#define GT_NOT_READY       (0x1A)   /* The other side is not ready yet       */
#define GT_ALREADY_EXIST   (0x1B)   /* Tried to create existing item         */
#define GT_OUT_OF_CPU_MEM  (0x1C)   /* Cpu memory allocation failed.         */
#define GT_ABORTED         (0x1D)   /* Operation has been aborted.           */

typedef void  (*VOIDFUNCPTR)(void); /* ptr to function returning void */
typedef unsigned int (*INTFUNCPTR)(void); /* ptr to function returning int  */



typedef unsigned char (*ISR_FUNCP)
(
    void *cookie
);

#if 0
typedef enum
{
    INTR_MODE_LOCK =0,
    INTR_MODE_UNLOCK
}INTERRUPT_MODE;
#endif

/*
 * Typedef: struct ISR_PARAMS
 *
 * Description: Holds interrupt service routine parameters to be used when an
 *              interrupt is received.
 *
 * Fields:
 *      isrFuncPtr  - Isr function pointer to be called on interrupt reception.
 *      cookie      - A user define parameter to be passed to the user on
 *                    interrupt service routine invokation.
 *
 */
typedef struct isr_params
{
    ISR_FUNCP isrFuncPtr;
    void      *cookie;
    struct isr_params   *next;
} ISR_PARAMS;


/*
 * Typedef: struct INT_VEC_MEM_LIST
 *
 * Description: interrupt vector members list, points to a list of devices
 *              connected to the specified interrupt vector.
 *
 * Fields:
 *      intVecNum - The interrupt vector number representing this list.
 *      devListHead - A pointer to the head of the linked list holding the
 *                    device number.
 *
 */
typedef struct
{
    unsigned int intVecNum;
    struct isr_params *first;
    ISR_PARAMS *isrParamsArray[MAX_PP_DEVICES];
} INT_VEC_MEM_LIST;



/*
 * Typedef: enum INT_STATUS
 *
 * Description: Defines the different interrupt node states, that an interrupt
 *              node may havemarvell.h.
 *
 * Fields:
 *      INT_ACTIVE      - The interrupt is active and can be handled, was not
 *                        set to pending state.
 *      INT_PENDING     - The interrupt is in the pending queue.
 *      INT_STOPPED     - Used to stop interrupt handling on Hot removal.
 *
 */
typedef enum
{
    INT_ACTIVE = 0,
    INT_PENDING,
    INT_STOPPED
} INT_STATUS;




/*
 * Typedef: struct FUNCP_HW_READ_REGISTER
 *
 * Description: Pointer to funtion performing hardware read
 *
 * Fields:
 *       devNum  - The device number to read.
 *       regAddr - The register's address to read from.
 *       data    - Data word pointer for read data.
 */
typedef unsigned int (*FUNCP_INT_READ_REGISTER)
(
    unsigned int devNum,
    ulong addr,
    ulong * value
);
/*
 * Typedef: struct FUNCP_HW_READ_REGISTER
 *
 * Description: Pointer to funtion performing hardware read
 *
 * Fields:
 *       devNum  - The device number to read.
 *       regAddr - The register's address to read from.
 *       data    - Data word pointer for read data.
 */
typedef unsigned int (*FUNCP_INT_WRITE_REGISTER)
(
    unsigned int devNum,
    ulong addr,
    ulong value
);


/*
 * Typedef: struct FUNCP_HW_READ_REGISTER
 *
 * Description: Pointer to funtion performing hardware read
 *
 * Fields:
 *       devNum  - The device number to read.
 *       regAddr - The register's address to read from.
 *       data    - Data word pointer for read data.
 */
typedef unsigned int (*FUNCP_HW_READ_REGISTER)
(
    unsigned char devNum,
    unsigned int regAddr,
    unsigned int *data
);

/*
 * Typedef: struct FUNCP_HW_WRITE_REGISTER
 *
 * Description: Pointer to funtion performing hardware read
 *
 * Fields:
 *       devNum  - The device number to write.
 *       regAddr - The register's address to write to.
 *       data    - Data word to write.
 */
typedef unsigned int (*FUNCP_HW_WRITE_REGISTER)
(
    unsigned char devNum,
    unsigned int regAddr,
    unsigned int data
);

typedef enum _int_boot
{
    _INT_FALSE = 0,
    _INT_TRUE  = 1
} INT_BOOL;


/*
 * Typedef: enum GT_GPP_ID
 *
 * Description: Holds the different GPP ids in a given Pp.
 *
 * Fields:
 *  GT_GPP_INT_1    - Gpp interrupt pin No.1
 *  GT_GPP_INT_3    - Gpp interrupt pin No.2
 *  GT_GPP_INT_2    - Gpp interrupt pin No.3
 *  GT_UPLINK_GPP   - Uplink Gpp.
 *  GT_GPP_INTERNAL - Gpp used for logical internal interrupt connections.
 *  GT_GPP_MAC_NUM  - Indicates the number of Gpps defined in system.
 *
 * Note:
 *  Don't modify the order or values of this enum.
 */
typedef enum
{
    GT_GPP_INT_1 = 0,
    GT_GPP_INT_2,
    GT_GPP_INT_3,
    GT_UPLINK_GPP,
    GT_GPP_INTERNAL_1,
    GT_GPP_INTERNAL_2,
    GT_GPP_INTERNAL_3,
    GT_GPP_INTERNAL_4,
    GT_GPP_INTERNAL_5,
    GT_GPP_INTERNAL_6,
    GT_GPP_INTERNAL_7,
    GT_GPP_INTERNAL_8,
    GT_GPP_INTERNAL_9,
    GT_GPP_INTERNAL_10,
    GT_GPP_INTERNAL_11,
    GT_GPP_INTERNAL_12,
    GT_GPP_INTERNAL_13,
    GT_GPP_INTERNAL_14,
    GT_GPP_INTERNAL_15,
    GT_GPP_INTERNAL_16,
    GT_GPP_INTERNAL_17,
    GT_GPP_MAX_NUM
} GPP_ID;

/*
 * Typedef: struct GPP_ISR_FUNCP
 *
 * Description: Pointer to interrupt service routine to be called on gpp
 *              interrupts.
 *
 * Fields:
 *      devNum  - The Pp device number.
 *      gppId   - The Gpp Id that caused the interrupt.
 */
typedef unsigned int (*GPP_ISR_FUNCPTR)
(
    unsigned char       devNum,
    GPP_ID   gppId
);

/*
 * Typedef: struct INTERRUPT_SCAN
 *
 * Description: Basic struct for accsessing interrupt register in hierarchy
 *              tree.
 *
 * Fields:
 *      bitNum          - Sum bit num in upper hierarchy, representing this
 *                        resigter.
 *      isGpp           - Is hierarcy connected to another device.
 *      gppId           - The gpp Id represented by this node (Valid only if
 *                        isGpp is GT_TRUE).
 *      gppFuncPtr      - Pointer to isrFunc to be called if isGPP == GT_TRUE.
 *      devNum          - Device number for isrFunc Call.
 *      causeRegAddr    - Address of the interrupt cause register to scan.
 *      maskRegAddr     - Address of the interrupt mask register to update after
 *                        scanning.
 *      pRegReadFunc    - A pointer to a function to be used for reading the
 *                        interrupt cause register.
 *      pRegWriteFunc   - A pointer to a function to be used for writing the
 *                        interrupt mask register.
 *      startIdx        - The start interrupt index representing the interrupts
 *                        to be scanned.
 *      endIdx          - The end interrupt index representing the interrupts to
 *                        be scanned.
 *      nonSumBitMask   - bit mask of non sum bits in the register
 *      rwBitMask       - bit mask of R/W bits that should be cleared by write.
 *      maskRcvIntrEn   - bit mask of interrupts to be masked after receive.
 *      subIntrListLen  -
 *      subIntrScan     - pointer to sum bit interrupt register structure.
 *      nextIntrScan    - pointer to next interrupt struct in hirarchy.
 */
typedef struct _intr_scan
{
	unsigned char            bitNum;
	INT_BOOL                 isGpp;
	GPP_ID                   gppId;
	GPP_ISR_FUNCPTR          gppFuncPtr;
	unsigned int             causeRegAddr;
	unsigned int             maskRegAddr;
	FUNCP_INT_READ_REGISTER  pRegReadFunc;
	FUNCP_INT_WRITE_REGISTER pRegWriteFunc;
	unsigned int             startIdx;
	unsigned int             endIdx;
	unsigned int             nonSumBitMask;
	unsigned int             rwBitMask;
	unsigned int             maskRcvIntrEn;
	unsigned int             subIntrListLen;
	struct _intr_scan        **subIntrScan;
	struct _intr_scan        **nextIntrScan;
} INTERRUPT_SCAN;

/*
 * Typedef: enum INT_RECEPTION_STATUS
 *
 * Description: This enum defines the different types of interrupt receptions.
 *
 * Fields:
 *      INT_RECEIVED    - The interrupt was received during regular operation
 *                        of the system (and was not handled yet).
 *      INT_MASK_REC    - The interrupt was received while it was masked. (and
 *                        was not handled yet).
 *      INT_IDLE        - The interrupt was not received lately. (was already
 *                        handled or was not received).
 *
 */
typedef enum
{
	INT_RECEIVED = 0,
	INT_MASK_REC,
	INT_IDLE
} INT_RECEPTION_STATUS;


/* Forward struct declaration.  */
struct _intNode;


/*
 * Typedef: FUNCP_INT_UNMASK
 *
 * Description: Interrupt unmasking function pointer.
 *
 * Fields:
 *
 */
typedef unsigned int (*FUNCP_INT_UNMASK)
(
	struct _intNode *intNode
);


/*
 * typedef: FUNCP_INT_REC_CB
 *
 * Description: Callback function called when an intterupt is received.
 *
 * Fields:
 *      devNum      - Device number.
 *      intIndex    - The interrupt index.
 *
 * Comment:
 */
typedef void* (*FUNCP_INT_REC_CB)
(
    unsigned char  devNum,
    unsigned int intIndex
);



/*
 * Typedef: struct INT_NODE
 *
 * Description: Includes information of received unhandled interrupts, to be
 *              managed throw the int queues structure.
 *
 * Fields:
 *      devNum      - The PP device number.
 *      intTypeIdx  - Index of the itnerrupt type represented by this node.
 *      intIndex    - The interrupt index.
 *      funcPtr     - A pointer to the function to be called to handle this
 *                    interrupt.
 *      cbFuncPtr   - A pointer to a callback function, to be called after
 *                    funcPtr is executed.
 *      intRecCbFuncPtr - A pointer to a callback function ,to be called after
 *                    an interrupt was recived before the interrupt task.
 *      priority    - The priority of the interrupt represented by this node.
 *      intMaskReg  - Register address of the interrupt_mask register
 *                    representing this interrupt.
 *      intBitMask  - Bit mask in intMaskReg register, which represents the bit
 *                    relevant to this interrupt.
 *      intCounter  - Counts the total interupts received of this type.
 *      queueIdx    - The queue index this interrupt belongs to, this value is
 *                    maintained from the intIndex2Queue[] array at
 *                    initialization phase.
 *      queued      - Indicates whether this interrupt is queued in the
 *                    interrupts queue or not.
 *      intRecStatus- Indicates the time by which the interrupt was received.
 *      prev        - A pointer to the previous INT_NODE in the linked-list.
 *      next        - A pointer to the next INT_NODE in the linked-list.
 */
typedef struct _intNode
{
	unsigned char  devNum;
	unsigned char intTypeIdx;
	unsigned int  intIndex;
  INTFUNCPTR funcPtr;
  INTFUNCPTR cbFuncPtr;
  FUNCP_INT_REC_CB intRecCbFuncPtr;
  FUNCP_INT_UNMASK intUnmaskFuncPtr;
  unsigned char priority;
  unsigned int  intMaskReg;
  unsigned int  intBitMask;
  unsigned int  intCounter;
  unsigned char queueIdx;
  unsigned char qWeight;
  INT_STATUS   intStatus;
  INT_RECEPTION_STATUS    intRecStatus;
  struct _intNode *prev;
  struct _intNode *next;
}INT_NODE, *INT_NODE_PTR;

/*
 * Typedef: struct INT_TASK_PARAMS
 *
 * Description: Holds interrupt task parameters to be passed to the generic
 *              interrupt task.
 *
 * Fields:
 *      intQueueId  - The interrupts queue pool Id.
 *      intTaskSem  - Interrupt task semaphore Id.
 *      intTaskId   - Interrupt task Task-Id.
 *      intTaskInProcess - Interrupt task in process flag.
 *
 */
typedef struct
{
	unsigned char intQueueId;
  void *       intTaskSem;
  unsigned int  intTaskId;
	INT_BOOL     intTaskInProcess;
}INT_TASK_PARAMS;

/*
 * Typedef: struct INTERRUPT_INIT_PARAMS
 *
 * Description: This struct includes initializing info for a given interrupt.
 *
 * Fields:
 *      intHandler - A pointer to a function to be called for handling this
 *                   interrupt.
 *      intCbFuncPtr - A pointer to the callBack function to be called after
 *                     interrupt handling was done.
 *      priority    - The priority to which this interrupt should be assigned.
 *      queueIdx    - A mapping of the interrupt causes, to queue index, which
 *                    is mainly used for Rx / Tx / AU purposes.
 *
 */
typedef struct
{
 INTFUNCPTR   intHandler;
 INTFUNCPTR   intCbFuncPtr;
 unsigned char priority;
 unsigned char queueIdx;
}INTERRUPT_INIT_PARAMS;




#define MAGIC 054217146
/* Should check what is the max size of read/write DMA in prestera */
#define MAX_LEN 64

unsigned int intConnect(unsigned int vector, void * routine, struct InterruptData **cookie);

#endif /* __PRESTERAIRQ */
