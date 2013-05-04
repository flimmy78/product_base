/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* shrMemRemapper.c
*
* DESCRIPTION:
*       This file contains remapping routines which remaps regular libcpss.so
*       shared object (TEXT section is common) to CPSS Shared Library (also
*       BSS and DATA sections are common).
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

/************* Includes *******************************************************/
#define _GNU_SOURCE

#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <gtOs/gtEnvDep.h>
#include <gtOs/gtOsIo.h>
#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsGen.h>
#include <gtOs/gtOsSharedMemoryRemapper.h>

#ifdef OS_NAMED_SEM
#   include <gtOs/gtOsSem.h>
#   define SEM_TYPE GT_SEM
#if 0
    static GT_STATUS SEM_UNLINK(const char *name)
    {
        GT_SEM sem;
        if (osSemMOpenNamed(name, &sem) != GT_OK)
            return GT_FAIL;
        osSemDelete(sem);
        return GT_OK;
    }
#endif
    static GT_STATUS SEM_OPEN(const char *name, SEM_TYPE *semid)
    {
        if(osSemMOpenNamed(name, semid) != GT_OK)
        {
            fprintf(stderr, "osSemMOpenNamed(\"%s\") : can't open/create semaphore\n",
                name);
            return GT_FAIL;
        }
        return GT_OK;
    }
    static GT_STATUS SEM_CLOSE(SEM_TYPE semid)
    {
        return GT_OK;
    }
    static GT_STATUS SEM_WAIT(SEM_TYPE semid)
    {
        return osSemWait(semid, OS_WAIT_FOREVER);
    }
    static GT_STATUS SEM_POST(SEM_TYPE semid)
    {
        return osSemSignal(semid);
    }
#endif
#ifdef POSIX_SEM
#   include <semaphore.h>
#   define  SEM_TYPE sem_t*
#if 0
    static GT_STATUS SEM_UNLINK(const char *name)
    {
        if(sem_unlink(name) < 0)
        {
            if(errno != ENOENT && 0 != errno)
            {
                fprintf(stderr, "sem_unlink() : %s %s\n", SHARED_MEMORY_SEM_CNS, strerror(errno));
                return GT_FAIL;
            }
        }
        return GT_OK;
    }
#endif
    static GT_STATUS SEM_OPEN(const char *name, SEM_TYPE *semid)
    {
        if((*semid = sem_open(name, O_CREAT, 0666, 1)) == SEM_FAILED)
        {
            fprintf(stderr, "sem_open() : %s\n", strerror(errno));
            return GT_FAIL;
        }
        return GT_OK;
    }
    static GT_STATUS SEM_CLOSE(SEM_TYPE semid)
    {
        if(sem_close(semid) == -1)
        {
            fprintf(stderr, "sem_close() : %s\n", strerror(errno));
            return GT_FAIL;
        }
        return GT_OK;
    }
    static GT_STATUS SEM_WAIT(SEM_TYPE semid)
    {
        if(sem_wait(semid) == -1)
        {
            fprintf(stderr, "sem_wait() : %s\n", strerror(errno));
            return GT_FAIL;
        }
        return GT_OK;
    }
    static GT_STATUS SEM_POST(SEM_TYPE semid)
    {
        if(sem_post(semid) == -1)
        {
            fprintf(stderr, "sem_post() : %s\n", strerror(errno));
            return GT_FAIL;
        }
        return GT_OK;
    }
#endif
#ifdef NO_NAMED_SEM
#   define SEM_TYPE int
#if 0
    static GT_STATUS SEM_UNLINK(const char *name)
    {
        return GT_OK;
    }
#endif
    static GT_STATUS SEM_OPEN(const char *name, SEM_TYPE *semid)
    {
        return GT_OK;
    }
    static GT_STATUS SEM_CLOSE(SEM_TYPE semid)
    {
        return GT_OK;
    }
    static GT_STATUS SEM_WAIT(SEM_TYPE semid)
    {
        return GT_OK;
    }
    static GT_STATUS SEM_POST(SEM_TYPE semid)
    {
        return GT_OK;
    }
#endif

/*#define MM_DEBUG */

/************* Functions ******************************************************/
/************* Static Functions ***********************************************/

/*******************************************************************************
* findAdressesOfSections
*
* DESCRIPTION:
*       Find termination adress of the shared section into /proc/self/maps file.
*
* INPUTS:
*       sectPtr  - pointer to structure with leading adress of the section
*
* OUTPUTS:
*       sectPtr  - pointer to structure for leading and termination adress
*       of the section
*
* RETURNS:
*       GT_OK   - Operation succeeded
*       GT_FAIL - Operaton failed
*
* COMMENTS:
*       Uses SHARED_DATA_ADDR_CNS (fixed well-known address) as a key
*       for searching required section by start address. Only end address
*       is calculated here.
*
*******************************************************************************/
static GT_STATUS findAdressesOfSections(INOUT SECTION_STC *sectPtr)
{
    FILE *map;
    GT_8 buf[4096];
    char attr1, attr2, attr3;

    /* Init structure -fill by zeroes */
    if(memset(sectPtr, 0, sizeof(SECTION_STC)) == NULL)
    {
        fprintf(stderr, "memset() : %s\n", strerror(errno));
        return GT_FAIL;
    }

    /* Open special file with section's info.
     * This file is handled by Linux kernel. */
    if( (map = fopen("/proc/self/maps", "r")) == NULL)
    {
        fprintf(stderr, "fopen() : %s\n", strerror(errno));
        return GT_FAIL;
    }

    /* Read it line-by-line */
    while(fgets(buf, sizeof(buf), map) != NULL)
    {
        unsigned int start, done;
            
        /* Parse varables from line */
        sscanf(buf, "%x-%x %c%c%c", &start, &done, &attr1, &attr2, &attr3);

        /* Start address matches the key? */
        if(start == SHARED_DATA_ADDR_CNS)
        {
            sectPtr->startPtr = (GT_VOID *)start;
            sectPtr->donePtr  = (GT_VOID *)done;
        }
        /* !! NOTE - we merge consecutive sections 'cause compiler
            can split them into parts. 
            Warning: unrelated sections shouldn't be consecutive!!! */
        else if((GT_VOID *)start == sectPtr->donePtr)
        {
            /* Don't merge with guider section (with no any rights) */
            if(attr1!='-' || attr2!='-' || attr3!='-')
                sectPtr->donePtr  = (GT_VOID *)done;
        }
    }

    /* Close /proc/self/maps */
    if (fclose(map) == EOF)
    {
        fprintf(stderr, "fclose() : %s\n", strerror(errno));
        return GT_FAIL;
    }
    
    return GT_OK;
}

/*******************************************************************************
* mapFile
*
* DESCRIPTION:
*       Maps shared buffer into processes address space instead of BSS/DATA
*       section. Also the first client have to fulfill initial content of
*       buffer by data from BSS/DATA.
*
* INPUTS:
*       namePtr       - name of file
*       sectPtr       - pointer to the BSS/DATA section for which
*                       mapping is processed for
*       isFirstClient - is client is the first CPSS client(Enabler) on not.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - Operation succeeded
*       GT_FAIL - Operaton failed
*
* COMMENTS:
*       This is the core function of Shared Memory approach.
*
*******************************************************************************/
static GT_STATUS mapFile
(
    IN GT_8         *namePtr,
    IN SECTION_STC  *sectPtr,
    IN GT_BOOL      isFirstClient
)
{
    GT_VOID_PTR ptr;
    GT_32 fd;
    void* (*mmapPtr)(void *start, size_t length, int prot , int flags, int fd,
       off_t offset);
   
    if(isFirstClient)
    {
        /* Create shared buffer file */
        /* NOTE - here we use open('dev/shm/XXX') instaead of shm_open('XXX')
         * 'cause this function is likely not supported by ucLibC
        */
        fd = open(namePtr, O_RDWR | O_CREAT, 0666);
        if (fd < 0)
        {
            fprintf(stderr, "open() : %s\n", strerror(errno));
            return GT_FAIL;
        }
        
        /* Truncate shared buffer file to required length 
         * (to the actual section's length) */
        if(ftruncate(fd, sectPtr->donePtr - sectPtr->startPtr) == -1)
        {
            fprintf(stderr, "ftruncate() : %s\n", strerror(errno));
            return GT_FAIL;
        }

        /* Write original content of BSS/DATA section into shared buffer */
        if(write(fd, sectPtr->startPtr, sectPtr->donePtr - sectPtr->startPtr) == -1)
        {
            fprintf(stderr, "write() : %s\n", strerror(errno));
            return GT_FAIL;
        }
    }
    else /* Non-first client */
    {
        /* Just open already created buffer */
        fd = open(namePtr, O_RDWR, 0666);
        if(fd < 0)
        {
            if(errno == ENOENT)
            {
                fprintf(stderr, "open() : file %s not exist, run Enabler first\n", namePtr);
                return GT_FAIL;
            }
            fprintf(stderr, "open() : %s\n", strerror(errno));
            return GT_FAIL;
        }
    }
    
    /* force dynamic linker to resolve mmap address */
    mmap(0,0,0,0,0,0);

	/* save the mmap address for later usage */
    mmapPtr = mmap;

    /* Shutdown old (private) BSS/DATA section */
    if(munmap(sectPtr->startPtr, sectPtr->donePtr - sectPtr->startPtr) == -1)
    {
        fprintf(stderr, "munmap() : %s\n", strerror(errno));
        return GT_FAIL;
    }

    /* !! NOTE - here is the popentially danger place -
        we havn't access to detached section !! */
    
    /* !! NOTE - address of 'mmap' function should be already resolved to avoid access
     * to untached section. */
    ptr = mmapPtr(sectPtr->startPtr, sectPtr->donePtr - sectPtr->startPtr,
               PROT_READ | PROT_WRITE /* | PROT_EXEC */,
               MAP_FIXED | MAP_SHARED, fd, 0);

    if(!ptr)
    {
        fprintf(stderr, "mmap() : %s\n", strerror(errno));
        return GT_FAIL;
    }
    
    /* Shared file descriptor is not required for now. 
     * We can close it. 
     * !!! Note - we still can use the mapped section into that file. */
    if(close(fd) == -1)
    {
        fprintf(stderr, "close() : %s\n", strerror(errno));
        return GT_FAIL;
    }

    return GT_OK;
}

/************* Public Functions ***********************************************/

/*******************************************************************************
* shrMemUnlinkShmObj
*
* DESCRIPTION:
*       Unlink shared files and semaphore.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*   GT_OK   - Operation succeeded
*   GT_FAIL - Operaton failed
*
* COMMENTS:
*       This action is required for detect-first-client-by-file-existence
*       method.
*
*******************************************************************************/
#if 0
GT_STATUS shrMemUnlinkShmObj(GT_VOID)
{
    /* Remove named semaphore for clean-up */
    if (SEM_UNLINK(SHARED_MEMORY_SEM_CNS) != GT_OK)
        return GT_FAIL;

    /* Unlink shared buffer */
    if(unlink(SHARED_MEMORY_DATA_CNS) < 0)
    {
        if(errno != ENOENT && 0 != errno)
        {
            fprintf(stderr, "unlink() : %s %s\n", SHARED_MEMORY_DATA_CNS, strerror(errno));
            return GT_FAIL;
        }
    }
    
    return GT_OK;
}
#endif

/*******************************************************************************
* shrMemDoShmMap
*
* DESCRIPTION:
*       Do sharing data for all processes.
*
* INPUTS:
*       isFirstClient - is client is the first CPSS client (CPSS Enabler)
*       or not. 
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - Operation succeeded
*       GT_FAIL - Operaton failed
*
* COMMENTS:
*       Performs major shared memory initializations.
*       It searches BSS/DATA section and remaps it to shared space.
*
*******************************************************************************/
GT_STATUS shrMemDoShmMap(GT_BOOL isFirstClient)
{
    SEM_TYPE    semid;
    SECTION_STC map_section;

    /* Open named semaphore to guard remapping code from other clients. */
    if (SEM_OPEN(SHARED_MEMORY_SEM_CNS, &semid) != GT_OK)
        return GT_FAIL;

    /* Now searching for section we should remap.
     * Only one section with known begin address should be searched.
     * We have to know end address of it to map whole section. */
    if(findAdressesOfSections(&map_section) == GT_FAIL)
    {
        fprintf(stderr, "findAdressesOfSections() : error\n");
        goto closeSemAndFail;
    }

    /* Lock semaphore */
    if(SEM_WAIT(semid) != GT_OK)
        goto closeSemAndFail;

#ifdef MM_DEBUG
    /* Output sections BEFORE remapping. */
    shrMemPrintMapsDebugInfo();
#endif
    
    /* Map shared buffer into processes address space.
     * The first client also have to create it before mapping. */
    if(mapFile(SHARED_MEMORY_DATA_CNS, &map_section, isFirstClient) == GT_FAIL)
    {
        SEM_POST(semid);
        fprintf(stderr, "mapFile() : error\n");
        goto closeSemAndFail;
    }

    if(SEM_POST(semid) != GT_OK)
        goto closeSemAndFail;

    if(SEM_CLOSE(semid) != GT_OK)
        return GT_FAIL;


#ifdef MM_DEBUG
    shrMemPrintMapsDebugInfo();
#endif

    return GT_OK;

closeSemAndFail:
    SEM_CLOSE(semid);
    return GT_FAIL;

}


/*******************************************************************************
* shrMemPrintMapsDebugInfo
*
* DESCRIPTION:
*       Ouputs /proc/self/maps to stderr for debug purpose
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       Should be used only for debugging on SHARED_MEMORY=1
*
*******************************************************************************/
GT_VOID shrMemPrintMapsDebugInfo(GT_VOID)
{
    FILE* fp;
    int num;
    char buffer[4*1024];

    fp = fopen("/proc/self/maps", "r");
    num = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);
    fwrite(buffer, 1, num, stderr);
}

