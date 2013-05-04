/*******************************************************************************
*              (c), Copyright 2007, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdStream.h
*
* DESCRIPTION:
*       This file contains types and routines for mainCmd I/O streams
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __cmdStreamh
#define __cmdStreamh

#include <cmdShell/common/cmdCommon.h>

#define STREAM_THREAD_PRIO 5

/*
 * Typedef: struct cmdStreamSTC
 *
 * Description:
 *      This structure represent a I/O stream.
 *
 * Fields:
 *      destroy()     - Destroy stream
 *      read()        - Raw read from stream (blocking)
 *      readChar()    - Read character from stream (blocking)
 *      readLine()    - Read line from stream
 *      write()       - Raw write to stream
 *      writeBuf()    - Write to stream with output filtering
 *      writeLine()   - Write line to stream
 *      connected()   - stream connection status
 *      grabStd()     - Redirect system stdout/stderr to stream
 *      setTtyMode()  - Set terminal mode to raw(0) or linemode(1)
 *      getFd()       - Return file descriptor for this stream
 *      isConsole     - console flag. Line editing/echoing/prompting required
 *      flags         - I/O flags for generic methods
 *      wasCR         - internal flag for generic readLine method
 *
 */
struct cmdStreamSTC {
    /***************************************************************************
    * destroy
    *
    * DESCRIPTION:
    *       Destroy stream
    *
    * INPUTS:
    *       stream - destination stream
    *
    * OUTPUTS:
    *       None
    *
    * RETURNS:
    *       GT_OK
    *       GT_FAIL
    *
    ***************************************************************************/
    int (*destroy)(struct cmdStreamSTC* stream);

    /***************************************************************************
    * read
    *
    * DESCRIPTION:
    *       Raw read from stream (blocking)
    *       Used by readLine method
    *
    * INPUTS:
    *       stream       - destination stream
    *       bufferLength - buffer length
    *
    * OUTPUTS:
    *       bufferPtr    - pointer to output buffer
    *
    * RETURNS:
    *       < 0 if error occured
    *       number of received bytes
    *
    ***************************************************************************/
    int (*read)(struct cmdStreamSTC* stream, void* bufferPtr, int bufferLength);

    /***************************************************************************
    * readChar
    *
    * DESCRIPTION:
    *       Read character from stream (blocking)
    *
    * INPUTS:
    *       stream       - destination stream
    *       timeOut      - timeout in milliseconds, -1 means wait forever
    *
    * OUTPUTS:
    *       charPtr    - pointer to output buffer
    *
    * RETURNS:
    *       < 0 if error occured
    *       0 if timeout
    *       1 if character read
    *       number of received bytes
    *
    ***************************************************************************/
    int (*readChar)(struct cmdStreamSTC* stream, char* charPtr, int timeOut);

    /***************************************************************************
    * readLine
    *
    * DESCRIPTION:
    *       Read line from stream. Perform input editing/translations
    *       Returned line is zero terminated with stripped EOL symbols
    *
    * INPUTS:
    *       stream       - destination stream
    *       bufferLength - buffer length
    *
    * OUTPUTS:
    *       bufferPtr    - pointer to output buffer
    *
    * RETURNS:
    *       < 0 if error occured
    *       number of received bytes
    *
    ***************************************************************************/
    int (*readLine)(struct cmdStreamSTC* stream, char* bufferPtr, int bufferLength);

    /***************************************************************************
    * write
    *
    * DESCRIPTION:
    *       Raw write to stream
    *       Used by writeBuf method
    *
    * INPUTS:
    *       stream       - destination stream
    *       bufferPtr    - pointer to buffer
    *       bufferLength - number of bytes to send
    *
    * OUTPUTS:
    *       None
    *
    * RETURNS:
    *       < 0 if error occured
    *       number of bytes was sent
    *
    ***************************************************************************/
    int (*write)(struct cmdStreamSTC* stream, const void* bufferPtr, int bufferLength);

    /***************************************************************************
    * writeBuf
    *
    * DESCRIPTION:
    *       Write to stream. Perform output translations
    *
    * INPUTS:
    *       stream       - destination stream
    *       bufferPtr    - pointer to buffer
    *       bufferLength - number of bytes to send
    *
    * OUTPUTS:
    *       None
    *
    * RETURNS:
    *       < 0 if error occured
    *       number of bytes was sent
    *
    ***************************************************************************/
    int (*writeBuf)(struct cmdStreamSTC* stream, const char* s, int len);

    /***************************************************************************
    * writeLine
    *
    * DESCRIPTION:
    *       Write line to stream. Perform output translations
    *
    * INPUTS:
    *       stream       - destination stream
    *       s            - pointer to string
    *
    * OUTPUTS:
    *       None
    *
    * RETURNS:
    *       < 0 if error occured
    *       number of bytes was sent
    *
    ***************************************************************************/
    int (*writeLine)(struct cmdStreamSTC* stream, const char* s);

    /***************************************************************************
    * connected
    *
    * DESCRIPTION:
    *       Return stream connected status
    *
    * INPUTS:
    *       stream       - destination stream
    *
    * OUTPUTS:
    *       None
    *
    * RETURNS:
    *       Non zero if stream connected
    *       Zero if not connected (connection lost)
    *
    ***************************************************************************/
    int (*connected)(struct cmdStreamSTC* stream);

    /***************************************************************************
    * grabStd
    *
    * DESCRIPTION:
    *       Redirect system stdout/stderr to stream
    *
    * INPUTS:
    *       stream       - destination stream
    *
    * OUTPUTS:
    *       None
    *
    * RETURNS:
    *       Non zero if stream connected
    *       Zero if not connected (connection lost)
    *
    ***************************************************************************/
    int (*grabStd)(struct cmdStreamSTC* stream);

    /***************************************************************************
    * setTtyMode
    *
    * DESCRIPTION:
    *       Set terminal mode to raw(0) or linemode(1)
    *
    * INPUTS:
    *       stream       - destination stream
    *
    * OUTPUTS:
    *       None
    *
    * RETURNS:
    *       Non zero if stream connected
    *       Zero if not connected (connection lost)
    *
    ***************************************************************************/
    int (*setTtyMode)(struct cmdStreamSTC* stream, int mode);

    /***************************************************************************
    * getFd
    *
    * DESCRIPTION:
    *       Return file descriptor for this stream
    *
    * INPUTS:
    *       stream       - stream
    *
    * OUTPUTS:
    *       None
    *
    * RETURNS:
    *       -1 on error
    *       file descriptor
    *
    ***************************************************************************/
    int (*getFd)(struct cmdStreamSTC* stream);

    /***************************************************************************
    * isConsole
    *
    * DESCRIPTION:
    *       Application flag
    *
    ***************************************************************************/
    GT_BOOL isConsole;

    /***************************************************************************
    * flags
    *
    * DESCRIPTION:
    *       Stream I/O filtering/editing flags
    *
    ***************************************************************************/
#define STREAM_FLAG_O_CRLF   1
#define STREAM_FLAG_I_CANON  2
#define STREAM_FLAG_I_ECHO   4
    int flags;

    /***************************************************************************
    * wasCR
    *
    * DESCRIPTION:
    *       Input editing flags (internal)
    *
    ***************************************************************************/
    GT_BOOL wasCR; /* flag for readline */
};
typedef struct cmdStreamSTC cmdStreamSTC;
typedef struct cmdStreamSTC *cmdStreamPTR;

/*******************************************************************************
* cmdStreamGrabSystemOutput
*
* DESCRIPTION:
*       cmdStreamRedirectStdout() will grab stdout and stderr streams
*       if this flags set to GT_TRUE
*
*******************************************************************************/
extern GT_BOOL cmdStreamGrabSystemOutput;

/*******************************************************************************
* cmdStreamRedirectStdout
*
* DESCRIPTION:
*       Redirect stdout/stderr to given stream
*
* INPUTS:
*       stream - destination stream
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK
*       GT_FAIL
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdStreamRedirectStdout
(
    IN cmdStreamPTR stream
);


/*******************************************************************************
* cmdStreamCreateNULL
*
* DESCRIPTION:
*       Create null stream. This stream equals to /dev/null
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       stream - null stream
*
* COMMENTS:
*       None
*
*******************************************************************************/
cmdStreamPTR cmdStreamCreateNULL(void);


/*******************************************************************************
* cmdStreamSerialInit
*
* DESCRIPTION:
*       Initialize serial engine
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK
*       GT_FAIL
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdStreamSerialInit(void);

/*******************************************************************************
* cmdStreamSerialFinish
*
* DESCRIPTION:
*       Close serial engine
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK
*       GT_FAIL
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdStreamSerialFinish(void);

/*******************************************************************************
* cmdStreamCreateSerial
*
* DESCRIPTION:
*       Create serial port stream
*
* INPUTS:
*       devNum  - the serial device port number (0 = COM1, ...)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       stream - serial stream
*       NULL if error
*
* COMMENTS:
*       None
*
*******************************************************************************/
cmdStreamPTR cmdStreamCreateSerial
(
    IN GT_U32 devNum
);

/*******************************************************************************
* cmdStreamSocketInit
*
* DESCRIPTION:
*       Initialize TCP/IP socket engine
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK
*       GT_FAIL
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdStreamSocketInit(void);

/*******************************************************************************
* cmdStreamSocketFinish
*
* DESCRIPTION:
*       Close socket engine
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK
*       GT_FAIL
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdStreamSocketFinish(void);

/*******************************************************************************
* cmdStreamCreateSocket
*
* DESCRIPTION:
*       Create socket stream
*
* INPUTS:
*       socket - socket descriptor
*
* OUTPUTS:
*       None
*
* RETURNS:
*       stream - socket stream
*       NULL if error
*
* COMMENTS:
*       None
*
*******************************************************************************/
cmdStreamPTR cmdStreamCreateSocket
(
    IN GT_SOCKET_FD socket
);

/*******************************************************************************
* cmdStreamCreateTelnet
*
* DESCRIPTION:
*       Create Telnet protocol stream
*
* INPUTS:
*       socket - socket I/O stream
*
* OUTPUTS:
*       None
*
* RETURNS:
*       stream - socket stream
*       NULL if error
*
* COMMENTS:
*       None
*
*******************************************************************************/
cmdStreamPTR cmdStreamCreateTelnet
(
    IN cmdStreamPTR socket
);

#if defined(_linux) || defined(__FreeBSD__)
/*******************************************************************************
* cmdStreamCreatePipe
*
* DESCRIPTION:
*       Create pipe stream
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       stream - pipe stream
*       NULL if error
*
* COMMENTS:
*       Input and output file descriptors must be specified in environment
*       variables PIPE_STREAM_INFD and PIPE_STREAM_OUTFD
*
*******************************************************************************/
cmdStreamPTR cmdStreamCreatePipe(void);
#endif /* defined(_linux) || defined(__FreeBSD__) */


/*
 * Typedef: eventLoopHandlerPTR
 *
 * Description:
 *      Pointer to application stream handler
 */
typedef GT_STATUS (*eventLoopHandlerPTR)(
        cmdStreamPTR stream
);

/*******************************************************************************
* cmdStreamStartEventLoopHandler
*
* DESCRIPTION:
*       Create new task to run stream handler
*
* INPUTS:
*       taskName   - task name for handler
*       handler    - pointer to handler function
*       stream     - stream
*       isConsole  - application flag
*
* OUTPUTS:
*       taskId     - pointer for task ID
*
* RETURNS:
*       GT_OK
*       GT_FAIL
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdStreamStartEventLoopHandler
(
    IN  char *taskName,
    IN  eventLoopHandlerPTR handler,
    IN  cmdStreamPTR stream,
    IN  GT_BOOL isConsole,
    OUT GT_TASK *taskId
);


/*******************************************************************************
* cmdStreamSocketCreateListenerTask
*
* DESCRIPTION:
*       Create socket listenet task
*       This task will accept incoming socket connections and then start
*       handler for each connection
*
* INPUTS:
*       listenerTaskName  - name for listener task
*       ip                - ip address to listen on
*                           NULL means listen on all interfaces
*       port                tcp port for incoming connections
*       streamTaskName    - task name for handler
*       handler           - pointer to handler function
*       multipleInstances - Allow more than one handler at time
*       isConsole         - application flag
*
* OUTPUTS:
*       taskId     - pointer for listener task ID
*
* RETURNS:
*       GT_OK
*       GT_FAIL
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdStreamSocketCreateListenerTask(
    IN  char *listenerTaskName,
    IN  char *ip,
    IN  int port,
    IN  char *streamTaskName,
    IN  eventLoopHandlerPTR handler,
    IN  GT_BOOL multipleInstances,
    IN  GT_BOOL isConsole,
    OUT GT_TASK *taskId
);


/*******************************************************************************
* streamGenericReadLine
*
* DESCRIPTION:
*       Generic readLine method for cmdStreamSTC
*       Read line from stream. Perform input editing/translations
*       Returned line is zero terminated with stripped EOL symbols
*
* INPUTS:
*       stream       - destination stream
*       bufferLength - buffer length
*
* OUTPUTS:
*       bufferPtr    - pointer to output buffer
*
* RETURNS:
*       < 0 if error occured
*       number of received bytes
*
*******************************************************************************/
int streamGenericReadLine(cmdStreamPTR streamP, char* bufferPtr, int bufferLength);

/*******************************************************************************
* streamGenericWriteBuf
*
* DESCRIPTION:
*       Generic writeBuf method for cmdStreamSTC
*       Write to stream. Perform output translations
*
* INPUTS:
*       stream       - destination stream
*       s            - pointer to buffer
*       bufferLength - number of bytes to send
*
* OUTPUTS:
*       None
*
* RETURNS:
*       < 0 if error occured
*       number of bytes was sent
*
*******************************************************************************/
int streamGenericWriteBuf(cmdStreamPTR streamP, const char *s, int len);

/***************************************************************************
* streamGenericWriteLine
*
* DESCRIPTION:
*       Generic writeLine method for cmdStreamSTC
*       Write line to stream. Perform output translations
*
* INPUTS:
*       stream       - destination stream
*       s            - pointer to string
*
* OUTPUTS:
*       None
*
* RETURNS:
*       < 0 if error occured
*       number of bytes was sent
*
***************************************************************************/
int streamGenericWriteLine(cmdStreamPTR streamP, const char *s);

#endif /* __cmdStreamh */
