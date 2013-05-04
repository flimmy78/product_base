/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtDfcdl.c
*
* DESCRIPTION:
*       This file includes functions for Prestera DFCDL tests.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <dfcdl/private/gtDfcdlPrv.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
#include <cpssXbar/generic/xbarControl/gtXbar.h>
#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>
#include <cpss/exMx/exMxGen/diag/cpssExMxDiag.h>
#include <cpss/exMx/exMxGen/port/cpssExMxPortStat.h>
#include <cpss/exMx/exMxGen/port/cpssExMxPortCtrl.h>
#include <cpss/exMx/exMxGen/port/cpssExMxPortTx.h>
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgVlan.h>
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgFdb.h>
#include <cpss/exMx/exMxGen/cos/cpssExMxL2Cos.h>
#include <cpss/exMx/exMxGen/phy/cpssExMxPhySmi.h>
#include <cpss/exMx/exMxGen/networkIf/cpssExMxNetIf.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpssCommon/private/prvCpssMath.h>
#include <cpss/generic/events/cpssGenEventRequests.h>

#ifdef IMPL_FA
#include <cpssFaDriver/gtDriverFaDefs.h>
#include <cpssFaDriver/hardware/gtFaHwCntl.h>
#include <cpssFa/generic/gen/private/gtCoreFaStructs.h>
#include <cpssFa/generic/voq/gtVoq.h>
#endif

/******************************************************************************/

#define DFCDL_PKT_SEND_THREAD_PRIO 100
#define DFCDL_PKT_SEND_THREAD_STACK_SIZE 0x10000

/* This vlan number was piked arbitrary */
#define DFCDL_FIRST_USED_VLAN 0x10

#define BUFFER_DRAM_MIN_PKS_SIZE 64
#define BUFFER_DRAM_MAX_PKS_SIZE 1520

/* threshold for finding Rx hang point */
#define DFCDL_HANG_POINT_THRESHOLD  3

/* the length of descend segment in DQS window */
#define DFCDL_DQS_FALL_LENGTH 3

/* threshold for top line of DQS length */
#define TOP_DQS_THRESHOLD   2

#define DFCDL_MAX_DQS_LEN_RETRY 3

#define ENLARGE_WIN 6

#define BUFFER_DRAM_DEFAULT_PKT_PATT GT_PATTERN_TYPE_SSO
#define BUFFER_DRAM_DEFAULT_PKT_SIZE 1020
#define BUFFER_DRAM_DEFAULT_SLEEP 50

#define VLAN_PORT_BITMAP_SIZE   16
#define PP_DEV_BITMAP_SIZE      (PRV_CPSS_MAX_PP_DEVICES_CNS / 8)

/******************************************************************************/

static DFCDL_TUNE_DB dfcdlTuneDb;                  /* Local database         */
static GT_BOOL       bDataBaseInit    = GT_FALSE;  /* local db libInit flag  */
static GT_BOOL       localTrafficInit = GT_FALSE;  /* Indicates if the local
                                                      traffic was already
                                                      configured */
static  GT_U64    *resetCnt;                       /* used to reset counters */

/********* External parameters ************************************************/

#ifdef IMPL_FA
extern DRIVER_FA_CONFIG * driverFaDevs[];
extern CORE_FA_CONFIG   * coreFaDevs[];
#endif

/********* Public functions ***************************************************/
GT_VOID debugSetBestRxClk();
GT_VOID debugSetBestTxClk();

/* size of internal traffic packet for auto scan */
GT_U32 internalTrafficlen = 512;

/* vlan is common only to sequential ports */
GT_U32 portChain = GT_FALSE;

/* enable to set port without phy loopback */
GT_U32 externalPort = 0xffffffff;

/* for external traffic only - enable to use phy loop back */
GT_U32 useLoopBack = GT_FALSE;

/* indicates whether the last port and first port have common vlan -
   only for portChain = GT_TRUE */
GT_U32 lastToFirst = GT_FALSE;

/* the vlan of the packets send by the CPU */
GT_U32 cpuVlan = 0x10;

/* stop CPU traffic between each dfcdl scaning */
GT_U32 stopTraffic = GT_FALSE;

/* offset from Rx hang point in internal S.B. */
GT_U32     rxHangPointOffsInt = DFCDL_HANG_POINT_DIF_INT ;
/* offset from Rx hang point in external S.B. */
GT_U32     rxHangPointOffsExt  = DFCDL_HANG_POINT_DIF_EXT;

/* Tx complete event handle */
static GT_U32  evReqHndl = 0;
/********* Local functions ****************************************************/

static GT_STATUS dfcdlPpDramSet
(
    IN GT_U8    devNum,
    IN GT_U32   rxClkValue,
    IN GT_U8    dqsValue,
    IN GT_U8    txClkValue
);

static GT_STATUS dfcdlFaDramSet
(
    IN GT_U8    devNum,
    IN GT_U32   rxClkValue,
    IN GT_U8    dqsValue,
    IN GT_U8    txClkValue
);

static GT_STATUS dfcdlConfigPpForTuneExternal
(
    IN GT_U8 devNum
);

static GT_STATUS dfcdlDisablePPIntExternal
(
    GT_U8 devNum
);

static GT_STATUS dfcdlGetRxClkScanVal
(
    INOUT DFCDL_BUFFER_DRAM_SCAN_PARAMS   *scanValues
);

static GT_STATUS dfcdlSetSB
(
    IN GT_U8    devNum,
    IN GT_BOOL  extSB,             /* GT_TRUE - external S.B. */
    IN GT_DFCDL_DEVICE_TYPE devType
);

static GT_BOOL dfcdlRxClkDqsPassCriteria
(
    GT_VOID
);

static GT_U64  gtU64ShiftRight
(
    GT_U64  a,
    GT_U32  n
);

static GT_STATUS dfcdlGetPortMaxSpeed
(
    IN  GT_U8                 devNum,
    IN  GT_U8                 portNum,
    OUT CPSS_PORT_SPEED_ENT * maxPortSpeed
);

static GT_STATUS dfcdlAddAllPort2Vlans
(
    IN  GT_U8 devNum,
    IN  GT_BOOL add
);

static GT_STATUS dfcdlInitLocalDb
(
    IN GT_BOOL  init
);

static GT_STATUS dfcdlConfigPpForTune
(
    IN GT_U8        devNum
);

static GT_STATUS dfcdlConfigPortMac
(
    GT_U8 devNum
);

static GT_STATUS dfcdlConfigPortPhyConfig
(
    GT_U8   devNum
);

static GT_STATUS dfcdlDisablePPInt
(
    GT_U8   devNum
);

static GT_STATUS dfcdlConfigFaForTune
(
    IN GT_U8 devNum
);

static GT_STATUS dfcdlDramRangeScan
(
    IN GT_U8                           devNum,
    IN DFCDL_BUFFER_DRAM_SCAN_PARAMS * scanParams,
    IN  GT_DFCDL_DEVICE_TYPE           devType
);

static GT_STATUS dfcdlBuildSSOPacket
(
    IN  GT_U32  bufferLen,
    OUT GT_U8   *buffer,
    OUT GT_U8   *invBuffer
);

static GT_STATUS dfcdlBuildSS32Packet
(
    IN  GT_U32  bufferLen,
    OUT GT_U8   *buffer,
    OUT GT_U8   *invBuffer
);

static GT_STATUS dfcdlBuildAA55Packet
(
    IN  GT_U32  bufferLen,
    OUT GT_U8 * buffer,
    OUT GT_U8 * invBuffer
);

static GT_STATUS dfcdlBuildRandomPacket
(
    IN  GT_U32  bufferLen,
    OUT GT_U8 * buffer,
    OUT GT_U8 * invBuffer
);

static GT_STATUS dfcdlBuildZeroPacket
(
    IN  GT_U32  bufferLen,
    OUT GT_U8   *buffer,
    OUT GT_U8   *invBuffer
);

static GT_STATUS dfcdlBuildPacket
(
    IN GT_PCK_PATTERN_TYPE  pattType,
    IN  GT_U32  pcktLength
);

static GT_STATUS dfcdlAddGoodTxClkEntry
(
    IN GT_U8 txClkValue
);

static GT_STATUS dfcdlAddGoodRxClkEntry
(
    IN GT_U8 rxClkValue,
    IN GT_U8 dqsValue
);

static GT_STATUS dfcdlResetCounter
(
    IN  GT_U8    devNum,
    IN  GT_U64   *portCrcCounter,
    IN  GT_U64   *portPktCounter
);

static GT_STATUS dfcdlReadCounters
(
    IN      GT_U8                           devNum,
    IN      GT_U64                        * portCrcCounter,
    IN      GT_U64                        * portPktCounter,
    IN      DFCDL_BUFFER_DRAM_TUNE_PARAMS * tuneParams,
    OUT     GT_BOOL                       * foundGoodEntry
);

static GT_STATUS dfcdlGetBestRxClkDqsValues
(
    IN  DFCDL_BUFFER_DRAM_SCAN_PARAMS   *scanParams
);

static GT_STATUS dfcdlGetBestTxClkValues
(
    GT_VOID
);

static GT_STATUS dfcdlDramFindPtr
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  delay,
    IN  GT_DFCDL_DEVICE_TYPE    devType,
    OUT GT_U8                   *entryPtr

);

static GT_STATUS dfcdlDramCalcSetPpTable
(
    IN  GT_U8   devNum,
    IN  GT_U8   entryPtr
);

static GT_STATUS dfcdlDramCalcSetFaTable
(
    IN  GT_U8   devNum,
    IN  GT_U8   entryPtr
);

static GT_STATUS dfcdlRound
(
    IN  float    value,
    OUT GT_U8    *res
);

static GT_STATUS gtDfcdlAutoScan
(
    IN GT_U8                            devNum,
    IN DFCDL_BUFFER_DRAM_SCAN_PARAMS    *scanParams,
    IN GT_DFCDL_DEVICE_TYPE             devType
);

static GT_STATUS writeVlanEntry
(
    IN GT_U16          vid,
    IN GT_U8           devNum,
    IN GT_U8           portInfoList[VLAN_PORT_BITMAP_SIZE],
    CPSS_VLAN_INFO_STC cpssVlanInfo,
    IN GT_U8           devBitmap[PP_DEV_BITMAP_SIZE]
);

/* update those parameter to define */
GT_U32 dqsPassCriteria = 6;
GT_U32 rxClkPassCriteria = 7;
GT_U32 dqsThreshold = 2;
GT_U32 stepBack = 1;

/*******************************************************************************
* gtDfcdlScanConfig
*
* DESCRIPTION:
*       Config the DFCDL scan parameters and initilze the library.
*
* INPUTS:
*       devNum          - The device number.
*       firstPortNum    - The first port number in the configured ports.
*       lastPortNum     - The last port number in the configured ports.
*       extTraffic      - GT_TRUE - External traffic generator is used to
*                                   generate the traffic.
*                         GT_FALSE - The traffic is generated by the CPU.
*       init            - GT_TRUE -  Initilaize the library.
*                         GT_FALSE - Close the library.
*       externalSB      - GT_TRUE - use external S.B. as default.
*       internalSB      - GT_TRUE - use external S.B. as default.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS gtDfcdlScanConfig
(
    IN GT_U8    devNum,
    IN GT_U8    firstPortNum,
    IN GT_U8    lastPortNum,
    IN GT_BOOL  extTraffic,
    IN GT_BOOL  init,
    IN GT_BOOL  externalSB
)
{
    GT_STATUS rc;       /* Return code */

    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);
    if (bDataBaseInit == init)
    {
        return (GT_INIT_ERROR);
    }

    /* Initilaize the database */
    rc = dfcdlInitLocalDb(init);
    RETURN_ON_ERROR(rc);

    /* Store the parameters in the database */
    if ( init == GT_TRUE )
    {
        dfcdlTuneDb.firstPortNum = firstPortNum;
        dfcdlTuneDb.lastPortNum  = lastPortNum;
        dfcdlTuneDb.extTraffic   = extTraffic;
        dfcdlTuneDb.extSB        = externalSB;

        if (extTraffic == GT_TRUE)
        {
            rc = dfcdlConfigPpForTuneExternal(devNum);
            RETURN_ON_ERROR(rc);
        }
    }

    bDataBaseInit = init;

    return GT_OK;
} /* gtDfcdlScanConfig */

/*******************************************************************************
* gtDfcdlConfigInternalTraffic
*
* DESCRIPTION:
*   Configure the internal traffic parameters.
*
* INPUTS:
*       devNum     - Device number.
*       pattType   - Pattern type.
*       pcktLength - The packet size
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_BAD_PARAM       - if packet length > 1520
*       GT_NOT_INITIALIZED - if lib not initialized
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS gtDfcdlConfigInternalTraffic
(
    IN GT_U8                devNum,
    IN GT_PCK_PATTERN_TYPE  pattType,
    IN GT_U32               pcktLength
)
{
    GT_STATUS      rc;         /* Return code                   */

    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);

    if ( pcktLength > BUFFER_DRAM_MAX_PKS_SIZE )
    {
        return GT_BAD_PARAM;
    }

    if ( bDataBaseInit == GT_FALSE )
    {
        dfcdlPrint("**** Database was not initialized.\n");
        return GT_NOT_INITIALIZED;
    }

    dfcdlTuneDb.intTraff.pattType   = pattType;
    dfcdlTuneDb.intTraff.pcktLength = pcktLength;
    /* Allocate memory for the buffers */
    dfcdlTuneDb.intTraff.buffer[0] = (GT_U8 *)
                                     cpssOsCacheDmaMalloc(pcktLength + 4 + 12);
    dfcdlTuneDb.intTraff.invBuffer[0] = (GT_U8 *)
                                     cpssOsCacheDmaMalloc(pcktLength+ 4 + 12);

    if ( (dfcdlTuneDb.intTraff.buffer[0] == NULL ) ||
         (dfcdlTuneDb.intTraff.invBuffer[0] == NULL))
    {
        dfcdlDebugPrint("allocation failed\n");
        return GT_OUT_OF_CPU_MEM;
    }

    /* Building the buffer according to the pattern type */
    rc = dfcdlBuildPacket(pattType, dfcdlTuneDb.intTraff.pcktLength);
    RETURN_ON_ERROR(rc);

    dfcdlTuneDb.intTraff.txParams.txParams.userPrioTag    = 0;
    dfcdlTuneDb.intTraff.txParams.txParams.tagged         = GT_FALSE;
    dfcdlTuneDb.intTraff.txParams.txParams.packetEncap    =
                                                   CPSS_EXMX_NET_REGULAR_PCKT_E;
    dfcdlTuneDb.intTraff.txParams.txParams.dropPrecedence = 0;
    dfcdlTuneDb.intTraff.txParams.txParams.recalcCrc      = GT_TRUE;
    dfcdlTuneDb.intTraff.txParams.txParams.vid            =
                               DFCDL_FIRST_USED_VLAN + dfcdlTuneDb.firstPortNum;
    dfcdlTuneDb.intTraff.txParams.txParams.macDaType      =
                                                    CPSS_EXMX_NET_UNICAST_MAC_E;
    dfcdlTuneDb.intTraff.txParams.txParams.txQueue        = 1;
    dfcdlTuneDb.intTraff.txParams.txParams.cookie         = (GT_VOID *)NULL;
    dfcdlTuneDb.intTraff.txParams.useVidx                 = GT_TRUE;
    dfcdlTuneDb.intTraff.txParams.dest.vidx               =
                                   (GT_U16)(cpuVlan + dfcdlTuneDb.firstPortNum);
    dfcdlTuneDb.intTraff.txParams.addDsa                  = GT_FALSE;

    if (0 == evReqHndl)
    {
        /* this is the first time, create TxBufferQueue Hhandle */
        CPSS_UNI_EV_CAUSE_ENT     evCause[1] = { CPSS_PP_TX_BUFFER_QUEUE_E };

        rc = cpssEventBind(evCause, 1, &evReqHndl);
        RETURN_ON_ERROR(rc);

        rc = cpssEventDeviceMaskSet(devNum, CPSS_PP_TX_BUFFER_QUEUE_E,
                                    CPSS_EVENT_UNMASK_E);
        RETURN_ON_ERROR(rc);
    }

    /* save the TxBufferQueue handle in the TX parameters */
    dfcdlTuneDb.intTraff.txParams.txParams.evReqHndl = evReqHndl;

    localTrafficInit = GT_TRUE;

    return GT_OK;
} /* gtDfcdlConfigInternalTraffic */

/*******************************************************************************
* dfcdlDramGetBoardParams
*
* DESCRIPTION:
*       Finds the device's Rx hang point.
*
* INPUTS:
*       devNum    - Device number.
*       *scanParams - Dfcdl scan params.
*       devType     - Device type (PP or FA).
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlDramGetBoardParams
(
    IN GT_U8                            devNum,
    IN DFCDL_BUFFER_DRAM_SCAN_PARAMS    *scanParams,
    IN GT_DFCDL_DEVICE_TYPE             devType
)
{
    GT_STATUS   rc = GT_OK;         /* Return code                      */
    GT_U8       rxClkCounter;       /* Loop counters                    */
    GT_U8       txClkCounter;       /* Loop counters                    */
    GT_U8       dqsCounter;         /* Loop counters                    */
    GT_BOOL     rxClkIncrement;     /* Increment or decrement the count */
    GT_U8       rxClkLoEdge;        /* RxClk lo edge number             */
    GT_U8       rxClkHiEdge;        /* RxClk high edge number           */
    GT_BOOL     skipFirst;          /* skip the first area of CRCs      */
    GT_U32      rxHangPoint;
    GT_BOOL     testStatus;
    GT_U32      addr;
    GT_U32      readVal, writeVal;
    GT_BOOL     isFound;            /* GT_TRUE - hang point is found    */
    GT_U32      vSize;              /* size of found CRC vector         */


    dfcdlPrint("Scanning for RxClk hang point...\n");
    if(devType == GT_DFCDL_PP)
    {
        /* configure register, set bits 17, 18 */
        rc = cpssDrvPpHwRegFieldSet(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                    PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->
                                    extMemRegs.bdTiming4Reg, 17, 2, 3);
    }
    else if(devType == GT_DFCDL_FA)
    {
#ifdef IMPL_FA
        /* configure register, set bits 17, 18 */
        rc = cpssDrvPpHwRegFieldSet(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                    coreFaDevs[devNum]->regsAddr->
                                    extMemRegs.bdTiming4Reg, 17, 2, 3);
#else
        return GT_FAIL;
#endif
    }

    RETURN_ON_ERROR(rc);

    if ((scanParams->rxClkEnd) > (scanParams->rxClkStart))
    {
        /* increment must be only for External S.B */
        if ((GT_FALSE == dfcdlTuneDb.extSB) &&
            (scanParams->rxClkEnd != scanParams->rxClkStart))
        {
            return GT_BAD_PARAM;
        }

        rxClkIncrement = GT_FALSE;
        rxClkLoEdge = scanParams->rxClkStart;
        rxClkHiEdge = scanParams->rxClkEnd;
        rxHangPoint = DFCDL_MIN_DELAY;
    }
    else
    {
        /* decrement must be only for Internal S.B */
        if ((GT_TRUE == dfcdlTuneDb.extSB) &&
            (scanParams->rxClkEnd != scanParams->rxClkStart))
        {
            return GT_BAD_PARAM;
        }

        rxClkIncrement = GT_TRUE;
        rxClkLoEdge = scanParams->rxClkEnd;
        rxClkHiEdge = scanParams->rxClkStart;
        rxHangPoint = DFCDL_MAX_DELAY - 1;
    }


    txClkCounter = (scanParams->txClkStart + scanParams->txClkEnd) >> 1;
    dqsCounter = (scanParams->dqsStart + scanParams->dqsEnd) >> 1;
    rxClkCounter = scanParams->rxClkEnd;
    skipFirst = GT_TRUE;
    isFound = GT_FALSE;
    vSize = 0;
    /* For all the range of the rxClk */
    do
    {
        if (devType == GT_DFCDL_PP)
        {
            /* Program the DFCDL table with the good values */
            rc = dfcdlPpDramSet(devNum, rxClkCounter, dqsCounter, txClkCounter);
        }
        else if (devType == GT_DFCDL_FA)
        {
            /* Program the DFCDL table with the good values */
            rc = dfcdlFaDramSet(devNum, rxClkCounter, dqsCounter, txClkCounter);
        }

        RETURN_ON_ERROR(rc);

        /* Give a delay before reseting counters to make sure that    */
        /* the DFCDL is set                                           */
        cpssOsTimerWkAfter(10);

        /* run diagnostic */
        if(devType == GT_DFCDL_PP)
        {
            rc = cpssExMxDiagMemTest (devNum, CPSS_DIAG_PP_MEM_BUFFER_DRAM_E, 0,
                                      100, CPSS_DIAG_TEST_BIT_TOGGLE_E,
                                      &testStatus, &addr, &readVal, &writeVal);
            if (testStatus == GT_FALSE)
            {
                rc = GT_FAIL;
            }
        }
        else if (devType == GT_DFCDL_FA)
        {
#ifdef IMPL_FA
            rc = diagTestFaMem (devNum, GT_FA_BUFFER_DRAM, 0, 100,
                                GT_BIT_TOGGLE, &addr, &readVal, &writeVal);
#else
            return GT_FAIL;
#endif
        }
        if(rc != GT_OK)
        {
            dfcdlPrint("RxClk = 0x%x Failed\n",  rxClkCounter);

            if(skipFirst != GT_TRUE)
            {/* that's Rx hang point */
                vSize++; /* increment size of found CRC vector */
                if(vSize >= DFCDL_HANG_POINT_THRESHOLD)
                {
                    isFound = GT_TRUE;
                    if(GT_TRUE == dfcdlTuneDb.extSB)
                    {
                        rxHangPoint = rxClkCounter + DFCDL_HANG_POINT_THRESHOLD;
                        if(devType == GT_DFCDL_PP)
                        {
                            rc = dfcdlPpDramSet(devNum,
                                                rxHangPoint + rxHangPointOffsExt,
                                                dqsCounter,txClkCounter);
                        }
                        else if (devType == GT_DFCDL_FA)
                        {
                            /* Program the DFCDL table with the good values */
                            rc = dfcdlFaDramSet(devNum,
                                                rxHangPoint + rxHangPointOffsExt,
                                                dqsCounter,txClkCounter);
                        }

                    }
                    else
                    {
                        rxHangPoint = rxClkCounter - DFCDL_HANG_POINT_THRESHOLD;
                        if(devType == GT_DFCDL_PP)
                        {
                            rc = dfcdlPpDramSet(devNum,
                                                rxHangPoint - rxHangPointOffsExt,
                                                dqsCounter,txClkCounter);
                        }
                        else if (devType == GT_DFCDL_FA)
                        {
                            /* Program the DFCDL table with the good values */
                            rc = dfcdlFaDramSet(devNum,
                                                rxHangPoint - rxHangPointOffsExt,
                                                dqsCounter,txClkCounter);
                        }
                    }

                    RETURN_ON_ERROR(rc);

                    cpssOsPrintf("Rx hang point = %d\n", rxHangPoint);

                    break;
                }
            }
        }
        else
        {/* that's area of good Rx following after CRC points */
            dfcdlPrint("RxClk = 0x%x Ok\n",  rxClkCounter);

            skipFirst = GT_FALSE;
            if(vSize != 0)
            {
                vSize = 0; /* reset CRC vector size */
            }
        }

        if (rxClkIncrement == GT_TRUE)
        {
            rxClkCounter++;
        }
        else
        {
            rxClkCounter--;
        }

    }while( (rxClkCounter >= rxClkLoEdge) &&
            (rxClkCounter <= rxClkHiEdge) );



    if(isFound == GT_FALSE)
    {
        if(GT_TRUE == dfcdlTuneDb.extSB)
        {
            if(vSize != 0)
            {
                rxHangPoint += vSize;
            }
            else
            {
                cpssOsPrintf("External S.B. Rx hang point is beyond %d\n",
                             rxHangPoint);
            }
        }
        else
        {
            if(vSize != 0)
            {
                rxHangPoint -= vSize;
            }
            else
            {
                cpssOsPrintf("Internal S.B. Rx hang point is above %d\n",
                             rxHangPoint);
            }
        }
    }
    /* set hang point */
    dfcdlTuneDb.externalSBHangPoint = rxHangPoint;
    dfcdlTuneDb.internalSBHangPoint = rxHangPoint;
    return GT_OK;
} /* dfcdlDramGetBoardParams */

/*******************************************************************************
* gtDfcdlPpAutoScan
*
* DESCRIPTION:
*   This function automatically find and set the configuration for the PP DFCDL.
*   The function stores the DFCDLs in the SRAM.
*
* INPUTS:
*       devNum          - The device number to do the scaning.
*       scanParams      - The scaning parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS gtDfcdlPpAutoScan
(
    IN GT_U8                            devNum,
    IN DFCDL_BUFFER_DRAM_SCAN_PARAMS    *scanParams
)
{
    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);
    return gtDfcdlAutoScan(devNum,scanParams,GT_DFCDL_PP);
}

/*******************************************************************************
* gtDfcdlFaAutoScan
*
* DESCRIPTION:
*   This function automatically find and set the configuration for the Fa DFCDL.
*   The function stores the DFCDLs in the SRAM.
*
* INPUTS:
*       devNum          - The device number to do the scaning.
*       scanParams      - The scaning parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS gtDfcdlFaAutoScan
(
    IN GT_U8                            devNum,
    IN DFCDL_BUFFER_DRAM_SCAN_PARAMS    *scanParams
)
{
#ifdef IMPL_FA
    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);
    return gtDfcdlAutoScan(devNum,scanParams,GT_DFCDL_FA);
#else
    return GT_FAIL;
#endif
}

/*******************************************************************************
* gtDfcdlAutoScan
*
* DESCRIPTION:
*   This function automatically find and set the configuration for the PP DFCDL
*   or FA DFCDL.
*   The function stores the DFCDLs in the SRAM.
*
* INPUTS:
*       devNum        - The device number to do the scaning.
*       scanParams    - The scaning parameters.
*       devType       - Device type (PP or FA)
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS gtDfcdlAutoScan
(
    IN GT_U8                           devNum,
    IN DFCDL_BUFFER_DRAM_SCAN_PARAMS * scanParams,
    IN GT_DFCDL_DEVICE_TYPE            devType
)
{
    GT_STATUS                     rc;              /* Return code        */
    DFCDL_BUFFER_DRAM_SCAN_PARAMS localScanValues; /* Local scan values  */
    GT_BOOL                       extSB;           /* Init S.B mode      */

    if ( GT_FALSE == bDataBaseInit )
    {
        dfcdlPrint("**** Database was not initialized.\n");
        return GT_NOT_INITIALIZED;
    }

    /* Validity check */
    if (scanParams == NULL)
    {
        return GT_BAD_PTR;
    }

    if ( (GT_FALSE == dfcdlTuneDb.extTraffic) &&
         (GT_FALSE == localTrafficInit) )
    {
        dfcdlPrint("\nThe interal traffic generation was not initilaized\n");
        return GT_NOT_INITIALIZED;
    }

    if(dqsPassCriteria < scanParams->dqsThreshold)
    {
        cpssOsPrintf("ERROR: Dqs pass criteria should be greater or equal to Dqs threshold\n");
        return GT_BAD_PARAM;
    }

    /* check if we start in internal mode */
    if (GT_FALSE == dfcdlTuneDb.extSB)
    {
        /* set internal S.B */
        rc = dfcdlSetSB(devNum, GT_FALSE, devType);
        RETURN_ON_ERROR(rc);

        cpssOsPrintf("Internal S.B.\n");
        extSB = dfcdlTuneDb.extSB;
    }
    else
    {
        /* set external S.B. */
        rc = dfcdlSetSB(devNum, GT_TRUE, devType);
        RETURN_ON_ERROR(rc);

        cpssOsPrintf("External S.B.\n");
        extSB = dfcdlTuneDb.extSB;
    }

    /* Find the rxClk hang point */
    rc = dfcdlDramGetBoardParams(devNum, scanParams, devType);
    if (rc != GT_OK)
    {
        cpssOsPrintf("\nError finding Hang point");
        return (rc);
    }

    /* Only in internal traffic the configuration is done */
    if ( GT_FALSE == dfcdlTuneDb.extTraffic )
    {
        /* Configure the PP for the tuning */
        rc = dfcdlConfigPpForTune(devNum);
        RETURN_ON_ERROR(rc);

        if ( GT_DFCDL_FA == devType )
        {
            /* Configure the Fa for the tuning */
            rc = dfcdlConfigFaForTune(devNum);
            RETURN_ON_ERROR(rc);
        }

        /* Start sending the packets (spawn a task) */
        rc = dfcdlSendInternalPackets(devNum, GT_TRUE);
        RETURN_ON_ERROR(rc);

        cpssOsTimerWkAfter(10);
    }
    /* Clean the DB */
    cpssOsMemSet(&(dfcdlTuneDb.bestEntry), 0, sizeof(dfcdlTuneDb.bestEntry));

    cpssOsMemSet(dfcdlTuneDb.goodRxClkValues,0,
                 sizeof(dfcdlTuneDb.goodRxClkValues) );

    cpssOsMemSet(&(dfcdlTuneDb.goodTxClkValues),0,
                 sizeof(dfcdlTuneDb.goodTxClkValues) );

    dfcdlTuneDb.validTxEntries   = 0;
    dfcdlTuneDb.startQueueTxClkValues = GT_FALSE;

    /* Scanning the valid entries of RxClk, DQS with const TxClk */
    cpssOsMemCpy(&localScanValues, scanParams, sizeof(localScanValues));

    localScanValues.txClkEnd = scanParams->txClkStart + (
                          (scanParams->txClkEnd - scanParams->txClkStart) / 2);

    localScanValues.txClkStart = localScanValues.txClkEnd;

    dfcdlDebugPrint("\n---> TxClock: %d\n", localScanValues.txClkEnd);

    /* calculate rx parameters */
    rc = dfcdlGetRxClkScanVal(&localScanValues);
    if (rc != GT_OK)
    {
        dfcdlSendInternalPackets(devNum,GT_FALSE);
        return (rc);
    }

    cpssOsPrintf("\nScaning for rxClk txClk and dqs values\n");
    rc = dfcdlDramRangeScan(devNum,&localScanValues,devType);

    if (rc != GT_OK)
    {
        /* Stop sending the packets */
        dfcdlSendInternalPackets(devNum,GT_FALSE);
        return (rc);
    }

    /* Get the best RxClk and DQS values */
    rc = dfcdlGetBestRxClkDqsValues(&localScanValues);
    if (rc != GT_OK)
    {
        /* Stop sending the packets */
        dfcdlSendInternalPackets(devNum,GT_FALSE);
        return (rc);
    }

    /* check if scan results comply with the pass criteria */
    if (GT_FALSE == dfcdlRxClkDqsPassCriteria())
    {
        /* failed pass criteria */
        if (GT_FALSE == dfcdlTuneDb.extSB)
        {
            /* internal S.B failed */
            dfcdlPrint("Internal S.B. pass criteria failed\n");
            dfcdlSendInternalPackets(devNum,GT_FALSE);
            return GT_FAIL;
        }

        /* try to change S.B to internal */
        dfcdlPrint("\nSecond scanning for Internal S.B\n");

        /* Stop sending the packets */
        rc = dfcdlSendInternalPackets(devNum,GT_FALSE);
        RETURN_ON_ERROR(rc);

        cpssOsTimerWkAfter(10);

        /*  set internal S.B */
        rc = dfcdlSetSB(devNum, GT_FALSE, devType);
        RETURN_ON_ERROR(rc);

        /* calculate rx parameters - from end to start */
        localScanValues.rxClkStart = scanParams->rxClkEnd;
        localScanValues.rxClkEnd = scanParams->rxClkStart;
        /* Find the rxClk hang point */
        rc = dfcdlDramGetBoardParams(devNum, &localScanValues, devType);
        if (rc != GT_OK)
        {
            dfcdlPrint("\nError finding Hang point");
            /* restore init S.B. mode */
            dfcdlSetSB(devNum,extSB, devType);
            return (rc);
        }

        if (dfcdlTuneDb.extTraffic == GT_FALSE)
        {
            /* Configure the PP for the tuning */
            rc = dfcdlConfigPpForTune(devNum);
            if (rc != GT_OK)
            {
                /* restore init S.B. mode */
                dfcdlSetSB(devNum,extSB, devType);
                return (rc);
            }

            if(devType == GT_DFCDL_FA)
            {
                /* Configure the Fa for the tuning */
                rc = dfcdlConfigFaForTune(devNum);
                if (rc != GT_OK)
                {
                    /* restore init S.B. mode */
                    dfcdlSetSB(devNum,extSB, devType);
                    return (rc);
                }
            }

            /* Start sending the packets */
            rc = dfcdlSendInternalPackets(devNum,GT_TRUE);
            if (rc != GT_OK)
            {
                /* restore init S.B. mode */
                dfcdlSetSB(devNum,extSB, devType);
                return (rc);
            }

            cpssOsTimerWkAfter(10);
        }

        /* Clear the counter */
        cpssOsMemSet(&(dfcdlTuneDb.bestEntry),0,sizeof(dfcdlTuneDb.bestEntry) );
        cpssOsMemSet(dfcdlTuneDb.goodRxClkValues,0,
                     sizeof(dfcdlTuneDb.goodRxClkValues) );

        rc = dfcdlGetRxClkScanVal(&localScanValues);
        if (rc != GT_OK)
        {
            dfcdlSendInternalPackets(devNum,GT_FALSE);
            /* restore init S.B. mode */
            dfcdlSetSB(devNum,extSB, devType);
            return (rc);
        }
        /* try to scan again */
        rc = dfcdlDramRangeScan(devNum,&localScanValues,devType);
        if (rc != GT_OK)
        {
            /* Stop sending the packets */
            dfcdlSendInternalPackets(devNum,GT_FALSE);
            /* restore init S.B. mode */
            dfcdlSetSB(devNum,extSB, devType);
            return (rc);
        }

        /* Get the best RxClk and DQS values */
        rc = dfcdlGetBestRxClkDqsValues(&localScanValues);
        if (rc != GT_OK)
        {
            /* Stop sending the packets */
            dfcdlSendInternalPackets(devNum,GT_FALSE);
            /* restore init S.B. mode */
            dfcdlSetSB(devNum,GT_TRUE, devType);
            return (rc);
        }

        /* check if second scan results comply with the pass criteria */
        if (GT_FALSE == dfcdlRxClkDqsPassCriteria())
        {
            /* Stop sending the packets */
            dfcdlSendInternalPackets(devNum,GT_FALSE);

            /* internal S.B failed */
            cpssOsPrintf("Internal S.B. pass criteria failed\n");
            /* restore init S.B. mode */
            dfcdlSetSB(devNum,extSB, devType);
            return GT_FAIL;
        }
    }

    /* Scanning the valid entries of TxClk with const RxClk,DQS */
    cpssOsMemCpy(&localScanValues,scanParams,sizeof(localScanValues));
    localScanValues.dqsEnd     = dfcdlTuneDb.bestEntry.dqs;
    localScanValues.dqsStart   = dfcdlTuneDb.bestEntry.dqs;
    localScanValues.rxClkEnd   = dfcdlTuneDb.bestEntry.rxClk;
    localScanValues.rxClkStart = dfcdlTuneDb.bestEntry.rxClk;
    dfcdlTuneDb.startQueueTxClkValues = GT_TRUE;

    rc = dfcdlDramRangeScan(devNum,&localScanValues,devType);
    if (rc != GT_OK)
    {
        /* Stop sending the packets */
        dfcdlSendInternalPackets(devNum,GT_FALSE);
        /* restore init S.B. mode */
        dfcdlSetSB(devNum,extSB, devType);
        return (rc);
    }

    /* Get the best TxClk values */
    rc = dfcdlGetBestTxClkValues();
    if (rc != GT_OK)
    {
        /* Stop sending the packets */
        dfcdlSendInternalPackets(devNum,GT_FALSE);
        /* restore init S.B. mode */
        dfcdlSetSB(devNum,extSB, devType);
        return (rc);
    }

    /* Find the entry point of the currrent DFCDL entry */
    rc = dfcdlDramFindPtr(devNum,scanParams->testDelay,devType,
                          &dfcdlTuneDb.activeEntry);
    if (rc != GT_OK)
    {
        cpssOsPrintf("\nError finding PVT entry");
        /* Stop sending the packets */
        dfcdlSendInternalPackets(devNum,GT_FALSE);
        /* restore init S.B. mode */
        dfcdlSetSB(devNum,extSB, devType);
        return (rc);
    }


    if (dfcdlTuneDb.extTraffic == GT_FALSE)
    {
        /* Stop sending packets */
        rc = dfcdlSendInternalPackets(devNum, GT_FALSE);
        if (rc != GT_OK)
        {
            /* restore init S.B. mode */
            dfcdlSetSB(devNum,extSB, devType);
            return (rc);
        }

        /* Delete the VLANs to stop the traffic */
        rc = dfcdlAddAllPort2Vlans(devNum, GT_FALSE);
        if (rc != GT_OK)
        {
            /* restore init S.B. mode */
            dfcdlSetSB(devNum,extSB,devType);
            return (rc);
        }
    }

    /* Calculate the set the DFCDL table to the SRAM */
    if(devType == GT_DFCDL_PP)
    {
        rc = dfcdlDramCalcSetPpTable(devNum,dfcdlTuneDb.activeEntry);
    }
    else if(devType == GT_DFCDL_FA)
    {
        rc = dfcdlDramCalcSetFaTable(devNum,dfcdlTuneDb.activeEntry);

    }

    if (rc != GT_OK)
    {
        cpssOsPrintf("\nError calculating DFCDL table");
        /* restore init S.B. mode */
        dfcdlSetSB(devNum,extSB,devType);
        return (rc);
    }

    /* print window */
    rc = gtDfcdlPrintWindows();
    if (rc != GT_OK)
    {
        /* restore init S.B. mode */
        dfcdlSetSB(devNum,extSB,devType);
        return (rc);
    }

    if (devType == GT_DFCDL_PP)
    {
        cpssOsPrintf("\nFinished the scan on PP\n");
    }
    else
    {
        cpssOsPrintf("\nFinished the scan on FA\n");
    }

    /* restore init S.B. mode */
    dfcdlSetSB(devNum,extSB,devType);

    return GT_OK;
} /* gtDfcdlAutoScan */

/*******************************************************************************
* gtDfcdlGetValidTxClkValues
*
* DESCRIPTION:
*   The function returns the valid TxClk values found in the scan. The function
*   also returns the RxClk and DQS values for the TxClk entry.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       txClkValidValues - Arrays of valid txClk values.
*       arrayLen         - The number of txClk values in the array.
*
* RETURNS :
*
*
*
* COMMENTS:
*       The memory for the array should be allocated before calling the
*       function. The values that will be returned are the last values of the
*       scan. If PP auto scan was invoked then the values will be the PP TxClk
*       values. If the FA auto scan was invoked then the values will be the FA
*       TxClk values.
*
*******************************************************************************/
GT_STATUS gtDfcdlGetValidTxClkValues
(
    OUT GT_U8   txClkValidValues[DFCDL_MAX_DELAY],
    OUT GT_U8   *arrayLen
)
{
    if (bDataBaseInit == GT_FALSE)
    {
        return (GT_NOT_INITIALIZED);
    }

    /* Validty check */
    if ((arrayLen == NULL) || (txClkValidValues == NULL))
    {
        return (GT_BAD_PARAM);
    }

    if ((*arrayLen) < dfcdlTuneDb.validTxEntries)
    {
        return (GT_BAD_SIZE);
    }

    cpssOsMemCpy(txClkValidValues,dfcdlTuneDb.goodTxClkValues,
                 dfcdlTuneDb.validTxEntries);

    *arrayLen = dfcdlTuneDb.validTxEntries;

    return (GT_OK);
}

/*******************************************************************************
* gtDfcdlPrintWindows
*
* DESCRIPTION:
*   The function prints the valid TxClk  and rxClk DQS windows.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*
*
*
* COMMENTS:
*   The values that will be printed are the last values of the
*   scan. If PP auto scan was invoked then the values will be the PP TxClk
*   values. If the FA auto scan was invoked then the values will be the FA
*   TxClk values.
*
*******************************************************************************/
GT_STATUS gtDfcdlPrintWindows
(
    GT_VOID
)
{
    GT_U8   dqsMin, dqsMax; /* minimal, maximal dqs value */
    GT_U8   dqsLen;
    GT_U8   dqsFirst, dqsLast;
    GT_U8   i;              /* iterator */

    dqsMin = 255;
    dqsMax = 0;
    for(i = dfcdlTuneDb.bestEntry.rxClk - 1;
        i <= dfcdlTuneDb.bestEntry.rxClk + 1; i++)
    {
        dqsLen = dfcdlTuneDb.goodRxClkValues[i].dqsValuesLength;
        if(dqsLen == 0)
        {
            continue;
        }
        dqsFirst = dfcdlTuneDb.goodRxClkValues[i].dqsValues[0];
        dqsLast = dfcdlTuneDb.goodRxClkValues[i].dqsValues[dqsLen - 1];

        /* get minimum dqs */
        if(dqsFirst < dqsMin)
        {
            dqsMin = dqsFirst;
        }
        /* get maximum dqs */
        if(dqsLast > dqsMax)
        {
            dqsMax = dqsLast;
        }

    }

    dfcdlPrint("- Dqs window size = %d (pass criteria = %d)\n", dqsMax - dqsMin + 1,
             dqsPassCriteria);
    dfcdlPrint("- RxClk window size = %d (pass criteria = %d)\n",dfcdlTuneDb.maxRx,
             rxClkPassCriteria);

    gtDfcdlPrintValidRxClkValues();
    gtDfcdlPrintValidTxClkValues();

    return (GT_OK);
}

/*******************************************************************************
* gtDfcdlPrintValidTxClkValues
*
* DESCRIPTION:
*   The function prints the valid TxClk values found in the scan. The function
*   also prints the RxClk and DQS values for the TxClk entry.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*
*
*
* COMMENTS:
*   The values that will be printed are the last values of the
*   scan. If PP auto scan was invoked then the values will be the PP TxClk
*   values. If the FA auto scan was invoked then the values will be the FA
*   TxClk values.
*
*******************************************************************************/
GT_STATUS gtDfcdlPrintValidTxClkValues
(
    GT_VOID
)
{
    GT_U8   i;  /* loop counter */

    if (bDataBaseInit == GT_FALSE)
    {
        return (GT_NOT_INITIALIZED);
    }

    dfcdlPrint("\n\nValid TxClk entries.\n");
    dfcdlPrint("====================\n");
    dfcdlPrint("rxClk:0x%02x  TxClk values :",dfcdlTuneDb.bestEntry.rxClk);

    /* For all the valid entries */
    for (i = 0 ; i < dfcdlTuneDb.validTxEntries ; i++)
    {
        dfcdlPrint("0x%02x  ",dfcdlTuneDb.goodTxClkValues[i]);

    }

    dfcdlPrint("\n");

    return (GT_OK);
}

/*******************************************************************************
* gtDfcdlGetValidRxClkValues
*
* DESCRIPTION:
*   The function returns the valid RxClk value. For each RxClk value it returns
*   the list of dqs valid values.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       rxClkValidValues - Arrays of valid rxClk values.
*
* RETURNS :
*
*
*
* COMMENTS:
*       The memory for the array should be allocated before calling the
*       function. The values that will be returned are the last values of the
*       scan. If PP auto scan was invoked then the values will be the PP RxClk
*       values. If the FA auto scan was invoked then the values will be the FA
*       RxClk values. The index of the array is the rxClk value. If the
*       dqsValuesLength = 0 then the rxClk value is not valid
*
*******************************************************************************/
GT_STATUS gtDfcdlGetValidRxClkValues
(
    OUT RXCLK_VS_DQS_WINDOW rxClkValidValues[DFCDL_MAX_DELAY]
)
{
    if (bDataBaseInit == GT_FALSE)
    {
        return (GT_NOT_INITIALIZED);
    }

    /* Validity check */
    if (rxClkValidValues == NULL)
    {
        return (GT_BAD_PARAM);
    }

    cpssOsMemCpy(rxClkValidValues,dfcdlTuneDb.goodRxClkValues,
                 sizeof(rxClkValidValues));

    return (GT_OK);
}

/*******************************************************************************
* gtDfcdlPrintValidRxClkValues
*
* DESCRIPTION:
*   The function prints the valid RxClk values found in the scan. The function
*   also prints the valid dqs valid for each rxClk value.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*
*
*
* COMMENTS:
*   The values that will be printed are the last values of the
*   scan. If PP auto scan was invoked then the values will be the PP RxClk
*   values. If the FA auto scan was invoked then the values will be the FA
*   RxClk values.
*
*******************************************************************************/
GT_STATUS gtDfcdlPrintValidRxClkValues
(
    GT_VOID
)
{
    GT_U8   i,j;  /* loop counter */
    GT_U8   length;

    if (bDataBaseInit == GT_FALSE)
    {
        return (GT_NOT_INITIALIZED);
    }

    dfcdlPrint("\n\nValid RxClk entries.\n");
    dfcdlPrint("====================\n");

    /* For all the valid entries */
    for (i = 0 ; i < DFCDL_MAX_DELAY ; i++)
    {
        length = dfcdlTuneDb.goodRxClkValues[i].dqsValuesLength;

        /* Only for the valid RxClk values */
        if (length > 0)
        {
            dfcdlPrint("\nRxClk:0x%02x  DQS values: ",i);
            /* Print all the dqs values */
            for (j = 0 ; j < length; j++ )
            {
                dfcdlPrint("0x%02x  ",
                            dfcdlTuneDb.goodRxClkValues[i].dqsValues[j]);
            }
        }
    }

    dfcdlPrint("\n");

    return (GT_OK);
}

/*******************************************************************************
* gtDfcdlGetValues
*
* DESCRIPTION:
*   Get the DFCDL values.
*
* INPUTS:
*       None.
* OUTPUTS:
*       dfcdlValuesArray    - Array containing the DFCDL values.
*
* RETURNS :
*
*
*
* COMMENTS:
*       The memory for the array should be allocated before calling the
*       function. The values that will be returned are the last values of the
*       scan. If PP auto scan was invoked then the values will be the PP DFCDL
*       values. If the FA auto scan was invoked then the values will be the FA
*       DFCDL values.
*
*
*******************************************************************************/
GT_STATUS gtDfcdlGetValues
(
    OUT GT_U32  dfcdlValuesArray[SDL_PP_SDRAM_SIZE]
)
{
    if (bDataBaseInit == GT_FALSE)
    {
        return (GT_NOT_INITIALIZED);
    }

    /* Validity check */
    if (dfcdlValuesArray == NULL)
    {
        return (GT_BAD_PARAM);
    }

    cpssOsMemCpy(dfcdlValuesArray,dfcdlTuneDb.dfcdlValues,
                 sizeof(dfcdlValuesArray));

    return (GT_OK);
} /* gtDfcdlGetValues */

/*******************************************************************************
* gtDfcdlPrintValues
*
* DESCRIPTION:
*   Print the DFCDL values.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*
*
* COMMENTS:
*       The values that will be printed are the last values of the
*       scan. If PP auto scan was invoked then the values will be the PP DFCDL
*       values. If the FA auto scan was invoked then the values will be the FA
*       DFCDL values.
*
*
*******************************************************************************/
GT_STATUS gtDfcdlPrintValues
(
    GT_VOID
)
{
    GT_U32  i; /* loop counter */

    if (bDataBaseInit == GT_FALSE)
    {
        return (GT_NOT_INITIALIZED);
    }

    dfcdlPrint("\n\nDFCDL values.\n");
    dfcdlPrint("====================\n");
    dfcdlPrint("\nEntry number \t DFCDL value");
    dfcdlPrint("\n============ \t ===========\n");
    /* For all the DFCDL entries */
    for (i = 0 ; i < SDL_PP_SDRAM_SIZE ; i++)
    {
        dfcdlPrint("\n%5d \t\t    0x%x",i,dfcdlTuneDb.dfcdlValues[i]);
    }

    dfcdlPrint("\n");

    return (GT_OK);
} /* gtDfcdlPrintValues */

/*******************************************************************************
* gtDfcdlPpScan
*
* DESCRIPTION:
*   The function set the dfcdl table with the given values and check the MIB
*   counter value.
*
* INPUTS:
*       devNum          - The device number to do the scaning.
*       scanParams      - The scaning parameters.
*       isFirst         - is first entry to scan.
*       isLast          - is this the last entry to scan.
*
* OUTPUTS:
*       errorCnt        - MIB error counter value.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS gtDfcdlPpScan
(
    IN  GT_U8                       devNum,
    IN  DFCDL_BUFFER_DRAM_PARAMS    *scanParam,
    IN  GT_BOOL                     isFirst,
    IN  GT_BOOL                     isLast,
    OUT GT_U64                      *errorCnt
)
{
    GT_STATUS rc;               /* return code                       */
    GT_U64    portPktCounter;   /* used to count send packet         */
    GT_U64    tmpGoodPacket;    /* used to read counters of one port */
    GT_U64    tmpCRCPacket;     /* used to read counters of one port */
    GT_U8     portNum;          /* used to scan all the ports        */

    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);

    if (GT_TRUE == isFirst)
    {
        /* first scan */
        rc = gtDfcdlScanSetConfig(devNum,GT_TRUE,scanParam->extTraffic,
                                  GT_FALSE,GT_PATTERN_TYPE_SSO);
        if (GT_OK != rc)
        {
            return rc;
        }

        /* allocate memory */
        resetCnt = cpssOsMalloc(sizeof(GT_U64) * (dfcdlTuneDb.lastPortNum + 1) );
        if (resetCnt == NULL)
        {
            return (GT_OUT_OF_CPU_MEM);
        }
    }

    /* write current values to the DFCDL */
    rc = dfcdlPpDramSet(devNum,scanParam->startBurst,scanParam->dqs,
                        scanParam->clkOut);
    if (rc != GT_OK)
    {
        return (rc);
    }

    /* Give a delay before reseting counters to make sure that    */
    /* the DFCDL is set                                           */
    rc = cpssOsTimerWkAfter(10);
    if ( rc != GT_OK )
    {
        return (rc);
    }

    /* Reset all the counters before running the new confguration */
    rc = dfcdlResetCounter(devNum,resetCnt,resetCnt);
    if (rc != GT_OK)
    {
        return (rc);
    }

    /* Delay before checking the results */
    rc = cpssOsTimerWkAfter(scanParam->testDelay);
    if ( rc != GT_OK )
    {
        return (rc);
    }


    /* Reset the counters */
    cpssOsMemSet(errorCnt, 0, sizeof(GT_U64) );
    cpssOsMemSet(&portPktCounter, 0, sizeof(GT_U64) );

    /* Count the number of packet recieved and number of packets */
    /* that had CRC errors                                       */
    for ( portNum = dfcdlTuneDb.firstPortNum ;
         portNum <= dfcdlTuneDb.lastPortNum ;
         portNum++ )
    {
        /* Reset the counters before reading*/
        cpssOsMemSet(&tmpGoodPacket, 0, sizeof(GT_U64) );
        cpssOsMemSet(&tmpCRCPacket, 0, sizeof(GT_U64) );

        /* Count the number of sent packets to see if there is    */
        /* traffic                                                */
        rc = cpssExMxPortMacCounterGet(devNum,portNum,CPSS_GOOD_PKTS_RCV_E,
                                       &tmpGoodPacket);
        if ( rc != GT_OK )
            return (rc);

        portPktCounter = prvCpssMathAdd64(portPktCounter,tmpGoodPacket);

        /* Count the number of CRCs */
        rc = cpssExMxPortMacCounterGet(devNum,portNum,CPSS_BAD_CRC_E,
                                       &tmpCRCPacket);
        if ( rc != GT_OK )
            return (rc);

        *errorCnt = prvCpssMathAdd64(*errorCnt,tmpCRCPacket);
    }

    /* If there was no Traffic print a Warning message */
    if ( (portPktCounter.l[0] == 0) && (portPktCounter.l[1] == 0) )
    {
        dfcdlPrint("\ndevice:0x%02x \t clkOut:0x%02x \t Dqs:0x%02x \t"
                 "startBurst:0x%02x \t NO TRAFFIC!!\n",devNum,
                 scanParam->clkOut,scanParam->dqs,scanParam->startBurst);
    }

    if (GT_TRUE == isLast)
    {
        /* last scan */

        rc = gtDfcdlScanSetConfig(devNum,GT_FALSE,scanParam->extTraffic,
                                  GT_FALSE,GT_PATTERN_TYPE_SSO);
        if (GT_OK != rc)
        {
            return rc;
        }

        /* free memory */
        cpssOsFree(resetCnt);
    }

    return GT_OK;
} /* gtDfcdlPpScan */

/*******************************************************************************
* gtDfcdlScanSetConfig
*
* DESCRIPTION:
*   The function set the dfcdl configuration before and after the dfcdl scan.
*
* INPUTS:
*       devNum          - The device number to do the scaning.
*       isStart         - start / stop the configuration.
*       isExternal      - external / internal traffic.
*       isFaLoop        - enable fa loop.
*       packetType      - packet type to transmit.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS gtDfcdlScanSetConfig
(
    IN  GT_U8                       devNum,
    IN  GT_BOOL                     isStart,
    IN  GT_BOOL                     isExternal,
    IN  GT_BOOL                     isFaLoop,
    IN  GT_PCK_PATTERN_TYPE         packetType
)
{
    GT_STATUS rc;        /* return code */
    GT_U8 lastPortNum;

    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);

    lastPortNum = PRV_CPSS_PP_MAC(devNum)->numOfPorts - 1;

    if (GT_TRUE == isStart)
    {
        /* start configuration */
        rc = gtDfcdlScanConfig(devNum, 0, lastPortNum, isExternal, GT_TRUE,
                               GT_TRUE);
        if (GT_OK != rc)
        {
            return rc;
        }

        if (GT_FALSE == isExternal)
        {
            /* init internal traffic */
            rc = gtDfcdlConfigInternalTraffic(devNum,packetType,
                                              internalTrafficlen);
            if (GT_OK != rc)
            {
                return rc;
            }

            /* Configure the PP for the tuning */
            rc = dfcdlConfigPpForTune(devNum);
            if (rc != GT_OK)
            {
                return (rc);
            }
        }

        if (GT_TRUE == isFaLoop)
        {
            /* enable ECC events */
            rc = dfcdlScanEccEventEnable(devNum);
            if (GT_OK != rc)
            {
                return rc;
            }

            /* Configure the Fa for the tuning */
            rc = gtPp2FAEnG4Loop(devNum,Pp2FAEnG4Loop_IN_Enable_TRUE,
                                 Pp2FAEnG4Loop_IN_isTagged_noTagging);
            if (rc != GT_OK)
            {
                return (rc);
            }
        }

        if (GT_FALSE == isExternal)
        {
            /* Start sending the packets */
            rc = dfcdlSendInternalPackets(devNum,GT_TRUE);
            if (rc != GT_OK)
            {
                return (rc);
            }
        }
    }
    else
    {
        /* stop configuration */
        if (isExternal == GT_FALSE)
        {
            /* Stop sending packets */
            rc = dfcdlSendInternalPackets(devNum,GT_FALSE);
            if (rc != GT_OK)
            {
                return (rc);
            }

            /* stop the packet send task */
            rc = cpssOsTimerWkAfter(10);
            if ( rc != GT_OK )
            {
                return (rc);
            }

        }

        /* Delete the VLANs to stop the traffic */
        rc = dfcdlAddAllPort2Vlans(devNum,GT_FALSE);
        if (rc != GT_OK)
        {
            return (rc);
        }

        rc = gtDfcdlScanConfig(devNum, 0, lastPortNum, isExternal, GT_FALSE,
                               GT_TRUE);
        if (GT_OK != rc)
        {
            return rc;
        }
    }

    return GT_OK;
} /* gtDfcdlScanSetConfig */

/********* Internal functions *************************************************/

/*******************************************************************************
* dfcdlSetInternalSB
*
* DESCRIPTION:
*   This function update the S.B type to hardware and to global DB.
*
* INPUTS:
*       devNum          - The device number.
*       extSB           - GT_TRUE - external S.B.
*       devType         - device type, PP or FA
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlSetSB
(
    IN GT_U8                devNum,
    IN GT_BOOL              extSB,
    IN GT_DFCDL_DEVICE_TYPE devType
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    fieldData;

    if(extSB == GT_TRUE)
    {/* external S.B. */
        fieldData = 0x0;
    }
    else
    {/* internal S.B. */
        fieldData = 0x3;

    }
    if(devType == GT_DFCDL_PP)
    {
        /* write to HW - set bit 20, 21 */
        rc = cpssDrvPpHwRegFieldSet(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                    PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->
                                    extMemRegs.bdTiming4Reg,20,2,fieldData);

    }
    else if(devType == GT_DFCDL_FA)
    {
#ifdef IMPL_FA
        /* write to HW - set bit 20, 21 */
        rc = cpssDrvPpHwRegFieldSet(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                    coreFaDevs[devNum]->regsAddr->
                                    extMemRegs.bdTiming4Reg,20,2,fieldData);
#else
        return GT_FAIL;
#endif
    }

    RETURN_ON_ERROR(rc);

    dfcdlTuneDb.extSB = extSB;

    return GT_OK;
} /* dfcdlSetSB */

/*******************************************************************************
* dfcdlGetRxClkScanVal
*
* DESCRIPTION:
*   This function calculate the rxClk parameters for scanning according to the
*   S.B type and to the hang poiunt.
*
* INPUTS:
*       scanParams      - The user scaning parameters.
*
* OUTPUTS:
*       scanValues      - The scaning parameters according to the S.B type
*                         and to the hang poiunt.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlGetRxClkScanVal
(
    INOUT DFCDL_BUFFER_DRAM_SCAN_PARAMS   *scanValues
)
{

    if (bDataBaseInit == GT_FALSE)
    {
        return (GT_NOT_INITIALIZED);
    }

    if (GT_TRUE == dfcdlTuneDb.extSB)
    {
        /* external S.B. */

        /* start parameters =
        maximum of rx hang point to user start parameter */
        scanValues->rxClkStart = (GT_U8)MAX(scanValues->rxClkStart,
                                            (dfcdlTuneDb.externalSBHangPoint +
                                             rxHangPointOffsExt));

        /* check if rx end value > rx start */
        if (scanValues->rxClkEnd < scanValues->rxClkStart)
        {
            return GT_BAD_PARAM;
        }
    }
    else
    {
        /* internal S.B. */
        /* start parameters =
        minimum of rx hang point to  user start parameter */
        scanValues->rxClkStart = (GT_U8)MIN(scanValues->rxClkStart,
                                            (dfcdlTuneDb.internalSBHangPoint -
                                             (rxHangPointOffsInt)));

        /* check if rx end value < rx start */
        if (scanValues->rxClkEnd > scanValues->rxClkStart)
        {
            return GT_BAD_PARAM;
        }
    }

    return GT_OK;
}


/*******************************************************************************
* dfcdlInitLocalDb
*
* DESCRIPTION:
*   Allocate and initilaize the local database for the DFCDL tuning. The functio
*   Also clears the memory of the database.
*
* INPUTS:
*       init - GT_TRUE  - Allocate and initialize the database.
*              GT_FALSE - Free the database memory.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK             - Operation succeeded
*       GT_OUT_OF_CPU_MEM - Cpu memory allocation failed.
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlInitLocalDb
(
    IN GT_BOOL  init
)
{
    if ( init == GT_TRUE )
    {
        /* Clean the memory */
        cpssOsMemSet(&dfcdlTuneDb, 0, sizeof(DFCDL_TUNE_DB));
        dfcdlTuneDb.startQueueTxClkValues = GT_FALSE;
    }
    /* Free the memory */
    else
    {

        if ( (NULL != dfcdlTuneDb.intTraff.invBuffer) &&
             (NULL != dfcdlTuneDb.intTraff.buffer) )
        {
            /* Free the buffers */
            cpssOsCacheDmaFree(dfcdlTuneDb.intTraff.buffer[0]);
            dfcdlTuneDb.intTraff.buffer[0] = NULL;
            cpssOsCacheDmaFree(dfcdlTuneDb.intTraff.invBuffer[0]);
            dfcdlTuneDb.intTraff.invBuffer[0] = NULL;
        }
        else
        {
            dfcdlPrint("**** Data structures was not initialized.\n");
            return GT_NOT_INITIALIZED;
        }
    }

    return GT_OK;
} /* dfcdlInitLocalDb */

/*******************************************************************************
* dfcdlConfigPpForTune
*
* DESCRIPTION:
*   Configure the PP for the tuning process.
*
* INPUTS:
*       devNum      - Device number.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK             - Operation succeeded
*
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlConfigPpForTune
(
    IN GT_U8        devNum
)
{
    GT_STATUS   rc;         /* Return code      */

    /* Add all the ports to a vlan */
    rc = dfcdlAddAllPort2Vlans(devNum, GT_TRUE);
    RETURN_ON_ERROR(rc);

    /* Config ports MAC configuration */
    rc = dfcdlConfigPortMac(devNum);
    RETURN_ON_ERROR(rc);

    /* Config ports PHY configuration */
    rc = dfcdlConfigPortPhyConfig(devNum);
    RETURN_ON_ERROR(rc);

    /* Disable all intterupts exept the TX end interrupt */
    rc = dfcdlDisablePPInt(devNum);
    RETURN_ON_ERROR(rc);

    return GT_OK;
} /* dfcdlConfigPpForTune */

/*******************************************************************************
* dfcdlConfigPpForTuneExternal
*
* DESCRIPTION:
*   Configure the PP for the tuning process.
*
* INPUTS:
*       devNum      - Device number.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK             - Operation succeeded
*
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlConfigPpForTuneExternal
(
    IN GT_U8        devNum
)
{
    GT_STATUS   rc;         /* Return code */

    /* Add all the ports to a vlan */
    rc = dfcdlAddAllPort2Vlans(devNum,GT_TRUE);
    RETURN_ON_ERROR(rc)

    /* Config ports MAC configuration */
    rc = dfcdlConfigPortMac(devNum);
    RETURN_ON_ERROR(rc)

    if (useLoopBack == GT_TRUE)
    {
        /* Config ports PHY configuration */
        rc = dfcdlConfigPortPhyConfig(devNum);
        RETURN_ON_ERROR(rc)
    }

    /* Disable all intterupts */
    rc = dfcdlDisablePPIntExternal(devNum);
    RETURN_ON_ERROR(rc)

    return (GT_OK);
} /* dfcdlConfigPpForTuneExternal */

/*******************************************************************************
* dfcdlConfigPortMac
*
* DESCRIPTION:
*   This function sets all the Mac configuration for the internal loop.
*
* INPUTS:
*       devNum      - Device number.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK             - Operation succeeded
*
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlConfigPortMac
(
    GT_U8   devNum
)
{
    GT_U8               portNum;    /* Port number loop counter */
    GT_STATUS           rc;         /* Return code              */
    GT_U8               middlePort; /* Middle port number       */
    CPSS_PORT_SPEED_ENT portSpeed;  /* To hold ports speed      */

    if (PRV_CPSS_PP_MAC(devNum)->numOfPorts == 1)
    {
        /* XG devices */
        rc = cpssExMxPortCrcCheckEnableSet(devNum,0,GT_TRUE);
        RETURN_ON_ERROR(rc);

        return GT_OK;
    }

    middlePort = dfcdlTuneDb.firstPortNum +
                ( (dfcdlTuneDb.lastPortNum - dfcdlTuneDb.firstPortNum) / 2 );

    for ( portNum = dfcdlTuneDb.firstPortNum;
          portNum <= dfcdlTuneDb.lastPortNum; portNum++ )
    {
        rc = cpssExMxBrgVlanPortIngFltEnable(devNum,portNum,GT_FALSE);
        RETURN_ON_ERROR(rc);

        rc = cpssExMxPortEnable(devNum,portNum,GT_TRUE);
        RETURN_ON_ERROR(rc);

        rc = cpssExMxPortDuplexModeSet(devNum,portNum,CPSS_PORT_FULL_DUPLEX_E);
        RETURN_ON_ERROR(rc);

        rc = dfcdlGetPortMaxSpeed(devNum,portNum,&portSpeed);
        RETURN_ON_ERROR(rc);

        rc = cpssExMxPortSpeedSet(devNum,portNum,portSpeed);
        RETURN_ON_ERROR(rc);

        rc = cpssExMxPortDuplexAutoNegEnable(devNum,portNum,GT_FALSE);
        RETURN_ON_ERROR(rc);

        rc = cpssExMxPortFlowCntrlAutoNegEnable(devNum,portNum,GT_FALSE,
                                                GT_FALSE);
        RETURN_ON_ERROR(rc);

        rc = cpssExMxPortSpeedAutoNegEnable(devNum,portNum,GT_FALSE);
        RETURN_ON_ERROR(rc);

        rc = cpssExMxPortFlowControlEnable(devNum,portNum,GT_FALSE);
        RETURN_ON_ERROR(rc);

        rc = cpssExMxPortPeriodicFcEnable(devNum,portNum,GT_FALSE);
        RETURN_ON_ERROR(rc);

        rc = cpssExMxPortBackPressureEnable(devNum,portNum,GT_FALSE);
        RETURN_ON_ERROR(rc);

        rc = cpssExMxPortForceLinkDownEnable(devNum,portNum,GT_FALSE);
        RETURN_ON_ERROR(rc);

        rc = cpssExMxPortForceLinkPassEnable(devNum,portNum,GT_TRUE);
        RETURN_ON_ERROR(rc);

        rc = cpssExMxCosPortOverrideTcEnable(devNum,portNum,GT_TRUE);
        RETURN_ON_ERROR(rc);

        rc = cpssExMxPortCrcCheckEnableSet(devNum,portNum,GT_TRUE);
        RETURN_ON_ERROR(rc);

        /* Distribute ports between the two almost evenly */
        if (portNum < middlePort)
        {
            rc = cpssExMxCosPortDefaultTcSet(devNum,portNum,0);
        }
        else
        {
            rc = cpssExMxCosPortDefaultTcSet(devNum,portNum,4);
        }

        RETURN_ON_ERROR(rc);

        rc = cpssExMxBrgVlanPortPvidSet(devNum,portNum,
                                     (GT_U16)(DFCDL_FIRST_USED_VLAN + portNum));
        RETURN_ON_ERROR(rc);

        if (portSpeed == CPSS_PORT_SPEED_1000_E)
        {
            rc = cpssExMxPortTxMaxDescNumberSet(devNum,portNum,0x38);
            RETURN_ON_ERROR(rc);
        }

        if (portSpeed == CPSS_PORT_SPEED_100_E)
        {
            rc = cpssExMxPortTxMaxDescNumberSet(devNum,portNum,0x8);
            RETURN_ON_ERROR(rc);
        }

        rc = cpssExMxPortTxWatchdogEnable(devNum,portNum,GT_FALSE,0x1f4);
        RETURN_ON_ERROR(rc);

    } /* for */

    return GT_OK;
} /* dfcdlConfigPortMac */

/*******************************************************************************
* dfcdlConfigPortPhyConfig
*
* DESCRIPTION:
*   This function sets all the Mac configuration for the internal loop.
*
* INPUTS:
*       devNum      - Device number.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK             - Operation succeeded
*
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlConfigPortPhyConfig
(
    IN GT_U8   devNum
)
{
    GT_U8               portNum;   /* Port number loop counter */
    GT_STATUS           rc;        /* Return code              */
    CPSS_PORT_SPEED_ENT portSpeed; /* Maximal port speed           */
    GT_U16              value;     /* Value to set to the register */

    if (PRV_CPSS_PP_MAC(devNum)->numOfPorts == 1)
    {
        /* XG devices */
        /* enable XPHY SHALOW LOOPBACK */

        /* set - bit 14 loopback enable */
        rc = cpssExMxPhyPort10GSmiRegisterWrite(devNum, 0, 0, GT_FALSE, 0x0000,
                                                5, 0x6040);
        RETURN_ON_ERROR(rc);

        rc = cpssExMxPhyPort10GSmiRegisterWrite(devNum, 0, 0, GT_FALSE, 0x8000,
                                                5, 0x470A);
        RETURN_ON_ERROR(rc);

        rc = cpssExMxPhyPort10GSmiRegisterWrite(devNum, 0, 0, GT_FALSE, 0x0000,
                                                5, 0x2040);
        RETURN_ON_ERROR(rc);
    }
    else
    {
        if ( dfcdlTuneDb.firstPortNum == dfcdlTuneDb.lastPortNum )
        {
            return GT_NOT_SUPPORTED;
        }
        /* For all the ports */
        for ( portNum = dfcdlTuneDb.firstPortNum;
              portNum <= dfcdlTuneDb.lastPortNum; portNum++ )
        {
            if (portNum == externalPort)
            {
                continue;
            }

            /* For devices with 52 ports the 4 GIGA ports need more
               configuration */
            if (PRV_CPSS_PP_MAC(devNum)->numOfPorts == 52)
            {
                if ( (portNum >= 48) && (portNum <= 51) )
                {
                    rc = cpssExMxPhyPortSmiRegisterWrite(devNum, portNum , 16,
                                                         0x68);
                    RETURN_ON_ERROR(rc);
                }
            }

            rc = dfcdlGetPortMaxSpeed(devNum,portNum,&portSpeed);
            RETURN_ON_ERROR(rc);

            switch (portSpeed)
            {
                case(GT_SPEED_100):
                    value = 0xA100;
                    break;

                case(GT_SPEED_1000):
                    value = 0x8140;
                    break;
                default:
                    return GT_BAD_VALUE;

            }
            /* Force Gig Speed (Bit-13/6 set to Gig) & Soft Reset (Bit-15) */
            rc = cpssExMxPhyPortSmiRegisterWrite(devNum, portNum, (GT_U8)0x0,
                                                 value);
            RETURN_ON_ERROR(rc);

            switch (portSpeed)
            {
                case(CPSS_PORT_SPEED_100_E):
                    value = 0x6100;
                    break;

                case(CPSS_PORT_SPEED_1000_E):
                    value = 0x4140;
                    break;
                default:
                    return GT_BAD_VALUE;
            }

            /* Enable LoopBack & No Soft Reset */
            rc = cpssExMxPhyPortSmiRegisterWrite(devNum, portNum ,(GT_U8)0x0,
                                                 value);
            RETURN_ON_ERROR(rc);

            if(portSpeed == GT_SPEED_100)
            {/* apply errata */
                rc = cpssExMxPhyPortSmiRegisterWrite(devNum, portNum ,(GT_U8)29,
                                                     0x2);
                RETURN_ON_ERROR(rc);

                /* Block 100Mb link */
                rc = cpssExMxPhyPortSmiRegisterWrite(devNum, portNum ,(GT_U8)30,
                                                     0x8767);
                RETURN_ON_ERROR(rc);
            }
        } /* for */
    }
    return GT_OK;
} /* dfcdlConfigPortPhyConfig */

/*******************************************************************************
* dfcdlDisablePPIntExternal
*
* DESCRIPTION:
*   Disable the interrupts of the PP.
*
* INPUTS:
*       devNum      - Device number.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK             - Operation succeeded
*
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlDisablePPIntExternal
(
    GT_U8   devNum
)
{
    GT_U32          i;          /* loop counter */
    GT_STATUS       rc;         /* return code */

    for (i = 0; i < CPSS_PP_UNI_EV_MAX_E; i++)
    {
        rc = cpssEventDeviceMaskSet(devNum, i, CPSS_EVENT_MASK_E);
        if(rc == GT_NOT_FOUND)
        {
            /* this event not relevant to this device */
            rc = GT_OK;
        }
        else
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}/* dfcdlDisablePPIntExternal */

/*******************************************************************************
* dfcdlDisablePPInt
*
* DESCRIPTION:
*   Disable the interrupts of the PP.
*
* INPUTS:
*       devNum      - Device number.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK             - Operation succeeded
*
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlDisablePPInt
(
    IN GT_U8   devNum
)
{
    GT_U32          i;          /* loop counter */
    GT_STATUS       rc;         /* return code */

    for (i = 0; i < CPSS_PP_UNI_EV_MAX_E; i++)
    {
        rc = cpssEventDeviceMaskSet(devNum, i, CPSS_EVENT_MASK_E);
        if(rc == GT_NOT_FOUND)
        {
            /* this event not relevant to this device */
            rc = GT_OK;
        }
        else
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    for (i = CPSS_PP_TX_BUFFER_QUEUE_E; i <= CPSS_PP_TX_END_E; i++)
    {
        rc = cpssEventDeviceMaskSet(devNum, i, CPSS_EVENT_UNMASK_E);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
} /* dfcdlDisablePPInt */

/*******************************************************************************
* dfcdlConfigFaForTune
*
* DESCRIPTION:
*   Configure the Fa and XBar for the tune.
*
* INPUTS:
*       devNum      - Device number.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlConfigFaForTune
(
    IN GT_U8    devNum
)
{
#ifdef IMPL_FA
    GT_STATUS rc;               /* Return code      */
    GT_U8     fport[LBH_SIZE];  /* fport array      */
    GT_U32    regAddr;          /* Register address */
    GT_U16    mcId;             /* MC id            */
    GT_U16    firstMcId;        /* First MC id      */
    GT_U16    lastMcId;         /* Last MC id       */
    GT_U8     mcFport;          /* Mc fport         */

    if ( NULL == driverFaDevs[devNum] )
    {
        dfcdlPrint("**** System was not initialized completely.\n");
        return GT_NOT_INITIALIZED;
    }
    firstMcId = DFCDL_FIRST_USED_VLAN + dfcdlTuneDb.firstPortNum;
    lastMcId  = DFCDL_FIRST_USED_VLAN + dfcdlTuneDb.lastPortNum;

    /* Enable the device */
    rc = gtFaVoqSetDevTbl(devNum, devNum, GT_TRUE);
    RETURN_ON_ERROR(rc);

    /* Enable Fport4 in the VOQ */
    rc = gtFaVoqFportTxEnable(devNum, 4, GT_TRUE);
    RETURN_ON_ERROR(rc);

    /* Map devNum-->Fport4 VOQ Mapping TarDev to Fport */
    fport[0] = 4;
    rc = gtFaVoqDevToFportMap(devNum, devNum, fport);
    RETURN_ON_ERROR(rc);

    /* Enable fport 4 in the xbar */
    rc = gtXbarFportEnable(devNum, 4, GT_TRUE);
    RETURN_ON_ERROR(rc);

    /* Enable local switching */
    rc = gtXbarLocalSwitchingEn(devNum, 4, GT_TRUE);
    RETURN_ON_ERROR(rc);

    /* Enable MC trafic */
    rc = gtFaVoqMcTxEn(devNum, GT_TRUE);
    RETURN_ON_ERROR(rc);

    /* Set the MC routing table */
    mcFport = 4;
    for ( mcId = firstMcId; mcId <= lastMcId; mcId++ )
    {
        rc = gtXbarSetMcRoute(devNum, mcId, &mcFport, 1);
        RETURN_ON_ERROR(rc);
    }
    /* Configure the PP to send all packets to uplink */
    regAddr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->
        bufferMng.eqBlkCfgRegs.rcvSniffMirror;

    rc = cpssDrvPpHwRegBitMaskWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr, 0x300000, 0x300000);
    RETURN_ON_ERROR(rc);

    return GT_OK;
#else
    return GT_FAIL;
#endif
} /* dfcdlConfigFaForTune */

/*******************************************************************************
* dfcdlDramRangeScan
*
* DESCRIPTION:
*       Scan for valid entries of rxClk,dqs and txClk parameters.
*
* INPUTS:
*       devNum       - Device number.
*       scanParams   - Scan parameters.
*       devType      - device type.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlDramRangeScan
(
    IN  GT_U8                            devNum,
    IN  DFCDL_BUFFER_DRAM_SCAN_PARAMS    *scanParams,
    IN  GT_DFCDL_DEVICE_TYPE             devType
)
{
    GT_STATUS   rc = GT_OK;         /* Return code                      */
    GT_U8       rxClkCounter;       /* Loop counters                    */
    GT_U8       txClkCounter;       /* Loop counters                    */
    GT_U8       dqsCounter;         /* Loop counters                    */
    GT_U64      *portCrcCounter;    /* Ports CRC counter array          */
    GT_U64      *portPktCounter;    /* Ports packet counter array       */
    GT_BOOL     foundGoodRxClk;     /* Good entire of rxClk was found   */
    GT_BOOL     rxClkIncrement;     /* Increment or decrement the count */
    GT_U8       rxClkLoEdge;        /* RxClk lo edge number             */
    GT_U8       rxClkHiEdge;        /* RxClk high edge number           */
    DFCDL_BUFFER_DRAM_TUNE_PARAMS tuneParams; /* Tuning parameters      */
    GT_U8       maxDqsValueLength;  /* Maximal DQS window length        */
    GT_U8       dqsLen;             /* counted dqs window length        */
    GT_U8       realDqsLen;         /* real length of dqs window        */
    GT_U8       dqsLenRetry;        /* Retry counter for dqsLen=0 cases */
    GT_U8       vSize1, vSize2;


  /* code optimization : seperate this function to 2 function
        1. loop to find rxClk & dqs values.
        2. loop to find txClk values. */

    /* Validity check */
    if (scanParams == NULL)
    {
        return GT_BAD_PTR;
    }

    /* Range check */
    if ( (scanParams->dqsStart > scanParams->dqsEnd) ||
         (scanParams->txClkStart > scanParams->txClkEnd) )
    {
        dfcdlDebugPrint("delay values range incorrect\n");
        return GT_BAD_VALUE;
    }

    if ( (scanParams->dqsEnd >= DFCDL_MAX_DELAY ) ||
         (scanParams->rxClkEnd >= DFCDL_MAX_DELAY) ||
         (scanParams->txClkEnd >= DFCDL_MAX_DELAY) )
    {
        dfcdlDebugPrint("delay values range too big\n");
        return GT_OUT_OF_RANGE;
    }

    if (scanParams->rxClkEnd > scanParams->rxClkStart)
    {
        /* increment must be only for External S.B */
        if ((GT_FALSE == dfcdlTuneDb.extSB) &&
            (scanParams->rxClkEnd != scanParams->rxClkStart))
        {
            dfcdlDebugPrint("Rx clk order incorrect\n");
            return GT_BAD_PARAM;
        }

        rxClkIncrement = GT_TRUE;
        rxClkLoEdge = scanParams->rxClkStart;
        rxClkHiEdge = scanParams->rxClkEnd;
        rxClkCounter = scanParams->rxClkStart;
    }
    else
    {
        /* decrement must be only for Internal S.B */
        if ((GT_TRUE == dfcdlTuneDb.extSB) &&
            (scanParams->rxClkEnd != scanParams->rxClkStart))
        {
            dfcdlDebugPrint("Rx clk order incorrect\n");
            return GT_BAD_PARAM;
        }

        rxClkIncrement = GT_FALSE;
        rxClkLoEdge = scanParams->rxClkEnd;
        rxClkHiEdge = scanParams->rxClkStart;
        rxClkCounter = scanParams->rxClkStart;
    }

    /* Allocate memory for the ports counter CRC and PKT */
    portCrcCounter = cpssOsMalloc(sizeof(GT_U64) *
                                  (dfcdlTuneDb.lastPortNum + 1) );
    if (portCrcCounter == NULL)
    {
        dfcdlDebugPrint("allocation failed\n");
        return GT_OUT_OF_CPU_MEM;
    }

    portPktCounter = cpssOsMalloc(sizeof(GT_U64) *
                                  (dfcdlTuneDb.lastPortNum + 1) );
    if (portPktCounter == NULL)
    {
        cpssOsFree(portCrcCounter);
        dfcdlDebugPrint("allocation failed\n");
        return GT_OUT_OF_CPU_MEM;
    }

    dfcdlTuneDb.rxMaxDqs = 0;

    maxDqsValueLength = 0;
    realDqsLen = 0;
    vSize1 = 0;
    vSize2 = 0;
    dfcdlTuneDb.foundBreak = GT_FALSE;
    dqsLenRetry            = 0;

    /* For all the range of the rxClk */
    do
    {
        /* For all the DQS values */
        for (dqsCounter = scanParams->dqsStart;
             dqsCounter <= scanParams->dqsEnd; dqsCounter++)
        {
            foundGoodRxClk = GT_FALSE;

            /* For all the txClk values */
            for (txClkCounter = scanParams->txClkStart;
                 txClkCounter <= scanParams->txClkEnd; txClkCounter++)
            {
                if (stopTraffic)
                {
                    GT_U8 port;
                    /* Enable all ports */
                    for (port = 0; port < PRV_CPSS_PP_MAC(devNum)->numOfPorts;
                          port++)
                    {
                        cpssExMxPortEnable(devNum,port,GT_FALSE);
                    }

                    dfcdlSendInternalPackets(devNum,GT_FALSE);

                    cpssOsTimerWkAfter(10);
                }

                if (devType == GT_DFCDL_PP)
                {
                    /* Program the DFCDL table with the good values */
                    rc = dfcdlPpDramSet(devNum,rxClkCounter,dqsCounter,
                                        txClkCounter);
                }
                else if (devType == GT_DFCDL_FA)
                {

                    /* Program the DFCDL table with the good values */
                    rc = dfcdlFaDramSet(devNum,rxClkCounter,dqsCounter,
                                        txClkCounter);
                }

                if (rc != GT_OK)
                {
                    cpssOsFree(portCrcCounter);
                    cpssOsFree(portPktCounter);
                    return (rc);
                }

                if (stopTraffic)
                {
                    GT_U8 port;
                    /* Enable all ports */
                    for (port = 0; port < PRV_CPSS_PP_MAC(devNum)->numOfPorts;
                          port++)
                    {
                        cpssExMxPortEnable(devNum,port,GT_TRUE);
                    }

                    dfcdlSendInternalPackets(devNum,GT_TRUE);
                }

                /* Give a delay before reseting counters to make sure that    */
                /* the DFCDL is set                                           */
                rc = cpssOsTimerWkAfter(10);
                if ( rc != GT_OK )
                {
                    cpssOsFree(portCrcCounter);
                    cpssOsFree(portPktCounter);
                    return (rc);
                }

                /* Reset all the counters before running the new confguration */
                rc = dfcdlResetCounter(devNum,portCrcCounter,portPktCounter);
                if (rc != GT_OK)
                {
                    cpssOsFree(portCrcCounter);
                    cpssOsFree(portPktCounter);
                    return (rc);
                }

                /* Delay before checking the results */
                rc = cpssOsTimerWkAfter(scanParams->testDelay);
                if ( rc != GT_OK )
                {
                    cpssOsFree(portCrcCounter);
                    cpssOsFree(portPktCounter);
                    return (rc);
                }

                /* Read the counters and update the counters */
                tuneParams.dqs   = dqsCounter;
                tuneParams.rxClk = rxClkCounter;
                tuneParams.txClk = txClkCounter;
                rc = dfcdlReadCounters(devNum,portCrcCounter,portPktCounter,
                                       &tuneParams,&foundGoodRxClk);
                if (rc != GT_OK)
                {
                    cpssOsFree(portCrcCounter);
                    cpssOsFree(portPktCounter);
                    return (rc);
                }

            }/* For all the txClk values */
            if ((foundGoodRxClk == GT_TRUE) &&
                (dfcdlTuneDb.startQueueTxClkValues == GT_FALSE))
            {
                /* Store the good entries of the rxClk and dqs */
                rc = dfcdlAddGoodRxClkEntry(rxClkCounter,dqsCounter);
                if (rc != GT_OK)
                {
                    cpssOsFree(portCrcCounter);
                    cpssOsFree(portPktCounter);
                    return (rc);
                }
            }

        }/* For all the DQS values */

        /* find maximum of Dqs value length */
        dqsLen = dfcdlTuneDb.goodRxClkValues[rxClkCounter].dqsValuesLength;
        /* calculate real dqs length */
        if(dqsLen != 0)
        {
            realDqsLen =
                dfcdlTuneDb.goodRxClkValues[rxClkCounter].dqsValues[dqsLen - 1] -
                dfcdlTuneDb.goodRxClkValues[rxClkCounter].dqsValues[0] + 1;
        }
        else
        {
            realDqsLen = 0;
        }
        if(realDqsLen > maxDqsValueLength)
        {
            dfcdlTuneDb.rxMaxDqs = rxClkCounter;
            maxDqsValueLength = realDqsLen;
        }
        /* test if we fall below dqs threshold */
        if((realDqsLen + scanParams->dqsThreshold) < maxDqsValueLength)
        {
            vSize1++;
            if((dfcdlTuneDb.foundBreak != GT_TRUE) && (vSize1 == 3))
            {
                dfcdlTuneDb.foundBreak = GT_TRUE;
                if(rxClkIncrement == GT_TRUE)
                {
                    dfcdlTuneDb.breakVal = rxClkCounter - vSize1;
                    dfcdlTuneDb.breakVal -= scanParams->stepBack;
                }
                else
                {
                    dfcdlTuneDb.breakVal = rxClkCounter + vSize1;
                    dfcdlTuneDb.breakVal += scanParams->stepBack;
                }
            }
        }
        else
        {
            vSize1 = 0;
        }

        if (rxClkIncrement == GT_TRUE)
        {
            rxClkCounter++;
        }
        else
        {
            rxClkCounter--;
        }

        /* stop scanning if dqs length is 0  */
        if(dqsLen == 0)
        {
            dqsLenRetry++;
            if (dqsLenRetry >= DFCDL_MAX_DQS_LEN_RETRY)
            {
                dfcdlPrint("Stop scan. DqsLen = 0.\n");
                break;
            }
        }
        else
        {
            dqsLenRetry = 0;
        }

    }while( (rxClkCounter >= rxClkLoEdge) &&
            (rxClkCounter <= rxClkHiEdge) );

    /* break point should exist anyway */
    if(dfcdlTuneDb.foundBreak != GT_TRUE)
    {
        dfcdlTuneDb.foundBreak = GT_TRUE;
        if(rxClkIncrement == GT_TRUE)
        {
            dfcdlTuneDb.breakVal = rxClkCounter - vSize1 - 1;
            /* correct on step back */
            if(vSize1 != 0)
            {
                dfcdlTuneDb.breakVal -= scanParams->stepBack;
            }
        }
        else
        {
            dfcdlTuneDb.breakVal = rxClkCounter + vSize1 + 1;
            /* correct on step back */
            if(vSize1 != 0)
            {
                dfcdlTuneDb.breakVal += scanParams->stepBack;
            }
        }
    }

    /* correct break point */
    if(rxClkIncrement == GT_TRUE)
    {
        dfcdlTuneDb.breakVal = MAX(dfcdlTuneDb.breakVal,
                                     rxClkLoEdge);
    }
    else
    {
        dfcdlTuneDb.breakVal = MIN(dfcdlTuneDb.breakVal,
                                    rxClkHiEdge);
    }

    /* Free all the allocated memory */
    cpssOsFree(portCrcCounter);
    cpssOsFree(portPktCounter);
    portPktCounter = NULL;
    portCrcCounter = NULL;

    return (GT_OK);

} /* dfcdlDramRangeScan */

/*******************************************************************************
* dfcdlPpDramSet
*
* DESCRIPTION:
*       This function sets delay value in all dfcdl dram Table.
*
* INPUTS:
*
*       devNum     - The device number to set
*       rxClkDel   - rxClk value to set in dram table
*       dqsDel     - dqs   value to set in dram table
*       txClkDel   - txClk value to set in dram table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS dfcdlPpDramSet
(
    IN GT_U8    devNum,
    IN GT_U32   rxClkValue,
    IN GT_U8    dqsValue,
    IN GT_U8    txClkValue
)
{
    GT_U8       i;        /* Loop counter     */
    GT_U32      regAddr;  /* Register address */
    GT_U32      value;    /* Value            */
    GT_STATUS   rc;       /* Return code      */

    value = 0;

    if (CPSS_PP_FAMILY_TIGER_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        /* for tiger devices */
        U32_SET_FIELD_MAC(value, 0, 7, dqsValue);
        U32_SET_FIELD_MAC(value, 7, 7, txClkValue); /* clkOut */
        U32_SET_FIELD_MAC(value, 14, 7, rxClkValue);/* start burst */
    }
    else
    {
        U32_SET_FIELD_MAC(value, 0, 6, dqsValue);
        U32_SET_FIELD_MAC(value, 6, 6, txClkValue);
        U32_SET_FIELD_MAC(value, 12, 6, rxClkValue);
    }

    regAddr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->extMemRegs.bdAddressReg;
    /* write SRam dfcdl address register */
    rc = cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr, 0);
    RETURN_ON_ERROR(rc);

    /* fill table with delay value*/
    regAddr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->extMemRegs.bdDataReg;
    for ( i= 0 ;i < SDL_PP_SDRAM_SIZE ; i++ )
    {
        rc = cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr, value);
        RETURN_ON_ERROR(rc);
    }

    return GT_OK;
}

/*******************************************************************************
* dfcdlFaDramSet
*
* DESCRIPTION:
*       This function sets delay value in all dfcdl dram Table.
*
* INPUTS:
*
*       devNum     - The device number to set
*       rxClkDel   - rxClk value to set in dram table
*       dqsDel     - dqs   value to set in dram table
*       txClkDel   - txClk value to set in dram table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS dfcdlFaDramSet
(
    IN GT_U8    devNum,
    IN GT_U32   rxClkValue,
    IN GT_U8    dqsValue,
    IN GT_U8    txClkValue
)
{
#ifdef IMPL_FA
    GT_U8       i;        /* Loop counter     */
    GT_U32      regAddr;  /* Register address */
    GT_U32      value;    /* Value            */
    GT_STATUS   rc;       /* Return code      */


    value = 0;
    U32_SET_FIELD_MAC(value, 0, 6, dqsValue);
    U32_SET_FIELD_MAC(value, 6, 6, txClkValue);
    U32_SET_FIELD_MAC(value, 12, 6, rxClkValue);

    regAddr = coreFaDevs[devNum]->regsAddr->extMemRegs.dfcdlAddressReg;
    /* write SRam dfcdl address register */
    rc = cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr, 0);
    RETURN_ON_ERROR(rc);

    /* fill table with delay value*/
    regAddr = coreFaDevs[devNum]->regsAddr->extMemRegs.dfcdlDataReg;
    for ( i= 0 ;i < SDL_FOX_SDRAM_SIZE ; i++ )
    {
        rc = cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr, value);
        RETURN_ON_ERROR(rc);
    }

    return GT_OK;
#else
    return GT_FAIL;
#endif
}

/*******************************************************************************
* dfcdlAddAllPort2Vlans
*
* DESCRIPTION:
*       Add all the ports to a VLANs
*
* INPUTS:
*       devNum - device number.
*       add    - GT_TRUE - Create the VLANs and add the ports.
*                GT_FALSE- Delete the vlans
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlAddAllPort2Vlans
(
    IN  GT_U8   devNum,
    IN  GT_BOOL add
)
{
    GT_STATUS          rc;                           /* Return code         */
    GT_U8              portInfoList[VLAN_PORT_BITMAP_SIZE]; /* Port bit map */
    CPSS_VLAN_INFO_STC vlanInfo;                     /* Vlan information    */
    GT_U32             portNum;                      /* Port number counter */
    GT_U8              devBitmap[PP_DEV_BITMAP_SIZE];/* Not used            */
    GT_U16             vlanId;                       /* Vlan id             */
    GT_U16             firstVlanId;                  /* First VlanId        */
    GT_U16             lastVlanId;                   /* Last vlan Id        */
    GT_U32             nextPortNum;                  /* next port in the    */
    GT_U16             cpuVlanId;                    /* vlan for CPU send   */
    GT_U32             byteOff;
    GT_U32             bitOff;

    firstVlanId = DFCDL_FIRST_USED_VLAN + dfcdlTuneDb.firstPortNum;
    lastVlanId  = DFCDL_FIRST_USED_VLAN + dfcdlTuneDb.lastPortNum;

    cpuVlanId   = (GT_U16)(cpuVlan + dfcdlTuneDb.firstPortNum);

    /* Setting the VLAN information */
    vlanInfo.hasLocals  = GT_TRUE;
    vlanInfo.hasUplinks = GT_FALSE;
    vlanInfo.isCpuMember  = GT_FALSE;
    vlanInfo.learnEnable  = GT_TRUE;
    vlanInfo.unregMcFilterCmd = CPSS_UNREG_MC_VLAN_FRWD_E;
    vlanInfo.perVlanSaMacEnabled = GT_FALSE;
    vlanInfo.stpId = 0;

    if (add == GT_TRUE)
    {
        /* add the first port to the CPU send packet VLAN */
        /* Clear the bitmap array */
        cpssOsMemSet(portInfoList, 0,sizeof(portInfoList));

        byteOff = dfcdlTuneDb.firstPortNum/4;
        bitOff = (dfcdlTuneDb.firstPortNum%4) * 2;
        portInfoList[byteOff] = portInfoList[byteOff] | (1 << bitOff);

        /* Add the vlan */
        rc = writeVlanEntry(cpuVlanId,devNum,portInfoList,vlanInfo,devBitmap);
        RETURN_ON_ERROR(rc)

        if (GT_FALSE == portChain)
        {
            /* Clear the bitmap array */
            cpssOsMemSet(&portInfoList, 0,sizeof(portInfoList));

            /* Set the port bit map */
            for (portNum = dfcdlTuneDb.firstPortNum ;
                 portNum <= dfcdlTuneDb.lastPortNum ; portNum++)
            {
                byteOff = portNum/4;
                bitOff = (portNum%4) * 2;
                portInfoList[byteOff] = portInfoList[byteOff] | (1 << bitOff);
            }

            /* Add the ports to all the vlans */
            for (vlanId = firstVlanId ; vlanId <= lastVlanId; vlanId++ )
            {
                /* Delete the vlan */
                rc = writeVlanEntry(vlanId,devNum,portInfoList,vlanInfo,devBitmap);
                RETURN_ON_ERROR(rc)
            }
        }
        else
        {
            for (vlanId = firstVlanId,portNum = dfcdlTuneDb.firstPortNum ;
                  vlanId <= lastVlanId; vlanId++,portNum++ )
            {

                if (GT_TRUE == dfcdlTuneDb.extTraffic)
                {
                    if (vlanId == lastVlanId)
                    {
                        /* for external don't connect the last port to
                           the first one */
                        break;
                    }
                }
                else
                {
                    /* connect the last port to the first one according to
                       lastToFirst flag */
                    if ((vlanId == lastVlanId) && (GT_FALSE == lastToFirst))
                    {
                        break;
                    }
                }

                /* Clear the bitmap array */
                cpssOsMemSet(&portInfoList, 0,sizeof(portInfoList));

                /* for each vlan add only 2 ports - current port and the next
                   port */
                byteOff = portNum/4;
                bitOff = (portNum%4) * 2;
                portInfoList[byteOff] = portInfoList[byteOff] | (1 << bitOff);

                /* found next port */
                if (portNum == dfcdlTuneDb.lastPortNum)
                {
                    nextPortNum = dfcdlTuneDb.firstPortNum;
                }
                else
                {
                    nextPortNum = portNum + 1;
                }

                byteOff = nextPortNum/4;
                bitOff = (nextPortNum%4) * 2;
                portInfoList[byteOff] = portInfoList[byteOff] | (1 << bitOff);

                /* Add the vlan */
                rc = writeVlanEntry(vlanId,devNum,portInfoList,vlanInfo,devBitmap);
                RETURN_ON_ERROR(rc)

                if (GT_TRUE == dfcdlTuneDb.extTraffic)
                {
                    /* for external traffic - the use external wires to connect
                       the next port to his next port */
                    vlanId++;
                    portNum++;
                }
            }
        }
    }
    else
    {
        /* Delete the vlans */
        for (vlanId = firstVlanId ; vlanId <= lastVlanId; vlanId++ )
        {
            /* Delete the VLAN */
            rc = cpssExMxBrgVlanEntryInvalidate(devNum,vlanId);
            RETURN_ON_ERROR(rc)
        }

        /* delete CPU vlan */
        rc = cpssExMxBrgVlanEntryInvalidate(devNum,cpuVlanId);
        RETURN_ON_ERROR(rc)
    }

    return (GT_OK);
}

/*******************************************************************************
* dfcdlSendInternalPackets
*
* DESCRIPTION:
*       Generate traffic from the CPU.
*
* INPUTS:
*       devNum - Device number.
*       start  - GT_TRUE  - Start sending packets.
*                GT_FALSE - Stop sending packets.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS dfcdlSendInternalPackets
(
    IN  GT_U8   devNum,
    IN  GT_BOOL start
)
{
    GT_STATUS       rc;         /* Return code   */
    static GT_U8    dev;        /* Device number */

    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);

    dev = devNum;

    /* Start the task that sends packets */
    if (start == GT_TRUE)
    {
        dfcdlTuneDb.intTraff.stopSendingFlag = GT_FALSE;

        /* Start the task */
        rc = cpssOsTaskCreate("dfcdlPcktTrans",
                              DFCDL_PKT_SEND_THREAD_PRIO,
                              DFCDL_PKT_SEND_THREAD_STACK_SIZE,
                              dfcdlSendTrafficTask,
                              &dev,
                              &(dfcdlTuneDb.intTraff.taskId));
        RETURN_ON_ERROR(rc);

    }
    /* Start the task that sends packets */
    else
    {
        dfcdlTuneDb.intTraff.stopSendingFlag = GT_TRUE;
    }

    return (GT_OK);
} /* dfcdlSendInternalPackets */

/*******************************************************************************
* dfcdlSendTrafficTask
*
* DESCRIPTION:
*       This is thread function that sends packets
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       None
*
*******************************************************************************/
GT_STATUS __TASKCONV dfcdlSendTrafficTask
(
    GT_VOID * deviceNum
)
{
    GT_U32      buffLen;    /* Buffer length */
    GT_STATUS   rc;         /* Return code   */
    GT_U8       devNum;     /* Device number */
    GT_U8       queueIdx;
    GT_STATUS   retVal;
    GT_PTR      cookie;
    CPSS_EXMX_NET_TX_PARAMS_STC pcktParamsPtr;

    buffLen = dfcdlTuneDb.intTraff.pcktLength + 12 + 4; /* for mac DA & SA and CRC */
    devNum = *((GT_U8 *)deviceNum);
    cpssOsMemCpy (&(pcktParamsPtr.txParams), &(dfcdlTuneDb.intTraff.txParams),
                  sizeof (CPSS_EXMX_NET_TX_PARAMS_STC));
    pcktParamsPtr.addDsa = GT_FALSE;
    pcktParamsPtr.dest.vidx = (GT_U16)(cpuVlan + dfcdlTuneDb.firstPortNum);
    pcktParamsPtr.useVidx = GT_TRUE;

    /* Send traffic until it is stopped */
    while(dfcdlTuneDb.intTraff.stopSendingFlag == GT_FALSE)
    {
        /* First send the regular buffer */
        rc = cpssExMxNetIfTxPacketSend (devNum, &pcktParamsPtr,
                                        dfcdlTuneDb.intTraff.buffer,
                                        &buffLen, 1);
        if (rc != GT_OK)
        {
            dfcdlDebugPrint("Send failed 1\n");
        }

        /* wait for the Tx-End event */
        rc = cpssEventSelect(evReqHndl, NULL, NULL, 0);
        RETURN_ON_ERROR(rc);

        /* get all Tx end events for the packet */
        rc = cpssExMxNetIfTxBufferQueueGet(evReqHndl, &devNum, &cookie, &queueIdx,
                                   &retVal);
        RETURN_ON_ERROR(rc);

        /* Now send the inverse buffer */
        rc = cpssExMxNetIfTxPacketSend (devNum, &pcktParamsPtr,
                                        dfcdlTuneDb.intTraff.invBuffer,
                                        &buffLen, 1);
        if (rc != GT_OK)
        {
            dfcdlDebugPrint("Send failed 2\n");
        }

        /* wait for the Tx-End event */
        rc = cpssEventSelect(evReqHndl, NULL, NULL, 0);
        RETURN_ON_ERROR(rc);

        /* get all Tx end events for the packet */
        rc = cpssExMxNetIfTxBufferQueueGet(evReqHndl, &devNum, &cookie, &queueIdx,
                                   &retVal);
        RETURN_ON_ERROR(rc);
    }

    return GT_OK;
} /* dfcdlSendTrafficTask */

/*******************************************************************************
* dfcdlBuildSSOPacket
*
* DESCRIPTION:
*       Build an SSO packets. An SSO packet is build of 64 bit of "1" and then
*       64 bit of "0"
*
* INPUTS:
*       bufferLen - Length of the buffer.
*
* OUTPUTS:
*       buffer    - Buffer.
*       invBuffer - The inverse of the packet.
*
* RETURNS :
*
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlBuildSSOPacket
(
    IN  GT_U32  bufferLen,
    OUT GT_U8   *buffer,
    OUT GT_U8   *invBuffer
)
{
    GT_U32  i;              /* Loop counter                     */
    GT_U32  numOfGroups;    /* Number of groups of 8 bytes each */
    GT_U32  remBytes;       /* Number of remainig bytes         */


    numOfGroups = bufferLen / 8; /* Number of 64 bit groups */
    remBytes    = bufferLen % 8; /* Number of bytes left    */

    for (i = 1 ; i <= numOfGroups ; i++)
    {
        if ( (i % 2) == 0)
        {
            cpssOsMemSet(buffer, 0xFF, 8);
            cpssOsMemSet(invBuffer,0x0,8);
        }
        else
        {
            cpssOsMemSet(buffer, 0x00, 8);
            cpssOsMemSet(invBuffer,0xFF,8);
        }

        buffer += 8;
        invBuffer += 8;
    }

    /* Setting the remaining bytes */
    if (remBytes > 0)
    {
        if ( (numOfGroups % 2 ) == 0 )
        {
            cpssOsMemSet(buffer, 0x00, remBytes);
            cpssOsMemSet(invBuffer,0xFF,remBytes);
        }
        else
        {
            cpssOsMemSet(buffer, 0xFF,remBytes);
            cpssOsMemSet(invBuffer,0x00,remBytes);
        }
    }

    return (GT_OK);
}

/*******************************************************************************
* dfcdlBuildSS32Packet
*
* DESCRIPTION:
*       Build an SSO packets. An SSO packet is build of 32 bit of "1" and then
*       32 bit of "0"
*
* INPUTS:
*       bufferLen - Length of the buffer.
*
* OUTPUTS:
*       buffer    - Buffer.
*       invBuffer - The inverse of the packet.
*
* RETURNS :
*
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlBuildSS32Packet
(
    IN  GT_U32  bufferLen,
    OUT GT_U8   *buffer,
    OUT GT_U8   *invBuffer
)
{
    GT_U32  i;              /* Loop counter                     */
    GT_U32  numOfGroups;    /* Number of groups of 8 bytes each */
    GT_U32  remBytes;       /* Number of remainig bytes         */


    numOfGroups = bufferLen / 4; /* Number of 32 bit groups */
    remBytes    = bufferLen % 4; /* Number of bytes left    */

    for (i = 1 ; i <= numOfGroups ; i++)
    {
        if ( (i % 2) == 0)
        {
            cpssOsMemSet(buffer, 0xFF, 8);
            cpssOsMemSet(invBuffer,0x0,8);
        }
        else
        {
            cpssOsMemSet(buffer, 0x00, 8);
            cpssOsMemSet(invBuffer,0xFF,8);
        }

        buffer += 8;
        invBuffer += 8;
    }

    /* Setting the remaining bytes */
    if (remBytes > 0)
    {
        if ( (numOfGroups % 2 ) == 0 )
        {
            cpssOsMemSet(buffer, 0x00, remBytes);
            cpssOsMemSet(invBuffer,0xFF,remBytes);
        }
        else
        {
            cpssOsMemSet(buffer, 0xFF,remBytes);
            cpssOsMemSet(invBuffer,0x00,remBytes);
        }
    }

    return (GT_OK);
}


/*******************************************************************************
* dfcdlBuildAA55Packet
*
* DESCRIPTION:
*       Build an AA55 packets. An AA55 packet is build of 64 bit with the pattern
*       "AA" and then 64 bits with the pattern "55"
*
* INPUTS:
*       bufferLen - Length of the buffer.
*
* OUTPUTS:
*       buffer    - Buffer.
*       invBuffer - The inverse of the packet.
*
* RETURNS :
*
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlBuildAA55Packet
(
    IN  GT_U32  bufferLen,
    OUT GT_U8   *buffer,
    OUT GT_U8   *invBuffer
)
{
    GT_U32  i;              /* Loop counter                     */
    GT_U32  numOfGroups;    /* Number of groups of 8 bytes each */
    GT_U32  remBytes;       /* Number of remainig bytes         */


    numOfGroups = bufferLen / 8; /* Number of 64 bit groups */
    remBytes    = bufferLen % 8; /* Number of bytes left    */

    for (i = 1 ; i <= numOfGroups ; i++)
    {
        if ( (i % 2) == 0)
        {
            cpssOsMemSet(buffer, 0xAA, 8);
            cpssOsMemSet(invBuffer,0x55,8);
        }
        else
        {
            cpssOsMemSet(buffer, 0x55, 8);
            cpssOsMemSet(invBuffer,0xAA,8);
        }

        buffer += 8;
        invBuffer += 8;
    }

    /* Setting the remaining bytes */
    if (remBytes > 0)
    {
        if ( (numOfGroups % 2 ) == 0)
        {
            cpssOsMemSet(buffer, 0x55, remBytes);
            cpssOsMemSet(invBuffer,0xAA,remBytes);
        }
        else
        {
            cpssOsMemSet(buffer, 0xAA,remBytes);
            cpssOsMemSet(invBuffer,0x55,remBytes);
        }
    }

    return (GT_OK);
}

/*******************************************************************************
* dfcdlBuildZeroPacket
*
* DESCRIPTION:
*       Build an Zero packets.
*
* INPUTS:
*       bufferLen - Length of the buffer.
*
* OUTPUTS:
*       buffer    - Buffer.
*       invBuffer - The inverse of the packet.
*
* RETURNS :
*
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlBuildZeroPacket
(
    IN  GT_U32  bufferLen,
    OUT GT_U8   *buffer,
    OUT GT_U8   *invBuffer
)
{
    cpssOsMemSet(buffer, 0, bufferLen);
    cpssOsMemSet(invBuffer, 0, bufferLen);

    return (GT_OK);
}

/*******************************************************************************
* dfcdlBuildRandomPacket
*
* DESCRIPTION:
*       Build a random packets.
*
* INPUTS:
*       bufferLen - Length of the buffer.
*
* OUTPUTS:
*       buffer    - Buffer.
*       invBuffer - The inverse of the packet.
*
* RETURNS :
*
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlBuildRandomPacket
(
    IN  GT_U32  bufferLen,
    OUT GT_U8   *buffer,
    OUT GT_U8   *invBuffer
)
{
    GT_U32  i;  /* loop counter */

    for (i = 0 ; i < bufferLen; i++)
    {
        buffer[i] = (GT_U8)cpssOsRand();
        invBuffer[i] = ~buffer[i];
    }

    return (GT_OK);
}

/*******************************************************************************
* dfcdlBuildPacket
*
* DESCRIPTION:
*       Build a packet of needed pattern type.
*
* INPUTS:
*       pattType  - pattern type
*       pcktLength - Length of the packet.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS  dfcdlBuildPacket
(
    IN GT_PCK_PATTERN_TYPE  pattType,
    IN  GT_U32  pcktLength
)
{
    GT_STATUS   rc; /* return code */
    GT_U8   *buffer;
    GT_U8   *invBuffer;
    GT_U32  i;     /* iterator */

    buffer       = (GT_U8 *)cpssOsMalloc(2048);
    invBuffer    = (GT_U8 *)cpssOsMalloc(2048);

     if( (buffer == NULL) || (invBuffer == NULL) )
    {
        return (GT_OUT_OF_CPU_MEM);
    }

    /* Clear 16 bytes */
    cpssOsMemSet(buffer,0,16);
    cpssOsMemSet(invBuffer,0,16);

    /* DA address FF:FF:FF:FF:FF:FF SA address 00:00:00:00:00:FF */
    for (i = 0 ; i < 6 ; i++)
    {
        buffer[i]         = 0x00;
        invBuffer[i]      = 0x00;
        buffer[i + 6]     = 0x0;
        invBuffer[i + 6]  = 0x0;
    }

    buffer[5] = 0x55;
    invBuffer[5] = 0x55;

    buffer[11] = 0x66;
    invBuffer[11] = 0x66;

    switch (pattType)
    {
        case(GT_PATTERN_TYPE_SSO):
            rc = dfcdlBuildSSOPacket(pcktLength - 4,
                                     buffer + 16,invBuffer + 16);
            RETURN_ON_ERROR(rc);
            break;

        case(GT_PATTERN_TYPE_RANDOM):
            rc = dfcdlBuildRandomPacket(pcktLength - 4,
                                        buffer + 16,invBuffer + 16);
            RETURN_ON_ERROR(rc);
            break;

        case(GT_PATTERN_TYPE_AA55):
            rc = dfcdlBuildAA55Packet(pcktLength - 4,
                                      buffer + 16,invBuffer + 16);
            RETURN_ON_ERROR(rc);
            break;

        case (GT_PATTERN_TYPE_SS32):
            rc = dfcdlBuildSS32Packet(pcktLength - 4,
                                      buffer + 16,invBuffer + 16);
            RETURN_ON_ERROR(rc);
            break;
        case (GT_PATTERN_TYPE_ZERO):
            rc = dfcdlBuildZeroPacket(pcktLength - 4,
                                      buffer + 16,invBuffer + 16);
            RETURN_ON_ERROR(rc);
            break;

        default:
            return GT_BAD_PARAM;
    }

    cpssOsMemCpy(dfcdlTuneDb.intTraff.buffer[0],buffer,pcktLength+ 4 + 12);
    cpssOsMemCpy(dfcdlTuneDb.intTraff.invBuffer[0],invBuffer,pcktLength+ 4 + 12);

    cpssOsFree(buffer);
    cpssOsFree(invBuffer);

    return GT_OK;
}/* dfcdlBuildPacket */

/*******************************************************************************
* dfcdlAddGoodTxClkEntry
*
* DESCRIPTION:
*       Addes a valid entry of txClk.
*
* INPUTS:
*       txClkValue - TxClk Value.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error
*
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlAddGoodTxClkEntry
(
IN GT_U8    txClkValue
)
{
    dfcdlTuneDb.goodTxClkValues[dfcdlTuneDb.validTxEntries] = txClkValue;
    dfcdlTuneDb.validTxEntries++;

    return GT_OK;
} /* dfcdlAddGoodTxClkEntry */

/*******************************************************************************
* dfcdlAddGoodRxClkEntry
*
* DESCRIPTION:
*       Addes a good value of rxClk and dqs.
*
* INPUTS:
*       rxClkValue - RxClk good value.
*       dqsValue   - dqs good value for the RxClk
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error
*
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlAddGoodRxClkEntry
(
    IN GT_U8    rxClkValue,
    IN GT_U8    dqsValue
)
{
    RXCLK_VS_DQS_WINDOW  *rxClkEntry;

    rxClkEntry = &(dfcdlTuneDb.goodRxClkValues[rxClkValue]);

    /* code optimization : no need to save all the value - need only
       minimal Dqs value & maximal Dqs value */
    rxClkEntry->dqsValues[rxClkEntry->dqsValuesLength] = dqsValue;
    rxClkEntry->dqsValuesLength++;

    return (GT_OK);
} /* dfcdlAddGoodRxClkEntry */

/*******************************************************************************
* dfcdlResetCounter
*
* DESCRIPTION:
*       Reset the CRC and PCk counters.
*
* INPUTS:
*       devNum - Device number.
*       firstPortNum - First port number.
*       lastPortNum  - Last port number.
*       portCrcCounter - Array of port CRC counters.
*       portPktCounter - Array of port PKT counters.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlResetCounter
(
    IN  GT_U8    devNum,
    IN  GT_U64   *portCrcCounter,
    IN  GT_U64   *portPktCounter
)
{
    GT_U8     portNum;  /* Loop counter */
    GT_STATUS rc;       /* Return code  */
    GT_U64      totalGCrcCounter;   /* Total G CRC counters          */
    GT_U64      totalFeCrcCounter;  /* Total G CRC counters          */

    cpssOsMemSet(&totalGCrcCounter, 0, sizeof(totalGCrcCounter) );
    cpssOsMemSet(&totalFeCrcCounter, 0, sizeof(totalFeCrcCounter) );

    /* Reset all the counters before running the new confguration */
    /* The counter are reseted when they are read                 */
    for ( portNum = dfcdlTuneDb.firstPortNum ;
          portNum <= dfcdlTuneDb.lastPortNum ;
          portNum++ )
    {
        /* Reset the counters before reading*/
        portCrcCounter[portNum].l[0] = 0;
        portCrcCounter[portNum].l[1] = 0;

        portPktCounter[portNum].l[0] = 0;
        portPktCounter[portNum].l[1] = 0;

        /* Get the bad CRC counter */
        rc = cpssExMxPortMacCounterGet(devNum,portNum,CPSS_BAD_CRC_E,
                                       &(portCrcCounter[portNum] ) );
        RETURN_ON_ERROR(rc);

        /* Read the good packets counter */
        rc = cpssExMxPortMacCounterGet(devNum,portNum,CPSS_GOOD_PKTS_RCV_E,
                                       &(portPktCounter[portNum]) );
        RETURN_ON_ERROR(rc);
        if (portNum >= 48)
        {
            totalGCrcCounter = prvCpssMathAdd64(totalGCrcCounter,
                                                portCrcCounter[portNum]);
        }
        else
        {
            totalFeCrcCounter = prvCpssMathAdd64(totalFeCrcCounter,
                                                 portCrcCounter[portNum]);
        }
    }

    if (dfcdlTuneDb.lastPortNum >= 48)
    {
        /* print 48 - 52 CRC packets */
        dfcdlDebugPrint("\n****** Reset received CRC packets for G ports: "
                 "low %d, high %d \n",
                 totalGCrcCounter.l[0],totalGCrcCounter.l[1]);
        dfcdlDebugPrint("\n****** Reset received CRC packets for FE ports: "
                 "low %d, high %d \n",
                 totalFeCrcCounter.l[0],totalFeCrcCounter.l[1]);
    }
    else
    {
        dfcdlDebugPrint("\n****** Reset received CRC packets for ports: "
                 "low %d, high %d \n",
                 totalFeCrcCounter.l[0],totalFeCrcCounter.l[1]);
    }

    return GT_OK;
} /* dfcdlResetCounter */

/*******************************************************************************
* dfcdlReadCounters
*
* DESCRIPTION:
*       Read the counter and check if there were any CRC errors
*
* INPUTS:
*       devNum - Device number.
*       portCrcCounter - Array of port CRC counters.
*       portPktCounter - Array of port PKT counters.
*       tuneParams     - Current tune parameters.
*
* OUTPUTS:
*       foundGoodEntry - GT_TRUE - Found good entry.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlReadCounters
(
    IN      GT_U8                           devNum,
    IN      GT_U64                          *portCrcCounter,
    IN      GT_U64                          *portPktCounter,
    IN      DFCDL_BUFFER_DRAM_TUNE_PARAMS   *tuneParams,
    OUT     GT_BOOL                         *foundGoodEntry
)
{
    GT_U8       portNum;            /* Loop counter                     */
    GT_STATUS   rc;                 /* Return code                      */
    GT_U64      totalCrcCounter;    /* Total CRC counters               */
    GT_U64      totalPktCounter;    /* Totoal packets counters          */
    GT_U64      totalGCrcCounter;   /* Total G CRC counters          */
    /* Reset the counters */
    cpssOsMemSet(&totalCrcCounter, 0, sizeof(totalCrcCounter) );
    cpssOsMemSet(&totalPktCounter, 0, sizeof(totalPktCounter) );
    cpssOsMemSet(&totalGCrcCounter, 0, sizeof(totalGCrcCounter) );

    /* Count the number of packet recieved and number of packets */
    /* that had CRC errors                                       */
    for ( portNum = dfcdlTuneDb.firstPortNum ;
          portNum <= dfcdlTuneDb.lastPortNum ;
          portNum++ )
    {
        /* Reset the counters before reading*/
        cpssOsMemSet(&(portCrcCounter[portNum]),  0, sizeof(GT_U64) );
        cpssOsMemSet(&(portPktCounter[portNum]),  0, sizeof(GT_U64) );

        /* Count the number of sent packets to see if there is    */
        /* traffic                                                */
        rc = cpssExMxPortMacCounterGet(devNum,portNum,CPSS_GOOD_PKTS_RCV_E,
                                       &(portPktCounter[portNum]));
        RETURN_ON_ERROR(rc);

        totalPktCounter = prvCpssMathAdd64(totalPktCounter,
                                    portPktCounter[portNum]);

        /* Count the number of CRCs */
        rc = cpssExMxPortMacCounterGet(devNum,portNum,CPSS_BAD_CRC_E,
                                       &(portCrcCounter[portNum]));
        RETURN_ON_ERROR(rc);

        totalCrcCounter = prvCpssMathAdd64(totalCrcCounter,
                                    portCrcCounter[portNum]);

        if (portNum >= 48)
        {
            totalGCrcCounter = prvCpssMathAdd64(totalGCrcCounter,
                                                portCrcCounter[portNum]);
        }
    }

    /* If there was no Traffic print a Warning message */
    if ( (totalPktCounter.l[0] == 0) && (totalPktCounter.l[1] == 0) )
        dfcdlPrint("\ndevice:0x%02x \t RxClk:0x%02x \t Dqs:0x%02x \t"
                 "TxClk:0x%02x \t NO TRAFFIC!!\n",devNum,
                 tuneParams->rxClk,tuneParams->dqs,tuneParams->txClk);
    else
    {

        dfcdlPrint("\ndevice:0x%02x \t RxClk:0x%02x \t Dqs:0x%02x \t"
                 "TxClk:0x%02x \t CRC:%d\n",devNum,tuneParams->rxClk,
                 tuneParams->dqs,tuneParams->txClk,totalCrcCounter.l[0]);

        dfcdlDebugPrint("\n****** received good packets: low %d, high %d \n",
                 totalPktCounter.l[0],totalPktCounter.l[1]);

        if (dfcdlTuneDb.lastPortNum >= 48)
        {
            /* print 48 - 52 CRC packets */
            dfcdlDebugPrint("\n****** received CRC packets for G ports: "
                     "low %d, high %d \n",
                     totalGCrcCounter.l[0],totalGCrcCounter.l[1]);
        }

        /* When Getting zero CRC store the good Configuration */
        if ( (totalCrcCounter.l[0] == 0) &&
             (totalCrcCounter.l[1] == 0) )
        {
            *foundGoodEntry = GT_TRUE;

            if (dfcdlTuneDb.startQueueTxClkValues == GT_TRUE)
            {

                /* Store the good entries of the TxClk */
                rc = dfcdlAddGoodTxClkEntry(tuneParams->txClk);
                if (rc != GT_OK)
                    {
                    return (rc);
                }
            }
        }
    }

    return GT_OK;
} /* dfcdlReadCounters */

/*******************************************************************************
* dfcdlGetBestRxClkDqsValues
*
* DESCRIPTION:
*       Finds the best RxClk values and its DQS value
*
* INPUTS:
*       scanParams  - scan parameters
*
* OUTPUTS:
*   None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlGetBestRxClkDqsValues
(
    IN  DFCDL_BUFFER_DRAM_SCAN_PARAMS   *scanParams
)
{
    GT_U8   i;             /* Loop counter                  */
    GT_U8   startValue;    /* Starting point of value       */
    GT_BOOL foundRight;    /* Right point found             */
    GT_BOOL foundLeft;     /* Right point found             */
    GT_U8   dqsFirst;      /* dqs first entry               */
    GT_U8   dqsLast;       /* dqs last entry                */
    GT_U8   dqsMin, dqsMax;/* dqs min, max value            */
    GT_U8   dqsLen;        /* Number of entries in the dqs  */
    GT_U8   realDqsLen;    /* Real Dqs entries length       */
    GT_U8   maxDqsValuesLength; /* maximal DQS values length*/
    GT_U8   rxStart;       /* rx minimal scan value         */
    GT_U8   rxLast;        /* rx maximal scan value         */
    GT_U8   rightPoint = 0;/* rx right point                */
    GT_U8   leftPoint = 0; /* rx left point                 */
    GT_U8   val2;          /* used to find optimal rxClk    */
    GT_U8   val3;          /* used to find optimal rxClk    */
    GT_U8   vSize;         /* size of continuous fall       */


    foundRight = GT_FALSE;
    foundLeft = GT_FALSE;

    /* find the optimal rxClk */

    maxDqsValuesLength = 0;
    startValue = 0;


    /* find rxClk limits */
    rxStart = MIN(scanParams->rxClkStart,scanParams->rxClkEnd);
    rxLast = MAX(scanParams->rxClkStart,scanParams->rxClkEnd);

    /* rx with naximal dqs window size */
    startValue = dfcdlTuneDb.rxMaxDqs;
    dqsLen = dfcdlTuneDb.goodRxClkValues[startValue].dqsValuesLength;
    maxDqsValuesLength =
        dfcdlTuneDb.goodRxClkValues[startValue].dqsValues[dqsLen - 1] -
        dfcdlTuneDb.goodRxClkValues[startValue].dqsValues[0] + 1;



    if (GT_TRUE == dfcdlTuneDb.extSB)
    {
        /* find the left point - point that have smaller Dqs window size
           - at least threshold */
        vSize = 0;
        for (i = (startValue - 1); (i >= rxStart); i--)
        {
            dqsLen = dfcdlTuneDb.goodRxClkValues[i].dqsValuesLength;
            if(dqsLen != 0)
            {
                realDqsLen = dfcdlTuneDb.goodRxClkValues[i].dqsValues[dqsLen - 1] -
                             dfcdlTuneDb.goodRxClkValues[i].dqsValues[0] + 1;
            }
            else
            {
                realDqsLen = 0;
            }

            if (realDqsLen < (maxDqsValuesLength - scanParams->dqsThreshold))
            {
                vSize++;
                if(vSize == 3)
                {
                    leftPoint = i+3;
                    leftPoint += scanParams->stepBack;
                    foundLeft = GT_TRUE;
                    break;
                }
            }
            else
            {
                vSize = 0;
            }
        }

        /* left point exist anyway */
        if(foundLeft == GT_FALSE)
        {
            foundLeft = GT_TRUE;
            leftPoint = rxStart + vSize;
            if(vSize !=0)
            {
                leftPoint += scanParams->stepBack;
            }
        }

        rightPoint = dfcdlTuneDb.breakVal;
        foundRight = GT_TRUE;
    }
    else
    {
        /* find the right point - point that have smaller Dqs window size
           - at least threshold */
        vSize = 0;
        for (i = (startValue + 1); i <= rxLast ; i++)
        {
            dqsLen = dfcdlTuneDb.goodRxClkValues[i].dqsValuesLength;
            if(dqsLen != 0)
            {
                realDqsLen = dfcdlTuneDb.goodRxClkValues[i].dqsValues[dqsLen - 1] -
                             dfcdlTuneDb.goodRxClkValues[i].dqsValues[0] + 1;
            }
            else
            {
                realDqsLen = 0;
            }
            if (realDqsLen < (maxDqsValuesLength - scanParams->dqsThreshold))
            {
                vSize++;
                if(vSize == 3)
                {
                    rightPoint = i-3;
                    rightPoint -= scanParams->stepBack;
                    foundRight = GT_TRUE;
                    break;
                }
            }
            else
            {
                vSize = 0;
            }
        }

        /* right point exists anyway */
        if(foundRight == GT_FALSE)
        {
            foundRight = GT_TRUE;
            rightPoint = rxLast - vSize;
            if(vSize != 0)
            {
                rightPoint -= scanParams->stepBack;
            }
        }

        leftPoint = dfcdlTuneDb.breakVal;
        foundLeft = GT_TRUE;
    }



    val2 = leftPoint;
    val3 = rightPoint;

    dfcdlPrint("leftPoint = 0x%x, rightPoint = 0x%x\n",leftPoint, rightPoint);

    /* optimal rxClk = (val_3 + val2) / 2 */
    dfcdlTuneDb.bestEntryForCalc.rxClk =
        (float)(((float)val3 + (float)val2) / 2);


    if (GT_TRUE == dfcdlTuneDb.extSB)
    {
        /* external S.B */

        /* if need to find middle for even range - round down */
        dfcdlTuneDb.bestEntry.rxClk = ((val3 + val2) >> 1);
    }
    else
    {
        /* internal S.B */

        if (GT_TRUE == ((val3 + val2) & 0x1))
        {
            /* find middle for even range - round up */
            dfcdlTuneDb.bestEntry.rxClk = ((val3 + val2) >> 1) + 1;
        }
        else
        {
            dfcdlTuneDb.bestEntry.rxClk = ((val3 + val2) >> 1);
        }
    }


    /* found optimal Dqs - The middle between the two edges of the Dqs window
                           found for the selected optimal RxClk*/
    dqsMin = 255;
    dqsMax = 0;
    for(i = dfcdlTuneDb.bestEntry.rxClk - 1;
        i <= dfcdlTuneDb.bestEntry.rxClk + 1; i++)
    {
        dqsLen = dfcdlTuneDb.goodRxClkValues[i].dqsValuesLength;
        if(dqsLen == 0)
        {
            continue;
        }
        dqsFirst = dfcdlTuneDb.goodRxClkValues[i].dqsValues[0];
        dqsLast = dfcdlTuneDb.goodRxClkValues[i].dqsValues[dqsLen - 1];

        /* get minimum dqs */
        if(dqsFirst < dqsMin)
        {
            dqsMin = dqsFirst;
        }
        /* get maximum dqs */
        if(dqsLast > dqsMax)
        {
            dqsMax = dqsLast;
        }
    }

    dqsLast = dqsMax;
    dqsFirst = dqsMin;
    if (GT_TRUE == ((dqsLast - dqsFirst) & 0x1))
    {
        /* need to find middle point for even range */
        if (GT_TRUE == dfcdlTuneDb.extSB)
        {
            /* external S.B - round up */
            dfcdlTuneDb.bestEntry.dqs =
                (dqsFirst + ((dqsLast - dqsFirst) >> 1) + 1);
        }
        else
        {
            /* internal S.B. - round down */
            dfcdlTuneDb.bestEntry.dqs = dqsFirst + ((dqsLast - dqsFirst) >> 1);
        }
    }
    else
    {
        dfcdlTuneDb.bestEntry.dqs = dqsFirst + ((dqsLast - dqsFirst) >> 1);
    }

    dfcdlTuneDb.bestEntryForCalc.dqs = (float)((float)dqsFirst +
                                                (((float)dqsLast -
                                                  (float)dqsFirst) / 2));

    return (GT_OK);
} /* dfcdlGetBestRxClkDqsValues */

/*******************************************************************************
* dfcdlGetBestTxClkValues
*
* DESCRIPTION:
*   Get the best TxClk value.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlGetBestTxClkValues
(
    GT_VOID
)
{
    GT_U8   txClkFirst;
    GT_U8   txClkLast;

    txClkFirst = dfcdlTuneDb.goodTxClkValues[0];
    txClkLast = dfcdlTuneDb.goodTxClkValues[dfcdlTuneDb.validTxEntries - 1];

    dfcdlTuneDb.bestEntryForCalc.txClk = (float)txClkFirst +
        ( ( (float)txClkLast - (float)txClkFirst ) / 2 );

    dfcdlRound(dfcdlTuneDb.bestEntryForCalc.txClk,
               &dfcdlTuneDb.bestEntry.txClk);

    return (GT_OK);
} /* dfcdlGetBestTxClkValues */

/*******************************************************************************
* dfcdlDramFindPtr
*
* DESCRIPTION:
*   Find the current PVT entry in the PP or FA.
*
* INPUTS:
*       devNum   - Device number.
*       delay    - test delay.
*       devType  - Device type.
*
* OUTPUTS:
*       entryPtr - DFCDL Entry Pointer
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error
*
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlDramFindPtr
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  delay,
    IN  GT_DFCDL_DEVICE_TYPE    devType,
    OUT GT_U8                   *entryPtr

)
{
    GT_STATUS   rc = GT_OK;         /* Return code                      */
    GT_U8       entry;              /* Dfcdl entry pointer              */
    GT_U32      badDfcdlValue = 0;  /* Bad dfcdl value                  */
    GT_U32      regAddr = 0;        /* Register address                 */
    GT_U64      *portCrcCounter;    /* Ports CRC counter array          */
    GT_U64      *portPktCounter;    /* Ports packet counter array       */
    GT_U8       portNum;            /* port number loop counter         */
    GT_U64      *totalCrcCounter;   /* Sum of all CRC counters          */
    GT_U64      totalPktCounter;    /* Sum of all PKT counters          */
    GT_U64      limitCrc,limitCrc1, limitCrc2;/* limits of  CRC for PVT */
    GT_U32      i;                  /* iterator                         */
    GT_U32      ptrVal = 0;
    GT_U32      regVal;
    GT_U8       entryIf;


    dfcdlPrint("\nFinding DB If entry - Please wait...\n");

    if (devType == GT_DFCDL_PP)
    {
        /* Program the DFCDL table with the good values */
        rc = dfcdlPpDramSet(devNum,dfcdlTuneDb.bestEntry.rxClk,
                            dfcdlTuneDb.bestEntry.dqs,
                            dfcdlTuneDb.bestEntry.txClk);
    }
    else if (devType == GT_DFCDL_FA)
    {
        /* Program the DFCDL table with the good values */
        rc = dfcdlFaDramSet(devNum,dfcdlTuneDb.bestEntry.rxClk,
                            dfcdlTuneDb.bestEntry.dqs,
                            dfcdlTuneDb.bestEntry.txClk);
        if(rc != GT_OK)
        {
            return (rc);
        }

#ifdef IMPL_FA
        /* read DFCDL entry pointer for FA */
        for(i = 0; i < 19; i++)
        {
            /* Delay before checking the results */
            cpssOsTimerWkAfter(delay);
            /* read ptr value */
            rc = hwFaWriteRegister(devNum,
                   coreFaDevs[devNum]->regsAddr->globalRegs.debugSelect,
                                   0xDEB38);
            if(rc != GT_OK)
            {
                return (rc);
            }
            rc = hwFaWriteRegister(devNum,coreFaDevs[devNum]->regsAddr->
                                   globalRegs.debugControl0, 0x19);
            if(rc != GT_OK)
            {
                return (rc);
            }
            rc = hwFaReadRegister(devNum,
                    coreFaDevs[devNum]->regsAddr->globalRegs.debugBus,
                                  &regVal);
            if(rc != GT_OK)
            {
                return (rc);
            }

            dfcdlPrint("RegVal = %x\n", regVal);

            ptrVal += U32_GET_FIELD_MAC(regVal,26,6);
        }

        dfcdlRound((float)ptrVal/19.0f, &entryIf);
        dfcdlPrint("Reg DB If entry = %d\n", entryIf);
        *entryPtr = entryIf;
        return GT_OK;
#else
        return GT_FAIL;
#endif
    }

    if (rc != GT_OK)
    {
        return (rc);
    }

    /* Calculate the worse dfcdl value accordind to the S.B */
    /* use optimal rxClk value */
    U32_SET_FIELD_MAC(badDfcdlValue, 12, 6, dfcdlTuneDb.bestEntry.rxClk);
    U32_SET_FIELD_MAC(badDfcdlValue, 0, 6, 0x3f);
    U32_SET_FIELD_MAC(badDfcdlValue, 6, 6, 0x3f);

    /* Allocate memory for the ports counter CRC and PKT */
    portCrcCounter = cpssOsMalloc(sizeof(GT_U64) *
                                  (dfcdlTuneDb.lastPortNum + 1) );
    if (portCrcCounter == NULL)
    {
        return (GT_OUT_OF_CPU_MEM);
    }

    portPktCounter = cpssOsMalloc(sizeof(GT_U64) *
                                  (dfcdlTuneDb.lastPortNum + 1) );
    if (portPktCounter == NULL)
    {
        cpssOsFree(portCrcCounter);
        return (GT_OUT_OF_CPU_MEM);
    }

    if (devType == GT_DFCDL_PP)
    {
        regAddr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->extMemRegs.bdAddressReg;
    }

    if (devType == GT_DFCDL_FA)
    {
#ifdef IMPL_FA
        regAddr = coreFaDevs[devNum]->regsAddr->extMemRegs.dfcdlAddressReg;
#else
        return GT_FAIL;
#endif
    }

    /* allocate memory for the total CRC counter */
    totalCrcCounter = cpssOsMalloc(sizeof(GT_U64) * SDL_PP_SDRAM_SIZE);
    if(totalCrcCounter == NULL)
    {
        cpssOsFree(portCrcCounter);
        cpssOsFree(portPktCounter);
        return (GT_OUT_OF_CPU_MEM);
    }

    /* write SRam dfcdl address register */
    rc = cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr, 0);
    if (rc != GT_OK)
    {
        cpssOsFree(portCrcCounter);
        cpssOsFree(portPktCounter);
        cpssOsFree(totalCrcCounter);
        return (rc);
    }

    if (devType == GT_DFCDL_PP)
    {
        /* Dfcdl data register */
        regAddr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->extMemRegs.bdDataReg;
    }

    if (devType == GT_DFCDL_FA)
    {
#ifdef IMPL_FA
        /* Dfcdl data register */
        regAddr = coreFaDevs[devNum]->regsAddr->extMemRegs.dfcdlDataReg;
#else
        return GT_FAIL;
#endif
    }


    /* Go over all the entries to find the current PVT */
    for (entry = 0 ; entry < SDL_PP_SDRAM_SIZE ; entry++)
    {
        /* Write the bad dfcdl value */
        rc = cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr, badDfcdlValue);
        if (rc != GT_OK)
        {
            cpssOsFree(portCrcCounter);
            cpssOsFree(portPktCounter);
            cpssOsFree(totalCrcCounter);
            return (rc);
        }

        /* Give a delay before reseting counters to make sure that the DFCDL  */
        /* is set                                                             */
        rc = cpssOsTimerWkAfter(10);
        if ( rc != GT_OK )
        {
            cpssOsFree(portCrcCounter);
            cpssOsFree(portPktCounter);
            cpssOsFree(totalCrcCounter);
            return (rc);
        }

        /* Reset all ports counters */
        rc = dfcdlResetCounter(devNum,portCrcCounter,portPktCounter);
        if ( rc != GT_OK )
        {
            cpssOsFree(portCrcCounter);
            cpssOsFree(portPktCounter);
            cpssOsFree(totalCrcCounter);
            return (rc);
        }


        /* Delay before checking the results */
        cpssOsTimerWkAfter(delay);

        cpssOsMemSet(&totalCrcCounter[entry],0,sizeof(GT_U64) );
        cpssOsMemSet(&totalPktCounter,0,sizeof(GT_U64) );

        /* Read all ports counters */
        for (portNum = dfcdlTuneDb.firstPortNum ;
             portNum <= dfcdlTuneDb.lastPortNum ; portNum++ )
        {
            /* Read the counters */
            cpssOsMemSet(&(portCrcCounter[portNum]),0,sizeof(GT_U64) );
            cpssOsMemSet(&(portPktCounter[portNum]),0,sizeof(GT_U64) );

            /* Count the number of CRCs */
            rc = cpssExMxPortMacCounterGet(devNum,portNum,CPSS_BAD_CRC_E,
                                           &portCrcCounter[portNum]);
            if ( rc != GT_OK )
            {
                cpssOsFree(portCrcCounter);
                cpssOsFree(portPktCounter);
                cpssOsFree(totalCrcCounter);
                return (rc);
            }

            /* Sum the port counter to the Total step counter */
            totalCrcCounter[entry] = prvCpssMathAdd64(totalCrcCounter[entry],
                                                      portCrcCounter[portNum]);


            /* Count the number of sent packets to see if there is traffic */
            rc = cpssExMxPortMacCounterGet(devNum,portNum,CPSS_GOOD_PKTS_RCV_E,
                                           &portPktCounter[portNum]);
            if ( rc != GT_OK )
            {
                cpssOsFree(portCrcCounter);
                cpssOsFree(portPktCounter);
                cpssOsFree(totalCrcCounter);
                return (rc);
            }

            totalPktCounter = prvCpssMathAdd64(totalPktCounter,
                                               portPktCounter[portNum]);
        }

        /* There is no trafiic */
        if ( (totalPktCounter.l[0] == 0) && (totalPktCounter.l[1] == 0) &&
             (totalCrcCounter[entry].l[0] == 0) && (totalCrcCounter[entry].l[1] == 0))
        {
            cpssOsFree(portCrcCounter);
            cpssOsFree(portPktCounter);
            cpssOsFree(totalCrcCounter);
            return (GT_FAIL);
        }

    } /* for (entry = 0 ; entry < SDL_PP_SDRAM_SIZE ; entry++) */

    /* divide on 2 the maximal CRC counter */
    limitCrc1 = gtU64ShiftRight(totalCrcCounter[SDL_PP_SDRAM_SIZE - 1], 1);
    /* divide on 4 the maximal CRC counter */
    limitCrc2 = gtU64ShiftRight(totalCrcCounter[SDL_PP_SDRAM_SIZE - 1], 3);
    /* sum limitCrc1 and limitCrc2 */
    limitCrc = prvCpssMathAdd64(limitCrc1,limitCrc2);

    /* Found the entry */
    for(entry = 0; entry < SDL_PP_SDRAM_SIZE; entry++)
    {
       if(totalCrcCounter[entry].l[1] > limitCrc.l[1])
       {/* entry is found */
           break;
       }
       else
       {
           if((limitCrc.l[1] == 0) &&
              (totalCrcCounter[entry].l[0] > limitCrc.l[0]))
           {/* entry is found */
               break;
           }
       }
    }

    /* The entry was not found */
    if (entry == SDL_PP_SDRAM_SIZE)
    {
        cpssOsFree(portCrcCounter);
        cpssOsFree(portPktCounter);
        cpssOsFree(totalCrcCounter);
        return (GT_NOT_FOUND);
    }

    *entryPtr = entry;
    dfcdlPrint("\nDB If Entry = %d\n",*entryPtr);

    /* Free memory */
    cpssOsFree(portCrcCounter);
    cpssOsFree(portPktCounter);
    cpssOsFree(totalCrcCounter);
    return (rc);
} /* dfcdlDramFindPtr */

/*******************************************************************************
* dfcdlDramCalcSetPpTable
*
* DESCRIPTION:
*   Calculate the DFCDL table and set it to the sram
*
* INPUTS:
*       devNum    - Device number.
*       bestEntry - Best entry found in the current PVT.
*       entryPtr  - Entry number of the current PVT.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlDramCalcSetPpTable
(
    IN  GT_U8                            devNum,
    IN  GT_U8                            entryPtr
)
{
    GT_U32      value;          /* Address to set    */
    GT_U32      dfcdlEntry;     /* Dfcdl table entry */
    GT_U32      regAddr;        /* register address  */
    GT_STATUS   rc;             /* Return code       */
    float       temp;           /* rxClk value       */
    GT_U8       roundTemp;      /* Rounded value     */

    dfcdlPrint("\nCaculating DFCDL table\n");
    /* write SRam dfcdl address register */
    regAddr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->extMemRegs.bdAddressReg;
    rc = cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr, 0);
    RETURN_ON_ERROR(rc);

    /* Dfcdl data register */
    regAddr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->extMemRegs.bdDataReg;

    /* Start from 1 to save the "+1" calculation */
    for (dfcdlEntry = 0; dfcdlEntry < SDL_PP_SDRAM_SIZE; dfcdlEntry++)
    {
        dfcdlPrint("\n%02d\t", dfcdlEntry);

        value = 0;
        /* calculate the dqs value */
        temp = ((float)dfcdlTuneDb.bestEntryForCalc.dqs * (float)dfcdlEntry) /
                                                            (float)entryPtr;
        dfcdlRound(temp,&roundTemp);

        /* Maximal value 0x27 */
        if (roundTemp > 0x27)
        {
            roundTemp = 0x27;
        }

        dfcdlPrint("Dqs: 0x%02x\t", roundTemp);

        U32_SET_FIELD_MAC(value, 0, 6, roundTemp);

        /* calculate the txClk value */
        temp = ((float)dfcdlTuneDb.bestEntryForCalc.txClk *
                (float)dfcdlEntry) / (float)entryPtr;
        dfcdlRound(temp,&roundTemp);

        /* Maximal value 0x27 */
        if (roundTemp > 0x27)
        {
            roundTemp = 0x27;
        }

        dfcdlPrint("TxClk: 0x%02x\t", roundTemp);

        U32_SET_FIELD_MAC(value, 6, 6,roundTemp);

        /* calculate the rxClk value */
        temp = ((float)dfcdlTuneDb.bestEntryForCalc.rxClk *
                (float)dfcdlEntry) / (float)entryPtr;
        dfcdlRound(temp,&roundTemp);
        /* Maximal value of rxClk */
        if (roundTemp > 0x27)
        {
            roundTemp = 0x27;
        }

        dfcdlPrint("RxClk: 0x%02x\t", roundTemp);

        U32_SET_FIELD_MAC(value, 12, 6,roundTemp);

        /* Write the value */
        dfcdlPrint("RegVal: 0x%x  ", value);

        rc = cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr, value);
        RETURN_ON_ERROR(rc);

        /* Store the value in the database */
        dfcdlTuneDb.dfcdlValues[dfcdlEntry] = value;
    }

    return (GT_OK);
} /* dfcdlDramCalcSetPpTable */

/*******************************************************************************
* dfcdlDramCalcSetFaTable
*
* DESCRIPTION:
*   Calculate the DFCDL table and set it to the sram
*
* INPUTS:
*       devNum    - Device number.
*       bestEntry - Best entry found in the current PVT.
*       entryPtr  - Entry number of the current PVT.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlDramCalcSetFaTable
(
    IN  GT_U8   devNum,
    IN  GT_U8   entryPtr
)
{
#ifdef IMPL_FA
    GT_U32      value;          /* Address to set    */
    GT_U32      dfcdlEntry;     /* Dfcdl table entry */
    GT_U32      regAddr;        /* register address  */
    GT_STATUS   rc;             /* Return code       */
    float       temp;           /* rxClk value       */
    GT_U8       roundTemp;      /* Rounded value     */

    cpssOsPrintf("\nCaculating DFCDL table\n");

    /* write SRam dfcdl address register */
    regAddr = coreFaDevs[devNum]->regsAddr->extMemRegs.dfcdlAddressReg;
    rc = cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr, 0);
    RETURN_ON_ERROR(rc);

    /* Dfcdl data register */
    regAddr = coreFaDevs[devNum]->regsAddr->extMemRegs.dfcdlDataReg;

    /* Start from 1 to save the "+1" calculation */
    for (dfcdlEntry = 0; dfcdlEntry < SDL_FOX_SDRAM_SIZE; dfcdlEntry++)
    {
        dfcdlPrint("\n%02d\t", dfcdlEntry);

        value = 0;

        /* calculate the dqs value */
        temp = ((float)dfcdlTuneDb.bestEntryForCalc.dqs * (float)dfcdlEntry) /
                                                            (float)entryPtr;
        dfcdlRound(temp,&roundTemp);

        /* Maximal value 0x27 */
        if (roundTemp > 0x27)
        {
            roundTemp = 0x27;
        }

        dfcdlPrint("Dqs: 0x%02x\t", roundTemp);

        U32_SET_FIELD_MAC(value, 0, 6, roundTemp);

        /* calculate the txClk value */
        temp = ((float)dfcdlTuneDb.bestEntryForCalc.txClk *
                (float)dfcdlEntry) / (float)entryPtr;
        dfcdlRound(temp,&roundTemp);

        /* Maximal value 0x27 */
        if (roundTemp > 0x27)
        {
            roundTemp = 0x27;
        }

        dfcdlPrint("TxClk: 0x%02x\t", roundTemp);

        U32_SET_FIELD_MAC(value, 6, 6,roundTemp);

        /* calculate the rxClk value */
        temp = ((float)dfcdlTuneDb.bestEntryForCalc.rxClk *
                (float)dfcdlEntry) / (float)entryPtr;
        dfcdlRound(temp,&roundTemp);
        /* Maximal value of rxClk */
        if (roundTemp > 0x27)
        {
            roundTemp = 0x27;
        }

        dfcdlPrint("RxClk: 0x%02x\t", roundTemp);

        U32_SET_FIELD_MAC(value, 12, 6,roundTemp);

        /* Write the value */
        dfcdlPrint("RegVal: 0x%x  ", value);

        rc = cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr, value);
        RETURN_ON_ERROR(rc);

        /* Store the value in the database */
        dfcdlTuneDb.dfcdlValues[dfcdlEntry] = value;
    }

    return (GT_OK);
#else
    return GT_FAIL;
#endif
} /* dfcdlDramCalcSetFaTable */

/*******************************************************************************
* gtDiagDFCDLBufferDramAutoTune
*
* DESCRIPTION:
*       High level function to run Marvell Auto Tuner utility.
*
*       The most parameters are taken by default values. If it's needed
*       specify parameters differ from defaults user should configure and
*       run others functions.
*
* INPUTS:
*       devNum          - The device number to do the scaning.
*       devType         - The device type
*       pattType        - pattern type of traffic
*       externalSB      - GT_TRUE - external S.B., GT_FALSE - internal S.B.
*       rxHPOffsExt     - the offset taken from RxClk hang point in External
*                         S.B. mode.
*       rxHPOffsInt     - the offset taken from RxClk hang point in Internal
*                         S.B. mode.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS gtDiagDFCDLBufferDramAutoTune
(
    IN GT_U8                   devNum,
    IN GT_DFCDL_DEVICE_TYPE    devType,
    IN GT_PCK_PATTERN_TYPE     pattType,
    IN GT_BOOL                 externalSB,
    IN GT_U32                  rxHPOffsExt,
    IN GT_U32                  rxHPOffsInt
)
{
    GT_STATUS   rc = GT_OK;
    GT_U8       rxStart, rxEnd;
    GT_U8       dqsStart = 0, dqsEnd = 0x14;
    GT_U8       txClkStart = 0, txClkEnd = 0x14;
    static GT_BOOL configured = GT_FALSE;
    DFCDL_BUFFER_DRAM_SCAN_PARAMS scanParams;
    GT_U8 lastPortNum;

    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);

    lastPortNum = PRV_CPSS_PP_MAC(devNum)->numOfPorts - 1;

    switch (devType)
    {
        case GT_DFCDL_PP :
        case GT_DFCDL_FA :
            {
                break;
            }
        default:
            {
                cpssOsPrintf("**** Device type error, can not continue.\n");
                return GT_BAD_PARAM;
            }
    } /* switch */

    switch ( pattType )
    {
        case GT_PATTERN_TYPE_SSO    :
        case GT_PATTERN_TYPE_RANDOM :
        case GT_PATTERN_TYPE_AA55   :
        case GT_PATTERN_TYPE_SS32   :
        case GT_PATTERN_TYPE_ZERO   :
            {
                break;
            }
        default:
            {
                dfcdlPrint("NOTE: Packet type error was was changed to use random\n");
                pattType = GT_PATTERN_TYPE_RANDOM;
                break;
            }
    } /* switch */

    /* set rxHangPointOffsets */
    rxHangPointOffsExt = rxHPOffsExt;
    rxHangPointOffsInt = rxHPOffsInt;

    cpssOsPrintf("\nFirst port = %d, Last port = %d\n", 0,
                 PRV_CPSS_PP_MAC(devNum)->numOfPorts - 1);

    if(configured == GT_FALSE)
    {
        /* scan config */
        rc = gtDfcdlScanConfig(devNum, 0, lastPortNum, 0, GT_TRUE, externalSB);
        if(rc != GT_OK)
        {
            cpssOsPrintf("Scan config failed!\n");
            return rc;
        }
        /* config internal traffic */
        rc = gtDfcdlConfigInternalTraffic(devNum,pattType, BUFFER_DRAM_DEFAULT_PKT_SIZE);
        if(rc != GT_OK)
        {
            cpssOsPrintf("Config internal traffic failed!\n");
            return rc;
        }
        configured = GT_TRUE;
    }
    else
    {
        /* pattern type maybe updated */
        dfcdlTuneDb.intTraff.pattType = pattType;
        dfcdlBuildPacket(pattType,dfcdlTuneDb.intTraff.pcktLength);
    }

    dfcdlTuneDb.extSB = externalSB;
    /* run auto tuner */
    if(externalSB == GT_TRUE)
    {
        rxStart = DFCDL_MIN_DELAY;
        rxEnd = DFCDL_MAX_DELAY - 1;
    }
    else
    {
        rxStart = DFCDL_MAX_DELAY - 1;
        rxEnd = DFCDL_MIN_DELAY;
    }

    if(pattType != GT_PATTERN_TYPE_SSO)
    {
        /* enlarge range of scan */
        if(dqsStart >= ENLARGE_WIN)
        {
            dqsStart -= ENLARGE_WIN;
        }
        else
        {
            dqsStart = 0;
        }
        dqsEnd = MIN(DFCDL_MAX_DELAY - 1, dqsEnd + ENLARGE_WIN);

        if(txClkStart >= ENLARGE_WIN)
        {
            txClkStart -= ENLARGE_WIN;
        }
        else
        {
            txClkStart = 0;
        }
        txClkEnd = MIN(DFCDL_MAX_DELAY - 1, txClkEnd + ENLARGE_WIN);

    }
    cpssOsPrintf("RxClk scan from 0x%x to 0x%x\n", rxStart,rxEnd);
    cpssOsPrintf("Dqs scan from 0x%x to 0x%x\n", dqsStart, dqsEnd);
    cpssOsPrintf("TxClk scan from 0x%x to 0x%x\n", txClkStart, txClkEnd);

    if(devType == GT_DFCDL_PP)
    {
        cpssOsPrintf("PP DB Interface under test\n");
        /*rc = debugDfcdlPpAutoScan(devNum,rxStart,rxEnd,dqsStart, dqsEnd,
                                  txClkStart, txClkEnd, 50);*/
        scanParams.dqsEnd = dqsEnd;
        scanParams.dqsStart = dqsStart;
        scanParams.rxClkEnd = rxEnd;
        scanParams.rxClkStart = rxStart;
        scanParams.testDelay = BUFFER_DRAM_DEFAULT_SLEEP;
        scanParams.txClkEnd = txClkEnd;
        scanParams.txClkStart = txClkStart;
        scanParams.dqsThreshold = (GT_U8)dqsThreshold;
        scanParams.stepBack = (GT_U8)stepBack;

        rc = gtDfcdlPpAutoScan(devNum,&scanParams);
    }
    else if(devType == GT_DFCDL_FA)
    {
        cpssOsPrintf("FA DB Interface under test\n");
        /*rc = debugDfcdlFaAutoScan(devNum,rxStart,rxEnd,dqsStart, dqsEnd,
                                  txClkStart, txClkEnd, 50);*/
        scanParams.dqsEnd = dqsEnd;
        scanParams.dqsStart = dqsStart;
        scanParams.rxClkEnd = rxEnd;
        scanParams.rxClkStart = rxStart;
        scanParams.testDelay = BUFFER_DRAM_DEFAULT_SLEEP;
        scanParams.txClkEnd = txClkEnd;
        scanParams.txClkStart = txClkStart;
        scanParams.dqsThreshold = 2;
        scanParams.stepBack = 2;

        rc = gtDfcdlFaAutoScan(devNum,&scanParams);
    }

    RETURN_ON_ERROR(rc);

    return GT_OK;

} /* gtDiagDFCDLBufferDramAutoTune */

/*******************************************************************************
* dfcdlGetPortMaxSpeed
*
* DESCRIPTION:
*   Get a device port's speed.
*
* INPUTS:
*       devNum       - Device number.
*       portNum      - the required port number speed
*
* OUTPUTS:
*       maxPortSpeed - the maximum speed of the port
*
* RETURNS :
*       GT_OK             - Operation succeeded
*
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlGetPortMaxSpeed
(
    IN  GT_U8                 devNum,
    IN  GT_U8                 portNum,
    OUT CPSS_PORT_SPEED_ENT * maxPortSpeed
)
{
#ifdef DFCDL_DEBUG
    static GT_BOOL             first         = GT_TRUE;
    static CPSS_PORT_SPEED_ENT lastPortSpeed = CPSS_PORT_SPEED_10_E;
#endif /* DFCDL_DEBUG */

    if ( PRV_CPSS_MAX_PP_PORTS_NUM_CNS <= portNum )
    {
        return GT_BAD_PARAM;
    }

    switch (PRV_CPSS_PP_MAC(devNum)->numOfPorts)
    {
        case 1:
            *maxPortSpeed = CPSS_PORT_SPEED_10000_E;
            break;

        case 12:
            *maxPortSpeed = CPSS_PORT_SPEED_1000_E;
            break;

        case 52:
            if (portNum <= 47)
            {
                *maxPortSpeed = CPSS_PORT_SPEED_100_E;
            }
            else
            {
                *maxPortSpeed = CPSS_PORT_SPEED_1000_E;
            }
            break;

        default:
            return GT_NOT_SUPPORTED;
    }

#ifdef DFCDL_DEBUG
    {
        char * speed;
        switch ( * maxPortSpeed )
        {
            case CPSS_PORT_SPEED_10_E:
                {
                    speed = "10";
                    break;
                }
            case CPSS_PORT_SPEED_100_E:
                {
                    speed = "100";
                    break;
                }
            case CPSS_PORT_SPEED_1000_E:
                {
                    speed = "1,000";
                    break;
                }
            case CPSS_PORT_SPEED_10000_E:
                {
                    speed = "10,000";
                    break;
                }
            default:
                {
                    speed = "ERROR";
                    break;
                }
        }
        if ( GT_TRUE == first )
        {
            first = GT_FALSE;
            lastPortSpeed = * maxPortSpeed;
            dfcdlDebugPrint("device# %-2d port# %-2d speed == %sMbps\n", devNum, portNum, speed);
        }
        else
        {
            if ( lastPortSpeed != * maxPortSpeed )
            {
                lastPortSpeed = * maxPortSpeed;
                dfcdlDebugPrint("device# %-2d port# %-2d speed == %sMbps\n", devNum, portNum, speed);
            }
        }
    }
#endif /* DFCDL_DEBUG */
    return GT_OK;
} /* dfcdlGetPortMaxSpeed */

/*******************************************************************************
* dfcdlRound
*
* DESCRIPTION:
*   Round a value.
*
* INPUTS:
*   value - Value to round.
*
* OUTPUTS:
*   res   - result after round
*
* RETURNS :
*   GT_OK
*
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS dfcdlRound
(
    IN  float    value,
    OUT GT_U8   *res
)
{
    float temp;

    temp = value - ( (GT_U8) value);
    if (temp < 0.5)
    {
        *res = (GT_U8)value;
    }
    else
    {
        *res = (GT_U8)value + 1;
    }

    return (GT_OK);
}

/*******************************************************************************
* dfcdlRxClkDqsPassCriteria
*
* DESCRIPTION:
*   Check if current scan pass scaning criteria.
*
* INPUTS:
*   value - Value to round.
*
* OUTPUTS:
*   None
*
* RETURNS :
*   GT_TRUE - if pass criteria.
*   GT_FLASE - otherwise.
*
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_BOOL dfcdlRxClkDqsPassCriteria
(
    GT_VOID
)
{
    GT_U8   rxClk;          /* rxClk value */
    GT_U8   rxClkL, rxClkR; /* left and right RxClk values passed Dqs T.H. */
    GT_BOOL foundLeft;
    GT_U8   maxDqs;         /* max dqs value                */
    GT_U8   dqsRealLength;  /* real dqs values length       */
    GT_U8   dqsLength;      /* calculated dqs values length */

    /* find left and right point passed Dqs T.H. */
    foundLeft = GT_FALSE;
    maxDqs = 0;
    rxClkL = 0;
    rxClkR = 0;

    for(rxClk = 0; rxClk < DFCDL_MAX_DELAY; rxClk++)
    {
        dqsLength = dfcdlTuneDb.goodRxClkValues[rxClk].dqsValuesLength;
        if(dqsLength == 0)
        {
            continue;
        }
        dqsRealLength = dfcdlTuneDb.goodRxClkValues[rxClk].dqsValues[dqsLength-1] -
            dfcdlTuneDb.goodRxClkValues[rxClk].dqsValues[0] + 1;
        /* find maximal dqs value */
        if(dqsRealLength > maxDqs)
        {
            maxDqs = dqsRealLength;
        }
        if(dqsRealLength >= dqsPassCriteria)
        {
            if(foundLeft == GT_FALSE)
            {/* found left point */
                foundLeft = GT_TRUE;
                rxClkL = rxClkR = rxClk;
            }
            else
            {/* found right point */
                rxClkR = rxClk;
            }
        }
    }

    if(foundLeft == GT_FALSE)
    {/* dqs criteria didn't pass */
        cpssOsPrintf("Dqs window size = %d (required = %d)\n",
                 maxDqs, dqsPassCriteria);
        return GT_FALSE;
    }

    /* calc maximal rx length */
    dfcdlTuneDb.maxRx = rxClkR - rxClkL + 1;


    /* check the maximal rx window size */
    if ((rxClkR - rxClkL + 1) < (GT_U8)rxClkPassCriteria)
    {
        cpssOsPrintf("RxClk window size = %d (required = %d)\n",
                 (rxClkR - rxClkL + 1), rxClkPassCriteria);
        return GT_FALSE;
    }

    return GT_TRUE;
} /* dfcdlRxClkDqsPassCriteria */

/*******************************************************************************
* dfcdlDiag
*
* DESCRIPTION:
*
*
* INPUTS:
*
*
* OUTPUTS:
*   None
*
* RETURNS :
*   GT_TRUE - if pass criteria.
*   GT_FLASE - otherwise.
*
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS dfcdlDiag
(
    IN GT_U8                   devNum,
    IN GT_DFCDL_DEVICE_TYPE    devType,
    IN GT_U32                  size,
    IN GT_MEM_TEST_PROFILE     profile,
    IN GT_U8                   txClk,
    IN GT_U8                   dqs

)
{
    GT_U8       rxClk;
    GT_STATUS   rc;     /* return code */
    GT_U32      addr;
    GT_U32      readVal, writeVal;
    GT_BOOL     testStatus;
    GT_BOOL     skipFirst;          /* skip the first area of CRCs      */
    CPSS_DIAG_TEST_PROFILE_ENT cpssProfile;

    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);

    dfcdlConvertProfile (profile, &cpssProfile);

    /* external S.B. */
    if(devType == GT_DFCDL_PP)
    {
        rc = cpssDrvPpHwRegisterWrite(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                      PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->
                                      extMemRegs.bdTiming4Reg, 0x60000);
    }
    else
    {
#ifdef IMPL_FA
        rc = cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,coreFaDevs[devNum]->regsAddr->
                                      extMemRegs.bdTiming4Reg, 0x60000);
#else
        return GT_FAIL;
#endif
    }

    if (rc != GT_OK)
    {
        return (rc);
    }

    skipFirst = GT_TRUE;
    for(rxClk = 0x27; rxClk > 0; rxClk--)
    {
        /* Program the DFCDL table with the good values */
        if(devType == GT_DFCDL_PP)
        {
            rc = dfcdlPpDramSet(devNum,rxClk,dqs,txClk);
        }
        else
        {
            rc = dfcdlFaDramSet(devNum,rxClk,dqs,txClk);
        }

        if (rc != GT_OK)
        {
            return (rc);
        }
        cpssOsTimerWkAfter(10);

        /* run diagnostic */
        if(devType == GT_DFCDL_PP)
        {
            rc = cpssExMxDiagMemTest(devNum,CPSS_DIAG_PP_MEM_BUFFER_DRAM_E,0,
                                     size,cpssProfile,&testStatus,&addr,&readVal,
                                     &writeVal);
            if (testStatus != GT_TRUE)
            {
                rc = GT_FAIL;
            }
        }
        else
        {
#ifdef IMPL_FA
            rc = diagTestFaMem(devNum,CPSS_DIAG_PP_MEM_BUFFER_DRAM_E,0,size,
                               profile,&addr,&readVal,&writeVal);
#else
            return GT_FAIL;
#endif
        }

        if(rc != GT_OK)
        {
            if(skipFirst != GT_TRUE)
            {
                /* that's Rx hang point */
                cpssOsPrintf("External S.B. Rx hang point = %d\n", rxClk);

                if(devType == GT_DFCDL_PP)
                {
                    rc = dfcdlPpDramSet(devNum,rxClk + rxHangPointOffsExt,dqs,
                                        txClk);
                }
                else
                {
                    rc = dfcdlFaDramSet(devNum,rxClk + rxHangPointOffsExt,dqs,
                                        txClk);
                }

                if (rc != GT_OK)
                {
                    return (rc);
                }

                break;
            }
        }
        else
        {/* that's area of good Rx following after CRC points */
            skipFirst = GT_FALSE;
        }
    }

    /* set internal S.B. */
    rc = dfcdlSetSB(devNum, GT_FALSE, devType);
    if(rc != GT_OK)
    {
        return (rc);
    }

    if(devType == GT_DFCDL_PP)
    {
        cpssDrvPpHwRegisterRead(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->
                                extMemRegs.bdTiming4Reg,&readVal);
    }
    else
    {
#ifdef IMPL_FA
        cpssDrvPpHwRegisterRead(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,coreFaDevs[devNum]->regsAddr->
                                extMemRegs.bdTiming4Reg,&readVal);
#else
        return GT_FAIL;
#endif
    }

    cpssOsPrintf("Reg = 0x%x\n", readVal);
    skipFirst = GT_TRUE;
    for(rxClk = 0; rxClk < 0x27; rxClk++)
    {
        /* Program the DFCDL table with the good values */
        if(devType == GT_DFCDL_PP)
        {
            rc = dfcdlPpDramSet(devNum,rxClk,dqs,txClk);
        }
        else
        {
            rc = dfcdlFaDramSet(devNum,rxClk,dqs,txClk);
        }

        if (rc != GT_OK)
        {
            return (rc);
        }
        cpssOsTimerWkAfter(10);

        /* run diagnostic */
        if(devType == GT_DFCDL_PP)
        {
            rc = cpssExMxDiagMemTest(devNum,CPSS_DIAG_PP_MEM_BUFFER_DRAM_E,0,
                                     size,cpssProfile,&testStatus,&addr,
                                     &readVal,&writeVal);
            if (testStatus == GT_FALSE)
            {
                rc = GT_FAIL;
            }
        }
        else
        {
#ifdef IMPL_FA
            rc = diagTestFaMem(devNum,CPSS_DIAG_PP_MEM_BUFFER_DRAM_E,0,size,
                               profile,&addr,&readVal,&writeVal);
#else
            return GT_FAIL;
#endif
        }

        if(rc != GT_OK)
        {
            if(skipFirst != GT_TRUE)
            {/* that's Rx hang point */
                cpssOsPrintf("Internal S.B. Rx hang point = %d\n", rxClk);
                break;
            }
        }
        else
        {/* that's area of good Rx following after CRC points */
            skipFirst = GT_FALSE;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* gtDfcdlHelp
*
* DESCRIPTION:
*   Print help for usage DFCDL Auto tuner.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS   gtDfcdlHelp
(
    GT_VOID
)
{

cpssOsPrintf("\n\n\n"
"@@@@@ First Level Debug COMMANDS @@@@@@@@@@@\n"
"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n"
"\n");

cpssOsPrintf("***Main command*************************************************************\n"
"gtDiagDFCDLBufferDramAutoTune 0,0,0,1,5,7   {devID,PP/FA,SSO64/Rnd/AA55/SSO32,Int/Ext-SB,RxHpOffsExt,"
"RxHpOffsInt}\n\n\n");

cpssOsPrintf("***Basic commands***********************************************************\n"
"gtDfcdlScanConfig 0,11,0,1,1                               {firstport,lastport"
",int/ext traffic,1,int/ext-SB}\n\n");

cpssOsPrintf("gtDfcdlConfigInternalTraffic 0,0,1020         {dev no,SSO64/Rnd/AA55/SSO32"
"length}\n\n");

cpssOsPrintf("*** change cpu vlan & chain *************************************************\n"
"cpuVlan        {default=0x10-cpu send broadcast, "
"0x50-only for first port-chain mode only\n\n"
"portChain          {default=0-Broadcast,1-chain mode}\n\n"
"lastToFirst        {default=0, 1-last & first in the same VLAN-chain mode only}\n\n");

cpssOsPrintf("*** stop traffic ***********************************************************\n"
"stopTraffic            {default=0, 1-stop traffic before updating dfcdl table}\n\n");

cpssOsPrintf("*** Pass Criteria **********************************************************\n"
"dqsPassCriteria        {Dqs pass criteria}  "
"!!! - dqsPassCriteria must be equal orgrater dqsTreshold\n\n"
"rxClkPassCriteria      {RxClk pass criteria}\n"
"\n");

cpssOsPrintf("*** hang point frm Diag to real ********************************************\n"
"rxHangPointOffsInt     {RxClk offset from diag finding to measurement"
" start point for Internal SB}\n\n"
"rxHangPointOffsExt     {RxClk offset from diag finding to measurement"
" start point for External SB}\n\n");
cpssOsPrintf("*** dqsTreshold & StepBack *************************************************\n"
"dqsThreshold       {default=2}\n"
"StepBack           {default=2}\n"
"\n");

cpssOsPrintf("*** change port chain ******************************************************\n"
"dfcdlDebugLoop 0,0,0x8,0x16,0x8,200,10,0 {devID,PP/Fox,dqs,rx,tx,wait time,loopNum, write again}\n"
"\n");

cpssOsPrintf("*** Diag for RxHangPoint ***************************************************\n"
"dfcdlDiag 0,0,100,1,8,8            {devID,PP/Fox,PacketSize,0-Inc/1-BitTuggle/2-AA55/3-Zero/4-One,"
"TxClk,Dqs}\n"
"\nKSM\n\n\n"
);
    return GT_OK;
}

/******************************************************************************/

/*******************************************************************************
* madLoop
*
* DESCRIPTION:
*   Test function. Loop the Dfcdl Auto tuner.
*
* INPUTS:
*       devNum          - The device number to do the scaning.
*       devType         - The device type
*       pattType        - pattern type of traffic
*       externalSB      - GT_TRUE - external S.B., GT_FALSE - internal S.B.
*       rxHPOffsExt     - the offset taken from RxClk hang point in External
*                         S.B. mode.
*       rxHPOffsInt     - the offset taken from RxClk hang point in Internal
*                         S.B. mode.
* OUTPUTS:
*       None.
*
* RETURNS :
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS   madLoop
(
    IN GT_U32                  n_times,
    IN GT_U8                   devNum,
    IN GT_DFCDL_DEVICE_TYPE    devType,
    IN GT_PCK_PATTERN_TYPE     pattType,
    IN GT_BOOL                 externalSB,
    IN GT_U32                  rxHPOffsExt,
    IN GT_U32                  rxHPOffsInt
)
{
    GT_U32 i;
    GT_U32 rc;

    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);

    for (i = 0; i < n_times; i++)
    {
        rc = gtDiagDFCDLBufferDramAutoTune(devNum,devType,pattType,externalSB,
                                           rxHPOffsExt,rxHPOffsInt);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}
/*******************************************************************************
* dfcdlDebugLoop
*
* DESCRIPTION:
*   Test function. Performs a loop which reset and read the CRC counters after
*   Configuring the Dfcdl table. The input values are being set to fill the
*   DFCDL table.
*
* INPUTS:
*  GT_U8                 devNum.
*  GT_DFCDL_DEVICE_TYPE  devType    - PP of FA.
*  GT_U8                 dqs        - dqs value to set.
*  GT_U8                 rx         - rx value to set.
*  GT_U8                 tx         - tx value to set.
*  GT_U32                wait       - Wait val (msec) between reset to read
*  GT_U32                loopNum    - Iteration number.
*  GT_U32                writeAgain - Weather to write table at each iteration
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS dfcdlDebugLoop
(
    IN  GT_U8                   devNum,
    IN  GT_DFCDL_DEVICE_TYPE    devType,
    IN  GT_U8                   dqs,
    IN  GT_U8                   rx,
    IN  GT_U8                   tx,
    IN  GT_U32                  wait,
    IN  GT_U32                  loopNum,
    IN  GT_U32                  writeAgain
)
{
    GT_U64      *portCrcCounter;    /* Ports CRC counter array          */
    GT_U64      *portPktCounter;    /* Ports packet counter array       */
    GT_STATUS   rc;                 /* Return code                      */
    GT_U32      i;
    GT_BOOL     foundGoodRxClk;
    DFCDL_BUFFER_DRAM_TUNE_PARAMS tuneParams; /* Tuning parameters      */

    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);

    /* config Internal/External S.B. */
    rc = dfcdlSetSB(devNum, dfcdlTuneDb.extSB, devType);
    if (GT_OK != rc)
    {
        return rc;
    }

    /* Only in internal traffic the configuration is done */
    if (dfcdlTuneDb.extTraffic == GT_FALSE)
    {
        /* Configure the PP for the tuning */
        rc = dfcdlConfigPpForTune(devNum);
        if (rc != GT_OK)
        {
            return (rc);
        }

        if(devType == GT_DFCDL_FA)
        {
            /* Configure the Fa for the tuning */
            rc = dfcdlConfigFaForTune(devNum);
            if (rc != GT_OK)
            {
                return (rc);
            }
        }

        /* Start sending the packets */
        rc = dfcdlSendInternalPackets(devNum,GT_TRUE);
        if (rc != GT_OK)
        {
            return (rc);
        }
        cpssOsTimerWkAfter(10);
    }

    /* Allocate memory for the ports counter CRC and PKT */
    portCrcCounter = cpssOsMalloc(sizeof(GT_U64) *
                                  (dfcdlTuneDb.lastPortNum + 1) );
    if (portCrcCounter == NULL)
    {
        return (GT_OUT_OF_CPU_MEM);
    }

    portPktCounter = cpssOsMalloc(sizeof(GT_U64) *
                                  (dfcdlTuneDb.lastPortNum + 1) );
    if (portPktCounter == NULL)
    {
        return (GT_OUT_OF_CPU_MEM);
    }

    /* Program the DFCDL table with the good values */
    if (devType == GT_DFCDL_PP)
    {
        /* Program the DFCDL table with the good values */
        rc = dfcdlPpDramSet(devNum,rx,dqs,tx);
    }
    else if (devType == GT_DFCDL_FA)
    {

        /* Program the DFCDL table with the good values */
        rc = dfcdlFaDramSet(devNum,rx,dqs,tx);
    }

    if (rc != GT_OK)
    {
        return (rc);
    }

    /* Give a delay before reseting counters to make sure that    */
    /* the DFCDL is set                                           */
    cpssOsTimerWkAfter(10);

    for (i = 0; i < loopNum; i++)
    {
        if (writeAgain == GT_TRUE)
        {
            if (devType == GT_DFCDL_PP)
            {
                /* Program the DFCDL table with the good values */
                rc = dfcdlPpDramSet(devNum,rx,dqs,tx);
            }
            else if (devType == GT_DFCDL_FA)
            {

                /* Program the DFCDL table with the good values */
                rc = dfcdlFaDramSet(devNum,rx,dqs,tx);
            }

            if (rc != GT_OK)
            {
                return (rc);
            }
        }

        /* Give a delay before reseting counters to make sure that    */
        /* the DFCDL is set                                           */
        rc = cpssOsTimerWkAfter(10);
        if ( rc != GT_OK )
        {
            cpssOsFree(portCrcCounter);
            cpssOsFree(portPktCounter);
            return (rc);
        }

        /* Reset all the counters before running the new confguration */
        rc = dfcdlResetCounter(devNum,portCrcCounter,portPktCounter);
        if (rc != GT_OK)
        {
            cpssOsFree(portCrcCounter);
            cpssOsFree(portPktCounter);
            return (rc);
        }

        /* Delay before checking the results */
        rc = cpssOsTimerWkAfter(wait);
        if ( rc != GT_OK )
        {
            cpssOsFree(portCrcCounter);
            cpssOsFree(portPktCounter);
            return (rc);
        }

        /* Read the counters and update the counters */
        tuneParams.dqs   = dqs;
        tuneParams.rxClk = rx;
        tuneParams.txClk = tx;
        rc = dfcdlReadCounters(devNum,portCrcCounter,portPktCounter,
                               &tuneParams,&foundGoodRxClk);
        if (rc != GT_OK)
        {
            cpssOsFree(portCrcCounter);
            cpssOsFree(portPktCounter);
            return (rc);
        }
    }

    if (dfcdlTuneDb.extTraffic == GT_FALSE)
    {
        /* Stop sending packets */
        rc = dfcdlSendInternalPackets(devNum,GT_FALSE);
        if (rc != GT_OK)
        {
            return (rc);
        }

        /* Delete the VLANs to stop the traffic */
        rc = dfcdlAddAllPort2Vlans(devNum,GT_FALSE);
        if (rc != GT_OK)
        {
            return (rc);
        }
    }

    return (GT_OK);
}

/*******************************************************************************
* gtU64ShiftRight
*
* DESCRIPTION:
*   Shift right 64 bit number
*
* INPUTS:
*       a -  64 bit number.
*       n -  number of bits to shift
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       Result of shift.
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_U64 gtU64ShiftRight
(
    GT_U64  a,
    GT_U32  n
)
{
    GT_U64  b;

    b.l[0] = a.l[0] >> n;
    U32_SET_FIELD_MAC(b.l[0], 32 - n, n, U32_GET_FIELD_MAC(a.l[1], 0, n));
    b.l[1] = a.l[1] >> n;

    return b;
} /* gtU64ShiftRight */

/*******************************************************************************
* gtPp2FAEnG4Loop
*
* DESCRIPTION:
*       Configure the PP & FA to perform a loopback from an input port to FA
*       and back to the input port.
*
*   INPUTS:
*       GT_U32 devNum -
*       Dev Num
*
*       Pp2FAEnG4Loop_IN_Enable Enable -
*       No description available.
*
*       Pp2FAEnG4Loop_IN_isTagged isTagged -
*       No description available.
*
*   OUTPUTS:
*       None.
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       G_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtPp2FAEnG4Loop
(
    IN   GT_U8 devNum,
    IN   Pp2FAEnG4Loop_IN_Enable    Enable,
    IN   Pp2FAEnG4Loop_IN_isTagged  isTagged
)
{
#ifdef IMPL_FA
    GT_STATUS   rc; /* return code */
    GT_U8       i;  /* iterator */
    GT_U32      memDump[16];    /* memory dump */
    GT_U8       mcFport[13];    /* */
    CPSS_MAC_ENTRY_STC macEntry;/* MAC entry     */
    CPSS_PORTS_BMP_STC memPorts; /* VLAN members   */
    CPSS_PORTS_BMP_STC portsTagging; /* port is tagged */
    GT_U32       regValue;/* register value */
    CPSS_VLAN_INFO_STC cpssVlanInfo;/* cpss vlan info format */

    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);

    cpssOsMemSet(&memPorts,0,sizeof(CPSS_PORTS_BMP_STC));
    cpssOsMemSet(&portsTagging,0,sizeof(CPSS_PORTS_BMP_STC));

    for(i = 0; i < PRV_CPSS_PP_MAC(devNum)->numOfPorts; i++)
    {
        CPSS_PORTS_BMP_PORT_SET_MAC(&memPorts, i);
        if (isTagged)
        {
            CPSS_PORTS_BMP_PORT_SET_MAC(&portsTagging, i);
        }
    }

    /* configure bridge */
    /* set VLAN mode */
    rc = cpssExMxBrgFdbMacVlanLookupModeSet(devNum, CPSS_IVL_E);
    if(rc != GT_OK)
    {
        return rc;
    }
    /* set MAC Action mode */
    rc = cpssExMxBrgFdbMacTriggerModeSet(devNum, CPSS_ACT_TRIG_E, 1, 0xfff);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set MAC entry */
    cpssOsMemSet(&macEntry.macAddr, 0, sizeof(GT_ETHERADDR));
    macEntry.macAddr.arEther[5] = 0x1;
    macEntry.dstInterface.type = CPSS_INTERFACE_PORT_E;
    macEntry.dstInterface.devPort.devNum = devNum;
    macEntry.dstInterface.devPort.portNum = 0;
    macEntry.isStatic = GT_TRUE;
    macEntry.vlanId = 1;
    macEntry.srcTc = 0;
    macEntry.dstTc = 0;
    macEntry.daCommand = CPSS_MAC_TABLE_FRWRD_E;
    macEntry.saCommand = CPSS_MAC_TABLE_FRWRD_E;
    macEntry.saClass = GT_FALSE;
    macEntry.daClass = GT_FALSE;
    macEntry.saCib = GT_FALSE;
    macEntry.daCib = GT_FALSE;
    macEntry.daRoute = GT_FALSE;

    rc = cpssExMxBrgFdbMacAddrEntrySet(devNum, &macEntry);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set VLAN ports */
    cpssOsMemSet(&cpssVlanInfo,0,sizeof(CPSS_VLAN_INFO_STC));
    cpssVlanInfo.isCpuMember = GT_FALSE;
    rc = cpssExMxBrgVlanEntryWrite(devNum, 1, &memPorts, &portsTagging,
                                   &cpssVlanInfo);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* local enable/disable */
    if(Enable == Pp2FAEnG4Loop_IN_Enable_TRUE)
    {
         regValue = 0x200000;
    }
    else
    {
        regValue = 0;
    }
    for(i = 0; i < PRV_CPSS_PP_MAC(devNum)->numOfPorts; i++)
    {
        rc = cpssDrvPpHwRegBitMaskWrite(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                        PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->
                                        bridgeRegs.portControl[i], 0x200000,
                                        regValue);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* config Voq */
    for(i = 0; i < 5; i++)
    {
        rc = gtFaVoqFportTxEnable(devNum, i, GT_TRUE);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = gtFaVoqSetDevTbl(devNum, i, GT_TRUE);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* write memory */
    for(i = 0; i < sizeof(memDump)/sizeof(GT_U32); i++)
    {
        memDump[i] = 0x20202;
    }
    rc = cpssDrvPpHwRamWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,0x40800080, sizeof(memDump)/sizeof(GT_U32),
                             memDump);
    if(rc != GT_OK)
    {
        return rc;
    }
    /* config Xbar */
    rc = gtXbarFportEnable(0, 0, GT_TRUE);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = gtXbarFportEnable(0, 1, GT_TRUE);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = gtXbarFportEnable(0, 4, GT_TRUE);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* Xbar local switching enable/disable */
    rc = gtXbarLocalSwitchingEn(0, 4, Enable);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* Set Xbar unicast routing table */
    rc = gtXbarSetUcRoute(0, 4, 0, 0, 0);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = gtXbarSetUcRoute(0, 1, 4, 0, 0);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* Set MC forwarding table */
    rc = cpssDrvPpHwRegBitMaskWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,driverFaDevs[devNum]->regsAddr->
                                    faVoqRegs.voqGlobalConfig, 0x8000000,
                                    0x8000000);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDrvPpHwRegBitMaskWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,0x42140000, 0xffffffff, 0x20);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDrvPpHwRegBitMaskWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,0x42040000, 0xffffffff, 0x200);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* all packets to uplink */
    rc = cpssDrvPpHwRegBitMaskWrite(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                    PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->
                                    bufferMng.eqBlkCfgRegs.rcvSniffMirror,
                                    0xffffffff, 0x300001);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* Uc route */
    rc = gtXbarLocalSwitchingEn(0, 4, Enable);
    if(rc != GT_OK)
    {
        return rc;
    }
    rc = gtXbarSetUcRoute(0, 4, 4, 0, 0);
    if(rc != GT_OK)
    {
        return rc;
    }

    cpssOsMemSet(mcFport, 0, sizeof(mcFport));
    mcFport[0] = 4;
    rc = gtXbarSetMcRoute(0, 1, mcFport, 1);
    if(rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
#else
    return GT_FAIL;
#endif
} /* gtPp2FAEnG4Loop */

/*******************************************************************************
* PHY_88E1043_C_PATCH
*
* DESCRIPTION:
*       PHY 1043, revision C, errata fix
*
* INPUTS:
*       devNum          - PP number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS PHY_88E1043_C_PATCH
(
    IN GT_U8 devNum
)
{
    GT_U8         portNum;   /* Port number loop counter */
    GT_STATUS     rc;        /* Return code              */
    GT_U16        value;     /* Value to set to the register */
    GT_U16        ctrl;

    for ( portNum = dfcdlTuneDb.firstPortNum;
          portNum <= dfcdlTuneDb.lastPortNum;
          portNum++ )
    {
        cpssOsPrintf(">>>> START 1043C PATCH %d\n", portNum);
        rc = cpssExMxPhyPortSmiRegisterRead(devNum, portNum, (GT_U8)0, &ctrl);
        RETURN_ON_ERROR(rc);
        ctrl |= 0x8000; /* Set the reset bit */
        /* FIXME: too explicit - This treatment is for ports 0/2/3 rev C1
           errata */
        if ( (portNum != 2) && (portNum != 4) && (portNum != 9) )
        {
            value = 0x0003;
            rc = cpssExMxPhyPortSmiRegisterWrite(devNum, portNum, (GT_U8)29,
                                                 value);
            RETURN_ON_ERROR(rc);

            value = 0xE968; /* FIXME: was 0xA969 */
            rc = cpssExMxPhyPortSmiRegisterWrite(devNum, portNum, (GT_U8)30,
                                                 value);
            RETURN_ON_ERROR(rc);

            /* Write control coz of the reset */
            rc = cpssExMxPhyPortSmiRegisterWrite(devNum, portNum, (GT_U8)0x0,
                                                 ctrl);
            RETURN_ON_ERROR(rc);
        } /* end of rev C1 errata */
        /* set to fiber rgmii mode, disable fiber autoneg bypass mode */
        value = 0x8083;
        rc = cpssExMxPhyPortSmiRegisterWrite(devNum, portNum, (GT_U8)27, value);
        RETURN_ON_ERROR(rc);

        /* bit 0 - off, advertise 1000 Full Duplex only */
        value = 0x0020;
        rc = cpssExMxPhyPortSmiRegisterWrite(devNum, portNum, (GT_U8)4, value);
        RETURN_ON_ERROR(rc);

        /* set led mode */
        /*  traffic rx/tx - blink, on - link on */
        rc = cpssExMxPhyPortSmiRegisterRead(devNum, portNum, (GT_U8)24, &value);
        RETURN_ON_ERROR(rc);
        value |= 7;
        rc = cpssExMxPhyPortSmiRegisterWrite(devNum, portNum, (GT_U8)24, value);
        RETURN_ON_ERROR(rc);

        /* disable link led for sfps for D1 phy rev */
        value = 0xAA0;
        rc = cpssExMxPhyPortSmiRegisterWrite(devNum, portNum, (GT_U8)25, value);
        RETURN_ON_ERROR(rc);

        /* Reset the PHY */
        rc = cpssExMxPhyPortSmiRegisterWrite(devNum, portNum, (GT_U8)0x0, ctrl);
        RETURN_ON_ERROR(rc);

        /* The initialization of the timing control register depends on the HW.
        */
        rc = cpssExMxPhyPortSmiRegisterRead(devNum, portNum, (GT_U8)20, &value);
        RETURN_ON_ERROR(rc);
        value |= 0x0082;
        rc = cpssExMxPhyPortSmiRegisterWrite(devNum, portNum, (GT_U8)20, value);
        cpssOsPrintf("port %d wrote reg 20 value 0x%x\n",portNum, value);
        RETURN_ON_ERROR(rc);
        /* Reset the PHY */
        rc = cpssExMxPhyPortSmiRegisterWrite(devNum, portNum, (GT_U8)0x0, ctrl);
        RETURN_ON_ERROR(rc);

        /* Enable 125M Clock */
        value = 0x0068;
        rc = cpssExMxPhyPortSmiRegisterWrite(devNum, portNum, (GT_U8)16, value);
        RETURN_ON_ERROR(rc);
        /* Reset the PHY */
        rc = cpssExMxPhyPortSmiRegisterWrite(devNum, portNum, (GT_U8)0x0, ctrl);
        RETURN_ON_ERROR(rc);
    }
    return GT_OK;
} /* PHY_88E1043_C_PATCH */

/*******************************************************************************
* writeVlanEntry
*
* DESCRIPTION:
*       Builds and writes vlan entry to HW .
*
* INPUTS:
*       vid             - VLAN Id
*       devNum          - PP number
*       portInfoList    - bitmap of VLAN member ports specific information;
*                         each member occupied two bits:
*                           bit 0 is 1, if port is member of vlan;
*                           bit 1 is 1, if port is tagged;
*       vlanInfo        - VLAN specific information
*       devBitmap       - bitmap of all devices belong to vlan
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS writeVlanEntry
(
    IN GT_U16          vid,
    IN GT_U8           devNum,
    IN GT_U8           portInfoList[VLAN_PORT_BITMAP_SIZE],
    CPSS_VLAN_INFO_STC cpssVlanInfo,
    IN GT_U8           devBitmap[PP_DEV_BITMAP_SIZE]
)
{
    CPSS_PORTS_BMP_STC portsMembers;
    CPSS_PORTS_BMP_STC portsTagging;
    GT_U8       port;/* current port number */

    cpssOsMemSet(&portsMembers,0,sizeof(CPSS_PORTS_BMP_STC));
    cpssOsMemSet(&portsTagging,0,sizeof(CPSS_PORTS_BMP_STC));

    for (port = 0;
          (portInfoList) && (port < PRV_CPSS_PP_MAC(devNum)->numOfVirtPorts);
          port++)
    {
        /* set the port as member of vlan */
        if((portInfoList[(port*2) >> 3] >> ((port*2) & 0x7)) & 0x1)/* first bit */
        {
           CPSS_PORTS_BMP_PORT_SET_MAC(&portsMembers,port);
        }

        /* set the port as tagged in vlan */
        if((portInfoList[(port*2) >> 3] >> ((port*2) & 0x7)) & 0x2)/* second bit */
        {
           CPSS_PORTS_BMP_PORT_SET_MAC(&portsTagging,port);
        }
    }

    return cpssExMxBrgVlanEntryWrite(devNum, vid, &portsMembers, &portsTagging,
                                    &cpssVlanInfo);
}

