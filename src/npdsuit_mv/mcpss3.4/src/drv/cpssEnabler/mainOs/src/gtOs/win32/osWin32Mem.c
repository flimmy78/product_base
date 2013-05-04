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
* osWin32Mem.c
*
* DESCRIPTION:
*       Win32 Operating System Simulation. Memory manipulation facility.
*
* DEPENDENCIES:
*       Win32, CPU independed.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsGen.h>
#include <gtOs/gtOsSem.h>

/* include simulation H files for shared memory issues */
/* next macro defined so the file of <os/simOsProcess.h> will not #error us */
#define EXPLICIT_INCLUDE_TO_SIM_OS_H_FILES
#include <os/simTypesBind.h>
#include <os/simOsProcess.h>
#include <asicSimulation/SDistributed/sdistributed.h>
#include <asicSimulation/SInit/sinit.h>

/***** External Functions **********************************************/
extern int osPrintf(const char* format, ...);
void *osMemPoolsAlloc(GT_U32 size);
void *osMemPoolsRealloc(void *ptr, GT_U32 size);
void osMemPoolFree(void *memBlock);
void *osStaticMallocFromChunk(GT_U32 size);

/***** Static functions ************************************************/

/***** Static variables ************************************************/
/* heap allocation measurement static variables */

GT_BOOL  firstInitialization = GT_TRUE;

/***** Public Functions ************************************************/
/* a number that indicate the 'key' representing the shared memory */
#define SIM_DMA_AS_SHARED_MEM_KEY_CNS   5678

/* id of the shared memory */
static GT_SH_MEM_ID simDmaAsSharedMemId = 0;

static GT_MUTEX simDmaMutexId;
/* pointer to the start of the shared memory */
static void* simDmaAsSharedMemPtr=NULL;

/* size of the shared memory */
static GT_U32 simDmaAsSharedMemSize = 0;

/* offset ued from start of the shared memory  */
static GT_U32 simDmaAsSharedMemOffsetUsed = 0;

/*******************************************************************************
* dmaAsSharedMemoryMalloc
*
* DESCRIPTION:
*       Allocates shared memory block of specified size, this shared memory will
*       be used for 'DMA memory'
*
* INPUTS:
*       size - bytes to allocate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       pointer to the shared memory (on NULL on fail)
*
* COMMENTS:
*       Usage of this function is only on FIRST initialization.
*       used only when distributed simulation of application side is working
*       with broker , in mode of DMA as shared memory(shared between the
*       broker and the application).
*
*******************************************************************************/
static void* dmaAsSharedMemoryMalloc
(
    IN GT_U32 size
)
{
    GT_STATUS   rc;

    rc = simOsSharedMemGet((GT_SH_MEM_KEY)SIM_DMA_AS_SHARED_MEM_KEY_CNS,size,0,&simDmaAsSharedMemId);
    if(rc != GT_OK)
    {
        return NULL;
    }

    /* Now we attach (map) the shared memory to our process memory */
    simDmaAsSharedMemPtr = simOsSharedMemAttach(simDmaAsSharedMemId,
            NULL ,/* NULL means we attach it to no specific memory , we let OS to decide */
            0);/*no specific flags*/

    if(simDmaAsSharedMemPtr)
    {
        simDmaAsSharedMemSize = size;
    }

    return simDmaAsSharedMemPtr;
}

/*******************************************************************************
* osMemInit
*
* DESCRIPTION:
*       Initialize the memory management.
*
* INPUTS:
*       size            - memory size to manage.
*       reallocOnDemand - defines whether in the case of memory chunk depleation
*                         a new chunk will be allocated or an out of memory will
*                         be reported.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_NO_RESOURCE - not enough memory in the system.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osMemInit
(
    IN GT_U32 size,
    IN GT_BOOL reallocOnDemand
)
{
    GT_MEMORY_MANAGE_METHOD_ENT memAllocMethod;
    GT_STATUS ret;

    if (reallocOnDemand == GT_TRUE)
    {
        memAllocMethod = GT_MEMORY_MANAGE_METHOD_REALLOC_ON_DEMAND_E;
    }
    else
    {
        memAllocMethod = GT_MEMORY_MANAGE_METHOD_NO_REALLOC_ON_DEMAND_E;
    }

    /* Create the simDma semaphore */
    if (osMutexCreate("osSimDma", &simDmaMutexId) != GT_OK)
    {
        return GT_FAIL;
    }
    if(sasicgSimulationRole == SASICG_SIMULATION_ROLE_DISTRIBUTED_APPLICATION_SIDE_VIA_BROKER_E)
    {
        /* the application side that work with broker may decide how the broker
           will handle DMA read/write requests:
            1. use shared memory and do read/write by itself.
            2. use socket to send to application the request from read/write DMA
        */

        if(brokerDmaMode == BROKER_NOT_USE_SOCKET)
        {
            /* steal 1/2 of memory for DMA */
            if(NULL == dmaAsSharedMemoryMalloc(size/2))
            {
                return GT_NO_RESOURCE;
            }

            /* send DMA registration info */
            simDistributedRegisterDma((GT_U32)simDmaAsSharedMemPtr,/* the address of the pointer in our memory */
                    simDmaAsSharedMemSize,/* the size of shared memory */
                    SIM_DMA_AS_SHARED_MEM_KEY_CNS,/* the key to retrieve the shared memory */
                    BROKER_DMA_MODE_SHARED_MEMORY);

            /* use other 1/2 of memory for non-DMA */
            size = size / 2;
        }
        else if(brokerDmaMode == BROKER_USE_SOCKET)
        {
            /* send DMA registration info -- we work in socket mode */
            simDistributedRegisterDma(0,/* don't care */
                                      0,/* don't care */
                                      0,/* don't care */
                                      BROKER_DMA_MODE_SOCKET);
        }
        else
        {
            return GT_BAD_STATE;
        }
    }

    ret = osMemLibInit(malloc,free,size, memAllocMethod);
    firstInitialization = GT_FALSE;
    return ret;
}

/*******************************************************************************
* osStaticAlloc
*
* DESCRIPTION:
*       Allocates memory block of specified size, this memory will not be free.
*
* INPUTS:
*       size - bytes to allocate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Void pointer to the allocated space, or NULL if there is
*       insufficient memory available or calling after first init.
*
* COMMENTS:
*       Usage of this function is only on FIRST initialization.
*
*******************************************************************************/
void *osStaticMalloc
(
    IN GT_U32 size
)
{
    if (firstInitialization == GT_TRUE)
    {
        return osMalloc(size);
    }
    return osStaticMallocFromChunk(size);
}

/*******************************************************************************
* osMalloc
*
* DESCRIPTION:
*       Allocates memory block of specified size.
*
* INPUTS:
*       size - bytes to allocate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Void pointer to the allocated space, or NULL if there is
*       insufficient memory available
*
* COMMENTS:
*       None
*
*******************************************************************************/
void *osMalloc
(
    IN GT_U32 size
)
{
    GT_U32 *magicPtr = NULL;
    GT_U32 i;        /* loop index */
    GT_U32 numWords; /* nuimber of words in allocated memory */
    GT_U32 randHihg, randLow; /* High and low parts of random value */


    if( size == 0) return NULL;

    if (firstInitialization == GT_TRUE)
    {
        /* during first init allocate from the OS */
        magicPtr = (GT_U32*)malloc(size+sizeof(GT_U32));
        if (magicPtr == NULL)
        {
            return NULL;
        }
        *magicPtr = (GT_U32)magicPtr;
        magicPtr++;
    }
    else
    {
        magicPtr = (GT_U32*)osMemPoolsAlloc(size);
        if (magicPtr == NULL)
        {
            return NULL;
        }
    }

    /* fill random values only whole words */
    numWords = (size/ sizeof(GT_U32));
    for (i = 0; (i < numWords); i++)
    {
        /* write random values to the allocated memory */
        /* the rand() function generates values in     */
        /* range 0 - 0x7FFF                            */
        randHihg = rand();
        randLow  = rand();
        magicPtr[i] = ((randHihg << 16) | randLow);
    }

    return (void*)magicPtr;
}

/*******************************************************************************
* osRealloc
*
* DESCRIPTION:
*       Reallocate memory block of specified size.
*
* INPUTS:
*       ptr  - pointer to previously allocated buffer
*       size - bytes to allocate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Void pointer to the allocated space, or NULL if there is
*       insufficient memory available
*
* COMMENTS:
*       None
*
*******************************************************************************/
void *osRealloc
(
    IN void * ptr ,
    IN GT_U32 size
)
{
    GT_U32 *magicPtr = NULL;
    
    if (ptr == NULL)
    {
        return osMalloc(size);
    }

    if (firstInitialization == GT_TRUE)
    {
        magicPtr = realloc((void*)(((GT_U32)ptr)-sizeof(GT_U32)), size+sizeof(GT_U32));
        if (magicPtr == NULL)
        {
            return NULL;
        }
        *magicPtr = (GT_U32)magicPtr;
        magicPtr++;
        return (void*)magicPtr;
    }
    else
    {
        return osMemPoolsRealloc(ptr, size);
    }
}

/*******************************************************************************
* osFree
*
* DESCRIPTION:
*       Deallocates or frees a specified memory block.
*
* INPUTS:
*       memblock - previously allocated memory block to be freed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
void osFree
(
    IN void* const memblock
)
{
    GT_U32 *magicPtr = NULL;
    assert(memblock != NULL);
    if (firstInitialization == GT_TRUE)
    {
        free((void*)((GT_U32)memblock-sizeof(GT_U32)));
    }
    else
    {
        magicPtr = (void*)((GT_U32)memblock-sizeof(GT_U32));
        /* check if this free is trying to free something that wasn't
           allocated by owr pools manager */
        if (*magicPtr == (GT_U32)magicPtr)
        {
#ifdef OSMEMPOOLS_DEBUG
            osPrintf("Warning:Trying to Free a pointer allocated by malloc after osMemInit !\n");
#endif /*OSMEMPOOLS_DEBUG*/
            free((void*)((GT_U32)memblock-sizeof(GT_U32)));
        }
        else
        {
            osMemPoolFree(memblock);
        }
    }
    return;
}

/*******************************************************************************
* osCacheDmaMalloc
*
* DESCRIPTION:
*       Allocate a cache-safe buffer  of specified size for DMA devices
*       and drivers
*
* INPUTS:
*       size - bytes to allocate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       A pointer to the cache-safe buffer, or NULL
*
* COMMENTS:
*       None
*
*******************************************************************************/
void *osCacheDmaMalloc
(
    IN GT_U32 size
)
{
    if(simDmaAsSharedMemSize)
    {
        char* tmpPtr; /* tmp pointer to start of memory to return */
        /* take sem */
        osMutexLock(simDmaMutexId);

        /* check if we have enough space for the new memory request */
        if((simDmaAsSharedMemOffsetUsed + size) > simDmaAsSharedMemSize)
        {
            /* free sem */
            osMutexUnlock(simDmaMutexId);
            return NULL;
        }

        tmpPtr = (char*)simDmaAsSharedMemPtr + simDmaAsSharedMemOffsetUsed;

        simDmaAsSharedMemOffsetUsed += size;

        /* free sem */
        osMutexUnlock(simDmaMutexId);

        return tmpPtr;
    }

    return malloc(size); /*TODO: os_mem_chunks_array.memAllocFuncPtr(size); */
}

/*******************************************************************************
* osCacheDmaFree
*
* DESCRIPTION:
*       Free the buffer acquired with osCacheDmaMalloc()
*
* INPUTS:
*       ptr - pointer to malloc/free buffer
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
GT_STATUS osCacheDmaFree
(
    IN void *ptr
)
{
    if(simDmaAsSharedMemSize)
    {
        /* not supporting free of this memory */
        return GT_OK;
    }


    free (ptr);
    return GT_OK;
}

/*******************************************************************************
* osPhy2Virt
*
* DESCRIPTION:
*       Converts physical address to virtual.
*
* INPUTS:
*       phyAddr  - physical address
*
* OUTPUTS:
*       virtAddr - virtual address
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS osPhy2Virt
(
    IN  GT_U32 phyAddr,
    OUT GT_U32 *virtAddr
)
{
    *virtAddr = phyAddr;
    return GT_OK;
}

/*******************************************************************************
* osVirt2Phy
*
* DESCRIPTION:
*       Converts virtual address to physical.
*
* INPUTS:
*       virtAddr - virtual address
*
* OUTPUTS:
*       phyAddr  - physical address
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS osVirt2Phy
(
    IN  GT_U32 virtAddr,
    OUT GT_U32 *phyAddr
)
{
    *phyAddr = virtAddr;
    return GT_OK;
}
