/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfPclUdb.h
*
* DESCRIPTION:
*       User Defined Bytes tests
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfPclUdbh
#define __prvTgfPclUdbh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* prvTgfPclIpv4TcpConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_IPV4_TCP_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_L4_E
*         - set UDB offset to 0
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv4TcpConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv4TcpTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 TCP packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on port 0
*
*           Send to device's port 0 TCP packet:
*               macDa = 00:00:00:00:34:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv4TcpTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv4TcpAdditionalConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_IPV4_TCP_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_INVALID_E
*         - set UDB offset to 0
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv4TcpAdditionalConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv4TcpAdditionalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 TCP packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv4TcpAdditionalTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv4TcpConfigRestore
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
GT_VOID prvTgfPclIpv4TcpConfigRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv4UdpConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_IPV4_UDP_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_L4_E
*         - set UDB offset to 0
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv4UdpConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv4UdpTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 UDP packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on port 0
*
*           Send to device's port 0 UDP packet:
*               macDa = 00:00:00:00:34:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv4UdpTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv4UdpAdditionalConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_IPV4_UDP_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_INVALID_E
*         - set UDB offset to 0
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv4UdpAdditionalConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv4UdpAdditionalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 UDP packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv4UdpAdditionalTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv4UdpConfigRestore
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
GT_VOID prvTgfPclIpv4UdpConfigRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv4FragmentConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_IPV4_FRAGMENT_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_L3_MINUS_2_E
*         - set UDB offset to 0
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv4FragmentConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv4FragmentTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 IPv4 packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on port 0
*
*           Send to device's port 0 IPv4 packet:
*               macDa = 00:00:00:00:34:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv4FragmentTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv4FragmentAdditionalConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E
*         - set key type to PRV_TGF_PCL_PACKET_TYPE_IPV4_FRAGMENT_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_INVALID_E
*         - set UDB offset to 0
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv4FragmentAdditionalConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv4FragmentAdditionalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 IPv4 packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv4FragmentAdditionalTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv4FragmentConfigRestore
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
GT_VOID prvTgfPclIpv4FragmentConfigRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv4OtherConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_IPV4_OTHER_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_L3_MINUS_2_E
*         - set UDB offset to 0
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv4OtherConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv4OtherTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 IPv4 packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on port 0
*
*           Send to device's port 0 IPv4 packet:
*               macDa = 00:00:00:00:34:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv4OtherTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv4OtherAdditionalConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E
*         - set key type to PRV_TGF_PCL_PACKET_TYPE_IPV4_OTHER_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_INVALID_E
*         - set UDB offset to 0
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv4OtherAdditionalConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv4OtherAdditionalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 IPv4 packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv4OtherAdditionalTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv4OtherConfigRestore
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
GT_VOID prvTgfPclIpv4OtherConfigRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclEthernetOtherConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_ETHERNET_OTHER_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_L2_E
*         - set UDB offset to 0
*         - set rule index to 0
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
GT_VOID prvTgfPclEthernetOtherConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclEthernetOtherTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on port 0
*
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:00:00:00:34:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclEthernetOtherTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclEthernetOtherAdditionalConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E
*         - set key type to PRV_TGF_PCL_PACKET_TYPE_ETHERNET_OTHER_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_INVALID_E
*         - set UDB offset to 0
*         - set rule index to 0
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
GT_VOID prvTgfPclEthernetOtherAdditionalConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclEthernetOtherAdditionalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclEthernetOtherAdditionalTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclEthernetOtherConfigRestore
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
GT_VOID prvTgfPclEthernetOtherConfigRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclMplsConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_MPLS_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_MPLS_MINUS_2_E
*         - set UDB offset to 0
*         - set rule index to 0
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
GT_VOID prvTgfPclMplsConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclMplsTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on port 0
*
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:00:00:00:34:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclMplsTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclMplsAdditionalConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E
*         - set key type to PRV_TGF_PCL_PACKET_TYPE_MPLS_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_INVALID_E
*         - set UDB offset to 0
*         - set rule index to 0
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
GT_VOID prvTgfPclMplsAdditionalConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclMplsAdditionalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclMplsAdditionalTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclMplsConfigRestore
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
GT_VOID prvTgfPclMplsConfigRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclUdeConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_UDE_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_L2_E
*         - set UDB offset to 0
*         - set rule index to 0
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
GT_VOID prvTgfPclUdeConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclUdeTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on port 0
*
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:00:00:00:34:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclUdeTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclUdeAdditionalConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E
*         - set key type to PRV_TGF_PCL_PACKET_TYPE_UDE_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_INVALID_E
*         - set UDB offset to 0
*         - set rule index to 0
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
GT_VOID prvTgfPclUdeAdditionalConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclUdeAdditionalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclUdeAdditionalTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclUdeConfigRestore
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
GT_VOID prvTgfPclUdeConfigRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv6ConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_IPV6_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_L3_E
*         - set UDB offset to 0
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv6ConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv6TrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on port 0
*
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:00:00:00:34:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv6TrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv6AdditionalConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_IPV6_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_INVALID_E
*         - set UDB offset to 0
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv6AdditionalConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv6AdditionalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv6AdditionalTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv6ConfigRestore
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
GT_VOID prvTgfPclIpv6ConfigRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv6TcpConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_IPV6_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_L4_E
*         - set UDB offset to 0
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv6TcpConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv6TcpTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on port 0
*
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:00:00:00:34:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv6TcpTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv6TcpAdditionalConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_IPV6_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_INVALID_E
*         - set UDB offset to 0
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv6TcpAdditionalConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv6TcpAdditionalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv6TcpAdditionalTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv6TcpConfigRestore
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
GT_VOID prvTgfPclIpv6TcpConfigRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclEthernetOtherVridConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_ETHERNET_OTHER_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_L2_E
*         - set useVrId to GT_TRUE
*         - set UDB offset to 0
*         - set rule index to 0
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
GT_VOID prvTgfPclEthernetOtherVridConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclEthernetOtherVridTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on port 0
*
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:00:00:00:34:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclEthernetOtherVridTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclEthernetOtherVridAdditionalConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E
*         - set key type to PRV_TGF_PCL_PACKET_TYPE_ETHERNET_OTHER_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_INVALID_E
*         - set useVrId to GT_TRUE
*         - set UDB offset to 0
*         - set rule index to 0
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
GT_VOID prvTgfPclEthernetOtherVridAdditionalConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclEthernetOtherVridAdditionalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 Ethernet packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclEthernetOtherVridAdditionalTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclEthernetOtherVridConfigRestore
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
GT_VOID prvTgfPclEthernetOtherVridConfigRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv4UdpQosConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_IPV4_UDP_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_L3_MINUS_2_E
*         - set useQosProfile to GT_TRUE
*         - set UDB offset to 0
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv4UdpQosConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv4UdpQosTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 UDP packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on port 0
*
*           Send to device's port 0 UDP packet:
*               macDa = 00:00:00:00:34:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv4UdpQosTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv4UdpQosAdditionalConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set packet type to PRV_TGF_PCL_PACKET_TYPE_IPV4_UDP_E
*         - set key type to PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E
*         - set UDB offset type to PRV_TGF_PCL_OFFSET_INVALID_E
*         - set useQosProfile to GT_TRUE
*         - set UDB offset to 0
*         - set rule index to 0
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
GT_VOID prvTgfPclIpv4UdpQosAdditionalConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv4UdpQosAdditionalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 UDP packet:
*               macDa = 00:11:22:33:44:55,
*               macSa = 00:66:77:88:99:11,
*           Success Criteria:
*               Packet is captured on ports 0, 8, 18, 23
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
GT_VOID prvTgfPclIpv4UdpQosAdditionalTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIpv4UdpQosConfigRestore
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
GT_VOID prvTgfPclIpv4UdpQosConfigRestore
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfPclUdbh */

