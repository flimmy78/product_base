/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChHwTables.c
*
* DESCRIPTION:
*     Private API implementation for tables access of the Cheetah and Cheetah 2.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwTables.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/* max num of vlans entries in Mapping table -- range 0..4095 */
#define PRV_CPSS_DXCH_MAX_NUM_VLANS_CNS (4096)

#define PRV_CPSS_DXCH_TABLE_SIZE_GET_MAC(devNum) \
           (PRV_CPSS_DXCH_PP_MAC(devNum)->accessTableInfoSize)

/*******************************************************************************
* PRV_TABLE_INFO_PTR_GET_MAC
*
* DESCRIPTION:
*       get the table info
* INPUTS:
*       devNum - device number
*       tableType - the table to get info
* OUTPUTS:
*       NONE
*
* RETURNS:
*       pointer to the table's info
*
* COMMENTS:
*
*
*******************************************************************************/
#define PRV_TABLE_INFO_PTR_GET_MAC(_devNum, _tableType) \
 (&PRV_CPSS_DXCH_PP_MAC(_devNum)->accessTableInfoPtr[_tableType])

/* access to info of the DxCh device tables */
#define PRV_CPSS_DXCH_DEV_TBLS_MAC(devNum)  \
    (PRV_CPSS_DXCH_PP_MAC(devNum)->accessTableInfoPtr)
/* max number of words in entry */
#define MAX_ENTRY_SIZE_CNS  16

/* for indirect information table */
/* for cheetah2 only */
#define PRV_CPSS_DXCH2_TABLE_STP_E       (PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E + 1)
#define PRV_CPSS_DXCH2_TABLE_VLAN_E      (PRV_CPSS_DXCH2_TABLE_STP_E + 1)
#define PRV_CPSS_DXCH2_TABLE_MULTICAST_E (PRV_CPSS_DXCH2_TABLE_VLAN_E + 1)

/* This value is used when control registor relates only to one table.
   In this case there is no need to update table index in control register.
   The action that is done for all tables is: table index << table index offset
   0 << 0 doesn't have any impact */
#define PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS           0

/* number of words in the cheetah QoS profile entry */
#define PRV_CPSS_DXCH_QOS_PROFILE_ENTRY_WORDS_COUNT_CNS       1


/* number of entries in cheetah vlan table */
#define PRV_CPSS_DXCH_MAX_VLANS_NUM_CNS                       4096
/* number of words in the cheetah vlan entry */
#define PRV_CPSS_DXCH_VLAN_ENTRY_WORDS_COUNT_CNS              3
#define PRV_CPSS_DXCH2_VLAN_ENTRY_WORDS_COUNT_CNS             4
#define PRV_CPSS_DXCHXCAT_VLAN_ENTRY_WORDS_COUNT_CNS          6

/* number of words in the cheetah multicast entry */
#define PRV_CPSS_DXCH_MULTICAST_ENTRY_WORDS_COUNT_CNS         1

/* number of words in the cheetah mac sa entry */
#define PRV_CPSS_DXCH_ROUTE_HA_MAC_SA_ENTRY_WORDS_COUNT_CNS   1


#define PRV_CPSS_DXCH_PORT_TBLS_MAX_INDEX_CNS 64

/* 16K */
#define PRV_CPSS_DXCH_FDB_MAX_ENTRIES_CNS                0x4000
#define PRV_CPSS_DXCH_FDB_MAC_ENTRY_WORDS_SIZE_CNS       4

/* 32K */
#define PRV_CPSS_DXCH3_FDB_MAX_ENTRIES_CNS                0x8000

/* number of entries in cheetah policer table */
#define PRV_CPSS_DXCH_POLICER_MAX_ENTRIES_CNS                256
/* number of words in cheetah policer entry*/
#define PRV_CPSS_DXCH_POLICER_ENTRY_WORDS_SIZE_CNS           2

/* number of entries in cheetah policer counters table */
#define PRV_CPSS_DXCH_POLICER_COUNTERS_MAX_ENTRIES_CNS      16
/* number of words in cheetah policer counters entry*/
#define PRV_CPSS_DXCH_POLICER_COUNTERS_ENTRY_WORDS_SIZE_CNS  2

/* 8 registers are in the same entry */
#define PRV_CPSS_DXCH_PRTCL_BASED_VLAN_ENTRY_WORDS_COUNT_CNS   8
#define PRV_CPSS_DXCHXCAT_PRTCL_BASED_VLAN_ENTRY_WORDS_COUNT_CNS   1

/* number of byte in the cheetah port vlan qos entry */
#define PRV_CPSS_DXCH_VLAN_PORT_QOS_ENTRY_WORDS_COUNT_CNS     2

/* 128 trunks * 8 members in trunk = 1024 */
#define PRV_CPSS_DXCH_TRUNK_MEMBERS_TABLE_MAX_ENTRIES_CNS     1024

/* number of words in the cheetah trunk entry */
#define PRV_CPSS_DXCH_TRUNK_ENTRY_WORDS_COUNT_CNS             1

/* number of entries in cheetah rate limit table */
#define PRV_CPSS_DXCH_RATE_LIMIT_TABLE_MAX_ENTRIES_CNS        32

/* number of words in the cheetah rate limit entry */
#define PRV_CPSS_DXCH_RATE_LIMIT_ENTRY_WORDS_COUNT_CNS        1

/* number of entries in cheetah pcl config table */
#define PRV_CPSS_DXCH_PCL_CONFIG_TABLE_MAX_ENTRIES_CNS        1152
#define PRV_CPSS_DXCH2_PCL_CONFIG_TABLE_MAX_ENTRIES_CNS       4224

#define PRV_CPSS_DXCH2_EGRESS_PCL_CONFIG_TABLE_MAX_ENTRIES_CNS 4160

/* number of words in the cheetah pcl config entry */
#define PRV_CPSS_DXCH_PCL_CONFIG_ENTRY_WORDS_COUNT_CNS        1

/* number of words in the cheetah pcl config entry */
#define PRV_CPSS_DXCH3_PCL_CONFIG_ENTRY_WORDS_COUNT_CNS        2

/* number of entries in cheetah arp da table */
#define PRV_CPSS_DXCH_ROUTE_HA_ARP_DA_TABLE_MAX_ENTRIES_CNS   1024

/* number of words in the cheetah arp da entry */
#define PRV_CPSS_DXCH_ROUTE_HA_ARP_DA_ENTRY_WORDS_COUNT_CNS   2
#define PRV_CPSS_DXCH2_ROUTE_HA_ARP_DA_ENTRY_WORDS_COUNT_CNS  6

/* number of words in the cheetah2 QoSProfile-to-Route-Block-Offset entry */
#define PRV_CPSS_DXCH2_QOS_TO_ROUTE_BLOCK_WORDS_COUNT_CNS   1
/* number of entries in the cheetah2 QoSProfile-to-Route-Block-Offset table */
#define PRV_CPSS_DXCH2_QOS_TO_ROUTE_BLOCK_TABLE_MAX_ENTRIES_CNS    16

/* number of words in the cheetah2 Router Access Matrix Line entry */
#define PRV_CPSS_DXCH2_ROUTE_ACCESS_MATRIX_WORDS_COUNT_CNS   1
/* number of entries in the cheetah2 Router Access Matrix Line table */
#define PRV_CPSS_DXCH2_ROUTE_ACCESS_MATRIX_TABLE_MAX_ENTRIES_CNS    8

/* number of words in LTT and TT Action Table entry */
#define PRV_CPSS_DXCH2_LTT_TT_ACTION_WORDS_COUNT_CNS   4
/* number of entries in the cheetah2 LTT and TT Action table */
#define PRV_CPSS_DXCH2_LTT_TT_ACTION_TABLE_MAX_ENTRIES_CNS    1024

/* number of words in Unicast/Multicast Router Next Hop  entry */
#define PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_WORDS_COUNT_CNS   4
/* number of entries in the cheetah2 Unicast/Multicast Router Next Hop table */
#define PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_TABLE_MAX_ENTRIES_CNS    4096

/* number of words in yhe cheetah2 Router Next Hop Table Age Bits entry */
#define PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_COUNT_CNS   1
/* number of entries in the cheetah2 Router Next Hop Table Age Bits Entry table */
#define PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_TABLE_MAX_ENTRIES_CNS    128
/* number of words in the cheetah3 mac2me table */
#define PRV_CPSS_DXCH3_MAC2ME_WORDS_COUNT_CNS   4
/* number of entries in the cheetah3 mac2me table */
#define PRV_CPSS_DXCH3_MAC2ME_TABLE_MAX_ENTRIES_CNS    8
/* number of words in the chhetah3 vlan translation table (ingress/egress) */
#define PRV_CPSS_DXCH3_VLAN_TRANSLATION_WORDS_COUNT_CNS     1
/* number of entries in the cheetah3 vlan translation tables (ingress/egress) */
#define PRV_CPSS_DXCH3_VLAN_TRANSLATION_TABLE_MAX_ENTRIES_CNS       4096
/* number of words in the chhetah3 VRF ID table */
#define PRV_CPSS_DXCH3_VRF_ID_WORDS_COUNT_CNS     1
/* number of entries in the cheetah3 VRF ID table */
#define PRV_CPSS_DXCH3_VRF_ID_TABLE_MAX_ENTRIES_CNS       4096
/* number of words in LTT and TT Action Table entry (Ch3) */
#define PRV_CPSS_DXCH3_LTT_TT_ACTION_WORDS_COUNT_CNS    4
/* number of entries in the cheetah3 LTT and TT Action table */
#define PRV_CPSS_DXCH3_LTT_TT_ACTION_TABLE_MAX_ENTRIES_CNS    1024
/* number of words in LTT and TT Action Table entry (xCat) */
#define PRV_CPSS_XCAT_LTT_TT_ACTION_WORDS_COUNT_CNS    5

/* number of entries in cheetah cpu code table */
#define PRV_CPSS_DXCH_CPU_CODE_TABLE_MAX_ENTRIES_CNS          256

/* number of words in the cheetah cpu code entry */
#define PRV_CPSS_DXCH_CPU_CODE_ENTRY_WORDS_COUNT_CNS          1

/* STG entry size -- in words */
#define PRV_CPSS_DXCH_STG_ENTRY_WORDS_SIZE_CNS           2

/* cheetah default value for next word offset for table direct access */
#define PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS           4

typedef enum {
    SPECIAL_ADDRESS_NOT_MODIFIED_E = 0x0EEEEEEE,/* unchanged address indication */
    SPECIAL_ADDRESS_NOT_EXISTS_E   = 0x0FFFFFFF /* non exists address */
}SPECIAL_ADDRESS_ENT;

#define TABLES_INFO_DIRECT_NOT_EXISTS_CNS   \
    SPECIAL_ADDRESS_NOT_EXISTS_E, 0 , 0

#define TABLES_INFO_DIRECT_NOT_MODIFIED_CNS   \
    SPECIAL_ADDRESS_NOT_MODIFIED_E, 0 , 0

static PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC indirectAccessTableInfo[] =
{
    /* PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E */
    {0x0B800328,  0x0B800300,   0,  2,  2,  10,  1},

    /* PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E */
    {0x0B800328,  0x0B800320,   0,  2,  1,  10,  1},

    /* PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E */
    {0x0B000028,  0x0B001000,   0,  2, PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS,
          PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS, 1},

    /* PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E */
    {0x0B000034,  0x0B004000,   0,  2, PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS,
          PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS, 1},

    /* PRV_CPSS_DXCH_TABLE_CPU_CODE_E */
    {0x0B000030,  0x0B003000,   0,  2, PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS,
          PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS, 1},

    /* PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E */
    {0x0B800204,  0x0B800200,   0,  2, PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS,
          PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS, 1},

    /* PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E */
    {0x0B00002C,  0x0B002000,   0,  2, PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS,
          PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS, 1},

    /* PRV_CPSS_DXCH_TABLE_REMARKING_E */
    {0x0C000024,  0x0C000020,   0 , 2, PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS,
          PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS, 1},

    /* PRV_CPSS_DXCH_TABLE_STG_E */
    {0x0A00000C,  0x0A000004,   15, 0,  2,  13,  12},

    /* PRV_CPSS_DXCH_TABLE_VLAN_E */
    {0x0A00000C,  0x0A000000,   15, 0,  0,  13,  12},

    /* PRV_CPSS_DXCH_TABLE_MULTICAST_E */
    {0x0A00000C,  0x0A000008,   15, 0,  1,  13,  12},

    /* PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E */
    {0x07800208,  0x07800204,   0,  3,  1,  2,   1},

    /* PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E */
    {0x07800208,  0x07800200,   0,  3,  0,  2,   1},

    /* PRV_CPSS_DXCH_TABLE_FDB_E */
    {0x06000064,  0x06000054,   0,  2, PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS,
          PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS, 1},

    /* PRV_CPSS_DXCH_TABLE_POLICER_E */
    {0x0C000014,  0x0C00000C,   0,  2, PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS,
          PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS, 1},

    /* PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E */
    {0x0C000038,  0x0C000030,   0,  2, PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS,
          PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS, 1},

    /* PRV_CPSS_DXCH2_TABLE_STP_E */
    {0x0A000010,  0x0A000008,   15, 0,  2,  13,  12},

    /* PRV_CPSS_DXCH2_TABLE_VLAN_E */
    {0x0A000010,  0x0A000000,   15, 0,  0,  13,  12},

    /* PRV_CPSS_DXCH2_TABLE_MULTICAST_E */
    {0x0A000010,  0x0A00000C,   15, 0,  1,  13,  12}

};

/* Cheetah's 2 and 3 only direct table information */
static PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC directAccessTableInfo[] =
{
    /* PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E */
    {0x0B810800, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E */
    {0x0B810000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E */
    {0x0B400000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E */
    {0x0B100000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_CPU_CODE_E */
    {0x0B200000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E */
    {0x0B840000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E */
    {0x0B300000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_REMARKING_E - used for CH3 only,
       CH2 uses indirect access, the base address is for CH3 only */
    {0x0c080000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_STG_E */
    {0x0A100000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_VLAN_E */
    {0x0A400000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_MULTICAST_E */
    {0x0A200000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E */
    {0x07F80000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E */
    {0x07E80000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_FDB_E */
    {0x06400000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_POLICER_E */
    {0x0C100000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E */
    {0x0C300000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E */
    {0x07700000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E */
    {0x07E80000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH2_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E */
    {0x02800300, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH2_TABLE_ROUTE_ACCESS_MATRIX_E */
    {0x02800440, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH2_LTT_TT_ACTION_E */
    {0x02900000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E */
    {0x02B00000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_E */
    {0x02801000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH3_TABLE_MAC2ME_E */
    {0xB800700, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH3_TABLE_INGRESS_VLAN_TRANSLATION_E */
    {0x0B804000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E */
    {0x07FC0000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH3_TABLE_VRF_ID_E */
    {0x0A300000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH3_LTT_TT_ACTION_E */
    {0x02900000, 0x4, 0x5000}
};

/* CH3 Ingress PCL configuration table direct info */
static PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC dxCh3IpclCfgTblInfo =
    {0x0B840000, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS};

/* CH3 Egress PCL configuration table direct info */
static PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC dxCh3EpclCfgTblInfo =
    {0x07f00000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS};

/*
 * typedef: enum PRV_DXCH_XCAT_TABLES_E
 *
 * Description:
 *    Index for directAccessXcatTableInfo table
 *
 * Enumerations:
 *    PRV_DXCH_XCAT_TABLE_VLAN_E                - VLAN table index
 *    PRV_DXCH_XCAT_TABLE_VRF_ID_E              - VRD-ID table index
 *    PRV_DXCH_XCAT_TABLE_PORT_VLAN_QOS_E       - Port VLAN QoS table index
 *    PRV_DXCH_XCAT_TABLE_STG_E                 - STG table index
 *    PRV_DXCH_XCAT_LTT_TT_ACTION_E             - LTT action table index
 *    PRV_DXCH_XCAT_TABLE_MULTICAST_E           - Multicast table index
 *    PRV_CPSS_XCAT_TABLE_ROUTE_HA_MAC_SA_E     - Router HA MAC SA table index
 *    PRV_CPSS_XCAT_TABLE_ROUTE_HA_ARP_DA_E     - Router HA ARP DA table index
 *    PRV_DXCH_XCAT_TABLE_INGRESS_TRANSLATION_E - Ingress VLAN translation table index
 *    PRV_DXCH_XCAT_TABLE_EGRESS_TRANSLATION_E  - Egress VLAN translation table index
 *    PRV_DXCH_XCAT_TABLE_IPCL_CFG_E            - Ingress PCL configuration table
 *    PRV_DXCH_XCAT_TABLE_IPCL_LOOKUP1_CFG_E    - Ingress PCL Lookup1
 *                                                configuration table
 *    PRV_DXCH_XCAT_TABLE_EPCL_CFG_E            - Egress PCL configuration table
 *    PRV_DXCH_XCAT_TABLE_IPCL_UDB_CFG_E        - Ingress PCL UDB Cfg table
 *    PRV_DXCH_XCAT_TABLE_TUNNEL_START_CFG_E    - Tunnel Start table
 *    PRV_DXCH_XCAT_TABLE_VLAN_PORT_PROTOCOL_E  - Port Protocol VID and QoS
 *                                                Configuration Table
 *    PRV_CPSS_XCAT_TABLE_MAC2ME_E              - TTI MAC2ME table
 *    PRV_DXCH_XCAT_TABLE_BCN_PROFILE_E         - BCN Profiles Table
 *    PRV_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E - Egress Policer Remarking table
 */
typedef enum {
    PRV_DXCH_XCAT_TABLE_VLAN_E = 0,
    PRV_DXCH_XCAT_TABLE_VRF_ID_E,
    PRV_DXCH_XCAT_TABLE_PORT_VLAN_QOS_E,
    PRV_DXCH_XCAT_TABLE_STG_E,
    PRV_DXCH_XCAT_LTT_TT_ACTION_E,
    PRV_DXCH_XCAT_TABLE_MULTICAST_E,
    PRV_CPSS_XCAT_TABLE_ROUTE_HA_MAC_SA_E,
    PRV_CPSS_XCAT_TABLE_ROUTE_HA_ARP_DA_E,
    PRV_DXCH_XCAT_TABLE_INGRESS_TRANSLATION_E,
    PRV_DXCH_XCAT_TABLE_EGRESS_TRANSLATION_E,
    PRV_DXCH_XCAT_TABLE_IPCL_CFG_E,
    PRV_DXCH_XCAT_TABLE_IPCL_LOOKUP1_CFG_E,
    PRV_DXCH_XCAT_TABLE_EPCL_CFG_E,
    PRV_DXCH_XCAT_TABLE_IPCL_UDB_CFG_E,
    PRV_DXCH_XCAT_TABLE_TUNNEL_START_CFG_E,
    PRV_DXCH_XCAT_TABLE_VLAN_PORT_PROTOCOL_E,
    PRV_CPSS_XCAT_TABLE_MAC2ME_E,
    PRV_DXCH_XCAT_TABLE_BCN_PROFILE_E,
    PRV_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E
}PRV_DXCH_XCAT_TABLES_E;

/*
 * typedef: enum PRV_DXCH_LION_TABLES_E
 *
 * Description:
 *    Index for directAccessLionBTableInfoExt table
 *
 * Enumerations:
 *      PRV_DXCH_LION_TABLE_START_E         - index of last xCat table
 *      PRV_DXCH_LION_TABLE_VLAN_INGRESS_E  - Ingress VLAN table index
 *      PRV_DXCH_LION_TABLE_VLAN_EGRESS_E   - Egress VLAN table index
 *      PRV_DXCH_LION_TABLE_STG_INGRESS_E   - Ingress STG table index
 *      PRV_DXCH_LION_TABLE_STG_EGRESS_E    - Egress STG table index
 *      PRV_CPSS_LION_TABLE_PORT_ISOLATION_L2_E - L2 port isolation table index
 *      PRV_CPSS_LION_TABLE_PORT_ISOLATION_L3_E - L3 port isolation table index
 *      PRV_CPSS_LION_TABLE_TXQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E -
 *                                                shaper per port per priority
 *                                                table index
 *      PRV_CPSS_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E-
 *                                                shaper per port table index
 *      PRV_CPSS_LION_TABLE_TXQ_SOURCE_ID_MEMBERS_E - Source ID table index
 *      PRV_CPSS_LION_TABLE_TXQ_NON_TRUNK_MEMBERS_E - Non trunk table index
 *      PRV_CPSS_LION_TABLE_TXQ_DESIGNATED_PORT_E   - Trunk designated table index
 *      PRV_CPSS_LION_TABLE_TXQ_EGRESS_STC_E        - Egress STC table index
 *      PRV_CPSS_LION_TABLE_ROUTER_VLAN_URPF_STC_E  - VLAN URPF table index
 *      PRV_CPSS_LION_TABLE_TRUNK_HASH_MASK_CRC_E   - Trunk Hash mask table index
 */
typedef enum {
    PRV_DXCH_LION_TABLE_START_E = PRV_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E,

    /* new Lion tables */

    PRV_DXCH_LION_TABLE_VLAN_INGRESS_E,
    PRV_DXCH_LION_TABLE_VLAN_EGRESS_E,

    PRV_DXCH_LION_TABLE_STG_INGRESS_E,
    PRV_DXCH_LION_TABLE_STG_EGRESS_E,

    PRV_CPSS_LION_TABLE_PORT_ISOLATION_L2_E,
    PRV_CPSS_LION_TABLE_PORT_ISOLATION_L3_E,

    PRV_CPSS_LION_TABLE_TXQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E,
    PRV_CPSS_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E,

    PRV_CPSS_LION_TABLE_TXQ_SOURCE_ID_MEMBERS_E,
    PRV_CPSS_LION_TABLE_TXQ_NON_TRUNK_MEMBERS_E,
    PRV_CPSS_LION_TABLE_TXQ_DESIGNATED_PORT_E,

    PRV_CPSS_LION_TABLE_TXQ_EGRESS_STC_E,
    PRV_CPSS_LION_TABLE_ROUTER_VLAN_URPF_STC_E,
    PRV_CPSS_LION_TABLE_TRUNK_HASH_MASK_CRC_E

}PRV_DXCH_LION_TABLES_E;

/*
 * typedef: enum PRV_DXCH_XCAT2_TABLES_E
 *
 * Description:
 *    Index for directAccessXCAT2TableInfo table
 *
 * Enumerations:
 *    PRV_DXCH_XCAT2_TABLE_IPCL_LOOKUP01_CFG_E  - IPCL Lookup 0_1 
 *                                                Configuration table index
 *    PRV_DXCH_XCAT2_TABLE_IPCL_LOOKUP1_CFG_E   - IPCL Lookup 1 
 *                                                Configuration table index
 *    PRV_DXCH_XCAT2_TABLE_MAC2ME_E             - MAC-2-Me table index
*/
typedef enum {
    PRV_DXCH_XCAT2_TABLE_IPCL_LOOKUP01_CFG_E = 0,
    PRV_DXCH_XCAT2_TABLE_IPCL_LOOKUP1_CFG_E,
    PRV_DXCH_XCAT2_TABLE_MAC2ME_E
}PRV_DXCH_XCAT2_TABLES_E;

/* Xcat revision 1 and above direct table information */
static PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC directAccessXcatTableInfo[] =
{
    /* PRV_DXCH_XCAT_TABLE_VLAN_E */
    {0x0A200000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_TABLE_VRF_ID_E */
    {0x0A180000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_TABLE_PORT_VLAN_QOS_E */
    {0x0c001000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_TABLE_STG_E */
    {0x0A080000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_LTT_TT_ACTION_E */
    {0x0e8c0000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_TABLE_MULTICAST_E */
    {0x0A100000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_XCAT_TABLE_ROUTE_HA_MAC_SA_E */
    {0x0F008000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_XCAT_TABLE_ROUTE_HA_ARP_DA_E */
    {0x0F040000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_TABLE_INGRESS_TRANSLATION_E */
    {0x0C00C000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_TABLE_EGRESS_TRANSLATION_E */
    {0x0F010000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_TABLE_IPCL_CFG_E */
    {0x0B810000, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_TABLE_IPCL_LOOKUP1_CFG_E */
    {0x0B820000, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_TABLE_EPCL_CFG_E */
    {0x0F808000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_TABLE_IPCL_UDB_CFG_E */
    {0x0C010000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_TABLE_TUNNEL_START_CFG_E */
    {0x0F040000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_TABLE_VLAN_PORT_PROTOCOL_E */
    {0x0C004000, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_XCAT_TABLE_MAC2ME_E */
    {0x0C001600, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /*PRV_DXCH_XCAT_TABLE_BCN_PROFILE_E */
    {0x0A400100, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /*PRV_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E*/
    {0x03880000, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},
};

typedef struct{
    PRV_CPSS_DXCH_TABLE_ENT                 globalIndex;
    PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC    directAccessInfo;
}PRV_CPSS_DXCH_TABLES_INFO_DIRECT_EXT_STC;

/* Lion B and above direct table information */
static PRV_CPSS_DXCH_TABLES_INFO_DIRECT_EXT_STC directAccessLionBTableInfoExt[] =
{
    {PRV_CPSS_DXCH_TABLE_VLAN_E,
    /* PRV_DXCH_XCAT_TABLE_VLAN_E */
    {  TABLES_INFO_DIRECT_NOT_EXISTS_CNS }}, /* see ingress/egress tables instead */

    {PRV_CPSS_DXCH3_TABLE_VRF_ID_E,
    /* PRV_DXCH_XCAT_TABLE_VRF_ID_E */
    {  TABLES_INFO_DIRECT_NOT_EXISTS_CNS }}, /* the field is in ingress vlan entry */

    {PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
    /* PRV_DXCH_XCAT_TABLE_PORT_VLAN_QOS_E */
    {0x01001000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_TABLE_STG_E,
    /* PRV_CPSS_DXCH_TABLE_STG_E */
    {  TABLES_INFO_DIRECT_NOT_EXISTS_CNS }}, /* see ingress/egress tables instead */

    {PRV_CPSS_DXCH3_LTT_TT_ACTION_E,
    /* PRV_DXCH_XCAT_LTT_TT_ACTION_E */
    {0x0D8C0000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_TABLE_MULTICAST_E,
    /* PRV_DXCH_XCAT_TABLE_MULTICAST_E */
    {0x11860000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E,
    /*PRV_CPSS_XCAT_TABLE_ROUTE_HA_MAC_SA_E,         */
    {0x0E808000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E,
    /*PRV_CPSS_XCAT_TABLE_ROUTE_HA_ARP_DA_E,         */
    {0x0E840000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH3_TABLE_INGRESS_VLAN_TRANSLATION_E,
    /* PRV_DXCH_XCAT_TABLE_INGRESS_TRANSLATION_E */
    {0x0100C000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E,
    /*PRV_DXCH_XCAT_TABLE_EGRESS_TRANSLATION_E,      */
    {0x0E810000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E,
    /*PRV_DXCH_XCAT_TABLE_IPCL_CFG_E,                */
    {TABLES_INFO_DIRECT_NOT_MODIFIED_CNS}},

    {PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_LOOKUP1_CONFIG_E,
    /*PRV_DXCH_XCAT_TABLE_IPCL_LOOKUP1_CFG_E,        */
    {TABLES_INFO_DIRECT_NOT_MODIFIED_CNS}},

    {PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E,
    /*PRV_DXCH_XCAT_TABLE_EPCL_CFG_E,                */
    {0x0E008000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_UDB_CONFIG_E,
    /* PRV_DXCH_XCAT_TABLE_IPCL_UDB_CFG_E */
    {0x01010000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E,
    /*PRV_DXCH_XCAT_TABLE_TUNNEL_START_CFG_E,        */
    {0x0E840000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E,
    /* PRV_DXCH_XCAT_TABLE_VLAN_PORT_PROTOCOL_E */
    {0x01004000, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH3_TABLE_MAC2ME_E,
    /* PRV_CPSS_XCAT_TABLE_MAC2ME_E */
    {0x01001600, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_XCAT_TABLE_BCN_PROFILES_E,
    /*PRV_DXCH_XCAT_TABLE_BCN_PROFILE_E,             */
    {TABLES_INFO_DIRECT_NOT_EXISTS_CNS}},

    {PRV_CPSS_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E,
    /*PRV_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E */
    {0x07880000, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
    /*PRV_DXCH_LION_TABLE_VLAN_INGRESS_E,            */
    {0x118A0000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E,
    /*PRV_DXCH_LION_TABLE_VLAN_EGRESS_E,             */
    {0x11800000, 0x40, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_STG_INGRESS_E,
    /*PRV_DXCH_LION_TABLE_STG_INGRESS_E,             */
    {0x118D0000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_STG_EGRESS_E,
    /*PRV_DXCH_LION_TABLE_STG_EGRESS_E,              */
    {0x11840000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L2_E,
    /*PRV_CPSS_LION_TABLE_PORT_ISOLATION_L2_E*/
    {0x11880000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L3_E,
    /*PRV_CPSS_LION_TABLE_PORT_ISOLATION_L3_E*/
    {0x11890000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E,
    /*PRV_CPSS_LION_TABLE_TXQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E*/
    {0x11002400, 0x40, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E,
    /*PRV_CPSS_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E*/
    {0x11002800, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_TXQ_SOURCE_ID_MEMBERS_E,
    /*PRV_CPSS_LION_TABLE_TXQ_SOURCE_ID_MEMBERS_E*/
    {0x01810500, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_TXQ_NON_TRUNK_MEMBERS_E,
    /*PRV_CPSS_LION_TABLE_TXQ_NON_TRUNK_MEMBERS_E*/
    {0x01810800, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_TXQ_DESIGNATED_PORT_E,
    /*PRV_CPSS_LION_TABLE_TXQ_DESIGNATED_PORT_E*/
    {0x01810200, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E,
    /*PRV_CPSS_LION_TABLE_TXQ_EGRESS_STC_E*/
    {0x11004400, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_ROUTER_VLAN_URPF_STC_E,
    /*PRV_CPSS_DXCH_LION_TABLE_ROUTER_VLAN_URPF_STC_E*/
    {0x02801400, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_TRUNK_HASH_MASK_CRC_E,
    /*PRV_CPSS_LION_TABLE_TRUNK_HASH_MASK_CRC_E*/
    {0x0b800400 , 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},
};

static GT_U32   directAccessLionBTableInfoExtNumEntries =
    sizeof(directAccessLionBTableInfoExt)/sizeof(directAccessLionBTableInfoExt[0]);

/* xCat2 direct table information */
static PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC directAccessXCAT2TableInfo[] =
{
    /*PRV_DXCH_XCAT2_TABLE_IPCL_LOOKUP01_CFG_E*/
    {0x0B820000 , 0x08, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /*PRV_DXCH_XCAT2_TABLE_IPCL_LOOKUP1_CFG_E*/
    {0x0B830000 , 0x08, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT2_TABLE_MAC2ME_E */
    {0xC001600, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}
};

/* XCat A1 revision Logical Target Mapping table indirect info */
static PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC xcatA1LogicalTargetMappingTblInfo =
    {0x0B0000A0, 0x0B00A000, 0, 2, 0, 0, 1};



/* Cheetah's table information */
static PRV_CPSS_DXCH_TABLES_INFO_STC cheetahTablesInfo[] =
{
    /* PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E */
    {PRV_CPSS_DXCH_PORT_TBLS_MAX_INDEX_CNS ,PRV_CPSS_DXCH_PRTCL_BASED_VLAN_ENTRY_WORDS_COUNT_CNS ,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                         PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E,
    PRV_CPSS_DXCH_INDIRECT_ACCESS_E, (indirectAccessTableInfo +
                                       PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E)},

    /* PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E */
    {PRV_CPSS_DXCH_PORT_TBLS_MAX_INDEX_CNS , PRV_CPSS_DXCH_VLAN_PORT_QOS_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                            PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
    PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                            PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E},

    /* PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E */
    {PRV_CPSS_DXCH_TRUNK_MEMBERS_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_TRUNK_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                             PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                             PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E},

    /* PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E */
    {PRV_CPSS_DXCH_RATE_LIMIT_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_RATE_LIMIT_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                     PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E,
    PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                     PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E},

    /* PRV_CPSS_DXCH_TABLE_CPU_CODE_E */
    {PRV_CPSS_DXCH_CPU_CODE_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_CPU_CODE_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                  PRV_CPSS_DXCH_TABLE_CPU_CODE_E,
      PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                  PRV_CPSS_DXCH_TABLE_CPU_CODE_E},

    /* PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E */
    {PRV_CPSS_DXCH_PCL_CONFIG_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_PCL_CONFIG_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                 PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                               PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E},

    /* PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E */
    {PRV_CPSS_DXCH_QOS_PROFILE_NUM_MAX_CNS, PRV_CPSS_DXCH_QOS_PROFILE_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E,
       PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                              PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E},
    /* PRV_CPSS_DXCH_TABLE_REMARKING_E */
    {PRV_CPSS_DXCH_QOS_PROFILE_NUM_MAX_CNS, PRV_CPSS_DXCH_QOS_PROFILE_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                  PRV_CPSS_DXCH_TABLE_REMARKING_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                PRV_CPSS_DXCH_TABLE_REMARKING_E},
    /* PRV_CPSS_DXCH_TABLE_STG_E */
    {PRV_CPSS_DXCH_STG_MAX_NUM_CNS, PRV_CPSS_DXCH_STG_ENTRY_WORDS_SIZE_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                        PRV_CPSS_DXCH_TABLE_STG_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                      PRV_CPSS_DXCH_TABLE_STG_E},

    /* PRV_CPSS_DXCH_TABLE_VLAN_E */
    {PRV_CPSS_DXCH_MAX_VLANS_NUM_CNS, PRV_CPSS_DXCH_VLAN_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                      PRV_CPSS_DXCH_TABLE_VLAN_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                      PRV_CPSS_DXCH_TABLE_VLAN_E},

    /* PRV_CPSS_DXCH_TABLE_MULTICAST_E */
    {PRV_CPSS_DXCH_MAX_VLANS_NUM_CNS, PRV_CPSS_DXCH_MULTICAST_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                 PRV_CPSS_DXCH_TABLE_MULTICAST_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                 PRV_CPSS_DXCH_TABLE_MULTICAST_E},

    /* PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E */
    {PRV_CPSS_DXCH_MAX_VLANS_NUM_CNS,  PRV_CPSS_DXCH_ROUTE_HA_MAC_SA_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                            PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                           PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E},

    /* PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E */
    {PRV_CPSS_DXCH_ROUTE_HA_ARP_DA_TABLE_MAX_ENTRIES_CNS,
        PRV_CPSS_DXCH_ROUTE_HA_ARP_DA_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                           PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                           PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E},

    /* PRV_CPSS_DXCH_TABLE_FDB_E */
    {PRV_CPSS_DXCH_FDB_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_FDB_MAC_ENTRY_WORDS_SIZE_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                       PRV_CPSS_DXCH_TABLE_FDB_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                       PRV_CPSS_DXCH_TABLE_FDB_E},

    /* PRV_CPSS_DXCH_TABLE_POLICER_E */
    {PRV_CPSS_DXCH_POLICER_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_POLICER_ENTRY_WORDS_SIZE_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                   PRV_CPSS_DXCH_TABLE_POLICER_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                   PRV_CPSS_DXCH_TABLE_POLICER_E},

    /* PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E */
    {PRV_CPSS_DXCH_POLICER_COUNTERS_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_POLICER_COUNTERS_ENTRY_WORDS_SIZE_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                          PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                          PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E}

};

static const GT_U32 cheetahTableInfoSize =
                          sizeof(cheetahTablesInfo)/sizeof(cheetahTablesInfo[0]);



/* Cheetah2's table information */
/* All CH2 tables beside FDB and VLAN has direct access by PSS default.
   FDB access is indirect to save entries consistency in the read or
   write under traffic.
   VLAN write access is indirect to save entries consistency in the write under
   traffic only. There is no inconsistency in the read VLAN because this
   table may be changed only by CPU.

   Errata issues:
   1)
   Due to Cheetah 2 Errata - "4.1 Errata (FEr# ) Direct Access to Policer space"
   -- see PRV_CPSS_DXCH2_DIRECT_ACCESS_TO_POLICER_ADDRESS_SPACE_WA_E
   indirect access should be used for:
         PRV_CPSS_DXCH_TABLE_REMARKING_E
         PRV_CPSS_DXCH_TABLE_POLICER_E
         PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E
   2)
   Due to Cheetah 2 Errata - Router ARP and Tunnel Start table do not support
   burst access via PCI (FEr#1003)
   -- see PRV_CPSS_DXCH2_ROUTER_ARP_AND_TS_TABLE_NOT_SUPPORT_BURST_ACCESS_VIA_PCI_WA_E
   The device supports burst reads from its internal memory address
   space. However, due to this erratum, the Router Address
   Resolution Protocol (ARP) and Tunnel Start (TS) table,
   0x07E80000 - 0x07E87FF4, cannot be accessed by using the burst
   access feature.
   NOTE : the CPSS_DXCH currently not support PCI bursts (so no issue here).

   3)
   Due to Cheetah 2 Errata -
   RdWrTrig, bit[15] in the VLT Tables Access Control register is ignored (FEr#1007)
   -- see PRV_CPSS_DXCH2_RDWRTRIG_BIT_IN_VLT_TABLES_ACCESS_CONTROL_REGISTER_IGNORED_E
   No dummy write commands should be done to the VLT Tables Access Control
   register (offset: 0x0A000010).
   NOTE : no "dummy" writes done in the "non-direct" access engine of DXCH


*/

static PRV_CPSS_DXCH_TABLES_INFO_STC cheetah2TablesInfo[] =
{
    /* PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E */
    {PRV_CPSS_DXCH_PORT_TBLS_MAX_INDEX_CNS , PRV_CPSS_DXCH_PRTCL_BASED_VLAN_ENTRY_WORDS_COUNT_CNS ,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                        PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E,
    PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                       PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E},

    /* PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E */
    {PRV_CPSS_DXCH_PORT_TBLS_MAX_INDEX_CNS , PRV_CPSS_DXCH_VLAN_PORT_QOS_ENTRY_WORDS_COUNT_CNS ,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo
                                           + PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
    PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo
                                          + PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E},

    /* PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E */
    {PRV_CPSS_DXCH_TRUNK_MEMBERS_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_TRUNK_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo
                                           + PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E,
    PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo
                                          + PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E},

    /* PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E */
    {PRV_CPSS_DXCH_RATE_LIMIT_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_RATE_LIMIT_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                    PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                   PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E},

    /* PRV_CPSS_DXCH_TABLE_CPU_CODE_E */
    {PRV_CPSS_DXCH_CPU_CODE_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_CPU_CODE_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                  PRV_CPSS_DXCH_TABLE_CPU_CODE_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                 PRV_CPSS_DXCH_TABLE_CPU_CODE_E},

    /* PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E */
    {PRV_CPSS_DXCH2_PCL_CONFIG_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_PCL_CONFIG_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                               PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E},

    /* PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E */
    {PRV_CPSS_DXCH2_QOS_PROFILE_NUM_MAX_CNS, PRV_CPSS_DXCH_QOS_PROFILE_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                               PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                              PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E},
    /* PRV_CPSS_DXCH_TABLE_REMARKING_E */
    {PRV_CPSS_DXCH2_QOS_PROFILE_NUM_MAX_CNS, PRV_CPSS_DXCH_QOS_PROFILE_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                 PRV_CPSS_DXCH_TABLE_REMARKING_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                PRV_CPSS_DXCH_TABLE_REMARKING_E},
    /* PRV_CPSS_DXCH_TABLE_STG_E */
    {PRV_CPSS_DXCH_STG_MAX_NUM_CNS, PRV_CPSS_DXCH_STG_ENTRY_WORDS_SIZE_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                      PRV_CPSS_DXCH_TABLE_STG_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                      PRV_CPSS_DXCH_TABLE_STG_E},

    /* PRV_CPSS_DXCH_TABLE_VLAN_E */
    {PRV_CPSS_DXCH_MAX_NUM_VLANS_CNS,PRV_CPSS_DXCH2_VLAN_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                     PRV_CPSS_DXCH_TABLE_VLAN_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                    PRV_CPSS_DXCH2_TABLE_VLAN_E},

    /* PRV_CPSS_DXCH_TABLE_MULTICAST_E */
    {PRV_CPSS_DXCH_MAX_NUM_VLANS_CNS, PRV_CPSS_DXCH_MULTICAST_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                PRV_CPSS_DXCH_TABLE_MULTICAST_E,
    PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                PRV_CPSS_DXCH_TABLE_MULTICAST_E},

    /* PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E */
    {PRV_CPSS_DXCH_MAX_VLANS_NUM_CNS, PRV_CPSS_DXCH_ROUTE_HA_MAC_SA_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                           PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                          PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E},

    /* PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E */
    {PRV_CPSS_DXCH_ROUTE_HA_ARP_DA_TABLE_MAX_ENTRIES_CNS,
        PRV_CPSS_DXCH2_ROUTE_HA_ARP_DA_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                           PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                          PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E},

    /* PRV_CPSS_DXCH_TABLE_FDB_E */
    {PRV_CPSS_DXCH_FDB_MAX_ENTRIES_CNS , PRV_CPSS_DXCH_FDB_MAC_ENTRY_WORDS_SIZE_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                       PRV_CPSS_DXCH_TABLE_FDB_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                      PRV_CPSS_DXCH_TABLE_FDB_E},

    /* PRV_CPSS_DXCH_TABLE_POLICER_E */
    {PRV_CPSS_DXCH_POLICER_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_POLICER_ENTRY_WORDS_SIZE_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                   PRV_CPSS_DXCH_TABLE_POLICER_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                  PRV_CPSS_DXCH_TABLE_POLICER_E},

    /* PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E */
    {PRV_CPSS_DXCH_POLICER_COUNTERS_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_POLICER_COUNTERS_ENTRY_WORDS_SIZE_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                          PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                         PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E},

    /* PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E */
    {PRV_CPSS_DXCH2_EGRESS_PCL_CONFIG_TABLE_MAX_ENTRIES_CNS,
                                            PRV_CPSS_DXCH_PCL_CONFIG_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                        PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                       PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E},
    /* PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E */
    {PRV_CPSS_DXCH_ROUTE_HA_ARP_DA_TABLE_MAX_ENTRIES_CNS,
                                      PRV_CPSS_DXCH2_ROUTE_HA_ARP_DA_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                      PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                     PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E},

    /* PRV_CPSS_DXCH2_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E */
    {PRV_CPSS_DXCH2_QOS_TO_ROUTE_BLOCK_TABLE_MAX_ENTRIES_CNS,
                                         PRV_CPSS_DXCH2_QOS_TO_ROUTE_BLOCK_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                               PRV_CPSS_DXCH2_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                              PRV_CPSS_DXCH2_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E},

    /* PRV_CPSS_DXCH2_TABLE_ROUTE_ACCESS_MATRIX_E */
    {PRV_CPSS_DXCH2_ROUTE_ACCESS_MATRIX_TABLE_MAX_ENTRIES_CNS,
                                        PRV_CPSS_DXCH2_ROUTE_ACCESS_MATRIX_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                      PRV_CPSS_DXCH2_TABLE_ROUTE_ACCESS_MATRIX_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                     PRV_CPSS_DXCH2_TABLE_ROUTE_ACCESS_MATRIX_E},

    /* PRV_CPSS_DXCH2_LTT_TT_ACTION_E */
    {PRV_CPSS_DXCH2_LTT_TT_ACTION_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH2_LTT_TT_ACTION_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                  PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                 PRV_CPSS_DXCH2_LTT_TT_ACTION_E},

    /* PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E */
    {PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_TABLE_MAX_ENTRIES_CNS,
                                       PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                           PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                          PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E},



     /* PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_E */
    {PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_TABLE_MAX_ENTRIES_CNS,
                                    PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                             PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                             PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_E}
};


static const GT_U32 cheetah2TableInfoSize =
                       sizeof(cheetah2TablesInfo)/sizeof(cheetah2TablesInfo[0]);

/* Cheetah3's table information */
/* All CH3 tables beside FDB and VLAN has direct access by PSS default.
   FDB access is indirect to save entries consistency in the read or
   write under traffic.
   VLAN write access is indirect to save entries consistency in the write under
   traffic only. There is no inconsistency in the read VLAN because this
   table may be changed only by CPU.

*/

static PRV_CPSS_DXCH_TABLES_INFO_STC cheetah3TablesInfo[] =
{
    /* PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E */
    {PRV_CPSS_DXCH_PORT_TBLS_MAX_INDEX_CNS , PRV_CPSS_DXCH_PRTCL_BASED_VLAN_ENTRY_WORDS_COUNT_CNS ,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                        PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E,
    PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                       PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E},

    /* PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E */
    {PRV_CPSS_DXCH_PORT_TBLS_MAX_INDEX_CNS , PRV_CPSS_DXCH_VLAN_PORT_QOS_ENTRY_WORDS_COUNT_CNS ,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo
                                           + PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
    PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo
                                          + PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E},

    /* PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E */
    {PRV_CPSS_DXCH_TRUNK_MEMBERS_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_TRUNK_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo
                                           + PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E,
    PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo
                                          + PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E},

    /* PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E */
    {PRV_CPSS_DXCH_RATE_LIMIT_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_RATE_LIMIT_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                    PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                   PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E},

    /* PRV_CPSS_DXCH_TABLE_CPU_CODE_E */
    {PRV_CPSS_DXCH_CPU_CODE_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_CPU_CODE_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                  PRV_CPSS_DXCH_TABLE_CPU_CODE_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                 PRV_CPSS_DXCH_TABLE_CPU_CODE_E},

    /* PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E */
    {PRV_CPSS_DXCH2_PCL_CONFIG_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH3_PCL_CONFIG_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, &dxCh3IpclCfgTblInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, &dxCh3IpclCfgTblInfo},

    /* PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E */
    {PRV_CPSS_DXCH2_QOS_PROFILE_NUM_MAX_CNS, PRV_CPSS_DXCH_QOS_PROFILE_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                               PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                              PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E},
    /* PRV_CPSS_DXCH_TABLE_REMARKING_E */
    {PRV_CPSS_DXCH2_QOS_PROFILE_NUM_MAX_CNS, PRV_CPSS_DXCH_QOS_PROFILE_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                 PRV_CPSS_DXCH_TABLE_REMARKING_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                PRV_CPSS_DXCH_TABLE_REMARKING_E},
    /* PRV_CPSS_DXCH_TABLE_STG_E */
    {PRV_CPSS_DXCH_STG_MAX_NUM_CNS, PRV_CPSS_DXCH_STG_ENTRY_WORDS_SIZE_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                      PRV_CPSS_DXCH_TABLE_STG_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                      PRV_CPSS_DXCH_TABLE_STG_E},

    /* PRV_CPSS_DXCH_TABLE_VLAN_E */
    {PRV_CPSS_DXCH_MAX_NUM_VLANS_CNS,PRV_CPSS_DXCH2_VLAN_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                     PRV_CPSS_DXCH_TABLE_VLAN_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                    PRV_CPSS_DXCH2_TABLE_VLAN_E},

    /* PRV_CPSS_DXCH_TABLE_MULTICAST_E */
    {PRV_CPSS_DXCH_MAX_NUM_VLANS_CNS, PRV_CPSS_DXCH_MULTICAST_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                PRV_CPSS_DXCH_TABLE_MULTICAST_E,
    PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                PRV_CPSS_DXCH_TABLE_MULTICAST_E},

    /* PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E */
    {PRV_CPSS_DXCH_MAX_VLANS_NUM_CNS, PRV_CPSS_DXCH_ROUTE_HA_MAC_SA_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                           PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                          PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E},

    /* PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E */
    {PRV_CPSS_DXCH_ROUTE_HA_ARP_DA_TABLE_MAX_ENTRIES_CNS,
        PRV_CPSS_DXCH2_ROUTE_HA_ARP_DA_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                           PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                          PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E},

    /* PRV_CPSS_DXCH_TABLE_FDB_E */
    {PRV_CPSS_DXCH3_FDB_MAX_ENTRIES_CNS , PRV_CPSS_DXCH_FDB_MAC_ENTRY_WORDS_SIZE_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                       PRV_CPSS_DXCH_TABLE_FDB_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                      PRV_CPSS_DXCH_TABLE_FDB_E},

    /* PRV_CPSS_DXCH_TABLE_POLICER_E - not supported for CH3.
        The dedicated algorithm is used for the table */
    {0, 0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                   PRV_CPSS_DXCH_TABLE_POLICER_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                  PRV_CPSS_DXCH_TABLE_POLICER_E},

    /* PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E  - not supported for CH3.
        The dedicated algorithm is used for the table */
    {0, 0,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, directAccessTableInfo +
                                          PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, directAccessTableInfo +
                                         PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E},

    /* PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E */
    {PRV_CPSS_DXCH2_EGRESS_PCL_CONFIG_TABLE_MAX_ENTRIES_CNS,
                                            PRV_CPSS_DXCH_PCL_CONFIG_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, &dxCh3EpclCfgTblInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, &dxCh3EpclCfgTblInfo},

    /* PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E */
    {PRV_CPSS_DXCH_ROUTE_HA_ARP_DA_TABLE_MAX_ENTRIES_CNS,
                                      PRV_CPSS_DXCH2_ROUTE_HA_ARP_DA_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                      PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                     PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E},

    /* PRV_CPSS_DXCH2_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E */
    {PRV_CPSS_DXCH2_QOS_TO_ROUTE_BLOCK_TABLE_MAX_ENTRIES_CNS,
                                         PRV_CPSS_DXCH2_QOS_TO_ROUTE_BLOCK_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                               PRV_CPSS_DXCH2_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                              PRV_CPSS_DXCH2_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E},

    /* PRV_CPSS_DXCH2_TABLE_ROUTE_ACCESS_MATRIX_E */
    {PRV_CPSS_DXCH2_ROUTE_ACCESS_MATRIX_TABLE_MAX_ENTRIES_CNS,
                                        PRV_CPSS_DXCH2_ROUTE_ACCESS_MATRIX_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                      PRV_CPSS_DXCH2_TABLE_ROUTE_ACCESS_MATRIX_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                     PRV_CPSS_DXCH2_TABLE_ROUTE_ACCESS_MATRIX_E},

    /* PRV_CPSS_DXCH2_LTT_TT_ACTION_E */
    {PRV_CPSS_DXCH2_LTT_TT_ACTION_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH2_LTT_TT_ACTION_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                  PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                 PRV_CPSS_DXCH2_LTT_TT_ACTION_E},

    /* PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E */
    {PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_TABLE_MAX_ENTRIES_CNS,
                                       PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                           PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                          PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E},

     /* PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_E */
    {PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_TABLE_MAX_ENTRIES_CNS,
                                    PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                             PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                             PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_E},

    /*PRV_CPSS_DXCH3_TABLE_MAC2ME_E */
    {PRV_CPSS_DXCH3_MAC2ME_TABLE_MAX_ENTRIES_CNS,
                                    PRV_CPSS_DXCH3_MAC2ME_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                             PRV_CPSS_DXCH3_TABLE_MAC2ME_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                             PRV_CPSS_DXCH3_TABLE_MAC2ME_E},

     /* PRV_CPSS_DXCH3_TABLE_INGRESS_VLAN_TRANSLATION_E */
    {PRV_CPSS_DXCH3_VLAN_TRANSLATION_TABLE_MAX_ENTRIES_CNS,
                                    PRV_CPSS_DXCH3_VLAN_TRANSLATION_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                             PRV_CPSS_DXCH3_TABLE_INGRESS_VLAN_TRANSLATION_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                             PRV_CPSS_DXCH3_TABLE_INGRESS_VLAN_TRANSLATION_E},

     /* PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E */
    {PRV_CPSS_DXCH3_VLAN_TRANSLATION_TABLE_MAX_ENTRIES_CNS,
                                    PRV_CPSS_DXCH3_VLAN_TRANSLATION_WORDS_COUNT_CNS,\
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                             PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                            PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E},

    /* PRV_CPSS_DXCH3_TABLE_VRF_ID_E */
    {PRV_CPSS_DXCH3_VRF_ID_TABLE_MAX_ENTRIES_CNS,
                                    PRV_CPSS_DXCH3_VRF_ID_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                             PRV_CPSS_DXCH3_TABLE_VRF_ID_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                            PRV_CPSS_DXCH3_TABLE_VRF_ID_E},

    /* PRV_CPSS_DXCH3_LTT_TT_ACTION_E */
    {PRV_CPSS_DXCH3_LTT_TT_ACTION_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH3_LTT_TT_ACTION_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                  PRV_CPSS_DXCH3_LTT_TT_ACTION_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                 PRV_CPSS_DXCH3_LTT_TT_ACTION_E}

};

#define  CHEETAH3_TABLE_INFO_SIZE_CNS (sizeof(cheetah3TablesInfo)/sizeof(cheetah3TablesInfo[0]))

static const GT_U32 cheetah3TableInfoSize = CHEETAH3_TABLE_INFO_SIZE_CNS;


/* xCat's table information */
/* All xCat tables beside FDB and VLAN has direct access by CPSS default.
   FDB access is indirect to save entries consistency in the read or
   write under traffic.
   VLAN write access is indirect to save entries consistency in the write under
   traffic only. There is no inconsistency in the read VLAN because this
   table may be changed only by CPU.
*/

#define XCAT_TABLES_NUM_CNS  CHEETAH3_TABLE_INFO_SIZE_CNS

/* xCatA1 table is greater the the CH3's table */
static PRV_CPSS_DXCH_TABLES_INFO_STC dxChXcatTablesInfo[PRV_CPSS_DXCH_XCAT_TABLE_LAST_E] =
{
    {0}
    /* in runtime the tables of the ch3 will be copied here ,
       and will be updated by the relevant xCat Rev1 code -->
       see dxChXcatInitTable() */

    /* in runtime the 'Extra tables' of the xCat will be copied here*/


};

static const GT_U32 dxChXcatTableInfoSize = PRV_CPSS_DXCH_XCAT_TABLE_LAST_E;

/* xCat2 table is greater the the XCAT's table */
static PRV_CPSS_DXCH_TABLES_INFO_STC dxChXcat2TablesInfo[PRV_CPSS_DXCH_XCAT2_TABLE_LAST_E] =
{
    {0}
    /* in runtime the tables of the xCat Rev1 will be copied here ,
       and will be updated by the relevant xCat2 code -->
       see dxChXcat2InitTable() */

    /* in runtime the 'Extra tables' of the xCat will be copied here*/


};

static const GT_U32 dxChXcat2TableInfoSize = PRV_CPSS_DXCH_XCAT2_TABLE_LAST_E;

static void dxChXcatInitTable(void);
static void dxChXcat2InitTable(void);
/* Lion */
#define LION_TABLES_NUM_CNS  XCAT_TABLES_NUM_CNS
/* lion  B table*/
static PRV_CPSS_DXCH_TABLES_INFO_STC lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_LAST_E] =
{
    {0}
    /* in runtime the tables of the xcat will be copied here ,
       and will be updated by the relevant lion B code -->
       see lionInitTable() */

    /* in runtime the 'Extra tables' of the lion will be copied here*/


};

/* lion B */
static const GT_U32 lionBTableInfoSize = PRV_CPSS_DXCH_LION_TABLE_LAST_E;

static void lionInitTable(void);


static void indexAsPortNumConvert
(
    IN GT_U8                   devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN GT_U32                  entryIndex,
    OUT GT_U32                 *portGroupIdPtr,
    OUT GT_U32                 *updatedEntryIndexPtr
);


/*******************************************************************************
* checkStatusOfPreviousAccess
*
* DESCRIPTION:
*       Check status of previous access.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id
*       tableInfoPtr - pointer to table
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*       GT_FAIL      - on failure.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS checkStatusOfPreviousAccess
(
    IN GT_U8                                 devNum,
    IN GT_U32                                portGroupId,
    IN PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC *tableInfoPtr
)
{
    return prvCpssDxChPortGroupBusyWait(devNum,portGroupId,
                tableInfoPtr->controlReg,
                tableInfoPtr->trigBit,
                GT_FALSE);
}


/*******************************************************************************
* setReadWriteCommandToControlReg
*
* DESCRIPTION:
*       Set Read / Write command to Control register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId  - the port group Id
*       tableInfoPtr - pointer to table
*       entryIndex   - index in the table
*       accessAction - access action to table: PRV_CPSS_DXCH_ACCESS_ACTION_READ_E,
*                                              PRV_CPSS_DXCH_ACCESS_ACTION_WRITE_E
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*       GT_FAIL      - on failure.
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS setReadWriteCommandToControlReg
(
    IN GT_U8                                    devNum,
    IN GT_U32                                   portGroupId,
    IN PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC   *tableInfoPtr,
    IN GT_U32                                   entryIndex,
    IN PRV_CPSS_DXCH_ACCESS_ACTION_ENT          accessAction
)
{
   GT_U32 controlValue;

   controlValue  = (entryIndex << tableInfoPtr->indexBit);

   /* trigger the action */
   controlValue |= (1 << tableInfoPtr->trigBit);

   /* add the specific table when this control register has more then one table
   if there is no specific table, specificTableValue = 0*/
   controlValue |= (tableInfoPtr->specificTableValue <<
                     tableInfoPtr->specificTableBit);

   /* set type of action - R/W */
   controlValue |= (accessAction << tableInfoPtr->actionBit);

   return prvCpssDxChHwPpPortGroupWriteRegister(devNum,portGroupId, tableInfoPtr->controlReg, controlValue);


}

/*******************************************************************************
* indirectReadTableEntry
*
* DESCRIPTION:
*       Read a whole entry from table by indirect access method.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId  - the portGroupId
*       tableType - the specific table name
*       tableInfoPtr - pointer to table information
*       entryIndex   - index in the table
*       tablePtr     - pointer to indirect information
*
* OUTPUTS:
*       entryValuePtr - pointer to the data read from the table
*
*
* RETURNS:
*       GT_OK        - on success.
*       GT_FAIL      - on failure.
*       GT_OUT_OF_RANGE - parameter not in valid range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS indirectReadTableEntry
(
    IN GT_U8                                    devNum,
    IN GT_U32                                   portGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT                  tableType,
    IN PRV_CPSS_DXCH_TABLES_INFO_STC            *tableInfoPtr,
    IN GT_U32                                   entryIndex,
    IN PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC   *tablePtr,
    OUT GT_U32                                  *entryValuePtr
)
{
    GT_STATUS   rc;
    GT_U32      wordNum; /* iterator */

    /* busy wait for the control reg to show "read was done" */
    rc = checkStatusOfPreviousAccess(devNum, portGroupId,tablePtr);
    if(rc != GT_OK)
        return rc;
    /* set Control register for Read action */
    rc = setReadWriteCommandToControlReg(devNum, portGroupId, tablePtr, entryIndex,
                                          PRV_CPSS_DXCH_ACCESS_ACTION_READ_E);
    if(rc != GT_OK)
        return rc;

    /* busy wait for the control reg to show "read was done" */
    rc = checkStatusOfPreviousAccess(devNum, portGroupId,tablePtr);
    if(rc != GT_OK)
        return rc;

    /* table entry read - read to the buffer  - entryValuePtr != NULL */
    /* field table read, field table write - entryValuePtr == NULL  */
    if( entryValuePtr != NULL)
    {
        /* VLAN and STG indirect access tables need word swapping */
        if((tableType == PRV_CPSS_DXCH_TABLE_VLAN_E) ||
           (tableType == PRV_CPSS_DXCH_TABLE_STG_E))
        {
            for(wordNum = 0; wordNum < tableInfoPtr->entrySize; wordNum++)
            {
                /* read data register and perform swapping */
                rc = prvCpssDxChHwPpPortGroupReadRegister(devNum, portGroupId,tablePtr->dataReg + wordNum * 4,
                                           entryValuePtr +
                                      (tableInfoPtr->entrySize - wordNum - 1));
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }
        else
        {
            /* read all data registers */
            rc =  prvCpssDxChHwPpPortGroupReadRam(devNum,portGroupId,
                            tablePtr->dataReg,
                            tableInfoPtr->entrySize,/* num of words */
                            entryValuePtr);
        }

    }

    return rc;
}

/*******************************************************************************
* indirectWriteTableEntry
*
* DESCRIPTION:
*       Write a whole entry to table by indirect access method.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id
*       tableType - the specific table name
*       tableInfoPtr - pointer to table information
*       entryIndex   - index in the table
*       entryValuePtr - pointer to the data read from the table
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*       GT_FAIL      - on failure.
*       GT_OUT_OF_RANGE - parameter not in valid range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS indirectWriteTableEntry
(
    IN GT_U8                            devNum,
    IN GT_U32                           portGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT          tableType,
    IN PRV_CPSS_DXCH_TABLES_INFO_STC    *tableInfoPtr,
    IN GT_U32                           entryIndex,
    IN GT_U32                           *entryValuePtr
)
{
    GT_U32      wordNum; /* iterator */
    GT_STATUS   rc;
    /* pointer to indirect table info */
    PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC *tablePtr;
    GT_U32      address;  /* address to write the information */

    tablePtr =
         (PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC*)(tableInfoPtr->writeTablePtr);

    /* busy wait for the control reg to show "read was done" */
    rc = checkStatusOfPreviousAccess(devNum, portGroupId,tablePtr);
    if(rc != GT_OK)
        return rc;

    /* entryValuePtr == NULL for Cheetah VB init of VLAN table */
    if(entryValuePtr != NULL)
    {
        /* VLAN and STG indirect access tables need word swapping */
        if((tableType == PRV_CPSS_DXCH_TABLE_VLAN_E) ||
           (tableType == PRV_CPSS_DXCH_TABLE_STG_E))
        {
            for(wordNum = 0; wordNum < tableInfoPtr->entrySize; wordNum++)
            {
                address = tablePtr->dataReg + wordNum * 4;
                /* write to data register and perform swapping */
                rc = prvCpssDxChHwPpPortGroupWriteRegister(devNum, portGroupId,address,
                           entryValuePtr[tableInfoPtr->entrySize - wordNum - 1]);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }

        }
        else
        {
           /* non-direct table --- write to it as continues memory */
            rc = prvCpssDxChHwPpPortGroupWriteRam(devNum,portGroupId,
                             tablePtr->dataReg,
                             tableInfoPtr->entrySize,
                             entryValuePtr);

            if (rc != GT_OK)
            {
                return rc;
            }

        }

    }

    /* set Control register for Write action */
    return setReadWriteCommandToControlReg(devNum, portGroupId, tablePtr, entryIndex,
                                            PRV_CPSS_DXCH_ACCESS_ACTION_WRITE_E);

}

/*******************************************************************************
* indirectReadTableEntryField
*
* DESCRIPTION:
*       Read a field of entry from table by indirect access method.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id
*       tableType - the specific table name
*       tableInfoPtr - pointer to table information
*       entryIndex   - index in the table
*       fieldWordNum - field word number
*       fieldOffset  - field offset
*       fieldLength - field length
*
* OUTPUTS:
*       fieldValuePtr - pointer to the data read from the table
*
* RETURNS:
*       GT_OK        - on success.
*       GT_FAIL      - on failure.
*       GT_OUT_OF_RANGE - parameter not in valid range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*
*******************************************************************************/

static GT_STATUS indirectReadTableEntryField
(
    IN GT_U8                        devNum,
    IN GT_U32                       portGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT       tableType,
    IN PRV_CPSS_DXCH_TABLES_INFO_STC *tableInfoPtr,
    IN GT_U32                       entryIndex,
    IN GT_U32                       fieldWordNum,
    IN GT_U32                       fieldOffset,
    IN GT_U32                       fieldLength,
    OUT GT_U32                      *fieldValuePtr
)
{
    GT_STATUS   rc;
    GT_U32 address; /* address to read from */
    /* pointer to indirect table info */
    PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC *tablePtr;

    tablePtr = (PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC*)(tableInfoPtr->readTablePtr);

    /* prepare to read action */
    rc = indirectReadTableEntry(devNum,portGroupId,tableType,tableInfoPtr,
                                entryIndex, tablePtr, NULL);
    if(rc != GT_OK)
        return GT_FAIL;

    /* VLAN and STG indirect access tables need word swapping */
    if((tableType == PRV_CPSS_DXCH_TABLE_VLAN_E) ||
       (tableType == PRV_CPSS_DXCH_TABLE_STG_E))
    {
        fieldWordNum = tableInfoPtr->entrySize - fieldWordNum - 1;
    }

    address = tablePtr->dataReg + fieldWordNum * 0x4;

    /* read the field from the data register */
    if(prvCpssDxChHwPpPortGroupGetRegField(devNum, portGroupId,address, fieldOffset, fieldLength, fieldValuePtr))
    {
        return GT_HW_ERROR;
    }

    return GT_OK;

}


/*******************************************************************************
* indirectWriteTableEntryField
*
* DESCRIPTION:
*       Read a field from the indirect access table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id
*       tableType - the specific table name
*       tableInfoPtr - pointer to table information
*       entryIndex   - index in the table
*       fieldWordNum - field word number
*       fieldOffset  - field offset
*       fieldLength - field length
*       fieldValue  - value that will be written to the table
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*       GT_FAIL      - on failure.
*       GT_OUT_OF_RANGE - parameter not in valid range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*
*******************************************************************************/

static GT_STATUS indirectWriteTableEntryField
(
    IN GT_U8                          devNum,
    IN GT_U32                         portGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT         tableType,
    IN PRV_CPSS_DXCH_TABLES_INFO_STC  *tableInfoPtr,
    IN GT_U32                         entryIndex,
    IN GT_U32                         fieldWordNum,
    IN GT_U32                         fieldOffset,
    IN GT_U32                         fieldLength,
    IN GT_U32                         fieldValue
)
{
    GT_STATUS   rc;
    GT_U32 address; /* address to read from */
    /* pointer to indirect table info */
    PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC *tablePtr;

    tablePtr =
         (PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC*)(tableInfoPtr->writeTablePtr);

    /* read the entry -- put entry data to Data register */
    rc = indirectReadTableEntry(devNum,portGroupId , tableType, tableInfoPtr,
                                 entryIndex, tablePtr, NULL);
    if(rc != GT_OK)
        return GT_FAIL;

    /* VLAN and STG indirect access tables need word swapping */
    if(((GT_U32)tableType == PRV_CPSS_DXCH_TABLE_VLAN_E) ||
       (tableType == PRV_CPSS_DXCH_TABLE_STG_E))
    {
        fieldWordNum = tableInfoPtr->entrySize - fieldWordNum - 1;
    }

    address = tablePtr->dataReg + fieldWordNum * 0x4;

    /* non-direct table write */
    rc =  prvCpssDxChHwPpPortGroupSetRegField(devNum, portGroupId,address,fieldOffset,fieldLength,fieldValue);
    if(rc != GT_OK)
        return GT_FAIL;

    /* set Control register for Write action */
    return setReadWriteCommandToControlReg(devNum, portGroupId ,tablePtr, entryIndex,
                                            PRV_CPSS_DXCH_ACCESS_ACTION_WRITE_E);

}


/*******************************************************************************
* prvCpssDxChReadTableEntry
*
* DESCRIPTION:
*       Read a whole entry from the table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       tableType - the specific table name
*       entryIndex   - index in the table
*
* OUTPUTS:
*       entryValuePtr - (pointer to) the data read from the table
*                       may be NULL in the case of indirect table.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*      In the case of entryValuePtr == NULL and indirect table the function
*      just send read entry command to device. And entry is stored in the
*      data registers of a indirect table
*******************************************************************************/
GT_STATUS prvCpssDxChReadTableEntry

(
    IN GT_U8                   devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN GT_U32                  entryIndex,
    OUT GT_U32                 *entryValuePtr
)
{
    GT_U32      portGroupId;/* support multi-port-groups device for table accesses with index = 'per port' */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* convert if needed the entry index , get specific portGroupId (or all port groups) */
    indexAsPortNumConvert(devNum,tableType,entryIndex, &portGroupId , &entryIndex);

    return prvCpssDxChPortGroupReadTableEntry(devNum,portGroupId,tableType,entryIndex,entryValuePtr);

}

/*******************************************************************************
* prvCpssDxChWriteTableEntry
*
* DESCRIPTION:
*       Write a whole entry to the table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       tableType - the specific table name
*       entryIndex   - index in the table
*       entryValuePtr - (pointer to) the data that will be written to the table
*                       may be NULL in the case of indirect table.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*      In the case of entryValuePtr == NULL and indirect table the function
*      just send write entry command to device. And entry is taken from the
*      data registers of a indirect table.
*******************************************************************************/
GT_STATUS prvCpssDxChWriteTableEntry
(
    IN GT_U8                   devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN GT_U32                  entryIndex,
    IN GT_U32                  *entryValuePtr
)
{
    GT_U32      portGroupId;/* support multi-port-groups device for table accesses with index = 'per port' */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* convert if needed the entry index , get specific portGroupId (or all port groups) */
    indexAsPortNumConvert(devNum,tableType,entryIndex, &portGroupId , &entryIndex);

    return prvCpssDxChPortGroupWriteTableEntry(devNum,portGroupId,tableType,entryIndex,entryValuePtr);
}

/*******************************************************************************
* prvCpssDxChReadTableEntryField
*
* DESCRIPTION:
*       Read a field from the table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       tableType - the specific table name
*       entryIndex   - index in the table
*       fieldWordNum - field word number
*                   use PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS
*                   if need global offset in the field of fieldOffset
*       fieldOffset  - field offset
*       fieldLength - field length
*
* OUTPUTS:
*       fieldValuePtr - (pointer to) the data read from the table
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssDxChReadTableEntryField
(
    IN GT_U8                  devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN GT_U32                 entryIndex,
    IN GT_U32                 fieldWordNum,
    IN GT_U32                 fieldOffset,
    IN GT_U32                 fieldLength,
    OUT GT_U32                *fieldValuePtr
)
{
    GT_U32      portGroupId;/* support multi-port-groups device for table accesses with index = 'per port' */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* convert if needed the entry index , get specific portGroupId (or all port groups) */
    indexAsPortNumConvert(devNum,tableType,entryIndex, &portGroupId , &entryIndex);

    return prvCpssDxChPortGroupReadTableEntryField(devNum,portGroupId ,
                tableType,entryIndex,fieldWordNum,
                fieldOffset,fieldLength,fieldValuePtr);
}

/*******************************************************************************
* prvCpssDxChWriteTableEntryField
*
* DESCRIPTION:
*       Write a field to the table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       tableType - the specific table name
*       entryIndex   - index in the table
*       fieldWordNum - field word number
*                   use PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS
*                   if need global offset in the field of fieldOffset
*       fieldOffset  - field offset
*       fieldLength - field length
*       fieldValue - the data write to the table
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssDxChWriteTableEntryField
(
    IN GT_U8                  devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN GT_U32                 entryIndex,
    IN GT_U32                 fieldWordNum,
    IN GT_U32                 fieldOffset,
    IN GT_U32                 fieldLength,
    IN GT_U32                 fieldValue
)
{
    GT_U32      portGroupId;/* support multi-port-groups device for table accesses with index = 'per port' */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* convert if needed the entry index , get specific portGroupId (or all port groups) */
    indexAsPortNumConvert(devNum,tableType,entryIndex, &portGroupId , &entryIndex);

    return prvCpssDxChPortGroupWriteTableEntryField(devNum,portGroupId ,
                tableType,entryIndex,fieldWordNum,
                fieldOffset,fieldLength,fieldValue);

}

/*******************************************************************************
* prvCpssDxChTableNumEntriesGet
*
* DESCRIPTION:
*       get the number of entries in a table
*       needed for debug purpose
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       tableType - the specific table name
*
* OUTPUTS:
*       numEntriesPtr - (pointer to) number of entries in the table
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChTableNumEntriesGet
(
    IN GT_U8                    devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT  tableType,
    OUT GT_U32                  *numEntriesPtr
)
{
    PRV_CPSS_DXCH_TABLES_INFO_STC *tableInfoPtr; /* pointer to table info */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* validity check */
    if((GT_U32)tableType >= PRV_CPSS_DXCH_TABLE_SIZE_GET_MAC(devNum))
       return GT_OUT_OF_RANGE;

    if(numEntriesPtr == NULL)
    {
        return GT_BAD_PARAM;
    }

    tableInfoPtr = PRV_TABLE_INFO_PTR_GET_MAC(devNum,tableType);

    *numEntriesPtr = tableInfoPtr->maxNumOfEntries;

    return GT_OK;
}



/*******************************************************************************
* prvCpssDxChTablesAccessInit
*
* DESCRIPTION:
*       Initializes all structures for table access.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS  prvCpssDxChTablesAccessInit
(
    IN GT_U8    devNum
)
{
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;
    PRV_CPSS_DXCH_TABLES_INFO_STC      *accessTableInfoPtr;/*pointer to tables info*/
    GT_U32                              accessTableInfoSize;/*tables info size */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
    {
        PRV_CPSS_DXCH_PP_MAC(devNum)->accessTableInfoPtr = cheetahTablesInfo;
        PRV_CPSS_DXCH_PP_MAC(devNum)->accessTableInfoSize = cheetahTableInfoSize;

        /* update tables size according to "fine tuning" */
        cheetahTablesInfo[PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E].maxNumOfEntries =
            fineTuningPtr->tableSize.routerArp;

    }
    else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH2_E)
    {
        PRV_CPSS_DXCH_PP_MAC(devNum)->accessTableInfoPtr = cheetah2TablesInfo;
        PRV_CPSS_DXCH_PP_MAC(devNum)->accessTableInfoSize = cheetah2TableInfoSize;

        /* update tables size according to "fine tuning" */
        cheetah2TablesInfo[PRV_CPSS_DXCH2_LTT_TT_ACTION_E].maxNumOfEntries =
            fineTuningPtr->tableSize.routerAndTunnelTermTcam;
        cheetah2TablesInfo[PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E].maxNumOfEntries =
            fineTuningPtr->tableSize.routerNextHop;
        cheetah2TablesInfo[PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E].maxNumOfEntries =
            fineTuningPtr->tableSize.tunnelStart;
    }
    else /* Cheetah3 */
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
        {
            accessTableInfoPtr = cheetah3TablesInfo;
            accessTableInfoSize = cheetah3TableInfoSize;
        }
        else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
        {
            accessTableInfoPtr = dxChXcatTablesInfo;
            accessTableInfoSize = dxChXcatTableInfoSize;
            dxChXcatInitTable();
        }
        else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
        {
            accessTableInfoPtr = dxChXcat2TablesInfo;
            accessTableInfoSize = dxChXcat2TableInfoSize;
            dxChXcat2InitTable();
        }
        else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            accessTableInfoPtr = lionBTablesInfo;
            accessTableInfoSize = lionBTableInfoSize;
            lionInitTable();
        }
        else
        {
            /* not supported/implemented yet */
            return GT_NOT_IMPLEMENTED;
        }

        PRV_CPSS_DXCH_PP_MAC(devNum)->accessTableInfoPtr = accessTableInfoPtr;
        PRV_CPSS_DXCH_PP_MAC(devNum)->accessTableInfoSize = accessTableInfoSize;

        /* update tables size according to "fine tuning" */
        accessTableInfoPtr[PRV_CPSS_DXCH3_LTT_TT_ACTION_E].maxNumOfEntries =
            fineTuningPtr->tableSize.routerAndTunnelTermTcam;
        accessTableInfoPtr[PRV_CPSS_DXCH2_LTT_TT_ACTION_E].maxNumOfEntries =
            fineTuningPtr->tableSize.routerAndTunnelTermTcam;
        accessTableInfoPtr[PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E].maxNumOfEntries =
            fineTuningPtr->tableSize.routerNextHop;
        /* Need to take the bigger value between ARP entries / 4 and TS entries */
        if ((PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.routerArp/4) >
            (PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.tunnelStart))
        {
            accessTableInfoPtr[PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E].maxNumOfEntries =
                PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.routerArp / 4;
        }
        else
        {
            accessTableInfoPtr[PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E].maxNumOfEntries =
                PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.tunnelStart;
        }
        accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E].maxNumOfEntries = 4160;

        /* Due to Cheetah 3 Errata - " FEr#2028: Direct access under traffic to Pre-Egress
         Engine (EQ) tables is not functional"
        -- see PRV_CPSS_DXCH3_DIRECT_ACCESS_TO_EQ_ADDRESS_SPACE_WA_E */
        if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH3_DIRECT_ACCESS_TO_EQ_ADDRESS_SPACE_WA_E))
        {
            /* PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E */
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E].readAccessType =
                PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E].readTablePtr =
                indirectAccessTableInfo + PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E;
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E].writeAccessType =
                PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E].writeTablePtr =
                indirectAccessTableInfo + PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E;

            /* PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E */
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E].readAccessType =
                PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E].readTablePtr =
                indirectAccessTableInfo + PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E;
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E].writeAccessType =
                PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E].writeTablePtr =
                indirectAccessTableInfo + PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E;

            /* PRV_CPSS_DXCH_TABLE_CPU_CODE_E */
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_CPU_CODE_E].readAccessType =
                PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_CPU_CODE_E].readTablePtr =
                indirectAccessTableInfo + PRV_CPSS_DXCH_TABLE_CPU_CODE_E;
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_CPU_CODE_E].writeAccessType =
                PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_CPU_CODE_E].writeTablePtr =
                indirectAccessTableInfo + PRV_CPSS_DXCH_TABLE_CPU_CODE_E;

            /* PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E */
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E].readAccessType =
                PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E].readTablePtr =
                indirectAccessTableInfo + PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E;
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E].writeAccessType =
                PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E].writeTablePtr =
                indirectAccessTableInfo + PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E;
        }


        /* FEr#2006 */
        if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH3_VLT_INDIRECT_ACCESS_WA_E))
        {
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_VLAN_E].writeAccessType =
                PRV_CPSS_DXCH_DIRECT_ACCESS_E;
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_VLAN_E].writeTablePtr =
                directAccessTableInfo + PRV_CPSS_DXCH_TABLE_VLAN_E;
        }

    }
    return GT_OK;

}

/*******************************************************************************
* prvCpssDxChPortGroupWriteTableEntryFieldList
*
* DESCRIPTION:
*       Write a list of fields to the table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       portGroupId - the port group Id , to support multi-port-group devices that need to access
*                specific port group
*       tableType         - HW table Id
*       entryIndex        - entry Index
*       entryMemoBufArr   - the work memory for read, update and write the entry
*       fieldsAmount      - amount of updated fields in the entry
*       fieldOffsetArr    - (array) the offset of the field in bits
*                           from the entry origin
*       fieldLengthArr    - (array) the length of the field in bits,
*                           0 - means to skip this subfield
*       fieldValueArr     - (array) the value of the field
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_TIMEOUT         - after max number of retries checking if PP ready
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupWriteTableEntryFieldList
(
    IN GT_U8                                  devNum,
    IN GT_U32                                 portGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT                tableType,
    IN GT_U32                                 entryIndex,
    IN GT_U32                                 entryMemoBufArr[],
    IN GT_U32                                 fieldsAmount,
    IN GT_U32                                 fieldOffsetArr[],
    IN GT_U32                                 fieldLengthArr[],
    IN GT_U32                                 fieldValueArr[]
)
{
    GT_STATUS                  res;           /* return code               */
    GT_U32                     idx;           /* field index               */
    GT_U32                     wordIdx;       /* word index                */
    GT_U32                     wordShift;     /* Shift in the word         */
    GT_U32                     len0;          /* length in the 1-th word   */
    GT_U32                     len1;          /* length in the 2-th word   */

    res = prvCpssDxChPortGroupReadTableEntry(
        devNum, portGroupId,tableType, entryIndex, entryMemoBufArr);
    if (res != GT_OK)
    {
        return res;
    }

    for (idx = 0; (idx < fieldsAmount); idx++)
    {
        if (fieldLengthArr[idx] > 32)
        {
            return GT_BAD_PARAM;
        }

        if (fieldLengthArr[idx] == 0)
            continue;

        wordIdx   = fieldOffsetArr[idx] >> 5;
        wordShift = fieldOffsetArr[idx] & 0x1F;

        /* the length of the field or of it's intersection with the first word */
        len0 = ((wordShift + fieldLengthArr[idx]) <= 32)
            ? fieldLengthArr[idx] : (32 - wordShift);

        /* copy the field or it's intersection with the first word */
        U32_SET_FIELD_MASKED_MAC(
           entryMemoBufArr[wordIdx], wordShift, len0, fieldValueArr[idx]);

        /* copy the field intersection with the second word */
        if (len0 != fieldLengthArr[idx])
        {
            len1 = fieldLengthArr[idx] - len0;
            U32_SET_FIELD_MASKED_MAC(
               entryMemoBufArr[wordIdx + 1] ,0 /*offset*/, len1,
               (fieldValueArr[idx] >> len0));
        }
    }

    return  prvCpssDxChPortGroupWriteTableEntry(
        devNum, portGroupId,tableType, entryIndex, entryMemoBufArr);
}

/*******************************************************************************
* prvCpssDxChReadTableEntryFieldList
*
* DESCRIPTION:
*       Read a list of fields from the table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       portGroupId - the port group Id , to support multi-port-group devices that need to access
*                specific port group
*       tableId           - HW table Id
*       entryIndex        - entry Index
*       entryMemoBufArr   - the work memory for read, update and write the entry
*       fieldsAmount      - amount of updated fields in the entry
*       fieldOffsetArr    - (array) the offset of the field in bits
*                           from the entry origin
*       fieldLengthArr    - (array) the length of the field in bits,
*                           0 - means to skip this subfield

* OUTPUTS:
*       fieldValueArr     - (array) the value of the field
*
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_TIMEOUT         - after max number of retries checking if PP ready
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupReadTableEntryFieldList
(
    IN  GT_U8                                  devNum,
    IN GT_U32                                 portGroupId,
    IN  PRV_CPSS_DXCH_TABLE_ENT                tableId,
    IN  GT_U32                                 entryIndex,
    IN  GT_U32                                 entryMemoBufArr[],
    IN  GT_U32                                 fieldsAmount,
    IN  GT_U32                                 fieldOffsetArr[],
    IN  GT_U32                                 fieldLengthArr[],
    OUT GT_U32                                 fieldValueArr[]
)
{
    GT_STATUS                  res;           /* return code               */
    GT_U32                     idx;           /* field index               */
    GT_U32                     wordIdx;       /* word index                */
    GT_U32                     wordShift;     /* Shift in the word         */
    GT_U32                     len0;          /* length in the 1-th word   */
    GT_U32                     len1;          /* length in the 2-th word   */

    res = prvCpssDxChPortGroupReadTableEntry(
        devNum, portGroupId, tableId, entryIndex, entryMemoBufArr);
    if (res != GT_OK)
    {
        return res;
    }

    for (idx = 0; (idx < fieldsAmount); idx++)
    {
        if (fieldLengthArr[idx] > 32)
        {
            return GT_BAD_PARAM;
        }

        if (fieldLengthArr[idx] == 0)
        {
            fieldValueArr[idx] = 0;
            continue;
        }

        wordIdx   = fieldOffsetArr[idx] >> 5;
        wordShift = fieldOffsetArr[idx] & 0x1F;

        /* the length of the field or of it's intersection with the first word */
        len0 = ((wordShift + fieldLengthArr[idx]) <= 32)
            ? fieldLengthArr[idx] : (32 - wordShift);

        /* copy the field or it's intersection with the first word */
        fieldValueArr[idx] =
            U32_GET_FIELD_MAC(entryMemoBufArr[wordIdx], wordShift, len0);

        /* copy the field intersection with the second word */
        if (len0 != fieldLengthArr[idx])
        {
            len1 = fieldLengthArr[idx] - len0;
            fieldValueArr[idx] |=
                ((U32_GET_FIELD_MAC(
                    entryMemoBufArr[wordIdx + 1], 0/*offset*/, len1))
                 << len0);
        }
    }

    return  GT_OK;
}


#ifdef CHEETAH_TABLE_DEBUG

GT_STATUS readWriteFieldTest
(
    IN GT_U8                  devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType
)
{
    PRV_CPSS_DXCH_TABLES_INFO_STC *tableInfoPtr; /* pointer to table info */
    GT_U32 field;
    GT_U32 readVal;
    GT_U32 wordNum;
    GT_U32 fieldOffset;
    GT_STATUS rc;
    GT_STATUS fieldLength;

    fieldLength = 32;
    tableInfoPtr = &(PRV_CPSS_DXCH_PP_MAC(devNum)->accessTableInfoPtr[tableType]);


    osSrand(5);

    for(wordNum = 0; wordNum < tableInfoPtr->entrySize; wordNum++)
    {
        for(fieldOffset = 0; fieldOffset < 0; fieldOffset+=fieldLength)
        {
            field = osRand();
            rc= coreCheetahWriteTableEntryField(devNum,tableType,1,wordNum,
                                                fieldOffset, fieldLength,field);
            if(rc != GT_OK)
                return GT_FAIL;

        }
    }

    osSrand(5);
    for(wordNum = 0; wordNum < tableInfoPtr->entrySize; wordNum++)
    {
        for(fieldOffset = 0; fieldOffset < 0; fieldOffset+=fieldLength)
        {
            field = osRand();
            rc= coreCheetahReadTableEntryField(devNum,tableType,1,wordNum,
                                            fieldOffset, fieldLength, &readVal);
            if(rc != GT_OK)
                return GT_FAIL;
            if(field != readVal)
            {
                osPrintf("\n wordNum =  %d ",wordNum);
                osPrintf("\n fieldOffset =  %d ",fieldOffset);
                return GT_FAIL;
            }

        }

    }
    osPrintf("\n Field test OK, tableType =  %d ",tableType);


    return GT_OK;
}




GT_U32 getMaskBits
(
    IN PRV_CPSS_DXCH_TABLE_ENT tableType
)
{
    GT_U32     maskBits;


        switch(tableType)
        {
            case PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E:
                maskBits = 0x7ff;
                break;

            case PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E:
                maskBits = 0x7ff;
                break;

            case PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E:
                maskBits = 0x7ff;
                break;

            case PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E:
                maskBits = 0xffffffff;
                break;

            case PRV_CPSS_DXCH_TABLE_CPU_CODE_E:
                maskBits = 0x3fff;
                break;

            case PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E:
                maskBits = 0x1fffffff;
                break;

            case PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E:
                maskBits = 0x3fff;
                break;

            case PRV_CPSS_DXCH_TABLE_REMARKING_E:
                maskBits = 0xff;
                break;

            case PRV_CPSS_DXCH_TABLE_STG_E:
                maskBits = 0x3fffff;
                break;

            case PRV_CPSS_DXCH_TABLE_VLAN_E:
                maskBits = 0xff7d7ffb;
                break;

            case PRV_CPSS_DXCH_TABLE_MULTICAST_E:
                maskBits = 0xfffffff;/* word1 */
                break;

            case PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E:
                maskBits = 0xff;
                break;

            case PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E:
                maskBits = 0xffff; /* word1 */
                break;

            case PRV_CPSS_DXCH_TABLE_FDB_E:
                maskBits = 0x3ff;
                break;

            default:
                maskBits = 0xffffffff;
                break;
        }

        return maskBits;
}



GT_STATUS readWriteEntryTest
(
    IN GT_U8                  devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType
)
{
    PRV_CPSS_DXCH_TABLES_INFO_STC *tableInfoPtr; /* pointer to table info */
    GT_U32 wordValue[CH_TABLE_ENTRY_MAX_WORDS_SIZE_CNS];
    GT_U32 readValue[CH_TABLE_ENTRY_MAX_WORDS_SIZE_CNS];
    GT_U32 wordNum;
    GT_U32 seed;
    GT_U32 entryNum;
    GT_STATUS rc;
#ifdef TBL_DEBUG_SIMULATION
    GT_STATUS i;
    GT_U32     trunkId;
    GT_U32     trunkMemberIdx;
    GT_U32 directReadValue[CH_TABLE_ENTRY_MAX_WORDS_SIZE_CNS];
    GT_U32 regAddress;
#endif /*TBL_DEBUG_SIMULATION*/


    tableInfoPtr = &(PRV_CPSS_DXCH_PP_MAC(devNum)->accessTableInfoPtr[tableType]);

    seed = 5;
    /*tableInfoPtr = PRV_CPSS_DXCH_TABLE_GET(devNum,tableType);*/
    osSrand(seed);


    for(entryNum = 0; entryNum < tableInfoPtr->maxNumOfEntries; entryNum++)
    {
        for(wordNum = 0; wordNum < tableInfoPtr->entrySize; wordNum++)
        {
            wordValue[wordNum] = osRand() & getMaskBits(tableType);
        }

        rc = coreCheetahWriteTableEntry(devNum, tableType, entryNum, wordValue);
        if(rc != GT_OK)
            return rc;

    }

    osSrand(seed);

    for(entryNum = 0; entryNum < tableInfoPtr->maxNumOfEntries; entryNum++)
    {
#ifdef TBL_DEBUG_SIMULATION
        switch(tableType)
        {
            case PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E:
                for (i = 0; i < tableInfoPtr->entrySize; i++)
                {
                  regAddress = SMEM_CHT_PROT_VLAN_QOS_TBL_ADDR(i, entryNum);
                  rc = prvCpssDxChHwPpReadRegister(devNum, regAddress,
                                                      &directReadValue[i]);
                }
                break;

            case PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E:
                regAddress = SMEM_CHT_VLAN_QOS_TBL_ADDR(entryNum);
                break;

            case PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E:

                trunkId = (entryNum >> 3) & 0x7f;
                trunkMemberIdx = entryNum &  0x7;
                regAddress = SMEM_CHT_TRUNK_TBL_ADDR(trunkMemberIdx, trunkId);
                break;

            case PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E:
                regAddress = SMEM_CHT_STAT_RATE_TBL_ADDR(entryNum);
                break;

            case PRV_CPSS_DXCH_TABLE_CPU_CODE_E:
                regAddress = SMEM_CHT_CPU_CODE_TBL_ADDR(entryNum);

                break;
            case PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E:
                regAddress = SMEM_CHT_PCL_ID_TBL_ADDR(entryNum);
                break;

            case PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E:
                regAddress = SMEM_CHT_QOS_TBL_ADDR(entryNum);
                break;

            case PRV_CPSS_DXCH_TABLE_REMARKING_E:
                regAddress = SMEM_CHT_POLICER_QOS_TBL_ADDR(entryNum);
                break;

            case PRV_CPSS_DXCH_TABLE_STG_E:
                regAddress = SMEM_CHT_STP_TBL_ADDR(entryNum);
                break;

            case PRV_CPSS_DXCH_TABLE_VLAN_E:
                regAddress = SMEM_CHT_VLAN_TBL_ADDR(entryNum);
                break;

            case PRV_CPSS_DXCH_TABLE_MULTICAST_E:
                regAddress = SMEM_CHT_MCST_TBL_ADDR(entryNum);
                break;

            case PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E:
                regAddress = SMEM_CHT_MAC_SA_TBL_ADDR(entryNum);
                break;

            case PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E:
                regAddress = SMEM_CHT_ARP_TBL_ADDR(entryNum);
                break;

            case PRV_CPSS_DXCH_TABLE_FDB_E:
                regAddress = SMEM_CHT_MAC_TBL_ADDR(entryNum);
                break;

            case PRV_CPSS_DXCH_TABLE_POLICER_E:
                regAddress = SMEM_CHT_POLICER_TBL_ADDR(entryNum);
                break;

            case PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E:
                regAddress = SMEM_CHT_POLICER_CNT_TBL_ADDR(entryNum);
                break;

            default:
                return GT_FAIL;

        }

        if (tableType != PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E)
        {
            rc = prvCpssDxChHwPpReadRam(devNum, regAddress,  tableInfoPtr->entrySize,
                                                               directReadValue);
            if(rc != GT_OK)
                return rc;

        }
#endif  /*TBL_DEBUG_SIMULATION*/

        rc = coreCheetahReadTableEntry(devNum,tableType, entryNum, readValue);
        if(rc != GT_OK)
            return rc;

        for(wordNum = 0; wordNum < tableInfoPtr->entrySize; wordNum++)
        {
#ifdef TBL_DEBUG_SIMULATION

            if(readValue[wordNum] != directReadValue[wordNum])
            {
                osPrintf("\n Direct: %d = wordNum",wordNum);
                osPrintf("\n Direct: %d = entrydNum",entryNum);

            }

#endif  /*TBL_DEBUG_SIMULATION*/

            wordValue[wordNum] = osRand() & getMaskBits(tableType);

            if(readValue[wordNum] != wordValue[wordNum])
            {
                osPrintf("\n Indirect %d = wordNum",wordNum);
                osPrintf("\n Indirect %d = entrydNum",entryNum);

                 return GT_FAIL;
            }

        }
    }
    readWriteFieldTest(devNum,tableType);
    return GT_OK;
}
#endif /*CHEETAH_TABLE_DEBUG*/


/*******************************************************************************
* dxChXcatInitTable
*
* DESCRIPTION:
*       init the xCat tables info
* INPUTS:
*
* OUTPUTS:
*       None
*
* RETURNS:
*       NONE
*
* COMMENTS:
*
*
*******************************************************************************/
static void dxChXcatInitTable
(
    void
)
{
    GT_U32 ii;

    /* start by copying the ch3 info */
    for(ii = 0 ; ii < cheetah3TableInfoSize; ii++)
    {
        dxChXcatTablesInfo[ii] = cheetah3TablesInfo[ii];
    }

    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_FDB_E].maxNumOfEntries = _16K;
    dxChXcatTablesInfo[PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_E].maxNumOfEntries = 64;

    /* fill the table for the XCAT A1 devices -- regardless to the rev of current device */

    /* update VLAN Entry table */
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_VLAN_E].entrySize =
        PRV_CPSS_DXCHXCAT_VLAN_ENTRY_WORDS_COUNT_CNS;
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_VLAN_E].readAccessType =
        PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_VLAN_E].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_VLAN_E];
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_VLAN_E].writeAccessType =
        PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_VLAN_E].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_VLAN_E];

    /* Update VRF ID Table */
    dxChXcatTablesInfo[PRV_CPSS_DXCH3_TABLE_VRF_ID_E].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_VRF_ID_E];
    dxChXcatTablesInfo[PRV_CPSS_DXCH3_TABLE_VRF_ID_E].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_VRF_ID_E];

    /* Update Port VLAN QoS table */
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E].entrySize = 3;
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_PORT_VLAN_QOS_E];
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_PORT_VLAN_QOS_E];

    /* Update STG Table */
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_STG_E].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_STG_E];
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_STG_E].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_STG_E];

    /* Update Multicast Table */
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_MULTICAST_E].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_MULTICAST_E];
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_MULTICAST_E].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_MULTICAST_E];

    /* Update HA MAC SA table */
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E].readTablePtr =
        &directAccessXcatTableInfo[PRV_CPSS_XCAT_TABLE_ROUTE_HA_MAC_SA_E];
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E].writeTablePtr =
        &directAccessXcatTableInfo[PRV_CPSS_XCAT_TABLE_ROUTE_HA_MAC_SA_E];

    /* Update HA ARP DA table */
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E].readTablePtr =
        &directAccessXcatTableInfo[PRV_CPSS_XCAT_TABLE_ROUTE_HA_ARP_DA_E];
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E].writeTablePtr =
        &directAccessXcatTableInfo[PRV_CPSS_XCAT_TABLE_ROUTE_HA_ARP_DA_E];

    /* Update Ingress VLAN Translation Table */
    dxChXcatTablesInfo[PRV_CPSS_DXCH3_TABLE_INGRESS_VLAN_TRANSLATION_E].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_INGRESS_TRANSLATION_E];
    dxChXcatTablesInfo[PRV_CPSS_DXCH3_TABLE_INGRESS_VLAN_TRANSLATION_E].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_INGRESS_TRANSLATION_E];

    /* Update Egress VLAN Translation Table */
    dxChXcatTablesInfo[PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_EGRESS_TRANSLATION_E];
    dxChXcatTablesInfo[PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_EGRESS_TRANSLATION_E];

    /* Update LTT and TT Action table */
    dxChXcatTablesInfo[PRV_CPSS_DXCH3_LTT_TT_ACTION_E].entrySize =
        PRV_CPSS_XCAT_LTT_TT_ACTION_WORDS_COUNT_CNS;
    dxChXcatTablesInfo[PRV_CPSS_DXCH3_LTT_TT_ACTION_E].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_LTT_TT_ACTION_E];
    dxChXcatTablesInfo[PRV_CPSS_DXCH3_LTT_TT_ACTION_E].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_LTT_TT_ACTION_E];

    /* Update Tunnel Start table */
    dxChXcatTablesInfo[PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_TUNNEL_START_CFG_E];
    dxChXcatTablesInfo[PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_TUNNEL_START_CFG_E];

    /* PCL tables */

    /* IPCL Configuration table */
    ii = PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E;
    dxChXcatTablesInfo[ii].maxNumOfEntries = 4224;
    dxChXcatTablesInfo[ii].entrySize = 2;
    dxChXcatTablesInfo[ii].readAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].writeAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_IPCL_CFG_E];
    dxChXcatTablesInfo[ii].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_IPCL_CFG_E];

    /* IPCL Lookup1 Configuration table */
    ii = PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_LOOKUP1_CONFIG_E;
    dxChXcatTablesInfo[ii].maxNumOfEntries = 4224;
    dxChXcatTablesInfo[ii].entrySize = 2;
    dxChXcatTablesInfo[ii].readAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].writeAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_IPCL_LOOKUP1_CFG_E];
    dxChXcatTablesInfo[ii].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_IPCL_LOOKUP1_CFG_E];

    /* EPCL Configuration table */
    ii = PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E;
    dxChXcatTablesInfo[ii].maxNumOfEntries = 4160;
    dxChXcatTablesInfo[ii].entrySize = 1;
    dxChXcatTablesInfo[ii].readAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].writeAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_EPCL_CFG_E];
    dxChXcatTablesInfo[ii].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_EPCL_CFG_E];

    /* IPCL User Defined Bytes Configuration table */
    ii = PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_UDB_CONFIG_E;
    dxChXcatTablesInfo[ii].maxNumOfEntries = 8;
    dxChXcatTablesInfo[ii].entrySize = 8;
    dxChXcatTablesInfo[ii].readAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].writeAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_IPCL_UDB_CFG_E];
    dxChXcatTablesInfo[ii].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_IPCL_UDB_CFG_E];

    /* update Port Protocol VID and QoS Configuration table */
    /* - table holds 12 protocol entries per port,
         if (line % 16 >= 12) then the line is reserved.
       - 64 ports are taking in to account.
       - every entry comprise data only for one protocol of specific port.
    */
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E].maxNumOfEntries = _1KB;
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E].entrySize =
        PRV_CPSS_DXCHXCAT_PRTCL_BASED_VLAN_ENTRY_WORDS_COUNT_CNS;
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_VLAN_PORT_PROTOCOL_E];
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_VLAN_PORT_PROTOCOL_E];

    /* Logical Target Mapping Table */
    ii = PRV_CPSS_DXCH_XCAT_TABLE_LOGICAL_TARGET_MAPPING_E;
    dxChXcatTablesInfo[ii].maxNumOfEntries = 512;
    dxChXcatTablesInfo[ii].entrySize = 1;
    dxChXcatTablesInfo[ii].readAccessType = PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].writeAccessType = PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].readTablePtr =
        &xcatA1LogicalTargetMappingTblInfo;
    dxChXcatTablesInfo[ii].writeTablePtr =
        &xcatA1LogicalTargetMappingTblInfo;

    /* Update MAC2ME table */
    dxChXcatTablesInfo[PRV_CPSS_DXCH3_TABLE_MAC2ME_E].readTablePtr =
        &directAccessXcatTableInfo[PRV_CPSS_XCAT_TABLE_MAC2ME_E];
    dxChXcatTablesInfo[PRV_CPSS_DXCH3_TABLE_MAC2ME_E].writeTablePtr =
        &directAccessXcatTableInfo[PRV_CPSS_XCAT_TABLE_MAC2ME_E];

    /* BCN Profiles Table */
    ii = PRV_CPSS_DXCH_XCAT_TABLE_BCN_PROFILES_E;
    dxChXcatTablesInfo[ii].maxNumOfEntries = 8;
    dxChXcatTablesInfo[ii].entrySize = 4;
    dxChXcatTablesInfo[ii].readAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].writeAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_BCN_PROFILE_E];
    dxChXcatTablesInfo[ii].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_BCN_PROFILE_E];

    /* Egress Policer Remarking Table */
    ii = PRV_CPSS_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E;
    dxChXcatTablesInfo[ii].maxNumOfEntries = 80;
    dxChXcatTablesInfo[ii].entrySize = 2;
    dxChXcatTablesInfo[ii].readAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].writeAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E];
    dxChXcatTablesInfo[ii].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E];
}

/*******************************************************************************
* dxChXcat2InitTable
*
* DESCRIPTION:
*       init the xCat2 tables info
* INPUTS:
*
* OUTPUTS:
*       None
*
* RETURNS:
*       NONE
*
* COMMENTS:
*
*
*******************************************************************************/
static void dxChXcat2InitTable
(
    void
)
{
    GT_U32 ii;

    dxChXcatInitTable();

    /* start by copying the xCat info */
    for(ii = 0 ; (ii < dxChXcatTableInfoSize); ii++)
    {
        dxChXcat2TablesInfo[ii] = dxChXcatTablesInfo[ii];
    }

    /* PCL tables */

    /* IPCL lookup01 Configuration table */
    ii = PRV_CPSS_DXCH_XCAT2_TABLE_INGRESS_PCL_LOOKUP01_CONFIG_E;
    dxChXcat2TablesInfo[ii].maxNumOfEntries = 4224;
    dxChXcat2TablesInfo[ii].entrySize = 2;
    dxChXcat2TablesInfo[ii].readAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcat2TablesInfo[ii].writeAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcat2TablesInfo[ii].readTablePtr =
        &directAccessXCAT2TableInfo[PRV_DXCH_XCAT2_TABLE_IPCL_LOOKUP01_CFG_E];
    dxChXcat2TablesInfo[ii].writeTablePtr =
        &directAccessXCAT2TableInfo[PRV_DXCH_XCAT2_TABLE_IPCL_LOOKUP01_CFG_E];

    /* IPCL lookup1 Configuration table */
    ii = PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_LOOKUP1_CONFIG_E;
    dxChXcat2TablesInfo[ii].maxNumOfEntries = 4224;
    dxChXcat2TablesInfo[ii].entrySize = 2;
    dxChXcat2TablesInfo[ii].readAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcat2TablesInfo[ii].writeAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcat2TablesInfo[ii].readTablePtr =
        &directAccessXCAT2TableInfo[PRV_DXCH_XCAT2_TABLE_IPCL_LOOKUP1_CFG_E];
    dxChXcat2TablesInfo[ii].writeTablePtr =
        &directAccessXCAT2TableInfo[PRV_DXCH_XCAT2_TABLE_IPCL_LOOKUP1_CFG_E];

    /* PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E xCat2 - 256 entries, xCat - 1024 */
    ii = PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E;
    dxChXcat2TablesInfo[ii].maxNumOfEntries = 256;

    /*PRV_CPSS_DXCH2_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E not present*/
    ii = PRV_CPSS_DXCH2_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E;
    dxChXcat2TablesInfo[ii].maxNumOfEntries = 0;

    /*PRV_CPSS_DXCH2_TABLE_ROUTE_ACCESS_MATRIX_E not present*/
    ii = PRV_CPSS_DXCH2_TABLE_ROUTE_ACCESS_MATRIX_E;
    dxChXcat2TablesInfo[ii].maxNumOfEntries = 0;

    /*PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E not present*/
    ii = PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E;
    dxChXcat2TablesInfo[ii].maxNumOfEntries = 0;

    /*PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_E not present*/
    ii = PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_E;
    dxChXcat2TablesInfo[ii].maxNumOfEntries = 0;

    /* IPCL lookup1 Configuration table */
    ii = PRV_CPSS_DXCH3_TABLE_MAC2ME_E;
    dxChXcat2TablesInfo[ii].maxNumOfEntries = 8;
    dxChXcat2TablesInfo[ii].entrySize = 4;
    dxChXcat2TablesInfo[ii].readAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcat2TablesInfo[ii].writeAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcat2TablesInfo[ii].readTablePtr =
        &directAccessXCAT2TableInfo[PRV_DXCH_XCAT2_TABLE_MAC2ME_E];
    dxChXcat2TablesInfo[ii].writeTablePtr =
        &directAccessXCAT2TableInfo[PRV_DXCH_XCAT2_TABLE_MAC2ME_E];

    /*PRV_CPSS_DXCH3_TABLE_VRF_ID_E not present*/
    ii = PRV_CPSS_DXCH3_TABLE_VRF_ID_E;
    dxChXcat2TablesInfo[ii].maxNumOfEntries = 0;

    /*PRV_CPSS_DXCH3_LTT_TT_ACTION_E 1024 entries*/
    ii = PRV_CPSS_DXCH3_LTT_TT_ACTION_E;
    dxChXcat2TablesInfo[ii].maxNumOfEntries = 1024;

    /*PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_UDB_CONFIG_E 12 entries*/
    ii = PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_UDB_CONFIG_E;
    dxChXcat2TablesInfo[ii].maxNumOfEntries = 12;

    /*PRV_CPSS_DXCH_TABLE_MULTICAST_E 1024 entries*/
    ii = PRV_CPSS_DXCH_TABLE_MULTICAST_E;
    dxChXcat2TablesInfo[ii].maxNumOfEntries = 1024;

    /* PCL unit - hash mask table (for CRC hashing for trunk) */
    ii = PRV_CPSS_DXCH_LION_TABLE_TRUNK_HASH_MASK_CRC_E;
    dxChXcat2TablesInfo[ii].maxNumOfEntries = 28; /* num entries  */
    dxChXcat2TablesInfo[ii].entrySize = 3;

    /* use LionB info */
    dxChXcat2TablesInfo[ii].readAccessType  =
    dxChXcat2TablesInfo[ii].writeAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcat2TablesInfo[ii].readTablePtr  =
    dxChXcat2TablesInfo[ii].writeTablePtr =
        &(directAccessLionBTableInfoExt[PRV_CPSS_LION_TABLE_TRUNK_HASH_MASK_CRC_E].directAccessInfo);
}

/*******************************************************************************
* lionInitTable
*
* DESCRIPTION:
*       init the Lion tables info
* INPUTS:
*
* OUTPUTS:
*       None
*
* RETURNS:
*       NONE
*
* COMMENTS:
*
*
*******************************************************************************/
static void lionInitTable
(
    void
)
{
    PRV_CPSS_DXCH_TABLES_INFO_STC   *tableInfoPtr;/* pointer to current table info */
    GT_U32 ii;
    GT_U32  index;

    /* init the xcat tables */
    dxChXcatInitTable();

    /* for Lion B */
    /* start by copying the xcat A1 info */
    for(ii = 0 ; ii < dxChXcatTableInfoSize; ii++)
    {
        lionBTablesInfo[ii] = dxChXcatTablesInfo[ii];
    }

    /******************************************/
    /* override it with  Lion B specific info */
    /******************************************/
    for(ii = 0 ; ii < directAccessLionBTableInfoExtNumEntries ; ii++)
    {
        if( directAccessLionBTableInfoExt[ii].directAccessInfo.baseAddress ==
            SPECIAL_ADDRESS_NOT_MODIFIED_E)
        {
            /* no need to modify the entry (same as XCAT A1) */
            continue;
        }

        index = directAccessLionBTableInfoExt[ii].globalIndex;

        lionBTablesInfo[index].readTablePtr =
        lionBTablesInfo[index].writeTablePtr =
            &directAccessLionBTableInfoExt[ii].directAccessInfo;
    }


    /* lion has 32K FDB per port group */
    lionBTablesInfo[PRV_CPSS_DXCH_TABLE_FDB_E].maxNumOfEntries = _32K;

    /***************************/
    /* updated existing tables */
    /***************************/

    /* Update Multicast Table */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_TABLE_MULTICAST_E];
    tableInfoPtr->entrySize = 3;/* 3 words */

    /* IPCL User Defined Bytes Configuration table */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_UDB_CONFIG_E];
    tableInfoPtr->maxNumOfEntries = 12;/* up to UDE 4 */

    /* Tunnel Start and Arp table entry size*/
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E];
    tableInfoPtr->entrySize = 7;

    /******************/
    /* set new tables */
    /******************/

    /* ingress VLAN */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E];
    tableInfoPtr->maxNumOfEntries = _4K;
    tableInfoPtr->entrySize = 5;

    /* egress VLAN */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E];
    tableInfoPtr->maxNumOfEntries = _4K;
    tableInfoPtr->entrySize = 9;

    /* ingress STG */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_STG_INGRESS_E];
    tableInfoPtr->maxNumOfEntries = 256;
    tableInfoPtr->entrySize = 4;

    /* egress STG */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_STG_EGRESS_E];
    tableInfoPtr->maxNumOfEntries = 256;
    tableInfoPtr->entrySize = 4;

    /* port isolation L2 */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L2_E];
    tableInfoPtr->maxNumOfEntries = _2K+128;
    tableInfoPtr->entrySize = 3;

    /* port isolation L3 */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L3_E];
    tableInfoPtr->maxNumOfEntries = _2K+128;
    tableInfoPtr->entrySize = 3;

    /* txq shaper per port per TC */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E];
    tableInfoPtr->maxNumOfEntries = 16;/* local ports num */
    tableInfoPtr->entrySize = 14;

    /* txq shaper per port */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E];
    tableInfoPtr->maxNumOfEntries = 16;/* local ports num */
    tableInfoPtr->entrySize = 2;


    /* txq sourceId members table */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_TXQ_SOURCE_ID_MEMBERS_E];
    tableInfoPtr->maxNumOfEntries = 32;/* source Id groups  */
    tableInfoPtr->entrySize = 2;

    /* txq non-trunk members table */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_TXQ_NON_TRUNK_MEMBERS_E];
    tableInfoPtr->maxNumOfEntries = 128;/* trunkId (include 0)  */
    tableInfoPtr->entrySize = 2;


    /* txq designated members table */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_TXQ_DESIGNATED_PORT_E];
    tableInfoPtr->maxNumOfEntries = 64;/* num entries  */
    tableInfoPtr->entrySize = 2;


    /* TXQ - egress STC table */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E];
    tableInfoPtr->maxNumOfEntries = 16;/* local ports number !!! --
                                          but we will emulate index as 'global port' */
    tableInfoPtr->entrySize = 3;

    /* VLAN UC Rpf mode table */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_ROUTER_VLAN_URPF_STC_E];
    tableInfoPtr->maxNumOfEntries = 256;/* num entries  */
    tableInfoPtr->entrySize = 1;

    /* PCL unit - hash mask table (for CRC hashing for trunk) */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_TRUNK_HASH_MASK_CRC_E];
    tableInfoPtr->maxNumOfEntries = 28; /* num entries  */
    tableInfoPtr->entrySize = 3;

}

/*******************************************************************************
* indexAsPortNumConvert
*
* DESCRIPTION:
*       convert index that is portNum ('Global port num') to portGroupId and
*       'Local port num'
*       for tables that are not with index = portNum , no modification in the
*       index , and portGroupId will be 'first active port group == 0'
*       for non multi-port-groups device : portGroupId is 'all port groups' , no index conversion
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       tableType - the specific table name
*       entryIndex   - index in the table
*
* OUTPUTS:
*       portGroupIdPtr - (pointer to) the portGroupId to use
*       updatedEntryIndexPtr - (pointer to) the updated index
*
* RETURNS:
*       none
*
* COMMENTS:
*
*******************************************************************************/
static void indexAsPortNumConvert
(
    IN GT_U8                   devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN GT_U32                  entryIndex,
    OUT GT_U32                 *portGroupIdPtr,
    OUT GT_U32                 *updatedEntryIndexPtr
)
{
    GT_U32  portNum; /* temp local port number */
    GT_U32  offset;  /* temp offset of port from the base port index */
    GT_U32  newIndex;/* temp index for the port calculation */

    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum) == 0)
    {
        /* no conversion for non-multi port group device */

        *portGroupIdPtr = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
        *updatedEntryIndexPtr = entryIndex;

        return ;
    }

    switch(tableType)
    {
        case PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E:
            /* special case :
                for some devices this is a 'per port' table
                for other devices this is a '16 entries per port' table

                see calculations in function cpssDxChBrgVlanPortProtoVlanQosSet(...)
            */
            if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
            {
                /* the index supposed to be ((portNum * 16) + entryNum)  */
                portNum = entryIndex / 16;/*portNum*/
                offset = entryIndex % 16; /*entryNum*/

                *portGroupIdPtr = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,portNum);
                newIndex = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);
                /* build the new index from the local port and the offset */
                newIndex = newIndex * 16 + offset;

                *updatedEntryIndexPtr = newIndex;
            }
            else
            {
                /* the index supposed to be portNum */
                *portGroupIdPtr = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,entryIndex);
                *updatedEntryIndexPtr = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,entryIndex);
            }
            break;
        case PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E:
        case PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E:
            /* the index supposed to be portNum */
            *portGroupIdPtr = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,entryIndex);
            *updatedEntryIndexPtr = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,entryIndex);
            break;
        case PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E:
        case PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E:
            *portGroupIdPtr = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,entryIndex);
            /* the index supposed to be localPortNum , but the CPU port must be converted to 0xF */
            *updatedEntryIndexPtr = entryIndex & 0xf;
            break;
        default:
            *portGroupIdPtr = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
            *updatedEntryIndexPtr = entryIndex;
            break;
    }

    return;
}


/*******************************************************************************
* prvCpssDxChPortGroupWriteTableEntry
*
* DESCRIPTION:
*       Write a whole entry to the table. - for specific portGroupId
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id , to support multi-port-group devices that need to access
*                specific port group
*       tableType - the specific table name
*       entryIndex   - index in the table
*       entryValuePtr - (pointer to) the data that will be written to the table
*                       may be NULL in the case of indirect table.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*      In the case of entryValuePtr == NULL and indirect table the function
*      just send write entry command to device. And entry is taken from the
*      data registers of a indirect table.
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupWriteTableEntry
(
    IN GT_U8                    devNum,
    IN GT_U32                   portGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT  tableType,
    IN GT_U32                   entryIndex,
    IN GT_U32                  *entryValuePtr
)
{
    PRV_CPSS_DXCH_TABLES_INFO_STC *tableInfoPtr; /* pointer to table info */
    GT_U32      address;  /* address to write the value */
    /* pointer to direct access table information */
    PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC *tablePtr;
    GT_U32      i;
    GT_STATUS   status;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* validity check */
    if((GT_U32)tableType >= PRV_CPSS_DXCH_TABLE_SIZE_GET_MAC(devNum))
        return GT_OUT_OF_RANGE;

    if(portGroupId == CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
    {
        GT_U32  tmpPortGroupId;/* temporary port group id */
        /* this needed for direct calls to prvCpssDxChPortGroupReadTableEntry with
           portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS , and still need convert
           of the entryIndex.
           for example :
           PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E
        */

        /* convert if needed the entry index , get specific portGroupId (or all port groups) */
        indexAsPortNumConvert(devNum,tableType,entryIndex, &tmpPortGroupId , &entryIndex);
    }

    tableInfoPtr = PRV_TABLE_INFO_PTR_GET_MAC(devNum,tableType);

    if(entryIndex >= tableInfoPtr->maxNumOfEntries)
        return GT_OUT_OF_RANGE;


    if(tableInfoPtr->writeAccessType == PRV_CPSS_DXCH_INDIRECT_ACCESS_E)
        /* Write a whole entry to table by indirect access method */
        return indirectWriteTableEntry(devNum, portGroupId,tableType, tableInfoPtr,
                                                     entryIndex, entryValuePtr);
    else
    {
        /* there is no information to write */
        if(entryValuePtr == NULL)
            return GT_FAIL;

        /* Write a whole entry to table by direct access method */
        tablePtr =
            (PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC*)(tableInfoPtr->writeTablePtr);

        if(tablePtr->baseAddress == SPECIAL_ADDRESS_NOT_EXISTS_E)
        {
            /* the table not supported */
            return GT_BAD_STATE;
        }

        address = tablePtr->baseAddress + entryIndex * tablePtr->step;

        if (tablePtr->nextWordOffset == PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS)
        {
            return prvCpssDxChHwPpPortGroupWriteRam(devNum, portGroupId, address, tableInfoPtr->entrySize,
                                                                 entryValuePtr);
        }
        else
        {
            address -= tablePtr->nextWordOffset;

            for (i=0; i<tableInfoPtr->entrySize; i++)
            {
                address += tablePtr->nextWordOffset;

                status =  prvCpssDxChHwPpPortGroupWriteRam(devNum, portGroupId, address, 1, &entryValuePtr[i]);

                if (status != GT_OK)
                {
                    return GT_FAIL;
                }
            }

            return GT_OK;
        }

    }
}

/*******************************************************************************
* prvCpssDxChPortGroupReadTableEntry
*
* DESCRIPTION:
*       Read a whole entry from the table. - for specific portGroupId
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id , to support multi-port-group devices that need to access
*                specific port group
*       tableType - the specific table name
*       entryIndex   - index in the table
*
* OUTPUTS:
*       entryValuePtr - (pointer to) the data read from the table
*                       may be NULL in the case of indirect table.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*      In the case of entryValuePtr == NULL and indirect table the function
*      just send read entry command to device. And entry is stored in the
*      data registers of a indirect table
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupReadTableEntry
(
    IN GT_U8                   devNum,
    IN GT_U32                  portGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN GT_U32                  entryIndex,
    OUT GT_U32                 *entryValuePtr
)
{
    PRV_CPSS_DXCH_TABLES_INFO_STC *tableInfoPtr; /* table info */
    GT_U32      address;  /* address the information is read from */
    /* pointer to direct access table information */
    PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC *tablePtr;
    GT_U32      i;
    GT_STATUS   status;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* validity check */
    if((GT_U32)tableType >=  PRV_CPSS_DXCH_TABLE_SIZE_GET_MAC(devNum))
       return GT_OUT_OF_RANGE;

    if(portGroupId == CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
    {
        GT_U32  tmpPortGroupId;/* temporary port group id */
        /* this needed for direct calls to prvCpssDxChPortGroupReadTableEntry with
           portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS , and still need convert
           of the entryIndex.
           for example :
           PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E
        */

        /* convert if needed the entry index , get specific portGroupId (or all port groups) */
        indexAsPortNumConvert(devNum,tableType,entryIndex, &tmpPortGroupId , &entryIndex);
    }

    tableInfoPtr = PRV_TABLE_INFO_PTR_GET_MAC(devNum,tableType);

    if(entryIndex >= tableInfoPtr->maxNumOfEntries)
        return GT_OUT_OF_RANGE;

    if(tableInfoPtr->readAccessType == PRV_CPSS_DXCH_INDIRECT_ACCESS_E)
    {
        /* Read a whole entry from table by indirect access method */
        return indirectReadTableEntry(devNum, portGroupId, tableType, tableInfoPtr,
                                                     entryIndex,
             (PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC*)(tableInfoPtr->readTablePtr),
                                          entryValuePtr);
    }
    else
    {    /* Read a whole entry from table by direct access method */
        tablePtr =
            (PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC*)(tableInfoPtr->readTablePtr);

        if(tablePtr->baseAddress == SPECIAL_ADDRESS_NOT_EXISTS_E)
        {
            /* the table not supported */
            return GT_BAD_STATE;
        }

        address = tablePtr->baseAddress + entryIndex * tablePtr->step;

        if (tablePtr->nextWordOffset == PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS)
        {
            return prvCpssDxChHwPpPortGroupReadRam(devNum, portGroupId,address, tableInfoPtr->entrySize,
                                                                 entryValuePtr);
        }
        else
        {
            address -= tablePtr->nextWordOffset;

            for (i=0; i<tableInfoPtr->entrySize; i++)
            {
                address += tablePtr->nextWordOffset;

                status =  prvCpssDxChHwPpPortGroupReadRam(devNum, portGroupId,address, 1, &entryValuePtr[i]);

                if (status != GT_OK)
                {
                    return GT_FAIL;
                }
            }

            return GT_OK;
        }

    }
}

/*******************************************************************************
* prvCpssDxChPortGroupReadTableEntryField
*
* DESCRIPTION:
*       Read a field from the table. - for specific portGroupId
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id , to support multi-port-group devices that need to access
*                specific port group
*       tableType - the specific table name
*       entryIndex   - index in the table
*       fieldWordNum - field word number
*                   use PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS
*                   if need global offset in the field of fieldOffset
*       fieldOffset  - field offset
*       fieldLength - field length
*
* OUTPUTS:
*       fieldValuePtr - (pointer to) the data read from the table
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupReadTableEntryField
(
    IN GT_U8                  devNum,
    IN GT_U32                  portGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN GT_U32                 entryIndex,
    IN GT_U32                 fieldWordNum,
    IN GT_U32                 fieldOffset,
    IN GT_U32                 fieldLength,
    OUT GT_U32                *fieldValuePtr
)
{
    GT_U32  entryMemoBufArr[MAX_ENTRY_SIZE_CNS];
    PRV_CPSS_DXCH_TABLES_INFO_STC *tableInfoPtr; /* pointer to table info */
    GT_U32 address; /* address to read from */
    /* pointer to direct table info */
    PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC *tablePtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* validity check */
    if((GT_U32)tableType >= PRV_CPSS_DXCH_TABLE_SIZE_GET_MAC(devNum))
        return GT_OUT_OF_RANGE;

    if(portGroupId == CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
    {
        GT_U32  tmpPortGroupId;/* temporary port group id */
        /* this needed for direct calls to prvCpssDxChPortGroupReadTableEntry with
           portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS , and still need convert
           of the entryIndex.
           for example :
           PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E
        */

        /* convert if needed the entry index , get specific portGroupId (or all port groups) */
        indexAsPortNumConvert(devNum,tableType,entryIndex, &tmpPortGroupId , &entryIndex);
    }

    tableInfoPtr = PRV_TABLE_INFO_PTR_GET_MAC(devNum,tableType);

    if(fieldWordNum == PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS)
    {
        /* the field of fieldOffset is used as 'Global offset' in the entry */
        if((entryIndex >= tableInfoPtr->maxNumOfEntries)    ||
           ((fieldOffset + fieldLength) > (tableInfoPtr->entrySize * 32)) ||
           ((fieldLength == 0) || (fieldLength > 32)))
        {
                return GT_OUT_OF_RANGE;
        }

        /* check if the write is to single word or more */
        if(OFFSET_TO_WORD_MAC(fieldOffset) != OFFSET_TO_WORD_MAC(fieldOffset + fieldLength))
        {
            /* writing to more then single word in the entry */

            if(tableInfoPtr->entrySize > MAX_ENTRY_SIZE_CNS)
            {
                /* need to set MAX_ENTRY_SIZE_CNS with larger value ?! */
                return GT_FAIL;
            }

            return prvCpssDxChPortGroupReadTableEntryFieldList(devNum,portGroupId ,
                tableType,entryIndex,entryMemoBufArr,1,
                &fieldOffset,&fieldLength,fieldValuePtr);
        }

        fieldWordNum = OFFSET_TO_WORD_MAC(fieldOffset);
        fieldOffset  = OFFSET_TO_BIT_MAC(fieldOffset);
    }
    else
    if((entryIndex >= tableInfoPtr->maxNumOfEntries)    ||
       ((fieldOffset + fieldLength) > 32)               ||
       (fieldLength == 0)                               ||
       (fieldWordNum >= tableInfoPtr->entrySize))
            return GT_OUT_OF_RANGE;


    if(tableInfoPtr->readAccessType == PRV_CPSS_DXCH_INDIRECT_ACCESS_E)
        /*Read a field of entry from table by indirect access method. */
        return indirectReadTableEntryField(devNum, portGroupId,tableType, tableInfoPtr,
               entryIndex,fieldWordNum, fieldOffset, fieldLength, fieldValuePtr);
    else
    {
        /*Read a field of entry from table by direct access method. */
        tablePtr =
             (PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC*)(tableInfoPtr->readTablePtr);

        if(tablePtr->baseAddress == SPECIAL_ADDRESS_NOT_EXISTS_E)
        {
            /* the table not supported */
            return GT_BAD_STATE;
        }

        address = tablePtr->baseAddress + entryIndex * tablePtr->step +
                                                               fieldWordNum * tablePtr->nextWordOffset;

        return prvCpssDxChHwPpPortGroupGetRegField(devNum, portGroupId, address,fieldOffset, fieldLength,
                                                                fieldValuePtr);

    }

}

/*******************************************************************************
* prvCpssDxChPortGroupWriteTableEntryField
*
* DESCRIPTION:
*       Write a field to the table.  - for specific portGroupId
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id , to support multi-port-group devices that need to access
*                specific port group
*       tableType - the specific table name
*       entryIndex   - index in the table
*       fieldWordNum - field word number
*                   use PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS
*                   if need global offset in the field of fieldOffset
*       fieldOffset  - field offset
*       fieldLength - field length
*       fieldValue - the data write to the table
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In xCat A1 and above devices the data is updated only when the last
*       word in the entry was written.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupWriteTableEntryField
(
    IN GT_U8                  devNum,
    IN GT_U32                 portGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN GT_U32                 entryIndex,
    IN GT_U32                 fieldWordNum,
    IN GT_U32                 fieldOffset,
    IN GT_U32                 fieldLength,
    IN GT_U32                 fieldValue
)
{
    GT_U32  entryMemoBufArr[MAX_ENTRY_SIZE_CNS];
    GT_STATUS   status = GT_OK;  /* return code */
    PRV_CPSS_DXCH_TABLES_INFO_STC *tableInfoPtr; /* pointer to table info */
    GT_U32 address; /* address to write to */
    /* pointer to direct table info */
    PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC *tablePtr;
    GT_U32      entrySize;  /* table entry size in words */
    GT_U32      data;       /* entry word data */
    GT_U32      mask;       /* mask for new word bits only */
    GT_U32      i;          /* loop index   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* validity check */
    if((GT_U32)tableType >= PRV_CPSS_DXCH_TABLE_SIZE_GET_MAC(devNum))
       return GT_OUT_OF_RANGE;

    if(portGroupId == CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
    {
        GT_U32  tmpPortGroupId;/* temporary port group id */
        /* this needed for direct calls to prvCpssDxChPortGroupReadTableEntry with
           portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS , and still need convert
           of the entryIndex.
           for example :
           PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E
        */

        /* convert if needed the entry index , get specific portGroupId (or all port groups) */
        indexAsPortNumConvert(devNum,tableType,entryIndex, &tmpPortGroupId , &entryIndex);
    }

    tableInfoPtr = PRV_TABLE_INFO_PTR_GET_MAC(devNum,tableType);

    entrySize = tableInfoPtr->entrySize;

    if(fieldWordNum == PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS)
    {
        /* the field of fieldOffset is used as 'Global offset' in the entry */
        if((entryIndex >= tableInfoPtr->maxNumOfEntries)    ||
           ((fieldOffset + fieldLength) > (entrySize * 32)) ||
           ((fieldLength == 0) || (fieldLength > 32)))
        {
                return GT_OUT_OF_RANGE;
        }

        /* check if the write is to single word or more */
        if(OFFSET_TO_WORD_MAC(fieldOffset) != OFFSET_TO_WORD_MAC(fieldOffset + fieldLength))
        {
            /* writing to more then single word in the entry */

            if(entrySize > MAX_ENTRY_SIZE_CNS)
            {
                /* need to set MAX_ENTRY_SIZE_CNS with larger value ?! */
                return GT_FAIL;
            }

            return prvCpssDxChPortGroupWriteTableEntryFieldList(devNum,portGroupId ,
                tableType,entryIndex,entryMemoBufArr,1,
                &fieldOffset,&fieldLength,&fieldValue);
        }

        fieldWordNum = OFFSET_TO_WORD_MAC(fieldOffset);
        fieldOffset  = OFFSET_TO_BIT_MAC(fieldOffset);
    }
    else
    if((entryIndex >= tableInfoPtr->maxNumOfEntries)    ||
       ((fieldOffset + fieldLength) > 32)               ||
       (fieldLength == 0)                               ||
       (fieldWordNum >= entrySize))
    {
            return GT_OUT_OF_RANGE;
    }

    if(tableInfoPtr->writeAccessType == PRV_CPSS_DXCH_INDIRECT_ACCESS_E)
        /* Write a field to table by indirect access method. */
        status = indirectWriteTableEntryField(devNum, portGroupId, tableType, tableInfoPtr,
                 entryIndex,fieldWordNum, fieldOffset, fieldLength, fieldValue);
    else
    {
        /* Write a field to table by direct access method. */
        tablePtr =
             (PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC*)(tableInfoPtr->writeTablePtr);

        if(tablePtr->baseAddress == SPECIAL_ADDRESS_NOT_EXISTS_E)
        {
            /* the table not supported */
            return GT_BAD_STATE;
        }

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* In xCat and above devices the data is updated only when the last */
            /* word in the entry was written. */

            address = tablePtr->baseAddress + entryIndex * tablePtr->step;

            for ( i = 0; i < entrySize; i++)
            {
                status = prvCpssDxChHwPpPortGroupReadRegister(devNum, portGroupId,
                                                        address + i * tablePtr->nextWordOffset,
                                                        &data);
                if (status != GT_OK)
                {
                    return status;
                }

                if(i == fieldWordNum)
                {
                    CALC_MASK_MAC(fieldLength, fieldOffset, mask);

                    /* turn the field off */
                    data &= ~mask;

                    /* insert the new value of field in its place */
                    data |= ((fieldValue << fieldOffset) & mask);
                }

                status = prvCpssDxChHwPpPortGroupWriteRegister(devNum, portGroupId,
                                                         address + i * tablePtr->nextWordOffset,
                                                         data);
                if (status != GT_OK)
                {
                    return status;
                }
            }
        }
        else
        {
            address = tablePtr->baseAddress +
                                entryIndex * tablePtr->step + fieldWordNum * tablePtr->nextWordOffset;

            status =  prvCpssDxChHwPpPortGroupSetRegField(devNum, portGroupId, address,fieldOffset,fieldLength,fieldValue);
        }
    }

    return status;
}

/*******************************************************************************
* prvCpssDxChHwTblAddrStcInfoGet
*
* DESCRIPTION:
*   This function return the address of the tables struct of cheetah devices.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       tblAddrPtrPtr - (pointer to) address of the tables struct
*       sizePtr - (pointer to) size of tables struct
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwTblAddrStcInfoGet
(
    IN  GT_U8     devNum,
    OUT GT_U32    **tblAddrPtrPtr,
    OUT GT_U32    *sizePtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    *tblAddrPtrPtr = (GT_U32 *)PRV_CPSS_DXCH_DEV_TBLS_MAC(devNum);
    *sizePtr = PRV_CPSS_DXCH_TABLE_SIZE_GET_MAC(devNum);

    return GT_OK;
}

