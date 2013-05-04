/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfPolicerGen.h
*
* DESCRIPTION:
*       Generic API for Ingress Policing Engine API.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __tgfPolicerGenh
#define __tgfPolicerGenh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <common/tgfCommon.h>

#ifdef CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/policer/cpssDxChPolicer.h>
    #include <cpss/dxCh/dxCh3/policer/cpssDxCh3Policer.h>
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    #include <cpss/exMxPm/exMxPmGen/policer/cpssExMxPmPolicer.h>
#endif /* EXMXPM_FAMILY */


/******************************************************************************\
 *                          Private type definitions                          *
\******************************************************************************/

/*
 * typedef: enum PRV_TGF_POLICER_COLOR_COUNT_MODE_ENT
 *
 * Description: Enumeration for Color Counting mode.
 *
 * Enumerations:
 *  PRV_TGF_POLICER_COLOR_COUNT_CL_E -   Color Counting is done according
 *                                          to the packet's Conformance Level.
 *
 *  PRV_TGF_POLICER_COLOR_COUNT_DP_E -   Color Counting is done according
 *                                          to the packet's Drop Precedence.
 */
typedef enum{
    PRV_TGF_POLICER_COLOR_COUNT_CL_E,
    PRV_TGF_POLICER_COLOR_COUNT_DP_E
} PRV_TGF_POLICER_COLOR_COUNT_MODE_ENT;

/*
 * typedef: enum PRV_TGF_POLICER_BILLING_CNTR_MODE_ENT
 *
 * Description: Billing Counters Mode. This enumeration controls the Billing
 *              counters resolution.
 *
 * Enumerations:
 *  PRV_TGF_POLICER_BILLING_CNTR_1_BYTE_E   - 1 Byte resolution.
 *
 *  PRV_TGF_POLICER_BILLING_CNTR_16_BYTES_E - 16 Byte resolution.
 *                                                In 16 Byte resolution,
 *                                                Billing counter is rounded
 *                                                down, i.e. a 64 Billing
 *                                                counter packet is counted as
 *                                                4 while a 79 Billing counter
 *                                                packet is counted as 4 as
 *                                                well.
 *
 *  PRV_TGF_POLICER_BILLING_CNTR_PACKET_E   - packet resolution.
 */
typedef enum{
    PRV_TGF_POLICER_BILLING_CNTR_1_BYTE_E,
    PRV_TGF_POLICER_BILLING_CNTR_16_BYTES_E,
    PRV_TGF_POLICER_BILLING_CNTR_PACKET_E
} PRV_TGF_POLICER_BILLING_CNTR_MODE_ENT;

/*
 * typedef: enum PRV_TGF_POLICER_METER_RESOLUTION_ENT
 *
 * Description: Packet/Byte Based Meter resolution.
 *              This enumeration controls the metering algorithm resolution:
 *              packets per second or Bytes per second.
 *
 *
 * Enumerations:
 *  PRV_TGF_POLICER_METER_RESOLUTION_BYTES_E   - Byte based Meter resolution.
 *
 *  PRV_TGF_POLICER_METER_RESOLUTION_PACKETS_E - Packet based Meter
 *                                                  resolution.
 */
typedef  enum{
    PRV_TGF_POLICER_METER_RESOLUTION_BYTES_E,
    PRV_TGF_POLICER_METER_RESOLUTION_PACKETS_E
} PRV_TGF_POLICER_METER_RESOLUTION_ENT;

/*
 * typedef: enum PRV_TGF_POLICER_MNG_CNTR_RESOLUTION_ENT
 *
 * Description: Enumeration for Management counters resolution.
 *
 * Enumerations:
 *  PRV_TGF_POLICER_MNG_CNTR_RESOLUTION_1B_E  -  1 Byte resolution.
 *
 *  PRV_TGF_POLICER_MNG_CNTR_RESOLUTION_16B_E -  16 Byte resolution.
 */
typedef enum{
    PRV_TGF_POLICER_MNG_CNTR_RESOLUTION_1B_E,
    PRV_TGF_POLICER_MNG_CNTR_RESOLUTION_16B_E
} PRV_TGF_POLICER_MNG_CNTR_RESOLUTION_ENT;

/*
 * typedef: enum PRV_TGF_POLICER_MNG_CNTR_SET_ENT
 *
 * Description: Enumeration for Management Counters Set
 *
 * Enumerations:
 *    PRV_TGF_POLICER_MNG_CNTR_SET0_E     - Management Counters Set #0
 *    PRV_TGF_POLICER_MNG_CNTR_SET1_E     - Management Counters Set #1
 *    PRV_TGF_POLICER_MNG_CNTR_SET2_E     - Management Counters Set #2
 *    PRV_TGF_POLICER_MNG_CNTR_DISABLED_E - Management Counters are disabled
 *
 */
typedef enum
{
    PRV_TGF_POLICER_MNG_CNTR_SET0_E,
    PRV_TGF_POLICER_MNG_CNTR_SET1_E,
    PRV_TGF_POLICER_MNG_CNTR_SET2_E,
    PRV_TGF_POLICER_MNG_CNTR_DISABLED_E
} PRV_TGF_POLICER_MNG_CNTR_SET_ENT;

/*
 * typedef: enum PRV_TGF_POLICER_MNG_CNTR_TYPE_ENT
 *
 * Description: Enumeration for Management Counters Type.
 *
 * Enumerations:
 *    PRV_TGF_POLICER_MNG_CNTR_GREEN_E  - These Management Counters count
 *                                           marked GREEN Packet and DU that
 *                                           passed in this flow.
 *    PRV_TGF_POLICER_MNG_CNTR_YELLOW_E - These Management Counters count
 *                                           marked YELLOW Packet and DU that
 *                                           passed in this flow.
 *    PRV_TGF_POLICER_MNG_CNTR_RED_E    - These Management Counters count
 *                                           marked RED Packet and DU that
 *                                           passed in this flow and were not
 *                                           dropped.
 *    PRV_TGF_POLICER_MNG_CNTR_DROP_E   - These Management Counters count
 *                                           marked RED Packet and DU that
 *                                           passed in this flow and were
 *                                           dropped.
 */
typedef enum
{
    PRV_TGF_POLICER_MNG_CNTR_GREEN_E,
    PRV_TGF_POLICER_MNG_CNTR_YELLOW_E,
    PRV_TGF_POLICER_MNG_CNTR_RED_E,
    PRV_TGF_POLICER_MNG_CNTR_DROP_E
} PRV_TGF_POLICER_MNG_CNTR_TYPE_ENT;

/*
 * typedef: enum PRV_TGF_POLICER_ENTRY_TYPE_ENT
 *
 * Description: Enumeration for Policer Entry Type.
 *
 * Enumerations:
 *    PRV_TGF_POLICER_ENTRY_METERING_E  - Metering Policer Entry.
 *    PRV_TGF_POLICER_ENTRY_BILLING_E   - Billing Policer Entry.
 */
typedef enum
{
    PRV_TGF_POLICER_ENTRY_METERING_E,
    PRV_TGF_POLICER_ENTRY_BILLING_E
} PRV_TGF_POLICER_ENTRY_TYPE_ENT;

/*
 * typedef: enum PRV_TGF_POLICER_TT_PACKET_SIZE_MODE_ENT
 *
 * Description: Enumeration for Tunnel Termination Packet Size Mode.
 *
 * Enumerations:
 *  PRV_TGF_POLICER_TT_PACKET_SIZE_REGULAR_ENT   -  Metering and counting
 *                                                      of TT packets is
 *                                                      performed according to
 *                                                      the Ingress Metered
 *                                                      Packet Size Mode.
 *
 *  PRV_TGF_POLICER_TT_PACKET_SIZE_PASSENGER_ENT -  Metering and counting
 *                                                      of TT packets is
 *                                                      performed according to
 *                                                      L3 datagram size only.
 *                                                      This mode doesn't
 *                                                      include the tunnel
 *                                                      header size, the L2
 *                                                      header size, and the
 *                                                      packet CRC in the
 *                                                      metering and counting.
 *
 */
typedef enum{
    PRV_TGF_POLICER_TT_PACKET_SIZE_REGULAR_ENT,
    PRV_TGF_POLICER_TT_PACKET_SIZE_PASSENGER_ENT
} PRV_TGF_POLICER_TT_PACKET_SIZE_MODE_ENT;

/*
 * typedef: enum PRV_TGF_POLICER_METER_MODE_ENT
 *
 * Description: Enumeration of the Meter modes
 *
 * Enumerations:
 *    PRV_TGF_POLICER_METER_MODE_SR_TCM_E - a Single Rate Three Color Marking mode
 *    PRV_TGF_POLICER_METER_MODE_TR_TCM_E - a Two Rate Three Color Marking mode
 */
typedef enum
{
    PRV_TGF_POLICER_METER_MODE_SR_TCM_E,
    PRV_TGF_POLICER_METER_MODE_TR_TCM_E
} PRV_TGF_POLICER_METER_MODE_ENT;

/*
 * typedef: enum PRV_TGF_POLICER_NON_CONFORM_CMD_ENT
 *
 * Description: Structure contains Policer command for Non-Conforming Packets.
 *              Conforming (i.e., In-profile) packets preserve their QoS
 *              parameters setting and their forwarding decision, as set in the
 *              previous engine in the ingress pipe. Non-conforming (i.e.,
 *              Out-ofprofile) packets, are subject to forwarding decision
 *              override and QoS parameter remarking according to the setting
 *              of this field.
 *              A non-conforming packet (marked by Red or Yellow color) can be
 *              dropped by the metering engine, QoS profile remark or can be
 *              forwarded.
 *
 * Enumerations:
 *      PRV_TGF_POLICER_NON_CONFORM_CMD_NO_CHANGE_E  - NO CHANGE.
 *                          Non-conforming packets preserve their QoS
 *                          parameters setting and their forwarding decision,
 *                          as set in the previous engine in the ingress pipe.
 *
 *      PRV_TGF_POLICER_NON_CONFORM_CMD_DROP_E       - DROP.
 *                          Non-conforming packets preserve their QoS
 *                          parameters setting as set in the previous engine in
 *                          the ingress pipe, but their forwarding decision
 *                          changes to Soft Drop or Hard Drop according to the
 *                          setting of the Policer Drop Mode.
 *
 *      PRV_TGF_POLICER_NON_CONFORM_CMD_REMARK_E     - REMARK. QoS Profile
 *                                                        Remarking by table.
 *                          Non-conforming packets preserve their forwarding
 *                          decision as set in the previous engine in the
 *                          ingress pipe, but their QoS parameters setting is
 *                          modified as follows:
 *                          - QoS Profile is extracted from the Policers QoS
 *                            Remarking Table Entry according to CL.
 *                          - ModifyUP is modified by this entry <modifyUP>.
 *                          - ModifyDSCP is modified by this entry <modifyDSCP>.
 *
 *      PRV_TGF_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E - REMARK. QoS Profile
 *                                                             Remark by entry.
 *                          Non-conforming packets preserve their forwarding
 *                          decision as set in the previous engine in the ingress
 *                          pipe, but their QoS parameters setting is modified as
 *                          follows:
 *                          - QoS Profile is modified by this entry <QosProfile>.
 *                          - ModifyUP is modified by this entry <ModifyUP>.
 *                          - ModifyDSCP is modified by this entry <ModifyDSCP>.
 *                          No access to remarking table is done.
 *                          Note: Relevant only for DxChXcat and above.
 *
 */
typedef enum
{
    PRV_TGF_POLICER_NON_CONFORM_CMD_NO_CHANGE_E,
    PRV_TGF_POLICER_NON_CONFORM_CMD_DROP_E,
    PRV_TGF_POLICER_NON_CONFORM_CMD_REMARK_E,
    PRV_TGF_POLICER_NON_CONFORM_CMD_REMARK_BY_ENTRY_E
} PRV_TGF_POLICER_NON_CONFORM_CMD_ENT;

/*
 * typedef: struct PRV_TGF_POLICER_MNG_CNTR_ENTRY_STC
 *
 * Description: Structure for Policer Management Counters Entry.
 *
 * Fields:
 *  duMngCntr       -   Data Unit Management Counter.
 *                      DU size defines according to the assigned resolution
 *                      of Management counters (1 Byte or 16 Bytes).
 *                      Note: DxCh3 - only 32 LSB are used.
 *                            DxChXcat and above - 42 bits are used.
 *  packetMngCntr   -   Packet 32 bit size Management Counter.
 *
 */
typedef struct{
    GT_U64  duMngCntr;
    GT_U32  packetMngCntr;
} PRV_TGF_POLICER_MNG_CNTR_ENTRY_STC;


/*
 * typedef: struct PRV_TGF_POLICER_BILLING_ENTRY_STC
 *
 * Description: Structure for Policer Billing Entry.
 *
 * Fields:
 *  greenCntr   -   Green Billing counter, counts the number of
 *                  Data Units (1 Byte/16 Bytes) of packets, marked Green
 *                  that passed in this flow.
 *                  Note: DxCh3 - only 32 LSB are used.
 *                        DxChXcat and above - 42 bits are used.
 *  yellowCntr  -   Yellow Billing counter, counts the number of
 *                  Data Units (1 Byte/16 Bytes) of packets, marked Yellow
 *                  that passed in this flow.
 *                  Note: DxCh3 - only 32 LSB are used.
 *                        DxChXcat and above - 42 bits are used.
 *  redCntr     -   Red Billing counter, counts the number of
 *                  Data Units (1 Byte/16 Bytes) of packets, marked Red
 *                  that passed in this flow.
 *                  Note: DxCh3 - only 32 LSB are used.
 *                        DxChXcat and above - 42 bits are used.
 *  billingCntrMode    -   The resolution of billing counters.
 *                  Applies only to the color counters.
 *                  Note: Relevant for DxChXcat and above.
 *
 */
typedef struct{
    GT_U64                                      greenCntr;
    GT_U64                                      yellowCntr;
    GT_U64                                      redCntr;
    PRV_TGF_POLICER_BILLING_CNTR_MODE_ENT    billingCntrMode;
} PRV_TGF_POLICER_BILLING_ENTRY_STC;

/*
 * typedef: union PRV_TGF_POLICER_METER_TB_PARAMS_UNT
 *
 * Description: Union for single/dual Token Bucket parameters
 *
 * Fields:
 *  srTcmParams:
 *     cir - Committed Information Rate in Kbps
 *     cbs - Committed Burst Size in bytes
 *     ebs - Excess Burst Size in bytes
 *  trTcmParams:
 *     cir - Committed Information Rate in Kbps
 *     cbs - Committed Burst Size in bytes
 *     pir - Peak Information Rate in Kbps (1K = 1000)
 *     pbs - Peak Burst Size in bytes
 *
 */
typedef union
{
    struct
    {
        GT_U32 cir;
        GT_U32 cbs;
        GT_U32 ebs;
    } srTcmParams;
    struct
    {
        GT_U32 cir;
        GT_U32 cbs;
        GT_U32 pir;
        GT_U32 pbs;
    } trTcmParams;
} PRV_TGF_POLICER_METER_TB_PARAMS_UNT;


/*
 * typedef: enum PRV_TGF_POLICER_REMARK_MODE_ENT
 *
 * Description: Enumeration of the Remark modes
 *
 * Enumerations:
 *    PRV_TGF_POLICER_REMARK_MODE_L2_E - TC or UP is index in L2 remarking table
 *    PRV_TGF_POLICER_REMARK_MODE_L3_E - DSCP for IP packet or EXP for MPLS
 *                                       packets is index in L3 remarking table
 *
 */
typedef enum
{
    PRV_TGF_POLICER_REMARK_MODE_L2_E,
    PRV_TGF_POLICER_REMARK_MODE_L3_E
} PRV_TGF_POLICER_REMARK_MODE_ENT;

/*
 * typedef: enum PRV_TGF_POLICER_CMD_ENT
 *
 * Description: Enumeration of Policer Commands is applied to packets that were
 *              classified as Red (out-of-profile) by the traffic meter
 *
 * Enumerations:
 *    PRV_TGF_POLICER_CMD_NONE_E     - performs no action on the packet
 *    PRV_TGF_POLICER_CMD_DROP_RED_E - the packet is SOFT or HARD dropped
 *    PRV_TGF_POLICER_CMD_QOS_MARK_BY_ENTRY_E         - preserve forwarding decision
 *    PRV_TGF_POLICER_CMD_QOS_PROFILE_MARK_BY_TABLE_E - preserve forwarding decision
 *
 */
typedef enum
{
    PRV_TGF_POLICER_CMD_NONE_E,
    PRV_TGF_POLICER_CMD_DROP_RED_E,
    PRV_TGF_POLICER_CMD_QOS_MARK_BY_ENTRY_E,
    PRV_TGF_POLICER_CMD_QOS_PROFILE_MARK_BY_TABLE_E
} PRV_TGF_POLICER_CMD_ENT;


/*
 * typedef: struct PRV_TGF_POLICER_ENTRY_STC
 *
 * Description: Structure for Policer Entry
 *
 * Fields:
 *    policerEnable   - policer Enable
 *    meterColorMode  - policer's meter color mode
 *    counterEnable   - enables counting In Profile and Out of Profile packets
 *    counterSetIndex - policing counters set index
 *    cmd             - policer command
 *    qosProfile      - QoSProfile assigned to non-conforming packets
 *    dropRed         - drop packets marked with Red Conformance Level
 *    modifyQosParams - enable/disable modification of the QoS parameters
 *    remarkMode      - remark mode
 *    meterMode       - meter color marking mode
 *    mngCounterSet   - affiliation of Policer Entry to Management Counter Set
 *    tbParams        - token bucket parameters
 *  countingEntryIndex      -   index of billing or policy counting entry.
 *  modifyUp                -   enable/disable modifying Drop Precedence.
 *                              Note: For DxChXcat and above Egress, only Enable/Disable
 *                                    options are valid.
 *  modifyDscp              -   enable/disable modifying IP DCSP field.
 *                              Note: For DxChXcat and above Egress, only Enable/Disable
 *                                    options are valid.
 *  modifyDp                -   enable/disable modifying DP field.
 *                              Note: Supported for DxChXcat and above.
 *                                    Ignored by DxCh3.
 *                              Note: Only Enable/Disable options are valid.
 *                              Note: only for Egress Metering Entry.
 *  yellowPcktCmd           -   Policer commands for Non-Conforming Yellows
 *                              Packets.
 *  redPcktCmd              -   Policer commands for Non-Conforming Red Packets.
 *
 */
typedef struct
{
    GT_BOOL                                 policerEnable;
    CPSS_POLICER_COLOR_MODE_ENT             meterColorMode;
    GT_BOOL                                 counterEnable;
    GT_U32                                  counterSetIndex;
    PRV_TGF_POLICER_CMD_ENT                 cmd;
    GT_U32                                  qosProfile;
    GT_BOOL                                 dropRed;
    PRV_TGF_QOS_PARAM_MODIFY_STC            modifyQosParams;
    PRV_TGF_POLICER_REMARK_MODE_ENT         remarkMode;
    PRV_TGF_POLICER_METER_MODE_ENT          meterMode;
    PRV_TGF_POLICER_MNG_CNTR_SET_ENT        mngCounterSet;
    PRV_TGF_POLICER_METER_TB_PARAMS_UNT     tbParams;
    GT_U32                                  countingEntryIndex;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT   modifyUp;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT   modifyDscp;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT   modifyDp;
    PRV_TGF_POLICER_NON_CONFORM_CMD_ENT     yellowPcktCmd;
    PRV_TGF_POLICER_NON_CONFORM_CMD_ENT     redPcktCmd;
} PRV_TGF_POLICER_ENTRY_STC;

/*
 * typedef: enum PRV_TGF_POLICER_STAGE_TYPE_ENT
 *
 * Description: Enumeration for Policer Unit Type.
 *
 * Enumerations:
 *    PRV_TGF_POLICER_STAGE_INGRESS_0_E   - Ingress Policer stage #0.
 *    PRV_TGF_POLICER_STAGE_INGRESS_1_E   - Ingress Policer stage #1.
 *    PRV_TGF_POLICER_STAGE_EGRESS_E      - Egress Policer stage.
 */
typedef enum
{
    PRV_TGF_POLICER_STAGE_INGRESS_0_E,
    PRV_TGF_POLICER_STAGE_INGRESS_1_E,
    PRV_TGF_POLICER_STAGE_EGRESS_E,
    PRV_TGF_POLICER_STAGE_NUM
} PRV_TGF_POLICER_STAGE_TYPE_ENT;


/*
 * typedef: enum PRV_TGF_POLICER_COUNTING_MODE_ENT
 *
 * Description: Enumeration for Policer Counting Mode.
 *
 * Enumerations:
 *    PRV_TGF_POLICER_COUNTING_DISABLE_E        - Counting is disabled.
 *    PRV_TGF_POLICER_COUNTING_BILLING_IPFIX_E  - Billing (per color counting)
 *                                                or IPFIX counting.
 *                                                Counting is triggered by
 *                                                Policy Action Entry or by
 *                                                Metering Entry.
 *    PRV_TGF_POLICER_COUNTING_POLICY_E         - Policy activity counting mode:
 *                                                packet counters only. Counting
 *                                                is triggered by Policy Action
 *                                                Entry.
 *    PRV_TGF_POLICER_COUNTING_VLAN_E           - VLAN counting mode: packet or
 *                                                byte counters. Counting is
 *                                                triggered for every packet
 *                                                according to VLAN-ID.
 */
typedef enum
{
    PRV_TGF_POLICER_COUNTING_DISABLE_E,
    PRV_TGF_POLICER_COUNTING_BILLING_IPFIX_E,
    PRV_TGF_POLICER_COUNTING_POLICY_E,
    PRV_TGF_POLICER_COUNTING_VLAN_E
} PRV_TGF_POLICER_COUNTING_MODE_ENT;


/*
 * typedef: enum PRV_TGF_POLICER_REMARK_TABLE_TYPE_ENT
 *
 * Description:
 *      QoS parameter enumeration
 *
 * Enumerations:
 *      PRV_TGF_POLICER_REMARK_TABLE_TYPE_TC_UP_E - TC/UP Remark table.
 *      PRV_TGF_POLICER_REMARK_TABLE_TYPE_DSCP_E - DSCP Remark table.
 *      PRV_TGF_POLICER_REMARK_TABLE_TYPE_EXP_E - EXP Remark table.
 *
 * COMMENTS:
 */
typedef enum {
    PRV_TGF_POLICER_REMARK_TABLE_TYPE_TC_UP_E,
    PRV_TGF_POLICER_REMARK_TABLE_TYPE_DSCP_E,
    PRV_TGF_POLICER_REMARK_TABLE_TYPE_EXP_E
}PRV_TGF_POLICER_REMARK_TABLE_TYPE_ENT;


/*
 * Typedef: PRV_TGF_POLICER_QOS_PARAM_STC
 *
 * Description: The Quality of Service parameters.
 *
 * Fields:
 *    up    - IEEE 802.1p User Priority (0..7)
 *    dscp  - IP DCSP field (0..63)
 *    exp   - MPLS label EXP value (0..7)
 *    dp    - Drop Precedence value: GREEN, YELLOW or RED.
 */
typedef struct
{
    GT_U32              up;
    GT_U32              dscp;
    GT_U32              exp;
    CPSS_DP_LEVEL_ENT   dp;
}PRV_TGF_POLICER_QOS_PARAM_STC;


/*
 * typedef: enum PRV_TGF_POLICER_STAGE_METER_MODE_ENT
 *
 * Description: Global stage mode.
 *
 * Enumerations:
 *    PRV_TGF_POLICER_STAGE_METER_MODE_PORT_E   - Port-based metering is performed if
 *                          the relevant port is enabled for metering.
 *                          Port policing is done per device local source port
 *                          for the Ingress Policer and per device local egress
 *                          port for Egress Policer.
 *    PRV_TGF_POLICER_STAGE_METER_MODE_FLOW_E   - Flow-based metering is performed if
 *                          metering is enabled in Policy/TTI Action and
 *                          the pointer is taken from the TTI/Policy Action.

 */
typedef enum
{
    PRV_TGF_POLICER_STAGE_METER_MODE_PORT_E,
    PRV_TGF_POLICER_STAGE_METER_MODE_FLOW_E
} PRV_TGF_POLICER_STAGE_METER_MODE_ENT;



/*
 * typedef: enum PRV_TGF_POLICER_STORM_TYPE_ENT
 *
 * Description: Storm Types.
 *
 * Enumerations:
 *    PRV_TGF_POLICER_STORM_TYPE_UC_KNOWN_E        - Knowm Unicast
 *    PRV_TGF_POLICER_STORM_TYPE_UC_UNKNOWN_E      - Unknowm Unicast
 *    PRV_TGF_POLICER_STORM_TYPE_MC_UNREGISTERED_E - Unregistered Multicast
 *    PRV_TGF_POLICER_STORM_TYPE_MC_REGISTERED_E   - Registered Multicast
 *    PRV_TGF_POLICER_STORM_TYPE_BC_E              - Broadcast
 *    PRV_TGF_POLICER_STORM_TYPE_TCP_SYN_E         - TCP-SYN
 */
typedef enum
{
    PRV_TGF_POLICER_STORM_TYPE_UC_KNOWN_E,
    PRV_TGF_POLICER_STORM_TYPE_UC_UNKNOWN_E,
    PRV_TGF_POLICER_STORM_TYPE_MC_UNREGISTERED_E,
    PRV_TGF_POLICER_STORM_TYPE_MC_REGISTERED_E,
    PRV_TGF_POLICER_STORM_TYPE_BC_E,
    PRV_TGF_POLICER_STORM_TYPE_TCP_SYN_E
} PRV_TGF_POLICER_STORM_TYPE_ENT;


/*
 * typedef: enum PRV_TGF_POLICER_L2_REMARK_MODEL_ENT
 *
 * Description: Enumeration for L2 packets remarking model.
 *
 * Enumerations:
 *    PRV_TGF_POLICER_L2_REMARK_MODEL_TC_E  - Traffic Class is index in the
 *                                              QoS table.
 *    PRV_TGF_POLICER_L2_REMARK_MODEL_UP_E  - User Priority is index in the
 *                                              QoS table.
 */
typedef enum
{
    PRV_TGF_POLICER_L2_REMARK_MODEL_TC_E,
    PRV_TGF_POLICER_L2_REMARK_MODEL_UP_E
} PRV_TGF_POLICER_L2_REMARK_MODEL_ENT;


#define PRV_TGF_POLICER_MEMORY_CTRL_MODE_PLR0_UPPER_PLR1_LOWER_E  CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_0_E
#define PRV_TGF_POLICER_MEMORY_CTRL_MODE_PLR0_UPPER_AND_LOWER_E   CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_1_E
#define PRV_TGF_POLICER_MEMORY_CTRL_MODE_PLR1_UPPER_AND_LOWER_E   CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_2_E
#define PRV_TGF_POLICER_MEMORY_CTRL_MODE_PLR1_UPPER_PLR0_LOWER_E  CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_3_E

/*
 * typedef: enum PRV_TGF_POLICER_MEMORY_CTRL_MODE_ENT
 *
 * Description: Enumeration for Memory control mode.
 *              Lower memory uses 256 entries. Upper memory uses the rest.
 *              In case of xCat2: memory 1 has 172 entries, memory 2 has
 *              84 entries and memory 3 has 28 entries.
 *
 * Enumerations:
 *    PRV_TGF_POLICER_MEMORY_CTRL_MODE_0_E  - Policer 0
 *                  uses the upper memory and Policer 1 uses the lower memory.
 *                  In case of xCat2: Policer 1 uses two memories (1 and 2),
 *                  Policer 0 uses one memory (3).
 *    PRV_TGF_POLICER_MEMORY_CTRL_MODE_1_E  - Policer 0
 *                  uses both memories.
 *                  In case of xCat2: Policer 0 uses all three memories.
 *    PRV_TGF_POLICER_MEMORY_CTRL_MODE_2_E  - Policer 1
 *                  uses both memories.
  *                  In case of xCat2: Policer 1 uses all three memories.
 *    PRV_TGF_POLICER_MEMORY_CTRL_MODE_3_E - Policer 1
 *                  uses the upper memory and Policer 0 uses the lower memory.
 *                  In case of xCat2: Policer 0 uses two memories (1 and 2),
 *                  Policer 1 uses one memory (3).
 *    PRV_TGF_POLICER_MEMORY_CTRL_MODE_4_E - Policer 0 uses one memory (1), 
 *                  Policer 1 uses two memories (2 and 3).
 *                  Applicable devices: xCat2 
 *                  Note: In this mode, when stage mode is Port, a compressed
 *                        address select should be configured: 
 *                        PRV_TGF_POLICER_PORT_MODE_ADDR_SELECT_TYPE_COMPRESSED_E
 *                        by cpssDxChPolicerPortModeAddressSelectSet().
 *    PRV_TGF_POLICER_MEMORY_CTRL_MODE_5_E - Policer 1 uses one memory (1), 
 *                  Policer 0 uses two memories (2 and 3).
 *                  Applicable devices: xCat2 
 *                  Note: In this mode, when stage mode is Port, a compressed
 *                        address select should be configured: 
 *                        PRV_TGF_POLICER_PORT_MODE_ADDR_SELECT_TYPE_COMPRESSED_E
 *                        by cpssDxChPolicerPortModeAddressSelectSet().
 */
typedef enum
{
    PRV_TGF_POLICER_MEMORY_CTRL_MODE_0_E,
    PRV_TGF_POLICER_MEMORY_CTRL_MODE_1_E,
    PRV_TGF_POLICER_MEMORY_CTRL_MODE_2_E,
    PRV_TGF_POLICER_MEMORY_CTRL_MODE_3_E,
    PRV_TGF_POLICER_MEMORY_CTRL_MODE_4_E,
    PRV_TGF_POLICER_MEMORY_CTRL_MODE_5_E
} PRV_TGF_POLICER_MEMORY_CTRL_MODE_ENT;




/*
 * typedef: enum PRV_TGF_POLICER_VLAN_CNTR_MODE_ENT
 *
 * Description: Packet/Byte Vlan counter mode.
 *
 *
 * Enumerations:
 *  PRV_TGF_POLICER_VLAN_CNTR_MODE_BYTES_E   - Bytes Vlan counter mode.
 *
 *  PRV_TGF_POLICER_VLAN_CNTR_MODE_PACKETS_E - Packets Vlan counter mode.
 */
typedef  enum{
    PRV_TGF_POLICER_VLAN_CNTR_MODE_BYTES_E,
    PRV_TGF_POLICER_VLAN_CNTR_MODE_PACKETS_E
} PRV_TGF_POLICER_VLAN_CNTR_MODE_ENT;

/*
 * typedef: struct PRV_TGF_POLICER_COUNTERS_STC
 *
 * Description: Structure for Policer Counters Entry
 *
 * Fields:
 *    outOfProfileBytesCnt - Out-of-Profile bytes counter
 *    inProfileBytesCnt    - In-Profile bytes counter
 *
 */
typedef struct
{
      GT_U32    outOfProfileBytesCnt;
      GT_U32    inProfileBytesCnt;
} PRV_TGF_POLICER_COUNTERS_STC;


/*
 * typedef: enum PRV_TGF_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT
 *
 * Description: Port Mode Address Select type.
 *
 *
 * Enumerations:
 *  PRV_TGF_POLICER_PORT_MODE_ADDR_SELECT_TYPE_FULL_E   - The address is {index,port_num}. 
 *
 *  PRV_TGF_POLICER_PORT_MODE_ADDR_SELECT_TYPE_COMPRESSED_E - Compressed; 
 *          The address is {port_num,index}.
 */
typedef enum{
    PRV_TGF_POLICER_PORT_MODE_ADDR_SELECT_TYPE_FULL_E,
    PRV_TGF_POLICER_PORT_MODE_ADDR_SELECT_TYPE_COMPRESSED_E
} PRV_TGF_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT;


/******************************************************************************\
 *                       CPSS generic API section                             *
\******************************************************************************/

#ifdef CHX_FAMILY
/*******************************************************************************
* prvTgfConvertGenericToDxChPolicerStage
*
* DESCRIPTION:
*       Convert generic into device specific policer stage
*
* INPUTS:
*       policerStage - (pointer to) policer stage
*
* OUTPUTS:
*       dxChStagePtr - (pointer to) DxCh policer stage
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfConvertGenericToDxChPolicerStage
(
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT      policerStage,
    OUT CPSS_DXCH_POLICER_STAGE_TYPE_ENT    *dxChStagePtr
);
#endif /* CHX_FAMILY */

/*******************************************************************************
* prvTgfPolicerInit
*
* DESCRIPTION:
*       Init Traffic Conditioner facility on specified device
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - otherwise
*       GT_BAD_PARAM - on illegal devNum
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerInit
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPolicerMeteringEnableSet
*
* DESCRIPTION:
*       Enables or disables metering per device.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       enable          - Enable/disable metering:
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerMeteringEnableSet
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN GT_BOOL                        enable
);

/*******************************************************************************
* prvTgfPolicerMeteringEnableGet
*
* DESCRIPTION:
*       Gets device metering status (Enable/Disable).
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       enablePtr       - pointer to Enable/Disable metering
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerMeteringEnableGet
(
    IN  GT_U8                          devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    OUT GT_BOOL                        *enablePtr
);

/*******************************************************************************
* prvTgfPolicerBillingCountingEnableSet
*
* DESCRIPTION:
*       Enables or disables Billing Counting.
*
* INPUTS:
*       devNum          - physical device number.
*       enable          - enable/disable Billing Counting:
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerBillingCountingEnableSet
(
    IN GT_BOOL  enable
);

/*******************************************************************************
* prvTgfPolicerBillingCountingEnableGet
*
* DESCRIPTION:
*       Gets device Billing Counting status (Enable/Disable).
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       enablePtr       - pointer to Enable/Disable Billing Counting
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerBillingCountingEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
);

/*******************************************************************************
* prvTgfPolicerCountingModeSet
*
* DESCRIPTION:
*       Configures counting mode.
*
* INPUTS:
*       devNum  - physical device number.
*       stage   - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       mode    -  counting mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or mode.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPolicerCountingModeSet
(
    IN GT_U8                             devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT    stage,
    IN PRV_TGF_POLICER_COUNTING_MODE_ENT mode
);

/*******************************************************************************
* prvTgfPolicerCountingModeGet
*
* DESCRIPTION:
*       Gets the couning mode.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       modePtr         - pointer to Counting mode.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPolicerCountingModeGet
(
    IN  GT_U8                             devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT    stage,
    OUT PRV_TGF_POLICER_COUNTING_MODE_ENT *modePtr
);

/*******************************************************************************
* prvTgfPolicerBillingCountersModeSet
*
* DESCRIPTION:
*      Sets the Billing Counters resolution.
*
* INPUTS:
*       devNum          - physical device number.
*       cntrMode        - Billing Counters resolution: 1 Byte
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or cntrMode.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerBillingCountersModeSet
(
    IN PRV_TGF_POLICER_BILLING_CNTR_MODE_ENT cntrMode
);


/*******************************************************************************
* prvTgfPolicerBillingCountersModeGet
*
* DESCRIPTION:
*      Gets the Billing Counters resolution.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       cntrModePtr     - pointer to the Billing Counters resolution
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerBillingCountersModeGet
(
    IN GT_U8                                  devNum,
    OUT PRV_TGF_POLICER_BILLING_CNTR_MODE_ENT *cntrModePtr
);

/*******************************************************************************
* prvTgfPolicerPacketSizeModeSet
*
* DESCRIPTION:
*       Sets metered Packet Size Mode that metering and billing is done
*       according to.
*
* INPUTS:
*       devNum             - physical device number.
*       stage              - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       packetSize         - Type of packet size
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or packetSize.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPolicerPacketSizeModeSet
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT    stage,
    IN CPSS_POLICER_PACKET_SIZE_MODE_ENT packetSize
);

/*******************************************************************************
* prvTgfPolicerPacketSizeModeGet
*
* DESCRIPTION:
*       Gets metered Packet Size Mode that metering and billing is done
*       according to.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       packetSizePtr   - pointer to the Type of packet size
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPolicerPacketSizeModeGet
(
    IN  GT_U8                               devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT      stage,
    OUT CPSS_POLICER_PACKET_SIZE_MODE_ENT   *packetSizePtr
);

/*******************************************************************************
* prvTgfPolicerMeterResolutionSet
*
* DESCRIPTION:
*       Sets metering algorithm resolution
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       resolution      - packet/Byte based Meter resolution.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or resolution.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPolicerMeterResolutionSet
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT       stage,
    IN PRV_TGF_POLICER_METER_RESOLUTION_ENT resolution
);

/*******************************************************************************
* prvTgfPolicerMeterResolutionGet
*
* DESCRIPTION:
*       Gets metering algorithm resolution
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       resolutionPtr   - pointer to the Meter resolution: packet or Byte based.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPolicerMeterResolutionGet
(
    IN GT_U8                                    devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT         stage,
    OUT PRV_TGF_POLICER_METER_RESOLUTION_ENT *resolutionPtr
);

/*******************************************************************************
* prvTgfPolicerDropTypeSet
*
* DESCRIPTION:
*       Sets the Policer out-of-profile drop command type.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0 or Ingress #1.
*       dropType        - Policer Drop Type: Soft or Hard.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, dropType or stage.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerDropTypeSet
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN CPSS_DROP_MODE_TYPE_ENT        dropType
);

/*******************************************************************************
* prvTgfPolicerDropTypeGet
*
* DESCRIPTION:
*       Gets the Policer out-of-profile drop command type.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0 or Ingress #1.
*
* OUTPUTS:
*       dropTypePtr     - pointer to the Policer Drop Type: Soft or Hard.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerDropTypeGet
(
    IN  GT_U8                               devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT     stage,
    OUT CPSS_DROP_MODE_TYPE_ENT             *dropTypePtr
);

/*******************************************************************************
* prvTgfPolicerCountingColorModeSet
*
* DESCRIPTION:
*       Sets the Policer color counting mode.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       mode            - Color counting mode: Drop Precedence or
*                         Conformance Level.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or mode.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerCountingColorModeSet
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT       stage,
    IN PRV_TGF_POLICER_COLOR_COUNT_MODE_ENT mode
);

/*******************************************************************************
* prvTgfPolicerCountingColorModeGet
*
* DESCRIPTION:
*       Gets the Policer color counting mode.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       modePtr         - pointer to the color counting mode:
*                         Drop Precedence or Conformance Level.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerCountingColorModeGet
(
    IN  GT_U8                                       devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT             stage,
    OUT PRV_TGF_POLICER_COLOR_COUNT_MODE_ENT     *modePtr
);

/*******************************************************************************
* prvTgfPolicerManagementCntrsResolutionSet
*
* DESCRIPTION:
*       Sets the Management counters resolution (either 1 Byte or 16 Bytes).
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       cntrSet         - Management Counters Set [0..2].
*       cntrResolution  - Management Counters resolution: 1 or 16 Bytes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or
*                                     Management Counters Set or cntrResolution.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerManagementCntrsResolutionSet
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT             stage,
    IN PRV_TGF_POLICER_MNG_CNTR_SET_ENT          cntrSet,
    IN PRV_TGF_POLICER_MNG_CNTR_RESOLUTION_ENT   cntrResolution
);

/*******************************************************************************
* prvTgfPolicerManagementCntrsResolutionGet
*
* DESCRIPTION:
*       Gets the Management counters resolution (either 1 Byte or 16 Bytes).
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       cntrSet         - Management Counters Set [0..2].
*
* OUTPUTS:
*       cntrResolutionPtr   - pointer to the Management Counters
*                             resolution: 1 or 16 Bytes.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or  Mng Counters Set.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerManagementCntrsResolutionGet
(
    IN  GT_U8                                       devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT             stage,
    IN  PRV_TGF_POLICER_MNG_CNTR_SET_ENT         cntrSet,
    OUT PRV_TGF_POLICER_MNG_CNTR_RESOLUTION_ENT  *cntrResolutionPtr
);

/*******************************************************************************
* prvTgfPolicerPacketSizeModeForTunnelTermSet
*
* DESCRIPTION:
*      Sets size mode for metering and counting of tunnel terminated packets.
*
* INPUTS:
*       devNum              - physical device number.
*       stage               - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       ttPacketSizeMode    - Tunnel Termination Packet Size Mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or ttPacketSizeMode.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerPacketSizeModeForTunnelTermSet
(
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT           stage,
    IN  PRV_TGF_POLICER_TT_PACKET_SIZE_MODE_ENT  ttPacketSizeMode
);

/*******************************************************************************
* prvTgfPolicerPacketSizeModeForTunnelTermGet
*
* DESCRIPTION:
*      Gets size mode for metering and counting of tunnel terminated packets.
*
* INPUTS:
*       devNum              - physical device number.
*       stage               - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       ttPacketSizeModePtr     - pointer to the Tunnel Termination
*                                 Packet Size Mode.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerPacketSizeModeForTunnelTermGet
(
    IN  GT_U8                                       devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT             stage,
    OUT PRV_TGF_POLICER_TT_PACKET_SIZE_MODE_ENT  *ttPacketSizeModePtr
);

/*******************************************************************************
* prvTgfPolicerMeteringAutoRefreshScanEnableSet
*
* DESCRIPTION:
*       Enables or disables the metering Auto Refresh Scan mechanism.
*
* INPUTS:
*       devNum          - physical device number.
*       enable          - Enable/disable Auto Refresh Scan mechanism:
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerMeteringAutoRefreshScanEnableSet
(
    IN GT_BOOL enable
);

/*******************************************************************************
* prvTgfPolicerMeteringAutoRefreshScanEnableGet
*
* DESCRIPTION:
*       Gets metering Auto Refresh Scan mechanism status (Enabled/Disabled).
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       enablePtr       - pointer on Auto Refresh Scan mechanism status
*                         (Enable/Disable) :
*                         GT_TRUE  - Auto Refresh scan mechanism is enabled.
*                         GT_FALSE - Auto Refresh scan mechanism is disabled.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerMeteringAutoRefreshScanEnableGet
(
    IN  GT_U8                               devNum,
    OUT GT_BOOL                             *enablePtr
);

/*******************************************************************************
* prvTgfPolicerMeteringAutoRefreshRangeSet
*
* DESCRIPTION:
*       Sets Metering Refresh Scan address range (Start and Stop addresses).
*
* INPUTS:
*       devNum              - physical device number.
*       startAddress        - beginning of the address range to be scanned
*                             by the Auto Refresh Scan mechanism [0..1023].
*       stopAddress         - end of the address range to be scanned by the
*                             Auto Refresh Scan mechanism [0..1023].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or illegal values
*                                     of startAddress and stopAddress.
*       GT_OUT_OF_RANGE             - on out of range of startAddress
*                                     or stopAddress.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerMeteringAutoRefreshRangeSet
(
    IN GT_U32 startAddress,
    IN GT_U32 stopAddress
);

/*******************************************************************************
* prvTgfPolicerMeteringAutoRefreshRangeGet
*
* DESCRIPTION:
*       Gets Metering Refresh Scan address range (Start and Stop addresses).
*
* INPUTS:
*       devNum              - physical device number.
*
* OUTPUTS:
*       startAddressPtr     - pointer to the beginning address of Refresh
*                             Scan address range.
*       stopAddressPtr      - pointer to the end address of Refresh Scan
*                             address range.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointers.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerMeteringAutoRefreshRangeGet
(
    IN GT_U8                                devNum,
    OUT GT_U32                              *startAddressPtr,
    OUT GT_U32                              *stopAddressPtr
);

/*******************************************************************************
* prvTgfPolicerMeteringAutoRefreshIntervalSet
*
* DESCRIPTION:
*       Sets the time interval between two refresh access to metering table.
*
* INPUTS:
*       devNum          - physical device number.
*       interval        - time interval between refresh of two entries
*                         in micro seconds. Upon 270 Mhz core clock value
*                         of DxCh3, the range [0..15907286 microS].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_OUT_OF_RANGE             - on out of range of interval value.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerMeteringAutoRefreshIntervalSet
(
    IN GT_U32 interval
);

/*******************************************************************************
* prvTgfPolicerMeteringAutoRefreshIntervalGet
*
* DESCRIPTION:
*       Gets the time interval between two refresh access to metering table.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       intervalPtr     - pointer to the time interval between refresh of two
*                         entries in micro seconds. Upon 270 Mhz core clock
*                         value of DxCh3, the range [0..15907286 microS].
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerMeteringAutoRefreshIntervalGet
(
    IN GT_U8                                devNum,
    OUT GT_U32                              *intervalPtr
);

/*******************************************************************************
* prvTgfPolicerMeteringEntryRefresh
*
* DESCRIPTION:
*       Refresh the Policer Metering Entry.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       entryIndex      - index of Policer Metering Entry
*                         going to be refreshed [0..1023].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_TIMEOUT                  - on time out of IPLR Table indirect access.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or entryIndex.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerMeteringEntryRefresh
(
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN  GT_U32                         entryIndex
);

/*******************************************************************************
* prvTgfPolicerPortMeteringEnableSet
*
* DESCRIPTION:
*       Enables or disables a port metering trigger for packets
*       arriving on this port.
*       When feature is enabled the meter entry index is a port number.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       portNum         - port number (including the CPU port).
*       enable          - Enable/Disable per-port metering for packets arriving
*                         on this port:
*                         GT_TRUE  - Metering is triggered on specified port.
*                         GT_FALSE - Metering isn't triggered on specified port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or portNum.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerPortMeteringEnableSet
(
    IN GT_U8                          devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN GT_U8                          portNum,
    IN GT_BOOL                        enable
);

/*******************************************************************************
* prvTgfPolicerPortMeteringEnableGet
*
* DESCRIPTION:
*       Gets port status (Enable/Disable) of metering for packets
*       arriving on this port.
*       When feature is enabled the meter entry index is a port number.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       portNum         - port number (including the CPU port).
*
* OUTPUTS:
*       enablePtr       - pointer on per-port metering status (Enable/Disable)
*                         for packets arriving on specified port:
*                         GT_TRUE  - Metering is triggered on specified port.
*                         GT_FALSE - Metering isn't triggered on specified port.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or portNum.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerPortMeteringEnableGet
(
    IN  GT_U8                               devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT     stage,
    IN  GT_U8                               portNum,
    OUT GT_BOOL                             *enablePtr
);

/*******************************************************************************
* prvTgfPolicerMruSet
*
* DESCRIPTION:
*       Sets the Policer Maximum Receive Unit size.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       mruSize         - Policer MRU value in bytes, range of values [0..64K].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_OUT_OF_RANGE             - on mruSize out of range [0..65535 Bytes].
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerMruSet
(
    IN GT_U8                                devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT     stage,
    IN GT_U32                               mruSize
);

/*******************************************************************************
* prvTgfPolicerMruGet
*
* DESCRIPTION:
*       Gets the Policer Maximum Receive Unit size.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       mruSizePtr      - pointer to the Policer MRU value in bytes,
*                         range of values [0..64K].
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerMruGet
(
    IN  GT_U8                               devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT     stage,
    OUT GT_U32                              *mruSizePtr
);

/*******************************************************************************
* prvTgfPolicerErrorGet
*
* DESCRIPTION:
*       Gets address and type of Policer Entry that had an ECC error.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       entryTypePtr    - pointer to the Type of Entry (Metering or Counting)
*                         that had an error.
*       entryAddrPtr    - pointer to the Policer Entry that had an error.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_EMPTY                    - on missing error information.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerErrorGet
(
    IN  GT_U8                               devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT     stage,
    OUT PRV_TGF_POLICER_ENTRY_TYPE_ENT   *entryTypePtr,
    OUT GT_U32                              *entryAddrPtr
);

/*******************************************************************************
* prvTgfPolicerErrorCounterGet
*
* DESCRIPTION:
*       Gets the value of the Policer ECC Error Counter.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       cntrValuePtr    - pointer to the Policer ECC Error counter value.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerErrorCounterGet
(
    IN  GT_U8                               devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT     stage,
    OUT GT_U32                              *cntrValuePtr
);

/*******************************************************************************
* prvTgfPolicerManagementCountersSet
*
* DESCRIPTION:
*       Sets the value of specified Management Counters.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for DxChXcat and above devices
*                         and ignored by DxCh3.
*       mngCntrSet      - Management Counters Set[0..2].
*       mngCntrType     - Management Counters Type (GREEN, YELLOW, RED, DROP).
*       mngCntrPtr      - pointer to the Management Counters Entry must be set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage or mngCntrType
*                                     or Management Counters Set number.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       In data unit management counter only 32 bits are used for DxCh3 devices
*       and 42 bits are used for DxChXcat and above devices.
*******************************************************************************/
GT_STATUS prvTgfPolicerManagementCountersSet
(
    IN GT_U8                              devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT     stage,
    IN PRV_TGF_POLICER_MNG_CNTR_SET_ENT   mngCntrSet,
    IN PRV_TGF_POLICER_MNG_CNTR_TYPE_ENT  mngCntrType,
    IN PRV_TGF_POLICER_MNG_CNTR_ENTRY_STC *mngCntrPtr
);

/*******************************************************************************
* prvTgfPolicerManagementCountersGet
*
* DESCRIPTION:
*       Gets the value of specified Management Counters.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant for DxChXcat and above devices
*                         and ignored by DxCh3.
*       mngCntrSet      - Management Counters Set[0..2].
*       mngCntrType     - Management Counters Type (GREEN, YELLOW, RED, DROP).
*
* OUTPUTS:
*       mngCntrPtr      - pointer to the requested Management Counters Entry.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage or mngCntrType
*                                     or Management Counters Set number.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       In data unit management counter only 32 bits are used for DxCh3 devices
*       and 42 bits are used for DxChXcat and above devices.
*******************************************************************************/
GT_STATUS prvTgfPolicerManagementCountersGet
(
    IN GT_U8                               devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT     stage,
    IN  PRV_TGF_POLICER_MNG_CNTR_SET_ENT   mngCntrSet,
    IN  PRV_TGF_POLICER_MNG_CNTR_TYPE_ENT  mngCntrType,
    OUT PRV_TGF_POLICER_MNG_CNTR_ENTRY_STC *mngCntrPtr
);

/*******************************************************************************
* prvTgfPolicerEntrySet
*
* DESCRIPTION:
*      Set Policer Entry configuration
*
* INPUTS:
*       devNum     - device number
*       stage      - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                    Stage type is significant for DxChXcat and above devices
*                    and ignored by DxCh3.
*       entryIndex - policer entry index
*       entryPtr   - (pointer to) policer entry
*
* OUTPUTS:
*       tbParamsPtr - (pointer to) actual policer token bucket parameters
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong devNum, entryIndex or entry parameter
*       GT_BAD_PTR   - on one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerEntrySet
(
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT       stage,
    IN  GT_U32                               entryIndex,
    IN  PRV_TGF_POLICER_ENTRY_STC           *entryPtr,
    OUT PRV_TGF_POLICER_METER_TB_PARAMS_UNT *tbParamsPtr
);

/*******************************************************************************
* prvTgfPolicerEntryGet
*
* DESCRIPTION:
*      Get Policer Entry parameters
*
* INPUTS:
*       devNum     - device number
*       stage      - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                    Stage type is significant for DxChXcat and above devices
*                    and ignored by DxCh3.
*       entryIndex - policer entry index
*
* OUTPUTS:
*       entryPtr - (pointer to) policer entry
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong devNum or entryIndex
*       GT_BAD_PTR   - on one of the parameters is NULL pointer
*       GT_TIMEOUT   - on max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerEntryGet
(
    IN  GT_U8                          devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN  GT_U32                         entryIndex,
    OUT PRV_TGF_POLICER_ENTRY_STC     *entryPtr
);

/*******************************************************************************
* prvTgfPolicerEntryMeterParamsCalculate
*
* DESCRIPTION:
*      Calculates Token Bucket parameters in the Application format without
*      HW update.
*
* INPUTS:
*       devNum          - device number
*       tbInParamsPtr   - pointer to Token bucket input parameters.
*       meterMode       - Meter mode (SrTCM or TrTCM).
*
* OUTPUTS:
*       tbOutParamsPtr  - pointer to Token bucket output paramters.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_BAD_PARAM                - on wrong devNum or meterMode.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerEntryMeterParamsCalculate
(
    IN  GT_U8                                   devNum,
    IN  PRV_TGF_POLICER_METER_MODE_ENT       meterMode,
    IN  PRV_TGF_POLICER_METER_TB_PARAMS_UNT  *tbInParamsPtr,
    OUT PRV_TGF_POLICER_METER_TB_PARAMS_UNT  *tbOutParamsPtr
);

/*******************************************************************************
* prvTgfPolicerBillingEntrySet
*
* DESCRIPTION:
*       Sets Policer Billing Counters.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       entryIndex      - index of Policer Billing Counters Entry.
*       billingCntrPtr  - pointer to the Billing Counters Entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_TIMEOUT                  - on time out upon counter reset by
*                                     indirect access.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or entryIndex.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerBillingEntrySet
(
    IN  GT_U8                                   devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT         stage,
    IN  GT_U32                                  entryIndex,
    IN  PRV_TGF_POLICER_BILLING_ENTRY_STC    *billingCntrPtr
);

/*******************************************************************************
* prvTgfPolicerBillingEntryGet
*
* DESCRIPTION:
*       Gets Policer Billing Counters.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       entryIndex      - index of Policer Billing Counters Entry.
*       reset           - reset flag:
*                         GT_TRUE  - performing read and reset atomic operation.
*                         GT_FALSE - performing read counters operation only.
*
* OUTPUTS:
*       billingCntrPtr  - pointer to the Billing Counters Entry.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_TIMEOUT                  - on time out of IPLR Table indirect access.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or entryIndex.
*       GT_BAD_STATE                - on counter entry type mismatch.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerBillingEntryGet
(
    IN  GT_U8                                   devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT         stage,
    IN  GT_U32                                  entryIndex,
    IN  GT_BOOL                                 reset,
    OUT PRV_TGF_POLICER_BILLING_ENTRY_STC    *billingCntrPtr
);

/*******************************************************************************
* prvTgfPolicerEntryInvalidate
*
* DESCRIPTION:
*      Invalidate Policer Entry
*
* INPUTS:
*       devNum     - device number
*       entryIndex - policer entry index
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong devNum or entryIndex
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerEntryInvalidate
(
    IN  GT_U32                        entryIndex
);

/*******************************************************************************
* prvTgfPolicerQosRemarkingEntrySet
*
* DESCRIPTION:
*       Sets Policer Relative Qos Remarking Entry.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum                      - device number.
*       stage                       - Policer Stage type: Ingress #0 or Ingress #1.
*       qosProfileIndex             - index of Qos Remarking Entry will be set.
*       yellowQosTableRemarkIndex   - QoS profile (index in the Qos Table)
*                                     assigned to Out-Of-Profile packets with
*                                     the Yellow color. Index range is [0..127].
*       redQosTableRemarkIndex      - QoS profile (index in the Qos Table)
*                                     assigned to Out-Of-Profile packets with
*                                     the Red color. Index range is [0..127].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or qosProfileIndex.
*       GT_OUT_OF_RANGE             - on yellowQosTableRemarkIndex and
*                                     redQosTableRemarkIndex out of range.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerQosRemarkingEntrySet
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN GT_U32                         qosProfileIndex,
    IN GT_U32                         yellowQosTableRemarkIndex,
    IN GT_U32                         redQosTableRemarkIndex
);

/*******************************************************************************
* prvTgfPolicerQosRemarkingEntryGet
*
* DESCRIPTION:
*       Gets Policer Relative Qos Remarking Entry.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum              - device number.
*       stage               - Policer Stage type: Ingress #0 or Ingress #1.
*       qosProfileIndex     - index of requested Qos Remarking Entry.
*
* OUTPUTS:
*       yellowQosTableRemarkIndexPtr    - pointer to the QoS profile (index in
*                                         the Qos Table) assigned to
*                                         Out-Of-Profile packets with the
*                                         Yellow color. Index range is [0..127].
*       redQosTableRemarkIndexPtr       - pointer to the QoS profile (index in
*                                         the Qos Table) assigned to
*                                         Out-Of-Profile packets with the Red
*                                         color. Index range is [0..127].
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum or qosProfileIndex.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerQosRemarkingEntryGet
(
    IN  GT_U8                               devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT     stage,
    IN GT_U32                               qosProfileIndex,
    OUT GT_U32                              *yellowQosTableRemarkIndexPtr,
    OUT GT_U32                              *redQosTableRemarkIndexPtr
);

/*******************************************************************************
* prvTgfPolicerEgressQosRemarkingEntrySet
*
* DESCRIPTION:
*       Sets Egress Policer Re-Marking table Entry.
*
* INPUTS:
*       devNum              - device number.
*       remarkTableType     - Remark table type: DSCP, EXP or TC/UP.
*       remarkParamValue    - QoS parameter value.
*       confLevel           - Conformance Level: Green, Yellow, Red.
*       qosParamPtr         - pointer to the Re-Marking Entry going to be set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_HW_ERROR         - on hardware error.
*       GT_BAD_PARAM        - on wrong devNum, remarkTableType,
*                                remarkParamValue or confLevel.
*
*       GT_BAD_PTR                               - on NULL pointer.
*       GT_OUT_OF_RANGE     - on QoS parameter out of range
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerEgressQosRemarkingEntrySet
(
    IN PRV_TGF_POLICER_REMARK_TABLE_TYPE_ENT remarkTableType,
    IN GT_U32                                remarkParamValue,
    IN CPSS_DP_LEVEL_ENT                     confLevel,
    IN PRV_TGF_POLICER_QOS_PARAM_STC         *qosParamPtr
);

/*******************************************************************************
* prvTgfPolicerEgressQosRemarkingEntryGet
*
* DESCRIPTION:
*       Gets Egress Policer Re-Marking table Entry.
*
* INPUTS:
*       devNum              - device number.
*       remarkTableType     - Remark table type: DSCP, EXP or TC/UP.
*       remarkParamValue    - QoS parameter value.
*       confLevel           - Conformance Level: Green, Yellow, Red.
*
* OUTPUTS:
*       qosParamPtr         - pointer to the requested Re-Marking Entry.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_HW_ERROR         - on hardware error.
*       GT_BAD_PARAM        - on wrong devNum, remarkTableType,
*                                remarkParamValue or confLevel.
*
*       GT_BAD_PTR                               - on NULL pointer.
*       GT_BAD_STATE                            - on bad value in a entry.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerEgressQosRemarkingEntryGet
(
    IN  GT_U8                                 devNum,
    IN  PRV_TGF_POLICER_REMARK_TABLE_TYPE_ENT remarkTableType,
    IN  GT_U32                                remarkParamValue,
    IN  CPSS_DP_LEVEL_ENT                     confLevel,
    OUT PRV_TGF_POLICER_QOS_PARAM_STC         *qosParamPtr
);

/*******************************************************************************
* prvTgfPolicerStageMeterModeSet
*
* DESCRIPTION:
*       Sets Policer Global stage mode.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       mode            - Policer meter mode: FLOW or PORT.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or mode.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerStageMeterModeSet
(
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT       stage,
    IN  PRV_TGF_POLICER_STAGE_METER_MODE_ENT mode
);

/*******************************************************************************
* prvTgfPolicerStageMeterModeGet
*
* DESCRIPTION:
*       Gets Policer Global stage mode.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       modePtr         - (pointer to) Policer meter mode: FLOW or PORT.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_BAD_PTR                  - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerStageMeterModeGet
(
    IN  GT_U8                                       devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT             stage,
    OUT PRV_TGF_POLICER_STAGE_METER_MODE_ENT      *modePtr
);

/*******************************************************************************
* prvTgfPolicerMeteringOnTrappedPktsEnableSet
*
* DESCRIPTION:
*       Enable/Disable Ingress metering for Trapped packets.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0 or Ingress #1.
*       enable          - GT_TRUE - policing is executed on all packets (data
*                                   and control).
*                         GT_FALSE - policing is executed only on packets that
*                                    were asigned Forward (mirrored to analyzer
*                                    is inclusive) or Mirror to CPU commands by
*                                    previous processing stages.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or mode.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerMeteringOnTrappedPktsEnableSet
(
    IN  GT_U8                                   devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT         stage,
    IN GT_BOOL                                  enable
);

/*******************************************************************************
* prvTgfPolicerMeteringOnTrappedPktsEnableGet
*
* DESCRIPTION:
*       Get Ingress metering status (Enabled/Disabled) for Trapped packets.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0 or Ingress #1.
*
* OUTPUTS:
*       enablePtr       - GT_TRUE - policing is executed on all packets (data
*                                   and control).
*                         GT_FALSE - policing is executed only on packets that
*                                    were asigned Forward (mirrored to analyzer
*                                    is inclusive) or Mirror to CPU commands by
*                                    previous processing stages.
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_BAD_PTR                  - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerMeteringOnTrappedPktsEnableGet
(
    IN  GT_U8                               devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT     stage,
    OUT GT_BOOL                             *enablePtr
);

/*******************************************************************************
* prvTgfPolicerPortStormTypeIndexSet
*
* DESCRIPTION:
*       Associates policing profile with source/target port and storm rate type.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       portNum         - port number (including the CPU port).
*       stormType       - storm type
*       index           - policer index (0..3)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage, stormType ot port.
*       GT_OUT_OF_RANGE             - on out of range of index.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerPortStormTypeIndexSet
(
    IN  GT_U8                               devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT     stage,
    IN GT_U8                                portNum,
    IN PRV_TGF_POLICER_STORM_TYPE_ENT     stormType,
    IN GT_U32                               index
);


/*******************************************************************************
* prvTgfPolicerPortStormTypeIndexGet
*
* DESCRIPTION:
*       Get policing profile for given source/target port and storm rate type.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       portNum         - port number (including the CPU port).
*       stormType       - storm type
*
* OUTPUTS:
*       indexPtr        - pointer to policer index.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage, stormType or portNum.
*       GT_OUT_OF_RANGE             - on out of range of port.
*       GT_BAD_PTR                  - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerPortStormTypeIndexGet
(
    IN  GT_U8                               devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT    stage,
    IN  GT_U8                               portNum,
    IN  PRV_TGF_POLICER_STORM_TYPE_ENT    stormType,
    OUT GT_U32                              *indexPtr
);

/*******************************************************************************
* prvTgfPolicerEgressL2RemarkModelSet
*
* DESCRIPTION:
*       Sets Egress Policer L2 packets remarking model.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - physical device number.
*       model           - L2 packets remarking model. It defines QoS parameter,
*                         which used as index in the Qos table:
*                         User Priority or Traffic Class.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerEgressL2RemarkModelSet
(
    IN PRV_TGF_POLICER_L2_REMARK_MODEL_ENT model
);

/*******************************************************************************
* prvTgfPolicerEgressL2RemarkModelGet
*
* DESCRIPTION:
*       Gets Egress Policer L2 packets remarking model.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       modelPtr        - L2 packets remarking model. It defines QoS parameter,
*                         which used as index in the Qos table
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_BAD_PTR               - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerEgressL2RemarkModelGet
(
    IN  GT_U8                                    devNum,
    OUT PRV_TGF_POLICER_L2_REMARK_MODEL_ENT    *modelPtr
);

/*******************************************************************************
* prvTgfPolicerEgressQosUpdateEnableSet
*
* DESCRIPTION:
*       The function enables or disables QoS remarking of conforming packets.
*
* INPUTS:
*       devNum  - physical device number.
*       enable  - Enable/disable Qos update for conforming packets:
*                 GT_TRUE  - Remark Qos parameters of conforming packets.
*                 GT_FALSE - Keep incoming Qos parameters of conforming packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS prvTgfPolicerEgressQosUpdateEnableSet
(
    IN GT_BOOL  enable
);

/*******************************************************************************
* prvTgfPolicerEgressQosUpdateEnableGet
*
* DESCRIPTION:
*       The function get QoS remarking status of conforming packets
*
* INPUTS:
*       devNum  - physical device number.
*
* OUTPUTS:
*       enablePtr  - Enable/disable Qos update for conforming packets
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_BAD_PTR               - on NULL pointer.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS prvTgfPolicerEgressQosUpdateEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* prvTgfPolicerVlanCntrSet
*
* DESCRIPTION:
*      Sets Policer VLAN Counters located in the Policer Counting Entry upon
*      enabled VLAN counting mode.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       vid             - VLAN ID. Bits [1:0] of VID are used to select the
*                         relevant 32 bit counter inside the Counting Entry.
*       cntrValue       - Packet/Byte counter indexed by the packet's VID.
*                         For counter reset this parameter should be nullified.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on disabled VLAN Counting.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or vid (VID range is limited by max
*                         number of Policer Counting Entries) or stage.
*
* COMMENTS:
*       The VLAN counters are free-running no-sticky counters.
*
*******************************************************************************/
GT_STATUS prvTgfPolicerVlanCntrSet
(
    IN  GT_U8                          devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN  GT_U16                         vid,
    IN  GT_U32                         cntrValue
);

/*******************************************************************************
* prvTgfPolicerVlanCntrGet
*
* DESCRIPTION:
*      Gets Policer VLAN Counters located in the Policer Counting Entry upon
*      enabled VLAN counting mode.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       vid             - VLAN ID. Bits [1:0] of VID are used to select the
*                         relevant 32 bit counter inside the Counting Entry.
*
* OUTPUTS:
*       cntrValuePtr    - Packet/Byte counter indexed by the packet's VID.
*                         For counter reset this parameter should be nullified.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on disabled VLAN Counting.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or vid (VID range is limited by max
*                         number of Policer Counting Entries) or stage.
*       GT_BAD_PTR               - on NULL pointer.
*
* COMMENTS:
*       The VLAN counters are free-running no-sticky counters.
*
*******************************************************************************/
GT_STATUS prvTgfPolicerVlanCntrGet
(
    IN  GT_U8                          devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN  GT_U16                         vid,
    OUT GT_U32                         *cntrValuePtr
);

/*******************************************************************************
* prvTgfPolicerVlanCountingPacketCmdTriggerSet
*
* DESCRIPTION:
*       Enables or disables VLAN counting triggering according to the
*       specified packet command.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0 or Ingress #1.
*       cmdTrigger      - The packet command trigger.
*       enable          - Enable/Disable VLAN Counting according to the packet
*                         command trigger
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_SUPPORTED         - on not supported egress direction
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerVlanCountingPacketCmdTriggerSet
(
    IN GT_U8                          devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN CPSS_PACKET_CMD_ENT            cmdTrigger,
    IN GT_BOOL                        enable
);

/*******************************************************************************
* prvTgfPolicerVlanCountingPacketCmdTriggerGet
*
* DESCRIPTION:
*       Gets VLAN counting triggering status (Enable/Disable) according to the
*       specified packet command.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0 or Ingress #1.
*       cmdTrigger      - The packet command trigger.
*
* OUTPUTS:
*       enablePtr       - Pointer to Enable/Disable VLAN Counting according
*                         to the packet command trigger:
*                         GT_TRUE  - Enable VLAN counting on packet with
*                                    triggered packet command.
*                         GT_FALSE - Disable VLAN counting on packet with
*                                    triggered packet command.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_BAD_PTR               - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerVlanCountingPacketCmdTriggerGet
(
    IN  GT_U8                          devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN  CPSS_PACKET_CMD_ENT            cmdTrigger,
    OUT GT_BOOL                        *enablePtr
);

/*******************************************************************************
* prvTgfPolicerVlanCountingModeSet
*
* DESCRIPTION:
*       Sets the Policer VLAN counters mode as byte or packet based.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       mode            - VLAN counters mode: Byte or Packet based.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerVlanCountingModeSet
(
    IN GT_U8                                    devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT         stage,
    IN PRV_TGF_POLICER_VLAN_CNTR_MODE_ENT     mode
);

/*******************************************************************************
* prvTgfPolicerVlanCountingModeGet
*
* DESCRIPTION:
*       Gets the Policer VLAN counters mode as byte or packet based.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       modePtr         - (pointer to) VLAN counters mode: Byte or Packet based.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_BAD_PTR               - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPolicerVlanCountingModeGet
(
    IN  GT_U8                                    devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT         stage,
    OUT PRV_TGF_POLICER_VLAN_CNTR_MODE_ENT     *modePtr
);

/*******************************************************************************
* prvTgfPolicerPolicyCntrSet
*
* DESCRIPTION:
*      Sets Policer Policy Counters located in the Policer Counting Entry upon
*      enabled Policy Counting mode.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       index           - index set by Policy Action Entry or Metering entry
*       cntrValue       - packets counter.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on disabled Policy Counting.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or index (index range is limited by
*                         max number of Policer Policy counters).
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerPolicyCntrSet
(
    IN GT_U8                           devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN  GT_U32                         index,
    IN  GT_U32                         cntrValue
);

/*******************************************************************************
* prvTgfPolicerPolicyCntrGet
*
* DESCRIPTION:
*      Gets Policer Policy Counters located in the Policer Counting Entry upon
*      enabled Policy Counting mode.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       index           - index set by Policy Action Entry or Metering entry
*
* OUTPUTS:
*       cntrValuePtr    - Pointer to the packet counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on disabled Policy Counting.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or index.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_TIMEOUT               - on time out.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerPolicyCntrGet
(
    IN GT_U8                           devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN  GT_U32                         index,
    OUT GT_U32                         *cntrValuePtr
);

/*******************************************************************************
* prvTgfPolicerCountersSet
*
* DESCRIPTION:
*      Set Policer Counters. To reset counters use zero values.
*
* INPUTS:
*       devNum          - device number
*       counterSetIndex - policing counters set index
*       countersPtr     - (pointer to) counters
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong devNum or counterSetIndex
*       GT_BAD_PTR   - on one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerCountersSet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        counterSetIndex,
    IN  PRV_TGF_POLICER_COUNTERS_STC *countersPtr
);

/*******************************************************************************
* prvTgfPolicerCountersGet
*
* DESCRIPTION:
*      Get Policer Counters
*
* INPUTS:
*       devNum          - device number
*       counterSetIndex - policing counters set index
*
* OUTPUTS:
*       countersPtr - (pointer to) counters
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong devNum or counterSetIndex
*       GT_BAD_PTR   - on one of the parameters is NULL pointer
*       GT_TIMEOUT   - on max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerCountersGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        counterSetIndex,
    OUT PRV_TGF_POLICER_COUNTERS_STC *countersPtr
);

/*******************************************************************************
* prvTgfPolicerMemorySizeModeSet
*
* DESCRIPTION:
*      Sets internal table sizes and the way they are shared between the Ingress
*      policers.
*
* INPUTS:
*       devNum          - device number.
*       mode            - The mode in which internal tables are shared.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong input parameters.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerMemorySizeModeSet
(
    IN GT_U8                                devNum,
    IN PRV_TGF_POLICER_MEMORY_CTRL_MODE_ENT mode
);

/*******************************************************************************
* prvTgfPolicerMemorySizeModeGet
*
* DESCRIPTION:
*      Gets internal table sizes and the way they are shared between the Ingress
*      policers.
*
* INPUTS:
*       devNum          - device number.
*
* OUTPUTS:
*       modePtr         - The mode in which internal tables are shared.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_BAD_PTR               - on NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPolicerMemorySizeModeGet
(
    IN  GT_U8                                      devNum,
    OUT PRV_TGF_POLICER_MEMORY_CTRL_MODE_ENT     *modePtr
);

/*******************************************************************************
* prvTgfPolicerCountingWriteBackCacheFlush
*
* DESCRIPTION:
*       Flush internal Write Back Cache (WBC) of counting entries.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or stage.
*       GT_HW_ERROR              - on Hardware error.
*       GT_TIMEOUT               - on time out.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPolicerCountingWriteBackCacheFlush
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage
);

/*******************************************************************************
* prvTgfPolicerConformanceLevelForce
*
* DESCRIPTION:
*    force the conformance level for the packets entering the policer
*       (traffic cond)
* INPUTS:
*       dp -  conformance level (drop precedence) - green/yellow/red
*
* RETURNS:
*       GT_OK - success, GT_FAIL otherwise
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvTgfPolicerConformanceLevelForce(
    IN  GT_U32      dp
);

/*******************************************************************************
* prvTgfPolicerStageCheck
*
* DESCRIPTION:
*    check if the device supports this policer stage
* INPUTS:
*       stage -  policer stage to check
*
* RETURNS:
*       GT_TRUE - the device supports the stage
*       GT_FALSE - the device not supports the stage
*
* COMMENTS:
*
*
*******************************************************************************/
GT_BOOL prvTgfPolicerStageCheck(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage
);


/*******************************************************************************
* prvTgfPolicerPortModeAddressSelectSet
*
* DESCRIPTION:
*       Configure Metering Address calculation type.
*       Relevant when stage mode is CPSS_DXCH_POLICER_STAGE_METER_MODE_PORT_E. 
*
* INPUTS:
*       devNum      - device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       type        - Address select type: Full or Compressed.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or type.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPolicerPortModeAddressSelectSet
(
    IN  GT_U8                                               devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT                      stage,
    IN  PRV_TGF_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT      type
);

/*******************************************************************************
* prvTgfPolicerPortModeAddressSelectGet
*
* DESCRIPTION:
*       Get Metering Address calculation type.
*       Relevant when stage mode is CPSS_DXCH_POLICER_STAGE_METER_MODE_PORT_E. 
*
* INPUTS:
*       devNum      - device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       typePtr     - (pointer to) Address select type: Full or Compressed.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_BAD_PTR                  - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPolicerPortModeAddressSelectGet
(
    IN  GT_U8                                               devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT                      stage,
    OUT PRV_TGF_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT      *typePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __tgfPolicerGenh */

