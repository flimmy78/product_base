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
* gtAppInit.c
*
* DESCRIPTION:
*       This file includes functions to be called on system initialization,
*       for demo and special purposes.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/


#include <cmdShell/shell/cmdMain.h>

/* sample code fo initializatuion befor the shell starts */
#ifdef INIT_BEFORE_SHELL

#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsTask.h>

GT_SEM semCmdSysInit;

/*******************************************************************************
* cpssInitSystemCall
*
* DESCRIPTION:
*       This routine is the starting point of the Driver.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssInitSystemCall(GT_VOID)
{
    GT_STATUS rc;
    printf("cpssInitSystem(25,1,0,0) will be called\n");
    rc = cpssInitSystem(25,1,0,0);
    printf("cpssInitSystem(25,1,0,0) returned 0x%X\n", rc );
    rc = osSemSignal(semCmdSysInit);
    printf("Signal semaphore, rc = 0x%X\n", rc);
    return GT_OK;
}
/*******************************************************************************
* cpssInitSystemCallTask
*
* DESCRIPTION:
*       This routine is the starting point of the Driver.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssInitSystemCallTask(GT_VOID)
{
    GT_STATUS rc;
    GT_U32    taskSysInitCmd;

    rc = osWrapperOpen(NULL);
    printf("osWrapperOpen called, rc = 0x%X\n", rc);
    rc = osSemBinCreate("semCmdSysInit", OS_SEMB_EMPTY, &semCmdSysInit);
    printf("Create semCmdSysInit semaphore, rc = 0x%X\n", rc);
    rc = osTaskCreate (
        "CMD_InitSystem", 10 /* priority */,
        32768 /*stack size */,
        (unsigned (__TASKCONV *)(void*)) cpssInitSystemCall,
        NULL, &taskSysInitCmd);
    printf("Create taskSysInitCmd task, rc = 0x%X, id = 0x%X \n",
           rc, taskSysInitCmd);
    rc = osSemWait(semCmdSysInit, 600000); /* 10 minutes */
    osTimerWkAfter(1000);
    printf("Wait for semaphore, rc = 0x%X\n", rc);
    return GT_OK;
}

#endif /*INIT_BEFORE_SHELL*/

/*******************************************************************************
* userAppInit
*
* DESCRIPTION:
*       This routine is the starting point of the Driver.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS userAppInit(GT_VOID)
{
    /* Set gtInitSystem to be the init function */
    /*cpssInitSystemFuncPtr = (GT_INTFUNCPTR)cpssInitSystem;*/

#ifdef INIT_BEFORE_SHELL
    printf("cpssInitSystemCallTask will be called\n");
    cpssInitSystemCallTask();
    printf("cpssInitSystemCallTask was called\n");
#endif /*INIT_BEFORE_SHELL*/
    /* Start the command shell */
    cmdInit(0);

    return GT_OK;
} /* userAppInit */



