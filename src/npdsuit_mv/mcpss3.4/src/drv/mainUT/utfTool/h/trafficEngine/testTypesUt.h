/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* testTypesUt.h
*
* DESCRIPTION:
*       define test types
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __testTypesUth
#define __testTypesUth

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <gtOs/gtOsIo.h>
#include <gtOs/gtOsStr.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>
#include <cpss/generic/ip/cpssIpTypes.h>
#include <cpss/generic/tunnel/cpssGenTunnelTypes.h>
#include <cpss/generic/networkIf/cpssGenNetIfTypes.h>



#include <trafficEngine/trafficEngineUt.h>

#ifdef CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
    #include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
    #include <cpss/dxCh/dxChxGen/tunnel/cpssDxChTunnel.h>
    #include <cpss/dxCh/dxChxGen/ip/cpssDxChIpTypes.h>
    #include <cpss/dxCh/dxChxGen/ip/cpssDxChIp.h>
    #include <cpss/dxCh/dxChxGen/ip/cpssDxChIpCtrl.h>
    #include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpmTypes.h>
    #include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpm.h>
#endif /*CHX_FAMILY*/


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* utTrafficDemoTestRun
*
* DESCRIPTION:
*       run a test
*
* INPUTS:
*       testClass - the class of test
*       testNum   - number of the test (in the class)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_NOT_IMPLEMENTED - test number not implemented
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS utTrafficDemoTestRun(
    IN GT_U32   testClass,
    IN GT_U32   testNum
);

/* max ports in device in PPs of Prestera family */
#define UT_TEST_MAX_PORTS_NUM_CNS   64

/* set compiler to avoid non used parameter of the function */
#define UT_TEST_PARAM_NUT_USED(x)   (void)x

/* number of the phase */
#define UT_TEST_PHASE(x)    x

/* name of test (function name) */
#define UT_TEST_NAME_MAC(_testClass,_testNum) utTrafficDemoTestRun_##_testClass##_##_testNum

/* copy field from source structure to destination structure
   dstStc can be pointer or not the 'field' should hold
   '->' or '.'

   --> the 'swap' allow to do copy of dst to src !
*/
#define UT_TEST_FIELD_COPY_MAC(swap,dstStcPtr,srcStcPtr,field) \
    if(swap == 0)                                \
        (dstStcPtr)->field = (srcStcPtr)->field;   \
    else                                         \
        (srcStcPtr)->field = (dstStcPtr)->field

/* macro that generate the field name (string) and it's value ,
used for function utTestExpectedResultCheck */
#define UT_TEST_FIELD_NAME_AND_VALUE_MAC(field)    #field,field

/* define value for 'don't use' the extraInfo in function utTestExpectedResultCheck */
#define UT_TEST_EXPECTED_EXTRA_INFO_UNUSED_CNS      0xfefefefe

/* reset array according to it's size */
#define UT_TEST_RESET_ARRAY_MAC(array)              osMemSet(array,0,sizeof(array))
/* reset parameter according to it's size */
#define UT_TEST_RESET_PARAM_MAC(param)              osMemSet(&(param),0,sizeof(param))
/* reset memory from pointer according to given size */
#define UT_TEST_RESET_FROM_POINTER_MAC(prt,size)    osMemSet(prt,0,size)


/* indication that test should fail on first fail */
extern GT_U32   utBreakOnFirstFail;
/* name of current test */
extern char utTestCurrentTestName[/*256*/];
/* state of current test */
extern GT_U32  utTestCurrentTestFail;

/* macro that check return value and print error location */
#define UT_TEST_RC_CHECK(returnVal)             \
    {                                           \
        GT_STATUS _rc = returnVal;              \
        if(_rc != GT_OK)                        \
        {                                       \
            utTestCurrentTestFail = 1;          \
            osPrintf(" failed rc=[0x%lx] in file[%s] in line [%d]\n",_rc,__FILE__,__LINE__);\
            if(utBreakOnFirstFail)              \
            {                                   \
                osPrintf("test failed: [%s]\n",utTestCurrentTestName); \
                return _rc;                     \
            }                                   \
        }                                       \
    }


/*typedef struct{
    CPSS_INTERFACE_TYPE_ENT     type;

    struct{
        GT_U8   devNum;
        GT_U8   portNum;
    }devPort;

    GT_TRUNK_ID  trunkId;
    GT_U16       vidx;
    GT_U16       vlanId;
    GT_U8        devNum;
    GT_U16       fabricVidx;
}CPSS_INTERFACE_INFO_STC;
*/

/* define ports : of type CPSS_INTERFACE_INFO_STC */
#define UT_TEST_PORT_MAC(device,port)   \
    {                           \
        CPSS_INTERFACE_PORT_E,  \
        {                       \
            device,             \
            port,               \
        },                      \
        0                       \
    }

/* device 0 -- port 0 */
#define UT_TEST_PORT_0_0            UT_TEST_PORT_MAC(0,0)
/* device 0 -- port 8 */
#define UT_TEST_PORT_0_8            UT_TEST_PORT_MAC(0,8)
/* device 0 -- port 18 */
#define UT_TEST_PORT_0_18           UT_TEST_PORT_MAC(0,18)
/* device 0 -- port 23 */
#define UT_TEST_PORT_0_23           UT_TEST_PORT_MAC(0,23)

/* device 0 -- port 24 */
#define UT_TEST_PORT_0_24           UT_TEST_PORT_MAC(0,24)
/* device 0 -- port 25 */
#define UT_TEST_PORT_0_25           UT_TEST_PORT_MAC(0,25)

/* default (empty) mac entry : of type CPSS_MAC_ENTRY_EXT_STC */
#define UT_TEST_MAC_ENTRY_DEFAULT_MAC                   \
    {                                                   \
        {                                               \
            CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E,         \
            {                                           \
                {                                       \
                    {{0,0,0,0,0,0}},                    \
                    0                                   \
                }                                       \
            }                                           \
        },                                              \
        UT_TEST_PORT_MAC(0,0),                          \
        0                                               \
    }

/* define mac address [0x00,0x00,0x00,0x00,0x34,0x02] */
#define UT_TEST_MAC_ADDR_DA_000000003402_CNS    \
    {0x00,0x00,0x00,0x00,0x34,0x02}

/* define mac address [0x00,0x00,0x00,0x00,0x00,0x01] */
#define UT_TEST_MAC_ADDR_SA_000000000001_CNS    \
    {0x00,0x00,0x00,0x00,0x00,0x01}

/* set dst mac address by src mac */
#define UT_TEST_MAC_ADDR_SET(macAddrDst,macAddrSrc)\
    macAddrDst[0] = macAddrSrc[0]; \
    macAddrDst[1] = macAddrSrc[1]; \
    macAddrDst[2] = macAddrSrc[2]; \
    macAddrDst[3] = macAddrSrc[3]; \
    macAddrDst[4] = macAddrSrc[4]; \
    macAddrDst[5] = macAddrSrc[5]


/* max buffer size for packets */
#define UT_TEST_BUFFER_MAX_SIZE_CNS   0x600   /*1536*/


/*
typedef struct{
    GT_U32                  totalLen; not include CRC
    GT_U32                  numOfParts;  number of elements in array of partsArray
    UT_PACKET_PART_STC      *partsArray; (pointer to array of)
                                            parts must be given in actual order
                                            in frame
                                            this way we can support :
                                            1. tunneling (ipv4-over-ipv4)
                                            2. multi vlan tag
                                            ...

}UT_TRAFFIC_ENGINE_PACKET_STC;
*/
#define UT_TEST_PACKET_IP_MAC(length,numParts,partsArray) \
    {                                                       \
        length,/*totalLen*/                                 \
        numParts ,/*numOfParts*/                            \
        partsArray /*partsArray*/                           \
    }

/* sections in the packet */
extern UT_TRAFFIC_ENGINE_PACKET_L2_STC          *utTestPacketL2PartPtr;
extern UT_TRAFFIC_ENGINE_PACKET_ETHERTYPE_STC   *utTestPacketEtherTypePartPtr;
extern UT_TRAFFIC_ENGINE_PACKET_VLAN_TAG_STC    *utTestPacketVlanTagPartPtr;
extern UT_TRAFFIC_ENGINE_PACKET_PAYLOAD_STC     *utTestPacketPayLoadPartPtr;


/* pointer to predefined IPv4 packet with vlan tag */
extern UT_TRAFFIC_ENGINE_PACKET_STC   *utTestPacketIpv4VlanTaggedPtr;
/* pointer to predefined IPv4 packet without vlan tag */
extern UT_TRAFFIC_ENGINE_PACKET_STC   *utTestPacketIpv4VlanUnTaggedPtr;

/* types of compares */
typedef enum
{
    UT_TEST_COMPARE_TYPE_NONE_E,              /* no compare , 'UT_TEST_COMPARE_PARAM_STC::value' not used */
    UT_TEST_COMPARE_TYPE_VALUE_E,             /* by value , 'UT_TEST_COMPARE_PARAM_STC::value' is the diff */
    UT_TEST_COMPARE_TYPE_PERCENTAGE_E,        /* by percentage , 'UT_TEST_COMPARE_PARAM_STC::value' is diff in percentage */

    UT_TEST_COMPARE_TYPE_BIGGER_THEN_E,       /* compare 2 values , 'UT_TEST_COMPARE_PARAM_STC::value' not used */
    UT_TEST_COMPARE_TYPE_BIGGER_EQUAL_THEN_E, /* compare 2 values , 'UT_TEST_COMPARE_PARAM_STC::value' not used */
    UT_TEST_COMPARE_TYPE_SMALLER_THEN_E,      /* compare 2 values , 'UT_TEST_COMPARE_PARAM_STC::value' not used */
    UT_TEST_COMPARE_TYPE_SMALLER_EQUAL_THEN_E,/* compare 2 values , 'UT_TEST_COMPARE_PARAM_STC::value' not used */
    UT_TEST_COMPARE_TYPE_EQUAL_TO_E,          /* compare 2 values , 'UT_TEST_COMPARE_PARAM_STC::value' not used */
    UT_TEST_COMPARE_TYPE_NOT_EQUAL_TO_E,      /* compare 2 values , 'UT_TEST_COMPARE_PARAM_STC::value' not used */

    UT_TEST_COMPARE_TYPE_LAST_E
}UT_TEST_COMPARE_TYPES_ENT;

/* types of operators */
typedef enum
{
    UT_TEST_COMPARE_OPERATOR_NONE_E,/* operator not used on next cases:
                                        UT_TEST_COMPARE_TYPE_BIGGER_THEN_E,
                                        UT_TEST_COMPARE_TYPE_BIGGER_EQUAL_THEN_E,
                                        UT_TEST_COMPARE_TYPE_SMALLER_THEN_E,
                                        UT_TEST_COMPARE_TYPE_SMALLER_EQUAL_THEN_E,
                                        UT_TEST_COMPARE_TYPE_EQUAL_TO_E,
                                        UT_TEST_COMPARE_TYPE_NOT_EQUAL_TO_E,
                                    */
    UT_TEST_COMPARE_OPERATOR_DIFF_ABSOLUTE_E,
    UT_TEST_COMPARE_OPERATOR_DIFF_ABOVE_E,
    UT_TEST_COMPARE_OPERATOR_DIFF_BELOW_E
}UT_TEST_COMPARE_OPERATOR_ENT;

/* compare info */
typedef struct{
    UT_TEST_COMPARE_TYPES_ENT   type;/* type of comparator */
    UT_TEST_COMPARE_OPERATOR_ENT operator;/* type of operator */
    GT_U32                      value;/* value relevant to type of compare */
}UT_TEST_COMPARE_PARAM_STC;


/*
 * Typedef: enum UT_TEST_BRG_IP_CTRL_TYPE_ENT
 *
 * Description:
 *      This enum defines IP Control to CPU Enable Mode
 * Fields:
 *     UT_TEST_BRG_IP_CTRL_NONE_E - Disable both IPv4 and Ipv6 Control
 *                                  traffic to CPU
 *     UT_TEST_BRG_IP_CTRL_IPV4_E  - Enable IPv4 Control traffic to CPU
 *     UT_TEST_BRG_IP_CTRL_IPV6_E  - Enable IPv6 Control traffic to CPU
 *     UT_TEST_BRG_IP_CTRL_IPV4_IPV6_E - Enable both IPv4 and IPv6 traffic
 *                                  to CPU
 *
 *  Comments:
 *
 */
typedef enum
{
    UT_TEST_BRG_IP_CTRL_NONE_E,
    UT_TEST_BRG_IP_CTRL_IPV4_E,
    UT_TEST_BRG_IP_CTRL_IPV6_E,
    UT_TEST_BRG_IP_CTRL_IPV4_IPV6_E
} UT_TEST_BRG_IP_CTRL_TYPE_ENT;

/*
 * typedef: struct UT_TEST_BRG_VLAN_INFO_STC
 *
 * Description: A structure to hold generic VLAN entry info.
 *
 * Fields:
 *      unkSrcAddrSecBreach - Unknown Source Address is security breach event.
 *                           GT_TRUE - Packets with unknown MAC Source Addresses
 *                                      generate security breach events.
 *                           GT_FALSE - Packets with unknown MAC Source Addresses
 *                                     assigned to this VLAN do not generate a
 *                                     security breach event and are processed
 *                                     according to the normal Source Address
 *                                     learning rules.
 *      unregNonIpMcastCmd  - The command of Unregistered non IP Multicast packets
 *                            (with a Multicast MAC_DA! = 01-00-5e-xx-xx-xx or
 *                            33-33-xx-xx-xx-xx) assigned to this VLAN.
 *      unregIpv4McastCmd   - The command of Unregistered IPv4 Multicast packets
 *                            (with MAC_DA= 01-00-5e-xx-xx-xx) assigned to
 *                            this VLAN.
 *      unregIpv6McastCmd   - The command of Unregistered IPv6 Multicast packets
 *                            (with MAC_DA= 33-33-xx-xx-xx-xx) assigned to
 *                            this VLAN.
 *      unkUcastCmd         - The command of Unknown Unicast packets assigned
 *                            to this VLAN.
 *      unregIpv4BcastCmd   - The command of Unregistered IPv4 Broadcast packets
 *                            assigned to this VLAN.
 *      unregNonIpv4BcastCmd - The command of Unregistered non-IPv4-Broadcast
 *                             packets assigned to this VLAN.
 *      ipv4IgmpToCpuEn     - Enable IGMP Trapping or Mirroring to CPU according
 *                            to the global setting
 *                            GT_TRUE - enable
 *                            GT_FALSE - disable
 *      mirrToRxAnalyzerEn  - Mirror packets to Rx Analyzer port
 *                            GT_TRUE - Ingress mirrored traffic assigned to
 *                                  this Vlan to the analyzer port
 *                            GT_FALSE - Don't ingress mirrored traffic assigned
 *                                  to this Vlan to the analyzer port
 *      ipv6IcmpToCpuEn     - Enable/Disable ICMPv6 trapping or mirroring to
 *                            the CPU, according to global ICMPv6 message type
 *                            GT_TRUE - enable
 *                            GT_FALSE - disable
 *      ipCtrlToCpuEn       - IPv4/6 control traffic trapping/mirroring to the CPU
 *                            If IPV4 set for this vlan, and the corresponding
 *                            mechanism specific global enable is set and the
 *                            packet is IPV4,then the packet is subject to
 *                            trapping/mirroring for the following packet types:
 *                            1. IP Link local Multicast control check
 *                            2. ARP BC
 *                            3. RIPv1
 *                            If IPv6 set for this vlan and the packet is IPv6,
 *                            then the packet is subject to trapping/mirroring
 *                            for the following packet types:
 *                            1. IP Link local Multicast control check
 *                            2. IPv6 Neighbor Solicitation Multicast
 *      ipv4IpmBrgMode      - IPV4 Multicast Bridging mode
 *                              relevant when IPv4IPM Bridging is enabled
 *      ipv6IpmBrgMode      - IPV6 Multicast Bridging mode
 *                              relevant when IPv6IPM Bridging is enabled
 *      ipv4IpmBrgEn        - IPv4 Multicast Bridging Enable
 *                            GT_TRUE - IPv4 multicast packets are bridged
 *                              according to ipv4IpmBrgMode
 *                            GT_FALSE - IPv4 multicast packets are bridged
 *                              according to MAC DA
 *      ipv6IpmBrgEn        - IPv6 Multicast Bridging Enable
 *                            GT_TRUE - IPv6 multicast packets are bridged
 *                              according to ipv6IpmBrgMode
 *                            GT_FALSE - IPv4 multicast packets are bridged
 *                              according to MAC DA
 *      ipv6SiteIdMode      - IPv6 Site Id Mode
 *                            Used by the router for IPv6 scope checking and
 *                            supported for DxCh2 and above devices only
 *      ipv4UcastRouteEn    - Enable/Disable IPv4 Unicast Routing on the vlan
 *                              GT_TRUE - enable,
 *                              GT_FALSE - disable
 *      ipv4McastRouteEn    - Enable/Disable IPv4 Multicast Routing on the vlan
 *                              GT_TRUE - enable,
 *                              GT_FALSE - disable
 *                             supported for DxCh2 and above devices only
 *      ipv6UcastRouteEn    - Enable/Disable IPv6 Unicast Routing on the vlan
 *                              GT_TRUE - enable,
 *                              GT_FALSE - disable
 *      ipv6McastRouteEn    - Enable/Disable IPv6 Multicast Routing on the vlan
 *                              GT_TRUE - enable,
 *                              GT_FALSE - disable
 *                             supported for DxCh2 and above devices only
 *      stgId               - stp group, range 0-255
 *      autoLearnDisable    - Enable/Disable automatic learning for this VLAN
 *                              GT_TRUE - Disable automatic learning
 *                              GT_FALSE - Enable automatic learning
 *                            supported for DxCh2 and above devices only
 *      naMsgToCpuEn        - Enable/Disable new address message sending to CPU
 *                              GT_TRUE - enable new address message to CPU
 *                              GT_FALSE - disable new address message to CPU
 *                            supported for DxCh2 and above devices only
 *      mruIdx              - The index that this Vlan's MRU configuration packets
 *                              are associated with (0..7)
 *                            supported for DxCh2 and above devices only
 *      bcastUdpTrapMirrEn  - Enables trapping/mirroring of Broadcast UDP packets
 *                              based on their destination UDP port
 *                              GT_TRUE - enable,
 *                              GT_FALSE - disable
 *                            supported for DxCh2 and above devices only
 *      vrfId               - Virtual Router ID
 *                            (included in the Router TCAM lookup key)
 *                            supported for CH3 devices only
 */
typedef struct
{
    GT_BOOL                                 unkSrcAddrSecBreach;
    CPSS_PACKET_CMD_ENT                     unregNonIpMcastCmd;
    CPSS_PACKET_CMD_ENT                     unregIpv4McastCmd;
    CPSS_PACKET_CMD_ENT                     unregIpv6McastCmd;
    CPSS_PACKET_CMD_ENT                     unkUcastCmd;
    CPSS_PACKET_CMD_ENT                     unregIpv4BcastCmd;
    CPSS_PACKET_CMD_ENT                     unregNonIpv4BcastCmd;
    GT_BOOL                                 ipv4IgmpToCpuEn;
    GT_BOOL                                 mirrToRxAnalyzerEn;
    GT_BOOL                                 ipv6IcmpToCpuEn;
    UT_TEST_BRG_IP_CTRL_TYPE_ENT            ipCtrlToCpuEn;
    CPSS_BRG_IPM_MODE_ENT                   ipv4IpmBrgMode;
    CPSS_BRG_IPM_MODE_ENT                   ipv6IpmBrgMode;
    GT_BOOL                                 ipv4IpmBrgEn;
    GT_BOOL                                 ipv6IpmBrgEn;
    CPSS_IP_SITE_ID_ENT                     ipv6SiteIdMode;
    GT_BOOL                                 ipv4UcastRouteEn;
    GT_BOOL                                 ipv4McastRouteEn;
    GT_BOOL                                 ipv6UcastRouteEn;
    GT_BOOL                                 ipv6McastRouteEn;
    GT_U32                                  stgId;
    GT_BOOL                                 autoLearnDisable;
    GT_BOOL                                 naMsgToCpuEn;
    GT_U32                                  mruIdx;
    GT_BOOL                                 bcastUdpTrapMirrEn;
    GT_U32                                  vrfId;
}UT_TEST_BRG_VLAN_INFO_STC;

/*
 * typedef: enum UT_TEST_BRG_VLAN_PORT_TAG_CMD_ENT
 *
 * Description: Enumeration of tagging egress commands.
 *
 * Enumerations:
 *    UT_TEST_BRG_VLAN_PORT_UNTAGGED_CMD_E              - if Tag0 and/or Tag1
 *                                                        were classified in 
 *                                                        the incoming packet,
 *                                                        they are removed 
 *                                                        from the packet.
 *    UT_TEST_BRG_VLAN_PORT_TAG0_CMD_E                  - packet egress with
 *                                                        Tag0 (Tag0 as 
 *                                                        defined in ingress
 *                                                        pipe).
 *    UT_TEST_BRG_VLAN_PORT_TAG1_CMD_E                  - packet egress with
 *                                                        Tag1 (Tag1 as 
 *                                                        defined in ingress
 *                                                        pipe).
 *    UT_TEST_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E - Outer Tag0, Inner
 *                                                        Tag1 (tag swap).
 *    UT_TEST_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E - Outer Tag1, Inner
 *                                                        Tag0 (tag swap).
 *    UT_TEST_BRG_VLAN_PORT_PUSH_TAG0_CMD_E             - TAG0 is added to 
 *                                                        the packet 
 *                                                        regardless of 
 *                                                        whether Tag0 and 
 *                                                        TAG1 were 
 *                                                        classified in the
 *                                                        incoming packet.
 *    UT_TEST_BRG_VLAN_PORT_POP_OUTER_TAG_CMD_E         - the incoming packet
 *                                                        outer tag is 
 *                                                        removed, regardless
 *                                                        of whether it is 
 *                                                        Tag0 or Tag1. This
 *                                                        operation is a NOP
 *                                                        if the packet 
 *                                                        arrived with neither
 *                                                        Tag0 nor Tag1 
 *                                                        classified.
 * Note:
 *      Relevant for devices with TR101 feature support.
 */
typedef enum
{
   UT_TEST_BRG_VLAN_PORT_UNTAGGED_CMD_E,
   UT_TEST_BRG_VLAN_PORT_TAG0_CMD_E,
   UT_TEST_BRG_VLAN_PORT_TAG1_CMD_E,
   UT_TEST_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E,
   UT_TEST_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E,
   UT_TEST_BRG_VLAN_PORT_PUSH_TAG0_CMD_E,
   UT_TEST_BRG_VLAN_PORT_POP_OUTER_TAG_CMD_E
}UT_TEST_BRG_VLAN_PORT_TAG_CMD_ENT;


/*
 * Typedef: structure UT_TEST_BRG_VLAN_PORTS_TAG_CMD_STC
 *
 * Description: structure that hold an array of ports' Tagging commands
 *
 * Fields:
 *      portsCmd - array of ports' tagging commands
 *
 * Note:
 *      Array index is port number.
 */
typedef struct{
    UT_TEST_BRG_VLAN_PORT_TAG_CMD_ENT portsCmd[UT_TEST_MAX_PORTS_NUM_CNS];
}UT_TEST_BRG_VLAN_PORTS_TAG_CMD_STC;

/*
 * typedef: enum UT_TEST_TUNNEL_START_QOS_MARK_MODE_ENT
 *
 * Description: Enumeration of tunnel start entry QoS mark modes.
 *
 * Enumerations:
 *
 *  UT_TEST_TUNNEL_START_QOS_MARK_FROM_ENTRY_E -
 *      Mark based on the explicit QoS fields in Tunnel Start entry.
 *
 *  UT_TEST_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E -
 *      Mark based on the packet QoS Profile assignment. The QoS Profile table
 *      entry provides the tunnel header QoS fields.
 *
 */
typedef enum
{
    UT_TEST_TUNNEL_START_QOS_MARK_FROM_ENTRY_E                = 0,
    UT_TEST_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E   = 1

} UT_TEST_TUNNEL_START_QOS_MARK_MODE_ENT;

/*
 * typedef: struct UT_TEST_TUNNEL_START_IPV4_CONFIG_STC
 *
 * Description: Struct of configuration for X-over-IPv4 tunnel start.
 *
 * Fields:
 *
 *  tagEnable         - If set, the packet is sent with VLAN tag
 *  vlanId            - Relevant only if entry <tagEnable> is set. This
 *                      field is the VID in the Tunnel VLAN tag (0..4095)
 *  upMarkMode        - Relevant only if entry <tagEnable> is set.
 *                      Options:
 *                      - Set user priority according to entry <802.1p UP>.
 *                      - Set user priority according to packet assigned QoS attributes
 *  up                - The tag 802.1p user priority (0..7)
 *                      Relevant only if entry <tagEnable> is set and entry
 *                      <upMarkMode> is set according to entry <802.1p UP>.
 *  dscpMarkMode      - Options:
 *                      - Set DSCP according to entry <DSCP>.
 *                      - Set DSCP according to packet QoS Profile assignment.
 *  dscp              - The DSCP in the IPv4 tunnel header (0..63)
 *                      Relevant only if entry <dscpMarkMode> is according
 *                      to entry <DSCP>.
 *  macDa             - Tunnel next hop MAC DA
 *  dontFragmentFlag  - Don't fragment flag in the tunnel IPv4 header
 *  ttl               - tunnel IPv4 header TTL (0..255)
 *                          0:     use passenger packet TTL
 *                          1-255: use this field for header TTL
 *  autoTunnel        - Relevant for IPv6-over-IPv4 and IPv6-over-GRE-IPv4 only,
 *                      if GT_TRUE, the IPv4 header DIP is derived from IPv6
 *                      passenger packet and destIp field is ignored
 *  autoTunnelOffset  - Relevant for IPv6-over-IPv4 or IPv6-over-GRE-IPv4 only (0..15)
 *                      If <autoTunnel> is GT_TRUE, this field is the offset
 *                      of IPv4 destination address inside IPv6 destination
 *                      address. Offset is measured in bytes between LSBs of
 *                      the addresses:
 *                          1. IPv4 compatible & ISATAP = 0
 *                          2. 6to4 = 10 (decimal)
 *  destIp            - Tunnel destination IP
 *  srcIp             - Tunnel source IP
 *  cfi               - CFI bit assigned for TS packets. APPLICABLE DEVICES: DxCh3 and above.
 */
typedef struct
{
    GT_BOOL         tagEnable;
    GT_U16          vlanId;
    UT_TEST_TUNNEL_START_QOS_MARK_MODE_ENT    upMarkMode;
    GT_U32          up;
    UT_TEST_TUNNEL_START_QOS_MARK_MODE_ENT    dscpMarkMode;
    GT_U32          dscp;
    GT_ETHERADDR    macDa;
    GT_BOOL         dontFragmentFlag;
    GT_U32          ttl;
    GT_BOOL         autoTunnel;
    GT_U32          autoTunnelOffset;
    GT_IPADDR       destIp;
    GT_IPADDR       srcIp;
    GT_U32          cfi;

} UT_TEST_TUNNEL_START_IPV4_CONFIG_STC;


/*
 * typedef: struct UT_TEST_TUNNEL_START_MPLS_CONFIG_STC
 *
 * Description: Struct of configuration for X-over-MPLS tunnel start
 *
 * Fields:
 *
 *  tagEnable         - If set, the packet is sent with VLAN tag
 *  vlanId            - Relevant only if entry <tagEnable> is set. This
 *                      field is the VID in the Tunnel VLAN tag (0..4095)
 *  upMarkMode        - Relevant only if entry <tagEnable> is set.
 *                      Options:
 *                      - Set user priority according to entry <802.1p UP>.
 *                      - Set user priority according to packet assigned QoS attributes
 *  up                - The tag 802.1p user priority (0..7)
 *                      Relevant only if entry <tagEnable> is set and entry
 *                      <upMarkMode> is set according to entry <802.1p UP>.
 *  macDa             - Tunnel next hop MAC DA
 *  numLabels         - number of MPLS labels (1 or 2)
 *  ttl               - tunnel IPv4 header TTL (0..255)
 *                          0:     use passenger packet TTL
 *                          1-255: use this field for header TTL
 *  label1            - Relevant only when number of labels is 2; (0..1048575)
 *                      MPLS label 1; if <numLabels> = 2 then this is the
 *                      inner label
 *  exp1MarkMode      - Relevant only when number of labels is 2;
 *                      Options:
 *                      - Set EXP1 according to entry <exp1>.
 *                      - Set EXP1 according to packet QoS Profile assignment
 *                        to EXP mapping table.
 *  exp1              - The EXP1 value (0..7)
 *                      Relevant only when number of labels is 2 and entry
 *                      <exp1MarkMode> is set according to entry <EXP1>.
 *  label2            - MPLS label 2; (0..1048575) if <numLabels> = 2 then
 *                      this is the outer label, immediately following the MPLS
 *                      Ether Type
 *  exp2MarkMode      - Options:
 *                      - Set EXP2 according to entry <exp2>.
 *                      - Set EXP2 according to packet QoS Profile assignment
 *                        to EXP mapping table.
 *  exp2              - The EXP2 value (0..7)
 *                      Relevant if entry <exp2MarkMode> is set to according
 *                      to entry <EXP2>.
 *  retainCRC         - Relevant for Ethernet-over-MPLS. If GT_TRUE, retain
 *                      the passenger packet CRC and add additional 4-byte CRC
 *                      based on the tunneling header packet.
 *  cfi               - CFI bit assigned for TS packets. APPLICABLE DEVICES: DxCh3 and above.
 */
typedef struct
{
    GT_BOOL         tagEnable;
    GT_U16          vlanId;
    UT_TEST_TUNNEL_START_QOS_MARK_MODE_ENT    upMarkMode;
    GT_U32          up;
    GT_ETHERADDR    macDa;
    GT_U32          numLabels;
    GT_U32          ttl;
    GT_U32          label1;
    UT_TEST_TUNNEL_START_QOS_MARK_MODE_ENT    exp1MarkMode;
    GT_U32          exp1;
    GT_U32          label2;
    UT_TEST_TUNNEL_START_QOS_MARK_MODE_ENT    exp2MarkMode;
    GT_U32          exp2;
    GT_BOOL         retainCRC;
    GT_U32          cfi;

} UT_TEST_TUNNEL_START_MPLS_CONFIG_STC;

/*
 * typedef: union UT_TEST_TUNNEL_START_CONFIG_UNT
 *
 * Description: Union for configuration for tunnel start (X-over-MPLS tunnel
 *              start or X-over-IPv4 tunnel start).
 *
 * Fields:
 *
 *  ipv4Cfg           - configuration for X-over-IPv4 tunnel start
 *  mplsCfg           - configuration for X-over-MPLS tunnel start
 *
 */
typedef union
{
    UT_TEST_TUNNEL_START_IPV4_CONFIG_STC  ipv4Cfg;
    UT_TEST_TUNNEL_START_MPLS_CONFIG_STC  mplsCfg;

} UT_TEST_TUNNEL_START_CONFIG_UNT;


/*
 * typedef: enum UT_TEST_IP_CNT_SET_MODE_ENT
 *
 * Description: the counter sets modes
 *
 * Enumerations:
 *  UT_TEST_IP_CNT_SET_INTERFACE_MODE_E - Interface counter, the counter will
 *                                   count all packets bounded to an interface
 *                                   according to the counter set bounded
 *                                   inteface configuration.
 *  UT_TEST_IP_CNT_SET_ROUTE_ENTRY_MODE_E - Route Entry counter, the counter
 *                                   counts all packets which are binded to it
 *                                   by the next hops (UC & MC)
 */
typedef enum
{
    UT_TEST_IP_CNT_SET_INTERFACE_MODE_E    = 0,
    UT_TEST_IP_CNT_SET_ROUTE_ENTRY_MODE_E      = 1
}UT_TEST_IP_CNT_SET_MODE_ENT;

/*
 * typedef: enum UT_TEST_IP_PORT_TRUNK_CNT_MODE_ENT
 *
 * Description: the counter Set Port/Trunk mode
 *
 * Enumerations:
 *  UT_TEST_IP_DISREGARD_PORT_TRUNK_CNT_MODE_E - This counter-set counts all
 *                                                 packets regardless of their
 *                                                 In / Out port or Trunk.
 *  UT_TEST_IP_PORT_CNT_MODE_E - This counter-set counts packets Received /
 *                                 Transmitted via binded Port + Dev.
 *  UT_TEST_IP_TRUNK_CNT_MODE_E- This counter-set counts packets Received /
 *                                 Transmitted via binded Trunk.
 */
typedef enum
{
    UT_TEST_IP_DISREGARD_PORT_TRUNK_CNT_MODE_E = 0,
    UT_TEST_IP_PORT_CNT_MODE_E                 = 1,
    UT_TEST_IP_TRUNK_CNT_MODE_E                = 2
}UT_TEST_IP_PORT_TRUNK_CNT_MODE_ENT;

/*
 * typedef: enum UT_TEST_IP_VLAN_CNT_MODE_ENT
 *
 * Description: the counter Set vlan mode
 *
 * Enumerations:
 *  UT_TEST_IP_DISREGARD_VLAN_CNT_MODE_E - This counter-set counts all packets
 *                                           regardless of their In / Out vlan.
 *  UT_TEST_IP_USE_VLAN_CNT_MODE_E - This counter-set counts packets Received
 *                                     / Transmitted via binded vlan.
 */
typedef enum
{
    UT_TEST_IP_DISREGARD_VLAN_CNT_MODE_E = 0,
    UT_TEST_IP_USE_VLAN_CNT_MODE_E       = 1
}UT_TEST_IP_VLAN_CNT_MODE_ENT;

/*
 * Typedef: struct UT_TEST_IP_COUNTER_SET_INTERFACE_CFG_STC
 *
 * Description: IPvX Counter Set interface mode configuration
 *
 * Fields:
 *      portTrunkCntMode - the counter Set Port/Trunk mode
 *      ipMode           - the cnt set IP interface mode , which protocol stack
 *                         ipv4 ,ipv6 or both (which is actully disregarding the
 *                         ip protocol)
 *      vlanMode         - the counter Set vlan mode.
 *      devNum           - the binded devNum , relevant only if
 *                         portTrunkCntMode = UT_TEST_IP_PORT_CNT_MODE_E
 *      port             - the binded por , relevant only if
 *                         portTrunkCntMode = UT_TEST_IP_PORT_CNT_MODE_E
 *      trunk            - the binded trunk , relevant only if
 *                         portTrunkCntMode = UT_TEST_IP_TRUNK_CNT_MODE_E
 *      portTrunk        - the above port/trunk
 *      vlanId           - the binded vlan , relevant only if
 *                         vlanMode = UT_TEST_IP_USE_VLAN_CNT_MODE_E
 */
typedef struct UT_TEST_IP_COUNTER_SET_INTERFACE_CFG_STCT
{
    UT_TEST_IP_PORT_TRUNK_CNT_MODE_ENT portTrunkCntMode;
    CPSS_IP_PROTOCOL_STACK_ENT           ipMode;
    UT_TEST_IP_VLAN_CNT_MODE_ENT       vlanMode;
    GT_U8                                devNum;
    union
    {
        GT_U8                            port;
        GT_TRUNK_ID                      trunk;
    }portTrunk;
    GT_U16                               vlanId;
}UT_TEST_IP_COUNTER_SET_INTERFACE_CFG_STC;

/*
 * typedef: enum UT_TEST_IP_CPU_CODE_INDEX_ENT
 *
 * Description:  this value is added to the CPU code assignment in corse of trap
 *               or mirror
 *
 * Enumerations:
 *  UT_TEST_IP_CPU_CODE_IDX_0_E - CPU code added index 0
 *  UT_TEST_IP_CPU_CODE_IDX_1_E - CPU code added index 1
 *  UT_TEST_IP_CPU_CODE_IDX_2_E - CPU code added index 2
 *  UT_TEST_IP_CPU_CODE_IDX_3_E - CPU code added index 3
 */
typedef enum
{
    UT_TEST_IP_CPU_CODE_IDX_0_E   = 0,
    UT_TEST_IP_CPU_CODE_IDX_1_E   = 1,
    UT_TEST_IP_CPU_CODE_IDX_2_E   = 2,
    UT_TEST_IP_CPU_CODE_IDX_3_E   = 3
}UT_TEST_IP_CPU_CODE_INDEX_ENT;

/*
 * Typedef: struct UT_TEST_IP_UC_ROUTE_ENTRY_FORMAT_STC
 *
 * Description: Unicast Route Entry
 *
 * Fields:
 *  cmd                - Route entry command. supported commands:
 *                          CPSS_PACKET_CMD_ROUTE_E,
 *                          CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E,
 *                          CPSS_PACKET_CMD_TRAP_TO_CPU_E,
 *                          CPSS_PACKET_CMD_DROP_SOFT_E ,
 *                          CPSS_PACKET_CMD_DROP_HARD_E.
 *  cpuCodeIdx         - the cpu code index of the specific code when trap or
 *                       mirror.
 *  appSpecificCpuCodeEnable- Enable CPU code overwritting according to the
 *                        application specific CPU Code assignment mechanism.
 *  unicastPacketSipFilterEnable - Enable: Assign the command HARD DROP if the
 *                       SIP lookup matches this entry
 *                       Disable: Do not assign the command HARD DROP due to
 *                       the SIP lookup matching this entry
 *  ttlHopLimitDecEnable - Enable TTL/Hop Limit Decrement
 *  ttlHopLimDecOptionsExtChkByPass- TTL/HopLimit Decrement and option/
 *                       Extention check bypass.
 *  ingressMirror      - mirror to ingress analyzer.
 *  qosProfileMarkingEnable- Enable Qos profile assignment.
 *  qosProfileIndex    - the qos profile to assign in case of
 *                       qosProfileMarkingEnable = GT_TRUE
 *  qosPrecedence      - whether this packet Qos parameters can be overridden
 *                       after this assigment.
 *  modifyUp           - whether to change the packets UP and how.
 *  modifyDscp         - whether to change the packets DSCP and how.
 *  countSet           - The counter set this route entry is linked to
 *  trapMirrorArpBcEnable - enable Trap/Mirror ARP Broadcasts with DIP matching
 *                       this entry
 *  sipAccessLevel     - The security level associated with the SIP.
 *  dipAccessLevel     - The security level associated with the DIP.
 *  ICMPRedirectEnable - Enable performing ICMP Redirect Exception Mirroring.
 *  scopeCheckingEnable- Enable IPv6 Scope Checking.
 *  siteId             - The site id of this route entry.
 *  mtuProfileIndex    - One of the eight global configurable MTU sizes (0..7).
 *  isTunnelStart      - weather this nexthop is tunnel start enrty, in which
 *                       case the outInteface & mac are irrlevant and the tunnel
 *                       id is used.
 *  nextHopVlanId      - the output vlan id (used also for SIP RPF check, and
 *                       ICMP check)
 *  nextHopInterface   - the output interface this next hop sends to. relevant
 *                       only if the isTunnelStart = GT_FALSE
 *  nextHopARPPointer  - The ARP Pointer indicating the routed packet MAC DA,
 *                       relevant only if the isTunnelStart = GT_FALSE
 *  nextHopTunnelPointer- the tunnel pointer in case this is a tunnel start
 *                       isTunnelStart = GT_TRUE
 *
 */
typedef struct
{
    CPSS_PACKET_CMD_ENT                 cmd;
    UT_TEST_IP_CPU_CODE_INDEX_ENT       cpuCodeIdx;
    GT_BOOL                             appSpecificCpuCodeEnable;
    GT_BOOL                             unicastPacketSipFilterEnable;
    GT_BOOL                             ttlHopLimitDecEnable;
    GT_BOOL                             ttlHopLimDecOptionsExtChkByPass;
    GT_BOOL                             ingressMirror;
    GT_BOOL                             qosProfileMarkingEnable;
    GT_U32                              qosProfileIndex;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT qosPrecedence;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT       modifyUp;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT       modifyDscp;
    CPSS_IP_CNT_SET_ENT                 countSet;
    GT_BOOL                             trapMirrorArpBcEnable;
    GT_U32                              sipAccessLevel;
    GT_U32                              dipAccessLevel;
    GT_BOOL                             ICMPRedirectEnable;
    GT_BOOL                             scopeCheckingEnable;
    CPSS_IP_SITE_ID_ENT                 siteId;
    GT_U32                              mtuProfileIndex;
    GT_BOOL                             isTunnelStart;
    GT_U16                              nextHopVlanId;
    CPSS_INTERFACE_INFO_STC             nextHopInterface;
    GT_U32                              nextHopARPPointer;
    GT_U32                              nextHopTunnelPointer;

}UT_TEST_IP_UC_ROUTE_ENTRY_FORMAT_STC;

/*
 * Typedef: struct UT_TEST_IP_UC_ECMP_RPF_FORMAT_STC
 *
 * Description: Unicast ECMP/QoS RPF check route entry format (supported by Ch3
 *              and above only)
 *
 * Fields:
 *      vlanArray     - array of vlan Ids that are compared against the
 *                      packet vlan as part of the unicast RPF check
 *
 */
typedef struct
{
    GT_U16      vlanArray[8];

}UT_TEST_IP_UC_ECMP_RPF_FORMAT_STC;


/*
 * typedef: struct UT_TEST_IP_UC_ROUTE_ENTRY_UNT
 *
 * Description:
 *      Holds the two possible unicast route entry format.
 *
 * Fields:
 *  regularEntry      - regular route entry format
 *  ecmpRpfCheck      - special route entry format for unicast ECMP/QOS RPF
 *                      check (supported by Ch3 and above only)
 */
typedef union UT_TEST_IP_UC_ROUTE_ENTRY_UNTT
{
    UT_TEST_IP_UC_ROUTE_ENTRY_FORMAT_STC     regularEntry;
    UT_TEST_IP_UC_ECMP_RPF_FORMAT_STC        ecmpRpfCheck;

}UT_TEST_IP_UC_ROUTE_ENTRY_UNT;

/*
 * typedef: enum UT_TEST_IP_UC_ROUTE_ENTRY_TYPE_ENT (type)
 *
 * Description: the type of uc route entry
 *
 * Enumerations:
 *      UT_TEST_IP_UC_ROUTE_ENTRY_E - regular route entry format
 *      UT_TEST_IP_UC_ECMP_RPF_E    - special route entry format for
 *                                      unicast ECMP/QOS RPF check (supported
 *                                      by Ch3 and above only)
 */
typedef enum
{
    UT_TEST_IP_UC_ROUTE_ENTRY_E   = 0,
    UT_TEST_IP_UC_ECMP_RPF_E      = 1
}UT_TEST_IP_UC_ROUTE_ENTRY_TYPE_ENT;

/*
 * Typedef: struct UT_TEST_IP_UC_ROUTE_ENTRY_STC
 *
 * Description: Unicast route entry
 *
 * Fields:
 *      type      - type of the route entry (refer to UT_TEST_IP_UC_ROUTE_ENTRY_TYPE_ENT)
 *      entry     - the route entry information
 *
 */
typedef struct
{
    UT_TEST_IP_UC_ROUTE_ENTRY_TYPE_ENT    type;
    UT_TEST_IP_UC_ROUTE_ENTRY_UNT         entry;

}UT_TEST_IP_UC_ROUTE_ENTRY_STC;

/*
 * Typedef: struct UT_TEST_PCL_ACTION_MIRROR_STC
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
    GT_BOOL                       mirrorToRxAnalyzerPort;
} UT_TEST_PCL_ACTION_MIRROR_STC;

/*
 * Typedef: struct UT_TEST_PCL_ACTION_MATCH_COUNTER_STC
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
    GT_BOOL   enableMatchCount;
    GT_U32    matchCounterIndex;
} UT_TEST_PCL_ACTION_MATCH_COUNTER_STC;

/*
 * Typedef: struct UT_TEST_PCL_ACTION_QOS_STC
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
 *           Egress Policy related actions supported only for DxCh2 and above
 *           devices
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
            GT_U32                                      profileIndex;
            GT_BOOL                                     profileAssignIndex;
            CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT profilePrecedence;
        } ingress;
        struct
        {
            GT_U8                              dscp;
            GT_U8                              up;
        } egress;
    } qos;
} UT_TEST_PCL_ACTION_QOS_STC;

/*
 * Typedef: enum UT_TEST_PCL_ACTION_REDIRECT_CMD_ENT
 *
 * Description: enumerator for PCL redirection target
 *
 * Fields:
 *  UT_TEST_PCL_ACTION_REDIRECT_CMD_NONE_E   - no redirection
 *  UT_TEST_PCL_ACTION_REDIRECT_CMD_OUT_IF_E - redirection to output interface
 *  UT_TEST_PCL_ACTION_REDIRECT_CMD_ROUTER_E - Ingress control pipe not
 *                        bypassed. If packet is triggered for routing,
 *                        Redirect the packet to the Router Lookup Translation
 *                        Table Entry specified in <routerLttIndex> bypassing
 *                        DIP lookup.
 *  UT_TEST_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E - redirect to virtual
 *                        router with the specified Id (VRF ID)
 */
typedef enum
{
    UT_TEST_PCL_ACTION_REDIRECT_CMD_NONE_E,
    UT_TEST_PCL_ACTION_REDIRECT_CMD_OUT_IF_E,
    UT_TEST_PCL_ACTION_REDIRECT_CMD_ROUTER_E,
    UT_TEST_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E
} UT_TEST_PCL_ACTION_REDIRECT_CMD_ENT;

/*
 * Typedef: enum UT_TEST_PCL_ACTION_REDIRECT_TARGET_TYPE_ENT
 *
 * Description: enumerator for PCL redirection target type
 *
 * Fields:
 *  UT_TEST_PCL_ACTION_REDIRECT_TARGET_UNICAST_E   - unicast
 *  UT_TEST_PCL_ACTION_REDIRECT_TARGET_MULTICAST_E - multicast
 *
 */
typedef enum
{
    UT_TEST_PCL_ACTION_REDIRECT_TARGET_UNICAST_E,
    UT_TEST_PCL_ACTION_REDIRECT_TARGET_MULTICAST_E
} UT_TEST_PCL_ACTION_REDIRECT_TARGET_TYPE_ENT;

/*
 * Typedef: enum UT_TEST_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ENT
 *
 * Description: enumerator for PCL redirection types of the passenger packet.
 *
 * Fields:
 *  UT_TEST_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ETHERNET_E - The passenger packet
 *                                                         is Ethernet.
 *  UT_TEST_PCL_ACTION_REDIRECT_TUNNEL_TYPE_IP_E - The passenger packet is IP.
 *
 */

typedef enum
{
    UT_TEST_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ETHERNET_E,
    UT_TEST_PCL_ACTION_REDIRECT_TUNNEL_TYPE_IP_E

}UT_TEST_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ENT;
/*
 * Typedef: struct UT_TEST_PCL_ACTION_REDIRECT_STC
 *
 * Description:
 *     This structure defines the redirection related Actions.
 *
 * Fields:
 *       redirectCmd   - redirection command
 *
 *       data.outIf   - out interface redirection data
 *                      relevant for UT_TEST_PCL_ACTION_REDIRECT_CMD_IF_E
 *                      packet redirected to output interface
 *       outInterface - output interface (port, trunk, VID, VIDX)
 *       vntL2Echo    - Enables Virtual Network Tester Layer 2 Echo
 *                      GT_TRUE - swap MAC SA and MAC DA in redirected packet
 *                      GT_FALSE - don't swap MAC SA and MAC DA
 *       tunnelType   -  tunnel type - Etrenet or IP,
 *                       Supported only DXCH3 and above.
 *                       DxCh1 and DxCh2 ignores the field.
 *       tunnelStart  - Indicates this action is a Tunnel Start point
 *                      GT_TRUE - Packet is redirected to an Egress Interface
 *                                and is tunneled as an  Ethernet-over-MPLS.
 *                      GT_FALSE - Packet is redirected to an Egress Interface
 *                                and is not tunneled.
 *       tunnelPtr    - the pointer to "Tunnel-start" entry
 *                      (relevant only if tunnelStart == GT_TRUE)
 *       interfaceType  - The type of the egress interface either multicast or
 *                        unicast
 *                       Supported only DXCH3 and above.
 *                       DxCh1 and DxCh2 ignores the field.
 *
 *       data.routerLttIndex  - index of IP router Lookup Translation Table entry
 *                       relevant for UT_TEST_PCL_ACTION_REDIRECT_CMD_ROUTER_E
 *       data.vrfId   - virtual router ID
 *                 relevant for UT_TEST_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E
 *                       Supported only DXCH3 and above.
 *                       DxCh1 and DxCh2 ignores the field.
 *
 *
 *  Comments:
 *      Tunnel, VNTL2Echo and Router redirection related actions
 *      supported only for DxCh2 and above devices
 *
 */
typedef struct
{
    UT_TEST_PCL_ACTION_REDIRECT_CMD_ENT  redirectCmd;
    union
    {
        struct
        {
            CPSS_INTERFACE_INFO_STC  outInterface;
            GT_BOOL                  vntL2Echo;
            GT_BOOL                  tunnelStart;
            GT_U32                   tunnelPtr;
            UT_TEST_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ENT tunnelType;
            UT_TEST_PCL_ACTION_REDIRECT_TARGET_TYPE_ENT interfaceType;
        } outIf;
        GT_U32                   routerLttIndex;
        GT_U32                                            vrfId;
    } data;

} UT_TEST_PCL_ACTION_REDIRECT_STC;


/*
 * Typedef: struct UT_TEST_PCL_ACTION_POLICER_STC
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
    GT_BOOL  policerEnable;
    GT_U32   policerId;

} UT_TEST_PCL_ACTION_POLICER_STC;

/*
 * Typedef: struct UT_TEST_PCL_ACTION_VLAN_STC
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
    GT_BOOL                                       nestedVlan;
    GT_U16                                        vlanId;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT   precedence;

} UT_TEST_PCL_ACTION_VLAN_STC;

/*
 * Typedef: struct UT_TEST_PCL_ACTION_IP_UC_ROUTE_STC
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
 *  Supported for DxCh only (not supported for DxCh2 and above).
 *  To configure IP Unicast route entry next elements of the action struct
 *  should be configured
 *    1. doIpUcRoute set to GT_TRUE
 *    2. redirection action set to UT_TEST_PCL_ACTION_REDIRECT_CMD_OUT_IF_E
 *      Out Interface data should be configured too.
 *    3. Policer should be disabled
 *    4. VLAN command UT_TEST_PCL_ACTION_VLAN_CMD_MODIFY_ALL_E
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
    GT_BOOL  doIpUcRoute;
    GT_U32   arpDaIndex;
    GT_BOOL  decrementTTL;
    GT_BOOL  bypassTTLCheck;
    GT_BOOL  icmpRedirectCheck;

} UT_TEST_PCL_ACTION_IP_UC_ROUTE_STC;

/*
 * Typedef: struct UT_TEST_PCL_ACTION_SOURCE_ID_STC
 *
 * Description:
 *     This structure defines packet Source Id assignment.
 *
 * Fields:
 *     assignSourceId  - assign Source Id enable:
 *                     - GT_TRUE  - Assign Source Id.
 *                     - GT_FALSE - Don't assign Source Id.
 *     sourceIdValue   - the Source Id value to be assigned
 *
 *  Comments:
 *
 */
typedef struct
{
    GT_BOOL  assignSourceId;
    GT_U32   sourceIdValue;
} UT_TEST_PCL_ACTION_SOURCE_ID_STC;

/*
 * typedef: struct UT_TEST_PCL_ACTION_STC
 *
 * Description:
 *      Policy Engine Action
 *
 * Fields:
 *      pktCmd       -  packet command (forward, mirror-to-cpu,
 *                      hard-drop, soft-drop, or trap-to-cpu)
 *      actionStop   - Action Stop
 *                     GT_TRUE  - to skip the following PCL lookups
 *                     GT_FALSE - to continue with following PCL lookups
 *                     Supported by DxCh3 and above.
 *                     DxCh1 and DxCh2 ignores the field.
 *      egressPolicy - GT_TRUE  - Action is used for the Egress Policy
 *                     GT_FALSE - Action is used for the Ingress Policy
 *      mirror       -  packet mirroring configuration
 *      matchCounter -  match counter configuration
 *      qos          -  packet QoS attributes modification configuration
 *      redirect     -  packet Policy redirection configuration
 *      policer      -  packet Policing configuration
 *      vlan         -  packet VLAN modification configuration
 *      ipUcRoute    -  special DxCh (not relevant for DxCh2 and above) Ip Unicast Route
 *                      action parameters configuration
 *      sourceId     -  packet source Id assignment
 *
 * Comment:
 */
typedef struct
{
    CPSS_PACKET_CMD_ENT                    pktCmd;
    GT_BOOL                                actionStop;
    GT_BOOL                                egressPolicy;
    UT_TEST_PCL_ACTION_MIRROR_STC        mirror;
    UT_TEST_PCL_ACTION_MATCH_COUNTER_STC matchCounter;
    UT_TEST_PCL_ACTION_QOS_STC           qos;
    UT_TEST_PCL_ACTION_REDIRECT_STC      redirect;
    UT_TEST_PCL_ACTION_POLICER_STC       policer;
    UT_TEST_PCL_ACTION_VLAN_STC          vlan;
    UT_TEST_PCL_ACTION_IP_UC_ROUTE_STC   ipUcRoute;
    UT_TEST_PCL_ACTION_SOURCE_ID_STC     sourceId;
} UT_TEST_PCL_ACTION_STC;

/*
 * typedef: enum UT_TEST_IP_ROUTE_ENTRY_METHOD_ENT (type)
 *
 * Description: Each Look Up Translation Entry may point to a Route/s entry/ies
 *              Each (block) route entry block has a type.
 *
 * Enumerations:
 *      UT_TEST_IP_ECMP_ROUTE_ENTRY_GROUP_E - Equal-Cost-MultiPath block,
 *          where traffic is split among of the route entries according to a
 *          hash function. regular route entries are consider ECMP type with
 *          size of 1!
 *      UT_TEST_IP_QOS_ROUTE_ENTRY_GROUP_E - Quality-of-Service block, where
 *          the route entry is according to the QoSProfile-to-Route-Block-Offset
 *          Table.
 */
typedef enum
{
    UT_TEST_IP_ECMP_ROUTE_ENTRY_GROUP_E = 0,
    UT_TEST_IP_QOS_ROUTE_ENTRY_GROUP_E = 1
}UT_TEST_IP_ROUTE_ENTRY_METHOD_ENT;

/*
 * Typedef: struct UT_TEST_IP_LTT_ENTRY_STC
 *
 * Description: LTT (lookup translation table) entry
 *
 * Fields:
 *      routeType              - Route Entry Type.
 *      numOfPaths             - The number of route paths bound to the lookup.
 *                               This value ranges from 0 (a single path) to
 *                               7 (8 ECMP or QoS route paths).
 *      routeEntryBaseIndex    - If numOfPaths = 0, this is the direct index to
 *                               the single Route entry. If numOfPaths is
 *                               greater than ‘0’, this is the base index to a
 *                               block of contiguous Route entries.
 *                               The Route Entry Index range: 0–4095.
 *      ucRPFCheckEnable         - Enable Unicast RPF check for this Entry.
 *      sipSaCheckMismatchEnable - Enable Unicast SIP MAC SA match check.
 *      ipv6MCGroupScopeLevel  - This is the IPv6 Multicast group scope level.
 *
 */
typedef struct
{
    UT_TEST_IP_ROUTE_ENTRY_METHOD_ENT routeType;
    GT_U32                              numOfPaths;
    GT_U32                              routeEntryBaseIndex;
    GT_BOOL                             ucRPFCheckEnable;
    GT_BOOL                             sipSaCheckMismatchEnable;
    CPSS_IPV6_PREFIX_SCOPE_ENT          ipv6MCGroupScopeLevel;
}UT_TEST_IP_LTT_ENTRY_STC;

/*
 * typedef: struct UT_TEST_IP_TCAM_ROUTE_ENTRY_INFO_UNT
 *
 * Description:
 *      This union holds the two possible associations a prefix can have
 *      for DXCH devices it will be a PCL action.
 *      for DXCH2 devices it will be a LTT entry.
 *
 * Fields:
 *  pclIpUcActionPtr - a pointer to PCL action representing the UC route entry
 *                     used for DXCH devices. (see UT_TEST_PCL_ACTION_STC)
 *  ipLttEntryPtr    - a pointer to a LTT entry which holds the route entry
 *                     pointer and more (see UT_TEST_IP_LTT_ENTRY_STC)
 */
typedef union
{
    UT_TEST_PCL_ACTION_STC        pclIpUcAction;
    UT_TEST_IP_LTT_ENTRY_STC      ipLttEntry;
}UT_TEST_IP_TCAM_ROUTE_ENTRY_INFO_UNT;

/*
 * Typedef: struct UT_TEST_IP_COUNTER_SET_STC
 *
 * Description: IPvX Counter Set
 *
 * Fields:
 *      inUcPkts                 - number of ingress unicast packets
 *      inMcPkts                 - number of ingress multicast packets
 *      inUcNonRoutedExcpPkts    - number of ingress Unicast packets that were
 *                                 not routed due to one or more exceptions.
 *      inUcNonRoutedNonExcpPkts - number of ingress Unicast packets that were
 *                                 not routed but didn't receive any exception.
 *      inMcNonRoutedExcpPkts    - number of ingress multicast packets that were
 *                                 not routed due to one or more exceptions.
 *      inMcNonRoutedNonExcpPkts - number of ingress multicast packets that were
 *                                 not routed but didn't receive any exception.
 *      inUcTrappedMirrorPkts    - number of ingress unicast packets that were
 *                                 trapped or mirrored by the router engine.
 *      inMcTrappedMirrorPkts    - number of ingress multicast packets that were
 *                                 trapped or mirrored by the router engine.
 *      mcRfpFailPkts            - number of multicast packets with fail RPF.
 *      outUcRoutedPkts          - the number of egress unicast routed packets.
 */
typedef struct
{
    GT_U32 inUcPkts;
    GT_U32 inMcPkts;
    GT_U32 inUcNonRoutedExcpPkts;
    GT_U32 inUcNonRoutedNonExcpPkts;
    GT_U32 inMcNonRoutedExcpPkts;
    GT_U32 inMcNonRoutedNonExcpPkts;
    GT_U32 inUcTrappedMirrorPkts;
    GT_U32 inMcTrappedMirrorPkts;
    GT_U32 mcRfpFailPkts;
    GT_U32 outUcRoutedPkts;
}UT_TEST_IP_COUNTER_SET_STC;

/*******************************************************************************
* utTrafficDemoInit
*
* DESCRIPTION:
*       init the traffic demo
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS utTrafficDemoInit(
    void
);

/*******************************************************************************
*   utTestExpectedResultCheck
*
* DESCRIPTION:
*       check the expected result against the actual results
*
* INPUTS:
*       fieldName - filed name (string)
*       actualResult   - actual Result
*       expectedResult - expected result
*       numCompares    - number of comparisons
*       compareParamsArray - (array of) comparisons info
*       extraInfo - extra info to print when error
*                   use value of UT_TEST_EXPECTED_EXTRA_INFO_UNUSED_CNS
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - compare failed , and test should fail on first fail
*       GT_BAD_PTR      - on NULL pointer
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS utTestExpectedResultCheck
(
    IN  char*                           fieldName,
    IN  GT_U32                          actualResult,
    IN  GT_U32                          expectedResult,
    IN  GT_U32                          numCompares,
    IN  UT_TEST_COMPARE_PARAM_STC       compareParamsArray[],
    IN  GT_U32                          extraInfo
);


/*******************************************************************************
*   utTestTitleTrace
*
* DESCRIPTION:
*       trace the title of the test and it's description
*
* INPUTS:
*       start - started/ended
*       testName - name of the test
*       testDescription - description of the test
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
* COMMENTS:
*
*
*******************************************************************************/
void utTestTitleTrace
(
    IN GT_BOOL  start,
    IN char *   testName,
    IN char *   testDescription,
    IN ...
);

/*******************************************************************************
*   utTestPhaseTrace
*
* DESCRIPTION:
*       trace the phase number and format
*
* INPUTS:
*       format - format for printing.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
* COMMENTS:
*
*
*******************************************************************************/
void utTestPhaseTrace
(
    IN GT_U32   phaseNum,
    IN char *   phaseFormat,
    IN ...
);

/*******************************************************************************
*   utTestExpectedResultsTrace
*
* DESCRIPTION:
*       trace the expected results
*
* INPUTS:
*       actual - actual value
*       expected - expected value
*       format - format for printing.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
* COMMENTS:
*
*
*******************************************************************************/
void utTestExpectedResultsTrace
(
    IN GT_U32   actual,
    IN GT_U32   expected,
    IN char *   format,
    IN ...
);

/*******************************************************************************
* utTestPortsAndTagsArrayToBmpConvert
*
* DESCRIPTION:
*       convert the list of ports and taggs to bmp of ports nad tags in device
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       devNum              - device number
*       numPorts     - number of ports to set to vlan
*       portsArray   - array of ports
*       taggedArray  - array of tagging of the ports
*       portsMembersPtr     - (pointer to) bmp of ports members in vlan
*       portsTaggingPtr     - (pointer to) bmp of ports tagged in the vlan
*       portsTaggingCmd     - (pointer to) ports tagging commands
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong port type
*       GT_BAD_PTR               - one portsMembersPtr NULL pointer or portsArray
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS utTestPortsAndTagsArrayToBmpConvert(
    IN  GT_U8                       devNum,
    IN  GT_U32                      numPorts,
    IN  CPSS_INTERFACE_INFO_STC     portsArray[],
    IN  GT_BOOL                     taggedArray[],
    OUT CPSS_PORTS_BMP_STC          *portsMembersPtr,
    OUT CPSS_PORTS_BMP_STC          *portsTaggingPtr,
    OUT UT_TEST_BRG_VLAN_PORTS_TAG_CMD_STC *portsTaggingCmd
);


/*******************************************************************************
* utTestBrgVlanEntryWrite
*
* DESCRIPTION:
*       Builds and writes vlan entry to HW.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       vlanId       - VLAN Id
*       numPorts     - number of ports to set to vlan
*       portsArray   - array of ports
*       vlanParamPtr - (pointer to) VLAN specific information
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong vid
*       GT_OUT_OF_RANGE          - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS   utTestBrgVlanEntryWrite(
    IN  UT_VLAN_ID              vlanId,
    IN  GT_U32                  numPorts,
    IN  CPSS_INTERFACE_INFO_STC portsArray[],
    IN  GT_BOOL                 taggedArray[],
    IN  UT_TEST_BRG_VLAN_INFO_STC *vlanParamPtr
);

/*******************************************************************************
* utTestBrgVlanLearningStateSet
*
* DESCRIPTION:
*       Sets state of VLAN based learning to specified VLAN on specified device.
*
* INPUTS:
*       vlanId    - vlan Id
*       status    - GT_TRUE for enable  or GT_FALSE otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong vlanId
*
* COMMENTS:
*******************************************************************************/
GT_STATUS utTestBrgVlanLearningStateSet
(
    IN GT_U16   vlanId,
    IN GT_BOOL  status
);

/*******************************************************************************
* utTestBrgFdbMacEntrySet
*
* DESCRIPTION:
*       Create new or update existing entry in Hardware MAC address table through
*       Address Update message.(AU message to the PP is non direct access to MAC
*       address table).
*       The function use New Address message (NA) format.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
*
* INPUTS:
*       devNum          - physical device number
*       macEntryPtr     - pointer mac table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Application should synchronize call of utTestBrgFdbMacEntrySet,
*       utTestBrgFdbQaSend and utTestBrgFdbMacEntryDelete functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the AU message processing has completed by
*       utTestBrgFdbFromCpuAuMsgStatusGet.
*       If the entry type is IPv6, then the Application should set the four
*       bytes for SIP and DIP that selected by  utTestBrgMcIpv6BytesSelectSet.
*
*******************************************************************************/
GT_STATUS utTestBrgFdbMacEntrySet
(
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
);


/*******************************************************************************
* utTestTunnelStartEntrySetDxCh
*
* DESCRIPTION:
*       Set a tunnel start entry.
*
*
* INPUTS:
*       routerArpTunnelStartLineIndex - line index for the tunnel start entry
*                       in the router ARP / tunnel start table (0..1023)
*       tunnelType    - type of the tunnel; valid options:
*                       CPSS_TUNNEL_X_OVER_IPV4_E,
*                       CPSS_TUNNEL_X_OVER_GRE_IPV4_E,
*                       CPSS_TUNNEL_X_OVER_MPLS_E
*       configPtr     - points to tunnel start configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Tunnel start entries table and router ARP addresses table reside at
*       the same physical memory. The table contains 1K lines.
*       Each line can hold:
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       Indexes for tunnel start range is (0..1023); Indexes for router ARP
*       addresses range is (0..4095).
*       Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*
*******************************************************************************/
GT_STATUS utTestTunnelStartEntrySet
(
    IN  GT_U32                              routerArpTunnelStartLineIndex,
    IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
    IN  UT_TEST_TUNNEL_START_CONFIG_UNT     *configPtr
);


/*******************************************************************************
* utTestIpCntSetModeSet
*
* DESCRIPTION:
*      Sets a counter set's bounded interface and interface mode.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       cntSet        - the counter set
*       cntSetMode    - the counter set bind mode (interface or Next hop)
*       interfaceModeCfgPtr - if cntSetMode = UT_TEST_IP_CNT_SET_INTERFACE_MODE_E
*                          this is the interface configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS utTestIpCntSetModeSet
(
    IN  CPSS_IP_CNT_SET_ENT                      cntSet,
    IN  UT_TEST_IP_CNT_SET_MODE_ENT              cntSetMode,
    IN  UT_TEST_IP_COUNTER_SET_INTERFACE_CFG_STC *interfaceModeCfgPtr
);



/*******************************************************************************
* utTestIpPortRoutingEnable
*
* DESCRIPTION:
*       Enable multicast/unicast IPv4/v6 routing on a port.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum        - the device number
*       portNum       - the port to enable on
*       ucMcEnable    - routing type to enable Unicast/Multicast
*       protocolStack - what type of traffic to enable ipv4 or ipv6 or both.
*       enableRouting - enable IP routing for this port
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none
*
*
*******************************************************************************/
GT_STATUS utTestIpPortRoutingEnable
(
    IN GT_U8                            devNum,
    IN GT_U8                            portNum,
    IN CPSS_IP_UNICAST_MULTICAST_ENT    ucMcEnable,
    IN CPSS_IP_PROTOCOL_STACK_ENT       protocolStack,
    IN GT_BOOL                          enableRouting
);


/*******************************************************************************
* utTestBrgVlanIpUcRouteEnable
*
* DESCRIPTION:
*       Enable/Disable IPv4/Ipv6 Unicast Routing on Vlan
*
* APPLIOCABLE DEVICES: All DxCh devices
*
* INPUTS:
*       devNum      - device number
*       vlanId      - Vlan ID
*       protocol    - ipv4 or ipv6
*       value       - GT_TRUE - enable ip unicast routing;
*                     GT_FALSE -  disable ip unicast routing.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS utTestBrgVlanIpUcRouteEnable
(
    IN GT_U8                        devNum,
    IN GT_U16                       vlanId,
    IN CPSS_IP_PROTOCOL_STACK_ENT   protocol,
    IN GT_BOOL                      enable
);


/*******************************************************************************
* utTestIpUcRouteEntriesWrite
*
* DESCRIPTION:
*    Writes an array of uc route entries to hw.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       baseRouteEntryIndex   - the index from which to write the array
*       routeEntriesArray     - the uc route entries array
*       numOfRouteEntries     - the number route entries in the array (= the
*                               number of route entries to write)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on devNum not active.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In case uRPF ECMP/QOS is globally enabled, then for ECMP/QOS block
*       with x route entries, additional route entry should be included after
*       the block with the uRPF route entry information.
*
*
*******************************************************************************/
GT_STATUS utTestIpUcRouteEntriesWrite
(
    IN GT_U32                          baseRouteEntryIndex,
    IN UT_TEST_IP_UC_ROUTE_ENTRY_STC   *routeEntriesArray,
    IN GT_U32                          numOfRouteEntries
);


/*******************************************************************************
* utTestIpCntGet
*
* DESCRIPTION:
*       Return the IP counter set requested.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       cntSet - counters set to retrieve.
*
* OUTPUTS:
*       countersPtr - (pointer to) counter values
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED       - The library was not initialized.
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This function doesn't handle counters overflow.
*
*******************************************************************************/
GT_STATUS utTestIpCntGet
(
    IN  CPSS_IP_CNT_SET_ENT          cntSet,
    OUT UT_TEST_IP_COUNTER_SET_STC  *countersPtr
);

/*******************************************************************************
* utTestIpCntSet
*
* DESCRIPTION:
*       set IP counter set requested.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       cntSet - counters set to retrieve.
*
* OUTPUTS:
*       countersPtr - (pointer to) counter values
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED       - The library was not initialized.
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This function doesn't handle counters overflow.
*
*******************************************************************************/
GT_STATUS utTestIpCntSet
(
    IN  CPSS_IP_CNT_SET_ENT          cntSet,
    IN  UT_TEST_IP_COUNTER_SET_STC  *countersPtr
);

/*******************************************************************************
* utTestIpLpmIpv4UcPrefixAdd
*
* DESCRIPTION:
*   Creates a new or override an existing Ipv4 prefix in a Virtual Router for
*   the specified LPM DB.
*
* APPLICABLE DEVICES: All DxCh Devices.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*                         future support only , currently only vrId = 0 is
*                         supported.
*       ipAddr          - The destination IP address of this prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*       nextHopInfoPtr  - the route entry info associated with this UC prefix.
*       override        - override an existing entry for this mask if it already
*                         exists, or don't override and return error.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_OUT_OF_CPU_MEM        - If failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - If failed to allocate TCAM memory, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
GT_STATUS utTestIpLpmIpv4UcPrefixAdd
(
    IN GT_U32                                   lpmDBId,
    IN GT_U32                                   vrId,
    IN GT_IPADDR                                ipAddr,
    IN GT_U32                                   prefixLen,
    IN UT_TEST_IP_TCAM_ROUTE_ENTRY_INFO_UNT     *nextHopInfoPtr,
    IN GT_BOOL                                  override
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __testTypesUth */

