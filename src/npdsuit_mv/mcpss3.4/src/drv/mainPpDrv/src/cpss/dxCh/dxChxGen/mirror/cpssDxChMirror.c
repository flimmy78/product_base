/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChMirror.c
*
* DESCRIPTION:
*       CPSS DxCh Mirror APIs implementation.
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/


#include <cpss/dxCh/dxChxGen/mirror/cpssDxChMirror.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwTables.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h>
#include <cpss/generic/cos/cpssCosTypes.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/dxCh/dxChxGen/mirror/cpssDxChStc.h>

/* Convert analyzer port and device to null port and device */
#define CORE_MIRR_CONVERT_ANALYZER_FOR_CPU_OR_NULL_PORT_MAC(                       \
                       _analyzDev, _analyzPort, _ownDevice, _devNullPort)      \
{                                                                              \
    if (_analyzPort == CPSS_CPU_PORT_NUM_CNS)                                  \
    {                                                                          \
        _analyzDev = _ownDevice;                                               \
    }                                                                          \
    else                                                                       \
    if (_analyzPort == CPSS_NULL_PORT_NUM_CNS)                                 \
    {                                                                          \
        _analyzDev = _ownDevice;                                               \
        _analyzPort = _devNullPort;                                            \
    }                                                                          \
}

/* maximal index of analyzer */
#define  PRV_CPSS_DXCH_MIRROR_ANALYZER_MAX_INDEX_CNS 6

/*******************************************************************************
* cpssDxChMirrorAnalyzerVlanTagEnable
*
* DESCRIPTION:
*       Enable/Disable Analyzer port adding additional VLAN Tag to mirrored
*       packets. The content of this VLAN tag is configured
*       by cpssDxChMirrorTxAnalyzerVlanTagConfig.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum         - device number.
*       portNum     - ports number to be set
*       enable      - GT_TRUE - additional VLAN tag inserted to mirrored packets
*                     GT_FALSE - no additional VLAN tag inserted to mirrored packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorAnalyzerVlanTagEnable
(
    IN GT_U8                    devNum,
    IN GT_U8                    portNum,
    IN GT_BOOL                  enable
)
{
    GT_U32      regAddr;        /* register address */
    GT_STATUS   rc;             /* function return code */
    GT_U32      fieldValue;     /* register's field value */
    GT_U32      mirrPort;       /* mirror port*/
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    /* Assign field value */
    fieldValue = BOOL2BIT_MAC(enable);

    mirrPort = OFFSET_TO_BIT_MAC(localPort);

    /* TO ANALYZER VLAN Adding Configuration Registers */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
        toAnalyzerVlanAddConfig[OFFSET_TO_WORD_MAC(localPort)];

    /* Bit per port indicating if a Vlan Tag is to be added to TO_ANALYZER Packets */
    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, mirrPort, 1, fieldValue);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorAnalyzerVlanTagEnableGet
*
* DESCRIPTION:
*       Get Analyzer port VLAN Tag to mirrored packets mode.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum         - device number.
*       portNum     - ports number to be set
*
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE - additional VLAN tag inserted to mirrored packets
*                     GT_FALSE - no additional VLAN tag inserted to mirrored packets
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorAnalyzerVlanTagEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32      regAddr;        /* register address */
    GT_STATUS   rc;             /* function return code */
    GT_U32      fieldValue;     /* register's field value */
    GT_U32      mirrPort;       /* mirror port*/
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    mirrPort = OFFSET_TO_BIT_MAC(localPort);

    /* TO ANALYZER VLAN Adding Configuration Registers */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
        toAnalyzerVlanAddConfig[OFFSET_TO_WORD_MAC(localPort)];

    /* Bit per port indicating if a Vlan Tag is to be added to TO_ANALYZER Packets */
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, mirrPort, 1, &fieldValue);
    if(rc != GT_OK)
        return rc;

    /* Assign field value */
    *enablePtr = ((fieldValue != 0) ? GT_TRUE : GT_FALSE);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorTxAnalyzerVlanTagConfig
*
* DESCRIPTION:
*      Set global Tx mirroring VLAN tag details: EtherType, VID, VPT, CFI
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - device number.
*      analyzerVlanTagConfigPtr - Pointer to analyzer Vlan tag configuration:
*                              EtherType, VPT, CFI and VID
*
* OUTPUTS:
*      None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxAnalyzerVlanTagConfig
(
    IN GT_U8                                       devNum,
    IN CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC *analyzerVlanTagConfigPtr
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      regData;        /* register data */
    GT_STATUS   rc;             /* function return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(analyzerVlanTagConfigPtr);
    PRV_CPSS_DXCH_COS_CHECK_UP_MAC(analyzerVlanTagConfigPtr->vpt);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(analyzerVlanTagConfigPtr->vid);
    if (analyzerVlanTagConfigPtr->cfi > 1)
    {
        return GT_BAD_PARAM;
    }

    /* VLAN tag data vid+cfi+vpt+ethertype */
    regData = (analyzerVlanTagConfigPtr->vid) |
              (analyzerVlanTagConfigPtr->cfi << 12) |
              (analyzerVlanTagConfigPtr->vpt << 13) |
              (analyzerVlanTagConfigPtr->etherType << 16);

    /* Egress Analyzer VLAN Tag Configuration Register */
    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.egrAnalyzerVlanTagConfig;

    rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, regData);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorTxAnalyzerVlanTagConfigGet
*
* DESCRIPTION:
*      Get global Tx mirroring VLAN tag details: EtherType, VID, VPT, CFI
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - device number.
*
* OUTPUTS:
*      analyzerVlanTagConfigPtr - Pointer to analyzer Vlan tag configuration:
*                              EtherType, VPT, CFI and VID
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxAnalyzerVlanTagConfigGet
(
    IN GT_U8                                       devNum,
    OUT CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC *analyzerVlanTagConfigPtr
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      regData;        /* register data */
    GT_STATUS   rc;             /* function return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(analyzerVlanTagConfigPtr);


    /* Egress Analyzer VLAN Tag Configuration Register */
    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.egrAnalyzerVlanTagConfig;

    rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &regData);
    if(rc != GT_OK)
        return rc;

    /* VLAN tag data vid+cfi+vpt+ethertype */
    analyzerVlanTagConfigPtr->vid = (GT_U16)(regData & 0xFFF);
    analyzerVlanTagConfigPtr->cfi = (GT_U8)((regData & 0x1000) >> 12);
    analyzerVlanTagConfigPtr->vpt = (GT_U8)((regData & 0xE000) >> 13);
    analyzerVlanTagConfigPtr->etherType = (GT_U16)((regData & 0xFFFF0000) >> 16);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorRxAnalyzerVlanTagConfig
*
* DESCRIPTION:
*      Set global Rx mirroring VLAN tag details: EtherType, VID, VPT, CFI
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - device number.
*      analyzerVlanTagConfigPtr - Pointer to analyzer Vlan tag configuration:
*                              EtherType, VPT, CFI and VID
*
* OUTPUTS:
*      None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorRxAnalyzerVlanTagConfig
(
    IN GT_U8                                       devNum,
    IN CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC *analyzerVlanTagConfigPtr
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      regData;        /* register data */
    GT_STATUS   rc;             /* function return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(analyzerVlanTagConfigPtr);
    PRV_CPSS_DXCH_COS_CHECK_UP_MAC(analyzerVlanTagConfigPtr->vpt);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(analyzerVlanTagConfigPtr->vid);
    if (analyzerVlanTagConfigPtr->cfi > 1)
    {
        return GT_BAD_PARAM;
    }

    /* VLAN tag data vid+cfi+vpt+ethertype */
    regData = (analyzerVlanTagConfigPtr->vid) |
              (analyzerVlanTagConfigPtr->cfi << 12) |
              (analyzerVlanTagConfigPtr->vpt << 13) |
              (analyzerVlanTagConfigPtr->etherType << 16);

    /* Ingress Analyzer VLAN Tag Configuration Register */
    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ingAnalyzerVlanTagConfig;

    rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, regData);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorRxAnalyzerVlanTagConfigGet
*
* DESCRIPTION:
*      Get global Rx mirroring VLAN tag details: EtherType, VID, VPT, CFI
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - device number.
*
* OUTPUTS:
*      analyzerVlanTagConfigPtr - Pointer to analyzer Vlan tag configuration:
*                              EtherType, VPT, CFI and VID
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorRxAnalyzerVlanTagConfigGet
(
    IN GT_U8                                       devNum,
    OUT CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC *analyzerVlanTagConfigPtr
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      regData;        /* register data */
    GT_STATUS   rc;             /* function return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(analyzerVlanTagConfigPtr);

    /* Ingress Analyzer VLAN Tag Configuration Register */
    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ingAnalyzerVlanTagConfig;

    rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &regData);
    if(rc != GT_OK)
        return rc;

    /* VLAN tag data vid+cfi+vpt+ethertype */
    analyzerVlanTagConfigPtr->vid = (GT_U16)(regData & 0xFFF);
    analyzerVlanTagConfigPtr->cfi = (GT_U8)((regData & 0x1000) >> 12);
    analyzerVlanTagConfigPtr->vpt = (GT_U8)((regData & 0xE000) >> 13);
    analyzerVlanTagConfigPtr->etherType = (GT_U16)((regData & 0xFFFF0000) >> 16);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorTxAnalyzerPortSet
*
* DESCRIPTION:
*       Sets a port to be an analyzer port of tx mirrored
*       ports on all system pp's.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* INPUTS:
*       devNum  - the device number
*       analyzerPort - port number of analyzer port
*       analyzerdev - pp Device number of analyzer port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxAnalyzerPortSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   analyzerPort,
    IN  GT_U8   analyzerdev
)
{

    GT_U32      regAddr;     /* hw pp memory address */
    GT_U32      data;        /* data for hw write in pp */
    GT_STATUS   rc;          /* function call return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);


    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.sniffPortsCfg;

    CORE_MIRR_CONVERT_ANALYZER_FOR_CPU_OR_NULL_PORT_MAC(
            analyzerdev, analyzerPort, PRV_CPSS_HW_DEV_NUM_MAC(devNum),
            PRV_CPSS_DXCH_NULL_PORT_NUM_CNS);

    data = ((analyzerPort & 0x3f) << 5) |
            (analyzerdev & 0x1f);

    /* configure the egress analyzer device and port */
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 11, data);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorTxAnalyzerPortGet
*
* DESCRIPTION:
*       Gets the analyzer port of tx mirrored ports on all system pp's.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* INPUTS:
*       devNum  - the device number
*
* OUTPUTS:
*       analyzerPortPtr - (pointer to)port number of analyzer port
*       analyzerDevPtr - (pointer to)pp Device number of analyzer port
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxAnalyzerPortGet
(
    IN  GT_U8   devNum,
    OUT GT_U8   *analyzerPortPtr,
    OUT GT_U8   *analyzerDevPtr
)
{
    GT_U32      regAddr;     /* hw pp memory address */
    GT_U32      data;        /* data for hw write in pp */
    GT_STATUS   rc;          /* function call return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(analyzerPortPtr);
    CPSS_NULL_PTR_CHECK_MAC(analyzerDevPtr);

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.sniffPortsCfg;

    /* get configuration of the egress analyzer device and port */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 11, &data);

    if(rc != GT_OK)
        return rc;


    *analyzerPortPtr = (GT_U8)((data & 0x7E0) >> 5);
    *analyzerDevPtr = (GT_U8)(data & 0x1f);

    return rc;

}

/*******************************************************************************
* cpssDxChMirrorRxAnalyzerPortSet
*
* DESCRIPTION:
*       Sets a specific port to be an analyzer port of Rx mirrored
*       ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* INPUTS:
*
*       devNum  - the device number
*       analyzerPort - port number of analyzer port
*       analyzerdev - pp Device number of analyzer port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorRxAnalyzerPortSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   analyzerPort,
    IN  GT_U8   analyzerdev
)
{

    GT_U32      regAddr;     /* hw pp memory address */
    GT_U32      data;        /* data for hw write in pp */
    GT_STATUS   rc;          /* function call return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);


    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.sniffPortsCfg;

    CORE_MIRR_CONVERT_ANALYZER_FOR_CPU_OR_NULL_PORT_MAC(
            analyzerdev, analyzerPort, PRV_CPSS_HW_DEV_NUM_MAC(devNum),
            PRV_CPSS_DXCH_NULL_PORT_NUM_CNS);

    data = ((analyzerPort & 0x3f) << 5) |
            (analyzerdev & 0x1f);


    /* configure the ingress analyzer device and port */
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 11, 11, data);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorRxAnalyzerPortGet
*
* DESCRIPTION:
*       Gets the analyzer port of Rx mirrored ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* INPUTS:
*
*       devNum  - the device number
*
* OUTPUTS:
*       analyzerPortPtr - (pointer to)port number of analyzer port
*       analyzerDevPtr - (pointer to)pp Device number of analyzer port
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorRxAnalyzerPortGet
(
    IN  GT_U8   devNum,
    OUT GT_U8   *analyzerPortPtr,
    OUT GT_U8   *analyzerDevPtr
)
{
    GT_U32      regAddr;     /* hw pp memory address */
    GT_U32      data;        /* data for hw write in pp */
    GT_STATUS   rc;          /* function call return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(analyzerPortPtr);
    CPSS_NULL_PTR_CHECK_MAC(analyzerDevPtr);

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.sniffPortsCfg;

    /* get configuration of the ingress analyzer device and port */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 11, 11, &data);

    if(rc != GT_OK)
        return rc;


    *analyzerPortPtr = (GT_U8)((data & 0x7E0) >> 5);
    *analyzerDevPtr = (GT_U8)(data & 0x1f);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorTxCascadeMonitorEnable
*
* DESCRIPTION:
*       One global bit that is set to 1 when performing egress mirroring or
*       egress STC of any of the cascading ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - the device number
*       enable    - enable/disable Egress Monitoring on cascading ports.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxCascadeMonitorEnable
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
)
{

    GT_U32      regAddr, regData; /* pp memory address and data for hw access*/
    GT_STATUS   rc;               /* function call return value            */
    GT_U32      bitNum;           /* bit number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    regData = (enable == GT_TRUE) ? 1 : 0;

    if(PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            bufferMng.eqBlkCfgRegs.cscdEgressMonitoringEnableConfReg;
        bitNum = 0;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.trSrcSniff;
        if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            /* Cheetah 2 and above devices */
            bitNum = 29;
        }
        else
        {
            /* Cheetah and Cheetah+ devices */
            bitNum = 28;
        }

    }

    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, bitNum, 1, regData);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorTxCascadeMonitorEnableGet
*
* DESCRIPTION:
*       Get One global bit that indicate performing egress mirroring or
*       egress STC of any of the cascading ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - the device number
*
* OUTPUTS:
*       enablePtr    - enable/disable Egress Monitoring on cascading ports.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxCascadeMonitorEnableGet
(
    IN   GT_U8   devNum,
    OUT  GT_BOOL *enablePtr
)
{

    GT_U32      regAddr, regData; /* pp memory address and data for hw access*/
    GT_STATUS   rc;               /* function call return value            */
    GT_U32      bitNum;           /* bit number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.trSrcSniff;

    if(PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            bufferMng.eqBlkCfgRegs.cscdEgressMonitoringEnableConfReg;
        bitNum = 0;
    }
    else
    {
        if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            /* Cheetah 2 and above devices */
            bitNum = 29;
        }
        else
        {
            /* Cheetah and Cheetah+ devices */
            bitNum = 28;
        }
    }

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, bitNum, 1, &regData);
    if(rc != GT_OK)
        return rc;

    *enablePtr = (regData == 0x1) ? GT_TRUE : GT_FALSE;


    return rc;
}
/*******************************************************************************
* prvCpssDxChMirrorPortIndexRegDataCalculate
*
* DESCRIPTION:
*       Calculate  start bit and  Port Index Register index
*       according to the port.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum         - the device number
*       port           - port number.
*       rxMirrorEnable - GT_TRUE - Calculate  start bit and
*                            Port Index Register index for ingress mirror port.
*                      - GT_FALSE - Calculate  start bit and
*                            Port Index Register index for egress mirror port.
*
* OUTPUTS:
*       startBitPtr  - pointer to start bit.
*       regIndexPtr  - pointer to register index.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvCpssDxChMirrorPortIndexRegDataCalculate
(
    IN  GT_U8   devNum,
    IN  GT_U8   port,
    IN  GT_BOOL rxMirrorEnable,
    OUT GT_U32  *startBitPtr,
    OUT GT_U32  *regIndexPtr
)
{
    if(((0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum)) ||
       (rxMirrorEnable == GT_TRUE)) && (port == CPSS_CPU_PORT_NUM_CNS))
    {
        /* CPU port for xCat device both Rx and Tx mirroring is in the 
           Port Ingress/Egress Mirror Index2 register bits 24:26.
           CPU port for Lion and above devices for Rx mirroring is in the 
           Port Ingress/Egress Mirror Index2 register bits 24:26. 
           CPU port for Lion and above devices for Tx mirroring is in the 
           Port Egress Mirror Index6. And calculation as for other ports. */
        *startBitPtr = 24;
        *regIndexPtr = 2;
    }
    else
    {
        *startBitPtr = (port % 10) * 3;
        *regIndexPtr = port / 10;
    }
}

/*******************************************************************************
* prvCpssDxChMirrorToAnalyzerForwardingModeGet
*
* DESCRIPTION:
*       Get Forwarding mode to Analyzer for egress/ingress mirroring.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum    - device number.
*
* OUTPUTS:
*      modePtr   - pointer to mode of forwarding To Analyzer packets.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChMirrorToAnalyzerForwardingModeGet
(
    IN  GT_U8     devNum,
    OUT CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT   *modePtr
)
{
    GT_U32      regAddr;      /* register address */
    GT_U32      regData;      /* register data */
    GT_STATUS   rc;           /* return status */

    /* getting register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bufferMng.eqBlkCfgRegs.analyzerPortGlobalConfig;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 1, &regData);
    if(rc != GT_OK)
    {
        return rc;
    }

    *modePtr = (regData == 0) ?
        CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_E :
        CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChMirrorRxPortSet
*
* DESCRIPTION:
*       Sets a specific port to be Rx mirrored port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - the device number
*       mirrPort  - port number, CPU port supported.
*       enable    - enable/disable Rx mirror on this port
*                   GT_TRUE - Rx mirroring enabled, packets
*                             received on a mirrPort are
*                             mirrored to Rx analyzer.
*                   GT_FALSE - Rx mirroring disabled.
*       index     - Analyzer destination interface index. (0 - 6)
*                   Supported for xCat and above device.
*                   Used only if forwarding mode to analyzer is Source-based.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, mirrPort.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorRxPortSet
(
    IN  GT_U8    devNum,
    IN  GT_U8    mirrPort,
    IN  GT_BOOL  enable,
    IN  GT_U32   index
)
{
    GT_U32    data;        /* data from Ports VLAN and QoS table entry */
    GT_U32    offset;      /* offset in VLAN and QoS table entry */
    GT_STATUS rc;          /* function call return value */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;  /* local port - support multi-port-groups device */
    CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT mode; /* forwarding mode */
    GT_U32  regIndex; /* the index of Port Ingress Mirror Index register */
    GT_U32  regAddr;  /* pp memory address for hw access*/


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, mirrPort);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, mirrPort);

    mode = CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E;

    if(PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* Get Analyzer forwarding mode */
        rc = prvCpssDxChMirrorToAnalyzerForwardingModeGet(devNum, &mode);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* Source-based-forwarding mode. */
        if(mode == CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_E)
        {
            if(index >  PRV_CPSS_DXCH_MIRROR_ANALYZER_MAX_INDEX_CNS)
            {
                return GT_OUT_OF_RANGE;
            }

            /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
            portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, mirrPort);
            /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
            localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,mirrPort);

            prvCpssDxChMirrorPortIndexRegDataCalculate(devNum, localPort, GT_TRUE,
                                                       &offset, &regIndex);

            /* getting register address */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                bufferMng.eqBlkCfgRegs.portRxMirrorIndex[regIndex];

            /* Set index to Analyzer interface for the port */
            if(enable == GT_TRUE)
            {
                data = index + 1;
            }
            /* No mirroring for the port */
            else
            {
                data = 0;
            }

            /* Set Analyzer destination interface index  */
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, offset,
                                             3, data);

            if(rc != GT_OK)
            {
                return rc;
            }
        }
        offset = 7;
    }
    else
    {
        offset = 23;
    }

    /* For xCat and above hop-by-hop forwarding mode and other DxCh devices */

    data = (enable==GT_TRUE) ? 1 : 0;

    /* configure the Ports VLAN and QoS configuration entry,
       enable MirrorToIngressAnalyzerPort field */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        mirrPort,
                                        0,
                                        offset,
                                        1,
                                        data);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorRxPortGet
*
* DESCRIPTION:
*       Gets status of Rx mirroring (enabled or disabled) of specific port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - the device number
*       mirrPort  - port number, CPU port supported.
*
* OUTPUTS:
*       enablePtr    - (pointer to) Rx mirror mode
*                   GT_TRUE - Rx mirroring enabled, packets
*                             received on a mirrPort are
*                             mirrored to Rx analyzer.
*                   GT_FALSE - Rx mirroring disabled.
*       indexPtr  - (pointer to) Analyzer destination interface index. (0 - 6)
*                   Supported for xCat and above device.
*                   Used only if forwarding mode to analyzer is Source-based.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, mirrPort.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorRxPortGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    mirrPort,
    OUT GT_BOOL  *enablePtr,
    OUT GT_U32   *indexPtr
)
{
    GT_U32    data;        /* HW data */
    GT_U32    offset;
    GT_STATUS rc;          /* function call return value */
    GT_U32  regIndex;   /* the index of Port Ingress Mirror Index register */
    CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT mode; /* forwarding mode */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;  /* local port - support multi-port-groups device */
    GT_U32  regAddr;    /* pp memory address for hw access*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, mirrPort);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    mode = CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E;

    if(PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* Get Analyzer forwarding mode */
        rc = prvCpssDxChMirrorToAnalyzerForwardingModeGet(devNum, &mode);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* Source-based-forwarding mode. */
        if(mode == CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_E)
        {
            /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
            portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, mirrPort);
            /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
            localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,mirrPort);

            prvCpssDxChMirrorPortIndexRegDataCalculate(devNum, localPort, GT_TRUE,
                                                       &offset, &regIndex);

            /* getting register address */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                bufferMng.eqBlkCfgRegs.portRxMirrorIndex[regIndex];

            /* Get Analyzer destination interface index  */
            rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, offset,
                                               3, &data);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* No mirroring for the port */
            if(!data)
            {
                *enablePtr = GT_FALSE;
            }
            /* Get index to Analyzer interface for the port */
            else
            {
                *enablePtr = GT_TRUE;
                *indexPtr = data - 1;
            }

            return GT_OK;
        }
        /* xCat and above hop-by-hop forwarding mode */
        else
        {
            offset = 7;
        }
    }
    else
    {
        offset = 23;
    }

    /* For xCat and above hop-by-hop forwarding mode and other DxCh devices */
    /* Get configuration MirrorToIngressAnalyzerPort field */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        mirrPort,
                                        0,
                                        offset,
                                        1,
                                        &data);
    if(rc != GT_OK)
        return rc;

    *enablePtr = (data != 0) ? GT_TRUE : GT_FALSE;

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorTxPortSet
*
* DESCRIPTION:
*       Enable or disable Tx mirroring per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - the device number
*       mirrPort  - port number.
*       enable    - enable/disable Tx mirror on this port
*                   GT_TRUE - Tx mirroring enabled, packets
*                             transmitted from a mirrPort are
*                             mirrored to Tx analyzer.
*                   GT_FALSE - Tx mirroring disabled.
*       index     - Analyzer destination interface index. (0 - 6)
*                   Supported for xCat and above device.
*                   Used only if forwarding mode to analyzer is Source-based.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or mirrPort.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxPortSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   mirrPort,
    IN  GT_BOOL enable,
    IN  GT_U32  index
)
{
    GT_STATUS   rc;     /* function call return value              */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32  regIndex;   /* the index of Port Egress Mirror Index register */
    CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT mode; /* forwarding mode */
    GT_U8   localPort;/* local port - support multi-port-groups device */
    GT_U32  regAddr1, regAddr2 = 0, regAddr3; /* pp memory address for hw access*/
    GT_U32  startBit1,startBit2 = 0, startBit3; /* bits to start to write data for registers 1 & 2 & 3*/
    GT_U32  regData2 = 0;  /* register2 data */
    GT_U32  stcLimit;   /* Egress STC limit */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, mirrPort);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, mirrPort);
    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,mirrPort);

    mode = CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E;

    if(PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* Get Analyzer forwarding mode */
        rc = prvCpssDxChMirrorToAnalyzerForwardingModeGet(devNum, &mode);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* Source-based-forwarding mode. */
        if(mode == CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_E)
        {
            if(index >  PRV_CPSS_DXCH_MIRROR_ANALYZER_MAX_INDEX_CNS)
            {
                return GT_OUT_OF_RANGE;
            }

            prvCpssDxChMirrorPortIndexRegDataCalculate(devNum, mirrPort, GT_FALSE,
                                                       &startBit2, &regIndex);

            /* getting register address */
            regAddr2 = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                bufferMng.eqBlkCfgRegs.portTxMirrorIndex[regIndex];

            /* Set index to Analyzer interface for the port */
            if(enable == GT_TRUE)
            {
                regData2 = index + 1;
            }
            /* No mirroring for the port */
            else
            {
                regData2 = 0;
            }
        }
    }

    /* configure EgressMirrorToAnalyzerEn field in Port<n> Txq
       Configuration Register */
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        PRV_CPSS_DXCH_PORT_TXQ_REG_MAC(devNum,mirrPort,&regAddr1);
        startBit1 = 20;
    }
    else
    {
        regAddr1 = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.
            statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort.
            egressAnalyzerEnable;

        startBit1 = localPort;

        if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
        {
            startBit1 &= 0xf;
        }
    }

    regAddr3 = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.trSrcSniff;

    startBit3 = mirrPort % 32;

    if(PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        if(mirrPort > 31)
        {
            regAddr3 += 0x64;
        }
    }
    else
    {
        if (mirrPort  == CPSS_CPU_PORT_NUM_CNS)
        {
            /* Determine CPU port bit offset */
            /* Cheetah 2,3.. devices */
            if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
            {
                startBit3 = 28;
            }
            else /* Cheetah and Cheetah+ devices */
            {
                startBit3 = 27;
            }
        }
    }

    /* For CPU port STC is not relevant, therefore for CPU port STC is considered as */
    /* disabled by forcing the limit indication to 0. */
    if (mirrPort  == CPSS_CPU_PORT_NUM_CNS)
    {
        stcLimit = 0;
    }
    else
    {
        rc = cpssDxChStcPortLimitGet(devNum, mirrPort, CPSS_DXCH_STC_EGRESS_E, &stcLimit);
        if (GT_OK != rc)
        {
            return rc;
        }
    }

    if(enable == GT_TRUE)
    {

        /* Prior to configuring any non-cascade port configured for egress mirroring,
           for source based mirroring set Port Egress Mirror Index,
           for hop-by-hop mirroring set the corresponding bit
           in the <PortEgress MonitorEn[28:0]> field of the
           Egress Monitoring Enable Configuration Register.
           The Egress Monitoring Enable Configuration Register be set according
           to the global port number and must be set in all the cores.
        */

        /* If Egress STC is disabled on port enable Egress monitoring on port. */
        if( 0 == stcLimit )
        {
            rc = prvCpssDrvHwPpSetRegField(devNum, regAddr3, startBit3, 1, 1);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        if( regAddr2 != 0 )
        {
            rc = prvCpssDrvHwPpSetRegField(devNum, regAddr2,
                                           startBit2, 3, regData2);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        rc = prvCpssDxChHwPpPortGroupSetRegField(devNum, portGroupId,regAddr1, startBit1, 1, 1);
        if (rc != GT_OK)
        {
            return rc;
        }

    }
    else
    {
        /* The order must be opposite from above. */
        rc = prvCpssDxChHwPpPortGroupSetRegField(devNum, portGroupId,regAddr1, startBit1, 1, 0);
        if (rc != GT_OK)
        {
            return rc;
        }

        if( regAddr2 != 0 )
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr2,
                                                    startBit2, 3, regData2);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        /* If Egress STC is disabled on port disable Egress monitoring on port. */
        if( 0 == stcLimit )
        {
            rc = prvCpssDrvHwPpSetRegField(devNum, regAddr3, startBit3, 1, 0);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChMirrorTxPortGet
*
* DESCRIPTION:
*       Get status (enabled/disabled) of Tx mirroring per port .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - the device number
*       mirrPort  - port number.
*
* OUTPUTS:
*       enablePtr - (pointer to) Tx mirror mode on this port
*                                GT_TRUE - Tx mirroring enabled, packets
*                                          transmitted from a mirrPort are
*                                          mirrored to Tx analyzer.
*                                GT_FALSE - Tx mirroring disabled.
*       indexPtr  - (pointer to) Analyzer destination interface index. (0 - 6)
*                   Supported for xCat and above device.
*                   Used only if forwarding mode to analyzer is Source-based.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or mirrPort.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxPortGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   mirrPort,
    OUT GT_BOOL *enablePtr,
    OUT GT_U32  *indexPtr
)
{

    GT_U32      regAddr;
    GT_STATUS   rc;
    GT_U32      value=0;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32  regIndex;   /* the index of Port Egress Mirror Index register */
    GT_U32  regData;    /* register data */
    GT_U32  startBit , startBit1;       /* bit to start to write data */
    CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT mode; /* forwarding mode */
    GT_U8   localPort; /* local port - support multi-port-groups device */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, mirrPort);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, mirrPort);
    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,mirrPort);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        PRV_CPSS_DXCH_PORT_TXQ_REG_MAC(devNum,mirrPort,&regAddr);
        startBit1 = 20;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.
            statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort.
            egressAnalyzerEnable;
        startBit1 = localPort;

        if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
        {
            startBit1 &= 0xf;
        }
    }

    rc = prvCpssDxChHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, startBit1, 1, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(value != 0)
    {
        *enablePtr = GT_TRUE;
        /* Get Analyzer destination interface index for xCat only, if
           Forwarding mode is Source-based. */
        if(PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* Get Analyzer forwarding mode */
            rc = prvCpssDxChMirrorToAnalyzerForwardingModeGet(devNum, &mode);
            if(rc != GT_OK)
            {
                return rc;
            }
            /* Source-based-forwarding mode. */
            if(mode == CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_E)
            {
                prvCpssDxChMirrorPortIndexRegDataCalculate(devNum, mirrPort, GT_FALSE,
                                                           &startBit, &regIndex);

                /* getting register address */
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    bufferMng.eqBlkCfgRegs.portTxMirrorIndex[regIndex];

                /* Get Analyzer destination interface index  */
                rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, startBit,
                                                 3, &regData);
                if(rc != GT_OK)
                {
                    return rc;
                }

                /* No mirroring for the port */
                if(!regData)
                {
                     return GT_BAD_STATE;
                }
                /* Get index to Analyzer interface for the port */
                else
                {
                    *indexPtr = regData - 1;
                }
            }
        }
    }
    else
    {
        *enablePtr = GT_FALSE;
    }

    return GT_OK;
}


/*******************************************************************************
* cpssDxChMirrRxStatMirroringToAnalyzerRatioSet
*
* DESCRIPTION:
*       Set the statistical mirroring rate in the Rx Analyzer port
*       Set Statistic mirroring to analyzer port statistical ratio configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum   - device number.
*      ratio - Indicates the ratio of egress mirrored to analyzer port packets
*              forwarded to the analyzer port. 1 of every 'ratio' packets are
*              forwarded to the analyzer port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or ratio.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      Examples:
*         ratio 0 -> no Rx mirroring
*         ratio 1 -> all Rx mirrored packets are forwarded out the Rx
*             Analyzer port
*         ratio 10 -> 1 in 10 Rx mirrored packets are forwarded out the Rx
*            Analyzer port
*         The maximum ratio value is 2047.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrRxStatMirroringToAnalyzerRatioSet
(
    IN GT_U8    devNum,
    IN GT_U32   ratio
)
{
    GT_U32      regAddr;
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    if(ratio >= BIT_11)/* 11 bits in hw */
        return GT_BAD_PARAM;

    /* getting register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.trapSniffed;

    /* writing <IngressStatMirroringToAnalyzerPortRatio> field */
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 11, ratio);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrRxStatMirroringToAnalyzerRatioGet
*
* DESCRIPTION:
*       Get the statistical mirroring rate in the Rx Analyzer port
*       Get Statistic mirroring to analyzer port statistical ratio configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum   - device number.
*
* OUTPUTS:
*      ratioPtr - (pointer to) Indicates the ratio of egress mirrored to analyzer port packets
*                 forwarded to the analyzer port. 1 of every 'ratio' packets are
*                 forwarded to the analyzer port.
*
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      Examples:
*         ratio 0 -> no Rx mirroring
*         ratio 1 -> all Rx mirrored packets are forwarded out the Rx
*             Analyzer port
*         ratio 10 -> 1 in 10 Rx mirrored packets are forwarded out the Rx
*            Analyzer port
*         The maximum ratio value is 2047.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrRxStatMirroringToAnalyzerRatioGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *ratioPtr
)
{
    GT_U32      regAddr;
    GT_STATUS   rc;
    GT_U32      value=0;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(ratioPtr);

    /* getting register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.trapSniffed;

    /* writing <IngressStatMirroringToAnalyzerPortRatio> field */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 11, &value);
    if(rc != GT_OK)
        return rc;

    *ratioPtr = value;

    return rc;
}

/*******************************************************************************
* cpssDxChMirrRxStatMirrorToAnalyzerEnable
*
* DESCRIPTION:
*       Enable Ingress Statistical Mirroring to the Ingress Analyzer Port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum    - device number.
*      enable - enable\disable Ingress Statistical Mirroring.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrRxStatMirrorToAnalyzerEnable
(
    IN GT_U8     devNum,
    IN GT_BOOL   enable
)
{
    GT_U32      regAddr;
    GT_U32      regData;
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* getting register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.trapSniffed;


    /* enable/disable IngressStatMirroringToAnalyzerPortEn
       Ingress Statistic Mirroring to Analyzer Port Configuration Register */
    regData = (enable == 1) ? 1 : 0;

    /* writing <IngressStatMirroringToAnalyzerPortEn> field */
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 11, 1, regData);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrRxStatMirrorToAnalyzerEnableGet
*
* DESCRIPTION:
*       Get Ingress Statistical Mirroring to the Ingress Analyzer Port Mode
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum    - device number.
*
* OUTPUTS:
*      enablePtr - (pointer to) Ingress Statistical Mirroring mode.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrRxStatMirrorToAnalyzerEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr
)
{
    GT_U32      regAddr;
    GT_U32      regData=0;
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* getting register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.trapSniffed;

    /* getting <IngressStatMirroringToAnalyzerPortEn> field */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 11, 1, &regData);
    if(rc != GT_OK)
        return rc;

    /* enable/disable IngressStatMirroringToAnalyzerPortEn
       Ingress Statistic Mirroring to Analyzer Port Configuration Register */
    *enablePtr = ((regData == 1) ? GT_TRUE : GT_FALSE);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrRxAnalyzerDpTcSet
*
* DESCRIPTION:
*       The TC/DP assigned to the packet forwarded to the ingress analyzer port due
*       to ingress mirroring to the analyzer port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum       - device number.
*      analyzerDp   - the Drop Precedence to be set
*      analyzerTc   - traffic class on analyzer port (0..7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrRxAnalyzerDpTcSet
(
    IN GT_U8             devNum,
    IN CPSS_DP_LEVEL_ENT analyzerDp,
    IN GT_U8             analyzerTc
)
{
    GT_U32      regData;
    GT_U32      regAddr;
    GT_U32      dp;
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_COS_DP_TO_HW_CHECK_AND_CONVERT_MAC(
        devNum, analyzerDp, dp);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(analyzerTc);

    /* getting register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.sniffQosCfg;

    /* setting data */
    regData = (dp | (analyzerTc << 2));

    /* writing data to register */
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 5, 5, regData);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrRxAnalyzerDpTcGet
*
* DESCRIPTION:
*       Get TC/DP assigned to the packet forwarded to the ingress analyzer port due
*       to ingress mirroring to the analyzer port cofiguration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum          - device number.
*
* OUTPUTS:
*      analyzerDpPtr   - pointer to the Drop Precedence.
*      analyzerTcPtr   - pointer to traffic class on analyzer port
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_BAD_PTR               - wrong pointer.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrRxAnalyzerDpTcGet
(
    IN GT_U8              devNum,
    OUT CPSS_DP_LEVEL_ENT *analyzerDpPtr,
    OUT GT_U8             *analyzerTcPtr
)
{
    GT_U32      regData;
    GT_U32      regAddr;
    GT_U32      dp;
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(analyzerDpPtr);
    CPSS_NULL_PTR_CHECK_MAC(analyzerTcPtr);

    /* getting register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.sniffQosCfg;

    /* reading data from register */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 5, 5, &regData);

    /* setting data */
    dp = (regData & 0x3);

    PRV_CPSS_DXCH_COS_DP_TO_SW_CHECK_AND_CONVERT_MAC(
        devNum, dp, (*analyzerDpPtr));

    *analyzerTcPtr = (GT_U8)((regData >> 2 ) & 0x7);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrTxAnalyzerDpTcSet
*
* DESCRIPTION:
*       The TC/DP assigned to the packet forwarded to the egress analyzer port due
*       to ingress mirroring to the analyzer port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum       - device number.
*      analyzerDp   - the Drop Precedence to be set
*      analyzerTc   - traffic class on analyzer port (0..7)
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrTxAnalyzerDpTcSet
(
    IN GT_U8              devNum,
    IN CPSS_DP_LEVEL_ENT  analyzerDp,
    IN GT_U8              analyzerTc
)
{
    GT_U32      regData;
    GT_U32      regAddr;
    GT_U32      dp;
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_COS_DP_TO_HW_CHECK_AND_CONVERT_MAC(
        devNum, analyzerDp, dp);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(analyzerTc);

    /* setting register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.sniffQosCfg;

    regData = (dp | (analyzerTc << 2));

    /* writing data to register */
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 5, regData);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrTxAnalyzerDpTcGet
*
* DESCRIPTION:
*       Get TC/DP assigned to the packet forwarded to the egress analyzer port due
*       to ingress mirroring to the analyzer port cofiguration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum          - device number.
*
* OUTPUTS:
*      analyzerDpPtr   - pointer to the Drop Precedence.
*      analyzerTcPtr   - pointer to traffic class on analyzer port
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_BAD_PTR               - wrong pointer.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrTxAnalyzerDpTcGet
(
    IN GT_U8              devNum,
    OUT CPSS_DP_LEVEL_ENT *analyzerDpPtr,
    OUT GT_U8             *analyzerTcPtr
)
{
    GT_U32      regData;
    GT_U32      regAddr;
    GT_U32      dp;
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(analyzerDpPtr);
    CPSS_NULL_PTR_CHECK_MAC(analyzerTcPtr);

    /* setting register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.sniffQosCfg;

    /* reading data from register */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 5, &regData);

    /* setting data */
    dp = (regData & 0x3);

    PRV_CPSS_DXCH_COS_DP_TO_SW_CHECK_AND_CONVERT_MAC(
        devNum, dp, (*analyzerDpPtr));

    *analyzerTcPtr = (GT_U8)((regData >> 2 ) & 0x7);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrTxStatMirroringToAnalyzerPortRatioSet
*
* DESCRIPTION:
*       Set the statistical mirroring rate in the Tx Analyzer port
*       Set Statistic mirroring to analyzer port statistical ratio configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum   - device number.
*      ratio - Indicates the ratio of egress mirrored to analyzer port packets
*              forwarded to the analyzer port. 1 of every 'ratio' packets are
*              forwarded to the analyzer port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device or ratio.
*       GT_HW_ERROR   - on writing to HW error.
*
* COMMENTS:
*      Examples:
*         ratio 0 -> no Tx mirroring
*         ratio 1 -> all Tx mirrored packets are forwarded out the Tx
*             Analyzer port
*         ratio 10 -> 1 in 10 Tx mirrored packets are forwarded out the Tx
*            Analyzer port
*         The maximum ratio value is 2047.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrTxStatMirroringToAnalyzerRatioSet
(
    IN GT_U8    devNum,
    IN GT_U32   ratio
)
{
    GT_U32      regAddr;
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    if(ratio > 2047)
        return GT_BAD_PARAM;

    /* getting register address */
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.trStatSniffAndStcReg;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.
            statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort.stcStatisticalTxSniffConfig;
    }

    /* writing <EgressStatMirroringToAnalyzerPortRatio> field */
    rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 5, 11, ratio);

    return rc;
}


/*******************************************************************************
* cpssDxChMirrTxStatMirroringToAnalyzerRatioGet
*
* DESCRIPTION:
*       Get the statistical mirroring rate in the Tx Analyzer port
*       Get Statistic mirroring to analyzer port statistical ratio configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum   - device number.
*
* OUTPUTS:
*      ratioPtr - (pointer to)Indicates the ratio of egress mirrored to analyzer
*              port packets forwarded to the analyzer port. 1 of every 'ratio'
*              packets are forwarded to the analyzer port.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      Examples:
*         ratio 0 -> no Tx mirroring
*         ratio 1 -> all Tx mirrored packets are forwarded out the Tx
*             Analyzer port
*         ratio 10 -> 1 in 10 Tx mirrored packets are forwarded out the Tx
*            Analyzer port
*         The maximum ratio value is 2047.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrTxStatMirroringToAnalyzerRatioGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *ratioPtr
)
{
    GT_U32      regAddr;
    GT_STATUS   rc;
    GT_U32      value=0;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(ratioPtr);

    /* getting register address */
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.trStatSniffAndStcReg;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.
            statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort.stcStatisticalTxSniffConfig;
    }

    /* getting <EgressStatMirroringToAnalyzerPortRatio> field */
    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 5, 11, &value);
    if(rc != GT_OK)
        return rc;

    *ratioPtr = value;

    return rc;
}

/*******************************************************************************
* cpssDxChMirrTxStatMirrorToAnalyzerEnable
*
* DESCRIPTION:
*       Enable Egress Statistical Mirroring to the Egress Analyzer Port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum    - device number.
*      enable - enable\disable Egress Statistical Mirroring.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrTxStatMirrorToAnalyzerEnable
(
    IN GT_U8     devNum,
    IN GT_BOOL   enable
)
{
    GT_U32      regAddr;
    GT_U32      regData;
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* getting register address */
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.trStatSniffAndStcReg;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.
            statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort.stcStatisticalTxSniffConfig;
    }


    regData = (enable == GT_TRUE) ? 1 : 0;

    /* writing <EgressStatMirrorEn> field */
    rc = prvCpssDxChHwPpSetRegField(devNum, regAddr, 4, 1, regData);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrTxStatMirrorToAnalyzerEnableGet
*
* DESCRIPTION:
*       Get mode of Egress Statistical Mirroring to the Egress Analyzer Port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum    - device number
*
* OUTPUTS:
*      enablePtr - (pointer to)  Egress Statistical Mirroring.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrTxStatMirrorToAnalyzerEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr
)
{
    GT_U32      regAddr;
    GT_U32      regData;
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* getting register address */
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.trStatSniffAndStcReg;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.
            statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort.stcStatisticalTxSniffConfig;
    }

    /* writing <EgressStatMirrorEn> field */
    rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 4, 1, &regData);
    if(rc != GT_OK)
        return rc;

    *enablePtr = ((regData != 0) ? GT_TRUE : GT_FALSE);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorToAnalyzerForwardingModeSet
*
* DESCRIPTION:
*       Set Forwarding mode to Analyzer for egress/ingress mirroring.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum    - device number.
*      mode   - mode of forwarding To Analyzer packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, mode.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To change  Forwarding mode, applicaton should
*       disable Rx/Tx mirrorred ports.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorToAnalyzerForwardingModeSet
(
    IN GT_U8     devNum,
    IN CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT   mode
)
{
    GT_U32      regAddr;         /* register address */
    GT_U32      regData;         /* register data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);


    switch(mode)
    {
        case CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E:
            regData = 3;
            break;
        case CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_E:
            regData = 0;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* getting register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bufferMng.eqBlkCfgRegs.analyzerPortGlobalConfig;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 2, regData);

}

/*******************************************************************************
* cpssDxChMirrorToAnalyzerForwardingModeGet
*
* DESCRIPTION:
*       Get Forwarding mode to Analyzer for egress/ingress mirroring.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum    - device number.
*
* OUTPUTS:
*      modePtr   - pointer to mode of forwarding To Analyzer packets.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorToAnalyzerForwardingModeGet
(
    IN  GT_U8     devNum,
    OUT CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT   *modePtr
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    return prvCpssDxChMirrorToAnalyzerForwardingModeGet(devNum, modePtr);
}

/*******************************************************************************
* cpssDxChMirrorAnalyzerInterfaceSet
*
* DESCRIPTION:
*       This function sets analyzer interface.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum         - device number.
*      index          - index of analyzer interface. (0 - 6)
*      interfacePtr   - Pointer to analyzer interface.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, index, interface type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_OUT_OF_RANGE          - on wrong port or device number in interfacePtr.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorAnalyzerInterfaceSet
(
    IN GT_U8     devNum,
    IN GT_U32    index,
    IN CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC   *interfacePtr
)
{
    GT_U32      regAddr;         /* register address */
    GT_U32      regData;         /* register data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if(interfacePtr->interface.type != CPSS_INTERFACE_PORT_E)
    {
        return GT_BAD_PARAM;
    }

    if((interfacePtr->interface.devPort.devNum >= BIT_5) ||
       (interfacePtr->interface.devPort.portNum >= BIT_6))
    {
        return GT_OUT_OF_RANGE;
    }

    if(index >  PRV_CPSS_DXCH_MIRROR_ANALYZER_MAX_INDEX_CNS)
    {
        return GT_BAD_PARAM;
    }

    /* getting register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bufferMng.eqBlkCfgRegs.mirrorInterfaceParameterReg[index];

    regData = interfacePtr->interface.devPort.devNum |
          (interfacePtr->interface.devPort.portNum << 5);

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 2, 11, regData);
}

/*******************************************************************************
* cpssDxChMirrorAnalyzerInterfaceGet
*
* DESCRIPTION:
*       This function gets analyzer interface.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum         - device number.
*      index          - index of analyzer interface. (0 - 6)
*
* OUTPUTS:
*      interfacePtr   - Pointer to analyzer interface.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorAnalyzerInterfaceGet
(
    IN  GT_U8     devNum,
    IN GT_U32     index,
    OUT CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC   *interfacePtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      regData;    /* register data */
    GT_STATUS   rc;         /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(interfacePtr);

    if(index >  PRV_CPSS_DXCH_MIRROR_ANALYZER_MAX_INDEX_CNS)
    {
        return GT_BAD_PARAM;
    }

    /* getting register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bufferMng.eqBlkCfgRegs.mirrorInterfaceParameterReg[index];

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 2, 11, &regData);
    if(rc != GT_OK)
    {
        return rc;
    }

    interfacePtr->interface.type = CPSS_INTERFACE_PORT_E;
    interfacePtr->interface.devPort.devNum = (GT_U8)(regData & 0x1F);
    interfacePtr->interface.devPort.portNum = (GT_U8)((regData >> 5) & 0x3F);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexSet
*
* DESCRIPTION:
*       This function sets analyzer interface index, used for ingress
*       mirroring from all engines except
*       port-mirroring source-based-forwarding mode.
*       (Port-Based hop-by-hop mode, Policy-Based, VLAN-Based,
*        FDB-Based, Router-Based).
*       If a packet is mirrored by both the port-based ingress mirroring,
*       and one of the other ingress mirroring, the selected analyzer is
*       the one with the higher index.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum    - device number.
*      enable    - global enable/disable mirroring for
*                  Port-Based hop-by-hop mode, Policy-Based,
*                  VLAN-Based, FDB-Based, Router-Based.
*                  - GT_TRUE - enable mirroring.
*                  - GT_FALSE - No mirroring.
*      index     - Analyzer destination interface index. (0 - 6)
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexSet
(
    IN GT_U8     devNum,
    IN GT_BOOL   enable,
    IN GT_U32    index
)
{
    GT_U32      regAddr;     /* register address */
    GT_U32      regData;     /* register data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if(index >  PRV_CPSS_DXCH_MIRROR_ANALYZER_MAX_INDEX_CNS)
    {
        return GT_OUT_OF_RANGE;
    }

    /* getting register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bufferMng.eqBlkCfgRegs.analyzerPortGlobalConfig;

    /* 0 is used for no mirroring */
    regData = (enable == GT_TRUE) ? (index + 1) : 0;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 2, 3, regData);
}

/*******************************************************************************
* cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexGet
*
* DESCRIPTION:
*       This function gets analyzer interface index, used for ingress mirroring
*       from all engines except port-mirroring source-based-forwarding mode.
*       (Port-Based hop-by-hop mode, Policy-Based, VLAN-Based,
*        FDB-Based, Router-Based).
*       If a packet is mirrored by both the port-based ingress mirroring,
*       and one of the other ingress mirroring, the selected analyzer is
*       the one with the higher index.
*
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum         - device number.
*
* OUTPUTS:
*      enablePtr    - Pointer to global enable/disable mirroring for
*                     Port-Based hop-by-hop mode, Policy-Based,
*                     VLAN-Based, FDB-Based, Router-Based.
*                     - GT_TRUE - enable mirroring.
*                     - GT_FALSE - No mirroring.
*      indexPtr     - pointer to analyzer destination interface index.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr,
    OUT GT_U32    *indexPtr
)
{
    GT_U32      regAddr;       /* register address */
    GT_U32      regData;       /* register data */
    GT_STATUS   rc;            /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(indexPtr);

    /* getting register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bufferMng.eqBlkCfgRegs.analyzerPortGlobalConfig;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 2, 3, &regData);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* No mirroring */
    if(regData == 0)
    {
        *enablePtr = GT_FALSE;
    }
    /* mirroring is enabled */
    else
    {
        *enablePtr = GT_TRUE;
        *indexPtr = regData - 1;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet
*
* DESCRIPTION:
*       This function sets analyzer interface index, used for egress
*       mirroring for Port-Based hop-by-hop mode.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum    - device number.
*      enable    - global enable/disable mirroring for
*                  Port-Based hop-by-hop mode.
*                  - GT_TRUE - enable mirroring.
*                  - GT_FALSE - No mirroring.
*      index     - Analyzer destination interface index. (0 - 6)
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet
(
    IN GT_U8     devNum,
    IN GT_BOOL   enable,
    IN GT_U32    index
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      regData;    /* register data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if(index >  PRV_CPSS_DXCH_MIRROR_ANALYZER_MAX_INDEX_CNS)
    {
        return GT_OUT_OF_RANGE;
    }

    /* getting register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bufferMng.eqBlkCfgRegs.analyzerPortGlobalConfig;

    /* 0 is used for no mirroring */
    regData = (enable == GT_TRUE) ? (index + 1) : 0;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 5, 3, regData);
}

/*******************************************************************************
* cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexGet
*
* DESCRIPTION:
*       This function gets analyzer interface index, used for egress
*       mirroring for Port-Based hop-by-hop mode.
*
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum         - device number.
*
* OUTPUTS:
*      enablePtr    - Pointer to global enable/disable mirroring for
*                     Port-Based hop-by-hop mode.
*                     - GT_TRUE - enable mirroring.
*                     - GT_FALSE - No mirroring.
*      indexPtr     - pointer to analyzer destination interface index.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr,
    OUT GT_U32    *indexPtr
)
{
    GT_U32      regAddr;      /* register address */
    GT_U32      regData;      /* register data */
    GT_STATUS   rc;           /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(indexPtr);

    /* getting register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bufferMng.eqBlkCfgRegs.analyzerPortGlobalConfig;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 5, 3, &regData);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* No mirroring */
    if(regData == 0)
    {
        *enablePtr = GT_FALSE;
    }
    /* mirroring is enabled */
    else
    {
        *enablePtr = GT_TRUE;
        *indexPtr = regData - 1;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChMirrorAnalyzerMirrorOnDropEnableSet
*
* DESCRIPTION:
*       Enable / Disable mirroring of dropped packets.
*
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum - device number.
*      index  - Analyzer destination interface index. (0 - 6)
*      enable - GT_TRUE  - dropped packets are mirrored to analyzer interface.
*               GT_FALSE - dropped packets are not mirrored to analyzer interface.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorAnalyzerMirrorOnDropEnableSet
(
    IN GT_U8     devNum,
    IN GT_U32    index,
    IN GT_BOOL   enable
)
{
    GT_U32      regAddr;         /* register address */
    GT_U32      regData;         /* register data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if(index >  PRV_CPSS_DXCH_MIRROR_ANALYZER_MAX_INDEX_CNS)
    {
        return GT_BAD_PARAM;
    }

    regData = (enable == GT_TRUE) ? 1 : 0;

    /* getting register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bufferMng.eqBlkCfgRegs.mirrorInterfaceParameterReg[index];

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 20, 1, regData);

}

/*******************************************************************************
* cpssDxChMirrorAnalyzerMirrorOnDropEnableGet
*
* DESCRIPTION:
*       Get mirroring status of dropped packets.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum - device number.
*      index  - Analyzer destination interface index. (0 - 6)
*
* OUTPUTS:
*      enablePtr - Pointer to mirroring status of dropped packets.
*                  - GT_TRUE  - dropped packets are mirrored to
*                               analyzer interface.
*                  - GT_FALSE - dropped packets are not mirrored to
*                               analyzer interface.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorAnalyzerMirrorOnDropEnableGet
(
    IN  GT_U8     devNum,
    IN GT_U32     index,
    OUT GT_BOOL   *enablePtr
)
{
    GT_U32      regAddr;      /* register address */
    GT_U32      regData;      /* register data */
    GT_STATUS   rc;           /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(index >  PRV_CPSS_DXCH_MIRROR_ANALYZER_MAX_INDEX_CNS)
    {
        return GT_BAD_PARAM;
    }

    /* getting register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bufferMng.eqBlkCfgRegs.mirrorInterfaceParameterReg[index];

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 20, 1, &regData);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (regData == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChMirrorAnalyzerVlanTagRemoveEnableSet
*
* DESCRIPTION:
*       Enable/Disable VLAN tag removal of mirrored traffic.
*       When VLAN tag removal is enabled for a specific analyzer port, all
*       packets that are mirrored to this port are sent without any VLAN tags.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       enable      - GT_TRUE  - VLAN tag is removed from mirrored traffic.
*                     GT_FALSE - VLAN tag isn't removed from mirrored traffic.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorAnalyzerVlanTagRemoveEnableSet
(
    IN GT_U8     devNum,
    IN GT_U8     portNum,
    IN GT_BOOL   enable
)
{
    GT_U32  regAddr;            /* register address     */
    GT_U32  data;               /* reg subfield data    */
    GT_U32  portGroupId;        /* the port group Id - support multi-port-groups device */
    GT_U32  fieldOffset;        /* register field offset */
    GT_U8   localPort;          /* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    fieldOffset = OFFSET_TO_BIT_MAC(localPort);

    data = (enable == GT_TRUE) ? 0 : 1;

    /* getting register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
        mirrorToAnalyzerHeaderConfReg[OFFSET_TO_WORD_MAC(localPort)];

    /* Enable/Disable VLAN tag removal of mirrored traffic. */
    return  prvCpssDxChHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                fieldOffset, 1, data);
}

/*******************************************************************************
* cpssDxChMirrorAnalyzerVlanTagRemoveEnableGet
*
* DESCRIPTION:
*       Get status of enabling/disabling VLAN tag removal of mirrored traffic.
*       When VLAN tag removal is enabled for a specific analyzer port, all
*       packets that are mirrored to this port are sent without any VLAN tags.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*
* OUTPUTS:
*       enablePtr  - pointer to status of VLAN tag removal of mirrored traffic.
*                  - GT_TRUE  - VLAN tag is removed from mirrored traffic.
*                    GT_FALSE - VLAN tag isn't removed from mirrored traffic.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorAnalyzerVlanTagRemoveEnableGet
(
    IN GT_U8      devNum,
    IN GT_U8      portNum,
    OUT GT_BOOL   *enablePtr
)
{
    GT_U32      regAddr;      /* register address */
    GT_U32      regData;      /* register data */
    GT_U32      portGroupId;  /* the port group Id - support multi-port-groups device */
    GT_U32      fieldOffset;  /* register field offset */
    GT_STATUS   rc;           /* return code */
    GT_U8       localPort;    /* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);
    fieldOffset = OFFSET_TO_BIT_MAC(localPort);

        /* getting register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
        mirrorToAnalyzerHeaderConfReg[OFFSET_TO_WORD_MAC(localPort)];

    /* Get status of enabling/disabling VLAN tag removal of mirrored traffic. */
    rc = prvCpssDxChHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                                fieldOffset, 1, &regData);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (regData == 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;

}


