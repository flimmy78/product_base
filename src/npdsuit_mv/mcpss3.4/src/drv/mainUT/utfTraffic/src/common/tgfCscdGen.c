/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfCscdGen.c
*
* DESCRIPTION:
*       Generic API implementation for Cscd
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
#include <common/tgfCscdGen.h>

#ifdef CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/cscd/cpssDxChCscd.h>
#endif /* CHX_FAMILY */

/******************************************************************************\
 *                           Private declarations                             *
\******************************************************************************/

/******************************************************************************\
 *                     Private function's implementation                      *
\******************************************************************************/

/******************************************************************************\
 *                       CPSS generic API section                             *
\******************************************************************************/
/*******************************************************************************
* prvTgfCscdDevMapTableSet
*
* DESCRIPTION:
*            Set the cascade map table . the map table define the local port or
*            trunk that packets destined to a destination device
*            should be transmitted to.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum         - physical device number
*       targetDevNum   - the device to be reached via cascade (0..31)
*       targetPortNum  - target port number
*                        Relevant only for Lion and above and for
*                        CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E mode,
*                        otherwise ignored.
*       cascadeLinkPtr - (pointer to)A structure holding the cascade link type
*                      (port (0..63) or trunk(0..127)) and the link number
*                      leading to the target device.
*       srcPortTrunkHashEn - Relevant when (cascadeLinkPtr->linkType ==
*                       CPSS_CSCD_LINK_TYPE_TRUNK_E)
*                       Enabled to set the load balancing trunk hash for packets
*                       forwarded via an trunk uplink to be based on the
*                       packet’s source port, and not on the packets data.
*                       Indicates the type of uplink.
*                       PRV_TGF_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E -
*                       Load balancing trunk hash is based on the ingress pipe
*                       hash mode as configured by function
*                       cpssDxChTrunkHashGeneralModeSet(...)
*                       PRV_TGF_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E - Load
*                       balancing trunk hash for this cascade trunk interface is
*                       based on the packet’s source port, regardless of the
*                       ingress pipe hash mode
*                       NOTE : this parameter is relevant only to DXCH2
*                       and above devices
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM     - on wrong device or target device or target port
*                          or bad trunk hash mode
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCscdDevMapTableSet
(
    IN GT_U8                        devNum,
    IN GT_U8                        targetDevNum,
    IN GT_U8                        targetPortNum,
    IN CPSS_CSCD_LINK_TYPE_STC      *cascadeLinkPtr,
    IN PRV_TGF_CSCD_TRUNK_LINK_HASH_ENT srcPortTrunkHashEn
)
{
#ifdef CHX_FAMILY
    CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT dxSrcPortTrunkHashEn;
    GT_STATUS rc;
    GT_U8                     hwDevNum;             /* hardware device number */

#endif /* CHX_FAMILY */

#ifdef CHX_FAMILY
    switch(srcPortTrunkHashEn)
    {
        case PRV_TGF_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E:
            dxSrcPortTrunkHashEn = CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E;
            break;
        case PRV_TGF_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E:
            dxSrcPortTrunkHashEn = CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E;
            break;
        case PRV_TGF_CSCD_TRUNK_LINK_HASH_IS_DST_PORT_E:
            dxSrcPortTrunkHashEn = CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_DST_PORT_E;
            break;
        default:
            return GT_BAD_PARAM;
    }

    rc = prvUtfHwFromSwDeviceNumberGet(targetDevNum, &hwDevNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* call device specific API */
    return cpssDxChCscdDevMapTableSet(devNum, hwDevNum, targetPortNum,cascadeLinkPtr,dxSrcPortTrunkHashEn);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(targetDevNum);
    TGF_PARAM_NOT_USED(targetPortNum);
    TGF_PARAM_NOT_USED(cascadeLinkPtr);
    TGF_PARAM_NOT_USED(srcPortTrunkHashEn);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfCscdDevMapTableGet
*
* DESCRIPTION:
*            Get the cascade map table
*
* INPUTS:
*       devNum        - device number
*       targetDevNum  - the device to be reached via cascade
*       targetPortNum - target port number
*
* OUTPUTS:
*       cascadeLinkPtr        - (pointer to) structure holding the cascade link type
*       srcPortTrunkHashEnPtr - (pointer to) set the load balancing trunk
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong device or target device or target port
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCscdDevMapTableGet
(
    IN  GT_U8                             devNum,
    IN  GT_U8                             targetDevNum,
    IN  GT_U8                             targetPortNum,
    OUT CPSS_CSCD_LINK_TYPE_STC          *cascadeLinkPtr,
    OUT PRV_TGF_CSCD_TRUNK_LINK_HASH_ENT *srcPortTrunkHashEnPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS                          rc       = GT_OK;
    GT_U8                              hwDevNum = 0;
    CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT dxChSrcPortTrunkHashEn;


    /* get HW device number */
    rc = prvUtfHwFromSwDeviceNumberGet(targetDevNum, &hwDevNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* call device specific API */
    rc = cpssDxChCscdDevMapTableGet(devNum, hwDevNum, targetPortNum,
                                    cascadeLinkPtr, &dxChSrcPortTrunkHashEn);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCscdDevMapTableGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert from device specific format */
    switch(dxChSrcPortTrunkHashEn)
    {
        case CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E:
            *srcPortTrunkHashEnPtr = PRV_TGF_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E;
            break;
        case CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E:
            *srcPortTrunkHashEnPtr = PRV_TGF_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E;
            break;
        case CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_DST_PORT_E:
            *srcPortTrunkHashEnPtr = PRV_TGF_CSCD_TRUNK_LINK_HASH_IS_DST_PORT_E;
            break;
        default:
            return GT_BAD_PARAM;
    }

    return rc;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(targetDevNum);
    TGF_PARAM_NOT_USED(targetPortNum);
    TGF_PARAM_NOT_USED(cascadeLinkPtr);
    TGF_PARAM_NOT_USED(srcPortTrunkHashEnPtr);

    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfCscdDevMapLookupModeSet
*
* DESCRIPTION:
*        Set lookup mode for accessing the Device Map table.
*
* APPLICABLE DEVICES: Lion.
*
* INPUTS:
*       mode         - device Map lookup mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong device or mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCscdDevMapLookupModeSet
(
    IN PRV_TGF_DEV_MAP_LOOKUP_MODE_ENT   mode
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;
    CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT dxMode;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* go over all active devices */
    while (GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        switch(mode)
        {
            case PRV_TGF_DEV_MAP_LOOKUP_MODE_TRG_DEV_E:
                dxMode = CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_E;
                break;
            case PRV_TGF_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E:
                dxMode = CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E;
                break;
            default:
                return GT_BAD_PARAM;
        }

        rc = cpssDxChCscdDevMapLookupModeSet(devNum,dxMode);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCscdDevMapLookupModeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#else
    /* avoid warnings */
    TGF_PARAM_NOT_USED(mode);

    /* not applicable for not DXCH devices */
    return GT_NOT_APPLICABLE_DEVICE;
#endif /* CHX_FAMILY */
}

/*******************************************************************************
* prvTgfCscdDevMapLookupModeGet
*
* DESCRIPTION:
*       Get lookup mode for accessing the Device Map table
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       modePtr - (pointer to) device Map lookup mode
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong device 
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_BAD_STATE - wrong hardware value.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCscdDevMapLookupModeGet
(
    IN  GT_U8                             devNum,
    OUT PRV_TGF_DEV_MAP_LOOKUP_MODE_ENT   *modePtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS                         rc = GT_OK;
    CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT dxChMode;


    /* call device specific API */
    rc = cpssDxChCscdDevMapLookupModeGet(devNum, &dxChMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCscdDevMapLookupModeGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert from device specific format */
    switch(dxChMode)
    {
        case CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_E:
            *modePtr = PRV_TGF_DEV_MAP_LOOKUP_MODE_TRG_DEV_E;
            break;
        case CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E:
            *modePtr = PRV_TGF_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E;
            break;
        default:
            return GT_BAD_PARAM;
    }

    return rc;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(modePtr);

    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfCscdPortLocalDevMapLookupEnableSet 
*
* DESCRIPTION:
*       Enable / Disable the local target port for device map lookup 
*       for local device.
*
* APPLICABLE DEVICES: Lion.
*
* INPUTS:
*       portNum         - port number or CPU port
*       portDirection   - port's direction:
*                         CPSS_DIRECTION_INGRESS_E - source port 
*                         CPSS_DIRECTION_EGRESS_E  -  target port
*                         CPSS_DIRECTION_BOTH_E    - both source and target ports
*       enable       - GT_TRUE  - the port is enabled for device map lookup
*                                 for local device. 
*                    - GT_FALSE - No access to Device map table for
*                                 local devices.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong device or port number or portDirection
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To enable access to the Device Map Table for the local target devices
*       -  Enable the local source port for device map lookup     
*       -  Enable the local target port for device map lookup
*
*******************************************************************************/
GT_STATUS prvTgfCscdPortLocalDevMapLookupEnableSet
(
    IN GT_U8                portNum,
    IN CPSS_DIRECTION_ENT   portDirection,   
    IN GT_BOOL              enable
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* go over all active devices */
    while (GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        rc = cpssDxChCscdPortLocalDevMapLookupEnableSet(devNum, portNum,
                                                        portDirection, 
                                                        enable);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCscdDevMapLookupModeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#else
    /* avoid warnings */
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(portDirection);
    TGF_PARAM_NOT_USED(enable);

    /* not applicable for not DXCH devices */
    return GT_NOT_APPLICABLE_DEVICE;
#endif /* CHX_FAMILY */
}


