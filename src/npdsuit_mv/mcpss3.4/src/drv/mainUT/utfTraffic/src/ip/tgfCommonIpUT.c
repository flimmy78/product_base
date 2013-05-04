/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfCommonIpUT.c
*
* DESCRIPTION:
*       Enhanced UTs for CPSS IPv4 and IPv6
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <ip/prvTgfBasicIpv4UcRouting.h>
#include <ip/prvTgfBasicIpv4UcEcmpRouting.h>
#include <ip/prvTgfBasicIpv6UcRouting.h>
#include <ip/prvTgfBasicIpv4UcLowLevelRouting.h>
#include <ip/prvTgfBasicIpv4McRouting.h>
#include <ip/prvTgfBasicIpv6McRouting.h>
#include <ip/prvTgfBasicIpv4Uc2VirtualRouter.h>
#include <ip/prvTgfBasicIpv42SharedVirtualRouter.h>
#include <ipLpmEngine/prvTgfDynamicPartitionIpLpmUcRouting.h>
#include <ip/prvTgfDefaultIpv4McRouting.h>
#include <ip/prvTgfDefaultIpv6McRouting.h>
#include <ip/prvTgfComplicatedIpv4McRouting.h>
#include <ip/prvTgfIpLpmDBCapacityUpdate.h>
#include <ip/prvTgfIpNonExactMatchRoutingIpv4Uc.h>
#include <ip/prvTgfIpNonExactMatchRoutingIpv6Uc.h>
#include <ip/prvTgfIpNonExactMatchRoutingIpv4Mc.h>
#include <ip/prvTgfIpNonExactMatchRoutingIpv6Mc.h>
#include <common/tgfIpGen.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>


/* Get parameter value from AppDemo DataBase. */
extern GT_STATUS appDemoDbEntryGet
(
    IN  GT_U8   *namePtr,
    OUT GT_U32  *valuePtr
);


/*----------------------------------------------------------------------------*/
/*
    Test basic IPv4 unicast routing
*/
UTF_TEST_CASE_MAC(tgfBasicIpv4UcRouting)
{
/*
    1. Set Base configuration
    2. Set Route configuration
    3. Generate traffic
    4. Restore configuration
*/

    /* Set Base configuration */
    prvTgfBasicIpv4UcRoutingBaseConfigurationSet(prvTgfPortsArray[1]);

    /* Set Route configuration */
    prvTgfBasicIpv4UcRoutingRouteConfigurationSet(prvTgfPortsArray[0], prvTgfPortsArray[3]);

    /* Generate traffic */
    prvTgfBasicIpv4UcRoutingTrafficGenerate(prvTgfPortsArray[0], prvTgfPortsArray[3]);

    /* Restore configuration */
    prvTgfBasicIpv4UcRoutingConfigurationRestore(prvTgfPortsArray[0]);
}

/*----------------------------------------------------------------------------*/
/*
    Test basic IPv4 unicast routing
*/
UTF_TEST_CASE_MAC(tgfBasicIpv4UcRoutingCheckAgeBit)
{
/*
    1. set port configuration if needed
    2. Set Base configuration
    3. Set Route configuration
    4. Generate traffic
    5. Check Age Bit updated
    6. Restore port configuration if needed
    7. Restore configuration
*/

    if(prvUtfDeviceTestNotSupport(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E))
    {
        /* Set Base configuration */
        prvTgfBasicIpv4UcRoutingBaseConfigurationSet(prvTgfPortsArray[0]);

        /* Set Route configuration */
        prvTgfBasicIpv4UcRoutingRouteConfigurationSet(prvTgfPortsArray[1],prvTgfPortsArray[2]);

        /* Generate traffic from port 18 to port 36 */
        prvTgfBasicIpv4UcRoutingTrafficGenerate(prvTgfPortsArray[1], prvTgfPortsArray[2]);

             /* Check age bit was updated */
        prvTgfBasicIpv4UcRoutingAgeBitGet();

        /* Restore configuration */
        prvTgfBasicIpv4UcRoutingConfigurationRestore(prvTgfPortsArray[1]);
    }
    else
    {
        /* Set Base configuration */
        prvTgfBasicIpv4UcRoutingBaseConfigurationSet(prvTgfPortsArray[1]);

        /* Set Route configuration */
        prvTgfBasicIpv4UcRoutingRouteConfigurationSet(prvTgfPortsArray[0], prvTgfPortsArray[3]);

        /* Generate traffic */
        prvTgfBasicIpv4UcRoutingTrafficGenerate(prvTgfPortsArray[0], prvTgfPortsArray[3]);

        /* Check age bit was updated */
        prvTgfBasicIpv4UcRoutingAgeBitGet();

        /* Restore configuration */
        prvTgfBasicIpv4UcRoutingConfigurationRestore(prvTgfPortsArray[0]);

    }
}

/*----------------------------------------------------------------------------*/
/*
    Test IpLpmDBCapacityUpdate
    Add IPv4 exact match prefix (like 1.1.1.3 with prefix length 32)
    Verify traffic to this prefix and also to the default prefix
    Call cpssDxChIpLpmDBCapacityUpdate with the following params:
        indexesRange: 0 - 9
        tcamLpmManagerCapcityCfg:
            numOfIpv4Prefixes: 4
            numOfIpv4McSourcePrefixes: 4
            numOfIpv6Prefixes: 4
    This should initiate moving of all existing prefixes.
    Now verify traffic again to both prefixes.
*/
UTF_TEST_CASE_MAC(tgfIpLpmDBCapacityUpdate)
{
    prvTgfIpLpmDBCapacityUpdateBaseConfigurationSet();

    prvTgfIpLpmDBCapacityUpdateRouteConfigurationSet();

    PRV_UTF_LOG0_MAC("======= Generating Traffic - matched prefix =======\n");
    prvTgfIpLpmDBCapacityUpdateTrafficGenerate(GT_TRUE);

    PRV_UTF_LOG0_MAC("======= Generating Traffic - default prefix =======\n");
    prvTgfIpLpmDBCapacityUpdateTrafficGenerate(GT_FALSE);

    PRV_UTF_LOG0_MAC("======= IpLpmDBCapacityUpdate =======\n");
    prvTgfIpLpmDBCapacityUpdateRouteConfigurationUpdate();

    PRV_UTF_LOG0_MAC("======= Generating Traffic - matched prefix =======\n");
    prvTgfIpLpmDBCapacityUpdateTrafficGenerate(GT_TRUE);

    PRV_UTF_LOG0_MAC("======= Generating Traffic - default prefix =======\n");
    prvTgfIpLpmDBCapacityUpdateTrafficGenerate(GT_FALSE);

    prvTgfIpLpmDBCapacityUpdateConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    Test basic IPv4 unicast ECMP routing
*/
UTF_TEST_CASE_MAC(tgfBasicIpv4UcEcmpRouting)
{
/*
    1. Set Base configuration
    2. Set Route configuration
    3. Generate traffic
    4. Restore configuration
*/

    /* Set Base configuration */
    prvTgfBasicIpv4UcEcmpRoutingBaseConfigurationSet();

    /* Set Route configuration */
    prvTgfBasicIpv4UcEcmpRoutingRouteConfigurationSet();

    /* Generate traffic */
    prvTgfBasicIpv4UcEcmpRoutingTrafficGenerate();

    /* Restore configuration */
    prvTgfBasicIpv4UcEcmpRoutingConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    Test basic IPv6 unicast routing
*/
UTF_TEST_CASE_MAC(tgfBasicIpv6UcRouting)
{
/*
    1. Set Base configuration
    2. Set Route configuration
    3. Generate traffic
    4. Restore configuration
*/

    /* Set Base configuration */
    prvTgfBasicIpv6UcRoutingBaseConfigurationSet();

    /* Set Route configuration */
    prvTgfBasicIpv6UcRoutingRouteConfigurationSet();

    /* Generate traffic */
    prvTgfBasicIpv6UcRoutingTrafficGenerate();

    /* Restore configuration */
    prvTgfBasicIpv6UcRoutingConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    Test basic IPv4 unicast low level routing
*/
UTF_TEST_CASE_MAC(tgfBasicIpv4UcLowLevelRouting)
{
/*
    1. Set Base configuration
    2. Set Route configuration
    3. Generate traffic
    4. Restore configuration
*/

    /* Set Base configuration */
    prvTgfBasicIpv4UcLowLevelRoutingBaseConfigurationSet();

    /* Set Route configuration */
    prvTgfBasicIpv4UcLowLevelRoutingRouteConfigurationSet();

    /* Generate traffic */
    prvTgfBasicIpv4UcLowLevelRoutingTrafficGenerate();

    /* Restore configuration */
    prvTgfBasicIpv4UcLowLevelRoutingConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    Test basic IPv4 multicast routing
      we have 3 VLANS[ports]: 55[0, 8], 17[18, 23], 80[18]
      there are 3 MC subscribers MC_1, MC_2, MC_3 on the ports 8, 23, 0
      the test sends a MC IPv4 packet to the port 18 in VLAN 80
      then checks that each of 3 MC subscribers receives the same MC packet
    To configure this the test set up 3 VLANs, 4 ports, 1 FDB record;
      entries: 3 MLL for 3 subscribers, 1 Route, 1 Ipv4 MC prefix;
      enable IPv4 MC in VLAN 80 and port 18;
    To control this the test uses capturing on MC_2 port;
      counters: 4 ETH and 4 IP for each port, 2 MLL for MC_1 and MC_2 ports;
*/
UTF_TEST_CASE_MAC(tgfBasicIpv4McRouting)
{
    prvTgfBasicIpv4McRoutingBaseConfigurationSet();

    prvTgfBasicIpv4McRoutingRouteConfigurationSet();

    prvTgfBasicIpv4McRoutingTrafficGenerate();

    prvTgfBasicIpv4McRoutingConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    Test basic IPv6 multicast routing (the same as for IPv4)
      we have 3 VLANS[ports]: 55[0, 8], 17[18, 23], 80[18]
      there are 3 MC subscribers MC_1, MC_2, MC_3 on the ports 8, 23, 0
      the test sends a MC IPv6 packet to the port 18 in VLAN 80
      then checks that each of 3 MC subscribers receives the same MC packet
    To configure this the test set up 3 VLANs, 4 ports, 1 FDB record;
      entries: 3 MLL for 3 subscribers, 1 Route, 1 Ipv4 MC prefix;
      enable IPv6 MC in VLAN 80 and port 18;
    To control this the test uses capturing on MC_2 port;
      counters: 4 ETH and 4 IP for each port, 2 MLL for MC_1 and MC_2 ports;
*/
UTF_TEST_CASE_MAC(tgfBasicIpv6McRouting)
{
    prvTgfBasicIpv6McRoutingBaseConfigurationSet();

    prvTgfBasicIpv6McRoutingRouteConfigurationSet();

    prvTgfBasicIpv6McRoutingTrafficGenerate();

    prvTgfBasicIpv6McRoutingConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    Test default IPv4 multicast routing
      we have 2 VLANS[ports]: 55[0, 8], 80[18, 23]
      there are 1 MC subscriber MC_1 on the port 8
      the test sends MC IPv4 packets without adding prefix and check that packet
      sent according to defaults config
      the test sends MC IPv4 packets with adding prefix to the port 18
      then checks that packet sent accordingly created config
      the test sends MC IPv4 packets with different Src with adding prefix
      to the port 18 then checks that packet sent according to defaults config
    To configure this the test set up 2 VLANs, 4 ports;
      entries: 2 MLL, 2 Routes, 1 Ipv4 MC prefixes;
      enable IPv4 MC in VLAN 80 and port 18;
    To control this the test uses capturing on 8 and 23 ports;
      counters: 4 ETH and 4 IP for each port, 2 MLL;
*/
UTF_TEST_CASE_MAC(tgfDefaultIpv4McRouting)
{
    prvTgfDefaultIpv4McRoutingBaseConfigurationSet();

    prvTgfDefaultIpv4McRoutingDefaultLttRouteConfigurationSet();

    prvTgfDefaultIpv4McRoutingDefaultTrafficGenerate();

    prvTgfDefaultIpv4McRoutingAdditionalRouteConfigurationSet();

    prvTgfDefaultIpv4McRoutingAdditionalTrafficGenerate();

    prvTgfDefaultIpv4McRoutingConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    Test default IPv6 multicast routing
      we have 2 VLANS[ports]: 55[0, 8], 80[18, 23]
      there are 1 MC subscriber MC_1 on the port 8
      the test sends MC IPv6 packets without adding prefix and check that packet
      sent according to defaults config
      the test sends MC IPv6 packets with adding prefix to the port 18
      then checks that packet sent accordingly created config
      the test sends MC IPv6 packets with different Src with adding prefix
      to the port 18 then checks that packet sent according to defaults config
    To configure this the test set up 2 VLANs, 4 ports;
      entries: 2 MLL, 2 Routes, 1 Ipv4 MC prefixes;
      enable IPv6 MC in VLAN 80 and port 18;
    To control this the test uses capturing on 8 and 23 ports;
      counters: 4 ETH and 4 IP for each port, 2 MLL;
*/
UTF_TEST_CASE_MAC(tgfDefaultIpv6McRouting)
{
    prvTgfDefaultIpv6McRoutingBaseConfigurationSet();

    prvTgfDefaultIpv6McRoutingDefaultLttRouteConfigurationSet();

    prvTgfDefaultIpv6McRoutingDefaultTrafficGenerate();

    prvTgfDefaultIpv6McRoutingAdditionalRouteConfigurationSet();

    prvTgfDefaultIpv6McRoutingAdditionalTrafficGenerate();

    prvTgfDefaultIpv6McRoutingConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    Test basic IPv4 unicast to virtual router
*/
UTF_TEST_CASE_MAC(tgfBasicIpv4Uc2VirtualRouter)
{
/*
    1. Set Base configuration
    2. Set Route configuration
    3. Generate traffic
    4. Restore configuration
*/

    /* Set Base configuration */
    prvTgfBasicIpv4Uc2VirtualRouterBaseConfigurationSet();

    /* Set Route configuration */
    prvTgfBasicIpv4Uc2VirtualRouterRouteConfigurationSet();

    /* Generate traffic */
    prvTgfBasicIpv4Uc2VirtualRouterTrafficGenerate();

    /* Restore configuration */
    prvTgfBasicIpv4Uc2VirtualRouterConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    Test basic IPV4 2 shared virtual Router
*/
UTF_TEST_CASE_MAC(tgfBasicIpv42SharedVirtualRouter)
{
/*
    1. Set Base configuration
    2. Set Route configuration
    3. Generate traffic
    4. Restore configuration
*/
    if (prvUtfIsPbrModeUsed())
    {
        return;
    }

    /* Set Base configuration */
    prvTgfBasicIpv42SharedVirtualRouterBaseConfigSet();

    /* Set Route configuration */
    prvTgfBasicIpv42SharedVirtualRouterRouteConfigSet();

    /* Set UC IP Configuration */
    prvTgfBasicIpv42SharedVirtualRouterUcIpConfigSet();

    /* Set MC IP Configuration */
    prvTgfBasicIpv42NonSharedVirtualRouterMcIpConfigSet();

    /* Enable routing mechanism */
    prvTgfBasicIpv42SharedVirtualRouterEnable();

    /* Generate traffic */
    prvTgfBasicIpv42SharedVirtualRouterTrafficGenerate();

    /* Restore configuration */
    prvTgfBasicIpv4Uc2SharedVRConfigRestore();
}

/*----------------------------------------------------------------------------*/
/*
    Test complicated IPv4 multicast routing
      we have 2 VLANS[ports]: 55[0, 8], 80[18, 23]
      there are 2 MC subscribers MC_1 on port 8 and MC_2 on port 0
    - send MC IPv4 packet without adding prefix and check that packet is sent
      according to defaults config
    - add two MC entries
    - search both entries -- 1 and 2 entries - found
    - send 2 MC IPv4 packets to the port 18 with prefix, checks that
      packets are sent accordingly created config - to ports 8 and port 0
    - delete second MC entry
    - search both entries -- 1 entry - found, 2 entry - not found
    - send 2 MC IPv4 packets to the port 18 with one prefix, checks that
      packets are sent accordingly created config - to ports 8 and to the default
    - restore second MC entry
    - search both entries -- 1 and 2 entries - found
    - send 2 MC IPv4 packets to the port 18 with prefix, checks that
      packets are sent accordingly created config - to ports 8 and port 0
    To configure this the test set up 2 VLANs, 4 ports;
      entries: 2 Routes, 2 Ipv4 MC prefixes;
      enable IPv4 MC in VLAN 80 and port 18;
    To control this the test uses capturing on 0, 8 and 23 ports;
      counters: 4 ETH and 4 IP for each port;
*/
UTF_TEST_CASE_MAC(tgfComplicatedIpv4McRouting)
{
    prvTgfComplicatedIpv4McRoutingBaseConfigurationSet();

    prvTgfComplicatedIpv4McRoutingDefaultLttRouteConfigurationSet();

    prvTgfComplicatedIpv4McRoutingDefaultTrafficGenerate();

    prvTgfComplicatedIpv4McRoutingAdditionalRouteConfigurationSet();

    prvTgfComplicatedIpv4McRoutingAdditionalTrafficGenerate();

    prvTgfComplicatedIpv4McRoutingConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    UC IPv4 non-exact match prefix test

    We have 2 VLANS[portIdx]: 5[0, 1], 6[2, 3]
    PortIdx [0] is used to send packets and portIdx[3] to receive.
    Base IP address = 10.153.102.170.
    - Generate IP address array in the following way:
      IP address is made from Base IP address by inverting one bit.
      Inverted bit index is equal to index in IP address array.
    - Start loop where prefixLen is incremented from 1 to 31.
    - Add UC prefix to the LPM DB, where IP prefix is equal to
      base IP address and Prefix Length is equal to current iteration number.
    - Transmit burst of 32 packets, where:
      DST IP addresses are got from generated IP address array.
      Payload Data contain modified bit index value.
    - Check Success Criteria:
      If modified bit index < prefixLen, packet is expected on portIdx 3
      If modified bit index >= prefixLen packet is not expected
    - Delete prefix from the LPM DB.
    - End loop.
*/
UTF_TEST_CASE_MAC(prvTgfIpv4NonExactMatchUcRouting)
{
/*
    1. Set Base configuration
    2. Set Route configuration
    3. Generate traffic
    4. Restore configuration
*/

    /* Set Base configuration */
    prvTgfIpNonExactMatchRoutingIpv4UcBaseConfigurationSet();

    /* Set Route configuration */
    prvTgfIpNonExactMatchRoutingIpv4UcRouteConfigurationSet();

    /* Generate traffic */
    prvTgfIpNonExactMatchRoutingIpv4UcTrafficGenerate();

    /* Restore configuration */
    prvTgfIpNonExactMatchRoutingIpv4UcConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    UC IPv6 non-exact match prefix test

    We have 2 VLANS[portIdx]: 5[0, 1], 6[2, 3]
    PortIdx [0] is used to send packets and portIdx[3] to receive.
    Base IP address = 0x6545::0x3212.
    - Generate IP address array in the following way:
      IP address is made from Base IP address by inverting one bit.
      Inverted bit index is equal to index in IP address array.
    - Start loop where prefixLen is incremented from 1 to 127.
    - Add UC prefix to the LPM DB, where IP prefix is equal to
      base IP address and Prefix Length is equal to current iteration number.
    - Transmit burst of 128 packets, where:
      DST IP addresses are got from generated IP address array.
      Payload Data contain modified bit index value.
    - Check Success Criteria:
      If modified bit index < prefixLen, packet is expected on portIdx 3
      If modified bit index >= prefixLen packet is not expected
    - Delete prefix from the LPM Data Base.
    - End loop.
*/
UTF_TEST_CASE_MAC(prvTgfIpv6NonExactMatchUcRouting)
{
/*
    1. Set Base configuration
    2. Set Route configuration
    3. Generate traffic
    4. Restore configuration
*/

    /* Set Base configuration */
    prvTgfIpNonExactMatchRoutingIpv6UcBaseConfigurationSet();

    /* Set Route configuration */
    prvTgfIpNonExactMatchRoutingIpv6UcRouteConfigurationSet();

    /* Generate traffic */
    prvTgfIpNonExactMatchRoutingIpv6UcTrafficGenerate();

    /* Restore configuration */
    prvTgfIpNonExactMatchRoutingIpv6UcConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    MC IPv4 non-exact match prefix test

    We have 3 VLANS[portIdx]: 5[0], 6[1, 2], 7[3]
    PortIdx [0] is used to send packets, portIdx[1, 2] to receive MC traffic,
    portIdx [3] to receive MC traffic according to default configuration.
    Base SRC IP address = 10.153.102.170.
    Base GRP IP address = 244.1.1.1.
    - Generate SRC IP address array in the following way:
      IP address is made from Base IP address by inverting one bit.
      Inverted bit index is equal to index in IP address array.
    - Start loop where prefixLen is incremented from 1 to 31.
    - Add MC prefix to the LPM DB, where SRC IP prefix is equal to
      base IP address and Prefix Length is equal to current iteration number.
      GRP IP prefix is equal to base GRP IP address with length 32.
    - Transmit burst of 32 packets, where:
      SRC IP addresses are got from generated IP address array.
      GRP IP addresses is equal to base GRP IP address.
      Payload Data contain modified bit index value.
    - Check Success Criteria:
      If modified bit index < prefixLen, packet is expected on portIdx 1, 2
      If modified bit index >= prefixLen packet is expected on portIdx 3
    - Delete prefix from the LPM DB.
    - End loop.
*/
UTF_TEST_CASE_MAC(prvTgfIpv4NonExactMatchMcRouting)
{
/*
    1. Set Base configuration
    2. Set Route configuration
    3. Generate traffic
    4. Restore configuration
*/

    /* Set Base configuration */
    prvTgfIpNonExactMatchRoutingIpv4McBaseConfigurationSet();

    /* Set Route configuration */
    prvTgfIpNonExactMatchRoutingIpv4McRouteConfigurationSet();

    /* Generate traffic */
    prvTgfIpNonExactMatchRoutingIpv4McTrafficGenerate();

    /* Restore configuration */
    prvTgfIpNonExactMatchRoutingIpv4McConfigurationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    MC IPv6 non-exact match prefix test

    We have 3 VLANS[portIdx]: 5[0], 6[1, 2], 7[3]
    PortIdx [0] is used to send packets, portIdx[1, 2] to receive MC traffic,
    portIdx [3] to receive MC traffic according to default configuration.
    Base SRC IP address = 0x6545::0x3212.
    Base GRP IP address = 0xFF01::0xEEFF.
    - Generate SRC IP address array in the following way:
      IP address is made from Base IP address by inverting one bit.
      Inverted bit index is equal to index in IP address array.
    - Start loop where prefixLen is incremented from 1 to 128.
    - Add MC prefix to the LPM DB, where SRC IP prefix is equal to
      base IP address and Prefix Length is equal to current iteration number.
      GRP IP prefix is equal to base GRP IP address with length 128.
    - Transmit burst of 128 packets, where:
      SRC IP addresses are got from generated IP address array.
      GRP IP addresses is equal to base GRP IP address.
      Payload Data contain modified bit index value.
    - Check Success Criteria:
      If modified bit index < prefixLen, packet is expected on portIdx 1, 2
      If modified bit index >= prefixLen packet is expected on portIdx 3
    - Delete prefix from the LPM DB.
    - End loop.
*/
UTF_TEST_CASE_MAC(prvTgfIpv6NonExactMatchMcRouting)
{
/*
    1. Set Base configuration
    2. Set Route configuration
    3. Generate traffic
    4. Restore configuration
*/

    /* Set Base configuration */
    prvTgfIpNonExactMatchRoutingIpv6McBaseConfigurationSet();

    /* Set Route configuration */
    prvTgfIpNonExactMatchRoutingIpv6McRouteConfigurationSet();

    /* Generate traffic */
    prvTgfIpNonExactMatchRoutingIpv6McTrafficGenerate();

    /* Restore configuration */
    prvTgfIpNonExactMatchRoutingIpv6McConfigurationRestore();
}

/*
 * Configuration of tgfIp suit
 */
UTF_SUIT_PBR_BEGIN_TESTS_MAC(tgfIp)
    UTF_SUIT_DECLARE_TEST_MAC(tgfBasicIpv4UcRouting)
    UTF_SUIT_DECLARE_TEST_MAC(tgfBasicIpv6UcRouting)
    UTF_SUIT_DECLARE_TEST_MAC(tgfIpLpmDBCapacityUpdate)
    UTF_SUIT_DECLARE_TEST_MAC(tgfBasicIpv4UcRoutingCheckAgeBit)
    UTF_SUIT_DECLARE_TEST_MAC(tgfBasicIpv4UcEcmpRouting)
    UTF_SUIT_DECLARE_TEST_MAC(tgfBasicIpv4McRouting)
    UTF_SUIT_DECLARE_TEST_MAC(tgfBasicIpv6McRouting)
    UTF_SUIT_DECLARE_TEST_MAC(tgfDefaultIpv4McRouting)
    UTF_SUIT_DECLARE_TEST_MAC(tgfDefaultIpv6McRouting)
    UTF_SUIT_DECLARE_TEST_MAC(tgfBasicIpv4UcLowLevelRouting)
    UTF_SUIT_DECLARE_TEST_MAC(tgfBasicIpv4Uc2VirtualRouter)
    UTF_SUIT_DECLARE_TEST_MAC(tgfBasicIpv42SharedVirtualRouter)
    UTF_SUIT_DECLARE_TEST_MAC(tgfComplicatedIpv4McRouting)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfIpv4NonExactMatchUcRouting)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfIpv6NonExactMatchUcRouting)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfIpv4NonExactMatchMcRouting)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfIpv6NonExactMatchMcRouting)
UTF_SUIT_PBR_END_TESTS_MAC(tgfIp, 3)
