#ifdef XCAT_DRV
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
* gXcatDragoniteDrv.c
*
* DESCRIPTION:
*       Includes DRAGONITE functions wrappers, for DRAGONITE manage
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

/*******************************************************************************
* extDrvDragoniteShMemBaseAddrGet
*
* DESCRIPTION:
*       Get start address of communication structure in DTCM
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       dtcmPtr      - Pointer to beginning of DTCM, where
*                      communication structures must be placed
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS extDrvDragoniteShMemBaseAddrGet
(
    OUT  GT_U32  *dtcmPtr
)
{
	*dtcmPtr = PRV_CPSS_EXT_DRV_XCAT_DRAGONITE_BASE_ADDR_CNS;

	return GT_OK;
}

/*******************************************************************************
* extDrvDragoniteSWDownload
*
* DESCRIPTION:
*       Download FW to instruction shared memory
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       For debug purposes only
*
*******************************************************************************/
GT_STATUS extDrvDragoniteSWDownload
(
    GT_VOID
)
{
	FILE      *fp;
	char config_file_name[128] = "/etc/drigonite_fw.bin";
	GT_U8 *binArray = NULL;
	GT_DragoniteSwDownload_STC downloadParams;
	GT_U32    size;       /* size of firmware to download */

	if ( (fp = fopen( config_file_name, "rb")) == NULL)
	{
		cpssOsPrintSync("1)appDemoBspDragoniteSWDownload:can't open %s - %s\n", 
						config_file_name, strerror(errno));
		return GT_FALSE;
	}

	binArray = cpssOsMalloc(_64K);
	if(binArray == NULL)
	{
		cpssOsPrintSync("2)appDemoBspDragoniteSWDownload:failed to allocate binArray\n");
		return GT_OUT_OF_CPU_MEM;
	}

	for(size = 0; feof(fp) == 0; size++)
	{
		/* read binary file by bytes */
		fread(binArray+size, 1, 1, fp);
		if(ferror(fp) != 0)
			cpssOsPrintSync("3)appDemoBspDragoniteSWDownload:ferror=%d\n", ferror(fp));
	}

	fclose(fp);

	downloadParams.buffer = (GT_VOID*)binArray;
	downloadParams.length = size-1;

/*	cpssOsPrintf("4)appDemoBspDragoniteSWDownload:sourcePtr=0x%x,size=%d\n", 
					downloadParams.buffer, downloadParams.length);
*/
	if (ioctl(gtPpFd, PRESTERA_IOC_DRAGONITESWDOWNLOAD, &downloadParams) < 0)
	{
		cpssOsPrintSync("5)bspDragoniteSWDownload fail\n");
		cpssOsFree(binArray);
		return GT_FAIL;
	}

	cpssOsFree(binArray);

	return GT_OK;
}


/*******************************************************************************
* extDrvDragoniteGetIntVec
*
* DESCRIPTION:
*       Get interrupt line number of Dragonite
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       intVec - interrupt line number
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       For debug purposes only
*
*******************************************************************************/
GT_STATUS extDrvDragoniteGetIntVec
(
    OUT GT_U32 *intVec
)
{
	if (ioctl(gtPpFd, PRESTERA_IOC_DRAGONITEGETINTVEC, intVec) < 0)
	{
		return GT_FAIL;
	}

	return GT_OK;
}

/*******************************************************************************
* extDrvDragoniteInit
*
* DESCRIPTION:
*       Get interrupt line number of Dragonite
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       For debug purposes only
*
*******************************************************************************/
GT_STATUS extDrvDragoniteInit
(
    GT_VOID
)
{	
	GT_U32 baseAddr;

	/* map Dragonite DTCM space */
	/* in current implementation always just one dragonite is active */
	baseAddr = (GT_U32) mmap((void*)PRV_CPSS_EXT_DRV_XCAT_DRAGONITE_BASE_ADDR_CNS, 
							_32K,
							PROT_READ | PROT_WRITE,
							MAP_SHARED | MAP_FIXED,
							gtPpFd,
							0);

	if (MAP_FAILED == (void*)baseAddr)
	{
		cpssOsPrintSync("mmap fail\n");
		return GT_FAIL;
	}

	if (ioctl(gtPpFd, PRESTERA_IOC_DRAGONITEINIT) < 0)
		return GT_FAIL;

	return GT_OK;
}

/*******************************************************************************
* extDrvDragoniteEnableSet
*
* DESCRIPTION:
*       Get interrupt line number of Dragonite
*
* INPUTS:
* 		enable - GT_TRUE - release Dragonite from reset
* 				 GT_FALSE - reset Dragonite
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       For debug purposes only
*
*******************************************************************************/
GT_STATUS extDrvDragoniteEnableSet
(
    IN  GT_BOOL enable
)
{
	if (ioctl(gtPpFd, PRESTERA_IOC_DRAGONITEENABLE, enable) < 0)
		return GT_FAIL;

	return GT_OK;
}

/*******************************************************************************
* extDrvDragoniteFwCrcCheck
*
* DESCRIPTION:
*       This routine executes Dragonite firmware checksum test
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       Mostly for debug purposes, when FW download executed by CPSS CRC check 
*       engaged automatically
*
*******************************************************************************/
GT_STATUS extDrvDragoniteFwCrcCheck
(
    GT_VOID
)
{	
	if (ioctl(gtPpFd, PRESTERA_IOC_DRAGONITEITCMCRCCHECK) < 0)
		return GT_FAIL;

	return GT_OK;
}
#endif /* XCAT_DRV */

