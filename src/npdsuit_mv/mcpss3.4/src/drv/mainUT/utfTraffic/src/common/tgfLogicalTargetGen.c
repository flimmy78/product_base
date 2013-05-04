/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfLogicalTargetGen.c
*
* DESCRIPTION:
*       Generic API implementation for logical target mapping
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
#include <common/tgfLogicalTargetGen.h>

#ifdef CHX_FAMILY
/*******************************************************************************
* prvTgfConvertGenericToDxChLogicalTargetEntry
*
* DESCRIPTION:
*       Convert generic  to device specific logical target entry
*
* INPUTS:
*       genTargetLogicalEntryPtr - (pointer to) generic logical target entry
*
* OUTPUTS:
*       dxChEntryPtr - (pointer to) DxCh logical target entry
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChLogicalTargetEntry
(
    IN  PRV_TGF_OUTLIF_INFO_STC         *genTargetLogicalEntryPtr,
    OUT CPSS_DXCH_OUTPUT_INTERFACE_STC  *dxChTargetLogicalEntryPtr
)
{
    GT_STATUS rc;

    cpssOsMemSet(dxChTargetLogicalEntryPtr, 0, sizeof(CPSS_DXCH_OUTPUT_INTERFACE_STC));

    switch (genTargetLogicalEntryPtr->outlifType)
    {
    case PRV_TGF_OUTLIF_TYPE_TUNNEL_E:
        dxChTargetLogicalEntryPtr->isTunnelStart = GT_TRUE;
        break;
    case PRV_TGF_OUTLIF_TYPE_LL_E:
    case PRV_TGF_OUTLIF_TYPE_DIT_E:
        dxChTargetLogicalEntryPtr->isTunnelStart = GT_FALSE;
        break;
    default:
        return GT_BAD_PARAM;
    }
    if (dxChTargetLogicalEntryPtr->isTunnelStart == GT_TRUE)
    {
        switch(genTargetLogicalEntryPtr->outlifPointer.tunnelStartPtr.passengerPacketType)
        {
        case PRV_TGF_TS_PASSENGER_PACKET_TYPE_ETHERNET_E:
            dxChTargetLogicalEntryPtr->tunnelStartInfo.passengerPacketType =
                CPSS_DXCH_TUNNEL_PASSENGER_ETHERNET_E;
            break;
        case PRV_TGF_TS_PASSENGER_PACKET_TYPE_OTHER_E:
            dxChTargetLogicalEntryPtr->tunnelStartInfo.passengerPacketType =
                CPSS_DXCH_TUNNEL_PASSENGER_OTHER_E;
            break;
        default:
            return GT_BAD_PARAM;
        }
        dxChTargetLogicalEntryPtr->tunnelStartInfo.ptr =
            genTargetLogicalEntryPtr->outlifPointer.tunnelStartPtr.ptr;
    }
    switch(genTargetLogicalEntryPtr->interfaceInfo.type)
    {
    case CPSS_INTERFACE_PORT_E:
        dxChTargetLogicalEntryPtr->physicalInterface.type = CPSS_INTERFACE_PORT_E;
        rc = prvUtfHwFromSwDeviceNumberGet(genTargetLogicalEntryPtr->interfaceInfo.devPort.devNum,
                                           &(dxChTargetLogicalEntryPtr->physicalInterface.devPort.devNum));
        if(GT_OK != rc)
            return rc;
        dxChTargetLogicalEntryPtr->physicalInterface.devPort.portNum =
            genTargetLogicalEntryPtr->interfaceInfo.devPort.portNum;
        break;
    case CPSS_INTERFACE_TRUNK_E:
        dxChTargetLogicalEntryPtr->physicalInterface.type = CPSS_INTERFACE_TRUNK_E;
        dxChTargetLogicalEntryPtr->physicalInterface.trunkId =
            genTargetLogicalEntryPtr->interfaceInfo.trunkId;
        break;
    case CPSS_INTERFACE_VIDX_E:
        dxChTargetLogicalEntryPtr->physicalInterface.type = CPSS_INTERFACE_VIDX_E;
        dxChTargetLogicalEntryPtr->physicalInterface.vidx =
            genTargetLogicalEntryPtr->interfaceInfo.vidx;
        break;

    default:
        return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertDxChToGenericLogicalTargetEntry
*
* DESCRIPTION:
*       Convert device specific logical target entry into generic
*
* INPUTS:
*       dxChTargetLogicalEntryPtr - (pointer to) DxCh logical target entry
*
* OUTPUTS:
*       genTargetLogicalEntryPtr - (pointer to) generic logical target entry
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertDxChToGenericLogicalTargetEntry
(
    IN  CPSS_DXCH_OUTPUT_INTERFACE_STC  *dxChTargetLogicalEntryPtr,
    OUT PRV_TGF_OUTLIF_INFO_STC         *genTargetLogicalEntryPtr
)
{
    GT_STATUS rc;

    cpssOsMemSet(genTargetLogicalEntryPtr, 0, sizeof(PRV_TGF_OUTLIF_INFO_STC));
    if (dxChTargetLogicalEntryPtr->isTunnelStart == GT_TRUE)
    {
        genTargetLogicalEntryPtr->outlifType = PRV_TGF_OUTLIF_TYPE_TUNNEL_E;
        switch(dxChTargetLogicalEntryPtr->tunnelStartInfo.passengerPacketType)
        {
        case CPSS_DXCH_TUNNEL_PASSENGER_ETHERNET_E:
            genTargetLogicalEntryPtr->outlifPointer.tunnelStartPtr.passengerPacketType =
                PRV_TGF_TS_PASSENGER_PACKET_TYPE_ETHERNET_E;
            break;
        case CPSS_DXCH_TUNNEL_PASSENGER_OTHER_E:
            genTargetLogicalEntryPtr->outlifPointer.tunnelStartPtr.passengerPacketType =
                PRV_TGF_TS_PASSENGER_PACKET_TYPE_OTHER_E;
            break;
        default:
            return GT_BAD_PARAM;
        }
        genTargetLogicalEntryPtr->outlifPointer.tunnelStartPtr.ptr =
            dxChTargetLogicalEntryPtr->tunnelStartInfo.ptr;
    }
    else
        genTargetLogicalEntryPtr->outlifType = PRV_TGF_OUTLIF_TYPE_LL_E;

    switch(dxChTargetLogicalEntryPtr->physicalInterface.type)
    {
    case CPSS_INTERFACE_PORT_E:
        genTargetLogicalEntryPtr->interfaceInfo.type = CPSS_INTERFACE_PORT_E;
        rc = prvUtfSwFromHwDeviceNumberGet(dxChTargetLogicalEntryPtr->physicalInterface.devPort.devNum,
                                           &(genTargetLogicalEntryPtr->interfaceInfo.devPort.devNum));
        if(GT_OK != rc)
            return rc;
        genTargetLogicalEntryPtr->interfaceInfo.devPort.devNum =
            dxChTargetLogicalEntryPtr->physicalInterface.devPort.devNum;
        genTargetLogicalEntryPtr->interfaceInfo.devPort.portNum =
            dxChTargetLogicalEntryPtr->physicalInterface.devPort.portNum;
        break;
    case CPSS_INTERFACE_TRUNK_E:
        genTargetLogicalEntryPtr->interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
        genTargetLogicalEntryPtr->interfaceInfo.trunkId =
            dxChTargetLogicalEntryPtr->physicalInterface.trunkId;
        break;
    case CPSS_INTERFACE_VIDX_E:
        genTargetLogicalEntryPtr->interfaceInfo.type = CPSS_INTERFACE_VIDX_E;
        genTargetLogicalEntryPtr->interfaceInfo.vidx =
            dxChTargetLogicalEntryPtr->physicalInterface.vidx;
        break;

    default:
        return GT_BAD_PARAM;
    }

    return GT_OK;
}
#endif


/*******************************************************************************
* prvTgfLogicalTargetMappingEnableSet
*
* DESCRIPTION:
*       Enable/disable logical port mapping feature on the specified device.
*
* INPUTS:
*       devNum   - physical device number
*       enable   - GT_TRUE:  enable
*                  GT_FALSE: disable .
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfLogicalTargetMappingEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
)
{
#ifdef CHX_FAMILY

    GT_STATUS rc = GT_OK;
    /* call device specific API */
    rc = cpssDxChLogicalTargetMappingEnableSet(devNum, enable);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChLogicalTargetMappingEnableSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;

#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */

}


/*******************************************************************************
* prvTgfLogicalTargetMappingEnableGet
*
* DESCRIPTION:
*       Get the Enable/Disable status logical port mapping feature on the
*       specified  device.
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       enablePtr - Pointer to the enable/disable state.
*                   GT_TRUE : enable,
*                   GT_FALSE: disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfLogicalTargetMappingEnableGet
(
    IN   GT_U8     devNum,
    OUT  GT_BOOL   *enablePtr
)
{
#ifdef CHX_FAMILY

    GT_STATUS rc = GT_OK;
    /* call device specific API */
    rc = cpssDxChLogicalTargetMappingEnableGet(devNum, enablePtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChLogicalTargetMappingEnableGet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;

#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */

}


/*******************************************************************************
* prvTgfLogicalTargetMappingDeviceEnableSet
*
* DESCRIPTION:
*       Enable/disable a target device to be considered as a logical device
*       on the specified device.
*
* INPUTS:
*       devNum              - physical device number
*       logicalTargetDevNum - logical target device number.
*       enable              - GT_TRUE:  enable
*                             GT_FALSE: disable .
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, logicalDevNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfLogicalTargetMappingDeviceEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     logicalTargetDevNum,
    IN  GT_BOOL   enable
)
{
#ifdef CHX_FAMILY

    GT_STATUS rc = GT_OK;
    /* call device specific API */
    rc = cpssDxChLogicalTargetMappingDeviceEnableSet(devNum, logicalTargetDevNum, enable );
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChLogicalTargetMappingDeviceEnableSet FAILED, rc = [%d]", rc);

        return rc;
    }
    return rc;

#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */

}


/*******************************************************************************
* prvTgfLogicalTargetMappingDeviceEnableGet
*
* DESCRIPTION:
*       Get Enable/disable status of target device to be considered as a logical device
*       on the specified device.
*
* INPUTS:
*       devNum              - physical device number
*       logicalTargetDevNum - logical target device number.
*
* OUTPUTS:
*       enablePtr     - Pointer to the  Enable/disable state.
*                       GT_TRUE : enable,
*                       GT_FALSE: disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, logicalDevNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfLogicalTargetMappingDeviceEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     logicalTargetDevNum,
    OUT  GT_BOOL   *enablePtr
)
{
#ifdef CHX_FAMILY

    GT_STATUS rc = GT_OK;
    /* call device specific API */
    rc = cpssDxChLogicalTargetMappingDeviceEnableGet(devNum, logicalTargetDevNum, enablePtr );
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChLogicalTargetMappingDeviceEnableGet FAILED, rc = [%d]", rc);

        return rc;
    }
    return rc;

#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */

}

/*******************************************************************************
* prvTgfLogicalTargetMappingTableEntrySet
*
* DESCRIPTION:
*       Set logical target mapping table entry.
*
* INPUTS:
*       devNum                       - physical device number.
*       logicalTargetDevNum          - logical traget device number.
*       logicalTargetPortNum         - logical target port number.
*       logicalTargetMappingEntryPtr - points to logical target Mapping  entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum, logicalDevNum, logicalPortNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - on the memebers of virtual port entry struct out of range.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfLogicalTargetMappingTableEntrySet
(
    IN GT_U8                           devNum,
    IN GT_U8                           logicalTargetDevNum,
    IN GT_U8                           logicalTargetPortNum,
    IN PRV_TGF_OUTLIF_INFO_STC         *logicalTargetMappingEntryPtr
)
{
#ifdef CHX_FAMILY

    GT_STATUS   rc = GT_OK;
    CPSS_DXCH_OUTPUT_INTERFACE_STC    dxChLogicalTargetMappingEntry;

    rc = prvTgfConvertGenericToDxChLogicalTargetEntry(logicalTargetMappingEntryPtr,
                                                      &dxChLogicalTargetMappingEntry);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChLogicalTargetEntry FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChLogicalTargetMappingTableEntrySet(devNum,
                                                   logicalTargetDevNum,
                                                   logicalTargetPortNum,
                                                   &dxChLogicalTargetMappingEntry );
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChLogicalTargetMappingTableEntrySet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;

#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */

}

/*******************************************************************************
* prvTgfLogicalTargetMappingTableEntryGet
*
* DESCRIPTION:
*       Get logical target mapping table entry.
*
* INPUTS:
*       devNum                       - physical device number.
*       logicalTargetDevNum          - logical target device number.
*       logicalTargetPortNum         - logical target port number.
*
* OUTPUTS:
*       logicalTargetMappingEntryPtr  - points to logical target Mapping  entry
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum, logicalDevNum, logicalPortNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - on the memebers of virtual port entry struct out of range.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfLogicalTargetMappingTableEntryGet
(
    IN GT_U8                            devNum,
    IN GT_U8                            logicalTargetDevNum,
    IN GT_U8                            logicalTargetPortNum,
    OUT PRV_TGF_OUTLIF_INFO_STC         *logicalTargetMappingEntryPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    CPSS_DXCH_OUTPUT_INTERFACE_STC  dxChLogicalTargetEntry;

    /* initialize dxChLogicalTargetEntry */
    cpssOsMemSet(&dxChLogicalTargetEntry , 0, sizeof(dxChLogicalTargetEntry));

    /* call device specific API */
    rc = cpssDxChLogicalTargetMappingTableEntryGet(devNum,
                                                   logicalTargetDevNum,
                                                   logicalTargetPortNum,
                                                   &dxChLogicalTargetEntry);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChLogicalTargetMappingTableEntryGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* Convert DXCH  logical target entry into generic format */
    rc = prvTgfConvertDxChToGenericLogicalTargetEntry(&dxChLogicalTargetEntry,
                                                       logicalTargetMappingEntryPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertDxChToGenericLogicalTargetEntry FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;

#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */

}




