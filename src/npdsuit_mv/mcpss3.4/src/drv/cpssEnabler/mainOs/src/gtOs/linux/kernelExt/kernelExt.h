/*******************************************************************************
*              Copyright 2010, Marvell Israel, LTD.
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
* kernelExt.h
*
* DESCRIPTION:
*       defines the interface to kernel module
*
* DEPENDENCIES:
*       None
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef _kernelExt_h
#define _kernelExt_h

#include <sys/ioctl.h>
#ifdef  MVKERNELEXT_SYSCALLS
#   include <linux/unistd.h>
#endif

#include "mv_KernelExt.h"

extern int mvKernelExtFd;

#ifdef  MVKERNELEXT_SYSCALLS

#ifdef _syscall0
# warning "*** _syscall0 defined"
#else
# warning "*** _syscall0 NOT defined"
#endif

#   if defined(_syscall0)
        
        int mv_noop(void);
        int mv_ctl(unsigned int cmd, unsigned long arg);
#       define mv_ctrl(cmd,arg)  mv_ctl((cmd),(unsigned long)(arg))
        int mv_tasklock(void);
        int mv_taskunlock(void);
        int mv_taskunlockforce(void);
        int mv_sem_give(int semid);
        int mv_sem_take(int semid);
        int mv_sem_trytake(int semid);
        int mv_sem_take_tmo(int semid, unsigned int timeout);

#   else /* !defined(_syscall0) */

#       include <unistd.h>
#       include <sys/syscall.h>
#       define mv_noop()                syscall(__NR_mv_noop)
#       define mv_ctrl(cmd,arg)         syscall(__NR_mv_ctl, (cmd), (arg))
#       define mv_tasklock()            syscall(__NR_mv_tasklock)
#       define mv_taskunlock()          syscall(__NR_mv_taskunlock)
#       define mv_taskunlockforce()     syscall(__NR_mv_taskunlockforce)
#       define mv_sem_give(semid)       syscall(__NR_mv_sem_give, semid)
#       define mv_sem_take(semid)       syscall(__NR_mv_sem_take, semid)
#       define mv_sem_trytake(semid)    syscall(__NR_mv_sem_trytake, semid)
#       define mv_sem_take_tmo(semid,timeout) syscall(__NR_mv_sem_take_tmo, semid, timeout)
#   endif /* KERNEL_SYSCALLS_INDIRECT */



#else /* !defined MVKERNELEXT_SYSCALLS */
#   include <sys/ioctl.h>
#   define mv_noop()                ioctl(mvKernelExtFd, MVKERNELEXT_IOC_NOOP)
#   define mv_ctrl(cmd,arg)         ioctl(mvKernelExtFd, (cmd), (arg))
#   define mv_tasklock()            ioctl(mvKernelExtFd, MVKERNELEXT_IOC_TASKLOCK)
#   define mv_taskunlock()          ioctl(mvKernelExtFd, MVKERNELEXT_IOC_TASKUNLOCK)
#   define mv_taskunlockforce()     ioctl(mvKernelExtFd, MVKERNELEXT_IOC_TASKUNLOCKFORCE)
#   define mv_sem_give(semid)       ioctl(mvKernelExtFd, MVKERNELEXT_IOC_SEMGIVE, semid)
#   define mv_sem_take(semid)       ioctl(mvKernelExtFd, MVKERNELEXT_IOC_SEMTAKE, semid)
#   define mv_sem_trytake(semid)    ioctl(mvKernelExtFd, MVKERNELEXT_IOC_SEMTRYTAKE, semid)
#   define mv_sem_take_tmo(semid,timeout) { \
        mv_sem_timedwait_stc params; \
        params.semid = semid; \
        params.timeout = max_wait; \
        ioctl(mvKernelExtFd, MVKERNELEXT_IOC_SEMTAKETMO, &params); \
    }
#endif /* !defined MVKERNELEXT_SYSCALLS */


#endif /* _kernelExt_h */
