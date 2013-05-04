/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfPortGen.h
*
* DESCRIPTION:
*       Generic API for Port configuration.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __tgfPortGenh
#define __tgfPortGenh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/port/cpssPortTx.h>
#include <cpss/generic/port/cpssPortCtrl.h>

/*
 * typedef: PRV_TGF_PORT_HOL_FC_ENT
 *
 * Description:  Enumeration of system modes HOL and Flow Control.
 *
 * Enumerations:
 *         PRV_TGF_PORT_FC_E    - Flow Control system mode enable.
 *         PRV_TGF_PORT_HOL_E   - HOL system mode enable.
 */
typedef enum
{
    PRV_TGF_PORT_PORT_FC_E,
    PRV_TGF_PORT_PORT_HOL_E
}PRV_TGF_PORT_HOL_FC_ENT;

/*
 * typedef: enum PRV_TGF_PORT_TX_SHARED_DP_MODE_ENT
 *
 * Description:  Drop Precedence (DP) enable mode to use the shared pool.
 *
 * Enumerations:
 *      PRV_TGF_PORT_TX_SHARED_DP_MODE_DISABLE_E      - 
 *                   None of the packets (with any DP) can use the shared pool.
 *                   Compatible with DxCh3 and DxChXcat Disable mode.
 *      PRV_TGF_PORT_TX_SHARED_DP_MODE_ALL_E       - 
 *                   All packets (with any DP) can use the shared pool.
 *                   Compatible with DxCh3 and DxChXcat Enable mode.
 *      PRV_TGF_PORT_TX_SHARED_DP_MODE_DP0_E       - 
 *                   Only packets with DP0 can use the shared pool.
 *      PRV_TGF_PORT_TX_SHARED_DP_MODE_DP0_DP1_E   - 
 *                   Packets with DP0 and DP1 can use the shared pool.
 */
typedef enum
{
    PRV_TGF_PORT_TX_SHARED_DP_MODE_DISABLE_E = GT_FALSE,
    PRV_TGF_PORT_TX_SHARED_DP_MODE_ALL_E = GT_TRUE,
    PRV_TGF_PORT_TX_SHARED_DP_MODE_DP0_E,
    PRV_TGF_PORT_TX_SHARED_DP_MODE_DP0_DP1_E
}PRV_TGF_PORT_TX_SHARED_DP_MODE_ENT;

/*
 * typedef: enum PRV_TGF_PORT_TX_TAIL_DROP_WRTD_ENABLERS_STC
 *
 * Description:  Tail Drop limits enabling for Weigthed Random Tail Drop.
 *
 * Fields:
 *      tcDpLimit       - enable\disable WRTD for {Queue,DP} descriptor/buffer
 *                        limits.
 *      portLimit       - enable\disable Port descriptor/buffer limits.
 *      tcLimit         - enable\disable Queue descriptor/buffer limits.
 *      sharedPoolLimit - enable\disable Shared Pool descriptor/buffer limits.
 */
typedef struct 
{
   GT_BOOL  tcDpLimit;
   GT_BOOL  portLimit;
   GT_BOOL  tcLimit;
   GT_BOOL  sharedPoolLimit;
}PRV_TGF_PORT_TX_TAIL_DROP_WRTD_ENABLERS_STC;

/**********************************************************************
 * typedef: PRV_TGF_PORT_TX_WRTD_MASK_LSB
 *
 * Description: The Tail Drop WRTD masked lsbs' for each limit type.
 *              All masks are up to 9 bits.
 *
 * Fields:
 *   tcDp   - The number of lsbs' masked for WRTD for {Queue,DP} limits.
 *   port   - The number of lsbs' masked for WRTD for Port limits.
 *   tc     - The number of lsbs' masked for WRTD for Queue limits.
 *   pool   - The number of lsbs' masked for WRTD for Shared Pool limits.
 *
 *********************************************************************/
typedef struct
{
    GT_U32  tcDp;
    GT_U32  port;
    GT_U32  tc;
    GT_U32  pool;
} PRV_TGF_PORT_TX_WRTD_MASK_LSB;

/**********************************************************************
 * typedef: PRV_TGF_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS
 *
 * Description: The Tail Drop profile Traffic Class parameters
 *
 * Fields:
 *   dp0MaxBuffNum   - the number of buffers that can be allocated to all of the
 *                     port's TC Tx queues, for a port that uses this profile.
 *                     for GREEN packets
 *   dp1MaxBuffNum   - the number of buffers that can be allocated to all of the
 *                     port's TC Tx queues, for a port that uses this profile.
 *                     for YELLOW packets
 *   dp2MaxBuffNum   - the number of buffers that can be allocated to all of the
 *                     port's TC Tx queues, for a port that uses this profile.
 *                     for RED packets
 *   dp0MaxDescrNum  - the number of descriptors that can be allocated to all of
 *                     the port's TC Tx queues, for a port that uses this
 *                     profile. for GREEN packets
 *   dp1MaxDescrNum  - the number of descriptors that can be allocated to all of
 *                     the port's TC Tx queues, for a port that uses this
 *                     profile. for Yellow packets
 *   dp2MaxDescrNum  - the number of descriptors that can be allocated to all of
 *                     the port's TC Tx queues, for a port that uses this
 *                     profile. for RED packets
 *   tcMaxBuffNum    - the number of buffers that can be allocated to all of the
 *                     port's TC Tx queues, for a port that uses this profile.
 *                     Relevant to Lion and above.
 *   tcMaxDescrNum   - the number of descriptors that can be allocated to all of
 *                     the port's TC Tx queues, for a port that uses this 
 *                     profile. Relevant to Lion and above.
 *********************************************************************/
typedef struct
{
    GT_U16  dp0MaxBuffNum;
    GT_U16  dp1MaxBuffNum;
    GT_U16  dp2MaxBuffNum;
    GT_U16  dp0MaxDescrNum;
    GT_U16  dp1MaxDescrNum;
    GT_U16  dp2MaxDescrNum;
    GT_U16  tcMaxBuffNum;
    GT_U16  tcMaxDescrNum;
} PRV_TGF_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS;

/*******************************************************************************
* prvTgfPortDeviceNumByPortGet
*
* DESCRIPTION:
*       Gets device number for specified port number.
*
*
* INPUTS:
*       portNum  - port number
*
* OUTPUTS:
*       devNumPtr   - (pointer to)device number
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_BAD_PARAM        - on port number not found in port list
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPortDeviceNumByPortGet
(
    IN  GT_U8                          portNum,
    OUT GT_U8                          *devNumPtr
);

/******************************************************************************\
 *                       CPSS generic API section                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfPortForceLinkDownEnableSet
*
* DESCRIPTION:
*       Enable/disable Force Link Down on specified port on specified device.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       state    - GT_TRUE for force link down, GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPortForceLinkDownEnableSet
(
    IN GT_U8                          portNum,
    IN GT_BOOL                        enable
);

/*******************************************************************************
* prvTgfPortEgressCntrModeSet
*
* DESCRIPTION:
*       Configure a set of egress counters that work in specified bridge egress
*       counters mode on specified device.
*
* INPUTS:
*       cntrSetNum - counter set number.
*       setModeBmp - counter mode bitmap. For example:
*                   GT_EGRESS_CNT_PORT | GT_EGRESS_CNT_VLAN | GT_EGRESS_CNT_DP.
*       portNum   - physical port number, if corresponding bit in setModeBmp is 1.
*       vlanId    - VLAN Id, if corresponding bit in setModeBmp is 1.
*       tc        - traffic class queue, if corresponding bit in setModeBmp is 1.
*       dpLevel   - drop precedence level, if corresponding bit in setModeBmp is 1.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported DP levels:  CPSS_DP_GREEN_E, CPSS_DP_RED_E.
*
*******************************************************************************/
GT_STATUS prvTgfPortEgressCntrModeSet
(
    IN  GT_U8                           cntrSetNum,
    IN  CPSS_PORT_EGRESS_CNT_MODE_ENT   setModeBmp,
    IN  GT_U8                           portNum,
    IN  GT_U16                          vlanId,
    IN  GT_U8                           tc,
    IN  CPSS_DP_LEVEL_ENT               dpLevel
);

/*******************************************************************************
* prvTgfPortEgressCntrsGet
*
* DESCRIPTION:
*       Gets a egress counters from specific counter-set.
*
* INPUTS:
*       cntrSetNum - counter set number : 0, 1
*
* OUTPUTS:
*       egrCntrPtr - (pointer to) structure of egress counters current values.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortEgressCntrsGet
(
    IN  GT_U8                       cntrSetNum,
    OUT CPSS_PORT_EGRESS_CNTR_STC   *egrCntrPtr
);
/*******************************************************************************
* prvTgfPortMacCounterGet
*
* DESCRIPTION:
*       Gets Ethernet MAC counter for a specified port on specified device.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*                  CPU port counters are valid only when using "Ethernet CPU
*                  port" (not using the SDMA interface).
*                  when using the SDMA interface refer to the API
*                  cpssDxChNetIfSdmaRxCountersGet(...)
*       cntrName - specific counter name
*
* OUTPUTS:
*       cntrValuePtr - (pointer to) current counter value.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number, device or counter name
*       GT_HW_ERROR  - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     The 10G MAC MIB counters are 64-bit wide.
*     Not supported counters: CPSS_BAD_PKTS_RCV_E, CPSS_UNRECOG_MAC_CNTR_RCV_E,
*     CPSS_BadFC_RCV_E, CPSS_GOOD_PKTS_RCV_E, CPSS_GOOD_PKTS_SENT_E.
*
*******************************************************************************/
GT_STATUS prvTgfPortMacCounterGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    IN  CPSS_PORT_MAC_COUNTERS_ENT  cntrName,
    OUT GT_U64                      *cntrValuePtr
);
/*******************************************************************************
* prvTgfPortMacCountersCaptureOnPortGet
*
* DESCRIPTION:
*     Gets captured Ethernet MAC counter for a particular Port.
*
* INPUTS:
*       portNum  - physical port number
*
* OUTPUTS:
*       portMacCounterSetArrayPtr - (pointer to) array of current counter values.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_STATE - previous capture operation isn't finished.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       Overiding of previous capture by another port in a group is occurs,
*       when the capture triggered port
*       (see cpssDxChPortMacCountersCaptureTriggerSet) and previous
*       capture triggered port are in the same group.
*       (This comment dosn't connected to HyperG.Stack ports.)
*        Not supported counters: CPSS_BAD_PKTS_RCV_E, CPSS_UNRECOG_MAC_CNTR_RCV_E,
*        CPSS_BadFC_RCV_E, CPSS_GOOD_PKTS_RCV_E, CPSS_GOOD_PKTS_SENT_E.
*
*******************************************************************************/
GT_STATUS prvTgfPortMacCountersCaptureOnPortGet
(
    IN  GT_U8                           portNum,
    OUT CPSS_PORT_MAC_COUNTER_SET_STC   *portMacCounterSetArrayPtr
);

/*******************************************************************************
* prvTgfPortMacCountersOnPortGet
*
* DESCRIPTION:
*     Gets Ethernet MAC counter for a particular Port.
*
* INPUTS:
*       portNum  - physical port number
*
* OUTPUTS:
*       portMacCounterSetArrayPtr - (pointer to) array of current counter values.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_STATE - previous capture operation isn't finished.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*     Not supported counters: CPSS_BAD_PKTS_RCV_E, CPSS_UNRECOG_MAC_CNTR_RCV_E,
*     CPSS_BadFC_RCV_E, CPSS_GOOD_PKTS_RCV_E, CPSS_GOOD_PKTS_SENT_E.
*
*******************************************************************************/
GT_STATUS prvTgfPortMacCountersOnPortGet
(
    IN  GT_U8                           portNum,
    OUT CPSS_PORT_MAC_COUNTER_SET_STC   *portMacCounterSetArrayPtr
);

/*******************************************************************************
* prvTgfPortMacCountersRxHistogramEnable
*
* DESCRIPTION:
*       Enable/disable updating of the RMON Etherstat histogram
*       counters for received packets per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*           - Ports 0 through 5
*           - Ports 6 through 11
*           - Ports 12 through 17
*           - Ports 18 through 23
*
* INPUTS:
*       portNum     - physical port number
*       enable      - enable updating of the counters for received packets
*                   GT_FALSE - Counters are updated.
*                   GT_TRUE - Counters are not updated.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_STATE - previous capture operation isn't finished.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     None.
*
*******************************************************************************/
GT_STATUS prvTgfPortMacCountersRxHistogramEnable
(
    IN GT_U8    portNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* prvTgfPortMacCountersTxHistogramEnable
*
* DESCRIPTION:
*       Enable/disable updating of the RMON Etherstat histogram
*       counters for transmitted packets per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*           - Ports 0 through 5
*           - Ports 6 through 11
*           - Ports 12 through 17
*           - Ports 18 through 23
*
* INPUTS:
*       portNum     - physical port number
*       enable      - enable updating of the counters for transmitted packets
*                   GT_FALSE - Counters are updated.
*                   GT_TRUE - Counters are not updated.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_STATE - previous capture operation isn't finished.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     None.
*
*******************************************************************************/
GT_STATUS prvTgfPortMacCountersTxHistogramEnable
(
    IN GT_U8    portNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* prvTgfPortMacCountersClearOnReadSet
*
* DESCRIPTION:
*       Enable or disable MAC Counters Clear on read per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*       - Ports 0 through 5
*       - Ports 6 through 11
*       - Ports 12 through 17
*       - Ports 18 through 23
*
* INPUTS:
*       portNum     - physical port number (or CPU port)
*       enable      - enable clear on read for MAC counters
*                   GT_FALSE - Counters are not cleared.
*                   GT_TRUE - Counters are cleared.

*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS prvTgfPortMacCountersClearOnReadSet
(
    IN GT_U8    portNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* prvTgfPortMruSet
*
* DESCRIPTION:
*       Sets the Maximal Receive Packet size for specified port
*       on specified device.
*
* INPUTS:
*       portNum     - physical port number (or CPU port)
*       mru         - max receive packet size in bytes. (0..16382)
*                     value must be even
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS prvTgfPortMruSet
(
    IN GT_U8    portNum,
    IN GT_U32   mru
);

/*******************************************************************************
* prvTgfPortMruGet
*
* DESCRIPTION:
*       Sets the Maximal Receive Packet size for specified port
*       on specified device.
*
* INPUTS:
*       portNum     - physical port number (or CPU port)
*
* OUTPUTS:
*       mruPtr      - max receive packet size in bytes.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS prvTgfPortMruGet
(
    IN  GT_U8    portNum,
    OUT GT_U32   *mruPtr
);

/*******************************************************************************
* prvTgfPortFcHolSysModeSet
*
* DESCRIPTION:
*       Set Flow Control or HOL system mode on the specified device.
*
* INPUTS:
*       devNum      - physical device number
*       modeFcHol   - Flow Control or HOL system mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvTgfPortFcHolSysModeSet
(
    IN  GT_U8                   devNum,
    IN  PRV_TGF_PORT_HOL_FC_ENT modeFcHol
);

/*******************************************************************************
* prvTgfPortTxTailDropProfileSet
*
* DESCRIPTION:
*       Enables/Disables sharing of buffers and descriptors for all queues of an
*       egress port and set maximal port's limits of buffers and descriptors.
*
* INPUTS:
*       devNum        - physical device number
*       profileSet    - the Profile Set in which the Traffic
*                       Class Drop Parameters is associated
*       enableSharing - If GT_TRUE Sharing is enabled. If a packet is to be queued
*                      to an empty queue, it is queued even if the number of buffers
*                       or descriptors allocated to this port exceeds the
*                       < portMaxBuffLimit > and < portMaxDescrLimit >.
*                       If GT_FALSE Sharing is disabled. If the sum of buffers or
*                       descriptors allocated per queue is larger than the number of
*                       buffers or descriptors allocated to this port
*                       < portMaxBuffLimit > and < portMaxDescrLimit >, then some of
*                       the queues may be starved.
*       portMaxBuffLimit - maximal number of buffers for a port.
*       portMaxDescrLimit - maximal number of descriptors for a port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxTailDropProfileSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet,
    IN  GT_BOOL                             enableSharing,
    IN  GT_U16                              portMaxBuffLimit,
    IN  GT_U16                              portMaxDescrLimit
);

/*******************************************************************************
* prvTgfPortTxTailDropProfileGet
*
* DESCRIPTION:
*       Get sharing status of buffers and descriptors for all queues of an
*       egress port and get maximal port's limits of buffers and descriptors.
*
* INPUTS:
*       devNum        - physical device number
*       profileSet    - the Profile Set in which the Traffic
*                       Class Drop Parameters is associated
*
* OUTPUTS:
*       enableSharingPtr - Pointer to sharing status.
*                      If GT_TRUE Sharing is enabled. If a packet is to be queued
*                      to an empty queue, it is queued even if the number of buffers
*                       or descriptors allocated to this port exceeds the
*                       < portMaxBuffLimit > and < portMaxDescrLimit >.
*                       If GT_FALSE Sharing is disabled. If the sum of buffers or
*                       descriptors allocated per queue is larger than the number of
*                       buffers or descriptors allocated to this port
*                       < portMaxBuffLimit > and < portMaxDescrLimit >, then some of
*                       the queues may be starved.
*       portMaxBuffLimitPtr - Pointer to maximal number of buffers for a port
*       portMaxDescrLimitPtr - Pointer to maximal number of descriptors for a port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxTailDropProfileGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet,
    OUT GT_BOOL                             *enableSharingPtr,
    OUT GT_U16                              *portMaxBuffLimitPtr,
    OUT GT_U16                              *portMaxDescrLimitPtr
);

/*******************************************************************************
* prvTgfPortTx4TcTailDropProfileSet
*
* DESCRIPTION:
*       Set tail drop profiles limits for particular TC.
*
* INPUTS:
*       devNum             - physical device number
*       profileSet         - the Profile Set in which the Traffic
*                            Class Drop Parameters is associated.
*       trafficClass       - the Traffic Class associated with this set of
*                            Drop Parameters.
*       tailDropProfileParamsPtr -
*                       the Drop Profile Parameters to associate
*                       with the Traffic Class in this Profile set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, profile set or
*                                  traffic class
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTx4TcTailDropProfileSet
(
    IN    GT_U8                                   devNum,
    IN    CPSS_PORT_TX_DROP_PROFILE_SET_ENT       profileSet,
    IN    GT_U8                                   trafficClass,
    IN    PRV_TGF_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS *tailDropProfileParamsPtr
);

/*******************************************************************************
* prvTgfPortTx4TcTailDropProfileGet
*
* DESCRIPTION:
*       Get tail drop profiles limits for particular TC.
*
* INPUTS:
*       devNum             - physical device number
*       profileSet         - the Profile Set in which the Traffic
*                            Class Drop Parameters is associated.
*       trafficClass       - the Traffic Class associated with this set of
*                            Drop Parameters.
*
* OUTPUTS:
*       tailDropProfileParamsPtr - Pointer to
*                       the Drop Profile Parameters to associate
*                       with the Traffic Class in this Profile.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, profile set or
*                                  traffic class
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTx4TcTailDropProfileGet
(
    IN    GT_U8                                   devNum,
    IN    CPSS_PORT_TX_DROP_PROFILE_SET_ENT       profileSet,
    IN    GT_U8                                   trafficClass,
    OUT   PRV_TGF_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS *tailDropProfileParamsPtr
);

/*******************************************************************************
*  prvTgfPortBindPortToDpSet
*
* DESCRIPTION:
*       Bind a Physical Port to a specific Drop Profile Set.
*
* INPUTS:
*       devNum        - physical device number
*       portNum       - physical port number
*       profileSet    - the Profile Set in which the Traffic
*                       Class Drop Parameters is associated
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxBindPortToDpSet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               portNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet
);

/*******************************************************************************
*  prvTgfPortTxBindPortToDpGet
*
* DESCRIPTION:
*       Get Drop Profile Set according to a Physical Port.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*
* OUTPUTS:
*       profileSetPtr    - Pointer to the Profile Set in which the Traffic
*                          Class Drop Parameters is associated
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxBindPortToDpGet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               portNum,
    OUT CPSS_PORT_TX_DROP_PROFILE_SET_ENT   *profileSetPtr
);

/*******************************************************************************
* prvTgfPortTxQueueTxEnableSet
*
* DESCRIPTION:
*       Enable/Disable transmission from a Traffic Class queue
*       on the specified port of specified device.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       tcQueue - traffic class queue on this Physical Port
*       enable  - GT_TRUE, enable transmission from the queue
*                GT_FALSE, disable transmission from the queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxQueueTxEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     tcQueue,
    IN  GT_BOOL   enable
);

/*******************************************************************************
* prvTgfPortTxResourceHistogramThresholdSet
*
* DESCRIPTION:
*       Sets threshold for Histogram counter increment.
*
* INPUTS:
*       devNum      - physical device number
*       cntrNum     - Histogram Counter number. Range 0..3.
*       threshold   - If the Global Descriptors Counter exceeds this Threshold,
*                     the Histogram Counter is incremented by 1.
*                     Range 0..0x3FFF.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, cntrNum
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxResourceHistogramThresholdSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  cntrNum,
    IN  GT_U32  threshold
);

/*******************************************************************************
* prvTgfPortTxResourceHistogramCounterGet
*
* DESCRIPTION:
*       Gets Histogram Counter.
*
* INPUTS:
*       devNum  - physical device number
*       cntrNum - Histogram Counter number. Range 0..3.
*
* OUTPUTS:
*       cntrPtr - (pointer to) Histogram Counter value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, cntrNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Histogram Counter is cleared on read.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxResourceHistogramCounterGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  cntrNum,
    OUT GT_U32  *cntrPtr
);

/*******************************************************************************
* prvTgfPortTxGlobalBufNumberGet
*
* DESCRIPTION:
*       Gets total number of virtual buffers enqueued.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       numberPtr - (pointer to) number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The buffers associated with enqueued multi-target descriptors are
*       counted multiple times, once for each multi-target descriptor instance.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxGlobalBufNumberGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *numberPtr
);

/*******************************************************************************
* prvTgfPortTxGlobalDescNumberGet
*
* DESCRIPTION:
*       Gets total number of descriptors allocated.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       numberPtr - (pointer to) number of descriptors
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxGlobalDescNumberGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *numberPtr
);

/*******************************************************************************
* prvTgfPortTxSharedResourceDescNumberGet
*
* DESCRIPTION:
*       Gets total number of descriptors allocated from the shared descriptors 
*       pool.
*
* INPUTS:
*       devNum  - physical device number
*       poolNum - shared pool number, range 0..7
*
* OUTPUTS:
*       numberPtr - (pointer to) number of descriptors
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong poolNum or devNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSharedResourceDescNumberGet
(
    IN  GT_U8  devNum,
    IN  GT_U32 poolNum,
    OUT GT_U32 *numberPtr
);

/*******************************************************************************
* prvTgfPortTxSharedResourceBufNumberGet
*
* DESCRIPTION:
*       Gets the number of virtual buffers enqueued from the shared buffer pool.
*
* INPUTS:
*       devNum  - physical device number
*       poolNum - shared pool number, range 0..7
*
* OUTPUTS:
*       numberPtr - (pointer to) number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong poolNum or devNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSharedResourceBufNumberGet
(
    IN  GT_U8  devNum,
    IN  GT_U32 poolNum,
    OUT GT_U32 *numberPtr
);
/*******************************************************************************
* prvTgfPortTxSniffedBuffersLimitSet
*
* DESCRIPTION:
*       Set maximal buffers limits for mirrored packets.
*
* INPUTS:
*       devNum            - physical device number
*       rxSniffMaxBufNum  - The number of buffers allocated for packets
*                           forwarded to the ingress analyzer port due to
*                           mirroring. Range 0..0xFFFF
*       txSniffMaxBufNum  - The number of buffers allocated for packets
*                           forwarded to the egress analyzer port due to
*                           mirroring. Range 0..0xFFFF
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on out of range value
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSniffedBuffersLimitSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  rxSniffMaxBufNum,
    IN  GT_U32  txSniffMaxBufNum
);

/*******************************************************************************
* prvTgfPortTxSniffedPcktDescrLimitSet
*
* DESCRIPTION:
*       Set maximal descriptors limits for mirrored packets.
*
*
* INPUTS:
*       devNum            - physical device number
*       rxSniffMaxDescNum - The number of descriptors allocated for packets
*                           forwarded to the ingress analyzer port due to
*                           mirroring.
*                           For DxCh1,DxCh2: range 0..0xFFF
*                           For DxCh3,xCat,xCat2: range 0..0x3FFF
*                           For Lion: range 0..0xFFFF
*       txSniffMaxDescNum - The number of descriptors allocated for packets
*                           forwarded to the egress analyzer port due to
*                           mirroring.
*                           For DxCh,DxCh2: range 0..0xFFF
*                           For DxCh3,xCat,xCat2: range 0..0x3FFF
*                           For Lion: range 0..0xFFFF
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on out of range value
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSniffedPcktDescrLimitSet
(
    IN    GT_U8                  devNum,
    IN    GT_U32                 rxSniffMaxDescNum,
    IN    GT_U32                 txSniffMaxDescNum
);

/*******************************************************************************
* prvTgfPortTxSniffedPcktDescrLimitGet
*
* DESCRIPTION:
*       Get maximal descriptors limits for mirrored packets.
*
*
* INPUTS:
*       devNum            - physical device number
*
* OUTPUTS:
*       rxSniffMaxDescNumPtr - (pointer to) The number of descriptors allocated
*                              for packets forwarded to the ingress analyzer
*                              port due to mirroring.
*       txSniffMaxDescNumPtr - (pointer to) The number of descriptors allocated
*                              for packets forwarded to the egress analyzer
*                              port due to mirroring.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSniffedPcktDescrLimitGet
(
    IN    GT_U8                  devNum,
    OUT   GT_U32                *rxSniffMaxDescNumPtr,
    OUT   GT_U32                *txSniffMaxDescNumPtr
);

/*******************************************************************************
* prvTgfPortTxSniffedDescNumberGet
*
* DESCRIPTION:
*       Gets total number of mirrored descriptors allocated.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       rxNumberPtr - (pointer to) number of ingress mirrored descriptors.
*       txNumberPtr - (pointer to) number of egress mirrored descriptors.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSniffedDescNumberGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *rxNumberPtr,
    OUT GT_U32 *txNumberPtr
);

/*******************************************************************************
* prvTgfPortTxSniffedBufNumberGet
*
* DESCRIPTION:
*       Gets total number of mirrored buffers allocated.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       rxNumberPtr - (pointer to) number of ingress mirrored buffers.
*       txNumberPtr - (pointer to) number of egress mirrored buffers.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSniffedBufNumberGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *rxNumberPtr,
    OUT GT_U32 *txNumberPtr
);

/*******************************************************************************
* prvTgfPortTxMcastDescNumberGet
*
* DESCRIPTION:
*       Gets total number of multi-target descriptors allocated.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       numberPtr - (pointer to) number of descriptors
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Multi-target descriptors are descriptors that are duplicated to 
*       multiple egress targets, e.g. Multicast, Broadcast, or unknown Unicast
*       flooded packets.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxMcastDescNumberGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *numberPtr
);

/*******************************************************************************
* prvTgfPortTxMcastBufNumberGet
*
* DESCRIPTION:
*       Gets total number of multi-target buffers allocated (virtual buffers).
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       numberPtr - (pointer to) number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The buffers associated with enqueued multi-target descriptors are
*       counted multiple times, once for each multi-target descriptor instance.
*       Multi-target descriptors are descriptors that are duplicated to 
*       multiple egress targets, e.g. Multicast, Broadcast, or unknown Unicast
*       flooded packets.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxMcastBufNumberGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *numberPtr
);

/*******************************************************************************
* prvTgfPortTxTcProfileSharedPoolSet
*
* DESCRIPTION:
*       Sets the shared pool associated for traffic class and Profile.
*
* INPUTS:
*       devNum  - physical device number
*       pfSet   - the Profile Set in which the Traffic
*                 Class Drop Parameters is associated
*       tc      - the Traffic Class, range 0..7.
*       poolNum - shared pool associated, range 0..7.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, pfSet
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxTcProfileSharedPoolSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT pfSet,
    IN  GT_U8                             tc,
    IN  GT_U32                            poolNum
);

/*******************************************************************************
* prvTgfPortTxTcProfileSharedPoolGet
*
* DESCRIPTION:
*       Gets the shared pool associated for traffic class and Profile.
*
* INPUTS:
*       devNum - physical device number
*       pfSet  - the Profile Set in which the Traffic
*                Class Drop Parameters is associated
*       tc     - the Traffic Class, range 0..7.
*
* OUTPUTS:
*       poolNumPtr - (pointer to) shared pool associated.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, pfSet
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxTcProfileSharedPoolGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT pfSet,
    IN  GT_U8                             tc,
    OUT GT_U32                            *poolNumPtr
);

/*******************************************************************************
* prvTgfPortTxSharedPoolLimitsSet
*
* DESCRIPTION:
*       Set maximal descriptors and buffers limits for shared pool.
*
* INPUTS:
*       devNum     - physical device number
*       poolNum    - Shared pool number. Range 0..7
*       maxBufNum  - The number of buffers allocated for a shared pool. 
*                    Range 0..0x3FFF.
*       maxDescNum - The number of descriptors allocated for a shared pool. 
*                    Range 0..0x3FFF.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on out of range value
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSharedPoolLimitsSet
(
    IN  GT_U8  devNum,
    IN  GT_U32 poolNum,  
    IN  GT_U32 maxBufNum,
    IN  GT_U32 maxDescNum
);

/*******************************************************************************
* prvTgfPortTxSharedPoolLimitsGet
*
* DESCRIPTION:
*       Get maximal descriptors and buffers limits for shared pool.
*
* INPUTS:
*       devNum  - physical device number
*       poolNum - Shared pool number. Range 0..7
*
* OUTPUTS:
*       maxBufNumPtr  - (pointer to) The number of buffers allocated for a 
*                       shared pool.
*       maxDescNumPtr - (pointer to) The number of descriptors allocated 
*                       for a shared pool.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSharedPoolLimitsGet
(
    IN  GT_U8  devNum,
    IN  GT_U32 poolNum,  
    OUT GT_U32 *maxBufNumPtr,
    OUT GT_U32 *maxDescNumPtr
);

/*******************************************************************************
* prvTgfPortTxTcSharedProfileEnableSet
*
* DESCRIPTION:
*       Enable/Disable usage of the shared  descriptors / buffer pool for
*       packets with the traffic class (tc) that are transmited via a
*       port that is associated with the Profile (pfSet).
*
* INPUTS:
*       devNum     - physical device number
*       pfSet      - the Profile Set in which the Traffic
*                  Class Drop Parameters is associated
*       tc         - the Traffic Class, range 0..7.
*       enableMode - Drop Precedence (DPs) enabled mode for sharing.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, pfSet
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxTcSharedProfileEnableSet
(
    IN  GT_U8                              devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT  pfSet,
    IN  GT_U8                              tc,
    IN  PRV_TGF_PORT_TX_SHARED_DP_MODE_ENT enableMode
);

/*******************************************************************************
* prvTgfPortTxTcSharedProfileEnableGet
*
* DESCRIPTION:
*       Gets usage of the shared  descriptors / buffer pool status for
*       packets with the traffic class (tc) that are transmited via a
*       port that is associated with the Profile (pfSet).
*
* INPUTS:
*       devNum - physical device number
*       pfSet  - the Profile Set in which the Traffic
*                Class Drop Parameters is associated
*       tc     - the Traffic Class, range 0..7.
*
* OUTPUTS:
*       enableModePtr - (pointer to) Drop Precedence (DPs) enabled mode for 
*                       sharing.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, pfSet
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxTcSharedProfileEnableGet
(
    IN  GT_U8                              devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT  pfSet,
    IN  GT_U8                              tc,
    OUT PRV_TGF_PORT_TX_SHARED_DP_MODE_ENT *enableModePtr
);

/*******************************************************************************
* prvTgfPortTxProfileWeightedRandomTailDropEnableSet
*
* DESCRIPTION:
*       Enable/disable Weighted Random Tail Drop Threshold to overcome
*       synchronization.
*
* INPUTS:
*       devNum      - device number.
*       pfSet       - the Profile Set in which the Traffic
*                     Class Drop Parameters is associated.
*       dp          - Drop Precedence
*                     (APPLICABLE RANGES: Lion 0..2; xCat2 0..1) 
*       tc          - the Traffic Class, range 0..7.
*                     (APPLICABLE DEVICES: xCat2)
*       enablersPtr - (pointer to) Tail Drop limits enabling 
*                     for Weigthed Random Tail Drop
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid input paramters value
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxProfileWeightedRandomTailDropEnableSet
(
    IN GT_U8                                       devNum,
    IN CPSS_PORT_TX_DROP_PROFILE_SET_ENT           pfSet,
    IN GT_U32                                      dp,
    IN GT_U8                                       tc,
    IN PRV_TGF_PORT_TX_TAIL_DROP_WRTD_ENABLERS_STC *enablersPtr
);

/*******************************************************************************
* prvTgfPortTxTailDropWrtdMasksSet
*
* DESCRIPTION:
*       Sets Weighted Random Tail Drop (WRTD) masks.
*
* INPUTS:
*       devNum     - physical device number
*       maskLsbPtr - WRTD masked least significant bits.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxTailDropWrtdMasksSet
(
    IN GT_U8                         devNum,
    IN PRV_TGF_PORT_TX_WRTD_MASK_LSB *maskLsbPtr
);

/*******************************************************************************
* prvTgfPortTxMcastBuffersLimitSet
*
* DESCRIPTION:
*       Set maximal buffers limits for multicast packets.
*
* INPUTS:
*       devNum         - physical device number
*       mcastMaxBufNum - The number of buffers allocated for multicast
*                        packets. Range 0..0xFFFF
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range mcastMaxBufNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None. 
*
*******************************************************************************/
GT_STATUS prvTgfPortTxMcastBuffersLimitSet
(
    IN  GT_U8  devNum,
    IN  GT_U32 mcastMaxBufNum
);

/*******************************************************************************
* prvTgfPortTxMcastBuffersLimitGet
*
* DESCRIPTION:
*       Get maximal buffers limits for multicast packets.
*
* INPUTS:
*       devNum            - physical device number
*
* OUTPUTS:
*       mcastMaxBufNumPtr - (pointer to) the number of buffers allocated
*                           for multicast packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxMcastBuffersLimitGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *mcastMaxBufNumPtr
);

/*******************************************************************************
* prvTgfPortTxMcastPcktDescrLimitSet
*
* DESCRIPTION:
*       Set maximal descriptors limits for multicast packets.
*
* INPUTS:
*       devNum          - physical device number
*       mcastMaxDescNum - The number of descriptors allocated for multicast
*                         packets.
*                         For all devices except Lion and above: in units of 128 
*                         descriptors, the actual number descriptors
*                         allocated will be 128 * mcastMaxDescNum.
*                         For Lion and above: actual descriptors number (granularity 
*                         of 1).
*                         For DxCh,DxCh2,DxCh3,DxChXcat: range 0..0xF80
*                         For Lion and above: range 0..0xFFFF
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxMcastPcktDescrLimitSet
(
    IN    GT_U8  devNum,
    IN    GT_U32 mcastMaxDescNum
);

/*******************************************************************************
* prvTgfPortTxMcastPcktDescrLimitGet
*
* DESCRIPTION:
*       Get maximal descriptors limits for multicast packets.
*
* INPUTS:
*       devNum              - physical device number
*
* OUTPUTS:
*       mcastMaxDescNumPtr  - (pointer to) the number of descriptors allocated
*                             for multicast packets.
*                             For all devices except Lion and above: in units of 128 
*                             descriptors, the actual number descriptors
*                             allocated will be 128 * mcastMaxDescNum.
*                             For Lion and above: actual descriptors number (granularity 
*                             of 1).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxMcastPcktDescrLimitGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *mcastMaxDescNumPtr
);

/*******************************************************************************
* prvTgfPortSpeedGet
*
* DESCRIPTION:
*       Gets speed for specified port on specified device.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       speedPtr - pointer to actual port speed
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - on no initialized SERDES per port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        1.This API also checks if at least one serdes per port was initialized.
*        In case there was no initialized SERDES per port GT_NOT_INITIALIZED is
*        returned.
*
*       2.Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*******************************************************************************/
GT_STATUS prvTgfPortSpeedGet
(
    IN  GT_U8                 devNum,
    IN  GT_U8                 portNum,
    OUT CPSS_PORT_SPEED_ENT   *speedPtr
);

/*******************************************************************************
* prvTgfPortFlowControlEnableSet
*
* DESCRIPTION:
*       Enable/disable receiving and transmission of 802.3x Flow Control frames
*       in full duplex on specified port on specified device.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       state    - Flow Control state: Both disabled,
*                  Both enabled, Only Rx or Only Tx enabled.
*                  Note: only XG ports can be configured in all 4 options,
*                        Tri-Speed and FE ports may use only first two.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, state or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortFlowControlEnableSet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    IN  CPSS_PORT_FLOW_CONTROL_ENT  state
);

/*******************************************************************************
* prvTgfPortTxSharingGlobalResourceEnableSet
*
* DESCRIPTION:
*       Enable/Disable sharing of resources for enqueuing of packets.
*
* INPUTS:
*       devNum  - physical device number
*       enable  - GT_TRUE   - enable TX queue resourses sharing
*                 GT_FALSE - disable TX queue resources sharing
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSharingGlobalResourceEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
);

/*******************************************************************************
* prvTgfPortTxSchedulerArbGroupSet
*
* DESCRIPTION:
*       Set Traffic Class Queue scheduling arbitration group on
*       specificed profile of specified device.
*
* INPUTS:
*       devNum     - physical device number
*       profileSet - the Tx Queue scheduler Profile Set in which the arbGroup
*                    parameter is associated.
*       tcQueue    - traffic class queue
*       arbGroup   - scheduling arbitration group:
*                     1) Strict Priority
*                     2) WRR Group 1
*                     3) WRR Group 0
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong profile number, device or arbGroup
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT               - previous updated operation not yet completed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSchedulerArbGroupSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  profileSet,
    IN  GT_U32                                  tcQueue,
    IN  CPSS_PORT_TX_Q_ARB_GROUP_ENT            arbGroup
);

/*******************************************************************************
* prvTgfPortTxSchedulerArbGroupGet
*
* DESCRIPTION:
*       Get Traffic Class Queue scheduling arbitration group on
*       specificed profile of specified device.
*
* INPUTS:
*       devNum     - physical device number
*       profileSet - the Tx Queue scheduler Profile Set in which the arbGroup
*                    parameter is associated.
*       tcQueue    - traffic class queue 
*
* OUTPUTS:
*       arbGroupPtr   - Pointer to scheduling arbitration group:
*                       1) Strict Priority
*                       2) WRR Group 1
*                       3) WRR Group 0
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong profile number, device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSchedulerArbGroupGet
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  profileSet,
    IN  GT_U32                                  tcQueue,
    OUT CPSS_PORT_TX_Q_ARB_GROUP_ENT            *arbGroupPtr
);

/*******************************************************************************
* prvTgfPortTxSchedulerProfileIndexSet
*
* DESCRIPTION:
*       Bind a port to scheduler profile set.
*
* INPUTS:
*       devNum     - device number
*       portNum    - physical port number,
*                    or the Queue Group number in case of Aggregate Mode
*                    in ExMxPm XG device.
*       index      - The scheduler profile index. range 0..7.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSchedulerProfileIndexSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_U32  index
);

/*******************************************************************************
* prvTgfPortTxSchedulerProfileIndexGet
*
* DESCRIPTION:
*       Get scheduler profile set that is binded to the port.
*
* INPUTS:
*       devNum        - device number
*       portNum       - physical port number,
*                       or the Queue Group number in case of Aggregate Mode
*                       in ExMxPm XG device.
*
* OUTPUTS:
*       indexPtr      - (pointer to) The scheduler profile index. range 0..7.
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPortTxSchedulerProfileIndexGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_U32  *indexPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __tgfPortGenh */

