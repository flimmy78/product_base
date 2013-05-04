/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChLogicalTargetMapping.c
*
* DESCRIPTION:
*       Logical Target mapping.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/logicalTarget/cpssDxChLogicalTargetMapping.h>

/*******************************************************************************
* prvCpssDxChLogicalTargetHw2LogicFormat
*
* DESCRIPTION:
*       Converts a given Logical Target Mapping Table Entry from hardware format
*       to logic format.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       hwFormatArrayPtr   - points to the configuration in HW format .
*
*
* OUTPUTS:
*       logicFormatPtr     - points to tunnel termination configuration in
*                            logic format
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PTR        - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLogicalTargetHw2LogicFormat
(
    IN GT_U32                              *hwFormatArrayPtr,
    OUT  CPSS_DXCH_OUTPUT_INTERFACE_STC    *logicFormatPtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArrayPtr);

    if ((*hwFormatArrayPtr >> 13) & 0x1 ) /* check if packet should be tunneled */
    {
        logicFormatPtr->isTunnelStart = GT_TRUE;
        if ((*hwFormatArrayPtr >> 26) & 0x1 )
        {
            logicFormatPtr->tunnelStartInfo.passengerPacketType =
                                        CPSS_DXCH_TUNNEL_PASSENGER_OTHER_E;
        }
        else
        {
            logicFormatPtr->tunnelStartInfo.passengerPacketType =
                                        CPSS_DXCH_TUNNEL_PASSENGER_ETHERNET_E;
        }
        logicFormatPtr->tunnelStartInfo.ptr = ((*hwFormatArrayPtr >> 14) & 0xfff );
    }

    if (*hwFormatArrayPtr & 0x1) /* check if multi-target (Vidx) */
    {
        logicFormatPtr->physicalInterface.type = CPSS_INTERFACE_VIDX_E;
        logicFormatPtr->physicalInterface.vidx = (GT_U16)((*hwFormatArrayPtr >> 1) & 0xfff );
    }
    else
        if ((*hwFormatArrayPtr >> 1) & 0x1) /* check if target is trunk */
        {
            /* it is trunkId interface or if packet should be tunneled -
               trunkId  + Tunnel-Start Pointer  interface*/
            logicFormatPtr->physicalInterface.type = CPSS_INTERFACE_TRUNK_E;
            logicFormatPtr->physicalInterface.trunkId = (GT_TRUNK_ID)((*hwFormatArrayPtr >> 6) & 0x7f );

        }
        else
        {
            /* it is  Single-target {Device, Port} or if packet should be tunneled -
               {Device, Port}  + Tunnel-Start Pointer  interface*/
            logicFormatPtr->physicalInterface.type = CPSS_INTERFACE_PORT_E;
            logicFormatPtr->physicalInterface.devPort.portNum = (GT_U8)((*hwFormatArrayPtr >> 2) & 0x3f );
            logicFormatPtr->physicalInterface.devPort.devNum = (GT_U8)((*hwFormatArrayPtr >> 8) & 0x1f );
        }
        return GT_OK;
}


/*******************************************************************************
* prvCpssDxChLogicalTargetLogic2HwFormat
*
* DESCRIPTION:
*       Converts a given Logical Target Mapping Table Entry from logic format
*       to hardware format.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       logicFormatPtr    - points to tunnel termination configuration in
*                           logic format
*
* OUTPUTS:
*       hwFormatArrayPtr  - points to the configuration in HW format .
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameters.
*       GT_BAD_PTR        - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLogicalTargetLogic2HwFormat
(
    IN  CPSS_DXCH_OUTPUT_INTERFACE_STC      *logicFormatPtr,
    OUT GT_U32                              *hwFormatArrayPtr
)
{
    GT_U32 useVidx;
    GT_U32 targetIsTrunk;

    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArrayPtr);

    *hwFormatArrayPtr = 0;
    if (logicFormatPtr->isTunnelStart == GT_TRUE)
    {
        *hwFormatArrayPtr |= ((BOOL2BIT_MAC(logicFormatPtr->isTunnelStart)) << 13);
        if (logicFormatPtr ->tunnelStartInfo.ptr >= BIT_12)
        {
            return GT_BAD_PARAM;
        }
        *hwFormatArrayPtr |= (logicFormatPtr ->tunnelStartInfo.ptr << 14);
        switch(logicFormatPtr->tunnelStartInfo.passengerPacketType)
        {
        case CPSS_DXCH_TUNNEL_PASSENGER_ETHERNET_E:
            *hwFormatArrayPtr |= (0 << 26);
            break;
        case CPSS_DXCH_TUNNEL_PASSENGER_OTHER_E:
            *hwFormatArrayPtr |= (1 << 26);
            break;
        default:
            return GT_BAD_PARAM;
        }
    }

    switch (logicFormatPtr->physicalInterface.type)
    {
    case CPSS_INTERFACE_PORT_E:
            if (logicFormatPtr->physicalInterface.devPort.devNum >= BIT_5)
            {
                return GT_BAD_PARAM;
            }
            if (logicFormatPtr->physicalInterface.devPort.portNum >= BIT_6)
            {
                return GT_BAD_PARAM;
            }
            useVidx = 0;
            targetIsTrunk = 0;
            *hwFormatArrayPtr |= useVidx                                                  |
                                 (targetIsTrunk << 1)                                     |
                                 (logicFormatPtr->physicalInterface.devPort.portNum << 2) |
                                (logicFormatPtr->physicalInterface.devPort.devNum << 8);
            /* the remained fields are not relevant */
            break;

    case CPSS_INTERFACE_TRUNK_E:
        if (logicFormatPtr->physicalInterface.trunkId >= BIT_7)
        {
            return GT_BAD_PARAM;
        }
        useVidx = 0;
        targetIsTrunk = 1;
        *hwFormatArrayPtr |=  useVidx              |
                              (targetIsTrunk << 1) |
                              (logicFormatPtr->physicalInterface.trunkId << 6);

        /* the remained fields are not relevant */
        break;

    case CPSS_INTERFACE_VIDX_E:
        if (logicFormatPtr->physicalInterface.vidx >= BIT_12)
        {
            return GT_BAD_PARAM;
        }
        *hwFormatArrayPtr |= 1 | (logicFormatPtr->physicalInterface.vidx << 1);
        /* the remained fields are not relevant */
        break;
    default:
            return GT_BAD_PARAM;
    }
    return GT_OK;
}
/*******************************************************************************
* cpssDxChLogicalTargetMappingEnableSet
*
* DESCRIPTION:
*       Enable/disable logical port mapping feature on the specified device.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
*
* INPUTS:
*       devNum   - physical device number
*       enable   - GT_TRUE:  enable
*                  GT_FALSE: disable .
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
)
{
    GT_U32 regAddr;
    GT_U32 value;
    GT_U32 fieldOffset;
    GT_U32 fieldLength;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    value = (enable == GT_TRUE) ? 1 : 0;
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.preEgrEngineGlobal;
    fieldOffset = 13;
    fieldLength = 1;
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, fieldOffset, fieldLength, value);

}


/*******************************************************************************
* cpssDxChLogicalTargetMappingEnableGet
*
* DESCRIPTION:
*       Get the Enable/Disable status logical port mapping feature on the
*       specified  device.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       enablePtr - Pointer to the enable/disable state.
*                   GT_TRUE : enable,
*                   GT_FALSE: disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingEnableGet
(
    IN   GT_U8     devNum,
    OUT  GT_BOOL   *enablePtr
)
{
    GT_STATUS rc;
    GT_U32    regAddr;
    GT_U32    value = 0;
    GT_U32    fieldOffset;
    GT_U32    fieldLength;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.preEgrEngineGlobal;
    fieldOffset = 13;
    fieldLength = 1;
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, fieldLength, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (value == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}



/*******************************************************************************
* cpssDxChLogicalTargetMappingDeviceEnableSet
*
* DESCRIPTION:
*       Enable/disable a target device to be considered as a logical device
*       on the specified device.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
*
* INPUTS:
*       devNum        - physical device number
*       logicalDevNum - logical device number.
*                       Range (24-31)
*       enable        - GT_TRUE:  enable
*                       GT_FALSE: disable .
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, logicalDevNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingDeviceEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     logicalDevNum,
    IN  GT_BOOL   enable
)
{
    GT_U32 regAddr;
    GT_U32 value;
    GT_U32 fieldOffset;
    GT_U32 fieldLength;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if ( (logicalDevNum < 24) || (logicalDevNum > 31) )
    {
        return GT_BAD_PARAM;
    }

    value = (enable == GT_TRUE) ? 1 : 0;
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.logicalTargetDeviceMappingConfReg;
    fieldOffset = logicalDevNum - 24;
    fieldLength = 1;
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, fieldOffset, fieldLength, value);
}


/*******************************************************************************
* cpssDxChLogicalTargetMappingDeviceEnableGet
*
* DESCRIPTION:
*       Get Enable/disable status of target device to be considered as a logical device
*       on the specified device.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
*
* INPUTS:
*       devNum        - physical device number
*       logicalDevNum - logical device number.
*                       Range (24-31)
*
* OUTPUTS:
*       enablePtr     - Pointer to the  Enable/disable state.
*                       GT_TRUE : enable,
*                       GT_FALSE: disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, logicalDevNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingDeviceEnableGet
(
    IN   GT_U8     devNum,
    IN  GT_U8      logicalDevNum,
    OUT  GT_BOOL   *enablePtr
)
{
    GT_STATUS rc;
    GT_U32    regAddr;
    GT_U32    value = 0;
    GT_U32    fieldOffset;
    GT_U32    fieldLength;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    if ( (logicalDevNum < 24) || (logicalDevNum > 31) )
    {
        return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.logicalTargetDeviceMappingConfReg;
    fieldOffset = logicalDevNum - 24;
    fieldLength = 1;
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, fieldLength, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (value == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChLogicalTargetMappingTableEntrySet
*
* DESCRIPTION:
*       Set logical target mapping table entry.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum                       - physical device number.
*       logicalDevNum                - logical device number.
*                                      Range (24-31)
*       logicalPortNum               - logical port number.
*                                      Range (0-63)
*       logicalPortMappingTablePtr   - points to logical Port Mapping  entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum, logicalDevNum, logicalPortNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - on the memebers of virtual port entry struct out of range.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingTableEntrySet
(
    IN GT_U8                           devNum,
    IN GT_U8                           logicalDevNum,
    IN GT_U8                           logicalPortNum,
    IN CPSS_DXCH_OUTPUT_INTERFACE_STC  *logicalPortMappingTablePtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U32 hwData = 0;
    GT_U32 entryIndex = 0;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(logicalPortMappingTablePtr);

    if ( (logicalDevNum < 24) || (logicalDevNum > 31) )
    {
        return GT_BAD_PARAM;
    }
    if (logicalPortNum > 63)
    {
        return GT_BAD_PARAM;
    }

    /* Converts a given Logical Target Mapping Table Entry from logic format
       to hardware format.
    */
    rc = prvCpssDxChLogicalTargetLogic2HwFormat(logicalPortMappingTablePtr, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Calculate entry index of Logical Target Mapping Table */
    entryIndex = ((logicalDevNum - 24) << 6) | logicalPortNum ;

    /* set Logical Target Mapping Table Entry */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_XCAT_TABLE_LOGICAL_TARGET_MAPPING_E,
                                         entryIndex, /* entryIndex   */
                                         0,          /* fieldWordNum */
                                         0,          /* fieldOffset  */
                                         27,         /* fieldLength  */
                                         hwData);    /* fieldValue   */
    return rc;
}

/*******************************************************************************
* cpssDxChLogicalTargetMappingTableEntryGet
*
* DESCRIPTION:
*       Get logical target mapping table entry.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum                       - physical device number.
*       logicalDevNum                - logical device number.
*                                      Range (24-31)
*       logicalPortNum               - logical port number.
*                                      Range (0-63)
*
* OUTPUTS:
*       logicalPortMappingTablePtr   - points to logical Port Mapping  entry
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum, logicalDevNum, logicalPortNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - on the memebers of virtual port entry struct out of range.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChLogicalTargetMappingTableEntryGet
(
    IN GT_U8                            devNum,
    IN GT_U8                            logicalDevNum,
    IN GT_U8                            logicalPortNum,
    OUT CPSS_DXCH_OUTPUT_INTERFACE_STC  *logicalPortMappingTablePtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U32 hwData = 0;
    GT_U32 entryIndex = 0;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(logicalPortMappingTablePtr);

    if ( (logicalDevNum < 24) || (logicalDevNum > 31) )
    {
        return GT_BAD_PARAM;
    }
    if (logicalPortNum > 63)
    {
        return GT_BAD_PARAM;
    }
    /* Calculate entry index of Logical Target Mapping Table */
    entryIndex = ((logicalDevNum - 24) << 6) | logicalPortNum ;
    rc =  prvCpssDxChReadTableEntryField (devNum,
                                          PRV_CPSS_DXCH_XCAT_TABLE_LOGICAL_TARGET_MAPPING_E,
                                          entryIndex,   /* entryIndex   */
                                          0,            /* fieldWordNum */
                                          0,            /* fieldOffset  */
                                          27,           /* fieldLength  */
                                          &hwData);     /* fieldValue   */
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Converts a given Logical Target Mapping Table Entry from hardware format
       to logic format.
    */
    rc = prvCpssDxChLogicalTargetHw2LogicFormat(&hwData, logicalPortMappingTablePtr);
    return rc;
}


