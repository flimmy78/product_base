/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxCh3Policer.c
*
* DESCRIPTION:
*       CPSS DxCh3 Policing Engine API.
*       The Policing Engine (or Traffic Conditioner) is responsible for:
*           - Ingress Metering - implemented using SrTCM Token Bucket,
*             TrTCM Token Bucket and Two Color Mode algorithms.
*           - Egress Metering - implemented for XCAT devices
*           - Ingress Billing - byte/packet counters that keep track of amount
*             of traffic belonging to a flow.
*             Re-Marking - QoS assigned to the packet according to metering
*             results.
*           - Egress Billing - implemented for XCAT and above devices
*             Re-Marking - QoS assigned to the packet according to metering
*             results.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/


#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/policer/cpssGenPolicerTypes.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxCh3/policer/cpssDxCh3Policer.h>
#include <cpss/dxCh/dxCh3/policer/private/prvCpssDxCh3Policer.h>
#include <cpssCommon/private/prvCpssMath.h>

/* Number of DXCH3 Policer Metering/Counting Entries */
#define PRV_CPSS_DXCH3_POLICER_ENTRY_NUM_CNS                    1024
/* Max Rate ratio applied for CIR, PIR calculation */
#define PRV_CPSS_DXCH3_POLICER_RATE_RATIO_CNS                   1023

/* Max Burst Size ratio applied for CBS, PBS calculation */
#define PRV_CPSS_DXCH3_POLICER_BURST_SIZE_RATIO_CNS             65535

/* Number of rate type supported by DxCh3 Policer */
#define PRV_CPSS_DXCH3_POLICER_RATE_TYPE_MAX_CNS                6

/* Check that Policer stage is Ingress #0 or Ingress #1 */
#define PRV_CPSS_DXCH_POLICER_INGRESS_STAGE_CHECK_MAC(_stage)              \
    (CPSS_DXCH_POLICER_STAGE_EGRESS_E - (_stage))

/* Policer counting entry size in words */
#define PRV_CPSS_DXCH_POLICER_COUNT_ENTRY_SIZE_CNS             8

/* Alignment of Policy Counters indexes in the Counting entry.
   Index alignment is 8 but only first 6 indexes are valid   */
#define PRV_CPSS_DXCH_POLICER_POLICY_CNTR_INDEX_ALIGNMENT_CNS   8

/* Max Rate ratio applied for CIR, PIR calculation, for xCat and above
  devices */
#define PRV_CPSS_DXCHXCAT_POLICER_RATE_RATIO_CNS                   102300

/************************************************************************/
/*                Metering Rate & Burst Ranges  for DxCh3               */
/************************************************************************/
/*        |   Rate(Kbps) - CIR, PIR | Burst size(B) - CBS, EBS     |
--------------------------------------------------------------------------
|rateType | min    | max = min*1023 |     min    | max = min*65535 |
--------------------------------------------------------------------------
|   0     | 1      |       1,023    |     1      |       65,535    |
|   1     | 10     |      10,230    |     8      |      524,280    |
|   2     | 100    |     102,300    |     64     |    4,194,240    |
|   3     | 1000   |   1,023,000    |     512    |   33,553,920    |
|   4     | 10000  |  10,230,000    |     4096   |  268,431,360    |
|   5     | 100000 | 102,300,000    |     32768  | 2,147,450,880   |
*************************************************************************/

/************************************************************************/
/*         Metering Rate & Burst Ranges  for xCat and above         */
/************************************************************************/
/*        |   Rate(Kbps) - CIR, PIR | Burst size(B) - CBS, EBS     |
--------------------------------------------------------------------------
|rateType | min    | max = min*102300  |     min    | max = min*65535 |
--------------------------------------------------------------------------
|   0     | 1      |        102,300    |     1      |       65,535    |
|   1     | 10     |      1,023,000    |     8      |      524,280    |
|   2     | 100    |     10,230,000    |     64     |    4,194,240    |
|   3     | 1000   |    102,300,000    |     512    |   33,553,920    |
|   4     | 10000  |  1,023,000,000    |     4096   |  268,431,360    |
|   5     | 100000 | 10,230,000,000    |     32768  | 2,147,450,880   |
*************************************************************************/
static const GT_U32 prvMeterMinRateBurstRangesArray
    [PRV_CPSS_DXCH3_POLICER_RATE_TYPE_MAX_CNS][2] = {{1, 1},
                                                     {10, 8},
                                                     {100, 64},
                                                     {1000, 512},
                                                     {10000, 4096},
                                                     {100000, 32768}
                                                    };

/*******************************************************************************
* prvCpssDxCh3PolicerHwTbCirCalculate
*
* DESCRIPTION:
*       Calculate the HW values of Token Bucket and Rate Type.
*       The calculation is CIR oriented which means that it looks for
*       the range with the highest granularity the CIR can fit into.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum              - device number.
*       swRate              - Rate (CIR or PIR) in the SW format.
*       swBurstSize0        - Burst Size (CBS or PBS) in the SW format.
*       swBurstSize1        - Burst Size (EBS) in the SW format.
*                             This parameter is used only upon SrTCM.
*
* OUTPUTS:
*       hwRatePtr           - Rate (CIR or PIR) in the HW format.
*       hwBurstSize0Ptr     - Burst Size (CBS or PBS) in the HW format.
*       hwBurstSize1Ptr     - Burst Size (EBS) in the HW format.
*                             This parameter is used only upon SrTCM.
*                             Upon TrTCM it should be NULL.
*       rateTypePtr         - pointer to the Rate Type.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_FAIL         - on error.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_BAD_PARAM    - on wrong input parameters.
*
* COMMENTS:
*       Upon TrTCM the hwBurstSize1Ptr parameter should be NULL.
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3PolicerHwTbCirCalculate
(
    IN  GT_U8   devNum,
    IN  GT_U32  swRate,
    IN  GT_U32  swBurstSize0,
    IN  GT_U32  swBurstSize1,
    OUT GT_U32  *hwRatePtr,
    OUT GT_U32  *hwBurstSize0Ptr,
    OUT GT_U32  *hwBurstSize1Ptr,
    OUT GT_U32  *rateTypePtr
)
{
    GT_U32  rateType;   /* Rate Type */

    /*******************************/
    /* Calculate CIR(PIR) HW value */
    /*******************************/
    for(rateType = 0 ; rateType < PRV_CPSS_DXCH3_POLICER_RATE_TYPE_MAX_CNS;
        rateType++)
    {
        if(swRate <= (prvMeterMinRateBurstRangesArray[rateType][0] *
                                PRV_CPSS_DXCH3_POLICER_RATE_RATIO_CNS))
        {
            *hwRatePtr = swRate / prvMeterMinRateBurstRangesArray[rateType][0];
            break;
        }
    }

    if(rateType == PRV_CPSS_DXCH3_POLICER_RATE_TYPE_MAX_CNS)
    {
        /* the user rate is over the max supported */
        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            *hwRatePtr = BIT_17 - 1;  /* 17 bits max value */
        }
        else
        {
            *hwRatePtr = BIT_10 - 1;  /* 10 bits max value */
        }
        rateType = PRV_CPSS_DXCH3_POLICER_RATE_TYPE_MAX_CNS - 1;
    }

    /***********************/
    /* Store the Rate Type */
    /***********************/
    *rateTypePtr = rateType;

    /*******************************/
    /* Calculate CBS(PBS) HW value */
    /*******************************/
    if(swBurstSize0 > (prvMeterMinRateBurstRangesArray[rateType][1] *
                            PRV_CPSS_DXCH3_POLICER_BURST_SIZE_RATIO_CNS))
    {
        *hwBurstSize0Ptr = 0xFFFF;     /* 16 bits */
    }
    else
    {
        *hwBurstSize0Ptr = swBurstSize0 /
            prvMeterMinRateBurstRangesArray[rateType][1];
    }

    /*************************************************/
    /* Calculate EBS HW value (SrTCM only parameter) */
    /*************************************************/
    if (hwBurstSize1Ptr != NULL)
    {
        if(swBurstSize1 > (prvMeterMinRateBurstRangesArray[rateType][1] *
                            PRV_CPSS_DXCH3_POLICER_BURST_SIZE_RATIO_CNS))
        {
            *hwBurstSize1Ptr = 0xFFFF;      /* 16 bits */
        }
        else
        {
            *hwBurstSize1Ptr = swBurstSize1 /
                prvMeterMinRateBurstRangesArray[rateType][1];
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxCh3PolicerHwTbCbsCalculate
*
* DESCRIPTION:
*       Calculate the HW values of Token Bucket and Rate Type.
*       The calculation is CBS/PBS oriented which means that it looks for
*       the range where the CBS/PBS value will be most accurate.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum              - device number.
*       stage               - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                             Stage type is significant for DxChXcat and above devices
*                             and ignored by DxCh3.
*       swRate              - Rate (CIR/PIR) in the SW format.
*       swBurstSize0        - Burst Size (CBS/PBS) in the SW format.
*       swBurstSize1        - Burst Size (EBS) in the SW format.
*                             This parameter is used only upon SrTCM.
*
* OUTPUTS:
*       hwRatePtr           - Rate (CIR/PIR) in the HW format.
*       hwBurstSize0Ptr     - Burst Size (CBS/PBS) in the HW format.
*       hwBurstSize1Ptr     - Burst Size (EBS) in the HW format.
*                             This parameter is used only upon SrTCM.
*                             Upon TrTCM it should be NULL.
*       rateTypePtr         - pointer to the Rate Type.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_FAIL         - on error.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_BAD_PARAM    - on wrong input parameters.
*
* COMMENTS:
*       Upon TrTCM the hwBurstSize1Ptr parameter should be NULL.
*       rate\burst HW limitation: Due to HW limitation, the result must satisfy
*                                 the following relation -
*           <rate_value_in_HW> <= <burst_value_in_HW>*<burst_granularity>
*       Or in HW fields notion:
*       The value configured in <Rate0> must be configured to be less than or
*       equal to <Max Burst Size0>*Burst Granularity - MRU.
*       The value configured in <Rate1> must be configured to be less than or
*       equal to <Max Burst Size1>*Burst Granularity - MRU.
*       The Burst Granularity is a function of <Rate Type0>/<Rate Type1>.
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3PolicerHwTbCbsCalculate
(
    IN  GT_U8   devNum,
    IN  CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN  GT_U32  swRate,
    IN  GT_U32  swBurstSize0,
    IN  GT_U32  swBurstSize1,
    OUT GT_U32  *hwRatePtr,
    OUT GT_U32  *hwBurstSize0Ptr,
    OUT GT_U32  *hwBurstSize1Ptr,
    OUT GT_U32  *rateTypePtr
)
{
    GT_U32  cirPirAllowedDeviation; /* rate allowed deviation */
    GT_U32  rateType;   /* Rate Type */
    GT_U32  rateTypeRateGranularity; /* Rate Type Rate granularity */
    GT_U32  rateTypeBurstGranularity = 0; /* Rate Type Burst granularity */
    GT_U32  swHighRate; /* Highest rate based on the allowed deviation */
    GT_U32  hwLowRate;  /* Lowest rate in HW due to granularity that still conforms with SW low rate */
    GT_U32  hwHighRate; /* Highest rate in HW due to granularity that still conforms with SW high rate */
    GT_U32  policerRateRatio; /* rate range ratio between minimum (also granularity) and maximum */
    GT_U32  lastQualifiedCirRateType; /* the last rate type which qualified with the CIR limitations */
    GT_U32  lastQualifiedHwLowRate;   /* Lowest rate in HW due to granularity for the last rate type */
                                      /* which qualified with the CIR limitations */
    GT_BOOL lowRateIsZero; /* flag indicating if the lowest allowed rate is 0 */
    GT_U32  mru;           /* Policer MRU */
    GT_U32  actualMaxBucketSize; /* actual maximal bucket size. MRU is taken into account */
    GT_STATUS rc;                /* return status */

    /* get MRU */
    rc = cpssDxCh3PolicerMruGet(devNum,stage,&mru);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* For all devices the granularity of all rate types is the same, but DxCh3 has different */
    /* range (100 times smaller) for the same rate type.                                      */
    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        policerRateRatio = PRV_CPSS_DXCHXCAT_POLICER_RATE_RATIO_CNS;
    }
    else
    {
        policerRateRatio = PRV_CPSS_DXCH3_POLICER_RATE_RATIO_CNS;
    }

    cirPirAllowedDeviation = PRV_CPSS_DXCH_PP_MAC(devNum)->policer.cirPirAllowedDeviation;
    lowRateIsZero = GT_FALSE;
    if( cirPirAllowedDeviation >= 100 )
    {
        /* If allowed deviation is 100% or more, lowest acceptable\actual rate is 0 */
        /* and not negative one. */
        lowRateIsZero = GT_TRUE;
    }

    if( swRate > (0xFFFFFFFF/(cirPirAllowedDeviation + 100))*100 )
    {
        /* Maximal allowed rate is set to 0xFFFFFFFF (32 bits maximal value) in case due to the */
        /* allowed deviation the highest acceptable rate will be more then 0xFFFFFFFF.          */
        swHighRate = 0xFFFFFFFF;
    }
    else
    {
        /* The calculation for range high rate is: rate*(100+deviation)/100. In case the result of */
        /* (rate*(100+deviation)) will overflow above 0xFFFFFFFF (but still the final result is    */
        /* below 0xFFFFFFFF otherwise the if case had already set the high rate to 0xFFFFFFFF)     */
        /* some modulo arithmetic is done. Otherwise the straight forward formula is used.         */
        if( swRate >= 0xFFFFFFFF/(cirPirAllowedDeviation + 100) )
        {
            swHighRate = (swRate/100)*(cirPirAllowedDeviation + 100)
                         + ((swRate%100)*cirPirAllowedDeviation)/100;
        }
        else
        {
            swHighRate = (swRate * (cirPirAllowedDeviation + 100))/100;
        }
    }

    lastQualifiedCirRateType = PRV_CPSS_DXCH3_POLICER_RATE_TYPE_MAX_CNS;
    lastQualifiedHwLowRate = 0;

    for(rateType = 0 ; rateType < PRV_CPSS_DXCH3_POLICER_RATE_TYPE_MAX_CNS;
        rateType++)
    {
        rateTypeRateGranularity  = prvMeterMinRateBurstRangesArray[rateType][0];
        rateTypeBurstGranularity = prvMeterMinRateBurstRangesArray[rateType][1];

        /* hwLowRate & hwHighRate are the quantized range edges due to the rate type granularity. */

        if( GT_TRUE == lowRateIsZero )
        {
            /* If the lowest allowed rate value is 0 then HW value is 0 as well. */
            hwLowRate = 0;
        }
        else if ( ((swRate/policerRateRatio)*(100 - cirPirAllowedDeviation) +
                   ((swRate%policerRateRatio)*(100 - cirPirAllowedDeviation))/policerRateRatio) >
                                                                        (rateTypeRateGranularity * 100) )
        {
            /* If the lowest allowed rate value is greater than the rate type maximal rate go to the    */
            /* next rate type. The straight forward theoretical check that the maximal rate is breached  */
            /* could be: SW_low_rate > Maximal_rate_of_rate_type                                        */
            /* or respectively: sw_rate * (100 - deviation)/100 > rate_granularity_of_rate_type * rate_ratio */
            /* Due to the fact that the product (sw_rate * (100 - deviation)) might be more than 32 bits  */
            /* and the fact that division operation in integer and not fractional (i.e. (x/y)*y might not */
            /* be equal x, the calculation above use some modulo arithmetic manipulations.                */
            continue;
        }
        else
        {
            /* The calculation for range low rate is: rate*(100-deviation)/100/granularity. Since the   */
            /* rate is not continuous but quantized by the granularity, its value is rounded up to the  */
            /* nearest value. This cause the modulo arithmetic in the second part of the formulas here  */
            /* below.                                                                                   */
            /* The distinction (if case) is required for cases in which the rate value is large and the */
            /* product (rate*(100-deviation)) exceed 0xFFFFFFFF. Otherwise the straight forward formula */
            /* can be used.                                                                             */
            if( swRate > 0xFFFFFFFF/(100 - cirPirAllowedDeviation))
            {
                hwLowRate = ((swRate/100)*(100 - cirPirAllowedDeviation) +
                             ((swRate%100)*(100 - cirPirAllowedDeviation)/100))/rateTypeRateGranularity +
                            ((((swRate/100)*(100 - cirPirAllowedDeviation) +
                               ((swRate%100)*(100 - cirPirAllowedDeviation)/100))
                                                                %rateTypeRateGranularity) == 0 ? 0 : 1 );
            }
            else
            {
                hwLowRate = swRate * (100 - cirPirAllowedDeviation) / (100 * rateTypeRateGranularity) +
                            (((swRate * (100 - cirPirAllowedDeviation)) % (100 * rateTypeRateGranularity)) == 0 ? 0 : 1);
            }
        }

        if( swHighRate < rateTypeRateGranularity )
        {
            /* Allowed high rate is below the rate type granularity. */
            hwHighRate = 0;
        }
        else if ( swHighRate/policerRateRatio > rateTypeRateGranularity )
        {
            /* Allowed high rate is above the rate type maximal rate. */
            hwHighRate = policerRateRatio;
        }
        else
        {
            hwHighRate = swHighRate/rateTypeRateGranularity;
        }

        /* In case the quantized range (quantized low range edge to quantized high range edge) */
        /* is an empty group (contains no value) go to the next rate type.                     */
        if( hwLowRate > hwHighRate )
        {
            continue;
        }

        if( hwLowRate <= (0xFFFF * rateTypeBurstGranularity) )
        {
            /* This rate type can fulfill the rate requirements and the rate\burst HW limitation */
            /* without taking into consideration the burst constraint.                           */
            lastQualifiedCirRateType = rateType;
            lastQualifiedHwLowRate = hwLowRate;
        }
        else
        {
            /* Setting any allowed rate from the requested one will in this rate type will */
            /* violate the rate\burst HW limitation.                                       */
            /* Go to the next rate type.                                                   */
            continue;
        }

        *hwBurstSize0Ptr = swBurstSize0/rateTypeBurstGranularity;

        /* Check if required burst can be matched in the current rate type. */
        if( *hwBurstSize0Ptr <= 0xFFFF )
        {
            /* the MRU value should be decreased from bucket size in order to 
              get actual size of bucket. The bucket size should be more then MRU.
              The bucket size less then MRU is used for debug purpose. */
            actualMaxBucketSize = *hwBurstSize0Ptr * rateTypeBurstGranularity;
            if (actualMaxBucketSize >= mru)
            {
                actualMaxBucketSize -= mru;
            }

            if( hwLowRate <= actualMaxBucketSize )
            {
                /* This rate type can be used to fulfill all requirements:  */
                /* - rate in requested range.                               */
                /* - burst as requested (up to granularity rounding).       */
                /* - rate\burst HW limitation obeyed.                       */

                /* Set selected rate type. */
                *rateTypePtr = rateType;

                /* Calculate naive HW rate */
                *hwRatePtr = swRate / rateTypeRateGranularity;

                if( *hwRatePtr < hwLowRate )
                {
                    /* If naive HW rate calculated lower then quantized range lowest rate, */
                    /* set selected HW rate to quantized range lowest rate.                */
                    *hwRatePtr = hwLowRate;
                }
                else if( *hwRatePtr > actualMaxBucketSize )
                {
                    /* If naive HW rate calculated violate rate\burst HW limitation, set HW rate */
                    /* to the maximal value conforming the limitation.                           */
                    *hwRatePtr = actualMaxBucketSize;
                }
                break;
            }

            /* If next rate type burst granularity is higher that the requested burst, no need to go */
            /* to next rate type. If the flow arrived here a rate type which fulfill the rate        */
            /* constraints was already been found.                                                   */
            if ( (rateType < (PRV_CPSS_DXCH3_POLICER_RATE_TYPE_MAX_CNS - 1)) &&
                 (swBurstSize0 < prvMeterMinRateBurstRangesArray[rateType+1][1]) )
            {
                rateType = PRV_CPSS_DXCH3_POLICER_RATE_TYPE_MAX_CNS;
                break;
            }
        }
    }

    if( PRV_CPSS_DXCH3_POLICER_RATE_TYPE_MAX_CNS == lastQualifiedCirRateType )
    {
        /* Rate type which fulfill the rate constraints was not found. */
        return GT_FAIL;
    }
    else if( PRV_CPSS_DXCH3_POLICER_RATE_TYPE_MAX_CNS == rateType )
    {
        /* No rate type which fulfill all requirements (rate, burst, rate\burst HW limitation was found. */
        /* Rate constraints and rate\burst HW limitation can be fulfilled.                               */
        if( GT_TRUE == PRV_CPSS_DXCH_PP_MAC(devNum)->policer.cbsPbsCalcOnFail )
        {
            /* Configure with burst best effort calculation. */

            /* Set selected rate type. */
            *rateTypePtr = lastQualifiedCirRateType;

            /* Calculate naive HW rate */
            *hwRatePtr = swRate / prvMeterMinRateBurstRangesArray[*rateTypePtr][0];

            if( *hwRatePtr > policerRateRatio )
            {
                /* If naive HW rate calculated highest than rate type maximal rate set */
                /* set selected HW rate to rate type maximal rate.                     */
                *hwRatePtr = policerRateRatio;
            }
            else if (*hwRatePtr < lastQualifiedHwLowRate)
            {
                /* If naive HW rate calculated lower then quantized range lowest rate, */
                /* set selected HW rate to quantized range lowest rate.                */
                *hwRatePtr = lastQualifiedHwLowRate;
            }

            rateTypeBurstGranularity = prvMeterMinRateBurstRangesArray[*rateTypePtr][1];

            /* Calculate best effort burst. <Rate0> must be configured to be less than or 
               equal to <Max Burst Size0>*Burst Granularity - MRU. */
            actualMaxBucketSize = swBurstSize0;
            if (actualMaxBucketSize >= mru)
            {
                actualMaxBucketSize -= mru;
            }

            if( (actualMaxBucketSize/rateTypeBurstGranularity) <= (*hwRatePtr/rateTypeBurstGranularity) )
            {
                /* Naive calcualtion of burst will give lower value than required by the rate\burst */
                /* HW limitation. Find the minimal value that will obey the limitation.             */
                if (actualMaxBucketSize != swBurstSize0)
                {
                    /* MRU should be taken in to account */
                    actualMaxBucketSize = *hwRatePtr + mru;
                }
                else
                {
                    /* burst size is less then MRU. Ignore MRU for calculation */
                    actualMaxBucketSize = *hwRatePtr;
                }

                *hwBurstSize0Ptr = actualMaxBucketSize/rateTypeBurstGranularity + 
                                    (actualMaxBucketSize%rateTypeBurstGranularity == 0 ? 0: 1 );
            }
            else
            {
                /* Maximum burst required. */
                *hwBurstSize0Ptr = 0xFFFF;
            }
        }
        else /* GT_FALSE == PRV_CPSS_DXCH_PP_MAC(devNum)->policer.cbsPbsCalcOnFail */
        {
            /* No configuration with burst best effort calculation requested. */
            return GT_FAIL;
        }

    }

    /*************************************************/
    /* Calculate EBS HW value (SrTCM only parameter) */
    /*************************************************/
    if (hwBurstSize1Ptr != NULL)
    {
        if(swBurstSize1 > (rateTypeBurstGranularity * PRV_CPSS_DXCH3_POLICER_BURST_SIZE_RATIO_CNS))
        {
            *hwBurstSize1Ptr = 0xFFFF;      /* 16 bits */
        }
        else
        {
            *hwBurstSize1Ptr = swBurstSize1 / rateTypeBurstGranularity;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxCh3PolicerHwTbCalculate
*
* DESCRIPTION:
*       Calculate the HW values of Token Bucket and Rate Type.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum              - device number.
*       stage               - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                             Stage type is significant for DxChXcat and above devices
*                             and ignored by DxCh3.
*       swRate              - Rate (CIR or PIR) in the SW format.
*       swBurstSize0        - Burst Size (CBS or PBS) in the SW format.
*       swBurstSize1        - Burst Size (EBS) in the SW format.
*                             This parameter is used only upon SrTCM.
*
* OUTPUTS:
*       hwRatePtr           - Rate (CIR or PIR) in the HW format.
*       hwBurstSize0Ptr     - Burst Size (CBS or PBS) in the HW format.
*       hwBurstSize1Ptr     - Burst Size (EBS) in the HW format.
*                             This parameter is used only upon SrTCM.
*                             Upon TrTCM it should be NULL.
*       rateTypePtr         - pointer to the Rate Type.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_FAIL         - on error.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_BAD_PARAM    - on wrong input parameters.
*
* COMMENTS:
*       Upon TrTCM the hwBurstSize1Ptr parameter should be NULL.
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3PolicerHwTbCalculate
(
    IN  GT_U8   devNum,
    IN  CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN  GT_U32  swRate,
    IN  GT_U32  swBurstSize0,
    IN  GT_U32  swBurstSize1,
    OUT GT_U32  *hwRatePtr,
    OUT GT_U32  *hwBurstSize0Ptr,
    OUT GT_U32  *hwBurstSize1Ptr,
    OUT GT_U32  *rateTypePtr
)
{
    if( CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_ONLY_E ==
        PRV_CPSS_DXCH_PP_MAC(devNum)->policer.meteringCalcMethod)
    {
        return prvCpssDxCh3PolicerHwTbCirCalculate(devNum,
                                                   swRate,
                                                   swBurstSize0,
                                                   swBurstSize1,
                                                   hwRatePtr,
                                                   hwBurstSize0Ptr,
                                                   hwBurstSize1Ptr,
                                                   rateTypePtr);
    }
    else /* CPSS_DXCH_POLICER_METERING_CALC_MODE_CBS_E */
    {
        return prvCpssDxCh3PolicerHwTbCbsCalculate(devNum,
                                                   stage,
                                                   swRate, 
                                                   swBurstSize0, 
                                                   swBurstSize1,
                                                   hwRatePtr,
                                                   hwBurstSize0Ptr,
                                                   hwBurstSize1Ptr,
                                                   rateTypePtr);
    }

}

/*******************************************************************************
* prvCpssDxCh3PolicerSwTbCalculate
*
* DESCRIPTION:
*       Convert Token Bucket from the HW format to the SW format.
*
* APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       meterMode       - Meter mode (SrTCM or TrTCM).
*       rateType0       - SrTCM Bucket Rate Type (or TrTCM Bucket 1 Rate Type).
*       rateType1       - TrTCM Bucket 2 Rate Type.
*       tbHwParamsPtr   - pointer to the HW Token Bucket format.
*
* OUTPUTS:
*       tbSwParamsPtr   - pointer to the SW Token Bucket format.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_BAD_PARAM    - on wrong input parameters.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvCpssDxCh3PolicerSwTbCalculate
(
    IN  CPSS_DXCH3_POLICER_METER_MODE_ENT       meterMode,
    IN  GT_U32                                  rateType0,
    IN  GT_U32                                  rateType1,
    IN  CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT  *tbHwParamsPtr,
    OUT CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT  *tbSwParamsPtr
)
{
    if ((rateType0 > PRV_CPSS_DXCH3_POLICER_RATE_TYPE_MAX_CNS - 1) |
        (rateType1 > PRV_CPSS_DXCH3_POLICER_RATE_TYPE_MAX_CNS - 1))
    {
        return GT_BAD_PARAM;
    }

    switch(meterMode)
    {
        case CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E:
            tbSwParamsPtr->srTcmParams.cir = tbHwParamsPtr->srTcmParams.cir *
                prvMeterMinRateBurstRangesArray[rateType0][0];

            tbSwParamsPtr->srTcmParams.cbs = tbHwParamsPtr->srTcmParams.cbs *
                prvMeterMinRateBurstRangesArray[rateType0][1];

            tbSwParamsPtr->srTcmParams.ebs = tbHwParamsPtr->srTcmParams.ebs *
                prvMeterMinRateBurstRangesArray[rateType0][1];
            break;
        case CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E:
            tbSwParamsPtr->trTcmParams.cir = tbHwParamsPtr->trTcmParams.cir *
                prvMeterMinRateBurstRangesArray[rateType0][0];

            tbSwParamsPtr->trTcmParams.pir = tbHwParamsPtr->trTcmParams.pir *
                prvMeterMinRateBurstRangesArray[rateType1][0];

            tbSwParamsPtr->trTcmParams.cbs = tbHwParamsPtr->trTcmParams.cbs *
                prvMeterMinRateBurstRangesArray[rateType0][1];

            tbSwParamsPtr->trTcmParams.pbs = tbHwParamsPtr->trTcmParams.pbs *
                prvMeterMinRateBurstRangesArray[rateType1][1];
            break;
        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxCh3PolicerInternalTableAccess
*
* DESCRIPTION:
*       Triggers access operation in policer internal table.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       portGroupId     - the portGroupId. to support multi-port-groups device
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above
*                         devices and ignored by DxCh3.
*       entryOffset     - The offset from the base address of the
*                         accessed entry.
*       triggerCmd      - Triggered command that the Policer needs to perform.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_TIMEOUT                  - on time out.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong input parameters.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssDxCh3PolicerInternalTableAccess
(
    IN GT_U8                                        devNum,
    IN  GT_U32                                      portGroupId,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT             stage,
    IN GT_U32                                       entryOffset,
    IN PRV_CPSS_DXCH3_POLICER_ACCESS_CTRL_CMD_ENT   triggerCmd
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      regValue;       /* register value */
    GT_U32      regMask;        /* mask for selecting bits in the register */
    GT_STATUS   retStatus;      /* generic return status code */


    /* Check Triggered command */
    switch(triggerCmd)
    {
        case PRV_CPSS_DXCH3_POLICER_CNTR_READ_AND_RESET_E:
        case PRV_CPSS_DXCH3_POLICER_CNTR_READ_ONLY_E:
        case PRV_CPSS_DXCH3_POLICER_METERING_UPDATE_E:
        case PRV_CPSS_DXCH3_POLICER_METERING_REFRESH_E:
        case PRV_CPSS_DXCH3_POLICER_CNTR_WBF_FLUSH_E:
            break;
        case PRV_CPSS_DXCH3_POLICER_CNTR_READ_AND_WRITE_E:
            /* check that the device is Lion B0 and above */
            if(0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
            {
                return GT_BAD_PARAM;
            }
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                        PLR[stage].policerTblAccessControlReg;

    /* In case of read&write the beasyWait should be done before writing to Access Data registers */
    if (triggerCmd != PRV_CPSS_DXCH3_POLICER_CNTR_READ_AND_WRITE_E)
    {
        /* wait for bit 0 to clear */
        retStatus = prvCpssDxChPortGroupBusyWait(devNum,portGroupId,regAddr,0,GT_FALSE);
        if(retStatus != GT_OK)
        {
            return retStatus;
        }
    }


    regMask = 0xFFFF008F; /* Mask [3:0], [7] and [31:16] ranges */

    /* If triggerCmd is "READ_AND_WRITE" bit 7 should be set,
       thus (triggerCmd << 1) will set this bit*/

    /* Set the Policer Table Access Control fields: */
    /* <TriggerCommand>, <Access Trigger>, <Update Billing Counter mode>
       and <EntryOffset> */
    regValue = 0x1 | (triggerCmd << 1) | (entryOffset << 16);



    /* Now we can write to the register and ask for command implementation */
    retStatus =
        prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum, portGroupId, regAddr,
                                               regMask, regValue);
    if (retStatus == GT_OK)
    {
        if (( PRV_CPSS_DXCH3_POLICER_CNTR_READ_AND_RESET_E == triggerCmd) ||
            ( PRV_CPSS_DXCH3_POLICER_CNTR_READ_ONLY_E == triggerCmd))
        {
            /* wait for bit 0 to clear */
            retStatus = prvCpssDxChPortGroupBusyWait(devNum,portGroupId,regAddr,0,GT_FALSE);
            if(retStatus != GT_OK)
            {
                return retStatus;
            }
        }
    }

    return retStatus;
}

/*******************************************************************************
* prvCpssDxChPolicerBillingCountersModeSet
*
* DESCRIPTION:
*      Sets the Billing Counters resolution.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       cntrMode        - Billing Counters resolution: 1 Byte,
*                         16 Byte or packet.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or cntrMode.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvCpssDxChPolicerBillingCountersModeSet
(
    IN GT_U8                                        devNum,
    IN CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT     cntrMode
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */

    /* Convert Billing Counters resolution to the Reg Value */
    switch(cntrMode)
    {
        case CPSS_DXCH3_POLICER_BILLING_CNTR_1_BYTE_E:
            regValue = 0;
            break;
        case CPSS_DXCH3_POLICER_BILLING_CNTR_16_BYTES_E:
            regValue = 1;
            break;
        case CPSS_DXCH3_POLICER_BILLING_CNTR_PACKET_E:
            regValue = 2;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[0].policerControlReg;

    /* Set Ingress Policer Control register, <Billing_Counters_Mode> field */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 1 , 2, regValue);
}


/*******************************************************************************
* prvCpssDxChPolicerBillingCountersModeGet
*
* DESCRIPTION:
*      Gets the Billing Counters resolution.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       cntrModePtr     - pointer to the Billing Counters resolution:
*                         1 Byte, 16 Byte or packet.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvCpssDxChPolicerBillingCountersModeGet
(
    IN GT_U8                                        devNum,
    OUT CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT    *cntrModePtr
)
{
    GT_U32      regAddr;     /* register address */
    GT_U32      regValue;    /* register value */
    GT_STATUS   retStatus;   /* generic return status code */

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[0].policerControlReg;

    /* Get Ingress Policer Control register, <Billing_Counters_Mode> field */
    retStatus = prvCpssDrvHwPpGetRegField(devNum, regAddr, 1, 2, &regValue);

    if (GT_OK == retStatus)
    {
        /* Convert HW Value to the Billing Counters resolution */
        switch(regValue)
        {
            case 0:
                *cntrModePtr = CPSS_DXCH3_POLICER_BILLING_CNTR_1_BYTE_E;
                break;
            case 1:
                *cntrModePtr = CPSS_DXCH3_POLICER_BILLING_CNTR_16_BYTES_E;
                break;
            case 2:
                *cntrModePtr = CPSS_DXCH3_POLICER_BILLING_CNTR_PACKET_E;
                break;
            default:
                return GT_BAD_PARAM;
        }
    }

    return retStatus;
}

/*******************************************************************************
* cpssDxCh3PolicerMeteringEnableSet
*
* DESCRIPTION:
*       Enables or disables metering per device.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*       enable          - Enable/disable metering:
*                         GT_TRUE  - metering is enabled on the device.
*                         GT_FALSE - metering is disabled on the device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       If metering is enabled, it can be triggered
*       either by Policy engine or per port.
*
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerMeteringEnableSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    IN GT_BOOL                              enable
)
{
    GT_U32  regAddr;    /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControlReg;

    /* Set Policer Control register, <Metering_Mode> field */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 1, BOOL2BIT_MAC(enable));
}

/*******************************************************************************
* cpssDxCh3PolicerMeteringEnableGet
*
* DESCRIPTION:
*       Gets device metering status (Enable/Disable).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*
* OUTPUTS:
*       enablePtr       - pointer to Enable/Disable metering:
*                         GT_TRUE  - metering is enabled on the device.
*                         GT_FALSE - metering is disabled on the device.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       If metering is enabled, it can be triggered
*       either by Policy engine or per port.
*
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerMeteringEnableGet
(
    IN  GT_U8                               devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    OUT GT_BOOL                             *enablePtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      regValue;    /* register value */
    GT_STATUS   retStatus;   /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControlReg;

    /* Get Policer Control register, <Metering_Mode> field */
    retStatus = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 1, &regValue);

    if (GT_OK == retStatus)
    {
        *enablePtr = BIT2BOOL_MAC(regValue);
    }

    return retStatus;
}

/*******************************************************************************
* cpssDxCh3PolicerBillingCountingEnableSet
*
* DESCRIPTION:
*       Enables or disables Billing Counting.
*       Billing is the process of counting the amount of traffic that
*       belongs to a flow and can be activated only by metering.
*       When metering is activated per source port, Billing counting
*       cannot be enabled.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum          - physical device number.
*       enable          - enable/disable Billing Counting:
*                         GT_TRUE  - Billing Counting is enabled on the device.
*                         GT_FALSE - Billing Counting is disabled on the device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerBillingCountingEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32  regAddr;    /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[0].policerControlReg;

    /* Set Ingress Policer Control register, <Counting_Mode> field */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 3, 1, BOOL2BIT_MAC(enable));
}

/*******************************************************************************
* cpssDxCh3PolicerBillingCountingEnableGet
*
* DESCRIPTION:
*       Gets device Billing Counting status (Enable/Disable).
*       Billing is the process of counting the amount of traffic that
*       belongs to a flow and can be activated only by metering.
*       When metering is activated per source port, Billing counting
*       cannot be enabled.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       enablePtr       - pointer to Enable/Disable Billing Counting:
*                         GT_TRUE  - Billing Counting is enabled on the device.
*                         GT_FALSE - Billing Counting is disabled on the device.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerBillingCountingEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32      regAddr;     /* register address */
    GT_U32      regValue;    /* register value */
    GT_STATUS   retStatus;   /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[0].policerControlReg;

    /* Get Ingress Policer Control register, <Counting_Mode> field */
    retStatus = prvCpssDrvHwPpGetRegField(devNum, regAddr, 3, 1, &regValue);

    if (GT_OK == retStatus)
    {
        *enablePtr = BIT2BOOL_MAC(regValue);
    }

    return retStatus;
}

/*******************************************************************************
* cpssDxChPolicerCountingModeSet
*
* DESCRIPTION:
*       Configures counting mode.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above
*                         devices and ignored by DxCh3.
*       mode           -  counting mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or mode.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerCountingModeSet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN CPSS_DXCH_POLICER_COUNTING_MODE_ENT      mode
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      fieldValue;     /* field value */
    GT_STATUS   rc;             /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    switch (mode)
    {
        case CPSS_DXCH_POLICER_COUNTING_DISABLE_E:
            fieldValue = 0;
            break;
        case CPSS_DXCH_POLICER_COUNTING_BILLING_IPFIX_E:
            fieldValue = 1;
            break;
        case CPSS_DXCH_POLICER_COUNTING_POLICY_E:
            fieldValue = 2;
            break;
        case CPSS_DXCH_POLICER_COUNTING_VLAN_E:
            if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
            {
                /* Not supported in xCat2 */
                return GT_NOT_APPLICABLE_DEVICE;
            }
            fieldValue = 3;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* set EPCL EPLR Global Configuration register before EPLR register
       when disable counting */
    if ((stage == CPSS_DXCH_POLICER_STAGE_EGRESS_E) &&
        (mode == CPSS_DXCH_POLICER_COUNTING_DISABLE_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.eplrGlobalConfig;

        /* Set Egress Policer Global Configuration register,
          <Counting Mode > field */
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 1, 2,
                                         fieldValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControlReg;

    /* Set Policer Control register, <Metering_Mode> field */
    rc = prvCpssDrvHwPpSetRegField(
        devNum, regAddr, 1, 2, fieldValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* set EPCL EPLR Global Configuration register after EPLR register
       when enable counting */
    if ((stage == CPSS_DXCH_POLICER_STAGE_EGRESS_E) &&
        (mode != CPSS_DXCH_POLICER_COUNTING_DISABLE_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.eplrGlobalConfig;

        /* Set Egress Policer Global Configuration register,
          <Counting Mode > field */
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 1, 2,
                                         fieldValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return rc;
}

/*******************************************************************************
* cpssDxChPolicerCountingModeGet
*
* DESCRIPTION:
*       Gets the couning mode.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above
*                         devices and ignored by DxCh3.
*
* OUTPUTS:
*       modePtr         - pointer to Counting mode.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerCountingModeGet
(
    IN  GT_U8                                   devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    OUT CPSS_DXCH_POLICER_COUNTING_MODE_ENT     *modePtr
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      fieldValue;     /* field value */
    GT_STATUS   retStatus;      /* return code  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControlReg;

    /* Set Policer Control register, <Metering_Mode> field */
    retStatus = prvCpssDrvHwPpGetRegField(
        devNum, regAddr, 1, 2, &fieldValue);
    if (retStatus != GT_OK)
    {
        return retStatus;
    }

    switch (fieldValue)
    {
        case 0:
            *modePtr = CPSS_DXCH_POLICER_COUNTING_DISABLE_E;
            break;
        case 1:
            *modePtr = CPSS_DXCH_POLICER_COUNTING_BILLING_IPFIX_E;
            break;
        case 2:
            *modePtr = CPSS_DXCH_POLICER_COUNTING_POLICY_E;
            break;
        case 3:
            *modePtr = CPSS_DXCH_POLICER_COUNTING_VLAN_E;
            break;
        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerBillingCountersModeSet
*
* DESCRIPTION:
*      Sets the Billing Counters resolution.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum          - physical device number.
*       cntrMode        - Billing Counters resolution: 1 Byte,
*                         16 Byte or packet.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or cntrMode.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerBillingCountersModeSet
(
    IN GT_U8                                        devNum,
    IN CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT     cntrMode
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);


    return prvCpssDxChPolicerBillingCountersModeSet(devNum,
                                                    cntrMode);
}

/*******************************************************************************
* cpssDxCh3PolicerBillingCountersModeGet
*
* DESCRIPTION:
*      Gets the Billing Counters resolution.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       cntrModePtr     - pointer to the Billing Counters resolution:
*                         1 Byte, 16 Byte or packet.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerBillingCountersModeGet
(
    IN GT_U8                                        devNum,
    OUT CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT    *cntrModePtr
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(cntrModePtr);

    return prvCpssDxChPolicerBillingCountersModeGet(devNum, cntrModePtr);
}

/*******************************************************************************
* cpssDxCh3PolicerPacketSizeModeSet
*
* DESCRIPTION:
*       Sets metered Packet Size Mode that metering and billing is done
*       according to.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum             - physical device number.
*       stage              - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                            Stage type is significant for xCat and above devices
*                            and ignored by DxCh3.
*       packetSize         - Type of packet size:
*                               - L3 datagram size only (this does not include
*                                 the L2 header size and packets CRC).
*                               - L2 packet length including.
*                               - L1 packet length including
*                                 (preamble + IFG + FCS).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or packetSize.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerPacketSizeModeSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    IN CPSS_POLICER_PACKET_SIZE_MODE_ENT    packetSize
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */
    GT_U32  length;     /* length of bit field */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    /* Convert metering and counting packet size to the Reg Value */
    regValue = 0;
    switch(packetSize)
    {
        case CPSS_POLICER_PACKET_SIZE_L3_ONLY_E:
            length = 1;
            break;
        case CPSS_POLICER_PACKET_SIZE_L1_INCLUDE_E:
        case CPSS_POLICER_PACKET_SIZE_L2_INCLUDE_E:
            if (packetSize == CPSS_POLICER_PACKET_SIZE_L1_INCLUDE_E)
            {
                regValue = 0x2;
            }
            regValue |= 0x1;
            length = 2;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControlReg;

    /* Set the following fields of Ingress Policer Control register:    */
    /* <Packet_Size_Mode> and <Include_Layer_1_Overhead>, if required.  */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 16, length, regValue);
}

/*******************************************************************************
* cpssDxCh3PolicerPacketSizeModeGet
*
* DESCRIPTION:
*       Gets metered Packet Size Mode that metering and billing is done
*       according to.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*
* OUTPUTS:
*       packetSizePtr   - pointer to the Type of packet size:
*                           - L3 datagram size only (this does not include
*                             the L2 header size and packets CRC).
*                           - L2 packet length including.
*                           - L1 packet length including (preamble + IFG + FCS).
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerPacketSizeModeGet
(
    IN  GT_U8                                devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT      stage,
    OUT CPSS_POLICER_PACKET_SIZE_MODE_ENT    *packetSizePtr
)
{
    GT_U32      regAddr;     /* register address */
    GT_U32      regValue;    /* register value */
    GT_STATUS   retStatus;   /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);
    CPSS_NULL_PTR_CHECK_MAC(packetSizePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControlReg;

    /* Get the following fields of Ingress Policer Control register: */
    /* <Packet_Size_Mode> and <Include_Layer_1_Overhead>             */
    retStatus = prvCpssDrvHwPpGetRegField(devNum, regAddr, 16, 2, &regValue);

    if (GT_OK == retStatus)
    {
        switch(regValue)
        {
            case 0:
            case 2:
                *packetSizePtr = CPSS_POLICER_PACKET_SIZE_L3_ONLY_E;
                break;
            case 1:
                *packetSizePtr = CPSS_POLICER_PACKET_SIZE_L2_INCLUDE_E;
                break;
            case 3:
                *packetSizePtr = CPSS_POLICER_PACKET_SIZE_L1_INCLUDE_E;
                break;
            default:
                return GT_BAD_PARAM;
        }
    }

    return retStatus;
}

/*******************************************************************************
* cpssDxCh3PolicerMeterResolutionSet
*
* DESCRIPTION:
*       Sets metering algorithm resolution: packets per
*       second or bytes per second.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*       resolution      - packet/Byte based Meter resolution.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or resolution.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerMeterResolutionSet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT  resolution
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    /* Convert Meter resolution to the Reg Value */
    switch(resolution)
    {
        case CPSS_DXCH3_POLICER_METER_RESOLUTION_BYTES_E:
            regValue = 0;
            break;
        case CPSS_DXCH3_POLICER_METER_RESOLUTION_PACKETS_E:
            regValue = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControlReg;

    /* Set Ingress Policer Control reg, <Packet_Based_Meter_Enable> field */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 4, 1, regValue);
}

/*******************************************************************************
* cpssDxCh3PolicerMeterResolutionGet
*
* DESCRIPTION:
*       Gets metering algorithm resolution: packets per
*       second or bytes per second.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*
* OUTPUTS:
*       resolutionPtr   - pointer to the Meter resolution: packet or Byte based.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerMeterResolutionGet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    OUT CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT *resolutionPtr
)
{
    GT_U32      regAddr;     /* register address */
    GT_U32      regValue;    /* register value */
    GT_STATUS   retStatus;   /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(resolutionPtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControlReg;

    /* Get Ingress Policer Control reg, <Packet_Based_Meter_Enable> field */
    retStatus = prvCpssDrvHwPpGetRegField(devNum, regAddr, 4, 1, &regValue);

    if (GT_OK == retStatus)
    {
        /* Convert HW Value to the Meter resolution */
        *resolutionPtr = (0 == regValue) ?
            CPSS_DXCH3_POLICER_METER_RESOLUTION_BYTES_E :
            CPSS_DXCH3_POLICER_METER_RESOLUTION_PACKETS_E;
    }

    return retStatus;
}

/*******************************************************************************
* cpssDxCh3PolicerDropTypeSet
*
* DESCRIPTION:
*       Sets the Policer out-of-profile drop command type.
*       This setting controls if non-conforming dropped packets
*       (Red or Yellow) will be SOFT DROP or HARD DROP.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0 or Ingress #1.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*       dropType        - Policer Drop Type: Soft or Hard.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, dropType or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerDropTypeSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    IN CPSS_DROP_MODE_TYPE_ENT              dropType
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    if (stage > 1)
    {
        /* not supported for Egress Policer */
        return GT_BAD_PARAM;
    }

    /* Convert Policer Drop Type to the Reg Value */
    switch(dropType)
    {
        case CPSS_DROP_MODE_SOFT_E:
            regValue = 0;
            break;
        case CPSS_DROP_MODE_HARD_E:
            regValue = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControlReg;

    /* Set IPLR Control register, <Non_Conforming_Packet_Drop_Mode> field */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 5, 1, regValue);
}

/*******************************************************************************
* cpssDxCh3PolicerDropTypeGet
*
* DESCRIPTION:
*       Gets the Policer out-of-profile drop command type.
*       This setting controls if non-conforming dropped packets
*       (Red or Yellow) will be SOFT DROP or HARD DROP.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0 or Ingress #1.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*
* OUTPUTS:
*       dropTypePtr     - pointer to the Policer Drop Type: Soft or Hard.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerDropTypeGet
(
    IN  GT_U8                               devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    OUT CPSS_DROP_MODE_TYPE_ENT             *dropTypePtr
)
{
    GT_U32      regAddr;     /* register address */
    GT_U32      regValue;    /* register value */
    GT_STATUS   retStatus;   /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(dropTypePtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    if (stage > 1)
    {
        /* not supported for Egress Policer */
        return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControlReg;

    /* Get IPLR Control register, <Non_Conforming_Packet_Drop_Mode> field */
    retStatus = prvCpssDrvHwPpGetRegField(devNum, regAddr, 5, 1, &regValue);

    if (GT_OK == retStatus)
    {
        /* Convert HW Value to the Meter resolution */
        *dropTypePtr = (0 == regValue) ? CPSS_DROP_MODE_SOFT_E :
                                         CPSS_DROP_MODE_HARD_E;
    }

    return retStatus;
}

/*******************************************************************************
* cpssDxCh3PolicerCountingColorModeSet
*
* DESCRIPTION:
*       Sets the Policer color counting mode.
*       The Color Counting can be done according to the packet's
*       Drop Precedence or Conformance Level.
*       This affects both the Billing and Management counters.
*       If the packet was subject to remarking, the drop precedence used
*       here is AFTER remarking.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*       mode            - Color counting mode: Drop Precedence or
*                         Conformance Level.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or mode.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerCountingColorModeSet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT  mode
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    /* Convert Color counting mode to the Reg Value */
    switch(mode)
    {
        case CPSS_DXCH3_POLICER_COLOR_COUNT_CL_E:
            regValue = 0;
            break;
        case CPSS_DXCH3_POLICER_COLOR_COUNT_DP_E:
            regValue = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControlReg;

    /* Set Ingress Policer Control register, <Counter_Color_Mode> field */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 7, 1, regValue);
}

/*******************************************************************************
* cpssDxCh3PolicerCountingColorModeGet
*
* DESCRIPTION:
*       Gets the Policer color counting mode.
*       The Color Counting can be done according to the packet's
*       Drop Precedence or Conformance Level.
*       This affects both the Billing and Management counters.
*       If the packet was subject to remarking, the drop precedence used
*       here is AFTER remarking.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*
* OUTPUTS:
*       modePtr         - pointer to the color counting mode:
*                         Drop Precedence or Conformance Level.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerCountingColorModeGet
(
    IN  GT_U8                                       devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT             stage,
    OUT CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT     *modePtr
)
{
    GT_U32      regAddr;     /* register address */
    GT_U32      regValue;    /* register value */
    GT_STATUS   retStatus;   /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControlReg;

    /* Get Ingress Policer Control register, <Counter_Color_Mode> field */
    retStatus = prvCpssDrvHwPpGetRegField(devNum, regAddr, 7, 1, &regValue);

    if (GT_OK == retStatus)
    {
        /* Convert HW Value to the Meter resolution */
        *modePtr = (0 == regValue) ? CPSS_DXCH3_POLICER_COLOR_COUNT_CL_E :
                                     CPSS_DXCH3_POLICER_COLOR_COUNT_DP_E;
    }

    return retStatus;
}

/*******************************************************************************
* cpssDxCh3PolicerManagementCntrsResolutionSet
*
* DESCRIPTION:
*       Sets the Management counters resolution (either 1 Byte or 16 Bytes).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*       cntrSet         - Management Counters Set [0..2].
*       cntrResolution  - Management Counters resolution: 1 or 16 Bytes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or
*                                     Management Counters Set or cntrResolution.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerManagementCntrsResolutionSet
(
    IN GT_U8                                        devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT             stage,
    IN CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT          cntrSet,
    IN CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT   cntrResolution
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */
    GT_U32  offset;     /* bit offset in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    /* Convert Management Counters Set to the bit offset */
    switch(cntrSet)
    {
        case CPSS_DXCH3_POLICER_MNG_CNTR_SET0_E:
            offset = 8;
            break;
        case CPSS_DXCH3_POLICER_MNG_CNTR_SET1_E:
            offset = 9;
            break;
        case CPSS_DXCH3_POLICER_MNG_CNTR_SET2_E:
            offset = 10;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* Convert Management counters resolution to the Reg Value */
    switch(cntrResolution)
    {
        case CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_1B_E:
            regValue = 0;
            break;
        case CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_16B_E:
            regValue = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControlReg;

    /* Set one of the following Ingress Policer Control register */
    /* fields: <Counter_Set_0_DU_Mode>, <Counter_Set_1_DU_Mode>  */
    /* or <Counter_Set_2_DU_Mode>.                               */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, offset , 1, regValue);
}

/*******************************************************************************
* cpssDxCh3PolicerManagementCntrsResolutionGet
*
* DESCRIPTION:
*       Gets the Management counters resolution (either 1 Byte or 16 Bytes).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*       cntrSet         - Management Counters Set [0..2].
*
* OUTPUTS:
*       cntrResolutionPtr   - pointer to the Management Counters
*                             resolution: 1 or 16 Bytes.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or  Mng Counters Set.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerManagementCntrsResolutionGet
(
    IN  GT_U8                                       devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT             stage,
    IN  CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT         cntrSet,
    OUT CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT  *cntrResolutionPtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      regValue;   /* register value */
    GT_U32      offset;     /* bit offset in the register */
    GT_STATUS   retStatus;  /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(cntrResolutionPtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    /* Convert Management Counters Set to the bit offset */
    switch(cntrSet)
    {
        case CPSS_DXCH3_POLICER_MNG_CNTR_SET0_E:
            offset = 8;
            break;
        case CPSS_DXCH3_POLICER_MNG_CNTR_SET1_E:
            offset = 9;
            break;
        case CPSS_DXCH3_POLICER_MNG_CNTR_SET2_E:
            offset = 10;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControlReg;

    /* Get one of the following Ingress Policer Control register */
    /* fields: <Counter_Set_0_DU_Mode>, <Counter_Set_1_DU_Mode>  */
    /* or <Counter_Set_2_DU_Mode>.                               */
    retStatus = prvCpssDrvHwPpGetRegField(devNum, regAddr,
                                                    offset, 1, &regValue);
    if (GT_OK == retStatus)
    {
        *cntrResolutionPtr = (regValue)?
                            CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_16B_E :
                            CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_1B_E;
    }

    return retStatus;
}

/*******************************************************************************
* cpssDxCh3PolicerPacketSizeModeForTunnelTermSet
*
* DESCRIPTION:
*      Sets size mode for metering and counting of tunnel terminated packets.
*      The Policer provides the following modes to define packet size:
*       - Regular packet metering and counting. The packet size is defined
*         by the cpssDxCh3PolicerPacketSizeModeSet().
*       - Passenger packet metering and counting.
*         Metering and counting of TT packets is performed according
*         to L3 datagram size only. This mode does not include the tunnel
*         header size, the L2 header size, and the packet CRC in the metering
*         and counting.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2.
*
* INPUTS:
*       devNum              - physical device number.
*       stage               - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                             Stage type is significant for xCat and above devices
*                             and ignored by DxCh3.
*       ttPacketSizeMode    - Tunnel Termination Packet Size Mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or ttPacketSizeMode.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerPacketSizeModeForTunnelTermSet
(
    IN  GT_U8                                       devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT             stage,
    IN  CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT  ttPacketSizeMode
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    /* Convert Tunnel Termination Packet Size Mode to the Reg Value */
    switch(ttPacketSizeMode)
    {
        case CPSS_DXCH3_POLICER_TT_PACKET_SIZE_REGULAR_ENT:
            regValue = 0;
            break;
        case CPSS_DXCH3_POLICER_TT_PACKET_SIZE_PASSENGER_ENT:
            regValue = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControlReg;

    /* Set IPLR Control register, <Tunnel_Termination_Packet_Size_Mode> field */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 24, 1, regValue);
}

/*******************************************************************************
* cpssDxCh3PolicerPacketSizeModeForTunnelTermGet
*
* DESCRIPTION:
*      Gets size mode for metering and counting of tunnel terminated packets.
*      The Policer provides the following modes to define packet size:
*       - Regular packet metering and counting. The packet size is defined
*         by the cpssDxCh3PolicerPacketSizeModeSet().
*       - Passenger packet metering and counting.
*         Metering and counting of TT packets is performed according
*         to L3 datagram size only. This mode does not include the tunnel
*         header size, the L2 header size, and the packet CRC in the metering
*         and counting.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2.
*
* INPUTS:
*       devNum              - physical device number.
*       stage               - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                             Stage type is significant for xCat and above devices
*                             and ignored by DxCh3.
*
* OUTPUTS:
*       ttPacketSizeModePtr     - pointer to the Tunnel Termination
*                                 Packet Size Mode.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerPacketSizeModeForTunnelTermGet
(
    IN  GT_U8                                       devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT             stage,
    OUT CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT  *ttPacketSizeModePtr
)
{
    GT_U32      regAddr;     /* register address */
    GT_U32      regValue;    /* register value */
    GT_STATUS   retStatus;   /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(ttPacketSizeModePtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControlReg;

    /* Get IPLR Control register, <Tunnel_Termination_Packet_Size_Mode> field */
    retStatus = prvCpssDrvHwPpGetRegField(devNum, regAddr, 24, 1, &regValue);
    if (GT_OK == retStatus)
    {
        *ttPacketSizeModePtr = (regValue)?
            CPSS_DXCH3_POLICER_TT_PACKET_SIZE_PASSENGER_ENT :
            CPSS_DXCH3_POLICER_TT_PACKET_SIZE_REGULAR_ENT;
    }

    return retStatus;
}

/*******************************************************************************
* cpssDxCh3PolicerMeteringAutoRefreshScanEnableSet
*
* DESCRIPTION:
*       Enables or disables the metering Auto Refresh Scan mechanism.
*
*       If no traffic passes through a meter (long periods of "silence") the
*       meter state variables do not get refreshed. This can cause a problem of
*       state misinterpreted once the flow traffic is restarted, because the
*       timers can have already wrapped around.
*       To avoid this problem the software must initiate a refresh transaction
*       on the meter every ten minutes.
*
*       This CPU intensive metering refresh maintanance can be offloaded by
*       Auto Refresh Scan mechanism.
*       The range of Auto Refresh Scan is configurable by the
*       cpssDxCh3PolicerMeteringAutoRefreshRangeSet().
*       The Auto Refresh Scan Rate can be controlled by setting time interval
*       between two refresh access to metering table. The time interval is
*       configured by the cpssDxCh3PolicerMeteringAutoRefreshIntervalSet().
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum          - physical device number.
*       enable          - Enable/disable Auto Refresh Scan mechanism:
*                         GT_TRUE  - enable Auto Refresh scan mechanism on
*                                    the device.
*                         GT_FALSE - disable Auto Refresh scan mechanism on
*                                    the device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       The CPU is still capable to perform any Refresh Transactions during
*       the Auto Refresh Scan operation.
*       Once the CPU triggers Refresh transaction, the policer accesses the
*       metering entry and performs metering refresh with the packet's Byte
*       Count set to zero.
*
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerMeteringAutoRefreshScanEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32  regAddr;    /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[0].policerControlReg;

    /* Set IPLR Control register, <Metering_Refresh_Scan_Enable> field */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 11, 1, BOOL2BIT_MAC(enable));
}

/*******************************************************************************
* cpssDxCh3PolicerMeteringAutoRefreshScanEnableGet
*
* DESCRIPTION:
*       Gets metering Auto Refresh Scan mechanism status (Enabled/Disabled).
*
*       If no traffic passes through a meter (long periods of "silence") the
*       meter state variables do not get refreshed. This can cause a problem of
*       state misinterpreted once the flow traffic is restarted, because the
*       timers can have already wrapped around.
*       To avoid this problem the software must initiate a refresh transaction
*       on the meter every ten minutes.
*
*       This CPU intensive metering refresh maintanance can be offloaded by
*       Auto Refresh Scan mechanism.
*       The range of Auto Refresh Scan is configurable by the
*       cpssDxCh3PolicerMeteringAutoRefreshRangeSet().
*       The Auto Refresh Scan Rate can be controlled by setting time interval
*       between two refresh access to metering table. The time interval is
*       configured by the cpssDxCh3PolicerMeteringAutoRefreshIntervalSet().
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       enablePtr       - pointer on Auto Refresh Scan mechanism status
*                         (Enable/Disable) :
*                         GT_TRUE  - Auto Refresh scan mechanism is enabled.
*                         GT_FALSE - Auto Refresh scan mechanism is disabled.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       The CPU is still capable to perform any Refresh Transactions during
*       the Auto Refresh Scan operation.
*       Once the CPU triggers Refresh transaction, the policer accesses the
*       metering entry and performs metering refresh with the packet's Byte
*       Count set to zero.
*
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerMeteringAutoRefreshScanEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32      regAddr;     /* register address */
    GT_U32      regValue;    /* register value */
    GT_STATUS   retStatus;   /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[0].policerControlReg;

    /* Get IPLR Control register, <Metering_Refresh_Scan_Enable> field */
    retStatus = prvCpssDrvHwPpGetRegField(devNum, regAddr, 11, 1, &regValue);

    if (GT_OK == retStatus)
    {
        *enablePtr = BIT2BOOL_MAC(regValue);
    }

    return retStatus;
}

/*******************************************************************************
* cpssDxCh3PolicerMeteringAutoRefreshRangeSet
*
* DESCRIPTION:
*       Sets Metering Refresh Scan address range (Start and Stop addresses).
*       The Auto Refresh Scan mechanism is enabled by the
*       cpssDxCh3PolicerMeteringAutoRefreshScanEnableSet().
*       The Auto Refresh Scan Rate can be controlled by setting time interval
*       between two refresh access to metering table. The time interval is
*       configured by the cpssDxCh3PolicerMeteringAutoRefreshIntervalSet().
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum              - physical device number.
*       startAddress        - beginning of the address range to be scanned
*                             by the Auto Refresh Scan mechanism [0..1023].
*       stopAddress         - end of the address range to be scanned by the
*                             Auto Refresh Scan mechanism [0..1023].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or illegal values
*                                     of startAddress and stopAddress.
*       GT_OUT_OF_RANGE             - on out of range of startAddress
*                                     or stopAddress.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerMeteringAutoRefreshRangeSet
(
    IN GT_U8    devNum,
    IN GT_U32   startAddress,
    IN GT_U32   stopAddress
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);


    if(startAddress >= BIT_16 || stopAddress >= BIT_16)  /*16 bits in HW */
    {
        return GT_OUT_OF_RANGE;
    }

    /* Check pointed startAddressPtr value less than stopAddressPtr value */
    if(startAddress > stopAddress)
    {
        return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                            PLR[0].policerRefreshScanRangeReg;

    /* Create HW data should be set to the register */
    regValue = startAddress | (stopAddress << 16);

    /* Set IPLR Metering refresh scan address range register: */
    /* <Metering_Scan_Start_Address> and <Metering_Scan_Stop_Address> fields */
    return prvCpssDrvHwPpWriteRegister(devNum, regAddr, regValue);
}

/*******************************************************************************
* cpssDxCh3PolicerMeteringAutoRefreshRangeGet
*
* DESCRIPTION:
*       Gets Metering Refresh Scan address range (Start and Stop addresses).
*       The Auto Refresh Scan mechanism is enabled by the
*       cpssDxCh3PolicerMeteringAutoRefreshScanEnableSet().
*       The Auto Refresh Scan Rate can be controlled by setting time interval
*       between two refresh access to metering table. The time interval is
*       configured by the cpssDxCh3PolicerMeteringAutoRefreshIntervalSet().
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum              - physical device number.
*
* OUTPUTS:
*       startAddressPtr     - pointer to the beginning address of Refresh
*                             Scan address range.
*       stopAddressPtr      - pointer to the end address of Refresh Scan
*                             address range.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointers.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerMeteringAutoRefreshRangeGet
(
    IN GT_U8    devNum,
    OUT GT_U32  *startAddressPtr,
    OUT GT_U32  *stopAddressPtr
)
{
    GT_U32      regAddr;     /* register address */
    GT_U32      regValue;    /* register value */
    GT_STATUS   retStatus;   /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(startAddressPtr);
    CPSS_NULL_PTR_CHECK_MAC(stopAddressPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                            PLR[0].policerRefreshScanRangeReg;

    /* Get IPLR Metering refresh scan address range register */
    retStatus = prvCpssDrvHwPpReadRegister(devNum, regAddr, &regValue);

    if (GT_OK == retStatus)
    {
        /* Extract <Metering_Scan_Start_Address> and
           <Metering_Scan_Stop_Address> fields from the HW register value */
        *startAddressPtr    = regValue & 0xFFFF;
        *stopAddressPtr     = regValue >> 16;
    }

    return retStatus;
}

/*******************************************************************************
* cpssDxCh3PolicerMeteringAutoRefreshIntervalSet
*
* DESCRIPTION:
*       Sets the time interval between two refresh access to metering table.
*       The Auto Refresh Scan mechanism is enabled by the
*       cpssDxCh3PolicerMeteringAutoRefreshScanEnableSet().
*       The range of Auto Refresh Scan is configurable by the
*       cpssDxCh3PolicerMeteringAutoRefreshRangeSet().
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum          - physical device number.
*       interval        - time interval between refresh of two entries
*                         in micro seconds. Upon 270 Mhz core clock value
*                         of DxCh3, the range [0..15907286 microS].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_OUT_OF_RANGE             - on out of range of interval value.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerMeteringAutoRefreshIntervalSet
(
    IN GT_U8    devNum,
    IN GT_U32   interval
)
{
    GT_U32  regAddr;            /* register address */
    GT_U32  regValue;           /* register value */
    GT_U32  maxIntervalValue;   /* maximum interval value in microS */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);


    /* Calculate maximum interval value in microS */
    maxIntervalValue = 0xFFFFFFFF / PRV_CPSS_PP_MAC(devNum)->coreClock;

    /* Check time interval range */
    if(maxIntervalValue < interval)
    {
        return GT_OUT_OF_RANGE;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                            PLR[0].policerRefreshScanRateReg;

    /* Create HW data should be set to the register */
    regValue = interval * PRV_CPSS_PP_MAC(devNum)->coreClock;

    /* Set Ingress Policer Metering refresh Scan Rate Limit register */
    return prvCpssDrvHwPpWriteRegister(devNum, regAddr, regValue);
}

/*******************************************************************************
* cpssDxCh3PolicerMeteringAutoRefreshIntervalGet
*
* DESCRIPTION:
*       Gets the time interval between two refresh access to metering table.
*       The Auto Refresh Scan mechanism is enabled by the
*       cpssDxCh3PolicerMeteringAutoRefreshScanEnableSet().
*       The range of Auto Refresh Scan is configurable by the
*       cpssDxCh3PolicerMeteringAutoRefreshRangeSet().
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       intervalPtr     - pointer to the time interval between refresh of two
*                         entries in micro seconds. Upon 270 Mhz core clock
*                         value of DxCh3, the range [0..15907286 microS].
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerMeteringAutoRefreshIntervalGet
(
    IN GT_U8    devNum,
    OUT GT_U32  *intervalPtr
)
{
    GT_U32      regAddr;     /* register address */
    GT_U32      regValue;    /* register value */
    GT_STATUS   retStatus;   /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(intervalPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                            PLR[0].policerRefreshScanRateReg;

    /* Get Ingress Policer Metering refresh Scan Rate Limit register */
    retStatus = prvCpssDrvHwPpReadRegister(devNum, regAddr, &regValue);

    if (GT_OK == retStatus)
    {
        /* Calculate Rate limit HW value in micro second */
        *intervalPtr = regValue / PRV_CPSS_PP_MAC(devNum)->coreClock;
    }

    return retStatus;
}

/*******************************************************************************
* cpssDxCh3PolicerMeteringEntryRefresh
*
* DESCRIPTION:
*       Refresh the Policer Metering Entry.
*       Meter Bucket's state parameters refreshing are needed in order to
*       prevent mis-behavior due to wrap around of timers.
*
*       The wrap around problem can occur when there are long periods of
*       'silence' on the flow and the bucket's state parameters do not get
*       refreshed (meter is in the idle state). This causes a problem once the
*       flow traffic is re-started since the timers might have already wrapped
*       around which can cause a situation in which the bucket's state is
*       mis-interpreted and the incoming packet is marked as non-conforming
*       even though the bucket was actually supposed to be full.
*       To prevent this from happening the CPU needs to trigger a meter
*       refresh transaction at least once every 10 minutes per meter.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above
*                         devices and ignored by DxCh3.
*       entryIndex      - index of Policer Metering Entry.
*                         Range: see datasheet for specific device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_TIMEOUT                  - on time out of IPLR Table indirect access.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or entryIndex.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       Once the CPU triggers Refresh transaction the policer accesses the
*       metering entry and performs metering refresh with the packet's Byte
*       Count set to zero.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerMeteringEntryRefresh
(
    IN  GT_U8                               devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    IN  GT_U32                              entryIndex
)
{
    return cpssDxChPolicerPortGroupMeteringEntryRefresh(
                                            devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            stage,
                                            entryIndex);
}

/*******************************************************************************
* cpssDxCh3PolicerPortMeteringEnableSet
*
* DESCRIPTION:
*       Enables or disables a port metering trigger for packets
*       arriving on this port.
*       When feature is enabled the meter entry index is a port number.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*       portNum         - port number (including the CPU port).
*       enable          - Enable/Disable per-port metering for packets arriving
*                         on this port:
*                         GT_TRUE  - Metering is triggered on specified port.
*                         GT_FALSE - Metering isn't triggered on specified port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or portNum.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerPortMeteringEnableSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    IN GT_U8                                portNum,
    IN GT_BOOL                              enable
)
{
    GT_U32  regAddr;    /* the register address */
    GT_U32  regValue;   /* register value       */
    GT_U32  regOffset;  /* register offset      */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */
    GT_U32  startBit;  /* the word's bit at which the field starts */

    GT_STATUS rc;       /* return code          */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    regOffset = 0;

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* DxXcat */
        if (localPort > 31)
            regOffset = 1;
    }

    /* Remap CPU Port to the offset in the Policer Port Metering Enable register */
    if(CPSS_CPU_PORT_NUM_CNS == localPort)
    {
        localPort = 31;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerPortMeteringEnReg[regOffset];

    regValue = (enable == GT_TRUE) ? 1 : 0;

    /* Enable/Disables Metering on this port by updating */
    /* Policer Port Metering Enable register.               */
    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr,
                                            (GT_U32)localPort, 1, regValue);
    if (rc != GT_OK) 
    {
        return rc;
    }

    if (PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum) &&
        (stage == CPSS_DXCH_POLICER_STAGE_EGRESS_E)) 
    {
        startBit = (localPort == CPSS_CPU_PORT_NUM_CNS) ?
                 PRV_CPSS_DXCH_PP_HW_INFO_HA_CPU_PORT_BIT_INDEX_MAC(devNum) :
                 localPort;

        /* EPCL - Port Based Metering Register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.
                                            eplrPortBasedMetering[OFFSET_TO_WORD_MAC(startBit)];

        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                OFFSET_TO_BIT_MAC(startBit), 1, regValue);
        if (rc != GT_OK) 
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerPortMeteringEnableGet
*
* DESCRIPTION:
*       Gets port status (Enable/Disable) of metering for packets
*       arriving on this port.
*       When feature is enabled the meter entry index is a port number.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*       portNum         - port number (including the CPU port).
*
* OUTPUTS:
*       enablePtr       - pointer on per-port metering status (Enable/Disable)
*                         for packets arriving on specified port:
*                         GT_TRUE  - Metering is triggered on specified port.
*                         GT_FALSE - Metering isn't triggered on specified port.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or portNum.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerPortMeteringEnableGet
(
    IN  GT_U8                               devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    IN  GT_U8                               portNum,
    OUT GT_BOOL                             *enablePtr
)
{
    GT_U32      regAddr;    /* the register address */
    GT_U32      regValue;   /* register value */
    GT_U32      regOffset;  /* register offset      */
    GT_STATUS   retStatus;  /* generic return status code */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    regOffset = 0;

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* DxXcat */
        if (localPort > 31)
            regOffset = 1;
    }

    /* Remap CPU Port to the offset in the Policer Port Metering Enable register */
    if(CPSS_CPU_PORT_NUM_CNS == localPort)
    {
        localPort = 31;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerPortMeteringEnReg[regOffset];

    /* Get port metering status for arrived packets (Enable/Disable) */
    /* by reading IPLR Port Metering Enable register.                */
    retStatus = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr,
                                          (GT_U32)localPort, 1, &regValue);
    if(GT_OK == retStatus)
    {
        *enablePtr = (regValue == 1) ? GT_TRUE : GT_FALSE;
    }

    return retStatus;
}

/*******************************************************************************
* cpssDxCh3PolicerMruSet
*
* DESCRIPTION:
*       Sets the Policer Maximum Receive Unit size.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*       mruSize         - Policer MRU value in bytes, range of values [0..64K].
*                         This value is used in the metering algorithm.
*                         When the number of bytes in the bucket is lower than
*                         this value a packet is marked as non conforming.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_OUT_OF_RANGE             - on mruSize out of range [0..65535 Bytes].
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerMruSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    IN GT_U32                               mruSize
)
{
    GT_U32  regAddr;    /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    /* Check MRU value */
    if(mruSize > 0xFFFF)
    {
        return GT_OUT_OF_RANGE;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerMRUReg;

    /* Set Ingress Policer MRU register */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 16, mruSize);
}

/*******************************************************************************
* cpssDxCh3PolicerMruGet
*
* DESCRIPTION:
*       Gets the Policer Maximum Receive Unit size.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*
* OUTPUTS:
*       mruSizePtr      - pointer to the Policer MRU value in bytes,
*                         range of values [0..64K].
*                         This value is used in the metering algorithm.
*                         When the number of bytes in the bucket is lower than
*                         this value a packet is marked as non conforming.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerMruGet
(
    IN  GT_U8                               devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    OUT GT_U32                              *mruSizePtr
)
{
    GT_U32      regAddr;     /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(mruSizePtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerMRUReg;

    /* Get Ingress Policer MRU value */
    return prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 16, mruSizePtr);
}

/*******************************************************************************
* cpssDxCh3PolicerErrorGet
*
* DESCRIPTION:
*       Gets address and type of Policer Entry that had an ECC error.
*       This information available if error is happened and not read till now.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*
* OUTPUTS:
*       entryTypePtr    - pointer to the Type of Entry (Metering or Counting)
*                         that had an error.
*       entryAddrPtr    - pointer to the Policer Entry that had an error.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_EMPTY                    - on missing error information.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerErrorGet
(
    IN  GT_U8                               devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    OUT CPSS_DXCH3_POLICER_ENTRY_TYPE_ENT   *entryTypePtr,
    OUT GT_U32                              *entryAddrPtr
)
{
    return cpssDxChPolicerPortGroupErrorGet(devNum,
                                             CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                             stage,
                                             entryTypePtr,
                                             entryAddrPtr);
}

/*******************************************************************************
* cpssDxCh3PolicerErrorCounterGet
*
* DESCRIPTION:
*       Gets the value of the Policer ECC Error Counter.
*       The Error Counter is a free-running non-sticky 8-bit read-only
*       counter.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*
* OUTPUTS:
*       cntrValuePtr    - pointer to the Policer ECC Error counter value.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerErrorCounterGet
(
    IN  GT_U8                               devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    OUT GT_U32                              *cntrValuePtr
)
{
    return cpssDxChPolicerPortGroupErrorCounterGet(
                                            devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            stage,
                                            cntrValuePtr);
}

/*******************************************************************************
* cpssDxCh3PolicerManagementCountersSet
*
* DESCRIPTION:
*       Sets the value of specified Management Counters.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*       mngCntrSet      - Management Counters Set[0..2].
*       mngCntrType     - Management Counters Type (GREEN, YELLOW, RED, DROP).
*       mngCntrPtr      - pointer to the Management Counters Entry must be set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage or mngCntrType
*                                     or Management Counters Set number.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       In data unit management counter only 32 bits are used for DxCh3 devices
*       and 42 bits are used for xCat and above devices.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerManagementCountersSet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT      mngCntrSet,
    IN CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT     mngCntrType,
    IN CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC    *mngCntrPtr
)
{
    return cpssDxChPolicerPortGroupManagementCountersSet(
                                        devNum,
                                        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                        stage,
                                        mngCntrSet,
                                        mngCntrType,
                                        mngCntrPtr);
}

/*******************************************************************************
* cpssDxCh3PolicerManagementCountersGet
*
* DESCRIPTION:
*       Gets the value of specified Management Counters.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*       mngCntrSet      - Management Counters Set[0..2].
*       mngCntrType     - Management Counters Type (GREEN, YELLOW, RED, DROP).
*
* OUTPUTS:
*       mngCntrPtr      - pointer to the requested Management Counters Entry.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage or mngCntrType
*                                     or Management Counters Set number.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       In data unit management counter only 32 bits are used for DxCh3 devices
*       and 42 bits are used for xCat and above devices.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerManagementCountersGet
(
    IN  GT_U8                                   devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN  CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT     mngCntrSet,
    IN  CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT    mngCntrType,
    OUT CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC   *mngCntrPtr
)
{
    return cpssDxChPolicerPortGroupManagementCountersGet(
                                            devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            stage,
                                            mngCntrSet,
                                            mngCntrType,
                                            mngCntrPtr);
}

/*******************************************************************************
* prvCpssDxChPolicerMeteringEntrySet
*
* DESCRIPTION:
*      Sets Metering Policer Entry starting from third word
*      for xCat and above.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*       entryPtr        - pointer to the metering policer entry going to be set.
*
* OUTPUTS:
*       tbParamsPtr     - pointer to actual policer token bucket parameters.
*                         The token bucket parameters are returned as output
*                         values. This is due to the hardware rate resolution,
*                         the exact rate or burst size requested may not be
*                         honored. The returned value gives the user the
*                         actual parameters configured in the hardware.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - wrong devNum, stage or entryIndex or
*                                     entry parameters.
*       GT_OUT_OF_RANGE             - on Billing Entry Index out of range.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvCpssDxChPolicerMeteringEntrySet
(
    IN  GT_U8                                   devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN  CPSS_DXCH3_POLICER_METERING_ENTRY_STC   *entryPtr,
    OUT CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT  *tbParamsPtr
)
{
    GT_U32              hwData;         /* data to write to HW */
    GT_U32              regAddr;        /* register address */
    GT_U32              hwRate0;        /* HW rate 0 value */
    GT_U32              hwRate1;        /* HW rate 1 value */
    GT_U32              hwBurstSize0;   /* HW Max Burst Size 0 value */
    GT_U32              hwBurstSize1;   /* HW Max Burst Size 1 value */
    GT_U32              hwRateType0;    /* HW rate type 0 value */
    GT_U32              hwRateType1;    /* HW rate type 1 value */
    GT_U32              hwBucketSize0;  /* HW Bucket Size 0 value */
    GT_U32              hwBucketSize1;  /* HW Bucket Size 1 value */
    GT_U32              redPcktCmdVal;      /* Hw value for red packet command */
    GT_U32              yellowPcktCmdVal;   /* Hw value for yellow packet command */
    GT_U32              meterColorModeVal;  /* Hw value for meter color mode */
    GT_U32              meterModeVal;       /* Hw value for meter mode */
    GT_U32              modifyDscpVal;      /* Hw value for modify dscp operation */
    GT_U32              modifyUpVal;        /* Hw value for modify up operation */
    GT_U32              mngCounterSetVal;   /* Hw value for management counter set */
    GT_STATUS           retStatus;      /* generic return status code */


    /* Calculate Token Backet  */
    switch(entryPtr->meterMode)
    {
        case CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E:
            retStatus = prvCpssDxCh3PolicerHwTbCalculate(
                           devNum,
                           stage,
                           entryPtr->tokenBucketParams.srTcmParams.cir,
                           entryPtr->tokenBucketParams.srTcmParams.cbs,
                           entryPtr->tokenBucketParams.srTcmParams.ebs,
                           &(tbParamsPtr->srTcmParams.cir), /* Rate0 = Rate1 */
                           &(tbParamsPtr->srTcmParams.cbs), /* MaxBurstSize0 */
                           &(tbParamsPtr->srTcmParams.ebs), /* MaxBurstSize1 */
                           &hwRateType0);      /* RateType0 = RateType1 */
            if (retStatus != GT_OK)
            {
                return retStatus;
            }
            hwRateType1 = hwRateType0;          /* RateType0 = RateType1 */
            hwRate0 = tbParamsPtr->srTcmParams.cir;
            hwRate1 = hwRate0;                              /* Rate0 = Rate1 */
            hwBurstSize0 = tbParamsPtr->srTcmParams.cbs;    /* MaxBurstSize0 */
            hwBurstSize1 = tbParamsPtr->srTcmParams.ebs;    /* MaxBurstSize1 */
            break;

        case CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E:
            retStatus = prvCpssDxCh3PolicerHwTbCalculate(
                           devNum,
                           stage,
                           entryPtr->tokenBucketParams.trTcmParams.cir,
                           entryPtr->tokenBucketParams.trTcmParams.cbs,
                           0,                   /* Not exists in the TrTCM */
                           &(tbParamsPtr->trTcmParams.cir), /* Rate0 */
                           &(tbParamsPtr->trTcmParams.cbs), /* MaxBurstSize0 */
                           NULL,                /* Not exists in the TrTCM */
                           &hwRateType0);       /* RateType0 */
            if (retStatus != GT_OK)
            {
                return retStatus;
            }
            hwRate0 = tbParamsPtr->trTcmParams.cir;         /* Rate0 */
            hwBurstSize0 = tbParamsPtr->trTcmParams.cbs;    /* MaxBurstSize0 */

            retStatus = prvCpssDxCh3PolicerHwTbCalculate(
                           devNum,
                           stage,
                           entryPtr->tokenBucketParams.trTcmParams.pir,
                           entryPtr->tokenBucketParams.trTcmParams.pbs,
                           0,                   /* Not exists in the TrTCM */
                           &(tbParamsPtr->trTcmParams.pir), /* Rate1 */
                           &(tbParamsPtr->trTcmParams.pbs), /* MaxBurstSize1 */
                           NULL,                /* Not exists in the TrTCM */
                           &hwRateType1);       /* RateType1 */
            if (retStatus != GT_OK)
            {
                return retStatus;
            }
            hwRate1 = tbParamsPtr->trTcmParams.pir;         /* Rate1 */
            hwBurstSize1 = tbParamsPtr->trTcmParams.pbs;    /* MaxBurstSize1 */

            break;

        default:
            return GT_BAD_PARAM;
    }

    /*************************************************/
    /* Convert actual TB parameters to the SW format */
    /*************************************************/
    retStatus = prvCpssDxCh3PolicerSwTbCalculate(entryPtr->meterMode,
                                                 hwRateType0, hwRateType1,
                                                 tbParamsPtr, tbParamsPtr);
    if (retStatus != GT_OK)
    {
        return retStatus;
    }

    /* get bucket size values in bytes as Maximal Burst size of token bucket */
    if(entryPtr->meterMode == CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E)
    {
        hwBucketSize0 = tbParamsPtr->srTcmParams.cbs;
        hwBucketSize1 = tbParamsPtr->srTcmParams.ebs;
    }
    else
    {
        hwBucketSize0 = tbParamsPtr->trTcmParams.cbs;
        hwBucketSize1 = tbParamsPtr->trTcmParams.pbs;
    }


    /* Calculate hw value for Policer Red Packet command  */
    switch(entryPtr->redPcktCmd)
    {
        case CPSS_DXCH3_POLICER_NON_CONFORM_CMD_NO_CHANGE_E:
            redPcktCmdVal = 0;
            break;
        case CPSS_DXCH3_POLICER_NON_CONFORM_CMD_DROP_E:
            redPcktCmdVal = 1;
            break;
        case CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_E:
            redPcktCmdVal = 2;
            break;
        case CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E:
            redPcktCmdVal = 3;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* Calculate hw value for Policer Yellow Packet command  */
    switch(entryPtr->yellowPcktCmd)
    {
        case CPSS_DXCH3_POLICER_NON_CONFORM_CMD_NO_CHANGE_E:
            yellowPcktCmdVal = 0;
            break;
        case CPSS_DXCH3_POLICER_NON_CONFORM_CMD_DROP_E:
            yellowPcktCmdVal = 1;
            break;
        case CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_E:
            yellowPcktCmdVal = 2;
            break;
        case CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E:
            yellowPcktCmdVal = 3;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* Calculate hw value for Color Mode */
    switch(entryPtr->meterColorMode)
    {
        case CPSS_POLICER_COLOR_BLIND_E:
            meterColorModeVal = 0;
            break;
        case CPSS_POLICER_COLOR_AWARE_E:
            meterColorModeVal = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* Calculate hw value for Meter Mode */
    switch(entryPtr->meterMode)
    {
        case CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E:
            meterModeVal = 0;
            break;
        case CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E:
            meterModeVal = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* Calculate hw value for Management Counter Set */
    switch(entryPtr->mngCounterSet)
    {
        case CPSS_DXCH3_POLICER_MNG_CNTR_SET0_E:
            mngCounterSetVal = 0;
            break;
        case CPSS_DXCH3_POLICER_MNG_CNTR_SET1_E:
            mngCounterSetVal = 1;
            break;
        case CPSS_DXCH3_POLICER_MNG_CNTR_SET2_E:
            mngCounterSetVal = 2;
            break;
        case CPSS_DXCH3_POLICER_MNG_CNTR_DISABLED_E:
            mngCounterSetVal = 3;
            break;
        default:
            return GT_BAD_PARAM;
    }


    /**********/
    /* Word 1 */
    /**********/
    /* set Bucket size 0 - LSB 6 bits to bits 26:31 */
    regAddr = 0x4 +
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerTblAccessDataReg;
    /* Set Ingress Policer Table Access Data 1 Register */
    retStatus = prvCpssDrvHwPpSetRegField(devNum, regAddr, 26, 6,
                                          (hwBucketSize0 & 0x3f));
    if (retStatus != GT_OK)
    {
        return retStatus;
    }

    /**********/
    /* Word 2 */
    /**********/
    /* set Bucket size 0 - MSB 26 bits to bits 0:25,
       set Bucket size 1 - LSB 6 bits to bits 26:31 */
    hwData = (hwBucketSize0 >> 6) | ((hwBucketSize1 & 0x3f) << 26);

    regAddr = 0x8 +
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerTblAccessDataReg;
    /* Set Ingress Policer Table Access Data 1 Register */
    retStatus = prvCpssDrvHwPpWriteRegister(devNum, regAddr, hwData);
    if (retStatus != GT_OK)
    {
        return retStatus;
    }

    /**********/
    /* Word 3 */
    /**********/
    /* set Bucket size 1 - MSB 26 bits to bits 0:25 */
    hwData = (hwBucketSize1 >> 6);

    /* Set RateType0 value */
    hwData |= (hwRateType0 << 26);
    /* Set RateType1 value */
    hwData |= (hwRateType1 << 29);

    regAddr = 0xC +
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerTblAccessDataReg;
    /* Set Policer Table Access Data 3 Register */
    retStatus = prvCpssDrvHwPpWriteRegister(devNum, regAddr, hwData);
    if (retStatus != GT_OK)
    {
        return retStatus;
    }

    /**********/
    /* Word 4 */
    /**********/

    /* Set Rate 0 (CIR) */
    /* bits 0:16 */
    hwData = (hwRate0 & 0x1FFFF);
    /* Set 15 LSB of Rate 1 (CIR) */
    /* bits 17:31 */
    hwData |= ((hwRate1 & 0x7FFF) << 17);

    regAddr = 0x10 +
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerTblAccessDataReg;
    /* Set Policer Table Access Data 4 Register */
    retStatus = prvCpssDrvHwPpWriteRegister(devNum, regAddr, hwData);
    if (retStatus != GT_OK)
    {
        return retStatus;
    }

    /**********/
    /* Word 5 */
    /**********/

    /* Set 2 MSB of Rate 1 (CIR) */
    /* bits 0:1 */
    hwData = ((hwRate1 >> 15) & 0x3);
    /* Set Maximum burst size for bucket 0 */
    /* bits 2:17 */
    hwData |= (hwBurstSize0 << 2);
    /* Set 14 LSB of Maximum burst size for bucket 1 */
    /* bits 18:31 */
    hwData |= ((hwBurstSize1 & 0x3FFF) << 18);

    regAddr = 0x14 +
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerTblAccessDataReg;
    /* Set Policer Table Access Data 5 Register */
    retStatus = prvCpssDrvHwPpWriteRegister(devNum, regAddr, hwData);
    if (retStatus != GT_OK)
    {
        return retStatus;
    }

    /**********/
    /* Word 6 */
    /**********/

    /* Check index of Policer Billing Entry
       Bits [15:3] contains the line offset of the counting entry */
    if(entryPtr->countingEntryIndex >= BIT_16)
    {
        return GT_OUT_OF_RANGE;
    }

    /* Set 2 MSB of Maximum burst size for bucket 1 */
    /* bits 0:1 */
    hwData = ((hwBurstSize1 >> 14) & 0x3);
    /* Set Color Mode */
    /* bit 2 */
    hwData |= (meterColorModeVal << 2);
    /* Set Meter Mode */
    /* bit 3 */
    hwData |= (meterModeVal << 3);
    /* Set Management Counter Set */
    /* bits 4:5 */
    hwData |= (mngCounterSetVal << 4);
    /* Set pointer to related counting entry */
    /* bits 6:21 */
    hwData |= (entryPtr->countingEntryIndex << 6);

    /* check Policer stage: Ingress or Egress */
    if (PRV_CPSS_DXCH_POLICER_INGRESS_STAGE_CHECK_MAC(stage))
    { /* IPLR */

        /* check Qos Profile range */
        if (entryPtr->qosProfile >= BIT_7)
        {
            return GT_OUT_OF_RANGE;
        }

        /* Calculate hw value for Modify DSCP operation */
        switch(entryPtr->modifyDscp)
        {
            case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
                modifyDscpVal = 0;
                break;
            case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
                modifyDscpVal = 1;
                break;
            case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
                modifyDscpVal = 2;
                break;
            default:
                return GT_BAD_PARAM;
        }

        /* Calculate hw value for Modify UP operation */
        switch(entryPtr->modifyUp)
        {
            case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
                modifyUpVal = 0;
                break;
            case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
                modifyUpVal = 1;
                break;
            case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
                modifyUpVal = 2;
                break;
            default:
                return GT_BAD_PARAM;
        }

        /* Set Policer commands for Red Packet */
        /* bits 22:23 */
        hwData |= (redPcktCmdVal << 22);
        /* Set Policer commands for Yellow Packet */
        /* bits 24:25 */
        hwData |= (yellowPcktCmdVal << 24);
        /* Set Modify DSCP operation */
        /* bits 26:27 */
        hwData |= (modifyDscpVal << 26);
        /* Set Modify UP operation */
        /* bits 28:29 */
        hwData |= (modifyUpVal << 28);
        /* Set 2 LSB of Qos Profile */
        /* bits 30:31 */
        hwData |= ((entryPtr->qosProfile & 0x3) << 30);
    }
    else
    { /* EPLR */

        /* Set Remark mode */
        /* bit 22 */
        switch (entryPtr->remarkMode)
        {
            case CPSS_DXCH_POLICER_REMARK_MODE_L2_E:
                break;
            case CPSS_DXCH_POLICER_REMARK_MODE_L3_E:
                hwData |= (1 << 22);
                break;
            default:
                return GT_BAD_PARAM;
        }

        /* Set MX Non Conforming Packet Command */
        /* bit 23 */
        switch(entryPtr->redPcktCmd)
        {
            case CPSS_DXCH3_POLICER_NON_CONFORM_CMD_DROP_E:
                hwData |= (1 << 23);
                break;
            case CPSS_DXCH3_POLICER_NON_CONFORM_CMD_NO_CHANGE_E:
                break;
            default:
                return GT_BAD_PARAM;
        }

        /* Calculate hw value for Modify DSCP operation */
        switch(entryPtr->modifyDscp)
        {
            case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
                break;
            case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
                hwData |= (1 << 25);
                break;
            default:
                return GT_BAD_PARAM;
        }

        /* Calculate hw value for Modify UP operation */
        switch(entryPtr->modifyUp)
        {
            case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
                break;
            case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
                hwData |= (1 << 27);
                break;
            default:
                return GT_BAD_PARAM;
        }

        /* Set Modify DP operation */
        /* bit 28 */
        switch(entryPtr->modifyDp)
        {
            case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
                break;
            case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
                hwData |= (1 << 28);
                break;
            default:
                return GT_BAD_PARAM;
        }
    }

    regAddr = 0x18 +
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerTblAccessDataReg;
    /* Set Policer Table Access Data 6 Register */
    retStatus = prvCpssDrvHwPpWriteRegister(devNum, regAddr, hwData);
    if (retStatus != GT_OK)
    {
        return retStatus;
    }

    /**********/
    /* Word 7 */
    /**********/

    /* for xCat and above Ingress Policers */
    if (PRV_CPSS_DXCH_POLICER_INGRESS_STAGE_CHECK_MAC(stage))
    {
        /* Set 5 MSB of Qos Profile */
        /* bits 0:4 */
        hwData = ((entryPtr->qosProfile >> 2) & 0x1F);

        regAddr = 0x1C +
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerTblAccessDataReg;

        /* Set Ingress Policer Table Access Data 7 Register */
        retStatus = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 5, hwData);
        if (retStatus != GT_OK)
        {
            return retStatus;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerMeteringEntrySet
*
* DESCRIPTION:
*      Sets Metering Policer Entry.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*       entryIndex      - index of Policer Metering Entry.
*                         Range: see datasheet for specific device.
*       entryPtr        - pointer to the metering policer entry going to be set.
*
* OUTPUTS:
*       tbParamsPtr     - pointer to actual policer token bucket parameters.
*                         The token bucket parameters are returned as output
*                         values. This is due to the hardware rate resolution,
*                         the exact rate or burst size requested may not be
*                         honored. The returned value gives the user the
*                         actual parameters configured in the hardware.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on hardware error.
*       GT_TIMEOUT                  - on time out of Policer Tables
*                                     indirect access.
*       GT_BAD_PARAM                - wrong devNum, stage or entryIndex or
*                                     entry parameters.
*       GT_OUT_OF_RANGE             - on Billing Entry Index out of range.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerMeteringEntrySet
(
    IN  GT_U8                                   devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN  GT_U32                                  entryIndex,
    IN  CPSS_DXCH3_POLICER_METERING_ENTRY_STC   *entryPtr,
    OUT CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT  *tbParamsPtr
)
{
    return cpssDxChPolicerPortGroupMeteringEntrySet(
                                            devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            stage,
                                            entryIndex,
                                            entryPtr,
                                            tbParamsPtr);
}


/*******************************************************************************
* prvCpssDxChPolicerMeteringEntryGet
*
* DESCRIPTION:
*      Gets Metering Policer Entry for xCat and above.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number.
*       portGroupId     - the portGroupId. to support multi-port-groups device
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*       entryIndex      - index of Policer Metering Entry.
*                         Range: see datasheet for specific device.
*
* OUTPUTS:
*       entryPtr        - pointer to the requested metering policer entry.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - wrong devNum or stage or entryIndex.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*       GT_BAD_STATE                - on bad value in a entry.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvCpssDxChPolicerMeteringEntryGet
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  portGroupId,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN  GT_U32                                  entryIndex,
    OUT CPSS_DXCH3_POLICER_METERING_ENTRY_STC   *entryPtr
)
{
    GT_U32              regAddr;        /* register address */
    GT_U32              hwData[5];      /* data read from HW */
    GT_U32              hwRateType0;    /* HW rate type 0 value */
    GT_U32              hwRateType1;    /* HW rate type 1 value */
    GT_U32              hwRate0;        /* HW rate 0 value */
    GT_U32              hwRate1;        /* HW rate 1 value */
    GT_U32              hwBurstSize0;   /* HW Max Burst Size 0 value */
    GT_U32              hwBurstSize1;   /* HW Max Burst Size 1 value */
    GT_U32              redPcktCmdVal;      /* Hw value for red packet command */
    GT_U32              yellowPcktCmdVal;   /* Hw value for yellow packet command */
    GT_U32              meterColorModeVal;  /* Hw value for meter color mode */
    GT_U32              meterModeVal;       /* Hw value for meter mode */
    GT_U32              mngCounterSetVal;   /* Hw value for management counter set */
    GT_STATUS           retStatus;      /* generic return status code */


    /*********************************************************/
    /* Read Word 3,4,5,6 of requesred Policer Metering Entry */
    /* Read Also word 7 in case of Ingress policer in XCAT and above  */
    /*********************************************************/
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        PLR[stage].policerMeteringCountingTbl + (entryIndex * 0x20) + 12;

    retStatus = prvCpssDrvHwPpPortGroupReadRam(devNum, portGroupId, regAddr, 5, &hwData[0]);
    if (retStatus != GT_OK)
    {
        return retStatus;
    }

    /**********/
    /* Word 3 */
    /**********/

    /* Get RateType0 value */
    hwRateType0 = (hwData[0] >> 26) & 0x7;
    /* Get RateType1 value */
    hwRateType1 = (hwData[0] >> 29);

    /**********/
    /* Word 4 */
    /**********/

    /* Get Rate 0 (CIR) */
    /* bits 0:16 */
    hwRate0 = (hwData[1] & 0x1FFFF);
    /* Set 15 LSB of Rate 1 (CIR) */
    /* bits 17:31 */
    hwRate1 = (hwData[1] >> 17);

    /**********/
    /* Word 5 */
    /**********/

    /* Get 2 MSB of Rate 1 (CIR) */
    /* bits 0:1 */
    hwRate1 |= ((hwData[2] & 0x3) << 15);
    /* Get Maximum burst size for bucket 0 */
    /* bits 2:17 */
    hwBurstSize0 = (hwData[2] >> 2) & 0xFFFF;
    /* Get 14 LSB of Maximum burst size for bucket 1 */
    /* bits 18:31 */
    hwBurstSize1 = (hwData[2] >> 18);

    /**********/
    /* Word 6 */
    /**********/

    /* Get 2 MSB of Maximum burst size for bucket 1 */
    /* bits 0:1 */
    hwBurstSize1 |= ((hwData[3] & 0x3) << 14);
    /* Get Color Mode hw value */
    /* bit 2 */
    meterColorModeVal = (hwData[3] >> 2) & 0x1;
    /* Get Meter Mode hw value */
    /* bit 3 */
    meterModeVal = (hwData[3] >> 3) & 0x1;
    /* Get Management Counter Set hw value */
    /* bits 4:5 */
    mngCounterSetVal = (hwData[3] >> 4) & 0x3;
    /* Get pointer to related counting entry */
    /* bits 6:21 */
    entryPtr->countingEntryIndex = (hwData[3] >> 6) & 0xFFFF;

    /* check Policer stage: Ingress or Egress */
    if (PRV_CPSS_DXCH_POLICER_INGRESS_STAGE_CHECK_MAC(stage))
    { /* IPLR */

        /* Get Policer commands hw value for Red Packet */
        /* bits 22:23 */
        redPcktCmdVal = (hwData[3] >> 22) & 0x3;
        /* Get Policer commands hw value for Yellow Packet */
        /* bits 24:25 */
        yellowPcktCmdVal = (hwData[3] >> 24) & 0x3;
        /* Get 2 LSB of Qos Profile */
        /* bits 30:31 */
        entryPtr->qosProfile = (hwData[3] >> 30);


        /* Get Modify DSCP operation */
        /* bits 26:27 */
        switch((hwData[3] >> 26) & 0x3)
        {
            case 0:
                entryPtr->modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
                break;
            case 1:
                entryPtr->modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
                break;
            case 2:
                entryPtr->modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
                break;
            default:
                return GT_BAD_STATE;
        }

        /* Get Modify UP operation */
        /* bits 28:29 */
        switch((hwData[3] >> 28) & 0x3)
        {
            case 0:
                entryPtr->modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
                break;
            case 2:
                entryPtr->modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
                break;
            case 1:
                entryPtr->modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
                break;
            default:
                return GT_BAD_STATE;
        }

        /**********/
        /* Word 7 */
        /**********/

        /* Get 5 MSB of Qos Profile */
        /* bits 0:4 */
        entryPtr->qosProfile |= ((hwData[4] & 0x1F) << 2);
    }
    else
    { /* EPLR */

        /* Get Remark mode */
        /* bit 22 */
        switch ((hwData[3] >> 22) & 0x1)
        {
            case 0:
                entryPtr->remarkMode = CPSS_DXCH_POLICER_REMARK_MODE_L2_E;
                break;
            case 1:
                entryPtr->remarkMode = CPSS_DXCH_POLICER_REMARK_MODE_L3_E;
                break;
            default:
                return GT_BAD_STATE;
        }

        /* Get MX Non Conforming Packet Command */
        /* bit 23 */
        switch((hwData[3] >> 23) & 0x1)
        {
            case 0:
                redPcktCmdVal = CPSS_DXCH3_POLICER_NON_CONFORM_CMD_NO_CHANGE_E;
                break;
            case 1:
                redPcktCmdVal = CPSS_DXCH3_POLICER_NON_CONFORM_CMD_DROP_E;
                break;
            default:
                return GT_BAD_STATE;
        }

        /* Get Modify DSCP operation */
        /* bit 25 */
        entryPtr->modifyDscp = ((hwData[3] >> 25) & 0x1) ?
                        CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E :
                        CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;

        /* Get Modify UP operation */
        /* bit 27 */
        entryPtr->modifyUp = ((hwData[3] >> 27) & 0x1) ?
                        CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E :
                        CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;

        /* Get Modify DP operation */
        /* bit 28 */
        entryPtr->modifyDp = ((hwData[3] >> 28) & 0x1) ?
                        CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E :
                        CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;

        /* Not relevant for Egress Policer */
        yellowPcktCmdVal = 0;
    }

    /* Get Policer command for Red Packet */
    switch(redPcktCmdVal)
    {
        case 0:
            entryPtr->redPcktCmd =
                                CPSS_DXCH3_POLICER_NON_CONFORM_CMD_NO_CHANGE_E;
            break;
        case 1:
            entryPtr->redPcktCmd = CPSS_DXCH3_POLICER_NON_CONFORM_CMD_DROP_E;
            break;
        case 2:
            entryPtr->redPcktCmd = CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_E;
            break;
        case 3:
            entryPtr->redPcktCmd =
                CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E;
            break;
        default:
            return GT_BAD_STATE;
    }

    /* Get Policer command for Yellow Packet */
    switch(yellowPcktCmdVal)
    {
        case 0:
            entryPtr->yellowPcktCmd =
                                CPSS_DXCH3_POLICER_NON_CONFORM_CMD_NO_CHANGE_E;
            break;
        case 1:
            entryPtr->yellowPcktCmd = CPSS_DXCH3_POLICER_NON_CONFORM_CMD_DROP_E;
            break;
        case 2:
            entryPtr->yellowPcktCmd =
                                CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_E;
            break;
        case 3:
            entryPtr->yellowPcktCmd =
                                CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E;
            break;
        default:
            return GT_BAD_STATE;
    }

    /* Get Color Mode */
    entryPtr->meterColorMode = (meterColorModeVal) ?
                                            CPSS_POLICER_COLOR_AWARE_E :
                                            CPSS_POLICER_COLOR_BLIND_E;

    /* Get Meter Mode */
    entryPtr->meterMode = (meterModeVal) ?
                             CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E :
                             CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E;

    /* Get Management Counter Set */
    switch(mngCounterSetVal)
    {
        case 0:
            entryPtr->mngCounterSet = CPSS_DXCH3_POLICER_MNG_CNTR_SET0_E;
            break;
        case 1:
            entryPtr->mngCounterSet = CPSS_DXCH3_POLICER_MNG_CNTR_SET1_E;
            break;
        case 2:
            entryPtr->mngCounterSet = CPSS_DXCH3_POLICER_MNG_CNTR_SET2_E;
            break;
        case 3:
            entryPtr->mngCounterSet = CPSS_DXCH3_POLICER_MNG_CNTR_DISABLED_E;
            break;
        default:
            return GT_FAIL; /* never should be reached*/
    }

    /****************************************************/
    /* Convert Token Backet Parameters in the SW format */
    /****************************************************/
    switch(entryPtr->meterMode)
    {
        case CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E:
            entryPtr->tokenBucketParams.srTcmParams.cir = hwRate0;
            entryPtr->tokenBucketParams.srTcmParams.cbs = hwBurstSize0;
            entryPtr->tokenBucketParams.srTcmParams.ebs = hwBurstSize1;

            retStatus = prvCpssDxCh3PolicerSwTbCalculate(
                entryPtr->meterMode,
                hwRateType0,
                0,
                &(entryPtr->tokenBucketParams),
                &(entryPtr->tokenBucketParams));

            if (retStatus != GT_OK)
            {
                return retStatus;
            }
            break;

        case CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E:
            entryPtr->tokenBucketParams.trTcmParams.cir = hwRate0;
            entryPtr->tokenBucketParams.trTcmParams.pir = hwRate1;
            entryPtr->tokenBucketParams.trTcmParams.cbs = hwBurstSize0;
            entryPtr->tokenBucketParams.trTcmParams.pbs = hwBurstSize1;

            retStatus = prvCpssDxCh3PolicerSwTbCalculate(
                entryPtr->meterMode,
                hwRateType0,
                hwRateType1,
                &(entryPtr->tokenBucketParams),
                &(entryPtr->tokenBucketParams));

            if (retStatus != GT_OK)
            {
                return retStatus;
            }
            break;

        default:
            return GT_FAIL; /* never should be reached*/
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerMeteringEntryGet
*
* DESCRIPTION:
*      Gets Metering Policer Entry configuration.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*       entryIndex      - index of Policer Metering Entry.
*                         Range: see datasheet for specific device.
*
* OUTPUTS:
*       entryPtr        - pointer to the requested metering policer entry.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - wrong devNum or stage or entryIndex.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*       GT_BAD_STATE                - on bad value in a entry.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerMeteringEntryGet
(
    IN  GT_U8                                   devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN  GT_U32                                  entryIndex,
    OUT CPSS_DXCH3_POLICER_METERING_ENTRY_STC   *entryPtr
)
{
    return cpssDxChPolicerPortGroupMeteringEntryGet(
                                            devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            stage,
                                            entryIndex,
                                            entryPtr);
}

/*******************************************************************************
* cpssDxCh3PolicerEntryMeterParamsCalculate
*
* DESCRIPTION:
*      Calculates Token Bucket parameters in the Application format without
*      HW update.
*      The token bucket parameters are returned as output values. This is
*      due to the hardware rate resolution, the exact rate or burst size
*      requested may not be honored. The returned value gives the user the
*      actual parameters configured in the hardware.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*       tbInParamsPtr   - pointer to Token bucket input parameters.
*       meterMode       - Meter mode (SrTCM or TrTCM).
*
* OUTPUTS:
*       tbOutParamsPtr  - pointer to Token bucket output paramters.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_BAD_PARAM                - on wrong devNum or meterMode.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerEntryMeterParamsCalculate
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_POLICER_STAGE_TYPE_ENT        stage,
    IN  CPSS_DXCH3_POLICER_METER_MODE_ENT       meterMode,
    IN  CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT  *tbInParamsPtr,
    OUT CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT  *tbOutParamsPtr
)
{
    GT_U32      rateType0;  /* Rate Type 0 */
    GT_U32      rateType1;  /* Rate Type 1 */
    GT_STATUS   retStatus;  /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(tbInParamsPtr);
    CPSS_NULL_PTR_CHECK_MAC(tbOutParamsPtr);

    /* Token Backet Calculation */
    switch(meterMode)
    {
        case CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E:
            retStatus = prvCpssDxCh3PolicerHwTbCalculate(
                      devNum,
                      stage,
                      tbInParamsPtr->srTcmParams.cir,
                      tbInParamsPtr->srTcmParams.cbs,
                      tbInParamsPtr->srTcmParams.ebs,
                      &(tbOutParamsPtr->srTcmParams.cir), /* HW Rate */
                      &(tbOutParamsPtr->srTcmParams.cbs), /* HW MaxBurstSize0 */
                      &(tbOutParamsPtr->srTcmParams.ebs), /* HW MaxBurstSize1 */
                      &rateType0);                        /* HW RateType */

            if (retStatus == GT_OK)
            {
                return prvCpssDxCh3PolicerSwTbCalculate(meterMode,
                                                         rateType0,
                                                         0,
                                                         tbOutParamsPtr,
                                                         tbOutParamsPtr);
            }
            break;

        case CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E:
            retStatus = prvCpssDxCh3PolicerHwTbCalculate(
                      devNum,
                      stage,
                      tbInParamsPtr->trTcmParams.cir,
                      tbInParamsPtr->trTcmParams.cbs,
                      0,                        /* Not exists in the TrTCM */
                      &(tbOutParamsPtr->trTcmParams.cir), /* HW Rate0 */
                      &(tbOutParamsPtr->trTcmParams.cbs), /* HW MaxBurstSize0 */
                      NULL,                     /* Not exists in the TrTCM */
                      &rateType0);                        /* HW RateType0 */

            if (retStatus != GT_OK)
            {
                return retStatus;
            }

            retStatus = prvCpssDxCh3PolicerHwTbCalculate(
                      devNum,
                      stage,
                      tbInParamsPtr->trTcmParams.pir,
                      tbInParamsPtr->trTcmParams.pbs,
                      0,                        /* Not exists in the TrTCM */
                      &(tbOutParamsPtr->trTcmParams.pir), /* HW Rate1 */
                      &(tbOutParamsPtr->trTcmParams.pbs), /* HW MaxBurstSize1 */
                      NULL,                     /* Not exists in the TrTCM */
                      &rateType1);                      /* HW RateType1 */

            if (retStatus == GT_OK)
            {
                return prvCpssDxCh3PolicerSwTbCalculate(meterMode,
                                                        rateType0,
                                                        rateType1,
                                                        tbOutParamsPtr,
                                                        tbOutParamsPtr);
            }

            break;

        default:
            return GT_BAD_PARAM;
    }

    return retStatus;
}

/*******************************************************************************
* prvCpssDxChPolicerBillingCntrToHwConvert
*
* DESCRIPTION:
*       Convert counters part and billing mode of Billing entry
*       of xCat and above to the HW format.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum               - device number.
*       billingCntrPtr       - pointer to the Billing Counters Entry.
*       billingCntrModeHwVal - HW value for counters mode
*
* OUTPUTS:
*       hwDataPtr      - pointer to the HW data.
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvCpssDxChPolicerBillingCntrToHwConvert
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH3_POLICER_BILLING_ENTRY_STC    *billingCntrPtr,
    IN  GT_U32                                   billingCntrModeHwVal,
    OUT GT_U32                                  *hwDataPtr
)
{

    hwDataPtr[0] = billingCntrPtr->greenCntr.l[0];
    hwDataPtr[1] = (billingCntrPtr->greenCntr.l[1]          |
                ((billingCntrPtr->yellowCntr.l[0] & 0x3FFFFF) << 10));
    hwDataPtr[2] = (billingCntrPtr->yellowCntr.l[0] >> 22) |
                (billingCntrPtr->yellowCntr.l[1] << 10) |
                ((billingCntrPtr->redCntr.l[0] & 0xFFF) << 20);

    hwDataPtr[3] = (billingCntrPtr->redCntr.l[0] >> 12)    |
                (billingCntrPtr->redCntr.l[1] << 20);

    /* convert counters mode */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)    
    {
        /* bits 126..127 */
        hwDataPtr[3] |= (billingCntrModeHwVal << 30);
    }
    else
    {
        /* bits 140..141 */
        hwDataPtr[4] = (billingCntrModeHwVal << 12);
    }
}

/*******************************************************************************
* prvCpssDxChPolicerBillingEntrySet
*
* DESCRIPTION:
*       Sets Policer Billing Counters of xCat and above.
*       Billing is the process of counting the amount of traffic that belongs
*       to a flow. This process can be activated only by metering and is used
*       either for statistical analysis of a flow's traffic or for actual
*       billing.
*       The billing counters are free-running no-sticky counters.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*       entryIndex      - index of Policer Billing Counters Entry.
*                         Range: see datasheet for specific device.
*       billingCntrPtr  - pointer to the Billing Counters Entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_TIMEOUT                  - on time out upon counter reset by
*                                     indirect access.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or entryIndex.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       The packet is counted per outgoing DP or Conformance Level. When
*       metering is disabled, billing is always done according to packet's
*       Drop Precedence.
*       When metering is activated per source port, Billing counting cannot
*       be enabled.
*       None-zeroed setting of Billing Counters while metring/counting is
*       enabled may cause data coherency problems.
*
*       In order to set Billing entry under traffic perform the following
*       algorithm:
*           - disconnect entryes from traffic
*           - call cpssDxChPolicerCountingWriteBackCacheFlush
*           - set new values
*           - connect entries back to traffic
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPolicerBillingEntrySet
(
    IN  GT_U8                                   devNum,
    IN  GT_PORT_GROUPS_BMP                      portGroupsBmp,
    IN  CPSS_DXCH_POLICER_STAGE_TYPE_ENT        stage,
    IN  GT_U32                                  entryIndex,
    IN  CPSS_DXCH3_POLICER_BILLING_ENTRY_STC    *billingCntrPtr
)
{
    GT_U32      regAddr;                /* register address */
    GT_U32      billingCntrModeHwVal;   /* Billing counter mode hw value */
    GT_U32      hwData[8];              /* HW data */
    GT_U32      portGroupId;            /* port group id */
    GT_U32      wordAddr;               /* entry word's address */
    GT_U16      portGroupNum;           /* number of port groups      */
    GT_U16      portGroupIndex;         /* index of port group in BMP */
    GT_STATUS   rc;                     /* return code */
    CPSS_DXCH3_POLICER_BILLING_ENTRY_STC    billingCntr; /* counters values */

    /* Check Billing Counters mode */
    switch (billingCntrPtr->billingCntrMode)
    {
        case CPSS_DXCH3_POLICER_BILLING_CNTR_1_BYTE_E:
            billingCntrModeHwVal = 0;
            break;
        case CPSS_DXCH3_POLICER_BILLING_CNTR_16_BYTES_E:
            billingCntrModeHwVal = 1;
            break;
        case CPSS_DXCH3_POLICER_BILLING_CNTR_PACKET_E:
            billingCntrModeHwVal = 2;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* Check MSB of word 1 of counters */
    if ((billingCntrPtr->greenCntr.l[1] >> 10) |
        (billingCntrPtr->yellowCntr.l[1] >> 10) |
        (billingCntrPtr->redCntr.l[1] >> 10))
    {
        return GT_BAD_PARAM;
    }

    cpssOsMemSet(hwData, 0, sizeof(hwData));

    if (!PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        /* convert entry */
        prvCpssDxChPolicerBillingCntrToHwConvert(devNum, billingCntrPtr,
                                                 billingCntrModeHwVal, hwData);

        /* Check if Policer Counting memory errata is enabled */
         if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                  PRV_CPSS_DXCH_LION_POLICER_COUNTING_MEM_ACCESS_WA_E))
         {
             /* Indirect access to counting memory should be used */


             /* check that the previous action is finished in each portGroup, do busy wait */
             rc = prvCpssDxCh3PolicerAccessControlBusyWait(
                                            devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            stage);
             if (rc != GT_OK)
             {
                 return rc;
             }

             /* Get Ingress Policer Table Access Data 0 Register Address */
             regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                             PLR[stage].policerTblAccessDataReg;

             /* Write data to Policer Table Access Data registers */
             rc = prvCpssDrvHwPpWriteRam(devNum, regAddr, 8, hwData);
             if (rc != GT_OK)
             {
                 return rc;
             }

             /* Trigger the read & write action */
             return prvCpssDxCh3PolicerInternalTableAccess(devNum,
                                   CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                   stage,
                                   entryIndex,
                                   PRV_CPSS_DXCH3_POLICER_CNTR_READ_AND_WRITE_E);
         }
         else
         {
             regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                             PLR[stage].policerCountingTbl + (entryIndex * 0x20);

             /* set first 5 words with counters and billing mode */
             rc = prvCpssDrvHwPpWriteRam(
                 devNum, regAddr, 5/* words*/, hwData);
             if (rc != GT_OK)
             {
                 return rc;
             }

             /* verify that all words were written before write last one.
                because last one triggers write of whole entry */
             GT_SYNC;

             /* set last word 7 - Counter Mode bit#5 is always 0 (billing).
               this set triggers update of whole entry. */
             wordAddr = regAddr + 0x1C;
             rc = prvCpssDrvHwPpWriteRegister(devNum, wordAddr, 0);

             return rc;
         }
    }

    /* calculate number of port groups */
    portGroupNum = 0;
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        portGroupNum ++;
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    if (portGroupNum == 0)
    {
        /* nothing to do */
        return GT_OK;
    }

    /* Check if Policer Counting memory errata is enabled */
    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                                               PRV_CPSS_DXCH_LION_POLICER_COUNTING_MEM_ACCESS_WA_E))
    {
        /* check that the previous action is finished in each portGroup, do busy wait */
        rc = prvCpssDxCh3PolicerAccessControlBusyWait(devNum, portGroupsBmp, stage);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* Get Ingress Policer Table Access Data 0 Register Address */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        PLR[stage].policerTblAccessDataReg;

    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        PLR[stage].policerCountingTbl + (entryIndex * 0x20);
    }

    /* loop on all port groups to set the counter value */
    portGroupIndex = 0;
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        /* calculate average value for green counter */
        prvCpssDxChPortGroupCounterAverageValueGet(portGroupNum, portGroupIndex,
                                                   billingCntrPtr->greenCntr,
                                                   &billingCntr.greenCntr);

        /* calculate average value for yellow counter */
        prvCpssDxChPortGroupCounterAverageValueGet(portGroupNum, portGroupIndex,
                                                   billingCntrPtr->yellowCntr,
                                                   &billingCntr.yellowCntr);

        /* calculate average value for red counter */
        prvCpssDxChPortGroupCounterAverageValueGet(portGroupNum, portGroupIndex,
                                                   billingCntrPtr->redCntr,
                                                   &billingCntr.redCntr);

        /* convert entry */
        prvCpssDxChPolicerBillingCntrToHwConvert(devNum, &billingCntr,
                                                 billingCntrModeHwVal, hwData);


        /* Check if Policer Counting memory errata is enabled */
         if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                  PRV_CPSS_DXCH_LION_POLICER_COUNTING_MEM_ACCESS_WA_E))
         {
             /* Indirect access to counting memory should be used */

             /* Write data to Policer Table Access Data registers */
             rc = prvCpssDrvHwPpPortGroupWriteRam(devNum, portGroupId,
                                                  regAddr, 8, hwData);
             if (rc != GT_OK)
             {
                 return rc;
             }

             /* Trigger the read & write action */
             rc = prvCpssDxCh3PolicerInternalTableAccess(devNum,
                                   portGroupId,
                                   stage,
                                   entryIndex,
                                   PRV_CPSS_DXCH3_POLICER_CNTR_READ_AND_WRITE_E);
             if (rc != GT_OK)
             {
                 return rc;
             }
         }
         else
         {
             /* set first 5 words with counters and billing mode */
             rc = prvCpssDrvHwPpPortGroupWriteRam(
                 devNum, portGroupId, regAddr, 5/* words*/, hwData);
             if (rc != GT_OK)
             {
                 return rc;
             }

             /* verify that all words were written before write last one.
                because last one triggers write of whole entry */
             GT_SYNC;

             /* set last word 7 - Counter Mode bit#5 is always 0 (billing).
               this write triggers update of whole entry. */
             wordAddr = regAddr + 0x1C;
             rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId, wordAddr, 0);
             if (rc != GT_OK)
             {
                 return rc;
             }
         }

         portGroupIndex++;

    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerBillingEntrySet
*
* DESCRIPTION:
*       Sets Policer Billing Counters.
*       Billing is the process of counting the amount of traffic that belongs
*       to a flow. This process can be activated only by metering and is used
*       either for statistical analysis of a flow's traffic or for actual
*       billing.
*       The billing counters are free-running no-sticky counters.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*       entryIndex      - index of Policer Billing Counters Entry.
*                         Range: see datasheet for specific device.
*       billingCntrPtr  - pointer to the Billing Counters Entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_TIMEOUT                  - on time out upon counter reset by
*                                     indirect access.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or entryIndex.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       The packet is counted per outgoing DP or Conformance Level. When
*       metering is disabled, billing is always done according to packet's
*       Drop Precedence.
*       When metering is activated per source port, Billing counting cannot
*       be enabled.
*       None-zeroed setting of Billing Counters while metring/counting is
*       enabled may cause data coherency problems.
*
*       In order to set Billing entry under traffic perform the following
*       algorithm:
*           - disconnect entryes from traffic
*           - call cpssDxChPolicerCountingWriteBackCacheFlush
*           - set new values
*           - connect entries back to traffic
*       This API address the same memory area (counters place) as can be set
*       by "cpssDxChIpfixEntrySet".
*
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerBillingEntrySet
(
    IN  GT_U8                                   devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN  GT_U32                                  entryIndex,
    IN  CPSS_DXCH3_POLICER_BILLING_ENTRY_STC    *billingCntrPtr
)
{
    return cpssDxChPolicerPortGroupBillingEntrySet(
                                        devNum,
                                        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                        stage,
                                        entryIndex,
                                        billingCntrPtr);
}

/*******************************************************************************
* prvCpssDxChPolicerBillingEntryGet
*
* DESCRIPTION:
*       Gets Policer Billing Counters of xCat and above.
*       Billing is the process of counting the amount of traffic that belongs
*       to a flow. This process can be activated only by metering and is used
*       either for statistical analysis of a flow's traffic or for actual
*       billing.
*       The billing counters are free-running no-sticky counters.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*
* OUTPUTS:
*       billingCntrPtr  - pointer to the Billing Counters Entry.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_TIMEOUT                  - on time out of IPLR Table indirect access.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or entryIndex.
*       GT_BAD_STATE                - on counter entry type mismatch.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       The packet is counted per outgoing DP or Conformance Level. When
*       metering is disabled, billing is always done according to packet's
*       Drop Precedence.
*       When metering is activated per source port, Billing counting cannot
*       be enabled.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPolicerBillingEntryGet
(
    IN  GT_U8                                   devNum,
    IN  GT_PORT_GROUPS_BMP                      portGroupsBmp,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    OUT CPSS_DXCH3_POLICER_BILLING_ENTRY_STC    *billingCntrPtr
)
{
    GT_U32      regAddr;                /* register address */
    GT_U32      regValue;               /* register hw value */
    GT_U32      billingCntrModeHwVal;   /* Billing counter mode hw value */
    GT_U32      portGroupId;            /* port group id */
    GT_U64      tmp;                    /* temporary variable */
    GT_STATUS   retStatus;              /* generic return status code */

    /******************************************************************/
    /* Get counter value from the corresponding Access Data Registers */
    /******************************************************************/

    /* Get the first active port group */
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(devNum, portGroupsBmp,
                                                        portGroupId);

    /* Get Ingress Policer Table Access Data 0 Register Address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerTblAccessDataReg;

    /* Get Ingress Policer Table Access Data 7 Register */
    regAddr += 28;

    retStatus = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,
                                                    regAddr, &regValue);
    if (retStatus != GT_OK)
    {
        return retStatus;
    }

    /* Check that counter mode is Billing and not IPFIX */
    if ((regValue >> 4) & 0x1)
    {
        return GT_BAD_STATE;
    }

    /* Set the register address back to the register 0 */
    regAddr -= 28;

    retStatus = prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                       regAddr,0,32,
                                                       NULL,
                                                       &billingCntrPtr->greenCntr);
    if (GT_OK != retStatus)
    {
        return retStatus;
    }

    /* Get Ingress Policer Table Access Data 1 Register */
    regAddr += 4;

    retStatus = prvCpssDxChPortGroupsBmpMultiCounterSummary(devNum,
                                portGroupsBmp,regAddr,
                                0,10,
                                &regValue,/* GC: 10 MSbit */
                                10,22,
                                &billingCntrPtr->yellowCntr.l[0],/* YC: 22 LSbit */
                                0,0,NULL);
    if (retStatus != GT_OK)
    {
        return retStatus;
    }

    billingCntrPtr->greenCntr.l[1] += regValue;

    /* Get Ingress Policer Table Access Data 2 Register */
    regAddr += 4;

    retStatus = prvCpssDxChPortGroupsBmpMultiCounterSummary(devNum,
                                portGroupsBmp, regAddr,
                                0,10,
                                &regValue,/* YC: 10 MSbit of first word */
                                10,10,
                                &billingCntrPtr->yellowCntr.l[1],/*YC:10MSbit*/
                                20,12,
                                &billingCntrPtr->redCntr.l[0]);/*RC:12LSbit*/
    if (retStatus != GT_OK)
    {
        return retStatus;
    }

    /* the regValue is sum of bits 0..9 from up to all port groups.
       therefore value of the variable may be more then 10 bits. All MSBs after
       10th should be stored in the MSB word of tmp */
    tmp.l[0] = regValue << 22; /*YC: 10 MSbit of first word*/
    tmp.l[1] = regValue >> 10;

    billingCntrPtr->yellowCntr = prvCpssMathAdd64(billingCntrPtr->yellowCntr, tmp);

    /* Get Ingress Policer Table Access Data 3 Register */
    regAddr += 4;

    retStatus = prvCpssDxChPortGroupsBmpMultiCounterSummary(devNum,
                                portGroupsBmp, regAddr,
                                0,20,
                                &regValue,/*RC:20LSbit*/
                                20,10,
                                &billingCntrPtr->redCntr.l[1],/*RC:10MSbit*/
                                0,0,NULL);
    if (GT_OK != retStatus)
    {
        return retStatus;
    }

    /* the regValue is sum of bits 0..19 from up to all port groups.
       therefore value of the variable may be more then 20 bits. All MSBs after
       20th should be stored in the MSB word of tmp */
    tmp.l[0] = regValue << 12; /*RC: 20 MSbits of first word */
    tmp.l[1] = regValue >> 20;

    billingCntrPtr->redCntr = prvCpssMathAdd64(billingCntrPtr->redCntr, tmp);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        /* Ingress Policer Table Access Data 3 Register */
        retStatus = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,
                                                        regAddr, &regValue);
        if (GT_OK != retStatus)
        {
            return retStatus;
        }

        /* 2 bits of Cntr Mode */
        billingCntrModeHwVal = (regValue >> 30);
    }
    else
    {
        /* Get Ingress Policer Table Access Data 4 Register */
        regAddr += 4;

        retStatus = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,
                                                        regAddr, &regValue);
        if (GT_OK != retStatus)
        {
            return retStatus;
        }

        /* 2 bits of Cntr Mode */
        billingCntrModeHwVal = ((regValue >> 12) & 0x3);
    }

    switch (billingCntrModeHwVal)
    {
        case 0:
            billingCntrPtr->billingCntrMode = CPSS_DXCH3_POLICER_BILLING_CNTR_1_BYTE_E;
            break;
        case 1:
            billingCntrPtr->billingCntrMode = CPSS_DXCH3_POLICER_BILLING_CNTR_16_BYTES_E;
            break;
        case 2:
            billingCntrPtr->billingCntrMode = CPSS_DXCH3_POLICER_BILLING_CNTR_PACKET_E;
            break;
        default:
            return GT_BAD_STATE;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerBillingEntryGet
*
* DESCRIPTION:
*       Gets Policer Billing Counters.
*       Billing is the process of counting the amount of traffic that belongs
*       to a flow. This process can be activated only by metering and is used
*       either for statistical analysis of a flow's traffic or for actual
*       billing.
*       The billing counters are free-running no-sticky counters.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*       entryIndex      - index of Policer Billing Counters Entry.
*                         Range: see datasheet for specific device.
*       reset           - reset flag:
*                         GT_TRUE  - performing read and reset atomic operation.
*                         GT_FALSE - performing read counters operation only.
*
* OUTPUTS:
*       billingCntrPtr  - pointer to the Billing Counters Entry.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_TIMEOUT                  - on time out of IPLR Table indirect access.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or entryIndex.
*       GT_BAD_STATE                - on counter entry type mismatch.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       The packet is counted per outgoing DP or Conformance Level. When
*       metering is disabled, billing is always done according to packet's
*       Drop Precedence.
*       When metering is activated per source port, Billing counting cannot
*       be enabled.
*
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerBillingEntryGet
(
    IN  GT_U8                                   devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN  GT_U32                                  entryIndex,
    IN  GT_BOOL                                 reset,
    OUT CPSS_DXCH3_POLICER_BILLING_ENTRY_STC    *billingCntrPtr
)
{
    return cpssDxChPolicerPortGroupBillingEntryGet(
                                        devNum,
                                        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                        stage,
                                        entryIndex,
                                        reset,
                                        billingCntrPtr);
}

/*******************************************************************************
* cpssDxCh3PolicerQosRemarkingEntrySet
*
* DESCRIPTION:
*       Sets Policer Relative Qos Remarking Entry.
*       The Qos Remarking Entry contains two indexes [0..127] of Qos Profile
*       assigned to Out of profile packets with the Yellow and Red colors.
*       According to this indexes, new QoS parameters: UP,DP,TC and DSCP,
*       are extracted from the Qos Profile Table Entry.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum                      - device number.
*       stage                       - Policer Stage type: Ingress #0 or Ingress #1.
*                                     Stage type is significant for xCat and above devices
*                                     and ignored by DxCh3.
*       qosProfileIndex             - index of Qos Remarking Entry will be set.
*       yellowQosTableRemarkIndex   - QoS profile (index in the Qos Table)
*                                     assigned to Out-Of-Profile packets with
*                                     the Yellow color. Index range is [0..127].
*       redQosTableRemarkIndex      - QoS profile (index in the Qos Table)
*                                     assigned to Out-Of-Profile packets with
*                                     the Red color. Index range is [0..127].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or qosProfileIndex.
*       GT_OUT_OF_RANGE             - on yellowQosTableRemarkIndex and
*                                     redQosTableRemarkIndex out of range.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       Supported only for Ingress Policer.
*
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerQosRemarkingEntrySet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    IN GT_U32                               qosProfileIndex,
    IN GT_U32                               yellowQosTableRemarkIndex,
    IN GT_U32                               redQosTableRemarkIndex
)
{
    GT_U32      regAddr;       /* register address */
    GT_U32      hwDataArr[2];  /* HW data */
    GT_U32      addrAllign; /* address alignment between entries */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_QOS_PROFILE_ID_CHECK_MAC(devNum, qosProfileIndex);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    /* check that the Policer is Ingress */
    if (stage > CPSS_DXCH_POLICER_STAGE_INGRESS_1_E)
    {
        return GT_BAD_PARAM;
    }

    /* Check QoS profile indexes assigned to Yelloow and Red packets */
    if((yellowQosTableRemarkIndex >= PRV_CPSS_DXCH2_QOS_PROFILE_NUM_MAX_CNS) ||
       (redQosTableRemarkIndex >= PRV_CPSS_DXCH2_QOS_PROFILE_NUM_MAX_CNS))
    {
        return GT_OUT_OF_RANGE;
    }

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat A1 and above devices has 8 bytes address alignment */
        addrAllign = 8;
    }
    else
    {
        /* CH3 and xCat/Lion A0 devices has 4 bytes address alignment */
        addrAllign = 4;
    }

    /* Calculate an address of Qos Remarking Entry will be set */
    regAddr = (addrAllign * qosProfileIndex) +
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerQosRemarkingTbl;

    hwDataArr[0] = redQosTableRemarkIndex | (yellowQosTableRemarkIndex << 7);

    /* Set Policer Relative Qos Remarking Entry */
    return prvCpssDrvHwPpWriteRam(devNum, regAddr,
                                  (addrAllign >> 2) /* two words should be
                                            written for xCat A1 and above.
                                            Content of second word is
                                            meaningless and ignored and used
                                            only to trigger the write of the
                                            first word */,
                                   hwDataArr);
}

/*******************************************************************************
* cpssDxCh3PolicerQosRemarkingEntryGet
*
* DESCRIPTION:
*       Gets Policer Relative Qos Remarking Entry.
*       The Qos Remarking Entry contains two indexes [0..127] of Qos Profile
*       assigned to Out of profile packets with the Yellow and Red colours.
*       According to this indexes, new QoS parameters: UP,DP,TC and DSCP,
*       are extracted from the Qos Profile Table Entry.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum              - device number.
*       stage               - Policer Stage type: Ingress #0 or Ingress #1.
*                             Stage type is significant for xCat and above devices
*                             and ignored by DxCh3.
*       qosProfileIndex     - index of requested Qos Remarking Entry.
*
* OUTPUTS:
*       yellowQosTableRemarkIndexPtr    - pointer to the QoS profile (index in
*                                         the Qos Table) assigned to
*                                         Out-Of-Profile packets with the
*                                         Yellow color. Index range is [0..127].
*       redQosTableRemarkIndexPtr       - pointer to the QoS profile (index in
*                                         the Qos Table) assigned to
*                                         Out-Of-Profile packets with the Red
*                                         color. Index range is [0..127].
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum or qosProfileIndex.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       Supported only for Ingress Policer.
*
*******************************************************************************/
GT_STATUS cpssDxCh3PolicerQosRemarkingEntryGet
(
    IN  GT_U8                               devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    IN GT_U32                               qosProfileIndex,
    OUT GT_U32                              *yellowQosTableRemarkIndexPtr,
    OUT GT_U32                              *redQosTableRemarkIndexPtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      hwData;     /* HW data */
    GT_STATUS   retStatus;  /* generic return status code */
    GT_U32      addrAllign; /* address alignment between entries */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_QOS_PROFILE_ID_CHECK_MAC(devNum, qosProfileIndex);
    CPSS_NULL_PTR_CHECK_MAC(yellowQosTableRemarkIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(redQosTableRemarkIndexPtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    /* check that the Policer is Ingress */
    if (stage > CPSS_DXCH_POLICER_STAGE_INGRESS_1_E)
    {
        return GT_BAD_PARAM;
    }

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat A1 and above devices has 8 bytes address alignment */
        addrAllign = 8;
    }
    else
    {
        /* CH3 and xCat/Lion A0 devices has 4 bytes address alignment */
        addrAllign = 4;
    }

    /* Calculate an address of Qos Remarking Entry */
    regAddr = (addrAllign * qosProfileIndex) +
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerQosRemarkingTbl;

    /* Get Policer Relative Qos Remarking Entry */
    retStatus = prvCpssDrvHwPpReadRam(devNum, regAddr, 1, &hwData);

    if(GT_OK == retStatus)
    {
        *redQosTableRemarkIndexPtr      = hwData & 0x7F;
        *yellowQosTableRemarkIndexPtr   = (hwData >> 7) & 0x7F;
    }

    return retStatus;
}

/*******************************************************************************
* cpssDxChPolicerEgressQosRemarkingEntrySet
*
* DESCRIPTION:
*       Sets Egress Policer Re-Marking table Entry.
*       Re-marking is the process of assigning new QoS parameters to the
*       packet, at the end of the metering process, based on the
*       Conformance Level the packet is assigned by metering process.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum              - device number.
*       remarkTableType     - Remark table type: DSCP, EXP or TC/UP.
*       remarkParamValue    - QoS parameter value.
*       confLevel           - Conformance Level: Green, Yellow, Red.
*       qosParamPtr         - pointer to the Re-Marking Entry going to be set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_HW_ERROR         - on hardware error.
*       GT_BAD_PARAM        - on wrong devNum, remarkTableType,
*                                remarkParamValue or confLevel.
*                              -------------------
*                             |QoS param | Range  |
*                             |----------|--------|
*                             |  DSCP    | [0:63] |
*                             |  TC/UP   | [0:7]  |
*                             |  EXP     | [0:7]  |
*                              -------------------
*       GT_BAD_PTR                               - on NULL pointer.
*       GT_OUT_OF_RANGE     - on QoS parameter out of range
*       GT_NOT_APPLICABLE_DEVICE                 - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPolicerEgressQosRemarkingEntrySet
(
    IN GT_U8                                        devNum,
    IN  CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_ENT     remarkTableType,
    IN GT_U32                                       remarkParamValue,
    IN CPSS_DP_LEVEL_ENT                            confLevel,
    IN CPSS_DXCH_POLICER_QOS_PARAM_STC              *qosParamPtr
)
{
    GT_U32      hwData;     /* data will be written to HW */
    GT_U32      offset;     /* bit offset in the specific word of Entry */
    GT_U32      length;     /* length of bit field */
    GT_U32      wordNum;    /* word number inside the entry */
    GT_U32      entryIndex; /* entry index in the memory  */
    GT_U32      dpVal;      /* hw value of dp             */
    GT_STATUS   retStatus;  /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(qosParamPtr);


    /* Calculate entry index in case of correct input parameters. */
    switch(remarkTableType)
    {
        case CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_DSCP_E:
            if (remarkParamValue > 63)
            {
                return GT_BAD_PARAM;
            }
            entryIndex = remarkParamValue;

            break;
        case CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_EXP_E:
            if (remarkParamValue > 7)
            {
                return GT_BAD_PARAM;
            }
            entryIndex = 64 + remarkParamValue;
            break;
        case CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_TC_UP_E:
            if (remarkParamValue > 7)
            {
                return GT_BAD_PARAM;
            }
            entryIndex = 72 + remarkParamValue;
            break;
        default:
            return GT_BAD_PARAM;
    }

    if((qosParamPtr->dscp > 63) || (qosParamPtr->up > 7) ||
       (qosParamPtr->exp > 7))
    {
        return GT_OUT_OF_RANGE;
    }

    switch (qosParamPtr->dp)
    {
        case CPSS_DP_GREEN_E:
            dpVal = 0;
            break;
        case CPSS_DP_YELLOW_E:
            dpVal = 1;
            break;
        case CPSS_DP_RED_E:
            dpVal = 2;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* Convert Re-Marking Entry data going to be set to the HW format */
    hwData = (qosParamPtr->dscp)| (dpVal << 6) | ((qosParamPtr->up) << 11) |
             ((qosParamPtr->exp) << 14);

    /************************/
    /* Set Re-Marking Entry */
    /************************/
    switch(confLevel)
    {
        case CPSS_DP_YELLOW_E:
            offset = 17;
            length = 15;
            wordNum = 0;

            /* write the 2 MSB bits of yellow conformance level */
            /* word 1, bits 0:1 */
            retStatus = prvCpssDxChWriteTableEntryField(
                                    devNum,
                                    PRV_CPSS_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E,
                                    entryIndex,
                                    1,  /* wordNum */
                                    0,  /* field offset*/
                                    2,  /* field length */
                                    (hwData >> 15));
            if (GT_OK != retStatus)
            {
                return retStatus;
            }
            break;
        case CPSS_DP_GREEN_E:
            wordNum = 0;
            offset = 0;
            length = 17;
            break;
        case CPSS_DP_RED_E:
            wordNum = 1;
            offset = 2;
            length = 17;
            break;
        default:
            return GT_BAD_PARAM;
    }

    return  prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E,
                                            entryIndex,
                                            wordNum, /* wordNum */
                                            offset,  /* field offset*/
                                            length,  /* field length */
                                            hwData);
}

/*******************************************************************************
* cpssDxChPolicerEgressQosRemarkingEntryGet
*
* DESCRIPTION:
*       Gets Egress Policer Re-Marking table Entry.
*       Re-marking is the process of assigning new QoS parameters to the
*       packet, at the end of the metering process, based on the
*       Conformance Level the packet is assigned by metering process.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum              - device number.
*       remarkTableType     - Remark table type: DSCP, EXP or TC/UP.
*       remarkParamValue    - QoS parameter value.
*       confLevel           - Conformance Level: Green, Yellow, Red.
*
* OUTPUTS:
*       qosParamPtr         - pointer to the requested Re-Marking Entry.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_HW_ERROR         - on hardware error.
*       GT_BAD_PARAM        - on wrong devNum, remarkTableType,
*                                remarkParamValue or confLevel.
*                              -------------------
*                             |QoS param | Range  |
*                             |----------|--------|
*                             |  DSCP    | [0:63] |
*                             |  TC/UP   | [0:7]  |
*                             |  EXP     | [0:7]  |
*                              -------------------
*       GT_BAD_PTR                               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE                 - on not applicable device
*       GT_BAD_STATE                            - on bad value in a entry.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPolicerEgressQosRemarkingEntryGet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_ENT     remarkTableType,
    IN  GT_U32                                      remarkParamValue,
    IN  CPSS_DP_LEVEL_ENT                           confLevel,
    OUT CPSS_DXCH_POLICER_QOS_PARAM_STC             *qosParamPtr
)
{

    GT_U32      hwData[2];  /* data will be written to HW */
    GT_U32      entryIndex; /* entry index in the memory */
    GT_U32      entryValue; /* Re-Marking Eentry value */
    GT_U32      dpVal;      /* hw value of dp           */
    GT_STATUS   retStatus;  /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(qosParamPtr);


    /* Calculate entry index in case of correct input parameters. */
    switch(remarkTableType)
    {
        case CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_DSCP_E:
            if (remarkParamValue > 63)
            {
                return GT_BAD_PARAM;
            }
            entryIndex = remarkParamValue;

            break;
        case CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_EXP_E:
            if (remarkParamValue > 7)
            {
                return GT_BAD_PARAM;
            }
            entryIndex = 64 + remarkParamValue;
            break;
        case CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_TC_UP_E:
            if (remarkParamValue > 7)
            {
                return GT_BAD_PARAM;
            }
            entryIndex = 72 + remarkParamValue;
            break;
        default:
            return GT_BAD_PARAM;
    }


    /************************/
    /* Get Re-Marking Entry */
    /************************/
    switch(confLevel)
    {
        case CPSS_DP_YELLOW_E:
            /* write the 2 MSB bits of yellow conformance level */
            /* word 1, bits 0:1 */
            retStatus = prvCpssDxChReadTableEntry(
                                    devNum,
                                    PRV_CPSS_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E,
                                    entryIndex,
                                    &hwData[0]);
            if (GT_OK != retStatus)
            {
                return retStatus;
            }
            entryValue = (hwData[0] >> 17) | ((hwData[1] & 0x3) << 15);
            break;

        case CPSS_DP_GREEN_E:
            retStatus = prvCpssDxChReadTableEntryField(
                                    devNum,
                                    PRV_CPSS_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E,
                                    entryIndex,
                                    0,  /* wordNum */
                                    0,  /* field offset*/
                                    17,  /* field length */
                                    &entryValue);
            if (GT_OK != retStatus)
            {
                return retStatus;
            }
            break;

        case CPSS_DP_RED_E:
            retStatus = prvCpssDxChReadTableEntryField(
                                    devNum,
                                    PRV_CPSS_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E,
                                    entryIndex,
                                    1,  /* wordNum */
                                    2,  /* field offset*/
                                    17,  /* field length */
                                    &entryValue);
            if (GT_OK != retStatus)
            {
                return retStatus;
            }
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* Gather QoS parameters from the Re-Marking Entry */
    qosParamPtr->dscp   = entryValue & 0x3F;
    qosParamPtr->up     = (entryValue >> 11) & 0x7;
    qosParamPtr->exp    = (entryValue >> 14) & 0x7;
    dpVal               = (entryValue >> 6) & 0x3;

    switch (dpVal)
    {
        case 0:
            qosParamPtr->dp = CPSS_DP_GREEN_E;
            break;
        case 1:
            qosParamPtr->dp = CPSS_DP_YELLOW_E;
            break;
        case 2:
            qosParamPtr->dp = CPSS_DP_RED_E;
            break;
        default:
            return GT_BAD_STATE;
    }

    return  GT_OK;
}

/*******************************************************************************
* cpssDxChPolicerStageMeterModeSet
*
* DESCRIPTION:
*       Sets Policer Global stage mode.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       mode            - Policer meter mode: FLOW or PORT.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or mode.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       In FLOW mode both the Policy and the port-trigger commands are
*       considered and in case of contention, the Policy command takes
*       precedence.
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerStageMeterModeSet
(
    IN  GT_U8                                       devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT             stage,
    IN  CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT      mode
)
{
    GT_U32  hwData;     /* hw data value */
    GT_U32  regAddr;    /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    switch (mode)
    {
        case CPSS_DXCH_POLICER_STAGE_METER_MODE_PORT_E:
            hwData = 0;
            break;
        case CPSS_DXCH_POLICER_STAGE_METER_MODE_FLOW_E:
            hwData = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControl2Reg;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 1, hwData);
}

/*******************************************************************************
* cpssDxChPolicerStageMeterModeGet
*
* DESCRIPTION:
*       Gets Policer Global stage mode.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       modePtr         - (pointer to) Policer meter mode: FLOW or PORT.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       In FLOW mode both the Policy and the port-trigger commands are
*       considered and in case of contention, the Policy command takes
*       precedence.
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerStageMeterModeGet
(
    IN  GT_U8                                       devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT             stage,
    OUT CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT      *modePtr
)
{
    GT_U32      hwData;     /* hw data value */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   retStatus;  /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControl2Reg;

    retStatus = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 1, &hwData);

    *modePtr = (hwData == 0) ? CPSS_DXCH_POLICER_STAGE_METER_MODE_PORT_E :
                               CPSS_DXCH_POLICER_STAGE_METER_MODE_FLOW_E;

    return retStatus;
}

/*******************************************************************************
* cpssDxChPolicerMeteringOnTrappedPktsEnableSet
*
* DESCRIPTION:
*       Enable/Disable Ingress metering for Trapped packets.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0 or Ingress #1.
*       enable          - GT_TRUE - policing is executed on all packets (data
*                                   and control).
*                         GT_FALSE - policing is executed only on packets that
*                                    were asigned Forward (mirrored to analyzer
*                                    is inclusive) or Mirror to CPU commands by
*                                    previous processing stages.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or mode.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       The Ingress Policer considers data traffic as any packet that is not
*       trapped or dropped.
*******************************************************************************/
GT_STATUS cpssDxChPolicerMeteringOnTrappedPktsEnableSet
(
    IN  GT_U8                                   devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN GT_BOOL                                  enable
)
{
    GT_U32      hwData;     /* hw data value */
    GT_U32      regAddr;    /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    /* Check that the stage is not Egress */
    if (stage > CPSS_DXCH_POLICER_STAGE_INGRESS_1_E)
    {
        return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControl2Reg;

    hwData = BOOL2BIT_MAC(enable);

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 1, 1, hwData);
}

/*******************************************************************************
* cpssDxChPolicerMeteringOnTrappedPktsEnableGet
*
* DESCRIPTION:
*       Get Ingress metering status (Enabled/Disabled) for Trapped packets.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0 or Ingress #1.
*
* OUTPUTS:
*       enablePtr       - GT_TRUE - policing is executed on all packets (data
*                                   and control).
*                         GT_FALSE - policing is executed only on packets that
*                                    were asigned Forward (mirrored to analyzer
*                                    is inclusive) or Mirror to CPU commands by
*                                    previous processing stages.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       The Ingress Policer considers data traffic as any packet that is not
*       trapped or dropped.
*******************************************************************************/
GT_STATUS cpssDxChPolicerMeteringOnTrappedPktsEnableGet
(
    IN  GT_U8                               devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    OUT GT_BOOL                             *enablePtr
)
{
    GT_U32      hwData;     /* hw data value */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   retStatus;  /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* Check that the stage is not Egress */
    if (stage > CPSS_DXCH_POLICER_STAGE_INGRESS_1_E)
    {
        return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControl2Reg;

    retStatus = prvCpssDrvHwPpGetRegField(devNum, regAddr, 1, 1, &hwData);

    *enablePtr = BIT2BOOL_MAC(hwData);

    return retStatus;
}
/*******************************************************************************
* prvCpssPortStormTypeOffsetAndRegisterAddressGet
*
* DESCRIPTION:
*       Get register address and field's offset for specific port
*       and storm type.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       localPort       - local port number (including the CPU port).
*       stormType       - storm type
*
* OUTPUTS:
*       fieldOffsetPtr  - (pointer to) field's offset
*       regAddrPtr      - (pointer to) register's address
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PARAM                - on wrong stormType.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvCpssPortStormTypeOffsetAndRegisterAddressGet
(
    IN  GT_U8                               devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    IN GT_U8                                localPort,
    IN CPSS_DXCH_POLICER_STORM_TYPE_ENT     stormType,
    OUT GT_U32                             *fieldOffsetPtr,
    OUT GT_U32                             *regAddrPtr
)
{

    /* check storm type and calculate bitoffset */
    switch (stormType)
    {
        case CPSS_DXCH_POLICER_STORM_TYPE_UC_KNOWN_E:
            *fieldOffsetPtr = 0;
            break;
        case CPSS_DXCH_POLICER_STORM_TYPE_UC_UNKNOWN_E:
            *fieldOffsetPtr = 2;
            break;
        case CPSS_DXCH_POLICER_STORM_TYPE_MC_REGISTERED_E:
            *fieldOffsetPtr = 4;
            break;
        case CPSS_DXCH_POLICER_STORM_TYPE_MC_UNREGISTERED_E:
            *fieldOffsetPtr = 6;
            break;
        case CPSS_DXCH_POLICER_STORM_TYPE_BC_E:
            *fieldOffsetPtr = 8;
            break;
        case CPSS_DXCH_POLICER_STORM_TYPE_TCP_SYN_E:
            *fieldOffsetPtr = 10;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* each register stores values for 2 ports.
       Even ports' values start from bit#0.
       Odd  ports' values start from bit#12. */
    *fieldOffsetPtr += (12 * (localPort & 1));
    *regAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                PLR[stage].portMeteringPtrIndexReg + ((localPort >> 1) * 0x4);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPolicerPortStormTypeIndexSet
*
* DESCRIPTION:
*       Associates policing profile with source/target port and storm rate type.
*       There are up to four policers per port and every storm type can be
*       flexibly associated with any of the four policers.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       portNum         - port number (including the CPU port).
*       stormType       - storm type
*       index           - policer index (0..3)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage, stormType ot port.
*       GT_OUT_OF_RANGE             - on out of range of index.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPolicerPortStormTypeIndexSet
(
    IN  GT_U8                               devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    IN GT_U8                                portNum,
    IN CPSS_DXCH_POLICER_STORM_TYPE_ENT     stormType,
    IN GT_U32                               index
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      bitOffset;  /* bit offset inside register */
    GT_U32      portGroupId;     /* the port group Id - support multi-port-groups device */
    GT_U8       localPort;  /* local port - support multi-port-groups device */
    GT_STATUS   rc;         /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    /* check policing profile index */
    if (index > 3)
    {
        return GT_OUT_OF_RANGE;
    }

    /* calculate register's address and position within register */
    rc = prvCpssPortStormTypeOffsetAndRegisterAddressGet(devNum, stage,
                                                         localPort, stormType,
                                                         &bitOffset, &regAddr);
    if (rc != GT_OK)
    {
        return rc;
    }

    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, bitOffset, 2, index);
}


/*******************************************************************************
* cpssDxChPolicerPortStormTypeIndexGet
*
* DESCRIPTION:
*       Get policing profile for given source/target port and storm rate type.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       portNum         - port number (including the CPU port).
*       stormType       - storm type
*
* OUTPUTS:
*       indexPtr        - pointer to policer index.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage, stormType or portNum.
*       GT_OUT_OF_RANGE             - on out of range of port.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPolicerPortStormTypeIndexGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_POLICER_STAGE_TYPE_ENT    stage,
    IN  GT_U8                               portNum,
    IN  CPSS_DXCH_POLICER_STORM_TYPE_ENT    stormType,
    OUT GT_U32                              *indexPtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      bitOffset;  /* bit offset inside register */
    GT_U32      portGroupId;     /*the port group Id - support multi-port-groups device */
    GT_U8       localPort;  /* local port - support multi-port-groups device */
    GT_STATUS   rc;         /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);
    CPSS_NULL_PTR_CHECK_MAC(indexPtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    /* calculate register's address and position within register */
    rc = prvCpssPortStormTypeOffsetAndRegisterAddressGet(devNum, stage,
                                                         localPort, stormType,
                                                         &bitOffset, &regAddr);
    if (rc != GT_OK)
    {
        return rc;
    }

    return prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, bitOffset, 2, indexPtr);
}


/*******************************************************************************
* cpssDxChPolicerEgressL2RemarkModelSet
*
* DESCRIPTION:
*       Sets Egress Policer L2 packets remarking model.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - physical device number.
*       model           - L2 packets remarking model. It defines QoS parameter,
*                         which used as index in the Qos table:
*                         User Priority or Traffic Class.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPolicerEgressL2RemarkModelSet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT    model
)
{
    GT_U32      hwData;     /* hw data value */
    GT_U32      regAddr;    /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    switch (model)
    {
        case CPSS_DXCH_POLICER_L2_REMARK_MODEL_TC_E:
            hwData = 0;
            break;
        case CPSS_DXCH_POLICER_L2_REMARK_MODEL_UP_E:
            hwData = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[2].policerControlReg;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 18, 1, hwData);
}

/*******************************************************************************
* cpssDxChPolicerEgressL2RemarkModelGet
*
* DESCRIPTION:
*       Gets Egress Policer L2 packets remarking model.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       modelPtr        - L2 packets remarking model. It defines QoS parameter,
*                         which used as index in the Qos table:
*                         User Priority or Traffic Class.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPolicerEgressL2RemarkModelGet
(
    IN  GT_U8                                    devNum,
    OUT CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT    *modelPtr
)
{
    GT_U32      hwData;     /* hw data value */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   retStatus;  /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(modelPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[2].policerControlReg;

    retStatus = prvCpssDrvHwPpGetRegField(devNum, regAddr, 18, 1, &hwData);

    switch (hwData)
    {
        case 0:
            *modelPtr = CPSS_DXCH_POLICER_L2_REMARK_MODEL_TC_E;
            break;
        case 1:
            *modelPtr = CPSS_DXCH_POLICER_L2_REMARK_MODEL_UP_E;
            break;
        default:
            return GT_BAD_PARAM;
    }

    return retStatus;
}

/*******************************************************************************
* cpssDxChPolicerEgressQosUpdateEnableSet
*
* DESCRIPTION:
*       The function enables or disables QoS remarking of conforming packets.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - physical device number.
*       enable  - Enable/disable Qos update for conforming packets:
*                 GT_TRUE  - Remark Qos parameters of conforming packets.
*                 GT_FALSE - Keep incoming Qos parameters of conforming packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerEgressQosUpdateEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32      hwData;     /* hw data value */
    GT_U32      regAddr;    /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[2].policerControlReg;

    hwData = BOOL2BIT_MAC(enable);

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 31, 1, hwData);
}

/*******************************************************************************
* cpssDxChPolicerEgressQosUpdateEnableGet
*
* DESCRIPTION:
*       The function get QoS remarking status of conforming packets
*       (enable/disable).
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - physical device number.
*
* OUTPUTS:
*       enablePtr  - Enable/disable Qos update for conforming packets:
*                   GT_TRUE  - Remark Qos parameters of conforming packets.
*                   GT_FALSE - Keep incoming Qos parameters of conforming
*                              packets.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerEgressQosUpdateEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32      hwData;     /* hw data value */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   retStatus;  /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[2].policerControlReg;

    retStatus = prvCpssDrvHwPpGetRegField(devNum, regAddr, 31, 1, &hwData);

    *enablePtr = BIT2BOOL_MAC(hwData);

    return retStatus;
}

/*******************************************************************************
* prvCpssDxChPolicerCountingEntryWordSet
*
* DESCRIPTION:
*      Sets particular word of the counting entry
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       entryIndex      - entry index.
*       entryWord       - entry's word number.
*       cntrValue       - counter's value.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum, entryIndex, entryWord
*                                  or stage.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPolicerCountingEntryWordSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  CPSS_DXCH_POLICER_STAGE_TYPE_ENT    stage,
    IN  GT_U32                              entryIndex,
    IN  GT_U32                              entryWord,
    IN  GT_U32                              cntrValue
)
{
    GT_U32  wordAddr;       /* counting entry's word address */
    GT_U32  wordData;       /* counting entry's word data */
    GT_U32  word;           /* counting entry's word */
    GT_U32  portGroupId;    /* port group id */
    GT_U32  regAddr;        /* register address */
    GT_STATUS rc;           /* return status */

    /* check counting entry index and word number */
    PRV_CPSS_DXCH_POLICERS_NUM_CHECK_MAC(devNum, stage, entryIndex);
    if (entryWord >= PRV_CPSS_DXCH_POLICER_COUNT_ENTRY_SIZE_CNS)
    {
        return GT_BAD_PARAM;
    }

    /* Check if Policer Counting memory errata is enabled */

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
             PRV_CPSS_DXCH_LION_POLICER_COUNTING_MEM_ACCESS_WA_E))
    {
        /* Indirect access to counting memory should be used */

        /* Get Ingress Policer Table Access Data 0 Register Address */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        PLR[stage].policerTblAccessDataReg;

        /* First read the entry on each portGroup from the bitmap */
        /* loop on all active port groups in the bmp */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
        {
            /* Perform indirect access to the Policer Table */
            rc = prvCpssDxCh3PolicerInternalTableAccess(
                                    devNum,
                                    portGroupId,
                                    stage,
                                    entryIndex,
                                    PRV_CPSS_DXCH3_POLICER_CNTR_READ_ONLY_E);
            if (GT_OK != rc)
            {
                return rc;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

        /* Update the desired word in access data registers */
        wordAddr = (regAddr + (entryWord * 4));

        /* set new value of counter */
        rc = prvCpssDxChPortGroupsBmpCounterSet(devNum, portGroupsBmp,
                                                wordAddr, 0,32, cntrValue);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* In the last word there is a bit that is responsible for
           Billing/IPFIX, it should be set to Billing and not IPFIX
           (the bit should be cleared).
           VLAN and Policy counters do not use the last word for counting */
        wordAddr = regAddr + 0x1C;

        /* loop on all active port groups in the bmp */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
        {
            rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,
                                                      portGroupId,
                                                      wordAddr,
                                                      0x0);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* Trigger the read & write action */
            rc = prvCpssDxCh3PolicerInternalTableAccess(
                                devNum,
                                portGroupId,
                                stage,
                                entryIndex,
                                PRV_CPSS_DXCH3_POLICER_CNTR_READ_AND_WRITE_E);
            if (rc != GT_OK)
            {
                return rc;
            }

        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    }


    else
    {
        /* Direct access to counting memory */

        wordAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        PLR[stage].policerCountingTbl + (entryIndex * 0x20);

        /* The device may write only whole counting memory entry.
           The device triggers write by the last word of the counting entry.
           Copy all words of entry and set given word. */
        for (word = 0; word < PRV_CPSS_DXCH_POLICER_COUNT_ENTRY_SIZE_CNS; word++)
        {
            /* verify that all words were written before write last one.
               because last one triggers write of whole entry */
            if (word == (PRV_CPSS_DXCH_POLICER_COUNT_ENTRY_SIZE_CNS - 1) )
            {
                GT_SYNC;
            }

            if (word == entryWord)
            {
                /* set new value of counter */
                rc = prvCpssDxChPortGroupsBmpCounterSet(devNum, portGroupsBmp,
                                                        wordAddr, 0,32, cntrValue);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
            else
            {
                /* Check if it the last word, if yes, clear it.
                   In the last word there is a bit that is responsible for
                   Billing/IPFIX, it should be set to Billing and not IPFIX
                   (the bit should be cleared).
                   VLAN and Policy counters do not use the last word for counting */
                if (word == (PRV_CPSS_DXCH_POLICER_COUNT_ENTRY_SIZE_CNS - 1))
                {

                    /* loop on all active port groups in the bmp */
                    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
                    {
                        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,
                                                                  portGroupId,
                                                                  wordAddr,
                                                                  0x0);
                        if (rc != GT_OK)
                        {
                            return rc;
                        }
                    }
                    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
                }
                else
                {
                    /* loop on all active port groups in the bmp */
                    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
                    {

                        /* just read word and write it back */
                        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum,
                                                                 portGroupId,
                                                                 wordAddr,
                                                                 &wordData);
                        if (rc != GT_OK)
                        {
                            return rc;
                        }

                        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,
                                                                  portGroupId,
                                                                  wordAddr,
                                                                  wordData);
                        if (rc != GT_OK)
                        {
                            return rc;
                        }
                    }
                    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
                }
            }

            /* increment word's address */
            wordAddr += 4;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPolicerVlanCntrSet
*
* DESCRIPTION:
*      Sets Policer VLAN Counters located in the Policer Counting Entry upon
*      enabled VLAN counting mode.
*      When working in VLAN Counting Mode each counter counts the number of
*      packets or bytes according to the VLAN counters mode set
*      by cpssDxChPolicerVlanCountingModeSet(). Index of counter is VLAN ID.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       vid             - VLAN ID. Bits [1:0] of VID are used to select the
*                         relevant 32 bit counter inside the Counting Entry.
*       cntrValue       - Packet/Byte counter indexed by the packet's VID.
*                         For counter reset this parameter should be nullified.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on disabled VLAN Counting.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or vid (VID range is limited by max
*                         number of Policer Counting Entries) or stage.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The VLAN counters are free-running no-sticky counters.
*
*       In order to set Vlan counting entry under traffic perform the following
*       algorithm:
*           - disconnect entryes from traffic
*           - call cpssDxChPolicerCountingWriteBackCacheFlush
*           - set new values
*           - connect entries back to traffic
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerVlanCntrSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_POLICER_STAGE_TYPE_ENT    stage,
    IN  GT_U16                              vid,
    IN  GT_U32                              cntrValue
)
{
    GT_U32  countNumber;    /* the number of counter in the Entry */
    GT_U32  entryIndex;     /* entry index in the Internal Table */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    /* each counting entry has 4 VLAN counters */
    entryIndex = (vid >> 2);
    countNumber = (vid & 0x3);

    /* Set counters value */
    return prvCpssDxChPolicerCountingEntryWordSet(
                                            devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            stage, entryIndex, countNumber,
                                            cntrValue);
}

/*******************************************************************************
* cpssDxChPolicerVlanCntrGet
*
* DESCRIPTION:
*      Gets Policer VLAN Counters located in the Policer Counting Entry upon
*      enabled VLAN counting mode.
*      When working in VLAN Counting Mode each counter counts the number of
*      packets or bytes according to the VLAN counters mode set
*      by cpssDxChPolicerVlanCountingModeSet(). Index of counter is VLAN ID.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       vid             - VLAN ID. Bits [1:0] of VID are used to select the
*                         relevant 32 bit counter inside the Counting Entry.
*
* OUTPUTS:
*       cntrValuePtr    - Packet/Byte counter indexed by the packet's VID.
*                         For counter reset this parameter should be nullified.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on disabled VLAN Counting.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or vid (VID range is limited by max
*                         number of Policer Counting Entries) or stage.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The VLAN counters are free-running no-sticky counters.
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerVlanCntrGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_POLICER_STAGE_TYPE_ENT    stage,
    IN  GT_U16                              vid,
    OUT GT_U32                              *cntrValuePtr
)
{
    GT_U32  regAddr;        /* register address                     */
    GT_U32  countNumber;    /* the number of counter in the Entry   */
    GT_U32  entryIndex;     /* entry index in the Internal Table    */
    GT_STATUS rc;           /* return code                          */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);
    CPSS_NULL_PTR_CHECK_MAC(cntrValuePtr);

    entryIndex = (vid >> 2);

    /* check allocated number of counters - 4 VLAN counters per Counting Vlan Entry */
    PRV_CPSS_DXCH_POLICERS_NUM_CHECK_MAC(devNum, stage, entryIndex);


    /* Check if Policer Counting memory errata is enabled */
    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                          PRV_CPSS_DXCH_LION_POLICER_COUNTING_MEM_ACCESS_WA_E))
    {
        /* Trigger Indirect Read from Counting memory */
        rc = prvCpssDxCh3PolicerInternalTableAccess(devNum,
                              CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                              stage,
                              entryIndex,
                              PRV_CPSS_DXCH3_POLICER_CNTR_READ_ONLY_E);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* Get Ingress Policer Table Access Data 0 Register Address */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        PLR[stage].policerTblAccessDataReg;
    }
    else
    {
        /* Direct access to counting memory */

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        PLR[stage].policerCountingTbl + (entryIndex * 0x20);
    }

    countNumber = (vid & 0x3);

    /* calculate the word address inside the table */
    regAddr += (countNumber * 0x4);


    /* Get counters value */
    return prvCpssDxChPortGroupsCounterSummary(devNum, regAddr,0,32, cntrValuePtr,NULL);
}

/*******************************************************************************
* cpssDxChPolicerVlanCountingPacketCmdTriggerSet
*
* DESCRIPTION:
*       Enables or disables VLAN counting triggering according to the
*       specified packet command.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0 or Ingress #1.
*       cmdTrigger      - The packet command trigger.
*                         The following packet command are supported:
*                           Forward (CPSS_PACKET_CMD_FORWARD_E),
*                           Hard Drop (CPSS_PACKET_CMD_DROP_HARD_E),
*                           Soft Drop (CPSS_PACKET_CMD_DROP_SOFT_E),
*                           Trap (CPSS_PACKET_CMD_TRAP_TO_CPU_E),
*                           Mirror to CPU (CPSS_PACKET_CMD_MIRROR_TO_CPU_E).
*       enable          - Enable/Disable VLAN Counting according to the packet
*                         command trigger:
*                         GT_TRUE  - Enable VLAN counting on packet with
*                                    packet command trigger.
*                         GT_FALSE - Disable VLAN counting on packet with
*                                    packet command trigger.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on not supported egress direction
*
* COMMENTS:
*       All packet commands are enabled in the HW by default.
*       For egress policer the default trigger values shouldn't be changed.
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerVlanCountingPacketCmdTriggerSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    IN CPSS_PACKET_CMD_ENT                  cmdTrigger,
    IN GT_BOOL                              enable
)
{
    GT_U32      hwData;     /* hw data value */
    GT_U32      regAddr;    /* register address */
    GT_U32      bitOffset;  /* bit offset inside register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    /* Check that the stage is Ingress */
    if (!PRV_CPSS_DXCH_POLICER_INGRESS_STAGE_CHECK_MAC(stage))
    {
        return GT_BAD_PARAM;
    }

    /* check vlan command trigger and calculate bit offset */
    switch (cmdTrigger)
    {
        case CPSS_PACKET_CMD_FORWARD_E:
            bitOffset = 25;
            break;
        case CPSS_PACKET_CMD_DROP_HARD_E:
        case CPSS_PACKET_CMD_DROP_SOFT_E:
            bitOffset = 26;
            break;
        case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            bitOffset = 27;
            break;
        case CPSS_PACKET_CMD_MIRROR_TO_CPU_E:
            bitOffset = 28;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControlReg;

    hwData = BOOL2BIT_MAC(enable);

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, bitOffset, 1, hwData);
}

/*******************************************************************************
* cpssDxChPolicerVlanCountingPacketCmdTriggerGet
*
* DESCRIPTION:
*       Gets VLAN counting triggering status (Enable/Disable) according to the
*       specified packet command.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0 or Ingress #1.
*       cmdTrigger      - The packet command trigger.
*                         The following packetcommand are supported:
*                           Forward (CPSS_PACKET_CMD_FORWARD_E),
*                           Hard Drop (CPSS_PACKET_CMD_DROP_HARD_E),
*                           Soft Drop (CPSS_PACKET_CMD_DROP_SOFT_E),
*                           Trap (CPSS_PACKET_CMD_TRAP_TO_CPU_E),
*                           Mirror to CPU (CPSS_PACKET_CMD_MIRROR_TO_CPU_E).
*
* OUTPUTS:
*       enablePtr       - Pointer to Enable/Disable VLAN Counting according
*                         to the packet command trigger:
*                         GT_TRUE  - Enable VLAN counting on packet with
*                                    triggered packet command.
*                         GT_FALSE - Disable VLAN counting on packet with
*                                    triggered packet command.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       All packet commands are enabled in the HW by default.
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerVlanCountingPacketCmdTriggerGet
(
    IN  GT_U8                               devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    IN  CPSS_PACKET_CMD_ENT                 cmdTrigger,
    OUT GT_BOOL                             *enablePtr
)
{
    GT_U32      hwData;     /* hw data value                */
    GT_U32      regAddr;    /* register address             */
    GT_U32      bitOffset;  /* bit offset inside register   */
    GT_STATUS   retStatus;  /* return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* Check that the stage is Ingress */
    if (!PRV_CPSS_DXCH_POLICER_INGRESS_STAGE_CHECK_MAC(stage))
    {
        return GT_BAD_PARAM;
    }

    /* check vlan command trigger and calculate bit offset */
    switch (cmdTrigger)
    {
        case CPSS_PACKET_CMD_FORWARD_E:
            bitOffset = 25;
            break;
        case CPSS_PACKET_CMD_DROP_HARD_E:
        case CPSS_PACKET_CMD_DROP_SOFT_E:
            bitOffset = 26;
            break;
        case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            bitOffset = 27;
            break;
        case CPSS_PACKET_CMD_MIRROR_TO_CPU_E:
            bitOffset = 28;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControlReg;

    retStatus = prvCpssDrvHwPpGetRegField(devNum, regAddr, bitOffset, 1, &hwData);


    *enablePtr = BIT2BOOL_MAC(hwData);

    return retStatus;
}

/*******************************************************************************
* cpssDxChPolicerVlanCountingModeSet
*
* DESCRIPTION:
*       Sets the Policer VLAN counters mode as byte or packet based.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       mode            - VLAN counters mode: Byte or Packet based.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPolicerVlanCountingModeSet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT     mode
)
{
    GT_U32      hwData;     /* hw data value                */
    GT_U32      regAddr;    /* register address             */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    /* Convert VLAN counters mode to the Reg Value */
    switch(mode)
    {
        case CPSS_DXCH_POLICER_VLAN_CNTR_MODE_BYTES_E:
            hwData = 1;
            break;
        case CPSS_DXCH_POLICER_VLAN_CNTR_MODE_PACKETS_E:
            hwData = 0;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControlReg;

    /* Set Policer Control0 register, <VlanCountingMode> field */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 3, 1, hwData);
}

/*******************************************************************************
* cpssDxChPolicerVlanCountingModeGet
*
* DESCRIPTION:
*       Gets the Policer VLAN counters mode as byte or packet based.
*
* APPLICABLE DEVICES:
*        xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       modePtr         - (pointer to) VLAN counters mode: Byte or Packet based.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPolicerVlanCountingModeGet
(
    IN  GT_U8                                    devNum,
    IN  CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    OUT CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT     *modePtr
)
{
    GT_U32      hwData;     /* hw data value                */
    GT_U32      regAddr;    /* register address             */
    GT_STATUS   retStatus;  /* return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControlReg;

    retStatus = prvCpssDrvHwPpGetRegField(devNum, regAddr, 3, 1, &hwData);

    *modePtr = (hwData == 0) ? CPSS_DXCH_POLICER_VLAN_CNTR_MODE_PACKETS_E:
                                CPSS_DXCH_POLICER_VLAN_CNTR_MODE_BYTES_E;

    return retStatus;
}

/*******************************************************************************
* cpssDxChPolicerPolicyCntrSet
*
* DESCRIPTION:
*      Sets Policer Policy Counters located in the Policer Counting Entry upon
*      enabled Policy Counting mode.
*      When working in Policy Counting Mode each counter counts the number of
*      packets matching on a Policy rules.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       index           - index set by Policy Action Entry or Metering entry
*                         and belongs to the range from 0 up to maximal number
*                         of Policer Policy counters.
*                         Maximal number is number of policer entries for
*                         counting is number of counting entries
*                         multiplied by 8.
*                         Some indexes in the range are not valid. Not valid
*                         indexes are:
*                           - (index % 8) == 6
*                           - (index % 8) == 7
*                         e.g. not valid indexes 6, 7, 14, 15, 22, 23 ...
*       cntrValue       - packets counter.
*                         For counter reset this parameter should be nullified.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on disabled Policy Counting.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or index (index range is limited by
*                         max number of Policer Policy counters).
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Policy counters are free-running no-sticky counters.
*
*       In order to set Policy counting entry under traffic perform the following
*       algorithm:
*           - disconnect entryes from traffic
*           - call cpssDxChPolicerCountingWriteBackCacheFlush
*           - set new values
*           - connect entries back to traffic
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerPolicyCntrSet
(
    IN  GT_U8                               devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    IN  GT_U32                              index,
    IN  GT_U32                              cntrValue
)
{
    return cpssDxChPolicerPortGroupPolicyCntrSet(
                                            devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            stage,
                                            index,
                                            cntrValue);
}

/*******************************************************************************
* cpssDxChPolicerPolicyCntrGet
*
* DESCRIPTION:
*      Gets Policer Policy Counters located in the Policer Counting Entry upon
*      enabled Policy Counting mode.
*      When working in Policy Counting Mode each counter counts the number of
*      packets matching on a a Policy rules.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       index           - index set by Policy Action Entry or Metering entry
*                         and belongs to the range from 0 up to maximal number
*                         of Policer Policy counters.
*                         Maximal number is number of policer entries for
*                         counting is number of counting entries
*                         multiplied by 8.
*                         Some indexes in the range are not valid. Not valid
*                         indexes are:
*                           - (index % 8) == 6
*                           - (index % 8) == 7
*                         e.g. not valid indexes 6, 7, 14, 15, 22, 23 ...
*
* OUTPUTS:
*       cntrValuePtr    - Pointer to the packet counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on disabled Policy Counting.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or index.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_TIMEOUT               - on time out.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Policy counters are free-running no-sticky counters.
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerPolicyCntrGet
(
    IN  GT_U8                               devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    IN  GT_U32                              index,
    OUT GT_U32                              *cntrValuePtr
)
{
    return cpssDxChPolicerPortGroupPolicyCntrGet(
                                        devNum,
                                        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                        stage,
                                        index,
                                        cntrValuePtr);
}

/*******************************************************************************
* cpssDxChPolicerMemorySizeModeSet
*
* DESCRIPTION:
*      Sets internal table sizes and the way they are shared between the Ingress
*      policers.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number.
*       mode            - The mode in which internal tables are shared.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Mode change should not be done when meter or counter entries are in use.
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerMemorySizeModeSet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT      mode
)
{
    GT_U32      hwData;         /* hw data value                     */
    GT_U32      hwData1;        /* hw data value for xCat2 and above */
    GT_U32      regAddr;        /* register address                  */
    GT_U32      offset;         /* field offset in the register      */
    GT_U32      numOfBits;      /* size of field in bits             */
    GT_U32      iplr0MemSize;   /* IPLR 0 memory size                */
    GT_U32      iplr1MemSize;   /* IPLR 1 memory size                */
    GT_STATUS   retStatus;      /* return code                       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);


    switch (mode)
    {
        case CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_PLR0_UPPER_PLR1_LOWER_E:
            iplr1MemSize = 256;
            iplr0MemSize = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policersNum -
                iplr1MemSize;
            hwData = 0;
            hwData1 = 5;
            break;
        case CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_PLR0_UPPER_AND_LOWER_E:
            iplr1MemSize = 0;
            iplr0MemSize = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policersNum;
            hwData = 1;
            hwData1 = 0;
            break;
        case CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_PLR1_UPPER_AND_LOWER_E:
            /* Check that the second stage is supported, if not, then memory
               size of the single stage will become 0
               Revision A0 does not supports second stage. */
            if (PRV_CPSS_DXCH_PP_MAC(devNum)->
                fineTuning.featureInfo.iplrSecondStageSupported == GT_FALSE)
            {
                return GT_BAD_PARAM;
            }
            iplr0MemSize = 0;
            iplr1MemSize = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policersNum;
            hwData = 2;
            hwData1 = 4;
            break;
        case CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_PLR1_UPPER_PLR0_LOWER_E:
            iplr0MemSize = 256;
            iplr1MemSize = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policersNum -
                iplr0MemSize;
            hwData = 3;
            hwData1 = 1;
            break;
        case CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_4_E:
            if (PRV_CPSS_DXCH_PP_MAC(devNum)->
                fineTuning.featureInfo.iplrSecondStageSupported == GT_FALSE)
            {
                return GT_BAD_PARAM;
            }
            if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E) 
            {
                iplr0MemSize = 172;
                iplr1MemSize = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policersNum -
                    iplr0MemSize;
                hwData = 0;
                hwData1 = 2;
                break;
            }
            else
            {
                return GT_BAD_PARAM;
            }
        case CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_5_E:
            if (PRV_CPSS_DXCH_PP_MAC(devNum)->
                fineTuning.featureInfo.iplrSecondStageSupported == GT_FALSE)
            {
                return GT_BAD_PARAM;
            }
            if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E) 
            {
                iplr1MemSize = 172;
                iplr0MemSize = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policersNum -
                    iplr1MemSize;
                hwData = 0;
                hwData1 = 6;
                break;
            }
            else
            {
                return GT_BAD_PARAM;
            }
        default:
            return GT_BAD_PARAM;
    }

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        /* xCat2 and above use Hierarchical Policer control register 
          for memory control. The register is policerControl2Reg in the DB */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[0].policerControl2Reg;
        offset = 5;
        numOfBits = 3;
        hwData = hwData1;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[0].policerControlReg;
        offset = 11;
        numOfBits = 2;
    }

    retStatus = prvCpssDrvHwPpSetRegField(devNum, regAddr, offset, numOfBits, hwData);
    if (retStatus != GT_OK)
    {
        return retStatus;
    }

    /* update memory size in the policer db */
    PRV_CPSS_DXCH_PP_MAC(devNum)->policer.memSize[0] = iplr0MemSize;
    PRV_CPSS_DXCH_PP_MAC(devNum)->policer.memSize[1] = iplr1MemSize;

    return retStatus;
}

/*******************************************************************************
* cpssDxChPolicerMemorySizeModeGet
*
* DESCRIPTION:
*      Gets internal table sizes and the way they are shared between the Ingress
*      policers.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number.
*
* OUTPUTS:
*       modePtr         - The mode in which internal tables are shared.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerMemorySizeModeGet
(
    IN  GT_U8                                      devNum,
    OUT CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT     *modePtr
)
{
    GT_U32      hwData;     /* hw data value */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   retStatus;  /* return code */
    GT_U32      offset;     /* field offset in the register      */
    GT_U32      numOfBits;  /* size of field in bits             */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        /* xCat2 and above use Hierarchical Policer control register 
          for memory control. The register is policerControl2Reg in the DB */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[0].policerControl2Reg;
        offset = 5;
        numOfBits = 3;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[0].policerControlReg;
        offset = 11;
        numOfBits = 2;
    }

    retStatus = prvCpssDrvHwPpGetRegField(devNum, regAddr, offset, numOfBits, &hwData);
    if (retStatus != GT_OK)
    {
        return retStatus;
    }

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        switch (hwData)
        {            
            case 0:
                *modePtr = CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_PLR0_UPPER_AND_LOWER_E;
                break;
            case 2:
                *modePtr = CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_4_E;
                break;
            case 1:
                *modePtr = CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_PLR1_UPPER_PLR0_LOWER_E;
                break;
            case 4:
                *modePtr = CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_PLR1_UPPER_AND_LOWER_E;
                break;
            case 5:
                *modePtr = CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_PLR0_UPPER_PLR1_LOWER_E;
                break;
            case 6:
                *modePtr = CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_5_E;
                break;
            default:
                return GT_BAD_STATE;
        }
    }
    else
    {    
        switch (hwData)
        {
            case 0:
                *modePtr = CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_PLR0_UPPER_PLR1_LOWER_E;
                break;
            case 1:
                *modePtr = CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_PLR0_UPPER_AND_LOWER_E;
                break;
            case 2:
                *modePtr = CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_PLR1_UPPER_AND_LOWER_E;
                break;
            case 3:
                *modePtr = CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_PLR1_UPPER_PLR0_LOWER_E;
                break;
            default:
                return GT_BAD_STATE;
        }
    }
    return retStatus;
}

/*******************************************************************************
* cpssDxChPolicerCountingWriteBackCacheFlush
*
* DESCRIPTION:
*       Flush internal Write Back Cache (WBC) of counting entries.
*       The Policer implements internal Write Back Cache for frequent and rapid
*       update of counting entries.
*       Since the WBC holds the policer entries' data, the CPU must clear its
*       content when performing direct write access of the counting entries by
*       one of following functions:
*        - cpssDxCh3PolicerBillingEntrySet
*        - cpssDxChPolicerVlanCntrSet
*        - cpssDxChPolicerPolicyCntrSet
*       The policer counting should be disabled before flush of WBC if direct
*       write access is performed under traffic. And policer counting should be
*       enabled again after finish of write access.
*       The cpssDxChPolicerCountingModeSet may be used to disable/enable
*       the policer counting.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or stage.
*       GT_HW_ERROR              - on Hardware error.
*       GT_TIMEOUT               - on time out.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To use the listed above APIs (cpssDxCh3PolicerBillingEntrySet,
*       cpssDxChPolicerVlanCntrSet and cpssDxChPolicerPolicyCntrSet) under
*       traffic, perform the following algorithm:
*           - disconnect entryes from traffic
*           - call cpssDxChPolicerCountingWriteBackCacheFlush
*           - set new values
*           - connect entries back to traffic

*******************************************************************************/
GT_STATUS cpssDxChPolicerCountingWriteBackCacheFlush
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    return prvCpssDxCh3PolicerInternalTableAccess(devNum,
                                                  CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                  stage,
                                                  0 /* not used for flush */,
                                                  PRV_CPSS_DXCH3_POLICER_CNTR_WBF_FLUSH_E);
}

/*******************************************************************************
* cpssDxChPolicerMeteringCalcMethodSet
*
* DESCRIPTION:
*      Sets Metering Policer parameters calculation orientation method.
*      Due to the fact that selecting algorithm has effect on how HW metering
*      parameters are set, this API influence on
*      cpssDxCh3PolicerMeteringEntrySet and
*      cpssDxCh3PolicerEntryMeterParamsCalculate APIs.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum                  - device number.
*       meteringCalcMethod      - selecting calculation of HW metering
*                                 parameters between CIR\PIR or CBS\PBS
*                                 orientation.
*       cirPirAllowedDeviation  - the allowed deviation in percentage from the
*                                 requested CIR\PIR. Relevant only for
*                         CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_AND_CBS_E.
*       cbsPbsCalcOnFail        - GT_TRUE: If CBS\PBS constraints cannot be
*                                 matched return to CIR\PIR oriented
*                                 calculation.
*                                 GT_FALSE: If CBS\PBS constraints cannot be
*                                 matched return error. Relevant only for
*                         CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_AND_CBS_E.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PARAM                - wrong devNum or method.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_ONLY_E - The default
*       algorithm in which metering parameters are configured in such a way
*       that the selected CIR\PIR is the one which is the nearest possible
*       (under the granularity constraint) to the requested CIR\PIR.
*       CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_AND_CBS_E - In this
*       algorithm a deviation from requested CIR\PIR is defined. At first stage
*       the algorithm tries to find parameters that will complay with both the
*       CBS\PBS requested and the range allowed for the CIR\PIR. If no matching
*       solution is achieved there is an option to perform another stage. At
*       the second (optional) stage CIR\PIR is selected in the allowed range
*       and under this selection CBS\PBS is configured to be as near as
*       possible to the requested one. If second stage is needed and not
*       selected failure (error) indication is reported.
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerMeteringCalcMethodSet
(
    IN GT_U8                                        devNum,
    IN CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT   meteringCalcMethod,
    IN GT_U32                                       cirPirAllowedDeviation,
    IN GT_BOOL                                      cbsPbsCalcOnFail
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    switch(meteringCalcMethod)
    {
        case CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_ONLY_E:
            PRV_CPSS_DXCH_PP_MAC(devNum)->policer.meteringCalcMethod =
                CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_ONLY_E;
            break;
        case CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_AND_CBS_E:
            PRV_CPSS_DXCH_PP_MAC(devNum)->policer.meteringCalcMethod =
                CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_AND_CBS_E;
            PRV_CPSS_DXCH_PP_MAC(devNum)->policer.cirPirAllowedDeviation =
                    cirPirAllowedDeviation;
            PRV_CPSS_DXCH_PP_MAC(devNum)->policer.cbsPbsCalcOnFail =
                cbsPbsCalcOnFail;
            break;
        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPolicerMeteringCalcMethodGet
*
* DESCRIPTION:
*      Gets Metering Policer parameters calculation orientation method.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum              - device number.
*
* OUTPUTS:
*       meteringCalcMethodPtr       - (pointer to) selected calculation of HW
*                                     metering parameters between CIR\PIR or
*                                     CBS\PBS orientation.
*       cirPirAllowedDeviationPtr   - (pointer to) the allowed deviation in
*                                     percentage from the requested CIR\PIR.
*                                     Relevant only for
*                         CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_AND_CBS_E.
*       cbsPbsCalcOnFailPtr         - (pointer to)
*                                     GT_TRUE: If CBS\PBS constraints cannot be
*                                     matched return to CIR\PIR oriented
*                                     calculation.
*                                     GT_FALSE: If CBS\PBS constraints cannot
*                                     be matched return error.
*                                     Relevant only for
*                         CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_AND_CBS_E.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_BAD_PARAM                - wrong devNum or method.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_ONLY_E - The default
*       algorithm in which metering parameters are configured in such a way
*       that the selected CIR\PIR is the one which is the nearest possible
*       (under the granularity constraint) to the requested CIR\PIR.
*       CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_AND_CBS_E - In this
*       algorithm a deviation from requested CIR\PIR is defined. At first stage
*       the algorithm tries to find parameters that will complay with both the
*       CBS\PBS requested and the range allowed for the CIR\PIR. If no matching
*       solution is achieved there is an option to perform another stage. At
*       the second (optional) stage CIR\PIR is selected in the allowed range
*       and under this selection CBS\PBS is configured to be as near as
*       possible to the requested one. If second stage is needed and not
*       selected failure (error) indication is reported.
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerMeteringCalcMethodGet
(
    IN GT_U8                                        devNum,
    OUT CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT  *meteringCalcMethodPtr,
    OUT GT_U32                                      *cirPirAllowedDeviationPtr,
    OUT GT_BOOL                                     *cbsPbsCalcOnFailPtr
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(meteringCalcMethodPtr);

    switch(PRV_CPSS_DXCH_PP_MAC(devNum)->policer.meteringCalcMethod)
    {
        case CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_ONLY_E:
            *meteringCalcMethodPtr = CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_ONLY_E;
            break;
        case CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_AND_CBS_E:
            CPSS_NULL_PTR_CHECK_MAC(cirPirAllowedDeviationPtr);
            CPSS_NULL_PTR_CHECK_MAC(cbsPbsCalcOnFailPtr);
            *meteringCalcMethodPtr = CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_AND_CBS_E;
            *cirPirAllowedDeviationPtr =
                PRV_CPSS_DXCH_PP_MAC(devNum)->policer.cirPirAllowedDeviation;
            *cbsPbsCalcOnFailPtr =
                PRV_CPSS_DXCH_PP_MAC(devNum)->policer.cbsPbsCalcOnFail;
            break;
        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}



/*******************************************************************************
* cpssDxChPolicerCountingTriggerByPortEnableSet
*
* DESCRIPTION:
*       Enable/Disable Counting when port based metering is triggered for
*       the packet.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum      - device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       enable      - GT_TRUE - If counting mode is Billing, counting is performed
*                               for every packet subject to port based metering.
*                               If counting mode is not Billing, counting is
*                               performed for these packets only if the Policy
*                               engine enabled for Counting.
*                     GT_FALSE - Counting is performed for these packets only
*                                if the Policy engine enabled for Counting.
.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerCountingTriggerByPortEnableSet
(
    IN  GT_U8                                   devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN GT_BOOL                                  enable
)
{
    GT_U32      hwData;         /* hw data value        */
    GT_U32      regAddr;        /* register address     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControl2Reg;

    hwData = BOOL2BIT_MAC(enable);

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 3, 1, hwData);
}

/*******************************************************************************
* cpssDxChPolicerCountingTriggerByPortEnableGet
*
* DESCRIPTION:
*       Get Counting trigger by port status.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       enablePtr   - GT_TRUE - If counting mode is Billing, counting is performed
*                               for every packet subject to port based metering.
*                               If counting mode is not Billing, counting is
*                               performed for these packets only if the Policy
*                               engine enabled for Counting.
*                     GT_FALSE - Counting is performed for these packets only
*                                if the Policy engine enabled for Counting.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_BAD_PTR                  - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerCountingTriggerByPortEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_POLICER_STAGE_TYPE_ENT        stage,
    OUT GT_BOOL                                 *enablePtr
)
{
    GT_U32      hwData;         /* hw data value        */
    GT_U32      regAddr;        /* register address     */
    GT_STATUS   retStatus;      /* return code          */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControl2Reg;

    retStatus = prvCpssDrvHwPpGetRegField(devNum, regAddr, 3, 1, &hwData);

    if (GT_OK == retStatus)
    {
        *enablePtr = BIT2BOOL_MAC(hwData);
    }

    return retStatus;
}


/*******************************************************************************
* cpssDxChPolicerPortModeAddressSelectSet
*
* DESCRIPTION:
*       Configure Metering Address calculation type.
*       Relevant when stage mode is CPSS_DXCH_POLICER_STAGE_METER_MODE_PORT_E. 
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*       devNum      - device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       type        - Address select type: Full or Compressed.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or type.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerPortModeAddressSelectSet
(
    IN  GT_U8                                               devNum,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT                     stage,
    IN CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT     type
)
{
    GT_U32      hwData;         /* hw data value        */
    GT_U32      regAddr;        /* register address     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControl2Reg;

    switch (type) 
    {
        case CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_FULL_E:
            hwData = 0;
            break;
        case CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_COMPRESSED_E:
            hwData = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 4, 1, hwData);
}


/*******************************************************************************
* cpssDxChPolicerPortModeAddressSelectGet
*
* DESCRIPTION:
*       Get Metering Address calculation type.
*       Relevant when stage mode is CPSS_DXCH_POLICER_STAGE_METER_MODE_PORT_E. 
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*       devNum      - device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       typePtr     - (pointer to) Address select type: Full or Compressed.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_BAD_PTR                  - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerPortModeAddressSelectGet
(
    IN  GT_U8                                               devNum,
    IN  CPSS_DXCH_POLICER_STAGE_TYPE_ENT                    stage,
    OUT CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT    *typePtr
)
{

    GT_U32      hwData;         /* hw data value        */
    GT_U32      regAddr;        /* register address     */
    GT_STATUS   retStatus;      /* return code          */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
                                          CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E);

    CPSS_NULL_PTR_CHECK_MAC(typePtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerControl2Reg;

    retStatus = prvCpssDrvHwPpGetRegField(devNum, regAddr, 4, 1, &hwData);

    if (GT_OK == retStatus)
    {
        *typePtr = (hwData) ? CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_COMPRESSED_E :
            CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_FULL_E;
    }

    return retStatus;
}

/******************************************************************************/
/******************************************************************************/
/******* start of functions with portGroupsBmp parameter **********************/
/******************************************************************************/
/******************************************************************************/


/*******************************************************************************
* cpssDxChPolicerPortGroupMeteringEntryRefresh
*
* DESCRIPTION:
*       Refresh the Policer Metering Entry.
*       Meter Bucket's state parameters refreshing are needed in order to
*       prevent mis-behavior due to wrap around of timers.
*
*       The wrap around problem can occur when there are long periods of
*       'silence' on the flow and the bucket's state parameters do not get
*       refreshed (meter is in the idle state). This causes a problem once the
*       flow traffic is re-started since the timers might have already wrapped
*       around which can cause a situation in which the bucket's state is
*       mis-interpreted and the incoming packet is marked as non-conforming
*       even though the bucket was actually supposed to be full.
*       To prevent this from happening the CPU needs to trigger a meter
*       refresh transaction at least once every 10 minutes per meter.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above
*                         devices and ignored by DxCh3.
*       entryIndex      - index of Policer Metering Entry.
*                         Range: see datasheet for specific device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_TIMEOUT                  - on time out of IPLR Table indirect access.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage, entryIndex or
*                                     portGroupsBmp
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       Once the CPU triggers Refresh transaction the policer accesses the
*       metering entry and performs metering refresh with the packet's Byte
*       Count set to zero.
*******************************************************************************/
GT_STATUS cpssDxChPolicerPortGroupMeteringEntryRefresh
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  CPSS_DXCH_POLICER_STAGE_TYPE_ENT    stage,
    IN  GT_U32                              entryIndex
)
{
    GT_U32      portGroupId;    /* port group id */
    GT_STATUS   retStatus;      /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    /* Check index of Policer Metering Entry */
    PRV_CPSS_DXCH_POLICERS_NUM_CHECK_MAC(devNum, stage, entryIndex);

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        retStatus =  prvCpssDxCh3PolicerInternalTableAccess(
                                devNum, portGroupId,
                                stage, entryIndex,
                                PRV_CPSS_DXCH3_POLICER_METERING_REFRESH_E);
        if (retStatus != GT_OK)
        {
            return retStatus;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPolicerPortGroupErrorGet
*
* DESCRIPTION:
*       Gets address and type of Policer Entry that had an ECC error.
*       This information available if error is happened and not read till now.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*
* OUTPUTS:
*       entryTypePtr    - pointer to the Type of Entry (Metering or Counting)
*                         that had an error.
*       entryAddrPtr    - pointer to the Policer Entry that had an error.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_EMPTY                    - on missing error information.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or portGroupsBmp.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPolicerPortGroupErrorGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    OUT CPSS_DXCH3_POLICER_ENTRY_TYPE_ENT   *entryTypePtr,
    OUT GT_U32                              *entryAddrPtr
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      regValue;       /* register value */
    GT_U32      entryTypeBit;   /* The bit number of entryType and the size of
                                   entry address in bits */
    GT_U32      addressBits;     /* The number of bits used for error address */
    GT_U32      portGroupId;    /* port group id */
    GT_STATUS   retStatus;      /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(entryTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(entryAddrPtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerErrorReg;
    retStatus = GT_EMPTY;

    /* initialize output parameters */
    *entryTypePtr = CPSS_DXCH3_POLICER_ENTRY_METERING_E;
    *entryAddrPtr = 0x0;

    /* Loop on all portGroups from the bitmap, untill first non-empty error is found */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum, portGroupsBmp, portGroupId)
    {

        /* Get Ingress Policer Error register */
        retStatus = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,
                                                        regAddr, &regValue);
        if (GT_OK == retStatus)
        {
            if(!((regValue >> 31) & 0x1))
            {
                retStatus = GT_EMPTY;

                /* move to the next portGroupId */
                continue;
            }

            if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
            {
                /* Lion and above */
                entryTypeBit = 29;
                addressBits = 24;
            }
            else if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
            {
                /* xCat */
                entryTypeBit = 24;
                addressBits = entryTypeBit;
            }
            else
            {
                /* DxCh3 */
                entryTypeBit = 30;
                addressBits = entryTypeBit;
            }

            *entryTypePtr = ((regValue >> entryTypeBit) & 0x1)?
                                        CPSS_DXCH3_POLICER_ENTRY_BILLING_E :
                                        CPSS_DXCH3_POLICER_ENTRY_METERING_E;

            *entryAddrPtr = regValue & (~(0xFFFFFFFF << addressBits));

            /* first non-empty error entry is found */
            break;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum, portGroupsBmp, portGroupId)


    return retStatus;
}

/*******************************************************************************
* cpssDxChPolicerPortGroupErrorCounterGet
*
* DESCRIPTION:
*       Gets the value of the Policer ECC Error Counter.
*       The Error Counter is a free-running non-sticky 8-bit read-only
*       counter.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*
* OUTPUTS:
*       cntrValuePtr    - pointer to the Policer ECC Error counter value.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or portGroupsBmp.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPolicerPortGroupErrorCounterGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    OUT GT_U32                              *cntrValuePtr
)
{
    GT_U32      regAddr;        /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(cntrValuePtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerErrorCntrReg;

    /* Get Ingress Policer Error Counter value */
    return prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp, regAddr,
                                                  0, 8, cntrValuePtr, NULL);
}

/*******************************************************************************
* cpssDxChPolicerPortGroupManagementCountersSet
*
* DESCRIPTION:
*       Sets the value of specified Management Counters.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*       mngCntrSet      - Management Counters Set[0..2].
*       mngCntrType     - Management Counters Type (GREEN, YELLOW, RED, DROP).
*       mngCntrPtr      - pointer to the Management Counters Entry must be set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage or mngCntrType,
*                                     Management Counters Set number or
*                                     portGroupsBmp.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       In data unit management counter only 32 bits are used for DxCh3 devices
*       and 42 bits are used for xCat and above devices.
*******************************************************************************/
GT_STATUS cpssDxChPolicerPortGroupManagementCountersSet
(
    IN GT_U8                                    devNum,
    IN GT_PORT_GROUPS_BMP                       portGroupsBmp,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT      mngCntrSet,
    IN CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT     mngCntrType,
    IN CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC    *mngCntrPtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      hwData[3];  /* data will be written to HW */
    GT_U32      wordEntryIndex; /* word base entry index in the Management Counters Table */
    GT_U32      numOfWords; /* number of words to write */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(mngCntrPtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    /* Verify Management Counters Set number and calculate Entry index */
    switch(mngCntrSet)
    {
        case CPSS_DXCH3_POLICER_MNG_CNTR_SET0_E:
        case CPSS_DXCH3_POLICER_MNG_CNTR_SET1_E:
        case CPSS_DXCH3_POLICER_MNG_CNTR_SET2_E:
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* Verify Management Counters Type and calculate Entry index */
    switch(mngCntrType)
    {
        case CPSS_DXCH3_POLICER_MNG_CNTR_GREEN_E:
        case CPSS_DXCH3_POLICER_MNG_CNTR_YELLOW_E:
        case CPSS_DXCH3_POLICER_MNG_CNTR_RED_E:
        case CPSS_DXCH3_POLICER_MNG_CNTR_DROP_E:
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* Nullify the hwData */
    cpssOsMemSet(hwData, 0, sizeof(hwData));

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */

        /* calculate word index */
        wordEntryIndex = (mngCntrSet * 16) + (mngCntrType * 4);

        /* number of words to write */
        numOfWords = 3;

        /* Create HW format of Management Counters Entry */
        hwData[0] = mngCntrPtr->duMngCntr.l[0];
        hwData[1] = (mngCntrPtr->duMngCntr.l[1] & 0x3FF) |
                    ((mngCntrPtr->packetMngCntr & 0x3FFFFF) << 10);
        hwData[2] = (mngCntrPtr->packetMngCntr >> 22);

    }
    else
    {
        /* DxCh3 */

        /* calculate word index */
        wordEntryIndex = (mngCntrSet * 8) + (mngCntrType * 2);

        /* number of words to write */
        numOfWords = 2;

        /* Create HW Management Counters Entry */
        hwData[0] = mngCntrPtr->duMngCntr.l[0];
        hwData[1] = mngCntrPtr->packetMngCntr;

    }

    regAddr = (0x4 * wordEntryIndex) +
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerManagementCntrsTbl;

    /* Set Management Counters Entry */
    return prvCpssDxChPortGroupsBmpCounterArraySet(devNum, portGroupsBmp,
                                                   regAddr, numOfWords,
                                                   &hwData[0]);
}

/*******************************************************************************
* cpssDxChPolicerPortGroupManagementCountersGet
*
* DESCRIPTION:
*       Gets the value of specified Management Counters.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*       mngCntrSet      - Management Counters Set[0..2].
*       mngCntrType     - Management Counters Type (GREEN, YELLOW, RED, DROP).
*
* OUTPUTS:
*       mngCntrPtr      - pointer to the requested Management Counters Entry.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage, mngCntrType,
*                                     Management Counters Set number or
*                                     portGroupsBmp.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       In data unit management counter only 32 bits are used for DxCh3 devices
*       and 42 bits are used for xCat and above devices.
*******************************************************************************/
GT_STATUS cpssDxChPolicerPortGroupManagementCountersGet
(
    IN  GT_U8                                   devNum,
    IN GT_PORT_GROUPS_BMP                       portGroupsBmp,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN  CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT     mngCntrSet,
    IN  CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT    mngCntrType,
    OUT CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC   *mngCntrPtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      regValue;   /* register hw value */
    GT_U32      wordEntryIndex; /* word base entry index in the Management Counters Table */
    GT_U32      numOfWords; /* number of words to write */
    GT_STATUS   retStatus;  /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(mngCntrPtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    /* Verify Management Counters Set number and calculate Entry index */
    switch(mngCntrSet)
    {
        case CPSS_DXCH3_POLICER_MNG_CNTR_SET0_E:
        case CPSS_DXCH3_POLICER_MNG_CNTR_SET1_E:
        case CPSS_DXCH3_POLICER_MNG_CNTR_SET2_E:
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* Verify Management Counters Type and calculate Entry index */
    switch(mngCntrType)
    {
        case CPSS_DXCH3_POLICER_MNG_CNTR_GREEN_E:
        case CPSS_DXCH3_POLICER_MNG_CNTR_YELLOW_E:
        case CPSS_DXCH3_POLICER_MNG_CNTR_RED_E:
        case CPSS_DXCH3_POLICER_MNG_CNTR_DROP_E:
            break;
        default:
            return GT_BAD_PARAM;
    }

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */

        /* calculate word index */
        wordEntryIndex = (mngCntrSet * 16) + (mngCntrType * 4);

        /* number of words to read */
        numOfWords = 3;

    }
    else
    {
        /* DxCh3 */

        /* calculate word index */
        wordEntryIndex = (mngCntrSet * 8) + (mngCntrType * 2);

        /* number of words to read */
        numOfWords = 2;
    }

    regAddr = (0x4 * wordEntryIndex) +
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerManagementCntrsTbl;

    /* Read a Data Unit counter part(Entry LSB) of Management Counters Entry */
    retStatus =
        prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp, regAddr,
                                               0,32, NULL, &mngCntrPtr->duMngCntr);
    if (retStatus != GT_OK)
    {
        return retStatus;
    }

    /* Read LSB of Management Counters Entry from the PLR Shadow reg */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerShadowReg;

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* Read Policer Shadow0 register, where are located the     */
       /* MSB of DU counter[10 bit] and the LSB of Packet Counter [22 bit] */
        retStatus = prvCpssDxChPortGroupsBmpMultiCounterSummary(devNum,
                                    portGroupsBmp, regAddr,
                                    0,10,
                                    &regValue,/* 10 MSbits of DU counter */
                                    10,22,
                                    &mngCntrPtr->packetMngCntr,/* 22 LSbits of Packt cntr */
                                    0,0,NULL);
        if (retStatus != GT_OK)
        {
            return retStatus;
        }

        mngCntrPtr->duMngCntr.l[1] += regValue;

        /* Calculate the address of the Policer second shadow register */
        regAddr += 0x4;

        /* Read Ingress Policer Shadow1 register, where   */
        /* are located the MSB of Packet Counter [10 bit] */
        retStatus = prvCpssDxChPortGroupsBmpCounterSummary(devNum,
                                                           portGroupsBmp,
                                                           regAddr,0,10,
                                                           &regValue,
                                                           NULL);
        if (retStatus != GT_OK)
        {
            return retStatus;
        }

        mngCntrPtr->packetMngCntr += (regValue << 22);
    }
    else
    {
        /* DxCh3 */

        /* Get Packet counter part(Entry MSB) of Management Counters Entry */
        retStatus = prvCpssDxChPortGroupsBmpCounterSummary(devNum,
                                               portGroupsBmp, regAddr, 0,32,
                                               &(mngCntrPtr->packetMngCntr),
                                               NULL);
    }

    return retStatus;
}

/*******************************************************************************
* cpssDxChPolicerPortGroupMeteringEntrySet
*
* DESCRIPTION:
*      Sets Metering Policer Entry.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*       entryIndex      - index of Policer Metering Entry.
*                         Range: see datasheet for specific device.
*       entryPtr        - pointer to the metering policer entry going to be set.
*
* OUTPUTS:
*       tbParamsPtr     - pointer to actual policer token bucket parameters.
*                         The token bucket parameters are returned as output
*                         values. This is due to the hardware rate resolution,
*                         the exact rate or burst size requested may not be
*                         honored. The returned value gives the user the
*                         actual parameters configured in the hardware.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on hardware error.
*       GT_TIMEOUT                  - on time out of Policer Tables
*                                     indirect access.
*       GT_BAD_PARAM                - wrong devNum, stage, entryIndex,
*                                     entry parameters or portGroupsBmp.
*       GT_OUT_OF_RANGE             - on Billing Entry Index out of range.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPolicerPortGroupMeteringEntrySet
(
    IN  GT_U8                                   devNum,
    IN GT_PORT_GROUPS_BMP                       portGroupsBmp,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN  GT_U32                                  entryIndex,
    IN  CPSS_DXCH3_POLICER_METERING_ENTRY_STC   *entryPtr,
    OUT CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT  *tbParamsPtr
)
{
    GT_U32              hwData;         /* data to write to HW */
    GT_U32              regAddr;        /* register address */
    GT_U32              hwRate0;        /* HW rate 0 value */
    GT_U32              hwRate1;        /* HW rate 1 value */
    GT_U32              hwBurstSize0;   /* HW Max Burst Size 0 value */
    GT_U32              hwBurstSize1;   /* HW Max Burst Size 1 value */
    GT_U32              hwRateType0;    /* HW rate type 0 value */
    GT_U32              hwRateType1;    /* HW rate type 1 value */
    GT_U32              hwBucketSize0;  /* HW Bucket Size 0 value */
    GT_U32              hwBucketSize1;  /* HW Bucket Size 1 value */
    GT_U32              portGroupId;    /* port group id */
    GT_STATUS           retStatus;      /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(entryPtr);
    CPSS_NULL_PTR_CHECK_MAC(tbParamsPtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    /* Check index of Policer Metering Entry */
    PRV_CPSS_DXCH_POLICERS_NUM_CHECK_MAC(devNum,stage,entryIndex);


    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        /* loop on all active port groups in the bmp */

        /* To write the rest of the entry,
          call private xCat A1 and above API */
        retStatus = prvCpssDxChPolicerMeteringEntrySet(devNum, stage,
                                                       entryPtr, tbParamsPtr);
        if (retStatus != GT_OK)
        {
            return retStatus;
        }
    }
    else
    {
        /* DxCh3 */

        /* Calculate Token Backet  */
        switch(entryPtr->meterMode)
        {
            case CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E:
                retStatus = prvCpssDxCh3PolicerHwTbCalculate(
                               devNum,
                               stage,
                               entryPtr->tokenBucketParams.srTcmParams.cir,
                               entryPtr->tokenBucketParams.srTcmParams.cbs,
                               entryPtr->tokenBucketParams.srTcmParams.ebs,
                               &(tbParamsPtr->srTcmParams.cir), /* Rate0 = Rate1 */
                               &(tbParamsPtr->srTcmParams.cbs), /* MaxBurstSize0 */
                               &(tbParamsPtr->srTcmParams.ebs), /* MaxBurstSize1 */
                               &hwRateType0);      /* RateType0 = RateType1 */
                if (retStatus != GT_OK)
                {
                    return retStatus;
                }
                hwRateType1 = hwRateType0;          /* RateType0 = RateType1 */
                hwRate0 = tbParamsPtr->srTcmParams.cir;
                hwRate1 = hwRate0;                              /* Rate0 = Rate1 */
                hwBurstSize0 = tbParamsPtr->srTcmParams.cbs;    /* MaxBurstSize0 */
                hwBurstSize1 = tbParamsPtr->srTcmParams.ebs;    /* MaxBurstSize1 */
                break;

            case CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E:
                retStatus = prvCpssDxCh3PolicerHwTbCalculate(
                               devNum,
                               stage,
                               entryPtr->tokenBucketParams.trTcmParams.cir,
                               entryPtr->tokenBucketParams.trTcmParams.cbs,
                               0,                   /* Not exists in the TrTCM */
                               &(tbParamsPtr->trTcmParams.cir), /* Rate0 */
                               &(tbParamsPtr->trTcmParams.cbs), /* MaxBurstSize0 */
                               NULL,                /* Not exists in the TrTCM */
                               &hwRateType0);       /* RateType0 */
                if (retStatus != GT_OK)
                {
                    return retStatus;
                }
                hwRate0 = tbParamsPtr->trTcmParams.cir;         /* Rate0 */
                hwBurstSize0 = tbParamsPtr->trTcmParams.cbs;    /* MaxBurstSize0 */

                retStatus = prvCpssDxCh3PolicerHwTbCalculate(
                               devNum,
                               stage,
                               entryPtr->tokenBucketParams.trTcmParams.pir,
                               entryPtr->tokenBucketParams.trTcmParams.pbs,
                               0,                   /* Not exists in the TrTCM */
                               &(tbParamsPtr->trTcmParams.pir), /* Rate1 */
                               &(tbParamsPtr->trTcmParams.pbs), /* MaxBurstSize1 */
                               NULL,                /* Not exists in the TrTCM */
                               &hwRateType1);       /* RateType1 */
                if (retStatus != GT_OK)
                {
                    return retStatus;
                }
                hwRate1 = tbParamsPtr->trTcmParams.pir;         /* Rate1 */
                hwBurstSize1 = tbParamsPtr->trTcmParams.pbs;    /* MaxBurstSize1 */

                break;

            default:
                return GT_BAD_PARAM;
        }

        /*************************************************/
        /* Convert actual TB parameters to the SW format */
        /*************************************************/
        retStatus = prvCpssDxCh3PolicerSwTbCalculate(entryPtr->meterMode,
                                                hwRateType0, hwRateType1,
                                                tbParamsPtr, tbParamsPtr);
        if (retStatus != GT_OK)
        {
            return retStatus;
        }

        /* get bucket size values in bytes as Maximal Burst size of token bucket */
        if(entryPtr->meterMode == CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E)
        {
            hwBucketSize0 = tbParamsPtr->srTcmParams.cbs;
            hwBucketSize1 = tbParamsPtr->srTcmParams.ebs;
        }
        else
        {
            hwBucketSize0 = tbParamsPtr->trTcmParams.cbs;
            hwBucketSize1 = tbParamsPtr->trTcmParams.pbs;
        }
        /**********/
        /* Word 1 */
        /**********/
        /* set Bucket size 0 - LSB 6 bits to bits 26:31 */
        regAddr = 0x4 +
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerTblAccessDataReg;
        /* Set Ingress Policer Table Access Data 1 Register */
        retStatus = prvCpssDrvHwPpSetRegField(devNum, regAddr, 26, 6,
                                              (hwBucketSize0 & 0x3f));
        if (retStatus != GT_OK)
        {
            return retStatus;
        }

        /**********/
        /* Word 2 */
        /**********/
        /* set Bucket size 0 - MSB 26 bits to bits 0:25,
           set Bucket size 1 - LSB 6 bits to bits 26:31 */
        hwData = (hwBucketSize0 >> 6) | ((hwBucketSize1 & 0x3f) << 26);

        regAddr = 0x8 +
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerTblAccessDataReg;
        /* Set Ingress Policer Table Access Data 1 Register */
        retStatus = prvCpssDrvHwPpWriteRegister(devNum, regAddr, hwData);
        if (retStatus != GT_OK)
        {
            return retStatus;
        }
        /**********/
        /* Word 3 */
        /**********/
        /* set Bucket size 1 - MSB 26 bits to bits 0:25 */
        hwData = (hwBucketSize1 >> 6);

        /* Set Policer command for Red Packet */
        switch(entryPtr->redPcktCmd)
        {
            case CPSS_DXCH3_POLICER_NON_CONFORM_CMD_NO_CHANGE_E:
                /* bits 26:27 = 0 */
                break;
            case CPSS_DXCH3_POLICER_NON_CONFORM_CMD_DROP_E:
                hwData |= (1 << 26);
                break;
            case CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_E:
                hwData |= (2 << 26);
                break;
            default:
                return GT_BAD_PARAM;
        }

        /* Get Policer commands for Yellow Packet */
        switch(entryPtr->yellowPcktCmd)
        {
            case CPSS_DXCH3_POLICER_NON_CONFORM_CMD_NO_CHANGE_E:
                /* bits 28:29 = 0 */
                break;
            case CPSS_DXCH3_POLICER_NON_CONFORM_CMD_DROP_E:
                hwData |= (1 << 28);
                break;
            case CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_E:
                hwData |= (2 << 28);
                break;
            default:
                return GT_BAD_PARAM;
        }

        /* Set 2 LSB of RateType0 value */
        hwData |= ((hwRateType0 & 0x3) << 30);

        regAddr = 0xC +
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerTblAccessDataReg;
        /* Set Ingress Policer Table Access Data 3 Register */
        retStatus = prvCpssDrvHwPpWriteRegister(devNum, regAddr, hwData);
        if (retStatus != GT_OK)
        {
            return retStatus;
        }

        /**********/
        /* Word 4 */
        /**********/

        /* Set 1 MSB of RateType0 value */
        hwData = (hwRateType0 >> 2) & 0x1;
        /* Set RateType1 value */
        hwData |= (hwRateType1 << 1);

        /* Set Color Mode */
        switch(entryPtr->meterColorMode)
        {
            case CPSS_POLICER_COLOR_BLIND_E:
                break;
            case CPSS_POLICER_COLOR_AWARE_E:
                hwData |= (1 << 4);
                break;
            default:
                return GT_BAD_PARAM;
        }

        /* Set Meter Mode */
        switch(entryPtr->meterMode)
        {
            case CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E:
                break;
            case CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E:
                hwData |= (1 << 5);
                break;
            default:
                return GT_BAD_PARAM;
        }

        /* Set Rate 0 (CIR) */
        hwData |= (hwRate0 << 6);
        /* Set Rate 1 (PIR) */
        hwData |= (hwRate1 << 16);
        /* Set 6 LSB of Maximum burst size for bucket 0 */
        hwData |= ((hwBurstSize0 & 0x3F) << 26);

        regAddr = 0x10 +
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerTblAccessDataReg;

        /* Set Ingress Policer Table Access Data 4 Register */
        retStatus = prvCpssDrvHwPpWriteRegister(devNum, regAddr, hwData);
        if (retStatus != GT_OK)
        {
            return retStatus;
        }

        /**********/
        /* Word 5 */
        /**********/

        /* Set 10 MSB of Maximum burst size for bucket 0 */
        hwData = ((hwBurstSize0 >> 6) & 0x3FF);
        /* Set Maximum burst size for bucket 1 */
        hwData |= (hwBurstSize1 << 10);

        /* Set Modify DSCP operation */
        switch(entryPtr->modifyDscp)
        {
            case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
                break;
            case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
                hwData |= (1 << 26);
                break;
            case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
                hwData |= (2 << 26);
                break;
            default:
                return GT_BAD_PARAM;
        }

        /* Set Modify UP operation */
        switch(entryPtr->modifyUp)
        {
            case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
                break;
            case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
                hwData |= (1 << 28);
                break;
            case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
                hwData |= (2 << 28);
                break;
            default:
                return GT_BAD_PARAM;
        }

        /* FE#2046 policer - QoS attributes incorrect modification for out-of-profile traffic */
        if (PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                                         PRV_CPSS_DXCH3_OUT_OF_PROFILE_QOS_ATTR_E))
        {
            if (entryPtr->yellowPcktCmd !=
                    CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_E &&
                entryPtr->redPcktCmd !=
                    CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_E)
            {
             /* Set zero to ModifyDSCP and ModifyUP bits if policer commands for
                Non-Conforming Yellows/Reds are different from CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_E */
                hwData &= ~(0xf << 26);
            }
        }

        /* Set Management Counter Set */
        switch(entryPtr->mngCounterSet)
        {
            case CPSS_DXCH3_POLICER_MNG_CNTR_SET0_E:
                break;
            case CPSS_DXCH3_POLICER_MNG_CNTR_SET1_E:
                hwData |= (1 << 30);
                break;
            case CPSS_DXCH3_POLICER_MNG_CNTR_SET2_E:
                hwData |= (2 << 30);
                break;
            case CPSS_DXCH3_POLICER_MNG_CNTR_DISABLED_E:
                hwData |= (3 << 30);
                break;
            default:
                return GT_BAD_PARAM;
        }

        regAddr = 0x14 +
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerTblAccessDataReg;
        /* Set Ingress Policer Table Access Data 5 Register */
        retStatus = prvCpssDrvHwPpWriteRegister(devNum, regAddr, hwData);
        if (retStatus != GT_OK)
        {
            return retStatus;
        }

        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
        {
            /* Word 6, Offset Formula: 0xC000030 + n*4 + max(n-5,0)*4:
                       where n (0-6) represents Word. For word 6 used additional
                       +4 in a address  */
            regAddr = 0x1C +
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerTblAccessDataReg;

        }
        else
        {
            /* Set Ingress Policer Table Access Data 6 Register */
            regAddr = 0x18 +
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerTblAccessDataReg;
        }

        /* Check index of Policer Billing Entry */
        if(entryPtr->countingEntryIndex >= BIT_10)/*10 bits*/
        {
            return GT_OUT_OF_RANGE;
        }

        /* Set pointer to related billing entry (Billing Entry Index)*/
        hwData = entryPtr->countingEntryIndex;

        /* Set Ingress Policer Table Access Data 6 Register */
        retStatus = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 10, hwData);
        if (retStatus != GT_OK)
        {
            return retStatus;
        }
    }


    /*******************************************************/
    /* Update Policer Metering Entry in the Internal Table */
    /*******************************************************/
    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        retStatus =
            prvCpssDxCh3PolicerInternalTableAccess(devNum, portGroupId, stage,
                             entryIndex,
                             PRV_CPSS_DXCH3_POLICER_METERING_UPDATE_E);
        if (retStatus != GT_OK)
        {
            return retStatus;
        }

    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}


/*******************************************************************************
* cpssDxChPolicerPortGroupMeteringEntryGet
*
* DESCRIPTION:
*      Gets Metering Policer Entry configuration.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*       entryIndex      - index of Policer Metering Entry.
*                         Range: see datasheet for specific device.
*
* OUTPUTS:
*       entryPtr        - pointer to the requested metering policer entry.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - wrong devNum, stage, entryIndex or
*                                     portGroupsBmp.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*       GT_BAD_STATE                - on bad value in a entry.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPolicerPortGroupMeteringEntryGet
(
    IN  GT_U8                                   devNum,
    IN  GT_PORT_GROUPS_BMP                      portGroupsBmp,
    IN  CPSS_DXCH_POLICER_STAGE_TYPE_ENT        stage,
    IN  GT_U32                                  entryIndex,
    OUT CPSS_DXCH3_POLICER_METERING_ENTRY_STC   *entryPtr
)
{
    GT_U32              regAddr;        /* register address */
    GT_U32              hwData[5];      /* data read from HW */
    GT_U32              hwRateType0;    /* HW rate type 0 value */
    GT_U32              hwRateType1;    /* HW rate type 1 value */
    GT_U32              hwRate0;        /* HW rate 0 value */
    GT_U32              hwRate1;        /* HW rate 1 value */
    GT_U32              hwBurstSize0;   /* HW Max Burst Size 0 value */
    GT_U32              hwBurstSize1;   /* HW Max Burst Size 1 value */
    GT_U32              portGroupId;    /* port group id */
    GT_STATUS           retStatus;      /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(entryPtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    /* Check index of Policer Metering Entry */
    PRV_CPSS_DXCH_POLICERS_NUM_CHECK_MAC(devNum,stage,entryIndex);

    /* Get the first active port group */
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(devNum, portGroupsBmp,
                                                        portGroupId);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */

        retStatus = prvCpssDxChPolicerMeteringEntryGet(devNum, portGroupId,
                                                       stage, entryIndex,
                                                       entryPtr);
        if (retStatus != GT_OK)
        {
            return retStatus;
        }
    }
    else
    {
        /* DxCh3 */

        /*********************************************************/
        /* Read Word 3,4,5,6 of requesred Policer Metering Entry */
        /*********************************************************/
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            PLR[stage].policerMeteringCountingTbl + (entryIndex * 0x20) + 12;

        retStatus = prvCpssDrvHwPpPortGroupReadRam(devNum, portGroupId,
                                                   regAddr, 4, &hwData[0]);
        if (retStatus != GT_OK)
        {
            return retStatus;
        }

     /* Get Policer command for Red Packet */
        switch((hwData[0] >> 26) & 0x3)
        {
            case 0:
                entryPtr->redPcktCmd =
                                    CPSS_DXCH3_POLICER_NON_CONFORM_CMD_NO_CHANGE_E;
                break;
            case 1:
                entryPtr->redPcktCmd = CPSS_DXCH3_POLICER_NON_CONFORM_CMD_DROP_E;
                break;
            case 2:
                entryPtr->redPcktCmd = CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_E;
                break;
            default:
                return GT_BAD_STATE;
        }

        /* Get Policer command for Yellow Packet */
        switch((hwData[0] >> 28) & 0x3)
        {
            case 0:
                entryPtr->yellowPcktCmd =
                                    CPSS_DXCH3_POLICER_NON_CONFORM_CMD_NO_CHANGE_E;
                break;
            case 1:
                entryPtr->yellowPcktCmd = CPSS_DXCH3_POLICER_NON_CONFORM_CMD_DROP_E;
                break;
            case 2:
                entryPtr->yellowPcktCmd =
                                    CPSS_DXCH3_POLICER_NON_CONFORM_CMD_REMARK_E;
                break;
            default:
                return GT_BAD_STATE;
        }

        /* Get Color Mode */
        entryPtr->meterColorMode = ((hwData[1] >> 4) & 0x1) ?
                                                CPSS_POLICER_COLOR_AWARE_E :
                                                CPSS_POLICER_COLOR_BLIND_E;

        /* Get Meter Mode */
        entryPtr->meterMode = ((hwData[1] >> 5) & 0x1) ?
                                 CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E :
                                 CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E;


        /* Get Modify DSCP operation */
        switch((hwData[2] >> 26) & 0x3)
        {
            case 0:
                entryPtr->modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
                break;
            case 1:
                entryPtr->modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
                break;
            case 2:
                entryPtr->modifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
                break;
            default:
                return GT_BAD_STATE;
        }

        /* Get Modify UP operation */
        switch((hwData[2] >> 28) & 0x3)
        {
            case 0:
                entryPtr->modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
                break;
            case 1:
                entryPtr->modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
                break;
            case 2:
                entryPtr->modifyUp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
                break;
            default:
                return GT_BAD_STATE;
        }

        /* Get Management Counter Set */
        switch((hwData[2] >> 30) & 0x3)
        {
            case 0:
                entryPtr->mngCounterSet = CPSS_DXCH3_POLICER_MNG_CNTR_SET0_E;
                break;
            case 1:
                entryPtr->mngCounterSet = CPSS_DXCH3_POLICER_MNG_CNTR_SET1_E;
                break;
            case 2:
                entryPtr->mngCounterSet = CPSS_DXCH3_POLICER_MNG_CNTR_SET2_E;
                break;
            case 3:
                entryPtr->mngCounterSet = CPSS_DXCH3_POLICER_MNG_CNTR_DISABLED_E;
                break;
            default:
                return GT_FAIL; /* never should be reached*/
        }

        /* Get Billing Entry Index */
        entryPtr->countingEntryIndex = hwData[3] & 0x3FF;

        /* Get RateType0 value */
        hwRateType0 = ((hwData[0] >> 30) & 0x3) | ((hwData[1] & 0x1) << 2);
        /* Get ateType0 value */
        hwRateType1 = (hwData[1] >> 1) & 0x7;
        /* Get Rate 0 (CIR) */
        hwRate0 = (hwData[1] >> 6) & 0x3FF;
        /* Get Rate 1 (PIR) */
        hwRate1 = (hwData[1] >> 16) & 0x3FF;
        /* Get Maximum burst size for bucket 0 */
        hwBurstSize0 = ((hwData[1] >> 26) & 0x3F) | ((hwData[2] & 0x3FF) << 6);
        /* Get Maximum burst size for bucket 1 */
        hwBurstSize1 = (hwData[2] >> 10) & 0xFFFF;

        /****************************************************/
        /* Convert Token Backet Parameters in the SW format */
        /****************************************************/
        switch(entryPtr->meterMode)
        {
            case CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E:
                entryPtr->tokenBucketParams.srTcmParams.cir = hwRate0;
                entryPtr->tokenBucketParams.srTcmParams.cbs = hwBurstSize0;
                entryPtr->tokenBucketParams.srTcmParams.ebs = hwBurstSize1;

                retStatus = prvCpssDxCh3PolicerSwTbCalculate(
                    entryPtr->meterMode,
                    hwRateType0,
                    0,
                    &(entryPtr->tokenBucketParams),
                    &(entryPtr->tokenBucketParams));

                if (retStatus != GT_OK)
                {
                    return retStatus;
                }
                break;

            case CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E:
                entryPtr->tokenBucketParams.trTcmParams.cir = hwRate0;
                entryPtr->tokenBucketParams.trTcmParams.pir = hwRate1;
                entryPtr->tokenBucketParams.trTcmParams.cbs = hwBurstSize0;
                entryPtr->tokenBucketParams.trTcmParams.pbs = hwBurstSize1;

                retStatus = prvCpssDxCh3PolicerSwTbCalculate(
                    entryPtr->meterMode,
                    hwRateType0,
                    hwRateType1,
                    &(entryPtr->tokenBucketParams),
                    &(entryPtr->tokenBucketParams));

                if (retStatus != GT_OK)
                {
                    return retStatus;
                }
                break;

            default:
                return GT_FAIL; /* never should be reached*/
        }
    }

    return retStatus;
}

/*******************************************************************************
* cpssDxChPolicerPortGroupBillingEntrySet
*
* DESCRIPTION:
*       Sets Policer Billing Counters.
*       Billing is the process of counting the amount of traffic that belongs
*       to a flow. This process can be activated only by metering and is used
*       either for statistical analysis of a flow's traffic or for actual
*       billing.
*       The billing counters are free-running no-sticky counters.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*       entryIndex      - index of Policer Billing Counters Entry.
*                         Range: see datasheet for specific device.
*       billingCntrPtr  - pointer to the Billing Counters Entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_TIMEOUT                  - on time out upon counter reset by
*                                     indirect access.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage, entryIndex or
*                                     portGroupsBmp.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       The packet is counted per outgoing DP or Conformance Level. When
*       metering is disabled, billing is always done according to packet's
*       Drop Precedence.
*       When metering is activated per source port, Billing counting cannot
*       be enabled.
*       None-zeroed setting of Billing Counters while metring/counting is
*       enabled may cause data coherency problems.
*
*       In order to set Billing entry under traffic perform the following
*       algorithm:
*           - disconnect entryes from traffic
*           - call cpssDxChPolicerCountingWriteBackCacheFlush
*           - set new values
*           - connect entries back to traffic
*       This API address the same memory area (counters place) as can be set
*       by "cpssDxChIpfixEntrySet".
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerPortGroupBillingEntrySet
(
    IN  GT_U8                                   devNum,
    IN  GT_PORT_GROUPS_BMP                      portGroupsBmp,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN  GT_U32                                  entryIndex,
    IN  CPSS_DXCH3_POLICER_BILLING_ENTRY_STC    *billingCntrPtr
)
{
    GT_U32      regAddr;            /* register address */
    GT_U32      hwData[7];          /* HW data */
    GT_U32      portGroupId;        /* port group id */
    GT_STATUS   retStatus;          /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(billingCntrPtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    /* Check index of Policer Metering Entry */
    PRV_CPSS_DXCH_POLICERS_NUM_CHECK_MAC(devNum,stage,entryIndex);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        retStatus = prvCpssDxChPolicerBillingEntrySet(devNum, portGroupsBmp,
                                                      stage, entryIndex,
                                                      billingCntrPtr);
    }
    else
    {
        /* DxCh3 and xCat A0 */

        /* check if the device type is xCat A0 */
        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* xCat A0 */

            /* configure Billing counter mode */
            retStatus = prvCpssDxChPolicerBillingCountersModeSet(devNum,
                                            billingCntrPtr->billingCntrMode);
            if (retStatus != GT_OK)
            {
                return retStatus;
            }
        }

        /* Using indirect access to the IPLR Table in case of counter's reset */
        if((0 == billingCntrPtr->greenCntr.l[0]) &&
           (0 == billingCntrPtr->yellowCntr.l[0]) &&
           (0 == billingCntrPtr->redCntr.l[0]))
        {
            /* loop on all active port groups in the bmp */
            PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
            {
                /* Perform reset by indirect access to the Policer Table */
                retStatus = prvCpssDxCh3PolicerInternalTableAccess(devNum, portGroupId,
                                                                   stage, entryIndex,
                                         PRV_CPSS_DXCH3_POLICER_CNTR_READ_AND_RESET_E);
                if (retStatus != GT_OK)
                {
                    return retStatus;
                }
            }
            PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

            return GT_OK;
        }

        /* Nullify the hwData */
        cpssOsMemSet(hwData, 0, sizeof(hwData));

        /* Remap Application Entry index to the HW Entry index */
        entryIndex += PRV_CPSS_DXCH3_POLICER_ENTRY_NUM_CNS;

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        PLR[stage].policerMeteringCountingTbl + (entryIndex * 0x20);

        /************************/
        /* Create HW entry data */
        /************************/
        hwData[0] = billingCntrPtr->greenCntr.l[0];
        hwData[1] = billingCntrPtr->yellowCntr.l[0];
        hwData[2] = billingCntrPtr->redCntr.l[0];

        /* Set counters value */
        retStatus = prvCpssDxChPortGroupsBmpCounterArraySet(devNum, portGroupsBmp,
                                                            regAddr, 7, &hwData[0]);
    }

    return retStatus;
}

/*******************************************************************************
* cpssDxChPolicerPortGroupBillingEntryGet
*
* DESCRIPTION:
*       Gets Policer Billing Counters.
*       Billing is the process of counting the amount of traffic that belongs
*       to a flow. This process can be activated only by metering and is used
*       either for statistical analysis of a flow's traffic or for actual
*       billing.
*       The billing counters are free-running no-sticky counters.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above devices
*                         and ignored by DxCh3.
*       entryIndex      - index of Policer Billing Counters Entry.
*                         Range: see datasheet for specific device.
*       reset           - reset flag:
*                         GT_TRUE  - performing read and reset atomic operation.
*                         GT_FALSE - performing read counters operation only.
*
* OUTPUTS:
*       billingCntrPtr  - pointer to the Billing Counters Entry.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_TIMEOUT                  - on time out of IPLR Table indirect access.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage, entryIndex or
*                                     portGroupsBmp.
*       GT_BAD_STATE                - on counter entry type mismatch.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       The packet is counted per outgoing DP or Conformance Level. When
*       metering is disabled, billing is always done according to packet's
*       Drop Precedence.
*       When metering is activated per source port, Billing counting cannot
*       be enabled.
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerPortGroupBillingEntryGet
(
    IN  GT_U8                                   devNum,
    IN  GT_PORT_GROUPS_BMP                      portGroupsBmp,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage,
    IN  GT_U32                                  entryIndex,
    IN  GT_BOOL                                 reset,
    OUT CPSS_DXCH3_POLICER_BILLING_ENTRY_STC    *billingCntrPtr
)
{
    GT_U32      regAddr;            /* register address */
    GT_U32      portGroupId;        /* port group id */
    GT_STATUS   retStatus;          /* generic return status code */

    PRV_CPSS_DXCH3_POLICER_ACCESS_CTRL_CMD_ENT  accessAction;/* access action */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(billingCntrPtr);
    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    /* Check index of Policer Metering Entry */
    PRV_CPSS_DXCH_POLICERS_NUM_CHECK_MAC(devNum,stage,entryIndex);


    /* Define Policer Table Indirect Access triggered action */
    accessAction = (reset == GT_TRUE) ?
                            PRV_CPSS_DXCH3_POLICER_CNTR_READ_AND_RESET_E :
                            PRV_CPSS_DXCH3_POLICER_CNTR_READ_ONLY_E;

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        /* Perform indirect access to the Policer Table */
        retStatus = prvCpssDxCh3PolicerInternalTableAccess(devNum,
                                                           portGroupId,
                                                           stage,
                                                           entryIndex,
                                                           accessAction);
        if (GT_OK != retStatus)
        {
            return retStatus;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)




    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above */
        retStatus = prvCpssDxChPolicerBillingEntryGet(devNum,
                                                      portGroupsBmp,
                                                      stage,
                                                      billingCntrPtr);
    }
    else
    {
        /* DxCh3 and xCat A0 */

        /* check if the device type is xCat A0 */
        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* xCat A0 */

            /* configure Billing counter mode */
            retStatus = prvCpssDxChPolicerBillingCountersModeGet(devNum,
                                            &billingCntrPtr->billingCntrMode);
            if (retStatus != GT_OK)
            {
                return retStatus;
            }
        }

        /******************************************************************/
        /* Get counter value from the corresponding Access Data Registers */
        /******************************************************************/

        /* Get Ingress Policer Table Access Data 0 Register Address */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[stage].policerTblAccessDataReg;

        /* Get value of Green Billing counter */
        retStatus = prvCpssDxChPortGroupsBmpCounterSummary(devNum,
                                        portGroupsBmp, regAddr,0,32,
                                        &(billingCntrPtr->greenCntr.l[0]),NULL);
        if (GT_OK != retStatus)
        {
            return retStatus;
        }

        /* Get value of Yellow Billing counter */
        regAddr += 4;
        retStatus = prvCpssDxChPortGroupsBmpCounterSummary(devNum,
                                       portGroupsBmp, regAddr,0,32,
                                       &(billingCntrPtr->yellowCntr.l[0]),NULL);

        if (GT_OK == retStatus)
        {
            /* Get value of Red Billing counter */
            regAddr += 4;
            retStatus = prvCpssDxChPortGroupsBmpCounterSummary(devNum,
                                         portGroupsBmp, regAddr,0,32,
                                         &(billingCntrPtr->redCntr.l[0]),NULL);
        }
    }

    return retStatus;
}

/*******************************************************************************
* cpssDxChPolicerPortGroupPolicyCntrSet
*
* DESCRIPTION:
*      Sets Policer Policy Counters located in the Policer Counting Entry upon
*      enabled Policy Counting mode.
*      When working in Policy Counting Mode each counter counts the number of
*      packets matching on a Policy rules.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       index           - index set by Policy Action Entry or Metering entry
*                         and belongs to the range from 0 up to maximal number
*                         of Policer Policy counters.
*                         Maximal number is number of policer entries for
*                         counting is number of counting entries
*                         multiplied by 8.
*                         Some indexes in the range are not valid. Not valid
*                         indexes are:
*                           - (index % 8) == 6
*                           - (index % 8) == 7
*                         e.g. not valid indexes 6, 7, 14, 15, 22, 23 ...
*       cntrValue       - packets counter.
*                         For counter reset this parameter should be nullified.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on disabled Policy Counting.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or index (index range is limited by
*                         max number of Policer Policy counters) or portGroupsBmp.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Policy counters are free-running no-sticky counters.
*
*       In order to set Policy counting entry under traffic perform the following
*       algorithm:
*           - disconnect entryes from traffic
*           - call cpssDxChPolicerCountingWriteBackCacheFlush
*           - set new values
*           - connect entries back to traffic
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerPortGroupPolicyCntrSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  CPSS_DXCH_POLICER_STAGE_TYPE_ENT    stage,
    IN  GT_U32                              index,
    IN  GT_U32                              cntrValue
)
{
    GT_U32  countNumber;    /* the number of counter in the Entry */
    GT_U32  entryIndex;     /* entry index in the Internal Table */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    /* calculate the index inside Policer Counting Table */
    /* Policy Entry allignment is 8, only 6 indexes are in use. */
    entryIndex = (index >> 3);

    /* calculate the word offset inside the entry */
    countNumber = (index & 0x7);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)  
    {
        /* word offsets 4,5,6 and 7 are not valid */
        if (countNumber > 3) 
        {
            return GT_BAD_PARAM;
        }
    }
    else
    {
        /* word offsets 6 and 7 are not valid */
        if ((countNumber == 6) || (countNumber == 7))
        {
            return GT_BAD_PARAM;
        }
    }

    /* Set counters value */
    return prvCpssDxChPolicerCountingEntryWordSet(devNum, portGroupsBmp, stage,
                                                  entryIndex, countNumber,
                                                  cntrValue);
}

/*******************************************************************************
* cpssDxChPolicerPortGroupPolicyCntrGet
*
* DESCRIPTION:
*      Gets Policer Policy Counters located in the Policer Counting Entry upon
*      enabled Policy Counting mode.
*      When working in Policy Counting Mode each counter counts the number of
*      packets matching on a a Policy rules.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       index           - index set by Policy Action Entry or Metering entry
*                         and belongs to the range from 0 up to maximal number
*                         of Policer Policy counters.
*                         Maximal number is number of policer entries for
*                         counting is number of counting entries
*                         multiplied by 8.
*                         Some indexes in the range are not valid. Not valid
*                         indexes are:
*                           - (index % 8) == 6
*                           - (index % 8) == 7
*                         e.g. not valid indexes 6, 7, 14, 15, 22, 23 ...
*
* OUTPUTS:
*       cntrValuePtr    - Pointer to the packet counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on disabled Policy Counting.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum, index or portGroupsBmp.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_TIMEOUT               - on time out.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Policy counters are free-running no-sticky counters.
*
*******************************************************************************/
GT_STATUS cpssDxChPolicerPortGroupPolicyCntrGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT     stage,
    IN  GT_U32                              index,
    OUT GT_U32                              *cntrValuePtr
)
{
    GT_U32  regAddr;        /* register address */
    GT_U32  countNumber;    /* the number of counter in the Entry */
    GT_U32  entryIndex;     /* entry index in the Internal Table */
    GT_STATUS rc;           /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PLR_STAGE_CHECK_MAC(devNum, stage);
    CPSS_NULL_PTR_CHECK_MAC(cntrValuePtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    /* calculate the index inside Policer Counting Table */
    entryIndex = (index >> 3);

    /* check allocated number of counters - Policy Entry allignment is 8,
       only 6 indexes are in use. */
    PRV_CPSS_DXCH_POLICERS_NUM_CHECK_MAC(devNum, stage, entryIndex);

    /* calculate the word offset inside the entry */
    countNumber = (index & 0x7);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)  
    {
        /* word offsets 4,5,6 and 7 are not valid */
        if (countNumber > 3) 
        {
            return GT_BAD_PARAM;
        }
    }
    else
    {
        /* word offsets 6 and 7 are not valid */
        if ((countNumber == 6) || (countNumber == 7))
        {
            return GT_BAD_PARAM;
        }
    }

    /* Check if Policer Counting memory errata is enabled */
    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                          PRV_CPSS_DXCH_LION_POLICER_COUNTING_MEM_ACCESS_WA_E))
    {
        /* Trigger Indirect Read from Counting memory */
        rc = prvCpssDxCh3PolicerInternalTableAccess(devNum,
                              CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                              stage,
                              entryIndex,
                              PRV_CPSS_DXCH3_POLICER_CNTR_READ_ONLY_E);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* Get Ingress Policer Table Access Data 0 Register Address */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        PLR[stage].policerTblAccessDataReg;
    }
    else
    {
        /* Direct access to counting memory */

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        PLR[stage].policerCountingTbl + (entryIndex * 0x20);
    }

    /* calculate the word address inside the table */
    regAddr += (countNumber * 0x4);

    /* Get counters value */
    return prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                  regAddr, 0, 32, cntrValuePtr,
                                                  NULL);
}

/*******************************************************************************
* prvCpssDxCh3PolicerAccessControlBusyWait
*
* DESCRIPTION:
*       Do busy wait on Policer Access Control Register.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - physical device number.
*       portGroupsBmp   - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                       bitmap must be set with at least one bit representing
*                       valid port group(s). If a bit of non valid port group
*                       is set then function returns GT_BAD_PARAM.
*                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for xCat and above
*                         devices and ignored by DxCh3.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_TIMEOUT      - on time out.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PARAM    - on wrong input parameters.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssDxCh3PolicerAccessControlBusyWait
(
    IN GT_U8                                        devNum,
    IN GT_PORT_GROUPS_BMP                           portGroupsBmp,
    IN CPSS_DXCH_POLICER_STAGE_TYPE_ENT             stage
)
{
    GT_U32      regAddr;                /* register address */
    GT_U32      portGroupId;            /* port group id    */
    GT_STATUS   retStatus;              /* return code      */

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                        PLR[stage].policerTblAccessControlReg;

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        retStatus = prvCpssDxChPortGroupBusyWait(devNum,portGroupId,regAddr,0,GT_FALSE);
        if (GT_OK != retStatus)
        {
            return retStatus;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}



