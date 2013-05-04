/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmBrgFdb.c
*
* DESCRIPTION:
*       Wrapper functions for cpssExMxPmBrgFdb cpss.exMxPm functions
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>


/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgFdb.h>
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgFdbHash.h>

/*include this H file as WA , to know the number of FDB entries used 32k/64k/...1M*/
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmInfo.h>

/* due to wrong use of enumeration in the Galtis we convert 2 enumerations: */
#define CONVERT____CPSS_PACKET_CMD_ENT___TO__CPSS_MAC_TABLE_CMD_ENT____MAC(/*IN*/packetCmd,/*OUT*/macTblCmd) \
    switch(packetCmd)                                                                               \
    {                                                                                               \
        case CPSS_PACKET_CMD_FORWARD_E: macTblCmd = CPSS_MAC_TABLE_FRWRD_E; break;                  \
        case CPSS_PACKET_CMD_MIRROR_TO_CPU_E: macTblCmd = CPSS_MAC_TABLE_MIRROR_TO_CPU_E; break;    \
        case CPSS_PACKET_CMD_TRAP_TO_CPU_E: macTblCmd = CPSS_MAC_TABLE_CNTL_E; break;               \
        case CPSS_PACKET_CMD_DROP_HARD_E: macTblCmd = CPSS_MAC_TABLE_DROP_E; break;                 \
        case CPSS_PACKET_CMD_DROP_SOFT_E: macTblCmd = CPSS_MAC_TABLE_SOFT_DROP_E; break;            \
        default: galtisOutput(outArgs, GT_BAD_STATE, "");return CMD_OK;                             \
    }

#define CONVERT____CPSS_MAC_TABLE_CMD_ENT___TO__CPSS_PACKET_CMD_ENT____MAC(/*IN*/macTblCmd,/*OUT*/packetCmd) \
    switch(macTblCmd)                                                                               \
    {                                                                                               \
        case CPSS_MAC_TABLE_FRWRD_E: packetCmd = CPSS_PACKET_CMD_FORWARD_E; break;                  \
        case CPSS_MAC_TABLE_DROP_E: packetCmd = CPSS_PACKET_CMD_DROP_HARD_E; break;                 \
        case CPSS_MAC_TABLE_CNTL_E: packetCmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E; break;               \
        case CPSS_MAC_TABLE_MIRROR_TO_CPU_E: packetCmd = CPSS_PACKET_CMD_MIRROR_TO_CPU_E; break;    \
        case CPSS_MAC_TABLE_SOFT_DROP_E: packetCmd = CPSS_PACKET_CMD_DROP_SOFT_E; break;            \
        case CPSS_MAC_TABLE_INTERV_E: /*no break !!!*/ /*not supported option*/                     \
        default: galtisOutput(outArgs, GT_BAD_PARAM, "");return CMD_OK;                             \
    }

/*current index in refreshing of the 'FDB TCAM memory' */
static GT_U32 currFdbTcamMemoryIndex=0;
/*current index in refreshing of the 'FDB LUT' */
static GT_U32 currFdbLutIndex=0;

#define BYTES_TO_U32_MAC(bytesPtr)  \
    (bytesPtr[0] << 24) | (bytesPtr[1] << 16) | (bytesPtr[2] << 8) | (bytesPtr[3])

/*
 * typedef: enum WR_REFRESH_TABLE_METHOD_ENT
 *
 * Description: Enumeration for table 'refresh'
 *
 * Enumerations:
 *      FULL_TABLE_E - Galtis prints out all valid existing entries
 *      ENTRY_E - print out only one entry according to input param.
 */
typedef enum
{
    FULL_TABLE_E,
    ENTRY_E
} WR_REFRESH_TABLE_METHOD_ENT;

/*
number of messages queues :
1. primary AU queue
2. (primary)FU queue
3. secondary AU queue
*/
#define NUM_OF_MESSAGES_QUEUES_CNS   3

/*
    mutex to synchronize between all tasks that use next functionality:

    1. trigger :aging/transplant/delete --> cpssExMxPmBrgFdbTriggeredActionStart(...) , cpssExMxPmBrgFdbAgingTriggerSet(...)
    2. trigger :upload                  --> cpssExMxPmBrgFdbTriggeredActionStart(...) , cpssExMxPmBrgFdbAgingTriggerSet(...)
    3. messages from CPU : entry set , entry delete --> cpssExMxPmBrgFdbEntrySet(...) , cpssExMxPmBrgFdbEntryDelete(...)
    4. get messages from the (primary) AUQ  --> cpssExMxPmBrgFdbAuMsgBlockGet(...)

    5. trigger WA of setting Secondary base address (when deadlock detected) -->cpssExMxPmBrgFdbTriggerAuQueueWa(...)
    6. get messages from the secondary AUQ -->cpssExMxPmBrgFdbSecondaryAuMsgBlockGet(...)

*/
static GT_MUTEX   auqRelateMessagesMutex = NULL;
/* do we need the WA to break the deadlock */
static GT_BOOL    auqDeadlockWaEnable = GT_TRUE;

typedef enum{
    WR_FDB_ENTRY_SET_MODE_BY_MESSAGE_E,
    WR_FDB_ENTRY_SET_MODE_BY_WRAPPER_E
}WR_FDB_ENTRY_SET_MODE_ENT;
/* the that by default the wrapper not calculate the hash index */
static WR_FDB_ENTRY_SET_MODE_ENT fdbEntrySetMode = WR_FDB_ENTRY_SET_MODE_BY_MESSAGE_E;

static GT_STATUS fdbEntryDelete
(
    IN GT_U8                            devNum,
    IN CPSS_EXMXPM_FDB_ENTRY_KEY_STC    *fdbEntryKeyPtr
);

static GT_STATUS fdbEntrySet
(
    IN GT_U8                            devNum,
    IN CPSS_EXMXPM_FDB_ENTRY_STC       *fdbEntryPtr
);

static GT_STATUS wrCpssExMxPmBrgFdbLsrDisable
(
    IN GT_U8         devNum
);

static GT_STATUS fdbEntryWrite
(
    IN GT_U8                            devNum,
    IN GT_U32                           index,
    IN GT_BOOL                          skip,
    IN CPSS_EXMXPM_FDB_ENTRY_STC        *fdbEntryPtr
);


/*******************************************************************************
* wrCpssExMxPmBrgFdbMessagesQueueManagerInfoGet
*
* DESCRIPTION:
*       debug tool - print info that the CPSS hold about the state of the :
*           primary AUQ,FUQ,secondary AU queue
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_SUPPORTED         - the device not need / not support the WA
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS wrCpssExMxPmBrgFdbMessagesQueueManagerInfoGet(
    IN  GT_U8   devNum
)
{
    PRV_CPSS_AU_DESC_CTRL_STC *managerArray[NUM_OF_MESSAGES_QUEUES_CNS];
    char*                     namesArray[NUM_OF_MESSAGES_QUEUES_CNS];
    GT_U32                    managerArrayNum;
    GT_U32  ii;

    PRV_CPSS_EXMXPM_DEV_CHECK_MAC(devNum);

    managerArrayNum = 0;
    namesArray[managerArrayNum] = "auq_prim";
    managerArray[managerArrayNum] = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.auDescCtrl[0]);
    managerArrayNum++;

    if(PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[0].blockSize)
    {
        namesArray[managerArrayNum] = "__fuq___";
        managerArray[managerArrayNum] = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[0]);
        managerArrayNum++;
    }

    if(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAuDescCtrl[0].blockSize)
    {
        namesArray[managerArrayNum] = "auq_seco";
        managerArray[managerArrayNum] = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAuDescCtrl[0]);
        managerArrayNum++;
    }


    cmdOsPrintf("queue   blockAddr  blockSize   currDescIdx unreadCncCounters  \n");
    cmdOsPrintf("============================================================= \n");

    for(ii = 0 ; ii < managerArrayNum ; ii++)
    {
        cmdOsPrintf("%s    0x%8.8x  %d \t %d  \t %d \t 0x%8.8x  \n",
                    namesArray[ii],
                    managerArray[ii]->blockAddr,
                    managerArray[ii]->blockSize,
                    managerArray[ii]->currDescIdx,
                    managerArray[ii]->unreadCncCounters
                    );
    }



    return GT_OK;
}

/*******************************************************************************
* wrapCpssExMxPmBrgAuqMutexCreate
*
* DESCRIPTION:
*       Create and initialize the 'AUQ Mutex'
*
* INPUTS:
*       none
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS wrapCpssExMxPmBrgAuqMutexCreate(void)
{
    if(auqRelateMessagesMutex == NULL)
    {
        return cmdOsMutexCreate("auqMutex",&auqRelateMessagesMutex);
    }

    return GT_OK;
}

/*******************************************************************************
* wrapCpssExMxPmBrgAuqMutexLock
*
* DESCRIPTION:
*       Lock the 'AUQ mutex'
*
* INPUTS:
*       void
*
* OUTPUTS:
*       None
*
* RETURNS:
*       none
*
* COMMENTS:
*       None
*
*******************************************************************************/
void wrapCpssExMxPmBrgAuqMutexLock(void)
{
    cmdOsMutexLock(auqRelateMessagesMutex);
}

/*******************************************************************************
* wrapCpssExMxPmBrgAuqMutexUnLock
*
* DESCRIPTION:
*       UnLock the 'AUQ mutex'
*
* INPUTS:
*       void
*
* OUTPUTS:
*       None
*
* RETURNS:
*       none
*
* COMMENTS:
*       None
*
*******************************************************************************/
void wrapCpssExMxPmBrgAuqMutexUnLock(void)
{
    cmdOsMutexUnlock(auqRelateMessagesMutex);
}

/*******************************************************************************
* wrapCpssExMxPmBrgFdbAuqDeadlockWaEnable
*
* DESCRIPTION:
*       allow to enable/disable the use of the WA for the deadlock of the AUQ
*       with other operations
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum  - device number.
*       enable  - enable/disable the WA
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK           - on success
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS wrapCpssExMxPmBrgFdbAuqDeadlockWaEnable
(
    IN  GT_U8                                     devNum,
    IN  GT_BOOL                                   enable
)
{
    devNum = devNum;
    auqDeadlockWaEnable = enable;

    cmdOsPrintf("wrapCpssExMxPmBrgFdbAuqDeadlockWaEnable: WA is %s \n",
            enable == GT_FALSE ?
                "Disabled" :
                "Enabled");

    return GT_OK;
}


/*
    handling a deadlock in the device:
    the AUQ is full and the AU messages from CPU ,and triggered actions are not
    performed.

    so after cpssExMxPmBrgFdbEntrySet(...) , cpssExMxPmBrgFdbEntryDelete(...)
    we need to check that operation finished by calling cpssExMxPmBrgFdbFromCpuAuMsgStatusGet(...)
    if the status not ready after 3 time we need to solve the problem.

    or after   cpssExMxPmBrgFdbTriggeredActionStart(...) , cpssExMxPmBrgFdbAgingTriggerSet(...)
    we need to check that operation finished by cpssExMxPmBrgFdbAgingTriggerGet(...)
    NOTE: only for operations that not generate AU messages to the CPU.
    like: trigger FDB upload , trigger delete

    or after cpssExMxPmBrgFdbLsrEnableSet(dev,GT_FALSE) when disabling the LSR
    we need to check that LSR disabled using cpssExMxPmBrgFdbLsrStoppedGet(...)

*/


typedef enum
{
    WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_NA_AA_FROM_CPU_E,
    WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_QUERY_FROM_CPU_E,
    WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_TRIGGERED_ACTION_E,
    WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_LSR_DISABLE_E
}WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_ENT;

/*******************************************************************************
* wrapCpssExMxPmBrgFdbResolveDeviceDeadLock
*
* DESCRIPTION:
*       function to check and resolve deadlock that relate to AUQ
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum  - device number.
*       action  - type of action that may be stacked in deadlock
* OUTPUTS:
*       repeatActionPtr - (pointer to) the action of the caller must be repeated
*                       because it not succeeded the previous time.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or mode
*       GT_BAD_PTR      - on NULL pointer
*       GT_INIT_ERROR   - recursive calling too depth
* COMMENTS:
*       function must be called under the locking of the 'AUQ mutex'
*
*******************************************************************************/
GT_STATUS wrapCpssExMxPmBrgFdbResolveDeviceDeadLock
(
    IN  GT_U8                                     devNum,
    IN  WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_ENT  action,
    OUT GT_BOOL                                   *repeatActionPtr
)
{
    GT_STATUS   rc;
    GT_BOOL completed,succeeded,isFull;
    GT_BOOL origActionEnable,origNaToCpu,origAaAndTaToCpu,origSpAaMsgToCpu;
    GT_U32  retryBusyCount   = 100;/* retry counter under 'busy wait' */
    GT_U32  maxIterations    = 50;/*retry count under 'sleep 1 tick' */
    GT_U32  minimalSleepTime = 1;/* minimal time to sleep*/
    GT_U32                                   numOfAu = 0;
    static CPSS_EXMXPM_FDB_UPDATE_MSG_ENTRY_STC     auMessagesArray[CPSS_EXMXPM_PP_FDB_AUQ_DEADLOCK_EXTRA_MESSAGES_NUM_WA_CNS];
    static GT_U32   callStackDepth = 0;
    GT_U32      ii;

    CPSS_NULL_PTR_CHECK_MAC(repeatActionPtr);

    *repeatActionPtr = GT_FALSE;
    if(auqDeadlockWaEnable == GT_FALSE)
    {
        /* this way we can test same scenarios to see with/without the WA */
        return GT_OK;
    }

    origActionEnable = GT_FALSE;
    origNaToCpu      = GT_FALSE;
    origAaAndTaToCpu = GT_FALSE;
    origSpAaMsgToCpu = GT_FALSE;


    callStackDepth ++;

#ifdef ASIC_SIMULATION
    /* 200 milliseconds to sleep between reads */
    minimalSleepTime = 200;
    /* total of 10 seconds (50*200ms=10sec)*/
    maxIterations = 50;
    /* no need for 'busy wait' */
    retryBusyCount = 0;
#endif /*ASIC_SIMULATIONs*/

    do
    {
        switch(action)
        {
            case WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_NA_AA_FROM_CPU_E:
                /* NA,AA use the same bit in the register */
                rc = cpssExMxPmBrgFdbFromCpuAuMsgStatusGet(devNum,CPSS_NA_E,&completed,&succeeded);
                break;
            case WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_QUERY_FROM_CPU_E:
                rc = cpssExMxPmBrgFdbFromCpuAuMsgStatusGet(devNum,CPSS_QA_E,&completed,&succeeded);
                break;
            case WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_TRIGGERED_ACTION_E:
                rc = cpssExMxPmBrgFdbAgingTriggerGet(devNum,&completed);
                break;
            case WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_LSR_DISABLE_E:
                rc = cpssExMxPmBrgFdbLsrStoppedGet(devNum,&completed);
#ifdef ASIC_SIMULATION
                completed = GT_TRUE;
#endif /*ASIC_SIMULATION*/
                break;
            default:
                return GT_BAD_PARAM;
        }

        if(rc != GT_OK)
        {
            goto exit_cleanly_lbl;
        }

        if(completed == GT_TRUE)
        {
            break;
        }

        if(retryBusyCount)
        {
            /* counting with 'busy wait' to avoid penalty of osTimerWkAfter(1)
               that will take at least '1 tick' that is 16-20 milliseconds
               in vxWorks  */
            retryBusyCount--;
        }
        else
        {
            /* after all the 'busy wait' we start take sleeps of 1 minimal tick */

            maxIterations--;
            /* set minimal time */
            /* this sleep is better then the 'busy wait' because it will be the
               same value in all types of CPUs

               in VxWorks : 50*ticks = 0.8 to 1 seconds

               */
            cmdOsTimerWkAfter(minimalSleepTime);
        }
    }while(retryBusyCount || maxIterations);

    if(completed == GT_TRUE)
    {
        /* no need to WA */
        rc = GT_OK;
        goto exit_cleanly_lbl;
    }


    /******************************************************/
    /* state that the original action need to be repeated */
    /******************************************************/
    *repeatActionPtr = GT_TRUE;


    /* check that the PP is stacked in the queue */

    rc = cpssExMxPmBrgFdbQueueFullGet(devNum,
            CPSS_EXMXPM_FDB_QUEUE_TYPE_AU_E,/*query AUQ and not FUQ */
            &isFull);
    if(rc != GT_OK)
    {
        goto exit_cleanly_lbl;
    }

    if(isFull != GT_TRUE)
    {
        cmdOsPrintf( "wrapCpssExMxPmBrgFdbResolveDeviceDeadLock: action not finished but PP not stacked \n");

        /* continue , because we not know that PP detect 'isFull' on all cases */
    }

    /* we run out of retries , and still PP not finished the action */
    /* we need to start the WA */

    if(callStackDepth == 1)
    {
        rc = cpssExMxPmBrgFdbActionsEnableGet(devNum,&origActionEnable);
        if(rc != GT_OK)
        {
            goto exit_cleanly_lbl;
        }

        if(origActionEnable == GT_TRUE)
        {
            /* disable the FDB actions : "stop the scanning " */
            rc = cpssExMxPmBrgFdbActionsEnableSet(devNum,GT_FALSE);
            if(rc != GT_OK)
            {
                goto exit_cleanly_lbl;
            }
        }

        if(action == WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_TRIGGERED_ACTION_E)
        {
            rc = cpssExMxPmBrgFdbNaToCpuGet(devNum, &origNaToCpu);
            if(rc != GT_OK)
            {
                goto exit_cleanly_lbl;
            }

            if(origNaToCpu == GT_TRUE)
            {
                rc = cpssExMxPmBrgFdbNaToCpuSet(devNum, GT_FALSE);
                if(rc != GT_OK)
                {
                    goto exit_cleanly_lbl;
                }
            }

            rc = cpssExMxPmBrgFdbAaAndTaToCpuGet(devNum, &origAaAndTaToCpu);
            if(rc != GT_OK)
            {
                goto exit_cleanly_lbl;
            }

            if(origAaAndTaToCpu == GT_TRUE)
            {
                rc = cpssExMxPmBrgFdbAaAndTaToCpuSet(devNum, GT_FALSE);
                if(rc != GT_OK)
                {
                    goto exit_cleanly_lbl;
                }
            }

            rc = cpssExMxPmBrgFdbSpAaMsgToCpuGet(devNum, &origSpAaMsgToCpu);
            if(rc != GT_OK)
            {
                goto exit_cleanly_lbl;
            }

            if(origSpAaMsgToCpu == GT_TRUE)
            {
                rc = cpssExMxPmBrgFdbSpAaMsgToCpuSet(devNum, GT_FALSE);
                if(rc != GT_OK)
                {
                    goto exit_cleanly_lbl;
                }
            }
        }

    }

    /* clean messages from the secondary queue , that may exists from the
       previous triggering of the WA */
    numOfAu = CPSS_EXMXPM_PP_FDB_AUQ_DEADLOCK_EXTRA_MESSAGES_NUM_WA_CNS;
    rc = cpssExMxPmBrgFdbSecondaryAuMsgBlockGet(devNum,&numOfAu,&auMessagesArray[0]);
    if(rc != GT_OK && rc != GT_NO_MORE)
    {
        goto exit_cleanly_lbl;
    }

    /* we must store ALL the messages from the secondary AUQ , before we start
       process them , because the PP can't process them ,
       this is the reason that we got to this code in the first place */

    rc = cpssExMxPmBrgFdbTriggerAuQueueWa(devNum);
    if(rc != GT_OK)
    {
        /* print some info */
        wrCpssExMxPmBrgFdbMessagesQueueManagerInfoGet(devNum);

        goto exit_cleanly_lbl;
    }


    /* now that we allow the PP to divert messages to the secondary Queue we
       should be able to process the messages */
    for(ii = 0 ; ii < numOfAu ; ii++)
    {
        switch (auMessagesArray[ii].updType)
        {
            case CPSS_AA_E:
            case CPSS_NA_E:
                    /* write data to cpss */
                    if(auMessagesArray[ii].updType == CPSS_AA_E)
                    {
                    rc = fdbEntryDelete(devNum, &auMessagesArray[ii].fdbEntry.key);
                    }
                    else
                    {
                    rc = fdbEntrySet(devNum, &auMessagesArray[ii].fdbEntry);
                    }

                    if (rc != GT_OK)
                    {
                        if(auMessagesArray[ii].updType == CPSS_AA_E)
                        {
                            cmdOsPrintf( "wrapCpssExMxPmBrgFdbResolveDeviceDeadLock: cpssExMxPmBrgFdbEntryDelete failed \n");
                        }
                        else
                        {
                            cmdOsPrintf( "wrapCpssExMxPmBrgFdbResolveDeviceDeadLock: cpssExMxPmBrgFdbEntrySet failed \n");
                        }
                        goto exit_cleanly_lbl;
                    }

                break;

            default:
                /* don't care */
                break;
        }
    }

exit_cleanly_lbl:

    /************************/
    /* restore HW settings  */
    /************************/

    if(origActionEnable == GT_TRUE)
    {
        /* we changed the state , so restore to original */
        cpssExMxPmBrgFdbActionsEnableSet(devNum,GT_TRUE);
    }

    if(origNaToCpu == GT_TRUE)
    {
        /* we changed the state , so restore to original */
        cpssExMxPmBrgFdbNaToCpuSet(devNum,GT_TRUE);
    }

    if(origAaAndTaToCpu == GT_TRUE)
    {
        /* we changed the state , so restore to original */
        cpssExMxPmBrgFdbAaAndTaToCpuSet(devNum,GT_TRUE);
    }

    if(origSpAaMsgToCpu == GT_TRUE)
    {
        /* we changed the state , so restore to original */
        cpssExMxPmBrgFdbSpAaMsgToCpuSet(devNum,GT_TRUE);
    }

    callStackDepth --;

    if(callStackDepth >= 5)
    {
        cmdOsPrintf( "wrapCpssExMxPmBrgFdbResolveDeviceDeadLock: function call stack overflow \n");
        /* use unique error , that not used in the used cpss function */
        rc = GT_INIT_ERROR;
    }

    return rc;
}


/*******************************************************************************
* cpssExMxPmBrgFdbInit  [DB]
*
* DESCRIPTION:
*       Init FDB system facility for a device.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum         - device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or mode
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbInit

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8               devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbInit(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbHashModeSet   [DB]
*
* DESCRIPTION:
*       Sets the FDB hash function mode.
*       The CRC based hash function provides the best hash index distribution
*       for random addresses and vlans.
*       The Legacy-based hash function is EXMX like hash function.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum  - device number
*       mode    - hash function based mode
*                 CPSS_EXMXPM_FDB_HASH_FUNC_CRC_E - CRC based hash function mode
*                 CPSS_EXMXPM_FDB_HASH_FUNC_LEGACY_E - Legacy based hash function
*                                                  mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or mode
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbHashModeSet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8                                devNum;
    CPSS_EXMXPM_FDB_HASH_FUNC_MODE_ENT   mode;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_EXMXPM_FDB_HASH_FUNC_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbHashModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgFdbHashModeGet   [DB]
*
* DESCRIPTION:
*       Gets the FDB hash function mode.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - pointer to hash function based mode:
*                 CPSS_EXMXPM_FDB_HASH_FUNC_CRC_E - CRC based hash function mode
*                 CPSS_EXMXPM_FDB_HASH_FUNC_LEGACY_E - Legacy based hash function
*                                                  mode - EXMX like hash function
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or mode
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbHashModeGet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8                               devNum;
    CPSS_EXMXPM_FDB_HASH_FUNC_MODE_ENT  modePtr;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbHashModeGet(devNum, &modePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", modePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbMacVlanLookupModeSet  [DB]
*
* DESCRIPTION:
*       Sets the VLAN Lookup mode.
*       This API is not relevant for IPM bridging.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum  - device number
*       mode    - lookup mode:
*                 CPSS_IVL_E - Both MAC and Vlan are used for MAC table lookup
*                              and learning.
*                 CPSS_SVL_E - Only MAC is used for MAC table lookup and learning.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on bad param
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbMacVlanLookupModeSet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8            devNum;
    CPSS_MAC_VL_ENT  mode;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_MAC_VL_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbMacVlanLookupModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbMacVlanLookupModeGet  [DB]
*
* DESCRIPTION:
*       Get VLAN Lookup mode.
*       This API is not relevant for IPM bridging.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - pointer to lookup mode:
*                 CPSS_IVL_E - Both MAC and Vlan are used for MAC table lookup
*                              and learning.
*                 CPSS_SVL_E - Only MAC is used for MAC table lookup and learning.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on bad param
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbMacVlanLookupModeGet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8            devNum;
    CPSS_MAC_VL_ENT  modePtr;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbMacVlanLookupModeGet(devNum, &modePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", modePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbExtLookupOffsetSizeSet    [DB]
*
* DESCRIPTION:
*       Set the size of the offset field in the external lookup table.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum      - physical device number
*       offset      - The size of the offset field.
*                     CPSS_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_5_BITS_E -
*                           offset size is 5 bits
*                     CPSS_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_6_BITS_E -
*                           offset size is 6 bits
*                     CPSS_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_7_BITS_E -
*                           offset size is 7 bits
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - on wrong devNum or offset
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbExtLookupOffsetSizeSet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8                                    devNum;
    GT_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_ENT  offset;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    offset = (GT_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbExtLookupOffsetSizeSet(devNum, offset);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbExtLookupOffsetSizeGet    [DB]
*
* DESCRIPTION:
*       Get the size of the offset field in the external lookup table.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       offsetPtr   - The size of the offset field.
*                     CPSS_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_5_BITS_E -
*                           offset size is 5 bits
*                     CPSS_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_6_BITS_E -
*                           offset size is 6 bits
*                     CPSS_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_7_BITS_E -
*                           offset size is 7 bits
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - on wrong devNum or offset
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbExtLookupOffsetSizeGet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_ENT  offsetPtr;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbExtLookupOffsetSizeGet(devNum, &offsetPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", offsetPtr);
    return CMD_OK;
}



/* cpssExMxPmBrgFdbMac    Table   (same as cpssDxChBrgFdbMac)   [DB] */

static GT_U32   indexCnt;




/*
* cpssExMxPmBrgFdbEntryWrite
*
* DESCRIPTION:
*       Write the new entry in Hardware FDB table in specified index.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum          - device number
*       index           - hw fdb entry index
*       skip            - entry skip control
*                         GT_TRUE - used to "skip" the entry ,
*                         the HW will treat this entry as "not used"
*                         GT_FALSE - used for valid/used entries.
*       fdbEntryPtr     - pointer to FDB entry parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum,saCommand,daCommand
*       GT_OUT_OF_RANGE - vidx/trunkId/portNum-devNum with values bigger then HW
*                         support
*                         index out of range
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbEntrySetByIndex

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_U32                          index;
    GT_BOOL                         skip;
    CPSS_EXMXPM_FDB_ENTRY_STC       macEntryPtr;
    GT_BYTE_ARRY                    sipBArr, dipBArr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    index = (GT_U32)inFields[0];
    skip = (GT_BOOL)inFields[1];

    macEntryPtr.dstOutlif.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
    macEntryPtr.key.entryType = (CPSS_EXMXPM_FDB_ENTRY_TYPE_ENT)inFields[2];

    switch(inFields[2])
    {
    case 0:
    case 3:
        galtisMacAddr(&macEntryPtr.key.key.macFid.macAddr,
                                       (GT_U8*)inFields[3]);
        macEntryPtr.key.key.macFid.fId = (GT_U16)inFields[4];
        break;

    case 1:
    case 2:
        macEntryPtr.key.key.ipMcast.fId = (GT_U16)inFields[4];

        galtisBArray(&sipBArr, (GT_U8*)inFields[5]);

        cmdOsMemCpy(
            macEntryPtr.key.key.ipMcast.sip, sipBArr.data, 4);

        galtisBArray(&dipBArr, (GT_U8*)inFields[6]);

        cmdOsMemCpy(
            macEntryPtr.key.key.ipMcast.dip, dipBArr.data, 4);
        break;

    default:
        break;
    }

    macEntryPtr.dstOutlif.interfaceInfo.type = (CPSS_INTERFACE_TYPE_ENT)inFields[7];

    switch(inFields[7])
    {
    case 0:
        macEntryPtr.dstOutlif.interfaceInfo.devPort.devNum = (GT_U8)inFields[8];
        macEntryPtr.dstOutlif.interfaceInfo.devPort.portNum = (GT_U8)inFields[9];
        CONVERT_DEV_PORT_DATA_MAC(macEntryPtr.dstOutlif.interfaceInfo.devPort.devNum,
                             macEntryPtr.dstOutlif.interfaceInfo.devPort.portNum);

        break;

    case 1:
        macEntryPtr.dstOutlif.interfaceInfo.trunkId = (GT_TRUNK_ID)inFields[10];
        break;

    case 2:
        macEntryPtr.dstOutlif.interfaceInfo.vidx = (GT_U16)inFields[11];
        break;

    case 3:
        macEntryPtr.dstOutlif.interfaceInfo.vlanId = (GT_U16)inFields[12];
        break;

    default:
        break;
    }

    macEntryPtr.isStatic = (GT_BOOL)inFields[13];

    CONVERT____CPSS_MAC_TABLE_CMD_ENT___TO__CPSS_PACKET_CMD_ENT____MAC(inFields[14],macEntryPtr.daCommand);
    CONVERT____CPSS_MAC_TABLE_CMD_ENT___TO__CPSS_PACKET_CMD_ENT____MAC(inFields[15],macEntryPtr.saCommand);
    macEntryPtr.daRoute = (GT_BOOL)inFields[16];
    macEntryPtr.mirrorToRxAnalyzerPortEn = (GT_BOOL)inFields[17];
    macEntryPtr.sourceId = (GT_U32)inFields[18];
    macEntryPtr.userDefined = (GT_U32)inFields[19];
    macEntryPtr.daQosIndex = (GT_U32)inFields[20];
    macEntryPtr.saQosIndex = (GT_U32)inFields[21];
    macEntryPtr.daSecurityLevel = (GT_U32)inFields[22];
    macEntryPtr.saSecurityLevel = (GT_U32)inFields[23];
    macEntryPtr.appSpecificCpuCode = (GT_BOOL)inFields[24];
    macEntryPtr.spUnknown = GT_FALSE;


    /* For Puma2 entry completion */
    macEntryPtr.pwId = 0;

    /* call cpss api function */
    result = fdbEntryWrite(devNum, index, skip, &macEntryPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbEntrySet
*
* DESCRIPTION:
*       Create new or update existing entry in Hardware FDB table through
*       Address Update message.(AU message to the PP is non direct access to FDB
*       table).
*       The function use New Address message (NA) format.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum          - device number
*       fdbEntryPtr     - pointer to FDB table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR  - on hardware error
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       Application should synchronize call of cpssExMxPmBrgFdbEntrySet,
*       cpssExMxPmBrgFdbQaSend and cpssExMxPmBrgFdbEntryDelete functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the AU message processing has completed by
*       cpssExMxPmBrgFdbFromCpuAuMsgStatusGet.
*       If the entry type is IPv6, then the Application should set the four
*       bytes for SIP and DIP that selected by  cpssExMxPmBrgMcIpv6BytesSelectSet.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbEntrySetByMacAddr

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    CPSS_EXMXPM_FDB_ENTRY_STC       macEntryPtr;
    GT_BYTE_ARRY                    sipBArr, dipBArr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    macEntryPtr.dstOutlif.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
    macEntryPtr.key.entryType = (CPSS_EXMXPM_FDB_ENTRY_TYPE_ENT)inFields[2];

    switch(inFields[2])
    {
    case 0:
    case 3:
        galtisMacAddr(&macEntryPtr.key.key.macFid.macAddr,
                                       (GT_U8*)inFields[3]);
        macEntryPtr.key.key.macFid.fId = (GT_U16)inFields[4];
        break;

    case 1:
    case 2:
        macEntryPtr.key.key.ipMcast.fId = (GT_U16)inFields[4];

        galtisBArray(&sipBArr, (GT_U8*)inFields[5]);

        cmdOsMemCpy(
            macEntryPtr.key.key.ipMcast.sip, sipBArr.data, 4);

        galtisBArray(&dipBArr, (GT_U8*)inFields[6]);

        cmdOsMemCpy(
            macEntryPtr.key.key.ipMcast.dip, dipBArr.data, 4);
        break;

    default:
        break;
    }

    macEntryPtr.dstOutlif.interfaceInfo.type = (CPSS_INTERFACE_TYPE_ENT)inFields[7];

    switch(inFields[7])
    {
    case 0:
        macEntryPtr.dstOutlif.interfaceInfo.devPort.devNum = (GT_U8)inFields[8];
        macEntryPtr.dstOutlif.interfaceInfo.devPort.portNum = (GT_U8)inFields[9];
        CONVERT_DEV_PORT_DATA_MAC(macEntryPtr.dstOutlif.interfaceInfo.devPort.devNum,
                             macEntryPtr.dstOutlif.interfaceInfo.devPort.portNum);


        break;

    case 1:
        macEntryPtr.dstOutlif.interfaceInfo.trunkId = (GT_TRUNK_ID)inFields[10];
        break;

    case 2:
        macEntryPtr.dstOutlif.interfaceInfo.vidx = (GT_U16)inFields[11];
        break;

    case 3:
        macEntryPtr.dstOutlif.interfaceInfo.vlanId = (GT_U16)inFields[12];
        break;

    default:
        break;
    }

    macEntryPtr.isStatic = (GT_BOOL)inFields[13];
    CONVERT____CPSS_MAC_TABLE_CMD_ENT___TO__CPSS_PACKET_CMD_ENT____MAC(inFields[14],macEntryPtr.daCommand);
    CONVERT____CPSS_MAC_TABLE_CMD_ENT___TO__CPSS_PACKET_CMD_ENT____MAC(inFields[15],macEntryPtr.saCommand);
    macEntryPtr.daRoute = (GT_BOOL)inFields[16];
    macEntryPtr.mirrorToRxAnalyzerPortEn = (GT_BOOL)inFields[17];
    macEntryPtr.sourceId = (GT_U32)inFields[18];
    macEntryPtr.userDefined = (GT_U32)inFields[19];
    macEntryPtr.daQosIndex = (GT_U32)inFields[20];
    macEntryPtr.saQosIndex = (GT_U32)inFields[21];
    macEntryPtr.daSecurityLevel = (GT_U32)inFields[22];
    macEntryPtr.saSecurityLevel = (GT_U32)inFields[23];
    macEntryPtr.appSpecificCpuCode = (GT_BOOL)inFields[24];
    macEntryPtr.spUnknown = GT_FALSE;

    /* For Puma2 entry completion */
    macEntryPtr.pwId = 0;

    result = fdbEntrySet(devNum, &macEntryPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgFdbEntrySet
*
* DESCRIPTION:
*   The function calls wrCpssExMxPmBrgFdbEntrySetByIndex or
*   wrCpssExMxPmBrgFdbEntrySetByMacAddr according to user's
*   preference as set in inArgs[1].
*
*
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbEntrySet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* the table resides only in this comment  */
    /* don't remove it                         */

    /* switch by byIndex boolean in inArgs[1]  */

    switch (inArgs[1])
    {
        case 0:
            return wrCpssExMxPmBrgFdbEntrySetByMacAddr(
                inArgs, inFields ,numFields ,outArgs);
        case 1:
            return wrCpssExMxPmBrgFdbEntrySetByIndex(
                inArgs, inFields ,numFields ,outArgs);

        default: return CMD_AGENT_ERROR;
    }


}

/*******************************************************************************
* cpssExMxPmBrgFdbEntryRead
*
* DESCRIPTION:
*       Reads the new entry in Hardware FDB table from specified index.
*       This action do direct read access to RAM .
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
* INPUTS:
*       devNum      - device number
*       index       - hw fdb entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*       agedPtr     - (pointer to) is entry aged
*       associatedDevNumPtr = (pointer to) is device number associated with the
*                     entry (even for vidx/trunk entries the field is used by
*                     PP for aging/flush/transplant purpose).
*                     Relevant only in case of FDB entry.
*       entryPtr    - (pointer to) extended FDB table entry
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - index out of range
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbEntryReadFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;

    GT_U8                               devNum;
    GT_BOOL                             showSkip;
    GT_BOOL                             validPtr;
    GT_BOOL                             skipPtr;
    GT_BOOL                             agedPtr;
    GT_U8                               associatedDevNumPtr;
    CPSS_EXMXPM_FDB_ENTRY_STC           entryPtr;
    GT_U32                              tempSip, tempDip;
    GT_U8                          dummyPort = 0;/* dummy port num for conversion */

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    /* by default : 'ignore skip' (the filed of inArgs[1] = 0 --> ignore skip)*/
    showSkip = (inArgs[1]==GT_TRUE) ? GT_TRUE : GT_FALSE;/* use the bylndex field */
    indexCnt = 0;

    do{
         /* call cpss api function */
        result = cpssExMxPmBrgFdbEntryRead(devNum, indexCnt, &validPtr, &skipPtr,
                                        &agedPtr, &associatedDevNumPtr, &entryPtr);

         if (result != GT_OK)
         {
             /* check end of table return code */
             if(GT_OUT_OF_RANGE == result)
             {
                 /* the result is ok, this is end of table */
                 result = GT_OK;
             }

             galtisOutput(outArgs, result, "%d", -1);
             return CMD_OK;
          }

      indexCnt++;

    }while (!validPtr || (!showSkip && skipPtr));


    if(validPtr)
    {
        inFields[0] = indexCnt - 1;
        inFields[1] = skipPtr;
        inFields[2] = (CPSS_MAC_ENTRY_EXT_TYPE_ENT)entryPtr.key.entryType;

        switch(entryPtr.key.entryType)
        {
        case 0:
            inFields[4] = entryPtr.key.key.macFid.fId;
            break;

        case 1:
        case 2:
            inFields[4] = entryPtr.key.key.ipMcast.fId;
            break;

        default:
            break;
        }

        cmdOsMemCpy(&tempSip, &entryPtr.key.key.ipMcast.sip[0], 4);
        tempSip = cmdOsHtonl(tempSip);
        inFields[5] = tempSip;

        cmdOsMemCpy(&tempDip, &entryPtr.key.key.ipMcast.dip[0], 4);
        tempDip = cmdOsHtonl(tempDip);
        inFields[6] = tempDip;

        inFields[7] = entryPtr.dstOutlif.interfaceInfo.type;
        CONVERT_BACK_DEV_PORT_DATA_MAC(entryPtr.dstOutlif.interfaceInfo.devPort.devNum,
                             entryPtr.dstOutlif.interfaceInfo.devPort.portNum);
        inFields[8] = entryPtr.dstOutlif.interfaceInfo.devPort.devNum;
        inFields[9] = entryPtr.dstOutlif.interfaceInfo.devPort.portNum;


        inFields[10] = entryPtr.dstOutlif.interfaceInfo.trunkId;
        inFields[11] = entryPtr.dstOutlif.interfaceInfo.vidx;
        inFields[12] = entryPtr.dstOutlif.interfaceInfo.vlanId;

        inFields[13] = entryPtr.isStatic;
        CONVERT____CPSS_PACKET_CMD_ENT___TO__CPSS_MAC_TABLE_CMD_ENT____MAC(entryPtr.daCommand,inFields[14]);
        CONVERT____CPSS_PACKET_CMD_ENT___TO__CPSS_MAC_TABLE_CMD_ENT____MAC(entryPtr.saCommand,inFields[15]);
        inFields[16] = entryPtr.daRoute;
        inFields[17] = entryPtr.mirrorToRxAnalyzerPortEn;
        inFields[18] = entryPtr.sourceId;
        inFields[19] = entryPtr.userDefined;
        inFields[20] = entryPtr.daQosIndex;
        inFields[21] = entryPtr.saQosIndex;
        inFields[22] = entryPtr.daSecurityLevel;
        inFields[23] = entryPtr.saSecurityLevel;
        inFields[24] = entryPtr.appSpecificCpuCode;
        inFields[25] = agedPtr;
        CONVERT_BACK_DEV_PORT_DATA_MAC(associatedDevNumPtr,
                             dummyPort);
        inFields[26] = associatedDevNumPtr;

        /* pack and output table fields */
        fieldOutput("%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                              inFields[0],  inFields[1],  inFields[2],
                              entryPtr.key.key.macFid.macAddr.arEther,
                              inFields[4],  inFields[5],  inFields[6],
                              inFields[7],  inFields[8],  inFields[9],
                              inFields[10], inFields[11], inFields[12],
                              inFields[13], inFields[14], inFields[15],
                              inFields[16], inFields[17], inFields[18],
                              inFields[19], inFields[20], inFields[21],
                              inFields[22], inFields[23], inFields[24],
                                            inFields[25], inFields[26]);

        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
       galtisOutput(outArgs, result, "");
    }
    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbEntryReadNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;

    GT_U8                               devNum;
    GT_BOOL                             showSkip;
    GT_BOOL                             validPtr;
    GT_BOOL                             skipPtr;
    GT_BOOL                             agedPtr;
    GT_U8                               associatedDevNumPtr;
    CPSS_EXMXPM_FDB_ENTRY_STC           entryPtr;
    GT_U32                              tempSip, tempDip;
    GT_U8                          dummyPort = 0;/* dummy port num for conversion */

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    /* by default : 'ignore skip' (the filed of inArgs[1] = 0 --> ignore skip)*/
    showSkip = (inArgs[1]==GT_TRUE) ? GT_TRUE : GT_FALSE;/* use the bylndex field */


    /* call cpss api function */
    do{
        result = cpssExMxPmBrgFdbEntryRead(devNum, indexCnt, &validPtr, &skipPtr,
                                        &agedPtr, &associatedDevNumPtr, &entryPtr);
        if (result != GT_OK)
        {
            /* check end of table return code */
            if((GT_OUT_OF_RANGE == result) || (!validPtr))
            {
                /* the result is ok, this is end of table */
                result = GT_OK;
            }

            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }

        ++indexCnt;

    }while ((!validPtr) || (!showSkip && skipPtr));

    CPSS_COVERITY_NON_ISSUE_BOOKMARK
    /* coverity[dead_error_begin] */
    if(validPtr)
    {
        inFields[0] = indexCnt - 1;
        inFields[1] = skipPtr;
        inFields[2] = (CPSS_MAC_ENTRY_EXT_TYPE_ENT)entryPtr.key.entryType;

        switch(entryPtr.key.entryType)
        {
        case 0:
            inFields[4] = entryPtr.key.key.macFid.fId;
            break;

        case 1:
        case 2:
            inFields[4] = entryPtr.key.key.ipMcast.fId;
            break;

        default:
            break;
        }

        cmdOsMemCpy(&tempSip, &entryPtr.key.key.ipMcast.sip[0], 4);
        tempSip = cmdOsHtonl(tempSip);
        inFields[5] = tempSip;

        cmdOsMemCpy(&tempDip, &entryPtr.key.key.ipMcast.dip[0], 4);
        tempDip = cmdOsHtonl(tempDip);
        inFields[6] = tempDip;

        inFields[7] = entryPtr.dstOutlif.interfaceInfo.type;
        CONVERT_BACK_DEV_PORT_DATA_MAC(entryPtr.dstOutlif.interfaceInfo.devPort.devNum,
                             entryPtr.dstOutlif.interfaceInfo.devPort.portNum);
        inFields[8] = entryPtr.dstOutlif.interfaceInfo.devPort.devNum;
        inFields[9] = entryPtr.dstOutlif.interfaceInfo.devPort.portNum;

        inFields[10] = entryPtr.dstOutlif.interfaceInfo.trunkId;
        inFields[11] = entryPtr.dstOutlif.interfaceInfo.vidx;
        inFields[12] = entryPtr.dstOutlif.interfaceInfo.vlanId;

        inFields[13] = entryPtr.isStatic;
        CONVERT____CPSS_PACKET_CMD_ENT___TO__CPSS_MAC_TABLE_CMD_ENT____MAC(entryPtr.daCommand,inFields[14]);
        CONVERT____CPSS_PACKET_CMD_ENT___TO__CPSS_MAC_TABLE_CMD_ENT____MAC(entryPtr.saCommand,inFields[15]);
        inFields[16] = entryPtr.daRoute;
        inFields[17] = entryPtr.mirrorToRxAnalyzerPortEn;
        inFields[18] = entryPtr.sourceId;
        inFields[19] = entryPtr.userDefined;
        inFields[20] = entryPtr.daQosIndex;
        inFields[21] = entryPtr.saQosIndex;
        inFields[22] = entryPtr.daSecurityLevel;
        inFields[23] = entryPtr.saSecurityLevel;
        inFields[24] = entryPtr.appSpecificCpuCode;
        inFields[25] = agedPtr;
        CONVERT_BACK_DEV_PORT_DATA_MAC(associatedDevNumPtr,
                             dummyPort);
        inFields[26] = associatedDevNumPtr;

        /* pack and output table fields */
        fieldOutput("%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                              inFields[0],  inFields[1],  inFields[2],
                              entryPtr.key.key.macFid.macAddr.arEther,
                              inFields[4],  inFields[5],  inFields[6],
                              inFields[7],  inFields[8],  inFields[9],
                              inFields[10], inFields[11], inFields[12],
                              inFields[13], inFields[14], inFields[15],
                              inFields[16], inFields[17], inFields[18],
                              inFields[19], inFields[20], inFields[21],
                              inFields[22], inFields[23], inFields[24],
                                            inFields[25], inFields[26]);

        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
       galtisOutput(outArgs, result, "%d", -1);

    }
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbEntryDelete
*
* DESCRIPTION:
*       Delete an old entry in Hardware FDB table through Address Update
*       message.(AU message to the PP is non direct access to FDB table).
*       The function use New Address message (NA) format with skip bit set to 1.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum          - device number
*       fdbEntryKeyPtr  - pointer to key parameters of the FDB entry
*                         according to the entry type:
*                         MAC Address entry -> MAC Address + vlan ID.
*                         IPv4/IPv6 entry -> srcIP + dstIP + vald ID.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or vlanId
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       Application should synchronize call of cpssExMxPmBrgFdbEntrySet,
*       cpssExMxPmBrgFdbQaSend and cpssExMxPmBrgFdbEntryDelete functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the AU message processing has completed by
*       cpssExMxPmBrgFdbFromCpuAuMsgStatusGet.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbEntryDeleteByMacAddr

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;

    GT_U8                                   devNum;
    CPSS_EXMXPM_FDB_ENTRY_KEY_STC           macEntryKeyPtr;
    GT_BYTE_ARRY                            sipBArr, dipBArr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    macEntryKeyPtr.entryType = (CPSS_MAC_ENTRY_EXT_TYPE_ENT)inFields[2];

    switch(inFields[2])
    {
    case 0:
        galtisMacAddr(&macEntryKeyPtr.key.macFid.macAddr, (GT_U8*)inFields[3]);
        macEntryKeyPtr.key.macFid.fId = (GT_U16)inFields[4];
        break;

    case 1:
    case 2:
        macEntryKeyPtr.key.ipMcast.fId = (GT_U16)inFields[4];

        galtisBArray(&sipBArr, (GT_U8*)inFields[5]);

        cmdOsMemCpy(
            macEntryKeyPtr.key.ipMcast.sip, sipBArr.data, 4);

        galtisBArray(&dipBArr, (GT_U8*)inFields[6]);

        cmdOsMemCpy(
            macEntryKeyPtr.key.ipMcast.dip, dipBArr.data, 4);
        break;

    default:
        break;
    }

    result = fdbEntryDelete(devNum, &macEntryKeyPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbEntryInvalidate
*
* DESCRIPTION:
*       Invalidate an entry in Hardware FDB address table in specified index.
*       the invalidation done by resetting to first word of the entry
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum      - device number
*       index       - hw fdb entry index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum
*       GT_OUT_OF_RANGE - index out of range
*
* COMMENTS:
*       May invalidate neighbour entries in the same hash chain
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbEntryDeleteByIndex

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS                 result;

    GT_U8                     devNum;
    GT_U32                    index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    index = (GT_U32)inFields[0];

    /* call cpss api function */
    if(index < PRV_CPSS_EXMXPM_PP_MAC(devNum)->moduleCfg.bridgeCfg.fdbNumOfEntries)
    {
        result = cpssExMxPmBrgFdbEntryInvalidate(devNum, index);
    }
    else
    {
        result = cpssExMxPmBrgFdbTcamEntryInvalidate(devNum,
                index - PRV_CPSS_EXMXPM_PP_MAC(devNum)->moduleCfg.bridgeCfg.fdbNumOfEntries);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrCpssExMxPmBrgFdbEntryDelete
*
* DESCRIPTION:
*   The function calls wrCpssExMxPmBrgFdbEntryDeleteByIndex or
*   wrCpssExMxPmBrgFdbEntryDeleteByMacAddr according to user's
*   preference as set in inArgs[1].
*
*
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbEntryDelete
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    /* the table resides only in this comment  */
    /* don't remove it                         */

    /* switch by byIndex boolean in inArgs[1]  */

    switch (inArgs[1])
    {
        case 0:
                return wrCpssExMxPmBrgFdbEntryDeleteByMacAddr(
                        inArgs, inFields ,numFields ,outArgs);
        case 1:
                return wrCpssExMxPmBrgFdbEntryDeleteByIndex(
                        inArgs, inFields ,numFields ,outArgs);

        default: return CMD_AGENT_ERROR;
    }


}


static GT_BOOL isExt;
/*if GT_TRUE -wrapper for cpssExMxPm2BrgFdbMacExt table
  if GT_FALSE-wrapper for cpssExMxPm2BrgFdbMac table
*/




/*
* cpssExMxPm2BrgFdbEntryWrite
*
* DESCRIPTION:
*       Write the new entry in Hardware FDB table in specified index.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum          - device number
*       index           - hw fdb entry index
*       skip            - entry skip control
*                         GT_TRUE - used to "skip" the entry ,
*                         the HW will treat this entry as "not used"
*                         GT_FALSE - used for valid/used entries.
*       fdbEntryPtr     - pointer to FDB entry parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum,saCommand,daCommand
*       GT_OUT_OF_RANGE - vidx/trunkId/portNum-devNum with values bigger then HW
*                         support
*                         index out of range
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2BrgFdbEntrySetByIndex

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS                    result;

    GT_U8                        devNum;
    GT_U32                       index;
    GT_BOOL                      skip;
    GT_U8                        fieldIndex=0;
    CPSS_EXMXPM_FDB_ENTRY_STC    fdbEntry;
    GT_BYTE_ARRY                 sipBArr, dipBArr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    index = (GT_U32)inFields[0];
    skip = (GT_BOOL)inFields[1];

    fdbEntry.key.entryType = (CPSS_EXMXPM_FDB_ENTRY_TYPE_ENT)inFields[2];

    switch(inFields[2])
    {
    case CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E:
    case CPSS_EXMXPM_FDB_ENTRY_TYPE_VPLS_E:

        galtisMacAddr(&fdbEntry.key.key.macFid.macAddr,
                        (GT_U8*)inFields[3]);
        fdbEntry.key.key.macFid.fId = (GT_U32)inFields[4];
        break;

    case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E:
    case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E:

        fdbEntry.key.key.ipMcast.fId = (GT_U32)inFields[4];

        galtisBArray(&sipBArr, (GT_U8*)inFields[5]);

        cmdOsMemCpy(
            fdbEntry.key.key.ipMcast.sip, sipBArr.data, 4);

        galtisBArray(&dipBArr, (GT_U8*)inFields[6]);

        cmdOsMemCpy(
            fdbEntry.key.key.ipMcast.dip, dipBArr.data, 4);
        break;

    default:
        break;
    }

    fdbEntry.dstOutlif.outlifType = (CPSS_EXMXPM_OUTLIF_TYPE_ENT)inFields[7];



    switch(fdbEntry.dstOutlif.outlifType)
    {
    case CPSS_EXMXPM_OUTLIF_TYPE_LL_E:
        fdbEntry.dstOutlif.outlifPointer.arpPtr=(GT_U32)inFields[8];
        break;
    case CPSS_EXMXPM_OUTLIF_TYPE_DIT_E:
        fdbEntry.dstOutlif.outlifPointer.ditPtr=(GT_U32)inFields[9];
        break;
    case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:

        fdbEntry.dstOutlif.outlifPointer.tunnelStartPtr.passengerPacketType=(CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ENT)inFields[10];
        fdbEntry.dstOutlif.outlifPointer.tunnelStartPtr.ptr=(GT_U32)inFields[11];


        break;
    default:
        break;
    }


    fdbEntry.dstOutlif.interfaceInfo.type=(CPSS_INTERFACE_TYPE_ENT)inFields[12] ;

    switch(inFields[12])
    {
    case 0:
        fdbEntry.dstOutlif.interfaceInfo.devPort.devNum = (GT_U8)inFields[13];
        fdbEntry.dstOutlif.interfaceInfo.devPort.portNum = (GT_U8)inFields[14];
        CONVERT_DEV_PORT_DATA_MAC(fdbEntry.dstOutlif.interfaceInfo.devPort.devNum,
                        fdbEntry.dstOutlif.interfaceInfo.devPort.portNum);

        break;

    case 1:
        fdbEntry.dstOutlif.interfaceInfo.trunkId = (GT_TRUNK_ID)inFields[15];
        break;

    case 2:
        fdbEntry.dstOutlif.interfaceInfo.vidx = (GT_U16)inFields[16];
        break;


    case 3:
        if(fieldIndex==GT_FALSE)
            fdbEntry.dstOutlif.interfaceInfo.vlanId = (GT_U16)inFields[17];
        break;

    default:
        break;
    }
        if(isExt==GT_FALSE)
        fieldIndex=17;
      else
        fieldIndex=16;

    fdbEntry.isStatic = (GT_BOOL)inFields[fieldIndex+1];
    CONVERT____CPSS_MAC_TABLE_CMD_ENT___TO__CPSS_PACKET_CMD_ENT____MAC(inFields[fieldIndex+2],fdbEntry.daCommand);
    CONVERT____CPSS_MAC_TABLE_CMD_ENT___TO__CPSS_PACKET_CMD_ENT____MAC(inFields[fieldIndex+3],fdbEntry.saCommand);
    fdbEntry.daRoute = (GT_BOOL)inFields[fieldIndex+4];
    fdbEntry.mirrorToRxAnalyzerPortEn = (GT_BOOL)inFields[fieldIndex+5];
    fdbEntry.sourceId = (GT_U32)inFields[fieldIndex+6];
    fdbEntry.userDefined = (GT_U32)inFields[fieldIndex+7];
    fdbEntry.daQosIndex = (GT_U32)inFields[fieldIndex+8];
    fdbEntry.saQosIndex = (GT_U32)inFields[fieldIndex+9];
    fdbEntry.daSecurityLevel = (GT_U32)inFields[fieldIndex+10];
    fdbEntry.saSecurityLevel = (GT_U32)inFields[fieldIndex+11];
    fdbEntry.appSpecificCpuCode = (GT_BOOL)inFields[fieldIndex+12];
    fdbEntry.pwId=(GT_U32)inFields[index+13];
    fdbEntry.spUnknown = GT_FALSE;


    /* call cpss api function */
    result = fdbEntryWrite(devNum, index, skip, &fdbEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPm2BrgFdbEntrySet
*
* DESCRIPTION:
*       Create new or update existing entry in Hardware FDB table through
*       Address Update message.(AU message to the PP is non direct access to FDB
*       table).
*       The function use New Address message (NA) format.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum          - device number
*       fdbEntryPtr     - pointer to FDB table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR  - on hardware error
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       Application should synchronize call of cpssExMxPmBrgFdbEntrySet,
*       cpssExMxPmBrgFdbQaSend and cpssExMxPmBrgFdbEntryDelete functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the AU message processing has completed by
*       cpssExMxPmBrgFdbFromCpuAuMsgStatusGet.
*       If the entry type is IPv6, then the Application should set the four
*       bytes for SIP and DIP that selected by  cpssExMxPmBrgMcIpv6BytesSelectSet.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2BrgFdbEntrySetByMacAddr

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS                    result;

    GT_U8                        devNum;
    CPSS_EXMXPM_FDB_ENTRY_STC    fdbEntry;
    GT_BYTE_ARRY                 sipBArr, dipBArr;
        GT_U8                        fieldIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    fdbEntry.key.entryType = (CPSS_EXMXPM_FDB_ENTRY_TYPE_ENT)inFields[2];

    switch(inFields[2])
    {
    case CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E:
    case CPSS_EXMXPM_FDB_ENTRY_TYPE_VPLS_E:

        galtisMacAddr(&fdbEntry.key.key.macFid.macAddr,
                        (GT_U8*)inFields[3]);
        fdbEntry.key.key.macFid.fId = (GT_U32)inFields[4];
        break;

    case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E:
    case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E:

        fdbEntry.key.key.ipMcast.fId = (GT_U32)inFields[4];

        galtisBArray(&sipBArr, (GT_U8*)inFields[5]);

        cmdOsMemCpy(
            fdbEntry.key.key.ipMcast.sip, sipBArr.data, 4);

        galtisBArray(&dipBArr, (GT_U8*)inFields[6]);

        cmdOsMemCpy(
            fdbEntry.key.key.ipMcast.dip, dipBArr.data, 4);
        break;

    default:
        break;
    }

    fdbEntry.dstOutlif.outlifType=(CPSS_EXMXPM_OUTLIF_TYPE_ENT)inFields[7];



    switch(fdbEntry.dstOutlif.outlifType)
    {
    case CPSS_EXMXPM_OUTLIF_TYPE_LL_E:
        fdbEntry.dstOutlif.outlifPointer.arpPtr=(GT_U32)inFields[8];
        break;
    case CPSS_EXMXPM_OUTLIF_TYPE_DIT_E:
        fdbEntry.dstOutlif.outlifPointer.ditPtr=(GT_U32)inFields[9];
        break;
    case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:

        fdbEntry.dstOutlif.outlifPointer.tunnelStartPtr.passengerPacketType=(CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ENT)inFields[10];
        fdbEntry.dstOutlif.outlifPointer.tunnelStartPtr.ptr=(GT_U32)inFields[11];


        break;
    default:
        break;
    }


    fdbEntry.dstOutlif.interfaceInfo.type=(CPSS_INTERFACE_TYPE_ENT)inFields[12] ;

    switch(inFields[12])
    {
    case 0:
        fdbEntry.dstOutlif.interfaceInfo.devPort.devNum = (GT_U8)inFields[13];
        fdbEntry.dstOutlif.interfaceInfo.devPort.portNum = (GT_U8)inFields[14];
        CONVERT_DEV_PORT_DATA_MAC(fdbEntry.dstOutlif.interfaceInfo.devPort.devNum,
                        fdbEntry.dstOutlif.interfaceInfo.devPort.portNum);

        break;

    case 1:
        fdbEntry.dstOutlif.interfaceInfo.trunkId = (GT_TRUNK_ID)inFields[15];
        break;

    case 2:
        fdbEntry.dstOutlif.interfaceInfo.vidx = (GT_U16)inFields[16];
        break;

    case 3:
        if(isExt==GT_FALSE)
            fdbEntry.dstOutlif.interfaceInfo.vlanId = (GT_U16)inFields[17];
        break;

    default:
        break;
    }
        if(isExt==GT_FALSE)
        fieldIndex=17;
      else
        fieldIndex=16;
    fdbEntry.isStatic = (GT_BOOL)inFields[fieldIndex+1];
    CONVERT____CPSS_MAC_TABLE_CMD_ENT___TO__CPSS_PACKET_CMD_ENT____MAC(inFields[fieldIndex+2],fdbEntry.daCommand);
    CONVERT____CPSS_MAC_TABLE_CMD_ENT___TO__CPSS_PACKET_CMD_ENT____MAC(inFields[fieldIndex+3],fdbEntry.saCommand);
    fdbEntry.daRoute = (GT_BOOL)inFields[fieldIndex+4];
    fdbEntry.mirrorToRxAnalyzerPortEn = (GT_BOOL)inFields[fieldIndex+5];
    fdbEntry.sourceId = (GT_U32)inFields[fieldIndex+6];
    fdbEntry.userDefined = (GT_U32)inFields[fieldIndex+7];
    fdbEntry.daQosIndex = (GT_U32)inFields[fieldIndex+8];
    fdbEntry.saQosIndex = (GT_U32)inFields[fieldIndex+9];
    fdbEntry.daSecurityLevel = (GT_U32)inFields[fieldIndex+10];
    fdbEntry.saSecurityLevel = (GT_U32)inFields[fieldIndex+11];
    fdbEntry.appSpecificCpuCode = (GT_BOOL)inFields[fieldIndex+12];
    fdbEntry.pwId=(GT_U32)inFields[fieldIndex+13];
    fdbEntry.spUnknown = GT_FALSE;

    result = fdbEntrySet(devNum, &fdbEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPm2BrgFdbEntrySet
*
* DESCRIPTION:
*   The function calls wrCpssExMxPmBrgFdbEntrySetByIndex or
*   wrCpssExMxPmBrgFdbEntrySetByMacAddr according to user's
*   preference as set in inArgs[1].
*
*
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static CMD_STATUS prvWrCpssExMxPm2BrgFdbEntrySet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    /* the table resides only in this comment  */
    /* don't remove it                         */

    /* switch by byIndex boolean in inArgs[1]  */

    switch (inArgs[1])
    {
        case 0:
                return wrCpssExMxPm2BrgFdbEntrySetByMacAddr(
                        inArgs, inFields ,numFields ,outArgs);
        case 1:
                return wrCpssExMxPm2BrgFdbEntrySetByIndex(
                        inArgs, inFields ,numFields ,outArgs);

        default: return CMD_AGENT_ERROR;
    }


}
/******************************************************************************/

/*update isExt parameter and call wrapper set*/

static CMD_STATUS wrCpssExMxPm2BrgFdbEntryExtSet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    isExt=GT_TRUE;

    return prvWrCpssExMxPm2BrgFdbEntrySet(inArgs,inFields,numFields,outArgs);
}
/******************************************************************************/
static CMD_STATUS wrCpssExMxPm2BrgFdbEntrySet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    isExt=GT_FALSE;

    return prvWrCpssExMxPm2BrgFdbEntrySet(inArgs,inFields,numFields,outArgs);
}
/*******************************************************************************
* cpssExMxPm2BrgFdbEntryRead
*
* DESCRIPTION:
*       Reads the new entry in Hardware FDB table from specified index.
*       This action do direct read access to RAM .
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
* INPUTS:
*       devNum      - device number
*       index       - hw fdb entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*       agedPtr     - (pointer to) is entry aged
*       associatedDevNumPtr = (pointer to) is device number associated with the
*                     entry (even for vidx/trunk entries the field is used by
*                     PP for aging/flush/transplant purpose).
*                     Relevant only in case of FDB entry.
*       entryPtr    - (pointer to) extended FDB table entry
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - index out of range
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS prvWrCpssExMxPm2BrgFdbEntryReadFirst

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS                       result;
    GT_BOOL                             showSkip;
    GT_U8                           devNum;
    GT_BOOL                         validPtr;
    GT_BOOL                         skipPtr;
    GT_BOOL                         agedPtr;
    GT_U8                           associatedDevNumPtr;
    CPSS_EXMXPM_FDB_ENTRY_STC       fdbEntry;
    GT_U32                          tempSip, tempDip;
    GT_U8                           tempDev,tempPort;
        GT_U8                           fieldIndex;
    GT_U8                          dummyPort = 0;/* dummy port num for conversion */

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    /* by default : 'ignore skip' (the filed of inArgs[1] = 0 --> ignore skip)*/
    showSkip = (inArgs[1]==GT_TRUE) ? GT_TRUE : GT_FALSE;/* use the bylndex field */

    indexCnt = 0;

    do{
        /* call cpss api function */
        if(indexCnt < PRV_CPSS_EXMXPM_PP_MAC(devNum)->moduleCfg.bridgeCfg.fdbNumOfEntries)
        {
            result = cpssExMxPmBrgFdbEntryRead(devNum, indexCnt, &validPtr, &skipPtr,
                            &agedPtr, &associatedDevNumPtr, &fdbEntry);
        }
        else
        {
            result = cpssExMxPmBrgFdbTcamEntryRead(devNum,
                        indexCnt - PRV_CPSS_EXMXPM_PP_MAC(devNum)->moduleCfg.bridgeCfg.fdbNumOfEntries,
                        &validPtr, &skipPtr, &agedPtr, &associatedDevNumPtr, &fdbEntry);
        }

        if (result != GT_OK)
        {
            /* check end of table return code */
            if(GT_OUT_OF_RANGE == result)
            {
                /* the result is ok, this is end of table */
                result = GT_OK;
            }

            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }

        ++indexCnt;

    }while (!validPtr || (!showSkip && skipPtr));

    if(validPtr)
    {
        inFields[0] = indexCnt - 1;
        inFields[1] = skipPtr;
        inFields[2]=  fdbEntry.key.entryType;

        switch(inFields[2])
        {
                case CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E:
                case CPSS_EXMXPM_FDB_ENTRY_TYPE_VPLS_E:

                        inFields[4]=fdbEntry.key.key.macFid.fId ;
            break;

                case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E:
                case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E:

                        inFields[4]=fdbEntry.key.key.ipMcast.fId;

                        cmdOsMemCpy(&tempSip, &fdbEntry.key.key.ipMcast.sip[0], 4);
                        tempSip = cmdOsHtonl(tempSip);
                        inFields[5] = tempSip;

                        cmdOsMemCpy(&tempDip, &fdbEntry.key.key.ipMcast.dip[0], 4);
                        tempDip = cmdOsHtonl(tempDip);
                        inFields[6] = tempDip;

                        break;

                default:
            break;
        }

                inFields[7]=fdbEntry.dstOutlif.outlifType;



        switch(fdbEntry.dstOutlif.outlifType)
        {
        case CPSS_EXMXPM_OUTLIF_TYPE_LL_E:
            inFields[8]=fdbEntry.dstOutlif.outlifPointer.arpPtr;
            break;
        case CPSS_EXMXPM_OUTLIF_TYPE_DIT_E:
            inFields[9]=fdbEntry.dstOutlif.outlifPointer.ditPtr;
            break;
        case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:

            inFields[10]=fdbEntry.dstOutlif.outlifPointer.tunnelStartPtr.passengerPacketType;
            inFields[11]=fdbEntry.dstOutlif.outlifPointer.tunnelStartPtr.ptr;
            break;
        default:
            break;
        }


        inFields[12]=fdbEntry.dstOutlif.interfaceInfo.type;

        switch(inFields[12])
        {
        case 0:
            tempDev=fdbEntry.dstOutlif.interfaceInfo.devPort.devNum;
            tempPort=fdbEntry.dstOutlif.interfaceInfo.devPort.portNum ;
            CONVERT_BACK_DEV_PORT_DATA_MAC(tempDev,tempPort);
            inFields[13]=tempDev;
            inFields[14]=tempPort;
            break;

        case 1:
            inFields[15]=fdbEntry.dstOutlif.interfaceInfo.trunkId;
            break;

        case 2:
            inFields[16]=fdbEntry.dstOutlif.interfaceInfo.vidx;
            break;

        case 3:
            if(isExt==GT_FALSE)
                inFields[17]=fdbEntry.dstOutlif.interfaceInfo.vlanId;
            break;

        default:
            break;
        }
                if(isExt==GT_FALSE)
            fieldIndex=17;
        else
           fieldIndex=16;

                inFields[fieldIndex+1]=fdbEntry.isStatic ;
                CONVERT____CPSS_PACKET_CMD_ENT___TO__CPSS_MAC_TABLE_CMD_ENT____MAC(fdbEntry.daCommand,inFields[fieldIndex+2]);
                CONVERT____CPSS_PACKET_CMD_ENT___TO__CPSS_MAC_TABLE_CMD_ENT____MAC(fdbEntry.saCommand,inFields[fieldIndex+3]);
                inFields[fieldIndex+4]=fdbEntry.daRoute;
                inFields[fieldIndex+5]=fdbEntry.mirrorToRxAnalyzerPortEn;
                inFields[fieldIndex+6]=fdbEntry.sourceId;
                inFields[fieldIndex+7]=fdbEntry.userDefined;
                inFields[fieldIndex+8]=fdbEntry.daQosIndex;
                inFields[fieldIndex+9]=fdbEntry.saQosIndex;
                inFields[fieldIndex+10]=fdbEntry.daSecurityLevel;
                inFields[fieldIndex+11]=fdbEntry.saSecurityLevel;
                inFields[fieldIndex+12]=fdbEntry.appSpecificCpuCode;
                inFields[fieldIndex+13]=fdbEntry.pwId;
                inFields[fieldIndex+14]=agedPtr;
                CONVERT_BACK_DEV_PORT_DATA_MAC(associatedDevNumPtr,
                                     dummyPort);
                inFields[fieldIndex+15]=associatedDevNumPtr;
                if(isExt==GT_FALSE)
        /* pack and output table fields */
        fieldOutput("%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                        inFields[0],  inFields[1],  inFields[2],
                        fdbEntry.key.key.macFid.macAddr.arEther,
                        inFields[4],  inFields[5],  inFields[6],
                        inFields[7],  inFields[8],  inFields[9],
                        inFields[10], inFields[11], inFields[12],
                        inFields[13], inFields[14], inFields[15],
                        inFields[16], inFields[17], inFields[18],
                        inFields[19], inFields[20], inFields[21],
                        inFields[22], inFields[23], inFields[24],
                        inFields[25], inFields[26],inFields[27],
                        inFields[28], inFields[29], inFields[30],
                        inFields[31], inFields[32]);

                else
            fieldOutput("%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                        inFields[0],  inFields[1],  inFields[2],
                        fdbEntry.key.key.macFid.macAddr.arEther,
                        inFields[4],  inFields[5],  inFields[6],
                        inFields[7],  inFields[8],  inFields[9],
                        inFields[10], inFields[11], inFields[12],
                        inFields[13], inFields[14], inFields[15],
                        inFields[16], inFields[17], inFields[18],
                        inFields[19], inFields[20], inFields[21],
                        inFields[22], inFields[23], inFields[24],
                        inFields[25], inFields[26],inFields[27],
                        inFields[28], inFields[29], inFields[30],
                        inFields[31]);

        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
                galtisOutput(outArgs, result, "");
    }


    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS prvWrCpssExMxPm2BrgFdbEntryReadNext
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS                       result;
    GT_BOOL                             showSkip;
    GT_U8                           devNum;
    GT_BOOL                         validPtr;
    GT_BOOL                         skipPtr;
    GT_BOOL                         agedPtr;
    GT_U8                           associatedDevNumPtr;
    GT_U32                          tempSip, tempDip;
    CPSS_EXMXPM_FDB_ENTRY_STC       fdbEntry;
    GT_U8                           tempDev,tempPort;
        GT_U8                           fieldIndex;
    GT_U8                          dummyPort = 0;/* dummy port num for conversion */

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    /* by default : 'ignore skip' (the filed of inArgs[1] = 0 --> ignore skip)*/
    showSkip = (inArgs[1]==GT_TRUE) ? GT_TRUE : GT_FALSE;/* use the bylndex field */



    /* call cpss api function */
    do{

        /* call cpss api function */
        if(indexCnt < PRV_CPSS_EXMXPM_PP_MAC(devNum)->moduleCfg.bridgeCfg.fdbNumOfEntries)
        {
            result = cpssExMxPmBrgFdbEntryRead(devNum, indexCnt, &validPtr, &skipPtr,
                            &agedPtr, &associatedDevNumPtr, &fdbEntry);
        }
        else
        {
            result = cpssExMxPmBrgFdbTcamEntryRead(devNum,
                        indexCnt - PRV_CPSS_EXMXPM_PP_MAC(devNum)->moduleCfg.bridgeCfg.fdbNumOfEntries,
                        &validPtr, &skipPtr, &agedPtr, &associatedDevNumPtr, &fdbEntry);
        }

        if (result != GT_OK)
        {
            /* check end of table return code */
            if((GT_OUT_OF_RANGE == result) || (!validPtr))
            {
                /* the result is ok, this is end of table */
                result = GT_OK;
            }

            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }

        ++indexCnt;

    }while (!validPtr || (!showSkip && skipPtr));


    CPSS_COVERITY_NON_ISSUE_BOOKMARK
    /* coverity[dead_error_begin] */
    if(validPtr)
    {
        inFields[0] = indexCnt - 1;
        inFields[1] = skipPtr;
        inFields[2]=  fdbEntry.key.entryType;

        switch(inFields[2])
        {
                case CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E:
                case CPSS_EXMXPM_FDB_ENTRY_TYPE_VPLS_E:

                        inFields[4]=fdbEntry.key.key.macFid.fId ;
            break;

                case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E:
                case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E:

                        inFields[4]=fdbEntry.key.key.ipMcast.fId;

                        cmdOsMemCpy(&tempSip, &fdbEntry.key.key.ipMcast.sip[0], 4);
                        tempSip = cmdOsHtonl(tempSip);
                        inFields[5] = tempSip;

                        cmdOsMemCpy(&tempDip, &fdbEntry.key.key.ipMcast.dip[0], 4);
                        tempDip = cmdOsHtonl(tempDip);
                        inFields[6] = tempDip;

                        break;

                default:
            break;
        }

                inFields[7]=fdbEntry.dstOutlif.outlifType;



        switch(fdbEntry.dstOutlif.outlifType)
        {
        case CPSS_EXMXPM_OUTLIF_TYPE_LL_E:
            inFields[8]=fdbEntry.dstOutlif.outlifPointer.arpPtr;
            break;
        case CPSS_EXMXPM_OUTLIF_TYPE_DIT_E:
            inFields[9]=fdbEntry.dstOutlif.outlifPointer.ditPtr;
            break;
        case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:

            inFields[10]=fdbEntry.dstOutlif.outlifPointer.tunnelStartPtr.passengerPacketType;
            inFields[11]=fdbEntry.dstOutlif.outlifPointer.tunnelStartPtr.ptr;
            break;
        default:
            break;
        }


        inFields[12]=fdbEntry.dstOutlif.interfaceInfo.type;

        switch(inFields[12])
        {
        case 0:
            tempDev=fdbEntry.dstOutlif.interfaceInfo.devPort.devNum;
            tempPort=fdbEntry.dstOutlif.interfaceInfo.devPort.portNum ;
            CONVERT_BACK_DEV_PORT_DATA_MAC(tempDev,tempPort);
            inFields[13]=tempDev;
            inFields[14]=tempPort;
            break;

        case 1:
            inFields[15]=fdbEntry.dstOutlif.interfaceInfo.trunkId;
            break;

        case 2:
            inFields[16]=fdbEntry.dstOutlif.interfaceInfo.vidx;
            break;

        case 3:
            if(isExt==GT_FALSE)
                inFields[17]=fdbEntry.dstOutlif.interfaceInfo.vlanId;
            break;

        default:
            break;
        }
                if(isExt==GT_FALSE)
            fieldIndex=17;
        else
           fieldIndex=16;

                inFields[fieldIndex+1]=fdbEntry.isStatic ;
                CONVERT____CPSS_PACKET_CMD_ENT___TO__CPSS_MAC_TABLE_CMD_ENT____MAC(fdbEntry.daCommand,inFields[fieldIndex+2]);
                CONVERT____CPSS_PACKET_CMD_ENT___TO__CPSS_MAC_TABLE_CMD_ENT____MAC(fdbEntry.saCommand,inFields[fieldIndex+3]);
                inFields[fieldIndex+4]=fdbEntry.daRoute;
                inFields[fieldIndex+5]=fdbEntry.mirrorToRxAnalyzerPortEn;
                inFields[fieldIndex+6]=fdbEntry.sourceId;
                inFields[fieldIndex+7]=fdbEntry.userDefined;
                inFields[fieldIndex+8]=fdbEntry.daQosIndex;
                inFields[fieldIndex+9]=fdbEntry.saQosIndex;
                inFields[fieldIndex+10]=fdbEntry.daSecurityLevel;
                inFields[fieldIndex+11]=fdbEntry.saSecurityLevel;
                inFields[fieldIndex+12]=fdbEntry.appSpecificCpuCode;
                inFields[fieldIndex+13]=fdbEntry.pwId;
                inFields[fieldIndex+14]=agedPtr;
                CONVERT_BACK_DEV_PORT_DATA_MAC(associatedDevNumPtr,
                                     dummyPort);
                inFields[fieldIndex+15]=associatedDevNumPtr;
                if(isExt==GT_FALSE)
        /* pack and output table fields */
        fieldOutput("%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                        inFields[0],  inFields[1],  inFields[2],
                        fdbEntry.key.key.macFid.macAddr.arEther,
                        inFields[4],  inFields[5],  inFields[6],
                        inFields[7],  inFields[8],  inFields[9],
                        inFields[10], inFields[11], inFields[12],
                        inFields[13], inFields[14], inFields[15],
                        inFields[16], inFields[17], inFields[18],
                        inFields[19], inFields[20], inFields[21],
                        inFields[22], inFields[23], inFields[24],
                        inFields[25], inFields[26],inFields[27],
                        inFields[28], inFields[29], inFields[30],
                        inFields[31], inFields[32]);

                else
            fieldOutput("%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                        inFields[0],  inFields[1],  inFields[2],
                        fdbEntry.key.key.macFid.macAddr.arEther,
                        inFields[4],  inFields[5],  inFields[6],
                        inFields[7],  inFields[8],  inFields[9],
                        inFields[10], inFields[11], inFields[12],
                        inFields[13], inFields[14], inFields[15],
                        inFields[16], inFields[17], inFields[18],
                        inFields[19], inFields[20], inFields[21],
                        inFields[22], inFields[23], inFields[24],
                        inFields[25], inFields[26],inFields[27],
                        inFields[28], inFields[29], inFields[30],
                        inFields[31]);

        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
                galtisOutput(outArgs, result, "");
    }


    return CMD_OK;
}
/*********************************************************************************/
static CMD_STATUS wrCpssExMxPm2BrgFdbEntryReadFirst

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    isExt=GT_FALSE;

    return prvWrCpssExMxPm2BrgFdbEntryReadFirst(inArgs,inFields,numFields,outArgs);

}
/*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2BrgFdbEntryReadNext

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    isExt=GT_FALSE;

    return prvWrCpssExMxPm2BrgFdbEntryReadNext(inArgs,inFields,numFields,outArgs);

}
/*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2BrgFdbEntryExtReadFirst

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    isExt=GT_TRUE;

    return prvWrCpssExMxPm2BrgFdbEntryReadFirst(inArgs,inFields,numFields,outArgs);

}
/*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2BrgFdbEntryExtReadNext

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    isExt=GT_TRUE;

    return prvWrCpssExMxPm2BrgFdbEntryReadNext(inArgs,inFields,numFields,outArgs);

}
/*******************************************************************************
* cpssExMxPm2BrgFdbEntryDelete
*
* DESCRIPTION:
*       Delete an old entry in Hardware FDB table through Address Update
*       message.(AU message to the PP is non direct access to FDB table).
*       The function use New Address message (NA) format with skip bit set to 1.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum          - device number
*       fdbEntryKeyPtr  - pointer to key parameters of the FDB entry
*                         according to the entry type:
*                         MAC Address entry -> MAC Address + vlan ID.
*                         IPv4/IPv6 entry -> srcIP + dstIP + vald ID.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or vlanId
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       Application should synchronize call of cpssExMxPmBrgFdbEntrySet,
*       cpssExMxPmBrgFdbQaSend and cpssExMxPmBrgFdbEntryDelete functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the AU message processing has completed by
*       cpssExMxPmBrgFdbFromCpuAuMsgStatusGet.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2BrgFdbEntryDeleteByMacAddr

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS                           result;

    GT_U8                               devNum;
    CPSS_EXMXPM_FDB_ENTRY_KEY_STC           fdbEntryKey;
    GT_BYTE_ARRY                        sipBArr, dipBArr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    fdbEntryKey.entryType = (CPSS_EXMXPM_FDB_ENTRY_TYPE_ENT)inFields[2];

    switch(inFields[2])
    {
    case CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E:
    case CPSS_EXMXPM_FDB_ENTRY_TYPE_VPLS_E:

        galtisMacAddr(&fdbEntryKey.key.macFid.macAddr,
                        (GT_U8*)inFields[3]);
        fdbEntryKey.key.macFid.fId = (GT_U32)inFields[4];
        break;

    case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E:
    case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E:

        fdbEntryKey.key.ipMcast.fId = (GT_U32)inFields[4];

        galtisBArray(&sipBArr, (GT_U8*)inFields[5]);

        cmdOsMemCpy(
            fdbEntryKey.key.ipMcast.sip, sipBArr.data, 4);

        galtisBArray(&dipBArr, (GT_U8*)inFields[6]);

        cmdOsMemCpy(
            fdbEntryKey.key.ipMcast.dip, dipBArr.data, 4);
        break;

    default:
        break;
    }

    /* call cpss api function */

    result = fdbEntryDelete(devNum, &fdbEntryKey);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrCpssExMxPm2BrgFdbEntryDelete
*
* DESCRIPTION:
*   The function calls wrCpssExMxPmBrgFdbEntryDeleteByIndex or
*   wrCpssExMxPmBrgFdbEntryDeleteByMacAddr according to user's
*   preference as set in inArgs[1].
*
*
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2BrgFdbEntryDelete
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    /* the table resides only in this comment  */
    /* don't remove it                         */

    /* switch by byIndex boolean in inArgs[1]  */

    switch (inArgs[1])
    {
        case 0:
                return wrCpssExMxPm2BrgFdbEntryDeleteByMacAddr(
                        inArgs, inFields ,numFields ,outArgs);
        case 1:
                return wrCpssExMxPmBrgFdbEntryDeleteByIndex(
                        inArgs, inFields ,numFields ,outArgs);

        default: return CMD_AGENT_ERROR;
    }


}


/* cpssExMxPmBrgFdbQa   Table   [DB]   */

/*******************************************************************************
* cpssExMxPmBrgFdbQaSend
*
* DESCRIPTION:
*       The function Send Query Address (QA) message to the hardware FDB
*       table.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that QA message was processed by PP.
*       The PP sends Query Response message after QA processing.
*       An application can get QR message by general AU message get API.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum          - device number
*       fdbEntryKeyPtr  - pointer to FDB entry key
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum/vlan
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       Application should synchronize call of cpssExMxPmBrgFdbEntrySet,
*       cpssExMxPmBrgFdbQaSend and cpssExMxPmBrgFdbEntryDelete functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the QA message processing has completed by
*       cpssExMxPmBrgFdbFromCpuAuMsgStatusGet.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbQaSend

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS                           result;

    GT_U8                               devNum;
    CPSS_EXMXPM_FDB_ENTRY_KEY_STC       fdbEntryKey;
    GT_BYTE_ARRY                        sipBArr, dipBArr;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    fdbEntryKey.entryType = (CPSS_EXMXPM_FDB_ENTRY_TYPE_ENT)inFields[0];

    switch(inFields[0])
    {
    case CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E:
    case CPSS_EXMXPM_FDB_ENTRY_TYPE_VPLS_E:

        galtisMacAddr(&fdbEntryKey.key.macFid.macAddr,
                        (GT_U8*)inFields[1]);
        fdbEntryKey.key.macFid.fId = (GT_U32)inFields[2];
        break;

    case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E:
    case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E:

        fdbEntryKey.key.ipMcast.fId = (GT_U32)inFields[2];

        galtisBArray(&sipBArr, (GT_U8*)inFields[3]);

        cmdOsMemCpy(
            fdbEntryKey.key.ipMcast.sip, sipBArr.data, 4);

        galtisBArray(&dipBArr, (GT_U8*)inFields[4]);

        cmdOsMemCpy(
            fdbEntryKey.key.ipMcast.dip, dipBArr.data, 4);
        break;

    default:
        break;
    }

    /* call cpss api function */

    result = cpssExMxPmBrgFdbQaSend(devNum, &fdbEntryKey);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgFdbEntryInvalidate   [DB]
*
* DESCRIPTION:
*       Invalidate an entry in Hardware FDB address table in specified index.
*       the invalidation done by resetting to first word of the entry
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum      - device number
*       index       - hw fdb entry index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum
*       GT_OUT_OF_RANGE - index out of range
*
* COMMENTS:
*       May invalidate neighbour entries in the same hash chain
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbEntryInvalidate

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8       devNum;
    GT_U32      index;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    /* call cpss api function */
    /* call cpss api function */
    if(index < PRV_CPSS_EXMXPM_PP_MAC(devNum)->moduleCfg.bridgeCfg.fdbNumOfEntries)
    {
        result = cpssExMxPmBrgFdbEntryInvalidate(devNum, index);
    }
    else
    {
        result = cpssExMxPmBrgFdbTcamEntryInvalidate(devNum,
                index - PRV_CPSS_EXMXPM_PP_MAC(devNum)->moduleCfg.bridgeCfg.fdbNumOfEntries);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrCpssExMxPmBrgFdbLutEntrySet
*
* DESCRIPTION:
*       This function writes FDB LUT (Lookup table) entry in Hardware in
*       specified index. Used only when external FDB is used.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number
*       index       - HW LUT entry index
*       lutEntryPtr - Pointer to LUT entry parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index out of range, fdbEntryOffset or
*                                  lutSecHashValue of one of the sub entries is
*                                  out of range
*       GT_BAD_STATE             - when external FDB not initiated
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS  wrCpssExMxPmBrgFdbLutEntrySet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8       devNum;
    GT_U32      index;
    CPSS_EXMXPM_FDB_LUT_ENTRY_STC lutEntry;
    GT_U32  ii;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* arguments */
    devNum = (GT_U8)inArgs[0];

    /* fields */
    index = inFields[0];

    for(ii = 0 ; ii < CPSS_EXMXPM_FDB_LUT_SUB_ENTRIES_NUM_CNS ; ii++)
    {
        lutEntry.subEntry[ii].lutSecHashValue = inFields[1+(ii*3)];
        lutEntry.subEntry[ii].fdbEntryOffset  = inFields[2+(ii*3)];
        lutEntry.subEntry[ii].isValid         = inFields[3+(ii*3)];
    }


    /* call cpss api function */
    rc = cpssExMxPmBrgFdbLutEntryWrite(devNum, index , &lutEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}


/*******************************************************************************
* internalFdbLutEntryGetNext
*
* DESCRIPTION:
*       This function reads FDB LUT (Lookup table) entry from  Hardware
*       specified index. Used only when external FDB is used.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number
*       index       - HW LUT entry index
*
* OUTPUTS:
*       lutEntryPtr - Pointer to LUT entry parameters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index out of range
*       GT_BAD_STATE             - when external FDB not initiated
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS internalFdbLutEntryGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8    devNum;
    CPSS_EXMXPM_FDB_LUT_ENTRY_STC lutEntry;
    GT_U32  ii;

    if(inArgs[0] > 255)
    {
        /* we cant convert to GT_U8 */
        galtisOutput(outArgs, GT_BAD_PARAM, "");

        return CMD_OK;
    }

    /* arguments */
    devNum = (GT_U8)inArgs[0];


    while(1)
    {
        rc = cpssExMxPmBrgFdbLutEntryRead(devNum,currFdbLutIndex,&lutEntry);
        if(rc == GT_OUT_OF_RANGE)
        {
            /* end of table */
            rc = GT_OK;

            galtisOutput(outArgs, rc, "%d", -1);
            return CMD_OK;
        }
        else if(rc != GT_OK)
        {
            galtisOutput(outArgs, rc, "");
            return CMD_OK;
        }

        currFdbLutIndex++;

        for(ii = 0 ; ii < CPSS_EXMXPM_FDB_LUT_SUB_ENTRIES_NUM_CNS;ii++)
        {
            if(lutEntry.subEntry[ii].isValid == GT_TRUE)
            {
                /* one of the sub entry is valid , so we print full entry */
                break;
            }
        }

        if(ii != CPSS_EXMXPM_FDB_LUT_SUB_ENTRIES_NUM_CNS)
        {
            /*we have entry with at least one valid sub entry to return top caller */
            break;
        }
    }


    /*fieldOutputSetAppendMode(); --> don't start with this call before first
      call to fieldOutput */
    fieldOutput("%d",(currFdbLutIndex-1));

    for(ii = 0 ; ii < CPSS_EXMXPM_FDB_LUT_SUB_ENTRIES_NUM_CNS;ii++)
    {
        fieldOutputSetAppendMode();
        fieldOutput("%d%d%d",
            lutEntry.subEntry[ii].lutSecHashValue,
            lutEntry.subEntry[ii].fdbEntryOffset,
            lutEntry.subEntry[ii].isValid);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrCpssExMxPmBrgFdbLutEntryGetFirst
*
* DESCRIPTION:
*       This function reads FDB LUT (Lookup table) entry from  Hardware
*       specified index. Used only when external FDB is used.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number
*       index       - HW LUT entry index
*
* OUTPUTS:
*       lutEntryPtr - Pointer to LUT entry parameters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index out of range
*       GT_BAD_STATE             - when external FDB not initiated
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbLutEntryGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    currFdbLutIndex = 0;

    return internalFdbLutEntryGetNext(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* wrCpssExMxPmBrgFdbLutEntryGetNext
*
* DESCRIPTION:
*       This function reads FDB LUT (Lookup table) entry from  Hardware
*       specified index. Used only when external FDB is used.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number
*       index       - HW LUT entry index
*
* OUTPUTS:
*       lutEntryPtr - Pointer to LUT entry parameters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index out of range
*       GT_BAD_STATE             - when external FDB not initiated
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbLutEntryGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return internalFdbLutEntryGetNext(inArgs,inFields,numFields,outArgs);
}


/*******************************************************************************
* cpssExMxPmBrgFdbAaAndTaToCpuSet   [DB]
*
* DESCRIPTION:
*       Enable/Disable the PP to/from sending an AA and TA address
*       update messages to the CPU.
*
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum      - device number
*       enable      - enable or disable the message
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or auMsgType
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbAaAndTaToCpuSet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbAaAndTaToCpuSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbAaAndTaToCpuGet   [DB]
*
* DESCRIPTION:
*       Get state of sending an AA and TA address update messages to the CPU
*       as configured to PP.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE - the messages to CPU enabled
*                                  GT_FALSE - the messages to CPU disabled
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or auMsgType
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbAaAndTaToCpuGet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enablePtr;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbAaAndTaToCpuGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbNaToCpuSet    [DB]
*
* DESCRIPTION:
*       Enables/disables forwarding a new mac address message to the CPU.
*
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE  - the messages to CPU enabled
*                     GT_FALSE - the messages to CPU disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbNaToCpuSet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbNaToCpuSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbNaToCpuGet    [DB]
*
* DESCRIPTION:
*       Get state of forwarding a new mac address message to the CPU.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE - the messages to CPU enabled
*                                  GT_FALSE - the messages to CPU disabled
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbNaToCpuGet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enablePtr;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbNaToCpuGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbQaUnicastToCpuSet [DB]
*
* DESCRIPTION:
*       Enable/Disable the PP to sending QA unicast destination address
*       query messages to the CPU.
*       The PP sends QA message to CPU when DA lookup has no match for unicast
*       packet.
*
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE - enable sending QA unicast destination address
*                               query messages to the CPU.
*                   - GT_FALSE - disable sending QA unicast destination address
*                               query messages to the CPU.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or auMsgType
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbQaUnicastToCpuSet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbQaUnicastToCpuSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbQaUnicastToCpuGet [DB]
*
* DESCRIPTION:
*       Get state of sending QA unicast destination address query messages to
*       the CPU as configured to PP.
*       The PP sends QA message to CPU when DA lookup has no match for unicast
*       packet.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE - sending QA unicast destination address
*                                            query messages to the CPU is enabled
*                                  GT_FALSE - sending QA unicast destination address
*                                            query messages to the CPU is disabled
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or auMsgType
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbQaUnicastToCpuGet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enablePtr;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbQaUnicastToCpuGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbQaMulticastToCpuSet   [DB]
*
* DESCRIPTION:
*       Enable/Disable the PP to/from sending QA Multicast destination address
*       query messages to the CPU.
*       The PP sends QA message to CPU when DA lookup has no match for multicast
*       packet.
*
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE - enable sending QA Multicast destination address
*                               query messages to the CPU.
*                   - GT_FALSE - disable sending QA Multicast destination address
*                               query messages to the CPU.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or auMsgType
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbQaMulticastToCpuSet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbQaMulticastToCpuSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgFdbQaMulticastToCpuGet   [DB]
*
* DESCRIPTION:
*       Get state of sending QA Multicast destination address query messages to
*       the CPU as configured to PP.
*       The PP sends QA message to CPU when DA lookup has no match for multicast
*       packet.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE - sending QA Multicast destination address
*                                            query messages to the CPU is enabled
*                                  GT_FALSE - sending QA Multicast destination address
*                                            query messages to the CPU is disabled
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or auMsgType
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbQaMulticastToCpuGet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enablePtr;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbQaMulticastToCpuGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbSpAaMsgToCpuSet   [DB]
*
* DESCRIPTION:
*       Enable/Disable sending AA messages to the CPU indicating that the
*       device aged-out storm prevention FDB entry.
*
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE - enable sending AA messages to the CPU
*                   - GT_FALSE - disable sending AA messages to the CPU
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbSpAaMsgToCpuSet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbSpAaMsgToCpuSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgFdbSpAaMsgToCpuGet   [DB]
*
* DESCRIPTION:
*       Get the state of sending AA messages to the CPU when device aged-out
*       storm prevention FDB entry.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE - sending AA messages to the CPU
*                                            is enabled
*                                  GT_FALSE - sending AA messages to the CPU
*                                            is disabled
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbSpAaMsgToCpuGet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enablePtr;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbSpAaMsgToCpuGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbSpEntriesDelEnableSet [DB]
*
* DESCRIPTION:
*       This function globally enable/disable the SP entries deletion by
*       FDB pass. The SP (storm prevent) entry is deleted each time the FDB
*       pass is performed (aging, deleting or transplaning pass).
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE - enable SP entries deletion on FDB pass.
*                     GT_FALSE - disable SP entries deletion on FDB pass.
*
* OUTPUTS:
*       None.
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbSpEntriesDelEnableSet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbSpEntriesDelEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbSpEntriesDelEnableGet [DB]
*
* DESCRIPTION:
*       This function gets the global enable/disable state of the SP entries
*       deletion by FDB pass. The SP (storm prevent) entry is deleted each time
*       the FDB pass is performed (aging, deleting or transplaning pass).
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr      - (pointer to) SP entries deletion state.
GT_TRUE - enable SP entries deletion on FDB pass.
*                        GT_FALSE - disable SP entries deletion on FDB pass.
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbSpEntriesDelEnableGet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enablePtr;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbSpEntriesDelEnableGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbNaMsgOnChainTooLongSet    [DB]
*
* DESCRIPTION:
*       Enable/Disable sending NA messages to the CPU indicating that the device
*       cannot learn a new SA . It has reached its max hop (bucket is full).
*
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE enable sending the message to the CPU
*                 GT_FALSE for disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbNaMsgOnChainTooLongSet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbNaMsgOnFailedLearningSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbNaMsgOnChainTooLongGet    [DB]
*
* DESCRIPTION:
*       Get the status (enabled/disabled) of sending NA messages to the CPU
*       indicating that the device cannot learn a new SA. It has reached its
*       max hop (bucket is full).
*
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - pointer to status of sending the NA message to the CPU
*                    GT_TRUE - enabled
*                    GT_FALSE - disabled
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbNaMsgOnChainTooLongGet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enablePtr;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbNaMsgOnFailedLearningGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbFromCpuAuMsgStatusGet
*
* DESCRIPTION:
*       Get status of FDB Address Update (AU) message processing in the PP.
*       The function checks the status of last AU message process:
*       completion and success.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum       - device number
*       msgType      - update message type
*
* OUTPUTS:
*       completedPtr - pointer to AU message processing completion.
*                      GT_TRUE - AU message was processed by PP.
*                      GT_FALSE - AU message processing is not completed yet by PP.
*       succeededPtr - pointer to a success of AU operation.
*                      GT_TRUE  - the AU action succeeded.
*                      GT_FALSE - the AU action has failed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or msgType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The succeed status is relevant only when completed.
*       An AU message sent by CPU can fail in the following cases:
*       1. The message type = CPSS_NA_E and the hash chain has reached it's
*          maximum length.
*       2. The message type = CPSS_QA_E and the FDB entry doesn't
*          exist.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2BrgFdbFromCpuAuMsgStatusGet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_UPD_MSG_TYPE_ENT msgType;
    GT_BOOL completed;
    GT_BOOL succeeded;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    msgType = (CPSS_UPD_MSG_TYPE_ENT)inArgs[1];



    /* call cpss api function */
    result = cpssExMxPmBrgFdbFromCpuAuMsgStatusGet(devNum, msgType, &completed, &succeeded);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", completed, succeeded);

    return CMD_OK;
}
/*****************************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbFromCpuAuMsgStatusGet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_UPD_MSG_TYPE_ENT msgType;
    GT_BOOL completed;
    GT_BOOL succeeded;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    msgType = CPSS_NA_E;



    /* call cpss api function */
    result = cpssExMxPmBrgFdbFromCpuAuMsgStatusGet(devNum, msgType, &completed, &succeeded);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", completed, succeeded);

    return CMD_OK;
}


/* cpssExMxPmBrgFdbAuMsgBlock table   global variable   [DB]  */
#define MAX_NUM_OF_AU_MSG  128

static GT_U32   indAu;
static GT_U32   numOfAuPtr;
static CPSS_EXMXPM_FDB_UPDATE_MSG_ENTRY_STC  auMessagesArr[MAX_NUM_OF_AU_MSG];

/*
* cpssExMxPmBrgFdbAuMsgBlockGet
*
* DESCRIPTION:
*       The function returns a block (array) of FDB Address Update (AU) messages,
*       the max number of elements defined by the caller.
*       The PP may place AU messages in common FDB Upload (FU) messages
*       queue or in separate queue only for AU messages. The function
*       cpssExMxPmHwPpPhase2Init configures queue for AU messages by the
*       fuqUseSeparate parameter. If common FU queue is used then function
*       returns all AU messages in the queue including FU ones.
*       If separate AU queue is used then function returns only AU messages.
*
*  APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum     - the device number from which AU are taken
*       numOfAuPtr - (pointer to)max number of AU messages to get
*
* OUTPUTS:
*       numOfAuPtr - (pointer to)actual number of AU messages that were received
*       auMessagesPtr - array that holds received AU messages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfAuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK        - on success
*       GT_NO_MORE   - the action succeeded and there are no more waiting
*                      AU messages
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error.
*
* COMMENTS:
*       If a common queue is used the block will contain AU and FU messages.
*       In this case there is no difference between using either function -
*       cpssDxChBrgFdbAuMsgBlockGet or cpssExMxPmBrgFdbFuMsgBlockGet.
*       If separate AU queue is used then the block contains only AU messages.
*
*******************************************************************************/


static CMD_STATUS wrCpssExMxPmBrgFdbAuMsgBlockGetNext

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS                             result=GT_OK;

    GT_U8                                 devNum;
    GT_U32                                tempSip, tempDip;
    GT_U8                                 tempDev,tempPort;
    GT_U8                          dummyPort = 0;/* dummy port num for conversion */


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    if (indAu > numOfAuPtr-1) {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }



    inFields[0] = indAu;

    inFields[1] = auMessagesArr[indAu].updType;
    inFields[2] = auMessagesArr[indAu].entryWasFound;
    inFields[3] = auMessagesArr[indAu].fdbEntryIndex;
    inFields[4]=  auMessagesArr[indAu].fdbEntry.key.entryType;

        switch(inFields[4])
        {
        case CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E:
        case CPSS_EXMXPM_FDB_ENTRY_TYPE_VPLS_E:

        inFields[6]=auMessagesArr[indAu].fdbEntry.key.key.macFid.fId ;
                break;

        case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E:
        case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E:

                inFields[6]=auMessagesArr[indAu].fdbEntry.key.key.ipMcast.fId;

                cmdOsMemCpy(&tempSip, &auMessagesArr[indAu].fdbEntry.key.key.ipMcast.sip[0], 4);
                tempSip = cmdOsHtonl(tempSip);
                inFields[7] = tempSip;

                cmdOsMemCpy(&tempDip, &auMessagesArr[indAu].fdbEntry.key.key.ipMcast.dip[0], 4);
                tempDip = cmdOsHtonl(tempDip);
                inFields[8] = tempDip;

                break;

        default:
                break;
        }

        inFields[9]=auMessagesArr[indAu].fdbEntry.dstOutlif.outlifType;



        switch(auMessagesArr[indAu].fdbEntry.dstOutlif.outlifType)
        {
        case CPSS_EXMXPM_OUTLIF_TYPE_LL_E:
                inFields[10]=auMessagesArr[indAu].fdbEntry.dstOutlif.outlifPointer.arpPtr;
                break;
        case CPSS_EXMXPM_OUTLIF_TYPE_DIT_E:
                inFields[11]=auMessagesArr[indAu].fdbEntry.dstOutlif.outlifPointer.ditPtr;
                break;
        case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:

                inFields[12]=auMessagesArr[indAu].fdbEntry.dstOutlif.outlifPointer.tunnelStartPtr.passengerPacketType;
                inFields[13]=auMessagesArr[indAu].fdbEntry.dstOutlif.outlifPointer.tunnelStartPtr.ptr;
                break;
        default:
                break;
        }


        auMessagesArr[indAu].fdbEntry.dstOutlif.interfaceInfo.type = (CPSS_INTERFACE_TYPE_ENT)inFields[14];

        switch(inFields[14])
        {
        case 0:
                tempDev=auMessagesArr[indAu].fdbEntry.dstOutlif.interfaceInfo.devPort.devNum;
                tempPort=auMessagesArr[indAu].fdbEntry.dstOutlif.interfaceInfo.devPort.portNum ;
                CONVERT_BACK_DEV_PORT_DATA_MAC(tempDev,tempPort);
                inFields[15]=tempDev;
                inFields[16]=tempPort;
                break;

        case 1:
                inFields[17]=auMessagesArr[indAu].fdbEntry.dstOutlif.interfaceInfo.trunkId;
                break;

        case 2:
                inFields[18]=auMessagesArr[indAu].fdbEntry.dstOutlif.interfaceInfo.vidx;
                break;

        case 3:
                inFields[19]=auMessagesArr[indAu].fdbEntry.dstOutlif.interfaceInfo.vlanId;
                break;

        default:
                break;
        }

    inFields[20]=auMessagesArr[indAu].fdbEntry.isStatic ;
    inFields[21]=auMessagesArr[indAu].fdbEntry.daCommand ;CPSS_TBD_BOOKMARK
    inFields[22]=auMessagesArr[indAu].fdbEntry.saCommand ;CPSS_TBD_BOOKMARK
    inFields[23]=auMessagesArr[indAu].fdbEntry.daRoute;
    inFields[24]=auMessagesArr[indAu].fdbEntry.mirrorToRxAnalyzerPortEn;
    inFields[25]=auMessagesArr[indAu].fdbEntry.sourceId;
    inFields[26]=auMessagesArr[indAu].fdbEntry.userDefined;
    inFields[27]=auMessagesArr[indAu].fdbEntry.daQosIndex;
    inFields[28]=auMessagesArr[indAu].fdbEntry.saQosIndex;
    inFields[29]=auMessagesArr[indAu].fdbEntry.daSecurityLevel;
    inFields[30]=auMessagesArr[indAu].fdbEntry.saSecurityLevel;
    inFields[31]=auMessagesArr[indAu].fdbEntry.appSpecificCpuCode;
    inFields[32]=auMessagesArr[indAu].fdbEntry.pwId;

    inFields[33] = auMessagesArr[indAu].skip;
    inFields[34] = auMessagesArr[indAu].aging;
    CONVERT_BACK_DEV_PORT_DATA_MAC(auMessagesArr[indAu].associatedDevNum,
                         dummyPort);
    inFields[35] = auMessagesArr[indAu].associatedDevNum;
    inFields[36] = auMessagesArr[indAu].learningFailed;
    inFields[37] = auMessagesArr[indAu].entryOffset;
    inFields[38] = auMessagesArr[indAu].isTcam;



    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                          inFields[0],  inFields[1],
                inFields[2],  inFields[3],  inFields[4],
                auMessagesArr[indAu].fdbEntry.key.key.macFid.macAddr.arEther,
                inFields[6],  inFields[7],  inFields[8],  inFields[9],
                inFields[10], inFields[11], inFields[12], inFields[13],
                inFields[14], inFields[15], inFields[16], inFields[17],
                inFields[18], inFields[19], inFields[20], inFields[21],
                inFields[22], inFields[23], inFields[24], inFields[25],
                inFields[26], inFields[27], inFields[28], inFields[29],
                inFields[30], inFields[31], inFields[32],inFields[33],
        inFields[34], inFields[35],inFields[36],inFields[37],inFields[38]);


    indAu++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}
/******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbAuMsgBlockGetFirst

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS                             result;
    GT_U8                                 devNum;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    numOfAuPtr = (GT_U32)inArgs[1];

    indAu=0;

    if ( numOfAuPtr>MAX_NUM_OF_AU_MSG) {
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : number of entries is bigger than maximum.\n");
        galtisOutput(outArgs, GT_OUT_OF_RANGE, "%d", -1);
        return CMD_OK;
    }

    if ( numOfAuPtr==0) {
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : number of entries is zero.\n");
        galtisOutput(outArgs, GT_OUT_OF_RANGE, "%d", -1);
        return CMD_OK;
    }

    cmdOsMemSet(&auMessagesArr[indAu], 0, sizeof(auMessagesArr[indAu])*numOfAuPtr);

    /* call cpss api function */
    wrapCpssExMxPmBrgAuqMutexLock();
    result = cpssExMxPmBrgFdbAuMsgBlockGet(devNum, &numOfAuPtr, auMessagesArr);
    wrapCpssExMxPmBrgAuqMutexUnLock();

    if (result != GT_OK) {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }


    return wrCpssExMxPmBrgFdbAuMsgBlockGetNext(inArgs,inFields,numFields,outArgs);
}


/* cpssExMxPmBrgFdbFuMsgBlock   Table global variable   [DB] */
#define MAX_NUM_OF_FU_MSG  128

static GT_U32   indFu;
static GT_U32   numOfFuPtr;
static CPSS_EXMXPM_FDB_UPDATE_MSG_ENTRY_STC  fuMessagesArr[MAX_NUM_OF_FU_MSG];

/*
* cpssExMxPmBrgFdbFuMsgBlockGet
*
* DESCRIPTION:
*       The function returns a block (array) of FDB Address Update (AU) messages,
*       the max number of elements defined by the caller.
*       The PP may place FU messages in common FDB Address Update (AU) messages
*       queue or in separate queue only for FU messages. The function
*       cpssExMxPmHwPpPhase2Init configures queue for FU messages by the
*       fuqUseSeparate parameter. If common AU queue is used then function
*       returns all AU messages in the queue including FU ones.
*       If separate FU queue is used then function returns only FU messages.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum     - the device number from which FU are taken
*       numOfFuPtr - (pointer to)max number of FU messages to get
*
* OUTPUTS:
*       numOfAuPtr - (pointer to)actual number of FU messages that were received
*       fuMessagesPtr - array that holds received FU messages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfFuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK           - on success
*       GT_NO_MORE      - the action succeeded and there are no more waiting
*                         FU messages
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error.
*
* COMMENTS:
*       If a common queue is used the block will contain AU and FU messages.
*       In this case there is no difference between using either function -
*       cpssDxChBrgFdbAuMsgBlockGet or cpssExMxPmBrgFdbFuMsgBlockGet.
*       If separate FU queue is used then the block contains only FU messages.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbFuMsgBlockGetNext

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS                             result=GT_OK;

    GT_U8                                 devNum;
    GT_U32                                tempSip, tempDip;
    GT_U8                                 tempDev,tempPort;
    GT_U8                          dummyPort = 0;/* dummy port num for conversion */

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    if (indFu > numOfFuPtr-1) {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = indFu;

    inFields[1] = fuMessagesArr[indFu].updType;
    inFields[2] = fuMessagesArr[indFu].entryWasFound;
    inFields[3] = fuMessagesArr[indFu].fdbEntryIndex;
    inFields[4]=  fuMessagesArr[indFu].fdbEntry.key.entryType;

    switch(inFields[4])
    {
    case CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E:
    case CPSS_EXMXPM_FDB_ENTRY_TYPE_VPLS_E:

        inFields[6]=fuMessagesArr[indFu].fdbEntry.key.key.macFid.fId ;
        break;

    case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E:
    case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E:

        inFields[6]=fuMessagesArr[indFu].fdbEntry.key.key.ipMcast.fId;

        cmdOsMemCpy(&tempSip, &fuMessagesArr[indFu].fdbEntry.key.key.ipMcast.sip[0], 4);
        tempSip = cmdOsHtonl(tempSip);
        inFields[7] = tempSip;

        cmdOsMemCpy(&tempDip, &fuMessagesArr[indFu].fdbEntry.key.key.ipMcast.dip[0], 4);
        tempDip = cmdOsHtonl(tempDip);
        inFields[8] = tempDip;

        break;

    default:
        break;
    }

    inFields[9]=fuMessagesArr[indFu].fdbEntry.dstOutlif.outlifType;



    switch(fuMessagesArr[indFu].fdbEntry.dstOutlif.outlifType)
    {
    case CPSS_EXMXPM_OUTLIF_TYPE_LL_E:
        inFields[10]=fuMessagesArr[indFu].fdbEntry.dstOutlif.outlifPointer.arpPtr;
        break;
    case CPSS_EXMXPM_OUTLIF_TYPE_DIT_E:
        inFields[11]=fuMessagesArr[indFu].fdbEntry.dstOutlif.outlifPointer.ditPtr;
        break;
    case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:

        inFields[12]=fuMessagesArr[indFu].fdbEntry.dstOutlif.outlifPointer.tunnelStartPtr.passengerPacketType;
        inFields[13]=fuMessagesArr[indFu].fdbEntry.dstOutlif.outlifPointer.tunnelStartPtr.ptr;
        break;
    default:
        break;
    }


    inFields[14]=fuMessagesArr[indFu].fdbEntry.dstOutlif.interfaceInfo.type;/**/

    switch(inFields[14])
    {
    case 0:
        tempDev=fuMessagesArr[indFu].fdbEntry.dstOutlif.interfaceInfo.devPort.devNum;
        tempPort=fuMessagesArr[indFu].fdbEntry.dstOutlif.interfaceInfo.devPort.portNum ;
        CONVERT_BACK_DEV_PORT_DATA_MAC(tempDev,tempPort);
        inFields[15]=tempDev;
        inFields[16]=tempPort;
        break;

    case 1:
        inFields[17]=fuMessagesArr[indFu].fdbEntry.dstOutlif.interfaceInfo.trunkId;
        break;

    case 2:
        inFields[18]=fuMessagesArr[indFu].fdbEntry.dstOutlif.interfaceInfo.vidx;
        break;

    case 3:
        inFields[19]=fuMessagesArr[indFu].fdbEntry.dstOutlif.interfaceInfo.vlanId;
        break;

    default:
        break;
    }

    inFields[20]=fuMessagesArr[indFu].fdbEntry.isStatic ;
    inFields[21]=fuMessagesArr[indFu].fdbEntry.daCommand ;CPSS_TBD_BOOKMARK
    inFields[22]=fuMessagesArr[indFu].fdbEntry.saCommand ;CPSS_TBD_BOOKMARK
    inFields[23]=fuMessagesArr[indFu].fdbEntry.daRoute;
    inFields[24]=fuMessagesArr[indFu].fdbEntry.mirrorToRxAnalyzerPortEn;
    inFields[25]=fuMessagesArr[indFu].fdbEntry.sourceId;
    inFields[26]=fuMessagesArr[indFu].fdbEntry.userDefined;
    inFields[27]=fuMessagesArr[indFu].fdbEntry.daQosIndex;
    inFields[28]=fuMessagesArr[indFu].fdbEntry.saQosIndex;
    inFields[29]=fuMessagesArr[indFu].fdbEntry.daSecurityLevel;
    inFields[30]=fuMessagesArr[indFu].fdbEntry.saSecurityLevel;
    inFields[31]=fuMessagesArr[indFu].fdbEntry.appSpecificCpuCode;
    inFields[32]=fuMessagesArr[indFu].fdbEntry.pwId;

    inFields[33] = fuMessagesArr[indFu].skip;
    inFields[34] = fuMessagesArr[indFu].aging;
    CONVERT_BACK_DEV_PORT_DATA_MAC(fuMessagesArr[indFu].associatedDevNum,
                         dummyPort);
    inFields[35] = fuMessagesArr[indFu].associatedDevNum;
    inFields[36] = fuMessagesArr[indFu].learningFailed;
    inFields[37] = fuMessagesArr[indFu].entryOffset;
    inFields[38] = fuMessagesArr[indFu].isTcam;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                  inFields[0],  inFields[1],
        inFields[2],  inFields[3],  inFields[4],
        fuMessagesArr[indFu].fdbEntry.key.key.macFid.macAddr.arEther,
        inFields[6],  inFields[7],  inFields[8],  inFields[9],
        inFields[10], inFields[11], inFields[12], inFields[13],
        inFields[14], inFields[15], inFields[16], inFields[17],
        inFields[18], inFields[19], inFields[20], inFields[21],
        inFields[22], inFields[23], inFields[24], inFields[25],
        inFields[26], inFields[27], inFields[28], inFields[29],
        inFields[30], inFields[31], inFields[32],inFields[33],
        inFields[34], inFields[35],inFields[36],inFields[37],inFields[38]);

    indFu++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}
/******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbFuMsgBlockGetFirst

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS                             result;
    GT_U8                                 devNum;



    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    numOfFuPtr = (GT_U32)inArgs[1];
    indFu = 0;

    if ( numOfFuPtr>MAX_NUM_OF_FU_MSG) {
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : number of entries is bigger than maximum.\n");
        galtisOutput(outArgs, GT_OUT_OF_RANGE, "%d", -1);
        return CMD_OK;
    }

     if ( numOfFuPtr==0) {
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : number of entries is zero.\n");
        galtisOutput(outArgs, GT_OUT_OF_RANGE, "%d", -1);
        return CMD_OK;
    }

    cmdOsMemSet(&fuMessagesArr[indFu], 0, sizeof(fuMessagesArr[indFu])*numOfFuPtr);

    /* call cpss api function */
    result = cpssExMxPmBrgFdbFuMsgBlockGet(devNum, &numOfFuPtr, fuMessagesArr);

    if (result != GT_OK) {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }


    return  wrCpssExMxPmBrgFdbFuMsgBlockGetNext(inArgs,inFields,numFields,outArgs);
}

/************************************************************************
* cpssExMxPmBrgFdbAuqFuqMessagesNumberGet
*
* DESCRIPTION:
*       The function scan the AU/FU queues and returns the number of
*       AU/FU messages in the queue.
*
*
*  APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum     - the device number on which AU are counted
*       queueType - AUQ or FUQ. FUQ valid for Ch2 and above.
*
* OUTPUTS:
*       numOfAuPtr - (pointer to) number of AU messages in the specified queue.
*       endOfQueueReachedPtr - (pointer to) GT_TRUE: The queue reached to the end.
*                                        GT_FALSE: else
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In order to have the accurate number of entries  application should
*       protect Mutual exclusion between HW access to the AUQ/FUQ
*       SW access to the AUQ/FUQ and calling to this API.
*       i.e. application should stop the PP from sending AU messages to CPU.
*       and should not call  the api's
*       cpssExMxPmBrgFdbFuMsgBlockGet, cpssExMxPmBrgFdbFuMsgBlockGet
*
***********************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbAuqFuqMessagesNumberGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                          devNum;
    CPSS_EXMXPM_FDB_QUEUE_TYPE_ENT queueType;
    GT_U32                         numOfAu;
    GT_BOOL                        endOfQueueReached;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    queueType = (CPSS_EXMXPM_FDB_QUEUE_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbAuqFuqMessagesNumberGet(devNum,queueType,&numOfAu,&endOfQueueReached);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", numOfAu, endOfQueueReached);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbDropAuEnableSet   [DB]
*
* DESCRIPTION:
*       Enable/Disable dropping the Address Update messages when the queue is
*       full.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE - Drop the AU messages
*                 GT_FALSE - Do not drop the AU messages
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbDropAuEnableSet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbDropAuEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbDropAuEnableGet   [DB]
*
* DESCRIPTION:
*       Get the state of dropping the Address Update messages, when the queue is
*       full.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE - The AU messages are dropped
*                    GT_FALSE - The AU messages are not dropped
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbDropAuEnableGet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enablePtr;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbDropAuEnableGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbAuMsgRateLimitSet [DB]
*
* DESCRIPTION:
*     Set Address Update CPU messages rate limit.
*     The function sets the maximum number of AU messages queued within a fixed
*     10 ms window of  time. The limit ranges from a 2 AU messages per 10ms to
*     a maximum 2040 AU messages per 10 ms in a step of 2.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum     - device number
*       msgRate    - maximal rate of AU messages per second
*       enable     - enable/disable message limit
*                    GT_TRUE - message rate is limited with msgRate
*                    GT_FALSE - there is no limit, WA to CPU
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on bad parameter
*       GT_HW_ERROR        - on error.
*       GT_OUT_OF_RANGE    - out of range.
*
* COMMENTS:
*       In case message rate limit is disabled, the rate is don't care.
*
*       The calculation of rate limit is as following:
*       PP Clock is in Mhrz.
*       window (in uSec) = (10000 * baseClock) / coreClock;
*       rate = ((msgRate / granularity) * window) / 1000000;
*
*       granularity - 200 messages,
*       example: when PP Clock is 200 Mhz - rate limit is 2 - 204000 messages
*       per second.
*       granularity and rate changed linearly depending on clock
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbAuMsgRateLimitSet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U32   msgRate;
    GT_BOOL  enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    msgRate = (GT_U32)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbAuMsgRateLimitSet(devNum, msgRate, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgFdbAuMsgRateLimitGet [DB]
*
* DESCRIPTION:
*     Get Address Update CPU messages rate limit
*     The function gets the maximum number of AU messages queued within a fixed
*     10 ms window of  time. The limit ranges from a 2 AU messages per 10ms to
*     a maximum 2040 AU messages per 10 ms in a step of 2.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       msgRatePtr - (pointer to) maximal rate of AU messages per second
*       enablePtr  - (pointer to) GT_TRUE - message rate is limited with
*                                           msgRatePtr
*                                 GT_FALSE - there is no limit, WA to CPU
*
* RETURNS:
*       GT_OK              - on success.
*       GT_HW_ERROR        - on error.
*       GT_BAD_PARAM       - on bad parameter
*       GT_BAD_PTR         - one of the parameters is NULL pointer
*
* COMMENTS:
*       In case message rate limit is disabled, the rate is don't care.
*
*       The calculation of rate limit is as following:
*       PP Clock is in Mhrz.
*       window (in uSec) = (10000 * baseClock) / coreClock;
*       rate = ((msgRate / granularity) * window) / 1000000;
*
*       granularity - 200 messages,
*       example: when PP Clock is 200 Mhz - rate limit is 2 - 204000 messages
*       per second.
*       granularity and rate changed linearly depending on clock
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbAuMsgRateLimitGet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8       devNum;
    GT_U32      msgRatePtr;
    GT_BOOL     enablePtr;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbAuMsgRateLimitGet(devNum, &msgRatePtr, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", msgRatePtr, enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbSecureAutoLearnSet    [DB]
*
* DESCRIPTION:
*        Configure secure auto learning mode.
*        If Secure Automatic Learning is enabled, and a packet enabled for
*        automatic learning, then:
*        - If the source FDB address is found in the FDB, and the associated
*          location of the address has changed, then the FDB entry is updated
*          with the new location of the address.
*        - If the FDB entry is modified with the new location of the address,
*          a New Address Update message is sent to the CPU,
*          if enabled to do so on the port and the packet VLAN.
*        - If the source FDB address is NOT found in the FDB, then
*          Secure Automatic Learning Unknown Source command (Trap to CPU,
*          Soft Drop or Hard Drop)  is assigned to the packet according to mode
*          input parameter and packet treated as security breach event.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum  - device number
*       mode    - The command assigned to packet with new SA not found in FDB
*                 when secure automatic learning enable.
*                 CPSS_MAC_SECURE_AUTO_LEARN_DISABLED_E - forward (disable secure
*                 automatic learning)
*                 CPSS_MAC_SECURE_AUTO_LEARN_UNK_TRAP_E - trap to CPU packets
*                 with unknown MAC DA Trap to CPU (enable secure auto learning)
*                 CPSS_MAC_SECURE_AUTO_LEARN_UNK_SOFT_DROP_E - soft drop
*                 packets with unknown MAC DA. (enable secure auto learning)
*                 CPSS_MAC_SECURE_AUTO_LEARN_UNK_HARD_DROP_E - hard drop
*                 packets with unknown MAC DA (enable secure auto learning)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbSecureAutoLearnSet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT  mode;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbSecureAutoLearnSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgFdbSecureAutoLearnGet    [DB]
*
* DESCRIPTION:
*        Get secure auto learning mode.
*        If Secure Automatic Learning is enabled, and a packet enabled for
*        automatic learning, then:
*        - If the source FDB address is found in the FDB, and the associated
*          location of the address has changed, then the FDB entry is updated
*          with the new location of the address.
*        - If the FDB entry is modified with the new location of the address,
*          a New Address Update message is sent to the CPU,
*          if enabled to do so on the port and the packet VLAN.
*        - If the source FDB address is NOT found in the FDB, then
*          Secure Automatic Learning Unknown Source command (Trap to CPU,
*          Soft Drop or Hard Drop)  is assigned to the packet according to mode
*          input parameter and packet treated as security breach event.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       mode    - Pointer to the command assigned to packet with new SA not
*                 found in FDB  when secure automatic learning enable.
*                 CPSS_MAC_SECURE_AUTO_LEARN_DISABLED_E - forward (secure
*                 automatic learning is disabled)
*                 CPSS_MAC_SECURE_AUTO_LEARN_UNK_TRAP_E - trap to CPU packets
*                 with unknown MAC DA Trap to CPU (secure auto learning is enabled)
*                 CPSS_MAC_SECURE_AUTO_LEARN_UNK_SOFT_DROP_E - soft drop
*                 packets with unknown MAC DA. (secure auto learning is enabled)
*                 CPSS_MAC_SECURE_AUTO_LEARN_UNK_HARD_DROP_E - hard drop
*                 packets with unknown MAC DA (secure auto learning is enabled).
*
* RETURNS:
*       GT_OK               - on success
*       GT_HW_ERROR         - on hardware error
*       GT_BAD_PARAM        - on wrong input parameters
*       GT_BAD_PTR          - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbSecureAutoLearnGet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT  modePtr;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbSecureAutoLearnGet(devNum, &modePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", modePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbSecureAgingSet    [DB]
*
* DESCRIPTION:
*      Enable or disable secure aging. This mode is relevant to automatic or
*      triggered aging with removal of aged out FDB entries. This mode is
*      applicable together with secure auto learning mode see
*      cpssExMxPmBrgFdbSecureAutoLearnSet. In the secure aging mode the device
*      sets aged out unicast FDB entry with <Multiple> =  1 and VIDX = 0xfff but
*      not invalidates entry. This causes packets destined to this FDB entry FDB
*      address to be flooded to the VLAN.  But such packets are known ones.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE - enable secure aging
*                 GT_FALSE - disable secure aging
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK               - on success
*       GT_HW_ERROR         - on hardware error
*       GT_BAD_PARAM       - on wrong input parameters
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbSecureAgingSet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbSecureAgingSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbSecureAgingGet    [DB]
*
* DESCRIPTION:
*      Get the status (enabled/disabled) of secure aging.
*      This mode is relevant to automatic or triggered aging with removal of
*      aged out FDB entries. This mode is applicable together with secure auto
*      learning mode see cpssExMxPmBrgFdbSecureAutoLearnGet. In the secure aging
*      mode  the device sets aged out unicast FDB entry with <Multiple> =  1 and
*      VIDX = 0xfff but not invalidates entry. This causes packets destined to
*      this FDB entry FDB address to be flooded to the VLAN.
*      But such packets are known ones.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE - enable secure aging
*                    GT_FALSE - disable secure aging
*
* RETURNS:
*       GT_OK               - on success
*       GT_HW_ERROR         - on hardware error
*       GT_BAD_PARAM       - on wrong input parameters
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbSecureAgingGet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enablePtr;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbSecureAgingGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbDeviceTableSet    [DB]
*
* DESCRIPTION:
*       This function sets the device table of the PP.
*       the PP use this configuration in the FDB aging daemon .
*       once the aging daemon encounter an entry with non-exists devNum
*       associated with it, the daemon will DELETE the entry without sending
*       the CPU any notification.
*       a non-exists device determined by it's bit in the "device table"
*
*       So for proper work of PP the application must set the relevant bits of
*       all devices in the system prior to inserting FDB entries associated with
*       them
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum      - device number
*       devTable    - (Array of) bmp (bitmap) of devices to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbDeviceTableSet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U32   devTable[CPSS_EXMXPM_FDB_DEV_TABLE_SIZE_CNS];

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    devTable[0] = (GT_U32)inArgs[1];
    devTable[1] = (GT_U32)inArgs[2];
    devTable[2] = (GT_U32)inArgs[3];
    devTable[3] = (GT_U32)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbDeviceTableSet(devNum, devTable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbDeviceTableGet  [DB]
*
* DESCRIPTION:
*       This function gets the device table of the PP.
*       the PP use this configuration in the FDB aging daemon.
*       once the aging daemon encounter an entry with non-exists devNum
*       associated with it, the daemon will DELETE the entry without sending
*       the CPU any notification.
*       a non-exists device determined by it's bit in the "device table"
*
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       devTable    - pointer to (Array of) bmp (bitmap) of devices.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbDeviceTableGet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U32   devTable[CPSS_EXMXPM_FDB_DEV_TABLE_SIZE_CNS];


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbDeviceTableGet(devNum, devTable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", devTable[0], devTable[1],
                devTable[2], devTable[3]);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbActionsEnableSet  [DB]
*
* DESCRIPTION:
*       Enables/Disables FDB actions: flushing, deleting, uploading and
*       transplanting.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE -  Actions are enabled
*                 GT_FALSE -  Actions are disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong input parameters
*
* COMMENTS:
*       There is no check done of current status of action's register
*       It's application responsibility to check the status of Aging Trigger by
*       using cpssExMxPmBrgFdbTrigActionStatusGet API, in order to be
*       sure that that there is currently no action done.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbActionsEnableSet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    wrapCpssExMxPmBrgAuqMutexLock();
    result = cpssExMxPmBrgFdbActionsEnableSet(devNum, enable);
    wrapCpssExMxPmBrgAuqMutexUnLock();

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgFdbActionsEnableGet  [DB]
*
* DESCRIPTION:
*       Get the status of FDB actions: flushing, deleting, uploading and
*       transplanting.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE - actions are enabled
*                    GT_FALSE - actions are disabled
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbActionsEnableGet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enablePtr;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbActionsEnableGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgFdbActionModeSet [DB]
*
* DESCRIPTION:
*       Sets FDB action mode without setting Action Trigger
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum  - device number
*       mode    - FDB action mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*       There is no check done of current status of action's register
*       It's application responsibility to check the status of Aging Trigger by
*       using cpssExMxPmBrgFdbTrigActionStatusGet API, in order to be
*       sure that that there is currently no action done.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbActionModeSet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    CPSS_EXMXPM_FDB_ACTION_MODE_ENT      mode;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_EXMXPM_FDB_ACTION_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbActionModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbActionModeGet [DB]
*
* DESCRIPTION:
*       Gets FDB action mode.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr    - FDB action mode
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbActionModeGet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8                               devNum;
    CPSS_EXMXPM_FDB_ACTION_MODE_ENT     modePtr;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbActionModeGet(devNum, &modePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", modePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbTriggerModeSet    [DB]
*
* DESCRIPTION:
*       Sets FDB table Triggered\Automatic action mode.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum  - device number
*       mode    - action mode:
*                 CPSS_EXMXPM_FDB_TRIGGER_AUTO_E - Action is done Automatically.
*                 CPSS_EXMXPM_FDB_TRIGGER_TRIG_E - Action is done via a trigger
*                                                  from CPU.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on unsupported parameters
*       GT_BAD_PARAM     - on wrong mode
*
* COMMENTS:
*       There is no check done of current status of action's register
*       It's application responsibility to check the status of Aging Trigger by
*       using cpssExMxPmBrgFdbTrigActionStatusGet API, in order to be
*       sure that that there is currently no action done.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbTriggerModeSet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8                            devNum;
    CPSS_EXMXPM_FDB_TRIGGER_MODE_ENT mode;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_EXMXPM_FDB_TRIGGER_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbTriggerModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbTriggerModeGet    [DB]
*
* DESCRIPTION:
*       Gets FDB table Triggered\Automatic action mode.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - pointer to action mode:
*       mode    - action mode:
*                 CPSS_EXMXPM_FDB_TRIGGER_AUTO_E - Action is done Automatically.
*                 CPSS_EXMXPM_FDB_TRIGGER_TRIG_E - Action is done via a trigger
*                                                  from CPU.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_BAD_PARAM    - on wrong mode
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbTriggerModeGet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8                              devNum;
    CPSS_EXMXPM_FDB_TRIGGER_MODE_ENT   modePtr;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbTriggerModeGet(devNum, &modePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", modePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbAgingTriggerSet   [DB]
*
* DESCRIPTION:
*       Sets FDB action trigger (aging, deleting, uploading and transplanting)
*       Setting this field invoke the configured action.
*       This field also act as completion status of the current action
*       taking place.
*       The status of the action (finished or not) can be read by
*       cpssExMxPmBrgFdbAgingTriggerGet API.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong mode
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*       There is no check done of current status of action's register
*       It's application responsibility to check the status of Aging Trigger by
*       using cpssExMxPmBrgFdbTrigActionStatusGet API, in order to be
*       sure that that there is currently no action done.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbAgingTriggerSet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbAgingTriggerSet(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbAgingTriggerGet   [DB]
*
* DESCRIPTION:
*       Gets FDB aging trigger (aging, deleting, uploading and transplanting)
*       This field also act as completion status of the current action taking
*       place.
*       FDB action can be triggered by setting FDB action trigger using
*       cpssExMxPmBrgFdbAgingTriggerSet API.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       actFinishedPtr  - GT_TRUE  - triggered action completed
*                         GT_FALSE - triggered action is not completed yet
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong mode
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbAgingTriggerGet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  actFinishedPtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbAgingTriggerGet(devNum, &actFinishedPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", actFinishedPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbTriggeredActionStart  [DB]
*
* DESCRIPTION:
*       Enable actions, sets action type, action mode to CPSS_ACT_TRIG_E and
*       starts triggered action by setting Aging Trigger.
*       This API may be used to start one of triggered actions: Aging, Deleting,
*       Transplanting and FDB Upload.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum  - device number
*       mode    - action mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on bad param
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_STATE    - when previous FDB triggered action is not completed yet
*
* COMMENTS:
*       Deleting and Transplanting actions can be done only by CPU triggered
*       FDB action that activated by the function.
*       See below sequence of CPSS API calls for Deleting and
*       Transplanting actions.
*
*       Before making Delete or Transplant sequence the application need to
*       disable Actions to avoid wrong automatic aging.
*
*       It is Application responsibility to get and store all parameters
*       that are changed during those actions.
*       1. AA message to CPU status by cpssExMxPmBrgFdbAAandTAToCpuGet.
*       2. Action Active configuration by
*          cpssExMxPmBrgFdbActionActiveInterfaceGet,
*          cpssExMxPmBrgFdbActionActiveDevGet,
*          cpssExMxPmBrgFdbActionActiveVlanGet.
*       3. Action mode and trigger mode by cpssExMxPmBrgFdbActionModeGet
*          cpssExMxPmBrgFdbActionTriggerModeGet.
*
*       The AA and TA messages may be disabled before the FDB action
*       4. Disable AA and TA messages to CPU by cpssExMxPmBrgFdbAAandTAToCpuSet.
*
*       5. Set Active configuration by: cpssExMxPmBrgFdbActionActiveInterfaceSet,
*       cpssExMxPmBrgFdbActionActiveVlanSet and cpssExMxPmBrgFdbActionActiveDevSet.
*
*       6. Start triggered action by cpssExMxPmBrgFdbTrigActionStart
*
*       7. Wait that triggered action is completed by:
*           -  check the FDB action status - cpssExMxPmBrgFdbTrigActionStatusGet
*           -  Wait of event CPSS_PP_MAC_AGE_VIA_TRIGGER_ENDED_E.
*              This wait may be done in context of dedicated task to restore
*              Active configuration and AA messages configuration.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbTriggeredActionStart

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8                            devNum;
    CPSS_EXMXPM_FDB_ACTION_MODE_ENT  mode;
    GT_BOOL repeatAction;
    GT_U32  numRetries=10;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_EXMXPM_FDB_ACTION_MODE_ENT)inArgs[1];

    /* call cpss api function */
    wrapCpssExMxPmBrgAuqMutexLock();
    do{
        result = cpssExMxPmBrgFdbTriggeredActionStart(devNum, mode);
        if(mode == CPSS_EXMXPM_FDB_ACTION_DELETING_E ||
           mode ==CPSS_EXMXPM_FDB_ACTION_FDB_UPLOAD_E)
        {
            wrapCpssExMxPmBrgFdbResolveDeviceDeadLock(devNum,
                        WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_TRIGGERED_ACTION_E,
                        &repeatAction);
        }
        else
        {
            repeatAction = GT_FALSE;
        }
        numRetries --;
    }while(repeatAction == GT_TRUE && numRetries);
    wrapCpssExMxPmBrgAuqMutexUnLock();
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/******************************************************************************
* cpssExMxPmBrgFdbAgingTimeoutSet   [DB]
*
* DESCRIPTION:
*       Sets the timeout period in seconds for aging out dynamically learned
*       forwarding information. The standard recommends 300 sec.
*
*       NOTE : The device support aging time with granularity of 1 second.
*       (1,2..4095).
*
*       When set to ZERO this indicates that the action must be performed ASAP.
*       This value is used when performing TriggeredDelete and
*       Transplanting operations.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum  - device number
*       timeout - aging time in seconds.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       For core clock of 200 MHZ The value ranges from 1 second to 4095
*       seconds in steps of 1 second.
*       For core clock of 144MHZ the aging timeout ranges from 1 (1.38) to 5687
*       seconds in steps of 1.38 seconds.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbAgingTimeoutSet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U32   timeout;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    timeout = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbAgingTimeoutSet(devNum, timeout);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/******************************************************************************
* cpssExMxPmBrgFdbAgingTimeoutGet   [DB]
*
* DESCRIPTION:
*       Gets the timeout period in seconds for aging out dynamically learned
*       forwarding information. The standard recommends 300 sec.
*
*       NOTE : The device support aging time with granularity of 1 second
*       (1,2..4095) .
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       timeoutPtr - pointer to aging time in seconds.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       For core clock of 200 MHZ The value ranges from 1 second to 4095
*       seconds in steps of 1 second.
*       For core clock of 144MHZ the aging timeout ranges from 1 (1.38) to 4085
*       seconds in steps of 1.38 seconds.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbAgingTimeoutGet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U32   timeoutPtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbAgingTimeoutGet(devNum, &timeoutPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", timeoutPtr);
    return CMD_OK;
}
/***********************************************************************
* cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableSet  [DB]
*
* DESCRIPTION:
*       Enable/Disable aging out of all entries associated with trunk,
*       regardless of the device to wich they are associated.
*
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum                      - device number
*       ageOutAllDevOnTrunkEnable   - GT_TRUE : All entries associated with a
*                                     trunk, regardless of the device they
*                                     are associated with are aged out.
*                                     GT_FALSE: Only entries associated with a
*                                     trunk on the local device are aged out.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*       There is no check done of current status of action's register
*       It's application responsibility to check the status of Aging Trigger by
*       using cpssExMxPmBrgFdbTrigActionStatusGet API, in order to be
*       sure that that there is currently no action done.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableSet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  ageOutAllDevOnTrunkEnable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ageOutAllDevOnTrunkEnable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableSet(devNum, ageOutAllDevOnTrunkEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableGet  [DB]
*
* DESCRIPTION:
*       Get if aging out of all entries associated with trunk, regardless of
*       the device to wich they are associated is enabled/disabled.
*
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum                         - device number
*
* OUTPUTS:
*       ageOutAllDevOnTrunkEnablePtr - GT_TRUE : All entries associated with a
*                                      trunk, regardless of the device they
*                                      are associated with are aged out.
*                                      GT_FALSE: Only entries associated with a
*                                      trunk on the local device are aged out.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableGet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  ageOutAllDevOnTrunkEnablePtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableGet(devNum, &ageOutAllDevOnTrunkEnablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ageOutAllDevOnTrunkEnablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableSet    [DB]
*
* DESCRIPTION:
*       enable the ability to delete static fdb entries when the aging daemon in
*       PP encounter with entry registered on non-exist device in the
*       "device table" the PP removes it ---> this flag regard the static
*       entries
*
* APPLICABLE DEVICES:  All ExMxPx Devices
* INPUTS:
*       devNum       - device number
*       deleteStatic -  GT_TRUE - Action will delete static entries as well as
*                                 dynamic
*                       GT_FALSE - only dynamic entries will be deleted.
*
* OUTPUTS:
*       NONE
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum
*
* COMMENTS:
*       There is no check done of current status of action's register
*       It's application responsibility to check the status of Aging Trigger by
*       using cpssExMxPmBrgFdbTrigActionStatusGet API, in order to be
*       sure that that there is currently no action done.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableSet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  deleteStatic;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    deleteStatic = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableSet(devNum, deleteStatic);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableGet    [DB]
*
* DESCRIPTION:
*       Get whether enabled/disabled the ability to delete static fdb entries
*       when the aging daemon in PP encounter with entry registered on non-exist
*       device in the "device table".
*       entries
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       deleteStaticPtr -  GT_TRUE - Action deletes entries if there associated
*                                    device number don't exist (static entries
*                                                                  as well)
*                          GT_FALSE - Action don't delete entries if there
*                                     associated device number don't exist
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong mode
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableGet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  deleteStaticPtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableGet(devNum, &deleteStaticPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", deleteStaticPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbStaticDelEnableSet    [DB]
*
* DESCRIPTION:
*       This routine determines whether flush delete operates on static entries.
*       When the PP will do the Flush , it will/won't Flush
*       FDB static entries (entries that are not subject to aging).
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE delete is enabled on static entries, GT_FALSE otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbStaticDelEnableSet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbStaticDelEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbStaticDelEnableGet    [DB]
*
* DESCRIPTION:
*       Get whether flush delete operates on static entries.
*       When the PP do the Flush , it do/don't Flush
*       FDB static entries (entries that are not subject to aging).
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE delete is enabled on static entries,
*                    GT_FALSE otherwise.
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbStaticDelEnableGet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enablePtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbStaticDelEnableGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbStaticTransEnableSet      [DB]
*
* DESCRIPTION:
*       This routine determines whether the transplanting operate on static
*       entries.
*       When the PP will do the transplanting , it will/won't transplant
*       FDB static entries (entries that are not subject to aging).
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum - device number
*       enable - GT_TRUE transplanting is enabled on static entries,
*                GT_FALSE otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbStaticTransEnableSet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbStaticTransEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbStaticTransEnableGet  [DB]
*
* DESCRIPTION:
*       Get whether the transplanting enabled to operate on static entries.
*       when the PP do the transplanting , it do/don't transplant
*       FDB static entries (entries that are not subject to aging).
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       enablePtr - GT_TRUE transplanting is enabled on static entries,
*                   GT_FALSE otherwise.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbStaticTransEnableGet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enablePtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbStaticTransEnableGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbLsrEnableSet  [DB]
*
* DESCRIPTION:
*       Enable/Disable the FDB Learning, Scanning and Refresh operations.
*       Halting of these operation allows the CPU un-interrupted access to
*       the FDB.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE = Learning, Scanning and Refresh operations do
*                               their regular work
*                     GT_FALSE = Learning, Scanning and Refresh operations halt
*                              to allow the CPU un-interrupted access to the FDB
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbLsrEnableSet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    if(enable == GT_FALSE)
    {
        result = wrCpssExMxPmBrgFdbLsrDisable(devNum);
    }
    else
    {
        result = cpssExMxPmBrgFdbLsrEnableSet(devNum, GT_TRUE);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbLsrEnableGet  [DB]
*
* DESCRIPTION:
*       Get the status of the FDB Learning, Scanning and Refresh operations
*       (Enabled/Disabled). Halting of these operation allows the CPU
*       un-interrupted access to the FDB.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - GT_TRUE = Learning, Scanning and Refresh operations do
*                               their regular work
*                     GT_FALSE = Learning, Scanning and Refresh operations halt
*                              to allow the CPU un-interrupted access to the FDB
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbLsrEnableGet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enablePtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbLsrEnableGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbMcAddrDelEnableSet    [DB]
*
* DESCRIPTION:
*       Enable/Disable FDB MAC entries with multiple = 1, to be subject to
*       triggered actions (aging, flushing and transplanting).
*       Relevant only for non-static MC entries.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE = MC MAC entries are subject to FDB triggered
*                               actions .
*                     GT_FALSE = MC MAC entries are not subject FDB triggered
*                               actions.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbMcAddrDelEnableSet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    /* API removed !!! */
    result = GT_NOT_SUPPORTED;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbMcAddrDelEnableGet    [DB]
*
* DESCRIPTION:
*       Get Enable/Disable FDB MAC entries (entries with multiple = 1), to be
*       subject to delete operations (aging and triggered delete).
*       Relevant only for non-static MC entries.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - GT_TRUE = MC MAC entries are subject to FDB delete
*                               operations.
*                     GT_FALSE = MC MAC entries are not subject to FDB  delete
*                               operations.
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbMcAddrDelEnableGet

(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8    devNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    /* API removed */
    result = GT_NOT_SUPPORTED;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", 0);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbActionActiveFidSet
*
* DESCRIPTION:
*       Set action active fid and fid mask.
*       All actions will be taken on entries belonging to a certain FID
*       or a subset of FIDs.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum      - device number
*       fId         - Forwarding Id
*       fIdMask     - Forwarding Id mask filter
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on unsupported parameters
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Use mask 0xFFFF and FID to set mode on a single FID
*       Use mask 0 and FID 0 to set mode for all FIDs
*
*       There is no check done of current status of action's register
*       It's application responsibility to check the status of Aging Trigger by
*       using cpssExMxPmBrgFdbAgingTriggerGet API, in order to be
*       sure that that there is currently no action done.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbActionActiveFidSet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 fId;
    GT_U32 fIdMask;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    fId = (GT_U32)inArgs[1];
    fIdMask = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbActionActiveFidSet(devNum, fId, fIdMask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmBrgFdbActionActiveFidGet
*
* DESCRIPTION:
*       Get action active fid and fid mask.
*       All actions are taken on entries belonging to a certain FID
*       or a subset of FIDs.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       fIdPtr         - pointer to fId
*       fIdMaskPtr     - pointer to fId mask filter
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Use mask 0xFFFF and FID to set mode on a single FID
*       Use mask 0 and FID 0 to set mode for all FIDs
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbActionActiveFidGet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 fId;
    GT_U32 fIdMask;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbActionActiveFidGet(devNum, &fId, &fIdMask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", fId, fIdMask);

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgFdbLsrStoppedGet
*
* DESCRIPTION:
*       Get state of Learning, scanning, and refresh (L.S.R) stopped operation.
*       When set the CPU can perform un-interrupted direct access operations.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - GT_TRUE  - Learning, scanning, and refresh (LSR)
*                                operations are all stopped due to FDB Global
*                                Control0<Disable LSR> trigger.
*                     GT_FALSE - Learning, scanning, and refresh (LSR)
*                                operations are not stopped.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbLsrStoppedGet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbLsrStoppedGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbAgeRefreshEnableSet
*
* DESCRIPTION:
*       Enable/Disable the aging refresh operation
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE  - enable the aging refresh operation
*                     GT_FALSE - disable the aging refresh operation
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbAgeRefreshEnableSet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbAgeRefreshEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbPwIdTransplantEnableGet
*
* DESCRIPTION:
*       Get the transplanting state of PW-ID field of VPLS entries.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       enablePtr - GT_TRUE  - transplanting of PW-ID field of VPLS entries is
*                              enabled
*                   GT_FALSE - transplanting of PW-ID field of VPLS entries is
*                              disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbPwIdTransplantEnableGet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbPwIdTransplantEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbEccErrGet
*
* DESCRIPTION:
*       Get FDB ECC error information
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       fdbEccErrPtr    - pointer to the last FDB error data
*       fdbEccErrCntPtr - pointer to the number of data errors encountered by
*                         the FDB
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbEccErrGet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 fdbEccErrCnt;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbEccErrGet(devNum, &fdbEccErrCnt);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d%d",0,0,
                0,0,0,0,
                fdbEccErrCnt);

    return CMD_OK;
}



/*******************************************************************************
* cpssExMxPmBrgFdbAgeRefreshEnableGet
*
* DESCRIPTION:
*       Get state of aging refresh operation
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - GT_TRUE  - aging refresh operation is enabled
*                     GT_FALSE - aging refresh operation is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbAgeRefreshEnableGet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbAgeRefreshEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgFdbActionTransplantL2VpnSet
*
* DESCRIPTION:
*       Set the new L2 VPN information to replace the Old one, in case of
*       transplanting match
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum          - device number
*       newL2VpnPtr     - (pointer to) the new L2 VPN information to replace
*                         the Old one, in case of transplanting match.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbActionTransplantL2VpnSet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FDB_SCAN_L2VPN_STC newL2Vpn;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

        devNum = (GT_U8)inArgs[0];

        newL2Vpn.isDit=(GT_U8)inArgs[1];
        newL2Vpn.ditPtr=(GT_U32)inArgs[2];
        newL2Vpn.pwId=(GT_U32)inArgs[3];


    /* call cpss api function */
    result = cpssExMxPmBrgFdbActionTransplantL2VpnSet(devNum, &newL2Vpn);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgFdbActionTransplantL2VpnGet
*
* DESCRIPTION:
*       Get the new L2 VPN information to replace the Old one, in case of
*       transplanting match.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       newL2VpnPtr      - (pointer to) the new transplanting match to replace
*                          the Old one, in case of transplanting match.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbActionTransplantL2VpnGet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FDB_SCAN_L2VPN_STC newL2Vpn;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbActionTransplantL2VpnGet(devNum, &newL2Vpn);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d",newL2Vpn.isDit,newL2Vpn.ditPtr,newL2Vpn.pwId);


    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbActionActiveL2VpnSet
*
* DESCRIPTION:
*       Set the L2 VPN information control the FDB actions of : Triggered aging
*       with removal, Triggered aging without removal and Triggered
*       address deleting.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum          - device number
*       l2VpnPtr        - (pointer to) L2 VPN info
*       l2VpnMaskPtr    - (pointer to) L2 VPN mask
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbActionActiveL2VpnSet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FDB_SCAN_L2VPN_STC l2Vpn;
    CPSS_EXMXPM_FDB_SCAN_L2VPN_STC l2VpnMask;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    l2Vpn.isDit=(GT_U8)inArgs[1];
    l2Vpn.ditPtr=(GT_U32)inArgs[2];
    l2Vpn.pwId=(GT_U32)inArgs[3];

    l2VpnMask.isDit=(GT_U8)inArgs[4];
    l2VpnMask.ditPtr=(GT_U32)inArgs[5];
    l2VpnMask.pwId=(GT_U32)inArgs[6];



    /* call cpss api function */
    result = cpssExMxPmBrgFdbActionActiveL2VpnSet(devNum, &l2Vpn, &l2VpnMask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbActionActiveL2VpnGet
*
* DESCRIPTION:
*       Get the L2 VPN information that control the FDB actions of : Triggered
*       aging with removal, Triggered aging without removal and Triggered
*       address deleting.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       l2VpnPtr        - (pointer to) L2 VPN info
*       l2VpnMaskPtr    - (pointer to) L2 VPN mask
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbActionActiveL2VpnGet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FDB_SCAN_L2VPN_STC l2Vpn;
    CPSS_EXMXPM_FDB_SCAN_L2VPN_STC l2VpnMask;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbActionActiveL2VpnGet(devNum, &l2Vpn, &l2VpnMask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d",l2Vpn.isDit,l2Vpn.ditPtr,l2Vpn.pwId,
                l2VpnMask.isDit,l2VpnMask.ditPtr,l2VpnMask.pwId);

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgFdbActionTransplantOutlifSet
*
* DESCRIPTION:
*       Prepares the new outlif parameters for transplanting.
*       FID and FID mask for transplanting is set by
*       cpssExMxPmBrgFdbActionActiveFidSet. Execution of transplanting is done
*       by cpssExMxPmBrgFdbTrigActionStart.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum          - device number
*       newOutlifPtr    - (pointer to) new outlif parameters
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbActionTransplantOutlifSet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FDB_SCAN_OUTLIF_STC newOutlif;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    newOutlif.isMultiple=(GT_U8)inArgs[1];
    newOutlif.isTrunk=(GT_U8)inArgs[2];
    newOutlif.vidx=(GT_U16)inArgs[3];
    newOutlif.portTrunk=(GT_U16)inArgs[4];
    newOutlif.devNum=(GT_U8)inArgs[5];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbActionTransplantOutlifSet(devNum, &newOutlif);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbActionTransplantOutlifGet
*
* DESCRIPTION:
*       Get transplant new outlif parameters
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       newOutlifPtr - (pointer to) new outlif parameters.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbActionTransplantOutlifGet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FDB_SCAN_OUTLIF_STC newOutlif;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];


    /* call cpss api function */
    result = cpssExMxPmBrgFdbActionTransplantOutlifGet(devNum, &newOutlif);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d",newOutlif.isMultiple,newOutlif.isTrunk,newOutlif.vidx,newOutlif.portTrunk,newOutlif.devNum);

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgFdbActionActiveOutlifSet
*
* DESCRIPTION:
*       Set the Active Outlif that control the FDB actions of : Triggered
*       aging with removal, Triggered aging without removal and Triggered
*       address deleting.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum              - device number
*       actOutlifPtr        - (pointer to) action active outlif,
*       actOutlifMaskPtr    - (pointer to) action active outlif mask.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is no check done of current status of action's register
*       It's application responsibility to check the status of Aging Trigger by
*       using cpssExMxPmBrgFdbAgingTriggerGet API, in order to be
*       sure that that there is currently no action done.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbActionActiveOutlifSet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FDB_SCAN_OUTLIF_STC actOutlif;
    CPSS_EXMXPM_FDB_SCAN_OUTLIF_STC actOutlifMask;
    GT_U32  ii;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    ii=0;
    devNum = (GT_U8)inArgs[ii++];

    actOutlif.isMultiple= (GT_U8)inArgs[ii++];
    actOutlif.isTrunk= (GT_U8)inArgs[ii++];
    actOutlif.vidx = (GT_U16)inArgs[ii++];
    actOutlif.portTrunk= (GT_U16)inArgs[ii++];
    actOutlif.devNum= (GT_U8)inArgs[ii++];

    actOutlifMask.isTrunk= (GT_U8)inArgs[ii++];
    actOutlifMask.vidx = (GT_U16)inArgs[ii++];
    actOutlifMask.portTrunk= (GT_U16)inArgs[ii++];
    actOutlifMask.devNum= (GT_U8)inArgs[ii++];
    actOutlifMask.isMultiple= (GT_U8)inArgs[ii++];/* put actOutlifMask.isMultiple as last one due to GALTIS order */

    /* call cpss api function */
    result = cpssExMxPmBrgFdbActionActiveOutlifSet(devNum, &actOutlif, &actOutlifMask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbActionActiveOutlifGet
*
* DESCRIPTION:
*       Get the Active Outlif that control the FDB actions of : Triggered
*       aging with removal, Triggered aging without removal and Triggered
*       address deleting.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       actOutlifPtr     - (pointer to) action active interface,
*       actOutlifMaskPtr - (pointer to) action active interface mask.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbActionActiveOutlifGet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FDB_SCAN_OUTLIF_STC actOutlif;
    CPSS_EXMXPM_FDB_SCAN_OUTLIF_STC actOutlifMask;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbActionActiveOutlifGet(devNum, &actOutlif, &actOutlifMask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d%d%d%d%d",actOutlif.isMultiple,actOutlif.isTrunk,
                actOutlif.vidx,actOutlif.portTrunk,actOutlif.devNum,
                actOutlifMask.isTrunk,actOutlifMask.vidx,actOutlifMask.portTrunk,actOutlifMask.devNum,
                actOutlifMask.isMultiple);/*put actOutlifMask.isMultiple last due to order in the GALTIS */

    return CMD_OK;
}

/*******************************************************************************
* "cpssExMxPmBrgFdbActionActiveInterfaceSet"
*
* DESCRIPTION:
*       Support for this wrapper is added only to be backward compatible
*       with tests already written since the
*       "cpssExMxPmBrgFdbActionActiveInterfaceSet" does not
*       exist any more and the implemetation here below is based on
*       the new configuration function "cpssExMxPmBrgFdbActionActiveOutlifSet".
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum           - device number
*       actIsTrunk       - determines if the interface is port or trunk,
*                          the entry is trunk when entry masked by
*                          actIsTrunkMask bit = actIsTrunk.
*       actIsTrunkMask   - action active trunk mask,.
*       actTrunkPort     - action active interface (port/trunk)
*       actTrunkPortMask - action active port/trunk mask.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Please refer to "cpssExMxPmBrgFdbActionActiveOutlifSet" for more details.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbActionActiveInterfaceSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FDB_SCAN_OUTLIF_STC actOutlif;
    CPSS_EXMXPM_FDB_SCAN_OUTLIF_STC actOutlifMask;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    actOutlif.isMultiple= 0;
    actOutlif.isTrunk= (GT_U8)inArgs[1];
    actOutlif.vidx = 0;
    actOutlif.portTrunk= (GT_U16)inArgs[3];
    actOutlif.devNum= 0;

    actOutlifMask.isMultiple= 0;
    actOutlifMask.isTrunk= (GT_U8)inArgs[2];
    actOutlifMask.vidx = 0;
    actOutlifMask.portTrunk= (GT_U16)inArgs[4];
    actOutlifMask.devNum= 0;

    /* call cpss api function */
    result = cpssExMxPmBrgFdbActionActiveOutlifSet(devNum, &actOutlif, &actOutlifMask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* "cpssExMxPmBrgFdbActionActiveInterfaceGet"
*
* DESCRIPTION:
*       Support for this wrapper is added only to be backward compatible
*       with tests already written since the
*       "cpssExMxPmBrgFdbActionActiveInterfaceGet" does not
*       exist any more and the implemetation here below is based on
*       the new configuration function "cpssExMxPmBrgFdbActionActiveOutlifGet".
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       actIsTrunkPtr       - determines if the interface is port or trunk
*       actIsTrunkMaskPtr   - action active trunk mask.
*       actTrunkPortPtr     - action active interface (port/trunk)
*       actTrunkPortMaskPtr - action active port/trunk mask.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - unexpected results
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device

* COMMENTS:
*       GT_BAD_STATE is returned in case "isMultiple" indication is turned on,
*       which was not supported when this API originally existed. In this case
*       dummy values will be inserted into the different params.
*       Please refer to "cpssExMxPmBrgFdbActionActiveOutlifGet" for more details.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbActionActiveInterfaceGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FDB_SCAN_OUTLIF_STC actOutlif;
    CPSS_EXMXPM_FDB_SCAN_OUTLIF_STC actOutlifMask;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbActionActiveOutlifGet(devNum, &actOutlif, &actOutlifMask);

    if( actOutlif.isMultiple && actOutlifMask.isMultiple )
    {
        actOutlif.isTrunk = 1;
        actOutlifMask.isTrunk = 1;
        actOutlif.portTrunk = 0xFE;
        actOutlifMask.portTrunk = 0xFF;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", actOutlif.isTrunk,
                 actOutlifMask.isTrunk, actOutlif.portTrunk, actOutlifMask.portTrunk);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbNumOfEntriesInLookupSet
*
* DESCRIPTION:
*       Set the number of entries to be looked up in the FDB table lookup
*       (the hash chain length).
*       Used only by internal FDB.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum      - device number
*       lookupLen   - The maximal length of FDB table lookup, this must be
*                     value divided by 4 with no left over.
*                     In range 4..32. (4,8,12..32)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM    - If the given lookupLen is too large, or not divided
*                         by 4 with no left over.
*       GT_OUT_OF_RANGE          - lookupLen > 32 or lookupLen < 4
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Maximal Hash chain length in HW calculated as follows:
*       Maximal Hash chain length = (regVal + 1) * 2
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbNumOfEntriesInLookupSet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 lookupLen;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    lookupLen = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbNumOfEntriesInLookupSet(devNum, lookupLen);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbNumOfEntriesInLookupGet
*
* DESCRIPTION:
*       Get the the number of entries to be looked up in the FDB table lookup
*       (the hash chain length).
*       Used only by internal FDB.
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       lookupLenPtr - The maximal length of FDB table lookup, this must be
*                      value divided by 4 with no left over.
*                      In range 4..32. (4,8,12..32)
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM    - If the given lookupLen is too large, or not divided
*                         by 2 with no left over.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Maximal Hash chain length in HW calculated as follows:
*       Maximal Hash chain length = (regVal + 1) * 2
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbNumOfEntriesInLookupGet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 lookupLen;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbNumOfEntriesInLookupGet(devNum, &lookupLen);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", lookupLen);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbPwIdTransplantEnableSet
*
* DESCRIPTION:
*       Enable/Disable transplanting of PW-ID field of VPLS entries
*
* APPLICABLE DEVICES:  All ExMxPx Devices
*
* INPUTS:
*       devNum - device number
*       enable - GT_TRUE  - enable transplanting of PW-ID field of VPLS entries
*                GT_FALSE - disable transplanting of PW-ID field of VPLS entries
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbPwIdTransplantEnableSet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgFdbPwIdTransplantEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* internalFdbTcamMemoryGetNext
*
* DESCRIPTION:
*       Read next entry from FDB TCAM memory in the specified index.
*       This function can be used to debug, to check that the entry 'key' that
*       was returned from this function is the same as the 'key' returned from
*       the function cpssExMxPmBrgFdbTcamEntryRead(...)
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       index           - hw fdb TCAM entry index (0..255)
*       readFormat      - the format to read the info : {x,y} or {pattern,mask}
*
* OUTPUTS:
*       entryPtr    - (pointer to) TCAM memory entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum , readFormat
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS internalFdbTcamMemoryGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   rc;
    WR_REFRESH_TABLE_METHOD_ENT getMethod;
    GT_U8                         devNum;
    CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_ENT        readFormat;
    CPSS_EXMXPM_BRG_FDB_TCAM_MEMORY_INFO_UNT    entry;
    GT_U32  outFieldIndex = 0;/* index of current output field we handle */
    GT_U32  numOutFields = 13;/* number of fields for output */
    CPSS_EXMXPM_FDB_ENTRY_KEY_STC   *keyPtr;
    GT_U32  ii,jj;/*iterators*/

    if(inArgs[0] > 255)
    {
        /* we cant convert to GT_U8 */
        galtisOutput(outArgs, GT_BAD_PARAM, "");

        return CMD_OK;
    }

    devNum = (GT_U8)inArgs[0];
    readFormat = (CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_ENT)inArgs[1];
    getMethod = (WR_REFRESH_TABLE_METHOD_ENT)inArgs[3];

    while(1)
    {

        rc = cpssExMxPmBrgFdbTcamMemoryRead(devNum,currFdbTcamMemoryIndex,readFormat,&entry);
        if(rc == GT_OUT_OF_RANGE)
        {
            if(getMethod != ENTRY_E)
            {
                /* end of table */
                rc = GT_OK;
            }

            galtisOutput(outArgs, rc, "%d", -1);
            return CMD_OK;
        }
        else if(rc != GT_OK)
        {
            galtisOutput(outArgs, rc, "");
            return CMD_OK;
        }

        currFdbTcamMemoryIndex++;

        if(getMethod == ENTRY_E)
        {
            /* we want to return the entry to the caller anyway */
            break;
        }

        if(readFormat == CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_PATTERN_MASK_E)
        {
            /* check if entry valid */
            /* we return only 'valid' entries */
            if(entry.patternMaskFormat.maskValid == GT_TRUE &&
               entry.patternMaskFormat.patternValid == GT_FALSE)
            {
                /* ignore this entry and continue to next entry */
                continue;
            }
        }
        else
        {
            /* in the format of XY we want to see all the entries ,
               for very low level debugging */
        }

        /* we found entry that we want to return to the caller */
        break;
    }

    /* the index we have anyway */
    /*fieldOutputSetAppendMode(); --> don't start with this call before first
      call to fieldOutput */
    fieldOutput("%d",(currFdbTcamMemoryIndex-1));
    outFieldIndex++;

    if(readFormat == CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E)
    {
        /*
            Tabulator 1 - X,Y format
            fields:
            1. index        GT_U32
            2. X0 – word 0 of X         GT_U32
            3. X1 – word 1 of X         GT_U32
            4. X2 – word 2 of X         GT_U32
            5. X3 – word 3 of X         GT_U32
            6. Y0 – word 0 of Y         GT_U32
            7. Y1 – word 1 of Y         GT_U32
            8. Y2 – word 2 of Y         GT_U32
            9. Y3 – word 3 of Y         GT_U32
        */

        /* X0,1,2,3 */
        for(ii = 0; ii < CPSS_EXMXPM_BRG_FDB_TCAM_MEMORY_INFO_X_Y_WORDS_NUM_CNS ; ii++)
        {
            fieldOutputSetAppendMode();
            fieldOutput("%d",entry.xyFormat.xWords[ii]);
            outFieldIndex++;
        }

        /* Y0,1,2,3 */
        for(ii = 0; ii < CPSS_EXMXPM_BRG_FDB_TCAM_MEMORY_INFO_X_Y_WORDS_NUM_CNS ; ii++)
        {
            fieldOutputSetAppendMode();
            fieldOutput("%d",entry.xyFormat.yWords[ii]);
            outFieldIndex++;
        }

        /* fill empty fields */
        for(/*continue*/;outFieldIndex < numOutFields ;outFieldIndex++)
        {
            fieldOutputSetAppendMode();
            fieldOutput("%d",0);
        }
    }
    else
    {
        /*
            Tabulator 2 :Pattern/mask  format
            fields:
            1. index           GT_U32
            2. patternValid - the valid bit in the pattern
            3. patternKey. entryType - the type of the entry: MAC Addr, IPv4 MC, IPv6 MC or VPLS of pattern
                one of:
                a.     CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E = 0,
                b.     CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E,
                c.     CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E,
                d.     CPSS_EXMXPM_FDB_ENTRY_TYPE_VPLS_E
            4. patternKey.key.mac- mac addr of pattern
            5. patternKey.key.fid - fId of pattern
            6. patternKey.sip – source IP of pattern
            7. patternKey.dip – destination IP of pattern
            8. maskValid    - the valid bit in the mask
            9. maskKey. entryType - the type of the entry: MAC Addr, IPv4 MC, IPv6 MC or VPLS of mask
                one of:
                a.     CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E = 0,
                b.     CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E,
                c.     CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E,
                d.     CPSS_EXMXPM_FDB_ENTRY_TYPE_VPLS_E
            10. maskKey.key.mac- mac addr of mask
            11. maskKey.key.fid - fId of mask
            12. maskKey.sip – source IP of mask
            13. maskKey.dip – destination IP of mask
        */
        for(jj = 0 ; jj < 2 ;jj++)
        {
            if(jj == 0)
            {
                /* pattern */
                fieldOutputSetAppendMode();
                fieldOutput("%d",entry.patternMaskFormat.patternValid);
                outFieldIndex++;

                keyPtr = &entry.patternMaskFormat.patternKey;
            }
            else
            {
                /* mask */
                fieldOutputSetAppendMode();
                fieldOutput("%d",entry.patternMaskFormat.maskValid);
                outFieldIndex++;

                keyPtr = &entry.patternMaskFormat.maskKey;
            }

            fieldOutputSetAppendMode();
            fieldOutput("%d",keyPtr->entryType);
            outFieldIndex++;

            fieldOutputSetAppendMode();
            fieldOutput("%6b",keyPtr->key.macFid.macAddr.arEther);
            outFieldIndex++;

            fieldOutputSetAppendMode();
            if(keyPtr->entryType == CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E ||
               keyPtr->entryType == CPSS_EXMXPM_FDB_ENTRY_TYPE_VPLS_E)
            {
                fieldOutput("%d",keyPtr->key.macFid.fId);
            }
            else
            {
                fieldOutput("%d",keyPtr->key.ipMcast.fId);
            }
            outFieldIndex++;

            fieldOutputSetAppendMode();

            fieldOutput("%d",BYTES_TO_U32_MAC(keyPtr->key.ipMcast.sip));
            outFieldIndex++;

            fieldOutputSetAppendMode();
            fieldOutput("%d",BYTES_TO_U32_MAC(keyPtr->key.ipMcast.dip));
            outFieldIndex++;

        }

        if(numOutFields != outFieldIndex)
        {
            /* bad management on the output fields --> need to be fixed */
            galtisOutput(outArgs, GT_BAD_STATE,"");
            return CMD_OK;
        }
    }

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrCpssExMxPmBrgFdbTcamMemoryGetFirst
*
* DESCRIPTION:
*       Read first entry from FDB TCAM memory in the specified index.
*       This function can be used to debug, to check that the entry 'key' that
*       was returned from this function is the same as the 'key' returned from
*       the function cpssExMxPmBrgFdbTcamEntryRead(...)
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       index           - hw fdb TCAM entry index (0..255)
*       readFormat      - the format to read the info : {x,y} or {pattern,mask}
*
* OUTPUTS:
*       entryPtr    - (pointer to) TCAM memory entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum , readFormat
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbTcamMemoryGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    WR_REFRESH_TABLE_METHOD_ENT getMethod;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    getMethod = (WR_REFRESH_TABLE_METHOD_ENT)inArgs[3];

    if(getMethod == ENTRY_E)
    {
        /* set to specific index */
        currFdbTcamMemoryIndex = inArgs[2];
    }
    else  /* refresh full table , so start with first index */
    {
        currFdbTcamMemoryIndex = 0;
    }

    return internalFdbTcamMemoryGetNext(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* wrCpssExMxPmBrgFdbTcamMemoryGetNext
*
* DESCRIPTION:
*       Read next entry from FDB TCAM memory in the specified index.
*       This function can be used to debug, to check that the entry 'key' that
*       was returned from this function is the same as the 'key' returned from
*       the function cpssExMxPmBrgFdbTcamEntryRead(...)
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       index           - hw fdb TCAM entry index (0..255)
*       readFormat      - the format to read the info : {x,y} or {pattern,mask}
*
* OUTPUTS:
*       entryPtr    - (pointer to) TCAM memory entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum , readFormat
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbTcamMemoryGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    WR_REFRESH_TABLE_METHOD_ENT getMethod;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    getMethod = (WR_REFRESH_TABLE_METHOD_ENT)inArgs[3];

    if(getMethod == ENTRY_E)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    return internalFdbTcamMemoryGetNext(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* wrCpssExMxPmBrgFdbTcamEnableSet
*
* DESCRIPTION:
*       This function globally enable/disable the usage of the internal FDB TCAM.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE - FDB TCAM will be used to learn new addresses.
*                     GT_FALSE - FDB TCAM will not be used to learn new addresses.
*
* OUTPUTS:
*       None.
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbTcamEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   rc;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    rc = cpssExMxPmBrgFdbTcamEnableSet(
            (GT_U8)inArgs[0],
            (GT_BOOL)inArgs[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs,rc,"");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssExMxPmBrgFdbTcamEnableGet
*
* DESCRIPTION:
*       This function gets the globally enable/disable the usage of the internal
*       FDB TCAM.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE - FDB TCAM will be used to learn new
*                                            addresses.
*                                  GT_FALSE - FDB TCAM will not be used to learn
*                                             new addresses.
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbTcamEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   rc;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    rc = cpssExMxPmBrgFdbTcamEnableGet((GT_U8)inArgs[0],&enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs,rc,"%d",enable);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssExMxPmBrgFdbPortUnkUcMacSaCmdSet
*
* DESCRIPTION:
*     Set <Unknown MAC SA Command> for this port.
*     NOTE: Final <Unknown MAC SA Command> will be calculated by the standard
*           command resolution of these two commands:
*           This field command and The InLIF <Unknown MAC SA Command> field
*           command
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*       command  - the command .
*                  valid values are :
*                  CPSS_PACKET_CMD_FORWARD_E
*                  CPSS_PACKET_CMD_MIRROR_TO_CPU_E
*                  CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                  CPSS_PACKET_CMD_DROP_HARD_E
*                  CPSS_PACKET_CMD_DROP_SOFT_E
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbPortUnkUcMacSaCmdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   rc;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    rc = cpssExMxPmBrgFdbPortUnkUcMacSaCmdSet(
        (GT_U8)inArgs[0],
        (GT_U8)inArgs[1],
        (CPSS_PACKET_CMD_ENT)inArgs[2]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs,rc,"");

    return CMD_OK;
}


/*******************************************************************************
* wrCpssExMxPmBrgFdbPortUnkUcMacSaCmdGet
*
* DESCRIPTION:
*     Get <Unknown MAC SA Command> for this port.
*     NOTE: Final <Unknown MAC SA Command> will be calculated by the standard
*           command resolution of these two commands:
*           This field command and The InLIF <Unknown MAC SA Command> field
*           command
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
* OUTPUTS:
*       commandPtr  - (pointer to)the command .
*                  valid values are :
*                  CPSS_PACKET_CMD_FORWARD_E
*                  CPSS_PACKET_CMD_MIRROR_TO_CPU_E
*                  CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                  CPSS_PACKET_CMD_DROP_HARD_E
*                  CPSS_PACKET_CMD_DROP_SOFT_E
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - the value read from the HW not match any
*                                  valid command option
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbPortUnkUcMacSaCmdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   rc;
    CPSS_PACKET_CMD_ENT command;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    rc = cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet(
            (GT_U8)inArgs[0],
            (GT_U8)inArgs[1],
            &command);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs,rc,"%d",command);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssExMxPmBrgFdbTcamStatusGet
*
* DESCRIPTION:
*       Application may use this API to check if the device has finished
*       processing previous operation and is ready for additional operation,
*       which may include:
*           1. 'write' FDB TCAM entry (cpssExMxPmBrgFdbTcamEntryWrite).
*           2. 'invalidate' FDB TCAM entry (cpssExMxPmBrgFdbTcamEntryInvalidate)
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       completedPtr - (pointer to) was the previous FDB TCAM operation completed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbTcamStatusGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   rc;
    GT_BOOL     completed;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    rc = cpssExMxPmBrgFdbTcamStatusGet((GT_U8)inArgs[0],&completed);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs,rc,"%d",completed);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssExMxPmBrgFdbEntrySetMode
*
* DESCRIPTION:
*       set the 'mode' for 'entry set' operation.
*       modes are :
*           1. set entry 'by message' --> pp calculate FDB hash
*           2. write entry by index --> wrapper will calculate FDB hash (+LUT for external memory)
*
* INPUTS:
*       none
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbEntrySetMode
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    fdbEntrySetMode = inArgs[0];

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{

    {"cpssExMxPmBrgFdbInit",
        &wrCpssExMxPmBrgFdbInit,
        1, 0},

    {"cpssExMxPmBrgFdbHashModeSet",
        &wrCpssExMxPmBrgFdbHashModeSet,
        2, 0},

    {"cpssExMxPmBrgFdbHashModeGet",
        &wrCpssExMxPmBrgFdbHashModeGet,
        1, 0},

    {"cpssExMxPmBrgFdbMacVlanLookupModeSet",
        &wrCpssExMxPmBrgFdbMacVlanLookupModeSet,
        2, 0},

    {"cpssExMxPmBrgFdbMacVlanLookupModeGet",
        &wrCpssExMxPmBrgFdbMacVlanLookupModeGet,
        1, 0},

    {"cpssExMxPmBrgFdbExtLookupOffsetSizeSet",
        &wrCpssExMxPmBrgFdbExtLookupOffsetSizeSet,
        2, 0},

    {"cpssExMxPmBrgFdbExtLookupOffsetSizeGet",
        &wrCpssExMxPmBrgFdbExtLookupOffsetSizeGet,
        1, 0},

    {"cpssExMxPm2BrgFdbQaSet",
        &wrCpssExMxPmBrgFdbQaSend,
        1, 5},


        /***   cpssExMxPmBrgFdbMac table commands   ***/

    /*  puma1 table */
     {"cpssExMxPmBrgFdbMacSet",
        &wrCpssExMxPmBrgFdbEntrySet,
        2, 27},

    {"cpssExMxPmBrgFdbMacGetFirst",
        &wrCpssExMxPmBrgFdbEntryReadFirst,
        2, 0},

    {"cpssExMxPmBrgFdbMacGetNext",
        &wrCpssExMxPmBrgFdbEntryReadNext,
        2, 0},

    {"cpssExMxPmBrgFdbMacDelete",
        &wrCpssExMxPmBrgFdbEntryDelete,
        2, 27},

        /*puma 2 table*/

    {"cpssExMxPm2BrgFdbMacSet",
        &wrCpssExMxPm2BrgFdbEntrySet,
        2, 31},

    {"cpssExMxPm2BrgFdbMacGetFirst",
        &wrCpssExMxPm2BrgFdbEntryReadFirst,
        2, 0},

    {"cpssExMxPm2BrgFdbMacGetNext",
        &wrCpssExMxPm2BrgFdbEntryReadNext,
        2, 0},

    {"cpssExMxPm2BrgFdbMacDelete",
        &wrCpssExMxPm2BrgFdbEntryDelete,
        2, 27},

    /*puma 2 extanded table*/

    {"cpssExMxPm2BrgFdbMacExtSet",
        &wrCpssExMxPm2BrgFdbEntryExtSet,
        2, 30},

    {"cpssExMxPm2BrgFdbMacExtGetFirst",
        &wrCpssExMxPm2BrgFdbEntryExtReadFirst,
        2, 0},

    {"cpssExMxPm2BrgFdbMacExtGetNext",
        &wrCpssExMxPm2BrgFdbEntryExtReadNext,
        2, 0},

    {"cpssExMxPm2BrgFdbMacExtDelete",
        &wrCpssExMxPm2BrgFdbEntryDelete,
        2, 27},

        /*** end cpssExMxPmBrgFdbMac table commands ***/


    {"cpssExMxPmBrgFdbEntryInvalidate",
        &wrCpssExMxPmBrgFdbEntryInvalidate,
        2, 0},

    {"cpssExMxPmBrgFdbLutEntrySet",
        &wrCpssExMxPmBrgFdbLutEntrySet,   /* STC with array of 4 sub-entries*/
        1, 13},

    {"cpssExMxPmBrgFdbLutEntryGetFirst",
         &wrCpssExMxPmBrgFdbLutEntryGetFirst,
        1, 0},
    {"cpssExMxPmBrgFdbLutEntryGetNext",
         &wrCpssExMxPmBrgFdbLutEntryGetNext,
        1, 0},

    {"cpssExMxPmBrgFdbAaAndTaToCpuSet",
        &wrCpssExMxPmBrgFdbAaAndTaToCpuSet,
        2, 0},

    {"cpssExMxPmBrgFdbAaAndTaToCpuGet",
        &wrCpssExMxPmBrgFdbAaAndTaToCpuGet,
        1, 0},

    {"cpssExMxPmBrgFdbNaToCpuSet",
        &wrCpssExMxPmBrgFdbNaToCpuSet,
        2, 0},

    {"cpssExMxPmBrgFdbNaToCpuGet",
        &wrCpssExMxPmBrgFdbNaToCpuGet,
        1, 0},

    {"cpssExMxPmBrgFdbQaUnicastToCpuSet",
        &wrCpssExMxPmBrgFdbQaUnicastToCpuSet,
        2, 0},

    {"cpssExMxPmBrgFdbQaUnicastToCpuGet",
        &wrCpssExMxPmBrgFdbQaUnicastToCpuGet,
        1, 0},

    {"cpssExMxPmBrgFdbQaMulticastToCpuSet",
        &wrCpssExMxPmBrgFdbQaMulticastToCpuSet,
        2, 0},

    {"cpssExMxPmBrgFdbQaMulticastToCpuGet",
        &wrCpssExMxPmBrgFdbQaMulticastToCpuGet,
        1, 0},

    {"cpssExMxPmBrgFdbSpAaMsgToCpuSet",
        &wrCpssExMxPmBrgFdbSpAaMsgToCpuSet,
        2, 0},

    {"cpssExMxPmBrgFdbSpAaMsgToCpuGet",
        &wrCpssExMxPmBrgFdbSpAaMsgToCpuGet,
        1, 0},

    {"cpssExMxPmBrgFdbSpEntriesDelEnableSet",
        &wrCpssExMxPmBrgFdbSpEntriesDelEnableSet,
        2, 0},

    {"cpssExMxPmBrgFdbSpEntriesDelEnableGet",
        &wrCpssExMxPmBrgFdbSpEntriesDelEnableGet,
        1, 0},

    {"cpssExMxPmBrgFdbNaMsgOnChainTooLongSet",
        &wrCpssExMxPmBrgFdbNaMsgOnChainTooLongSet,
        2, 0},

    {"cpssExMxPmBrgFdbNaMsgOnChainTooLongGet",
        &wrCpssExMxPmBrgFdbNaMsgOnChainTooLongGet,
        1, 0},

    {"cpssExMxPm2BrgFdbFromCpuAuMsgStatusGet",
        &wrCpssExMxPm2BrgFdbFromCpuAuMsgStatusGet,
        2, 0},

    {"cpssExMxPmBrgFdbFromCpuAuMsgStatusGet",
        &wrCpssExMxPmBrgFdbFromCpuAuMsgStatusGet,
        1, 0},

    /*  cpssExMxPmBrgFdbAuMsgBlockGet - same as DxCh  */

    {"cpssExMxPmBrgFdbAuMsgBlockGetFirst",
        &wrCpssExMxPmBrgFdbAuMsgBlockGetFirst,
        2, 0},

    {"cpssExMxPmBrgFdbAuMsgBlockGetNext",
        &wrCpssExMxPmBrgFdbAuMsgBlockGetNext,
        2, 0},

    {"cpssExMxPmBrgFdbFuMsgBlockGetFirst",
        &wrCpssExMxPmBrgFdbFuMsgBlockGetFirst,
        2, 0},

    {"cpssExMxPmBrgFdbFuMsgBlockGetNext",
        &wrCpssExMxPmBrgFdbFuMsgBlockGetNext,
        2, 0},

    {"cpssExMxPmBrgFdbAuqFuqMessagesNumberGet",
        &wrCpssExMxPmBrgFdbAuqFuqMessagesNumberGet,
        2, 0},

    /* ------------- */

    {"cpssExMxPmBrgFdbDropAuEnableSet",
        &wrCpssExMxPmBrgFdbDropAuEnableSet,
        2, 0},

    {"cpssExMxPmBrgFdbDropAuEnableGet",
        &wrCpssExMxPmBrgFdbDropAuEnableGet,
        1, 0},

    {"cpssExMxPmBrgFdbAuMsgRateLimitSet",
        &wrCpssExMxPmBrgFdbAuMsgRateLimitSet,
        3, 0},

    {"cpssExMxPmBrgFdbAuMsgRateLimitGet",
        &wrCpssExMxPmBrgFdbAuMsgRateLimitGet,
        1, 0},

    {"cpssExMxPmBrgFdbSecureAutoLearnSet",
        &wrCpssExMxPmBrgFdbSecureAutoLearnSet,
        2, 0},

    {"cpssExMxPmBrgFdbSecureAutoLearnGet",
        &wrCpssExMxPmBrgFdbSecureAutoLearnGet,
        1, 0},

    {"cpssExMxPmBrgFdbSecureAgingSet",
        &wrCpssExMxPmBrgFdbSecureAgingSet,
        2, 0},

    {"cpssExMxPmBrgFdbSecureAgingGet",
        &wrCpssExMxPmBrgFdbSecureAgingGet,
        1, 0},

    {"cpssExMxPmBrgFdbDeviceTableSet",
        &wrCpssExMxPmBrgFdbDeviceTableSet,
        5, 0},

    {"cpssExMxPmBrgFdbDeviceTableGet",
        &wrCpssExMxPmBrgFdbDeviceTableGet,
        1, 0},

    {"cpssExMxPmBrgFdbActionsEnableSet",
        &wrCpssExMxPmBrgFdbActionsEnableSet,
        2, 0},

    {"cpssExMxPmBrgFdbActionsEnableGet",
        &wrCpssExMxPmBrgFdbActionsEnableGet,
        1, 0},

    {"cpssExMxPmBrgFdbActionModeSet",
        &wrCpssExMxPmBrgFdbActionModeSet,
        2, 0},

    {"cpssExMxPmBrgFdbActionModeGet",
        &wrCpssExMxPmBrgFdbActionModeGet,
        1, 0},

    {"cpssExMxPmBrgFdbTriggerModeSet",
        &wrCpssExMxPmBrgFdbTriggerModeSet,
        2, 0},

    {"cpssExMxPmBrgFdbTriggerModeGet",
        &wrCpssExMxPmBrgFdbTriggerModeGet,
        1, 0},

    {"cpssExMxPmBrgFdbAgingTriggerSet",
        &wrCpssExMxPmBrgFdbAgingTriggerSet,
        1, 0},

    {"cpssExMxPmBrgFdbAgingTriggerGet",
        &wrCpssExMxPmBrgFdbAgingTriggerGet,
        1, 0},

    {"cpssExMxPmBrgFdbTriggeredActionStart",
        &wrCpssExMxPmBrgFdbTriggeredActionStart,
        2, 0},

    {"cpssExMxPmBrgFdbAgingTimeoutSet",
        &wrCpssExMxPmBrgFdbAgingTimeoutSet,
        2, 0},

    {"cpssExMxPmBrgFdbAgingTimeoutGet",
        &wrCpssExMxPmBrgFdbAgingTimeoutGet,
        1, 0},


    /* --- cpssExMxPmBrgFdbActionTransplantData Table   */


    {"cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableSet",
        &wrCpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableSet,
        2, 0},

    {"cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableGet",
        &wrCpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableGet,
        1, 0},

    {"cpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableSet",
        &wrCpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableSet,
        2, 0},

    {"cpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableGet",
        &wrCpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableGet,
        1, 0},

    {"cpssExMxPmBrgFdbStaticDelEnableSet",
        &wrCpssExMxPmBrgFdbStaticDelEnableSet,
        2, 0},

    {"cpssExMxPmBrgFdbStaticDelEnableGet",
        &wrCpssExMxPmBrgFdbStaticDelEnableGet,
        1, 0},

    {"cpssExMxPmBrgFdbStaticTransEnableSet",
        &wrCpssExMxPmBrgFdbStaticTransEnableSet,
        2, 0},

    {"cpssExMxPmBrgFdbStaticTransEnableGet",
        &wrCpssExMxPmBrgFdbStaticTransEnableGet,
        1, 0},

    {"cpssExMxPmBrgFdbLsrEnableSet",
        &wrCpssExMxPmBrgFdbLsrEnableSet,
        2, 0},

    {"cpssExMxPmBrgFdbLsrEnableGet",
        &wrCpssExMxPmBrgFdbLsrEnableGet,
        1, 0},

    {"cpssExMxPmBrgFdbMcAddrDelEnableSet",
        &wrCpssExMxPmBrgFdbMcAddrDelEnableSet,
        2, 0},

    {"cpssExMxPmBrgFdbMcAddrDelEnableGet",
        &wrCpssExMxPmBrgFdbMcAddrDelEnableGet,
        1, 0},

    {"cpssExMxPmBrgFdbActionActiveVlanSet",
        &wrCpssExMxPmBrgFdbActionActiveFidSet,
        3, 0},

        {"cpssExMxPmBrgFdbActionActiveVlanGet",
        &wrCpssExMxPmBrgFdbActionActiveFidGet,
        1, 0},

        {"cpssExMxPm2BrgFdbLsrStoppedGet",
        &wrCpssExMxPmBrgFdbLsrStoppedGet,
        1, 0},

        {"cpssExMxPm2BrgFdbAgeRefreshEnableSet",
        &wrCpssExMxPmBrgFdbAgeRefreshEnableSet,
        2, 0},

        {"cpssExMxPm2BrgFdbPwIdTransplantEnableGet",
        &wrCpssExMxPmBrgFdbPwIdTransplantEnableGet,
        2, 0},

        {"cpssExMxPm2BrgFdbEccErrGet",
        &wrCpssExMxPmBrgFdbEccErrGet,
        1, 0},


        {"cpssExMxPm2BrgFdbAgeRefreshEnableGet",
        &wrCpssExMxPmBrgFdbAgeRefreshEnableGet,
        1, 0},

        {"cpssExMxPm2BrgFdbActionTransplantL2VpnSet",
        &wrCpssExMxPmBrgFdbActionTransplantL2VpnSet,
        4, 0},
        {"cpssExMxPm2BrgFdbActionTransplantL2VpnGet",
        &wrCpssExMxPmBrgFdbActionTransplantL2VpnGet,
        1, 0},

        {"cpssExMxPm2BrgFdbActionActiveL2VpnSet",
        &wrCpssExMxPmBrgFdbActionActiveL2VpnSet,
        7, 0},

        {"cpssExMxPm2BrgFdbActionActiveL2VpnGet",
        &wrCpssExMxPmBrgFdbActionActiveL2VpnGet,
        1, 0},

        {"cpssExMxPm2BrgFdbActionTransplantOutlifSet",
        &wrCpssExMxPmBrgFdbActionTransplantOutlifSet,
        6, 0},

        {"cpssExMxPm2BrgFdbActionTransplantOutlifGet",
        &wrCpssExMxPmBrgFdbActionTransplantOutlifGet,
        1, 0},

        {"cpssExMxPmBrgFdbActionActiveInterfaceSet",
        &wrCpssExMxPmBrgFdbActionActiveInterfaceSet,
        5, 0},

        {"cpssExMxPmBrgFdbActionActiveInterfaceGet",
        &wrCpssExMxPmBrgFdbActionActiveInterfaceGet,
        1, 0},

        {"cpssExMxPm2BrgFdbActionActiveOutlifSet",
        &wrCpssExMxPmBrgFdbActionActiveOutlifSet,
        11, 0},

        {"cpssExMxPm2BrgFdbActionActiveOutlifGet",
        &wrCpssExMxPmBrgFdbActionActiveOutlifGet,
        1, 0},
        {"cpssExMxPmBrgFdbMaxLookupLenSet",
        &wrCpssExMxPmBrgFdbNumOfEntriesInLookupSet,
        2, 0},

        {"cpssExMxPmBrgFdbMaxLookupLenGet",
        &wrCpssExMxPmBrgFdbNumOfEntriesInLookupGet,
        1, 0},

        {"cpssExMxPm2BrgFdbPwIdTransplantEnableSet",
        &wrCpssExMxPmBrgFdbPwIdTransplantEnableSet,
        2, 0},


        /* table cpssExMxPmBrgFdbTcamMemory -- supports only 'refresh' */
        {"cpssExMxPmBrgFdbTcamMemoryGetFirst",
          wrCpssExMxPmBrgFdbTcamMemoryGetFirst,
          4,0},

        {"cpssExMxPmBrgFdbTcamMemoryGetNext",
          wrCpssExMxPmBrgFdbTcamMemoryGetNext,
          4,0},
        /* end of table cpssExMxPmBrgFdbTcamMemory */

        {"cpssExMxPmBrgFdbTcamEnableSet",
          wrCpssExMxPmBrgFdbTcamEnableSet,
          2,0},

        {"cpssExMxPmBrgFdbTcamEnableGet",
          wrCpssExMxPmBrgFdbTcamEnableGet,
          1,0},

        {"cpssExMxPmBrgFdbPortUnkUcMacSaCmdSet",
          wrCpssExMxPmBrgFdbPortUnkUcMacSaCmdSet,
          3,0},

        {"cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet",
          wrCpssExMxPmBrgFdbPortUnkUcMacSaCmdGet,
          2,0},

        {"cpssExMxPmBrgFdbTcamStatusGet",
          wrCpssExMxPmBrgFdbTcamStatusGet,
          1,0},

        {"cpssExMxPmBrgFdbEntrySetMode",
          wrCpssExMxPmBrgFdbEntrySetMode,
          1,0}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChBridgePrvEdgeVlan
*
* DESCRIPTION:
*     Library database initialization function.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
GT_STATUS cmdLibInitCpssExMxPmBrgFdb
(
 GT_VOID
 )
{
    return cmdInitLibrary(dbCommands, numCommands);
}

/* debug function for read valid entries from LUT using : cpssExMxPmBrgFdbLutEntryRead */
/* function print all valid entries (if there is at least on sub section valid in entry)*/
GT_STATUS wrCpssExMxPmBrgFdbLutScan
(
    IN  GT_U8                           devNum
)
{
    GT_STATUS rc=GT_OK;
    GT_U32                      currIndex=0;
    CPSS_EXMXPM_FDB_LUT_ENTRY_STC      lutEntry;
    GT_U32  ii;

    cmdOsPrintf("f1 = lutSecHashValue , f2 = fdbEntryOffset, f3 = isValid \n");
    cmdOsPrintf("isValid has 2 values : V --> valid , NV --> not valid \n");
    cmdOsPrintf("\n");

    cmdOsPrintf("index | f1 , f2 , f3 | f1 , f2 , f3 | f1 , f2 , f3  | f1 , f2 , f3 \n");
    cmdOsPrintf("======|==============|==============|===============|============= \n");

    while(rc == GT_OK)
    {
        rc = cpssExMxPmBrgFdbLutEntryRead(devNum,currIndex,&lutEntry);
        if(rc == GT_OUT_OF_RANGE)
        {
            rc = GT_OK;
            break;
        }
        else if(rc != GT_OK)
        {
            cmdOsPrintf("wrCpssExMxPmBrgFdbLutScan : ERROR [%d] in index [%d] ", rc,currIndex);
            break;
        }

        if(lutEntry.subEntry[0].isValid == GT_TRUE ||
           lutEntry.subEntry[1].isValid == GT_TRUE ||
           lutEntry.subEntry[2].isValid == GT_TRUE ||
           lutEntry.subEntry[3].isValid == GT_TRUE)
        {
            /* print index */
            cmdOsPrintf("0x%8.8x " , currIndex);

            /* print the LUT entry , 4 sub section */
            for(ii = 0 ; ii < CPSS_EXMXPM_FDB_LUT_SUB_ENTRIES_NUM_CNS ; ii++)
            {
                cmdOsPrintf("| 0x%3.3x,0x%2.2x,%s \n",
                    lutEntry.subEntry[ii].lutSecHashValue,
                    lutEntry.subEntry[ii].fdbEntryOffset,
                    lutEntry.subEntry[ii].isValid == GT_TRUE ? " V" : "NV"
                    );
            }
        }

        currIndex ++;
    }

    cmdOsPrintf("wrCpssExMxPmBrgFdbLutScan : done \n");

    return rc;
}


GT_STATUS wrCpssExMxPmBrgFdbLutEntryWrite_debug
(
    IN GT_U8                            devNum,
    /* index,subIndex */
    IN GT_U32                           index,
    IN GT_U32                           subIndex,/*0..3 , 0xffffffff --> for all subEntries */
    /*data*/
    IN GT_U32                           lutSecHashValue,
    IN GT_U32                           fdbEntryOffset,
    IN GT_BOOL                          isValid
)
{
    GT_STATUS   rc;
    GT_U32      ii;
    CPSS_EXMXPM_FDB_LUT_ENTRY_STC      lutEntry;

    switch(subIndex)
    {
        case 0:
        case 1:
        case 2:
        case 3:
            /*read the entry from HW , so we can update*/
            rc = cpssExMxPmBrgFdbLutEntryRead(devNum,index,&lutEntry);
            if(rc != GT_OK)
            {
                return rc;
            }

            lutEntry.subEntry[subIndex].lutSecHashValue  = lutSecHashValue;
            lutEntry.subEntry[subIndex].fdbEntryOffset   = fdbEntryOffset;
            lutEntry.subEntry[subIndex].isValid          = isValid;
            break;
        case 0xFFFFFFFF:
            for(ii = 0 ; ii < CPSS_EXMXPM_FDB_LUT_SUB_ENTRIES_NUM_CNS ; ii++)
            {
                lutEntry.subEntry[ii].lutSecHashValue  = lutSecHashValue;
                lutEntry.subEntry[ii].fdbEntryOffset   = fdbEntryOffset;
                lutEntry.subEntry[ii].isValid          = isValid;
            }
            break;
        default:
            cmdOsPrintf("wrCpssExMxPmBrgFdbLutEntryWrite_debug : subIndex must be:0..3 , or 0xffffffff --> for all subEntries \n");
            return GT_BAD_PARAM;
    }

    return cpssExMxPmBrgFdbLutEntryWrite(devNum,index,&lutEntry);
}


GT_STATUS wrCpssExMxPmBrgFdbTcamMemoryScan
(
    IN GT_U8    devNum,
    IN GT_BOOL  xyOrPatternMask /*GT_TRUE-{x,y} , GT_FALSE {pattern,mask}*/
)
{
    GT_STATUS       rc=GT_OK;
    GT_U32          currIndex=0;
    CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_ENT readFormat ;
    CPSS_EXMXPM_BRG_FDB_TCAM_MEMORY_INFO_UNT    entry;
    CPSS_EXMXPM_FDB_ENTRY_KEY_STC   *keyPtr;
    GT_U32  ii,jj;

    readFormat = (xyOrPatternMask == GT_FALSE) ?
        CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_PATTERN_MASK_E :
        CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E;

    cmdOsPrintf("wrCpssExMxPmBrgFdbTcamMemoryScan : start \n");

    if(readFormat == CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E)
    {
        cmdOsPrintf(" type   : word0 ...                        word8 \n");
    }
    else
    {
        cmdOsPrintf(" type   : valid   mac             fId \n");
        cmdOsPrintf(" type   : valid   sip      dip    fId \n");
        cmdOsPrintf("===================================== \n");
    }


    while(rc ==GT_OK)
    {
        rc = cpssExMxPmBrgFdbTcamMemoryRead(devNum,currIndex,readFormat,&entry);
        if(rc == GT_OUT_OF_RANGE)
        {
            rc = GT_OK;
            break;
        }
        else if(rc != GT_OK)
        {
            cmdOsPrintf("wrCpssExMxPmBrgFdbTcamMemoryScan : ERROR [%d] in index [%d] ", rc,currIndex);
            break;
        }

        currIndex++;

        if(readFormat == CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E)
        {
            cmdOsPrintf("%3.3d ",currIndex);
            cmdOsPrintf("x:");
            /* we print all the entries */
            for(ii = 0; ii < CPSS_EXMXPM_BRG_FDB_TCAM_MEMORY_INFO_X_Y_WORDS_NUM_CNS ; ii++)
            {
                cmdOsPrintf(" %8.8x ",entry.xyFormat.xWords[ii]);
            }
            cmdOsPrintf("\n");
            cmdOsPrintf("%3.3d ",currIndex);
            cmdOsPrintf("y:");
            for(ii = 0; ii < CPSS_EXMXPM_BRG_FDB_TCAM_MEMORY_INFO_X_Y_WORDS_NUM_CNS ; ii++)
            {
                cmdOsPrintf(" %8.8x ",entry.xyFormat.yWords[ii]);
            }
            cmdOsPrintf("\n");
        }
        else
        {
            /* we print only 'valid' entries */
            if(entry.patternMaskFormat.maskValid == GT_TRUE && entry.patternMaskFormat.patternValid == GT_FALSE)
            {
                continue;
            }

            cmdOsPrintf("%3.3d ",currIndex);
            for(jj = 0 ; jj < 2 ;jj++)
            {
                if(jj == 0)
                {
                    cmdOsPrintf(" pattern: ");
                    cmdOsPrintf("%s",entry.patternMaskFormat.patternValid == GT_TRUE ? "V " : "NV");
                    keyPtr = &entry.patternMaskFormat.patternKey;
                }
                else
                {
                    cmdOsPrintf(" mask   : ");
                    cmdOsPrintf("%s",entry.patternMaskFormat.maskValid == GT_TRUE ? "V " : "NV");
                    keyPtr = &entry.patternMaskFormat.maskKey;
                }

                cmdOsPrintf("%d",keyPtr->entryType);
                if(keyPtr->entryType == CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E ||
                   keyPtr->entryType == CPSS_EXMXPM_FDB_ENTRY_TYPE_VPLS_E)
                {
                    cmdOsPrintf("[");
                    for(ii = 0; ii < 6 ; ii++)
                    {
                        cmdOsPrintf("%2.2x",keyPtr->key.macFid.macAddr.arEther[ii]);
                    }
                    cmdOsPrintf("]     ");

                }
                else
                {
                    cmdOsPrintf("[");
                    for(ii = 0; ii < 4 ; ii++)
                    {
                        cmdOsPrintf("%2.2x",keyPtr->key.ipMcast.sip[ii]);
                    }
                    cmdOsPrintf("] ");
                    cmdOsPrintf("[");
                    for(ii = 0; ii < 4 ; ii++)
                    {
                        cmdOsPrintf("%2.2x",keyPtr->key.ipMcast.dip[ii]);
                    }
                    cmdOsPrintf("] ");

                    cmdOsPrintf("%4.4d",keyPtr->key.ipMcast.fId);
                }
            }

            cmdOsPrintf("\n");
        }
    }

    cmdOsPrintf("wrCpssExMxPmBrgFdbTcamMemoryScan : done \n");

    return rc;
}

/*******************************************************************************
* fdbEntrySet
*
* DESCRIPTION:
*       internal wrapper for the cpssExMxPmBrgFdbEntrySet / cpssExMxPmBrgFdbEntryWrite
*       that decided according to fdbEntrySetMode
*
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       fdbEntryPtr     - pointer to FDB table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS fdbEntrySet
(
    IN GT_U8                            devNum,
    IN CPSS_EXMXPM_FDB_ENTRY_STC       *fdbEntryPtr
)
{
    GT_STATUS                       result;
    GT_BOOL repeatAction;
    GT_U32  numRetries=10;
    GT_U32  lutHashIndex,lutSecHashValue,fdbHashIndex;
    CPSS_EXMXPM_FDB_LUT_ENTRY_STC   lutEntry;
    GT_U32  ii;
    GT_U32  emptyIndex = 0xff;

    if(fdbEntrySetMode == WR_FDB_ENTRY_SET_MODE_BY_MESSAGE_E)
    {
        /* call cpss api function */
        wrapCpssExMxPmBrgAuqMutexLock();
        do{
            result = cpssExMxPmBrgFdbEntrySet(devNum, fdbEntryPtr);
            wrapCpssExMxPmBrgFdbResolveDeviceDeadLock(devNum,
                        WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_NA_AA_FROM_CPU_E,
                        &repeatAction);
            numRetries --;
        }while(repeatAction == GT_TRUE && numRetries);
        wrapCpssExMxPmBrgAuqMutexUnLock();
    }
    else
    {

        if(PRV_CPSS_EXMXPM_PP_MAC(devNum)->moduleCfg.bridgeCfg.fdbMemoryMode
            == CPSS_EXMXPM_PP_CONFIG_FDB_TABLE_MODE_EXTERNAL_E)
        {
            /* FDB in the external memory --> calculate also the LUT hash */
            result = cpssExMxPmBrgFdbExternalHashCalc(devNum,&fdbEntryPtr->key,
                        &lutHashIndex,
                        &lutSecHashValue,
                        &fdbHashIndex);
            if(result != GT_OK)
            {
                return result;
            }

            /* read the LUT table */
            result = cpssExMxPmBrgFdbLutEntryRead(devNum,lutHashIndex,&lutEntry);
            if(result != GT_OK)
            {
                return result;
            }

            for(ii = 0 ; ii < CPSS_EXMXPM_FDB_LUT_SUB_ENTRIES_NUM_CNS ; ii ++)
            {
                if(lutEntry.subEntry[ii].isValid == GT_FALSE)
                {
                    if(emptyIndex == 0xff)
                    {
                        emptyIndex = ii;
                    }
                }
                else if (lutEntry.subEntry[ii].lutSecHashValue == lutSecHashValue)
                {
                    /* LUT sub-entry already exists */
                    break;
                }
            }

            if(ii == CPSS_EXMXPM_FDB_LUT_SUB_ENTRIES_NUM_CNS)
            {
                /* entry was NOT found */
                if(emptyIndex == 0xff)
                {
                    /* force the LUT sub-entry to start index */
                    ii = 0;
                }
                else
                {
                    /* set the LUT sub-entry instead of empty one */
                    ii = emptyIndex;
                }
            }

            /* validate the LUT sub-entry */
            lutEntry.subEntry[ii].isValid = GT_TRUE;
            lutEntry.subEntry[ii].lutSecHashValue = lutSecHashValue;
            /* set the offset to 0 , to indicate that the FDB entry exactly
               on start of the hash in the FDB */
            lutEntry.subEntry[ii].fdbEntryOffset = 0;


            /* write the updated LUT table */
            result = cpssExMxPmBrgFdbLutEntryWrite(devNum,lutHashIndex,&lutEntry);
            if(result != GT_OK)
            {
                return result;
            }
        }
        else
        {
            /* FDB in the internal memory */
            result = cpssExMxPmBrgFdbInternalHashCalc(devNum,&fdbEntryPtr->key,&fdbHashIndex);
            if(result != GT_OK)
            {
                return result;
            }
        }

        result = fdbEntryWrite(devNum,fdbHashIndex,GT_FALSE,fdbEntryPtr);
    }

    return result;
}

/*******************************************************************************
* fdbEntryDelete
*
* DESCRIPTION:
*       internal wrapper for the cpssExMxPmBrgFdbEntryDelete / cpssExMxPmBrgFdbEntryWrite
*       that decided according to fdbEntrySetMode
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       fdbEntryKeyPtr  - pointer to key parameters of the FDB entry
*                         according to the entry type:
*                         MAC Address entry -> MAC Address + vlan ID.
*                         IPv4/IPv6 entry -> srcIP + dstIP + vald ID.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vlanId
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS fdbEntryDelete
(
    IN GT_U8                            devNum,
    IN CPSS_EXMXPM_FDB_ENTRY_KEY_STC    *fdbEntryKeyPtr
)
{
    GT_STATUS                       result;
    GT_BOOL repeatAction;
    GT_U32  numRetries=10;
    CPSS_EXMXPM_FDB_ENTRY_STC       fdbEntry;
    GT_U32  lutHashIndex,lutSecHashValue,fdbHashIndex;
    CPSS_EXMXPM_FDB_LUT_ENTRY_STC   lutEntry;
    GT_U32  ii;

    if(fdbEntrySetMode == WR_FDB_ENTRY_SET_MODE_BY_MESSAGE_E)
    {
        wrapCpssExMxPmBrgAuqMutexLock();
        do{
            result = cpssExMxPmBrgFdbEntryDelete(devNum, fdbEntryKeyPtr);
            wrapCpssExMxPmBrgFdbResolveDeviceDeadLock(devNum,
                        WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_NA_AA_FROM_CPU_E,
                        &repeatAction);
            numRetries --;
        }while(repeatAction == GT_TRUE && numRetries);
        wrapCpssExMxPmBrgAuqMutexUnLock();
    }
    else
    {
        cmdOsMemSet(&fdbEntry,0,sizeof(fdbEntry));
        fdbEntry.key = *fdbEntryKeyPtr;


        if(PRV_CPSS_EXMXPM_PP_MAC(devNum)->moduleCfg.bridgeCfg.fdbMemoryMode
            == CPSS_EXMXPM_PP_CONFIG_FDB_TABLE_MODE_EXTERNAL_E)
        {
            /* FDB in the external memory --> calculate also the LUT hash */
            result = cpssExMxPmBrgFdbExternalHashCalc(devNum,&fdbEntry.key,
                        &lutHashIndex,
                        &lutSecHashValue,
                        &fdbHashIndex);
            if(result != GT_OK)
            {
                return result;
            }


            /* read the LUT table */
            result = cpssExMxPmBrgFdbLutEntryRead(devNum,lutHashIndex,&lutEntry);
            if(result != GT_OK)
            {
                return result;
            }

            for(ii = 0 ; ii < CPSS_EXMXPM_FDB_LUT_SUB_ENTRIES_NUM_CNS ; ii ++)
            {
                if(lutEntry.subEntry[ii].isValid == GT_TRUE)
                {
                    continue;
                }

                if (lutEntry.subEntry[ii].lutSecHashValue == lutSecHashValue)
                {
                    /* LUT sub-entry exists */
                    break;
                }
            }

            if(ii != CPSS_EXMXPM_FDB_LUT_SUB_ENTRIES_NUM_CNS)
            {
                /* invalidate the LUT sub-entry */
                lutEntry.subEntry[ii].isValid = GT_FALSE;
                lutEntry.subEntry[ii].lutSecHashValue = 0;
                lutEntry.subEntry[ii].fdbEntryOffset = 0;

                /* write the updated LUT table */
                result = cpssExMxPmBrgFdbLutEntryWrite(devNum,lutHashIndex,&lutEntry);
                if(result != GT_OK)
                {
                    return result;
                }
            }
        }
        else
        {
            /* FDB in the internal memory */
            result = cpssExMxPmBrgFdbInternalHashCalc(devNum,&fdbEntry.key,&fdbHashIndex);
            if(result != GT_OK)
            {
                return result;
            }
        }

        /* invalidate the entry -->
           NOTE: the hash index will never be in the TCAM entries !!!
        */
        /* disable LSR for 'direct' writing to the FDB PP memory */
        wrCpssExMxPmBrgFdbLsrDisable(devNum);
        result = cpssExMxPmBrgFdbEntryInvalidate(devNum,fdbHashIndex);
        /* enable LSR after 'direct' writing to the FDB PP memory */
        cpssExMxPmBrgFdbLsrEnableSet(devNum,GT_TRUE);
    }

    return result;
}

/*******************************************************************************
* wrCpssExMxPmBrgFdbLsrDisable
*
* DESCRIPTION:
*       Disable the FDB Learning, Scanning and Refresh operations.
*       Halting of these operation allows the CPU uninterrupted access to
*       the FDB.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_TIMEOUT               - timed out
* COMMENTS:
*   The function reset the <LSR stopped> bit before disable L.S.R
*
*   relate to CQ#110147
*
*******************************************************************************/
static GT_STATUS wrCpssExMxPmBrgFdbLsrDisable
(
    IN GT_U8         devNum
)
{
    GT_STATUS                       result;
    GT_BOOL repeatAction;
    GT_U32  numRetries=10;

    do{
        result = cpssExMxPmBrgFdbLsrEnableSet(devNum, GT_FALSE);
        wrapCpssExMxPmBrgFdbResolveDeviceDeadLock(devNum,
                    WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_LSR_DISABLE_E,
                    &repeatAction);
        numRetries --;
    }while(repeatAction == GT_TRUE && numRetries);

    return result;
}

/*******************************************************************************
* exMxPmFdbEntryWrite
*
* DESCRIPTION:
*       Write the new entry in Hardware FDB table in the specified index.
*       !!! disable the LSR while doing it !!!
*       !!! wait for the LSR to be stopped !!!
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       index           - hw fdb entry index
*       skip            - entry skip control
*                         GT_TRUE - used to "skip" the entry ,
*                         the HW will treat this entry as "not used"
*                         GT_FALSE - used for valid/used entries.
*       fdbEntryPtr     - pointer to FDB entry parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum,saCommand,daCommand
*       GT_OUT_OF_RANGE - vidx/trunkId/portNum-devNum with values bigger then HW
*                         support
*                         index out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS fdbEntryWrite
(
    IN GT_U8                            devNum,
    IN GT_U32                           index,
    IN GT_BOOL                          skip,
    IN CPSS_EXMXPM_FDB_ENTRY_STC        *fdbEntryPtr
)
{
    GT_STATUS error,rc;                 /* return code      */

    /* disable LSR for 'direct' writing to the FDB PP memory */
    rc = wrCpssExMxPmBrgFdbLsrDisable(devNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* write Mac entry by index */
    /* call cpss api function */
    if(index < PRV_CPSS_EXMXPM_PP_MAC(devNum)->moduleCfg.bridgeCfg.fdbNumOfEntries)
    {
        rc = cpssExMxPmBrgFdbEntryWrite(devNum, index, skip, fdbEntryPtr);
    }
    else
    {
        /* this function will write to the : FDB table in the 256
           'Reserved for tcam' entries and to the actual TCAM */
        rc = cpssExMxPmBrgFdbTcamEntryWrite(devNum,
                index - PRV_CPSS_EXMXPM_PP_MAC(devNum)->moduleCfg.bridgeCfg.fdbNumOfEntries,
                skip, fdbEntryPtr);
    }

    error = rc; /* save error value if occurred prior to next line */

    /* enable LSR after 'direct' writing to the FDB PP memory */
    rc = cpssExMxPmBrgFdbLsrEnableSet(devNum,GT_TRUE);

    return (error != GT_OK) ? error : rc;
}


/*******************************************************************************
* wrapCpssExMxPmBrgFdbEntrySetMode
*
* DESCRIPTION:
*       set the 'mode' for 'entry set' operation.
*       modes are :
*           1. set entry 'by message' --> pp calculate FDB hash
*           2. write entry by index --> wrapper will calculate FDB hash (+LUT for external memory)
*
* INPUTS:
*       none
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       function to use by terminal command until wrapper to the function
*       wrCpssExMxPmBrgFdbEntrySetMode exists in Galtis
*
*******************************************************************************/
GT_STATUS wrapCpssExMxPmBrgFdbEntrySetMode
(
    IN  WR_FDB_ENTRY_SET_MODE_ENT   mode
)
{
    /* map input arguments to locals */
    fdbEntrySetMode = mode;

    return GT_OK;
}

#define CPSS_FDB_DB_DEBUG
#ifdef CPSS_FDB_DB_DEBUG

GT_STATUS dumpTable
(
    IN  PRV_CPSS_EXMXPM_TABLE_INFO_STC  *tablePtr
)
{
    GT_U32 data[20];
    GT_U32 i;

    cpssOsPrintf("tablePtr->baseAddr  = 0x%x\n",tablePtr->baseAddr);
    cpssOsPrintf("tablePtr->memType = %d\n", tablePtr->memType);
    cpssOsPrintf("tablePtr->entrySize = %d\n", tablePtr->entrySize);
    cpssOsPrintf("tablePtr->lineAddrAlign = %d\n", tablePtr->lineAddrAlign);
    cpssOsPrintf("tablePtr->numOfEntries = %d\n", tablePtr->numOfEntries);

    /* fields for memory controller */
    if(tablePtr->memCtrlPtr)
    {
        cpssOsMemCpy(data, &(tablePtr->memCtrlPtr->csuInfoPtr->controlerInfo),
                      sizeof(PRV_CPSS_EXMXPM_MEMORY_CONTROLER_INFO_STC));

        cpssOsPrintf("tablePtr->memCtrlPtr->csuInfoPtr->controlerInfo\n");
        for(i = 0; i < 10; i++)
        {
            cpssOsPrintf("data[%d] = %d\n", i, data[i]);
        }

        cpssOsMemCpy(data, &(tablePtr->memCtrlPtr->csuInfoPtr->eccEmulationInfo),
                      sizeof(PRV_CPSS_EXMXPM_CSU_ECC_EMULATION_INFO));

        cpssOsPrintf("tablePtr->memCtrlPtr->csuInfoPtr->eccEmulationInfo\n");
        for(i = 0; i < 10; i++)
        {
            cpssOsPrintf("data[%d] = %d\n", i, data[i]);
        }

        cpssOsPrintf("tablePtr->memCtrlPtr->generalControlRegisterAddr = %d\n",tablePtr->memCtrlPtr->generalControlRegisterAddr);
        cpssOsPrintf("tablePtr->memCtrlPtr->lineAddrAlign = %d\n",tablePtr->memCtrlPtr->lineAddrAlign);
        cpssOsPrintf("tablePtr->memCtrlPtr->lineWidthInBits = %d\n",tablePtr->memCtrlPtr->lineWidthInBits);
        cpssOsPrintf("tablePtr->memCtrlPtr->memCheckType = %d\n",tablePtr->memCtrlPtr->memCheckType);

    }

    cpssOsPrintf("tablePtr->swNeedCalcEccOrParity = %d\n",tablePtr->swNeedCalcEccOrParity);
    cpssOsPrintf("tablePtr->dataContainParityBit = %d\n",tablePtr->dataContainParityBit);
    cpssOsPrintf("tablePtr->tableType = %d\n",tablePtr->tableType);

    return GT_OK;

}

GT_STATUS dumpFdbTable
(
    IN  GT_U8    dev
)
{
    PRV_CPSS_EXMXPM_TABLE_INFO_STC  *tablePtr;

    tablePtr = PRV_CPSS_EXMXPM_PP_MAC(dev)->tblsAddr.genericPointers.fdbTablePtr;

    return dumpTable(tablePtr);

}


GT_STATUS dumpFdbTcamTable
(
    IN  GT_U8    dev
)
{
    PRV_CPSS_EXMXPM_TABLE_INFO_STC  *tablePtr;

    tablePtr = &PRV_CPSS_EXMXPM_PP_MAC(dev)->tblsAddr.MT.fdbEntriesForTcam;

    return dumpTable(tablePtr);

}

#endif

