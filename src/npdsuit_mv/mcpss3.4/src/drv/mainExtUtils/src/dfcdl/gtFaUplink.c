/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtFaUplink.c
*
* DESCRIPTION:
*       This file includes functions for Fox dfcdl calibration.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <dfcdl/gtFaUplink.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpssFa/generic/gen/private/gtCoreFaStructs.h>
#include <cpssCommon/private/prvCpssMath.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>

/********* Defines ************************************************************/
#define DFCDL_FA_BUFFER_DRAM_TABLE_SIZE  (64)  /* DFCDL size FOX  BUFFER_DRAM */
#define DFCDL_FA_UPLINK_TABLE_SIZE       (31)  /* DFCDL size FOX Uplink       */
#define DFCDL_UPLINK_C_VERSION           (1)
#define DFCDL_UPLINK_FIELD_MAX_VALUE     (63)

/*
 * typedef: struct GT_DFCDL_UPLINK_DEV_DATA_STC
 *
 * Description:
 *      DFCDL uplink calibration parameters per device
 *
 * Fields:
 *      dfcdlShadowTbl - The mirror table for the device
 *      dfcdlShadowIdx - Last value of the PVT entry.
 *
 *
 * Comment:
 */
typedef struct
{
    GT_U32 dfcdlShadowTbl[DFCDL_FA_BUFFER_DRAM_TABLE_SIZE];
    GT_U32 dfcdlShadowIdx; /* Last index that was written */
} GT_DFCDL_UPLINK_DEV_DATA_STC;


/********* Local / Extern variables *******************************************/
extern CORE_FA_CONFIG * coreFaDevs[];

/* The following var will hold the number of devices that was init */
static GT_U8 sNumOfDev2Calibrate    = 0;
static GT_DFCDL_UPLINK_DEV_DATA_STC * sDevData[PRV_CPSS_MAX_PP_DEVICES_CNS] =
                                                                        {NULL};

/********* Debug variables ****************************************************/

#ifdef FA_UPLINK_DFCDL_DEBUG

#define sNumFuncCalled         sNumFuncCalled
#define sNumIdxChangeCount     sNumIdxChangeCount
#define sLastUplinkValue       sLastUplinkValue
#define sLastIdx               sLastIdx

static GT_U32  sNumFuncCalled          = 0;
static GT_U32  sNumIdxChangeCount      = 0;
static GT_U32  sLastUplinkValue        = 0;
static GT_U32  sLastIdx                = 0;

#else

#define sNumFuncCalled         0xFFFFFFFF
#define sNumIdxChangeCount     0xFFFFFFFF
#define sLastUplinkValue       0xFFFFFFFF
#define sLastIdx               0xFFFFFFFF

#endif /* FA_UPLINK_DFCDL_DEBUG */


/********* Local Functions Prototypes *****************************************/
#ifdef FA_UPLINK_DFCDL_DEBUG
#define DEBUG_UPLINK_RC
GT_STATUS uplink_stop_error(IN GT_STATUS rc)
{
    rc = rc;
    return rc;
} /* uplink_stop_error */

#endif /* FA_UPLINK_DFCDL_DEBUG */

#ifdef DEBUG_UPLINK_RC
#define DEBUG_STOP_ERROR(_rc_) uplink_stop_error(_rc_)
#else
#define DEBUG_STOP_ERROR(_rc_) (_rc_)
#endif

#define RETURN_ON_ERROR(_rc_)              \
        if (GT_OK != (_rc_))               \
        {                                  \
            return DEBUG_STOP_ERROR(_rc_); \
        }

#if ((defined FA_UPLINK_DFCDL_PRINT) || (defined FA_UPLINK_DFCDL_DEBUG))
#define printFunc         cpssOsPrintSync

#define faUplinkPrint     printFunc
#else
#define faUplinkPrint     if(0) printFunc
#endif

#ifdef FA_UPLINK_DFCDL_DEBUG
#define faUplinkDebugPrint     printFunc
#else
#define faUplinkDebugPrint     if(0) printFunc
#endif

/*******************************************************************************
* diagFaUplinkDfcdlCalibrationInitDev
*
* DESCRIPTION:
*       See header file
*
*******************************************************************************/
GT_STATUS diagFaUplinkDfcdlCalibrationInitDev
(
    IN  GT_U8                          devNum,
    IN  GT_U8                          activeDramIdx,
    IN  GT_U8                          uplinkWide,
    IN  GT_U8                          uplinkNarrow,
    IN  GT_DFCDL_UPLINK_FUNC_CURVE_ENT curve
)
{
    GT_STATUS rc;
    GT_U32  i;                    /* Loop counter         */
    GT_U8   newUplinkNarrow;      /* New uplink narrow value */
    GT_U8   newUplinkWide;        /* New uplink wide value   */
    GT_U32  dfcdlValue;           /* Dfcdl value          */
    GT_U32  tmp_val;

    if ( NULL == coreFaDevs[devNum] )
    {
        return GT_NOT_INITIALIZED;
    }

    /* I use shadow of bigger size. */
    if ( ( DFCDL_FA_BUFFER_DRAM_TABLE_SIZE <= activeDramIdx ) ||
         ( 0 == activeDramIdx ) ) /* can't use 0 of in the function. */
    {
        return GT_OUT_OF_RANGE;
    }

    if (NULL == sDevData[devNum])
    {
        sDevData[devNum] = cpssOsMalloc(sizeof(GT_DFCDL_UPLINK_DEV_DATA_STC));
    }
    /* set the dfcdl entry pointer to invalid index */
    sDevData[devNum]->dfcdlShadowIdx = 0xffffffff;

    /* Clear the shadow */
    cpssOsMemSet(sDevData[devNum]->dfcdlShadowTbl, 0,
                 sizeof(sDevData[devNum]->dfcdlShadowTbl) );

    /* Fill all the entries of the shadow */
    for ( i = 0; i < DFCDL_FA_BUFFER_DRAM_TABLE_SIZE; i++ )
    {
        /* Calculate the new uplink narrow entry */
        rc = prvCpssRoundDiv(uplinkNarrow * (i + curve),
                             activeDramIdx + curve,
                             &tmp_val);
        RETURN_ON_ERROR(rc);
        newUplinkNarrow = (GT_U8)tmp_val;

        if ( newUplinkNarrow > DFCDL_UPLINK_FIELD_MAX_VALUE )
        {
            newUplinkNarrow = DFCDL_UPLINK_FIELD_MAX_VALUE;
        }

        /* Calculate the new uplink wide entry */
        rc = prvCpssRoundDiv(uplinkWide * (i + curve),
                             activeDramIdx + curve,
                             &tmp_val);
        newUplinkWide = (GT_U8)tmp_val;
        RETURN_ON_ERROR(rc);

        if ( newUplinkWide > DFCDL_UPLINK_FIELD_MAX_VALUE )
        {
            newUplinkWide = DFCDL_UPLINK_FIELD_MAX_VALUE;
        }

        dfcdlValue = 0;
        U32_SET_FIELD_MAC(dfcdlValue, 0, 6, newUplinkWide);
        U32_SET_FIELD_MAC(dfcdlValue, 6, 6, newUplinkNarrow);
        sDevData[devNum]->dfcdlShadowTbl[i] = dfcdlValue;
        faUplinkPrint("dfcdl[%d] = 0x%x\n", i, dfcdlValue);
    } /* for */

    /* Since everything was OK till now */
    ++sNumOfDev2Calibrate;
    return GT_OK;
} /* diagFaUplinkDfcdlCalibrationInitDev */

/*******************************************************************************
* diagDfcdlUplinkTuneFuncSingleDev
*
* DESCRIPTION:
*       See header file
*
*******************************************************************************/
GT_STATUS diagDfcdlUplinkTuneFuncSingleDev
(
    IN GT_U8  devNum,
    IN GT_U32 numOfReads
)
{
    GT_STATUS rc;
    GT_U32    j;
    GT_U32    index;
    GT_U32    value;
    GT_U32    regAddr;  /* Register address */
    GT_U8     avgIdx;
    GT_U32    tmp_avg;

    if ( (NULL == sDevData[devNum]) ||
         (NULL == coreFaDevs[devNum]) )
    {
        return GT_NOT_INITIALIZED;
    }

    /* Switch to debug mode */
    regAddr = coreFaDevs[devNum]->regsAddr->globalRegs.debugSelect;
    value   = 0xdeb38;
    cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr, value);
    regAddr = coreFaDevs[devNum]->regsAddr->globalRegs.debugControl0;
    value   = 0x19;
    cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr, value);

    /* Read the PVT entry
     * How many times to read the PVT entry.
     * Since the pointer can be jittering between two intexes,
     * the index will be read few times (recommended 10-15) and
     * algebraic average will be calculated.
     */
    regAddr = coreFaDevs[devNum]->regsAddr->globalRegs.debugBus;
    for ( index = 0, j = 0; j < numOfReads; j++ )
    {
        cpssDrvPpHwRegFieldGet(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                               regAddr,
                               26, /* First bit    */
                               5,  /* Field Length */
                               &value);
        if (DFCDL_FA_BUFFER_DRAM_TABLE_SIZE <= value)
        {
            cpssOsPrintSync("Invalid FA uplink parameters\n"
                            "Consult with Marvell for new values\n");
        }
        index += value;
    }

    rc = prvCpssRoundDiv(index, numOfReads, &tmp_avg); /* get average */
    RETURN_ON_ERROR(rc);
    avgIdx = (GT_U8)tmp_avg;

    if ( sDevData[devNum]->dfcdlShadowIdx != avgIdx )
    {
#ifdef FA_UPLINK_DFCDL_DEBUG
        sLastIdx = avgIdx;
        ++sNumIdxChangeCount;
#endif /* FA_UPLINK_DFCDL_DEBUG */
        /* Set the value to the uplink table */
        regAddr = coreFaDevs[devNum]->regsAddr->globalRegs.dfcdlSramAddr;
        value   = 0;
        cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr, value);

        regAddr = coreFaDevs[devNum]->regsAddr->globalRegs.dfcdlSramData;
        value   = sDevData[devNum]->dfcdlShadowTbl[avgIdx];
#ifdef FA_UPLINK_DFCDL_DEBUG
        sLastUplinkValue = value;
#endif /* FA_UPLINK_DFCDL_DEBUG */

        for ( j = 0; j <= DFCDL_FA_UPLINK_TABLE_SIZE; j++ )
        {
            cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr, value);
        }
        sDevData[devNum]->dfcdlShadowIdx = avgIdx;
    }
    return GT_OK;
} /* diagDfcdlUplinkTuneFuncSingleDev */

/********************** DEBUG FUNCTIONS ***************************************/
GT_U32 dfcdlGetNumDevices(GT_VOID)
{
    return sNumOfDev2Calibrate;
} /* dfcdlGetTaskCount */

GT_BOOL dfcdlIsDeviceInit
(
    IN GT_U8 devNum
)
{
    return (NULL != sDevData[devNum]);
} /* dfcdlIsDeviceInit */

GT_U32 dfcdlGetTaskCount(GT_VOID)
{
    return sNumFuncCalled;
} /* dfcdlGetTaskCount */

GT_VOID dfcdlIncTaskCount(GT_VOID)
{
#ifdef FA_UPLINK_DFCDL_DEBUG
    ++sNumFuncCalled;
#endif /* FA_UPLINK_DFCDL_DEBUG */
    return;
} /* dfcdlIncTaskCount */

GT_U32 dfcdlGetIdxChangeCount(GT_VOID)
{
    return sNumIdxChangeCount;
} /* dfcdlGetIdxChangeCount */

GT_U32 dfcdlGetLastUplinkWrite(GT_VOID)
{
    return sLastUplinkValue;
} /* dfcdlGetLastUplinkWrite */

GT_U32 dfcdlGetLastIdx(GT_VOID)
{
    return sLastIdx;
} /* dfcdlGetLastIdx */

GT_STATUS dfcdlClearInfo(GT_VOID)
{
#ifdef FA_UPLINK_DFCDL_DEBUG
    sNumFuncCalled = 0;
    sNumIdxChangeCount = 0;
    sLastUplinkValue = 0;
    sLastIdx = 0;
#endif /* FA_UPLINK_DFCDL_DEBUG */
    return GT_OK;
} /* dfcdlClearInfo */

