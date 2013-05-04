/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* main.c
*
* DESCRIPTION:
*       Initialization functions for the simulation on Unix-like OS
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*
*******************************************************************************/
#include <ctype.h>
#define __USE_BSD
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <gtOs/gtOsGen.h>
#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsInit.h>
#include <gtOs/gtOsSharedMemory.h>
#include <gtStack/gtOsSocket.h>

#include <asicSimulation/SInit/sinit.h>
#include <os/simTypesBind.h>

#define EXPLICIT_INCLUDE_TO_SIM_OS_H_FILES
#include <os/simOsBindOwn.h>

/* commandLine should belong to simulation (simulation can be used by another client withot this var) */
#ifdef SHARED_MEMORY
extern 
#endif
char commandLine[128];
char **cmdArgv;

int DPmutex = 0;
int DPmutex_init = 0;

/* IntLockmutex should belong to simulation (simulation can be used without Enabler)! */
#ifdef SHARED_MEMORY
extern 
#endif
int IntLockmutex;

extern int osSemBinCreate(char *name, int intCount, int *smid); 
extern int osSemMCreate(char *name, int *smid);
extern int osSprintf(char * buffer, const char* format, ...);

#define BIND_FUNC(funcName)	\
			simOsBindInfo.funcName = funcName

#define BIND_LEVEL_FUNC(level,funcName) \
            simOsBindInfo.level.funcName = funcName

SIM_OS_FUNC_BIND_STC simOsBindInfo;

static void simulationBindOsFuncs(void)
{
	/* reset all fields of simOsBindInfo */
	osMemSet(&simOsBindInfo,0,sizeof(simOsBindInfo));

	/* let the simulation OS bind it's own functions to the simulation */
	simOsFuncBindOwnSimOs();

	BIND_FUNC(osWrapperOpen);
	
	BIND_LEVEL_FUNC(sockets,osSocketTcpCreate);
	BIND_LEVEL_FUNC(sockets,osSocketUdpCreate);
	BIND_LEVEL_FUNC(sockets,osSocketTcpDestroy);
	BIND_LEVEL_FUNC(sockets,osSocketUdpDestroy);
	BIND_LEVEL_FUNC(sockets,osSocketCreateAddr);
	BIND_LEVEL_FUNC(sockets,osSocketDestroyAddr);
	BIND_LEVEL_FUNC(sockets,osSocketBind);
	BIND_LEVEL_FUNC(sockets,osSocketListen);
	BIND_LEVEL_FUNC(sockets,osSocketAccept);
	BIND_LEVEL_FUNC(sockets,osSocketConnect);
	BIND_LEVEL_FUNC(sockets,osSocketSetNonBlock);
	BIND_LEVEL_FUNC(sockets,osSocketSetBlock);
	BIND_LEVEL_FUNC(sockets,osSocketSend);
	BIND_LEVEL_FUNC(sockets,osSocketSendTo);
	BIND_LEVEL_FUNC(sockets,osSocketRecv);
	BIND_LEVEL_FUNC(sockets,osSocketRecvFrom);
	BIND_LEVEL_FUNC(sockets,osSocketSetSocketNoLinger);
	BIND_LEVEL_FUNC(sockets,osSocketExtractIpAddrFromSocketAddr);
	BIND_LEVEL_FUNC(sockets,osSocketGetSocketAddrSize);
	BIND_LEVEL_FUNC(sockets,osSocketShutDown);
	
	BIND_LEVEL_FUNC(sockets,osSelectCreateSet);
	BIND_LEVEL_FUNC(sockets,osSelectEraseSet);
	BIND_LEVEL_FUNC(sockets,osSelectZeroSet);
	BIND_LEVEL_FUNC(sockets,osSelectAddFdToSet);
	BIND_LEVEL_FUNC(sockets,osSelectClearFdFromSet);
	BIND_LEVEL_FUNC(sockets,osSelectIsFdSet);
	BIND_LEVEL_FUNC(sockets,osSelectCopySet);
	BIND_LEVEL_FUNC(sockets,osSelect);
	BIND_LEVEL_FUNC(sockets,osSocketGetSocketFdSetSize);
	
	/* this needed for binding the OS of simulation with our OS functions */
	simOsFuncBind(&simOsBindInfo);

	return;
}

void simulationStart (void)
{
    extern int userAppInit(void);
    
    char   semName[50];

    /* Must be called before use any of OS functions. */
    osWrapperOpen(NULL);
    /* create semaphores */
    osSprintf(semName, "DPmutex");

    if (osSemMCreate(semName, &DPmutex ) != 0)
        exit (0);

    osSprintf(semName, "IntLockmutex");

    if (osSemBinCreate(semName, 1, &IntLockmutex) != 0)
        exit (0);

    /* mark semaphores initialized */
    DPmutex_init = 1;

	simulationBindOsFuncs();

    /* init CMD */
    userAppInit();
#ifndef RTOS_ON_SIM
    /* call simulation init */
    simulationLibInit();
#endif
}

extern GT_BOOL cmdStreamGrabSystemOutput;
int main(int argc, char * argv[])
{

	int i, len=0;

#ifdef SHARED_MEMORY
    if(shrMemInitSharedLibrary_FirstClient() != GT_OK)
        return -1;
#endif

    cmdArgv = calloc(argc+1, sizeof(char*));
    for (i=0; i<argc; i++)
    {
        len += strlen(argv[i]);
        cmdArgv[i] = argv[i];

        if (len < sizeof(commandLine)-2) /* leave place for ' ' and '\0' */
        {
            strcat(commandLine,argv[i]);

            if (i<argc-1)
            {
                strcat(commandLine," ");
                len++;
            }
        }

        /* no need to define environment variable PSEUDO to cause simulation 
         * print output on local console 
         */
        /* new key "-tty" causes all input/output be on local console */
        if (i > 0 && !strcmp(argv[i], "-tty"))
        {
            setenv("PSEUDO", "/dev/tty", 1);
        }
        cmdStreamGrabSystemOutput = GT_FALSE;

        /* new key equale to '-stdout comPort' */
        if (i > 0 && !strcmp(argv[i], "-grabstd"))
        {
            cmdStreamGrabSystemOutput = GT_TRUE;
        }

        /* support "-stdout" key like in window's simulation */
        if (i > 0 && (i+1 < argc) && !strcmp(argv[i], "-stdout"))
        {
            if (strcmp(argv[i+1], "comPort") == 0 ||
                    strcmp(argv[i+1], "comport") == 0)
            {
                cmdStreamGrabSystemOutput = GT_TRUE;
            }
        }
    }
    cmdArgv[argc] = 0;

    if (osStartEngine(argc, (const char**)argv, "appDemoSim", simulationStart) != GT_OK)
        return 1;

    return 0;
}

