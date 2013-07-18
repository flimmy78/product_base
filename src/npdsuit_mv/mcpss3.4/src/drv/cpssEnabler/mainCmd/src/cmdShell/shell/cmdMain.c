/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdMain.c
*
* DESCRIPTION:
*       command shell entry point implementation
*
*
* DEPENDENCIES:
*       gtOs.h, gtOsEx.h
*
* FILE REVISION NUMBER:
*       $Revision: 1.5 $
*******************************************************************************/

/***** Include files ***************************************************/

#include <cmdShell/common/cmdCommon.h>

#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/shell/cmdMain.h>
#include <cmdShell/shell/cmdEngine.h>
#include <cmdShell/shell/cmdConsole.h>
#include <cmdShell/shell/cmdSystem.h>
#include <cmdShell/shell/cmdParser.h>
#include <cmdShell/shell/userInitCmdDb.h>
#include <cmdShell/os/cmdStream.h>
#include <cmdShell/FS/cmdFS.h>
#include <sysdef/npd_sysdef.h>

/* for virtual-banch 2012-07-28 */
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/hardware/prvCpssDrvObj.h>
/* end */
#define MAX_NUM_UNITS 128
#define MSG_EXCHANGING_BASE_SOCKET_PORT 6000


#ifdef LINUX_SIM
    #include <string.h>
#endif

/* definition for Multi-TAPI unit switching */
#define CMD_SWITCHING_BASE_SOCKET_PORT  8000

/***** Variables *******************************************************/
static GT_32 globalInit = 0;

extern cmdStreamPTR defaultStdio;

/* ports for Galtis command switching, must by initialized */
/* the value may be overridden                             */
GT_U16 cmdSwitchingServerPort = CMD_SWITCHING_BASE_SOCKET_PORT;

/* Unit to IP Mapping linked list */
/*STRUCT_LL_HEAD *cmdLlHead;*/

/* cpssInitSystem has been run */
extern GT_BOOL systemInitialized;

/* Unit is connected/not connected to Galtis */
#if (defined PSS_PRODUCT) || (defined CPSS_PRODUCT)
extern 
#else
static
#endif 
GT_BOOL terminalUnit;

/* for task thread safe */
GT_SEM parserMutex = 0;
GT_SEM serverMutex = 0;

GT_U32 taskSerial = 0;

/* Stdio sem -  only one stream can have the stdio */
GT_MUTEX  gStdioMtx;

/* handle for last cmdServer TCP                    */
/* needed for restar server when changing TCP port  */
static  GT_U32 mainTCPServerHandle = 0;

#ifndef __AX_PLATFORM__
/**
  *	Auteware support all series of products use Marvell PP devices
  *	all supported products and enumerate defition as follows:
  *		PRODUCT_ID_NONE	- 0x000000
  *		PRODUCT_ID_AX7K 	- 0xC5A000	(chassis,5 slots, marvell, cheetah2 275)
  *		PRODUCT_ID_AX5K 	- 0xB1A300	(box, 1 slots, marvell, cheetah2 265_lite<only use 12GE+2XG>)
  *		PRODUCT_ID_AX5K_I	- 0xB1A400	(box, 1 slots, marvell, cheetah2 265e_e<only use 8GE+2XG>)
  *		PRODUCT_ID_AU4K 	- 0xB1A100	(box, 1 slots, marvell, cheetah2 265)
  *		PRODUCT_ID_AU3K 	- 0xB1A200	(box, 1 slots, marvell, cheetah2 255)
  *		PRODUCT_ID_AU3K_BCM - 0xB1B000 	(box, 1 slots, broadcom, raven 56024B0)
  *
  *	NOTE:
  *		Enumerate definitions may change when new product is supported.
  *		All enumerate definitions must be consistent with type 
  *		enum product_id_e {
  *			...
  *		}
  *		defined in NPD
  */
unsigned int auteware_product_id = 0;
#endif


/***** Public Functions ************************************************/

GT_STATUS tcpKeepAliveTask(
        cmdStreamPTR stream
)
{
    char buf[100];
    int k;
    while (stream->connected(stream))
    {
        if ((k = stream->read(stream, buf, sizeof(buf))) > 0)
            stream->write(stream, buf, k);
    }
    stream->destroy(stream);
    return GT_OK;
}

/* extern */
GT_STATUS cmdEventLoop
(
    cmdStreamPTR IOStream
);

GT_STATUS tcpServerInstance(
        cmdStreamPTR stream
)
{
    /* start keepalive here */
    cmdEventLoop(stream);
    stream->destroy(stream);
    return GT_OK;
}

#ifdef CMD_LUA_CLI
GT_STATUS luaCLI_EventLoop
(
    cmdStreamPTR IOStream
);
GT_STATUS luaCLI_tcpInstance(
        cmdStreamPTR stream
)
{
    /* add telnet protocol layer */
    stream = cmdStreamCreateTelnet(stream);
    luaCLI_EventLoop(stream);
    stream->destroy(stream);
    return GT_OK;
}
#endif
#ifdef  CMD_FS
GT_STATUS cmdFileTransferServiceStart(void);
#endif
#ifdef _WIN32
cmdStreamPTR cmdStreamCreateWin32Console(void);
#endif

/*******************************************************************************
* cmdInit
*
* DESCRIPTION:
*       commander initialization (entry point); this function spawns
*       configured streaming threads and immediately returns to caller.
*
* INPUTS:
*       devNum  - The device number of the serial port.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_NO_RESOURCE     - failed to allocate memory for socket
*       GT_FAIL            - on error
*
* COMMENTS:
*
* For a new pemanent connection (like serial one) add the following: 
*   cmdStreamPTR stream; 
*   ... 
*   stream = cmdStreamCreate....(); 
*   cmdStreamStartEventLoopHandler(...); 
*   
*   To accept tcp connections on second port: 
*   cmdStreamSocketCreateListenerTask(...);
*
*******************************************************************************/
GT_STATUS cmdInit
(
    IN  GT_U32  devNum
)
{
    GT_TASK taskTCPIP = 0, taskSwitchCmd = 0, taskloop = 0;
#ifdef CMD_INCLUDE_TCPIP
    GT_TASK taskTCPKeepAlive = 0;
#endif
#ifdef CMD_INCLUDE_SERIAL
    GT_TASK taskSerial = 0;
#endif
    
    GT_STATUS retVal;

#ifndef PSS_PRODUCT
    /* initialize external services (can't call directly to mainOs functions)*/
    if(GT_OK != cmdInitExtServices())
    {
        return GT_FAIL;
    }
    /* force cmdPrintf as cmdOsPrintf */
    cmdOsPrintf = cmdPrintf;
#endif

#if 1   /* Forbid cmd to avoid osPrintf bug. 20110119*/

    #ifndef ASIC_SIMULATION
        cmdSwitchingServerPort = CMD_SWITCHING_BASE_SOCKET_PORT;
    #endif /*ASIC_SIMULATION*/

#if (defined PSS_PRODUCT) || (defined CPSS_PRODUCT)
    if (osWrapperOpen(NULL) != GT_OK)
    {   /* os wrapper failed to initialize */
        cmdOsPrintf("osWrapper initialization failure!\n");
        return GT_FAIL;
    }
#endif

    if (cmdStreamSocketInit() != GT_OK)
    {   /* os wrapper failed to initialize */
        cmdOsPrintf("cmdStreamSocketInit() initialization failure!\n");
        return GT_FAIL;
    }
#if 0   /*xcat debug 20110114*/
#ifdef CMD_INCLUDE_SERIAL
    if (cmdStreamSerialInit() != GT_OK)
    {   /* os wrapper failed to initialize */
        cmdOsPrintf("cmdStreamSocketInit() initialization failure!\n");
        return GT_FAIL;
    }
#endif
#endif

    /* was before - osMutexCreate("parser", &parserMutex) */
    if (cmdParseInit(NULL) == GT_FAIL ||
        cmdOsMutexCreate("server", &serverMutex) != GT_OK)
    {
        cmdOsPrintf("failed to create mutex descriptors!\n");
        return GT_FAIL;
    }

    /* check if we already running */
    if (globalInit++)
        return GT_FAIL;

    /* initialize commander database */
    if (cmdCpssInitDatabase() != GT_OK)
    {
        cmdOsPrintf("failed to initialize command database!\n");
        return GT_FAIL;
    }
    /* add test app level commands to commander database */
    if (cmdUserInitDatabase() != GT_OK)
    {
        cmdOsPrintf("failed to initialize command database!\n");
        return GT_FAIL;
    }
    if (cmdEngineInit() != GT_OK)
    {
            cmdOsPrintf("failed to initialize command engine!\n");
       return GT_FAIL;
    }
    terminalUnit = GT_TRUE;
	
	#if 0   /* Jump it, for CMCC test. zhangdi@autelan.com 2012-07-03  */
    if ((retVal = cmdStreamSocketCreateListenerTask(
                    "CMD_Switching",
                    "127.0.0.1", cmdSwitchingServerPort,
                    "CMD_Switching_",
                    tcpServerInstance,
                    GT_FALSE/*multipleInstances*/,
                    GT_FALSE/*cmdOsConsole*/,
                    &taskSwitchCmd)) != GT_OK)

    {
        /* do nothing */
    }
    else
    {
        /* If connection is requested, then commands are not executed directly
           on this unit */
        terminalUnit = GT_FALSE;
    }
    #endif
	
    /* Allocate stdio semaphore */
    if (cmdOsMutexCreate("cmdStdioMtx", &gStdioMtx) != GT_OK)
    {
        cmdOsPrintf("failed to initialize stdio semaphore!\n");
        return GT_FAIL;
    }

#if 0   /*xcat debug 20110111*/
#ifdef CMD_INCLUDE_SERIAL
    defaultStdio = cmdStreamCreateSerial(devNum);
#else
    defaultStdio = cmdStreamCreateNULL();
#endif /* CMD_INCLUDE_SERIAL */
#ifdef  _WIN32
    if (devNum == 0)
    {
        /* if appDemo started with -serial 0 use console */
        if (defaultStdio != NULL)
        {
            defaultStdio->destroy(defaultStdio);
        }
        defaultStdio = cmdStreamCreateWin32Console();
    }
#endif
    printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);/*xcat debug*/

    if (defaultStdio == NULL)
    {
        defaultStdio = cmdStreamCreateNULL();
    }
    cmdStreamRedirectStdout(defaultStdio);
    printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);/*xcat debug*/

#if 0
#ifdef _WIN32
    {
        extern GT_U32 simulationInitReady;

        while(simulationInitReady == 0)
        {
            /* wait for simulation to be in operational state */
            cmdOsTimerWkAfter(500);
        }
    }
#endif /*_WIN32*/
#endif


#ifdef CMD_INCLUDE_SERIAL
    /* initialize serial server thread */
    if (cmdStreamStartEventLoopHandler(
                "SERIAL",
                cmdEventLoop,
                defaultStdio,
                GT_TRUE/*isConsole*/,
                &taskSerial) != GT_OK)
    {
        cmdOsPrintf("commander: failed to spawn serial thread\n");
        taskSerial = 0;
    }
#endif /* CMD_INCLUDE_SERIAL */
#endif

#if defined(_linux) || defined(__FreeBSD__)
    {
        cmdStreamPTR pipe;
        GT_TASK taskPipe = 0;

        pipe = cmdStreamCreatePipe();
        if (pipe != NULL)
        {
            /* if pipe descriptors passed 
			 initialize pipe server thread */
			if (cmdStreamStartEventLoopHandler(
                    "PIPE",
                    cmdPipeEventLoop,
                    pipe,
                    GT_TRUE/*isConsole*/,
                    &taskPipe) != GT_OK)
			{
				cmdOsPrintf("commander: failed to spawn pipe thread\n");
			}
		}
    }
#endif

#ifdef CMD_INCLUDE_TCPIP
#ifdef  CMD_LUA_CLI
    /* initialize LUA CLI */
    if (cmdStreamSocketCreateListenerTask(
                "SERVER",
                NULL, 12345/*port*/,
                "SERVER_",
                luaCLI_tcpInstance,
                GT_TRUE/*multipleInstances*/,
                GT_TRUE/*isConsole*/,
                &taskTCPIP) != GT_OK)
    {
        cmdOsPrintf("commander: failed to create LUA CLI listening task\n");
    }
#endif
#ifdef  CMD_FS
    cmdFSinit();
    cmdFileTransferServiceStart();
#endif
    #if 0   /* Jump it, for CMCC test. zhangdi@autelan.com 2012-07-03  */
    /* initialize tcp/ip server thread */
    if (cmdStreamSocketCreateListenerTask(
                "SERVER",
                NULL, CMD_SERVER_PORT,
                "SERVER_",
                tcpServerInstance,
                GT_TRUE/*multipleInstances*/,
                GT_TRUE/*isConsole*/,
                &taskTCPIP) != GT_OK)
    {
        cmdOsPrintf("commander: failed to create tcpip server thread\n");
        taskTCPIP = 0;
    }
    else
    {
        mainTCPServerHandle = taskTCPIP;
    }
    if (cmdStreamSocketCreateListenerTask(
                "TCPKEEPALIVE",
                NULL, CMD_KA_SERVER_PORT,
                "TCPKEEPALIVE_",
                tcpKeepAliveTask,
                GT_TRUE/*multipleInstances*/,
                GT_FALSE/*isConsole*/,
                &taskTCPKeepAlive) != GT_OK)
    {
        cmdOsPrintf("commander: failed to create tcpip keepalive task thread\n");
    }
	#endif
#endif /* CMD_INCLUDE_TCPIP */
#ifndef AX_WITH_GALTIS
    appDemoShowBoardsList();
#endif

#if 0
    cmdOsPrintf("commander: threads are running: %s%s\n\n",
        (taskTCPIP|taskSerial) == 0 ? "NONE" :
        (taskTCPIP)  ? "TCPIP " : "",
        (taskSerial) ? "SERIAL" : "");


    /* initialize remote ip detection thread */
    if (cmdRemoteIpDetectionServerCreate() != GT_OK)
    {
        cmdOsPrintf("commander: fail to create remoteip detection server thread\n");
    }
#endif

    
#ifdef AX_WITH_GALTIS
    /* initialize remote ip detection thread */
    if (cmdRemoteIpDetectionServerCreate() != GT_OK)
    {
        osPrintf("commander: fail to create remoteip detection server thread\n");
    }
	osPrintf("cmdInit::remoteip detection server thread created.\n");
#endif

#endif /* Forbid cmd to avoid osPrintf bug. 20110119*/

#ifndef __AX_PLATFORM__
	/* initialize cpss */
	if((PRODUCT_ID_AX7K_I == auteware_product_id) || /* PRODUCT_ID_AX7K_I_ALPHA */
		(PRODUCT_ID_AX7K == auteware_product_id)) 
	{
        if(BOARD_TYPE == BOARD_TYPE_AX71_2X12G12S)
        {
            if((retVal=cpssInitSystem(19,1,0))!=GT_OK)
            {
    			printf("cpssInitSystem(19,1,0) failed %d\n",retVal);
    			return retVal;
    		}
    		printf("cmdInit::cpssInitSystem(19,1,0) ok\n");
			
        }
		/* PRODUCT_ID_AX7K */
		else
		{
			if((retVal=cpssInitSystem(14,1,0))!=GT_OK)
			{
    			osPrintf("cpssInitSystem(14,1,0) failed %d\n",retVal);
    			return retVal;
			}
			osPrintf("cmdInit::cpssInitSystem(14,1,0) ok\n");	
		}
	}
	else if((PRODUCT_ID_AX5K == auteware_product_id)|| /* PRODUCT_ID_AX5K */
			(PRODUCT_ID_AX5K_I == auteware_product_id)|| /* PRODUCT_ID_AX5K_I */
			(PRODUCT_ID_AU4K == auteware_product_id)|| /* PRODUCT_ID_AU4K */
			(PRODUCT_ID_AU3K == auteware_product_id)) { /* PRODUCT_ID_AU3K */
		if((retVal=cpssInitSystem(14,1,0))!=GT_OK)
		{
			osPrintf("cpssInitSystem(14,1,0) failed %d\n",retVal);
			return retVal;
		}
		osPrintf("cmdInit::cpssInitSystem(14,1,0) ok\n");
	}
	else if((BOARD_TYPE == BOARD_TYPE_AX71_2X12G12S)||(BOARD_TYPE == BOARD_TYPE_AX81_1X12G12S))
    {
        if((retVal=cpssInitSystem(19,1,0))!=GT_OK)
        {
			printf("cpssInitSystem(19,1,0) failed %d\n",retVal);
			return retVal;
		}
		printf("cmdInit::cpssInitSystem(19,1,0) ok\n");
    }
    else if(BOARD_TYPE == BOARD_TYPE_AX81_AC12C)
    {
        if((retVal=cpssInitSystem(19,1,0))!=GT_OK)
        {
			printf("cpssInitSystem(19,1,0) failed %d\n",retVal);
			return retVal;
		}
		printf("cmdInit::cpssInitSystem(19,1,0) ok\n");
    }
    else if(BOARD_TYPE == BOARD_TYPE_AX81_AC8C)
    {
        if((retVal=cpssInitSystem(19,1,0))!=GT_OK)
        {
			printf("cpssInitSystem(19,1,0) failed %d\n",retVal);
			return retVal;
		}
		printf("cmdInit::cpssInitSystem(19,1,0) ok\n");
    }		
    else if(BOARD_TYPE == BOARD_TYPE_AX81_SMU)
    {
        if((retVal=cpssInitSystem(20,1,0))!=GT_OK)
        {
			printf("cpssInitSystem(20,1,0) failed %d\n",retVal);
			return retVal;
		}
		printf("cmdInit::cpssInitSystem(20,1,0) ok\n");
    }    
    else if(BOARD_TYPE == BOARD_TYPE_AX81_12X)
    {
        if((retVal=cpssInitSystem(20,1,0))!=GT_OK)
        {
			printf("cpssInitSystem(20,1,0) failed %d\n",retVal);
			return retVal;
		}
		printf("cmdInit::cpssInitSystem(20,1,0) ok\n");
    }
	else if(BOARD_TYPE == BOARD_TYPE_AX81_AC4X)
    {
        if((retVal=cpssInitSystem(20,1,0))!=GT_OK)
        {
			printf("cpssInitSystem(20,1,0) failed %d\n",retVal);
			return retVal;
		}
		printf("cmdInit::cpssInitSystem(20,1,0) ok\n");
    }
	else if(BOARD_TYPE == BOARD_TYPE_AX81_SMUE)
    {
        if((retVal=cpssInitSystem(20,1,0))!=GT_OK)
        {
			printf("cpssInitSystem(20,1,0) failed %d\n",retVal);
			return retVal;
		}
		printf("cmdInit::cpssInitSystem(20,1,0) ok\n");
    }	
    else
    {
		if((retVal=cpssInitSystem(19,1,0))!=GT_OK)
        {
			osPrintf("cpssInitSystem(19,1,0) failed %d\n",retVal);
    		for(taskloop = 0; taskloop < PRV_CPSS_MAX_PP_DEVICES_CNS; taskloop++)                         
            {
                prvCpssDrvPpObjConfig[taskloop] = NULL;
            }
    		for(taskloop = 0; taskloop < PRV_CPSS_MAX_PP_DEVICES_CNS; taskloop++)
            {
                prvCpssDrvPpConfig[taskloop] = NULL;
            }
		    if((retVal=cpssInitSystem(20,1,0))!=GT_OK)
            {
				osPrintf("cpssInitSystem(20,1,0) failed %d\n",retVal);
				osPrintf("cmdInit::cpssInitSystem(Unknown Type)\n");
		    }
			else
			{
				osPrintf("cmdInit::cpssInitSystem(20,1,0) ok\n");
			}
		}
		else
		{
			osPrintf("cmdInit::cpssInitSystem(19,1,0) ok\n");
		}
    }	
	
#endif

    return GT_OK;
}

#if 0
/*******************************************************************************
* cmdTcpPortSet
*
* DESCRIPTION:
*       change cmdServer TCP port. Old server is closed, new server is started
*       on specified TCP port.
*
* INPUTS:
*       tcpPort  -   new TCP/IP port for command console server
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK     -   TCP port changed was changed successfully.
*       GT_FAIL   -   error. Can happen on following reason:
*                     OS_MAX_TASKS limit exceeded - can't start new server tasks
*
* COMMENTS:
*       This function can be executed only after cmdInit() call.
*       This command can be executed from console.
*
*******************************************************************************/
GT_STATUS cmdTcpPortSet
(
    IN GT_U16 tcpPort
)
{
    GT_STATUS   res = GT_OK;
#ifdef CMD_INCLUDE_TCPIP
    GT_STATUS   st;

    if (0 == mainTCPServerHandle)
    {
        cmdOsPrintf("failed to change cmd TCP Port - cmd TCP server was not created.\n");
        return GT_FAIL;
    }

    st = cmdServerDelete(mainTCPServerHandle);
    if (GT_OK == st)
    {
        cmdOsPrintf("cmd TCP server deleted\n");
    }
    else
    {
        cmdOsPrintf("Failed to delete cmd TCP server\n");
        res = GT_FAIL;
    }

    /* initialize tcp/ip server thread */
    if (cmdServerCreate(tcpPort, CMD_KA_SERVER_PORT, cmdEventLoopThread,
        GT_FALSE, &mainTCPServerHandle) != GT_OK)
    {
        cmdOsPrintf("commander: failed to create tcpip server thread\n");
        mainTCPServerHandle = 0;
        res = GT_FAIL;
    }
    else
    {
        cmdOsPrintf("new cmd TCP server started on port %d\n", tcpPort);
    }
#endif /* CMD_INCLUDE_TCPIP */

    return res;
}   /* tcpPortSet   */
#endif


