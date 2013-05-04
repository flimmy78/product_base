/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdExtServices.h
*
* DESCRIPTION:
*       Common definition and APIs for Commander external services.
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#ifndef __cmdExtServices_h_
#define __cmdExtServices_h_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef PSS_PRODUCT

#include <gtOs/gtGenTypes.h>
#include <gtOs/gtOsIo.h>
#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsStr.h>
#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsTask.h>
#include <gtOs/gtOsExc.h>
#include <gtOs/gtOsGen.h>
#include <gtOs/gtOsInet.h>
#include <gtOs/gtOsIntr.h>
#include <gtOs/gtOsRand.h>
#include <gtOs/gtOsTimer.h>
#include <gtOs/gtOsVersion.h>
#include <gtStack/gtOsSocket.h>
#include <gtStack/gtOsSelect.h>
#include <appDemo/boardConfig/appDemoBoardConfig.h>

int cmdPrintf(const GT_CHAR* format, ...);

#define cmdOsNtohl                  osNtohl
#define cmdOsHtonl                  osHtonl
#define cmdOsNtohs                  osNtohs
#define cmdOsHtons                  osHtons
#define cmdOsInetNtoa               osInetNtoa

#define cmdOsIntEnable              osIntEnable
#define cmdOsIntDisable             osIntDisable
#define cmdOsSetIntLockUnlock       osSetIntLockUnlock
#define cmdOsInterruptConnect       osInterruptConnect

#define CPSS_OS_BIND_STDOUT_FUNC_PTR OS_BIND_STDOUT_FUNC_PTR
#define cmdOsBindStdOut             osBindStdOut
#define cmdOsPrintf                 cmdPrintf
#define cmdOsSprintf                osSprintf
#define cmdOsPrintSync              osPrintSync
#define cmdOsGets                   osGets

#define cmdOsBzero                  osBzero
#define cmdOsMemSet                 osMemSet
#define cmdOsMemCpy                 osMemCpy
#define cmdOsMemCmp                 osMemCmp
#define cmdOsStaticMalloc           osStaticMalloc
#define cmdOsMalloc                 osMalloc
#define cmdOsRealloc                osRealloc
#define cmdOsFree                   osFree
#define cmdOsCacheDmaMalloc         osCacheDmaMalloc
#define cmdOsCacheDmaFree           osCacheDmaFree
#define cmdOsPhy2Virt               osPhy2Virt
#define cmdOsVirt2Phy               osVirt2Phy

#define cmdOsRand                   osRand
#define cmdOsSrand                  osSrand

#define CPSS_OS_SEM_WAIT_FOREVER_CNS OS_WAIT_FOREVER
#define CPSS_OS_SEM_NO_WAIT_CNS     OS_NO_WAIT
#define CPSS_OS_SEMB_EMPTY_E        OS_SEMB_EMPTY
#define CPSS_OS_SEMB_FULL_E         OS_SEMB_FULL
#define cmdOsMutexCreate            osMutexCreate
#define cmdOsMutexDelete            osMutexDelete
#define cmdOsMutexLock              osMutexLock
#define cmdOsMutexUnlock            osMutexUnlock

#define cmdOsSigSemBinCreate        osSemBinCreate
#define cmdOsSigSemDelete           osSemDelete
#define cmdOsSigSemWait             osSemWait
#define cmdOsSigSemSignal           osSemSignal

#define cmdOsStrlen                 osStrlen
#define cmdOsStrCpy                 osStrCpy
#define cmdOsStrChr                 osStrChr
#define cmdOsStrCmp                 osStrCmp
#define cmdOsStrCat                 osStrCat
#define cmdOsStrNCat                osStrNCat
#define cmdOsToUpper                osToUpper
#define cmdOsStrTo32                osStrTo32
#define cmdOsStrToU32               osStrToU32

#define cmdOsTimerWkAfter           osTimerWkAfter
#define cmdOsTickGet                osTickGet
#define cmdOsTime                   osTime

#define cmdOsTaskCreate             osTaskCreate
#define cmdOsTaskDelete             osTaskDelete
#define cmdOsTaskLock               osTaskLock
#define cmdOsTaskUnLock             osTaskUnLock

#define cmdOsTaskGetSelf            osTaskGetSelf
#define cmdOsSetTaskPrior           osSetTaskPrior

#define cmdOsSocketTcpCreate        osSocketTcpCreate
#define cmdOsSocketUdpCreate        osSocketUdpCreate
#define cmdOsSocketTcpDestroy       osSocketTcpDestroy
#define cmdOsSocketUdpDestroy       osSocketUdpDestroy
#define cmdOsSocketCreateAddr       osSocketCreateAddr
#define cmdOsSocketDestroyAddr      osSocketDestroyAddr
#define cmdOsSocketBind             osSocketBind
#define cmdOsSocketListen           osSocketListen
#define cmdOsSocketAccept           osSocketAccept
#define cmdOsSocketConnect          osSocketConnect
#define cmdOsSocketSetNonBlock      osSocketSetNonBlock
#define cmdOsSocketSetBlock         osSocketSetBlock
#define cmdOsSocketSend             osSocketSend
#define cmdOsSocketSendTo           osSocketSendTo
#define cmdOsSocketRecv             osSocketRecv
#define cmdOsSocketRecvFrom         osSocketRecvFrom
#define cmdOsSocketSetSocketNoLinger osSocketSetSocketNoLinger
#define cmdOsSocketExtractIpAddrFromSocketAddr osSocketExtractIpAddrFromSocketAddr
#define cmdOsSocketGetSocketAddrSize osSocketGetSocketAddrSize
#define cmdOsSocketShutDown         osSocketShutDown

#define cmdOsSelectCreateSet        osSelectCreateSet
#define cmdOsSelectEraseSet         osSelectEraseSet
#define cmdOsSelectZeroSet          osSelectZeroSet
#define cmdOsSelectAddFdToSet       osSelectAddFdToSet
#define cmdOsSelectClearFdFromSet   osSelectClearFdFromSet
#define cmdOsSelectIsFdSet          osSelectIsFdSet
#define cmdOsSelectCopySet          osSelectCopySet
#define cmdOsSelect                 osSelect
#define cmdOsSocketGetSocketFdSetSize osSocketGetSocketFdSetSize


#define cmdOsPoolCreatePool         osPoolCreatePool
#define cmdOsPoolCreateDmaPool      osPoolCreateDmaPool
#define cmdOsPoolReCreatePool       osPoolReCreatePool
#define cmdOsPoolDeletePool         osPoolDeletePool
#define cmdOsPoolGetBuf             osPoolGetBuf
#define cmdOsPoolFreeBuf            osPoolFreeBuf
#define cmdOsPoolGetBufSize         osPoolGetBufSize
#define cmdOsPoolExpandPool         osPoolExpandPool
#define cmdOsPoolGetBufFreeCnt      osPoolGetBufFreeCnt
#define cmdOsPoolPrintStats         osPoolPrintStats

#define cmdAppShowBoardsList appDemoShowBoardsList

#else /* if cpss only */

/* external services support */
#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <cpss/extServices/cpssExtServices.h>
#include <cpss/generic/events/cpssGenEventUnifyTypes.h>
#include <cpss/generic/events/cpssGenEventRequests.h>
#include <cpss/generic/cpssTypes.h>

#ifdef CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>
    #include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#endif /*CHX_FAMILY*/

#ifdef EXMXPM_FAMILY
    #include <cpss/exMxPm/exMxPmGen/networkIf/cpssExMxPmNetIfTypes.h>
    #include <cpss/exMxPm/exMxPmGen/extTcam/cpssExMxPmExternalTcam.h>
#endif /*EXMXPM_FAMILY*/


/* check that not define by PSS/Cpss that already set types */
#ifndef __gtStackTypes
/* WA to avoid 'redefenitions' with the simulation */
#define __gtStackTypes

/* Data block size generic type */
typedef int  GT_SIZE, *GT_SIZE_PTR;
/* File descriptor generic type */
typedef int GT_FD ;
#endif /*!__gtStackTypes*/

#ifndef __gtOsSocket_h

/* WA to avoid 'redefenitions' with the simulation */
#define __gtOsSocket_h

#define GT_SOCKET_DEFAULT_SIZE    -1

/* Socket file descriptor and socket port generic type */
typedef int GT_SOCKET_FD ;
/* Socket file descriptor and socket port generic type */
typedef int GT_SOCKET_PORT ;

/*******************************************************************************
 * Typedef:     struct   SOCKET_LINGER
 *
 * Description: Structure sent to setsockopt in order to set the SO_LINGER
 *              option for the socket
 *
 * Fields :
 *       lOnOff     - enable/disable the linger option when closing socket.
 *       lLinger    - indicates the amount of time to linger.
 * Comments:
 *
 *******************************************************************************/
typedef struct
{
    GT_32   lOnOff;
    GT_32   lLinger;
}SOCKET_LINGER;



/*******************************************************************************
 * Typedef:     enum   SOCKET_SHUTDOWN_OPERATION
 *
 * Description: enum that describes what types of operation will no longer be allowed
 *
 * Fields :
 *       lOnOff     - enable/disable the linger option when closing socket.
 *       lLinger    - indicates the amount of time to linger.
 * Comments:
 *
 *******************************************************************************/
typedef enum
{
    SOCKET_SHUTDOWN_CLOSE_INPUT,                  /* receives disallowed */
    SOCKET_SHUTDOWN_CLOSE_OUTPUT,                 /* sends disallowed */
    SOCKET_SHUTDOWN_CLOSE_ALL                     /* sends and receives disallowed */
}SOCKET_SHUTDOWN_OPERATION;



/* Define socket's errors */
#define GT_SOCKET_ERROR_BASE    0x30000
/* Sockets definitions of regular Berkeley error constants */
#define GT_SOCKET_EAGAIN        (GT_SOCKET_ERROR_BASE + 11)
#define GT_SOCKET_EINTR         (GT_SOCKET_ERROR_BASE + 4)
#define GT_SOCKET_EINVAL        (GT_SOCKET_ERROR_BASE + 22)
#define GT_SOCKET_EMSGSIZE      (GT_SOCKET_ERROR_BASE + 36)
#define GT_SOCKET_EISCONN       (GT_SOCKET_ERROR_BASE + 56)
#define GT_SOCKET_SHUTDOWN      (GT_SOCKET_ERROR_BASE + 58)
#define GT_SOCKET_EWOULDBLOCK   (GT_SOCKET_ERROR_BASE + 70)
#define GT_SOCKET_EINPROGRESS   (GT_SOCKET_ERROR_BASE + 68)
#define GT_SOCKET_EALREADY      (GT_SOCKET_ERROR_BASE + 69)

#endif /*__gtOsSocket_h*/

#ifndef __gtOsTaskh
typedef GT_U32  GT_TASK;
#endif /*__gtOsTaskh*/


/*******************************************************************************
* osSocketTcpCreate()
*
* DESCRIPTION:
*       Create a socket from TCP type
*
* INPUTS:
*       sockSize - the size of the receive & send socket buffer. -1 indicates
*                  the default size.
*
* OUTPUTS:
*
* RETURNS:
*       Valid file descriptor on success
*       -1 On fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_SOCKET_FD (*CMD_OS_SOCKET_TCP_CREATE_FUN) (
    IN  GT_32 sockSize
);


/*******************************************************************************
* osSocketUdpCreate()
*
* DESCRIPTION:
*       sockSize - the size of the receive & send socket buffer. -1 indicates
*                  the default size.*
* INPUTS:
*       None
*
* OUTPUTS:
*
* RETURNS:
*       Valid file descriptor on success
*       -1 On fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_SOCKET_FD (*CMD_OS_SOCKET_UDP_CREATE_FUN) (
    IN  GT_32 sockSize
);


/*******************************************************************************
* osSocketTcpDestroy()
*
* DESCRIPTION:
*       Destroy a TCP socket
*
* INPUTS:
*       socketFd - Socket descriptor
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*CMD_OS_SOCKET_TCP_DESTROY_FUN)
(
    IN GT_SOCKET_FD   socketFd
);


/*******************************************************************************
* osSocketUdpDestroy()
*
* DESCRIPTION:
*       Destroy a UDP socket
*
* INPUTS:
*       socketFd - Socket descriptor
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*CMD_OS_SOCKET_UDP_DESTROY_FUN)
(
    IN  GT_SOCKET_FD   socketFd
);


/*******************************************************************************
* osSocketCreateAddr()
*
* DESCRIPTION:
*       Build a TCP/IP address to be used across all other functions
*
* INPUTS:
*       ipAddr       - String represents a dotted decimal IP address, such as
*                      "10.2.40.10"
*       ipPort       - IP Port
*
* OUTPUTS:
*       sockAdd      - Pointer to a buffer pointer sent by the caller that is
*                      used as the address. The caller is unaware of what is
*                      the sockAddr size and type.THe function allocates the
*                      required size for the structure
*       sockAddrLen  - Length of the size used in sockAdd
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*CMD_OS_SOCKET_CREATE_ADDR_FUN)
(
    IN  GT_CHAR *        ipAddr,
    IN  GT_SOCKET_PORT   ipPort,
    IN  GT_VOID **       sockAddr,
    IN  GT_SIZE *        sockAddrLen
);


/*******************************************************************************
* osSocketDestroyAddr()
*
* DESCRIPTION:
*       Destroy a TCP/IP address object built via  osSocketBuildAddr()
*
* INPUTS:
*       sockAdd      - Pointer to a buffer pointer sent by the caller that is
*                      used as the address. The caller is unaware of what is
*                      the sockAddr size and type.
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
typedef GT_VOID (*CMD_OS_SOCKET_DESTROY_ADDR_FUN)
(
    IN  GT_VOID *       sockAddr
);


/*******************************************************************************
* osSocketBind()
*
* DESCRIPTION:
*       Bind a name to a TCP or UDP socket
*
* INPUTS:
*       socketFd    - Socket descriptor
*       sockAddr    - A pointer to the local socket address structure,
*                     probably filled out by osSocketBuildAddr.The caller
*                     is unaware of what is the sockAddr type.
*       sockAddrLen - Length of socketAddr
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*CMD_OS_SOCKET_BIND_FUN)
(
    IN  GT_SOCKET_FD     socketFd ,
    IN  GT_VOID *        sockAddr,
    IN  GT_SIZE          sockAddrLen
);


/*******************************************************************************
* osSocketListen()
*
* DESCRIPTION:
*       Listen for new connections on a TCP socket
*
* INPUTS:
*       socketFd       - Socket descriptor
*       maxConnections - Number of connections to queue
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail
*
* COMMENTS:
*       Used only for TCP connection oriented sockets
*
*******************************************************************************/
typedef GT_STATUS (*CMD_OS_SOCKET_LISTEN_FUN)
(
    IN  GT_SOCKET_FD     socketFd ,
    IN  GT_SIZE          maxConnections
);


/*******************************************************************************
* osSocketAccept()
*
* DESCRIPTION:
*       Extracts the first TCP connection request on the queue of pending
*       connections and creates a new connected socket.
*
* INPUTS:
*       socketFd    - Socket descriptor
*
* OUTPUTS:
*       sockAddr    - A pointer to a socket address structure, represents the
*                     client (target) address that requests the connection.
*                     If the server is not interested in the Client's protocol
*                     information, a NULL should be provided.
*       sockAddrLen - A value-result parameter, initialized to the size of
*                     the allocated buffer sockAddr, and modified on return
*                     to indicate the actual size of the address stored there.
*                     If the server is not interested in the Client's protocol
*                     information, a NULL should be provided.
*
* RETURNS:
*       socketFd - Socket descriptor
*
* COMMENTS:
*       Used only for TCP connection oriented sockets
*
*******************************************************************************/
typedef GT_SOCKET_FD (*CMD_OS_SOCKET_ACCEPT_FUN)
(
    IN  GT_SOCKET_FD     socketFd,
    OUT  GT_VOID *        sockAddr,
    OUT  GT_SIZE *        sockAddrLen
);


/*******************************************************************************
* osSocketConnect()
*
* DESCRIPTION:
*       Connect socket to a remote address:
*       For UDP sockets, specifies the address to which datagrams are sent by
*       default, and the only address from which datagrams are received. For
*       TCP sockets, used by the TCP client that attempts to make a connection
*       to the TCP server.
*
* INPUTS:
*       socketFd    - Socket descriptor
*       sockAddr    - A pointer to a socket address structure, represents the
*                     remote address to connect to. For TCP, it is the server
*                     address.
*       sockAddrLen - Length of sockAddr
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_STATUS (*CMD_OS_SOCKET_CONNECT_FUN)
(
    IN  GT_SOCKET_FD     socketFd,
    IN  GT_VOID *        sockAddr,
    IN  GT_SIZE          sockAddrLen
);


/*******************************************************************************
* osSocketSetNonBlock()
*
* DESCRIPTION:
*       Set a socket option to be non-blocked
*
* INPUTS:
*       socketFd - Socket descriptor
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*CMD_OS_SOCKET_SET_NON_BLOCK_FUN)
(
    IN  GT_SOCKET_FD     socketFd
);


/*******************************************************************************
* osSocketSetBlock()
*
* DESCRIPTION:
*       Set a socket option to be blocked
*
* INPUTS:
*       socketFd    - Socket descriptor
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*CMD_OS_SOCKET_SET_BLOCK_FUN)
(
    IN  GT_SOCKET_FD     socketFd
);


/*******************************************************************************
* osSocketSend()
*
* DESCRIPTION:
*       Send data to a TCP socket
*
* INPUTS:
*       socketFd    - Socket descriptor
*       buff        - Buffer to send
*       buffLen     - Max length to send
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Number of bytes sent
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_SIZE (*CMD_OS_SOCKET_SEND_FUN)
(
    IN  GT_SOCKET_FD   socketFd ,
    IN  GT_VOID *      buff,
    IN  GT_SIZE        buffLen
);


/*******************************************************************************
* osSocketSendTo()
*
* DESCRIPTION:
*       Send data to a UDP socket
*
* INPUTS:
*       socketFd    - Socket descriptor
*       buff        - Buffer to send
*       buffLen     - Max length to send
*       sockAddr    - A pointer to the targent socket address structure,
*                     probably filled out by osSocketBuildAddr.  The caller
*                     is unaware of what is the sockAddr type.
*       sockAddrLen - Length of socketAddr
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Number of bytes sent
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_SIZE (*CMD_OS_SOCKET_SEND_TO_FUN)
(
    IN  GT_SOCKET_FD   socketFd ,
    IN  GT_VOID *      buff,
    IN  GT_SIZE        buffLen,
    IN  GT_VOID *      sockAddr,
    IN  GT_SIZE        sockAddrLen
);


/*******************************************************************************
* osSocketRecv()
*
* DESCRIPTION:
*       Receive data from a TCP socket
*
* INPUTS:
*       socketFd    - Socket descriptor
*       buff        - Buffer to receive
*       buffLen     - Max length to send
*       removeFlag  - Flag indicate if to remove or no remove the data from
*                     the socket buffer
*
* OUTPUTS:
*
* RETURNS:
*       Number of bytes sent
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_SIZE (*CMD_OS_SOCKET_RECV_FUN)
(
    IN  GT_SOCKET_FD   socketFd,
    IN  GT_VOID *      buff,
    IN  GT_SIZE        buffLen,
    IN  GT_BOOL        removeFlag
);


/*******************************************************************************
* osSocketRecvFrom()
*
* DESCRIPTION:
*       Receive data from a UDP socket
*
* INPUTS:
*       socketFd    - Socket descriptor
*       buff        - Buffer to send
*       buffLen     - Max length to send
*       removeFlag  - Flag indicate if to remove or no remove the data from
*                     the socket buffer
*
* OUTPUTS:
*       sockAddr    - A pointer to a socket address structure, represents
*                     the source (target) address that has sent the packet.
*                     the caller can then compare the address later.
*                     If this is NULL, this information will not be provided.
*       sockAddrLen - A value-result parameter, initialized to the size of
*                     the allocated buffer sockAddr, and modified on return
*                     to indicate the actual size of the address stored there.
*
* RETURNS:
*       Number of bytes sent
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_SIZE (*CMD_OS_SOCKET_RECV_FROM_FUN)
(
    IN  GT_SOCKET_FD   socketFd ,
    IN  GT_VOID *      buff,
    IN  GT_SIZE        buffLen,
    IN  GT_BOOL        removeFlag,
    OUT GT_VOID *      sockAddr,
    OUT GT_SIZE *      sockAddrLen
);


/*******************************************************************************
* osSocketSetSocketNoLinger()
*
* DESCRIPTION:
*       Set the socket option to be no linger when closing connection.
*
* INPUTS:
*       socketFd    - Socket descriptor
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*CMD_OS_SOCKET_SET_SOCKET_NO_LINGER_FUN)
(
    IN  GT_SOCKET_FD     socketFd
);

/*******************************************************************************
* osSocketExtractIpAddrFromSocketAddr()
*
* DESCRIPTION:
*       Extracts the IP address from sockaddr_in structure.
*
* INPUTS:
*       sockAddr    - pointer to sockaddr_in.
*       ipAddr      - pointer to address in GT_U32 format to be filled by the
*                     routine.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*CMD_OS_SOCKET_EXTRACT_IP_ADDR_FROM_SOCKET_ADDR_FUN)
(
    IN  GT_VOID*   sockAddr,
    OUT GT_U32*    ipAddr
);

/*******************************************************************************
* osSocketGetSocketAddrSize()
*
* DESCRIPTION:
*       Returns the size of sockaddr_in.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       sockAddrSize - to be filled with sockaddr_in size.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail
*
* COMMENTS:
*       USERS THAT IMPLEMENTS THEIR OWN OS LAYER CAN RETURN SIZE = 0.
*
*******************************************************************************/
typedef GT_STATUS (*CMD_OS_SOCKET_GET_SOCKET_ADDR_SIZE_FUN)
(
    OUT GT_U32*    sockAddrSize
);



/*******************************************************************************
* osSocketShutDown ()
*
* DESCRIPTION:
*       This routine shuts down all, or part, of a connection-based socket.
*
* INPUTS:
*       socketFd    - Socket descriptor
*       operation   - 0 = receives disallowed
*                     1 = sends disallowed
*                     2 = sends and disallowed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail
*
* COMMENTS:
*
*
*******************************************************************************/
typedef GT_STATUS (*CMD_OS_SOCKET_SHUT_DOWN_FUN)
(
    IN GT_SOCKET_FD   socketFd ,
    IN SOCKET_SHUTDOWN_OPERATION operation
);

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*******************************************************************************
* osSelectCreateSet()
*
* DESCRIPTION:
*       Create a set of file descriptors for the select function
*
* INPUTS:
*       none
*
* OUTPUTS:
*       none
*
* RETURNS:
*       Pointer to the set. If unable to create, returns null. Note that the
*       pointer is from void type.
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_VOID * (*CMD_OS_SELECT_CREATE_SET_FUN)
(
    GT_VOID
);


/*******************************************************************************
* osSelectEraseSet()
*
* DESCRIPTION:
*       Erase (delete) a set of file descriptors
*
* INPUTS:
*       set - Pointer to the set.
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
typedef GT_VOID (*CMD_OS_SELECT_ERASE_SET_FUN)
(
    IN GT_VOID *  set
);


/*******************************************************************************
* osSelectZeroSet()
*
* DESCRIPTION:
*       Zeros a set of file descriptors
*
* INPUTS:
*       set - Pointer to the set.
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
typedef GT_VOID (*CMD_OS_SELECT_ZERO_SET_FUN)
(
    INOUT GT_VOID *  set
);


/*******************************************************************************
* osSelectAddFdToSet()
*
* DESCRIPTION:
*       Add a file descriptor to a specific set
*
* INPUTS:
*       set - Pointer to the set
*       fd  - A file descriptor
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
typedef GT_VOID (*CMD_OS_SELECT_ADD_FD_TO_SET_FUN)
(
    INOUT GT_VOID *  set ,
    IN GT_FD      fd
);


/*******************************************************************************
* osSelectClearFdFromSet()
*
* DESCRIPTION:
*       Remove (clear) a file descriptor from a specific set
*
* INPUTS:
*       set - Pointer to the set
*       fd  - A file descriptor
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
typedef GT_VOID (*CMD_OS_SELECT_CLEAR_FD_FROM_SET_FUN)
(
    INOUT GT_VOID *  set ,
    IN GT_FD      fd
);


/*******************************************************************************
* osSelectIsFdSet()
*
* DESCRIPTION:
*       Test if a specific file descriptor is set in a set
*
* INPUTS:
*       set - Pointer to the set
*       fd  - A file descriptor
*
* OUTPUTS:
*    None
*
* RETURNS:
*       GT_TRUE  (non zero) if set , returned as unsigned int
*       GT_FALSE (zero) if not set , returned as unsigned int
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_BOOL (*CMD_OS_SELECT_IS_FD_SET_FUN)
(
    INOUT GT_VOID *  set ,
    IN GT_FD      fd
);


/*******************************************************************************
* osSelectCopySet()
*
* DESCRIPTION:
*       Duplicate sets (require 2 pointers for sets)
*
* INPUTS:
*       srcSet - Pointer to source set
*       dstSet - Pointer to destination set
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Mone
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_VOID (*CMD_OS_SELECT_COPY_SET_FUN)
(
    IN GT_VOID *  srcSet ,
    OUT GT_VOID *  dstSet
);


/*******************************************************************************
* osSelect()
*
* DESCRIPTION:
*       OS Dependent select function
*
* INPUTS:
*       width       - The highest-numbered descriptor in any of the next three
*                     sets + 1 (if zero, The default length will be taken)
*       readSet     - Pointer to a read operation  descriptor set
*       writeSet    - Pointer to a write operation descriptor set
*       exceptionSet- Pointer to an exception descriptor set (not supported in
*                     all OS, such as VxWorks)
*       timeOut     - Maximum time to wait on in milliseconds. Sending a
*                     negative value will block indefinitely. Zero value cause
*                     no block
*
* OUTPUTS:
*       According to posix, all files descriptors will be zeroed , and only
*       bits that represents file descriptors which are ready will be set.
*
* RETURNS:
*       On success, returns the number of descriptors contained in the
*       descriptor sets,  which  may be zero if the timeout expires before
*       anything interesting happens. On  error, -1 is returned, and errno
*       is set appropriately; the sets and timeout become
*       undefined, so do not rely on their contents after an error.
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_SIZE (*CMD_OS_SELECT_FUN)
(
    IN GT_SIZE    width ,
    INOUT GT_VOID *  readSet ,
    INOUT GT_VOID *  writeSet ,
    INOUT GT_VOID *  exceptionSet ,
    IN GT_U32     timeOut
);

/*******************************************************************************
* osSocketGetSocketFdSetSize()
*
* DESCRIPTION:
*       Returns the size of sockaddr_in.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       sockAddrSize - to be filled with sockaddr_in size.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail
*
* COMMENTS:
*       USERS THAT IMPLEMENTS THEIR OWN OS LAYER CAN RETURN SIZE = 0.
*
*******************************************************************************/
typedef GT_STATUS (*CMD_OS_SOCKET_GET_SOCKET_FD_SET_SIZE_FUN)
(
    OUT GT_U32*    sockFdSize
);


/* CMD_OS_FUNC_BIND_SOCKETS_STC -
*    structure that hold the "os sockets" functions needed be bound to CMD.
*
*/
typedef struct{
    CMD_OS_SOCKET_TCP_CREATE_FUN        osSocketTcpCreate;
    CMD_OS_SOCKET_UDP_CREATE_FUN        osSocketUdpCreate;
    CMD_OS_SOCKET_TCP_DESTROY_FUN       osSocketTcpDestroy;
    CMD_OS_SOCKET_UDP_DESTROY_FUN       osSocketUdpDestroy;
    CMD_OS_SOCKET_CREATE_ADDR_FUN       osSocketCreateAddr;
    CMD_OS_SOCKET_DESTROY_ADDR_FUN      osSocketDestroyAddr;
    CMD_OS_SOCKET_BIND_FUN              osSocketBind;
    CMD_OS_SOCKET_LISTEN_FUN            osSocketListen;
    CMD_OS_SOCKET_ACCEPT_FUN            osSocketAccept;
    CMD_OS_SOCKET_CONNECT_FUN           osSocketConnect;
    CMD_OS_SOCKET_SET_NON_BLOCK_FUN     osSocketSetNonBlock;
    CMD_OS_SOCKET_SET_BLOCK_FUN         osSocketSetBlock;
    CMD_OS_SOCKET_SEND_FUN              osSocketSend;
    CMD_OS_SOCKET_SEND_TO_FUN           osSocketSendTo;
    CMD_OS_SOCKET_RECV_FUN              osSocketRecv;
    CMD_OS_SOCKET_RECV_FROM_FUN         osSocketRecvFrom;
    CMD_OS_SOCKET_SET_SOCKET_NO_LINGER_FUN osSocketSetSocketNoLinger;
    CMD_OS_SOCKET_EXTRACT_IP_ADDR_FROM_SOCKET_ADDR_FUN osSocketExtractIpAddrFromSocketAddr;
    CMD_OS_SOCKET_GET_SOCKET_ADDR_SIZE_FUN osSocketGetSocketAddrSize;
    CMD_OS_SOCKET_SHUT_DOWN_FUN         osSocketShutDown;

    CMD_OS_SELECT_CREATE_SET_FUN        osSelectCreateSet;
    CMD_OS_SELECT_ERASE_SET_FUN         osSelectEraseSet;
    CMD_OS_SELECT_ZERO_SET_FUN          osSelectZeroSet;
    CMD_OS_SELECT_ADD_FD_TO_SET_FUN     osSelectAddFdToSet;
    CMD_OS_SELECT_CLEAR_FD_FROM_SET_FUN osSelectClearFdFromSet;
    CMD_OS_SELECT_IS_FD_SET_FUN         osSelectIsFdSet;
    CMD_OS_SELECT_COPY_SET_FUN          osSelectCopySet;
    CMD_OS_SELECT_FUN                   osSelect;
    CMD_OS_SOCKET_GET_SOCKET_FD_SET_SIZE_FUN osSocketGetSocketFdSetSize;
}CMD_OS_FUNC_BIND_SOCKETS_STC;

/*******************************************************************************
* osTaskGetSelf
*
* DESCRIPTION:
*       returns the current task (thread) id
*
* INPUTS:
*       none
*
* OUTPUTS:
*       tid -  the current task (thread) id
*
* RETURNS:
*       GT_OK - on success.
*       GT_FAIL - if parameter is invalid
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*CMD_OS_TASK_GET_SELF_FUN)
(
    OUT GT_U32 *tid
);

/*******************************************************************************
* osSetTaskPrior
*
* DESCRIPTION:
*       Changes priority of task/thread.
*
* INPUTS:
*       tid     - Task ID
*       newprio - new priority of task
*
* OUTPUTS:
*       oldprio - old priority of task
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       If tid = 0, change priority of calling task (itself)
*
*******************************************************************************/
typedef GT_STATUS (*CMD_OS_TASK_SET_TASK_PRIOR_FUNC)
(
    IN  GT_TASK tid,
    IN  GT_U32  newprio,
    OUT GT_U32  *oldprio
);



/* CMD_OS_FUNC_BIND_TASKS_STC -
*    structure that hold the "os tasks" functions needed be bound to CMD.
*
*/
typedef struct{
    CMD_OS_TASK_GET_SELF_FUN            osTaskGetSelf;
    CMD_OS_TASK_SET_TASK_PRIOR_FUNC     osSetTaskPrior;
}CMD_OS_FUNC_BIND_TASKS_STC;


#ifndef __gtBmPoolh
/************* Define *********************************************************/

#define GT_POOL_NULL_ID  ((GT_POOL_ID)NULL)



/************* Typedef ********************************************************/


/*
 * Typedef: GT_POOL_ID
 *
 * Description:
 *  Define type for pool ID.
 *  The pool ID is allocated and returned to the user by the poolCreate function.
 *  In order to delet pool, allocate buffer or free buffer, the pool ID is given as
 *  parametr to those service functions.
 *
 */
typedef GT_VOID* GT_POOL_ID;

/*
 * Typedef: enum GT_POOL_ALIGNMENT
 *
 * Description:
 *  Define the buffer alignment supported by the module.
 *
 */
typedef enum
{
    GT_1_BYTE_ALIGNMENT  = 0x1, /* 00000001 */
    GT_4_BYTE_ALIGNMENT  = 0x3, /* 00000011 */
    GT_8_BYTE_ALIGNMENT  = 0x7, /* 00000111 */
    GT_16_BYTE_ALIGNMENT = 0xF  /* 00001111 */

}GT_POOL_ALIGNMENT;


#endif /* __gtBmPoolh */

/************* Functions ******************************************************/


/*******************************************************************************
* gtPoolCreatePool
*
* DESCRIPTION:
*           Create pool of buffers. All buffers have the same size.
*
* INPUTS:
*           bufferSize   - Buffer's size
*           alignment    - Buffers' alignment
*           numOfBuffers - number of buffer at the pool
*           useSem       - GT_TRUE to use a semaphore for mutual exclusion on
*                         access to the pool.
*                  Note: Semaphore protection is one for all pools.
*
* OUTPUTS:
*           pPoolId       - Return the pool ID
* RETURNS:
*       GT_OK - For successful operation.
*       GT_BAD_VALUE - Request for zero number of buffers or
*                      buffer's size is smaller than the size of
*                      pointer (usually 4 bytes)
*       GT_BAD_PTR - Bad pointer for the pPoolId
*       GT_NO_RESOURCE - No memory is available to create the pool.
*       GT_NOT_INITIALIZED - In case of un-initialized package.
*
* COMMENTS:
*       Usage of this function is only during FIRST initialization.
*
*******************************************************************************/
typedef GT_STATUS  (*CMD_OS_POOL_CREATE_POOL_FUNC)
(
    IN   GT_U32            reqBufferSize,
    IN   GT_POOL_ALIGNMENT alignment,
    IN   GT_U32            numOfBuffers,
    IN   GT_BOOL           useSem,
    OUT  GT_POOL_ID       *pPoolId
);

/*******************************************************************************
* gtPoolCreateDmaPool
*
* DESCRIPTION:
*           Create pool of buffers. All buffers have the same size.
*           The buffers are created in a physically contiguous area.
*
* INPUTS:
*           bufferSize   - Buffer's size
*           alignment    - Buffers' alignment
*           numOfBuffers - number of buffer at the pool
*           useSem       - GT_TRUE to use a semaphore for mutual exclusion on
*                         access to the pool.
*                  Note: Semaphore protection is one for all pools.
*
* OUTPUTS:
*           pPoolId       - Return the pool ID
* RETURNS:
*       GT_OK - For successful operation.
*       GT_BAD_VALUE - Request for zero number of buffers or
*                      buffer's size is smaller than the size of
*                      pointer (usually 4 bytes)
*       GT_BAD_PTR - Bad pointer for the pPoolId
*       GT_NO_RESOURCE - No memory is available to create the pool.
*       GT_NOT_INITIALIZED - In case of un-initialized package.
*
* COMMENTS:
*       Usage of this function is only during FIRST initialization.
*
*******************************************************************************/
typedef GT_STATUS  (*CMD_OS_POOL_CREATE_DMA_POOL_FUNC)
(
    IN   GT_U32            reqBufferSize,
    IN   GT_POOL_ALIGNMENT alignment,
    IN   GT_U32            numOfBuffers,
    IN   GT_BOOL           useSem,
    OUT  GT_POOL_ID       *pPoolId
);

/*******************************************************************************
* gtPoolReCreatePool
*
* DESCRIPTION:
*           Create pool of buffers. All buffers have the same size.
*
* INPUTS:
*           poolId    - Id of the pool to be re-created.

* OUTPUTS:
*           None.
* RETURNS:
*       GT_OK        - For successful operation.
*       GT_BAD_VALUE - Bad poolId
*       GT_NOT_INITIALIZED - In case of un-initialized package.
*
* COMMENTS:
*       Usage of this function is only during FIRST initialization.
*
*******************************************************************************/
typedef GT_STATUS  (*CMD_OS_POOL_RE_CREATE_POOL_FUNC)
(
    IN  GT_POOL_ID  poolId
);

/*******************************************************************************
* gtPoolDeletePool
*
* DESCRIPTION:
*           Delete a pool and free all the memory occupied by that pool.
*           The operation is failed if not all the buffers belong to that pool
*           have been freed.
* INPUTS:
*           PoolId - The pool ID as returned by the create function.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - For successful operation.
*       GT_BAD_STATE - Not all buffers belong to that pool have
*                      been freed.
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS  (*CMD_OS_POOL_DELETE_POOL_FUNC)
(
    IN  GT_POOL_ID  poolId
);

/*******************************************************************************
* gtPoolGetBuf
*
* DESCRIPTION:
*           Return pointer to a buffer from the requested pool
*
* INPUTS:
*           PoolId - The pool ID as returned by the create function.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_VOID* - Pointer to the new allocated buffer. NULL is returned in case
*               no buffer is not available.
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_VOID* (*CMD_OS_POOL_GET_BUF_FUNC)
(
    IN        GT_POOL_ID  poolId
);


/*******************************************************************************
* gtPoolFreeBuf
*
* DESCRIPTION:
*           Free a buffer back to its pool.
*
* INPUTS:
*           PoolId - The pool ID as returned by the create function.
*           pBuf   - Pointer to buffer to be freed
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_STATUS    GT_OK - For successful operation.
*                      GT_BAD_PTR - The returned buffer is not belongs to that pool
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS  (* CMD_OS_POOL_FREE_BUF_FUNC)
(
    IN        GT_POOL_ID  poolId,
    IN        GT_VOID*    pBuf
);


/*******************************************************************************
* gtPoolGetBufSize
*
* DESCRIPTION:
*           Get the buffer size.
*
* INPUTS:
*           PoolId - The pool ID as returned by the create function.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_U32  - the buffer size.
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_U32  (* CMD_OS_POOL_GET_BUF_SIZE_FUNC)
(
    IN        GT_POOL_ID  poolId
);

/*******************************************************************************
* gtPoolExpandPool
*
* DESCRIPTION:
*           Expand a pool of buffers. All buffers have the same size.
*
* INPUTS:
*           PoolId - The pool ID as returned by the create function.
*           forDma       - is physically contiguous.
*           numOfBuffers - number of buffer to add to the pool
*
* OUTPUTS:
*       none.
* RETURNS:
*       GT_OK - For successful operation.
*       GT_BAD_VALUE - Request for zero number of buffers or
*                      buffer's size is smaller than the size of
*                      pointer (usually 4 bytes)
*       GT_BAD_PTR - Bad pointer for the pPoolId
*       GT_NO_RESOURCE - No memory is available to create the pool.
*       GT_NOT_INITIALIZED - In case of un-initialized package.
*
* COMMENTS:
*       not used for Dma buffers!
*
*******************************************************************************/
typedef GT_STATUS  (* CMD_OS_POOL_EXPAND_POOL_FUNC)
(
    IN   GT_POOL_ID        pPoolId,
    IN   GT_U32            numOfBuffers
);


/*******************************************************************************
* gtPoolGetBufFreeCnt
*
* DESCRIPTION:
*           Get the free buffer count.
*
* INPUTS:
*           PoolId - The pool ID as returned by the create function.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_U32  - the free buffer count.
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_U32 (*CMD_OS_POOL_GET_BUF_FREE_CNT_FUNC)
(
    IN        GT_POOL_ID  poolId
);


/*******************************************************************************
* gtPoolPrintStats
*
* DESCRIPTION:
*           Print pool's statistics for the requested pool, or for all
*           the pols in case poolID is GT_POOL_NULL_ID.
*
* INPUTS:
*           PoolId      - The pool ID as returned by the create function.
*                         or GT_POOL_NULL_ID to print all pools' information.
* OUTPUTS:
*       None
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_VOID  (* CMD_OS_POOL_PRINT_STATS_FUNC)
(
    IN  GT_POOL_ID poolId
);



/* CMD_OS_FUNC_BIND_POOLS_STC -
*    structure that hold the "os pools" functions needed be bound to CMD.
*
*/
typedef struct{
    CMD_OS_POOL_CREATE_POOL_FUNC        gtPoolCreatePool;
    CMD_OS_POOL_CREATE_DMA_POOL_FUNC    gtPoolCreateDmaPool;
    CMD_OS_POOL_RE_CREATE_POOL_FUNC     gtPoolReCreatePool;
    CMD_OS_POOL_DELETE_POOL_FUNC        gtPoolDeletePool;
    CMD_OS_POOL_GET_BUF_FUNC            gtPoolGetBuf;
    CMD_OS_POOL_FREE_BUF_FUNC           gtPoolFreeBuf;
    CMD_OS_POOL_GET_BUF_SIZE_FUNC       gtPoolGetBufSize;
    CMD_OS_POOL_EXPAND_POOL_FUNC        gtPoolExpandPool;
    CMD_OS_POOL_GET_BUF_FREE_CNT_FUNC   gtPoolGetBufFreeCnt;
    CMD_OS_POOL_PRINT_STATS_FUNC        gtPoolPrintStats;
}CMD_OS_FUNC_BIND_POOLS_STC;



/*******************************************************************************
* cmdIsCpuEtherPortUsed
*
* DESCRIPTION:
*           is CPU Ethernet port used (and not SDMA)
*
* INPUTS:
*           none
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_TRUE - if CPU Ethernet port used (and not SDMA)
*       GT_FALSE - otherwise
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_BOOL (*CMD_CPU_ETHERNET_IS_CPU_ETHER_PORT_USED)
(
    GT_VOID
);


/* CMD_FUNC_BIND_CPU_ETHERNET_PORT_STC -
*    structure that hold the "cpu Ethernet port" functions needed be bound to CMD.
*
*/
typedef struct{
    CMD_CPU_ETHERNET_IS_CPU_ETHER_PORT_USED        cmdIsCpuEtherPortUsed;
}CMD_FUNC_BIND_CPU_ETHERNET_PORT_STC;



/*******************************************************************************
* CMD_CPSS_EVENT_BIND_FUNC
*
* DESCRIPTION:
*       This routine binds a user process to unified event. The routine returns
*       a handle that is used when the application wants to wait for the event
*       (cpssEventSelect), receive the event(cpssEventRecv) or transmit a packet
*       using the Network Interface.
*
*       NOTE : the function does not mask/unmask the HW events in any device.
*              This is Application responsibility to unmask the relevant events
*              on the needed devices , using function cpssEventDeviceMaskSet
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       uniEventArr - The unified event list.
*       arrLength   - The unified event list length.
*
* OUTPUTS:
*       hndlPtr     - (pointer to) The user handle for the bounded events.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on failure
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_OUT_OF_CPU_MEM - failed to allocate CPU memory,
*       GT_FULL - when trying to set the "tx buffer queue unify event"
*                 (CPSS_PP_TX_BUFFER_QUEUE_E) with other events in the same
*                 handler
*       GT_ALREADY_EXIST - one of the unified events already bound to another
*                 handler (not applicable to CPSS_PP_TX_BUFFER_QUEUE_E)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*CMD_CPSS_EVENT_BIND_FUNC)
(
    IN  CPSS_UNI_EV_CAUSE_ENT uniEventArr[],
    IN  GT_U32               arrLength,
    OUT GT_U32               *hndlPtr
);

/*******************************************************************************
* CMD_CPSS_EVENT_SELECT_FUNC
*
* DESCRIPTION:
*       This function waiting for one of the events ,relate to the handler,
*       to happen , and gets a list of events (in array of bitmaps format) that
*       occurred .
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       hndl                - The user handle for the bounded events.
*       timeoutPtr          - (pointer to) Wait timeout in milliseconds
*                             NULL pointer means wait forever.
*       evBitmapArrLength   - The bitmap array length (in words).
*
* OUTPUTS:
*       evBitmapArr         - The bitmap array of the received events.
*
* RETURNS:
*       GT_OK  - success
*       GT_FAIL - general failure
*       GT_TIMEOUT - when the "time out" requested by the caller expired and no
*                    event occurred during this period
*       GT_BAD_PARAM - bad hndl parameter , the hndl parameter is not legal
*                     (was not returned by cpssEventBind(...))
*       GT_BAD_PTR  - evBitmapArr parameter is NULL pointer
*                     (and evBitmapArrLength != 0)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*CMD_CPSS_EVENT_SELECT_FUNC)
(
    IN  GT_U32              hndl,
    IN  GT_U32              *timeoutPtr,
    OUT GT_U32              evBitmapArr[],
    IN  GT_U32              evBitmapArrLength
);

/*******************************************************************************
* CMD_CPSS_EVENT_RECV_FUNC
*
* DESCRIPTION:
*       This function gets general information about the selected unified event.
*       The function retrieve information about the device number that relate to
*       the event , and extra info about port number / priority queue that
*       relate to the event.
*       The function retrieve the info about the first occurrence of this event
*       in the queue.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       hndl        - The user handle for the bounded events.
*       evCause     - The specified unify event that info about it's first
*                     waiting occurrence required.
* OUTPUTS:
*       evExtDataPtr- (pointer to)The additional date (port num / priority
*                     queue number) the event was received upon.
*       evDevPtr    - (pointer to)The device the event was received upon
*
* RETURNS:
*       GT_OK  - success
*       GT_FAIL - general failure
*       GT_NO_MORE   - There is no more info to retrieve about the specified
*                      event .
*       GT_BAD_PARAM - bad hndl parameter ,
*                      or hndl bound to CPSS_PP_TX_BUFFER_QUEUE_E --> not allowed
*                      use dedicated "get tx ended info" function instead
*       GT_BAD_PTR  - one of the parameters is NULL pointer
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*CMD_CPSS_EVENT_RECV_FUNC)
(
    IN  GT_U32              hndl,
    IN  CPSS_UNI_EV_CAUSE_ENT evCause,
    OUT GT_U32              *evExtDataPtr,
    OUT GT_U8               *evDevPtr
);

/*******************************************************************************
* appDemoCpssEventDeviceMaskSet
*
* DESCRIPTION:
*       This routine mask/unmasks an unified event on specific device.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       devNum - device number - PP/FA/Xbar device number -
*                depend on the uniEvent
*                if the uniEvent is in range of PP events , then devNum relate
*                to PP
*                if the uniEvent is in range of FA events , then devNum relate
*                to FA
*                if the uniEvent is in range of XBAR events , then devNum relate
*                to XBAR
*       uniEvent   - The unified event.
*       operation  - the operation : mask / unmask
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK  - on success
*       GT_FAIL - on failure.
*       GT_BAD_PARAM - bad unify event value or bad device number
*
* COMMENTS:
*
*       The OS interrupts and the ExtDrv interrupts are locked during the
*       operation of the function.
*
*******************************************************************************/
typedef GT_STATUS (*CMD_CPSS_EVENT_DEVICE_MASK_SET_FUNC)
(
    IN GT_U8                    devNum,
    IN CPSS_UNI_EV_CAUSE_ENT    uniEvent,
    IN CPSS_EVENT_MASK_SET_ENT  operation
);



/* CMD_FUNC_BIND_EVENTS_STC -
*    structure that hold the "events" functions needed be bound to CMD.
*
*/
typedef struct{
    CMD_CPSS_EVENT_BIND_FUNC            cmdCpssEventBind;
    CMD_CPSS_EVENT_SELECT_FUNC          cmdCpssEventSelect;
    CMD_CPSS_EVENT_RECV_FUNC            cmdCpssEventRecv;
    CMD_CPSS_EVENT_DEVICE_MASK_SET_FUNC cmdCpssEventDeviceMaskSet;
}CMD_FUNC_BIND_EVENTS_STC;

/*******************************************************************************
* appDemoDbEntryAdd
*
* DESCRIPTION:
*       Set AppDemo DataBase value.This value will be considered during system
*       initialization process.
*
* INPUTS:
*       namePtr         - points to parameter name
*       value           - parameter value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - if name is too long
*       GT_BAD_PTR      - if NULL pointer
*       GT_NO_RESOURCE  - if Database is full
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (* CMD_APP_DB_ENTRY_ADD_FUNC)
(
    IN  GT_CHAR *namePtr,
    IN  GT_U32  value
);


/******************************************************************************
* appDemoDbEntryGet
*
* DESCRIPTION:
*       Get parameter value from AppDemo DataBase.
*
* INPUTS:
*       namePtr         - points to parameter name
*
* OUTPUTS:
*       valuePtr        - points to parameter value
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - if name is too long
*       GT_BAD_PTR      - if NULL pointer
*       GT_NO_SUCH      - there is no such parameter in Database
*
* COMMENTS:
*       None.
*
******************************************************************************/
typedef GT_STATUS (* CMD_APP_DB_ENTRY_GET_FUNC)
(
    IN  GT_CHAR *namePtr,
    OUT GT_U32  *valuePtr
);


/******************************************************************************
* appDemoDbDump
*
* DESCRIPTION:
*       Dumps entries set in AppDemo database to console.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*
* COMMENTS:
*       None.
*
******************************************************************************/
typedef GT_STATUS (*CMD_APP_DB_DUMP_FUNC)
(
    GT_VOID
);

/*
 * Typedef: struct CMD_APP_PP_CONFIG
 *
 * Description: Holds Pp configuration that Application hold
 *
 * Fields:
 *      wa - info about WA (workarounds used)
 *          trunkToCpuPortNum - do we use the trunk WA : traffic to CPU from
 *              trunk member get wrong port num indication
 */
typedef struct
{
    CPSS_PP_DEVICE_TYPE             deviceId;
    GT_U8                           devNum;
    GT_BOOL                         valid;
    struct{
        GT_BOOL     trunkToCpuPortNum;
    }wa;
}CMD_APP_PP_CONFIG;


/******************************************************************************
* cmdAppPpConfigGet
*
* DESCRIPTION:
*       get pointer to the application's info about the PP
*
* INPUTS:
*       devIndex - index of the device in the Application DB (0..127)
*
* OUTPUTS:
*       ppConfigPtr - the info about the PP
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PTR - on NULL pointer
*       GT_OUT_OF_RANGE - on devIndex > 127
*
* COMMENTS:
*       None.
*
******************************************************************************/
typedef GT_STATUS (*CMD_APP_PP_CONFIG_GET_FUNC)
(
    IN GT_U8    devIndex,
    OUT CMD_APP_PP_CONFIG* ppConfigPtr
);

/******************************************************************************
* cmdAppPpConfigPrint
*
* DESCRIPTION:
*       print Pp configuration
*
* INPUTS:
*       devNum - device number(0..127)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*       GT_OUT_OF_RANGE - on devIndex > 127
*
* COMMENTS:
*       None.
*
******************************************************************************/
typedef GT_STATUS (*CMD_APP_PP_CONFIG_PRINT_FUNC)
(
    IN GT_U8    devNum
);


/*******************************************************************************
* cpssInitSystem
*
* DESCRIPTION:
*       This is the main board initialization function for CPSS driver.
*
* INPUTS:
*       boardIdx      - The index of the board to be initialized from the board
*                       list.
*       boardRevId    - Board revision Id.
*       reloadEeprom  - Whether the Eeprom should be reloaded when
*                       corePpHwStartInit() is called.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*CMD_APP_INIT_SYSTEM_FUNC)
(
    IN  GT_U32  boardIdx,
    IN  GT_U32  boardRevId,
    IN  GT_U32  reloadEeprom
);

/*******************************************************************************
* confi2InitSystem
*
* DESCRIPTION:
*       This is the main board initialization function.
*
* INPUTS:
*       theConfiId  - the confi ID
*       echoOn      - if GT_TRUE the echo is on, otherwise the echo is off.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - if succeeded,
*       GT_FAIL - if failed
*
* COMMENTS:
*
*
*******************************************************************************/
typedef GT_STATUS (*CMD_APP_INIT_CONFI_FUNC)
(
    IN GT_U8        theConfiId,
    IN GT_BOOL      echoOn
);

/*******************************************************************************
* appDemoShowBoardsList
** DESCRIPTION:
*       This function displays the boards list which have an automatic
*       initialization support.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       The printed table should look like:
*       +-------------------------------+-----------------------+
*       | Board name                    | Revisions             |
*       +-------------------------------+-----------------------+
*       | 01 - RD-EX120-24G             |                       |
*       |                               | 01 - Rev 1.0          |
*       |                               | 02 - Rev 2.0          |
*       +-------------------------------+-----------------------+
*       | 02 - DB-MX610-48F4GS          |                       |
*       |                               | 01 - Rev 1.0          |
*       |                               | 02 - Rev 2.5          |
*       +-------------------------------+-----------------------+
*
*******************************************************************************/
typedef GT_STATUS (*CMD_APP_SHOW_BOARDS_LIST_FUNC)
(
    GT_VOID
);

/*******************************************************************************
* cmdAppIsSystemInitialized
** DESCRIPTION:
*       is system initialized
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE   - system initialized
*       GT_FALSE  - system NOT initialized
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_BOOL (*CMD_APP_IS_SYSTEM_INITIALIZED_FUNC)
(
    GT_VOID
);

#ifdef EXMXPM_FAMILY

#ifndef __appDemoExMxPmExternalTcamConfigh
/*
 * typedef: struct APP_DEMO_EXTERNAL_TCAM_OVERWRITTEN_DEFAULT_STC
 *
 * Description:
 *      Entry for collecting information of the number of rules requested
 *      for client, key and lookup combination that will overwrite the
 *      default values.
 *
 * Fields:
 *       devNum             - device number
 *       tcamClient         - EXMXPM device External TCAM unit client.
 *       clientKeyType      - The selected key type enumeration per client:
 *                            please refer to Functional Spec for key types number.
 *       lookupIndex        - The number of lookup.
 *       requestedRulesNum  - the number of rules requested (will overwrite the
 *                            default value).
 *       inUse              - entry already in use (GT_TRUE) or not yet used (GT_FALSE)
 *
 * Comment:
 */
typedef struct
{
  GT_U8                                 devNum;
  CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT  tcamClient;
  GT_U32                                clientKeyType;
  GT_U32                                lookupIndex;
  GT_U32                                requestedRulesNum;
  GT_BOOL                               inUse;
} APP_DEMO_EXTERNAL_TCAM_OVERWRITTEN_DEFAULT_STC;

/*
 * typedef: struct APP_DEMO_EXTERNAL_TCAM_NUM_OF_RULES_STC
 *
 * Description:
 *      Entry for collecting information of the number of rules
 *      for client, key and lookup combination.
 *
 * Fields:
 *       lookupIndex        - The number of lookup.
 *       tcamClient         - EXMXPM device External TCAM unit client.
 *       clientKeyType      - The selected key type enumeration per client:
 *                            please refer to Functional Spec for key types number.
 *       ruleSize            - size of rule entry.
 *       numOfRules         - the number of rules for this combination.
 *
 * Comment:
 */
typedef struct
{
  GT_U32                                    lookupIndex;
  CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT      tcamClient;
  GT_U32                                    clientKeyType;
  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT   ruleSize;
  GT_U32                                    numOfRules;
} APP_DEMO_EXTERNAL_TCAM_NUM_OF_RULES_STC;

/*
 * typedef: struct APP_DEMO_EXTERNAL_TCAM_KEY_LOOKUP_STC
 *
 * Description:
 *      Parameters for the external TCAM management, used for each external TCAM
 *      rule size (160, 320 or 640 bits) and lookup number combination.
 *
 * Fields:
 *       ruleStartIndex     - index of rule in external TCAM where area for rule size
 *                            in the lookup starts.
 *       maxRuleNum         - the maximal number of rules that can reside in the
 *                            specified area.
 *       actionStartIndex   - relative index of control memory line of external
 *                            TCAM actions chunk.
 *
 * Comment:
 */
typedef struct APP_DEMO_EXTERNAL_TCAM_KEY_LOOKUP_STCT
{
  GT_U32    ruleStartIndex;
  GT_U32    maxRuleNum;
  GT_U32    actionStartIndex;
} APP_DEMO_EXTERNAL_TCAM_KEY_LOOKUP_STC;

#endif /*__appDemoExMxPmExternalTcamConfigh*/

/*******************************************************************************
* appDemoExMxPmExternalTcamRuleIndexByLenAndLookupGet
*
* DESCRIPTION:
*       The function gets the rule start index and the maximum number of
*       rules that describe the external TCAM rule area appropriate to
*       a rule size and lookup number combination.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number
*       ruleSize            - size of rule entry.
*       lookupIndex         - the number of lookup.
*
* OUTPUTS:
*       ruleStartIndexPtr   - (pointer to) rule start index number of the area.
*       maxRuleNumPtr       - (pointer to) the maximal number of rules that can
*                             be inserted into the area.
*       linesPerRulePtr     - (pointer to) how many external TCAM lines a rule
*                             of ruleSize type consumes.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_STATUS (*CMD_APP_EXMXPM_EXTERNAL_TCAM_RULE_INDEX_BY_LEN_AND_LOOKUP_GET_FUNC)
(
    IN GT_U8                                        devNum,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT     ruleSize,
    IN  GT_U32                                      lookupIndex,
    OUT GT_U32                                      *ruleStartIndexPtr,
    OUT GT_U32                                      *maxRuleNumPtr,
    OUT GT_U32                                      *linesPerRulePtr
);

/*******************************************************************************
* appDemoExMxPmExternalTcamActionIndexGet
*
* DESCRIPTION:
*       The function gets the line in control memory area allocated for external
*       TCAM actions for a defined rule line in external TCAM.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number
*       ruleSize            - size of rule entry.
*       lookupIndex         - the number of lookup.
*       ruleStartIndex      - rule start index.
*
* OUTPUTS:
*       actionStartIndexPtr - (pointer to) action start index.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_STATUS (*CMD_APP_EXMXPM_EXTERNAL_TCAM_ACTION_INDEX_GET_FUNC)
(
    IN  GT_U8                                       devNum,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT     ruleSize,
    IN  GT_U32                                      lookupIndex,
    IN  GT_U32                                      ruleStartIndex,
    OUT GT_U32                                      *actionStartIndexPtr
);

/*******************************************************************************
* appDemoExMxPmExternalTcamClientKeyLookupSet
*
* DESCRIPTION:
*       This function sets the value of a number of rules requested for a
*       device, client, key and lookup combination later to be used and
*       overwrite the default system value.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number
*       tcamClient          - EXMXPM device External TCAM unit client.
*       clientKeyType       - The selected key type enumeration per client:
*                             please refer to Functional Spec for key types number.
*       lookupIndex         - The number of lookup.
*       requestedRulesNum   - the number of rules requested (will overwrite the
*                             default value).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NO_RESOURCE           - no place to save the request
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In order to effect the system external TCAM configuration this
*       function must be called before cpssInitSystem!!!
*
*******************************************************************************/
typedef GT_STATUS (*CMD_APP_EXMXPM_EXTERNAL_TCAM_CLIENT_KEY_LOOKUP_SET_FUNC)
(
  IN GT_U8                                 devNum,
  IN CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT  tcamClient,
  IN GT_U32                                clientKeyType,
  IN GT_U32                                lookupIndex,
  IN GT_U32                                requestedRulesNum
);

/*******************************************************************************
* appDemoExMxPmExternalTcamClientKeyLookupGet
*
* DESCRIPTION:
*       This function gets the value of a number of rules requested for a
*       device, client, key and lookup combination later to be used and
*       overwrite the default system value.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum                  - device number
*       tcamClient              - EXMXPM device External TCAM unit client.
*       clientKeyType           - The selected key type enumeration per client:
*                                 please refer to Functional Spec for key types number.
*       lookupIndex             - The number of lookup.
*
* OUTPUTS:
*       requestedRulesNumPtr    - (pointer to) the number of rules requested.
*       validPtr                - (pointer to) indication whether a match to the
*                                 device, client, key and lookup was found.
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_STATUS (*CMD_APP_EXMXPM_EXTERNAL_TCAM_CLIENT_KEY_LOOKUP_GET_FUNC)
(
    IN GT_U8                                 devNum,
    IN CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT  tcamClient,
    IN GT_U32                                clientKeyType,
    IN GT_U32                                lookupIndex,
    OUT GT_U32                               *requestedRulesNumPtr,
    OUT GT_BOOL                              *validPtr
);

/*******************************************************************************
* appDemoExMxPmExternalTcamClientKeyLookupSetByIndex
*
* DESCRIPTION:
*       This function sets an entry (specified by index) in the database
*       of values to overwrite the default of maximum number of rules for
*       device, client, key and lookup combination.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       entryIndex  - entry index in database.
*       entryPtr    - (pointer to) database entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_STATUS (*CMD_APP_EXMXPM_EXTERNAL_TCAM_CLIENT_KEY_LOOKUP_SET_BY_INDEX_FUNC)
(
    IN GT_U32                                           entryIndex,
    IN APP_DEMO_EXTERNAL_TCAM_OVERWRITTEN_DEFAULT_STC   *entryPtr
);

/*******************************************************************************
* appDemoExMxPmExternalTcamClientKeyLookupGetByIndex
*
* DESCRIPTION:
*       This function gets an entry (specified by index) from the database
*       of values to overwrite the default of maximum number of rules for
*       device, client, key and lookup combination.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       entryIndex  - entry index in database.
*
* OUTPUTS:
*       entryPtr    - (pointer to) database entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_STATUS (*CMD_APP_EXMXPM_EXTERNAL_TCAM_CLIENT_KEY_LOOKUP_GET_BY_INDEX_FUNC)
(
    IN GT_U32                                           entryIndex,
    OUT APP_DEMO_EXTERNAL_TCAM_OVERWRITTEN_DEFAULT_STC  *entryPtr
);

/*******************************************************************************
* appDemoExMxPmExternalTcamConfig
*
* DESCRIPTION:
*       Configure external TCAM device (by setting appropriate values to external
*       TCAM registers, not ETC registers) based on the external TCAM management
*       database content.
*       This function configure the ruleStartIndex fields in the external TCAM
*       management database.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       None.

* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Since the external TCAM type used is required this function is called
*       after CPSS database is initialized with external TCAM type.
*
*******************************************************************************/
typedef GT_STATUS (*CMD_APP_EXMXPM_EXTERNAL_TCAM_CONFIG_FUNC)
(
    IN  GT_U8                        devNum
);


/*******************************************************************************
* appDemoExMxPmHsuOldImagePreUpdatePreparation
*
* DESCRIPTION:
*       This function perform following steps of HSU process:
*       1. Disable interrupts
*       2. export of all hsu datatypes
*       3. warm restart
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       isMultipleIteration - GT_TRUE - multiIteration HSU operation
*                             GT_FALSE - single iteration HSU operation
*       origIterationSize   - hsu iteration size
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NO_RESOURCE           - no place to save the request
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*CMD_APP_EXMXPM_EXTERNAL_HSU_OLD_IMAGE_PRE_UPDATE_PREPARATION_FUNC)
(
    IN GT_BOOL                               isMultipleIteration,
    IN GT_U32                                origIterationSize
);

/*******************************************************************************
* appDemoExMxPmHsuNewImageUpdateAndSynch
*
* DESCRIPTION:
*       This function perform following steps of HSU process:
*       1. import of all hsu datatypes
*       2. setting HSU state in "HSU is complete"
*       4. open pp interrupts
*       3. enable interrupts
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       isMultipleIteration - GT_TRUE - multiIteration HSU operation
*                             GT_FALSE - single iteration HSU operation
*       origIterationSize   - hsu iteration size
*       hsuDataSize         - hsu import data size
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NO_RESOURCE           - no place to save the request
*
* COMMENTS:
*       When appDemoExMxPmHsuOldImagePreUpdatePreparation is finished and new image
*       have arisen after warm restart, user should perform
*       cpssHsuStateSet(CPSS_HSU_STATE_IN_PROGRESS_E), cpssInitSystem and only
*       after that call appDemoExMxPmHsuNewImageUpdateAndSynch.
*
*******************************************************************************/
typedef GT_STATUS (*CMD_APP_EXMXPM_EXTERNAL_HSU_NEW_IMAGE_UPDATE_AND_SYNCH_FUNC)
(
    IN GT_BOOL                               isMultipleIteration,
    IN GT_U32                                origIterationSize,
    IN GT_U32                                hsuDataSize
);


#endif /*EXMXPM_FAMILY*/

#ifdef CHX_FAMILY
/*******************************************************************************
* gtAppDemoLionPortModeSpeedSet
*
* DESCRIPTION:
*       Example of configuration sequence of port interface mode and speed
*       to show move from 1G to 10G and from 10G to 1G port data speed
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number (or CPU port)
*       ifMode    - Interface mode.
*       speed    - port speed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_HW_ERROR - hw error
*
* COMMENTS:
*       Current supported modes 1G SGMII (1.25G serdes speed)
*               and 10G RXAUI (6.25G serdes speed)
*
*******************************************************************************/
typedef GT_STATUS (*CMD_APP_LION_PORT_MODE_SPEED_SET_FUNC)
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode,
    IN  CPSS_PORT_SPEED_ENT  speed
);


/*******************************************************************************
* gtAppDemoXcat2StackPortsModeSpeedSet
*
* DESCRIPTION:
*       Example of configuration sequence for stack ports of xcat2
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number (or CPU port)
*       ifMode   - Interface mode.
*       speed    - port speed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_HW_ERROR - hw error
*       GT_BAD_PARAM - device not exists
*       GT_NOT_SUPPORTED - wrong ifMode,speed
*       GT_NOT_APPLICABLE_DEVICE - wrong devFamily
*
* COMMENTS:
*       At this moment supported:
*           CPSS_PORT_INTERFACE_MODE_SGMII_E        10M, 100M, 1G, 2.5G
*           CPSS_PORT_INTERFACE_MODE_1000BASE_X_E   1G
*           CPSS_PORT_INTERFACE_MODE_100BASE_FX_E   100M
*
*******************************************************************************/
typedef GT_STATUS (*CMD_APP_XCAT2_STACK_PORTS_MODE_SPEED_SET_FUNC)
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode,
    IN  CPSS_PORT_SPEED_ENT  speed
);

#endif


/*******************************************************************************
* allowProcessingOfAuqMessages
*
* DESCRIPTION:
*     Function to allow set the flag of : allowProcessingOfAuqMessages
*
* INPUTS:
*       enable - GT_TRUE: allow the processing of the AUQ messages
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*CMD_APP_ALLOW_PROCESSING_OF_AUQ_MESSAGES_FUNC)
(
    IN  GT_BOOL enable
);

/* CMD_OS_FUNC_BIND_APP_DB_STC -
*    structure that hold the "application DB" functions needed be bound to CMD.
*
*/
typedef struct{
    CMD_APP_DB_ENTRY_ADD_FUNC       cmdAppDbEntryAdd;
    CMD_APP_DB_ENTRY_GET_FUNC       cmdAppDbEntryGet;
    CMD_APP_DB_DUMP_FUNC            cmdAppDbDump;
    CMD_APP_PP_CONFIG_GET_FUNC      cmdAppPpConfigGet;
    CMD_APP_INIT_SYSTEM_FUNC        cmdInitSystem;
    CMD_APP_INIT_CONFI_FUNC         cmdInitConfi;
    CMD_APP_SHOW_BOARDS_LIST_FUNC   cmdAppShowBoardsList;
    CMD_APP_IS_SYSTEM_INITIALIZED_FUNC  cmdAppIsSystemInitialized;
    CMD_APP_PP_CONFIG_PRINT_FUNC  cmdAppPpConfigPrint;
    CMD_APP_ALLOW_PROCESSING_OF_AUQ_MESSAGES_FUNC   cmdAppAllowProcessingOfAuqMessages;
#ifdef EXMXPM_FAMILY
    CMD_APP_EXMXPM_EXTERNAL_TCAM_RULE_INDEX_BY_LEN_AND_LOOKUP_GET_FUNC  cmdAppExMxPmExternalTcamRuleIndexByLenAndLookupGet;
    CMD_APP_EXMXPM_EXTERNAL_TCAM_ACTION_INDEX_GET_FUNC                  cmdAppExMxPmExternalTcamActionIndexGet;
    CMD_APP_EXMXPM_EXTERNAL_TCAM_CLIENT_KEY_LOOKUP_SET_FUNC             cmdAppExMxPmExternalTcamClientKeyLookupSet;
    CMD_APP_EXMXPM_EXTERNAL_TCAM_CLIENT_KEY_LOOKUP_GET_FUNC             cmdAppExMxPmExternalTcamClientKeyLookupGet;
    CMD_APP_EXMXPM_EXTERNAL_TCAM_CLIENT_KEY_LOOKUP_SET_BY_INDEX_FUNC    cmdAppExMxPmExternalTcamClientKeyLookupSetByIndex;
    CMD_APP_EXMXPM_EXTERNAL_TCAM_CLIENT_KEY_LOOKUP_GET_BY_INDEX_FUNC    cmdAppExMxPmExternalTcamClientKeyLookupGetByIndex;
    CMD_APP_EXMXPM_EXTERNAL_TCAM_CONFIG_FUNC                            cmdAppExMxPmExternalTcamConfig;
    CMD_APP_EXMXPM_EXTERNAL_HSU_OLD_IMAGE_PRE_UPDATE_PREPARATION_FUNC   cmdAppExMxPmHsuOldImagePreUpdatePreparation;
    CMD_APP_EXMXPM_EXTERNAL_HSU_NEW_IMAGE_UPDATE_AND_SYNCH_FUNC         cmdAppExMxPmHsuNewImageUpdateAndSynch;
#endif /*EXMXPM_FAMILY*/
#ifdef CHX_FAMILY
    CMD_APP_LION_PORT_MODE_SPEED_SET_FUNC  cmdAppLionPortModeSpeedSet;
    CMD_APP_XCAT2_STACK_PORTS_MODE_SPEED_SET_FUNC  cmdAppXcat2StackPortsModeSpeedSet;
#endif /*CHX_FAMILY*/
}CMD_FUNC_BIND_APP_DB_STC;



/* CPSS_OS_FUNC_BIND_STC -
*    structure that hold the other "os" functions needed be bound to CMD.
*    and not defined for cpss
*
*       osSocketsBindInfo -  set of call back functions -
*                        sockets manipulation
*       osTasksBindInfo -  set of call back functions -
*                        tasks manipulation
*       osPoolsBindInfo -  set of call back functions -
*                        pools manipulation
*       osCpuEthernetPortBindInfo - set of call back functions
*                        CPU Ethernet port manipulation
*/
typedef struct{
    CMD_OS_FUNC_BIND_SOCKETS_STC    osSocketsBindInfo;
    CMD_OS_FUNC_BIND_TASKS_STC      osTasksBindInfo;
    CMD_OS_FUNC_BIND_POOLS_STC      osPoolsBindInfo;
}CMD_OS_FUNC_BIND_EXTRA_STC;


/* CPSS_FUNC_BIND_STC -
*    structure that hold the other  functions needed be bound to CMD.
*    and not defined for cpss
*
*       osCpuEthernetPortBindInfo - set of call back functions
*                        CPU Ethernet port manipulation
*       eventsBindInfo - set of call back functions
*                        events manipulation
*       appDbBindInfo  - set of call back functions
*                        application DB manipulation
*/
typedef struct{
    CMD_FUNC_BIND_CPU_ETHERNET_PORT_STC cpuEthernetPortBindInfo;
    CMD_FUNC_BIND_EVENTS_STC            eventsBindInfo;
    CMD_FUNC_BIND_APP_DB_STC            appDbBindInfo;
}CMD_FUNC_BIND_EXTRA_STC;


/*******************************************************************************
* applicationExtServicesBind
*
* DESCRIPTION:
*       the mainCmd calls this function , so the application (that implement
*       this function) will bind the mainCmd (and GaltisWrapper) with OS ,
*       external driver functions.
*
* INPUTS:
*       none.
* OUTPUTS:
*       extDrvFuncBindInfoPtr - (pointer to) set of external driver call back functions
*       osFuncBindPtr - (pointer to) set of OS call back functions
*       osExtraFuncBindPtr - (pointer to) set of extra OS call back functions (that CPSS not use)
*       extraFuncBindPtr - (pointer to) set of extra call back functions (that CPSS not use) (non OS functions)
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       This function must be implemented by the Application !!!
*
*******************************************************************************/
GT_STATUS   applicationExtServicesBind(
    OUT CPSS_EXT_DRV_FUNC_BIND_STC   *extDrvFuncBindInfoPtr,
    OUT CPSS_OS_FUNC_BIND_STC        *osFuncBindPtr,
    OUT CMD_OS_FUNC_BIND_EXTRA_STC   *osExtraFuncBindPtr,
    OUT CMD_FUNC_BIND_EXTRA_STC      *extraFuncBindPtr
);

#ifndef __gtOsSemh
typedef GT_VOID * GT_SEM;
typedef GT_VOID * GT_MUTEX;
#endif /*__gtOsSemh*/


/************* global *********************************************************/

extern CPSS_EXT_DRV_MGMT_CACHE_FLUSH_FUNC      cmdExtDrvMgmtCacheFlush;
extern CPSS_EXT_DRV_MGMT_CACHE_INVALIDATE_FUNC cmdExtDrvMgmtCacheInvalidate;

extern CPSS_EXTDRV_DMA_WRITE_FUNC  cmdExtDrvDmaWrite;
extern CPSS_EXTDRV_DMA_READ_FUNC   cmdExtDrvDmaRead;

extern CPSS_EXTDRV_ETH_PORT_RX_INIT_FUNC       cmdExtDrvEthPortRxInit;
extern CPSS_EXTDRV_ETH_PORT_TX_INIT_FUNC       cmdExtDrvEthPortTxInit;
extern CPSS_EXTDRV_ETH_PORT_ENABLE_FUNC        cmdExtDrvEthPortEnable;
extern CPSS_EXTDRV_ETH_PORT_DISABLE_FUNC       cmdExtDrvEthPortDisable;
extern CPSS_EXTDRV_ETH_PORT_TX_FUNC            cmdExtDrvEthPortTx;
extern CPSS_EXTDRV_ETH_PORT_INPUT_HOOK_ADD_FUNC  cmdExtDrvEthInputHookAdd;
extern CPSS_EXTDRV_ETH_PORT_TX_COMPLETE_HOOK_ADD_FUNC  cmdExtDrvEthTxCompleteHookAdd;
extern CPSS_EXTDRV_ETH_PORT_RX_PACKET_FREE_FUNC  cmdExtDrvEthRxPacketFree;

extern CPSS_EXTDRV_INT_CONNECT_FUNC        cmdExtDrvIntConnect;
extern CPSS_EXTDRV_INT_ENABLE_FUNC         cmdExtDrvIntEnable;
extern CPSS_EXTDRV_INT_DISABLE_FUNC        cmdExtDrvIntDisable;
extern CPSS_EXTDRV_INT_LOCK_MODE_SET_FUNC  cmdExtDrvSetIntLockUnlock;

extern CPSS_EXTDRV_PCI_CONFIG_WRITE_REG_FUNC      cmdExtDrvPciConfigWriteReg;
extern CPSS_EXTDRV_PCI_CONFIG_READ_REG_FUNC       cmdExtDrvPciConfigReadReg;
extern CPSS_EXTDRV_PCI_DEV_FIND_FUNC              cmdExtDrvPciFindDev;
extern CPSS_EXTDRV_PCI_INT_VEC_GET_FUNC           cmdExtDrvGetPciIntVec;
extern CPSS_EXTDRV_INT_MASK_GET_FUNC              cmdExtDrvGetIntMask;
extern CPSS_EXTDRV_PCI_COMBINED_ACCESS_ENABLE_FUNC cmdExtDrvEnableCombinedPciAccess;

extern CPSS_EXTDRV_HW_IF_SMI_INIT_DRIVER_FUNC             cmdExtDrvHwIfSmiInitDriver;
extern CPSS_EXTDRV_HW_IF_SMI_WRITE_REG_FUNC               cmdExtDrvHwIfSmiWriteReg;
extern CPSS_EXTDRV_HW_IF_SMI_READ_REG_FUNC                cmdExtDrvHwIfSmiReadReg;
extern CPSS_EXTDRV_HW_IF_SMI_TASK_REG_RAM_READ_FUNC       cmdExtDrvHwIfSmiTskRegRamRead;
extern CPSS_EXTDRV_HW_IF_SMI_TASK_REG_RAM_WRITE_FUNC      cmdExtDrvHwIfSmiTskRegRamWrite;
extern CPSS_EXTDRV_HW_IF_SMI_TASK_REG_VEC_READ_FUNC       cmdExtDrvHwIfSmiTskRegVecRead;
extern CPSS_EXTDRV_HW_IF_SMI_TASK_REG_VEC_WRITE_FUNC      cmdExtDrvHwIfSmiTskRegVecWrite;
extern CPSS_EXTDRV_HW_IF_SMI_TASK_WRITE_REG_FUNC          cmdExtDrvHwIfSmiTaskWriteReg;
extern CPSS_EXTDRV_HW_IF_SMI_TASK_READ_REG_FUNC           cmdExtDrvHwIfSmiTaskReadReg;
extern CPSS_EXTDRV_HW_IF_SMI_INTERRUPT_WRITE_REG_FUNC     cmdExtDrvHwIfSmiInterruptWriteReg;
extern CPSS_EXTDRV_HW_IF_SMI_INTERRUPT_READ_REG_FUNC      cmdExtDrvHwIfSmiInterruptReadReg;
extern CPSS_EXTDRV_HW_IF_SMI_DEV_VENDOR_ID_GET_FUNC       cmdExtDrvSmiDevVendorIdGet;
/*extern CPSS_EXTDRV_HW_IF_SMI_TASK_WRITE_REG_FIELD_FUNC    cmdExtDrvHwIfSmiTaskWriteRegField;*/

extern CPSS_EXTDRV_HW_IF_TWSI_INIT_DRIVER_FUNC  cmdExtDrvHwIfTwsiInitDriver;
extern CPSS_EXTDRV_HW_IF_TWSI_WRITE_REG_FUNC    cmdExtDrvHwIfTwsiWriteReg;
extern CPSS_EXTDRV_HW_IF_TWSI_READ_REG_FUNC     cmdExtDrvHwIfTwsiReadReg;

extern CPSS_EXTDRV_I2C_MGMT_MASTER_INIT_FUNC      cmdExtDrvI2cMgmtMasterInit;
extern CPSS_EXTDRV_MGMT_READ_REGISTER_FUNC        cmdExtDrvMgmtReadRegister;
extern CPSS_EXTDRV_MGMT_WRITE_REGISTER_FUNC       cmdExtDrvMgmtWriteRegister;
extern CPSS_EXTDRV_MGMT_ISR_READ_REGISTER_FUNC    cmdExtDrvMgmtIsrReadRegister;
extern CPSS_EXTDRV_MGMT_ISR_WRITE_REGISTER_FUNC   cmdExtDrvMgmtIsrWriteRegister;


extern CPSS_OS_INET_NTOHL_FUNC  cmdOsNtohl;
extern CPSS_OS_INET_HTONL_FUNC  cmdOsHtonl;
extern CPSS_OS_INET_NTOHS_FUNC  cmdOsNtohs;
extern CPSS_OS_INET_HTONS_FUNC  cmdOsHtons;
extern CPSS_OS_INET_NTOA_FUNC   cmdOsInetNtoa;

extern CPSS_OS_INT_ENABLE_FUNC    cmdOsIntEnable;
extern CPSS_OS_INT_DISABLE_FUNC   cmdOsIntDisable;
extern CPSS_OS_INT_MODE_SET_FUNC  cmdOsSetIntLockUnlock;
extern CPSS_OS_INT_CONNECT_FUNC   cmdOsInterruptConnect;

extern CPSS_OS_IO_BIND_STDOUT_FUNC cmdOsBindStdOut;
extern CPSS_OS_IO_PRINTF_FUNC      cmdOsPrintf;
extern CPSS_OS_IO_SPRINTF_FUNC     cmdOsSprintf;
extern CPSS_OS_IO_PRINT_SYNC_FUNC  cmdOsPrintSync;
extern CPSS_OS_IO_GETS_FUNC        cmdOsGets;

extern CPSS_OS_BZERO_FUNC             cmdOsBzero;
extern CPSS_OS_MEM_SET_FUNC           cmdOsMemSet;
extern CPSS_OS_MEM_CPY_FUNC           cmdOsMemCpy;
extern CPSS_OS_MEM_CMP_FUNC           cmdOsMemCmp;
extern CPSS_OS_STATIC_MALLOC_FUNC     cmdOsStaticMalloc;
extern CPSS_OS_MALLOC_FUNC            cmdOsMalloc;
extern CPSS_OS_REALLOC_FUNC           cmdOsRealloc;
extern CPSS_OS_FREE_FUNC              cmdOsFree;
extern CPSS_OS_CACHE_DMA_MALLOC_FUNC  cmdOsCacheDmaMalloc;
extern CPSS_OS_CACHE_DMA_FREE_FUNC    cmdOsCacheDmaFree;
extern CPSS_OS_PHY_TO_VIRT_FUNC       cmdOsPhy2Virt;
extern CPSS_OS_VIRT_TO_PHY_FUNC       cmdOsVirt2Phy;

extern CPSS_OS_RAND_FUNC  cmdOsRand;
extern CPSS_OS_SRAND_FUNC cmdOsSrand;

extern CPSS_OS_MUTEX_CREATE_FUNC        cmdOsMutexCreate;
extern CPSS_OS_MUTEX_DELETE_FUNC        cmdOsMutexDelete;
extern CPSS_OS_MUTEX_LOCK_FUNC          cmdOsMutexLock;
extern CPSS_OS_MUTEX_UNLOCK_FUNC        cmdOsMutexUnlock;

extern CPSS_OS_SIG_SEM_BIN_CREATE_FUNC  cmdOsSigSemBinCreate;
extern CPSS_OS_SIG_SEM_DELETE_FUNC      cmdOsSigSemDelete;
extern CPSS_OS_SIG_SEM_WAIT_FUNC        cmdOsSigSemWait;
extern CPSS_OS_SIG_SEM_SIGNAL_FUNC      cmdOsSigSemSignal;

extern CPSS_OS_STR_LEN_FUNC     cmdOsStrlen;
extern CPSS_OS_STR_CPY_FUNC     cmdOsStrCpy;
extern CPSS_OS_STR_CHR_FUNC     cmdOsStrChr;
extern CPSS_OS_STR_CMP_FUNC     cmdOsStrCmp;
extern CPSS_OS_STR_CAT_FUNC     cmdOsStrCat;
extern CPSS_OS_STR_N_CAT_FUNC   cmdOsStrNCat;
extern CPSS_OS_TO_UPPER_FUNC    cmdOsToUpper;
extern CPSS_OS_STR_TO_32_FUNC   cmdOsStrTo32;
extern CPSS_OS_STR_TO_U32_FUNC  cmdOsStrToU32;

extern CPSS_OS_TIME_WK_AFTER_FUNC  cmdOsTimerWkAfter;
extern CPSS_OS_TIME_TICK_GET_FUNC  cmdOsTickGet;
extern CPSS_OS_TIME_GET_FUNC       cmdOsTime;

extern CPSS_OS_TASK_CREATE_FUNC    cmdOsTaskCreate;
extern CPSS_OS_TASK_DELETE_FUNC    cmdOsTaskDelete;
extern CPSS_OS_TASK_LOCK_FUNC      cmdOsTaskLock;
extern CPSS_OS_TASK_UNLOCK_FUNC    cmdOsTaskUnLock;

extern CMD_OS_TASK_GET_SELF_FUN    cmdOsTaskGetSelf;
extern CMD_OS_TASK_SET_TASK_PRIOR_FUNC cmdOsSetTaskPrior;

extern CMD_OS_SOCKET_TCP_CREATE_FUN        cmdOsSocketTcpCreate;
extern CMD_OS_SOCKET_UDP_CREATE_FUN        cmdOsSocketUdpCreate;
extern CMD_OS_SOCKET_TCP_DESTROY_FUN       cmdOsSocketTcpDestroy;
extern CMD_OS_SOCKET_UDP_DESTROY_FUN       cmdOsSocketUdpDestroy;
extern CMD_OS_SOCKET_CREATE_ADDR_FUN       cmdOsSocketCreateAddr;
extern CMD_OS_SOCKET_DESTROY_ADDR_FUN      cmdOsSocketDestroyAddr;
extern CMD_OS_SOCKET_BIND_FUN              cmdOsSocketBind;
extern CMD_OS_SOCKET_LISTEN_FUN            cmdOsSocketListen;
extern CMD_OS_SOCKET_ACCEPT_FUN            cmdOsSocketAccept;
extern CMD_OS_SOCKET_CONNECT_FUN           cmdOsSocketConnect;
extern CMD_OS_SOCKET_SET_NON_BLOCK_FUN     cmdOsSocketSetNonBlock;
extern CMD_OS_SOCKET_SET_BLOCK_FUN         cmdOsSocketSetBlock;
extern CMD_OS_SOCKET_SEND_FUN              cmdOsSocketSend;
extern CMD_OS_SOCKET_SEND_TO_FUN           cmdOsSocketSendTo;
extern CMD_OS_SOCKET_RECV_FUN              cmdOsSocketRecv;
extern CMD_OS_SOCKET_RECV_FROM_FUN         cmdOsSocketRecvFrom;
extern CMD_OS_SOCKET_SET_SOCKET_NO_LINGER_FUN cmdOsSocketSetSocketNoLinger;
extern CMD_OS_SOCKET_EXTRACT_IP_ADDR_FROM_SOCKET_ADDR_FUN cmdOsSocketExtractIpAddrFromSocketAddr;
extern CMD_OS_SOCKET_GET_SOCKET_ADDR_SIZE_FUN cmdOsSocketGetSocketAddrSize;
extern CMD_OS_SOCKET_SHUT_DOWN_FUN         cmdOsSocketShutDown;

extern CMD_OS_SELECT_CREATE_SET_FUN        cmdOsSelectCreateSet;
extern CMD_OS_SELECT_ERASE_SET_FUN         cmdOsSelectEraseSet;
extern CMD_OS_SELECT_ZERO_SET_FUN          cmdOsSelectZeroSet;
extern CMD_OS_SELECT_ADD_FD_TO_SET_FUN     cmdOsSelectAddFdToSet;
extern CMD_OS_SELECT_CLEAR_FD_FROM_SET_FUN cmdOsSelectClearFdFromSet;
extern CMD_OS_SELECT_IS_FD_SET_FUN         cmdOsSelectIsFdSet;
extern CMD_OS_SELECT_COPY_SET_FUN          cmdOsSelectCopySet;
extern CMD_OS_SELECT_FUN                   cmdOsSelect;
extern CMD_OS_SOCKET_GET_SOCKET_FD_SET_SIZE_FUN cmdOsSocketGetSocketFdSetSize;


extern CMD_OS_POOL_CREATE_POOL_FUNC        cmdOsPoolCreatePool;
extern CMD_OS_POOL_CREATE_DMA_POOL_FUNC    cmdOsPoolCreateDmaPool;
extern CMD_OS_POOL_RE_CREATE_POOL_FUNC     cmdOsPoolReCreatePool;
extern CMD_OS_POOL_DELETE_POOL_FUNC        cmdOsPoolDeletePool;
extern CMD_OS_POOL_GET_BUF_FUNC            cmdOsPoolGetBuf;
extern CMD_OS_POOL_FREE_BUF_FUNC           cmdOsPoolFreeBuf;
extern CMD_OS_POOL_GET_BUF_SIZE_FUNC       cmdOsPoolGetBufSize;
extern CMD_OS_POOL_EXPAND_POOL_FUNC        cmdOsPoolExpandPool;
extern CMD_OS_POOL_GET_BUF_FREE_CNT_FUNC   cmdOsPoolGetBufFreeCnt;
extern CMD_OS_POOL_PRINT_STATS_FUNC        cmdOsPoolPrintStats;

extern CMD_CPU_ETHERNET_IS_CPU_ETHER_PORT_USED cmdIsCpuEtherPortUsed;

extern CMD_CPSS_EVENT_BIND_FUNC            cmdCpssEventBind;
extern CMD_CPSS_EVENT_SELECT_FUNC          cmdCpssEventSelect;
extern CMD_CPSS_EVENT_RECV_FUNC            cmdCpssEventRecv;
extern CMD_CPSS_EVENT_DEVICE_MASK_SET_FUNC cmdCpssEventDeviceMaskSet;



extern CMD_APP_DB_ENTRY_ADD_FUNC       cmdAppDbEntryAdd;
extern CMD_APP_DB_ENTRY_GET_FUNC       cmdAppDbEntryGet;
extern CMD_APP_DB_DUMP_FUNC            cmdAppDbDump;
extern CMD_APP_PP_CONFIG_GET_FUNC      cmdAppPpConfigGet;
extern CMD_APP_INIT_SYSTEM_FUNC        cmdInitSystem;
extern CMD_APP_INIT_CONFI_FUNC         cmdInitConfi;
extern CMD_APP_SHOW_BOARDS_LIST_FUNC   cmdAppShowBoardsList;
extern CMD_APP_IS_SYSTEM_INITIALIZED_FUNC  cmdAppIsSystemInitialized;
extern CMD_APP_PP_CONFIG_PRINT_FUNC  cmdAppPpConfigPrint;
extern CMD_APP_ALLOW_PROCESSING_OF_AUQ_MESSAGES_FUNC    cmdAppAllowProcessingOfAuqMessages;

#ifdef EXMXPM_FAMILY
extern CMD_APP_EXMXPM_EXTERNAL_TCAM_RULE_INDEX_BY_LEN_AND_LOOKUP_GET_FUNC  cmdAppExMxPmExternalTcamRuleIndexByLenAndLookupGet;
extern CMD_APP_EXMXPM_EXTERNAL_TCAM_ACTION_INDEX_GET_FUNC                  cmdAppExMxPmExternalTcamActionIndexGet;
extern CMD_APP_EXMXPM_EXTERNAL_TCAM_CLIENT_KEY_LOOKUP_SET_FUNC             cmdAppExMxPmExternalTcamClientKeyLookupSet;
extern CMD_APP_EXMXPM_EXTERNAL_TCAM_CLIENT_KEY_LOOKUP_GET_FUNC             cmdAppExMxPmExternalTcamClientKeyLookupGet;
extern CMD_APP_EXMXPM_EXTERNAL_TCAM_CLIENT_KEY_LOOKUP_SET_BY_INDEX_FUNC    cmdAppExMxPmExternalTcamClientKeyLookupSetByIndex;
extern CMD_APP_EXMXPM_EXTERNAL_TCAM_CLIENT_KEY_LOOKUP_GET_BY_INDEX_FUNC    cmdAppExMxPmExternalTcamClientKeyLookupGetByIndex;
extern CMD_APP_EXMXPM_EXTERNAL_TCAM_CONFIG_FUNC                            cmdAppExMxPmExternalTcamConfig;
extern CMD_APP_EXMXPM_EXTERNAL_HSU_OLD_IMAGE_PRE_UPDATE_PREPARATION_FUNC   cmdAppExMxPmHsuOldImagePreUpdatePreparation;
extern CMD_APP_EXMXPM_EXTERNAL_HSU_NEW_IMAGE_UPDATE_AND_SYNCH_FUNC         cmdAppExMxPmHsuNewImageUpdateAndSynch;

#endif /*EXMXPM_FAMILY*/

#ifdef CHX_FAMILY
extern CMD_APP_LION_PORT_MODE_SPEED_SET_FUNC                               cmdAppLionPortModeSpeedSet;
extern CMD_APP_XCAT2_STACK_PORTS_MODE_SPEED_SET_FUNC                       cmdAppXcat2StackPortsModeSpeedSet;
#endif

/*******************************************************************************
* cmdInitExtServices
*
* DESCRIPTION:
*       commander external services initialization
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_FAIL            - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdInitExtServices
(
    GT_VOID
);

#endif /* PSS_PRODUCT */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /*__cmdExtServices_h_*/
