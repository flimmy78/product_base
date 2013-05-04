/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfDetailedTtiActionType2Entry.h
*
* DESCRIPTION:
*       Verify the functionality of TTI entry
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfDetailedTtiActionType2Entryh
#define __prvTgfDetailedTtiActionType2Entryh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* prvTgfTunnelTermTag1VlanCommandFieldConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set rule index to 0
*         - set macMode PRV_TGF_TTI_MAC_MODE_DA_E
*         - create VLAN 5 and VLAN 6
*         - enable Ethernet TTI lookup
*         - set Ethernet TTI rule to match packets with VLAN tags 5 & 7
*           and specific MAC DA address 
*         - set TTI action with the following parameters:
*              forward to egress interface 
*              set Tag0 VLAN from entry with Tag0 VLAN value 6
*              set Tag1 VLAN from entry with Tag1 VLAN value 8
*         - set Egress Tag field in VLAN 6 entry to <outer Tag1, inner Tag0>
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
GT_VOID prvTgfTunnelTermTag1VlanCommandFieldConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermTag1VlanCommandFieldTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send QinQ packet with matching VLANS:
*               macDa = 00:00:00:00:00:22,
*               macSa = 00:00:00:00:34:02,
*               Tagged: true (with VLAN tag 5)
*               Ether Type: 0x8988 (MIM Ether type) 
*               iSid: 0x123456
*               iUP: 0x5
*               iDP: 0
*
*           Success Criteria:
*               Packet striped and forwarded to port 23 VLAN 6
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
GT_VOID prvTgfTunnelTermTag1VlanCommandFieldTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfUseTunnelTermTag1VlanCommandFieldConfigRestore
*
* DESCRIPTION:
*       Restore test configuration
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
GT_VOID prvTgfTunnelTermTag1VlanCommandFieldConfigRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermTrustExpQosFieldConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set rule index to 0
*         - set macMode PRV_TGF_TTI_MAC_MODE_DA_E
*         - create VLAN 5 and VLAN 6
*         - enable Ethernet TTI lookup
*         - set MPLS TTI rule to match MPLS packet with 2 labels
*         - set TTI action to terminate the tunnel and set trust EXP field to true
*         - set EXP to QoS profile mapping entry with EXP matching the EXP of
*           the outer MPLS label
*         - set IPv4 prefix that will route the tunnel passenger packet
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
GT_VOID prvTgfTunnelTermTrustExpQosFieldConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermTrustExpQosFieldTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send QinQ packet with matching VLANS:
*               macDa = 00:00:00:00:00:22,
*               macSa = 00:00:00:00:34:02,
*               Tagged: true (with VLAN tag 5)
*               Ether Type: 0x8847 (MPLS Ether type) 
*
*           Success Criteria:
*               Verify that the QoS fields are set according to the QoS profile
*               assigned in the EXP to QoS table
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
GT_VOID prvTgfTunnelTermTrustExpQosFieldTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermTrustExpQosFieldConfigRestore
*
* DESCRIPTION:
*       Restore test configuration
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
GT_VOID prvTgfTunnelTermTrustExpQosFieldConfigRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTrustExpQosFieldNonTermConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set rule index to 0
*         - set macMode PRV_TGF_TTI_MAC_MODE_DA_E
*         - create VLAN 5 and VLAN 6
*         - set port default QoS profile
*         - set MPLS TTI rule to match MPLS packet with 3 labels
*         - set TTI action with pop 2 MPLS command
*         - set trust EXP field to true
*         - set redirect packet to egress interface
*         - set EXP to QoS profile entry
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
GT_VOID prvTgfTunnelTrustExpQosFieldNonTermConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTrustExpQosFieldNonTermTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send QinQ packet with matching VLANS:
*               macDa = 00:00:00:00:00:22,
*               macSa = 00:00:00:00:34:02,
*               Tagged: true (with VLAN tag 5)
*               Ether Type: 0x8847 (MPLS Ether type) 
*
*           Success Criteria:
*               Packet is forward correctly with 1 MPLS label
*               QoS fields are set according to the QoS profile matching
*               the EXP field in the last MPLS label that was not popped
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
GT_VOID prvTgfTunnelTrustExpQosFieldNonTermTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTrustExpQosFieldNonTermConfigRestore
*
* DESCRIPTION:
*       Restore test configuration
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
GT_VOID prvTgfTunnelTrustExpQosFieldNonTermConfigRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermTrustAnyQosFieldConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set rule index to 0
*         - set macMode PRV_TGF_TTI_MAC_MODE_DA_E
*         - create VLAN 5 and VLAN 6
*         - set port default QoS profile
*         - set IPv4 TTI rule to match IPv4 tunneled packets
*         - set TTI action to terminate the tunnel, not to trust any QoS fields
*           and with default QoS profile index
*         - set IPv4 prefix that will route the tunnel passenger packet
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
GT_VOID prvTgfTunnelTermTrustAnyQosFieldConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermTrustAnyQosFieldTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send Ipv4-Over-Ipv4 packet:
*               macDa = 00:00:00:00:00:22,
*               macSa = 00:00:00:00:34:02,
*               Tagged: true (with VLAN tag 5)
*               Ether Type: 0x0800 (Ipv4 Ether type) 
*
*           Success Criteria:
*               Packet is forward correctly with 1 MPLS label
*               QoS fields are set according to the QoS profile matching
*               the EXP field in the last MPLS label that was not popped
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
GT_VOID prvTgfTunnelTermTrustAnyQosFieldTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermTrustAnyQosFieldConfigRestore
*
* DESCRIPTION:
*       Restore test configuration
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
GT_VOID prvTgfTunnelTermTrustAnyQosFieldConfigRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermTrustDscpQosFieldConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set rule index to 0
*         - set macMode PRV_TGF_TTI_MAC_MODE_DA_E
*         - create VLAN 5 and VLAN 6
*         - set port default QoS profile
*         - set IPv4 TTI rule to match IPv4 tunneled packets
*         - set TTI action to terminate the tunnel and to trust DSCP
*         - set DSCP to QoS profile mapping entry with DSCP matching the DSCP
*           of the passenger packet
*         - set IPv4 prefix that will route the tunnel passenger packet
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
GT_VOID prvTgfTunnelTermTrustDscpQosFieldConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermTrustDscpQosFieldTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send Ipv4-Over-Ipv4 packet:
*               macDa = 00:00:00:00:00:22,
*               macSa = 00:00:00:00:34:02,
*               Tagged: true (with VLAN tag 5)
*               Ether Type: 0x0800 (Ipv4 Ether type)
*
*           Success Criteria:
*               Packet is forward correctly with 1 MPLS label
*               QoS fields are set according to the QoS profile matching
*               the EXP field in the last MPLS label that was not popped
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
GT_VOID prvTgfTunnelTermTrustDscpQosFieldTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermTrustDscpQosFieldConfigRestore
*
* DESCRIPTION:
*       Restore test configuration
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
GT_VOID prvTgfTunnelTermTrustDscpQosFieldConfigRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermTrustUpQosFieldConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set rule index to 0
*         - set macMode PRV_TGF_TTI_MAC_MODE_DA_E
*         - create VLAN 5 and VLAN 6
*         - set port default QoS profile
*         - set IPv4 TTI rule to match IPv4 tunneled packets
*         - set TTI action to terminate the tunnel and to trust DSCP
*         - set DSCP to QoS profile mapping entry with DSCP matching the DSCP
*           of the passenger packet
*         - set IPv4 prefix that will route the tunnel passenger packet
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
GT_VOID prvTgfTunnelTermTrustUpQosFieldConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermTrustUpQosFieldTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send Ipv4-Over-Ipv4 packet:
*               macDa = 00:00:00:00:00:22,
*               macSa = 00:00:00:00:34:02,
*               Tagged: true (with VLAN tag 5)
*               Ether Type: 0x0800 (Ipv4 Ether type)
*
*           Success Criteria:
*               Packet is forward correctly with 1 MPLS label
*               QoS fields are set according to the QoS profile matching
*               the EXP field in the last MPLS label that was not popped
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
GT_VOID prvTgfTunnelTermTrustUpQosFieldTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermTrustUpQosFieldConfigRestore
*
* DESCRIPTION:
*       Restore test configuration
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
GT_VOID prvTgfTunnelTermTrustUpQosFieldConfigRestore
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfDetailedTtiActionType2Entryh */
