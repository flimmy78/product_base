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
* cpssDxChBrgSecurityBreach.c
*
* DESCRIPTION:
*       CPSS DxCh implementation for Bridge Security Breach Functionality
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

/* get the device info and common info */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgSecurityBreach.h>


/*******************************************************************************
* cpssDxChBrgSecurBreachPortVlanDropCntrModeSet
*
* DESCRIPTION:
*      Set Port/VLAN Security Breach Drop Counter to count security breach
*      dropped packets based on there ingress port or their assigned VID.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - physical device number
*       cntrCfgPtr          - (pointer to) security breach port/vlan counter
*                             mode and it's configuration parameters.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachPortVlanDropCntrModeSet
(
    IN GT_U8                                        dev,
    IN CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC     *cntrCfgPtr
)
{
    GT_STATUS   rc;             /* return code */
    GT_U32      regAddr;        /* hw register address */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32      finalPortGroupId;/* the port group Id that will be used for configuration that
                              relate to specific port */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    CPSS_NULL_PTR_CHECK_MAC(cntrCfgPtr);

    /* get address of Bridge Configuration Register1 */
    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.bridgeGlobalConfigRegArray[1];


    switch (cntrCfgPtr->dropCntMode)
    {
        case CPSS_BRG_SECUR_BREACH_DROP_COUNT_PORT_E:



            /* called without portGroupId , loop done inside the driver */
            rc = prvCpssDrvHwPpSetRegField(dev, regAddr, 19, 1, 0);
            if (rc != GT_OK)
                return rc;

            /* check port validity */
            PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(dev, cntrCfgPtr->port);

            /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
            finalPortGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, cntrCfgPtr->port);
            /* loop on all port groups :
                on the port group that 'own' the port , set the needed configuration
                on other port groups put 'NULL port'
            */
            PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

            {
                if(finalPortGroupId == portGroupId)
                {
                    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(dev,cntrCfgPtr->port);
                }
                else
                {
                    localPort = PRV_CPSS_DXCH_NULL_PORT_NUM_CNS;
                }

                rc = prvCpssDrvHwPpPortGroupSetRegField(dev, portGroupId,regAddr, 1, 6,localPort);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
            PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)
            break;

        case CPSS_BRG_SECUR_BREACH_DROP_COUNT_VLAN_E:
            finalPortGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

            rc = prvCpssDrvHwPpSetRegField(dev, regAddr, 19, 1, 1);
            if (rc != GT_OK)
                return rc;

            /* check vlan validity */
            PRV_CPSS_VLAN_VALUE_CHECK_MAC(cntrCfgPtr->vlan);

            rc = prvCpssDrvHwPpSetRegField(dev, regAddr, 7, 12,
                                       cntrCfgPtr->vlan);
            break;

        default:
            return GT_BAD_PARAM;
    }

    if(rc == GT_OK)
    {
        /* save the info for 'get' configuration and 'read' counters */
        PRV_CPSS_DXCH_PP_MAC(dev)->portGroupsExtraInfo.
                securBreachDropCounterInfo.counterMode = cntrCfgPtr->dropCntMode;
        PRV_CPSS_DXCH_PP_MAC(dev)->portGroupsExtraInfo.
                securBreachDropCounterInfo.portGroupId = finalPortGroupId;
    }

    return rc;

}

/*******************************************************************************
* cpssDxChBrgSecurBreachPortVlanDropCntrModeGet
*
* DESCRIPTION:
*       Get mode and it's configuration parameters for security breach
*       Port/VLAN drop counter.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - physical device number
*
* OUTPUTS:
*       cntrCfgPtr          - (pointer to) security breach port/vlan counter
*                             mode and it's configuration parameters.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachPortVlanDropCntrModeGet
(
    IN  GT_U8                                        dev,
    OUT CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC     *cntrCfgPtr
)
{
    GT_STATUS   rc;             /* return code */
    GT_U32      regAddr;        /* hw register address */
    GT_U32      value;          /* value to read from register */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    CPSS_NULL_PTR_CHECK_MAC(cntrCfgPtr);

    /* get address of Bridge Configuration Register1 */
    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.bridgeGlobalConfigRegArray[1];

    /* get the info from DB */
    portGroupId = PRV_CPSS_DXCH_PP_MAC(dev)->portGroupsExtraInfo.
                securBreachDropCounterInfo.portGroupId;

    /* get security bridge counter mode, vlan and port */
    rc = prvCpssDrvHwPpPortGroupGetRegField(dev, portGroupId,regAddr, 1, 19, &value);
    if (rc != GT_OK)
        return rc;

    if ((value >> 18) & 0x1)
    {
        /* counts all packets assigned with VID */
        cntrCfgPtr->dropCntMode = CPSS_BRG_SECUR_BREACH_DROP_COUNT_VLAN_E;
    }
    else
    {
        /* counts all packets received on security breach drop port*/
        cntrCfgPtr->dropCntMode = CPSS_BRG_SECUR_BREACH_DROP_COUNT_PORT_E;
    }

    cntrCfgPtr->port = (GT_U8)(value & 0x3F);
    cntrCfgPtr->vlan = (GT_U16)((value >> 6) & 0xFFF);

    /* support multi-port-groups device , convert local port to global port */
    cntrCfgPtr->port = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(dev,portGroupId,cntrCfgPtr->port);

    return rc;

}

/*******************************************************************************
* cpssDxChBrgSecurBreachGlobalDropCntrGet
*
* DESCRIPTION:
*       Get values of security breach global drop counter. Global security breach
*       counter counts all dropped packets due to security breach event.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev             - physical device number
*
* OUTPUTS:
*       counValuePtr    - (pointer to) secure breach global counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachGlobalDropCntrGet
(
    IN  GT_U8    dev,
    OUT GT_U32   *counValuePtr
)
{
    return cpssDxChBrgSecurBreachPortGroupGlobalDropCntrGet(dev,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                            counValuePtr);
}

/*******************************************************************************
* cpssDxChBrgSecurBreachPortVlanCntrGet
*
* DESCRIPTION:
*       Get value of security breach Port/Vlan drop counter. This counter counts
*       all of the packets received according to Port/Vlan security breach drop
*       count mode configuration and dropped due to any security breach event.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev             - physical device number
*
* OUTPUTS:
*       counValuePtr    - (pointer to) secure breach Port/Vlan drop counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachPortVlanCntrGet
(
    IN  GT_U8    dev,
    OUT GT_U32   *counValuePtr
)
{
    return cpssDxChBrgSecurBreachPortGroupPortVlanCntrGet(dev, 
                                              CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                          counValuePtr);
}

/*******************************************************************************
* cpssDxChBrgSecurBreachNaPerPortSet
*
* DESCRIPTION:
*       Enable/Disable per port the unknown Source Addresses to be considered as
*       security breach event. When enabled, the NA message is not sent to CPU
*       and this address is not learned.
*       Only relevant in controlled learning mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - physical device number
*       port                - port
*       enable              - GT_TRUE - New Address is security breach event
*                             GT_FALSE - New Address is not security breach event
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachNaPerPortSet
(
    IN GT_U8                dev,
    IN GT_U8                port,
    IN GT_BOOL              enable
)
{
    GT_STATUS   rc;             /* return code */
    GT_U32      regAddr;        /* hw register address */
    GT_U32      value;          /* value to write to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(dev,port);

    /* get address of Ingress Port Bridge Configuration Register0 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.portControl[port];

    value = BOOL2BIT_MAC(enable);

    rc = prvCpssDrvHwPpPortGroupSetRegField(dev,PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, port),
            regAddr, 20, 1, value);

    return rc;

}

/*******************************************************************************
* cpssDxChBrgSecurBreachNaPerPortGet
*
* DESCRIPTION:
*       Get per port the status of NA security breach event (enabled/disabled).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - physical device number
*       port                - port number
*
* OUTPUTS:
*       enablePtr           - GT_TRUE - NA is security breach event
*                             GT_FALSE - NA is not security breach event
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachNaPerPortGet
(
    IN  GT_U8                dev,
    IN  GT_U8                port,
    OUT GT_BOOL              *enablePtr
)
{
    GT_STATUS   rc;             /* return code */
    GT_U32      regAddr;        /* hw register address */
    GT_U32      value;          /* value to read from register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(dev,port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* get address of Ingress Port Bridge Configuration Register0 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.portControl[port];

    rc = prvCpssDrvHwPpPortGroupGetRegField(dev, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, port),
            regAddr, 20, 1, &value);

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgSecurBreachMovedStaticAddrSet
*
* DESCRIPTION:
*       When the FDB entry is signed as a static one, then this entry is not
*       subject to aging. In addition, there may be cases where the interface
*       does't match the interface from which this packet was received. In this
*       case, this feature enables/disables to regard moved static adresses as
*       a security breach event.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - physical device number
*       enable              - GT_TRUE - moved static addresses are regarded as
*                                   security breach event and are dropped.
*                             GT_FALSE - moved static addresses are not regarded
*                                   as security breach event and are forwarded.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachMovedStaticAddrSet
(
    IN GT_U8                dev,
    IN GT_BOOL              enable
)
{
    GT_U32      regAddr;        /* hw register address */
    GT_U32      value;          /* value to write to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);

    /* get address of Bridge Global Configuration Register0 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.
        bridgeGlobalConfigRegArray[0];

    value = BOOL2BIT_MAC(enable);

    return prvCpssDrvHwPpSetRegField(dev, regAddr, 14, 1, value);
}

/*******************************************************************************
* cpssDxChBrgSecurBreachMovedStaticAddrGet
*
* DESCRIPTION:
*       Get if static addresses that are moved, are regarded as Security Breach
*       or not (this is only in case that the fdb entry is static or the
*       interface does't match the interface from which this packet was received).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - physical device number
*
* OUTPUTS:
*       enablePtr           - GT_TRUE - moved static addresses are regarded as
*                                   security breach event and are dropped.
*                             GT_FALSE - moved static addresses are not regarded
*                                   as security breach event and are forwarded.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachMovedStaticAddrGet
(
    IN  GT_U8                dev,
    OUT GT_BOOL              *enablePtr
)
{
    GT_STATUS   rc;             /* return code */
    GT_U32      regAddr;        /* hw register address */
    GT_U32      value;          /* value to write to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* get address of Bridge Global Configuration Register0 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.
        bridgeGlobalConfigRegArray[0];

    rc = prvCpssDrvHwPpGetRegField(dev, regAddr, 14, 1, &value);

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;
}


/*******************************************************************************
* cpssDxChBrgSecurBreachEventDropModeSet
*
* DESCRIPTION:
*       Set for given security breach event it's drop mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - physical device number
*       eventType           - security breach event type.
*       dropMode            - soft or hard drop mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The acceptable events are:
*           CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachEventDropModeSet
(
    IN GT_U8                                dev,
    IN CPSS_BRG_SECUR_BREACH_EVENTS_ENT     eventType,
    IN CPSS_DROP_MODE_TYPE_ENT              dropMode
)
{
    GT_STATUS   rc;         /* return code                          */
    GT_U32      regAddr;    /* register's address                   */
    GT_U32      setBit;     /* bit number to set in the register    */
    GT_U32      bitValue;   /* the value to write to register       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);

    if (eventType == CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E)
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.
                                        bridgeGlobalConfigRegArray[0];
    else
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.
                                        bridgeGlobalConfigRegArray[1];

    switch (eventType) {

        case CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E:
             if (PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(dev))
             {
                 setBit = 0;
             }
             else
             {
                 setBit = 25;
             }                 
             break;

        case CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E:
             setBit = 26;
             break;

        case CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E:
             setBit = 27;
             break;

        case CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E:
             setBit = 30;
             break;

        case CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E:
             setBit = 15;
             break;

        default:
             return GT_BAD_PARAM;
    }

    switch (dropMode)
    {                                                                                                                                        \
        case CPSS_DROP_MODE_SOFT_E:
            bitValue = 0x0;
            break;
        case CPSS_DROP_MODE_HARD_E:
            bitValue = 0x1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    rc = prvCpssDrvHwPpSetRegField(dev, regAddr, setBit, 1, bitValue);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgSecurBreachEventDropModeGet
*
* DESCRIPTION:
*      Get for given security breach event it's drop mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - physical device number
*       eventType           - security breach event type.
*
* OUTPUTS:
*       dropModePtr         - (pointer to) drop mode - soft or hard.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The acceptable events are:
*           CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachEventDropModeGet
(
    IN  GT_U8                               dev,
    IN  CPSS_BRG_SECUR_BREACH_EVENTS_ENT    eventType,
    OUT CPSS_DROP_MODE_TYPE_ENT             *dropModePtr
)
{
    GT_STATUS   rc;         /* return code                          */
    GT_U32      regAddr;    /* register's address                   */
    GT_U32      getBit;     /* bit number to read from  register    */
    GT_U32      hwValue;    /* hw value to read from register       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    CPSS_NULL_PTR_CHECK_MAC(dropModePtr);

    if (eventType == CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E)
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.
                                        bridgeGlobalConfigRegArray[0];
    else
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.
                                        bridgeGlobalConfigRegArray[1];

    switch (eventType) {

        case CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E:
             if (PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(dev))
             {
                 getBit = 0;
             }
             else
             {
                 getBit = 25;
             }                 
             break;

        case CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E:
             getBit = 26;
             break;

        case CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E:
             getBit = 27;
             break;

        case CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E:
             getBit = 30;
             break;

        case CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E:
             getBit = 15;
             break;

        default:
             return GT_BAD_PARAM;
    }

    rc = prvCpssDrvHwPpGetRegField(dev, regAddr, getBit, 1, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

     *dropModePtr = (hwValue == 0) ? CPSS_DROP_MODE_SOFT_E :
                                     CPSS_DROP_MODE_HARD_E;

    return GT_OK;
}
/*******************************************************************************
* cpssDxChSecurBreachMsgGet
*
* DESCRIPTION:
*       Reads the Security Breach message update. When a security breach occures
*       and the CPU had read the previous message (the message is locked untill
*       CPU reads it, after it the new message can be written) the security
*       massage parameters are updated. Those parameters include: MAC address of
*       the breaching packet, security breach port, VID and security breach code.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev      - device number
*
* OUTPUTS:
*       sbMsgPtr    - (pointer to) security breach message
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChSecurBreachMsgGet
(
    IN  GT_U8                           dev,
    OUT CPSS_BRG_SECUR_BREACH_MSG_STC   *sbMsgPtr
)
{
    GT_STATUS                           rc;
    GT_U32                              regAddr;      /* register address */
    GT_U32                              regVal;       /* register value */
    CPSS_BRG_SECUR_BREACH_EVENTS_ENT    breachCode; /* for conversion Cheetah2's breach Code */
    GT_U32                              portGroupId; /*the port group Id - support
                                                       multi-port-groups device */
    GT_U32                              portGroupsBmp; /* port groups to query -
                                                          support multi-port-groups device */
    GT_U8                               localPort;    /* Local port number for specific port
                                                         group */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    CPSS_NULL_PTR_CHECK_MAC(sbMsgPtr);

    /* get address of Security Breach Status Register0 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.
                                    bridgeSecurBreachStatusRegArray[0];


    if(0 == PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
    {
        portGroupId = PRV_CPSS_PP_MAC(dev)->portGroupsInfo.firstActivePortGroup;
    }
    else
    {
        portGroupId =
            PRV_CPSS_PP_MAC(dev)->portGroupsInfo.secureBreachNextPortGroupToServe;

        /* start with bmp of all active port groups */
        portGroupsBmp = PRV_CPSS_PP_MAC(dev)->portGroupsInfo.activePortGroupsBmp;


        /* look for port group to query */
        while(0 == (portGroupsBmp & (1 << portGroupId)))
        {
            if(portGroupId > PRV_CPSS_PP_MAC(dev)->portGroupsInfo.lastActivePortGroup)
            {
                portGroupId = PRV_CPSS_PP_MAC(dev)->portGroupsInfo.firstActivePortGroup;
                break;
            }

            portGroupId++;
        }

        /* save the info about the next port group to be served */
        PRV_CPSS_PP_MAC(dev)->portGroupsInfo.secureBreachNextPortGroupToServe =
            (portGroupId + 1);
    }


    rc = prvCpssDrvHwPpPortGroupReadRegister(dev, portGroupId, regAddr, &regVal);
    if (rc != GT_OK)
    {
        return rc;
    }
    /* Take Bits[31:0] of source MAC address of breaching packet */
    sbMsgPtr->macSa.arEther[5] = (GT_U8)U32_GET_FIELD_MAC(regVal, 0, 8);
    sbMsgPtr->macSa.arEther[4] = (GT_U8)U32_GET_FIELD_MAC(regVal, 8, 8);
    sbMsgPtr->macSa.arEther[3] = (GT_U8)U32_GET_FIELD_MAC(regVal, 16, 8);
    sbMsgPtr->macSa.arEther[2] = (GT_U8)U32_GET_FIELD_MAC(regVal, 24, 8);

    /* Read Security Breach Status Register1 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.
                                bridgeSecurBreachStatusRegArray[1];

    rc = prvCpssDrvHwPpPortGroupReadRegister(dev, portGroupId, regAddr, &regVal);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Take Bits[32:47] of source MAC address of breaching packet */
    sbMsgPtr->macSa.arEther[1] = (GT_U8)U32_GET_FIELD_MAC(regVal, 0, 8);
    sbMsgPtr->macSa.arEther[0] = (GT_U8)U32_GET_FIELD_MAC(regVal, 8, 8);

    /* Read Security Breach Status Register2 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.
                                    bridgeSecurBreachStatusRegArray[2];

    rc = prvCpssDrvHwPpPortGroupReadRegister(dev, portGroupId, regAddr, &regVal);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* local source port on which the security breaching packet was received */
    localPort = (GT_U8)U32_GET_FIELD_MAC(regVal, 16, 6);

    /* Convert Local port to Global one */
    sbMsgPtr->port =
        PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(dev, portGroupId, localPort);

    /* check the type of the Cheetah family */
    if(PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
    {
        breachCode = U32_GET_FIELD_MAC(regVal, 22, 5);
    }
    else
    {
        breachCode = U32_GET_FIELD_MAC(regVal, 0, 4);

        if (breachCode > 8)
        {
            return GT_BAD_PARAM;
        }

        /* convert Cheetah values to Cheetah2 values */
        if (breachCode == 7)
        {
            /* CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NA_E */
            breachCode = 2;
        }
        else if (breachCode == 8)
        {
            /* GT_SECUR_BREACH_CODE_VLAN_NA_E */
            breachCode = 20;
        }
        /* CPSS_BRG_SECUR_BREACH_EVENTS_FDB_ENTRY_E is the same for both Cheetas */
        else if (breachCode > 1)
        {
            /* increase the breach code for Cheetah2 value */
            breachCode++;
        }
    }

    /* type of security breach, hw values are identical to sw */
    sbMsgPtr->code = breachCode;

    /* vid assigned to the security breaching packet */
    sbMsgPtr->vlan = (GT_U16)U32_GET_FIELD_MAC(regVal, 4, 12);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgSecurBreachPortGroupPortVlanCntrGet
*
* DESCRIPTION:
*       Get value of security breach Port/Vlan drop counter. This counter counts
*       all of the packets received according to Port/Vlan security breach drop
*       count mode configuration and dropped due to any security breach event.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev             - physical device number
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
*       counValuePtr    - (pointer to) secure breach Port/Vlan drop counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachPortGroupPortVlanCntrGet
(
    IN  GT_U8                   dev,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    OUT GT_U32                  *counValuePtr
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    CPSS_NULL_PTR_CHECK_MAC(counValuePtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(dev, portGroupsBmp);

    /* get address of Global Security Breach Filter Counter */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.
        bridgePortVlanSecurBreachDropCounter;

    rc = prvCpssDxChPortGroupsBmpCounterSummary(dev, portGroupsBmp,
                                                regAddr, 0, 32,
                                                counValuePtr, NULL);

    return rc;

}

/*******************************************************************************
* cpssDxChBrgSecurBreachPortGroupGlobalDropCntrGet
*
* DESCRIPTION:
*       Get values of security breach global drop counter. Global security breach
*       counter counts all dropped packets due to security breach event.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev             - physical device number
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
*       counValuePtr    - (pointer to) secure breach global counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachPortGroupGlobalDropCntrGet
(
    IN  GT_U8                   dev,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    OUT GT_U32                  *counValuePtr
)
{
    GT_U32      regAddr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    CPSS_NULL_PTR_CHECK_MAC(counValuePtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(dev, portGroupsBmp);

    /* get address of Global Security Breach Filter Counter */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.
        bridgeGlobalSecurBreachDropCounter;

    return prvCpssDxChPortGroupsBmpCounterSummary(dev, portGroupsBmp,
                                                  regAddr, 0, 32, 
                                                  counValuePtr, NULL);
}

