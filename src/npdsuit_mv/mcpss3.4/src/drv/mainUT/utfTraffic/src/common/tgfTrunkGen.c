/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfTrunkGen.c
*
* DESCRIPTION:
*       Generic API implementation for Trunk
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
#include <common/tgfTrunkGen.h>


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
* prvTgfTrunkMembersSet
*
* DESCRIPTION:
*       This function set the trunk members
*
* INPUTS:
*       trunkId              - trunk id
*       enabledMembersArray  - (array of) enabled members
*       numOfEnabledMembers  - number of enabled members in the array
*       disabledMembersArray - (array of) disabled members
*       numOfDisabledMembers - number of enabled members in the array
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_FAIL            - on error
*       GT_NOT_INITIALIZED - trunk library was not initialized
*       GT_HW_ERROR        - on hardware error
*       GT_OUT_OF_RANGE    - some values are out of range
*       GT_BAD_PARAM       - on illegal parameters
*       GT_BAD_PTR         - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST   - one of the members already exists in another trunk
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTrunkMembersSet
(
    IN GT_TRUNK_ID                    trunkId,
    IN GT_U32                         numOfEnabledMembers,
    IN CPSS_TRUNK_MEMBER_STC          enabledMembersArray[],
    IN GT_U32                         numOfDisabledMembers,
    IN CPSS_TRUNK_MEMBER_STC          disabledMembersArray[]
)
{
#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
   GT_U8       devNum  = 0;
   GT_STATUS   rc, rc1 = GT_OK;
   GT_U32      i;

   /* prepare device iterator */
   PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

   /* go over all active devices */
   while (GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
   {
       for(i = 0 ; i < numOfEnabledMembers ; i++)
       {
           rc = prvUtfHwFromSwDeviceNumberGet(enabledMembersArray[i].device,
                                              &(enabledMembersArray[i].device));
           if (GT_OK != rc)
           {
               PRV_UTF_LOG1_MAC(
                   "[TGF]: prvUtfHwFromSwDeviceNumberGet FAILED, rc = [%d]", rc);

               return rc;
           }
       }

       for(i = 0 ; i < numOfDisabledMembers ; i++)
       {
           rc = prvUtfHwFromSwDeviceNumberGet(disabledMembersArray[i].device,
                                              &(disabledMembersArray[i].device));
           if (GT_OK != rc)
           {
               PRV_UTF_LOG1_MAC(
                   "[TGF]: prvUtfHwFromSwDeviceNumberGet FAILED, rc = [%d]", rc);

               return rc;
           }
       }

       /* call device specific API */
#ifdef EXMXPM_FAMILY
       rc = cpssExMxPmTrunkMembersSet(
#else
       rc = cpssDxChTrunkMembersSet(
#endif
            devNum, trunkId, numOfEnabledMembers, enabledMembersArray,
            numOfDisabledMembers, disabledMembersArray);
       if (GT_OK != rc)
       {
#ifdef EXMXPM_FAMILY
           PRV_UTF_LOG1_MAC(
               "[TGF]: cpssExMxPmTrunkMembersSet FAILED, rc = [%d]", rc);
#else
           PRV_UTF_LOG1_MAC(
               "[TGF]: cpssDxChTrunkMembersSet FAILED, rc = [%d]", rc);
#endif

           rc1 = rc;
       }

       for(i = 0 ; i < numOfEnabledMembers ; i++)
       {
           rc = prvUtfSwFromHwDeviceNumberGet(enabledMembersArray[i].device,
                                              &(enabledMembersArray[i].device));
           if (GT_OK != rc)
           {
               PRV_UTF_LOG1_MAC(
                   "[TGF]: prvUtfSwFromHwDeviceNumberGet FAILED, rc = [%d]", rc);

               return rc;
           }
       }

       for(i = 0 ; i < numOfDisabledMembers ; i++)
       {
           rc = prvUtfSwFromHwDeviceNumberGet(disabledMembersArray[i].device,
                                              &(disabledMembersArray[i].device));
           if (GT_OK != rc)
           {
               PRV_UTF_LOG1_MAC(
                   "[TGF]: prvUtfSwFromHwDeviceNumberGet FAILED, rc = [%d]", rc);

               return rc;
           }
       }
   }

   return rc1;

#else
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfTrunkHashGlobalModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the general hashing mode of trunk hash generation.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       hashMode   - hash mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or hash mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none
*******************************************************************************/
GT_STATUS prvTgfTrunkHashGlobalModeSet
(
    IN PRV_TGF_TRUNK_LBH_GLOBAL_MODE_ENT  hashMode
)
{
#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
   GT_U8       devNum  = 0;
   GT_STATUS   rc, rc1 = GT_OK;

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_TRUNK_LBH_GLOBAL_MODE_ENT  cpssHashMode;
#else
    CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT  cpssHashMode;
#endif

    switch (hashMode)
    {
        case PRV_TGF_TRUNK_LBH_PACKETS_INFO_E:
#ifdef EXMXPM_FAMILY
            cpssHashMode = CPSS_EXMXPM_TRUNK_LBH_PACKETS_INFO_E;
#else
            cpssHashMode = CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_E;
#endif
            break;
        case PRV_TGF_TRUNK_LBH_INGRESS_PORT_E:
#ifdef EXMXPM_FAMILY
            cpssHashMode = CPSS_EXMXPM_TRUNK_LBH_INGRESS_PORT_E;
#else
            cpssHashMode = CPSS_DXCH_TRUNK_LBH_INGRESS_PORT_E;
#endif
            break;

#ifdef EXMXPM_FAMILY
        case PRV_TGF_TRUNK_LBH_PERFECT_E:
            cpssHashMode = CPSS_EXMXPM_TRUNK_LBH_PERFECT_E;
            break;
#endif

#ifndef EXMXPM_FAMILY
        case PRV_TGF_TRUNK_LBH_PACKETS_INFO_CRC_E:
            cpssHashMode = CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_CRC_E;
            break;
#endif
        default:
            return GT_BAD_PARAM;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while (GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
#ifdef EXMXPM_FAMILY
        rc = cpssExMxPmTrunkHashGlobalModeSet(devNum, cpssHashMode);
#else
        rc = cpssDxChTrunkHashGlobalModeSet(devNum, cpssHashMode);
#endif
        if (GT_OK != rc)
        {
#ifdef EXMXPM_FAMILY
            PRV_UTF_LOG1_MAC(
                "[TGF]: cpssExMxPmTrunkHashGlobalModeSet FAILED, rc = [%d]", rc);
#else
            PRV_UTF_LOG1_MAC(
                "[TGF]: cpssDxChTrunkHashGlobalModeSet FAILED, rc = [%d]", rc);
#endif

            rc1 = rc;
        }
    }

    return rc1;
#else
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfTrunkHashGlobalModeGet
*
* DESCRIPTION:
*       Get the general hashing mode of trunk hash generation
*
* INPUTS:
*       devNum - the device number
*
* OUTPUTS:
*       hashModePtr - (pointer to) hash mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or hash mode
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none
*******************************************************************************/
GT_STATUS prvTgfTrunkHashGlobalModeGet
(
    IN  GT_U8                               devNum,
    OUT PRV_TGF_TRUNK_LBH_GLOBAL_MODE_ENT  *hashModePtr
)
{
#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
   GT_STATUS   rc = GT_OK;

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_TRUNK_LBH_GLOBAL_MODE_ENT  cpssHashMode;
#else
    CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT  cpssHashMode;
#endif

    /* call device specific API */
#ifdef EXMXPM_FAMILY
    rc = cpssExMxPmTrunkHashGlobalModeGet(devNum, &cpssHashMode);
#else
    rc = cpssDxChTrunkHashGlobalModeGet(devNum, &cpssHashMode);
#endif
    if (GT_OK != rc)
    {
#ifdef EXMXPM_FAMILY
        PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmTrunkHashGlobalModeSet FAILED, rc = [%d]", rc);
#else
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChTrunkHashGlobalModeSet FAILED, rc = [%d]", rc);
#endif
        return rc;
    }

    /* convert from device specific format */
    switch (cpssHashMode)
    {
#ifdef EXMXPM_FAMILY
        case CPSS_EXMXPM_TRUNK_LBH_PACKETS_INFO_E:
#else
        case CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_E:
#endif
            *hashModePtr = PRV_TGF_TRUNK_LBH_PACKETS_INFO_E;

            break;
#ifdef EXMXPM_FAMILY
        case CPSS_EXMXPM_TRUNK_LBH_INGRESS_PORT_E:
#else
        case CPSS_DXCH_TRUNK_LBH_INGRESS_PORT_E:
#endif
            *hashModePtr = PRV_TGF_TRUNK_LBH_INGRESS_PORT_E;
            break;
#ifdef EXMXPM_FAMILY
        case CPSS_EXMXPM_TRUNK_LBH_PERFECT_E:
            *hashModePtr = PRV_TGF_TRUNK_LBH_PERFECT_E;
            break;
#endif
        default:
            return GT_BAD_PARAM;
    }

    return rc;

#else
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfTrunkHashMplsModeEnableSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Enable/Disable the device from considering the MPLS information,
*       when calculating the trunk hashing index for a packet.
*
*       Note:
*       1. Not relevant to NON-MPLS packets.
*       2. Relevant when cpssExMxPmTrunkHashGlobalModeSet(devNum,
*                       CPSS_EXMXPM_TRUNK_LBH_PACKETS_INFO_E)
*
* INPUTS:
*       enable - GT_FALSE - MPLS parameter are not used in trunk hash index
*                GT_TRUE  - The following function is added to the trunk load
*                   balancing hash:
*               MPLSTrunkHash = (mpls_label0[5:0] & mpls_trunk_lbl0_mask) ^
*                               (mpls_label1[5:0] & mpls_trunk_lbl1_mask) ^
*                               (mpls_label2[5:0] & mpls_trunk_lbl2_mask)
*               NOTE:
*                   If any of MPLS Labels 0:2 do not exist in the packet,
*                   the default value 0x0 is used for TrunkHash calculation
*                   instead.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTrunkHashMplsModeEnableSet
(
    IN GT_BOOL enable
)
{
#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
   GT_U8       devNum  = 0;
   GT_STATUS   rc, rc1 = GT_OK;


   /* prepare device iterator */
   PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

   /* go over all active devices */
   while (GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
   {
       /* call device specific API */
#ifdef EXMXPM_FAMILY
       rc = cpssExMxPmTrunkHashMplsModeEnableSet(devNum, enable);
#else
       rc = cpssDxChTrunkHashMplsModeEnableSet(devNum, enable);
#endif
       if (GT_OK != rc)
       {
#ifdef EXMXPM_FAMILY
           PRV_UTF_LOG1_MAC(
               "[TGF]: cpssDxChTrunkHashMplsModeEnableSet FAILED, rc = [%d]", rc);
#else
           PRV_UTF_LOG1_MAC(
               "[TGF]: cpssDxChTrunkHashMplsModeEnableSet FAILED, rc = [%d]", rc);
#endif

           rc1 = rc;
       }
   }

   return rc1;
#else
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfTrunkHashMaskSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the masks for the various packet fields being used at the Trunk
*       hash calculations
*
* INPUTS:
*       maskedField - field to apply the mask on
*       maskValue - The mask value to be used (0..0x3F)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or maskedField
*       GT_OUT_OF_RANGE          - maskValue > 0x3F
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS prvTgfTrunkHashMaskSet
(
    IN PRV_TGF_TRUNK_LBH_MASK_ENT     maskedField,
    IN GT_U8                          maskValue
)
{
#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
   GT_U8       devNum  = 0;
   GT_STATUS   rc, rc1 = GT_OK;
#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_TRUNK_LBH_MASK_ENT  maskField;
#else
    CPSS_DXCH_TRUNK_LBH_MASK_ENT    maskField;
#endif

    switch (maskedField)
    {
        case PRV_TGF_TRUNK_LBH_MASK_MAC_DA_E:
#ifdef EXMXPM_FAMILY
            maskField = CPSS_EXMXPM_TRUNK_LBH_MASK_MAC_DA_E;
#else
            maskField = CPSS_DXCH_TRUNK_LBH_MASK_MAC_DA_E;
#endif
            break;
        case PRV_TGF_TRUNK_LBH_MASK_MAC_SA_E:
#ifdef EXMXPM_FAMILY
            maskField = CPSS_EXMXPM_TRUNK_LBH_MASK_MAC_SA_E;
#else
            maskField = CPSS_DXCH_TRUNK_LBH_MASK_MAC_SA_E;
#endif
            break;
        case PRV_TGF_TRUNK_LBH_MASK_MPLS_LABEL0_E:
#ifdef EXMXPM_FAMILY
            maskField = CPSS_EXMXPM_TRUNK_LBH_MASK_MPLS_LABEL0_E;
#else
            maskField = CPSS_DXCH_TRUNK_LBH_MASK_MPLS_LABEL0_E;
#endif
            break;
        case PRV_TGF_TRUNK_LBH_MASK_MPLS_LABEL1_E:
#ifdef EXMXPM_FAMILY
            maskField = CPSS_EXMXPM_TRUNK_LBH_MASK_MPLS_LABEL1_E;
#else
            maskField = CPSS_DXCH_TRUNK_LBH_MASK_MPLS_LABEL1_E;
#endif
            break;
        case PRV_TGF_TRUNK_LBH_MASK_MPLS_LABEL2_E:
#ifdef EXMXPM_FAMILY
            maskField = CPSS_EXMXPM_TRUNK_LBH_MASK_MPLS_LABEL2_E;
#else
            maskField = CPSS_DXCH_TRUNK_LBH_MASK_MPLS_LABEL2_E;
#endif
            break;
        case PRV_TGF_TRUNK_LBH_MASK_IPV4_DIP_E:
#ifdef EXMXPM_FAMILY
            maskField = CPSS_EXMXPM_TRUNK_LBH_MASK_IPV4_DIP_E;
#else
            maskField = CPSS_DXCH_TRUNK_LBH_MASK_IPV4_DIP_E;
#endif
            break;
        case PRV_TGF_TRUNK_LBH_MASK_IPV4_SIP_E:
#ifdef EXMXPM_FAMILY
            maskField = CPSS_EXMXPM_TRUNK_LBH_MASK_IPV4_SIP_E;
#else
            maskField = CPSS_DXCH_TRUNK_LBH_MASK_IPV4_SIP_E;
#endif
            break;
        case PRV_TGF_TRUNK_LBH_MASK_IPV6_DIP_E:
#ifdef EXMXPM_FAMILY
            maskField = CPSS_EXMXPM_TRUNK_LBH_MASK_IPV6_DIP_E;
#else
            maskField = CPSS_DXCH_TRUNK_LBH_MASK_IPV6_DIP_E;
#endif
            break;
        case PRV_TGF_TRUNK_LBH_MASK_IPV6_SIP_E:
#ifdef EXMXPM_FAMILY
            maskField = CPSS_EXMXPM_TRUNK_LBH_MASK_IPV6_SIP_E;
#else
            maskField = CPSS_DXCH_TRUNK_LBH_MASK_IPV6_SIP_E;
#endif
            break;
        case PRV_TGF_TRUNK_LBH_MASK_IPV6_FLOW_E:
#ifdef EXMXPM_FAMILY
            maskField = CPSS_EXMXPM_TRUNK_LBH_MASK_IPV6_FLOW_E;
#else
            maskField = CPSS_DXCH_TRUNK_LBH_MASK_IPV6_FLOW_E;
#endif
            break;
        case PRV_TGF_TRUNK_LBH_MASK_L4_DST_PORT_E:
#ifdef EXMXPM_FAMILY
            maskField = CPSS_EXMXPM_TRUNK_LBH_MASK_L4_DST_PORT_E;
#else
            maskField = CPSS_DXCH_TRUNK_LBH_MASK_L4_DST_PORT_E;
#endif
            break;
        case PRV_TGF_TRUNK_LBH_MASK_L4_SRC_PORT_E:
#ifdef EXMXPM_FAMILY
            maskField = CPSS_EXMXPM_TRUNK_LBH_MASK_L4_SRC_PORT_E;
#else
            maskField = CPSS_DXCH_TRUNK_LBH_MASK_L4_SRC_PORT_E;
#endif
            break;
        default: return GT_BAD_PARAM;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while (GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
#ifdef EXMXPM_FAMILY
        rc = cpssExMxPmTrunkHashMaskSet(devNum, maskField, maskValue);
#else
        rc = cpssDxChTrunkHashMaskSet(devNum, maskField, maskValue);
#endif
        if (GT_OK != rc)
        {
#ifdef EXMXPM_FAMILY
            PRV_UTF_LOG1_MAC(
                "[TGF]: cpssExMxPmTrunkHashMaskSet FAILED, rc = [%d]", rc);
#else
            PRV_UTF_LOG1_MAC(
                "[TGF]: cpssDxChTrunkHashMaskSet FAILED, rc = [%d]", rc);
#endif

            rc1 = rc;
        }
    }

    return rc1;

#else
    return GT_BAD_STATE;
#endif
}


/*******************************************************************************
* prvTgfTrunkHashIpShiftSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the shift being done to IP addresses prior to hash calculations.
*
* INPUTS:
*       protocolStack - Set the shift to either IPv4 or IPv6 IP addresses.
*       isSrcIp       - GT_TRUE  = Set the shift to IPv4/6 source addresses.
*                       GT_FALSE = Set the shift to IPv4/6 destination addresses.
*       shiftValue    - The shift to be done.
*                       IPv4 valid shift: 0-3 bytes (Value = 0: no shift).
*                       IPv6 valid shift: 0-15 bytes (Value = 0: no shift).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or protocolStack
*       GT_OUT_OF_RANGE - shiftValue > 3 for IPv4 , shiftValue > 15 for IPv6
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS prvTgfTrunkHashIpShiftSet
(
    IN CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    IN GT_BOOL                      isSrcIp,
    IN GT_U32                       shiftValue
)
{
#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
   GT_U8       devNum  = 0;
   GT_STATUS   rc, rc1 = GT_OK;

   /* prepare device iterator */
   PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

   /* go over all active devices */
   while (GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
   {
       /* call device specific API */
#ifdef EXMXPM_FAMILY
       rc = cpssExMxPmTrunkHashIpShiftSet(
#else
       rc = cpssDxChTrunkHashIpShiftSet(
#endif
           devNum, protocolStack, isSrcIp, shiftValue);
       if (GT_OK != rc)
       {
#ifdef EXMXPM_FAMILY
           PRV_UTF_LOG1_MAC(
               "[TGF]: cpssExMxPmTrunkHashIpShiftSet FAILED, rc = [%d]", rc);
#else
           PRV_UTF_LOG1_MAC(
               "[TGF]: cpssDxChTrunkHashIpShiftSet FAILED, rc = [%d]", rc);
#endif

           rc1 = rc;
       }
   }

   return rc1;

#else
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfTrunkHashIpModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Enable/Disable the device from considering the IP SIP/DIP information,
*       when calculation the trunk hashing index for a packet.
*       Relevant when the is IPv4 or IPv6 and <TrunkHash Mode> = 0.
*
*       Note:
*       1. Not relevant to NON Ip packets.
*       2. Not relevant to multi-destination packets (include routed IPM).
*       3. Not relevant when cpssDxChTrunkHashGlobalModeSet(devNum,
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E)
*
* INPUTS:
*       enable - GT_FALSE - IP data is not added to the trunk load balancing
*                           hash.
*               GT_TRUE - The following function is added to the trunk load
*                         balancing hash, if the packet is IPv6.
*                         IPTrunkHash = according to setting of API
*                                       cpssDxChTrunkHashIpv6ModeSet(...)
*                         else packet is IPv4:
*                         IPTrunkHash = SIP[5:0]^DIP[5:0]^SIP[21:16]^DIP[21:16].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTrunkHashIpModeSet
(
    IN GT_BOOL enable
)
{
#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
   GT_U8       devNum  = 0;
   GT_STATUS   rc, rc1 = GT_OK;

   /* prepare device iterator */
   PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

   /* go over all active devices */
   while (GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
   {
       /* call device specific API */
#ifdef EXMXPM_FAMILY
       rc = cpssExMxPmTrunkHashIpModeEnableSet(devNum, enable);
#else
       rc = cpssDxChTrunkHashIpModeSet(devNum, enable);
#endif
       if (GT_OK != rc)
       {
#ifdef EXMXPM_FAMILY
           PRV_UTF_LOG1_MAC(
               "[TGF]: cpssExMxPmTrunkHashIpModeEnableSet FAILED, rc = [%d]", rc);
#else
           PRV_UTF_LOG1_MAC(
               "[TGF]: cpssDxChTrunkHashIpModeSet FAILED, rc = [%d]", rc);
#endif

           rc1 = rc;
       }
   }

   return rc1;

#else
    return GT_BAD_STATE;
#endif
}


/*******************************************************************************
* prvTgfTrunkDesignatedMemberSet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function Configures per-trunk the designated member -
*       value should be stored (to DB) even designated member is not currently
*       a member of Trunk.
*       Setting value replace previously assigned designated member.
*
*       NOTE that:
*       under normal operation this should not be used with cascade Trunks,
*       due to the fact that in this case it affects all types of traffic -
*       not just Multi-destination as in regular Trunks.
*
*  Diagram 1 : Regular operation - Traffic distribution on all enabled
*              members (when no specific designated member defined)
*
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*  index \ member  %   M1  %   M2   %    M3  %  M4  %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 0       %   1   %   0    %    0   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 1       %   0   %   1    %    0   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 2       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 3       %   0   %   0    %    0   %  1   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 4       %   1   %   0    %    0   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 5       %   0   %   1    %    0   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 6       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 7       %   0   %   0    %    0   %  1   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*  Diagram 2: Traffic distribution once specific designated member defined
*             (M3 in this case - which is currently enabled member in trunk)
*
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*  index \ member  %   M1  %   M2   %    M3  %  M4  %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 0       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 1       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 2       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 3       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 4       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 5       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 6       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 7       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum      - the device number
*       trunkId     - the trunk id.
*       enable      - enable/disable designated trunk member.
*                     GT_TRUE -
*                       1. Clears all current Trunk member's designated bits
*                       2. If input designated member, is currently an
*                          enabled-member at Trunk (in local device) enable its
*                          bits on all indexes
*                       3. Store designated member at the DB (new DB parameter
*                          should be created for that)
*
*                     GT_FALSE -
*                       1. Redistribute current Trunk members bits (on all enabled members)
*                       2. Clear designated member at  the DB
*
*       memberPtr   - (pointer to)the designated member we set to the trunk.
*                     relevant only when enable = GT_TRUE
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0xE0) != 0  means that the HW can't support
*                                              this value , since HW has 5 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST         - this member already exists in another trunk.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvTgfTrunkDesignatedMemberSet
(
    IN GT_TRUNK_ID              trunkId,
    IN GT_BOOL                  enable,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
{
#if (defined CHX_FAMILY)
   GT_U8       devNum  = 0;
   GT_STATUS   rc, rc1 = GT_OK;
   CPSS_TRUNK_MEMBER_STC localMember, *localMemberPtr;

   /* prepare device iterator */
   PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

   if( NULL != memberPtr)
   {
       localMember = *memberPtr;
       rc = prvUtfHwFromSwDeviceNumberGet(memberPtr->device, &(localMember.device));
       if (GT_OK != rc)
       {
           PRV_UTF_LOG1_MAC(
               "[TGF]: prvUtfHwFromSwDeviceNumberGet FAILED, rc = [%d]", rc);

           return rc;
       }
       localMemberPtr = &localMember;
   }
   else
       localMemberPtr = NULL;

   /* go over all active devices */
   while (GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
   {
       /* call device specific API */
       rc = cpssDxChTrunkDesignatedMemberSet(devNum, trunkId,enable, localMemberPtr);
       if (GT_OK != rc)
       {
           PRV_UTF_LOG1_MAC(
               "[TGF]: cpssDxChTrunkDesignatedMemberSet FAILED, rc = [%d]", rc);
           rc1 = rc;
       }
   }

   return rc1;

#else
    trunkId = trunkId;
    enable = enable;
    memberPtr = memberPtr;

    return GT_BAD_STATE;
#endif

}

/*******************************************************************************
* prvTgfTrunkDbDesignatedMemberGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function get Configuration per-trunk the designated member -
*       value should be stored (to DB) even designated member is not currently
*       a member of Trunk.
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum      - the device number
*       trunkId     - the trunk id.
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable/disable designated trunk member.
*                     GT_TRUE -
*                       1. Clears all current Trunk member's designated bits
*                       2. If input designated member, is currently an
*                          enabled-member at Trunk (in local device) enable its
*                          bits on all indexes
*                       3. Store designated member at the DB (new DB parameter
*                          should be created for that)
*
*                     GT_FALSE -
*                       1. Redistribute current Trunk members bits (on all enabled members)
*                       2. Clear designated member at  the DB
*
*       memberPtr   - (pointer to) the designated member of the trunk.
*                     relevant only when enable = GT_TRUE
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvTgfTrunkDbDesignatedMemberGet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    OUT GT_BOOL                 *enablePtr,
    OUT CPSS_TRUNK_MEMBER_STC   *memberPtr
)
{
#if (defined CHX_FAMILY)
   GT_STATUS   rc;

   /* call device specific API */
   rc = cpssDxChTrunkDbDesignatedMemberGet(devNum, trunkId,enablePtr,memberPtr);
   if (GT_OK != rc)
   {
       PRV_UTF_LOG1_MAC(
           "[TGF]: cpssDxChTrunkDbDesignatedMemberGet FAILED, rc = [%d]", rc);

       return rc;
   }

   if( NULL != memberPtr)
   {
       rc = prvUtfSwFromHwDeviceNumberGet(memberPtr->device, &(memberPtr->device));
       if (GT_OK != rc)
       {
           PRV_UTF_LOG1_MAC(
               "[TGF]: prvUtfSwFromHwDeviceNumberGet FAILED, rc = [%d]", rc);

       }
   }

   return rc;

#else
    devNum = devNum;
    trunkId = trunkId;
    enablePtr = enablePtr;
    memberPtr = memberPtr;

    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfTrunkDbIsMemberOfTrunk
*
* DESCRIPTION:
*       Function Relevant mode : High level mode
*
*       Checks if a member {device,port} is a trunk member.
*       if it is trunk member the function retrieve the trunkId of the trunk.
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number.
*       memberPtr - (pointer to) the member to check if is trunk member
*
* OUTPUTS:
*       trunkIdPtr - (pointer to) trunk id of the port .
*                    this pointer allocated by the caller.
*                    this can be NULL pointer if the caller not require the
*                    trunkId(only wanted to know that the member belongs to a
*                    trunk)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM             - bad device number
*       GT_NOT_FOUND             - the pair {devNum,port} not a trunk member
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTrunkDbIsMemberOfTrunk
(
    IN  GT_U8                   devNum,
    IN  CPSS_TRUNK_MEMBER_STC   *memberPtr,
    OUT GT_TRUNK_ID             *trunkIdPtr
)
{
#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
   GT_STATUS   rc;
   CPSS_TRUNK_MEMBER_STC localMember, *localMemberPtr;

   if( NULL != memberPtr)
   {
       localMember = *memberPtr;
       rc = prvUtfHwFromSwDeviceNumberGet(memberPtr->device, &(localMember.device));
       if (GT_OK != rc)
       {
           PRV_UTF_LOG1_MAC(
               "[TGF]: prvUtfHwFromSwDeviceNumberGet FAILED, rc = [%d]", rc);

           return rc;
       }
       localMemberPtr = &localMember;
   }
   else
       localMemberPtr = NULL;

   /* call device specific API */
#ifdef EXMXPM_FAMILY
    rc = cpssExMxPmTrunkDbIsMemberOfTrunk(devNum, localMemberPtr , trunkIdPtr);
    #else
    rc = cpssDxChTrunkDbIsMemberOfTrunk(devNum, localMemberPtr , trunkIdPtr);
#endif
    if (GT_OK != rc && rc != GT_NOT_FOUND)
    {
#ifdef EXMXPM_FAMILY
       PRV_UTF_LOG1_MAC(
           "[TGF]: cpssExMxPmTrunkDbIsMemberOfTrunk FAILED, rc = [%d]", rc);
#else
       PRV_UTF_LOG1_MAC(
           "[TGF]: cpssDxChTrunkDbIsMemberOfTrunk FAILED, rc = [%d]", rc);
#endif
    }

    return rc;

#else
    devNum = devNum;
    trunkId = trunkId;
    enablePtr = enablePtr;
    memberPtr = memberPtr;

    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfTrunkDesignatedPortsEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the designated trunk table specific entry.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum          - the device number
*       entryIndex      - the index in the designated ports bitmap table
*
* OUTPUTS:
*       designatedPortsPtr - (pointer to) designated ports bitmap
*
* RETURNS:
*       GT_OK                    - successful completion
*       GT_FAIL                  - an error occurred.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_OUT_OF_RANGE          - entryIndex exceed the number of HW table.
*                         the index must be in range (0 - 7)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTrunkDesignatedPortsEntryGet
(
    IN  GT_U8               devNum,
    IN  GT_U32              entryIndex,
    OUT  CPSS_PORTS_BMP_STC  *designatedPortsPtr
)
{
#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
   GT_STATUS   rc;

   /* call device specific API */
#ifdef EXMXPM_FAMILY
   rc = cpssExMxPmTrunkDesignatedPortsEntryGet(devNum, entryIndex , designatedPortsPtr);
#else
   rc = cpssDxChTrunkDesignatedPortsEntryGet(devNum, entryIndex , designatedPortsPtr);
#endif
   if (GT_OK != rc)
   {
#ifdef EXMXPM_FAMILY
       PRV_UTF_LOG1_MAC(
           "[TGF]: cpssExMxPmTrunkDesignatedPortsEntryGet FAILED, rc = [%d]", rc);
#else
       PRV_UTF_LOG1_MAC(
           "[TGF]: cpssDxChTrunkDesignatedPortsEntryGet FAILED, rc = [%d]", rc);
#endif
   }

   return rc;

#else
    devNum              = devNum;
    entryIndex          = entryIndex;
    designatedPortsPtr  = designatedPortsPtr;
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfTrunkMemberAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function add member to the trunk in the device.
*       If member is already in this trunk , function do nothing and
*       return GT_OK.
*
*       Notes about designated trunk table:
*       If (no designated defined)
*           re-distribute MC/Cascade trunk traffic among the enabled members,
*           now taking into account also the added member
*       else
*           1. If added member is not the designated member - set its relevant bits to 0
*           2. If added member is the designated member & it's enabled,
*              set its relevant bits on all indexes to 1.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       trunkId     - the trunk id.
*       memberPtr   - (pointer to)the member to add to the trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0xE0) != 0  means that the HW can't support
*                                              this value , since HW has 5 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST         - this member already exists in another trunk.
*       GT_FULL - trunk already contains maximum supported members
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvTgfTrunkMemberAdd
(
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
{
#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
   GT_U8       devNum  = 0;
   GT_STATUS   rc, rc1 = GT_OK;
   CPSS_TRUNK_MEMBER_STC localMember, *localMemberPtr;

   /* prepare device iterator */
   PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

   if( NULL != memberPtr)
   {
       localMember = *memberPtr;
       rc = prvUtfHwFromSwDeviceNumberGet(memberPtr->device, &(localMember.device));
       if (GT_OK != rc)
       {
           PRV_UTF_LOG1_MAC(
               "[TGF]: prvUtfHwFromSwDeviceNumberGet FAILED, rc = [%d]", rc);

           return rc;
       }
       localMemberPtr = &localMember;
   }
   else
       localMemberPtr = NULL;

   /* go over all active devices */
   while (GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
   {
       /* call device specific API */
#ifdef EXMXPM_FAMILY
       rc = cpssExMxPmTrunkMemberAdd(devNum, trunkId, localMemberPtr);
#else
       rc = cpssDxChTrunkMemberAdd(devNum, trunkId, localMemberPtr);
#endif
       if (GT_OK != rc)
       {
#ifdef EXMXPM_FAMILY
           PRV_UTF_LOG1_MAC(
               "[TGF]: cpssExMxPmTrunkMemberAdd FAILED, rc = [%d]", rc);
#else
           PRV_UTF_LOG1_MAC(
               "[TGF]: cpssDxChTrunkDesignatedMemberSet FAILED, rc = [%d]", rc);
#endif
           rc1 = rc;
       }
   }

   return rc1;

#else
    trunkId = trunkId;
    memberPtr = memberPtr;

    return GT_BAD_STATE;
#endif

}

/*******************************************************************************
* prvTgfTrunkMemberRemove
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function remove member from a trunk in the device.
*       If member not exists in this trunk , function do nothing and
*       return GT_OK.
*
*       Notes about designated trunk table:
*       If (no designated defined)
*           re-distribute MC/Cascade trunk traffic among the enabled members -
*           now taking into account also the removed member
*       else
*           1. If removed member is not the designated member - nothing to do
*           2. If removed member is the designated member set its relevant bits
*              on all indexes to 1.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       trunkId     - the trunk id.
*       memberPtr   - (pointer to)the member to remove from the trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0xE0) != 0  means that the HW can't support
*                                              this value , since HW has 5 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTrunkMemberRemove
(
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
{
#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
   GT_U8       devNum  = 0;
   GT_STATUS   rc, rc1 = GT_OK;
   CPSS_TRUNK_MEMBER_STC localMember, *localMemberPtr;

   /* prepare device iterator */
   PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

   if( NULL != memberPtr)
   {
       localMember = *memberPtr;
       rc = prvUtfHwFromSwDeviceNumberGet(memberPtr->device, &(localMember.device));
       if (GT_OK != rc)
       {
           PRV_UTF_LOG1_MAC(
               "[TGF]: prvUtfHwFromSwDeviceNumberGet FAILED, rc = [%d]", rc);

           return rc;
       }
       localMemberPtr = &localMember;
   }
   else
       localMemberPtr = NULL;

   /* go over all active devices */
   while (GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
   {
       /* call device specific API */
#ifdef EXMXPM_FAMILY
       rc = cpssExMxPmTrunkMemberRemove(devNum, trunkId, localMemberPtr);
#else
       rc = cpssDxChTrunkMemberRemove(devNum, trunkId, localMemberPtr);
#endif
       if (GT_OK != rc)
       {
#ifdef EXMXPM_FAMILY
           PRV_UTF_LOG1_MAC(
               "[TGF]: cpssExMxPmTrunkMemberRemove FAILED, rc = [%d]", rc);
#else
           PRV_UTF_LOG1_MAC(
               "[TGF]: cpssDxChTrunkMemberRemove FAILED, rc = [%d]", rc);
#endif
           rc1 = rc;
       }
   }

   return rc1;

#else
    trunkId = trunkId;
    memberPtr = memberPtr;

    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfTrunkMemberDisable
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function disable (enabled)existing member of trunk in the device.
*       If member is already disabled in this trunk , function do nothing and
*       return GT_OK.
*
*       Notes about designated trunk table:
*       If (no designated defined)
*           re-distribute MC/Cascade trunk traffic among the enabled members -
*           now taking into account also the disabled member
*       else
*           1. If disabled member is not the designated member - set its relevant bits to 0
*           2. If disabled member is the designated member set its relevant bits
*               on all indexes to 0.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       trunkId     - the trunk id.
*       memberPtr   - (pointer to)the member to disable in the trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0xE0) != 0  means that the HW can't support
*                                              this value , since HW has 5 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_FOUND - this member not found (member not exist) in the trunk
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTrunkMemberDisable
(
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
{
#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
   GT_U8       devNum  = 0;
   GT_STATUS   rc, rc1 = GT_OK;
   CPSS_TRUNK_MEMBER_STC localMember, *localMemberPtr;

   /* prepare device iterator */
   PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

   if( NULL != memberPtr)
   {
       localMember = *memberPtr;
       rc = prvUtfHwFromSwDeviceNumberGet(memberPtr->device, &(localMember.device));
       if (GT_OK != rc)
       {
           PRV_UTF_LOG1_MAC(
               "[TGF]: prvUtfHwFromSwDeviceNumberGet FAILED, rc = [%d]", rc);

           return rc;
       }
       localMemberPtr = &localMember;
   }
   else
       localMemberPtr = NULL;

   /* go over all active devices */
   while (GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
   {
       /* call device specific API */
#ifdef EXMXPM_FAMILY
       rc = cpssExMxPmTrunkMemberDisable(devNum, trunkId, localMemberPtr);
#else
       rc = cpssDxChTrunkMemberDisable(devNum, trunkId, localMemberPtr);
#endif
       if (GT_OK != rc)
       {
#ifdef EXMXPM_FAMILY
           PRV_UTF_LOG1_MAC(
               "[TGF]: cpssExMxPmTrunkMemberDisable FAILED, rc = [%d]", rc);
#else
           PRV_UTF_LOG1_MAC(
               "[TGF]: cpssDxChTrunkMemberDisable FAILED, rc = [%d]", rc);
#endif
           rc1 = rc;
       }
   }

   return rc1;

#else
    trunkId = trunkId;
    memberPtr = memberPtr;

    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfTrunkMemberEnable
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function enable (disabled)existing member of trunk in the device.
*       If member is already enabled in this trunk , function do nothing and
*       return GT_OK.
*
*       Notes about designated trunk table:
*       If (no designated defined)
*           re-distribute MC/Cascade trunk traffic among the enabled members -
*           now taking into account also the enabled member
*       else
*           1. If enabled member is not the designated member - set its relevant bits to 0
*           2. If enabled member is the designated member set its relevant bits
*               on all indexes to 1.
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       trunkId     - the trunk id.
*       memberPtr   - (pointer to)the member to enable in the trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0xE0) != 0  means that the HW can't support
*                                              this value , since HW has 5 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_FOUND - this member not found (member not exist) in the trunk
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTrunkMemberEnable
(
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
{
#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
   GT_U8       devNum  = 0;
   GT_STATUS   rc, rc1 = GT_OK;
   CPSS_TRUNK_MEMBER_STC localMember, *localMemberPtr;

   /* prepare device iterator */
   PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

   if( NULL != memberPtr)
   {
       localMember = *memberPtr;
       rc = prvUtfHwFromSwDeviceNumberGet(memberPtr->device, &(localMember.device));
       if (GT_OK != rc)
       {
           PRV_UTF_LOG1_MAC(
               "[TGF]: prvUtfHwFromSwDeviceNumberGet FAILED, rc = [%d]", rc);

           return rc;
       }
       localMemberPtr = &localMember;
   }
   else
       localMemberPtr = NULL;

   /* go over all active devices */
   while (GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
   {
       /* call device specific API */
#ifdef EXMXPM_FAMILY
       rc = cpssExMxPmTrunkMemberEnable(devNum, trunkId, localMemberPtr);
#else
       rc = cpssDxChTrunkMemberEnable(devNum, trunkId, localMemberPtr);
#endif
       if (GT_OK != rc)
       {
#ifdef EXMXPM_FAMILY
           PRV_UTF_LOG1_MAC(
               "[TGF]: cpssExMxPmTrunkMemberEnable FAILED, rc = [%d]", rc);
#else
           PRV_UTF_LOG1_MAC(
               "[TGF]: cpssDxChTrunkMemberEnable FAILED, rc = [%d]", rc);
#endif
           rc1 = rc;
       }
   }

   return rc1;

#else
    trunkId = trunkId;
    memberPtr = memberPtr;

    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfTrunkCascadeTrunkPortsSet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function sets the 'cascade' trunk with the specified 'Local ports'
*       overriding any previous setting.
*       The cascade trunk may be invalidated/unset by portsMembersPtr = NULL.
*       Local ports are ports of only configured device.
*       Cascade trunk is:
*           - members are ports of only configured device pointed by devNum
*           - trunk members table is empty (see cpssDxChTrunkTableEntrySet)
*             Therefore it cannot be used as target by ingress engines like FDB,
*             Router, TTI, Ingress PCL and so on.
*           - members ports trunk ID is not set (see cpssDxChTrunkPortTrunkIdSet).
*             Therefore packets those ingresses in member ports are not associated with trunk
*           - all members are enabled only and cannot be disabled.
*           - may be used for cascade traffic and pointed by the 'Device map table'
*             as the local target to reach to the 'Remote device'.
*             (For 'Device map table' refer to cpssDxChCscdDevMapTableSet(...))
*
*
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*
*       devNum      - device number
*       trunkId     - trunk id
*       portsMembersPtr - (pointer to) local ports bitmap to be members of the
*                   cascade trunk.
*                   NULL - meaning that the trunk-id is 'invalidated' and
*                          trunk-type will be changed to : CPSS_TRUNK_TYPE_FREE_E
*                   not-NULL - meaning that the trunk-type will be : CPSS_TRUNK_TYPE_CASCADE_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_FAIL            - on error.
*       GT_NOT_INITIALIZED - the trunk library was not initialized for the device
*       GT_HW_ERROR        - on hardware error
*       GT_OUT_OF_RANGE    - there are ports in the bitmap that not supported by
*                            the device.
*       GT_BAD_PARAM       - bad device number , or bad trunkId number , or number
*                            of ports (in the bitmap) larger then the number of
*                            entries in the 'Designated trunk table'
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_CASCADE_E
*       GT_ALREADY_EXIST   - one of the members already exists in another trunk ,
*                            or this trunk hold members defined using cpssDxChTrunkMembersSet(...)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*   1. This function does not set ports are 'Cascade ports' (and also not
*       check that ports are 'cascade').
*   2. This function sets only next tables :
*       a. the designated trunk table:
*          distribute MC/Cascade trunk traffic among the members
*       b. the 'Non-trunk' table entry.
*   3. because this function not set the 'Trunk members' table entry , the
*       application should not point to this trunk from any ingress unit , like:
*       FDB , PCL action redirect , NH , TTI action redirect , PVE ...
*       (it should be pointed ONLY from the device map table)
*   4. Application can manipulate the 'Per port' trunk-id , for those ports ,
*       using the 'Low level API' of : cpssDxChTrunkPortTrunkIdSet(...)
*   5. this API supports only trunks with types : CPSS_TRUNK_TYPE_FREE_E or
*       CPSS_TRUNK_TYPE_CASCADE_E.
*   6. next APIs are not supported from trunk with type : CPSS_TRUNK_TYPE_CASCADE_E
*       cpssDxChTrunkMembersSet ,
*       cpssDxChTrunkMemberAdd , cpssDxChTrunkMemberRemove,
*       cpssDxChTrunkMemberEnable , cpssDxChTrunkMemberDisable
*       cpssDxChTrunkDbEnabledMembersGet , cpssDxChTrunkDbDisabledMembersGet
*       cpssDxChTrunkDesignatedMemberSet , cpssDxChTrunkDbDesignatedMemberGet
*
********************************************************************************
*   Comparing the 2 function :
*
*        cpssDxChTrunkCascadeTrunkPortsSet   |   cpssDxChTrunkMembersSet
*   ----------------------------------------------------------------------------
*   1. purpose 'Cascade trunk'               | 1. purpose 'Network trunk' , and
*                                            |    also 'Cascade trunk' with up to
*                                            |    8 members
*   ----------------------------------------------------------------------------
*   2. supported number of members depends   | 2. supports up to 8 members
*      on number of entries in the           |    (also in Lion).
*      'Designated trunk table'              |
*      -- Lion supports 64 entries (so up to |
*         64 ports in the 'Cascade trunk').  |
*      -- all other devices supports 8       |
*         entries (so up to 8 ports in the   |
*         'Cascade trunk').                  |
*   ----------------------------------------------------------------------------
*   3. manipulate only 'Non-trunk' table and | 3. manipulate all trunk tables :
*      'Designated trunk' table              |  'Per port' trunk-id , 'Trunk members',
*                                            |  'Non-trunk' , 'Designated trunk' tables.
*   ----------------------------------------------------------------------------
*   4. ingress unit must not point to this   | 4. no restriction on ingress/egress
*      trunk (because 'Trunk members' entry  |    units.
*       hold no ports)                       |
*   ----------------------------------------------------------------------------
*   5. not associated with trunk Id on       | 5. for cascade trunk : since 'Per port'
*      ingress                               | the trunk-Id is set , then load balance
*                                            | according to 'Local port' for traffic
*                                            | that ingress cascade trunk and
*                                            | egress next cascade trunk , will
*                                            | egress only from one of the egress
*                                            | trunk ports. (because all ports associated
*                                            | with the trunk-id)
*   ----------------------------------------------------------------------------
*
*******************************************************************************/
GT_STATUS prvTgfTrunkCascadeTrunkPortsSet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_PORTS_BMP_STC       *portsMembersPtr
)
{
#if (defined CHX_FAMILY)
       /* call device specific API */
    return cpssDxChTrunkCascadeTrunkPortsSet(devNum, trunkId, portsMembersPtr);
#else
    devNum = devNum;
    trunkId = trunkId;
    portsMembersPtr = portsMembersPtr;

    return GT_BAD_STATE;
#endif
}


/*******************************************************************************
* prvTgfTrunkHashCrcParametersSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the CRC hash parameter , relevant for CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_CRC_E .
*
* APPLICABLE DEVICES:   Lion and above devices.
*
* INPUTS:
*       crcMode - The CRC mode .
*       crcSeed - The seed used by the CRC computation .
*                 when crcMode is CRC_6 mode : crcSeed is in range of 0..0x3f (6 bits value)
*                 when crcMode is CRC_16 mode : crcSeed is in range of 0..0xffff (16 bits value)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or crcMode
*       GT_OUT_OF_RANGE          - crcSeed out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       related to feature 'CRC hash mode'
*******************************************************************************/
GT_STATUS prvTgfTrunkHashCrcParametersSet
(
    IN PRV_TGF_TRUNK_LBH_CRC_MODE_ENT     crcMode,
    IN GT_U32                               crcSeed
)
{
#if (defined CHX_FAMILY)
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;
    CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT dxCrcMode;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    switch(crcMode)
    {
        case PRV_TGF_TRUNK_LBH_CRC_6_MODE_E:
           dxCrcMode = CPSS_DXCH_TRUNK_LBH_CRC_6_MODE_E ;
           break;
        case PRV_TGF_TRUNK_LBH_CRC_16_AND_PEARSON_MODE_E:
           dxCrcMode = CPSS_DXCH_TRUNK_LBH_CRC_16_AND_PEARSON_MODE_E ;
           break;
        default:
            return GT_BAD_PARAM;
    }

    /* go over all active devices */
    while (GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChTrunkHashCrcParametersSet(devNum, dxCrcMode, crcSeed);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC(
               "[TGF]: cpssDxChTrunkHashCrcParametersSet FAILED, rc = [%d]", rc);
            rc1 = rc;
        }
    }

    return rc1;

#else
    crcMode = crcMode;
    crcSeed = crcSeed;

    return GT_BAD_STATE;
#endif
}


/*******************************************************************************
* prvTgfTrunkPortHashMaskInfoSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set port-based hash mask info.
*
* APPLICABLE DEVICES:   Lion and above devices.
*
* INPUTS:
*       devNum  - The device number.
*       portNum - The port number.
*       overrideEnable - enable/disable the override
*       index - the index to use when 'Override enabled'.
*               (values 0..15) , relevant only when overrideEnable = GT_TRUE
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or portNum
*       GT_OUT_OF_RANGE          - when overrideEnable is enabled and index out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       related to feature 'CRC hash mode'
*******************************************************************************/
GT_STATUS prvTgfTrunkPortHashMaskInfoSet
(
    IN GT_U8     devNum,
    IN GT_U8     portNum,
    IN GT_BOOL   overrideEnable,
    IN GT_U32    index
)
{
#if (defined CHX_FAMILY)
       /* call device specific API */
    return cpssDxChTrunkPortHashMaskInfoSet(devNum, portNum, overrideEnable, index);
#else
    devNum = devNum;
    portNum = portNum;
    overrideEnable = overrideEnable;
    index = index;

    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfTrunkHashMaskCrcEntrySet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the entry of masks in the specified index in 'CRC hash mask table'.
*
* APPLICABLE DEVICES:   Lion and above devices.
*
* INPUTS:
*       index   - the table index (0..27)
*       entryPtr - (pointer to) The entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or index
*       GT_OUT_OF_RANGE          - one of the fields in entryPtr are out of range
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       related to feature 'CRC hash mode'
*******************************************************************************/
GT_STATUS prvTgfTrunkHashMaskCrcEntrySet
(
    IN GT_U32                         index,
    IN PRV_TGF_TRUNK_LBH_CRC_MASK_ENTRY_STC *entryPtr
)
{
#if (defined CHX_FAMILY)
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;
    CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC   dxChEntry;

    if(entryPtr == NULL)
    {
        return GT_BAD_PARAM;
    }

    dxChEntry.l4DstPortMaskBmp        =     entryPtr->l4DstPortMaskBmp;
    dxChEntry.l4SrcPortMaskBmp        =     entryPtr->l4SrcPortMaskBmp;
    dxChEntry.ipv6FlowMaskBmp         =     entryPtr->ipv6FlowMaskBmp;
    dxChEntry.ipDipMaskBmp            =     entryPtr->ipDipMaskBmp;
    dxChEntry.ipSipMaskBmp            =     entryPtr->ipSipMaskBmp;
    dxChEntry.macDaMaskBmp            =     entryPtr->macDaMaskBmp;
    dxChEntry.macSaMaskBmp            =     entryPtr->macSaMaskBmp;
    dxChEntry.mplsLabel0MaskBmp       =     entryPtr->mplsLabel0MaskBmp;
    dxChEntry.mplsLabel1MaskBmp       =     entryPtr->mplsLabel1MaskBmp;
    dxChEntry.mplsLabel2MaskBmp       =     entryPtr->mplsLabel2MaskBmp;
    dxChEntry.localSrcPortMaskBmp     =     entryPtr->localSrcPortMaskBmp;
    dxChEntry.udbsMaskBmp             =     entryPtr->udbsMaskBmp;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all active devices */
    while (GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChTrunkHashMaskCrcEntrySet(devNum, index, &dxChEntry);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC(
               "[TGF]: cpssDxChTrunkHashMaskCrcEntrySet FAILED, rc = [%d]", rc);
            rc1 = rc;
        }
    }

    return rc1;

#else
    index = index;
    entryPtr = entryPtr;

    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfTrunkHashPearsonValueSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the Pearson hash value for the specific index.
*
*       NOTE: the Pearson hash used when CRC-16 mode is used.
*
* APPLICABLE DEVICES:   Lion and above devices.
*
* INPUTS:
*       index   - the table index (0..63)
*       value   - the Pearson hash value (0..63)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or index
*       GT_OUT_OF_RANGE          - value > 63
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       related to feature 'CRC hash mode'
*******************************************************************************/
GT_STATUS prvTgfTrunkHashPearsonValueSet
(
    IN GT_U32                         index,
    IN GT_U32                         value
)
{
#if (defined CHX_FAMILY)
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all active devices */
    while (GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChTrunkHashPearsonValueSet(devNum, index, value);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC(
               "[TGF]: cpssDxChTrunkHashPearsonValueSet FAILED, rc = [%d]", rc);
            rc1 = rc;
        }
    }

    return rc1;

#else
    index = index;
    value = value;

    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfTrunkHashDesignatedTableModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the hash mode of the designated trunk table .
*
* APPLICABLE DEVICES:   All Devices
*
* INPUTS:
*       mode - The designated table hashing mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number or mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTrunkHashDesignatedTableModeSet
(
    IN PRV_TGF_TRUNK_DESIGNATED_TABLE_MODE_ENT    mode
)
{
#if (defined CHX_FAMILY)
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;
    CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT   dxMode;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    switch(mode)
    {
        case PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_FIRST_INDEX_E   :
            dxMode = CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_FIRST_INDEX_E;
            break;
        case PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_E  :
            dxMode = CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_E;
            break;
        case PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_AND_VID_E:
            dxMode = CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_AND_VID_E;
            break;
        case PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_SOURCE_INFO_E   :
            dxMode = CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_SOURCE_INFO_E;
            break;
        default:
            return GT_BAD_PARAM;
    }


    /* go over all active devices */
    while (GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChTrunkHashDesignatedTableModeSet(devNum, dxMode);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC(
               "[TGF]: cpssDxChTrunkHashDesignatedTableModeSet FAILED, rc = [%d]", rc);
            rc1 = rc;
        }
    }

    return rc1;

#else
    mode = mode;

    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfTrunkMcLocalSwitchingEnableSet
*
*       Function Relevant mode : High Level mode
*
* DESCRIPTION:
*       Enable/Disable sending multi-destination packets back to its source
*       trunk on the local device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number .
*       trunkId     - the trunk id.
*       enable      - Boolean value:
*                     GT_TRUE  - multi-destination packets may be sent back to
*                                their source trunk on the local device.
*                     GT_FALSE - multi-destination packets are not sent back to
*                                their source trunk on the local device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or bad trunkId number
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. the behavior of multi-destination traffic ingress from trunk is
*       not-affected by setting of cpssDxChBrgVlanLocalSwitchingEnableSet
*       and not-affected by setting of cpssDxChBrgPortEgressMcastLocalEnable
*       2. the functionality manipulates the 'non-trunk' table entry of the trunkId
*
*******************************************************************************/
GT_STATUS prvTgfTrunkMcLocalSwitchingEnableSet
(
    IN GT_TRUNK_ID      trunkId,
    IN GT_BOOL          enable
)
{
#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
   GT_U8       devNum  = 0;
   GT_STATUS   rc, rc1 = GT_OK;

   /* prepare device iterator */
   PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

   /* go over all active devices */
   while (GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
   {
       /* call device specific API */
#ifdef EXMXPM_FAMILY
       rc = cpssExMxPmTrunkMcLocalSwitchingEnableSet(devNum, trunkId, enable);
#else
       rc = cpssDxChTrunkMcLocalSwitchingEnableSet(devNum, trunkId, enable);
#endif
       if (GT_OK != rc)
       {
#ifdef EXMXPM_FAMILY
           PRV_UTF_LOG1_MAC(
               "[TGF]: cpssExMxPmTrunkMcLocalSwitchingEnableSet FAILED, rc = [%d]", rc);
#else
           PRV_UTF_LOG1_MAC(
               "[TGF]: cpssDxChTrunkMcLocalSwitchingEnableSet FAILED, rc = [%d]", rc);
#endif
           rc1 = rc;
       }
   }

   return rc1;

#else
    trunkId = trunkId;
    memberPtr = memberPtr;

    return GT_BAD_STATE;
#endif
}

