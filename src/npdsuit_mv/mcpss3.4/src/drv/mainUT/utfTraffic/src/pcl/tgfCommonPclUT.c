/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfCommonPclUT.c
*
* DESCRIPTION:
*       Enhanced UTs for CPSS PCL
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>
#include <common/tgfCommon.h>

#include <pcl/prvTgfIngressPclKeyStdL2.h>
#include <pcl/prvTgfPclValidInvalidRulesBasicExtended.h>
#include <pcl/prvTgfPclValidInvalidRulesActionUpdate.h>
#include <pcl/prvTgfPclSecondLookupMatch.h>
#include <pcl/prvTgfPclBothUserDefinedBytes.h>
#include <pcl/prvTgfPclUdb.h>
#include <pcl/prvTgfPclUdbFlds.h>
#include <pcl/prvTgfPclBypass.h>
#include <pcl/prvTgfPclTcpRstFinMirroring.h>
#include <pcl/prvTgfPclPolicer.h>
#include <pcl/prvTgfPclPortGroupPolicer.h>
#include <pcl/prvTgfPclFullLookupControl.h>
#include <pcl/prvTgfPclMacModify.h>
#include <pcl/prvTgfPclTrunkHashVal.h>
#include <pcl/prvTgfPclUdePacketTypes.h>
#include <pcl/prvTgfPclEgrOrgVid.h>
#include <common/tgfTunnelGen.h>
#include <pcl/prvTgfPclMiscellanous.h>

/*----------------------------------------------------------------------------*/
/*
    tgfIngressPclKeyStdL2
*/
UTF_TEST_CASE_MAC(tgfIngressPclKeyStdL2)
{
/*
    1. Set configuration
    2. Set PCL configuration
    3. Generate traffic
    4. Restore configuration
*/

    /* Set configuration */
    prvTgfConfigurationSet();

    /* Set PCL configuration */
    prvTgfPclConfigurationSet();

    /* Generate traffic */
    prvTgfTrafficGenerate();

    /* Restore configuration */
    prvTgfConfigurationRestore();

    prvTgfPclRestore();

}

/*----------------------------------------------------------------------------*/
/*
    tgfPclValidInvalidRulesBasicExtended
*/
UTF_TEST_CASE_MAC(tgfPclValidInvalidRulesBasicExtended)
{
/*
    1. Set configuration
    2. Set PCL configuration
    3. Generate traffic
    4. Restore configuration
*/

    /* Set configuration */
    prvTgfPclValidInvalidRulesBasicExtendedConfigurationSet();

    /* Set PCL configuration */
    prvTgfPclValidInvalidRulesBasicExtendedPclConfigurationSet();

    /* Generate traffic */
    prvTgfPclValidInvalidRulesBasicExtendedTrafficGenerate();

    /* Restore configuration */
    prvTgfPclValidInvalidRulesBasicExtendedConfigurationRestore();

    prvTgfPclRestore();

}

/*----------------------------------------------------------------------------*/
/*
    tgfPclValidInvalidRulesActionUpdate
*/
UTF_TEST_CASE_MAC(tgfPclValidInvalidRulesActionUpdate)
{
/*
    1. Set configuration
    2. Set PCL configuration
    3. Generate traffic
    4. Restore configuration
*/

    /* Set configuration */
    prvTgfPclValidInvalidRulesActionUpdateConfigurationSet();

    /* Set PCL configuration */
    prvTgfPclValidInvalidRulesActionUpdatePclConfigurationSet();

    /* Generate traffic */
    prvTgfPclValidInvalidRulesActionUpdateTrafficGenerate();

    /* Restore configuration */
    prvTgfPclValidInvalidRulesActionUpdateConfigurationRestore();

    prvTgfPclRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclSecondLookupMatch
*/
UTF_TEST_CASE_MAC(tgfPclSecondLookupMatch)
{
/*
    1. Set configuration
    2. Set PCL configuration
    3. Generate traffic
    4. Restore configuration
*/

    /* Set configuration */
    prvTgfPclSecondLookupMatchConfigurationSet();

    /* Set PCL configuration */
    prvTgfPclSecondLookupMatchPclConfigurationSet();

    /* Generate traffic */
    prvTgfPclSecondLookupMatchTrafficGenerate();

    /* Restore configuration */
    prvTgfPclSecondLookupMatchConfigurationRestore();

    prvTgfPclRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclBothUserDefinedBytes
*/
UTF_TEST_CASE_MAC(tgfPclBothUserDefinedBytes)
{
/*
    1. Set configuration
    2. Set PCL configuration
    3. Generate traffic
    4. Restore configuration
*/

    /* Set configuration */
    prvTgfPclBothUserDefinedBytesConfigurationSet();

    /* Set PCL configuration */
    prvTgfPclBothUserDefinedBytesPclConfigurationSet();

    /* Generate traffic */
    prvTgfPclBothUserDefinedBytesTrafficGenerate();

    /* Restore configuration */
    prvTgfPclBothUserDefinedBytesConfigurationRestore();

    prvTgfPclRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclUserDefinedBytes
*/
UTF_TEST_CASE_MAC(tgfPclUserDefinedBytes)
{
/*
    1. Set configuration
    2. Set PCL configuration
    3. Generate traffic
    4. Restore configuration
*/

    /* Set bridge configuration */
    prvTgfPclBothUserDefinedBytesConfigurationSet();

    /* 1. Set PCL configuration to check IPV4 packet:
     * OFFSET_L2 0-8 bytes – Lion-B0 simulation
     * Generate traffic
     */
    prvTgfPclUserDefinedBytes_L2_0_8_ConfigurationSet();
    prvTgfPclBothUserDefinedBytesTrafficGenerate();

    /* 2. Set PCL configuration to check IPV4 packet:
     * OFFSET_L4 0-15 bytes – Lion-B0 simulation
     * Generate traffic
     */
    prvTgfPclUserDefinedBytes_L4_0_15_ConfigurationSet();
    prvTgfPclBothUserDefinedBytesTrafficGenerate();

    /* 3. Set PCL configuration to check ARP packet:
     * OFFSET_L2 0-11 bytes – Lion-B0 simulation
     * Generate traffic
     */
    prvTgfPclUserDefinedBytes_L2_0_11_ConfigurationSet();
    prvTgfPclBothUserDefinedBytesTrafficGenerate();

    /* Restore configuration */
    prvTgfPclBothUserDefinedBytesConfigurationRestore();

    prvTgfPclRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclIpv4TcpUdb
*/
UTF_TEST_CASE_MAC(tgfPclIpv4TcpUdb)
{
/*
    1. Set configuration
    2. Generate traffic
    3. Set additional configuration
    4. Generate traffic
    5. Restore configuration
*/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Set configuration */
    prvTgfPclIpv4TcpConfigSet();

    /* Generate traffic */
    prvTgfPclIpv4TcpTrafficGenerate();

    /* Set additional configuration */
    prvTgfPclIpv4TcpAdditionalConfigSet();

    /* Generate traffic */
    prvTgfPclIpv4TcpAdditionalTrafficGenerate();

    /* Restore configuration */
    prvTgfPclIpv4TcpConfigRestore();

    prvTgfPclRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclIpv4UdpUdb
*/
UTF_TEST_CASE_MAC(tgfPclIpv4UdpUdb)
{
/*
    1. Set configuration
    2. Generate traffic
    3. Set additional configuration
    4. Generate traffic
    5. Restore configuration
*/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Set configuration */
    prvTgfPclIpv4UdpConfigSet();

    /* Generate traffic */
    prvTgfPclIpv4UdpTrafficGenerate();

    /* Set additional configuration */
    prvTgfPclIpv4UdpAdditionalConfigSet();

    /* Generate traffic */
    prvTgfPclIpv4UdpAdditionalTrafficGenerate();

    /* Restore configuration */
    prvTgfPclIpv4UdpConfigRestore();

    prvTgfPclRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclIpv4FragmentUdb
*/
UTF_TEST_CASE_MAC(tgfPclIpv4FragmentUdb)
{
/*
    1. Set configuration
    2. Generate traffic
    3. Set additional configuration
    4. Generate traffic
    5. Restore configuration
*/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Set configuration */
    prvTgfPclIpv4FragmentConfigSet();

    /* Generate traffic */
    prvTgfPclIpv4FragmentTrafficGenerate();

    /* Set additional configuration */
    prvTgfPclIpv4FragmentAdditionalConfigSet();

    /* Generate traffic */
    prvTgfPclIpv4FragmentAdditionalTrafficGenerate();

    /* Restore configuration */
    prvTgfPclIpv4FragmentConfigRestore();

    prvTgfPclRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclIpv4OtherUdb
*/
UTF_TEST_CASE_MAC(tgfPclIpv4OtherUdb)
{
/*
    1. Set configuration
    2. Generate traffic
    3. Set additional configuration
    4. Generate traffic
    5. Restore configuration
*/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Set configuration */
    prvTgfPclIpv4OtherConfigSet();

    /* Generate traffic */
    prvTgfPclIpv4OtherTrafficGenerate();

    /* Set additional configuration */
    prvTgfPclIpv4OtherAdditionalConfigSet();

    /* Generate traffic */
    prvTgfPclIpv4OtherAdditionalTrafficGenerate();

    /* Restore configuration */
    prvTgfPclIpv4OtherConfigRestore();

    prvTgfPclRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclEthernetOtherUdb
*/
UTF_TEST_CASE_MAC(tgfPclEthernetOtherUdb)
{
/*
    1. Set configuration
    2. Generate traffic
    3. Set additional configuration
    4. Generate traffic
    5. Restore configuration
*/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Set configuration */
    prvTgfPclEthernetOtherConfigSet();

    /* Generate traffic */
    prvTgfPclEthernetOtherTrafficGenerate();

    /* Set additional configuration */
    prvTgfPclEthernetOtherAdditionalConfigSet();

    /* Generate traffic */
    prvTgfPclEthernetOtherAdditionalTrafficGenerate();

    /* Restore configuration */
    prvTgfPclEthernetOtherConfigRestore();

    prvTgfPclRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclMplsUdb
*/
UTF_TEST_CASE_MAC(tgfPclMplsUdb)
{
/*
    1. Set configuration
    2. Generate traffic
    3. Set additional configuration
    4. Generate traffic
    5. Restore configuration
*/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Set configuration */
    prvTgfPclMplsConfigSet();

    /* Generate traffic */
    prvTgfPclMplsTrafficGenerate();

    /* Set additional configuration */
    prvTgfPclMplsAdditionalConfigSet();

    /* Generate traffic */
    prvTgfPclMplsAdditionalTrafficGenerate();

    /* Restore configuration */
    prvTgfPclMplsConfigRestore();

    prvTgfPclRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclUdeUdb
*/
UTF_TEST_CASE_MAC(tgfPclUdeUdb)
{
/*
    1. Set configuration
    2. Generate traffic
    3. Set additional configuration
    4. Generate traffic
    5. Restore configuration
*/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Set configuration */
    prvTgfPclUdeConfigSet();

    /* Generate traffic */
    prvTgfPclUdeTrafficGenerate();

    /* Set additional configuration */
    prvTgfPclUdeAdditionalConfigSet();

    /* Generate traffic */
    prvTgfPclUdeAdditionalTrafficGenerate();

    /* Restore configuration */
    prvTgfPclUdeConfigRestore();

    prvTgfPclRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclIpv6Udb
*/
UTF_TEST_CASE_MAC(tgfPclIpv6Udb)
{
/*
    1. Set configuration
    2. Generate traffic
    3. Set additional configuration
    4. Generate traffic
    5. Restore configuration
*/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Set configuration */
    prvTgfPclIpv6ConfigSet();

    /* Generate traffic */
    prvTgfPclIpv6TrafficGenerate();

    /* Set additional configuration */
    prvTgfPclIpv6AdditionalConfigSet();

    /* Generate traffic */
    prvTgfPclIpv6AdditionalTrafficGenerate();

    /* Restore configuration */
    prvTgfPclIpv6ConfigRestore();

    prvTgfPclRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclIpv6TcpUdb
*/
UTF_TEST_CASE_MAC(tgfPclIpv6TcpUdb)
{
/*
    1. Set configuration
    2. Generate traffic
    3. Set additional configuration
    4. Generate traffic
    5. Restore configuration
*/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Set configuration */
    prvTgfPclIpv6TcpConfigSet();

    /* Generate traffic */
    prvTgfPclIpv6TcpTrafficGenerate();

    /* Set additional configuration */
    prvTgfPclIpv6TcpAdditionalConfigSet();

    /* Generate traffic */
    prvTgfPclIpv6TcpAdditionalTrafficGenerate();

    /* Restore configuration */
    prvTgfPclIpv6TcpConfigRestore();

    prvTgfPclRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclEthernetOtherVridUdb
*/
UTF_TEST_CASE_MAC(tgfPclEthernetOtherVridUdb)
{
/*
    1. Set configuration
    2. Generate traffic
    3. Set additional configuration
    4. Generate traffic
    5. Restore configuration
*/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(
        prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E
        | UTF_CH3_E | UTF_XCAT2_E);

    if(GT_FALSE == prvTgfBrgTtiCheck())
    {
        return;
    }

    /* Set configuration */
    prvTgfPclEthernetOtherVridConfigSet();

    /* Generate traffic */
    prvTgfPclEthernetOtherVridTrafficGenerate();

    /* Set additional configuration */
    prvTgfPclEthernetOtherVridAdditionalConfigSet();

    /* Generate traffic */
    prvTgfPclEthernetOtherVridAdditionalTrafficGenerate();

    /* Restore configuration */
    prvTgfPclEthernetOtherVridConfigRestore();

    prvTgfPclRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclIpv4UdpQosUdb
*/
UTF_TEST_CASE_MAC(tgfPclIpv4UdpQosUdb)
{
/*
    1. Set configuration
    2. Generate traffic
    3. Set additional configuration
    4. Generate traffic
    5. Restore configuration
*/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Set configuration */
    prvTgfPclIpv4UdpQosConfigSet();

    /* Generate traffic */
    prvTgfPclIpv4UdpQosTrafficGenerate();

    /* Set additional configuration */
    prvTgfPclIpv4UdpQosAdditionalConfigSet();

    /* Generate traffic */
    prvTgfPclIpv4UdpQosAdditionalTrafficGenerate();

    /* Restore configuration */
    prvTgfPclIpv4UdpQosConfigRestore();

    prvTgfPclRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclUdbFldsIsIp
*/

UTF_TEST_CASE_MAC(tgfPclUdbFldsIsIp)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_IS_IP_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E);
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_IS_IP_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclUdbFldsPacketDepend
*/

UTF_TEST_CASE_MAC(tgfPclUdbFldsPacketDepend)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* common tests note 1 */
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_VID_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E);
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_VID_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);

    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_UP_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E);
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_UP_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);

    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_DSCP_OR_EXP_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E);
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_DSCP_OR_EXP_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);

    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_MAC_DA_TYPE_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E);
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_MAC_DA_TYPE_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);

    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_L4_PROTOCOL_TYPE_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E);
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_L4_PROTOCOL_TYPE_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);

    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_PKT_TAGGING_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E);
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_PKT_TAGGING_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);

#if 0
    /* not debugged cases */
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_PKT_TYPE_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E);
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_PKT_TYPE_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);

    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_IS_UDB_VALID_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E);
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_IS_UDB_VALID_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);

    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_L3_OFFSET_INVALIDATE_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E);
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_L3_OFFSET_INVALIDATE_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);

    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_IP_HEADER_OK_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E);
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_IP_HEADER_OK_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);

    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_L4_OFFSET_INVALID_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E);
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_L4_OFFSET_INVALID_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);

    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_L2_ENCAPSULATION_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E);
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_L2_ENCAPSULATION_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);

    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_IS_IPV6_EH_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E);
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_IS_IPV6_EH_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);

    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_IS_IPV6_HOP_BY_HOP_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E);
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_IS_IPV6_HOP_BY_HOP_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);
#endif
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclUdbFldsNotPacketDepend
*/

UTF_TEST_CASE_MAC(tgfPclUdbFldsNotPacketDepend)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* common tests note 2 */
    prvTgfPclUdbFldsTwoPorts(PRV_TGF_PCL_RULE_FORMAT_PCL_ID_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E);
    prvTgfPclUdbFldsTwoPorts(PRV_TGF_PCL_RULE_FORMAT_PCL_ID_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);

    prvTgfPclUdbFldsTwoPorts(PRV_TGF_PCL_RULE_FORMAT_SOURCE_PORT_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E);
    prvTgfPclUdbFldsTwoPorts(PRV_TGF_PCL_RULE_FORMAT_SOURCE_PORT_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclUdbFldsMacToMe
*/

UTF_TEST_CASE_MAC(tgfPclUdbFldsMacToMe)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* common tests note 3 */
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_MAC_TO_ME_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E);
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_MAC_TO_ME_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);

}

/*----------------------------------------------------------------------------*/
/*
    tgfPclUdbFldsComplicated
*/

UTF_TEST_CASE_MAC(tgfPclUdbFldsComplicated)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(
        prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E
        | UTF_CH2_E | UTF_CH3_E | UTF_XCAT2_E);

    if(GT_FALSE == prvTgfBrgTtiCheck())
    {
        return;
    }

    /* common tests note 3 */
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_IS_L2_VALID_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E);
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_IS_L2_VALID_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclUdbFldsStd
*/

UTF_TEST_CASE_MAC(tgfPclUdbFldsStd)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* std tests only */
    prvTgfPclUdbFlds(
        PRV_TGF_PCL_RULE_FORMAT_IS_IPV4_E,
        PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E);
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclUdbFldsExt
*/

UTF_TEST_CASE_MAC(tgfPclUdbFldsExt)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* ext tests only */
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_IS_IPV6_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_IP_PROTOCOL_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_SIP_BITS_31_0_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_SIP_BITS_79_32_OR_MAC_SA_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_SIP_BITS_127_80_OR_MAC_DA_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_DIP_BITS_127_112_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);
    prvTgfPclUdbFlds(PRV_TGF_PCL_RULE_FORMAT_DIP_BITS_31_0_E, PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E);
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclBypassBridge
*/

UTF_TEST_CASE_MAC(tgfPclBypassBridge)
{
    /* Test of Bypass bridge feature */
    prvTgfPclBypass(PRV_TGF_PCL_BYPASS_BRIDGE_E);
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclBypassIngressPipe
*/

UTF_TEST_CASE_MAC(tgfPclBypassIngressPipe)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Test of Bypass ingress pipe feature */
    prvTgfPclBypass(PRV_TGF_PCL_BYPASS_INGRESS_PIPE_E);
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclTcpRstFinMirroring
*/

UTF_TEST_CASE_MAC(tgfPclTcpRstFinMirroring)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Test of action TCP RST/FIN mirroring feature */
    prvTgfPclTcpRstFinMirroring();
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclPolicerCounting
*/

UTF_TEST_CASE_MAC(tgfPclPolicerCounting)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    if(GT_FALSE == prvTgfPclPolicerCheck())
    {
        /* the device may support policer but not from the PCL/TTI */
        return;
    }


    /* Test of action Policer Counting only */
    prvTgfPclPolicerCounting();
}


/*----------------------------------------------------------------------------*/
/*
    tgfPclPolicerMetering
*/

UTF_TEST_CASE_MAC(tgfPclPolicerMetering)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    if(GT_FALSE == prvTgfPclPolicerCheck())
    {
        /* the device may support policer but not from the PCL/TTI */
        return;
    }

    /* Test of action Policer Metering only */
    prvTgfPclPolicerMetering();
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclPortGroupPolicerCounting
*/

UTF_TEST_CASE_MAC(tgfPclPortGroupPolicerCounting)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* Test of action Policer Counting only */
    prvTgfPclPortGroupPolicerCounting();
}


/*----------------------------------------------------------------------------*/
/*
    tgfPclPortGroupPolicerMetering
*/

UTF_TEST_CASE_MAC(tgfPclPortGroupPolicerMetering)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* Test of action Policer Metering only */
    prvTgfPclPortGroupPolicerMetering();
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclFullLookupControl
*/

UTF_TEST_CASE_MAC(tgfPclFullLookupControl)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Test of action Full lookup control */
    prvTgfPclFullLookupControl();
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclRedirectWithMacDaModify
*/

UTF_TEST_CASE_MAC(tgfPclRedirectWithMacDaModify)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* Test of redirection with MAC_DA modification */
    prvTgfPclRedirectMacDaModify();
}

/*----------------------------------------------------------------------------*/
/*
    tgfPclRedirectWithMacSaModify
*/

UTF_TEST_CASE_MAC(tgfPclRedirectWithMacSaModify)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* Test of redirection with MAC_SA modification */
    prvTgfPclRedirectMacSaModify();
}

/*----------------------------------------------------------------------------*/
/*
    prvTgfPclOverrideUserDefinedBytesByTrunkHashSet
*/

UTF_TEST_CASE_MAC(prvTgfPclOverrideUserDefinedBytesByTrunkHashSet)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* Test of Trunk Hash Value use */
    prvTgfPclTrunkHashTrafficTest();
}

/*----------------------------------------------------------------------------*/
/*
    prvTgfPclUdeEtherTypeSet
*/

UTF_TEST_CASE_MAC(prvTgfPclUdeEtherTypeSet)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* Test of UDE Packet types use */
    prvTgfPclUdePktTypesTrafficTest();
}

/*----------------------------------------------------------------------------*/
/*
    prvTgfPclEgressKeyFieldsVidUpModeSet
*/

UTF_TEST_CASE_MAC(prvTgfPclEgressKeyFieldsVidUpModeSet)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* Test of UDE Packet types use */
    prvTgfPclEgrOrgVidTrafficTest();
}

/*----------------------------------------------------------------------------*/
/*
    prvTgfPclLookup00And01
*/
UTF_TEST_CASE_MAC(prvTgfPclLookup00And01)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(
        prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E |
        UTF_CH3_E | UTF_XCAT_E| UTF_LION_E);

    /* Test of xCat2 lookup0_0 and Lookup0_1 different PCL keys */
    prvTgfPclLookup00And01();
}

/*----------------------------------------------------------------------------*/
/*
    prvTgfPclPortListMode
*/
UTF_TEST_CASE_MAC(prvTgfPclPortListMode)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(
        prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E |
        UTF_CH3_E | UTF_XCAT_E| UTF_LION_E);

    /* Test of xCat2 port list mode */
    prvTgfPclPortListMode();
}

/*----------------------------------------------------------------------------*/
/*
    prvTgfPclWriteRuleInvalid
*/
UTF_TEST_CASE_MAC(prvTgfPclWriteRuleInvalid)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(
        prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Test of invalid rule writing */
    prvTgfPclWriteRuleInvalid();
}

/*----------------------------------------------------------------------------*/
/*
    prvTgfPclFourLookups
*/
UTF_TEST_CASE_MAC(prvTgfPclFourLookups)
{
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(
        prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Test of four lookups packet matching */
    prvTgfPclFourLookups();
}

/*
 * Configuration of tgfPcl suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(tgfPcl)
    UTF_SUIT_DECLARE_TEST_MAC(tgfIngressPclKeyStdL2)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclValidInvalidRulesBasicExtended)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclValidInvalidRulesActionUpdate)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclSecondLookupMatch)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclBothUserDefinedBytes)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclUserDefinedBytes)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclIpv4TcpUdb)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclIpv4UdpUdb)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclIpv4FragmentUdb)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclIpv4OtherUdb)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclEthernetOtherUdb)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclMplsUdb)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclUdeUdb)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclIpv6Udb)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclIpv6TcpUdb)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclEthernetOtherVridUdb)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclIpv4UdpQosUdb)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclUdbFldsIsIp)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclUdbFldsPacketDepend)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclUdbFldsNotPacketDepend)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclUdbFldsMacToMe)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclUdbFldsComplicated)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclUdbFldsStd)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclUdbFldsExt)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclBypassBridge)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclBypassIngressPipe)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclTcpRstFinMirroring)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclPolicerCounting)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclPolicerMetering)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclPortGroupPolicerCounting)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclPortGroupPolicerMetering)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclFullLookupControl)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclRedirectWithMacDaModify)
    UTF_SUIT_DECLARE_TEST_MAC(tgfPclRedirectWithMacSaModify)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfPclOverrideUserDefinedBytesByTrunkHashSet)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfPclUdeEtherTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfPclEgressKeyFieldsVidUpModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfPclLookup00And01)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfPclPortListMode)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfPclWriteRuleInvalid)
    UTF_SUIT_DECLARE_TEST_MAC(prvTgfPclFourLookups)
UTF_SUIT_END_TESTS_MAC(tgfPcl)

