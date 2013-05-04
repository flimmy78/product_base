/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortBufMg.c
*
* DESCRIPTION:
*       CPSS implementation for Port configuration and control facility -DXCH
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortBufMg.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortTx.h>

/* check the flow control profile set range */
#define PRV_CPSS_DXCH_FC_PROFILE_CHECK_MAC(profileSet) \
        if ((GT_U32)(profileSet) > (GT_U32)CPSS_PORT_RX_FC_PROFILE_4_E) \
             return GT_BAD_PARAM

/* check the global Xon/Xoff flow control limit value range */
/* field support 11 bits */
#define PRV_CPSS_DXCH_GLOBAL_XON_XOFF_CHECK_MAC(xonXoff) \
        if (xonXoff >= BIT_11) return GT_BAD_PARAM


/*******************************************************************************
* cpssDxChGlobalXonLimitSet
*
* DESCRIPTION:
*       Sets the Global Xon limit value. Enables global control over the
*       number of buffers allocated for all ports by Global Xon limit setting.
*       When the total number of buffers allocated reaches this threshold
*       (xonLimit), all ports that support 802.3x Flow Control send an Xon
*       frame.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - PP's device number.
*       xonLimit   - X-ON limit in resolution of 2 buffers.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or xonLimit.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. The Global Xon limit must be not be greater than Global Xoff Limit.
*       2. To disable Global Xon, the 2*xonLimit should be set greater-or-equal
*          of 8*<Max Buffer Limit> (maximal number of buffers allocated for
*          all ports).
*       3. Since in CH3 and above the HW resolution is 8,
*          in case the xonLimit input parameter is not a multiple of 8 we
*          round it UP to the nearest one.
*
*******************************************************************************/
GT_STATUS cpssDxChGlobalXonLimitSet
(
    IN GT_U8    devNum,
    IN GT_U32   xonLimit
)
{
    GT_U32      regAddr;     /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* For DXCH and DXCH2 devices the field resolution is of 2 buffers */
    /* For DXCH3 and above, the field resolution is of 8 buffers,*/
    /* which we are rounding up */
    if ((PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        >= CPSS_PP_FAMILY_CHEETAH3_E))
    {
        if( xonLimit != 0 )
        {
            xonLimit = ((xonLimit - 1) >> 2) + 1;
        }
    }

    PRV_CPSS_DXCH_GLOBAL_XON_XOFF_CHECK_MAC(xonLimit);
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.bufMngGlobalLimit;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 11, xonLimit);
}

/*******************************************************************************
* cpssDxChGlobalXonLimitGet
*
* DESCRIPTION:
*       Gets the Global Xon limit value.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - PP's device number.
*
* OUTPUTS:
*       xonLimitPtr  - (pointer to) the X-ON limit value in resolution
*                       of 2 buffers.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChGlobalXonLimitGet
(
    IN GT_U8    devNum,
    OUT GT_U32  *xonLimitPtr
)
{
    GT_U32      regAddr;     /* register address */
    GT_STATUS   rc;          /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(xonLimitPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.bufMngGlobalLimit;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 11, xonLimitPtr);
    if( rc != GT_OK )
        return rc;

    /* For DXCH3 and above, the field resolution is 8 buffers instead of 2 */
    if ((PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        >= CPSS_PP_FAMILY_CHEETAH3_E))
    {
        *xonLimitPtr = (*xonLimitPtr) << 2;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChGlobalXoffLimitSet
*
* DESCRIPTION:
*       Sets the Global Xoff limit value. Enables global control over the
*       number of buffers allocated for all ports by Global Xoff limit setting.
*       When the total number of buffers allocated reaches this threshold
*       (xoffLimit), all ports that support 802.3x Flow Control send a pause
*       frame.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - PP's device number.
*       xoffLimit   - X-OFF limit in resolution of 2 buffers.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or xoffLimit.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. To disable Global Xoff, the 2*xoffLimit should be set
*          greater-or-equal of 8*<Max Buffer Limit> (maximal number of buffers
*          allocated for all ports).
*       2. Since in CH3 and above the HW resolution is 8,
*          in case the xoffLimit input parameter is not a multiple of 8 we
*          round it UP to the nearest one.
*
*******************************************************************************/
GT_STATUS cpssDxChGlobalXoffLimitSet
(
    IN GT_U8    devNum,
    IN GT_U32   xoffLimit
)
{
    GT_U32      regAddr;     /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* For DXCH and DXCH2 devices the field resolution is of 2 buffers */
    /* For DXCH3 and above, the field resolution is of 8 buffers,*/
    /* which we are rounding up */
    if ((PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        >= CPSS_PP_FAMILY_CHEETAH3_E))
    {
        if( xoffLimit != 0 )
        {
            xoffLimit = ((xoffLimit - 1) >> 2) + 1;
        }
    }

    PRV_CPSS_DXCH_GLOBAL_XON_XOFF_CHECK_MAC(xoffLimit);
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.bufMngGlobalLimit;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 11, 11, xoffLimit);
}

/*******************************************************************************
* cpssDxChGlobalXoffLimitGet
*
* DESCRIPTION:
*       Gets the Global Xoff limit value.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - PP's device number.
*
* OUTPUTS:
*       xoffLimitPtr - (pointer to) the X-OFF limit value in resolution
*                       of 2 buffers.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChGlobalXoffLimitGet
(
    IN GT_U8    devNum,
    OUT GT_U32  *xoffLimitPtr
)
{
    GT_U32      regAddr;     /* register address */
    GT_STATUS   rc;          /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(xoffLimitPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.bufMngGlobalLimit;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 11, 11, xoffLimitPtr);
    if( rc != GT_OK )
        return rc;

    /* For DXCH3 and above, the field resolution is 8 buffers instead of 2 */
    if ((PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        >= CPSS_PP_FAMILY_CHEETAH3_E))
    {
        *xoffLimitPtr = (*xoffLimitPtr) << 2;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortRxFcProfileSet
*
* DESCRIPTION:
*       Bind a port to a flow control profile.
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
*       profileSet - the Profile Set in which the Flow Control Parameters are
*                    associated.
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
GT_STATUS cpssDxChPortRxFcProfileSet
(
    IN GT_U8                            devNum,
    IN GT_U8                            portNum,
    IN CPSS_PORT_RX_FC_PROFILE_SET_ENT  profileSet
)
{
    GT_U32  regAddr;        /* register address */
    GT_U32  fieldOffset;    /* the start bit number in the register         */
    GT_U32  index;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_FC_PROFILE_CHECK_MAC(profileSet);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    index = (localPort % 32);/* support also CPU port */

    fieldOffset =  (index % 16) * 2;
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
                bufLimitsProfile.associationReg0 + 4 * (index / 16);


    return prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,
                                     regAddr,
                                     fieldOffset,
                                     2,
                                     (GT_U32)profileSet);
}

/*******************************************************************************
* cpssDxChPortRxFcProfileGet
*
* DESCRIPTION:
*       Get the port's flow control profile.
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
*
* OUTPUTS:
*       profileSetPtr - (pointer to) the Profile Set in which the Flow Control
*                       Parameters are associated.
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
GT_STATUS cpssDxChPortRxFcProfileGet
(
    IN  GT_U8                            devNum,
    IN  GT_U8                            portNum,
    OUT CPSS_PORT_RX_FC_PROFILE_SET_ENT  *profileSetPtr
)
{
    GT_U32  regAddr;        /* register address */
    GT_U32  fieldOffset;    /* the start bit number in the register         */
    GT_U32  index;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(profileSetPtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    index = (localPort % 32);/* support also CPU port */

    fieldOffset =  (index % 16) * 2;
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
                bufLimitsProfile.associationReg0 + 4 * (index / 16);

    return prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId,
                                     regAddr,
                                     fieldOffset,
                                     2,
                                     (GT_U32*)profileSetPtr);
}

/*******************************************************************************
* cpssDxChPortXonLimitSet
*
* DESCRIPTION:
*       Sets X-ON port limit. When Flow Control is enabled, X-ON packet sent
*       when the number of buffers for this port is less than xonLimit.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       profileSet - the Profile Set in which the Flow Control Parameters are
*                    associated.
*       xonLimit   - X-ON limit in resolution of 2 buffers.
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
*       Since in CH3 the HW resolution is 8, in case the xonLimit input
*       parameter is not a multiple of 8 we round it UP to the nearest one.
*
*******************************************************************************/
GT_STATUS cpssDxChPortXonLimitSet
(
    IN GT_U8                            devNum,
    IN CPSS_PORT_RX_FC_PROFILE_SET_ENT  profileSet,
    IN GT_U32                           xonLimit
)
{
    GT_U32  regAddr;        /* register address                             */
    GT_U32  fieldOffset;    /* the start bit number in the register         */
    GT_U32  fieldLength;    /* the number of bits to be written to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_FC_PROFILE_CHECK_MAC(profileSet);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
              bufLimitProfileConfig[profileSet];

    /* For DXCH and DXCH2 devices the field resolution is of 2 buffers */
    /* For DXCH3 the field resolution is of 8 buffers, which we are rounding up */
    if ((PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        >= CPSS_PP_FAMILY_CHEETAH3_E))
    {
        if( xonLimit != 0 )
        {
            xonLimit = ((xonLimit - 1) >> 2) + 1;
        }
    }

    if(xonLimit >= BIT_11)
    {
        return GT_BAD_PARAM;
    }

    fieldOffset= 0;
    fieldLength= 11;

    return  prvCpssDrvHwPpSetRegField(devNum,
                                      regAddr,
                                      fieldOffset,
                                      fieldLength,
                                      xonLimit);
}

/*******************************************************************************
* cpssDxChPortXonLimitGet
*
* DESCRIPTION:
*       Gets the X-ON port limit. When Flow Control is enabled, X-ON packet
*       sent when the number of buffers for this port is less than xonLimit.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       profileSet - the Profile Set in which the Flow Control Parameters of
*                    are associated.
*
* OUTPUTS:
*       xonLimitPtr  - (pointer to) the X-ON limit value in resolution
*                       of 2 buffers.
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
GT_STATUS cpssDxChPortXonLimitGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORT_RX_FC_PROFILE_SET_ENT profileSet,
    OUT GT_U32                          *xonLimitPtr
)
{
    GT_U32  regAddr;        /* register address                             */
    GT_U32  fieldOffset;    /* the start bit number in the register         */
    GT_U32  fieldLength;    /* the number of bits to be written to register */
    GT_STATUS   rc;         /* return code                    */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_FC_PROFILE_CHECK_MAC(profileSet);
    CPSS_NULL_PTR_CHECK_MAC(xonLimitPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
              bufLimitProfileConfig[profileSet];

    fieldOffset= 0;
    fieldLength= 11;

    rc = prvCpssDrvHwPpGetRegField(devNum,
                                   regAddr,
                                   fieldOffset,
                                   fieldLength,
                                   xonLimitPtr);
    if( rc != GT_OK )
        return rc;

    /* For DXCH3 the field resolution is 8 buffers instead of 2 */
    if ((PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        >= CPSS_PP_FAMILY_CHEETAH3_E))
    {
        *xonLimitPtr = (*xonLimitPtr) << 2;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortXoffLimitSet
*
* DESCRIPTION:
*       Sets X-OFF port limit. When Flow Control is enabled, X-OFF packet sent
*       when the number of buffers for this port is less than xoffLimit.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       profileSet - the Profile Set in which the Flow Control Parameters are
*                    associated.
*       xoffLimit  - X-OFF limit in resolution of 2 buffers.
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
*       Since in CH3 the HW resolution is 8, in case the xoffLimit input
*       parameter is not a multiple of 8 we round it UP to the nearest one.
*
*******************************************************************************/
GT_STATUS cpssDxChPortXoffLimitSet
(
    IN GT_U8                            devNum,
    IN CPSS_PORT_RX_FC_PROFILE_SET_ENT  profileSet,
    IN GT_U32                           xoffLimit
)
{
    GT_U32  regAddr;        /* register address */
    GT_U32  fieldOffset;    /* the start bit number in the register         */
    GT_U32  fieldLength;    /* the number of bits to be written to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_FC_PROFILE_CHECK_MAC(profileSet);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
              bufLimitProfileConfig[profileSet];

    /* For DXCH and DXCH2 devices the field resolution is of 2 buffers */
    /* For DXCH3 the field resolution is of 8 buffers, which we are rounding up */
    if ((PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        >= CPSS_PP_FAMILY_CHEETAH3_E))
    {
        if( xoffLimit != 0 )
        {
            xoffLimit = ((xoffLimit - 1) >> 2) + 1;
        }
    }

    if(xoffLimit >= BIT_11)
    {
        return GT_BAD_PARAM;
    }


    fieldOffset= 11;
    fieldLength= 11;

    return  prvCpssDrvHwPpSetRegField(devNum,
                                      regAddr,
                                      fieldOffset,
                                      fieldLength,
                                      xoffLimit);
}

/*******************************************************************************
* cpssDxChPortXoffLimitGet
*
* DESCRIPTION:
*       Gets the X-OFF port limit. When Flow Control is enabled, X-OFF packet
*       sent when the number of buffers for this port is less than xoffLimit.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       profileSet - the Profile Set in which the Flow Control Parameters are
*                    associated.
*
* OUTPUTS:
*       xoffLimitPtr - (pointer to) the X-OFF limit value in resolution
*                       of 2 buffers.
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
GT_STATUS cpssDxChPortXoffLimitGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORT_RX_FC_PROFILE_SET_ENT profileSet,
    OUT GT_U32                          *xoffLimitPtr
)
{
    GT_U32  regAddr;        /* register address */
    GT_U32  fieldOffset;    /* the start bit number in the register         */
    GT_U32  fieldLength;    /* the number of bits to be written to register */
    GT_STATUS   rc;         /* return code                    */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_FC_PROFILE_CHECK_MAC(profileSet);
    CPSS_NULL_PTR_CHECK_MAC(xoffLimitPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
              bufLimitProfileConfig[profileSet];
    fieldOffset= 11;
    fieldLength= 11;

    rc = prvCpssDrvHwPpGetRegField(devNum,
                                   regAddr,
                                   fieldOffset,
                                   fieldLength,
                                   xoffLimitPtr);

    if( rc != GT_OK )
        return rc;

    /* For DXCH3 the field resolution is 8 buffers instead of 2 */
    if ((PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        >= CPSS_PP_FAMILY_CHEETAH3_E))
    {
        *xoffLimitPtr = (*xoffLimitPtr) << 2;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortRxBufLimitSet
*
* DESCRIPTION:
*       Sets receive buffer limit threshold for the specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       profileSet - the Profile Set in which the Flow Control Parameters are
*                    associated.
*       rxBufLimit - buffer limit threshold in resolution of 4 buffers.
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
*       Since in CH3 and above, the HW resolution is 16, in case the rxBufLimit
*       input parameter is not a multiple of 16 we round it UP to the nearest one.
*
*******************************************************************************/
GT_STATUS cpssDxChPortRxBufLimitSet
(
    IN GT_U8                            devNum,
    IN CPSS_PORT_RX_FC_PROFILE_SET_ENT  profileSet,
    IN GT_U32                           rxBufLimit
)
{
    GT_U32  regAddr;        /* register address */
    GT_U32  fieldOffset;    /* the start bit number in the register         */
    GT_U32  fieldLength;    /* the number of bits to be written to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_FC_PROFILE_CHECK_MAC(profileSet);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
              bufLimitProfileConfig[profileSet];

    /* For DXCH and DXCH2 devices the field resolution is of 4 buffers */
    /* For DXCH3 the field resolution is of 16 buffers, which we are rounding up */
    if ((PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        >= CPSS_PP_FAMILY_CHEETAH3_E))
    {
        if( rxBufLimit != 0 )
        {
            rxBufLimit = ((rxBufLimit - 1) >> 2) + 1;
        }
    }

    if(rxBufLimit >= BIT_10)
    {
        return GT_BAD_PARAM;
    }


    fieldOffset= 22;
    fieldLength= 10;

    return prvCpssDrvHwPpSetRegField(devNum,
                                     regAddr,
                                     fieldOffset,
                                     fieldLength,
                                     rxBufLimit);
}

/*******************************************************************************
* cpssDxChPortRxBufLimitGet
*
* DESCRIPTION:
*       Gets the receive buffer limit threshold for the specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       profileSet - the Profile Set in which the Flow Control Parameters are
*                    associated.
*
* OUTPUTS:
*       rxBufLimitPtr - (pointer to) the rx buffer limit value in resolution
*                       of 4 buffers.
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
GT_STATUS cpssDxChPortRxBufLimitGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORT_RX_FC_PROFILE_SET_ENT profileSet,
    OUT GT_U32                          *rxBufLimitPtr
)
{
    GT_U32  regAddr;        /* register address */
    GT_U32  fieldOffset;    /* the start bit number in the register         */
    GT_U32  fieldLength;    /* the number of bits to be written to register */
    GT_STATUS   rc;         /* return code                    */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_FC_PROFILE_CHECK_MAC(profileSet);
    CPSS_NULL_PTR_CHECK_MAC(rxBufLimitPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
              bufLimitProfileConfig[profileSet];

    fieldOffset= 22;
    fieldLength= 10;

    rc = prvCpssDrvHwPpGetRegField(devNum,
                                   regAddr,
                                   fieldOffset,
                                   fieldLength,
                                   rxBufLimitPtr);

    if( rc != GT_OK )
        return rc;

    /* For DXCH3 the field resolution is 16 buffers instead of 4 */
    if ((PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        >= CPSS_PP_FAMILY_CHEETAH3_E))
    {
        *rxBufLimitPtr = (*rxBufLimitPtr) << 2;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCpuRxBufCountGet
*
* DESCRIPTION:
*       Gets receive buffer count for the CPU on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The device to get the Cpu Rx buffer count for.
*
* OUTPUTS:
*       cpuRxBufCntPtr - (Pointer to) number of buffers allocated for the CPU.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCpuRxBufCountGet
(
    IN  GT_U8   devNum,
    OUT GT_U16  *cpuRxBufCntPtr
)
{
    GT_U32 data;
    GT_U32 mask;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(cpuRxBufCntPtr);

    mask= 0x0FFF;

    if (prvCpssDrvHwPpReadRegBitMask(devNum,
                           PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
                           cpuRxBufCount,
                           mask,
                           &data) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    *cpuRxBufCntPtr = (GT_U16) (data & 0xFFFF);
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortFcHolSysModeSet
*
* DESCRIPTION:
*       Set Flow Control or HOL system mode on the specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - physical device number
*       modeFcHol - GT_FC_E  : set Flow Control mode
*                   GT_HOL_E : set HOL system mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPortFcHolSysModeSet
(
    IN  GT_U8                     devNum,
    IN  CPSS_DXCH_PORT_HOL_FC_ENT modeFcHol
)
{
    GT_U32      tailDropDis;
    GT_U32      bufsTailDropEn;
    GT_U32      descLim;  /* value to write into register   */
    GT_U32      cellBase; /* value to write into register   */
    GT_STATUS   rc;       /* return code                    */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    cellBase = 0; /* Cell based HOL parameter by default */
    descLim  = 0x10;

    switch (modeFcHol)
    {
        case CPSS_DXCH_PORT_FC_E:
            tailDropDis    = 1;
            bufsTailDropEn = 0;

            /* When setting FC, first buffer's mode must be set and then */
            /* disable the Tail Drop. */

            /* symmetric FC - set buffer's mode to divided */
            rc = cpssDxChPortBuffersModeSet(devNum, 
                                       CPSS_DXCH_PORT_BUFFERS_MODE_DIVIDED_E);
            if (rc != GT_OK)
                return rc;

            /* RM#26 - for DX106/107 */
            /* RM#27 - for DX249 */
            if((PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX106_CNS) ||
               (PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX107_CNS) ||
               (PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX249_CNS))
            {
                /* Transmit Queue Control Register */
                descLim = 0x1;

            }
            break;

        case CPSS_DXCH_PORT_HOL_E:
            tailDropDis    = 0;
            bufsTailDropEn = 1;
            
            /* RM#26 - for DX106/107 */
            /* RM#27 - for DX249 */
            if((PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX106_CNS) ||
               (PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX107_CNS) ||
               (PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX249_CNS))
            {
                /* Transmit Queue Control Register - MultiDescLimit */
               descLim = 0x3;

            }
            break;

        default:
            return GT_BAD_PARAM;

    }

    /* enable / disable Tail Drop */
    /* Disable Tail Drop for all Unicast packets. */
    rc = cpssDxChPortTxTailDropUcEnableSet(devNum,
        (tailDropDis == GT_TRUE ? GT_FALSE : GT_TRUE));

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* set a descriptor limit for Multicast packets*/
        rc = cpssDxChPortTxMcastPcktDescrLimitSet(devNum,descLim);
        if (rc != GT_OK)
            return rc;
    
        /* enable / disable Tail Drop according to the number of buffers */
        /* in the queues. */
        rc = cpssDxChPortTxBufferTailDropEnableSet(devNum,bufsTailDropEn);
        if (rc != GT_OK)
            return rc;
    }

    /* When setting HOL, first enable the Tail Drop (already done) and then */
    /* set buffer's mode. */
    if( CPSS_DXCH_PORT_HOL_E == modeFcHol )
    {
        /* tail drop - set buffer's mode to shared */
        rc = cpssDxChPortBuffersModeSet(devNum, CPSS_DXCH_PORT_BUFFERS_MODE_SHARED_E);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChPortBuffersModeSet
*
* DESCRIPTION:
*      The function sets buffers mode either shared or divided.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number.
*       bufferMode - buffers mode, divided or shared.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on invalid input parameters value
*       GT_HW_ERROR                - on hardware error
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*******************************************************************************/
GT_STATUS cpssDxChPortBuffersModeSet
(
    IN GT_U8                            devNum,
    IN CPSS_DXCH_PORT_BUFFERS_MODE_ENT  bufferMode
)
{

    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to write into the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    switch (bufferMode)
    {
        case CPSS_DXCH_PORT_BUFFERS_MODE_DIVIDED_E:
            value = 1;
            break;
        case CPSS_DXCH_PORT_BUFFERS_MODE_SHARED_E:
            value = 0;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.bufMngGlobalLimit;

    return  prvCpssDrvHwPpSetRegField(devNum, regAddr, 31, 1,value);
}

/*******************************************************************************
* cpssDxChPortBuffersModeGet
*
* DESCRIPTION:
*      The function gets buffers mode either shared or divided.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - device number.
*
* OUTPUTS:
*       bufferModePtr - pointer to the buffer mode.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on invalid input parameters value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR                - on hardware error
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*******************************************************************************/
GT_STATUS cpssDxChPortBuffersModeGet
(
    IN  GT_U8                           devNum,
    OUT CPSS_DXCH_PORT_BUFFERS_MODE_ENT *bufferModePtr
)
{
    GT_U32    regAddr;     /* register address */
    GT_U32    value;       /* value to be read from the register */
    GT_STATUS rc;          /* function return value            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(bufferModePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.bufMngGlobalLimit;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 31, 1, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    *bufferModePtr = (value == 1) ?
            CPSS_DXCH_PORT_BUFFERS_MODE_DIVIDED_E :
            CPSS_DXCH_PORT_BUFFERS_MODE_SHARED_E;
    return GT_OK;

}

/*******************************************************************************
* cpssDxChPortGroupXonLimitSet
*
* DESCRIPTION:
*       The function sets the port-group X-ON threshold.
*       When Flow Control is enabled, X-ON packet sent
*       when the number of buffers for this port is less than xonLimit.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - port group
*       xonLimit  -  X-ON limit in buffers.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Resolution of xonLimit field in hardware:
*       - DXCH and DXCH2 devices: 2 buffers.
*       - DXCH3 and above devices: 8 buffers.
*       The value is rounded down before write to HW.
*
*******************************************************************************/
GT_STATUS cpssDxChPortGroupXonLimitSet
(
    IN GT_U8                     devNum,
    IN CPSS_DXCH_PORT_GROUP_ENT  portGroup,
    IN GT_U32                    xonLimit
)
{
    GT_U32 regAddr;         /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    switch(portGroup)
    {
        case CPSS_DXCH_PORT_GROUP_GIGA_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                bufferMng.bufMngGigaPortGroupLimit;
            break;

        case CPSS_DXCH_PORT_GROUP_HGS_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                bufferMng.bufMngHyperGStackPortGroupLimit;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* For DXCH and DXCH2 devices the field resolution is of 2 buffers */
    /* For DXCH3 and above, the field resolution is of 8 buffers,*/
    /* which we are rounding down */
    if ((PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        >= CPSS_PP_FAMILY_CHEETAH3_E))
    {
        xonLimit = xonLimit/8;
    }
    else
    {
        xonLimit = xonLimit/2;
    }

    PRV_CPSS_DXCH_GLOBAL_XON_XOFF_CHECK_MAC(xonLimit);

    return  prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 11, xonLimit);

}

/*******************************************************************************
* cpssDxChPortGroupXonLimitGet
*
* DESCRIPTION:
*       The function gets the port-group X-ON threshold.
*       When Flow Control is enabled, X-ON packet
*       sent when the number of buffers for this port is less than xonLimit.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - port group
*
* OUTPUTS:
*       xonLimitPtr  - Pointer to the  X-ON limit in buffers.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortGroupXonLimitGet
(
    IN  GT_U8                    devNum,
    IN  CPSS_DXCH_PORT_GROUP_ENT portGroup,
    OUT GT_U32                   *xonLimitPtr
)
{
    GT_U32    regAddr;         /* register address */
    GT_STATUS rc;              /* return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(xonLimitPtr);

    switch(portGroup)
    {
        case CPSS_DXCH_PORT_GROUP_GIGA_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                bufferMng.bufMngGigaPortGroupLimit;
            break;

        case CPSS_DXCH_PORT_GROUP_HGS_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                bufferMng.bufMngHyperGStackPortGroupLimit;
            break;

        default:
            return GT_BAD_PARAM;
    }


    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 11, xonLimitPtr);
    if(rc != GT_OK)
        return rc;

    /* For DXCH and DXCH2 devices the field resolution is of 2 buffers */
    /* For DXCH3 and above, the field resolution is of 8 buffers */
    if ((PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        >= CPSS_PP_FAMILY_CHEETAH3_E))
    {
        *xonLimitPtr = (*xonLimitPtr) * 8;
    }
    else
    {
        *xonLimitPtr = (*xonLimitPtr) * 2;
    }

    return GT_OK;


}

/*******************************************************************************
* cpssDxChPortGroupXoffLimitSet
*
* DESCRIPTION:
*       The function sets the port-group X-OFF threshold.
*       When Flow Control is enabled, X-OFF packet sent
*       when the number of buffers for this port is less than xoffLimit.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - port group
*       xoffLimit  - X-OFF limit in buffers
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Resolution of xoffLimit field in hardware:
*       - DXCH and DXCH2 devices: 2 buffers.
*       - DXCH3 and above devices: 8 buffers.
*       The value is rounded down before write to HW.
*
*******************************************************************************/
GT_STATUS cpssDxChPortGroupXoffLimitSet
(
    IN GT_U8                     devNum,
    IN CPSS_DXCH_PORT_GROUP_ENT  portGroup,
    IN GT_U32                    xoffLimit
)
{
    GT_U32 regAddr;         /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    switch(portGroup)
    {
        case CPSS_DXCH_PORT_GROUP_GIGA_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                bufferMng.bufMngGigaPortGroupLimit;
            break;

        case CPSS_DXCH_PORT_GROUP_HGS_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                bufferMng.bufMngHyperGStackPortGroupLimit;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* For DXCH and DXCH2 devices the field resolution is of 2 buffers */
    /* For DXCH3 and above, the field resolution is of 8 buffers,*/
    /* which we are rounding down */
    if ((PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        >= CPSS_PP_FAMILY_CHEETAH3_E))
    {
        xoffLimit = xoffLimit/8;
    }
    else
    {
        xoffLimit = xoffLimit/2;
    }

    PRV_CPSS_DXCH_GLOBAL_XON_XOFF_CHECK_MAC(xoffLimit);

    return  prvCpssDrvHwPpSetRegField(devNum, regAddr, 11, 11, xoffLimit);

}

/*******************************************************************************
* cpssDxChPortGroupXoffLimitGet
*
* DESCRIPTION:
*       The function gets the port-group X-OFF threshold.
*       When Flow Control is enabled, X-OFF packet
*       sent when the number of buffers for this port is less than xoffLimit.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - port group
*
* OUTPUTS:
*       xoffLimitPtr  - Pointer to the X-OFF limit in buffers.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortGroupXoffLimitGet
(
    IN  GT_U8                    devNum,
    IN  CPSS_DXCH_PORT_GROUP_ENT portGroup,
    OUT GT_U32                   *xoffLimitPtr
)
{
    GT_U32    regAddr;         /* register address */
    GT_STATUS rc;              /* return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(xoffLimitPtr);

    switch(portGroup)
    {
        case CPSS_DXCH_PORT_GROUP_GIGA_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                bufferMng.bufMngGigaPortGroupLimit;
            break;

        case CPSS_DXCH_PORT_GROUP_HGS_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                bufferMng.bufMngHyperGStackPortGroupLimit;
            break;

        default:
            return GT_BAD_PARAM;
    }


    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 11, 11, xoffLimitPtr);
    if(rc != GT_OK)
        return rc;

    /* For DXCH and DXCH2 devices the field resolution is of 2 buffers */
    /* For DXCH3 and above, the field resolution is of 8 buffers */
    if ((PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        >= CPSS_PP_FAMILY_CHEETAH3_E))
    {
        *xoffLimitPtr = (*xoffLimitPtr) * 8;
    }
    else
    {
        *xoffLimitPtr = (*xoffLimitPtr) * 2;
    }

    return GT_OK;

}

/*******************************************************************************
* cpssDxChPortGroupRxBufLimitSet
*
* DESCRIPTION:
*       Sets receive buffer limit threshold for the specified port-group.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - port group
*       rxBufLimit - Maximal number of buffers that may be allocated for the
*                    ports belong to a group
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Resolution of rxBufLimit field in hardware:
*       - DXCH and DXCH2 devices: 4 buffers.
*       - DXCH3 and above devices: 16 buffers.
*       The value is rounded down before write to HW.
*
*******************************************************************************/
GT_STATUS cpssDxChPortGroupRxBufLimitSet
(
    IN GT_U8                     devNum,
    IN CPSS_DXCH_PORT_GROUP_ENT  portGroup,
    IN GT_U32                    rxBufLimit
)
{
    GT_U32 regAddr;         /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    switch(portGroup)
    {
        case CPSS_DXCH_PORT_GROUP_GIGA_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                bufferMng.bufMngGigaPortGroupLimit;
            break;

        case CPSS_DXCH_PORT_GROUP_HGS_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                bufferMng.bufMngHyperGStackPortGroupLimit;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* For DXCH and DXCH2 devices the field resolution is of 4 buffers */
    /* For DXCH3 and above, the field resolution is of 16 buffers,*/
    /* which we are rounding down */
    if ((PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        >= CPSS_PP_FAMILY_CHEETAH3_E))
    {
        rxBufLimit = rxBufLimit/16;
    }
    else
    {
        rxBufLimit = rxBufLimit/4;
    }

    if(rxBufLimit >= BIT_10)
    {
        return GT_BAD_PARAM;
    }

    return  prvCpssDrvHwPpSetRegField(devNum, regAddr, 22, 10, rxBufLimit);

}

/*******************************************************************************
* cpssDxChPortGroupRxBufLimitGet
*
* DESCRIPTION:
*       Gets the receive buffer limit threshold for the specified port-group.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - port group
*
* OUTPUTS:
*       rxBufLimitPtr - Pointer to value of maximal number of buffers that
*                       may be allocated for the ports belong to a group.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortGroupRxBufLimitGet
(
    IN  GT_U8                     devNum,
    IN  CPSS_DXCH_PORT_GROUP_ENT  portGroup,
    OUT GT_U32                    *rxBufLimitPtr
)
{
    GT_U32    regAddr;         /* register address */
    GT_U32    value;           /* register value */
    GT_STATUS rc;              /* return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(rxBufLimitPtr);

    switch(portGroup)
    {
        case CPSS_DXCH_PORT_GROUP_GIGA_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                bufferMng.bufMngGigaPortGroupLimit;
            break;

        case CPSS_DXCH_PORT_GROUP_HGS_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                bufferMng.bufMngHyperGStackPortGroupLimit;
            break;

        default:
            return GT_BAD_PARAM;
    }


    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 22, 10, &value);
    if( rc != GT_OK )
        return rc;

    /* For DXCH and DXCH2 devices the field resolution is of 4 buffers */
    /* For DXCH3 and above, the field resolution is of 16 buffers */
    if ((PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        >= CPSS_PP_FAMILY_CHEETAH3_E))
    {
        *rxBufLimitPtr = value * 16;
    }
    else
    {
        *rxBufLimitPtr = value * 4;
    }

    return GT_OK;

}

/*******************************************************************************
* cpssDxChPortCrossChipFcPacketRelayEnableSet
*
* DESCRIPTION:
*     Enable/Disable SOHO Cross Chip Flow control packet relay.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number.
*       enable     - GT_TRUE:  Enable SOHO Cross Chip Flow control packet relay.
*                    GT_FALSE: Disable SOHO Cross Chip Flow control packet relay.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*******************************************************************************/
GT_STATUS cpssDxChPortCrossChipFcPacketRelayEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{

    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    value = (enable == GT_TRUE) ? 1 : 0;
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.ctrlReg0;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 22, 1, value);


}

/*******************************************************************************
* cpssDxChPortCrossChipFcPacketRelayEnableGet
*
* DESCRIPTION:
*     Gets the current status of SOHO Cross Chip Flow control packet relay.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number.
*
* OUTPUTS:
*       enablePtr - pointer to SOHO Cross Chip Flow control packet relay staus:
*                   GT_TRUE  -  Enable SOHO Cross Chip Flow control packet relay.
*                   GT_FALSE - Disable SOHO Cross Chip Flow control packet relay.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*******************************************************************************/
GT_STATUS cpssDxChPortCrossChipFcPacketRelayEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32     value;     /* register value */
    GT_U32     regAddr;   /* register address */
    GT_STATUS  rc;        /* function return value */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.ctrlReg0;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 22, 1, &value);

    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (value == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChPortClearByBufferEnableSet
*
* DESCRIPTION:
*       Set port to work in Clear By Buffer mode.
*
* APPLICABLE DEVICES:
*        DxCh1.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (CPU port supported as well).
*       enable     - GT_TRUE: Clear By Buffer mode.
*                    GT_FALSE: Regular Clear mode.
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
GT_STATUS cpssDxChPortClearByBufferEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  enable
)
{
    GT_U32  index;      /* index of port bit in the register */
    GT_U32  regAddr;    /* register address */
    GT_U32  regData;    /* register data */
    GT_STATUS rc;       /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    if(portNum == CPSS_CPU_PORT_NUM_CNS)
    {
        index = 27;
    }
    else /* physical port - not CPU port */
    {
        index = portNum;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->addrSpace.buffMemClearType;

    if(enable == GT_TRUE)
    {
        /* set port bit in buffer memory clear type register */
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, index, 1, 1);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* RM: enable Clear By Buffer in Buffer Management Metal Fix Register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.bufMngMetalFix;
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 4, 1, 1);
    }
    else
    {
        /* clear port bit in buffer memory clear type register */
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, index, 1, 0);
        if(rc != GT_OK)
        {
            return rc;
        }

        regData = 0;
        rc = prvCpssDrvHwPpReadRegBitMask(devNum, regAddr, 0x0FFFFFFF, &regData);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* check if all port bits are cleared. */
        /* In case no port is working in Clear By Buffer mode, */
        /* disable Clear By Buffer in Buffer Management Metal Fix Register */
        if( regData == 0x0 )
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.bufMngMetalFix;
            rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 4, 1, 0);
        }
    }

    return rc;
}

/*******************************************************************************
* cpssDxChPortClearByBufferEnableGet
*
* DESCRIPTION:
*       Get port Buffer Memory Clear type.
*
* APPLICABLE DEVICES:
*        DxCh1.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum     - physical device number.
*       portNum    - physical port number (CPU port supported as well).
*
* OUTPUTS:
*       enablePtr  - (pointer to) GT_TRUE: Clear By Buffer mode.
*                                 GT_FALSE: Regular Clear mode.
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
GT_STATUS cpssDxChPortClearByBufferEnableGet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32  index;      /* index of port bit in the register */
    GT_U32  regAddr;    /* register address */
    GT_U32  value;      /* register bit value */
    GT_STATUS rc;       /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(portNum == CPSS_CPU_PORT_NUM_CNS)
    {
        index = 27;
    }
    else /* physical port - not CPU port */
    {
        index = portNum;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->addrSpace.buffMemClearType;

    /* set port bit in buffer memory clear type register */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, index, 1, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (value == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortGlobalRxBufNumberGet
*
* DESCRIPTION:
*       Gets total number of buffers currently allocated.
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
*       numOfBuffersPtr - (pointer to) number of buffers
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
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortGlobalRxBufNumberGet
(
    IN  GT_U8    devNum,
    OUT GT_U32  *numOfBuffersPtr
)
{
    GT_U32      regAddr;     /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(numOfBuffersPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.totalBufAllocCntr;

    /* Get counters value */
    return prvCpssDxChPortGroupsCounterSummary(devNum, regAddr,0,13, numOfBuffersPtr,NULL);
}

/*******************************************************************************
* cpssDxChPortGroupRxBufNumberGet
*
* DESCRIPTION:
*       Gets total number of buffers currently allocated for the specified
*       port group.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - port group
*
* OUTPUTS:
*       numOfBuffersPtr - (pointer to) number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device or port group
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortGroupRxBufNumberGet
(
    IN  GT_U8    devNum,
    IN  CPSS_DXCH_PORT_GROUP_ENT  portGroup,
    OUT GT_U32  *numOfBuffersPtr
)
{
    GT_U32      regAddr;     /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(numOfBuffersPtr);

    switch (portGroup)
    {
        case CPSS_DXCH_PORT_GROUP_GIGA_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.netPortGroupAllocCntr;
            break;
        case CPSS_DXCH_PORT_GROUP_HGS_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.stackPortGroupAllocCntr;
            break;
        default:
            return GT_BAD_PARAM;
    }
    
    /* Get counters value */
    return prvCpssDxChPortGroupsCounterSummary(devNum, regAddr,0,13, numOfBuffersPtr,NULL);
}

/*******************************************************************************
* cpssDxChPortRxBufNumberGet
*
* DESCRIPTION:
*       Gets total number of buffers currently allocated for the specified port.
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
*       numOfBuffersPtr - (pointer to) number of buffers
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
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortRxBufNumberGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT GT_U32  *numOfBuffersPtr
)
{
    GT_U32      regAddr;     /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(numOfBuffersPtr);

    if(portNum == CPSS_CPU_PORT_NUM_CNS)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            bufferMng.cpuBufMngPerPort.portBufAllocCounter;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            bufferMng.bufMngPerPort[portNum].portBufAllocCounter;
    }

    return prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, 0, 13, numOfBuffersPtr);
}

/*******************************************************************************
* cpssDxChPortGlobalPacketNumberGet
*
* DESCRIPTION:
*       Gets total number of unique packets currently in the system.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       numOfPacketsPtr - (pointer to) number of packets
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
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortGlobalPacketNumberGet
(
    IN  GT_U8    devNum,
    OUT GT_U32  *numOfPacketsPtr
)
{
    GT_U32      regAddr;     /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);
    CPSS_NULL_PTR_CHECK_MAC(numOfPacketsPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.bufMngPacketCntr;

    /* Get counters value */
    return prvCpssDxChPortGroupsCounterSummary(devNum, regAddr,0,13, numOfPacketsPtr,NULL);
}


/*******************************************************************************
* cpssDxChPortBufMemXgFifosThresholdSet
*
* DESCRIPTION:
*       Sets buffer memory XG FIFOs Threshold.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum - physical device number
*       xgDescFifoThreshold - Number of descriptors per 20G/40G/HGL port mode. 
*                             This is relevant to ports that work in shared 
*                             mode (20G/40G/HGL).
*       xgTxFifoThreshold -  Number of lines per Network port in the Tx FIFO.
*       minXgPortXFifoReadThreshold - Minimal number of descriptors to read per 
*                            XG port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortBufMemXgFifosThresholdSet
(
    IN  GT_U8    devNum,
    IN  GT_U32   xgDescFifoThreshold,
    IN  GT_U32   xgTxFifoThreshold,
    IN  GT_U32   minXgPortXFifoReadThreshold
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      hwData;     /* data to read from hw */
    
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E |
                                          CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    if ((xgDescFifoThreshold >= BIT_4) || (xgTxFifoThreshold >= BIT_6) ||
        (minXgPortXFifoReadThreshold >= BIT_6)) 
    {
        return GT_OUT_OF_RANGE;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.txdmaBufMemXgFifosThreshold;

    hwData = (xgDescFifoThreshold | (xgTxFifoThreshold << 4) | 
              (minXgPortXFifoReadThreshold << 10));

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 16, hwData);
}

/*******************************************************************************
* cpssDxChPortBufMemXgFifosThresholdGet
*
* DESCRIPTION:
*       Gets buffer memory XG FIFOs Threshold.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       xgDescFifoThresholdPtr - (pointer to) Number of descriptors per 
*                                20G/40G/HGL port mode. This is relevant to 
*                                ports that work in shared mode (20G/40G/HGL).
*       xgTxFifoThresholdPtr -  (pointer to) Number of lines per Network 
*                                port in the Tx FIFO.
*       minXgPortXFifoReadThresholdPtr - (pointer to) Minimal number of  
*                               descriptors to read per XG port.
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
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortBufMemXgFifosThresholdGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *xgDescFifoThresholdPtr,
    OUT GT_U32   *xgTxFifoThresholdPtr,
    OUT GT_U32   *minXgPortXFifoReadThresholdPtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      hwData;     /* data to read from hw */
    GT_STATUS   rc;         /* return code      */
    
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E |
                                          CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(xgDescFifoThresholdPtr);
    CPSS_NULL_PTR_CHECK_MAC(xgTxFifoThresholdPtr);
    CPSS_NULL_PTR_CHECK_MAC(minXgPortXFifoReadThresholdPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.txdmaBufMemXgFifosThreshold;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 16, &hwData);
    if (rc != GT_OK) 
    {
        return rc;
    }

    *xgDescFifoThresholdPtr = (hwData & 0xF);
    *xgTxFifoThresholdPtr = ((hwData >> 4) & 0x3F);
    *minXgPortXFifoReadThresholdPtr = ((hwData >> 10) & 0x3F);

    return GT_OK;
}
