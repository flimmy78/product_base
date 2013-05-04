/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgNestVlan.c
*
* DESCRIPTION:
*       CPSS DxCh Nested VLANs facility implementation.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgNestVlan.h>

/*******************************************************************************
* cpssDxChBrgNestVlanAccessPortSet
*
* DESCRIPTION:
*       Configure given port as Nested VLAN access port.
*       The VID of all the packets received on Nested VLAN access port is
*       discarded and they are assigned with the Port VID that set by
*       cpssDxChBrgVlanPortVidSet(). This VLAN assignment may be subsequently
*       overridden by the protocol based VLANs or policy based VLANs algorithms.
*       When a packet received on an access port is transmitted via a core port
*       or a cascading port, a VLAN tag is added to the packet, in addition to
*       any existing VLAN tag.
*       The 802.1p User Priority field of this added tag may be one of the
*       following, based on the ModifyUP QoS parameter set to the packet at
*       the end of the Ingress pipe:
*              - If ModifyUP is "Modify", it is the UP extracted from the
*              QoS Profile table entry that configured by the
*              cpssDxChCosProfileEntrySet().
*              - If ModifyUP is "Not Modify", it is the original packet
*              802.1p User Priority field, if it is tagged. If untagged,
*              it is ingress port's default user priority that configured by
*              cpssDxChPortDefaultUPSet().
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - PP's device number.
*       portNum - physical or CPU port number.
*       enable  -  GT_TRUE  - port is Nested VLAN Access Port.
*                  GT_FALSE - port is usual port (Core or Customer).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To complete Access Port Configuration do the following:
*       For DxCh1, DxCh2 and DxCh3 devices:
*           -  by API cpssDxChBrgNestVlanEtherTypeSelectSet(), set
*              the Port VLAN Selection to the CPSS_VLAN_ETHERTYPE0_E;
*           -  update egress Port VLAN EtherType;
*           -  by API cpssDxChBrgVlanForcePvidEnable(), set
*              Port VID Assignment mode.
*           To complete Core Port Configuration, select CPSS_VLAN_ETHERTYPE1_E
*           for Usual Port.
*           To complete Customers Bridges(bridges that don't employ Nested VLANs)
*           Configuration, select CPSS_VLAN_ETHERTYPE0_E for Usual Port.
*       For xCat and above devices:
*           See CPSS user guide how to configure Nested VLAN or TR101 features.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgNestVlanAccessPortSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_BOOL     enable
)
{
    GT_U32  hwData;      /* data to write to HW */
    GT_U32  fieldOffset; /* offset of the field in the VLAN QoS Table */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

    /* Convert boolean value to the HW format */
    hwData = (enable == GT_TRUE) ? 1 : 0;

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        fieldOffset = 28;
    }
    else
    {
        fieldOffset = 24;
    }

    /* Update port mode in the VLAN and QoS Configuration Entry */
    return prvCpssDxChWriteTableEntryField(devNum,
                                           PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                           (GT_U32)portNum,
                                           0,       /* start at word 0 */
                                           fieldOffset, /* start at bit 24 or 28 */
                                           1,       /* 1 bit */
                                           hwData);
}

/*******************************************************************************
* cpssDxChBrgNestVlanAccessPortGet
*
* DESCRIPTION:
*       Gets configuration of the given port (Nested VLAN access port
*       or Core/Customer port).
*       The VID of all the packets received on Nested VLAN access port is
*       discarded and they are assigned with the Port VID that set by
*       cpssDxChBrgVlanPortVidSet(). This VLAN assignment may be subsequently
*       overridden by the protocol based VLANs or policy based VLANs algorithms.
*       When a packet received on an access port is transmitted via a core port
*       or a cascading port, a VLAN tag is added to the packet, in addition to
*       any existing VLAN tag.
*       The 802.1p User Priority field of this added tag may be one of the
*       following, based on the ModifyUP QoS parameter set to the packet at
*       the end of the Ingress pipe:
*              - If ModifyUP is "Modify", it is the UP extracted from the
*              QoS Profile table entry that configured by the
*              cpssDxChCosProfileEntrySet().
*              - If ModifyUP is "Not Modify", it is the original packet
*              802.1p User Priority field, if it is tagged. If untagged,
*              it is ingress port's default user priority that configured by
*              cpssDxChPortDefaultUPSet().
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - PP's device number.
*       portNum     - physical or CPU port number.
*
* OUTPUTS:
*       enablePtr   - Pointer to the Boolean value:
*                     GT_TRUE   - port is Nested VLAN Access Port.
*                     GT_FALSE  - port is usual port:
*                                      - Core Port in case of
*                                         CPSS_VLAN_ETHERTYPE1_E selected;
*                                      - Customers Bridges (bridges that don't
*                                         employ Nested VLANs) in case of
*                                         CPSS_VLAN_ETHERTYPE0_E selected;
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum values.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgNestVlanAccessPortGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32      hwData;     /* data to write to HW */
    GT_STATUS   retStatus;  /* generic return status code */
    GT_U32      fieldOffset; /* offset of the field in the VLAN QoS Table */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        fieldOffset = 28;
    }
    else
    {
        fieldOffset = 24;
    }
    /* Read port mode from VLAN and QoS Configuration Entry */
    retStatus = prvCpssDxChReadTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         (GT_U32)portNum,
                                         0,        /* start at word 0 */
                                         fieldOffset, /* start at bit 24 or 28 */
                                         1,        /* 1 bit */
                                         &hwData);
    if(GT_OK == retStatus)
    {
        /* Convert gathered HW value to the SW format */
        *enablePtr = (hwData == 1) ? GT_TRUE : GT_FALSE;
    }

    return retStatus;
}

/*******************************************************************************
* cpssDxChBrgNestVlanEtherTypeSelectSet
*
* DESCRIPTION:
*       Sets selected port VLAN EtherType.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* INPUTS:
*       devNum          - PP's device number.
*       portNum         - physical or CPU port number.
*       ingressVlanSel  - selected port VLAN EtherType.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or portNum or ingressVlanSel values.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For the VLAN EtherType coherency between Ingress and Egress,
*       the VLAN EtherType shall be configured to match the ingressVlanSel.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgNestVlanEtherTypeSelectSet
(
    IN  GT_U8                   devNum,
    IN  GT_U8                   portNum,
    IN  CPSS_ETHER_MODE_ENT     ingressVlanSel
)
{
    GT_U32                          hwData;         /* data to write to HW */
    GT_STATUS                       retStatus;      /* gen return status code */
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC  *regsAddrPtr;   /* pointer to reg address */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

    /* Get EhterType HW format */
    switch (ingressVlanSel)
    {
        case CPSS_VLAN_ETHERTYPE0_E:
            hwData = 0;
            break;
        case CPSS_VLAN_ETHERTYPE1_E:
            hwData = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* Set selected VLAN EtherType in the VLAN and QoS Configuration Entry */
    retStatus = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         (GT_U32)portNum,
                                         0,         /* start at word 0 */
                                         22,        /* start at bit 22 */
                                         1,         /* 1 bit */
                                         hwData);

    /********************************************************/
    /* For the VLAN EtherType coherency between Ingress and */
    /* Egress, update Egress VLAN EtherType Select Register */
    /********************************************************/

    if (GT_OK == retStatus)
    {
        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        /* Update portNum in case of CPU Port */
        if (localPort == CPSS_CPU_PORT_NUM_CNS)
        {
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
            {
                localPort = 27;
            }
            else
            {
                localPort = 31;
            }
        }

        regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);
        retStatus = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,
                               regsAddrPtr->bridgeRegs.egressVlanEtherTypeSel,
                               (GT_U32)localPort, /* start at portNum bit */
                               1,               /* 1 bit */
                               hwData);
    }

    return retStatus;
}

/*******************************************************************************
* cpssDxChBrgNestVlanEtherTypeSelectGet
*
* DESCRIPTION:
*       Gets selected port VLAN EtherType.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* INPUTS:
*       devNum              - PP's device number.
*       portNum             - physical or CPU port number.
*
* OUTPUTS:
*       ingressVlanSelPtr   - pointer to selected port VLAN EtherType.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum values.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgNestVlanEtherTypeSelectGet
(
    IN  GT_U8                   devNum,
    IN  GT_U8                   portNum,
    OUT CPSS_ETHER_MODE_ENT     *ingressVlanSelPtr
)
{
    GT_U32      hwData;     /* data to write to HW */
    GT_STATUS   retStatus;  /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(ingressVlanSelPtr);

    /* Read selected VLAN EtherType from the VLAN and QoS Configuration Entry */
    retStatus = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        (GT_U32)portNum,
                                        0, /* start at word 0 */
                                        22,/* start at bit 24 */
                                        1, /* 1 bit */
                                        &hwData);
    if(GT_OK == retStatus)
    {
        /* Convert gathered HW value to the SW format */
        *ingressVlanSelPtr = (hwData == 1) ?
            CPSS_VLAN_ETHERTYPE1_E : CPSS_VLAN_ETHERTYPE0_E;
    }

    return retStatus;
}
