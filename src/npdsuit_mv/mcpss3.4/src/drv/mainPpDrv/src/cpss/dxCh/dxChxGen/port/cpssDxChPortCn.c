/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortCn.c
*
* DESCRIPTION:
*       CPSS implementation for Port Congestion Notification API.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCn.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPort.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h>

/*******************************************************************************
* cpssDxChPortCnModeEnableSet
*
* DESCRIPTION:
*       Enable/Disable Congestion Notification (CN) mechanism.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum     - device number.
*       enable     - GT_TRUE: Enable congestion notification mechanism.
*                    GT_FALSE: Disable congestion notification mechanism.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnModeEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32 regAddr;          /* register address */
    GT_STATUS rc;            /* function return value */
    GT_U32 controlRegVal;    /* BCN Control register value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {

        if(GT_FALSE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                                 PRV_CPSS_DXCH_LION_RM_QCN_FOR_CN_FORMAT_WA_E))
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                globalRegs.extendedGlobalControl2;

            /* set CnFrameFormat */
            rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 29, 1, BOOL2BIT_MAC(enable));
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.congestNotification.cnGlobalConfigReg;

        /* set CnEn */
        rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 1, BOOL2BIT_MAC(enable));
        if (rc != GT_OK)
        {
            return rc;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiUnitConfig;
    
        /* set CNM Termination Mode in TTI */
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 18, 2, BOOL2BIT_MAC(enable));
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        if (enable == GT_TRUE)
        {
            controlRegVal = 0xD;
        }
        else
        {
            controlRegVal = 0x0;
        }
    
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.bcnControlReg;
    
        /* set BCN Control register */
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 4, controlRegVal);
        if (rc != GT_OK)
        {
            return rc;
        }
    
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiUnitConfig;
    
        /* set Enable/Disable identification of CCFC frames in TTI */
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 19, 1, BOOL2BIT_MAC(enable));
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnModeEnableGet
*
* DESCRIPTION:
*       Gets the current status of Congestion Notification (CN) mode.
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
*       enablePtr  - (pointer to) status of Congestion Notification mode
*                     GT_TRUE:  Congestion Notification mode enabled.
*                     GT_FALSE: Congestion Notification mode disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnModeEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32    value;     /* register value */
    GT_U32    regAddr;   /* register address */
    GT_STATUS rc;        /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.congestNotification.cnGlobalConfigReg;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.bcnControlReg;
    }

    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 1, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnProfileSet
*
* DESCRIPTION:
*       Binds a port to a Congestion Notification profile.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - port number.
*       profileIndex - profile index (0..7).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_OUT_OF_RANGE          - on out of range profile index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. To disable port for triggering CN frames bind port to profile with
*          all thresholds set to maximum.
*       2. On Lion device CN Profile is part of the Tail-Drop Profile.
*          To bind port to Tail-Drop Profile use cpssDxChPortTxBindPortToDpSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnProfileSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_U32   profileIndex
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 fieldOffset; /* the start bit number in the register */
    GT_U32 portGroupId; /* the port group Id - support multi-port-groups device */
    GT_U8  localPort;   /* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    if (profileIndex > 7 )
    {
        return GT_OUT_OF_RANGE;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.
                portsBcnProfileReg0 + 4 * (localPort / 10);

    fieldOffset = (localPort % 10) * 3;

    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                     fieldOffset, 3, profileIndex);
}

/*******************************************************************************
* cpssDxChPortCnProfileGet
*
* DESCRIPTION:
*       Gets the port's Congestion Notification profile.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*
* OUTPUTS:
*       profileIndexPtr - (pointer to) profile index.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnProfileGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_U32  *profileIndexPtr
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 fieldOffset; /* the start bit number in the register */
    GT_U32 portGroupId; /* the port group Id - support multi-port-groups device */
    GT_U8  localPort;   /* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(profileIndexPtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.
                portsBcnProfileReg0 + 4 * (localPort / 10);

    fieldOffset = (localPort % 10) * 3;

    return prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                     fieldOffset, 3, profileIndexPtr);
}

/*******************************************************************************
* cpssDxChPortCnProfileQueueThresholdSet
*
* DESCRIPTION:
*       Sets the buffer threshold for triggering CN frame for a given tc queue.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum       - device number.
*       profileIndex - profile index (0..7).
*       tcQueue      - traffic class queue (0..7).
*       threshold    - buffer limit threshold for triggering CN frame on a given
*                      tc queue (0..0x1FFF).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_OUT_OF_RANGE          - on out of range threshold value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To bind port to CN profile in Lion use
*          cpssDxChPortCnProfileQueueConfigSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnProfileQueueThresholdSet
(
    IN GT_U8    devNum,
    IN GT_U32   profileIndex,
    IN GT_U8    tcQueue,
    IN GT_U32   threshold
)
{
    GT_STATUS rc;    /* function return value */
    GT_U32 offset;   /* offset from the beginning of the entry */
    GT_U32 temp;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);

    if (profileIndex > 7 )
    {
        return GT_BAD_PARAM;
    }

    if (threshold > 0x1FFF)
    {
        return GT_OUT_OF_RANGE;
    }

    offset = tcQueue * 13;

    temp = 32 - offset%32;

    if (temp >= 13)
    {
        return prvCpssDxChWriteTableEntryField(devNum,PRV_CPSS_DXCH_XCAT_TABLE_BCN_PROFILES_E,
                                               profileIndex,offset /32 ,offset % 32,13,threshold);
    }
    else
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,PRV_CPSS_DXCH_XCAT_TABLE_BCN_PROFILES_E,
                                               profileIndex,offset /32 ,offset % 32,temp,threshold);
        if (rc != GT_OK)
        {
            return rc;
        }

        return prvCpssDxChWriteTableEntryField(devNum,PRV_CPSS_DXCH_XCAT_TABLE_BCN_PROFILES_E,
                                               profileIndex,(offset /32)+1 ,0,13 - temp,threshold>>temp);

    }
}

/*******************************************************************************
* cpssDxChPortCnProfileQueueThresholdGet
*
* DESCRIPTION:
*       Gets the buffer threshold for triggering CN frame for a given tc queue.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum       - device number.
*       profileIndex - profile index (0..7).
*       tcQueue      - traffic class queue (0..7).
*
* OUTPUTS:
*       thresholdPtr - (pointer to) buffer limit threshold for
*                      triggering CN frame on a given tc queue.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnProfileQueueThresholdGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   profileIndex,
    IN  GT_U8    tcQueue,
    OUT GT_U32   *thresholdPtr
)
{
    GT_STATUS rc;     /* function return value */
    GT_U32 value;     /* register value */
    GT_U32 offset;
    GT_U32 temp;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);
    CPSS_NULL_PTR_CHECK_MAC(thresholdPtr);

    if (profileIndex > 7 )
    {
        return GT_BAD_PARAM;
    }

    offset = tcQueue * 13;

    temp = 32 - offset%32;

    if (temp >= 13)
    {
        return prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH_XCAT_TABLE_BCN_PROFILES_E,
                                              profileIndex,offset/32,offset%32,13,thresholdPtr);
    }
    else
    {
        rc = prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH_XCAT_TABLE_BCN_PROFILES_E,
                                            profileIndex,offset/32,offset%32,temp,&value);
        if (rc != GT_OK)
        {
            return rc;
        }
        *thresholdPtr = value;
        rc = prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH_XCAT_TABLE_BCN_PROFILES_E,
                                            profileIndex,(offset/32)+1,0,13-temp,&value);
        if (rc != GT_OK)
        {
            return rc;
        }
        *thresholdPtr |= (value << temp);
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnProfileThresholdSet
*
* DESCRIPTION:
*       Sets the buffer threshold for triggering CN frame.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum       - device number.
*       profileIndex - profile index (0..7).
*       threshold    - buffer limit threshold
*                      for triggering CN frame (0..0x1FFF).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or profile index
*       GT_OUT_OF_RANGE          - on out of range threshold value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnProfileThresholdSet
(
    IN GT_U8    devNum,
    IN GT_U32   profileIndex,
    IN GT_U32   threshold
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E );

    if (profileIndex > 7 )
    {
        return GT_BAD_PARAM;
    }

    if (threshold > 0x1FFF)
    {
        return GT_OUT_OF_RANGE;
    }

    return prvCpssDxChWriteTableEntryField(devNum,PRV_CPSS_DXCH_XCAT_TABLE_BCN_PROFILES_E,
                                              profileIndex,3,8,13,threshold);
}

/*******************************************************************************
* cpssDxChPortCnProfileThresholdGet
*
* DESCRIPTION:
*       Gets the buffer threshold for triggering CN frame.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum       - device number.
*       profileIndex - profile index (0..7).
*
* OUTPUTS:
*       thresholdPtr - (pointer to) buffer limit threshold for
*                     triggering CN frame.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number or profile index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnProfileThresholdGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  profileIndex,
    OUT GT_U32  *thresholdPtr
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    CPSS_NULL_PTR_CHECK_MAC(thresholdPtr);

    if (profileIndex > 7 )
    {
        return GT_BAD_PARAM;
    }

    return prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH_XCAT_TABLE_BCN_PROFILES_E,
                                              profileIndex,3,8,13,thresholdPtr);
}

/*******************************************************************************
* cpssDxChPortCnQueueAwareEnableSet
*
* DESCRIPTION:
*       Enable/Disable Congestion Notification awareness on a given
*       tc queue.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum     - device number.
*       tcQueue    - traffic class queue (0..7).
*       enable     - GT_TRUE: CN aware. Packets enqueued to tcQueue are enabled to
*                    trigger Congestion Notification frames.
*                    GT_FALSE: CN blind. Packets enqueued to tcQueue never trigger
*                    Congestion Notification frames.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnQueueAwareEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    tcQueue,
    IN GT_BOOL  enable
)
{
    GT_U32 regAddr;   /* register address */
    GT_U32 value;     /* register value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);

    value = BOOL2BIT_MAC(enable);
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.bcnControlReg;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 4 + tcQueue, 1, value);
}

/*******************************************************************************
* cpssDxChPortCnQueueAwareEnableGet
*
* DESCRIPTION:
*       Gets the status of Congestion Notification awareness on a given
*       tc queue.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum     - device number.
*       tcQueue    - traffic class queue (0..7).
* OUTPUTS:
*       enablePtr  - (pointer to) status of Congestion Notification awareness
*                    GT_TRUE: CN aware. Packets enqueued to tcQueue are enabled to
*                    trigger Congestion Notification frames.
*                    GT_FALSE: CN blind. Packets enqueued to tcQueue never trigger
*                    Congestion Notification frames.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnQueueAwareEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    tcQueue,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32 regAddr;   /* register address */
    GT_U32 value;     /* register value */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.bcnControlReg;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 4 + tcQueue, 1, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnFrameQueueSet
*
* DESCRIPTION:
*       Set traffic class queue associated with CN frames generated by device.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum     - device number.
*       tcQueue    - traffic class queue associated with CN frames (0..7).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CN frames must be enqueued to priority queues disabled to trigger CN.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnFrameQueueSet
(
    IN GT_U8    devNum,
    IN GT_U8    tcQueue
)
{
    GT_U32 regAddr;   /* register address */
    GT_U32 fieldOffset;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.congestNotification.cnGlobalConfigReg;
        fieldOffset = 1;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.bcnControlReg;
        fieldOffset = 12;
    }

    return prvCpssDxChHwPpSetRegField(devNum, regAddr, fieldOffset, 3, tcQueue);
}

/*******************************************************************************
* cpssDxChPortCnFrameQueueGet
*
* DESCRIPTION:
*       Get traffic class queue associated with CN frames generated by device.
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
*       tcQueuePtr - (pointer to) traffic class queue associated with CN frames.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnFrameQueueGet
(
    IN  GT_U8    devNum,
    OUT GT_U8    *tcQueuePtr
)
{
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */
    GT_U32 value;     /* register value */
    GT_U32 fieldOffset;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(tcQueuePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.congestNotification.cnGlobalConfigReg;
        fieldOffset = 1;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.bcnControlReg;
        fieldOffset = 12;
    }

    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, fieldOffset, 3, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    *tcQueuePtr = (GT_U8)value;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnEtherTypeSet
*
* DESCRIPTION:
*       Sets the EtherType to identify CN frames.
*       This EtherType also inserted in the CN header of generated CN frames.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number.
*       etherType   - EtherType for CN frames (0..0xFFFF).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range etherType values.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       The CN EtherType must be unique in the system.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnEtherTypeSet
(
    IN GT_U8    devNum,
    IN GT_U32   etherType
)
{
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if(etherType > 0xFFFF)
    {
        return GT_OUT_OF_RANGE;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ccfcEthertype;
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 16, etherType);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCnmHeaderConfig;
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 16, etherType);
}

/*******************************************************************************
* cpssDxChPortCnEtherTypeGet
*
* DESCRIPTION:
*       Gets the EtherType for CN frames. The EtherType inserted in the CN
*       header.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum       - device number.
*
* OUTPUTS:
*       etherTypePtr - (pointer to) EtherType for CN frames.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnEtherTypeGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *etherTypePtr
)
{
    GT_U32    regAddr;   /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(etherTypePtr);

    /* we are reading only ccfcEthertype register because it reflects the
       value in the hdrAltCnmHeaderConfig as well */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ccfcEthertype;
    return prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 16, etherTypePtr);
}

/*******************************************************************************
* cpssDxChPortCnSpeedIndexSet
*
* DESCRIPTION:
*       Sets port speed index of the egress port. The device inserts this index
*       in the generated CCFC frame.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum         - device number.
*       portNum        - port number.
*       portSpeedIndex - port speed index (0..7).
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_OUT_OF_RANGE          - on out of range port speed index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Panic Pause uses Port Speed Index 0 and 7 to indicate XON and XOFF.
*          Congested ports should use Port Speed Index 1 to 6 if
*          Panic Pause is used.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnSpeedIndexSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_U32   portSpeedIndex
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 fieldOffset; /* the start bit number in the register */
    GT_U32 portGroupId; /* the port group Id - support multi-port-groups device */
    GT_U8  localPort;   /* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    if (portSpeedIndex > 7 )
    {
        return GT_OUT_OF_RANGE;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.
                portsSpeedIndexesReg0 + 4 * (localPort / 10);

    fieldOffset = (localPort % 10) * 3;

    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                     fieldOffset, 3, portSpeedIndex);
}

/*******************************************************************************
* cpssDxChPortSpeedIndexGet
*
* DESCRIPTION:
*       Gets port speed index of the egress port. The device inserts this index
*       in the generated CCFC frame.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum            - device number.
*       portNum    -      - port number.
*
* OUTPUTS:
*       portSpeedIndexPtr - (pointer to) port speed index.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnSpeedIndexGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT GT_U32   *portSpeedIndexPtr
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 fieldOffset; /* the start bit number in the register */
    GT_U32 portGroupId; /* the port group Id - support multi-port-groups device */
    GT_U8  localPort;   /* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(portSpeedIndexPtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.
                portsSpeedIndexesReg0 + 4 * (localPort / 10);

    fieldOffset = (localPort % 10) * 3;

    return prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                     fieldOffset, 3, portSpeedIndexPtr);
}


/*******************************************************************************
* cpssDxChPortCnFcEnableSet
*
* DESCRIPTION:
*       Enable/Disable generation of flow control packets as result of CN
*       frame termination or Panic Pause.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*       enable     - GT_TRUE:  enable. Flow control packets can be issued.
*                    GT_FALSE: disable. Flow control packets can't be issued.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnFcEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  enable
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 fieldOffset; /* the start bit number in the register */
    GT_U32 value;       /* register value */
    GT_STATUS rc;       /* function return value */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    value = BOOL2BIT_MAC(enable);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E ||
        PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
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
                PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,portMacType,&regAddr);
                fieldOffset = 15;
            }
            else
            {
                PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portNum,portMacType,&regAddr);
                fieldOffset = 2;
            }

            if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
            {
                rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,fieldOffset, 1, value);

                if (rc != GT_OK) 
                {
                    return rc;
                }
            }
#ifdef __AX_PLATFORM__
    	}
#endif
    }
    else /*lion*/
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.congestNotification.cnFcEn[OFFSET_TO_WORD_MAC(portNum)];
        fieldOffset = OFFSET_TO_BIT_MAC(portNum);
    
        rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, fieldOffset, 1, value);
        if (rc != GT_OK)
        {
            return rc;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.egr.filterConfig.egrCnFcEn[OFFSET_TO_WORD_MAC(portNum)];

        rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, fieldOffset, 1, value);
        if (rc != GT_OK ) 
        {
            return rc;
        }
    }
    return GT_OK;

}

/*******************************************************************************
* cpssDxChPortCnFcEnableGet
*
* DESCRIPTION:
*       Gets the status of generation of flow control packets as result of CN
*       frame termination or Panic Pause on a given port.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE:  enable. Flow control packets is issued.
*                    GT_FALSE: disable. Flow control packets is not issued.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnFcEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 fieldOffset;/* the start bit number in the register */
    GT_U32 value;      /* register value */
    GT_STATUS rc;      /* function return value */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E ||
       PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

        if(portMacType >= PRV_CPSS_PORT_XG_E)
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,portMacType,&regAddr);
            fieldOffset = 15;
        }
        else
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portNum,portMacType,&regAddr);
            fieldOffset = 2;
        }
    
        rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, 1, &value);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else /*lion*/
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
         txqVer1.egr.filterConfig.egrCnFcEn[OFFSET_TO_WORD_MAC(portNum)];
        fieldOffset = OFFSET_TO_BIT_MAC(portNum);

        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr,fieldOffset, 1, &value);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnPauseTriggerEnableSet
*
* DESCRIPTION:
*       Enable the CN triggering engine to trigger a pause frame.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*       enable     - GT_TRUE:  enable.
*                    GT_FALSE: disable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        To enable proper operation of CCFC, this configuration must be
*        enabled for all network ports and disabled for all cascade ports.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnPauseTriggerEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  enable
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 portGroupId;     /* the port group Id - support multi-port-groups device */
    GT_U32 value;      /* register value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    value = BOOL2BIT_MAC(enable);
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.bcnPauseTriggerEnableReg;


    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, portNum, 1, value);
}

/*******************************************************************************
* cpssDxChPortCnPauseTriggerEnableGet
*
* DESCRIPTION:
*       Gets the status triggering engine a pause frame on a given port.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE:  enable.
*                    GT_FALSE: disable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnPauseTriggerEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 portGroupId;     /* the port group Id - support multi-port-groups device */
    GT_U32 value;      /* register value */
    GT_STATUS rc;      /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.bcnPauseTriggerEnableReg;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,portNum, 1, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnFcTimerSet
*
* DESCRIPTION:
*       Sets timer value for the IEEE 802.3x/PFC frame issued as result
*       of CN frame termination.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum         - device number.
*       portNum        - port number.
*       index          - index (0..7)
*       timer          - 802.3x/PFC pause time (0..0xFFFF).
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, device or speed index
*       GT_OUT_OF_RANGE          - on out of range pause time value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. xCat: Panic Pause uses Port Speed Index 0 and 7 to indicate
*          XON and XOFF. Port timer configurations 0 and 7 must be configured
*          to 0x0 and 0xFFFF if Panic Pause is used.
*       2. xCat: index is always congested port speed index (CCFC mode).
*       3. Lion, xCat2: index determined by 
*          cpssDxChPortCnPrioritySpeedLocationSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnFcTimerSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_U32   index,
    IN GT_U32   timer
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 portGroupId;     /* the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    if (index > 7 )
    {
        return GT_BAD_PARAM;
    }

    if (timer > 0xFFFF)
    {
        return GT_OUT_OF_RANGE;
    }

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_XG_E)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
            perPortRegs[portNum].ccfcFcTimerBaseXg;

        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
        {
            timer = (timer >> 8) | ((timer & 0xFF) << 8);
        }
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
            perPortRegs[portNum].ccfcFcTimerBaseGig;
    }

    regAddr += index*4;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,0, 16, timer);
}

/*******************************************************************************
* cpssDxChPortCnFcTimerGet
*
* DESCRIPTION:
*       Gets timer value for the IEEE 802.3x/PFC frame issued as result
*       of CN frame termination.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum         - device number.
*       portNum        - port number.
*       index          - index (0..7).
*
* OUTPUTS:
*       timerPtr       - (pointer to) 802.3x/PFC Pause time
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number, device or speed index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. xCat: index is always congested port speed index (CCFC mode).
*       2. Lion, xCat2: index determined by 
*          cpssDxChPortCnPrioritySpeedLocationSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnFcTimerGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    IN  GT_U32   index,
    OUT GT_U32   *timerPtr
)
{
    GT_U32 regAddr;  /* register address */
    GT_U32 portGroupId;   /* the port group Id - support multi-port-groups device */
    GT_STATUS rc;    /* function return value */
    GT_U32 value;    /* register value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(timerPtr);

    if (index > 7 )
    {
        return GT_BAD_PARAM;
    }

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) >= PRV_CPSS_PORT_XG_E)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
            perPortRegs[portNum].ccfcFcTimerBaseXg;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
            perPortRegs[portNum].ccfcFcTimerBaseGig;
    }

    regAddr += index*4;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,0, 16, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_XG_E &&
      (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E))
    {
        *timerPtr = (value >> 8) | ((value & 0xFF) << 8);
    }
    else
    {
        *timerPtr = value;
    }
    return GT_OK;
}


/*******************************************************************************
* cpssDxChPortCnPanicPauseThresholdsSet
*
* DESCRIPTION:
*       Sets the thresholds for triggering a Panic Pause: Global XOFF/XON frame
*       over all ports enabled to send a Panic Pause.
*       A global XOFF frame is triggered if the number of buffers occupied
*       by the CN aware frames crosses up xoffLimit. A global XON frame is
*       triggered if the number of buffers occupied by the CN aware frames
*       crosses down xonLimit.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum      - device number.
*       enable      - GT_TRUE: Enable Panic Pause.
*                     GT_FALSE: Disable Panic Pause.
*       xoffLimit   - X-OFF limit (0..1FFF).Relevant only if enable==GT_TRUE.
*       xonLimit    - X-ON limit (0..1FFF).Relevant only if enable==GT_TRUE.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range xoffLimit or
*                                  xonLimit value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Panic Pause uses Port Speed Index 0 and 7 to indicate XON and XOFF.
*          Port timer configurations 0 and 7 must be configured
*           to 0x0 and 0xFFFF, see cpssDxChPortCnFcTimerSet.
*          Congested ports should use Port Speed Index 1 to 6,
*          see cpssDxChPortCnSpeedIndexSet.
*       2. Panic Pause cannot operate together with the IEEE 802.3x triggers.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnPanicPauseThresholdsSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable,
    IN GT_U32   xoffLimit,
    IN GT_U32   xonLimit
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    /* for disable set to maximum value */
    if (enable == GT_FALSE)
    {
        xoffLimit = 0x1FFF;
        xonLimit = 0x1FFF;
    }
    if (xoffLimit > 0x1FFF || xonLimit > 0x1FFF)
    {
        return GT_OUT_OF_RANGE;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.bcnGlobalPauseThresholdsReg;
    value = xoffLimit | xonLimit << 13;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 26, value);
}

/*******************************************************************************
* cpssDxChPortCnPanicPauseThresholdsGet
*
* DESCRIPTION:
*       Gets the thresholds for triggering a Panic Pause: Global XOFF/XON frame
*       over all ports enabled to send a Panic Pause.
*       A global XOFF frame is triggered if the number of buffers occupied
*       by the CN aware frames crosses up xoffLimit. A global XON frame is
*       triggered if the number of buffers occupied by the CN aware frames
*       crosses down xonLimit.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum       - device number.
*
* OUTPUTS:
*       enablePtr    - (pointer to):
*                      GT_TRUE: Enable Panic Pause.
*                      GT_FALSE: Disable Panic Pause.
*       xoffLimitPtr - (pointer to) the X-OFF limit value.
*                      Relevant only if *enablePtr==GT_TRUE.
*       xonLimitPtr  - (pointer to) the X-ON limit value.
*                      Relevant only if *enablePtr==GT_TRUE.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnPanicPauseThresholdsGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr,
    OUT GT_U32   *xoffLimitPtr,
    OUT GT_U32   *xonLimitPtr
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(xoffLimitPtr);
    CPSS_NULL_PTR_CHECK_MAC(xonLimitPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.bcnGlobalPauseThresholdsReg;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 26, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    *enablePtr = (value == 0x3FFFFFF)? GT_FALSE:GT_TRUE;
    *xoffLimitPtr = value & 0x1FFF;
    *xonLimitPtr = value >> 13;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnTerminationEnableSet
*
* DESCRIPTION:
*       Enable/Disable termination of CNM (Congestion Notification Message) on given port.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*       enable     - GT_TRUE: enable. CNM is terminated, i.e not transmited on this port.
*                    GT_FALSE: disable. CNM isn't terminated, i.e transmited on this port.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnTerminationEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  enable
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* register value */
    GT_STATUS rc;       /* function return value */
    GT_U32 fieldOffset;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    value = (enable == GT_TRUE)? 0 : 1;
    
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.queue.congestNotification.cnEnCnFrameTx[OFFSET_TO_WORD_MAC(portNum)];
    fieldOffset = OFFSET_TO_BIT_MAC(portNum);

    rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, fieldOffset, 1, value);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.egr.filterConfig.egrCnEn[OFFSET_TO_WORD_MAC(portNum)];

    return prvCpssDxChHwPpSetRegField(devNum, regAddr, fieldOffset, 1, value);
}

/*******************************************************************************
* cpssDxChPortCnTerminationEnableGet
*
* DESCRIPTION:
*       Gets the status of CNM termination on given port.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE: enable. CNM is terminated, i.e not transmited on this port.
*                    GT_FALSE: disable. CNM isn't terminated, i.e transmited on this port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnTerminationEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* register value */
    GT_STATUS rc;       /* function return value */
    GT_U32 fieldOffset;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.egr.filterConfig.egrCnEn[OFFSET_TO_WORD_MAC(portNum)];
    fieldOffset = OFFSET_TO_BIT_MAC(portNum);

    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr,fieldOffset, 1, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    *enablePtr = (value == 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnProfileQueueConfigSet
*
* DESCRIPTION:
*       Sets CN profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum          - device number.
*       profileSet      - profile set.
*       tcQueue         - traffic class queue (0..7).
*       cnProfileCfgPtr - pointer to CN Profile configurations.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number, profile set
*                                  or traffic class queue
*       GT_OUT_OF_RANGE          - on out of range threshold value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CN Profile is part of the Tail-Drop Profile.
*       To bind port to Tail-Drop Profile use cpssDxChPortTxBindPortToDpSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnProfileQueueConfigSet
(
    IN GT_U8                               devNum,
    IN CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet,
    IN GT_U8                               tcQueue,
    IN CPSS_PORT_CN_PROFILE_CONFIG_STC     *cnProfileCfgPtr
)
{
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */
    GT_U32 regOffset; /* register offset */
    GT_U32 fieldOffset; /* field offset */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);
    CPSS_NULL_PTR_CHECK_MAC(cnProfileCfgPtr);
    PRV_CPSS_DXCH_DROP_PROFILE_CHECK_MAC(profileSet,devNum);

    if (cnProfileCfgPtr->threshold > 0x1FFF)
    {
        return GT_OUT_OF_RANGE;
    }
    /* set CN awareness */
    regOffset = (profileSet/4) * 0x4;
    fieldOffset = (profileSet%4)*8 + tcQueue;
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.queue.congestNotification.cnPrioQueueEnProfile + regOffset;
    rc = prvCpssDxChHwPpSetRegField(devNum, regAddr,fieldOffset, 1, 
                                   BOOL2BIT_MAC(cnProfileCfgPtr->cnAware));
    if (rc != GT_OK)
    {
        return rc;
    }

    /* set threshold */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.queue.congestNotification.cnProfileThreshold[tcQueue][profileSet];
    rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 13, cnProfileCfgPtr->threshold);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnProfileQueueConfigGet
*
* DESCRIPTION:
*       Gets CN profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       profileSet   - profile set.
*       tcQueue      - traffic class queue (0..7).
*
* OUTPUTS:
*       cnProfileCfgPtr - pointer to CN Profile configurations.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number, profile set
*                                  or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnProfileQueueConfigGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet,
    IN  GT_U8                               tcQueue,
    OUT CPSS_PORT_CN_PROFILE_CONFIG_STC     *cnProfileCfgPtr
)
{
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */
    GT_U32 regOffset;    /* register offset */
    GT_U32 fieldOffset;  /* field offset */
    GT_U32 value;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);
    CPSS_NULL_PTR_CHECK_MAC(cnProfileCfgPtr);
    PRV_CPSS_DXCH_DROP_PROFILE_CHECK_MAC(profileSet,devNum);

    /* get CN awareness */
    regOffset = (profileSet/4) * 0x4;
    fieldOffset = (profileSet%4)*8 + tcQueue;
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.queue.congestNotification.cnPrioQueueEnProfile + regOffset;
    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, fieldOffset, 1, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    cnProfileCfgPtr->cnAware = BIT2BOOL_MAC(value);

    regOffset = profileSet*0x20 + tcQueue* 0x4;

    /* get threshold */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.queue.congestNotification.cnProfileThreshold[tcQueue][profileSet];
    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 13, &(cnProfileCfgPtr->threshold));
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnQueueStatusModeEnableSet
*
* DESCRIPTION:
*       Enable/Disable queue status mode. When enabled, CNM is sent to a
*       configurable target that examines the queue status.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       enable     - GT_TRUE: CNM is sent to a targetDev/targetPort.
*                    GT_FALSE: CNM is sent to a sampled packet's source.
*       targetDev  - Target device for the CNM (0..31).
*                    Relevant only if enable==GT_TRUE.
*       targetPort - Target port for the CNM (0..63).
*                    Relevant only if enable==GT_TRUE.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range targetDev or targetPort
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnQueueStatusModeEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable,
    IN GT_U8    targetDev,
    IN GT_U8    targetPort
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    if (enable == GT_TRUE)
    {
        if (targetDev > 31 || targetPort > 63)
        {
            return GT_OUT_OF_RANGE;
        }

        value = 1 | (targetDev & 0x1F) << 1 | (targetPort & 0x3F) << 6;
    }
    else
    {
        value = 0;
    }
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.congestNotification.cnGlobalConfigReg;
    return prvCpssDxChHwPpSetRegField(devNum, regAddr, 8, 12, value);
}

/*******************************************************************************
* cpssDxChPortCnQueueStatusModeEnableGet
*
* DESCRIPTION:
*        Gets the current status of queue status mode.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       enablePtr     - (pointer to) status of queue status mode.
*                       GT_TRUE: CNM is sent to a targetDev/targetPort.
*                       GT_FALSE: CNM is sent to a sampled packet's source.
*       targetDevPtr  - (pointer to) Target device for the CNM.
*                                    Relevant only if enable==GT_TRUE.
*       targetPortPtr - (pointer to) Target port for the CNM.
*                                    Relevant only if enable==GT_TRUE.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnQueueStatusModeEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr,
    OUT GT_U8    *targetDevPtr,
    OUT GT_U8    *targetPortPtr
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(targetDevPtr);
    CPSS_NULL_PTR_CHECK_MAC(targetPortPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.congestNotification.cnGlobalConfigReg;
    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 8, 12, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    *enablePtr = BIT2BOOL_MAC(value & 0x1);
    *targetDevPtr = (GT_U8)((value >> 1) & 0x1F);
    *targetPortPtr = (GT_U8)((value >> 6) & 0x3f);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnSampleEntrySet
*
* DESCRIPTION:
*       Sets CN sample interval entry.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum - device number.
*       entryIndex  - index: qFb 3 msb. (0..7)
*       entryPtr   - (pointer to) entry settings.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number, or entry index
*       GT_OUT_OF_RANGE          - on out of range interval
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.To implement CCFC functionality set entryPtr fields to:
*          interval = (MTU+256b)/16
*          randBitmap = 0
*       2.To achieve uniform distribution of random values, clear the interval
*         to be randomized to 0. For example, if interval = 0xF0 and
*         randBitmap = 0x0F then the sampling interval is a random value
*         in [0xF0,0xFF].
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnSampleEntrySet
(
    IN GT_U8    devNum,
    IN GT_U8    entryIndex,
    IN CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC *entryPtr
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(entryPtr);
    if (entryIndex > 7)
        return GT_BAD_PARAM;

    if (entryPtr->interval > 0xFFFF ||
        entryPtr->randBitmap > 0xFFFF)
    {
        return GT_OUT_OF_RANGE;
    }
    value = entryPtr->interval | entryPtr->randBitmap << 16;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.queue.congestNotification.cnSampleTbl;
    regAddr += entryIndex*0x4;
    return prvCpssDxChHwPpWriteRegister(devNum, regAddr, value);
}

/*******************************************************************************
* cpssDxChPortCnSampleEntryGet
*
* DESCRIPTION:
*       Gets CN sample interval entry.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum - device number.
*       entryIndex  - index: qFb 3 msb. (0..7)
* OUTPUTS:
*       entryPtr   - (pointer to) entry settings.
*
* RETURNS:
*       GT_OK                    - on success
*
*       GT_BAD_PARAM             - on wrong device number, or entry index
*       GT_OUT_OF_RANGE          - on out of range interval
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnSampleEntryGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    entryIndex,
    OUT CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC *entryPtr
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(entryPtr);
    if (entryIndex > 7)
        return GT_BAD_PARAM;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.queue.congestNotification.cnSampleTbl;
    regAddr += entryIndex*0x4;
    rc = prvCpssDxChHwPpReadRegister(devNum, regAddr, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    entryPtr->interval = value & 0xFFFF;
    entryPtr->randBitmap = value >> 16;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnFbCalcConfigSet
*
* DESCRIPTION:
*       Sets Fb (Feedback) calculation configuration.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum - device number.
*       fbCalcCfgPtr  - (pointer to) feedback Calculation configurations.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To implement CCFC functionality set fbCalcCfgPtr fields to:
*          deltaEnable = 0
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnFbCalcConfigSet
(
    IN GT_U8    devNum,
    IN CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC *fbCalcCfgPtr
)
{
    GT_U32 value;     /* register value */
    GT_U32 wExpValue; /* wExp hw value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(fbCalcCfgPtr);

    if (fbCalcCfgPtr->wExp > 7 || fbCalcCfgPtr->wExp < -8 ||
        fbCalcCfgPtr->fbMin > 0xFFFFF || fbCalcCfgPtr->fbMax > 0xFFFFF ||
        fbCalcCfgPtr->fbLsb > 0x1F)
    {
        return GT_OUT_OF_RANGE;
    }

    /* Wexp is given in 2s complement format */
    if (fbCalcCfgPtr->wExp >= 0)
    {
        wExpValue = (GT_U32)fbCalcCfgPtr->wExp;
    }
    else
    {
        wExpValue =(GT_U32)(16 + fbCalcCfgPtr->wExp);
    }

    /* set Feedback Calc Configurations register */
    value = BOOL2BIT_MAC(fbCalcCfgPtr->deltaEnable) |
            fbCalcCfgPtr->fbLsb << 1 |
            wExpValue << 6;
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.queue.congestNotification.fbCalcConfigReg;
    rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 10, value);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* set Feedback MIN register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.queue.congestNotification.fbMinReg;
    rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 20, fbCalcCfgPtr->fbMin);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* set Feedback MAX register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.queue.congestNotification.fbMaxReg;
    return prvCpssDxChHwPpSetRegField(devNum, regAddr, 0, 20, fbCalcCfgPtr->fbMax);

}

/*******************************************************************************
* cpssDxChPortCnFbCalcConfigGet
*
* DESCRIPTION:
*       Gets Fb (Feedback) calculation configuration.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum - device number.
*
* OUTPUTS:
*       fbCalcCfgPtr  - (pointer to) feedback Calculation configurations.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnFbCalcConfigGet
(
    IN  GT_U8    devNum,
    OUT CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC *fbCalcCfgPtr
)
{
    GT_U32 value;     /* register value */
    GT_U32 wExpValue; /* wExp hw value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(fbCalcCfgPtr);

    /* get Feedback Calc Configurations register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.queue.congestNotification.fbCalcConfigReg;
    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 10, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    fbCalcCfgPtr->deltaEnable = BIT2BOOL_MAC(value & 0x1);
    fbCalcCfgPtr->fbLsb = (value >> 1) & 0x1F;

    /* Wexp is given in 2s complement format */
    wExpValue = (value >> 6) & 0xF;
    if (wExpValue < 8)
    {
        fbCalcCfgPtr->wExp = wExpValue;
    }
    else
    {
        fbCalcCfgPtr->wExp = wExpValue - 16;
    }


    /* get Feedback MIN register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.queue.congestNotification.fbMinReg;
    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 20, &(fbCalcCfgPtr->fbMin));
    if (rc != GT_OK)
    {
        return rc;
    }

    /* get Feedback MAX register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.queue.congestNotification.fbMaxReg;
    return prvCpssDxChHwPpGetRegField(devNum, regAddr, 0, 20, &(fbCalcCfgPtr->fbMax));
}

/*******************************************************************************
* cpssDxChPortCnPacketLengthSet
*
* DESCRIPTION:
*       Sets packet length used by the CN frames triggering logic.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       packetLength - packet length.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or packet length.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Configuring a value other than CPSS_DXCH_PORT_CN_LENGTH_ORIG_PACKET_E
*       should be used when the packet length is unknown, e.g. Cut-through mode.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnPacketLengthSet
(
    IN GT_U8                               devNum,
    IN CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT packetLength
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    switch (packetLength)
    {
        case CPSS_DXCH_PORT_CN_LENGTH_ORIG_PACKET_E:
            value = 0;
            break;
        case CPSS_DXCH_PORT_CN_LENGTH_1_5_KB_E:
            value = 1;
            break;
        case CPSS_DXCH_PORT_CN_LENGTH_2_KB_E:
            value = 2;
            break;
        case CPSS_DXCH_PORT_CN_LENGTH_10_KB_E:
            value = 3;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.congestNotification.cnGlobalConfigReg;
    return prvCpssDxChHwPpSetRegField(devNum, regAddr, 6, 2, value);
}

/*******************************************************************************
* cpssDxChPortCnPacketLengthGet
*
* DESCRIPTION:
*       Gets packet length used by the CN frames triggering logic.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       packetLengthPtr - (pointer to) packet length.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnPacketLengthGet
(
    IN  GT_U8    devNum,
    OUT CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT  *packetLengthPtr
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(packetLengthPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.congestNotification.cnGlobalConfigReg;

    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 6, 2, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    *packetLengthPtr = (CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT)value;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnMessageGenerationConfigSet
*
* DESCRIPTION:
*       Sets CNM generation configuration.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum - device number.
*       cnmGenerationCfgPtr - (pointer to) CNM generation configurations.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnMessageGenerationConfigSet
(
    IN GT_U8    devNum,
    IN CPSS_PORT_CNM_GENERATION_CONFIG_STC *cnmGenerationCfgPtr
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_U32 scaleFactorHwValue;
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    CPSS_NULL_PTR_CHECK_MAC(cnmGenerationCfgPtr);

    if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    if (cnmGenerationCfgPtr->qosProfileId > 0x7F ||
        cnmGenerationCfgPtr->isRouted > 1 ||
        (cnmGenerationCfgPtr->overrideUp == GT_TRUE && cnmGenerationCfgPtr->cnmUp > 7) ||
        cnmGenerationCfgPtr->defaultVlanId > 0xFFF ||
       (cnmGenerationCfgPtr->scaleFactor != 256 &&
        cnmGenerationCfgPtr->scaleFactor != 512) ||
        cnmGenerationCfgPtr->version > 0xF ||
        cnmGenerationCfgPtr->flowIdTag > 0xFFFF ||
        cnmGenerationCfgPtr->cpidMsb.l[1] > 0x3FFFFF)

    {
        return GT_OUT_OF_RANGE;
    }
    /* set Congestion Notification Configuration register */
    value = cnmGenerationCfgPtr->defaultVlanId |
            (BOOL2BIT_MAC(cnmGenerationCfgPtr->overrideUp) << 16) |
            ((cnmGenerationCfgPtr->cnmUp & 0x7) << 17) |
            (cnmGenerationCfgPtr->isRouted << 20) |
            (cnmGenerationCfgPtr->qosProfileId << 21) |
            (BOOL2BIT_MAC(cnmGenerationCfgPtr->forceCnTag) << 28);
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCnConfig;
    rc = prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0x1FFF0FFF, value);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* set CNM Header Configuration register*/
    scaleFactorHwValue = (cnmGenerationCfgPtr->scaleFactor == 256) ? 1 : 0;

    value = cnmGenerationCfgPtr->version << 16 |
            scaleFactorHwValue << 23 |
           ((!BOOL2BIT_MAC(cnmGenerationCfgPtr->appendPacket)) << 25);


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCnmHeaderConfig;
    rc = prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0x28F0000, value);
    if (rc != GT_OK)
    {
        return rc;
    }
    /* set CPID Register 0  and CPID Register 1*/
    value = cnmGenerationCfgPtr->cpidMsb.l[0] & 0x3FFFFF;


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCpidReg0;
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0,22, value);
    if (rc != GT_OK)
    {
        return rc;
    }
    value = cnmGenerationCfgPtr->cpidMsb.l[0] >> 22 |
            cnmGenerationCfgPtr->cpidMsb.l[1] << 10 ;
    regAddr += 4;
    rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr,value);
    if (rc != GT_OK)
    {
        return rc;
    }
    /* set QCN CN-tag FlowID register*/
    value = cnmGenerationCfgPtr->flowIdTag;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCnTagFlowId;
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0,16, value);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* set CN Global Configurations register */
    value = BOOL2BIT_MAC(cnmGenerationCfgPtr->cnUntaggedEnable);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.congestNotification.cnGlobalConfigReg;
        rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 22,1, value);
                if (rc != GT_OK)
        {
            return rc;
        }
    }
    else /* XCAT2 */
    {
        if (cnmGenerationCfgPtr->qosProfileId > 0x7F ||
            cnmGenerationCfgPtr->defaultVlanId > 0xFFF)
        {
            return GT_OUT_OF_RANGE;
        }

        /* set Congestion Notification Configuration register */
        value = cnmGenerationCfgPtr->defaultVlanId |
                (cnmGenerationCfgPtr->qosProfileId << 21);
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCnConfig;
        rc = prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0xFE00FFF, value);
        if (rc != GT_OK)
        {
            return rc;
        }

        value = (!BOOL2BIT_MAC(cnmGenerationCfgPtr->appendPacket));
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCnmHeaderConfig;
        rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 25, 1, value);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnMessageGenerationConfigGet
*
* DESCRIPTION:
*       Gets CNM generation configuration.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum - device number.
*
* OUTPUTS:
*       cnmGenerationCfgPtr - (pointer to) CNM generation configurations.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnMessageGenerationConfigGet
(
    IN  GT_U8    devNum,
    OUT CPSS_PORT_CNM_GENERATION_CONFIG_STC *cnmGenerationCfgPtr
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    CPSS_NULL_PTR_CHECK_MAC(cnmGenerationCfgPtr);
    if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    /* get Congestion Notification Configuration register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCnConfig;
    rc = prvCpssDrvHwPpReadRegister(devNum, regAddr,&value);
    if (rc != GT_OK)
    {
        return rc;
    }
    cnmGenerationCfgPtr->defaultVlanId = (GT_U16)(value & 0xFFF);
    cnmGenerationCfgPtr->overrideUp = BIT2BOOL_MAC((value>>16) & 0x1);
    cnmGenerationCfgPtr->cnmUp = (GT_U8)((value >> 17) & 0x7);
    cnmGenerationCfgPtr->isRouted = (GT_U8)((value >> 20) & 0x1);
    cnmGenerationCfgPtr->qosProfileId = (value >> 21) & 0x7F;
    cnmGenerationCfgPtr->forceCnTag = BIT2BOOL_MAC((value>>28) & 0x1);

    /* get CNM Header Configuration register*/
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCnmHeaderConfig;
    rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    cnmGenerationCfgPtr->version = value >> 16 & 0xf;
    cnmGenerationCfgPtr->scaleFactor = ((value >> 23) & 0x1) ? 256 : 512;
    cnmGenerationCfgPtr->appendPacket =  BIT2BOOL_MAC((!(value>>25) & 0x1));

    /* get CPID Register 0  and CPID Register 1*/
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCpidReg0;
    rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    cnmGenerationCfgPtr->cpidMsb.l[0] = value & 0x3FFFFF;
    regAddr += 4;
    rc = prvCpssDrvHwPpReadRegister(devNum, regAddr,&value);
    if (rc != GT_OK)
    {
        return rc;
    }
    cnmGenerationCfgPtr->cpidMsb.l[0] |= (value << 22) & 0xFFC00000;
    cnmGenerationCfgPtr->cpidMsb.l[1] = value >> 10;

    /* get QCN CN-tag FlowID register*/
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCnTagFlowId;
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0,16, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    cnmGenerationCfgPtr->flowIdTag = value;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.congestNotification.cnGlobalConfigReg;
    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 22,1, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    cnmGenerationCfgPtr->cnUntaggedEnable = BIT2BOOL_MAC(value);
    }
    else /* XCAT2 */
    {
        /* get Congestion Notification Configuration register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCnConfig;
        rc = prvCpssDrvHwPpReadRegister(devNum, regAddr,&value);
        if (rc != GT_OK)
        {
            return GT_OK;
        }
        cnmGenerationCfgPtr->defaultVlanId = (GT_U16)(value & 0xFFF);
        cnmGenerationCfgPtr->qosProfileId = (value >> 21) & 0x7F;
    
        /* get CNM Header Configuration register*/
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCnmHeaderConfig;
        rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &value);
        if (rc != GT_OK)
        {
            return rc;
        }

        cnmGenerationCfgPtr->appendPacket =  BIT2BOOL_MAC((!(value>>25) & 0x1));
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnPrioritySpeedLocationSet
*
* DESCRIPTION:
*       Sets location of the priority/speed bits in the CNM header for parsing
*       and generation of CNM.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum        - device number.
*       prioritySpeedLocation   - priority speed location.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnPrioritySpeedLocationSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT prioritySpeedLocation
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    switch (prioritySpeedLocation)
    {
        case CPSS_DXCH_PORT_CN_PRIORITY_SPEED_CPID_E:
            value = 0;
            break;
        case CPSS_DXCH_PORT_CN_SDU_UP_E:
            value = 1;
            break;
        case CPSS_DXCH_PORT_CN_SPEED_E:
            value = 2;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;
    rc =  prvCpssDrvHwPpSetRegField(devNum, regAddr, 19, 2, value);
    if (rc != GT_OK)
    {
        return rc;
    }
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.globalUnitConfig;
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 8, 2, value);

}

/*******************************************************************************
* cpssDxChPortCnPrioritySpeedLocationGet
*
* DESCRIPTION:
*       Gets location of the index (priority/speed) bits in the CNM header
*       for parsing and generation of CNM.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum        - device number.
*
* OUTPUTS:
*       prioritySpeedLocationPtr - (pointer to) priority speed location.
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
GT_STATUS cpssDxChPortCnPrioritySpeedLocationGet
(
    IN  GT_U8                                devNum,
    OUT CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT *prioritySpeedLocationPtr
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    CPSS_NULL_PTR_CHECK_MAC(prioritySpeedLocationPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;
    rc =  prvCpssDrvHwPpGetRegField(devNum, regAddr, 19, 2, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    *prioritySpeedLocationPtr = (CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT)value;
    return GT_OK;
}

