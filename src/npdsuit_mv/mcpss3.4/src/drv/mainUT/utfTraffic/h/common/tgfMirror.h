/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfMirrorGen.h
*
* DESCRIPTION:
*       Generic APIs Mirroring.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __tgfMirrorGenh
#define __tgfMirrorGenh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/mirror/cpssDxChMirror.h>
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    #include <cpss/exMxPm/exMxPmGen/monitor/cpssExMxPmMirror.h>
#endif /* EXMXPM_FAMILY */

/*
 * typedef: PRV_TGF_MIRROR_ANALYZER_INTERFACE_STC
 *
 * Description: used to describe Analyzer interface.
 *
 * Fields:
 *      interface - Analyzer interface. 
 *      Only CPSS_INTERFACE_PORT_E  interface type is supported.
 *
 */
typedef struct
{
    CPSS_INTERFACE_INFO_STC  interface;
    
}PRV_TGF_MIRROR_ANALYZER_INTERFACE_STC;

/*
 * Typedef: enum PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT
 *
 * Description:
 *      This enum defines mode of forwarding To Analyzer packets.
 * Fields:
 *     PRV_TGF_MIRROR_ANALYZER_FORWARDING_HOP_BY_HOP_E - DSA tag holds
 *     the Device/Port trigerring parameters. The forwarding decision of the 
 *     To Analyzer packet is performed at each hop according to the local 
 *     configuration of the ingress/egress analyzer.  
 *     PRV_TGF_MIRROR_ANALYZER_FORWARDING_SOURCE_BASED_E - DSA tag holds
 *     the mirroring Device/Port destination. The destination analyzer is 
 *     determined at the triggered mirroring device  
 *  Comments:
 *
 */
typedef enum
{
    PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E,
    PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_E
} PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT;


/*******************************************************************************
* prvTgfMirrorTxGlobalAnalyzerInterfaceIndexSet
*
* DESCRIPTION:
*       This function sets analyzer interface index, used for egress
*       mirroring for Port-Based hop-by-hop mode.
*
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
GT_STATUS prvTgfMirrorTxGlobalAnalyzerInterfaceIndexSet
(
    IN GT_BOOL   enable,
    IN GT_U32    index
);

/*******************************************************************************
* prvTgfMirrorTxGlobalAnalyzerInterfaceIndexGet
*
* DESCRIPTION:
*       This function gets analyzer interface index, used for egress
*       mirroring for Port-Based hop-by-hop mode.
*
*
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
GT_STATUS prvTgfMirrorTxGlobalAnalyzerInterfaceIndexGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr,
    OUT GT_U32    *indexPtr
);

/*******************************************************************************
* prvTgfMirrorRxGlobalAnalyzerInterfaceIndexSet
*
* DESCRIPTION:
*       This function sets analyzer interface index, used for ingress
*       mirroring for Port-Based hop-by-hop mode.
*
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
GT_STATUS prvTgfMirrorRxGlobalAnalyzerInterfaceIndexSet
(
    IN GT_BOOL   enable,
    IN GT_U32    index
);

/*******************************************************************************
* prvTgfMirrorRxGlobalAnalyzerInterfaceIndexGet
*
* DESCRIPTION:
*       This function gets analyzer interface index, used for ingress
*       mirroring for Port-Based hop-by-hop mode.
*
*
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
GT_STATUS prvTgfMirrorRxGlobalAnalyzerInterfaceIndexGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr,
    OUT GT_U32    *indexPtr
);

/*******************************************************************************
* prvTgfMirrorAnalyzerInterfaceSet
*
* DESCRIPTION:
*       This function sets analyzer interface.
*
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
*       GT_OUT_OF_RANGE          - on wrong port or device number in interfacePtr.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfMirrorAnalyzerInterfaceSet
(
    IN GT_U32    index,
    IN PRV_TGF_MIRROR_ANALYZER_INTERFACE_STC   *interfacePtr
);

/*******************************************************************************
* prvTgfMirrorAnalyzerInterfaceGet
*
* DESCRIPTION:
*       This function gets analyzer interface.
*
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
GT_STATUS prvTgfMirrorAnalyzerInterfaceGet
(
    IN  GT_U8     devNum,
    IN GT_U32     index,
    OUT PRV_TGF_MIRROR_ANALYZER_INTERFACE_STC   *interfacePtr
);

/*******************************************************************************
* prvTgfMirrorToAnalyzerForwardingModeSet
*
* DESCRIPTION:
*       Set Forwarding mode to Analyzer for egress/ingress mirroring.
*
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
GT_STATUS prvTgfMirrorToAnalyzerForwardingModeSet
(
    IN PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT   mode
);

/*******************************************************************************
* prvTgfMirrorToAnalyzerForwardingModeGet
*
* DESCRIPTION:
*       Get Forwarding mode to Analyzer for egress/ingress mirroring.
*
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
GT_STATUS prvTgfMirrorToAnalyzerForwardingModeGet
(
    IN  GT_U8     devNum,
    OUT PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT   *modePtr
);

/*******************************************************************************
* prvTgfMirrorRxPortSet
*
* DESCRIPTION:
*       Sets a specific port to be Rx mirrored port.
*
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
*                   Supported for DxChXcat and above device.
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
GT_STATUS prvTgfMirrorRxPortSet
(
    IN  GT_U8    mirrPort,
    IN  GT_BOOL  enable,
    IN  GT_U32   index
);

/*******************************************************************************
* prvTgfMirrorRxPortGet
*
* DESCRIPTION:
*       Gets status of Rx mirroring (enabled or disabled) of specific port
*
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
*                   Supported for DxChXcat and above device.
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
GT_STATUS prvTgfMirrorRxPortGet
(
    IN  GT_U8    mirrPort,
    OUT GT_BOOL  *enablePtr,
    OUT GT_U32   *indexPtr
);

/*******************************************************************************
* prvTgfMirrorTxPortSet
*
* DESCRIPTION:
*       Sets a specific port to be Tx mirrored port.
*
*
* INPUTS:
*       devNum       - the device number
*       mirrPort  - port number, CPU port supported.
*       enable    - enable/disable Tx mirror on this port
*                   GT_TRUE - Tx mirroring enabled, packets
*                             transmitted from a mirrPort are
*                             mirrored to Tx analyzer.
*                   GT_FALSE - Tx mirroring disabled.
*       index     - Analyzer destination interface index. (0 - 6)
*                   Supported for DxChXcat and above device.
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
GT_STATUS prvTgfMirrorTxPortSet
(
    IN  GT_U8    mirrPort,
    IN  GT_BOOL  enable,
    IN  GT_U32   index
);

/*******************************************************************************
* prvTgfMirrorTxPortGet
*
* DESCRIPTION:
*       Get status (enabled/disabled) of Tx mirroring per port .
*
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
*                   Supported for DxChXcat and above device.
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
GT_STATUS prvTgfMirrorTxPortGet
(
    IN  GT_U8    mirrPort,
    OUT GT_BOOL  *enablePtr,
    OUT GT_U32   *indexPtr
);


/*******************************************************************************
* prvTgfMirrorRxAnalyzerPortSet
*
* DESCRIPTION:
*       Sets a specific port to be an analyzer port of Rx mirrored
*       ports
*
* INPUTS:
*       devNum       - the device number
*       analyzerPort - port number of analyzer port
*       analyzerdev  - pp Device number of analyzer port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong device number
*       GT_HW_ERROR  - on writing to HW error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvTgfMirrorRxAnalyzerPortSet
(
    IN  GT_U8                         analyzerPort,
    IN  GT_U8                         analyzerdev
);

/*******************************************************************************
* prvTgfMirrorRxAnalyzerPortGet
*
* DESCRIPTION:
*       Gets the analyzer port of Rx mirrored ports
*
* INPUTS:
*       devNum - the device number
*
* OUTPUTS:
*       analyzerPortPtr - (pointer to) port number of analyzer port
*       analyzerDevPtr  - (pointer to) pp Device number of analyzer port
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong device number
*       GT_HW_ERROR  - on writing to HW error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvTgfMirrorRxAnalyzerPortGet
(
    IN  GT_U8                         devNum,
    OUT GT_U8                        *analyzerPortPtr,
    OUT GT_U8                        *analyzerDevPtr
);

/*******************************************************************************
* prvTgfMirrorAnalyzerVlanTagRemoveEnableSet
*
* DESCRIPTION:
*       Enable/Disable VLAN tag removal of mirrored traffic.
*       When VLAN tag removal is enabled for a specific analyzer port, all
*       packets that are mirrored to this port are sent without any VLAN tags.
*
* APPLICABLE DEVICES:  Lion and above
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
GT_STATUS prvTgfMirrorAnalyzerVlanTagRemoveEnableSet
(
    IN GT_U8     portNum,
    IN GT_BOOL   enable
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __tgfMirrorGenh */

