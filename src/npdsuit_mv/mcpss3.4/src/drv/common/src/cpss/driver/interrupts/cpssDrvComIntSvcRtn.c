/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvComIntSvcRtn.c
*
* DESCRIPTION:
*       Includes generic interrupt service routine functions declarations.
*
* FILE REVISION NUMBER:
*       $Revision: 1.2 $
*
*******************************************************************************/

/* get the OS , extDrv functions*/
#include <cpss/extServices/private/prvCpssBindFunc.h>
/* get common defs */
#include <cpssCommon/cpssPresteraDefs.h>

/* get the common deriver info */
#include <cpss/driver/interrupts/cpssDrvComIntEvReqQueues.h>
#include <cpss/driver/interrupts/cpssDrvComIntSvcRtn.h>

/* get interrupts names */
#include <cpss/generic/events/private/prvCpssGenEvReq.h>

/*******************************************************************************
* internal definitions
*******************************************************************************/

#ifdef GEN_ISR_DEBUG
#define DBG_INFO(x)   cpssOsPrintf x
#else
#define DBG_INFO(x)
#endif

/*
 * Typedef: struct PRV_CPSS_DRV_INT_ISR_COOKIE_STC_XXX
 * 
 * Description: a cookie info for the ISR to use , to distinguish
 *              between devices and port groupss .
 *
 * Fields:
 *      devNum      - the device number that associated with the cookie.
 *      portGroupId - the port group Id that associated with the cookie.
 *                    for 'non-multi port groups' device use index
 *                    CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS.
 * prvCpssDrvInterrupts.h
 * zhangdi@autelan.com 2012-04-13 for 2 lion interrupt scan on AX81-SMU .
 */
typedef struct{
    GT_U8       devNum;
    GT_U32      portGroupId;
}PRV_CPSS_DRV_INT_ISR_COOKIE_STC_XXX;


/*
*
* add by zhubo 2008.6.16
*/

typedef struct {
	GT_U8       arEther[6];
}ETHERADDR;

typedef struct {

    ETHERADDR                        macSa;
    GT_U16                              vlan;
    GT_U8                               port;
    GT_U32    							code;

}BRG_SECUR_BREACH_MSG_STC;


#ifndef __AX_PLATFORM__
#define IS_SDMA_RX_INT_CAUSE(uniEvCause)	\
		(uniEvCause>=CPSS_PP_RX_BUFFER_QUEUE0_E && uniEvCause<=CPSS_PP_RX_ERR_QUEUE7_E) ? GT_TRUE : GT_FALSE
#define IS_SDMA_RX_PKT_INT_CAUSE(uniEvCause) 	\
		(uniEvCause>=CPSS_PP_RX_BUFFER_QUEUE0_E && uniEvCause<=CPSS_PP_RX_BUFFER_QUEUE7_E) ? GT_TRUE : GT_FALSE
#define IS_SDMA_RX_ERR_INT_CAUSE(uniEvCause) 	\
			(uniEvCause>=CPSS_PP_RX_ERR_QUEUE0_E && uniEvCause<=CPSS_PP_RX_ERR_QUEUE7_E) ? GT_TRUE : GT_FALSE
#endif

GT_VOID interruptMainSr
(
    IN GT_U8 intVecMapIdx
);

/*  about INT_SCAN_DEBUG :
    to open this flag use environment parameter:
    set INT_SCAN_DEBUG=DEBUG_ON

    use function intScanPrintControl(...) to allow/deny printings
*/

/* #define INT_SCAN_DEBUG */

/*
    about INT_SCAN_DEBUG_NOT_ENABLE_INTERRUPT_IN_INITIALIZATION :
    allow to skip the enabling of the interrupts : cpssExtDrvIntEnable(intMask);
    --> to allow initialization to go over with no 'Interrupt issues'
        --> to allow to debug interrupts after initialization.
            --> call extDrvIntEnable(intMask) <-- according to the printings of "intMask"
*/
/*#define INT_SCAN_DEBUG_NOT_ENABLE_INTERRUPT_IN_INITIALIZATION*/
/*#ifdef INT_SCAN_DEBUG*/
#if 1
	
    #undef DBG_INFO
    #define DBG_INFO(x)     cpssOsPrintSync x

    static char * uniEvName[CPSS_UNI_EVENT_COUNT_E] = {UNI_EV_NAME};
    static GT_U32   allowPrint=1;/* option to disable the print in runtime*/
    #ifdef ASIC_SIMULATION
        #define DBG_LOG(x)  if(allowPrint) cpssOsPrintf x
    #else
/*        extern int logMsg(char *, int, int, int, int, int, int); */
        #define DBG_LOG(x)  if(allowPrint) cpssOsPrintSync x
    #endif

/*control the interrupt printings , set new value
 function return the old value

 this function need to be called from the terminal...
*/
extern GT_U32 intScanPrintControl(IN GT_U32  allowPrintNew)
{
    GT_U32  oldState = allowPrint;
    allowPrint = allowPrintNew;

    return oldState;
}

/* function need to be called before the initSystem ,
   the function will generate task that will be responsible to periodically
   (every 200ms) to cal the ISR
*/
typedef GT_U32  GT_TASK;
extern GT_STATUS osTaskCreate
(
    IN  char    *name,
    IN  GT_U32  prio,
    IN  GT_U32  stack,
    IN  unsigned (__TASKCONV *start_addr)(void*),
    IN  void    *arglist,
    OUT GT_TASK *tid
);
extern GT_STATUS osTimerWkAfter(    IN GT_U32 mils);

#define INT_SCAN_PRIO_CNS            0
#define INT_SCAN_STACK_SIZE_CNS      0x2000

/* set to 1 after aw.state = 1 , in appDemoBoardAfterSysUpIntEnable() */
extern GT_U32   pollInterruptIsEnabled=0;

static GT_U32   dbgIntScanUseTaskRunning =0;
static GT_U32   dbgInterruptIsEnabled=0;
static GT_U8    dbgDevNum=0;
static unsigned __TASKCONV intScanPollingTask
(
    IN GT_VOID * num_of_asic
)
{
    GT_U8 asic = (GT_U8)num_of_asic;
    GT_8  tName[30] = {0};  /* The task name */
    GT_U8 i = 0;
	/* tell my thead id */
	sprintf(tName, "intScanPolling_%d",asic);
	npd_init_tell_whoami(tName,0);
	osPrintfDbg("intScanPollingTask_%d :: dev %d\n", \
				asic, asic);


    /* wait for the 'Interrupt enable' is called */
    while(1)
    {
		#if 0
        osTimerWkAfter(10);
        if(dbgInterruptIsEnabled)
        {
            break;
        }
		#else
        osTimerWkAfter(100);		
        /* set to 1 after aw.state = 1 , in appDemoBoardAfterSysUpIntEnable() */
		if(pollInterruptIsEnabled)
        {
            break;
        };
        #endif
    }
    /* pulling on the ISR to check if there are interrupts waiting */
    while(1)
    {
		for(i=0;i<asic;i++)
		{
            osTimerWkAfter(200);
            /* invoke ISR until all interrupts have been treated */
            interruptMainSr(i);
		}
    }

}


extern GT_STATUS intScanUseTask(IN GT_U8 num_of_asic)
{
    GT_TASK tid;
    GT_U8  i,devNum;
	GT_U32	pollArrIdx;
    GT_8  taskName[30] = {0};  /* The task name */

    osSprintf(taskName, "intScanPolling_%d", i);

    if(osTaskCreate(taskName,
        INT_SCAN_PRIO_CNS , INT_SCAN_STACK_SIZE_CNS,
        /* create the ISR pulling task */
        intScanPollingTask,(GT_VOID*)num_of_asic, &tid) != GT_OK)
    {
        return GT_FAIL;
    }

#if 0   /* change to use one thread, zhangdi@autelan.com 2012-06-12 */
    for(i=0;i<num_of_asic;i++)
    {
        osSprintf(taskName, "intScanPolling_%d", i);

        if(osTaskCreate(taskName,
            INT_SCAN_PRIO_CNS , INT_SCAN_STACK_SIZE_CNS,
            /* create the ISR pulling task */
            intScanPollingTask,(GT_VOID*)i, &tid) != GT_OK)
        {
            return GT_FAIL;
        }

    }
#endif
    /*dbgIntScanUseTaskRunning = 1;*/

    /*we can't allow printings because the task doing 'pulling' every 200ms */
    intScanPrintControl(0);	
    return GT_OK;
}

#else
    #undef DBG_INFO
    #define DBG_INFO(x)
    #define DBG_LOG(x)

extern GT_STATUS intScanUseTask(void)
{
    return GT_NOT_IMPLEMENTED;
}
#endif

/*******************************************************************************
* Internal usage variables
*******************************************************************************/
/* flag that state the DB of cpss common was released and now we are after that stage
    NOTE: the flag start with GT_FALSE , but once changed to GT_TRUE it will stay
          that way forever.
*/
GT_BOOL  prvCpssCommonAfterDbRelease = GT_FALSE;

/*
 * Typedef: structure PRV_CPSS_DRV_COMMON_INT_SERVICE_ROUTINE_STC
 *
 * Description: the DB of this file
 *
 * Fields:
 *
 *      intVecMap - mapping between interrupt vectors and the devices connected to them
 *                  NOTE : this array is NOT accessed by devNum as index !!!
 *                         this array contain the devices from index 0 till max
 *                         index
 *      txEndCb - tx competed call back that need to be called when the
 *                "tx ended" received for the PP
 *                This array is accessed by devNum as index
 *
*/
typedef struct{
    PRV_CPSS_DRV_INT_VEC_MEM_LIST_STC *intVecMap[PRV_CPSS_MAX_PP_DEVICES_CNS];
    PRV_CPSS_DRV_COM_INT_TX_ENDED_CB   txEndCb[PRV_CPSS_MAX_PP_DEVICES_CNS];
}PRV_CPSS_DRV_COMMON_INT_SERVICE_ROUTINE_STC;

/* the DB of this file */
static PRV_CPSS_DRV_COMMON_INT_SERVICE_ROUTINE_STC  prvCpssDrvComIntSvcRtnDb=
    {
        {NULL},/*intVecMap[PRV_CPSS_MAX_PP_DEVICES_CNS]*/
        {NULL},/*txEndCb[PRV_CPSS_MAX_PP_DEVICES_CNS]*/
    };

/* Macro definitions for interrupts scanning functions use.  */

/* Return an interrupt cause specific bit.          */
#define INT_CAUSE_BIT(causeReg,intIdx)  \
            (((causeReg) >> ((intIdx) & 0x1f)) & 0x1)


/* Updated the interrupt mask bits to be set into   */
/* the int_mask register.                           */
#define UPDATE_INT_MASK_BITS(intMaskBits,i) \
            ((intMaskBits) |= (1 << ((i) & 0x1f)))


#define NOT_USED_CNS    0xffffffff

/* indication that array of afterDbReleaseIntVecAttached was initialized */
static GT_BOOL afterDbReleaseIntVecAttached_initialized = GT_FALSE;
/* DB to save the interrupt lines that were attached to the extDrv (external driver) */
static GT_U32 afterDbReleaseIntVecAttached[PRV_CPSS_MAX_PP_DEVICES_CNS] =
    {
        0
    };

/*******************************************************************************
* isIntLineAlreadyUsed
*
* DESCRIPTION:
*       check if ISR was already connected to the extDrv (external driver), for given interrupt line
*       this function needed when we are initialize after we had 'DB release'
*
* INPUTS:
*       intVecNum   - The interrupt vector number this device is connected to.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_FALSE   - the interrupt line was not connected yet to extDrv (external driver)
*       GT_TRUE    - the interrupt line was connected already to extDrv (external driver)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_BOOL isIntLineAlreadyUsed
(
    IN  GT_U32          intVecNum
)
{
    GT_U32  ii;

    if(prvCpssCommonAfterDbRelease == GT_FALSE)
    {
        return GT_FALSE;
    }

    if(afterDbReleaseIntVecAttached_initialized == GT_FALSE)
    {
        /* check in a 'static' DB for the interrupt lines that already bound */
        for(ii = 0 ;ii < PRV_CPSS_MAX_PP_DEVICES_CNS ; ii++)
        {
            afterDbReleaseIntVecAttached[ii] = NOT_USED_CNS;
        }

        afterDbReleaseIntVecAttached_initialized = GT_TRUE;
    }

    /* check in a 'static' DB for the interrupt lines that already bound */
    for(ii = 0 ;ii < PRV_CPSS_MAX_PP_DEVICES_CNS ; ii++)
    {
        if(afterDbReleaseIntVecAttached[ii] == intVecNum)
        {
            /* existing interrupt line */
            return GT_TRUE;
        }
    }

    /* new interrupt line */
    return GT_FALSE;
}

/*******************************************************************************
* intLineIsUsed
*
* DESCRIPTION:
*       set the ISR was connected to the extDrv (external driver), for given interrupt line
*       this function needed when we are initialize after we had 'DB release'
*
* INPUTS:
*       intVecNum   - The interrupt vector number this device is connected to.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       none
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void intLineIsUsed
(
    IN  GT_U32          intVecNum
)
{
    GT_U32  ii;

    /* check in a 'static' DB for the interrupt lines that a */
    for(ii = 0 ;ii < PRV_CPSS_MAX_PP_DEVICES_CNS ; ii++)
    {
        /* find first free place */
        if(afterDbReleaseIntVecAttached[ii] == NOT_USED_CNS)
        {
            /* set the interrupt line as used */
            afterDbReleaseIntVecAttached[ii] = intVecNum;
            return;
        }
    }

    return;
}



#define FIRST_FREE_NOT_FOUND_CNS 0xFFFFFFFF
/*******************************************************************************
* prvCpssDrvInterruptConnect
*
* DESCRIPTION:
*       Connect a given function to the given interrupt vector (mask).
*
* INPUTS:
*       intVecNum   - The interrupt vector number this device is connected to.
*       intMask     - The interrupt mask to enable/disable interrupts on
*                     this device.
*       intRoutine  - A pointer to the interrupt routine to be connected to the
*                     given interrupt line.
*       cookie      - A user defined cookie to be passed to the isr on interrupt
*                     reception.
*       connectionId- Id to be used for future access to the connected isr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on failure
*       GT_OUT_OF_CPU_MEM - on memory allocation failure
*       GT_FULL - the DB is full and can't connect the new interrupt vector num
*
*
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvInterruptConnect
(
    IN  GT_U32          intVecNum,
    IN  GT_U32          intMask,
    IN  CPSS_EVENT_ISR_FUNC intRoutine,
    IN  void            *cookie,
    OUT GT_U32          *connectionId
)
{
    GT_U32          firstFree;  /* The index of the first free cell in      */
                                /* prvCpssDrvComIntSvcRtnDb.intVecMap to be */
                                /* used if we need to create                */
                                /* a new list for a new interrupt vector.   */
    GT_BOOL         newList;    /* Whether a new list of devices should be  */
                                /* created.                                 */
    PRV_CPSS_DRV_ISR_PARAMS_STC*  isrParams, *prev,*isrParams_temp;
    GT_U32          i,j;
    PRV_CPSS_DRV_INT_VEC_MEM_LIST_STC   *vecMemListPtr;/* pointer to the current
                                interrupt vector */

    PRV_CPSS_DRV_INT_ISR_COOKIE_STC_XXX        *isrCookieInfoPtr;/*pointer to format of the cookie of interrupt*/


    firstFree   = FIRST_FREE_NOT_FOUND_CNS;
    newList     = GT_TRUE;


    isrCookieInfoPtr = cookie ;
	osPrintfDbg("\n***@@@*********** dev %d group %d\n", isrCookieInfoPtr->devNum, isrCookieInfoPtr->portGroupId);

    #if 0
    for(i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        if(prvCpssDrvComIntSvcRtnDb.intVecMap[i] == NULL)
        {
            if(firstFree == FIRST_FREE_NOT_FOUND_CNS)
            {
                firstFree = i;
            }
        }
        else if(prvCpssDrvComIntSvcRtnDb.intVecMap[i]->intVecNum == intVecNum)
        {
            newList  = GT_FALSE;
            break;
        }
    }
    #else
	/* Set newlist by dev, not by interrupt number. zhangdi@autelan.com 2012-04-17 */
	i = isrCookieInfoPtr->devNum;

    if(prvCpssDrvComIntSvcRtnDb.intVecMap[isrCookieInfoPtr->devNum] == NULL)
    {
        if(firstFree == FIRST_FREE_NOT_FOUND_CNS)
        {
            firstFree = isrCookieInfoPtr->devNum;
        }
    }
    else
    {
        newList  = GT_FALSE;
     }	
	#endif

	
	osPrintfDbg("*****@@@*********** newList %d i %d \n", newList,i);

    if (newList == GT_TRUE)
    {
        if(firstFree == FIRST_FREE_NOT_FOUND_CNS)
        {
            /* all the array filled already with other vectors */
            return GT_FULL;
        }

        i = firstFree;
        prvCpssDrvComIntSvcRtnDb.intVecMap[i] =
            cpssOsMalloc(sizeof(PRV_CPSS_DRV_INT_VEC_MEM_LIST_STC));
        if(prvCpssDrvComIntSvcRtnDb.intVecMap[i] == NULL)
        {
            return GT_OUT_OF_CPU_MEM;
        }

        vecMemListPtr = prvCpssDrvComIntSvcRtnDb.intVecMap[i];

        cpssOsMemSet(vecMemListPtr,0,sizeof(PRV_CPSS_DRV_INT_VEC_MEM_LIST_STC));

        vecMemListPtr->intVecNum     = intVecNum;

        j = 0;/* set index 0 as the first free index ... */
    }
    else
    {

        vecMemListPtr = prvCpssDrvComIntSvcRtnDb.intVecMap[i];

        /* look for an empty space to add the device's isr params */
        for (j = 0; j < PRV_CPSS_MAX_PP_DEVICES_CNS; j++)
        {
            if (vecMemListPtr->isrParamsArray[j]== NULL ||
                vecMemListPtr->isrParamsArray[j]->isrFuncPtr == NULL)
                break;
        }
    }

    /* no room left */
    if (j == PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        return GT_FULL;
    }

    if (vecMemListPtr->isrParamsArray[j]== NULL)
    {
        /* Allocate a new isr params struct.    */
        isrParams = cpssOsMalloc(sizeof(PRV_CPSS_DRV_ISR_PARAMS_STC));
        if(isrParams == NULL)
        {
            return GT_OUT_OF_CPU_MEM;
        }
        vecMemListPtr->isrParamsArray[j] = isrParams;
    }
    else
    {
        isrParams = vecMemListPtr->isrParamsArray[j];
    }
    isrParams->cookie       = cookie;
    isrParams->isrFuncPtr   = intRoutine;
    *connectionId           = (GT_U32)isrParams;


    /* arrange the list */
    prev = NULL;
    for (j = 0; j < PRV_CPSS_MAX_PP_DEVICES_CNS; j++)
    {
        if (vecMemListPtr->isrParamsArray[j] != NULL &&
            vecMemListPtr->isrParamsArray[j]->isrFuncPtr != NULL)
        {
            osPrintfDbg("****@@@*********** j %d \n", j);
			
            if (prev != NULL)
            {
                prev->next = vecMemListPtr->isrParamsArray[j];
            }
            prev = vecMemListPtr->isrParamsArray[j];
        }
    }

    if (prev != NULL)
    {
        prev->next = NULL;
    }


    /* For debug the cookie list */
    isrParams_temp = vecMemListPtr->isrParamsArray[0];
    while(isrParams_temp != NULL)
    {

        isrCookieInfoPtr = isrParams_temp->cookie ;
    	osPrintfDbg("*****@@@*********** :: dev %d group %d\n", isrCookieInfoPtr->devNum, isrCookieInfoPtr->portGroupId);				

        /* proceed to the next device */
        isrParams_temp = isrParams_temp->next;
    }
    /* End */

    /* If reached here, the interrupt vector represented in cell i  */
    /* of prvCpssDrvComIntSvcRtnDb.intVecMap contains the device number
       devNum in it's list. */

    if(newList == GT_TRUE)
    {
        vecMemListPtr->first = isrParams;

        if(isIntLineAlreadyUsed(intVecNum) == GT_FALSE)
        {
#ifdef INT_SCAN_DEBUG
            if(dbgIntScanUseTaskRunning)
            {
                dbgInterruptIsEnabled = 1;
                dbgDevNum = (GT_U8)i;
            }
            else
#endif /*INT_SCAN_DEBUG*/
            {
                /* Connect the ISR to the interrupt vector. */
                cpssExtDrvIntConnect(intVecNum, (GT_VOIDFUNCPTR)interruptMainSr, i);

#ifndef INT_SCAN_DEBUG_NOT_ENABLE_INTERRUPT_IN_INITIALIZATION
                /* enable interrupt.    */
                cpssExtDrvIntEnable(intMask);
#else /*INT_SCAN_DEBUG_NOT_ENABLE_INTERRUPT_IN_INITIALIZATION*/
                cpssOsPrintf("intMask [%d] \n",intMask);
#endif /*INT_SCAN_DEBUG_NOT_ENABLE_INTERRUPT_IN_INITIALIZATION*/
            }

            /* set the DB that the interrupt line connected to extDrv (external driver) */
            intLineIsUsed(intVecNum);
        }
    }

    return GT_OK;
}


/*******************************************************************************
* prvCpssDrvInterruptDisconnect
*
* DESCRIPTION:
*       Disconnect a device from a given interrupt vector.
*
* INPUTS:
*       intVecNum   - The interrupt vector number the device is connected to.
*       connectioId - The connection Id returned on the interrupt connect
*                     operation.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_NO_SUCH - there is not such interrupt vector num
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvInterruptDisconnect
(
    IN  GT_U32  intVecNum,
    IN  GT_U32  connectionId
)
{
    PRV_CPSS_DRV_ISR_PARAMS_STC         *isrParams; /* pointer to ISR params */
    PRV_CPSS_DRV_ISR_PARAMS_STC         *prev;      /* previous pointer to ISR params */
    GT_U32           i,j,k;              /* Loops index.                     */
    PRV_CPSS_DRV_INT_VEC_MEM_LIST_STC   *vecMemListPtr = NULL;/* pointer to the current
                                interrupt vector */


    /* Search for the appropriate cell in prvCpssDrvComIntSvcRtnDb.intVecMap.*/
    for(i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++ )
    {
        vecMemListPtr = prvCpssDrvComIntSvcRtnDb.intVecMap[i];
        if(vecMemListPtr == NULL)
        {
            continue;
        }

        if(vecMemListPtr->intVecNum == intVecNum)
        {
            break;
        }
    }

    if(i == PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        /* not found , return OK !! */
        /* nothing more to clean ! */
        return GT_OK;
    }

    isrParams = (PRV_CPSS_DRV_ISR_PARAMS_STC*)connectionId;
    prev = NULL;
    /* find appropriate device ISR parameters  */
    for (j = 0; j < PRV_CPSS_MAX_PP_DEVICES_CNS; j++)
    {
        if (vecMemListPtr->isrParamsArray[j] != NULL &&
            vecMemListPtr->isrParamsArray[j]->isrFuncPtr != NULL)
        {
            if (vecMemListPtr->isrParamsArray[j] == isrParams)
            {
                break;
            }
            else
            {
                prev = vecMemListPtr->isrParamsArray[j];
            }
        }
    }

    if(j == PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        /* not found , return OK !! */
        /* nothing more to clean ! */
        return GT_OK;
    }


    if(isrParams)
    {
        /* Delete the node from the linked-list.    */
        isrParams->cookie = NULL;
        isrParams->isrFuncPtr = NULL;
        isrParams->next = NULL;
    }

    /* look for next element in the linked list  */
    for (; j < PRV_CPSS_MAX_PP_DEVICES_CNS; j++)
    {
        if (vecMemListPtr->isrParamsArray[j] != NULL &&
            vecMemListPtr->isrParamsArray[j]->isrFuncPtr != NULL)
            break;
    }

    if (j == PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        /* deleted element is in the tail of the linked list */
        if (prev != NULL)
        {
            prev->next = NULL;
        }
        else
        {
            vecMemListPtr->first = NULL;
        }
    }
    else
    {
        if (prev != NULL)
        {
            prev->next = vecMemListPtr->isrParamsArray[j];
        }
        else
        {
            vecMemListPtr->first = vecMemListPtr->isrParamsArray[j];
        }
    }

    /* cpssDrv support for "shutdown" of devices */
    if(vecMemListPtr->first == NULL)
    {
        /* we need to free the memory , in order to support "shutdown" of
           devices -- see the use of "osMalloc" instead of "osStaticMalloc"
        */
        for (k = 0; k < PRV_CPSS_MAX_PP_DEVICES_CNS; k++)
        {
            if(vecMemListPtr->isrParamsArray[k])
                cpssOsFree(vecMemListPtr->isrParamsArray[k]);
        }

        cpssOsFree(vecMemListPtr);

        prvCpssDrvComIntSvcRtnDb.intVecMap[i] = NULL;
    }

    return GT_OK;
}


/*******************************************************************************
* prvCpssDrvInterruptScanInit
*
* DESCRIPTION:
*       This function initializes the Devices interrupt data structures for
*       isr interrupt scan operations.
*
* INPUTS:
*       intrScanArrayLen- intrScanArray Length.
*       intrScanArray   - Array of hierarchy tree of interrupt scan struct to
*                         initialize.
*       scanArrayIdx    - start index in scan array to use.
*       subIntrScan     - Pointer to be assigned with scan tree.
*
* OUTPUTS:
*       scanArrayIdx    - New start index in scan array to use.
*       subIntrScan     - Pointer to scan tree created.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvInterruptScanInit
(
    IN      GT_U32          intrScanArrayLen,
    IN      const PRV_CPSS_DRV_INTERRUPT_SCAN_STC  *intrScanArray,
    INOUT   GT_U32          *scanArrayIdx,
    INOUT   PRV_CPSS_DRV_INTERRUPT_SCAN_STC  **subIntrScan
)
{
    PRV_CPSS_DRV_INTERRUPT_SCAN_STC *intrScan;
    GT_STATUS       status;
    GT_U8           i;

    if(*scanArrayIdx == intrScanArrayLen)
    {
        DBG_INFO(("Mismatch between array and hierarchy struct"));
        return GT_FAIL;
    }

    intrScan = cpssOsMalloc(sizeof(PRV_CPSS_DRV_INTERRUPT_SCAN_STC));
    if (intrScan == NULL)
    {
        return (GT_OUT_OF_CPU_MEM);
    }

    *subIntrScan = intrScan;

    cpssOsMemCpy(intrScan,
                 &(intrScanArray[(*scanArrayIdx)]),
                 sizeof(PRV_CPSS_DRV_INTERRUPT_SCAN_STC));

    if(intrScan->subIntrListLen > 0)
    {
        intrScan->subIntrScan =
            cpssOsMalloc(sizeof(PRV_CPSS_DRV_INTERRUPT_SCAN_STC*) *
                                         (intrScan->subIntrListLen));

        for(i = 0; i < intrScan->subIntrListLen; i++)
        {
            (*scanArrayIdx)++;
            status = prvCpssDrvInterruptScanInit(intrScanArrayLen,intrScanArray,
                                       scanArrayIdx,
                                       &(intrScan->subIntrScan[i]));
            if(status != GT_OK)
            {
                DBG_INFO(("Scan Init error.\n"));
                return status;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* interruptScanFree
*
* DESCRIPTION:
*       This function Frees the Device's interrupt data structures
*
* INPUTS:
*       subIntrScan     - Pointer to be assigned with scan tree.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssDrvInterruptScanFree
(
    IN   PRV_CPSS_DRV_INTERRUPT_SCAN_STC  *subIntrScan
)
{
    GT_U8           i;
    PRV_CPSS_DRV_INTERRUPT_SCAN_STC  **iter;

    if(subIntrScan == NULL)
    {
        return;
    }

    if(subIntrScan->subIntrScan != NULL)
    {
        iter = subIntrScan->subIntrScan;
        for(i = 0; i < subIntrScan->subIntrListLen; i++)
        {
            if (*iter != NULL)
            {
                prvCpssDrvInterruptScanFree(*iter);
            }
            iter++;
        }
    }

    FREE_PTR_MAC(subIntrScan->subIntrScan);
    FREE_PTR_MAC(subIntrScan);

    return;
}


/*******************************************************************************
* prvCpssDrvInterruptCauseRegsClear
*
* DESCRIPTION:
*       This function clears the interrupt cause registers for the given device.
*
* INPUTS:
*       devNum          - The device number to scan.
*       portGroupId          - The portGroupId. for multi-port-groups device.
*       pIntScanNode    - Struct for accessing interrupt register in hierarchy
*                         tree.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvInterruptCauseRegsClear
(
    IN GT_U8               devNum,
    IN GT_U32              portGroupId,
    IN PRV_CPSS_DRV_INTERRUPT_SCAN_STC      *pIntScanNode
)
{
    GT_U32      intCause;       /* A temp var. to hold the interrupt    */
                                /* cause reg. value.                    */
    GT_STATUS   retVal;         /* Function's return value.             */
    GT_U32      i;

    if(pIntScanNode->isGpp == GT_TRUE)
    {
        return GT_OK;
    }

    /* Clear all sub registers before clearing this level.  */
    for(i = 0; i < pIntScanNode->subIntrListLen; i++)
    {
        retVal = prvCpssDrvInterruptCauseRegsClear(devNum,portGroupId,pIntScanNode->subIntrScan[i]);
        if(retVal != GT_OK)
        {
            return retVal;
        }
    }

    /* Clear the interrupt cause register.                  */
    pIntScanNode->pRegReadFunc(devNum,portGroupId,pIntScanNode->causeRegAddr,&intCause);

    /* Clear the interrupts (if needed).                    */
    if(pIntScanNode->rwBitMask != 0)
    {
        pIntScanNode->pRegWriteFunc(devNum,portGroupId,pIntScanNode->causeRegAddr,0);
    }

    return GT_OK;
}


/*******************************************************************************
* interruptMainSr
*
* DESCRIPTION:
*       This is the main interrupt service routine, from which the other
*       interrupt SRs are called according to the devices types.
*
* INPUTS:
*       intVecMapIdx - An index to the prvCpssDrvComIntSvcRtnDb.intVecMap list,
*                      identifying the list of potential devices that could have
*                      caused the interrupt.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID interruptMainSr
(
    IN GT_U8 intVecMapIdx
)
{
    PRV_CPSS_DRV_ISR_PARAMS_STC   *pIsrParams;
    GT_32           intKey = 0; /* Unlocking.                           */
    void            *cookie;
    PRV_CPSS_DRV_INT_ISR_COOKIE_STC_XXX        *isrCookieInfoPtr;   /*pointer to format of the cookie of interrupt*/

    /* this is the interrupt lock, but is't no use now. zhangdi@autelan.com 2012-06-21 */
    cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &intKey);

    pIsrParams = prvCpssDrvComIntSvcRtnDb.intVecMap[intVecMapIdx]->first;

    while(pIsrParams != NULL)
    {
		#if 0
		/* For debug the polling dev & group */
        osPrintfDbg("*****@@@@@@@@@@@*********** pIsrParams 0x%x \n", pIsrParams);		
        isrCookieInfoPtr = pIsrParams->cookie ;
      	osPrintfDbg("*****^^^^^  @@@@@@@@@@@*********** :: dev %d group %d\n", isrCookieInfoPtr->devNum, isrCookieInfoPtr->portGroupId);				
        #endif
        cookie = pIsrParams->cookie;

        /* invoke ISR untill all interrupts have been treated */
        while((pIsrParams->isrFuncPtr)(cookie));

        /* proceed to the next device */
        pIsrParams = pIsrParams->next;
    }

    cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);

    return;
}


/*******************************************************************************
* prvCpssDrvIntEvReqDrvnScan
*
* DESCRIPTION:
*       This function scans a given interrupt cause register, and performs all
*       needed interrupt nodes queuing.
*
* INPUTS:
*       devNum         - The device number to scan.
*       portGroupId          - The portGroupId. for multi-port-groups device.
*       intNodesPool   - Pool of interrupt nodes belonging to the scanned
*                        device.
*       intMaskShadow  - Interrupt mask registers shadow array.
*       pIntScanNode   - Structure for accessing interrupt register in hierarchy
*                         tree.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0 - always.
*
* COMMENTS:
*       This routine is used on systems configured to poll events.
*
*******************************************************************************/
GT_U8 prvCpssDrvIntEvReqDrvnScan
(
    GT_U8               devNum,
    GT_U32              portGroupId,
    PRV_CPSS_DRV_EV_REQ_NODE_STC            *intNodesPool,
    GT_U32              *intMaskShadow,
    PRV_CPSS_DRV_INTERRUPT_SCAN_STC      *pIntScanNode
)
{
    PRV_CPSS_DRV_EV_REQ_NODE_STC    *evNodesPool;   /* Event node pool for the device     */
    PRV_CPSS_DRV_INTERRUPT_SCAN_STC *intScanStackPtr[PRV_CPSS_DRV_SCAN_STACK_SIZE_CNS]; /* scan stack      */
    PRV_CPSS_DRV_INTERRUPT_SCAN_STC *currIntScanPtr;/* The current pointer in scan stack  */
    PRV_CPSS_DRV_INTERRUPT_SCAN_STC **intStackTopPtr;/* The top of scan stack pointer     */
    GT_U32              i;             /* Iterator                            */
    GT_U32              tmpCauseBits;  /* Temp to hold the needed cause bits  */
    GT_U32              intMaskBits;   /* The int mask bits to be set to      */
                                       /* 1, at the end of scanning.          */
    GT_U32              intMaskBit;    /* Interrupt mask bit                  */
    GT_U32              *maskShdwValPtr; /* mask shadow value pointer         */
    GT_BOOL             intMasked;      /* Int received while masked          */
    GT_U32              intCause;       /* Interrupt cause register value     */
    GT_U32              newInt;         /* New interrupt was received flag    */

    evNodesPool = intNodesPool;
    newInt = 0;

    /* push */
    intScanStackPtr[0] = pIntScanNode;
    intStackTopPtr = &intScanStackPtr[0];

    DBG_LOG((">>> INTERRUPT dev %d, scan registers...\n",devNum,2,3,4,5,6));

    while (intStackTopPtr >= &intScanStackPtr[0])
    {
        /* pop the interrupt scan node */
        currIntScanPtr = *intStackTopPtr;
        intStackTopPtr--;

        if(currIntScanPtr->isGpp == GT_TRUE)
        {
            DBG_LOG(("GPP interrupt id %d causeReg 0x%X bitNum %d\n",
                 currIntScanPtr->gppId, currIntScanPtr->causeRegAddr,
                 currIntScanPtr->bitNum, 4,5,6));

            /* This is a Gpp interrupt, call the attached function. */
            currIntScanPtr->gppFuncPtr(devNum,currIntScanPtr->gppId);
            newInt = 1;
            continue;
        }

        /* Read the interrupt cause register.           */
        currIntScanPtr->pRegReadFunc(devNum, portGroupId,currIntScanPtr->causeRegAddr,
                                     &intCause);

        maskShdwValPtr = &intMaskShadow[currIntScanPtr->startIdx >> 5];

        /* Scan local interrupts (non-summary bits).    */
        tmpCauseBits = intCause & currIntScanPtr->nonSumBitMask;
        intMaskBits = 0;

        DBG_LOG(("intCause 0x%X=0x%X (non-masked 0x%X  , nonSumBitMask=0x%X , shadowMask=0x%X)\n",
                 currIntScanPtr->causeRegAddr,
                 intCause, tmpCauseBits,
                 currIntScanPtr->nonSumBitMask,
                 *maskShdwValPtr,6));
        
#ifdef AX_FULL_DEBUG
		if(0 != intCause)
			osPrintfDbg("prvCpssDrvIntEvReqDrvnScan::dev %d reg %#x=%#x (non-masked %#x)\n",
				devNum,currIntScanPtr->causeRegAddr,intCause, tmpCauseBits);
#endif

        if (tmpCauseBits != 0)
        {
            tmpCauseBits >>= currIntScanPtr->startIdx & 0x1f;

            for (i = currIntScanPtr->startIdx;
                 (i <= currIntScanPtr->endIdx && tmpCauseBits);
                 i++, tmpCauseBits >>= 1)
            {			
                if (tmpCauseBits & 1)
                {				
                     DBG_LOG(("new event: dev %d uniEv %-50s indx %d\n",devNum,
                             (evNodesPool[i].uniEvCause >= CPSS_UNI_EVENT_COUNT_E) ?
                             (GT_U32)"RESERVED" :
                             (GT_U32)uniEvName[evNodesPool[i].uniEvCause], i, 4,5,6));
                    
                #ifndef  __AX_PLATFORM__
					extern char * evReason[22];
					extern GT_STATUS cpssDxChSecurBreachMsgGet(GT_U8 devNum,BRG_SECUR_BREACH_MSG_STC * msgStc);
					BRG_SECUR_BREACH_MSG_STC msgStc;

					cpssOsMemSet(&msgStc, 0, sizeof(BRG_SECUR_BREACH_MSG_STC));
					if(CPSS_PP_EB_SECURITY_BREACH_UPDATE_E == evNodesPool[i].uniEvCause) {
						cpssDxChSecurBreachMsgGet(devNum,&msgStc);
						osPrintfDbg("CPSS_PP_EB_SECURITY_BREACH_UPDATE_E event\n");
						osPrintfDbg(" <%d,%d> vid %d code %d %02x:%02x:%02x:%02x:%02x:%02x\r\n",	\
							devNum,msgStc.port,msgStc.vlan,msgStc.code,		  \
							msgStc.macSa.arEther[0],msgStc.macSa.arEther[1],  \
							msgStc.macSa.arEther[2],msgStc.macSa.arEther[3],  \
							msgStc.macSa.arEther[4],msgStc.macSa.arEther[5]);
						/*osPrintf("type of security breach reported :: %s\n",(GT_U32)evReason[msgStc.code]);*/
							
					}
					
					/* handle SDMA Rx Error event */
					if(IS_SDMA_RX_ERR_INT_CAUSE(evNodesPool[i].uniEvCause)) {
						dxChNetIfSdmaRxResErrPacketHandle(devNum,evNodesPool[i].uniEvExt);
						cpssDrvIntScanCpuRxErrMaskShadowSet(devNum, evNodesPool[i].uniEvExt, GT_FALSE); 
					}
                #endif
                   
                    /* If a callBack routine is registered for the event    */
                    /* invoke it.                                           */
                    if (evNodesPool[i].intCbFuncPtr != NULL)
                    {
                        evNodesPool[i].intCbFuncPtr(devNum, i);
                    }

                    /* check with the "unified event causes" --
                       if it is Tx completed event */
                    if (CPSS_PP_TX_BUFFER_QUEUE_E == evNodesPool[i].uniEvCause)
                    {
                        /* invoke the Tx end call back from the array of
                           call backs */
                        if(prvCpssDrvComIntSvcRtnDb.txEndCb[devNum])
                        {
                            (prvCpssDrvComIntSvcRtnDb.txEndCb[devNum])(devNum,
                                                    (GT_U8)evNodesPool[i].uniEvExt);
                        }

                        newInt = 1;
                        continue;
                    }

                    /* This is a regular event */
                    if (NULL != prvCpssDrvEvReqQUserHndlGet(evNodesPool[i].uniEvCause))
                    {
                    	#ifdef AX_FULL_DEBUG
                    	osPrintfDbg("prvCpssDrvIntEvReqDrvnScan::prvCpssDrvEvReqQUserHndlGet %d ok\n", \
												evNodesPool[i].uniEvCause);
						#endif
                        /* insert the event into queue */
                        intMaskBit = 1 << (i & 0x1f);
                        intMaskBits |= intMaskBit;

                        if (*maskShdwValPtr & intMaskBit)
                        {
                            intMasked = GT_FALSE;
                            newInt = 1;
                        }
						#ifndef CPSS_PACKET_RX_INT
						else if(IS_SDMA_RX_PKT_INT_CAUSE(evNodesPool[i].uniEvCause))
						{
							intMasked = GT_FALSE;
							newInt = 1;
						}
						#endif
                        else
                        {
                            intMasked = GT_TRUE;
                        }

                        prvCpssDrvEvReqQInsert(evNodesPool, i, intMasked);
                    }
                    else
                    {
                        newInt = 1;
                        DBG_LOG(("...unbinded event, evNodesPool[%d].uniEvCause = 0x%x, "
                                 "intCause 0x%X=0x%X, intMask 0x%X=0x%X !!\n",
                                 i, evNodesPool[i].uniEvCause,
                                 currIntScanPtr->causeRegAddr, intCause,
                                 currIntScanPtr->maskRegAddr, *maskShdwValPtr));
                    }
                }
            }

            /* mask the received interrupt bits if needed */
            *maskShdwValPtr &= ~(intMaskBits & currIntScanPtr->maskRcvIntrEn);

            DBG_LOG(("mask interrupts addr 0x%X value 0x%X\n",
                     currIntScanPtr->maskRegAddr, *maskShdwValPtr,3,4,5,6));

            currIntScanPtr->pRegWriteFunc(devNum,portGroupId,
                                          currIntScanPtr->maskRegAddr,
                                          *maskShdwValPtr);
        }

        /* Scan summary interrupt bits.                 */
        tmpCauseBits = (intCause & ~(currIntScanPtr->nonSumBitMask) &
                        *maskShdwValPtr);

        if(tmpCauseBits != 0)
        {
            for(i = 0; i < currIntScanPtr->subIntrListLen; i++)
            {
                if (((tmpCauseBits >> ((currIntScanPtr->subIntrScan[i]->bitNum)
                                       & 0x1f)) & 1) == 1)
                {
                    intStackTopPtr++;
                    if (intStackTopPtr > &intScanStackPtr[PRV_CPSS_DRV_SCAN_STACK_LAST_ENTRY_CNS])
                    {
                        DBG_LOG(("scan stack overflow !!\n",1,2,3,4,5,6));
                        return 0;
                    }
                    *intStackTopPtr = currIntScanPtr->subIntrScan[i];
                }
            }
        }

        /* Clear the interrupts (if needed).            */
        if(currIntScanPtr->rwBitMask != 0)
        {
            currIntScanPtr->pRegWriteFunc(devNum,portGroupId,currIntScanPtr->causeRegAddr,
                                        ~(intCause & currIntScanPtr->rwBitMask));
        }
    }

    return (GT_U8)newInt;
}


/*******************************************************************************
* prvCpssDrvInterruptPpTxEndedCbBind
*
* DESCRIPTION:
*       bind a callback function for the device(Pp) for the "tx ended" event
*
* INPUTS:
*       devNum - the device number of the device that received the "tx ended"
*                event
*       txEndedCb - the call back function to be called when the "tx ended"
*                   event occur on the device
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_BAD_PARAM - on bad device number.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvInterruptPpTxEndedCbBind
(
    IN  GT_U8   devNum,
    IN  PRV_CPSS_DRV_COM_INT_TX_ENDED_CB txEndedCb
)
{
    if(devNum >= PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        return GT_BAD_PARAM;
    }

    prvCpssDrvComIntSvcRtnDb.txEndCb[devNum] = txEndedCb;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDrvInterruptsMaskDefSummaryInit
*
* DESCRIPTION:
*       Initialize the interrupt mask default value for polling mode. Only the
*       summary bits need to be unmasked, cause bits are unmasked by user app
*       on event bind
*
* INPUTS:
*       devMaskRegSize      - The size of interrupt mask register array.
*       maskRegMapArr       - The interrupt mask register map.
*       intScanRootPtr      - The interrupt information hierarchy tree root.
*
* OUTPUTS:
*       maskRegDefArr       - The interrupt mask default value.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvInterruptsMaskDefSummaryInit
(
    IN GT_U32                   devMaskRegSize,
    IN GT_U32                   *maskRegMapArr,
    IN GT_U32                   *maskRegDefArr,
    OUT PRV_CPSS_DRV_INTERRUPT_SCAN_STC          *intScanRootPtr
)
{
    PRV_CPSS_DRV_INTERRUPT_SCAN_STC  *intScanStackPtr[PRV_CPSS_DRV_SCAN_STACK_SIZE_CNS]; /* scan stack         */
    PRV_CPSS_DRV_INTERRUPT_SCAN_STC  *currIntScanPtr;    /* The current pointer in scan stack */
    PRV_CPSS_DRV_INTERRUPT_SCAN_STC  **intStackTopPtr;   /* The top of scan stack pointer     */
    GT_U32          i;                  /* Iterator                          */
    GT_U32          maskRegIdx;         /* The mask register index           */
    GT_BOOL         regFound;           /* A flag for register addr search   */

    /* clear the default mask value */
    cpssOsBzero((char *)maskRegDefArr, devMaskRegSize * sizeof(maskRegDefArr[0]));

    /* push the first scan element into the stack */
    intScanStackPtr[0] = intScanRootPtr;
    intStackTopPtr     = &intScanStackPtr[0];

    while (intStackTopPtr >= &intScanStackPtr[0])
    {
        /* pop the interrupt scan node */
        currIntScanPtr = *intStackTopPtr;
        intStackTopPtr--;

        if (0 == currIntScanPtr->subIntrListLen)
        {
            /* scan element does not have sub interrupts */
            continue;
        }
        else
        {
            regFound = GT_FALSE;

            /* search for the register address */
            for (maskRegIdx = 0; maskRegIdx < devMaskRegSize; maskRegIdx++)
            {
                if (maskRegMapArr[maskRegIdx] == currIntScanPtr->maskRegAddr)
                {
                    regFound = GT_TRUE;
                    break;
                }
            }

            if (GT_FALSE == regFound)
            {
                /* register address was not found, this is an error */
                return GT_FAIL;
            }

            /* for all sub interrupts, update the mask default value */
            for (i = 0; i < currIntScanPtr->subIntrListLen; i++)
            {
                maskRegDefArr[maskRegIdx] |=
                                    1 << currIntScanPtr->subIntrScan[i]->bitNum;

                intStackTopPtr++;

                if (intStackTopPtr > &intScanStackPtr[PRV_CPSS_DRV_SCAN_STACK_LAST_ENTRY_CNS])
                {
                    /* stack is too small */
                    return GT_FAIL;
                }
                *intStackTopPtr = currIntScanPtr->subIntrScan[i];
            }
        }
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDrvComIntSvcRtnDbRelease
*
* DESCRIPTION:
*       private (internal) function to release the DB of the interrupts.
*       NOTE: function  'free' the allocated memory.
*             and restore DB to 'pre-init' state
*
*  APPLICABLE DEVICES: ALL
*
* INPUTS:
*       none
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDrvComIntSvcRtnDbRelease
(
    void
)
{
    GT_STATUS rc;

    cpssOsMemSet(&prvCpssDrvComIntSvcRtnDb,0,sizeof(prvCpssDrvComIntSvcRtnDb));

    rc = prvCpssDrvComIntEvReqQueuesDbRelease();
    if (rc != GT_OK)
    {
        return rc;
    }

    prvCpssCommonAfterDbRelease = GT_TRUE;

    return GT_OK;
}

