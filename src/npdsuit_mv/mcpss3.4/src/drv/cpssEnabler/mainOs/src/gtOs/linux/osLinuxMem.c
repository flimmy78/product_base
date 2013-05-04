/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* osLinuxMem.c
*
* DESCRIPTION:
*       Linux Operating System wrapper. Memory manipulation facility.
*
* DEPENDENCIES:
*       Linux, CPU independed.
*
* FILE REVISION NUMBER:
*       $Revision: 1.2 $
*******************************************************************************/
/***** Includes ********************************************************/
#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsGen.h>
#include <gtOs/gtOsIo.h>
#include <gtOs/gtOsSharedMalloc.h>
#include <sysdef/npd_sysdef.h>

#if __WORDSIZE == 64
# include <sys/mman.h>
#endif

#ifndef LINUX_SIM
/*#include <private/gtLinux.h>*/
 #define PRV_SHARED_PPS_VIRT_ADDR_MAC    ASIC_DMA_VIRT_ADDR
#endif

#ifdef LINUX_SIM
#define EXPLICIT_INCLUDE_TO_SIM_OS_H_FILES
#include <os/simTypesBind.h>
#include <os/simOsProcess.h>

#include <asicSimulation/SDistributed/sdistributed.h>
#include <asicSimulation/SInit/sinit.h>
#endif

/* add for appDemoPpConfigList[] */
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>

/***** Function Declaration ********************************************/
#ifdef LINUX_SIM
static GT_VOID* dmaAsSharedMemoryMalloc(IN GT_U32 size);
#endif

/***** External Functions **********************************************/
GT_VOID *osMemPoolsAlloc(GT_U32 size);
GT_VOID *osMemPoolsRealloc(GT_VOID *ptr, GT_U32 size);
GT_VOID osMemPoolFree(GT_VOID *memBlock);
GT_VOID *osStaticMallocFromChunk(GT_U32 size);

/***** Global Vars ********************************************/

GT_BOOL  firstInitialization = GT_TRUE;

#ifndef LINUX_SIM
    extern GT_32 gtPpFd;
#endif

#ifdef LINUX_SIM
/* a number that indicate the 'key' representing the shared memory */
#define SIM_DMA_AS_SHARED_MEM_KEY_CNS   5678

/* id of the shared memory */
static GT_SH_MEM_ID simDmaAsSharedMemId = 0;

static GT_MUTEX simDmaMutexId;
/* pointer to the start of the shared memory */
static GT_VOID* simDmaAsSharedMemPtr=NULL;

/* size of the shared memory */
static GT_U32 simDmaAsSharedMemSize = 0;

/* offset ued from start of the shared memory  */
static GT_U32 simDmaAsSharedMemOffsetUsed = 0;
#endif /* LINUX_SIM */

#ifdef SHARED_MEMORY
extern SHARED_MEM_STC cdma_mem;
extern GT_U32 baseAddr;
extern GT_U32 dmaSize;
#else
SHARED_MEM_STC cdma_mem;
GT_U32 baseAddr = 0;
//GT_U32 dmaSize = 2 * (1024 * 1024); /*default value*/
GT_U32 dmaSize = 4 * (1024 * 1024); /*default value*/
#endif


#ifndef __AX_PLATFORM__
extern unsigned int auteware_product_id;
#endif

#if 0
static SHARED_MEM_STC cdma_mem = {0,0,0,0};
static char * dma_name = "DMA";
#endif
#ifndef __AX_PLATFORM__
#define AX7_MVPP_NUMBER	2
#define AX7_MVPP_DMA_BASEADDR 0x50000000
#define AX7_MVPP_DMA_PER_SIZE 4*1024*1024
#if 0   /*luoxun --cdma_mem conflict.*/
static SHARED_MEM_STC cdma_mem[] = { \
	{0,0,0,0},		\
	{0,0,0,0}		\
};
#endif
static char * dma_name_ax7[] = { \
	"98DX275DMA", 	\
	"98DX804DMA"  	\
};

#define AX5_MVPP_NUMBER 1
#define AX5_MVPP_DMA_BASEADDR AX7_MVPP_DMA_BASEADDR
#define AX5_MVPP_DMA_PER_SIZE AX7_MVPP_DMA_PER_SIZE

static char * dma_name_ax5[] = { \
	"98DX265DMA" 	\
};

#define AX5I_MVPP_NUMBER 1
#define AX5I_MVPP_DMA_BASEADDR AX7_MVPP_DMA_BASEADDR
#define AX5I_MVPP_DMA_PER_SIZE AX7_MVPP_DMA_PER_SIZE

static char * dma_name_ax5i[] = { \
	"98DX265DMA" 	\
};

#define AU4_MVPP_NUMBER 1
#define AU4_MVPP_DMA_BASEADDR AX7_MVPP_DMA_BASEADDR
#define AU4_MVPP_DMA_PER_SIZE AX7_MVPP_DMA_PER_SIZE

static char * dma_name_au4[] = { \
	"98DX265DMA" 	\
};

#define AU3_MVPP_NUMBER 1
#define AU3_MVPP_DMA_BASEADDR AU4_MVPP_DMA_BASEADDR
#define AU3_MVPP_DMA_PER_SIZE AU4_MVPP_DMA_PER_SIZE

static char * dma_name_au3[] = { \
	"98DX255DMA" 	\
};
#endif
#if 0 /*Sharing PSS Dynamic Data - Curentlly not used */
static SHARED_MEM_STC malloc_mem = {0,0,0,0};
static char * dyn_name = "DYNAMIC";
#endif


/***** Public Functions ************************************************/
#if __WORDSIZE == 64
#undef SHARED_MALLOC_MAC
#undef SHARED_FREE_MAC
/* workaround: allocate in first 2Gig address space */
static GT_VOID* malloc_32bit(unsigned int size)
{
    size_t *ptr;
    ptr = mmap(NULL, size + sizeof(size_t),
        PROT_READ | PROT_WRITE,
        MAP_32BIT | MAP_ANONYMOUS | MAP_PRIVATE,
        0, 0);
    if (ptr)
    {
        *ptr = size;
        ptr++;
    }
    return ptr;
}
static GT_VOID free_32bit(GT_VOID* ptr)
{
    size_t *sptr = (size_t*)ptr;
    if (sptr)
    {
        sptr--;
        munmap(sptr, *sptr + sizeof(size_t));
    }
}
static GT_VOID* realloc_32bit(GT_VOID* ptr, size_t newsize)
{
    size_t *sptr = (size_t*)ptr;
    GT_VOID   *newptr;
    if (ptr == NULL)
        return malloc_32bit(newsize);
    sptr--;
    if (newsize == *sptr)
    {
        return ptr;
    }
    newptr = malloc_32bit(newsize);
    memcpy(newptr, ptr, (*sptr < newsize) ? *sptr : newsize);
    free_32bit(ptr);
    return newptr;
}
#define SHARED_MALLOC_MAC malloc_32bit
#define SHARED_FREE_MAC free_32bit
#define realloc realloc_32bit
#endif

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

#ifdef LINUX_SIM
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
#endif
    ret = osMemLibInit(SHARED_MALLOC_MAC,SHARED_FREE_MAC,size, memAllocMethod);
    firstInitialization = GT_FALSE;
    return ret;
}

/*******************************************************************************
* osStaticMalloc
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
GT_VOID *osStaticMalloc
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
*       Usage of this function is NEVER during initialization.
*
*******************************************************************************/
GT_VOID *osMalloc
(
    IN GT_U32 size
)
{
    GT_UINTPTR *magicPtr = NULL;
    IS_WRAPPER_OPEN_PTR;

    if( size == 0) return NULL;

    if (firstInitialization == GT_TRUE)
    {
        magicPtr = (GT_UINTPTR*)SHARED_MALLOC_MAC(size+sizeof(GT_UINTPTR));
        if (magicPtr == NULL)
        {
            return NULL;
        }
        *magicPtr = (GT_UINTPTR)magicPtr;
        magicPtr++;
        return (GT_VOID*)magicPtr;
    }
    else
    {
        return osMemPoolsAlloc(size);
    }
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
*       Usage of this function is NEVER during initialization.
*       Recommended to avoid usage of this function.
*
*******************************************************************************/
GT_VOID *osRealloc
(
    IN GT_VOID * ptr ,
    IN GT_U32 size
)
{
    IS_WRAPPER_OPEN_PTR;

    if (ptr == NULL)
    {
        return osMalloc(size);
    }

    if (firstInitialization == GT_TRUE)
    {
#if defined(SHARED_MEMORY) || !defined(LINUX_SIM)
        GT_VOID *allocPtr = NULL;

        osPrintf("Warning: osRealloc before initialization\n");

        allocPtr = SHARED_MALLOC_MAC(size+sizeof(GT_UINTPTR));
        if (allocPtr == NULL)
        {
            return NULL;
        }
        osMemCpy(allocPtr,(GT_VOID*)((GT_UINTPTR)ptr-sizeof(GT_UINTPTR)),size+sizeof(GT_UINTPTR));
        return (GT_VOID*)((GT_UINTPTR)allocPtr + sizeof(GT_UINTPTR));
#else
        GT_UINTPTR *magicPtr =
            realloc((GT_VOID*)(((GT_UINTPTR)ptr)-sizeof(GT_UINTPTR)), size+sizeof(GT_UINTPTR));
        if (magicPtr == NULL)
        {
            return NULL;
        }
        *magicPtr = (GT_UINTPTR)magicPtr;
        magicPtr++;
        return (GT_VOID*)magicPtr;
#endif

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
*       Usage of this function is NEVER during initialization.
*
*******************************************************************************/
GT_VOID osFree
(
    IN GT_VOID* const memblock
)
{
    GT_UINTPTR *magicPtr = NULL;
    assert(memblock != NULL);
    if (firstInitialization == GT_TRUE)
    {
#ifdef LINUX_SIM
        SHARED_FREE_MAC(memblock-sizeof(GT_UINTPTR));
#endif
    }
    else
    {
        magicPtr = memblock-sizeof(GT_UINTPTR);
        /* check if this free is trying to free something that wasn't
           allocated by owr pools manager */
        if (*magicPtr == (GT_UINTPTR)magicPtr)
        {
#ifdef OSMEMPOOLS_DEBUG
            osPrintf("Warning:Trying to Free a pointer allocated by malloc after osMemInit !\n");
#endif

#ifdef LINUX_SIM
            SHARED_FREE_MAC(memblock-sizeof(GT_UINTPTR));
#endif
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
GT_VOID *osCacheDmaMalloc
(
    IN GT_U32 size
)
{
    IS_WRAPPER_OPEN_PTR;
#ifndef LINUX_SIM
    return shrMemSharedMallocIntoBlock(&cdma_mem, gtPpFd, size);
#else
    if(simDmaAsSharedMemSize)
    {
        GT_CHAR* tmpPtr; /* tmp pointer to start of memory to return */
        /* take sem */
        osMutexLock(simDmaMutexId);

        /* check if we have enough space for the new memory request */
        if((simDmaAsSharedMemOffsetUsed + size) > simDmaAsSharedMemSize)
        {
            /* free sem */
            osMutexUnlock(simDmaMutexId);
            return NULL;
        }

        tmpPtr = (GT_CHAR*)simDmaAsSharedMemPtr + simDmaAsSharedMemOffsetUsed;

        simDmaAsSharedMemOffsetUsed += size;

        /* free sem */
        osMutexUnlock(simDmaMutexId);

        return tmpPtr;
    }
    return SHARED_MALLOC_MAC(size) ;
#endif
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
    IN GT_VOID *ptr
)
{
    IS_WRAPPER_OPEN_STATUS;
#ifdef LINUX_SIM
    if(simDmaAsSharedMemSize)
    {
        /* not supporting free of this memory */
        return GT_OK;
    }
    SHARED_FREE_MAC(ptr) ;
#endif
  return GT_OK;
}

#if 0
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
    
    IS_WRAPPER_OPEN_STATUS;

#ifndef LINUX_SIM
    if (baseAddr == 0xffffffff)
    {
        osPrintf("osPhy2Virt:ERROR,baseAddr(0x%0x)\n", (int)baseAddr);
        return GT_FAIL;
        }

    if (phyAddr >= baseAddr && phyAddr < baseAddr + dmaSize)
    {
        *virtAddr = PRV_SHARED_PPS_VIRT_ADDR_MAC + (phyAddr - baseAddr);
        return GT_OK;
    }
    else
    {
        fprintf(stderr, "osPhy2Virt:ERROR,phyAddr(0x%0x)\n", (int)phyAddr);
        *virtAddr = 0xffffffff;
        osPrintf("osPhy2Virt:ERROR\n");
        return GT_FAIL;
    }
#else
    *virtAddr = phyAddr;
    return GT_OK;
#endif
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
    IS_WRAPPER_OPEN_STATUS;

#ifndef LINUX_SIM
    if (baseAddr == 0xffffffff)
    {
        /*ofPrintf(stderr, "osVirt2Phy:ERROR,baseAddr(0x%0x)\n", (int)baseAddr);*/
        return GT_FAIL;
    }

    if(virtAddr >= PRV_SHARED_PPS_VIRT_ADDR_MAC &&
       virtAddr < (PRV_SHARED_PPS_VIRT_ADDR_MAC+dmaSize))
    {
        *phyAddr = baseAddr + (virtAddr - PRV_SHARED_PPS_VIRT_ADDR_MAC);
        /*osPrintf("\nosVirt2Phy: *phyAddr 0x%X\n",*phyAddr);*/
        return GT_OK;
    }
    else
    {
        fprintf(stderr, "osVirt2Phy:ERROR,phyAddr(0x%0x)\n", (int)phyAddr);
        *phyAddr = 0xffffffff;
        /*osPrintf("osVirt2Phy:ERROR2\n");*/
        return GT_FAIL;
    }
#else
    *phyAddr = virtAddr;
    return GT_OK;
#endif
}
#endif

//#if 0
/*********************************************/
GT_STATUS osPhy2Virt
(
    IN  GT_U32 phyAddr,
    OUT GT_U32 *virtAddr
)
{

	unsigned int baseAddress = appDemoPpConfigList[0].pciInfo.deviceDmaBase;
    //osPrintf("osPhy2Virt:baseAddress(0x%0x)\n", baseAddress);

    IS_WRAPPER_OPEN_STATUS;

#ifndef LINUX_SIM
    if (baseAddress == 0xffffffff)
    {
        osPrintf("osPhy2Virt:ERROR,baseAddr(0x%0x)\n", (int)baseAddr);
        return GT_FAIL;
        }

    if (phyAddr >= baseAddress && phyAddr < baseAddress + dmaSize)
    {
        *virtAddr = PRV_SHARED_PPS_VIRT_ADDR_MAC + (phyAddr - baseAddress);

		//osPrintf("osPhy2Virt: base %#0x  %#0x --> %#0x\n",baseAddress,phyAddr,*virtAddr);

        return GT_OK;
    }
    else
    {
        fprintf(stderr, "osPhy2Virt:ERROR,phyAddr(0x%0x)\n", (int)phyAddr);
        *virtAddr = 0xffffffff;
        osPrintf("osPhy2Virt:ERROR\n");
        return GT_FAIL;
    }
#else
    *virtAddr = phyAddr;
    return GT_OK;
#endif
}

/**************************************************************/
GT_STATUS osVirt2Phy
(
    IN  GT_U32 virtAddr,
    OUT GT_U32 *phyAddr
)
{

	unsigned int baseAddress = appDemoPpConfigList[0].pciInfo.deviceDmaBase;
    //osPrintf("osVirt2Phy:baseAddress(0x%0x)\n", baseAddress);

    IS_WRAPPER_OPEN_STATUS;

#ifndef LINUX_SIM
    if (baseAddress == 0xffffffff)
    {
        /*ofPrintf(stderr, "osVirt2Phy:ERROR,baseAddr(0x%0x)\n", (int)baseAddr);*/
        return GT_FAIL;
    }

    if(virtAddr >= PRV_SHARED_PPS_VIRT_ADDR_MAC && virtAddr < (PRV_SHARED_PPS_VIRT_ADDR_MAC+dmaSize))
    {
        *phyAddr = baseAddress + (virtAddr - PRV_SHARED_PPS_VIRT_ADDR_MAC);
        //osPrintf("\nosVirt2Phy: *phyAddr 0x%X\n",*phyAddr);
        return GT_OK;
    }
    else
    {
        fprintf(stderr, "osVirt2Phy:ERROR,phyAddr(0x%0x)\n", (int)phyAddr);
        *phyAddr = 0xffffffff;
        /*osPrintf("osVirt2Phy:ERROR2\n");*/
        return GT_FAIL;
    }
#else
    *phyAddr = virtAddr;
    return GT_OK;
#endif
}
//#endif

/***** Private ********************************************************/

#ifdef LINUX_SIM
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
static GT_VOID* dmaAsSharedMemoryMalloc
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
#endif /* LINUX_SIM */
