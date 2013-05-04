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
* cpssDxChBrgSrcId.c
*
* DESCRIPTION:
*       CPSS DxCh Source ID facility implementation
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/private/prvCpssCommonDefs.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgSrcId.h>


/*******************************************************************************
* prvCpssDxChBrgSrcIdGroupTableSet
*
* DESCRIPTION:
*        Set a bit per port indicating if the packet with this Source-ID may be
*        forwarded to this port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       sourceId - Source ID number that the port is added to (0..31)
*       portNum  - Physical port number, CPU port
*       action   - GT_FALSE  - the port is not a member of this Source ID group
*                              and packets assigned with this Source ID are not
*                              forwarded to this port.
*                - GT_TRUE  - the portis a member of this Source ID group and
*                             packets assigned with this Source ID may be
*                             forwarded to this port.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - wrong devNum, portNum, sourceId
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*
*******************************************************************************/

static GT_STATUS prvCpssDxChBrgSrcIdGroupTableSet
(
    IN GT_U8   devNum,
    IN GT_U32  sourceId,
    IN GT_U8   portNum,
    IN GT_BOOL action
)
{
    GT_U32 data;        /* Data to be written into the register */
    GT_U32 regAddr;     /* register address */
    GT_U32 fieldOffset; /* The start bit number in the register */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    if(sourceId >= BIT_5)
        return GT_BAD_PARAM;

    data = (action == GT_TRUE) ? 1 : 0;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        fieldOffset = localPort;

        if(localPort == CPSS_CPU_PORT_NUM_CNS)
        {
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
            {
                fieldOffset = 27; /* CPU port is bit 27 in Cheetah */
            }
            else
            {
                fieldOffset = 31; /* CPU port is bit 31 in Cheetah2 */
            }
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                              bufferMng.srcIdEggFltrTbl[sourceId];
        return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, fieldOffset, 1, data);
    }
    else
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                    PRV_CPSS_DXCH_LION_TABLE_TXQ_SOURCE_ID_MEMBERS_E,
                                    sourceId,
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                    portNum,
                                    1,
                                    data);
    }


}

/*******************************************************************************
* cpssDxChBrgSrcIdGroupPortAdd
*
* DESCRIPTION:
*         Add a port to Source ID group. Packets assigned with this Source ID
*         may be forwarded to this port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       sourceId - Source ID  number that the port is added to (0..31).
*       portNum  - Physical port number, CPU port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum, sourceId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdGroupPortAdd
(
    IN GT_U8   devNum,
    IN GT_U32  sourceId,
    IN GT_U8   portNum
)
{
    return prvCpssDxChBrgSrcIdGroupTableSet(devNum,sourceId,portNum,GT_TRUE);
}

/*******************************************************************************
* cpssDxChBrgSrcIdGroupPortDelete
*
* DESCRIPTION:
*         Delete a port from Source ID group. Packets assigned with this
*         Source ID will not be forwarded to this port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       sourceId - Source ID  number that the port is added to (0..31).
*       portNum  - Physical port number, CPU port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum, sourceId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdGroupPortDelete
(
    IN GT_U8   devNum,
    IN GT_U32  sourceId,
    IN GT_U8   portNum
)
{
    return prvCpssDxChBrgSrcIdGroupTableSet(devNum,sourceId,portNum,GT_FALSE);
}

/*******************************************************************************
* portGroupSrcIdGroupEntrySet
*
* DESCRIPTION:
*        Set entry in Source ID Egress Filtering table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupId          - the port group Id . to support multi-port-groups device
*       sourceId        - Source ID  number (0..31).
*       maxPortNum      - max number of bits for the ports
*       cpuPortBit      - the bit for the CPU port
*       cpuSrcIdMember  - GT_TRUE - CPU is member of of Src ID group.
*                         GT_FALSE - CPU isn't member of of Src ID group.
*       portsMembersPtr - pointer to bitmap of ports that are
*                         Source ID Members of specified PP device.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, sourceId, ports bitmap value
*       GT_BAD_PTR               - portsMembersPtr is NULL pointer
*       GT_OUT_OF_RANGE          - length of portsMembersPtr is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS portGroupSrcIdGroupEntrySet
(
    IN GT_U8               devNum,
    IN GT_U32              portGroupId,
    IN GT_U32              sourceId,
    IN GT_U32              maxPortNum,
    IN GT_U32              cpuPortBit,
    IN GT_BOOL             cpuSrcIdMember,
    IN CPSS_PORTS_BMP_STC  *portsMembersPtr
)
{
    GT_U32 regAddr;           /* register address */
    GT_U32 data;              /* data to be written into the register */
    GT_U32 cpuPortBitValue;   /* CPU port bit value*/
    GT_STATUS rc;             /* return value */
    CPSS_PORTS_BMP_STC  tmpPortsBmp;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                              bufferMng.srcIdEggFltrTbl[sourceId];


        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum,portGroupId, regAddr, &data);
        if(rc != GT_OK)
            return rc;

        /* set port bitmap */
        U32_SET_FIELD_MAC(data,0,maxPortNum,portsMembersPtr->ports[0]);

        cpuPortBitValue = (cpuSrcIdMember == GT_TRUE) ? 1 : 0;

        /* set CPU bit */
        U32_SET_FIELD_MAC(data,cpuPortBit,1,cpuPortBitValue);

        /* Write bitmap of ports are belonged to the Source ID */
        return prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId, regAddr, data);
    }
    else
    {
        tmpPortsBmp = *portsMembersPtr;

        /* set the CPU port as bit in BMP */
        U32_SET_FIELD_IN_ENTRY_MAC(tmpPortsBmp.ports,63,1,(BOOL2BIT_MAC(cpuSrcIdMember)));

        return prvCpssDxChWriteTableEntry(devNum,
                                    PRV_CPSS_DXCH_LION_TABLE_TXQ_SOURCE_ID_MEMBERS_E,
                                    sourceId,
                                    tmpPortsBmp.ports);
    }
}

/*******************************************************************************
* cpssDxChBrgSrcIdGroupEntrySet
*
* DESCRIPTION:
*        Set entry in Source ID Egress Filtering table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       sourceId       - Source ID  number (0..31).
*       cpuSrcIdMember - GT_TRUE - CPU is member of of Src ID group.
*                        GT_FALSE - CPU isn't member of of Src ID group.
*       portsMembersPtr - pointer to the bitmap of ports that are
*                        Source ID Members of specified PP device.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, sourceId, ports bitmap value
*       GT_BAD_PTR               - portsMembersPtr is NULL pointer
*       GT_OUT_OF_RANGE          - length of portsMembersPtr is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/

GT_STATUS cpssDxChBrgSrcIdGroupEntrySet
(
    IN GT_U8               devNum,
    IN GT_U32              sourceId,
    IN GT_BOOL             cpuSrcIdMember,
    IN CPSS_PORTS_BMP_STC  *portsMembersPtr
)
{
    GT_U32 cpuPortBit;        /* CPU port bit */
    GT_U32 maxPortNum;        /* max port number in the Source ID group */
                              /* DxCh2 - 28; DxCh - 27 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(portsMembersPtr);

    if(sourceId >= BIT_5)
      return GT_BAD_PARAM;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
        {
            maxPortNum = 27;
            cpuPortBit = 27;

        }
        else
        {
            maxPortNum = 28;
            cpuPortBit = 31;

        }

        if (portsMembersPtr->ports[0] >= ((GT_U32)(1 << maxPortNum)))
        {
            return GT_OUT_OF_RANGE;
        }
    }
    else
    {
        maxPortNum = 0;/* don't care */
        cpuPortBit = 0;/* don't care */
    }

    return portGroupSrcIdGroupEntrySet(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,sourceId,
            maxPortNum,cpuPortBit,cpuSrcIdMember,portsMembersPtr);
}

/*******************************************************************************
* cpssDxChBrgSrcIdGroupEntryGet
*
* DESCRIPTION:
*        Get entry in Source ID Egress Filtering table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       sourceId          - Source ID number (0..31).
*
* OUTPUTS:
*       cpuSrcIdMemberPtr - GT_TRUE - CPU is member of of Src ID group.
*                           GT_FALSE - CPU isn't member of of Src ID group.
*       portsMembersPtr   - pointer to the bitmap of ports are Source ID Members
*                           of specified PP device.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, sourceId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/

GT_STATUS cpssDxChBrgSrcIdGroupEntryGet
(
    IN  GT_U8               devNum,
    IN  GT_U32              sourceId,
    OUT GT_BOOL             *cpuSrcIdMemberPtr,
    OUT CPSS_PORTS_BMP_STC  *portsMembersPtr
)
{
    GT_U32    regAddr;      /* register address */
    GT_U32    data;         /* data to be read from the register */
    GT_U32    cpuPortBit;   /* CPU port bit */
    GT_U32    maxPortNum;   /* max port number in the Source ID group */
                            /* DxCh2 - 28; DxCh - 27 */
    GT_STATUS rc;           /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(cpuSrcIdMemberPtr);
    CPSS_NULL_PTR_CHECK_MAC(portsMembersPtr);
    if(sourceId >= BIT_5)
        return GT_BAD_PARAM;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        cpssOsMemSet(portsMembersPtr, 0, sizeof(CPSS_PORTS_BMP_STC));

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                              bufferMng.srcIdEggFltrTbl[sourceId];

        rc =  prvCpssDrvHwPpReadRegister(devNum,regAddr,&data);
        if(rc != GT_OK)
            return rc;

        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
        {
            maxPortNum = 27;
            cpuPortBit = 27;

        }
        else
        {
            maxPortNum = 28;
            cpuPortBit = 31;
        }

        /* set port bitmap excluding CPU bit */
        U32_SET_FIELD_MASKED_MAC(portsMembersPtr->ports[0],0,maxPortNum,data);

        if(((data >> cpuPortBit) & 0x1) == 1)
        {
            *cpuSrcIdMemberPtr = GT_TRUE;
        }
        else
        {
            *cpuSrcIdMemberPtr = GT_FALSE;
        }
    }
    else
    {
        rc = prvCpssDxChReadTableEntry(devNum,
                                    PRV_CPSS_DXCH_LION_TABLE_TXQ_SOURCE_ID_MEMBERS_E,
                                    sourceId,
                                    portsMembersPtr->ports);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* get the CPU port as bit in BMP */
        U32_GET_FIELD_IN_ENTRY_MAC(portsMembersPtr->ports,63,1,data);
        *cpuSrcIdMemberPtr = BIT2BOOL_MAC(data);

        /* reset the CPU port as bit in BMP */
        U32_SET_FIELD_IN_ENTRY_MAC(portsMembersPtr->ports,63,1,0);
    }


    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgSrcIdPortDefaultSrcIdSet
*
* DESCRIPTION:
*       Configure Port's Default Source ID.
*       The Source ID is used for source based egress filtering.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portNum         - Physical port number, CPU port
*       defaultSrcId    - Port's Default Source ID (0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum, defaultSrcId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdPortDefaultSrcIdSet
(
    IN GT_U8   devNum,
    IN GT_U8   portNum,
    IN GT_U32  defaultSrcId
)
{

    GT_U32 regAddr;  /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    if(defaultSrcId >= BIT_5)
        return GT_BAD_PARAM;

    if(portNum == CPSS_CPU_PORT_NUM_CNS)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.cpuPortControl;
    }

    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                               bridgeRegs.portControl[portNum];
    }

    return prvCpssDrvHwPpPortGroupSetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, 6, 5, defaultSrcId);
}

/*******************************************************************************
* cpssDxChBrgSrcIdPortDefaultSrcIdGet
*
* DESCRIPTION:
*       Get configuration of Port's Default Source ID.
*       The Source ID is used for Source based egress filtering.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       portNum           - Physical port number, CPU port
*
* OUTPUTS:
*       defaultSrcIdPtr   - Port's Default Source ID
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdPortDefaultSrcIdGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_U32  *defaultSrcIdPtr
)
{

    GT_U32 regAddr;  /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(defaultSrcIdPtr);

    if(portNum == CPSS_CPU_PORT_NUM_CNS)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.cpuPortControl;
    }

    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                               bridgeRegs.portControl[portNum];
    }

    return prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, 6, 5, defaultSrcIdPtr);
}


/*******************************************************************************
* cpssDxChBrgSrcIdGlobalUcastEgressFilterSet
*
* DESCRIPTION:
*     Enable or disable Source ID egress filter for unicast
*     packets. The Source ID filter is configured by
*     cpssDxChBrgSrcIdGroupPortAdd and cpssDxChBrgSrcIdGroupPortDelete.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion.
*
* INPUTS:
*       devNum   - device number
*       enable   - GT_TRUE -  enable Source ID filtering on unicast packets.
*                             Unicast packet is dropped if egress port is not
*                             member in the Source ID group.
*                - GT_FALSE - disable Source ID filtering on unicast packets.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdGlobalUcastEgressFilterSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32  regAddr;      /* register address */
    GT_U32  data;         /* reg subfield data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_LION_E);

    data = (enable == GT_TRUE) ? 1 : 0;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfigExt;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 29, 1, data);
}

/*******************************************************************************
* cpssDxChBrgSrcIdGlobalUcastEgressFilterGet
*
* DESCRIPTION:
*     Get Source ID egress filter configuration for unicast
*     packets. The Source ID filter is configured by
*     cpssDxChBrgSrcIdGroupPortAdd and cpssDxChBrgSrcIdGroupPortDelete.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       enablePtr   - GT_TRUE -  enable Source ID filtering on unicast packets.
*                             Unicast packet is dropped if egress port is not
*                             member in the Source ID group.
*                   - GT_FALSE - disable Source ID filtering on unicast packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdGlobalUcastEgressFilterGet
(
    IN GT_U8     devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32    regAddr;      /* register address */
    GT_U32    data;         /* reg subfield data */
    GT_STATUS rc;           /* return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_LION_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfigExt;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 29, 1, &data);
    if(rc != GT_OK)
        return rc;

    *enablePtr = (data == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;

}


/*******************************************************************************
* cpssDxChBrgSrcIdGlobalSrcIdAssignModeSet
*
* DESCRIPTION:
*         Set Source ID Assignment mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       mode      - the assignment mode of the packet Source ID.
*                   CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_DA_PORT_DEFAULT_E mode is
*                   supported for xCat and above.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdGlobalSrcIdAssignModeSet
(
    IN GT_U8                            devNum,
    IN CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT  mode
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 data;        /* reg subfield data */
    GT_U32 fieldOffset; /* field offset */
    GT_U32 fieldLength; /* field length */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH2_E | CPSS_CH3_E);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        /* FDBbasedSrcID Assign Mode [14:13] */
        fieldLength = 2;
        fieldOffset = 13;
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                         bridgeRegs.bridgeGlobalConfigRegArray[2];
    }
    else
    {
        /* UsePortDefault-SrcId [4] */
        fieldLength = 1;
        fieldOffset = 4;
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                         bridgeRegs.bridgeGlobalConfigRegArray[0];
    }

    switch(mode)
    {
        /* XCAT A1 and above:
            0 = Disable: FDB source ID assignment is disabled
            1 = DA based: FDB source ID assignment is enabled for DA-based assignment
            2 = SA based: FDB source ID assignment is enabled for SA-based assignment
        */
        /* DxCh1
            0 = Assign non-DSA tagged packets with the Source-ID configured in the
                source address FDB entry.
            1 = Assign non-DSA tagged packets with the Source-ID according to the
                port default configuration (ignoring the source FDB entry Source-ID
                assignment)
        */
        case CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E:
            data = (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E) ? 2 : 0;
            break;
        case CPSS_BRG_SRC_ID_ASSIGN_MODE_PORT_DEFAULT_E:
            data = (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E) ? 0 : 1;
            break;
        case CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_DA_PORT_DEFAULT_E:
            if (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
            {
                data = 1;
            }
            else
            {
                return GT_BAD_PARAM;
            }
            break;
        default:
            return GT_BAD_PARAM;
    }

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, fieldOffset, fieldLength, data);
}

/*******************************************************************************
* cpssDxChBrgSrcIdGlobalSrcIdAssignModeGet
*
* DESCRIPTION:
*         Get Source ID Assignment mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*
* OUTPUTS:
*       modePtr      - the assignment mode of the packet Source ID
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdGlobalSrcIdAssignModeGet
(
    IN  GT_U8                            devNum,
    OUT CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT  *modePtr
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 data;        /* reg subfield data */
    GT_STATUS rc;       /* return value */
    GT_U32 fieldOffset; /* field offset */
    GT_U32 fieldLength; /* field length */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        /* FDBbasedSrcID Assign Mode [14:13] */
        fieldLength = 2;
        fieldOffset = 13;
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                         bridgeRegs.bridgeGlobalConfigRegArray[2];
    }
    else
    {
        /* UsePortDefault-SrcId [4] */
        fieldLength = 1;
        fieldOffset = 4;
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                         bridgeRegs.bridgeGlobalConfigRegArray[0];
    }

    rc =  prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, fieldLength, &data);
    if(rc != GT_OK)
        return rc;

    switch(data)
    {
        case 0:
            *modePtr = (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
                        ? CPSS_BRG_SRC_ID_ASSIGN_MODE_PORT_DEFAULT_E
                        : CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E;
            break;
        case 1:
            *modePtr = (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
                        ? CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_DA_PORT_DEFAULT_E
                        : CPSS_BRG_SRC_ID_ASSIGN_MODE_PORT_DEFAULT_E;
            break;
        default:
            *modePtr = CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E;
            break;
    }

    return GT_OK;
}


/*******************************************************************************
* cpssDxChBrgSrcIdPortSrcIdAssignModeSet
*
* DESCRIPTION:
*         Set Source ID Assignment mode for non-DSA tagged packets
*         per Ingress Port.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum    - device number
*       portNum   - Physical port number, CPU port
*       mode      - the assignment mode of the packet Source ID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum, mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdPortSrcIdAssignModeSet
(
    IN GT_U8                            devNum,
    IN GT_U8                            portNum,
    IN CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT  mode
)
{

    GT_U32 regAddr;     /* register address */
    GT_U32 regData;     /* register data */
    GT_U32 data;        /* reg subfield data */
    GT_U32 fieldOffset; /* The start bit number in the register */
    GT_STATUS rc;       /* return value */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    /* DxCh2, DxCh3 are supported */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                   bridgeRegs.srcIdReg.srcIdAssignedModeConfReg;

    if(localPort == CPSS_CPU_PORT_NUM_CNS)
        fieldOffset = 31; /* CPU port is bit 31 */
    else
        fieldOffset = localPort;

    switch(mode)
    {
       case CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E:
            data = 0;
            break;
       case CPSS_BRG_SRC_ID_ASSIGN_MODE_PORT_DEFAULT_E:
            data = 1;
            break;
       default:
            return GT_BAD_PARAM;
    }
    /* FEr#1084 -  Wrong data when reading Source-ID Assignment Mode Conf. reg. */
    if(PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
          PRV_CPSS_DXCH2_READ_SRC_ID_ASSIGMENT_MODE_CONF_REG_WA_E) == GT_TRUE)
    {
        /* NOTE: this WA not relevant to the Lion - multi-port-groups device ,
            otherwise DB of srcIdAssignedModeConfRegData should have been per port group */
        regData =  (PRV_CPSS_DXCH_PP_MAC(devNum))->
                                            errata.srcIdAssignedModeConfRegData;
        U32_SET_FIELD_MAC(regData,fieldOffset,1,data);

        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regData);
        if(rc != GT_OK)
            return rc;

        (PRV_CPSS_DXCH_PP_MAC(devNum))->errata.srcIdAssignedModeConfRegData =
                                                                        regData;
        return GT_OK;

    }
    else
    {
        return prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId, regAddr, fieldOffset, 1, data);

    }

}

/*******************************************************************************
* cpssDxChBrgSrcIdPortSrcIdAssignModeGet
*
* DESCRIPTION:
*         Get Source ID Assignment mode for non-DSA tagged packets
*         per Ingress Port.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum    - device number
*       portNum   - Physical port number, CPU port
*
* OUTPUTS:
*       modePtr   - the assignment mode of the packet Source ID
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdPortSrcIdAssignModeGet
(
    IN GT_U8                            devNum,
    IN GT_U8                            portNum,
    OUT CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT  *modePtr
)
{

    GT_U32 regAddr;     /* register address */
    GT_U32 data;        /* reg subfield data */
    GT_U32 fieldOffset; /* The start bit number in the register */
    GT_STATUS rc;       /* return value */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    /* DxCh2, DxCh3 are supported */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                   bridgeRegs.srcIdReg.srcIdAssignedModeConfReg;

    if(localPort == CPSS_CPU_PORT_NUM_CNS)
        fieldOffset = 31; /* CPU port is bit 31 */
    else
        fieldOffset = localPort;

    /* FEr#1084 -  Wrong data when reading Source-ID Assignment Mode Conf. reg. */
    if(PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
          PRV_CPSS_DXCH2_READ_SRC_ID_ASSIGMENT_MODE_CONF_REG_WA_E) == GT_TRUE)
    {
        /* NOTE: this WA not relevant to the Lion - multi-port-groups device ,
            otherwise DB of srcIdAssignedModeConfRegData should have been per port group */

        data =  (PRV_CPSS_DXCH_PP_MAC(devNum))->
                                            errata.srcIdAssignedModeConfRegData;

        data = (data >> fieldOffset) & 1;
    }
    else
    {
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, fieldOffset, 1, &data);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    switch(data)
    {
       case 0:
            *modePtr = CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E;
            break;
       case 1:
            *modePtr = CPSS_BRG_SRC_ID_ASSIGN_MODE_PORT_DEFAULT_E;
            break;
       default:
            break;
    }

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgSrcIdPortUcastEgressFilterSet
*
* DESCRIPTION:
*     Per Egress Port enable or disable Source ID egress filter for unicast
*     packets. The Source ID filter is configured by
*     cpssDxChBrgSrcIdGroupPortAdd and cpssDxChBrgSrcIdGroupPortDelete.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*       portNum  - Physical port number/all ports wild card, CPU port
*       enable   - GT_TRUE - enable Source ID filtering on unicast packets
*                            forwarded to this port. Unicast packet is dropped
*                            if egress port is not member in the Source ID group.
*                - GT_FALSE - disable Source ID filtering on unicast packets
*                             forwarded to this port.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdPortUcastEgressFilterSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  enable
)
{
    GT_U32  regAddr;      /* register address */
    GT_U32  data;         /* reg subfield data */
    GT_U32  fieldOffset;  /* The start bit number in the register */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    data = (enable == GT_TRUE) ? 1 : 0;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* CPU port is bit 31 */
        fieldOffset = (localPort == CPSS_CPU_PORT_NUM_CNS) ? 31 : localPort;

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                     bridgeRegs.srcIdReg.srcIdUcEgressFilterConfReg;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.egr.filterConfig.ucSrcIdFilterEn[OFFSET_TO_WORD_MAC(portNum)];
        fieldOffset = OFFSET_TO_BIT_MAC(portNum);
    }

    return prvCpssDxChHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, fieldOffset, 1, data);
}

/*******************************************************************************
* cpssDxChBrgSrcIdPortUcastEgressFilterGet
*
* DESCRIPTION:
*     Get Per Egress Port Source ID egress filter configuration for unicast
*     packets. The Source ID filter is configured by
*     cpssDxChBrgSrcIdGroupPortAdd and cpssDxChBrgSrcIdGroupPortDelete.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*       portNum  - Physical port number/all ports wild card, CPU port
*
* OUTPUTS:
*       enablePtr   - GT_TRUE - enable Source ID filtering on unicast packets
*                            forwarded to this port. Unicast packet is dropped
*                            if egress port is not member in the Source ID group.
*                   - GT_FALSE - disable Source ID filtering on unicast packets
*                             forwarded to this port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdPortUcastEgressFilterGet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32  regAddr;      /* register address */
    GT_U32  data;         /* reg subfield data */
    GT_U32  fieldOffset;  /* The start bit number in the register */
    GT_STATUS rc;         /* return value */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* CPU port is bit 31 */
        fieldOffset = (localPort == CPSS_CPU_PORT_NUM_CNS) ? 31 : localPort;

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                     bridgeRegs.srcIdReg.srcIdUcEgressFilterConfReg;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.egr.filterConfig.ucSrcIdFilterEn[OFFSET_TO_WORD_MAC(portNum)];
        fieldOffset = OFFSET_TO_BIT_MAC(portNum);
    }


    rc = prvCpssDxChHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, fieldOffset, 1, &data);
    if(rc != GT_OK)
        return rc;

    *enablePtr = (data == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgSrcIdPortSrcIdForceEnableSet
*
* DESCRIPTION:
*         Set Source ID Assignment force mode per Ingress Port.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       portNum   - Physical port number, CPU port
*       enable    - enable/disable SourceID force mode
*                     GT_TRUE - enable Source ID force mode
*                     GT_FALSE - disable Source ID force mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdPortSrcIdForceEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  enable
)
{
    GT_U32  regAddr;     /* register address */
    GT_U32  data;        /* data to write into the register */
    GT_U32  fieldOffset; /* field offset */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                   bridgeRegs.srcIdReg.srcIdAssignedModeConfReg;

    if(localPort == CPSS_CPU_PORT_NUM_CNS)
        fieldOffset = 31; /* CPU port is bit 31 */
    else
        fieldOffset = localPort;

    data = BOOL2BIT_MAC(enable);

    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, fieldOffset, 1, data);
}

/*******************************************************************************
* cpssDxChBrgSrcIdPortSrcIdForceEnableGet
*
* DESCRIPTION:
*         Get Source ID Assignment force mode per Ingress Port.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number, CPU port
*
* OUTPUTS:
*       enablePtr - (pointer to) Source ID Assignment force mode state
*                     GT_TRUE - Source ID force mode is enabled
*                     GT_FALSE - Source ID force mode is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdPortSrcIdForceEnableGet
(
    IN GT_U8     devNum,
    IN GT_U8     portNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32  regAddr;     /* register address */
    GT_U32  data;        /* data to write into the register */
    GT_U32  fieldOffset; /* field offset */
    GT_STATUS rc;        /* returned status */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                   bridgeRegs.srcIdReg.srcIdAssignedModeConfReg;

    if(localPort == CPSS_CPU_PORT_NUM_CNS)
        fieldOffset = 31; /* CPU port is bit 31 */
    else
        fieldOffset = localPort;

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, regAddr, fieldOffset, 1, &data);
    if (GT_OK != rc)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(data);

    return GT_OK;
}


