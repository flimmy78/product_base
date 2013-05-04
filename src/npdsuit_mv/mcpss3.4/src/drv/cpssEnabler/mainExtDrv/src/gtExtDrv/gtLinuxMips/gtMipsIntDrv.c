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
* gt88fxx81IntDrv.c
*
* DESCRIPTION:
*       Interrupt driver for linux uClinux based boards.
*
* DEPENDENCIES:
*       -   System controller.
*       -   LSP.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#include <gtOs/gtOsGen.h>
#include <gtOs/gtOsIntr.h>
#include <gtOs/gtOsTask.h>
#include <gtOs/gtOsMem.h>


#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <linux/if_ether.h>
#include <linux/sockios.h>
#include <string.h>

#include "kerneldrv/include/prestera_glob.h"


#define IN
#define OUT

/******************************************************************************
* struct intTaskParams_STC
* 
* DESCRIPTION:
*       This structure is used to pass parameters to intTask()
*******************************************************************************/
struct intTaskParams_STC {
    GT_U32 devNum;
    unsigned int cookieLow32;
	unsigned int cookieHigh32;
};
#ifdef __AX_PLATFORM__
static GT_U32 tid;
#else
typedef struct _intTaskBind_S {
	GT_U32 tid;
	GT_U32 intVecNum;
}INT_TASK_BIND_t;

INT_TASK_BIND_t tidArr[2] = { 	\
	{	0,	0	},				\
	{	0,	0	}				\
};
#endif
static GT_VOIDINTFUNCPTR _userRoutine;

/*******************************************************************************
* npd_init_tell_whoami
*
* DESCRIPTION:
*      This function is used by each thread to tell its name and pid to NPD_ALL_THREAD_PID_PATH
*
* INPUTS:
*	  tName - thread name.
*	  lastTeller - is this the last teller or not, pid file should be closed if true.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*	  None.
*
* COMMENTS:
*       
*
*******************************************************************************/
extern void npd_init_tell_whoami
(
	unsigned char *tName,
	unsigned char lastTeller
);

/*************** Globals ******************************************************/

/* file descriptor returnd by openning the PP *nix device driver */
GT_32 gtPpFd;


/*******************************************************************************
* extDrvIntEnable
*
* DESCRIPTION:
*       Enable corresponding interrupt bits
*
* INPUTS:
*       intVecNum - new interrupt bits
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
GT_STATUS extDrvIntEnable
(
    IN GT_U32   intVecNum
)
{
	#ifndef __AX_PLATFORM__
	GT_U32 tid = 0;
	
	if(intVecNum == tidArr[0].intVecNum)
		tid = tidArr[0].tid;
	else if(intVecNum == tidArr[1].intVecNum)
		tid = tidArr[1].tid;
	else
		tid = 0;
	#endif

    if (0 == tid)
    {
        fprintf(stderr, "intEnable - tid(0): %s\n", strerror(errno));
        return GT_FAIL;
    }

    if (osTaskResume(tid))
    {
        fprintf(stderr, "osTaskResume: %s\n", strerror(errno));
        return GT_FAIL;
    }

	#ifdef __AX_PLATFORM__
    /* Enable the IRQ  */
    if (ioctl (gtPpFd, PRESTERA_IOC_INTENABLE, intVecNum))
    {
        fprintf(stderr, "Interrupt enabled failed: errno(%s)\n",
                        strerror(errno));
        return GT_FAIL;
    }
    #endif

    return GT_OK ;
}


/*******************************************************************************
* extDrvIntDisable
*
* DESCRIPTION:
*       Disable corresponding interrupt bits.
*
* INPUTS:
*       intVecNum - new interrupt bits
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
GT_STATUS extDrvIntDisable
(
    IN GT_U32   intVecNum
)
{
	GT_U32 tid = 0;
	
	#ifdef __AX_PLATFORM__	
    /* Disable the irq */
    if (ioctl (gtPpFd, PRESTERA_IOC_INTDISABLE, intVecNum))
    {
        fprintf(stderr, "Interrupt disabled failed: errno(%s)\n",
                        strerror(errno));
        return GT_FAIL;
    }
	#endif
	
    #ifndef __AX_PLATFORM__	
	if(intVecNum == tidArr[0].intVecNum)
		tid = tidArr[0].tid;
	else if(intVecNum == tidArr[1].intVecNum)
		tid = tidArr[1].tid;
	else
		tid = 0;
	#endif

    if (0 == tid)
    {
        fprintf(stderr, "intDisable - tid(0): %s\n", strerror(errno));
        return GT_FAIL;
    }

    if (osTaskSuspend(tid))
    {
        fprintf(stderr, "osTaskSuspend: %s\n", strerror(errno));
        return GT_FAIL;
    }

    return GT_OK ;
}


/*******************************************************************************
* intTask
*
* DESCRIPTION:
*       Interrupt handler task.
*
* INPUTS:
*       param1  - device number
*       param2  - ISR cookie
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
unsigned __TASKCONV intTask(GT_VOID *paramsPtr)
{
    GT_U32 devNum;
    GT_Cookie_Ptr_STC cookie;
	unsigned char tName[32] = {0};
	
    cookie.low32	= ((struct intTaskParams_STC*)paramsPtr)->cookieLow32;
	cookie.high32	= ((struct intTaskParams_STC*)paramsPtr)->cookieHigh32; 
     
    devNum = ((struct intTaskParams_STC*)paramsPtr)->devNum;

	/* tell my thead id */
	sprintf(tName,"intTask%d",devNum);
	npd_init_tell_whoami(tName,0);

    osFree(paramsPtr);
    
    /* No need to wait because not interrupts in the driver until intEnable */
    while (1)
    {
        /* Wait for interrupt */
        if (ioctl (gtPpFd, PRESTERA_IOC_WAIT, &cookie))
        {
            fprintf(stderr, "Interrupt wait failed: errno(%s)\n",
                            strerror(errno));
            return (int)GT_FAIL;
        }

        osTaskLock();
		#ifndef __AX_PLATFORM__
        _userRoutine(devNum);
		#endif

        osTaskUnLock();
    }
}


/*******************************************************************************
* extDrvIntConnect
*
* DESCRIPTION:
*       Connect a specified C routine to a specified interrupt vector.
*
* INPUTS:
*       vector    - interrupt vector number to attach to
*       routine   - routine to be called
*       parameter - parameter to be passed to routine
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
GT_U32 extDrvIntConnect
(
    IN  GT_U32           intVec,
    IN  GT_VOIDFUNCPTR   routine,
    IN  GT_U32           parameter
)
{
    static unsigned int intialized[2] = { 0, 0 };
    GT_U8 taskName[32];
    GT_VecotrCookie_STC vector_cookie;
    struct intTaskParams_STC* paramsPtr;
	GT_U32 tid = 0;
	/* OCTEON_IRQ_PCI_INT0 is start from 128 in SDK2.2, zhangdi@autelan.com 2011-01-08 */
    #if 0   
    if (intVec > 63)
    {
        fprintf(stderr, "unknown interrupt vector: %ul\n", (int)intVec);
        return GT_FAIL;
    }
    if (intialized[intVec >> 5] & (1 << (intVec & 0x1F)))
    {
        fprintf(stderr, "intialized: %s\n", strerror(errno));
        return GT_FAIL;
    }
    intialized[intVec >> 5] |= (1 << (intVec & 0x1F));
    #else
    osPrintf("interrupt vector: %ul\n", (int)intVec);
    /*OCTEON_IRQ_PCI_INT0 in src\kernel2.6.32.27cn\arch\mips\include\asm\Mach-cavium-octeon\irq.h */	
    if (intVec > 131)
    {
        fprintf(stderr, "unknown interrupt vector: %ul\n", (int)intVec);
        return GT_FAIL;
    }
	/*
    if (intialized[intVec >> 8] & (1 << (intVec & 0xFF)))
    {
        fprintf(stderr, "intialized: %s\n", strerror(errno));
        return GT_FAIL;
    }
    intialized[intVec >> 8] |= (1 << (intVec & 0xFF));
	*/
	#endif
    vector_cookie.vector = intVec;
    if (ioctl (gtPpFd, PRESTERA_IOC_INTCONNECT, &vector_cookie))
    {
        fprintf(stderr, "Interrupt connect failed: errno(%s)\n",
                        strerror(errno));
        return GT_FAIL;
    }
    _userRoutine = (GT_VOIDINTFUNCPTR)routine;

    sprintf(taskName, "intTask%d", parameter);

    paramsPtr = (struct intTaskParams_STC*)osMalloc(sizeof(*paramsPtr));
	if(paramsPtr==NULL)
	{
		osPrintf("int connect memory alloc error!\n");
		return GT_NO_RESOURCE;
	}
	osMemSet(paramsPtr,0,sizeof(*paramsPtr));
    paramsPtr->cookieLow32 = vector_cookie.cookieLow32;
    paramsPtr->cookieHigh32 = vector_cookie.cookieHigh32;
	paramsPtr->devNum = parameter;
    
    osTaskCreate(taskName, 0/*prio*/, 0x2000/*stk size*/,
            intTask, paramsPtr, &tid);

    if (0 == tid)
    {
        osFree(paramsPtr);
        fprintf(stderr, "extDrvIntConnect: osTaskCreate(%s): %s\n", taskName, strerror(errno));
        return GT_FAIL;
    }
	#ifndef __AX_PLATFORM__
	if((0 == tidArr[0].tid)&&(0 == tidArr[0].intVecNum))
	{
		tidArr[0].intVecNum = intVec;
		tidArr[0].tid 		= tid;
	}
	else if((0 == tidArr[1].tid)&&(0 == tidArr[1].intVecNum))
	{
		tidArr[1].intVecNum = intVec;
		tidArr[1].tid 		= tid;
	}
	else
	{
        fprintf(stderr, "extDrvIntConnect: no task id array left\n");
        return GT_FAIL;
	}
	#endif
    return  GT_OK;
}


/*******************************************************************************
* extDrvSetIntLockUnlock
*
* DESCRIPTION:
*       Lock/unlock interrupts
*
* INPUTS:
*       mode   - interrupt state lock/unlock
*       key    - if mode is INTR_MODE_UNLOCK, lock key returned by
*                preceding interrupt disable call
*
* OUTPUTS:
*       key    - if mode is INTR_MODE_LOCK lock key for the interrupt
*                level
*
* RETURNS:
*       Lock key for the interrupt level
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_32 extDrvSetIntLockUnlock
(
    IN      INTERRUPT_MODE mode,
    INOUT   GT_32          *key
)
{
    return GT_OK;
}


