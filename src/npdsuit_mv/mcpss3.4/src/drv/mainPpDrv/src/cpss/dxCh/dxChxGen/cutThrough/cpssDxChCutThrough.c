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
* cpssDxChCutThrough.c
*
* DESCRIPTION:
*     CPSS DXCH Cut Through facility facility implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/private/prvCpssCommonDefs.h>
#include <cpss/dxCh/dxChxGen/cutThrough/cpssDxChCutThrough.h>

/*******************************************************************************
* cpssDxChCutThroughPortEnableSet
*
* DESCRIPTION:
*      Enable/Disable Cut Through forwarding for packets received on the port.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number including CPU port.
*       enable   - GT_TRUE:  Enable the port for Cut Through.
*                  GT_FALSE: Disable the port for Cut Through.
*       untaggedEnable -  GT_TRUE:  Enable Cut Through forwarding for
*                                   untagged packets received on the port.
*                         GT_FALSE: Disable Cut Through forwarding for
*                                    untagged packets received on the port.
*                         Used only if enable == GT_TRUE.
*                         Untagged packets for Cut Through purposes - packets
*                         that don't have VLAN tag or its ethertype isn't equal
*                         to one of two configurable VLAN ethertypes.
*                         See cpssDxChCutThroughVlanEthertypeSet.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       Cut Through forwarding for tagged packets is enabled
*       per source port and UP. See cpssDxChCutThroughUpEnableSet.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughPortEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  enable,
    IN GT_BOOL  untaggedEnable
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    fieldOffset;       /* The start bit number in the register */
    GT_U32    data;              /* reg sub field data */
    GT_U32    mask;              /* Mask for selecting the written bits */
    GT_U32    portGroupId;       /* the port group Id - support multi port group device */
    GT_U8     localPort;         /* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi port groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    if(CPSS_CPU_PORT_NUM_CNS == localPort)
    {
        fieldOffset = 15;
    }
    else
    {
        fieldOffset = localPort;

    }

    data = (enable == GT_TRUE) ? (1 << fieldOffset) : 0;
    mask = 1 << fieldOffset;

    /* Set Cut Through forwarding data for untagged packets received on the port */
    fieldOffset += 16;
    data |= (untaggedEnable == GT_TRUE) ? (1 << fieldOffset) : 0;
    mask |= 1 << fieldOffset;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                       cutThroughRegs.ctEnablePerPortReg;

    /* Enable/Disable Cut Through forwarding */
    return prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum, portGroupId,
                                             regAddr, mask, data);
}

/*******************************************************************************
* cpssDxChCutThroughPortEnableGet
*
* DESCRIPTION:
*      Get Cut Through forwarding mode on the specified port.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number including CPU port.
*
* OUTPUTS:
*       enablePtr   - pointer to Cut Through forwarding mode status:
*                    - GT_TRUE:  Enable the port for Cut Through.
*                      GT_FALSE: Disable the port for Cut Through.
*       untaggedEnablePtr - pointer to Cut Through forwarding mode status
*                            for untagged packets.
*                            Used only if *enablePtr == GT_TRUE.
*                            GT_TRUE:  Enable Cut Through forwarding for
*                                      untagged packets received on the port.
*                            GT_FALSE: Disable Cut Through forwarding for
*                                      untagged packets received on the port.
*                         Untagged packets for Cut Through purposes - packets
*                         that don't have VLAN tag or its ethertype isn't equal
*                         to one of two configurable VLAN ethertypes.
*                         See cpssDxChCutThroughVlanEthertypeSet.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughPortEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT GT_BOOL  *enablePtr,
    OUT GT_BOOL  *untaggedEnablePtr
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    fieldOffset;       /* The start bit number in the register */
    GT_U32    data;              /* reg data */
    GT_U32    portGroupId;       /* the port group Id - support multi port group device */
    GT_U8     localPort;         /* local port - support multi-port-groups device */
    GT_STATUS rc;              /* return status                */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(untaggedEnablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi port groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum);


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                       cutThroughRegs.ctEnablePerPortReg;

    /* Get Enable/Disable Cut Through forwarding status */
    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &data);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(CPSS_CPU_PORT_NUM_CNS == localPort)
    {
        fieldOffset = 15;
    }
    else
    {
        fieldOffset = localPort;
    }

    *enablePtr = (((data >> fieldOffset) & 0x1) == 0) ? GT_FALSE : GT_TRUE;

    /* Get Cut Through forwarding status for untagged packets received on the port */
    *untaggedEnablePtr = (((data >> (fieldOffset + 16)) & 0x1) == 0) ?
        GT_FALSE : GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCutThroughUpEnableSet
*
* DESCRIPTION:
*       Enable / Disable tagged packets with the specified UP
*       to be Cut Through.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum          - device number.
*       up              - user priority of a VLAN or DSA tagged
*                         packet [0..7].
*       enable   - GT_TRUE:  tagged packets, with the specified UP
*                            may be subject to Cut Through forwarding.
*                  GT_FALSE: tagged packets, with the specified UP
*                            aren't subject to Cut Through forwarding.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or up.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The ingress port should be enabled for Cut Through forwarding.
*       Use cpssDxChCutThroughPortEnableSet for it.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughUpEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    up,
    IN GT_BOOL  enable
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | 
         CPSS_XCAT2_E);

    if(up > 7)
    {
        return GT_BAD_PARAM;
    }

    data = (enable == GT_TRUE) ? 1 : 0;
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cutThroughRegs.ctUpEnableReg;

    /* Enable / Disable tagged packets, with the specified UP to be Cut Through. */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, up, 1, data);

}

/*******************************************************************************
* cpssDxChCutThroughUpEnableGet
*
* DESCRIPTION:
*      Get Cut Through forwarding mode for tagged packets
*      with the specified UP.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum      - device number
*       up              - user priority of a VLAN or DSA tagged
*                         packet [0..7].
*
* OUTPUTS:
*       enablePtr   - pointer to Cut Through forwarding mode status
*                     for tagged packets, with the specified UP.:
*                     - GT_TRUE:  tagged packets, with the specified UP
*                                 may be subject to Cut Through forwarding.
*                       GT_FALSE: tagged packets, with the specified UP
*                                 aren't subject to Cut Through forwarding.
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or up
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughUpEnableGet
(
    IN  GT_U8    devNum,
    IN GT_U8     up,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32    regAddr;     /* register address */
    GT_U32    data;        /* reg sub field data */
    GT_STATUS rc;          /* return status                */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | 
         CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(up > 7)
    {
        return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cutThroughRegs.ctUpEnableReg;

    /* Enable / Disable tagged packets, with the specified UP to be Cut Through. */
    rc =  prvCpssDrvHwPpGetRegField(devNum, regAddr, up, 1, &data);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (data == 0) ? GT_FALSE : GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCutThroughVlanEthertypeSet
*
* DESCRIPTION:
*       Set VLAN Ethertype in order to identify tagged packets.
*       A packed is identified as VLAN tagged for cut-through purposes.
*       Packet considered as tagged if packet's Ethertype equals to one of two
*       configurable VLAN Ethertypes.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum           - device number.
*       etherType0       - VLAN EtherType0, values 0..0xFFFF
*       etherType1       - VLAN EtherType1, values 0..0xFFFF
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_OUT_OF_RANGE          - on wrong etherType0 or etherType1
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughVlanEthertypeSet
(
    IN GT_U8    devNum,
    IN GT_U32   etherType0,
    IN GT_U32   etherType1
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | 
         CPSS_XCAT2_E);

    if((etherType0 > 0xFFFF) || (etherType1 > 0xFFFF))
    {
        return GT_OUT_OF_RANGE;
    }
    data = etherType0 | etherType1 << 16;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cutThroughRegs.ctEthertypeReg;

    /* Set VLAN Ethertype in order to identify tagged packets. */
    return prvCpssDrvHwPpWriteRegister(devNum, regAddr, data);
}

/*******************************************************************************
* cpssDxChCutThroughVlanEthertypeGet
*
* DESCRIPTION:
*       Get VLAN Ethertype in order to identify tagged packets.
*       A packed is identified as VLAN tagged for cut-through purposes.
*       Packet considered as tagged if packet's Ethertype equals to one of two
*       configurable VLAN Ethertypes.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum           - device number.
*
* OUTPUTS:
*       etherType0Ptr    - Pointer to VLAN EtherType0
*       etherType1Ptr    - Pointer to VLAN EtherType1.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughVlanEthertypeGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *etherType0Ptr,
    OUT GT_U32   *etherType1Ptr
)
{
    GT_U32    regAddr;     /* register address   */
    GT_U32    data;        /* reg data */
    GT_STATUS rc;          /* return status      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | 
         CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(etherType0Ptr);
    CPSS_NULL_PTR_CHECK_MAC(etherType1Ptr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cutThroughRegs.ctEthertypeReg;

    /* Get VLAN Ethertype in order to identify tagged packets. */
    rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &data);
    if(rc != GT_OK)
    {
        return rc;
    }

    *etherType0Ptr = data & 0xFFFF;
    *etherType1Ptr = (data >> 16) & 0xFFFF;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCutThroughMinimalPacketSizeSet
*
* DESCRIPTION:
*       Set minimal packet size that is enabled for Cut Through.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum      - device number.
*       size        - minimal packet size in bytes for Cut Through [129..16376].
*                     Granularity - 8 bytes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_OUT_OF_RANGE          - on wrong size
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       When using cut-through to 1G port, and bypassing
*       of Router And Ingress Policer engines is disabled,
*       the minimal packet size should be 512 bytes.
*       When bypassing of Router And Ingress Policer engines is enabled,
*       the minimal cut-through packet size should be:
*       - for 10G or faster ports - at least 257 bytes.
*       - for ports slower than 10 G  - at least 513 bytes
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughMinimalPacketSizeSet
(
    IN GT_U8    devNum,
    IN GT_U32   size
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | 
         CPSS_XCAT2_E);

    if((size < 0x81) || (size > 0x3FF8))
    {
        return GT_OUT_OF_RANGE;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        cutThroughRegs.ctPacketIndentificationReg;

    /* The field is defined in "Number of transactions" in resolution of 8 bytes.
       For example: 17 transactions (default value) means a packet whose
       byte count is 129B up to 136B */
    data = size / 8;
    if((size % 8) != 0)
    {
        data++;
    }

    /* Set minimal packet size that is enabled for Cut Through. */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 11, data);
}

/*******************************************************************************
* cpssDxChCutThroughMinimalPacketSizeGet
*
* DESCRIPTION:
*       Get minimal packet size that is enabled for Cut Through.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum      - device number.
*
* OUTPUTS:
*       sizePtr     - pointer to minimal packet size in bytes for Cut Through.
*                     Granularity - 8 bytes.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughMinimalPacketSizeGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *sizePtr
)
{
    GT_U32    regAddr;     /* register address */
    GT_U32    data;        /* reg sub field data */
    GT_STATUS rc;          /* return status                */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | 
         CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(sizePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        cutThroughRegs.ctPacketIndentificationReg;

    /* Get minimal packet size that is enabled for Cut Through. */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 11, &data);
    if(rc != GT_OK)
    {
        return rc;
    }

    *sizePtr = data * 8;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChCutThroughMemoryRateLimitSet
*
* DESCRIPTION:
*       Set rate limiting of read operations from the memory
*       per target port in Cut Through mode according to the port speed.
*       To prevent congestion in egress pipe, buffer memory read operations
*       are rate limited according to the target port speed.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - physical port number including CPU port.
*       enable       - GT_TRUE - rate limiting is enabled.
*                    - GT_FALSE - rate limiting is disabled.
*       portSpeed    - port speed.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portNum or portSpeed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Rate limit is recommended be enabled
*       on all egress ports for cut-through traffic.
*       Rate limit to the CPU port should be configured as a 1G port.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughMemoryRateLimitSet
(
    IN GT_U8   devNum,
    IN GT_U8   portNum,
    IN GT_BOOL enable,
    IN CPSS_PORT_SPEED_ENT  portSpeed
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */
    GT_U32    fieldOffset;       /* The start bit number in the register */
    GT_U32    portGroupId;       /* the port group Id - support multi port group device */
    GT_U8     localPort;         /* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | 
         CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi port groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum);

    if(enable == GT_FALSE)
    {
        data = 0;
    }
    else
    {
        switch(portSpeed)
        {
            case CPSS_PORT_SPEED_1000_E:
                data = 360;
                break;
            case CPSS_PORT_SPEED_10000_E:
                data = 36;
                break;
            case CPSS_PORT_SPEED_2500_E:
                data = 144;
                break;
           case CPSS_PORT_SPEED_5000_E:
               data = 72;
               break;
            case CPSS_PORT_SPEED_12000_E:
                data = 34;
                break;
            case CPSS_PORT_SPEED_13600_E:
                data = 27;
                break;
            case CPSS_PORT_SPEED_20000_E:
                data = 18;
                break;
            case CPSS_PORT_SPEED_40000_E:
                data = 9;
                break;
            default:
                return GT_BAD_PARAM;
        }
    }

    if(CPSS_CPU_PORT_NUM_CNS == localPort)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            cutThroughRegs.ctCpuPortMemoryRateLimitThresholdReg;
        fieldOffset = 0;
    }
    else
    {
        /* fix Coverity warning OVERRUN_STATIC for array:
         PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cutThroughRegs.ctPortMemoryRateLimitThresholdReg
        */
        if (localPort >= 12)
        {
            /* it's should never happen because
            PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC does not allows
            such ports to be accepted  */
            return GT_FAIL;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            cutThroughRegs.ctPortMemoryRateLimitThresholdReg[localPort / 2];
        fieldOffset = (localPort % 2) * 10;
    }

    /* Set rate limiting. */
    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                         fieldOffset, 10, data);

}

/*******************************************************************************
* cpssDxChCutThroughMemoryRateLimitGet
*
* DESCRIPTION:
*       Get rate limiting of read operations from the memory
*       per target port in Cut Through mode.
*       To prevent congestion in egress pipe, buffer memory read operations
*       are rate limited according to the target port speed.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum        - device number.
*       portNum       - physical port number including CPU port.
*
* OUTPUTS:
*       enablePtr     - pointer to rate limiting mode status.
*                        - GT_TRUE - rate limiting is enabled.
*                        - GT_FALSE - rate limiting is disabled.
*       portSpeedPtr  - pointer to port speed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or portNum
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughMemoryRateLimitGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL *enablePtr,
    OUT CPSS_PORT_SPEED_ENT  *portSpeedPtr
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */
    GT_U32    fieldOffset;       /* The start bit number in the register */
    GT_U32    portGroupId;       /* the port group Id - support multi port group device */
    GT_U8     localPort;         /* local port - support multi-port-groups device */
    GT_STATUS rc;          /* return status                */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(portSpeedPtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi port groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum);

    if(CPSS_CPU_PORT_NUM_CNS == localPort)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            cutThroughRegs.ctCpuPortMemoryRateLimitThresholdReg;
        fieldOffset = 0;
    }
    else
    {
        /* fix Coverity warning OVERRUN_STATIC for array:
         PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cutThroughRegs.ctPortMemoryRateLimitThresholdReg
        */
        if (localPort >= 12)
        {
            /* it's should never happen because
            PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC does not allows
            such ports to be accepted  */
            return GT_FAIL;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            cutThroughRegs.ctPortMemoryRateLimitThresholdReg[localPort / 2];
        fieldOffset = (localPort % 2) * 10;
    }

    /* Get rate limiting. */
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                       fieldOffset, 10, &data);
    if(rc != GT_OK)
    {
        return rc;
    }
    if(data == 0)
    {
        *enablePtr = GT_FALSE;
    }
    else
    {
        switch(data)
        {
            case 360:
                *portSpeedPtr = CPSS_PORT_SPEED_1000_E;
                break;
            case 36:
                *portSpeedPtr = CPSS_PORT_SPEED_10000_E;
                break;
            case 144:
                *portSpeedPtr = CPSS_PORT_SPEED_2500_E;
                break;
            case 72:
                *portSpeedPtr = CPSS_PORT_SPEED_5000_E;
                break;
            case 34:
                *portSpeedPtr = CPSS_PORT_SPEED_12000_E;
                break;
            case 27:
                *portSpeedPtr = CPSS_PORT_SPEED_13600_E;
                break;
            case 18:
                *portSpeedPtr = CPSS_PORT_SPEED_20000_E;
                break;
            case 9:
                *portSpeedPtr = CPSS_PORT_SPEED_40000_E;
                break;
            default:
                return GT_BAD_STATE;
        }

        *enablePtr = GT_TRUE;
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChCutThroughBypassRouterAndPolicerEnableSet
*
* DESCRIPTION:
*       Enable / Disable bypassing the Router and Ingress Policer engines.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       enable       - GT_TRUE -  bypassing of Router and Ingress Policer
*                                 engines is enabled.
*                    - GT_FALSE - bypassing of Router and Ingress Policer
*                                 engines is disabled.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughBypassRouterAndPolicerEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | 
         CPSS_XCAT2_E);

    data = (enable == GT_TRUE) ? 1 : 0;
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bridgeRegs.bridgeGlobalConfigRegArray[2];

    /* Enable / Disable bypassing the Router and Ingress Policer engines.  */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 20, 1, data);

}

/*******************************************************************************
* cpssDxChCutThroughBypassRouterAndPolicerEnableGet
*
* DESCRIPTION:
*       Get mode of bypassing the Router and Ingress Policer engines.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum        - device number.
*
* OUTPUTS:
*       enablePtr     - pointer to bypassing the Router and Ingress
*                       Policer engines status.
*                      - GT_TRUE  -  bypassing of Router and Ingress Policer
*                                    engines is enabled.
*                      - GT_FALSE -  bypassing of Router and Ingress Policer
*                                  engines is disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughBypassRouterAndPolicerEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32    regAddr;     /* register address */
    GT_U32    data;        /* reg sub field data */
    GT_STATUS rc;          /* return status                */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | 
         CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bridgeRegs.bridgeGlobalConfigRegArray[2];

    /* Get mode of bypassing the Router and Ingress Policer engines. */
    rc =  prvCpssDrvHwPpGetRegField(devNum, regAddr, 20, 1, &data);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (data == 0) ? GT_FALSE : GT_TRUE;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChCutThroughDefaultPacketLengthSet
*
* DESCRIPTION:
*       Set default packet length for Cut-Through mode.
*       This packet length is used for all byte count based mechanisms.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum        - device number.
*       packetLength  - default packet length for Cut-Through mode. (0...0x3fff)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_OUT_OF_RANGE          - on wrong packetLength.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughDefaultPacketLengthSet
(
    IN GT_U8  devNum,
    IN GT_U32 packetLength
)
{
    GT_U32    regAddr;           /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | 
         CPSS_XCAT2_E);

    if(packetLength > 0x3FFF)
    {
        return GT_OUT_OF_RANGE;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        cutThroughRegs.ctGlobalConfigurationReg;

    /* Set default packet length for Cut-Through mode. */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 14, packetLength);

}

/*******************************************************************************
* cpssDxChCutThroughDefaultPacketLengthGet
*
* DESCRIPTION:
*       Get default packet length for Cut-Through mode.
*       This packet length is used for all byte count based mechanisms.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum        - device number.
*
* OUTPUTS:
*       packetLengthPtr - pointer to default packet length for Cut-Through mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughDefaultPacketLengthGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *packetLengthPtr
)
{
    GT_U32    regAddr;     /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | 
         CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(packetLengthPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        cutThroughRegs.ctGlobalConfigurationReg;

     /* Get default packet length for Cut-Through mode. */
    return  prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 14, packetLengthPtr);

}

