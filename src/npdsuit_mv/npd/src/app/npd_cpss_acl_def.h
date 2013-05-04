/********************************************************************************
* npdcpssacldef.h
*
* DESCRIPTION:
*       CPSS generic PCL lib API implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 1.10 $
*******************************************************************************/
#ifndef __NPD_CPSS_ACL_DEF_H__
#define __NPD_CPSS_ACL_DEF_H__

#if 0
/*
 * Typedef: struct GT_BYTE_ARRY
 *
 * Description:
 *    This structure contains byte array data and length.
 *
 * Fields:
 *    data   - pointer to allocated data buffer
 *    length - buffer length
 */
typedef struct
{
    unsigned char      *data;
    unsigned long      length;
} GT_BYTE_ARRY;


/*
 * Typedef: CPSS_NET_RX_CPU_CODE_ENT
 *
 * Description: Defines the different CPU codes that indicate the reason for
 *              sending a packet to the CPU.
 *              This enumeration type is enlarged over 8 bits
 *              for none TWIST PP.
 * Fields:
 *
 *  Codes set by Ethernet Bridging Engine:
 *      CPSS_NET_CONTROL_E
 *                                      - Can be initiated only by another CPU.
 *      CPSS_NET_CONTROL_BPDU_E            - Control BPDU packet.
 *      CPSS_NET_CONTROL_DEST_MAC_TRAP_E
                                       - Destination MAC trapped packet.
 *      CPSS_NET_CONTROL_SRC_MAC_TRAP_E    - Source MAC trapped packet.
 *      CPSS_NET_CONTROL_SRC_DST_MAC_TRAP_E
                                       - Source & Destination MAC trapped packet.
 *      CPSS_NET_CONTROL_MAC_RANGE_TRAP_E  - MAC Range Trapped packet.
 *      CPSS_NET_RX_SNIFFER_TRAP_E  - Trapped to CPU due to Rx sniffer
 *      CPSS_NET_INTERVENTION_ARP_E - Intervention ARP
 *      CPSS_NET_INTERVENTION_IGMP_E- Intervention IGMP
 *      CPSS_NET_INTERVENTION_SA_E  - Intervention Source address.
 *      CPSS_NET_INTERVENTION_DA_E  - Intervention Destination address.
 *      CPSS_NET_INTERVENTION_SA_DA_E - Intervention Source and
 *                                               Destination addresses
 *      CPSS_NET_INTERVENTION_PORT_LOCK_E  - Intervention - Port Lock to CPU.
 *  Codes set by L3- L7 Engines:
 *      CPSS_NET_RESERVED_SIP_TRAP_E    - Trapped Due to Reserved SIP.
 *      CPSS_NET_INTERNAL_SIP_TRAP_E    - Trapped Due to Internal SIP.
 *      CPSS_NET_SIP_SPOOF_TRAP_E       - Trapped Due to SIP Spoofing
 *  TCB Codes:
 *      CPSS_NET_DEF_KEY_TRAP_E  - Trap by default Key Entry,after no
 *                                          match was found in the Flow Table.
 *      CPSS_NET_IP_CLASS_TRAP_E  - Trap by Ip Classification Tables
 *      CPSS_NET_CLASS_KEY_TRAP_E - Trap by Classifier's Key entry
 *                                after a match was found in the Flow Table.
 *      CPSS_NET_TCP_RST_FIN_TRAP_E - Packet Mirrored to CPU because
 *                                             of TCP Rst_FIN trapping.
 *      CPSS_NET_CLASS_KEY_MIRROR_E - Packet Mirrored to CPU because
 *                                           of mirror bit in Key entry.
 *      CPSS_NET_TRAP_BY_DEFAULT_ENTRY0_E  - Trap by default entry 0
 *      CPSS_NET_TRAP_BY_DEFAULT_ENTRY1_E  - Trap by default entry 1
 *      CPSS_NET_TRAP_UD_INVALID_E         - Trap due to User Defined
 *                                                    bits not all valid
 *      CPSS_NET_CLASS_MTU_EXCEED_E - Classifier MTU exceed when
 *                                redirecting a packet through the classifier.
 *  Ipv4 Codes:
 *      CPSS_NET_RESERVED_DIP_TRAP_E - Reserved DIP Trapping.
 *      CPSS_NET_MC_BOUNDARY_TRAP_E  - Multicast Boundary Trapping.
 *      CPSS_NET_INTERNAL_DIP_E      - Internal DIP.
 *      CPSS_NET_IP_ZERO_TTL_TRAP_E  - Packet was trapped due to
 *                                          TTL = 0 (valid for IP header only).
 *      CPSS_NET_BAD_IP_HDR_CHECKSUM_E - Bad IP header Checksum.
 *      CPSS_NET_RPF_CHECK_FAILED_E    - Packet did not pass RPF check,
 *                                                need to send prune message.
 *      CPSS_NET_OPTIONS_IN_IP_HDR_E  - Packet with Options in the IP header.
 *      CPSS_NET_END_OF_IP_TUNNEL_E   - Packet which is in the End of
 *                                  an IP tunnel sent for reassembly to the CPU.
 *      CPSS_NET_BAD_TUNNEL_HDR_E     - Bad tunnel header - Bad GRE
 *                       header or packet need to be fragmented with DF bit set.
 *      CPSS_NET_IP_HDR_ERROR_E - IP header contains Errors -
 *                                          version!= 4, Ip header length < 20.
 *      CPSS_NET_ROUTE_ENTRY_TRAP_E- Trap Command was found in the
 *                                            Route Entry.
 *      CPSS_NET_DIP_CHECK_ERROR_E - DIP check error
 *      CPSS_NET_ILLEGAL_DIP_E     - Illegal DIP, class D or mismatch
 *      CPSS_NET_POLICY_TRAP_E     - Trap from policy engine
 *      CPSS_NET_DEFAULT_ROUTE_TRAP_E- Packet is trapped due to default
 *                                              rout entry
 *      CPSS_NET_IP_MTU_EXCEED_E     - IPv4 MTU exceed.
 *  MPLS Codes:
 *      CPSS_NET_MPLS_MTU_EXCEED_E   - MPLS MTU exceed.
 *      CPSS_NET_MPLS_ZERO_TTL_TRAP_E- TTL in the MPLS shim header is 0.
 *      CPSS_NET_NHLFE_ENTRY_TRAP_E  - Trap command was found in the NHLFE.
 *      CPSS_NET_ILLEGAL_POP_E       - Illegal pop operation was done.
 *      CPSS_NET_INVALID_MPLS_IF_E   - Invalid MPLS Interface Entry was fetched.
 *
 *  Tiger used additional Codes:
 *
 *  Tiger IPv6 codes:
 *      CPSS_NET_IPV6_ICMP_MLD_E    - ICNPV6 packet trap
 *      CPSS_NET_IPV6_IGMP_E        - IGMP packet trap
 *      CPSS_NET_IPV6_MC_PCL_E      - IPV6 trap due to MC entry
                                             in Policy Control List
 *      CPSS_NET_IPV6_HOP_BY_HOP_E  - IPV6 trap Hop by Hop
 *      CPSS_NET_IPV6_EXT_HEADER_E  - IPV6 trap extension headers
 *      CPSS_NET_IPV6_BAD_HEADER_E  - IPV6 header error
 *      CPSS_NET_IPV6_ILLIGAL_DIP_E - IPV6 illegal address
 *      CPSS_NET_IPV6_DIP_ERROR_E   - IPV6 DIP check error
 *      CPSS_NET_IPV6_ROUTE_TRAP_E  - IPV6 packet is dropped due
 *                                             to route entry
 *      CPSS_NET_IPV6_ROUTER_RPF_E  - IPV6 packet didn't pass RPF check
 *      CPSS_NET_IPV6_SCOP_FAIL_E   - IPV6 packet failed scope check
 *      CPSS_NET_IPV6_TTL_TRAP_E    - IPV6 trap hop limit
 *      CPSS_NET_IPV6_DEFAULT_ROUTE_E- IPV6 packet is trapped due
                                              to default route entry
 *
 *  Tiger Mirror codes
 *      CPSS_NET_MIRROR_PCL_RST_FIN_E      - policy reset FIN mirror
 *      CPSS_NET_MIRROR_PCL_PCE_E          - policy action mirror
 *      CPSS_NET_MIRROR_BRIDGE_E           - bridge mirror
 *      CPSS_NET_MIRROR_MLD_E              - MLD mirror
 *      CPSS_NET_MIRROR_IGMP_E             - IGMP snooping
 *      CPSS_NET_MIRROR_RIPV1_E            - RIPv1 Mirroring
 *      CPSS_NET_MIRROR_TTL_E              - TTL Mirror
 *      CPSS_NET_MIRROR_RPF_E              - RPF Check Fail Mirror
 *      CPSS_NET_MIRROR_IP_OPTIONS_E       - IP Options Mirror
 *      CPSS_NET_MIRROR_ROUTE_E            - Route Entry Mirror
 *      CPSS_NET_MIRROR_ICMP_E             - ICMP Redirect Mirror
 *      CPSS_NET_MIRROR_HOP_BY_HOP_E       - IP Hop by Hop
 *      CPSS_NET_MIRROR_EXT_HEADER_E       - IP Extension Hdr
 *      CPSS_NET_MIRROR_HEADER_ERROR_E     - IP Header Error
 *      CPSS_NET_MIRROR_ILLIGAL_IP_E       - IP Illegal Address
 *      CPSS_NET_MIRROR_SPECIAL_DIP_E      - IP Special DIP
 *      CPSS_NET_MIRROR_IP_SCOP_E          - IP Scope
 *      CPSS_NET_MIRROR_IP_MTU_E           - IP Mirror MTU
 *      CPSS_NET_MIRROR_INLIF_E            - Mirror due to InLif Entry
 *      CPSS_NET_MIRROR_PCL_MTU_E          - Policy Mirror MTU
 *      CPSS_NET_MIRROR_DEFAULT_ROUTE_E    - Default Route Entry Mirror
 *      CPSS_NET_MIRROR_MC_ROUTE1_E        - MC Route Entry Mirror1
 *      CPSS_NET_MIRROR_MC_ROUTE2_E        - MC Route Entry Mirror2
 *      CPSS_NET_MIRROR_MC_ROUTE3_E        - MC Route Entry Mirror3
 *      CPSS_NET_MIRROR_IPV4_BC_E          - IPv4 MC Broadcast

 *
 *  Egress Pipe Codes:
 *      CPSS_NET_ETH_BRIDGED_LLT_E  - Regular packet passed to CPU,
 *                                Last level treated (LLT) = Ethernet Bridged.
 *      CPSS_NET_IPV4_ROUTED_LLT_E - Regular packet passed to CPU,
 *                                LLT = IPv4 Routed.
 *      CPSS_NET_UC_MPLS_LLT_E    - Regular packet passed to CPU,
 *                                           LLT = Unicast MPLS.
 *      CPSS_NET_MC_MPLS_LLT_E    - Regular packet passed to CPU,
 *                                LLT = Multicast MPLS (currently not supported).
 *      CPSS_NET_IPV6_ROUTED_LLT_E - Regular packet passed to CPU,
 *                                LLT = IPv6 Routed (currently not supported).
 *      CPSS_NET_L2CE_LLT_E - Regular packet passed to CPU, LLT = L2CE.
 *      CPSS_NET_EGRESS_MIRROR_TO_CPU_E    - Egress mirrored to CPU frame,
 *       due to sampling to CPU or CPU being configured as egress analyzer port.
 *
 *
 *
 *  DXSal used additional Codes:
 *      CPSS_NET_UNKNOWN_UC_E              - Unknown Unicast packet
 *      CPSS_NET_UNREG_MC_E                - Unregistered Multicast packet
 
 *  DXSal, DXCH and EXMXPM used additional Codes:
 *      CPSS_NET_LOCK_PORT_MIRROR_E        - Locked Port Mirror packet
 *
 *  DXCH and EXMXPM used additional Codes:
 *      CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E - Packet with MAC DA in
 *         the IEEE Reserved Multicast range trapped or mirrored to the host CPU
 *      CPSS_NET_IPV6_ICMP_PACKET_E - IPv6 ICMP packet trapped or
 *                                         mirrored to the host CPU.
 *      CPSS_NET_MIRROR_IPV4_RIPV1_TO_CPU_E - IPv4 RIPv1 packet
 *                                                     mirrored to the host CPU.
 *      CPSS_NET_IPV6_NEIGHBOR_SOLICITATION -IPv6 neighbor solicitation
 *                                   packet trapped or mirrored to the host CPU.
 *      CPSS_NET_IPV4_BROADCAST_PACKET_E  - IPv4 Broadcast packet
 *         assigned to a VLAN with IPv4 Broadcast Command set to Trap or Mirror.
 *      CPSS_NET_NON_IPV4_BROADCAST_PACKET_E- Non IPv4 Broadcast packet
 *     assigned to a VLAN with non IPv4 Broadcast Command set to Trap or Mirror.
 *      CPSS_NET_CISCO_MULTICAST_MAC_RANGE_E- A Multicast packet with a
 *          MAC DA in the CISCO AUI range trapped or mirrored to the host CPU.
 *      CPSS_NET_UNREGISTERED_MULTICAST_E - Non IPv4/IPv6
 *          Unregistered Multicast packet assigned to a VLAN with non IPv4/IPv6
 *          Unregistered Multicast Command set to Trap or Mirror.
 *      CPSS_NET_IPV4_UNREGISTERED_MULTICAST_E  - IPv4 Unregistered
 *          Multicast packet assigned to a VLAN with IPv4 Unregistered Multicast
 *          Command set to Trap or Mirror.
 *      CPSS_NET_IPV6_UNREGISTERED_MULTICAST_E  - IPv6 Unregistered
 *          Multicast packet assigned to a VLAN with IPv6 Unregistered Multicast
 *          Command set to Trap or Mirror.
 *              CPSS_NET_UDP_BC_MIRROR_TRAP0_E -
 *              CPSS_NET_UDP_BC_MIRROR_TRAP1_E
 *              CPSS_NET_UDP_BC_MIRROR_TRAP2_E
 *              CPSS_NET_UDP_BC_MIRROR_TRAP3_E
 *
 *      CPSS_NET_BRIDGED_PACKET_FORWARD_E - Packet forwarded to
 *                  the host CPU by one of the following engines in the device:
 *                  1. Redirected by the PCL engine to the CPU port number.
 *                  2. MAC DA is associated with the CPU port number.
 *                  3. Private VLAN edge target set to CPU port number.
 *      CPSS_NET_INGRESS_MIRRORED_TO_ANLYZR_E- Ingress mirrored packets
 *       to the analyzer port, when the ingress analyzer port number is set
 *       to the CPU port number.
 *      CPSS_NET_EGRESS_MIRRORED_TO_ANLYZR_E - Egress mirrored packets
 *          to the analyzer port, when the egress analyzer port number is set
 *          to the CPU port number.
 *      CPSS_NET_MAIL_FROM_NEIGHBOR_CPU_E - A packet sent to the local
 *                                  host CPU as a mail from the neighboring CPU.
 *      CPSS_NET_CPU_TO_CPU_E - A packet sent to the local host CPU,
 *                                from one of the other host CPUs in the system.
 *      CPSS_NET_EGRESS_SAMPLED_E - Packet mirrored to the host CPU by
 *                                         STC mechanism.
 *      CPSS_NET_INGRESS_SAMPLED_E - Packet mirrored to the host CPU by the ingress
 *                                         the egress STC mechanism.
 *      CPSS_NET_INVALID_PCL_KEY_TRAP_E - Packet trapped/mirrored to
 *       the host CPU by the Policy Engine due to the following: User-defined
 *       bytes in the key could not be parsed due to packet header length or its
 *       format.
 *
 *  Puma used additional Codes:
 *
 *      CPSS_NET_INLIF_MIRROR_E
 *                                      - Packet mirrored to the CPU due to
 *                                        by the bridge according to the INLIF
 *                                        entry.
 *      CPSS_NET_MC_BRIDGED_PACKET_FORWARD_E
 *                                      - An L2 packet is sent to the CPU when
 *                                        it is a memeber of the packet's L2 MC
 *                                        group.
 *      CPSS_NET_MC_ROUTED_PACKET_FORWARD_E
 *                                      - An IP MC packet is sent to the CPU when
 *                                        it is a member of the packet's L2 MC
 *                                        group or from the DIT entry.
 *      CPSS_NET_L2VPN_PACKET_FORWARD_E
 *                                      - An VPLS packet is sent to the CPU from
 *                                        the DIT entry or from the MC group
 *                                        pointed to by DIT entry.
 *      CPSS_NET_MPLS_PACKET_FORWARD_E
 *                                      - An MPLS packet is sent to the CPU from
 *                                        the DIT entry or from the MC group
 *                                        pointed to by DIT entry.
 *      CPSS_NET_UNREGISTERED_MC_EGRESS_FILTER_E
 *                                      - An unregistered MC packet is trapped/
 *                                        mirrored by the egress filter mechanism
 *      CPSS_NET_UNKNOWN_UC_EGRESS_FILTER_E
 *                                      - An unknown UC packet is trapped/mirrored
 *                                        by the egress filter mechanism.
 *      CPSS_NET_ARP_BC_EGRESS_MIRROR_E
 *                                      - An ARP BC packet is trapped to the CPU
 *                                        by the egress pipe.
 *      CPSS_NET_VPLS_UNREGISTERED_MC_EGRESS_FILTER_E
 *                                      - An VPLS packet that is subject to
 *                                        egress filtering is trapped to the CPU.
 *      CPSS_NET_VPLS_UNKWONW_UC_EGRESS_FILTER_E
 *                                      - An VPLS packet that is subject to
 *                                        egress filtering is trapped to the CPU.
 *      CPSS_NET_VPLS_BC_EGRESS_FILTER_E
 *                                      - An VPLS packet that is subject to
 *                                        egress filtering is trapped to the CPU.
 *      CPSS_NET_MC_CPU_TO_CPU_E
 *                                      - When packet is sent from the CPU to a
 *                                        MC group in which the CPU is a member
 *                                        as well.
 *      CPSS_NET_PCL_MIRRORED_TO_ANALYZER_E
 *                                      - Ingress policy mirrored packets to the
 *                                        analyzer port
 *      CPSS_NET_VPLS_UNTAGGED_MRU_FILTER_E
 *                                      - A VPLS triggered packet is trapped
 *                                        since it's BC exceeds the configured
 *                                        untagged mru.
 *      CPSS_NET_TT_MPLS_HEADER_CHECK_E
 *                                      - A tunnel terminated MPLS packet is
 *                                        trapped due to an illegal MPLS header
 *                                        or MPLS command in action entry.
 *      CPSS_NET_TT_MPLS_TTL_EXCEED_E
 *                                      - A tunnel terminated MPLS packet is
 *                                        trapped due to an assigned TTL of ZERO.
 *      CPSS_NET_TTI_MIRROR_E
 *                                      - A packet is mirrored due to TTI Action
 *                                        Entry Command.
 *      CPSS_NET_MPLS_SRC_FILTERING_E
 *                                      - A VPLS packet is trapped/mirrored due
 *                                        to source local filtering.
 *      CPSS_NET_IPV4_TTL1_EXCEEDED_E
 *                                      - IPv4 packet, triggered for routing
 *                                        Trapped/Mirrored to the CPU due to TTL
 *                                        exceeded.
 *      CPSS_NET_IPV6_HOPLIMIT1_EXCEED_E
 *                                      - IPv4 packet, triggered for routing
 *                                        Trapped/Mirrored to the CPU due to
 *                                        Hop Limit exceeded.
 *
 *
 *
 *      CPSS_NET_FIRST_USER_DEFINED_E   - User-Defined CPU codes.
 *                                        The user may use this range for any
 *                                        application specific purpose.
 *
 *      CPSS_NET_LAST_USER_DEFINED_E    - User-Defined CPU codes.
 *                                        The user may use this range for any
 *                                        application specific purpose.
 *
 *      CPSS_NET_FIRST_UNKNOWN_HW_CPU_CODE_E - first code for non-converted DSA
 *             codes.
 *
 *             NOTE :
 *             this range is reserved for CPU codes from the DSA , that are not
 *             covered by the mapping from "DSA CPU opcode" to
 *             "unified Rx CPU opcode"
 *
 *             so value of CPSS_NET_FIRST_UNKNOWN_HW_CPU_CODE_E + 89
 *             means that the DSA tag was with reason 89 , but since the CPSS SW
 *             do not know what is the meaning of 89 , we let application deal
 *             with it.
 *
 *             because the CPSS not want to "drop" this RX packet !
 *
 *      CPSS_NET_LAST_UNKNOWN_HW_CPU_CODE_E - last code for non-converted DSA
 *             codes
 *
 *      CPSS_NET_ALL_CPU_OPCODES_E - to ensure 32 bit size of type's
 *             variables and for cases of actions general for all cpu opcodes 
 */
typedef enum
{
    /* L2 Bridge */
    CPSS_NET_CONTROL_E                          = 1,
    CPSS_NET_UN_KNOWN_UC_E                      = 10,
    CPSS_NET_UN_REGISTERD_MC_E                  = 11,
    CPSS_NET_CONTROL_BPDU_E                     = 16,
    CPSS_NET_CONTROL_DEST_MAC_TRAP_E            = 17,
    CPSS_NET_CONTROL_SRC_MAC_TRAP_E             = 18,
    CPSS_NET_CONTROL_SRC_DST_MAC_TRAP_E         = 19,
    CPSS_NET_CONTROL_MAC_RANGE_TRAP_E           = 20,
    /* Pre-egress engine */
    CPSS_NET_RX_SNIFFER_TRAP_E                  = 21,
    CPSS_NET_INTERVENTION_ARP_E                 = 32,
    CPSS_NET_INTERVENTION_IGMP_E                = 33,
    CPSS_NET_INTERVENTION_SA_E                  = 34,
    CPSS_NET_INTERVENTION_DA_E                  = 35,
    CPSS_NET_INTERVENTION_SA_DA_E               = 36,
    CPSS_NET_INTERVENTION_PORT_LOCK_E           = 37,
    CPSS_NET_EXP_TNL_BAD_IPV4_HDR_E             = 38,
    CPSS_NET_EXP_TNL_BAD_VLAN_E                 = 39,
    /* MLL */
    CPSS_NET_MLL_RPF_TRAP_E                     = 120,
    /* Pre-router */
    CPSS_NET_RESERVED_SIP_TRAP_E                = 128,
    CPSS_NET_INTERNAL_SIP_TRAP_E                = 129,
    CPSS_NET_SIP_SPOOF_TRAP_E                   = 130,
    /* PCL */
    CPSS_NET_DEF_KEY_TRAP_E                     = 132,
    CPSS_NET_IP_CLASS_TRAP_E                    = 133,
    CPSS_NET_CLASS_KEY_TRAP_E                   = 134,
    CPSS_NET_TCP_RST_FIN_TRAP_E                 = 135,
    CPSS_NET_CLASS_KEY_MIRROR_E                 = 136,
    CPSS_NET_TRAP_BY_DEFAULT_ENTRY0_E           = 137,
    CPSS_NET_TRAP_BY_DEFAULT_ENTRY1_E           = 138,
    CPSS_NET_TRAP_UD_INVALID_E                  = 139,
    /* IPv4 */
    CPSS_NET_RESERVED_DIP_TRAP_E                = 144,
    CPSS_NET_MC_BOUNDARY_TRAP_E                 = 145,
    CPSS_NET_INTERNAL_DIP_E                     = 146,
    CPSS_NET_IP_ZERO_TTL_TRAP_E                 = 147,
    CPSS_NET_BAD_IP_HDR_CHECKSUM_E              = 148,
    CPSS_NET_RPF_CHECK_FAILED_E                 = 149,
    CPSS_NET_OPTIONS_IN_IP_HDR_E                = 150,
    CPSS_NET_END_OF_IP_TUNNEL_E                 = 151,
    CPSS_NET_BAD_TUNNEL_HDR_E                   = 152,
    CPSS_NET_IP_HDR_ERROR_E                     = 153,
    CPSS_NET_ROUTE_ENTRY_TRAP_E                 = 154,
    CPSS_NET_DIP_CHECK_ERROR_E                  = 155,
    CPSS_NET_ILLEGAL_DIP_E                      = 156,
    CPSS_NET_POLICY_TRAP_E                      = 157,
    CPSS_NET_DEFAULT_ROUTE_TRAP_E               = 158,
    /*MTU Exceeded */
    CPSS_NET_IP_MTU_EXCEED_E                    = 161,
    CPSS_NET_MPLS_MTU_EXCEED_E                  = 162,
    CPSS_NET_CLASS_MTU_EXCEED_E                 = 163,
    /*Mpls */
    CPSS_NET_MPLS_ZERO_TTL_TRAP_E               = 171,
    CPSS_NET_NHLFE_ENTRY_TRAP_E                 = 172,
    CPSS_NET_ILLEGAL_POP_E                      = 173,
    CPSS_NET_INVALID_MPLS_IF_E                  = 174,

    /* Start of additional Values for Tiger use only */
    /* Tiger IPv6 codes */
    CPSS_NET_IPV6_ICMP_MLD_E                    = 175,
    CPSS_NET_IPV6_IGMP_E                        = 176,
    CPSS_NET_IPV6_MC_PCL_E                      = 186,
    CPSS_NET_IPV6_HOP_BY_HOP_E                  = 187,
    CPSS_NET_IPV6_EXT_HEADER_E                  = 188,
    CPSS_NET_IPV6_BAD_HEADER_E                 = 189,
    CPSS_NET_IPV6_ILLIGAL_DIP_E                 = 190,
    CPSS_NET_IPV6_DIP_ERROR_E                  = 191,
    CPSS_NET_IPV6_ROUTE_TRAP_E                  = 192,
    CPSS_NET_IPV6_ROUTER_RPF_E                  = 193,
    CPSS_NET_IPV6_SCOP_FAIL_E                   = 194,
    CPSS_NET_IPV6_TTL_TRAP_E                    = 195,
    CPSS_NET_IPV6_DEFAULT_ROUTE_E               = 196,
    /* Tiger Mirror codes */
    CPSS_NET_MIRROR_PCL_RST_FIN_E               = 201,
    CPSS_NET_MIRROR_PCL_PCE_E                   = 202,
    CPSS_NET_MIRROR_BRIDGE_E                    = 203,
    CPSS_NET_MIRROR_MLD_E                       = 204,
    CPSS_NET_MIRROR_IGMP_E                      = 205,
    CPSS_NET_MIRROR_RIPV1_E                     = 206,
    CPSS_NET_MIRROR_TTL_E                       = 207,
    CPSS_NET_MIRROR_RPF_E                       = 208,
    CPSS_NET_MIRROR_IP_OPTIONS_E                = 209,
    CPSS_NET_MIRROR_ROUTE_E                     = 210,
    CPSS_NET_MIRROR_ICMP_E                      = 211,
    CPSS_NET_MIRROR_HOP_BY_HOP_E                = 212,
    CPSS_NET_MIRROR_EXT_HEADER_E                = 213,
    CPSS_NET_MIRROR_HEADER_ERROR_E              = 214,
    CPSS_NET_MIRROR_ILLIGAL_IP_E                = 215,
    CPSS_NET_MIRROR_SPECIAL_DIP_E               = 216,
    CPSS_NET_MIRROR_IP_SCOP_E                   = 217,
    CPSS_NET_MIRROR_IP_MTU_E                    = 218,
    CPSS_NET_MIRROR_INLIF_E                     = 219,
    CPSS_NET_MIRROR_PCL_MTU_E                   = 220,
    CPSS_NET_MIRROR_DEFAULT_ROUTE_E             = 221,
    CPSS_NET_MIRROR_MC_ROUTE1_E                 = 222,
    CPSS_NET_MIRROR_MC_ROUTE2_E                 = 223,
    CPSS_NET_MIRROR_MC_ROUTE3_E                 = 224,
    CPSS_NET_MIRROR_IPV4_BC_E                   = 225,
    /*End of additional Values for Tiger use only */

    /*Egress pipe */
    CPSS_NET_ETH_BRIDGED_LLT_E                  = 248,
    CPSS_NET_IPV4_ROUTED_LLT_E                  = 249,
    CPSS_NET_UC_MPLS_LLT_E                      = 250,
    CPSS_NET_MC_MPLS_LLT_E                      = 251,
    CPSS_NET_IPV6_ROUTED_LLT_E                  = 252,
    CPSS_NET_L2CE_LLT_E                         = 253,
    CPSS_NET_EGRESS_MIRROR_TO_CPU_E             = 254,
    
    /* addition values for DXSal devices use only */
    CPSS_NET_UNKNOWN_UC_E                       = 261,
    CPSS_NET_UNREG_MC_E                         = 262,

    /* additional values for DXSal,DXCH and EXMXPM devices */
    CPSS_NET_LOCK_PORT_MIRROR_E                 = 271,

    /* addition values for DXCH and EXMXPM devices */
    CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E        = 300,
    CPSS_NET_IPV6_ICMP_PACKET_E                 = 301,
    CPSS_NET_MIRROR_IPV4_RIPV1_TO_CPU_E         = 302,
    CPSS_NET_IPV6_NEIGHBOR_SOLICITATION_E       = 303,
    CPSS_NET_IPV4_BROADCAST_PACKET_E            = 304,
    CPSS_NET_NON_IPV4_BROADCAST_PACKET_E        = 305,
    CPSS_NET_CISCO_MULTICAST_MAC_RANGE_E        = 306,
    CPSS_NET_UNREGISTERED_MULTICAST_E           = 307,
    CPSS_NET_IPV4_UNREGISTERED_MULTICAST_E      = 308,
    CPSS_NET_IPV6_UNREGISTERED_MULTICAST_E      = 309,

    CPSS_NET_UDP_BC_MIRROR_TRAP0_E              = 322,
    CPSS_NET_UDP_BC_MIRROR_TRAP1_E              = 323,
    CPSS_NET_UDP_BC_MIRROR_TRAP2_E              = 324,
    CPSS_NET_UDP_BC_MIRROR_TRAP3_E              = 325,

    CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E      = 326,
    CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E      = 327,
    CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E      = 328,

    CPSS_NET_BRIDGED_PACKET_FORWARD_E           = 400,
    CPSS_NET_INGRESS_MIRRORED_TO_ANLYZER_E      = 401,
    CPSS_NET_EGRESS_MIRRORED_TO_ANLYZER_E       = 402,
    CPSS_NET_MAIL_FROM_NEIGHBOR_CPU_E           = 403,
    CPSS_NET_CPU_TO_CPU_E                       = 404,
    CPSS_NET_EGRESS_SAMPLED_E                   = 405,
    CPSS_NET_INGRESS_SAMPLED_E                  = 406,
    CPSS_NET_INVALID_PCL_KEY_TRAP_E             = 409,

    CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E = 410,
    CPSS_NET_PACKET_TO_VIRTUAL_ROUTER_PORT_E       = 411,
    CPSS_NET_MIRROR_IPV4_UC_ICMP_REDIRECT_E        = 412,
    CPSS_NET_MIRROR_IPV6_UC_ICMP_REDIRECT_E        = 413,
    CPSS_NET_ROUTED_PACKET_FORWARD_E               = 414,
    CPSS_NET_IP_DIP_DA_MISMATCH_E                  = 415,
    CPSS_NET_IP_UC_SIP_SA_MISMATCH_E               = 416,

    CPSS_NET_IPV4_UC_ROUTE1_TRAP_E                 = 417,
    CPSS_NET_IPV4_UC_ROUTE2_TRAP_E                 = 418,
    CPSS_NET_IPV4_UC_ROUTE3_TRAP_E                 = 419,
    CPSS_NET_IPV4_MC_ROUTE0_TRAP_E                 = 420,
    CPSS_NET_IPV4_MC_ROUTE1_TRAP_E                 = 421,
    CPSS_NET_IPV4_MC_ROUTE2_TRAP_E                 = 422,
    CPSS_NET_IPV4_MC_ROUTE3_TRAP_E                 = 423,

    CPSS_NET_IPV6_UC_ROUTE1_TRAP_E                 = 424,
    CPSS_NET_IPV6_UC_ROUTE2_TRAP_E                 = 425,
    CPSS_NET_IPV6_UC_ROUTE3_TRAP_E                 = 426,
    CPSS_NET_IPV6_MC_ROUTE0_TRAP_E                 = 427,
    CPSS_NET_IPV6_MC_ROUTE1_TRAP_E                 = 428,
    CPSS_NET_IPV6_MC_ROUTE2_TRAP_E                 = 430,
    CPSS_NET_IPV6_MC_ROUTE3_TRAP_E                 = 431,
    CPSS_NET_IP_UC_RPF_FAIL_E                      = 432,
    CPSS_NET_ARP_BC_TO_ME_E                        = 433,

    CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E = 434,
    CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E = 435,
    CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E = 436,
    CPSS_NET_SEC_AUTO_LEARN_UNK_SRC_TRAP_E           = 437,
    CPSS_NET_IPV4_TT_HEADER_ERROR_E                  = 438,
    CPSS_NET_IPV4_TT_OPTION_FRAG_ERROR_E             = 439,
    CPSS_NET_IPV4_TT_UNSUP_GRE_ERROR_E               = 440,
    CPSS_NET_ARP_REPLY_TO_ME_E                       = 441,
    CPSS_NET_CPU_TO_ALL_CPUS_E                       = 442,
    CPSS_NET_TCP_SYN_TO_CPU_E                        = 443,

    CPSS_NET_INLIF_MIRROR_E                          = 444,
    CPSS_NET_MC_BRIDGED_PACKET_FORWARD_E             = 445,
    CPSS_NET_MC_ROUTED_PACKET_FORWARD_E              = 446,
    CPSS_NET_L2VPN_PACKET_FORWARD_E                  = 447,
    CPSS_NET_MPLS_PACKET_FORWARD_E                   = 448,
    CPSS_NET_UNREGISTERED_MC_EGRESS_FILTER_E         = 449,
    CPSS_NET_UNKNOWN_UC_EGRESS_FILTER_E              = 450,
    CPSS_NET_ARP_BC_EGRESS_MIRROR_E                  = 451,
    CPSS_NET_VPLS_UNREGISTERED_MC_EGRESS_FILTER_E    = 452,
    CPSS_NET_VPLS_UNKWONW_UC_EGRESS_FILTER_E         = 453,
    CPSS_NET_VPLS_BC_EGRESS_FILTER_E                 = 454,
    CPSS_NET_MC_CPU_TO_CPU_E                         = 455,
    CPSS_NET_PCL_MIRRORED_TO_ANALYZER_E              = 456,
    CPSS_NET_VPLS_UNTAGGED_MRU_FILTER_E              = 457,
    CPSS_NET_TT_MPLS_HEADER_CHECK_E                  = 458,
    CPSS_NET_TT_MPLS_TTL_EXCEED_E                    = 459,
    CPSS_NET_TTI_MIRROR_E                            = 460,
    CPSS_NET_MPLS_SRC_FILTERING_E                    = 461,
    CPSS_NET_IPV4_TTL1_EXCEEDED_E                    = 462,
    CPSS_NET_IPV6_HOPLIMIT1_EXCEED_E                 = 463,


    CPSS_NET_FIRST_USER_DEFINED_E               = 500,
    CPSS_NET_LAST_USER_DEFINED_E = (CPSS_NET_FIRST_USER_DEFINED_E + 63),

    CPSS_NET_FIRST_UNKNOWN_HW_CPU_CODE_E            = 1024,
    CPSS_NET_LAST_UNKNOWN_HW_CPU_CODE_E             = (CPSS_NET_FIRST_UNKNOWN_HW_CPU_CODE_E + 255),

    CPSS_NET_ALL_CPU_OPCODES_E                  = 0x7FFFFFFF
}CPSS_NET_RX_CPU_CODE_ENT;
/*
 * Typedef: enum CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT
 *
 * Description:
 *     Enumerator for modification of packet's attribute
 *     like User Priority and DSCP.
 *
 * Fields:
 *     CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E  - Keep
 *                      previous packet's attribute modification command.
 *     CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E,       - disable
 *                      modification of the packet's attribute.
 *     CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E         - enable
 *                      modification of the packet's attribute.
 *     CPSS_PACKET_ATTRIBUTE_MODIFY_INVALID_E        - invalid value
 *  Comments:
 *
 */
typedef enum
{
    CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E = 0,
    CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E,
    CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E,
    CPSS_PACKET_ATTRIBUTE_MODIFY_INVALID_E
} CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT;
/*
 * Typedef: enum CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT
 *
 * Description:
 *      Enumerator for the packet's attribute assignment precedence
 *      for the subsequent assignment mechanism.
 *
 * Fields:
 *      CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E   - Soft precedence:
 *                        The packet's attribute assignment can be overridden
 *                        by the subsequent assignment mechanism
 *      CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E   - Hard precedence:
 *                        The packet's attribute assignment is locked
 *                        to the last value of attribute assigned to the packet
 *                        and cannot be overridden.
 *
 *  Comments:
 *
 */
typedef enum
{
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E = 0,
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E
} CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT;

/*
 * Typedef: GT_TRUNK_ID
 *
 * Description: Defines trunk id
 *
 *  used as the type for the trunk Id's
 *
 */
typedef unsigned short  GT_TRUNK_ID;
/*
 * Typedef: enum CPSS_INTERFACE_TYPE_ENT
 *
 * Description: enumerator for interface
 *   To be used for:
 *   1. set mac entry info
 *   2. set next hop info
 *   3. redirect pcl info
 *   4. get new Address info
 *
 * Fields:
 *      CPSS_INTERFACE_PORT_E  - the interface is of port type  {dev,port}
 *      CPSS_INTERFACE_TRUNK_E - the interface is of trunk type {trunkId}
 *      CPSS_INTERFACE_VIDX_E  - the interface is of Vidx type  {vidx}
 *      CPSS_INTERFACE_VID_E   - the interface is of Vid type  {vlan-id}
 */
typedef enum {
    CPSS_INTERFACE_PORT_E = 0,
    CPSS_INTERFACE_TRUNK_E,
    CPSS_INTERFACE_VIDX_E,
    CPSS_INTERFACE_VID_E
}CPSS_INTERFACE_TYPE_ENT;
/*
 * Typedef: enum CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT
 *
 * Description: Enumerator for the packet's attribute assignment command.
 *
 * Fields:
 *  CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E     - packet's attribute assignment
 *                         disabled
 *  CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E   - packet's attribute assignment
 *                        only if the packet is VLAN tagged.
 *  CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E - packet's attribute assignment
 *                        only if the packet is untagged or Priority-tagged.
 *  CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E      - packet's attribute assignment
 *                        regardless of packet tagging state.
 *
 */
typedef enum
{
    CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
    CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E,
    CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E,
    CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E
} CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT;


/*
 * Typedef: enum CPSS_PACKET_CMD_ENT
 *
 * Description:
 *     This enum defines the packet command.
 * Fields:
 *     CPSS_PACKET_CMD_FORWARD_E           - forward packet
 *     CPSS_PACKET_CMD_MIRROR_TO_CPU_E     - mirror packet to CPU
 *     CPSS_PACKET_CMD_TRAP_TO_CPU_E       - trap packet to CPU
 *     CPSS_PACKET_CMD_DROP_HARD_E         - hard drop packet
 *     CPSS_PACKET_CMD_ROUTE_E             - IP Forward the packets
 *     CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E  - Packet is routed and mirrored to
 *                                           the CPU.
 *     CPSS_PACKET_CMD_DROP_SOFT_E         - soft drop packet
 *     CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E - Bridge and Mirror to CPU.
 *     CPSS_PACKET_CMD_BRIDGE_E            - Bridge only
 *     CPSS_PACKET_CMD_NONE_E              - Do nothing. (disable)
 *     CPSS_PACKET_CMD_INVALID_E        - invalid packet command
 *
 *  Comments:
 */
typedef enum
{
    CPSS_PACKET_CMD_FORWARD_E          , /* 00 */
    CPSS_PACKET_CMD_MIRROR_TO_CPU_E    , /* 01 */
    CPSS_PACKET_CMD_TRAP_TO_CPU_E      , /* 02 */
    CPSS_PACKET_CMD_DROP_HARD_E        , /* 03 */
    CPSS_PACKET_CMD_DROP_SOFT_E        , /* 04 */
    CPSS_PACKET_CMD_ROUTE_E            , /* 05 */
    CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E , /* 06 */
    CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E, /* 07 */
    CPSS_PACKET_CMD_BRIDGE_E           , /* 08 */
    CPSS_PACKET_CMD_NONE_E             , /* 09 */
    CPSS_PACKET_CMD_INVALID_E            /* 10 */
} CPSS_PACKET_CMD_ENT;

/*
 * Typedef: structure GT_ETHERADDR
 *
 * Description: Defines the mac address
 *
 * Fields:
 *      arEther - the mac as 6 bytes - NETWORK ORDER
 *
 */
/*
 * Typedef: union GT_IPADDR
 *
 * Description: Defines the IP address
 *
 * Fields:
 *      u32Ip - the IP as single WORD
 *      arIP  - the IP as 4 bytes - NETWORK ORDER
 */
typedef union
{
    unsigned long  u32Ip;
    unsigned char   arIP[4];
}GT_IPADDR;
	
/*#define GT_IPADDR unsigned long */
/*
 * Typedef: union GT_IPV6ADDR
 *
 * Description: Defines the IPv6 address
 *
 * Fields:
 *      u32Ip - the IP as 4 WORDs
 *      arIP  - the IP as 16 bytes - NETWORK ORDER
 */
typedef union
{
    unsigned long u32Ip[4];
    unsigned char  arIP[16];
}GT_IPV6ADDR;
/*
 * Typedef: enum CPSS_PCL_RULE_SIZE_ENT
 *
 * Description:
 *     This enum describes sizes of Policy Rules.
 * Fields:
 *     CPSS_PCL_RULE_SIZE_STD_E           - standard size rule
 *     CPSS_PCL_RULE_SIZE_EXT_E           - extended size rule
 *
 *  Comments:
 */
typedef enum
{
    CPSS_PCL_RULE_SIZE_STD_E,
    CPSS_PCL_RULE_SIZE_EXT_E
} CPSS_PCL_RULE_SIZE_ENT;
/*
 * typedef: CPSS_PCL_DIRECTION_ENT
 *
 * Description:
 *      PCL direction.
 * Fields:
 *      CPSS_PCL_DIRECTION_INGRESS - The PCL will work on ingress traffic.
 *      CPSS_PCL_DIRECTION_EGRESS  - The PCL will work on egress traffic.
 *
 * Comment:
 */
typedef enum
{
    CPSS_PCL_DIRECTION_INGRESS_E = 0,
    CPSS_PCL_DIRECTION_EGRESS_E
} CPSS_PCL_DIRECTION_ENT;
/*
 * typedef: CPSS_PCL_LOOKUP_NUMBER_ENT
 *
 * Description:
 *      Number of lookup for the PCL
 *
 * Fields:
 *      CPSS_PCL_LOOKUP_0_E - first lookup #0.
 *      CPSS_PCL_LOOKUP_1_E - second lookup #1.
 *
 * Comment:
 */
typedef enum
{
    CPSS_PCL_LOOKUP_0_E = 0,
    CPSS_PCL_LOOKUP_1_E
} CPSS_PCL_LOOKUP_NUMBER_ENT;

/*
 * Typedef: structure CPSS_INTERFACE_INFO_STC
 *
 * Description: Defines the interface info
 *   To be used for:
 *   1. set mac entry info
 *   2. set next hop info
 *   3. redirect pcl info
 *   4. get new Address info
 *
 * Fields:
 *      type - the interface type -- port/trunk/vidx
 *
 *      !!!! NOTE : next files treated as UNION !!!!
 *
 *      devPort - info about the {dev,port} - relevant to CPSS_INTERFACE_PORT_E
 *          devNum - the device number
 *          portNum - the port number
 *      trunkId - info about the {trunkId} - relevant to CPSS_INTERFACE_TRUNK_E
 *      vidx   -  info about the {vidx} - relevant to CPSS_INTERFACE_VIDX_E
 *      vlanId -  info about the {vid}  - relevant to CPSS_INTERFACE_VID_E
 *
 */
typedef struct{
    CPSS_INTERFACE_TYPE_ENT     type;

    struct{
        unsigned char   devNum;
        unsigned char   portNum;
    }devPort;

    GT_TRUNK_ID  trunkId;
    unsigned short       vidx;
    unsigned short       vlanId;
#ifdef CPU_ARM_XCAT
	unsigned char		 devNum;
	unsigned short		 fabricVidx;
	unsigned int		 index;
#endif
}CPSS_INTERFACE_INFO_STC;
/* Action definitions for Policy Rules */

/*
 * Typedef: struct CPSS_DXCH_PCL_ACTION_MIRROR_STC
 *
 * Description:
 *     This structure defines the mirroring related Actions.
 *
 * Fields:
 *       mirrorToRxAnalyzerPort - Enables mirroring the packet to
 *              the ingress analyzer port.
 *              GT_FALSE = Packet is not mirrored to ingress analyzer port.
 *              GT_TRUE = Packet is mirrored to ingress analyzer port.
 *       cpuCode                - The CPU code assigned to packets
 *              Mirrored to CPU or Trapped to CPU due
 *              to a match in the Policy rule entry
 *
 *  Comments:
 *
 */
typedef struct
{
    CPSS_NET_RX_CPU_CODE_ENT      cpuCode;
    unsigned long                 mirrorToRxAnalyzerPort;
} CPSS_DXCH_PCL_ACTION_MIRROR_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_ACTION_MATCH_COUNTER_STC
 *
 * Description:
 *     This structure defines the using of rule match counter.
 *
 * Fields:
 *       enableMatchCount  - Enables the binding of this
 *                  policy action entry to the Policy Rule Match Counter<n>
 *                  (0<=n<32) indexed by matchCounterIndex
 *                  GT_FALSE  = Match counter binding is disabled.
 *                  GT_TRUE = Match counter binding is enabled.
 *
 *       matchCounterIndex - A index one of the 32 Policy Rule Match Counter<n>
 *                 (0<=n<32) The counter is incremented for every packet
 *                 satisfying both of the following conditions:
 *                    - Matching this rule.
 *                    - The previous packet command is not hard drop.
 *
 *  Comments:
 *
 */
typedef struct
{
    char   enableMatchCount;
    unsigned long    matchCounterIndex;
} CPSS_DXCH_PCL_ACTION_MATCH_COUNTER_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_ACTION_QOS_STC
 *
 * Description:
 *     This structure defines the packet's QoS attributes mark Actions.
 *
 * Fields:
 *
 *    modifyDscp             - For Ingress Policy:
 *                               The Modify DSCP QoS attribute of the packet.
 *                             Enables modification of the packet's DSCP field.
 *                             Only relevant if QoS precedence of the previous
 *                             QoS assignment mechanisms (Port, Protocol Based
 *                             QoS, and previous matching rule) is Soft.
 *                             Relevant for IPv4/IPv6 packets, only.
 *                             ModifyDSCP enables the following:
 *                               - Keep previous DSCP modification command.
 *                               - Enable modification of the DSCP field in
 *                                 the packet.
 *                               - Disable modification of the DSCP field in
 *                                 the packet.
 *
 *                             For Egress Policy:
 *                               Enables modification of the IP Header DSCP field
 *                             to egress.dscp of the transmitted packet.
 *                             ModifyDSCP enables the following:
 *                             - Keep previous packet DSCP setting.
 *                             - Modify the Packet's DSCP to <egress.dscp>.
 *
 *    modifyUp               - For Ingress Policy:
 *                            The Modify UP QoS attribute of the packet.
 *                            Enables modification of the packet's
 *                            802.1p User Priority field.
 *                            Only relevant if QoS precedence of the previous
 *                            QoS assignment mechanisms  (Port, Protocol Based
 *                            QoS, and previous matching rule) is Soft.
 *                            ModifyUP enables the following:
 *                            - Keep previous QoS attribute <ModifyUP> setting.
 *                            - Set the QoS attribute <modifyUP> to 1.
 *                            - Set the QoS attribute <modifyUP> to 0.
 *
 *                              For Egress Policy:
 *                            Enables the modification of the 802.1p User
 *                            Priority field to egress.up of packet
 *                            transmitted with a VLAN tagged.
 *                            ModifyUP enables the following:
 *                            - Keep previous QoS attribute <ModifyUP> setting.
 *                            - Modify the Packet's UP to <egress.up>.
 *
 *    egress.dscp            - The DSCP field set to the transmitted packets.
 *                             Relevant for Egress Policy only.
 *    egress.up              - The 802.1p UP field set to the transmitted packets.
 *                             Relevant for Egress Policy only.
 *    ingress members:
 *    profileIndex           - The QoS Profile Attribute of the packet.
 *                             Only relevant if the QoS precedence of the
 *                             previous QoS Assignment Mechanisms (Port,
 *                             Protocol Based QoS, and previous matching rule)
 *                             is Soft and profileAssignIndex is set
 *                             to GT_TRUE.the QoSProfile is used to index the
 *                             QoSProfile to QoS Table Entry<n> (0<=n<72)
 *                             and map the QoS Parameters for the packet,
 *                             which are TC, DP, UP and DSCP
 *                             Valid Range - 0 through 71
 *    profileAssignIndex     - Enable marking of QoS Profile Attribute of
 *                             the packet.
 *                             GT_TRUE - Assign <profileIndex> to the packet.
 *                             GT_FALSE - Preserve previous QoS Profile setting.
 *    profilePrecedence      - Marking of the QoSProfile Precedence.
 *                             Setting this bit locks the QoS parameters setting
 *                             from being modified by subsequent QoS
 *                             assignment engines in the ingress pipe.
 *                             QoSPrecedence enables the following:
 *                             - QoS precedence is soft and the packet's QoS
 *                               parameters may be overridden by subsequent
 *                               QoS assignment engines.
 *                             - QoS precedence is hard and the packet's QoS
 *                               parameters setting is performed until
 *                               this stage is locked. It cannot be overridden
 *                               by subsequent QoS assignment engines.
 *
 *  Comments:
 *           Egress Policy related actions supported only for DxCh2 devices
 *
 */
typedef struct
{
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT      modifyDscp;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT      modifyUp;
    union
    {
        struct
        {
            unsigned long                                      profileIndex;
            unsigned long                                     profileAssignIndex;
            CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT profilePrecedence;
        } ingress;
        struct
        {
            unsigned char                              dscp;
            unsigned char                              up;
        } egress;
    } qos;
} CPSS_DXCH_PCL_ACTION_QOS_STC;

/*
 * Typedef: enum CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT
 *
 * Description: enumerator for PCL redirection target
 *
 * Fields:
 *  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E   - no redirection
 *  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E - redirection to output interface
 *  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E - Ingress control pipe not
 *                        bypassed. If packet is triggered for routing,
 *                        Redirect the packet to the Router Lookup Translation
 *                        Table Entry specified in <routerLttIndex> bypassing
 *                        DIP lookup.
 */
typedef enum
{
    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E,
    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E
} CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT;

/*
 * Typedef: struct CPSS_DXCH_PCL_ACTION_REDIRECT_STC
 *
 * Description:
 *     This structure defines the redirection related Actions.
 *
 * Fields:
 *       redirectCmd   - redirection command
 *
 *       data.outIf   - out interface redirection data
 *                      relevant for CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_IF_E
 *                      packet redirected to output interface
 *       outInterface - output interface (port, trunk, VID, VIDX)
 *       vntL2Echo    - Enables Virtual Network Tester Layer 2 Echo
 *                      GT_TRUE - swap MAC SA and MAC DA in redirected packet
 *                      GT_FALSE - don't swap MAC SA and MAC DA
 *       tunnelStart  - Indicates this action is a Tunnel Start point
 *                      GT_TRUE - Packet is redirected to an Egress Interface
 *                                and is tunneled as an  Ethernet-over-MPLS.
 *                      GT_FALSE - Packet is redirected to an Egress Interface
 *                                and is not tunneled.
 *       tunnelPtr    - the pointer to "Tunnel-start" entry
 *                      (relevant only if tunnelStart == GT_TRUE)
 *
 *       data.routerLttIndex  - index of IP router Lookup Translation Table entry
 *                       relevant for CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E
 *
 *
 *  Comments:
 *      Tunnel, VNTL2Echo and Router redirection related actions
 *      supported only for DxCh2 devices
 *
 */
typedef struct
{
    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT  redirectCmd;
    union
    {
        struct
        {
            CPSS_INTERFACE_INFO_STC  outInterface;
            unsigned long                  vntL2Echo;
            unsigned long                  tunnelStart;
            unsigned long                   tunnelPtr;
        } outIf;
        unsigned long                   routerLttIndex;
    } data;

} CPSS_DXCH_PCL_ACTION_REDIRECT_STC;


/*
 * Typedef: struct CPSS_DXCH_PCL_ACTION_POLICER_STC
 *
 * Description:
 *     This structure defines the policer related Actions.
 *
 * Fields:
 *       policerEnable  - GT_TRUE - policer enable,
 *                        GT_FALSE - policer disable
 *       policerId      - policers table entry index
 *
 *  Comments:
 *
 */
typedef struct
{
    unsigned long  policerEnable;
    unsigned long   policerId;

} CPSS_DXCH_PCL_ACTION_POLICER_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_ACTION_VLAN_STC
 *
 * Description:
 *     This structure defines the VLAN modification related Actions.
 *
 * Fields:
 *       modifyVlan  - VLAN id modification command
 *
 *       nestedVlan  -  When this field is set to GT_TRUE, this rule matching
 *                      flow is defined as an access flow. The VID of all
 *                      packets received on this flow is discarded and they
 *                      are assigned with a VID using the device's VID
 *                      assignment algorithms, as if they are untagged. When
 *                      a packet received on an access flow is transmitted via
 *                      a core port or a Cascading port, a VLAN tag is added
 *                      to the packet (on top of the existing VLAN tag, if
 *                      any). The VID field is the VID assigned to the packet
 *                      as a result of all VLAN assignment algorithms. The
 *                      802.1p User Priority field of this added tag may be
 *                      one of the following, depending on the ModifyUP QoS
 *                      parameter set to the packet at the end of the Ingress
 *                      pipe:
 *                      - If ModifyUP is GT_TRUE, it is the UP extracted
 *                      from the QoSProfile to QoS Table Entry<n>
 *                      - If ModifyUP is GT_FALSE, it is the original packet
 *                      802.1p User Priority field if it is tagged and is UP
 *                      if the original packet is untagged.
 *
 *       vlanId      - VLAN id used for VLAN id modification if command
 *                     not CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E 
 *       precedence  - The VLAN Assignment precedence for the subsequent
 *                     VLAN assignment mechanism, which is the Policy engine
 *                     next policy-pass rule. Only relevant if the
 *                     VID precedence set by the previous VID assignment
 *                     mechanisms (Port, Protocol Based VLANs, and previous
 *                     matching rule) is Soft.
 *                     - Soft precedence The VID assignment can be overridden
 *                       by the subsequent VLAN assignment mechanism,
 *                       which is the Policy engine.
 *                     - Hard precedence The VID assignment is locked to the
 *                       last VLAN assigned to the packet and cannot be overridden.
 *
 *  Comments:
 *
 */
typedef struct
{
    CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT          modifyVlan;
    unsigned long                                       nestedVlan;
    unsigned short                                        vlanId;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT   precedence;

} CPSS_DXCH_PCL_ACTION_VLAN_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_ACTION_IP_UC_ROUTE_STC
 *
 * Description:
 *     This structure defines the IP unicast route parameters.
 *     Dedicated to override the relevant field of
 *     general action definitions.
 *
 * Fields:
 *       doIpUcRoute       - Configure IP Unicast Routing Actions
 *                           GT_TRUE - the action used for IP unicast routing
 *                           GT_FALSE - the action is not used for IP unicast
 *                                      routing, all data of the structure
 *                                      ignored.
 *       arpDaIndex        - Route Entry ARP Index to the ARP Table (10 bit)
 *       decrementTTL      - Decrement IPv4 <TTL> or IPv6 <Hop Limit> enable
 *                           GT_TRUE - TTL Decrement for routed packets is enabled
 *                           GT_FALSE - TTL Decrement for routed packets is disabled
 *       bypassTTLCheck    - Bypass Router engine TTL and Options Check
 *                           GT_TRUE - the router engine bypasses the
 *                                     IPv4 TTL/Option check and the IPv6 Hop
 *                                     Limit/Hop-by-Hop check. This is used for
 *                                     IP-TO-ME host entries, where the packet
 *                                     is destined to this device
 *                           GT_FALSE - the check is not bypassed
 *       icmpRedirectCheck - ICMP Redirect Check Enable
 *                           GT_TRUE - the router engine checks if the next hop
 *                                     VLAN is equal to the packet VLAN
 *                                     assignment, and if so, the packet is
 *                                     mirrored to the CPU, in order to send an
 *                                     ICMP Redirect message back to the sender.
 *                          GT_FALSE - the check disabled
 *
 *  Comments:
 *  Supported for DxCh only (not supported for DxCh2).
 *  To configure IP Unicast route entry next elements of the action struct
 *  should be configured
 *    1. doIpUcRoute set to GT_TRUE
 *    2. redirection action set to CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E
 *      Out Interface data should be configured too.
 *    3. Policer should be disabled
 *    4. VLAN command CPSS_DXCH_PCL_ACTION_VLAN_CMD_MODIFY_ALL_E
 *    5. Nested VLAN should be disabled
 *    6. The packet command <pktCmd> should be set according to route entry
 *        purpose:
 *        - SOFT_DROP       - packet is dropped
 *        - HARD_DROP       - packet is dropped
 *        - TRAP            - packet is trapped to CPU with CPU code
 *                            IPV4_UC_ROUTE or IPV6_UC_ROUTE
 *        - FORWARD         - packet is routed
 *        - MIRROR_TO_CPU   - packet is routed and mirrored to the CPU with
 *                            CPU code IPV4_UC_ROUTE or IPV6_UC_ROUTE
 *
 */
typedef struct
{
     unsigned long  doIpUcRoute;
     unsigned long   arpDaIndex;
     unsigned long  decrementTTL;
     unsigned long  bypassTTLCheck;
     unsigned long  icmpRedirectCheck;

} CPSS_DXCH_PCL_ACTION_IP_UC_ROUTE_STC;

/*
 * typedef: struct CPSS_DXCH_PCL_ACTION_STC
 *
 * Description:
 *      Policy Engine Action
 *
 * Fields:
 *      pktCmd       -  packet command (forward, mirror-to-cpu,
 *                      hard-drop, soft-drop, or trap-to-cpu)
 *      egressPolicy - GT_TRUE  - Action is used for the Egress Policy
 *                     GT_FALSE - Action is used for the Ingress Policy
 *      mirror       -  packet mirroring configuration
 *      matchCounter -  match counter configuration
 *      qos          -  packet QoS attributes modification configuration
 *      redirect     -  packet Policy redirection configuration
 *      policer      -  packet Policing configuration
 *      vlan         -  packet VLAN modification configuration
 *      ipUcRoute    -  special DxCh (not relevant for DxCh2) Ip Unicast Route
 *                      action parameters configuration
 *
 * Comment:
 */
typedef struct
{
    CPSS_PACKET_CMD_ENT                    pktCmd;
    unsigned long                          egressPolicy;
    CPSS_DXCH_PCL_ACTION_MIRROR_STC        mirror;
    CPSS_DXCH_PCL_ACTION_MATCH_COUNTER_STC matchCounter;
    CPSS_DXCH_PCL_ACTION_QOS_STC           qos;
    CPSS_DXCH_PCL_ACTION_REDIRECT_STC      redirect;
    CPSS_DXCH_PCL_ACTION_POLICER_STC       policer;
    CPSS_DXCH_PCL_ACTION_VLAN_STC          vlan;
    CPSS_DXCH_PCL_ACTION_IP_UC_ROUTE_STC   ipUcRoute;

} CPSS_DXCH_PCL_ACTION_STC;


/*
 * Typedef: enum CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT
 *
 * Description:
 *     This enum describes possible formats of Policy rules.
 * Fields:
 *     key formats for Ingress
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E    - Standard (24B) L2
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E - Standard (24B)
 *                          L2+IPv4/v6 QoS
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E   - Standard (24B)
 *                          IPv4+L4
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E  - Standard (24B)
 *                          DxCh specific IPV6 DIP (used for routing)
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E  - Extended (48B)
 *                          L2+IPv4 + L4
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E   - Extended (48B)
 *                          L2+IPv6
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E   - Extended (48B)
 *                          L4+IPv6
 *     key formats for Egress (DxCh2 only)
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E     - Standard (24B) L2
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E  - Standard (24B)
 *                          L2+IPv4/v6 QoS
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E    - Standard (24B)
 *                          IPv4+L4
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E   - Extended (48B)
 *                          L2+IPv4 + L4
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E    - Extended (48B)
 *                          L2+IPv6
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E    - Extended (48B)
 *                          L4+IPv6
 *
 *  Comments:
 */
typedef enum
{
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E
} CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT;

/*
 * Typedef: enum CPSS_DXCH_PCL_OFFSET_TYPE_ENT
 *
 * Description: Offset types for packet headers parsing used for user defined
 *              bytes configuration
 *
 * Enumerations:
 *    CPSS_DXCH_PCL_OFFSET_L2_E       - offset from start of mac(L2) header
 *    CPSS_DXCH_PCL_OFFSET_L3_E       - offset from start of L3 header.
 *    CPSS_DXCH_PCL_OFFSET_L4_E       - offset from start of L4 header
 *    CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E - offset from start of
 *                                          IPV6 Extention Header .
 *    CPSS_DXCH_PCL_OFFSET_TCP_UDP_COMPARATOR_E - the user defined byte used
 *                   for TCP or UDP comparator, not for byte from packet
 *
 * Comment:
 *      all possible valid UDB configurations:
 *      key-format, UDB-index, (valid offset types list)
 *      STD_NOT_IP,    0,   (L2, L3)
 *      STD_NOT_IP,    1,   (L2, L3)
 *      STD_NOT_IP,    2,   (L3)
 *      STD_L2_IP_QOS, 0,   (L2, L3, L4, IPV6_EH, TCP_UDP_COMPR)
 *      STD_L2_IP_QOS, 1,   (L2, L3, L4, IPV6_EH)
 *      STD_IPV4_L4,   0,   (L2, L3, L4, TCP_UDP_COMPR)
 *      STD_IPV4_L4,   1,   (L2, L3, L4)
 *      STD_IPV4_L4,   2,   (L3, L4)
 *      EXT_NOT_IPV6,  0,   (L2, L3, L4)
 *      EXT_NOT_IPV6,  1,   (L2, L3, L4)
 *      EXT_NOT_IPV6,  2,   (L3, L4, TCP_UDP_COMPR)
 *      EXT_NOT_IPV6,  3,   (L3, L4)
 *      EXT_NOT_IPV6,  4,   (L3, L4)
 *      EXT_NOT_IPV6,  5,   (L3, L4)
 *      EXT_IPV6_L2,   0,   (L2, L3, L4, IPV6_EH)
 *      EXT_IPV6_L2,   1,   (L2, L3, L4, IPV6_EH)
 *      EXT_IPV6_L2,   2,   (L3, L4, IPV6_EH, TCP_UDP_COMPR)
 *      EXT_IPV6_L2,   3,   (L3, L4, IPV6_EH)
 *      EXT_IPV6_L2,   4,   (L3, L4, IPV6_EH)
 *      EXT_IPV6_L2,   5,   (L3, L4, IPV6_EH)
 *      EXT_IPV6_L4,   0,   (L2, L3, L4, IPV6_EH)
 *      EXT_IPV6_L4,   1,   (L2, L3, L4, IPV6_EH)
 *      EXT_IPV6_L4,   2,   (L3, L4, TCP_UDP_COMPR)
  */
typedef enum
{
    CPSS_DXCH_PCL_OFFSET_L2_E,
    CPSS_DXCH_PCL_OFFSET_L3_E,
    CPSS_DXCH_PCL_OFFSET_L4_E,
    CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E,
    CPSS_DXCH_PCL_OFFSET_TCP_UDP_COMPARATOR_E

} CPSS_DXCH_PCL_OFFSET_TYPE_ENT;

/*
 * Typedef: enum CPSS_DXCH_UDB_ERROR_CMD_ENT
 *
 * Description:
 *     This enum defines packet command taken in the UDB error case.
 * Fields:
 *     CPSS_DXCH_UDB_ERROR_CMD_LOOKUP_E         - continue Policy Lookup
 *     CPSS_DXCH_UDB_ERROR_CMD_TRAP_TO_CPU_E    - trap packet to CPU
 *     CPSS_DXCH_UDB_ERROR_CMD_DROP_HARD_E      - hard drop packet
 *     CPSS_DXCH_UDB_ERROR_CMD_DROP_SOFT_E      - soft drop packet
 *
 *  Comments:
 */
typedef enum
{
    CPSS_DXCH_UDB_ERROR_CMD_LOOKUP_E,
    CPSS_DXCH_UDB_ERROR_CMD_TRAP_TO_CPU_E,
    CPSS_DXCH_UDB_ERROR_CMD_DROP_HARD_E,
    CPSS_DXCH_UDB_ERROR_CMD_DROP_SOFT_E
} CPSS_DXCH_UDB_ERROR_CMD_ENT;

/*
 * Typedef: enum CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_ENT
 *
 * Description:
 *     This enum defines the ip-total-length check algorithm
 *     that used for calculalate the key "IP Header OK" bit.
 * Fields:
 *       CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L3_E    - compare
 *             ip-total-lengts with the size of L3 level part of the packet
 *       CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L2_E    - compare
 *             ip-total-lengts with the total size of the packet
 *
 *  Comments:
 */
typedef enum
{
    CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L3_E,
    CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L2_E
} CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_ENT;

/*-------------------------------------------------------------------------*/
/*                           Policy Rules Formats                          */
/*-------------------------------------------------------------------------*/

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC
 *
 * Description:
 *     This structure describes the common segment of all key formats.
 *
 * Fields:
 *     pclId       - The PCL-ID attribute assigned to this lookup.
 *     sourcePort  - The port number from which the packet ingressed the device.
 *                   Port 63 is the CPU port.
 *     isTagged    - Flag indicating the packet Tag state
 *                   For Ingress PCL Keys:
 *                   For non-tunnel terminated packets:
 *                   - If packet was received on DSA-tagged, this field is set
 *                     to the DSA tag <SrcTagged> field.
 *                   - If packet was received non-DSA-tagged, this field is set
 *                     to 1 if the packet was received  VLAN or Priority-tagged.
 *                   For Ethernet-Over-xxx tunnel-terminated packets: Passenger
 *                   packet VLAN tag format.
 *
 *                   For Egress PCL Keys:
 *                   For non Tunnel Start packets: Transmitted packet's VLAN tag
 *                   format after tag addition/removal/modification
 *                   For Ethernet-Over-xxx Tunnel Start packets:
 *                   Passenger packet's VLAN tag format after tag
 *                   addition/removal/modification.
 *                   0 = Packet is untagged.
 *                   1 = Packet is tagged.
 *
 *     vid         - VLAN ID assigned to the packet.
 *     up          - The packet's 802.1p User Priority field.
 *     qosProfile  - The QoS Profile assigned to the packet until this lookup
 *                   stage, as assigned by the device,
 *                   according to the QoS marking algorithm
 *     isIp        - An indication that the packet is IP
 *                   0 = Non IP packet.
 *                   1 = IPv4/6 packet.
 *     isL2Valid   - Indicates that Layer 2 information in the search key is valid.
 *                   0 = Layer 2 fields in the Ingress PCL (IPCL) or
 *                       Egress PCL (EPCL) are not valid
 *                   1 = Layer 2 field in the Ingress PCL or Egress PCL key is valid.
 *                     For Ingress PCL Keys: Layer 2 fields are not valid in the
 *                   IPCL keys when IP-Over-xxx is tunnel-terminated and its key
 *                   is based on the passenger IP packet which does not include
 *                   a Layer 2 header.
 *                     For Egress PCL Keys: Layer 2 fields are not valid in
 *                   the EPCL keys when an IP-Over-xxx is forwarded to a Tunnel
 *                   Start, and its PCL Key is based on the passenger IP packet
 *                   which does not include Layer 2 data.
 *                     Supported by DxCh2 and above.
 *                   DxCh1 devices ignores the field.
 *
 *     isUdbValid  - User-defined bytes (UDBs) are valid.
 *                   Indicates that all user-defined bytes used in that search
 *                   key were successfully parsed.
 *                   0 = At least 1 user-defined byte couldn't be parsed.
 *                   1 = All user-defined bytes used in this search key
 *                   were successfully parsed.
 *
 *  Comments:
 */
typedef struct
{
    unsigned short     pclId;
    unsigned char      sourcePort;
    unsigned char      isTagged;
    unsigned short     vid;
    unsigned char      up;
    unsigned char      qosProfile;
    unsigned char      isIp;
    unsigned char      isL2Valid;
    unsigned char      isUdbValid;
} CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *
 * Description:
 *     This structure describes the common segment of all extended key formats.
 *
 * Fields:
 *     isIpv6     - An indication that the packet is IPv6.
 *                  0 = Non IPv6 packet.
 *                  1 = IPv6 packet.
 *     ipProtocol - IP protocol/Next Header type.
 *     dscp       - The DSCP field of the IPv4/6 header.
 *     isL4Valid  - Layer 4 information is valid.
 *                  This field indicates that all the Layer 4
 *                  information required for the search key is available
 *                  and the IP header is valid.
 *                  0 = Layer 4 information is not valid.
 *                  1 = Layer 4 information is valid.
 *                  Layer 4 information may not be available for any of the
 *                   following reasons:
 *                  - Layer 4 information is not included in the packet.
 *                     For example, Layer 4 information
 *                     isn't available in non-IP packets, or in IPv4
 *                     non-initial-fragments.
 *                  - Parsing failure: Layer 4 information is beyond
 *                     the first 128B of the packet, or beyond
 *                     IPv6 extension headers parsing capabilities.
 *                  - IP header is invalid.
 *     l4Byte0    - The following Layer 4 information is available for
 *                  UDP and TCP packets - L4 Header Byte0 through Byte3,
 *                  which contain the UDP/TCP destination and source ports.
 *                  For TCP also L4 Header Byte13, which contains the TCP flags.
 *                  For IGMP L4 Header Byte0, which contain the IGMP Type.
 *                  For ICMP L4 Header Byte0 and Byte1,
 *                  which contain the ICMP Type and Code fields
 *                  and L4 Header Byte4<in l4Byte2> and Byte5 <in l4Byte3>,
 *                  which contain the ICMP authentication field.
 *                  The following Layer 4 information is available for packets
 *                  that are not TCP, UDP, IGMP or ICMP:
 *                  L4 Header Byte0 through Byte3 L4 Header Byte13
 *     l4Byte1    - see l4Byte0
 *     l4Byte2    - see l4Byte0
 *     l4Byte3    - see l4Byte0
 *     l4Byte13   - see l4Byte0
 *     ipHeaderOk - Indicates a valid IP header.
 *                  0 = Packet IP header is invalid.
 *                  1 = Packet IP header is valid.
 *
 *  Comments:
 */
typedef struct
{
    unsigned char      isIpv6;
    unsigned char      ipProtocol;
    unsigned char      dscp;
    unsigned char      isL4Valid;
    unsigned char      l4Byte0;
    unsigned char      l4Byte1;
    unsigned char      l4Byte2;
    unsigned char      l4Byte3;
    unsigned char      l4Byte13;
    unsigned char      ipHeaderOk;

} CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC
 *
 * Description:
 *     This structure describes the common segment
 *     of all standard IP key formats.
 *
 * Fields:
 *     isIpv4      - An indication that the packet is IPv4.
 *                  0 = Non IPv4 packet.
 *                  1 = IPv4 packet.
 *     ipProtocol  - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     dscp        - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     isL4Valid   - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte2     - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte3     - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     ipHeaderOk  - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     ipv4Fragmented - Identifies an IPv4 fragment.
 *                      0 = Not an IPv4 packet or not an IPv4 fragment.
 *                      1 = Packet is an IPv4 fragment (could be
 *                      the first fragment or any subsequent fragment)
 *
 *  Comments:
 */
typedef struct
{
    unsigned char      isIpv4;
    unsigned char      ipProtocol;
    unsigned char      dscp;
    unsigned char      isL4Valid;
    unsigned char      l4Byte2;
    unsigned char      l4Byte3;
    unsigned char      ipHeaderOk;
    unsigned char      ipv4Fragmented;

} CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *
 * Description:
 *     This structure describes the standard not-IP key.
 * Fields:
 *     common     - the common part for all formats (see above)
 *     isIpv4      - An indication that the packet is IPv4.
 *                  0 = Non IPv4 packet.
 *                  1 = IPv4 packet.
 *     etherType  - Valid when <IsL2Valid> =1.
 *                  If <L2 Encap Type> = 0, this field contains the
 *                  <Dsap-Ssap> of the LLC NON-SNAP packet.
 *                  If <L2 Encap Type> = 1, this field contains the <EtherType/>
 *                  of the Ethernet V2 or LLC with SNAP packet.
 *     isArp      - An indication that the packet is an ARP packet
 *                   (identified by EtherType == 0x0806.)
 *                  0 = Non ARP packet.
 *                  1 = ARP packet.
 *     l2Encap    - The Layer 2 encapsulation of the packet.
 *                   0 = The L2 Encapsulation is LLC NON-SNAP.
 *                   1 = The L2 Encapsulation is Ethernet V2 or LLC with SNAP.
 *     macDa      - Ethernet Destination MAC address.
 *     macSa      - Ethernet Source MAC address.
 *     udb        - User Defined Bytes with indexes 0,1,2
 *
 *  Comments:
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC common;
    unsigned char                                isIpv4;
    unsigned short                               etherType;
    unsigned char                                isArp;
    unsigned char                                l2Encap;
    GT_ETHERADDR                         macDa;
    GT_ETHERADDR                         macSa;
    unsigned char                                udb[3];

} CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC
 *
 * Description:
 *     This structure describes the standard IPV4/V6 L2_QOS key.
 *
 * Fields:
 *     common            - the common part for all formats (see upper)
 *     commonStdIp       - the common part for all stanard IP formats
 *     isArp             - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *     isIpv6ExtHdrExist - IPv6 extension header exists.
 *                         Indicates that an IPv6 extension exists.
 *                         0 = Non-Ipv6 packet or IPv6 extension header does
 *                         not exist.
 *                         1 = Packet is IPv6 and extension header exists.
 *     isIpv6HopByHop    - Indicates that the IPv6 Original Extension Header
 *                          is Hop-by-Hop.
 *                         0 = Non-IPv6 packet or IPv6 extension header type is
 *                          not Hop-by-Hop Option Header.
 *                         1 = Packet is IPv6 and extension header type
 *                          is Hop-by-Hop Option Header.
 *     macDa             - Ethernet Destination MAC address.
 *     macSa             - Ethernet Source MAC address.
 *     udb               - User Defined Bytes with indexes 0,1
 *
 *  Comments:
 *  udb[0] may contain the TCP/UDP comparator result
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC        common;
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC commonStdIp;
    unsigned char                                       isArp;
    unsigned char                                       isIpv6ExtHdrExist;
    unsigned char                                       isIpv6HopByHop;
    GT_ETHERADDR                                macDa;
    GT_ETHERADDR                                macSa;
    unsigned char                                       udb[2];

} CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC
 *
 * Description:
 *     This structure describes the standard IPV4_L4 key.
 *
 * Fields:
 *     common      - the common part for all formats (see upper)
 *     commonStdIp - the common part for all stanard IP formats
 *     isArp       - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *     isBc        - Ethernet Broadcast packet.
 *                   Valid when <IsL2Valid> =1.
 *                   Indicates an Ethernet Broadcast packet
 *                    (<MAC_DA> == FF:FF:FF:FF:FF:FF).
 *                   0 = MAC_DA is not Broadcast.
 *                   1 = MAC_DA is Broadcast.
 *     sip         - IPv4 source IP address field.
 *                   For ARP packets this field holds the sender's IPv4 address.
 *     dip         - IPv4 destination IP address field.
 *                   For ARP packets this field holds the target IPv4 address.
 *     l4Byte0     - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte1     - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte13    - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     udb         - User Defined Bytes with indexes 0,1,2
 *
 *  Comments:
 *  udb[0] may contain the TCP/UDP comparator result
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC        common;
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC commonStdIp;
    unsigned char                                       isArp;
    unsigned char                                       isBc;
    GT_IPADDR                                   sip;
    GT_IPADDR                                   dip;
    unsigned char                                       l4Byte0;
    unsigned char                                       l4Byte1;
    unsigned char                                       l4Byte13;
    unsigned char                                       udb[3];

} CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV6_DIP_STC
 *
 * Description:
 *     This structure describes the standard IPV6 DIP key.
 *
 * Fields:
 *     common      - the common part for all formats (see upper)
 *     commonStdIp - the common part for all stanard IP formats
 *     isArp       - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *     isIpv6ExtHdrExist - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC
 *     isIpv6HopByHop    - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC
 *     dip         - 16 bytes IPV6 destination address.
 *
 *  Comments:
 *  Not supported by DxCh2
 *  Used for Unicast IPV6 routing, lookup1 only
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC        common;
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC commonStdIp;
    unsigned char                                       isArp;
    unsigned char                                       isIpv6ExtHdrExist;
    unsigned char                                       isIpv6HopByHop;
    GT_IPV6ADDR                                 dip;

} CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV6_DIP_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC
 *
 * Description:
 *     This structure describes the extended not-IPV6 key.
 *
 * Fields:
 *     common      - the common part for all formats (see upper)
 *     commonExt   - the common part for all extended formats
 *     sip         - IPv4 source IP address field.
 *                   For ARP packets this field holds the sender's IPv4 address.
 *     dip         - IPv4 header destination IP address field.
 *                   For ARP packets this field holds the target IPv4 address.
 *     etherType   - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *     l2Encap     - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *     macDa       - Ethernet Destination MAC address.
 *     macSa       - Ethernet Source MAC address.
 *     ipv4Fragmented - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC
 *     udb         - User Defined Bytes with indexes 0,1,2,3,4,5
 *
 *  Comments:
 *  udb[2] may contain the TCP/UDP comparator result
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC     common;
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC commonExt;
    GT_IPADDR                                sip;
    GT_IPADDR                                dip;
    unsigned short                                   etherType;
    unsigned char                                    l2Encap;
    GT_ETHERADDR                             macDa;
    GT_ETHERADDR                             macSa;
    unsigned char                                    ipv4Fragmented;
    unsigned char                                    udb[6];

} CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC
 *
 * Description:
 *     This structure describes the extended IPV6+L2  key.
 *
 * Fields:
 *     common      - the common part for all formats (see upper)
 *     commonExt   - the common part for all extended formats
 *     sip         - IPv6 source IP address field.
 *     dipBits127to120   - IPV6 destination address highest 8 bits.
 *     isIpv6ExtHdrExist - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC
 *     isIpv6HopByHop    - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC
 *     macDa       - Ethernet Destination MAC address.
 *     macSa       - Ethernet Source MAC address.
 *     udb         - User Defined Bytes with indexes 0,1,2,3,4,5
 *
 *  Comments:
 *  udb[2] may contain the TCP/UDP comparator result
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC     common;
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC commonExt;
    GT_IPV6ADDR                              sip;
    unsigned char                                    dipBits127to120;
    unsigned char                                    isIpv6ExtHdrExist;
    unsigned char                                    isIpv6HopByHop;
    GT_ETHERADDR                             macDa;
    GT_ETHERADDR                             macSa;
    unsigned char                                    udb[6];

} CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC
 *
 * Description:
 *     This structure describes the extended IPV6+L4  key.
 *
 * Fields:
 *     common      - the common part for all formats (see upper)
 *     commonExt   - the common part for all extended formats
 *     sip         - IPv6 source IP address field.
 *     dip         - IPv6 destination IP address field.
 *     isIpv6ExtHdrExist - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC
 *     isIpv6HopByHop    - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC
 *     udb         - User Defined Bytes with indexes 0,1,2
 *
 *  Comments:
 *  udb[2] may contain the TCP/UDP comparator result
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC     common;
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC commonExt;
    GT_IPV6ADDR                              sip;
    GT_IPV6ADDR                              dip;
    unsigned char                                    isIpv6ExtHdrExist;
    unsigned char                                    isIpv6HopByHop;
    unsigned char                                    udb[3];

} CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L4_STC;

/* EGRESS KEY FORMATS */

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STC
 *
 * Description:
 *     This structure describes the common segment of all egress key formats.
 *
 * Fields:
 *     pclId       - The PCL-ID attribute assigned to this lookup.
 *     sourcePort  - The port number from which the packet ingressed the device.
 *                   Port 63 is the CPU port.
 *     isTagged    - Flag indicating the packet Tag state
 *                   For Ingress PCL Keys:
 *                   For non-tunnel terminated packets:
 *                   - If packet was received on DSA-tagged, this field is set
 *                     to the DSA tag <SrcTagged> field.
 *                   - If packet was received non-DSA-tagged, this field is set
 *                     to 1 if the packet was received  VLAN or Priority-tagged.
 *                   For Ethernet-Over-xxx tunnel-terminated packets: Passenger
 *                   packet VLAN tag format.
 *
 *                   For Egress PCL Keys:
 *                   For non Tunnel Start packets: Transmitted packet's VLAN tag
 *                   format after tag addition/removal/modification
 *                   For Ethernet-Over-xxx Tunnel Start packets:
 *                   Passenger packet's VLAN tag format after tag
 *                   addition/removal/modification.
 *                   0 = Packet is untagged.
 *                   1 = Packet is tagged.
 *
 *     vid         - VLAN ID assigned to the packet.
 *     up          - The packet's 802.1p User Priority field.
 *     isIp        - An indication that the packet is IP
 *                   0 = Non IP packet.
 *                   1 = IPv4/6 packet.
 *     isL2Valid   - Indicates that Layer 2 information in the search key is valid.
 *                   0 = Layer 2 fields in the Ingress PCL (IPCL) or
 *                       Egress PCL (EPCL) are not valid
 *                   1 = Layer 2 field in the Ingress PCL or Egress PCL key is valid.
 *                   For Ingress PCL Keys: Layer 2 fields are not valid in the
 *                   IPCL keys when IP-Over-xxx is tunnel-terminated and its key
 *                   is based on the passenger IP packet which does not include
 *                   a Layer 2 header.
 *                   For Egress PCL Keys: Layer 2 fields are not valid in
 *                   the EPCL keys when an IP-Over-xxx is forwarded to a Tunnel
 *                   Start, and its PCL Key is based on the passenger IP packet
 *                   which does not include Layer 2 data.
 *     egrPacketType - two bits field:
 *                   0 - packet to CPU
 *                   1 - packet from CPU
 *                   2 - packet to ANALYZER
 *                   3 - forward DATA packet
 *     cpuCode     - The CPU Code forwarded to the CPU.
 *                   Relevant only when packet is of type is TO_CPU.
 *     srcTrg      - The <SrcTrg> field extracted from the TO_CPU DSA<SrcTrg>:
 *                   0 = The packet was sent to the CPU by the ingress pipe and
 *                   the DSA tag contain attributes related to the
 *                    packet ingress.
 *                   1 = The packet was sent to the CPU by the egress pipe
 *                   and the DSA tag contains attributes related to the
 *                   packet egress.
 *                   Relevant only when packet is of type is TO_CPU.
 *     tc -          The Traffic Class of the FROM_CPU packet extracted
 *                   from DSA<TC>.
 *     dp -          The Drop Precedence of the FROM_CPU packet extracted
 *                   from DSA<DP>.
 *     egrFilterEnable - The <EgressFilterEn> extracted from
 *                   FROM_CPU DSA<EgressFilterEn>:
 *                   0 = FROM_CPU packet is subject to egress filtering,
 *                   e.g. data traffic from the CPU
 *                   1 = FROM_CPU packet is not subject egress filtering,
 *                   e.g. control traffic from the CPU
 *     isUnknown -   Indicates that the packet’s MAC DA
 *                   was not found in the FDB.
 *                   0 = The packet’s MAC DA was found in the FDB.
 *                   1 = the packet’s MAC DA was not found in the FDB.
 *                   Relevant only when packet is of type is FORWARD.
 *     rxSniff -     Indicates if this packet is Ingress or Egress
 *                   mirrored to the Analyzer.
 *                   0 = This packet is Egress mirrored to the analyzer.
 *                   1 = This packet is Ingress mirrored to the analyzer.
 *                   Relevant only when packet is of type is TO_ANALYZER.
 *     qosProfile  - The QoS Profile assigned to the packet until this lookup
 *                   stage, as assigned by the device,
 *                   according to the QoS marking algorithm
 *                   Relevant only when packet is of type is FORWARD.
 *     srcTrunkId  - Id of source trunk (see srcIsTrunk)
 *                   Relevant only when packet is of type is FORWARD.
 *     srcIsTrunk  - Indicates packets received from a Trunk on a
 *                   remote device or on the local device
 *                   0 = Source is not a Trunk due to one of the following:
 *                   - A non-DSA-tagged packet received from a network port
 *                   which is not a trunk member
 *                   - A DSA-tagged packet with DSA<TagCmd> =FORWARD
 *                   and DSA<SrcIsTrunk> =0
 *                   1 = Source is a Trunk due to one of the following
 *                   - A non-DSA-tagged packet received from a network port
 *                    which is a trunk member
 *                   - A DSA-tagged packet with DSA<TagCmd> =FORWARD and
 *                    DSA<SrcIsTrunk =1
 *                   Relevant only when packet is of type is FORWARD.
 *     isRouted    - Indicates that the packet has been routed either by the
 *                   local device or some previous device in a cascaded system.
 *                   0 = The packet has not been routed.
 *                   1 = The packet has been routed.
 *                   Relevant only when packet is of type is FORWARD.
 *     srcDev      - Together with <SrcPort> and <SrcDev> indicates the
 *                   network port at which the packet was received.
 *                   Relevant for all packet types exclude the
 *                   FWD_DATA packed entered to the PP from Trunk
 *     sourceId    - The Source ID assigned to the packet.
 *     isVidx      - Indicates that a packet is forwarded to a Multicast group.
 *                   0 = The packet is a Unicast packet forwarded to a
 *                   specific target port or trunk.
 *                   1 = The packet is a multi-destination packet forwarded
 *                   to a Multicast group.
 *
 *  Comments:
 *  The cpssDxChPclRuleSet function includes the egrPktType union members
 *  to the rule only if the egrPacketType field in the mask is equal to 3
 *  (i.e. all two bits of egress packet type checked). At this condition the
 *  egrPktType union member that matches the egrPacketType
 *  field in the pattern included to the rule
 *  (0 - toCpu, 1 - fromCpu, 2 - toAnalyzer and 3 - fwdData).
 *  If the egrPacketType field in the mask is no equal to 3 all
 *  egrPktType union members are ignored.
 */
typedef struct
{
    unsigned short     pclId;
    unsigned char      sourcePort;
    unsigned char      isTagged;
    unsigned short     vid;
    unsigned char      up;
    unsigned char      isIp;
    unsigned char      isL2Valid;
    unsigned char      egrPacketType;
    union
    {
        struct
        {
            unsigned char cpuCode;
            unsigned char srcTrg;
        } toCpu;
        struct
        {
            unsigned char tc;
            unsigned char dp;
            unsigned char egrFilterEnable;
        } fromCpu;
        struct
        {
            unsigned char rxSniff;
        } toAnalyzer;
        struct
        {
            unsigned char qosProfile;
            unsigned char srcTrunkId;
            unsigned char srcIsTrunk;
            unsigned char isUnknown;
            unsigned char isRouted;
        } fwdData;
    } egrPktType;
    unsigned char      srcDev;
    unsigned char      sourceId;
    unsigned char      isVidx;
} CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_EXT_STC
 *
 * Description:
 *     This structure describes the common segment of all extended
 *     egress key formats.
 *
 * Fields:
 *     isIpv6     - An indication that the packet is IPv6.
 *                  0 = Non IPv6 packet.
 *                  1 = IPv6 packet.
 *     ipProtocol - IP protocol/Next Header type.
 *     dscp       - The DSCP field of the IPv4/6 header.
 *     isL4Valid  - Layer 4 information is valid.
 *                  This field indicates that all the Layer 4
 *                  information required for the search key is available
 *                  and the IP header is valid.
 *                  0 = Layer 4 information is not valid.
 *                  1 = Layer 4 information is valid.
 *                  Layer 4 information may not be available for any of the
 *                   following reasons:
 *                  - Layer 4 information is not included in the packet.
 *                     For example, Layer 4 information
 *                     isn't available in non-IP packets, or in IPv4
 *                     non-initial-fragments.
 *                  - Parsing failure: Layer 4 information is beyond
 *                     the first 128B of the packet, or beyond
 *                     IPv6 extension headers parsing capabilities.
 *                  - IP header is invalid.
 *     l4Byte0    - The following Layer 4 information is available for
 *                  UDP and TCP packets - L4 Header Byte0 through Byte3,
 *                  which contain the UDP/TCP destination and source ports.
 *                  For TCP also L4 Header Byte13, which contains the TCP flags.
 *                  For IGMP L4 Header Byte0, which contain the IGMP Type.
 *                  For ICMP L4 Header Byte0 and Byte1,
 *                  which contain the ICMP Type and Code fields
 *                  and L4 Header Byte4<in l4Byte2> and Byte5 <in l4Byte3>,
 *                  which contain the ICMP authentication field.
 *                  The following Layer 4 information is available for packets
 *                  that are not TCP, UDP, IGMP or ICMP:
 *                  L4 Header Byte0 through Byte3 L4 Header Byte13
 *     l4Byte1    - see l4Byte0
 *     l4Byte2    - see l4Byte0
 *     l4Byte3    - see l4Byte0
 *     l4Byte13   - see l4Byte0
 *
 *     egrTcpUdpPortComparator - For non Tunnel Start packets:
 *                  The transmitted packet TCP/UDP comparator result
 *                  For Tunnel Start packets:
 *                  The transmitted passenger packet TCP/UDP comparator result
 *
 *  Comments:
 */
typedef struct
{
    unsigned char      isIpv6;
    unsigned char      ipProtocol;
    unsigned char      dscp;
    unsigned char      isL4Valid;
    unsigned char      l4Byte0;
    unsigned char      l4Byte1;
    unsigned char      l4Byte2;
    unsigned char      l4Byte3;
    unsigned char      l4Byte13;
    unsigned char      egrTcpUdpPortComparator;

} CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_EXT_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STD_IP_STC
 *
 * Description:
 *     This structure describes the common segment
 *     of all egress standard IP key formats.
 *
 * Fields:
 *     isIpv4      - An indication that the packet is IPv4.
 *                  0 = Non IPv4 packet.
 *                  1 = IPv4 packet.
 *     ipProtocol  - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     dscp        - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     isL4Valid   - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte2     - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte3     - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     ipv4Fragmented - Identifies an IPv4 fragment.
 *                      0 = Not an IPv4 packet or not an IPv4 fragment.
 *                      1 = Packet is an IPv4 fragment (could be
 *                      the first fragment or any subsequent fragment)
 *
 *     egrTcpUdpPortComparator - For non Tunnel Start packets:
 *                  The transmitted packet TCP/UDP comparator result
 *                  For Tunnel Start packets:
 *                  The transmitted passenger packet TCP/UDP comparator result
 *
 *  Comments:
 */
typedef struct
{
    unsigned char      isIpv4;
    unsigned char      ipProtocol;
    unsigned char      dscp;
    unsigned char      isL4Valid;
    unsigned char      l4Byte2;
    unsigned char      l4Byte3;
    unsigned char      ipv4Fragmented;
    unsigned char      egrTcpUdpPortComparator;

} CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STD_IP_STC;


/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_NOT_IP_STC
 *
 * Description:
 *     This structure describes the standard egress not-IP key.
 * Fields:
 *     common     - the common part for all formats (see above)
 *     isIpv4      - An indication that the packet is IPv4.
 *                  0 = Non IPv4 packet.
 *                  1 = IPv4 packet.
 *     etherType  - Valid when <IsL2Valid> =1.
 *                  If <L2 Encap Type> = 0, this field contains the
 *                  <Dsap-Ssap> of the LLC NON-SNAP packet.
 *                  If <L2 Encap Type> = 1, this field contains the <EtherType/>
 *                  of the Ethernet V2 or LLC with SNAP packet.
 *     isArp      - An indication that the packet is an ARP packet
 *                   (identified by EtherType == 0x0806.)
 *                  0 = Non ARP packet.
 *                  1 = ARP packet.
 *     l2Encap    - The Layer 2 encapsulation of the packet.
 *                   0 = The L2 Encapsulation is LLC NON-SNAP.
 *                   1 = The L2 Encapsulation is Ethernet V2 or LLC with SNAP.
 *     macDa      - Ethernet Destination MAC address.
 *     macSa      - Ethernet Source MAC address.
 *
 *  Comments:
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STC common;
    unsigned char                                    isIpv4;
    unsigned short                                   etherType;
    unsigned char                                    isArp;
    unsigned char                                    l2Encap;
    GT_ETHERADDR                             macDa;
    GT_ETHERADDR                             macSa;

} CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_NOT_IP_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_IP_L2_QOS_STC
 *
 * Description:
 *     This structure describes the standard egress IPV4/V6 L2_QOS key.
 *
 * Fields:
 *     common            - the common part for all formats (see upper)
 *     commonStdIp       - the common part for all stanard IP formats
 *     isArp             - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *     dipBits0to31      - for IPV4 destination Ip address, for IPV6 the
 *                         31 LSBs of destination Ip address (network order)
 *     l4Byte13          - see in CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_EXT_STC
 *     macDa             - Ethernet Destination MAC address.
 *
 *  Comments:
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STC        common;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STD_IP_STC commonStdIp;
    unsigned char                                           isArp;
    unsigned char                                           dipBits0to31[4];
    unsigned char                                           l4Byte13;
    GT_ETHERADDR                                    macDa;

} CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_IP_L2_QOS_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_IPV4_L4_STC
 *
 * Description:
 *     This structure describes the standard egress IPV4_L4 key.
 *
 * Fields:
 *     common      - the common part for all formats (see upper)
 *     commonStdIp - the common part for all stanard IP formats
 *     isArp       - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *     isBc        - Ethernet Broadcast packet.
 *                   Valid when <IsL2Valid> =1.
 *                   Indicates an Ethernet Broadcast packet
 *                    (<MAC_DA> == FF:FF:FF:FF:FF:FF).
 *                   0 = MAC_DA is not Broadcast.
 *                   1 = MAC_DA is Broadcast.
 *     sip         - IPv4 source IP address field.
 *                   For ARP packets this field holds the sender's IPv4 address.
 *     dip         - IPv4 destination IP address field.
 *                   For ARP packets this field holds the target IPv4 address.
 *     l4Byte0     - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte1     - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte13    - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *
 *  Comments:
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STC        common;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STD_IP_STC commonStdIp;
    unsigned char                                           isArp;
    unsigned char                                           isBc;
    GT_IPADDR                                       sip;
    GT_IPADDR                                       dip;
    unsigned char                                           l4Byte0;
    unsigned char                                           l4Byte1;
    unsigned char                                           l4Byte13;

} CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_IPV4_L4_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_NOT_IPV6_STC
 *
 * Description:
 *     This structure describes the egress extended not-IPV6 key.
 *
 * Fields:
 *     common      - the common part for all formats (see upper)
 *     commonExt   - the common part for all extended formats
 *     sip         - IPv4 source IP address field.
 *                   For ARP packets this field holds the sender's IPv4 address.
 *     dip         - IPv4 header destination IP address field.
 *                   For ARP packets this field holds the target IPv4 address.
 *     etherType   - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *     l2Encap     - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *     macDa       - Ethernet Destination MAC address.
 *     macSa       - Ethernet Source MAC address.
 *     ipv4Fragmented - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC
 *
 *  Comments:
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STC     common;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_EXT_STC commonExt;
    GT_IPADDR                                    sip;
    GT_IPADDR                                    dip;
    unsigned short                                       etherType;
    unsigned char                                        l2Encap;
    GT_ETHERADDR                                 macDa;
    GT_ETHERADDR                                 macSa;
    unsigned char                                        ipv4Fragmented;

} CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_NOT_IPV6_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_IPV6_L2_STC
 *
 * Description:
 *     This structure describes the egress extended IPV6+L2  key.
 *
 * Fields:
 *     common      - the common part for all formats (see upper)
 *     commonExt   - the common part for all extended formats
 *     sip         - IPv6 source IP address field.
 *     dipBits127to120   - IPV6 destination address highest 8 bits.
 *     macDa       - Ethernet Destination MAC address.
 *     macSa       - Ethernet Source MAC address.
 *
 *  Comments:
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STC     common;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_EXT_STC commonExt;
    GT_IPV6ADDR                                  sip;
    unsigned char                                        dipBits127to120;
    GT_ETHERADDR                                 macDa;
    GT_ETHERADDR                                 macSa;

} CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_IPV6_L2_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_IPV6_L2_STC
 *
 * Description:
 *     This structure describes the egress extended IPV6+L4  key.
 *
 * Fields:
 *     common      - the common part for all formats (see upper)
 *     commonExt   - the common part for all extended formats
 *     sip         - IPv6 source IP address field.
 *     dip         - IPv6 destination IP address field.
 *
 *  Comments:
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STC     common;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_EXT_STC commonExt;
    GT_IPV6ADDR                              sip;
    GT_IPV6ADDR                              dip;

} CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_IPV6_L4_STC;

/*
 * Typedef: union CPSS_DXCH_PCL_RULE_FORMAT_UNT
 *
 * Description:
 *     This union describes the PCL key.
 *
 * Fields:
 *     Ingress Policy Key format structures
 *     ruleStdNotIp       - Standard Not IP packet key
 *     ruleStdIpL2Qos     - Standard IPV4 and IPV6 packets
 *                          L2 and QOS styled key
 *     ruleStdIpv4L4      - Standard IPV4 packet L4 styled key
 *     ruleStdIpv6Dip     - Standard IPV6 packet DIP styled key (DxCh only)
 *     ruleExtNotIpv6     - Extended Not IP and IPV4 packet key
 *     ruleExtIpv6L2      - Extended IPV6 packet L2 styled key
 *     ruleExtIpv6L4      - Extended IPV6 packet L2 styled key
 *     ruleEgrStdNotIp    - Egress Standard Not IP packet key
 *     ruleEgrStdIpL2Qos  - Egress Standard IPV4 and IPV6 packets
 *                          L2 and QOS styled key
 *     ruleEgrStdIpv4L4   - Egress Standard IPV4 packet L4 styled key
 *     ruleEgrExtNotIpv6  - Egress SExtended Not IP and IPV4 packet key
 *     ruleEgrExtIpv6L2   - Egress SExtended IPV6 packet L2 styled key
 *     ruleEgrExtIpv6L4   - Egress SExtended IPV6 packet L2 styled key
 *
 *  Comments:
 */
typedef union
{
    CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC          ruleStdNotIp;
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC       ruleStdIpL2Qos;
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC         ruleStdIpv4L4;
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV6_DIP_STC        ruleStdIpv6Dip;
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC        ruleExtNotIpv6;
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC         ruleExtIpv6L2;
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L4_STC         ruleExtIpv6L4;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_NOT_IP_STC      ruleEgrStdNotIp;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_IP_L2_QOS_STC   ruleEgrStdIpL2Qos;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_IPV4_L4_STC     ruleEgrStdIpv4L4;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_NOT_IPV6_STC    ruleEgrExtNotIpv6;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_IPV6_L2_STC     ruleEgrExtIpv6L2;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_IPV6_L4_STC     ruleEgrExtIpv6L4;
} CPSS_DXCH_PCL_RULE_FORMAT_UNT;

/*
 * typedef: enum CPSS_DXCH_PCL_EGRESS_PKT_TYPE_ENT
 *
 * Description: packet types that can be enabled/disabled for Egress PCL.
 *              By default on all ports and on all packet types are disabled
 *
 * Enumerators:
 *   CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E - Enable/Disable Egress Policy
 *                   for Control packets FROM CPU.
 *   CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_DATA_E    - Enable/Disable Egress Policy
 *                   for data packets FROM CPU.
 *   CPSS_DXCH_PCL_EGRESS_PKT_TO_CPU_E           - Enable/Disable Egress Policy
 *                   on TO-CPU packets
 *   CPSS_DXCH_PCL_EGRESS_PKT_TO_ANALYZER_E      - Enable/Disable Egress Policy
 *                   for TO ANALYZER packets.
 *   CPSS_DXCH_PCL_EGRESS_PKT_TS_E               - Enable/Disable Egress Policy
 *                   for data packets that are tunneled in this device.
 *   CPSS_DXCH_PCL_EGRESS_PKT_NON_TS_E           - Enable/Disable Egress Policy
 *                   for data packets that are not tunneled in this device.
 *
 * Comment:
 *    relevant only for DxCh2 devices
 */
typedef enum
{
    CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E,
    CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_DATA_E,
    CPSS_DXCH_PCL_EGRESS_PKT_TO_CPU_E,
    CPSS_DXCH_PCL_EGRESS_PKT_TO_ANALYZER_E,
    CPSS_DXCH_PCL_EGRESS_PKT_TS_E,
    CPSS_DXCH_PCL_EGRESS_PKT_NON_TS_E
} CPSS_DXCH_PCL_EGRESS_PKT_TYPE_ENT;

/*
 * typedef: enum CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT
 *
 * Description: Type of access to Ingress / Egress PCL configuration Table.
 *              Type of access defines how device calculates index of PCL
 *              configuration Table for packet.
 *
 * Enumerators:
 *   CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E - by PORT access mode.
 *                                             ingress port or trunk id is used
 *                                             to calculate index of the PCL
 *                                             configuration table
 *   CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E - by VLAN access mode
 *                                             VLAN ID is used
 *                                             to calculate index of the PCL
 *                                             configuration table
 * Comment:
 *
 */
typedef enum
{
    CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E,
    CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E
} CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT;

/*
 * typedef: enum CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_ENT
 *
 * Description: Global Egress PCL or Ingress Policy
 *              Configuration Table Access mode
 *
 * Enumerators:
 *   CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E - access by local port number
 *   CPSS_DXCH_PCL_CFG_TBL_ACCESS_NON_LOCAL_PORT_E - access by remote device Id
 *             and Port number or by trunk Id,
 *             Used for Value Blade systems based on DX switch device.
 *             The remote port and device are taken from DSA tag and
 *             represent real source port and device for a packets.
 *
 * Comment:
 *
 */
typedef enum
{
    CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E,
    CPSS_DXCH_PCL_CFG_TBL_ACCESS_NON_LOCAL_PORT_E
} CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_ENT;


/*
 * typedef: enum CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_ENT
 *
 * Description: size of Ingress Policy / Egress PCL Configuration
 *              table segment accessed by "non-local device and port"
 *
 * Enumerators:
 *   CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E - support 32 port remote devices
 *   CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E - support 64 port remote devices
 *
 * Comment:
 *    relevant only for DxCh2 devices
*/
typedef enum
{
    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E,
    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E
} CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_ENT;

/*
 * typedef: enum CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE_ENT
 *
 * Description: base of Ingress Policy / Egress PCL Configuration
 *              table segment accessed
 *              by "non-local device and port"
 *
 * Enumerators:
 *   CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E    - the base is 0
 *   CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE1024_E - the base is 1024 (only 32-port)
 *   CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E - the base is 2048
 *   CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE3072_E - the base is 3072 (only 32-port)
 *
 * Comment:
 *    relevant only for DxCh2 devices
 *    Both egress and ingress PCL Configuration tables contain 4096
 *    entries block "entry per VLAN".
 *    In Non-local mode the packets that mapped by device-and-port served
 *    by entry from MAX-DEVICES(i.e.32)* MAX-PORTS(i.e. 32 * 64) entries block
 *    that overlaps the "entry per VLAN" block, but least than it
 *    (the size is 1024 or 2048 entries). The values upper dedicated to
 *    configure it's position.
 *
 */
typedef enum
{
    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E,
    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE1024_E,
    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E,
    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE3072_E
} CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE_ENT;

/*
 * typedef: struct CPSS_DXCH_PCL_LOOKUP_CFG_STC
 *
 * Description: PCL Configuration Table Entry Per Lookup Attributes.
 *
 * Fields:
 *    enableLookup           - Enable Lookup
 *                             GT_TRUE - enable Lookup
 *                             GT_FALSE - disable Lookup
 *    pclId                  - PCL-ID bits in the TCAM search key.
 *    groupKeyTypes.nonIpKey - type of TCAM search key for NON-IP packets.
 *    groupKeyTypes.ipv4Key  - type of TCAM search key for IPV4 packets
 *    groupKeyTypes.ipv6Key  - type of TCAM search key for IPV6 packets
 *
 * Comments:
 *    1. For DxCh2 and above devices the entry contains
 *    - "lookup enable" bit
 *    - 10 "PCL-ID" bits
 *    - 1 bit "not IP key" type standard or extended
 *    - 2 bits "IPV4 key" type std-L2, std-L4, ext
 *    - 2 bits "IPV6 key" type std-L2, ext-L2, ext-L4
 *    2. For DxCh1 devices the entry contains
 *    - "lookup enable" bit
 *    - 10 "PCL-ID" bits
 *    - 1 bit - key standard or extended (for all types)
 *    - 1 bit - key L2 or L4 styled for standard IPv4 or extended IPv6 packets
 *    - 1 bit - (lookup1 only) IPV6 std-DIP key
 *    2.1 Acceptable key types for DxCh1 devices.
 *    2.1.1  Standard Keys.
 *          nonIpKey - CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E
 *          ipv4Key  - CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E
 *                       L2 style of key for IPv4 packets
 *                     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E
 *                       L4 style of key for IPv4 packets
 *          ipv6Key  - CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E
 *                       L2 style of key for IPv6 packets
 *                     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E
 *                      IPv6 DIP key for lookup#1 only.
 *    2.1.2 Extended Keys.
 *          nonIpKey - CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E
 *          ipv4Key  - CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E
 *          ipv6Key  - CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E
 *                       L2 style of key for IPv6 packets
 *                     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E
 *                       L4 style of key for IPv6 packets
 *    3. The SW structure can represent any possible HW configuration
 *    If an impossible configuraration passed an appropriate APIs
 *    GT_BAD_PARAM is returned.
 */
typedef struct
{
    char                                   enableLookup;
    unsigned long                                    pclId;
    struct
    {
        CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT    nonIpKey;
        CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT    ipv4Key;
        CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT    ipv6Key;
    }                                         groupKeyTypes;

} CPSS_DXCH_PCL_LOOKUP_CFG_STC;

/*
 * typedef: struct CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC
 *
 * Description: global access mode to Ingress or Egress
 *              PCL configuration tables.
 *
 * Fields:
 *    ipclAccMode     - Ingress Policy local/non-local mode
 *    ipclMaxDevPorts - Ingress Policy support 32/64 port remote devices
 *    ipclDevPortBase - Ingress Policy Configuration table access base
 *    epclAccMode     - Egress PCL local/non-local mode
 *    epclMaxDevPorts - Egress PCL support 32/64 port remote devices
 *    epclDevPortBase - Egress PCL Configuration table access base
 * Comment:
 *   - for DxCh2 all fields relevant
 *   - for DxCh  relevant only ipclAccMode member
 *
 */
typedef struct
{
    CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_ENT   ipclAccMode;
    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_ENT ipclMaxDevPorts;
    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE_ENT ipclDevPortBase;
    CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_ENT   epclAccMode;
    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_ENT epclMaxDevPorts;
    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE_ENT epclDevPortBase;

} CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC;

typedef enum {
    PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E = 0,
    PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
    PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E,
    PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E, /*3*/
    PRV_CPSS_DXCH_TABLE_CPU_CODE_E,
    PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E,
    PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E,/*6*/
    PRV_CPSS_DXCH_TABLE_REMARKING_E,
    PRV_CPSS_DXCH_TABLE_STG_E,
    PRV_CPSS_DXCH_TABLE_VLAN_E,/*9*/
    PRV_CPSS_DXCH_TABLE_MULTICAST_E,
    PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E,
    PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E,
    PRV_CPSS_DXCH_TABLE_FDB_E,
    PRV_CPSS_DXCH_TABLE_POLICER_E,
    PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E,
    PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E,
    PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E,
    PRV_CPSS_DXCH2_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E,
    PRV_CPSS_DXCH2_TABLE_ROUTE_ACCESS_MATRIX_E,
    PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
    PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E,
    PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_E,

    /* must be last */
    PRV_CPSS_DXCH_TABLE_LAST_E
}PRV_CPSS_DXCH_TABLE_ENT;
#endif
#endif /* __npdcpssacldefsh */
