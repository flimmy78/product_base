/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* osWin32Task.c
*
* DESCRIPTION:
*       Win32 Operating System Simulation. Task facility implementation.
*
* DEPENDENCIES:
*       Win32, CPU independed.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <windows.h>
#include <process.h>
#include <time.h>
#include <winbase.h>
#include <assert.h>

#include <gtOs/gtOsTask.h>
#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsTimer.h>

#include <asicSimulation/SCIB/scib.h>

#define PROTECT_TASK_DOING_READ_WRITE_START_MAC         SCIB_SEM_TAKE

#define PROTECT_TASK_DOING_READ_WRITE_END_MAC       SCIB_SEM_SIGNAL

/* SHOST functions that manage registered tasks */
extern GT_VOID t_reg_task_table(DWORD win_tid, HANDLE hnd, const GT_CHAR *task_name);

/************ Defines  ********************************************************/

static GT_SEM OS_TaskSemId;
static GT_STATUS osTaskInit (GT_VOID);

#define OS_MAX_TASK_NAME_LENGTH 20

#define OS_MAX_TASKS    20

typedef struct
{
    GT_BOOL         valid;
    GT_CHAR         threadName[OS_MAX_TASK_NAME_LENGTH];
    HANDLE          threadHandle;
    DWORD           threadId;
}OS_TASK_T;

static OS_TASK_T     OS_TaskArray[OS_MAX_TASKS];

/************ Public Functions ************************************************/

/*******************************************************************************
* osTaskCreate
*
* DESCRIPTION:
*       Create OS Task and start it.
*
* INPUTS:
*       name    - task name, string no longer then OS_MAX_TASK_NAME_LENGTH
*       prio    - task priority 1 - 64 => HIGH
*       stack   - task Stack Size
*       start_addr - task Function to execute
*       arglist    - pointer to list of parameters for task function
*
* OUTPUTS:
*       tid   - Task ID
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osTaskCreate
(
    IN  GT_CHAR *name,
    IN  GT_U32  prio,
    IN  GT_U32  stack,
    IN  unsigned (__TASKCONV *start_addr)(GT_VOID*),
    IN  GT_VOID *arglist,
    OUT GT_TASK *tid
)
{
    DWORD                       i, threadID;
    HANDLE                      hThread;
    static GT_BOOL facInit  =   GT_FALSE;

    if (facInit == GT_FALSE)
    {
        osTaskInit();
        facInit = GT_TRUE;
    }

    if(strlen(name)>OS_MAX_TASK_NAME_LENGTH)
        return(GT_FAIL);

    osSemWait(OS_TaskSemId, 0);

    /* Look for free place in OS_TaskArray */
    for(i = 1; i < OS_MAX_TASKS; i++)
    {
        if( OS_TaskArray[i].valid == GT_FALSE)
            break;
    }
    if(i == OS_MAX_TASKS)
    {
        /* No more place in the OS_TaskArray */
        osSemSignal(OS_TaskSemId);
        return GT_FAIL;
    }
    /* Create thread and save Handle */
    hThread = CreateThread(NULL, stack,(LPTHREAD_START_ROUTINE)start_addr,arglist, CREATE_SUSPENDED,&threadID);

    /* A handle to the new thread indicates success. NULL indicates failure. */
    if(hThread == NULL)
    {
        /* Can't create Thread */
        osSemSignal(OS_TaskSemId);
        return GT_FAIL;
    }

    /* register the task with the SHOST */
    /* we need to register the task , because the SHOST in the context of
       'interrupts' will do 'tasks lock' but not via the APIs of mainOs but only
       on the tasks that are registered internally with the SHOST
       meaning we must register out tasks there */
    t_reg_task_table( threadID ,hThread, name );


    /* Save parameters in the taskArray */
    OS_TaskArray[i].valid = GT_TRUE;
    OS_TaskArray[i].threadHandle = hThread;
    OS_TaskArray[i].threadId = threadID;
    strcpy(OS_TaskArray[i].threadName, name);

    if(tid != NULL)
    { /* task ID is a thread ID */
        *tid = i;
    }

    osSemSignal(OS_TaskSemId);

    osTaskResume(i);

    osSetTaskPrior(i, prio, NULL);

    return GT_OK;
}


/*******************************************************************************
* osTaskDelete
*
* DESCRIPTION:
*       Deletes existing task.
*
* INPUTS:
*       tid   - Task ID
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       If tid = 0, delete calling task (itself)
*
*******************************************************************************/
GT_STATUS osTaskDelete
(
    IN GT_TASK tid
)
{
        BOOL        ret;
        DWORD        threadId;
        int                i;
        GT_TASK        it=0;

    osSemWait(OS_TaskSemId, 0);

        /* check validity of task number */
        if(tid != 0 && OS_TaskArray[tid].valid == GT_FALSE)
        {
                osSemSignal(OS_TaskSemId);
                return GT_FALSE;
        }

        /* get current thread Id */
        threadId = GetCurrentThreadId();
        /* If terminating this thread */
    if (tid == 0 || OS_TaskArray[tid].threadId == threadId)
    {/* Terminate current thread with code 0 */
                /* Look for task ID in OS_TaskArray */
                for(i = 1, ret = FALSE; i < OS_MAX_TASKS; i++)
                {
                        if( OS_TaskArray[i].valid == GT_TRUE &&
                                OS_TaskArray[i].threadId == threadId)
                        {
                                it = i; /* current task Id */
                                ret = TRUE; /* tid is found */
                                break;
                        }
                }

                if(ret == TRUE)
                {/* update entry */
                        OS_TaskArray[it].valid = GT_FALSE;        /* invalidate entry */
                        OS_TaskArray[it].threadId = 0;
                        OS_TaskArray[it].threadName[0] = 0;
                        OS_TaskArray[it].threadHandle = 0;
                }

                osSemSignal(OS_TaskSemId);
                ExitThread(0);
    }


        /* terminate not current thread */
        ret = TerminateThread(OS_TaskArray[tid].threadHandle, 0);
        if(ret)
        { /* thread was successfully terminated */

                OS_TaskArray[tid].valid = GT_FALSE;        /* invalidate entry */
                OS_TaskArray[tid].threadId = 0;
                OS_TaskArray[tid].threadName[0] = 0;
                OS_TaskArray[tid].threadHandle = 0;
        }

        osSemSignal(OS_TaskSemId);

    if(!ret)
        {
                return GT_FAIL;
    }

        return GT_OK;
}

/*******************************************************************************
* osTaskSuspend
*
* DESCRIPTION:
*       Suspends existing task/thread.
*
* INPUTS:
*       tid   - Task ID
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       If tid = 0, suspend calling task (itself)
*
*******************************************************************************/
GT_STATUS osTaskSuspend
(
    IN GT_TASK tid
)
{
        DWORD        retVal;

        if(tid == 0)
        {/* suspend current thread */
                retVal = SuspendThread(GetCurrentThread());
        }
        else
        {
                /* check validity of task number */
                if(OS_TaskArray[tid].valid == GT_FALSE)
                        return GT_FALSE;
                /* suspend thread */
                retVal = SuspendThread(OS_TaskArray[tid].threadHandle);
        }

        if(retVal == -1)
                return GT_FAIL;

    return GT_OK;
}


/*******************************************************************************
* osTaskResume
*
* DESCRIPTION:
*       Resumes existing task/thread.
*
* INPUTS:
*       tid   - Task ID
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS osTaskResume
(
    IN GT_TASK tid
)
{
        DWORD        retVal;

        if(tid == 0)
        {/* resume current thread */
                retVal = ResumeThread(GetCurrentThread());
        }
        else
        {
                /* check validity of task number */
                if(OS_TaskArray[tid].valid == GT_FALSE)
                        return GT_FALSE;
                /* suspend thread */
                retVal = ResumeThread(OS_TaskArray[tid].threadHandle);
        }

        if(retVal == -1)
                return GT_FAIL;

    return GT_OK;

}


/*******************************************************************************
* osTaskGetSelf
*
* DESCRIPTION:
*       returns the current task (thread) id
*
* INPUTS:
*       none
*
* OUTPUTS:
*       tid -  the current task (thread) id
*
* RETURNS:
*       GT_OK - on success.
*       GT_FAIL - if parameter is invalid
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osTaskGetSelf
(
    OUT GT_U32 *tid
)
{
    DWORD        threadId;
    int                i;

        /* check validity of function arguments */
    if (tid == NULL)
        return GT_FAIL;

    threadId = GetCurrentThreadId();
    /* look for thread in table */
    for(i = 1; i < OS_MAX_TASKS; i++)
    {
        if(threadId == OS_TaskArray[i].threadId)
        {/* if thread ID found */
            *tid = i;
            return GT_OK;
        }
    }

    *tid = threadId;/* return the 'WIN32' handle */

    return GT_FAIL;
}


/*******************************************************************************
* osSetTaskPrior
*
* DESCRIPTION:
*       Changes priority of task/thread.
*
* INPUTS:
*       tid     - Task ID
*       newprio - new priority of task
*
* OUTPUTS:
*       oldprio - old priority of task
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       If tid = 0, change priotity of calling task (itself)
*
*******************************************************************************/
GT_STATUS osSetTaskPrior
(
    IN  GT_TASK tid,
    IN  GT_U32  newprio,
    OUT GT_U32  *oldprio
)
{
        HANDLE        hThread;
        int           win32Pri;

        if(tid == 0)
        {/* get handle to the current thread */
                hThread = GetCurrentThread();
        }
        else
        {
                /* check validity of task number */
                if(OS_TaskArray[tid].valid == GT_FALSE)
                        return GT_FAIL;

                /* get handle to the task */
                hThread = OS_TaskArray[tid].threadHandle;
        }

        /* get old and set new priority */
    if(oldprio != NULL)
        *oldprio = (unsigned long)GetThreadPriority(hThread);

    switch (newprio)
    {
        case 10: win32Pri = THREAD_PRIORITY_ABOVE_NORMAL; break;
        default: win32Pri = THREAD_PRIORITY_NORMAL; break;
    }

    if( SetThreadPriority(hThread, win32Pri) == FALSE)
        return  GT_FAIL;

    return GT_OK;
}

/*******************************************************************************
* osGetTaskPrior
*
* DESCRIPTION:
*       Gets priority of task/thread.
*
* INPUTS:
*       tid     - Task ID
*
* OUTPUTS:
*       prio - priority of task
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       If tid = 0, gets priotity of calling task (itself)
*
*******************************************************************************/
GT_STATUS osGetTaskPrior
(
    IN  GT_TASK tid,
    OUT GT_U32  *prio
)
{
        HANDLE        hThread;

    if(prio == NULL)
        return GT_FAIL;


        if(tid == 0)
        {/* get handle to the current thread */
                hThread = GetCurrentThread();
        }
        else
        {
                /* check validity of task number */
                if(OS_TaskArray[tid].valid == GT_FALSE)
                        return GT_FAIL;

                /* get handle to the task ID*/
                hThread = OS_TaskArray[tid].threadHandle;
        }



    *prio = (unsigned long)GetThreadPriority(hThread);

    return GT_OK;
}

/*******************************************************************************
* osTaskLock
*
* DESCRIPTION:
*       Disable task rescheduling of current task.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osTaskLock(GT_VOID)
{
    GT_U8 tdLoop;
    GT_U32 tid;

    tid = GetCurrentThreadId();

    PROTECT_TASK_DOING_READ_WRITE_START_MAC;

    for (tdLoop = 0 ; tdLoop < OS_MAX_TASKS ; tdLoop++)
    {
        if ( (OS_TaskArray[tdLoop].valid) &&
             (OS_TaskArray[tdLoop].threadId != tid) )
        {
            if (osTaskSuspend (tdLoop) != GT_OK)
            {
                PROTECT_TASK_DOING_READ_WRITE_END_MAC;
                return GT_FAIL;
            }
        }
    }

    PROTECT_TASK_DOING_READ_WRITE_END_MAC;

    return GT_OK;
}


/*******************************************************************************
* osTaskUnLock
*
* DESCRIPTION:
*       Enable task rescheduling.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osTaskUnLock (GT_VOID)
{
    GT_U8 tdLoop;
    GT_U32 tid;

    tid = GetCurrentThreadId();

    for (tdLoop = 0 ; tdLoop < OS_MAX_TASKS ; tdLoop++)
    {
        if ( (OS_TaskArray[tdLoop].valid) &&
             (OS_TaskArray[tdLoop].threadId != tid) )
        {
          if (osTaskResume (tdLoop) != GT_OK)
            return GT_FAIL;
        }
    }

    return GT_OK;
}


static GT_STATUS osTaskInit (GT_VOID)
{
    long    i, rc;

    for (i=0; i<OS_MAX_TASKS; i++)
    {
                OS_TaskArray[i].valid = GT_FALSE;
        OS_TaskArray[i].threadHandle = 0;
        OS_TaskArray[i].threadId = 0;
        OS_TaskArray[i].threadName[0] = 0;
    }
    rc = osSemBinCreate(NULL, 1, &OS_TaskSemId);

    return rc;
}



