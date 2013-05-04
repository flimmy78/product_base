/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgPrvEdgeVlan.c
*
* DESCRIPTION:
*       cpss Dx-Ch implementation for Prestera Private Edge VLANs.
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgPrvEdgeVlan.h>

/*******************************************************************************
* cpssDxChBrgPrvEdgeVlanEnable
*
* DESCRIPTION:
*       This function enables/disables the Private Edge VLAN on
*       specified device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       enable - GT_TRUE to enable the feature,
*                GT_FALSE to disable the feature
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPrvEdgeVlanEnable
(
    IN GT_U8        devNum,
    IN GT_BOOL      enable
)
{
    GT_U32  regAddr;    /* address of register */
    GT_U32  value;      /* value to write into a register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    value = (enable == GT_TRUE) ? 1 : 0;
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.bridgeGlobalConfigRegArray[0];

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 30, 1, value);
}

/*******************************************************************************
* cpssDxChBrgPrvEdgeVlanPortEnable
*
* DESCRIPTION:
*       Enable/Disable a specified port to operate in Private Edge VLAN mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number to set state.
*       enable    - GT_TRUE for enabled, GT_FALSE for disabled
*       dstPort   - the destination "uplink" physical port to which all traffic
*                   received on srcPort&srcDev is forwarded.  This parameter
*                   is ignored if disabling the mode.
*       dstDev    - the destination "uplink" physical device to which all
*                   traffic received on srcPort&srcDev is forwarded.  This
*                   parameter is ignored if disabling the mode.
*       dstTrunk  - the destination is a trunk member. This parameter
*                   is ignored if disabling the mode.
*                   GT_TRUE - dstPort hold value of trunkId , and dstDev is
*                             ignored
*                   GT_FALSE - dstPort hold value of port in device dstDev
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM     - on bad portNum or devNum or dstPort or dstDev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPrvEdgeVlanPortEnable
(
    IN GT_U8      devNum,
    IN GT_U8      portNum,
    IN GT_BOOL    enable,
    IN GT_U8      dstPort,
    IN GT_U8      dstDev,
    IN GT_BOOL    dstTrunk
)
{
    GT_STATUS rc;
    GT_U32  regAddr;    /* address of register */
    GT_U32  value;      /* value to write into a register */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    /* dstPort and dstDev are meaningless in the disable case:*/
    /* replace them by srcPort and srcDev for checking needs  */
    if(enable == GT_FALSE)
    {
        dstPort = portNum;
        dstDev  = PRV_CPSS_HW_DEV_NUM_MAC(devNum);
        dstTrunk = GT_FALSE;
    }

    if(dstTrunk == GT_TRUE)
    {
        dstDev = PRV_CPSS_HW_DEV_NUM_MAC(devNum);/* HW ignore this value */
        if(dstPort >= BIT_7)/* support 127 trunks */
        {
            /* HW support 7 bits */
            return GT_BAD_PARAM;
        }
    }
    else
    {
        if(dstDev >= BIT_5 || dstPort >= BIT_6)
        {
            /* dstDev  - only 5 bits in HW for this parameter */
            /* dstPort - only 6 bits in HW for this parameter */

            return GT_BAD_PARAM;
        }
    }

    value = ((enable == GT_TRUE) ? 1 : 0) |
            (((dstTrunk == GT_TRUE) ? 1 : 0) << 1) |
            (dstPort << 2) ;
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                        bridgeRegs.portControl[portNum];

    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId, regAddr,23 ,9 ,value);
    if(rc != GT_OK)
    {
        return rc;
    }

    value = dstDev;

    return prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId, regAddr + 0x10 ,0 ,5 ,value);
}

/*******************************************************************************
* cpssDxChBrgPrvEdgeVlanPortEnableGet
*
* DESCRIPTION:
*       Get enabling/disabling status to operate in Private Edge VLAN mode 
*       and destination parameters for a specified port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number to set state.
*
* OUTPUTS:
*       enablePtr - pointer to status of enabling/disabling a specified port 
*                   to operate in Private Edge VLAN mode.
*                   GT_TRUE for enabled,
*                   GT_FALSE for disabled
*       dstPortPtr  - pointer to the destination "uplink" physical port to which
*                     all trafficreceived on srcPort&srcDev is forwarded.  
*                     This parameter is ignored if disabling the mode.
*       dstDevPtr   - pointer to the destination "uplink" physical device to 
*                     which all traffic received on srcPort&srcDev is forwarded.  
*                     This parameter is ignored if disabling the mode.
*       dstTrunkPtr - pointer to the destination is a trunk member.
*                     This parameter is ignored if disabling the mode.
*                     GT_TRUE - dstPort hold value of trunkId , and dstDev is
*                               ignored
*                     GT_FALSE - dstPort hold value of port in device dstDev
*                     
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad portNum or devNum 
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPrvEdgeVlanPortEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    OUT GT_BOOL    *enablePtr,
    OUT GT_U8      *dstPortPtr,
    OUT GT_U8      *dstDevPtr,
    OUT GT_BOOL    *dstTrunkPtr
)
{
    GT_STATUS rc;
    GT_U32  regAddr;    /* address of register */
    GT_U32  value;      /* register field value */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(dstPortPtr);
    CPSS_NULL_PTR_CHECK_MAC(dstDevPtr);
    CPSS_NULL_PTR_CHECK_MAC(dstTrunkPtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                        bridgeRegs.portControl[portNum];

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, regAddr,23 ,9 ,&value);
    if(rc != GT_OK)
    {
        return rc;
    }

    if((value & 0x1) == 1)
    {
        *enablePtr = GT_TRUE;
        *dstPortPtr = (GT_U8)(value >> 2);

        if(((value >> 1) & 0x1) == 1)
        { 
            *dstTrunkPtr = GT_TRUE;
        }
        else
        {
            *dstTrunkPtr = GT_FALSE;
            rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, 
                                                    regAddr + 0x10 ,0 ,5 ,&value);
            if(rc != GT_OK)
            {
                return rc;
            }
            *dstDevPtr = (GT_U8)value;
        }
    }
    else
    {
        *enablePtr = GT_FALSE;
    }

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkSet
*
* DESCRIPTION:
*       Enable/Disable per port forwarding control taffic to Private Edge
*       VLAN Uplink. PVE port can be disabled from trapping or mirroring
*       bridged packets to the CPU. In this case, as long as the packet is not
*       assigned a HARD DROP or SOFT DROP, the packet is unconditionally
*       assigned a FORWARD command with the ingress ports configured
*       PVE destination (PVLAN Uplink).
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - device number
*       port    - physical or CPU port to set.
*       enable  - GT_TRUE  - forward control traffic to PVE Uplink
*                 GT_FALSE - not forward control traffic to PVE Uplink,
*                            trapped or mirrored to CPU packets are sent to
*                            CPU port
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, port
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       port,
    IN  GT_BOOL     enable
)
{
    GT_U32  regAddr;    /* address of register */
    GT_U32  value;      /* value to write into a register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,port);

    if (port != CPSS_CPU_PORT_NUM_CNS)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                        bridgeRegs.portControl[port];
    }
    else
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                        bridgeRegs.cpuPortControl;

    value = (GT_TRUE == enable) ? 1 : 0;

    return prvCpssDrvHwPpPortGroupSetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port),
            regAddr, 22, 1, value);

}

/*******************************************************************************
* cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkGet
*
* DESCRIPTION:
*       Get Enable state per Port forwarding control taffic to Private Edge
*       VLAN Uplink. PVE port can be disabled from trapping or mirroring
*       bridged packets to the CPU. In this case, as long as the packet is not
*       assigned a HARD DROP or SOFT DROP, the packet is unconditionally
*       assigned a FORWARD command with the ingress ports configured
*       PVE destination (PVLAN Uplink).
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - device number
*       port    - physical or CPU port to set.
*
* OUTPUTS:
*       enablePtr GT_TRUE  - forward control traffic to PVE Uplink
*                 GT_FALSE - not forward control traffic to PVE Uplink,
*                            trapped or mirrored to CPU packets are sent to
*                            CPU port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, port
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       port,
    OUT GT_BOOL     *enablePtr
)
{
    GT_STATUS rc;       /* return status */
    GT_U32  regAddr;    /* address of register */
    GT_U32  value;      /* value to write into a register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,port);

    if (port != CPSS_CPU_PORT_NUM_CNS)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                        bridgeRegs.portControl[port];
    }
    else
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                        bridgeRegs.cpuPortControl;

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port),
            regAddr, 22, 1, &value);
    if(rc != GT_OK)
        return rc;

    *enablePtr = (value == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;

}


/****
	by zhubo@autelan.com set cscd port
******/
#ifndef __AX_PLATFORM__
int cscd(unsigned char dev,unsigned char port,unsigned int enable)
{
	     if (prvCpssDrvHwPpSetRegField(dev, 0x07800004, port, 1, enable) != 0)
            return GT_HW_ERROR;
        if (prvCpssDrvHwPpSetRegField(dev, 0x07800020, port, 1,enable) != 0)
            return GT_HW_ERROR;

		return 0;
}
#endif

