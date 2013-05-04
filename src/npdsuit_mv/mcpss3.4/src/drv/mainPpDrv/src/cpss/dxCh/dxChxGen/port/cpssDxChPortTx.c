/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortTx.c
*
* DESCRIPTION:
*       CPSS implementation for configuring the Physical Port Tx Traffic Class
*       Queues.
*       This covers:
*       - queuing enable/disable
*       - transmit enable/disable
*       - scheduling algorithms, bandwidth management
*       - shaping on queues and the logical port
*       - drop algorithms for congestion handling
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortTx.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortPfc.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h>
#include <cpssCommon/private/prvCpssMath.h>

#define PRV_CPSS_DXCH_CPU_PORT_PROFILE_CNS       CPSS_PORT_TX_SCHEDULER_PROFILE_1_E
#define PRV_CPSS_DXCH_NET_GE_PORT_PROFILE_CNS    CPSS_PORT_TX_SCHEDULER_PROFILE_2_E
#define PRV_CPSS_DXCH_NET_10GE_PORT_PROFILE_CNS  CPSS_PORT_TX_SCHEDULER_PROFILE_3_E
#define PRV_CPSS_DXCH_CASCADING_PORT_PROFILE_CNS CPSS_PORT_TX_SCHEDULER_PROFILE_4_E

#define PRV_CPSS_DXCH_TC_DP_BUFFER_LIMIT_MAX_CNS                0xFFF
#define PRV_CPSS_DXCH_TC_DP_DESC_LIMIT_MAX_CNS                  0xFFF
#define PRV_CPSS_DXCH_TC_DP_PARAMS_LEN_CNS                      24
#define PRV_CPSS_DXCH_TC_DP_BUFFER_LIMIT_OFFSET_CNS             12
#define PRV_CPSS_DXCH_TC_DP_BUFFER_LIMIT_LEN_CNS                12
#define PRV_CPSS_DXCH_TC_DP_DESC_LIMIT_OFFSET_CNS               0
#define PRV_CPSS_DXCH_TC_DP_DESC_LIMIT_LEN_CNS                  12

#define PRV_CPSS_DXCH3_TC_DP_BUFFER_LIMIT_MAX_CNS               0x3FFF
#define PRV_CPSS_DXCH3_TC_DP_DESC_LIMIT_MAX_CNS                 0x3FFF
#define PRV_CPSS_DXCH3_TC_DP_PARAMS_LEN_CNS                     28
#define PRV_CPSS_DXCH3_TC_DP_BUFFER_LIMIT_OFFSET_CNS            14
#define PRV_CPSS_DXCH3_TC_DP_BUFFER_LIMIT_LEN_CNS               14
#define PRV_CPSS_DXCH3_TC_DP_DESC_LIMIT_OFFSET_CNS              0
#define PRV_CPSS_DXCH3_TC_DP_DESC_LIMIT_LEN_CNS                 14

#define PRV_CPSS_DXCH_INGRESS_MIRR_2_ANLY_DESC_MAX_CNS          0xFFF
#define PRV_CPSS_DXCH_EGRESS_MIRR_2_ANLY_DESC_MAX_CNS           0xFFF
#define PRV_CPSS_DXCH_INGRESS_MIRR_2_ANLY_DESC_MAX_OFFSET_CNS   12
#define PRV_CPSS_DXCH_MIRR_2_ANLY_DESC_MAX_PARAM_LEN_CNS        24

#define PRV_CPSS_DXCH3_INGRESS_MIRR_2_ANLY_DESC_MAX_CNS         0x3FFF
#define PRV_CPSS_DXCH3_EGRESS_MIRR_2_ANLY_DESC_MAX_CNS          0x3FFF
#define PRV_CPSS_DXCH3_INGRESS_MIRR_2_ANLY_DESC_MAX_OFFSET_CNS  14
#define PRV_CPSS_DXCH3_MIRR_2_ANLY_DESC_MAX_PARAM_LEN_CNS       28

#define PRV_CPSS_DXCH_MULTICAST_PCKTS_DESC_LIMIT_MAX_CNS        0x1F
#define PRV_CPSS_DXCH_TOKEN_BUCKET_TOKENS_OFFSET_CNS            4
#define PRV_CPSS_DXCH_TOKEN_BUCKET_TOKENS_LEN_CNS               12
#define PRV_CPSS_DXCH_TOKEN_BUCKET_SLOW_RATE_EN_OFFSET_CNS      1
#define PRV_CPSS_DXCH_TOKEN_BUCKET_SLOW_RATE_EN_LEN_CNS         1
#define PRV_CPSS_DXCH_TOKEN_BUCKET_EN_LEN_CNS                   1
#define PRV_CPSS_DXCH_TOKEN_BUCKET_RESERVED_LEN_CNS             2

#define PRV_CPSS_DXCH_XG_TOKEN_BCKT_UPD_RATE_MIN_CNS            1
#define PRV_CPSS_DXCH_XG_TOKEN_BCKT_UPD_RATE_MAX_CNS            15
#define PRV_CPSS_DXCH_GIG_TOKEN_BCKT_UPD_RATE_MIN_CNS           1
#define PRV_CPSS_DXCH_GIG_TOKEN_BCKT_UPD_RATE_MAX_CNS           15
#define PRV_CPSS_DXCH_GIG_SLOW_TOKEN_BCKT_UPD_RATIO_MIN_CNS     1
#define PRV_CPSS_DXCH_GIG_SLOW_TOKEN_BCKT_UPD_RATIO_MAX_CNS     16
#define PRV_CPSS_DXCH_XG_TOKEN_BCKT_UPD_RATE_OFFSET_CNS         26
#define PRV_CPSS_DXCH_XG_TOKEN_BCKT_UPD_RATE_LEN_CNS            4
#define PRV_CPSS_DXCH_GIG_TOKEN_BCKT_UPD_RATE_OFFSET_CNS        22
#define PRV_CPSS_DXCH_GIG_TOKEN_BCKT_UPD_RATE_LEN_CNS           4
#define PRV_CPSS_DXCH_GIG_SLOW_TOKEN_BCKT_UPD_RATIO_OFFSET_CNS  18
#define PRV_CPSS_DXCH_GIG_SLOW_TOKEN_BCKT_UPD_RATIO_LEN_CNS     4

#define PRV_CPSS_DXCH_TXQREV1_TOKEN_BCKT_UPD_RATE_MIN_CNS        1
#define PRV_CPSS_DXCH_TXQREV1_TOKEN_BCKT_UPD_RATE_MAX_CNS        15
#define PRV_CPSS_DXCH_TXQREV1_SLOW_TOKEN_BCKT_UPD_RATIO_MIN_CNS  1
#define PRV_CPSS_DXCH_TXQREV1_SLOW_TOKEN_BCKT_UPD_RATIO_MAX_CNS  16
#define PRV_CPSS_DXCH_TXQREV1_TOKENS_REFILL_MAX_CNS              0xFFF


#define PRV_CPSS_DXCH_PORT_REQUEST_MASK_INDEX_MAX_CNS           3

/*
 * typedef: struct PORT_TX_SHAPER_TOKEN_BUCKET_STC
 *
 * Description:  structure to hold port's shaper info (per port/tc)
 *
 * Enumerations:
 *         tokenBucketEn - token Bucket Enable
 *         slowRateEn    - slow Rate Enable
 *         tbUpdateRatio - token bucket update ratio
 *         tokens - number of tokens
 *         maxBucketSize - max Bucket Size
 *         currentBucketSize - current Bucket Size
 *
 * Comments:
 *          for TXQ version 1 and above
 */
typedef struct{               /* start..end  numBits*/
    GT_BOOL tokenBucketEn    ;/*   0           1    */
    GT_BOOL slowRateEn       ;/*   1           1    */
    GT_U32  tbUpdateRatio    ;/*   2..5        4    */
    GT_U32  tokens           ;/*   6..17       12   */
    GT_U32  maxBucketSize    ;/*   18..29      12   */
    GT_U32  currentBucketSize;/*   30..53      24   */
}PORT_TX_SHAPER_TOKEN_BUCKET_STC;

/* number of words in the shaper per port per TC entry */
#define TXQ_SHAPER_ENTRY_WORDS_NUM_CNS  14

/*
 * typedef: enum SHAPER_DISABLE_STAGE_ENT
 *
 * Description:  enum for stage in the disabling shaper operation
 *
 * Enumerations:
 *         SHAPER_DISABLE_STAGE_1_E - stage 1 -- until sleep , without sleep
 *         SHAPER_DISABLE_STAGE_2_E - stage 2 -- from sleep , without sleep
 *
 * Comments:
 *          for TXQ version 1 and above
 */
typedef enum{
    SHAPER_DISABLE_STAGE_1_E,
    SHAPER_DISABLE_STAGE_2_E
}SHAPER_DISABLE_STAGE_ENT;

/*
 * typedef: enum DP_SHARING_ENT
 *
 * Description:  enum for DP share buffers modes --
 *               for backwards compatible handling (with TXQ version 0)
 *
 * Enumerations:
 *         DP_SHARING_DP0_ENABLE_E - enable DP0
 *         DP_SHARING_DP0_DISABLE_E - disable DP0
 *         DP_SHARING_DP1_ENABLE_E - enable DP1
 *         DP_SHARING_DP1_DISABLE_E - disable DP1
 *
 * Comments:
 *          for TXQ version 1 and above
 */
typedef enum{
    DP_SHARING_DP0_ENABLE_E,
    DP_SHARING_DP0_DISABLE_E,
    DP_SHARING_DP1_ENABLE_E,
    DP_SHARING_DP1_DISABLE_E
}DP_SHARING_ENT;

static GT_STATUS prvDxChShaperDisable
(
    IN  GT_U8   devNum,
    IN  GT_U32  portGroupId,
    IN  GT_U32  tokenBucketRegAddr
);

static GT_STATUS prvDxChPortTxCalcShaperTokenBucketRate
(
    IN    GT_U8                   devNum,
    IN    GT_U8                   portNum,
    IN    GT_U32                  xgPortsTokensRate,
    IN    GT_U32                  gigPortsTokensRate,
    IN    GT_U32                  gigPortsSlowRateRatio,
    IN    GT_U32                  tokenRefillValue,
    IN    GT_U32                  slowRateEn,
    IN    GT_BOOL                 stackAsGig,
    OUT   GT_U32                 *tokenBucketRatePtr
);

static GT_STATUS prvDxChPortTxCalcShaperTokenBucketRate_rev1
(
    IN    GT_U8                   devNum,
    IN    GT_U8                   portNum,
    IN    CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC *shaperConfigPtr,
    IN    CPSS_PORT_TX_DROP_SHAPER_MODE_ENT     shaperMode,
    IN    GT_BOOL                 slowRateEn,
    IN    GT_U32                  tbUpdateRatio,
    IN    GT_U32                  tokenRefillValue,
    OUT   GT_U32                 *tokenBucketRatePtr

);

static GT_STATUS prvDxChPortTxSetShaperTokenBucketParams
(
    IN    GT_U8                   devNum,
    IN    GT_U8                   portNum,
    IN    GT_BOOL                 usePerTc,
    IN    GT_U32                  tcQueue,
    IN    GT_U32                  old_xgPortsTokensRate,
    IN    GT_U32                  old_gigPortsTokensRate,
    IN    GT_U32                  old_gigPortsSlowRateRatio,
    IN    GT_BOOL                 old_StackAsGig,
    IN    GT_U32                  xgPortsTokensRate,
    IN    GT_U32                  gigPortsTokensRate,
    IN    GT_U32                  gigPortsSlowRateRatio,
    IN    GT_BOOL                 stackAsGig
);

static GT_STATUS prvCpssDxChPortTxReCalcShaperTokenBucketRate
(
    IN    GT_U8                   devNum,
    IN    GT_U8                   portNum,
    IN    GT_U32                  xgPortsTokensRate,
    IN    GT_U32                  gigPortsTokensRate,
    IN    GT_U32                  gigPortsSlowRateRatio,
    IN    GT_BOOL                 stackAsGig,
    OUT   GT_U32                 *tokenRefillValuePtr,
    OUT   GT_U32                 *slowRateEnPtr,
    IN    GT_U32                  tokenBucketRate
);

static GT_STATUS prvCpssDxChPortTxReCalcShaperTokenBucketRate_rev1
(
    IN  GT_U8                                 devNum,
    IN  GT_U8                                 portNum,
    IN  GT_BOOL                               usePerTc,
    IN  CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC   *shaperConfigPtr,
    IN  CPSS_PORT_TX_DROP_SHAPER_MODE_ENT     shaperMode,
    IN  GT_U32                                tokenBucketRate,
    OUT GT_BOOL                               *slowRateEnPtr,
    OUT GT_U32                                *tbUpdateRatioPtr,
    OUT GT_U32                                *tokensPtr
);

GT_STATUS prvCpssDxChPortTxShaperTokenBucketEntryWrite_rev1
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_BOOL usePerTc,
    IN  GT_U32  tc,
    IN  GT_BOOL slowRateEn,
    IN  GT_U32  tbUpdateRatio,
    IN  GT_U32  tokens,
    IN  GT_U32  maxBucketSize,
    IN  GT_BOOL updCurrentBucketSize,
    IN  GT_U32  currentBucketSize
);

static GT_STATUS portTxShaperTokenBucketEnable_rev1
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_BOOL usePerTc,
    IN  GT_U32  tc,
    IN  GT_BOOL enable
);

static GT_STATUS portTxShaperTokenBucketAllShapersDisable_rev1
(
    IN  GT_U8    devNum
);

static GT_STATUS portTxShaperTokenBucketShapersDisable_rev1
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum
);

static GT_STATUS portTxShaperTokenBucketEntryRead_rev1
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_BOOL usePerTc,
    IN  GT_U32  tc,
    OUT PORT_TX_SHAPER_TOKEN_BUCKET_STC *subEntryPtr
);

static GT_STATUS prvCpssDxChPortTxShaperTokenBucketRateGet
(
    IN    GT_U8                   devNum,
    IN    GT_U8                   portNum,
    IN    GT_BOOL                 usePerTc,
    IN    GT_U32                  tcQueue,
    IN    GT_U32                  xgPortsTokensRate,
    IN    GT_U32                  gigPortsTokensRate,
    IN    GT_U32                  gigPortsSlowRateRatio,
    IN    GT_BOOL                 stackAsGig,
    OUT   GT_BOOL                *tokenBucketEnPtr,
    OUT   GT_U32                 *tokenRefillValuePtr,
    OUT   GT_U32                 *slowRateEnPtr,
    OUT   GT_U16                 *burstSizePtr,
    OUT   GT_U32                 *tokenBucketRatePtr
);

/* macro to get the msb which is set */
#define MSB_SET_GET_MAC(_msbNum , _mask)    \
    _msbNum = 0;                            \
    while( 0x0 != _mask )                   \
    {                                       \
        _msbNum++;                          \
        _mask = _mask >> 1;                 \
    }

/*******************************************************************************
* cpssDxChPortTxInit
*
* DESCRIPTION:
*       Init port Tx configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - physical device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In SALSA family the first profile (profile_0) is used for CPU port and
*       the second one (profile_1) for network ports.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxInit
(
    IN  GT_U8    devNum
)
{
    GT_U8                                   portNum;
    GT_STATUS                               rc;         /* return code */
    CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profile;
    GT_U8                                   tc;
    
	#ifndef __AX_PLATFORM__
    GT_U8	queueIdx = 0;
	CPSS_PORT_TX_Q_ARB_GROUP_ENT cpuArbGroup = CPSS_PORT_TX_WRR_ARB_GROUP_0_E;
	GT_U8	cpuWrrQueueWeight[8] = {4,4,3,3,2,2,1,1};
	#endif

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    for (portNum = 0; portNum < PRV_CPSS_PP_MAC(devNum)->numOfPorts; portNum++)
    {
        if (! PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, portNum))
            continue;

        /* Set the tail drop Profile association used for network ports */
        profile = (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) >= PRV_CPSS_PORT_XG_E) ?
            PRV_CPSS_DXCH_NET_10GE_PORT_PROFILE_CNS :
            PRV_CPSS_DXCH_NET_GE_PORT_PROFILE_CNS;

        rc = cpssDxChPortTxBindPortToDpSet(devNum,portNum,(CPSS_PORT_TX_DROP_PROFILE_SET_ENT)profile);
        if (rc != GT_OK)
            return rc;

        /* Set the scheduler TX Profile association used for   *
         * WRR algorithm for network ports                     */
        rc = cpssDxChPortTxBindPortToSchedulerProfileSet(devNum,portNum,profile);
        if (rc != GT_OK)
            return rc;
    }

    /* Set the tail drop Profile association used for   *
     *  CPU port                                        */
    portNum = (GT_U8) CPSS_CPU_PORT_NUM_CNS;
    profile = PRV_CPSS_DXCH_CPU_PORT_PROFILE_CNS;

    rc = cpssDxChPortTxBindPortToDpSet(devNum,portNum,(CPSS_PORT_TX_DROP_PROFILE_SET_ENT)profile);
    if (rc != GT_OK)
        return rc;

    /* Set the scheduler TX Profile association used for   *
     * WRR algorithm for CPU port                          */
    rc = cpssDxChPortTxBindPortToSchedulerProfileSet(devNum,portNum,profile);
    if (rc != GT_OK)
        return rc;

    /* set the initial value of shaper to port speed ratio to 100% */
    if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        for (profile = CPSS_PORT_TX_SCHEDULER_PROFILE_1_E;
              profile <= CPSS_PORT_TX_SCHEDULER_PROFILE_8_E;profile++)
        {
            for (tc = 0; tc < CPSS_TC_RANGE_CNS; tc++)
            {
                rc = cpssDxChPortPfcShaperToPortRateRatioSet(devNum,profile,tc,100);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }
		return GT_OK;   /* return for lion error in the following code, zhangdi@autelan.com 2011-11-29 */
    }
    #if 0   /* luoxun -- cpss1.3 */
    #ifndef __AX_PLATFORM__
	/* Set CPU port scheduler Tx Profile: All WRR group 0  by qinhs@autelan.com 11/08/2008 */
	for(queueIdx = 0; queueIdx < 8; queueIdx++) {
		rc = cpssDxChPortTxQWrrProfileSet(devNum, queueIdx, \
			 					cpuWrrQueueWeight[queueIdx],PRV_CPSS_DXCH_CPU_PORT_PROFILE);
		if(rc != GT_OK)
			return rc;
		
		rc = cpssDxChPortTxQArbGroupSet(devNum, queueIdx, cpuArbGroup,	\
											PRV_CPSS_DXCH_CPU_PORT_PROFILE);
		if(rc != GT_OK) 
			return rc;
	}
    #endif
    #endif
    #ifndef __AX_PLATFORM__
	/* Set CPU port scheduler Tx Profile: All WRR group 0  by qinhs@autelan.com 11/08/2008 */
	for(queueIdx = 0; queueIdx < 8; queueIdx++) {
		rc = cpssDxChPortTxQWrrProfileSet(devNum, queueIdx, \
			 					cpuWrrQueueWeight[queueIdx],PRV_CPSS_DXCH_CPU_PORT_PROFILE_CNS);
		if(rc != GT_OK)
			return rc;
		
		rc = cpssDxChPortTxQArbGroupSet(devNum, queueIdx, cpuArbGroup,	\
											PRV_CPSS_DXCH_CPU_PORT_PROFILE_CNS);
		if(rc != GT_OK) 
			return rc;
	}
    #endif

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxQueueEnableSet
*
* DESCRIPTION:
*       Enable/Disable enqueuing on all Tx queues on a specific device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - physical device number
*       enable - GT_TRUE, Global enqueuing enabled
*                GT_FALSE, Global enqueuing disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
#if 1    /* change the api to cpss3.4 2011-11-29 zhangdi@autelan.com */
GT_STATUS cpssDxChPortTxQueueEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
)
{
    GT_U32      regAddr;    /* register address             */
    GT_U32      value;      /* value to write into register */
    GT_STATUS	ret = GT_OK;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    value = BOOL2BIT_MAC(enable);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfig;
        ret = prvCpssDxChHwPpSetRegField(devNum, regAddr, 1, 1, value);
        if(GT_OK != ret) {
		    return ret;	
        }
        #ifndef __AX_PLATFORM__ /* by yinlm@autelan.com 10/13/2008 for QoS Reserved. Must be set to 1*/
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfigExt;
        ret = prvCpssDrvHwPpSetRegField(devNum, regAddr, 30, 2, 1);
        #endif
        return ret;		
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.egr.filterConfig.globalEnables;

        return prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 1, value);
    }

}
#else
/*luoxun --combination may be wrong, attention*/
GT_STATUS cpssDxChPortTxQueueEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
)
{
    GT_U32      regAddr;    /* register address             */
    GT_U32      value;      /* value to write into register */
    GT_STATUS	ret = GT_OK;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    value = BOOL2BIT_MAC(enable);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfig;
        ret = prvCpssDxChHwPpSetRegField(devNum, regAddr, 1, 1, value);
        if(GT_OK != ret) {
		    return ret;	
        }
        #ifndef __AX_PLATFORM__ /* by yinlm@autelan.com 10/13/2008 for QoS Reserved. Must be set to 1*/
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfigExt;
        ret = prvCpssDrvHwPpSetRegField(devNum, regAddr, 30, 2, 1);
        #endif
        return ret;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.egr.filterConfig.globalEnables;
        ret = prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 1, value);
        if(GT_OK != ret) {
		    return ret;	
        }
        #ifndef __AX_PLATFORM__ /* by yinlm@autelan.com 10/13/2008 for QoS Reserved. Must be set to 1*/
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfigExt;
        ret = prvCpssDrvHwPpSetRegField(devNum, regAddr, 30, 2, 1);
        #endif
        return ret;
    }

}
#endif
/*******************************************************************************
* cpssDxChPortTxQueueEnableGet
*
* DESCRIPTION:
*       Get status of enqueuing on all Tx queues on a specific device 
*       (enabled/disabled).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       enablePtr - GT_TRUE, Global enqueuing enabled
*                  GT_FALSE, Global enqueuing disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQueueEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr
)
{
    GT_U32      regAddr;    /* register address             */
    GT_U32      value;      /* value to write into register */
    GT_U32      bitOffset;  /* bit offset inside register   */
    GT_STATUS   rc;         /* return value                 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);


    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfig;
        bitOffset = 1;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.egr.filterConfig.globalEnables;
        bitOffset = 0;
    }

    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, bitOffset, 1, &value);
    if (rc != GT_OK) 
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;
}

/*******************************************************************************
* cpssDxChPortTxQueueingEnableSet
*
* DESCRIPTION:
*       Enable/Disable enqueuing to a Traffic Class queue
*       on the specified port of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical or CPU port number
*       tcQueue - traffic class queue on this device (0..7)
*       enable  - GT_TRUE, enable enqueuing to the queue
*                 GT_FALSE, disable enqueuing to the queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQueueingEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     tcQueue,
    IN  GT_BOOL   enable
)
{
    GT_U32      regAddr;    /* register address                 */
    GT_U32      value;      /* value to write into register     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* validate Traffic Classes queue */
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);

    value = BOOL2BIT_MAC(enable);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        PRV_CPSS_DXCH_PORT_TXQ_REG_MAC(devNum,portNum,&regAddr);

        return prvCpssDxChHwPpPortGroupSetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                                                  regAddr, tcQueue, 1, value);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.tailDrop.config.portEnqueueEnable[portNum];

        return prvCpssDxChHwPpSetRegField(devNum, regAddr, tcQueue, 1, value);
    }
}

/*******************************************************************************
* cpssDxChPortTxQueueingEnableGet
*
* DESCRIPTION:
*       Get the status of enqueuing to a Traffic Class queue
*       on the specified port of specified device (Enable/Disable).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical or CPU port number
*       tcQueue - traffic class queue on this device (0..7)
*
* OUTPUTS:
*       enablePtr  - GT_TRUE, enable enqueuing to the queue
*                    GT_FALSE, disable enqueuing to the queue
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQueueingEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     tcQueue,
    OUT GT_BOOL   *enablePtr
)
{
    GT_U32      regAddr;    /* register address                 */
    GT_U32      value;      /* value to write into register     */
    GT_STATUS   rc;         /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* validate Traffic Classes queue */
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);


    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        PRV_CPSS_DXCH_PORT_TXQ_REG_MAC(devNum,portNum,&regAddr);

        rc = prvCpssDxChHwPpPortGroupGetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                                                  regAddr, tcQueue, 1, &value);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.tailDrop.config.portEnqueueEnable[portNum];

        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, tcQueue, 1, &value);
    }
    if (rc != GT_OK) 
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxQueueTxEnableSet
*
* DESCRIPTION:
*       Enable/Disable transmission from a Traffic Class queue
*       on the specified port of specified device.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical or CPU port number
*       tcQueue - traffic class queue on this Physical Port (0..7)
*       enable  - GT_TRUE, enable transmission from the queue
*                GT_FALSE, disable transmission from the queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQueueTxEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     tcQueue,
    IN  GT_BOOL   enable
)
{
    GT_U32      value;      /* value to write into register     */
    GT_U32      regAddr;    /* register address                 */
    GT_U32      offset;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);

    value = BOOL2BIT_MAC(enable);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        PRV_CPSS_DXCH_PORT_TXQ_REG_MAC(devNum,portNum,&regAddr);

        offset = 8 + tcQueue;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.dq.shaper.portDequeueEnable[portNum];
        offset = tcQueue;
    }

    return prvCpssDxChHwPpPortGroupSetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, offset, 1, value);
}


/*******************************************************************************
* cpssDxChPortTxQueueTxEnableGet
*
* DESCRIPTION:
*       Get the status of transmission from a Traffic Class queue
*       on the specified port of specified device (Enable/Disable).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       tcQueue - traffic class queue on this Physical Port (0..7)
*
* OUTPUTS:
*       enablePtr  - GT_TRUE, enable transmission from the queue
*                    GT_FALSE, disable transmission from the queue
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQueueTxEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     tcQueue,
    OUT GT_BOOL   *enablePtr
)
{
    GT_U32      value;      /* value to write into register     */
    GT_U32      regAddr;    /* register address                 */
    GT_U32      offset;
    GT_STATUS   rc;         /* return code                      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);


    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        PRV_CPSS_DXCH_PORT_TXQ_REG_MAC(devNum,portNum,&regAddr);

        offset = 8 + tcQueue;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.dq.shaper.portDequeueEnable[portNum];
        offset = tcQueue;
    }

    rc = prvCpssDxChHwPpPortGroupGetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, offset, 1, &value);
    if (rc != GT_OK) 
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxFlushQueuesSet
*
* DESCRIPTION:
*       Flush all the traffic class queues on the specified port of
*       specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - physical device number
*       portNum- physical or CPU port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxFlushQueuesSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      offset;     /* field offset     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        PRV_CPSS_DXCH_PORT_TXQ_REG_MAC(devNum,portNum,&regAddr);
        offset = 21;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.
            global.flushTrig.portTxqFlushTrigger[portNum];
        offset = 0;
    }

    return prvCpssDxChHwPpPortGroupSetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, offset, 1, 1);
}

/*******************************************************************************
* cpssDxChPortTxShaperEnableSet
*
* DESCRIPTION:
*       Enable/Disable Token Bucket rate shaping on specified port of
*       specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical or CPU port number
*       enable  - GT_TRUE, enable Shaping
*                 GT_FALSE, disable Shaping
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
{
    GT_U32      regAddr1;    /* register address for token bucket*/
    GT_U32      portGroupId; /*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_LION_TB_NOT_SUPPORTED_WA_E))
    {
        return GT_NOT_SUPPORTED;
    }

    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        PRV_CPSS_DXCH_PORT_TOKEN_BUCKET_REG_MAC(devNum,portNum,&regAddr1);

        if (GT_FALSE == enable)
        {
            return prvDxChShaperDisable(devNum,
                                        portGroupId,
                                        regAddr1);
        }
        else
        {
            return prvCpssDxChHwPpPortGroupSetRegField(devNum,
                                                      portGroupId,
                                                      regAddr1, 0, 1, 1);
        }
    }
    else
    {
        return portTxShaperTokenBucketEnable_rev1(devNum,portNum,GT_FALSE,0,enable);
    }

}

/*******************************************************************************
* cpssDxChPortTxShaperEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable Token Bucket rate shaping status on specified port of
*       specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical or CPU port number
*
* OUTPUTS:
*       enablePtr - Pointer to Token Bucket rate shaping status.
*                   - GT_TRUE, enable Shaping
*                   - GT_FALSE, disable Shaping
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT  GT_BOOL  *enablePtr
)
{
    GT_U32      regAddr;    /* register address for token bucket*/
    GT_U32      value;      /* register field value */
    GT_STATUS   rc;         /* return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_LION_TB_NOT_SUPPORTED_WA_E))
    {
        return GT_NOT_SUPPORTED;
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        PRV_CPSS_DXCH_PORT_TOKEN_BUCKET_REG_MAC(devNum,portNum,&regAddr);

        rc = prvCpssDxChHwPpPortGroupGetRegField(devNum,
                PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                regAddr, 0, 1, &value);
        if(rc != GT_OK)
        {
            return rc;
        }

        *enablePtr = BIT2BOOL_MAC(value);
    }
    else
    {
        PORT_TX_SHAPER_TOKEN_BUCKET_STC subEntry;

        /* read entry from HW */
        rc = portTxShaperTokenBucketEntryRead_rev1(devNum,portNum,GT_FALSE,0,&subEntry);
        if(rc != GT_OK)
        {
            return rc;
        }
        *enablePtr = subEntry.tokenBucketEn;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortShaperProfileSet
*
* DESCRIPTION:
*       Set Shaper Profile for Traffic Class Queue of specified port /
*       specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*       tcQueue    - traffic class queue on the port
*                    CPSS_PARAM_NOT_USED_CNS - to get per port Shaper Profile.
*       burstSize  - burst size in units of 4K bytes
*                    (max value is 4K which results in 16K burst size)
*       maxRatePtr - Requested Rate in Kbps
*
* OUTPUTS:
*       maxRatePtr   - (pointer to) the actual Rate value in Kbps.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number, device, tcQueue
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortShaperProfileSet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    IN  GT_U32   tcQueue,
    IN  GT_U16   burstSize,
    INOUT GT_U32 *maxRatePtr
)
{
    GT_STATUS   rc;                     /* return value */
    GT_U32      regData;                /* value to read from register */
    GT_U32      regAddr = 0;            /* register address            */
    GT_U32      xgPortsTokensRate;      /* XG token rate */
    GT_U32      gigPortsTokensRate;     /* GE token rate */
    GT_U32      gigPortsSlowRateRatio;  /* GE Slow rate  */
    GT_U32      slowRateEn;   /* 1 - Slow rate is enabled, 0 - Slow rate is disabled */
    GT_U32      tokens;       /* number of tokens that are added to the bucket */
    GT_BOOL     usePerTc;/* do we set 'per port' or 'per port per tc'*/
    GT_BOOL     stackAsGig = GT_FALSE;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(maxRatePtr);

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_LION_TB_NOT_SUPPORTED_WA_E))
    {
        return GT_NOT_SUPPORTED;
    }

    /* check burst size validity - 12 bits in HW */
    if(burstSize >= BIT_12)
    {
        return GT_BAD_PARAM;
    }

    if(tcQueue == CPSS_PARAM_NOT_USED_CNS)
    {
        usePerTc = GT_FALSE;
    }
    else
    {
        /* validate Traffic Classes queue */
        PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);
        usePerTc = GT_TRUE;
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* Get port Token bucket configuration register */
        if(usePerTc == GT_FALSE)
        {
            PRV_CPSS_DXCH_PORT_TOKEN_BUCKET_REG_MAC(devNum, portNum, &regAddr);
        }
        /* Get Queue -- Token bucket configuration register. */
        else
        {
            PRV_CPSS_DXCH_PORT_Q_TOKEN_BUCKET_REG_MAC(devNum, portNum, tcQueue, &regAddr);
        }

        rc = cpssDxChPortTxShaperGlobalParamsGet(devNum,
                                             &xgPortsTokensRate,
                                             &gigPortsTokensRate,
                                             &gigPortsSlowRateRatio);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* check if StackAsGig mode enabled */
        if ((PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(devNum)) || 
            (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            rc = cpssDxChPortTxShaperOnStackAsGigEnableGet(devNum,&stackAsGig);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        rc = prvCpssDxChPortTxReCalcShaperTokenBucketRate(devNum,
                                                          portNum,
                                                          xgPortsTokensRate,
                                                          gigPortsTokensRate,
                                                          gigPortsSlowRateRatio,
                                                          stackAsGig,
                                                          &tokens,
                                                          &slowRateEn,
                                                          *maxRatePtr);

        if(rc != GT_OK)
        {
            return rc;
        }

        regData = slowRateEn | ((tokens & 0xFFF) << 3) | ((burstSize & 0xFFF) << 15);

        rc = prvCpssDxChHwPpPortGroupSetRegField(devNum,
                PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                regAddr, 1, 27, regData);
        if(rc != GT_OK)
        {
            return rc;
        }

        return prvDxChPortTxCalcShaperTokenBucketRate(devNum,
                                                  portNum,
                                                  xgPortsTokensRate,
                                                  gigPortsTokensRate,
                                                  gigPortsSlowRateRatio,
                                                  tokens,
                                                  slowRateEn,
                                                  stackAsGig,
                                                  maxRatePtr);
    }
    else
    {
        CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC shaperConfig; /* global shaper configurations */
        CPSS_PORT_TX_DROP_SHAPER_MODE_ENT   shaperMode;   /* shaper mode on port */
        GT_U32 tbUpdateRatio;    /* TB interval update ratio value */
        GT_BOOL slowRateEnable;  /* slow rate enable value */

        /* get global shaper configurations */
        rc = cpssDxChPortTxShaperConfigurationGet(devNum,&shaperConfig);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* get shaper mode on port */
        rc = cpssDxChPortTxShaperModeGet(devNum,portNum,&shaperMode);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* calulate per TB configurations */
        rc = prvCpssDxChPortTxReCalcShaperTokenBucketRate_rev1(devNum,
                                                               portNum,
                                                               usePerTc,
                                                               &shaperConfig,
                                                               shaperMode,
                                                               *maxRatePtr,
                                                               &slowRateEnable,
                                                               &tbUpdateRatio,
                                                               &tokens);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* update TB entry */
        /* set the current current Bucket Size to max Bucket Size */
        rc = prvCpssDxChPortTxShaperTokenBucketEntryWrite_rev1(devNum,
                                                               portNum,
                                                               usePerTc,
                                                               tcQueue,
                                                               slowRateEnable,
                                                               tbUpdateRatio,
                                                               tokens,
                                                               burstSize,
                                                               GT_TRUE,
                                                               burstSize * _4KB);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* calulate the actual Rate value */
        return prvDxChPortTxCalcShaperTokenBucketRate_rev1(devNum,
                                                           portNum,
                                                           &shaperConfig,
                                                           shaperMode,
                                                           slowRateEnable,
                                                           tbUpdateRatio,
                                                           tokens,
                                                           maxRatePtr);
    }
}
/*******************************************************************************
* prvCpssDxChPortShaperProfileGet
*
* DESCRIPTION:
*       Get Shaper Profile for Traffic Class Queue of specified port /
*       specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical or CPU port number
*       tcQueue    - traffic class queue on the port
*                    CPSS_PARAM_NOT_USED_CNS - to get per port Shaper Profile.
*
* OUTPUTS:
*       burstSizePtr - (pointer to) burst size in units of 4K bytes
*       maxRatePtr   - (pointer to) the actual Rate value in Kbps.
*       enablePtr    - (pointer to) Token Bucket rate shaping status.
*                           - GT_TRUE - Token Bucket rate shaping is enabled.
*                           - GT_FALSE - Token Bucket rate shaping is disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number, device, tcQueue
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortShaperProfileGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    IN  GT_U32   tcQueue,
    OUT GT_U16   *burstSizePtr,
    OUT GT_U32   *maxRatePtr,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS   rc;            /* return value */
    GT_U32      xgPortsTokensRate;      /* XG token rate */
    GT_U32      gigPortsTokensRate;     /* GE token rate */
    GT_U32      gigPortsSlowRateRatio;  /* GE Slow rate  */
    GT_U32      tokens;        /* number of tokens that are added to the bucket */
    GT_U32      slowRateEn;    /* 1 - Slow rate is enabled, 0 - Slow rate is disabled */
    PORT_TX_SHAPER_TOKEN_BUCKET_STC subEntry;/* sub entry format - relevant for TXQ_REV_1 */
    GT_BOOL     usePerTc;/* do we set 'per port' or 'per port per tc'*/
    CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC shaperConfig; /* shaper global configuration */
    CPSS_PORT_TX_DROP_SHAPER_MODE_ENT   shaperMode;   /* shaper mode on port */
    GT_BOOL stackAsGig = GT_FALSE;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(burstSizePtr);
    CPSS_NULL_PTR_CHECK_MAC(maxRatePtr);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_LION_TB_NOT_SUPPORTED_WA_E))
    {
        return GT_NOT_SUPPORTED;
    }

    if(tcQueue == CPSS_PARAM_NOT_USED_CNS)
    {
        usePerTc = GT_FALSE;
    }
    else
    {
        /* validate Traffic Classes queue */
        PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);
        usePerTc = GT_TRUE;
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {

        rc = cpssDxChPortTxShaperGlobalParamsGet(devNum,
                                                 &xgPortsTokensRate,
                                                 &gigPortsTokensRate,
                                                 &gigPortsSlowRateRatio);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* check if StackAsGig mode enabled */
        if ((PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(devNum)) || 
            (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            rc = cpssDxChPortTxShaperOnStackAsGigEnableGet(devNum,&stackAsGig);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        return prvCpssDxChPortTxShaperTokenBucketRateGet(devNum,portNum,usePerTc,tcQueue,
                                                         xgPortsTokensRate,
                                                         gigPortsTokensRate,
                                                         gigPortsSlowRateRatio,
                                                         stackAsGig,
                                                         enablePtr,
                                                         &tokens,
                                                         &slowRateEn,
                                                         burstSizePtr,
                                                         maxRatePtr);
    }
    else
    {
        /* read the entry */
        rc = portTxShaperTokenBucketEntryRead_rev1(devNum,portNum,usePerTc,tcQueue,&subEntry);
        if(rc != GT_OK)
        {
            return rc;
        }

        *enablePtr = subEntry.tokenBucketEn;
        /* Token Bucket rate shaping is disabled. */
        if(*enablePtr == GT_FALSE)
        {
            return GT_OK;
        }
        /* Token Bucket rate shaping is enabled. */
        *burstSizePtr = (GT_U16)subEntry.maxBucketSize;

        /* get global shaper configurations */
        rc = cpssDxChPortTxShaperConfigurationGet(devNum,&shaperConfig);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortTxShaperModeGet(devNum,portNum,&shaperMode);
        if (rc != GT_OK)
        {
            return rc;
        }

        return prvDxChPortTxCalcShaperTokenBucketRate_rev1(devNum,
                                                           portNum,
                                                          &shaperConfig,
                                                          shaperMode,
                                                          subEntry.slowRateEn,
                                                          subEntry.tbUpdateRatio,
                                                          subEntry.tokens,
                                                          maxRatePtr);
    }
}

/*******************************************************************************
* cpssDxChPortTxShaperProfileSet
*
* DESCRIPTION:
*       Set Token Bucket Shaper Profile on specified port of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical or CPU port number
*       burstSize  - burst size in units of 4K bytes
*                    (max value is 4K which results in 16M burst size)
*       maxRatePtr - Requested Rate in Kbps or packets per second
*                    according to port shaper mode.
*                    Packet per second relevant only for Lion and above.
*
* OUTPUTS:
*       maxRatePtr   - (pointer to) the actual Rate value in Kbps
*                      or packets per second.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. For DxCh1, DxCh2, DxCh3, xCat and xCat2:
*       For port number in the range 0..23, whether G or XG type, the global
*       shaper used parameters are those referenced as gig ones -
*       "gigPortsTokensRate" and "gigPortsSlowRateRatio" inputs for
*       "cpssDxChPortTxShaperGlobalParamsSet" API.
*       2. For xCat-A3 and xCat2:
*       For port number in the range 24..27, whether G or XG type, the global
*       shaper used parameters are determined by
*       "cpssDxChPortTxShaperOnStackAsGigEnableSet".
*       3. For Lion:
*       If the shaper mode on a given port is packet based and the packet
*       length (portsPacketLength/cpuPacketLength) configuration isn't null,
*       maxRatePtr units are packets per second,
*       otherwise maxRatePtr units are Kbps.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperProfileSet
(
    IN    GT_U8     devNum,
    IN    GT_U8     portNum,
    IN    GT_U16    burstSize,
    INOUT GT_U32    *maxRatePtr
)
{
    return prvCpssDxChPortShaperProfileSet(devNum, portNum,
                                           CPSS_PARAM_NOT_USED_CNS,
                                           burstSize, maxRatePtr);
}

/*******************************************************************************
* cpssDxChPortTxShaperProfileGet
*
* DESCRIPTION:
*       Get Token Bucket Shaper Profile on specified port of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical or CPU port number
*
* OUTPUTS:
*       burstSizePtr - (pointer to) burst size in units of 4K bytes
*       maxRatePtr   - (pointer to) the actual Rate value in Kbps
*                                   or packets per second.
*       enablePtr    - (pointer to) Token Bucket rate shaping status.
*                           - GT_TRUE - Token Bucket rate shaping is enabled.
*                           - GT_FALSE - Token Bucket rate shaping is disabled.
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. For Lion:
*          If the shaper mode on a given port is packet based and the packet
*          length (portsPacketLength/cpuPacketLength) configuration isn't null,
*          maxRatePtr units are packets per second,
*          otherwise maxRatePtr units are Kbps.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperProfileGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_U16  *burstSizePtr,
    OUT GT_U32  *maxRatePtr,
    OUT GT_BOOL *enablePtr
)
{
    return prvCpssDxChPortShaperProfileGet(devNum, portNum,
                                           CPSS_PARAM_NOT_USED_CNS,
                                           burstSizePtr, maxRatePtr, enablePtr);
}

/*******************************************************************************
* cpssDxChPortTxQShaperEnableSet
*
* DESCRIPTION:
*       Enable/Disable shaping of transmitted traffic from a specified Traffic
*       Class Queue and specified port of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical or CPU port number
*       tcQueue - traffic class queue on this port (0..7)
*       enable  - GT_TRUE, enable shaping on this traffic queue
*                 GT_FALSE, disable shaping on this traffic queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQShaperEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     tcQueue,
    IN  GT_BOOL   enable
)
{
    GT_U32      regAddr1;    /* register address for token bucket*/
    GT_U32      portGroupId; /*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_LION_TB_NOT_SUPPORTED_WA_E))
    {
        return GT_NOT_SUPPORTED;
    }

    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        PRV_CPSS_DXCH_PORT_Q_TOKEN_BUCKET_REG_MAC(devNum,portNum,tcQueue,&regAddr1);

        if (GT_FALSE == enable)
        {
            return prvDxChShaperDisable(devNum,
                                        portGroupId,
                                        regAddr1);
        }
        else
        {
            return prvCpssDxChHwPpPortGroupSetRegField(devNum,
                                                      portGroupId,
                                                      regAddr1, 0, 1, 1);
        }
    }
    else
    {
        return portTxShaperTokenBucketEnable_rev1(devNum,portNum,GT_TRUE,tcQueue,enable);
    }
}

/*******************************************************************************
* cpssDxChPortTxQShaperEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable shaping status
*       of transmitted traffic from a specified Traffic
*       Class Queue and specified port of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical or CPU port number
*       tcQueue - traffic class queue on this port (0..7)
*
* OUTPUTS:
*       enablePtr  - Pointer to  shaping status
*                    - GT_TRUE, enable shaping on this traffic queue
*                    - GT_FALSE, disable shaping on this traffic queue
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQShaperEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     tcQueue,
    OUT GT_BOOL   *enablePtr
)
{
    GT_U32      regAddr;    /* register address for token bucket*/
    GT_U32      value;      /* register field value */
    GT_STATUS   rc;         /* return value */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_LION_TB_NOT_SUPPORTED_WA_E))
    {
        return GT_NOT_SUPPORTED;
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        PRV_CPSS_DXCH_PORT_Q_TOKEN_BUCKET_REG_MAC(devNum,portNum,tcQueue,&regAddr);

        rc = prvCpssDxChHwPpPortGroupGetRegField(devNum,
                PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                regAddr, 0, 1, &value);
        if(rc != GT_OK)
        {
            return rc;
        }

        *enablePtr = BIT2BOOL_MAC(value);
    }
    else
    {
        PORT_TX_SHAPER_TOKEN_BUCKET_STC subEntry;

        /* read entry from HW */
        rc = portTxShaperTokenBucketEntryRead_rev1(devNum,portNum,GT_TRUE,tcQueue,&subEntry);
        if(rc != GT_OK)
        {
            return rc;
        }
        *enablePtr = subEntry.tokenBucketEn;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxQShaperProfileSet
*
* DESCRIPTION:
*       Set Shaper Profile for Traffic Class Queue of specified port on
*       specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - physical device number
*       portNum      - physical or CPU port number
*       tcQueue      - traffic class queue on this Logical Port
*       burstSize    - burst size in units of 4K bytes
*                      (max value is 4K which results in 16M burst size)
*       maxRatePtr   - Requested Rate in Kbps or packets per second.
*
* OUTPUTS:
*       maxRatePtr   - (pointer to) the actual Rate value in Kbps or
*                       or packets per second.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. For DxCh1, DxCh2, DxCh3, xCat and xCat2:
*       For port number in the range 0..23, whether G or XG type, the global
*       shaper used parameters are those referenced as gig ones -
*       "gigPortsTokensRate" and "gigPortsSlowRateRatio" inputs for
*       "cpssDxChPortTxShaperGlobalParamsSet" API.
*       2. For xCat-A3 and xCat2:
*       For port number in the range 24..27, whether G or XG type, the global
*       shaper used parameters are determined by
*       "cpssDxChPortTxShaperOnStackAsGigEnableSet".
*       3. For Lion:
*       If the shaper mode on a given port is packet based and the packet
*       length (portsPacketLength/cpuPacketLength) configuration isn't null,
*       maxRatePtr units are packets per second,
*       otherwise maxRatePtr units are Kbps.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQShaperProfileSet
(
    IN    GT_U8     devNum,
    IN    GT_U8     portNum,
    IN    GT_U8     tcQueue,
    IN    GT_U16    burstSize,
    INOUT GT_U32    *maxRatePtr
)
{
    return prvCpssDxChPortShaperProfileSet(devNum, portNum, tcQueue,
                                           burstSize, maxRatePtr);
}

/*******************************************************************************
* cpssDxChPortTxQShaperProfileGet
*
* DESCRIPTION:
*       Get Shaper Profile for Traffic Class Queue of specified port on
*       specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical or CPU port number
*       tcQueue    - traffic class queue on the port
*
* OUTPUTS:
*       burstSizePtr - (pointer to) burst size in units of 4K bytes
*       maxRatePtr   - (pointer to) the actual Rate value in Kbps
*                                   or packets per second.
*       enablePtr    - (pointer to) Token Bucket rate shaping status.
*                           - GT_TRUE - Token Bucket rate shaping is enabled.
*                           - GT_FALSE - Token Bucket rate shaping is disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. For Lion:
*          If the shaper mode on a given port is packet based and the packet
*          length (portsPacketLength/cpuPacketLength) configuration isn't null,
*          maxRatePtr units are packets per second,
*          otherwise maxRatePtr units are Kbps.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQShaperProfileGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    IN  GT_U8    tcQueue,
    OUT GT_U16   *burstSizePtr,
    OUT GT_U32   *maxRatePtr,
    OUT GT_BOOL  *enablePtr
)
{
    return prvCpssDxChPortShaperProfileGet(devNum, portNum, tcQueue,
                                           burstSizePtr, maxRatePtr, enablePtr);
}

/*******************************************************************************
* cpssDxChPortTxBindPortToSchedulerProfileSet
*
* DESCRIPTION:
*       Bind a port to scheduler profile set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical or CPU port number
*       profileSet - The Profile Set in which the scheduler's parameters are
*                    associated.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxBindPortToSchedulerProfileSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U8                                   portNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet
)
{
    GT_U32  regAddr;        /* register address */
    GT_U32  fieldOffset;    /* the start bit number in the register         */
    GT_U32  fieldLength;    /* the number of bits to be written to register */
    GT_U32  hwProfileId;    /* HW profile ID */
    GT_STATUS rc;           /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_SCHEDULER_PROFILE_CHECK_MAC(profileSet, devNum);

    hwProfileId = (GT_U32)profileSet;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        fieldOffset = 16;
        fieldLength = 2;

        PRV_CPSS_DXCH_PORT_TXQ_REG_MAC(devNum,portNum,&regAddr);

        /* Set the scheduler TX Profile 2 LSB association used for   *
         * WRR algorithm for network ports                          */
        rc = prvCpssDxChHwPpPortGroupSetRegField(devNum,PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                                         regAddr,
                                         fieldOffset,
                                         fieldLength,
                                         hwProfileId);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* DxCh2 and above supports 8 profiles */
        if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            /* set MSB */
            hwProfileId >>= 2;
            rc = prvCpssDxChHwPpPortGroupSetRegField(devNum,PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                                           regAddr,
                                           24,
                                           1,
                                           hwProfileId);
        }
    }
    else
    {
        if (portNum == CPSS_CPU_PORT_NUM_CNS)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.
                scheduler.priorityArbiterWeights.portCpuSchedulerProfile;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.
                scheduler.priorityArbiterWeights.portSchedulerProfile[portNum];
        }


        rc = prvCpssDxChHwPpPortGroupSetRegField(devNum,
                    PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                    regAddr,0,3,hwProfileId);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChPortTxBindPortToSchedulerProfileGet
*
* DESCRIPTION:
*       Get scheduler profile set that is binded to the port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical or CPU port number
*
* OUTPUTS:
*       profileSetPtr - The Profile Set in which the scheduler's parameters are
*                        associated.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxBindPortToSchedulerProfileGet
(
    IN  GT_U8                                   devNum,
    IN  GT_U8                                   portNum,
    OUT CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  *profileSetPtr
)
{
    GT_U32    regAddr;        /* register address */
    GT_U32    fieldOffset;    /* the start bit number in the register         */
    GT_U32    fieldLength;    /* the number of bits to be written to register */
    GT_U32    hwProfileId;    /* HW profile ID */
    GT_STATUS rc;             /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(profileSetPtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        fieldOffset = 16;
        fieldLength = 2;

        PRV_CPSS_DXCH_PORT_TXQ_REG_MAC(devNum,portNum,&regAddr);

        /* Get the scheduler TX Profile LBSs association used for   *
         * WRR algorithm for network ports                     */
        rc = prvCpssDxChHwPpPortGroupGetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                        regAddr,fieldOffset, fieldLength, &hwProfileId);

        if(rc != GT_OK)
        {
            return rc;
        }

        *profileSetPtr = (CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT)hwProfileId;

        /* DxCh2 and above supports 8 profiles */
        if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            /* get MSB */
            rc = prvCpssDxChHwPpPortGroupGetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                            regAddr,24, 1, &hwProfileId);

            hwProfileId <<= 2;

            *profileSetPtr = (*profileSetPtr) |
                             (CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT)hwProfileId;
        }
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.
            scheduler.priorityArbiterWeights.portSchedulerProfile[portNum];

        rc = prvCpssDxChHwPpPortGroupGetRegField(devNum,
                    PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                    regAddr,0,3,&hwProfileId);

        *profileSetPtr = (CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT)hwProfileId;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChPortTx4TcTailDropProfileSet
*
* DESCRIPTION:
*       Set tail drop profiles limits for particular TC.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum             - physical device number
*       profileSet         - the Profile Set in which the Traffic
*                            Class Drop Parameters is associated.
*       trafficClass       - the Traffic Class associated with this set of
*                            Drop Parameters (0..7).
*       tailDropProfileParamsPtr -
*                       the Drop Profile Parameters to associate
*                       with the Traffic Class in this Profile set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, profile set or
*                                  traffic class
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTx4TcTailDropProfileSet
(
    IN    GT_U8                                   devNum,
    IN    CPSS_PORT_TX_DROP_PROFILE_SET_ENT       profileSet,
    IN    GT_U8                                   trafficClass,
    IN    CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS *tailDropProfileParamsPtr
)
{
    GT_U32      regAddr;    /* register address                         */
    GT_U32      buffOffset; /* buffer field offset                      */
    GT_U32      totalLen;   /* total buffer & descriptor len            */
    GT_U32      regData;    /* data to write into register              */
    GT_U32      maxDescVal; /* the maximum allowed descriptors value    */
    GT_U32      maxBuffVal; /* the maximum allowed buffers value        */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(tailDropProfileParamsPtr);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(trafficClass);
    PRV_CPSS_DXCH_DROP_PROFILE_CHECK_MAC(profileSet, devNum);

    /* CH & CH2 */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
    {
       maxDescVal = PRV_CPSS_DXCH_TC_DP_DESC_LIMIT_MAX_CNS;
       maxBuffVal = PRV_CPSS_DXCH_TC_DP_BUFFER_LIMIT_MAX_CNS;
       buffOffset = PRV_CPSS_DXCH_TC_DP_BUFFER_LIMIT_OFFSET_CNS;
       totalLen   = PRV_CPSS_DXCH_TC_DP_PARAMS_LEN_CNS;
    }
    else /* CH3 and above */
    {
       maxDescVal = PRV_CPSS_DXCH3_TC_DP_DESC_LIMIT_MAX_CNS;
       maxBuffVal = PRV_CPSS_DXCH3_TC_DP_BUFFER_LIMIT_MAX_CNS;
       buffOffset = PRV_CPSS_DXCH3_TC_DP_BUFFER_LIMIT_OFFSET_CNS;
       totalLen   = PRV_CPSS_DXCH3_TC_DP_PARAMS_LEN_CNS;
    }

    if ( (tailDropProfileParamsPtr->dp0MaxBuffNum  > maxBuffVal) ||
         (tailDropProfileParamsPtr->dp0MaxDescrNum > maxDescVal) ||
         (tailDropProfileParamsPtr->dp2MaxBuffNum  > maxBuffVal) ||
         (tailDropProfileParamsPtr->dp2MaxDescrNum > maxDescVal) )
    {
        return GT_OUT_OF_RANGE;
    }


    if( 0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum) )
    {
        /* Set DP0 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                           egrTxQConf.setsConfigRegs.tcDp0Red[trafficClass][profileSet];

        regData = tailDropProfileParamsPtr->dp0MaxDescrNum |
                                        (tailDropProfileParamsPtr->dp0MaxBuffNum << buffOffset);

        if (prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, totalLen, regData) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        /* Set DP1 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                           egrTxQConf.setsConfigRegs.tcDp1Red[trafficClass][profileSet];

        regData = tailDropProfileParamsPtr->dp2MaxDescrNum |
                                        (tailDropProfileParamsPtr->dp2MaxBuffNum << buffOffset);

        return prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, totalLen, regData);
    }
    else
    {
        if ( (tailDropProfileParamsPtr->dp1MaxBuffNum  > maxBuffVal) ||
             (tailDropProfileParamsPtr->dp1MaxDescrNum > maxDescVal) ||
             (tailDropProfileParamsPtr->tcMaxBuffNum  > maxBuffVal) ||
             (tailDropProfileParamsPtr->tcMaxDescrNum > maxDescVal) )
        {
            return GT_OUT_OF_RANGE;
        }

        /* Set DP0 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.tailDrop.limits.eqQueueLimitsDp0[profileSet][trafficClass];

        regData = tailDropProfileParamsPtr->dp0MaxDescrNum |
                                        (tailDropProfileParamsPtr->dp0MaxBuffNum << buffOffset);

        if (prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, totalLen, regData) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.tailDrop.limits.dqQueueDescLimits[profileSet][trafficClass];

        regData = tailDropProfileParamsPtr->dp0MaxDescrNum;

        if (prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 14, regData) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.tailDrop.limits.dqQueueBufLimits[profileSet][trafficClass];

        regData = tailDropProfileParamsPtr->dp0MaxBuffNum;

        if (prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 14, regData) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        /* Set DP1 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.tailDrop.limits.eqQueueLimitsDp1[profileSet][trafficClass];

        regData = tailDropProfileParamsPtr->dp1MaxDescrNum |
                                        (tailDropProfileParamsPtr->dp1MaxBuffNum << buffOffset);

        if (prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, totalLen, regData) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        /* Set DP2 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.tailDrop.limits.eqQueueLimitsDp2[profileSet][trafficClass];

        regData = tailDropProfileParamsPtr->dp2MaxDescrNum |
                                        (tailDropProfileParamsPtr->dp2MaxBuffNum << buffOffset);

        if (prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, totalLen, regData) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        /* Set TC */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.tailDrop.limits.maximumQueueLimits[profileSet][trafficClass];

        regData = tailDropProfileParamsPtr->tcMaxDescrNum |
                                        (tailDropProfileParamsPtr->tcMaxBuffNum << buffOffset);

        return prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, totalLen, regData);
    }
}

/*******************************************************************************
* cpssDxChPortTx4TcTailDropProfileGet
*
* DESCRIPTION:
*       Get tail drop profiles limits for particular TC.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum             - physical device number
*       profileSet         - the Profile Set in which the Traffic
*                            Class Drop Parameters is associated.
*       trafficClass       - the Traffic Class associated with this set of
*                            Drop Parameters (0..7).
*
* OUTPUTS:
*       tailDropProfileParamsPtr - Pointer to
*                       the Drop Profile Parameters to associate
*                       with the Traffic Class in this Profile.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, profile set or
*                                  traffic class
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTx4TcTailDropProfileGet
(
    IN    GT_U8                                   devNum,
    IN    CPSS_PORT_TX_DROP_PROFILE_SET_ENT       profileSet,
    IN    GT_U8                                   trafficClass,
    OUT   CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS *tailDropProfileParamsPtr
)
{
    GT_U32      regAddr;    /* register address                         */
    GT_U32      buffOffset; /* buffer field offset                      */
    GT_U32      buffLen;    /* buffer field length                      */
    GT_U32      descLen;    /* descriptor field len                     */
    GT_U32      totalLen;   /* total buffer & descriptor len            */
    GT_U32      regData;    /* data to write into register              */
    GT_STATUS   rc;         /* return value                             */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(tailDropProfileParamsPtr);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(trafficClass);
    PRV_CPSS_DXCH_DROP_PROFILE_CHECK_MAC(profileSet, devNum);

    /* CH & CH2 */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
    {
       buffOffset = PRV_CPSS_DXCH_TC_DP_BUFFER_LIMIT_OFFSET_CNS;
       buffLen    = PRV_CPSS_DXCH_TC_DP_BUFFER_LIMIT_LEN_CNS;
       descLen    = PRV_CPSS_DXCH_TC_DP_DESC_LIMIT_LEN_CNS;
       totalLen   = PRV_CPSS_DXCH_TC_DP_PARAMS_LEN_CNS;
    }
    else /* CH3 and above */
    {
       buffOffset = PRV_CPSS_DXCH3_TC_DP_BUFFER_LIMIT_OFFSET_CNS;
       buffLen    = PRV_CPSS_DXCH3_TC_DP_BUFFER_LIMIT_LEN_CNS;
       descLen    = PRV_CPSS_DXCH3_TC_DP_DESC_LIMIT_LEN_CNS;
       totalLen   = PRV_CPSS_DXCH3_TC_DP_PARAMS_LEN_CNS;
    }

    if( 0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum) )
    {
        /* Get DP0 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                           egrTxQConf.setsConfigRegs.tcDp0Red[trafficClass][profileSet];

        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, totalLen, &regData);
        if(rc != GT_OK)
        {
            return rc;
        }

        tailDropProfileParamsPtr->dp0MaxDescrNum = (GT_U16)U32_GET_FIELD_MAC(regData, 0, descLen);
        tailDropProfileParamsPtr->dp0MaxBuffNum  = (GT_U16)U32_GET_FIELD_MAC(regData, buffOffset, buffLen);

        /* Get DP1 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                           egrTxQConf.setsConfigRegs.tcDp1Red[trafficClass][profileSet];

        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, totalLen, &regData);
        if(rc != GT_OK)
        {
            return rc;
        }

        tailDropProfileParamsPtr->dp2MaxDescrNum = (GT_U16)U32_GET_FIELD_MAC(regData, 0, descLen);
        tailDropProfileParamsPtr->dp2MaxBuffNum  = (GT_U16)U32_GET_FIELD_MAC(regData, buffOffset, buffLen);
    }
    else
    {
        /* Get DP0 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.tailDrop.limits.eqQueueLimitsDp0[profileSet][trafficClass];

        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, totalLen, &regData);
        if(rc != GT_OK)
        {
            return rc;
        }

        tailDropProfileParamsPtr->dp0MaxDescrNum = (GT_U16)U32_GET_FIELD_MAC(regData, 0, descLen);
        tailDropProfileParamsPtr->dp0MaxBuffNum  = (GT_U16)U32_GET_FIELD_MAC(regData, buffOffset, buffLen);

        /* Get DP1 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.tailDrop.limits.eqQueueLimitsDp1[profileSet][trafficClass];

        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, totalLen, &regData);
        if(rc != GT_OK)
        {
            return rc;
        }

        tailDropProfileParamsPtr->dp1MaxDescrNum = (GT_U16)U32_GET_FIELD_MAC(regData, 0, descLen);
        tailDropProfileParamsPtr->dp1MaxBuffNum  = (GT_U16)U32_GET_FIELD_MAC(regData, buffOffset, buffLen);

        /* Get DP2 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.tailDrop.limits.eqQueueLimitsDp2[profileSet][trafficClass];

        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, totalLen, &regData);
        if(rc != GT_OK)
        {
            return rc;
        }

        tailDropProfileParamsPtr->dp2MaxDescrNum = (GT_U16)U32_GET_FIELD_MAC(regData, 0, descLen);
        tailDropProfileParamsPtr->dp2MaxBuffNum  = (GT_U16)U32_GET_FIELD_MAC(regData, buffOffset, buffLen);

        /* Get TC */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.tailDrop.limits.maximumQueueLimits[profileSet][trafficClass];

        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, totalLen, &regData);
        if(rc != GT_OK)
        {
            return rc;
        }

        tailDropProfileParamsPtr->tcMaxDescrNum = (GT_U16)U32_GET_FIELD_MAC(regData, 0, descLen);
        tailDropProfileParamsPtr->tcMaxBuffNum  = (GT_U16)U32_GET_FIELD_MAC(regData, buffOffset, buffLen);
    }

    return GT_OK;
}


/*******************************************************************************
* cpssDxChPortTxTailDropProfileSet
*
* DESCRIPTION:
*       Enables/Disables sharing of buffers and descriptors for all queues of an
*       egress port and set maximal port's limits of buffers and descriptors.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - physical device number
*       profileSet    - the Profile Set in which the Traffic
*                       Class Drop Parameters is associated
*       enableSharing - If GT_TRUE Sharing is enabled. If a packet is to be queued
*                      to an empty queue, it is queued even if the number of buffers
*                       or descriptors allocated to this port exceeds the
*                       < portMaxBuffLimit > and < portMaxDescrLimit >.
*                       If GT_FALSE Sharing is disabled. If the sum of buffers or
*                       descriptors allocated per queue is larger than the number of
*                       buffers or descriptors allocated to this port
*                       < portMaxBuffLimit > and < portMaxDescrLimit >, then some of
*                       the queues may be starved.
*                       DxCh2 and above devices support only GT_FALSE value.
*       portMaxBuffLimit  - maximal number of buffers for a port.
*                          (APPLICABLE RANGES: DxCh1,DxCh2 0..4095;
*                           DxCh3,xCat,Lion,xCat2 0..16383)
*       portMaxDescrLimit - maximal number of descriptors for a port.
*                          (APPLICABLE RANGES: DxCh1,DxCh2 0..4095;
*                           DxCh3,xCat,Lion,xCat2 0..16383)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTailDropProfileSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet,
    IN  GT_BOOL                             enableSharing,
    IN  GT_U16                              portMaxBuffLimit,
    IN  GT_U16                              portMaxDescrLimit
)
{
    GT_U32      regAddr;    /* register address                         */
    GT_U32      buffOffset; /* buffer field offset                      */
    GT_U32      totalLen;   /* total buffer & descriptor len            */
    GT_U32      regData;    /* data to write into register              */
    GT_U32      maxDescVal; /* the maximum allowed descriptors value    */
    GT_U32      maxBuffVal; /* the maximum allowed buffers value        */
    GT_U32      enBuffShar;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_DROP_PROFILE_CHECK_MAC(profileSet, devNum);

    /* enableSharing supported for DxCh1 devices only */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_CHEETAH2_E)
    {
        if(enableSharing == GT_TRUE)
        {
            return GT_BAD_PARAM;
        }
    }
    /* CH & CH2 */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
    {
       maxDescVal = PRV_CPSS_DXCH_TC_DP_DESC_LIMIT_MAX_CNS;
       maxBuffVal = PRV_CPSS_DXCH_TC_DP_BUFFER_LIMIT_MAX_CNS;
       buffOffset = PRV_CPSS_DXCH_TC_DP_BUFFER_LIMIT_OFFSET_CNS;
       totalLen   = PRV_CPSS_DXCH_TC_DP_PARAMS_LEN_CNS;
    }
    else /* CH3 and above */
    {
       maxDescVal = PRV_CPSS_DXCH3_TC_DP_DESC_LIMIT_MAX_CNS;
       maxBuffVal = PRV_CPSS_DXCH3_TC_DP_BUFFER_LIMIT_MAX_CNS;
       buffOffset = PRV_CPSS_DXCH3_TC_DP_BUFFER_LIMIT_OFFSET_CNS;
       totalLen   = PRV_CPSS_DXCH3_TC_DP_PARAMS_LEN_CNS;
    }

    if ( (portMaxBuffLimit > maxBuffVal) ||
         (portMaxDescrLimit > maxDescVal) )
    {
        return GT_OUT_OF_RANGE;
    }

    PRV_CPSS_DXCH_PORT_DESCR_LIMIT_REG_MAC(devNum,profileSet,&regAddr);

    regData = portMaxDescrLimit | (portMaxBuffLimit << buffOffset);

    /* DxCh1 - Enable / Disable sharing of descriptors and buffers resources */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily < CPSS_PP_FAMILY_CHEETAH2_E)
    {
        enBuffShar = BOOL2BIT_MAC(enableSharing);
        regData |= enBuffShar << totalLen;
        totalLen++;
    }

    return prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, totalLen, regData);
}

/*******************************************************************************
* cpssDxChPortTxTailDropProfileGet
*
* DESCRIPTION:
*       Get sharing status of buffers and descriptors for all queues of an
*       egress port and get maximal port's limits of buffers and descriptors.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - physical device number
*       profileSet    - the Profile Set in which the Traffic
*                       Class Drop Parameters is associated
*
* OUTPUTS:
*       enableSharingPtr - Pointer to sharing status.
*                      If GT_TRUE Sharing is enabled. If a packet is to be queued
*                      to an empty queue, it is queued even if the number of buffers
*                       or descriptors allocated to this port exceeds the
*                       < portMaxBuffLimit > and < portMaxDescrLimit >.
*                       If GT_FALSE Sharing is disabled. If the sum of buffers or
*                       descriptors allocated per queue is larger than the number of
*                       buffers or descriptors allocated to this port
*                       < portMaxBuffLimit > and < portMaxDescrLimit >, then some of
*                       the queues may be starved.
*                       DxCh2 and above devices support only GT_FALSE value.
*       portMaxBuffLimitPtr - Pointer to maximal number of buffers for a port
*       portMaxDescrLimitPtr - Pointer to maximal number of descriptors for a port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTailDropProfileGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet,
    OUT GT_BOOL                             *enableSharingPtr,
    OUT GT_U16                              *portMaxBuffLimitPtr,
    OUT GT_U16                              *portMaxDescrLimitPtr
)
{
    GT_U32      regAddr;    /* register address                         */
    GT_U32      buffOffset; /* buffer field offset                      */
    GT_U32      buffLen;    /* buffer field length                      */
    GT_U32      descLen;    /* descriptor field len                     */
    GT_U32      totalLen;   /* total buffer & descriptor len            */
    GT_U32      regData;    /* data to write into register              */
    GT_STATUS   rc;         /* return value                             */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_DROP_PROFILE_CHECK_MAC(profileSet, devNum);
    CPSS_NULL_PTR_CHECK_MAC(enableSharingPtr);
    CPSS_NULL_PTR_CHECK_MAC(portMaxBuffLimitPtr);
    CPSS_NULL_PTR_CHECK_MAC(portMaxDescrLimitPtr);

    /* CH & CH2 */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
    {
       buffOffset = PRV_CPSS_DXCH_TC_DP_BUFFER_LIMIT_OFFSET_CNS;
       buffLen    = PRV_CPSS_DXCH_TC_DP_BUFFER_LIMIT_LEN_CNS;
       descLen    = PRV_CPSS_DXCH_TC_DP_DESC_LIMIT_LEN_CNS;
       totalLen   = PRV_CPSS_DXCH_TC_DP_PARAMS_LEN_CNS;
    }
    else /* CH3 and above */
    {
       buffOffset = PRV_CPSS_DXCH3_TC_DP_BUFFER_LIMIT_OFFSET_CNS;
       buffLen    = PRV_CPSS_DXCH3_TC_DP_BUFFER_LIMIT_LEN_CNS;
       descLen    = PRV_CPSS_DXCH3_TC_DP_DESC_LIMIT_LEN_CNS;
       totalLen   = PRV_CPSS_DXCH3_TC_DP_PARAMS_LEN_CNS;
    }

    PRV_CPSS_DXCH_PORT_DESCR_LIMIT_REG_MAC(devNum,profileSet,&regAddr);

    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, totalLen+1, &regData);
    if(rc != GT_OK)
    {
        return rc;
    }

    *portMaxDescrLimitPtr = (GT_U16)U32_GET_FIELD_MAC(regData, 0, descLen);
    *portMaxBuffLimitPtr = (GT_U16)U32_GET_FIELD_MAC(regData, buffOffset, buffLen);
    *enableSharingPtr = (((regData >> totalLen) & 0x1) == 0) ? GT_FALSE : GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxSniffedPcktDescrLimitSet
*
* DESCRIPTION:
*       Set maximal descriptors limits for mirrored packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - physical device number
*       rxSniffMaxDescNum - The number of descriptors allocated for packets
*                           forwarded to the ingress analyzer port due to
*                           mirroring.
*                           For DxCh1,DxCh2: range 0..0xFFF
*                           For DxCh3,xCat,xCat2: range 0..0x3FFF
*                           For Lion: range 0..0xFFFF
*       txSniffMaxDescNum - The number of descriptors allocated for packets
*                           forwarded to the egress analyzer port due to
*                           mirroring.
*                           For DxCh,DxCh2: range 0..0xFFF
*                           For DxCh3,xCat,xCat2: range 0..0x3FFF
*                           For Lion: range 0..0xFFFF
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on out of range value
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSniffedPcktDescrLimitSet
(
    IN    GT_U8                  devNum,
    IN    GT_U32                 rxSniffMaxDescNum,
    IN    GT_U32                 txSniffMaxDescNum
)
{
    GT_U32      regAddr;        /* register address     */
    GT_U32      rxOffset;       /* ingress field offset */
    GT_U32      len;            /* all fields len       */
    GT_U32      maxRxDescVal;   /* the maximum allowed ingress descriptors value */
    GT_U32      maxTxDescVal;   /* the maximum allowed egress descriptors value  */
    GT_U32      value;          /* value to write into register     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* CH & CH2 */
        if (PRV_CPSS_PP_MAC(devNum)->devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
        {
           maxRxDescVal = PRV_CPSS_DXCH_INGRESS_MIRR_2_ANLY_DESC_MAX_CNS;
           maxTxDescVal = PRV_CPSS_DXCH_EGRESS_MIRR_2_ANLY_DESC_MAX_CNS;
           rxOffset     = PRV_CPSS_DXCH_INGRESS_MIRR_2_ANLY_DESC_MAX_OFFSET_CNS;
           len          = PRV_CPSS_DXCH_MIRR_2_ANLY_DESC_MAX_PARAM_LEN_CNS;
        }
        else /* CH3 and above */
        {
           maxRxDescVal = PRV_CPSS_DXCH3_INGRESS_MIRR_2_ANLY_DESC_MAX_CNS;
           maxTxDescVal = PRV_CPSS_DXCH3_EGRESS_MIRR_2_ANLY_DESC_MAX_CNS;
           rxOffset     = PRV_CPSS_DXCH3_INGRESS_MIRR_2_ANLY_DESC_MAX_OFFSET_CNS;
           len          = PRV_CPSS_DXCH3_MIRR_2_ANLY_DESC_MAX_PARAM_LEN_CNS;
        }

        if ( (rxSniffMaxDescNum > maxRxDescVal) || (txSniffMaxDescNum > maxTxDescVal) )
        {
            return GT_OUT_OF_RANGE;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.sniffTailDropCfgReg;

        value = txSniffMaxDescNum | (rxSniffMaxDescNum << rxOffset);
    }
    else
    {
        if(rxSniffMaxDescNum >= BIT_16 ||
           txSniffMaxDescNum >= BIT_16)
        {
            return GT_OUT_OF_RANGE;
        }

        len = 32;

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.tailDrop.
                    mcFilterLimits.mirroredPacketsToAnalyzerPortDescriptorsLimit;

        value = rxSniffMaxDescNum | (txSniffMaxDescNum << 16);
    }

    /* Set the number of descriptors allocated for packets forwarded
       to the ingress analyzer port and egress analyzer port due to mirroring. */
    return prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, len, value);
}

/*******************************************************************************
* cpssDxChPortTxSniffedPcktDescrLimitGet
*
* DESCRIPTION:
*       Get maximal descriptors limits for mirrored packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - physical device number
*
* OUTPUTS:
*       rxSniffMaxDescNumPtr - (pointer to) The number of descriptors allocated
*                              for packets forwarded to the ingress analyzer
*                              port due to mirroring.
*       txSniffMaxDescNumPtr - (pointer to) The number of descriptors allocated
*                              for packets forwarded to the egress analyzer
*                              port due to mirroring.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSniffedPcktDescrLimitGet
(
    IN    GT_U8                  devNum,
    OUT   GT_U32                *rxSniffMaxDescNumPtr,
    OUT   GT_U32                *txSniffMaxDescNumPtr
)
{
    GT_U32      regAddr;        /* register address     */
    GT_U32      rxOffset;       /* ingress field offset */
    GT_U32      maxRxDescVal;   /* the maximum allowed ingress descriptors value */
    GT_U32      maxTxDescVal;   /* the maximum allowed egress descriptors value  */
    GT_U32      len;            /* all fields len       */
    GT_U32      value;          /* value to write into register     */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(rxSniffMaxDescNumPtr);
    CPSS_NULL_PTR_CHECK_MAC(txSniffMaxDescNumPtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* CH & CH2 */
        if (PRV_CPSS_PP_MAC(devNum)->devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
        {
            maxRxDescVal = PRV_CPSS_DXCH_INGRESS_MIRR_2_ANLY_DESC_MAX_CNS;
            maxTxDescVal = PRV_CPSS_DXCH_EGRESS_MIRR_2_ANLY_DESC_MAX_CNS;
            rxOffset     = PRV_CPSS_DXCH_INGRESS_MIRR_2_ANLY_DESC_MAX_OFFSET_CNS;
            len          = PRV_CPSS_DXCH_MIRR_2_ANLY_DESC_MAX_PARAM_LEN_CNS;
        }
        else /* CH3 and above */
        {
            maxRxDescVal = PRV_CPSS_DXCH3_INGRESS_MIRR_2_ANLY_DESC_MAX_CNS;
            maxTxDescVal = PRV_CPSS_DXCH3_EGRESS_MIRR_2_ANLY_DESC_MAX_CNS;
            rxOffset     = PRV_CPSS_DXCH3_INGRESS_MIRR_2_ANLY_DESC_MAX_OFFSET_CNS;
            len          = PRV_CPSS_DXCH3_MIRR_2_ANLY_DESC_MAX_PARAM_LEN_CNS;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.sniffTailDropCfgReg;
    }
    else
    {
        maxRxDescVal = BIT_16-1;
        maxTxDescVal = BIT_16-1;
        rxOffset     = 0;
        len          = 32;

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.tailDrop.
            mcFilterLimits.mirroredPacketsToAnalyzerPortDescriptorsLimit;
    }

    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, len, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        *txSniffMaxDescNumPtr = value & maxTxDescVal;
        *rxSniffMaxDescNumPtr = (value >> rxOffset) & maxRxDescVal;
    }
    else
    {
        *rxSniffMaxDescNumPtr = value & maxRxDescVal;
        *txSniffMaxDescNumPtr = (value >> 16) & maxTxDescVal;
    }


    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxMcastPcktDescrLimitSet
*
* DESCRIPTION:
*       Set maximal descriptors limits for multicast packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - physical device number
*       mcastMaxDescNum   - The number of descriptors allocated for multicast
*                           packets.
*                           For all devices except Lion: in units of 128
*                           descriptors, the actual number descriptors
*                           allocated will be 128 * mcastMaxDescNum.
*                           For Lion: actual descriptors number (granularity
*                           of 1).
*                           For DxCh1,DxCh2,DxCh3,xCat,xCat2: range 0..0xF80
*                           For Lion: range 0..0xFFFF
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxMcastPcktDescrLimitSet
(
    IN    GT_U8                  devNum,
    IN    GT_U32                 mcastMaxDescNum
)
{
    GT_U32      regAddr;    /* register address                 */
    GT_U32      offset;     /* field offset                     */
    GT_U32      len;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        if (mcastMaxDescNum > PRV_CPSS_DXCH_MULTICAST_PCKTS_DESC_LIMIT_MAX_CNS)
        {
            return GT_BAD_PARAM;
        }

        /* Set the number of descriptors allocated for multicast packets. */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfig;
        len    = 5;
        offset = 11;  
    }
    else
    {
        if ( mcastMaxDescNum >= BIT_16 )
        {
            return GT_BAD_PARAM;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.tailDrop.
                    mcFilterLimits.multicastDescriptorsLimit;
        len    = 16;
        offset = 0;
    }

    return prvCpssDxChHwPpSetRegField(devNum, regAddr, offset, len, mcastMaxDescNum);
}

/*******************************************************************************
* cpssDxChPortTxMcastPcktDescrLimitGet
*
* DESCRIPTION:
*       Get maximal descriptors limits for multicast packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - physical device number
*
* OUTPUTS:
*       mcastMaxDescNumPtr  - (pointer to) the number of descriptors allocated
*                             for multicast packets.
*                             For all devices except Lion: in units of 128
*                             descriptors, the actual number descriptors
*                             allocated will be 128 * mcastMaxDescNum.
*                             For Lion: actual descriptors number (granularity
*                             of 1).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxMcastPcktDescrLimitGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *mcastMaxDescNumPtr
)
{
    GT_U32      regAddr;    /* register address                 */
    GT_U32      offset;     /* field offset                     */
    GT_U32      len;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(mcastMaxDescNumPtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* Get the number of descriptors allocated for multicast packets. */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfig;
        len    = 5;
        offset = 11;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.tailDrop.
                    mcFilterLimits.multicastDescriptorsLimit;
        len    = 16;
        offset = 0;
    }

    return prvCpssDxChHwPpGetRegField(devNum, regAddr, offset, len, mcastMaxDescNumPtr);
}

/*******************************************************************************
* cpssDxChPortTxWrrGlobalParamSet
*
* DESCRIPTION:
*       Set global parameters for WRR scheduler.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - physical device number
*       wrrMode - Transmit Scheduler count mode.
*       wrrMtu  - MTU for DSWRR, resolution of WRR weights for byte based
*                 Transmit Scheduler count mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        For DxCh1, DxCh2, DxCh3, xCat and xCat2 only CPSS_PORT_TX_WRR_MTU_2K_E
*        and CPSS_PORT_TX_WRR_MTU_8K_E are supported.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxWrrGlobalParamSet
(
    IN    GT_U8                     devNum,
    IN    CPSS_PORT_TX_WRR_MODE_ENT wrrMode,
    IN    CPSS_PORT_TX_WRR_MTU_ENT  wrrMtu
)
{
    GT_U32      regAddr;    /* register address                 */
    GT_U32      offset;     /* field offset                     */
    GT_U32      len;
    GT_U32      wrrModeValue;
    GT_U32      wrrMtuValue;
    GT_U32      value;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    switch (wrrMode)
    {
        case CPSS_PORT_TX_WRR_BYTE_MODE_E:
            wrrModeValue = 1;
            break;
        case CPSS_PORT_TX_WRR_PACKET_MODE_E:
            wrrModeValue = 0;
            break;
        default:
            return GT_BAD_PARAM;
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        switch (wrrMtu)
        {
            case CPSS_PORT_TX_WRR_MTU_2K_E:
                wrrMtuValue = 0;
                break;
            case CPSS_PORT_TX_WRR_MTU_8K_E:
                wrrMtuValue = 1;
                break;
            case CPSS_PORT_TX_WRR_MTU_256_E:
            default:
                return GT_BAD_PARAM;
        }

        /* Set global parameters for WRR scheduler. */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfigExt;

        offset = 1;
        len = 2;

        value = wrrModeValue | (wrrMtuValue << 1);

        return prvCpssDxChHwPpSetRegField(devNum,regAddr,offset,len,value);
    }
    else
    {
        switch (wrrMtu)
        {
            case CPSS_PORT_TX_WRR_MTU_2K_E:
                wrrMtuValue = 5;
                break;
            case CPSS_PORT_TX_WRR_MTU_8K_E:
                wrrMtuValue = 7;
                break;
            case CPSS_PORT_TX_WRR_MTU_256_E:
                wrrMtuValue = 2;
                break;
            case CPSS_PORT_TX_WRR_MTU_64_E:
                wrrMtuValue = 0;
                break;
            case CPSS_PORT_TX_WRR_MTU_128_E:
                wrrMtuValue = 1;
                break;
            case CPSS_PORT_TX_WRR_MTU_512_E:
                wrrMtuValue = 3;
                break;
            case CPSS_PORT_TX_WRR_MTU_1K_E:
                wrrMtuValue = 4;
                break;
            case CPSS_PORT_TX_WRR_MTU_4K_E:
                wrrMtuValue = 6;
                break;
            case CPSS_PORT_TX_WRR_MTU_16K_E:
                wrrMtuValue = 8;
                break;
            case CPSS_PORT_TX_WRR_MTU_32K_E:
                wrrMtuValue = 9;
                break;
            case CPSS_PORT_TX_WRR_MTU_64K_E:
                wrrMtuValue = 10;
                break;
            default:
                return GT_BAD_PARAM;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.scheduler.config.config;
        offset = 1;
        len = 5;

        value = wrrModeValue | (wrrMtuValue << 1);

        return prvCpssDxChHwPpSetRegField(devNum,regAddr,offset,len,value);
    }
}

/*******************************************************************************
* cpssDxChPortTxWrrGlobalParamGet
*
* DESCRIPTION:
*       Get global parameters for WRR scheduler.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       wrrModePtr - (pointer to) Transmit Scheduler count mode.
*       wrrMtuPtr  - (pointer to) MTU for DSWRR, resolution of WRR weights
*                    for byte based Transmit Scheduler count mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxWrrGlobalParamGet
(
    IN    GT_U8                      devNum,
    OUT   CPSS_PORT_TX_WRR_MODE_ENT *wrrModePtr,
    OUT   CPSS_PORT_TX_WRR_MTU_ENT  *wrrMtuPtr
)
{
    GT_STATUS   rc;         /* return value         */
    GT_U32      regAddr;    /* register address     */
    GT_U32      value;      /* register field value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(wrrModePtr);
    CPSS_NULL_PTR_CHECK_MAC(wrrMtuPtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfigExt;
        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 1, 2, &value);
        if (rc != GT_OK)
            return rc;

        *wrrModePtr = (value & 0x1) ?  CPSS_PORT_TX_WRR_BYTE_MODE_E :
            CPSS_PORT_TX_WRR_PACKET_MODE_E ;
        *wrrMtuPtr = ((value >> 1) & 0x1) ? CPSS_PORT_TX_WRR_MTU_8K_E :
            CPSS_PORT_TX_WRR_MTU_2K_E;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.scheduler.config.config;
        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 1, 5, &value);
        if (rc != GT_OK)
            return rc;

        *wrrModePtr = (value & 0x1) ?  CPSS_PORT_TX_WRR_BYTE_MODE_E :
            CPSS_PORT_TX_WRR_PACKET_MODE_E ;
        switch (value >> 1)
        {
            case 5:
                *wrrMtuPtr = CPSS_PORT_TX_WRR_MTU_2K_E;
                break;
            case 7:
                *wrrMtuPtr = CPSS_PORT_TX_WRR_MTU_8K_E;
                break;
            case 2:
                *wrrMtuPtr = CPSS_PORT_TX_WRR_MTU_256_E;
                break;
            case 0:
                *wrrMtuPtr = CPSS_PORT_TX_WRR_MTU_64_E;
                break;
            case 1:
                *wrrMtuPtr = CPSS_PORT_TX_WRR_MTU_128_E;
                break;
            case 3:
                *wrrMtuPtr = CPSS_PORT_TX_WRR_MTU_512_E;
                break;
            case 4:
                *wrrMtuPtr = CPSS_PORT_TX_WRR_MTU_1K_E;
                break;
            case 6:
                *wrrMtuPtr = CPSS_PORT_TX_WRR_MTU_4K_E;
                break;
            case 8:
                *wrrMtuPtr = CPSS_PORT_TX_WRR_MTU_16K_E;
                break;
            case 9:
                *wrrMtuPtr = CPSS_PORT_TX_WRR_MTU_32K_E;
                break;
            case 10:
                *wrrMtuPtr = CPSS_PORT_TX_WRR_MTU_64K_E;
                break;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxShaperGlobalParamsSet
*
* DESCRIPTION:
*       Set Global parameters for shaper.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion.
*
* INPUTS:
*       devNum               - physical device number
*       xgPortsTokensRate    - tokens rate for XG ports' shapers (1..15)
*       gigPortsTokensRate   - tokens rate for Tri Speed ports' shapers.(1..15)
*       gigPortsSlowRateRatio - slow rate ratio for Tri Speed ports.
*                               Tokens Update rate for ports with slow rate
*                               divided to the ratio. (1..16)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperGlobalParamsSet
(
    IN    GT_U8                   devNum,
    IN    GT_U32                  xgPortsTokensRate,
    IN    GT_U32                  gigPortsTokensRate,
    IN    GT_U32                  gigPortsSlowRateRatio
)
{
    GT_U32      regAddr;    /* register address                 */
    GT_U32      offset;     /* field offset                     */
    GT_U32      len;
    GT_U32      value;      /* value to write into register     */
    GT_U8       port;
    GT_U32      tcQueue = 0;
    GT_U32      old_xgPortsTokensRate;
    GT_U32      old_gigPortsTokensRate;
    GT_U32      old_gigPortsSlowRateRatio;
    GT_STATUS   rc;
    GT_BOOL     stackAsGig = GT_FALSE;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,CPSS_LION_E);

    if ( (xgPortsTokensRate < PRV_CPSS_DXCH_XG_TOKEN_BCKT_UPD_RATE_MIN_CNS) ||
         (xgPortsTokensRate > PRV_CPSS_DXCH_XG_TOKEN_BCKT_UPD_RATE_MAX_CNS) ||
         (gigPortsTokensRate < PRV_CPSS_DXCH_GIG_TOKEN_BCKT_UPD_RATE_MIN_CNS) ||
         (gigPortsTokensRate > PRV_CPSS_DXCH_GIG_TOKEN_BCKT_UPD_RATE_MAX_CNS) ||
         (gigPortsSlowRateRatio < PRV_CPSS_DXCH_GIG_SLOW_TOKEN_BCKT_UPD_RATIO_MIN_CNS) ||
         (gigPortsSlowRateRatio > PRV_CPSS_DXCH_GIG_SLOW_TOKEN_BCKT_UPD_RATIO_MAX_CNS) )
    {
        return GT_BAD_PARAM;
    }

    /* Set global parameters for shaper. */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQMcFifoEccConfig;

    offset = PRV_CPSS_DXCH_GIG_SLOW_TOKEN_BCKT_UPD_RATIO_OFFSET_CNS;
    len = PRV_CPSS_DXCH_GIG_SLOW_TOKEN_BCKT_UPD_RATIO_LEN_CNS +
          PRV_CPSS_DXCH_GIG_TOKEN_BCKT_UPD_RATE_LEN_CNS +
          PRV_CPSS_DXCH_XG_TOKEN_BCKT_UPD_RATE_LEN_CNS; /* =12 */

    if (prvCpssDxChHwPpGetRegField(devNum, regAddr, offset, len, &value) != GT_OK)
        return GT_HW_ERROR;

    old_gigPortsSlowRateRatio = (value & 0xF) + 1;
    old_gigPortsTokensRate = (value >>
                               (PRV_CPSS_DXCH_GIG_TOKEN_BCKT_UPD_RATE_OFFSET_CNS - offset)) &
                              0xF;
    old_xgPortsTokensRate = (value >>
                              (PRV_CPSS_DXCH_XG_TOKEN_BCKT_UPD_RATE_OFFSET_CNS - offset)) &
                             0xF;

    value = (gigPortsSlowRateRatio - 1) |
            ((gigPortsTokensRate & 0xF) <<
             (PRV_CPSS_DXCH_GIG_TOKEN_BCKT_UPD_RATE_OFFSET_CNS - offset)) |
            ((xgPortsTokensRate & 0xF) <<
             (PRV_CPSS_DXCH_XG_TOKEN_BCKT_UPD_RATE_OFFSET_CNS - offset));

    if (prvCpssDxChHwPpSetRegField(devNum, regAddr, offset, len, value) != GT_OK)
        return GT_HW_ERROR;

    /* check if StackAsGig mode enabled */
    if ((PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(devNum)) || 
        (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        rc = cpssDxChPortTxShaperOnStackAsGigEnableGet(devNum,&stackAsGig);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* Reconfigure all enabled Token Buckets per port and per port per queue. */
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    for (port = 0; port < PRV_CPSS_PP_MAC(devNum)->numOfPorts; port++)
    {
        if (! PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, port))
            continue;

        /* per port */
        rc = prvDxChPortTxSetShaperTokenBucketParams(
                devNum,
                port,
                GT_FALSE,
                tcQueue,
                old_xgPortsTokensRate,
                old_gigPortsTokensRate,
                old_gigPortsSlowRateRatio,
                stackAsGig,
                xgPortsTokensRate,
                gigPortsTokensRate,
                gigPortsSlowRateRatio,
                stackAsGig);

        if (rc != GT_OK)
        {
            return rc;
        }

        /* per port per traffic class (tcQueue) */
        for (tcQueue = 0; tcQueue < CPSS_TC_RANGE_CNS; tcQueue++)
        {
            rc = prvDxChPortTxSetShaperTokenBucketParams(
                    devNum,
                    port,
                    GT_TRUE,
                    tcQueue,
                    old_xgPortsTokensRate,
                    old_gigPortsTokensRate,
                    old_gigPortsSlowRateRatio,
                    stackAsGig,
                    xgPortsTokensRate,
                    gigPortsTokensRate,
                    gigPortsSlowRateRatio,
                    stackAsGig);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    /* for CPU port */
    port = (GT_U8)CPSS_CPU_PORT_NUM_CNS;
    rc = prvDxChPortTxSetShaperTokenBucketParams(
            devNum,
            port,
            GT_FALSE,
            tcQueue,
            old_xgPortsTokensRate,
            old_gigPortsTokensRate,
            old_gigPortsSlowRateRatio,
            stackAsGig,
            xgPortsTokensRate,
            gigPortsTokensRate,
            gigPortsSlowRateRatio,
            stackAsGig);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* CPU port per traffic class (tcQueue) */
    for (tcQueue = 0; tcQueue < CPSS_TC_RANGE_CNS; tcQueue++)
    {
        rc = prvDxChPortTxSetShaperTokenBucketParams(
                devNum,
                port,
                GT_TRUE,
                tcQueue,
                old_xgPortsTokensRate,
                old_gigPortsTokensRate,
                old_gigPortsSlowRateRatio,
                stackAsGig,
                xgPortsTokensRate,
                gigPortsTokensRate,
                gigPortsSlowRateRatio,
                stackAsGig);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxShaperGlobalParamsGet
*
* DESCRIPTION:
*       Get Global parameters for shaper.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion.
*
* INPUTS:
*      devNum                   - device number
*
* OUTPUTS:
*       xgPortsTokensRatePtr     - (pointer to) tokens rate for XG ports' shapers
*       gigPortsTokensRatePtr    - (pointer to) tokens rate for Tri Speed ports' shapers
*       gigPortsSlowRateRatioPtr - (pointer to) slow rate ratio for Tri Speed ports.
*                                  Tokens Update rate for ports with slow rate divided
*                                  to the ratio.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperGlobalParamsGet
(
    IN    GT_U8          devNum,
    OUT   GT_U32         *xgPortsTokensRatePtr,
    OUT   GT_U32         *gigPortsTokensRatePtr,
    OUT   GT_U32         *gigPortsSlowRateRatioPtr
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regData;    /* register data    */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,CPSS_LION_E);

    CPSS_NULL_PTR_CHECK_MAC(xgPortsTokensRatePtr);
    CPSS_NULL_PTR_CHECK_MAC(gigPortsTokensRatePtr);
    CPSS_NULL_PTR_CHECK_MAC(gigPortsSlowRateRatioPtr);


    /* Set global parameters for shaper. */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQMcFifoEccConfig;


    if (prvCpssDxChHwPpReadRegister(devNum, regAddr, &regData) != GT_OK)
        return GT_HW_ERROR;

    *xgPortsTokensRatePtr = U32_GET_FIELD_MAC(regData,
                                              PRV_CPSS_DXCH_XG_TOKEN_BCKT_UPD_RATE_OFFSET_CNS,
                                              PRV_CPSS_DXCH_XG_TOKEN_BCKT_UPD_RATE_LEN_CNS);

    *gigPortsTokensRatePtr = U32_GET_FIELD_MAC(regData,
                                               PRV_CPSS_DXCH_GIG_TOKEN_BCKT_UPD_RATE_OFFSET_CNS,
                                               PRV_CPSS_DXCH_GIG_TOKEN_BCKT_UPD_RATE_LEN_CNS);

    *gigPortsSlowRateRatioPtr = U32_GET_FIELD_MAC(regData,
                                                  PRV_CPSS_DXCH_GIG_SLOW_TOKEN_BCKT_UPD_RATIO_OFFSET_CNS,
                                                  PRV_CPSS_DXCH_GIG_SLOW_TOKEN_BCKT_UPD_RATIO_LEN_CNS) + 1;

    if ( *xgPortsTokensRatePtr < PRV_CPSS_DXCH_XG_TOKEN_BCKT_UPD_RATE_MIN_CNS)
    {
        return GT_HW_ERROR;
    }

    if(*gigPortsTokensRatePtr < PRV_CPSS_DXCH_GIG_TOKEN_BCKT_UPD_RATE_MIN_CNS)
    {
        return GT_HW_ERROR;
    }

    return GT_OK;
}

/*******************************************************************************
* updateSchedVarTriggerBitWaitAndSet
*
* DESCRIPTION:
*       wait for <UpdateSchedVarTrigger> to be ready for triggering , and when
*       ready --> trigger new action.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT               - previous updated operation not yet completed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS    updateSchedVarTriggerBitWaitAndSet(
    IN  GT_U8  devNum
)
{
    GT_U32      regAddr;    /* register address                 */
    GT_U32      updateSchedVarTriggerBit;/*bit of <UpdateSchedVarTrigger>*/
    GT_STATUS   rc;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        updateSchedVarTriggerBit = 4;
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfig;
    }
    else
    {
        updateSchedVarTriggerBit = 6;
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.scheduler.config.config;
    }

    /* Each of the the scheduling parameters need to be set by the <UpdateSchedVarTrigger> */
    /* in the Transmit Queue Control Register in order to take effect. */

    /* wait for bit <UpdateSchedVarTrigger> to clear */
    rc = prvCpssDxChPortGroupBusyWait(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,updateSchedVarTriggerBit,GT_FALSE);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* 'Trigger the new setting' */
    return prvCpssDxChHwPpSetRegField(devNum, regAddr, updateSchedVarTriggerBit, 1, 1);
}

/*******************************************************************************
* cpssDxChPortTxQWrrProfileSet
*
* DESCRIPTION:
*       Set Weighted Round Robin profile on the specified port's
*       Traffic Class Queue.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       tcQueue    - traffic class queue on this Port (0..7)
*       wrrWeight  - proportion of bandwidth assigned to this queue
*                    relative to the other queues in this
*                    Arbitration Group - resolution is 1/255.
*       profileSet - the Tx Queue scheduler Profile Set in which the wrrWeight
*                    Parameter is associated.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, profile set or
*                                  traffic class
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT               - previous updated operation not yet completed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*         - if weight will be less then port's MTU (maximum transmit unit) there
*           is possibility for empty WRR loops for given TC queue, but to not
*           tight user and for future ASIC's where this problem will be solved
*           check of wrrWeight * 256 > MTU not implemented
*         - There is errata for Cheetah FEr#29. weigh cannot be = 255 for some
*           modes. The functions limits weight accordingly.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQWrrProfileSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U8                                   tcQueue,
    IN  GT_U8                                   wrrWeight,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet
)
{
    GT_U32      value;      /* value to write into register     */
    GT_U32      regAddr;    /* register address                 */
    GT_U32      offset;     /* register field offset */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_SCHEDULER_PROFILE_CHECK_MAC(profileSet, devNum);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);

    /* In Jumbo frames mode the maximal SDWRR weight that may be
       configured is 254. (FEr#29) */
    value = ((wrrWeight == 255) && (PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
    PRV_CPSS_DXCH_JUMBO_FRAMES_MODE_SDWRR_WEIGHT_LIMITATION_WA_E) == GT_TRUE)) ?
    254 : wrrWeight;

    if (tcQueue < 4)
    {
        PRV_CPSS_DXCH_PORT_WRR_WEIGHTS_0_REG_MAC(devNum,profileSet,&regAddr);
        offset = tcQueue << 3;
    }
    else
    {
        PRV_CPSS_DXCH_PORT_WRR_WEIGHTS_1_REG_MAC(devNum,profileSet,&regAddr);
        offset = (tcQueue - 4) << 3;
    }

    if(prvCpssDxChHwPpSetRegField(devNum, regAddr, offset, 8, value) != GT_OK)
        return GT_HW_ERROR;

    /* Each of the the scheduling parameters need to be set by the <UpdateSchedVarTrigger> */
    /* in the Transmit Queue Control Register in order to take effect. */
    return updateSchedVarTriggerBitWaitAndSet(devNum);
}

/*******************************************************************************
* cpssDxChPortTxQWrrProfileGet
*
* DESCRIPTION:
*       Get Weighted Round Robin profile on the specified port's
*       Traffic Class Queue.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       tcQueue    - traffic class queue on this Port (0..7)
*       profileSet - the Tx Queue scheduler Profile Set in which the wrrWeight
*                    Parameter is associated.
* OUTPUTS:
*       wrrWeightPtr  - Pointer to proportion of bandwidth assigned to this queue
*                       relative to the other queues in this
*                       Arbitration Group - resolution is 1/255.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, profile set or
*                                  traffic class
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT               - previous updated operation not yet completed
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQWrrProfileGet
(
    IN  GT_U8                                   devNum,
    IN  GT_U8                                   tcQueue,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    OUT  GT_U8                                  *wrrWeightPtr
)
{
    GT_U32      regAddr;      /* register address */
    GT_U32      offset;       /* register field offset */
    GT_U32      fieldValue;   /* register field value */
    GT_STATUS   rc;           /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_SCHEDULER_PROFILE_CHECK_MAC(profileSet, devNum);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);
    CPSS_NULL_PTR_CHECK_MAC(wrrWeightPtr);


    if (tcQueue < 4)
    {
        PRV_CPSS_DXCH_PORT_WRR_WEIGHTS_0_REG_MAC(devNum,profileSet,&regAddr);
        offset = tcQueue << 3;
    }
    else
    {
        PRV_CPSS_DXCH_PORT_WRR_WEIGHTS_1_REG_MAC(devNum,profileSet,&regAddr);
        offset = (tcQueue - 4) << 3;
    }

    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, offset, 8, &fieldValue);
    if(rc != GT_OK)
    {
        return rc;
    }

    *wrrWeightPtr = (GT_U8)fieldValue;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxQArbGroupSet
*
* DESCRIPTION:
*       Set Traffic Class Queue scheduling arbitration group on
*       specificed profile of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       tcQueue    - traffic class queue (0..7)
*       arbGroup   - scheduling arbitration group:
*                     1) Strict Priority
*                     2) WRR Group 1
*                     3) WRR Group 0
*       profileSet - the Tx Queue scheduler Profile Set in which the arbGroup
*                    parameter is associated.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong profile number, device or arbGroup
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT               - previous updated operation not yet completed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQArbGroupSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U8                                   tcQueue,
    IN  CPSS_PORT_TX_Q_ARB_GROUP_ENT            arbGroup,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet
)
{
    GT_U32      wrrEn  = 0;   /* WRR arbitration group enable     */
    GT_U32      wrrGrp = 0;   /* WRR arbitration group            */
    GT_U32      regAddr;      /* register address                 */
    GT_U32      offset;       /* register field offset */
    GT_U32      fieldLength;  /* register field offset */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_SCHEDULER_PROFILE_CHECK_MAC(profileSet, devNum);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);

    switch (arbGroup)
    {
        case CPSS_PORT_TX_WRR_ARB_GROUP_0_E:
            wrrEn = 1;
            wrrGrp = 0;
        break;
        case CPSS_PORT_TX_WRR_ARB_GROUP_1_E:
            wrrEn = 1;
            wrrGrp = 1;
        break;
        case CPSS_PORT_TX_SP_ARB_GROUP_E:
            wrrEn = 0;
        break;
        default:
            return GT_BAD_PARAM;
    }

    PRV_CPSS_DXCH_PORT_WRR_STRICT_PRIO_EN_REG_MAC(devNum,profileSet,&regAddr);

    offset = tcQueue;
    fieldLength = 1;

    if (prvCpssDxChHwPpSetRegField(devNum, regAddr, offset, fieldLength, wrrEn) != GT_OK)
        return GT_HW_ERROR;

    if ( arbGroup != CPSS_PORT_TX_SP_ARB_GROUP_E)
    {
        PRV_CPSS_DXCH_PORT_WRR_STRICT_PRIO_REG_MAC(devNum,profileSet,&regAddr);

        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
        {
            offset = tcQueue + 8;
        }
        else
        {
            offset = tcQueue;
        }

        fieldLength = 1;

        if (prvCpssDxChHwPpSetRegField(devNum, regAddr, offset, fieldLength, wrrGrp) != GT_OK)
            return GT_HW_ERROR;
    }

    /* Each of the the scheduling parameters need to be set by the <UpdateSchedVarTrigger> */
    /* in the Transmit Queue Control Register in order to take effect. */

    return updateSchedVarTriggerBitWaitAndSet(devNum);
}

/*******************************************************************************
* cpssDxChPortTxQArbGroupGet
*
* DESCRIPTION:
*       Get Traffic Class Queue scheduling arbitration group on
*       specificed profile of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       tcQueue    - traffic class queue (0..7)
*       profileSet - the Tx Queue scheduler Profile Set in which the arbGroup
*                    parameter is associated.
*
* OUTPUTS:
*       arbGroupPtr   - Pointer to scheduling arbitration group:
*                       1) Strict Priority
*                       2) WRR Group 1
*                       3) WRR Group 0
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong profile number, device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQArbGroupGet
(
    IN  GT_U8                                   devNum,
    IN  GT_U8                                   tcQueue,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    OUT CPSS_PORT_TX_Q_ARB_GROUP_ENT            *arbGroupPtr
)
{

    GT_STATUS   rc;           /* return status */
    GT_U32      wrrEn;        /* WRR arbitration group enable     */
    GT_U32      wrrGrp;       /* WRR arbitration group            */
    GT_U32      regAddr;      /* register address                 */
    GT_U32      offset;       /* register field offset */
    GT_U32      fieldLength;  /* register field length */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);
    PRV_CPSS_DXCH_SCHEDULER_PROFILE_CHECK_MAC(profileSet, devNum);
    CPSS_NULL_PTR_CHECK_MAC(arbGroupPtr);

    PRV_CPSS_DXCH_PORT_WRR_STRICT_PRIO_EN_REG_MAC(devNum,profileSet,&regAddr);

    offset = tcQueue;
    fieldLength = 1;

    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, offset, fieldLength, &wrrEn);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(wrrEn == 0)
    {
        *arbGroupPtr = CPSS_PORT_TX_SP_ARB_GROUP_E;
    }
    else
    {
        PRV_CPSS_DXCH_PORT_WRR_STRICT_PRIO_REG_MAC(devNum,profileSet,&regAddr);

        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
        {
            offset = tcQueue + 8;
        }
        else
        {
            offset = tcQueue;
        }

        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, offset, fieldLength, &wrrGrp);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(wrrGrp == 1)
        {
            *arbGroupPtr = CPSS_PORT_TX_WRR_ARB_GROUP_1_E;
        }
        else
        {
            *arbGroupPtr = CPSS_PORT_TX_WRR_ARB_GROUP_0_E;
        }
    }

    return GT_OK;

}

/*******************************************************************************
* cpssTxPortBindPortToDpSet
*
* DESCRIPTION:
*       Bind a Physical Port to a specific Drop Profile Set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - physical device number
*       portNum       - physical or CPU port number
*       profileSet    - the Profile Set in which the Traffic
*                       Class Drop Parameters is associated
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxBindPortToDpSet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               portNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet
)
{
    GT_STATUS   rc;
    GT_U32      lowValue;   /* two lower bits of the value to write
                               into register */
    GT_U32      upperValue; /* upper bit of the value to write into register*/
    GT_U32      regAddr;    /* register address           */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_DROP_PROFILE_CHECK_MAC(profileSet, devNum);

    lowValue = profileSet;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        PRV_CPSS_DXCH_PORT_TXQ_REG_MAC(devNum,portNum,&regAddr);

        lowValue &= 0x3;

        /* set the two low bits of Drop Profile value */
        if (prvCpssDxChHwPpPortGroupSetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                regAddr, 18, 2, lowValue) != GT_OK)
            return GT_HW_ERROR;

        if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            if (profileSet > CPSS_PORT_TX_DROP_PROFILE_4_E)
                upperValue = 1;
            else
                upperValue = 0;

            /* set the upper bit of the Drop Profile value - bit 25 */
            return prvCpssDxChHwPpPortGroupSetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                    regAddr, 25, 1, upperValue);

        }
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.
            tailDrop.config.tailDropCnProfile[portNum];

        rc = prvCpssDxChHwPpPortGroupSetRegField(devNum,
                PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                regAddr, 0, 3, lowValue);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxBindPortToDpGet
*
* DESCRIPTION:
*       Get Drop Profile Set according to a Physical Port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical or CPU port number
*
* OUTPUTS:
*       profileSetPtr    - Pointer to the Profile Set in which the Traffic
*                          Class Drop Parameters is associated
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxBindPortToDpGet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               portNum,
    OUT CPSS_PORT_TX_DROP_PROFILE_SET_ENT   *profileSetPtr
)
{
    GT_U32      lowValue;   /* two lower bits of the value to read
                               from register */
    GT_U32      upperValue; /* upper bit of the value to read from register*/
    GT_U32      regAddr;    /* register address           */
    GT_STATUS   rc;         /* return value */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(profileSetPtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        PRV_CPSS_DXCH_PORT_TXQ_REG_MAC(devNum,portNum,&regAddr);

        /* get the two low bits of Drop Profile value */
        rc = prvCpssDxChHwPpPortGroupGetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                regAddr, 18, 2, &lowValue);
        if(rc != GT_OK)
        {
            return rc;
        }

        if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            /* get the upper bit of the Drop Profile value - bit 25 */
            rc = prvCpssDxChHwPpPortGroupGetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                    regAddr, 25, 1, &upperValue);
            if(rc != GT_OK)
            {
                return rc;
            }

            lowValue |= upperValue << 2;

        }
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.
            tailDrop.config.tailDropCnProfile[portNum];

        rc = prvCpssDxChHwPpPortGroupGetRegField(devNum,
                PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                regAddr, 0, 3, &lowValue);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    switch (lowValue)
    {
        case 0:
            *profileSetPtr = CPSS_PORT_TX_DROP_PROFILE_1_E;
            break;
        case 1:
            *profileSetPtr = CPSS_PORT_TX_DROP_PROFILE_2_E;
            break;
        case 2:
            *profileSetPtr = CPSS_PORT_TX_DROP_PROFILE_3_E;
            break;
        case 3:
            *profileSetPtr = CPSS_PORT_TX_DROP_PROFILE_4_E;
            break;
        case 4:
            *profileSetPtr = CPSS_PORT_TX_DROP_PROFILE_5_E;
            break;
        case 5:
            *profileSetPtr = CPSS_PORT_TX_DROP_PROFILE_6_E;
            break;
        case 6:
            *profileSetPtr = CPSS_PORT_TX_DROP_PROFILE_7_E;
            break;
        case 7:
            *profileSetPtr = CPSS_PORT_TX_DROP_PROFILE_8_E;
            break;
        default: return GT_FAIL;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxDescNumberGet
*
* DESCRIPTION:
*       Gets the current number of descriptors allocated per specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical or CPU port number
*
* OUTPUTS:
*       numberPtr - number of descriptors
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxDescNumberGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_U16      *numberPtr
)
{
    GT_U32      value;      /* value to write into register     */
    GT_U32      regAddr;    /* register address                 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(numberPtr);

    PRV_CPSS_DXCH_PORT_DESCR_COUNT_REG_MAC(devNum,portNum,&regAddr);

    if(prvCpssDxChHwPpPortGroupGetRegField(devNum,PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr,0,14,&value) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    *numberPtr = (GT_U16)value;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxBufNumberGet
*
* DESCRIPTION:
*       Gets the current number of buffers allocated per specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical or CPU port number
*
* OUTPUTS:
*       numPtr   - number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxBufNumberGet
(
    IN   GT_U8       devNum,
    IN   GT_U8       portNum,
    OUT  GT_U16      *numPtr
)
{

    GT_U32      value;      /* value to write into register     */
    GT_U32      regAddr;    /* register address                 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(numPtr);

    PRV_CPSS_DXCH_PORT_BUFFER_COUNT_REG_MAC(devNum,portNum,&regAddr);

    if(prvCpssDxChHwPpPortGroupGetRegField(devNum,PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr,0,14,&value) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    *numPtr = (GT_U16)value;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTx4TcDescNumberGet
*
* DESCRIPTION:
*       Gets the current number of descriptors allocated on specified port
*       for specified Traffic Class queues.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical or CPU port number
*       trafClass - trafiic class (0..7)
*
* OUTPUTS:
*       numberPtr  - (pointer to) the number of descriptors
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTx4TcDescNumberGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_U8       trafClass,
    OUT GT_U16      *numberPtr
)
{
    GT_U32      value;      /* value to write into register     */
    GT_U32      regAddr;    /* register address                 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(trafClass);
    CPSS_NULL_PTR_CHECK_MAC(numberPtr);

    PRV_CPSS_DXCH_PORT_TC_DESCR_COUNT_REG_MAC(devNum,portNum,trafClass,&regAddr);

    if(prvCpssDxChHwPpPortGroupGetRegField(devNum,PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr,0,14,&value) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    *numberPtr = (GT_U16)value;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTx4TcBufNumberGet
*
* DESCRIPTION:
*       Gets the current number of buffers allocated on specified port
*       for specified Traffic Class queues.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical or CPU port number
*       trafClass - trafiic class (0..7)
*
* OUTPUTS:
*       numPtr    - (pointer to) the number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTx4TcBufNumberGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_U8       trafClass,
    OUT GT_U16      *numPtr
)
{
    GT_U32      value;      /* value to write into register     */
    GT_U32      regAddr;    /* register address                 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(trafClass);
    CPSS_NULL_PTR_CHECK_MAC(numPtr);

    PRV_CPSS_DXCH_PORT_TC_BUFFER_COUNT_REG_MAC(devNum,portNum,trafClass,&regAddr);

    if(prvCpssDxChHwPpPortGroupGetRegField(devNum,PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr,0,14,&value) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    *numPtr = (GT_U16)value;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxToCpuShaperModeSet
*
* DESCRIPTION:
*       Set Shaper mode packet or byte based for CPU port shapers
*       Shapers are configured by cpssDxChPortTxShaperProfileSet or
*       cpssDxChPortTxQShaperProfileSet
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion.
*
* INPUTS:
*       devNum  - physical device number
*       mode    - shaper mode: byte count or packet number based ackets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxToCpuShaperModeSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_SHAPER_MODE_ENT   mode
)
{
    GT_U32 hwCpuPortTbMode; /* The CPU Port Token bucket Rate shaper mode  */
    GT_U32 regAddr;         /* register address                 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,CPSS_LION_E);

    switch (mode)
    {
        case CPSS_PORT_TX_DROP_SHAPER_BYTE_MODE_E:
            hwCpuPortTbMode = 0;
            break;
        case CPSS_PORT_TX_DROP_SHAPER_PACKET_MODE_E:
            hwCpuPortTbMode = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueMiscCtrl;

    /* The CPU Port Token bucket Rate shaper mode */
    return prvCpssDxChHwPpSetRegField(devNum, regAddr, 2, 1, hwCpuPortTbMode);
}

/*******************************************************************************
* cpssDxChPortTxToCpuShaperModeGet
*
* DESCRIPTION:
*       Get Shaper mode packet or byte based for CPU port shapers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion.
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       modePtr - (pointer to) shaper mode: byte count or packet number based ackets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxToCpuShaperModeGet
(
    IN  GT_U8                               devNum,
    OUT CPSS_PORT_TX_DROP_SHAPER_MODE_ENT  *modePtr
)
{
    GT_U32 hwCpuPortTbMode; /* The CPU Port Token bucket Rate shaper mode */
    GT_U32 regAddr;         /* register address */
    GT_STATUS   rc;         /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,CPSS_LION_E);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueMiscCtrl;

    /* The CPU Port Token bucket Rate shaper mode */
    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 2, 1, &hwCpuPortTbMode);
    if (rc != GT_OK)
        return rc;

    *modePtr = (hwCpuPortTbMode == 1) ? CPSS_PORT_TX_DROP_SHAPER_PACKET_MODE_E:
        CPSS_PORT_TX_DROP_SHAPER_BYTE_MODE_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxSharingGlobalResourceEnableSet
*
* DESCRIPTION:
*       Enable/Disable sharing of resources for enqueuing of packets.
*       The shared resources configuration is set by
*       cpssDxChPortTxSharedGlobalResourceLimitsSet
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Lion.
*
* INPUTS:
*       devNum  - physical device number
*       enable  - GT_TRUE   - enable TX queue resourses sharing
*                 GT_FALSE - disable TX queue resources sharing
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSharingGlobalResourceEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
)
{
    GT_U32 regAddr;         /* register address                     */
    GT_U32 value;           /* value to be written to the register  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_LION_E);

    value = BOOL2BIT_MAC(enable);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            egrTxQConf.txQueueResSharingAndTunnelEgrFltr;

    /* Set <ResourceShareEn> in the Transmit Queue Resource Sharing Register */
    return prvCpssDxChHwPpSetRegField(devNum, regAddr, 24, 1, value);
}


/*******************************************************************************
* cpssDxChPortTxSharingGlobalResourceEnableGet
*
* DESCRIPTION:
*       Get enable/disable sharing of resources for enqueuing of packets.
*       The shared resources configuration is set by
*       cpssDxChPortTxSharedGlobalResourceLimitsSet
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Lion.
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE   - enable TX queue resourses sharing
*                    GT_FALSE - disable TX queue resources sharing
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSharingGlobalResourceEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL    *enablePtr
)
{
    GT_U32 regAddr;         /* register address                     */
    GT_U32 value;           /* value to be written to the register  */
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_LION_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* Get <ResourceShareEn> in the Transmit Queue Resource Sharing Register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    egrTxQConf.txQueueResSharingAndTunnelEgrFltr;
    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 24, 1, &value);
    if( GT_OK != rc )
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxSharedGlobalResourceLimitsSet
*
* DESCRIPTION:
*       Configuration of shared resources for enqueuing of packets.
*       The using of shared resources is set
*       by cpssDxChPortTxSharingGlobalResourceEnableSet
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Lion.
*
* INPUTS:
*       devNum             - physical device number
*       sharedBufLimit     - The number of buffers in all Transmit queues that
*                            can be shared between eligible packets.
*                            For DxCh2: range 0..4095
*                            For DxCh3,xCat,xCat2: range 0..16380
*       sharedDescLimit    - The number of descriptors that can be shared
*                            between all eligible packets.
*                            For DxCh2: range 0..4095
*                            For DxCh3,xCat,xCat2: range 0..16380
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For DxCh3,xCat,xCat2: sharedBufLimit and sharedDescLimit are rounded up
*       to the nearest multiple of 4 since the corresponding fields in the
*       Transmit Queue Resource Sharing register are in 4 buffers\descriptors
*       resolution.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSharedGlobalResourceLimitsSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  sharedBufLimit,
    IN  GT_U32  sharedDescLimit
)
{
    GT_U32 regAddr;             /* register address                     */
    GT_U32 data;                /* data to be written to the register   */
    GT_U32 sharedBufLimitInt;   /* local use of sharedBufLimit value    */
    GT_U32 sharedDescLimitInt;  /* local use of sharedDescLimit value   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_LION_E);

    /* for CH2 no change */
    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH2_E)
    {
        sharedBufLimitInt  = sharedBufLimit;
        sharedDescLimitInt = sharedDescLimit;
    }
    else /* for CH3 and above - 4 buffers\descriptors resolution */
    {
        sharedBufLimitInt  = (sharedBufLimit + 3) >> 2;
        sharedDescLimitInt = (sharedDescLimit + 3) >> 2;
    }
    /* check validity of the input parameters */
    if ((sharedBufLimitInt > 0xFFF) || (sharedDescLimitInt > 0xFFF))
    {
        return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            egrTxQConf.txQueueResSharingAndTunnelEgrFltr;

    data = (sharedBufLimitInt << 12) | sharedDescLimitInt;

    /* Set <Share DescLimit> and <Share BufLimit> in the
       Transmit Queue Resource Sharing Register */
    return prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 24, data);
}


/*******************************************************************************
* cpssDxChPortTxSharedGlobalResourceLimitsGet
*
* DESCRIPTION:
*       Get the configuration of shared resources for enqueuing of packets.
*       The using of shared resources is set
*       by cpssDxChPortTxSharingGlobalResourceEnableSet
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Lion.
*
* INPUTS:
*       devNum             - physical device number
*
* OUTPUTS:
*       sharedBufLimitPtr  - (pointer to) The number of buffers in all
*                            Transmit queues that
*                            can be shared between eligible packets.
*       sharedDescLimitPtr - (pointer to) The number of descriptors that
*                            can be shared between all eligible packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSharedGlobalResourceLimitsGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *sharedBufLimitPtr,
    OUT GT_U32  *sharedDescLimitPtr
)
{
    GT_U32 regAddr;             /* register address                     */
    GT_U32 data;                /* data to be written to the register   */
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_LION_E);

    CPSS_NULL_PTR_CHECK_MAC(sharedBufLimitPtr);
    CPSS_NULL_PTR_CHECK_MAC(sharedDescLimitPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                  egrTxQConf.txQueueResSharingAndTunnelEgrFltr;
    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 24, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* for CH2 no change */
    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH2_E)
    {
        *sharedDescLimitPtr = data & 0xFFF;;
        *sharedBufLimitPtr  = (data >> 12) & 0xFFF;
    }
    else /* for CH3 and above - 4 buffers\descriptors resolution */
    {
        *sharedDescLimitPtr = (data & 0xFFF) << 2 ;
        *sharedBufLimitPtr  = ((data >> 12) & 0xFFF) << 2;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxSharedPolicySet
*
* DESCRIPTION:
*       Sets shared pool allocation policy for enqueuing of packets.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum     - device number.
*       policy     - shared pool allocation policy.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or wrong policy
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSharedPolicySet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PORT_TX_SHARED_POLICY_ENT  policy
)
{
    GT_U32  value;      /* value to set to register */
    GT_U32  regAddr;    /* register address         */
    GT_U32  offset;     /* offset within register   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    switch (policy)
    {
        case CPSS_DXCH_PORT_TX_SHARED_POLICY_UNCONSTRAINED_E:
            value = 0;
            break;
        case CPSS_DXCH_PORT_TX_SHARED_POLICY_CONSTRAINED_E:
            value = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.tailDrop.config.config;
        offset = 4;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.
                                    txQueueResSharingAndTunnelEgrFltr;
        offset = 29;
    }

    return prvCpssDxChHwPpSetRegField(devNum, regAddr, offset, 1, value);
}

/*******************************************************************************
* cpssDxChPortTxSharedPolicyGet
*
* DESCRIPTION:
*       Gets shared pool allocation policy for enqueuing of packets.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       policyPtr  - (pointer to) shared pool allocation policy.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSharedPolicyGet
(
    IN  GT_U8                                devNum,
    OUT CPSS_DXCH_PORT_TX_SHARED_POLICY_ENT  *policyPtr
)
{
    GT_U32      value;      /* value to read from the register  */
    GT_U32      regAddr;    /* register address                 */
    GT_U32      offset;     /* offset within register           */
    GT_STATUS   rc;         /* function return value            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(policyPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.tailDrop.config.config;
        offset = 4;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.
                                    txQueueResSharingAndTunnelEgrFltr;
        offset = 29;
    }

    rc =  prvCpssDxChHwPpGetRegField(devNum, regAddr, offset, 1, &value);

    if(rc != GT_OK)
    {
        return rc;
    }

    *policyPtr = (value == 1) ? CPSS_DXCH_PORT_TX_SHARED_POLICY_CONSTRAINED_E :
                                CPSS_DXCH_PORT_TX_SHARED_POLICY_UNCONSTRAINED_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxWatchdogGet
*
* DESCRIPTION:
*       Get Tx Port Watchdog status and timeout
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical or CPU port number
*
* OUTPUTS:
*       enablePtr     - (pointer to) the enable / disable state
*       timeoutPtr    - (pointer to) the watchdog timeout
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - enablePtr or timeoutPtr is a null pointer
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxWatchdogGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL     *enablePtr,
    OUT GT_U16      *timeoutPtr
)
{
    GT_U32      regAddr;    /* register address                 */
    GT_U32      value;      /* value to write into register     */
    GT_STATUS   rc;         /* function return value            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(timeoutPtr);

    if(portNum == CPSS_CPU_PORT_NUM_CNS)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txCpuRegs.wdTxFlush;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txPortRegs[portNum].wdTxFlush;
    }

    rc = prvCpssDxChHwPpGetRegField(devNum,regAddr, 0, 16, &value);

    *enablePtr = BIT2BOOL_MAC(value & 1);
    *timeoutPtr = (GT_U16)((value & 0xFFF0) >> 4);

    return rc;
}

/*******************************************************************************
* cpssDxChPortTxWatchdogEnableSet
*
* DESCRIPTION:
*       Enable/Disable Watchdog on specified port of specified port of
*       specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical or CPU port number
*       enable  - GT_TRUE, enable Watchdog
*                 GT_FALSE, disable Watchdog
*       timeout - Watchdog timeout in milliseconds (1..4K ms)
*                 If disabling the Watchdog, this parameter is ignored
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxWatchdogEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable,
    IN  GT_U16    timeout
)
{
    GT_U32      regAddr;    /* register address                 */
    GT_U32      value;      /* value to write into register     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    if (enable == GT_TRUE)
    {
        if(timeout == 0 || timeout >= BIT_12)
        {
            /* HW support 12 bits -- HW not support 0 */
            return GT_OUT_OF_RANGE;
        }
        /* 3 bits of 1,2,3 are reserved and need to keep then 0 */
        value = 1 | ((timeout & 0xFFF) << 4);
    }
    else
    {
        value = 0;
    }

    if(portNum == CPSS_CPU_PORT_NUM_CNS)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txCpuRegs.wdTxFlush;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txPortRegs[portNum].wdTxFlush;
    }

    return prvCpssDxChHwPpSetRegField(devNum,regAddr, 0, 16, value);
}

/***********internal functions*************************************************/

/*******************************************************************************
* prvDxChShaperDisable
*
* DESCRIPTION:
*       Disable Token Bucket rate shaping on specified port of
*       specified device or
*       Disable shaping of transmitted traffic from a specified Traffic
*       Class Queue and specified port of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                - physical device number
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       tokenBucketRegAddr    - token bucket register address
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong device number
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       1. Read and store bucket's parameters
*       2. Set maximal parameters to the bucket
*       3. Read MTU from register
*       4. Delay some microseconds
*       5. Disable shaper
*       6. Restore previous values
*
*******************************************************************************/
static GT_STATUS prvDxChShaperDisable
(
    IN  GT_U8   devNum,
    IN  GT_U32  portGroupId,
    IN  GT_U32  tokenBucketRegAddr
)
{
    GT_U32      oldValue;    /* old value of Token Bucket Register */
    GT_U32      data;        /* data to write */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    /* Disabling the Egress Rate Shaper under traffic may hang its
       relevant transmit queue. (FEr#47) */
    if(PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_TOKEN_BUCKET_RATE_SHAPING_DISABLE_WA_E) == GT_FALSE)
    {
        /* no WA needed just disable the shaper */
        return prvCpssDxChHwPpPortGroupSetRegField(devNum, portGroupId, tokenBucketRegAddr, 0, 1, 0);
    }

    /* read and store bucket parameters */
    if(prvCpssDxChHwPpPortGroupReadRegister(devNum, portGroupId, tokenBucketRegAddr, &oldValue) != GT_OK)
    {
        return GT_FAIL;
    }

    /* Set Number Tokens and Bucket Size to a maximal values - 0xFFF
       and disable slow rate */
    data = (oldValue & 0xFFFFFFFD) | (0xFFFFFF << 4);

    if (prvCpssDxChHwPpPortGroupWriteRegister(devNum, portGroupId, tokenBucketRegAddr, data) != GT_OK)
    {
        return GT_FAIL;
    }

    /* Token bucket update to new maximal values will be configured by next
       pulse of token bucket update clock. It's need to be done delay to
       disable token bucket to guaranty maximal values update.
       The delay need to be for time more then maximal token bucket update
       period - 15360 core clock cycles. For Core Clock 144MHz delay should be
       more then 100 micro seconds. */
    cpssOsTimerWkAfter(1);

    /* disable shaping */
    if (prvCpssDxChHwPpPortGroupSetRegField(devNum, portGroupId, tokenBucketRegAddr, 0, 1, 0) != GT_OK)
    {
        return GT_FAIL;
    }

    /* restore old values */
    oldValue &= ~1;

    return prvCpssDxChHwPpPortGroupWriteRegister(devNum, portGroupId, tokenBucketRegAddr, oldValue);
}

/*******************************************************************************
* prvDxChPortTxCalcShaperTokenBucketRate
*
* DESCRIPTION:
*       Get shaper parameters for token bucket per port / per port per queue:
*       - Token refill value.
*       - slow-rate mode.
*       - tokens rate for XG/GIG.
*       - slow rate ratio.
*  The Formula is:
*                 <Core Clock Frequency in Kbps> * <Token refill value in bits>
*  Rate in Kbps = -------------------------------------------------------------
*                               <Refill period in clock cycles>
*
*  Refill period for 1000/100/10Mbps ports (GIG) is:
*  Refill period in clock cycles = <TriSpeed ports tokens rate> * 128
*                                  * <Slow Rate>
*  Where <Slow Rate> is:
*  Slow Rate = [1 - <Slow Rate mode>] + [<Slow Rate mode> * <Slow Rate Ratio>]
*  Where <Slow Rate mode> is: 0 for disabled, 1 for enabled.
*
*  Refill period for HyperG.Stack ports (XG) is:
*  Refill period in clock cycles = <XG ports tokens rate> * 8
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                    - physical device number
*       portNum                   - physical port number
*       xgPortsTokensRate        - tokens rate for XG ports' shapers
*       gigPortsTokensRate       - tokens rate for Tri Speed ports' shapers
*       gigPortsSlowRateRatio     - slow rate ratio for Tri Speed ports. Tokens Update
*                                   rate for ports with slow rate divided to the ratio.
*       tokenRefillValue          - number of tokens added to the bucket on each update
*                                   cycle.
*       slowRateEn                - Enable the division of <gigPortsTokensRate> by
*                                   <gigPortsSlowRateRatio>.
*       stackAsGig                - GT_TRUE, stacking ports shapers operates
*                                   as Tri Speed ports' shapers.
*                                   GT_FALSE, stacking ports shapers operates as XG ports.
*
* OUTPUTS:
*       tokenBucketRatePtr       - (pointer to) the shaping rate - actual Rate in Kbps.
*
* RETURNS:
*       GT_OK        - on success
*
* COMMENTS:
*       1.If the Token Bucket rate shaping is disabled then shaping rate returned
*       is set to 0.
*       For port number in the range 0..23, whether G or XG type, the global
*       shaper used parameters are those referenced as gig ones -
*       "gigPortsTokensRate" and "gigPortsSlowRateRatio" inputs for
*       "cpssDxChPortTxShaperGlobalParamsSet" API.
*       2. Txq revision 1 is not supported by this function.
*
*******************************************************************************/
static GT_STATUS prvDxChPortTxCalcShaperTokenBucketRate
(
    IN    GT_U8                   devNum,
    IN    GT_U8                   portNum,
    IN    GT_U32                  xgPortsTokensRate,
    IN    GT_U32                  gigPortsTokensRate,
    IN    GT_U32                  gigPortsSlowRateRatio,
    IN    GT_U32                  tokenRefillValue,
    IN    GT_U32                  slowRateEn,
    IN    GT_BOOL                 stackAsGig,
    OUT   GT_U32                 *tokenBucketRatePtr
)
{
    GT_U32      rateDividor;    /* rate dividor due to token refill period and */
                                /* the slow rate (if enabled). */
    GT_U32      coreClockInK;   /* clock rate in Khz */

    coreClockInK = PRV_CPSS_PP_MAC(devNum)->coreClock * 1000;

    if ( (portNum != CPSS_CPU_PORT_NUM_CNS) && (portNum > 23) &&
         (stackAsGig == GT_FALSE) &&
         (CPSS_PP_FAMILY_DXCH_LION_E != PRV_CPSS_PP_MAC(devNum)->devFamily) )
    {
        /* no slow rate support */
        rateDividor = xgPortsTokensRate * 8;
    }
    else
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            rateDividor = gigPortsTokensRate * 8;
        }
        else
        {
            rateDividor = gigPortsTokensRate * 128;
        }

        if( slowRateEn )
        {
            rateDividor = rateDividor * gigPortsSlowRateRatio;
        }
    }

    *tokenBucketRatePtr = tokenRefillValue * coreClockInK / rateDividor ;

    /* round up the result */
    if (tokenRefillValue * coreClockInK % rateDividor)
    {
        *tokenBucketRatePtr += 1;
    }

    return GT_OK;
}

/*******************************************************************************
* prvDxChPortTxCalcShaperTokenBucketRate_rev1
*
* DESCRIPTION:
*       Get shaper parameters for token bucket per port / per port per queue:
*       - Token refill value.
*       - slow-rate mode.
*       - tokens rate for XG/GIG.
*       - slow rate ratio.
*  The Formula is:
*                 <Core Clock Frequency in Kbps> * <Token refill value in bits>
*  Rate in Kbps = -------------------------------------------------------------
*                               <Refill period in clock cycles>
*
*  Refill period in clock cycles when slow rate is enabled =
*    <tokensRate> * <tokensRateGran> * <Slow Rate>
*  Refill period in clock cycles when slow rate is disabled =
*    <tokensRate> * <tokensRateGran> * 2 ^ < tbUpdateRatio >
*
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum             - physical device number
*       portNum            - port number
*       shaperConfigPtr    - (pointer to) shaper configuration
*       shaperMode         - shaper mode
*       slowRateEn         - slow rate enable value
*       tbUpdateRatio      - TB interval update ratio value
*       tokenRefillValue   - TB refill value
*
* OUTPUTS:
*       tokenBucketRatePtr - (pointer to) the actual Rate in Kbps or pps.
*
* RETURNS:
*       GT_OK        - on success
*
* COMMENTS:
*      Only Txq revision 1 is supported by this function.
*
*******************************************************************************/
static GT_STATUS prvDxChPortTxCalcShaperTokenBucketRate_rev1
(
    IN    GT_U8                   devNum,
    IN    GT_U8                   portNum,
    IN    CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC *shaperConfigPtr,
    IN    CPSS_PORT_TX_DROP_SHAPER_MODE_ENT   shaperMode,
    IN    GT_BOOL                 slowRateEn,
    IN    GT_U32                  tbUpdateRatio,
    IN    GT_U32                  tokenRefillValue,
    OUT   GT_U32                 *tokenBucketRatePtr
)
{
    GT_U32 refillPeriod;     /* Refill period in clock cycles */
    GT_U32 coreClockInK;     /* clock rate in Khz */
    GT_U32 packetLengthBits; /* packet length used in packet based shaping */
                             /* in bits */
    GT_U32 tempRate;

    if (tbUpdateRatio > 10)
    {
        return GT_FAIL;
    }
    packetLengthBits = ((portNum == CPSS_CPU_PORT_NUM_CNS) ?
                    shaperConfigPtr->cpuPacketLength :
                    shaperConfigPtr->portsPacketLength) * 8;

    coreClockInK = PRV_CPSS_PP_MAC(devNum)->coreClock * 1000;

    if (shaperConfigPtr->tokensRateGran == CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_64_CORE_CLOCKS_E)
    {
        refillPeriod = shaperConfigPtr->tokensRate * 8;
    }
    else
    {
        refillPeriod = shaperConfigPtr->tokensRate * 128;
    }

    if (slowRateEn)
    {
        refillPeriod *= shaperConfigPtr->slowRateRatio;
    }
    else
    {
        refillPeriod *= 1 << tbUpdateRatio;
    }

    *tokenBucketRatePtr = tokenRefillValue * coreClockInK / refillPeriod;

    /* round up the result */
    if ( (tokenRefillValue * coreClockInK) % refillPeriod)
    {
        *tokenBucketRatePtr += 1;
    }

    if (packetLengthBits != 0 && shaperMode == CPSS_PORT_TX_DROP_SHAPER_PACKET_MODE_E)
    {
        tempRate = *tokenBucketRatePtr;
        *tokenBucketRatePtr = tempRate * 1000 / packetLengthBits;

        /* round up the result */
        if ((tempRate * 1000) % packetLengthBits)
        {
            *tokenBucketRatePtr += 1;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxDhPortTxReCalcShaperTokenBucketRate
*
* DESCRIPTION:
*       Calculate shaping rate parameters (<Token refill value>, <slow-rate mode>)
*       for token bucket per port / per port per queue.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                    - physical device number
*       portNum                   - physical port number
*       xgPortsTokensRate        - tokens rate for XG ports' shapers
*       gigPortsTokensRate       - tokens rate for Tri Speed ports' shapers
*       gigPortsSlowRateRatio     - slow rate ratio for Tri Speed ports. Tokens Update
*                                   rate for ports with slow rate divided to the ratio.
*       stackAsGig                - GT_TRUE, stacking ports shapers operates
*                                   as Tri Speed ports' shapers.
*                                   GT_FALSE, stacking ports shapers operates as XG ports.
*       tokenBucketRate           - the shaping rate - actual Rate in Kbps.
*
* OUTPUTS:
*       tokenRefillValuePtr      - number of tokens added to the bucket on each update
*                                   cycle.
*       slowRateEnPtr            - Enable the division of <gigPortsTokensRate> by
*                                   <gigPortsSlowRateRatio>.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.For port number in the range 0..23, whether G or XG type, the global
*       shaper used parameters are those referenced as gig ones -
*       "gigPortsTokensRate" and "gigPortsSlowRateRatio" inputs for
*       "cpssDxChPortTxShaperGlobalParamsSet" API.
*       2. Txq revision 1 is not supported by this function.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortTxReCalcShaperTokenBucketRate
(
    IN    GT_U8                   devNum,
    IN    GT_U8                   portNum,
    IN    GT_U32                  xgPortsTokensRate,
    IN    GT_U32                  gigPortsTokensRate,
    IN    GT_U32                  gigPortsSlowRateRatio,
    IN    GT_BOOL                 stackAsGig,
    OUT   GT_U32                 *tokenRefillValuePtr,
    OUT   GT_U32                 *slowRateEnPtr,
    IN    GT_U32                  tokenBucketRate
)
{
    GT_U32      maxRate;            /* the maximal rate available when slow rate in disabled */
    GT_U32      newRate;            /* the rate that will be requested when max and min limits */
                                    /* are taken into consideration */
    GT_U32      minSlowRate;        /* the minimal rate available when slow rate is enabled */
    GT_U32      maxSlowRate;        /* the maximal rate available when slow rate is enabled */
    GT_U32      rateDividor;        /* rate dividor due to token refill period */
    GT_U32      slowRateDividor;    /* rate dividor due to token refill period and the slow rate */
    GT_U32      coreClockInK;       /* clock rate in Khz */
    GT_U64      tempResult;         /* temprory result for u64 calculations */

    coreClockInK = PRV_CPSS_PP_MAC(devNum)->coreClock * 1000;

    if ((portNum != CPSS_CPU_PORT_NUM_CNS) && (portNum > 23) &&
        (stackAsGig == GT_FALSE) &&
        (CPSS_PP_FAMILY_DXCH_LION_E != PRV_CPSS_PP_MAC(devNum)->devFamily))
    {
        /* no slow rate support therefore slowRate is same as Rate */
        rateDividor = slowRateDividor = xgPortsTokensRate * 8;
    }
    else
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            rateDividor = gigPortsTokensRate * 8;
        }
        else
        {
            rateDividor = gigPortsTokensRate * 128;
        }
        slowRateDividor = rateDividor * gigPortsSlowRateRatio;
    }

    minSlowRate = coreClockInK / slowRateDividor;

    /* the maximal tokens number is 0xFFF (4095) */
    maxRate = coreClockInK * 0xFFF / rateDividor;
    maxSlowRate = coreClockInK * 0xFFF / slowRateDividor;

    if (tokenBucketRate < minSlowRate)
    {
        /* this is the min rate that can be supported */
        newRate = minSlowRate;
        *slowRateEnPtr = 1;
    }
    else if (tokenBucketRate > maxRate)
    {
        /* this is the max rate that can be supported */
        newRate = maxRate;
        *slowRateEnPtr = 0;
    }
    else
    {
        newRate = tokenBucketRate;

        if (tokenBucketRate > maxSlowRate)
        {
            /* the *maxRate between maxSlowRate and maxRate */
            *slowRateEnPtr = 0;
        }
        else
        {
            /* the *maxRrate between minSlowRate and maxSlowRate */
            *slowRateEnPtr = 1;
        }
    }

    if (*slowRateEnPtr)
    {
        tempResult = prvCpssMathMul64(newRate,slowRateDividor);
    }
    else
    {
        tempResult = prvCpssMathMul64(newRate,rateDividor);
    }

    tempResult = prvCpssMathDiv64By16(tempResult,(GT_U16)(coreClockInK/1000));
    tempResult = prvCpssMathDiv64By16(tempResult,1000);

    if (tempResult.l[1])
    {
        return GT_FAIL;
    }

    *tokenRefillValuePtr = tempResult.l[0];

    if( 0 == *tokenRefillValuePtr )
        *tokenRefillValuePtr = 1;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortTxReCalcShaperTokenBucketRate_rev1
*
* DESCRIPTION:
*       Calculate shaping rate parameters for given tokenBucketRate:
*          1. TB refill value
*          2. slow rate enable value
*          3. TB interval update ratio value
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum             - physical device number
*       portNum            - physical port number
*       usePerTc           - token bucket per port or per queue
*       shaperConfigPtr    - (pointer to) shaper configuration.
*       shaperMode         - shaper mode
*       tokenBucketRate    - the shaping rate - actual Rate in Kbps.
*
* OUTPUTS:
*       slowRateEnPtr       - (pointer to) slow rate enable value
*       tbUpdateRatioPtr    - (pointer to) TB interval update ratio value
*       tokensPtr           - (pointer to) TB refill value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      Only Txq revision 1 is supported by this function.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortTxReCalcShaperTokenBucketRate_rev1
(
    IN  GT_U8                                 devNum,
    IN  GT_U8                                 portNum,
    IN  GT_BOOL                               usePerTc,
    IN  CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC   *shaperConfigPtr,
    IN  CPSS_PORT_TX_DROP_SHAPER_MODE_ENT     shaperMode,
    IN  GT_U32                                tokenBucketRate,
    OUT GT_BOOL                               *slowRateEnPtr,
    OUT GT_U32                                *tbUpdateRatioPtr,
    OUT GT_U32                                *tokensPtr
)
{
    GT_U32      rateDividor;        /* rate dividor due to token refill period */
    GT_U16      coreClockInM;       /* clock rate in Mhz */
    GT_U32      packetLengthBits;   /* packet length used in packet based shaping */
    GT_U32      TBIntervalUpdateRatio;
    GT_U64      tempResult;         /* used for temp calculation with U64 */
    GT_U32      temp;

    tempResult.l[0] = tempResult.l[1] = 0;

    packetLengthBits = ((portNum == CPSS_CPU_PORT_NUM_CNS) ?
                    shaperConfigPtr->cpuPacketLength :
                    shaperConfigPtr->portsPacketLength) * 8;

    if (packetLengthBits != 0 && shaperMode == CPSS_PORT_TX_DROP_SHAPER_PACKET_MODE_E)
    {
        tokenBucketRate = tokenBucketRate * packetLengthBits / 1000;
    }

    coreClockInM = (GT_U16)PRV_CPSS_PP_MAC(devNum)->coreClock;

    rateDividor = shaperConfigPtr->tokensRate *
     (shaperConfigPtr->tokensRateGran == CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_64_CORE_CLOCKS_E ?
                     8 : 128);

    TBIntervalUpdateRatio=11;
    do
    {
        TBIntervalUpdateRatio--;
        temp = ( 1 << TBIntervalUpdateRatio) * rateDividor;
        tempResult = prvCpssMathMul64(tokenBucketRate,temp);
        tempResult = prvCpssMathDiv64By16(tempResult,coreClockInM);
        tempResult = prvCpssMathDiv64By16(tempResult,1000);
    }
    while ((TBIntervalUpdateRatio != 0) && ((tempResult.l[1] > 0) ||
          (tempResult.l[0] > PRV_CPSS_DXCH_TXQREV1_TOKENS_REFILL_MAX_CNS)));

    /* check for minimum */
    if ((tempResult.l[0] == 0) && (tempResult.l[1] == 0))
    {
        tempResult.l[0] = 1;
    }

    /* check for maximum */
    if ((tempResult.l[1] > 0) || (tempResult.l[0] > PRV_CPSS_DXCH_TXQREV1_TOKENS_REFILL_MAX_CNS))
    {
        tempResult.l[0] = PRV_CPSS_DXCH_TXQREV1_TOKENS_REFILL_MAX_CNS;
        tempResult.l[1] = 0;

    }

    *slowRateEnPtr = GT_FALSE;
    *tokensPtr = tempResult.l[0];
    *tbUpdateRatioPtr = TBIntervalUpdateRatio;

    /* check if slowRate should be used */
    if(usePerTc == GT_TRUE ||
       (GT_FALSE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,PRV_CPSS_DXCH_LION_SLOW_RATE_ON_PORT_TB_NOT_SUPPORTED_WA_E)))
    {
        if ((1U << TBIntervalUpdateRatio) < shaperConfigPtr->slowRateRatio)
        {
            temp = shaperConfigPtr->slowRateRatio * rateDividor;
            tempResult = prvCpssMathMul64(tokenBucketRate,temp);
            tempResult = prvCpssMathDiv64By16(tempResult,coreClockInM);
            tempResult = prvCpssMathDiv64By16(tempResult,1000);

            if ((tempResult.l[1] == 0) && (tempResult.l[0] < PRV_CPSS_DXCH_TXQREV1_TOKENS_REFILL_MAX_CNS))
            {
                *slowRateEnPtr = GT_TRUE;
                *tokensPtr = tempResult.l[0];
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortTxShaperTokenBucketRateGet
*
* DESCRIPTION:
*       Get shaper parameters for token bucket per port / per port per queue:
*       - Token refill value.
*       - slow-rate mode.
*       - Bucket size.
*       - the shaping rate.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                    - physical device number
*       portNum                   - physical port number
*       usePerTc                  - token bucket per port or per queue
*       tcQueue                   - traffic class queue on this Physical Port
*       xgPortsTokensRate         - tokens rate for XG ports' shapers
*       gigPortsTokensRate        - tokens rate for Tri Speed ports' shapers
*       gigPortsSlowRateRatio     - slow rate ratio for Tri Speed ports. Tokens Update
*                                   rate for ports with slow rate divided to the ratio.
*       stackAsGig                - GT_TRUE, stacking ports shapers operates
*                                   as Tri Speed ports' shapers.
*                                   GT_FALSE, stacking ports shapers operates as XG ports.
*
* OUTPUTS:
*       tokenBucketEnPtr         - (pointer to) Bucket enable status.
*       tokenRefillValuePtr      - number of tokens added to the bucket on each update
*                                   cycle.
*       slowRateEnPtr            - Enable the division of <gigPortsTokensRate> by
*                                   <gigPortsSlowRateRatio>.
*       burstSizePtr             - burst size.
*       tokenBucketRatePtr       - the shaping rate - actual Rate in Kbps.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       1. If the Token Bucket rate shaping is disabled then shaping rate returned
*          is set to 0.
*       2. Txq revision 1 is not supported by this function.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortTxShaperTokenBucketRateGet
(
    IN    GT_U8                   devNum,
    IN    GT_U8                   portNum,
    IN    GT_BOOL                 usePerTc,
    IN    GT_U32                  tcQueue,
    IN    GT_U32                  xgPortsTokensRate,
    IN    GT_U32                  gigPortsTokensRate,
    IN    GT_U32                  gigPortsSlowRateRatio,
    IN    GT_BOOL                 stackAsGig,
    OUT   GT_BOOL                 *tokenBucketEnPtr,
    OUT   GT_U32                  *tokenRefillValuePtr,
    OUT   GT_U32                  *slowRateEnPtr,
    OUT   GT_U16                  *burstSizePtr,
    OUT   GT_U32                  *tokenBucketRatePtr
)
{
    GT_U32      regAddr;    /* register address                 */
    GT_U32      offset;     /* field offset                     */
    GT_U32      len;
    GT_U32      value;      /* value to write into register     */

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* cannot devide in zero. see prvDxChPortTxCalcShaperTokenBucketRate */
    if((xgPortsTokensRate == 0) || (gigPortsTokensRate == 0))
        return GT_BAD_PARAM;

    /* Get port Token bucket configuration register */
    if(usePerTc == GT_FALSE)
    {
        /* token bucket per port */
        PRV_CPSS_DXCH_PORT_TOKEN_BUCKET_REG_MAC(devNum,portNum,&regAddr);
    }
    else
    {
        /* token bucket per port per queue */
        PRV_CPSS_DXCH_PORT_Q_TOKEN_BUCKET_REG_MAC(devNum,portNum,tcQueue,&regAddr);
    }

    /* read the Token Bucket Configuration register values */
    offset = 0;
    len = 28;

    if (prvCpssDxChHwPpPortGroupGetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, offset, len, &value) != GT_OK)
        return GT_HW_ERROR;

    *tokenBucketEnPtr = (value & 0x1);
    *slowRateEnPtr = (value >>
                      (PRV_CPSS_DXCH_TOKEN_BUCKET_SLOW_RATE_EN_OFFSET_CNS - offset)) & 0x1;
    *tokenRefillValuePtr = (value >>
                            (PRV_CPSS_DXCH_TOKEN_BUCKET_TOKENS_OFFSET_CNS - offset)) & 0xFFF;
    *burstSizePtr = (GT_U16)((value >> 16) & 0xFFF);

    if (*tokenBucketEnPtr == GT_FALSE)
    {
        return GT_OK;
    }

    return prvDxChPortTxCalcShaperTokenBucketRate(devNum,
                                                 portNum,
                                                 xgPortsTokensRate,
                                                 gigPortsTokensRate,
                                                 gigPortsSlowRateRatio,
                                                 *tokenRefillValuePtr,
                                                 *slowRateEnPtr,
                                                 stackAsGig,
                                                 tokenBucketRatePtr);

}

/*******************************************************************************
* prvDxChPortTxSetShaperTokenBucketParams
*
* DESCRIPTION:
*       Set shaper paramters for token bucket per port / per port per queue.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                   - physical device number
*       portNum                  - physical port number
*       usePerTc                 - token bucket per port or per queue
*       tcQueue                  - traffic class queue on this Physical Port
*       old_xgPortsTokensRate    - old tokens rate for XG ports' shapers
*       old_gigPortsTokensRate   - old tokens rate for Tri Speed ports' shapers
*       old_gigPortsSlowRateRatio - old slow rate ratio for Tri Speed ports. Tokens
*                                   Update rate for ports with slow rate divided to
*                                   the ratio.
*       old_StackAsGig           - old stacking ports shaper mode
*       xgPortsTokensRate        - tokens rate for XG ports' shapers
*       gigPortsTokensRate       - tokens rate for Tri Speed ports' shapers
*       gigPortsSlowRateRatio    - slow rate ratio for Tri Speed ports. Tokens Update
*                                  rate for ports with slow rate divided to the ratio.
*       stackAsGig               - GT_TRUE, stacking ports shapers operates
*                                  as Tri Speed ports' shapers.
*                                  GT_FALSE, stacking ports shapers operates as XG ports.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       Txq revision 1 is not supported by this function.
*
*******************************************************************************/
static GT_STATUS prvDxChPortTxSetShaperTokenBucketParams
(
    IN    GT_U8                   devNum,
    IN    GT_U8                   portNum,
    IN    GT_BOOL                 usePerTc,
    IN    GT_U32                  tcQueue,
    IN    GT_U32                  old_xgPortsTokensRate,
    IN    GT_U32                  old_gigPortsTokensRate,
    IN    GT_U32                  old_gigPortsSlowRateRatio,
    IN    GT_BOOL                 old_StackAsGig,
    IN    GT_U32                  xgPortsTokensRate,
    IN    GT_U32                  gigPortsTokensRate,
    IN    GT_U32                  gigPortsSlowRateRatio,
    IN    GT_BOOL                 stackAsGig
)
{
    GT_U32      regAddr;    /* register address                 */
    GT_U32      offset;     /* field offset                     */
    GT_U32      len;
    GT_U32      value;      /* value to write into register     */
    GT_U32      tokenRefillValue;
    GT_U32      slowRateEn;
    GT_STATUS   rc;
    GT_BOOL     tokenBucketEn;
    GT_U32      tokenBucketRate;
    GT_U16      burstSize;

    rc = prvCpssDxChPortTxShaperTokenBucketRateGet(devNum,
                                                  portNum,
                                                  usePerTc,
                                                  tcQueue,
                                                  old_xgPortsTokensRate,
                                                  old_gigPortsTokensRate,
                                                  old_gigPortsSlowRateRatio,
                                                  old_StackAsGig,
                                                  &tokenBucketEn,
                                                  &tokenRefillValue,
                                                  &slowRateEn,
                                                  &burstSize,
                                                  &tokenBucketRate);

    if (rc != GT_OK)
    {
        return rc;
    }

    /* The function should do nothing in the case disabled bucket. */
    if (tokenBucketEn == 0)
    {
        return GT_OK;
    }
    /* calulate with new params */
    rc = prvCpssDxChPortTxReCalcShaperTokenBucketRate(devNum,
                                                 portNum,
                                                 xgPortsTokensRate,
                                                 gigPortsTokensRate,
                                                 gigPortsSlowRateRatio,
                                                 stackAsGig,
                                                 &tokenRefillValue,
                                                 &slowRateEn,
                                                 tokenBucketRate);
     if (rc != GT_OK)
     {
         return rc;
     }

    /* tokenRefillValue should never be 0. */
    if (tokenRefillValue == 0)
    {
        tokenRefillValue = 1;
    }

    /* Get port Token bucket configuration register */
    if(usePerTc == GT_FALSE)
    {
        PRV_CPSS_DXCH_PORT_TOKEN_BUCKET_REG_MAC(devNum,portNum,&regAddr);
    }
    else
    {
        PRV_CPSS_DXCH_PORT_Q_TOKEN_BUCKET_REG_MAC(devNum,portNum,tcQueue,&regAddr);
    }

    value = slowRateEn | ((tokenRefillValue & 0xFFF) << 3);
    offset = 1;
    len = 15;
    if (prvCpssDxChHwPpPortGroupSetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, offset, len, value) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    return GT_OK;
}


/*******************************************************************************
* prvDxChPortTxSetShaperTokenBucketParams_rev1
*
* DESCRIPTION:
*       Set shaper paramters for token bucket per port / per port per queue.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum             - physical device number
*       portNum            - physical port number
*       shaperMode         - shaper mode
*       usePerTc           - are we use 'per port' or 'per port per TC'
*       tcQueue            - traffic class queue on this Physical Port
*       oldShaperConfigPtr - (pointer to) old shaper global configurations
*       newShaperConfigPtr - (pointer to) new shaper global configurations
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*      Only Txq revision 1 is supported by this function.
*
*******************************************************************************/
static GT_STATUS prvDxChPortTxSetShaperTokenBucketParams_rev1
(
    IN GT_U8                               devNum,
    IN GT_U8                               portNum,
    IN CPSS_PORT_TX_DROP_SHAPER_MODE_ENT   shaperMode,
    IN GT_BOOL                             usePerTc,
    IN GT_U32                              tcQueue,
    IN CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC *oldShaperConfigPtr,
    IN CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC *newShaperConfigPtr
)
{
    PORT_TX_SHAPER_TOKEN_BUCKET_STC subEntry; /* sub entry format */
    GT_STATUS   rc;                /* return code */
    GT_U32      tokenBucketRate;   /* TB rate in Kbps or pps */
    GT_BOOL     slowRateEn;        /* slow rate enable */
    GT_U32      tbUpdateRatio;     /* TB interval update ratio value */
    GT_U32      tokenRefillValue;  /* TB refill value */

    rc = portTxShaperTokenBucketEntryRead_rev1(devNum,portNum,usePerTc,tcQueue,&subEntry);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* The function should do nothing in the case disabled bucket. */
    if(subEntry.tokenBucketEn == GT_FALSE)
    {
        return GT_OK;
    }

    /* calculate the rate */
    rc = prvDxChPortTxCalcShaperTokenBucketRate_rev1(devNum,
                                                     portNum,
                                                    oldShaperConfigPtr,
                                                    shaperMode,
                                                    subEntry.slowRateEn,
                                                    subEntry.tbUpdateRatio,
                                                    subEntry.tokens,
                                                    &tokenBucketRate);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* calulate per TB configurations with new global shaper configurations */
    rc = prvCpssDxChPortTxReCalcShaperTokenBucketRate_rev1(devNum,
                                                           portNum,
                                                           usePerTc,
                                                           newShaperConfigPtr,
                                                           shaperMode,
                                                           tokenBucketRate,
                                                           &slowRateEn,
                                                           &tbUpdateRatio,
                                                           &tokenRefillValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* update the TB entry */
    rc = prvCpssDxChPortTxShaperTokenBucketEntryWrite_rev1(devNum,portNum,
                                                           usePerTc,tcQueue,
                                                           slowRateEn,
                                                           tbUpdateRatio,
                                                           tokenRefillValue,
                                                           subEntry.maxBucketSize,
                                                           GT_FALSE, 0);
    if(rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}


/*******************************************************************************
* cpssDxChPortTxTailDropUcEnableSet
*
* DESCRIPTION:
*       Enable/Disable tail-dropping for all packets based on the profile limits.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   -  device number.
*       enable   -  GT_TRUE  - Tail Drop is enabled.
*                   GT_FALSE - The Tail Drop limits for all packets are
*                              ignored and packet is dropped only
*                              when the Tx Queue has reached its global
*                              descriptors limit.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTailDropUcEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_U32   tailDropDisable;     /* whether to disable tail drop for uc */
    GT_U32   regAddr;             /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        tailDropDisable = (enable == GT_TRUE)? 0 : 1;

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfig;

        return prvCpssDxChHwPpSetRegField(devNum, regAddr, 10, 1, tailDropDisable);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.tailDrop.config.config;

        return prvCpssDxChHwPpSetRegField(devNum, regAddr, 5, 1, BOOL2BIT_MAC(enable));
    }
}

/*******************************************************************************
* cpssDxChPortTxTailDropUcEnableGet
*
* DESCRIPTION:
*       Get enable/disable tail-dropping for all packets based on the profile limits.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      -  device number.
*
* OUTPUTS:
*       enablePtr   -  pointer to tail drop status:
*                          GT_TRUE  - Tail Drop is enabled.
*                          GT_FALSE - The Tail Drop limits for all packets are
*                                     ignored and packet is dropped only
*                                     when the Tx Queue has reached its global
*                                     descriptors limit.
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTailDropUcEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32      value;     /* whether to disable tail drop for uc         */
    GT_U32      regAddr;             /* register address */
    GT_STATUS   rc;                  /* function return value            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfig;

        rc =  prvCpssDxChHwPpGetRegField(devNum, regAddr, 10, 1, &value);

        *enablePtr = (value == 0) ? GT_TRUE : GT_FALSE;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.tailDrop.config.config;

        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 5, 1, &value);

        *enablePtr = BIT2BOOL_MAC(value);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChPortTxBufferTailDropEnableSet
*
* DESCRIPTION:
*       Enable/disable Tail Drop according to the number of buffers in the
*       queues.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion.
*
* INPUTS:
*       devNum   - device number.
*       enable   -  GT_TRUE  -  Enables Tail Drop according to the number of
*                               buffers in the queues. Tail drop use both
*                               decsriptiors and buffers limits.
*                   GT_FALSE - Tail drop use only descriptor limits.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxBufferTailDropEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_U32  value;            /* value to set to register  */
    GT_U32  regAddr;          /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,CPSS_LION_E);

    value = BOOL2BIT_MAC(enable);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfigExt;

    return prvCpssDxChHwPpSetRegField(devNum, regAddr, 23, 1, value);
}

/*******************************************************************************
* cpssDxChPortTxBufferTailDropEnableGet
*
* DESCRIPTION:
*       Get enable/disable Tail Drop status according to the number of buffers
*       in the queues.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion.
*
* INPUTS:
*       devNum      - device number.
*
* OUTPUTS:
*       enablePtr   -  pointer to Tail Drop status:
*                       GT_TRUE  - Enables Tail Drop according to the number of
*                                  buffers in the queues. Tail drop use both
*                                  decsriptiors and buffers limits.
*                       GT_FALSE - Tail drop use only descriptor limits.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxBufferTailDropEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32      value;     /* value to read from the register  */
    GT_U32      regAddr;   /* register address */
    GT_STATUS   rc;        /* function return value            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,CPSS_LION_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfigExt;

    rc =  prvCpssDxChHwPpGetRegField(devNum, regAddr, 23, 1, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxBuffersSharingMaxLimitSet
*
* DESCRIPTION:
*       Sets the maximal number of shared buffers in a Tail Drop system.
*       When the total number of buffers exceeds this threshold, all of the
*       shared buffers are currently used and packets are queued according to
*       their target queue guaranteed limits.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Lion.
*
* INPUTS:
*       devNum  - physical device number
*       limit   - maximal number of shared buffers in a Tail Drop system.
*                 For DxCh2: range 0..0xFFF
*                          To disable Buffers Sharing for Transmit queues
*                          set this field to 0xFFF.
*                 For DxCh3,xCat,xCat2: range 0..0x3FFF
*                          To disable Buffers Sharing for Transmit queues
*                          set this field to 0.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxBuffersSharingMaxLimitSet
(
    IN  GT_U8       devNum,
    IN  GT_U32      limit
)
{
    GT_U32 regAddr;          /* register address */
    GT_U32 fieldLength;      /* The number of bits to be written to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_LION_E);

    if(PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
    {
        if(limit > 0x3FFF)
        {
            return GT_OUT_OF_RANGE;
        }
        fieldLength = 14;

    }
    else
    {
        if(limit > 0xFFF)
        {
            return GT_OUT_OF_RANGE;
        }
        fieldLength = 12;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.bufMngSharedBufConfigReg;

    return prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, fieldLength, limit);

}

/*******************************************************************************
* cpssDxChPortTxBuffersSharingMaxLimitGet
*
* DESCRIPTION:
*       Gets the maximal number of shared buffers in a Tail Drop system.
*       When the total number of buffers exceeds this threshold, all of the
*       shared buffers are currently used and packets are queued according to
*       their target queue guaranteed limits.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Lion.
*
* INPUTS:
*       devNum  - physical device number
*
*
* OUTPUTS:
*       limitPtr   -  pointer to maximal number of shared buffers
*                     in a Tail Drop system
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxBuffersSharingMaxLimitGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *limitPtr
)
{
    GT_U32    regAddr;        /* register address */
    GT_U32    fieldLength;    /* The number of bits to be written to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_LION_E);

    CPSS_NULL_PTR_CHECK_MAC(limitPtr);

    if(PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
    {
        fieldLength = 14;
    }
    else
    {
        fieldLength = 12;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.bufMngSharedBufConfigReg;

    return prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, fieldLength, limitPtr);
}

/*******************************************************************************
* cpssDxChPortTxDp1SharedEnableSet
*
* DESCRIPTION:
*       Enable/Disable packets with DP1 (Red) to use the shared
*       descriptors / buffers pool.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; Lion.
*
* INPUTS:
*       devNum  - physical device number
*       enable  - GT_TRUE   - Allow DP1 (Red) in shared pool.
*                 GT_FALSE  - Disallow DP1 to be shared.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxDp1SharedEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
)
{
    GT_U32      value;      /* value to write */
    GT_U32      regAddr;    /* register address */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_LION_E);

    /* write enable value */
    value = BOOL2BIT_MAC(enable);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueDpToCfiReg;

    return prvCpssDxChHwPpSetRegField(devNum, regAddr, 5, 1, value);
}

/*******************************************************************************
* cpssDxChPortTxDp1SharedEnableGet
*
* DESCRIPTION:
*       Gets current status of  shared  descriptors / buffer pool usage
*       for packets with DP1 (Red).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; Lion.
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       enablePtr  - pointer to current status of
*                    shared  descroptors / Buffers pool usage:
*                  - GT_TRUE   - Allow DP1 (Red) in shared pool.
*                  - GT_FALSE  - Allow DP0 (Green) only in shared pool.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxDp1SharedEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32      value;      /* register value */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_LION_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueDpToCfiReg;
    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 5, 1, &value);
    if( GT_OK != rc )
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxTcSharedProfileEnableSet
*
* DESCRIPTION:
*       Enable/Disable usage of the shared  descriptors / buffer pool for
*       packets with the traffic class (tc) that are transmited via a
*       port that is associated with the Profile (pfSet).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum   - physical device number
*       pfSet    - the Profile Set in which the Traffic
*                  Class Drop Parameters is associated
*       tc       - the Traffic Class, range 0..7.
*       enableMode  - Drop Precedence (DPs) enabled mode for sharing.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, pfSet
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTcSharedProfileEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT       pfSet,
    IN  GT_U8                                   tc,
    IN  CPSS_PORT_TX_SHARED_DP_MODE_ENT         enableMode
)
{
    GT_U32      regAddr;    /* register address                 */
    GT_U32      offset;     /* field offset                     */
    GT_U32      value;      /* value to write into register     */
    GT_U32      fieldLen;   /* register field length            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tc);
    PRV_CPSS_DXCH_DROP_PROFILE_CHECK_MAC(pfSet, devNum);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* write enable value */
        switch(enableMode)
        {
            case CPSS_PORT_TX_SHARED_DP_MODE_DISABLE_E: value = 0;
                                                        break;
            case CPSS_PORT_TX_SHARED_DP_MODE_ALL_E:     value = 1;
                                                        break;
            default: return GT_BAD_PARAM;
        }

        /* Shared priority register0: tc 0 - 3,
           Shared priority register1: tc 4 - 7 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            egrTxQConf.txQueueSharedPriorityReg[pfSet/4];

        offset = tc + (pfSet % 4) * 8;

        fieldLen = 1;
    }
    else
    {
        switch(enableMode)
        {
            case CPSS_PORT_TX_SHARED_DP_MODE_ALL_E:     value = 2;
                                                        break;
            case CPSS_PORT_TX_SHARED_DP_MODE_DISABLE_E: value = 3;
                                                        break;
            case CPSS_PORT_TX_SHARED_DP_MODE_DP0_E:     value = 0;
                                                        break;
            case CPSS_PORT_TX_SHARED_DP_MODE_DP0_DP1_E: value = 1;
                                                        break;
            default: return GT_BAD_PARAM;
        }

        offset = pfSet * 2;

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.
                        tailDrop.config.tcProfileEnableSharedPoolUsage[tc];

        fieldLen = 2;
    }

    return prvCpssDxChHwPpSetRegField(devNum, regAddr, offset, fieldLen, value);
}

/*******************************************************************************
* cpssDxChPortTxTcSharedProfileEnableGet
*
* DESCRIPTION:
*       Gets usage of the shared  descriptors / buffer pool status for
*       packets with the traffic class (tc) that are transmited via a
*       port that is associated with the Profile (pfSet).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum   - physical device number
*       pfSet    - the Profile Set in which the Traffic
*                  Class Drop Parameters is associated
*       tc       - the Traffic Class, range 0..7.
*
* OUTPUTS:
*       enableModePtr   - (pointer to) Drop Precedence (DPs) enabled mode for
*                         sharing.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, pfSet
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTcSharedProfileEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT       pfSet,
    IN  GT_U8                                   tc,
    OUT CPSS_PORT_TX_SHARED_DP_MODE_ENT         *enableModePtr
)
{
    GT_U32      regAddr;    /* register address                 */
    GT_U32      value;      /* value to write into register     */
    GT_U32      offset;     /* field offset                     */
    GT_STATUS   rc;         /* function return value            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tc);
    PRV_CPSS_DXCH_DROP_PROFILE_CHECK_MAC(pfSet, devNum);
    CPSS_NULL_PTR_CHECK_MAC(enableModePtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {

        /* Shared priority register0: tc 0 - 3,
           Shared priority register1: tc 4 - 7 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            egrTxQConf.txQueueSharedPriorityReg[pfSet/4];

        offset = tc + (pfSet % 4) * 8;

        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, offset, 1, &value);
        if( GT_OK != rc )
        {
            return rc;
        }

        *enableModePtr = (0 == value) ? CPSS_PORT_TX_SHARED_DP_MODE_DISABLE_E :
                                        CPSS_PORT_TX_SHARED_DP_MODE_ALL_E ;
    }
    else
    {
        offset = pfSet * 2;

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.
                        tailDrop.config.tcProfileEnableSharedPoolUsage[tc];

        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, offset, 2, &value);
        if( GT_OK != rc )
        {
            return rc;
        }

        switch(value)
        {
            case 0: *enableModePtr = CPSS_PORT_TX_SHARED_DP_MODE_DP0_E;
                    break;
            case 1: *enableModePtr = CPSS_PORT_TX_SHARED_DP_MODE_DP0_DP1_E;
                    break;
            case 2: *enableModePtr = CPSS_PORT_TX_SHARED_DP_MODE_ALL_E;
                    break;
            case 3: *enableModePtr = CPSS_PORT_TX_SHARED_DP_MODE_DISABLE_E;
                    break;
            default: return GT_BAD_PARAM;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxShaperTokenBucketMtuSet
*
* DESCRIPTION:
*       Set Token bucket maximum transmission unit (MTU).
*       The token bucket MTU defines the minimum number of tokens required to
*       permit a packet to be transmitted (i.e., conforming).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion.
*
* INPUTS:
*       devNum   - physical device number
*       mtu      - MTU for egress rate shaper
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, mtu
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperTokenBucketMtuSet
(
    IN GT_U8                                  devNum,
    IN CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_ENT mtu
)
{
    GT_U32      regAddr;     /* register address */
    GT_U32      value;       /* value to write into register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,CPSS_LION_E);

    switch(mtu)
    {
        case CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_1_5K_E:
            value = 0;
            break;
        case CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_2K_E:
            value = 1;
            break;
        case CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_10K_E:
            value = 2;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* Set Token bucket maximum transmission unit */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfigExt;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 3, 2, value);
}

/*******************************************************************************
* cpssDxChPortTxShaperTokenBucketMtuGet
*
*       Get Token bucket maximum transmission unit (MTU).
*       The token bucket MTU defines the minimum number of tokens required to
*       permit a packet to be transmitted (i.e., conforming).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion.
*
* INPUTS:
*       devNum   - physical device number
*
* OUTPUTS:
*       mtuPtr   -  pointer to MTU for egress rate shaper
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperTokenBucketMtuGet
(
    IN  GT_U8                                  devNum,
    OUT CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_ENT *mtuPtr
)
{
    GT_U32      regAddr;    /* register address                */
    GT_U32      value;      /* value to read from register     */
    GT_STATUS   rc;         /* function return value           */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,CPSS_LION_E);

    CPSS_NULL_PTR_CHECK_MAC(mtuPtr);

    /* Get Token bucket maximum transmission unit */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfigExt;

    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 3, 2, &value);

    if (rc != GT_OK)
        return rc;

    switch(value)
    {
        case 0:
            *mtuPtr = CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_1_5K_E;
            break;
        case 1:
            *mtuPtr = CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_2K_E;
            break;
        case 2:
            *mtuPtr = CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_10K_E;
            break;
        default:
            return GT_BAD_STATE;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxRandomTailDropEnableSet
*
* DESCRIPTION:
*       Enable/disable Random Tail drop Threshold, to overcome synchronization.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; Lion.
*
* INPUTS:
*       devNum   - device number.
*       enable   -  GT_TRUE  -  Enable Random Tail drop Threshold.
*                   GT_FALSE -  Disable Random Tail drop Threshold.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid input paramters value
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxRandomTailDropEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_U32  value;            /* value to set to register  */
    GT_U32  regAddr;          /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_LION_E);

    value = BOOL2BIT_MAC(enable);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfigExt;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 30, 1, value);
}

/*******************************************************************************
* cpssDxChPortTxRandomTailDropEnableGet
*
* DESCRIPTION:
*       Get Random Tail drop Threshold status.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; Lion.
*
* INPUTS:
*       devNum      - device number.
*
* OUTPUTS:
*       enablePtr   -  pointer to Random Tail drop Threshold status:
*                       GT_TRUE  - Random Tail drop Threshold enabled.
*                       GT_FALSE - Random Tail drop Threshold disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid input paramters value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxRandomTailDropEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32      value;     /* value to read from the register  */
    GT_U32      regAddr;   /* register address */
    GT_STATUS   rc;        /* function return value            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_LION_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfigExt;

    rc =  prvCpssDxChHwPpGetRegField(devNum, regAddr, 30, 1, &value);
    if( GT_OK != rc )
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChTxPortShaperMaxValueSet
*
* DESCRIPTION:
*      Set Number Tokens and Bucket Size to a maximal values - 0xFFF
*      and disable slow rate
*
*
* INPUTS:
*       devNum              - physical device number.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       tokenBucketAddress  - token bucket register address
*
* OUTPUTS:
*       tokenBucketOldValuePtr - pointer to old port (tc) token bucket register value
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChTxPortShaperMaxValueSet
(
    IN  GT_U8    devNum,
    IN  GT_U32   portGroupId,
    IN  GT_U32   tokenBucketAddress,
    OUT GT_U32  *tokenBucketOldValuePtr
)
{
    GT_U32      data;       /* register data */
    GT_STATUS   rc;

    /* read and store bucket parameters */
    rc = prvCpssDxChHwPpPortGroupReadRegister(devNum, portGroupId, tokenBucketAddress, tokenBucketOldValuePtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Set Number Tokens and Bucket Size to a maximal values - 0xFFF
       and disable slow rate */
    data = (*tokenBucketOldValuePtr & 0xFFFFFFFD) | (0xFFFFFF << 4);

    return prvCpssDxChHwPpPortGroupWriteRegister(devNum, portGroupId, tokenBucketAddress, data);
}

/*******************************************************************************
* prvCpssDxChTxPortShaperDisable
*
* DESCRIPTION:
*      Disable shaping and restore old values
*
*
*
* INPUTS:
*       devNum     - physical device number.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       tokenBucketAddress  - token bucket register address
*       tokenBucketOldValue - old port (tc) token bucket register value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChTxPortShaperDisable
(
    IN  GT_U8    devNum,
    IN  GT_U32   portGroupId,
    IN  GT_U32   tokenBucketAddress,
    IN  GT_U32   tokenBucketOldValue
)
{
    GT_STATUS   rc;         /* return code */

    /* disable shaping */
    rc = prvCpssDxChHwPpPortGroupSetRegField(devNum, portGroupId, tokenBucketAddress, 0, 1, 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* restore old values */
    tokenBucketOldValue &= ~1;

    return prvCpssDxChHwPpPortGroupWriteRegister(devNum, portGroupId, tokenBucketAddress, tokenBucketOldValue);
}

/*******************************************************************************
* cpssDxChTxPortAllShapersDisable
*
* DESCRIPTION:
*       Disables all ports and queues shapers for specified device in minimum delay.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - physical device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTxPortAllShapersDisable
(
    IN  GT_U8    devNum
)
{
    GT_U8       port;       /* device port */
    GT_U8       queue;      /* port traffic class */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* return code */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */
    static GT_U32  portTbRegValue[PRV_CPSS_MAX_PP_PORTS_NUM_CNS];  /* port token bucket register value */
    static GT_U32  portTcTbRegValue[PRV_CPSS_MAX_PP_PORTS_NUM_CNS][8]; /* port, TC token bucket register value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_LION_TB_NOT_SUPPORTED_WA_E))
    {
        return GT_NOT_SUPPORTED;
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        for (port = 0; port < PRV_CPSS_MAX_PP_PORTS_NUM_CNS; port++)
        {
            if (!PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, port) && port != CPSS_CPU_PORT_NUM_CNS)
                continue;

            /* Get port Token bucket configuration register */
            PRV_CPSS_DXCH_PORT_TOKEN_BUCKET_REG_MAC(devNum, port, &regAddr);

            portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);

            /* Set Number Tokens and Bucket Size to a maximal values - 0xFFF
               and disable slow rate */
            rc = prvCpssDxChTxPortShaperMaxValueSet(devNum,
                                                    portGroupId,
                                                    regAddr,
                                                    &portTbRegValue[port]);
            /* Per port per traffic class (queue) */
            for (queue = 0; queue < CPSS_TC_RANGE_CNS; queue++)
            {
                PRV_CPSS_DXCH_PORT_Q_TOKEN_BUCKET_REG_MAC(devNum, port, queue, &regAddr);

                /* Set Number Tokens and Bucket Size to a maximal values - 0xFFF
                   and disable slow rate */
                rc = prvCpssDxChTxPortShaperMaxValueSet(devNum,
                                                        portGroupId,
                                                        regAddr,
                                                        &portTcTbRegValue[port][queue]);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
        }
        /* wait at least 1 millisecond to guaranty bucket size will be > MTU.
          this avoid traffic stuck during shaper disable under traffic.*/
        cpssOsTimerWkAfter(1);

        for (port = 0; port < PRV_CPSS_MAX_PP_PORTS_NUM_CNS; port++)
        {
            if (!PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, port) && port != CPSS_CPU_PORT_NUM_CNS)
                continue;

            PRV_CPSS_DXCH_PORT_TOKEN_BUCKET_REG_MAC(devNum, port, &regAddr);

            portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);

            /* Disable shaping and restore old values */
            rc = prvCpssDxChTxPortShaperDisable(devNum,
                                                portGroupId,
                                                regAddr,
                                                portTbRegValue[port]);
            if(rc != GT_OK)
            {
                return rc;
            }
            /* Per port per traffic class (queue) */
            for (queue = 0; queue < CPSS_TC_RANGE_CNS; queue++)
            {
                PRV_CPSS_DXCH_PORT_Q_TOKEN_BUCKET_REG_MAC(devNum, port, queue, &regAddr);

                /* Disable shaping and restore old values */
                rc = prvCpssDxChTxPortShaperDisable(devNum,
                                                    portGroupId,
                                                    regAddr,
                                                    portTcTbRegValue[port][queue]);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
        }
    }
    else
    {
        return portTxShaperTokenBucketAllShapersDisable_rev1(devNum);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTxPortShapersDisable
*
* DESCRIPTION:
*       Disable Shaping on Port and all it's queues.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - physical device number.
*       port    - physical or CPU port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTxPortShapersDisable
(
    IN  GT_U8    devNum,
    IN  GT_U8    port
)
{
    GT_U8       queue;      /* port traffic class */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* return code */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32      portTbRegValue;      /* port token bucket register value */
    GT_U32      portTcTbRegValue[8]; /* port, TC token bucket register value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_LION_TB_NOT_SUPPORTED_WA_E))
    {
        return GT_NOT_SUPPORTED;
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);

        PRV_CPSS_DXCH_PORT_TOKEN_BUCKET_REG_MAC(devNum, port, &regAddr);

        /* Set Number Tokens and Bucket Size to a maximal values - 0xFFF
           and disable slow rate */
        rc = prvCpssDxChTxPortShaperMaxValueSet(devNum,
                                                portGroupId,
                                                regAddr,
                                                &portTbRegValue);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* Per port per traffic class (queue) */
        for (queue = 0; queue < CPSS_TC_RANGE_CNS; queue++)
        {
            PRV_CPSS_DXCH_PORT_Q_TOKEN_BUCKET_REG_MAC(devNum, port, queue, &regAddr);

            /* Set Number Tokens and Bucket Size to a maximal values - 0xFFF
               and disable slow rate */
            rc = prvCpssDxChTxPortShaperMaxValueSet(devNum,
                                                    portGroupId,
                                                    regAddr,
                                                    &portTcTbRegValue[queue]);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        /* Wait at least 1 millisecond to guaranty bucket size will be > MTU.
           this avoid traffic stuck during shaper disable under traffic.*/
        cpssOsTimerWkAfter(1);

        PRV_CPSS_DXCH_PORT_TOKEN_BUCKET_REG_MAC(devNum, port, &regAddr);

        /* Disable shaping and restore old values */
        rc = prvCpssDxChTxPortShaperDisable(devNum,
                                            portGroupId,
                                            regAddr,
                                            portTbRegValue);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* Per port per traffic class (queue) */
        for (queue = 0; queue < CPSS_TC_RANGE_CNS; queue++)
        {
            PRV_CPSS_DXCH_PORT_Q_TOKEN_BUCKET_REG_MAC(devNum, port, queue, &regAddr);

            /* Disable shaping and restore old values */
            rc = prvCpssDxChTxPortShaperDisable(devNum,
                                                portGroupId,
                                                regAddr,
                                                portTcTbRegValue[queue]);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }
    else
    {
        rc = portTxShaperTokenBucketShapersDisable_rev1(devNum,port);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* portTxShaperTokenBucketBuild
*
* DESCRIPTION:
*       build entry format for tx shaper token bucket configuration
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       startHwArray - pointer to start of HW format of the full entry
*       usePerTc - are we use 'per port' or 'per port per TC'
*       tc - traffic class , relevant when usePerTc = GT_TRUE
*       subEntryPtr - (pointer to) entry when usePerTc = GT_FALSE
*                     (pointer to) sub entry when usePerTc = GT_TRUE
* OUTPUTS:
*       startHwArray - pointer to start of HW format of the full entry
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS portTxShaperTokenBucketBuild
(
    INOUT GT_U32  *startHwArray,
    IN    GT_BOOL usePerTc,
    IN    GT_U32  tc,
    IN    PORT_TX_SHAPER_TOKEN_BUCKET_STC *subEntryPtr
)
{
    GT_U32  startSubEntryOffset;/* offset till the sub entry  */
    GT_U32  offset;/* offset in side the sub entry */
    GT_U32  value;/* hwValue*/

    startSubEntryOffset = (usePerTc == GT_TRUE) ? (54 * tc) : 0;

    value = BOOL2BIT_MAC(subEntryPtr->tokenBucketEn);
    offset = 0;
    U32_SET_FIELD_IN_ENTRY_MAC(startHwArray,(offset + startSubEntryOffset),1,value);

    offset = 1;
    value = BOOL2BIT_MAC(subEntryPtr->slowRateEn);
    U32_SET_FIELD_IN_ENTRY_MAC(startHwArray,(offset + startSubEntryOffset),1,value);

    offset = 2;
    value = subEntryPtr->tbUpdateRatio;
    U32_SET_FIELD_IN_ENTRY_MAC(startHwArray,(offset + startSubEntryOffset),4,value);

    offset = 6;
    value = subEntryPtr->tokens;
    U32_SET_FIELD_IN_ENTRY_MAC(startHwArray,(offset + startSubEntryOffset),12,value);

    offset = 18;
    value = subEntryPtr->maxBucketSize;
    U32_SET_FIELD_IN_ENTRY_MAC(startHwArray,(offset + startSubEntryOffset),12,value);

    offset = 30;
    value = subEntryPtr->currentBucketSize;
    U32_SET_FIELD_IN_ENTRY_MAC(startHwArray,(offset + startSubEntryOffset),24,value);

    return GT_OK;
}

/*******************************************************************************
* portTxShaperTokenBucketParse
*
* DESCRIPTION:
*       parse entry format from tx shaper token bucket configuration
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       startHwArray - pointer to start of HW format of the full entry
*       usePerTc - are we use 'per port' or 'per port per TC'
*       tc - traffic class , relevant when usePerTc = GT_TRUE
* OUTPUTS:
*       subEntryPtr - (pointer to) entry when usePerTc = GT_FALSE
*                     (pointer to) sub entry when usePerTc = GT_TRUE
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS portTxShaperTokenBucketParse
(
    IN  GT_U32  *startHwArray,
    IN  GT_BOOL usePerTc,
    IN  GT_U32  tc,
    OUT  PORT_TX_SHAPER_TOKEN_BUCKET_STC *subEntryPtr
)
{
    GT_U32  startSubEntryOffset;/* offset till the sub entry  */
    GT_U32  offset;/* offset in side the sub entry */
    GT_U32  value;/* hwValue*/

    startSubEntryOffset = (usePerTc == GT_TRUE) ? (54 * tc) : 0;

    offset = 0;
    U32_GET_FIELD_IN_ENTRY_MAC(startHwArray,(offset + startSubEntryOffset),1,value);
    subEntryPtr->tokenBucketEn = BIT2BOOL_MAC(value);

    offset = 1;
    U32_GET_FIELD_IN_ENTRY_MAC(startHwArray,(offset + startSubEntryOffset),1,value);
    subEntryPtr->slowRateEn = BIT2BOOL_MAC(value);

    offset = 2;
    U32_GET_FIELD_IN_ENTRY_MAC(startHwArray,(offset + startSubEntryOffset),4,value);
    subEntryPtr->tbUpdateRatio = value;

    offset = 6;
    U32_GET_FIELD_IN_ENTRY_MAC(startHwArray,(offset + startSubEntryOffset),12,value);
    subEntryPtr->tokens = value;

    offset = 18;
    U32_GET_FIELD_IN_ENTRY_MAC(startHwArray,(offset + startSubEntryOffset),12,value);
    subEntryPtr->maxBucketSize = value;

    offset = 30;
    U32_GET_FIELD_IN_ENTRY_MAC(startHwArray,(offset + startSubEntryOffset),24,value);
    subEntryPtr->currentBucketSize = value;

    return GT_OK;
}

/*******************************************************************************
* portTxShaperTokenBucketEntryWrite_rev1
*
* DESCRIPTION:
*       write entry to tx shaper token bucket configuration
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum  - device number.
*       portNum - port number
*       usePerTc - are we use 'per port' or 'per port per TC'
*       tc - traffic class , relevant when usePerTc = GT_TRUE
*       subEntryPtr - (pointer to) entry when usePerTc = GT_FALSE
*                     (pointer to) sub entry when usePerTc = GT_TRUE
* OUTPUTS:
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*      Only Txq revision 1 is supported by this function.
*
*******************************************************************************/
static GT_STATUS portTxShaperTokenBucketEntryWrite_rev1
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_BOOL usePerTc,
    IN  GT_U32  tc,
    IN  PORT_TX_SHAPER_TOKEN_BUCKET_STC *subEntryPtr
)
{
    GT_U32  hwEntryArray[TXQ_SHAPER_ENTRY_WORDS_NUM_CNS];/* HW entry array */
    GT_STATUS rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */
    PRV_CPSS_DXCH_TABLE_ENT tableType;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    if(usePerTc == GT_FALSE)
    {
        tableType =  PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E;
    }
    else
    {
        tableType =  PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E;
    }

    if(usePerTc == GT_TRUE)
    {
        /* read the entry */
        rc = prvCpssDxChPortGroupReadTableEntry(devNum,portGroupId,
            tableType,
            localPort,
            hwEntryArray);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* build the entry */
    rc = portTxShaperTokenBucketBuild(hwEntryArray,usePerTc,tc,subEntryPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* write the entry */
    rc = prvCpssDxChPortGroupWriteTableEntry(devNum,portGroupId,
        tableType,
        localPort,
        hwEntryArray);

    return rc;
}

/*******************************************************************************
* portTxShaperTokenBucketEntryRead_rev1
*
* DESCRIPTION:
*       read entry from tx shaper token bucket configuration
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum  - device number.
*       portNum - port number
*       usePerTc - are we use 'per port' or 'per port per TC'
*       tc - traffic class , relevant when usePerTc = GT_TRUE
* OUTPUTS:
*       subEntryPtr - (pointer to) entry when usePerTc = GT_FALSE
*                     (pointer to) sub entry when usePerTc = GT_TRUE
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*      Only Txq revision 1 is supported by this function.
*
*******************************************************************************/
static GT_STATUS portTxShaperTokenBucketEntryRead_rev1
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_BOOL usePerTc,
    IN  GT_U32  tc,
    OUT PORT_TX_SHAPER_TOKEN_BUCKET_STC *subEntryPtr
)
{
    GT_U32  hwEntryArray[TXQ_SHAPER_ENTRY_WORDS_NUM_CNS];/* HW entry array */
    GT_STATUS rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */
    PRV_CPSS_DXCH_TABLE_ENT tableType;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    if(usePerTc == GT_FALSE)
    {
        tableType =  PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E;
    }
    else
    {
        tableType =  PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E;
    }

    /* read the entry */
    rc = prvCpssDxChPortGroupReadTableEntry(devNum,portGroupId,
        tableType,
        localPort,
        hwEntryArray);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* parse the entry */
    rc = portTxShaperTokenBucketParse(hwEntryArray,usePerTc,tc,subEntryPtr);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortTxShaperTokenBucketEntryWrite_rev1
*
* DESCRIPTION:
*       Write entry to tx shaper token bucket configuration.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum  - device number.
*       portNum - port number
*       usePerTc - are we use 'per port' or 'per port per TC'
*       tc - traffic class , relevant when usePerTc = GT_TRUE
*       slowRateEn    - slow rate enable value
*       tbUpdateRatio - TB interval update ratio value
*       tokens        - TB refill value
*       maxBucketSize - max Bucket Size
*       updCurrentBucketSize - GT_TRUE: update CurrentBucketSize
*                              GT_FALSE: don't update CurrentBucketSize
*       currentBucketSize - current Bucket Size. Relevant only when
*                           updCurrentBucketSize is GT_TRUE.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*      Only Txq revision 1 is supported by this function.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortTxShaperTokenBucketEntryWrite_rev1
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_BOOL usePerTc,
    IN  GT_U32  tc,
    IN  GT_BOOL slowRateEn,
    IN  GT_U32  tbUpdateRatio,
    IN  GT_U32  tokens,
    IN  GT_U32  maxBucketSize,
    IN  GT_BOOL updCurrentBucketSize,
    IN  GT_U32  currentBucketSize
)
{
    GT_STATUS rc;
    PORT_TX_SHAPER_TOKEN_BUCKET_STC subEntry;

    /* read entry from HW */
    rc = portTxShaperTokenBucketEntryRead_rev1(devNum,portNum,usePerTc,tc,&subEntry);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* update the entry */
    subEntry.slowRateEn = slowRateEn;
    subEntry.tbUpdateRatio = tbUpdateRatio;
    subEntry.tokens = tokens;
    subEntry.maxBucketSize = maxBucketSize;

    if (updCurrentBucketSize == GT_TRUE)
    {
        subEntry.currentBucketSize = currentBucketSize;
    }

    /* write updated entry to HW */
    rc = portTxShaperTokenBucketEntryWrite_rev1(devNum,portNum,usePerTc,tc,&subEntry);
    return rc;
}


/*******************************************************************************
* portTxShaperTokenBucketEnable_rev1
*
* DESCRIPTION:
*       enable entry to tx shaper token bucket configuration
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum  - device number.
*       portNum - port number
*       usePerTc - are we use 'per port' or 'per port per TC'
*       tc - traffic class , relevant when usePerTc = GT_TRUE
*       enable  - GT_TRUE, enable shaping
*                 GT_FALSE, disable shaping
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*      Only Txq revision 1 is supported by this function.
*
*******************************************************************************/
static GT_STATUS portTxShaperTokenBucketEnable_rev1
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_BOOL usePerTc,
    IN  GT_U32  tc,
    IN  GT_BOOL enable
)
{
    GT_STATUS rc;
    PORT_TX_SHAPER_TOKEN_BUCKET_STC subEntry;
    PORT_TX_SHAPER_TOKEN_BUCKET_STC tmpSubEntry;

    /* read entry from HW */
    rc = portTxShaperTokenBucketEntryRead_rev1(devNum,portNum,usePerTc,tc,&subEntry);
    if(rc != GT_OK)
    {
        return rc;
    }

    if((enable == GT_TRUE) ||
       (PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_TOKEN_BUCKET_RATE_SHAPING_DISABLE_WA_E) == GT_FALSE))
    {
        /* update the entry */
        subEntry.tokenBucketEn = enable;

        /* write updated entry to HW */
        rc = portTxShaperTokenBucketEntryWrite_rev1(devNum,portNum,usePerTc,tc,&subEntry);
        return rc;
    }

/*
    NOTE: next logic replace the  prvDxChShaperDisable(...)
*/

    /* Disabling the Egress Rate Shaper under traffic may hang its relevant transmit queue. (FEr#47) */

    tmpSubEntry = subEntry;
    /* Set Number Tokens and Bucket Size to a maximal values - 0xFFF
       and disable slow rate */
    tmpSubEntry.slowRateEn = GT_FALSE;
    tmpSubEntry.tokens = 0xFFF;
    tmpSubEntry.maxBucketSize = 0xFFF;
    tmpSubEntry.tbUpdateRatio = 0;

    /* write updated entry to HW */
    rc = portTxShaperTokenBucketEntryWrite_rev1(devNum,portNum,usePerTc,tc,&tmpSubEntry);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* Token bucket update to new maximal values will be configured by next
       pulse of token bucket update clock. It's need to be done delay to
       disable token bucket to guaranty maximal values update.
       The delay need to be for time more then maximal token bucket update
       period - 15360 core clock cycles. For Core Clock 144MHz delay should be
       more then 100 micro seconds. */
    cpssOsTimerWkAfter(1);

    /* disable shaping */
    tmpSubEntry.tokenBucketEn = GT_FALSE;
    /* write updated entry to HW */
    rc = portTxShaperTokenBucketEntryWrite_rev1(devNum,portNum,usePerTc,tc,&tmpSubEntry);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* restore old values */
    subEntry.tokenBucketEn = GT_FALSE;

    /* write updated entry to HW */
    return portTxShaperTokenBucketEntryWrite_rev1(devNum,portNum,usePerTc,tc,&subEntry);
}

/*******************************************************************************
* portTxShaperTokenBucketShapersDisableStage_rev1
*
* DESCRIPTION:
*       Disable Shaping on Port and all it's queues.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum  - physical device number.
*       portNum - physical port number (CPU port supported as well as regular ports).
*       stage   - disable stage.
*       origPortEntryPtr - (pointer to) original port TB entry value.
*       tempPortEntryPtr - (pointer to) temp port TB entry value.
*       origTcEntryPtr   - (pointer to) original port/tc TB entries.
*       tempTcEntryPtr   - (pointer to) temp port/tc TB entries.
*
* OUTPUTS:
*       origPortEntryPtr - (pointer to) original port TB entry value.
*       tempPortEntryPtr - (pointer to) temp port TB entry value.
*       origTcEntryPtr   - (pointer to) original port/tc TB entries.
*       tempTcEntryPtr   - (pointer to) temp port/tc TB entries.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      Only Txq revision 1 is supported by this function.
*
*******************************************************************************/
static GT_STATUS portTxShaperTokenBucketShapersDisableStage_rev1
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    IN  SHAPER_DISABLE_STAGE_ENT    stage,
    INOUT PORT_TX_SHAPER_TOKEN_BUCKET_STC *origPortEntryPtr,
    INOUT PORT_TX_SHAPER_TOKEN_BUCKET_STC *tempPortEntryPtr,
    INOUT PORT_TX_SHAPER_TOKEN_BUCKET_STC *origTcEntryPtr,
    INOUT PORT_TX_SHAPER_TOKEN_BUCKET_STC *tempTcEntryPtr
)
{
    GT_U8       queue;      /* port traffic class */
    GT_STATUS   rc;         /* return code */
    GT_U32  portGroupId;    /*the port group Id - support multi-port-groups device */
    GT_U8   localPort;      /* local port - support multi-port-groups device */
    PRV_CPSS_DXCH_TABLE_ENT tableType =
        PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E;
    GT_U32  portTcTbRegValueArr[TXQ_SHAPER_ENTRY_WORDS_NUM_CNS];
    PORT_TX_SHAPER_TOKEN_BUCKET_STC *currentTempTcEntryPtr;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    if(stage == SHAPER_DISABLE_STAGE_1_E)
    {
        /* Disabling the Egress Rate Shaper under traffic may hang its */
        /*   relevant transmit queue. (FEr#47) */

        /**************/
        /* 'per port' */
        /**************/

        /* read the entry */
        rc = portTxShaperTokenBucketEntryRead_rev1(devNum,portNum,GT_FALSE,0,tempPortEntryPtr);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* save the original entry values */
        *origPortEntryPtr = *tempPortEntryPtr;

        /* Set Number Tokens and Bucket Size to a maximal values - 0xFFF*/
        /*   and disable slow rate */
        tempPortEntryPtr->slowRateEn = GT_FALSE;
        tempPortEntryPtr->tokens = 0xFFF;
        tempPortEntryPtr->maxBucketSize = 0xFFF;
        tempPortEntryPtr->tbUpdateRatio = 0;

        /* write the entry */
        rc = portTxShaperTokenBucketEntryWrite_rev1(devNum,portNum,GT_FALSE,0,tempPortEntryPtr);
        if(rc != GT_OK)
        {
            return rc;
        }

        /*********************/
        /* 'per port per tc' */
        /*********************/

        /* read the HW entry */

        rc = prvCpssDxChPortGroupReadTableEntry(devNum,portGroupId,PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E,localPort,portTcTbRegValueArr);
        if(rc != GT_OK)
        {
            return rc;
        }

        for (queue = 0; queue < CPSS_TC_RANGE_CNS; queue++)
        {
            /* parse the sub entries */
            currentTempTcEntryPtr = tempTcEntryPtr + queue;
            rc = portTxShaperTokenBucketParse(portTcTbRegValueArr,GT_TRUE,queue,currentTempTcEntryPtr);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* save the original entry values */
            *(origTcEntryPtr  + queue) = *currentTempTcEntryPtr;

            /* Set Number Tokens and Bucket Size to a maximal values - 0xFFF */
            /*   and disable slow rate */
            currentTempTcEntryPtr->slowRateEn = GT_FALSE;
            currentTempTcEntryPtr->tokens = 0xFFF;
            currentTempTcEntryPtr->maxBucketSize = 0xFFF;
            currentTempTcEntryPtr->tbUpdateRatio = 0;

            /* update HW entry */
            rc = portTxShaperTokenBucketBuild(portTcTbRegValueArr,GT_TRUE,queue,currentTempTcEntryPtr);
            if(rc != GT_OK)
            {
                return rc;
            }

        }

        /* write HW entry */
        rc = prvCpssDxChPortGroupWriteTableEntry(devNum,portGroupId,tableType,localPort,portTcTbRegValueArr);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(stage == SHAPER_DISABLE_STAGE_2_E)
    {
        /**************/
        /* 'per port' */
        /**************/
        tempPortEntryPtr->tokenBucketEn = GT_FALSE;

        /* write the temp entry */
        rc = portTxShaperTokenBucketEntryWrite_rev1(devNum,portNum,GT_FALSE,0,tempPortEntryPtr);
        if(rc != GT_OK)
        {
            return rc;
        }

        origPortEntryPtr->tokenBucketEn = GT_FALSE;

        /* write the original entry */
        rc = portTxShaperTokenBucketEntryWrite_rev1(devNum,portNum,GT_FALSE,0,origPortEntryPtr);
        if(rc != GT_OK)
        {
            return rc;
        }

        /*********************/
        /* 'per port per tc' */
        /*********************/
        for (queue = 0; queue < CPSS_TC_RANGE_CNS; queue++)
        {

            currentTempTcEntryPtr = tempTcEntryPtr + queue;
            currentTempTcEntryPtr->tokenBucketEn = GT_FALSE;

            /* update HW entry */
            rc = portTxShaperTokenBucketBuild(portTcTbRegValueArr,GT_TRUE,queue,currentTempTcEntryPtr);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        /* write the HW entry */
        rc = prvCpssDxChPortGroupWriteTableEntry(devNum,portGroupId,tableType,localPort,portTcTbRegValueArr);
        if(rc != GT_OK)
        {
            return rc;
        }

        for (queue = 0; queue < CPSS_TC_RANGE_CNS; queue++)
        {

            currentTempTcEntryPtr = origTcEntryPtr + queue;
            currentTempTcEntryPtr->tokenBucketEn = GT_FALSE;

            /* update HW entry */
            rc = portTxShaperTokenBucketBuild(portTcTbRegValueArr,GT_TRUE,queue,currentTempTcEntryPtr);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        /* write the HW entry */
        rc = prvCpssDxChPortGroupWriteTableEntry(devNum,portGroupId,tableType,localPort,portTcTbRegValueArr);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* portTxShaperTokenBucketShapersDisable_rev1
*
* DESCRIPTION:
*       Disable Shaping on Port and all it's queues.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum  - physical device number.
*       portNum - physical port number (CPU port supported as well as regular ports)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      Only Txq revision 1 is supported by this function.
*
*******************************************************************************/
static GT_STATUS portTxShaperTokenBucketShapersDisable_rev1
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum
)
{
    PORT_TX_SHAPER_TOKEN_BUCKET_STC origPortEntry;
    PORT_TX_SHAPER_TOKEN_BUCKET_STC tempPortEntry;
    PORT_TX_SHAPER_TOKEN_BUCKET_STC origTcEntryArr[CPSS_TC_RANGE_CNS];
    PORT_TX_SHAPER_TOKEN_BUCKET_STC tempTcEntry[CPSS_TC_RANGE_CNS];
    GT_U8       queue;      /* port traffic class */
    GT_STATUS   rc;         /* return code */

    if(PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_TOKEN_BUCKET_RATE_SHAPING_DISABLE_WA_E) == GT_FALSE)
    {
        /* no need for WA */
        rc = portTxShaperTokenBucketEnable_rev1(devNum,portNum,GT_FALSE,0,GT_FALSE);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* Per port per traffic class (queue) */
        for (queue = 0; queue < CPSS_TC_RANGE_CNS; queue++)
        {
            rc = portTxShaperTokenBucketEnable_rev1(devNum,portNum,GT_TRUE,queue,GT_FALSE);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        return GT_OK;
    }

    /* Disabling the Egress Rate Shaper under traffic may hang its
       relevant transmit queue. (FEr#47) */

    rc = portTxShaperTokenBucketShapersDisableStage_rev1(devNum,portNum,
        SHAPER_DISABLE_STAGE_1_E,
        &origPortEntry,
        &tempPortEntry,
        origTcEntryArr,
        tempTcEntry);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* wait at least 1 millisecond to guaranty bucket size will be > MTU.
      this avoid traffic stuck during shaper disable under traffic.*/
    cpssOsTimerWkAfter(1);

    rc = portTxShaperTokenBucketShapersDisableStage_rev1(devNum,portNum,
        SHAPER_DISABLE_STAGE_2_E,
        &origPortEntry,
        &tempPortEntry,
        origTcEntryArr,
        tempTcEntry);
    if(rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}


/*******************************************************************************
* portTxShaperTokenBucketAllShapersDisable_rev1
*
* DESCRIPTION:
*       Disables all ports and queues shapers for specified device in minimum delay.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum - physical device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      Only Txq revision 1 is supported by this function.
*
*******************************************************************************/
static GT_STATUS portTxShaperTokenBucketAllShapersDisable_rev1
(
    IN  GT_U8    devNum
)
{
    static PORT_TX_SHAPER_TOKEN_BUCKET_STC origPortEntry[PRV_CPSS_MAX_PP_PORTS_NUM_CNS];
    static PORT_TX_SHAPER_TOKEN_BUCKET_STC tempPortEntry[PRV_CPSS_MAX_PP_PORTS_NUM_CNS];
    static PORT_TX_SHAPER_TOKEN_BUCKET_STC origTcEntryArr[PRV_CPSS_MAX_PP_PORTS_NUM_CNS][CPSS_TC_RANGE_CNS];
    static PORT_TX_SHAPER_TOKEN_BUCKET_STC tempTcEntry[PRV_CPSS_MAX_PP_PORTS_NUM_CNS][CPSS_TC_RANGE_CNS];
    GT_U8       portNum;       /* device port */
    GT_STATUS   rc;         /* return code */
    GT_U8       queue;      /* port traffic class */

    if(PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_TOKEN_BUCKET_RATE_SHAPING_DISABLE_WA_E) == GT_FALSE)
    {
        /* no need for WA */
        for (portNum = 0; portNum < PRV_CPSS_MAX_PP_PORTS_NUM_CNS; portNum++)
        {
            if (!PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, portNum) && portNum != CPSS_CPU_PORT_NUM_CNS)
                continue;

            /* Per port */
            rc = portTxShaperTokenBucketEnable_rev1(devNum,portNum,GT_FALSE,0,GT_FALSE);
            if(rc != GT_OK)
            {
                return rc;
            }
    
            /* Per port per traffic class (queue) */
            for (queue = 0; queue < CPSS_TC_RANGE_CNS; queue++)
            {
                rc = portTxShaperTokenBucketEnable_rev1(devNum,portNum,GT_TRUE,queue,GT_FALSE);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
        }

        return GT_OK;
    }

    /* Disabling the Egress Rate Shaper under traffic may hang its
       relevant transmit queue. (FEr#47) */
    for (portNum = 0; portNum < PRV_CPSS_MAX_PP_PORTS_NUM_CNS; portNum++)
    {
        if (!PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, portNum) && portNum != CPSS_CPU_PORT_NUM_CNS)
            continue;

        rc = portTxShaperTokenBucketShapersDisableStage_rev1(devNum,portNum,
            SHAPER_DISABLE_STAGE_1_E,
            &origPortEntry[portNum],
            &tempPortEntry[portNum],
            &origTcEntryArr[portNum][0],
            &tempTcEntry[portNum][0]);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* wait at least 1 millisecond to guaranty bucket size will be > MTU.
      this avoid traffic stuck during shaper disable under traffic.*/
    cpssOsTimerWkAfter(1);

    for (portNum = 0; portNum < PRV_CPSS_MAX_PP_PORTS_NUM_CNS; portNum++)
    {
        if (!PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, portNum) && portNum != CPSS_CPU_PORT_NUM_CNS)
            continue;

        rc = portTxShaperTokenBucketShapersDisableStage_rev1(devNum,portNum,
            SHAPER_DISABLE_STAGE_2_E,
            &origPortEntry[portNum],
            &tempPortEntry[portNum],
            &origTcEntryArr[portNum][0],
            &tempTcEntry[portNum][0]);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortTxRegisterAddrGet
*
* DESCRIPTION:
*       get register address for 'txq per port' .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number.
*       primaryIndex - primary index , can be used as :
*               port number (also CPU port)
*               or as profile number
*       secondaryIndex - secondary index , can be used as TC
*       registerType - register type
* OUTPUTS:
*       regAddrPtr - (pointer to) register address
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortTxRegisterAddrGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   primaryIndex,
    IN  GT_U32   secondaryIndex,
    IN  PRV_CPSS_DXCH_PORT_TX_REG_TYPE_ENT  registerType,
    OUT GT_U32   *regAddrPtr
)
{
    dxch_txPortRegs *txPortsPtr;/* current tx Ports info */

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        if(primaryIndex == CPSS_CPU_PORT_NUM_CNS)
        {
            txPortsPtr = &PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txCpuRegs;
        }
        else
        {
            txPortsPtr = &PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txPortRegs[primaryIndex];
        }

        switch(registerType)
        {
            case PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TX_CONFIG_E:
                *(regAddrPtr) = txPortsPtr->txConfig;
            break;

            case PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TOKEN_BUCK_E:
                *(regAddrPtr) = txPortsPtr->tokenBuck;
            break;

            case PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TOKEN_BUCK_LEN_E:
                *(regAddrPtr) = txPortsPtr->tokenBuckLen;
            break;

            case PRV_CPSS_DXCH_PORT_TX_REG_TYPE_PRIO_TOKEN_BUCK_E:
                *(regAddrPtr) = txPortsPtr->prioTokenBuck[secondaryIndex];
            break;

            case PRV_CPSS_DXCH_PORT_TX_REG_TYPE_PRIO_TOKEN_BUCK_LEN_E:
                *(regAddrPtr) = txPortsPtr->prioTokenBuckLen[secondaryIndex];
            break;

            case PRV_CPSS_DXCH_PORT_TX_REG_TYPE_DESCR_COUNTER_E:
                *(regAddrPtr) = txPortsPtr->descrCounter;
            break;

            case PRV_CPSS_DXCH_PORT_TX_REG_TYPE_BUFFER_COUNTER_E:
                *(regAddrPtr) = txPortsPtr->bufferCounter;
            break;

            case PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TC_DESCR_COUNTER_E:
                *(regAddrPtr) = txPortsPtr->tcDescrCounter[secondaryIndex];
            break;

            case PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TC_BUFFER_COUNTER_E:
                *(regAddrPtr) = txPortsPtr->tcBufferCounter[secondaryIndex];
            break;

            case PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_WRR_WEIGHTS_0_E:
                *(regAddrPtr) = txPortsPtr->wrrWeights0;
            break;

            case PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_WRR_WEIGHTS_1_E:
                *(regAddrPtr) = txPortsPtr->wrrWeights1;
            break;

            case PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_WRR_STRICT_PRIO_E:
            case PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_WRR_STRICT_PRIO_EN_E:
                *(regAddrPtr) = txPortsPtr->wrrStrictPrio;
            break;

            case PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_DESCR_LIMIT_E:
                PRV_CPSS_DXCH_DROP_PROFILE_CHECK_MAC(primaryIndex, devNum);
                *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    egrTxQConf.setsConfigRegs.portLimitsConf[primaryIndex];
            break;

            default:
                return GT_BAD_PARAM;
        }
    }
    else
    {

        switch(registerType)
        {
            case PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TX_CONFIG_E:
                /* this register spread to many registers in txqVer1 */
                return GT_NOT_IMPLEMENTED;

            case PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TOKEN_BUCK_E:
                /* use tables engine , with table:
                PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E
                */
                return GT_NOT_IMPLEMENTED;

            case PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TOKEN_BUCK_LEN_E:
                /* not used in cpss yet */
                return GT_NOT_IMPLEMENTED;

            case PRV_CPSS_DXCH_PORT_TX_REG_TYPE_PRIO_TOKEN_BUCK_E:
                /* use tables engine , with table:
                PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E
                */
                return GT_NOT_IMPLEMENTED;

            case PRV_CPSS_DXCH_PORT_TX_REG_TYPE_PRIO_TOKEN_BUCK_LEN_E:
                /* not used in cpss yet */
                return GT_NOT_IMPLEMENTED;

            case PRV_CPSS_DXCH_PORT_TX_REG_TYPE_DESCR_COUNTER_E:
                *(regAddrPtr) =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.tailDrop.
                        counters.portDescCounter[primaryIndex];
            break;

            case PRV_CPSS_DXCH_PORT_TX_REG_TYPE_BUFFER_COUNTER_E:
                *(regAddrPtr) =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.tailDrop.
                        counters.portBuffersCounter[primaryIndex];
            break;

            case PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TC_DESCR_COUNTER_E:
                *(regAddrPtr) =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.tailDrop.
                        counters.qMainDesc[primaryIndex][secondaryIndex];
            break;

            case PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TC_BUFFER_COUNTER_E:
                *(regAddrPtr) =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.tailDrop.
                        counters.qMainBuff[primaryIndex][secondaryIndex];
            break;

            case PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_WRR_WEIGHTS_0_E:
                *(regAddrPtr) =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.scheduler.
                        priorityArbiterWeights.profileSdwrrWeightsConfig0[primaryIndex];
            break;

            case PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_WRR_WEIGHTS_1_E:
                *(regAddrPtr) =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.scheduler.
                        priorityArbiterWeights.profileSdwrrWeightsConfig1[primaryIndex];
            break;

            case PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_WRR_STRICT_PRIO_E:
                *(regAddrPtr) =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.scheduler.
                        priorityArbiterWeights.profileSdwrrGroup[primaryIndex];
            break;

            case PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_WRR_STRICT_PRIO_EN_E:
                *(regAddrPtr) =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.scheduler.
                        priorityArbiterWeights.profileSdwrrEnable[primaryIndex];
            break;

            case PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_DESCR_LIMIT_E:
                *(regAddrPtr) =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.tailDrop.
                            limits.profilePortLimits[primaryIndex];
            break;

            default:
                return GT_BAD_PARAM;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxSchedulerDeficitModeEnableSet
*
* DESCRIPTION:
*       Enable/Disable scheduler deficit mode.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum  - physical device number
*       enable  - GT_TRUE: Enable Scheduler Deficit mode.
*                 GT_FALSE: Disable Scheduler Deficit mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Deficit mode should be enabled to support line-rate scheduling
*       in 40/100 Gbps ports.
*       2. Request Masks must be properly configured if deficit mode
*          is enabled, see:
*          cpssDxChPortTxRequestMaskSet
*          cpssDxChPortTxRequestMaskIndexSet.
*       3. If Deficit scheduling is enabled, the shaper’s baseline must be
*          at least 8*MTU, see:
*          cpssDxChPortTxShaperBaselineSet
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSchedulerDeficitModeEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    value = BOOL2BIT_MAC(enable);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.scheduler.config.config;
    return prvCpssDxChHwPpSetRegField(devNum, regAddr, 23, 1, value);
}

/*******************************************************************************
* cpssDxChPortTxSchedulerDeficitModeEnableGet
*
* DESCRIPTION:
*       Gets the current status of scheduler deficit mode.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       enablePtr  - (pointer to) status of scheduler deficit mode
*                    GT_TRUE: Scheduler deficit mode enabled.
*                    GT_FALSE: Scheduler Deficit mode disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSchedulerDeficitModeEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.scheduler.config.config;
    rc =  prvCpssDxChHwPpGetRegField(devNum, regAddr, 23, 1, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxRequestMaskSet
*
* DESCRIPTION:
*       Sets request mask value to given mask index.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       maskIndex    - request mask index (0..3).
*       maskValue    - request mask value defines the minimum number of core clock
*                      cycles between two consecutive service grants
*                      to the same port (3..255).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or mask index
*       GT_OUT_OF_RANGE          - on out of range mask value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Request masks used only when deficit mode is enabled, see:
*          cpssDxChPortTxSchedulerDeficitModeEnableSet.
*       2. To bind a port to request mask index use:
*          cpssDxChPortTxRequestMaskIndexSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxRequestMaskSet
(
    IN GT_U8    devNum,
    IN GT_U32   maskIndex,
    IN GT_U32   maskValue
)
{
    GT_U32 regAddr;   /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    if (maskIndex > PRV_CPSS_DXCH_PORT_REQUEST_MASK_INDEX_MAX_CNS)
    {
        return GT_BAD_PARAM;
    }
    if (maskValue < 3 || maskValue > 255)
    {
        return GT_OUT_OF_RANGE;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.dq.scheduler.config.portRequestMask;
    return prvCpssDxChHwPpSetRegField(devNum, regAddr, maskIndex*8, 8, maskValue);
}

/*******************************************************************************
* cpssDxChPortTxRequestMaskGet
*
* DESCRIPTION:
*       Gets request mask value to given mask index.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       maskIndex    - request mask index (0..3).
*
* OUTPUTS:
*       maskValuePtr - (pointer to) request mask value defines the minimum number
*                      of core clock cycles between two consecutive service
*                      grants to the same port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number or mask index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxRequestMaskGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   maskIndex,
    OUT GT_U32  *maskValuePtr
)
{
    GT_U32 regAddr;   /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(maskValuePtr);
    if (maskIndex > PRV_CPSS_DXCH_PORT_REQUEST_MASK_INDEX_MAX_CNS)
    {
        return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.dq.scheduler.config.portRequestMask;
    return prvCpssDxChHwPpGetRegField(devNum, regAddr, maskIndex*8, 8, maskValuePtr);
}

/*******************************************************************************
* cpssDxChPortTxRequestMaskIndexSet
*
* DESCRIPTION:
*       Binds a port to a request mask.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - port number.
*       maskIndex    - mask index (0..3).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_OUT_OF_RANGE          - on out of range mask index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxRequestMaskIndexSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_U32   maskIndex
)
{
    GT_U32 regAddr;   /* register address */
    GT_U32 portGroupId; /* the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    if (maskIndex > PRV_CPSS_DXCH_PORT_REQUEST_MASK_INDEX_MAX_CNS)
    {
        return GT_OUT_OF_RANGE;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.dq.scheduler.config.portRequestMaskSelector[portNum];
    return prvCpssDxChHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 0, 2, maskIndex);
}

/*******************************************************************************
* cpssDxChPortTxRequestMaskIndexGet
*
* DESCRIPTION:
*       Gets the port's request mask.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - port number.
*
* OUTPUTS:
*       maskIndexPtr - (pointer to) mask index.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxRequestMaskIndexGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT GT_U32   *maskIndexPtr
)
{
    GT_U32 regAddr;   /* register address */
    GT_U32 portGroupId; /* the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(maskIndexPtr);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.dq.scheduler.config.portRequestMaskSelector[portNum];
    return prvCpssDxChHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, 0, 2, maskIndexPtr);
}

/*******************************************************************************
* cpssDxChPortTxShaperModeSet
*
* DESCRIPTION:
*       Set Shaper mode, packet or byte based for given port.
*       Shapers are configured by cpssDxChPortTxShaperProfileSet or
*       cpssDxChPortTxQShaperProfileSet
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum  - device number.
*       portNum - port number (CPU port supported as well as regular ports).
*       mode    - shaper mode: byte count or packet number based.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperModeSet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    IN  CPSS_PORT_TX_DROP_SHAPER_MODE_ENT   mode
)
{
    GT_U32  regAddr;         /* register address             */
    GT_U32  hwPortTbMode;    /* The Port Token bucket Rate shaper mode  */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    switch (mode)
    {
        case CPSS_PORT_TX_DROP_SHAPER_BYTE_MODE_E:
            hwPortTbMode = 0;
            break;
        case CPSS_PORT_TX_DROP_SHAPER_PACKET_MODE_E:
            hwPortTbMode = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    if (localPort == CPSS_CPU_PORT_NUM_CNS)
    {
        localPort &= 0xF;
    }

    /* Set Token Bucket Mode */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.dq.shaper.tokenBucketMode;

    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, localPort, 1, hwPortTbMode);
}

/*******************************************************************************
* cpssDxChPortTxShaperModeGet
*
* DESCRIPTION:
*       Get Shaper mode, packet or byte based for given port.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum  - device number.
*       portNum - port number (CPU port supported as well as regular ports).
*
* OUTPUTS:
*       modePtr - (pointer to) shaper mode: byte count or packet number based.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperModeGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT CPSS_PORT_TX_DROP_SHAPER_MODE_ENT   *modePtr
)
{
    GT_U32     regAddr;     /* register address             */
    GT_U32     value;       /* value to read from register */
    GT_STATUS  rc;          /* return code */
    GT_U32     portGroupId; /*the port group Id - support multi-port-groups device */
    GT_U8      localPort;   /* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    if (localPort == CPSS_CPU_PORT_NUM_CNS)
    {
        localPort &= 0xF;
    }

    /* Get Token Bucket Mode */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.dq.shaper.tokenBucketMode;

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, localPort, 1, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    *modePtr = (value == 1) ? CPSS_PORT_TX_DROP_SHAPER_PACKET_MODE_E :
                              CPSS_PORT_TX_DROP_SHAPER_BYTE_MODE_E;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxShaperBaselineSet
*
* DESCRIPTION:
*       Set Token Bucket Baseline.
*       The Token Bucket Baseline is the "zero" level of the token bucket.
*       When the token bucket fill level < Baseline, the respective queue/port
*       is not served.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum   - device number.
*       baseline - Token Bucket Baseline value in bytes(0..0xFFFFFF).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range baseline
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Token Bucket Baseline must be configured as follows:
*        1. At least MTU.
*        2. If PFC response is enabled, the Baseline must be
*           at least 0x3FFFC0, see:
*           cpssDxChPortPfcEnableSet.
*        3. If Deficit scheduling is enabled, the Baseline must be
*           at least 8*MTU, see:
*           cpssDxChPortTxSchedulerDeficitModeEnableSet.
*        4. When packet based shaping is enabled, the following used as
*           shaper’s MTU:
*           CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC. portsPacketLength
*           CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC. cpuPacketLength
*           see:
*           cpssDxChPortTxShaperConfigurationSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperBaselineSet
(
    IN GT_U8    devNum,
    IN GT_U32   baseline
)
{
    GT_U32      regAddr;    /* register address             */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }
    if (baseline > 0xFFFFFF)
    {
        return GT_OUT_OF_RANGE;
    }

    /* Set Token Bucket Base Line */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.dq.shaper.tokenBucketBaseLine;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 24, baseline);
}

/*******************************************************************************
* cpssDxChPortTxShaperBaselineGet
*
* DESCRIPTION:
*       Get Token Bucket Baseline.
*       The Token Bucket Baseline is the "zero" level of the token bucket.
*       When the token bucket fill level < Baseline, the respective queue/port
*       is not served.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum  - device number.
*
* OUTPUTS:
*       baselinePtr - (pointer to) Token Bucket Baseline value in bytes.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperBaselineGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *baselinePtr
)
{
    GT_U32      regAddr;    /* register address                */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(baselinePtr);
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    /* Get Token Bucket Base Line */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.dq.shaper.tokenBucketBaseLine;

    return prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 24, baselinePtr);
}

/*******************************************************************************
* cpssDxChPortTxShaperConfigurationSet
*
* DESCRIPTION:
*       Set global configuration for shaper.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*      devNum     - device number.
*      configsPtr - (pointer to) shaper configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        When packet based shaping is enabled, the following used as shaper’s MTU:
*        configsPtr->portsPacketLength
*        configsPtr->cpuPacketLength
*        see:
*        cpssDxChPortTxShaperBaselineSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperConfigurationSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC *configsPtr
)
{
    GT_U32      value;      /* value to write into register */
    GT_STATUS   rc;         /* return code */
    GT_U32      regAddr;    /* register address             */
    CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC oldConfigs; /* old global configurations */
    CPSS_PORT_TX_DROP_SHAPER_MODE_ENT shaperMode; /* shaper mode on port */
    GT_U8 port;
    GT_U32 tcQueue = 0;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(configsPtr);
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    if((configsPtr->tokensRate < PRV_CPSS_DXCH_TXQREV1_TOKEN_BCKT_UPD_RATE_MIN_CNS) ||
       (configsPtr->tokensRate > PRV_CPSS_DXCH_TXQREV1_TOKEN_BCKT_UPD_RATE_MAX_CNS) ||
       (configsPtr->slowRateRatio < PRV_CPSS_DXCH_TXQREV1_SLOW_TOKEN_BCKT_UPD_RATIO_MIN_CNS) ||
       (configsPtr->slowRateRatio > PRV_CPSS_DXCH_TXQREV1_SLOW_TOKEN_BCKT_UPD_RATIO_MAX_CNS) ||
       (configsPtr->portsPacketLength > 0xFFFFFF) ||
       (configsPtr->cpuPacketLength > 0xFFFFFF))
    {
        return GT_OUT_OF_RANGE;
    }

    /* save the old configurations */
    rc = cpssDxChPortTxShaperConfigurationGet(devNum,&oldConfigs);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Set update rates */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.dq.shaper.tokenBucketUpdateRate;

    value = ((configsPtr->slowRateRatio - 1)& 0xF) << 18 |
            (configsPtr->tokensRate & 0xF) << 26;

    switch (configsPtr->tokensRateGran)
    {
        case CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_64_CORE_CLOCKS_E:
            value |= BIT_31;
            break;
        case CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_1024_CORE_CLOCKS_E:
            break;
        default:
            return GT_BAD_PARAM;
    }

    rc = prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0xBC3C0000, value);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Set CPU Token Bucket MTU */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.dq.shaper.cpuTokenBucketMtuConfig;
    value = configsPtr->cpuPacketLength;
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 24, value);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Set Ports Token Bucket MTU */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.dq.shaper.portsTokenBucketMtuConfig;
    value = configsPtr->portsPacketLength;
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 24, value);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_LION_TB_NOT_SUPPORTED_WA_E))
    {
        return GT_OK;
    }

    /* Reconfigure all enabled Token Buckets per port and per port per queue. */
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    for (port = 0; port < PRV_CPSS_PP_MAC(devNum)->numOfPorts; port++)
    {
        if (! PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, port))
            continue;

        /* get shaper mode on the port */
        rc = cpssDxChPortTxShaperModeGet(devNum,port,&shaperMode);
        if (rc != GT_OK)
        {
            return rc;
        }


        /* per port */
        rc = prvDxChPortTxSetShaperTokenBucketParams_rev1(
                devNum,
                port,
                shaperMode,
                GT_FALSE,
                tcQueue,
                &oldConfigs,
                configsPtr);

        if (rc != GT_OK)
        {
            return rc;
        }

        /* per port per traffic class (tcQueue) */
        for (tcQueue = 0; tcQueue < CPSS_TC_RANGE_CNS; tcQueue++)
        {
            rc = prvDxChPortTxSetShaperTokenBucketParams_rev1(
                    devNum,
                    port,
                    shaperMode,
                    GT_TRUE,
                    tcQueue,
                    &oldConfigs,
                    configsPtr);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    /* for CPU port */
    port = (GT_U8)CPSS_CPU_PORT_NUM_CNS;

    /* get shaper mode on the port */
    rc = cpssDxChPortTxShaperModeGet(devNum,port,&shaperMode);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvDxChPortTxSetShaperTokenBucketParams_rev1(
            devNum,
            port,
            shaperMode,
            GT_FALSE,
            tcQueue,
            &oldConfigs,
            configsPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* CPU port per traffic class (tcQueue) */
    for (tcQueue = 0; tcQueue < CPSS_TC_RANGE_CNS; tcQueue++)
    {
        rc = prvDxChPortTxSetShaperTokenBucketParams_rev1(
                devNum,
                port,
                shaperMode,
                GT_TRUE,
                tcQueue,
                &oldConfigs,
                configsPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxShaperConfigurationGet
*
* DESCRIPTION:
*       Get global configuration for shaper.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*      devNum     - device number.
*
* OUTPUTS:
*      configsPtr - (pointer to) shaper configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperConfigurationGet
(
    IN  GT_U8                   devNum,
    OUT CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC *configsPtr
)
{
    GT_U32      value;      /* value to write into register */
    GT_STATUS   rc;         /* return code */
    GT_U32      regAddr;    /* register address             */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(configsPtr);
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    /* Get update rates */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.dq.shaper.tokenBucketUpdateRate;

    rc = prvCpssDrvHwPpReadRegBitMask(devNum, regAddr, 0xBC3C0000, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    configsPtr->slowRateRatio = ((value >> 18) & 0xF) + 1;
    configsPtr->tokensRate = (value >> 26) & 0xF;
    configsPtr->tokensRateGran = ((value >> 31) == 0) ?
        CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_1024_CORE_CLOCKS_E:
        CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_64_CORE_CLOCKS_E;

    /* Get CPU Token Bucket MTU */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.dq.shaper.cpuTokenBucketMtuConfig;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 24, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    configsPtr->cpuPacketLength = value;

    /* Get Ports Token Bucket MTU */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.dq.shaper.portsTokenBucketMtuConfig;
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 24, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    configsPtr->portsPacketLength = value;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxTailDropBufferConsumptionModeSet
*
* DESCRIPTION:
*       Sets the packet buffer consumption mode and its parameter.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum  - device number.
*       mode    - packet buffer consumption mode.
*       length  - Defines the number of buffers consumed by a packet for
*                 Tail Drop and Scheduling. Relevant only for mode
*                 CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_PACKET_E.
*                 Range: 0..63
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or wrong mode
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on length out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function should be called only when traffic disabled
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTailDropBufferConsumptionModeSet
(
    IN  GT_U8                                                   devNum,
    IN  CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_ENT mode,
    IN  GT_U32                                                  length
)
{
    GT_STATUS rc;       /* return code          */
    GT_U32  regAddr;    /* register address     */
    GT_U32  regData;    /* register data        */
    GT_U32  regDataLen; /* register data length */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);


    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    switch (mode)
    {
        case CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_BYTE_E:
                regData = 0;
                regDataLen = 1;
                break;
        case CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_PACKET_E:
                if( length >= BIT_6 )
                {
                    return GT_OUT_OF_RANGE;
                }
                regData = 1;
                U32_SET_FIELD_MAC(regData, 1, 6, length);
                regDataLen = 7;
                break;
        default: return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.queue.tailDrop.config.byteCount;

    rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, regDataLen, regData);
    if( GT_OK != rc )
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.dq.scheduler.config.schedulerByteCountForTailDrop;

    return prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, regDataLen, regData);
}

/*******************************************************************************
* cpssDxChPortTxTailDropBufferConsumptionModeGet
*
* DESCRIPTION:
*       Gets the packet buffer consumption mode and its parameter.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum  - device number.
*
* OUTPUTS:
*       modePtr     - (pointer to) packet buffer consumption mode.
*       lengthPtr   - (pointer to ) the number of buffers consumed by a packet
*                     for Tail Drop and Scheduling. Relevant only for mode
*                 CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_PACKET_E.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or wrong mode
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTailDropBufferConsumptionModeGet
(
    IN  GT_U8                                                   devNum,
    OUT CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_ENT *modePtr,
    OUT GT_U32                                                  *lengthPtr
)
{
    GT_STATUS rc;       /* return code          */
    GT_U32  regAddr;    /* register address     */
    GT_U32  regData;    /* register data        */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);
    CPSS_NULL_PTR_CHECK_MAC(lengthPtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.queue.tailDrop.config.byteCount;

    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 7, &regData);
    if( GT_OK != rc )
    {
        return rc;
    }

    *modePtr = U32_GET_FIELD_MAC(regData, 0, 1) == 0 ?
                CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_BYTE_E :
                CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_PACKET_E ;

    *lengthPtr = U32_GET_FIELD_MAC(regData, 1, 6);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxTcProfileSharedPoolSet
*
* DESCRIPTION:
*       Sets the shared pool associated for traffic class and Profile.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum  - physical device number
*       pfSet   - the Profile Set in which the Traffic
*                 Class Drop Parameters is associated
*       tc      - the Traffic Class, range 0..7.
*       poolNum - shared pool associated, range 0..7.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, pfSet
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTcProfileSharedPoolSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   pfSet,
    IN  GT_U8                               tc,
    IN  GT_U32                              poolNum
)
{
    GT_U32  regAddr;    /* register address     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tc);
    PRV_CPSS_DXCH_DROP_PROFILE_CHECK_MAC(pfSet, devNum);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    if( poolNum >= SHARED_POOLS_NUM_CNS )
    {
        return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                txqVer1.queue.tailDrop.config.profilePriorityQueueToSharedPoolAssociation[pfSet];

    return prvCpssDxChHwPpSetRegField(devNum, regAddr, tc*3, 3, poolNum);
}

/*******************************************************************************
* cpssDxChPortTxTcProfileSharedPoolGet
*
* DESCRIPTION:
*       Gets the shared pool associated for traffic class and Profile.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum      - physical device number
*       pfSet       - the Profile Set in which the Traffic
*                     Class Drop Parameters is associated
*       tc          - the Traffic Class, range 0..7.
*
* OUTPUTS:
*       poolNumPtr  - (pointer to) shared pool associated.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, pfSet
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTcProfileSharedPoolGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   pfSet,
    IN  GT_U8                               tc,
    OUT GT_U32                              *poolNumPtr
)
{
    GT_U32  regAddr;    /* register address     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tc);
    CPSS_NULL_PTR_CHECK_MAC(poolNumPtr);
    PRV_CPSS_DXCH_DROP_PROFILE_CHECK_MAC(pfSet, devNum);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                txqVer1.queue.tailDrop.config.profilePriorityQueueToSharedPoolAssociation[pfSet];

    return prvCpssDxChHwPpGetRegField(devNum, regAddr, tc*3, 3, poolNumPtr);
}

/*******************************************************************************
* cpssDxChPortTxMcastBuffersLimitSet
*
* DESCRIPTION:
*       Set maximal buffers limits for multicast packets.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum            - physical device number
*       mcastMaxBufNum    - The number of buffers allocated for multicast
*                           packets. Range 0..0xFFFF
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range mcastMaxBufNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxMcastBuffersLimitSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  mcastMaxBufNum
)
{
    GT_U32  regAddr;    /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);


    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    if( mcastMaxBufNum >= BIT_16 )
    {
        return GT_OUT_OF_RANGE;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.tailDrop.
                    mcFilterLimits.multicastBuffersLimit;

    return prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 16, mcastMaxBufNum);
}

/*******************************************************************************
* cpssDxChPortTxMcastBuffersLimitGet
*
* DESCRIPTION:
*       Get maximal buffers limits for multicast packets.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum              - physical device number
*
* OUTPUTS:
*       mcastMaxBufNumPtr   - (pointer to) the number of buffers allocated
*                             for multicast packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxMcastBuffersLimitGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *mcastMaxBufNumPtr
)
{
    GT_U32  regAddr;    /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(mcastMaxBufNumPtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.tailDrop.
                    mcFilterLimits.multicastBuffersLimit;

    return prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 16, mcastMaxBufNumPtr);
}

/*******************************************************************************
* cpssDxChPortTxSniffedBuffersLimitSet
*
* DESCRIPTION:
*       Set maximal buffers limits for mirrored packets.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum            - physical device number
*       rxSniffMaxBufNum  - The number of buffers allocated for packets
*                           forwarded to the ingress analyzer port due to
*                           mirroring. Range 0..0xFFFF
*       txSniffMaxBufNum  - The number of buffers allocated for packets
*                           forwarded to the egress analyzer port due to
*                           mirroring. Range 0..0xFFFF
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on out of range value
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSniffedBuffersLimitSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  rxSniffMaxBufNum,
    IN  GT_U32  txSniffMaxBufNum
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regData;    /* register value   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    if( rxSniffMaxBufNum >= BIT_16 ||
        txSniffMaxBufNum >= BIT_16 )
    {
        return GT_OUT_OF_RANGE;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.tailDrop.
                    mcFilterLimits.mirroredPacketsToAnalyzerPortBuffersLimit;

    regData = rxSniffMaxBufNum;
    U32_SET_FIELD_MAC(regData, 16, 16, txSniffMaxBufNum);

    return prvCpssDxChHwPpWriteRegister(devNum, regAddr, regData);
}

/*******************************************************************************
* cpssDxChPortTxSniffedBuffersLimitGet
*
* DESCRIPTION:
*       Get maximal buffers limits for mirrored packets.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum              - physical device number
*
* OUTPUTS:
*       rxSniffMaxBufNumPtr - (pointer to) The number of buffers allocated
*                              for packets forwarded to the ingress analyzer
*                              port due to mirroring.
*       txSniffMaxBufNumPtr - (pointer to) The number of buffers allocated
*                              for packets forwarded to the egress analyzer
*                              port due to mirroring.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSniffedBuffersLimitGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *rxSniffMaxBufNumPtr,
    OUT GT_U32  *txSniffMaxBufNumPtr
)
{
    GT_STATUS rc;       /* return code      */
    GT_U32  regAddr;    /* register address */
    GT_U32  regData;    /* register value   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(rxSniffMaxBufNumPtr);
    CPSS_NULL_PTR_CHECK_MAC(txSniffMaxBufNumPtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.tailDrop.
                    mcFilterLimits.mirroredPacketsToAnalyzerPortBuffersLimit;

    rc =  prvCpssDxChHwPpReadRegister(devNum, regAddr, &regData);
    if( GT_OK != rc )
    {
        return rc;
    }

    *rxSniffMaxBufNumPtr = U32_GET_FIELD_MAC(regData, 0, 16);
    *txSniffMaxBufNumPtr = U32_GET_FIELD_MAC(regData, 16, 16);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxSharedPoolLimitsSet
*
* DESCRIPTION:
*       Set maximal descriptors and buffers limits for shared pool.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum     - physical device number
*       poolNum    - Shared pool number. Range 0..7
*       maxBufNum  - The number of buffers allocated for a shared pool.
*                    Range 0..0x3FFF.
*       maxDescNum - The number of descriptors allocated for a shared pool.
*                    Range 0..0x3FFF.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on out of range value
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSharedPoolLimitsSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  poolNum,
    IN  GT_U32  maxBufNum,
    IN  GT_U32  maxDescNum
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regData;    /* register value   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    if( poolNum >= SHARED_POOLS_NUM_CNS )
    {
        return GT_BAD_PARAM;
    }

    if( maxBufNum >= BIT_14 ||
        maxDescNum >= BIT_14 )
    {
        return GT_OUT_OF_RANGE;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.tailDrop.
                                            limits.sharedPoolLimits[poolNum];

    regData = maxDescNum;
    U32_SET_FIELD_MAC(regData, 14, 14, maxBufNum);

    return prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 28, regData);
}

/*******************************************************************************
* cpssDxChPortTxSharedPoolLimitsGet
*
* DESCRIPTION:
*       Get maximal descriptors and buffers limits for shared pool.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum        - physical device number
*       poolNum       - Shared pool number. Range 0..7
*
* OUTPUTS:
*       maxBufNumPtr  - (pointer to) The number of buffers allocated for a
*                       shared pool.
*       maxDescNumPtr - (pointer to) The number of descriptors allocated
*                       for a shared pool.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSharedPoolLimitsGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  poolNum,
    OUT GT_U32  *maxBufNumPtr,
    OUT GT_U32  *maxDescNumPtr
)
{
    GT_STATUS rc;       /* return code      */
    GT_U32  regAddr;    /* register address */
    GT_U32  regData;    /* register value   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(maxBufNumPtr);
    CPSS_NULL_PTR_CHECK_MAC(maxDescNumPtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    if( poolNum >= SHARED_POOLS_NUM_CNS  )
    {
        return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.tailDrop.
                                            limits.sharedPoolLimits[poolNum];

    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 28, &regData);
    if( GT_OK != rc )
    {
        return rc;
    }

    *maxDescNumPtr = U32_GET_FIELD_MAC(regData, 0, 14);
    *maxBufNumPtr = U32_GET_FIELD_MAC(regData, 14, 28);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxProfileWeightedRandomTailDropEnableSet
*
* DESCRIPTION:
*       Enable/disable Weighted Random Tail Drop Threshold to overcome
*       synchronization.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum      - device number.
*       pfSet       - the Profile Set in which the Traffic
*                     Class Drop Parameters is associated.
*       dp          - Drop Precedence
*                     (APPLICABLE RANGES: Lion 0..2; xCat2 0..1) 
*       tc          - the Traffic Class, range 0..7.
*                     (APPLICABLE DEVICES: xCat2)
*       enablersPtr - (pointer to) Tail Drop limits enabling 
*                     for Weigthed Random Tail Drop
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid input paramters value
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxProfileWeightedRandomTailDropEnableSet
(
    IN GT_U8                                    devNum,
    IN CPSS_PORT_TX_DROP_PROFILE_SET_ENT        pfSet,
    IN GT_U32                                   dp,
    IN GT_U8                                    tc,
    IN CPSS_PORT_TX_TAIL_DROP_WRTD_ENABLERS_STC *enablersPtr
)
{
    GT_U32 regAddr;    /* register address     */
    GT_U32 regData;    /* register value       */
    GT_U32 bitOffset;  /* bit offset inside register   */
    GT_U32 value;      /* value to write into register */
    GT_STATUS rc;      /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    CPSS_NULL_PTR_CHECK_MAC(enablersPtr);
    PRV_CPSS_DXCH_DROP_PROFILE_CHECK_MAC(pfSet, devNum);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        if( dp > 1 )
        {
            return GT_BAD_PARAM;
        }
        /* validate Traffic Classes queue */
        PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tc);

        bitOffset = dp*8 + tc;

        /* set Port WRTD En */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.setsConfigRegs.portWRTDEn[pfSet];
        value = BOOL2BIT_MAC(enablersPtr->portLimit);
        rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, bitOffset, 1, value);
        if (rc != GT_OK)
            return rc;

        /* set Queue WRTD En */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.setsConfigRegs.queueWRTDEn[pfSet];
        value = BOOL2BIT_MAC(enablersPtr->tcDpLimit);
        rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, bitOffset, 1, value);
        if (rc != GT_OK)
            return rc;

        /* set Shared Pool WRTD En */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.setsConfigRegs.sharedPoolWRTDEn[pfSet];
        value = BOOL2BIT_MAC(enablersPtr->sharedPoolLimit);
        rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, bitOffset, 1, value);
        if (rc != GT_OK)
            return rc;
    }
    else
    {
        if( dp > 2 )
        {
            return GT_BAD_PARAM;
        }
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.tailDrop.config.profileEnableWrtdDp[dp];

        regData = BOOL2BIT_MAC(enablersPtr->portLimit);
        U32_SET_FIELD_MAC(regData,  8, 1, BOOL2BIT_MAC(enablersPtr->tcLimit));
        U32_SET_FIELD_MAC(regData, 16, 1, BOOL2BIT_MAC(enablersPtr->tcDpLimit));
        U32_SET_FIELD_MAC(regData, 24, 1, BOOL2BIT_MAC(enablersPtr->sharedPoolLimit));

        rc = prvCpssDxChHwPpWriteRegBitMask(devNum, regAddr,
                                         (0x01010101 << pfSet) , regData << pfSet);
        if (rc != GT_OK)
            return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxProfileWeightedRandomTailDropEnableGet
*
* DESCRIPTION:
*       Get Random Tail drop Threshold status.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum      - device number.
*       pfSet       - the Profile Set in which the Traffic
*                     Class Drop Parameters is associated.
*       dp          - Drop Precedence
*                     (APPLICABLE RANGES: Lion 0..2; xCat2 0..1) 
*       tc          - the Traffic Class, range 0..7.
*                     (APPLICABLE DEVICES: xCat2)
*
* OUTPUTS:
*       enablersPtr - (pointer to) Tail Drop limits enabling 
*                     for Weigthed Random Tail Drop.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid input paramters value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxProfileWeightedRandomTailDropEnableGet
(
    IN  GT_U8                                    devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT        pfSet,
    IN  GT_U32                                   dp,
    IN  GT_U8                                    tc,
    OUT CPSS_PORT_TX_TAIL_DROP_WRTD_ENABLERS_STC *enablersPtr
)
{
    GT_STATUS rc;      /* return code          */
    GT_U32 regAddr;    /* register address     */
    GT_U32 regData;    /* register value       */
    GT_U32 bitOffset;  /* bit offset inside register   */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    CPSS_NULL_PTR_CHECK_MAC(enablersPtr);
    PRV_CPSS_DXCH_DROP_PROFILE_CHECK_MAC(pfSet, devNum);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        if( dp > 1 )
        {
            return GT_BAD_PARAM;
        }
        /* validate Traffic Classes queue */
        PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tc);

        bitOffset = dp*8 + tc;

        /* get Port WRTD En */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.setsConfigRegs.portWRTDEn[pfSet];
        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, bitOffset, 1, &regData);
        if (rc != GT_OK)
            return rc;
        enablersPtr->portLimit = BIT2BOOL_MAC(regData);

        /* get Queue WRTD En */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.setsConfigRegs.queueWRTDEn[pfSet];
        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, bitOffset, 1, &regData);
        if (rc != GT_OK)
            return rc;
        enablersPtr->tcDpLimit = BIT2BOOL_MAC(regData);

        /* get Shared Pool WRTD En */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.setsConfigRegs.sharedPoolWRTDEn[pfSet];
        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, bitOffset, 1, &regData);
        if (rc != GT_OK)
            return rc;
        enablersPtr->sharedPoolLimit = BIT2BOOL_MAC(regData);
    }
    else
    {
        if( dp > 2 )
        {
            return GT_BAD_PARAM;
        }
    
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        txqVer1.queue.tailDrop.config.profileEnableWrtdDp[dp];
    
        rc = prvCpssDxChHwPpReadRegister(devNum, regAddr, &regData);
        if( GT_OK != rc )
        {
            return rc;
        }
    
        regData = regData >> pfSet;
    
        enablersPtr->portLimit       = BIT2BOOL_MAC(U32_GET_FIELD_MAC(regData,  0, 1));
        enablersPtr->tcLimit         = BIT2BOOL_MAC(U32_GET_FIELD_MAC(regData,  8, 1));
        enablersPtr->tcDpLimit       = BIT2BOOL_MAC(U32_GET_FIELD_MAC(regData, 16, 1));
        enablersPtr->sharedPoolLimit = BIT2BOOL_MAC(U32_GET_FIELD_MAC(regData, 24, 1));
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxTailDropWrtdMasksSet
*
* DESCRIPTION:
*       Sets Weighted Random Tail Drop (WRTD) masks.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum     - physical device number
*       maskLsbPtr - WRTD masked least significant bits.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTailDropWrtdMasksSet
(
    IN  GT_U8                            devNum,
    IN  CPSS_PORT_TX_WRTD_MASK_LSB_STC  *maskLsbPtr
)
{
    GT_STATUS rc;       /* return code          */
    GT_U32  regAddr;    /* register address     */
    GT_U32  regData;    /* register value       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    CPSS_NULL_PTR_CHECK_MAC(maskLsbPtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        if( maskLsbPtr->port > 14 || maskLsbPtr->tcDp > 14 ||
            maskLsbPtr->pool > 14 )
        {
            return GT_OUT_OF_RANGE;
        }
        /* set WRTD mask for the Queue buffer/descriptor limits. */
        regData = BIT_MASK_0_31_MAC(maskLsbPtr->tcDp);
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.queueWRTDMask;
        rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 14, regData);
        if( GT_OK != rc )
            return rc;

        /* set WRTD mask for the Port buffer/descriptor limits. */

        regData = BIT_MASK_0_31_MAC(maskLsbPtr->port);
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.portWRTDMask;
        rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 14, regData);
        if( GT_OK != rc )
            return rc;

        /* set WRTD mask for the Shared Pool buffer/descriptor limits. */
        regData = BIT_MASK_0_31_MAC(maskLsbPtr->pool);
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.sharedPoolWRTDMask;
        rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 14, regData);
        if( GT_OK != rc )
            return rc;
    }
    else
    {
        if( maskLsbPtr->port > 9 || maskLsbPtr->tcDp > 9 ||
            maskLsbPtr->tc   > 9 || maskLsbPtr->pool > 9 )
        {
            return GT_OUT_OF_RANGE;
        }
    
        regData = BIT_MASK_0_31_MAC(maskLsbPtr->tc);
        U32_SET_FIELD_MAC(regData, 9, 9, BIT_MASK_0_31_MAC(maskLsbPtr->port));
    
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        txqVer1.queue.tailDrop.config.wrtdMask0;
    
        rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 18, regData);
        if( GT_OK != rc )
        {
            return rc;
        }
    
        regData = BIT_MASK_0_31_MAC(maskLsbPtr->tcDp);
        U32_SET_FIELD_MAC(regData, 9, 9, BIT_MASK_0_31_MAC(maskLsbPtr->pool));
    
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        txqVer1.queue.tailDrop.config.wrtdMask1;

        rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 18, regData);
        if( GT_OK != rc )
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxTailDropWrtdMasksGet
*
* DESCRIPTION:
*       Gets Weighted Random Tail Drop (WRTD) masks.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum     - physical device number
*
* OUTPUTS:
*       maskLsbPtr - (pointer to) WRTD masked least significant bits.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTailDropWrtdMasksGet
(
    IN  GT_U8                            devNum,
    OUT CPSS_PORT_TX_WRTD_MASK_LSB_STC  *maskLsbPtr
)
{
    GT_STATUS rc;       /* return code          */
    GT_U32  regAddr;    /* register address     */
    GT_U32  regData;    /* register value       */
    GT_U32  localMask;  /* bits mask from the register value    */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    CPSS_NULL_PTR_CHECK_MAC(maskLsbPtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* get WRTD mask for the Queue buffer/descriptor limits. */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.queueWRTDMask;
        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 14, &regData);
        if( GT_OK != rc )
            return rc;
        MSB_SET_GET_MAC(maskLsbPtr->tcDp , regData);

        /* get WRTD mask for the Port buffer/descriptor limits. */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.portWRTDMask;
        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 14, &regData);
        if( GT_OK != rc )
            return rc;
        MSB_SET_GET_MAC(maskLsbPtr->port , regData);

        /* get WRTD mask for the Shared Pool buffer/descriptor limits. */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.sharedPoolWRTDMask;
        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 14, &regData);
        if( GT_OK != rc )
            return rc;
        MSB_SET_GET_MAC(maskLsbPtr->pool , regData);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        txqVer1.queue.tailDrop.config.wrtdMask0;
    
        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 18, &regData);
        if( GT_OK != rc )
        {
            return rc;
        }
    
        localMask = U32_GET_FIELD_MAC(regData, 0, 9);
        MSB_SET_GET_MAC(maskLsbPtr->tc , localMask);
        localMask = U32_GET_FIELD_MAC(regData, 9, 9);
        MSB_SET_GET_MAC(maskLsbPtr->port , localMask);
    
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        txqVer1.queue.tailDrop.config.wrtdMask1;
    
        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 18, &regData);
        if( GT_OK != rc )
        {
            return rc;
        }
    
        localMask = U32_GET_FIELD_MAC(regData, 0, 9);
        MSB_SET_GET_MAC(maskLsbPtr->tcDp , localMask);
        localMask = U32_GET_FIELD_MAC(regData, 9, 9);
        MSB_SET_GET_MAC(maskLsbPtr->pool , localMask);
    }
    
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxResourceHistogramThresholdSet
*
* DESCRIPTION:
*       Sets threshold for Histogram counter increment.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum      - physical device number
*       cntrNum     - Histogram Counter number. Range 0..3.
*       threshold   - If the Global Descriptors Counter exceeds this Threshold,
*                     the Histogram Counter is incremented by 1.
*                     Range 0..0x3FFF.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, cntrNum
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxResourceHistogramThresholdSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  cntrNum,
    IN  GT_U32  threshold
)
{
    GT_U32  regAddr;    /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);


    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    switch(cntrNum)
    {
        case 0:
        case 1: regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.tailDrop.resourceHistogram.limitRegister1;
                break;
        case 2:
        case 3: regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.tailDrop.resourceHistogram.limitRegister2;
                break;
        default: return GT_BAD_PARAM;
    }

    if( threshold >= BIT_14 )
    {
        return GT_OUT_OF_RANGE;
    }

    return prvCpssDxChHwPpSetRegField(devNum, regAddr, (cntrNum & 0x1)*14, 14, threshold);
}

/*******************************************************************************
* cpssDxChPortTxResourceHistogramThresholdGet
*
* DESCRIPTION:
*       Gets threshold for Histogram counter increment.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum          - physical device number
*       cntrNum         - Histogram Counter number. Range 0..3.
*
* OUTPUTS:
*       thresholdPtr    - (pointer to) If the Global Descriptors Counter
*                         exceeds this Threshold, the Histogram Counter
*                         is incremented by 1.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, cntrNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxResourceHistogramThresholdGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  cntrNum,
    OUT GT_U32  *thresholdPtr
)
{
    GT_U32  regAddr;    /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(thresholdPtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    switch(cntrNum)
    {
        case 0:
        case 1: regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.tailDrop.resourceHistogram.limitRegister1;
                break;
        case 2:
        case 3: regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.tailDrop.resourceHistogram.limitRegister2;
                break;
        default: return GT_BAD_PARAM;
    }

    return prvCpssDxChHwPpGetRegField(devNum, regAddr, (cntrNum & 0x1)*14, 14, thresholdPtr);
}

/*******************************************************************************
* cpssDxChPortTxResourceHistogramCounterGet
*
* DESCRIPTION:
*       Gets Histogram Counter.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum  - physical device number
*       cntrNum - Histogram Counter number. Range 0..3.
*
* OUTPUTS:
*       cntrPtr - (pointer to) Histogram Counter value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, cntrNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Histogram Counter is cleared on read.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxResourceHistogramCounterGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  cntrNum,
    OUT GT_U32  *cntrPtr
)
{
    GT_U32  regAddr;    /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(cntrPtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    if( cntrNum > 3 )
    {
        return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                txqVer1.queue.tailDrop.resourceHistogram.counters[cntrNum];

    return prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 14, cntrPtr);
}

/*******************************************************************************
* cpssDxChPortTxGlobalDescNumberGet
*
* DESCRIPTION:
*       Gets total number of descriptors allocated.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       numberPtr - number of descriptors
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxGlobalDescNumberGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *numberPtr
)
{
    GT_U32 regAddr; /* register address     */
    GT_U32 len;     /* register data length */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(numberPtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.totalDescCounter;
        len = 16;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.tailDrop.counters.totalDescCounter;
        len = 14;
    }

    return prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, len, numberPtr);
}

/*******************************************************************************
* cpssDxChPortTxGlobalBufNumberGet
*
* DESCRIPTION:
*       Gets total number of virtual buffers enqueued.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       numberPtr   - number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The buffers associated with enqueued multi-target descriptors are
*       counted multiple times, once for each multi-target descriptor instance.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxGlobalBufNumberGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *numberPtr
)
{
    GT_U32 regAddr; /* register address     */
    GT_U32 len;     /* register data length */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(numberPtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.totalBuffersCounter;
        len = 18;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        txqVer1.queue.tailDrop.counters.totalBuffersCounter;
        len = 14;
    }

    return prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, len, numberPtr);
}


/*******************************************************************************
* cpssDxChPortTxMcastDescNumberGet
*
* DESCRIPTION:
*       Gets total number of multi-target descriptors allocated.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       numberPtr - number of descriptors
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Multi-target descriptors are descriptors that are duplicated to
*       multiple egress targets, e.g. Multicast, Broadcast, or unknown Unicast
*       flooded packets.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxMcastDescNumberGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *numberPtr
)
{
    GT_U32 regAddr; /* register address     */
    GT_U32 len;     /* register data length */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(numberPtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.multicastDescriptorsCounter;
        len = 14;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                txqVer1.queue.tailDrop.mcFilterCounters.multicastDescriptorsCounter;
        len = 16;
    }

    return prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, len, numberPtr);
}

/*******************************************************************************
* cpssDxChPortTxMcastBufNumberGet
*
* DESCRIPTION:
*       Gets total number of multi-target buffers allocated (virtual buffers).
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       numberPtr   - number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The buffers associated with enqueued multi-target descriptors are
*       counted multiple times, once for each multi-target descriptor instance.
*       Multi-target descriptors are descriptors that are duplicated to
*       multiple egress targets, e.g. Multicast, Broadcast, or unknown Unicast
*       flooded packets.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxMcastBufNumberGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *numberPtr
)
{
    GT_U32 regAddr; /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(numberPtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                txqVer1.queue.tailDrop.mcFilterCounters.multicastBuffersCounter;

    return prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 16, numberPtr);
}

/*******************************************************************************
* cpssDxChPortTxSniffedDescNumberGet
*
* DESCRIPTION:
*       Gets total number of mirrored descriptors allocated.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       rxNumberPtr - number of ingress mirrored descriptors.
*       txNumberPtr - number of egress mirrored descriptors.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSniffedDescNumberGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *rxNumberPtr,
    OUT GT_U32  *txNumberPtr
)
{
    GT_STATUS rc;       /* return code      */
    GT_U32 regAddr;     /* register address */
    GT_U32 regData;     /* register value   */
    GT_U32 fieldLen;    /* register field length */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(rxNumberPtr);
    CPSS_NULL_PTR_CHECK_MAC(txNumberPtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.snifferDescriptorsCounter;
        fieldLen = 14;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.tailDrop.mcFilterCounters.snifferDescriptorsCounter;
        fieldLen = 16;
    }

    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 2*fieldLen, &regData);
    if( GT_OK != rc )
    {
        return rc;
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
       *txNumberPtr = U32_GET_FIELD_MAC(regData, 0, fieldLen);
       *rxNumberPtr = U32_GET_FIELD_MAC(regData, fieldLen, fieldLen);
    }
    else
    {
       *rxNumberPtr = U32_GET_FIELD_MAC(regData, 0, fieldLen);
       *txNumberPtr = U32_GET_FIELD_MAC(regData, fieldLen, fieldLen);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxSniffedBufNumberGet
*
* DESCRIPTION:
*       Gets total number of mirrored buffers allocated.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       rxNumberPtr - number of ingress mirrored buffers.
*       txNumberPtr - number of egress mirrored buffers.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSniffedBufNumberGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *rxNumberPtr,
    OUT GT_U32  *txNumberPtr
)
{
    GT_STATUS rc;   /* return code      */
    GT_U32 regAddr; /* register address */
    GT_U32 regData; /* register value   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(rxNumberPtr);
    CPSS_NULL_PTR_CHECK_MAC(txNumberPtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                txqVer1.queue.tailDrop.mcFilterCounters.snifferBuffersCounter;

    rc = prvCpssDxChHwPpReadRegister(devNum, regAddr, &regData);
    if( GT_OK != rc )
    {
        return rc;
    }

    *rxNumberPtr = U32_GET_FIELD_MAC(regData, 0, 16);
    *txNumberPtr = U32_GET_FIELD_MAC(regData, 16, 16);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxSharedResourceDescNumberGet
*
* DESCRIPTION:
*       Gets total number of descriptors allocated from the shared descriptors
*       pool.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum    - physical device number
*       poolNum   - shared pool number, range 0..7
*
* OUTPUTS:
*       numberPtr - number of descriptors
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong poolNum or devNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSharedResourceDescNumberGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  poolNum,
    OUT GT_U32  *numberPtr
)
{
    GT_U32 regAddr; /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(numberPtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    if( poolNum >= SHARED_POOLS_NUM_CNS )
    {
        return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                txqVer1.queue.tailDrop.counters.tcSharedDescCounter[poolNum];

    return prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 14, numberPtr);
}

/*******************************************************************************
* cpssDxChPortTxSharedResourceBufNumberGet
*
* DESCRIPTION:
*       Gets the number of virtual buffers enqueued from the shared buffer pool.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum    - physical device number
*       poolNum   - shared pool number, range 0..7
*
* OUTPUTS:
*       numberPtr - number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong poolNum or devNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSharedResourceBufNumberGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  poolNum,
    OUT GT_U32  *numberPtr
)
{
    GT_U32 regAddr; /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(numberPtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    if( poolNum >= SHARED_POOLS_NUM_CNS )
    {
        return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                txqVer1.queue.tailDrop.counters.tcSharedBuffersCounter[poolNum];

    return prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 14, numberPtr);
}

/*******************************************************************************
* cpssDxChPortTxShaperOnStackAsGigEnableSet
*
* DESCRIPTION:
*       Enables/Disables the shaper on stacking ports (24-27) to operate as
*       Tri Speed ports' shapers.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum  - physical device number
*       enable  - GT_TRUE, stacking ports shapers operates
*                          as Tri Speed ports' shapers.
*                 GT_FALSE, stacking ports shapers operates as XG ports.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       When "Stack As Gig" enabled, for a port number in the range 24..27,
*       whether Gig or XG type, the global shaper used parameters are those
*       referenced as gig ones -
*       "gigPortsTokensRate" and "gigPortsSlowRateRatio" inputs for
*       "cpssDxChPortTxShaperGlobalParamsSet" API.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperOnStackAsGigEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
)
{
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* function return code */
    GT_U32      value;      /* value to write into register */
    GT_U32      fieldOffset;/* the start bit number in the register */
    GT_BOOL     oldEnable;
    GT_U8       port;
    GT_U32      tcQueue = 0;
    GT_U32      xgPortsTokensRate;
    GT_U32      gigPortsTokensRate;
    GT_U32      gigPortsSlowRateRatio;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        PRV_CPSS_DXCH_XCAT_A3_ONLY_DEV_CHECK_MAC(devNum);
    }

    regAddr = PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum) ?
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQMcFifoEccConfig :
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueMiscCtrl;

    fieldOffset = PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum) ? 31 : 10;

    /* read the old value */
    rc = prvCpssDrvHwPpGetRegField(devNum,regAddr,fieldOffset,1,&value);
    if(rc != GT_OK)
    {
        return rc;
    }

    oldEnable = BIT2BOOL_MAC(value);

    /* do nothing if same value */
    if (enable == oldEnable)
    {
        return GT_OK;
    }

    /* write register value */
    value = BOOL2BIT_MAC(enable);
    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,fieldOffset,1,value);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* get global shaper params */
    rc = cpssDxChPortTxShaperGlobalParamsGet(devNum,
                                             &xgPortsTokensRate,
                                             &gigPortsTokensRate,
                                             &gigPortsSlowRateRatio);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* update TB parameters on stacking ports */
    for (port = 24; port < 28; port++)
    {
        if (! PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, port))
            continue;

        /* per port */
        rc = prvDxChPortTxSetShaperTokenBucketParams(
                devNum,
                port,
                GT_FALSE,
                tcQueue,
                xgPortsTokensRate,
                gigPortsTokensRate,
                gigPortsSlowRateRatio,
                oldEnable,
                xgPortsTokensRate,
                gigPortsTokensRate,
                gigPortsSlowRateRatio,
                enable);

        if (rc != GT_OK)
        {
            return rc;
        }

        /* per port per traffic class (tcQueue) */
        for (tcQueue = 0; tcQueue < CPSS_TC_RANGE_CNS; tcQueue++)
        {
            rc = prvDxChPortTxSetShaperTokenBucketParams(
                    devNum,
                    port,
                    GT_TRUE,
                    tcQueue,
                    xgPortsTokensRate,
                    gigPortsTokensRate,
                    gigPortsSlowRateRatio,
                    oldEnable,
                    xgPortsTokensRate,
                    gigPortsTokensRate,
                    gigPortsSlowRateRatio,
                    enable);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    return rc;
}

/*******************************************************************************
* cpssDxChPortTxShaperOnStackAsGigEnableGet
*
* DESCRIPTION:
*       Gets the current state (enable/disable) of the shaper's mode on
*       stacking ports (24-27).
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                 GT_TRUE, stacking ports shapers operates
*                          as Tri Speed ports' shapers.
*                 GT_FALSE, stacking ports shapers operates as XG ports.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperOnStackAsGigEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32    regAddr;     /* register address */
    GT_STATUS rc;          /* function return code */
    GT_U32    value;       /* value to read from the register */
    GT_U32    fieldOffset; /* the start bit number in the register */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        PRV_CPSS_DXCH_XCAT_A3_ONLY_DEV_CHECK_MAC(devNum);
    }
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* read register value */
    regAddr = PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum) ?
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQMcFifoEccConfig :
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueMiscCtrl;

    fieldOffset = PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum) ? 31 : 10;

    rc = prvCpssDrvHwPpGetRegField(devNum,regAddr,fieldOffset,1,&value);

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;
}

/*******************************************************************************
* cpssDxChPortTxByteCountChangeEnableSet
*
* DESCRIPTION:
*       Enables/Disables Byte Count Change of the packet length by per port
*       constant for shaping and/or scheduling rate calculation.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum - physical device number.
*       bcChangeEnable - Byte Count Change enable opition.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number or BC change option
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxByteCountChangeEnableSet
(
    IN  GT_U8     devNum,
    IN  CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_ENT bcChangeEnable
)
{
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* function return code */
    GT_U32      value;      /* value to write into register */
    GT_U32      mask;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        PRV_CPSS_DXCH_XCAT_A3_ONLY_DEV_CHECK_MAC(devNum);
    }

    switch (bcChangeEnable)
    {
        case CPSS_DXCH_PORT_TX_BC_CHANGE_DISABLE_ALL_E:
            value = 0;
            break;
        case CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_SHAPER_ONLY_E:
            value = PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum) ? 0x600 : 0x300;
            break;
        case CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_SCHEDULER_ONLY_E:
            value = PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum) ? 0xA00 : 0x900;
            break;
        case CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_SHAPER_AND_SCHEDULER_E:
            value = PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum) ? 0xE00 : 0xB00;
            break;
        default:
            return GT_BAD_PARAM;
    }

    mask = PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum) ? 0xE00 : 0xB00;

    /* write register value */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueMiscCtrl;
    rc = prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, mask, value);

    return rc;
}

/*******************************************************************************
* cpssDxChPortTxByteCountChangeEnableGet
*
* DESCRIPTION:
*       Gets the Byte Count Change enable opition.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum - physical device number.
*
* OUTPUTS:
*       bcChangeEnablePtr - (pointer to) Byte Count Change enable opition.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxByteCountChangeEnableGet
(
    IN  GT_U8     devNum,
    OUT CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_ENT *bcChangeEnablePtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* function return code */
    GT_U32      value;      /* value to read from the register  */
    GT_BOOL     shaperEnabled,schedulerEnabled;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        PRV_CPSS_DXCH_XCAT_A3_ONLY_DEV_CHECK_MAC(devNum);
    }
    CPSS_NULL_PTR_CHECK_MAC(bcChangeEnablePtr);

    /* read register value */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueMiscCtrl;
    rc = prvCpssDrvHwPpGetRegField(devNum,regAddr,9,3,&value);
    if (rc != GT_OK)
        return rc;

    shaperEnabled = PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum) ? 
        BIT2BOOL_MAC((value & 0x2) >> 1) :
        BIT2BOOL_MAC(value & 0x1);
    schedulerEnabled = BIT2BOOL_MAC((value & 0x4) >> 2);

    if (shaperEnabled == GT_FALSE && schedulerEnabled == GT_FALSE)
    {
        *bcChangeEnablePtr = CPSS_DXCH_PORT_TX_BC_CHANGE_DISABLE_ALL_E;
    }
    else if(shaperEnabled == GT_TRUE && schedulerEnabled == GT_FALSE)
    {
        *bcChangeEnablePtr = CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_SHAPER_ONLY_E;
    }
    else if(shaperEnabled == GT_FALSE && schedulerEnabled == GT_TRUE)
    {
        *bcChangeEnablePtr = CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_SCHEDULER_ONLY_E;
    }
    else
    {
        *bcChangeEnablePtr = CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_SHAPER_AND_SCHEDULER_E;
    }

    return GT_OK;
}


/*******************************************************************************
* cpssDxChPortTxByteCountChangeValueSet
*
* DESCRIPTION:
*       Sets the number of bytes added/subtracted to the length of every frame
*       for shaping/scheduling rate calculation for given port when
*       Byte Count Change is enabled.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical or CPU port number
*       bcValue - number of bytes added to the length of every frame
*                 for shaping/scheduling rate calculation.(-63..63)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. To globally enable/disable Byte Count Change,
*          use cpssDxChPortTxByteCountChangeEnableSet
*       2. To update bcValue for given port Byte Count Change globally disabled.
*          This can cause side effects to shaper/scheduler behavior
*          on other ports if performed under traffic.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxByteCountChangeValueSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_32     bcValue
)
{
    GT_U32      value;      /* value to write */
    GT_STATUS   rc;         /* function return code */
    GT_U32      regAddr;    /* register address */
    GT_BOOL     bcEnable;
    GT_U32      enableBcChangefieldOffset;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        PRV_CPSS_DXCH_XCAT_A3_ONLY_DEV_CHECK_MAC(devNum);
    }
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

    enableBcChangefieldOffset = PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum) ? 9 : 8;

    /* The field MUST be updated only when the feature is globally disabled.*/
    /* Get the value of <EnableBcChange> */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueMiscCtrl;
    rc = prvCpssDrvHwPpGetRegField(devNum,regAddr,enableBcChangefieldOffset,1,&value);
    if (rc != GT_OK)
        return rc;

    /* Disable if enabled */
    bcEnable = BIT2BOOL_MAC(value);
    if (bcEnable == GT_TRUE)
    {
        rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,enableBcChangefieldOffset,1,0);
        if (rc != GT_OK)
            return rc;
    }

    /* prepare value to write */
    if (bcValue >= 0 && bcValue < 64)
    {
        value = bcValue;
    }
    else if (bcValue < 0 && bcValue > -64)
    {
        value = bcValue * (-1);
        value |= 1 << 6;
    }
    else
    {
        return GT_OUT_OF_RANGE;
    }

    if(portNum == CPSS_CPU_PORT_NUM_CNS)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            egrTxQConf.txCpuRegs.l1OverheadConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            egrTxQConf.txPortRegs[portNum].l1OverheadConfig;
    }

     /* write register value */
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 4, 7, value);
    if (rc != GT_OK)
        return rc;

    /* restore EnableBcChange value */
    if (bcEnable == GT_TRUE)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueMiscCtrl;
        rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,enableBcChangefieldOffset,1,1);
        if (rc != GT_OK)
            return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortTxByteCountChangeValueGet
*
* DESCRIPTION:
*       Gets the number of bytes added/subtracted to the length of every frame
*       for shaping/scheduling rate calculation for given port when
*       Byte Count Change is enabled.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical or CPU port number
*
* OUTPUTS:
*       bcValuePtr - (pointer to) number of bytes added to the length of
*                    every frame for shaping/scheduling rate calculation.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxByteCountChangeValueGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT GT_32    *bcValuePtr
)
{
    GT_U32      value;      /* value to write */
    GT_STATUS   rc;         /* function return code */
    GT_U32      regAddr;    /* register address */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        PRV_CPSS_DXCH_XCAT_A3_ONLY_DEV_CHECK_MAC(devNum);
    }
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(bcValuePtr);

    if(portNum == CPSS_CPU_PORT_NUM_CNS)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            egrTxQConf.txCpuRegs.l1OverheadConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            egrTxQConf.txPortRegs[portNum].l1OverheadConfig;
    }

     /* read register value */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 4, 7, &value);
    if (rc != GT_OK)
        return rc;

    *bcValuePtr = (value & 0x40) ? (value & 0x3F) * -1 :
                                   (value & 0x3F);
    return GT_OK;
}
