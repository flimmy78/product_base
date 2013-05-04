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
* genDrvHsuDrv.c
*
* DESCRIPTION:
*       Includes HSU function wrappers, for HSU functionality
*
* DEPENDENCIES:
*       -   BSP.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

#include <gtExtDrv/drivers/gtDragoniteDrv.h>
#include <gtExtDrv/drivers/pssBspApis.h>

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <errno.h>
#include <sys/mman.h>

#include <sys/ioctl.h>
#include "kerneldrv/include/presteraGlob.h"
extern GT_32 gtPpFd;                /* pp file descriptor           */

#ifdef GDA8548_DRV
static GT_U32 hsuBaseAddr = 0;
#endif

/*******************************************************************************
* extDrvHsuMemBaseAddrGet
*
* DESCRIPTION:
*       Get start address of HSU area
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       hsuPtr      - Pointer to beginning of HSU area
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS extDrvHsuMemBaseAddrGet
(
    OUT  GT_U32  *hsuPtr
)
{
#ifdef GDA8548_DRV
  if (hsuBaseAddr == 0)
  {
    hsuBaseAddr = (GT_U32) mmap((void*)PRV_CPSS_EXT_DRV_HSU_BASE_ADDR_CNS,
                                (8 * _1M),
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED | MAP_FIXED,
                                gtPpFd,
                                0);
    
    if (MAP_FAILED == (void*)hsuBaseAddr)
    {
      cpssOsPrintSync("mmap failed for HSU\n");
      return GT_FAIL;
    }
  } 
  
  *hsuPtr = PRV_CPSS_EXT_DRV_HSU_BASE_ADDR_CNS;
#endif
  
  return GT_OK;
}

/*******************************************************************************
* extDrvHsuWarmRestart
*
* DESCRIPTION:
*       Performs warm restart of the 8548 cpu
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       No return
*
* COMMENTS:
*       None
*
*******************************************************************************/

GT_VOID extDrvHsuWarmRestart
(
    GT_VOID
)
{
#ifdef GDA8548_DRV
	if (ioctl(gtPpFd, PRESTERA_IOC_HSUWARMRESTART) < 0)
    fprintf(stderr, "Fail to perform Hsu warm restart\n");
#endif
}

