/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtDfcdlPrv.h
*
* DESCRIPTION:
*       This file includes functions for Prestera DFCDL table tune.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __gtDfcdlPrvh
#define __gtDfcdlPrvh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/********* Include files ******************************************************/

#include <dfcdl/gtDfcdl.h>
#include <cpss/extServices/cpssExtServices.h>
#include <cpss/exMx/exMxGen/networkIf/cpssExMxGenNetIfTypes.h>
#include <cpss/generic/diag/cpssDiag.h>

/********* Defines ************************************************************/

#ifdef DFCDL_DEBUG
#define GT_DEBUG_RC
#endif

#if ((defined DFCDL_PRINT) || (defined DFCDL_DEBUG))
#define dfcdlPrint     cpssOsPrintf
#else
#define dfcdlPrint     if(0) cpssOsPrintf
#endif

#ifdef DFCDL_DEBUG
#define dfcdlDebugPrint     cpssOsPrintf
#else
#define dfcdlDebugPrint     if(0) cpssOsPrintf
#endif


#ifdef GT_DEBUG_RC
GT_STATUS debug_stop_error(IN GT_STATUS rc)
{
    rc = rc;
    return rc;
} /* debug_stop_error */

#define DEBUG_STOP_ERROR(_rc_) debug_stop_error(_rc_)
#else
#define DEBUG_STOP_ERROR(_rc_) (_rc_)
#endif

#define RETURN_ON_ERROR(_rc_)              \
        if (GT_OK != (_rc_))               \
        {                                  \
            return DEBUG_STOP_ERROR(_rc_); \
        }

#define DFCDL_TASK_PRIORITY      100                    /* Task Priority   */
#define DFCDL_TASK_STACK_SIZE    0x10000                /* Task Stack Size */

/* the default offset from the hang point in internal S.B. */
#define DFCDL_HANG_POINT_DIF_INT 6
/* the default ofsset from the hang point in external S.B. */
#define DFCDL_HANG_POINT_DIF_EXT 4

/* the default offset from the hang point in internal S.B. for TWIST12*/
#define DFCDL_HANG_POINT_DIF_INT_TWISTD12 6
/* the default ofsset from the hang point in external S.B. for TWIST12*/
#define DFCDL_HANG_POINT_DIF_EXT_TWISTD12 4

/******************************** Typedefs ************************************/

typedef enum
{
    ECC_PP_WIDE,    /* wide C2C of the Packet Processor */
    ECC_PP_NARROW,  /* narrow C2C of the Packet Processor */
    ECC_FOX_WIDE,   /* wide C2C of the Fabric Adaptor */
    ECC_FOX_NARROW  /* narrow C2C of the Fabric Adaptor */
} CPSS_C2C_INT_TYPE;

/*
 * typedef: struct DFCDL_SEND_PCKT_PARAMS
 *
 * Description:
 *      Definition of the internal packet generation.
 *
 * Fields:
 *      pattType   - Pattern of the packet
 *      pcktLength - Length of the packet
 *      buffer     - Packet to send.
 *      invBuffer  - Inverse packet to send.
 *      txParams   - Tx parameters.
 *      stopSendingFlag - Indicates if to stop the task or not.
 *      taskId          - Sending traffic task id.
 *
 * Comment:
 */
typedef struct
{
    GT_PCK_PATTERN_TYPE                    pattType;
    GT_U32                                 pcktLength;
    GT_U8                                  *buffer[1];
    GT_U8                                  *invBuffer[1];
    CPSS_EXMX_NET_TX_PARAMS_STC            txParams;
    GT_BOOL                                stopSendingFlag;
    CPSS_TASK                              taskId;

}DFCDL_SEND_PCKT_PARAMS;

/*
 * typedef: struct FLOATE_TUNE_PARAMS
 *
 * Description:
 *      Contains the parameters that are used to tune the DFCDLs
 *
 * Fields:
 *      rxClk - rxClk.
 *      txClk - txClk.
 *      dqs   - dqs
 *
 * Comment:
 */
typedef struct
{
    float   rxClk;
    float   txClk;
    float   dqs;

}FLOATE_TUNE_PARAMS;

/*
 * typedef: struct DFCDL_TUNE_DB
 *
 * Description:
 *      Contains information for the DFCDL tuning proccess.
 *
 * Fields:
 *      firstPortNumber - The first port number in the configured ports.
 *      lastPortNumber  - The last port number in the configured ports.
 *      extTraffic      - GT_TRUE - External traffic generator is used to
 *                                   generate the traffic.
 *      ppOrgValues - Original values of the PP before the test.
 *      intTraff    - Internal traffic parameters.
 *      goodTxClkValues - List of good txClk entries.
 *      validTxEntries  - Number of valid entries in the list.
 *      goodRxClkValues - List of goof RxClk values.
 *      startQueueTxClkValues - Start queuing txClk values.
 *      externalSBHangPoint - External S.B RxClk hang point.
 *      internalSBHangPoint - Internal S.B RxClk hang point.
 *      activeEntry         - the current active DFCDL table entry.
 *      extSB               - GT_TRUE - external S.B.
 *      rxMaxDqs            - rx value with maximal dqs window size
 *      maxRx               - maximal rx window size
 *      foundBreak          - break at the auto scan loop
 *      breakVal            - rx Value in which found break
 *
 * Comment:
 */
typedef struct
{
    GT_U8                           firstPortNum;
    GT_U8                           lastPortNum;
    GT_BOOL                         extTraffic;
    DFCDL_SEND_PCKT_PARAMS          intTraff;
    DFCDL_BUFFER_DRAM_TUNE_PARAMS   bestEntry;
    FLOATE_TUNE_PARAMS              bestEntryForCalc;
    GT_U8                           goodTxClkValues[DFCDL_MAX_DELAY];
    GT_U8                           validTxEntries;
    GT_BOOL                         startQueueTxClkValues;
    RXCLK_VS_DQS_WINDOW             goodRxClkValues[DFCDL_MAX_DELAY];
    GT_U32                          dfcdlValues[SDL_PP_SDRAM_SIZE];
    GT_U32                          externalSBHangPoint;
    GT_U32                          internalSBHangPoint;
    GT_U8                           activeEntry;
    GT_BOOL                         extSB;
    GT_U8                           rxMaxDqs;
    GT_U8                           maxRx;
    GT_BOOL                         foundBreak;
    GT_U8                           breakVal;
}DFCDL_TUNE_DB;

/*******************************************************************************
* PRV_CPSS_DFCDL_ECC_ERROR_FUNC
*
* DESCRIPTION:
*       ECC error function pointer.
*
* INPUTS:
*       devNum  - The device number from which this interrupt was received.
*       uniEv   - The event type.
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
typedef GT_STATUS (*PRV_CPSS_DFCDL_ECC_ERROR_FUNC)
(
    IN GT_U8        devNum,
    IN GT_U32       uniEv
);

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
);

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
    GT_VOID* deviceNum
);

/*******************************************************************************
* eccEventHandle
*
* DESCRIPTION:
*       Get ECC event and handle it.
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
*******************************************************************************/
GT_STATUS eccEventHandle
(
    GT_U32                        evReqHndl,
    GT_BOOL                       conditionParam,
    PRV_CPSS_DFCDL_ECC_ERROR_FUNC eccErrorFunc
);

/*******************************************************************************
* dfcdlCountEccInt
*
* DESCRIPTION:
*       This function counts Wide and Narrow ECC interrupts for PP and FA.
*
* INPUTS:
*       devNum   - device number.
*       waitTime - wait time (msec) defined before checking interrupt count
*       eccType  - type of ECC interrupt to count:
*
* OUTPUTS:
*       None
*
* RETURNS:
*       A positive counter value (including zero) or
*       -1 in case of error (bad parameters or general fail).
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_U32 dfcdlCountEccInt
(
    IN  GT_U8             devNum,
    IN  GT_U32            waitTime,
    IN  CPSS_C2C_INT_TYPE eccType
);

/*******************************************************************************
* dfcdlEccPpC2cCount
*
* DESCRIPTION:
*       This is an interrupt handler function. Its called upon receiving of an
*       interrupt for uplink ECC error. The function updates the wide or narrow
*       ECC error counter according to the type of interrupt.
*
* INPUTS:
*       devNum  - The device number from which this interrupt was received.
*       uniEv   - The event type.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - On success,
*       GT_FAIL - Otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS dfcdlEccPpC2cCount
(
    IN GT_U8        devNum,
    IN GT_U32       uniEv
);

/*******************************************************************************
* dfcdlScanEccEventEnable
*
* DESCRIPTION:
*   The function enable the ECC events.
*
* INPUTS:
*       devNum          - The device number.
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
GT_STATUS dfcdlScanEccEventEnable
(
    IN  GT_U8 devNum
);

/*******************************************************************************
* dfcdlConvertProfile
*
* DESCRIPTION:
*       Convert diag test profile from PSS enumerator to CPSS enumerator
*
* INPUTS:
*       profile     - PSS test profile
*
* OUTPUTS:
*       cpssProfile - CPSS test profile
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on bad input parameter
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS dfcdlConvertProfile
(
    IN  GT_MEM_TEST_PROFILE          profile,
    OUT CPSS_DIAG_TEST_PROFILE_ENT   *cpssProfile
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __gtDfcdlPrvh */

