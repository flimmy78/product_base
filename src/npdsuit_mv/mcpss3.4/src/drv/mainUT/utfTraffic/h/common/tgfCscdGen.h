/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfCscdGen.h
*
* DESCRIPTION:
*       Generic API for CSCD
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __tgfCscdGenh
#define __tgfCscdGenh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cscd/cpssGenCscd.h>
/******************************************************************************\
 *                          Private type definitions                          *
\******************************************************************************/

/*
 * typedef: PRV_TGF_CSCD_TRUNK_LINK_HASH_ENT
 *
 * Description: type of the load balancing trunk hash for packets forwarded
 *              via trunk cascade link
 *              Enabled to set the load balancing trunk hash for packets
 *              forwarded via an trunk uplink to be based on the packet’s source
 *              port, and not on the packets data.
 *
 * Enumerations:
 *       PRV_TGF_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E -
 *              Load balancing trunk hash is based on the ingress pipe
 *              hash mode as configured by function
 *              cpssDxChTrunkHashModeSet(...)
 *       PRV_TGF_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E - Load balancing trunk
 *              hash for this cascade trunk interface is based on the packet’s
 *              source port, regardless of the ingress pipe hash mode.
 *       PRV_TGF_CSCD_TRUNK_LINK_HASH_IS_DST_PORT_E - Load balancing trunk
 *              hash for this cascade trunk interface is based on the packet’s
 *              destination port, regardless of the ingress pipe hash mode.
 *              Relevant only for Lion and above.
*/

typedef enum
{
    PRV_TGF_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E,
    PRV_TGF_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E,
    PRV_TGF_CSCD_TRUNK_LINK_HASH_IS_DST_PORT_E
} PRV_TGF_CSCD_TRUNK_LINK_HASH_ENT;

/*
 * typedef: enum PRV_TGF_DEV_MAP_LOOKUP_MODE_ENT
 *
 * Description: Device Map lookup mode.
 *              The mode defines index calculation that is used to
 *              access Device Map table.
 *
 * Enumerations:
 *      PRV_TGF_DEV_MAP_LOOKUP_MODE_TRG_DEV_E - Target Device
 *                                       (bits [4:0]) is used as index.
 *      PRV_TGF_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E -
 *            Both Target Device and Target Port are used to get index.
 *            Index bits representation is Target Device[4:0], Target Port[5:0].
 */
typedef enum
{
     PRV_TGF_DEV_MAP_LOOKUP_MODE_TRG_DEV_E,
     PRV_TGF_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E
}PRV_TGF_DEV_MAP_LOOKUP_MODE_ENT;


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
);

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
);

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
);

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
);

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
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __tgfCscdGenh */

