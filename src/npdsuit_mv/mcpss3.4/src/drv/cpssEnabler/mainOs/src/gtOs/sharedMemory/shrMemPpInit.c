/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* shrMemPpInit.c
*
* DESCRIPTION:
*       File contains routines for Packet Processor initialization
*       (applicable ONLY for BM).
*       Also routine for sections mapping registry is implemented here.
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.2 $
*
*******************************************************************************/

/************* Includes *******************************************************/
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <gtOs/gtOsIo.h>
#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsGen.h>
#include <gtOs/gtOsSharedMemory.h>
#include <gtOs/gtOsSharedMemoryRemapper.h>
#include <gtOs/gtOsSharedMalloc.h>
#include <gtOs/gtOsSharedPp.h>
#include <sysdef/npd_sysdef.h>

/* This part is relevant ONLY for BlackMode */
#define PRV_SHARED_DMA_VIRT_ADDR_MAC    ASIC_DMA_VIRT_ADDR

GT_32 gtPpFd;
extern GT_U32 baseAddr;
extern GT_U32 dmaSize;
extern SHARED_MEM_STC cdma_mem;
static char *dma_name = "DMA";

/***** External Functions **********************************************/
GT_STATUS extDrvGetDmaBase(OUT GT_U32 * dmaBase);
GT_STATUS extDrvGetDmaSize(OUT GT_U32 * dmaSize);

#ifdef SHARED_MEMORY
/* Registry of mmaped sections (located into shared memory) */
extern PP_MMAP_INFO_STC pp_mmap_sections_registry[PP_REGISTRY_SIZE_CNS];
extern int pp_mmap_registry_index;

/************* Functions ******************************************************/
/************* Public Functions ***********************************************/

/*******************************************************************************
* shrMemRegisterPpMmapSection
*
* DESCRIPTION:
*       Register section which should be mmaped by all non-first clients.
*
* INPUTS:
*       startPtr - start address to be mapped
*       length - length of block to be mapped
*       offset - offset into file to be mapped
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success
*       GT_OUT_OF_RANGE - too many sections for static registry,
*         must be increased 
*
* COMMENTS:
*       Operation is relevant only for SHARED_MEMORY=1
*       Also operation is relevant only for undefined ASIC_SIMULATION (BlackMode).
*
*******************************************************************************/
GT_STATUS shrMemRegisterPpMmapSection
(
    IN GT_VOID *startPtr,
    IN size_t  length,
    IN off_t   offset
)
{
    PP_MMAP_INFO_STC *infoPtr;
    if(pp_mmap_registry_index == PP_REGISTRY_SIZE_CNS)
    {
        fprintf(stderr, "shrMemRegisterPpMmapSection: index out of space, please increase PP_REGISTRY_SIZE_CNS\n");
        return GT_OUT_OF_RANGE;
    }
    infoPtr = &pp_mmap_sections_registry[pp_mmap_registry_index++];

    /* Store required variables into registry */
    infoPtr->startPtr  = startPtr;
    infoPtr->length    = length;
    infoPtr->offset    = offset;

    return GT_OK;
}
#endif /* SHARED_MEMORY */

/*******************************************************************************
* shrMemSharedPpInit
*
* DESCRIPTION:
*       Performs sharing operations for DMA special device file.
*
* INPUTS:
*       isFirstClient - the-first-client flag 
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       Special device file /dev/mvPP is handled by presera kernel module.
*       mmap operation with fixed addresses will be redirected to prestera_mmap
*       to do special operations with DMA, register and config space.
*
*       Operation is relevant only for BlackMode (undefined ASIC_SIMULATION).
*
*******************************************************************************/
GT_STATUS shrMemSharedPpInit(IN GT_BOOL isFirstClient)
{
    GT_STATUS retVal;

    /* Open special device file.
     * It is handled by prestera kernel module. */
    gtPpFd = open("/dev/mvPP", O_RDWR);
    if (gtPpFd < 0)
    {
        fprintf(stderr, "Cannot open /dev/mvPP for Read-Write, error=%d\n", errno);
        return GT_FAIL;
    }
#if 0
    return GT_OK;/*xcat debug 20110111*/
#endif
    fprintf(stderr, "\n\t\t*********** in shrMemSharedPpInit() init DMA !!! zhangdi\n");

    if(isFirstClient)
    {
        /* Map shared buffer and init management structure. */
        extDrvGetDmaSize(&dmaSize);
        retVal = shrMemSharedBufferInit(dma_name, PRV_SHARED_DMA_VIRT_ADDR_MAC,
                                        dmaSize, gtPpFd,
                                        GT_FALSE, &cdma_mem);
        if (GT_OK != retVal)
            return GT_NO_RESOURCE;

#ifdef SHARED_MEMORY
        /* Register this mmaped section into local registry.
         * Non-first client HAVE to use this remarks to restore all mappings
         * into own address space. */
        retVal = shrMemRegisterPpMmapSection((GT_VOID*)PRV_SHARED_DMA_VIRT_ADDR_MAC,
                                             dmaSize,
                                             0);
        if (GT_OK != retVal) return retVal;
#endif
        #if 0 /* zhangdi cancel  */
        /* Get physical address of the dma area need for Virt2Phys and Phys2Virt */    
        extDrvGetDmaBase(&baseAddr);	
        #endif
	}
    else
    {
#ifdef SHARED_MEMORY
        int i=0;
        /* All other clients just map sections from registry */
        while(i<pp_mmap_registry_index)
        {
            /* Map registry */
            retVal = shrMemMmapMemory((unsigned long)pp_mmap_sections_registry[i].startPtr,
                                      pp_mmap_sections_registry[i].length, 
                                      pp_mmap_sections_registry[i].offset,
                                      gtPpFd);
            if (GT_OK != retVal)
                return retVal;

            i++;
        }
#else
        /* Whithout shared memory approach only SINGLE client is accessible.
         * So it should be the only first one.*/
        return GT_BAD_VALUE;
#endif
    }

    return GT_OK;
}

