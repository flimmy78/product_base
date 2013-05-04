/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* appDemoBoardConfig.c
*
* DESCRIPTION:
*       Includes board specific initialization definitions and data-structures.
*
* FILE REVISION NUMBER:
*       $Revision: 1.3 $
*
*******************************************************************************/
#include <gtExtDrv/drivers/gtIntDrv.h>
#include <gtExtDrv/drivers/gtPciDrv.h>
#include <gtExtDrv/drivers/gtCacheMng.h>
#include <gtExtDrv/drivers/gtSmiHwCtrl.h>
#include <gtExtDrv/drivers/gtTwsiHwCtrl.h>
#include <gtExtDrv/drivers/gtDmaDrv.h>
#include <gtExtDrv/drivers/gtEthPortCtrl.h>
#include <gtExtDrv/drivers/gtHsuDrv.h>
#include <gtExtDrv/drivers/gtI2cDrv.h>
#include <gtExtDrv/drivers/gtHwIfDrv.h>
#include <gtExtDrv/drivers/gtDragoniteDrv.h>

#include <cpss/generic/init/cpssInit.h>
#include <cpss/extServices/cpssExtServices.h>

#include <gtOs/gtOsExc.h>
#include <gtOs/gtOsInet.h>
#include <gtStack/gtOsSocket.h>
#include <gtOs/gtOsGen.h>

#ifdef  LINUX
#include <gtOs/gtOsStdLib.h>
#endif

#include <gtOs/gtOsMsgQ.h>
#include <gtUtil/gtBmPool.h>
#include <gtOs/gtOsStr.h>
#include <gtOs/gtOsIo.h>
#include <gtOs/gtOsTimer.h>
#include <gtOs/gtOsStdLib.h>

#include <appDemo/sysHwConfig/appDemoDb.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/confi/confi2.h>

#include <appDemo/userExit/userEventHandler.h>

/* For software CPSS version information */
#include <cpss/generic/version/cpssGenVersion.h>
#include <cpss/generic/version/cpssGenStream.h>

#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpss/generic/cpssHwInit/private/prvCpssHwInit.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>

#if defined(SHARED_MEMORY)
#if (!defined(LINUX_SIM))
/* Some ABI for BM with shared memory should use work-around for pointers to functions */
#include <gtOs/gtOsSharedFunctionPointers.h>
#endif

#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#endif



#ifdef IMPL_FA
#include <cpssFa/generic/version/gtFaVersion.h>
#endif  /* IMPL_FA */

#ifdef IMPL_XBAR
#include <cpssXbar/generic/version/gtXbarVersion.h>
#endif  /* IMPL_XBAR */

#if (defined EX_FAMILY) || (defined MX_FAMILY)
#include <cpss/exMx/exMxGen/version/cpssExMxVersion.h>
#endif /*(defined EX_FAMILY) || (defined MX_FAMILY) */

#ifdef CHX_FAMILY
#include <cpss/dxCh/dxChxGen/version/cpssDxChVersion.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#endif  /* CHX_FAMILY */

#ifdef SAL_FAMILY
#include <cpss/dxSal/version/cpssDxSalVersion.h>
#endif  /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
#include <cpss/exMxPm/exMxPmGen/version/cpssExMxPmVersion.h>
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmInfo.h>
#include <appDemo/utils/extTcam/appDemoExMxPmExternalTcamConfig.h>
#include <appDemo/utils/hsu/appDemoExMxPmHsuUtils.h>
#endif  /* EXMXPM_FAMILY */

#ifdef SHARED_MEMORY
extern APP_DEMO_SYS_CONFIG_STC appDemoSysConfig;
CPSS_DXCH_MP_SMI_TRANSMIT_PARAMS_STC ethPortParams;
#endif /*SHLIB */

#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include <sys/un.h>
#include <signal.h>

/* Default memory size */
#define APP_DEMO_OSMEM_DEFAULT_MEM_INIT_SIZE (2048*1024)

/* signal number for device traffic enable */
#define APP_DEMO_TRAFFIC_ENABLE_SIGNUM	(SIGRTMIN+1)


#include <cmdShell/common/cmdCommon.h>
#include <appDemo/userExit/userEventHandler.h>

#include <appDemo/utils/appDemoFdbUpdateLock.h>

#include <cpss/generic/config/private/prvCpssConfigTypes.h>

#define LINUX   /* cpss1.3 - for compiling. */
#if (!defined(FREEBSD) && !defined(UCLINUX) && !defined(LINUX) && !defined(_WIN32) && !defined(ASIC_SIMULATION))
/* This is standard VxWorks function which returns the BSP version */
extern char *sysBspRev(void);
#endif

#if (defined CHX_FAMILY) && (!defined EX_FAMILY)
GT_STATUS gtAppDemoLionPortModeSpeedSet(GT_U8 devNum, GT_U8 portNum, CPSS_PORT_INTERFACE_MODE_ENT ifMode, CPSS_PORT_SPEED_ENT speed);
GT_STATUS gtAppDemoXcat2StackPortsModeSpeedSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode,
    IN  CPSS_PORT_SPEED_ENT  speed
);
#endif

/**********************************************************************************
        if multiProcessAppDemo = 0 the appDemo run as regular (one process)
        if multiProcessAppDemo = 1 the appDemo run as multiprocess application
                the multiprocess application includes:
                - appDemo process
                - fdb learning process
                - RxTx process
        the variable multiProcessAppDemo used in file userEventHandler.c to define
        the event handler configuration
*********************************************************************************/
int multiProcessAppDemo = 0;


/*******************************************************************************
 * External variables
 ******************************************************************************/
GT_U32                          appDemoPpConfigDevAmount;
unsigned int 					asicType[PRV_CPSS_MAX_PP_DEVICES_CNS];
CPSS_PP_CONFIG_INIT_STC         ppUserLogicalConfigParams[PRV_CPSS_MAX_PP_DEVICES_CNS];
extern APP_DEMO_PP_CONFIG       appDemoPpConfigList[];
extern GT_BOOL                  systemInitialized;
extern GT_BOARD_LIST_ELEMENT    boardsList[];
extern GT_U32                   boardListLen;   /* Length of boardsList array.  */
/* Enable printing inside interrupt routine. */
extern GT_STATUS extDrvUartInit
(
    GT_VOID
);

/* Initialize Unit Test framework for CPSS */
extern GT_STATUS utfPreInitPhase
(
    GT_VOID
);

extern GT_STATUS utfInit
(
    GT_U8 firstDevNum
);

#ifdef IMPL_DUNE
extern GT_STATUS duneUtfPreInitPhase
(
    GT_VOID
);

extern GT_STATUS duneUtfInit
(
    GT_VOID
);
#endif /* IMPL_DUNE */

#ifdef  INCLUDE_UTF
CPSS_OS_MUTEX appDemoFdbUpdateLockMtx;
#endif

extern GT_VOID utfPostInitPhase
(
    GT_STATUS st
);


/* when appDemoOnDistributedSimAsicSide == 1
   this image is running on simulation:
   1. our application on distributed "Asic side"
   2. the Other application that run on the "application side" is in
      charge on all Asic configurations
   3. our application responsible to initialize the cpss DB,
      to allow "Galtis wrappers" to be called on this side !
      (because the Other side application may have difficulty running
       "Galtis wrappers" from it's side)
*/
GT_U32  appDemoOnDistributedSimAsicSide = 0;

#ifdef ASIC_SIMULATION
    #include <gtStack/gtOsSocket.h>
    /* Simulation H files */
    #include <common/Version/sstream.h>
    #include <asicSimulation/SInit/sinit.h>
    #include <os/simTypesBind.h>
    #define EXPLICIT_INCLUDE_TO_SIM_OS_H_FILES
    #include <os/simOsBindOwn.h>
    #undef EXPLICIT_INCLUDE_TO_SIM_OS_H_FILES

    #include <asicSimulation/SDistributed/sdistributed.h>

    #define BIND_FUNC(funcName) \
            simOsBindInfo.funcName = funcName

    #define BIND_LEVEL_FUNC(level,funcName)     \
            simOsBindInfo.level.funcName = funcName
#endif /*ASIC_SIMULATION*/


#define BIND_APP_LEVEL_FUNC(infoPtr,level,funcName)     \
        infoPtr->level.funcName = funcName

#define BIND_APP2_LEVEL_FUNC(infoPtr,level,funcInLevel,funcName)     \
        infoPtr->level.funcInLevel = funcName

#define APP_DEMO_CV_INIT_MASK 0xC8
/*******************************************************************************
 * Local usage variables
 ******************************************************************************/
static GT_BOOL  appDemoPrePhase1Init = GT_FALSE;
static GT_BOOL  gReloadEeprom;
static GT_BOOL  gIsB2bSystem;
static GT_BOOL  appDemoCpssInitialized = GT_FALSE;
/* do we bypass the initialization of the events */
GT_BOOL appDemoBypassEventInitialize = GT_FALSE;
/* pointer to the function that allow override of setting done by appDemoSysConfigFuncsGet */
/* TBD - not supported yet */
APP_DEMO_SYS_CONFIG_FUNCS_EXTENTION_GET_FUNC appDemoSysConfigFuncsExtentionGetPtr = NULL;


/* macro to force casting between 2 functions prototypes */
#define FORCE_FUNC_CAST (void*)

/* Do common initialization for all families */
static GT_STATUS appDemoInitGlobalModuls
(
    IN  GT_U32  numOfMemBytes,
    IN  GT_U32  reloadEeprom
);

/* Cpss initialization */
static GT_STATUS appDemoCpssInit
(
    GT_VOID
);

/* if not used shared memory or it's unix-like simulation do compile this */
#if (!defined(SHARED_MEMORY) || defined(LINUX_SIM))
static GT_STATUS cpssGetDefaultExtDrvFuncs(
    OUT CPSS_EXT_DRV_FUNC_BIND_STC  *extDrvFuncBindInfoPtr
);


static GT_STATUS cpssGetDefaultOsBindFuncs(
    OUT CPSS_OS_FUNC_BIND_STC *osFuncBindPtr
);
#endif

/* HW phase 1 initialization */
static GT_STATUS appDemoBoardPhase1Init
(
    IN  GT_U8                   boardRevId,
    IN  GT_BOARD_CONFIG_FUNCS   *boardCfgFuncs,
    IN  GT_U8                   numOfDev,
    IN  GT_U8                   numOfFa,
    IN  GT_U8                   numOfXbar
);

/* HW phase 2 initialization */
static GT_STATUS appDemoBoardPhase2Init
(
    IN  GT_U8                   boardRevId,
    IN  GT_BOARD_CONFIG_FUNCS   *boardCfgFuncs,
    IN  GT_U8                   numOfDev,
    IN  GT_U8                   numOfFa,
    IN  GT_U8                   numOfXbar
);

/* Logical phase initialization */
static GT_STATUS appDemoBoardLogicalInit
(
    IN  GT_U8                   boardRevId,
    IN  GT_BOARD_CONFIG_FUNCS   *boardCfgFuncs,
    IN  GT_U8                   numOfDev,
    IN  GT_U8                   numOfFa,
    IN  GT_U8                   numOfXbar
);

/* General phase initialization */
static GT_STATUS appDemoBoardGeneralInit
(
    IN  GT_U8                   boardRevId,
    IN  GT_BOARD_CONFIG_FUNCS   *boardCfgFuncs,
    IN  GT_U8                   numOfDev,
    IN  GT_U8                   numOfFa,
    IN  GT_U8                   numOfXbar
);

/* Updates PP phase 1 params according to app demo database. */
static GT_STATUS appDemoUpdatePpPhase1Params
(
    INOUT CPSS_PP_PHASE1_INIT_PARAMS    *paramsPtr
);

/* Updates PP phase 2 params according to app demo database. */
static GT_STATUS appDemoUpdatePpPhase2Params
(
    INOUT CPSS_PP_PHASE2_INIT_PARAMS    *paramsPtr
);

/* Updates PP logical init params according to app demo database. */
static GT_STATUS appDemoUpdatePpLogicalInitParams
(
    INOUT CPSS_PP_CONFIG_INIT_STC       *paramsPtr
);

/* Updates lib init params according to app demo database. */
static GT_STATUS appDemoUpdateLibInitParams
(
    INOUT APP_DEMO_LIB_INIT_PARAMS      *paramsPtr
);

void appDemoRtosOnSimulationInit
(
    void
);


/* DB to store device type id */
static GT_U32   deviceIdDebug[PRV_CPSS_MAX_PP_DEVICES_CNS];

/* Flag for using device id from deviceIdDebug array */
static GT_BOOL  useDebugDeviceId = GT_FALSE;

/* flag to state that the trace will print also functions that return GT_OK */
/* usually we not care about 'GT_OK' only about fails */
/* the flag can be changed before running the cpssInitSystem(...) --> from terminal vxWorks */
GT_U32 appDemoTraceOn_GT_OK = 1;

/*******************************************************************************
* appDemoTraceOn_GT_OK_Set
*
* DESCRIPTION:
*       This function configures 'print returning GT_OK' functions flag
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
*       - in unix-like systems special function needed to configure appDemoTraceOn_GT_OK
*           specific ifdef's needless, because this function can't harm anything
*       - must be external to ensure it's not ommited by optimization.
*******************************************************************************/
GT_VOID appDemoTraceOn_GT_OK_Set
(
    GT_U32 enable
)
{
    appDemoTraceOn_GT_OK = enable;
    return;
}

/*******************************************************************************
 * Public API implementation
 ******************************************************************************/

/*******************************************************************************
* appDemoShowCpssSwVersion
*
* DESCRIPTION:
*       This function returns CPSS SW version. Also, it calls
*       appDemoCpssInit() for CPSS initialization.
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
*
*******************************************************************************/
GT_STATUS appDemoShowCpssSwVersion(GT_VOID)
{
    GT_STATUS               rc = GT_OK;

#ifndef DUNE_FE_IMAGE
    CPSS_VERSION_INFO_STC   versionInfo;    /* Holds CPSS versions SW */
#endif /* DUNE_FE_IMAGE */

    /*****************************************************************/
    /* CPSS general initialization                                   */
    /*****************************************************************/
    rc = appDemoCpssInit();
    if (rc != GT_OK)
    {
        return rc;
    }

    #if (!defined(FREEBSD) && !defined(UCLINUX) && !defined(LINUX) && !defined(_WIN32) && !defined(ASIC_SIMULATION))
    /* This is standard VxWorks function which returns the BSP version */
    cpssOsPrintf("\nBSP  Version: %s", sysBspRev());
    #endif
    cpssOsPrintf("\nCPSS Version Stream: %s\n",CPSS_STREAM_NAME_CNS);

    #ifdef ASIC_SIMULATION
        cpssOsPrintf("\nCHIP SIMULTATION Version Stream: %s\n",CHIP_SIMULATION_STREAM_NAME_CNS);
    #endif

#ifndef DUNE_FE_IMAGE

#ifdef IMPL_FA
    rc = gtFaVersion(&versionInfo);
    if (rc != GT_OK)
    {
        return rc;
    }
#endif  /* IMPL_FA */

#ifdef IMPL_XBAR
{
    GT_VERSION xbarVersion;
    rc = gtXbarVersion(&xbarVersion);
    if (rc != GT_OK)
    {
        return rc;
    }
}
#endif  /* IMPL_XBAR */


#if (defined EX_FAMILY) || (defined MX_FAMILY)
    rc = cpssExMxVersionGet(&versionInfo);
#endif /*(defined EX_FAMILY) || (defined MX_FAMILY) */

#ifdef CHX_FAMILY
    rc = cpssDxChVersionGet(&versionInfo);
#endif  /* CHX_FAMILY */

#ifdef SAL_FAMILY
    rc = cpssDxSalVersionGet(&versionInfo);
#endif  /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    rc = cpssExMxPmVersionGet(&versionInfo);
#endif  /* EXMXPM_FAMILY */

#endif  /* ifndef DUNE_FE_IMAGE */

    return rc;
}

/*******************************************************************************
* appDemoShowSingleBoard
*
* DESCRIPTION:
*       This function displays single board which have an automatic
*       initialization support.
*
* INPUTS:
*       pBoardInfo - (pointer to) the board info
*       id - the first parameter of gtInitSystem function
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
*
*       | 02 - DB-MX610-48F4GS          |                       |
*       |                               | 01 - Rev 1.0          |
*       |                               | 02 - Rev 2.5          |
*       +-------------------------------+-----------------------+
*
*******************************************************************************/
GT_STATUS appDemoShowSingleBoard(
    IN GT_BOARD_LIST_ELEMENT   *pBoardInfo,
    IN GT_U32   id
)
{
    GT_U32  j; /* Loops index.                 */

    osPrintf("| %02d - %-27s",id,pBoardInfo->boardName);
    osPrintf("|%-25c|\n",' ');

    for(j = 0; j < pBoardInfo->numOfRevs; j++)
    {
        if(0 == osStrCmp(GT_BOARD_LIST_NOT_USE_REVISION_STR_CNS , pBoardInfo->boardRevs[j]))
        {
            /* skip the not used revisions */
            continue;
        }

        osPrintf("|%-33c|",' ');
        osPrintf(" %02d - %-19s|\n",j + 1,pBoardInfo->boardRevs[j]);
    }
    osPrintf("+---------------------------------+-------------------------"
             "+\n");

    return GT_OK;
}

/*******************************************************************************
* appDemoShowBoardsList
*
* DESCRIPTION:
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
GT_STATUS appDemoShowBoardsList(GT_VOID)
{
    GT_BOARD_LIST_ELEMENT   *pBoardInfo;    /* Holds the board information. */
    #ifndef __AX_PLATFORM__
    GT_U32                  i;              /* Loops index.                 */
    GT_U32                  j;              /* Loops index.                 */
    #endif
    GT_STATUS               rc = GT_OK;

    rc = appDemoShowCpssSwVersion();
    if (rc != GT_OK)
    {
        return rc;
    }
    #ifndef __AX_PLATFORM__
    osPrintf("\nSupported boards:\n");
    osPrintf("+---------------------------------+-------------------------+\n");
    osPrintf("| Board name                      | Revisions               |\n");
    osPrintf("+---------------------------------+-------------------------+\n");

    for(i = 0; i < boardListLen; i++)
    {
        pBoardInfo = &(boardsList[i]);
        if(osStrlen(pBoardInfo->boardName) == 0)
        {
            continue;
        }

        /* print the line */
        appDemoShowSingleBoard(pBoardInfo,i+1);
    }
    osPrintf("\n");

    osPrintf("Call cpssInitSystem(index,boardRedId,reloadEeprom), where:\n");
    osPrintf("\tindex        - The index of the system to be initialized.\n");
    osPrintf("\tboardRevId   - The index of the board revision.\n");
    osPrintf("\treloadEeprom - Whether the device's eeprom should be reloaded \
              \t\t\tafter start-init.\n");

    osPrintf("\n");
    #endif
    return GT_OK;
} /* appDemoShowBoardsList */

/*******************************************************************************
* cpssInitSystem
*
* DESCRIPTION:
*       This is the main board initialization function.
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
GT_STATUS cpssInitSystem
(
    IN  GT_U32  boardIdx,
    IN  GT_U32  boardRevId,
    IN  GT_U32  reloadEeprom
)
{
    GT_STATUS               rc = GT_OK;
    GT_BOARD_LIST_ELEMENT   *pBoardInfo;    /* Holds the board information              */
    GT_BOARD_CONFIG_FUNCS   boardCfgFuncs;  /* Board specific configuration functions.  */
    GT_U8                   numOfPp;        /* Number of Pp's in system.                */
    GT_U8                   numOfFa;        /* Number of Fa's in system.                */
    GT_U8                   numOfXbar;      /* Number of Xbar's in system.              */
    GT_U32                  numOfMem;       /* Memory size in bytes.                    */
    GT_U32                  value;/*tmp value*/
	unsigned int 			devNum;

    printf("********cpssInitSystem(%d, %d, %d)*********\n",
            boardIdx, boardRevId, reloadEeprom);

    /*   set multiprocess or regular appDemo      */
    if ( (boardIdx & APP_DEMO_CV_INIT_MASK) == APP_DEMO_CV_INIT_MASK)
    {
        appDemoPrePhase1Init = GT_TRUE;
        boardIdx = boardIdx - APP_DEMO_CV_INIT_MASK;
    }

#ifdef SHARED_MEMORY

    if(reloadEeprom == 2)
        multiProcessAppDemo = 1;
    else
        multiProcessAppDemo = 0;

#endif


    /****************************************************/
    /* Check parameters                                 */
    /****************************************************/
    if ((boardIdx > boardListLen) || (boardIdx == 0))
    {
        return GT_BAD_PARAM;
    }

    pBoardInfo = &(boardsList[boardIdx - 1]);
    if (boardRevId > pBoardInfo->numOfRevs)
    {
        return GT_FAIL;
    }

    /* check if we run RTOS on simulation , to connect remote simulation */
#if (defined ASIC_SIMULATION) &&  (defined RTOS_ON_SIM)
    /*simulation initialization*/
    appDemoRtosOnSimulationInit();

#endif /*(defined ASIC_SIMULATION) &&  (defined RTOS_ON_SIM)*/

#ifdef ASIC_SIMULATION
    if(sasicgSimulationRole != SASICG_SIMULATION_ROLE_NON_DISTRIBUTED_E)
    {
        switch(sasicgSimulationRole)
        {
            case SASICG_SIMULATION_ROLE_DISTRIBUTED_APPLICATION_SIDE_E:
            case SASICG_SIMULATION_ROLE_DISTRIBUTED_APPLICATION_SIDE_VIA_BROKER_E:
                break;
            default:
                /*bypass the need to call from terminal*/
                appDemoOnDistributedSimAsicSide = 1;
                break;
        }

        osPrintf(" cpssInitSystem : initialize for Distributed Simulation [%s]\n",consoleDistStr[sasicgSimulationRole]);
    }
#endif /*ASIC_SIMULATION*/

    /* Enable printing inside interrupt routine. */
    extDrvUartInit();

    /* Call to fatal_error initialization, use default fatal error call_back */
    rc = osFatalErrorInit((FATAL_FUNC_PTR)NULL);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************************/
    /* global initialization phase must be done before phase 1 and 2 */
    /*****************************************************************/

    /* Make sure all pointers are NULL in case user won't init all pointers */
    osMemSet(&boardCfgFuncs, 0, sizeof(GT_BOARD_CONFIG_FUNCS));

    /* Make sure all data is cleared */
    osMemSet(&appDemoSysConfig, 0, sizeof(appDemoSysConfig));

    /* ability to set different device numbers for stacking */
    if(appDemoDbEntryGet("firstDevNum", &value) == GT_OK)
    {
        appDemoSysConfig.firstDevNum = (GT_U8)value;
    }
    else
    {
        appDemoSysConfig.firstDevNum = 0;
    }

    /* Check if board registerFunc is not NULL */
    if (NULL == pBoardInfo->registerFunc)
    {
        return GT_NOT_SUPPORTED;
    }

    /* Get board specific functions accordingly to board rev. id */
    rc = pBoardInfo->registerFunc((GT_U8)boardRevId, &boardCfgFuncs);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("pBoardInfo->registerFunc", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*return GT_OK;*/
	
    /* At first get memory size for the board           */
    /* It should be done before any memory allocation   */
    numOfMem = APP_DEMO_OSMEM_DEFAULT_MEM_INIT_SIZE;

    /* If board has own function for memory size then use it */
    if (boardCfgFuncs.boardGetMemInfo != NULL)
    {
        rc = boardCfgFuncs.boardGetMemInfo((GT_U8)boardRevId, &numOfMem);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs.boardGetMemInfo", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* Do global initialization for AppDemo before all phases */
    rc = appDemoInitGlobalModuls(numOfMem, reloadEeprom);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoInitGlobalModuls", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Get board information */
    rc = boardCfgFuncs.boardGetInfo((GT_U8)boardRevId,
                                    &numOfPp,
                                    &numOfFa,
                                    &numOfXbar,
                                    &gIsB2bSystem);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs.boardGetInfo", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* for fasten the "all PPs loops */
    appDemoPpConfigDevAmount = numOfPp;

    /* Does board specific settings before phase 1 */
    if (boardCfgFuncs.boardBeforePhase1Config != NULL)
    {
        rc = boardCfgFuncs.boardBeforePhase1Config((GT_U8)boardRevId);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs.boardBeforePhase1Config", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /*****************************************************************/
    /* HW phase 1 initialization                                     */
    /*****************************************************************/
    rc = appDemoBoardPhase1Init((GT_U8)boardRevId,
                                &boardCfgFuncs,
                                numOfPp,
                                numOfFa,
                                numOfXbar);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoBoardPhase1Init", rc);
    if (rc != GT_OK)
    {
        return rc;
    }


    if((appDemoOnDistributedSimAsicSide == 0) && (appDemoPrePhase1Init == GT_FALSE))
    {
        /* Does board specific settings after phase 1 */
        if (boardCfgFuncs.boardAfterPhase1Config != NULL)
        {
            CPSS_ENABLER_DBG_TRACE_RC_MAC("boardAfterPhase1Config", boardRevId);
            rc = boardCfgFuncs.boardAfterPhase1Config((GT_U8)boardRevId);
            printf("%s %s %d, stdin=%d stdout=%d\n", __FILE__, __FUNCTION__, __LINE__, stdin, stdout);/*xcat debug*/
            CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs.boardAfterPhase1Config", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    /*****************************************************************/
    /* HW phase 2 initialization                                     */
    /*****************************************************************/
    rc = appDemoBoardPhase2Init((GT_U8)boardRevId,
                                &boardCfgFuncs,
                                numOfPp,
                                numOfFa,
                                numOfXbar);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoBoardPhase2Init", rc);
    if (rc != GT_OK)
    {
        return rc;
    }
    if((appDemoOnDistributedSimAsicSide == 0) && (appDemoPrePhase1Init == GT_FALSE))
    {
        /* Does board specific settings after phase 2 */
        if (boardCfgFuncs.boardAfterPhase2Config != NULL)
        {
            rc = boardCfgFuncs.boardAfterPhase2Config((GT_U8)boardRevId);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs.boardAfterPhase2Config", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    /*****************************************************************/
    /* Logic phase initialization                                    */
    /*****************************************************************/
    rc = appDemoBoardLogicalInit((GT_U8)boardRevId,
                                 &boardCfgFuncs,
                                 numOfPp,
                                 numOfFa,
                                 numOfXbar);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoBoardLogicalInit", rc);
    if (rc != GT_OK)
    {
        return rc;
    }
    if (appDemoPrePhase1Init == GT_TRUE)
    {
        /* for this special mode only events handler should be invoked */
        /* spawn the user event handler processes */
        rc = appDemoEventRequestDrvnModeInit();
        CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoEventRequestDrvnModeInit", rc);
        return rc;

    }

    /*****************************************************************/
    /* General phase initialization                                  */
    /*****************************************************************/
    rc = appDemoBoardGeneralInit((GT_U8)boardRevId,
                                 &boardCfgFuncs,
                                 numOfPp,
                                 numOfFa,
                                 numOfXbar);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoBoardGeneralInit", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

#ifdef  INCLUDE_UTF
    {
        char mtxName[20];
        osStrCpy(mtxName, "fdbUpdate");

        rc = cpssOsMutexCreate(mtxName, &appDemoFdbUpdateLockMtx);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
#endif

    if(appDemoOnDistributedSimAsicSide == 0)
    {
        /*****************************************************************/
        /* Perform board specific settings after logical phase           */
        /*****************************************************************/
        if (boardCfgFuncs.boardAfterInitConfig != NULL)
        {
            rc = boardCfgFuncs.boardAfterInitConfig((GT_U8)boardRevId);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs.boardAfterInitConfig", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        if ((numOfPp + numOfFa + numOfXbar) != 0       &&
            (appDemoBypassEventInitialize == GT_FALSE) &&
            (hsuInfo.hsuState == CPSS_HSU_STATE_COMPLETED_E))
        {
    		printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);/*xcat debug*/	
            printf("\n appDemoEventRequestDrvnModeInit() start !\n");
			
            /* spawn the user event handler processes */
            rc = appDemoEventRequestDrvnModeInit();
            CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoEventRequestDrvnModeInit", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

	printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);	
    printf("\n******************************************************\n");
    printf("******** Call intScanUseTask() ************************\n");
    printf("*******************************************************\n");
	rc = intScanUseTask(numOfPp);
    if (rc != GT_OK)
    {	
    	printf("cpssInitSystem::intScanUseTask rc %d\n",rc);
        return rc;
    }

    /* Here is the RX polling zhangdi */
#ifndef CPSS_PACKET_RX_INT
	/* spawn the SDMA Rx Polling processes */
	printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);	
    printf("\n******************************************************\n");
    printf("******** Call appDemoBoardSdmaRxPollingInit() *********\n");
    printf("*******************************************************\n");
	rc = appDemoBoardSdmaRxPollingInit(boardRevId,numOfPp,numOfFa,numOfXbar);
    if (rc != GT_OK)
    {	
    	printf("cpssInitSystem::appDemoBoardSdmaRxPollingInit rc %d\n",rc);
        return rc;
    }
	printf("cpssInitSystem::appDemoBoardSdmaRxPollingInit ok\r\n");

#endif


    systemInitialized = GT_TRUE;


/****************************************
* Run fdbLearning abd RxTx processes
* for multi process appDemo
*
*  multiProcessAppDemo set by cpssInitSystem
*  if argv[2] == 2
****************************************/
#ifdef SHARED_MEMORY

 if (multiProcessAppDemo == 1)
 {

        /*****************************************/
        /*   Run fdbLearning and RxTx processes  */
        /****************************************/
        pid_t fdbLearningProcessId;
        pid_t RxTxProcessId;


        fdbLearningProcessId = fork();

        if(fdbLearningProcessId < 0)
        {
                printf("Run fdbLearningProcessId failed\n");
                return GT_FAIL;
        }

        if(fdbLearningProcessId == 0)
        {
                execl("/usr/bin/fdbLearning", "fdbLearning",NULL);
                exit (1);
        }


        RxTxProcessId = fork();

        if(RxTxProcessId < 0)
        {
                printf("Run fdbLearningProcessId failed\n");
                return GT_FAIL;;
        }

        if(RxTxProcessId == 0)
        {
                execl("/usr/bin/RxTxProcess", "RxTxProcess",NULL);
                exit (1);
        }

#ifdef APP_DEMO_CPU_ETH_PORT_MANAGE
        /* Shared Memory implementation should be updated to use native MII APIs. */
        CPSS_TBD_BOOKMARK

        /* set eth port parameters for RxTxProcess */
        ethPortParams.valid = GT_TRUE;
        ethPortParams.appDemoSysConfig = appDemoSysConfig;
        rc = cpssMultiProcComSetSmiTransmitParams(&ethPortParams);

#endif

}
#endif


    /*****************************************************************/
    /* External modules initializations                              */
    /*****************************************************************/

#ifdef INCLUDE_UTF
    /* Initialize unit tests for CPSS */
    rc = utfPreInitPhase();
    if (rc != GT_OK)
    {
        utfPostInitPhase(rc);
        return rc;
    }

    rc = utfInit(SYSTEM_DEV_NUM_MAC(0));
    if (rc != GT_OK)
    {
        utfPostInitPhase(rc);
        return rc;
    }

#ifdef IMPL_DUNE
    /* Initialize unit tests for DUNE */
    rc = duneUtfPreInitPhase();
    if (rc == GT_OK)
    {
        rc = duneUtfInit();
        if (rc != GT_OK)
        {
            utfPostInitPhase(rc);
            return rc;
        }
    }
    else
    {
        /* there are no FX devices was found.
           it's legal for using FX image for boards without FX devices. */
        rc = GT_OK;
    }

#endif /* IMPL_DUNE */

    utfPostInitPhase(rc);

#endif /* INCLUDE_UTF */

	for(devNum = 0;devNum < appDemoPpConfigDevAmount;devNum++)
	{
		asicType[devNum] = PRV_CPSS_PP_MAC(devNum)->devFamily;
	}
    return rc;
} /* cpssInitSystem */


/*******************************************************************************
 * Private functions implementation
 ******************************************************************************/

/*******************************************************************************
* appDemoInitGlobalModuls
*
* DESCRIPTION: Initialize global settings for CPSS
*
* INPUTS:
*       numOfMemBytes   -   memory size for the board
*       reloadEeprom    -   reload eeprom flag
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS appDemoInitGlobalModuls
(
    IN  GT_U32  numOfMemBytes,
    IN  GT_U32  reloadEeprom
)
{
    GT_STATUS       rc;             /* To hold funcion return code  */
    GT_U32          i;              /* Loop index                   */


    /* Initialize memory pool. It must be done before any memory allocations */
    rc = osMemInit(numOfMemBytes, GT_TRUE);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* save reload eeprom for future use */
    gReloadEeprom = (reloadEeprom == 1 ? GT_TRUE : GT_FALSE);

    /* Initialize the PP array with default parameters */
    for (i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        osMemSet(&appDemoPpConfigList[i], 0, sizeof(appDemoPpConfigList[i]));

        appDemoPpConfigList[i].ppPhase1Done = GT_FALSE;
        appDemoPpConfigList[i].ppPhase2Done = GT_FALSE;
        appDemoPpConfigList[i].ppLogicalInitDone = GT_FALSE;
        appDemoPpConfigList[i].ppGeneralInitDone = GT_FALSE;
        appDemoPpConfigList[i].valid = GT_FALSE;

        /* default value for cpu tx/rx mode */
        appDemoPpConfigList[i].cpuPortMode = PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_SDMA_E;

        /* cascading information */
        appDemoPpConfigList[i].numberOfCscdTrunks = 0;
        appDemoPpConfigList[i].numberOfCscdPorts = 0;
        appDemoPpConfigList[i].numberOfCscdTargetDevs = 0;
        appDemoPpConfigList[i].numberOf10GPortsToConfigure = 0;
        appDemoPpConfigList[i].internal10GPortConfigFuncPtr = NULL;
        appDemoPpConfigList[i].internalCscdPortConfigFuncPtr = NULL;
    }
    return GT_OK;
} /* appDemoInitGlobalModuls */


/*******************************************************************************
* cpssGetDefaultOsBindFuncs
*
* DESCRIPTION:
*       Receives default cpss bind from OS
* INPUTS:
*       None.
* OUTPUTS:
*       osFuncBindPtr - (pointer to) set of call back functions
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       Function should reside into CPSS library to resolve correct
*       pointers to functions.
*
*******************************************************************************/
/* if not used shared memory or it's unix-like simulation do compile this */
#if (!defined(SHARED_MEMORY) || defined(LINUX_SIM))

static GT_STATUS cpssGetDefaultOsBindFuncs(
    OUT CPSS_OS_FUNC_BIND_STC *osFuncBindPtr
)
{

    osMemSet(osFuncBindPtr,0,sizeof(*osFuncBindPtr));

    /* bind the OS functions to the CPSS */
    osFuncBindPtr->osMemBindInfo.osMemBzeroFunc =             osBzero;
    osFuncBindPtr->osMemBindInfo.osMemSetFunc   =             osMemSet;
    osFuncBindPtr->osMemBindInfo.osMemCpyFunc   =             osMemCpy;
    osFuncBindPtr->osMemBindInfo.osMemCmpFunc   =             osMemCmp;
    osFuncBindPtr->osMemBindInfo.osMemStaticMallocFunc =      osStaticMalloc;
    osFuncBindPtr->osMemBindInfo.osMemMallocFunc =            osMalloc;
    osFuncBindPtr->osMemBindInfo.osMemReallocFunc =           osRealloc;
    osFuncBindPtr->osMemBindInfo.osMemFreeFunc   =            osFree;
    osFuncBindPtr->osMemBindInfo.osMemCacheDmaMallocFunc =    osCacheDmaMalloc;
    osFuncBindPtr->osMemBindInfo.osMemCacheDmaFreeFunc =      osCacheDmaFree;
    osFuncBindPtr->osMemBindInfo.osMemPhyToVirtFunc =         osPhy2Virt;
    osFuncBindPtr->osMemBindInfo.osMemVirtToPhyFunc =         osVirt2Phy;

    osFuncBindPtr->osStrBindInfo.osStrlenFunc       = osStrlen;
    osFuncBindPtr->osStrBindInfo.osStrCpyFunc       = osStrCpy;
#ifdef  LINUX
    osFuncBindPtr->osStrBindInfo.osStrNCpyFunc      = osStrNCpy;
#endif
    osFuncBindPtr->osStrBindInfo.osStrChrFunc       = osStrChr;
    osFuncBindPtr->osStrBindInfo.osStrCmpFunc       = osStrCmp;
    osFuncBindPtr->osStrBindInfo.osStrNCmpFunc      = osStrNCmp;
    osFuncBindPtr->osStrBindInfo.osStrCatFunc       = osStrCat;
    osFuncBindPtr->osStrBindInfo.osStrStrNCatFunc   = osStrNCat;
    osFuncBindPtr->osStrBindInfo.osStrChrToUpperFunc= osToUpper;
    osFuncBindPtr->osStrBindInfo.osStrTo32Func      = osStrTo32;
    osFuncBindPtr->osStrBindInfo.osStrToU32Func     = osStrToU32;

    osFuncBindPtr->osSemBindInfo.osMutexCreateFunc     = osMutexCreate;
    osFuncBindPtr->osSemBindInfo.osMutexDeleteFunc     = osMutexDelete;
    osFuncBindPtr->osSemBindInfo.osMutexLockFunc       = osMutexLock;
    osFuncBindPtr->osSemBindInfo.osMutexUnlockFunc     = osMutexUnlock;

    osFuncBindPtr->osSemBindInfo.osSigSemBinCreateFunc = FORCE_FUNC_CAST osSemBinCreate;
#ifdef  LINUX
    osFuncBindPtr->osSemBindInfo.osSigSemMCreateFunc   = osSemMCreate;
    osFuncBindPtr->osSemBindInfo.osSigSemCCreateFunc   = osSemCCreate;
#endif
    osFuncBindPtr->osSemBindInfo.osSigSemDeleteFunc    = osSemDelete;
    osFuncBindPtr->osSemBindInfo.osSigSemWaitFunc      = osSemWait;
    osFuncBindPtr->osSemBindInfo.osSigSemSignalFunc    = osSemSignal;

    osFuncBindPtr->osIoBindInfo.osIoBindStdOutFunc  = osBindStdOut;
    osFuncBindPtr->osIoBindInfo.osIoPrintfFunc      = osPrintf;
#ifdef  LINUX
    osFuncBindPtr->osIoBindInfo.osIoVprintfFunc     = osVprintf;
#endif
    osFuncBindPtr->osIoBindInfo.osIoSprintfFunc     = osSprintf;
#ifdef  LINUX
    osFuncBindPtr->osIoBindInfo.osIoVsprintfFunc    = osVsprintf;
#endif
    osFuncBindPtr->osIoBindInfo.osIoPrintSynchFunc  = osPrintSync;
    osFuncBindPtr->osIoBindInfo.osIoGetsFunc        = osGets;

    osFuncBindPtr->osInetBindInfo.osInetNtohlFunc = osNtohl;
    osFuncBindPtr->osInetBindInfo.osInetHtonlFunc = osHtonl;
    osFuncBindPtr->osInetBindInfo.osInetNtohsFunc = osNtohs;
    osFuncBindPtr->osInetBindInfo.osInetHtonsFunc = osHtons;
    osFuncBindPtr->osInetBindInfo.osInetNtoaFunc  = osInetNtoa;

    osFuncBindPtr->osTimeBindInfo.osTimeWkAfterFunc = osTimerWkAfter;
    osFuncBindPtr->osTimeBindInfo.osTimeTickGetFunc = osTickGet;
    osFuncBindPtr->osTimeBindInfo.osTimeGetFunc     = osTime;
    osFuncBindPtr->osTimeBindInfo.osTimeRTFunc      = osTimeRT;
#ifdef  LINUX
    osFuncBindPtr->osTimeBindInfo.osGetSysClockRateFunc = osGetSysClockRate;
    osFuncBindPtr->osTimeBindInfo.osDelayFunc       = osDelay;
#endif

#if !defined(UNIX) || defined(ASIC_SIMULATION)
    osFuncBindPtr->osIntBindInfo.osIntEnableFunc   = osIntEnable;
    osFuncBindPtr->osIntBindInfo.osIntDisableFunc  = osIntDisable;
    osFuncBindPtr->osIntBindInfo.osIntConnectFunc  = osInterruptConnect;
#endif
#if (!defined(FREEBSD) && !defined(UCLINUX)) || defined(ASIC_SIMULATION)
    /* this function required for sand_os_mainOs_interface.c
     * Now it is implemented for:
     *   all os with ASIC simulation
     *   vxWorks
     *   Win32
     *   Linux (stub which does nothing)
     */
    osFuncBindPtr->osIntBindInfo.osIntModeSetFunc  = FORCE_FUNC_CAST osSetIntLockUnlock;
#endif
    osFuncBindPtr->osRandBindInfo.osRandFunc  = osRand;
    osFuncBindPtr->osRandBindInfo.osSrandFunc = osSrand;

    osFuncBindPtr->osTaskBindInfo.osTaskCreateFunc = osTaskCreate;
    osFuncBindPtr->osTaskBindInfo.osTaskDeleteFunc = osTaskDelete;
    /*osFuncBindPtr->osTaskBindInfo.osTaskGetSelfFunc= osTaskGetSelf;*/
    osFuncBindPtr->osTaskBindInfo.osTaskLockFunc   = osTaskLock;
    osFuncBindPtr->osTaskBindInfo.osTaskUnLockFunc = osTaskUnLock;

#ifdef  LINUX
    osFuncBindPtr->osStdLibBindInfo.osQsortFunc    = osQsort;
    osFuncBindPtr->osStdLibBindInfo.osBsearchFunc  = osBsearch;
#endif

    osFuncBindPtr->osMsgQBindInfo.osMsgQCreateFunc  = osMsgQCreate;
    osFuncBindPtr->osMsgQBindInfo.osMsgQDeleteFunc  = osMsgQDelete;
    osFuncBindPtr->osMsgQBindInfo.osMsgQSendFunc    = osMsgQSend;
    osFuncBindPtr->osMsgQBindInfo.osMsgQRecvFunc    = osMsgQRecv;
    osFuncBindPtr->osMsgQBindInfo.osMsgQNumMsgsFunc = osMsgQNumMsgs;


    /* Now we should be sure that ALL pointers are set.
     * Occasionally you can add new field into the structure and forget
     * to initialize it!
     *
     * Warning! Here we suppose that compiler doesn't realign this structure!!!
     * Thus we can scan all words inside structure as pointers, there are no
     * alignment bytes there! */

    /* Wrong assumption !!! Compiler CAN realign structure!
    for(i=0; i<sizeof(*osFuncBindPtr); i+=4)
    {
        void **func_ptr=((unsigned char*)osFuncBindPtr) + i;
        if(*func_ptr == NULL)
        {
            osPrintf("Error: pointer into CPSS_OS_FUNC_BIND_STC struct is NULL (offset %d), sizeof(CPSS_OS_FUNC_BIND_STC)=%d\n",
                     i, sizeof(CPSS_OS_FUNC_BIND_STC));
            return GT_FAIL;
        }
    }
    */
    return GT_OK;
}

/*******************************************************************************
* cpssGetDefaultExtDrvFuncs
*
* DESCRIPTION:
*       Receives default cpss bind from extDrv
* INPUTS:
*       None.
* OUTPUTS:
*       extDrvFuncBindInfoPtr - (pointer to) set of call back functions
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       Function should reside into CPSS library to resolve correct
*       pointers to functions.
*
*******************************************************************************/
static GT_STATUS cpssGetDefaultExtDrvFuncs(
    OUT CPSS_EXT_DRV_FUNC_BIND_STC  *extDrvFuncBindInfoPtr
)
{
    osMemSet(extDrvFuncBindInfoPtr,0,sizeof(*extDrvFuncBindInfoPtr));

    /* bind the external drivers functions to the CPSS */
    extDrvFuncBindInfoPtr->extDrvMgmtCacheBindInfo.extDrvMgmtCacheFlush      = FORCE_FUNC_CAST extDrvMgmtCacheFlush;
    extDrvFuncBindInfoPtr->extDrvMgmtCacheBindInfo.extDrvMgmtCacheInvalidate = FORCE_FUNC_CAST extDrvMgmtCacheInvalidate;

    extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiInitDriverFunc      = hwIfSmiInitDriver;
    extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiWriteRegFunc        = hwIfSmiWriteReg;
    extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiReadRegFunc         = hwIfSmiReadReg;
    extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskRegRamReadFunc  = hwIfSmiTskRegRamRead;
    extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskRegRamWriteFunc = hwIfSmiTskRegRamWrite;
    extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskRegVecReadFunc  = hwIfSmiTskRegVecRead;
    extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskRegVecWriteFunc = hwIfSmiTskRegVecWrite;
    extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskWriteRegFunc    = hwIfSmiTaskWriteReg;
    extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskReadRegFunc     = hwIfSmiTaskReadReg;
    extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiIntReadRegFunc      = hwIfSmiInterruptReadReg;
    extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiIntWriteRegFunc     = hwIfSmiInterruptWriteReg;
    extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiDevVendorIdGetFunc  = extDrvSmiDevVendorIdGet;
/*  used only in linux -- will need to be under some kind of COMPILATION FLAG
    extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskWriteFieldFunc  = hwIfSmiTaskWriteRegField;
*/

#ifdef GT_I2C
    extDrvFuncBindInfoPtr->extDrvHwIfTwsiBindInfo.extDrvHwIfTwsiInitDriverFunc = hwIfTwsiInitDriver;
    extDrvFuncBindInfoPtr->extDrvHwIfTwsiBindInfo.extDrvHwIfTwsiWriteRegFunc   = hwIfTwsiWriteReg;
    extDrvFuncBindInfoPtr->extDrvHwIfTwsiBindInfo.extDrvHwIfTwsiReadRegFunc    = hwIfTwsiReadReg;
#endif /* GT_I2C */


/*  XBAR related services */
#if defined(IMPL_FA) || defined(IMPL_XBAR)
    extDrvFuncBindInfoPtr->extDrvMgmtHwIfBindInfo.extDrvI2cMgmtMasterInitFunc    = gtI2cMgmtMasterInit;
    extDrvFuncBindInfoPtr->extDrvMgmtHwIfBindInfo.extDrvMgmtReadRegisterFunc     = FORCE_FUNC_CAST extDrvMgmtReadRegister;
    extDrvFuncBindInfoPtr->extDrvMgmtHwIfBindInfo.extDrvMgmtWriteRegisterFunc    = FORCE_FUNC_CAST extDrvMgmtWriteRegister;
    extDrvFuncBindInfoPtr->extDrvMgmtHwIfBindInfo.extDrvMgmtIsrReadRegisterFunc  = FORCE_FUNC_CAST extDrvMgmtIsrReadRegister;
    extDrvFuncBindInfoPtr->extDrvMgmtHwIfBindInfo.extDrvMgmtIsrWriteRegisterFunc = FORCE_FUNC_CAST extDrvMgmtIsrWriteRegister;
#endif

    extDrvFuncBindInfoPtr->extDrvDmaBindInfo.extDrvDmaWriteDriverFunc = extDrvDmaWrite;
    extDrvFuncBindInfoPtr->extDrvDmaBindInfo.extDrvDmaReadFunc        = extDrvDmaRead;

    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPortRxInitFunc            = extDrvEthPortRxInit;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPortTxInitFunc            = extDrvEthPortTxInit;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPortEnableFunc            = extDrvEthPortEnable;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPortDisableFunc           = extDrvEthPortDisable;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPortTxFunc                = extDrvEthPortTx;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPortInputHookAddFunc      = extDrvEthInputHookAdd;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPortTxCompleteHookAddFunc = extDrvEthTxCompleteHookAdd;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPortRxPacketFreeFunc      = extDrvEthRxPacketFree;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPortTxModeSetFunc         = FORCE_FUNC_CAST extDrvEthPortTxModeSet;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthRawSocketModeSetFunc  = extDrvEthRawSocketModeSet;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthRawSocketModeGetFunc  = extDrvEthRawSocketModeGet;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvLinuxModeSetFunc  = extDrvLinuxModeSet;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvLinuxModeGetFunc  = extDrvLinuxModeGet;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthRawSocketRxHookAddFunc = extDrvEthRawSocketRxHookAdd;

    extDrvFuncBindInfoPtr->extDrvHsuDrvBindInfo.extDrvHsuMemBaseAddrGetFunc = extDrvHsuMemBaseAddrGet;
    extDrvFuncBindInfoPtr->extDrvHsuDrvBindInfo.extDrvHsuWarmRestartFunc = extDrvHsuWarmRestart;

#if defined (XCAT_DRV)
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthCpuCodeToQueueFunc        = extDrvEthCpuCodeToQueue;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPrePendTwoBytesHeaderSetFunc = extDrvEthPrePendTwoBytesHeaderSet;
#endif

    extDrvFuncBindInfoPtr->extDrvIntBindInfo.extDrvIntConnectFunc     = extDrvIntConnect;
    extDrvFuncBindInfoPtr->extDrvIntBindInfo.extDrvIntEnableFunc      = extDrvIntEnable;
    extDrvFuncBindInfoPtr->extDrvIntBindInfo.extDrvIntDisableFunc     = extDrvIntDisable;
    extDrvFuncBindInfoPtr->extDrvIntBindInfo.extDrvIntLockModeSetFunc = FORCE_FUNC_CAST extDrvSetIntLockUnlock;

    extDrvFuncBindInfoPtr->extDrvPciInfo.extDrvPciConfigWriteRegFunc        = extDrvPciConfigWriteReg;
    extDrvFuncBindInfoPtr->extDrvPciInfo.extDrvPciConfigReadRegFunc         = extDrvPciConfigReadReg;
    extDrvFuncBindInfoPtr->extDrvPciInfo.extDrvPciDevFindFunc               = extDrvPciFindDev;
    extDrvFuncBindInfoPtr->extDrvPciInfo.extDrvPciIntVecFunc                = FORCE_FUNC_CAST extDrvGetPciIntVec;
    extDrvFuncBindInfoPtr->extDrvPciInfo.extDrvPciIntMaskFunc               = FORCE_FUNC_CAST extDrvGetIntMask;
    extDrvFuncBindInfoPtr->extDrvPciInfo.extDrvPciCombinedAccessEnableFunc  = extDrvEnableCombinedPciAccess;
    extDrvFuncBindInfoPtr->extDrvPciInfo.extDrvPciDoubleWriteFunc           = extDrvPciDoubleWrite;
    extDrvFuncBindInfoPtr->extDrvPciInfo.extDrvPciDoubleReadFunc            = extDrvPciDoubleRead;

#ifdef DRAGONITE_TYPE_A1
    extDrvFuncBindInfoPtr->extDrvDragoniteInfo.extDrvDragoniteShMemBaseAddrGetFunc = extDrvDragoniteShMemBaseAddrGet;
#endif

    return GT_OK;
}

#endif
/*******************************************************************************
* appDemoCpssInit
*
* DESCRIPTION:
*       This function call CPSS to do initial initialization.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*       This function must be called before phase 1 init.
*
*******************************************************************************/
static GT_STATUS appDemoCpssInit
(
    GT_VOID
)
{
    GT_STATUS                   rc;
    CPSS_EXT_DRV_FUNC_BIND_STC  extDrvFuncBindInfo;
    CPSS_OS_FUNC_BIND_STC       osFuncBind;

    if (appDemoCpssInitialized == GT_TRUE)
    {
        return GT_OK;
    }

#if defined(SHARED_MEMORY) && (!defined(LINUX_SIM))
    /* Shared memory approach on ARM should use dlsym-based workaround for pointers to functions */
    rc = shrMemGetDefaultOsBindFuncsThruDynamicLoader(&osFuncBind);
#else
    rc = cpssGetDefaultOsBindFuncs(&osFuncBind);
#endif

    if(rc != GT_OK) return rc;

#if defined(SHARED_MEMORY) && (!defined(LINUX_SIM))
    /* Shared memory approach on ARM should use dlsym-based workaround for pointers to functions */
    rc = shrMemGetDefaultExtDrvFuncs(&extDrvFuncBindInfo);
#else
    rc = cpssGetDefaultExtDrvFuncs(&extDrvFuncBindInfo);
#endif

    if(rc != GT_OK) return rc;

    /* 1'st step */
    rc = cpssExtServicesBind(&extDrvFuncBindInfo, &osFuncBind);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* 2'nd step */
    rc = cpssPpInit();
    if(rc != GT_OK)
    {
        return rc;
    }
    appDemoCpssInitialized = GT_TRUE;
    return GT_OK;
} /* appDemoCpssInit */



/*******************************************************************************
* appDemoBoardPhase1Init
*
* DESCRIPTION:
*       Perform phase1 initialization for all devices (Pp, Fa, Xbar).
*
* INPUTS:
*       boardRevId      - Board revision Id.
*       boardCfgFuncs   - Board configuration functions.
*       numOfPp         - Number of Pp's in system.
*       numOfFa         - Number of Fa's in system.
*       numOfXbar       - Number of Xbar's in system.
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
static GT_STATUS appDemoBoardPhase1Init
(
    IN  GT_U8                   boardRevId,       /* Revision of this board             */
    IN  GT_BOARD_CONFIG_FUNCS   *boardCfgFuncs,   /* Config functions for this board    */
    IN  GT_U8                   numOfDev,         /* Number of devices in this board    */
    IN  GT_U8                   numOfFa,          /* Number of FAs in this board        */
    IN  GT_U8                   numOfXbar         /* Number of xbars in this board      */
)
{
    CPSS_PP_PHASE1_INIT_PARAMS      corePpPhase1Params;     /* Phase 1 PP params                            */
    CPSS_REG_VALUE_INFO_STC         *regCfgList;            /* Config functions for this board              */
    GT_U32                          regCfgListSize;         /* Number of config functions for this board    */
    GT_STATUS                       rc;                     /* Func. return val.                            */
    GT_U8                           devIdx;                 /* Loop index.                                  */
    CPSS_PP_DEVICE_TYPE             ppDevType;              /* CPSS Pp device type.                         */

#ifdef IMPL_XBARDRIVER
    GT_XBAR_CORE_SYS_CFG            xbarSysCfg;
    GT_XBAR_DEVICE                  xbarDevType;            /* Xbar device type.*/
    GT_XBAR_PHASE1_INIT_PARAMS      coreXbarPhase1Params;
#endif

#ifdef IMPL_FA
    GT_U8                           deviceNumber;           /* Real device number: FA or PP                 */
    GT_FA_PHASE1_INIT_PARAMS        coreFaPhase1Params;
    GT_FA_DEVICE                    faDevType;              /* Fa device type.  */
#endif


    /* Set xbar configuration parameters.           */
#ifdef IMPL_XBARDRIVER
    if((boardCfgFuncs->boardGetXbarCfgParams != NULL) &&
       (numOfXbar != 0))
    {
        rc = boardCfgFuncs->boardGetXbarCfgParams((GT_U8)boardRevId, &xbarSysCfg);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs->boardGetXbarCfgParams", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* CPSS works only in event driven mode */
        xbarSysCfg.eventRequestDriven = GT_TRUE;

        /* Initialize xbar core & driver dbases */
        rc = xbarSysConfig(&xbarSysCfg);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("xbarSysConfig", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
#endif

#ifdef IMPL_PP
    /* Set Pp Phase1 configuration parameters.      */
    for (devIdx = SYSTEM_DEV_NUM_MAC(0); devIdx < SYSTEM_DEV_NUM_MAC(numOfDev); devIdx++)
    {
        APP_DEMO_SYS_CONFIG_FUNCS*  sysCfgFuncs;

        /* Get PP config of this device */
        rc = boardCfgFuncs->boardGetPpPh1Params((GT_U8)boardRevId,
                                                devIdx,
                                                &corePpPhase1Params);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs->boardGetPpPh1Params", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* override PP phase 1 parameters according to app demo database */
        rc = appDemoUpdatePpPhase1Params(&corePpPhase1Params);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoUpdatePpPhase1Params", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* Update PP config list device number */
        appDemoPpConfigList[devIdx].devNum   = corePpPhase1Params.devNum;
        appDemoPpConfigList[devIdx].valid    = GT_TRUE;

        /* get family type to understand which API should be used below */
        /* There are two type of API here: EX and DX                    */
        rc = appDemoSysConfigFuncsGet(corePpPhase1Params.deviceId,
                                      &appDemoPpConfigList[devIdx].sysConfigFuncs,
                                      &appDemoPpConfigList[devIdx].apiSupportedBmp);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoSysConfigFuncsGet", rc);
        if (GT_OK != rc)
        {
            return rc;
        }


        if(appDemoSysConfigFuncsExtentionGetPtr)
        {
            rc = appDemoSysConfigFuncsExtentionGetPtr(corePpPhase1Params.deviceId,
                                      &appDemoPpConfigList[devIdx].sysConfigFuncs,
                                      &appDemoPpConfigList[devIdx].apiSupportedBmp);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoSysConfigFuncsExtentionGetPtr", rc);
            if (GT_OK != rc)
            {
                return rc;
            }
        }

        sysCfgFuncs = &appDemoPpConfigList[devIdx].sysConfigFuncs;

#ifdef ASIC_SIMULATION
        if(sasicgSimulationRole == SASICG_SIMULATION_ROLE_DISTRIBUTED_APPLICATION_SIDE_VIA_BROKER_E)
        {
            /* bind interrupts when work via broker */
            simDistributedRegisterInterrupt(corePpPhase1Params.intVecNum,
                    corePpPhase1Params.devNum,/* use the device ID as the signal ID */
                    sdistAppViaBrokerInterruptMaskMode);
        }
#endif /*ASIC_SIMULATION*/

        /* check if debug device id was set */
        if (useDebugDeviceId == GT_TRUE)
        {
            /* write device id to internal DB */
            rc = prvCpssDrvDebugDeviceIdSet(appDemoPpConfigList[devIdx].devNum,
                                            deviceIdDebug[appDemoPpConfigList[devIdx].devNum]);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("prvCpssDrvDebugDeviceIdSet", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        if (appDemoPrePhase1Init == GT_TRUE)
        {
            rc = prvCpssPrePhase1PpInit(PRV_CPSS_PP_PRE_PHASE1_INIT_MODE_NO_PP_INIT_E);
            if (rc != GT_OK)
            {
                return rc;
            }

        }
        /* Do HW phase 1 */
        rc = sysCfgFuncs->cpssHwPpPhase1Init(appDemoPpConfigList[devIdx].devNum,
                                             &corePpPhase1Params,
                                             &ppDevType);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("sysCfgFuncs->cpssHwPpPhase1Init", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* Update PP config list element of device */
        appDemoPpConfigList[devIdx].ppPhase1Done = GT_TRUE;
        appDemoPpConfigList[devIdx].deviceId = ppDevType;

        /* Get list of registers to be configured.  */
        if (boardCfgFuncs->boardGetPpRegCfgList == NULL)
        {
            /* if there is no board-specific function, call the common one */
            rc = appDemoGetPpRegCfgList(ppDevType,
                                        gIsB2bSystem,
                                        &regCfgList,
                                        &regCfgListSize);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoGetPpRegCfgList", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        else
        {
            rc = boardCfgFuncs->boardGetPpRegCfgList(boardRevId,
                                                     appDemoPpConfigList[devIdx].devNum,
                                                     &regCfgList,
                                                     &regCfgListSize);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs->boardGetPpRegCfgList", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        if (appDemoPrePhase1Init == GT_FALSE)
        {
            if(sysCfgFuncs->cpssHwPpStartInit)
            {
                /* Set PP's registers */
                rc = sysCfgFuncs->cpssHwPpStartInit(appDemoPpConfigList[devIdx].devNum,
                                                    gReloadEeprom,
                                                    regCfgList,
                                                    regCfgListSize);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("sysCfgFuncs->cpssHwPpStartInit", rc);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }
    }/* for (Loop on devices) */
#endif /* IMPL_PP */

    /* Set Fa Phase1 configuration parameters.      */
#ifdef IMPL_FA
    if(boardCfgFuncs->boardGetFaPh1Params != NULL)
    {
        /* Assume num of PP, num of FA and num of XBAR are the same */
        for (devIdx = SYSTEM_DEV_NUM_MAC(0); devIdx < SYSTEM_DEV_NUM_MAC(numOfFa); devIdx++)
        {
            /* Get FA config of this device */
            rc = boardCfgFuncs->boardGetFaPh1Params((GT_U8)boardRevId,
                                                    devIdx,
                                                    &coreFaPhase1Params);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs->boardGetFaPh1Params", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
            /* Phase 1 init & registration of FA */

            rc = coreFaHwPhase1Init(&coreFaPhase1Params, &faDevType);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("coreFaHwPhase1Init", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* Set Fa Start-Init configuration parameters.  */
            /* Get list of registers to be configured.  */
            if (boardCfgFuncs->boardGetFaRegCfgList == NULL)
            {
                /* if there is no board-specific function, call the common one */
                rc = appDemoGetFaRegCfgList(faDevType,
#ifdef IMPL_PP
                                            appDemoPpConfigList[devIdx].deviceId,
#else
                                            APP_DEMO_CPSS_NO_PP_CONNECTED_CNS,/*0xFFFFFFFF*/
#endif
                                            &regCfgList,
                                            &regCfgListSize);

                CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoGetFaRegCfgList", rc);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
            else
            {
                rc = boardCfgFuncs->boardGetFaRegCfgList(boardRevId,
                                                         appDemoPpConfigList[devIdx].devNum,
                                                         &regCfgList,
                                                         &regCfgListSize);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs->boardGetFaRegCfgList", rc);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
            /* if NO PP used device number is FA device number */
            deviceNumber = (numOfDev > 0) ? appDemoPpConfigList[devIdx].devNum : devIdx;


            /* Set FA's registers */
            rc = coreFaHwStartInit(deviceNumber,
                                   GT_FALSE,
                                   (GT_REG_VALUE_INFO*)regCfgList,
                                   regCfgListSize);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("coreFaHwStartInit", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }/* Loop on FAs */
    }/* FA exists */
#endif /*IMPL_FA*/

#ifdef IMPL_XBARDRIVER
    /* Set xbar core Phase1 configuration parameters.    */
    if(boardCfgFuncs->boardGetXbarPh1Params != NULL)
    {
        /* Assume num of PP, num of FA and num of XBAR are the same */
        for (devIdx = SYSTEM_DEV_NUM_MAC(0); devIdx < SYSTEM_DEV_NUM_MAC(numOfXbar); devIdx++)
        {
            rc = boardCfgFuncs->boardGetXbarPh1Params((GT_U8)boardRevId,
                                                      devIdx,
                                                      &coreXbarPhase1Params);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs->boardGetXbarPh1Params", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            rc = coreXbarHwPhase1Init(&coreXbarPhase1Params, &xbarDevType);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("coreXbarHwPhase1Init", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }/* Loop on XBARs */
    } /* Xbar exists */
#endif /*IMPL_XBARDRIVER*/

    return GT_OK;
} /* appDemoBoardPhase1Init */

/*******************************************************************************
* appDemoBoardPhase2Init
*
* DESCRIPTION:
*       Perform phase2 initialization for all devices (Pp, Fa, Xbar).
*
* INPUTS:
*       boardRevId      - Board revision Id.
*       boardCfgFuncs   - Board configuration functions
*       numOfDev        - Number of devices in devList
*       numOfFa         - Number of Fa's in system
*       numOfXbar       - Number of Xbar's in system
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
static GT_STATUS appDemoBoardPhase2Init
(
    IN  GT_U8                   boardRevId,     /* Board revision Id             */
    IN  GT_BOARD_CONFIG_FUNCS   *boardCfgFuncs, /* Board configuration functions */
    IN  GT_U8                   numOfDev,       /* Number of devices in devList  */
    IN  GT_U8                   numOfFa,        /* Number of Fa's in system      */
    IN  GT_U8                   numOfXbar       /* Number of Xbar's in system    */
)
{
    CPSS_PP_PHASE2_INIT_PARAMS          cpssPpPhase2Params;     /* PP phase 2 params                        */
    GT_STATUS                           rc;                     /* Func. return val.                        */
    GT_U8                               devIdx;                 /* Loop index.                              */
    GT_32                               intKey;                 /* Interrupt key.                           */

#ifdef IMPL_XBARDRIVER
    GT_XBAR_PHASE2_INIT_PARAMS          coreXbarPhase2Params;   /* XBAR phase 2 params */
#endif

#ifdef IMPL_FA
    GT_U8                               deviceNumber;           /* Real device number: FA or PP                 */
    GT_FA_PHASE2_INIT_PARAMS            coreFaPhase2Params;     /* FA phase 2 params */
#endif

#ifdef IMPL_PP
    /* Set Pp Phase2 configuration parameters */
    for (devIdx = SYSTEM_DEV_NUM_MAC(0); devIdx < SYSTEM_DEV_NUM_MAC(numOfDev); devIdx++)
    {
        APP_DEMO_SYS_CONFIG_FUNCS*  sysCfgFuncs;

        /* Get PP phase 2 params */
        rc = boardCfgFuncs->boardGetPpPh2Params(boardRevId,
                                                appDemoPpConfigList[devIdx].devNum,
                                                &cpssPpPhase2Params);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs->boardGetPpPh2Params", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* override PP phase 2 parameters according to app demo database */
        rc = appDemoUpdatePpPhase2Params(&cpssPpPhase2Params);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoUpdatePpPhase2Params", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        if(appDemoOnDistributedSimAsicSide)
        {
            /* no need to init the external driver */
            appDemoSysConfig.cpuEtherInfo.initFunc = NULL;
            /* we must not init the HW */
            cpssPpPhase2Params.netIfCfg.txDescBlock = NULL;
            cpssPpPhase2Params.netIfCfg.rxDescBlock = NULL;
            cpssPpPhase2Params.auqCfg.auDescBlock   = NULL;
        }

        appDemoPpConfigList[devIdx].oldDevNum = appDemoPpConfigList[devIdx].devNum;
        sysCfgFuncs = &appDemoPpConfigList[devIdx].sysConfigFuncs;

        if(sysCfgFuncs->cpssHwPpPhase2Init)
        {
            /* Lock the interrupts, this phase changes the interrupts nodes pool data */
            extDrvSetIntLockUnlock(INTR_MODE_LOCK, &intKey);

            /* PP HW phase 2 Init */
            rc = sysCfgFuncs->cpssHwPpPhase2Init(appDemoPpConfigList[devIdx].oldDevNum,
                                                 &cpssPpPhase2Params);

            CPSS_ENABLER_DBG_TRACE_RC_MAC("sysCfgFuncs->cpssHwPpPhase2Init", rc);
            /* Unlock interrupts after phase 2 init */
            extDrvSetIntLockUnlock(INTR_MODE_UNLOCK, &intKey);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        /* Current PP init completed */
        appDemoPpConfigList[devIdx].ppPhase2Done = GT_TRUE;
    }
#endif /* IMPL_PP */
    /* Set Fa Phase2 configuration parameters.      */
#ifdef IMPL_FA
    if(boardCfgFuncs->boardGetFaPh2Params != NULL)
    {
        /* Assume num of PP, num of FA and num of XBAR are the same */
        for (devIdx = SYSTEM_DEV_NUM_MAC(0); devIdx < SYSTEM_DEV_NUM_MAC(numOfFa); devIdx++)
        {
            /* if NO PP used device number is FA device number */
            deviceNumber = (numOfDev > 0) ? appDemoPpConfigList[devIdx].devNum : devIdx;

            rc = boardCfgFuncs->boardGetFaPh2Params((GT_U8)boardRevId,
                                                    deviceNumber,
                                                    &coreFaPhase2Params);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs->boardGetFaPh2Params", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            coreFaPhase2Params.devNum = deviceNumber;

            /* Init FA */
            rc = coreFaHwPhase2Init(&coreFaPhase2Params);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("coreFaHwPhase2Init", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
#endif /*IMPL_FA*/

    /* Set xbar Phase2 configuration parameters.    */
#ifdef IMPL_XBARDRIVER
    if(boardCfgFuncs->boardGetXbarPh2Params != NULL)
    {
        /* Assume num of PP, num of FA and num of XBAR are the same */
        for (devIdx = SYSTEM_DEV_NUM_MAC(0); devIdx < SYSTEM_DEV_NUM_MAC(numOfXbar); devIdx++)
        {
            /* Get XBAR phase 2 params */
            rc = boardCfgFuncs->boardGetXbarPh2Params((GT_U8)boardRevId,
                                                      appDemoPpConfigList[devIdx].devNum,
                                                      &coreXbarPhase2Params);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs->boardGetXbarPh2Params", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            coreXbarPhase2Params.devNum = appDemoPpConfigList[devIdx].devNum;

            /* Init Xbar */
            rc = coreXbarHwPhase2Init(&coreXbarPhase2Params);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("coreXbarHwPhase2Init", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        } /* Loop on XBARs */
    } /* Xbar exists */
#endif /*IMPL_XBARDRIVER*/

    return GT_OK;
} /* appDemoBoardPhase2Init */

/*******************************************************************************
* appDemoBoardLogicalInit
*
* DESCRIPTION:
*       Perform logical phase initialization for all devices (Pp, Fa, Xbar).
*
* INPUTS:
*       boardRevId      - Board revision Id.
*       boardCfgFuncs   - Board configuration functions
*       numOfDev        - Number of devices in devList
*       numOfFa         - Number of Fa's in system
*       numOfXbar       - Number of Xbar's in system
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
static GT_STATUS appDemoBoardLogicalInit
(
    IN  GT_U8                   boardRevId,
    IN  GT_BOARD_CONFIG_FUNCS   *boardCfgFuncs,
    IN  GT_U8                   numOfDev,
    IN  GT_U8                   numOfFa,
    IN  GT_U8                   numOfXbar
)
{
    GT_STATUS                   rc;
    CPSS_PP_CONFIG_INIT_STC     ppLogicalConfigParams;
    GT_U8                       devIdx;


    /* Set Pp Logical configuration parameters */
    for (devIdx = SYSTEM_DEV_NUM_MAC(0); devIdx < SYSTEM_DEV_NUM_MAC(numOfDev); devIdx++)
    {
        APP_DEMO_SYS_CONFIG_FUNCS*  sysCfgFuncs;

        /* update device config list */
        sysCfgFuncs = &appDemoPpConfigList[devIdx].sysConfigFuncs;


        /* Get PP logical init configuration */
        rc = boardCfgFuncs->boardGetPpLogInitParams(boardRevId,
                                                    appDemoPpConfigList[devIdx].devNum,
                                                    &ppLogicalConfigParams);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs->boardGetPpLogInitParams", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* override logical init configuration according to app demo database */
        rc = appDemoUpdatePpLogicalInitParams(&ppLogicalConfigParams);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoUpdatePpLogicalInitParams", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* Do CPSS logical init and fill PP_INFO structure */
        rc = sysCfgFuncs->cpssPpLogicalInit(appDemoPpConfigList[devIdx].devNum,
                                            &ppLogicalConfigParams);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("sysCfgFuncs->cpssPpLogicalInit", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
        appDemoPpConfigList[devIdx].ppLogicalInitDone = GT_TRUE;
        osMemCpy(&ppUserLogicalConfigParams[appDemoPpConfigList[devIdx].devNum],
                 &ppLogicalConfigParams,
                 sizeof(CPSS_PP_CONFIG_INIT_STC));
    }

    return GT_OK;

} /* appDemoBoardLogicalInit */

/*******************************************************************************
* appDemoBoardGeneralInit
*
* DESCRIPTION:
*       Perform general initialization for all devices (Pp, Fa, Xbar).
*       This function includes initializations that common for all devices.
*
* INPUTS:
*       boardRevId      - Board revision Id.
*       boardCfgFuncs   - Board configuration functions.
*       numOfPp         - Number of Pp's in system.
*       numOfFa         - Number of Fa's in system.
*       numOfXbar       - Number of Xbar's in system.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       This function must be called after logical init.
*
*******************************************************************************/
static GT_STATUS appDemoBoardGeneralInit
(
    IN  GT_U8                   boardRevId,       /* Revision of this board             */
    IN  GT_BOARD_CONFIG_FUNCS   *boardCfgFuncs,   /* Config functions for this board    */
    IN  GT_U8                   numOfDev,         /* Number of devices in this board    */
    IN  GT_U8                   numOfFa,          /* Number of FAs in this board        */
    IN  GT_U8                   numOfXbar         /* Number of xbars in this board      */
)
{
    GT_STATUS                   rc = GT_OK;
    GT_U8                       devIdx;
    APP_DEMO_LIB_INIT_PARAMS    libInitParams;
    CPSS_PP_CONFIG_INIT_STC     ppLogicalConfigParams;

	osPrintf("appDemoBoardGeneralInit::devNo %d faNo %d xbarNo %d revId %d\r\n", \
			numOfDev,numOfFa,numOfXbar,boardRevId);


    /* Do general configuration for all devices */
    for (devIdx = SYSTEM_DEV_NUM_MAC(0); devIdx < SYSTEM_DEV_NUM_MAC(numOfDev); devIdx++)
    {
        APP_DEMO_SYS_CONFIG_FUNCS*  sysCfgFuncs;
        GT_U8                       dev;



        /* get init parameters from appdemo init array */
        sysCfgFuncs = &appDemoPpConfigList[devIdx].sysConfigFuncs;
        dev = appDemoPpConfigList[devIdx].devNum;

        /* Get PP logical init configuration */
        rc = boardCfgFuncs->boardGetPpLogInitParams(boardRevId,
                                                    appDemoPpConfigList[devIdx].devNum,
                                                    &ppLogicalConfigParams);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs->boardGetPpLogInitParams", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
        osPrintf("appDemoBoardGeneralInit::boardGetPpLogInitParams ok\r\n");
        
        /* override logical init configuration according to app demo database */
        rc = appDemoUpdatePpLogicalInitParams(&ppLogicalConfigParams);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoUpdatePpLogicalInitParams", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
        /* Get library initialization parameters */
        rc = boardCfgFuncs->boardGetLibInitParams(boardRevId,
                                                  appDemoPpConfigList[devIdx].devNum,
                                                  &libInitParams);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs->boardGetLibInitParams", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
        osPrintf("appDemoBoardGeneralInit::boardGetLibInitParams ok\r\n");
        
        /* override library initialization parameters according to app demo database */
        rc = appDemoUpdateLibInitParams(&libInitParams);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoUpdateLibInitParams", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* Initialize CPSS libraries accordingly to a given parameters */
        if(sysCfgFuncs->cpssLibrariesInit)
        {
            rc = sysCfgFuncs->cpssLibrariesInit(appDemoPpConfigList[devIdx].devNum,
                                                &libInitParams,
                                                &ppLogicalConfigParams);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("sysCfgFuncs->cpssLibrariesInit", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        osPrintf("appDemoBoardGeneralInit::cpssLibrariesInit ok\r\n");

        if(sysCfgFuncs->cpssPpGeneralInit)
        {
            /* Do CPSS general initialization for given device id */
            rc = sysCfgFuncs->cpssPpGeneralInit(dev);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("sysCfgFuncs->cpssPpGeneralInit", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        osPrintf("appDemoBoardGeneralInit::cpssPpGeneralInit ok\r\n");

        if(sysCfgFuncs->cpssTrafficEnable)
        {
            /* Enable traffic for given device */
            rc = sysCfgFuncs->cpssTrafficEnable(appDemoPpConfigList[devIdx].devNum);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("sysCfgFuncs->cpssTrafficEnable", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        osPrintf("appDemoBoardGeneralInit::cpssTrafficEnable ok\r\n");

        /* 
		  * Some procedure need be done when product initialization done correctly, 
		  * we will catch user signal to do these things
		  *
		  * by qinhs@autelan.com on Jul. 29 2008
		  */
		#ifdef __AX_PLATFORM__ 
		rc = npd_system_signal_hndlr_init();
		if(rc != GT_OK) {
			osPrintf("appDemoBoardGeneralInit::register signal handler fail %d\n",rc);
		}
		else {
			osPrintf("appDemoBoardGeneralInit::register signal handler ok\n");
		}
		#endif

        appDemoPpConfigList[devIdx].ppGeneralInitDone = GT_TRUE;
    }
    return GT_OK;
} /* appDemoBoardGeneralInit */


#ifndef __AX_PLATFORM__
/*******************************************************************************
* appDemoBoardTrafficEnable
*
* DESCRIPTION:
*       Perform board traffic enable process to enable all ports and devices.
*
* INPUTS:
*	  None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       This function must be called after logical init.
*
*******************************************************************************/
GT_STATUS appDemoBoardTrafficEnable
(
	void
)
{
    GT_STATUS                   rc = GT_OK;
    GT_U8                       devIdx;
	
    /* Do general configuration for all devices */
    for (devIdx = 0; devIdx < appDemoPpConfigDevAmount; devIdx++)
    {
        APP_DEMO_SYS_CONFIG_FUNCS*  sysCfgFuncs;
        GT_U8                       dev;

        /* get init parameters from appdemo init array */
        sysCfgFuncs = &appDemoPpConfigList[devIdx].sysConfigFuncs;
        dev = appDemoPpConfigList[devIdx].devNum;

        /* Enable traffic for given device */
        rc = sysCfgFuncs->cpssTrafficEnable(appDemoPpConfigList[devIdx].devNum);
        if (rc != GT_OK)
        {
            return rc;
        }
		osPrintf("appDemoBoardTrafficEnable::cpssTrafficEnable dev %d ok\n",dev);
    }

	return rc;
}
/* used in intScanPullingTaskset() for irq polling */
extern GT_U32   pollInterruptIsEnabled;
/*******************************************************************************
* appDemoBoardAfterSysUpIntEnable
*
* DESCRIPTION:
*       Enable DxCh devices' interrupt hierarchy to report interrupts 
*	   via enabling root mask register bits of the interrupt scanning-tree.
*
* INPUTS:
*	  None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       This function must be called after system started up.
*
*******************************************************************************/
GT_STATUS appDemoBoardAfterSysUpIntEnable
(
	void
)
{
    GT_STATUS                   rc = GT_OK;
    GT_U8                       devIdx, devNum = 0;
	unsigned int level = 0;
	/* enable syslog level SYSLOG_DBG_EVT */
 	level = cpss_debug_status_get();
	cpss_debug_level_set(0x8); 
    /* Do general configuration for all devices */
    for (devIdx = 0; devIdx < appDemoPpConfigDevAmount; devIdx++)
    {

        devNum = appDemoPpConfigList[devIdx].devNum;

        /* Enable traffic for given device */
        rc = prvCpssDrvInterruptsAfterSysUp(devNum);
        if (rc != GT_OK)
        {
            return rc;
        }
		osPrintfEvt("system startup end stage on device %d ok\r\n",devNum);
    }
	/* set pollInterruptIsEnabled to 1, enable interruptMainSr(). zhangdi@autelan.com 2012-02-07 */
	pollInterruptIsEnabled = 1;
	/* disable syslog */
  	cpss_debug_level_clr(0x8);
	cpss_debug_status_set(level); 
	return rc;
}

/*******************************************************************************
* appDemoBoardSigHandler
*
* DESCRIPTION:
*       Perform board signal process to catch up external signals
*
* INPUTS:
*	  signo - signal number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       
*
*******************************************************************************/
GT_STATUS appDemoBoardSigHandler
(
	int signo
)
{
	GT_STATUS rc = GT_OK;
	unsigned int level = 0;
	
	/* enable syslog level SYSLOG_DBG_EVT */
	level = cpss_debug_status_get();
	cpss_debug_level_set(0x8);
	osPrintf("appDemoBoardSigHandler::catch up signal %d\n",signo);
	if(APP_DEMO_TRAFFIC_ENABLE_SIGNUM == signo) {
		rc = appDemoBoardAfterSysUpIntEnable();
		if(GT_OK != rc) {
			osPrintfEvt("appDemoBoardSigHandler::appDemoBoardAfterSysUpIntEnable fail %d\n",rc);
			return rc;
		}
	}
	else {
		osPrintfEvt("appDemoBoardSigHandler::sigal number not match\n");
	}
	/* disable syslog */
	cpss_debug_level_clr(0x8);
	cpss_debug_status_set(level);
	
	return rc;
}
#endif


/*******************************************************************************
* appDemoUpdatePpPhase1Params
*
* DESCRIPTION:
*       Updates PP phase 1 params according to app demo database.
*
* INPUTS:
*       paramsPtr       - points to params to be updated
*
* OUTPUTS:
*       paramsPtr       - points to params after update
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS appDemoUpdatePpPhase1Params
(
    INOUT CPSS_PP_PHASE1_INIT_PARAMS    *paramsPtr
)
{
    GT_U32  value;

    CPSS_NULL_PTR_CHECK_MAC(paramsPtr);

    /* follow this sample code to override a parameter according        */
    /* to appDemo database entries                                      */
    /* please keep the convention that database entry name is the       */
    /* same as parameter to update                                      */
    /* if(appDemoDbEntryGet("[databse entry name]", &value) == GT_OK)   */
    /*    paramsPtr->[parameter to update] = value;                     */

    /* ingress buffer SRAM */
    if(appDemoDbEntryGet("ingressBufferMemoryEnableScrambling", &value) == GT_OK)
    {
        if(value == 0)
        {
            paramsPtr->ingressBufferMemoryEnableScrambling = GT_FALSE;
        }
        else
        {
            paramsPtr->ingressBufferMemoryEnableScrambling = GT_TRUE;
        }
    }

    /* egress buffer SRAM */
    if(appDemoDbEntryGet("egressBufferMemoryEnableScrambling", &value) == GT_OK)
    {
        if(value == 0)
        {
            paramsPtr->egressBufferMemoryEnableScrambling = GT_FALSE;
        }
        else
        {
            paramsPtr->egressBufferMemoryEnableScrambling = GT_TRUE;
        }
    }
    if(appDemoDbEntryGet("egressBufferRamType", &value) == GT_OK)
    {
        if(value == 0)
        {
            paramsPtr->egressBufferRamType = APPDEMO_EXMXPM_RAM_TYPE_SRAM_E;
        }
        else
        {
            paramsPtr->egressBufferRamType = APPDEMO_EXMXPM_RAM_TYPE_RLDRAM_E;
        }
    }

    /* routingSramCfgType (->external LPM sram) */
    if(appDemoDbEntryGet("routingSramCfgType", &value) == GT_OK)
    {
        switch(value)
        {
            case 0: paramsPtr->routingSramCfgType = CPSS_TWIST_INTERNAL_E;
                    break;
            case 1: paramsPtr->routingSramCfgType = CPSS_TWIST_EXTERNAL_E;
                    break;
            default: return GT_FAIL;
        }
    }

    /* external CSU */
    if(appDemoDbEntryGet("controlSramUsedArray_0", &value) == GT_OK)
    {
        if(value == 0)
        {
            paramsPtr->controlSramUsedArray[0] = GT_FALSE;
        }
        else
        {
            paramsPtr->controlSramUsedArray[0] = GT_TRUE;
        }
    }
    if(appDemoDbEntryGet("controlSramUsedArray_1", &value) == GT_OK)
    {
        if(value == 0)
        {
            paramsPtr->controlSramUsedArray[1] = GT_FALSE;
        }
        else
        {
            paramsPtr->controlSramUsedArray[1] = GT_TRUE;
        }
    }
    if(appDemoDbEntryGet("controlSramProtectArray_0", &value) == GT_OK)
    {
        if(value == 0)
        {
            paramsPtr->controlSramProtectArray[0] = APPDEMO_EXMXPM_CSU_PROTECT_PARITY_E;
        }
        else
        {
            paramsPtr->controlSramProtectArray[0] = APPDEMO_EXMXPM_CSU_PROTECT_ECC_E;
        }
    }
    if(appDemoDbEntryGet("controlSramProtectArray_1", &value) == GT_OK)
    {
        if(value == 0)
        {
            paramsPtr->controlSramProtectArray[1] = APPDEMO_EXMXPM_CSU_PROTECT_PARITY_E;
        }
        else
        {
            paramsPtr->controlSramProtectArray[1] = APPDEMO_EXMXPM_CSU_PROTECT_ECC_E;
        }
    }

    /* external TCAM */
    if(appDemoDbEntryGet("externalTcamUsed", &value) == GT_OK)
    {
        if(value == 0)
        {
            paramsPtr->externalTcamUsed = GT_FALSE;
        }
        else
        {
            paramsPtr->externalTcamUsed = GT_TRUE;
        }
    }

    if(appDemoDbEntryGet("intVecNum", &value) == GT_OK)
    {
        paramsPtr->intVecNum = value;

        if(paramsPtr->intVecNum == 0xFFFFFFFF)
        {
            appDemoBypassEventInitialize = GT_TRUE;
        }
    }
    /* initSerdesDefaults */
    if(appDemoDbEntryGet("initSerdesDefaults", &value) == GT_OK)
    {
        if(value == 0)
        {
            paramsPtr->initSerdesDefaults = GT_FALSE;
        }
        else
        {
            paramsPtr->initSerdesDefaults = GT_TRUE;
        }
    }
    /* serdes reference clock */
    if(appDemoDbEntryGet("serdesRefClock", &value) == GT_OK)
    {
        switch(value)
        {
        case 1:
            paramsPtr->serdesRefClock = APPDEMO_SERDES_REF_CLOCK_EXTERNAL_125_SINGLE_ENDED_E;
            break;
        case 2:
            paramsPtr->serdesRefClock = APPDEMO_SERDES_REF_CLOCK_EXTERNAL_125_DIFF_E;
            break;
        case 5: paramsPtr->serdesRefClock = APPDEMO_SERDES_REF_CLOCK_INTERNAL_125_E;
            break;
        default:
            return GT_FAIL;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* appDemoUpdatePpPhase2Params
*
* DESCRIPTION:
*       Updates PP phase 2 params according to app demo database.
*
* INPUTS:
*       paramsPtr       - points to params to be updated
*
* OUTPUTS:
*       paramsPtr       - points to params after update
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS appDemoUpdatePpPhase2Params
(
    INOUT CPSS_PP_PHASE2_INIT_PARAMS    *paramsPtr
)
{
    GT_U32  value;

    CPSS_NULL_PTR_CHECK_MAC(paramsPtr);

    /* follow this sample code to override a parameter according        */
    /* to appDemo database entries                                      */
    /* please keep the convention that database entry name is the       */
    /* same as parameter to update                                      */
    /* if(appDemoDbEntryGet("[databse entry name]", &value) == GT_OK)   */
    /*    paramsPtr->[parameter to update] = value;                     */

    if(appDemoDbEntryGet("netifSdmaPortGroupId", &value) == GT_OK)
    {
        paramsPtr->netifSdmaPortGroupId = value;
    }

    return GT_OK;
}

/*******************************************************************************
* appDemoUpdatePpLogicalInitParams
*
* DESCRIPTION:
*       Updates PP logical init params according to app demo database.
*
* INPUTS:
*       paramsPtr       - points to params to be updated
*
* OUTPUTS:
*       paramsPtr       - points to params after update
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS appDemoUpdatePpLogicalInitParams
(
    INOUT CPSS_PP_CONFIG_INIT_STC       *paramsPtr
)
{
    GT_U32  value;

    CPSS_NULL_PTR_CHECK_MAC(paramsPtr);

    /* follow this sample code to override a parameter according        */
    /* to appDemo database entries                                      */
    /* please keep the convention that database entry name is the       */
    /* same as parameter to update                                      */
    /* if(appDemoDbEntryGet("[databse entry name]", &value) == GT_OK)   */
    /*    paramsPtr->[parameter to update] = value;                     */

    /* Inlif */
    if(appDemoDbEntryGet("maxNumOfLifs", &value) == GT_OK)
        paramsPtr->maxNumOfLifs = value;
    if(appDemoDbEntryGet("inlifPortMode", &value) == GT_OK)
        paramsPtr->inlifPortMode = (APP_DEMO_CPSS_INLIF_PORT_MODE_ENT)value;

    /* Policer */
    if(appDemoDbEntryGet("maxNumOfPolicerEntries", &value) == GT_OK)
        paramsPtr->maxNumOfPolicerEntries = value;
    if(appDemoDbEntryGet("policerConformCountEn", &value) == GT_OK)
        paramsPtr->policerConformCountEn = (GT_BOOL)value;

    /* MPLS    */
    if(appDemoDbEntryGet("maxNumOfNhlfe", &value) == GT_OK)
        paramsPtr->maxNumOfNhlfe = value;
    if(appDemoDbEntryGet("maxNumOfMplsIfs", &value) == GT_OK)
        paramsPtr->maxNumOfMplsIfs = value;

    /* PCL     */
    if(appDemoDbEntryGet("maxNumOfPclAction", &value) == GT_OK)
        paramsPtr->maxNumOfPclAction = value;
    if(appDemoDbEntryGet("pclActionSize", &value) == GT_OK)
        paramsPtr->pclActionSize = value;

    /* Policy-based routing  */
    if(appDemoDbEntryGet("maxNumOfPceForIpPrefixes", &value) == GT_OK)
        paramsPtr->maxNumOfPceForIpPrefixes = value;
    if(appDemoDbEntryGet("usePolicyBasedRouting", &value) == GT_OK)
        paramsPtr->usePolicyBasedRouting = value;
    if(appDemoDbEntryGet("usePolicyBasedDefaultMc", &value) == GT_OK)
        paramsPtr->usePolicyBasedDefaultMc = value;

    /* Bridge           */
    if(appDemoDbEntryGet("maxVid", &value) == GT_OK)
        paramsPtr->maxVid = (GT_U16)value;
    if(appDemoDbEntryGet("maxMcGroups", &value) == GT_OK)
        paramsPtr->maxMcGroups = (GT_U16)value;

    /* IP Unicast     */
    if(appDemoDbEntryGet("maxNumOfVirtualRouters", &value) == GT_OK)
        paramsPtr->maxNumOfVirtualRouters = value;
    if(appDemoDbEntryGet("maxNumOfIpNextHop", &value) == GT_OK)
        paramsPtr->maxNumOfIpNextHop = value;

    /* IPv4 Unicast     */
    if(appDemoDbEntryGet("maxNumOfIpv4Prefixes", &value) == GT_OK)
        paramsPtr->maxNumOfIpv4Prefixes = value;

    /* IPv4/IPv6 Multicast   */
    if(appDemoDbEntryGet("maxNumOfIpv4McEntries", &value) == GT_OK)
        paramsPtr->maxNumOfIpv4McEntries = value;
    if(appDemoDbEntryGet("maxNumOfMll", &value) == GT_OK)
        paramsPtr->maxNumOfMll = value;

    /* IPv6 Unicast     */
    if(appDemoDbEntryGet("maxNumOfIpv6Prefixes", &value) == GT_OK)
        paramsPtr->maxNumOfIpv6Prefixes = value;

    /* IPv6 Multicast   */
    if(appDemoDbEntryGet("maxNumOfIpv6McGroups", &value) == GT_OK)
        paramsPtr->maxNumOfIpv6McGroups = value;

    /* Tunnels */
    if(appDemoDbEntryGet("maxNumOfTunnelEntries", &value) == GT_OK)
        paramsPtr->maxNumOfTunnelEntries = value;
    if(appDemoDbEntryGet("maxNumOfIpv4TunnelTerms", &value) == GT_OK)
        paramsPtr->maxNumOfIpv4TunnelTerms = value;

    /* tunnel termination (of all types) / TTI entries - for Dx devices */
    if(appDemoDbEntryGet("maxNumOfTunnelTerm", &value) == GT_OK)
        paramsPtr->maxNumOfTunnelTerm = value;

    /* inlifs */
    if(appDemoDbEntryGet("vlanInlifEntryType", &value) == GT_OK)
        paramsPtr->vlanInlifEntryType = value;

    /* if GT_FALSE - no division of memory pools for the use of different IP versions */
    if(appDemoDbEntryGet("ipMemDivisionOn", &value) == GT_OK)
        paramsPtr->ipMemDivisionOn = (GT_BOOL)value;

    /* devision of memory between IPv4 and IPv6 prefixes*/
    if(appDemoDbEntryGet("ipv6MemShare", &value) == GT_OK)
        paramsPtr->ipv6MemShare = value;

    /* Number of trunks */
    if(appDemoDbEntryGet("numOfTrunks", &value) == GT_OK)
        paramsPtr->numOfTrunks = value;

    /* PCL rule index and PCL ID for defualt IPv6 MC entry for devices */
    /* where IPv6 MC group search is implemented in PCL                */
    if(appDemoDbEntryGet("defIpv6McPclRuleIndex", &value) == GT_OK)
        paramsPtr->defIpv6McPclRuleIndex = value;
    if(appDemoDbEntryGet("vrIpv6McPclId", &value) == GT_OK)
        paramsPtr->vrIpv6McPclId = value;

    /* TCAM LPM DB configurations */
    if(appDemoDbEntryGet("lpmDbPartitionEnable", &value) == GT_OK)
        paramsPtr->lpmDbPartitionEnable = (GT_BOOL)value;
    if(appDemoDbEntryGet("lpmDbFirstTcamLine", &value) == GT_OK)
        paramsPtr->lpmDbFirstTcamLine = value;
    if(appDemoDbEntryGet("lpmDbLastTcamLine", &value) == GT_OK)
        paramsPtr->lpmDbLastTcamLine = value;
    if(appDemoDbEntryGet("lpmDbSupportIpv4", &value) == GT_OK)
        paramsPtr->lpmDbSupportIpv4 = (GT_BOOL)value;
    if(appDemoDbEntryGet("lpmDbSupportIpv6", &value) == GT_OK)
        paramsPtr->lpmDbSupportIpv6 = (GT_BOOL)value;

    /* number of memory configurations supported by the lpm db */
    if(appDemoDbEntryGet("lpmDbNumOfMemCfg", &value) == GT_OK)
        paramsPtr->lpmDbNumOfMemCfg = value;

    /* array of memory configurations supported by the lpm db */
    if(appDemoDbEntryGet("lpmDbMemCfgArray->routingSramCfgType", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->routingSramCfgType = (APP_DEMO_CPSS_LPM_MEM_CFG_ENT)value;
    if(appDemoDbEntryGet("lpmDbMemCfgArray->sramsSizeArray[0]", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->sramsSizeArray[0] = (CPSS_SRAM_SIZE_ENT)value;
    if(appDemoDbEntryGet("lpmDbMemCfgArray->sramsSizeArray[1]", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->sramsSizeArray[1] = (CPSS_SRAM_SIZE_ENT)value;
    if(appDemoDbEntryGet("lpmDbMemCfgArray->sramsSizeArray[2]", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->sramsSizeArray[2] = (CPSS_SRAM_SIZE_ENT)value;
    if(appDemoDbEntryGet("lpmDbMemCfgArray->sramsSizeArray[2]", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->sramsSizeArray[2] = (CPSS_SRAM_SIZE_ENT)value;
    if(appDemoDbEntryGet("lpmDbMemCfgArray->sramsSizeArray[3]", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->sramsSizeArray[3] = (CPSS_SRAM_SIZE_ENT)value;
    if(appDemoDbEntryGet("lpmDbMemCfgArray->sramsSizeArray[4]", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->sramsSizeArray[4] = (CPSS_SRAM_SIZE_ENT)value;
    if(appDemoDbEntryGet("lpmDbMemCfgArray->sramsSizeArray[5]", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->sramsSizeArray[5] = (CPSS_SRAM_SIZE_ENT)value;
    if(appDemoDbEntryGet("lpmDbMemCfgArray->sramsSizeArray[6]", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->sramsSizeArray[6] = (CPSS_SRAM_SIZE_ENT)value;
    if(appDemoDbEntryGet("lpmDbMemCfgArray->sramsSizeArray[7]", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->sramsSizeArray[7] = (CPSS_SRAM_SIZE_ENT)value;
    if(appDemoDbEntryGet("lpmDbMemCfgArray->sramsSizeArray[8]", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->sramsSizeArray[8] = (CPSS_SRAM_SIZE_ENT)value;
    if(appDemoDbEntryGet("lpmDbMemCfgArray->sramsSizeArray[9]", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->sramsSizeArray[9] = (CPSS_SRAM_SIZE_ENT)value;
    if(appDemoDbEntryGet("lpmDbMemCfgArray->numOfSramsSizes", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->numOfSramsSizes = value;

    /* DIT configuration */
    if(appDemoDbEntryGet("ditMemoryMode", &value) == GT_OK)
        paramsPtr->ditMemoryMode = (APP_DEMO_CPSS_DIT_MEMORY_MODE_ENT)value;
    if(appDemoDbEntryGet("ditIpMcMode", &value) == GT_OK)
        paramsPtr->ditIpMcMode = (APP_DEMO_CPSS_DIT_IP_MC_MODE_ENT)value;
    if(appDemoDbEntryGet("maxNumOfDitUcIpEntries", &value) == GT_OK)
        paramsPtr->maxNumOfDitUcIpEntries = value;
    if(appDemoDbEntryGet("maxNumOfDitUcMplsEntries", &value) == GT_OK)
        paramsPtr->maxNumOfDitUcMplsEntries = value;
    if(appDemoDbEntryGet("maxNumOfDitUcOtherEntries", &value) == GT_OK)
        paramsPtr->maxNumOfDitUcOtherEntries = value;
    if(appDemoDbEntryGet("maxNumOfDitMcIpEntries", &value) == GT_OK)
        paramsPtr->maxNumOfDitMcIpEntries = value;
    if(appDemoDbEntryGet("maxNumOfDitMcMplsEntries", &value) == GT_OK)
        paramsPtr->maxNumOfDitMcMplsEntries = value;
    if(appDemoDbEntryGet("maxNumOfDitMcOtherEntries", &value) == GT_OK)
        paramsPtr->maxNumOfDitMcOtherEntries = value;

    /* fdb table mode */
    if(appDemoDbEntryGet("fdbTableInfo.fdbTableMode", &value) == GT_OK)
        paramsPtr->fdbTableInfo.fdbTableMode = value;
    if(appDemoDbEntryGet("fdbTableInfo.fdbToLutRatio", &value) == GT_OK)
        paramsPtr->fdbTableInfo.fdbToLutRatio = value;
    if(appDemoDbEntryGet("fdbTableInfo.fdbHashSize", &value) == GT_OK)
        paramsPtr->fdbTableInfo.fdbHashSize = value;

    /* note that external TCAM info parameters can't be modified */

    return GT_OK;
}

/*******************************************************************************
* appDemoUpdateLibInitParams
*
* DESCRIPTION:
*       Updates lib init params according to app demo database.
*
* INPUTS:
*       paramsPtr       - points to params to be updated
*
* OUTPUTS:
*       paramsPtr       - points to params after update
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS appDemoUpdateLibInitParams
(
    INOUT APP_DEMO_LIB_INIT_PARAMS      *paramsPtr
)
{
    GT_U32  value;

    CPSS_NULL_PTR_CHECK_MAC(paramsPtr);

    /* follow this sample code to override a parameter according        */
    /* to appDemo database entries                                      */
    /* please keep the convention that database entry name is the       */
    /* same as parameter to update                                      */
    /* if(appDemoDbEntryGet("[databse entry name]", &value) == GT_OK)   */
    /*    paramsPtr->[parameter to update] = value;                     */

    if(appDemoDbEntryGet("initBridge", &value) == GT_OK)
        paramsPtr->initBridge = (GT_BOOL)value;
    if(appDemoDbEntryGet("initClassifier", &value) == GT_OK)
        paramsPtr->initClassifier = (GT_BOOL)value;
    if(appDemoDbEntryGet("initCos", &value) == GT_OK)
        paramsPtr->initCos = (GT_BOOL)value;
    if(appDemoDbEntryGet("initIpv4", &value) == GT_OK)
        paramsPtr->initIpv4 = (GT_BOOL)value;
    if(appDemoDbEntryGet("initIpv6", &value) == GT_OK)
        paramsPtr->initIpv6 = (GT_BOOL)value;
    if(appDemoDbEntryGet("initIpv4Filter", &value) == GT_OK)
        paramsPtr->initIpv4Filter = (GT_BOOL)value;
    if(appDemoDbEntryGet("initIpv4Tunnel", &value) == GT_OK)
        paramsPtr->initIpv4Tunnel = (GT_BOOL)value;
    if(appDemoDbEntryGet("initLif", &value) == GT_OK)
        paramsPtr->initLif = (GT_BOOL)value;
    if(appDemoDbEntryGet("initMirror", &value) == GT_OK)
        paramsPtr->initMirror = (GT_BOOL)value;
    if(appDemoDbEntryGet("initMpls", &value) == GT_OK)
        paramsPtr->initMpls = (GT_BOOL)value;
    if(appDemoDbEntryGet("initMplsTunnel", &value) == GT_OK)
        paramsPtr->initMplsTunnel = (GT_BOOL)value;
    if(appDemoDbEntryGet("initNetworkIf", &value) == GT_OK)
        paramsPtr->initNetworkIf = (GT_BOOL)value;
    if(appDemoDbEntryGet("initI2c", &value) == GT_OK)
        paramsPtr->initI2c = (GT_BOOL)value;
    if(appDemoDbEntryGet("initPhy", &value) == GT_OK)
        paramsPtr->initPhy = (GT_BOOL)value;
    if(appDemoDbEntryGet("initPort", &value) == GT_OK)
        paramsPtr->initPort = (GT_BOOL)value;
    if(appDemoDbEntryGet("initTc", &value) == GT_OK)
        paramsPtr->initTc = (GT_BOOL)value;
    if(appDemoDbEntryGet("initTrunk", &value) == GT_OK)
        paramsPtr->initTrunk = (GT_BOOL)value;
    if(appDemoDbEntryGet("initPcl", &value) == GT_OK)
        paramsPtr->initPcl = (GT_BOOL)value;
    if(appDemoDbEntryGet("initPolicer", &value) == GT_OK)
        paramsPtr->initPolicer = (GT_BOOL)value;
    if(appDemoDbEntryGet("initAction", &value) == GT_OK)
        paramsPtr->initAction = (GT_BOOL)value;
    if(appDemoDbEntryGet("initSflow", &value) == GT_OK)
        paramsPtr->initSflow = (GT_BOOL)value;
    if(appDemoDbEntryGet("initNst", &value) == GT_OK)
        paramsPtr->initNst = (GT_BOOL)value;

    return GT_OK;
}

/*******************************************************************************
* appDemoOnDistributedSimAsicSideSet
*
* DESCRIPTION:
*       set value to the appDemoOnDistributedSimAsicSide flag.
*
* INPUTS:
*       value  - value to be set
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_STATE - if called after initialization of system -- after cpssInitSystem
*
* COMMENTS:
*       Function is called from the TERMINAL
*
*******************************************************************************/
GT_STATUS appDemoOnDistributedSimAsicSideSet
(
    IN GT_U32  value
)
{
    if(systemInitialized == GT_TRUE)
    {
        /* will not have effect if called after initialization of system */
        osPrintf("appDemoOnDistributedSimAsicSideSet : should be called before cpssInitSystem \n");

        return GT_BAD_STATE;
    }

    /* when appDemoOnDistributedSimAsicSide != 0
       this image is running on simulation:
       1. our application on distributed "Asic side"
       2. the Other application that run on the "application side" is in
          charge about all Asic configurations
       3. our application responsible to initialize the cpss DB.
       4. the main motivation is to allow "Galtis wrappers" to be called on
          this side !
          (because the Other side application may have difficult running
           "Galtis wrappers" from there)
    */
    appDemoOnDistributedSimAsicSide = value;

    return GT_OK;
}

#ifdef ASIC_SIMULATION
/*******************************************************************************
* appDemoRtosOnSimulationInit
*
* DESCRIPTION:
*       initialize the Simulation section on the "target board" part of the
*       "RTOS on simulation" .
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
*
* COMMENTS:
*       Function is called as first step of the cpssInitSystem
*
*******************************************************************************/
void appDemoRtosOnSimulationInit
(
    void
)
{
    SIM_OS_FUNC_BIND_STC simOsBindInfo;
    static int initDone = 0;

    if(initDone)
    {
        /* allow to call this before cpssInitSystem */
        return;
    }

    /* Must be called before use any of OS functions. */
    osWrapperOpen(NULL);

    /**********************************************/
    /* bind the simulation with extra OS services */
    /**********************************************/
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
    }

    /************************************************/
    /* init the kernel of simulation -- the PPs,FAs */
    /************************************************/
    simulationLibInit();

    /* state that init was done */
    initDone = 1;

    return;
}
#endif /*ASIC_SIMULATION*/


/*******************************************************************************
* appDemoCpuEtherPortUsed
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
static GT_BOOL appDemoCpuEtherPortUsed(void)
{
    return appDemoSysConfig.cpuEtherPortUsed;
}

/*******************************************************************************
* APP_DEMO_CPSS_EVENT_BIND_FUNC
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
static GT_STATUS appDemoEventBind
(
    IN  CPSS_UNI_EV_CAUSE_ENT uniEventArr[],
    IN  GT_U32               arrLength,
    OUT GT_U32               *hndlPtr
)
{
    return cpssEventBind(uniEventArr,arrLength,hndlPtr);
}

/*******************************************************************************
* APP_DEMO_CPSS_EVENT_SELECT_FUNC
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
static GT_STATUS appDemoEventSelect
(
    IN  GT_U32              hndl,
    IN  GT_U32              *timeoutPtr,
    OUT GT_U32              evBitmapArr[],
    IN  GT_U32              evBitmapArrLength
)
{
    return  cpssEventSelect(hndl,timeoutPtr,evBitmapArr,evBitmapArrLength);
}

/*******************************************************************************
* APP_DEMO_CPSS_EVENT_RECV_FUNC
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
static GT_STATUS appDemoEventRecv
(
    IN  GT_U32              hndl,
    IN  CPSS_UNI_EV_CAUSE_ENT evCause,
    OUT GT_U32              *evExtDataPtr,
    OUT GT_U8               *evDevPtr
)
{
    return cpssEventRecv(hndl,evCause,evExtDataPtr,evDevPtr);
}

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
static GT_STATUS appDemoEventDeviceMaskSet
(
    IN GT_U8                    devNum,
    IN CPSS_UNI_EV_CAUSE_ENT    uniEvent,
    IN CPSS_EVENT_MASK_SET_ENT  operation
)
{
    return cpssEventDeviceMaskSet(devNum,uniEvent,operation);
}


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
static GT_STATUS appDemoPpConfigGet
(
    IN GT_U8    devIndex,
    OUT CMD_APP_PP_CONFIG* ppConfigPtr
)
{
    if(ppConfigPtr == NULL)
    {
        return GT_BAD_PTR;
    }

    if(devIndex >= PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        return GT_OUT_OF_RANGE;
    }

    ppConfigPtr->deviceId = appDemoPpConfigList[devIndex].deviceId;
    ppConfigPtr->devNum = appDemoPpConfigList[devIndex].devNum;
    ppConfigPtr->valid = appDemoPpConfigList[devIndex].valid;
    ppConfigPtr->wa.trunkToCpuPortNum = appDemoPpConfigList[devIndex].wa.trunkToCpuPortNum;

    return GT_OK;
}

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
static GT_BOOL appDemoIsSystemInitialized
(
    void
)
{
    return systemInitialized;
}




/*******************************************************************************
* appDemoPpConfigPrint
*
* DESCRIPTION:
*       Return system configuration that was set by user .
*
* INPUTS:
*       devNum              - device number.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK               - on success,
*       GT_BAD_PARAM        - if deviceId == NULL or current device not exist.
*       GT_NO_SUCH          - if there is no next deviceId
*       GT_NOT_INITIALIZED  - Library not initialized
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS appDemoPpConfigPrint
(
    IN  GT_U8  devNum
)
{

#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)

#ifdef EXMXPM_FAMILY
GT_U8 i;
PRV_CPSS_EXMXPM_MODULE_CONFIG_STC *moduleCnfgPtr = &PRV_CPSS_EXMXPM_PP_MAC(devNum)->moduleCfg;
PRV_CPSS_EXMXPM_PP_TBLS_ADDR_STC  *tblsPtr       = &PRV_CPSS_EXMXPM_PP_MAC(devNum)->tblsAddr;
#endif

#ifdef CHX_FAMILY
       PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;
#endif


        osPrintf("\n");
        osPrintf("                  CPSS System configuration                \n");
        osPrintf(" __________________________________________________________\n");
        osPrintf("|          Config parameter            |        value      |\n");
        osPrintf("|__________________________________________________________|\n");
        osPrintf("|__________numOfTrunks_________________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].numOfTrunks);
        osPrintf("|__________maxNumOfVirtualRouters______|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfVirtualRouters);
        osPrintf("|__________maxNumOfIpNextHop___________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfIpNextHop);
#ifdef CHX_FAMILY

        osPrintf("|__________maxNumOfMll_________________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfMll);
        osPrintf("|__________maxNumOfIpv4Prefixes________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfIpv4Prefixes);
        osPrintf("|__________maxNumOfIpv6Prefixes________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfIpv6Prefixes);

        osPrintf("|__________maxNumOfIpv4McEntries_______|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfIpv4McEntries);

        osPrintf("|__________maxNumOfPceForIpPrefixes____|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfPceForIpPrefixes);
        osPrintf("|__________usePolicyBasedRouting_______|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].usePolicyBasedRouting);
        osPrintf("|__________usePolicyBasedDefaultMc_____|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].usePolicyBasedDefaultMc);
        osPrintf("|__________maxNumOfTunnelEntries_______|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfTunnelEntries);
        osPrintf("|__________maxNumOfIpv4TunnelTerms_____|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfIpv4TunnelTerms);
        osPrintf("|__________lpmDbPartitionEnable________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].lpmDbPartitionEnable);
        osPrintf("|__________lpmDbSupportIpv4____________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].lpmDbSupportIpv4);
        osPrintf("|__________lpmDbSupportIpv6____________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].lpmDbSupportIpv6);
        osPrintf("|__________lpmDbFirstTcamLine__________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].lpmDbFirstTcamLine);
        osPrintf("|__________lpmDbLastTcamLine___________|________%-8d___|\n",
                 ppUserLogicalConfigParams[devNum].lpmDbLastTcamLine);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
        osPrintf("|__________maxNumOfLifs________________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfLifs);
        osPrintf("|__________inlifPortMode_______________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].inlifPortMode);
        osPrintf("|__________maxNumOfNhlfe_______________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfNhlfe);
        osPrintf("|__________maxNumOfMplsIfs_____________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfMplsIfs);
        osPrintf("|__________ipv6MemShare________________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].ipv6MemShare);
        osPrintf("|__________maxNumOfPolicerEntries______|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfPolicerEntries);
        osPrintf("|__________lpmDbNumOfMemCfg____________|________%-8d___|\n",
                 ppUserLogicalConfigParams[devNum].lpmDbNumOfMemCfg);
        osPrintf("|__________lpm db ip memory config:____|_______________|\n");
        osPrintf("|__________routingSramCfgType__________|________%-8d___|\n",
                 ppUserLogicalConfigParams[devNum].lpmDbMemCfgArray->routingSramCfgType);

        osPrintf("|__________numOfSramsSizes_____________|________%-8d___|\n",
                 ppUserLogicalConfigParams[devNum].lpmDbMemCfgArray->numOfSramsSizes);
        for( i=0; i<ppUserLogicalConfigParams[devNum].lpmDbMemCfgArray->numOfSramsSizes; i++)
        {
        osPrintf("|__________sramsSizeArray[%d]__________|________%-8d___|\n",
                     i,ppUserLogicalConfigParams[devNum].lpmDbMemCfgArray->sramsSizeArray[i]);
        }
        osPrintf("|__________ditMemoryMode_______________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].ditMemoryMode);
        osPrintf("|__________ditIpMcMode_________________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].ditIpMcMode);
        osPrintf("|__________maxNumOfDitUcIpEntries______|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfDitUcIpEntries);
        osPrintf("|__________maxNumOfDitUcMplsEntries____|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfDitUcMplsEntries);
        osPrintf("|__________maxNumOfDitUcOtherEntries___|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfDitUcOtherEntries);
        osPrintf("|__________maxNumOfDitMcIpEntries______|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfDitMcIpEntries);
        osPrintf("|__________maxNumOfDitMcMplsEntries____|________%-8d___|\n",
                   ppUserLogicalConfigParams[devNum].maxNumOfDitMcMplsEntries);
        osPrintf("|__________maxNumOfDitMcOtherEntries___|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfDitMcOtherEntries);
        osPrintf("|__________fdbHashSize_________________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].fdbTableInfo.fdbHashSize);
        osPrintf("|__________fdbTableMode________________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].fdbTableInfo.fdbTableMode);
        osPrintf("|__________fdbToLutRatio_______________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].fdbTableInfo.fdbToLutRatio);
        osPrintf("|__________extTcamActionLinesMemCntl0__|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].externalTcamInfo.actionLinesInMemoryControl0);
        osPrintf("|__________extTcamActionLinesMemCntl1__|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].externalTcamInfo.actionLinesInMemoryControl1);

#endif  /* EXMXPM_FAMILY */

        osPrintf("|______________________________________________________|\n");
        osPrintf("|___________________other_params_______________________|\n");
        osPrintf("|______________________________________________________|\n");
        osPrintf("|__________maxMcGroups_________________|_______________|\n",
                    ppUserLogicalConfigParams[devNum].maxMcGroups);
        osPrintf("|__________maxVid______________________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxVid);
        osPrintf("|__________maxNumOfPclAction___________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfPclAction);
        osPrintf("|__________pclActionSize_______________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].pclActionSize);
        osPrintf("|__________policerConformCountEn_______|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].policerConformCountEn);
        osPrintf("|__________maxNumOfIpv6McGroups________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfIpv6McGroups);
        osPrintf("|__________maxNumOfTunnelTerm__________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfTunnelTerm);
        osPrintf("|__________vlanInlifEntryType__________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].vlanInlifEntryType);
        osPrintf("|__________ipMemDivisionOn_____________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].ipMemDivisionOn);
        osPrintf("|__________defIpv6McPclRuleIndex_______|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].defIpv6McPclRuleIndex);
        osPrintf("|__________vrIpv6McPclId_______________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].vrIpv6McPclId);
        osPrintf("|______________________________________________________|\n");
        osPrintf("|_________________some_table_sizes_____________________|\n");
        osPrintf("|______________________________________________________|\n");

#ifdef CHX_FAMILY
        osPrintf("|__________amount of CNC blocks________|________%-8d___|\n",
                    fineTuningPtr->tableSize.cncBlocks);
        osPrintf("|__________entries num in  CNC block___|________%-8d___|\n",
                    fineTuningPtr->tableSize.cncBlockNumEntries);
        osPrintf("|__________bufferMemory________________|________%-8d___|\n",
                    fineTuningPtr->tableSize.bufferMemory);
        osPrintf("|__________fdb_________________________|________%-8d___|\n",
                    fineTuningPtr->tableSize.fdb);
        osPrintf("|__________mllPairs____________________|________%-8d___|\n",
                    fineTuningPtr->tableSize.mllPairs);
        osPrintf("|__________egressPolicersNum___________|________%-8d___|\n",
                    fineTuningPtr->tableSize.egressPolicersNum);
        osPrintf("|__________policersNum_________________|________%-8d___|\n",
                    fineTuningPtr->tableSize.policersNum);
        osPrintf("|__________policyTcamRaws______________|________%-8d___|\n",
                    fineTuningPtr->tableSize.policyTcamRaws);
        osPrintf("|__________routerAndTunnelTermTcam_____|________%-8d___|\n",
                    fineTuningPtr->tableSize.routerAndTunnelTermTcam);
        osPrintf("|__________tunnelStart_________________|________%-8d___|\n",
                    fineTuningPtr->tableSize.tunnelStart);
        osPrintf("|__________routerArp___________________|________%-8d___|\n",
                    fineTuningPtr->tableSize.routerArp);
        osPrintf("|__________routerNextHop_______________|________%-8d___|\n",
                    fineTuningPtr->tableSize.routerNextHop);
        osPrintf("|__________stgNum______________________|________%-8d___|\n",
                    fineTuningPtr->tableSize.stgNum);
        osPrintf("|__________transmitDescr_______________|________%-8d___|\n",
                    fineTuningPtr->tableSize.transmitDescr);
        osPrintf("|__________vidxNum_____________________|________%-8d___|\n",
                    fineTuningPtr->tableSize.vidxNum);
#endif

#ifdef EXMXPM_FAMILY
        osPrintf("|__________fdbMemoryMode_______________|________%-8d___|\n",
                    moduleCnfgPtr->bridgeCfg.fdbMemoryMode);
        osPrintf("|__________fdbNumOfEntries_____________|________%-8d___|\n",
                    moduleCnfgPtr->bridgeCfg.fdbNumOfEntries);
        osPrintf("|__________fdbTcamNumOfEntries_________|________%-8d___|\n",
                    moduleCnfgPtr->bridgeCfg.fdbTcamNumOfEntries);
        osPrintf("|__________ditMcIpEntriesNum___________|________%-8d___|\n",
                    moduleCnfgPtr->ditCfg.mcIpEntriesNum);
        osPrintf("|__________ditUcIpEntriesNum___________|________%-8d___|\n",
                    moduleCnfgPtr->ditCfg.ucIpEntriesNum);
        osPrintf("|__________externalTcamUsed____________|________%-8d___|\n",
                    moduleCnfgPtr->externalTcamCfg.externalTcamUsed);
        osPrintf("|__________stgNum______________________|________%-8d___|\n",
                    tblsPtr->VLAN.STGTable.numOfEntries);
        osPrintf("|__________vidxNum_____________________|________%-8d___|\n",
                    tblsPtr->VLAN.VIDXTable.numOfEntries);
        osPrintf("|__________vlanNum_____________________|________%-8d___|\n",
                    tblsPtr->VLAN.VLANTable.numOfEntries);
        osPrintf("|__________ttiDSATagQoS2QoSNum_________|________%-8d___|\n",
                    tblsPtr->TTI.DSATagQoS2QoSParametersMapMemory.numOfEntries);
        osPrintf("|__________ttiDSCP2QoS_________________|________%-8d___|\n",
                    tblsPtr->TTI.DSCP2QoSParametersMapMemory.numOfEntries);
        osPrintf("|__________ttiMac2MeNum________________|________%-8d___|\n",
                    tblsPtr->TTI.MAC2METCAM.numOfEntries);
        osPrintf("|__________ttiPortProtocolVIDAndQoSNum_|________%-8d___|\n",
                    tblsPtr->TTI.portProtocolVIDAndQoSConfigTable.numOfEntries);
        osPrintf("|__________ttiPortVLANAndQoS___________|________%-8d___|\n",
                    tblsPtr->TTI.portVLANAndQoSConfigTable.numOfEntries);
        osPrintf("|__________ttiVirtualPortID____________|________%-8d___|\n",
                    tblsPtr->TTI.virtualPortIDAssignmentMemory.numOfEntries);
        osPrintf("|__________ttiUserDefinedBytesCnfg_____|________%-8d___|\n",
                    tblsPtr->TTI.userDefinedBytesConfigMemory.numOfEntries);
        osPrintf("|__________ttiUserDefinedBytesId_______|________%-8d___|\n",
                    tblsPtr->TTI.userDefinedBytesIDMemory);
        osPrintf("|__________pclInternalInLIFNum_________|________%-8d___|\n",
                    tblsPtr->PCL.internalInLIFMemory.numOfEntries);
        osPrintf("|__________pclIPCLConfigNum____________|________%-8d___|\n",
                    tblsPtr->PCL.IPCLConfigMemory.numOfEntries);
        osPrintf("|__________mtARPTunnelMemoryNum________|________%-8d___|\n",
                    tblsPtr->MT.ARPTunnelMemory.numOfEntries);
        osPrintf("|__________mtFdbEntriesTcamNum_________|________%-8d___|\n",
                    tblsPtr->MT.fdbEntriesForTcam.numOfEntries);
        osPrintf("|__________mtFDBMemoryNum______________|________%-8d___|\n",
                    tblsPtr->MT.FDBMemory.numOfEntries);
        osPrintf("|__________mtFDBTCAMNum________________|________%-8d___|\n",
                    tblsPtr->MT.FDBTCAM.numOfEntries);
        osPrintf("|__________mtFDBTSNum__________________|________%-8d___|\n",
                    tblsPtr->MT.FDBTSMemory.numOfEntries);
        osPrintf("|__________extTcamAction1Num___________|________%-8d___|\n",
                    tblsPtr->externalMemory.extTcamAction1Table.numOfEntries);
        osPrintf("|__________extTcamActionNum____________|________%-8d___|\n",
                    tblsPtr->externalMemory.extTcamActionTable.numOfEntries);
        osPrintf("|__________extFdbLutNum________________|________%-8d___|\n",
                    tblsPtr->externalMemory.fdbLutTable.numOfEntries);
        osPrintf("|__________extFdbNum___________________|________%-8d___|\n",
                    tblsPtr->externalMemory.fdbTable.numOfEntries);
        osPrintf("|__________extInlif____________________|________%-8d___|\n",
                    tblsPtr->externalMemory.inlif.numOfEntries);
        osPrintf("|__________tccInternalActionNum________|________%-8d___|\n",
                    tblsPtr->TCC.internalAction.numOfEntries);
        osPrintf("|__________tccInternalTcamNum__________|________%-8d___|\n",
                    tblsPtr->TCC.internalTcam.numOfEntries);
        osPrintf("|__________MPLSInterfaceNum____________|________%-8d___|\n",
                    tblsPtr->MPLS.MPLSInterfaceTable.numOfEntries);
        osPrintf("|__________MPLSNhlfeNum________________|________%-8d___|\n",
                    tblsPtr->MPLS.MPLSNhlfeTable.numOfEntries);
        osPrintf("|__________ingrPortsBridge_____________|________%-8d___|\n",
                    tblsPtr->L2I.ingrPortsBridgeConfig.ingrPortBridgeConfigTable.numOfEntries);
        osPrintf("|__________routerNSTNum________________|________%-8d___|\n",
                    tblsPtr->IPvX.routerNSTTable.numOfEntries);
        osPrintf("|__________routerNextHopNum____________|________%-8d___|\n",
                    tblsPtr->IPvX.routerNextHopTable.numOfEntries);
        osPrintf("|__________iCCCInternalNum_____________|________%-8d___|\n",
                    tblsPtr->ICCC.iCCCInternalMemory.numOfEntries);
        osPrintf("|__________eCCCInternalNum_____________|________%-8d___|\n",
                    tblsPtr->ECCC.eCCCInternalMemory.numOfEntries);
        osPrintf("|__________egrPolicerManagementNum_____|________%-8d___|\n",
                    tblsPtr->EPLR.egrPolicerManagementCntrsMemory.numOfEntries);
        osPrintf("|__________egrPolicerReMarkingNum______|________%-8d___|\n",
                    tblsPtr->EPLR.egrPolicerReMarkingMemory.numOfEntries);
        osPrintf("|__________ingrPolicerManagementNum____|________%-8d___|\n",
                    tblsPtr->IPLR.ingrPolicerManagementCntrsMemory.numOfEntries);
        osPrintf("|__________iingrPolicerReMarkingNum____|________%-8d___|\n",
                    tblsPtr->IPLR.ingrPolicerReMarkingMemory.numOfEntries);

#endif /* EXMXPM_FAMILY */


#endif  /* (defined CHX_FAMILY) || (defined EXMXPM_FAMILY) */

        return GT_OK;

}




/*******************************************************************************
* applicationExtServicesBind
*
* DESCRIPTION:
*       the mainCmd calls this function , so the application (that implement
*       this function) will bind the mainCmd (and GaltisWrapper) with OS ,
*       external driver functions  and other application functions
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
)
{
    GT_STATUS   rc;


    if(extDrvFuncBindInfoPtr == NULL ||
       osFuncBindPtr == NULL ||
       osExtraFuncBindPtr == NULL ||
       extraFuncBindPtr == NULL)
    {
        return GT_BAD_PTR;
    }


    /* must be called before any Os function */
    rc = osWrapperOpen(NULL);
    if(rc != GT_OK)
    {
        return rc;
    }

#if defined(SHARED_MEMORY) && (!defined(LINUX_SIM))
    /* Shared memory approach on ARM should use dlsym-based workaround for pointers to functions */
    rc = shrMemGetDefaultExtDrvFuncs(extDrvFuncBindInfoPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = shrMemGetDefaultOsBindFuncsThruDynamicLoader(osFuncBindPtr);
    if(rc != GT_OK)
    {
        return rc;
    }
#else
    rc = cpssGetDefaultExtDrvFuncs(extDrvFuncBindInfoPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = cpssGetDefaultOsBindFuncs(osFuncBindPtr);
    if(rc != GT_OK)
    {
        return rc;
    }
#endif

#if 0
    /* 
        *   Such as: cpssOsMalloc -> osMalloc
        *                add by luoxun@autelan.com
        **/
    rc = cpssExtServicesBind(extDrvFuncBindInfoPtr, osFuncBindPtr);
    if(rc != GT_OK)
    {
        return rc;
    }
#endif

    osFuncBindPtr->osTaskBindInfo.osTaskCreateFunc = osTaskCreate;
    osFuncBindPtr->osTaskBindInfo.osTaskDeleteFunc = osTaskDelete;
    /*osFuncBindPtr->osTaskBindInfo.osTaskGetSelfFunc = osTaskGetSelf;*/

    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketTcpCreate                       );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketTcpCreate                       );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketUdpCreate                       );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketTcpDestroy                      );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketUdpDestroy                      );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketCreateAddr                      );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketDestroyAddr                     );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketBind                            );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketListen                          );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketAccept                          );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketConnect                         );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketSetNonBlock                     );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketSetBlock                        );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketSend                            );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketSendTo                          );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketRecv                            );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketRecvFrom                        );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketSetSocketNoLinger               );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketExtractIpAddrFromSocketAddr     );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketGetSocketAddrSize               );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketShutDown                        );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSelectCreateSet                       );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSelectEraseSet                        );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSelectZeroSet                         );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSelectAddFdToSet                      );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSelectClearFdFromSet                  );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSelectIsFdSet                         );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSelectCopySet                         );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSelect                                );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketGetSocketFdSetSize              );

    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osTasksBindInfo,osTaskGetSelf                            );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osTasksBindInfo,osSetTaskPrior                           );

    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osPoolsBindInfo,gtPoolCreatePool        );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osPoolsBindInfo,gtPoolCreateDmaPool     );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osPoolsBindInfo,gtPoolReCreatePool      );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osPoolsBindInfo,gtPoolDeletePool        );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osPoolsBindInfo,gtPoolGetBuf            );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osPoolsBindInfo,gtPoolFreeBuf           );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osPoolsBindInfo,gtPoolGetBufSize        );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osPoolsBindInfo,gtPoolExpandPool        );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osPoolsBindInfo,gtPoolGetBufFreeCnt     );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osPoolsBindInfo,gtPoolPrintStats        );

    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,cpuEthernetPortBindInfo,cmdIsCpuEtherPortUsed        ,appDemoCpuEtherPortUsed             );

    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,eventsBindInfo,cmdCpssEventBind         ,appDemoEventBind             );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,eventsBindInfo,cmdCpssEventSelect       ,appDemoEventSelect           );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,eventsBindInfo,cmdCpssEventRecv         ,appDemoEventRecv             );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,eventsBindInfo,cmdCpssEventDeviceMaskSet,appDemoEventDeviceMaskSet    );


    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppDbEntryAdd ,appDemoDbEntryAdd );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppDbEntryGet ,appDemoDbEntryGet );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppDbDump     ,appDemoDbDump     );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppPpConfigGet,appDemoPpConfigGet);
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdInitSystem    ,cpssInitSystem    );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdInitConfi     ,confi2InitSystem    );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppShowBoardsList         ,appDemoShowBoardsList    );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppIsSystemInitialized    ,appDemoIsSystemInitialized);
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppPpConfigPrint,appDemoPpConfigPrint);
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppAllowProcessingOfAuqMessages ,
                         appDemoAllowProcessingOfAuqMessages);


#ifdef EXMXPM_FAMILY
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppExMxPmExternalTcamRuleIndexByLenAndLookupGet,appDemoExMxPmExternalTcamRuleIndexByLenAndLookupGet);
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppExMxPmExternalTcamActionIndexGet            ,appDemoExMxPmExternalTcamActionIndexGet            );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppExMxPmExternalTcamClientKeyLookupSet        ,appDemoExMxPmExternalTcamClientKeyLookupSet        );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppExMxPmExternalTcamClientKeyLookupGet        ,appDemoExMxPmExternalTcamClientKeyLookupGet        );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppExMxPmExternalTcamClientKeyLookupSetByIndex ,appDemoExMxPmExternalTcamClientKeyLookupSetByIndex );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppExMxPmExternalTcamClientKeyLookupGetByIndex ,appDemoExMxPmExternalTcamClientKeyLookupGetByIndex );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppExMxPmExternalTcamConfig                    ,appDemoExMxPmExternalTcamConfig                    );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppExMxPmHsuOldImagePreUpdatePreparation       ,appDemoExMxPmHsuOldImagePreUpdatePreparation       );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppExMxPmHsuNewImageUpdateAndSynch             ,appDemoExMxPmHsuNewImageUpdateAndSynch             );
#endif /*EXMXPM_FAMILY*/

#if (defined CHX_FAMILY) && (!defined EX_FAMILY)
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppLionPortModeSpeedSet,gtAppDemoLionPortModeSpeedSet);
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppXcat2StackPortsModeSpeedSet,gtAppDemoXcat2StackPortsModeSpeedSet);
#endif
    return GT_OK;
}

GT_STATUS appDemoDbPpDump(void)
{

    GT_U32 i; /* iterator*/

    for(i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        if(appDemoPpConfigList[i].valid)
        {
          osPrintf("Found device with devNum %d, device id 0x%8X, family %d, mng interface %d\n",
                   appDemoPpConfigList[i].devNum, appDemoPpConfigList[i].deviceId,
                   appDemoPpConfigList[i].devFamily, appDemoPpConfigList[i].channel);
          if(appDemoPpConfigList[i].channel==CPSS_CHANNEL_PCI_E ||
             appDemoPpConfigList[i].channel==CPSS_CHANNEL_PEX_E)
          {
              /* print PCI/PEX info */
              osPrintf("   Internal Regs BAR 0x%8X, PCI/PEX regs BAR 0x%8X\n",
                       appDemoPpConfigList[i].pciInfo.pciBaseAddr, appDemoPpConfigList[i].pciInfo.internalPciBase);
          }
          else if (appDemoPpConfigList[i].channel==CPSS_CHANNEL_SMI_E)
          {
              osPrintf("   SMI address %d\n",
                       appDemoPpConfigList[i].smiInfo.smiIdSel);

          }

          osPrintf(  "Init flags: ppPhase1Done %d, ppPhase2Done %d, ppLogicalInitDone %d, ppGeneralInitDone %d\n",
                     appDemoPpConfigList[i].ppPhase1Done, appDemoPpConfigList[i].ppPhase2Done,
                     appDemoPpConfigList[i].ppLogicalInitDone, appDemoPpConfigList[i].ppGeneralInitDone);

          osPrintf("   Interrupt line %d\n",
                   appDemoPpConfigList[i].intLine);

        }
    }


    return 0;
}


/******************************************************************************
* appDemoDebugDeviceIdSet
*
* DESCRIPTION:
*       Set given device ID value for specific device.
*       Should be called before cpssInitSystem().
*
* INPUTS:
*       devIndex - index of the device in the Application DB (0..127)
*       deviceId - device type
*
* OUTPUTS:
*       None.
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
extern GT_STATUS appDemoDebugDeviceIdSet
(
    IN GT_U8    devNum,
    IN GT_U32   deviceId
)
{
    if(devNum >= PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        return GT_OUT_OF_RANGE;
    }

    deviceIdDebug[devNum] = deviceId;

    useDebugDeviceId = GT_TRUE;

    return GT_OK;
}

/******************************************************************************
* appDemoDebugDeviceIdGet
*
* DESCRIPTION:
*       Get given device ID value for specific device.
*
* INPUTS:
*       devIndex - index of the device in the Application DB (0..127)
*
* OUTPUTS:
*       deviceIdPtr - pointer to device type
*
* RETURNS:
*       GT_OK      - on success
*       GT_NO_SUCH - on not set debug device ID
*       GT_OUT_OF_RANGE - on devIndex > 127
*
* COMMENTS:
*       None.
*
******************************************************************************/
extern GT_STATUS appDemoDebugDeviceIdGet
(
    IN  GT_U8    devNum,
    OUT GT_U32  *deviceIdPtr
)
{
    if(devNum >= PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        return GT_OUT_OF_RANGE;
    }

    if ((useDebugDeviceId == GT_FALSE) ||
        (0 == deviceIdDebug[devNum]))
    {
        return GT_NO_SUCH;
    }

    *deviceIdPtr = deviceIdDebug[devNum];

    return GT_OK;
}


