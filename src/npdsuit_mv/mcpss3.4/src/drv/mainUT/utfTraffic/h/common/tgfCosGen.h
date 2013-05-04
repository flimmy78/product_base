/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfCosGen.h
*
* DESCRIPTION:
*       Generic API for Cos Technology facility API.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __tgfCosGenh
#define __tgfCosGenh

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cos/cpssCosTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/bridge/cpssGenBrgVlanTypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* default UP profile index to use */
#define PRV_TGF_COS_UP_PROFILE_INDEX_DEFAULT_CNS    0

/******************************************************************************\
 *                          Private type definitions                          *
\******************************************************************************/

/*
 * Typedef: PRV_TGF_COS_PROFILE_STC
 *
 * Description: The Prestera Class-of-Service profile parameters
 *
 * Fields:
 *    dropPrecedence  - drop precedence (color) (0..2)
 *    userPriority    - IEEEE 802.1p User Priority (0..7)
 *    trafficClass    - egress tx traffic class queue (0..7)
 *    dscp            - ip DCSP field (0..63)
 *    exp             - MPLS label exp value (0..7)
 */
typedef struct
{
    CPSS_DP_LEVEL_ENT   dropPrecedence;
    GT_U32              userPriority;
    GT_U32              trafficClass;
    GT_U32              dscp;
    GT_U32              exp;
} PRV_TGF_COS_PROFILE_STC;


/******************************************************************************\
 *                       CPSS generic API section                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfCosExpToProfileMapSet
*
* DESCRIPTION:
*       Maps the packet MPLS exp to a QoS Profile
*
* INPUTS:
*       exp          - mpls exp
*       profileIndex - profile index
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong dev, exp or profileIndex
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvTgfCosExpToProfileMapSet
(
    IN  GT_U32                        exp,
    IN  GT_U32                        profileIndex
);

/*******************************************************************************
* prvTgfCosExpToProfileMapGet
*
* DESCRIPTION:
*       Returns the mapping of packet MPLS exp to a QoS Profile
*
* INPUTS:
*       exp - mpls exp
*
* OUTPUTS:
*       profileIndexPtr - (pointer to) profile index
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong dev, exp or profileIndex
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfCosExpToProfileMapGet
(
    IN  GT_U32                        exp,
    OUT GT_U32                       *profileIndexPtr
);

/*******************************************************************************
* prvTgfCosDscpToProfileMapSet
*
* DESCRIPTION:
*       Maps the packet DSCP (or remapped DSCP) to a QoS Profile
*
* INPUTS:
*       dscp         - DSCP of a IP packet
*       profileIndex - profile index
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong dev, dscp or profileIndex
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvTgfCosDscpToProfileMapSet
(
    IN  GT_U8                         dscp,
    IN  GT_U32                        profileIndex
);

/*******************************************************************************
* prvTgfCosDscpToProfileMapGet
*
* DESCRIPTION:
*       Get the Mapping: packet DSCP (or remapped DSCP) to a QoS Profile
*
* INPUTS:
*       dscp - DSCP of a IP packet
*
* OUTPUTS:
*       profileIndexPtr - (pointer to) profile index
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong dev, dscp or profileIndex
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfCosDscpToProfileMapGet
(
    IN  GT_U8                         dscp,
    OUT GT_U32                       *profileIndexPtr
);

/*******************************************************************************
* prvTgfCosPortQosConfigSet
*
* DESCRIPTION:
*       Configures the port's QoS attributes.
*
* APPLICABLE DEVICES:  All DxCh Devices.
*
* INPUTS:
*       port            - logical port number, CPU port.
*       portQosCfgPtr  - Pointer to QoS related configuration of a port.
*                         portQosCfgPtr->qosProfileId -
*                                                   [0..71] for Ch,
*                                                   [0..127] for Ch2 and above;
*                         portQosCfgPtr->enableModifyDscp and
*                         portQosCfgPtr->enableModifyUp
*                         only [CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E,
*                              CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E]
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong dev, port or portQosCfgPtr
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfCosPortQosConfigSet
(
    IN GT_U8                port,
    IN CPSS_QOS_ENTRY_STC  *portQosCfgPtr
);


/*******************************************************************************
* prvTgfCosPortQosTrustModeSet
*
* DESCRIPTION:
*       Configures port's Trust Mode.
*
* APPLICABLE DEVICES:  All DxCh Devices.
*
* INPUTS:
*       port                - logical port number, CPU port.
*       portQosTrustMode    - QoS trust mode of a port.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong dev, port or trustMode.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfCosPortQosTrustModeSet
(
    IN GT_U8                         port,
    IN CPSS_QOS_PORT_TRUST_MODE_ENT  portQosTrustMode
);

/*******************************************************************************
* prvTgfCosProfileEntrySet
*
* DESCRIPTION:
*       Configures the Profile Table Entry and
*       Initial Drop Precedence (DP) for Policer.
*
* INPUTS:
*       profileIndex  - index of a profile in the profile table
*                       Ch values (0..71); Ch2 and above values (0..127).
*       cosPtr        - Pointer to new CoS values for packet
*                       (dp = CPSS_DP_YELLOW_E is not applicable).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum or cos.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For DxCh3 devices Initial DP supports 3 levels (Yellow, Red, Green),
*       but QoS profile entry supports only 2 levels by the following way:
*        - The function set value 0 for Green DP
*        - The function set value 1 for both Yellow and Red DPs.
*
*******************************************************************************/
GT_STATUS prvTgfCosProfileEntrySet
(
     IN  GT_U8                      profileIndex,
     IN  PRV_TGF_COS_PROFILE_STC    *cosPtr
);

/*******************************************************************************
* prvTgfCosTrustExpModeSet
*
* DESCRIPTION:
*       Set Enable/Disable state of trust EXP mode for MPLS packets.
*
* INPUTS:
*       port       - port number, CPU port.
*       enable     - trust the MPLS header's EXP field.
*                    GT_FALSE - QoS profile from Exp To Qos Profile table
*                               not assigned to the MPLS packet.
*                    GT_TRUE  - QoS profile from Exp To Qos Profile table
*                               assigned to the MPLS packet.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum or port.
*       GT_TIMEOUT               - after max number of retries checking if PP ready.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfCosTrustExpModeSet
(
    IN  GT_U8      port,
    OUT GT_BOOL    enable
);

/*******************************************************************************
* prvTgfCosUpCfiDeiToProfileMapSet
*
* DESCRIPTION:
*       Maps the UP Profile Index and packet's User Priority and CFI/DEI bit (Canonical Format
*       Indicator/Drop Eligibility Indicator) to QoS Profile.
*       The mapping relevant for L2 and L2-L3 QoS Trust modes for 802.1Q
*       or DSA tagged packets.
*
* INPUTS:
*       upProfileIndex - the UP profile index (table selector)
*                        this parameter not relevant for ch1,2,3,xcat.
*                       see also API : cpssDxChCosPortUpProfileIndexSet
*       up              - User Priority of a VLAN or DSA tagged packet [0..7].
*       cfiDeiBit       - value of CFI/DEI bit[0..1].
*                         this parameter not relevant for ch1,2.
*       profileIndex    - QoS Profile index, which is assigned to a VLAN or DSA
*                         tagged packet with the UP on ports with enabled trust
*                         L2 or trust L2-L3. Range is [0..127].(for ch1 range is [0..71])
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum, up or cfiDeiBit.
*       GT_OUT_OF_RANGE - on profileIndex out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfCosUpCfiDeiToProfileMapSet
(
    IN GT_U32   upProfileIndex,
    IN GT_U8    up,
    IN GT_U8    cfiDeiBit,
    IN GT_U32   profileIndex
);

/*******************************************************************************
* prvTgfCosDpToCfiDeiMapSet
*
* DESCRIPTION:
*       Maps Drop Precedence to Drop Eligibility Indicator bit.
*
* INPUTS:
*       dp        - Drop Precedence [Green, Red].
*       cfiDeiBit - value of CFI/DEI bit[0..1].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or dp.
*       GT_OUT_OF_RANGE - on cfiDeiBit out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Three DP levels are mapped to only two values of CFI/DEI bit.
*
*******************************************************************************/
GT_STATUS prvTgfCosDpToCfiDeiMapSet
(
    IN CPSS_DP_LEVEL_ENT    dp,
    IN GT_U8                cfiDeiBit
);

/*******************************************************************************
* prvTgfCosL2TrustModeVlanTagSelectSet
*
* DESCRIPTION:
*       Select type of VLAN tag (either Tag0 or Tag1) for Trust L2 Qos mode.
*
* INPUTS:
*       port            - port number, CPU port.
*       vlanTagType     - Vlan Tag Type (tag0 or tag1)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum or port or vlanTagType.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfCosL2TrustModeVlanTagSelectSet
(
    IN GT_U8                     port,
    IN CPSS_VLAN_TAG_TYPE_ENT    vlanTagType
);


/*******************************************************************************
* prvTgfCosPortUpProfileIndexSet
*
* DESCRIPTION:
*       set per ingress port and per UP (user priority) the 'UP profile index' (table selector).
*       See also API prvTgfCosUpCfiDeiToProfileMapSet
*
* APPLICABLE DEVICES:   Lion and above.
*
* INPUTS:
*       devNum          - device number.
*       portNum         - ingress port number including CPU port.
*       up              - user priority of a VLAN or DSA tagged packet [0..7].
*       upProfileIndex  - the UP profile index (table selector) , value 0..1
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - wrong devNum, portNum or up
*       GT_OUT_OF_RANGE    - upProfileIndex  > 1
*       GT_HW_ERROR        - on HW error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfCosPortUpProfileIndexSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_U8    up,
    IN GT_U32   upProfileIndex
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __tgfCosGenh */

