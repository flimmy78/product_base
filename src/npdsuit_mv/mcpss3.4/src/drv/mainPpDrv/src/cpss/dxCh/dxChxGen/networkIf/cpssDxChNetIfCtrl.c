/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChNetIf.c
*
* DESCRIPTION:
*       Include DxCh network interface API functions
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.3 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h>
#include <cpss/dxCh/dxChxGen/trunk/private/prvCpssDxChTrunk.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChPacketHandling.h>

/* constant to show "vlan instead vidx"*/
#define VID_INSTEAD_VIDX_CNS            0xfff
#define VID_INSTEAD_VIDX_9_BITS_CNS     0x1ff

/* macro convert the RX cpu code to DSA code
   NOTE : macro return on error !!!
*/
#define RX_OPCODE_TO_DSA_CONVERT_MAC(rxCode,dsaCodePtr)                    \
    {                                                                      \
        GT_STATUS rcTemp = prvCpssDxChNetIfCpuToDsaCode(rxCode,dsaCodePtr);\
        if(rcTemp != GT_OK)                                                \
        {                                                                  \
            return rcTemp;                                                 \
        }                                                                  \
    }

/* macro convert the DSA code RX cpu code
   NOTE : macro return on error !!!
*/
#define DSA_TO_RX_OPCODE_CONVERT_MAC(dsaCode,rxCodePtr)                    \
    {                                                                      \
        GT_STATUS rcTemp = prvCpssDxChNetIfDsaToCpuCode(dsaCode,rxCodePtr);\
        if(rcTemp != GT_OK)                                                \
        {                                                                  \
            return rcTemp;                                                 \
        }                                                                  \
    }

/* the max valid index in the table of : cpu code ip protocol */
#define DXCH2_CPU_CODE_IP_PROTOCOL_MAX_INDEX_CNS        7

/* the max valid index in the TCP/UPD Destination Port Range CPU Code Table */
#define DXCH2_CPU_CODE_L4_DEST_PORT_RANGE_MAX_INDEX_CNS        15

/* rate limiter index to be in the "cpu code table" */
#define CPU_CODE_RATE_LIMITER_INDEX_CHECK_MAC(index)    \
    if(index > 31)                                  \
        return GT_BAD_PARAM

/* statistical rate limiter index to be used when access the "statistical rate limiter table" */
#define CPU_CODE_STATISTICAL_RATE_LIMITER_INDEX_CHECK_MAC(index)   \
    if(index > 31 )                                   \
        return GT_BAD_PARAM

/* designated device index to be used when access the "designated device table" */
#define CPU_CODE_DESIGNATED_DEVICE_INDEX_CHECK_MAC(index)          \
    if(index > 7 )                                    \
        return GT_BAD_PARAM

/* rate limiter index to be used when access the "rate limiter table" */
#define CPU_CODE_RATE_LIMITER_INDEX_AS_POINTER_CHECK_MAC(index)    \
    if(index > 31 || index == 0)                                   \
        return GT_BAD_PARAM

/* statistical rate limiter index to be used when access the "statistical rate limiter table" */
#define CPU_CODE_STATISTICAL_RATE_LIMITER_INDEX_AS_POINTER_CHECK_MAC(index)   \
    if(index > 31 /*index 0 is OK*/)                                   \
        return GT_BAD_PARAM

/* designated device index to be used when access the "designated device table" */
#define CPU_CODE_DESIGNATED_DEVICE_INDEX_AS_POINTER_CHECK_MAC(index)          \
    if(index > 7 || index == 0)                                    \
        return GT_BAD_PARAM


#ifndef __AX_PLATFORM__
    /* rate limiter default resolution */
    /* default value means 200us = <0x4E2 * 32(system clock) * 5 ns > */
#define CPSS_PRE_EGRESS_RATE_LIMIT_RESOLUTION_DEFAULT	(0x4E2 * 32 * 5) 
    
    /* CPU port packet rate limiter index */
    /* packet send via sdma queue 7 */
#define CPU_PORT_BPDU_RATE		1
#define CPU_PORT_VRRP_RATE		2
    
    /* packet send via sdma queue 6 */
#define CPU_PORT_TELNET_RATE		3
#define CPU_PORT_SSH_RATE			4
#define CPU_PORT_HTTP_RATE		5
#define CPU_PORT_HTTPS_RATE		CPU_PORT_HTTP_RATE
#define CPU_PORT_SNMP_RATE		6
    
    /* packet send via sdma queue 5 */
#define CPU_PORT_RIPv1_RATE		7
#define CPU_PORT_RIPv2_RATE		CPU_PORT_RIPv1_RATE
#define CPU_PORT_OSPFv1_RATE		8
#define CPU_PORT_OSPFv2_RATE		CPU_PORT_OSPFv1_RATE
#define CPU_PORT_OSPFDSG_RATE		CPU_PORT_OSPFv1_RATE
    
    /* packet send via sdma queue 4 */
#define CPU_PORT_PIM_RATE			9
    
    /* packet send via sdma queue 3 */
#define CPU_PORT_CAPWAP_RATE		10
#define CPU_PORT_8021x_RATE		11
    
    /* packet send via sdma queue 2 */
#define CPU_PORT_DHCPREQ_RATE		12
#define CPU_PORT_DHCPRPL_RATE		CPU_PORT_DHCPREQ_RATE
#define CPU_PORT_IGMP_RATE		13
#define CPU_PORT_ALL_MC_RATE		CPU_PORT_IGMP_RATE
#define CPU_PORT_ALL_ROUTERS_RATE 	CPU_PORT_IGMP_RATE
    
    /* packet send via sdma queue 1 */
#define CPU_PORT_ARP_BC_RATE		14
#define CPU_PORT_ARP_UC_RATE		15
    
    /* packet send via sdma queue 0 */
#define CPU_PORT_BRIDGE_RATE		16
#define CPU_PORT_ROUTE_RATE		17
#define CPU_PORT_ICMP_RATE		CPU_PORT_ROUTE_RATE
#define CPU_PORT_TCPSYN_RATE		18
    
#define CPU_PORT_DEFAULT_RATE		0
    
    /* CPU code for OAM traffic */
#define CPSS_NET_OAM_TCP_TELNET_E	(CPSS_NET_FIRST_USER_DEFINED_E + 1)
#define CPSS_NET_OAM_TCP_SSH_E		(CPSS_NET_FIRST_USER_DEFINED_E + 2)
#define CPSS_NET_OAM_TCP_HTTP_E		(CPSS_NET_FIRST_USER_DEFINED_E + 3)
#define CPSS_NET_OAM_TCP_HTTPS_E		(CPSS_NET_FIRST_USER_DEFINED_E + 4)
#define CPSS_NET_OAM_UDP_SNMP_E		(CPSS_NET_FIRST_USER_DEFINED_E + 5)
    
    /* CPU code for wireless capwap control/data packet */
#define CPSS_NET_WIRELESS_UDP_CNTL_E		(CPSS_NET_FIRST_USER_DEFINED_E + 6)
#define CPSS_NET_WIRELESS_UDP_DATA_E		(CPSS_NET_FIRST_USER_DEFINED_E + 7)
    
    /*
     * packet rate caculation as follows:
     *      *********************************
     *      *   pktLimit/(windowSize * resolution)  *
     *      *********************************
     * where resolution is configured via cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet
     * as default resolution has value 0x4E2 which means (0x4E2 * 5ns *32(system clock) = 200us), while 5ns is from system clock
     */
    struct NETIF_PORT_TRFFIC_LIMIT_INFO {
        unsigned int windowSize;
        unsigned int pktLimit;
    } cpu_port_rate_limiter[] = {
        /* 31 valid rate limiter supported */
        /* rate limiter index 0 means no limit *//* no HW entry for index 0 */
        {0,0},/* Limiter 0 *//* not exist in HW */
        
        /*  BPDU/CAPWAP/DHCP/DOT1x/IGMP/OSPFv1(v2)/RIPv1(v2) */
        {50,5},/* Limiter 1 *//* every 5pkts/10ms =  500 pps */
        
        /* VRRP */
        {50,10},/* Limiter 2 *//* every 10pkts/10ms =  1000 pps */
        
        /* TELNET */
        {50,5},/* Limiter 3 *//* every 5pkts/10ms =  500 pps */
        
        /* SSH */
        {50,5},/* Limiter 4 *//* every 5pkts/10ms = 500 pps */
    
        /* HTTP/HTTPS */
        {50,5},/* Limiter 5 *//* every 5pkts/10ms =  500 pps */
    
        /* SNMP */
        {50,5},/* Limiter 6 *//* every 5pkts/10ms =  500 pps */
    
        /* RIPv1 or v2 */
        {50,5},/* Limiter 7 *//* every 5pkts/10ms =  500 pps */
    
        /* OSPFv1 or v2 */
        {50,5},/* Limiter 8 *//* every 5pkts/10ms =  500 pps */
    
        /* PIM */
        {50,5},/* Limiter 9 *//* every 5pkts/10ms =  500 pps */
    
        /* CAPWAP */
        {50,5},/* Limiter 10 *//* every 5pkts/10ms =  500 pps */
    
        /* 802.1x */
        {50,5},/* Limiter 11 *//* every 5pkts/10ms =  500 pps */
    
        /* DHCP request/reply */
        {50,5},/* Limiter 12 *//* every 5pkts/10ms =  500 pps */
    
        /* IGMP/all mc/all router */
        {50,5},/* Limiter 13 *//* every 5pkts/10ms =  500 pps */
        
        /* ARP request */
        {50,5},/* Limiter 14 *//* every 5pkts/10ms =  500 pps */
    
        /* ARP reply */
        {50,5},/* Limiter 15 *//* every 5pkts/10ms =  500 pps */
    
        /* Bridge forward */
        {50,5},/* Limiter 16 *//* every 5pkts/10ms =  500 pps */
    
        /* Route forward *//* packet count >= 45 to admit jumbo packet size 65500(45 fragments) */
        {500,50},/* Limiter 17 *//* every 50pkts/100ms =  500 pps */
    
        /* TCP syn */
        {50,2},/* Limiter 18 *//* every 2pkts/10ms =  200 pps */
        
        {0,0},/* Limiter 19 */
        {0,0},/* Limiter 20 */
        {0,0},/* Limiter 22 */
        {0,0},/* Limiter 23 */
        {0,0},/* Limiter 24 */
        {0,0},/* Limiter 25 */
        {0,0},/* Limiter 26 */
        {0,0},/* Limiter 27 */
        {0,0},/* Limiter 28 */
        {0,0},/* Limiter 29 */
        {0,0},/* Limiter 30 */
        {0,0} /* Limiter 31 */
    };
    
    struct NETIF_PORT_TRAFFIC_CLASSIFIER {
        CPSS_NET_RX_CPU_CODE_ENT cpuCode;
        GT_U8 tc;
        CPSS_DP_LEVEL_ENT dp;
        GT_BOOL   truncate;
        CPSS_NET_CPU_CODE_RATE_LIMIT_MODE_ENT cpuRateLimitMode;
        GT_U32  cpuCodeRateLimiterIndex;
        GT_U32  cpuCodeStateRateLimitIndex;
        GT_U32  destDevNumIndex;
    } cpu_port_traffic_classifier[] = {
        /* BPDU packet (format 01-80-C2-00-00-00) */
        {
            CPSS_NET_CONTROL_BPDU_E,/* CPU CODE:2 */ CPU_PORT_BPDU_QUEUE, CPSS_DP_GREEN_E, GT_FALSE, 
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_BPDU_RATE, 0, 0
        },
    
        /* ARP packet BC request (format mac FF-FF-FF-FF-FF-FF ether-type 0x0806) */
        {
            CPSS_NET_INTERVENTION_ARP_E,/* CPU CODE:5 */ CPU_PORT_ARP_BC_QUEUE, CPSS_DP_GREEN_E, GT_FALSE, 
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_ARP_BC_RATE, 0, 0
        },
    
        /* ARP packet UC reply to me( format mac SYSTEM MAC ether-type 0x0806) */
        {
            CPSS_NET_ARP_REPLY_TO_ME_E,/* CPU CODE:188 */ CPU_PORT_ARP_UC_QUEUE, CPSS_DP_GREEN_E, GT_FALSE,
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_ARP_UC_RATE, 0, 0
        },
        
        /* RIPv1 packet (format mac FF-FF-FF-FF-FF-FF udp dst-port 520 ) */
        {
            CPSS_NET_MIRROR_IPV4_RIPV1_TO_CPU_E,/* CPU CODE:17 */ CPU_PORT_RIPv1_QUEUE, CPSS_DP_GREEN_E, GT_FALSE, 
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_RIPv1_RATE, 0, 0
        },
    
        /* RIPv2 packet (format mac 01-00-5E-XX-XX-XX dip 224.0.0.9) */
        {
            CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E,/* CPU CODE:29 */ CPU_PORT_RIPv2_QUEUE, CPSS_DP_GREEN_E, GT_FALSE, 
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_RIPv2_RATE, 0, 0
        },
        
        /* OSPF packet (format mac 01-00-5E-XX-XX-XX dip 224.0.0.5(6) ) */
        {
            CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E,/* CPU CODE:29 */ CPU_PORT_OSPFv1_QUEUE, CPSS_DP_GREEN_E, GT_FALSE, 
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_OSPFv1_RATE, 0, 0
        },
    
        /* CAPWAP discovery packet (format mac FF-FF-FF-FF-FF-FF udp dst-port 1234 ) */
        {
            CPSS_NET_UDP_BC_MIRROR_TRAP1_E,/* CPU CODE:33 */ CPU_PORT_CAPWAP_QUEUE, CPSS_DP_GREEN_E, GT_FALSE, 
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_CAPWAP_RATE, 0, 0
        },
    
        /* CAPWAP control packet (format mac SYSTEM MAC udp dst-port 1234 ) */
        {
            CPSS_NET_WIRELESS_UDP_CNTL_E,/* CPU CODE:198 */ CPU_PORT_CAPWAP_QUEUE, CPSS_DP_GREEN_E, GT_FALSE, 
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_CAPWAP_RATE, 0, 0
        },
        
        /* CAPWAP data packet (format mac SYSTEM MAC udp dst-port 8888 ) */
        {
            CPSS_NET_WIRELESS_UDP_DATA_E,/* CPU CODE:199 */ CPU_PORT_CAPWAP_QUEUE, CPSS_DP_GREEN_E, GT_FALSE, 
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_CAPWAP_RATE, 0, 0
        },
        
        /* DHCP request packet (format mac FF-FF-FF-FF-FF-FF udp dst-port 67 ) */   
        {
            CPSS_NET_IPV4_BROADCAST_PACKET_E,/* CPU CODE:19 */ CPU_PORT_DHCPREQ_QUEUE, CPSS_DP_GREEN_E, GT_FALSE, 
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_DHCPREQ_RATE, 0, 0
        },
    
        /* DHCP reply(broadcast) packet (format mac FF-FF-FF-FF-FF-FF udp dst-port 68 ) */
        {
            CPSS_NET_UDP_BC_MIRROR_TRAP0_E,/* CPU CODE:32 */ CPU_PORT_DHCPRPL_QUEUE, CPSS_DP_GREEN_E, GT_FALSE, 
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_DHCPRPL_RATE, 0, 0
        },
    
        /* 802.1x packet (format mac 01-80-C2-00-00-03) */
        {
            CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E,/* CPU CODE:13 */ CPU_PORT_8021x_QUEUE, CPSS_DP_GREEN_E, GT_FALSE, 
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_8021x_RATE, 0, 0
        },
    
        /* IGMPv1/v2/v3 packet (format mac 01-00-5E-XX-XX-XX ip-protocol 2) */
        {
            CPSS_NET_INTERVENTION_IGMP_E,/* CPU CODE:6 */ CPU_PORT_IGMP_QUEUE, CPSS_DP_GREEN_E, GT_FALSE,
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_IGMP_RATE, 0, 0
        },
    
        /* OAM telnet packet */
        {
            CPSS_NET_OAM_TCP_TELNET_E,/* CPU CODE: 193 */ CPU_PORT_TELNET_QUEUE, CPSS_DP_GREEN_E, GT_FALSE,
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_TELNET_RATE, 0, 0
        },
    
        /* OAM ssh packet */
        {
            CPSS_NET_OAM_TCP_SSH_E,/* CPU CODE: 194 */ CPU_PORT_SSH_QUEUE, CPSS_DP_GREEN_E, GT_FALSE,
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_SSH_RATE, 0, 0
        },
    
        /* OAM http packet */
        {
            CPSS_NET_OAM_TCP_HTTP_E,/* CPU CODE: 195 */ CPU_PORT_HTTP_QUEUE, CPSS_DP_GREEN_E, GT_FALSE,
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_HTTP_RATE, 0, 0
        },
    
        /* OAM https packet */
        {
            CPSS_NET_OAM_TCP_HTTPS_E,/* CPU CODE: 196 */ CPU_PORT_HTTPS_QUEUE, CPSS_DP_GREEN_E, GT_FALSE,
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_HTTPS_RATE, 0, 0
        },
    
        /* OAM SNMP packet */
        {
            CPSS_NET_OAM_UDP_SNMP_E,/* CPU CODE: 197 */ CPU_PORT_SNMP_QUEUE, CPSS_DP_GREEN_E, GT_FALSE,
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_SNMP_RATE, 0, 0
        },
        
        /* packet bridge to CPU(e.g. forward by bridge engine to CPU port 63 */
        {
            CPSS_NET_BRIDGED_PACKET_FORWARD_E,/* CPU CODE:65 */ CPU_PORT_BRIDGE_QUEUE, CPSS_DP_GREEN_E, GT_FALSE,
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_BRIDGE_RATE, 0, 0
        },
            
        /* packet route to CPU(e.g. forward by route engine to CPU port 63 or trap to CPU */
        {
            CPSS_NET_ROUTED_PACKET_FORWARD_E,/* CPU CODE:64 */ CPU_PORT_ROUTE_QUEUE, CPSS_DP_GREEN_E, GT_FALSE,
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_ROUTE_RATE, 0, 0
        },
    
        {
            CPSS_NET_ROUTE_ENTRY_TRAP_E,/* CPU CODE:160 */ CPU_PORT_ROUTE_QUEUE, CPSS_DP_GREEN_E, GT_FALSE,
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_ROUTE_RATE, 0, 0
        },
        
        {
            CPSS_NET_IPV4_UC_ROUTE1_TRAP_E,/* CPU CODE:161 */ CPU_PORT_ROUTE_QUEUE, CPSS_DP_GREEN_E, GT_FALSE,
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_ROUTE_RATE, 0, 0
        },
        
        {
            CPSS_NET_IPV4_UC_ROUTE2_TRAP_E,/* CPU CODE:162 */ CPU_PORT_ROUTE_QUEUE, CPSS_DP_GREEN_E, GT_FALSE,
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_ROUTE_RATE, 0, 0
        },
        
        {
            CPSS_NET_IPV4_UC_ROUTE3_TRAP_E,/* CPU CODE:163 */ CPU_PORT_ROUTE_QUEUE, CPSS_DP_GREEN_E, GT_FALSE,
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_ROUTE_RATE, 0, 0
        },
    
        /*all system on the subnet  224.0.0.1*/
        {
            CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E,/* CPU CODE:16 */ CPU_PORT_ALL_MC_QUEUE, CPSS_DP_GREEN_E, GT_FALSE, 
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_ALL_MC_RATE, 0, 0
        },
    
        /*all routers on the subnet 224.0.0.2*/
        {
            CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E,/* CPU CODE:16 */ CPU_PORT_ALL_ROUTERS_QUEUE, CPSS_DP_GREEN_E, GT_FALSE, 
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_ALL_ROUTERS_RATE, 0, 0
        },
    
        /*All PIM Routers 224.0.0.13*/
        {
            CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E,/* CPU CODE:30 */ CPU_PORT_PIM_QUEUE, CPSS_DP_GREEN_E, GT_FALSE, 
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_PIM_RATE, 0, 0
        },
    
        /*VRRP 224.0.0.18*/
        {
            CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E,/* CPU CODE:31 */ CPU_PORT_VRRP_QUEUE, CPSS_DP_GREEN_E, GT_FALSE, 
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_VRRP_RATE, 0, 0
        },
    
        /* all TCP SYN packet */
        {
            CPSS_NET_TCP_SYN_TO_CPU_E,/* CPU CODE:190 */ CPU_PORT_TCPSYN_QUEUE, CPSS_DP_GREEN_E, GT_FALSE, 
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E, CPU_PORT_TCPSYN_RATE, 0, 0
        },
    
        /* other */
    };
    
#endif


/*******************************************************************************
* dxChNetIfDsaRegularTagParse
*
* DESCRIPTION:
*       parse the DSA tag parameters from the DSA tag on the packet. --
*           ===== regular DSA tag ====
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number (different DXCH devices may support differently
*                the DSA tag)
*       dsaWord - the DSA tag in the packet (single word -> 32 bits)
*       dsaInfoPtr - (pointer to) the DSA parameters that were parsed from the
*                   packet --> All the common fields are already set !
*                   and the DSA tag
*                   All other fields are "reset"
*
* OUTPUTS:
*       dsaInfoPtr - (pointer to) the DSA parameters that were parsed from the
*                   packet
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on bad device number
*       GT_BAD_PTR       - one of the parameters in NULL pointer.
*
*
* COMMENTS:
*           ===== regular DSA tag ====
*
*******************************************************************************/
static GT_STATUS dxChNetIfDsaRegularTagParse
(
    IN  GT_U8                          devNum,
    IN  GT_U32                         dsaWord,
    INOUT CPSS_DXCH_NET_DSA_PARAMS_STC   *dsaInfoPtr
)
{
    GT_STATUS   rc;
    CPSS_DXCH_NET_DSA_TO_CPU_STC         *toCpuPtr = &(dsaInfoPtr->dsaInfo.toCpu);
    CPSS_DXCH_NET_DSA_FROM_CPU_STC       *fromCpuPtr = &(dsaInfoPtr->dsaInfo.fromCpu);
    CPSS_DXCH_NET_DSA_TO_ANALYZER_STC    *toAnalyzerPtr = &(dsaInfoPtr->dsaInfo.toAnalyzer);
    CPSS_DXCH_NET_DSA_FORWARD_STC        *forwardPtr = &(dsaInfoPtr->dsaInfo.forward);
    GT_U32  cpuCode; /* cpu code  -- for "to cpu" */
    GT_U16  vidx;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    switch(dsaInfoPtr->dsaType)
    {
        case CPSS_DXCH_NET_DSA_CMD_TO_CPU_E:
            toCpuPtr->isTagged = (GT_BOOL)
                                U32_GET_FIELD_MAC(dsaWord,
                                29,
                                1);

            toCpuPtr->devNum = (GT_U8)U32_GET_FIELD_MAC(dsaWord,
                                          24,
                                          5);

            if(toCpuPtr->srcIsTrunk == GT_TRUE)
            {
                /* trunk */

                /* 5 bits */
                toCpuPtr->interface.srcTrunkId = (GT_TRUNK_ID)U32_GET_FIELD_MAC(dsaWord,
                                                19, /*19*/
                                                5); /*5*/
            }
            else
            {
                /* port */

                /* 5 bits */
                toCpuPtr->interface.portNum = (GT_U8)
                        U32_GET_FIELD_MAC(dsaWord,
                        19,/*19*/
                        5);  /*5*/
            }

           cpuCode =
                (GT_U8)(((U32_GET_FIELD_MAC(dsaWord ,
                              16,
                              3))<< 1) |
                        U32_GET_FIELD_MAC(dsaWord,
                        12,
                        1));

            /* convert CPU code from DSA to the unified "RX CPU code"*/
            rc = prvCpssDxChNetIfDsaToCpuCode(cpuCode,&toCpuPtr->cpuCode);
            if(rc != GT_OK)
            {
                /* should not happen */
                return rc;
            }
        break;

        case CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E:
            toAnalyzerPtr->isTagged = (GT_BOOL)
                                U32_GET_FIELD_MAC(dsaWord,
                                29,  /*29*/
                                1);    /*1*/
            toAnalyzerPtr->devPort.devNum = (GT_U8)U32_GET_FIELD_MAC(dsaWord,
                                          24,  /*24*/
                                          5);    /*5*/

            /* src is never trunk !!! */

            /* 5 bits */
            toAnalyzerPtr->devPort.portNum = (GT_U8)U32_GET_FIELD_MAC(dsaWord,
                                            19, /*19*/
                                            5); /*5*/

            toAnalyzerPtr->rxSniffer = (GT_BOOL)
                    U32_GET_FIELD_MAC(dsaWord,
                                    18, /*18*/
                                    1);/*1*/
            break;
        case CPSS_DXCH_NET_DSA_CMD_FORWARD_E:
            forwardPtr->srcIsTagged = (GT_BOOL)
                                U32_GET_FIELD_MAC(dsaWord,
                                29,  /*29*/
                                1);    /*1*/
            forwardPtr->srcDev = (GT_U8)U32_GET_FIELD_MAC(dsaWord,
                                          24,  /*24*/
                                          5);    /*5*/

            forwardPtr->srcIsTrunk = (GT_BOOL)U32_GET_FIELD_MAC(dsaWord,
                                       18,/*18*/
                                       1);  /*1*/


            if(forwardPtr->srcIsTrunk == GT_TRUE)
            {
                /* trunk */

                /* 5 bits */
                forwardPtr->source.trunkId = (GT_TRUNK_ID)
                        U32_GET_FIELD_MAC(dsaWord,
                                      19,/*19*/
                                      5);  /*5*/
            }
            else
            {
                /* port */

                /* 5 bits */
                forwardPtr->source.portNum = (GT_U8)
                        U32_GET_FIELD_MAC(dsaWord,
                                      19,/*19*/
                                      5);  /*5*/
            }
            break;
        case CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E:
            if(U32_GET_FIELD_MAC(dsaWord,18,1))/* use vidx */
            {
                /* bit 19 is reserved and is not part of the vidx bits !!! */

                vidx = (GT_U16)
                        U32_GET_FIELD_MAC(dsaWord,
                                20,
                                9 );

                /* check if VIDX means VLAN_ID */
                if(vidx == VID_INSTEAD_VIDX_9_BITS_CNS)
                {
                    fromCpuPtr->dstInterface.type = CPSS_INTERFACE_VID_E;
                    fromCpuPtr->dstInterface.vlanId = dsaInfoPtr->commonParams.vid;
                }
                else
                {
                    fromCpuPtr->dstInterface.type = CPSS_INTERFACE_VIDX_E;
                    fromCpuPtr->dstInterface.vidx = vidx;
                }
            }
            else   /* not using vidx */
            {
                /* destination trunk is not supported */
                fromCpuPtr->dstInterface.type = CPSS_INTERFACE_PORT_E;

                fromCpuPtr->dstInterface.devPort.portNum = (GT_U8)
                    /* 5 bits */
                    U32_GET_FIELD_MAC(dsaWord,
                                    19,
                                    5) ;

                fromCpuPtr->dstInterface.devPort.devNum = (GT_U8)
                    U32_GET_FIELD_MAC(dsaWord,
                                            24, /*24*/
                                            5); /*5*/

                fromCpuPtr->extDestInfo.devPort.dstIsTagged = (GT_BOOL)
                                U32_GET_FIELD_MAC(dsaWord,
                                29,  /*29*/
                                1);    /*1*/

            }
            break;
        default:/* default for compilation warnings */
            break;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfDsaTagParse
*
* DESCRIPTION:
*       parse the DSA tag parameters from the DSA tag on the packet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number (different DXCH devices may support differently
*                the DSA tag)
*       dsaBytesPtr - pointer to the start of DSA tag in the packet
*
* OUTPUTS:
*       dsaInfoPtr - (pointer to) the DSA parameters that were parsed from the
*                   packet
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_DSA_PARSING_ERROR     - DSA tag parsing error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*           support both :
*           ===== regular DSA tag  ====
*           ===== extended DSA tag ====
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfDsaTagParse
(
    IN  GT_U8                          devNum,
    IN  GT_U8                          *dsaBytesPtr,
    OUT CPSS_DXCH_NET_DSA_PARAMS_STC   *dsaInfoPtr
)
{
    GT_STATUS   rc;
    GT_U32  longCpuCode; /*long cpu code  -- for "to cpu" */
    GT_U32  wordsPtr[2];
    CPSS_DXCH_NET_DSA_TO_CPU_STC         *toCpuPtr;
    CPSS_DXCH_NET_DSA_FROM_CPU_STC       *fromCpuPtr;
    CPSS_DXCH_NET_DSA_TO_ANALYZER_STC    *toAnalyzerPtr;
    CPSS_DXCH_NET_DSA_FORWARD_STC        *forwardPtr;
    GT_TRUNK_ID  excludeTrunkId;
    GT_U32  excluseIsTrunk;
    GT_U16  vidx;
    GT_BOOL extendedDsa;
    /* added bytes to original packet size (FEr 89).*/
    GT_U32  addedOrigPacketSize;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(dsaBytesPtr);
    CPSS_NULL_PTR_CHECK_MAC(dsaInfoPtr);

    /* need to copy for alignment issues*/
    wordsPtr[0] = (((GT_U32)dsaBytesPtr[0]) << 24) |
                  (((GT_U32)dsaBytesPtr[1]) << 16) |
                  (((GT_U32)dsaBytesPtr[2]) << 8)  |
                  (((GT_U32)dsaBytesPtr[3]) << 0) ;

    cpssOsMemSet(dsaInfoPtr,0,sizeof(*dsaInfoPtr));

    /* set the common parameters */
    dsaInfoPtr->dsaType = (CPSS_DXCH_NET_DSA_CMD_ENT)
                                U32_GET_FIELD_MAC(wordsPtr[0] ,30,2);

    dsaInfoPtr->commonParams.vid = (GT_U16)U32_GET_FIELD_MAC(wordsPtr[0],0,12);


    dsaInfoPtr->commonParams.vpt = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[0],13,3);

    extendedDsa = GT_TRUE;

    /* check the extended bit */
    if(0 == U32_GET_FIELD_MAC(wordsPtr[0] ,12,1))
    {
        extendedDsa = GT_FALSE;
    }
    else if(dsaInfoPtr->dsaType == CPSS_DXCH_NET_DSA_CMD_TO_CPU_E)
    {
        /* check that the "old" cpu opcode is set the 0xF (with the extended bit) */
        if(0x7 != U32_GET_FIELD_MAC(wordsPtr[0] ,16,3))
        {
            extendedDsa = GT_FALSE;
        }
    }

    if(dsaInfoPtr->dsaType != CPSS_DXCH_NET_DSA_CMD_TO_CPU_E)
    {
        dsaInfoPtr->commonParams.cfiBit =
            (GT_U8)U32_GET_FIELD_MAC(wordsPtr[0],16,1);

    }

    if(extendedDsa == GT_FALSE)
    {
        dsaInfoPtr->commonParams.dsaTagType = CPSS_DXCH_NET_DSA_TYPE_REGULAR_E;

        /* we must set all the common parameters prior to calling next
           function */

        return dxChNetIfDsaRegularTagParse(devNum,wordsPtr[0],dsaInfoPtr);
    }

    /* copy the data of the second word */
    wordsPtr[1] = (((GT_U32)dsaBytesPtr[4]) << 24) |
                  (((GT_U32)dsaBytesPtr[5]) << 16) |
                  (((GT_U32)dsaBytesPtr[6]) <<  8) |
                  (((GT_U32)dsaBytesPtr[7]) <<  0) ;
	osPrintfErr("DSA %02x %02x %02x %02x %02x %02x %02x %02x\n",	\
		dsaBytesPtr[0],dsaBytesPtr[1],dsaBytesPtr[2],dsaBytesPtr[3],	\
		dsaBytesPtr[4],dsaBytesPtr[5],dsaBytesPtr[6],dsaBytesPtr[7]);

    dsaInfoPtr->commonParams.dsaTagType = CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E;

    toCpuPtr = &(dsaInfoPtr->dsaInfo.toCpu);
    fromCpuPtr = &(dsaInfoPtr->dsaInfo.fromCpu);
    toAnalyzerPtr = &(dsaInfoPtr->dsaInfo.toAnalyzer);
    forwardPtr = &(dsaInfoPtr->dsaInfo.forward);

    if(dsaInfoPtr->dsaType == CPSS_DXCH_NET_DSA_CMD_TO_CPU_E)
    {
        dsaInfoPtr->commonParams.cfiBit =
            (GT_U8)U32_GET_FIELD_MAC(wordsPtr[1],30,1);
    }

    switch(dsaInfoPtr->dsaType)
    {
        case CPSS_DXCH_NET_DSA_CMD_TO_CPU_E:

            toCpuPtr->isTagged = (GT_BOOL)
                                U32_GET_FIELD_MAC(wordsPtr[0],29,1);

            toCpuPtr->devNum = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[0],24,5);

            toCpuPtr->srcIsTrunk = (GT_BOOL)
                                U32_GET_FIELD_MAC(wordsPtr[1],27,1);


            if(toCpuPtr->srcIsTrunk == GT_TRUE)
            {
                /* trunk */

                /* 5 bits */
                toCpuPtr->interface.srcTrunkId = (GT_TRUNK_ID)U32_GET_FIELD_MAC(wordsPtr[0],19,5);
                /* 2 more bits */
                toCpuPtr->interface.srcTrunkId |= (GT_TRUNK_ID)
                        (U32_GET_FIELD_MAC(wordsPtr[1],10,2)) << 5;
            }
            else
            {
                /* port */

                /* 5 bits */
                toCpuPtr->interface.portNum = (GT_U8)
                        U32_GET_FIELD_MAC(wordsPtr[0],19,5);
                /* 1 more bit */
                toCpuPtr->interface.portNum |=
                        (U32_GET_FIELD_MAC(wordsPtr[1],10,1)) << 5;
            }

            toCpuPtr->isEgressPipe = (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[1],8,1);

            toCpuPtr->originByteCount = U32_GET_FIELD_MAC(wordsPtr[1],12,14);

            toCpuPtr->timestamp = U32_GET_FIELD_MAC(wordsPtr[1],12,14) << 1;
            toCpuPtr->timestamp |= U32_GET_FIELD_MAC(wordsPtr[1],9,1);

            /* When a packet is received from a remote device, the CPU should
              calculate the correct byte count by decrementing the DSA tag
              size from the PktOrigBC. (FEr#89)*/
            if((PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
               PRV_CPSS_DXCH_PACKET_ORIG_BYTE_COUNT_WA_E) == GT_TRUE) &&
               (toCpuPtr->isEgressPipe == 0) && (toCpuPtr->devNum != PRV_CPSS_HW_DEV_NUM_MAC(devNum)))
            {
                if(toCpuPtr->isTagged == 1)
                {
                    /* 8 bytes has replaced 4 bytes VLAN tag.
                      So only 4 bytes were added */
                    addedOrigPacketSize = 4;
                }
                else
                {
                    /* 8 bytes were added. */
                    addedOrigPacketSize = 8;
                }

                toCpuPtr->originByteCount -= addedOrigPacketSize;
            }

            toCpuPtr->wasTruncated = (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[1],26,1);

            longCpuCode = U32_GET_FIELD_MAC(wordsPtr[1],0,8);

            /* convert CPU code from DSA to the unified "RX CPU code"*/
            rc = prvCpssDxChNetIfDsaToCpuCode(longCpuCode,&toCpuPtr->cpuCode);
            if(rc != GT_OK)
            {
                return rc;
            }

			/* zhangdi add for debug */			
			if(toCpuPtr->srcIsTrunk)
			{
				osPrintfErr("DSA(TO_CPU) trunk(%d,%d) vlan%d(%s) cpu-code(%d,%d) length(%dB)\n", \
					toCpuPtr->devNum,toCpuPtr->interface.srcTrunkId, \
					dsaInfoPtr->commonParams.vid,toCpuPtr->isTagged ? "tag":"untag", \
					toCpuPtr->cpuCode,longCpuCode,toCpuPtr->originByteCount);
				
			}
			else
			{
			    osPrintfErr("DSA(TO_CPU) port(%d,%d) vlan%d(%s) cpu-code(%d,%d) length(%dB)\n", \
					toCpuPtr->devNum,toCpuPtr->interface.portNum, \
					dsaInfoPtr->commonParams.vid,toCpuPtr->isTagged ? "tag":"untag", \
					toCpuPtr->cpuCode,longCpuCode,toCpuPtr->originByteCount);
	
				
			} 

            /* check for Cheetah3 device */
            if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily >=
                CPSS_PP_FAMILY_CHEETAH3_E)
            {
                /* get Fast Stack Fail over Recovery parameters */
                dsaInfoPtr->commonParams.dropOnSource =
                    (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[1], 29, 1);

                dsaInfoPtr->commonParams.packetIsLooped =
                    (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[1], 28, 1);
            }

        break;

        case CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E:
            toAnalyzerPtr->isTagged = (GT_BOOL)
                                U32_GET_FIELD_MAC(wordsPtr[0],29,1);
            toAnalyzerPtr->devPort.devNum = (GT_U8)
                                U32_GET_FIELD_MAC(wordsPtr[0],24,5);

            /* src is never trunk !!! */

            /* 5 bits */
            toAnalyzerPtr->devPort.portNum = (GT_U8)
                                U32_GET_FIELD_MAC(wordsPtr[0],19,5);
            /* 1 more bit */
            toAnalyzerPtr->devPort.portNum |=
                    (U32_GET_FIELD_MAC(wordsPtr[1],10,1)) << 5;

            toAnalyzerPtr->rxSniffer = (GT_BOOL)
                    U32_GET_FIELD_MAC(wordsPtr[0],18,1);

            /* check for Cheetah3 device */
            if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily >=
                CPSS_PP_FAMILY_CHEETAH3_E)
            {
                /* get Fast Stack Fail over Recovery parameters */
                dsaInfoPtr->commonParams.dropOnSource =
                    (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[1], 29, 1);

                dsaInfoPtr->commonParams.packetIsLooped =
                    (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[1], 28, 1);
            }

            break;
        case CPSS_DXCH_NET_DSA_CMD_FORWARD_E:
            forwardPtr->srcIsTagged = (GT_BOOL)
                                U32_GET_FIELD_MAC(wordsPtr[0],29,1);

            forwardPtr->srcDev = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[0],24,5);

            forwardPtr->srcIsTrunk = (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[0],18,1);

            if(forwardPtr->srcIsTrunk == GT_TRUE)
            {
                /* trunk */

                /* 5 bits */
                forwardPtr->source.trunkId = (GT_TRUNK_ID)
                        U32_GET_FIELD_MAC(wordsPtr[0],19,5);
                /* 2 more bits */
                forwardPtr->source.trunkId |= (GT_TRUNK_ID)
                        (U32_GET_FIELD_MAC(wordsPtr[1],29,2))<< 5;
            }
            else
            {
                /* port */

                /* 5 bits */
                forwardPtr->source.portNum = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[0],19,5);
                /* 1 more bit */
                forwardPtr->source.portNum |= (U32_GET_FIELD_MAC(wordsPtr[1],29,1)) << 5;
            }

            forwardPtr->egrFilterRegistered = (GT_BOOL)
                        U32_GET_FIELD_MAC(wordsPtr[1],28,1);

            forwardPtr->wasRouted = (GT_BOOL)
                        U32_GET_FIELD_MAC(wordsPtr[1],25,1);

            forwardPtr->srcId =
                        U32_GET_FIELD_MAC(wordsPtr[1],20,5);

            forwardPtr->qosProfileIndex =
                        U32_GET_FIELD_MAC(wordsPtr[1],13,7);

            if(U32_GET_FIELD_MAC(wordsPtr[1],12,1)) /* useVidx*/
            {
                vidx = (GT_U16)
                        U32_GET_FIELD_MAC(wordsPtr[1],0,12);

                /* check if VIDX means VLAN_ID */
                if(vidx == VID_INSTEAD_VIDX_CNS)
                {
                    forwardPtr->dstInterface.type = CPSS_INTERFACE_VID_E;
                    forwardPtr->dstInterface.vlanId = dsaInfoPtr->commonParams.vid;
                }
                else
                {
                    forwardPtr->dstInterface.type = CPSS_INTERFACE_VIDX_E;
                    forwardPtr->dstInterface.vidx = vidx;
                }
            }
            else
            {
                /* the destination is never trunk (the ingress device choose a
                   port in the trunk) */
                forwardPtr->dstInterface.type = CPSS_INTERFACE_PORT_E;
                forwardPtr->dstInterface.devPort.portNum = (GT_U8)
                        U32_GET_FIELD_MAC(wordsPtr[1],5,6);

                forwardPtr->dstInterface.devPort.devNum = (GT_U8)
                        U32_GET_FIELD_MAC(wordsPtr[1],0,5);
            }

            /* check for Cheetah3 device */
            if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily >=
                CPSS_PP_FAMILY_CHEETAH3_E)
            {
                /* get Fast Stack Fail over Recovery parameters */
                dsaInfoPtr->commonParams.dropOnSource =
                    (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[1], 27, 1);

                dsaInfoPtr->commonParams.packetIsLooped =
                    (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[1], 26, 1);
            }

            break;
        case CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E:
            fromCpuPtr->srcDev = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[1],15,5);
            fromCpuPtr->tc = (GT_U8)
               (
                      U32_GET_FIELD_MAC(wordsPtr[0],17,1)        |
                    ((U32_GET_FIELD_MAC(wordsPtr[1],14,1)) << 1) |
                    ((U32_GET_FIELD_MAC(wordsPtr[1],27,1)) << 2)
                );


            /* check for Cheetah3 device */
            if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily >=
                CPSS_PP_FAMILY_CHEETAH3_E)
            {
                /* get Fast Stack Fail over Recovery parameters */
                dsaInfoPtr->commonParams.dropOnSource =
                    (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[1], 26, 1);

                dsaInfoPtr->commonParams.packetIsLooped =
                    (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[1], 25, 1);
            }
            /* Not suppoted for Ch3 and above devices.
               To get DP, call cpssDxChNetIfFromCpuDpGet */
            else
            {
                /* cheetah support only those 2 types */
                fromCpuPtr->dp = ((U32_GET_FIELD_MAC(wordsPtr[1],25,1)) ?
                                              CPSS_DP_RED_E :
                                              CPSS_DP_GREEN_E);


                if(U32_GET_FIELD_MAC(wordsPtr[1],26,1))
                {
                    /* unknown setting !! the DXCH not support it */

                    /* since there is no reason to DROP the packet , let
                       Application to handle it */
                    fromCpuPtr->dp = CPSS_DP_RED_E;
                }

            }

            fromCpuPtr->srcId = U32_GET_FIELD_MAC(wordsPtr[1],20,5);

            fromCpuPtr->egrFilterEn = U32_GET_FIELD_MAC(wordsPtr[1],30,1);

            fromCpuPtr->cascadeControl = U32_GET_FIELD_MAC(wordsPtr[1],29,1);

            fromCpuPtr->egrFilterRegistered = U32_GET_FIELD_MAC(wordsPtr[1],28,1);

            if(U32_GET_FIELD_MAC(wordsPtr[0],18,1))/* use vidx */
            {
                vidx = (GT_U16)
                    (
                         U32_GET_FIELD_MAC(wordsPtr[0],19,10)       |
                        (U32_GET_FIELD_MAC(wordsPtr[1],12,2 )<< 10)
                    );

                /* check if VIDX means VLAN_ID */
                if(vidx == VID_INSTEAD_VIDX_CNS)
                {
                    fromCpuPtr->dstInterface.type = CPSS_INTERFACE_VID_E;
                    fromCpuPtr->dstInterface.vlanId = dsaInfoPtr->commonParams.vid;
                }
                else
                {
                    fromCpuPtr->dstInterface.type = CPSS_INTERFACE_VIDX_E;
                    fromCpuPtr->dstInterface.vidx = vidx;
                }

                excluseIsTrunk = U32_GET_FIELD_MAC(wordsPtr[1],11,1);

                if(excluseIsTrunk)
                {
                    /* bit 10 - MirrorToAllCPUs */
                    fromCpuPtr->extDestInfo.multiDest.mirrorToAllCPUs = (GT_BOOL)
                                U32_GET_FIELD_MAC(wordsPtr[1],10,1);

                    excludeTrunkId = (GT_TRUNK_ID)U32_GET_FIELD_MAC(wordsPtr[1],0,7);

                    if(excludeTrunkId == 0)
                    {
                        /* no interface was excluded */
                        fromCpuPtr->extDestInfo.multiDest.excludeInterface = GT_FALSE;
                    }
                    else
                    {
                        fromCpuPtr->extDestInfo.multiDest.excludeInterface = GT_TRUE;
                        fromCpuPtr->extDestInfo.multiDest.excludedInterface.type = CPSS_INTERFACE_TRUNK_E;
                        fromCpuPtr->extDestInfo.multiDest.excludedInterface.trunkId = excludeTrunkId;
                    }
                }
                else
                {
                    fromCpuPtr->extDestInfo.multiDest.excludeInterface = GT_TRUE;
                    fromCpuPtr->extDestInfo.multiDest.excludedInterface.type = CPSS_INTERFACE_PORT_E;

                    fromCpuPtr->extDestInfo.multiDest.excludedInterface.devPort.portNum = (GT_U8)
                        U32_GET_FIELD_MAC(wordsPtr[1],5,6);

                    fromCpuPtr->extDestInfo.multiDest.excludedInterface.devPort.devNum = (GT_U8)
                        U32_GET_FIELD_MAC(wordsPtr[1],0,5);
                }
            }
            else   /* not using vidx */
            {
                /* destination trunk is not supported */
                fromCpuPtr->dstInterface.type = CPSS_INTERFACE_PORT_E;

                fromCpuPtr->dstInterface.devPort.portNum = (GT_U8)
                (
                    /* 5 bits */
                    U32_GET_FIELD_MAC(wordsPtr[0],19,5) |
                    /* 1 more bit */
                    (U32_GET_FIELD_MAC(wordsPtr[1],10,1)<< 5)
                );

                fromCpuPtr->dstInterface.devPort.devNum = (GT_U8)
                    U32_GET_FIELD_MAC(wordsPtr[0],24,5);

                fromCpuPtr->extDestInfo.devPort.dstIsTagged = (GT_BOOL)
                                U32_GET_FIELD_MAC(wordsPtr[0],29,1);

                fromCpuPtr->extDestInfo.devPort.mailBoxToNeighborCPU = (GT_BOOL)
                        U32_GET_FIELD_MAC(wordsPtr[1],13,1);

            }


            break;
        default:/* default for compilation warnings */
            break;
    }

    /* check if need to WA the "wrong trunk id source port
       information of packet to CPU" */
    rc = prvCpssDxChTrunkTrafficToCpuWaUpToMemberConvert(devNum,dsaInfoPtr);
    if(rc != GT_OK && rc != GT_NO_CHANGE)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfDsaTagBuild
*
* DESCRIPTION:
*       Build DSA tag bytes on the packet from the DSA tag parameters .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number (different DXCH devices may support differently
*                the DSA tag)
*       dsaInfoPtr - (pointer to) the DSA parameters that need to be built into
*                   the packet
*
* OUTPUTS:
*       dsaBytesPtr - pointer to the start of DSA tag in the packet
*                     This pointer in the packet should hold space for :
*                     4 bytes when using regular DSA tag.
*                     8 bytes when using extended DSA tag.
*                     This function only set values into the 4 or 8 bytes
*                     according to the parameters in parameter dsaInfoPtr.
*                     NOTE : this memory is the caller's responsibility (to
*                     allocate / free)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*           support both :
*           ===== regular DSA tag  ====
*           ===== extended DSA tag ====
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfDsaTagBuild
(
    IN  GT_U8                          devNum,
    IN  CPSS_DXCH_NET_DSA_PARAMS_STC   *dsaInfoPtr,
    OUT GT_U8                          *dsaBytesPtr
)
{
    GT_STATUS rc;
    GT_U32  wordsPtr[2] = {0,0};   /* 2 words of DSA tag */
    CPSS_DXCH_NET_DSA_TO_CPU_STC         *toCpuPtr;/* pointer to the "to cpu" format */
    CPSS_DXCH_NET_DSA_FROM_CPU_STC       *fromCpuPtr;/* pointer to the "from cpu" format */
    CPSS_DXCH_NET_DSA_TO_ANALYZER_STC    *toAnalyzerPtr;/* pointer to the "to analyzer" format */
    CPSS_DXCH_NET_DSA_FORWARD_STC        *forwardPtr;/* pointer to the "forward" format */
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT   dsaCpuCode;/* HW dsa cpu opcode */
    GT_BOOL useVidx = GT_FALSE;/* use vidx (and vlan) or use single interface (port/trunk)*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(dsaBytesPtr);
    CPSS_NULL_PTR_CHECK_MAC(dsaInfoPtr);

    /* need to copy for alignment issues*/
    toCpuPtr = &(dsaInfoPtr->dsaInfo.toCpu);
    fromCpuPtr = &(dsaInfoPtr->dsaInfo.fromCpu);
    toAnalyzerPtr = &(dsaInfoPtr->dsaInfo.toAnalyzer);
    forwardPtr = &(dsaInfoPtr->dsaInfo.forward);

	/* Add from code of 275  zhangdi */
	if(CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E == dsaInfoPtr->dsaType) {
		if(CPSS_INTERFACE_PORT_E == fromCpuPtr->dstInterface.type) {
			osPrintfDbg("build DSA FROM_CPU port(%d,%d) vlan%d(%s)\n", \
							fromCpuPtr->dstInterface.devPort.devNum,	\
							fromCpuPtr->dstInterface.devPort.portNum,	\
							dsaInfoPtr->commonParams.vid, \
							(fromCpuPtr->extDestInfo.devPort.dstIsTagged == GT_TRUE)?"tag":"untag");
		}
		else if(CPSS_INTERFACE_TRUNK_E == fromCpuPtr->dstInterface.type) {
			osPrintfDbg("build DSA FROM_CPU trunk(%d) vlan%d(%s)\n", \
							fromCpuPtr->dstInterface.trunkId,	\
							dsaInfoPtr->commonParams.vid, \
							(fromCpuPtr->extDestInfo.devPort.dstIsTagged == GT_TRUE)?"tag":"untag");
		}
	}



    /************************/
    /* build the first word */
    /************************/

    switch(dsaInfoPtr->dsaType)
    {
        case CPSS_DXCH_NET_DSA_CMD_TO_CPU_E:
            /* bits 30:31 - TagCommand 0 = TO_CPU */
            /*U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],30,2,0);*/

            /* bit 29 -- SrcTagged/TrgTagged */
            if(toCpuPtr->isTagged == GT_TRUE)
            {
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],29,1,1);
            }

            /* bits 24:28 -- SrcDev/TrgDev */
            if(toCpuPtr->devNum >= BIT_5)
            {
                return GT_BAD_PARAM;
            }
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],24,5,toCpuPtr->devNum);

            if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E)
            {
                if(toCpuPtr->srcIsTrunk == GT_FALSE)
                {
                    /* bits 19:23 -- SrcPort[4:0]/TrgPort[4:0] */
                    if(toCpuPtr->interface.portNum >= BIT_6)
                    {
                        return GT_BAD_PARAM;
                    }
                    U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],19,5,toCpuPtr->interface.portNum);
                }
                else
                {
                    if(toCpuPtr->interface.srcTrunkId >= BIT_7)
                    {
                        return GT_BAD_PARAM;
                    }
                    /* bits 19:23 -- SrcTrunk[4:0] */
                    U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],19,5,toCpuPtr->interface.srcTrunkId);
                }

                /* bits 16:18 -- CPU_Code[3:1] */
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],16,3,7);
                /* bit 12 -- CPU_Code[0] --> will be set by
                   CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E */
            }
            else
            {
                if(toCpuPtr->interface.portNum >= BIT_5)
                {
                    return GT_BAD_PARAM;
                }
                /* bits 19:23 -- SrcPort[4:0] */
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],19,5,toCpuPtr->interface.portNum);

                rc = prvCpssDxChNetIfCpuToDsaCode(toCpuPtr->cpuCode,&dsaCpuCode);
                if(rc != GT_OK)
                {
                    return rc;
                }

                if(dsaCpuCode > 14)
                {
                    /* support values 0..15
                      but value 15 is special for "internal" use !!!
                    */
                    return GT_BAD_PARAM;
                }

                /* bits 16:18 -- CPU_Code[3:1] */
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],16,3,((GT_U32)dsaCpuCode) >> 1);
                /* bit 12 -- CPU_Code[0] */
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],12,1,dsaCpuCode);/* use masked marco !! */
            }

            break;
        case CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E:
            /* bits 30:31 - TagCommand 1 = FROM_CPU */
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],30,2,1);
            /* bit 29 -- SrcTagged */
            if(fromCpuPtr->dstInterface.type == CPSS_INTERFACE_PORT_E &&
               fromCpuPtr->extDestInfo.devPort.dstIsTagged == GT_TRUE)
            {
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],29,1,1);
            }

            if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E)
            {
                if(fromCpuPtr->dstInterface.type == CPSS_INTERFACE_VIDX_E)
                {
                    useVidx = GT_TRUE;
                    if(fromCpuPtr->dstInterface.vidx >= (BIT_12-1))
                    {
                        return GT_BAD_PARAM;
                    }
                    U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],19,10,fromCpuPtr->dstInterface.vidx);
                }
                else if (fromCpuPtr->dstInterface.type == CPSS_INTERFACE_VID_E)
                {
                    useVidx = GT_TRUE;
                    U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],19,10,VID_INSTEAD_VIDX_CNS);
                }

                /* bit 17 -- TC[0] */
                if(fromCpuPtr->tc >= BIT_3)
                {
                    return GT_BAD_PARAM;
                }

                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],17,1,fromCpuPtr->tc);
            }
            else
            {
                if(fromCpuPtr->dstInterface.type == CPSS_INTERFACE_VIDX_E)
                {
                    useVidx = GT_TRUE;
                    if(fromCpuPtr->dstInterface.vidx >= (BIT_9-1))
                    {
                        return GT_BAD_PARAM;
                    }
                    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],20,9,fromCpuPtr->dstInterface.vidx);
                }
                else if (fromCpuPtr->dstInterface.type == CPSS_INTERFACE_VID_E)
                {
                    useVidx = GT_TRUE;
                    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],20,9,VID_INSTEAD_VIDX_9_BITS_CNS);
                }

                /* When the tag is not extended, this field is set to 1. */
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],17,1,1);
            }

            if(useVidx == GT_TRUE)
            {
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],18,1,1);
            }
            else if(fromCpuPtr->dstInterface.type == CPSS_INTERFACE_TRUNK_E)
            {
                return GT_BAD_PARAM;/* trunk is not supported !!! */
            }
            else if(fromCpuPtr->dstInterface.type == CPSS_INTERFACE_PORT_E)
            {
                if(fromCpuPtr->dstInterface.devPort.devNum >= BIT_5)
                {
                    return GT_BAD_PARAM;
                }

                if((fromCpuPtr->dstInterface.devPort.portNum >= BIT_5 &&
                    dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_TYPE_REGULAR_E) ||
                    fromCpuPtr->dstInterface.devPort.portNum >= BIT_6)
                {
                    return GT_BAD_PARAM;
                }
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],24,5,fromCpuPtr->dstInterface.devPort.devNum);
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],19,5,fromCpuPtr->dstInterface.devPort.portNum);
            }
            else
            {
                return GT_BAD_PARAM;
            }



            /* bit 16 - CFI */
            if(dsaInfoPtr->commonParams.cfiBit == 1)
            {
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],16,1,1);
            }
            else if(dsaInfoPtr->commonParams.cfiBit > 1)
            {
                return GT_BAD_PARAM;
            }


            break;
        case CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E:
            /* bits 30:31 - TagCommand 2 = TO_ANALYZER */
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],30,2,2);

            /* bit 29 -- SrcTagged/TrgTagged */
            if(toAnalyzerPtr->isTagged == GT_TRUE)
            {
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],29,1,1);
            }

            if(toAnalyzerPtr->devPort.devNum >= BIT_5)
            {
                return GT_BAD_PARAM;
            }

            if((toAnalyzerPtr->devPort.portNum >= BIT_5 &&
                dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_TYPE_REGULAR_E) ||
                toAnalyzerPtr->devPort.portNum >= BIT_6)
            {
                return GT_BAD_PARAM;
            }

            /* bits 24:28 -- SrcPort/TrgPort */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],24,5,toAnalyzerPtr->devPort.devNum);

            /* bits 19:23 -- SrcDev/TrgDev */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],19,5,toAnalyzerPtr->devPort.portNum);

            /* bit 18 -- rxSniff */
            if(toAnalyzerPtr->rxSniffer == 1)
            {
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],18,1,1);
            }
            else if(dsaInfoPtr->commonParams.cfiBit > 1)
            {
                return GT_BAD_PARAM;
            }

            /* bit 17 = 0 -- reserved */

            /* bit 16 - CFI */
            if(dsaInfoPtr->commonParams.cfiBit == 1)
            {
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],16,1,1);
            }

            break;
        case CPSS_DXCH_NET_DSA_CMD_FORWARD_E:
            /* bits 30:31 - TagCommand 3 = FORWARD */
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],30,2,3);

            /* bit 29 -- SrcTagged */
            if(forwardPtr->srcIsTagged == GT_TRUE)
            {
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],29,1,1);
            }

            if(forwardPtr->srcDev >= BIT_5)
            {
                return GT_BAD_PARAM;
            }

            /* bits 24:28 -- SrcDev */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],24,5,forwardPtr->srcDev);

            if(forwardPtr->srcIsTrunk == GT_FALSE)
            {
                if((forwardPtr->source.portNum >= BIT_5 &&
                    dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_TYPE_REGULAR_E) ||
                    forwardPtr->source.portNum >= BIT_6)
                {
                    return GT_BAD_PARAM;
                }

                /* bits 19:23 -- SrcPort[4:0] */
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],19,5,forwardPtr->source.portNum);

                /* bit 18 -- SrcIsTrunk */
                /* not trunk */
            }
            else
            {
                if((forwardPtr->source.trunkId >= BIT_5 &&
                    dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_TYPE_REGULAR_E) ||
                    forwardPtr->source.trunkId >= BIT_7)
                {
                    return GT_BAD_PARAM;
                }

                /* bits 19:23 -- SrcTrunk[4:0] */
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],19,5,forwardPtr->source.trunkId);

                /* bit 18 -- SrcIsTrunk */
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],18,1,1);
            }

            /* bit 17 = 0 -- reserved */

            /* bit 16 - CFI */
            if(dsaInfoPtr->commonParams.cfiBit == 1)
            {
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],16,1,1);
            }
            else if(dsaInfoPtr->commonParams.cfiBit > 1)
            {
                return GT_BAD_PARAM;
            }

            break;

        default:
            return GT_BAD_PARAM;
    }

    if(dsaInfoPtr->commonParams.vpt >= BIT_3)
    {
        return GT_BAD_PARAM;
    }

    /* bits 13:15 -- UP */
    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],13,3,dsaInfoPtr->commonParams.vpt);

    /* bit 12 - Extended
       (need to be set also for "TO_CPU" extended DSA !!!) */
    if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E)
    {
        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],12,1,1);
    }

    if(dsaInfoPtr->commonParams.vid >= BIT_12)
    {
        return GT_BAD_PARAM;
    }

    /* bits 0:11 -- VID */
    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],0,12,dsaInfoPtr->commonParams.vid);

    dsaBytesPtr[0] = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[0],24,8);
    dsaBytesPtr[1] = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[0],16,8);
    dsaBytesPtr[2] = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[0], 8,8);
    dsaBytesPtr[3] = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[0], 0,8);

    if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_TYPE_REGULAR_E)
    {
        return GT_OK;
    }
    else if(dsaInfoPtr->commonParams.dsaTagType != CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E)
    {
        return GT_BAD_PARAM;
    }

    /*************************/
    /* build the second word */
    /*************************/


    switch(dsaInfoPtr->dsaType)
    {
        case CPSS_DXCH_NET_DSA_CMD_TO_CPU_E:
            /* bit 31 = 0 -- Extend */

            /* bit 30 - CFI */
            if(dsaInfoPtr->commonParams.cfiBit == 1)
            {
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],30,1,1);
            }
            else if(dsaInfoPtr->commonParams.cfiBit > 1)
            {
                return GT_BAD_PARAM;
            }

            /* bit 27 -- OrigIsTrunk */
            if(toCpuPtr->srcIsTrunk == GT_TRUE)
            {
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],27,1,1);
            }

            /* bit 26 -- Truncated */
            if(toCpuPtr->wasTruncated == GT_TRUE)
            {
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],26,1,1);
            }

            /* bits 12:25 -- PktOrigBC */
            if(toCpuPtr->originByteCount >= BIT_14)
            {
                return GT_BAD_PARAM;
            }

            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],12,14,toCpuPtr->originByteCount);

            if(toCpuPtr->srcIsTrunk == GT_FALSE)
            {
                /* bit 11 -- reserved with value 0 */

                /* bits 10 -- SrcPort[5]/TrgPort[5] */
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],10,1,toCpuPtr->interface.portNum>>5);
            }
            else
            {
                /* bits 10:11 -- SrcTrunk[6:5] */
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],10,2,toCpuPtr->interface.srcTrunkId>>5);
            }

            /* bit 9 -- reserved with value 0 or                                     */
            /*          bit 0 of IPFIX timestamp (only for xCat A1 and above devices */
            if( PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
            {
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],9,1,toCpuPtr->timestamp);
            }

            /* bit 8 -- SrcTrg */
            if(toCpuPtr->isEgressPipe == GT_TRUE)
            {
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],8,1,1);
            }

            rc = prvCpssDxChNetIfCpuToDsaCode(toCpuPtr->cpuCode,&dsaCpuCode);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* bits 0:7-- LongCPUCode */
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],0,8,dsaCpuCode);

            /* check for Cheetah3 device */
            if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily >=
                CPSS_PP_FAMILY_CHEETAH3_E)
            {
                if (dsaInfoPtr->commonParams.dropOnSource == GT_TRUE)
                {
                    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],29,1,1);
                }

                if (dsaInfoPtr->commonParams.packetIsLooped == GT_TRUE)
                {
                    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],28,1,1);
                }
            }

            break;
        case CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E:
            /* bit 31 = 0 -- Extend */

            /* bit 30 -- EgressFilterEn */
            if(fromCpuPtr->egrFilterEn == GT_TRUE)
            {
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],30,1,1);
            }

            /* bit 29 -- cascadeControl */
            if(fromCpuPtr->cascadeControl == GT_TRUE)
            {
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],29,1,1);
            }

            /* bit 28 -- egrFilterRegistered */
            if(fromCpuPtr->egrFilterRegistered == GT_TRUE)
            {
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],28,1,1);
            }

            /* bit 27 -- TC[2] */
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],27,1,fromCpuPtr->tc>>2);

            /* check for Cheetah3 device */
            if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily >=
                CPSS_PP_FAMILY_CHEETAH3_E)
            {
                if (dsaInfoPtr->commonParams.dropOnSource == GT_TRUE)
                {
                    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],26,1,1);
                }

                if (dsaInfoPtr->commonParams.packetIsLooped == GT_TRUE)
                {
                    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],25,1,1);
                }
            }
            /* Not suppoted for Ch3 and above devices.
               To set DP, call cpssDxChNetIfFromCpuDpSet */
            else
            {
               /* cheetah support only those 2 types */
                switch (fromCpuPtr->dp)
                {
                case CPSS_DP_GREEN_E:
                    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],26,1,0);
                    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],25,1,0);
                    break;
                case CPSS_DP_RED_E:
                    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],26,1,0);
                    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],25,1,1);
                    break;
                default:
                    return GT_BAD_PARAM;
                }
            }

            if(fromCpuPtr->srcId >= BIT_5)
            {
                return GT_BAD_PARAM;
            }

            if(fromCpuPtr->srcDev >= BIT_5)
            {
                return GT_BAD_PARAM;
            }

            /* bits 20-24 -- Src-ID */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],20,5,fromCpuPtr->srcId);

            /* bits 15-19 -- SrcDev */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],15,5,fromCpuPtr->srcDev);

            /* bit 14 -- TC[1] */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],14,1,fromCpuPtr->tc>>1);

            if(useVidx == GT_TRUE)
            {
                /* bits 12:13 - VIDX[11:10] */
                if(fromCpuPtr->dstInterface.type == CPSS_INTERFACE_VIDX_E)
                {
                    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],12,2,fromCpuPtr->dstInterface.vidx>>10);
                }
                else /* vid */
                {
                    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],12,2,VID_INSTEAD_VIDX_CNS>>10);
                }

                if(fromCpuPtr->extDestInfo.multiDest.excludeInterface == GT_TRUE)
                {
                    if(fromCpuPtr->extDestInfo.multiDest.excludedInterface.type == CPSS_INTERFACE_TRUNK_E)
                    {
                        /* bit 11 - ExcludeIsTrunk */
                        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],11,1,1);

                        /* bit 10 - MirrorToAllCPUs */
                        if(fromCpuPtr->extDestInfo.multiDest.mirrorToAllCPUs == GT_TRUE)
                        {
                            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],10,1,1);
                        }

                        /* bits 7:9 -- reserved --> set to 0 */

                        if(fromCpuPtr->extDestInfo.multiDest.excludedInterface.trunkId >= BIT_7)
                        {
                            return GT_BAD_PARAM;
                        }
                        /* bits 0:6 -  ExcludedTrunk*/
                        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],0,7,
                            fromCpuPtr->extDestInfo.multiDest.excludedInterface.trunkId);

                    }
                    else if(fromCpuPtr->extDestInfo.multiDest.excludedInterface.type != CPSS_INTERFACE_PORT_E)
                    {
                        return GT_BAD_PARAM; /* must be port/trunk */
                    }
                    else /* must be port */
                    {
                        if(fromCpuPtr->extDestInfo.multiDest.excludedInterface.devPort.portNum >= BIT_6)
                        {
                            return GT_BAD_PARAM;
                        }
                        if(fromCpuPtr->extDestInfo.multiDest.excludedInterface.devPort.devNum >= BIT_5)
                        {
                            return GT_BAD_PARAM;
                        }
                        /* bits 5:10 -  ExcludedPort*/
                        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],5,6,
                            fromCpuPtr->extDestInfo.multiDest.excludedInterface.devPort.portNum);
                        /* bits 0:4 -  ExcludedDev*/
                        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],0,5,
                            fromCpuPtr->extDestInfo.multiDest.excludedInterface.devPort.devNum);
                    }
                }
                else
                {
                    /* bit 11 - ExcludeIsTrunk */
                    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],11,1,1);

                    /* bit 10 - MirrorToAllCPUs */
                    if(fromCpuPtr->extDestInfo.multiDest.mirrorToAllCPUs == GT_TRUE)
                    {
                        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],10,1,1);
                    }

                    /* bits 7:9 -- reserved --> set to 0 */

                    /* bits 0:6 -  ExcludedTrunk --> set to 0 */
                }
            }
            else if(fromCpuPtr->dstInterface.type == CPSS_INTERFACE_TRUNK_E)
            {
                return GT_BAD_PARAM; /* trunk not supported */
            }
            else /* not use vidx -- must not be trunk */
            {
                /* bit 13 - MailBoxToNeighborCPU */
                if(fromCpuPtr->extDestInfo.devPort.mailBoxToNeighborCPU == GT_TRUE)
                {
                    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],13,1,1);
                }

                /* bits 11:12 -- reserved with value 0 */

                /* bit 10 - TrgPort[5] */
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],10,1,
                    fromCpuPtr->dstInterface.devPort.portNum>>5);

                /* bits 0:9 -- reserved with value 0 */
            }


            break;
        case CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E:
            /* bit 31 = 0 -- Extend */

            /* bits 11:27 -- reserved with value 0 */

            /* bits 10 -- SrcPort[5]/TrgPort[5] */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],10,1,toAnalyzerPtr->devPort.portNum>>5);

            /* bits 0:9 -- reserved with value 0 */
            /* check for Cheetah3 device */
            if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily >=
                CPSS_PP_FAMILY_CHEETAH3_E)
            {
                if (dsaInfoPtr->commonParams.dropOnSource == GT_TRUE)
                {
                    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],29,1,1);
                }

                if (dsaInfoPtr->commonParams.packetIsLooped == GT_TRUE)
                {
                    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],28,1,1);
                }
            }

            break;
        case CPSS_DXCH_NET_DSA_CMD_FORWARD_E:

            /* bit 31 = 0 -- Extend */

            if(forwardPtr->srcIsTrunk == GT_FALSE)
            {
                /* bit 30 = 0 -- reserved */

                /* bit 29 */
                /* set use msb src port bit [5] */
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],
                                     29,/* 29 */
                                     1,   /*  1 */
                                     (forwardPtr->source.portNum >> 5));
            }
            else
            {
                /* bits 29:30*/
                /* set use msb src trunk bits [5:6] */
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],
                                     29,/* 29 */
                                     2,   /*  2 */
                                     (forwardPtr->source.trunkId >> 5));

            }

            /* bit 28 -- egrFilterRegistered */
            if(forwardPtr->egrFilterRegistered == GT_TRUE)
            {
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],28,1,1);
            }

            /* bit 25 -- Routed */
            if(forwardPtr->wasRouted == GT_TRUE)
            {
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],25,1,1);
            }


            if(forwardPtr->srcId >= BIT_5)
            {
                return GT_BAD_PARAM;
            }

            if(forwardPtr->qosProfileIndex >= BIT_7)
            {
                return GT_BAD_PARAM;
            }

            /* bits 20-24 -- Src-ID */
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],20,5,forwardPtr->srcId);

            /* bits 13-19 -- QoSProfile*/
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],13,7,forwardPtr->qosProfileIndex);

            if(forwardPtr->dstInterface.type == CPSS_INTERFACE_VIDX_E)
            {
                /* bit 12 -- use_vidx */
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],12,1,1);

                /* bits 0-11 -- VIDX[11:0]*/
                if(forwardPtr->dstInterface.vidx >= (BIT_12-1))
                {
                    return GT_BAD_PARAM;
                }

                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],0,12,forwardPtr->dstInterface.vidx);
            }
            else if(forwardPtr->dstInterface.type == CPSS_INTERFACE_VID_E)
            {
                /* bit 12 -- use_vidx */
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],12,1,1);

                /* bits 0-11 -- VIDX[11:0]*/
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],0,12,VID_INSTEAD_VIDX_CNS);
            }
            else if(forwardPtr->dstInterface.type == CPSS_INTERFACE_TRUNK_E)
            {
                return GT_BAD_PARAM; /* trunk is not supported */
            }
            else if(forwardPtr->dstInterface.type == CPSS_INTERFACE_PORT_E)/* port -- not support trunk */

            {
                /* bit 11 -- Reserved with value 0 */

                if(forwardPtr->dstInterface.devPort.devNum >= BIT_5)
                {
                    return GT_BAD_PARAM;
                }

                if(forwardPtr->dstInterface.devPort.portNum >= BIT_6)
                {
                    return GT_BAD_PARAM;
                }

                /* bits 5-10 -- TrgPort*/
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],5,6,
                                     forwardPtr->dstInterface.devPort.portNum);

                /* bits 0-4 -- TrgDev*/
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],0,5,
                                     forwardPtr->dstInterface.devPort.devNum);
            }
            else
            {
                return GT_BAD_PARAM;
            }

            /* check for Cheetah3 device */
            if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily >=
                CPSS_PP_FAMILY_CHEETAH3_E)
            {
                if (dsaInfoPtr->commonParams.dropOnSource == GT_TRUE)
                {
                    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],27,1,1);
                }

                if (dsaInfoPtr->commonParams.packetIsLooped == GT_TRUE)
                {
                    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],26,1,1);
                }
            }


            break;

        default:
            break;
    }

    dsaBytesPtr[4] = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[1],24,8);
    dsaBytesPtr[5] = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[1],16,8);
    dsaBytesPtr[6] = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[1], 8,8);
    dsaBytesPtr[7] = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[1], 0,8);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfDuplicateEnableSet
*
* DESCRIPTION:
*       Enable descriptor duplication (mirror, STC and ingress mirroring
*       to analyzer port when the analyzer port is the CPU), Or Disable any kind
*       of duplication.
*
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum - device number
*       enable -
*               GT_FALSE = Disable any kind of duplication
*
*               GT_TRUE = enable descriptor duplication (mirror, STC and ingress
*                         mirroring to analyzer port when the analyzer port is
*                         the CPU).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfDuplicateEnableSet
(
    IN  GT_U8                          devNum,
    IN  GT_BOOL                        enable
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    return prvCpssDrvHwPpSetRegField(devNum,
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.dupPktsToCpuConfReg,
                29,
                1,
                BOOL2BIT_MAC(enable));
}


/*******************************************************************************
* cpssDxChNetIfPortDuplicateToCpuSet
*
* DESCRIPTION:
*       set per ingress port if Packets received from the port that are
*       duplicated and their duplication target is the CPU, ( mirror, STC and
*       ingress mirroring to analyzer port when the analyzer port is the CPU)
*       are will be duplicated or just are only forwarded to their original
*       target.
*       NOTE : port may be "CPU port" (63)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum - device number
*       portNum - port number
*       enable -
*               GT_FALSE = Packets received from the port that are duplicated
*                          and their duplication target is the CPU, (mirror, STC
*                          and ingress mirroring to analyzer port when the
*                          analyzer port is the CPU) are not duplicated and are
*                          only forwarded to their original target.
*
*               GT_TRUE = Packets received from the port that are duplicated and
*                         their duplication target is the CPU, (mirror, STC and
*                         ingress mirroring to analyzer port when the analyzer
*                         port is the CPU) are duplicated and are only forwarded
*                         to their original target and to the CPU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfPortDuplicateToCpuSet
(
    IN  GT_U8                          devNum,
    IN  GT_U8                          portNum,
    IN  GT_BOOL                        enable
)
{
    GT_U32  bitNum;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    bitNum = (localPort == CPSS_CPU_PORT_NUM_CNS) ? 28 : localPort;

    return prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.dupPktsToCpuConfReg,
                bitNum,
                1,
                BOOL2BIT_MAC(enable));
}

/*******************************************************************************
* cpssDxChNetIfAppSpecificCpuCodeTcpSynSet
*
* DESCRIPTION:
*       Enable/Disable Application Specific CPU Code for TCP SYN packets
*       forwarded to the CPU - TCP_SYN_TO_CPU.
*       The feature enabled for packets if one of conditions are true:
*       1. MAC DA is found in the FDB and Application Specific CPU Code field
*           of FDB entry is True (see appCpuCodeEn in GT_MAC_ENTRY)
*       2. Packet passed Routing and Application Specific CPU Code field is
*           True in the Route entry (Next Hop)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum - Device number.
*       enable - GT_TRUE  - enable application specific CPU Code for TCP SYN
*                           packets.
*               GT_FALSE - disable application specific CPU Code for TCP SYN
*                           packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfAppSpecificCpuCodeTcpSynSet
(
    IN  GT_U8                          devNum,
    IN  GT_BOOL                        enable
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    return prvCpssDrvHwPpSetRegField(devNum,
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.preEgrEngineGlobal,
                12,
                1,
                BOOL2BIT_MAC(enable));
}

/*******************************************************************************
* cpssDxChNetIfAppSpecificCpuCodeTcpSynGet
*
* DESCRIPTION:
*       Get whether Application Specific CPU Code for TCP SYN packets forwarded
*       to the CPU - TCP_SYN_TO_CPU.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum - Device number.
*
* OUTPUTS:
*       enablePtr - (pointer to) is the feature enabled
*               GT_TRUE  - enabled ,application specific CPU Code for TCP SYN
*                           packets.
*               GT_FALSE - disabled ,application specific CPU Code for TCP SYN
*                           packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfAppSpecificCpuCodeTcpSynGet
(
    IN  GT_U8                          devNum,
    OUT GT_BOOL                        *enablePtr
)
{
    GT_STATUS   rc;
    GT_U32      data;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    rc = prvCpssDrvHwPpGetRegField(devNum,
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.preEgrEngineGlobal,
                12,
                1,
                &data);

    *enablePtr = BIT2BOOL_MAC(data);

    return rc;
}

/*******************************************************************************
* cpssDxChNetIfAppSpecificCpuCodeIpProtocolSet
*
* DESCRIPTION:
*       Set IP Protocol in the IP Protocol CPU Code Table  with specific
*       CPU Code. There are 8 IP Protocols may be defined.
*       The feature enabled for packets if one of conditions are true:
*       1. MAC DA is found in the FDB and Application Specific CPU Code field
*           of FDB entry is True (see appCpuCodeEn in GT_MAC_ENTRY)
*       2. Packet passed Routing and Application Specific CPU Code field is
*           True in the Route entry (Next Hop)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - Device number.
*       index     - the index of the IP Protocol (range 0..7)
*       protocol  - IP protocol (range 0..255)
*       cpuCode   - CPU Code for given IP protocol
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad index or bad cpuCode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfAppSpecificCpuCodeIpProtocolSet
(
    IN GT_U8            devNum,
    IN GT_U32           index,
    IN GT_U8            protocol,
    IN CPSS_NET_RX_CPU_CODE_ENT   cpuCode
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;      /* The register address to read from.           */
    GT_U32      regData;      /* Data read/write from/to register.            */
    GT_U32      ipRegIndex;   /* index of the register                        */
    GT_U32      bitOffset;    /* the bit offset inside register               */
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT      dsaCpuCode;  /* DSA Tag CPU Code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    if (index > DXCH2_CPU_CODE_IP_PROTOCOL_MAX_INDEX_CNS)
    {
        return GT_BAD_PARAM;
    }

    /* calculate the index of the IP Protocol CPU Code registers */
    ipRegIndex = index / 2;

    /* calculate the address of the IP Protocol CPU Code register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bufferMng.eqBlkCfgRegs.ipProtCpuCodeBase + (ipRegIndex * 4);

    /* calculate the bit offset */
    bitOffset = 16 * (index % 2);

    RX_OPCODE_TO_DSA_CONVERT_MAC(cpuCode,&dsaCpuCode);

    /* calculate register data */
    regData = ((dsaCpuCode << 8) | protocol);

    /* Set IP Protocol and CPU Code in the IP Protocol CPU Code register */
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, bitOffset, 16, regData);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* calculate the address of the IP Protocol CPU Code Valid register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bufferMng.eqBlkCfgRegs.ipProtCpuCodeValid;

    /* Set Valid bit in the IP Protocol CPU Code Valid register */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, index, 1, 1);
}

/*******************************************************************************
* cpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate
*
* DESCRIPTION:
*       Invalidate entry in the IP Protocol CPU Code Table .
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - Device number.
*       index     - the index of the IP Protocol (range 0..7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad index
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate
(
    IN GT_U8            devNum,
    IN GT_U32           index
)
{
    GT_U32      regAddr;      /* The register address to read from.           */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    if (index > DXCH2_CPU_CODE_IP_PROTOCOL_MAX_INDEX_CNS)
    {
        return GT_BAD_PARAM;
    }

    /* calculate the address of the IP Protocol CPU Code Valid register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bufferMng.eqBlkCfgRegs.ipProtCpuCodeValid;

    /* Set Valid bit in the IP Protocol CPU Code Valid register */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, index, 1, 0);
}

/*******************************************************************************
* cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet
*
* DESCRIPTION:
*       Get IP Protocol in the IP Protocol CPU Code Table  with specific
*       CPU Code.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - Device number.
*       index     - the index of the IP Protocol (range 0..7)
*
* OUTPUTS:
*       validPtr     - (pointer to)valid entry.
*                       GT_FALSE - the entry is not valid
*                       GT_TRUE - the entry is valid
*       protocolPtr  - (pointer to)IP protocol (range 0..255)
*       cpuCodePtr   - (pointer to)CPU Code for given IP protocol
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad index
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet
(
    IN GT_U8            devNum,
    IN GT_U32           index,
    OUT GT_BOOL         *validPtr,
    OUT GT_U8           *protocolPtr,
    OUT CPSS_NET_RX_CPU_CODE_ENT   *cpuCodePtr
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;      /* The register address to read from.           */
    GT_U32      regData;      /* Data read/write from/to register.            */
    GT_U32      ipRegIndex;   /* index of the register                        */
    GT_U32      bitOffset;    /* the bit offset inside register               */
    GT_U32      dsaCpuCode;  /* DSA Tag CPU Code                         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(validPtr);
    CPSS_NULL_PTR_CHECK_MAC(protocolPtr);
    CPSS_NULL_PTR_CHECK_MAC(cpuCodePtr);

    if (index > DXCH2_CPU_CODE_IP_PROTOCOL_MAX_INDEX_CNS)
    {
        return GT_BAD_PARAM;
    }

    /* calculate the index of the IP Protocol CPU Code registers */
    ipRegIndex = index / 2;

    /* calculate the address of the IP Protocol CPU Code register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bufferMng.eqBlkCfgRegs.ipProtCpuCodeBase + (ipRegIndex * 4);

    /* calculate the bit offset */
    bitOffset = 16 * (index % 2);

    /* Get IP Protocol and CPU Code in the IP Protocol CPU Code register */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, bitOffset, 16, &regData);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* calculate register data */
    *protocolPtr = (GT_U8)U32_GET_FIELD_MAC(regData,0,8);
    dsaCpuCode  =        U32_GET_FIELD_MAC(regData,8,8);

    DSA_TO_RX_OPCODE_CONVERT_MAC(dsaCpuCode,cpuCodePtr);

    /* calculate the address of the IP Protocol CPU Code Valid register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bufferMng.eqBlkCfgRegs.ipProtCpuCodeValid;

    /* Set Valid bit in the IP Protocol CPU Code Valid register */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, index, 1, &regData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *validPtr = BIT2BOOL_MAC(regData);

    return GT_OK;
}


/*******************************************************************************
* cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet
*
* DESCRIPTION:
*       Set range to TCP/UPD Destination Port Range CPU Code Table with specific
*       CPU Code. There are 16 ranges may be defined.
*       The feature enabled for packets if one of conditions are true:
*        1. MAC DA is found in the FDB and Application Specific CPU Code field
*           of FDB entry is True (see appCpuCodeEn in GT_MAC_ENTRY)
*        2. Packet passed Routing and Application Specific CPU Code field is
*           True in the Route entry (Next Hop)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum     - Device number.
*       rangeIndex - the index of the range (range 0..15)
*       maxDstPort - the maximum destination port in this range.
*       minDstPort - the minimum destination port in this range
*       packetType - packet type (Unicast/Multicast)
*       protocol   - protocol type (UDP/TCP)
*       cpuCode    - CPU Code Index for this TCP/UDP range
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad rangeIndex or
*                          bad packetType or bad protocol or bad cpuCode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet
(
    IN GT_U8                            devNum,
    IN GT_U32                           rangeIndex,
    IN GT_U16                           minDstPort,
    IN GT_U16                           maxDstPort,
    IN CPSS_NET_TCP_UDP_PACKET_TYPE_ENT packetType,
    IN CPSS_NET_PROT_ENT                protocol,
    IN CPSS_NET_RX_CPU_CODE_ENT         cpuCode
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;      /* The register address to read from.           */
    GT_U32      regData;      /* Data read/write from/to register.            */
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT      dsaCpuCode;  /* DSA Tag CPU Code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    if(rangeIndex > DXCH2_CPU_CODE_L4_DEST_PORT_RANGE_MAX_INDEX_CNS)
    {
        return GT_BAD_PARAM;
    }

    /* Address of the TCP/UDP Dest Port Range CPU Code word 0 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
        eqBlkCfgRegs.tcpUdpDstPortRangeCpuCodeWord0Base + (rangeIndex * 4);

    /* calculate register data */
    regData = ((maxDstPort << 16) | minDstPort);

    /* write Port Range to the TCP/UDP Dest CPU Code word 0 */
    rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, regData);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Address of the TCP/UDP Dest Port Range CPU Code word 1 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
        eqBlkCfgRegs.tcpUdpDstPortRangeCpuCodeWord1Base + (rangeIndex * 4);

    regData = 0;

    /* calculate register data */
    switch (packetType)
    {
        case CPSS_NET_TCP_UDP_PACKET_UC_E:
            U32_SET_FIELD_NO_CLEAR_MAC(regData,10,2,0);
            break;
        case CPSS_NET_TCP_UDP_PACKET_MC_E:
            U32_SET_FIELD_NO_CLEAR_MAC(regData,10,2,1);
            break;
        case CPSS_NET_TCP_UDP_PACKET_BOTH_UC_MC_E:
            U32_SET_FIELD_NO_CLEAR_MAC(regData,10,2,2);
            break;
        default:
            return GT_BAD_PARAM;
    }

    switch (protocol)
    {
        case CPSS_NET_PROT_UDP_E:
            U32_SET_FIELD_NO_CLEAR_MAC(regData,8,2,1);
            break;
        case CPSS_NET_PROT_TCP_E:
            U32_SET_FIELD_NO_CLEAR_MAC(regData,8,2,2);
            break;
        case CPSS_NET_PROT_BOTH_UDP_TCP_E:
            U32_SET_FIELD_NO_CLEAR_MAC(regData,8,2,3);
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* translate cpu opcode provided by application to HW cpu opcode */
    RX_OPCODE_TO_DSA_CONVERT_MAC(cpuCode,&dsaCpuCode);

    U32_SET_FIELD_NO_CLEAR_MAC(regData,0,8,dsaCpuCode);

    /* set TCP/UDP Port range parameters in the
       TCP/UDP Port range CPU Code Entry word 1*/
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 12, regData);
}

/*******************************************************************************
* cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate
*
* DESCRIPTION:
*       invalidate range to TCP/UPD Destination Port Range CPU Code Table with
*       specific CPU Code. There are 16 ranges may be defined.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum     - Device number.
*       rangeIndex - the index of the range (range 0..15)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad rangeIndex
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate
(
    IN GT_U8                            devNum,
    IN GT_U32                           rangeIndex
)
{
    GT_U32      regAddr;      /* The register address to read from.        */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    if(rangeIndex > DXCH2_CPU_CODE_L4_DEST_PORT_RANGE_MAX_INDEX_CNS)
    {
        return GT_BAD_PARAM;
    }

    /* Address of the TCP/UDP Dest Port Range CPU Code word 1 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
        eqBlkCfgRegs.tcpUdpDstPortRangeCpuCodeWord1Base + (rangeIndex * 4);

    /* set the 2 bits to value 0  --> no valid entry */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 8, 2, 0);
}


/*******************************************************************************
* cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet
*
* DESCRIPTION:
*       Get range for TCP/UPD Destination Port Range CPU Code Table with specific
*       CPU Code. There are 16 ranges may be defined.
*       The feature enabled for packets if one of conditions are true:
*        1. MAC DA is found in the FDB and Application Specific CPU Code field
*           of FDB entry is True (see appCpuCodeEn in GT_MAC_ENTRY)
*        2. Packet passed Routing and Application Specific CPU Code field is
*           True in the Route entry (Next Hop)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum     - Device number.
*       rangeIndex - the index of the range (range 0..15)
*
* OUTPUTS:
*       validPtr  - (pointer to) is the entry valid
*                   GT_FALSE - range is't valid
*                   GT_TRUE  - range is valid
*       maxDstPortPtr - (pointer to)the maximum destination port in this range.
*       minDstPortPtr - (pointer to)the minimum destination port in this range
*       packetTypePtr - (pointer to)packet type (Unicast/Multicast)
*       protocolPtr- (pointer to)protocol type (UDP/TCP)
*                    NOTE : this field will hold valid value only when
*                           (*validPtr) = GT_TRUE
*                           because in HW the "valid" is one of the protocol
*                           options
*       cpuCodePtr - (pointer to)CPU Code Index for this TCP/UDP range
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad rangeIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet
(
    IN GT_U8                             devNum,
    IN GT_U32                            rangeIndex,
    OUT GT_BOOL                          *validPtr,
    OUT GT_U16                           *minDstPortPtr,
    OUT GT_U16                           *maxDstPortPtr,
    OUT CPSS_NET_TCP_UDP_PACKET_TYPE_ENT *packetTypePtr,
    OUT CPSS_NET_PROT_ENT                *protocolPtr,
    OUT CPSS_NET_RX_CPU_CODE_ENT         *cpuCodePtr
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;      /* The register address to read from.           */
    GT_U32      regData;      /* Data read/write from/to register.            */
    GT_U32      dsaCpuCode;  /* DSA Tag CPU Code                         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(validPtr);
    CPSS_NULL_PTR_CHECK_MAC(minDstPortPtr);
    CPSS_NULL_PTR_CHECK_MAC(maxDstPortPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(protocolPtr);
    CPSS_NULL_PTR_CHECK_MAC(cpuCodePtr);

    if(rangeIndex > DXCH2_CPU_CODE_L4_DEST_PORT_RANGE_MAX_INDEX_CNS)
    {
        return GT_BAD_PARAM;
    }

    /* Address of the TCP/UDP Dest Port Range CPU Code word 0 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
        eqBlkCfgRegs.tcpUdpDstPortRangeCpuCodeWord0Base + (rangeIndex * 4);

    /* write Port Range to the TCP/UDP Dest CPU Code word 0 */
    rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &regData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *minDstPortPtr = (GT_U16)U32_GET_FIELD_MAC(regData, 0,16);
    *maxDstPortPtr = (GT_U16)U32_GET_FIELD_MAC(regData,16,16);


    /* Address of the TCP/UDP Dest Port Range CPU Code word 1 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
        eqBlkCfgRegs.tcpUdpDstPortRangeCpuCodeWord1Base + (rangeIndex * 4);

    /* set TCP/UDP Port range parameters in the
       TCP/UDP Port range CPU Code Entry word 1*/
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 12, &regData);
    if (rc != GT_OK)
    {
        return rc;
    }

    dsaCpuCode = U32_GET_FIELD_MAC(regData,0,8);
    DSA_TO_RX_OPCODE_CONVERT_MAC(dsaCpuCode,cpuCodePtr);

    /* calculate register data */
    switch (U32_GET_FIELD_MAC(regData,10,2))
    {
        case 0:
            *packetTypePtr = CPSS_NET_TCP_UDP_PACKET_UC_E;
            break;
        case 1:
            *packetTypePtr = CPSS_NET_TCP_UDP_PACKET_MC_E;
            break;
        case 2:
        default:
            *packetTypePtr = CPSS_NET_TCP_UDP_PACKET_BOTH_UC_MC_E;
            break;
    }

    *validPtr = GT_TRUE;

    switch (U32_GET_FIELD_MAC(regData,8,2))
    {
        case 0:
            *validPtr = GT_FALSE;
            *protocolPtr = CPSS_NET_PROT_UDP_E;/* dummy !!! */
            break;
        case 1:
            *protocolPtr = CPSS_NET_PROT_UDP_E;
            break;
        case 2:
            *protocolPtr = CPSS_NET_PROT_TCP_E;
            break;
        case 3:
        default:
            *protocolPtr = CPSS_NET_PROT_BOTH_UDP_TCP_E;
            break;
    }


    return GT_OK;

}

/*******************************************************************************
* cpssDxChNetIfCpuCodeIpLinkLocalProtSet
*
* DESCRIPTION:
*       Configure CPU code for IPv4 and IPv6 Link Local multicast packets
*       with destination IP in range of 224.0.0.0 ... 224.0.0.255
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - Device number.
*       ipVer    - IP version: Ipv4 or Ipv6
*       protocol - Specifies the LSB of IP Link Local multicast protocol
*       cpuCode  - The CPU Code
*           The 4 CPU acceptable CPU Codes are:
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad ipVer or bad cpuCode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeIpLinkLocalProtSet
(
    IN GT_U8                devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT    ipVer,
    IN GT_U8                protocol,
    IN CPSS_NET_RX_CPU_CODE_ENT  cpuCode
)
{
    GT_U32      regAddr;      /* The register address to read from.           */
    GT_U32      regIndex;     /* Register index                               */
    GT_U32      regData;      /* Data read/write from/to register.            */
    GT_U32      bitOffset;    /* The bit offset inside the register           */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    /* check cpuCode validity */
    switch (cpuCode)
    {
        case CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E:
            regData = 0;
            break;
        case CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E:
            regData = 1;
            break;
        case CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E:
            regData = 2;
            break;
        case CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E:
            regData = 3;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* calculate the base address of the register */
    switch (ipVer)
    {
        case CPSS_IP_PROTOCOL_IPV4_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                bufferMng.eqBlkCfgRegs.ipv4McstLinkLocalCpuCodeIndexBase;
            break;
        case CPSS_IP_PROTOCOL_IPV6_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                bufferMng.eqBlkCfgRegs.ipv6McstLinkLocalCpuCodeIndexBase;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* calculate register index and bit offset according to the protocol */
    regIndex = (protocol / 16);
    bitOffset = (2 * (protocol % 16));

    /* Address of the IPv4 or IPv6 Multicast CPU Code register */
    regAddr += (regIndex * 0x1000);

    /* write IP Multicast CPU Code index to one of the
      IPv4 or IPv6 Multicast Link Local CPU Code Index registers */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, bitOffset, 2, regData);
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeIpLinkLocalProtGet
*
* DESCRIPTION:
*       get the Configuration CPU code for IPv4 and IPv6 Link Local multicast
*       packets with destination IP in range of 224.0.0.0 ... 224.0.0.255
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - Device number.
*       ipVer    - IP version: Ipv4 or Ipv6
*       protocol - Specifies the LSB of IP Link Local multicast protocol
*
* OUTPUTS:
*       cpuCodePtr  - (pointer to) The CPU Code
*           The 4 CPU acceptable CPU Codes are:
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad ipVer
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeIpLinkLocalProtGet
(
    IN GT_U8                devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT    ipVer,
    IN GT_U8                protocol,
    OUT CPSS_NET_RX_CPU_CODE_ENT  *cpuCodePtr
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;      /* The register address to read from.           */
    GT_U32      regIndex;     /* Register index                               */
    GT_U32      regData;      /* Data read/write from/to register.            */
    GT_U32      bitOffset;    /* The bit offset inside the register           */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(cpuCodePtr);

    /* calculate register index and bit offset according to the protocol */
    regIndex = (protocol / 16);
    bitOffset = (2 * (protocol % 16));

    /* calculate the base address of the register */
    switch (ipVer)
    {
        case CPSS_IP_PROTOCOL_IPV4_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                bufferMng.eqBlkCfgRegs.ipv4McstLinkLocalCpuCodeIndexBase;
            break;
        case CPSS_IP_PROTOCOL_IPV6_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                bufferMng.eqBlkCfgRegs.ipv6McstLinkLocalCpuCodeIndexBase;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* Address of the IPv4 or IPv6 Multicast CPU Code register */
    regAddr += (regIndex * 0x1000);

    /* write IP Multicast CPU Code index to one of the
      IPv4 or IPv6 Multicast Link Local CPU Code Index registers */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, bitOffset, 2, &regData);
    if(rc != GT_OK)
    {
        return rc;
    }

    switch (regData)
    {
        case 0:
            *cpuCodePtr = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;
            break;
        case 1:
            *cpuCodePtr = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E;
            break;
        case 2:
            *cpuCodePtr = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E;
            break;
        case 3:
        default:
            *cpuCodePtr = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E;
            break;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeIeeeReservedMcastProtSet
*
* DESCRIPTION:
*       Configure CPU code for particular IEEE reserved mcast protocol
*       (01-80-C2-00-00-00  ... 01-80-C2-00-00-FF)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - Device number.
*       protocol - Specifies the LSB of IEEE Multicast protocol
*       cpuCode  - The CPU Code
*           The 4 CPU acceptable CPU Codes are:
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad cpuCode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeIeeeReservedMcastProtSet
(
    IN GT_U8            devNum,
    IN GT_U8            protocol,
    IN CPSS_NET_RX_CPU_CODE_ENT   cpuCode
)
{
    GT_U32      regAddr;      /* The register address to read from.           */
    GT_U32      regIndex;     /* Register index                               */
    GT_U32      regData;      /* Data read/write from/to register.            */
    GT_U32      bitOffset;    /* The bit offset inside the register           */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    /* check validity of cpuCode */
    switch (cpuCode)
    {
        case CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E:
            regData = 0;
            break;
        case CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E:
            regData = 1;
            break;
        case CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E:
            regData = 2;
            break;
        case CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E:
            regData = 3;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* calculate register index and bit offset according to protocol */
    regIndex = protocol / 16;
    bitOffset = (2 * (protocol % 16));

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
            eqBlkCfgRegs.ieeeResMcstCpuCodeIndexBase + (regIndex * 0x1000);

    /* Set IEEE CPU Code Index for specific protocol */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, bitOffset, 2, regData);
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet
*
* DESCRIPTION:
*       get the Configuration CPU code for particular IEEE reserved mcast protocol
*       (01-80-C2-00-00-00  ... 01-80-C2-00-00-FF)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - Device number.
*       protocol - Specifies the LSB of IEEE Multicast protocol
*
* OUTPUTS:
*       cpuCodePtr  - (pointer to)The CPU Code
*           The 4 CPU acceptable CPU Codes are:
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet
(
    IN GT_U8            devNum,
    IN GT_U8            protocol,
    OUT CPSS_NET_RX_CPU_CODE_ENT   *cpuCodePtr
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;      /* The register address to read from.           */
    GT_U32      regIndex;     /* Register index                               */
    GT_U32      regData;      /* Data read/write from/to register.            */
    GT_U32      bitOffset;    /* The bit offset inside the register           */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(cpuCodePtr);

    /* calculate register index and bit offset according to protocol */
    regIndex = protocol / 16;
    bitOffset = (2 * (protocol % 16));

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
            eqBlkCfgRegs.ieeeResMcstCpuCodeIndexBase + (regIndex * 0x1000);

    /* Set IEEE CPU Code Index for specific protocol */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, bitOffset, 2, &regData);
    if(rc != GT_OK)
    {
        return rc;
    }

    switch (regData)
    {
        case 0:
            *cpuCodePtr = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E;
            break;
        case 1:
            *cpuCodePtr = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E;
            break;
        case 2:
            *cpuCodePtr = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E;
            break;
        case 3:
        default:
            *cpuCodePtr = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E;
            break;
    }

    return GT_OK;
}


/*******************************************************************************
* multiPortGroupDebugNetIfCpuCodeTableSet
*
* DESCRIPTION:
*       Function to set the "CPU code table" entry, for a specific CPU code.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum  - device number
*       cpuCode - the CPU code (key parameter to access the "CPU code table").
*                 use CPSS_NET_ALL_CPU_OPCODES_E as a wildcard to be used when
*                 need to set all the table entries with the same info.
*       dsaCpuCode - the index in HW in the table (converted from cpuCode)
*       hwDataPtr - pointer to the HW format entry
*
* OUTPUTS:
*       wasSetPtr - (pointer to) the function already set the needed CPU code entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad cpuCode or bad one of
*                          bad one entryInfoPtr parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS multiPortGroupDebugNetIfCpuCodeTableSet
(
    IN GT_U8                    devNum,
    IN CPSS_NET_RX_CPU_CODE_ENT cpuCode,
    IN PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT      dsaCpuCode,
    IN GT_U32   *hwDataPtr,
    OUT GT_BOOL *wasSetPtr
)
{
    GT_STATUS   rc;
    GT_U32  ii;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32    hwDataNotLastPortGroup[1];/* HW value for the port groups that get the debug value */

    *wasSetPtr = GT_FALSE;
    hwDataNotLastPortGroup[0] = hwDataPtr[0];

    for(ii = 0; ii < PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->numOfCpuCodes ; ii++)
    {
        if(PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->unknownUnregCpuCodesPtr[ii] == cpuCode)
        {
            /* this cpu code should be handled differently then others */
            break;
        }
    }

    if(ii == PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->numOfCpuCodes)
    {
        return GT_OK;
    }

    *wasSetPtr = GT_TRUE;

    /* use the debug device index , instead the one from the caller */
    U32_SET_FIELD_MASKED_MAC(hwDataNotLastPortGroup[0],11,3,
        PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->unknownUnregDesignatedDeviceIndex);

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        if((1 << portGroupId) &
            PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->lastPortGroupBmp)
        {
            /* set the 'last port group' with the value that set by the caller */
            rc = prvCpssDxChPortGroupWriteTableEntry(devNum,portGroupId,
                                                PRV_CPSS_DXCH_TABLE_CPU_CODE_E,
                                                (GT_U32)dsaCpuCode,/* DSA CPU code */
                                                 hwDataPtr);
        }
        else
        {
            rc = prvCpssDxChPortGroupWriteTableEntry(devNum,portGroupId,
                                            PRV_CPSS_DXCH_TABLE_CPU_CODE_E,
                                            (GT_U32)dsaCpuCode,/* DSA CPU code */
                                            &(hwDataNotLastPortGroup[0]));
        }

        if(rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeTableSet
*
* DESCRIPTION:
*       Function to set the "CPU code table" entry, for a specific CPU code.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       cpuCode - the CPU code (key parameter to access the "CPU code table").
*                 use CPSS_NET_ALL_CPU_OPCODES_E as a wildcard to be used when
*                 need to set all the table entries with the same info.
*       entryInfoPtr - (pointer to) The entry information
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad cpuCode or bad one of
*                          bad one entryInfoPtr parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeTableSet
(
    IN GT_U8                    devNum,
    IN CPSS_NET_RX_CPU_CODE_ENT cpuCode,
    IN CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC *entryInfoPtr
)
{
    GT_STATUS   rc;
    GT_U32  ii;
    GT_U32  hwData[1] = {0};
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT      dsaCpuCode=0;  /* DSA Tag CPU Code */
    GT_BOOL wasSet;/* entry was set and no more treatment needed */
    GT_U32      dp;   /* drop precedence HW value                */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(entryInfoPtr);
    if(cpuCode != CPSS_NET_ALL_CPU_OPCODES_E)
    {
        RX_OPCODE_TO_DSA_CONVERT_MAC(cpuCode,&dsaCpuCode);
    }
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(entryInfoPtr->tc);
    PRV_CPSS_DXCH_COS_DP_TO_HW_CHECK_AND_CONVERT_MAC(
        devNum, entryInfoPtr->dp, dp);

    if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* for CH2 and above */
        CPU_CODE_RATE_LIMITER_INDEX_CHECK_MAC(entryInfoPtr->cpuCodeRateLimiterIndex);

        U32_SET_FIELD_NO_CLEAR_MAC(hwData[0],14,5,entryInfoPtr->cpuCodeRateLimiterIndex);
    }
    CPU_CODE_STATISTICAL_RATE_LIMITER_INDEX_CHECK_MAC(entryInfoPtr->cpuCodeStatRateLimitIndex);
    CPU_CODE_DESIGNATED_DEVICE_INDEX_CHECK_MAC(entryInfoPtr->designatedDevNumIndex);

    switch(entryInfoPtr->cpuRateLimitMode)
    {
        case CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E:
            break;
        case CPSS_NET_CPU_CODE_RATE_LIMIT_AGGREGATE_E:
            U32_SET_FIELD_NO_CLEAR_MAC(hwData[0],19,1,1);
            break;
        default:
            return GT_BAD_PARAM;
    }

    U32_SET_FIELD_NO_CLEAR_MAC(hwData[0],0,3,entryInfoPtr->tc);

    U32_SET_FIELD_NO_CLEAR_MAC(hwData[0],3,2,dp);

    if(entryInfoPtr->truncate == GT_TRUE)
    {
        U32_SET_FIELD_NO_CLEAR_MAC(hwData[0],5,1,1);
    }

    U32_SET_FIELD_NO_CLEAR_MAC(hwData[0], 6,5,entryInfoPtr->cpuCodeStatRateLimitIndex);
    U32_SET_FIELD_NO_CLEAR_MAC(hwData[0],11,3,entryInfoPtr->designatedDevNumIndex);

    if(cpuCode != CPSS_NET_ALL_CPU_OPCODES_E) /* specific entry */
    {
        /* write CPU code entry */
        if(PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->debugInfoValid == GT_TRUE)
        {
            rc = multiPortGroupDebugNetIfCpuCodeTableSet(devNum,cpuCode,dsaCpuCode,hwData,&wasSet);
            if(rc != GT_OK)
            {
                return rc;
            }

            if(wasSet == GT_TRUE)
            {
                return GT_OK;
            }
        }

        rc = prvCpssDxChWriteTableEntry(devNum,
                                        PRV_CPSS_DXCH_TABLE_CPU_CODE_E,
                                        (GT_U32)dsaCpuCode,/* DSA CPU code */
                                        hwData);
        if (rc != GT_OK)
        {
            return rc;
        }
        if ( PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
        {
            rc = cpssExtDrvEthCpuCodeToQueue(dsaCpuCode,entryInfoPtr->tc);
            /* ignore GT_NOT_SUPPORTED, GT_NOT_IMPLEMENTED ...*/
            if (rc == GT_FAIL)
            {
                return rc;
            }
        }
        return GT_OK;
    }
    else
    {
        for(ii = 0 ; ii < 256 ; ii++)
        {
            /* write CPU code entry */
            if(PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->debugInfoValid == GT_TRUE)
            {
                rc = multiPortGroupDebugNetIfCpuCodeTableSet(devNum,cpuCode,ii,hwData,&wasSet);
                if(rc != GT_OK)
                {
                    return rc;
                }

                if(wasSet == GT_TRUE)
                {
                    continue;
                }
            }

            /* write the full table */
            rc = prvCpssDxChWriteTableEntry(devNum,
                                            PRV_CPSS_DXCH_TABLE_CPU_CODE_E,
                                            ii,/* DSA CPU code */
                                            hwData);
            if(rc != GT_OK)
            {
                return rc;
            }
            if ( PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
            {
                rc = cpssExtDrvEthCpuCodeToQueue(ii,entryInfoPtr->tc);
                /* ignore GT_NOT_SUPPORTED, GT_NOT_IMPLEMENTED ...*/
                if (rc == GT_FAIL)
                {
                    return rc;
                }
            }
        }

        return GT_OK;
    }
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeTableGet
*
* DESCRIPTION:
*       Function to get the "CPU code table" entry, for a specific CPU code.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       cpuCode - the CPU code (key parameter to access the "CPU code table").
*                 use CPSS_NET_ALL_CPU_OPCODES_E as a wildcard to be used when
*                 need to set all the table entries with the same info.
*
* OUTPUTS:
*       entryInfoPtr - (pointer to) The entry information
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad cpuCode
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeTableGet
(
    IN  GT_U8                    devNum,
    IN  CPSS_NET_RX_CPU_CODE_ENT cpuCode,
    OUT CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC *entryInfoPtr
)
{
    GT_STATUS   rc;
    GT_U32      hwDataArr[1]= {0}; /* data in the HW format */
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT      dsaCpuCode=0;  /* DSA Tag CPU Code */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32  hwDp;       /* HW DP value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(entryInfoPtr);
    RX_OPCODE_TO_DSA_CONVERT_MAC(cpuCode,&dsaCpuCode);

    if(PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->debugInfoValid == GT_TRUE)
    {
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,
            PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->lastPortGroupBmp,
            portGroupId)
        {
            /* need to read the info from a specific port group */
            /* read CPU code entry */
            rc = prvCpssDxChPortGroupReadTableEntry(devNum,
                                            portGroupId,
                                            PRV_CPSS_DXCH_TABLE_CPU_CODE_E,
                                            (GT_U32)dsaCpuCode,/* DSA CPU code */
                                            hwDataArr);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* read only from the first port group in BMP */
            break;
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,
            PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->lastPortGroupBmp,
            portGroupId)
    }
    else
    {
        /* read CPU code entry */
        rc = prvCpssDxChReadTableEntry(devNum,
                                        PRV_CPSS_DXCH_TABLE_CPU_CODE_E,
                                        (GT_U32)dsaCpuCode,/* DSA CPU code */
                                        hwDataArr);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    entryInfoPtr->tc = (GT_U8)U32_GET_FIELD_MAC(hwDataArr[0],0,3);

    hwDp = U32_GET_FIELD_MAC(hwDataArr[0],3,2);
    PRV_CPSS_DXCH_COS_DP_TO_SW_CHECK_AND_CONVERT_MAC(
        devNum, hwDp, entryInfoPtr->dp);

    entryInfoPtr->truncate = U32_GET_FIELD_MAC(hwDataArr[0],5,1) ? GT_TRUE : GT_FALSE;

    entryInfoPtr->cpuCodeStatRateLimitIndex = U32_GET_FIELD_MAC(hwDataArr[0], 6,5);
    entryInfoPtr->designatedDevNumIndex     = U32_GET_FIELD_MAC(hwDataArr[0],11,3);
    entryInfoPtr->cpuCodeRateLimiterIndex   = U32_GET_FIELD_MAC(hwDataArr[0],14,5);

    entryInfoPtr->cpuRateLimitMode = U32_GET_FIELD_MAC(hwDataArr[0],19,1) ?
            CPSS_NET_CPU_CODE_RATE_LIMIT_AGGREGATE_E :
            CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E ;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet
*
* DESCRIPTION:
*       Function to set the "Statistical Rate Limits Table" entry.
*       The cpssDxChNetIfCpuCodeTableSet(...) points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       index - index into the Statistical Rate Limits Table (range 0..31)
*       statisticalRateLimit - The statistical rate limit compared to the
*                              32-bit pseudo-random generator (PRNG).
*           NOTE:
*           For all packets to be forwarded to the CPU, set this field to
*           0xFFFFFFFF.
*           For all packets to be dropped, set this field to 0x0.
*
*           This mechanism is based on a hardware-based 32-bit pseudo-random
*           generator (PRNG). The PRNG generates evenly-distributed 32-bit
*           numbers ranging from 0 through 0xFFFFFFFF.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad index
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet
(
    IN GT_U8        devNum,
    IN GT_U32       index,
    IN GT_U32       statisticalRateLimit
)
{
    GT_U32  hwData[1];

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPU_CODE_STATISTICAL_RATE_LIMITER_INDEX_AS_POINTER_CHECK_MAC(index);

    hwData[0] = statisticalRateLimit;

    /* write statistical rate limit entry */
    return prvCpssDxChWriteTableEntry(devNum,
                                    PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E,
                                    index,/* index */
                                    hwData);
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet
*
* DESCRIPTION:
*       Function to get the "Statistical Rate Limits Table" entry.
*       The cpssDxChNetIfCpuCodeTableSet(...) points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       index - index into the Statistical Rate Limits Table (range 0..31)
*
* OUTPUTS:
*       statisticalRateLimitPtr - (pointer to)The statistical rate limit
*             compared to the 32-bit pseudo-random generator (PRNG).
*
*           NOTE:
*           For all packets to be forwarded to the CPU, set this field to
*           0xFFFFFFFF.
*           For all packets to be dropped, set this field to 0x0.
*
*           This mechanism is based on a hardware-based 32-bit pseudo-random
*           generator (PRNG). The PRNG generates evenly-distributed 32-bit
*           numbers ranging from 0 through 0xFFFFFFFF.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad index
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet
(
    IN  GT_U8       devNum,
    IN  GT_U32      index,
    OUT GT_U32      *statisticalRateLimitPtr
)
{
    GT_STATUS   rc;
    GT_U32  hwData;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPU_CODE_STATISTICAL_RATE_LIMITER_INDEX_AS_POINTER_CHECK_MAC(index);
    CPSS_NULL_PTR_CHECK_MAC(statisticalRateLimitPtr);

    /* read statistical rate limit entry */
    rc = prvCpssDxChReadTableEntry(devNum,
                                    PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E,
                                    index,/* index */
                                    &hwData);

    *statisticalRateLimitPtr = hwData;

    return rc;
}

/*******************************************************************************
* multiPortGroupDebugNetIfCpuCodeDesignatedDeviceTableSet
*
* DESCRIPTION:
*       Function to set the "designated device Table" entry.
*       The cpssDxChNetIfCpuCodeTableSet(...) points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum  - device number
*       index - index into the designated device table (range 1..7)
*       startBit - the start bit of the hwData
*       hwData - HW Data to set to the 'last port group'
*
* OUTPUTS:
*       wasSetPtr - (pointer to) the function already set the needed index entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad index or bad
*                          designatedDevNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS multiPortGroupDebugNetIfCpuCodeDesignatedDeviceTableSet
(
    IN GT_U8        devNum,
    IN GT_U32       index,
    IN GT_U32       startBit,
    IN GT_U32       hwData,
    OUT GT_BOOL     *wasSetPtr
)
{
    GT_STATUS   rc;
    GT_U32    portGroupId = 0;/*the port group Id - support multi-port-groups device */
    GT_U32    hwDataNotLastPortGroup;/* HW value for the port groups that get the debug value */

    *wasSetPtr = GT_FALSE;

    if(index != PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->unknownUnregDesignatedDeviceIndex)
    {
        return GT_OK;
    }

    *wasSetPtr = GT_TRUE;

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        if((1 << portGroupId) &
            PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->lastPortGroupBmp)
        {
            /* set the 'last port group' with the value that set by the caller */
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,
                        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.cpuTargDevConfReg0 +
                            (((index -1) / 4) * 4),
                            startBit,
                            5,
                            hwData);
        }
        else
        {
            hwDataNotLastPortGroup = PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->
                                    unknownUnregDesignatedDeviceArray[portGroupId];

            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.cpuTargDevConfReg0 +
                        (((index -1) / 4) * 4),
                        startBit,
                        5,
                        hwDataNotLastPortGroup);
        }

        if(rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeDesignatedDeviceTableSet
*
* DESCRIPTION:
*       Function to set the "designated device Table" entry.
*       The cpssDxChNetIfCpuCodeTableSet(...) points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       index - index into the designated device table (range 1..7)
*       designatedDevNum - The designated device number (range 0..31)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad index or bad
*                          designatedDevNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeDesignatedDeviceTableSet
(
    IN GT_U8        devNum,
    IN GT_U32       index,
    IN GT_U8        designatedDevNum
)
{
    GT_STATUS   rc;
    GT_U32  hwData;
    GT_U32  startBit;
    GT_BOOL wasSet;/* entry was set and no more treatment needed */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPU_CODE_DESIGNATED_DEVICE_INDEX_AS_POINTER_CHECK_MAC(index);
    if(designatedDevNum >= BIT_5)
    {
        /* only 5 bits in HW */
        return GT_BAD_PARAM;
    }

    hwData = designatedDevNum;
    startBit = ((index - 1) % 4) * 5;

    if(PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->debugInfoValid == GT_TRUE)
    {
        rc = multiPortGroupDebugNetIfCpuCodeDesignatedDeviceTableSet(devNum,index,startBit,hwData,&wasSet);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(wasSet == GT_TRUE)
        {
            return GT_OK;
        }
    }

    /* write designated device entry */
    return prvCpssDrvHwPpSetRegField(devNum,
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.cpuTargDevConfReg0 +
            (((index -1) / 4) * 4),
            startBit,
            5,
            hwData);
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeDesignatedDeviceTableGet
*
* DESCRIPTION:
*       Function to get the "designated device Table" entry.
*       The cpssDxChNetIfCpuCodeTableSet(...) points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       index - index into the designated device table (range 1..7)
*
* OUTPUTS:
*       designatedDevNumPtr - (pointer to)The designated device number
*                              (range 0..31)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad index
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeDesignatedDeviceTableGet
(
    IN  GT_U8       devNum,
    IN  GT_U32      index,
    OUT GT_U8       *designatedDevNumPtr
)
{
    GT_STATUS rc = GT_FAIL;
    GT_U32  hwData = 0;
    GT_U32  startBit;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPU_CODE_DESIGNATED_DEVICE_INDEX_AS_POINTER_CHECK_MAC(index);
    CPSS_NULL_PTR_CHECK_MAC(designatedDevNumPtr);

    startBit = ((index - 1) % 4) * 5;

    if(PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->debugInfoValid == GT_TRUE)
    {
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,
            PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->lastPortGroupBmp,
            portGroupId)
        {
            /* need to read the info from a specific port group */
            /* read designated device entry */
            rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,
                portGroupId,
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.cpuTargDevConfReg0 +
                    (((index - 1) / 4) * 4),
                    startBit,
                    5,
                    &hwData);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* read only from the first port group in BMP */
            break;
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,
            PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->lastPortGroupBmp,
            portGroupId)
    }
    else
    {
        /* read designated device entry */
        rc = prvCpssDrvHwPpGetRegField(devNum,
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.cpuTargDevConfReg0 +
                (((index - 1) / 4) * 4),
                startBit,
                5,
                &hwData);
    }

    *designatedDevNumPtr = (GT_U8)hwData;

    return rc;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeRateLimiterTableSet
*
* DESCRIPTION:
*       Configure CPU rate limiter entry attributes: window size and max packets
*       allowed within that window.
*       cpssDxChNetIfCpuCodeTableSet() points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*       rateLimiterIndex - CPU Code Rate Limiter Index
*                           range 1..31.
*       windowSize        - window size for this Rate Limiter in steps of
*                           Rate Limiter Window Resolution set by
*                       cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet(...)
*                           range 0..0xFFF (12 bits)
*       pktLimit          - number of packets allowed to be sent to CPU
*                           during within the configured windowSize
*                           range 0..0xFFFF (16 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad rateLimiterIndex
*       GT_OUT_OF_RANGE - on pktLimit >= 0x10000 or windowSize >= 0x1000
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeRateLimiterTableSet
(
    IN GT_U8  devNum,
    IN GT_U32 rateLimiterIndex,
    IN GT_U32 windowSize,
    IN GT_U32 pktLimit
)
{
    return cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                               rateLimiterIndex,
                                               windowSize,
                                               pktLimit);
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeRateLimiterTableGet
*
* DESCRIPTION:
*       Get the Configured CPU rate limiter enrty attributes: window size
*       and max packets allowed within that window.
*       cpssDxChNetIfCpuCodeTableSet() points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*       rateLimiterIndex - CPU Code Rate Limiter Index
*                           range 1..31.
*
* OUTPUTS:
*       windowSizePtr - (pointer to) window size for this Rate Limiter in steps
*                       of Rate Limiter Window Resolution set by
*                       cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet(...)
*                           range 0..0xFFF (12 bits)
*       pktLimitPtr - (pointer to) number of packets allowed to be sent to CPU
*                           during within the configured windowSize
*                           range 0..0xFFFF (16 bits)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad rateLimiterIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeRateLimiterTableGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  rateLimiterIndex,
    OUT GT_U32  *windowSizePtr,
    OUT GT_U32  *pktLimitPtr
)
{
    return cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                               rateLimiterIndex,
                                               windowSizePtr,
                                               pktLimitPtr);
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet
*
* DESCRIPTION:
*       Set the TO CPU Rate Limiter Window Resolution,
*       which is used as steps in Rate Limiter Window size by
*       cpssDxChNetIfCpuCodeRateLimiterTableSet(...)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*       windowResolution - The TO CPU window size resolution -- this field is
*                          the number of NanoSeconds.
*                          NOTE:
*                          The PP's actual resolution is in 32 system clock
*                          cycles (for a system clock of 200 MHz = 5nano
*                          per tick --> from 160 nanoseconds up to 327
*                          milliseconds, steps of 160 nanoseconds)
*                          So this function will round the value to the nearest
*                          PP's option.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_OUT_OF_RANGE - on windowResolution too large(depends on system clock)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet
(
    IN GT_U8  devNum,
    IN GT_U32 windowResolution
)
{
    return cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionSet(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                               windowResolution);
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet
*
* DESCRIPTION:
*       Get the TO CPU Rate Limiter Window Resolution,
*       which is used as steps in Rate Limiter Window size by
*       cpssDxChNetIfCpuCodeRateLimiterTableSet(...)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*
* OUTPUTS:
*       windowResolutionPtr - (pointer to)The TO CPU window size resolution --
*                          this field is the number of NanoSeconds.
*                          NOTE:
*                          The PP's actual resolution is in 32 system clock
*                          cycles (for a system clock of 200 MHz = 5nano
*                          per tick --> from 160 nanoseconds up to 327
*                          milliseconds, steps of 160 nanoseconds)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *windowResolutionPtr
)
{
    return cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                               windowResolutionPtr);
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet
*
* DESCRIPTION:
*       Gets the cpu code rate limiter packet counter for specific
*       rate limiter index.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number.
*       rateLimiterIndex - CPU Code Rate Limiter Index
*                           range 1..31.
*
* OUTPUTS:
*       packetCntrPtr  - pointer to the number of packets forwarded to the CPU
*                       during the current window (set by
*                       cpssDxChNetIfCpuCodeRateLimiterTableSet)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*   The counter is reset to 0 when the window ends.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet
(
    IN   GT_U8               devNum,
    IN   GT_U32              rateLimiterIndex,
    OUT  GT_U32              *packetCntrPtr
)
{

    return cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                               rateLimiterIndex,
                                               packetCntrPtr);
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeRateLimiterDropCntrGet
*
* DESCRIPTION:
*   get the number of packets forwarded to the CPU and dropped due to any
*   of the CPU rate limiters.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*
* OUTPUTS:
*       dropCntrPtr  - (pointer to) The total number of dropped packets
*                           due to any of the CPU rate limiters.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on bad parameters
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeRateLimiterDropCntrGet
(
    IN GT_U8                devNum,
    OUT GT_U32              *dropCntrPtr
)
{

    return cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                               dropCntrPtr);
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeRateLimiterDropCntrSet
*
* DESCRIPTION:
*   set the cpu code rate limiter drop counter to a specific value. This counter
*   counts the number of packets forwarded to the CPU and dropped due to any
*   of the CPU rate limiters.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*       dropCntrVal       - the value to be configured.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad parameters
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeRateLimiterDropCntrSet
(
    IN GT_U8                devNum,
    IN GT_U32               dropCntrVal
)
{
    return cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrSet(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                               dropCntrVal);
}

/*******************************************************************************
* cpssDxChNetIfFromCpuDpSet
*
* DESCRIPTION:
*       Set DP for from CPU DSA tag packets.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       dpLevel           - drop precedence level [Green, Red].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number, dpLevel
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfFromCpuDpSet
(
    IN GT_U8               devNum,
    IN  CPSS_DP_LEVEL_ENT  dpLevel
)
{
    GT_U32      regAddr;      /* The register address to read from.           */
    GT_U32      regData;      /* Data read/write from/to register.            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_COS_DP_TO_HW_CHECK_AND_CONVERT_MAC(
        devNum, dpLevel, regData);

    regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.fastStack;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 1, 2, regData);
}

/*******************************************************************************
* cpssDxChNetIfFromCpuDpGet
*
* DESCRIPTION:
*       Get DP for from CPU DSA tag packets.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*
* OUTPUTS:
*       dpLevelPtr        - pointer to drop precedence level
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - invalid hardware value for drop precedence level
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfFromCpuDpGet
(
    IN  GT_U8              devNum,
    OUT CPSS_DP_LEVEL_ENT  *dpLevelPtr
)
{
    GT_U32      regAddr;      /* The register address to read from.           */
    GT_U32      regData;      /* Data read/write from/to register.            */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(dpLevelPtr);

    regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.fastStack;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 1, 2, &regData);
    if(rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_DXCH_COS_DP_TO_SW_CHECK_AND_CONVERT_MAC(
        devNum, regData, (*dpLevelPtr));

    return GT_OK;

}

/*******************************************************************************
* cpssDxChNetIfSdmaRxResourceErrorModeSet
*
* DESCRIPTION:
*       Set a bit per TC queue which defines the behavior in case of
*       RX resource error
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number.
*       queue      - traffic class queue (0..7)
*       mode       - current packet mode: retry to send or abort
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number, queue, mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - the request is not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaRxResourceErrorModeSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U8                                   queue,
    IN  CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT   mode
)
{
    GT_U32      regAddr;      /* The register address to read from.           */
    GT_U32      regData;      /* Data read/write from/to register.            */
    GT_U32         portGroupId;/* port group Id for multi-port-groups device support */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(queue);

    /* we will use the 'First active port group' , to represent the whole device.
       that way we allow application to give SDMA memory to single port group instead
       of split it between all active port groups
    */
    portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);

    /* Due to Cheetah 3 Errata - " FEr#2009: DSDMA resource errors may cause
       PCI Express packets reception malfunction"
       -- see PRV_CPSS_DXCH3_SDMA_WA_E */
    if (PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,PRV_CPSS_DXCH3_SDMA_WA_E) == GT_TRUE)
    {
        return GT_NOT_SUPPORTED;
    }

    regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.sdmaCfgReg;

    switch (mode)
    {
    case CPSS_DXCH_NET_RESOURCE_ERROR_MODE_RETRY_E:
        regData =  0;
        break;
    case CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ABORT_E:
        regData = 1;
        break;
    default:
        return GT_BAD_PARAM;
    }

    /* relevant for bits 8:15 - so need to do shift of 8 bits */
    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, queue + 8, 1, regData);

}


/*******************************************************************************
* cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet
*
* DESCRIPTION:
*       Configure CPU rate limiter entry attributes: window size and max packets
*       allowed within that window.
*       cpssDxChNetIfCpuCodeTableSet() points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*       portGroupsBmp - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter
*                            is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       rateLimiterIndex - CPU Code Rate Limiter Index
*                           range 1..31.
*       windowSize        - window size for this Rate Limiter in steps of
*                           Rate Limiter Window Resolution set by
*                       cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet(...)
*                           range 0..0xFFF (12 bits)
*       pktLimit          - number of packets allowed to be sent to CPU
*                           during the configured windowSize.
*                           range 0..0xFFFF (16 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_OUT_OF_RANGE - on pktLimit >= 0x10000 or windowSize >= 0x1000
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*   for multi portgroup device, a rate limiter limits the rate of the incoming
*   traffic from a given port group. i.e. if a CPU code is configured to allow up
*   to 10 packets per window, it will actually allow up to 10 packets per window
*   from each portgroup.
*******************************************************************************/
GT_STATUS cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet
(
    IN GT_U8                devNum,
    IN GT_PORT_GROUPS_BMP   portGroupsBmp,
    IN GT_U32               rateLimiterIndex,
    IN GT_U32               windowSize,
    IN GT_U32               pktLimit
)
{
    GT_STATUS   rc;
    GT_U32      hwData; /* the HW format entry */
    GT_U32      regAddr;/* The register address to write to */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    CPU_CODE_RATE_LIMITER_INDEX_AS_POINTER_CHECK_MAC(rateLimiterIndex);
    if(pktLimit >= BIT_16 || windowSize>= BIT_12)
    {
        return GT_OUT_OF_RANGE;
    }

    hwData = 0;
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.cpuRateLimiterConfBase +
              (rateLimiterIndex-1) * 4;

    U32_SET_FIELD_NO_CLEAR_MAC(hwData,0,16,pktLimit);
    U32_SET_FIELD_NO_CLEAR_MAC(hwData,16,12,windowSize);

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        /* write rate limit entry */
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,hwData);
        if (GT_OK != rc)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet
*
* DESCRIPTION:
*       Get the Configured CPU rate limiter enrty attributes: window size
*       and max packets allowed within that window.
*       cpssDxChNetIfCpuCodeTableSet() points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*       portGroupsBmp - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter
*                            is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       rateLimiterIndex - CPU Code Rate Limiter Index
*                           range 1..31.
*
* OUTPUTS:
*       windowSizePtr - (pointer to) window size for this Rate Limiter in steps
*                        of Rate Limiter Window Resolution set by
*                       cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet(...)
*                           range 0..0xFFF (12 bits)
*       pktLimitPtr - (pointer to) number of packets allowed to be sent to CPU
*                           during the configured windowSize.
*                           range 0..0xFFFF (16 bits)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad rateLimiterIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   for multi portgroup device, a rate limiter limits the rate of the incoming
*   traffic from a given port group. i.e. if a CPU code is configured to allow up
*   to 10 packets per window, it will actually allow up to 10 packets per window
*   from each portgroup.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet
(
    IN  GT_U8               devNum,
    IN GT_PORT_GROUPS_BMP   portGroupsBmp,
    IN  GT_U32              rateLimiterIndex,
    OUT GT_U32              *windowSizePtr,
    OUT GT_U32              *pktLimitPtr
)
{
    GT_STATUS   rc;
    GT_U32      hwData; /* the HW format entry */
    GT_U32      regAddr;/* The register address to read from */
    GT_U32      portGroupId;/*the port group Id- support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPU_CODE_RATE_LIMITER_INDEX_AS_POINTER_CHECK_MAC(rateLimiterIndex);
    CPSS_NULL_PTR_CHECK_MAC(windowSizePtr);
    CPSS_NULL_PTR_CHECK_MAC(pktLimitPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.cpuRateLimiterConfBase +
              (rateLimiterIndex-1) * 4;

    /* read rate limit entry */
    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum,portGroupId,regAddr,&hwData);

    *pktLimitPtr   = U32_GET_FIELD_MAC(hwData, 0,16);
    *windowSizePtr = U32_GET_FIELD_MAC(hwData,16,12);

    return rc;
}

/*******************************************************************************
* cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionSet
*
* DESCRIPTION:
*       Set the TO CPU Rate Limiter Window Resolution,
*       which is used as steps in Rate Limiter Window size by
*       cpssDxChNetIfCpuCodeRateLimiterTableSet(...)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*       portGroupsBmp - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter
*                            is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       windowResolution - The TO CPU window size resolution -- this field is
*                          the number of NanoSeconds.
*                          NOTE:
*                          The PP's actual resolution is in 32 system clock
*                          cycles (for a system clock of 200 MHz = 5nano
*                          per tick --> from 160 nanoseconds up to 327
*                          milliseconds, steps of 160 nanoseconds)
*                          So this function will round the value to the nearest
*                          PP's option.
*                          field range: minimal value is 1. Maximal is 2047.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_OUT_OF_RANGE - on windowResolution too large(depends on system clock)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionSet
(
    IN GT_U8                devNum,
    IN GT_PORT_GROUPS_BMP   portGroupsBmp,
    IN GT_U32               windowResolution
)
{
    GT_STATUS   rc;
    GT_U32  regAddr;      /* The register address to read from.           */
    GT_U32  regData;      /* Data read/write from/to register.            */
    GT_U32  fieldResolution; /* resolution of field in the PP reg
                                   in nano seconds                        */
    GT_U32  hwWindowResolution;/* HW resolution is in steps of fieldResolution*/
    GT_U32  roundValue; /* round value is needed in order to calculate HW
                            window resolution in steps of field resolutiuon */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    /* for CH2 board systemClock = 200 MHz, the window
       resolution is in steps of 32 clock cycles */
    fieldResolution = (1000 * 32) /  PRV_CPSS_PP_MAC(devNum)->coreClock;
    roundValue = fieldResolution / 2;

    /* The resolution is in steps of fieldResolution */
    hwWindowResolution = (windowResolution + roundValue) / fieldResolution;

    /* minimal value in the register is 1 and maximal is 11 bits */
    if ((hwWindowResolution == 0) || (hwWindowResolution > BIT_11))
    {
        return GT_BAD_PARAM;
    }


    /* Get the address of the Pre Egress Engine register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bufferMng.eqBlkCfgRegs.preEgrEngineGlobal;

    regData = hwWindowResolution;

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        /* set Window Resolution field in
           Pre-Egress Engine Global Configuration register */
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,
                                                        0, 11, regData);
        if (GT_OK != rc)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet
*
* DESCRIPTION:
*       Get the TO CPU Rate Limiter Window Resolution,
*       which is used as steps in Rate Limiter Window size by
*       cpssDxChNetIfCpuCodeRateLimiterTableSet(...)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*       portGroupsBmp - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter
*                            is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       windowResolutionPtr - (pointer to)The TO CPU window size resolution --
*                          this field is the number of NanoSeconds.
*                          NOTE:
*                          The PP's actual resolution is in 32 system clock
*                          cycles (for a system clock of 200 MHz = 5nano
*                          per tick --> from 160 nanoseconds up to 327
*                          milliseconds, steps of 160 nanoseconds)
*                          field range: minimal value is 1. Maximal is 2047.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_GROUPS_BMP   portGroupsBmp,
    OUT GT_U32               *windowResolutionPtr
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;      /* The register address to read from.           */
    GT_U32      regData;      /* Data read/write from/to register.            */
    GT_U32      fieldResolution; /* resolution of field in the PP reg
                                   in the nano seconds                        */
    GT_U32      hwWindowResolution;/* HW resolution is in steps of fieldResolution*/
    GT_U32      portGroupId;/*the port group Id- support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(windowResolutionPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    /* Get the address of the Pre Egress Engine register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bufferMng.eqBlkCfgRegs.preEgrEngineGlobal;

    /* get Window Resolution field in
       Pre-Egress Engine Global Configuration register */
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                                    0, 11, &regData);

    hwWindowResolution = regData;

    /* for CH2 board systemClock = 200 MHz, the window
       resolution is in steps of 32 clock cycles */
    fieldResolution = (1000 * 32) /  PRV_CPSS_PP_MAC(devNum)->coreClock;

    /* The resolution is in steps of fieldResolution */
    *windowResolutionPtr = hwWindowResolution * fieldResolution;

    return rc;
}

/*******************************************************************************
* cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet
*
* DESCRIPTION:
*       Gets the cpu code rate limiter packet counter for specific
*       rate limiter index.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number.
*       portGroupsBmp  - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       rateLimiterIndex - CPU Code Rate Limiter Index
*                           range 1..31.
*
* OUTPUTS:
*       packetCntrPtr  - pointer to the number of packets forwarded to the CPU
*                       during the current window (set by
*                       cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*   The counter is reset to 0 when the window ends.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet
(
    IN   GT_U8               devNum,
    IN   GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN   GT_U32              rateLimiterIndex,
    OUT  GT_U32              *packetCntrPtr
)
{
    GT_U32      regAddr; /* The register address to read from */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPU_CODE_RATE_LIMITER_INDEX_AS_POINTER_CHECK_MAC(rateLimiterIndex);
    CPSS_NULL_PTR_CHECK_MAC(packetCntrPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.cpuRateLimiterPktCntrBase +
              ((rateLimiterIndex - 1) * 4);

    return prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                  regAddr, 0, 16,
                                                  packetCntrPtr, NULL);

}
/*******************************************************************************
* cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet
*
* DESCRIPTION:
*   get the number of packets forwarded to the CPU and dropped due to any
*   of the CPU rate limiters.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*       portGroupsBmp  - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
*
* OUTPUTS:
*       dropCntrPtr  - (pointer to) The total number of dropped packets
*                           due to any of the CPU rate limiters.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on bad parameters
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet
(
    IN GT_U8                devNum,
    IN GT_PORT_GROUPS_BMP   portGroupsBmp,
    OUT GT_U32              *dropCntrPtr
)
{
    GT_U32      regAddr;/* The register address to read from */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(dropCntrPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.cpuRateLimiterDropCntrReg;

    return prvCpssDxChPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                  regAddr, 0, 32,
                                                  dropCntrPtr, NULL);

}
/*******************************************************************************
* cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrSet
*
* DESCRIPTION:
*   set the cpu code rate limiter drop counter to a specific value. This counter
*   counts the number of packets forwarded to the CPU and dropped due to any
*   of the CPU rate limiters.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*       portGroupsBmp  - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       dropCntrVal       - the value to be configured.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad parameters
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrSet
(
    IN GT_U8                devNum,
    IN GT_PORT_GROUPS_BMP   portGroupsBmp,
    IN GT_U32               dropCntrVal
)
{
    GT_U32  regAddr;/* The register address to write to */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.cpuRateLimiterDropCntrReg;

    return prvCpssDxChPortGroupsBmpCounterSet(devNum, portGroupsBmp,
                                              regAddr, 0, 32, dropCntrVal);

}



#ifndef __AX_PLATFORM__
/*******************************************************************************
* cpssDxChNetIfCpuCodeEntrySet
*
* DESCRIPTION:
*       Set CPU code entry
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum    	- Device number.
*	  cpuCode	- CPU code number.
*	  tc			- Traffic class.
*	  dp 			- Drop precedence.
*	  truncate 	- truncate packet to 128 bytes or not.
*	  cpuRateLimitMode - CPU rate limit mode: local or aggragate.
*	  cpuCodeRateLimiterIndex - index to CPU code rate limiter.
*	  cpuCodeStateRateLimitIndex - Index of one of the 32 statistical rate
*                            limiters for this CPU Code .
*	   destDevNumIndex - The target device index for packets with this
*                              CPU code.
*	
* OUTPUTS:
*	  NONE
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeEntrySet
(
    IN  GT_U8 devNum,
    IN  CPSS_NET_RX_CPU_CODE_ENT cpuCode,
    IN  GT_U8 tc,
    IN  CPSS_DP_LEVEL_ENT dp,
    IN 	GT_BOOL	  truncate,
    IN	CPSS_NET_CPU_CODE_RATE_LIMIT_MODE_ENT cpuRateLimitMode,
    IN	GT_U32	cpuCodeRateLimiterIndex,
    IN	GT_U32	cpuCodeStateRateLimitIndex,
    IN	GT_U32	destDevNumIndex
)
{
    GT_STATUS result;

    CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC  entryInfo;

    /* check for valid arguments */
    if(devNum > 1)
        return GT_BAD_PARAM;

    /* map input arguments to locals */

    entryInfo.tc =  tc;
    entryInfo.dp =  dp;
    entryInfo.truncate =  truncate;
    entryInfo.cpuRateLimitMode = cpuRateLimitMode;
    entryInfo.cpuCodeRateLimiterIndex =  cpuCodeRateLimiterIndex;
    entryInfo.cpuCodeStatRateLimitIndex =  cpuCodeStateRateLimitIndex;
    entryInfo.designatedDevNumIndex =  destDevNumIndex;

    /* call cpss api function */
    result = cpssDxChNetIfCpuCodeTableSet(devNum, cpuCode, &entryInfo);

	if(GT_OK!=result)
		osPrintf("cpssDxChNetIfCpuCodeEntrySet::dev %d cpuCode %d rc %d\n",devNum,cpuCode,result);
    return result;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeEntryGet
*
* DESCRIPTION:
*       Get CPU code entry
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum    	- Device number.
*	  cpuCode	- CPU code number.
*	
* OUTPUTS:
*	  entryInfo 	- CPU code table entry 
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeEntryGet
(
    IN  GT_U8 devNum,
    IN  CPSS_NET_RX_CPU_CODE_ENT cpuCode,
    OUT CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC *entryInfo
)
{
    GT_STATUS result;

    /* check for valid arguments */
    if(devNum > 1)
        return GT_BAD_PARAM;

    /* map input arguments to locals */
	if(NULL == entryInfo) 
		return GT_BAD_PTR;
	
    /* call cpss api function */
    result = cpssDxChNetIfCpuCodeTableGet(devNum, cpuCode, entryInfo);

	if(result != GT_OK) {
		return GT_FAIL;
	}
	else {
		fprintf(stdout,"----- --- ---\n");
		fprintf(stdout,"%-5s %-3s %-3s\n","CODE","TC","DP");
		fprintf(stdout,"----- --- ---\n");
		fprintf(stdout,"%-5d %-3d %-3d\n",cpuCode,entryInfo->tc,entryInfo->dp);
		fprintf(stdout,"----- --- ---\n");
	}

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfCpuRateLimiterInit
*
* DESCRIPTION:
*       Initialize CPU port rate limiter according to static configuration
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum    	- Device number.
*	
* OUTPUTS:
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuRateLimiterInit
(
	IN GT_U8 devNum
)
{
	GT_STATUS result 	= GT_OK;

	GT_U32 i = 0, count = 0;

	struct NETIF_PORT_TRFFIC_LIMIT_INFO *limiter = NULL;

	result = cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet(
							devNum,	 CPSS_PRE_EGRESS_RATE_LIMIT_RESOLUTION_DEFAULT);
	if(GT_OK != result) {
		osPrintfErr("cpu port rate limit set device %d time resolution %#x err %d\n",	\
					devNum,CPSS_PRE_EGRESS_RATE_LIMIT_RESOLUTION_DEFAULT,result);
		return result;
	}

	count = sizeof(cpu_port_rate_limiter)/sizeof(struct NETIF_PORT_TRFFIC_LIMIT_INFO);
	/* rate limiter 0 is invalid */
	for(i = 1;i < count; i++) {
		limiter = &(cpu_port_rate_limiter[i]);
		if((0 == limiter->pktLimit) || (0 == limiter->windowSize)) 	
			continue;
		
		result =  cpssDxChNetIfCpuCodeRateLimiterTableSet( devNum,
										i, limiter->windowSize, limiter->pktLimit);
		if(GT_OK != result) 
			return result;
	}

	/* enable TCP-SYN cpu code */
	result = cpssDxChNetIfAppSpecificCpuCodeTcpSynSet(devNum, GT_TRUE);
	if(GT_OK != result) {
		osPrintfErr("enable TCP-SYN cpu code error %d\n",result);
		return result;
	}

	return result;
}

/*******************************************************************************
* cpssDxChNetIfCpuRxFcInit
*
* DESCRIPTION:
*       Initialize CPU port Rx flow control
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum    	- Device number.
*	
* OUTPUTS:
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
* +-------------+---------------------------+-----------+
* |	PACKET TYPE	|	CPU CODE 				|	QUEUE	|
* +-------------+---------------------------+-----------+
* | 	 BPDU		|  	 2(BPDU_TRAP) 			|    	1		| 
* +-------------+---------------------------+-----------+
* | 	 ARP			|  	 5(ARP_BC_TRAP/MIRROR) 	|    	2		| 
* +-------------+---------------------------+-----------+
* | 	 RIPv1		|  	 17(RIPv1_MIRROR)		|    	3		| 
* +-------------+---------------------------+-----------+
* | 	 OSPF		|  	 3(FDB_TRAP/MIRROR)		|    	3		| 
* +-------------+---------------------------+-----------+
* | 	 CAPWAP		|  	 32(UDP_BC_TRAP/MIRROR)	|    	4		| 
* +-------------+---------------+-----------+-----------+
* | 	 DHCP<Req>	|  	 19(IPv4_BC_TRAP/MIRROR)	|    	5		| 
* +-------------+---------------+-----------+-----------+
* | 	 DHCP<Rep>	|  	 32(UDP_BC_TRAP/MIRROR)	|    	5		| 
* +-------------+---------------------------+-----------+
* | 	 Other		|  	 other value 				|    	0		| 
* +-------------+---------------------------+-----------+
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuRxFcInit
(
	IN GT_U8 devNum
)
{
	GT_STATUS result 	= GT_OK;
	GT_U32 i = 0, count = 0;
	struct NETIF_PORT_TRAFFIC_CLASSIFIER *classifier = NULL;
	unsigned long tbMtu = 0;
	unsigned int bpsBase = 0, value = 0;
	unsigned int portShaper = 0;
	unsigned int queueShaper[NUM_OF_TX_QUEUES] = { \
		800,800,600,600,400,400,200,200
	};

    PRV_CPSS_DXCH2_DEV_CHECK_MAC(devNum);

	/* STAGE 1: setup traffic classifier */
	count = sizeof(cpu_port_traffic_classifier)/sizeof(struct NETIF_PORT_TRAFFIC_CLASSIFIER);

	for(i = 0; i < count; i++) {
		classifier = &(cpu_port_traffic_classifier[i]);
		result = cpssDxChNetIfCpuCodeEntrySet(devNum,
												classifier->cpuCode,
												classifier->tc,
												classifier->dp,
												classifier->truncate,
												classifier->cpuRateLimitMode,
												classifier->cpuCodeRateLimiterIndex,
												classifier->cpuCodeStateRateLimitIndex,
												classifier->destDevNumIndex);
		if(GT_OK != result) {
			osPrintfErr("cpu flow control init device %d packet type %d cpuCode %d tc %d dp %d rc %d\n",	\
							devNum,i,classifier->cpuCode,	\
							classifier->tc, classifier->dp,result);
			return result;
		}
	}

	/* STAGE 2: setup CPU port Tx queue shaper */
	/* step 1: set CPU port packet Tx shaper mode PACKET 
 	 *  0 - CPSS_PORT_TX_DROP_SHAPER_BYTE_MODE_E
 	 *  1 - CPSS_PORT_TX_DROP_SHAPER_PACKET_MODE_E
	 */
	
	result = cpssDxChPortTxToCpuShaperModeSet(devNum,1);
	if(GT_OK != result) {
		osPrintfErr("set device %d cpu port Tx shaper mode packet-based error %d\n",result);
		return result;
	}
	/* step 2: convert shaper value from PPS to bps */
	result = cpssDrvPpHwRegFieldGet(devNum,0, 0x01800004,3,2,&tbMtu);
	if(GT_OK != result) {
		osPrintfErr("get token bucket MTU error %d when set cpu port and queue\n", result);
		return result;
	}
	else {
		osPrintfDbg("get token bucket MTU %d when set cpu port and queue shaper\n", tbMtu);
		/* 2 for 10KB, 1 for 2KB, 0 for 1.5KB */
		bpsBase = (0x2 == tbMtu) ? (10 * 1000 * 8) : 	\
				  (0x1 == tbMtu) ? (2 * 1000 * 8) : (1.5 * 1000 * 8);
	}
	/* step 3: set CPU port all queue shaper value */
	for(i = 0; i < NUM_OF_TX_QUEUES; i++) {
		value = (bpsBase * queueShaper[i])/1000;
		result = cpssDxChPortTxQShaperEnableSet(devNum,CPSS_CPU_PORT_NUM_CNS,i,1);
		if(GT_OK != result) {
			osPrintfErr("set device %d CPU port queue %d shaper %d enable error %d\n",	\
					devNum, i,queueShaper[i], result);
		}
		
		osPrintfDbg("set device %d CPU port queue %d shaper<%d,%d>\n",	\
					devNum, i,queueShaper[i], value);
		result = cpssDxChPortTxQShaperProfileSet(devNum,CPSS_CPU_PORT_NUM_CNS,i,0x1, &value);
		if(GT_OK != result) {
			osPrintfErr("set device %d CPU port queue %d shaper<%d,%d> error %d\n",	\
					devNum, i,queueShaper[i], value, result);
		}
		else {
			osPrintfDbg("set device %d CPU port queue %d shaper %d real %d\n",	\
					devNum, i,queueShaper[i], value);
		}
	}
	
	/* STAGE 3: setup CPU port Tx shaper */
	portShaper = 1200;
	value = (bpsBase * portShaper)/1000;
	result = cpssDxChPortTxShaperEnableSet(devNum,CPSS_CPU_PORT_NUM_CNS,1);
	if(GT_OK != result) {
		osPrintfErr("set device %d CPU port shaper enable error %d\n",devNum,result);
		return result;
	}
	osPrintfDbg("set device %d CPU port shaper<%d,%d>\n",devNum, portShaper, value);
	result = cpssDxChPortTxShaperProfileSet(devNum,CPSS_CPU_PORT_NUM_CNS,0x1, &value);
	if(GT_OK != result) {
		osPrintfErr("set device %d CPU port shaper<%d,%d> error %d\n",	\
				devNum, portShaper, value, result);
		return result;
	}
	else {
		osPrintfDbg("set device %d CPU port shaper %d real %d\n",	\
				devNum, portShaper, value);
	}
	return result;
}

/*******************************************************************************
* cpssDxChNetIfCpufcRateLimiterSet
*
* DESCRIPTION:
*       Set CPU port Rx flow control for specified packet
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum    	- Device number.
*	  	 packetType - Packet type to set cpu flow control
*       windowSize        - window size for this Rate Limiter in steps of
*                           Rate Limiter Window Resolution set by
*                       cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet(...)
*                           range 0..0xFFF (12 bits)
*       pktLimit          - number of packets allowed to be sent to CPU
*                           during within the configured windowSize
*                           range 0..0xFFFF (16 bits)
*	
* OUTPUTS:
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpufcRateLimiterSet
(
	IN GT_U8 devNum,
	IN CPU_FC_PACKET_TYPE	packetType,
	IN GT_U32	windowSize,
	IN GT_U32 	pktCnt
)
{
	GT_STATUS result = GT_OK, retVal = GT_OK;
	unsigned int rateLimiterIndex = CPU_PORT_DEFAULT_RATE, i = 0;
	CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC entryInfo;
	CPSS_NET_RX_CPU_CODE_ENT	cpuCode[10] = {0};
	
	PRV_CPSS_DXCH2_DEV_CHECK_MAC(devNum);

	switch(packetType) {
		case CPU_FC_TYPE_ARP_E:
			rateLimiterIndex = CPU_PORT_ARP_BC_RATE;
			cpuCode[0] = CPSS_NET_INTERVENTION_ARP_E;
			cpuCode[1] = CPSS_NET_ARP_REPLY_TO_ME_E;
			break;
		case CPU_FC_TYPE_BPDU_E:
			rateLimiterIndex = CPU_PORT_BPDU_RATE;
			cpuCode[0] = CPSS_NET_CONTROL_BPDU_E;
			break;
		case CPU_FC_TYPE_DHCP_E:
			rateLimiterIndex = CPU_PORT_DHCPRPL_RATE;
			cpuCode[0] = CPSS_NET_IPV4_BROADCAST_PACKET_E;
			cpuCode[1] = CPSS_NET_UDP_BC_MIRROR_TRAP0_E;
			break;
		case CPU_FC_TYPE_ICMP_E:
			rateLimiterIndex = CPU_PORT_ICMP_RATE;
			cpuCode[0] = CPSS_NET_ROUTED_PACKET_FORWARD_E;
			cpuCode[1] = CPSS_NET_ROUTE_ENTRY_TRAP_E;
			cpuCode[2] = CPSS_NET_IPV4_UC_ROUTE1_TRAP_E;
			cpuCode[3] = CPSS_NET_IPV4_UC_ROUTE2_TRAP_E;
			cpuCode[4] = CPSS_NET_IPV4_UC_ROUTE3_TRAP_E;
			break;
		case CPU_FC_TYPE_IGMP_E:
			rateLimiterIndex = CPU_PORT_IGMP_RATE;
			cpuCode[0] = CPSS_NET_INTERVENTION_IGMP_E;
			cpuCode[1] = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;
			break;
		case CPU_FC_TYPE_OSPF_E:
			rateLimiterIndex = CPU_PORT_OSPFv1_RATE;
			cpuCode[0] = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E;
			break;
		case CPU_FC_TYPE_RIPv1_E:
			rateLimiterIndex = CPU_PORT_RIPv1_RATE;
			cpuCode[0] = CPSS_NET_MIRROR_IPV4_RIPV1_TO_CPU_E;
			break;
		case CPU_FC_TYPE_RIPv2_E:
			rateLimiterIndex = CPU_PORT_RIPv2_RATE;
			cpuCode[0] = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E;
			break;
		case CPU_FC_TYPE_TELNET_E:
			rateLimiterIndex = CPU_PORT_TELNET_RATE;
			cpuCode[0] = CPSS_NET_OAM_TCP_TELNET_E;
			break;
		case CPU_FC_TYPE_SSH_E:
			rateLimiterIndex = CPU_PORT_SSH_RATE;
			cpuCode[0] = CPSS_NET_OAM_TCP_SSH_E;
			break;
		case CPU_FC_TYPE_HTTP_E:
			rateLimiterIndex = CPU_PORT_HTTP_RATE;
			cpuCode[0] = CPSS_NET_OAM_TCP_HTTP_E;
			break;		
		case CPU_FC_TYPE_HTTPS_E:
			rateLimiterIndex = CPU_PORT_HTTPS_RATE;
			cpuCode[0] = CPSS_NET_OAM_TCP_HTTPS_E;
			break;		
		case CPU_FC_TYPE_SNMP_E:
			rateLimiterIndex = CPU_PORT_SNMP_RATE;
			cpuCode[0] = CPSS_NET_OAM_UDP_SNMP_E;
			break;
		case CPU_FC_TYPE_CAPWAP_CTRL_E:
			rateLimiterIndex = CPU_PORT_CAPWAP_RATE;
			cpuCode[0] = CPSS_NET_WIRELESS_UDP_CNTL_E; 
			break;
		case CPU_FC_TYPE_CAPWAP_DATA_E:
			rateLimiterIndex = CPU_PORT_CAPWAP_RATE;
			cpuCode[0] = CPSS_NET_WIRELESS_UDP_CNTL_E; 
			break;
		default:
			break;
	}

	if(CPU_PORT_DEFAULT_RATE == rateLimiterIndex) {
		return GT_NOT_SUPPORTED;
	}
	
	/* no rate limiter when windowSize or pktCnt is zero */
	if(0 == windowSize || 0 == pktCnt) {
		rateLimiterIndex = CPU_PORT_DEFAULT_RATE;
	}
	else {		
		result = cpssDxChNetIfCpuCodeRateLimiterTableSet(devNum,	\
										rateLimiterIndex,windowSize,pktCnt);
	}
	
	for(i=0;(i<10)&&(cpuCode[i] != 0); i++) {
		memset(&entryInfo, 0, sizeof(CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC));
		retVal = cpssDxChNetIfCpuCodeTableGet(devNum, cpuCode[i], &entryInfo);
		if(GT_OK != retVal) {
			osPrintfErr("set rate limiter get cpu code %d table entry error %d\r\n", \
					cpuCode[i], retVal);
		}
		if(rateLimiterIndex != entryInfo.cpuCodeRateLimiterIndex) {
			osPrintfDbg("update cpu code %d table entry rate limiter %d -> %d\r\n", \
					cpuCode[i], entryInfo.cpuCodeRateLimiterIndex, rateLimiterIndex);
			entryInfo.cpuCodeRateLimiterIndex = rateLimiterIndex;
			retVal = cpssDxChNetIfCpuCodeTableSet(devNum, cpuCode[i], &entryInfo);
			if(GT_OK != retVal) {
				osPrintfErr("set rate limiter update cpu code %d table entry error %d\r\n", \
						cpuCode[i], retVal);
			}
		}
	}
	
	return result;
}

/*******************************************************************************
* cpssDxChNetIfOamTrafficSet
*
* DESCRIPTION:
*       Set range to TCP/UPD Destination Port Range CPU Code Table with specific
*       CPU Code. There are 16 ranges may be defined.
*       The feature enabled for packets if one of conditions are true:
*        1. MAC DA is found in the FDB and Application Specific CPU Code field
*           of FDB entry is True (see appCpuCodeEn in GT_MAC_ENTRY)
*        2. Packet passed Routing and Application Specific CPU Code field is
*           True in the Route entry (Next Hop)
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum  - Device number.
*       oamType - OAM traffic type
*       dstPort - OAM traffic destination port.
*	  rangeIdx - tcp/udp range index add to hw table
*
* OUTPUTS:
*       rangeIdx - tcp/udp range index acutally add to hw table
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad rangeIndex or
*                          bad packetType or bad protocol or bad cpuCode
*
* COMMENTS:
*	  if hw table range index need allocate by driver, value ~0UL should given to rangeIdx;
*  otherwise, the given value of rangeIdx will be used. 
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfOamTrafficSet
(
	IN GT_U8						devNum,
	IN CPSS_OAM_TRAFFIC_TYPE_ENT	oamType,
	IN GT_U16						dstPort,
	INOUT GT_U32						*rangeIdx
)
{
	static GT_U32 rangeIndexArr[PRV_CPSS_MAX_PP_DEVICES_CNS] = {0};
	GT_U32	rangeIndex = 0;
	CPSS_NET_TCP_UDP_PACKET_TYPE_ENT packetType = CPSS_NET_TCP_UDP_PACKET_BOTH_UC_MC_E;
	CPSS_NET_PROT_ENT	protocol = CPSS_NET_PROT_BOTH_UDP_TCP_E;
	CPSS_NET_RX_CPU_CODE_ENT cpuCode = CPSS_NET_ALL_CPU_OPCODES_E;
	GT_STATUS rc = GT_OK;

	PRV_CPSS_DXCH2_DEV_CHECK_MAC(devNum);

	/* no range index specified, allocate one */
	if(~0UL == *rangeIdx) {
		rangeIndex = rangeIndexArr[devNum];
	}
	else {
		rangeIndex = *rangeIdx;
	}
	
	if(rangeIndex > DXCH2_CPU_CODE_L4_DEST_PORT_RANGE_MAX_INDEX_CNS) {
		osPrintfErr("tcp/udp dest port range selector index exhausted\n");
		return GT_BAD_PARAM;
	}
	rangeIndexArr[devNum]++;


	packetType = CPSS_NET_TCP_UDP_PACKET_UC_E;
	protocol = CPSS_NET_PROT_TCP_E;
	
	switch(oamType) {
		default:
			return GT_BAD_PARAM;
		case CPSS_OAM_TRAFFIC_TELNET_E:
			cpuCode = CPSS_NET_OAM_TCP_TELNET_E;
			break;
		case CPSS_OAM_TRAFFIC_SSH_E:
			cpuCode = CPSS_NET_OAM_TCP_SSH_E;
			break;
		case CPSS_OAM_TRAFFIC_HTTP_E:
			cpuCode = CPSS_NET_OAM_TCP_HTTP_E;
			break;
		case CPSS_OAM_TRAFFIC_HTTPS_E:
			cpuCode = CPSS_NET_OAM_TCP_HTTPS_E;
			break;
		case CPSS_OAM_TRAFFIC_SNMP_E:
			protocol = CPSS_NET_PROT_UDP_E;
			cpuCode = CPSS_NET_OAM_UDP_SNMP_E;
			break;
		case CPSS_WIRELESS_CAPWAP_CNTL_E:
			protocol = CPSS_NET_PROT_UDP_E;
			cpuCode = CPSS_NET_WIRELESS_UDP_CNTL_E;
			break;
		case CPSS_WIRELESS_CAPWAP_DATA_E:
			protocol = CPSS_NET_PROT_UDP_E;
			cpuCode = CPSS_NET_WIRELESS_UDP_DATA_E;	
			break;
	}

	rc = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet(	\
						devNum, rangeIndex, dstPort, dstPort,		\
						packetType, protocol,cpuCode);
	*rangeIdx = rangeIndex;
	
	return rc;
}

#endif


