/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgCount.c
*
* DESCRIPTION:
*       CPSS DxCh Ingress Bridge Counters facility implementation.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1.5 $
*******************************************************************************/
/* get the device info and common info */

/* macro needed to support the call to PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC */
/* this define must come before include files zhangdi@autelan.com 2012-08-08*/
#define PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_SUPPORTED_FLAG_CNS

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/* get vid checking */
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>

#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgCount.h>

/* Add for PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC */
#include <cpss/generic/config/private/prvCpssConfigTypes.h>

/* Add for the define of BOARD_TYPE, zhangdi 2011-04-11 */
#if 0
#include <../../../../../npd/src/app/npd_board.h>
#else
#include <sysdef/npd_sysdef.h>   /* for board_info */
#endif
/* Add end */

/*******************************************************************************
* cpssDxChBrgCntDropCntrModeSet
*
* DESCRIPTION:
*       Sets Drop Counter Mode (configures a Bridge Drop Counter "reason").
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number.
*       dropMode  - Drop Counter mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum or dropMode
*       GT_NOT_SUPPORTED         - on non-supported dropMode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntDropCntrModeSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT dropMode
)
{
    GT_U32      regAddr;     /* register address */
    GT_U32      regValue;    /* register value */
    GT_STATUS   retStatus;   /* generic return status code */
    GT_U32      fieldLength; /* the number of bits to be written to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    if (dropMode > CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_NON_IPV4_BC_E ||
        dropMode < CPSS_DXCH_BRG_DROP_CNTR_COUNT_ALL_E)
    {
        return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.
                                         bridgeGlobalConfigRegArray[1];

    /*******************************************/
    /* Calculate HW value of Drop Counter Mode */
    /*******************************************/
    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
    {
        if (dropMode > CPSS_DXCH_BRG_DROP_CNTR_MOVED_STATIC_ADDR_E)
        {
            if (dropMode < CPSS_DXCH_BRG_DROP_CNTR_RATE_LIMIT_E)
            {
                return GT_NOT_SUPPORTED;
            }
            else
            {
                regValue = ((GT_U32)dropMode - 13);
            }
        }
        else
        {
            regValue = (GT_U32)dropMode;
        }

        fieldLength = 5;
    }
    else
    {
        if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
        {
            regValue = (GT_U32)dropMode;
            fieldLength = 6;
        }

        else
        {
            regValue = ((GT_U32)dropMode) & 0x1F;
            retStatus = prvCpssDrvHwPpSetRegField(devNum,
                                                  regAddr, 0, 1, (dropMode >> 5));
            if (GT_OK != retStatus)
            {
                return retStatus;
            }

            fieldLength = 5;
        }

    }

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 20, fieldLength, regValue);

}

/*******************************************************************************
* cpssDxChBrgCntDropCntrModeGet
*
* DESCRIPTION:
*       Gets the Drop Counter Mode (Bridge Drop Counter "reason").
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*
* OUTPUTS:
*       dropModePtr - pointer to the Drop Counter mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_SUPPORTED         - on non-supported dropMode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntDropCntrModeGet
(
    IN   GT_U8                                   devNum,
    OUT  CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT     *dropModePtr
)
{
    GT_U32      regAddr;     /* register address */
    GT_U32      regValue;    /* register value */
    GT_STATUS   retStatus;   /* generic return status code */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(dropModePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.
                                     bridgeGlobalConfigRegArray[1];

    retStatus = prvCpssDrvHwPpReadRegister(devNum, regAddr, &regValue);
    if (GT_OK != retStatus)
    {
        return retStatus;
    }

    /************************************/
    /* Gather Drop Counter Mode from HW */
    /************************************/
    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
    {
        regValue = (regValue >> 20) & 0x1F;

        if (regValue > 20)
        {
            retStatus = GT_NOT_SUPPORTED;
        }
        else
        {
            if (regValue > 7)
            {
                regValue = regValue + 13;
            }
        }

    }
    else
    {
        if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
        {
            regValue = (regValue >> 20) & 0x3F;
        }
        else
        {
            regValue = ((regValue >> 20) & 0x1F) | ((regValue & 0x1) << 5);
        }

        if (regValue > CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_NON_IPV4_BC_E)
        {
            retStatus = GT_NOT_SUPPORTED;
        }
    }

    /* In any case, the HW reg value will be stored in the *dropModePtr */
    *dropModePtr = (CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT)regValue;

    return retStatus;

}

/*******************************************************************************
* cpssDxChBrgCntMacDaSaSet
*
* DESCRIPTION:
*       Sets a specific MAC DA and SA to be monitored by Host
*       and Matrix counter groups on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       saAddrPtr   - source MAC address (MAC SA).
*       daAddrPtr   - destination MAC address (MAC DA).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntMacDaSaSet
(
    IN  GT_U8           devNum,
    IN  GT_ETHERADDR    *saAddrPtr,
    IN  GT_ETHERADDR    *daAddrPtr
)
{
    GT_U32      regAddr;     /* register address */
    GT_U32      regValue;    /* register value */
    GT_STATUS   retStatus;   /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(saAddrPtr);
    CPSS_NULL_PTR_CHECK_MAC(daAddrPtr);

    /* Set 4 LSB of MAC DA to the HW */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.brgMacCntr0;
    regValue = GT_HW_MAC_LOW32(daAddrPtr);
    retStatus = prvCpssDrvHwPpWriteRegister(devNum, regAddr, regValue);
    if (GT_OK != retStatus)
    {
        return retStatus;
    }

    /* Set 2 MSB of MAC DA and 2 LSB of MAC SA to the HW */
    regValue = GT_HW_MAC_HIGH16(daAddrPtr) | (GT_HW_MAC_LOW16(saAddrPtr) << 16);
    retStatus = prvCpssDrvHwPpWriteRegister(devNum, regAddr + 4, regValue);
    if (GT_OK == retStatus)
    {
        /* Set 4 MSB of MAC SA to the HW */
        regValue = GT_HW_MAC_HIGH32(saAddrPtr);
        retStatus = prvCpssDrvHwPpWriteRegister(devNum, regAddr + 8, regValue);
    }

    return retStatus;

}

/*******************************************************************************
* cpssDxChBrgCntMacDaSaGet
*
* DESCRIPTION:
*       Gets a MAC DA and SA are monitored by Host
*       and Matrix counter groups on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*
* OUTPUTS:
*       saAddrPtr   - source MAC address (MAC SA).
*       daAddrPtr   - destination MAC address (MAC DA).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntMacDaSaGet
(
    IN   GT_U8           devNum,
    OUT  GT_ETHERADDR    *saAddrPtr,
    OUT  GT_ETHERADDR    *daAddrPtr
)
{
    GT_U32      regAddr;     /* register address */
    GT_U32      regValue;    /* register value */
    GT_STATUS   retStatus;   /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(saAddrPtr);
    CPSS_NULL_PTR_CHECK_MAC(daAddrPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.brgMacCntr0;

    /* Get 4 LSB of MAC DA */
    retStatus = prvCpssDrvHwPpReadRegister(devNum, regAddr, &regValue);
    if (GT_OK != retStatus)
    {
        return retStatus;
    }
    daAddrPtr->arEther[5] = (GT_U8)U32_GET_FIELD_MAC(regValue, 0, 8);
    daAddrPtr->arEther[4] = (GT_U8)U32_GET_FIELD_MAC(regValue, 8, 8);
    daAddrPtr->arEther[3] = (GT_U8)U32_GET_FIELD_MAC(regValue, 16, 8);
    daAddrPtr->arEther[2] = (GT_U8)U32_GET_FIELD_MAC(regValue, 24, 8);

    /* Get 2 MSB of MAC DA and 2 LSB of MAC SA */
    retStatus = prvCpssDrvHwPpReadRegister(devNum, regAddr + 4, &regValue);
    if (GT_OK != retStatus)
    {
        return retStatus;
    }
    daAddrPtr->arEther[1] = (GT_U8)U32_GET_FIELD_MAC(regValue, 0, 8);
    daAddrPtr->arEther[0] = (GT_U8)U32_GET_FIELD_MAC(regValue, 8, 8);
    saAddrPtr->arEther[5] = (GT_U8)U32_GET_FIELD_MAC(regValue, 16, 8);
    saAddrPtr->arEther[4] = (GT_U8)U32_GET_FIELD_MAC(regValue, 24, 8);

    /* Get 4 MSB of MAC SA */
    retStatus = prvCpssDrvHwPpReadRegister(devNum, regAddr+ 8, &regValue);
    if (GT_OK == retStatus)
    {
        saAddrPtr->arEther[3] = (GT_U8)U32_GET_FIELD_MAC(regValue, 0, 8);
        saAddrPtr->arEther[2] = (GT_U8)U32_GET_FIELD_MAC(regValue, 8, 8);
        saAddrPtr->arEther[1] = (GT_U8)U32_GET_FIELD_MAC(regValue, 16, 8);
        saAddrPtr->arEther[0] = (GT_U8)U32_GET_FIELD_MAC(regValue, 24, 8);
    }

    return retStatus;

}

/*******************************************************************************
* cpssDxChBrgCntBridgeIngressCntrModeSet
*
* DESCRIPTION:
*       Configures a specified Set of Bridge Ingress
*       counters to work in requested mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number
*       cntrSetId - Counter Set ID
*       setMode   - count mode of specified Set of Bridge Ingress Counters.
*       port      - port number monitored by Set of Counters.
*                   This parameter is applied upon CPSS_BRG_CNT_MODE_1_E and
*                   CPSS_BRG_CNT_MODE_3_E counter modes.
*       vlan      - VLAN ID monitored by Counters Set.
*                   This parameter is applied upon CPSS_BRG_CNT_MODE_2_E and
*                   CPSS_BRG_CNT_MODE_3_E counter modes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum, setMode or cntrSetId.
*       GT_OUT_OF_RANGE          - on port number bigger then 63.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntBridgeIngressCntrModeSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT    cntrSetId,
    IN  CPSS_BRIDGE_INGR_CNTR_MODES_ENT     setMode,
    IN  GT_U8                               port,
    IN  GT_U16                              vlan
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;     /* register address */
    GT_U32      regValue;    /* register value */
    GT_U32      mode;        /* local count mode value */
    GT_U32  portGroupId,portPortGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);


    /* Check Counter Set ID */
    switch(cntrSetId)
    {
        case CPSS_DXCH_BRG_CNT_SET_ID_0_E:
        case CPSS_DXCH_BRG_CNT_SET_ID_1_E:
            break;
        default:
            return GT_BAD_PARAM;
    }

    /************************************************/
    /* Calculate Counter Set Configuration HW value */
    /************************************************/
    switch (setMode)
    {
        case CPSS_BRG_CNT_MODE_0_E:
            mode = 0;
            port = 0;
            vlan = 0;
            break;
        case CPSS_BRG_CNT_MODE_1_E:
            if(port >= BIT_6)
            {
                return GT_OUT_OF_RANGE;
            }
            mode = 1;
            vlan = 0;
            break;
        case CPSS_BRG_CNT_MODE_2_E:
            PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlan);
            mode = 2;
            port = 0;
            break;
        case CPSS_BRG_CNT_MODE_3_E:
            PRV_CPSS_VLAN_VALUE_CHECK_MAC(vlan);
            if(port >= BIT_6)
            {
                return GT_OUT_OF_RANGE;
            }
            mode = 3;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.
                                         brgCntrSet[cntrSetId].cntrSetCfg;

    if(mode & 1)
    {
        /* hold 'port filter' */

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portPortGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);
        /* loop on all port groups :
            on the port group that 'own' the port , set the needed configuration
            on other port groups put 'NULL port'
        */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

        {
            if(portPortGroupId == portGroupId)
            {
                localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,port);
            }
            else
            {
                localPort = PRV_CPSS_DXCH_NULL_PORT_NUM_CNS;
            }

            regValue = mode | (localPort << 2) | (vlan << 8);

            rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr,regValue);

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
        portPortGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
        regValue = mode | (port << 2) | (vlan << 8);

        rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, regValue);
    }

    if(rc == GT_OK)
    {
        /* save the info for 'get' configuration and 'read' counters */
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.
                bridgeIngressCntrMode[cntrSetId].portGroupId = portPortGroupId;
    }

    return rc;

}

/*******************************************************************************
* cpssDxChBrgCntBridgeIngressCntrModeGet
*
* DESCRIPTION:
*       Gets the mode (port number and VLAN Id as well) of specified
*       Bridge Ingress counters Set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number
*       cntrSetId - Counter Set ID
*
* OUTPUTS:
*       setModePtr   - pointer to the count mode of specified Set of Bridge
*                      Ingress Counters.
*       portPtr      - pointer to the port number monitored by Set of Counters.
*                      This parameter is applied upon CPSS_BRG_CNT_MODE_1_E and
*                      CPSS_BRG_CNT_MODE_3_E counter modes.
*                      This parameter can be NULL;
*       vlanPtr      - pointer to the VLAN ID monitored by Counters Set.
*                      This parameter is applied upon CPSS_BRG_CNT_MODE_2_E and
*                      CPSS_BRG_CNT_MODE_3_E counter modes.
*                      This parameter can be NULL;
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum or setMode
*       GT_OUT_OF_RANGE - on port number bigger then 63 or counter set number
*                         that is out of range of [0-1].
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntBridgeIngressCntrModeGet
(
    IN   GT_U8                               devNum,
    IN   CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT    cntrSetId,
    OUT  CPSS_BRIDGE_INGR_CNTR_MODES_ENT     *setModePtr,
    OUT  GT_U8                               *portPtr,
    OUT  GT_U16                              *vlanPtr
)
{
    GT_U32      regAddr;     /* register address */
    GT_U32      regValue;    /* register value */
    GT_U32      mode;        /* local count mode value */
    GT_STATUS   retStatus;   /* generic return status code */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(setModePtr);

    /* Check Counter Set ID */
    switch(cntrSetId)
    {
        case CPSS_DXCH_BRG_CNT_SET_ID_0_E:
        case CPSS_DXCH_BRG_CNT_SET_ID_1_E:
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* get the info from DB */
    portGroupId = PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.
                bridgeIngressCntrMode[cntrSetId].portGroupId;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.
                                         brgCntrSet[cntrSetId].cntrSetCfg;
    retStatus = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,regAddr, &regValue);
    if (GT_OK != retStatus)
    {
        return retStatus;
    }

    /* Gather count mode of specified Bridge Ingress Counters Set */
    mode = regValue & 0x3;
    switch (mode)
    {
        case 0:
            *setModePtr = CPSS_BRG_CNT_MODE_0_E;
            break;
        case 1:
            *setModePtr = CPSS_BRG_CNT_MODE_1_E;
            break;
        case 2:
            *setModePtr = CPSS_BRG_CNT_MODE_2_E;
            break;
        case 3:
            *setModePtr = CPSS_BRG_CNT_MODE_3_E;
            break;
        default:
            return GT_FAIL;
    }

    /* Gather port number monitored by specified Bridge Ingress Counters Set */
    if (portPtr != NULL)
    {
        *portPtr = (GT_U8)((regValue >> 2) & 0x3F);
        if(portGroupId != CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
        {
            /* support multi-port-groups device , convert local port to global port */
            *portPtr = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum,portGroupId,(*portPtr));
        }
    }

    /* Gather VLAN ID  monitored by specified Bridge Ingress Counters Set */
    if (vlanPtr != NULL)
    {
        *vlanPtr = (GT_U16)((regValue >> 8) & 0xFFF);
    }

    return retStatus;
}

/*******************************************************************************
* cpssDxChBrgCntDropCntrGet
*
* DESCRIPTION:
*       Gets the Bridge Ingress Drop Counter of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*
* OUTPUTS:
*       dropCntPtr  - pointer to the number of packets that were dropped
*                     due to drop reason configured
*                     by the cpssDxChBrgCntDropCntrModeSet().
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntDropCntrGet
(
    IN   GT_U8      devNum,
    OUT  GT_U32     *dropCntPtr
)
{
    return cpssDxChBrgCntPortGroupDropCntrGet(devNum,
                                              CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                              dropCntPtr);
}

/*******************************************************************************
* cpssDxChBrgCntDropCntrSet
*
* DESCRIPTION:
*       Sets the Bridge Ingress Drop Counter of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number.
*       dropCnt  - number of packets that were dropped due to drop reason
*                  configured by the cpssDxChBrgCntDropCntrModeSet().
*                  The parameter enables an application to initialize the counter,
*                  for the desired counter value.
*                  Range: (0..0xFFFF).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM - on wrong devNum or invalid Drop Counter Value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntDropCntrSet
(
    IN  GT_U8      devNum,
    IN  GT_U32     dropCnt
)
{
    return cpssDxChBrgCntPortGroupDropCntrSet(devNum,
                                              CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                              dropCnt);
}

/*******************************************************************************
* cpssDxChBrgCntHostGroupCntrsGet
*
* DESCRIPTION:
*       Gets Bridge Host group counters value of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       hostGroupCntPtr - structure with current counters value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Bridge Host group of counters are clear-on-read.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntHostGroupCntrsGet
(
    IN   GT_U8                              devNum,
    OUT  CPSS_DXCH_BRIDGE_HOST_CNTR_STC     *hostGroupCntPtr
)
{
    return cpssDxChBrgCntPortGroupHostGroupCntrsGet(devNum,
                                              CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    hostGroupCntPtr);
}

/*******************************************************************************
* cpssDxChBrgCntMatrixGroupCntrsGet
*
* DESCRIPTION:
*       Gets Bridge Matrix counter value of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                - physical device number.
*
* OUTPUTS:
*       matrixCntSaDaPktsPtr  - number of packets (good only) with a MAC SA/DA
*                               matching of the CPU-configured MAC SA/DA.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Bridge Matrix counter is clear-on-read.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntMatrixGroupCntrsGet
(
    IN   GT_U8      devNum,
    OUT  GT_U32     *matrixCntSaDaPktsPtr
)
{
    return cpssDxChBrgCntPortGroupMatrixGroupCntrsGet(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                      matrixCntSaDaPktsPtr);
}

/*******************************************************************************
* cpssDxChBrgCntBridgeIngressCntrsGet
*
* DESCRIPTION:
*       Gets a Bridge ingress Port/VLAN/Device counters from
*       specified counter set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number.
*       cntrSetId       - counter set number.
*
* OUTPUTS:
*       ingressCntrPtr  - structure of bridge ingress counters current values.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum, on counter set number
*                         that is out of range of [0-1]
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Bridge Ingress group of counters are clear-on-read.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntBridgeIngressCntrsGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT    cntrSetId,
    OUT CPSS_BRIDGE_INGRESS_CNTR_STC        *ingressCntrPtr
)
{
    return cpssDxChBrgCntPortGroupBridgeIngressCntrsGet(devNum,
                                              CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                      cntrSetId,ingressCntrPtr);
}

/*******************************************************************************
* cpssDxChBrgCntLearnedEntryDiscGet
*
* DESCRIPTION:
*       Gets the total number of source addresses the were
*       not learned due to bridge internal congestion.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       countValuePtr   - The value of the counter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hw error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Learned Entry Discards Counter is clear-on-read.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntLearnedEntryDiscGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *countValuePtr
)
{
    return cpssDxChBrgCntPortGroupLearnedEntryDiscGet(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                      countValuePtr);
}

/*******************************************************************************
* cpssDxChBrgCntPortGroupDropCntrGet
*
* DESCRIPTION:
*       Gets the Bridge Ingress Drop Counter of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portGroupsBmp  - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       dropCntPtr  - pointer to the number of packets that were dropped
*                     due to drop reason configured
*                     by the cpssDxChBrgCntDropCntrModeSet().
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntPortGroupDropCntrGet
(
    IN   GT_U8               devNum,
    IN   GT_PORT_GROUPS_BMP  portGroupsBmp,
    OUT  GT_U32              *dropCntPtr
)
{
    GT_U32      regAddr;     /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(dropCntPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    /************************************/
    /* Read Bridge Ingress Drop Counter */
    /************************************/
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.dropIngrCntr;

    return prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                  regAddr, 0, 16,
                                                  dropCntPtr, NULL);
}

/*******************************************************************************
* cpssDxChBrgCntPortGroupDropCntrSet
*
* DESCRIPTION:
*       Sets the Bridge Ingress Drop Counter of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number.
*       portGroupsBmp  - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       dropCnt  - number of packets that were dropped due to drop reason
*                  configured by the cpssDxChBrgCntDropCntrModeSet().
*                  The parameter enables an application to initialize the counter,
*                  for the desired counter value.
*                  Range: (0..0xFFFF).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM - on wrong devNum or invalid Drop Counter Value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntPortGroupDropCntrSet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_GROUPS_BMP   portGroupsBmp,
    IN  GT_U32               dropCnt
)
{
    GT_U32      regAddr;     /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    if (dropCnt >= BIT_16)
    {
        /* 16 bits in HW */
        return GT_BAD_PARAM;
    }
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    /************************************/
    /* Set Bridge Ingress Drop Counter */
    /************************************/
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.dropIngrCntr;
    return prvCpssDxChPortGroupsBmpCounterSet(devNum, portGroupsBmp,
                                              regAddr, 0, 16, dropCnt);
}

/*******************************************************************************
* cpssDxChBrgCntPortGroupHostGroupCntrsGet
*
* DESCRIPTION:
*       Gets Bridge Host group counters value of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - physical device number.
*       portGroupsBmp  - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       hostGroupCntPtr - structure with current counters value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Bridge Host group of counters are clear-on-read.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntPortGroupHostGroupCntrsGet
(
    IN   GT_U8                              devNum,
    IN   GT_PORT_GROUPS_BMP                 portGroupsBmp,
    OUT  CPSS_DXCH_BRIDGE_HOST_CNTR_STC     *hostGroupCntPtr
)
{
    GT_U32      regAddr;     /* register address */
    GT_STATUS   retStatus;   /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(hostGroupCntPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    /************************************************/
    /* Read value of Host Incoming Packets Counter. */
    /************************************************/
    regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.hostInPckt;
    retStatus = prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                       regAddr, 0, 32,
                                              &(hostGroupCntPtr->gtHostInPkts),
                                                       NULL);
    if (GT_OK != retStatus)
    {
        return retStatus;
    }

    /************************************************/
    /* Read value of Host Outgoing Packets Counter. */
    /************************************************/
    regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.hostOutPckt;
    retStatus = prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                       regAddr, 0, 32,
                                           &(hostGroupCntPtr->gtHostOutPkts),
                                                       NULL);
    if (GT_OK != retStatus)
    {
        return retStatus;
    }

    /**********************************************************/
    /* Read value of Host Outgoing Multicast Packets Counter. */
    /**********************************************************/
    regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.hostOutMcPckt;
    retStatus = prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                       regAddr, 0,  32,
                                  &(hostGroupCntPtr->gtHostOutMulticastPkts),
                                                       NULL);
    if (GT_OK != retStatus)
    {
        return retStatus;
    }

    /**********************************************************/
    /* Read value of Host Outgoing Broadcast Packets Counter. */
    /**********************************************************/
    regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.hostOutBrdPckt;
    return prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                  regAddr, 0, 32,
                                  &(hostGroupCntPtr->gtHostOutBroadcastPkts),
                                                  NULL);

}

/*******************************************************************************
* cpssDxChBrgCntPortGroupMatrixGroupCntrsGet
*
* DESCRIPTION:
*       Gets Bridge Matrix counter value of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - physical device number.
*       portGroupsBmp  - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       matrixCntSaDaPktsPtr  - number of packets (good only) with a MAC SA/DA
*                               matching of the CPU-configured MAC SA/DA.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Bridge Matrix counter is clear-on-read.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntPortGroupMatrixGroupCntrsGet
(
    IN   GT_U8                     devNum,
    IN   GT_PORT_GROUPS_BMP        portGroupsBmp,
    OUT  GT_U32                    *matrixCntSaDaPktsPtr
)
{
    GT_U32      regAddr;     /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(matrixCntSaDaPktsPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    /************************************/
    /* Read Matrix SA/DA Packet Counter */
    /************************************/
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.matrixPckt;
    return prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                  regAddr, 0, 32,
                                                  matrixCntSaDaPktsPtr, NULL);

}

/*******************************************************************************
* cpssDxChBrgCntPortGroupBridgeIngressCntrsGet
*
* DESCRIPTION:
*       Gets a Bridge ingress Port/VLAN/Device counters from
*       specified counter set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number.
*       portGroupsBmp   - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       cntrSetId       - counter set number.
*
* OUTPUTS:
*       ingressCntrPtr  - structure of bridge ingress counters current values.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum, on counter set number
*                         that is out of range of [0-1]
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Bridge Ingress group of counters are clear-on-read.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntPortGroupBridgeIngressCntrsGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT    cntrSetId,
    OUT CPSS_BRIDGE_INGRESS_CNTR_STC        *ingressCntrPtr
)
{
    GT_U32      regAddr;     /* register address */
    GT_STATUS   retStatus;   /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(ingressCntrPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    /* Check Counter Set ID */
    switch(cntrSetId)
    {
        case CPSS_DXCH_BRG_CNT_SET_ID_0_E:
        case CPSS_DXCH_BRG_CNT_SET_ID_1_E:
            break;
        default:
            return GT_BAD_PARAM;
    }

    /***********************************************/
    /* Read value of Bridge Ingress Frames Counter */
    /***********************************************/
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.
                                         brgCntrSet[cntrSetId].inPckt;
    retStatus = prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                       regAddr, 0, 32,
                                          &ingressCntrPtr->gtBrgInFrames, NULL);
    if (GT_OK != retStatus)
    {
        return retStatus;
    }

    /******************************************************/
    /* Read value of VLAN Ingress Filtered Packet Counter */
    /******************************************************/
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.
                                         brgCntrSet[cntrSetId].inFltPckt;
    retStatus = prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                    regAddr, 0, 32,
                                 &ingressCntrPtr->gtBrgVlanIngFilterDisc, NULL);
    if (GT_OK != retStatus)
    {
        return retStatus;
    }

    /**************************************************/
    /* Read value of Security Filtered Packet Counter */
    /**************************************************/
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.
                                         brgCntrSet[cntrSetId].secFltPckt;
    retStatus = prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                    regAddr, 0, 32,
                                  &ingressCntrPtr->gtBrgSecFilterDisc, NULL);
    if (GT_OK != retStatus)
    {
        return retStatus;
    }

    /************************************************/
    /* Read value of Bridge Filtered Packet Counter */
    /************************************************/

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.
                                         brgCntrSet[cntrSetId].brgFltPckt;
    retStatus = prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                    regAddr, 0, 32,
                                     &ingressCntrPtr->gtBrgLocalPropDisc, NULL);

    return retStatus;

}

/*******************************************************************************
* cpssDxChBrgCntPortGroupLearnedEntryDiscGet
*
* DESCRIPTION:
*       Gets the total number of source addresses the were
*       not learned due to bridge internal congestion.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number.
*       portGroupsBmp   - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       countValuePtr   - The value of the counter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hw error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Learned Entry Discards Counter is clear-on-read.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntPortGroupLearnedEntryDiscGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    OUT GT_U32                  *countValuePtr
)
{
    GT_U32 regAddr;     /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(countValuePtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);


    /* Read value of Learned Entry Discards Counter */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.learnedDisc;
    return prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                  regAddr, 0, 32,
                                                  countValuePtr, NULL);
}


#ifndef __AX_PLATFORM__
/*******************************************************************************
* cpssDxChBrgMIBRead
*
* DESCRIPTION:
*       Gets all the MIB counter value for XG ports of the specified device
*
* APPLICABLE DEVICES: ALL DxCh Devices.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hw error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       Learned Entry Discards Counter is clear-on-read.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgMIBRead
(
)
{
	#define PRINTSTAR "***********************"
	GT_U8  devNumArr[] = { 			\
		0,/* 98Dx275 */ 			\
		1 /* 98Dx804 */
	};/* device number */
	
	char *devDesc[] = {	\
		"98Dx275",					\
		"98Dx804"					\
	};
	
    long regAddrArr[] = {									\
		0x01C80010,/* Port<26> Rx Ucast Pkt */				\
		0x01C80018,/* Port<26> Rx Bcast Pkt */					\
		0x01C8001C,/* Port<26> Rx Mcast Pkt */				\
		0x01C80070,/* Port<26> Rx Error  Pkt */				\
		0x01C80074,/* Port<26> BadCRC  */					\
		0x01C8005C,/* Port<26> ReceivedFIFO overrun */			\
		0x01C80060,/* Port<26> Undersize */					\
		0x01C80064,/* Port<26> Fragments */					\
		0x01C80068,/* Port<26> Oversize */					\
		0x01C8006C,/* Port<26> Jabber */						\
		0x01C80040,/* Port<26> Tx Ucast Pkt */					\
		0x01C8004C,/* Port<26> Tx Bcast Pkt */					\
		0x01C80048,/* Port<26> Tx Mcast Pkt */				\
		0x01C8000C,/* Port<26> Tx CRC Error */				\
		0x01DC0010,/* Port<27> Rx Ucast Pkt */				\
		0x01DC0018,/* Port<27> Rx Bcast Pkt */					\
		0x01DC001C,/* Port<27> Rx Mcast Pkt */				\
		0x01DC0070,/* Port<27> Rx Error  Pkt */				\
		0x01DC0074,/* Port<27> BadCRC  */					\
		0x01DC005C,/* Port<27> ReceivedFIFO overrun */			\
		0x01DC0060,/* Port<27> Undersize */					\
		0x01DC0064,/* Port<27> Fragments */					\
		0x01DC0068,/* Port<27> Oversize */					\
		0x01DC006C,/* Port<27> Jabber */						\
		0x01DC0040,/* Port<27> Tx Ucast Pkt */					\
		0x01DC004C,/* Port<27> Tx Bcast Pkt */					\
		0x01DC0048,/* Port<27> Tx Mcast Pkt */				\
		0x01DC000C,/* Port<27> Tx CRC Error */				\
		0x020400BC,/* Host Incoming Packets Count */ 			\
		0x020400C0,/* Host Outgoing Packets Count */ 			\
		0x020400D0,/* Host Outgoing Broadcast Packet Count */ 	\
		0x020400CC,/* Host Outgoing Multicase Packet Count */  	\
		0x020400D4,/* Matrix Source/Destination Packet Count */	\
		0x020400E0,/* Set0 Incoming Packet Count */  			\
		0x020400E4,/* Set0 VLAN Ingress Filtered Packet Count */	\
		0x020400E8,/* Set0 Security Filtered Packet Count */	  	\
		0x020400EC,/* Set0 Bridge Filtered Packet Count */      	\
		0x020400F4,/* Set1 Incoming Packet Count */  			\
		0x020400F8,/* Set1 VLAN Ingress Filtered Packet Count */	\
		0x020400FC,/* Set1 Security Filtered Packet Count */	  	\
		0x02040100,/* Set1 Bridge Filtered Packet Count */      	\
		0x02040150,/* Bridge Filter Counter */					\
		0x02040104,/* Global Security Breach Filter Counter */ 		\
		0x02040108,/* Port/VLAN Security Breach Drop Counter */	\
		0x01B40144,/* Set0 Outgoing Unicast Packet Counter */ 	\
		0x01B40148,/* Set0 Outgoing Multicast Packet Counter */	\
		0x01B4014C,/* Set0 Outgoing Broadcast Packet Counter */	\
		0x01B40150,/* Set0 Bridge Egress Filtered Packet Count */	\
		0x01B40154,/* Set0 Tail Dropped Packet Counter */		\
		0x01B40158,/* Set0 Control Packet Counter */			\
		0x01B4015C,/* Set0 Egress Forwarding Restriction Dropped Packet Count */ 	\
		0x01B40180,/* Set0 Multicast FIFO Dropped Packet Counter */				\
		0x01B40164,/* Set1 Outgoing Unicast Packet Counter */ 	\
		0x01B40168,/* Set1 Outgoing Multicast Packet Counter */	\
		0x01B4016C,/* Set1 Outgoing Broadcast Packet Counter */	\
		0x01B40170,/* Set1 Bridge Egress Filtered Packet Count */	\
		0x01B40174,/* Set1 Tail Dropped Packet Counter */		\
		0x01B40178,/* Set1 Control Packet Counter */			\
		0x01B4017C,/* Set1 Egress Forwarding Restriction Dropped Packet Count */ 	\
		0x01B40184 /* Set1 Multicast FIFO Dropped Packet Counter */				\
	}; /* register address */

	char *regDesc[] = { 	\
		"Port<26>RxUcastPkts",		\
		"Port<26>RxBcastPkts",		\
		"Port<26>RxMcastPkts",		\
		"Port<26>RxErrorPkts",		\
		"Port<26>BadCRCPkts",		\
		"Port<26>RxFifoOverrun",	\
		"Port<26>UndersizePkts",	\
		"Port<26>FragmentPkts",		\
		"Port<26>OversizePkts",		\
		"Port<26>JabberPkts",		\
		"Port<26>TxUcastPkts",		\
		"Port<26>TxBcastPkts",		\
		"Port<26>TxMcastPkts",		\
		"Port<26>TxCRCError",		\
		"Port<27>RxUcastPkts",		\
		"Port<27>RxBcastPkts",		\
		"Port<27>RxMcastPkts",		\
		"Port<27>RxErrorPkts",		\
		"Port<27>BadCRCPkts",		\
		"Port<27>RxFifoOverrun",	\
		"Port<27>UndersizePkts",	\
		"Port<27>FragmentPkts",		\
		"Port<27>OversizePkts",		\
		"Port<27>JabberPkts",		\
		"Port<27>TxUcastPkts",		\
		"Port<27>TxBcastPkts",		\
		"Port<27>TxMcastPkts",		\
		"Port<27>TxCRCError",		\
		"HostInPkts",				\
		"HostOutPkts",				\
		"OutBcastPkts",				\
		"OutMCastPkts",				\
		"MatrixSDPkts",				\
		"Set0InPkts",				\
		"Set0IngFilterPkts",		\
		"Set0SecFilterPkts",		\
		"Set0BrgFilterPkts",		\
		"Set1InPkts",				\
		"Set1IngFilterPkts",		\
		"Set1SecFilterPkts",		\
		"Set1BrgFilterPkts",		\
		"BrgDropCnt",				\
		"GlbSecBreachDropCnt",		\
		"Port/VLANSecBreachDropCnt",\
		"Set0OutUcCnt",				\
		"Set0OutMcCnt",				\
		"Set0OutBcCnt",				\
		"Set0BrgEgrFilteredCnt",	\
		"Set0TailDroppedCnt",		\
		"Set0OutControlCnt",		\
		"Set0EgrFwdRestricDropCnt",	\
		"Set0McFifoFullDropCnt",	\
		"Set1OutUcCnt",				\
		"Set1OutMcCnt",				\
		"Set1OutBcCnt",				\
		"Set1BrgEgrFilteredCnt",	\
		"Set1TailDroppedCnt",		\
		"Set1OutControlCnt",		\
		"Set1EgrFwdRestricDropCnt",	\
		"Set1McFifoFullDropCnt"		\
	}; /* register address description */
	
	unsigned int value; /* register value */
	GT_U8 devCount = sizeof(devNumArr)/sizeof(GT_U8);
	GT_U8 regCount = sizeof(regAddrArr)/sizeof(long);
	GT_U8 devNum,i,j;
	GT_STATUS rc;

	osPrintf("\n");
	for(i = 0; i < devCount; i++)
	{
		devNum = devNumArr[i];
		PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
		osPrintf("%sCascade Port for %s%s\n",PRINTSTAR,devDesc[i],PRINTSTAR);
		for(j = 0; j < regCount; j++)
		{
			if((devNum == 0)&&
				((regAddrArr[j] & 0xFFFF0000) == 0x01DC0000))/* 98Dx275 have no Port<27> */
				continue;
			
			rc = prvCpssDrvHwPpReadRegister(devNum, (unsigned long)regAddrArr[j], &value);
			osPrintf("%#0x:%-26s\t%#0x\n",(unsigned int)regAddrArr[j],regDesc[j],value);			
		}
		osPrintf("%s%s%s\n",PRINTSTAR,PRINTSTAR,PRINTSTAR);
	}
	return GT_OK;
}

#define PRINTSTAR "***********************"
GT_U8  mibDevNumArr[] = {		\
	0,/* 98Dx275 */ 			\
	1 /* 98Dx804 */
};/* device number */

char *mibDevDesc[] = { 			\
	"98Dx275",					\
	"98Dx804"					\
};

/* Add for XCAT device */
char *mibDevDesc_xcat[] = { 	\
	"98Dx4122"				\
};
long mibGeRegBaseArr[] = {		\
	0x04010000,/* Port<0-5> */	\
	0x04810000,/* Port<6-11> */ \
	0x05010000,/* Port<12-17>*/ \
	0x05810000 /* Port<18-23>*/ \
};
long mibXgRegBaseArr[] = { 		\
	0x01C00000,/* Port<24> */	\
	0x01C40000,/* Port<25> */	\
	0x01C80000,/* Port<26> */	\
	0x01DC0000,/* Port<27> */	\
};

/* Add for XG port on XCAT, zhangdi 2011-04-07 */
unsigned int mibXgRegBaseArr_xcat[] = {     \
	0x09300000,/* Port<24> */	\
	0x09320000,/* Port<25> */	\
	0x09340000,/* Port<26> */	\
	0x09360000 /* Port<27> */	\
};

/* Add for XG port on lion, zhangdi 2012-08-07 */
unsigned int mibXgRegBaseArr_lion[] = {     \
    0x09000000, \
    0x09020000, \
    0x09040000, \
    0x09060000, \
    0x09080000, \
    0x090A0000, \
    0x090C0000, \
    0x090E0000, \    
    0x09100000, \
    0x09120000, \
    0x09140000, \     
    0x09160000
};
/* Add end, the offset below we are the same */

long mibRegOffset[] = { 					\
	0x10,/* Port<n> Rx Ucast Pkt */ 			\
	0x18,/* Port<n> Rx Bcast Pkt */ 			\
	0x1C,/* Port<n> Rx Mcast Pkt */ 			\
	0x70,/* Port<n> Rx Error  Pkt */			\
	0x74,/* Port<n> BadCRC	*/				\
	0x5C,/* Port<n> ReceivedFIFO overrun */ 	\
	0x60,/* Port<n> Undersize */				\
	0x64,/* Port<n> Fragments */				\
	0x68,/* Port<n> Oversize */ 				\
	0x6C,/* Port<n> Jabber */					\
	0x40,/* Port<n> Tx Ucast Pkt */ 			\
	0x4C,/* Port<n> Tx Bcast Pkt */ 			\
	0x48,/* Port<n> Tx Mcast Pkt */ 			\
	0x0C /* Port<n> Tx CRC Error */ 			\
}; /* register address */

long mibRegOffsetAll [] = {
	0x0, /* good byte rx LSB-bits */
	0x4, /* good byte rx MSB-bits */
	0x8, /* Bad bytes rx */
	0x10,/*	good uncast frame rx */
	0x18,/* Bcast frame rx */
	0x1c,/* Mcast frame rx */
	0x58,/* FC frmae rx	*/
	0x5c,/* Fifo overrun rx */
	0x60,/* undersize pkts rx */
	0x64,/* fragments rx */
	0x68,/* overSize pkts */
	0x6c,/* Jabber pkts */
	0x70,/* Error frames rx */
	
	0x74,/* Bad CRC */
	0x78,/* Collisions */
	0x7c,/* late collisions */

	0x14,/* sent deferred */     
	0x38,/* good byte tx LSB-bits */
	0x3c,/* good byte tx MSB-bits */
	0x40,/* ucast frame tx */
	0x44,/* Excessive Collision -only applicable Half-duplex mode.*/
	0x48,/* Mcast frame tx */
	0x4c,/* Bcast frame tx */
	0x50,/* Sent Multiple */
	0x54,/* FC frame tx	*/
	0x0c,/* Fifo  underrun & CRC error in Tx.*/

	0x34,/* 1024-max */
	0x30,/* 512-1023 */
	0x2c,/* 256-511 */
	0x28,/* 128-255 */
	0x24,/* 65-127 */
	0x20,/* 64 */
};	/*Reg Addr*/

char *mibRegDesc[] = { 	\
	"RxUcastPkts",		\
	"RxBcastPkts",		\
	"RxMcastPkts",		\
	"RxErrorPkts",		\
	"BadCRCPkts",		\
	"RxFifoOverrun",	\
	"UndersizePkts",	\
	"FragmentPkts", 	\
	"OversizePkts", 	\
	"JabberPkts",		\
	"TxUcastPkts",		\
	"TxBcastPkts",		\
	"TxMcastPkts",		\
	"TxCRCError"		\
}; /* register address description */

GT_U8 mibDevCount = sizeof(mibDevNumArr)/sizeof(GT_U8);
GT_U8 mibRegCount = sizeof(mibRegOffset)/sizeof(long);
GT_U8 mibRegCountAll = sizeof(mibRegOffsetAll)/sizeof(long);

/*******************************************************************************
* cpssDxChBrgGEPortMIBRead
*
* DESCRIPTION:
*       Gets all the MIB counter value for GE ports of the specified device
*
* APPLICABLE DEVICES: ALL DxCh Devices.
*
* INPUTS:
*
* OUTPUTS:
*
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hw error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       Learned Entry Discards Counter is clear-on-read.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGEPortMIBRead
(
	IN GT_U8 devNum,
	IN GT_U8 portNum
)
{
	unsigned int value; /* register value */
	GT_U8 k;
	unsigned long regAddr;
	GT_STATUS rc;

    /* Use this branch for XCAT, copy from dxChPortBufMgInit(),zhangdi */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {   
    	osPrintfDbg("\n");
    	//if(devNum > 0) return GT_BAD_PARAM;/* check 98Dx275 only */
    	
    	PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    	PRV_CPSS_DXCH_VIRTUAL_PORT_CHECK_MAC(devNum,portNum);
    	
    	osPrintfDbg("%sGE Port for XCAT: %s%s\n",PRINTSTAR,mibDevDesc_xcat[devNum],PRINTSTAR);
    	for(k = 0;k < mibRegCount; k++)
    	{
    		regAddr = mibGeRegBaseArr[portNum/6] + (portNum%6)*0x80 + mibRegOffset[k];
    		rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &value);
    		osPrintfDbg("%#0x:Port<%2d>%-26s\t%ld\n",(unsigned int)regAddr,portNum,mibRegDesc[k],value);			
    	}
    	osPrintfDbg("%s%s%s\n",PRINTSTAR,PRINTSTAR,PRINTSTAR);
    	return GT_OK;
    }
    else 
    {
    	osPrintfDbg("\n");
    	if(devNum > 0) return GT_BAD_PARAM;/* check 98Dx275 only */
    	
    	PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    	PRV_CPSS_DXCH_VIRTUAL_PORT_CHECK_MAC(devNum,portNum);
    	
    	osPrintfDbg("%sGE Port for %s%s\n",PRINTSTAR,mibDevDesc[devNum],PRINTSTAR);
    	for(k = 0;k < mibRegCount; k++)
    	{
    		regAddr = mibGeRegBaseArr[portNum/6] + (portNum%6)*0x80 + mibRegOffset[k];
    		rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &value);
    		osPrintfDbg("%#0x:Port<%2d>%-26s\t%ld\n",(unsigned int)regAddr,portNum,mibRegDesc[k],value);			
    	}
    	osPrintfDbg("%s%s%s\n",PRINTSTAR,PRINTSTAR,PRINTSTAR);
    	return GT_OK;
    }
}

/*******************************************************************************
* cpssDxChBrgGEPortPktStatistic
*
* DESCRIPTION:
*       Gets all the MIB counter value for GE ports of the specified device
*
* APPLICABLE DEVICES: ALL DxCh Devices.
*
* INPUTS:
*
* OUTPUTS:
*
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hw error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       Learned Entry Discards Counter is clear-on-read.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGEPortPktStatistic
(
	IN GT_U8 devNum,
	IN GT_U8 portNum,
	OUT struct npd_port_counter  *portPktCount,
	OUT struct port_oct_s	*portOtcCount
)
{
	GT_STATUS rc = GT_OK;
	unsigned int value[sizeof(mibRegOffsetAll)/sizeof(long)]; /* register value */
	CPSS_PORT_DUPLEX_ENT	dMode;
	GT_U8 k;
	unsigned long regAddr;
	
	if(devNum > 0) return GT_BAD_PARAM;/* check 98Dx275 only */
	
	PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
	PRV_CPSS_DXCH_VIRTUAL_PORT_CHECK_MAC(devNum,portNum);
	
	/*osPrintf("%sCascade Port for %s%s\r\n",PRINTSTAR,mibDevDesc[devNum],PRINTSTAR);*/
	for(k = 0;k < mibRegCountAll; k++)
	{
		regAddr = mibGeRegBaseArr[portNum/6] + (portNum%6)*0x80 + mibRegOffsetAll[k];
		rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &value[k]);
		/*osPrintf("%#0x:Port<%2d>%-26s\t%#0x\r\n",(unsigned int)regAddr,portNum,mibRegDesc[k],value[k]);*/		
	}
	osPrintf("value get finish\n");
	rc = cpssDxChPortDuplexModeGet(devNum,portNum,&dMode);
	/**/
	portPktCount->rx.goodbytesl = value[0];/*0x0 GoodOctetsReceived*/
	portPktCount->rx.goodbytesh = value[1];/*0x4 GoodOctetsReceived*/
	portPktCount->rx.badbytes = value[2];/*0x8 Bad Octets Received*/
	portPktCount->rx.uncastframes = value[3];/*0x10 GoodUnicast Frames Received*/
	portPktCount->rx.bcastframes = value[4];/*0x18 Bcast*/
	portPktCount->rx.mcastframes = value[5];/*0x1c Mcast*/
	portPktCount->rx.fcframe = value[6];/*0x58 FC rx*/
	portPktCount->rx.fifooverruns = value[7];/*0x5c fifo overrun rx*/
	portPktCount->rx.underSizepkt = value[8];/*0x60 underSize*/
	portPktCount->rx.fragments = value[9];/*0x64 fragments*/
	portPktCount->rx.overSizepkt = value[10];/*0x68 overSize*/
	portPktCount->rx.jabber = value[11];/*0x6c Jabber*/
	portPktCount->rx.errorframe = value[12];/*0x70 Error frame*/

	portOtcCount->badCRC = value[13];/*0x74 Error frame*/
	portOtcCount->collision = value[14];/*0x78 Error frame*/
	portOtcCount->late_collision = value[15];/*0x7c Error frame*/
	
	portPktCount->tx.sent_deferred = value[16];/*0x14 Sentdeferred*/
	portPktCount->tx.goodbytesl = value[17];/*0x38 GoodOctetsSent*/
	portPktCount->tx.goodbytesh =	value[18];/*0x3c GoodOctetsSent*/
	portPktCount->tx.uncastframe = value[19];/*0x40 Unicast Frame Sent*/
	portPktCount->tx.excessiveCollision = value[20];/*0x44 Excessive Collision*/
	portPktCount->tx.mcastframe = value[21];/*0x48 Multicast FrameSent*/
	portPktCount->tx.bcastframe = value[22];/*0x4c Broadcast FrameSent*/
	portPktCount->tx.sentMutiple= value[23];/*0x50 SentMultiple*/
	portPktCount->tx.fcframe = value[24];/*0x54 FcSent*/
	portPktCount->tx.crcerror_fifooverrun = value[25];/*0x0c Tx FIFO underrun and Tx CRC errors*/

	portOtcCount->b1024oct2max = value[26];/*0x34 1024-max*/
	portOtcCount->b512oct21023 = value[27];/*0x30 512-1023*/
	portOtcCount->b256oct511 = value[28];/*0x2c 256-511*/
	portOtcCount->b128oct255 = value[29] ;/*0x28 128-255*/
	portOtcCount->b64oct127 = value[30] ;/*0x24 64-127*/
	portOtcCount->b64oct = value[31] ;/*0x20 64*/

	#if 0 /*delete by wujh 08/08/20*/
	osPrintf("map value to pportPKtCount\n");
	if(CPSS_PORT_HALF_DUPLEX_E ==  dMode){
		portPktCount->rx.collision = value[15] + value[16] + value[20];
	}
	else { 
		portPktCount->rx.collision = value[15] + value[16];/*collision *//*lata collision*/
	}
	#endif

#if 0
	struct cpss_pkg_stat  stat;
	CPSS_PORT_DUPLEX_ENT	dMode = 0;
	cpssDxChPortDuplexModeGet(devNum,portNum,&dMode);

	memset(&stat, 0, sizeof(struct cpss_pkg_stat));
	stat.devNum = devNum;
	stat.portNum = portNum;
	stat.mode = dMode;

	/*stat type same to GT_PKG_COUNT_STAT*/
	rc = ugetpktstat(&stat);
	if(rc != 0) {
		osPrintf("prestera ioctl error\n");
		memset(portPktCount,0,sizeof(struct npd_port_counter));

	}
	else {
		memcpy(&portPktCount->rx ,&stat.rx,sizeof(struct rx_s));
		memcpy(&portPktCount->tx ,&stat.tx,sizeof(struct tx_s));
	}
#endif

	return rc;
}

/*******************************************************************************
* cpssDxChBrgGEPortPktStatistic
*
* DESCRIPTION:
*       Gets all the MIB counter value for GE ports of the specified device
*
* APPLICABLE DEVICES: ALL DxCh Devices.
*
* INPUTS:
*
* OUTPUTS:
*
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hw error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       Learned Entry Discards Counter is clear-on-read.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgGEPortPktClear
(
	IN GT_U8 devNum,
	IN GT_U8 portNum
)
{
	GT_STATUS rc = GT_OK;

	rc = uclearpktstat(devNum, portNum);
	if(rc != 0) {
		osPrintf("prestera ioctl error\n");
	}
	
	return rc;
}

/*******************************************************************************
* cpssDxChBrgXGPortMIBRead
*
* DESCRIPTION:
*       Gets all the MIB counter value for XG ports of the specified device
*
* APPLICABLE DEVICES: ALL DxCh Devices.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hw error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       Learned Entry Discards Counter is clear-on-read.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgXGPortMIBRead
(
	IN GT_U8 devNum,
	IN GT_U8 portNum
)
{
	GT_U8 j;
	unsigned int value; /* register value */
	unsigned long regAddr;
	GT_STATUS rc;

    /* Use this branch for XCAT, copy from dxChPortBufMgInit(),zhangdi */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {   
    	osPrintfDbg("\n");
    	PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    	PRV_CPSS_DXCH_VIRTUAL_PORT_CHECK_MAC(devNum,portNum);

    	osPrintfDbg("%sXG Port for XCAT: %s%s\n",PRINTSTAR,mibDevDesc_xcat[devNum],PRINTSTAR);
    	for(j = 0; j < mibRegCount; j++)
    	{
			/* code optmize: Overrunning array "mibXgRegBaseArr". zhangdi@autelan.com 2013-01-17 */
	        regAddr = mibXgRegBaseArr_xcat[portNum - 24] + mibRegOffset[j];
    		rc = prvCpssDrvHwPpReadRegister(devNum, (unsigned long)regAddr, &value);
    		osPrintfDbg("%#0x:<Port%2d>%-26s\t%ld\n",(unsigned int)regAddr,portNum,mibRegDesc[j],value);			
    	}
    	osPrintfDbg("%s%s%s\n",PRINTSTAR,PRINTSTAR,PRINTSTAR);
    	return GT_OK;
    }
    else 
    {
		/* Here is for 275 and so on */
    	osPrintfDbg("\n");
    	PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    	PRV_CPSS_DXCH_VIRTUAL_PORT_CHECK_MAC(devNum,portNum);
    	
    	osPrintfDbg("%sCascade Port for %s%s\n",PRINTSTAR,mibDevDesc[devNum],PRINTSTAR);
    	for(j = 0; j < mibRegCount; j++)
    	{
			/* code optmize: Overrunning array "mibXgRegBaseArr". zhangdi@autelan.com 2013-01-17 */			
    		regAddr = mibXgRegBaseArr[portNum - 24] + mibRegOffset[j];
    		if((devNum == 0)&&
    			((regAddr & 0xFFFF0000) == 0x01DC0000))/* 98Dx275 have no Port<27> */
    			continue;
    		
    		rc = prvCpssDrvHwPpReadRegister(devNum, (unsigned long)regAddr, &value);
    		osPrintfDbg("%#0x:<Port%2d>%-26s\t%ld\n",(unsigned int)regAddr,portNum,mibRegDesc[j],value);			
    	}
    	osPrintfDbg("%s%s%s\n",PRINTSTAR,PRINTSTAR,PRINTSTAR);
    	return GT_OK;
    }
}

/*******************************************************************************
* cpssDxChBrgXGPortPktStatistic
*
* DESCRIPTION:
*       Gets all the MIB counter value for XG ports of the specified device
*
* APPLICABLE DEVICES: ALL DxCh Devices.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hw error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       Learned Entry Discards Counter is clear-on-read.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgXGPortPktStatistic
(
	IN GT_U8 devNum,
	IN GT_U8 portNum,
	OUT struct npd_port_counter * portPktCount,
	OUT struct port_oct_s	*portOtcCount
)
{

	GT_U8 j;
	GT_STATUS rc;
    GT_U32  regAddr; /* register address */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
	unsigned int value[sizeof(mibRegOffsetAll)/sizeof(long)]={0}; /* register value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

	/* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    /* Use this branch for XCAT, zhangdi 2011-07-14 */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
		osPrintfDbg("%sXG Port for XCAT\n",PRINTSTAR,PRINTSTAR);
    	for(j = 0; j < mibRegCountAll; j++)
    	{
			/* code optmize: Overrunning array "mibXgRegBaseArr". zhangdi@autelan.com 2013-01-17 */			
    		regAddr = mibXgRegBaseArr_xcat[portNum - 24] + mibRegOffsetAll[j];
    		rc = prvCpssDrvHwPpPortGroupReadRegister(devNum,portGroupId,regAddr, &value[j]);
    		osPrintfDbg("%#0x:<Port%2d>%ld\n",(unsigned int)regAddr,portNum,value[j]);	
    	}
    }
    /* Use this branch for lion, zhangdi@autelan.com 2012-08-07 */
    else if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
    	osPrintfDbg("%sXG Port for lion\n",PRINTSTAR,PRINTSTAR);
    	osPrintfDbg("dev:%d group:%d port:%d \n",devNum,portGroupId,portNum);
    	for(j = 0; j < mibRegCountAll; j++)
    	{
    		regAddr = mibXgRegBaseArr_lion[portNum%12] + mibRegOffsetAll[j];
    		rc = prvCpssDrvHwPpPortGroupReadRegister(devNum,portGroupId,regAddr, &value[j]);
    		osPrintfDbg("%#0x:<Port%2d>%ld\n",regAddr,portNum,value[j]);	
    	}
    }	
	else
	{
    	PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    	PRV_CPSS_DXCH_VIRTUAL_PORT_CHECK_MAC(devNum,portNum);
    	
    	osPrintf("%sCascade Port for %s%s\n",PRINTSTAR,mibDevDesc[devNum],PRINTSTAR);
    	for(j = 0; j < mibRegCountAll; j++)
    	{
			/* code optmize: Overrunning array "mibXgRegBaseArr". zhangdi@autelan.com 2013-01-17 */
    		regAddr = mibXgRegBaseArr[portNum-24] + mibRegOffsetAll[j];
    		if((devNum == 0)&&
    			((regAddr & 0xFFFF0000) == 0x01DC0000))/* 98Dx275 have no Port<27> */
    			continue;
    		rc = prvCpssDrvHwPpReadRegister(devNum,regAddr, &value[j]);
    		osPrintf("%#0x:<Port%2d>%ld\n",(unsigned int)regAddr,portNum,value[j]);	
    	}
	}

	portPktCount->rx.goodbytesl = value[0];
	portPktCount->rx.goodbytesh  = 	value[1]; /*0x0,0x4;*/
	portPktCount->rx.badbytes = value[2];/*0x8*/
	portPktCount->rx.uncastframes= value[3];/*0x10*/
	portPktCount->rx.bcastframes = value[4];/*0x18 Bcast*/
	portPktCount->rx.mcastframes = value[5];/*0x1c Mcast*/
	portPktCount->rx.fcframe= value[6];/*0x58 fragments*/
	portPktCount->rx.fifooverruns = value[7];/*0x5c fifo overrun rx*/
	portPktCount->rx.underSizepkt = value[8];/*0x60 underSize*/
	portPktCount->rx.fragments = value[9];/*0x64 fragments*/
	portPktCount->rx.overSizepkt= value[10];/*0x68 overSize*/
	portPktCount->rx.jabber = value[11];/*0x6c Jabber*/
	portPktCount->rx.errorframe= value[12];/*0x70 Error frame*/
	portOtcCount->badCRC = value[13];/*0x74 Bad CRC*/
	portOtcCount->collision = value[14];/*0x78 Error frame*/
	portOtcCount->late_collision = value[15];/*0x7c Error frame*/
	portPktCount->tx.sent_deferred =value[16];/*0x14 sent deferred */
	portPktCount->tx.goodbytesl = value[17];/* good byte tx LSB-bits */
	portPktCount->tx.goodbytesh = value[18];/* good byte tx MSB-bits */
	portPktCount->tx.uncastframe = value[19];/* ucast frame tx */
	portPktCount->tx.excessiveCollision = value[20];/*0x44 Excessive Collision*/
	portPktCount->tx.mcastframe = value[21];/*0x48 multicast */
	portPktCount->tx.bcastframe = value[22];/*0x4c broadcast*/
	portPktCount->tx.sentMutiple= value[23];/*0x50 SentMultiple*/
	portPktCount->tx.fcframe= value[24];/*0x54 FCSent*/
	portPktCount->tx.crcerror_fifooverrun = value[25];/*0x0c Tx FIFO underrun and Tx CRC errors*/
	portOtcCount->b1024oct2max = value[26];/*0x34 1024-max Frames1024toMaxOctets*/
	portOtcCount->b512oct21023 = value[27];/*0x30 512-1023 Frames512to1023Octets*/
	portOtcCount->b128oct255 = value[28] ;/*0x28 128-255*/
	portOtcCount->b256oct511 = value[29];/*0x2c 256-511  Frames256to511Octets*/
	portOtcCount->b64oct127 = value[30] ;/*0x24 64-127 Frames128to256Octets*/
	portOtcCount->b64oct = value[31] ;/*0x20 64 Frames64Octets*/
	return GT_OK;
}

extern  GT_U32	appDemoPpConfigDevAmount;
GT_STATUS xgportmibreadall()
{
	GT_U8 devNum,portNum;
	GT_STATUS rc;
	unsigned int level = 0;
	
	level = cpss_debug_status_get();
	cpss_debug_level_set(0xFF);

	for(devNum = 0;devNum < appDemoPpConfigDevAmount;devNum++)
	{
		for(portNum = 24;portNum < 28;portNum++)
		{
			rc = cpssDxChBrgXGPortMIBRead(devNum,portNum);
			if(GT_OK != rc) {
				osPrintfErr("xg port(%d,%d) mib read error(%d)!\n", devNum, portNum, rc);
				return rc;
			}
		}
	}

	cpss_debug_level_set(0);
	cpss_debug_status_set(level);
	return rc;
}

GT_STATUS geportmibreadall()
{
	GT_U8 devNum = 0,portNum = 0;
	GT_STATUS rc = GT_OK;
	unsigned int level = 0;
	
	level = cpss_debug_status_get();
	cpss_debug_level_set(0xFF);

	for(devNum = 0;devNum < appDemoPpConfigDevAmount; devNum++)
	{
		if(BOARD_TYPE == BOARD_TYPE_AX81_AC8C)
		{   
			/* There is only 4 port on board AX81-AC12C */
			for(portNum = 0;portNum < 4;portNum++)
    		{
    			rc = cpssDxChBrgGEPortMIBRead(devNum,portNum);
    			if(GT_OK != rc) {
    				osPrintfErr("ge port(%d,%d) mib read error(%d)!\n", devNum, portNum, rc);
    				return rc;
    			}
    		}	
		}
		else
		{
    		for(portNum = 0;portNum < 24;portNum++)
    		{
    			rc = cpssDxChBrgGEPortMIBRead(devNum,portNum);
    			if(GT_OK != rc) {
    				osPrintfErr("ge port(%d,%d) mib read error(%d)!\n", devNum, portNum, rc);
    				return rc;
    			}
    		}
		}
	}
	cpss_debug_level_set(0);
	cpss_debug_level_set(level);
	return rc;
}
#endif

