
/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgPortIsolation.c
*
* DESCRIPTION:
*       CPSS DxCh NST Port Isolation Mechanism.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/nst/cpssDxChNstPortIsolation.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/* width of port isolation table -- number of words */
#define PORT_ISOLATION_NOM_WORDS_CNS    3

/*******************************************************************************
* portIsolationEntryIndexCalc
*
* DESCRIPTION:
*       Function calculates port isolation table index.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum           - device number
*       srcInterfacePtr  - (pointer to) source interface information
*
* OUTPUTS:
*       indexPtr    - (pointer to) port isolation table index
*
* RETURNS:
*       GT_OK         - on success
*       GT_FAIL       - on error.
*       GT_BAD_PARAM  - wrong devNum, portNum, trandId
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS portIsolationEntryIndexCalc
(
    IN GT_U8                                          devNum,
    IN CPSS_INTERFACE_INFO_STC                        *srcInterfacePtr,
    OUT GT_U32                                        *indexPtr
)
{
    devNum = devNum;

    /* Port isolation table index calculation */
    switch (srcInterfacePtr->type)
    {
        case CPSS_INTERFACE_PORT_E:
            if ((srcInterfacePtr->devPort.devNum >= BIT_5) ||
                (srcInterfacePtr->devPort.portNum >= BIT_6))
            {
                return GT_BAD_PARAM;
            }

            *indexPtr = (srcInterfacePtr->devPort.devNum << 6) | (srcInterfacePtr->devPort.portNum);
            break;

        case CPSS_INTERFACE_TRUNK_E:
            if (srcInterfacePtr->trunkId >= BIT_7)
            {
                return GT_BAD_PARAM;
            }
            *indexPtr = 2048 + srcInterfacePtr->trunkId;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChNstPortIsolationEntryAddrCalc
*
* DESCRIPTION:
*       Function calculates port isolation table's entry address.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       trafficType   - packets traffic type - L2 or L3
*       srcInterfacePtr  - source interface
*
* OUTPUTS:
*       regAddrPtr    - (pointer to) table register
*
* RETURNS:
*       GT_OK         - on success
*       GT_FAIL       - on error.
*       GT_BAD_PARAM  - wrong devNum, portNum, trandId
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChNstPortIsolationEntryAddrCalc
(
    IN GT_U8                                          devNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN CPSS_INTERFACE_INFO_STC                        *srcInterfacePtr,
    OUT GT_U32                                        *regAddrPtr
)
{
    GT_STATUS   rc;
    GT_U32  tblIndex;  /* port isolation table index */

    /* calculate index */
    rc = portIsolationEntryIndexCalc(devNum,srcInterfacePtr,&tblIndex);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* Address of L2/L3 Port Isolation Register */
    switch (trafficType)
    {
        case CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E:
            *regAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.l2PortIsolationTableBase + tblIndex * 0x10;
            break;

        case CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E:
            *regAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.l3PortIsolationTableBase + tblIndex * 0x10;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNstPortIsolationEnableSet
*
* DESCRIPTION:
*       Function enables/disables the port isolation feature.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       enable    - port isolation feature enable/disable
*                   GT_TRUE  - enable
*                   GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32      regAddr;    /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);


    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* Address of Transmit Queue Resource Sharing Register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueResSharingAndTunnelEgrFltr;
        return prvCpssDrvHwPpSetRegField(devNum, regAddr, 28, 1, BOOL2BIT_MAC(enable));
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.egr.
                filterConfig.globalEnables;
        return prvCpssDxChHwPpSetRegField(devNum, regAddr, 7, 1, BOOL2BIT_MAC(enable));
    }
}

/*******************************************************************************
* cpssDxChNstPortIsolationEnableGet
*
* DESCRIPTION:
*       Function gets enabled/disabled state of the port isolation feature.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*
* OUTPUTS:
*       enablePtr - (pointer to) port isolation feature state
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationEnableGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32      hwValue;    /* register value */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* Address of Transmit Queue Resource Sharing Register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueResSharingAndTunnelEgrFltr;

        rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 28, 1, &hwValue);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.egr.
                filterConfig.globalEnables;
        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 7, 1, &hwValue);
    }

    /* Convert return value to BOOLEAN type */
    *enablePtr = BIT2BOOL_MAC(hwValue);

    return rc;

}

/*******************************************************************************
* cpssDxChNstPortIsolationTableEntrySet
*
* DESCRIPTION:
*       Function sets port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Each entry holds bitmap of all local device ports (and CPU port), where
*       for each local port there is a bit marking. If it's a member of source
*       interface (if outgoing traffic from this source interface is allowed to
*       go out at this specific local port).
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum               - device number
*       trafficType          - packets traffic type - L2 or L3
*       srcInterface         - table index is calculated from source interface.
*                              Only portDev and Trunk are supported.
*       cpuPortMember        - port isolation for CPU Port
*                               GT_TRUE - member
*                               GT_FALSE - not member
*       localPortsMembersPtr - (pointer to) port bitmap to be written to the
*                              L2/L3 PI table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface or
*                                  localPortsMembersPtr
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Traffic is separated for L2 and L3 (means that application may
*       block L2 traffic while allowing L3).
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationTableEntrySet
(
    IN GT_U8                                          devNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN CPSS_INTERFACE_INFO_STC                        srcInterface,
    IN GT_BOOL                                        cpuPortMember,
    IN CPSS_PORTS_BMP_STC                             *localPortsMembersPtr
)
{
    GT_U32      hwValue;    /* register value */
    GT_U32      regAddr;    /* register address */
    GT_U32      maxPortNum;
    GT_STATUS   rc;         /* return status */
    GT_U32      tblIndex;/*table index*/
    GT_U32      hwValueArr[PORT_ISOLATION_NOM_WORDS_CNS];    /* table entry value */
    PRV_CPSS_DXCH_TABLE_ENT tableType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(localPortsMembersPtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* check port bitmap */
        maxPortNum = PRV_CPSS_PP_MAC(devNum)->numOfPorts;

        if ((localPortsMembersPtr->ports[0] >= ((GT_U32)(1 << maxPortNum))) || (localPortsMembersPtr->ports[1]))
        {
            return GT_BAD_PARAM;
        }

        /* get register address */
        rc = prvCpssDxChNstPortIsolationEntryAddrCalc(devNum, trafficType, &srcInterface, &regAddr);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* combine register value from port bitmap and cpu port */
        hwValue = (localPortsMembersPtr->ports[0]) | (BOOL2BIT_MAC(cpuPortMember) << 28);

        return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 29, hwValue);
    }
    else
    {
        /* calculate index */
        rc = portIsolationEntryIndexCalc(devNum,&srcInterface,&tblIndex);
        if(rc != GT_OK)
        {
            return rc;
        }

        switch(trafficType)
        {
            case CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E:
                tableType = PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L2_E;
                break;
            case CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E:
                tableType = PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L3_E;
                break;
            default:
                return GT_BAD_PARAM;
        }

        hwValueArr[0] =  localPortsMembersPtr->ports[0];
        hwValueArr[1] =  localPortsMembersPtr->ports[1] |
                         (BOOL2BIT_MAC(cpuPortMember) << 31);
        hwValueArr[2] =  0;

        return prvCpssDxChWriteTableEntry(devNum,
                                        tableType,
                                        tblIndex,
                                        hwValueArr);
    }
}

/*******************************************************************************
* cpssDxChNstPortIsolationTableEntryGet
*
* DESCRIPTION:
*       Function gets port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Each entry holds bitmap of all local device ports (and CPU port), where
*       for each local port there is a bit marking if it's a member of source
*       interface (if outgoing traffic from this source interface is allowed to
*       go out at this specific local port).
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum               - device number
*       trafficType          - packets traffic type - L2 or L3
*       srcInterface         - table index is calculated from source interfaces
*                              Only portDev and Trunk are supported.
*
* OUTPUTS:
*       cpuPortMemberPtr     - (pointer to) port isolation for CPU Port
*                                GT_TRUE - member
*                                GT_FALSE - not member
*       localPortsMembersPtr - (pointer to) port bitmap to be written
*                              to the L2/L3 PI table
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Traffic is separated for L2 and L3 (means that application may
*       block L2 traffic while allowing L3).
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationTableEntryGet
(
    IN GT_U8                                          devNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN CPSS_INTERFACE_INFO_STC                        srcInterface,
    OUT GT_BOOL                                       *cpuPortMemberPtr,
    OUT CPSS_PORTS_BMP_STC                            *localPortsMembersPtr
)
{
    GT_U32      hwValue;    /* register value */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* return status */
    GT_U32      tblIndex;/*table index*/
    GT_U32      hwValueArr[PORT_ISOLATION_NOM_WORDS_CNS];    /* table entry value */
    PRV_CPSS_DXCH_TABLE_ENT tableType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(localPortsMembersPtr);
    CPSS_NULL_PTR_CHECK_MAC(cpuPortMemberPtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* get register address */
        rc = prvCpssDxChNstPortIsolationEntryAddrCalc(devNum, trafficType, &srcInterface, &regAddr);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 29, &hwValue);
        if (rc != GT_OK)
        {
            return GT_HW_ERROR;
        }

        localPortsMembersPtr->ports[0] = hwValue & 0x0FFFFFFF;
        localPortsMembersPtr->ports[1] = 0;

        *cpuPortMemberPtr = BIT2BOOL_MAC((hwValue >> 28) & 1);
    }
    else
    {
        /* calculate index */
        rc = portIsolationEntryIndexCalc(devNum,&srcInterface,&tblIndex);
        if(rc != GT_OK)
        {
            return rc;
        }

        switch(trafficType)
        {
            case CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E:
                tableType = PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L2_E;
                break;
            case CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E:
                tableType = PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L3_E;
                break;
            default:
                return GT_BAD_PARAM;
        }

        rc = prvCpssDxChReadTableEntry(devNum,
                                        tableType,
                                        tblIndex,
                                        hwValueArr);
        if(rc != GT_OK)
        {
            return rc;
        }

        localPortsMembersPtr->ports[0] = hwValueArr[0];
        localPortsMembersPtr->ports[1] = hwValueArr[1];

        /* get the CPU port as bit in BMP */
        U32_GET_FIELD_IN_ENTRY_MAC(localPortsMembersPtr->ports,63,1,hwValue);
        *cpuPortMemberPtr = BIT2BOOL_MAC(hwValue);

        /* reset the CPU port as bit in BMP */
        U32_SET_FIELD_IN_ENTRY_MAC(localPortsMembersPtr->ports,63,1,0);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNstPortIsolationPortAdd
*
* DESCRIPTION:
*       Function adds single local port to port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Adding local port (may be also CPU port 63) to port isolation entry
*       means that traffic which came from srcInterface and wish to egress
*       at the specified local port isn't blocked.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       trafficType   - packets traffic type - L2 or L3
*       srcInterface  - table index is calculated from source interface
*                       Only portDev and Trunk are supported.
*       portNum       - local port(include CPU port) to be added to bitmap
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface, portNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationPortAdd
(
    IN GT_U8                                          devNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN CPSS_INTERFACE_INFO_STC                        srcInterface,
    IN GT_U8                                          portNum
)
{
    GT_U32      offset;     /* offset */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* return status */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */
    GT_U32  tblIndex;  /* port isolation table index */
    PRV_CPSS_DXCH_TABLE_ENT tableType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);


    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* check port */
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        /* get register address */
        rc = prvCpssDxChNstPortIsolationEntryAddrCalc(devNum, trafficType, &srcInterface, &regAddr);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* calculate bit's offset */
        if (localPort == CPSS_CPU_PORT_NUM_CNS)
        {
            offset =  28;
        }
        else
        {
            offset =  localPort;
        }

        /* set relevant bit in the bitmap */
        return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, offset , 1, 1);
    }
    else
    {
        /* check port */
        if(portNum > 63)
        {
            /* supports 64 ports */
            return GT_BAD_PARAM;
        }

        /* calculate index */
        rc = portIsolationEntryIndexCalc(devNum,&srcInterface,&tblIndex);
        if(rc != GT_OK)
        {
            return rc;
        }

        switch(trafficType)
        {
            case CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E:
                tableType = PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L2_E;
                break;
            case CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E:
                tableType = PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L3_E;
                break;
            default:
                return GT_BAD_PARAM;
        }

        return prvCpssDxChWriteTableEntryField(devNum,
                                        tableType,
                                        tblIndex,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                        portNum,
                                        1,
                                        1);
    }
}

/*******************************************************************************
* cpssDxChNstPortIsolationPortDelete
*
* DESCRIPTION:
*       Function deletes single local port to port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Deleting local port (may be also CPU port 63) to port isolation entry
*       means that traffic which came from srcInterface and wish to egress
*       at the specified local port is blocked.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       trafficType   - packets traffic type - L2 or L3
*       srcInterface  - table index is calculated from source interface
*                       Only portDev and Trunk are supported.
*       portNum       - local port(include CPU port) to be deleted from bitmap
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface, portNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In srcInterface parameter only portDev and Trunk are supported.
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationPortDelete
(
    IN GT_U8                                          devNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN CPSS_INTERFACE_INFO_STC                        srcInterface,
    IN GT_U8                                          portNum
)
{
    GT_U32      offset;     /* offset */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* return status */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */
    GT_U32  tblIndex;  /* port isolation table index */
    PRV_CPSS_DXCH_TABLE_ENT tableType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);


    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* check port */
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        /* get register address */
        rc = prvCpssDxChNstPortIsolationEntryAddrCalc(devNum, trafficType, &srcInterface, &regAddr);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* calculate bit's offset */
        if (localPort == CPSS_CPU_PORT_NUM_CNS)
        {
            offset =  28;
        }
        else
        {
            offset =  localPort;
        }

        return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, offset , 1, 0);
    }
    else
    {
        /* check port */
        if(portNum > 63)
        {
            /* supports 64 ports */
            return GT_BAD_PARAM;
        }

        /* calculate index */
        rc = portIsolationEntryIndexCalc(devNum,&srcInterface,&tblIndex);
        if(rc != GT_OK)
        {
            return rc;
        }

        switch(trafficType)
        {
            case CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E:
                tableType = PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L2_E;
                break;
            case CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E:
                tableType = PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L3_E;
                break;
            default:
                return GT_BAD_PARAM;
        }

        return prvCpssDxChWriteTableEntryField(devNum,
                                        tableType,
                                        tblIndex,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                        portNum,
                                        1,
                                        0);
    }
}

