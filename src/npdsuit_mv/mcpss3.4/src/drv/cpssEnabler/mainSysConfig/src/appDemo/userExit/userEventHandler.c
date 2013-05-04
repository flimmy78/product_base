/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* userEventHandler.c
*
* DESCRIPTION:
*       This module defines the Application Demo requested event (polling) mode
*       process creation and event handling.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1.5 $
*******************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
    
#include <sys/un.h>
#include <sys/socket.h>

#include <appDemo/os/appOs.h>
#include <appDemo/userExit/userEventHandler.h>
#include <appDemo/sysHwConfig/appDemoDb.h>
#include <cpss/generic/events/private/prvCpssGenEvReq.h>


#include <gtExtDrv/drivers/gtIntDrv.h>

#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>
#ifndef __AX_PLATFORM__
#include <cpss/driver/interrupts/cpssDrvComIntSvcRtn.h>
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <errno.h>
#endif

#ifdef SHARED_MEMORY

        extern int         multiProcessAppDemo;

#endif

/* global variables used in single Rx task mode */
/* is single Rx task mode enabled */
static GT_BOOL singleTaskRxEnable = GT_FALSE;
/* is random distribution of packets to msgQs enabled */
static GT_BOOL singleTaskRxRandomEnable = GT_FALSE;
/* number of treat Rx tasks */
static GT_U32  treatTasksNum = 2;
static CPSS_OS_MSGQ_ID singleRxMsgQIds[8];
CPSS_OS_MUTEX rxMutex;

#ifdef EX_FAMILY
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgFdb.h>
#endif

#include <galtisAgent/wrapCpss/Gen/Events/wrapCpssGenEvents.h>
#ifndef __AX_PLATFORM__
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cos/cpssCosTypes.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChPacketHandling.h>
#endif

/* The next define can be uncommented for debug use only !! */
/* #define APPDEMO_REQ_MODE_DEBUG */

#ifdef APPDEMO_REQ_MODE_DEBUG
  static GT_CHAR * uniEvName[CPSS_UNI_EVENT_COUNT_E] = {UNI_EV_NAME};
  #ifdef ASIC_SIMULATION
    #define DBG_LOG(x)  osPrintf x
  #else
    extern int  logMsg (char *fmt, int arg1, int arg2, int arg3,
                        int arg4, int arg5, int arg6);
    #define DBG_LOG(x)  osPrintf x
  #endif
  #define DBG_INFO(x)   osPrintf x
#else
  #define DBG_INFO(x)
  #define DBG_LOG(x)
#endif

/* full queue process quota */
#define SDMA_RX_QUEUE0_QUOTA		(~0UL)
#define SDMA_RX_QUEUE1_QUOTA		(~0UL)
#define SDMA_RX_QUEUE2_QUOTA		(~0UL)
#define SDMA_RX_QUEUE3_QUOTA		(~0UL)
#define SDMA_RX_QUEUE4_QUOTA		(~0UL)
#define SDMA_RX_QUEUE5_QUOTA		(~0UL)
#define SDMA_RX_QUEUE6_QUOTA		(~0UL)
#define SDMA_RX_QUEUE7_QUOTA		(~0UL)

unsigned int sdmaRxQuota[NUM_OF_RX_QUEUES] = {	\
	SDMA_RX_QUEUE0_QUOTA,	\
	SDMA_RX_QUEUE1_QUOTA,	\
	SDMA_RX_QUEUE2_QUOTA,	\
	SDMA_RX_QUEUE3_QUOTA,	\
	SDMA_RX_QUEUE4_QUOTA,	\
	SDMA_RX_QUEUE5_QUOTA,	\
	SDMA_RX_QUEUE6_QUOTA,	\
	SDMA_RX_QUEUE7_QUOTA
} ;

#define SDMA_RX_THRD_PRIO 160

/* Local routines */
static unsigned __TASKCONV appDemoEvHndlr(GT_VOID * param);
static unsigned __TASKCONV appDemoSingleRxEvHndlr(GT_VOID * param);
static unsigned __TASKCONV appDemoSingleRxTreatTask(GT_VOID * param);

/* Local Defines */
#define EV_HANDLER_MAX_PRIO 200

/* appDemoPpConfigList - Holds the Pp's configuration parameters.   */
extern APP_DEMO_PP_CONFIG appDemoPpConfigList[PRV_CPSS_MAX_PP_DEVICES_CNS];

#if (defined CHX_FAMILY)
    #define DXCH_CODE
#endif /* (defined CHX_FAMILY) */

#if (defined EXMXPM_FAMILY)
    #define PM_CODE
#endif /* (defined EXMXPM_FAMILY) */

#ifdef DXCH_CODE
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>

GT_STATUS appDemoDxChNetRxPktHandle(IN GT_U8 devNum,
                                    IN GT_U8 queueIdx);
GT_STATUS appDemoDxChNetRxPktGet(IN GT_U8 devNum,
                                 IN GT_U8 queueIdx,
                                 IN CPSS_OS_MSGQ_ID msgQId);
GT_STATUS appDemoDxChNetRxPktTreat(IN APP_DEMO_RX_PACKET_PARAMS *rxParamsPtr);

#endif /*DXCH_CODE*/

#ifdef PM_CODE
#include <cpss/exMxPm/exMxPmGen/networkIf/cpssExMxPmNetIf.h>

GT_STATUS appDemoExMxPmNetRxPktHandle(IN GT_U8 devNum,
                                      IN GT_U8 queueIdx);
GT_STATUS appDemoExMxPmNetRxPktGet(IN GT_U8 devNum,
                                   IN GT_U8 queueIdx,
                                   IN CPSS_OS_MSGQ_ID msgQId);
GT_STATUS appDemoExMxPmNetRxPktTreat(IN APP_DEMO_RX_PACKET_PARAMS *rxParamsPtr);

#endif /*PM_CODE*/

extern GT_STATUS appDemoGenNetRxPktHandle(IN GT_U8 devNum,
                                          IN GT_U8 queueIdx);

extern GT_STATUS cpssEnGenAuMsgHandle(IN GT_U8  devNum,
                                      IN GT_U32 evExtData);

/*
 * typedef: struct EV_HNDLR_PARAM
 *
 * Description:
 *
 * Fields:
 *   evHndl         - The process unified event handle (got from appDemoCpssEventBind).
 *   hndlrIndex     - The process number
 */
typedef struct
{
    GT_U32              evHndl;
    GT_U32              hndlrIndex;
} EV_HNDLR_PARAM;

/*
 * Description:
 *
 * Fields:
 *   devNum        - The device number
 *   taskName     - The task name
 */
typedef struct
{
    GT_U8				devNum;
    GT_U8				taskName[30];
} SDMA_RxPoll_PARAM;



static GT_STATUS prvUniEvMaskAllSet
(
    IN  CPSS_UNI_EV_CAUSE_ENT       cpssUniEventArr[],
    IN  GT_U32                      arrLength,
    IN  CPSS_EVENT_MASK_SET_ENT     operation
);

static GT_STATUS cpssEnRxPacketGet
(
    IN GT_U8                devNum,
    IN GT_U8                queue
);
static GT_STATUS appDemoGetMaxPackets
(
    IN GT_U8                devNum,
    IN GT_U8                queue,
    IN GT_U32               maxPacket
);

#define MAX_NUM_DEVICES 128

/* Event Counter increment function. Implemented in Galtis*/
EVENT_COUNTER_INCREMENT_FUNC *eventIncrementFunc = NULL;

/* flag to enable the printings of 'link change' --
    by default not enabled because it may cause RDE tests to fail due to the
    printings , tests that cause link change.
*/
#ifdef _WIN32
static GT_U32   printLinkChangeEnabled = 1;
#else  /*_WIN32*/
static GT_U32   printLinkChangeEnabled = 0;
#endif /*_WIN32*/

/* flag that state the tasks may process events that relate to AUQ messages.
    this flag allow us to stop processing those messages , by that the AUQ may
    be full, or check AU storm prevention, and other.

    use function appDemoAllowProcessingOfAuqMessages(...) to set this flag
*/
static GT_BOOL  allowProcessingOfAuqMessages = GT_TRUE;

/* The interrupt locking is needed for mutual exclusion prevention between   */
/* ISR and user task on the event queues.                                   */
#define INT_LOCK(key)                                                   \
    key = 0;                                                            \
    osTaskLock();                                                       \
    extDrvSetIntLockUnlock(INTR_MODE_LOCK, &(key))

#define INT_UNLOCK(key)                                                 \
    extDrvSetIntLockUnlock(INTR_MODE_UNLOCK, &(key));                   \
    osTaskUnLock()


#ifndef __AX_PLATFORM__
/* Ethernet port link UP/DOWN Event callbacks */
///typedef int (*NPD_ETH_PORT_NOTIFIER_FUNC)(unsigned int,unsigned int);    /*luoxun -- cpss1.3*/

extern NPD_ETH_PORT_NOTIFIER_FUNC portNotifier;
extern unsigned int auteware_product_id;
#if 1
extern unsigned int (*npd_get_global_index_by_devport)
(
	unsigned char 		devNum,
	unsigned char 		portNum,
	unsigned int		*eth_g_index
);
#else
extern unsigned int npd_get_global_index_by_devport
(
	unsigned char 		devNum,
	unsigned char 		portNum,
	unsigned int		*eth_g_index
);
#endif
#endif


/*******************************************************************************
* appDemoPrintLinkChangeFlagSet
*
* DESCRIPTION:
*       function to allow set the flag of : printLinkChangeEnabled
*
* INPUTS:
*       enable - enable/disable the printings of 'link change'
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoPrintLinkChangeFlagSet
(
    IN GT_U32   enable
)
{
    printLinkChangeEnabled = enable;
    return GT_OK;
}

/*******************************************************************************
* appDemoAllowProcessingOfAuqMessages
*
* DESCRIPTION:
*       function to allow set the flag of : allowProcessingOfAuqMessages
*
* INPUTS:
*       enable - enable/disable the processing of the AUQ messages
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*    flag that state the tasks may process events that relate to AUQ messages.
*    this flag allow us to stop processing those messages , by that the AUQ may
*    be full, or check AU storm prevention, and other.
*
*******************************************************************************/
GT_STATUS   appDemoAllowProcessingOfAuqMessages
(
    GT_BOOL     enable
)
{
    GT_U8   dev;

    if(enable == GT_TRUE && allowProcessingOfAuqMessages == GT_FALSE)
    {
        /* move from no learn to learn */
        /* start processing the messages stuck in the queue */

        for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(appDemoPpConfigDevAmount); dev++)
        {
            if(appDemoPpConfigList[dev].valid == GT_FALSE)
            {
                continue;
            }

            cpssEnGenAuMsgHandle(dev,
                0);/*unused param*/
        }
    }

    allowProcessingOfAuqMessages = enable;

    return GT_OK;
}

static CPSS_UNI_EV_CAUSE_ENT evHndlr0CauseDefaultArr[] = APP_DEMO_P0_UNI_EV_DEFAULT;
static CPSS_UNI_EV_CAUSE_ENT evHndlr1CauseDefaultArr[] = APP_DEMO_P1_UNI_EV_DEFAULT;
static CPSS_UNI_EV_CAUSE_ENT evHndlr2CauseDefaultArr[] = APP_DEMO_P2_UNI_EV_DEFAULT;
static CPSS_UNI_EV_CAUSE_ENT evHndlr3CauseDefaultArr[] = APP_DEMO_P3_UNI_EV_DEFAULT;
static CPSS_UNI_EV_CAUSE_ENT evHndlr4CauseDefaultArr[] = APP_DEMO_P4_UNI_EV_DEFAULT;
static CPSS_UNI_EV_CAUSE_ENT evHndlr5CauseDefaultArr[] = APP_DEMO_P5_UNI_EV_DEFAULT;
static CPSS_UNI_EV_CAUSE_ENT evHndlr6CauseDefaultArr[] = APP_DEMO_P6_UNI_EV_DEFAULT;
static CPSS_UNI_EV_CAUSE_ENT evHndlr7CauseDefaultArr[] = APP_DEMO_P7_UNI_EV_DEFAULT;
static CPSS_UNI_EV_CAUSE_ENT evHndlr8CauseDefaultArr[] = APP_DEMO_P8_UNI_EV_DEFAULT;
static CPSS_UNI_EV_CAUSE_ENT evHndlr9CauseDefaultArr[] = APP_DEMO_P9_UNI_EV_DEFAULT;

static GT_U32 evHndlr0CauseDefaultArrSize = sizeof(evHndlr0CauseDefaultArr)/sizeof(evHndlr0CauseDefaultArr[0]);
static GT_U32 evHndlr1CauseDefaultArrSize = sizeof(evHndlr1CauseDefaultArr)/sizeof(evHndlr1CauseDefaultArr[0]);
static GT_U32 evHndlr2CauseDefaultArrSize = sizeof(evHndlr2CauseDefaultArr)/sizeof(evHndlr2CauseDefaultArr[0]);
static GT_U32 evHndlr3CauseDefaultArrSize = sizeof(evHndlr3CauseDefaultArr)/sizeof(evHndlr3CauseDefaultArr[0]);
static GT_U32 evHndlr4CauseDefaultArrSize = sizeof(evHndlr4CauseDefaultArr)/sizeof(evHndlr4CauseDefaultArr[0]);
static GT_U32 evHndlr5CauseDefaultArrSize = sizeof(evHndlr5CauseDefaultArr)/sizeof(evHndlr5CauseDefaultArr[0]);
static GT_U32 evHndlr6CauseDefaultArrSize = sizeof(evHndlr6CauseDefaultArr)/sizeof(evHndlr6CauseDefaultArr[0]);
static GT_U32 evHndlr7CauseDefaultArrSize = sizeof(evHndlr7CauseDefaultArr)/sizeof(evHndlr7CauseDefaultArr[0]);
static GT_U32 evHndlr8CauseDefaultArrSize = sizeof(evHndlr8CauseDefaultArr)/sizeof(evHndlr8CauseDefaultArr[0]);
static GT_U32 evHndlr9CauseDefaultArrSize = sizeof(evHndlr9CauseDefaultArr)/sizeof(evHndlr9CauseDefaultArr[0]);


static CPSS_UNI_EV_CAUSE_ENT evHndlr6CauseSingleTaskRxArr[] = APP_DEMO_P6_UNI_EV_SINGLE_RX_TASK_MODE;
static GT_U32 evHndlr6CauseSingleTaskRxArrSize = sizeof(evHndlr6CauseSingleTaskRxArr)/sizeof(evHndlr6CauseSingleTaskRxArr[0]);

/*******************************************************************************
* appDemoEventRequestDrvnModeInit
*
* DESCRIPTION:
*       This routine spawns the App Demo event handlers.
*
* INPUTS:
*       None.
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
GT_STATUS appDemoEventRequestDrvnModeInit
(
    IN GT_VOID
)
{
    GT_STATUS rc;              /* The returned code            */
    GT_TASK   eventHandlerTid; /* The task Tid                 */
    GT_U32    i;               /* Iterator                     */
    GT_CHAR   name[30];        /* The task/msgQ name           */
    GT_32     intKey;          /* The interrupt lock key       */
    GT_U32    value;
    unsigned (__TASKCONV *start_addr)(GT_VOID*);

    CPSS_UNI_EV_CAUSE_ENT *evHndlrCauseAllArr[APP_DEMO_PROCESS_NUMBER];
    GT_U32 evHndlrCauseAllArrSize[APP_DEMO_PROCESS_NUMBER];

    /* Note - should be only static (global), cannot resides in stack! */
    static EV_HNDLR_PARAM taskParamArr[APP_DEMO_PROCESS_NUMBER];

    if (appDemoDbEntryGet("singleRxTask",&value) == GT_OK)
    {
        singleTaskRxEnable = (value == 1) ? GT_TRUE : GT_FALSE;
    }
    if (appDemoDbEntryGet("singleRxTaskRandom",&value) == GT_OK)
    {
        singleTaskRxRandomEnable = (value == 1) ? GT_TRUE : GT_FALSE;
    }

    /* set default arrays and sizes */
    evHndlrCauseAllArr[0] = evHndlr0CauseDefaultArr;
    evHndlrCauseAllArr[1] = evHndlr1CauseDefaultArr;
    evHndlrCauseAllArr[2] = evHndlr2CauseDefaultArr;
    evHndlrCauseAllArr[3] = evHndlr3CauseDefaultArr;
    evHndlrCauseAllArr[4] = evHndlr4CauseDefaultArr;
    evHndlrCauseAllArr[5] = evHndlr5CauseDefaultArr;
    evHndlrCauseAllArr[6] = evHndlr6CauseDefaultArr;
    evHndlrCauseAllArr[7] = evHndlr7CauseDefaultArr;
    evHndlrCauseAllArr[8] = evHndlr8CauseDefaultArr;
    evHndlrCauseAllArr[9] = evHndlr9CauseDefaultArr;

    evHndlrCauseAllArrSize[0] = evHndlr0CauseDefaultArrSize;
    evHndlrCauseAllArrSize[1] = evHndlr1CauseDefaultArrSize;
    evHndlrCauseAllArrSize[2] = evHndlr2CauseDefaultArrSize;
    evHndlrCauseAllArrSize[3] = evHndlr3CauseDefaultArrSize;
    evHndlrCauseAllArrSize[4] = evHndlr4CauseDefaultArrSize;
    evHndlrCauseAllArrSize[5] = evHndlr5CauseDefaultArrSize;
    evHndlrCauseAllArrSize[6] = evHndlr6CauseDefaultArrSize;
    evHndlrCauseAllArrSize[7] = evHndlr7CauseDefaultArrSize;
    evHndlrCauseAllArrSize[8] = evHndlr8CauseDefaultArrSize;
    evHndlrCauseAllArrSize[9] = evHndlr9CauseDefaultArrSize;

    /* override defaults */
#ifdef SHARED_MEMORY
    if(multiProcessAppDemo == 1)
    {
        evHndlrCauseAllArrSize[1] = 0;
        evHndlrCauseAllArrSize[2] = 0;
        evHndlrCauseAllArrSize[3] = 0;
        evHndlrCauseAllArrSize[4] = 0;
        evHndlrCauseAllArrSize[5] = 0;
        evHndlrCauseAllArrSize[6] = 0;
        evHndlrCauseAllArrSize[7] = 0;
        evHndlrCauseAllArrSize[8] = 0;
    }
    else
#endif
    if (singleTaskRxEnable == GT_TRUE)
    {
        evHndlrCauseAllArrSize[1] = evHndlrCauseAllArrSize[2] = evHndlrCauseAllArrSize[3] =
        evHndlrCauseAllArrSize[4] = evHndlrCauseAllArrSize[5] = 0;

        evHndlrCauseAllArr[APP_DEMO_SINGLE_RX_TASK_NUMBER] = evHndlr6CauseSingleTaskRxArr;
        evHndlrCauseAllArrSize[APP_DEMO_SINGLE_RX_TASK_NUMBER] = evHndlr6CauseSingleTaskRxArrSize;
    }

#if (defined EX_FAMILY) || (defined MX_FAMILY)
    /* re-size array[7] that hold as last event the :
        CPSS_PP_MAC_AGE_VIA_TRIGGER_ENDED_E
        --> because on ExMx the cpss do internal bind to the event
    */
    if(evHndlrCauseAllArrSize[7])
    {
        evHndlrCauseAllArrSize[7]--;
    }
#endif /*(defined EX_FAMILY) || (defined MX_FAMILY)*/

    {
        /* silent the next events that are part of the ' controlled learning' .
           this event cause when we add/delete mac from the FDB and the FDB notify
           us , that the action done ,
           we get those from 4 port groups and it slows the operations */
        if (appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].devFamily ==
            CPSS_PP_FAMILY_DXCH_LION_E)
        {
            for(i = 0 ; i < evHndlrCauseAllArrSize[0];i++)
            {
                if(evHndlr0CauseDefaultArr[i] == CPSS_PP_MAC_NA_LEARNED_E)
                {
                    evHndlr0CauseDefaultArr[i] = evHndlr0CauseDefaultArr[evHndlrCauseAllArrSize[0] - 1];
                    break;
                }
            }

            if(i != evHndlrCauseAllArrSize[0])
            {
                /* we removed CPSS_PP_MAC_NA_LEARNED_E */
                /* so need to decrement the number of elements */
                evHndlrCauseAllArrSize[0] --;
            }

        }
    }

    /* Get event increment CB routine. */
    wrCpssGenEventCountersCbGet(&eventIncrementFunc);

    /* bind the events for all the App-Demo event handlers */

    /* NOTE : MUST lock interrupts from first bind and unmask till end
             of bind and unmask , so we will not loose any event.

       explanation on option to loose interrupt if not locking:
       I will describe loose interrupt of 'link change' for a port that when
       'power up to board' is in state of 'up'.

       assume that the loop in not under 'lock interrupts'

       the event of 'link change' CPSS_PP_PORT_LINK_STATUS_CHANGED_E is in taskParamArr[9]
       and the event of CPSS_PP_PORT_RX_FIFO_OVERRUN_E is in taskParamArr[0]

       BUT those to events are actually in the same register of 'per port' ,
       so if the HW initialized also the 'rx fifo overrun' interrupts , then
       the after the 'bind and unmask' of this event the ISR will read (and clear)
       the register of the 'per port' and will not care about the 'link change'
       event that is now LOST !
    */
    /* lock section to disable interruption of ISR while unmasking events */
    INT_LOCK(intKey);
    for (i = 0; i < APP_DEMO_PROCESS_NUMBER; i++)
    {
        if(evHndlrCauseAllArrSize[i] == 0 || evHndlrCauseAllArr[i][0] == CPSS_UNI_RSRVD_EVENT_E)
            continue;
        taskParamArr[i].hndlrIndex = i;

        DBG_LOG(("appDemoEventRequestDrvnModeInit: subscribed group #%d for %d events\n", i, evHndlrCauseAllArrSize[i], 3, 4, 5, 6));
        printf("\n******appDemoEventRequestDrvnModeInit!!! \n");
        /* call CPSS to bind the events under single handler */
        rc = cpssEventBind(evHndlrCauseAllArr[i],
                           evHndlrCauseAllArrSize[i],
                           &taskParamArr[i].evHndl);
        if (GT_OK != rc)
        {
            INT_UNLOCK(intKey);
            return GT_FAIL;
        }

        /* call the CPSS to enable those interrupts in the HW of the device */
        rc = prvUniEvMaskAllSet(evHndlrCauseAllArr[i],
                                evHndlrCauseAllArrSize[i],
                                CPSS_EVENT_UNMASK_E);
        if (GT_OK != rc)
        {
            INT_UNLOCK(intKey);
            return GT_FAIL;
        }
    }
    INT_UNLOCK(intKey);

    
    if (singleTaskRxEnable == GT_TRUE)
    {
        for (i = 0; i < treatTasksNum; i++)
        {
            /* create msgQ */
            osSprintf(name, "msgQ_%d", i);
            rc = cpssOsMsgQCreate(name,APP_DEMO_MSGQ_SIZE,
                                  sizeof(APP_DEMO_RX_PACKET_PARAMS),
                                  &(singleRxMsgQIds[i]));
            if (rc != GT_OK)
            {
                return rc;
            }

            rc = cpssOsMutexCreate("rxMutex",&rxMutex);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* spawn rx treat task */
            osSprintf(name, "treatRx_%d", i);
            rc = osTaskCreate(name,
                          EV_HANDLER_MAX_PRIO - APP_DEMO_SINGLE_RX_TASK_NUMBER - 1,
                          _8KB,
                          appDemoSingleRxTreatTask,
                          &(singleRxMsgQIds[i]),
                          &eventHandlerTid);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    /* spawn all the event handler processes */
    for (i = 0; i < APP_DEMO_PROCESS_NUMBER; i++)
    {
        if(evHndlrCauseAllArrSize[i] == 0 || evHndlrCauseAllArr[i][0] == CPSS_UNI_RSRVD_EVENT_E)
            continue;
        osSprintf(name, "evHndl_%d", i);

        if (singleTaskRxEnable == GT_TRUE && i == APP_DEMO_SINGLE_RX_TASK_NUMBER)
        {
            start_addr = appDemoSingleRxEvHndlr;
        }
        else
        {
            start_addr = appDemoEvHndlr;
        }
		printf("******* to :appDemoEvHndlr   *********\n");
        rc = osTaskCreate(name,
                          EV_HANDLER_MAX_PRIO - i,
                          _8KB,
                          start_addr,
                          &taskParamArr[i],
                          &eventHandlerTid);
        if (rc != GT_OK)
        {
            return GT_FAIL;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvUniEvMaskAllSet
*
* DESCRIPTION:
*       This routine unmasks all the events according to the unified event list.
*
* INPUTS:
*       cpssUniEventArr - The CPSS unified event list.
*       arrLength   - The unified event list length.
*       operation   - type of operation mask/unmask to do on the events
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL if failed.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvUniEvMaskAllSet
(
    IN  CPSS_UNI_EV_CAUSE_ENT       cpssUniEventArr[],
    IN  GT_U32                      arrLength,
    IN  CPSS_EVENT_MASK_SET_ENT     operation
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    i;              /* Iterator                     */
    GT_U8     dev;            /* Device iterator              */
    GT_32     intKey;         /* The interrupt lock key       */

    /* lock section to disable interruption of ISR while unmasking events */
    INT_LOCK(intKey);

    /* unmask the interrupt */
    for (i = 0; i < arrLength; i++)
    {
        if (cpssUniEventArr[i] > CPSS_UNI_EVENT_COUNT_E) /* last event */
        {
            rc = GT_FAIL;
            goto exit_cleanly_lbl;
        }

        /* unmask the interrupt for all PPs/XBARs/FAs */

        /* assume PRV_CPSS_MAX_PP_DEVICES_CNS >= PRV_CPSS_MAX_XBAR_DEVICES_CNS */
        for (dev = 0; dev < PRV_CPSS_MAX_PP_DEVICES_CNS; dev++)
        {
            /*luoxun --need by autelan?*/
            if(appDemoPpConfigList[dev].valid == GT_FALSE)
            {
                continue;
            }
			#ifndef __AX_PLATFORM__
			if(CPSS_PP_PORT_SYNC_STATUS_CHANGED_E == cpssUniEventArr[i])
				continue;
			/*add by zhubo@autelan.com 2008.6.19*/
			if(CPSS_PP_EB_SECURITY_BREACH_UPDATE_E  == cpssUniEventArr[i])
				continue;
			/* by qinhs@autelan.com 11/07/2008 - no detailed definition in FuncSpec */
			if(CPSS_PP_MISC_ILLEGAL_ADDR_E == cpssUniEventArr[i])
				continue;
			/*add by yinlm@autelan.com 07/30/2009 */			
			if(CPSS_PP_BM_MAX_BUFF_REACHED_E == cpssUniEventArr[i])
				continue;
			if(CPSS_PP_BM_MISC_E == cpssUniEventArr[i])
				continue;
			if(CPSS_PP_PORT_RX_FIFO_OVERRUN_E == cpssUniEventArr[i])
				continue;
			/*
			  * by qinhs@autelan.com 08/12/2009
			  *	for GPP is not used currently and may interfere with unpredictable signals which
			  *   have been found in Weihai Mobile Project
			  */
			if(CPSS_PP_GPP_E == cpssUniEventArr[i]) {
				continue;
			}
			#endif
			#ifndef CPSS_PACKET_RX_INT
			if((CPSS_PP_RX_BUFFER_QUEUE0_E <= cpssUniEventArr[i])&&
				(CPSS_PP_RX_ERR_QUEUE7_E >= cpssUniEventArr[i]))
				continue;
			#endif
			#ifdef __AX_PCI_TEST__
			if((CPSS_PP_RX_ERR_QUEUE0_E <= cpssUniEventArr[i]) && 
				(CPSS_PP_RX_CNTR_OVERFLOW_E >= cpssUniEventArr[i])) 
				continue;
			#endif
            
            rc = cpssEventDeviceMaskSet(dev, cpssUniEventArr[i], operation);
            switch(rc)
            {
                case GT_NOT_INITIALIZED:
                    /* assume there are no FA/XBAR devices in the system ,
                       because there was initialization of the 'phase 1' for any FA/XBAR */

                    /* fall through */
                case GT_BAD_PARAM:
                    /* assume that this PP/FA/XBAR device not exists , so no DB of PP/FA/XBAR devices needed .... */

                    /* fall through */
                case GT_NOT_FOUND:
                    /* this event not relevant to this device */
                    rc = GT_OK;
                    break;

                case GT_OK:
                    break;

                default:
                    /* other real error */
                    goto exit_cleanly_lbl;
            }
        }
    }

exit_cleanly_lbl:
    INT_UNLOCK(intKey);

    return rc;
} /* prvUniEvMaskAllSet */

/*******************************************************************************
* cpssEnFuMsgGet
*
* DESCRIPTION:
*       This routine handles FDB upload (FU) messages.
*
* INPUTS:
*       devNum      - the device number.
*       evExtData   - Unified event additional information
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
static GT_STATUS cpssEnFuMsgGet
(
    IN GT_U8                devNum,
    GT_U32                  evExtData
)
{
    GT_STATUS   rc = GT_NOT_IMPLEMENTED;

    if(IS_API_CH_DEV_MAC(devNum))/* is device support dxch API ? */
    {
#ifdef DXCH_CODE
/*      if(1)*//* 2 queues used *//*
        {
            rc = cpssEnChDxFuMsgHandle(devNum, evExtData);
        }
        else
        {
            rc = cpssEnChDxAuMsgHandle(devNum, evExtData);
        }*/
#endif /*DXCH_CODE*/
    }

    return rc;
}

/*******************************************************************************
* appDemoEnPpEvTreat
*
* DESCRIPTION:
*       This routine handles PP events.
*
* INPUTS:
*       devNum      - the device number.
*       uniEv       - Unified event number
*       evExtData   - Unified event additional information
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
static GT_STATUS appDemoEnPpEvTreat
(
    GT_U8                   devNum,
    GT_U32                  uniEv,
    GT_U32                  evExtData
)
{
    GT_STATUS   rc = GT_OK;
    GT_U8       queue = 0, i = 0;
	unsigned int eth_g_index = 0;
	GT_BOOL		isLinkUp = GT_FALSE, result = GT_FALSE;
#define MV_MAX_HSC_PORT_NUM  (4)
#define MV_HSC_INT_LINK_MSG_INTERVAL	30
	int ii = 0;
	typedef struct _cscdIntLinkStat {
		GT_U8 	devNum;
		GT_U8 	portNum;
		GT_U32	intLinkCnt;
	} cscdIntLinkStat;
	//GT_BOOL		isLinkUp = GT_FALSE;
	struct timespec delta;

    static cscdIntLinkStat	hscPortInfo[MV_MAX_HSC_PORT_NUM] = { \
			{0xFF, 0xFF, ~0UL},{0xFF, 0xFF, ~0UL},{0xFF, 0xFF, ~0UL},{0xFF, 0xFF, ~0UL}};
	static int idlePos = 0;
	/* add for remove the debug of event 117 122 124 (MAC message) zhangdi 2011-09-23 */
	if((uniEv<114)||(uniEv>134))
	{
	    osPrintfDbg("handle event %d on device %d data %d\n",uniEv,devNum,evExtData);
	}
	else
	{
		/* once the broadcast into port, make npd(appEvHndlr0 & intTask0) not grab the CPU. (event 117) */
    	/* zhangdi@autelan.com 2012-03-09 bug: AXSSZFI-722 */		
        osTimerWkAfter(50);		
	}
	
	/* end */
    switch (uniEv)
    {
        case CPSS_PP_RX_BUFFER_QUEUE0_E:
        case CPSS_PP_RX_BUFFER_QUEUE1_E:
        case CPSS_PP_RX_BUFFER_QUEUE2_E:
        case CPSS_PP_RX_BUFFER_QUEUE3_E:
        case CPSS_PP_RX_BUFFER_QUEUE4_E:
        case CPSS_PP_RX_BUFFER_QUEUE5_E:
        case CPSS_PP_RX_BUFFER_QUEUE6_E:
        case CPSS_PP_RX_BUFFER_QUEUE7_E:
                queue = (GT_U8)(uniEv - CPSS_PP_RX_BUFFER_QUEUE0_E);

                rc = cpssEnRxPacketGet(devNum, queue);
				/* add form 275  */
				cpssDrvIntScanCpuRxErrMaskShadowSet(devNum, queue, GT_TRUE);
	
            break;
        /* rx_buffer event doesn't arrive on every rx buffer event,
         * so theoretically under heavy load, there could be scenario,
         * when just rx_error events will signal that there are
         * descriptors to treat in chain
         */
        #if 0
        case CPSS_PP_RX_ERR_QUEUE0_E:
        case CPSS_PP_RX_ERR_QUEUE1_E:
        case CPSS_PP_RX_ERR_QUEUE2_E:
        case CPSS_PP_RX_ERR_QUEUE3_E:
        case CPSS_PP_RX_ERR_QUEUE4_E:
        case CPSS_PP_RX_ERR_QUEUE5_E:
        case CPSS_PP_RX_ERR_QUEUE6_E:
        case CPSS_PP_RX_ERR_QUEUE7_E:
                queue = (GT_U8)(uniEv - CPSS_PP_RX_ERR_QUEUE0_E);

                rc = cpssEnRxPacketGet(devNum, queue);
            break;
        #endif
        case CPSS_PP_MAC_MESSAGE_TO_CPU_READY_E:
        case CPSS_PP_EB_AUQ_PENDING_E:

            if(allowProcessingOfAuqMessages == GT_TRUE)
            {
                rc = cpssEnGenAuMsgHandle(devNum, evExtData);
                if (GT_OK != rc)
                {
                    DBG_LOG(("cpssEnAuMsgGet: error, devNum=%d, uniEv=%d, rc=%d\n", devNum, uniEv, rc, 4, 5, 6));
                }
            }

            break;
        case CPSS_PP_EB_FUQ_PENDING_E:

            rc = cpssEnFuMsgGet(devNum, evExtData);
            if (GT_OK != rc)
            {
                DBG_LOG(("cpssEnFuMsgGet: error, devNum=%d, uniEv=%d, rc=%d\n", devNum, uniEv, rc, 4, 5, 6));
            }
            break;
        /*luoxun --combination may be wrong, attention.*/
        #if 0
        case CPSS_PP_PORT_LINK_STATUS_CHANGED_E:
            /* use function appDemoPrintLinkChangeFlagSet to modify the : printLinkChangeEnabled */
            if(printLinkChangeEnabled)
            {
                osPrintf(" LINK CHANGED : devNum[%ld] port[%ld] \n",(GT_U32)devNum,evExtData);
            }
            break;
        #endif
#ifndef __AX_PLATFORM__
		case CPSS_PP_PORT_LINK_STATUS_CHANGED_E:
			delta.tv_sec = 0;
	        delta.tv_nsec = 1000000;/* 1ms */
			if(PRV_CPSS_DXCH_PORT_TYPE(devNum,evExtData) == PRV_CPSS_PORT_XG_E) {
				/* wait for a while when xg port link status change */
				for(;i < 3; i++) {
					rc = cpssDxChPortLinkStatusGet(devNum,evExtData,&isLinkUp);
					nanosleep(&delta, NULL);
				}
			}
			rc = cpssDxChPortLinkStatusGet(devNum,evExtData,&isLinkUp);
			if(rc != GT_OK) 
			{
				osPrintfErr("handle port(%d,%d) link status change event read port status error!\n", \
						devNum, evExtData);
			}
			else 
			{
				/* check HSC port */
				rc = cpssDxChCheckCscdPort(devNum, evExtData, &result);
				if(GT_OK != rc) {
					osPrintfErr("handle port(%d,%d) link status change %s event check Cscd error!\n", \
									devNum, evExtData, isLinkUp ? "UP":"DOWN");
				}
				else if(GT_TRUE == result) { /* exception event: HSC port link changed!!!*/
					for(ii = 0; ii < MV_MAX_HSC_PORT_NUM; ii++) {
						if((devNum == hscPortInfo[ii].devNum) &&(evExtData == hscPortInfo[ii].portNum)) {
							/* change the log level from osPrintfSyslog7Notice to err, zhangdi@autelan.com 2011-12-28 */
							osPrintfErr("HSC port(%d,%d) link(%s) status change event report %d times,please contact vendor!\n", \
											devNum, evExtData, isLinkUp ? "up":"down", hscPortInfo[ii].intLinkCnt);
							if(0 == hscPortInfo[ii].intLinkCnt % MV_HSC_INT_LINK_MSG_INTERVAL) {
								cpss_write_crit_log("%% HSC port(%d,%d) link(%s) report %d times,please contact vendor!\n",  \
											devNum, evExtData, isLinkUp ? "up":"down", hscPortInfo[ii].intLinkCnt);
							}
							++hscPortInfo[ii].intLinkCnt;
							break;
						}
						else if((0xFF == hscPortInfo[ii].devNum) && (0xFF == hscPortInfo[ii].portNum)) {
							hscPortInfo[ii].devNum = devNum;
							hscPortInfo[ii].portNum = evExtData;
							hscPortInfo[ii].intLinkCnt = 1;
							osPrintfSyslog7Notice("HSC port(%d,%d) link(%s) change event 1st time report, add to %d\n", \
											devNum, evExtData, isLinkUp ? "up":"down", ii);
							break;
						}
					}
				}
				else {
					rc = npd_get_global_index_by_devport(devNum,(GT_U8)evExtData,&eth_g_index);
					if(GT_OK != rc) 
					{
						osPrintfErr("handle port(%d,%d) link status change %s event get eth port index error!\n", \
									devNum, evExtData, isLinkUp ? "UP":"DOWN");	
					}
					else if(GT_TRUE == isLinkUp) 
					{
						osPrintfEvt("port(%d,%d) %#x link status change event %s!\n",	\
									devNum,evExtData,eth_g_index,isLinkUp ? "UP":"DOWN");
						
						rc = portNotifier(eth_g_index,GT_FALSE); /* 0-link up  1-link down */
					}
					else 
					{
						osPrintfEvt("port(%d,%d) %#x link status change event %s!\n",	\
									devNum,evExtData,eth_g_index,isLinkUp ? "UP":"DOWN");
						rc = portNotifier(eth_g_index,GT_TRUE);
					}
				}
			}
			break;
		case CPSS_PP_RX_ERR_QUEUE0_E:
		case CPSS_PP_RX_ERR_QUEUE1_E:
		case CPSS_PP_RX_ERR_QUEUE2_E:
		case CPSS_PP_RX_ERR_QUEUE3_E:
		case CPSS_PP_RX_ERR_QUEUE4_E:
		case CPSS_PP_RX_ERR_QUEUE5_E:
		case CPSS_PP_RX_ERR_QUEUE6_E:
		case CPSS_PP_RX_ERR_QUEUE7_E:
			queue = (GT_U8)(uniEv - CPSS_PP_RX_BUFFER_QUEUE0_E);
			rc = cpssDrvIntScanCpuRxErrMaskShadowSet(devNum, queue, GT_FALSE);
			if(rc != GT_OK) 
			{
				osPrintfErr("handle cpu queue %d rx error event MASKED error!\n", queue);
			}
			break;
#endif

        default:
            break;
    }

    return rc;
}


/*******************************************************************************
* appDemoXbarEvTreat
*
* DESCRIPTION:
*       This routine handles XBAR events.
*
* INPUTS:
*       devNum      - the device number.
*       uniEv       - Unified event number
*       evExtData   - Unified event additional information
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
static GT_STATUS appDemoXbarEvTreat
(
    GT_U8                   devNum,
    GT_U32                  uniEv,
    GT_U32                  evExtData
)
{

        /* TODO: add XBar event handlers here */

        return GT_OK;
}
/*******************************************************************************
* appDemoFaEvTreat
*
* DESCRIPTION:
*       This routine handles FA events.
*
* INPUTS:
*       devNum      - the device number.
*       uniEv       - Unified event number
*       evExtData   - Unified event additional information
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
static GT_STATUS appDemoFaEvTreat
(
    GT_U8                   devNum,
    GT_U32                  uniEv,
    GT_U32                  evExtData
)
{

        /* TODO: add XBar event handlers here */

        return GT_OK;
}

/*******************************************************************************
* appDemoEvHndlr
*
* DESCRIPTION:
*       This routine is the event handler for PSS Event-Request-Driven mode
*       (polling mode).
*
* INPUTS:
*       param - The process data structure.
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
static unsigned __TASKCONV appDemoEvHndlr
(
    GT_VOID * param
)
{
    GT_STATUS           rc;                                         /* return code         */
    GT_U32              i;                                          /* iterator            */
    GT_U32              evHndl;                                     /* event handler       */
    GT_U32              evBitmapArr[CPSS_UNI_EV_BITMAP_SIZE_CNS];   /* event bitmap array  */
    GT_U32              evBitmap;                                   /* event bitmap 32 bit */
    GT_U32              evExtData;                                  /* event extended data */
    GT_U8               devNum;                                     /* device number       */
    GT_U32              uniEv;                                      /* unified event cause */
    GT_U32              evCauseIdx;                                 /* event index         */
    EV_HNDLR_PARAM      *hndlrParamPtr;                             /* bind event array    */
	static unsigned char hdlCount = 0; 
	unsigned char tName[32] = {0};                          /* bind event array    */
#ifdef APPDEMO_REQ_MODE_DEBUG
    GT_U32 tid, prio;
#endif

	sprintf(tName,"appEvHndlr%d",hdlCount++);
	npd_init_tell_whoami(tName,0);

    hndlrParamPtr = (EV_HNDLR_PARAM*)param;
    evHndl        = hndlrParamPtr->evHndl;

    while (1)
    {
        rc = cpssEventSelect(evHndl, NULL, evBitmapArr, (GT_U32)CPSS_UNI_EV_BITMAP_SIZE_CNS);
        if (GT_OK != rc)
        {
            DBG_LOG(("appDemoCpssEventSelect: err [%d]\n", rc, 2, 3, 4, 5, 6));
            continue;
        }

        for (evCauseIdx = 0; evCauseIdx < CPSS_UNI_EV_BITMAP_SIZE_CNS; evCauseIdx++)
        {
            if (evBitmapArr[evCauseIdx] == 0)
            {
                continue;
            }

            evBitmap = evBitmapArr[evCauseIdx];

            for (i = 0; evBitmap; evBitmap >>= 1, i++)
            {
                if ((evBitmap & 1) == 0)
                {
                    continue;
                }

                uniEv = (evCauseIdx << 5) + i;

                if (cpssEventRecv(evHndl, uniEv, &evExtData, &devNum) == GT_OK)
                {
#ifdef APPDEMO_REQ_MODE_DEBUG
                    osTaskGetSelf(&tid);
                    osGetTaskPrior(tid, &prio);
                    DBG_LOG(("cpssEventRecv: %d <dev %d, %s, extData %d> tid 0x%x prio %d\n",
                             (GT_U32)hndlrParamPtr->hndlrIndex, devNum,
                             (GT_U32)uniEvName[uniEv], evExtData, tid, prio));
#endif
                    if (/*(uniEv >= CPSS_PP_UNI_EV_MIN_E) &&*/
                        (uniEv <= CPSS_PP_UNI_EV_MAX_E))
                    {
                        rc = appDemoEnPpEvTreat(devNum, uniEv, evExtData);
                    }
                    else if ((uniEv >= CPSS_XBAR_UNI_EV_MIN_E) &&
                             (uniEv <= CPSS_XBAR_UNI_EV_MAX_E))
                    {
                        rc = appDemoXbarEvTreat(devNum, uniEv, evExtData);
                    }
                    else if ((uniEv >= CPSS_FA_UNI_EV_MIN_E) &&
                             (uniEv <= CPSS_FA_UNI_EV_MAX_E))
                    {
                        rc = appDemoFaEvTreat(devNum, uniEv, evExtData);
                    }

                    /* counting the event */
                    if(eventIncrementFunc != NULL)
                    {
                        /* don't care about the return value */
                        (GT_VOID)eventIncrementFunc(devNum, uniEv);
                    }

                    if (GT_OK != rc)
                    {
                        DBG_LOG(("appDemoEnPpEvTreat: error - 0x%x, uniEv [%d]\n", rc, uniEv, 3, 4, 5, 6));
                    }
                }
            }
        }
    }
}
static GT_U32 rx2cpu_queue_wrr[] = {APP_DEMO_SINGLE_RX_TASK_P0_WEIGHT,
                                    APP_DEMO_SINGLE_RX_TASK_P1_WEIGHT,
                                    APP_DEMO_SINGLE_RX_TASK_P2_WEIGHT,
                                    APP_DEMO_SINGLE_RX_TASK_P3_WEIGHT,
                                    APP_DEMO_SINGLE_RX_TASK_P4_WEIGHT,
                                    APP_DEMO_SINGLE_RX_TASK_P5_WEIGHT,
                                    APP_DEMO_SINGLE_RX_TASK_P6_WEIGHT,
                                    APP_DEMO_SINGLE_RX_TASK_P7_WEIGHT};

/*******************************************************************************
* appDemoSingleRxEvHndlr
*
* DESCRIPTION:
*       This routine is the event handler for all RX events for all queues.
*
* INPUTS:
*       param - The process data structure.
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
static unsigned __TASKCONV appDemoSingleRxEvHndlr
(
    GT_VOID * param
)
{
    GT_STATUS           rc;                                         /* return code         */
    GT_U32              evHndl;                                     /* event handler       */
    GT_U32              evBitmapArr[CPSS_UNI_EV_BITMAP_SIZE_CNS];   /* event bitmap array  */
    GT_U32              evExtData;                                  /* event extended data */
    GT_U8               devNum;                                     /* device number       */
    EV_HNDLR_PARAM      *hndlrParamPtr;                             /* bind event array    */

    GT_BOOL cpuRxNotEmpty[PRV_CPSS_MAX_PP_DEVICES_CNS][8] = {{0}};
    GT_U32 queue;
    GT_U32 maxPacket;
    GT_U8 dev;
    GT_U32 counter =0;
    GT_U32 fullQueues =0;
    hndlrParamPtr = (EV_HNDLR_PARAM*)param;
    evHndl        = hndlrParamPtr->evHndl;

    while (1)
    {
        rc = cpssEventSelect(evHndl, NULL, evBitmapArr, (GT_U32)CPSS_UNI_EV_BITMAP_SIZE_CNS);
        if (GT_OK != rc)
        {
            DBG_LOG(("cpssEventSelect: err [%d]\n", rc, 2, 3, 4, 5, 6));
            continue;
        }
        do
        {
            for (queue = 0; queue < 8; queue++)
            {
                do
                {   
                    /* get all the events for current queue*/
                    rc = cpssEventRecv(evHndl,CPSS_PP_RX_BUFFER_QUEUE0_E+queue,&evExtData,&devNum);
                    if (rc == GT_OK)
                    {
                        (GT_VOID)eventIncrementFunc(devNum, CPSS_PP_RX_BUFFER_QUEUE0_E+queue);
                        cpuRxNotEmpty[devNum][queue] = GT_TRUE;
                    }
                    rc = cpssEventRecv(evHndl,CPSS_PP_RX_ERR_QUEUE0_E+queue,&evExtData,&devNum);
                    if (rc == GT_OK)
                    {
                        (GT_VOID)eventIncrementFunc(devNum, CPSS_PP_RX_ERR_QUEUE0_E+queue);
                        cpuRxNotEmpty[devNum][queue] = GT_TRUE;
                    }
                }
                while (rc == GT_OK);

                maxPacket = rx2cpu_queue_wrr[queue];
                for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(appDemoPpConfigDevAmount); dev++)
                {
                    if(appDemoPpConfigList[dev].valid == GT_FALSE || cpuRxNotEmpty[dev][queue] == GT_FALSE)
                    {
                        counter++;
                        continue;
                    }
                    /* get up to maxPacket */
                    rc = appDemoGetMaxPackets((GT_U8)dev,(GT_U8)queue,maxPacket);
                    if (rc == GT_NO_MORE)
                    {
                        counter++;
                        cpssEventTreatedEventsClear(evHndl);
                        cpuRxNotEmpty[dev][queue] = GT_FALSE;
                    }
                }
                if (counter >= appDemoPpConfigDevAmount)
                {
                    U32_SET_FIELD_MAC(fullQueues,queue,1,0);
                }
                else
                {
                    U32_SET_FIELD_MAC(fullQueues,queue,1,1);
                }
                counter = 0;
            }
        } while ( fullQueues );
    }
}


/*******************************************************************************
* appDemoSingleRxTreatTask
*
* DESCRIPTION:
*       This routine treats Rx packets in single Rx task mode.
*
* INPUTS:
*       param - The process data structure.
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
static unsigned __TASKCONV appDemoSingleRxTreatTask
(
    GT_VOID * param
)
{
    GT_STATUS           rc; /* return code         */
    GT_U32              size; /* size of buffer pointed by message */
    GT_U8               devNum; 
    CPSS_OS_MSGQ_ID    *msgQIdPtr;
    APP_DEMO_RX_PACKET_PARAMS  rxParams;
    msgQIdPtr = (CPSS_OS_MSGQ_ID*)param;

    size = sizeof(APP_DEMO_RX_PACKET_PARAMS);

    while (1)
    {
        rc = cpssOsMsgQRecv(*msgQIdPtr,&rxParams,&size,CPSS_OS_MSGQ_WAIT_FOREVER);
        if (rc != GT_OK)
        {
            DBG_LOG(("cpssOsMsgQRecv: err [%d]\n", rc, 2, 3, 4, 5, 6));
            continue;
        }
        devNum = rxParams.devNum;
        rc = GT_NOT_IMPLEMENTED;
        if(IS_API_CH_DEV_MAC(devNum))/* is device support dxch API ? */
        {
#ifdef DXCH_CODE
            /* apply the App Demo treatment for the new packet */
            rc = appDemoDxChNetRxPktTreat(&rxParams);
#endif /*DXCH_CODE*/
        }
        else if(IS_API_EXMXPM_DEV_MAC(devNum))/* is device support ExMxPm API ? */
        {
#ifdef PM_CODE
            /* apply the App Demo treatment for the new packet */
            rc = appDemoExMxPmNetRxPktTreat(&rxParams);
#endif /*PM_CODE*/
        }

        if (rc != GT_OK)
        {
            DBG_LOG(("XXX_NetRxPktTreat: err [%d]\n", rc, 2, 3, 4, 5, 6));
        }
    }
}


/*******************************************************************************
* cpssEnRxPacketGet
*
* DESCRIPTION:
*       This routine handles the packet Rx event.
*
* INPUTS:
*       devNum  - the device number.
*       queue    - the queue the rx event occurred upon
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
static GT_STATUS cpssEnRxPacketGet
(
    IN GT_U8                devNum,
    IN GT_U8                queue
)
{
    GT_STATUS       rc = GT_NOT_IMPLEMENTED;
	osPrintfErr("receive packet on device %d queue %d\n",devNum,queue);

    do
    {
        if(IS_API_CH_DEV_MAC(devNum))/* is device support dxch API ? */
        {
#ifdef DXCH_CODE
            /* apply the App Demo treatment for the new packet */
            rc = appDemoDxChNetRxPktHandle(devNum, queue);
#endif /*DXCH_CODE*/
        }

        if(IS_API_EXMXPM_DEV_MAC(devNum))/* is device support ExMxPm API ? */
        {
#ifdef PM_CODE
            /* apply the App Demo treatment for the new packet */
            rc = appDemoExMxPmNetRxPktHandle(devNum, queue);
#endif /*PM_CODE*/
        }

        if(IS_API_EXMX_DEV_MAC(devNum)) /* is device support ExMx API ? */
        {
            rc = appDemoGenNetRxPktHandle(devNum, queue);
        }

    }while(rc == GT_OK);

    return rc;
}

/*******************************************************************************
* appDemoGetMaxPackets
*
* DESCRIPTION:
*       This routine get the RX packets from given device and queue 
*       up to maxPacket.
*
* INPUTS:
*       devNum  - the device number.
*       queue    - the queue the rx event occurred upon
*       maxPacket - maximum packets to get from the queue
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
static GT_STATUS appDemoGetMaxPackets
(
    IN GT_U8                devNum,
    IN GT_U8                queue,
    IN GT_U32               maxPacket
)
{
    GT_STATUS       rc = GT_NOT_IMPLEMENTED; /* The returned code            */
    GT_U32          counter = 0; /* counter */
    CPSS_OS_MSGQ_ID msgQId; 
    GT_U32 number;

    do
    {
        /* choose msgQ */
        number = (singleTaskRxRandomEnable == GT_TRUE) ?
                    cpssOsRand() : queue;
        msgQId = singleRxMsgQIds[number % treatTasksNum];
        
        if(IS_API_CH_DEV_MAC(devNum))/* is device support dxch API ? */
        {
#ifdef DXCH_CODE
            /* apply the App Demo treatment for the new packet */
            rc = appDemoDxChNetRxPktGet(devNum, queue,msgQId);
#endif /*DXCH_CODE*/
        }
        else if(IS_API_EXMXPM_DEV_MAC(devNum))/* is device support ExMxPm API ? */
        {
#ifdef PM_CODE
            /* apply the App Demo treatment for the new packet */
            rc = appDemoExMxPmNetRxPktGet(devNum, queue,msgQId);
#endif /*PM_CODE*/
        }
        else
        {
            rc = GT_NOT_IMPLEMENTED;
        }

        counter++;

    }while(rc == GT_OK && counter < maxPacket);

    return rc;
}


#ifndef __AX_PLATFORM__
#if 0
#define   ADPT_THREAD_PRIO 	200
void    *userMutexId;/* signal new packet received */
void    *varLockId;/* global variable lock */
void	*streamSockId;/*igmp socket stream*/
GT_BOOL 	rstpSocketCreated = GT_FALSE;/* RSTP Sock Stream synchronization sem */

void 	*semPktRxMutex;
void	*semKapIoMutex;
void	*semRtRwMutex;

#ifndef __IGMP_SNP_SOCK_STREAM__
/* IgmpSnoop Sock Stream synchronization sem */
GT_BOOL	igmpSnpSockCreated = GT_FALSE; /*added by wujh*/
#endif

int       adptVirRxFd;
int		  adptTaskId;
int       sockeTaskId;
#endif

PRV_CPSS_DRV_INTERRUPT_SCAN_STC * appDemoSdmaRxPollRoot;
GT_TASK		sdmaRxPollTid[PRV_CPSS_MAX_PP_DEVICES_CNS]= {0};/* The task Tid */
extern PRV_CPSS_DRIVER_PP_CONFIG_STC* prvCpssDrvPpConfig[];

#ifndef __AX_PLATFORM__
extern const PRV_CPSS_DRV_INTERRUPT_SCAN_STC cheetah2SdmaRxPollArr[];
extern unsigned int cheetah2SdmaRxPollSize;

extern unsigned int xcatSdmaRxPollSize;
extern const PRV_CPSS_DRV_INTERRUPT_SCAN_STC xcatSdmaRxPollArr[];

extern unsigned int lionSdmaRxPollSize;
extern const PRV_CPSS_DRV_INTERRUPT_SCAN_STC lionSdmaRxPollArr[];
#endif


/*******************************************************************************
* appDemoSdmaRxPollingRootInit
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
GT_STATUS appDemoSdmaRxPollingRootInit
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
	        status = appDemoSdmaRxPollingRootInit(intrScanArrayLen,intrScanArray,
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
* appDemoRxPollingTask
*
* DESCRIPTION:
*       This function scans SDMA Rx interrupt cause register, and performs all
*       needed interrupt nodes queuing.
*
* INPUTS:
*       param         - The device number to scan.
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
GT_U8 appDemoRxPollingTask
(
	GT_VOID * param
)
{
	GT_U8 devNum,newRx = 0;
	SDMA_RxPoll_PARAM *rxPollParam = NULL;
    PRV_CPSS_DRV_INT_CTRL_STC *intCtrlPtr;
	unsigned char tName[32] = {0};
	
	rxPollParam	= (SDMA_RxPoll_PARAM *)param;
	devNum 		= rxPollParam->devNum;
	intCtrlPtr 	= &(prvCpssDrvPpConfig[devNum]->intCtrl);
	
    GT_U32   portGroupId; 
    portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);

	/* tell my thead id */
	sprintf(tName, "sdmaRxPoll%d",devNum);
	npd_init_tell_whoami(tName,0);
	osPrintfDbg("appDemoRxPollingTask[%s]::dev %d,PollRoot %#0lx\n", \
				rxPollParam->taskName, devNum,appDemoSdmaRxPollRoot);
	while(1)
	{
        /*luoxun -- struct 'intCtrlPtr' diff */
        #if 0
		newRx = prvCpssDrvIntEvReqDrvnScan(devNum,
								intCtrlPtr->intNodesPool,
								intCtrlPtr->intMaskShadow,
								appDemoSdmaRxPollRoot);

        newRx = prvCpssDrvIntEvReqDrvnScan(devNum, 0,
								intCtrlPtr->portGroupInfo->intNodesPool,
								intCtrlPtr->portGroupInfo->intMaskShadow,
								appDemoSdmaRxPollRoot); 
		#endif

        newRx = prvCpssDrvIntEvReqDrvnScan(devNum,portGroupId,
                   intCtrlPtr->portGroupInfo[portGroupId].intNodesPool,
                   intCtrlPtr->portGroupInfo[portGroupId].intMaskShadow,
                   appDemoSdmaRxPollRoot);
		
		if(1 == newRx)
		{
			osPrintfDbg("appDemoRxPollingTask::new packets detected\r\n");
		}
		#ifndef __AX_PCI_TEST__
		#if 0
		sleep(1);
		#else
        osTimerWkAfter(2);
		#endif
		#endif
	}
}
/*******************************************************************************
* appDemoBoardSdmaRxPollingInit
*
* DESCRIPTION:
*       Perform SDMA Rx Polling initialization for all devices (Pp, Fa, Xbar).
*       This function includes initializations that common for all devices.
*
* INPUTS:
*       boardRevId      - Board revision Id.
*       numOfPp         - Number of Pp's in system.
*       numOfFa         - Number of Fa's in system.
*       numOfXbar       - Number of Xbar's in system.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       This function must be called after all PP initialization done.
*
*******************************************************************************/
GT_STATUS appDemoBoardSdmaRxPollingInit
(
    IN  GT_U8                   boardRevId,       /* Revision of this board             */
    IN  GT_U8                   numOfDev,         /* Number of devices in this board    */
    IN  GT_U8                   numOfFa,          /* Number of FAs in this board        */
    IN  GT_U8                   numOfXbar         /* Number of xbars in this board      */
)
{
    GT_STATUS                   rc = GT_OK;
    GT_U8                       devIdx,devNum;
	GT_U32						pollArrIdx;
    GT_8                       	taskName[30] = {0};  /* The task name */
	SDMA_RxPoll_PARAM			*taskParamPtr = NULL; /* The device number */

	osPrintfErr("appDemoBoardSdmaRxPollingInit::devNo %d faNo %d xbarNo %d revId %d\r\n", \
			numOfDev,numOfFa,numOfXbar,boardRevId);

    /* Do SDMA Rx Polling configuration for all devices */
    for (devIdx = 0; devIdx < numOfDev; devIdx++)
    {    	
        devNum = appDemoPpConfigList[devIdx].devNum;
		pollArrIdx = 0;

		if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E){
			rc =  appDemoSdmaRxPollingRootInit(
								xcatSdmaRxPollSize,
								xcatSdmaRxPollArr,
								&pollArrIdx,
								&appDemoSdmaRxPollRoot);	
		}else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E){
			rc =  appDemoSdmaRxPollingRootInit(
								lionSdmaRxPollSize,
								lionSdmaRxPollArr,
								&pollArrIdx,
								&appDemoSdmaRxPollRoot);
		}else{
			rc =  appDemoSdmaRxPollingRootInit(
								cheetah2SdmaRxPollSize,
								cheetah2SdmaRxPollArr,
								&pollArrIdx,
								&appDemoSdmaRxPollRoot);
		}


		if(rc != GT_OK)
		{
			osPrintfErr("appDemoBoardSdmaRxPollingInit::appDemoSdmaRxPollingRootInit error\r\n");
			return GT_FAIL;
		}

        osSprintf(taskName, "sdmaRxPoll_%d", devNum);
		taskParamPtr = (SDMA_RxPoll_PARAM*)osMalloc(sizeof(SDMA_RxPoll_PARAM));
		taskParamPtr->devNum = devNum;
		osMemCpy(taskParamPtr->taskName,taskName,osStrlen(taskName));

        rc = osTaskCreate(taskName,
                          SDMA_RX_THRD_PRIO,
                          _8KB,
                          appDemoRxPollingTask,
                          taskParamPtr,
                          &sdmaRxPollTid[devIdx]);
        if (rc != GT_OK)
        {
            return GT_FAIL;
        }
    }
	osPrintfErr("appDemoBoardSdmaRxPollingInit::Init done return ok\r\n");
    return GT_OK;
} /* appDemoBoardSdmaRxPollingInit */
/*******************************************************************************
* cpssDxChSdmaRxPollEnable
*
* DESCRIPTION:
*       Enable SDMA Rx Polling process for the specified device
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChSdmaRxPollEnable
(
    IN GT_U8 devNum
)
{
	GT_U32 tid = 0;

	PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
	
	tid = sdmaRxPollTid[devNum];
	
    if (0 == tid)
    {
        fprintf(stderr, "sdmaRxPollEnable - tid(0): %s\n", strerror(errno));
        return GT_FAIL;
    }

    if (osTaskResume(tid))
    {
        fprintf(stderr, "osTaskResume: %s\n", strerror(errno));
        return GT_FAIL;
    }

	osPrintf("sdmaRxPollEnable::task[%#0x] resumed\r\n",tid);
    return GT_OK ;
}

/*******************************************************************************
* cpssDxChSdmaRxPollDisable
*
* DESCRIPTION:
*       Disable SDMA Rx Polling process for the specified device
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChSdmaRxPollDisable
(
	IN GT_U8 devNum
)
{
	GT_U32 tid = 0;

	PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
	
	tid = sdmaRxPollTid[devNum];
	
	if (0 == tid)
	{
		fprintf(stderr, "sdmaRxPollDisable - tid(0): %s\n", strerror(errno));
		return GT_FAIL;
	}

	if (osTaskSuspend(tid))
	{
	    fprintf(stderr, "osTaskSuspend: %s\n", strerror(errno));
	    return GT_FAIL;
	}

	osPrintf("sdmaRxPollDisable::task[%#0x] suspended\r\n",tid);
	return GT_OK ;
}

/*******************************************************************************
* cpssDxChSdmaRxQueueQuotaSet
*
* DESCRIPTION:
*       This method set SDMA Rx queue quota for the specified {device,queue}
*
* INPUTS:
* 	devNum  - device number
*	queue - queue number
*	quota - quota assigned to the queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChSdmaRxQueueQuotaSet
(
	IN GT_U8 devNum,
	IN GT_U32 queue,
	IN GT_U32 quota
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
	GT_U8 i = 0;
	
	if(queue > NUM_OF_RX_QUEUES) {
		return GT_BAD_PARAM;
	}

	sdmaRxQuota[queue] = quota;
	osPrintfDbg("dev %d queue %d set cpufc quota %d %d\n",devNum, queue, quota,sdmaRxQuota[queue]);
	for(i=0; i<NUM_OF_RX_QUEUES; i++) 
		osPrintfDbg("%d %d\n",i,sdmaRxQuota[i]);


	return GT_OK;
}

/*******************************************************************************
* cpssDxChSdmaRxQueueQuotaGet
*
* DESCRIPTION:
*       This method get SDMA Rx queue quota for the specified {device,queue}
*
* INPUTS:
* 	devNum  - device number
*	queue - queue number
*
* OUTPUTS:
*	quota - quota assigned to the queue
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChSdmaRxQueueQuotaGet
(
	IN GT_U8 devNum,
	IN GT_U32 queue,
	IN GT_U32 *quota
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(quota);
	GT_U8 i = 0;

	if(queue > NUM_OF_RX_QUEUES) {
		return GT_BAD_PARAM;
	}

	*quota = sdmaRxQuota[queue];
	osPrintfDbg("dev %d queue %d get cpufc quota %d\n",devNum, queue, *quota);
	for(i=0; i<NUM_OF_RX_QUEUES; i++) 
		osPrintfDbg("%d %d\n",i,sdmaRxQuota[i]);
	
	return GT_OK;
}

/*******************************************************************************
* cpssDxChPortLinkStatusIntMaskSet
*
* DESCRIPTION:
*       This routine unmasks/masks the interrupt event CPSS_PP_PORT_LINK_STATUS_CHANGED_E to
* enable/disable link status change interrupt.
*
* INPUTS:
*       	devNum - device number.
*  		portNum - port number on the DXCH device.
*	   	enable - enable/disable flag to unmask/mask HW interrupt.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL if failed.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortLinkStatusIntMaskSet
(
    IN  GT_U8	devNum,
    IN  GT_U8	portNum,
    IN GT_BOOL	enable
)
{
	GT_STATUS rc = GT_OK;

	/* setup link status change event mask shadow */
	rc = cpssDrvIntScanPortLinkStatusMaskShadowSet(devNum,portNum,enable);
	
	return rc;
}

/*******************************************************************************
* cpssDxChCheckCscdPort
*
* DESCRIPTION:
*       This routine checks whether the given port is cascade port or not.
*
* INPUTS:
*       	devNum - device number.
*  		portNum - port number on the DXCH device.
*	   	enable - port is cascade port or not (GT_TRUE if yes, other GT_FALSE)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL if failed.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCheckCscdPort
(
    IN  GT_U8	devNum,
    IN  GT_U8	portNum,
    OUT GT_BOOL	*result
)
{
	GT_STATUS rc = GT_OK;
	GT_BOOL status = GT_FALSE;
	GT_U8 port = 0;
	int ii = 0;
	

	PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    for(ii = 0; ii < appDemoPpConfigList[devNum].numberOfCscdPorts; ++ii)
    {
        port = appDemoPpConfigList[devNum].cscdPortsArr[ii].portNum;
		if(portNum == port) {
			status = GT_TRUE;
		}
    }	

	*result = status;
	return rc;
}

#ifdef __AX_PCI_TEST__
extern unsigned long pci_test_pkt_count;
GT_STATUS asdf()
{
	fprintf(stdout,"::PACKET COUNT - %ld\n",pci_test_pkt_count);
	return GT_OK;
}

GT_STATUS gh()
{
	pci_test_pkt_count = 0;
	fprintf(stdout,"::PACKET COUNT - %ld\n",pci_test_pkt_count);
	return GT_OK;
}
#endif
#endif

