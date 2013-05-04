/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* shrMemSharedMalloc.c
*
* DESCRIPTION:
*       This file contains realization for functions which allocate memory
*       for run-time variables in shared memory. 
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

/************* Includes *******************************************************/
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <gtOs/gtOsIo.h>
#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsSharedMalloc.h>

#define PRV_SHARED_MALLOC_DATA_CNS  "/dev/shm/CPSS_SHM_MALLOC"
#define PRV_SHARED_MALLOC_VIRT_ADDR_MAC 0x51000000

/************* Locals (static - local to c file) *******************************/
/* Declare pointer to shared memory descriptor structure
    inside shared memory. shrMemSharedInit should accept
    this trick! */
static SHARED_MEM_STC * const malloc_mem =
    (SHARED_MEM_STC*)PRV_SHARED_MALLOC_VIRT_ADDR_MAC;
/* Descriptors cannot be shared, so place private descriptor
    here (will be build into libhelper.so - private lib) */
static GT_U32 shared_malloc_fd;
static char *dyn_name = "DYNAMIC";

/************* Functions ******************************************************/
/************* Public Functions ***********************************************/

/*******************************************************************************
* shrMemInitSharedMallocInit
*
* DESCRIPTION:
*       Initialize shared memory buffer for shared malloc call.
*
* INPUTS:
*       isFirstClient - specify if client is the first client (CPSS enabler)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - Operation succeeded
*       GT_FAIL         - Operation failed
*       GT_NO_RESOURCE  - Operaton failed. No resource.
*
* COMMENTS:
*       Inits shared_malloc_fd descriptor (private for each process).
*
*******************************************************************************/
GT_STATUS shrMemSharedMallocInit(IN GT_BOOL isFirstClient)
{
    GT_STATUS retVal;

    if(isFirstClient)
    {
        /* Create shared buffer file for shared malloc */
        shared_malloc_fd = open(PRV_SHARED_MALLOC_DATA_CNS, O_RDWR|O_CREAT|O_TRUNC, 0777);
        /* (GT_U32) shm_open("/PSSMDYNAMIC", flags, 0777); */
        if(shared_malloc_fd == -1)
            return GT_FAIL;

        /* First client init shared buffer */
        retVal = shrMemSharedBufferInit(dyn_name, PRV_SHARED_MALLOC_VIRT_ADDR_MAC,
            OSMEM_MAX_MEMORY_SIZE_USE, shared_malloc_fd, GT_TRUE, malloc_mem);

        if (GT_OK != retVal)
            return GT_NO_RESOURCE;
    }
    else
    {
        /* Open shared buffer file for shared malloc */
        shared_malloc_fd = open(PRV_SHARED_MALLOC_DATA_CNS, O_RDWR, 0777);
        if (-1 == shared_malloc_fd)
        {
            if(errno == ENOENT) 
            {
                fprintf(stderr, "open() : file %s not exist, run Enabler first\n", 
                    PRV_SHARED_MALLOC_DATA_CNS);
            }
            return GT_FAIL;
        }

        /* All non-first clients just map shared buffer */
        retVal = shrMemMmapMemory
        (
            PRV_SHARED_MALLOC_VIRT_ADDR_MAC,
            OSMEM_MAX_MEMORY_SIZE_USE,
            0,
            shared_malloc_fd
        );

        if (GT_OK != retVal)
            return GT_FAIL;
    }

    return GT_OK;
}

/*******************************************************************************
* shrMemSharedMalloc
*
* DESCRIPTION:
*       Allocates memory into shared buffer for shared malloc.
*       Size of required block is specified as parameter.
*
* INPUTS:
*       size    - size of memory which needs to be allocated
*
* OUTPUTS:
*       None
*
* RETURNS:
*       void*   - Operation succeeded. Adress of allocated memory
*       NULL    - Operaton failed
*
* COMMENTS:
*       Uses static pointer malloc_mem to access shared structure.
*       Uses static variable shared_malloc_fd to extend shared file.
*
*******************************************************************************/
GT_VOID *shrMemSharedMalloc(IN unsigned int size)
{
    return shrMemSharedMallocIntoBlock(malloc_mem, shared_malloc_fd, size);
}

