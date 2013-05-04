/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtPrvDfcdl.c
*
* DESCRIPTION:
*       This file includes private DFCDL interfaces.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <dfcdl/private/gtDfcdlPrv.h>
#include <cpss/generic/events/cpssGenEventRequests.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>

#ifdef DFCDL_DEBUG
#define GT_DEBUG_RC
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

/* counter of ECC interrupts in PP Wide C2C */
static GT_U32 eccPpWideC2cNumber[PRV_CPSS_MAX_PP_DEVICES_CNS];
/* counter of ECC interrupts in PP Narrow C2C */
static GT_U32 eccPpNarrowC2cNumber[PRV_CPSS_MAX_PP_DEVICES_CNS];

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
)
{
    GT_U32 evBitmapArr[CPSS_UNI_EV_BITMAP_SIZE_CNS];
    GT_U32 evBitmap;
    GT_U32 i;
    GT_U32 intExtData;                               /* unused */
    GT_U8  devNum;                                   /* device number */
    GT_U32 evCauseIdx;                               /* event index */
    CPSS_UNI_EV_CAUSE_ENT uniEv;                     /* the event to handle */
    GT_STATUS rc;

    /* wait for the event */
    rc = cpssEventSelect(evReqHndl, NULL, evBitmapArr,
                         CPSS_UNI_EV_BITMAP_SIZE_CNS);
    RETURN_ON_ERROR(rc);

    if (conditionParam == GT_FALSE)
    {
        return GT_OK;
    }
    for (evCauseIdx = 0; evCauseIdx < CPSS_UNI_EV_BITMAP_SIZE_CNS;
          evCauseIdx++)
    {
        if (evBitmapArr[evCauseIdx] == 0)
        {
            continue;
        }

        evBitmap = evBitmapArr[evCauseIdx];

        for (i = 0; evBitmap; evBitmap >>= 1, i++)
        {
            if ((evBitmap & 1) == 0)
            {
                continue;
            }

            uniEv = (evCauseIdx << 5) + i;

            rc = cpssEventRecv(evReqHndl, uniEv, &intExtData, &devNum);
            RETURN_ON_ERROR(rc);

            rc = eccErrorFunc(devNum, uniEv);
            RETURN_ON_ERROR(rc);
        }
    }
    return GT_OK;
}

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
)
{
    GT_U32 *retCountValue;

    switch (eccType)
    {
        case ECC_PP_WIDE:
            eccPpWideC2cNumber[devNum] = 0;
            retCountValue = &eccPpWideC2cNumber[devNum];
            break;
        case ECC_PP_NARROW:
            eccPpNarrowC2cNumber[devNum] = 0;
            retCountValue = &eccPpNarrowC2cNumber[devNum];
            break;
        default:
            return GT_FAIL;
    }

    cpssOsTimerWkAfter(waitTime);

    return (*retCountValue);
}

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
)
{
    if ((uniEv == CPSS_PP_MISC_UPLINK_W_ECC_ON_DATA_E) ||
        (uniEv == CPSS_PP_MISC_UPLINK_W_ECC_ON_HEADER_E))
    {
        eccPpWideC2cNumber[devNum]++;
    }
    else if ((uniEv == CPSS_PP_MISC_UPLINK_N_ECC_ON_DATA_E) ||
             (uniEv == CPSS_PP_MISC_UPLINK_N_ECC_ON_HEADER_E))
    {
        eccPpNarrowC2cNumber[devNum]++;
    }

    return GT_OK;
}

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
)
{
    GT_STATUS                rc;         /* return status */

    rc = cpssEventDeviceMaskSet(devNum, CPSS_PP_MISC_C2C_DATA_ERR_E,
                                CPSS_EVENT_UNMASK_E);
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
    rc = cpssEventDeviceMaskSet(devNum, CPSS_PP_MISC_MSG_TIME_OUT_E,
                                CPSS_EVENT_UNMASK_E);
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
    rc = cpssEventDeviceMaskSet(devNum, CPSS_PP_MISC_UPLINK_W_ECC_ON_DATA_E,
                                CPSS_EVENT_UNMASK_E);
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
    rc = cpssEventDeviceMaskSet(devNum, CPSS_PP_MISC_UPLINK_W_ECC_ON_HEADER_E,
                                CPSS_EVENT_UNMASK_E);
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
    rc = cpssEventDeviceMaskSet(devNum, CPSS_PP_MISC_UPLINK_N_ECC_ON_DATA_E,
                                CPSS_EVENT_UNMASK_E);
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
    rc = cpssEventDeviceMaskSet(devNum, CPSS_PP_MISC_UPLINK_N_ECC_ON_HEADER_E,
                                CPSS_EVENT_UNMASK_E);
    if(rc == GT_NOT_FOUND)
    {
        /* this event not relevant to this device */
        rc = GT_OK;
    }

    return rc;
}

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
)
{
    switch (profile)
    {
        case GT_INCREMENTAL:
            *cpssProfile = CPSS_DIAG_TEST_INCREMENTAL_E;
            break;
        case GT_BIT_TOGGLE:
            *cpssProfile = CPSS_DIAG_TEST_BIT_TOGGLE_E;
            break;
        case GT_AA_55:
            *cpssProfile = CPSS_DIAG_TEST_AA_55_E;
            break;
        default:
            return GT_BAD_PARAM;
    }
    return GT_OK;
}

