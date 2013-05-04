/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfCncGen.h
*
* DESCRIPTION:
*       Generic API for CNC Technology facility API.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#ifndef __tgfCncGenh
#define __tgfCncGenh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* get public types */
#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <cpss/generic/cpssTypes.h>

/*
 * typedef: PRV_TGF_CNC_CLIENT_ENT
 *
 * Description:
 *      CNC clients.
 * Enumerations:
 *   PRV_TGF_CNC_CLIENT_L2L3_INGRESS_VLAN_E       - L2/L3 Ingress VLAN
 *   PRV_TGF_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_E    - Ingress PCL0 lookup 0
 *   PRV_TGF_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_1_E  - Ingress PCL0 lookup 1
 *                                                    (APPLICABLE DEVICES:
 *                                                     DxChXcat and above)
 *   PRV_TGF_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_E    - Ingress PCL1 lookup
 *   PRV_TGF_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E  - Ingress VLAN Pass/Drop
 *   PRV_TGF_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E   - Egress VLAN Pass/Drop
 *   PRV_TGF_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E  - Egress Queue Pass/Drop
 *             Note: For Lion and above devices
 *                   the CN messages may be counted instead
 *                   of Egress Queue Pass/Drop, if CN mode enabled.
 *   PRV_TGF_CNC_CLIENT_EGRESS_PCL_E              - Egress PCL
 *   PRV_TGF_CNC_CLIENT_ARP_TABLE_ACCESS_E        - ARP Table access
 *   PRV_TGF_CNC_CLIENT_TUNNEL_START_E            - Tunnel Start
 *   PRV_TGF_CNC_CLIENT_TTI_E                     - TTI
 *   PRV_TGF_CNC_CLIENT_LAST_E                    - amount of enum members
 *
 * Comment:
 */
typedef enum
{
    PRV_TGF_CNC_CLIENT_L2L3_INGRESS_VLAN_E,
    PRV_TGF_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_E,
    PRV_TGF_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_1_E,
    PRV_TGF_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_E,
    PRV_TGF_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E,
    PRV_TGF_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E,
    PRV_TGF_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E,
    PRV_TGF_CNC_CLIENT_EGRESS_PCL_E,
    PRV_TGF_CNC_CLIENT_ARP_TABLE_ACCESS_E,
    PRV_TGF_CNC_CLIENT_TUNNEL_START_E,
    PRV_TGF_CNC_CLIENT_TTI_E,
    PRV_TGF_CNC_CLIENT_LAST_E
} PRV_TGF_CNC_CLIENT_ENT;

/*
 * typedef: PRV_TGF_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT
 *
 * Description:
 *      CNC modes of Egress Queue counting.
 *
 * Enumerations:
 *   PRV_TGF_CNC_EGRESS_QUEUE_CLIENT_MODE_TAIL_DROP_E -
 *         count egress queue pass and tail-dropped packets.
 *   PRV_TGF_CNC_EGRESS_QUEUE_CLIENT_MODE_CN_E        -
 *         count Congestion Notification messages.
 *
 * Comment:
 */
typedef enum
{
    PRV_TGF_CNC_EGRESS_QUEUE_CLIENT_MODE_TAIL_DROP_E,
    PRV_TGF_CNC_EGRESS_QUEUE_CLIENT_MODE_CN_E
} PRV_TGF_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT;

/*
 * typedef: PRV_TGF_CNC_BYTE_COUNT_MODE_ENT
 *
 * Description:
 *      CNC modes for byte count counters.
 *
 * Enumerations:
 *   PRV_TGF_CNC_BYTE_COUNT_MODE_L2_E - The Byte Count counter counts the
 *                 entire packet byte count for all packet type
 *   PRV_TGF_CNC_BYTE_COUNT_MODE_L3_E - Byte Count counters counts the
 *                 packet L3 fields (the entire packet
 *                 minus the L3 offset) and only
 *                 the passenger part for tunnel-terminated
 *                 packets or tunnel-start packets.
 *
 * Comment:
 */
typedef enum
{
    PRV_TGF_CNC_BYTE_COUNT_MODE_L2_E,
    PRV_TGF_CNC_BYTE_COUNT_MODE_L3_E
} PRV_TGF_CNC_BYTE_COUNT_MODE_ENT;

/*
 * typedef: PRV_TGF_CNC_EGRESS_DROP_COUNT_MODE_ENT
 *
 * Description:
 *      CNC modes Egress VLAN Drop counting.
 *
 * Enumerations:
 *      PRV_TGF_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_AND_TAIL_DROP_E -
 *          Drop-counter counts egress-filtered and tail-dropped traffic
 *      PRV_TGF_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_ONLY_E          -
 *          Drop-counter counts egress filtered traffic only
 *      PRV_TGF_CNC_EGRESS_DROP_COUNT_MODE_TAIL_DROP_ONLY_E              -
 *          Drop-counter counts tail-drop only
 *
 * Comment:
 */
typedef enum
{
    PRV_TGF_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_AND_TAIL_DROP_E,
    PRV_TGF_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_ONLY_E,
    PRV_TGF_CNC_EGRESS_DROP_COUNT_MODE_TAIL_DROP_ONLY_E
} PRV_TGF_CNC_EGRESS_DROP_COUNT_MODE_ENT;

/*
 * typedef: enum PRV_TGF_CNC_COUNTING_ENABLE_UNIT_ENT
 *
 * Description:
 *      CNC counting enabled unit
 *
 * Enumerations:
 *      PRV_TGF_CNC_COUNTING_ENABLE_UNIT_TTI_E - enable counting for TTI
 *                                                 client.
 *      PRV_TGF_CNC_COUNTING_ENABLE_UNIT_PCL_E - enable counting for extended
 *                                                 PCL unit (this unit includes
 *                                                 PCL0_0, PCL0_1, PCL1 & L2/L3
 *                                                 Ingress VLAN clients).
 *
 * Comment:
 *      Other clients are implicitly enabled for counting and binding to
 *      CNC block operation is needed to perform counting.
 */
typedef enum
{
    PRV_TGF_CNC_COUNTING_ENABLE_UNIT_TTI_E,
    PRV_TGF_CNC_COUNTING_ENABLE_UNIT_PCL_E
} PRV_TGF_CNC_COUNTING_ENABLE_UNIT_ENT;


/*
 * typedef: PRV_TGF_CNC_COUNTER_FORMAT_ENT
 *
 * Description:
 *      CNC modes of counter formats.
 *
 * Enumerations:
 *   PRV_TGF_CNC_COUNTER_FORMAT_MODE_0_E - PKT_29_BC_35;
 *         Partitioning of the 64 bits entry is as following:
 *         Packets counter: 29 bits, Byte Count counter: 35 bits
 *   PRV_TGF_CNC_COUNTER_FORMAT_MODE_1_E - PKT_27_BC_37;
 *         Partitioning of the 64 bits entry is as following:
 *         Packets counter: 27 bits, Byte Count counter: 37 bits
 *   PRV_TGF_CNC_COUNTER_FORMAT_MODE_2_E - PKT_37_BC_27;
 *         Partitioning of the 64 bits entry is as following:
 *         Packets counter: 37 bits, Byte Count counter: 27 bits
 *
 * Comment:
 */
typedef enum
{
    PRV_TGF_CNC_COUNTER_FORMAT_MODE_0_E,
    PRV_TGF_CNC_COUNTER_FORMAT_MODE_1_E,
    PRV_TGF_CNC_COUNTER_FORMAT_MODE_2_E
} PRV_TGF_CNC_COUNTER_FORMAT_ENT;

/*
 * typedef: struct PRV_TGF_CNC_COUNTER_STC
 *
 * Description:
 *      The counter entry contents.
 *
 * Fields:
 *      byteCount      - byte count
 *      packetCount    - packets count
 *
 * Comment:
 *      See the possible counter HW formats of Lion and above devices in
 *      PRV_TGF_CNC_COUNTER_FORMAT_ENT description.
 *      For DxCh3 and DxChXcat devices the byte counter has 35 bits,
 *      the packets counter has 29 bits
 */
typedef struct
{
    GT_U64     byteCount;
    GT_U64     packetCount;
} PRV_TGF_CNC_COUNTER_STC;

/*******************************************************************************
* prvTgfCncBlockClientEnableSet
*
* DESCRIPTION:
*   The function binds/unbinds the selected client to/from a counter block.
*
* INPUTS:
*       blockNum       - CNC block number
*                        valid range see in datasheet of specific device.
*       client         - CNC client
*       updateEnable   - the client enable to update the block
*                        GT_TRUE - enable, GT_FALSE - disable
*                        It is forbidden to enable update the same
*                        block by more then one client. When an
*                        application enables some client it is responsible
*                        to disable all other clients it enabled before
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncBlockClientEnableSet
(
    IN  GT_U32                    blockNum,
    IN  PRV_TGF_CNC_CLIENT_ENT    client,
    IN  GT_BOOL                   updateEnable
);

/*******************************************************************************
* prvTgfCncBlockClientEnableGet
*
* DESCRIPTION:
*   The function gets bind/unbind of the selected client to a counter block.
*
* INPUTS:
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*       client         -  CNC client
*
* OUTPUTS:
*       updateEnablePtr - (pointer to) the client enable to update the block
*                         GT_TRUE - enable, GT_FALSE - disable
*                         It is forbidden to enable update the same
*                         block by more then one client. When an
*                         application enables some client it is responsible
*                         to disable all other clients it enabled before
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncBlockClientEnableGet
(
    IN  GT_U32                    blockNum,
    IN  PRV_TGF_CNC_CLIENT_ENT    client,
    OUT GT_BOOL                   *updateEnablePtr
);

/*******************************************************************************
* prvTgfCncBlockClientRangesSet
*
* DESCRIPTION:
*   The function sets index ranges per CNC client and Block
*
* INPUTS:
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       client           - CNC client
*       indexRangesBmp   - the counter index ranges bitmap
*                         DxCh3 and DxChXcat devices have 8 ranges of counter indexes,
*                         each 2048 indexes (0..(2K-1), 2K..(4K-1), ..., 14K..(16K-1))
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         The Lion devices has 64 ranges (512 indexes each).
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on not relevant bits in indexRangesBmp
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncBlockClientRangesSet
(
    IN  GT_U32                      blockNum,
    IN  PRV_TGF_CNC_CLIENT_ENT      client,
    IN  GT_U64                      indexRangesBmp
);

/*******************************************************************************
* prvTgfCncBlockClientRangesGet
*
* DESCRIPTION:
*   The function gets index ranges per CNC client and Block
*
* INPUTS:
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       client           - CNC client
*
* OUTPUTS:
*       indexRangesBmpPtr - (pointer to) the counter index ranges bitmap
*                         DxCh3 and DxChXcat devices have 8 ranges of counter indexes,
*                         each 2048 indexes (0..(2K-1), 2K..(4K-1), ..., 14K..(16K-1))
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         The Lion devices has 64 ranges (512 indexes each).
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncBlockClientRangesGet
(
    IN  GT_U32                    blockNum,
    IN  PRV_TGF_CNC_CLIENT_ENT    client,
    OUT GT_U64                    *indexRangesBmpPtr
);

/*******************************************************************************
* prvTgfCncPortClientEnableSet
*
* DESCRIPTION:
*   The function sets the given client Enable counting per port.
*   Currently only L2/L3 Ingress Vlan client supported.
*
* INPUTS:
*       portIndex        - port index in UTF array
*       client           - CNC client
*       enable           - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncPortClientEnableSet
(
    IN  GT_U8                     portIndex,
    IN  PRV_TGF_CNC_CLIENT_ENT    client,
    IN  GT_BOOL                   enable
);

/*******************************************************************************
* prvTgfCncPortClientEnableGet
*
* DESCRIPTION:
*   The function gets the given client Enable counting per port.
*   Currently only L2/L3 Ingress Vlan client supported.
*
* INPUTS:
*       portIndex        - port index in UTF array
*       client           - CNC client
*
* OUTPUTS:
*       enablePtr        - (pointer to) enable
*                          GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncPortClientEnableGet
(
    IN  GT_U8                     portIndex,
    IN  PRV_TGF_CNC_CLIENT_ENT    client,
    OUT GT_BOOL                   *enablePtr
);

/*******************************************************************************
* prvTgfCncIngressVlanPassDropFromCpuCountEnableSet
*
* DESCRIPTION:
*   The function enables or disables counting of FROM_CPU packets
*   for the Ingress Vlan Pass/Drop CNC client.
*
* INPUTS:
*       enable           - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncIngressVlanPassDropFromCpuCountEnableSet
(
    IN  GT_BOOL enable
);

/*******************************************************************************
* prvTgfCncIngressVlanPassDropFromCpuCountEnableGet
*
* DESCRIPTION:
*   The function gets status of counting of FROM_CPU packets
*   for the Ingress Vlan Pass/Drop CNC client.
*
* INPUTS:
*
* OUTPUTS:
*       enablePtr        - (pointer to) enable
*                          GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncIngressVlanPassDropFromCpuCountEnableGet
(
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* prvTgfCncClientByteCountModeSet
*
* DESCRIPTION:
*   The function sets byte count counter mode for CNC client.
*
* INPUTS:
*       client           - CNC client
*       countMode        - count mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncClientByteCountModeSet
(
    IN  PRV_TGF_CNC_CLIENT_ENT            client,
    IN  PRV_TGF_CNC_BYTE_COUNT_MODE_ENT   countMode
);

/*******************************************************************************
* prvTgfCncClientByteCountModeGet
*
* DESCRIPTION:
*   The function gets byte count counter mode for CNC client.
*
* INPUTS:
*       client           - CNC client
*
* OUTPUTS:
*       countModePtr     - (pointer to) count mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncClientByteCountModeGet
(
    IN  PRV_TGF_CNC_CLIENT_ENT            client,
    OUT PRV_TGF_CNC_BYTE_COUNT_MODE_ENT   *countModePtr
);

/*******************************************************************************
* prvTgfCncEgressVlanDropCountModeSet
*
* DESCRIPTION:
*   The function sets Egress VLAN Drop counting mode.
*
* INPUTS:
*       mode             - Egress VLAN Drop counting mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncEgressVlanDropCountModeSet
(
    IN  PRV_TGF_CNC_EGRESS_DROP_COUNT_MODE_ENT   mode
);

/*******************************************************************************
* prvTgfCncEgressVlanDropCountModeGet
*
* DESCRIPTION:
*   The function gets Egress VLAN Drop counting mode.
*
* INPUTS:
*
* OUTPUTS:
*       modePtr             - (pointer to) Egress VLAN Drop counting mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - on reserved value found in HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncEgressVlanDropCountModeGet
(
    OUT PRV_TGF_CNC_EGRESS_DROP_COUNT_MODE_ENT   *modePtr
);

/*******************************************************************************
* prvTgfCncCounterClearByReadEnableSet
*
* DESCRIPTION:
*   The function enable/disables clear by read mode of CNC counters read
*   operation.
*   If clear by read mode is disable the counters after read
*   keep the current value and continue to count normally.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
* INPUTS:
*       enable       - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCounterClearByReadEnableSet
(
    IN  GT_BOOL  enable
);

/*******************************************************************************
* prvTgfCncCounterClearByReadEnableGet
*
* DESCRIPTION:
*   The function gets clear by read mode status of CNC counters read
*   operation.
*   If clear by read mode is disable the counters after read
*   keep the current value and continue to count normally.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
* INPUTS:
*
* OUTPUTS:
*       enablePtr    - (pointer to) enable
*                      GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCounterClearByReadEnableGet
(
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* prvTgfCncCounterClearByReadValueSet
*
* DESCRIPTION:
*   The function sets the counter clear by read globally configured value.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
* INPUTS:
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*       counterPtr       - (pointer to) counter contents
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCounterClearByReadValueSet
(
    IN  PRV_TGF_CNC_COUNTER_FORMAT_ENT    format,
    IN  PRV_TGF_CNC_COUNTER_STC           *counterPtr
);

/*******************************************************************************
* prvTgfCncCounterClearByReadValueGet
*
* DESCRIPTION:
*   The function gets the counter clear by read globally configured value.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
* INPUTS:
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*
* OUTPUTS:
*       counterPtr       - (pointer to) counter contents
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCounterClearByReadValueGet
(
    IN  PRV_TGF_CNC_COUNTER_FORMAT_ENT    format,
    OUT PRV_TGF_CNC_COUNTER_STC           *counterPtr
);

/*******************************************************************************
* prvTgfCncCounterWraparoundEnableSet
*
* DESCRIPTION:
*   The function enables/disables wraparound for all CNC counters
*
* INPUTS:
*       enable       - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCounterWraparoundEnableSet
(
    IN  GT_BOOL  enable
);

/*******************************************************************************
* prvTgfCncCounterWraparoundEnableGet
*
* DESCRIPTION:
*   The function gets status of wraparound for all CNC counters
*
* INPUTS:
*
* OUTPUTS:
*       enablePtr    - (pointer to) enable
*                      GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCounterWraparoundEnableGet
(
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* prvTgfCncCounterWraparoundIndexesGet
*
* DESCRIPTION:
*   The function gets the counter Wrap Around last 8 indexes
*
* INPUTS:
*       blockNum     - CNC block number
*                      valid range see in datasheet of specific device.
*       indexNumPtr  - (pointer to) maximal size of array of indexes
*
* OUTPUTS:
*       indexNumPtr  - (pointer to) actual size of array of indexes
*       indexesArr[] - (pointer to) array of indexes of counters wrapped around
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The wraparound indexes are cleared on read.
*       Since in the HW each pair of wraparound indexes resides in one register,
*       cleared by read, in case not all the 8 possible indexes are read at once,
*       some data may be lost.
*       Recommendation: Always request 8 indexes => *indexNumPtr=8 as input.
*
*******************************************************************************/
GT_STATUS prvTgfCncCounterWraparoundIndexesGet
(
    IN    GT_U32   blockNum,
    INOUT GT_U32   *indexNumPtr,
    OUT   GT_U32   indexesArr[]
);

/*******************************************************************************
* prvTgfCncCounterSet
*
* DESCRIPTION:
*   The function sets the counter contents
*
* INPUTS:
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       index            - counter index in the block
*                          valid range see in datasheet of specific device.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*       counterPtr       - (pointer to) counter contents
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCounterSet
(
    IN  GT_U32                            blockNum,
    IN  GT_U32                            index,
    IN  PRV_TGF_CNC_COUNTER_FORMAT_ENT    format,
    IN  PRV_TGF_CNC_COUNTER_STC           *counterPtr
);

/*******************************************************************************
* prvTgfCncCounterGet
*
* DESCRIPTION:
*   The function gets the counter contents
*
* INPUTS:
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       index            - counter index in the block
*                          valid range see in datasheet of specific device.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*
* OUTPUTS:
*       counterPtr       - (pointer to) received CNC counter value.
*                          For multi port group devices
*                          the result counter contains the sum of
*                          counters read from all port groups.
*                          It contains more bits than in HW.
*                          For example the sum of 4 35-bit values may be
*                          37-bit value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCounterGet
(
    IN  GT_U32                            blockNum,
    IN  GT_U32                            index,
    IN  PRV_TGF_CNC_COUNTER_FORMAT_ENT    format,
    OUT PRV_TGF_CNC_COUNTER_STC           *counterPtr
);

/*******************************************************************************
* prvTgfCncBlockUploadTrigger
*
* DESCRIPTION:
*   The function triggers the Upload of the given counters block.
*   The function checks that there is no unfinished CNC and FDB upload (FU).
*   Also the function checks that all FU messages of previous FU were retrieved
*   by cpssDxChBrgFdbFuMsgBlockGet.
*   An application may check that CNC upload finished by
*   prvTgfCncBlockUploadInProcessGet.
*   An application may sequentially upload several CNC blocks before start
*   to retrieve uploaded counters.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE   - if the previous CNC upload or FU in process or
*                        FU is finished but all FU messages were not
*                        retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by prvTgfCncUploadedBlockGet.
*
*******************************************************************************/
GT_STATUS prvTgfCncBlockUploadTrigger
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      blockNum
);

/*******************************************************************************
* prvTgfCncBlockUploadInProcessGet
*
* DESCRIPTION:
*   The function gets bitmap of numbers of such counters blocks that upload
*   of them yet in process. The HW cannot keep more then one block in such
*   state, but an API matches the original HW representation of the state.
*
* INPUTS:
*       devNum                  - device number
*
* OUTPUTS:
*       inProcessBlocksBmpPtr    - (pointer to) bitmap of in-process blocks
*                                   value 1 of the bit#n means that CNC upload
*                                   is not finished yet for block n
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncBlockUploadInProcessGet
(
    IN  GT_U8                       devNum,
    OUT GT_U32                      *inProcessBlocksBmpPtr
);

/*******************************************************************************
* prvTgfCncUploadedBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of CNC counter values,
*       the maximal number of elements defined by the caller.
*       The CNC upload may triggered by prvTgfCncBlockUploadTrigger.
*       The CNC upload transfers whole CNC block (2K CNC counters)
*       to FDB Upload queue. An application must get all transferred counters.
*       An application may sequentially upload several CNC blocks before start
*       to retrieve uploaded counters.
*       The device may transfer only part of CNC block because of FUQ full. In
*       this case the prvTgfCncUploadedBlockGet may return only part of the
*       CNC block with return GT_OK. An application must to call
*       prvTgfCncUploadedBlockGet one more time to get rest of the block.
*
* INPUTS:
*       devNum                - device number
*       numOfCounterValuesPtr - (pointer to) maximal number of CNC counters
*                               values to get.This is the size of
*                               counterValuesPtr array allocated by caller.
*       format                - CNC counter HW format,
*                               relevant only for Lion and above
*
* OUTPUTS:
*       numOfCounterValuesPtr - (pointer to) actual number of CNC
*                               counters values in counterValuesPtr.
*       counterValuesPtr      - (pointer to) array that holds received CNC
*                               counters values. Array must be allocated by
*                               caller. For multi port group devices
*                               the result counters contain the sum of
*                               counters read from all port groups.
*                               It contains more bits than in HW.
*                               For example the sum of 4 35-bit values may be
*                               37-bit value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE       - the action succeeded and there are no more waiting
*                          CNC counter value
*       GT_FAIL                  - on failure
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - if the previous FU messages were not
*                          retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by prvTgfCncUploadedBlockGet.
*
*******************************************************************************/
GT_STATUS prvTgfCncUploadedBlockGet
(
    IN  GT_U8                                devNum,
    INOUT  GT_U32                            *numOfCounterValuesPtr,
    IN     PRV_TGF_CNC_COUNTER_FORMAT_ENT    format,
    OUT    PRV_TGF_CNC_COUNTER_STC           *counterValuesPtr
);

/*******************************************************************************
* prvTgfCncCountingEnableSet
*
* DESCRIPTION:
*   The function enables counting on selected cnc unit.
*
* INPUTS:
*       cncUnit - selected unit for enable\disable counting
*       enable  - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCountingEnableSet
(
    IN  PRV_TGF_CNC_COUNTING_ENABLE_UNIT_ENT    cncUnit,
    IN  GT_BOOL                                 enable
);

/*******************************************************************************
* prvTgfCncCountingEnableGet
*
* DESCRIPTION:
*   The function gets enable counting status on selected cnc unit.
*
* INPUTS:
*       cncUnit - selected unit for enable\disable counting
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable
*                     GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCountingEnableGet
(
    IN  PRV_TGF_CNC_COUNTING_ENABLE_UNIT_ENT    cncUnit,
    OUT GT_BOOL                                 *enablePtr
);

/*******************************************************************************
* prvTgfCncCounterFormatSet
*
* DESCRIPTION:
*   The function sets format of CNC counter
*
* INPUTS:
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*       format          - CNC counter format
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCounterFormatSet
(
    IN  GT_U32                            blockNum,
    IN  PRV_TGF_CNC_COUNTER_FORMAT_ENT    format
);

/*******************************************************************************
* prvTgfCncCounterFormatGet
*
* DESCRIPTION:
*   The function gets format of CNC counter
*
* INPUTS:
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*
* OUTPUTS:
*       formatPtr       - (pointer to) CNC counter format
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*                                  on not supported client for device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCounterFormatGet
(
    IN  GT_U32                            blockNum,
    OUT PRV_TGF_CNC_COUNTER_FORMAT_ENT    *formatPtr
);

/*******************************************************************************
* prvTgfCncEgressQueueClientModeSet
*
* DESCRIPTION:
*   The function sets Egress Queue client counting mode
*
* INPUTS:
*       mode            - Egress Queue client counting mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncEgressQueueClientModeSet
(
    IN  PRV_TGF_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT   mode
);

/*******************************************************************************
* prvTgfCncEgressQueueClientModeGet
*
* DESCRIPTION:
*   The function gets Egress Queue client counting mode
*
* INPUTS:
*
* OUTPUTS:
*       modePtr         - (pointer to) Egress Queue client counting mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncEgressQueueClientModeGet
(
    OUT PRV_TGF_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT   *modePtr
);

/*******************************************************************************
* prvTgfCncCpuAccessStrictPriorityEnableSet
*
* DESCRIPTION:
*   The function enables strict priority of CPU access to counter blocks
*   Disable: Others clients have strict priority
*   Enable:  CPU has strict priority
*
* INPUTS:
*       enable          - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCpuAccessStrictPriorityEnableSet
(
    IN  GT_BOOL   enable
);

/*******************************************************************************
* prvTgfCncCpuAccessStrictPriorityEnableGet
*
* DESCRIPTION:
*   The function gets enable status of strict priority of
*   CPU access to counter blocks
*   Disable: Others clients have strict priority
*   Enable:  CPU has strict priority
*
* INPUTS:
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncCpuAccessStrictPriorityEnableGet
(
    OUT GT_BOOL   *enablePtr
);

/*******************************************************************************
* prvTgfCncPortGroupBlockClientEnableSet
*
* DESCRIPTION:
*   The function binds/unbinds the selected client to/from a counter block.
*
* INPUTS:
*       portGroupsBmp  - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum       - CNC block number
*                        valid range see in datasheet of specific device.
*       client         - CNC client
*       updateEnable   - the client enable to update the block
*                        GT_TRUE - enable, GT_FALSE - disable
*                        It is forbidden to enable update the same
*                        block by more then one client. When an
*                        application enables some client it is responsible
*                        to disable all other clients it enabled before
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncPortGroupBlockClientEnableSet
(
    IN  GT_PORT_GROUPS_BMP        portGroupsBmp,
    IN  GT_U32                    blockNum,
    IN  PRV_TGF_CNC_CLIENT_ENT    client,
    IN  GT_BOOL                   updateEnable
);

/*******************************************************************************
* prvTgfCncPortGroupBlockClientEnableGet
*
* DESCRIPTION:
*   The function gets bind/unbind of the selected client to a counter block.
*
* INPUTS:
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       blockNum       - CNC block number
*                        valid range see in datasheet of specific device.
*       client         -  CNC client
*
* OUTPUTS:
*       updateEnablePtr - (pointer to) the client enable to update the block
*                         GT_TRUE - enable, GT_FALSE - disable
*                         It is forbidden to enable update the same
*                         block by more then one client. When an
*                         application enables some client it is responsible
*                         to disable all other clients it enabled before
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncPortGroupBlockClientEnableGet
(
    IN  GT_PORT_GROUPS_BMP        portGroupsBmp,
    IN  GT_U32                    blockNum,
    IN  PRV_TGF_CNC_CLIENT_ENT    client,
    OUT GT_BOOL                   *updateEnablePtr
);

/*******************************************************************************
* prvTgfCncPortGroupBlockClientRangesSet
*
* DESCRIPTION:
*   The function sets index ranges per CNC client and Block
*
* INPUTS:
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       client           - CNC client
*       indexRangesBmp   - bitmap of counter index ranges
*                         DxCh3 and DxChXcat devices have 8 ranges of counter indexes,
*                         each 2048 indexes (0..(2K-1), 2K..(4K-1), ..., 14K..(16K-1))
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         The Lion devices has 64 ranges (512 indexes each).
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on not relevant bits in indexRangesBmp
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncPortGroupBlockClientRangesSet
(
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  GT_U32                      blockNum,
    IN  PRV_TGF_CNC_CLIENT_ENT      client,
    IN  GT_U64                      indexRangesBmp
);

/*******************************************************************************
* prvTgfCncPortGroupBlockClientRangesGet
*
* DESCRIPTION:
*   The function gets index ranges per CNC client and Block
*
* INPUTS:
*       portGroupsBmp    - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       client           - CNC client
*
* OUTPUTS:
*       indexRangesBmpPtr - (pointer to) bitmap counter index ranges
*                         DxCh3 and DxChXcat devices have 8 ranges of counter indexes,
*                         each 2048 indexes (0..(2K-1), 2K..(4K-1), ..., 14K..(16K-1))
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         The Lion devices has 64 ranges (512 indexes each).
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncPortGroupBlockClientRangesGet
(
    IN  GT_PORT_GROUPS_BMP        portGroupsBmp,
    IN  GT_U32                    blockNum,
    IN  PRV_TGF_CNC_CLIENT_ENT    client,
    OUT GT_U64                    *indexRangesBmpPtr
);

/*******************************************************************************
* prvTgfCncPortGroupCounterWraparoundIndexesGet
*
* DESCRIPTION:
*   The function gets the counter Wrap Around last 8 indexes
*
* INPUTS:
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       indexNumPtr     - (pointer to) maximal size of array of indexes
*
* OUTPUTS:
*       indexNumPtr     - (pointer to) actual size of array of indexes
*       portGroupIdArr[]- (pointer to) array of port group Ids of
*                         counters wrapped around.
*                         The NULL pointer supported.
*       indexesArr[]    - (pointer to) array of indexes of counters
*                         wrapped around
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The wraparound indexes are cleared on read.
*       Since in the HW each pair of wraparound indexes resides in one register,
*       cleared by read, in case not all the 8 possible indexes are read at once,
*       some data may be lost.
*       Recommendation: Always request 8 indexes => *indexNumPtr=8 as input.
*
*******************************************************************************/
GT_STATUS prvTgfCncPortGroupCounterWraparoundIndexesGet
(
    IN    GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN    GT_U32                  blockNum,
    INOUT GT_U32                  *indexNumPtr,
    OUT   GT_U32                  portGroupIdArr[],
    OUT   GT_U32                  indexesArr[]
);

/*******************************************************************************
* prvTgfCncPortGroupCounterSet
*
* DESCRIPTION:
*   The function sets the counter contents
*
* INPUTS:
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       index            - counter index in the block
*                          valid range see in datasheet of specific device.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*       counterPtr       - (pointer to) counter contents
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncPortGroupCounterSet
(
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            blockNum,
    IN  GT_U32                            index,
    IN  PRV_TGF_CNC_COUNTER_FORMAT_ENT    format,
    IN  PRV_TGF_CNC_COUNTER_STC           *counterPtr
);

/*******************************************************************************
* prvTgfCncPortGroupCounterGet
*
* DESCRIPTION:
*   The function gets the counter contents
*
* INPUTS:
*       portGroupsBmp    - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       index            - counter index in the block
*                          valid range see in datasheet of specific device.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*
* OUTPUTS:
*       counterPtr       - (pointer to) received CNC counter value.
*                          For multi port group devices
*                          the result counter contains the sum of
*                          counters read from all port groups.
*                          It contains more bits than in HW.
*                          For example the sum of 4 35-bit values may be
*                          37-bit value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncPortGroupCounterGet
(
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            blockNum,
    IN  GT_U32                            index,
    IN  PRV_TGF_CNC_COUNTER_FORMAT_ENT    format,
    OUT PRV_TGF_CNC_COUNTER_STC           *counterPtr
);

/*******************************************************************************
* prvTgfCncPortGroupBlockUploadTrigger
*
* DESCRIPTION:
*   The function triggers the Upload of the given counters block.
*   The function checks that there is no unfinished CNC and FDB upload (FU).
*   Also the function checks that all FU messages of previous FU were retrieved
*   by cpssDxChBrgFdbFuMsgBlockGet.
*   An application may check that CNC upload finished by
*   prvTgfCncPortGroupBlockUploadInProcessGet.
*   An application may sequentially upload several CNC blocks before start
*   to retrieve uploaded counters.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE   - if the previous CNC upload or FU in process or
*                        FU is finished but all FU messages were not
*                        retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by prvTgfCncPortGroupUploadedBlockGet.
*
*******************************************************************************/
GT_STATUS prvTgfCncPortGroupBlockUploadTrigger
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  GT_U32                      blockNum
);

/*******************************************************************************
* prvTgfCncPortGroupBlockUploadInProcessGet
*
* DESCRIPTION:
*   The function gets bitmap of numbers of such counters blocks that upload
*   of them yet in process. The HW cannot keep more then one block in such
*   state, but an API matches the original HW representation of the state.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          Checks all active port groups of the bitmap.
*
* OUTPUTS:
*       inProcessBlocksBmpPtr     - (pointer to) bitmap of in-process blocks
*                                   value 1 of the bit#n means that CNC upload
*                                   is not finished yet for block n
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncPortGroupBlockUploadInProcessGet
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    OUT GT_U32                      *inProcessBlocksBmpPtr
);

/*******************************************************************************
* prvTgfCncPortGroupUploadedBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of CNC counter values,
*       the maximal number of elements defined by the caller.
*       The CNC upload may triggered by prvTgfCncPortGroupBlockUploadTrigger.
*       The CNC upload transfers whole CNC block (2K CNC counters)
*       to FDB Upload queue. An application must get all transferred counters.
*       An application may sequentially upload several CNC blocks before start
*       to retrieve uploaded counters.
*       The device may transfer only part of CNC block because of FUQ full. In
*       this case the prvTgfCncPortGroupUploadedBlockGet may return only part of the
*       CNC block with return GT_OK. An application must to call
*       prvTgfCncPortGroupUploadedBlockGet one more time to get rest of the block.
*
* INPUTS:
*       devNum                - device number
*       portGroupsBmp         - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       numOfCounterValuesPtr - (pointer to) maximal number of CNC counters
*                               values to get.This is the size of
*                               counterValuesPtr array allocated by caller.
*       format                - CNC counter HW format,
*                               relevant only for Lion and above
*
* OUTPUTS:
*       numOfCounterValuesPtr - (pointer to) actual number of CNC
*                               counters values in counterValuesPtr.
*       counterValuesPtr      - (pointer to) array that holds received CNC
*                               counters values. Array must be allocated by
*                               caller. For multi port group devices
*                               the result counters contain the sum of
*                               counters read from all port groups.
*                               It contains more bits than in HW.
*                               For example the sum of 4 35-bit values may be
*                               37-bit value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE       - the action succeeded and there are no more waiting
*                          CNC counter value
*       GT_FAIL                  - on failure
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - if the previous FU messages were not
*                          retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by prvTgfCncPortGroupUploadedBlockGet.
*
*******************************************************************************/
GT_STATUS prvTgfCncPortGroupUploadedBlockGet
(
    IN     GT_U8                             devNum,
    IN     GT_PORT_GROUPS_BMP                portGroupsBmp,
    INOUT  GT_U32                            *numOfCounterValuesPtr,
    IN     PRV_TGF_CNC_COUNTER_FORMAT_ENT    format,
    OUT    PRV_TGF_CNC_COUNTER_STC           *counterValuesPtr
);

/*******************************************************************************
* prvTgfCncPortGroupCounterFormatSet
*
* DESCRIPTION:
*   The function sets format of CNC counter
*
* INPUTS:
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*       format          - CNC counter format
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncPortGroupCounterFormatSet
(
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            blockNum,
    IN  PRV_TGF_CNC_COUNTER_FORMAT_ENT    format
);

/*******************************************************************************
* prvTgfCncPortGroupCounterFormatGet
*
* DESCRIPTION:
*   The function gets format of CNC counter
*
* INPUTS:
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*
* OUTPUTS:
*       formatPtr       - (pointer to) CNC counter format
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*                                  on not supported client for device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCncPortGroupCounterFormatGet
(
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            blockNum,
    OUT PRV_TGF_CNC_COUNTER_FORMAT_ENT    *formatPtr
);

/*******************************************************************************
* prvTgfCncFineTuningBlockAmountGet
*
* DESCRIPTION:
*   The function gets Amount of Cnc Blocks per device.
*   if Cnc not supported - 0 returned.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       amount of CnC Blocks per Device or per Port Group
*
* COMMENTS:
*
*******************************************************************************/
GT_U32 prvTgfCncFineTuningBlockAmountGet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfCncFineTuningBlockSizeGet
*
* DESCRIPTION:
*   The function gets Amount of Cnc Blocks per device.
*   if Cnc not supported - 0 returned.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       amount of CnC Blocks per Device or per Port Group
*
* COMMENTS:
*
*******************************************************************************/
GT_U32 prvTgfCncFineTuningBlockSizeGet
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __tgfCncGenh */



