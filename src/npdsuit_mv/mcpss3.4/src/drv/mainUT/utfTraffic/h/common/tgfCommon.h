/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfCommon.h
*
* DESCRIPTION:
*       Common helper API for enhanced UTs
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __tgfCommonh
#define __tgfCommonh

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/cos/cpssCosTypes.h>
#include <cpss/generic/port/cpssPortStat.h>

#include <trafficEngine/tgfTrafficEngine.h>
#include <trafficEngine/tgfTrafficGenerator.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************\
 *                     Default device\port numbers                            *
\******************************************************************************/
/* device number of first device */
#define PRV_TGF_DEV_0_CNS    0

/* max ports in test */
#define PRV_TGF_MAX_PORTS_NUM_CNS               10
/* default number for ports in test */
/* most tests written for 4 ports , but specific test
    can set it to higher value */
#define PRV_TGF_DEFAULT_NUM_PORTS_IN_TEST_CNS   4

/* ports array */
extern GT_U8    prvTgfPortsArray[];/* = {0, 8, 18, 23}; */

/* devices array */
extern GT_U8    prvTgfDevsArray[];/* = {0, 0,  0,  0}; */

/* number of ports/devices in arrays of :prvTgfPortsArray,prvTgfDevsArray */
extern GT_U8    prvTgfPortsNum;/* default value is PRV_TGF_DEFAULT_NUM_PORTS_IN_TEST_CNS
    max value allowed is PRV_TGF_MAX_PORTS_NUM_CNS */

/*
    structure: PRV_TGF_DEV_PORT_STC
    for dev,port pair
*/
typedef struct{
    GT_U8   devNum;
    GT_U8   portNum;
}PRV_TGF_DEV_PORT_STC;

/*
    structure: PRV_TGF_MEMBER_FORCE_INFO_STC

    support ring ports that need to be in all vlan/mc groups.
        NOTE: in lion A those port must NOT be in vidx !
    support cascade ports that need to be in all vlan/mc groups.

    fields:
    member - {dev,port} that we refer to.
    forceToVlan - do want to force the member to vlan.
    forceToVidx - do we want to force the member to vidx.
    vlanInfo - info relate only to vlans.
        .tagged - the member tagged or untagged.
            relevant to devices without tr101 support.
        .tagCmd - tag command. the member's tag command.
            relevant to devices with tr101 support.
            hold value of one of : PRV_TGF_BRG_VLAN_PORT_TAG_CMD_ENT

*/
typedef struct{
    PRV_TGF_DEV_PORT_STC    member;
    GT_BOOL     forceToVlan;
    GT_BOOL     forceToVidx;
    struct{
        GT_BOOL                                        tagged;
        GT_U32 /*PRV_TGF_BRG_VLAN_PORT_TAG_CMD_ENT*/   tagCmd;
    }vlanInfo;
}PRV_TGF_MEMBER_FORCE_INFO_STC;

/* max number of ports in devices that must be added to all vlans and MC (vidx) groups */
#define PRV_TGF_FORCE_MEMBERS_COUNT_CNS     128

/* list of ports in devices that must be added to all vlans and MC (vidx) groups */
extern PRV_TGF_MEMBER_FORCE_INFO_STC  prvTgfDevPortForceArray[/*PRV_TGF_FORCE_MEMBERS_COUNT_CNS*/];
/* number of used ports/devices in arrays of :prvTgfDevPortForceArray[] */
extern GT_U32    prvTgfDevPortForceNum;

/* enum for indexes to access ports through arrays prvTgfDevsArray & prvTgfPortsArray */
enum{
    U1_INDEX_E = 0,
    U2_INDEX_E,
    U3_INDEX_E,
    U4_INDEX_E,
    N1_INDEX_E,
    N2_INDEX_E,
    N3_INDEX_E,
    N4_INDEX_E
};

/* enum for port device type */
typedef enum
{
    PRV_TGF_28_PORT_DEVICE_TYPE_E = 0,
    PRV_TGF_60_PORT_DEVICE_TYPE_E,

    PRV_TGF_DEVICE_TYPE_COUNT_E
} PRV_TGF_DEVICE_TYPE_ENT;

/*
 * typedef: PRV_TGF_TX_MODE_ENT
 *
 * Description:
 *      The Tx mode of packets engine.
 * Fields:
 *    PRV_TGF_TX_MODE_SINGLE_BURST_E - packet engine sends single burst of 
 *                                   packets. The prvTgfSetTxSetupEth configures
 *                                   burst size.
 *    PRV_TGF_TX_MODE_CONTINUOUS_E    - packet engine send packets continuously.
 *                                   The prvTgfStopTransmitingEth stops transmission.
 *
 * Comment:
 *   The continuous mode use Rx Mirroring for traffic generation. The source 
 *   based mirroring is used. Up to 7 ports may be used to transmission.
 */
typedef enum{
    PRV_TGF_TX_MODE_SINGLE_BURST_E,
    PRV_TGF_TX_MODE_CONTINUOUS_E
}PRV_TGF_TX_MODE_ENT;

/*
 * typedef: PRV_TGF_TX_CONT_MODE_UNIT_ENT
 *
 * Description:
 *      The Tx continuouse mode units type.
 * Fields:
 *      PRV_TGF_TX_CONT_MODE_UNIT_WS_E - wirespeed traffic with port's line rate
 *      PRV_TGF_TX_CONT_MODE_UNIT_KBPS_SEC_E - kilo bits per second rate units
 *      PRV_TGF_TX_CONT_MODE_UNIT_PERCENT_E - percent of line rate units
 *
 * Comment:
 */
typedef enum{
    PRV_TGF_TX_CONT_MODE_UNIT_WS_E,
    PRV_TGF_TX_CONT_MODE_UNIT_KBPS_SEC_E,
    PRV_TGF_TX_CONT_MODE_UNIT_PERCENT_E
}PRV_TGF_TX_CONT_MODE_UNIT_ENT;

/* use this constant when need exact size in compilation time */
#define PRV_TGF_PORTS_NUM_CNS   4

/* macro to verify return code */
#define PRV_UTF_VERIFY_GT_OK(rc, msg)                                        \
    if (GT_OK != rc)                                                         \
    {                                                                        \
        PRV_UTF_LOG2_MAC("\n[TGF]: %s FAILED, rc = [%d]\n", msg, rc);        \
        return rc;                                                           \
    }

#define PRV_UTF_VERIFY_RC1(rc, name)                                           \
    if (GT_OK != (rc))                                                         \
    {                                                                          \
        PRV_UTF_LOG2_MAC("\n[TGF]: %s FAILED, rc = [%d]\n", name, rc);         \
        rc1 = rc;                                                              \
    }


/******************************************************************************\
 *                          Private type definitions                          *
\******************************************************************************/

/*
 * Typedef: enum PRV_TGF_OUTLIF_TYPE_ENT
 *
 * Description: enumerator for outlif type
 *
 * Fields:
 *      PRV_TGF_OUTLIF_TYPE_LL_E     - outlif is a regular ethernet interface
 *      PRV_TGF_OUTLIF_TYPE_DIT_E    - outlif is a DIT entry
 *      PRV_TGF_OUTLIF_TYPE_TUNNEL_E - outlif is a tunnel
 */
typedef enum {
    PRV_TGF_OUTLIF_TYPE_LL_E,
    PRV_TGF_OUTLIF_TYPE_DIT_E,
    PRV_TGF_OUTLIF_TYPE_TUNNEL_E
} PRV_TGF_OUTLIF_TYPE_ENT;

/*
 * typedef: enum PRV_TGF_TS_PASSENGER_PACKET_TYPE_ENT
 *
 * Description:
 *      Tunnel Start passenger protocol types
 *
 * Enumerations:
 *      PRV_TGF_TS_PASSENGER_PACKET_TYPE_ETHERNET_E - ethernet
 *      PRV_TGF_TS_PASSENGER_PACKET_TYPE_OTHER_E    - other (IP or MPLS)
 *
 */
typedef enum
{
    PRV_TGF_TS_PASSENGER_PACKET_TYPE_ETHERNET_E,
    PRV_TGF_TS_PASSENGER_PACKET_TYPE_OTHER_E
} PRV_TGF_TS_PASSENGER_PACKET_TYPE_ENT;

/*
 * Typedef: structure PRV_TGF_OUTLIF_INFO_STC
 *
 * Description: Defines the interface info
 *
 * Fields:
 *      outlifType          - the outlif type (Link Layer/DIT/Tunnel)
 *      interfaceInfo       - interface info (devPort, trunk, vlan, vidx, device)
 *      arpPtr              - pointer to an ARP pointer
 *      ditPtr              - pointer to a DIT entry
 *      tunnelStartPtr      - tunnel start information
 *      passengerPacketType - the packet type
 *      ptr                 - pointer to a tunnel entry
 *
 */
typedef struct
{
    PRV_TGF_OUTLIF_TYPE_ENT   outlifType;

    CPSS_INTERFACE_INFO_STC   interfaceInfo;

    union {
        GT_U32  arpPtr;
        GT_U32  ditPtr;

        struct {
            PRV_TGF_TS_PASSENGER_PACKET_TYPE_ENT passengerPacketType;
            GT_U32                               ptr;
        } tunnelStartPtr;

    } outlifPointer;

} PRV_TGF_OUTLIF_INFO_STC;

/*
 * typedef: PRV_TGF_QOS_PARAM_MODIFY_STC
 *
 * Description: QoS modify parameters
 *
 * Fields:
 *    modifyTc   - enable/disable modifing traffic class
 *    modifyUp   - enable/disable modifing IEEE 802.1p User Priority
 *    modifyDp   - enable/disable modifing Drop Precedence
 *    modifyDscp - enable/disable modifing IP DSCP field
 *    modifyExp  - enable/disable modifing MPLS label EXP value
 */
typedef struct
{
    GT_BOOL             modifyTc;
    GT_BOOL             modifyUp;
    GT_BOOL             modifyDp;
    GT_BOOL             modifyDscp;
    GT_BOOL             modifyExp;
} PRV_TGF_QOS_PARAM_MODIFY_STC;

/*
 * typedef: PRV_TGF_QOS_PARAM_STC
 *
 * Description: The Quality of Service parameters
 *
 * Fields:
 *    tc   - egress TX traffic class queue
 *    dp   - Drop Precedence (color) (Green, Yellow or Red)
 *    up   - IEEEE 802.1p User Priority
 *    dscp - IP DCSP field
 *    exp  - MPLS label EXP value
 */
typedef struct
{
    GT_U32                  tc;
    CPSS_DP_LEVEL_ENT       dp;
    GT_U32                  up;
    GT_U32                  dscp;
    GT_U32                  exp;
} PRV_TGF_QOS_PARAM_STC;


/******************************************************************************\
 *                              Marco definitions                             *
\******************************************************************************/

/* copy field from source structure into destination structure  */
#define PRV_TGF_STC_S2D_FIELD_COPY_MAC(dstStcPtr, srcStcPtr, field)            \
        (dstStcPtr)->field = (srcStcPtr)->field

/* copy field from destination structure into source structure  */
#define PRV_TGF_STC_D2S_FIELD_COPY_MAC(dstStcPtr, srcStcPtr, field)            \
        (srcStcPtr)->field = (dstStcPtr)->field

/* case to convert from\into generic values */
#define PRV_TGF_SWITCH_CASE_MAC(varName, val1, val2)                           \
    case val1:                                                                 \
    {                                                                          \
        varName = val2;                                                        \
        break;                                                                 \
    }

/* macro to check if equal expected and recieved port's counters */
#define PRV_TGF_VERIFY_COUNTERS_MAC(isEqual, expCntrs, rcvCntrs)               \
    isEqual = rcvCntrs.goodOctetsSent.l[0] == expCntrs.goodOctetsSent.l[0] &&  \
              rcvCntrs.goodPktsSent.l[0]   == expCntrs.goodPktsSent.l[0] &&    \
              rcvCntrs.ucPktsSent.l[0]     == expCntrs.ucPktsSent.l[0] &&      \
              rcvCntrs.brdcPktsSent.l[0]   == expCntrs.brdcPktsSent.l[0] &&    \
              rcvCntrs.mcPktsSent.l[0]     == expCntrs.mcPktsSent.l[0] &&      \
              rcvCntrs.goodOctetsRcv.l[0]  == expCntrs.goodOctetsRcv.l[0] &&   \
              rcvCntrs.goodPktsRcv.l[0]    == expCntrs.goodPktsRcv.l[0] &&     \
              rcvCntrs.ucPktsRcv.l[0]      == expCntrs.ucPktsRcv.l[0] &&       \
              rcvCntrs.brdcPktsRcv.l[0]    == expCntrs.brdcPktsRcv.l[0] &&     \
              rcvCntrs.mcPktsRcv.l[0]      == expCntrs.mcPktsRcv.l[0]

/* macro to print port's counters that not match the expected and received */
#define PRV_TGF_PRINT_DIFF_COUNTERS_MAC(isEqual, expCntrs, rcvCntrs)           \
    if(isEqual == 0)                                                                                                    \
    {/* at least one counter not match */                                                                               \
        PRV_UTF_LOG0_MAC(" counter name ,   expected , received \n");                                                   \
        PRV_UTF_LOG0_MAC(" ==================================== \n");                                                   \
        if(rcvCntrs.goodOctetsSent.l[0] != expCntrs.goodOctetsSent.l[0])                                                \
            PRV_UTF_LOG2_MAC("  goodOctetsSent , %d , %d \n", expCntrs.goodOctetsSent.l[0],rcvCntrs.goodOctetsSent.l[0]);\
        if(rcvCntrs.goodPktsSent.l[0]   != expCntrs.goodPktsSent.l[0])                                                  \
            PRV_UTF_LOG2_MAC("  goodPktsSent   , %d , %d \n", expCntrs.goodPktsSent.l[0],rcvCntrs.goodPktsSent.l[0]);   \
        if(rcvCntrs.ucPktsSent.l[0]     != expCntrs.ucPktsSent.l[0])                                                    \
            PRV_UTF_LOG2_MAC("  ucPktsSent     , %d , %d \n", expCntrs.ucPktsSent.l[0],rcvCntrs.ucPktsSent.l[0]);       \
        if(rcvCntrs.brdcPktsSent.l[0]   != expCntrs.brdcPktsSent.l[0])                                                  \
            PRV_UTF_LOG2_MAC("  brdcPktsSent   , %d , %d \n", expCntrs.brdcPktsSent.l[0],rcvCntrs.brdcPktsSent.l[0]);   \
        if(rcvCntrs.mcPktsSent.l[0]   != expCntrs.mcPktsSent.l[0])                                                      \
            PRV_UTF_LOG2_MAC("  mcPktsSent     , %d , %d \n", expCntrs.mcPktsSent.l[0],rcvCntrs.mcPktsSent.l[0]);       \
        if(rcvCntrs.goodOctetsRcv.l[0]   != expCntrs.goodOctetsRcv.l[0])                                                \
            PRV_UTF_LOG2_MAC("  goodOctetsRcv  , %d , %d \n", expCntrs.goodOctetsRcv.l[0],rcvCntrs.goodOctetsRcv.l[0]); \
        if(rcvCntrs.goodPktsRcv.l[0]   != expCntrs.goodPktsRcv.l[0])                                                    \
            PRV_UTF_LOG2_MAC("  goodPktsRcv    , %d , %d \n", expCntrs.goodPktsRcv.l[0],rcvCntrs.goodPktsRcv.l[0]);     \
        if(rcvCntrs.ucPktsRcv.l[0]   != expCntrs.ucPktsRcv.l[0])                                                        \
            PRV_UTF_LOG2_MAC("  ucPktsRcv      , %d , %d \n", expCntrs.ucPktsRcv.l[0],rcvCntrs.ucPktsRcv.l[0]);         \
        if(rcvCntrs.brdcPktsRcv.l[0]   != expCntrs.brdcPktsRcv.l[0])                                                    \
            PRV_UTF_LOG2_MAC("  brdcPktsRcv    , %d , %d \n", expCntrs.brdcPktsRcv.l[0],rcvCntrs.brdcPktsRcv.l[0]);     \
        if(rcvCntrs.mcPktsRcv.l[0]   != expCntrs.mcPktsRcv.l[0])                                                        \
            PRV_UTF_LOG2_MAC("  mcPktsRcv      , %d , %d \n", expCntrs.mcPktsRcv.l[0],rcvCntrs.mcPktsRcv.l[0]);         \
        PRV_UTF_LOG0_MAC("\n");                                                                                         \
    }

/*
 * typedef: PRV_TGF_MULTI_DESTINATION_TYPE_ENT
 *
 * Description: enum for types of multi-destination sending
 *
 * Values:
 *    PRV_TGF_MULTI_DESTINATION_TYPE_BC_E  - BC with incremental SA
 *    PRV_TGF_MULTI_DESTINATION_TYPE_MC_E  - MC with incremental DA
 *    PRV_TGF_MULTI_DESTINATION_TYPE_UNKNOWN_UC_SA_INCREMENT_E  - unknown UC with incremental SA
 *    PRV_TGF_MULTI_DESTINATION_TYPE_UNKNOWN_UC_SA_INCREMENT_E  - unknown UC with incremental DA
 *    PRV_TGF_MULTI_DESTINATION_TYPE_ALL_E - all type
 */
typedef enum {
    PRV_TGF_MULTI_DESTINATION_TYPE_BC_E,
    PRV_TGF_MULTI_DESTINATION_TYPE_MC_E,
    PRV_TGF_MULTI_DESTINATION_TYPE_UNKNOWN_UC_SA_INCREMENT_E,
    PRV_TGF_MULTI_DESTINATION_TYPE_UNKNOWN_UC_DA_INCREMENT_E,

    PRV_TGF_MULTI_DESTINATION_TYPE_ALL_E

}PRV_TGF_MULTI_DESTINATION_TYPE_ENT;

/******************************************************************************\
 *                       Public functions section                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfResetCountersEth
*
* DESCRIPTION:
*       Reset port counters and set this port in linkup mode
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfResetCountersEth
(
    IN GT_U8                devNum,
    IN GT_U8                portNum
);

/*******************************************************************************
* prvTgfSetTxSetupEth
*
* DESCRIPTION:
*       Setting transmit parameters
*
* INPUTS:
*       devNum        - the CPU device number
*       packetInfoPtr - (pointer to) the packet info
*       burstCount    - number of frames
*       numVfd        - number of VFDs
*       vfdArray      - (pointer to) array of VFDs
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfSetTxSetupEth
(
    IN GT_U8                devNum,
    IN TGF_PACKET_STC      *packetInfoPtr,
    IN GT_U32               burstCount,
    IN GT_U32               numVfd,
    IN TGF_VFD_INFO_STC     vfdArray[]
);

/*******************************************************************************
* prvTgfSetTxSetup2Eth
*
* DESCRIPTION:
*       Setting transmit 2 parameters .
*       NOTE:
*           This function should be called after prvTgfSetTxSetupEth(..) ,
*           when need to set the parameters of this function.
*
* INPUTS:
*       sleepAfterXCount - do 'sleep' after X packets sent
*                          when = 0 , meaning NO SLEEP needed during the burst
*                          of 'burstCount'
*       sleepTime        - sleep time (in milliseconds) after X packets sent , see
*                          parameter sleepAfterXCount
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfSetTxSetup2Eth
(
    IN GT_U32               sleepAfterXCount,
    IN GT_U32               sleepTime
);

/*******************************************************************************
* prvTgfBurstTraceSet
*
* DESCRIPTION:
*       Set max number of packets in burst count that will be printed
*
* INPUTS:
*       packetCount - disable logging when burst is lager then packetCount
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success.
*
* COMMENTS:
*       To restore default setting call function with packetCount == 0
*
*******************************************************************************/
GT_STATUS prvTgfBurstTraceSet
(
    IN GT_U32       packetCount
);

/*******************************************************************************
* prvTgfStartTransmitingEth
*
* DESCRIPTION:
*       Transmit packet
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfStartTransmitingEth
(
    IN GT_U8                devNum,
    IN GT_U8                portNum
);

/*******************************************************************************
* prvTgfStopTransmitingEth
*
* DESCRIPTION:
*       Stop transmit packets
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfStopTransmitingEth
(
    IN GT_U8                devNum,
    IN GT_U8                portNum
);

/*******************************************************************************
* prvTgfTxModeSetupEth
*
* DESCRIPTION:
*       Transmit mode configuration
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       mode    - Tx mode
*       unitsType - Tx units type
*       units     - Tx units values
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTxModeSetupEth
(
    IN GT_U8                            devNum,
    IN GT_U8                            portNum,
    IN PRV_TGF_TX_MODE_ENT              mode,
    IN PRV_TGF_TX_CONT_MODE_UNIT_ENT    unitsType,
    IN GT_U32                           units
);

/*******************************************************************************
* prvTgfReadPortCountersEth
*
* DESCRIPTION:
*       Read port's counters
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       enablePrint - enable\disable counters print
*
* OUTPUTS:
*       portCountersPtr - (pointer to) received counters values
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_FAIL      - if test doesn't pass comparing.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfReadPortCountersEth
(
    IN  GT_U8                          devNum,
    IN  GT_U8                          portNum,
    IN  GT_BOOL                        enablePrint,
    OUT CPSS_PORT_MAC_COUNTER_SET_STC *portCountersPtr
);

/*******************************************************************************
* prvTgfPacketSizeGet
*
* DESCRIPTION:
*       Calculate packet size
*
* INPUTS:
*       partsArray - packet part's array
*       partsCount - number of parts
*
* OUTPUTS:
*       packetSize - (pointer to) packet size
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_FAIL      - if test doesn't pass comparing.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPacketSizeGet
(
    IN  TGF_PACKET_PART_STC  partsArray[],
    IN  GT_U32               partsCount,
    OUT GT_U32              *packetSizePtr
);

/*******************************************************************************
* prvTgfPortCapturedPacketPrint
*
* DESCRIPTION:
*       Print captured packet on specific port
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - wrong interface
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPortCapturedPacketPrint
(
    IN GT_U8                devNum,
    IN GT_U8                portNum
);

/*******************************************************************************
* prvTgfEthCountersCheck
*
* DESCRIPTION:
*       Read and check port's counters
*
* INPUTS:
*       devNum         - device number
*       portNum        - port number
*       packetsCountRx - Expected number of Rx packets
*       packetsCountTx - Expected number of Tx packets
*       packetSize     - Size of packets
*       burstCount     - Number of packets with the same size
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - wrong interface
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfEthCountersCheck
(
    IN GT_U8  devNum,
    IN GT_U8  portNum,
    IN GT_U8  packetsCountRx,
    IN GT_U8  packetsCountTx,
    IN GT_U32 packetSize,
    IN GT_U32 burstCount
);

/*******************************************************************************
* prvTgfEthCountersReset
*
* DESCRIPTION:
*       Reset all Ethernet port's counters
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong interface
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfEthCountersReset
(
    IN GT_U8 devNum
);

/*******************************************************************************
* prvTgfCommonMultiDestinationTrafficSend
*
* DESCRIPTION:
*   Send multi-destination traffic , meaning from requested port:
*       Send BC (FF:FF:FF:FF:FF:FF) with burstNum incremental SA.
*       Send unregistered MC (01:02:03:04:05:06) with burstNum incremental SA.
*       Send unknown unicast (00:09:99:99:99:99) with burstNum incremental DA and static SA
*       Send unknown unicast (00:09:99:99:99:99) with burstNum static DA and incremental SA
*
*
* INPUTS:
*       devNum       - the device to send traffic from.
*       portNum      - the port to send traffic from.
*       burstNum     - number of packets to send in the burst
*       trace        - enable\disable packet tracing
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
void prvTgfCommonMultiDestinationTrafficSend
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_U32  burstNum,
    IN  GT_BOOL trace
);

/*******************************************************************************
* prvTgfCommonMultiDestinationTrafficCounterCheck
*
* DESCRIPTION:
*   check that traffic counter match the needed value
*
* INPUTS:
*       devNum - device number
*       portNum - port number
*       expectedCounterValue - expected counter value (counter of goodPktsSent.l[0])
*
* OUTPUTS:
*       devNum - device number
*       portNum - port number
*       expectedCounterValue - expected counter value (counter of goodPktsSent.l[0])
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
void prvTgfCommonMultiDestinationTrafficCounterCheck
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_U32   expectedCounterValue
);

/*******************************************************************************
* prvTgfCommonIncrementalSaMacSend
*
* DESCRIPTION:
*   Send packets with incremental SA mac address (that start from specified mac
*   address)
*   -- this used for 'mac incremental learning'
*
* INPUTS:
*       devNum       - the device to send traffic from.
*       portNum      - the port to send traffic from.
*       saMacPtr     - (pointer to) the base mac address (of source mac)
*       burstNum     - number of packets to send in the burst
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
void prvTgfCommonIncrementalSaMacSend
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  TGF_MAC_ADDR    *saMacPtr,
    IN  GT_U32  burstNum
);

/*******************************************************************************
* prvTgfCommonMultiDestinationTrafficSend
*
* DESCRIPTION:
*   Send packets with incremental SA mac address .
*   this start from mac address of:    [00:00:port:port:port:port]
*   For example :
*   for port 0x22     --> [00:00:22:22:22:22]
*   and for port 0x6  --> [00:00:06:06:06:06]
*
*   -- this used for 'mac incremental learning'
*
* INPUTS:
*       devNum       - the device to send traffic from.
*       portNum      - the port to send traffic from.
*       burstNum     - number of packets to send in the burst
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
void prvTgfCommonIncrementalSaMacByPortNumSend
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_U32  burstNum
);


/*******************************************************************************
* prvTgfCommonIncrementalDaMacSend
*
* DESCRIPTION:
*   Send packets with incremental DA mac address (that start from specified mac
*   address) and static SA mac address
*   -- this used for checking the already learned mac addresses
*
* INPUTS:
*       devNum       - the device to send traffic from.
*       portNum      - the port to send traffic from.
*       saMacPtr     - (pointer to) the SA mac address
*       daMacPtr     - (pointer to) the DA base mac address
*       burstNum     - number of packets to send in the burst
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
void prvTgfCommonIncrementalDaMacSend
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  TGF_MAC_ADDR    *saMacPtr,
    IN  TGF_MAC_ADDR    *daMacPtr,
    IN  GT_U32  burstNum
);

/*******************************************************************************
* prvTgfCommonIncrementalDaMacByPortNumSend
*
* DESCRIPTION:
*   Send packets with incremental DA mac address (according to dstPortNum)
*   and static SA mac address (according to portNum)
*
*   DA start from mac address of:    [00:00:dstPortNum:dstPortNum:dstPortNum:dstPortNum]
*   For example :
*   for port 0x22     --> [00:00:22:22:22:22]
*   and for port 0x6  --> [00:00:06:06:06:06]
*
*   SA start from mac address of:    [00:00:portNum:portNum:portNum:portNum]
*   For example :
*   for port 0x33     --> [00:00:33:33:33:33]
*   and for port 0x8  --> [00:00:08:08:08:08]
*
*   -- this used for checking the already learned mac addresses
*
* INPUTS:
*       devNum       - the device to send traffic from.
*       portNum      - the port to send traffic from.
*                      this port number also used for SA mac address
*       dstPortNum   - the alleged port number to receive the traffic
*                      this port number used for base DA mac address
*       daMacPtr     - (pointer to) the DA base mac address
*       burstNum     - number of packets to send in the burst
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
void prvTgfCommonIncrementalDaMacByPortNumSend
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_U8   dstPortNum,
    IN  GT_U32  burstNum
);

/*******************************************************************************
* prvTgfCommonMultiDestinationTrafficTypeSend
*
*
* DESCRIPTION:
*   like prvTgfCommonMultiDestinationTrafficSend it :
*   Send multi-destination traffic , meaning from requested port:
*       Send BC (FF:FF:FF:FF:FF:FF) with burstNum incremental SA.
*       Send unregistered MC (01:02:03:04:05:06) with burstNum incremental SA.
*       Send unknown unicast (00:09:99:99:99:99) with burstNum incremental DA and static SA
*       Send unknown unicast (00:09:99:99:99:99) with burstNum static DA and incremental SA
*
*   BUT the packet itself (beside the SA,DA) is given by the caller.

*
* INPUTS:
*       devNum       - the device to send traffic from.
*       portNum      - the port to send traffic from.
*       burstNum     - number of packets to send in the burst
*       trace        - enable\disable packet tracing
*       packetPtr    - pointer to the packet
*       type         - multi-destination type of sending
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
void prvTgfCommonMultiDestinationTrafficTypeSend
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_U32  burstNum,
    IN  GT_BOOL trace,
    IN  TGF_PACKET_STC *packetPtr,
    IN  PRV_TGF_MULTI_DESTINATION_TYPE_ENT  type
);

/*******************************************************************************
* prvTgfTransmitPacketsWithCapture
*
* DESCRIPTION:
*       None
*
* INPUTS:
*       inDevNum      - ingress device number
*       inPortNum     - ingress port number
*       packetInfoPtr - (pointer to) the packet info
*       burstCount    - number of frames
*       numVfd        - number of VFDs
*       vfdArray      - (pointer to) array of VFDs
*       outDevNum     - egress device number
*       outPortNum    - egress port number
*       captureMode   - packet capture mode
*       captureOnTime - time the capture will be enabe (in milliseconds)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTransmitPacketsWithCapture
(
    IN GT_U8                inDevNum,
    IN GT_U8                inPortNum,
    IN TGF_PACKET_STC       *packetInfoPtr,
    IN GT_U32               burstCount,
    IN GT_U32               numVfd,
    IN TGF_VFD_INFO_STC     vfdArray[],
    IN GT_U8                outDevNum,
    IN GT_U8                outPortNum,
    IN TGF_CAPTURE_MODE_ENT captureMode,
    IN GT_U32               captureOnTime
);

/*******************************************************************************
* prvTgfDefPortsArraySet
*
* DESCRIPTION:
*       Set predefined number of ports and port's array
*
* INPUTS:
*       portsArray - array of ports
*       arraySize  - number of ports
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_SIZE  - wrong array size
*       GT_BAD_PTR   - wrong pointer
*
* COMMENTS:
*       Call this function each time when needs to override default ports array
*       by specific values
*
*******************************************************************************/
GT_STATUS prvTgfDefPortsArraySet
(
    IN GT_U8    portsArray[],
    IN GT_U8    arraySize
);

/*******************************************************************************
* prvTgfPortsArrayByDevTypeSet
*
* DESCRIPTION:
*       Override default ports from prvTgfPortsArray by new ports from
*       prvTgfDefPortsArray depending on device type
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfPortsArrayByDevTypeSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPortsArrayPrint
*
* DESCRIPTION:
*       Print number of used ports and port's array prvTgfPortsArray
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfPortsArrayPrint
(
    GT_VOID
);

/*******************************************************************************
* prvTgfCommonInit
*
* DESCRIPTION:
*       Initialize TFG configuration
*
* INPUTS:
*       firstDevNum - device number of first device.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       Put all necessary initializations into this function
*
*******************************************************************************/
GT_STATUS prvTgfCommonInit
(
    GT_U8 firstDevNum
);

/*******************************************************************************
* prvTgfCommonMemberForceInfoSet
*
* DESCRIPTION:
*       add member info to DB of 'members to force to vlan/mc groups'
*       in case that the member already exists {dev,port} the other parameters
*       are 'updated' according to last setting.
*
* INPUTS:
*       memberInfoPtr - (pointer to) member info
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FULL - the DB is full and no more members can be added -->
*                 need to make DB larger.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvTgfCommonMemberForceInfoSet(
    IN PRV_TGF_MEMBER_FORCE_INFO_STC    *memberInfoPtr
);

/*******************************************************************************
* prvTgfCommonIsDeviceForce
*
* DESCRIPTION:
*       check if the device has ports that used with force configuration
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_TRUE  - the device force configuration
*       GT_FALSE - the device NOT force configuration
*
* COMMENTS:
*
*******************************************************************************/
GT_BOOL prvTgfCommonIsDeviceForce(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvTgfCommonIsUseLastTpid
*
* DESCRIPTION:
*       check if the test use last TPIDs for ingress
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_TRUE  - the device  use last TPIDs for ingress
*       GT_FALSE - the device  NOT use last TPIDs for ingress
*
* COMMENTS:
*
*******************************************************************************/
GT_BOOL prvTgfCommonIsUseLastTpid(
    void
);

/*******************************************************************************
* prvTgfCommonIsUseLastTpid
*
* DESCRIPTION:
*       state that test use last TPIDs for ingress or not
*
* INPUTS:
*       enable
*           GT_TRUE  - the device  use last TPIDs for ingress
*           GT_FALSE - the device  NOT use last TPIDs for ingress
*
* OUTPUTS:
*       None
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
void prvTgfCommonUseLastTpid(
    IN GT_BOOL  enable
);

/*******************************************************************************
* prvTgfCommonPortTxTwoUcTcRateGet
*
* DESCRIPTION:
*       Get rate of two unicast flows by egress counters.
*
* INPUTS:
*       portIdx - index of egress port
*       tc1     - traffic class of first unicast flow
*       tc2     - traffic class of second unicast flow
*       timeOut - timeout in milli seconds to calculate rate
*
* OUTPUTS:
*       outRate1Ptr - (pointer to) Tx rate of first flow
*       outRate1Ptr - (pointer to) Tx rate of second flow
*       dropRate1Ptr - (pointer to) drop rate of first flow
*       dropRate1Ptr - (pointer to) drop rate of second flow
*
* RETURNS:
*       none
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfCommonPortTxTwoUcTcRateGet
(
    IN  GT_U32 portIdx,
    IN  GT_U32 tc1,
    IN  GT_U32 tc2,
    IN  GT_U32 timeOut,
    OUT GT_U32 *outRate1Ptr,
    OUT GT_U32 *outRate2Ptr,
    OUT GT_U32 *dropRate1Ptr,
    OUT GT_U32 *dropRate2Ptr
);

/*******************************************************************************
* prvTgfCommonPortCntrRateGet
*
* DESCRIPTION:
*       Get rate of specific port MAC MIB counter.
*
* INPUTS:
*       portIdx - index of egress port
*       cntrName - name of counter
*       timeOut - timeout in milli seconds to calculate rate
*
* OUTPUTS:
*       outRatePtr - (pointer to) rate of port's counter
*
* RETURNS:
*       none
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfCommonPortCntrRateGet
(
    IN  GT_U32                      portIdx,
    IN  CPSS_PORT_MAC_COUNTERS_ENT  cntrName,
    IN  GT_U32                      timeOut,
    OUT GT_U32                      *outRatePtr
);

/*******************************************************************************
* prvTgfCommonPortTxRateGet
*
* DESCRIPTION:
*       Get Tx rate of port by MAC MIB counters.
*
* INPUTS:
*       portIdx - index of egress port
*       timeOut - timeout in milli seconds to calculate rate
*
* OUTPUTS:
*       outRatePtr - (pointer to) Tx rate of port
*
* RETURNS:
*       none
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfCommonPortTxRateGet
(
    IN  GT_U32 portIdx,
    IN  GT_U32 timeOut,
    OUT GT_U32 *outRatePtr
);

/*******************************************************************************
* prvTgfCommonPortRxRateGet
*
* DESCRIPTION:
*       Get Rx rate of port by MAC MIB counters.
*
* INPUTS:
*       portIdx - index of ingress port
*       timeOut - timeout in milli seconds to calculate rate
*
* OUTPUTS:
*       outRatePtr - (pointer to) Rx rate of port
*
* RETURNS:
*       none
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfCommonPortRxRateGet
(
    IN  GT_U32 portIdx,
    IN  GT_U32 timeOut,
    OUT GT_U32 *outRatePtr
);

/*******************************************************************************
* prvTgfCommonDiffInPercentCalc
*
* DESCRIPTION:
*       Calculate difference between input values in percent
*
* INPUTS:
*       value1    - value1 to compare
*       value2    - value2 to compare
*
* OUTPUTS:
*       none
*
* RETURNS:
*       difference between input values in percent
*
* COMMENTS:
*
*******************************************************************************/
GT_U32 prvTgfCommonDiffInPercentCalc
(
    IN  GT_U32 value1,
    IN  GT_U32 value2
);

/*******************************************************************************
* prvTgfCommonPortWsRateGet
*
* DESCRIPTION:
*       Get wire speed rate of port in packets per second
*
* INPUTS:
*       portIdx    - index of egress port
*       packetSize - packet size in bytes including CRC
*
* OUTPUTS:
*       none
*
* RETURNS:
*       wire speed rate of port in packets per second.
*       0xFFFFFFFF - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_U32 prvTgfCommonPortWsRateGet
(
    IN  GT_U32 portIdx,
    IN  GT_U32 packetSize
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __fgfCommonh */

