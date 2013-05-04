/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgEgrFlt.c
*
* DESCRIPTION:
*       Egress filtering facility DxCh cpss implementation
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgEgrFlt.h>

/*******************************************************************************
* cpssDxChBrgPortEgrFltUnkEnable
*
* DESCRIPTION:
*      Enable/Disable egress Filtering for bridged Unknown Unicast packets
*      on the specified egress port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev      - device number
*       port     - CPU port, physical port number
*       enable   - GT_TRUE: Unknown Unicast packets are filtered and are
*                           not forwarded to this port.
*                  GT_FALSE: Unknown Unicast packets are not filtered and may
*                           be forwarded to this port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgrFltUnkEnable
(
    IN GT_U8    dev,
    IN GT_U8    port,
    IN GT_BOOL  enable
)
{
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(dev,port);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, port);
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(dev,port);

        return prvCpssDrvHwPpPortGroupSetRegField(dev,portGroupId,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilter,
                 localPort, 1, BOOL2BIT_MAC(enable));
    }
    else
    {
        return prvCpssDxChHwPpPortGroupSetRegField(dev,portGroupId,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.egr.
                    filterConfig.unknownUcFilterEn[OFFSET_TO_WORD_MAC(port)],
                 OFFSET_TO_BIT_MAC(port), 1, BOOL2BIT_MAC(enable));
    }
}

/*******************************************************************************
* cpssDxChBrgPortEgrFltUnkEnableGet
*
* DESCRIPTION:
*      This function gets the egress Filtering current state (enable/disable)
*      for bridged Unknown Unicast packets on the specified egress port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*       port        - CPU port, physical port number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) bridged unknown unicast packets filtering
*                   GT_TRUE:  Unknown Unicast packets are filtered and are
*                             not forwarded to this port.
*                   GT_FALSE: Unknown Unicast packets are not filtered and may
*                             be forwarded to this port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgrFltUnkEnableGet
(
    IN  GT_U8   dev,
    IN  GT_U8   port,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_STATUS   rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(dev,port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, port);
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(dev,port);

        rc = prvCpssDrvHwPpPortGroupGetRegField(dev,portGroupId,
                                       PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilter,
                                       localPort, 1, &value);
    }
    else
    {
        rc = prvCpssDxChHwPpPortGroupGetRegField(dev,portGroupId,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.egr.
                    filterConfig.unknownUcFilterEn[OFFSET_TO_WORD_MAC(port)],
                 OFFSET_TO_BIT_MAC(port), 1, &value);
    }

    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgPortEgrFltUregMcastEnable
*
* DESCRIPTION:
*      Enable/Disable egress Filtering for bridged Unregistered Multicast packets
*      on the specified egress port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev      - device number
*       port     - CPU port, physical port number
*       enable   - GT_TRUE: Unregistered Multicast packets are filtered and
*                           are not forwarded to this port.
*                  GT_FALSE:Unregistered Multicast packets are not filtered
*                           and may be forwarded to this port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgrFltUregMcastEnable
(
    IN GT_U8    dev,
    IN GT_U8    port,
    IN GT_BOOL  enable
)
{
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(dev,port);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, port);
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(dev,port);

        return prvCpssDrvHwPpPortGroupSetRegField(dev,portGroupId,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilter2,
                 localPort, 1, BOOL2BIT_MAC(enable));
    }
    else
    {
        return prvCpssDxChHwPpPortGroupSetRegField(dev,portGroupId,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.egr.
                    filterConfig.unregisteredMcFilterEn[OFFSET_TO_WORD_MAC(port)],
                 OFFSET_TO_BIT_MAC(port), 1, BOOL2BIT_MAC(enable));
    }
}

/*******************************************************************************
* cpssDxChBrgPortEgrFltUregMcastEnableGet
*
* DESCRIPTION:
*      This function gets the egress Filtering current state (enable/disable)
*      for bridged Unregistered Multicast packets on the specified egress port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*       port        - CPU port, physical port number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) bridged unregistered multicast packets filtering
*                   GT_TRUE:  Unregistered Multicast packets are filtered and
*                             are not forwarded to this port.
*                   GT_FALSE: Unregistered Multicast packets are not filtered
*                             and may be forwarded to this port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgrFltUregMcastEnableGet
(
    IN  GT_U8   dev,
    IN  GT_U8   port,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_STATUS   rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(dev,port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, port);
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(dev,port);

        rc = prvCpssDrvHwPpPortGroupGetRegField(dev,portGroupId,
                                       PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilter2,
                                       localPort, 1, &value);
    }
    else
    {
        rc = prvCpssDxChHwPpPortGroupGetRegField(dev,portGroupId,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.egr.
                    filterConfig.unregisteredMcFilterEn[OFFSET_TO_WORD_MAC(port)],
                 OFFSET_TO_BIT_MAC(port), 1, &value);
    }

    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgVlanEgressFilteringEnable
*
* DESCRIPTION:
*       Enable/Disable VLAN Egress Filtering on specified device for Bridged
*       Known Unicast packets.
*       If enabled the VLAN egress filter verifies that the egress port is a
*       member of the VID assigned to the packet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev      - device number
*       enable   - GT_TRUE: VLAN egress filtering verifies that the egress
*                          port is a member of the packet's VLAN classification
*                 GT_FLASE: the VLAN egress filtering check is disabled.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong dev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       VLAN egress filtering is required by 802.1Q, but if desired, this
*       mechanism can be disabled, thus allowing "leaky VLANs".
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEgressFilteringEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  enable
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        return prvCpssDrvHwPpSetRegField(dev,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilter,
                 29, 1, BOOL2BIT_MAC(enable));
    }
    else
    {
        return prvCpssDxChHwPpSetRegField(dev,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.sht.global.shtGlobalConfig,
                 0, 1, BOOL2BIT_MAC(enable));
    }
}

/*******************************************************************************
* cpssDxChBrgVlanEgressFilteringEnableGet
*
* DESCRIPTION:
*      This function gets the VLAN Egress Filtering current state (enable/disable)
*      on specified device for Bridged Known Unicast packets.
*      If enabled the VLAN egress filter verifies that the egress port is a
*      member of the VID assigned to the packet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) bridged known unicast packets filtering
*                   GT_TRUE:   VLAN egress filtering verifies that the egress
*                              port is a member of the packet's VLAN classification
*                   GT_FLASE: the VLAN egress filtering check is disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEgressFilteringEnableGet
(
    IN  GT_U8   dev,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        rc = prvCpssDrvHwPpGetRegField(dev,
                                       PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilter,
                                       29, 1, &value);
    }
    else
    {
        rc =  prvCpssDxChHwPpGetRegField(dev,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.sht.global.shtGlobalConfig,
                 0, 1, &value);
    }

    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgRoutedUnicastEgressFilteringEnable
*
* DESCRIPTION:
*       Enable/Disable VLAN egress filtering on Routed Unicast packets.
*       If disabled, the destination port may or may not be a member of the VLAN.
*       If enabled, the VLAN egress filter verifies that the egress port is a
*       member of the VID assigned to the packet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev      - device number
*       enable   - GT_TRUE: Egress filtering is enabled
*                  GT_FLASE: Egress filtering is disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong dev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgRoutedUnicastEgressFilteringEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  enable
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        return prvCpssDrvHwPpSetRegField(dev,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilter,
                 28, 1, BOOL2BIT_MAC(enable));
    }
    else
    {
        return prvCpssDxChHwPpSetRegField(dev,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.sht.global.shtGlobalConfig,
                 1, 1, BOOL2BIT_MAC(enable));
    }
}

/*******************************************************************************
* cpssDxChBrgRoutedUnicastEgressFilteringEnableGet
*
* DESCRIPTION:
*      This function gets the VLAN Egress Filtering current state (enable/disable)
*      on Routed Unicast packets.
*      If disabled, the destination port may or may not be a member of the VLAN.
*      If enabled, the VLAN egress filter verifies that the egress port is a
*      member of the VID assigned to the packet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) routed unicast packets filtering
*                   GT_TRUE:  Egress filtering is enabled
*                   GT_FLASE: Egress filtering is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgRoutedUnicastEgressFilteringEnableGet
(
    IN  GT_U8   dev,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        rc = prvCpssDrvHwPpGetRegField(dev,
                                       PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilter,
                                       28, 1, &value);
    }
    else
    {
        rc = prvCpssDxChHwPpGetRegField(dev,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.sht.global.shtGlobalConfig,
                 1, 1, &value);
    }

    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgRoutedSpanEgressFilteringEnable
*
* DESCRIPTION:
*       Enable/Disable STP egress Filtering on Routed Packets.
*       If disabled the packet may be forwarded to its egress port,
*       regardless of its Span State.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev      - device number
*       enable   - GT_TRUE: Span state filtering is enabled
*                  GT_FLASE: Span state filtering is disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong dev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgRoutedSpanEgressFilteringEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  enable
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        return prvCpssDrvHwPpSetRegField(dev,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilter,
                 31, 1, BOOL2BIT_MAC(enable));
    }
    else
    {
        return prvCpssDxChHwPpSetRegField(dev,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.sht.global.shtGlobalConfig,
                 2, 1, BOOL2BIT_MAC(enable));
    }
}

/*******************************************************************************
* cpssDxChBrgRoutedSpanEgressFilteringEnableGet
*
* DESCRIPTION:
*      This function gets the STP egress Filtering current state (enable/disable)
*      on Routed packets.
*      If disabled the packet may be forwarded to its egress port,
*      regardless of its Span State.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) routed packets filtering
*                   GT_TRUE:  Span state filtering is enabled
*                   GT_FLASE: Span state filtering is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgRoutedSpanEgressFilteringEnableGet
(
    IN  GT_U8   dev,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        rc = prvCpssDrvHwPpGetRegField(dev,
                                       PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilter,
                                       31, 1, &value);
    }
    else
    {
        rc = prvCpssDxChHwPpGetRegField(dev,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.sht.global.shtGlobalConfig,
                 2, 1, &value);
    }

    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgPortEgrFltUregBcEnable
*
* DESCRIPTION:
*       Enables or disables egress filtering of unregistered broadcast packets.
*       Unregistered broadcast packets are:
*        - packets with destination MAC ff-ff-ff-ff-ff-ff
*        - destination MAC address lookup not finds matching entry
*        - packets were not routed
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       dev     - device number
*       port    - port number
*       enable  - GT_TRUE  - enable filtering of unregistered broadcast packets.
*                            unregistered broadcast packets are dropped
*                 GT_FALSE - disable filtering of unregistered broadcast packets.
*                            unregistered broadcast packets are not dropped
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong dev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgrFltUregBcEnable
(
    IN GT_U8       dev,
    IN GT_U8       port,
    IN GT_BOOL     enable
)
{
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    /* cheetah2 support it , but cheetah not */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(dev, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(dev,port);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, port);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(dev,port);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        return prvCpssDrvHwPpPortGroupSetRegField(dev,portGroupId,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilterUnregBc,
                 localPort, 1, BOOL2BIT_MAC(enable));
    }
    else
    {
        return prvCpssDxChHwPpPortGroupSetRegField(dev,portGroupId,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->
                    txqVer1.egr.filterConfig.unregisteredBcFilterEn[OFFSET_TO_WORD_MAC(port)],
                 OFFSET_TO_BIT_MAC(port), 1, BOOL2BIT_MAC(enable));
    }
}

/*******************************************************************************
* cpssDxChBrgPortEgrFltUregBcEnableGet
*
* DESCRIPTION:
*      This function gets the egress Filtering current state (enable/disable)
*      of unregistered broadcast packets.
*      Unregistered broadcast packets are:
*      - packets with destination MAC ff-ff-ff-ff-ff-ff
*      - destination MAC address lookup not finds matching entry
*      - packets were not routed
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       dev         - device number
*       port        - port number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) unregistered broadcast packets filtering
*                   GT_TRUE  - enable filtering of unregistered broadcast packets.
*                              unregistered broadcast packets are dropped
*                   GT_FALSE - disable filtering of unregistered broadcast packets.
*                              unregistered broadcast packets are not dropped
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgrFltUregBcEnableGet
(
    IN  GT_U8   dev,
    IN  GT_U8    port,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_STATUS   rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    /* cheetah2 support it , but cheetah not */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(dev, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(dev,port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, port);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(dev,port);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        rc = prvCpssDrvHwPpPortGroupGetRegField(dev,portGroupId,
                                       PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilterUnregBc,
                                       localPort, 1, &value);
    }
    else
    {
        rc = prvCpssDxChHwPpPortGroupGetRegField(dev,portGroupId,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->
                    txqVer1.egr.filterConfig.unregisteredBcFilterEn[OFFSET_TO_WORD_MAC(port)],
                 OFFSET_TO_BIT_MAC(port), 1, &value);
    }

    if (rc != GT_OK)
        return rc;

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgPortEgressMcastLocalEnable
*
* DESCRIPTION:
*       Enable/Disable sending Multicast packets back to its source
*       port on the local device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev     - device number
*       port    - port number
*       enable  - Boolean value:
*                 GT_TRUE  - Multicast packets may be sent back to
*                            their source port on the local device.
*                 GT_FALSE - Multicast packets are not sent back to
*                            their source port on the local device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong dev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For xCat and above devices to enable local switching of Multicast,
*       unknown Unicast, and Broadcast traffic, both egress port configuration
*       and the field in the VLAN entry (by function
*       cpssDxChBrgVlanLocalSwitchingEnableSet) must be enabled.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgressMcastLocalEnable
(
    IN GT_U8       dev,
    IN GT_U8       port,
    IN GT_BOOL     enable
)
{
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(dev,port);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, port);
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(dev,port);

        return prvCpssDrvHwPpPortGroupSetRegField(dev,portGroupId,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.mcastLocalEnableConfig,
                 localPort, 1, BOOL2BIT_MAC(enable));
    }
    else
    {
        return prvCpssDxChHwPpPortGroupSetRegField(dev,portGroupId,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.egr.
                    filterConfig.mcLocalEn[OFFSET_TO_WORD_MAC(port)],
                 OFFSET_TO_BIT_MAC(port), 1, BOOL2BIT_MAC(enable));
    }


}

/*******************************************************************************
* cpssDxChBrgPortEgressMcastLocalEnableGet
*
* DESCRIPTION:
*      This function gets current state (enable/disable) for sending
*      Multicast packets back to its source port on the local device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*       port        - port number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) sending Multicast packets back to its source
*                   GT_TRUE  - Multicast packets may be sent back to
*                              their source port on the local device.
*                   GT_FALSE - Multicast packets are not sent back to
*                              their source port on the local device.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgressMcastLocalEnableGet
(
    IN  GT_U8   dev,
    IN  GT_U8    port,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_STATUS   rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(dev,port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, port);
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(dev,port);

        rc = prvCpssDrvHwPpPortGroupGetRegField(dev,portGroupId,
                                       PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.mcastLocalEnableConfig,
                                       localPort, 1, &value);
    }
    else
    {
        rc = prvCpssDxChHwPpPortGroupGetRegField(dev,portGroupId,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.egr.
                    filterConfig.mcLocalEn[OFFSET_TO_WORD_MAC(port)],
                 OFFSET_TO_BIT_MAC(port), 1, &value);
    }

    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgPortEgrFltIpMcRoutedEnable
*
* DESCRIPTION:
*      Enable/Disable egress filtering for IP Multicast Routed packets
*      on the specified egress port.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       dev      - device number
*       port     - physical port number
*       enable   - GT_TRUE: IP Multicast Routed packets are filtered and are
*                           not forwarded to this port.
*                  GT_FALSE: IP Multicast Routed packets are not filtered and may
*                           be forwarded to this port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgrFltIpMcRoutedEnable
(
    IN GT_U8    dev,
    IN GT_U8    port,
    IN GT_BOOL  enable
)
{
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(dev,
           CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(dev,port);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, port);
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(dev,port);

        return prvCpssDrvHwPpPortGroupSetRegField(dev,portGroupId,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilterIpMcRouted,
                 localPort, 1, BOOL2BIT_MAC(enable));
    }
    else
    {
        return prvCpssDxChHwPpPortGroupSetRegField(dev,portGroupId,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.egr.
                    filterConfig.ipmcRoutedFilterEn[OFFSET_TO_WORD_MAC(port)],
                 OFFSET_TO_BIT_MAC(port), 1, BOOL2BIT_MAC(enable));
    }
}

/*******************************************************************************
* cpssDxChBrgPortEgrFltIpMcRoutedEnableGet
*
* DESCRIPTION:
*      This function gets the egress Filtering current state (enable/disable)
*      for IP Multicast Routed packets on the specified egress port.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       dev         - device number
*       port        - port number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) IP Multicast Routed packets filtering
*                   GT_TRUE:  IP Multicast Routed packets are filtered and are
*                             not forwarded to this port.
*                   GT_FALSE: IP Multicast Routed packets are not filtered and may
*                             be forwarded to this port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgrFltIpMcRoutedEnableGet
(
    IN  GT_U8   dev,
    IN  GT_U8   port,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_STATUS   rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(dev,
           CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(dev,port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, port);
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(dev,port);

        rc = prvCpssDrvHwPpPortGroupGetRegField(dev,portGroupId,
                                       PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilterIpMcRouted,
                                       localPort, 1, &value);
    }
    else
    {
        rc = prvCpssDxChHwPpPortGroupGetRegField(dev,portGroupId,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.egr.
                    filterConfig.ipmcRoutedFilterEn[OFFSET_TO_WORD_MAC(port)],
                 OFFSET_TO_BIT_MAC(port), 1, &value);
    }

    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;

}

