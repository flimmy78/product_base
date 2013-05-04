/****************************************************
* cmdFS_RAM.c
*
* DESCRIPTION:
*       RAM cmdFS implementation
*
* DEPENDENCIES:
*
* COMMENTS:
*       This FS has no protection for simultaneous write:
*       there should be only one writer at time and no readers
*       when writer active
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/***** Include files ***************************************************/
#ifdef TEST
#  include <stdlib.h>
#  include <string.h>
#else
#include <cmdShell/common/cmdCommon.h>
#endif
#include <cmdShell/FS/cmdFS.h>

/***** Defines  ********************************************************/
#ifdef TEST
#  define CMD_FS_STRCMP(a,b)    strcmp(a,b)
#  define CMD_FS_STRDUP(a)      strdup(a)
#  define CMD_FS_STRNCPY(d,s,n) strncpy(d,s,n)
#  define CMD_FS_BZERO(p,l)     memset(p,0,l)
#  define CMD_FS_BCOPY(s,d,l)   memcpy(d,s,l)
#  define CMD_FS_MALLOC(l)      malloc(l)
#  define CMD_FS_FREE(l)        free(l)
#  define CMD_FS_TIME()         ((unsigned)time(NULL))
#else
#  define CMD_FS_STRCMP(a,b)    cmdOsStrCmp(a,b)
   char* CMD_FS_STRDUP(const char *s)
   {
       char *d = cmdOsMalloc(cmdOsStrlen(s)+1);
       if (d != NULL)
           cmdOsStrCpy(d,s);
       return d;
   }
   char* CMD_FS_STRNCPY(char *d, const char *s, int n)   
   {
       char *dPtr;
       if (d == NULL || s == NULL)
           return d;
       for(dPtr = d; *s && n > 0; s++, n--)
           *(dPtr++) = *s;
       *dPtr = 0;
       return d;
   }
#  define CMD_FS_BZERO(p,l)     cmdOsBzero((char*)(p),l)
#  define CMD_FS_BCOPY(s,d,l)   cmdOsMemCpy(d,s,l)
#  define CMD_FS_MALLOC(l)      cmdOsMalloc(l)
#  define CMD_FS_FREE(l)        cmdOsFree(l)
#  define CMD_FS_TIME()         ((unsigned)cmdOsTime())
#endif

#define CMD_FS_MAX_OPEN_FILES   10
#define CMD_FS_O_DIRECTORY      0x40000000

/***** Private Types ***************************************************/

typedef struct CMD_FS_CHUNK_STC {
    size_t                  chunksize;
    struct CMD_FS_CHUNK_STC *next;
    char                    data[1];
} CMD_FS_CHUNK_STC;

typedef struct CMD_FS_INODE_STC {
    char                    *name;
    CMD_FS_CHUNK_STC        *data;
    int                     nrefs; /* number of references */
    unsigned                ctime;
    unsigned                mtime;
    struct CMD_FS_INODE_STC *next;
} CMD_FS_INODE_STC;

typedef struct CMD_FS_FDESCR_STC {
    int                 flags;      /* open flags, 0 means not opened */
    CMD_FS_INODE_STC    *inode;
    int                 pos;        /* current position */
    CMD_FS_CHUNK_STC    *chunk;     /* current chunk */
    int                 chunkpos;   /* position in current chunk */
} CMD_FS_FDESCR_STC;

/***** Private Data ****************************************************/
static CMD_FS_INODE_STC *cmdFSdir = NULL;
static CMD_FS_FDESCR_STC    fdescr[CMD_FS_MAX_OPEN_FILES];
static const char *last_error = "";

#ifdef  CMDFS_RAM_EMBEDDED_DATA
#include "cmdFS_RAM_embed_files.inc"
#endif

/***** Private Functions ***********************************************/

/*******************************************************************************
* CMD_FS_CHECK_FD
*
* DESCRIPTION:
*       Check that fd is valid opened file descriptor
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       -1  - on error
*
* COMMENTS:
*
*******************************************************************************/
#define FD                  fdescr[fd]
#define CMD_FS_CHECK_FD() \
    if (fd < 0 || fd >= CMD_FS_MAX_OPEN_FILES) \
    { \
        last_error = "Wrong file descriptor"; \
        return -1; \
    } \
    if (FD.flags == 0) \
    { \
        last_error = "Bad file descriptor"; \
        return -1; \
    }

/*******************************************************************************
* cmdFS_RAM_lookupFile
*
* DESCRIPTION:
*       Scan directory linked list for given file
*
* INPUTS:
*       name    - filename
*
* OUTPUTS:
*       None
*
* RETURNS:
*       inode pointer
*       - NULL if error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_FS_INODE_STC* cmdFS_RAM_lookupFile(const char *name)
{
    CMD_FS_INODE_STC  *inode;

    for (inode = cmdFSdir; inode; inode = inode->next)
    {
        if (!CMD_FS_STRCMP(inode->name, name))
        {
            return inode;
        }
    }
    return NULL;
}

/*******************************************************************************
* cmdFS_RAM_newFileEntry
*
* DESCRIPTION:
*       Allocate inode for new file and link it into directory
*
* INPUTS:
*       name    - filename
*
* OUTPUTS:
*       None
*
* RETURNS:
*       inode pointer
*       - NULL if error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_FS_INODE_STC* cmdFS_RAM_newFileEntry(const char* name)
{
    CMD_FS_INODE_STC *inode;
    inode = cmdFSdir;
    if (inode)
    {
        while(inode->next)
            inode = inode->next;
        inode->next = (CMD_FS_INODE_STC*) CMD_FS_MALLOC(sizeof(*inode));
        inode = inode->next;
    } else {
        inode = cmdFSdir = (CMD_FS_INODE_STC*) CMD_FS_MALLOC(sizeof(*inode));
    }
    CMD_FS_BZERO(inode, sizeof(*inode));
    inode->name = CMD_FS_STRDUP(name);
    inode->ctime = CMD_FS_TIME();
    inode->mtime = inode->ctime;
    inode->nrefs = 1;
    return inode;
}

/*******************************************************************************
* cmdFS_RAM_truncate
*
* DESCRIPTION:
*       Truncate file - delete all data chunks
*
* INPUTS:
*       inode   - file inode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void cmdFS_RAM_truncate(CMD_FS_INODE_STC *inode)
{
    CMD_FS_CHUNK_STC *data;

    data = inode->data;
    while (data)
    {
        CMD_FS_CHUNK_STC *next = data->next;
        CMD_FS_FREE(data);
        data = next;
    }
    inode->data = NULL;
}

/*******************************************************************************
* cmdFS_RAM_allocChunk
*
* DESCRIPTION:
*       Allocate chunk for file data
*
* INPUTS:
*       size    - chunk size
*
* OUTPUTS:
*       None
*
* RETURNS:
*       chunk pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_FS_CHUNK_STC* cmdFS_RAM_allocChunk(int size)
{
    CMD_FS_CHUNK_STC *chunk;
    if (size == 0)
        return NULL;
    chunk = (CMD_FS_CHUNK_STC *)CMD_FS_MALLOC(sizeof(CMD_FS_CHUNK_STC)+size);
    chunk->chunksize = size;
    chunk->next = NULL;
    return chunk;
}

/*******************************************************************************
* cmdFS_RAM_dereference_inode
*
* DESCRIPTION:
*       Dereference inode: redude nrefs counter and destroy inode if
*       no mode references left
*
* INPUTS:
*       inode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       none
*
* COMMENTS:
*
*******************************************************************************/
static void cmdFS_RAM_dereference_inode(CMD_FS_INODE_STC *inode)
{
    inode->nrefs--;

    if (inode->nrefs > 0)
    {
        /* at least one referece left */
        return;
    }
    /* no more references */
    cmdFS_RAM_truncate(inode);
    CMD_FS_FREE(inode->name);
    CMD_FS_FREE(inode);
}

/***** Public Functions ************************************************/

/*******************************************************************************
* cmdFSinit
*
* DESCRIPTION:
*       Initialize cmdFS, initialize built-in files
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0   - on success
*
* COMMENTS:
*
*******************************************************************************/
int cmdFSinit(void)
{
    /* init built-in files */
#ifdef  CMDFS_RAM_EMBEDDED_DATA
    CMDFS_RAM_INIT_EMBEDDED();
#endif
    CMD_FS_BZERO(fdescr, sizeof(fdescr));
    return 0;
}

/*******************************************************************************
* cmdFSlastError
*
* DESCRIPTION:
*       Return string with last error description
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0   - on success
*
* COMMENTS:
*
*******************************************************************************/
const char* cmdFSlastError(void)
{
    return last_error;
}

/*******************************************************************************
* cmdFSopen
*
* DESCRIPTION:
*       Open and possible create a new file
*
* INPUTS:
*       name    - file name
*       flags   - open flags, see CMD_FS_O_*,
*                 like POSIX open()
*
* OUTPUTS:
*       None
*
* RETURNS:
*       file descriptor or < 0 if error
*
* COMMENTS:
*       read POSIX open() as reference
*
*******************************************************************************/
int cmdFSopen(const char* name, int flags)
{
    int fd;
    CMD_FS_INODE_STC  *inode;

    if (flags == 0 || (flags & CMD_FS_O_DIRECTORY))
    {
        last_error = "open(): wrong flags";
        return -1;
    }
    /* look for new fd */
    for (fd = 0; fd < CMD_FS_MAX_OPEN_FILES; fd++)
    {
        if (FD.flags == 0)
        {
            break;
        }
    }
    if (fd >= CMD_FS_MAX_OPEN_FILES)
    {
        last_error = "No enough descriptors";
        return -1;
    }
    /* unused file descriptor found. lock it first */
    FD.flags = -1; /* lock */

    inode = cmdFS_RAM_lookupFile(name);
    if (inode == NULL)
    {
        /* file not yet exists */
        if ((flags & CMD_FS_O_CREAT) == 0)
        {
            FD.flags = 0; /* unlock */
            last_error = "No such file";
            return -1;
        }
        if ((flags & CMD_FS_O_WRONLY) == 0)
        {
            FD.flags = 0; /* unlock */
            last_error = "Wrong open flags (O_CREAT w/o write)";
            return -1;
        }
        inode = cmdFS_RAM_newFileEntry(name);
        if (inode == NULL)
        {
            FD.flags = 0; /* unlock */
            last_error = "Failed to create new file (can't alloc)";
            return -1;
        }
    }
    else
    {
        /* file exists */
        if ((flags & CMD_FS_O_CREAT) && (flags & CMD_FS_O_EXCL))
        {
            FD.flags = 0; /* unlock */
            last_error = "File already exists";
            return -1;

        }
    }
    /* truncate file if O_TRUNC and writable */
    if ((flags & CMD_FS_O_TRUNC) && (flags & CMD_FS_O_WRONLY))
    {
        /* O_TRUNK and write allowed */
        cmdFS_RAM_truncate(inode);

    }

    /* initialize file descripttor */
    FD.flags = flags;
    FD.inode = inode;
    inode->nrefs++;
    FD.pos = 0;
    FD.chunk = inode->data;
    FD.chunkpos = 0;

    return fd;
}

/*******************************************************************************
* cmdFSclose
*
* DESCRIPTION:
*       Close a file descriptor
*
* INPUTS:
*       fd      - file descriptor
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0 if success, < 0 if error
*
* COMMENTS:
*       read POSIX close() as reference
*
*******************************************************************************/
int cmdFSclose(int fd)
{
    CMD_FS_CHECK_FD();

    if ((FD.flags & CMD_FS_O_DIRECTORY) == 0)
    {
        /* file */
        cmdFS_RAM_dereference_inode(FD.inode);
    }
    FD.flags = 0;
    return 0;
}

/*******************************************************************************
* cmdFSread
*
* DESCRIPTION:
*       read from a file descriptor
*
* INPUTS:
*       fd      - file descriptor
*       count   - number of bytes to read
*
* OUTPUTS:
*       buf     - read data to buffer started at this pointer
*
* RETURNS:
*       The number of bytes read or <0 of error
*
* COMMENTS:
*       read POSIX read() as reference
*
*******************************************************************************/
int cmdFSread(int fd, void *buf, int count)
{
    char    *optr = (char*)buf;
    int     nread = 0;

    CMD_FS_CHECK_FD();

    if (count < 0)
    {
        last_error = "Bad parameter: count";
        return -1;
    }
    if (buf == NULL)
    {
        last_error = "Bad parameter: buf";
        return -1;
    }
    if ((FD.flags & CMD_FS_O_RDONLY) == 0)
    {
        last_error = "File is not opened for reading";
        return -1;
    }
    if (FD.chunk == NULL)
    {
        /* eof */
        return 0;
    }
    while (count > 0)
    {
        int n;
        /* read from current chunk starting chunkpos till end of chunk
         * but no more than count bytes
         */
        n = FD.chunk->chunksize - FD.chunkpos;
        if (n > count)
            n = count;
        if (n)
        {
            CMD_FS_BCOPY(FD.chunk->data+FD.chunkpos, optr, n);
            FD.chunkpos += n;
            FD.pos += n;
            optr += n;
            nread += n;
            count -= n;
        }

        if (FD.chunkpos == (int)FD.chunk->chunksize)
        {
            /* current position is end of chunk */
            if (FD.chunk->next == NULL)
            {
                /* no more chunks, eof */
                return nread;
            }
            FD.chunk = FD.chunk->next;
            FD.chunkpos = 0;
        }
    }
    return nread;
}

/*******************************************************************************
* cmdFSwrite
*
* DESCRIPTION:
*       Write to a file descriptor
*
* INPUTS:
*       fd      - file descriptor
*       buf     - write from buffer started at this pointer
*       count   - number of bytes to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The number of bytes written or <0 of error
*
* COMMENTS:
*       read POSIX write() as reference
*
*******************************************************************************/
int cmdFSwrite(int fd, const void *buf, int count)
{
    const char  *iptr = (const char*)buf;
    int         nwritten = 0;

    CMD_FS_CHECK_FD();

    if (count < 0)
    {
        last_error = "Bad parameter: count";
        return -1;
    }
    if (buf == NULL)
    {
        last_error = "Bad parameter: buf";
        return -1;
    }
    if ((FD.flags & CMD_FS_O_WRONLY) == 0)
    {
        last_error = "File is not opened for writing";
        return -1;
    }

    if (FD.inode->data == NULL)
    {
        /* file is empty, create first chunk */
        FD.inode->data = cmdFS_RAM_allocChunk(count);
        if (FD.inode->data == NULL)
        {
            last_error = "cmdFSwrite(): failed to alloc";
            return -1;
        }
        FD.chunk = FD.inode->data;
        CMD_FS_BCOPY(buf, FD.chunk->data, count);
        FD.chunkpos = count;
        FD.pos = count;
        FD.inode->mtime = CMD_FS_TIME();
        return count;
    }

    if (FD.flags & CMD_FS_O_APPEND)
    {
        /* seek to the end */
        while (FD.chunk->next != 0)
        {
            FD.pos += FD.chunk->chunksize - FD.chunkpos;
            FD.chunk = FD.chunk->next;
            FD.chunkpos = 0;
        }
        FD.pos += FD.chunk->chunksize - FD.chunkpos;
        FD.chunkpos = FD.chunk->chunksize;
    }

    while (count > 0)
    {
        int n;
        if (FD.chunkpos == (int)FD.chunk->chunksize)
        {
            /* current position is end of chunk */
            if (FD.chunk->next == NULL)
            {
                /* eof, allocate chunk for rest of data */
                FD.chunk->next = cmdFS_RAM_allocChunk(count);
                if (FD.chunk->next == NULL)
                {
                    last_error = "cmdFSwrite(): failed to alloc";
                    return count;
                }
                FD.chunk = FD.chunk->next;
                CMD_FS_BCOPY(iptr, FD.chunk->data, count);
                FD.chunkpos = count;
                FD.pos += count;
                nwritten += count;
                FD.inode->mtime = CMD_FS_TIME();
                break;
            }
            /* get next chunk */
            FD.chunk = FD.chunk->next;
            FD.chunkpos = 0;
        }
        /* write to current chunk starting chunkpos till end of chunk
         * but no more than count bytes
         */
        n = FD.chunk->chunksize - FD.chunkpos;
        if (n > count)
            n = count;
        if (n)
        {
            CMD_FS_BCOPY(iptr, FD.chunk->data+FD.chunkpos, n);
            FD.inode->mtime = CMD_FS_TIME();
            FD.chunkpos += n;
            FD.pos += n;
            iptr += n;
            nwritten += n;
            count -= n;
        }
    }
    return nwritten;
}


/*******************************************************************************
* cmdFSlseek
*
* DESCRIPTION:
*       reposition read/write file offset
*
* INPUTS:
*       fd      - file descriptor
*       offset  - 
*       whence  - one of
*                 CMD_FS_SEEK_SET   - The offset is set to offset bytes
*                 CMD_FS_SEEK_CUR   - The offset is set to current location
*                                     plus offset bytes
*                 CMD_FS_SEEK_END   - The offset is set to size of the file
*                                     PLUS offset bytes
*
* OUTPUTS:
*       None
*
* RETURNS:
*       the resulting offset location as measured in bytes from the
*                 beginning of the file.
*       -1 if error
*
* COMMENTS:
*       read POSIX lseek() as reference
*
*******************************************************************************/
int cmdFSlseek(int fd, int offset, int whence)
{
    CMD_FS_CHECK_FD();
    last_error = "Not implemented";
    return -1;
}






/*******************************************************************************
* cmdFSstat
*
* DESCRIPTION:
*       get file status
*
* INPUTS:
*       name    - file name
*
* OUTPUTS:
*       buf     - pointer to stat structure
*
* RETURNS:
*       0 if success
*       -1 if error
*
* COMMENTS:
*       read POSIX stat() as reference
*
*******************************************************************************/
int cmdFSstat(const char* name, CMD_FS_STAT_STC *buf)
{
    CMD_FS_INODE_STC  *inode;
    CMD_FS_CHUNK_STC *chunk;

    inode = cmdFS_RAM_lookupFile(name);
    if (!inode)
    {
        last_error = "File not found";
        return -1;
    }
    CMD_FS_BZERO(buf, sizeof(*buf));
    buf->st_mode = CMD_FS_S_IFREG; /* regular file */
    buf->st_ctime = inode->ctime;
    buf->st_mtime = inode->mtime;

    for (chunk = inode->data; chunk; chunk = chunk->next)
    {
        buf->st_size += chunk->chunksize;
    }
    return 0;
}


/*******************************************************************************
* cmdFSunlink
*
* DESCRIPTION:
*       delete a name and possibly the file it refers to
*
* INPUTS:
*       name    - file name
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0   - on success
*
* COMMENTS:
*       read POSIX unlink() as reference
*
*******************************************************************************/
int cmdFSunlink(const char* name)
{
    CMD_FS_INODE_STC *inode;
    
    inode = cmdFS_RAM_lookupFile(name);
    if (!inode)
    {
        last_error = "File not found";
        return -1;
    }

    /* remove from dir inode chain */
    if (cmdFSdir == inode)
    {
        cmdFSdir = inode->next;
    } else {
        CMD_FS_INODE_STC *d = cmdFSdir;
        while (d->next != inode)
            d = d->next;
        d->next = inode->next;
    }
    cmdFS_RAM_dereference_inode(inode);
    return 0;
}

/*******************************************************************************
* cmdFSopendir
*
* DESCRIPTION:
*       open a directory
*
* INPUTS:
*       name    - directory name
*                 (will beignored in current implementation)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       directory file descriptor or <0 if error
*
* COMMENTS:
*
*******************************************************************************/
int cmdFSopendir(const char *name)
{
    int fd;

    /* look for new fd */
    for (fd = 0; fd < CMD_FS_MAX_OPEN_FILES; fd++)
    {
        if (FD.flags == 0)
        {
            break;
        }
    }
    if (fd >= CMD_FS_MAX_OPEN_FILES)
    {
        last_error = "No enough descriptors";
        return -1;
    }
    /* unused file descriptor found. */
    FD.flags = CMD_FS_O_DIRECTORY;
    FD.inode = cmdFSdir;
    return fd;
}

/*******************************************************************************
* cmdFSclosedir
*
* DESCRIPTION:
*       close a directory
*
* INPUTS:
*       fd      - directory file descriptor
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0 if success or <0 if error
*
* COMMENTS:
*
*******************************************************************************/
int cmdFSclosedir(int fd)
{
    return cmdFSclose(fd);
}


/*******************************************************************************
* cmdFSreaddir
*
* DESCRIPTION:
*       read a directory entry
*
* INPUTS:
*       fd      - directory file descriptor
*
* OUTPUTS:
*       dirPtr  - pointer to directory entry structure
*
* RETURNS:
*       1 On success
*       0 On end of directory
*       <0 On error
*
* COMMENTS:
*
*******************************************************************************/
int cmdFSreaddir(int fd, CMD_FS_DIRENT_STC *dirPtr)
{
    CMD_FS_CHECK_FD();

    if ((FD.flags & CMD_FS_O_DIRECTORY) == 0)
    {
        last_error = "descriptor is not a directory";
        return -1;
    }
    if (dirPtr == NULL)
    {
        last_error = "Bad parameter: dirPtr";
        return -1;
    }
    if (FD.inode == NULL)
    {
        return 0;
    }
    CMD_FS_BZERO(dirPtr, sizeof(*dirPtr));
    CMD_FS_STRNCPY(dirPtr->d_name, FD.inode->name, CMD_FS_NAME_MAX);
    FD.inode = FD.inode->next;
    return 1;
}

#ifdef TEST
int main()
{
    int fd;
    char buf[100];
    int k;

    fd = cmdFSopen("test", CMD_FS_O_WRONLY); /* failure */
    fd = cmdFSopen("test", CMD_FS_O_WRONLY|CMD_FS_O_CREAT);
    k = cmdFSwrite(fd, "12345", 5);
    k = cmdFSwrite(fd, "67890abc", 8);
    cmdFSclose(fd);
    fd = cmdFSopen("test", CMD_FS_O_RDWR);
    k = cmdFSread(fd, buf, 3);
    k = cmdFSwrite(fd, "xyzkl", 5);
    k = cmdFSread(fd, buf, 10); /* 3 bytes read */
    k = cmdFSread(fd, buf, 10); /* 0 bytes: eof */
    cmdFSclose(fd);
    return 0;
}
#endif
