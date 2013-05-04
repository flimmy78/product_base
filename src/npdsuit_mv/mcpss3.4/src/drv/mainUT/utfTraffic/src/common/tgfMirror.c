/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfMirrorGen.c
*
* DESCRIPTION:
*       Generic API for Mirror APIs.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

#include <utf/private/prvUtfHelpers.h>
#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

#include <trafficEngine/tgfTrafficEngine.h>
#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <common/tgfCommon.h>
#include <common/tgfMirror.h>


/******************************************************************************\
 *                       CPSS generic API section                             *
\******************************************************************************/

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
)
{

#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);
    
    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet(devNum, enable, index);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet FAILED, rc = [%d]", rc);
    
            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(enable);
    TGF_PARAM_NOT_USED(index);

    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

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
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexGet(devNum, enablePtr, indexPtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
     /* avoid warnings */
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(enablePtr);
    TGF_PARAM_NOT_USED(indexPtr);

    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

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
)
{

#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);
    
    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexSet(devNum, enable, index);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexSet FAILED, rc = [%d]", rc);
    
            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(enable);
    TGF_PARAM_NOT_USED(index);

    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

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
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexGet(devNum, enablePtr, indexPtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
     /* avoid warnings */
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(enablePtr);
    TGF_PARAM_NOT_USED(indexPtr);

    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

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
)
{

#ifdef CHX_FAMILY
    CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC cpssInterface;
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    cpssInterface.interface.type = interfacePtr->interface.type;
    if( CPSS_INTERFACE_PORT_E == interfacePtr->interface.type )
    {
        rc = prvUtfHwFromSwDeviceNumberGet(interfacePtr->interface.devPort.devNum,
                                           &(cpssInterface.interface.devPort.devNum));
        if(GT_OK != rc)
            return rc;
    }
    cpssInterface.interface.devPort.portNum = interfacePtr->interface.devPort.portNum; 

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChMirrorAnalyzerInterfaceSet(devNum, index, &cpssInterface);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChMirrorAnalyzerInterfaceSet FAILED, rc = [%d]", rc);
    
            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(index);
    TGF_PARAM_NOT_USED(interfacePtr);

    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

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
)
{
#ifdef CHX_FAMILY
    CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC cpssInterface;
    GT_STATUS   rc;

    /* call device specific API */
    rc = cpssDxChMirrorAnalyzerInterfaceGet(devNum, index, &cpssInterface);
    if(rc != GT_OK)
    {
        return rc;
    }

    interfacePtr->interface.type = cpssInterface.interface.type;
    if( CPSS_INTERFACE_PORT_E == cpssInterface.interface.type )
    {
        rc = prvUtfSwFromHwDeviceNumberGet(cpssInterface.interface.devPort.devNum,
                                           &(interfacePtr->interface.devPort.devNum));
        if(GT_OK != rc)
            return rc;
    }
    interfacePtr->interface.devPort.portNum = cpssInterface.interface.devPort.portNum; 

    return GT_OK;

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
     /* avoid warnings */
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(index);
    TGF_PARAM_NOT_USED(interfacePtr);

    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

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
)
{

#ifdef CHX_FAMILY
    CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT   cpssMode;
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    switch(mode)
    {
        case PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E:
            cpssMode = CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E;
            break;
        case PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_E:
            cpssMode = CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_E;
            break;
        default:
            return GT_BAD_PARAM;
    }
    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChMirrorToAnalyzerForwardingModeSet(devNum, cpssMode);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChMirrorToAnalyzerForwardingModeSet FAILED, rc = [%d]", rc);
    
            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(mode);

    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

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
)
{
#ifdef CHX_FAMILY
    CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT   mode;
    GT_STATUS rc;
    /* call device specific API */
    rc =  cpssDxChMirrorToAnalyzerForwardingModeGet(devNum, &mode);
    if(rc != GT_OK)
    {
        return rc;
    }

    *modePtr = (mode == CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E) ?
        PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E :   
        PRV_TGF_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_E ;

    return GT_OK;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
     /* avoid warnings */
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(modePtr);

    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

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
)
{
    GT_U32  portIter = 0;


    /* find port index */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        if (prvTgfPortsArray[portIter] == mirrPort)
        {
            break;
        }
    }

    /* check if port is valid */
    if (prvTgfPortsArray[portIter] != mirrPort)
    {
        PRV_UTF_LOG1_MAC("[TGF]: Port [%d] is not valid", mirrPort);

        return GT_BAD_PARAM;
    }

#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChMirrorRxPortSet(prvTgfDevsArray[portIter], mirrPort, enable, index);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    TGF_PARAM_NOT_USED(index);
    /* call device specific API */
    return cpssExMxPmMirrorPortEnableSet(prvTgfDevsArray[portIter], mirrPort, 
                                         CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E, enable);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

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
)
{
    GT_U32  portIter = 0;
    

    /* find port index */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        if (prvTgfPortsArray[portIter] == mirrPort)
        {
            break;
        }
    }

    /* check if port is valid */
    if (prvTgfPortsArray[portIter] != mirrPort)
    {
        PRV_UTF_LOG1_MAC("[TGF]: Port [%d] is not valid", mirrPort);

        return GT_BAD_PARAM;
    }

#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChMirrorRxPortGet(prvTgfDevsArray[portIter], mirrPort, 
                                   enablePtr, indexPtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    TGF_PARAM_NOT_USED(indexPtr);
    /* call device specific API */
    return cpssExMxPmMirrorPortEnableGet(prvTgfDevsArray[portIter], mirrPort,
                                         CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E, enablePtr);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

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
)
{
    GT_U32  portIter = 0;


    /* find port index */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        if (prvTgfPortsArray[portIter] == mirrPort)
        {
            break;
        }
    }

    /* check if port is valid */
    if (prvTgfPortsArray[portIter] != mirrPort)
    {
        PRV_UTF_LOG1_MAC("[TGF]: Port [%d] is not valid", mirrPort);

        return GT_BAD_PARAM;
    }

#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChMirrorTxPortSet(prvTgfDevsArray[portIter], mirrPort, enable, index);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    TGF_PARAM_NOT_USED(index);
    /* call device specific API */
    return cpssExMxPmMirrorPortEnableSet(prvTgfDevsArray[portIter], mirrPort, 
                                         CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E, enable);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

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
)
{
    GT_U32  portIter = 0;
    

    /* find port index */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        if (prvTgfPortsArray[portIter] == mirrPort)
        {
            break;
        }
    }

    /* check if port is valid */
    if (prvTgfPortsArray[portIter] != mirrPort)
    {
        PRV_UTF_LOG1_MAC("[TGF]: Port [%d] is not valid", mirrPort);

        return GT_BAD_PARAM;
    }

#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChMirrorTxPortGet(prvTgfDevsArray[portIter], mirrPort, 
                                   enablePtr, indexPtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    TGF_PARAM_NOT_USED(indexPtr);
    /* call device specific API */
    return cpssExMxPmMirrorPortEnableGet(prvTgfDevsArray[portIter], mirrPort,
                                         CPSS_EXMXPM_MIRROR_TYPE_EGRESS_E, enablePtr);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}
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
)
{
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;
#ifdef CHX_FAMILY
    CPSS_PP_FAMILY_TYPE_ENT                     devFamily;
    CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC     analyzerInf;
    GT_U8                                       analyzerHwDev;
#endif /*CHX_FAMILY*/
#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_MIRROR_TYPE_ENT   mirrorType     = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;
    GT_U32                        pclAnalyzerNum = 0;
    CPSS_EXMXPM_OUTLIF_INFO_STC   interfaceInfo  = {0};
#endif /* EXMXPM_FAMILY */

#ifdef CHX_FAMILY
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        rc = prvUtfDeviceFamilyGet(devNum, &devFamily);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvUtfDeviceFamilyGet FAILED, rc = [%d]", rc);
            return rc;
        }

        /* call device specific API */
        if (devFamily < CPSS_PP_FAMILY_DXCH_XCAT_E)
        {
            rc = prvUtfHwFromSwDeviceNumberGet(analyzerdev, &analyzerHwDev);
            if(GT_OK != rc)
                return rc;
            rc = cpssDxChMirrorRxAnalyzerPortSet(devNum, analyzerPort, analyzerHwDev);
        }
        else
        {
            analyzerInf.interface.type = CPSS_INTERFACE_PORT_E;
            rc = prvUtfHwFromSwDeviceNumberGet(analyzerdev,
                                               &(analyzerInf.interface.devPort.devNum));
            if(GT_OK != rc)
                return rc;
            analyzerInf.interface.devPort.portNum = analyzerPort;

            /* use interface 0 for Rx analyzer by default */
            rc = cpssDxChMirrorAnalyzerInterfaceSet(devNum, 0, &analyzerInf);
        }

        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChMirrorRxAnalyzerPortSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* set mirror type */
    mirrorType     = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;
    CPSS_COVERITY_NON_ISSUE_BOOKMARK
    /* coverity[dead_error_begin] */
    pclAnalyzerNum = (CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E == mirrorType) ? 1 : 0;

    /* set interface info */
    interfaceInfo.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;

    interfaceInfo.interfaceInfo.type            = CPSS_INTERFACE_PORT_E;
    interfaceInfo.interfaceInfo.devPort.devNum  = analyzerdev;
    interfaceInfo.interfaceInfo.devPort.portNum = analyzerPort;

    /* prepare device iterator */
    rc = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceReset FAILED, rc = [%d]", rc);

        return rc;
    }

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssExMxPmMirrorDestInterfaceSet(devNum, mirrorType, pclAnalyzerNum, &interfaceInfo);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmMirrorDestInterfaceSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

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
)
{
#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_MIRROR_TYPE_ENT   mirrorType     = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;
    GT_U32                        pclAnalyzerNum = 0;
    CPSS_EXMXPM_OUTLIF_INFO_STC   interfaceInfo  = {0};
    GT_STATUS                     rc             = GT_OK;
#endif /* EXMXPM_FAMILY */

#ifdef CHX_FAMILY
    CPSS_PP_FAMILY_TYPE_ENT                     devFamily;
    CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC     analyzerInf;
    GT_STATUS                                   rc;
#endif /*CHX_FAMILY*/

#ifdef CHX_FAMILY
    rc = prvUtfDeviceFamilyGet(devNum, &devFamily);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfDeviceFamilyGet FAILED, rc = [%d]", rc);
        return rc;
    }

    /* call device specific API */
    if (devFamily < CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        rc = cpssDxChMirrorRxAnalyzerPortGet(devNum, analyzerPortPtr, analyzerDevPtr);
    }
    else
    {
        /* use interface 0 for Rx analyzer by default */
        rc = cpssDxChMirrorAnalyzerInterfaceGet(devNum, 0, &analyzerInf);
        if(GT_OK != rc)
            return rc;
        if( CPSS_INTERFACE_PORT_E == analyzerInf.interface.type )
        {
            rc = prvUtfSwFromHwDeviceNumberGet(analyzerInf.interface.devPort.devNum,
                                               analyzerDevPtr);
            if(GT_OK != rc)
                return rc;
        }
        *analyzerPortPtr = analyzerInf.interface.devPort.portNum;

    }
    return rc;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* set mirror type */
    mirrorType     = CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E;
    CPSS_COVERITY_NON_ISSUE_BOOKMARK
    /* coverity[dead_error_begin] */
    pclAnalyzerNum = (CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E == mirrorType) ? 1 : 0;

    /* call device specific API */
    rc = cpssExMxPmMirrorDestInterfaceGet(devNum, mirrorType, pclAnalyzerNum, &interfaceInfo);
    if (GT_OK == rc)
    {
        if ((CPSS_EXMXPM_OUTLIF_TYPE_LL_E == interfaceInfo.outlifType) &&
            (CPSS_INTERFACE_PORT_E == interfaceInfo.interfaceInfo.type))
        {
            *analyzerDevPtr  = interfaceInfo.interfaceInfo.devPort.devNum;
            *analyzerPortPtr = interfaceInfo.interfaceInfo.devPort.portNum;
        }
        else
        {
            rc = GT_FAIL;
        }
    }

    return rc;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

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
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);
    
    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChMirrorAnalyzerVlanTagRemoveEnableSet(devNum, portNum, enable);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChMirrorAnalyzerVlanTagRemoveEnableSet FAILED, rc = [%d]", rc);
    
            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(enable);
    TGF_PARAM_NOT_USED(portNum);

    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */

}

