/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortSyncEther.c
*
* DESCRIPTION:
*       CPSS implementation for Sync-E (Synchronious Ethernet)
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortSyncEther.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPort.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>

/*******************************************************************************
* prvCpssDxChPortSyncEtherPortLaneCheckAndInterfaceModeGet
*
* DESCRIPTION:
*       Function check if the lane number is within the allowable range for
*       the port interface mode.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*       laneNum     - SERDES lane number within port.
*                     Allowable ranges according to port interface:
*                     SGMII - 0
*                     RXAUI - 0..1
*                     XAUII - 0..3
*                     XLG   - 0..7
*
* OUTPUTS:
*       ifModePtr   - (pointer to) port interface mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum or lane number
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortSyncEtherPortLaneCheck
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           portNum,
    IN  GT_U32                          laneNum,
    OUT CPSS_PORT_INTERFACE_MODE_ENT    *ifModePtr
)
{
    GT_STATUS                       rc;     /* return code */
    GT_U32                          maxLaneNum; /* lanes number according to */
                                                /* port interface mode       */

    rc = cpssDxChPortInterfaceModeGet(devNum, portNum, ifModePtr);
    if( GT_OK != rc )
    {
        return rc;
    }

    switch(*ifModePtr)
    {
        case CPSS_PORT_INTERFACE_MODE_SGMII_E:  maxLaneNum = 0;
                                                break;
        case CPSS_PORT_INTERFACE_MODE_RXAUI_E:  maxLaneNum = 1;
                                                break;
        case CPSS_PORT_INTERFACE_MODE_XGMII_E:  maxLaneNum = 3;
                                                break;
        case CPSS_PORT_INTERFACE_MODE_XLG_E:    maxLaneNum = 7;
                                                break;
        default: return GT_BAD_PARAM;
                 break;
    }

    if( laneNum > maxLaneNum )
    {
        return GT_BAD_PARAM;

    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortSyncEtherSecondRecoveryClkEnableSet
*
* DESCRIPTION:
*       Function enables/disables second recovery clock output pin.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum  - device number.
*       enable  - enable/disable second clock output pin
*                   GT_TRUE  - enable
*                   GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Function performs all SERDES power down and restore original SERDES 
*       power state.
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherSecondRecoveryClkEnableSet
(
    IN  GT_U8    devNum,
    IN  GT_BOOL  enable
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */
    GT_U32  regValue1;  /* register value */
    GT_STATUS rc;       /* returned status */
    GT_U8   i;          /* loop iterator */
    GT_U8   portNum;    /* port number */
    GT_BOOL powerStateArr[10]; /* There are 6(Giga) + 4(Stacking) SERDES groups */
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode[10]; /* port interface mode */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    switch (enable)
    {
        case GT_FALSE:
            regValue = 0;   /* <2nd_rcvr_clk_out_sel> field value */
            regValue1 = 0;  /* <ref_clk_125_sel> field value */
            break;
        case GT_TRUE:
            regValue = 1;   /* <2nd_rcvr_clk_out_sel> field value */
            regValue1 = 1;  /* <ref_clk_125_sel> field value */
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* power down SERDESes and save the current
       SERDESes power state of ports */
    for (i = 0; i < 10; i++)
    {
        if (i < 6)
        {
            /* Giga ports */
            portNum = (GT_U8)(i * 4);
        }
        else
        {   /* Stacking ports */
            portNum = (GT_U8)(18 + i);
        }

        rc = cpssDxChPortInterfaceModeGet(devNum, portNum, &ifMode[i]);
        if( GT_OK != rc )
        {
            return rc;
        }

        if( CPSS_PORT_INTERFACE_MODE_MII_E == ifMode[i] )
        {
            continue;
        }

        /* store current SERDES power state */
        rc = prvCpssDxCh3PortSerdesPowerUpDownGet(devNum, portNum, &powerStateArr[i]);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* power down SERDES */
        rc = cpssDxChPortGroupSerdesPowerStatusSet(devNum, i, GT_FALSE);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl2;

    /* set <2nd_rcvr_clk_out_sel> in extended global register2 */
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 18, 1, regValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl;

    /* set <ref_clk_125_sel> in extended global register */
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 10, 1, regValue1);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* restore previous SERDES power state */
    for (i = 0; i < 10; i++)
    {
        if ( (powerStateArr[i] == GT_FALSE) ||
             (CPSS_PORT_INTERFACE_MODE_MII_E == ifMode[i]) )
        {
            continue;
        }

        rc = cpssDxChPortGroupSerdesPowerStatusSet(devNum, i, powerStateArr[i]);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortSyncEtherSecondRecoveryClkEnableGet
*
* DESCRIPTION:
*       Function gets status (enabled/disabled) of
*       second recovery clock output pin.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum  - device number.
*
* OUTPUTS:
*       enablePtr - (pointer to) second clock output pin state
*                    GT_TRUE  - enabled
*                    GT_FALSE - disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE             - on bad state
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherSecondRecoveryClkEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */
    GT_U32  regValue1;  /* register value */
    GT_STATUS rc;       /* returned status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl2;

    /* get <2nd_rcvr_clk_out_sel> in extended global register2 */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 18, 1, &regValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl;

    /* get <ref_clk_125_sel> in extended global register */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 10, 1, &regValue1);
    if (rc != GT_OK)
    {
        return rc;
    }

    if ((regValue == 0) && (regValue1 == 0))
    {
        *enablePtr = GT_FALSE;
    }
    else if ((regValue == 1) && (regValue1 == 1))
    {
        *enablePtr = GT_TRUE;
    }
    else
    {
        return GT_BAD_STATE;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortSyncEtherPllRefClkSelectSet
*
* DESCRIPTION:
*       Function sets PLL reference clock select:
*       oscillator clock or recovered clock.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - port number.
*                      For xCat & xCat2 valid ports for configuration:
*                      Giga Ports: 0,4,8,12,16,20
*                      Stacking Ports: 24,25,26,27
*       pllClkSelect - clock source: recovered or oscillator
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, portNum or pllClkSelect
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Function performs SERDES power down and restore
*       original SERDES power state.
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherPllRefClkSelectSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_TYPE_ENT pllClkSelect
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */
    GT_U32  portGroup;  /* port group */
    GT_U8   localPort;  /* local port - support multi-port-groups device */
    GT_STATUS rc;       /* returned status */
    GT_BOOL prevPowerState; /* SERDES previous power state */
    GT_U32  portSerdesGroup;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

    if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

        portSerdesGroup = portNum;
    }
    else
    {
        /* Check port number */
        switch (portNum)
        {
            case 0:     /* Group index for Giga ports = 4-9 */
            case 4:
            case 8:
            case 12:
            case 16:
            case 20:
                portGroup = (4 + portNum / 4);
                portSerdesGroup = portNum / 4;
                break;
            case 24:    /* Group index for Stacking ports = 0-3 */
            case 25:
            case 26:
            case 27:
                portGroup = (portNum - 24);
                portSerdesGroup = (portNum - 18);
                break;
            default:
                return GT_BAD_PARAM;
        }
    }

    /* Check clock source */
    switch(pllClkSelect)
    {
        case CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_OSCILLATOR_E:
            regValue = 0;
            break;
        case CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_RECOVERED_E:
            regValue = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* store current SERDES power state */
    rc = prvCpssDxCh3PortSerdesPowerUpDownGet(devNum, portNum, &prevPowerState);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* power down SERDES */
    rc = cpssDxChPortGroupSerdesPowerStatusSet(devNum, portSerdesGroup, GT_FALSE);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        /* Extended Global Control1 register address */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl;

        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        /* set <sd_pll_ref_clk<n>_sel> in extended global register1 */
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,  portGroup, regAddr, localPort+20, 1, regValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        /* Extended Global Control2 register address */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl2;

        /* set <sd_pll_ref_clk<n>_sel> in extended global register2 */
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, portGroup, 1, regValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if (prevPowerState == GT_TRUE)
    {
        /* restore previous SERDES power state */
        rc = cpssDxChPortGroupSerdesPowerStatusSet(devNum, portSerdesGroup, prevPowerState);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortSyncEtherPllRefClkSelectGet
*
* DESCRIPTION:
*       Function gets PLL reference clock select:
*       oscillator clock or recovered clock.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number.
*       portNum   - port number.
*                   For xCat & xCat2 valid ports for configuration:
*                   Giga Ports: 0,4,8,12,16,20
*                   Stacking Ports: 24,25,26,27
*
* OUTPUTS:
*       pllClkSelectPtr - (pointer to) clock source: recovered or oscillator
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherPllRefClkSelectGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_TYPE_ENT *pllClkSelectPtr
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */
    GT_U32  portGroup;  /* port group */
    GT_U8   localPort;  /* local port - support multi-port-groups device */
    GT_STATUS rc;       /* returned status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(pllClkSelectPtr);

    if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    }
    else
    {
        /* Check port group */
        switch (portNum)
        {
            case 0:     /* Group index for Giga ports = 4-9 */
            case 4:
            case 8:
            case 12:
            case 16:
            case 20:
                portGroup = (4 + portNum / 4);
                break;
            case 24:    /* Group index for Stacking ports = 0-3 */
            case 25:
            case 26:
            case 27:
                portGroup = (portNum - 24);
                break;
            default:
                return GT_BAD_PARAM;
        }
    }

    if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        /* Extended Global Control1 register address */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl;

        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        /* get <sd_pll_ref_clk<n>_sel> in extended global register1 */
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,  portGroup, regAddr, localPort+20, 1, &regValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        /* Extended Global Control2 register address */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl2;

        /* get <sd_pll_ref_clk<n>_sel> in extended global register2 */
        rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, portGroup, 1, &regValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* Check clock source */
    *pllClkSelectPtr = (regValue == 1) ? CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_RECOVERED_E
                                       : CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_OSCILLATOR_E;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortSyncEtherRecoveryClkConfigSetForLion
*
* DESCRIPTION:
*       Function configures the recovery clock enable/disable state and sets
*       its source portNum.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum          - device number.
*       recoveryClkType - recovered clock output type.
*       enable          - enable/disable recovered clock1/clock2
*                         GT_TRUE - enable
*                         GT_FALSE - disable
*       portNum         - port number.
*       laneNum         - selected SERDES lane number within port.
*                         relevant only for Lion.
*                         Ranges according to port interface:
*                         SGMII - 0
*                         RXAUI - 0..1
*                         XAUII - 0..3
*                         XLG   - 0..7
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, recoveryClkType, portNum
*                                  or laneNum.
*       GT_BAD_STATE             - another SERDES already enabled
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*       If SERDES enabling is requesed and another SERDES in the port group
*       is already enabled, GT_BAD_STATE is returned.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortSyncEtherRecoveryClkConfigSetForLion
(
    IN  GT_U8   devNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT  recoveryClkType,
    IN  GT_BOOL enable,
    IN  GT_U8   portNum,
    IN  GT_U32  laneNum
)
{
    GT_STATUS rc;       /* returned status */
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */
    GT_U32  portGroup;  /* port group */
    GT_U8   localPort;  /* local port - support multi-port-groups device */
    GT_U32  serdes;     /* loop index */
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode; /* interface mode */

    rc = prvCpssDxChPortSyncEtherPortLaneCheck(devNum, portNum, laneNum, &ifMode);
    if( GT_OK != rc )
    {
        return rc;
    }

    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    if( CPSS_PORT_INTERFACE_MODE_XLG_E == ifMode )
    {
        localPort = 8;
    }
    else
    {
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);
    }

    switch(recoveryClkType)
    {
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK0_E: if( 0 != portGroup ) return GT_BAD_PARAM;
                                                        break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK1_E: if( 1 != portGroup ) return GT_BAD_PARAM;
                                                        break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK2_E: if( 2 != portGroup ) return GT_BAD_PARAM;
                                                        break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK3_E: if( 3 != portGroup ) return GT_BAD_PARAM;
                                                        break;
        default: return GT_BAD_PARAM;
                 break;
    }

    if( GT_FALSE == enable )
    {
        /* Serdes External Configuration 1 Register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[localPort*2+laneNum].serdesExternalReg2;

        /* <rcvrd_clk0_sel> */
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,  portGroup, regAddr, 11, 1, 1);
    }
    else /* GT_TRUE == enable */
    {
        for( serdes = 0 ; serdes < PRV_CPSS_LION_SERDES_NUM_CNS ; serdes++ )
        {
            if ( serdes == (GT_U32)(localPort*2+laneNum) )
            {
                continue;
            }

            /* Serdes External Configuration 1 Register */
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdes].serdesExternalReg2;

            /* <rcvrd_clk0_sel> */
            rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,  portGroup, regAddr, 11, 1, &regValue);
            if( GT_OK != rc )
            {
                return rc;
            }

            if( 0 == regValue )
            {
                return GT_BAD_STATE;
            }
        }

        /* Serdes External Configuration 1 Register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[localPort*2+laneNum].serdesExternalReg2;

        /* <rcvrd_clk0_sel> */
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,  portGroup, regAddr, 11, 1, 0);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChPortSyncEtherRecoveryClkConfigSet
*
* DESCRIPTION:
*       Function configures the recovery clock enable/disable state and sets
*       its source portNum.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number.
*       recoveryClkType - recovered clock output type.
*       enable          - enable/disable recovered clock1/clock2
*                         GT_TRUE - enable
*                         GT_FALSE - disable
*       portNum         - port number.
*                         For xCat & xCat2 valid ports for configuration:
*                         Giga Ports: 0,4,8,12,16,20
*                         Stacking Ports: 24,25,26,27
*       laneNum         - selected SERDES lane number within port.
*                         relevant only for Lion.
*                         Ranges according to port interface:
*                         SGMII - 0
*                         RXAUI - 0..1
*                         XAUII - 0..3
*                         XLG   - 0..7
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, recoveryClkType, portNum
*                                  or laneNum.
*       GT_BAD_STATE             - another SERDES already enabled
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherRecoveryClkConfigSet
(
    IN  GT_U8   devNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT  recoveryClkType,
    IN  GT_BOOL enable,
    IN  GT_U8   portNum,
    IN  GT_U32  laneNum
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */
    GT_U32  regOffset;  /* register offset */
    GT_U32  portGroup;  /* port group */
    GT_STATUS rc;       /* returned status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

    if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        return prvCpssDxChPortSyncEtherRecoveryClkConfigSetForLion(
                            devNum,recoveryClkType,enable,portNum,laneNum);
    }


    /* Check port number */
    switch (portNum)
    {
        case 0:     /* Group index for Giga ports = 4-9 */
        case 4:
        case 8:
        case 12:
        case 16:
        case 20:
            portGroup = (4 + portNum / 4);
            break;
        case 24:    /* Group index for Stacking ports = 0-3 */
        case 25:
        case 26:
        case 27:
            portGroup = (portNum - 24);
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* calculate register offset */
    switch(recoveryClkType)
    {
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK0_E:
            regOffset = 10;
            break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK1_E:
            regOffset = 14;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* set register value */
    regValue = (enable == GT_TRUE) ? portGroup : 0xF;

    /* Extended Global Control2 register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl2;

    /* set <rcvr_clk0_ctrl> or <rcvr_clk1_ctrl> in extended global register2 */
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, regOffset, 4, regValue);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortSyncEtherRecoveryClkConfigGetForLion
*
* DESCRIPTION:
*       Function gets the recovery clock enable/disable state and its source
*       portNum.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum          - device number.
*       recoveryClkType - recovered clock output type
*
* OUTPUTS:
*       enablePtr       - (pointer to) port state as reference.
*                         GT_TRUE -  enabled
*                         GT_FALSE - disbled
*       portNumPtr      - (pointer to) port number.
*       laneNumPtr      - (pointer to) selected SERDES lane number within port.
*                         relevant only for Lion.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, recoveryClkType
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - on bad port interface state
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortSyncEtherRecoveryClkConfigGetForLion
(
    IN  GT_U8    devNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT  recoveryClkType,
    OUT GT_BOOL  *enablePtr,
    OUT GT_U8    *portNumPtr,
    OUT GT_U32   *laneNumPtr
)
{
    GT_STATUS rc;       /* returned status */
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */
    GT_U32  portGroup;  /* port group */
    GT_U8   localPort;  /* local port - support multi-port-groups device */
    GT_U32  serdes;     /* loop index */
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode; /* port interface mode */

    switch(recoveryClkType)
    {
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK0_E: portGroup = 0;
                                                        break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK1_E: portGroup = 1;
                                                        break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK2_E: portGroup = 2;
                                                        break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK3_E: portGroup = 3;
                                                        break;
        default: return GT_BAD_PARAM;
                 break;
    }

    for( serdes = 0 ; serdes < PRV_CPSS_LION_SERDES_NUM_CNS ; serdes++ )
    {
        /* Serdes External Configuration 1 Register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdes].serdesExternalReg2;

        /* <rcvrd_clk0_sel> */
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,  portGroup, regAddr, 11, 1, &regValue);
        if( GT_OK != rc )
        {
            return rc;
        }

        if( 0 == regValue )
        {
            break;
        }
    }

    if( PRV_CPSS_LION_SERDES_NUM_CNS == serdes )
    {
        *enablePtr = GT_FALSE;
    }
    else /* SERDES which was configured for recovery clock was found */
    {
        /* find port interface to calculate lane number */

        /* check is SGMII port */
        localPort = (GT_U8)(serdes/2);
        *portNumPtr = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum, portGroup, localPort);
        rc = cpssDxChPortInterfaceModeGet(devNum, *portNumPtr, &ifMode);
        if( (GT_OK == rc) &&  (CPSS_PORT_INTERFACE_MODE_SGMII_E == ifMode) )
        {
            if( localPort != serdes*2 )
            {
                return GT_BAD_STATE;
            }

            *enablePtr = GT_TRUE;
            *laneNumPtr = 0 ;
            return GT_OK;
        }

        /* check is RXAUI port */
        localPort = (GT_U8)(serdes/2);
        *portNumPtr = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum, portGroup, localPort);
        rc = cpssDxChPortInterfaceModeGet(devNum, *portNumPtr, &ifMode);
        if( (GT_OK == rc) &&  (CPSS_PORT_INTERFACE_MODE_RXAUI_E == ifMode) )
        {
            *enablePtr = GT_TRUE;
            *laneNumPtr = serdes - localPort*2;
            return GT_OK;
        }

        /* check is XAUI port */
        localPort = (GT_U8)((serdes/4)*2);
        *portNumPtr = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum, portGroup, localPort);
        rc = cpssDxChPortInterfaceModeGet(devNum, *portNumPtr, &ifMode);
        if( (GT_OK == rc) &&  (CPSS_PORT_INTERFACE_MODE_XGMII_E == ifMode) )
        {
            *enablePtr = GT_TRUE;
            *laneNumPtr = serdes - localPort*2;
            return GT_OK;
        }

        /* check is XLG port */
        if( serdes >= 16 && serdes < 24 )
        {
            localPort = 10;
            *portNumPtr = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum, portGroup, localPort);
            rc = cpssDxChPortInterfaceModeGet(devNum, *portNumPtr, &ifMode);
            if( (GT_OK == rc) &&  (CPSS_PORT_INTERFACE_MODE_XLG_E == ifMode) )
            {
                *enablePtr = GT_TRUE;
                *laneNumPtr = serdes - 16;
                return GT_OK;
            }
        }

        /* no port interface to lane number match found */
        return GT_BAD_STATE;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortSyncEtherRecoveryClkConfigGet
*
* DESCRIPTION:
*       Function gets the recovery clock enable/disable state and its source
*       portNum.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number.
*       recoveryClkType - recovered clock output type
*
* OUTPUTS:
*       enablePtr       - (pointer to) port state as reference.
*                         GT_TRUE -  enabled
*                         GT_FALSE - disbled
*       portNumPtr      - (pointer to) port number.
*       laneNumPtr      - (pointer to) selected SERDES lane number within port.
*                         relevant only for Lion.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, recoveryClkType
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - on bad state of register
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherRecoveryClkConfigGet
(
    IN  GT_U8    devNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT  recoveryClkType,
    OUT GT_BOOL  *enablePtr,
    OUT GT_U8    *portNumPtr,
    OUT GT_U32   *laneNumPtr
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */
    GT_U32  regOffset;  /* register offset */
    GT_STATUS rc;       /* returned status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(portNumPtr);
    CPSS_NULL_PTR_CHECK_MAC(laneNumPtr);

    if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        return prvCpssDxChPortSyncEtherRecoveryClkConfigGetForLion(
                            devNum,recoveryClkType,enablePtr,portNumPtr,laneNumPtr);
    }

    /* calculate register offset */
    switch(recoveryClkType)
    {
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK0_E:
            regOffset = 10;
            break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK1_E:
            regOffset = 14;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* Extended Global Control2 register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl2;

    /* get <rcvr_clk0_ctrl> or <rcvr_clk1_ctrl> in extended global register2 */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, regOffset, 4, &regValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (regValue < 10)
    {
        *enablePtr = GT_TRUE;
    }
    else if (regValue == 0xF)
    {
        *enablePtr = GT_FALSE;
        return GT_OK;
    }
    else
        return GT_BAD_STATE;

    /* recovery clock output pin port group number */
    switch (regValue)
    {
        /* Stacking ports */
        case 0:
        case 1:
        case 2:
        case 3:
            *portNumPtr = (GT_U8)(24 + regValue);
            break;
        /* Giga ports */
        default:
            *portNumPtr = (GT_U8)((regValue - 4) * 4);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortSyncEtherRecoveryClkDividerBypassEnableSet
*
* DESCRIPTION:
*       Function sets recovery clock divider bypass enable.
*
* APPLICABLE DEVICES:
*        xCat.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2.
*
* INPUTS:
*       devNum      - device number.
*       enable      - enable/disable recovery clock divider bypass enable
*                       GT_TRUE - enable
*                       GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, enable
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherRecoveryClkDividerBypassEnableSet
(
    IN  GT_U8  devNum,
    IN  GT_BOOL enable
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */
    GT_STATUS rc;       /* returned status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_XCAT2_E);

    /* set register value */
    regValue = BOOL2BIT_MAC(enable);

    /* Extended Global Control2 register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl;

    /* set <sd_clkdiv_bypass_en> in extended global register */
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 26, 1, regValue);

    return rc;
}

/*******************************************************************************
* cpssDxChPortSyncEtherRecoveryClkDividerBypassEnableGet
*
* DESCRIPTION:
*       Function gets recovery clock divider bypass status.
*
* APPLICABLE DEVICES:
*        xCat.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2.
*
* INPUTS:
*       devNum      - device number.
*
* OUTPUTS:
*       enablePtr   - (pointer to) recovery clock divider bypass state
*                       GT_TRUE - enable
*                       GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherRecoveryClkDividerBypassEnableGet
(
    IN  GT_U8  devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */
    GT_STATUS rc;       /* returned status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* Extended Global Control2 register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl;

    /* set <sd_clkdiv_bypass_en> in extended global register */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 26, 1, &regValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* set return value */
    *enablePtr = BIT2BOOL_MAC(regValue);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortSyncEtherRecoveryClkDividerValueSet
*
* DESCRIPTION:
*       Function sets recovery clock divider value.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*                     For xCat2 valid ports for configuration:
*                     Giga Ports: 0,4,8,12,16,20
*                     Stacking Ports: 24,25,26,27
*       laneNum     - selected SERDES lane number within port.
*                     relevant only for Lion.
*                     Ranges according to port interface:
*                     SGMII - 0
*                     RXAUI - 0..1
*                     XAUII - 0..3
*                     XLG   - 0..7
*       value       - recovery clock divider value to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, portNum or lane number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on not supported port type
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherRecoveryClkDividerValueSet
(
    IN  GT_U8                                               devNum,
    IN  GT_U8                                               portNum,
    IN  GT_U32                                              laneNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_ENT  value
)
{
    GT_STATUS   rc;         /* return code      */
    GT_U32      regAddr;    /* register address */
    GT_U32      regValue;   /* register data    */
    GT_U32      regOffset;  /* offset in register */
    GT_U32      portGroup;  /* port group */
    GT_U32      serdesNum;  /* SERDES number */
    GT_U8       localPort;  /* local port - support multi-port-groups device */
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode; /* interface mode */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_FE_E)
        return GT_NOT_SUPPORTED;

    portGroup = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

    if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        rc = prvCpssDxChPortSyncEtherPortLaneCheck(devNum, portNum, laneNum, &ifMode);
        if( GT_OK != rc )
        {
            return rc;
        }
    
        portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        if( CPSS_PORT_INTERFACE_MODE_XLG_E == ifMode )
        {
            localPort = 8;
        }
        else
        {
            localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);
        }

        /* Serdes External Configuration 1 Register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[localPort*2+laneNum].serdesExternalReg2;

        regOffset = 8;
    }
    else /* xCat2 */
    {
        /* get SERDES number */
        switch (portNum)
        {
            case 0:     
            case 4:
            case 8:
            case 12:
            case 16:
            case 20:
                serdesNum = portNum/4;
                break;
            case 24:
            case 25:
            case 26:
            case 27:
                serdesNum = (portNum - 18);
                break;
            default:
                return GT_BAD_PARAM;
        }

        /* Serdes External Configuration 3 Register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].serdesExternalReg3;
        
        regOffset = 1;
    }

    switch(value)
    {
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_1_E:    regValue = 0;
                                                                    break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_2_E:    regValue = 1;
                                                                    break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_3_E:    regValue = 2;
                                                                    break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_4_E:    regValue = 3;
                                                                    break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_5_E:    regValue = 4;
                                                                    break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_8_E:    regValue = 5;
                                                                    break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_16_E:   regValue = 6;
                                                                    break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_2_5_E:  regValue = 7;
                                                                    break;
        default: return GT_BAD_PARAM;
                 break;

    }

    /* <rcvrd_clk0_div> */
    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,  portGroup, regAddr, regOffset, 3, regValue);

    return rc;
}

/*******************************************************************************
* cpssDxChPortSyncEtherRecoveryClkDividerValueGet
*
* DESCRIPTION:
*       Function gets recovery clock divider value.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*                     For xCat2 valid ports for configuration:
*                     Giga Ports: 0,4,8,12,16,20
*                     Stacking Ports: 24,25,26,27
*       laneNum     - selected SERDES lane number within port.
*                     relevant only for Lion.
*                     Ranges according to port interface:
*                     SGMII - 0
*                     RXAUI - 0..1
*                     XAUII - 0..3
*                     XLG   - 0..7
*
* OUTPUTS:
*       valuePtr   - (pointer to) recovery clock divider value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, portNum or lane number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_SUPPORTED         - on not supported port type
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherRecoveryClkDividerValueGet
(
    IN  GT_U8                                               devNum,
    IN  GT_U8                                               portNum,
    IN  GT_U32                                              laneNum,
    OUT CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_ENT  *valuePtr
)
{
    GT_STATUS   rc;         /* return code      */
    GT_U32      regAddr;    /* register address */
    GT_U32      regValue;   /* register data    */
    GT_U32      regOffset;  /* offset in register */
    GT_U32      portGroup;  /* port group */
    GT_U32      serdesNum;  /* SERDES number */
    GT_U8       localPort;  /* local port - support multi-port-groups device */
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode; /* interface mode */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(valuePtr);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_FE_E)
        return GT_NOT_SUPPORTED;

    portGroup = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

    if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        rc = prvCpssDxChPortSyncEtherPortLaneCheck(devNum, portNum, laneNum, &ifMode);
        if( GT_OK != rc )
        {
            return rc;
        }
    
        portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        if( CPSS_PORT_INTERFACE_MODE_XLG_E == ifMode )
        {
            localPort = 8;
        }
        else
        {
            localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);
        }

        /* Serdes External Configuration 1 Register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[localPort*2+laneNum].serdesExternalReg2;

        regOffset = 8;
    }
    else /* xCat2 */
    {
        /* get SERDES number */
        switch (portNum)
        {
            case 0:     
            case 4:
            case 8:
            case 12:
            case 16:
            case 20:
                serdesNum = portNum/4;
                break;
            case 24:
            case 25:
            case 26:
            case 27:
                serdesNum = (portNum - 18);
                break;
            default:
                return GT_BAD_PARAM;
        }

        /* Serdes External Configuration 3 Register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].serdesExternalReg3;
        
        regOffset = 1;
    }

    /* <rcvrd_clk0_div> */
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,  portGroup, regAddr, regOffset, 3, &regValue);
    if( GT_OK != rc )
    {
        return rc;
    }

    switch(regValue)
    {
        case 0: *valuePtr = CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_1_E;
                break;
        case 1: *valuePtr = CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_2_E;
                break;
        case 2: *valuePtr = CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_3_E;
                break;
        case 3: *valuePtr = CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_4_E;
                break;
        case 4: *valuePtr = CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_5_E;
                break;
        case 5: *valuePtr = CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_8_E;
                break;
        case 6: *valuePtr = CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_16_E;
                break;
        case 7: *valuePtr = CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_2_5_E;
                break;
        default: return GT_BAD_STATE;
                 break;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableSet
*
* DESCRIPTION:
*       Function sets Recovered Clock Automatic Masking enabling.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*                     For xCat2 valid ports for configuration:
*                     Giga Ports: 0,4,8,12,16,20
*                     Stacking Ports: 24,25,26,27
*       laneNum     - selected SERDES lane number within port.
*                     relevant only for Lion.
*                     Ranges according to port interface:
*                     SGMII - 0
*                     RXAUI - 0..1
*                     XAUII - 0..3
*                     XLG   - 0..7
*       enable      - enable/disable Recovered Clock Automatic Masking
*                     GT_TRUE - enable
*                     GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, portNum or lane number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on not supported port type
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_U32  laneNum,
    IN  GT_BOOL enable
)
{
    GT_STATUS   rc;         /* return code      */
    GT_U32      regAddr;    /* register address */
    GT_U32      regValue;   /* register data    */
    GT_U32      portGroup;  /* port group */
    GT_U32      serdesNum;  /* SERDES number */
    GT_U8       localPort;  /* local port - support multi-port-groups device */
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode; /* interface mode */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_FE_E)
        return GT_NOT_SUPPORTED;

    portGroup = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

    if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        rc = prvCpssDxChPortSyncEtherPortLaneCheck(devNum, portNum, laneNum, &ifMode);
        if( GT_OK != rc )
        {
            return rc;
        }
    
        portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        if( CPSS_PORT_INTERFACE_MODE_XLG_E == ifMode )
        {
            localPort = 8;
        }
        else
        {
            localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);
        }

        /* Serdes External Configuration 2 Register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[localPort*2+laneNum].serdesExternalReg3;
    }
    else /* xCat2 */
    {
        /* get SERDES number */
        switch (portNum)
        {
            case 0:     
            case 4:
            case 8:
            case 12:
            case 16:
            case 20:
                serdesNum = portNum/4;
                break;
            case 24:
            case 25:
            case 26:
            case 27:
                serdesNum = (portNum - 18);
                break;
            default:
                return GT_BAD_PARAM;
        }

        /* Serdes External Configuration 3 Register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].serdesExternalReg3;
    }

    /* set register value */
    regValue = BOOL2BIT_MAC(enable);

    /* <rcvrd_clk0_auto_mask> */
    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,  portGroup, regAddr, 0, 1, regValue);

    return rc;
}

/*******************************************************************************
* cpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableGet
*
* DESCRIPTION:
*       Function gets Recovered Clock Automatic Masking status.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*                     For xCat2 valid ports for configuration:
*                     Giga Ports: 0,4,8,12,16,20
*                     Stacking Ports: 24,25,26,27
*       laneNum     - selected SERDES lane number within port.
*                     relevant only for Lion.
*                     Ranges according to port interface:
*                     SGMII - 0
*                     RXAUI - 0..1
*                     XAUII - 0..3
*                     XLG   - 0..7
*
* OUTPUTS:
*       enablePtr   - (pointer to) Recovered Clock Automatic Masking state
*                     GT_TRUE - enable
*                     GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, portNum or lane number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_SUPPORTED         - on not supported port type
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_U32  laneNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS   rc;         /* return code      */
    GT_U32      regAddr;    /* register address */
    GT_U32      regValue;   /* register data    */
    GT_U32      portGroup;  /* port group */
    GT_U32      serdesNum;  /* SERDES number */
    GT_U8       localPort;  /* local port - support multi-port-groups device */
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode; /* interface mode */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_FE_E)
        return GT_NOT_SUPPORTED;

    portGroup = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

    if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        rc = prvCpssDxChPortSyncEtherPortLaneCheck(devNum, portNum, laneNum, &ifMode);
        if( GT_OK != rc )
        {
            return rc;
        }
    
        portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        if( CPSS_PORT_INTERFACE_MODE_XLG_E == ifMode )
        {
            localPort = 8;
        }
        else
        {
            localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);
        }

        /* Serdes External Configuration 2 Register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[localPort*2+laneNum].serdesExternalReg3;
    }
    else /* xCat2 */
    {
        /* get SERDES number */
        switch (portNum)
        {
            case 0:     
            case 4:
            case 8:
            case 12:
            case 16:
            case 20:
                serdesNum = portNum/4;
                break;
            case 24:
            case 25:
            case 26:
            case 27:
                serdesNum = (portNum - 18);
                break;
            default:
                return GT_BAD_PARAM;
        }

        /* Serdes External Configuration 3 Register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].serdesExternalReg3;
    }

    /* <rcvrd_clk0_auto_mask> */
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,  portGroup, regAddr, 0, 1, &regValue);
    if( GT_OK != rc )
    {
        return rc;
    }

    /* set return value */
    *enablePtr = BIT2BOOL_MAC(regValue);

    return GT_OK;
}
