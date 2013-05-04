/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChCnc.c
*
* DESCRIPTION:
*       CPSS DxCh Centralized Counters (CNC) API.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/


#include <cpss/dxCh/dxChxGen/cnc/cpssDxChCnc.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpss/dxCh/dxChxGen/cnc/private/prvCpssDxChCnc.h>
#include <cpssCommon/private/prvCpssMath.h>

/* address of register increments: */
/* wraparound indexes per client */
#define CNC_WRAPAROUND_STATUS_ADDR_INCREMENT_CNS  0x00000100
/* 2k-counter block */
#define CNC_COUNTER_BLOCK_ADDR_INCREMENT_CNS      0x00004000

/* check that CNC block upload operation is supported,      */
/* not supported if PCI is not available or that workaround */
/* for FDB AU messages problem is used.                     */
#define PRV_CNC_BLOCK_UPLOAD_SUPPORT_CHECK_MAC(_dev)                    \
    if((PRV_CPSS_HW_IF_PCI_COMPATIBLE_MAC(_dev) == 0) ||                \
       (PRV_CPSS_DXCH_PP_MAC(_dev)->errata.                             \
                   info_PRV_CPSS_DXCH_XCAT_FDB_AU_FIFO_CORRUPT_WA_E.    \
                       enabled == GT_TRUE))                             \
    {                                                                   \
        return GT_NOT_SUPPORTED;                                        \
    }

/* The macro checks the _format parameter validity for Lion and        */
/* above devices. For other devices it assigns the _format to the      */
/* alone value supported for these devices.                            */
/* Such behavior of the macro makes the _format value passed by the    */
/* the caller not relevant for not Lion devices.                       */
#define PRV_CNC_COUNTER_FORMAT_CHECK_MAC(_devNum, _format)                  \
    if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(_devNum))                   \
    {                                                                   \
        if ((GT_U32)_format                                             \
            > (GT_U32)CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E)            \
        {                                                               \
            return GT_BAD_PARAM;                                        \
        }                                                               \
    }                                                                   \
    else                                                                \
    {                                                                   \
        _format = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E;                \
    }




/*******************************************************************************
* prvCpssDxChCncCounterSwToHw
*
* DESCRIPTION:
*   The function converts the SW counter to 64-bit HW format.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       format         - CNC counter HW format,
*                        relevant only for Lion and above
*       swCounterPtr   - (pointer to) CNC Counter in SW format
*
* OUTPUTS:
*       hwCounterArr   - (pointer to) CNC Counter in HW format
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChCncCounterSwToHw
(
    IN   CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    IN   CPSS_DXCH_CNC_COUNTER_STC         *swCounterPtr,
    OUT  GT_U32                            hwCounterArr[]
)
{
    switch (format)
    {
        case CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E:
            /* Packets counter: 29 bits, Byte Count counter: 35 bits */
            hwCounterArr[0] =
                (swCounterPtr->packetCount.l[0] & 0x1FFFFFFF)
                | (swCounterPtr->byteCount.l[1] << 29);
            hwCounterArr[1] = swCounterPtr->byteCount.l[0];
            break;

        case CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_1_E:
            /* Packets counter: 27 bits, Byte Count counter: 37 bits */
            hwCounterArr[0] =
                (swCounterPtr->packetCount.l[0] & 0x07FFFFFF)
                | (swCounterPtr->byteCount.l[1] << 27);
            hwCounterArr[1] = swCounterPtr->byteCount.l[0];
            break;

        case CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E:
            /* Packets counter: 37 bits, Byte Count counter: 27 bits */
            hwCounterArr[0] = swCounterPtr->packetCount.l[0];
            hwCounterArr[1] =
                (swCounterPtr->byteCount.l[0] & 0x07FFFFFF)
                | (swCounterPtr->packetCount.l[1] << 27);
            break;

        default: return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChCncCounterHwToSw
*
* DESCRIPTION:
*   The function converts the 64-bit HW counter to SW format.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       format        - CNC counter HW format,
*                       relevant only for Lion and above
*       hwCounterArr   - (pointer to) CNC Counter in HW format
*
* OUTPUTS:
*       swCounterPtr   - (pointer to) CNC Counter in SW format
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChCncCounterHwToSw
(
    IN   CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    IN   GT_U32                            hwCounterArr[],
    OUT  CPSS_DXCH_CNC_COUNTER_STC         *swCounterPtr
)
{
    switch (format)
    {
        case CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E:
            /* Packets counter: 29 bits, Byte Count counter: 35 bits */
            swCounterPtr->packetCount.l[0] =
                (hwCounterArr[0] & 0x1FFFFFFF);
            swCounterPtr->packetCount.l[1] = 0;
            swCounterPtr->byteCount.l[0] = hwCounterArr[1];
            swCounterPtr->byteCount.l[1] =
                ((hwCounterArr[0] >> 29) & 0x07);
            break;

        case CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_1_E:
            /* Packets counter: 27 bits, Byte Count counter: 37 bits */
            swCounterPtr->packetCount.l[0] =
                (hwCounterArr[0] & 0x07FFFFFF);
            swCounterPtr->packetCount.l[1] = 0;
            swCounterPtr->byteCount.l[0] = hwCounterArr[1];
            swCounterPtr->byteCount.l[1] =
                ((hwCounterArr[0] >> 27) & 0x1F);
            break;

        case CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E:
            /* Packets counter: 37 bits, Byte Count counter: 27 bits */
            swCounterPtr->packetCount.l[0] = hwCounterArr[0];
            swCounterPtr->packetCount.l[1] =
                ((hwCounterArr[1] >> 27) & 0x1F);
            swCounterPtr->byteCount.l[0] =
                (hwCounterArr[1] & 0x07FFFFFF);
            swCounterPtr->byteCount.l[1] = 0;
            break;
        default: return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChCncPortGroupCounterValueAverageSet
*
* DESCRIPTION:
*   The function sets the average counter value.
*   The get function must sum the values per port groups
*   and the calculated result must be as given.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       regAddr          - address of 64-bit memory to set the average value
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*       counterPtr       - (pointer to) counter contents
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChCncPortGroupCounterValueAverageSet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            regAddr,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    IN  CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
)
{
    GT_STATUS rc;               /* return code                */
    GT_U32    data[2];          /* registers value            */
    GT_U16    portGroupNum;     /* number of port groups      */
    GT_U16    portGroupIndex;   /* index of port group in BMP */
    GT_U32    portGroupId;      /* the port group Id - support multi port groups device */
    CPSS_DXCH_CNC_COUNTER_STC   counter; /* work counter      */
    GT_U16    pktCntrMod;       /* packetCouner % portGroupNum*/
    GT_U16    byteCntrMod;      /* byteCouner % portGroupNum  */
    GT_U64    w64;              /* work variable              */


    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    PRV_CNC_COUNTER_FORMAT_CHECK_MAC(devNum, format);
    CPSS_NULL_PTR_CHECK_MAC(counterPtr);

    if (! PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        rc = prvCpssDxChCncCounterSwToHw(
            format, counterPtr, data);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDrvHwPpWriteRam(
            devNum, regAddr, 2/* words*/, data);
        return rc;
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

    /* calculate average counter value */
    pktCntrMod = prvCpssMathMod64By16(
        counterPtr->packetCount, portGroupNum);
    byteCntrMod = prvCpssMathMod64By16(
        counterPtr->byteCount, portGroupNum);
    counter.packetCount = prvCpssMathDiv64By16(
        counterPtr->packetCount, portGroupNum);
    counter.byteCount = prvCpssMathDiv64By16(
        counterPtr->byteCount, portGroupNum);

    /* add one to average */
    w64.l[1] = 0;
    w64.l[0] = 1;
    counter.packetCount = prvCpssMathAdd64(
        counter.packetCount, w64);
    counter.byteCount = prvCpssMathAdd64(
        counter.byteCount, w64);

    /* to avoid compiler warning */
    data[0] = 0;
    data[1] = 0;


    /* loop on all port groups to get the counter value from */
    portGroupIndex = 0;
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        if (portGroupIndex == pktCntrMod)
        {
            /* subtract one from average back */
            counter.packetCount = prvCpssMathSub64(
                counter.packetCount, w64);
        }

        if (portGroupIndex == byteCntrMod)
        {
            /* subtract one from average back */
            counter.byteCount = prvCpssMathSub64(
                counter.byteCount, w64);
        }

        if ((portGroupIndex == 0)
            || (portGroupIndex == pktCntrMod)
            || (portGroupIndex == byteCntrMod))
        {
            /* convert counter from SW to HW format       */
            /* this conversion done at the first loop and */
            /* any time when "counter" updated            */
            rc = prvCpssDxChCncCounterSwToHw(
                format, &counter, data);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        rc = prvCpssDrvHwPpPortGroupWriteRam(
            devNum, portGroupId, regAddr, 2/* words*/, data);
        if (rc != GT_OK)
        {
            return rc;
        }
        portGroupIndex ++;
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChCncPortGroupCounterValueSummingGet
*
* DESCRIPTION:
*   The function gets the counter contents summing per port groups
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       regAddr          - address of 64-bit memory to get the sum value
*                          DxCh3, xCat - 0..2047,
*                          Lion - 0..511
*                          xCat2 - 0..255
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*
* OUTPUTS:
*       counterPtr        - (pointer to) counter contents
*                           The result counter contains the sum of counters
*                           read from all specified port groups.
*                           It contains more bits than in HW.
*                           For example the sum of 4 35-bit values may be
*                           37-bit value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChCncPortGroupCounterValueSummingGet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            regAddr,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    OUT CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
)
{
    GT_STATUS rc;               /* return code       */
    GT_U32    data[2];          /* field value       */
    GT_U32    portGroupId;      /* port group Id     */
    OUT CPSS_DXCH_CNC_COUNTER_STC  counter; /* work counter */

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    PRV_CNC_COUNTER_FORMAT_CHECK_MAC(devNum, format);
    CPSS_NULL_PTR_CHECK_MAC(counterPtr);

    if (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        cpssOsMemSet(counterPtr, 0, sizeof(CPSS_DXCH_CNC_COUNTER_STC));

        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)
        {
            rc = prvCpssDrvHwPpPortGroupReadRam(
                devNum, portGroupId, regAddr, 2/*words*/, data);
            if (rc != GT_OK)
            {
                return rc;
            }

            rc = prvCpssDxChCncCounterHwToSw(
                format, data, &counter);
            if (rc != GT_OK)
            {
                return rc;
            }

            counterPtr->packetCount =
                prvCpssMathAdd64(
                    counterPtr->packetCount, counter.packetCount);

            counterPtr->byteCount =
                prvCpssMathAdd64(
                    counterPtr->byteCount, counter.byteCount);

        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)
    }
    else  /* non multi-port-groups device */
    {
        rc = prvCpssDrvHwPpReadRam(
            devNum, regAddr, 2/*words*/, data);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDxChCncCounterHwToSw(
            format, data, counterPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChCncBlockClientToCfgAddr
*
* DESCRIPTION:
*   The function converts the client to HW base address of configuration register.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
*       client         - CNC client
*       blockNum       - CNC block number
*                        valid range see in datasheet of specific device.
* OUTPUTS:
*       cfgRegPtr    - (pointer to) address of the block configuration register.
*       bitOffsetPtr - (pointer to) bit offset of the block configuration.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChCncBlockClientToCfgAddr
(
    IN   GT_U8                     devNum,
    IN   CPSS_DXCH_CNC_CLIENT_ENT  client,
    IN   GT_U32                    blockNum,
    OUT  GT_U32                    *cfgRegPtr,
    OUT  GT_U32                    *bitOffsetPtr
)
{
    GT_U32      index;  /* client HW index */

    PRV_CPSS_DXCH_CNC_BLOCK_INDEX_VALID_CHECK_MAC(devNum, blockNum);

    switch (client)
    {
        case CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E:      index = 0; break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_E:   index = 1; break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_1_E: index = 2; break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_E:   index = 3; break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E: index = 4; break;
        case CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E:  index = 5; break;
        case CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E: index = 6; break;
        case CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_E:             index = 7; break;
        case CPSS_DXCH_CNC_CLIENT_ARP_TABLE_ACCESS_E:       index = 8; break;
        case CPSS_DXCH_CNC_CLIENT_TUNNEL_START_E:           index = 9; break;
        case CPSS_DXCH_CNC_CLIENT_TTI_E:                    index = 10; break;
        default: return GT_BAD_PARAM;
    }

    if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        /* Lion, xCat2   */
        /* per block registers base */
        *cfgRegPtr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                cncRegs.blockClientCfgAddrBase
           + (CNC_BLOCK_CLIENT_CFG_ADDR_INCREMENT_CNS * (blockNum));

        /* client enable bit */
        *bitOffsetPtr = index;
    }
    else
    {
        /* DxCh3, xCat         */
        /* block + client register */
        *cfgRegPtr = PRV_CPSS_DXCH_CNC_CLIENT_CFG_ADDR_MAC(
            devNum, blockNum, index);

        /* bits 12:0 for even, bits 26:13 for odd */
        *bitOffsetPtr = (index & 1) ? 13 : 0;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChCncClientCountModeRegGet
*
* DESCRIPTION:
*   The function calculates register address, bit offset and L2-mode
*   pattern for CNC client count mode.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
*       client         - CNC client
*
* OUTPUTS:
*       regAddrPtr       - (pointer to) register address.
*       bitOffPtr        - (pointer to) bit offset in register.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxChCncClientCountModeRegGet
(
    IN   GT_U8                     devNum,
    IN   CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT  GT_U32                    *regAddrPtr,
    OUT  GT_U32                    *bitOffPtr
)
{
    switch (client)
    {
        case CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E:
            *regAddrPtr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.cncCountMode;
            *bitOffPtr = 3;
            break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_E:
            *regAddrPtr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.cncCountMode;
            *bitOffPtr = 0;
            break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_1_E:
            *regAddrPtr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.cncCountMode;
            *bitOffPtr = 1;
            break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_E:
            *regAddrPtr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.cncCountMode;
            *bitOffPtr = 2;
            break;
        case CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E:
            *regAddrPtr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    bufferMng.eqBlkCfgRegs.preEgrEngineGlobal;
            *bitOffPtr = 14;
            break;
        case CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E:
            if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
            {
                *regAddrPtr =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.cncCountMode;
            }
            else
            {
                *regAddrPtr =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.
                        peripheralAccess.cncModes.modesRegister;
            }
            *bitOffPtr = 2;
            break;
        case CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E:
            if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
            {
                *regAddrPtr =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.cncCountMode;
            }
            else
            {
                *regAddrPtr =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.
                        peripheralAccess.cncModes.modesRegister;
            }
            *bitOffPtr = 0;
            break;
        case CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_E:
            *regAddrPtr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.epclGlobalConfig;
            *bitOffPtr = 25;
            break;
        case CPSS_DXCH_CNC_CLIENT_ARP_TABLE_ACCESS_E:
            *regAddrPtr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;
            *bitOffPtr = 0;
            break;
        case CPSS_DXCH_CNC_CLIENT_TUNNEL_START_E:
            *regAddrPtr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;
            *bitOffPtr = 1;
            break;
        case CPSS_DXCH_CNC_CLIENT_TTI_E:
            if(PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
            {
                *regAddrPtr =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiUnitConfig;
                *bitOffPtr = 4;
            }
            else
            {
                *regAddrPtr =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.cncCountMode;
                *bitOffPtr = 4;
            }
            break;

        default: return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncBlockClientEnableSet
*
* DESCRIPTION:
*   The function binds/unbinds the selected client to/from a counter block.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
*       blockNum       - CNC block number
*                        valid range see in datasheet of specific device.
*       client         - CNC client
*       updateEnable   - the client enable to update the block
*                        GT_TRUE - enable, GT_FALSE - disable
*                        It is forbidden to enable update the same
*                        block by more then one client. When an
*                        application enables some client it is responsible
*                        to disable all other clients it enabled before
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncBlockClientEnableSet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    IN  GT_BOOL                   updateEnable
)
{
    return cpssDxChCncPortGroupBlockClientEnableSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        blockNum, client, updateEnable);
}

/*******************************************************************************
* cpssDxChCncBlockClientEnableGet
*
* DESCRIPTION:
*   The function gets bind/unbind of the selected client to a counter block.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       blockNum       - CNC block number
*                        valid range see in datasheet of specific device.
*       client         -  CNC client
*
* OUTPUTS:
*       updateEnablePtr - (pointer to) the client enable to update the block
*                         GT_TRUE - enable, GT_FALSE - disable
*                         It is forbidden to enable update the same
*                         block by more then one client. When an
*                         application enables some client it is responsible
*                         to disable all other clients it enabled before
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncBlockClientEnableGet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_BOOL                   *updateEnablePtr
)
{
    return cpssDxChCncPortGroupBlockClientEnableGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        blockNum, client, updateEnablePtr);
}

/*******************************************************************************
* cpssDxChCncBlockClientRangesSet
*
* DESCRIPTION:
*   The function sets index ranges per CNC client and Block
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       client           - CNC client
*       indexRangesBmp   - the counter index ranges bitmap
*                         DxCh3 and xCat devices have 8 ranges of counter indexes,
*                         each 2048 indexes (0..(2K-1), 2K..(4K-1), ..., 14K..(16K-1))
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         Lion devices has 64 ranges (512 indexes each).
*                         xCat2 devices has 64 ranges (256 indexes each).
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on not relevant bits in indexRangesBmp
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncBlockClientRangesSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT    client,
    IN  GT_U64                      indexRangesBmp
)
{
    return cpssDxChCncPortGroupBlockClientRangesSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        blockNum, client, indexRangesBmp);
}

/*******************************************************************************
* cpssDxChCncBlockClientRangesGet
*
* DESCRIPTION:
*   The function gets index ranges per CNC client and Block
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       client           - CNC client
*
* OUTPUTS:
*       indexRangesBmpPtr - (pointer to) the counter index ranges bitmap
*                         DxCh3 and xCat devices have 8 ranges of counter indexes,
*                         each 2048 indexes (0..(2K-1), 2K..(4K-1), ..., 14K..(16K-1))
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         Lion devices has 64 ranges (512 indexes each).
*                         xCat2 devices has 64 ranges (256 indexes each).
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncBlockClientRangesGet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_U64                    *indexRangesBmpPtr
)
{
    return cpssDxChCncPortGroupBlockClientRangesGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        blockNum, client, indexRangesBmpPtr);
}

/*******************************************************************************
* cpssDxChCncPortClientEnableSet
*
* DESCRIPTION:
*   The function sets the given client Enable counting per port.
*   Currently only L2/L3 Ingress Vlan client supported.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portNum          - port number
*       client           - CNC client
*       enable           - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortClientEnableSet
(
    IN  GT_U8                     devNum,
    IN  GT_U8                     portNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    IN  GT_BOOL                   enable
)
{
    GT_U32 regAddr;          /* register address  */
    GT_U32 fieldOffset;      /* field Offset      */
    GT_U32 value;            /* field value       */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    switch (client)
    {
        case CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E: break;
        default: return GT_BAD_PARAM;
    }

    value = (enable == GT_FALSE) ? 0 : 1;

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.cncl2l3IngressVlanCountEnable;

    fieldOffset = (localPort != CPSS_CPU_PORT_NUM_CNS) ? localPort : 31;

    return prvCpssDrvHwPpPortGroupSetRegField(
        devNum, portGroupId ,regAddr, fieldOffset, 1/*fieldLength*/, value);
}

/*******************************************************************************
* cpssDxChCncPortClientEnableGet
*
* DESCRIPTION:
*   The function gets the given client Enable counting per port.
*   Currently only L2/L3 Ingress Vlan client supported.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portNum          - port number
*       client           - CNC client
*
* OUTPUTS:
*       enablePtr        - (pointer to) enable
*                          GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortClientEnableGet
(
    IN  GT_U8                     devNum,
    IN  GT_U8                     portNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_BOOL                   *enablePtr
)
{
    GT_STATUS rc;               /* return code       */
    GT_U32    regAddr;          /* register address  */
    GT_U32    fieldOffset;      /* field Offset      */
    GT_U32    value;            /* field value       */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    switch (client)
    {
        case CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E: break;
        default: return GT_BAD_PARAM;
    }

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.cncl2l3IngressVlanCountEnable;

    fieldOffset = (localPort != CPSS_CPU_PORT_NUM_CNS) ? localPort : 31;

    rc = prvCpssDrvHwPpPortGroupGetRegField(
        devNum, portGroupId , regAddr, fieldOffset, 1/*fieldLength*/, &value);

    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (value == 0) ? GT_FALSE : GT_TRUE;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncIngressVlanPassDropFromCpuCountEnableSet
*
* DESCRIPTION:
*   The function enables or disables counting of FROM_CPU packets
*   for the Ingress Vlan Pass/Drop CNC client.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       enable           - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncIngressVlanPassDropFromCpuCountEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
)
{
    GT_U32 data;             /* data              */
    GT_U32 regAddr;          /* register address  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bufferMng.eqBlkCfgRegs.preEgrEngineGlobal;

    data = (enable == GT_FALSE) ? 0 : 1;

    return prvCpssDrvHwPpSetRegField(
        devNum, regAddr, 15/*fieldOffset*/, 1/*fieldLength*/, data);
}

/*******************************************************************************
* cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet
*
* DESCRIPTION:
*   The function gets status of counting of FROM_CPU packets
*   for the Ingress Vlan Pass/Drop CNC client.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       enablePtr        - (pointer to) enable
*                          GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;               /* return code       */
    GT_U32    data;             /* data              */
    GT_U32    regAddr;          /* register address  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bufferMng.eqBlkCfgRegs.preEgrEngineGlobal;

    rc = prvCpssDrvHwPpGetRegField(
        devNum, regAddr, 15/*fieldOffset*/, 1/*fieldLength*/, &data);

    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (data == 0) ? GT_FALSE : GT_TRUE;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncClientByteCountModeSet
*
* DESCRIPTION:
*   The function sets byte count counter mode for CNC client.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       client           - CNC client
*       countMode        - count mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncClientByteCountModeSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT          client,
    IN  CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT countMode
)
{
    GT_STATUS rc;               /* return code        */
    GT_U32    data;             /* data               */
    GT_U32    regAddr;          /* register address   */
    GT_U32    bitOff;           /* offset of mode bit */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    rc = prvCpssDxChCncClientCountModeRegGet(
        devNum, client, &regAddr, &bitOff);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (countMode)
    {
        case CPSS_DXCH_CNC_BYTE_COUNT_MODE_L2_E: data = 0; break;
        case CPSS_DXCH_CNC_BYTE_COUNT_MODE_L3_E: data = 1; break;
        default: return GT_BAD_PARAM;
    }

    return prvCpssDxChHwPpSetRegField(
        devNum, regAddr, bitOff, 1/*fieldLength*/, data);
}

/*******************************************************************************
* cpssDxChCncClientByteCountModeGet
*
* DESCRIPTION:
*   The function gets byte count counter mode for CNC client.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       client           - CNC client
*
* OUTPUTS:
*       countModePtr     - (pointer to) count mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncClientByteCountModeGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT          client,
    OUT CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT *countModePtr
)
{
    GT_STATUS rc;               /* return code        */
    GT_U32    data;             /* data               */
    GT_U32    regAddr;          /* register address   */
    GT_U32    bitOff;           /* offset of mode bit */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(countModePtr);

    rc = prvCpssDxChCncClientCountModeRegGet(
        devNum, client, &regAddr, &bitOff);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChHwPpGetRegField(
        devNum, regAddr, bitOff, 1/*fieldLength*/, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    *countModePtr = (data == 0)
        ? CPSS_DXCH_CNC_BYTE_COUNT_MODE_L2_E
        : CPSS_DXCH_CNC_BYTE_COUNT_MODE_L3_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncEgressVlanDropCountModeSet
*
* DESCRIPTION:
*   The function sets Egress VLAN Drop counting mode.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       mode             - Egress VLAN Drop counting mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncEgressVlanDropCountModeSet
(
    IN  GT_U8                                    devNum,
    IN  CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT mode
)
{
    GT_STATUS rc;               /* return code        */
    GT_U32    data;             /* data               */
    GT_U32    regAddr;          /* register address   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    rc = prvCpssDxChCncClientCountModeRegGet(
           devNum, CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E, &regAddr, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (mode)
    {
        case CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_AND_TAIL_DROP_E:
            data = 0;
            break;
        case CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_ONLY_E:
            data = 1;
            break;
        case CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_TAIL_DROP_ONLY_E:
            data = 2;
            break;
        default: return GT_BAD_PARAM;
    }

    return prvCpssDxChHwPpSetRegField(
        devNum, regAddr, 4/*fieldOffset*/, 2/*fieldLength*/, data);
}

/*******************************************************************************
* cpssDxChCncEgressVlanDropCountModeGet
*
* DESCRIPTION:
*   The function gets Egress VLAN Drop counting mode.
*
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       modePtr             - (pointer to) Egress VLAN Drop counting mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - on reserved value found in HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncEgressVlanDropCountModeGet
(
    IN  GT_U8                                    devNum,
    OUT CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT *modePtr
)
{
    GT_STATUS rc;               /* return code        */
    GT_U32    data;             /* data               */
    GT_U32    regAddr;          /* register address   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    rc = prvCpssDxChCncClientCountModeRegGet(
           devNum, CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E, &regAddr, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChHwPpGetRegField(
        devNum, regAddr, 4/*fieldOffset*/, 2/*fieldLength*/, &data);

    switch (data)
    {
        case 0:
            *modePtr =
                CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_AND_TAIL_DROP_E;
            break;
        case 1:
            *modePtr =
                CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_ONLY_E;
            break;
        case 2:
            *modePtr =CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_TAIL_DROP_ONLY_E;
            break;
        default: return GT_BAD_STATE;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncCounterClearByReadEnableSet
*
* DESCRIPTION:
*   The function enable/disables clear by read mode of CNC counters read
*   operation.
*   If clear by read mode is disable the counters after read
*   keep the current value and continue to count normally.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum       - device number
*       enable       - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterClearByReadEnableSet
(
    IN  GT_U8    devNum,
    IN  GT_BOOL  enable
)
{
    GT_U32    data;             /* data               */
    GT_U32    regAddr;          /* register address   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.globalCfg;

    data = (enable == GT_FALSE) ? 0 : 1;

    return prvCpssDrvHwPpSetRegField(
        devNum, regAddr, 1/*fieldOffset*/, 1/*fieldLength*/, data);
}

/*******************************************************************************
* cpssDxChCncCounterClearByReadEnableGet
*
* DESCRIPTION:
*   The function gets clear by read mode status of CNC counters read
*   operation.
*   If clear by read mode is disable the counters after read
*   keep the current value and continue to count normally.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       enablePtr    - (pointer to) enable
*                      GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterClearByReadEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;               /* return code       */
    GT_U32    regAddr;          /* register address  */
    GT_U32    data;             /* field value       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.globalCfg;

    rc = prvCpssDrvHwPpGetRegField(
        devNum, regAddr, 1/*fieldOffset*/, 1/*fieldLength*/, &data);

    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (data == 0) ? GT_FALSE : GT_TRUE;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncCounterClearByReadValueSet
*
* DESCRIPTION:
*   The function sets the counter clear by read globally configured value.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*       counterPtr       - (pointer to) counter contents
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterClearByReadValueSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    IN  CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
)
{
    GT_U32    regAddr;          /* register address           */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.clearByReadValueWord0;

    return prvCpssDxChCncPortGroupCounterValueAverageSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        regAddr, format, counterPtr);
}

/*******************************************************************************
* cpssDxChCncCounterClearByReadValueGet
*
* DESCRIPTION:
*   The function gets the counter clear by read globally configured value.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*
* OUTPUTS:
*       counterPtr       - (pointer to) counter contents
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterClearByReadValueGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    OUT CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
)
{
    GT_U32    regAddr;          /* register address  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.clearByReadValueWord0;

    return prvCpssDxChCncPortGroupCounterValueSummingGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        regAddr, format, counterPtr);
}

/*******************************************************************************
* cpssDxChCncCounterWraparoundEnableSet
*
* DESCRIPTION:
*   The function enables/disables wraparound for all CNC counters
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum       - device number
*       enable       - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterWraparoundEnableSet
(
    IN  GT_U8    devNum,
    IN  GT_BOOL  enable
)
{
    GT_U32    data;             /* data               */
    GT_U32    regAddr;          /* register address   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.globalCfg;

    data = (enable == GT_FALSE) ? 0 : 1;

    return prvCpssDrvHwPpSetRegField(
        devNum, regAddr, 0/*fieldOffset*/, 1/*fieldLength*/, data);
}

/*******************************************************************************
* cpssDxChCncCounterWraparoundEnableGet
*
* DESCRIPTION:
*   The function gets status of wraparound for all CNC counters
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       enablePtr    - (pointer to) enable
*                      GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterWraparoundEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;               /* return code       */
    GT_U32    regAddr;          /* register address  */
    GT_U32    data;             /* field value       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.globalCfg;

    rc = prvCpssDrvHwPpGetRegField(
        devNum, regAddr, 0/*fieldOffset*/, 1/*fieldLength*/, &data);

    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (data == 0) ? GT_FALSE : GT_TRUE;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncCounterWraparoundIndexesGet
*
* DESCRIPTION:
*   The function gets the counter Wrap Around last 8 indexes
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum       - device number
*       blockNum     - CNC block number
*                      valid range see in datasheet of specific device.
*       indexNumPtr  - (pointer to) maximal size of array of indexes
*
* OUTPUTS:
*       indexNumPtr  - (pointer to) actual size of array of indexes
*       indexesArr[] - (pointer to) array of indexes of counters wrapped around
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The wraparound indexes are cleared on read.
*       Since in the HW each pair of wraparound indexes resides in one register,
*       cleared by read, in case not all the 8 possible indexes are read at once,
*       some data may be lost.
*       Recommendation: Always request 8 indexes => *indexNumPtr=8 as input.
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterWraparoundIndexesGet
(
    IN    GT_U8    devNum,
    IN    GT_U32   blockNum,
    INOUT GT_U32   *indexNumPtr,
    OUT   GT_U32   indexesArr[]
)
{
    return cpssDxChCncPortGroupCounterWraparoundIndexesGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        blockNum, indexNumPtr,
        NULL /*portGroupIdArr*/, indexesArr);
}

/*******************************************************************************
* cpssDxChCncCounterSet
*
* DESCRIPTION:
*   The function sets the counter contents
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       index            - counter index in the block
*                          valid range see in datasheet of specific device.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*       counterPtr       - (pointer to) counter contents
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterSet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            blockNum,
    IN  GT_U32                            index,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    IN  CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
)
{
    return cpssDxChCncPortGroupCounterSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        blockNum, index, format, counterPtr);
}

/*******************************************************************************
* cpssDxChCncCounterGet
*
* DESCRIPTION:
*   The function gets the counter contents
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       index            - counter index in the block
*                          valid range see in datasheet of specific device.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*
* OUTPUTS:
*       counterPtr       - (pointer to) received CNC counter value.
*                          For multi port group devices
*                          the result counter contains the sum of
*                          counters read from all port groups.
*                          It contains more bits than in HW.
*                          For example the sum of 4 35-bit values may be
*                          37-bit value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterGet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            blockNum,
    IN  GT_U32                            index,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    OUT CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
)
{
    return cpssDxChCncPortGroupCounterGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        blockNum, index, format, counterPtr);
}

/*******************************************************************************
* cpssDxChCncBlockUploadTrigger
*
* DESCRIPTION:
*   The function triggers the Upload of the given counters block.
*   The function checks that there is no unfinished CNC and FDB upload (FU).
*   Also the function checks that all FU messages of previous FU were retrieved
*   by cpssDxChBrgFdbFuMsgBlockGet.
*   An application may check that CNC upload finished by
*   cpssDxChCncBlockUploadInProcessGet.
*   An application may sequentially upload several CNC blocks before start
*   to retrieve uploaded counters.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE   - if the previous CNC upload or FU in process or
*                        FU is finished but all FU messages were not
*                        retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by cpssDxChCncUploadedBlockGet.
*
*******************************************************************************/
GT_STATUS cpssDxChCncBlockUploadTrigger
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      blockNum
)
{
    return cpssDxChCncPortGroupBlockUploadTrigger(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, blockNum);
}

/*******************************************************************************
* cpssDxChCncBlockUploadInProcessGet
*
* DESCRIPTION:
*   The function gets bitmap of numbers of such counters blocks that upload
*   of them yet in process. The HW cannot keep more then one block in such
*   state, but an API matches the original HW representation of the state.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       inProcessBlocksBmpPtr     - (pointer to) bitmap of in-process blocks
*                                   value 1 of the bit#n means that CNC upload
*                                   is not finished yet for block n
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncBlockUploadInProcessGet
(
    IN  GT_U8                       devNum,
    OUT GT_U32                     *inProcessBlocksBmpPtr
)
{
    return cpssDxChCncPortGroupBlockUploadInProcessGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, inProcessBlocksBmpPtr);
}

/*******************************************************************************
* cncUploadedBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of CNC counter values,
*       the maximal number of elements defined by the caller.
*       The CNC upload may triggered by cpssDxChCncBlockUploadTrigger.
*       The CNC upload transfers whole CNC block (2K CNC counters)
*       to FDB Upload queue. An application must get all transferred counters.
*       An application may sequentially upload several CNC blocks before start
*       to retrieve uploaded counters.
*       The device may transfer only part of CNC block because of FUQ full. In
*       this case the cpssDxChCncUploadedBlockGet may return only part of the
*       CNC block with return GT_OK. An application must to call
*       cpssDxChCncUploadedBlockGet one more time to get rest of the block.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum                - the device number from which FU are taken
*       portGroupId                - the port group Id. to support multi-port-groups device.
*       initOutCounters       - GT_TRUE - override the memory contents
*                             - GT_FALSE - add to memory contents
*       numOfCounterValuesPtr - (pointer to) maximal number of CNC counters
*                               values to get.This is the size of
*                               counterValuesPtr array allocated by caller.
*       format                - CNC counter HW format,
*                               relevant only for Lion and above
*
* OUTPUTS:
*       numOfCounterValuesPtr - (pointer to) actual number of CNC
*                               counters values in counterValuesPtr.
*       counterValuesPtr      - (pointer to) array that holds received CNC
*                               counters values. Array must be allocated by
*                               caller.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_NO_MORE       - the action succeeded and there are no more waiting
*                          CNC counter value
*       GT_FAIL                  - on failure
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - if the previous FU messages were not
*                          retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by cpssDxChCncUploadedBlockGet.
*
*******************************************************************************/
static GT_STATUS cncUploadedBlockGet
(
    IN     GT_U8                             devNum,
    IN     GT_U32                            portGroupId,
    IN     GT_BOOL                           initOutCounters,
    IN     CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    INOUT  GT_U32                            *numOfCounterValuesPtr,
    OUT    CPSS_DXCH_CNC_COUNTER_STC         *counterValuesPtr
)
{
    GT_STATUS                   rc;            /* return code                   */
    PRV_CPSS_AU_DESC_STC        *descPtr;   /*pointer to the current descriptor */
    PRV_CPSS_AU_DESC_CTRL_STC   *descCtrlPtr;  /* pointer to the descriptors DB
                                                 of the device                  */
    PRV_CPSS_AU_DESC_STC        *descBlockPtr; /* AU descriptors block          */
    GT_U32                      ii;            /* iterator                      */
    GT_U32                      phyAddr;       /* phy address                   */
    GT_U32                      cncMaxCnt;     /* number of CNC messages        */
    GT_U32                      regAddr;       /* register address              */
    GT_U32                      word[2];       /* work data                     */
    CPSS_DXCH_CNC_COUNTER_STC   cuppPortGroupCounter;/* current counter value for multi-port-groups support */

    /* check that the FUQ initialized */
    PRV_CPSS_FUQ_DESCRIPTORS_INIT_DONE_CHECK_MAC(devNum);

    descCtrlPtr = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[portGroupId]);
    regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.fuQBaseAddr;

    /* get address of AU descriptors block */
    descBlockPtr = (PRV_CPSS_AU_DESC_STC*)(descCtrlPtr->blockAddr);

    /* pointer to the current descriptor */
    descPtr = &(descBlockPtr[descCtrlPtr->currDescIdx]);

    cncMaxCnt = *numOfCounterValuesPtr;

    *numOfCounterValuesPtr = 0;

    /* read number of AU descriptors */
    for (ii = 0; (ii < cncMaxCnt); ii++)
    {
        if (descCtrlPtr->unreadCncCounters == 0)
        {
            *numOfCounterValuesPtr = ii;
            return GT_NO_MORE;
        }

        /* point descPtr to the next AU descriptor in the queue */
        descPtr = &(descBlockPtr[descCtrlPtr->currDescIdx]);

        if (initOutCounters != GT_FALSE)
        {
            /* start with clean counter value */
            cpssOsMemSet(
                &(counterValuesPtr[ii]), 0,
                sizeof(CPSS_DXCH_CNC_COUNTER_STC));
        }

        /* even counter in the first part of upload record */
        if ((descCtrlPtr->unreadCncCounters & 1) == 0)
        {
            word[0] = prvCpssDrvHwPpPortGroupByteSwap(devNum,portGroupId, descPtr->word0);
            word[1] = prvCpssDrvHwPpPortGroupByteSwap(devNum,portGroupId, descPtr->word1);

            rc = prvCpssDxChCncCounterHwToSw(
                format, word, &cuppPortGroupCounter);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* summary the counter from all the port groups */
            counterValuesPtr[ii].byteCount =
                prvCpssMathAdd64(counterValuesPtr[ii].byteCount,
                    cuppPortGroupCounter.byteCount);

            counterValuesPtr[ii].packetCount =
                prvCpssMathAdd64(counterValuesPtr[ii].packetCount,
                    cuppPortGroupCounter.packetCount);

            descCtrlPtr->unreadCncCounters --;
            continue;
        }

        /* odd counter in the second part of upload record */
        word[0] = prvCpssDrvHwPpPortGroupByteSwap(devNum,portGroupId, descPtr->word2);
        word[1] = prvCpssDrvHwPpPortGroupByteSwap(devNum,portGroupId, descPtr->word3);

        rc = prvCpssDxChCncCounterHwToSw(
            format, word, &cuppPortGroupCounter);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* summary the counter from all the port groups */
        counterValuesPtr[ii].byteCount =
            prvCpssMathAdd64(counterValuesPtr[ii].byteCount,
                cuppPortGroupCounter.byteCount);

        counterValuesPtr[ii].packetCount =
            prvCpssMathAdd64(counterValuesPtr[ii].packetCount,
                cuppPortGroupCounter.packetCount);

        descCtrlPtr->unreadCncCounters --;

        descCtrlPtr->currDescIdx = ((descCtrlPtr->currDescIdx + 1) %
                                   descCtrlPtr->blockSize);

        /* erase memory to be compatible with FU messages upload */
        AU_DESC_RESET_MAC(descPtr);

        /* reached the end of AU block */
        if(descCtrlPtr->currDescIdx == 0)
        {
            /* reset the FU descriptors queue */
            rc = cpssOsVirt2Phy(descCtrlPtr->blockAddr, &phyAddr);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,phyAddr);
            *numOfCounterValuesPtr = ii;
            return rc;
        }
    }

    *numOfCounterValuesPtr = cncMaxCnt;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncUploadedBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of CNC counter values,
*       the maximal number of elements defined by the caller.
*       The CNC upload may triggered by cpssDxChCncBlockUploadTrigger.
*       The CNC upload transfers whole CNC block (2K CNC counters)
*       to FDB Upload queue. An application must get all transferred counters.
*       An application may sequentially upload several CNC blocks before start
*       to retrieve uploaded counters.
*       The device may transfer only part of CNC block because of FUQ full. In
*       this case the cpssDxChCncUploadedBlockGet may return only part of the
*       CNC block with return GT_OK. An application must to call
*       cpssDxChCncUploadedBlockGet one more time to get rest of the block.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum                - the device number from which FU are taken
*       numOfCounterValuesPtr - (pointer to) maximal number of CNC counters
*                               values to get.This is the size of
*                               counterValuesPtr array allocated by caller.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*
* OUTPUTS:
*       numOfCounterValuesPtr - (pointer to) actual number of CNC
*                               counters values in counterValuesPtr.
*       counterValuesPtr      - (pointer to) array that holds received CNC
*                               counters values. Array must be allocated by
*                               caller. For multi port group devices
*                               the result counters contain the sum of
*                               counters read from all port groups.
*                               It contains more bits than in HW.
*                               For example the sum of 4 35-bit values may be
*                               37-bit value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE       - the action succeeded and there are no more waiting
*                          CNC counter value
*       GT_FAIL                  - on failure
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - if the previous FU messages were not
*                          retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by cpssDxChCncUploadedBlockGet.
*
*******************************************************************************/
GT_STATUS cpssDxChCncUploadedBlockGet
(
    IN     GT_U8                             devNum,
    INOUT  GT_U32                            *numOfCounterValuesPtr,
    IN     CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    OUT    CPSS_DXCH_CNC_COUNTER_STC         *counterValuesPtr
)
{
    return cpssDxChCncPortGroupUploadedBlockGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        numOfCounterValuesPtr, format, counterValuesPtr);
}

/*******************************************************************************
* cpssDxChCncCountingEnableSet
*
* DESCRIPTION:
*   The function enables counting on selected cnc unit.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - device number
*       cncUnit - selected unit for enable\disable counting
*       enable  - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCountingEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT  cncUnit,
    IN  GT_BOOL                                 enable
)
{
    GT_U32 data;        /* data                   */
    GT_U32 regAddr;     /* register address       */
    GT_U32 bitOffset;   /* bit offset in register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    switch (cncUnit)
    {
        case CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_TTI_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiUnitConfig;
            bitOffset = 3;
            break;
        case CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_PCL_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.cncCountMode;
            bitOffset = 4;
            break;
        default:
            return GT_BAD_PARAM;
    }

    data = (enable == GT_FALSE) ? 0 : 1;

    return prvCpssDrvHwPpSetRegField(
        devNum, regAddr, bitOffset/*fieldOffset*/, 1/*fieldLength*/, data);
}

/*******************************************************************************
* cpssDxChCncCountingEnableGet
*
* DESCRIPTION:
*   The function gets enable counting status on selected cnc unit.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - device number
*       cncUnit - selected unit for enable\disable counting
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable
*                     GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCountingEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT  cncUnit,
    OUT GT_BOOL                                 *enablePtr
)
{
    GT_STATUS rc;       /* return code            */
    GT_U32 data;        /* data                   */
    GT_U32 regAddr;     /* register address       */
    GT_U32 bitOffset;   /* bit offset in register */

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    switch (cncUnit)
    {
        case CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_TTI_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiUnitConfig;
            bitOffset = 3;
            break;
        case CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_PCL_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.cncCountMode;
            bitOffset = 4;
            break;
        default:
            return GT_BAD_PARAM;
    }

    rc = prvCpssDrvHwPpGetRegField(
        devNum, regAddr, bitOffset/*fieldOffset*/, 1/*fieldLength*/, &data);

    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (data == 0) ? GT_FALSE : GT_TRUE;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncCounterFormatSet
*
* DESCRIPTION:
*   The function sets format of CNC counter
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*       format          - CNC counter format
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterFormatSet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            blockNum,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format
)
{
    return cpssDxChCncPortGroupCounterFormatSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        blockNum, format);
}

/*******************************************************************************
* cpssDxChCncCounterFormatGet
*
* DESCRIPTION:
*   The function gets format of CNC counter
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*
* OUTPUTS:
*       formatPtr       - (pointer to) CNC counter format
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*                                  on not supported client for device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterFormatGet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            blockNum,
    OUT CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  *formatPtr
)
{
    return cpssDxChCncPortGroupCounterFormatGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        blockNum, formatPtr);
}

/*******************************************************************************
* cpssDxChCncEgressQueueClientModeSet
*
* DESCRIPTION:
*   The function sets Egress Queue client counting mode
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum          - device number
*       mode            - Egress Queue client counting mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncEgressQueueClientModeSet
(
    IN  GT_U8                                      devNum,
    IN  CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT mode
)
{
    GT_U32 data;        /* data                   */
    GT_U32 regAddr;     /* register address       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.queue.peripheralAccess.cncModes.modesRegister;

    switch (mode)
    {
        case CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_TAIL_DROP_E:
            data = 0;
            break;
        case CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_CN_E:
            data = 1;
            break;
        default: return  GT_BAD_PARAM;
    }

    return prvCpssDxChHwPpSetRegField(
        devNum, regAddr, 8/*fieldOffset*/, 1/*fieldLength*/, data);
}

/*******************************************************************************
* cpssDxChCncEgressQueueClientModeGet
*
* DESCRIPTION:
*   The function gets Egress Queue client counting mode
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       modePtr         - (pointer to) Egress Queue client counting mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncEgressQueueClientModeGet
(
    IN  GT_U8                                      devNum,
    OUT CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT *modePtr
)
{
    GT_STATUS rc;       /* return code            */
    GT_U32 data;        /* data                   */
    GT_U32 regAddr;     /* register address       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.queue.peripheralAccess.cncModes.modesRegister;

    rc = prvCpssDxChHwPpGetRegField(
        devNum, regAddr, 8/*fieldOffset*/, 1/*fieldLength*/, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    *modePtr =
        (data == 0)
            ? CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_TAIL_DROP_E
            : CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_CN_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncCpuAccessStrictPriorityEnableSet
*
* DESCRIPTION:
*   The function enables strict priority of CPU access to counter blocks
*   Disable: Others clients have strict priority
*   Enable:  CPU has strict priority
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       enable          - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCpuAccessStrictPriorityEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
)
{
    GT_U32 data;        /* data                   */
    GT_U32 regAddr;     /* register address       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.globalCfg;

    data = (enable == GT_FALSE) ? 0 : 1;

    return prvCpssDrvHwPpSetRegField(
        devNum, regAddr, 10/*fieldOffset*/, 1/*fieldLength*/, data);
}

/*******************************************************************************
* cpssDxChCncCpuAccessStrictPriorityEnableGet
*
* DESCRIPTION:
*   The function gets enable status of strict priority of
*   CPU access to counter blocks
*   Disable: Others clients have strict priority
*   Enable:  CPU has strict priority
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCpuAccessStrictPriorityEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr
)
{
    GT_STATUS rc;       /* return code            */
    GT_U32 data;        /* data                   */
    GT_U32 regAddr;     /* register address       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.globalCfg;

    rc = prvCpssDrvHwPpGetRegField(
        devNum, regAddr, 10/*fieldOffset*/, 1/*fieldLength*/, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (data == 0) ? GT_FALSE : GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncPortGroupBlockClientEnableSet
*
* DESCRIPTION:
*   The function binds/unbinds the selected client to/from a counter block.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum       - CNC block number
*                        valid range see in datasheet of specific device.
*       client         - CNC client
*       updateEnable   - the client enable to update the block
*                        GT_TRUE - enable, GT_FALSE - disable
*                        It is forbidden to enable update the same
*                        block by more then one client. When an
*                        application enables some client it is responsible
*                        to disable all other clients it enabled before
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupBlockClientEnableSet
(
    IN  GT_U8                     devNum,
    IN  GT_PORT_GROUPS_BMP        portGroupsBmp,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    IN  GT_BOOL                   updateEnable
)
{
    GT_STATUS   rc;           /* return code                  */
    GT_U32      data;         /* data                         */
    GT_U32      regAddr;      /* register address             */
    GT_U32      fieldOffset;  /* field Offset                 */
    GT_U32      portGroupId;  /* the port group Id            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    if (prvCpssDxChCncBlockClientToCfgAddr(
        devNum, client, blockNum, &regAddr, &fieldOffset) != GT_OK)
    {
        return GT_BAD_PARAM;
    }

    data = (updateEnable == GT_FALSE) ? 0 : 1;

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        rc = prvCpssDrvHwPpPortGroupSetRegField(
            devNum, portGroupId, regAddr,
            fieldOffset, 1/*fieldLength*/, data);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncPortGroupBlockClientEnableGet
*
* DESCRIPTION:
*   The function gets bind/unbind of the selected client to a counter block.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       blockNum       - CNC block number
*                        valid range see in datasheet of specific device.
*       client         -  CNC client
*
* OUTPUTS:
*       updateEnablePtr - (pointer to) the client enable to update the block
*                         GT_TRUE - enable, GT_FALSE - disable
*                         It is forbidden to enable update the same
*                         block by more then one client. When an
*                         application enables some client it is responsible
*                         to disable all other clients it enabled before
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupBlockClientEnableGet
(
    IN  GT_U8                     devNum,
    IN  GT_PORT_GROUPS_BMP        portGroupsBmp,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_BOOL                   *updateEnablePtr
)
{
    GT_STATUS rc;             /* return code        */
    GT_U32    data;           /* data               */
    GT_U32    regAddr;        /* register address   */
    GT_U32    fieldOffset;    /* field Offset       */
    GT_U32    portGroupId;    /* the port group Id  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(updateEnablePtr);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    if (prvCpssDxChCncBlockClientToCfgAddr(
        devNum, client, blockNum, &regAddr, &fieldOffset) != GT_OK)
    {
        return GT_BAD_PARAM;
    }

    rc = prvCpssDrvHwPpPortGroupGetRegField(
        devNum, portGroupId,
        regAddr, fieldOffset, 1/*fieldLength*/, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    *updateEnablePtr = (data == 0) ? GT_FALSE : GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncPortGroupBlockClientRangesSet
*
* DESCRIPTION:
*   The function sets index ranges per CNC client and Block
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       client           - CNC client
*       indexRangesBmp   - bitmap of counter index ranges
*                         DxCh3 and xCat devices have 8 ranges of counter indexes,
*                         each 2048 indexes (0..(2K-1), 2K..(4K-1), ..., 14K..(16K-1))
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         Lion devices has 64 ranges (512 indexes each).
*                         xCat2 devices has 64 ranges (256 indexes each).
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on not relevant bits in indexRangesBmp
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupBlockClientRangesSet
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  GT_U32                      blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT    client,
    IN  GT_U64                      indexRangesBmp
)
{
    GT_STATUS rc;            /* return code       */
    GT_U32    regAddr;       /* register address  */
    GT_U32    fieldOffset;   /* field Offset      */
    GT_U32    portGroupId;   /* the port group Id */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    if (prvCpssDxChCncBlockClientToCfgAddr(
        devNum, client, blockNum, &regAddr, &fieldOffset) != GT_OK)
    {
        return GT_BAD_PARAM;
    }

    if (! PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        /* DxCh3, xCat - 8 ranges of 2048 counters */

        /* check that indexRangesBmp does not contain bits 8-31 */
        if ((indexRangesBmp.l[0] > 0xFF) || (indexRangesBmp.l[1] != 0))
        {
            return GT_OUT_OF_RANGE;
        }

        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(
            devNum, portGroupId, regAddr,
            (fieldOffset + 1), 8/*fieldLength*/, indexRangesBmp.l[0]);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)

        return GT_OK;
    }

    /* Lion, xCat2 - 64 ranges                          */
    /* The ranges mapped to this block for all clients  */

    /* The block configuration registers 1-2            */
    regAddr += 4;

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        rc = prvCpssDrvHwPpPortGroupWriteRam(
        devNum, portGroupId, regAddr,
        2/*words*/, indexRangesBmp.l);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncPortGroupBlockClientRangesGet
*
* DESCRIPTION:
*   The function gets index ranges per CNC client and Block
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       client           - CNC client
*
* OUTPUTS:
*       indexRangesBmpPtr - (pointer to) bitmap counter index ranges
*                         DxCh3 and xCat devices have 8 ranges of counter indexes,
*                         each 2048 indexes (0..(2K-1), 2K..(4K-1), ..., 14K..(16K-1))
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         Lion devices has 64 ranges (512 indexes each).
*                         xCat2 devices has 64 ranges (256 indexes each).
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupBlockClientRangesGet
(
    IN  GT_U8                     devNum,
    IN  GT_PORT_GROUPS_BMP        portGroupsBmp,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_U64                    *indexRangesBmpPtr
)
{
    GT_U32 regAddr;          /* register address  */
    GT_U32 fieldOffset;      /* field Offset      */
    GT_U32  portGroupId;     /* the port group Id */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(indexRangesBmpPtr);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    if (prvCpssDxChCncBlockClientToCfgAddr(
        devNum, client, blockNum, &regAddr, &fieldOffset) != GT_OK)
    {
        return GT_BAD_PARAM;
    }

    if (! PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        /* DxCh3, xCat - 8 ranges of 2048 counters */

        indexRangesBmpPtr->l[1] = 0;

        return prvCpssDrvHwPpPortGroupGetRegField(
            devNum, portGroupId, regAddr,
            (fieldOffset + 1), 8/*fieldLength*/,
            &(indexRangesBmpPtr->l[0]));
    }

    /* Lion, xCat2    - 64 ranges                       */
    /* The ranges mapped to this block for all clients  */

    /* The block configuration registers 1-2            */
    regAddr += 4;


    return prvCpssDrvHwPpPortGroupReadRam(
        devNum, portGroupId, regAddr,
        2/*words*/, indexRangesBmpPtr->l);
}

/*******************************************************************************
* cpssDxChCncPortGroupCounterWraparoundIndexesGet
*
* DESCRIPTION:
*   The function gets the counter Wrap Around last 8 indexes
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*       indexNumPtr     - (pointer to) maximal size of array of indexes
*
* OUTPUTS:
*       indexNumPtr     - (pointer to) actual size of array of indexes
*       portGroupIdArr[]- (pointer to) array of port group Ids of
*                         counters wrapped around.
*                         The NULL pointer supported.
*       indexesArr[]    - (pointer to) array of indexes of counters
*                         wrapped around
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The wraparound indexes are cleared on read.
*       Since in the HW each pair of wraparound indexes resides in one register,
*       cleared by read, in case not all the 8 possible indexes are read at once,
*       some data may be lost.
*       Recommendation: Always request 8 indexes => *indexNumPtr=8 as input.
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupCounterWraparoundIndexesGet
(
    IN    GT_U8                   devNum,
    IN     GT_PORT_GROUPS_BMP     portGroupsBmp,
    IN    GT_U32                  blockNum,
    INOUT GT_U32                  *indexNumPtr,
    OUT   GT_U32                  portGroupIdArr[],
    OUT   GT_U32                  indexesArr[]
)
{
    GT_STATUS rc;               /* return code       */
    GT_U32    regAddr;          /* register address  */
    GT_U32    data;             /* HW data           */
    GT_U32    i;                /* loop index        */
    GT_U32    maxIndexes;       /* index  array size */
    GT_U32    portGroupId;      /*the port group Id  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    PRV_CPSS_DXCH_CNC_BLOCK_INDEX_VALID_CHECK_MAC(devNum, blockNum);
    CPSS_NULL_PTR_CHECK_MAC(indexNumPtr);
    CPSS_NULL_PTR_CHECK_MAC(indexesArr);

    if (*indexNumPtr == 0)
    {
        return GT_OK;
    }

    maxIndexes = *indexNumPtr;

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.blockWraparoundStatusAddrBase
        + (CNC_BLOCK_CLIENT_CFG_ADDR_INCREMENT_CNS * blockNum);

    *indexNumPtr = 0;

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        for (i = 0; (i < 4); i++)
        {
            rc = prvCpssDrvHwPpPortGroupReadRegister(
                devNum, portGroupId, (regAddr + (i * 4)), &data);
            if (rc != GT_OK)
            {
                return rc;
            }

            if (data & 1)
            {
                indexesArr[*indexNumPtr] = (data >> 1) & 0x0FFF;
                if (portGroupIdArr != NULL)
                {
                    portGroupIdArr[*indexNumPtr] = portGroupId;
                }

                (*indexNumPtr) ++;

                if (*indexNumPtr == maxIndexes)
                {
                    return GT_OK;
                }
            }

            if ((data >> 16) & 1)
            {
                indexesArr[*indexNumPtr] = (data >> 17) & 0x0FFF;
                if (portGroupIdArr != NULL)
                {
                    portGroupIdArr[*indexNumPtr] = portGroupId;
                }

                (*indexNumPtr) ++;

                if (*indexNumPtr == maxIndexes)
                {
                    return GT_OK;
                }
            }
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncPortGroupCounterSet
*
* DESCRIPTION:
*   The function sets the counter contents
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       index            - counter index in the block
*                          valid range see in datasheet of specific device.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*       counterPtr       - (pointer to) counter contents
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupCounterSet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            blockNum,
    IN  GT_U32                            index,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    IN  CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
)
{
    GT_U32    regAddr;          /* register address           */

    /* other parameters checked in called function */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_CNC_BLOCK_INDEX_VALID_CHECK_MAC(devNum, blockNum);
    PRV_CPSS_DXCH_CNC_COUNTER_INDEX_VALID_CHECK_MAC(devNum, index);

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.blockCountersAddrBase
        + (blockNum * CNC_COUNTER_BLOCK_ADDR_INCREMENT_CNS) + (index * 8);

    return prvCpssDxChCncPortGroupCounterValueAverageSet(
        devNum, portGroupsBmp,
        regAddr, format, counterPtr);
}

/*******************************************************************************
* cpssDxChCncPortGroupCounterGet
*
* DESCRIPTION:
*   The function gets the counter contents
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       index            - counter index in the block
*                          valid range see in datasheet of specific device.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*
* OUTPUTS:
*       counterPtr       - (pointer to) received CNC counter value.
*                          For multi port group devices
*                          the result counter contains the sum of
*                          counters read from all port groups.
*                          It contains more bits than in HW.
*                          For example the sum of 4 35-bit values may be
*                          37-bit value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupCounterGet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            blockNum,
    IN  GT_U32                            index,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    OUT CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
)
{
    GT_U32    regAddr;          /* register address  */

    /* other parameters checked in called function */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_CNC_BLOCK_INDEX_VALID_CHECK_MAC(devNum, blockNum);
    PRV_CPSS_DXCH_CNC_COUNTER_INDEX_VALID_CHECK_MAC(devNum, index);

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.blockCountersAddrBase
        + (blockNum * CNC_COUNTER_BLOCK_ADDR_INCREMENT_CNS) + (index * 8);

    return prvCpssDxChCncPortGroupCounterValueSummingGet(
        devNum, portGroupsBmp,
        regAddr, format, counterPtr);
}

/*******************************************************************************
* cpssDxChCncPortGroupBlockUploadTrigger
*
* DESCRIPTION:
*   The function triggers the Upload of the given counters block.
*   The function checks that there is no unfinished CNC and FDB upload (FU).
*   Also the function checks that all FU messages of previous FU were retrieved
*   by cpssDxChBrgFdbFuMsgBlockGet.
*   An application may check that CNC upload finished by
*   cpssDxChCncPortGroupBlockUploadInProcessGet.
*   An application may sequentially upload several CNC blocks before start
*   to retrieve uploaded counters.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE   - if the previous CNC upload or FU in process or
*                        FU is finished but all FU messages were not
*                        retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by cpssDxChCncPortGroupUploadedBlockGet.
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupBlockUploadTrigger
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  GT_U32                      blockNum
)
{
    GT_STATUS              rc;               /* return code                       */
    GT_U32                 regAddr;          /* register address                  */
    GT_U32                 data;             /* field value                       */
    GT_BOOL                actionCompleted;  /* Action Trigger Flag               */
    GT_BOOL                uploadEnable;     /* FU upload Enable                  */
    CPSS_FDB_ACTION_MODE_ENT    mode;        /* FDB action mode                   */
    PRV_CPSS_AU_DESC_STC   *descPtr;         /*pointer to the current descriptor  */
    PRV_CPSS_AU_DESC_CTRL_STC   *descCtrlPtr;/* pointer to the descriptors DB
                                                 of the device                    */
    PRV_CPSS_AU_DESC_STC        *descBlockPtr;/* AU descriptors block             */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    PRV_CPSS_DXCH_CNC_BLOCK_INDEX_VALID_CHECK_MAC(devNum, blockNum);
    PRV_CNC_BLOCK_UPLOAD_SUPPORT_CHECK_MAC(devNum);

    /* the feartue needs separated AU and FU message queues */
    if (PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.fuqUseSeparate == GT_FALSE)
    {
        return GT_BAD_STATE;
    }

    /* check that there is not not-finished FDB upload */
    rc = cpssDxChBrgFdbTrigActionStatusGet(devNum, &actionCompleted);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChBrgFdbUploadEnableGet(devNum, &uploadEnable);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChBrgFdbActionModeGet(devNum, &mode);
    if (rc != GT_OK)
    {
        return rc;
    }

    if ((uploadEnable == GT_TRUE) && (actionCompleted == GT_FALSE)
        && (mode == CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E))
    {
        return GT_BAD_STATE;
    }

    /* loop on all port groups to check the configuration */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        /* used separate FU queue if enabled */
        descCtrlPtr = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[portGroupId]);

        /* check there are no FU messages in the queue */
        if (descCtrlPtr->unreadCncCounters == 0)
        {
            /* check that the FUQ initialized */
            PRV_CPSS_FUQ_DESCRIPTORS_INIT_DONE_CHECK_MAC(devNum);

            /* get address of AU descriptors block */
            descBlockPtr = (PRV_CPSS_AU_DESC_STC*)(descCtrlPtr->blockAddr);
            /* pointer to the current "retrieved" descriptor */
            descPtr = &(descBlockPtr[descCtrlPtr->currDescIdx]);
            /* found valid FU message in the queue */
            if (AU_DESC_IS_NOT_VALID(descPtr) != GT_TRUE)
            {
                return GT_BAD_STATE;
            }
        }

        /* check there are no CNC blocks yet being uploaded */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.blockUploadTriggers;
        rc = prvCpssDrvHwPpPortGroupReadRegister(
            devNum, portGroupId, regAddr, &data);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* triggers/busyStates for all 10 CNC 2048-counter blocks */
        if ((data & 0x03FF) != 0)
        {
            return GT_BAD_STATE;
        }

    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    /* loop on all port groups to trigger the upload */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        /* Must not reset SelCNCOrFU for xCat2 and above */
        if (!PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
        {
            /* set CNC to be the Queue owner */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.globalControl;
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 14, 1, 0);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        /* increase amount of queued counters */
        PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[portGroupId].unreadCncCounters +=
            PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.cncBlockNumEntries;

        /* trigger CNC counters upload */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.blockUploadTriggers;
        rc = prvCpssDrvHwPpPortGroupSetRegField(
            devNum, portGroupId,regAddr,
            blockNum/*fieldOffset*/, 1/*fieldLength*/, 1);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncPortGroupBlockUploadInProcessGet
*
* DESCRIPTION:
*   The function gets bitmap of numbers of such counters blocks that upload
*   of them yet in process. The HW cannot keep more then one block in such
*   state, but an API matches the original HW representation of the state.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          Checks all active port groups of the bitmap.
*
* OUTPUTS:
*       inProcessBlocksBmpPtr     - (pointer to) bitmap of in-process blocks
*                                   value 1 of the bit#n means that CNC upload
*                                   is not finished yet for block n
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupBlockUploadInProcessGet
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    OUT GT_U32                      *inProcessBlocksBmpPtr
)
{
    GT_STATUS   rc;               /* return code       */
    GT_U32      regAddr;          /* register address  */
    GT_U32      regData;          /* register data     */
    /* support multi-port-groups device */
    GT_U32      portGroupId;      /* the port group Id */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(inProcessBlocksBmpPtr);
    PRV_CNC_BLOCK_UPLOAD_SUPPORT_CHECK_MAC(devNum);

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.blockUploadTriggers;

    *inProcessBlocksBmpPtr = 0x0;

    /* loop on all port groups to set the same configuration */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        rc = prvCpssDrvHwPpPortGroupGetRegField(
            devNum, portGroupId, regAddr,
            0/*fieldOffset*/, 12/*fieldLength*/,
            &regData);
        if (rc != GT_OK)
        {
            return rc;
        }
        *inProcessBlocksBmpPtr |= regData;
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncPortGroupUploadedBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of CNC counter values,
*       the maximal number of elements defined by the caller.
*       The CNC upload may triggered by cpssDxChCncPortGroupBlockUploadTrigger.
*       The CNC upload transfers whole CNC block (2K CNC counters)
*       to FDB Upload queue. An application must get all transferred counters.
*       An application may sequentially upload several CNC blocks before start
*       to retrieve uploaded counters.
*       The device may transfer only part of CNC block because of FUQ full. In
*       this case the cpssDxChCncPortGroupUploadedBlockGet may return only part of the
*       CNC block with return GT_OK. An application must to call
*       cpssDxChCncPortGroupUploadedBlockGet one more time to get rest of the block.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum                - the device number from which FU are taken
*       portGroupsBmp         - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       numOfCounterValuesPtr - (pointer to) maximal number of CNC counters
*                               values to get.This is the size of
*                               counterValuesPtr array allocated by caller.
*       format                - CNC counter HW format,
*                               relevant only for Lion and above
*
* OUTPUTS:
*       numOfCounterValuesPtr - (pointer to) actual number of CNC
*                               counters values in counterValuesPtr.
*       counterValuesPtr      - (pointer to) array that holds received CNC
*                               counters values. Array must be allocated by
*                               caller. For multi port group devices
*                               the result counters contain the sum of
*                               counters read from all port groups.
*                               It contains more bits than in HW.
*                               For example the sum of 4 35-bit values may be
*                               37-bit value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE       - the action succeeded and there are no more waiting
*                          CNC counter value
*       GT_FAIL                  - on failure
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - if the previous FU messages were not
*                          retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by cpssDxChCncPortGroupUploadedBlockGet.
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupUploadedBlockGet
(
    IN     GT_U8                             devNum,
    IN     GT_PORT_GROUPS_BMP                portGroupsBmp,
    INOUT  GT_U32                            *numOfCounterValuesPtr,
    IN     CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    OUT    CPSS_DXCH_CNC_COUNTER_STC         *counterValuesPtr
)
{
    GT_STATUS   rc; /* return code */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32  portGroupNumOfCounters;/* number of counters requested and actually */
                                   /* retrieved for the current port group. */
    GT_U32  minNumOfCounters;      /* the minimum number of counters read per port */
                                   /* group from all port groups.   */
    GT_BOOL firstPortGroup;        /* is first Port Group the first */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(numOfCounterValuesPtr);
    PRV_CNC_COUNTER_FORMAT_CHECK_MAC(devNum, format);
    CPSS_NULL_PTR_CHECK_MAC(counterValuesPtr);
    PRV_CNC_BLOCK_UPLOAD_SUPPORT_CHECK_MAC(devNum);

    if (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        rc = GT_OK;

        minNumOfCounters = *numOfCounterValuesPtr;

        firstPortGroup = GT_TRUE;

        /* sum the counters from all the port groups */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)
        {
            portGroupNumOfCounters = *numOfCounterValuesPtr;
            /* all port groups have the same number of counters */
            rc = cncUploadedBlockGet(
                devNum, portGroupId, firstPortGroup /*initOutCounters*/,
                format, &portGroupNumOfCounters, counterValuesPtr);

            if( portGroupNumOfCounters < minNumOfCounters )
            {
                minNumOfCounters = portGroupNumOfCounters;
            }

            if( GT_OK == rc || GT_NO_MORE == rc )
            {
                /* continute to next port group in case get number of requested counters */
                /* successfuly or this port group has no more counters */
            }
            else
            {
                return rc;
            }

            firstPortGroup = GT_FALSE;
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)

        *numOfCounterValuesPtr = minNumOfCounters;

        return rc;
    }

    /* non multi-port-groups device */
    return cncUploadedBlockGet(
        devNum, CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS,
        GT_TRUE /*initOutCounters*/, format,
        numOfCounterValuesPtr,counterValuesPtr);
}

/*******************************************************************************
* cpssDxChCncPortGroupCounterFormatSet
*
* DESCRIPTION:
*   The function sets format of CNC counter
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*       format          - CNC counter format
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupCounterFormatSet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            blockNum,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format
)
{
    GT_STATUS rc;            /* return code          */
    GT_U32    regAddr;       /* register address     */
    GT_U32    data;          /* HW data              */
    GT_U32    portGroupId;   /*the port group Id     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    PRV_CPSS_DXCH_CNC_BLOCK_INDEX_VALID_CHECK_MAC(devNum, blockNum);

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.blockClientCfgAddrBase
        + (blockNum * 0x100);

    switch (format)
    {
        case CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E:
            data = 0;
            break;
        case CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_1_E:
            data = 1;
            break;
        case CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E:
            data = 2;
            break;
        default: return GT_BAD_PARAM;
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        rc = prvCpssDrvHwPpPortGroupSetRegField(
            devNum, portGroupId, regAddr,
            12/*offset*/, 2/*length*/, data);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCncPortGroupCounterFormatGet
*
* DESCRIPTION:
*   The function gets format of CNC counter
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*
* OUTPUTS:
*       formatPtr       - (pointer to) CNC counter format
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*                                  on not supported client for device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupCounterFormatGet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            blockNum,
    OUT CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  *formatPtr
)
{
    GT_STATUS rc;            /* return code           */
    GT_U32    regAddr;       /* register address      */
    GT_U32    data;          /* HW data               */
    GT_U32    portGroupId;   /* the port group Id     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    PRV_CPSS_DXCH_CNC_BLOCK_INDEX_VALID_CHECK_MAC(devNum, blockNum);
    CPSS_NULL_PTR_CHECK_MAC(formatPtr);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cncRegs.blockClientCfgAddrBase
        + (blockNum * 0x100);

    rc = prvCpssDrvHwPpPortGroupGetRegField(
        devNum, portGroupId, regAddr,
        12/*offset*/, 2/*length*/, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (data)
    {
        case 0:
            *formatPtr = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E;
            break;
        case 1:
            *formatPtr = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_1_E;
            break;
        case 2:
            *formatPtr = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E;
            break;
        default: return GT_BAD_STATE;
    }

    return GT_OK;
}
