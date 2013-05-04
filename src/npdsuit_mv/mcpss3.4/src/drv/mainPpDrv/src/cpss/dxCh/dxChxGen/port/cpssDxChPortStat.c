/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortStat.c
*
* DESCRIPTION:
*       CPSS implementation for core port statistics and
*       egress counters facility.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* macro needed to support the call to PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC */
/* this define must come before include files */
#define PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_SUPPORTED_FLAG_CNS
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortStat.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h>
#include <cpssCommon/private/prvCpssMath.h>

#ifdef PRESTERA_DEBUG
#define PORT_DEBUG
#endif /* PRESTERA_DEBUG */

#ifdef PORT_DEBUG
#define DBG_INFO(x) osPrintf x
#else
#define DBG_INFO(x)
#endif

/* does this port uses the GE port mechanism or the XG mechanism */
/* 1 - port uses GE port mechanism */
/* 0 - port uses XG port mechanism */
#define IS_GE_PORT_COUNTERS_MAC(_devNum,_portNum)                    \
    ((PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(_devNum,_portNum) == GT_FALSE &&    \
      PRV_CPSS_DXCH_PORT_TYPE_MAC(_devNum,_portNum) <= PRV_CPSS_PORT_GE_E) ? \
      ((_portNum < 24) ? 1 : 0) : 0)

/******* info about Ethernet CPU port ******************************************
    support only next counters:

    1. CPSS_GOOD_PKTS_SENT_E:      32 bits counter
    2. CPSS_MAC_TRANSMIT_ERR_E:    16 bits counter
    3. CPSS_GOOD_OCTETS_SENT_E:    32 bits counter
    4. CPSS_DROP_EVENTS_E:         16 bits counter
    5. CPSS_GOOD_PKTS_RCV_E:       16 bits counter
    6. CPSS_BAD_PKTS_RCV_E:        16 bits counter
    7. CPSS_GOOD_OCTETS_RCV_E:     32 bits counter
    8. CPSS_BAD_OCTETS_RCV_E:      16 bits counter

*******************************************************************************/

/* array of MAC counters offsets */
/* gtMacCounterOffset[0] - offsets for not XGMII interface */
static GT_U8 gtMacCounterOffset[CPSS_LAST_MAC_COUNTER_NUM_E];

/*******************************************************************************
* cpssDxChPortStatInit
*
* DESCRIPTION:
*       Init port statistics counter set CPSS facility.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
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
*
*******************************************************************************/
GT_STATUS cpssDxChPortStatInit
(
    IN  GT_U8       devNum
)
{
    GT_U8  i;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    gtMacCounterOffset[CPSS_GOOD_OCTETS_RCV_E]  = 0x0;
    gtMacCounterOffset[CPSS_BAD_OCTETS_RCV_E]   = 0x8;
    gtMacCounterOffset[CPSS_GOOD_OCTETS_SENT_E] = 0x38;
    gtMacCounterOffset[CPSS_GOOD_UC_PKTS_RCV_E] = 0x10;
    gtMacCounterOffset[CPSS_GOOD_UC_PKTS_SENT_E] = 0x40;
    gtMacCounterOffset[CPSS_MULTIPLE_PKTS_SENT_E] = 0x50;
    gtMacCounterOffset[CPSS_DEFERRED_PKTS_SENT_E] = 0x14;

    for (i = CPSS_MAC_TRANSMIT_ERR_E; i <= CPSS_GOOD_OCTETS_SENT_E; i++)
        gtMacCounterOffset[i] = (GT_U8)(0x4 + (i * 4));

    for (i = CPSS_GOOD_OCTETS_SENT_E + 1; i < CPSS_GOOD_UC_PKTS_RCV_E; i++)
        gtMacCounterOffset[i] = (GT_U8)(0x8 + (i * 4));

    return GT_OK;
}

/*******************************************************************************
* dxChMacCountersCpuPortGet
*
* DESCRIPTION:
*       Gets Ethernet CPU port MAC counter .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*                  CPU port counters are valid only when using "Ethernet CPU
*                  port" (not using the SDMA interface).
*                  when using the SDMA interface refer to the API
*                  cpssDxChNetIfSdmaRxCountersGet(...)
*
* INPUTS:
*       devNum   - physical device number
*       cntrName - specific counter name
*
* OUTPUTS:
*       cntrValuePtr - (pointer to) current counter value.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_NOT_SUPPORTED - not supported counter on the port
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS dxChMacCountersCpuPortGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_PORT_MAC_COUNTERS_ENT  cntrName,
    OUT GT_U64                      *cntrValuePtr
)
{
    GT_U32  regAddr = /*0x00000060*/
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.macCounters;

    switch(cntrName)
    {
        case CPSS_GOOD_PKTS_SENT_E:
            regAddr += 0x0; /* 32 bits */
            break;
        case CPSS_MAC_TRANSMIT_ERR_E:
            regAddr += 0x4; /* 16 bits */
            break;
        case CPSS_GOOD_OCTETS_SENT_E:
            regAddr += 0x8; /* 32 bits */
            break;
        case CPSS_DROP_EVENTS_E:
            regAddr += 0xC; /* 16 bits */
            break;
        case CPSS_GOOD_PKTS_RCV_E:
            regAddr += 0x10;/* 16 bits */
            break;
        case CPSS_BAD_PKTS_RCV_E:
            regAddr += 0x14;/* 16 bits */
            break;
        case CPSS_GOOD_OCTETS_RCV_E:
            regAddr += 0x18;/* 32 bits */
            break;
        case CPSS_BAD_OCTETS_RCV_E:
            regAddr += 0x1c;/* 16 bits */
            break;
        default:
            /* not supported counter */
            return GT_NOT_SUPPORTED;
    }

    return prvCpssDrvHwPpPortGroupReadRegister(devNum, PRV_CPSS_CPU_PORT_PORT_GROUP_ID_MAC(devNum),
            regAddr, &(cntrValuePtr->l[0]));
}

/*******************************************************************************
* prvCpssDxChMacCounterGet
*
* DESCRIPTION:
*       Gets Ethernet MAC counter /  MAC Captured counter for a
*       specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number,
*                        CPU port if getFromCapture is GT_FALSE
*       cntrName       - specific counter name
*       getFromCapture -  GT_TRUE -  Gets the captured Ethernet MAC counter
*                         GT_FALSE - Gets the Ethernet MAC counter
*
* OUTPUTS:
*       cntrValuePtr - (pointer to) current counter value.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number, device or counter name
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     The 10G MAC MIB counters are 64-bit wide.
*     Not supported counters: CPSS_BAD_PKTS_RCV_E, CPSS_UNRECOG_MAC_CNTR_RCV_E,
*     CPSS_BadFC_RCV_E, CPSS_GOOD_PKTS_RCV_E, CPSS_GOOD_PKTS_SENT_E.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChMacCounterGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    IN  CPSS_PORT_MAC_COUNTERS_ENT  cntrName,
    IN  GT_BOOL                     getFromCapture,
    OUT GT_U64                      *cntrValuePtr
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 baseRegAddr;     /* base register address */
    GT_U32 regVal;        /* register value */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    if(getFromCapture == GT_TRUE)
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    }

    CPSS_NULL_PTR_CHECK_MAC(cntrValuePtr);

    if((GT_U32)cntrName >= (GT_U32)CPSS_LAST_MAC_COUNTER_NUM_E)
        return GT_BAD_PARAM;

    cntrValuePtr->l[0] = 0;
    cntrValuePtr->l[1] = 0;

    if(portNum == CPSS_CPU_PORT_NUM_CNS)
    {
        /* the CPU port support only 8 counters */
        return dxChMacCountersCpuPortGet(devNum,cntrName,cntrValuePtr);
    }

    /* these counters do NOT exist */
    if ( (cntrName == CPSS_BAD_PKTS_RCV_E) ||
         (cntrName == CPSS_UNRECOG_MAC_CNTR_RCV_E) ||
         (cntrName == CPSS_BadFC_RCV_E) ||
         (cntrName == CPSS_GOOD_PKTS_RCV_E) ||
         (cntrName == CPSS_GOOD_PKTS_SENT_E))
    {
        return GT_BAD_PARAM;
    }

    /* these counters do NOT exist for 10 Giga ports */
    if( (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) >= PRV_CPSS_PORT_XG_E) &&
        ( (cntrName == CPSS_EXCESSIVE_COLLISIONS_E) ||
          (cntrName == CPSS_COLLISIONS_E) ||
          (cntrName == CPSS_LATE_COLLISIONS_E) ||
          (cntrName == CPSS_MULTIPLE_PKTS_SENT_E) ||
          (cntrName == CPSS_DEFERRED_PKTS_SENT_E)))
    {
        return GT_BAD_PARAM;
    }

    if(getFromCapture == GT_TRUE)
    {
        baseRegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                  perPortRegs[portNum].macCaptureCounters;
    }
    else
    {
        baseRegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                  perPortRegs[portNum].macCounters;
    }

    regAddr = baseRegAddr + gtMacCounterOffset[cntrName];

    if ( prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,
            regAddr, &(cntrValuePtr->l[0])) != GT_OK)
        return GT_HW_ERROR;


    switch (cntrName)
    {
        case CPSS_GOOD_OCTETS_RCV_E:
        case CPSS_GOOD_OCTETS_SENT_E:
            /* this counter has 64 bits */
            regAddr = regAddr + 4;
            if ( prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,
                    regAddr, &(cntrValuePtr->l[1])) != GT_OK)
                return GT_HW_ERROR;
            break;

        case CPSS_OVERSIZE_PKTS_E:
            /* Wrong read value for oversize and jabber counters in XG MIBs
               (FEr#85)*/
            if((PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                PRV_CPSS_DXCH_XG_MIB_READ_OVERSIZE_AND_JABBER_COUNTERS_WA_E ==
                GT_TRUE)) &&
                (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) >= PRV_CPSS_PORT_XG_E))
            {
                /* Dummy read of GT_JABBER_PKTS */
                regAddr = baseRegAddr + gtMacCounterOffset[cntrName + 1];

                if (prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,
                        regAddr, &regVal) != GT_OK)
                   return GT_HW_ERROR;
            }
            break;
        case CPSS_JABBER_PKTS_E:
            if((PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                PRV_CPSS_DXCH_XG_MIB_READ_OVERSIZE_AND_JABBER_COUNTERS_WA_E ==
                GT_TRUE)) &&
                (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) >= PRV_CPSS_PORT_XG_E))
            {
                /* Last read was GT_OVERSIZE_PKTS. Read again. */
                if (prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,
                        regAddr, &(cntrValuePtr->l[0])) != GT_OK)
                    return GT_HW_ERROR;
            }
            break;
        default:
            break;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChMacCounterGet
*
* DESCRIPTION:
*       Gets Ethernet MAC counter for a specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*                  CPU port counters are valid only when using "Ethernet CPU
*                  port" (not using the SDMA interface).
*                  when using the SDMA interface refer to the API
*                  cpssDxChNetIfSdmaRxCountersGet(...)
*       cntrName - specific counter name
*
* OUTPUTS:
*       cntrValuePtr - (pointer to) current counter value.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number, device or counter name
*       GT_HW_ERROR  - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     The 10G MAC MIB counters are 64-bit wide.
*     Not supported counters: CPSS_BAD_PKTS_RCV_E, CPSS_UNRECOG_MAC_CNTR_RCV_E,
*     CPSS_BadFC_RCV_E, CPSS_GOOD_PKTS_RCV_E, CPSS_GOOD_PKTS_SENT_E.
*
*******************************************************************************/
GT_STATUS cpssDxChMacCounterGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    IN  CPSS_PORT_MAC_COUNTERS_ENT  cntrName,
    OUT GT_U64                      *cntrValuePtr
)
{
    PRV_CPSS_DXCH_ERRATA_STACK_MAC_COUNT_SHADOW_STC *macCountShadowPtr; /* Pointer To Shadow of all
                                                    MAC Counters of ports 24-27 */
    GT_BOOL                                         clearOnReadWa;      /* clear On Read WA   */
    GT_U64                                          cntrValue;          /* counter value      */
    GT_U64                                          *cntrShadowPtr;     /* Pointer To Shadow of
                                                    MAC Counter specified by function parameters */
    GT_STATUS                                       rc;                 /* return code        */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(cntrValuePtr);
    if((GT_U32)cntrName >= (GT_U32)CPSS_LAST_MAC_COUNTER_NUM_E)
        return GT_BAD_PARAM;

    /* default */
    clearOnReadWa     = GT_FALSE;
    macCountShadowPtr = NULL;

    if (PRV_CPSS_DXCH_XCAT2_STACK_MAC_COUNT_NO_CLEAR_ON_READ_WA_MAC(devNum, portNum))
    {
        macCountShadowPtr =
        PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
            info_PRV_CPSS_DXCH_XCAT2_STACK_MAC_COUNT_NO_CLEAR_ON_READ_WA_E.
            stackMacCntPtr;
        clearOnReadWa =
            macCountShadowPtr->stackPortArr
            [portNum - PRV_CPSS_DXCH_ERRATA_STACK_PORT_BASE_CNS].clearOnRead;
    }

    rc = prvCpssDxChMacCounterGet(
        devNum, portNum, cntrName, GT_FALSE, &cntrValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (clearOnReadWa == GT_FALSE)
    {
        *cntrValuePtr = cntrValue;
        return GT_OK;
    }

    /* clearOnRead Work Around implementation */

    cntrShadowPtr =
        &(macCountShadowPtr->stackPortArr[portNum - 24].
          macCntrShadow[cntrName]);

    /* subtract the shadowed value instead of clearing the counter */
    *cntrValuePtr = prvCpssMathSub64(cntrValue, *cntrShadowPtr);

    /* update the counter shadow */
    *cntrShadowPtr = cntrValue;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortMacCountersOnPortGet
*
* DESCRIPTION:
*       Gets Ethernet MAC counter for a particular Port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number,
*                        CPU port if getFromCapture is GT_FALSE
*       getFromCapture  -  GT_TRUE -  Gets the captured Ethernet MAC counter
*                         GT_FALSE - Gets the Ethernet MAC counter
*
* OUTPUTS:
*       portMacCounterSetArrayPtr - (pointer to) array of current counter values.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     Not supported counters: CPSS_BAD_PKTS_RCV_E, CPSS_UNRECOG_MAC_CNTR_RCV_E,
*     CPSS_BadFC_RCV_E, CPSS_GOOD_PKTS_RCV_E, CPSS_GOOD_PKTS_SENT_E.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortMacCountersOnPortGet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           portNum,
    IN  GT_BOOL                         getFromCapture,
    OUT CPSS_PORT_MAC_COUNTER_SET_STC   *portMacCounterSetArrayPtr
)
{
    GT_U64                     cntrValue[CPSS_LAST_MAC_COUNTER_NUM_E];
    CPSS_PORT_MAC_COUNTERS_ENT cntrName;  /* read the counter name  */
    GT_STATUS                  rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(getFromCapture == GT_TRUE)
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    }

    CPSS_NULL_PTR_CHECK_MAC(portMacCounterSetArrayPtr);

    /* reset counter value */
    cpssOsMemSet(cntrValue, 0, sizeof(GT_U64)*CPSS_LAST_MAC_COUNTER_NUM_E);
    cpssOsMemSet(portMacCounterSetArrayPtr, 0, sizeof(CPSS_PORT_MAC_COUNTER_SET_STC));

    /* get all mac counters */
    for(cntrName = 0; cntrName < CPSS_LAST_MAC_COUNTER_NUM_E; cntrName++)
    {
        /* the CPU port support only 8 counters */
        if(portNum == CPSS_CPU_PORT_NUM_CNS)
        {
            if ( (cntrName != CPSS_GOOD_PKTS_SENT_E) &&
                 (cntrName != CPSS_MAC_TRANSMIT_ERR_E) &&
                 (cntrName != CPSS_GOOD_OCTETS_SENT_E) &&
                 (cntrName != CPSS_DROP_EVENTS_E) &&
                 (cntrName != CPSS_GOOD_PKTS_RCV_E) &&
                 (cntrName != CPSS_BAD_PKTS_RCV_E) &&
                 (cntrName != CPSS_GOOD_OCTETS_RCV_E) &&
                 (cntrName != CPSS_BAD_OCTETS_RCV_E) )
            {
                continue;
            }
        }
        else
        {
            /* these counters do NOT exist */
            if ( (cntrName == CPSS_BAD_PKTS_RCV_E) ||
                 (cntrName == CPSS_UNRECOG_MAC_CNTR_RCV_E) ||
                 (cntrName == CPSS_BadFC_RCV_E) ||
                 (cntrName == CPSS_GOOD_PKTS_RCV_E) ||
                 (cntrName == CPSS_GOOD_PKTS_SENT_E))
            {
                continue;
            }
        }

        /* these counters do NOT exist for 10 Giga ports */
        if( (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) >= PRV_CPSS_PORT_XG_E) &&
            ( (cntrName == CPSS_EXCESSIVE_COLLISIONS_E) ||
              (cntrName == CPSS_COLLISIONS_E) ||
              (cntrName == CPSS_LATE_COLLISIONS_E) ||
              (cntrName == CPSS_MULTIPLE_PKTS_SENT_E) ||
              (cntrName == CPSS_DEFERRED_PKTS_SENT_E)))
        {
            continue;
        }

        if(portNum == CPSS_CPU_PORT_NUM_CNS)
        {
            /* the CPU port support only 8 counters --
               ignore return value on unsupported counters */
            (void)dxChMacCountersCpuPortGet(devNum,cntrName,&cntrValue[cntrName]);
        }
        else
        {
            /* get value for counter i */
            if(getFromCapture == GT_TRUE)
            {
                rc = cpssDxChPortMacCounterCaptureGet(devNum, portNum, cntrName,
                                                      &cntrValue[cntrName]);
            }
            else
            {
                rc = cpssDxChMacCounterGet(devNum, portNum, cntrName,
                                            &cntrValue[cntrName]);
            }
            if (rc != GT_OK)
                return rc;
        }
    }

    for (cntrName = 0 ; cntrName < CPSS_LAST_MAC_COUNTER_NUM_E ; cntrName++)
    {
        switch (cntrName)
        {
            case CPSS_GOOD_OCTETS_RCV_E:
                portMacCounterSetArrayPtr->goodOctetsRcv = cntrValue[cntrName];
                break;
            case CPSS_BAD_OCTETS_RCV_E:
                portMacCounterSetArrayPtr->badOctetsRcv = cntrValue[cntrName];
                break;
            case CPSS_MAC_TRANSMIT_ERR_E:
                portMacCounterSetArrayPtr->macTransmitErr = cntrValue[cntrName];
                break;
            /* This counter is for CPU port only */
            case CPSS_GOOD_PKTS_RCV_E:
                portMacCounterSetArrayPtr->goodPktsRcv = cntrValue[cntrName];
                break;
            case CPSS_BRDC_PKTS_RCV_E:
                portMacCounterSetArrayPtr->brdcPktsRcv = cntrValue[cntrName];
                break;
            case CPSS_MC_PKTS_RCV_E:
                portMacCounterSetArrayPtr->mcPktsRcv = cntrValue[cntrName];
                break;
            case CPSS_PKTS_64_OCTETS_E:
                portMacCounterSetArrayPtr->pkts64Octets = cntrValue[cntrName];
                break;
            case CPSS_PKTS_65TO127_OCTETS_E:
                portMacCounterSetArrayPtr->pkts65to127Octets = cntrValue[cntrName];
                break;
            case CPSS_PKTS_128TO255_OCTETS_E:
                portMacCounterSetArrayPtr->pkts128to255Octets = cntrValue[cntrName];
                break;
            case CPSS_PKTS_256TO511_OCTETS_E:
                portMacCounterSetArrayPtr->pkts256to511Octets = cntrValue[cntrName];
                break;
            case CPSS_PKTS_512TO1023_OCTETS_E:
                portMacCounterSetArrayPtr->pkts512to1023Octets = cntrValue[cntrName];
                break;
            case CPSS_PKTS_1024TOMAX_OCTETS_E:
                portMacCounterSetArrayPtr->pkts1024tomaxOoctets = cntrValue[cntrName];
                break;
            case CPSS_GOOD_OCTETS_SENT_E:
                portMacCounterSetArrayPtr->goodOctetsSent = cntrValue[cntrName];
                break;
            /* This counter is for CPU port only */
            case CPSS_GOOD_PKTS_SENT_E:
                portMacCounterSetArrayPtr->goodPktsSent = cntrValue[cntrName];
                break;
            case CPSS_EXCESSIVE_COLLISIONS_E:
                portMacCounterSetArrayPtr->excessiveCollisions = cntrValue[cntrName];
                break;
            case CPSS_MC_PKTS_SENT_E:
                portMacCounterSetArrayPtr->mcPktsSent = cntrValue[cntrName];
                break;
            case CPSS_BRDC_PKTS_SENT_E:
                portMacCounterSetArrayPtr->brdcPktsSent = cntrValue[cntrName];
                break;
            case CPSS_FC_SENT_E:
                portMacCounterSetArrayPtr->fcSent = cntrValue[cntrName];
                break;
            case CPSS_GOOD_FC_RCV_E:
                portMacCounterSetArrayPtr->goodFcRcv = cntrValue[cntrName];
                break;
            case CPSS_DROP_EVENTS_E:
                /* for Twist-D-XG, GT_DROP_EVENTS */
                /*offset represent BadFCReceived */
                portMacCounterSetArrayPtr->dropEvents = cntrValue[cntrName];
                break;
            case CPSS_UNDERSIZE_PKTS_E:
                portMacCounterSetArrayPtr->undersizePkts = cntrValue[cntrName];
                break;
            case CPSS_FRAGMENTS_PKTS_E:
                portMacCounterSetArrayPtr->fragmentsPkts = cntrValue[cntrName];
                break;
            case CPSS_OVERSIZE_PKTS_E:
                portMacCounterSetArrayPtr->oversizePkts = cntrValue[cntrName];
                break;
            case CPSS_JABBER_PKTS_E:
                portMacCounterSetArrayPtr->jabberPkts = cntrValue[cntrName];
                break;
            case CPSS_MAC_RCV_ERROR_E:
                portMacCounterSetArrayPtr->macRcvError = cntrValue[cntrName];
                break;
            case CPSS_BAD_CRC_E:
                portMacCounterSetArrayPtr->badCrc = cntrValue[cntrName];
                break;
            case CPSS_COLLISIONS_E:
                portMacCounterSetArrayPtr->collisions = cntrValue[cntrName];
                break;
            case CPSS_LATE_COLLISIONS_E:
                portMacCounterSetArrayPtr->lateCollisions = cntrValue[cntrName];
                break;
            case CPSS_GOOD_UC_PKTS_RCV_E:
                portMacCounterSetArrayPtr->ucPktsRcv = cntrValue[cntrName];
                break;
            case CPSS_GOOD_UC_PKTS_SENT_E:
                portMacCounterSetArrayPtr->ucPktsSent = cntrValue[cntrName];
                break;
            case CPSS_MULTIPLE_PKTS_SENT_E:
                portMacCounterSetArrayPtr->multiplePktsSent = cntrValue[cntrName];
                break;
            case CPSS_DEFERRED_PKTS_SENT_E:
                portMacCounterSetArrayPtr->deferredPktsSent = cntrValue[cntrName];
                break;
            default:
                break;

        }
    }

    return GT_OK;
}


/*******************************************************************************
* cpssDxChPortMacCountersOnPortGet
*
* DESCRIPTION:
*       Gets Ethernet MAC counter for a particular Port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*                  CPU port counters are valid only when using "Ethernet CPU
*                  port" (not using the SDMA interface).
*                  when using the SDMA interface refer to the API
*                  cpssDxChNetIfSdmaRxCountersGet(...)
*
* OUTPUTS:
*       portMacCounterSetArrayPtr - (pointer to) array of current counter values.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     Not supported counters: CPSS_BAD_PKTS_RCV_E, CPSS_UNRECOG_MAC_CNTR_RCV_E,
*     CPSS_BadFC_RCV_E, CPSS_GOOD_PKTS_RCV_E, CPSS_GOOD_PKTS_SENT_E.
*
*******************************************************************************/
GT_STATUS   cpssDxChPortMacCountersOnPortGet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           portNum,
    OUT CPSS_PORT_MAC_COUNTER_SET_STC   *portMacCounterSetArrayPtr
)
{
    return prvCpssDxChPortMacCountersOnPortGet(devNum,portNum, GT_FALSE,
                                               portMacCounterSetArrayPtr);
}

/*******************************************************************************
* cpssDxChPortMacCounterCaptureGet
*
* DESCRIPTION:
*       Gets the captured Ethernet MAC counter for a specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       cntrName - specific counter name
*
* OUTPUTS:
*       cntrValuePtr - (pointer to) current counter value.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_BAD_PARAM - on wrong port number, device or counter name
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     The 10G MAC MIB counters are 64-bit wide.
*       Overiding of previous capture by another port in a group is occurs,
*       when the capture triggered port
*       (see cpssDxChPortMacCountersCaptureTriggerSet) and previous
*       capture triggered port are in the same group.
*       (This comment dosn't connected to HyperG.Stack ports.)
*       Not supported counters: CPSS_BAD_PKTS_RCV_E, CPSS_UNRECOG_MAC_CNTR_RCV_E,
*       CPSS_BadFC_RCV_E, CPSS_GOOD_PKTS_RCV_E, CPSS_GOOD_PKTS_SENT_E.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacCounterCaptureGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    IN  CPSS_PORT_MAC_COUNTERS_ENT  cntrName,
    OUT GT_U64                      *cntrValuePtr
)
{
    if((GT_U32)cntrName >= (GT_U32)CPSS_LAST_MAC_COUNTER_NUM_E)
        return GT_BAD_PARAM;

    return prvCpssDxChMacCounterGet(devNum, portNum, cntrName,
                                    GT_TRUE, cntrValuePtr);
}

/*******************************************************************************
* cpssDxChPortMacCountersCaptureOnPortGet
*
* DESCRIPTION:
*       Gets captured Ethernet MAC counter for a particular Port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       portMacCounterSetArrayPtr - (pointer to) array of current counter values.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Overiding of previous capture by another port in a group is occurs,
*       when the capture triggered port
*       (see cpssDxChPortMacCountersCaptureTriggerSet) and previous
*       capture triggered port are in the same group.
*       (This comment dosn't connected to HyperG.Stack ports.)
*        Not supported counters: CPSS_BAD_PKTS_RCV_E, CPSS_UNRECOG_MAC_CNTR_RCV_E,
*        CPSS_BadFC_RCV_E, CPSS_GOOD_PKTS_RCV_E, CPSS_GOOD_PKTS_SENT_E.
*
*******************************************************************************/
GT_STATUS   cpssDxChPortMacCountersCaptureOnPortGet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           portNum,
    OUT CPSS_PORT_MAC_COUNTER_SET_STC   *portMacCounterSetArrayPtr
)
{
    return prvCpssDxChPortMacCountersOnPortGet(devNum,portNum, GT_TRUE,
                                               portMacCounterSetArrayPtr);
}

/*******************************************************************************
* cpssDxChPortMacCountersCaptureTriggerSet
*
* DESCRIPTION:
*     The function triggers a capture of MIB counters for specific port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number, whose counters are to be captured.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_STATE - previous capture operation isn't finished.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*        To get the counters, see cpssDxChPortMacCountersCaptureOnPortGet,
*                                 cpssDxChPortMacCounterCaptureGet.
*        For HyperG.Stack ports the feature operates per port, for other ports
*        it operates per group.
*        Ports 0-5 belong to group 0.
*        Ports 6-11 belong to group 1.
*        Ports 12-17 belong to group 2.
*        Ports 18-23 belong to group 3.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacCountersCaptureTriggerSet
(
    IN  GT_U8  devNum,
    IN  GT_U8  portNum
)
{
    GT_U32      regAddr; /* register address to set */
    GT_U32      value;   /* register field value */
    GT_U32      group;   /* Port's group (GOP) */
    GT_U32      offset;  /* bit's offset */
    GT_STATUS   rc;      /* return status */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32 useAsGePort;/* does this port uses the GE port mechanism or the XG mechanism */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    if (PRV_CPSS_DXCH_XCAT2_STACK_MAC_COUNT_NO_CLEAR_ON_READ_WA_MAC(devNum, portNum))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    useAsGePort = IS_GE_PORT_COUNTERS_MAC(devNum,portNum);

    /* flex ports uses counters like the XG ports */
    if (useAsGePort)
    {
        group = (PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum) / 6);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.macCountersCtrl[group] ;


        offset = 0;


    }
    else
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
        {
            switch(portNum){
                case 24:
                    offset = 3;
                    break;
                case 25:
                    offset = 4;
                    break;
                case 26:
                    offset = 5;
                    break;
                case 27:
                    offset = 20;
                    break;
                default:
                    return GT_FAIL;
            }

           regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.hgXsmiiCountersCtr;

        }
        /* Cheetah3 and above */
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                macRegs.perPortRegs[portNum].xgMibCountersCtrl;
            offset = 0;
        }
    }

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId,
            regAddr,offset,1,&value);

    if(rc != GT_OK)
    {
        return rc;
    }
    /* Previous capture operation isn't finished.*/
    if(value != 0)
    {
        return GT_BAD_STATE;
    }


    if (useAsGePort)
    {
        /* Set capture port. */
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,
                regAddr,1,3,(portNum % 6));

        if(rc != GT_OK)
        {
            return rc;
        }

    }

    /* Set cature trigger. */
    return prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,
            regAddr,offset,1,1);

}

/*******************************************************************************
* cpssDxChPortMacCountersCaptureTriggerGet
*
* DESCRIPTION:
*    The function gets status of a capture of MIB counters for specific port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number.
*
* OUTPUTS:
*       captureIsDonePtr     - pointer to status of Capture counter Trigger
*                              - GT_TRUE  - capture is done.
*                              - GT_FALSE - capture action is in proccess.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong device number
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        To get the counters, see cpssDxChPortMacCountersCaptureOnPortGet,
*                                 cpssDxChPortMacCounterCaptureGet.
*        For HyperG.Stack ports the feature operates per port, for other ports
*        it operates per group.
*        Ports 0-5 belong to group 0.
*        Ports 6-11 belong to group 1.
*        Ports 12-17 belong to group 2.
*        Ports 18-23 belong to group 3.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacCountersCaptureTriggerGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT GT_BOOL  *captureIsDonePtr
)
{
    GT_U32      regAddr; /* register address to set */
    GT_U32      group;   /* Port's group (GOP) */
    GT_U32      offset;  /* bit's offset */
    GT_U32      value;   /* register field value */
    GT_STATUS   rc;      /* return status */
    GT_U32 useAsGePort;/* does this port uses the GE port mechanism or the XG mechanism */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(captureIsDonePtr);

    useAsGePort = IS_GE_PORT_COUNTERS_MAC(devNum,portNum);

    if(useAsGePort)
    {
        group = (PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum) / 6);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.macCountersCtrl[group] ;


        offset = 0;


    }
    else
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
        {
            switch(portNum){
                case 24:
                    offset = 3;
                    break;
                case 25:
                    offset = 4;
                    break;
                case 26:
                    offset = 5;
                    break;
                case 27:
                    offset = 20;
                    break;
                default:
                    return GT_FAIL;
            }

           regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.hgXsmiiCountersCtr;

        }
        /* Cheetah3 and above */
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                macRegs.perPortRegs[portNum].xgMibCountersCtrl;
            offset = 0;
        }

    }

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr,offset,1,&value);
    if(rc != GT_OK)
    {
        return rc;
    }

    *captureIsDonePtr = (value == 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortEgressCntrModeSet
*
* DESCRIPTION:
*       Configure a set of egress counters that work in specified bridge egress
*       counters mode on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       cntrSetNum - counter set number.
*       setModeBmp - counter mode bitmap. For example:
*                   GT_EGRESS_CNT_PORT | GT_EGRESS_CNT_VLAN | GT_EGRESS_CNT_DP.
*       portNum   - physical port number, if corresponding bit in setModeBmp is 1.
*       vlanId    - VLAN Id, if corresponding bit in setModeBmp is 1.
*       tc        - traffic class queue (0..7),
*                   if corresponding bit in setModeBmp is 1.
*       dpLevel   - drop precedence level, if corresponding bit in setModeBmp is 1.
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
*       Supported DP levels:  CPSS_DP_GREEN_E, CPSS_DP_RED_E.
*
*******************************************************************************/
GT_STATUS cpssDxChPortEgressCntrModeSet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           cntrSetNum,
    IN  CPSS_PORT_EGRESS_CNT_MODE_ENT   setModeBmp,
    IN  GT_U8                           portNum,
    IN  GT_U16                          vlanId,
    IN  GT_U8                           tc,
    IN  CPSS_DP_LEVEL_ENT               dpLevel
)
{
    GT_STATUS   rc;
    GT_U32 regAddr;         /* register address */
    GT_U32 dxDpLevel;/* DP level value */
    GT_U32 value;/* register value*/
    GT_U32  portGroupId; /*the port group Id - support multi-port-groups device */
    GT_U32  portPortGroupId; /*the port group Id for 'port mode' */
    GT_U8   localPort;/* local port - support multi-port-groups device */
    GT_U32 portValue;/* tmp register value in 'port mode' */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* only 2 sets supported for now */
    if(cntrSetNum > 1)
       return GT_BAD_PARAM;

    if(setModeBmp &~ (
                CPSS_EGRESS_CNT_PORT_E |
                CPSS_EGRESS_CNT_VLAN_E |
                CPSS_EGRESS_CNT_TC_E   |
                CPSS_EGRESS_CNT_DP_E
                ))
    {
        return GT_BAD_PARAM;
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    egrTxQConf.txQCountSet[cntrSetNum].txQConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.peripheralAccess.egressMibCounterSet.config[cntrSetNum];
    }

    value = setModeBmp & 0xF;

    if (setModeBmp & CPSS_EGRESS_CNT_VLAN_E)
    {
        PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlanId);

        /* write the port monitored by set */
        value |= (vlanId & 0xFFF) << 10;
    }
    if (setModeBmp & CPSS_EGRESS_CNT_TC_E)
    {
        PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tc);

        /* write the port monitored by set */
        value |= (tc & 0x7) << 22;
    }
    if (setModeBmp & CPSS_EGRESS_CNT_DP_E)
    {
        PRV_CPSS_DXCH_COS_DP_TO_HW_CHECK_AND_CONVERT_MAC(
            devNum, dpLevel, dxDpLevel);

        /* write the port monitored by set */
        value |= dxDpLevel << 25;
    }

    if (setModeBmp & CPSS_EGRESS_CNT_PORT_E)
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portPortGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
        {
            /* loop on all port groups :
                on the port group that 'own' the port , set the needed configuration
                on other port groups put 'NULL port'
            */
            PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
            {
                if(portPortGroupId == portGroupId)
                {
                    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);
                }
                else
                {
                    localPort = PRV_CPSS_DXCH_NULL_PORT_NUM_CNS;
                }

                /* write the port monitored by set */
                portValue =  (localPort & 0x3F) << 4;

                rc = prvCpssDxChHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr,
                        (value | portValue));
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
            PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

            rc = GT_OK;
        }
        else
        {
            if(portNum == CPSS_CPU_PORT_NUM_CNS)
            {
                /* CPU port is configured according to SDMA port group
                   Port 15 ?port group 0 SDMA  Port 31 ?port group 1 SDMA
                   Port 47 ?port group 2 SDMA  Port 63 ?port group 3 SDMA */
                portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);
                portValue = (((portGroupId) & 0x3)<< 4) | 0xF;
            }
            else
            {
                portValue =  portNum & 0x3F; /* use global port number */
            }

            /* write the port monitored by set */
            portValue = portValue << 4;

            rc = prvCpssDxChHwPpPortGroupWriteRegister(devNum, portPortGroupId,regAddr,
                    (value | portValue));
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }
    else
    {
        portPortGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        rc = prvCpssDxChHwPpWriteRegister(devNum, regAddr, value);
    }

    if(rc == GT_OK)
    {
        /* save the info for 'get' configuration */
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.
                portEgressCntrModeInfo[cntrSetNum].portGroupId = portPortGroupId;
    }

    return rc;

}

/*******************************************************************************
* cpssDxChPortEgressCntrModeGet
*
* DESCRIPTION:
*       Get configuration of a bridge egress counters set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       cntrSetNum - counter set number.
*
* OUTPUTS:
*       setModeBmpPtr - Pointer to counter mode bitmap. For example:
*                       GT_EGRESS_CNT_PORT | GT_EGRESS_CNT_VLAN | GT_EGRESS_CNT_DP.
*       portNumPtr    - Pointer to the egress port of the packets counted by
*                       the set of counters
*       vlanIdPtr     - Pointer to the vlan id of the packets counted by the
*                       set of counters.
*       tcPtr         - Pointer to the tc of the packets counted by the set
*                       of counters.
*       dpLevelPtr    - Pointer to the dp of the packets counted by the set
*                       of counters.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported DP levels:  CPSS_DP_GREEN_E, CPSS_DP_RED_E.
*
*******************************************************************************/
GT_STATUS cpssDxChPortEgressCntrModeGet
(
    IN   GT_U8                           devNum,
    IN   GT_U8                           cntrSetNum,
    OUT  CPSS_PORT_EGRESS_CNT_MODE_ENT   *setModeBmpPtr,
    OUT  GT_U8                           *portNumPtr,
    OUT  GT_U16                          *vlanIdPtr,
    OUT  GT_U8                           *tcPtr,
    OUT  CPSS_DP_LEVEL_ENT               *dpLevelPtr
)
{
    GT_STATUS rc;              /* return code                   */
    GT_U32    regAddr;         /* register address              */
    GT_U32    dxDpLevel;       /* DP HW value                   */
    GT_U32    dxDpLevelMask;   /* DP HW value mask              */
    GT_U32    value;           /* HW value of register contents */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(setModeBmpPtr);
    CPSS_NULL_PTR_CHECK_MAC(portNumPtr);
    CPSS_NULL_PTR_CHECK_MAC(vlanIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcPtr);
    CPSS_NULL_PTR_CHECK_MAC(dpLevelPtr);

    /* only 2 sets supported for now */
    if(cntrSetNum > 1)
       return GT_BAD_PARAM;

    /* get the info from DB */
    portGroupId = PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.
                portEgressCntrModeInfo[cntrSetNum].portGroupId;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    egrTxQConf.txQCountSet[cntrSetNum].txQConfig;
        dxDpLevelMask = 1;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.peripheralAccess.egressMibCounterSet.config[cntrSetNum];
        dxDpLevelMask = 3;
    }

    rc = prvCpssDxChHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &value);

    *setModeBmpPtr = value & 0xF;

    *portNumPtr = (GT_U8)((value >> 4) & 0x3F);
    *vlanIdPtr = (GT_U16)((value >> 10) & 0xFFF);
    *tcPtr = (GT_U8)((value >> 22) & 0x7);


    dxDpLevel = (value >> 25) & dxDpLevelMask;

    PRV_CPSS_DXCH_COS_DP_TO_SW_CHECK_AND_CONVERT_MAC(
        devNum, dxDpLevel, (*dpLevelPtr));

    if(portGroupId != CPSS_PORT_GROUP_UNAWARE_MODE_CNS &&
       (0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum)))
    {
        /* support multi-port-groups device , convert local port to global port */
        *portNumPtr = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum,portGroupId,(*portNumPtr));
    }

    return rc;

}

/*******************************************************************************
* cpssDxChPortEgressCntrsGet
*
* DESCRIPTION:
*       Gets a egress counters from specific counter-set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - physical device number
*       cntrSetNum - counter set number : 0, 1
*
* OUTPUTS:
*       egrCntrPtr - (pointer to) structure of egress counters current values.
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
GT_STATUS cpssDxChPortEgressCntrsGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       cntrSetNum,
    OUT CPSS_PORT_EGRESS_CNTR_STC   *egrCntrPtr
)
{
    GT_U32 regAddr;         /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(egrCntrPtr);

    if (cntrSetNum > 1)
    {
        return GT_BAD_PARAM;
    }

    PRV_CPSS_DXCH_LION_BOOKMARK /* need to support PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.peripheralAccess.egressMibCounterSet.multicastFifoDroppedPacketsCounter[cntrSetNum]

                    like in EXMXPM : CPSS_EXMXPM_BRG_EGRESS_CNTR_STC
                    */

    /* read Outgoing Unicast Packet Count Register */
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    egrTxQConf.txQCountSet[cntrSetNum].outUcPkts;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.peripheralAccess.egressMibCounterSet.outgoingUnicastPacketCounter[cntrSetNum];
    }

    if (prvCpssDxChPortGroupsCounterSummary(devNum, regAddr,0,32, &egrCntrPtr->outUcFrames,NULL) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* read Outgoing Multicast Packet Count Register */
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        egrTxQConf.txQCountSet[cntrSetNum].outMcPkts;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.peripheralAccess.egressMibCounterSet.outgoingMulticastPacketCounter[cntrSetNum];
    }


    if(prvCpssDxChPortGroupsCounterSummary(devNum, regAddr,0,32, &egrCntrPtr->outMcFrames,NULL)
       != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* read Outgoing Byte Count Packet Count Register */
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        egrTxQConf.txQCountSet[cntrSetNum].outBytePkts;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.peripheralAccess.egressMibCounterSet.outgoingBroadcastPacketCounter[cntrSetNum];
    }

    if (prvCpssDxChPortGroupsCounterSummary(devNum, regAddr,0,32, &egrCntrPtr->outBcFrames,NULL) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* read Bridge Egress Filtered Packet Count Register */
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        egrTxQConf.txQCountSet[cntrSetNum].brgEgrFiltered;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.peripheralAccess.egressMibCounterSet.bridgeEgressFilteredPacketCounter[cntrSetNum];
    }

    if (prvCpssDxChPortGroupsCounterSummary(devNum, regAddr,0,32, &egrCntrPtr->brgEgrFilterDisc,NULL) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* read Transmit Queue Filtered Packet Count Register */
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        egrTxQConf.txQCountSet[cntrSetNum].txQFiltered;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.peripheralAccess.egressMibCounterSet.tailDroppedPacketCounter[cntrSetNum];
    }

    if (prvCpssDxChPortGroupsCounterSummary(devNum, regAddr,0,32, &egrCntrPtr->txqFilterDisc,NULL) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* read  to cpu, from cpu and to analyzer Packets Count Register*/
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        egrTxQConf.txQCountSet[cntrSetNum].outCtrlPkts;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    txqVer1.queue.peripheralAccess.egressMibCounterSet.controlPacketCounter[cntrSetNum];
    }

    if (prvCpssDxChPortGroupsCounterSummary(devNum, regAddr,0,32, &egrCntrPtr->outCtrlFrames,NULL) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* read  egress forwarding restriction dropped Packets Count Register
           Supported from ch2 devices.*/
        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            egrTxQConf.txQCountSet[cntrSetNum].egrFrwDropPkts;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        txqVer1.queue.peripheralAccess.egressMibCounterSet.
                            egressForwardingRestrictionDroppedPacketsCounter[cntrSetNum];
        }

        if (prvCpssDxChPortGroupsCounterSummary(devNum, regAddr,0,32, &egrCntrPtr->egrFrwDropFrames,NULL) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }
    else
    {
        egrCntrPtr->egrFrwDropFrames = 0;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortMacCountersEnable
*
* DESCRIPTION:
*       Enable or disable MAC Counters update for this port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number.
*       portNum  - physical port number (or CPU port)
*       enable   -  enable update of MAC counters
*                   GT_FALSE = MAC counters update for this port is disabled.
*                   GT_TRUE = MAC counters update for this port is enabled.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None;
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacCountersEnable(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN GT_BOOL enable
)
{
    GT_U32  regAddr;/* register address to set */
    GT_U32  value;/* value to set to bits */
    GT_U32  offset;/* field offset */
    PRV_CPSS_PORT_TYPE_ENT portMacType;
    GT_U32     portGroupId;/*the port group Id - support multi-port-groups device */
    GT_STATUS rc;/* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
   /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

#ifdef __AX_PLATFORM__
			for(portMacType = PRV_CPSS_PORT_GE_E; portMacType < PRV_CPSS_PORT_NOT_APPLICABLE_E; portMacType++)
			{
#else
			portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);
#endif
				if((portNum == CPSS_CPU_PORT_NUM_CNS) && (portMacType >= PRV_CPSS_PORT_XG_E))
#ifdef __AX_PLATFORM__
					continue;
#else
					return GT_OK;
#endif


        if(portMacType >= PRV_CPSS_PORT_XG_E)
        {
            value = ((enable == GT_FALSE) ? 1 : 0);
            offset = ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) ? 14 : 15) ;
        }
        else
        {
            value = ((enable == GT_TRUE) ? 1 : 0);
            offset = 15;
        }

        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);

        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, offset, 1, value);

            if (rc!=GT_OK)
            {
                return rc;
            }
        }
#ifdef __AX_PLATFORM__
    }
#endif
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortMacCountersClearOnReadSet
*
* DESCRIPTION:
*       Enable or disable MAC Counters Clear on read per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*       - Ports 0 through 5
*       - Ports 6 through 11
*       - Ports 12 through 17
*       - Ports 18 through 23
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - physical port number (or CPU port)
*       enable      - enable clear on read for MAC counters
*                   GT_FALSE - Counters are not cleared.
*                   GT_TRUE - Counters are cleared.

*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChPortMacCountersClearOnReadSet(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  enable
)
{
    GT_U32      regAddr;/* register address to set */
    GT_U32      value;  /* value to set to bits */
    GT_U32      group;  /* Port's group (GOP) */
    GT_U32      offset; /* bit's offset */
    GT_STATUS   rc;     /* return code */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_DXCH_ERRATA_STACK_MAC_COUNT_SHADOW_STC *macCountShadowPtr; /* Pointer To MAC Counters Shadow */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
   /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    if (PRV_CPSS_DXCH_XCAT2_STACK_MAC_COUNT_NO_CLEAR_ON_READ_WA_MAC(devNum, portNum))
    {
        macCountShadowPtr =
        PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
            info_PRV_CPSS_DXCH_XCAT2_STACK_MAC_COUNT_NO_CLEAR_ON_READ_WA_E.
            stackMacCntPtr;
        macCountShadowPtr->stackPortArr[portNum - 24].clearOnRead = enable;
        return GT_OK;
    }

    if((portNum) == CPSS_CPU_PORT_NUM_CNS)
    {
        value = ((enable == GT_TRUE) ? 1 : 0);
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.cpuPortCtrlReg;
        offset = 3;
        return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                  regAddr, offset, 1, value);
    }
    else
    {
        value = ((enable == GT_TRUE) ? 0 : 1);
        offset = 4;
        group = (PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum) / 6);
        if(group < 4)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.macCountersCtrl[group];
            if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
            {
                rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                        regAddr, offset, 1, value);
                if (rc!=GT_OK)
                {
                    return rc;
                }
            }
        }

        if(PRV_CPSS_PP_MAC(devNum)->devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
        {
            if(portNum < 24)
                return GT_OK;

            switch(portNum){
            case 24:
                offset = 0;
                break;
            case 25:
                offset = 1;
                break;
            case 26:
                offset = 2;
                break;
            case 27:
                offset = 19;
                break;
            default:
                return GT_FAIL;
            }

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.hgXsmiiCountersCtr;
        }
        /* Cheetah3 and above */
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                macRegs.perPortRegs[portNum].xgMibCountersCtrl;
            offset = 1;
        }

        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                      regAddr, offset, 1, value);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortMacCountersClearOnReadGet
*
* DESCRIPTION:
*       Get "Clear on read" status of MAC Counters per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*       - Ports 0 through 5
*       - Ports 6 through 11
*       - Ports 12 through 17
*       - Ports 18 through 23
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr   - (pointer to) "Clear on read" status for MAC counters
*                     GT_FALSE - Counters are not cleared.
*                     GT_TRUE - Counters are cleared.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on error.
*       GT_HW_ERROR                 - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_BAD_PTR                  - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChPortMacCountersClearOnReadGet(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS   rc;      /* return code */
    GT_U32      regAddr; /* register address to get */
    GT_U32      regData; /* register data */
    GT_U32      group;   /* Port's group (GOP) */
    GT_U32      offset;  /* bit's offset */
    GT_U32 useAsGePort;/* does this port uses the GE port mechanism or the XG mechanism */
    PRV_CPSS_DXCH_ERRATA_STACK_MAC_COUNT_SHADOW_STC *macCountShadowPtr; /* Pointer To MAC Counters Shadow */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    useAsGePort = IS_GE_PORT_COUNTERS_MAC(devNum,portNum);

    if (PRV_CPSS_DXCH_XCAT2_STACK_MAC_COUNT_NO_CLEAR_ON_READ_WA_MAC(devNum, portNum))
    {
        macCountShadowPtr =
        PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
            info_PRV_CPSS_DXCH_XCAT2_STACK_MAC_COUNT_NO_CLEAR_ON_READ_WA_E.
            stackMacCntPtr;
        *enablePtr = macCountShadowPtr->stackPortArr[portNum - 24].clearOnRead;
        return GT_OK;
    }

    if((portNum) == CPSS_CPU_PORT_NUM_CNS)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.cpuPortCtrlReg;
        offset = 3;
    }
    else
    {
        if(useAsGePort)
        {
            group = (PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum) / 6);

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.macCountersCtrl[group] ;
            offset = 4;
        }
        else
        {
            if(PRV_CPSS_PP_MAC(devNum)->devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
            {
                switch(portNum){
                    case 24:
                        offset = 0;
                        break;
                    case 25:
                        offset = 1;
                        break;
                    case 26:
                        offset = 2;
                        break;
                    case 27:
                        offset = 19;
                        break;
                    default:
                        return GT_FAIL;
                }

                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.hgXsmiiCountersCtr;
            }

            /* Cheetah3 and above */
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[portNum].xgMibCountersCtrl;
                offset = 1;
            }

        }

    }

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,
            PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, offset, 1, &regData);
    if( GT_OK != rc )
    {
        return rc;
    }

    if((portNum) == CPSS_CPU_PORT_NUM_CNS)
    {
        *enablePtr = BIT2BOOL_MAC(regData);
    }
    else
    {
        *enablePtr = (regData == 0 ) ? GT_TRUE : GT_FALSE ;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortMacCountersRxHistogramEnable
*
* DESCRIPTION:
*       Enable/disable updating of the RMON Etherstat histogram
*       counters for received packets per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*           - Ports 0 through 5
*           - Ports 6 through 11
*           - Ports 12 through 17
*           - Ports 18 through 23
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - physical port number
*       enable      - enable updating of the counters for received packets
*                   GT_FALSE - Counters are updated.
*                   GT_TRUE - Counters are not updated.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChPortMacCountersRxHistogramEnable(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  enable
)
{
    GT_U32      regAddr;/* register address to set */
    GT_U32      value;/* value to set to bits */
    GT_U32      group;  /* Port's group (GOP) */
    GT_U32      offset; /* bit's offset */
    GT_STATUS rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    value = ((enable == GT_TRUE) ? 1 : 0);

    if((PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum,portNum) == GT_FALSE) &&  (portNum < 24) )
     {
         group = (PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum) / 6);

         regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.macCountersCtrl[group] ;
         offset = 5;

        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                    regAddr, offset, 1, value);

            if (rc!=GT_OK)
            {
                    return rc;
            }
        }
     }

    if(PRV_CPSS_PP_MAC(devNum)->devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
    {
        if (portNum < 24)
            return GT_OK;

        switch(portNum){
        case 24:
            offset = 6;
            break;
        case 25:
            offset = 7;
            break;
        case 26:
            offset = 8;
            break;
        case 27:
            offset = 21;
            break;
        default:
            return GT_FAIL;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.hgXsmiiCountersCtr ;
     }
    /* Cheetah3 and above */
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            macRegs.perPortRegs[portNum].xgMibCountersCtrl;
        offset = 2;
    }

    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        return prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,
                                                      regAddr, offset, 1, value);
    }

    return GT_OK;

}

/*******************************************************************************
* cpssDxChPortMacCountersTxHistogramEnable
*
* DESCRIPTION:
*       Enable/disable updating of the RMON Etherstat histogram
*       counters for transmitted packets per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*           - Ports 0 through 5
*           - Ports 6 through 11
*           - Ports 12 through 17
*           - Ports 18 through 23
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - physical port number
*       enable      - enable updating of the counters for transmitted packets
*                   GT_FALSE - Counters are updated.
*                   GT_TRUE - Counters are not updated.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChPortMacCountersTxHistogramEnable(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  enable
)
{
    GT_U32      regAddr;/* register address to set */
    GT_U32      value;/* value to set to bits */
    GT_U32      group;  /* Port's group (GOP) */
    GT_U32      offset; /* bit's offset */
    GT_STATUS rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    value = ((enable == GT_TRUE) ? 1 : 0);

    if((PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum,portNum) == GT_FALSE) &&  (portNum < 24) )
     {
         group = (PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum) / 6);
         regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.macCountersCtrl[group] ;
         offset = 6;

       if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                    regAddr, offset, 1, value);

            if (rc!=GT_OK)
            {
                    return rc;
            }
        }
     }

    if(PRV_CPSS_PP_MAC(devNum)->devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
    {
        if(portNum < 24)
            return GT_OK;

        switch(portNum){
        case 24:
            offset = 9;
            break;
        case 25:
            offset = 10;
            break;
        case 26:
            offset = 11;
            break;
        case 27:
            offset = 22;
            break;
        default:
            return GT_FAIL;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.hgXsmiiCountersCtr ;
    }
    /* Cheetah3 and above */
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            macRegs.perPortRegs[portNum].xgMibCountersCtrl;
        offset = 3;
    }

    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        return prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,
                                                  regAddr, offset, 1, value);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortMacOversizedPacketsCounterModeSet
*
* DESCRIPTION:
*       Sets MIB counters behaviour for oversized packets on given port.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number
*       counterMode - oversized packets counter mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number, device or counter mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The oversized packets counter mode determines the behavour of the
*       CPSS_PKTS_1024TOMAX_OCTETS_E and CPSS_DEFERRED_PKTS_SENT_E counters.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacOversizedPacketsCounterModeSet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    IN  CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_ENT  counterMode
)
{
    GT_U32 regAddr;    /* register address to set */
    GT_U32 value;      /* value to set to bits */
    GT_U32 group;      /* Port's group (GOP) */
    GT_U32 mask;       /* bit's offset */
    GT_U32 portOffset; /* port's offset in register */
    GT_U32 useAsGePort;/* does this port uses the GE port mechanism or the XG mechanism */
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    if (PRV_CPSS_DXCH_XCAT2_STACK_MAC_COUNT_NO_CLEAR_ON_READ_WA_MAC(devNum, portNum))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    useAsGePort = IS_GE_PORT_COUNTERS_MAC(devNum,portNum);

    switch(counterMode)
    {
        case CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_NORMAL_E:
            value = 0x0; break;
        case CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_HISTOGRAM_1518_E:
            value = 0x1; break;
        case CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_HISTOGRAM_1522_E:
            value = useAsGePort ? 0x41 : 0x3; break;
        default:
            return GT_BAD_PARAM;
    }

     if(useAsGePort)
     {
         group = (PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum) / 6);
         portOffset = (PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum) % 6);
         regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.macCountersCtrl[group] ;

         mask = 0x410000 << portOffset;
         value <<=  16 + portOffset;

         rc = prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum,
           PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
           regAddr,mask,value);
     }
     else
     {
         regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
             macRegs.perPortRegs[portNum].xgMibCountersCtrl;
         rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,
           PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
           regAddr,6,2,value);
     }

     return rc;
}

/*******************************************************************************
* cpssDxChPortMacOversizedPacketsCounterModeGet
*
* DESCRIPTION:
*       Gets MIB counters behaviour for oversized packets on given port.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*       devNum         - device number
*       portNum        - physical port number
*
* OUTPUTS:
*       counterModePtr - (pointer to) oversized packets counter mode
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
*       The oversized packets counter mode determines the behavour of the
*       CPSS_PKTS_1024TOMAX_OCTETS_E and CPSS_DEFERRED_PKTS_SENT_E counters.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacOversizedPacketsCounterModeGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    OUT CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_ENT  *counterModePtr
)
{
    GT_U32 regAddr;    /* register address to set */
    GT_U32 value;      /* value to set to bits */
    GT_U32 group;      /* Port's group (GOP) */
    GT_U32 mask;       /* bit's offset */
    GT_U32 portOffset; /* port's offset in register */
    GT_U32 useAsGePort;/* does this port uses the GE port mechanism or the XG mechanism */
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(counterModePtr);

    useAsGePort = IS_GE_PORT_COUNTERS_MAC(devNum,portNum);

    if(useAsGePort)
    {
         group = (PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum) / 6);
         portOffset = (PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum) % 6);
         regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.macCountersCtrl[group] ;

         mask = 0x410000 << portOffset;

         rc = prvCpssDrvHwPpPortGroupReadRegBitMask(devNum,
           PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
           regAddr,mask,&value);
         if (rc != GT_OK)
         {
             return rc;
         }
         value >>= 16 + portOffset;
         switch (value & 0x41)
         {
             case 0x0:
                 *counterModePtr = CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_NORMAL_E;
                 break;
             case 0x1:
                 *counterModePtr = CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_HISTOGRAM_1518_E;
                 break;
             case 0x40:
                 *counterModePtr = CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_NORMAL_E;
                 break;
             case 0x41:
                 *counterModePtr = CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_HISTOGRAM_1522_E;
                 break;
         }
    }
    else
    {
         regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
             macRegs.perPortRegs[portNum].xgMibCountersCtrl;
         rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,
           PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
           regAddr,6,2,&value);
         if (rc != GT_OK)
         {
             return rc;
         }
         switch (value)
         {
             case 0:
                 *counterModePtr = CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_NORMAL_E;
                 break;
             case 1:
                 *counterModePtr = CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_HISTOGRAM_1518_E;
                 break;
             case 2:
                 *counterModePtr = CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_NORMAL_E;
                 break;
             case 3:
                 *counterModePtr = CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_HISTOGRAM_1522_E;
                 break;
         }
    }

    return GT_OK;
}

