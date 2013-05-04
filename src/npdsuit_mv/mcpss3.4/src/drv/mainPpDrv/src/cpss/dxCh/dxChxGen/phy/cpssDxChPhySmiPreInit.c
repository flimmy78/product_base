/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
*
* cpssDxChPhySmiPreInit.c
*
* DESCRIPTION:
*       API implementation for PHY SMI pre init configuration.
*       All functions should be called after cpssDxChHwPpPhase1Init() and
*       before cpssDxChPhyPortSmiInit().
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmiPreInit.h>
/*#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmi.h>*/
#include <cpss/generic/phy/private/prvCpssGenPhySmi.h>
#include <cpss/dxCh/dxChxGen/phy/private/prvCpssDxChPhySmi.h>

/***************Private functions ********************************************/

/* Convert SMI Auto Poll number of ports software value to hardware */
#define PRV_CPSS_DXCH_SMI_AUTO_POLL_NUM_OF_PORTS_SW_TO_HW_CONVERT_MAC(_autoPollNumOfPorts,_value) \
    switch (_autoPollNumOfPorts)                            \
    {                                                       \
        case CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_8_E:  \
            _value = 0x0;                                   \
            break;                                          \
        case CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_12_E: \
            _value = 0x1;                                   \
            break;                                          \
        case CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_16_E: \
            _value = 0x2;                                   \
            break;                                          \
        default:                                            \
            return GT_BAD_PARAM;                            \
    }


/* Convert SMI Auto Poll number of ports hardware value to software  */
#define PRV_CPSS_DXCH_SMI_AUTO_POLL_NUM_OF_PORTS_HW_TO_SW_CONVERT_MAC(_value, _autoPollNumOfPorts) \
    switch (_value)                                                                 \
    {                                                                               \
        case 0x0:                                                                   \
            _autoPollNumOfPorts = CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_8_E;     \
            break;                                                                  \
        case 0x1:                                                                   \
            _autoPollNumOfPorts = CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_12_E;    \
            break;                                                                  \
        case 0x2:                                                                   \
            _autoPollNumOfPorts = CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_16_E;    \
            break;                                                                  \
        default:                                                                    \
            return GT_BAD_PARAM;                                                    \
    }

/*******************************************************************************
* cpssDxChPhyPortAddrSet
*
* DESCRIPTION:
*       Configure the port's default phy address, this function should be
*       used to change the default port's phy address.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - port number.
*       phyAddr     - The new phy address, (value 0...31).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_SUPPORTED         - for XG ports
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For flex ports this API and cpssDxChPhyPortSmiInterfaceSet() must be
*       used before any access to flex port phy related registers.
*       This function should be called after cpssDxChHwPpPhase1Init()
*       and before cpssDxChPhyPortSmiInit().
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPortAddrSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_U8   phyAddr
)
{
    GT_U32  regOffset;              /* Phy address register offset.     */
    GT_U32  fieldOffset;            /* Phy address reg. field offset.   */
    GT_U32  regAddr;                /* Register address.                */
    GT_U32  phyAddress;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */
    GT_STATUS rc = GT_OK;     /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    if(phyAddr >= BIT_5)
    {
        return GT_OUT_OF_RANGE;
    }

    /* XG ports are not supported */
    if ((GT_FALSE == PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum,portNum)) &&
        PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType == PRV_CPSS_PORT_XG_E)
    {
        return GT_NOT_SUPPORTED;
    }

    phyAddress = (GT_U32)phyAddr;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    /* the DxCh devices has PHY polling support (Out-Of-Band autonegotiation)
       for first 24 ports only. Some xCat devices has Flex ports (24-27).
       This ports has no PHY polling support. */
    if (localPort < PRV_CPSS_DXCH_SMI_PPU_PORTS_NUM_CNS)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiPhyAddr;

        /* calc the reg and bit offset for the port */
        regOffset   = (localPort / 6) * CPSS_DX_PHY_ADDR_REG_OFFSET_CNS;
        fieldOffset = (localPort % 6) * 5;

        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,
                                  regAddr + regOffset,
                                  fieldOffset,
                                  5,
                                  phyAddress);
    }
    else if (PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum,portNum))
    {
        /* flex port */
        PRV_CPSS_PHY_SMI_PORT_ADDR_MAC(devNum,portNum) = phyAddr;
    }
    else
        rc = GT_NOT_SUPPORTED;

    return rc;
}

/*******************************************************************************
* cpssDxChPhyPortAddrGet
*
* DESCRIPTION:
*       Gets port's phy address from hardware and from database.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - port number.
*
* OUTPUTS:
*       phyAddFromHwPtr - (pointer to) phy address in HW.
*       phyAddFromDbPtr - (pointer to) phy address in DB.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_SUPPORTED         - for XG ports
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For flex ports no hardware value exists and this field should be
*       ignored.
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPortAddrGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_U8   *phyAddFromHwPtr,
    OUT GT_U8   *phyAddFromDbPtr
)
{
    GT_U32  regOffset;              /* Phy address register offset.     */
    GT_U32  fieldOffset;            /* Phy address reg. field offset.   */
    GT_U32  regAddr;                /* Register address.                */
    GT_U32  phyAddress;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */
    GT_STATUS rc = GT_OK;     /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(phyAddFromHwPtr);
    CPSS_NULL_PTR_CHECK_MAC(phyAddFromDbPtr);

    /* XG ports are not supported */
    if ((GT_FALSE == PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum,portNum)) &&
        (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType == PRV_CPSS_PORT_XG_E))
    {
        return GT_NOT_SUPPORTED;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    /* the DxCh devices has PHY polling support (Out-Of-Band autonegotiation)
       for first 24 ports only. Some xCat devices has Flex ports (24-27).
       This ports has no PHY polling support. */
    if (localPort < PRV_CPSS_DXCH_SMI_PPU_PORTS_NUM_CNS)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiPhyAddr;

        /* calc the reg and bit offset for the port */
        regOffset   = (localPort / 6) * CPSS_DX_PHY_ADDR_REG_OFFSET_CNS;
        fieldOffset = (localPort % 6) * 5;

        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId,
                                  regAddr + regOffset,
                                  fieldOffset,
                                  5,
                                  &phyAddress);
        if( rc != GT_OK )
        {
            return rc;
        }
        *phyAddFromHwPtr = (GT_U8)phyAddress;
    }
    else if (PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum,portNum))
    {
        /* flex port */
        *phyAddFromDbPtr = (GT_U8)PRV_CPSS_PHY_SMI_PORT_ADDR_MAC(devNum,portNum);
    }
    else
        rc = GT_NOT_SUPPORTED;

    return rc;
}

/*******************************************************************************
* cpssDxChPhyAutoPollNumOfPortsSet
*
* DESCRIPTION:
*       Configure number of auto poll ports for SMI0 and SMI1.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum                  - physical device number.
*       autoPollNumOfPortsSmi0  - number of ports for SMI0.
*       autoPollNumOfPortsSmi1  - number of ports for SMI1.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, autoPollNumOfPortsSmi0 or
*                                                   autoPollNumOfPortsSmi1.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The acceptable combinations for Auto Poll number of ports are:
*       |-----------------------|
*       |   SMI 0   |   SMI 1   |
*       |-----------|-----------|
*       |     8     |    16     |
*       |     8     |    12     |
*       |    12     |    12     |
*       |    16     |    8      |
*       |-----------------------|
*
*       This function should be called after cpssDxChHwPpPhase1Init()
*       and before cpssDxChPhyPortSmiInit().
*
*******************************************************************************/
GT_STATUS cpssDxChPhyAutoPollNumOfPortsSet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    autoPollNumOfPortsSmi0,
    IN  CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    autoPollNumOfPortsSmi1
)
{
    GT_U32          regAddr;        /* register address */
    GT_U32          regValue;       /* hw value         */
    GT_STATUS       rc;             /* return code      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);


    /* check that the combination is acceptable */
    if (((autoPollNumOfPortsSmi0 == CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_8_E) &&
        (autoPollNumOfPortsSmi1 == CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_16_E)) ||
        ((autoPollNumOfPortsSmi0 == CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_12_E) &&
        (autoPollNumOfPortsSmi1 == CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_12_E)) ||
        ((autoPollNumOfPortsSmi0 == CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_16_E) &&
        (autoPollNumOfPortsSmi1 == CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_8_E)) ||
        ((autoPollNumOfPortsSmi0 == CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_8_E) &&
        (autoPollNumOfPortsSmi1 == CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_12_E)))
    {
        /* valid combination, do nothing */
    }
    else
    {
        return GT_BAD_PARAM;
    }

    PRV_CPSS_DXCH_SMI_AUTO_POLL_NUM_OF_PORTS_SW_TO_HW_CONVERT_MAC(
                                                    autoPollNumOfPortsSmi0,
                                                    regValue)

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms0MiscConfig;

    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 18, 2, regValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_DXCH_SMI_AUTO_POLL_NUM_OF_PORTS_SW_TO_HW_CONVERT_MAC(
                                                    autoPollNumOfPortsSmi1,
                                                    regValue)

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms1MiscConfig;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 18, 2, regValue);

}

/*******************************************************************************
* cpssDxChPhyAutoPollNumOfPortsGet
*
* DESCRIPTION:
*       Get number of auto poll ports for SMI0 and SMI1.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum                  - physical device number.
*
* OUTPUTS:
*       autoPollNumOfPortsSmi0Ptr  - number of ports for SMI0.
*       autoPollNumOfPortsSmi1Ptr  - number of ports for SMI1.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, autoPollNumOfPortsSmi0 or
*                                                   autoPollNumOfPortsSmi1.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPhyAutoPollNumOfPortsGet
(
    IN  GT_U8                                           devNum,
    OUT CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    *autoPollNumOfPortsSmi0Ptr,
    OUT CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    *autoPollNumOfPortsSmi1Ptr
)
{
    GT_U32          regAddr;        /* register address */
    GT_U32          regValue;       /* hw value         */
    GT_STATUS       rc;             /* return code      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);
    
    CPSS_NULL_PTR_CHECK_MAC(autoPollNumOfPortsSmi0Ptr);
    CPSS_NULL_PTR_CHECK_MAC(autoPollNumOfPortsSmi1Ptr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms0MiscConfig;
    
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 18, 2, &regValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_DXCH_SMI_AUTO_POLL_NUM_OF_PORTS_HW_TO_SW_CONVERT_MAC(
                                                    regValue,
                                                    *autoPollNumOfPortsSmi0Ptr)

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms1MiscConfig;
    
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 18, 2, &regValue);
    
    PRV_CPSS_DXCH_SMI_AUTO_POLL_NUM_OF_PORTS_HW_TO_SW_CONVERT_MAC(
                                                    regValue,
                                                    *autoPollNumOfPortsSmi1Ptr)
    return rc;
}

/*******************************************************************************
* cpssDxChPhyPortSmiInterfaceSet
*
* DESCRIPTION:
*       Configure port SMI interface.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number.
*       smiInterface    - port SMI interface.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, portNum or SMI interface.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For flex ports this API and cpssDxChPhyPortAddrSet() must be
*       used before any access to flex port PHY related registers.
*       This function should be called after cpssDxChHwPpPhase1Init()
*       and before cpssDxChPhyPortSmiInit().
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPortSmiInterfaceSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  CPSS_PHY_SMI_INTERFACE_ENT  smiInterface
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    switch(smiInterface)
    {
        case CPSS_PHY_SMI_INTERFACE_0_E:
            /* use SMI#0 controller to access port's PHY*/
            PRV_CPSS_PHY_SMI_CTRL_REG_ADDR_MAC(devNum,portNum) =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiControl;
            break;
        case CPSS_PHY_SMI_INTERFACE_1_E:
            /* use SMI#1 controller to access port's PHY*/
            PRV_CPSS_PHY_SMI_CTRL_REG_ADDR_MAC(devNum,portNum) =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiControl +
                CPSS_DX_SMI_MNG_CNTRL_OFFSET_CNS;
            break;
        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPhyPortSmiInterfaceGet
*
* DESCRIPTION:
*       Get port SMI interface.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number.
*
* OUTPUTS:
*       smiInterfacePtr - (pointer to) port SMI interface.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, portNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - port SMI interface was not initialized
*                                  correctly.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPortSmiInterfaceGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT CPSS_PHY_SMI_INTERFACE_ENT  *smiInterfacePtr
)
{
    GT_U32 smiCtrlAddr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(smiInterfacePtr);

    smiCtrlAddr = PRV_CPSS_PHY_SMI_CTRL_REG_ADDR_MAC(devNum,portNum);

    if( smiCtrlAddr == PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiControl )
    {
        *smiInterfacePtr = CPSS_PHY_SMI_INTERFACE_0_E;
    }
    else if( smiCtrlAddr == (PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiControl +
                                CPSS_DX_SMI_MNG_CNTRL_OFFSET_CNS) )
    {
        *smiInterfacePtr = CPSS_PHY_SMI_INTERFACE_1_E;
    }
    else
        return GT_NOT_INITIALIZED;

    return GT_OK;
}
