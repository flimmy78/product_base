/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtOsMemLib.c
*
* DESCRIPTION:
*       osMemPool implementation
*
* DEPENDENCIES:
*       None
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/***** Includes ********************************************************/
#ifdef _VXWORKS
#include <vxWorks.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsGen.h>
#include <gtOs/gtOsIo.h>

/***** Function Declaration ********************************************/
GT_VOID *osMemPoolsAlloc(GT_U32 size);
GT_VOID osMemPoolFree(GT_VOID *memBlock);
GT_VOID *osMemPoolsRealloc(GT_VOID *ptr, GT_U32 size);
static GT_STATUS osMemChunkAllocate(GT_VOID);

static GT_STATUS osMemPoolsInit(GT_U32 blockSizeStep, GT_U32 stepsLimit,
                                GT_U32 poolsLimit);
static OSMEM_POOL_HDR *osMemPoolCreate(GT_U32 elemSize,GT_U32 poolIndex);
static GT_VOID* osMemPoolAllocSize(OSMEM_POOL_HDR_PTR poolHdrPtr,GT_U32 size);
static GT_VOID *osMemAllocDynamicFromChunk(GT_U32 size,GT_U32 *chunkIndex);
static GT_STATUS osMemPoolClear(OSMEM_POOL_HDR_PTR poolHdrPtr);

/***** External Functions **********************************************/
extern GT_STATUS mathLog2(IN  GT_U32  num,OUT GT_U32  *result);

/***** Global Vars ********************************************/

static OSMEM_POOL_HDR_PTR* os_mem_pools_array;
static GT_U32 os_mem_pools_array_size;
static GT_MUTEX os_mem_pools_mtx_id;
static GT_U32 *os_mem_pools_size2index;
static OSMEM_CHUNK_ARRAY os_mem_chunks_array;
static GT_U32 os_mem_poolsLimit;

/* poolIndex to mark osStaticMalloc */
#define POOL_INDEX_STATIC_CNS   0xFFFFFFFF


/* heap allocation measurement static variables */
static GT_U32 heap_bytes_allocated = 0;

/***** Public Functions ************************************************/

/*******************************************************************************
* osBzero
*
* DESCRIPTION:
*       Fills the first nbytes characters of the specified buffer with 0
*
* INPUTS:
*       start  - start address of memory block to be zeroed
*       nbytes - size of block to be set
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
GT_VOID osBzero
(
    IN GT_CHAR * start,
    IN GT_U32 nbytes
)
{
    assert(start != NULL);
#ifdef _VXWORKS
    bzero((GT_CHAR *)start, nbytes);
#else
    memset((GT_VOID *)start, 0,  nbytes);
#endif
    return;
}

/*******************************************************************************
* osMemSet
*
* DESCRIPTION:
*       Stores 'symbol' converted to an unsigned char in each of the elements
*       of the array of unsigned char beginning at 'start', with size 'size'.
*
* INPUTS:
*       start  - start address of memory block for setting
*       symbol - character to store, converted to an unsigned char
*       size   - size of block to be set
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Pointer to set memory block
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID * osMemSet
(
    IN GT_VOID * start,
    IN int    symbol,
    IN GT_U32 size
)
{
    if(start == NULL)
        return start;
#ifdef _VXWORKS
    IS_WRAPPER_OPEN_PTR;
#endif
    return memset(start, symbol, size);
}

/*******************************************************************************
* osMemCpy
*
* DESCRIPTION:
*       Copies 'size' characters from the object pointed to by 'source' into
*       the object pointed to by 'destination'. If copying takes place between
*       objects that overlap, the behavior is undefined.
*
* INPUTS:
*       destination - destination of copy
*       source      - source of copy
*       size        - size of memory to copy
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Pointer to destination
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID * osMemCpy
(
    IN GT_VOID *       destination,
    IN const GT_VOID * source,
    IN GT_U32       size
)
{
    if(destination == NULL || source == NULL)
        return destination;

#ifdef _VXWORKS
    IS_WRAPPER_OPEN_PTR;
#endif
    return memcpy(destination, source, size);
}

/*******************************************************************************
* osMemCmp
*
* DESCRIPTION:
*       Compare 'size' characters from the object pointed to by 'str1' to
*       the object pointed to by 'str2'.
*
* INPUTS:
*       str1 - first memory area
*       str2 - second memory area
*       size - size of memory to compare
*
* OUTPUTS:
*       None
*
* RETURNS:
*       > 0  - if str1 is alfabetic bigger than str2
*       == 0 - if str1 is equal to str2
*       < 0  - if str1 is alfabetic smaller than str2
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_32 osMemCmp
(
    IN const GT_VOID  *str1,
    IN const GT_VOID  *str2,
    IN GT_U32       size
)
{
    if(str1 == NULL || str2== NULL)
        return GT_BAD_PARAM;

#ifdef _VXWORKS
    IS_WRAPPER_OPEN_PTR;
#endif
    return (GT_32) memcmp(str1, str2, size);
}


/*******************************************************************************
* osMemLibInit
*
* DESCRIPTION:
*       Initialize the memory management with a user memory function.
*       Can be used to allocate from a dedicated memory pool that remains till
*       HW reset.
*
* INPUTS:
*       memAllocFuncPtr - the function to be used in memory allocation.
*       memFreeFuncPtr - the function to be used in memory allocation.
*       size - The memory size to manage
*       memAllocMethod - determines the method of the allocation
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
GT_STATUS osMemLibInit
(
    IN GT_MEMORY_ALLOC_FUNC memAllocFuncPtr,
    IN GT_MEMORY_FREE_FUNC memFreeFuncPtr,
    IN GT_U32 size,
    IN GT_MEMORY_MANAGE_METHOD_ENT memAllocMethod
)
{
    GT_U32 chunkMaxAmount = 0;
    GT_STATUS retVal = GT_OK;

    static GT_BOOL firstMemInit = GT_TRUE;

    if (firstMemInit == GT_TRUE)
    {
        if (size == 0)
        {
            return GT_BAD_PARAM;
        }

        chunkMaxAmount = OSMEM_MAX_MEMORY_SIZE_USE / size;

        os_mem_chunks_array.chunkArray = memAllocFuncPtr(sizeof(OSMEM_CHUNK_POINTER)
                                                            *chunkMaxAmount);
        os_mem_chunks_array.chunkSize = size;
        os_mem_chunks_array.lastChunkIndex = 0;
        os_mem_chunks_array.staticIndex = 0;
        os_mem_chunks_array.memAllocMethod = memAllocMethod;
        os_mem_chunks_array.chunkArraySize = chunkMaxAmount;
        os_mem_chunks_array.memAllocFuncPtr = memAllocFuncPtr;
        os_mem_chunks_array.memFreeFuncPtr = memFreeFuncPtr;

        retVal  = osMemChunkAllocate();
        if (retVal != GT_OK)
        {
            return retVal;
        }

        retVal = osMemPoolsInit(OSMEM_DEFAULT_BLOCK_STEP_SIZE,
                       OSMEM_DEFAULT_STEPS_LIMIT,
                       OSMEM_DEFAULT_POOLS_LIMIT);
        if (retVal != GT_OK)
        {
            memFreeFuncPtr(os_mem_chunks_array.chunkArray[0].startAddr);
            os_mem_chunks_array.lastChunkIndex = 0;
            memFreeFuncPtr(os_mem_chunks_array.chunkArray);
            os_mem_chunks_array.chunkArray = NULL;
            return retVal;
        }
#ifdef _VXWORKS
        /*gtShutdownRegister(osMemPoolsClear);*/
#endif
    }

    firstMemInit = GT_FALSE;

    return retVal;
}

/*******************************************************************************
* osStaticMallocFromChunk
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
GT_VOID *osStaticMallocFromChunk
(
    IN GT_U32 size
)
{
    OSMEM_POOL_ELEM_PTR retPtr = NULL;
    OSMEM_CHUNK_POINTER_PTR curChunk;
    GT_U32 allocSize;

    /* Align size to 4 bytes. */
    size = (size + 3) & 0xFFFFFFFC;

    osMutexLock(os_mem_pools_mtx_id);
    allocSize = size + sizeof (OSMEM_POOL_ELEM);

    if (allocSize > os_mem_chunks_array.chunkSize)
    {
        osPrintf("Warning:Tried to allocate %d which is bigger then the memory chunk size. \n",
                 size);
        osMutexUnlock(os_mem_pools_mtx_id);

        return os_mem_chunks_array.memAllocFuncPtr(size);
    }
    curChunk =
        &os_mem_chunks_array.chunkArray[os_mem_chunks_array.staticIndex];

    while(os_mem_chunks_array.staticIndex < os_mem_chunks_array.lastChunkIndex &&
          curChunk->staticAddr - allocSize < curChunk->dynamicAddr)
    {
        os_mem_chunks_array.staticIndex++;
        curChunk =
        &os_mem_chunks_array.chunkArray[os_mem_chunks_array.staticIndex];
    }

    if (os_mem_chunks_array.staticIndex >= os_mem_chunks_array.lastChunkIndex)
    {
        if (os_mem_chunks_array.memAllocMethod ==
                GT_MEMORY_MANAGE_METHOD_NO_REALLOC_ON_DEMAND_E)
        {
            osMutexUnlock(os_mem_pools_mtx_id);

            osPrintf("Out Of Memory!!\n");
            return NULL;
        }
        if (osMemChunkAllocate() != GT_OK)
        {
            osMutexUnlock(os_mem_pools_mtx_id);
            return NULL;
        }
    }

    curChunk->staticAddr -= allocSize;
    curChunk->freeSize -= allocSize;
    retPtr = (OSMEM_POOL_ELEM_PTR)curChunk->staticAddr;

    retPtr->poolIndex = POOL_INDEX_STATIC_CNS;  /* it's more safe to use larger value, it allow more than 255 pools */

    osMutexUnlock(os_mem_pools_mtx_id);

    retPtr++;
    return (retPtr);
}

/*******************************************************************************
* osMemPoolsRealloc
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
GT_VOID *osMemPoolsRealloc
(
    IN GT_VOID * ptr ,
    IN GT_U32    size
)
{
    GT_VOID *allocPtr = NULL;
    GT_UINTPTR  *magicPtr;
    GT_U32  oldSize = size;
    OSMEM_POOL_ELEM_PTR memPoolElem;
    OSMEM_VAR_POOL_ELEM_PTR allocVarPtr;
#ifdef _VXWORKS
    IS_WRAPPER_OPEN_PTR;
#endif

    allocPtr = osMemPoolsAlloc(size);
    if (allocPtr == NULL)
    {
        return NULL;
    }

    /* resolve size of memory pointed by ptr */
    magicPtr = ((GT_UINTPTR*)ptr)-1;
    if (*magicPtr != (GT_UINTPTR)magicPtr)
    {
        /* this memory is allocated by pool manager */
        memPoolElem = (OSMEM_POOL_ELEM_PTR)((GT_VOID*)((GT_UINTPTR)ptr - sizeof(OSMEM_POOL_ELEM)));
        if (memPoolElem->poolIndex <= os_mem_pools_array_size)
        {
            oldSize = os_mem_pools_array[memPoolElem->poolIndex]->elem_size;

            if (oldSize == 0)
            {
                allocVarPtr = (OSMEM_VAR_POOL_ELEM_PTR)
                              ((GT_VOID*)((GT_UINTPTR)ptr - sizeof(OSMEM_VAR_POOL_ELEM)));
                oldSize = allocVarPtr->size;
            }
        }
    }

    osMemCpy(allocPtr,ptr,size > oldSize ? oldSize : size);
    osMemPoolFree(ptr);
    return allocPtr;
}

/*******************************************************************************
* osMemPoolsClear
*
* DESCRIPTION:
*      returns all the allocated memory from all the pools to free.
*
* INPUTS:
*       none.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - if success.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osMemPoolsClear()
{
    GT_U32 i,dynSize;

    osMutexLock(os_mem_pools_mtx_id);

    for (i = 0; i < os_mem_pools_array_size; ++i)
    {
        osMemPoolClear(os_mem_pools_array[i]);
    }

    for (i = 0; i < os_mem_chunks_array.lastChunkIndex; ++i)
    {
        dynSize = os_mem_chunks_array.chunkArray[i].dynamicAddr -
            os_mem_chunks_array.chunkArray[i].startAddr;

        os_mem_chunks_array.chunkArray[i].freeSize += dynSize;

        os_mem_chunks_array.chunkArray[i].dynamicAddr =
            os_mem_chunks_array.chunkArray[i].startAddr;
    }

    osMutexUnlock(os_mem_pools_mtx_id);

    return GT_OK;
}

/*******************************************************************************
* osMemBindMemAllocFunc
*
* DESCRIPTION:
*        Binds the the memory allocation function
*
* INPUTS:
*        memAllocFuncPtr - the function to be used in memory allocation
*                          (refer to osMemLibInit)
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - operation succeeded
*       GT_FAIL - operation failed
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS osMemBindMemAllocFunc
(
    IN   GT_MEMORY_ALLOC_FUNC    memAllocFuncPtr
)
{
    os_mem_chunks_array.memAllocFuncPtr = memAllocFuncPtr;

    return GT_OK;
}

/***** Private ********************************************************/

/*******************************************************************************
* osMemChunkAllocate
*
* DESCRIPTION:
*       Allocates another chunk to the chunk array.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - if success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS osMemChunkAllocate(GT_VOID)
{
    if (os_mem_chunks_array.lastChunkIndex ==os_mem_chunks_array.chunkArraySize)
    {
        return GT_OUT_OF_RANGE;
    }

    os_mem_chunks_array.chunkArray[os_mem_chunks_array.lastChunkIndex].startAddr
        = os_mem_chunks_array.memAllocFuncPtr(os_mem_chunks_array.chunkSize);

    if (os_mem_chunks_array.chunkArray[os_mem_chunks_array.lastChunkIndex].startAddr
        == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    os_mem_chunks_array.chunkArray[os_mem_chunks_array.lastChunkIndex].dynamicAddr =
        os_mem_chunks_array.chunkArray[os_mem_chunks_array.lastChunkIndex].startAddr;

    os_mem_chunks_array.chunkArray[os_mem_chunks_array.lastChunkIndex].staticAddr =
        os_mem_chunks_array.chunkArray[os_mem_chunks_array.lastChunkIndex].startAddr +
        os_mem_chunks_array.chunkSize;

    os_mem_chunks_array.chunkArray[os_mem_chunks_array.lastChunkIndex].freeSize =
        os_mem_chunks_array.chunkSize;

    os_mem_chunks_array.lastChunkIndex++;
    return GT_OK;

}

/*******************************************************************************
* osMemPoolsInit
*
* DESCRIPTION:
*      initializes the pools for memory management. it creates pools with sizes
*      starting with blockSizeStep and adding blockSizeStep to the created pool
*      size until it reaches stepsLimit, from then on till poolsLimit it doubles
*      the size every time. (example: if blockSizeStep = 2, stepsLimit = 8,
*      poolsLimit = 32 :the pools that will be created are with element sizes
       of : 2,4,6,8,16,32)
* INPUTS:
*       blockSizeStep - the step in size that should be added for every
*                       sequanctual pool created.
*       stepsLimit - the limit at which sizes stop being add to but doubled
*                    (must be a power of 2)
*       poolsLimit - the biggest size pool that will be created.
*                    (must be a power of 2)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - if success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS osMemPoolsInit
(
    IN GT_U32 blockSizeStep,
    IN GT_U32 stepsLimit,
    IN GT_U32 poolsLimit

)
{
    GT_U32 i, curSize,curInd,log2ofStepsLimit,log2ofPoolsLimit;
    log2ofStepsLimit = 0;
    log2ofPoolsLimit = 0;

    /* some sanity checks first */
    if (blockSizeStep == 0 ||
        stepsLimit == 0 ||
        poolsLimit == 0 ||
        (poolsLimit < stepsLimit) ||
        ((stepsLimit % blockSizeStep) != 0 ) ||
        (mathLog2(stepsLimit,&log2ofStepsLimit) != GT_OK) ||
        (mathLog2(poolsLimit,&log2ofPoolsLimit) != GT_OK))

    {
        return GT_FAIL;
    }

    os_mem_poolsLimit = poolsLimit;

    /* Create the sync semaphore */
    if (osMutexCreate("osMemPools", &os_mem_pools_mtx_id) != GT_OK)
    {
        return GT_FAIL;
    }
    osMutexLock(os_mem_pools_mtx_id);

    /* calculate the amount of pools we need */
    os_mem_pools_array_size = (stepsLimit / blockSizeStep) +
        (log2ofPoolsLimit - log2ofStepsLimit) + 1 ;

    os_mem_pools_array =
        os_mem_chunks_array.memAllocFuncPtr(sizeof(OSMEM_POOL_HDR_PTR) * (os_mem_pools_array_size + 1));

    os_mem_pools_size2index = os_mem_chunks_array.memAllocFuncPtr(sizeof(*os_mem_pools_size2index) *(poolsLimit+1));

    if (os_mem_pools_array == NULL || os_mem_pools_size2index == NULL)
    {
        osMutexUnlock(os_mem_pools_mtx_id);
        osMutexDelete(os_mem_pools_mtx_id);
        os_mem_pools_mtx_id = (GT_MUTEX)0;
        return GT_FAIL;
    }

    curSize = blockSizeStep;
    for (i = 0; i < os_mem_pools_array_size - 1; ++i)
    {
        os_mem_pools_array[i] = osMemPoolCreate(curSize,i);
        if (os_mem_pools_array[i] == NULL)
        {
            osMutexUnlock(os_mem_pools_mtx_id);
            osMutexDelete(os_mem_pools_mtx_id);
            os_mem_pools_mtx_id = (GT_MUTEX)0;
            return GT_FAIL;
        }

        if (curSize >= stepsLimit)
        {
            curSize *= 2;
        }
        else
        {
            curSize += blockSizeStep;
        }
    }

    /* now build the variuos sizes pool */
    os_mem_pools_array[i] = osMemPoolCreate(0,i);

    /* now lets build the index list for fast size to pools indexing */
    curInd = 0;
    for (i = 1; i<= poolsLimit; i++)
    {
        os_mem_pools_size2index[i] = curInd;
        if (i >= stepsLimit)
        {
            if (mathLog2(i,&log2ofStepsLimit) == GT_OK)
            {
                curInd++;
            }
        }
        else if ( (i % blockSizeStep) == 0)
        {
            curInd++;
        }
    }

    osMutexUnlock(os_mem_pools_mtx_id);

    return GT_OK;
}

/*******************************************************************************
* osMemPoolCreate
*
* DESCRIPTION:
*      creates a memory pool for malloc managment
*
* INPUTS:
*       elemSize - the size of elements to be allocated from this pool.
*                  in case the size = 0 then this is a various sizes pool.
*       poolIndex - the index of the pool in the pool array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       the pool id .
*
* COMMENTS:
*       None
*
*******************************************************************************/
static OSMEM_POOL_HDR *osMemPoolCreate
(
    IN GT_U32 elemSize,
    IN GT_U32 poolIndex
)
{
    OSMEM_POOL_HDR *poolHdrPtr;
    poolHdrPtr = NULL;

    poolHdrPtr = os_mem_chunks_array.memAllocFuncPtr(sizeof(OSMEM_POOL_HDR));

    if (poolHdrPtr == NULL)
    {
        return NULL;
    }

    poolHdrPtr->elem_size = elemSize;
    poolHdrPtr->first_free_PTR = NULL;
    poolHdrPtr->chunkIndex = 0;
    poolHdrPtr->poolIndex = poolIndex;

    poolHdrPtr->free_size = 0;
    poolHdrPtr->size = 0;
    poolHdrPtr->overAlloc = 0;
    poolHdrPtr->alloc_times = 0;
    poolHdrPtr->malloc_used = 0;
    poolHdrPtr->free_times = 0;

    return poolHdrPtr;
}

/*******************************************************************************
* osMemPoolsAlloc
*
* DESCRIPTION:
*       Allocates memory block of specified siz.
*
* INPUTS:
*       none.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Void pointer to the allocated space, or NULL if there is
*       insufficient memory available.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID *osMemPoolsAlloc
(
    IN GT_U32 size
)
{
    GT_VOID *memBlock;
    GT_UINTPTR *u32MemBlockPtr;
    OSMEM_POOL_HDR_PTR curPool = NULL;

    /* search through the pools for the appropriate size pool */
    if (size > os_mem_poolsLimit)
    {
        if ((size + sizeof (OSMEM_VAR_POOL_ELEM)) > os_mem_chunks_array.chunkSize)
        {
            osPrintf("Warning:Tried to allocate %d which is bigger then the memory chunk size. \n",
                     size);
            osMutexUnlock(os_mem_pools_mtx_id);
            u32MemBlockPtr = (GT_UINTPTR*)os_mem_chunks_array.memAllocFuncPtr(size+sizeof(GT_UINTPTR));
            if( NULL == u32MemBlockPtr )
            {
                return NULL;
            }
            *u32MemBlockPtr = (GT_UINTPTR)u32MemBlockPtr;
            return (GT_VOID*)(u32MemBlockPtr+1);
        }
        curPool = os_mem_pools_array[os_mem_pools_array_size-1];
    }
    else
    {
        curPool = os_mem_pools_array[os_mem_pools_size2index[size]];
    }
    /* heap allocation byte counter correction */
    heap_bytes_allocated += curPool->elem_size;

    osMutexLock(os_mem_pools_mtx_id);

    /* now allocate the pointer */
    memBlock = osMemPoolAllocSize(curPool,size);


    osMutexUnlock(os_mem_pools_mtx_id);

    return memBlock;
}

/*******************************************************************************
* osMemPoolAllocSize
*
* DESCRIPTION:
*      allocates a pointer from a memory pool (for sizes bigger then
*      poolsLimit - see osMemPoolsInit).
*
* INPUTS:
*       poolId - the pool from which to allocate.
*       size   - the size to allocate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       the pointer to the newly allocated data.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_VOID* osMemPoolAllocSize
(
    IN OSMEM_POOL_HDR_PTR poolHdrPtr,
    IN GT_U32             size
)
{
    OSMEM_POOL_FREE_ELEM_PTR freePtr = NULL;
    OSMEM_POOL_FREE_ELEM_PTR prvFreePtr = NULL;

    OSMEM_VAR_POOL_ELEM_PTR allocVarPtr = NULL;
    OSMEM_POOL_ELEM_PTR allocPtr = NULL;
    GT_U32 allocSize = 0;

    poolHdrPtr->alloc_times++;
    /* if there is a free element in the free list take from there */

    /* first deal with various sizes pool */
    if (poolHdrPtr->elem_size == 0)
    {
        size = (size + 3) & 0xfffffffc; /* align 4 bytes */
        if (poolHdrPtr->first_free_PTR != NULL)
        {
            freePtr = poolHdrPtr->first_free_PTR;
            while ((freePtr != NULL) && (freePtr->size != size))
            {
                prvFreePtr = freePtr;
                freePtr = freePtr->next_elem;
            }
            if (freePtr != NULL)
            {
                if (prvFreePtr == NULL)
                {
                    poolHdrPtr->first_free_PTR = freePtr->next_elem;
                }
                else
                {
                    prvFreePtr->next_elem = freePtr->next_elem;
                }

                poolHdrPtr->free_size  = poolHdrPtr->free_size - size;

            }
        }

    }
    else if (poolHdrPtr->first_free_PTR != NULL)
    {
        freePtr = poolHdrPtr->first_free_PTR;

        /* now link out the pieces */
        poolHdrPtr->first_free_PTR = freePtr->next_elem;

        poolHdrPtr->free_size  = poolHdrPtr->free_size -
                                 poolHdrPtr->elem_size;
    }


    allocPtr = (OSMEM_POOL_ELEM_PTR)freePtr;
    allocVarPtr = (OSMEM_VAR_POOL_ELEM_PTR)freePtr;
    /* if we could find any elements in the free list -> take a new element
       from the chunk */

    /* firsr deal with various size pool */
    if (poolHdrPtr->elem_size == 0)
    {
        if (allocVarPtr == NULL)
        {
            allocSize = sizeof(OSMEM_VAR_POOL_ELEM) + size;

            allocVarPtr = osMemAllocDynamicFromChunk(allocSize,
                                                     &poolHdrPtr->chunkIndex);

            if (allocVarPtr == NULL)
            {
                return NULL;
            }

            allocVarPtr->size = size;

            poolHdrPtr->malloc_used++;

            poolHdrPtr->size += size;
        }

        /* this variable consists the required allocation size  */
        /* allocVarPtr->poolIndex = size;  - it seems to be an error */
        /* last poolIndex - var size pool */
        allocVarPtr->poolIndex = poolHdrPtr->poolIndex;

        /* return a pointer to the data segment only */
        allocVarPtr++;
        return(allocVarPtr);

    }
    else if (allocPtr == NULL)
    {
        allocSize = poolHdrPtr->elem_size;
        poolHdrPtr->overAlloc += poolHdrPtr->elem_size - size;

        allocPtr = osMemAllocDynamicFromChunk(
            sizeof(OSMEM_POOL_ELEM) + allocSize,
            &poolHdrPtr->chunkIndex);

        if (allocPtr == NULL)
        {
            return NULL;
        }

        poolHdrPtr->malloc_used++;

        poolHdrPtr->size += poolHdrPtr->elem_size;
    }

    /* this variable consists the required allocation size  */
    allocPtr->poolIndex = poolHdrPtr->poolIndex;

    /* return a pointer to the data segment only */
    allocPtr++;
    return (allocPtr);

}

/*******************************************************************************
* osMemAllocDynamicFromChunk
*
* DESCRIPTION:
*       Allocates dynamic alloc memory from the given chunk.
*
* INPUTS:
*       size - the size to allocate.
*       chunkIndex - the index of the chunk from which to allocate.
*
* OUTPUTS:
*       chunkIndex - if the memory was allocated from a different chunk then the
*                    given one , then that chunk index is returned.
*
* RETURNS:
*       a pointer to the allocated memory.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_VOID *osMemAllocDynamicFromChunk
(
    IN    GT_U32 size,
    INOUT GT_U32 *chunkIndex
)
{
    GT_VOID *retPtr = NULL;
    OSMEM_CHUNK_POINTER_PTR curChunk;

    /* start by using the given chunk */
    curChunk = &(os_mem_chunks_array.chunkArray[*chunkIndex]);

    /* now look for the first allocated chunk with enough memory (could be the
       given one) */
    while (*chunkIndex < os_mem_chunks_array.lastChunkIndex &&
           curChunk->dynamicAddr + size > curChunk->staticAddr)
    {
        (*chunkIndex)++;
        curChunk = &(os_mem_chunks_array.chunkArray[*chunkIndex]);
    }

    /* if none of the allocated ones hasn't got enough memory allocated a new
       one (if possible) */
    if (*chunkIndex >= os_mem_chunks_array.lastChunkIndex)
    {
        /* check if allowed to allocate more? */
        if (os_mem_chunks_array.memAllocMethod ==
                    GT_MEMORY_MANAGE_METHOD_NO_REALLOC_ON_DEMAND_E)
        {
            /* we reached out of mem situation! */
            return NULL;
        }
        /* allocate another chunk */
        if (osMemChunkAllocate() != GT_OK)
        {
            return NULL;
        }
    }
    /* now take the dynamicAddr for the return ptr */
    retPtr = curChunk->dynamicAddr;
    /* and update the chunk */
    curChunk->dynamicAddr += size;
    curChunk->freeSize -= size;

    /* and as a precheck that the next element can or cannot be allocated from
       this chunk */
    if (curChunk->dynamicAddr + size > curChunk->staticAddr)
    {
        (*chunkIndex)++;
    }

    return retPtr;
}

/*******************************************************************************
* osMemPoolFree
*
* DESCRIPTION:
*      frees a pointer from a memory pool
*
* INPUTS:
*       memBlock - the pointer which was allocated.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       none
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID osMemPoolFree
(
    IN GT_VOID    *memBlock
)
{
    OSMEM_POOL_HDR_PTR poolHdrPtr;
    OSMEM_POOL_ELEM_PTR allocPtr;
    OSMEM_VAR_POOL_ELEM_PTR allocVarPtr;
    OSMEM_POOL_FREE_ELEM_PTR freePtr;

    GT_U32 freeSize = 0;

    osMutexLock(os_mem_pools_mtx_id);

    /* Note: the structure in memory just before *memBlock can be:
        1) OSMEM_POOL_ELEM_PTR      (4 bytes:              GT_U32 poolIndex)
        2) OSMEM_VAR_POOL_ELEM_PTR  (8 bytes: GT_U32 size, GT_U32 poolIndex) */
    allocPtr = (OSMEM_POOL_ELEM_PTR)((GT_VOID*)((GT_UINTPTR)memBlock - sizeof(OSMEM_POOL_ELEM)));

    if (allocPtr->poolIndex > os_mem_pools_array_size)
    {   /* out of range poolIndex */
        if (allocPtr->poolIndex == POOL_INDEX_STATIC_CNS)
        {
            osPrintf("Warning:Trying to free a osStaticMalloc pointer !!!\n");
            osMutexUnlock(os_mem_pools_mtx_id);
            return;
        }
        else
        {   /* this should never happen */
            osPrintf("Warning: osMemPoolFree - invalid poolIndex, memory lost !!!\n");
            osMutexUnlock(os_mem_pools_mtx_id);
            return;
        }
    }

    poolHdrPtr = os_mem_pools_array[allocPtr->poolIndex];

    if (poolHdrPtr->elem_size == 0)
    {
        /* various size pool */
        allocVarPtr = (OSMEM_VAR_POOL_ELEM_PTR)
                      ((GT_VOID*)((GT_UINTPTR)memBlock - sizeof(OSMEM_VAR_POOL_ELEM)));
        freePtr = (OSMEM_POOL_FREE_ELEM_PTR)allocVarPtr;
        freePtr->size = allocVarPtr->size;

        freeSize = freePtr->size;

    }
    else
    {
        freePtr = (OSMEM_POOL_FREE_ELEM_PTR)allocPtr;
        freeSize = poolHdrPtr->elem_size;
    }

    /* heap allocation byte counter correction */
    heap_bytes_allocated -= freeSize;

    poolHdrPtr->free_times++;

   /* now deal with the free list ***** */

    freePtr->next_elem = poolHdrPtr->first_free_PTR;
    poolHdrPtr->first_free_PTR = freePtr;

    poolHdrPtr->free_size = poolHdrPtr->free_size + freeSize;

    osMutexUnlock(os_mem_pools_mtx_id);

    return;
}

/*******************************************************************************
* osMemPoolClear
*
* DESCRIPTION:
*      returns all the allocated memory from this pool to free.
*
* INPUTS:
*       poolId - the pool to clear.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - if success.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS osMemPoolClear
(
    IN OSMEM_POOL_HDR_PTR poolHdrPtr
)
{
    poolHdrPtr->first_free_PTR = NULL;
    poolHdrPtr->free_size = 0;
    poolHdrPtr->size = 0;
    poolHdrPtr->chunkIndex = 0;

    return GT_OK;
}

/***** Debug ********************************************************/
#ifdef OSMEMPOOLS_DEBUG

/* Global variables for debug */
static GT_U32 heap_start_counter_value = 0;
static GT_32  countDynamic = 0;
static GT_32  countStatic = 0;
static GT_32  countDynamicOver = 0;

/*******************************************************************************
* osMemPrintMemPoolsStat
*
* DESCRIPTION:
*        print statistics of all pools
*
* INPUTS:
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID osMemPrintMemPoolsStat
(
    GT_VOID
)
{
    GT_U32 i;
    GT_U32 totalFree, totalAlloc, totalAllocTimes, totalFreeTimes;
    GT_U32 totalMallocUsed, totalOverhead, totalOverAlloc, dynamicSize, staticSize;
    GT_U32 totalStatic, totalDynamic;
    
    totalFree = 0;
    totalAlloc = 0;
    totalAllocTimes = 0;
    totalFreeTimes = 0;
    totalMallocUsed = 0;
    totalOverhead = 0;
    totalOverAlloc = 0;
    totalStatic = 0;
    totalDynamic = 0;

    osPrintf("osMemPools Status:\n");
    osPrintf("------------------\n\n");
    for (i = 0; i < os_mem_pools_array_size; ++i)
    {
        if (os_mem_pools_array[i]->size > 0)
        {
            osPrintf("Statistics for Pool with Element Size of:       %d\n",
                     os_mem_pools_array[i]->elem_size);
            osPrintf("-------------------------------------------------------------------------\n");
            osPrintf("Free: %d \tUsed: %d\tSize: %d\tOverhead:(Alloc) %d\t (List)%d\n",
                     os_mem_pools_array[i]->free_size,
                     (os_mem_pools_array[i]->size -
                     os_mem_pools_array[i]->free_size),
                     os_mem_pools_array[i]->size,
                     os_mem_pools_array[i]->overAlloc,
                     os_mem_pools_array[i]->alloc_times*
                     sizeof(OSMEM_POOL_ELEM));
            osPrintf("Alloc called: %d \tFree called %d \t Reusage(Per) %d\t Chunk:%d->%d\n\n",
                     os_mem_pools_array[i]->alloc_times,
                     os_mem_pools_array[i]->free_times,
                     ((os_mem_pools_array[i]->alloc_times -
                      os_mem_pools_array[i]->malloc_used)*100) /
                     os_mem_pools_array[i]->alloc_times,
                     os_mem_pools_array[i]->chunkIndex,
                     os_mem_chunks_array.chunkArray[os_mem_pools_array[i]->chunkIndex].freeSize);
            totalFree += os_mem_pools_array[i]->free_size;
            totalAlloc += os_mem_pools_array[i]->size;
            totalAllocTimes += os_mem_pools_array[i]->alloc_times;
            totalFreeTimes += os_mem_pools_array[i]->free_times;
            totalMallocUsed += os_mem_pools_array[i]->malloc_used;
            totalOverhead += os_mem_pools_array[i]->malloc_used*
                     sizeof(OSMEM_POOL_ELEM);
            totalOverAlloc += os_mem_pools_array[i]->overAlloc;
        }
    }

    osPrintf("Total Free: %d\tTotal Allocated: %d\tTotal OverHead:(Alloc) %d (List) %d\n",
             totalFree,totalAlloc,totalOverAlloc, totalOverhead);
    osPrintf("Total Alloc called: %d\tTotal Free called %d\tTotal malloc used: %d\n",
             totalAllocTimes,totalFreeTimes,totalMallocUsed);
    osPrintf("Total Memory used: %d \n",totalOverhead+totalAlloc);

    totalFree = 0;
    totalAlloc = 0;
    for (i = 0; i < os_mem_chunks_array.lastChunkIndex; ++i)
    {
        osPrintf("\nStatistics for Chunk %d \t Size: %d",
                 i,
                 os_mem_chunks_array.chunkSize);
        osPrintf("-------------------------------------------------------------------------\n");
        dynamicSize = os_mem_chunks_array.chunkArray[i].dynamicAddr -
                      os_mem_chunks_array.chunkArray[i].startAddr;
        totalDynamic += dynamicSize;
        staticSize = os_mem_chunks_array.chunkSize -
                     (os_mem_chunks_array.chunkArray[i].staticAddr -
                      os_mem_chunks_array.chunkArray[i].startAddr);
        totalStatic += staticSize;

        totalFree += os_mem_chunks_array.chunkArray[i].freeSize;
        totalAlloc += os_mem_chunks_array.chunkSize;

        osPrintf("Dynamic: %d(%d)\t Static: %d(%d)\t Free:%d(%d)\n",
                 dynamicSize,
                 (dynamicSize* 100)/os_mem_chunks_array.chunkSize,
                 staticSize,
                 (staticSize* 100)/os_mem_chunks_array.chunkSize,
                 os_mem_chunks_array.chunkArray[i].freeSize,
                 (os_mem_chunks_array.chunkArray[i].freeSize* 100)
                 /os_mem_chunks_array.chunkSize);
    }

    osPrintf("\nTotal Chunk: Dynamic : %d(%d) Static: %d(%d) Free: %d(%d)\n",
             totalDynamic,
             (totalDynamic*100)/totalAlloc,
             totalStatic,
             (totalStatic*100)/totalAlloc,
             totalFree,
             (totalFree*100)/totalAlloc);

    return;
}

/*******************************************************************************
* osMemPrintMemDelta
*
* DESCRIPTION:
*        print memory delta
*
* INPUTS:
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID osMemPrintMemDelta
(
    GT_U32* dyn, 
    GT_U32 *dynOver, 
    GT_U32 *staticAlloc
)
{
    GT_U32 i;
    GT_U32 totalFree, totalAlloc, totalAllocTimes, totalFreeTimes;
    GT_U32 totalMallocUsed, totalOverhead,totalOverAlloc, staticSize;
    GT_U32 totalStatic, totalDynamic;
    
    totalFree = 0;
    totalAlloc = 0;
    totalAllocTimes =0;
    totalFreeTimes = 0;
    totalMallocUsed = 0;
    totalOverhead = 0;
    totalOverAlloc = 0;
    totalStatic = 0;
    totalDynamic = 0;

    for (i = 0; i < os_mem_pools_array_size; ++i)
    {
        if (os_mem_pools_array[i]->size > 0)
        {
            totalAlloc += os_mem_pools_array[i]->size - os_mem_pools_array[i]->free_size;
            totalOverhead += os_mem_pools_array[i]->malloc_used*
                     sizeof(OSMEM_POOL_ELEM);
        }
    }

   for (i = 0; i < os_mem_chunks_array.lastChunkIndex; ++i)
   {
       staticSize = os_mem_chunks_array.chunkSize -
                    (os_mem_chunks_array.chunkArray[i].staticAddr -
                     os_mem_chunks_array.chunkArray[i].startAddr);
       totalStatic += staticSize;
   }
   /*osPrintf("\nDelta: Dynamic : %d dynamic+overhead : %d Static: %d \n",
            totalAlloc - countDynamic,
            totalAlloc+totalOverhead - countDynamicOver,
            totalStatic - countStatic);*/

    if ((dyn != NULL) &&
        (dynOver != NULL) &&
        (staticAlloc != NULL))
    {
        *dyn += totalAlloc - countDynamic;
        *dynOver += totalAlloc+totalOverhead - countDynamicOver;
        *staticAlloc += totalStatic - countStatic;
    }

   countDynamic = totalAlloc;
   countStatic = totalStatic;
   countDynamicOver = totalAlloc+totalOverhead;

    return;
}

/*******************************************************************************
* osMemGetHeapBytesAllocated
*
* DESCRIPTION:
*       returns the number of bytes allocated in heap
*
* INPUTS:
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
*
* COMMENTS:
*
*******************************************************************************/
GT_U32 osMemGetHeapBytesAllocated
(
    GT_VOID
)
{
    return heap_bytes_allocated;
}

/*******************************************************************************
* osMemStartHeapAllocationCounter
*
* DESCRIPTION:
*        set the current value of heap allocated bytes to the allocation counter
*
* INPUTS:
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID osMemStartHeapAllocationCounter
(
    GT_VOID
)
{
    heap_start_counter_value = heap_bytes_allocated;
}

/*******************************************************************************
* osMemGetHeapAllocationCounter
*
* DESCRIPTION:
*        returns the delta of current allocated bytes number and the value of allocation counter set by
*        preveous startHeapAllocationCounter() function
*
* INPUTS:
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
GT_U32 osMemGetHeapAllocationCounter
(
    GT_VOID
)
{
    return heap_bytes_allocated - heap_start_counter_value;
}

#endif /*OSMEMPOOLS_DEBUG*/
