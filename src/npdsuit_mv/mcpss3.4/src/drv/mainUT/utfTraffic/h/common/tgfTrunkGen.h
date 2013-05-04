/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfTrunkGen.h
*
* DESCRIPTION:
*       Generic API for Trunk
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __tgfTrunkGenh
#define __tgfTrunkGenh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/trunk/cpssDxChTrunk.h>
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    #include <cpss/exMxPm/exMxPmGen/trunk/cpssExMxPmTrunk.h>
#endif /* EXMXPM_FAMILY */

/******************************************************************************\
 *                          Private type definitions                          *
\******************************************************************************/

/*
 * typedef: enum PRV_TGF_TRUNK_LBH_GLOBAL_MODE_ENT
 *
 * Description: Enumeration of general LBH (load balance hash) Mode.
 *              the hashing will be according to use of bits from packet, or
 *              according to the ingress port number.
 *
 * Enumerations:
 *    PRV_TGF_TRUNK_LBH_PACKETS_INFO_E - Hash is based on the packets data.
 *    PRV_TGF_TRUNK_LBH_INGRESS_PORT_E - Hash is based on the packet's
 *                                       Source Port# or Source Trunk#.
 *                                       On Cascading ports, the Source Port#
 *                                       or Source Trunk# is assigned from the
 *                                       DSA tag.
 *    PRV_TGF_TRUNK_LBH_PERFECT_E      - Hash index is cyclic on the trunk members
 *                                       (per trunk).
 *                                       (Hash index is reset to 0 when number of
 *                                       members change)
 *                                       This assure a "perfect" load balance
 *                                       distribution between trunk members.
 *                                       NOTE:
 *                                       Meaning that if there are 3 members in
 *                                       the trunk ,
 *                                       -then first packet destined to this trunk
 *                                       will egress from first port in trunk.
 *                                       -the second packet destined to this trunk
 *                                       will egress from second port in trunk.
 *                                       -the third packet destined to this trunk
 *                                       will egress from third port in trunk.
 *                                       -the forth packet destined to this trunk
 *                                       will forth from first port in trunk.
 *                                       -the fifth packet destined to this trunk
 *                                       will forth from second port in trunk.
 *                                       ...
 *                                       -the (X) packet destined to this trunk
 *                                       will forth from (X%Y) port in trunk.
 *                                 where X - is the packet number
 *                                       Y - is number of ports in trunk
 *                                       relevant for EXMXPM devices only.
 *    PRV_TGF_TRUNK_LBH_PACKETS_INFO_CRC_E - CRC Hash based on the packet's data.
 *                                       Note :
 *                                       1. Applicable for Lion and above.
 *                                       2. Using this value will return GT_BAD_PARAM
 *                                          for devices that not support the feature.
 */
typedef enum
{
    PRV_TGF_TRUNK_LBH_PACKETS_INFO_E,
    PRV_TGF_TRUNK_LBH_INGRESS_PORT_E,
    PRV_TGF_TRUNK_LBH_PERFECT_E,
    PRV_TGF_TRUNK_LBH_PACKETS_INFO_CRC_E
} PRV_TGF_TRUNK_LBH_GLOBAL_MODE_ENT;

/*
 * typedef: enum PRV_TGF_TRUNK_LBH_MASK_ENT
 *
 * Description: Enumeration of trunk LBH (load balance hash) relate fields to
 *              apply mask .
 *
 * Enumerations:
 *    PRV_TGF_TRUNK_LBH_MASK_MAC_DA_E         - mask for MAC DA
 *    PRV_TGF_TRUNK_LBH_MASK_MAC_SA_E         - mask for MAC SA
 *    PRV_TGF_TRUNK_LBH_MASK_MPLS_LABEL0_E    - mask for MPLS label 0
 *    PRV_TGF_TRUNK_LBH_MASK_MPLS_LABEL1_E    - mask for MPLS label 1
 *    PRV_TGF_TRUNK_LBH_MASK_MPLS_LABEL2_E    - mask for MPLS label 2
 *    PRV_TGF_TRUNK_LBH_MASK_IPV4_DIP_E       - mask for IPv4 DIP
 *    PRV_TGF_TRUNK_LBH_MASK_IPV4_SIP_E       - mask for IPv4 SIP
 *    PRV_TGF_TRUNK_LBH_MASK_IPV6_DIP_E       - mask for IPv6 DIP
 *    PRV_TGF_TRUNK_LBH_MASK_IPV6_SIP_E       - mask for IPv6 SIP
 *    PRV_TGF_TRUNK_LBH_MASK_IPV6_FLOW_E      - mask for IPv6 flow
 *    PRV_TGF_TRUNK_LBH_MASK_L4_DST_PORT_E    - mask for L4 DST port
 *    PRV_TGF_TRUNK_LBH_MASK_L4_SRC_PORT_E    - mask for L4 SRC port
 */
typedef enum {
    PRV_TGF_TRUNK_LBH_MASK_MAC_DA_E,
    PRV_TGF_TRUNK_LBH_MASK_MAC_SA_E,
    PRV_TGF_TRUNK_LBH_MASK_MPLS_LABEL0_E,
    PRV_TGF_TRUNK_LBH_MASK_MPLS_LABEL1_E,
    PRV_TGF_TRUNK_LBH_MASK_MPLS_LABEL2_E,
    PRV_TGF_TRUNK_LBH_MASK_IPV4_DIP_E,
    PRV_TGF_TRUNK_LBH_MASK_IPV4_SIP_E,
    PRV_TGF_TRUNK_LBH_MASK_IPV6_DIP_E,
    PRV_TGF_TRUNK_LBH_MASK_IPV6_SIP_E,
    PRV_TGF_TRUNK_LBH_MASK_IPV6_FLOW_E,
    PRV_TGF_TRUNK_LBH_MASK_L4_DST_PORT_E,
    PRV_TGF_TRUNK_LBH_MASK_L4_SRC_PORT_E
} PRV_TGF_TRUNK_LBH_MASK_ENT;

/*
 * typedef: enum PRV_TGF_TRUNK_LBH_CRC_MODE_ENT
 *
 *      Description: enumerator for CRC LBH (load balance hash) mode.
 *
 * Enumerations:
 *      PRV_TGF_TRUNK_LBH_CRC_6_MODE_E  - CRC use 'CRC 6'  ,
 *                                      polynomial: x^6+ x + 1 .
 *      PRV_TGF_TRUNK_LBH_CRC_16_AND_PEARSON_MODE_E - CRC use 'CRC 16' and Pearson hash ,
 *                                      polynomial: x^16 + x^15 + x^2 + 1 .
 *
 *
 */
typedef enum {
    PRV_TGF_TRUNK_LBH_CRC_6_MODE_E = 6,/* make it != 0 --> for loop with (mode--) purpose */
    PRV_TGF_TRUNK_LBH_CRC_16_AND_PEARSON_MODE_E
}PRV_TGF_TRUNK_LBH_CRC_MODE_ENT;

/*
 * typedef: structure PRV_TGF_TRUNK_LBH_CRC_MASK_ENTRY_STC
 *
 * Description: structure of entry of 'CRC hash mask' table .
 *      === related to feature 'CRC hash mode' ===
 *
 *  in those fields each bit in the bitmap represents a BYTE to be added to hash.
 *
 *  for fields from the packet : the bitmap represent in bit 0 the MSB and in
 *      last bit the LSB (network order bytes)
 *  for UDBs : the bmp represent the UDBs (bit 14 --> UDB 14 , bit 22 UDB 22)
 *
 *  NOTE: the actual write to the HW will be in 'Little Endian' order (which is
 *         reverse to the 'Network order' for fields from the packet)
 *
 *  For L4 dst port the bmp is of 2 bits , because 2 bytes in L4 port
 *      using bit 0 to represent MSB , bit 1 the LSB (network order bytes)
 *  For mpls label 0 (and mpls label 1,mpls label 2) the bmp of 3 bits (for 3 bytes
 *      from label - actually 2.5 MSB)
 *      using bit 0 to represent MSB , bit 1 the second byte ,bit 2 the third byte (LSB) (network order bytes)
 *  For IP_SIP the bmp is of 16 bits , because 128 bits (16 bytes) in ipv6
 *      using bit 0 to represent IP[0] , bit 1 represent IP[1] ..., bit 15 represent IP[15] (network order bytes)
 *      the bits for ipv4 addresses (4 bits) located in bits 12..15 (MSB..LSB) (network order bytes)
 *  For UDBS the bmp is of 32 bits , because 32 UDBs
 *      using bit 0 to represent UDB 0 , bit 1 represent UDB 1 ..., bit 31 represent UDB 31
 *              Lion supports only UDBs 14..22 (bits 14..22 in the bitmap)
 *  For Local source port the bmp is of single bit (bit 0)
 *
 * fields:
 *    l4DstPortMaskBmp  - L4 DST port mask bmp  ( 2 bits for  2 bytes-'Network order')
 *    l4SrcPortMaskBmp  - L4 SRC port mask bmp  ( 2 bits for  2 bytes-'Network order')
 *    ipv6FlowMaskBmp   - IPv6 flow   mask bmp  ( 3 bits for  3 bytes-'Network order')
 *    ipDipMaskBmp      - IPv6/v4 DIP mask bmp  (16 bits for 16 bytes-'Network order')
 *    ipSipMaskBmp      - IPv6/v4 SIP mask bmp  (16 bits for 16 bytes-'Network order')
 *    macDaMaskBmp      - MAC DA      mask bmp  ( 6 bits for  6 bytes-'Network order')
 *    macSaMaskBmp      - MAC SA      mask bmp  ( 6 bits for  6 bytes-'Network order')
 *    mplsLabel0MaskBmp - MPLS label 0 mask bmp ( 3 bits for  3 bytes-'Network order')
 *    mplsLabel1MaskBmp - MPLS label 1 mask bmp ( 3 bits for  3 bytes-'Network order')
 *    mplsLabel2MaskBmp - MPLS label 2 mask bmp ( 3 bits for  3 bytes-'Network order')
 *    localSrcPortMaskBmp - local source port mask bmp (1 bit for 1 byte)
 *    udbsMaskBmp         - UDBs mask bmp (user defined bytes) (32 bits for 32 UDBs)
 *                          lion supports only UDBs 14..22
 *
 */
typedef struct {
    GT_U32      l4DstPortMaskBmp;
    GT_U32      l4SrcPortMaskBmp;
    GT_U32      ipv6FlowMaskBmp;
    GT_U32      ipDipMaskBmp;
    GT_U32      ipSipMaskBmp;
    GT_U32      macDaMaskBmp;
    GT_U32      macSaMaskBmp;
    GT_U32      mplsLabel0MaskBmp;
    GT_U32      mplsLabel1MaskBmp;
    GT_U32      mplsLabel2MaskBmp;
    GT_U32      localSrcPortMaskBmp;
    GT_U32      udbsMaskBmp;
}PRV_TGF_TRUNK_LBH_CRC_MASK_ENTRY_STC ;

/*
 * typedef: enum PRV_TGF_TRUNK_DESIGNATED_TABLE_MODE_ENT
 *
 * Description: enumeration for how to access the designated trunk table.
 *
 * Enumerations:
 *    PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_FIRST_INDEX_E - force the use of only
 *                              index 0 in the table.(impact multi-destination
 *                              traffic as well as known UC to cascade trunk).
 *    PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_E - use the computed
 *                              trunk hash that was calculated in ingress.
 *    PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_AND_VID_E - use the computed
 *                              trunk hash that was calculated in ingress , and
 *                              XOR it with the vid.
 *    PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_SOURCE_INFO_E - The index is based on
 *                              the original source port/trunk, VID and VIDX.
 *                              If the original source interface is a trunks,
 *                              the 6 LSb of the trunk-id are used.
 */
typedef enum {
    PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_FIRST_INDEX_E  = GT_FALSE,
    PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_E = GT_TRUE,
    PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_AND_VID_E,
    PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_SOURCE_INFO_E
}PRV_TGF_TRUNK_DESIGNATED_TABLE_MODE_ENT;


/* number of entries in the Pearson table */
#define PRV_TGF_TRUNK_PEARSON_TABLE_SIZE_CNS  64

/* number of entries in the hash mask table */
#define PRV_TGF_TRUNK_HASH_MASK_TABLE_SIZE_CNS  28
/* the start index of 'Traffic type' entries in the hash mask table */
#define PRV_TGF_TRUNK_HASH_MASK_TABLE_TRAFFIC_TYPE_FIRST_INDEX_CNS  16
/* indication to clear the table of mask hash */
#define PRV_TGF_TRUNK_HASH_MASK_TABLE_INDEX_CLEAR_ALL_CNS  0xFFFFFFFF

/* type of fields into trunk hash */
typedef enum{
    PRV_TGF_TRUNK_FIELD_L4_PORT_E,   /*src/dst*/
    PRV_TGF_TRUNK_FIELD_IPV6_FLOW_E,
    PRV_TGF_TRUNK_FIELD_IP_ADDR_E,   /*src/dst*/
    PRV_TGF_TRUNK_FIELD_MAC_ADDR_E,  /*src/dst*/
    PRV_TGF_TRUNK_FIELD_MPLS_LABEL_E,/*0/1/2*/
    PRV_TGF_TRUNK_FIELD_LOCAL_SRC_PORT_E,
    PRV_TGF_TRUNK_FIELD_UDBS_E,        /*0..31 (lion 14..22) */

    PRV_TGF_TRUNK_FIELD_LAST_E
}PRV_TGF_TRUNK_FIELD_TYPE_ENT;

/* direction : source/destination of fields in PRV_TGF_TRUNK_FIELD_TYPE_ENT */
typedef enum{
    PRV_TGF_TRUNK_FIELD_DIRECTION_SOURCE_E,
    PRV_TGF_TRUNK_FIELD_DIRECTION_DESTINATION_E
}PRV_TGF_TRUNK_FIELD_DIRECTION_ENT;

/* get string for CRC mode */
#define TO_STRING_CRC_MODE_MAC(_crcMode)    \
        (                               \
        _crcMode ==    PRV_TGF_TRUNK_LBH_CRC_6_MODE_E               ? "CRC_6"                :\
        _crcMode ==    PRV_TGF_TRUNK_LBH_CRC_16_AND_PEARSON_MODE_E  ? "CRC_16_AND_PEARSON"   :\
                                                                      "???")

/* get string for hash mask sub field type */
#define TO_STRING_HASH_MASK_FIELD_SUB_TYPE_MAC(_hashMaskFieldSubType)    \
        (                                                        \
        _hashMaskFieldSubType == PRV_TGF_TRUNK_FIELD_DIRECTION_SOURCE_E      ? "source"      : \
        _hashMaskFieldSubType == PRV_TGF_TRUNK_FIELD_DIRECTION_DESTINATION_E ? "destination" : \
                                                                             "???")


/* get string for hash mask field type */
#define TO_STRING_HASH_MASK_FIELD_TYPE_MAC(_hashMaskFieldType)    \
        (                                                         \
        _hashMaskFieldType == PRV_TGF_TRUNK_FIELD_L4_PORT_E           ? "L4_PORT"         : \
        _hashMaskFieldType == PRV_TGF_TRUNK_FIELD_IPV6_FLOW_E          ? "IPV6_FLOW"      : \
        _hashMaskFieldType == PRV_TGF_TRUNK_FIELD_IP_ADDR_E            ? "IP_ADDR"        : \
        _hashMaskFieldType == PRV_TGF_TRUNK_FIELD_MAC_ADDR_E           ? "MAC_ADDR"       : \
        _hashMaskFieldType == PRV_TGF_TRUNK_FIELD_MPLS_LABEL_E         ? "MPLS_LABEL"     : \
        _hashMaskFieldType == PRV_TGF_TRUNK_FIELD_LOCAL_SRC_PORT_E     ? "LOCAL_SRC_PORT" : \
        _hashMaskFieldType == PRV_TGF_TRUNK_FIELD_UDBS_E               ? "UDBS"           : \
                                                                              "???")
/* get string for designated table mode */
#define TO_STRING_DESIGNATED_TABLE_MODE_MAC(_desigMode)    \
        (                                                         \
        _desigMode == PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_FIRST_INDEX_E           ? "USE_FIRST_INDEX_E"        : \
        _desigMode == PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_E          ? "USE_INGRESS_HASH"         : \
        _desigMode == PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_AND_VID_E  ? "USE_INGRESS_HASH_AND_VID" : \
        _desigMode == PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_SOURCE_INFO_E           ? "USE_SOURCE_INFO"          : \
                                                                              "???")

/* get string for LBH mode */
#define TO_STRING_LBH_MODE_MAC(_lbhMode)    \
        (                                                         \
        _lbhMode == PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_SINGLE_PORT_E          ? "LOAD_BALANCE_MODE_SINGLE_PORT"           : \
        _lbhMode == PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_EVEN_E                 ? "LOAD_BALANCE_MODE_EVEN"                  : \
        _lbhMode == PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_EMPTY_E                ? "LOAD_BALANCE_MODE_EMPTY"                 : \
        _lbhMode == PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_ONLY_SPECIFIC_MEMBERS_E? "LOAD_BALANCE_MODE_ONLY_SPECIFIC_MEMBERS" : \
        _lbhMode == PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_LIMITED_MEMBERS_E      ? "LOAD_BALANCE_MODE_LIMITED_MEMBERS"       : \
                                                                              "???")

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
);

/*******************************************************************************
* prvTgfTrunkHashGlobalModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the general hashing mode of trunk hash generation.
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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);


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
);

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
);

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
*       devNum      - the device number on which to remove member from the trunk
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
);


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
);

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
);

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
);


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
);

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
);

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
);

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
);

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
);


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
);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __tgfTrunkGenh */

